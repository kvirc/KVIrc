//=============================================================================
//
//   File : KviIrcNetwork.cpp
//   Creation date : Wed Aug 27 2008 17:44:56 by Alexey Uzhva
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Alexey Uzhva (wizard at opendoor dot ru)
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

#include "KviIrcNetwork.h"
#include "KviIrcServer.h"
#include "KviNickServRuleSet.h"
#include "KviQString.h"

KviIrcNetwork::KviIrcNetwork(const KviIrcNetwork & src)
{
	m_pChannelList = nullptr;
	m_pNickServRuleSet = nullptr;
	m_pServerList = new KviPointerList<KviIrcServer>;
	m_pServerList->setAutoDelete(true);
	copyFrom(src);
}

KviIrcNetwork::KviIrcNetwork(const QString & name)
{
	m_szName = name;
	m_pChannelList = nullptr;
	m_pNickServRuleSet = nullptr;
	m_bAutoConnect = false;
	m_pServerList = new KviPointerList<KviIrcServer>;
	m_pServerList->setAutoDelete(true);
	m_pCurrentServer = nullptr;
}

KviIrcNetwork::~KviIrcNetwork()
{
	if(m_pChannelList)
		delete m_pChannelList;
	if(m_pNickServRuleSet)
		delete m_pNickServRuleSet;
	delete m_pServerList;
}

void KviIrcNetwork::setAutoJoinChannelList(QStringList * pNewChannelList)
{
	if(m_pChannelList)
		delete m_pChannelList;
	m_pChannelList = pNewChannelList;
}

void KviIrcNetwork::setAutoJoinChannelList(const QString & szNewChannelList)
{
	if(m_pChannelList)
		delete m_pChannelList;
	QStringList lChans = szNewChannelList.split(",");
	if(lChans.isEmpty())
		m_pChannelList = nullptr;
	else
		m_pChannelList = new QStringList(lChans);
}

void KviIrcNetwork::setNickServRuleSet(KviNickServRuleSet * s)
{
	if(m_pNickServRuleSet)
		delete m_pNickServRuleSet;
	m_pNickServRuleSet = s;
}

void KviIrcNetwork::copyFrom(const KviIrcNetwork & src)
{
	m_szName = src.m_szName;
	m_szEncoding = src.m_szEncoding;
	m_szTextEncoding = src.m_szTextEncoding;
	m_szDescription = src.m_szDescription;
	m_szNickName = src.m_szNickName;
	m_szAlternativeNickName = src.m_szAlternativeNickName;
	m_szRealName = src.m_szRealName;
	m_szUserName = src.m_szUserName;
	m_szPass = src.m_szPass;
	m_bAutoConnect = src.m_bAutoConnect;
	m_szUserIdentityId = src.m_szUserIdentityId;
	m_szOnConnectCommand = src.m_szOnConnectCommand;
	m_szOnLoginCommand = src.m_szOnLoginCommand;
	if(m_pChannelList)
		delete m_pChannelList;
	if(src.m_pChannelList)
		m_pChannelList = new QStringList(*(src.m_pChannelList));
	else
		m_pChannelList = nullptr;
	if(m_pNickServRuleSet)
		delete m_pNickServRuleSet;
	if(src.m_pNickServRuleSet)
		m_pNickServRuleSet = new KviNickServRuleSet(*(src.m_pNickServRuleSet));
	else
		m_pNickServRuleSet = nullptr;
	/*
	// We don't copy the server list, since this function is called in KviIrcServerOptionsWidget::commit()
	// to recreate the server list from scratch; copying the original servers will mean duplicate servers
	// (see bug ticket #300)
	for (KviIrcServer *s = src.m_pServerList->first(); s; s = src.m_pServerList->next())
	{
		m_pServerList->append(new KviIrcServer(*s));
	}
*/
}

void KviIrcNetwork::insertServer(KviIrcServer * srv)
{
	m_pServerList->append(srv);
}

KviIrcServer * KviIrcNetwork::findServer(const QString & szHostname)
{
	for(KviIrcServer * s = m_pServerList->first(); s; s = m_pServerList->next())
	{
		if(KviQString::equalCI(s->hostName(), szHostname))
			return s;
	}
	return nullptr;
}

KviIrcServer * KviIrcNetwork::findServer(const KviIrcServer * pServer)
{
	for(KviIrcServer * s = m_pServerList->first(); s; s = m_pServerList->next())
	{
		// we better go with the unique id first
		if(!s->id().isEmpty())
		{
			// at least one of the 2 id is not empty, so if they match we're done
			if(KviQString::equalCI(s->id(), pServer->id()))
				return s;
		}
		else
		{
			// failback on the "check everything" method
			if(KviQString::equalCI(s->hostName(), pServer->hostName())
			    && (s->port() == pServer->port()) && (s->useSSL()
			                                             == pServer->useSSL())
			    && (s->isIPv6() == pServer->isIPv6()))
				return s;
		}
	}
	return nullptr;
}

void KviIrcNetwork::setCurrentServer(KviIrcServer * srv)
{
	if(m_pServerList->findRef(srv) != -1)
		m_pCurrentServer = srv;
}

KviIrcServer * KviIrcNetwork::currentServer()
{
	if(m_pCurrentServer)
		return m_pCurrentServer;
	m_pCurrentServer = m_pServerList->first();
	return m_pCurrentServer;
}
