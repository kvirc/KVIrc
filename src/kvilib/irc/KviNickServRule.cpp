//=============================================================================
//
//   File : KviNickServRule.cpp
//   Creation date : Thu Aug 09 2001 17:44:56 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviNickServRule.h"
#include "KviConfigurationFile.h"
#include "KviIrcMask.h"

void KviNickServRule::copyFrom(const KviNickServRule &src)
{
	m_szRegisteredNick = src.m_szRegisteredNick;
	m_szNickServMask = src.m_szNickServMask;
	m_szMessageRegexp = src.m_szMessageRegexp;
	m_szIdentifyCommand = src.m_szIdentifyCommand;
	m_szServerMask = src.m_szServerMask;
}

void KviNickServRule::save(KviConfigurationFile * cfg,const QString &prefix)
{
	QString tmp;
	KviQString::sprintf(tmp,"%QRegisteredNick",&prefix);
	cfg->writeEntry(tmp,m_szRegisteredNick);
	KviQString::sprintf(tmp,"%QNickServMask",&prefix);
	cfg->writeEntry(tmp,m_szNickServMask);
	KviQString::sprintf(tmp,"%QMessageRegexp",&prefix);
	cfg->writeEntry(tmp,m_szMessageRegexp);
	KviQString::sprintf(tmp,"%QIdentifyCommand",&prefix);
	cfg->writeEntry(tmp,m_szIdentifyCommand);
	KviQString::sprintf(tmp,"%QServerMask",&prefix);
	cfg->writeEntry(tmp,m_szServerMask);
}

bool KviNickServRule::load(KviConfigurationFile * cfg,const QString &prefix)
{
	QString tmp;
	KviQString::sprintf(tmp,"%QRegisteredNick",&prefix);
	m_szRegisteredNick = KviQString::trimmed(cfg->readEntry(tmp));
	if(m_szRegisteredNick.isEmpty())return false;
	KviQString::sprintf(tmp,"%QNickServMask",&prefix);
	m_szNickServMask = cfg->readEntry(tmp);
	if(m_szNickServMask.isEmpty())return false;
	KviQString::sprintf(tmp,"%QServerMask",&prefix);
	m_szServerMask = cfg->readEntry(tmp,QString());
	KviQString::sprintf(tmp,"%QMessageRegexp",&prefix);
	m_szMessageRegexp = cfg->readEntry(tmp);
	if(m_szMessageRegexp.isEmpty())return false;
	KviQString::sprintf(tmp,"%QIdentifyCommand",&prefix);
	m_szIdentifyCommand = cfg->readEntry(tmp);
	return !m_szIdentifyCommand.isEmpty();
}

KviNickServRule * KviNickServRule::createInstance(const QString &szRegisteredNick,
		const QString &szNickServMask,
		const QString &szMessageRegexp,
		const QString &szIdentifyCommand,
		const QString &szServerMask)
{
	return new KviNickServRule(szRegisteredNick,szNickServMask,szMessageRegexp,szIdentifyCommand,szServerMask);
}

