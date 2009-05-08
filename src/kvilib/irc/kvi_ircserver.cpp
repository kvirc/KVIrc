//=============================================================================
//
//   File : kvi_ircserver.cpp
//   Creation date : Mon Jul 10 2000 03:42:59 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2004 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#define __KVILIB__

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

// FIXME: This should be renamed to KviServer or sth like that
KviIrcServer::KviIrcServer()
{
	m_pReconnectInfo=0;
	m_uFlags = 0;
	m_uPort  = 6667;
	m_pChannelList = 0;
	m_bAutoConnect = false;
	m_iProxy = -1;
}

KviProxy* KviIrcServer::proxyServer(KviProxyDataBase * pDb)
{
	int i=0;
	if(proxy()<0) return 0;
	KviPointerList<KviProxy> * proxylist = pDb->proxyList();
	for(KviProxy * p = proxylist->first();p;p = proxylist->next())
	{
		if(i==proxy()) return p;
		i++;
	}
	return 0;
}

KviIrcServer::KviIrcServer(const KviIrcServer &serv)
{
	if(serv.m_pReconnectInfo) m_pReconnectInfo = new KviIrcServerReconnectInfo(*(serv.m_pReconnectInfo));
	else m_pReconnectInfo = 0;
	m_szHostname         = serv.m_szHostname;
	m_szIp               = serv.m_szIp;
	m_szDescription      = serv.m_szDescription;
	m_szUser             = serv.m_szUser;
	m_szPass             = serv.m_szPass;
	m_uPort              = serv.m_uPort;
	m_szNick             = serv.m_szNick;
	m_szRealName         = serv.m_szRealName;
	m_szEncoding         = serv.m_szEncoding;
	m_uFlags             = serv.m_uFlags;
	m_szInitUMode        = serv.m_szInitUMode;
	m_szOnConnectCommand = serv.m_szOnConnectCommand;
	m_szOnLoginCommand   = serv.m_szOnLoginCommand;
	m_szLinkFilter       = serv.m_szLinkFilter;
	m_szId               = serv.m_szId;
	m_iProxy             = serv.m_iProxy;
	m_szUserIdentityId   = serv.m_szUserIdentityId;
	if(serv.m_pChannelList)
		m_pChannelList = new QStringList(*(serv.m_pChannelList));
	else m_pChannelList = 0;
	m_bAutoConnect       = serv.m_bAutoConnect;
}

void KviIrcServer::operator=(const KviIrcServer &serv)
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
	m_uFlags             = serv.m_uFlags;
	m_szInitUMode = serv.m_szInitUMode;
	m_szOnConnectCommand = serv.m_szOnConnectCommand;
	m_szOnLoginCommand   = serv.m_szOnLoginCommand;
	m_szLinkFilter       = serv.m_szLinkFilter;
	m_szId               = serv.m_szId;
	m_szUserIdentityId   = serv.m_szUserIdentityId;
	m_iProxy	     = serv.m_iProxy;
	if(m_pChannelList)delete m_pChannelList;
	if(serv.m_pChannelList)
		m_pChannelList = new QStringList(*(serv.m_pChannelList));
	else m_pChannelList = 0;
	m_bAutoConnect    = serv.m_bAutoConnect;
}


KviIrcServer::~KviIrcServer()
{
	if(m_pChannelList)delete m_pChannelList;
	if(m_pReconnectInfo) delete m_pReconnectInfo;
}

void KviIrcServer::generateUniqueId()
{
	struct timeval tv;
	kvi_gettimeofday(&tv,0);
	KviQString::sprintf(m_szId,"myserver%d%d%d",tv.tv_usec,rand() % 1000,rand() % 1000);
}

QString KviIrcServer::ircUri()
{
	QString uri("irc");
	if(useSSL())uri += "s";
	if(isIpV6())uri += "6";
	uri += "://";
	uri += m_szHostname;

	if(m_uPort!=6667)
	{
		uri += ":";
		QString num;
		num.setNum(m_uPort);
		uri += num;
	}
	return uri;
}

void KviIrcServer::setAutoJoinChannelList(QStringList * pNewChannelList)
{
	if(m_pChannelList)delete m_pChannelList;
	m_pChannelList = pNewChannelList;
}


