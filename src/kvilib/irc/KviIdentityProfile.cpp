//=============================================================================
//
//   File : KviIdentityProfile.cpp
//   Creation date : Thu Oct 16 2008 16:37:48 by Elvio Basello
//
//   This file is part of the KVIrc irc client distribution
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

#include "KviIdentityProfile.h"
#include "KviConfigurationFile.h"

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

void KviIdentityProfile::save(KviConfigurationFile * pCfg, const QString & szPrefix)
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

bool KviIdentityProfile::load(KviConfigurationFile * pCfg, const QString & szPrefix)
{
	QString szTmp;

	KviQString::sprintf(szTmp,"%QName",&szPrefix);
	m_szName = KviQString::trimmed(pCfg->readEntry(szTmp));
	if(m_szName.isEmpty())
		return false;

	KviQString::sprintf(szTmp,"%QNetwork",&szPrefix);
	m_szNetwork = pCfg->readEntry(szTmp);
	if(m_szNetwork.isEmpty())
		return false;

	KviQString::sprintf(szTmp,"%QNick",&szPrefix);
	m_szNick = pCfg->readEntry(szTmp);
	if(m_szNick.isEmpty())
		return false;

	KviQString::sprintf(szTmp,"%QAltNick",&szPrefix);
	m_szAltNick = pCfg->readEntry(szTmp);

	KviQString::sprintf(szTmp,"%QUserName",&szPrefix);
	m_szUserName = pCfg->readEntry(szTmp);

	KviQString::sprintf(szTmp,"%QRealName",&szPrefix);
	m_szRealName = pCfg->readEntry(szTmp);
	
	return true;
}
