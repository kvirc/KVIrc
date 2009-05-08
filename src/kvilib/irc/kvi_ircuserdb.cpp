//=============================================================================
//
//   File : kvi_ircuserdb.cpp
//   Creation date : Mon Jul 31 2000 21:23:22 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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



#include "kvi_debug.h"
#include "kvi_ircuserdb.h"
#include "kvi_mirccntrl.h"
#include "kvi_qstring.h"
#include "kvi_stringconversion.h"

//static int cacheHit = 0;
//static int cacheMiss = 0;

KviIrcUserEntry::KviIrcUserEntry(const QString &user,const QString &host)
{
	m_szUser = user;
	m_szHost = host;
	m_pAvatar = 0;
	m_nRefs = 1;
	m_iHops = -1;
	m_bAway = false;
	m_bIrcOp = false;
	m_eGender = Unknown;
	m_bBot = false;
	m_bNotFoundRegUserLoockup=false;
	m_bUseCustomColor=false;
}

void KviIrcUserEntry::setRealName(const QString &rn)
{
	m_szRealName = rn;
	m_szRealName = KviQString::trimmed(m_szRealName);
	if(m_szRealName.length()>=3)
	{
		if( (m_szRealName[0].unicode()==KVI_TEXT_COLOR) && (m_szRealName[2].unicode()==KVI_TEXT_RESET) )
		{
			// hum.. encoded as hidden color code eh ? publish is somewhere, so others might implement this...
			// for backwards compatibily, 3=bot
			if(m_szRealName[1].unicode() & 1 && m_szRealName[1].unicode() & 2)
			{
				setBot(true); //3
			} else {
				if(m_szRealName[1].unicode() & 1)
				{
					setGender(Male); //1
				} else {
					if(m_szRealName[1].unicode() & 2)
					{
						setGender(Female); //2
					}
				}
			}
			m_szRealName.remove(0,3);
		}
	}
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
: QObject()
{
	// we expect a maximum of ~4000 users (= ~16 KB array on a 32 bit machine)
	// ...after that we will loose in performance
	// ... well...4000 users is a really big number...say 6-7 really big channels
	// (4001 is prime)
	// up to 12000 users we will have a reasonably fast access.
	// the performance increase since kvirc versions < 3.0.0
	// is really big anyway (there was a linear list instead of a hash!!!)

	m_pDict = new KviPointerHashTable<QString,KviIrcUserEntry>(4001,false);
	m_pDict->setAutoDelete(true);
	setupConnectionWithReguserDb();
}

KviIrcUserDataBase::~KviIrcUserDataBase()
{
	delete m_pDict;
}

bool KviIrcUserDataBase::haveCustomColor(const QString & nick)
{
	KviIrcUserEntry *u = find(nick);
	if(!u) return false;
	if( u->m_szLastRegisteredMatchNick!=nick)
		registeredUser(nick);
	if(!u->m_bNotFoundRegUserLoockup)
	{
		return u->m_bUseCustomColor;
	}
	return false;
}

QColor* KviIrcUserDataBase::customColor(const QString & nick)
{
	KviIrcUserEntry *u = find(nick);
	if(!u) return 0;
	if( u->m_szLastRegisteredMatchNick!=nick)
		registeredUser(nick);

	if(!u->m_bNotFoundRegUserLoockup)
	{
		return &(u->m_cachedColor);
	}
	return 0;
}


KviRegisteredUser* KviIrcUserDataBase::registeredUser(const QString & nick,const QString & user,const QString & host)
{
	if(nick.isEmpty()) return 0;
	KviIrcUserEntry *u = find(nick);
	if(!u) return g_pRegisteredUserDataBase->findMatchingUser(nick,user,host);
	KviRegisteredUser* pUser=0;

	if(u->m_bNotFoundRegUserLoockup && u->m_szLastRegisteredMatchNick==nick)
	{
		//cacheHit++;
		//debug("cache hits/miss = %i/%i",cacheHit,cacheMiss);
		return 0;
	}

	if(!u->m_szRegisteredUserName.isEmpty() && u->m_szLastRegisteredMatchNick==nick)
	{
		pUser = g_pRegisteredUserDataBase->getUser(u->m_szRegisteredUserName);
		//if(pUser) cacheHit++;
	}

	if(!pUser) {
		//user renamed or it is a first loockup
		if(u->hasHost() && u->hasUser())
		{
			pUser=g_pRegisteredUserDataBase->findMatchingUser(nick,u->user(),u->host());
			//cacheMiss++;
			if(pUser) {
				u->m_szLastRegisteredMatchNick=nick;
				u->m_szRegisteredUserName=pUser->name();

				u->m_bUseCustomColor=pUser->getBoolProperty("useCustomColor");
				QString szTmp=pUser->getProperty("customColor");
				KviStringConversion::fromString(szTmp,u->m_cachedColor);

				u->m_bNotFoundRegUserLoockup=false; //to be shure
			} else {
				u->m_szLastRegisteredMatchNick=nick;
				u->m_bNotFoundRegUserLoockup=true;
			}
		}
	}

//	debug("cache hits/miss = %i/%i",cacheHit,cacheMiss);
	return pUser;
}

KviRegisteredUser* KviIrcUserDataBase::registeredUser(const QString & nick)
{
	if(nick.isEmpty()) return 0;
	KviIrcUserEntry *u = find(nick);
	if(!u) return 0;
	return registeredUser(nick,u->user(),u->host());
}

void KviIrcUserDataBase::clear()
{
	delete m_pDict;
	m_pDict = new KviPointerHashTable<QString,KviIrcUserEntry>(4001,false);
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

void KviIrcUserDataBase::setupConnectionWithReguserDb()
{
	connect(g_pRegisteredUserDataBase,SIGNAL(userRemoved(const QString&)),this,SLOT(registeredUserRemoved(const QString&)));
	connect(g_pRegisteredUserDataBase,SIGNAL(userChanged(const QString&)),this,SLOT(registeredUserChanged(const QString&)));
	connect(g_pRegisteredUserDataBase,SIGNAL(userAdded(const QString&)),this,SLOT(registeredUserAdded(const QString&)));
	connect(g_pRegisteredUserDataBase,SIGNAL(databaseCleared()),this,SLOT(registeredDatabaseCleared()));
}

void KviIrcUserDataBase::registeredUserRemoved(const QString & user)
{
	KviPointerHashTableIterator<QString,KviIrcUserEntry> it( *m_pDict );
	for( ; it.current(); ++it )
	{
		if(it.current()->m_szRegisteredUserName==user)
		{
			it.current()->m_szRegisteredUserName="";
			it.current()->m_bNotFoundRegUserLoockup=false;
		}
	}
}

void KviIrcUserDataBase::registeredUserChanged(const QString & user)
{
	//the same as above
	KviPointerHashTableIterator<QString,KviIrcUserEntry> it( *m_pDict );
	for( ; it.current(); ++it )
	{
		if(it.current()->m_szRegisteredUserName==user)
		{
			it.current()->m_szRegisteredUserName="";
			it.current()->m_bNotFoundRegUserLoockup=false;
		}
	}
}

void KviIrcUserDataBase::registeredUserAdded(const QString &)
{
	KviPointerHashTableIterator<QString,KviIrcUserEntry> it( *m_pDict );
	for( ; it.current(); ++it )
	{
		if(it.current()->m_szRegisteredUserName.isEmpty())
		{
			it.current()->m_bNotFoundRegUserLoockup=false;
		}
	}
}

void KviIrcUserDataBase::registeredDatabaseCleared()
{
	KviPointerHashTableIterator<QString,KviIrcUserEntry> it( *m_pDict );
	for( ; it.current(); ++it )
	{
		it.current()->m_szRegisteredUserName="";
		it.current()->m_bNotFoundRegUserLoockup=false;
	}
}
