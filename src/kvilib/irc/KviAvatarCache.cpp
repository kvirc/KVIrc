//=============================================================================
//
//   File : KviAvatarCache.cpp
//   Creation date : Sat 27 Dec 2003 21:19:47 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviAvatarCache.h"
#include "KviConfigurationFile.h"
#include "KviIrcMask.h"
#include "KviPointerList.h"
#include "KviQString.h"

// this level triggers a cleanup
#define MAX_AVATARS_IN_CACHE 100
// this is the level that has be reached by a cleanup
#define CACHE_GUARD_LEVEL 85
// dictionary size
#define CACHE_DICT_SIZE 101
// keep the unaccessed avatars for 30 days
#define MAX_UNACCESSED_TIME (3600 * 24 * 30)

KviAvatarCache * KviAvatarCache::m_pAvatarCacheInstance = nullptr;

void KviAvatarCache::init()
{
	if(m_pAvatarCacheInstance)
	{
		qDebug("WARNING: trying to initialize the avatar cache twice");
		return;
	}

	m_pAvatarCacheInstance = new KviAvatarCache();
}

void KviAvatarCache::done()
{
	if(!m_pAvatarCacheInstance)
	{
		qDebug("WARNING: trying to destroy an uninitialized avatar cache");
		return;
	}

	delete m_pAvatarCacheInstance;
	m_pAvatarCacheInstance = nullptr;
}

KviAvatarCache::KviAvatarCache()
{
	m_pAvatarDict = new KviPointerHashTable<QString, KviAvatarCacheEntry>(CACHE_DICT_SIZE, false);
	m_pAvatarDict->setAutoDelete(true);
}

KviAvatarCache::~KviAvatarCache()
{
	delete m_pAvatarDict;
}

void KviAvatarCache::replace(const QString & szIdString, const KviIrcMask & mask, const QString & szNetwork)
{
	QString szKey;

	mask.mask(szKey, KviIrcMask::NickCleanUserSmartNet);
	szKey.append(QChar('+'));
	szKey.append(szNetwork);

	KviAvatarCacheEntry * e = new KviAvatarCacheEntry;
	e->szIdString = szIdString;
	e->tLastAccess = kvi_unixTime();

	m_pAvatarDict->replace(szKey, e);

	if(m_pAvatarDict->count() > MAX_AVATARS_IN_CACHE)
	{
		cleanup();
	}
}

void KviAvatarCache::remove(const KviIrcMask & mask, const QString & szNetwork)
{
	QString szKey;

	mask.mask(szKey, KviIrcMask::NickCleanUserSmartNet);
	szKey.append(QChar('+'));
	szKey.append(szNetwork);

	m_pAvatarDict->remove(szKey);
}

const QString & KviAvatarCache::lookup(const KviIrcMask & mask, const QString & szNetwork)
{
	QString szKey;

	mask.mask(szKey, KviIrcMask::NickCleanUserSmartNet);
	szKey.append(QChar('+'));
	szKey.append(szNetwork);

	KviAvatarCacheEntry * e = m_pAvatarDict->find(szKey);
	if(!e)
		return KviQString::Empty;
	e->tLastAccess = kvi_unixTime();
	return e->szIdString;
}

void KviAvatarCache::load(const QString & szFileName)
{
	m_pAvatarDict->clear();

	KviConfigurationFile cfg(szFileName, KviConfigurationFile::Read);

	kvi_time_t tNow = kvi_unixTime();

	KviConfigurationFileIterator it(*(cfg.dict()));

	int cnt = 0;

	while(it.current())
	{
		cfg.setGroup(it.currentKey());

		kvi_time_t tLastAccess = cfg.readUIntEntry("LastAccess", 0);
		if((tNow - tLastAccess) < MAX_UNACCESSED_TIME)
		{
			QString szIdString = cfg.readEntry("Avatar", "");

			if(!szIdString.isEmpty())
			{
				KviAvatarCacheEntry * e = new KviAvatarCacheEntry;
				e->tLastAccess = tLastAccess;
				e->szIdString = szIdString;
				m_pAvatarDict->replace(it.currentKey(), e);
				cnt++;
				if(cnt >= MAX_AVATARS_IN_CACHE)
					return; // done
			}
		}
		++it;
	}
}

void KviAvatarCache::save(const QString & szFileName)
{
	KviConfigurationFile cfg(szFileName, KviConfigurationFile::Write);
	//	cfg.clear(); // not needed with KviConfigurationFile::Write

	KviPointerHashTableIterator<QString, KviAvatarCacheEntry> it(*m_pAvatarDict);

	while(KviAvatarCacheEntry * e = it.current())
	{
		if(e->tLastAccess)
		{
			cfg.setGroup(it.currentKey());
			cfg.writeEntry("Avatar", e->szIdString);
			cfg.writeEntry("LastAccess", ((unsigned int)(e->tLastAccess)));
		}
		++it;
	}
}

void KviAvatarCache::cleanup()
{
	// first do a quick run deleting the avatars really too old
	KviPointerHashTableIterator<QString, KviAvatarCacheEntry> it(*m_pAvatarDict);

	kvi_time_t tNow = kvi_unixTime();

	KviPointerList<QString> l;
	l.setAutoDelete(false);

	KviAvatarCacheEntry * e;

	while((e = it.current()))
	{
		if((tNow - e->tLastAccess) > MAX_UNACCESSED_TIME)
		{
			l.append(new QString(it.currentKey()));
		}
		++it;
	}

	for(QString * s = l.first(); s; s = l.next())
		m_pAvatarDict->remove(*s);

	if(m_pAvatarDict->count() < CACHE_GUARD_LEVEL)
		return;

	// not done.. need to kill the last accessed :/

	it.toFirst();

	KviPointerList<KviAvatarCacheEntry> ll;
	ll.setAutoDelete(true);

	// here we use the cache entries in another way
	// szAvatar is the KEY instead of the avatar name
	while((e = it.current()))
	{
		KviAvatarCacheEntry * current = ll.first();
		unsigned int idx = 0;
		while(current)
		{
			// if the current is newer than the inserted one
			// then stop searching and insert it just before
			if(current->tLastAccess > e->tLastAccess)
				break;
			// otherwise the current is older and the inserted
			// one goes after
			current = ll.next();
			idx++;
		}

		KviAvatarCacheEntry * xx = new KviAvatarCacheEntry;
		xx->szIdString = it.currentKey();
		xx->tLastAccess = e->tLastAccess;

		if(current)
			ll.insert(idx, xx);
		else
			ll.append(xx);
		++it;
	}

	// the oldest keys are at the beginning
	int uRemove = ll.count() - CACHE_GUARD_LEVEL;
	if(uRemove < 1)
		return; // huh ?

	// remember that szAvatar contains the key!
	for(e = ll.first(); e && (uRemove > 0); e = ll.next())
	{
		m_pAvatarDict->remove(e->szIdString);
		uRemove--;
	}
	// now we should be ok
}
