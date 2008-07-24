//=============================================================================
//
//   File : kvi_ircserverdb.cpp
//   Creation date : Mon Jul 10 2000 14:25:00 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2004 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#define __KVILIB__


#include <QApplication>
#include <QLayout>
#include <QMessageBox>
#include <QCheckBox>

#include "kvi_ircserverdb.h"
#include "kvi_config.h"
#include "kvi_locale.h"
#include "kvi_netutils.h"
#include "kvi_nickserv.h"

KviServerDataBaseRecord::KviServerDataBaseRecord(KviNetwork * n)
{
	m_pNetwork = n;
	m_pServerList = new KviPointerList<KviServer>;
	m_pServerList->setAutoDelete(true);
	m_pCurrentServer = 0;
}

KviServerDataBaseRecord::~KviServerDataBaseRecord()
{
	delete m_pNetwork;
	delete m_pServerList;
}

void KviServerDataBaseRecord::insertServer(KviServer *srv)
{
	m_pServerList->append(srv);
}

KviServer * KviServerDataBaseRecord::findServer(const KviServer * pServer, bool bName)
{
	for(KviServer *s=m_pServerList->first();s;s=m_pServerList->next())
	{
		if(bName)
		{
			if(KviQString::equalCI(s->m_szHostname,pServer->m_szHostname)) return s;
		} else {
			if(KviQString::equalCI(s->m_szHostname,pServer->m_szHostname) &&
				(s->m_uPort == pServer->m_uPort) &&
				(s->useSSL() == pServer->useSSL()) &&
				(s->isIPv6() == pServer->isIPv6())) return s;
		}
	}
	return 0;
}

void KviServerDataBaseRecord::setCurrentServer(KviServer *srv)
{
	if(m_pServerList->findRef(srv) != -1)m_pCurrentServer = srv;
}

KviServer * KviServerDataBaseRecord::currentServer()
{
	if(m_pCurrentServer)return m_pCurrentServer;
	m_pCurrentServer = m_pServerList->first();
	return m_pCurrentServer;
}


KviServerDataBase::KviServerDataBase()
{
	m_pRecords = new KviPointerHashTable<QString,KviServerDataBaseRecord>(17,false);
	m_pRecords->setAutoDelete(true);
	m_pAutoConnectOnStartupServers = 0;
	m_pAutoConnectOnStartupNetworks = 0;
}

KviServerDataBase::~KviServerDataBase()
{
	delete m_pRecords;
	if(m_pAutoConnectOnStartupServers)delete m_pAutoConnectOnStartupServers;
	if(m_pAutoConnectOnStartupNetworks)delete m_pAutoConnectOnStartupNetworks;
}

void KviServerDataBase::clearAutoConnectOnStartupServers()
{
	if(!m_pAutoConnectOnStartupServers)return;
	delete m_pAutoConnectOnStartupServers;
	m_pAutoConnectOnStartupServers = 0;
}

void KviServerDataBase::clearAutoConnectOnStartupNetworks()
{
	if(!m_pAutoConnectOnStartupNetworks)return;
	delete m_pAutoConnectOnStartupNetworks;
	m_pAutoConnectOnStartupNetworks = 0;
}

void KviServerDataBase::clear()
{
	m_pRecords->clear();
	m_szCurrentNetwork = "";
}

KviServerDataBaseRecord * KviServerDataBase::insertNetwork(KviNetwork *n)
{
	KviServerDataBaseRecord * r = new KviServerDataBaseRecord(n);
	m_pRecords->replace(n->name(),r);
	return r;
}

KviServerDataBaseRecord * KviServerDataBase::findRecord(const QString &szNetName)
{
	return m_pRecords->find(szNetName);
}


KviNetwork * KviServerDataBase::findNetwork(const QString &szName)
{
	KviServerDataBaseRecord * r = m_pRecords->find(szName);
	if(!r)return 0;
	return r->network();
}

KviServerDataBaseRecord * KviServerDataBase::currentRecord()
{
	KviServerDataBaseRecord * r = 0;
	if(!m_szCurrentNetwork.isEmpty())r = m_pRecords->find(m_szCurrentNetwork);
	if(r)return r;

	KviPointerHashTableIterator<QString,KviServerDataBaseRecord> it(*m_pRecords);
	r = it.current();
	if(!r)return 0;
	m_szCurrentNetwork = r->network()->name();
	return r;
}

void KviServerDataBase::updateServerIp(KviServer * pServer,const QString & ip)
{
	KviPointerHashTableIterator<QString,KviServerDataBaseRecord> it(*m_pRecords);
	while(KviServerDataBaseRecord * r = it.current())
	{
		KviServer * srv = r->findServer(pServer);
		if(srv)
		{
			srv->m_szIp = ip;
			return;
		}
		++it;
	}
}

