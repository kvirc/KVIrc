#ifndef _KVI_IDENTITYPROFILES_H_
#define _KVI_IDENTITYPROFILES_H_
//=============================================================================
//
//   File : kvi_identityprofile.h
//   Creation date : Thu Oct 16 2008 16:37:48 by Elvio Basello
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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
* \file kvi_identityprofile.h
* \author Elvio Basello
* \brief Identity profiles handling
*/

#include "kvi_settings.h"
#include "kvi_qstring.h"
#include "kvi_heapobject.h"
#include "kvi_pointerlist.h"

class KviConfig;

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
	void setName(const QString & szName){ m_szName = szName; };

	/**
	* \brief Sets the network of the profile
	* \param szNetwork The network of the profile
	* \return void
	*/
	void setNetwork(const QString & szNetwork){ m_szNetwork = szNetwork; };

	/**
	* \brief Sets the nickname of the profile
	* \param szNick The nickname of the profile
	* \return void
	*/
	void setNick(const QString & szNick){ m_szNick = szNick; };

	/**
	* \brief Sets the alternative nickname of the profile
	* \param szAltNick The alternative nickname of the profile
	* \return void
	*/
	void setAltNick(const QString & szAltNick){ m_szAltNick = szAltNick; };

	/**
	* \brief Sets the username of the profile
	* \param szUserName The username of the profile
	* \return void
	*/
	void setUserName(const QString & szUserName){ m_szUserName = szUserName; };

	/**
	* \brief Sets the realname of the profile
	* \param szRealName The realname of the profile
	* \return void
	*/
	void setRealName(const QString & szRealName){ m_szRealName = szRealName; };

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
	void save(KviConfig * pCfg, const QString & szPrefix);

	/**
	* \brief Loads the configuration from file.
	*
	* Returns false if the loaded data has no sense
	* \param pCfg The configuration file
	* \param szPrefix The prefix to use
	* \return bool
	*/
	bool load(KviConfig * pCfg, const QString & szPrefix);
};

/**
* \class KviIdentityProfileSet
* \brief Class which manages the list of identity profiles
*/
class KVILIB_API KviIdentityProfileSet : public KviHeapObject
{
public:
	/**
	* \brief Constructs the identity profile set object
	* \return KviIdentityProfileSet
	*/
	KviIdentityProfileSet();

	/**
	* \brief Constructs the identity profile set object
	*
	* This is a carbon copy.
	* \param set A profile set to copy from
	* \return KviIdentityProfileSet
	*/
	KviIdentityProfileSet(const KviIdentityProfileSet & set);

	/**
	* \brief Destroys the identity profile set object
	*/
	~KviIdentityProfileSet();
private:
	static KviIdentityProfileSet * m_pSelf;
	static unsigned int            m_uCount;
protected:
	KviPointerList<KviIdentityProfile> * m_pProfiles;
	bool                                 m_bEnabled;
public:
	/**
	* \brief Initializes the class instance
	* \return void
	*/
	static void init();

	/**
	* \brief Destroys the class instance
	* \return void
	*/
	static void done();

	/**
	* \brief Returns the instance of the class
	* \return KviIdentityProfileSet *
	*/
	static inline KviIdentityProfileSet * instance(){ return m_pSelf; };

	/**
	* \brief Returns the number of instances of the class
	* \return unsigned int
	*/
	unsigned int count(){ return m_uCount; };

	/**
	* \brief Returns the profiles set
	* \return KviPointerList<KviIdentityProfile> *
	*/
	KviPointerList<KviIdentityProfile> * profiles(){ return m_pProfiles; };

	/**
	* \brief Searches for a profile name. Returns true if the profile exists
	* \param szName The name of the profile
	* \return KviIdentityProfile *
	*/
	KviIdentityProfile * findName(const QString & szName);

	/**
	* \brief Searches for a profile network. Returns true if the profile exists
	* \param szNetwork The network name of the profile
	* \return KviIdentityProfile *
	*/
	KviIdentityProfile * findNetwork(const QString & szNetwork);

	/**
	* \brief Clears the list
	* \return void
	*/
	void clear();

	/**
	* \brief Returns true if the profile set is empty
	* \return bool
	*/
	bool isEmpty(){ return m_pProfiles ? m_pProfiles->isEmpty() : true; };

	/**
	* \brief Returns true if the profile set is enabled
	* \return bool
	*/
	bool isEnabled(){ return m_bEnabled; };

	/**
	* \brief Enables the profile set
	* \param bEnabled Whether to enable the profile set
	* \return void
	*/
	void setEnabled(bool bEnabled){ m_bEnabled = bEnabled; };

	/**
	* \brief Carbon copy
	* \param src The source profile set
	* \return void
	*/
	void copyFrom(const KviIdentityProfileSet & src);

	/**
	* \brief Adds a profile to the set
	* \param pProfile The source profile to add
	* \return void
	*/
	void addProfile(KviIdentityProfile * pProfile);

	/**
	* \brief Loads the configuration of the profiles from file
	* \param szConfigFile The file where to load
	* \return void
	*/
	void load(const QString & szConfigFile);

	/**
	* \brief Saves the configuration of the profiles to file
	* \param szConfigFile The file to save
	* \return void
	*/
	void save(const QString & szConfigFile);

	/**
	* \brief Saves the configuration of the profiles
	* \param pCfg The configuration file
	* \param szPrefix The prefix of the rules
	* \return void
	*/
	void save(KviConfig * pCfg, const QString & szPrefix);
protected:
	/**
	* \brief Loads the configuration of the profiles from file
	* \param pCfg The configuration file
	* \param szPrefix The prefix of the rules
	* \param uEntries The number of entries
	* \return bool
	*/
	bool loadPrivate(KviConfig * pCfg, const QString & szPrefix, unsigned int uEntries);
};

#endif // _KVI_IDENTITYPROFILES_H_
