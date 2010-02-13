//=============================================================================
//
//   File : kvi_ircserver.cpp
//   Creation date : Mon Jul 10 2000 03:42:59 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2009 Szymon Stefanek (pragma at kvirc dot net)
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


#include "kvi_ircserver.h"
#include "kvi_malloc.h"

#include "kvi_config.h"
#include "kvi_nickserv.h"
#include "kvi_time.h"
#include "kvi_proxydb.h"
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

KviServer::KviServer()
{
	m_pReconnectInfo       = 0;
	m_uFlags               = 0;
	m_uPort                = 6667;
	m_pAutoJoinChannelList = 0;
	m_bAutoConnect         = false;
	m_iProxy               = -1;
}

KviProxy * KviServer::proxyServer(KviProxyDataBase * pDb)
{
	int i = 0;
	if(proxy() < 0) return 0;
	KviPointerList<KviProxy> * proxylist = pDb->proxyList();
	for(KviProxy * p = proxylist->first(); p; p = proxylist->next())
	{
		if(i == proxy()) return p;
		i++;
	}
	return 0;
}

KviServer::KviServer(const KviServer & serv)
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
	else m_pAutoJoinChannelList = 0;

	if(serv.m_pReconnectInfo)
		m_pReconnectInfo = new KviServerReconnectInfo(*(serv.m_pReconnectInfo));
	else m_pReconnectInfo = 0;
}

void KviServer::operator=(const KviServer & serv)
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

	if(m_pAutoJoinChannelList) delete m_pAutoJoinChannelList;
	if(serv.m_pAutoJoinChannelList)
		m_pAutoJoinChannelList = new QStringList(*(serv.m_pAutoJoinChannelList));
	else m_pAutoJoinChannelList = 0;

	if(m_pReconnectInfo) delete m_pReconnectInfo;
	if(serv.m_pReconnectInfo)
		m_pReconnectInfo = new KviServerReconnectInfo(*(serv.m_pReconnectInfo));
	else m_pReconnectInfo = 0;
}

KviServer::~KviServer()
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

void KviServer::generateUniqueId()
{
	struct timeval tv;
	kvi_gettimeofday(&tv,0);
	KviQString::sprintf(m_szId,"myserver%d%d%d",tv.tv_usec,rand() % 1000,rand() % 1000);
}

