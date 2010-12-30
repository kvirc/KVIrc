//=============================================================================
//
//   File : KviHttpRequestThread.cpp
//   Creation date : Sat Aug 17 13:43:32 2002 GMT by Szymon Stefanek
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

#include "KviHttpRequestThread.h"
#include "KviHttpRequest.h"

#include "KviLocale.h"
#include "KviNetUtils.h"
#include "KviError.h"
#include "KviTimeUtils.h"
#include "KviDataBuffer.h"

#include "kvi_debug.h"
#include "kvi_socket.h"

#ifdef COMPILE_SSL_SUPPORT
	#include "KviSSL.h"
#endif

//#include <zlib.h>


KviHttpRequestThread::KviHttpRequestThread(
		KviHttpRequest * r,
		const QString &szHost,
		const QString &szIp,
		unsigned short uPort,
		const QString & szPath,
		unsigned int uContentOffset,
		RequestMethod m,
		const QString &szPostData,
		bool bUseSSL
	) : KviSensitiveThread()
{
	m_pRequest = r;
	m_szHost = szHost;
	m_szIp = szIp;
	m_szPath = szPath;
	m_uPort = uPort > 0 ? uPort : 80;
	m_uContentOffset = uContentOffset;
	m_eRequestMethod = m;
	m_szPostData = szPostData;
	m_sock = KVI_INVALID_SOCKET;
	m_bUseSSL = bUseSSL;
	m_uConnectionTimeout = 60;
#ifdef COMPILE_SSL_SUPPORT
	m_pSSL = 0;
#endif
}

KviHttpRequestThread::~KviHttpRequestThread()
{
}

bool KviHttpRequestThread::processInternalEvents()
{
	while(KviThreadEvent *e = dequeueEvent())
	{
		switch(e->id())
		{
			case KVI_THREAD_EVENT_TERMINATE:
			{
				delete e;
				return false;
			}
			break;
			default:
				qDebug("Unrecognized event in http thread");
				delete e;
				return false;
			break;
		}
	}

	return true;
}

bool KviHttpRequestThread::failure(const char *error)
{
	if(error)
	{
		postEvent(m_pRequest,new KviThreadDataEvent<KviCString>(KVI_THREAD_EVENT_ERROR,new KviCString(error)));
	} /*else {
		postEvent(m_pRequest,new KviThreadDataEvent<KviCString>(KVI_THREAD_EVENT_ERROR,new KviCString(__tr2qs("Aborted"))));
	}*/
	return false;
}

bool KviHttpRequestThread::selectForWrite(int iTimeoutInSecs)
{

	kvi_time_t startTime = kvi_unixTime();

	for(;;)
	{
		if(!processInternalEvents())
		{
			return failure(0);
		}

		fd_set writeSet;

		FD_ZERO(&writeSet);

		FD_SET(m_sock,&writeSet);

		struct timeval tmv;
		tmv.tv_sec  = 0;
		tmv.tv_usec = 1000; // we wait 1000 usecs for an event


		int nRet = kvi_socket_select(m_sock + 1,0,&writeSet,0,&tmv);

		if(nRet > 0)
		{
			if(FD_ISSET(m_sock,&writeSet))
			{
				// connected!
				return true;
			}
		} else {
			if(nRet < 0)
			{
				int err = kvi_socket_error();
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
				if((err != EAGAIN) && (err != EINTR) && (err != WSAEWOULDBLOCK))
#else
				if((err != EAGAIN) && (err != EINTR))
#endif
				{
					return failure(KviError::getUntranslatedDescription(KviError::translateSystemError(err)));
				}
			}
		}


		if((time(0) - startTime) > iTimeoutInSecs)
			return failure(__tr_no_lookup("Operation timed out"));

		usleep(100000); // 1/10 sec
	}

	return false;
}

bool KviHttpRequestThread::sslFailure()
{
#ifdef COMPILE_SSL_SUPPORT
	KviCString buffer;
	if(m_pSSL->getLastErrorString(buffer))
	{
		failure(buffer.ptr());
	} else {
		failure(__tr_no_lookup("Unexpected SSL error"));
	}
#endif
	return false;
}

