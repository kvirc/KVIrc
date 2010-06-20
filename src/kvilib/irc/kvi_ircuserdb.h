#ifndef _KVI_IRCUSERDB_H_
#define _KVI_IRCUSERDB_H_

//=============================================================================
//
//   File : kvi_ircuserdb.h
//   Creation date : Mon Jul 31 2000 20:59:12 by Szymon Stefanek
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

/**
* \file kvi_ircuserdb.h
* \author Szymon Stefanek
* \brief User DB handling
*
* The defines are ordered from the least to the most powerful. All the flags are
* usermodes.
*
* \def KVI_USERFLAG_USEROP Defines the userop user
* \def KVI_USERFLAG_VOICE Defines the voice user
* \def KVI_USERFLAG_HALFOP Defines the halfop user
* \def KVI_USERFLAG_OP Defines the op user
* \def KVI_USERFLAG_CHANADMIN Defines the chanadmin user
* \def KVI_USERFLAG_CHANOWNER Defines the chanowner user
* \def KVI_USERFLAG_MODEMASK Defines the flags for the user
*/

#include "kvi_settings.h"
#include "kvi_pointerhashtable.h"
#include "kvi_string.h"
#include "kvi_avatar.h"
#include "kvi_regusersdb.h"

class KviIrcUserDataBase;

#define KVI_USERFLAG_USEROP 1
#define KVI_USERFLAG_VOICE 2
#define KVI_USERFLAG_HALFOP 4
#define KVI_USERFLAG_OP 8
#define KVI_USERFLAG_CHANADMIN 16
#define KVI_USERFLAG_CHANOWNER 32

#define KVI_USERFLAG_MODEMASK (KVI_USERFLAG_USEROP | KVI_USERFLAG_VOICE | KVI_USERFLAG_HALFOP | KVI_USERFLAG_OP | KVI_USERFLAG_CHANADMIN | KVI_USERFLAG_CHANOWNER)

/**
* \class KviIrcUserEntry
* \brief A class to handle the entries of the user database
*/
class KVILIB_API KviIrcUserEntry
{
	friend class KviIrcUserDataBase;
public:
	/**
	* \brief Constructs the entry object
	* \param user The user to add
	* \param host The host of the user
	* \return KviIrcUserEntry
	*/
	KviIrcUserEntry(const QString & user, const QString & host);

	/**
	* \brief Destroys the object
	*/
	~KviIrcUserEntry();

	/** \enum Gender */
	enum Gender {
		Male = 0, /**< Male */
		Female = 1, /**< Female */
		Unknown = 3 /**< Unknown */
	};
protected:
	QString     m_szUser;
	QString     m_szHost;

	QString     m_szServer;
	QString     m_szRealName;
	int         m_iHops;
	Gender      m_eGender;
	QString     m_szUserFlags;
	
	bool        m_bAway;
	bool        m_bIrcOp;

	KviAvatar * m_pAvatar;

	int         m_nRefs;
	bool        m_bBot;
	bool        m_bAvatarRequested;

	bool        m_bNotFoundRegUserLoockup;
	QString     m_szRegisteredUserName;
	QString     m_szLastRegisteredMatchNick;

	QColor      m_cachedColor;
	bool        m_bUseCustomColor;

	int         m_iSmartNickColor;
public:
	/**
	* \brief Returns the ircview smart nick color of the user
	* \return int
	*/
	int smartNickColor() { return m_iSmartNickColor; };

	/**
	* \brief Sets the ircview smart nick color of the user
	* \param iColor The color value
	* \return void
	*/
	void setSmartNickColor(const int iColor) { m_iSmartNickColor=iColor; };

	/**
	* \brief Returns the gender of the user
	* \return Gender
	*/
	Gender gender() { return m_eGender; };

	/**
	* \brief Sets the user as a bot
	* \param bIsBot If the user is a bot
	* \return void
	*/
	void setBot(bool bIsBot) { m_bBot = bIsBot; };

	/**
	* \brief Sets the gender of the user
	* \param g The gender
	* \return void
	*/
	void setGender(Gender g) { m_eGender=g; };

	/**
	* \brief Sets the username of the user
	* \param user The username
	* \return void
	*/
	void setUser(const QString & user) { m_szUser = user; };

	/**
	* \brief Sets the server of the user
	* \param serv The server
	* \return void
	*/
	void setServer(const QString & serv) { m_szServer = serv; };

	/**
	* \brief Sets the realname of the user
	* \param rn The realname
	* \return void
	*/
	void setRealName(const QString & rn);

	/**
	* \brief Sets the number of hops of the user
	* \param hops The number of hops
	* \return void
	*/
	void setHops(int hops) { m_iHops = hops; };

	/**
	* \brief Sets the hostname of the user
	* \param host The hostname
	* \return void
	*/
	void setHost(const QString & host) { m_szHost = host; };

	/**
	* \brief Sets the away state of the user
	* \param bAway The away state
	* \return void
	*/
	void setAway(bool bAway) { m_bAway = bAway; };

	/**
	* \brief Sets the irc operator state of the user
	* \param bIrcOp The ircop state
	* \return void
	*/
	void setIrcOp(bool bIrcOp) { m_bIrcOp = bIrcOp; };

	/**
	* \brief Sets the avatar of the user
	* \param av The avatar
	* \return void
	* \warning The ownership passes to this class!
	*/
	void setAvatar(KviAvatar * av = 0);

