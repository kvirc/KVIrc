//=============================================================================
//
//   File : thread.cpp
//   Creation date : Tue Sep 20 09 2000 18:29:51 CEST Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#define _KVI_DEBUG_CHECK_RANGE_

#include "thread.h"

#include "kvi_debug.h"
#include "kvi_window.h"
#include "kvi_error.h"
#include "kvi_memmove.h"
#include "kvi_malloc.h"
#include "kvi_netutils.h"
#include "kvi_socket.h"

#ifdef COMPILE_SSL_SUPPORT
	#include "kvi_sslmaster.h"
#endif

KviDccThread::KviDccThread(QObject * par,kvi_socket_t fd)
: KviSensitiveThread()
{
	m_pParent     = par;
	m_fd          = fd;
	m_pMutex      = new KviMutex();
#ifdef COMPILE_SSL_SUPPORT
//	debug("CLEARING SSL IN KviDccThread constructor");
	m_pSSL        = 0;
#endif
}

KviDccThread::~KviDccThread()
{
#ifdef COMPILE_SSL_SUPPORT
	if(m_pSSL)KviSSLMaster::freeSSL(m_pSSL);
	m_pSSL = 0;
#endif
	if(m_fd != KVI_INVALID_SOCKET)kvi_socket_close(m_fd);
	__range_invalid(m_pMutex->locked());
	delete m_pMutex;
}

#ifdef COMPILE_SSL_SUPPORT
void KviDccThread::setSSL(KviSSL * s)
{
	if(m_pSSL)KviSSLMaster::freeSSL(m_pSSL);
	m_pSSL = s;
}
#endif

bool KviDccThread::handleInvalidSocketRead(int readLen)
{
	__range_valid(readLen < 1);
	if(readLen == 0)
	{
		// connection closed
		postErrorEvent(KviError_remoteEndClosedConnection);
		return false;
	} else {
		// error ?
		int err = kvi_socket_error();
		if((err != EINTR) && (err != EAGAIN))
		{
			postErrorEvent(KviError::translateSystemError(err));
			return false;
		}
	}
	return true; // continue
}

#ifdef COMPILE_SSL_SUPPORT
void KviDccThread::raiseSSLError()
{
	KviStr buffer;
	while(m_pSSL->getLastErrorString(buffer))
	{
		KviStr msg(KviStr::Format,"[SSL ERROR]: %s",buffer.ptr());
		postMessageEvent(msg.ptr());
	}
}
#endif

void KviDccThread::postErrorEvent(int err)
{
	KviThreadDataEvent<int> * e = new KviThreadDataEvent<int>(KVI_DCC_THREAD_EVENT_ERROR);
	e->setData(new int(err));
	postEvent(m_pParent,e);
}

void KviDccThread::postMessageEvent(const char * message)
{
	KviThreadDataEvent<KviStr> * e = new KviThreadDataEvent<KviStr>(KVI_DCC_THREAD_EVENT_MESSAGE);
	e->setData(new KviStr(message));
	postEvent(m_pParent,e);
}
