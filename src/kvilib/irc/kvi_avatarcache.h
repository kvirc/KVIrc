#ifndef _KVI_AVATARCACHE_H_
#define _KVI_AVATARCACHE_H_
//=============================================================================
//
//   File : kvi_avatarcache.h
//   Created on Sat 27 Dec 2003 21:19:47 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_settings.h"
#include "kvi_qstring.h"
#include "kvi_time.h"
#include "kvi_ircmask.h"

#include "kvi_pointerhashtable.h"


typedef struct _KviAvatarCacheEntry
{
	QString    szIdString;
	kvi_time_t tLastAccess;
} KviAvatarCacheEntry;



class KVILIB_API KviAvatarCache
{
protected:
	KviAvatarCache();
	~KviAvatarCache();
public:
	KviPointerHashTable<QString,KviAvatarCacheEntry> * m_pAvatarDict;
protected:
	static KviAvatarCache * m_pAvatarCacheInstance;
public:
	static void init();
	static void done();
	
	static KviAvatarCache * instance(){ return m_pAvatarCacheInstance; };

	void replace(const QString &szIdString,const KviIrcMask &mask,const QString &szNetwork);
	void remove(const KviIrcMask &mask,const QString &szNetwork);
	const QString & lookup(const KviIrcMask &mask,const QString &szNetwork);
	
	void cleanup();
	
	void load(const QString &szFileName);
	void save(const QString &szFileName);
};


#endif //!_KVI_AVATARCACHE_H_
