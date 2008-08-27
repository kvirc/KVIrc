/*
 * kvi_network.cpp
 *
 *  Created on: 27.08.2008
 *      Author: Alexey
 */

#include "kvi_network.h"
#include "kvi_ircserver.h"
#include "kvi_nickserv.h"
#include <QStringList>

KviNetwork::KviNetwork(const KviNetwork &src)
{
	m_pChannelList = 0;
	m_pNickServRuleSet = 0;
	m_pServerList = new KviPointerList<KviServer>;
	m_pServerList->setAutoDelete(true);
	copyFrom(src);
}

KviNetwork::KviNetwork(const QString &name)
{
	m_szName = name;
	m_pChannelList = 0;
	m_pNickServRuleSet = 0;
	m_bAutoConnect = false;
	m_pServerList = new KviPointerList<KviServer>;
	m_pServerList->setAutoDelete(true);
	m_pCurrentServer = 0;
}

KviNetwork::~KviNetwork()
{
	if(m_pChannelList)delete m_pChannelList;
	if(m_pNickServRuleSet)delete m_pNickServRuleSet;
	delete m_pServerList;
}

void KviNetwork::setAutoJoinChannelList(QStringList * pNewChannelList)
{
	if(m_pChannelList)delete m_pChannelList;
	m_pChannelList = pNewChannelList;
}

void KviNetwork::setNickServRuleSet(KviNickServRuleSet * s)
{
	if(m_pNickServRuleSet)delete m_pNickServRuleSet;
	m_pNickServRuleSet = s;
}

void KviNetwork::copyFrom(const KviNetwork &src)
{
	m_szName = src.m_szName;
	m_szEncoding = src.m_szEncoding;
	m_szTextEncoding = src.m_szTextEncoding;
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
	for (KviServer *s = src.m_pServerList->first(); s; s = src.m_pServerList->next())
	{
		m_pServerList->append(new KviServer(*s));
	}
}

void KviNetwork::insertServer(KviServer *srv)
{
	m_pServerList->append(srv);
}

KviServer * KviNetwork::findServer(const QString & szHostname)
{
	for (KviServer *s = m_pServerList->first(); s; s = m_pServerList->next())
	{
		if (KviQString::equalCI(s->m_szHostname, szHostname))
			return s;
	}
	return 0;
}

KviServer * KviNetwork::findServer(const KviServer * pServer)
{
	for (KviServer *s = m_pServerList->first(); s; s = m_pServerList->next())
	{
		if (KviQString::equalCI(s->m_szHostname, pServer->m_szHostname)
				&& (s->m_uPort == pServer->m_uPort) && (s->useSSL()
				== pServer->useSSL()) && (s->isIPv6() == pServer->isIPv6()))
			return s;
	}
	return 0;
}

void KviNetwork::setCurrentServer(KviServer *srv)
{
	if(m_pServerList->findRef(srv) != -1)m_pCurrentServer = srv;
}

KviServer * KviNetwork::currentServer()
{
	if(m_pCurrentServer)return m_pCurrentServer;
	m_pCurrentServer = m_pServerList->first();
	return m_pCurrentServer;
}
