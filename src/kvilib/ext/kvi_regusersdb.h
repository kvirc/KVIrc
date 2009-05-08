#ifndef _KVI_REGUSERSDB_H_
#define _KVI_REGUSERSDB_H_
//=================================================================================================
//
//   File : kvi_regusersdb.h
//   Creation date : Sat Sep 09 2000 15:30:56 by Szymon Stefanek
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
//=================================================================================================

//
// REGISTERED USERS
//
//     Here we manage users resigered by mask and their (generic!) properties
//

#include "kvi_settings.h"
#include "kvi_heapobject.h"
#include "kvi_qstring.h"
#include "kvi_ircmask.h"
#include "kvi_debug.h"

#include "kvi_pointerlist.h"
#include "kvi_pointerhashtable.h"
#include <QObject>

class KviRegisteredUserDataBase;

#ifndef _KVI_REGUSERDB_CPP_
	extern KVILIB_API KviRegisteredUserDataBase * g_pRegisteredUserDataBase;
#endif //!_KVI_REGUSERDB_CPP_

//=================================================================================================
//
// KviRegisteredUser
//

class KVILIB_API KviRegisteredUser : public KviHeapObject
{
	friend class KviRegisteredUserDataBase;
public:
	enum IgnoreFlags {
		Channel=1,
		Query=2,
		Notice=4,
		Ctcp=8,
		Invite=16,
		Dcc=32
	};

	KviRegisteredUser(const QString &name);
	~KviRegisteredUser();
private:
	int                        m_iIgnoreFlags;
	bool                       m_bIgnoreEnabled;
	QString                    m_szName;
	QString					   m_szGroup;
	KviPointerHashTable<QString,QString>           * m_pPropertyDict;   // owned properties
	KviPointerList<KviIrcMask>     * m_pMaskList;       // owned masks
protected:
	// mask ownership is transferred! (always!) returns false if the mask was already there
	bool addMask(KviIrcMask * mask);
	bool removeMask(KviIrcMask * mask);
	KviIrcMask * findMask(const KviIrcMask &mask);
public:
	int  ignoreFlags()                  { return m_iIgnoreFlags; };
	void setIgnoreFlags(int flags)      {m_iIgnoreFlags=flags; };
	bool ignoreEnagled()                { return m_bIgnoreEnabled; };
	void setIgnoreEnabled(bool enabled) {m_bIgnoreEnabled=enabled;};
	bool isIgnoreEnabledFor(IgnoreFlags flag);

	const QString &name(){ return m_szName; };
	bool matches(const KviIrcMask &mask);
	bool matchesFixed(const KviIrcMask &mask);
	bool matchesFixed(const QString &nick,const QString &user,const QString &host);

	void setProperty(const QString &name,const QString &value);
	void setProperty(const QString &name,bool value);

	void setGroup(const QString &name) { m_szGroup=name; };
	const QString &group(){ return m_szGroup; };

	const QString & getProperty(const QString &name);       // returns 0 if the property is not there
	bool getProperty(const QString &name,QString &value); // returns false if the property is not there
	bool getBoolProperty(const QString &name,bool def=FALSE);           // returns true if the property is there and is true
	// the propertyDict may be 0!
	KviPointerHashTable<QString,QString> * propertyDict(){ return m_pPropertyDict; };
	// this is never zero (but may contain no masks)
	KviPointerList<KviIrcMask> * maskList(){ return m_pMaskList; };
};

//============================================================================================================
//
// KviRegisteredUserGroup
//

class KVILIB_API KviRegisteredUserGroup : public KviHeapObject
{
	friend class KviRegisteredUserDataBase;
public:
	KviRegisteredUserGroup(const QString &name);
	~KviRegisteredUserGroup();

	void setName(const QString &name) { m_szName=name; };
	const QString &name(){ return m_szName; };
private:
	QString                      m_szName;
};
//============================================================================================================
//
// KviRegisteredMask
//

class KVILIB_API KviRegisteredMask
{
private:
	KviRegisteredUser * m_pUser;               // pointer , not owned!
	KviIrcMask        * m_pMask;               // pointer , not owned!
	int                 m_iMaskNonWildChars;
public:
	KviRegisteredMask(KviRegisteredUser * u,KviIrcMask * m);
	~KviRegisteredMask(){};
public:
	int                 nonWildChars(){ return m_iMaskNonWildChars; };
	KviRegisteredUser * user(){ return m_pUser; };
	KviIrcMask        * mask(){ return m_pMask; };
};

typedef KviPointerList<KviRegisteredMask> KviRegisteredMaskList;

//=================================================================================================
//
// KviRegisteredUsersDb
//
//    Manages a set of KviRegisteredUser instances stored in the m_pUserDict dictionary
//    The users are identified by masks stored in m_pMaskDict and m_pWildMaskList
//    m_pMaskDict contains lists of non wild-nick KviRegisteredMask that point to users
//    m_pWildMaskList is a list of wild-nick KviRegisteredMask that point to users
//

class KVILIB_API KviRegisteredUserDataBase : public QObject
{
	Q_OBJECT
public:
	KviRegisteredUserDataBase();
	~KviRegisteredUserDataBase();
private:
	KviPointerHashTable<QString,KviRegisteredUser>     * m_pUserDict; // unique namespace, owns the objects, does not copy keys
	KviPointerHashTable<QString,KviRegisteredMaskList> * m_pMaskDict; // owns the objects, copies the keys
	KviRegisteredMaskList          * m_pWildMaskList; // owns the objects
	KviPointerHashTable<QString,KviRegisteredUserGroup>* m_pGroupDict;
public:
	void copyFrom(KviRegisteredUserDataBase * db);
	KviRegisteredUser * addUser(const QString &name); // returns 0 if already there
	KviRegisteredUser * getUser(const QString &name); // returns existing or adds
	bool removeUser(const QString &name);
	bool removeGroup(const QString &name);
	KviRegisteredUser * findUserByName(const QString &name){ return m_pUserDict->find(name); };
	// mask must be allocated on the heap and the ownership is transferred!
	// returns non zero if there is already an user with this mask (returns the pointer to it!)
	KviRegisteredUser * addMask(KviRegisteredUser * u,KviIrcMask * mask);
	bool removeMaskByPointer(KviIrcMask * mask);
	bool removeMask(const KviIrcMask &mask);
	KviRegisteredUser * findMatchingUser(const QString &nick,const QString &user,const QString &host);
	KviRegisteredUser * findUserWithMask(const KviIrcMask &mask);
	KviRegisteredMask * findExactMask(const KviIrcMask &mask);
	KviRegisteredMask * findMatchingMask(const QString &nick,const QString &user,const QString &host);
	//Only used in few places (actually one) of the code, but lot of times;perfect for inlining...
	//bool isIgnoredUser(const char * nick,const char * user,const char * host);
	void load(const QString &filename);
	void save(const QString &filename);

	KviPointerHashTable<QString,KviRegisteredUser> * userDict(){ return m_pUserDict; };
	KviPointerHashTable<QString,KviRegisteredUserGroup>* groupDict() { return m_pGroupDict; };

	KviRegisteredUserGroup* addGroup(const QString &name);
signals:
	void userRemoved(const QString&);
	void userChanged(const QString&);
	void userAdded  (const QString&);
	void databaseCleared();
};


#endif //_KVI_REGUSERSDB_H_
