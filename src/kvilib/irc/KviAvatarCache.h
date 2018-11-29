#ifndef _KVI_AVATARCACHE_H_
#define _KVI_AVATARCACHE_H_
//=============================================================================
//
//   File : KviAvatarCache.h
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

/**
* \file KviAvatarCache.h
* \author Szymon Stefanek
* \brief Avatar cache handling
*/

#include "kvi_settings.h"
#include "KviTimeUtils.h"
#include "KviPointerHashTable.h"

#include <QString>

class KviIrcMask;

/**
* \typedef KviAvatarCacheEntry
* \struct _KviAvatarCacheEntry
* \brief Defines a struct for the avatar entry in the cache
*/
struct KviAvatarCacheEntry
{
	QString szIdString;     /**< The id of the avatar */
	kvi_time_t tLastAccess; /**< The time the avatar was last accessed */
};

/**
* \class KviAvatarCache
* \brief Avatar cache handling class
*/
class KVILIB_API KviAvatarCache
{
protected:
	/**
	* \brief Constructs the avatar cache object
	* \return KviAvatarCache
	*/
	KviAvatarCache();

	/**
	* \brief Destroys the avatar cache object
	*/
	~KviAvatarCache();

public:
	KviPointerHashTable<QString, KviAvatarCacheEntry> * m_pAvatarDict;

protected:
	static KviAvatarCache * m_pAvatarCacheInstance;

public:
	/**
	* \brief Initializes the avatar cache
	* \return void
	*/
	static void init();

	/**
	* \brief Destroys the avatar cache
	* \return void
	*/
	static void done();

	/**
	* \brief Returns the instance of the avatar cache
	* \return KviAvatarCache *
	*/
	static KviAvatarCache * instance() { return m_pAvatarCacheInstance; };

	/**
	* \brief Replaces a cached avatar
	* \param szIdString The id of the avatar
	* \param mask The mask of the user
	* \param szNetwork The network where the user is on
	* \return void
	*/
	void replace(const QString & szIdString, const KviIrcMask & mask, const QString & szNetwork);

	/**
	* \brief Remove an avatar from the cache
	* \param mask The mask of the user
	* \param szNetwork The network where the user is on
	* \return void
	*/
	void remove(const KviIrcMask & mask, const QString & szNetwork);

	/**
	* \brief Search a user in the cache and returns the id of the avatar
	* \param mask The mask of the user
	* \param szNetwork The network where the user is on
	* \return const QString &
	*/
	const QString & lookup(const KviIrcMask & mask, const QString & szNetwork);

	/**
	* \brief Deletes the cache
	* \return void
	*/
	void cleanup();

	/**
	* \brief Loads the cache
	* \param szFileName The cache filename
	* \return void
	*/
	void load(const QString & szFileName);

	/**
	* \brief Saves the cache
	* \param szFileName The cache filename
	* \return void
	*/
	void save(const QString & szFileName);
};

#endif //_KVI_AVATARCACHE_H_
