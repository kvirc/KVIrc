#ifndef _KVI_IDENTITY_PROFILE_SET_H_
#define _KVI_IDENTITY_PROFILE_SET_H_
//=============================================================================
//
//   File : KviIdentityProfileSet.h
//   Creation date : Thu Dec 30 2010 15:54:48 by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Elvio Basello (hellvis69 at netsons dot org)
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
* \file KviIdentityProfileSet.h
* \author Elvio Basello
* \brief Identity profiles handling
*/

#include "kvi_settings.h"
#include "KviHeapObject.h"
#include "KviIdentityProfile.h"
#include "KviPointerList.h"

class KviConfigurationFile;

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
	static unsigned int m_uCount;

protected:
	KviPointerList<KviIdentityProfile> * m_pProfiles;
	bool m_bEnabled;

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
	static inline KviIdentityProfileSet * instance() { return m_pSelf; };

	/**
	* \brief Returns the number of instances of the class
	* \return unsigned int
	*/
	unsigned int count() { return m_uCount; };

	/**
	* \brief Returns the profiles set
	* \return KviPointerList<KviIdentityProfile> *
	*/
	KviPointerList<KviIdentityProfile> * profiles() { return m_pProfiles; };

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
	bool isEmpty() { return m_pProfiles ? m_pProfiles->isEmpty() : true; };

	/**
	* \brief Returns true if the profile set is enabled
	* \return bool
	*/
	bool isEnabled() { return m_bEnabled; };

	/**
	* \brief Enables the profile set
	* \param bEnabled Whether to enable the profile set
	* \return void
	*/
	void setEnabled(bool bEnabled) { m_bEnabled = bEnabled; };

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
	void save(KviConfigurationFile * pCfg, const QString & szPrefix);

protected:
	/**
	* \brief Loads the configuration of the profiles from file
	* \param pCfg The configuration file
	* \param szPrefix The prefix of the rules
	* \param uEntries The number of entries
	* \return bool
	*/
	bool loadPrivate(KviConfigurationFile * pCfg, const QString & szPrefix, unsigned int uEntries);
};

#endif // _KVI_IDENTITY_PROFILE_SET_H_