bool KviHttpRequestThread::connectToRemoteHost()
{
	m_sock = kvi_socket_create(KVI_SOCKET_PF_INET,KVI_SOCKET_TYPE_STREAM,0); //tcp
	if(m_sock == KVI_INVALID_SOCKET)
		return failure(__tr_no_lookup("Failed to create the socket"));

	if(!kvi_socket_setNonBlocking(m_sock))
		return failure(__tr_no_lookup("Failed to enter non blocking mode"));

	sockaddr_in saddr;

	if(!KviNetUtils::stringIpToBinaryIp(m_szIp,&(saddr.sin_addr)))
		return failure(__tr_no_lookup("Invalid target address"));

	saddr.sin_port = htons(m_uPort);
	saddr.sin_family = AF_INET;

	if(!kvi_socket_connect(m_sock,(struct sockaddr *)&saddr,sizeof(saddr)))
	{
		int err = kvi_socket_error();
		if(!kvi_socket_recoverableConnectError(err))
		{
			return failure(KviError::getUntranslatedDescription(KviError::translateSystemError(err)));
		}
	}

	// now loop selecting for write

	if(!selectForWrite(m_uConnectionTimeout))
		return false;

	int sockError;
	int iSize = sizeof(sockError);
	if(!kvi_socket_getsockopt(m_sock,SOL_SOCKET,SO_ERROR,(void *)&sockError,&iSize))sockError = -1;
	if(sockError != 0)
	{
		//failed
		KviError::Code eError;
		if(sockError > 0)
			eError = KviError::translateSystemError(sockError);
		else
			eError = KviError::UnknownError;
		return failure(KviError::getUntranslatedDescription(eError));
	}

#ifdef COMPILE_SSL_SUPPORT
	if(m_bUseSSL)
	{
		m_pSSL = new KviSSL();
		if(!m_pSSL->initContext(KviSSL::Client))
			return failure(__tr_no_lookup("Failed to initialize the SSL context"));
		if(!m_pSSL->initSocket(m_sock))
			return failure(__tr_no_lookup("Failed to initialize the SSL connection"));

		for(;;)
		{
			switch(m_pSSL->connect())
			{
				case KviSSL::Success:
					// done: connected.
					return true;
				break;
				case KviSSL::WantRead:
					if(!selectForRead(m_uConnectionTimeout))
						return false;
				break;
				case KviSSL::WantWrite:
					if(!selectForWrite(m_uConnectionTimeout))
						return false;
				break;
				case KviSSL::RemoteEndClosedConnection:
					return failure(__tr_no_lookup("Remote end has closed the connection"));
				break;
				case KviSSL::SSLError:
					return sslFailure();
				break;
				case KviSSL::SyscallError:
				{
					// syscall problem
					int err = kvi_socket_error();
					if(!kvi_socket_recoverableError(err))
					{
						// Declare problems :)
						return failure(__tr_no_lookup("Unrecoverable SSL error during handshake"));
					} // else can recover ? (EAGAIN, EINTR ?) ... should select for read or for write
				}
				break;
				default:
					return sslFailure();
				break;
			}
		}

		// never here
		return true;
	}
#endif

	return true;
}


