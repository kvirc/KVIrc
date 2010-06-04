//=============================================================================
//
//   File : kvi_ircserverdb.cpp
//   Creation date : Mon Jul 10 2000 14:25:00 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================


#include "kvi_ircserverdb.h"
#include "kvi_config.h"
#include "kvi_locale.h"
#include "kvi_netutils.h"
#include "kvi_nickserv.h"

#include <QApplication>
#include <QLayout>
#include <QMessageBox>
#include <QCheckBox>

KviServerDataBase::KviServerDataBase()
{
	m_pRecords = new KviPointerHashTable<QString,KviNetwork>(17,false);
	m_pRecords->setAutoDelete(true);
	m_pAutoConnectOnStartupServers = 0;
	m_pAutoConnectOnStartupNetworks = 0;
}

KviServerDataBase::~KviServerDataBase()
{
	delete m_pRecords;
	if(m_pAutoConnectOnStartupServers)
		delete m_pAutoConnectOnStartupServers;
	if(m_pAutoConnectOnStartupNetworks)
		delete m_pAutoConnectOnStartupNetworks;
}

void KviServerDataBase::clearAutoConnectOnStartupServers()
{
	if(!m_pAutoConnectOnStartupServers)
		return;

	delete m_pAutoConnectOnStartupServers;
	m_pAutoConnectOnStartupServers = 0;
}

void KviServerDataBase::clearAutoConnectOnStartupNetworks()
{
	if(!m_pAutoConnectOnStartupNetworks)
		return;

	delete m_pAutoConnectOnStartupNetworks;
	m_pAutoConnectOnStartupNetworks = 0;
}

void KviServerDataBase::clear()
{
	m_pRecords->clear();
	m_szCurrentNetwork = "";
}

void KviServerDataBase::addNetwork(KviNetwork * n)
{
	m_pRecords->replace(n->name(),n);
}

KviNetwork * KviServerDataBase::findNetwork(const QString & szName)
{
	KviNetwork * r = m_pRecords->find(szName);
	return r;
}

unsigned int KviServerDataBase::networkCount() const
{
	return m_pRecords->count();
}


KviNetwork * KviServerDataBase::currentNetwork()
{
	KviNetwork * r = 0;
	if(!m_szCurrentNetwork.isEmpty())
		r = m_pRecords->find(m_szCurrentNetwork);
	if(r)
		return r;

	return 0;
}

bool KviServerDataBase::makeCurrentBestServerInNetwork(const QString & szNetName, KviNetwork * r, QString & szError)
{
	m_szCurrentNetwork = szNetName;
	// find a round-robin server in that network

	if(r->m_pServerList->isEmpty())
	{
		szError = __tr2qs("The specified network has no server entries");
		return false;
	}

	for(KviServer * s = r->m_pServerList->first(); s; s = r->m_pServerList->next())
	{
		if(s->m_szDescription.contains("random",Qt::CaseInsensitive) ||
			(s->m_szDescription.contains("round",Qt::CaseInsensitive) &&
			 s->m_szDescription.contains("robin",Qt::CaseInsensitive)))
		{
			r->setCurrentServer(s);
			return true;
		}
	}

	// no explicit round robin... try some common names

	QString szTryAlso1, szTryAlso2, szTryAlso3;

	KviQString::sprintf(szTryAlso1,"irc.%Q.org",&szNetName);
	KviQString::sprintf(szTryAlso2,"irc.%Q.net",&szNetName);
	KviQString::sprintf(szTryAlso3,"irc.%Q.com",&szNetName);

	for(KviServer * ss = r->m_pServerList->first(); ss; ss = r->m_pServerList->next())
	{
		if(KviQString::equalCI(ss->m_szHostname,szTryAlso1) ||
			KviQString::equalCI(ss->m_szHostname,szTryAlso2) ||
			KviQString::equalCI(ss->m_szHostname,szTryAlso3))
		{
			r->setCurrentServer(ss);
			return true;
		}
	}

	// a random one in this network
	return true;
}

