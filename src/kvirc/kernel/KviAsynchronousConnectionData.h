#ifndef _KVI_ASYNCHRONOUSCONNECTIONDATA_H_
#define _KVI_ASYNCHRONOUSCONNECTIONDATA_H_
//=============================================================================
//
//   File : KviAsynchronousConnectionData.h
//   Creation date : Sun 04 Jul 2004 16:22:25 by Szymon Stefanek
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
#include "KviHeapObject.h"
#include "KviIrcServer.h"

class KviAsynchronousConnectionData : public KviHeapObject
{
public:
	KviAsynchronousConnectionData();
	~KviAsynchronousConnectionData();

public:
	QString szServer;
	bool bUseLastServerInContext = false; // this is checked ONLY if szServer is empty
	kvi_u32_t uPort;
	bool bPortIsOk;
	bool bUseIPv6;
	bool bUseSSL;
	bool bSTARTTLS;
	QString szBindAddress; // ip address (or interface name) to bind the connection to
	QString szCommandToExecAfterConnect;
	QString szLinkFilter;
	QString szPass;
	QString szNick;
	QString szInitUMode;
	QString szServerId;
	KviIrcServerReconnectInfo * m_pReconnectInfo = nullptr;
};

#endif //!_KVI_ASYNCHRONOUSCONNECTIONDATA_H_