bool KviHttpRequestThread::sendBuffer(const char * buffer,int bufLen,int iTimeoutInSecs)
{
	const char * ptr = buffer;
	int curLen       = bufLen;

	time_t startTime = time(0);

	for(;;)
	{
		if(!processInternalEvents())
			return failure();

		int wrtn;
#ifdef COMPILE_SSL_SUPPORT
		if(m_pSSL)
		{
			wrtn = m_pSSL->write((char *)ptr,curLen);
		} else {
#endif
			wrtn = kvi_socket_send(m_sock,ptr,curLen);
#ifdef COMPILE_SSL_SUPPORT
		}
#endif

		if(wrtn > 0)
		{
			curLen -= wrtn;

			if(curLen <= 0)
				break;

			ptr += wrtn;
		} else {
			if(wrtn < 0)
			{
#ifdef COMPILE_SSL_SUPPORT
				if(m_pSSL)
				{
					// ops...might be an SSL error
					switch(m_pSSL->getProtocolError(wrtn))
					{
						case KviSSL::WantWrite:
							if(!selectForWrite(m_uConnectionTimeout))
								return false;
							break;
						case KviSSL::WantRead:
							if(!selectForRead(m_uConnectionTimeout))
								return false;
							break;
						case KviSSL::SyscallError:
							if(wrtn == 0)
							{
								return failure(__tr_no_lookup("Remote end has closed the connection"));
							} else {
								int iSSLErr = m_pSSL->getLastError(true);
								if(iSSLErr != 0)
								{
									return sslFailure();
								} else {
									goto handle_system_error;
								}
							}
						break;
						case KviSSL::SSLError:
							return sslFailure();
						break;
						default:
							return sslFailure();
						break;
					}
				} else {
#endif //COMPILE_SSL_SUPPORT

handle_system_error:
					int err = kvi_socket_error();
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
					if((err != EAGAIN) && (err != EINTR) && (err != WSAEWOULDBLOCK))
#else
					if((err != EAGAIN) && (err != EINTR))
#endif
					{
						return failure(KviError::getUntranslatedDescription(KviError::translateSystemError(err)));
					}
#ifdef COMPILE_SSL_SUPPORT
				}
#endif
			}
		}

		int diff = time(0) - startTime;
		if(diff > iTimeoutInSecs)
			return failure(__tr_no_lookup("Operation timed out"));

		usleep(10000);
	}

	return true;
}

int KviHttpRequestThread::selectForReadStep()
{
	// calls select on the main socket
	// returns 1 if there is data available for reading
	// returns 0 if there is no data available but there was no error
	// returns -1 if there was a critical error (socket closed)
	fd_set readSet;

	FD_ZERO(&readSet);

	FD_SET(m_sock,&readSet);

	struct timeval tmv;
	tmv.tv_sec  = 0;
	tmv.tv_usec = 1000; // we wait 1000 usecs for an event


	int nRet = kvi_socket_select(m_sock + 1,&readSet,0,0,&tmv);

	if(nRet > 0)
	{
		if(FD_ISSET(m_sock,&readSet))
		{
			// ok
			return 1;
		}
	} else {
		if(nRet < 0)
		{
			int err = kvi_socket_error();
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
			if((err != EAGAIN) && (err != EINTR) && (err != WSAEWOULDBLOCK))
#else
			if((err != EAGAIN) && (err != EINTR))
#endif
			{
				failure(KviError::getUntranslatedDescription(KviError::translateSystemError(err)));
				return -1;
			}
		}
	}

	return 0;
}

bool KviHttpRequestThread::selectForRead(int iTimeoutInSecs)
{
	// waits for some data to arrive on the socket
	// up to iTimeoutInSecs seconds
	// returns true if data is available on the socket
	// or false if there was a select() error or no data
	// was available in the specified amount of time

	time_t startTime = time(0);

	for(;;)
	{
		if(!processInternalEvents())
		{
			return failure(); // ensure that the socket is closed
		}

		int nRet = selectForReadStep();

		if(nRet < 0)return false;
		if(nRet > 0)return true;

		int diff = time(0) - startTime;
		if(diff > iTimeoutInSecs)
			return failure(__tr_no_lookup("Operation timed out (while selecting for read)"));

		usleep(100000); // 1/10 sec
	}

	return false;
}