	/**
	* \brief Sets the user global flags (eg: "G*")
	* \param userFlags
	* \return void
	*/
	void setUserFlags(const QString & userFlags) { m_szUserFlags = userFlags; };
	
	/**
	* \brief Returns true if the user is marked as a bot
	* \return bool
	*/
	bool isBot() { return m_bBot; };

	/**
	* \brief Returns true if the user is in away mode
	* \return bool
	*/
	bool isAway() const { return m_bAway; };

	/**
	* \brief Returns true if the user is an irc operator
	* \return bool
	*/
	bool isIrcOp() const { return m_bIrcOp; };

	/**
	* \brief Returns true if the user has the username set
	* \return bool
	*/
	bool hasUser() { return (!m_szUser.isEmpty()); };

	/**
	* \brief Returns true if the user has the hostname set
	* \return bool
	*/
	bool hasHost() { return (!m_szHost.isEmpty()); };

	/**
	* \brief Returns true if the user has the server set
	* \return bool
	*/
	bool hasServer() { return (!m_szServer.isEmpty()); };

	/**
	* \brief Returns true if the user has the realname set
	* \return bool
	*/
	bool hasRealName() { return (!m_szRealName.isEmpty()); };

	/**
	* \brief Returns true if the user has hops set
	* \return bool
	*/
	bool hasHops() { return m_iHops >= 0; };

	/**
	* \brief Returns the username of the user
	* \return const QString &
	*/
	const QString & user() { return m_szUser; };

	/**
	* \brief Returns the hostname of the user
	* \return const QString &
	*/
	const QString & host() { return m_szHost; };

	/**
	* \brief Returns the server of the user
	* \return const QString &
	*/
	const QString & server() { return m_szServer; };

	/**
	* \brief Returns the realname of the user
	* \return const QString &
	*/
	const QString & realName() { return m_szRealName; };

	/**
	* \brief Returns the user flags
	* \return const QString &
	*/
	const QString & userFlags() { return m_szUserFlags; };

	/**
	* \brief Returns the number of hops
	* \return int
	*/
	int hops() { return m_iHops; };

	/**
	* \brief Returns the avatar of the user
	* \return KviAvatar
	*/
	KviAvatar * avatar() { return m_pAvatar; };

	/**
	* \brief Returns the number of references of the user in the database
	* \return int
	*/
	int nRefs() { return m_nRefs; };

	/**
	* \brief Removes the avatar associated to the user
	* \return KviAvatar *
	*/
	KviAvatar * forgetAvatar();

	/**
	* \brief Returns true if kvirc already made an attempt to download user's avatar
	* \return bool
	*/
	bool avatarRequested() const { return m_bAvatarRequested; };

	/**
	* \brief Mark that kvirc is trying (already tried) to get user's avatar
	*/
	void setAvatarRequested() { m_bAvatarRequested=true; };

};

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
	KviPointerHashTable<QString,KviIrcUserEntry> * m_pDict;
public:
	/**
	* \brief Clears the database
	* \return void
	*/
	void clear();

	/**
	* \brief Adds the user to the database
	* \param nick The nickname of the user
	* \param user The username of the user
	* \param hostname The hostname of the user
	* \return KviIrcUserEntry *
	*/
	KviIrcUserEntry * insertUser(const QString & nick, const QString & user, const QString & hostname);

	/**
	* \brief Searches for a user in the database
	* \param nick The nickname of the user to find
	* \return KviIrcUserEntry *
	*/
	KviIrcUserEntry * find(const QString & nick){ return m_pDict->find(nick); };

	/**
	* \brief Decrements the user reference count and if it reaches 0 then deletes the user from the database
	* \param nick The nickname of the user
	* \param e The entry of the user
	* \return true if the reference count reached 0 and false otherwise (so true if the user was completely deleted from the db)
	*/
	bool removeUser(const QString & nick, KviIrcUserEntry * e);

	/**
	* \brief Returns the database dictionary
	* \return KviPointerHashTable<QString,KviIrcUserEntry> *
	*/
	KviPointerHashTable<QString,KviIrcUserEntry> * dict(){ return m_pDict; };

	/**
	* \brief Returns the registered user, if any, or 0
	* \param nick The nickname of the user
	* \return KviRegisteredUser *
	*/
	KviRegisteredUser * registeredUser(const QString & nick);

	/**
	* \brief Returns the registered user, if any, or 0
	* \param nick The nickname of the user
	* \param user The username of the user
	* \param host The hostname of the user
	* \return KviRegisteredUser *
	*/
	KviRegisteredUser * registeredUser(const QString & nick, const QString & user, const QString & host);

	/**
	* \brief Returns true if the user has custom colors set
	* \param nick The nickname of the user
	* \return bool
	*/
	bool haveCustomColor(const QString & nick);

	/**
	* \brief Returns the custom color of the user
	* \param nick The nickname of the user
	* \return QColor *
	*/
	QColor * customColor(const QString & nick);

	/**
	* \brief Initialize the database by connecting signals and slots
	* \return void
	*/
	void setupConnectionWithReguserDb();
protected slots:
	/**
	* \brief Slot called when a registered user is removed
	* \param szUser The user removed
	* \return void
	*/
	void registeredUserRemoved(const QString & szUser);

	/**
	* \brief Slot called when a registered user is changed
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