QString KviServer::ircUri()
{
	QString szUri("irc");
	if(useSSL()) szUri += "s";
	if(isIPv6()) szUri += "6";
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

void KviServer::setAutoJoinChannelList(QStringList * pNewChannelList)
{
	if(m_pAutoJoinChannelList) delete m_pAutoJoinChannelList;
	m_pAutoJoinChannelList = pNewChannelList;
}

bool KviServer::load(KviConfig * cfg, const QString & szPrefix)
{
	QString szTmp;
	KviQString::sprintf(szTmp,"%QHostname",&szPrefix);
	m_szHostname = cfg->readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QIp",&szPrefix);
	m_szIp = cfg->readQStringEntry(szTmp);
	if(m_szHostname.isEmpty() && m_szIp.isEmpty())	return false;

	KviQString::sprintf(szTmp,"%QDescription",&szPrefix);
	m_szDescription = cfg->readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QUser",&szPrefix);
	m_szUser = cfg->readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QPass",&szPrefix);
	m_szPass = cfg->readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QNick",&szPrefix);
	m_szNick = cfg->readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QSaslPass",&szPrefix);
	m_szSaslPass = cfg->readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QSaslNick",&szPrefix);
	m_szSaslNick = cfg->readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QRealName",&szPrefix);
	m_szRealName = cfg->readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QInitUmode",&szPrefix);
	m_szInitUMode = cfg->readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QAutoJoinChannels",&szPrefix);
	QStringList l = cfg->readStringListEntry(szTmp,QStringList());
	if(l.count() > 0) setAutoJoinChannelList(new QStringList(l));

	KviQString::sprintf(szTmp,"%QAutoConnect",&szPrefix);
	m_bAutoConnect = cfg->readBoolEntry(szTmp,false);
	KviQString::sprintf(szTmp,"%QEncoding",&szPrefix);
	m_szEncoding = cfg->readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QTextEncoding",&szPrefix);
	m_szTextEncoding = cfg->readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QOnConnectCommand",&szPrefix);
	m_szOnConnectCommand = cfg->readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QOnLoginCommand",&szPrefix);
	m_szOnLoginCommand = cfg->readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QLinkFilter",&szPrefix);
	m_szLinkFilter = cfg->readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QPort",&szPrefix);
	m_uPort = cfg->readUIntEntry(szTmp,6667);
	KviQString::sprintf(szTmp,"%QId",&szPrefix);
	m_szId = cfg->readQStringEntry(szTmp);
	if(m_szId.isEmpty()) generateUniqueId();

	KviQString::sprintf(szTmp,"%QIPv6",&szPrefix);
	setIPv6(cfg->readBoolEntry(szTmp,false));
	KviQString::sprintf(szTmp,"%QCacheIp",&szPrefix);
	setCacheIp(cfg->readBoolEntry(szTmp,false)); // true ?
	KviQString::sprintf(szTmp,"%QSSL",&szPrefix);
	setUseSSL(cfg->readBoolEntry(szTmp,false));
	KviQString::sprintf(szTmp,"%QEnabledCAP",&szPrefix);
	setEnabledCAP(cfg->readBoolEntry(szTmp,true));
	KviQString::sprintf(szTmp,"%QEnabledSTARTTLS",&szPrefix);
	setEnabledSTARTTLS(cfg->readBoolEntry(szTmp,true));
	KviQString::sprintf(szTmp,"%QEnabledSASL",&szPrefix);
	setEnabledSASL(cfg->readBoolEntry(szTmp,false));
	KviQString::sprintf(szTmp,"%QProxy",&szPrefix);
	setProxy(cfg->readIntEntry(szTmp,-2));
	KviQString::sprintf(szTmp,"%QUserIdentityId",&szPrefix);
	m_szUserIdentityId = cfg->readQStringEntry(szTmp);
	return true;
}

void KviServer::save(KviConfig * cfg, const QString & szPrefix)
{
	QString szTmp;
	KviQString::sprintf(szTmp,"%QHostname",&szPrefix);
	cfg->writeEntry(szTmp,m_szHostname);
	KviQString::sprintf(szTmp,"%QId",&szPrefix);
	cfg->writeEntry(szTmp,m_szId);
	if(!m_szIp.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QIp",&szPrefix);
		cfg->writeEntry(szTmp,m_szIp);
	}
	if(!m_szDescription.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QDescription",&szPrefix);
		cfg->writeEntry(szTmp,m_szDescription);
	}
	if(!m_szUser.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QUser",&szPrefix);
		cfg->writeEntry(szTmp,m_szUser);
	}
	if(!m_szPass.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QPass",&szPrefix);
		cfg->writeEntry(szTmp,m_szPass);
	}
	if(!m_szNick.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QNick",&szPrefix);
		cfg->writeEntry(szTmp,m_szNick);
	}
	if(!m_szSaslPass.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QSaslPass",&szPrefix);
		cfg->writeEntry(szTmp,m_szSaslPass);
	}
	if(!m_szSaslNick.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QSaslNick",&szPrefix);
		cfg->writeEntry(szTmp,m_szSaslNick);
	}
	if(!m_szRealName.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QRealName",&szPrefix);
		cfg->writeEntry(szTmp,m_szRealName);
	}
	if(!m_szInitUMode.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QInitUMode",&szPrefix);
		cfg->writeEntry(szTmp,m_szInitUMode);
	}
	if(autoJoinChannelList())
	{
		KviQString::sprintf(szTmp,"%QAutoJoinChannels",&szPrefix);
		cfg->writeEntry(szTmp,*(autoJoinChannelList()));
	}
	if(autoConnect()) // otherwise it defaults to false anyway
	{
		KviQString::sprintf(szTmp,"%QAutoConnect",&szPrefix);
		cfg->writeEntry(szTmp,autoConnect());
	}
	if(!m_szEncoding.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QEncoding",&szPrefix);
		cfg->writeEntry(szTmp,m_szEncoding);
	}
	if(!m_szTextEncoding.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QTextEncoding",&szPrefix);
		cfg->writeEntry(szTmp,m_szTextEncoding);
	}
	if(!m_szOnConnectCommand.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QOnConnectCommand",&szPrefix);
		cfg->writeEntry(szTmp,m_szOnConnectCommand);
	}
	if(!m_szOnLoginCommand.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QOnLoginCommand",&szPrefix);
		cfg->writeEntry(szTmp,m_szOnLoginCommand);
	}
	if(!m_szLinkFilter.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QLinkFilter",&szPrefix);
		cfg->writeEntry(szTmp,m_szLinkFilter);
	}
	if(m_uPort != 6667)
	{
		KviQString::sprintf(szTmp,"%QPort",&szPrefix);
		cfg->writeEntry(szTmp,m_uPort);
	}
	if(isIPv6())
	{
		KviQString::sprintf(szTmp,"%QIPv6",&szPrefix);
		cfg->writeEntry(szTmp,isIPv6());
	}
	if(cacheIp())
	{
		KviQString::sprintf(szTmp,"%QCacheIp",&szPrefix);
		cfg->writeEntry(szTmp,cacheIp());
	}
	if(useSSL())
	{
		KviQString::sprintf(szTmp,"%QSSL",&szPrefix);
		cfg->writeEntry(szTmp,useSSL());
	}
	if(!enabledCAP())
	{
		KviQString::sprintf(szTmp,"%QEnabledCAP",&szPrefix);
		cfg->writeEntry(szTmp,enabledCAP());
	}
	if(!enabledSTARTTLS())
	{
		KviQString::sprintf(szTmp,"%QEnabledSTARTTLS",&szPrefix);
		cfg->writeEntry(szTmp,enabledSTARTTLS());
	}
	if(enabledSASL())
	{
		KviQString::sprintf(szTmp,"%QEnabledSASL",&szPrefix);
		cfg->writeEntry(szTmp,enabledSASL());
	}
	if(proxy()!=-2)
	{
		KviQString::sprintf(szTmp,"%QProxy",&szPrefix);
		cfg->writeEntry(szTmp,proxy());
	}
	if(!m_szUserIdentityId.isEmpty())
	{
		KviQString::sprintf(szTmp,"%QUserIdentityId",&szPrefix);
		cfg->writeEntry(szTmp,m_szUserIdentityId);
	}
}


KviServerReconnectInfo::KviServerReconnectInfo()
{
	m_bIsAway = false;
}

KviServerReconnectInfo::~KviServerReconnectInfo()
{
}

KviServerReconnectInfo::KviServerReconnectInfo(const KviServerReconnectInfo & info)
{
	m_szNick         = info.m_szNick;
	m_szAwayReason   = info.m_szAwayReason;
	m_szJoinChannels = info.m_szJoinChannels;
	m_szOpenQueryes  = info.m_szOpenQueryes;
	m_bIsAway        = info.m_bIsAway;
}
