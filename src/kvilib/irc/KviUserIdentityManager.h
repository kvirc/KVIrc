#ifndef _KVI_USERIDENTITY_MANAGER_H_
#define _KVI_USERIDENTITY_MANAGER_H_
//=============================================================================
//
//   File : KviUserIdentityManager.h
//   Creation date : Thu Dec Jan 2007 05:03:47 by Elvio Basello
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2010 Elvio Basello (hellvis69 at gmail dot com)
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
* \file KviUserIdentityManager.h
* \author Szymon Stefanek
* \brief User identity handling
*
* This file was originally part of KviUserIdentity.h
*/

#include "kvi_settings.h"
#include "KviHeapObject.h"
#include "KviPointerHashTable.h"

#include <QString>

class KviUserIdentity;

/**
* \class KviUserIdentityManager
* \brief The class to manage the user identity
*/
class KVILIB_API KviUserIdentityManager : public KviHeapObject
{
protected:
	/**
	* \brief Constructs the user identity manager object
	* \return KviUserIdentityManager
	*/
	KviUserIdentityManager();

	/**
	* \brief Destroys the user identity manager object
	*/
	~KviUserIdentityManager();

protected:
	static KviUserIdentityManager * m_pInstance;
	KviPointerHashTable<QString, KviUserIdentity> * m_pIdentityDict;
	QString m_szDefaultIdentity;

public:
	/**
	* \brief Initializes a new user identity
	* \return void
	*/
	static void init();

	/**
	* \brief Deletese the user identity
	* \return void
	*/
	static void done();

	/**
	* \brief Returns the instance of the identity
	* \returns KviUserIdentityManager *
	*/
	static KviUserIdentityManager * instance() { return m_pInstance; };

	/**
	* \brief Returns the identity dictionary
	* \returns KviPointerHashTable<QString,KviUserIdentity> *
	*/
	KviPointerHashTable<QString, KviUserIdentity> * identityDict() { return m_pIdentityDict; };

	/**
	* \brief Searches fot an identity
	* \param szId The identity id to find
	* \return const KviUserIdentity *
	*/
	const KviUserIdentity * findIdentity(const QString & szId) { return m_pIdentityDict->find(szId); };

	/**
	* \brief Returns the default identity
	* It's NEVER null.
	* \return const KviUserIdentity *
	*/
	const KviUserIdentity * defaultIdentity();

	/**
	* \brief Sets the default identity
	* \param szIdentityId The identity id to set
	* \return void
	*/
	void setDefaultIdentity(const QString & szIdentityId) { m_szDefaultIdentity = szIdentityId; };

	/**
	* \brief Creates a new working copy
	* \return KviUserIdentityManager *
	*/
	KviUserIdentityManager * createWorkingCopy();

	/**
	* \brief Carbon copy
	* \param pWorkingCopy The source working copy
	* \return void
	*/
	void copyFrom(KviUserIdentityManager * pWorkingCopy);

	/**
	* \brief Deletes the working copy
	* \param pWorkingCopy The source working copy
	* \return void
	*/
	void releaseWorkingCopy(KviUserIdentityManager * pWorkingCopy);

	/**
	* \brief Saves the user identity database
	* \param szFileName The filename where to save
	* \return void
	*/
	void save(const QString & szFileName);

	/**
	* \brief Loads the user identity database
	* \param szFileName The filename where to load
	* \return void
	*/
	void load(const QString & szFileName);
};

#endif // _KVI_USERIDENTITY_MANAGER_H_
