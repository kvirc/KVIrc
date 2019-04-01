#ifndef _THREAD_H_
#define _THREAD_H_
//=============================================================================
//
//   File : DccThread.h
//   Creation date : Tue Sep 20 09 2000 18:28:44 by Szymon Stefanek
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
#include "KviThread.h"
#include "kvi_sockettype.h"
#include "KviPointerList.h"

#include <QObject>

#ifdef COMPILE_SSL_SUPPORT
#include "KviSSL.h"
#endif

// KviThreadDataEvent<int>
#define KVI_DCC_THREAD_EVENT_ERROR (KVI_THREAD_USER_EVENT_BASE + 1)
// KviThreadDataEvent<KviCString>
#define KVI_DCC_THREAD_EVENT_DATA (KVI_THREAD_USER_EVENT_BASE + 2)
// KviThreadEvent
#define KVI_DCC_THREAD_EVENT_SUCCESS (KVI_THREAD_USER_EVENT_BASE + 3)
// KviThreadDataEvent<KviCString>
#define KVI_DCC_THREAD_EVENT_MESSAGE (KVI_THREAD_USER_EVENT_BASE + 4)
// KviThreadDataEvent<int>
#define KVI_DCC_THREAD_EVENT_ACTION (KVI_THREAD_USER_EVENT_BASE + 5)

struct KviDccThreadIncomingData
{
	int iLen;
	char * buffer;
};

class DccThread : public KviSensitiveThread
{
public:
	DccThread(QObject * par, kvi_socket_t fd);
	~DccThread();

protected:
	KviMutex * m_pMutex; // OWNED! PROTECTS m_pOutBuffers
	kvi_socket_t m_fd;
	QObject * m_pParent; // READ ONLY!
#ifdef COMPILE_SSL_SUPPORT
	KviSSL * m_pSSL;
#endif
protected:
	bool handleInvalidSocketRead(int readLen);

public:
	QObject * parent() { return m_pParent; };
	void postErrorEvent(int err);
	// Warning!..newer call __tr() here!...use __tr_no_lookup()
	void postMessageEvent(const char * message);
#ifdef COMPILE_SSL_SUPPORT
	void raiseSSLError();
	void setSSL(KviSSL * s);
	KviSSL * getSSL() const { return m_pSSL; };
#endif
};

#endif //_THREAD_H_
