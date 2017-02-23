#ifndef _KVI_USERIDENTITY_H_
#define _KVI_USERIDENTITY_H_
//=============================================================================
//
//   File : KviUserIdentity.h
//   Creation date : Sun 21 Jan 2007 04:31:47 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2007-2010 Szymon Stefanek <pragma at kvirc dot net>
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
* \file KviUserIdentity.h
* \author Szymon Stefanek
* \brief User identity handling
*/
#include "kvi_settings.h"
#include "KviHeapObject.h"
#include "KviPixmap.h"

#include <QString>

class KviConfigurationFile;

/**
* \class KviUserIdentity
* \brief User identity handling class
*/
class KVILIB_API KviUserIdentity : public KviHeapObject
{
	friend class KviUserIdentityManager;

public:
	/**
	* \brief Constructs the user identity object
	* \return KviUserIdentity
	*/
	KviUserIdentity();

	/**
	* \brief Destroys the user identity object
	*/
	~KviUserIdentity();

protected:
	QString m_szId; // the identity set name

	QString m_szNickName;

	QString m_szAltNickName1;
	QString m_szAltNickName2;
	QString m_szAltNickName3;

	QString m_szUserName;
	QString m_szRealName;
	QString m_szPassword;

	KviPixmap m_pixAvatar;

	QString m_szPartMessage;
	QString m_szQuitMessage;

	QString m_szAge;
	QString m_szGender;
	QString m_szLocation;
	QString m_szLanguages;
	QString m_szOtherInfo;

	QString m_szUserMode;

	QString m_szOnConnectCommand;
	QString m_szOnLoginCommand;

public:
	/**
	* \brief Returns the id of the user
	* \return const QString &
	*/
	const QString & id() const { return m_szId; };

	/**
	* \brief Returns the nickname of the user
	* \return const QString &
	*/
	const QString & nickName() const { return m_szNickName; };

	/**
	* \brief Returns the alternative nickname of the user
	* \return const QString &
	*/
	const QString & altNickName1() const { return m_szAltNickName1; };

	/**
	* \brief Returns the second alternative nickname of the user
	* \return const QString &
	*/
	const QString & altNickName2() const { return m_szAltNickName2; };

	/**
	* \brief Returns the third alternative nickanem of the user
	* \return const QString &
	*/
	const QString & altNickName3() const { return m_szAltNickName3; };

	/**
	* \brief Returns the username of the user
	* \return const QString &
	*/
	const QString & userName() const { return m_szUserName; };

	/**
	* \brief Returns the password of the user
	* \return const QString &
	*/
	const QString & password() const { return m_szPassword; };

	/**
	* \brief Returns the avatar of the user
	* \return KviPixmap
	*/
	const KviPixmap & avatar() const { return m_pixAvatar; };

	/**
	* \brief Returns the part message of the user
	* \return const QString &
	*/
	const QString & partMessage() const { return m_szPartMessage; };

	/**
	* \brief Returns the quit message of the user
	* \return const QString &
	*/
	const QString & quitMessage() const { return m_szQuitMessage; };

	/**
	* \brief Returns the age of the user
	* \return const QString &
	*/
	const QString & age() const { return m_szAge; };

	/**
	* \brief Returns the gender of the user
	* \return const QString &
	*/
	const QString & gender() const { return m_szGender; };

	/**
	* \brief Returns the location of the user
	* \return const QString &
	*/
	const QString & location() const { return m_szLocation; };

	/**
	* \brief Returns the languages of the user
	* \return const QString &
	*/
	const QString & languages() const { return m_szLanguages; };

	/**
	* \brief Returns the other info field of the user
	* \return const QString &
	*/
	const QString & otherInfo() const { return m_szOtherInfo; };

	/**
	* \brief Returns the user mode of the user
	* \return const QString &
	*/
	const QString & userMode() const { return m_szUserMode; };

	/**
	* \brief Returns the list of commands to run on connection
	* \return const QString &
	*/
	const QString & onConnectCommand() const { return m_szOnConnectCommand; };

	/**
	* \brief Returns the list of commands to run on login
	* \return const QString &
	*/
	const QString & onLoginCommand() const { return m_szOnLoginCommand; };

	/**
	* \brief Sets the id of the user
	* \param szId The id of the user
	* \return void
	*/
	void setId(const QString & szId) { m_szId = szId; };