bool KviServerDataBase::makeCurrentBestServerInNetwork(const QString &szNetName,KviServerDataBaseRecord * r,QString &szError)
{
	m_szCurrentNetwork = szNetName;
	// find a round-robin server in that network
	
	if(r->m_pServerList->isEmpty())
	{
		szError = __tr2qs("The specified network has no server entries");
		return false;
	}

	for(KviServer * s = r->m_pServerList->first();s;s = r->m_pServerList->next())
	{
		if(s->m_szDescription.contains("random",Qt::CaseInsensitive) ||
			(s->m_szDescription.contains("round",Qt::CaseInsensitive) && s->m_szDescription.contains("robin",Qt::CaseInsensitive)))
		{
			r->setCurrentServer(s);
			return true;
		}
	}

	// no explicit round robin... try some common names

	QString tryAlso1,tryAlso2,tryAlso3;

	KviQString::sprintf(tryAlso1,"irc.%Q.org",&szNetName);
	KviQString::sprintf(tryAlso2,"irc.%Q.net",&szNetName);
	KviQString::sprintf(tryAlso3,"irc.%Q.com",&szNetName);

	for(KviServer * ss = r->m_pServerList->first();ss;ss = r->m_pServerList->next())
	{
		if(KviQString::equalCI(ss->m_szHostname,tryAlso1) ||
			KviQString::equalCI(ss->m_szHostname,tryAlso2) ||
			KviQString::equalCI(ss->m_szHostname,tryAlso3))
		{
			r->setCurrentServer(ss);
			return true;
		}
	}

	// a random one in this network
	return true;
}


