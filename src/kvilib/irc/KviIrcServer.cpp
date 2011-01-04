//=============================================================================
//
//   File : KviIrcServer.cpp
//   Creation date : Mon Jul 10 2000 03:42:59 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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


#include "KviIrcServer.h"
#include "KviIrcServerReconnectInfo.h"
#include "KviMemory.h"
#include "KviConfigurationFile.h"
#include "KviNickServRuleSet.h"
#include "KviTimeUtils.h"
#include "KviProxyDataBase.h"

#include <stdlib.h>

// This is not allowed on windows unless we force the symbol to be undefined
// It works on linux since gcc allows undefined symbols by default
// but it is also "theoretically" wrong:
//   kvilib is not linked to kvirc: it's kvirc being linked to kvilib
//   thus kvilib should not depend on symbols defined in the kvirc core.
// We must find another way to do that (like having just the id and finding
// the proxy in the kvirc core, or just passing the pointer to the db from outside).
//      Pragma

//extern KVIRC_API KviProxyDataBase * g_pProxyDataBase;

KviIrcServer::KviIrcServer()
{
	m_pReconnectInfo       = 0;
	m_uFlags               = 0;
	m_uPort                = 6667;
	m_pAutoJoinChannelList = 0;
	m_bAutoConnect         = false;
	m_iProxy               = -1;
}

KviProxy * KviIrcServer::proxyServer(KviProxyDataBase * pDb)
{
	int i = 0;
	if(proxy() < 0)
		return 0;
	KviPointerList<KviProxy> * proxylist = pDb->proxyList();
	for(KviProxy * pProxy = proxylist->first(); pProxy; pProxy = proxylist->next())
	{
		if(i == proxy())
			return pProxy;
		i++;
	}
	return 0;
}

KviIrcServer::KviIrcServer(const KviIrcServer & serv)
{
	m_szHostname         = serv.m_szHostname;
	m_szIp               = serv.m_szIp;
	m_szDescription      = serv.m_szDescription;
	m_szUser             = serv.m_szUser;
	m_szPass             = serv.m_szPass;
	m_uPort              = serv.m_uPort;
	m_szNick             = serv.m_szNick;
	m_szRealName         = serv.m_szRealName;
	m_szEncoding         = serv.m_szEncoding;
	m_szTextEncoding     = serv.m_szTextEncoding;
	m_uFlags             = serv.m_uFlags;
	m_szInitUMode        = serv.m_szInitUMode;
	m_szOnConnectCommand = serv.m_szOnConnectCommand;
	m_szOnLoginCommand   = serv.m_szOnLoginCommand;
	m_szLinkFilter       = serv.m_szLinkFilter;
	m_szId               = serv.m_szId;
	m_iProxy             = serv.m_iProxy;
	m_szUserIdentityId   = serv.m_szUserIdentityId;
	m_bAutoConnect       = serv.m_bAutoConnect;
	m_szSaslNick         = serv.m_szSaslNick;
	m_szSaslPass         = serv.m_szSaslPass;

	if(serv.m_pAutoJoinChannelList)
		m_pAutoJoinChannelList = new QStringList(*(serv.m_pAutoJoinChannelList));
	else
		m_pAutoJoinChannelList = 0;

	if(serv.m_pReconnectInfo)
		m_pReconnectInfo = new KviIrcServerReconnectInfo(*(serv.m_pReconnectInfo));
	else
		m_pReconnectInfo = 0;
}

