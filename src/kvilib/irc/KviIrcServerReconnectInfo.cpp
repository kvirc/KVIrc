//=============================================================================
//
//   File : KviIrcServerReconnectInfo.h
//   Creation date : Tue Jan 04 2010 21:44:11 by Elvio Basello
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2010 Elvio Basello (hellvis69 at gmail dot com)
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

#include "KviIrcServerReconnectInfo.h"

KviIrcServerReconnectInfo::KviIrcServerReconnectInfo()
{
	m_bIsAway = false;
}

KviIrcServerReconnectInfo::~KviIrcServerReconnectInfo()
{
}

KviIrcServerReconnectInfo::KviIrcServerReconnectInfo(const KviIrcServerReconnectInfo & info)
{
	m_szNick         = info.m_szNick;
	m_szPass         = info.m_szPass;
	m_szAwayReason   = info.m_szAwayReason;
	m_szJoinChannels = info.m_szJoinChannels;
	m_szOpenQueryes  = info.m_szOpenQueryes;
	m_bIsAway        = info.m_bIsAway;
}
