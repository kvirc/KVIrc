//=============================================================================
//
//   File : KviIdentityProfile.cpp
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

#include "KviIdentityProfile.h"
#include "KviConfigurationFile.h"

KviIdentityProfile::KviIdentityProfile()
    : KviHeapObject()
{
}

KviIdentityProfile::~KviIdentityProfile()
    = default;

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

	szTmp = QString("%1Name").arg(szPrefix);
	pCfg->writeEntry(szTmp, m_szName);
	szTmp = QString("%1Network").arg(szPrefix);
	pCfg->writeEntry(szTmp, m_szNetwork);
	szTmp = QString("%1Nick").arg(szPrefix);
	pCfg->writeEntry(szTmp, m_szNick);
	szTmp = QString("%1AltNick").arg(szPrefix);
	pCfg->writeEntry(szTmp, m_szAltNick);
	szTmp = QString("%1UserName").arg(szPrefix);
	pCfg->writeEntry(szTmp, m_szUserName);
	szTmp = QString("%1RealName").arg(szPrefix);
	pCfg->writeEntry(szTmp, m_szRealName);
}

bool KviIdentityProfile::load(KviConfigurationFile * pCfg, const QString & szPrefix)
{
	QString szTmp;

	szTmp = QString("%1Name").arg(szPrefix);
	m_szName = pCfg->readEntry(szTmp).trimmed();
	if(m_szName.isEmpty())
		return false;

	szTmp = QString("%1Network").arg(szPrefix);
	m_szNetwork = pCfg->readEntry(szTmp);
	if(m_szNetwork.isEmpty())
		return false;

	szTmp = QString("%1Nick").arg(szPrefix);
	m_szNick = pCfg->readEntry(szTmp);
	if(m_szNick.isEmpty())
		return false;

	szTmp = QString("%1AltNick").arg(szPrefix);
	m_szAltNick = pCfg->readEntry(szTmp);

	szTmp = QString("%1UserName").arg(szPrefix);
	m_szUserName = pCfg->readEntry(szTmp);

	szTmp = QString("%1RealName").arg(szPrefix);
	m_szRealName = pCfg->readEntry(szTmp);

	return true;
}
