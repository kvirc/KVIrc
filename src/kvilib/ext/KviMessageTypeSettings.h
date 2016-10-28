#ifndef _KVI_MSGTYPE_H_
#define _KVI_MSGTYPE_H_
//=============================================================================
//
//   File : KviMessageTypeSettings.h
//   Creation date : Fri Jun 30 2000 13:50:11 by Szymon Stefanek
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

#include "kvi_settings.h"

#define KVI_MSGTYPE_MINLEVEL 0
#define KVI_MSGTYPE_LEVEL_0 0
#define KVI_MSGTYPE_LEVEL_1 1
#define KVI_MSGTYPE_LEVEL_2 2
#define KVI_MSGTYPE_LEVEL_3 3
#define KVI_MSGTYPE_LEVEL_4 4
#define KVI_MSGTYPE_LEVEL_5 5
#define KVI_MSGTYPE_MAXLEVEL 5

// 1 : Common activity
// 2 : People talking
// 3 : Response messages (whois, dns)
// 4 :
// 5 : Highlighted text

// FIXME: Check the message ranges!!!

class KVILIB_API KviMessageTypeSettings
{
public:
	KviMessageTypeSettings(const char * type, int pixId, unsigned char fore, unsigned char back, bool bLog, int iLevel);
	KviMessageTypeSettings(const KviMessageTypeSettings & msg);
	~KviMessageTypeSettings();

public:
	const char * m_szType;
	int m_iPixId;
	unsigned char m_cForeColor;
	unsigned char m_cBackColor;
	bool m_bLogEnabled;
	int m_iLevel;

public:
	void setBack(char back) { m_cBackColor = back; };
	void setFore(char fore) { m_cForeColor = fore; };
	int pixId() { return m_iPixId; };
	int level() { return m_iLevel; };
	void setLevel(int iLevel)
	{
		if((iLevel < KVI_MSGTYPE_MINLEVEL) || (iLevel > KVI_MSGTYPE_MAXLEVEL))
			m_iLevel = 1;
		else
			m_iLevel = iLevel;
	};
	void setPixId(int pixId) { m_iPixId = pixId; };
	unsigned char back() { return m_cBackColor; };
	unsigned char fore() { return m_cForeColor; };
	bool logEnabled() { return m_bLogEnabled; };
	void enableLogging(bool bEnable) { m_bLogEnabled = bEnable; };
	const char * type() { return m_szType; };
	KviMessageTypeSettings & operator=(const KviMessageTypeSettings & msg); // deep copy
};

#endif //_KVI_MSGTYPE_H_
