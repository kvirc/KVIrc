#ifndef _KVI_IRCCONNECTIONSTATISTICS_H_
#define _KVI_IRCCONNECTIONSTATISTICS_H_
//=============================================================================
//
//   File : KviIrcConnectionStatistics.h
//   Creation date : Sun 27 Jun 2004 21:13:48 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_settings.h"
#include "KviQString.h"
#include "KviTimeUtils.h"

class KVIRC_API KviIrcConnectionStatistics
{
	friend class KviIrcConnection;

public:
	KviIrcConnectionStatistics();
	~KviIrcConnectionStatistics();

protected:
	kvi_time_t m_tConnectionStart = 0; // (valid only when Connected or LoggingIn)
	kvi_time_t m_tLastMessage = 0;     // last message received from server
public:
	kvi_time_t connectionStartTime() const { return m_tConnectionStart; }
	kvi_time_t lastMessageTime() const { return m_tLastMessage; }
protected:
	void setLastMessageTime(kvi_time_t t) { m_tLastMessage = t; }
	void setConnectionStartTime(kvi_time_t t) { m_tConnectionStart = t; }
};

#endif //!_KVI_IRCCONNECTIONSTATISTICS_H_
