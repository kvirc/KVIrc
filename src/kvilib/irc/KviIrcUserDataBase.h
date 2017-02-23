#ifndef _KVI_IRCUSERDB_H_
#define _KVI_IRCUSERDB_H_
//=============================================================================
//
//   File : KviIrcUserDataBase.h
//   Creation date : Mon Jul 31 2000 20:59:12 by Szymon Stefanek
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

/**
* \file KviIrcUserDataBase.h
* \author Szymon Stefanek
* \brief User DB handling
*/

#include "kvi_settings.h"
#include "KviIrcUserEntry.h"
#include "KviPointerHashTable.h"

#include <QObject>
#include <QString>

class KviRegisteredUser;

/**
* \class KviIrcUserDataBase
* \brief The class which manages the user database
*/
class KVILIB_API KviIrcUserDataBase : public QObject
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the Irc user database object
	* \return KviIrcUserDataBase
	*/
	KviIrcUserDataBase();

	/**
	* \brief Destoys the object
	*/
	~KviIrcUserDataBase();

private:
	KviPointerHashTable<QString, KviIrcUserEntry> * m_pDict;

public:
	/**
	* \brief Clears the database
	* \return void
	*/
	void clear();

	/**
	* \brief Adds the user to the database
	* \param szNick The nickname of the user
	* \param szUser The username of the user
	* \param szHost The hostname of the user
	* \return KviIrcUserEntry *
	*/
	KviIrcUserEntry * insertUser(const QString & szNick, const QString & szUser, const QString & szHost);

	/**
	* \brief Searches for a user in the database
	* \param szNick The nickname of the user to find
	* \return KviIrcUserEntry *
	*/
	KviIrcUserEntry * find(const QString & szNick) { return m_pDict->find(szNick); };

	/**
	* \brief Decrements the user reference count and if it reaches 0 then deletes the user from the database
	* \param szNick The nickname of the user
	* \param pEntry The entry of the user
	* \return true if the reference count reached 0 and false otherwise (so true if the user was completely deleted from the db)
	*/
	bool removeUser(const QString & szNick, KviIrcUserEntry * pEntry);

	/**
	* \brief Returns the database dictionary
	* \return KviPointerHashTable<QString,KviIrcUserEntry> *
	*/
	KviPointerHashTable<QString, KviIrcUserEntry> * dict() { return m_pDict; };

	/**
	* \brief Returns the registered user, if any, or 0
	* \param szNick The nickname of the user
	* \return KviRegisteredUser *
	*/
	KviRegisteredUser * registeredUser(const QString & szNick);

	/**
	* \brief Returns the registered user, if any, or 0
	* \param szNick The nickname of the user
	* \param szUser The username of the user
	* \param szHost The hostname of the user
	* \return KviRegisteredUser *
	*/
	KviRegisteredUser * registeredUser(const QString & szNick, const QString & szUser, const QString & szHost);

	/**
	* \brief Returns true if the user has custom colors set
	* \param szNick The nickname of the user
	* \return bool
	*/
	bool haveCustomColor(const QString & szNick);

	/**
	* \brief Returns the custom color of the user
	* \param szNick The nickname of the user
	* \return QColor *
	*/
	QColor * customColor(const QString & szNick);

	/**
	* \brief Initialize the database by connecting signals and slots
	* \return void
	*/
	void setupConnectionWithReguserDb();
protected slots:
	/**
	* \brief Slot called when a registered user is changed or removed
	* \param szUser The user changed
	* \return void
	*/
	void registeredUserChanged(const QString & szUser);

	/**
	* \brief Slot called when a registered user is added
	* \param szUser The user added
	* \return void
	*/
	void registeredUserAdded(const QString & szUser);

	/**
	* \brief Slot called when the database is cleared
	* \return void
	*/
	void registeredDatabaseCleared();
};

#endif //_KVI_IRCUSERDB_H_
