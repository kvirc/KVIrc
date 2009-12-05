//=============================================================================
//
//   File : kvi_identityprofile.cpp
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

#include "kvi_identityprofile.h"
#include "kvi_config.h"

KviIdentityProfileSet * KviIdentityProfileSet::m_pSelf = NULL;
unsigned int KviIdentityProfileSet::m_uCount = 0;

KviIdentityProfile::KviIdentityProfile()
: KviHeapObject()
{
}

KviIdentityProfile::~KviIdentityProfile()
{
}

void KviIdentityProfile::copyFrom(const KviIdentityProfile & src)
{
	m_szName = src.m_szName;
	m_szNetwork = src.m_szNetwork;
	m_szNick = src.m_szNick;
	m_szAltNick = src.m_szAltNick;
	m_szUserName = src.m_szUserName;
	m_szRealName = src.m_szRealName;
}

void KviIdentityProfile::save(KviConfig * pCfg, const QString & szPrefix)
{
	QString szTmp;

	KviQString::sprintf(szTmp,"%QName",&szPrefix);
	pCfg->writeEntry(szTmp,m_szName);
	KviQString::sprintf(szTmp,"%QNetwork",&szPrefix);
	pCfg->writeEntry(szTmp,m_szNetwork);
	KviQString::sprintf(szTmp,"%QNick",&szPrefix);
	pCfg->writeEntry(szTmp,m_szNick);
	KviQString::sprintf(szTmp,"%QAltNick",&szPrefix);
	pCfg->writeEntry(szTmp,m_szAltNick);
	KviQString::sprintf(szTmp,"%QUserName",&szPrefix);
	pCfg->writeEntry(szTmp,m_szUserName);
	KviQString::sprintf(szTmp,"%QRealName",&szPrefix);
	pCfg->writeEntry(szTmp,m_szRealName);
}

bool KviIdentityProfile::load(KviConfig * pCfg, const QString & szPrefix)
{
	QString szTmp;

	KviQString::sprintf(szTmp,"%QName",&szPrefix);
	m_szName = KviQString::trimmed(pCfg->readQStringEntry(szTmp));
	if(m_szName.isEmpty())
		return false;

	KviQString::sprintf(szTmp,"%QNetwork",&szPrefix);
	m_szNetwork = pCfg->readQStringEntry(szTmp);
	if(m_szNetwork.isEmpty())
		return false;

	KviQString::sprintf(szTmp,"%QNick",&szPrefix);
	m_szNick = pCfg->readQStringEntry(szTmp);
	if(m_szNick.isEmpty())
		return false;

	KviQString::sprintf(szTmp,"%QAltNick",&szPrefix);
	m_szAltNick = pCfg->readQStringEntry(szTmp);

	KviQString::sprintf(szTmp,"%QUserName",&szPrefix);
	m_szUserName = pCfg->readQStringEntry(szTmp);

	KviQString::sprintf(szTmp,"%QRealName",&szPrefix);
	m_szRealName = pCfg->readQStringEntry(szTmp);
	
	return true;
}


KviIdentityProfileSet::KviIdentityProfileSet()
: KviHeapObject()
{
	m_bEnabled = false;
	m_pProfiles = 0;
}

KviIdentityProfileSet::KviIdentityProfileSet(const KviIdentityProfileSet & set)
{
	m_pProfiles = 0;
	copyFrom(set);
}

KviIdentityProfileSet::~KviIdentityProfileSet()
{
	if(m_pProfiles) delete m_pProfiles;
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
		m_pProfiles = 0;
	}
	m_bEnabled = false;
}

KviIdentityProfile * KviIdentityProfileSet::findName(const QString & szName)
{
	if(!m_pProfiles) return 0;

	KviIdentityProfile * pProfile;
	for(pProfile = m_pProfiles->first(); pProfile; pProfile = m_pProfiles->next())
	{
		if(KviQString::matchString(pProfile->name(),szName,false,true))
			return pProfile;
	}

	return 0;
}

KviIdentityProfile * KviIdentityProfileSet::findNetwork(const QString & szNetwork)
{
	if(!m_pProfiles) return 0;

	KviIdentityProfile * pProfile;
	for(pProfile = m_pProfiles->first(); pProfile; pProfile = m_pProfiles->next())
	{
		if(KviQString::matchString(pProfile->network(),szNetwork,false,true))
			return pProfile;
	}

	return 0;
}

void KviIdentityProfileSet::copyFrom(const KviIdentityProfileSet & src)
{
	if(src.m_pProfiles)
	{
		if(m_pProfiles) m_pProfiles->clear();
		else {
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
			m_pProfiles = 0;
		} else {
			m_bEnabled = src.m_bEnabled;
		}
	} else {
		m_bEnabled = false;
		if(m_pProfiles)
		{
			delete m_pProfiles;
			m_pProfiles = 0;
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
	KviConfig cfg(szConfigFile,KviConfig::Read);

	QString szTmp;
	KviQString::sprintf(szTmp,"ProfilesNumber");

	unsigned int uCount = cfg.readUIntEntry(szTmp,0);
	if(uCount == 0) return;
	loadPrivate(&cfg,QString(""),uCount);
}

void KviIdentityProfileSet::save(const QString & szConfigFile)
{
	KviConfig cfg(szConfigFile,KviConfig::Write);
	cfg.clear();
	save(&cfg,QString(""));
}

void KviIdentityProfileSet::save(KviConfig * pCfg, const QString & szPrefix)
{
	if(!m_pProfiles)
		return;
	if(m_pProfiles->isEmpty())
		return;

	QString szTmp;
	if(m_bEnabled)
	{
		KviQString::sprintf(szTmp,"%QProfilesEnabled",&szPrefix);
		pCfg->writeEntry(szTmp,m_bEnabled);
	}

	KviQString::sprintf(szTmp,"%QProfilesNumber",&szPrefix);
	pCfg->writeEntry(szTmp,m_pProfiles->count());

	int iIdx = 0;
	for(KviIdentityProfile * pProfile = m_pProfiles->first(); pProfile; pProfile = m_pProfiles->next())
	{
		KviQString::sprintf(szTmp,"%QProfile%d_",&szPrefix,iIdx);
		pProfile->save(pCfg,szTmp);
		iIdx++;
	}
}

bool KviIdentityProfileSet::loadPrivate(KviConfig * pCfg, const QString & szPrefix, unsigned int uEntries)
{
	if(m_pProfiles) m_pProfiles->clear();
	else {
		m_pProfiles = new KviPointerList<KviIdentityProfile>;
		m_pProfiles->setAutoDelete(true);
	}

	if(uEntries != 0)
	{
		QString szTmp;
		KviQString::sprintf(szTmp,"%QProfilesEnabled",&szPrefix);
		m_bEnabled = pCfg->readBoolEntry(szTmp,false);

		for(unsigned int u = 0; u < uEntries; u++)
		{
			KviQString::sprintf(szTmp,"%QProfile%u_",&szPrefix,u);
			KviIdentityProfile * pProfile = new KviIdentityProfile();
			if(!pProfile->load(pCfg,szTmp))
				delete pProfile;
			else m_pProfiles->append(pProfile);
		}
	}

	if(m_pProfiles->isEmpty())
	{
		m_bEnabled = false;
		delete m_pProfiles;
		m_pProfiles = 0;
		return false;
	}
	return true;
}
