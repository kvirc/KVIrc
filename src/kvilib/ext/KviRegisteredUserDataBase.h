#ifndef _KVI_REGUSERSDB_H_
#define _KVI_REGUSERSDB_H_
//=============================================================================
//
//   File : KviRegisteredUserDataBase.h
//   Creation date : Sat Sep 09 2000 15:30:56 by Szymon Stefanek
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

//
// REGISTERED USERS
//
//     Here we manage users resigered by mask and their (generic!) properties
//

#include "kvi_settings.h"
#include "kvi_debug.h"

#include "KviPointerHashTable.h"
#include "KviRegisteredUserGroup.h"
#include "KviRegisteredUserMask.h"
#include "KviRegisteredUser.h"

#include <QObject>

class KviIrcMask;
class QString;

//
// KviRegisteredUserDataBase
//
//    Manages a set of KviRegisteredUser instances stored in the m_pUserDict dictionary
//    The users are identified by masks stored in m_pMaskDict and m_pWildMaskList
//    m_pMaskDict contains lists of non wild-nick KviRegisteredUserMask that point to users
//    m_pWildMaskList is a list of wild-nick KviRegisteredUserMask that point to users
//

class KVILIB_API KviRegisteredUserDataBase : public QObject
{
	Q_OBJECT
public:
	KviRegisteredUserDataBase();
	~KviRegisteredUserDataBase();

private:
	KviPointerHashTable<QString, KviRegisteredUser> * m_pUserDict;         // unique namespace, owns the objects, does not copy keys
	KviPointerHashTable<QString, KviRegisteredUserMaskList> * m_pMaskDict; // owns the objects, copies the keys
	KviRegisteredUserMaskList * m_pWildMaskList;                           // owns the objects
	KviPointerHashTable<QString, KviRegisteredUserGroup> * m_pGroupDict;

public:
	void copyFrom(KviRegisteredUserDataBase * db);
	KviRegisteredUser * addUser(const QString & name); // returns 0 if already there
	KviRegisteredUser * getUser(const QString & name); // returns existing or adds
	bool removeUser(const QString & name);
	bool removeGroup(const QString & name);
	KviRegisteredUser * findUserByName(const QString & name) { return m_pUserDict->find(name); };
	// mask must be allocated on the heap and the ownership is transferred!
	// returns non zero if there is already a user with this mask (returns the pointer to it!)
	KviRegisteredUser * addMask(KviRegisteredUser * u, KviIrcMask * mask);
	bool removeMaskByPointer(KviIrcMask * mask);
	bool removeMask(const KviIrcMask & mask);
	KviRegisteredUser * findMatchingUser(const QString & nick, const QString & user, const QString & host);
	KviRegisteredUser * findUserWithMask(const KviIrcMask & mask);
	KviRegisteredUserMask * findExactMask(const KviIrcMask & mask);
	KviRegisteredUserMask * findMatchingMask(const QString & nick, const QString & user, const QString & host);
	//Only used in few places (actually one) of the code, but lot of times;perfect for inlining...
	//bool isIgnoredUser(const char * nick,const char * user,const char * host);
	void load(const QString & filename);
	void save(const QString & filename);

	KviPointerHashTable<QString, KviRegisteredUser> * userDict() { return m_pUserDict; };
	KviPointerHashTable<QString, KviRegisteredUserGroup> * groupDict() { return m_pGroupDict; };

	KviRegisteredUserGroup * addGroup(const QString & name);
signals:
	void userRemoved(const QString &);
	void userChanged(const QString &);
	void userAdded(const QString &);
	void databaseCleared();
};

#ifndef _KVI_REGUSERDB_CPP_
extern KVILIB_API KviRegisteredUserDataBase * g_pRegisteredUserDataBase;
#endif // _KVI_REGUSERDB_CPP_

#endif //_KVI_REGUSERSDB_H_
