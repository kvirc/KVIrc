//=============================================================================
//
//   File : kvi_ircuserdb.cpp
//   Creation date : Mon Jul 31 2000 21:23:22 by Szymon Stefanek
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#define __KVILIB__


#include "kvi_debug.h"
#include "kvi_ircuserdb.h"


KviIrcUserEntry::KviIrcUserEntry(const QString &user,const QString &host)
{
	m_szUser = user;
	m_szHost = host;
	m_pAvatar = 0;
	m_nRefs = 1;
	m_iHops = -1;
	m_bAway = false;
}

KviIrcUserEntry::~KviIrcUserEntry()
{
	if(m_pAvatar)delete m_pAvatar;
}

void KviIrcUserEntry::setAvatar(KviAvatar * av)
{
	if(m_pAvatar)delete m_pAvatar;
	m_pAvatar = av;
}

KviAvatar * KviIrcUserEntry::forgetAvatar()
{
	KviAvatar * ret = m_pAvatar;
	m_pAvatar = 0;
	return ret;
}



KviIrcUserDataBase::KviIrcUserDataBase()
{
	// we expect a maximum of ~4000 users (= ~16 KB array on a 32 bit machine)
	// ...after that we will loose in performance
	// ... well...4000 users is a really big number...say 6-7 really big channels
	// (4001 is prime)
	// up to 12000 users we will have a reasonably fast access.
	// the performance increase since kvirc versions < 3.0.0
	// is really big anyway (there was a linear list instead of a hash!!!)

	m_pDict = new QDict<KviIrcUserEntry>(4001,false);
	m_pDict->setAutoDelete(true);
}

KviIrcUserDataBase::~KviIrcUserDataBase()
{
	delete m_pDict;
}

void KviIrcUserDataBase::clear()
{
	delete m_pDict;
	m_pDict = new QDict<KviIrcUserEntry>(4001,false);
	m_pDict->setAutoDelete(true);
}

KviIrcUserEntry * KviIrcUserDataBase::insertUser(const QString &nick,const QString &user,const QString &hostname)
{
	KviIrcUserEntry * e = m_pDict->find(nick);
	if(e)
	{
		e->m_nRefs++;
		if(e->m_szUser.isEmpty())
		{
			e->m_szUser = user;
			e->m_szHost = hostname;
		}
	} else {
		e = new KviIrcUserEntry(user,hostname);
		m_pDict->insert(nick,e);
	}
	return e;
}

void KviIrcUserDataBase::removeUser(const QString &nick,KviIrcUserEntry * e)
{
	e->m_nRefs--;
	if(e->m_nRefs == 0)m_pDict->remove(nick);
}

