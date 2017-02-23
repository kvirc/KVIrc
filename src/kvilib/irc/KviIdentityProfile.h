#ifndef _KVI_IDENTITY_PROFILE_H_
#define _KVI_IDENTITY_PROFILE_H_
//=============================================================================
//
//   File : KviIdentityProfile.h
//   Creation date : Thu Oct 16 2008 16:37:48 by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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
* \file KviIdentityProfile.h
* \author Elvio Basello
* \brief Identity profiles handling
*/

#include "kvi_settings.h"
#include "KviHeapObject.h"

#include <QString>

class KviConfigurationFile;

/**
* \class KviIdentityProfile
* \brief Class which manages the identity profile
*/
class KVILIB_API KviIdentityProfile : public KviHeapObject
{
public:
	/**
	* \brief Constructs the identity profile object
	* \return KviIdentityProfile
	*/
	KviIdentityProfile();

	/**
	* \brief Destroys the identity profile object
	* \return KviIdentityProfile
	*/
	~KviIdentityProfile();

protected:
	QString m_szName;
	QString m_szNetwork;
	QString m_szNick;
	QString m_szAltNick;
	QString m_szUserName;
	QString m_szRealName;

public:
	/**
	* \brief Returns the name of the profile
	* \return const QString &
	*/
	const QString & name() const { return m_szName; };

	/**
	* \brief Returns the network of the profile
	* \return const QString &
	*/
	const QString & network() const { return m_szNetwork; };

	/**
	* \brief Returns the nickname of the profile
	* \return const QString &
	*/
	const QString & nick() const { return m_szNick; };

	/**
	* \brief Returns the alternative nickname of the profile
	* \return const QString &
	*/
	const QString & altNick() const { return m_szAltNick; };

	/**
	* \brief Returns the username of the profile
	* \return const QString &
	*/
	const QString & userName() const { return m_szUserName; };

	/**
	* \brief Returns the realname of the profile
	* \return const QString &
	*/
	const QString & realName() const { return m_szRealName; };

	/**
	* \brief Sets the name of the profile
	* \param szName The name of the profile
	* \return void
	*/
	void setName(const QString & szName) { m_szName = szName; };

	/**
	* \brief Sets the network of the profile
	* \param szNetwork The network of the profile
	* \return void
	*/
	void setNetwork(const QString & szNetwork) { m_szNetwork = szNetwork; };

	/**
	* \brief Sets the nickname of the profile
	* \param szNick The nickname of the profile
	* \return void
	*/
	void setNick(const QString & szNick) { m_szNick = szNick; };

	/**
	* \brief Sets the alternative nickname of the profile
	* \param szAltNick The alternative nickname of the profile
	* \return void
	*/
	void setAltNick(const QString & szAltNick) { m_szAltNick = szAltNick; };

	/**
	* \brief Sets the username of the profile
	* \param szUserName The username of the profile
	* \return void
	*/
	void setUserName(const QString & szUserName) { m_szUserName = szUserName; };

	/**
	* \brief Sets the realname of the profile
	* \param szRealName The realname of the profile
	* \return void
	*/
	void setRealName(const QString & szRealName) { m_szRealName = szRealName; };

	/**
	* \brief Carbon copy
	* \param src The source profile
	* \return void
	*/
	void copyFrom(const KviIdentityProfile & src);

	/**
	* \brief Saves the configuration to file
	* \param pCfg The configuration file
	* \param szPrefix The prefix to use
	* \return void
	*/
	void save(KviConfigurationFile * pCfg, const QString & szPrefix);

	/**
	* \brief Loads the configuration from file.
	*
	* Returns false if the loaded data has no sense
	* \param pCfg The configuration file
	* \param szPrefix The prefix to use
	* \return bool
	*/
	bool load(KviConfigurationFile * pCfg, const QString & szPrefix);
};

#endif // _KVI_IDENTITY_PROFILE_H_
