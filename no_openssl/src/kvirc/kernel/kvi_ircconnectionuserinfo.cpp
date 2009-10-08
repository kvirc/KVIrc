//=============================================================================
//
//   File : kvi_ircconnectionuserinfo.cpp
//   Creation date : Sun 20 Jun 2004 01:45:42 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2008 Szymon Stefanek <pragma at kvirc dot net>
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



#include "kvi_ircconnectionuserinfo.h"

KviIrcConnectionUserInfo::KviIrcConnectionUserInfo()
{
	m_bAway = false;
}

bool KviIrcConnectionUserInfo::hasUserMode(const QChar &m)
{
	return m_szUserMode.contains(m) > 0;
}

bool KviIrcConnectionUserInfo::addUserMode(const QChar &m)
{
	if(hasUserMode(m))return false;
	m_szUserMode.append(m);
	return true;
}

bool KviIrcConnectionUserInfo::removeUserMode(const QChar &m)
{
	int idx = m_szUserMode.indexOf(m,0);
	if(idx == -1)return false;
	m_szUserMode.replace(m,QString(""));
	return true;
}

void KviIrcConnectionUserInfo::setAway()
{
	m_szNickBeforeAway = m_szNickName;
	m_tAway = kvi_unixTime();
	m_bAway = true;
}

void KviIrcConnectionUserInfo::setBack()
{
	m_szNickBeforeAway = "";
	m_bAway = false;
}
