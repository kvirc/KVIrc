//=============================================================================
//
//   File : KviIrcServer.cpp
//   Creation date : Mon Jul 10 2000 03:42:59 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
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

#include <memory>
#include <vector>
#include <cstdlib>

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
	m_pReconnectInfo = nullptr;
	m_uFlags = Flags::CAP | Flags::STARTTLS;
	m_uPort = 6667;
	m_pAutoJoinChannelList = nullptr;
	m_bAutoConnect = false;
	m_iProxy = -1;
}

KviProxy * KviIrcServer::proxyServer(KviProxyDataBase * pDb)
{
	int i = 0;
	if(proxy() < 0)
		return nullptr;
	std::vector<std::unique_ptr<KviProxy>> & proxylist = pDb->proxyList();
	for(auto & pProxy : proxylist)
	{
		if(i == proxy())
			return pProxy.get();
		i++;
	}
	return nullptr;
}

KviIrcServer::KviIrcServer(const KviIrcServer & serv)
{
	m_szHostname = serv.m_szHostname;
	m_szIp = serv.m_szIp;
	m_szDescription = serv.m_szDescription;
	m_szUser = serv.m_szUser;
	m_szPass = serv.m_szPass;
	m_uPort = serv.m_uPort;
	m_szNick = serv.m_szNick;
	m_szAlternativeNick = serv.m_szAlternativeNick;
	m_szRealName = serv.m_szRealName;
	m_szEncoding = serv.m_szEncoding;
	m_szTextEncoding = serv.m_szTextEncoding;
	m_uFlags = serv.m_uFlags;
	m_szInitUMode = serv.m_szInitUMode;
	m_szOnConnectCommand = serv.m_szOnConnectCommand;
	m_szOnLoginCommand = serv.m_szOnLoginCommand;
	m_szLinkFilter = serv.m_szLinkFilter;
	m_szId = serv.m_szId;
	m_iProxy = serv.m_iProxy;
	m_szUserIdentityId = serv.m_szUserIdentityId;
	m_bAutoConnect = serv.m_bAutoConnect;
	m_szSaslNick = serv.m_szSaslNick;
	m_szSaslPass = serv.m_szSaslPass;
	m_szSaslMethod = serv.m_szSaslMethod;

	if(serv.m_pAutoJoinChannelList)
		m_pAutoJoinChannelList = new QStringList(*(serv.m_pAutoJoinChannelList));
	else
		m_pAutoJoinChannelList = nullptr;

	if(serv.m_pReconnectInfo)
		m_pReconnectInfo = new KviIrcServerReconnectInfo(*(serv.m_pReconnectInfo));
	else
		m_pReconnectInfo = nullptr;
}

void KviIrcServer::operator=(const KviIrcServer & serv)
{
	m_szHostname = serv.m_szHostname;
	m_szIp = serv.m_szIp;
	m_szDescription = serv.m_szDescription;
	m_szUser = serv.m_szUser;
	m_szPass = serv.m_szPass;
	m_uPort = serv.m_uPort;
	m_szNick = serv.m_szNick;
	m_szAlternativeNick = serv.m_szAlternativeNick;
	m_szRealName = serv.m_szRealName;
	m_szEncoding = serv.m_szEncoding;
	m_szTextEncoding = serv.m_szTextEncoding;
	m_uFlags = serv.m_uFlags;
	m_szInitUMode = serv.m_szInitUMode;
	m_szOnConnectCommand = serv.m_szOnConnectCommand;
	m_szOnLoginCommand = serv.m_szOnLoginCommand;
	m_szLinkFilter = serv.m_szLinkFilter;
	m_szId = serv.m_szId;
	m_szUserIdentityId = serv.m_szUserIdentityId;
	m_iProxy = serv.m_iProxy;
	m_bAutoConnect = serv.m_bAutoConnect;
	m_szSaslNick = serv.m_szSaslNick;
	m_szSaslPass = serv.m_szSaslPass;

	if(m_pAutoJoinChannelList)
		delete m_pAutoJoinChannelList;
	if(serv.m_pAutoJoinChannelList)
		m_pAutoJoinChannelList = new QStringList(*(serv.m_pAutoJoinChannelList));
	else
		m_pAutoJoinChannelList = nullptr;

	if(m_pReconnectInfo)
		delete m_pReconnectInfo;
	if(serv.m_pReconnectInfo)
		m_pReconnectInfo = new KviIrcServerReconnectInfo(*(serv.m_pReconnectInfo));
	else
		m_pReconnectInfo = nullptr;
}

