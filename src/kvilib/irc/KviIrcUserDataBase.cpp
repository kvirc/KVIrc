//=============================================================================
//
//   File : KviIrcUserDataBase.cpp
//   Creation date : Mon Jul 31 2000 21:23:22 by Szymon Stefanek
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

#include "kvi_debug.h"
#include "KviIrcUserDataBase.h"
#include "KviRegisteredUser.h"
#include "KviRegisteredUserDataBase.h"
#include "KviStringConversion.h"

KviIrcUserDataBase::KviIrcUserDataBase()
    : QObject()
{
	// we expect a maximum of ~4000 users (= ~16 KB array on a 32 bit machine)
	// ...after that we will loose in performance
	// ... well...4000 users is a really big number...say 6-7 really big channels
	// (4001 is prime)
	// up to 12000 users we will have a reasonably fast access.
	// the performance increase since kvirc versions < 3.0.0
	// is really big anyway (there was a linear list instead of a hash!!!)

	m_pDict = new KviPointerHashTable<QString, KviIrcUserEntry>(4001, false);
	m_pDict->setAutoDelete(true);
	setupConnectionWithReguserDb();
}

KviIrcUserDataBase::~KviIrcUserDataBase()
{
	delete m_pDict;
}

bool KviIrcUserDataBase::haveCustomColor(const QString & szNick)
{
	KviIrcUserEntry * pEntry = find(szNick);
	if(!pEntry)
		return false;
	if(pEntry->m_szLastRegisteredMatchNick != szNick)
		registeredUser(szNick);
	if(!pEntry->m_bNotFoundRegUserLookup)
		return pEntry->m_bUseCustomColor;

	return false;
}

QColor * KviIrcUserDataBase::customColor(const QString & szNick)
{
	KviIrcUserEntry * pEntry = find(szNick);
	if(!pEntry)
		return nullptr;
	if(pEntry->m_szLastRegisteredMatchNick != szNick)
		registeredUser(szNick);

	if(!pEntry->m_bNotFoundRegUserLookup)
		return &(pEntry->m_cachedColor);

	return nullptr;
}

KviRegisteredUser * KviIrcUserDataBase::registeredUser(const QString & szNick, const QString & szUser, const QString & szHost)
{
	if(szNick.isEmpty())
		return nullptr;
	KviIrcUserEntry * pEntry = find(szNick);
	if(!pEntry)
		return g_pRegisteredUserDataBase->findMatchingUser(szNick, szUser, szHost);

	KviRegisteredUser * pUser = nullptr;

	if(pEntry->m_bNotFoundRegUserLookup && pEntry->m_szLastRegisteredMatchNick == szNick)
		return nullptr;

	if(!pEntry->m_szRegisteredUserName.isEmpty() && pEntry->m_szLastRegisteredMatchNick == szNick)
		pUser = g_pRegisteredUserDataBase->getUser(pEntry->m_szRegisteredUserName);

	if(!pUser)
	{
		//user renamed or it is a first loockup
		if(pEntry->hasHost() && pEntry->hasUser())
		{
			pUser = g_pRegisteredUserDataBase->findMatchingUser(szNick, pEntry->user(), pEntry->host());
			if(pUser)
			{
				pEntry->m_szLastRegisteredMatchNick = szNick;
				pEntry->m_szRegisteredUserName = pUser->name();

				pEntry->m_bUseCustomColor = pUser->getBoolProperty("useCustomColor");
				QString szTmp = pUser->getProperty("customColor");
				KviStringConversion::fromString(szTmp, pEntry->m_cachedColor);

				pEntry->m_bNotFoundRegUserLookup = false; //to be sure
			}
			else
			{
				pEntry->m_szLastRegisteredMatchNick = szNick;
				pEntry->m_bNotFoundRegUserLookup = true;
			}
		}
	}

	return pUser;
}

KviRegisteredUser * KviIrcUserDataBase::registeredUser(const QString & szNick)
{
	if(szNick.isEmpty())
		return nullptr;
	KviIrcUserEntry * pEntry = find(szNick);
	if(!pEntry)
		return nullptr;
	return registeredUser(szNick, pEntry->user(), pEntry->host());
}

void KviIrcUserDataBase::clear()
{
	delete m_pDict;
	m_pDict = new KviPointerHashTable<QString, KviIrcUserEntry>(4001, false);
	m_pDict->setAutoDelete(true);
}

KviIrcUserEntry * KviIrcUserDataBase::insertUser(const QString & szNick, const QString & szUser, const QString & szHost)
{
	KviIrcUserEntry * pEntry = m_pDict->find(szNick);
	if(pEntry)
	{
		pEntry->m_nRefs++;
		if(pEntry->m_szUser.isEmpty())
		{
			pEntry->m_szUser = szUser;
			pEntry->m_szHost = szHost;
		}
	}
	else
	{
		pEntry = new KviIrcUserEntry(szUser, szHost);
		m_pDict->insert(szNick, pEntry);
	}
	return pEntry;
}

bool KviIrcUserDataBase::removeUser(const QString & szNick, KviIrcUserEntry * pEntry)
{
	pEntry->m_nRefs--;
	if(pEntry->m_nRefs == 0)
	{
		m_pDict->remove(szNick);
		return true;
	}
	return false;
}

void KviIrcUserDataBase::setupConnectionWithReguserDb()
{
	connect(g_pRegisteredUserDataBase, SIGNAL(userRemoved(const QString &)), this, SLOT(registeredUserChanged(const QString &)));
	connect(g_pRegisteredUserDataBase, SIGNAL(userChanged(const QString &)), this, SLOT(registeredUserChanged(const QString &)));
	connect(g_pRegisteredUserDataBase, SIGNAL(userAdded(const QString &)), this, SLOT(registeredUserAdded(const QString &)));
	connect(g_pRegisteredUserDataBase, SIGNAL(databaseCleared()), this, SLOT(registeredDatabaseCleared()));
}

void KviIrcUserDataBase::registeredUserChanged(const QString & szUser)
{
	KviPointerHashTableIterator<QString, KviIrcUserEntry> it(*m_pDict);
	for(; it.current(); ++it)
	{
		if(it.current()->m_szRegisteredUserName == szUser)
		{
			it.current()->m_szRegisteredUserName = "";
			it.current()->m_bNotFoundRegUserLookup = false;
		}
	}
}

void KviIrcUserDataBase::registeredUserAdded(const QString &)
{
	KviPointerHashTableIterator<QString, KviIrcUserEntry> it(*m_pDict);
	for(; it.current(); ++it)
	{
		if(it.current()->m_szRegisteredUserName.isEmpty())
		{
			it.current()->m_bNotFoundRegUserLookup = false;
		}
	}
}

void KviIrcUserDataBase::registeredDatabaseCleared()
{
	KviPointerHashTableIterator<QString, KviIrcUserEntry> it(*m_pDict);
	for(; it.current(); ++it)
	{
		it.current()->m_szRegisteredUserName = "";
		it.current()->m_bNotFoundRegUserLookup = false;
	}
}
