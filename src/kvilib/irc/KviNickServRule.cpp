//=============================================================================
//
//   File : KviNickServRule.cpp
//   Creation date : Thu Aug 09 2001 17:44:56 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviConfigurationFile.h"
#include "KviIrcMask.h"
#include "KviNickServRule.h"

void KviNickServRule::copyFrom(const KviNickServRule & src)
{
	m_szRegisteredNick = src.m_szRegisteredNick;
	m_szNickServMask = src.m_szNickServMask;
	m_szMessageRegexp = src.m_szMessageRegexp;
	m_szIdentifyCommand = src.m_szIdentifyCommand;
	m_szServerMask = src.m_szServerMask;
}

void KviNickServRule::save(KviConfigurationFile * cfg, const QString & szPrefix)
{
	QString szTmp;
	szTmp = QString("%1RegisteredNick").arg(szPrefix);
	cfg->writeEntry(szTmp, m_szRegisteredNick);
	szTmp = QString("%1NickServMask").arg(szPrefix);
	cfg->writeEntry(szTmp, m_szNickServMask);
	szTmp = QString("%1MessageRegexp").arg(szPrefix);
	cfg->writeEntry(szTmp, m_szMessageRegexp);
	szTmp = QString("%1IdentifyCommand").arg(szPrefix);
	cfg->writeEntry(szTmp, m_szIdentifyCommand);
	szTmp = QString("%1ServerMask").arg(szPrefix);
	cfg->writeEntry(szTmp, m_szServerMask);
}

bool KviNickServRule::load(KviConfigurationFile * cfg, const QString & szPrefix)
{
	QString szTmp;
	szTmp = QString("%1RegisteredNick").arg(szPrefix);
	m_szRegisteredNick = cfg->readEntry(szTmp).trimmed();
	if(m_szRegisteredNick.isEmpty())
		return false;
	szTmp = QString("%1NickServMask").arg(szPrefix);
	m_szNickServMask = cfg->readEntry(szTmp);
	if(m_szNickServMask.isEmpty())
		return false;
	szTmp = QString("%1ServerMask").arg(szPrefix);
	m_szServerMask = cfg->readEntry(szTmp, QString());
	szTmp = QString("%1MessageRegexp").arg(szPrefix);
	m_szMessageRegexp = cfg->readEntry(szTmp);
	if(m_szMessageRegexp.isEmpty())
		return false;
	szTmp = QString("%1IdentifyCommand").arg(szPrefix);
	m_szIdentifyCommand = cfg->readEntry(szTmp);
	return !m_szIdentifyCommand.isEmpty();
}

KviNickServRule * KviNickServRule::createInstance(const QString & szRegisteredNick,
    const QString & szNickServMask,
    const QString & szMessageRegexp,
    const QString & szIdentifyCommand,
    const QString & szServerMask)
{
	return new KviNickServRule(szRegisteredNick, szNickServMask, szMessageRegexp, szIdentifyCommand, szServerMask);
}