KviIrcServer::~KviIrcServer()
{
	if(m_pAutoJoinChannelList)
	{
		delete m_pAutoJoinChannelList;
		m_pAutoJoinChannelList = nullptr;
	}
	if(m_pReconnectInfo)
	{
		delete m_pReconnectInfo;
		m_pReconnectInfo = nullptr;
	}
}

void KviIrcServer::setReconnectInfo(KviIrcServerReconnectInfo * pInfo)
{
	if(m_pReconnectInfo)
		delete m_pReconnectInfo;
	m_pReconnectInfo = pInfo;
}

void KviIrcServer::clearReconnectInfo()
{
	if(!m_pReconnectInfo)
		return;
	delete m_pReconnectInfo;
	m_pReconnectInfo = nullptr;
}

void KviIrcServer::generateUniqueId()
{
	struct timeval tv;
	kvi_gettimeofday(&tv);
	m_szId = QString("myserver%1%2%3").arg(tv.tv_usec).arg(rand() % 1000).arg(rand() % 1000);
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
		m_pAutoJoinChannelList = nullptr;
	else
		m_pAutoJoinChannelList = new QStringList(lChans);
}

bool KviIrcServer::load(KviConfigurationFile * pCfg, const QString & szPrefix)
{
	QString szTmp;
	szTmp = QString("%1Hostname").arg(szPrefix);
	m_szHostname = pCfg->readEntry(szTmp);
	szTmp = QString("%1Ip").arg(szPrefix);
	m_szIp = pCfg->readEntry(szTmp);
	if(m_szHostname.isEmpty() && m_szIp.isEmpty())
		return false;

	szTmp = QString("%1Description").arg(szPrefix);
	m_szDescription = pCfg->readEntry(szTmp);
	szTmp = QString("%1User").arg(szPrefix);
	m_szUser = pCfg->readEntry(szTmp);
	szTmp = QString("%1Pass").arg(szPrefix);
	m_szPass = pCfg->readEntry(szTmp);
	szTmp = QString("%1Nick").arg(szPrefix);
	m_szNick = pCfg->readEntry(szTmp);
	szTmp = QString("%1AlternativeNick").arg(szPrefix);
	m_szAlternativeNick = pCfg->readEntry(szTmp);
	szTmp = QString("%1SaslPass").arg(szPrefix);
	m_szSaslPass = pCfg->readEntry(szTmp);
	szTmp = QString("%1SaslNick").arg(szPrefix);
	m_szSaslNick = pCfg->readEntry(szTmp);
	szTmp = QString("%1SaslMethod").arg(szPrefix);
	m_szSaslMethod = pCfg->readEntry(szTmp, QStringLiteral("PLAIN"));
	szTmp = QString("%1RealName").arg(szPrefix);
	m_szRealName = pCfg->readEntry(szTmp);
	szTmp = QString("%1InitUmode").arg(szPrefix);
	m_szInitUMode = pCfg->readEntry(szTmp);
	szTmp = QString("%1AutoJoinChannels").arg(szPrefix);
	QStringList l = pCfg->readStringListEntry(szTmp, QStringList());
	if(l.count() > 0)
		setAutoJoinChannelList(new QStringList(l));

	szTmp = QString("%1AutoConnect").arg(szPrefix);
	m_bAutoConnect = pCfg->readBoolEntry(szTmp, false);
	szTmp = QString("%1Encoding").arg(szPrefix);
	m_szEncoding = pCfg->readEntry(szTmp);
	szTmp = QString("%1TextEncoding").arg(szPrefix);
	m_szTextEncoding = pCfg->readEntry(szTmp);
	szTmp = QString("%1OnConnectCommand").arg(szPrefix);
	m_szOnConnectCommand = pCfg->readEntry(szTmp);
	szTmp = QString("%1OnLoginCommand").arg(szPrefix);
	m_szOnLoginCommand = pCfg->readEntry(szTmp);
	szTmp = QString("%1LinkFilter").arg(szPrefix);
	m_szLinkFilter = pCfg->readEntry(szTmp);
	szTmp = QString("%1Port").arg(szPrefix);
	m_uPort = pCfg->readUIntEntry(szTmp, 6667);
	szTmp = QString("%1Id").arg(szPrefix);
	m_szId = pCfg->readEntry(szTmp);
	if(m_szId.isEmpty())
		generateUniqueId();

	szTmp = QString("%1IPv6").arg(szPrefix);
	setIPv6(pCfg->readBoolEntry(szTmp, false));
	szTmp = QString("%1CacheIp").arg(szPrefix);
	setCacheIp(pCfg->readBoolEntry(szTmp, false)); // true ?
	szTmp = QString("%1SSL").arg(szPrefix);
	setUseSSL(pCfg->readBoolEntry(szTmp, false));
	szTmp = QString("%1EnabledCAP").arg(szPrefix);
	setEnabledCAP(pCfg->readBoolEntry(szTmp, true));
	szTmp = QString("%1EnabledSTARTTLS").arg(szPrefix);
	setEnabledSTARTTLS(pCfg->readBoolEntry(szTmp, true));
	szTmp = QString("%1EnabledSASL").arg(szPrefix);
	setEnabledSASL(pCfg->readBoolEntry(szTmp, false));
	szTmp = QString("%1Proxy").arg(szPrefix);
	setProxy(pCfg->readIntEntry(szTmp, -2));
	szTmp = QString("%1UserIdentityId").arg(szPrefix);
	m_szUserIdentityId = pCfg->readEntry(szTmp);
	szTmp = QString("%1Favorite").arg(szPrefix);
	setFavorite(pCfg->readBoolEntry(szTmp, false));
	return true;
}