void KviIrcServer::operator=(const KviIrcServer & serv)
{
	m_szHostname         = serv.m_szHostname;
	m_szIp               = serv.m_szIp;
	m_szDescription      = serv.m_szDescription;
	m_szUser             = serv.m_szUser;
	m_szPass             = serv.m_szPass;
	m_uPort              = serv.m_uPort;
	m_szNick             = serv.m_szNick;
	m_szRealName         = serv.m_szRealName;
	m_szEncoding         = serv.m_szEncoding;
	m_szTextEncoding     = serv.m_szTextEncoding;
	m_uFlags             = serv.m_uFlags;
	m_szInitUMode        = serv.m_szInitUMode;
	m_szOnConnectCommand = serv.m_szOnConnectCommand;
	m_szOnLoginCommand   = serv.m_szOnLoginCommand;
	m_szLinkFilter       = serv.m_szLinkFilter;
	m_szId               = serv.m_szId;
	m_szUserIdentityId   = serv.m_szUserIdentityId;
	m_iProxy	     = serv.m_iProxy;
	m_bAutoConnect       = serv.m_bAutoConnect;
	m_szSaslNick         = serv.m_szSaslNick;
	m_szSaslPass         = serv.m_szSaslPass;

	if(m_pAutoJoinChannelList)
		delete m_pAutoJoinChannelList;
	if(serv.m_pAutoJoinChannelList)
		m_pAutoJoinChannelList = new QStringList(*(serv.m_pAutoJoinChannelList));
	else
		m_pAutoJoinChannelList = 0;

	if(m_pReconnectInfo)
		delete m_pReconnectInfo;
	if(serv.m_pReconnectInfo)
		m_pReconnectInfo = new KviIrcServerReconnectInfo(*(serv.m_pReconnectInfo));
	else
		m_pReconnectInfo = 0;
}

KviIrcServer::~KviIrcServer()
{
	if(m_pAutoJoinChannelList)
	{
		delete m_pAutoJoinChannelList;
		m_pAutoJoinChannelList = 0;
	}
	if(m_pReconnectInfo)
	{
		delete m_pReconnectInfo;
		m_pReconnectInfo = 0;
	}
}

void KviIrcServer::generateUniqueId()
{
	struct timeval tv;
	kvi_gettimeofday(&tv,0);
	KviQString::sprintf(m_szId,"myserver%d%d%d",tv.tv_usec,rand() % 1000,rand() % 1000);
}

QString KviIrcServer::ircUri()
{
	QString szUri("irc");
	if(useSSL())
		szUri += "s";
	if(isIPv6())
		szUri += "6";
	szUri += "://";
	szUri += m_szHostname;

	if(m_uPort != 6667)
	{
		szUri += ":";
		QString szNum;
		szNum.setNum(m_uPort);
		szUri += szNum;
	}
	return szUri;
}

void KviIrcServer::setAutoJoinChannelList(QStringList * pNewChannelList)
{
	if(m_pAutoJoinChannelList)
		delete m_pAutoJoinChannelList;
	m_pAutoJoinChannelList = pNewChannelList;
}

void KviIrcServer::setAutoJoinChannelList(const QString & szNewChannelList)
{
	if(m_pAutoJoinChannelList)
		delete m_pAutoJoinChannelList;
	QStringList lChans = szNewChannelList.split(",");
	if(lChans.isEmpty())
		m_pAutoJoinChannelList = NULL;
	else
		m_pAutoJoinChannelList = new QStringList(lChans);
}