	/**
	* \brief Sets the nickname of the user
	* \param szNickName The nickname of the user
	* \return void
	*/
	void setNickName(const QString & szNickName) { m_szNickName = szNickName; };

	/**
	* \brief Sets the alternative nickname of the user
	* \param szNickName The nickname of the user
	* \return void
	*/
	void setAltNickName1(const QString & szNickName) { m_szAltNickName1 = szNickName; };

	/**
	* \brief Sets the second alternative nickname of the user
	* \param szNickName The nickname of the user
	* \return void
	*/
	void setAltNickName2(const QString & szNickName) { m_szAltNickName2 = szNickName; };

	/**
	* \brief Sets the third alternative nickname of the user
	* \param szNickName The nickname of the user
	* \return void
	*/
	void setAltNickName3(const QString & szNickName) { m_szAltNickName3 = szNickName; };

	/**
	* \brief Sets the username of the user
	* \param szUserName The username of the user
	* \return void
	*/
	void setUserName(const QString & szUserName) { m_szUserName = szUserName; };

	/**
	* \brief Sets the realname of the user
	* \param szRealName The realname of the user
	* \return void
	*/
	void setRealName(const QString & szRealName) { m_szRealName = szRealName; };

	/**
	* \brief Sets the password of the user
	* \param szPassword The password of the user
	* \return void
	*/
	void setPassword(const QString & szPassword) { m_szPassword = szPassword; };

	/**
	* \brief Sets the avatar of the user
	* \param pix The avatar of the user
	* \return void
	*/
	void setAvatar(const KviPixmap & pix) { m_pixAvatar = pix; };

	/**
	* \brief Sets the part messaege of the user
	* \param szMsg The part message of the user
	* \return void
	*/
	void setPartMessage(const QString & szMsg) { m_szPartMessage = szMsg; };

	/**
	* \brief Sets the quit messaege of the user
	* \param szMsg The quit message of the user
	* \return void
	*/
	void setQuitMessage(const QString & szMsg) { m_szQuitMessage = szMsg; };

	/**
	* \brief Sets the age of the user
	* \param szAge The age of the user
	* \return void
	*/
	void setAge(const QString & szAge) { m_szAge = szAge; };

	/**
	* \brief Sets the gender of the user
	* \param szGender The gemder of the user
	* \return void
	*/
	void setGender(const QString & szGender) { m_szGender = szGender; };

	/**
	* \brief Sets the location of the user
	* \param szLocation The location of the user
	* \return void
	*/
	void setLocation(const QString & szLocation) { m_szLocation = szLocation; };

	/**
	* \brief Sets the languages of the user
	* \param szLanguages The languages of the user
	* \return void
	*/
	void setLanguages(const QString & szLanguages) { m_szLanguages = szLanguages; };

	/**
	* \brief Sets the other info for the user
	* \param szOtherInfo The other info of the user
	* \return void
	*/
	void setOtherInfo(const QString & szOtherInfo) { m_szOtherInfo = szOtherInfo; };

	/**
	* \brief Sets the user mode of the user
	* \param szUserMode The user mode of the user
	* \return void
	*/
	void setUserMode(const QString & szUserMode) { m_szUserMode = szUserMode; };

	/**
	* \brief Sets the commands list to run on connection
	* \param szOnConnectCommand The commands to run
	* \return void
	*/
	void setOnConnectCommand(const QString & szOnConnectCommand) { m_szOnConnectCommand = szOnConnectCommand; };

	/**
	* \brief Sets the commands list to run on login
	* \param szOnLoginCommand The commands to run
	* \return void
	*/
	void setOnLoginCommand(const QString & szOnLoginCommand) { m_szOnLoginCommand = szOnLoginCommand; };
protected:
	/**
	* \brief Carbon copy
	* \param src The source user identity
	* \return void
	*/
	void copyFrom(const KviUserIdentity & src);

	/**
	* \brief Saves the user identity
	* \param cfg The config file where to save
	* \return bool
	*/
	bool save(KviConfigurationFile & cfg);

	/**
	* \brief Loads the user identity
	* \param cfg The config file where to load
	* \return bool
	*/
	bool load(KviConfigurationFile & cfg);
};

#endif // _KVI_USERIDENTITY_H_