void KviIrcServer::save(KviConfigurationFile * pCfg, const QString & szPrefix)
{
	QString szTmp;
	szTmp = QString("%1Hostname").arg(szPrefix);
	pCfg->writeEntry(szTmp, m_szHostname);
	szTmp = QString("%1Id").arg(szPrefix);
	pCfg->writeEntry(szTmp, m_szId);
	if(!m_szIp.isEmpty())
	{
		szTmp = QString("%1Ip").arg(szPrefix);
		pCfg->writeEntry(szTmp, m_szIp);
	}
	if(!m_szDescription.isEmpty())
	{
		szTmp = QString("%1Description").arg(szPrefix);
		pCfg->writeEntry(szTmp, m_szDescription);
	}
	if(!m_szUser.isEmpty())
	{
		szTmp = QString("%1User").arg(szPrefix);
		pCfg->writeEntry(szTmp, m_szUser);
	}
	if(!m_szPass.isEmpty())
	{
		szTmp = QString("%1Pass").arg(szPrefix);
		pCfg->writeEntry(szTmp, m_szPass);
	}
	if(!m_szNick.isEmpty())
	{
		szTmp = QString("%1Nick").arg(szPrefix);
		pCfg->writeEntry(szTmp, m_szNick);
	}
	if(!m_szAlternativeNick.isEmpty())
	{
		szTmp = QString("%1AlternativeNick").arg(szPrefix);
		pCfg->writeEntry(szTmp, m_szAlternativeNick);
	}
	if(!m_szSaslPass.isEmpty())
	{
		szTmp = QString("%1SaslPass").arg(szPrefix);
		pCfg->writeEntry(szTmp, m_szSaslPass);
	}
	if(!m_szSaslNick.isEmpty())
	{
		szTmp = QString("%1SaslNick").arg(szPrefix);
		pCfg->writeEntry(szTmp, m_szSaslNick);
	}
	if(!m_szSaslMethod.isEmpty() && enabledSASL())
	{
		szTmp = QString("%1SaslMethod").arg(szPrefix);
		pCfg->writeEntry(szTmp, m_szSaslMethod);
	}
	if(!m_szRealName.isEmpty())
	{
		szTmp = QString("%1RealName").arg(szPrefix);
		pCfg->writeEntry(szTmp, m_szRealName);
	}
	if(!m_szInitUMode.isEmpty())
	{
		szTmp = QString("%1InitUMode").arg(szPrefix);
		pCfg->writeEntry(szTmp, m_szInitUMode);
	}
	if(autoJoinChannelList())
	{
		szTmp = QString("%1AutoJoinChannels").arg(szPrefix);
		pCfg->writeEntry(szTmp, *(autoJoinChannelList()));
	}
	if(autoConnect()) // otherwise it defaults to false anyway
	{
		szTmp = QString("%1AutoConnect").arg(szPrefix);
		pCfg->writeEntry(szTmp, autoConnect());
	}
	if(!m_szEncoding.isEmpty())
	{
		szTmp = QString("%1Encoding").arg(szPrefix);
		pCfg->writeEntry(szTmp, m_szEncoding);
	}
	if(!m_szTextEncoding.isEmpty())
	{
		szTmp = QString("%1TextEncoding").arg(szPrefix);
		pCfg->writeEntry(szTmp, m_szTextEncoding);
	}
	if(!m_szOnConnectCommand.isEmpty())
	{
		szTmp = QString("%1OnConnectCommand").arg(szPrefix);
		pCfg->writeEntry(szTmp, m_szOnConnectCommand);
	}
	if(!m_szOnLoginCommand.isEmpty())
	{
		szTmp = QString("%1OnLoginCommand").arg(szPrefix);
		pCfg->writeEntry(szTmp, m_szOnLoginCommand);
	}
	if(!m_szLinkFilter.isEmpty())
	{
		szTmp = QString("%1LinkFilter").arg(szPrefix);
		pCfg->writeEntry(szTmp, m_szLinkFilter);
	}
	if(m_uPort != 6667)
	{
		szTmp = QString("%1Port").arg(szPrefix);
		pCfg->writeEntry(szTmp, m_uPort);
	}
	if(isIPv6())
	{
		szTmp = QString("%1IPv6").arg(szPrefix);
		pCfg->writeEntry(szTmp, isIPv6());
	}
	if(cacheIp())
	{
		szTmp = QString("%1CacheIp").arg(szPrefix);
		pCfg->writeEntry(szTmp, cacheIp());
	}
	if(useSSL())
	{
		szTmp = QString("%1SSL").arg(szPrefix);
		pCfg->writeEntry(szTmp, useSSL());
	}
	if(!enabledCAP())
	{
		szTmp = QString("%1EnabledCAP").arg(szPrefix);
		pCfg->writeEntry(szTmp, enabledCAP());
	}
	if(!enabledSTARTTLS())
	{
		szTmp = QString("%1EnabledSTARTTLS").arg(szPrefix);
		pCfg->writeEntry(szTmp, enabledSTARTTLS());
	}
	if(enabledSASL())
	{
		szTmp = QString("%1EnabledSASL").arg(szPrefix);
		pCfg->writeEntry(szTmp, enabledSASL());
	}
	if(proxy() != -2)
	{
		szTmp = QString("%1Proxy").arg(szPrefix);
		pCfg->writeEntry(szTmp, proxy());
	}
	if(!m_szUserIdentityId.isEmpty())
	{
		szTmp = QString("%1UserIdentityId").arg(szPrefix);
		pCfg->writeEntry(szTmp, m_szUserIdentityId);
	}
	if(favorite())
	{
		szTmp = QString("%1Favorite").arg(szPrefix);
		pCfg->writeEntry(szTmp, favorite());
	}
}