bool KviIrcServer::load(KviConfigurationFile * pCfg, const QString & szPrefix)
{
	QString szTmp;
	KviQString::sprintf(szTmp,"%QHostname",&szPrefix);
	m_szHostname = pCfg->readEntry(szTmp);
	KviQString::sprintf(szTmp,"%QIp",&szPrefix);
	m_szIp = pCfg->readEntry(szTmp);
	if(m_szHostname.isEmpty() && m_szIp.isEmpty())
		return false;

	KviQString::sprintf(szTmp,"%QDescription",&szPrefix);
	m_szDescription = pCfg->readEntry(szTmp);
	KviQString::sprintf(szTmp,"%QUser",&szPrefix);
	m_szUser = pCfg->readEntry(szTmp);
	KviQString::sprintf(szTmp,"%QPass",&szPrefix);
	m_szPass = pCfg->readEntry(szTmp);
	KviQString::sprintf(szTmp,"%QNick",&szPrefix);
	m_szNick = pCfg->readEntry(szTmp);
	KviQString::sprintf(szTmp,"%QSaslPass",&szPrefix);
	m_szSaslPass = pCfg->readEntry(szTmp);
	KviQString::sprintf(szTmp,"%QSaslNick",&szPrefix);
	m_szSaslNick = pCfg->readEntry(szTmp);
	KviQString::sprintf(szTmp,"%QRealName",&szPrefix);
	m_szRealName = pCfg->readEntry(szTmp);
	KviQString::sprintf(szTmp,"%QInitUmode",&szPrefix);
	m_szInitUMode = pCfg->readEntry(szTmp);
	KviQString::sprintf(szTmp,"%QAutoJoinChannels",&szPrefix);
	QStringList l = pCfg->readStringListEntry(szTmp,QStringList());
	if(l.count() > 0)
		setAutoJoinChannelList(new QStringList(l));

	KviQString::sprintf(szTmp,"%QAutoConnect",&szPrefix);
	m_bAutoConnect = pCfg->readBoolEntry(szTmp,false);
	KviQString::sprintf(szTmp,"%QEncoding",&szPrefix);
	m_szEncoding = pCfg->readEntry(szTmp);
	KviQString::sprintf(szTmp,"%QTextEncoding",&szPrefix);
	m_szTextEncoding = pCfg->readEntry(szTmp);
	KviQString::sprintf(szTmp,"%QOnConnectCommand",&szPrefix);
	m_szOnConnectCommand = pCfg->readEntry(szTmp);
	KviQString::sprintf(szTmp,"%QOnLoginCommand",&szPrefix);
	m_szOnLoginCommand = pCfg->readEntry(szTmp);
	KviQString::sprintf(szTmp,"%QLinkFilter",&szPrefix);
	m_szLinkFilter = pCfg->readEntry(szTmp);
	KviQString::sprintf(szTmp,"%QPort",&szPrefix);
	m_uPort = pCfg->readUIntEntry(szTmp,6667);
	KviQString::sprintf(szTmp,"%QId",&szPrefix);
	m_szId = pCfg->readEntry(szTmp);
	if(m_szId.isEmpty())
		generateUniqueId();

	KviQString::sprintf(szTmp,"%QIPv6",&szPrefix);
	setIPv6(pCfg->readBoolEntry(szTmp,false));
	KviQString::sprintf(szTmp,"%QCacheIp",&szPrefix);
	setCacheIp(pCfg->readBoolEntry(szTmp,false)); // true ?
	KviQString::sprintf(szTmp,"%QSSL",&szPrefix);
	setUseSSL(pCfg->readBoolEntry(szTmp,false));
	KviQString::sprintf(szTmp,"%QEnabledCAP",&szPrefix);
	setEnabledCAP(pCfg->readBoolEntry(szTmp,true));
	KviQString::sprintf(szTmp,"%QEnabledSTARTTLS",&szPrefix);
	setEnabledSTARTTLS(pCfg->readBoolEntry(szTmp,true));
	KviQString::sprintf(szTmp,"%QEnabledSASL",&szPrefix);
	setEnabledSASL(pCfg->readBoolEntry(szTmp,false));
	KviQString::sprintf(szTmp,"%QProxy",&szPrefix);
	setProxy(pCfg->readIntEntry(szTmp,-2));
	KviQString::sprintf(szTmp,"%QUserIdentityId",&szPrefix);
	m_szUserIdentityId = pCfg->readEntry(szTmp);
	return true;
}

