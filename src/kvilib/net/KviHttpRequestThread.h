#ifndef _KviHttpRequestThread_h_
#define _KviHttpRequestThread_h_
//=============================================================================
//
//   File : KviHttpRequestThread.h
//   Creation date : Sat Aug 17 13:43:31 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"

#include "KviQString.h"
#include "KviThread.h"
#include "kvi_sockettype.h"

#ifdef COMPILE_SSL_SUPPORT
	class KviSSL;
#endif //COMPILE_SSL_SUPPORT

class KviHttpRequest;

class KviHttpRequestThread : public KviSensitiveThread
{
	friend class KviHttpRequest;
public:
	enum RequestMethod { Post, Get, Head };
protected:
	KviHttpRequestThread(KviHttpRequest * r,
			const QString &szHost,
			const QString &szIp,
			unsigned short uPort,
			const QString &szPath,
			unsigned int uContentOffset,
			RequestMethod m,
			const QString &szPostData = QString(),
			bool bUseSSL = false
		);

public:
	~KviHttpRequestThread();
protected:
	KviHttpRequest * m_pRequest;

	QString           m_szHost;
	QString          m_szIp;
	QString          m_szPath;
	unsigned int     m_uContentOffset;
	RequestMethod    m_eRequestMethod;
	QString          m_szPostData;

	unsigned short   m_uPort;
	kvi_socket_t     m_sock;
	bool             m_bUseSSL;
	unsigned int     m_uConnectionTimeout;
#ifdef COMPILE_SSL_SUPPORT
	KviSSL         * m_pSSL;
#endif //COMPILE_SSL_SUPPORT
protected:
	void setConnectionTimeout(unsigned int uTimeout)
	{
		m_uConnectionTimeout = uTimeout;
		if(m_uConnectionTimeout < 5)
			m_uConnectionTimeout = 5; // keep it sane
	}
	int selectForReadStep();
	bool selectForRead(int iTimeoutInSecs);
	bool readDataStep();
	bool sendBuffer(const char *buffer,int bufLen,int iTimeoutInSecs);
	bool failure(const char *error=0);
	bool sslFailure();
	bool selectForWrite(int iTimeoutInSecs);
	bool connectToRemoteHost();
	bool processInternalEvents();
	void runInternal();
	virtual void run();
}; // class KviHttpRequestThread


#define KVI_HTTP_REQUEST_THREAD_EVENT_CONNECTED (KVI_THREAD_USER_EVENT_BASE + 0xCAFE)
#define KVI_HTTP_REQUEST_THREAD_EVENT_REQUESTSENT (KVI_THREAD_USER_EVENT_BASE + 0xCAFF)


#endif //_KviHttpRequestThread_h_