bool KviHttpRequestThread::readDataStep()
{
	unsigned char buffer[2048];
	int readed;


#ifdef COMPILE_SSL_SUPPORT
	if(m_pSSL)
	{
		readed = m_pSSL->read((char *)buffer,2048);
		if(readed <= 0)
		{
			// ssl error....?
			switch(m_pSSL->getProtocolError(readed))
			{
				case KviSSL::ZeroReturn:
					readed = 0;
				break;
				case KviSSL::WantRead:
					return selectForRead(m_uConnectionTimeout);
				break;
				case KviSSL::WantWrite:
					return selectForWrite(m_uConnectionTimeout);
				break;
				case KviSSL::SyscallError:
				{
					int iE = m_pSSL->getLastError(true);
					if(iE != 0)return sslFailure();
				}
				break;
				case KviSSL::SSLError:
					return sslFailure();
				break;
				default:
					return sslFailure();
				break;
			}
		}
	} else {
#endif
		readed = kvi_socket_read(m_sock,buffer,2048);
#ifdef COMPILE_SSL_SUPPORT
	}
#endif

	if(readed > 0)
	{
		postEvent(m_pRequest,new KviThreadDataEvent<KviDataBuffer>(KVI_THREAD_EVENT_BINARYDATA,new KviDataBuffer(readed,buffer)));
	} else {
		if(readed < 0)
		{
			// Read error ?
			int err = kvi_socket_error();
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
			if((err != EAGAIN) && (err != EINTR) && (err != WSAEWOULDBLOCK))
#else
			if((err != EAGAIN) && (err != EINTR))
#endif
			{
				// yes...read error
				return failure(KviError::getUntranslatedDescription(KviError::translateSystemError(err)));
			}
			return selectForRead(m_uConnectionTimeout); // EINTR or EAGAIN...transient problem
		} else {
			// readed == 0
			// Connection closed by remote host
			postEvent(m_pRequest,new KviThreadEvent(KVI_THREAD_EVENT_SUCCESS));
			return false;
		}
	}
	return selectForRead(m_uConnectionTimeout);
}

void KviHttpRequestThread::run()
{
	// setup:
	//    nothing needed

	// run:
	runInternal();

	// cleanup:
#ifdef COMPILE_SSL_SUPPORT
	if(m_pSSL)
	{
		delete m_pSSL;
		m_pSSL = 0;
	}
#endif

	if(kvi_socket_isValid(m_sock))
	{
		kvi_socket_close(m_sock);
		m_sock = KVI_INVALID_SOCKET;
	}
}

void KviHttpRequestThread::runInternal()
{
#ifndef COMPILE_SSL_SUPPORT
	if(m_bUseSSL)
	{
		failure(__tr_no_lookup("This KVIrc executable has no SSL support"));
		return;
	}
#endif

	if(!connectToRemoteHost())return;

	postEvent(m_pRequest,new KviThreadEvent(KVI_HTTP_REQUEST_THREAD_EVENT_CONNECTED));

	// FIXME: Other headers ?

	KviCString szMethod;
	switch(m_eRequestMethod)
	{
		case Head: szMethod = "HEAD"; break;
		case Post: szMethod = "POST"; break;
		case Get: szMethod = "GET"; break;
	}

	KviCString szRequest(KviCString::Format,"%s %s HTTP/1.1\r\n" \
				"Host: %s\r\n" \
				"Connection: Close\r\n" \
				"User-Agent: KVIrc-http-slave/1.0.0\r\n" \
				"Accept: */*\r\n",
				szMethod.ptr(),KviQString::toUtf8(m_szPath).data(),KviQString::toUtf8(m_szHost).data());

	if(m_uContentOffset > 0)
		szRequest.append(KviCString::Format,"Range: bytes=%u-\r\n",m_uContentOffset);

	if(m_eRequestMethod == Post)
	{
		szRequest.append(KviCString::Format,"Content-Type: application/x-www-form-urlencoded\r\n" \
				"Content-Length: %u\r\n" \
				"Cache-control: no-cache\r\n" \
				"Pragma: no-cache\r\n",m_szPostData.length());
	}

	szRequest += "\r\n";

	if(m_eRequestMethod == Post)
	{
		if(!m_szPostData.isEmpty())
			szRequest.append(m_szPostData);
		szRequest += "\r\n";
	}

	//qDebug("SENDING REQUEST:\n%s",szRequest.ptr());

	if(!sendBuffer(szRequest.ptr(),szRequest.len(),60))return;

	// now loop reading data
	postEvent(m_pRequest,new KviThreadDataEvent<QString>(KVI_HTTP_REQUEST_THREAD_EVENT_REQUESTSENT,new QString(szRequest)));

	for(;;)
	{
		if(!readDataStep())return;
	}
}