bool KviIrcServer::load(KviConfig * cfg,const QString &prefix)
{
	QString tmp;
	KviQString::sprintf(tmp,"%QHostname",&prefix);
	m_szHostname = cfg->readQStringEntry(tmp);
	KviQString::sprintf(tmp,"%QIp",&prefix);
	m_szIp = cfg->readQStringEntry(tmp);
	if(m_szHostname.isEmpty() && m_szIp.isEmpty())return false;
	KviQString::sprintf(tmp,"%QDescription",&prefix);
	m_szDescription = cfg->readQStringEntry(tmp);
	KviQString::sprintf(tmp,"%QUser",&prefix);
	m_szUser = cfg->readQStringEntry(tmp);
	KviQString::sprintf(tmp,"%QPass",&prefix);
	m_szPass = cfg->readQStringEntry(tmp);
	KviQString::sprintf(tmp,"%QNick",&prefix);
	m_szNick = cfg->readQStringEntry(tmp);
	KviQString::sprintf(tmp,"%QRealName",&prefix);
	m_szRealName = cfg->readQStringEntry(tmp);
	KviQString::sprintf(tmp,"%QInitUmode",&prefix);
	m_szInitUMode = cfg->readQStringEntry(tmp);
	KviQString::sprintf(tmp,"%QAutoJoinChannels",&prefix);
	QStringList l = cfg->readStringListEntry(tmp,QStringList());
	if(l.count() > 0)setAutoJoinChannelList(new QStringList(l));
	KviQString::sprintf(tmp,"%QAutoConnect",&prefix);
	m_bAutoConnect = cfg->readBoolEntry(tmp,false);
	KviQString::sprintf(tmp,"%QEncoding",&prefix);
	m_szEncoding = cfg->readQStringEntry(tmp);
	KviQString::sprintf(tmp,"%QOnConnectCommand",&prefix);
	m_szOnConnectCommand = cfg->readQStringEntry(tmp);
	KviQString::sprintf(tmp,"%QOnLoginCommand",&prefix);
	m_szOnLoginCommand = cfg->readQStringEntry(tmp);
	KviQString::sprintf(tmp,"%QLinkFilter",&prefix);
	m_szLinkFilter = cfg->readQStringEntry(tmp);
	KviQString::sprintf(tmp,"%QPort",&prefix);
	m_uPort = cfg->readUIntEntry(tmp,6667);
	KviQString::sprintf(tmp,"%QId",&prefix);
	m_szId = cfg->readQStringEntry(tmp);
	if(m_szId.isEmpty())generateUniqueId();
	KviQString::sprintf(tmp,"%QIpV6",&prefix);
	setIpV6(cfg->readBoolEntry(tmp,false));
	KviQString::sprintf(tmp,"%QCacheIp",&prefix);
	setCacheIp(cfg->readBoolEntry(tmp,false)); // true ?
	KviQString::sprintf(tmp,"%QSSL",&prefix);
	setUseSSL(cfg->readBoolEntry(tmp,false));
	KviQString::sprintf(tmp,"%QProxy",&prefix);
	setProxy(cfg->readIntEntry(tmp,-2));
	KviQString::sprintf(tmp,"%QUserIdentityId",&prefix);
	m_szUserIdentityId = cfg->readQStringEntry(tmp);
	return true;
}

void KviIrcServer::save(KviConfig * cfg,const QString &prefix)
{
	QString tmp;
	KviQString::sprintf(tmp,"%QHostname",&prefix);
	cfg->writeEntry(tmp,m_szHostname);
	KviQString::sprintf(tmp,"%QId",&prefix);
	cfg->writeEntry(tmp,m_szId);
	if(!m_szIp.isEmpty())
	{
		KviQString::sprintf(tmp,"%QIp",&prefix);
		cfg->writeEntry(tmp,m_szIp);
	}
	if(!m_szDescription.isEmpty())
	{
		KviQString::sprintf(tmp,"%QDescription",&prefix);
		cfg->writeEntry(tmp,m_szDescription);
	}
	if(!m_szUser.isEmpty())
	{
		KviQString::sprintf(tmp,"%QUser",&prefix);
		cfg->writeEntry(tmp,m_szUser);
	}
	if(!m_szPass.isEmpty())
	{
		KviQString::sprintf(tmp,"%QPass",&prefix);
		cfg->writeEntry(tmp,m_szPass);
	}
	if(!m_szNick.isEmpty())
	{
		KviQString::sprintf(tmp,"%QNick",&prefix);
		cfg->writeEntry(tmp,m_szNick);
	}
	if(!m_szRealName.isEmpty())
	{
		KviQString::sprintf(tmp,"%QRealName",&prefix);
		cfg->writeEntry(tmp,m_szRealName);
	}
	if(!m_szInitUMode.isEmpty())
	{
		KviQString::sprintf(tmp,"%QInitUMode",&prefix);
		cfg->writeEntry(tmp,m_szInitUMode);
	}
	if(autoJoinChannelList())
	{
		KviQString::sprintf(tmp,"%QAutoJoinChannels",&prefix);
		cfg->writeEntry(tmp,*(autoJoinChannelList()));
	}
	if(autoConnect()) // otherwise it defaults to false anyway
	{
		KviQString::sprintf(tmp,"%QAutoConnect",&prefix);
		cfg->writeEntry(tmp,autoConnect());
	}
	if(!m_szEncoding.isEmpty())
	{
		KviQString::sprintf(tmp,"%QEncoding",&prefix);
		cfg->writeEntry(tmp,m_szEncoding);
	}
	if(!m_szOnConnectCommand.isEmpty())
	{
		KviQString::sprintf(tmp,"%QOnConnectCommand",&prefix);
		cfg->writeEntry(tmp,m_szOnConnectCommand);
	}
	if(!m_szOnLoginCommand.isEmpty())
	{
		KviQString::sprintf(tmp,"%QOnLoginCommand",&prefix);
		cfg->writeEntry(tmp,m_szOnLoginCommand);
	}
	if(!m_szLinkFilter.isEmpty())
	{
		KviQString::sprintf(tmp,"%QLinkFilter",&prefix);
		cfg->writeEntry(tmp,m_szLinkFilter);
	}
	if(m_uPort != 6667)
	{
		KviQString::sprintf(tmp,"%QPort",&prefix);
		cfg->writeEntry(tmp,m_uPort);
	}
	if(isIpV6())
	{
		KviQString::sprintf(tmp,"%QIpV6",&prefix);
		cfg->writeEntry(tmp,isIpV6());
	}
	if(cacheIp())
	{
		KviQString::sprintf(tmp,"%QCacheIp",&prefix);
		cfg->writeEntry(tmp,cacheIp());
	}
	if(useSSL())
	{
		KviQString::sprintf(tmp,"%QSSL",&prefix);
		cfg->writeEntry(tmp,useSSL());
	}
	if(proxy()!=-2)
	{
		KviQString::sprintf(tmp,"%QProxy",&prefix);
		cfg->writeEntry(tmp,proxy());
	}
	if(!m_szUserIdentityId.isEmpty())
	{
		KviQString::sprintf(tmp,"%QUserIdentityId",&prefix);
		cfg->writeEntry(tmp,m_szUserIdentityId);
	}
}



