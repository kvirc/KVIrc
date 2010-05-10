//=============================================================================
//
//   File : kvi_network.cpp
//   Creation date : Wed Aug 27 2008 17:44:56 by Alexey Uzhva
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2008 Alexey Uzhva (wizard at opendoor dot ru)
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
	m_szPass = src.m_szPass;
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
/*
	// We don't copy the server list, since this function is called in KviServerOptionsWidget::commit()
	// to recreate the server list from scratch; copying the original servers will mean duplicate servers
	// (see bug ticket #300)
	for (KviServer *s = src.m_pServerList->first(); s; s = src.m_pServerList->next())
	{
		m_pServerList->append(new KviServer(*s));
	}
*/
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
		// we better go with the unique id first
		if(!s->m_szId.isEmpty())
		{
			// at least one of the 2 id is not empty, so if they match we're done
			if(KviQString::equalCI(s->m_szId, pServer->m_szId))
				return s;
		} else {
			// failback on the "check everything" method

			if (KviQString::equalCI(s->m_szHostname, pServer->m_szHostname)
					&& (s->m_uPort == pServer->m_uPort) && (s->useSSL()
					== pServer->useSSL()) && (s->isIPv6() == pServer->isIPv6()))
				return s;
		}
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
