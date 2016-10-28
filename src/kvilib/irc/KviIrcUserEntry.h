#ifndef _KVI_IRCUSER_ENTRY_H_
#define _KVI_IRCUSER_ENTRY_H_
//=============================================================================
//
//   File : KviIrcUserEntry.h
//   Creation date : Tue Jan 04 2010 22:45:12 by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
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
* \file KviIrcUserEntry.h
* \author Szymon Stefanek
* \brief User DB handling
*
* This file was originally part of KviIrcUserDataBase.h
*/

#include "kvi_settings.h"
#include "KviAvatar.h"

#include <memory>

/**
* \class KviIrcUserEntry
* \brief A class to handle the entries of the user database
*/
class KVILIB_API KviIrcUserEntry
{
	friend class KviIrcUserDataBase;

public:
	/**
	* \enum Gender
	* \brief Defines the gender of the user
	*/
	enum Gender
	{
		Male = 0,   /**< Male */
		Female = 1, /**< Female */
		Unknown = 3 /**< Unknown */
	};

	/**
	* \enum UserMode
	* \brief Defines the user mode in a channel
	*/
	enum UserMode
	{
		UserOp = 1,     /**< the user is a userop of the channel */
		Voice = 2,      /**< the user is a voice of the channel */
		HalfOp = 4,     /**< the user is a halfop of the channel */
		Op = 8,         /**< the user is an operator of the channel */
		ChanAdmin = 16, /**< the user is an admin of the channel */
		ChanOwner = 32, /**< the user is an owner of the channel */
		ModeMask = UserOp | Voice | HalfOp | Op | ChanAdmin | ChanOwner
	};

	/**
	* \brief Constructs the entry object
	* \param user The user to add
	* \param host The host of the user
	* \return KviIrcUserEntry
	*/
	KviIrcUserEntry(const QString & user, const QString & host);

protected:
	QString m_szUser;
	QString m_szHost;

	QString m_szServer;
	QString m_szRealName;
	int m_iHops;
	Gender m_eGender;
	QString m_szUserFlags;

	bool m_bAway;
	bool m_bIrcOp;

	std::unique_ptr<KviAvatar> m_upAvatar;

	int m_nRefs;
	bool m_bBot;
	bool m_bAvatarRequested;

	bool m_bNotFoundRegUserLookup; //wtf?
	QString m_szRegisteredUserName;
	QString m_szLastRegisteredMatchNick;

	QColor m_cachedColor;
	bool m_bUseCustomColor;

	int m_iSmartNickColor;
	QString m_szAccountName;

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
	void setSmartNickColor(const int iColor) { m_iSmartNickColor = iColor; };

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
	void setGender(Gender g) { m_eGender = g; };

	/**
	* \brief Sets the username of the user
	* \param szUser The username
	* \return void
	*/
	void setUser(const QString & szUser) { m_szUser = szUser; };

	/**
	* \brief Sets the server of the user
	* \param szServ The server
	* \return void
	*/
	void setServer(const QString & szServ) { m_szServer = szServ; };

	/**
	* \brief Sets the realname of the user
	* \param szReal The realname
	* \return void
	*/
	void setRealName(const QString & szReal);

	/**
	* \brief Sets the number of hops of the user
	* \param iHops The number of hops
	* \return void
	*/
	void setHops(int iHops) { m_iHops = iHops; };

	/**
	* \brief Sets the hostname of the user
	* \param szHost The hostname
	* \return void
	*/
	void setHost(const QString & szHost) { m_szHost = szHost; };

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
	* \param upAvatar The avatar
	* \return void
	* \warning The ownership passes to this class!
	*/
	void setAvatar(std::unique_ptr<KviAvatar> upAvatar = nullptr);

	/**
	* \brief Sets the avatar of the user
	* \param pAvatar The avatar
	* \return void
	* \warning The ownership passes to this class!
	* \warning This is an overload function for backwards compatibility
	*/
	void setAvatar(KviAvatar * upAvatar = nullptr);

	/**
	* \brief Sets the user global flags (eg: "G*")
	* \param szFlags
	* \return void
	*/
	void setUserFlags(const QString & szFlags) { m_szUserFlags = szFlags; };

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
	KviAvatar * avatar() { return m_upAvatar.get(); };

	/**
	* \brief Returns the number of references of the user in the database
	* \return int
	*/
	int nRefs() { return m_nRefs; };

	/**
	* \brief Removes the avatar associated to the user
	* \return KviAvatar *
	*/
	std::unique_ptr<KviAvatar> forgetAvatar();

	/**
	* \brief Returns true if kvirc already made an attempt to download user's avatar
	* \return bool
	*/
	bool avatarRequested() const { return m_bAvatarRequested; };

	/**
	* \brief Mark that kvirc is trying (already tried) to get user's avatar
	*/
	void setAvatarRequested() { m_bAvatarRequested = true; };

	/**
	* \brief Returns the account name (if any)
	* \return const QString &
	*/
	const QString & accountName() { return m_szAccountName; };

	/**
	* \brief Sets the account name for the user
	*/
	void setAccountName(const QString & szAccountName) { m_szAccountName = szAccountName; };

	/**
	* \brief Returns true if the user has an account name
	* \return bool
	*/
	bool hasAccountName() { return (!m_szAccountName.isEmpty()); };
};

#endif // _KVI_IRCUSER_ENTRY_H_