KviIrcNetwork::KviIrcNetwork(const KviIrcNetwork &src)
{
	m_pChannelList = 0;
	m_pNickServRuleSet = 0;
	copyFrom(src);
}

KviIrcNetwork::KviIrcNetwork(const QString &name)
{
	m_szName = name;
	m_pChannelList = 0;
	m_pNickServRuleSet = 0;
	m_bAutoConnect = false;
	// m_szEncoding = QString::null; // set by default
}

KviIrcNetwork::~KviIrcNetwork()
{
	if(m_pChannelList)delete m_pChannelList;
	if(m_pNickServRuleSet)delete m_pNickServRuleSet;
}

void KviIrcNetwork::setAutoJoinChannelList(QStringList * pNewChannelList)
{
	if(m_pChannelList)delete m_pChannelList;
	m_pChannelList = pNewChannelList;
}

void KviIrcNetwork::setNickServRuleSet(KviNickServRuleSet * s)
{
	if(m_pNickServRuleSet)delete m_pNickServRuleSet;
	m_pNickServRuleSet = s;
}


void KviIrcNetwork::copyFrom(const KviIrcNetwork &src)
{
	m_szName = src.m_szName;
	m_szEncoding = src.m_szEncoding;
	m_szDescription = src.m_szDescription;
	m_szNickName = src.m_szNickName;
	m_szRealName = src.m_szRealName;
	m_szUserName = src.m_szUserName;
	m_bAutoConnect = src.m_bAutoConnect;
	m_szUserIdentityId = src.m_szUserIdentityId;
	m_szOnConnectCommand  = src.m_szOnConnectCommand;
	m_szOnLoginCommand = src.m_szOnLoginCommand;
	if(m_pChannelList)delete m_pChannelList;
	if(src.m_pChannelList)m_pChannelList = new QStringList(*(src.m_pChannelList));
	else m_pChannelList = 0;
	if(m_pNickServRuleSet)delete m_pNickServRuleSet;
	if(src.m_pNickServRuleSet)m_pNickServRuleSet = new KviNickServRuleSet(*(src.m_pNickServRuleSet));
	else m_pNickServRuleSet = 0;
}

KviIrcServerReconnectInfo::KviIrcServerReconnectInfo()
{
	m_bIsAway=false;
}

KviIrcServerReconnectInfo::~KviIrcServerReconnectInfo()
{
}

KviIrcServerReconnectInfo::KviIrcServerReconnectInfo(const KviIrcServerReconnectInfo &info)
{
	m_szNick         = info.m_szNick;
	m_szAwayReason   = info.m_szAwayReason;
	m_szJoinChannels = info.m_szJoinChannels;
	m_szOpenQueryes  = info.m_szOpenQueryes;
	m_bIsAway        = info.m_bIsAway;
}
