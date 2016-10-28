//=============================================================================
//
//   File : KviUserIdentityManager.cpp
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

#include "KviUserIdentityManager.h"
#include "KviConfigurationFile.h"
#include "KviLocale.h"
#include "KviUserIdentity.h"
#include "kvi_defaults.h"

KviUserIdentityManager * KviUserIdentityManager::m_pInstance = nullptr;

KviUserIdentityManager::KviUserIdentityManager()
    : KviHeapObject()
{
	m_pIdentityDict = new KviPointerHashTable<QString, KviUserIdentity>();
	m_pIdentityDict->setAutoDelete(true);
}

KviUserIdentityManager::~KviUserIdentityManager()
{
	delete m_pIdentityDict;
}

void KviUserIdentityManager::init()
{
	if(m_pInstance)
		return;
	m_pInstance = new KviUserIdentityManager();
}

void KviUserIdentityManager::done()
{
	if(!m_pInstance)
		return;
	delete m_pInstance;
	m_pInstance = nullptr;
}

const KviUserIdentity * KviUserIdentityManager::defaultIdentity()
{
	KviUserIdentity * pUser;
	if(!m_szDefaultIdentity.isEmpty())
	{
		pUser = m_pIdentityDict->find(m_szDefaultIdentity);
		if(pUser)
			return pUser;
	}

	// the default identity is borken :/
	// grab the first one
	KviPointerHashTableIterator<QString, KviUserIdentity> it(*m_pIdentityDict);
	pUser = it.current();
	if(pUser)
	{
		m_szDefaultIdentity = pUser->id();
		return pUser;
	}
	// no identities available: create the default
	pUser = new KviUserIdentity();

	pUser->setId(__tr2qs("Default"));
	pUser->setNickName(KVI_DEFAULT_NICKNAME1);

	pUser->setAltNickName1(QString(KVI_DEFAULT_NICKNAME2).replace(KVI_DEFAULT_NICKNAME_TOKEN, KVI_DEFAULT_NICKNAME1));
	pUser->setAltNickName2(QString(KVI_DEFAULT_NICKNAME3).replace(KVI_DEFAULT_NICKNAME_TOKEN, KVI_DEFAULT_NICKNAME1));
	pUser->setAltNickName3(QString(KVI_DEFAULT_NICKNAME4).replace(KVI_DEFAULT_NICKNAME_TOKEN, KVI_DEFAULT_NICKNAME1));
	pUser->setUserName(KVI_DEFAULT_USERNAME);
	pUser->setRealName(KVI_DEFAULT_REALNAME);
	pUser->setPartMessage(KVI_DEFAULT_PART_MESSAGE);
	pUser->setQuitMessage(KVI_DEFAULT_QUIT_MESSAGE);

	m_pIdentityDict->replace(pUser->id(), pUser);

	return pUser;
}

void KviUserIdentityManager::load(const QString & szFileName)
{
	m_pIdentityDict->clear();

	KviConfigurationFile cfg(szFileName, KviConfigurationFile::Read);

	cfg.setGroup("KVIrc");

	m_szDefaultIdentity = cfg.readEntry("DefaultIdentity", "");

	KviConfigurationFileIterator it(*(cfg.dict()));
	while((it.current()))
	{
		if(!KviQString::equalCI(it.currentKey(), "KVIrc"))
		{
			cfg.setGroup(it.currentKey());

			KviUserIdentity * pUser = new KviUserIdentity();
			if(pUser->load(cfg))
				m_pIdentityDict->replace(pUser->id(), pUser);
			else
				delete pUser;
		}
		++it;
	}
}

void KviUserIdentityManager::save(const QString & szFileName)
{
	KviConfigurationFile cfg(szFileName, KviConfigurationFile::Write);
	cfg.clear();

	cfg.setGroup("KVIrc");

	cfg.writeEntry("DefaultIdentity", m_szDefaultIdentity);

	KviPointerHashTableIterator<QString, KviUserIdentity> it(*m_pIdentityDict);
	while(KviUserIdentity * pUser = it.current())
	{
		pUser->save(cfg);
		++it;
	}
}

void KviUserIdentityManager::copyFrom(KviUserIdentityManager * pWorkingCopy)
{
	m_pIdentityDict->clear();
	m_szDefaultIdentity = pWorkingCopy->m_szDefaultIdentity;
	KviPointerHashTableIterator<QString, KviUserIdentity> it(*(pWorkingCopy->m_pIdentityDict));
	while(KviUserIdentity * pUser = it.current())
	{
		KviUserIdentity * pNew = new KviUserIdentity();
		pNew->copyFrom(*pUser);
		m_pIdentityDict->replace(pNew->id(), pNew);
		++it;
	}
}

KviUserIdentityManager * KviUserIdentityManager::createWorkingCopy()
{
	KviUserIdentityManager * pCopy = new KviUserIdentityManager();
	pCopy->copyFrom(this);
	return pCopy;
}

void KviUserIdentityManager::releaseWorkingCopy(KviUserIdentityManager * pWorkingCopy)
{
	if(pWorkingCopy)
		delete pWorkingCopy;
}
