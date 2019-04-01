#ifndef _LIBKVIIDENT_H_
#define _LIBKVIIDENT_H_
//=============================================================================
//
//   File : libkviident.h
//   Creation date : Tue Oct  2 18:22:05 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviThread.h"
#include "kvi_sockettype.h"
#include "KviCString.h"
#include "KviTimeUtils.h"
#include "kvi_settings.h"
#include "KviPointerList.h"

#include <QObject>

class KviIdentRequest
{
public:
	KviIdentRequest(kvi_socket_t sock, const char * host, kvi_u32_t uPort);
	~KviIdentRequest();

public:
	kvi_socket_t m_sock;
	KviCString m_szData;
	KviCString m_szHost;
	kvi_u32_t m_uPort;
	time_t m_tStart;
};

struct KviIdentMessageData
{
	KviCString szMessage;
	KviCString szHost;
	KviCString szAux;
	unsigned int uPort;
};

class KviIdentSentinel : public QObject
{
	Q_OBJECT
public:
	KviIdentSentinel();
	~KviIdentSentinel();

protected:
	bool event(QEvent * e) override;
};

class KviIdentDaemon : public KviSensitiveThread
{
public:
	KviIdentDaemon();
	~KviIdentDaemon();

private:
	KviCString m_szUser;
	kvi_u32_t m_uPort;
	bool m_bEnableIPv6;
	bool m_bIPv6ContainsIPv4;
	kvi_socket_t m_sock;
	kvi_socket_t m_sock6;
	KviPointerList<KviIdentRequest> * m_pRequestList = nullptr;

public:
	void run() override;

protected:
	void postMessage(const char * message, KviIdentRequest * r, const char * szAux = nullptr);
};

#endif //_LIBKVIIDENT_H_
