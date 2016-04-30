//=============================================================================
//
//   File : KviMessageTypeSettings.cpp
//   Creation date : Fri 30 24 2000 13:53:21 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#define _KVI_DEBUG_CHECK_RANGE_

#include "kvi_debug.h"
#include "KviMessageTypeSettings.h"

KviMessageTypeSettings::KviMessageTypeSettings(const char * type, int pixId, unsigned char fore, unsigned char back, bool bLog, int iLevel)
{
	m_szType = type;
	m_iPixId = pixId;
	m_cForeColor = fore;
	m_cBackColor = back;
	m_bLogEnabled = bLog;
	if((iLevel < KVI_MSGTYPE_MINLEVEL) || (iLevel > KVI_MSGTYPE_MAXLEVEL))
		iLevel = 1;
	m_iLevel = iLevel;
}

KviMessageTypeSettings::KviMessageTypeSettings(const KviMessageTypeSettings & msg)
{
	m_szType = msg.m_szType;
	m_iPixId = msg.m_iPixId;
	m_cForeColor = msg.m_cForeColor;
	m_cBackColor = msg.m_cBackColor;
	m_bLogEnabled = msg.m_bLogEnabled;
	m_iLevel = msg.m_iLevel;
}

KviMessageTypeSettings::~KviMessageTypeSettings()
    = default;

KviMessageTypeSettings & KviMessageTypeSettings::operator=(const KviMessageTypeSettings & msg)
    = default;
