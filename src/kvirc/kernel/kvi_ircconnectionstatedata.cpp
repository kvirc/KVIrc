//=============================================================================
//
//   File : kvi_ircconnectionstatedata.cpp
//   Creation date : Sat 26 Jun 2004 09:31:52 by Szymon Stefanek
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================



#include "kvi_ircconnectionstatedata.h"

KviIrcConnectionStateData::KviIrcConnectionStateData()
{
	m_bSentStartTls = false;
	m_bSentQuit = false;
	m_uLoginNickIndex = 0;
	m_bSimulateUnexpectedDisconnect = false;
	m_tLastReceivedChannelWhoReply = kvi_unixTime();
	m_tLastSentChannelWhoRequest = m_tLastReceivedChannelWhoReply;
	m_tLastReceivedWhoisReply = 0;
}

KviIrcConnectionStateData::~KviIrcConnectionStateData()
{
}
