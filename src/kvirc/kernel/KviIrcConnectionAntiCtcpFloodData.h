#ifndef _KVI_IRCCONNECTIONANTICTCPFLOODDATA_H_
#define _KVI_IRCCONNECTIONANTICTCPFLOODDATA_H_
//=============================================================================
//
//   File : KviIrcConnectionAntiCtcpFloodData.h
//   Creation date : Sat 26 Jun 2004 18:09:42 by Szymon Stefanek
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
#include "KviTimeUtils.h"

class KVIRC_API KviIrcConnectionAntiCtcpFloodData
{
public:
	KviIrcConnectionAntiCtcpFloodData();
	~KviIrcConnectionAntiCtcpFloodData();

protected:
	// ctcp antiflood mechanism
	kvi_time_t m_tLastCtcp = 0;    // the time we have received the last "may flood" ctcp request
	unsigned int m_uCtcpCount = 0; // the ctcp counter for the antiflooder
public:
	kvi_time_t lastCtcpTime() const { return m_tLastCtcp; }
	unsigned int ctcpCount() const { return m_uCtcpCount; }
	void setLastCtcpTime(kvi_time_t tLastCtcp) { m_tLastCtcp = tLastCtcp; }
	void increaseCtcpCount() { m_uCtcpCount++; }
	void setCtcpCount(unsigned int uCtcpCount) { m_uCtcpCount = uCtcpCount; }
};

#endif //!_KVI_IRCCONNECTIONANTICTCPFLOODDATA_H_
