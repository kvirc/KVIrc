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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================



#include "kvi_ircconnectionstatedata.h"

KviIrcConnectionStateData::KviIrcConnectionStateData()
{
	m_bSentStartTls = false;
	m_bSentQuit = false;
	m_bInsideInitialCapLs = false;
	m_bInsideInitialCapReq = false;
	m_bIgnoreOneYouHaveNotRegisteredError = false;
	m_uLoginNickIndex = 0;
	m_bSimulateUnexpectedDisconnect = false;
	m_tLastReceivedChannelWhoReply = kvi_unixTime();
	m_tLastSentChannelWhoRequest = m_tLastReceivedChannelWhoReply;
	m_tLastReceivedWhoisReply = 0;
	m_bIdentifyMsgCapabilityEnabled = false;
}

KviIrcConnectionStateData::~KviIrcConnectionStateData()
{
}

void KviIrcConnectionStateData::changeEnabledCapList(const QString &szCapList)
{
	QStringList lTmp = szCapList.split(' ',QString::SkipEmptyParts);
	foreach(QString szCap,lTmp)
	{
		// cap modifiers are:
		//  '-' : disable a capability (should not be present in a LS message...)
		//  '=' : sticky (can't be disabled once enabled)
		//  '~' : needs ack for modification

		if(szCap.length() < 1)
			continue; // shouldn't happen

		bool bRemove = false;

		switch(szCap[0].unicode())
		{
			case '-':
				bRemove = true;
				// fall through
			case '~':
			case '=':
				szCap.remove(0,1);
			break;
			default:
				// ok
			break;
		}

		szCap = szCap.toLower();

		if(szCap == "identify-msg")
			m_bIdentifyMsgCapabilityEnabled = !bRemove;

		if(bRemove)
		{
			m_lEnabledCaps.removeAll(szCap);
		} else {
			if(!m_lEnabledCaps.contains(szCap))
				m_lEnabledCaps.append(szCap);
		}
	}
}