bool KviServerDataBase::makeCurrentServer(KviServerDefinition * d, QString & szError)
{
	KviServer * pServer = 0;

	KviPointerHashTableIterator<QString,KviNetwork> it(*m_pRecords);
	KviNetwork * r = 0;
	KviServer * srv;

	if(KviQString::equalCIN(d->szServer,"net:",4))
	{
		// net:networkname form
		QString szNet = d->szServer;
		szNet.remove(0,4);
		KviNetwork * r = m_pRecords->find(szNet);
		if(r)return makeCurrentBestServerInNetwork(szNet,r,szError);
		szError = __tr2qs("The server specification seems to be in the net:<string> but the network couln't be found in the database");
		return false;
	}

	if(KviQString::equalCIN(d->szServer,"id:",3))
	{
		// id:serverid form
		QString szId = d->szServer;
		szId.remove(0,3);

		while((r = it.current()))
		{
			for(srv = r->serverList()->first();srv && (!pServer);srv = r->serverList()->next())
			{
				if(KviQString::equalCI(srv->id(),szId))
				{
					pServer = srv;
					goto search_finished;
				}
			}
			++it;
		}
		szError = __tr2qs("The server specification seems to be in the id:<string> form but the identifier coulnd't be found in the database");
		return false;
	}

	it.toFirst();

	while((r = it.current()))
	{
		for(srv = r->serverList()->first(); srv && (!pServer); srv = r->serverList()->next())
		{
			if(KviQString::equalCI(srv->hostName(),d->szServer))
			{
				if(d->szId.isEmpty() || KviQString::equalCI(srv->id(),d->szId))
				{
					if(d->bIPv6 == srv->isIPv6())
					{
						if(d->bSSL == srv->useSSL())
						{
							if(d->bPortIsValid)
							{
								// must match the port
								if(d->uPort == srv->port())
								{
									// port matches
									if(!d->szLinkFilter.isEmpty())
									{
										// must match the link filter
										if(KviQString::equalCI(d->szLinkFilter,srv->linkFilter()))
										{
											// link filter matches
											pServer = srv;
											goto search_finished;
										} // else link filter doesn't match
									} else {
										// no need to match the link filter
										pServer = srv;
										goto search_finished;
									}
								} // else port doesn't match
							} else {
								// no need to match the port
								if(!d->szLinkFilter.isEmpty())
								{
									// must match the link filter
									if(KviQString::equalCI(d->szLinkFilter,srv->linkFilter()))
									{
										// link filter matches
										pServer = srv;
										goto search_finished;
									} // else link filter doesn't match
								} else {
									// no need to match the link filter
									pServer = srv;
									goto search_finished;
								}
							}
						}
					}
				}
			}
		}
		++it;
	}

search_finished:

	if(r && pServer)
	{
		if(!d->szNick.isEmpty())
			pServer->m_szNick = d->szNick;
		if(!d->szPass.isEmpty())
			pServer->m_szPass = d->szPass; // don't clear the pass!
		if(!d->szInitUMode.isEmpty())
			pServer->m_szInitUMode = d->szInitUMode;

		m_szCurrentNetwork = r->name();
		r->setCurrentServer(pServer);
		return true;
	}

	// no such server: is it a valid ip address or hostname ?
	bool bIsValidIPv4 = KviNetUtils::isValidStringIp(d->szServer);
#ifdef COMPILE_IPV6_SUPPORT
	bool bIsValidIPv6 = KviNetUtils::isValidStringIPv6(d->szServer);
#else
	bool bIsValidIPv6 = false;
#endif

	if(!(bIsValidIPv4 || bIsValidIPv6))
	{
		// is it a valid hostname ? (must contain at least one dot)
		if(!d->szServer.contains('.'))
		{
			// assume it is a network name!
			KviNetwork * r = m_pRecords->find(d->szServer);
			if(r)return makeCurrentBestServerInNetwork(d->szServer,r,szError);
			// else probably not a network name
		}
	}

	// a valid hostname or ip address , not found in list : add it and make it current

	r = m_pRecords->find(__tr2qs("Standalone Servers"));
	if(!r)
	{
		r = new KviNetwork(__tr2qs("Standalone Servers"));
		m_pRecords->replace(r->name(),r);
	}

	KviServer * s = new KviServer();
	s->m_szHostname = d->szServer;
	if(bIsValidIPv4)
	{
		s->m_szIp = d->szServer;
		s->setCacheIp(true);
#ifdef COMPILE_IPV6_SUPPORT
	} else {
		if(bIsValidIPv6)
		{
			s->m_szIp = d->szServer;
			s->setCacheIp(true);
			d->bIPv6 = true;
		}
	}
#else
	}
#endif
	s->m_uPort = d->bPortIsValid ? d->uPort : 6667;
	s->setLinkFilter(d->szLinkFilter);
	s->m_szPass= d->szPass;
	s->m_szNick= d->szNick;
	s->m_szInitUMode = d->szInitUMode;
	s->setIPv6(d->bIPv6);
	s->setUseSSL(d->bSSL);
	s->setEnabledSTARTTLS(d->bSTARTTLS);
	r->insertServer(s);
	m_szCurrentNetwork = r->name();
	r->setCurrentServer(s);

	return true;
}