bool KviServerDataBase::makeCurrentServer(KviServerDefinition * d,QString &szError)
{
	KviServer * pServer = 0;

	KviPointerHashTableIterator<QString,KviServerDataBaseRecord> it(*m_pRecords);
	KviServerDataBaseRecord * r = 0;
	KviServer * srv;

	if(KviQString::equalCIN(d->szServer,"net:",4))
	{
		// net:networkname form
		QString szNet = d->szServer;
		szNet.remove(0,4);
		KviServerDataBaseRecord * r = m_pRecords->find(szNet);
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
		for(srv = r->serverList()->first();srv && (!pServer);srv = r->serverList()->next())
		{
			if(KviQString::equalCI(srv->hostName(),d->szServer))
			{
				if(KviQString::equalCI(srv->id(),d->szId))
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
		if(!d->szNick.isEmpty())pServer->m_szNick = d->szNick;
		if(!d->szPass.isEmpty())pServer->m_szPass = d->szPass; // don't clear the pass!
		if(!d->szInitUMode.isEmpty())pServer->m_szInitUMode = d->szInitUMode;
		
		m_szCurrentNetwork = r->network()->name();
		r->setCurrentServer(pServer);
		return true;
	}

	// no such server: is it a valid ip address or hostname ?
	bool bIsValidIPv4 = KviNetUtils::isValidStringIp(d->szServer);
#ifdef COMPILE_IPV6_SUPPORT
	bool bIsValidIPv6 =KviNetUtils::isValidStringIp_V6(d->szServer);
#else
	bool bIsValidIPv6 = false;
#endif

	if(!(bIsValidIPv4 || bIsValidIPv6))
	{
		// is it a valid hostname ? (must contain at least one dot)
		if(!d->szServer.contains('.'))
		{
			// assume it is a network name!
			KviServerDataBaseRecord * r = m_pRecords->find(d->szServer);
			if(r)return makeCurrentBestServerInNetwork(d->szServer,r,szError);
			// else probably not a network name
		}
	}

	// a valid hostname or ip address , not found in list : add it and make it current

	r = m_pRecords->find(__tr2qs("Standalone Servers"));
	if(!r)
	{
		r = new KviServerDataBaseRecord(new KviNetwork(__tr2qs("Standalone Servers")));
		m_pRecords->replace(r->network()->name(),r);
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
	r->insertServer(s);
	m_szCurrentNetwork = r->network()->name();
	r->setCurrentServer(s);
	
	return true;
}
				
void parseMircServerRecord(QString entry,QString& szNet,
QString& szDescription,QString& szHost,QString& szPort,bool& bSsl,kvi_u32_t& uPort)
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

void KviServerDataBase::loadFromMircIni(const QString & filename, const QString & szMircIni, QStringList& recentServers)
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

				KviServerDataBaseRecord * r = findRecord(szNet);

				if(!r) {
					KviNetwork * n = new KviNetwork(szNet);
					r = insertNetwork(n);
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
			KviNetwork * n = new KviNetwork(it.currentKey());
			KviServerDataBaseRecord * r = insertNetwork(n);
			cfg.setGroup(it.currentKey());
			n->m_szEncoding = cfg.readQStringEntry("Encoding");
			n->m_szTextEncoding = cfg.readQStringEntry("TextEncoding");
			n->m_szDescription = cfg.readQStringEntry("Description");
			n->m_szNickName = cfg.readQStringEntry("NickName");
			n->m_szRealName = cfg.readQStringEntry("RealName");
			n->m_szUserName = cfg.readQStringEntry("UserName");
			n->m_szOnConnectCommand = cfg.readQStringEntry("OnConnectCommand");
			n->m_szOnLoginCommand = cfg.readQStringEntry("OnLoginCommand");
			n->m_pNickServRuleSet = KviNickServRuleSet::load(&cfg,QString::null);
			n->m_bAutoConnect = cfg.readBoolEntry("AutoConnect",false);
			n->m_szUserIdentityId = cfg.readQStringEntry("UserIdentityId");
			if(n->m_bAutoConnect)
			{
				if(!m_pAutoConnectOnStartupNetworks)
				{
					m_pAutoConnectOnStartupNetworks = new KviPointerList<KviServerDataBaseRecord>;
					m_pAutoConnectOnStartupNetworks->setAutoDelete(false);
				}
				m_pAutoConnectOnStartupNetworks->append(r);
			}
			QStringList l = cfg.readStringListEntry("AutoJoinChannels",QStringList());
			if(l.count() > 0)n->setAutoJoinChannelList(new QStringList(l));

			if(cfg.readBoolEntry("Current",false))m_szCurrentNetwork = it.currentKey();

			int nServers = cfg.readIntEntry("NServers",0);
			for(int i=0;i < nServers;i++)
			{
				KviServer *s = new KviServer();
				KviQString::sprintf(tmp,"%d_",i);
				if(s->load(&cfg,tmp))
				{
					r->m_pServerList->append(s);
					KviQString::sprintf(tmp,"%d_Current",i);
					if(cfg.readBoolEntry(tmp,false))r->m_pCurrentServer = s;
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
			if(!r->m_pCurrentServer)r->m_pCurrentServer = r->m_pServerList->first();
		}
		++it;
	}
}

void KviServerDataBase::save(const QString &filename)
{
	KviConfig cfg(filename,KviConfig::Write);

	cfg.clear(); // clear any old entry

	KviPointerHashTableIterator<QString,KviServerDataBaseRecord> it(*m_pRecords);

	QString tmp;

	while(KviServerDataBaseRecord * r = it.current())
	{
		KviNetwork * n = r->network();
		cfg.setGroup(n->m_szName);
		cfg.writeEntry("NServers",r->m_pServerList->count());
		if(n->m_bAutoConnect)
			cfg.writeEntry("AutoConnect",true);
		if(!n->m_szEncoding.isEmpty())
			cfg.writeEntry("Encoding",n->m_szEncoding);
		if(!n->m_szTextEncoding.isEmpty())
			cfg.writeEntry("TextEncoding",n->m_szTextEncoding);
		if(!n->m_szDescription.isEmpty())
			cfg.writeEntry("Description",n->m_szDescription);
		if(!n->m_szNickName.isEmpty())
			cfg.writeEntry("NickName",n->m_szNickName);
		if(!n->m_szRealName.isEmpty())
			cfg.writeEntry("RealName",n->m_szRealName);
		if(!n->m_szUserName.isEmpty())
			cfg.writeEntry("UserName",n->m_szUserName);
		if(!n->m_szOnConnectCommand.isEmpty())
			cfg.writeEntry("OnConnectCommand",n->m_szOnConnectCommand);
		if(!n->m_szOnLoginCommand.isEmpty())
			cfg.writeEntry("OnLoginCommand",n->m_szOnLoginCommand);
		if(n->m_pNickServRuleSet)n->m_pNickServRuleSet->save(&cfg,QString::null);
		if(n->autoJoinChannelList())
			cfg.writeEntry("AutoJoinChannels",*(n->autoJoinChannelList()));
		if(n->m_szName == m_szCurrentNetwork)cfg.writeEntry("Current",true);
		if(!n->m_szUserIdentityId.isEmpty())
			cfg.writeEntry("UserIdentityId",n->m_szUserIdentityId);
		int i=0;
		for(KviServer *s = r->m_pServerList->first();s;s = r->m_pServerList->next())
		{
			KviQString::sprintf(tmp,"%d_",i);
			s->save(&cfg,tmp);

			if(s == r->m_pCurrentServer)
			{
				KviQString::sprintf(tmp,"%d_Current",i);
				cfg.writeEntry(tmp,true);
			}

			i++;
		}
		++it;
	}
}
