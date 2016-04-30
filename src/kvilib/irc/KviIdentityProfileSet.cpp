//=============================================================================
//
//   File : KviIdentityProfileSet.cpp
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

#include "KviIdentityProfileSet.h"
#include "KviConfigurationFile.h"

KviIdentityProfileSet * KviIdentityProfileSet::m_pSelf = nullptr;
unsigned int KviIdentityProfileSet::m_uCount = 0;

KviIdentityProfileSet::KviIdentityProfileSet()
    : KviHeapObject()
{
	m_bEnabled = false;
	m_pProfiles = nullptr;
}

KviIdentityProfileSet::KviIdentityProfileSet(const KviIdentityProfileSet & set)
{
	m_pProfiles = nullptr;
	copyFrom(set);
}

KviIdentityProfileSet::~KviIdentityProfileSet()
{
	if(m_pProfiles)
		delete m_pProfiles;
}

void KviIdentityProfileSet::init()
{
	if((!m_pSelf) && (m_pSelf->count() == 0))
	{
		m_pSelf = new KviIdentityProfileSet();
		m_uCount++;
	}
}

void KviIdentityProfileSet::done()
{
	m_uCount--;
	if(m_pSelf->count() == 0)
		delete m_pSelf;
}

void KviIdentityProfileSet::clear()
{
	if(m_pProfiles)
	{
		delete m_pProfiles;
		m_pProfiles = nullptr;
	}
	m_bEnabled = false;
}

KviIdentityProfile * KviIdentityProfileSet::findName(const QString & szName)
{
	if(!m_pProfiles)
		return nullptr;

	KviIdentityProfile * pProfile;
	for(pProfile = m_pProfiles->first(); pProfile; pProfile = m_pProfiles->next())
	{
		if(KviQString::matchString(pProfile->name(), szName, false, true))
			return pProfile;
	}

	return nullptr;
}

KviIdentityProfile * KviIdentityProfileSet::findNetwork(const QString & szNetwork)
{
	if(!m_pProfiles)
		return nullptr;

	KviIdentityProfile * pProfile;
	for(pProfile = m_pProfiles->first(); pProfile; pProfile = m_pProfiles->next())
	{
		if(KviQString::matchString(pProfile->network(), szNetwork, false, true))
			return pProfile;
	}

	return nullptr;
}

void KviIdentityProfileSet::copyFrom(const KviIdentityProfileSet & src)
{
	if(src.m_pProfiles)
	{
		if(m_pProfiles)
			m_pProfiles->clear();
		else
		{
			m_pProfiles = new KviPointerList<KviIdentityProfile>;
			m_pProfiles->setAutoDelete(true);
		}

		for(KviIdentityProfile * pSrcProfile = src.m_pProfiles->first(); pSrcProfile; pSrcProfile = src.m_pProfiles->next())
		{
			KviIdentityProfile * pProfile = new KviIdentityProfile();
			pProfile->copyFrom(*pSrcProfile);
			m_pProfiles->append(pProfile);
		}

		if(m_pProfiles->isEmpty())
		{
			m_bEnabled = false;
			delete m_pProfiles;
			m_pProfiles = nullptr;
		}
		else
		{
			m_bEnabled = src.m_bEnabled;
		}
	}
	else
	{
		m_bEnabled = false;
		if(m_pProfiles)
		{
			delete m_pProfiles;
			m_pProfiles = nullptr;
		}
	}
}

void KviIdentityProfileSet::addProfile(KviIdentityProfile * pProfile)
{
	if(!m_pProfiles)
	{
		m_pProfiles = new KviPointerList<KviIdentityProfile>;
		m_pProfiles->setAutoDelete(true);
	}

	m_pProfiles->append(pProfile);
}

void KviIdentityProfileSet::load(const QString & szConfigFile)
{
	clear();
	KviConfigurationFile cfg(szConfigFile, KviConfigurationFile::Read);

	QString szTmp = "ProfilesNumber";

	unsigned int uCount = cfg.readUIntEntry(szTmp, 0);
	if(uCount == 0)
		return;
	loadPrivate(&cfg, QString(""), uCount);
}

void KviIdentityProfileSet::save(const QString & szConfigFile)
{
	KviConfigurationFile cfg(szConfigFile, KviConfigurationFile::Write);
	cfg.clear();
	save(&cfg, QString(""));
}

void KviIdentityProfileSet::save(KviConfigurationFile * pCfg, const QString & szPrefix)
{
	if(!m_pProfiles)
		return;
	if(m_pProfiles->isEmpty())
		return;

	QString szTmp;
	if(m_bEnabled)
	{
		szTmp = QString("%1ProfilesEnabled").arg(szPrefix);
		pCfg->writeEntry(szTmp, m_bEnabled);
	}

	szTmp = QString("%1ProfilesNumber").arg(szPrefix);
	pCfg->writeEntry(szTmp, m_pProfiles->count());

	int iIdx = 0;
	for(KviIdentityProfile * pProfile = m_pProfiles->first(); pProfile; pProfile = m_pProfiles->next())
	{
		szTmp = QString("%1Profile%2_").arg(szPrefix).arg(iIdx);
		pProfile->save(pCfg, szTmp);
		iIdx++;
	}
}

bool KviIdentityProfileSet::loadPrivate(KviConfigurationFile * pCfg, const QString & szPrefix, unsigned int uEntries)
{
	if(m_pProfiles)
		m_pProfiles->clear();
	else
	{
		m_pProfiles = new KviPointerList<KviIdentityProfile>;
		m_pProfiles->setAutoDelete(true);
	}

	if(uEntries != 0)
	{
		QString szTmp = QString("%1ProfilesEnabled").arg(szPrefix);
		m_bEnabled = pCfg->readBoolEntry(szTmp, false);

		for(unsigned int u = 0; u < uEntries; u++)
		{
			szTmp = QString("%1Profile%2_").arg(szPrefix, u);
			KviIdentityProfile * pProfile = new KviIdentityProfile();
			if(!pProfile->load(pCfg, szTmp))
				delete pProfile;
			else
				m_pProfiles->append(pProfile);
		}
	}

	if(m_pProfiles->isEmpty())
	{
		m_bEnabled = false;
		delete m_pProfiles;
		m_pProfiles = nullptr;
		return false;
	}
	return true;
}