void parseMircServerRecord(QString entry,QString & szNet,
QString & szDescription,QString & szHost,QString & szPort,bool & bSsl,kvi_u32_t & uPort)
{
	bSsl = false;
	int idx = KviQString::find(entry,"SERVER:");
	if(idx != -1)
	{
		szDescription = entry.left(idx);
		szNet=szDescription.section(':',0,0);
		szDescription=szDescription.section(':',1,1);

		entry.remove(0,idx + 7);
		idx = KviQString::find(entry,"GROUP:");
		if(idx != -1)
		{
			szHost = entry.left(idx);
		} else {
			szHost = entry;
		}

		szPort = szHost.section(':',1,1);
		if(szPort[0]=='+')
		{
			bSsl = true;
			szPort.remove(0,1);
		}
		szHost = szHost.section(':',0,0);

		bool bOk;
		uPort = szPort.toUInt(&bOk);
		if(!bOk)uPort = 6667;
	}
}

void KviServerDataBase::importFromMircIni(const QString & filename, const QString & szMircIni, QStringList & recentServers)
{
	clear();
	recentServers.clear();
	QString szDefaultServer;
	KviConfig mircCfg(szMircIni,KviConfig::Read,true);
	if(mircCfg.hasGroup("mirc"))
	{
		mircCfg.setGroup("mirc");
		szDefaultServer = mircCfg.readQStringEntry("host");
	}

	KviConfig cfg(filename,KviConfig::Read,true);
	int i = 0;

	QString entry;
	QString key;
	if(cfg.hasGroup("recent"))
	{
		cfg.setGroup("recent");
		do {
			KviQString::sprintf(key,"n%d",i);
			entry = cfg.readEntry(key);
			if(!entry.isEmpty())
			{
				QString szNet;
				QString szDescription;
				QString szHost;
				QString szPort;
				bool bSsl = false;
				kvi_u32_t uPort = 0;

				parseMircServerRecord(entry,szNet,
					   szDescription,szHost,szPort,bSsl,uPort);

				recentServers << (bSsl ? "ircs://" : "irc://" ) +szHost+":"+szPort;
			}
			i++;
		} while(!entry.isEmpty());
	}

	i = 0;
	if(cfg.hasGroup("servers"))
	{
		cfg.setGroup("servers");
		do {
			KviQString::sprintf(key,"n%d",i);
			entry = cfg.readEntry(key);
			if(!entry.isEmpty())
			{
				bool bDefault = false;
				QString szNet;
				QString szDescription;
				QString szHost;
				QString szPort;
				bool bSsl = false;
				kvi_u32_t uPort = 0;
				// <net>:<description>SERVER:<server:port>GROUP:<group???>
				if(entry==szDefaultServer)
					bDefault = true;

				parseMircServerRecord(entry,szNet,
						   szDescription,szHost,szPort,bSsl,uPort);

				KviNetwork * r = findNetwork(szNet);

				if(!r) {
					r = new KviNetwork(szNet);
					addNetwork(r);
				}

				KviServer *s = new KviServer();
				s->m_szHostname = szHost;
				s->m_szDescription = szDescription;
				s->m_uPort = uPort;


				r->m_pServerList->append(s);
				if(bDefault)
				{
					m_szCurrentNetwork = szNet;
				}
			}
			i++;
		} while(!entry.isEmpty());
	}
}