void KviIrcServer::save(KviConfigurationFile * pCfg, const QString & szPrefix)
{
	QString szTmp;
	KviQString::sprintf(szTmp,"%QHostname",&szPrefix);
	pCfg->writeEntry(szTmp,m_szHostname);
	KviQString::sprintf(szTmp,"%QId",&szPrefix);
	pCfg->writeEntry(szTmp,m_szId);
	if(!m_szIp.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QIp",&szPrefix);
		pCfg->writeEntry(szTmp,m_szIp);
	}
	if(!m_szDescription.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QDescription",&szPrefix);
		pCfg->writeEntry(szTmp,m_szDescription);
	}
	if(!m_szUser.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QUser",&szPrefix);
		pCfg->writeEntry(szTmp,m_szUser);
	}
	if(!m_szPass.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QPass",&szPrefix);
		pCfg->writeEntry(szTmp,m_szPass);
	}
	if(!m_szNick.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QNick",&szPrefix);
		pCfg->writeEntry(szTmp,m_szNick);
	}
	if(!m_szSaslPass.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QSaslPass",&szPrefix);
		pCfg->writeEntry(szTmp,m_szSaslPass);
	}
	if(!m_szSaslNick.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QSaslNick",&szPrefix);
		pCfg->writeEntry(szTmp,m_szSaslNick);
	}
	if(!m_szRealName.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QRealName",&szPrefix);
		pCfg->writeEntry(szTmp,m_szRealName);
	}
	if(!m_szInitUMode.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QInitUMode",&szPrefix);
		pCfg->writeEntry(szTmp,m_szInitUMode);
	}
	if(autoJoinChannelList())
	{
		KviQString::sprintf(szTmp,"%QAutoJoinChannels",&szPrefix);
		pCfg->writeEntry(szTmp,*(autoJoinChannelList()));
	}
	if(autoConnect()) // otherwise it defaults to false anyway
	{
		KviQString::sprintf(szTmp,"%QAutoConnect",&szPrefix);
		pCfg->writeEntry(szTmp,autoConnect());
	}
	if(!m_szEncoding.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QEncoding",&szPrefix);
		pCfg->writeEntry(szTmp,m_szEncoding);
	}
	if(!m_szTextEncoding.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QTextEncoding",&szPrefix);
		pCfg->writeEntry(szTmp,m_szTextEncoding);
	}
	if(!m_szOnConnectCommand.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QOnConnectCommand",&szPrefix);
		pCfg->writeEntry(szTmp,m_szOnConnectCommand);
	}
	if(!m_szOnLoginCommand.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QOnLoginCommand",&szPrefix);
		pCfg->writeEntry(szTmp,m_szOnLoginCommand);
	}
	if(!m_szLinkFilter.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QLinkFilter",&szPrefix);
		pCfg->writeEntry(szTmp,m_szLinkFilter);
	}
	if(m_uPort != 6667)
	{
		KviQString::sprintf(szTmp,"%QPort",&szPrefix);
		pCfg->writeEntry(szTmp,m_uPort);
	}
	if(isIPv6())
	{
		KviQString::sprintf(szTmp,"%QIPv6",&szPrefix);
		pCfg->writeEntry(szTmp,isIPv6());
	}
	if(cacheIp())
	{
		KviQString::sprintf(szTmp,"%QCacheIp",&szPrefix);
		pCfg->writeEntry(szTmp,cacheIp());
	}
	if(useSSL())
	{
		KviQString::sprintf(szTmp,"%QSSL",&szPrefix);
		pCfg->writeEntry(szTmp,useSSL());
	}
	if(!enabledCAP())
	{
		KviQString::sprintf(szTmp,"%QEnabledCAP",&szPrefix);
		pCfg->writeEntry(szTmp,enabledCAP());
	}
	if(!enabledSTARTTLS())
	{
		KviQString::sprintf(szTmp,"%QEnabledSTARTTLS",&szPrefix);
		pCfg->writeEntry(szTmp,enabledSTARTTLS());
	}
	if(enabledSASL())
	{
		KviQString::sprintf(szTmp,"%QEnabledSASL",&szPrefix);
		pCfg->writeEntry(szTmp,enabledSASL());
	}
	if(proxy()!=-2)
	{
		KviQString::sprintf(szTmp,"%QProxy",&szPrefix);
		pCfg->writeEntry(szTmp,proxy());
	}
	if(!m_szUserIdentityId.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QUserIdentityId",&szPrefix);
		pCfg->writeEntry(szTmp,m_szUserIdentityId);
	}
}
