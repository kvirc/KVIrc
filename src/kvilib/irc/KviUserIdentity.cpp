//=============================================================================
//
//   File : KviUserIdentity.cpp
//   Creation date : Sun 21 Jan 2007 04:31:47 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2007-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviUserIdentity.h"
#include "KviConfigurationFile.h"
#include "KviLocale.h"
#include "kvi_defaults.h"

// FIXME: Put here also the default away message, default away nick, default ctcp replies etc ?

KviUserIdentity::KviUserIdentity()
    : KviHeapObject()
{
}

KviUserIdentity::~KviUserIdentity()
    = default;

bool KviUserIdentity::load(KviConfigurationFile & cfg)
{
	m_szId = cfg.group();
	m_szNickName = cfg.readEntry("NickName");
	m_szAltNickName1 = cfg.readEntry("AltNickName1");
	m_szAltNickName2 = cfg.readEntry("AltNickName2");
	m_szAltNickName3 = cfg.readEntry("AltNickName3");
	m_szUserName = cfg.readEntry("UserName");
	// FIXME: At least scramble the pass ?
	m_szPassword = cfg.readEntry("Password");
	KviPixmap def;
	m_pixAvatar = cfg.readPixmapEntry("Avatar", def);
	m_szPartMessage = cfg.readEntry("PartMessage");
	m_szQuitMessage = cfg.readEntry("QuitMessage");
	m_szAge = cfg.readEntry("Age");
	m_szGender = cfg.readEntry("Gender");
	m_szLocation = cfg.readEntry("Location");
	m_szLanguages = cfg.readEntry("Languages");
	m_szOtherInfo = cfg.readEntry("OtherInfo");
	m_szUserMode = cfg.readEntry("UserMode");
	m_szOnConnectCommand = cfg.readEntry("OnConnectCommand");
	m_szOnLoginCommand = cfg.readEntry("OnLoginCommand");
	return !(m_szId.isEmpty() || m_szNickName.isEmpty());
}

bool KviUserIdentity::save(KviConfigurationFile & cfg)
{
	cfg.setGroup(m_szId);
	cfg.writeEntry("NickName", m_szNickName);
	cfg.writeEntry("AltNickName1", m_szAltNickName1);
	cfg.writeEntry("AltNickName2", m_szAltNickName2);
	cfg.writeEntry("AltNickName3", m_szAltNickName3);
	cfg.writeEntry("UserName", m_szUserName);
	// FIXME: At least scramble the pass ?
	cfg.writeEntry("Password", m_szPassword);
	cfg.writeEntry("Avatar", m_pixAvatar);
	cfg.writeEntry("PartMessage", m_szPartMessage);
	cfg.writeEntry("QuitMessage", m_szQuitMessage);
	cfg.writeEntry("Age", m_szAge);
	cfg.writeEntry("Gender", m_szGender);
	cfg.writeEntry("Location", m_szLocation);
	cfg.writeEntry("Languages", m_szLanguages);
	cfg.writeEntry("OtherInfo", m_szOtherInfo);
	cfg.writeEntry("UserMode", m_szUserMode);
	cfg.writeEntry("OnConnectCommand", m_szOnConnectCommand);
	cfg.writeEntry("OnLoginCommand", m_szOnLoginCommand);
	return true;
}

void KviUserIdentity::copyFrom(const KviUserIdentity & src)
{
	m_szId = src.m_szId;
	m_szNickName = src.m_szNickName;

	m_szAltNickName1 = src.m_szAltNickName1;
	m_szAltNickName2 = src.m_szAltNickName2;
	m_szAltNickName3 = src.m_szAltNickName3;

	m_szUserName = src.m_szUserName;
	m_szRealName = src.m_szRealName;
	m_szPassword = src.m_szPassword;

	m_pixAvatar = src.m_pixAvatar;

	m_szUserName = src.m_szUserName;
	m_szRealName = src.m_szRealName;
	m_szPassword = src.m_szPassword;

	m_pixAvatar = src.m_pixAvatar;

	m_szPartMessage = src.m_szPartMessage;
	m_szQuitMessage = src.m_szQuitMessage;

	m_szAge = src.m_szAge;
	m_szGender = src.m_szGender;
	m_szLocation = src.m_szLocation;
	m_szLanguages = src.m_szLanguages;
	m_szOtherInfo = src.m_szOtherInfo;

	m_szUserMode = src.m_szUserMode;

	m_szOnConnectCommand = src.m_szOnConnectCommand;
	m_szOnLoginCommand = src.m_szOnLoginCommand;
}