void KviServerDataBase::load(const QString & filename)
{
	clear();
	KviConfig cfg(filename,KviConfig::Read);

	KviConfigIterator it(*(cfg.dict()));

	QString tmp;

	while(it.current())
	{
		if(it.current()->count() > 0)
		{
			KviNetwork * pNewNet = new KviNetwork(it.currentKey());
			addNetwork(pNewNet);
			cfg.setGroup(it.currentKey());
			pNewNet->m_szEncoding = cfg.readQStringEntry("Encoding");
			pNewNet->m_szTextEncoding = cfg.readQStringEntry("TextEncoding");
			pNewNet->m_szDescription = cfg.readQStringEntry("Description");
			pNewNet->m_szNickName = cfg.readQStringEntry("NickName");
			pNewNet->m_szRealName = cfg.readQStringEntry("RealName");
			pNewNet->m_szUserName = cfg.readQStringEntry("UserName");
			pNewNet->m_szPass = cfg.readQStringEntry("Pass");
			pNewNet->m_szOnConnectCommand = cfg.readQStringEntry("OnConnectCommand");
			pNewNet->m_szOnLoginCommand = cfg.readQStringEntry("OnLoginCommand");
			pNewNet->m_pNickServRuleSet = KviNickServRuleSet::load(&cfg,QString());
			pNewNet->m_bAutoConnect = cfg.readBoolEntry("AutoConnect",false);
			pNewNet->m_szUserIdentityId = cfg.readQStringEntry("UserIdentityId");
			if(pNewNet->m_bAutoConnect)
			{
				if(!m_pAutoConnectOnStartupNetworks)
				{
					m_pAutoConnectOnStartupNetworks = new KviPointerList<KviNetwork>;
					m_pAutoConnectOnStartupNetworks->setAutoDelete(false);
				}
				m_pAutoConnectOnStartupNetworks->append(pNewNet);
			}
			QStringList l = cfg.readStringListEntry("AutoJoinChannels",QStringList());
			if(l.count() > 0)pNewNet->setAutoJoinChannelList(new QStringList(l));

			if(cfg.readBoolEntry("Current",false))m_szCurrentNetwork = it.currentKey();

			int nServers = cfg.readIntEntry("NServers",0);
			for(int i=0;i < nServers;i++)
			{
				KviServer *s = new KviServer();
				KviQString::sprintf(tmp,"%d_",i);
				if(s->load(&cfg,tmp))
				{
					pNewNet->m_pServerList->append(s);
					KviQString::sprintf(tmp,"%d_Current",i);
					if(cfg.readBoolEntry(tmp,false))pNewNet->m_pCurrentServer = s;
					if(s->autoConnect())
					{
						if(!m_pAutoConnectOnStartupServers)
						{
							m_pAutoConnectOnStartupServers = new KviPointerList<KviServer>;
							m_pAutoConnectOnStartupServers->setAutoDelete(false);
						}
						m_pAutoConnectOnStartupServers->append(s);
					}
				} else delete s;
			}
			if(!pNewNet->m_pCurrentServer)pNewNet->m_pCurrentServer = pNewNet->m_pServerList->first();
		}
		++it;
	}
}

void KviServerDataBase::save(const QString &filename)
{
	KviConfig cfg(filename,KviConfig::Write);

	cfg.clear(); // clear any old entry

	KviPointerHashTableIterator<QString,KviNetwork> it(*m_pRecords);

	QString tmp;

	while(KviNetwork * pNetwork = it.current())
	{
		cfg.setGroup(pNetwork->name());
		cfg.writeEntry("NServers",pNetwork->m_pServerList->count());
		if(pNetwork->m_bAutoConnect)
			cfg.writeEntry("AutoConnect",true);
		if(!pNetwork->m_szEncoding.isEmpty())
			cfg.writeEntry("Encoding",pNetwork->m_szEncoding);
		if(!pNetwork->m_szTextEncoding.isEmpty())
			cfg.writeEntry("TextEncoding",pNetwork->m_szTextEncoding);
		if(!pNetwork->m_szDescription.isEmpty())
			cfg.writeEntry("Description",pNetwork->m_szDescription);
		if(!pNetwork->m_szNickName.isEmpty())
			cfg.writeEntry("NickName",pNetwork->m_szNickName);
		if(!pNetwork->m_szRealName.isEmpty())
			cfg.writeEntry("RealName",pNetwork->m_szRealName);
		if(!pNetwork->m_szUserName.isEmpty())
			cfg.writeEntry("UserName",pNetwork->m_szUserName);
		if(!pNetwork->m_szPass.isEmpty())
			cfg.writeEntry("Pass",pNetwork->m_szPass);
		if(!pNetwork->m_szOnConnectCommand.isEmpty())
			cfg.writeEntry("OnConnectCommand",pNetwork->m_szOnConnectCommand);
		if(!pNetwork->m_szOnLoginCommand.isEmpty())
			cfg.writeEntry("OnLoginCommand",pNetwork->m_szOnLoginCommand);
		if(pNetwork->m_pNickServRuleSet)pNetwork->m_pNickServRuleSet->save(&cfg,QString());
		if(pNetwork->autoJoinChannelList())
			cfg.writeEntry("AutoJoinChannels",*(pNetwork->autoJoinChannelList()));
		if(pNetwork->m_szName == m_szCurrentNetwork)cfg.writeEntry("Current",true);
		if(!pNetwork->m_szUserIdentityId.isEmpty())
			cfg.writeEntry("UserIdentityId",pNetwork->m_szUserIdentityId);
		int i=0;
		for(KviServer *s = pNetwork->m_pServerList->first();s;s = pNetwork->m_pServerList->next())
		{
			KviQString::sprintf(tmp,"%d_",i);
			s->save(&cfg,tmp);

			if(s == pNetwork->m_pCurrentServer)
			{
				KviQString::sprintf(tmp,"%d_Current",i);
				cfg.writeEntry(tmp,true);
			}

			i++;
		}
		++it;
	}
}
