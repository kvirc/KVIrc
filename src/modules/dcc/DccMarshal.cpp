//=============================================================================
//
//   File : DccMarshal.cpp
//   Creation date : Sun Sep 17 2000 15:59:11 by Szymon Stefanek
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

#include "DccMarshal.h"

#include "kvi_settings.h"
#include "KviNetUtils.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "KviMemory.h"
#include "kvi_socket.h"
#include "KviFileUtils.h"

#include <cstdlib> //for exit()

#include <QTimer>

DccMarshal::DccMarshal(DccMarshalOutputContext * ctx)
    : QObject(nullptr)
{
	setObjectName("dcc_marshal");
	m_pSn = nullptr;
	m_fd = KVI_INVALID_SOCKET;
	m_pTimeoutTimer = nullptr;
	m_bIPv6 = false;
	m_pOutputContext = ctx;
#ifdef COMPILE_SSL_SUPPORT
	m_pSSL = nullptr;
#endif
	m_szIp = "";
	m_szPort = "";
	m_szSecondaryIp = "";
	m_szSecondaryPort = "";
}

DccMarshal::~DccMarshal()
{
	reset();
}

kvi_socket_t DccMarshal::releaseSocket()
{
	kvi_socket_t aux_fd = m_fd;
	m_fd = KVI_INVALID_SOCKET;
	return aux_fd;
}

#ifdef COMPILE_SSL_SUPPORT
KviSSL * DccMarshal::releaseSSL()
{
	KviSSL * theSSL = m_pSSL;
	m_pSSL = nullptr;
	return theSSL;
}
#endif

void DccMarshal::reset()
{
	if(m_pSn)
	{
		delete m_pSn;
		m_pSn = nullptr;
	}
	if(m_fd != KVI_INVALID_SOCKET)
	{
		kvi_socket_close(m_fd);
		m_fd = KVI_INVALID_SOCKET;
	}
#ifdef COMPILE_SSL_SUPPORT
	//	qDebug("MARSHAL RESETTING (SSL=%d)",m_pSSL);
	if(m_pSSL)
	{
		//		qDebug("MARSHAL CLEARING THE SSL");
		KviSSLMaster::freeSSL(m_pSSL);
		m_pSSL = nullptr;
	}
#endif
	if(m_pTimeoutTimer)
	{
		delete m_pTimeoutTimer;
		m_pTimeoutTimer = nullptr;
	}
	m_bIPv6 = false;
}

KviError::Code DccMarshal::dccListen(const QString & ip, const QString & port, bool bUseTimeout, bool bUseSSL)
{
	if(m_fd != KVI_INVALID_SOCKET)
		return KviError::AnotherConnectionInProgress;

	m_szIp = ip;
	m_szPort = port;

	m_bOutgoing = false;

	m_bUseTimeout = bUseTimeout;

	if(m_pTimeoutTimer)
	{
		delete m_pTimeoutTimer;
		m_pTimeoutTimer = nullptr;
	}

#ifdef COMPILE_SSL_SUPPORT
	m_bUseSSL = bUseSSL;
#else
	if(bUseSSL)
		return KviError::NoSSLSupport;
#endif
	QTimer::singleShot(100, this, SLOT(doListen()));

	return KviError::Success;
}

void DccMarshal::doListen()
{
	// Check the address type
	if(!KviNetUtils::isValidStringIp(m_szIp))
	{
#ifdef COMPILE_IPV6_SUPPORT
		if(!KviNetUtils::isValidStringIPv6(m_szIp))
		{
			emit error(KviError::InvalidIpAddress);
			return;
		}
		else
			m_bIPv6 = true;
#else
		emit error(KviError::InvalidIpAddress);
		return;
#endif
	}

	bool bOk;
	m_uPort = m_szPort.toUInt(&bOk);
	if(!bOk)
	{
		emit error(KviError::InvalidPortNumber);
		return;
	}

#ifndef COMPILE_IPV6_SUPPORT
	if(m_bIPv6)
	{
		emit error(KviError::NoIPv6Support);
		return;
	}
#endif

#ifdef COMPILE_IPV6_SUPPORT
	m_fd = kvi_socket_create(m_bIPv6 ? KVI_SOCKET_PF_INET6 : KVI_SOCKET_PF_INET,
	    KVI_SOCKET_TYPE_STREAM, KVI_SOCKET_PROTO_TCP);
#else
	m_fd = kvi_socket_create(KVI_SOCKET_PF_INET, KVI_SOCKET_TYPE_STREAM, KVI_SOCKET_PROTO_TCP);
#endif

	if(m_fd == KVI_INVALID_SOCKET)
	{
		emit error(KviError::SocketCreationFailed);
		return;
	}

	if((!KVI_OPTION_BOOL(KviOption_boolUserDefinedPortRange)) || (m_uPort != 0))
	{
#ifdef COMPILE_IPV6_SUPPORT
		KviSockaddr sa(m_szIp.toUtf8().data(), m_uPort, m_bIPv6);
#else
		KviSockaddr sa(m_szIp.toUtf8().data(), m_uPort, false);
#endif

		if(!sa.socketAddress())
		{
			reset();
			emit error(KviError::BindFailed);
			return;
		}

		if(!kvi_socket_bind(m_fd, sa.socketAddress(), sa.addressLength()))
		{
			reset();
			emit error(KviError::BindFailed);
			return;
		}
	}
	else
	{
		m_uPort = KVI_OPTION_UINT(KviOption_uintDccMinPort);
		if(KVI_OPTION_UINT(KviOption_uintDccMaxPort) > 65535)
			KVI_OPTION_UINT(KviOption_uintDccMaxPort) = 65535;
		bool bBindSuccess;
		do
		{
#ifdef COMPILE_IPV6_SUPPORT
			KviSockaddr sa(m_szIp.toUtf8().data(), m_uPort, m_bIPv6);
#else
			KviSockaddr sa(m_szIp.toUtf8().data(), m_uPort, false);
#endif
			if(!sa.socketAddress())
			{
				reset();
				emit error(KviError::BindFailed);
				return;
			}

			bBindSuccess = kvi_socket_bind(m_fd, sa.socketAddress(), sa.addressLength());

			if(!bBindSuccess)
			{
				if(m_uPort == 65535)
				{
					reset();
					emit error(KviError::BindFailed);
					return;
				}
				m_uPort++;
			}

		} while((!bBindSuccess) && (m_uPort <= KVI_OPTION_UINT(KviOption_uintDccMaxPort)));

		if(!bBindSuccess)
		{
			reset();
			emit error(KviError::BindFailed);
			return;
		}
	}

	if(!kvi_socket_listen(m_fd, 1))
	{
		reset();
		emit error(KviError::ListenFailed);
		return;
	}

// Reread the port in case we're binding to a random one (0)

#ifdef COMPILE_IPV6_SUPPORT
	KviSockaddr sareal(0, m_bIPv6);
#else
	KviSockaddr sareal(0, false);
#endif

	int size = sareal.addressLength();

	if(kvi_socket_getsockname(m_fd, sareal.socketAddress(), &size))
	{
		//		qDebug("GETSOCKNAMEOK");
		m_szPort.setNum(sareal.port());
		m_uPort = sareal.port();
		//		qDebug("REALPORT %u",m_uPort);
	}
	else
	{
		//		qDebug("GETSOCKNAMEFAILED");
	}

	// and setup the READ notifier...
	m_pSn = new QSocketNotifier(m_fd, QSocketNotifier::Read);
	QObject::connect(m_pSn, SIGNAL(activated(int)), this, SLOT(snActivated(int)));
	m_pSn->setEnabled(true);

	// set the timer
	if(KVI_OPTION_UINT(KviOption_uintDccSocketTimeout) < 5)
		KVI_OPTION_UINT(KviOption_uintDccSocketTimeout) = 5;

	if(m_pTimeoutTimer)
	{
		delete m_pTimeoutTimer;
		m_pTimeoutTimer = nullptr;
	}

	if(m_bUseTimeout)
	{
		m_pTimeoutTimer = new QTimer();
		connect(m_pTimeoutTimer, SIGNAL(timeout()), this, SLOT(connectionTimedOut()));
		m_pTimeoutTimer->setSingleShot(true);
		m_pTimeoutTimer->setInterval((KVI_OPTION_UINT(KviOption_uintDccSocketTimeout) * 1000));
		m_pTimeoutTimer->start();
	}
	// and wait for connect

	emit inProgress();
}

KviError::Code DccMarshal::dccConnect(const char * ip, const char * port, bool bUseTimeout, bool bUseSSL)
{
	if(m_fd != KVI_INVALID_SOCKET)
		return KviError::AnotherConnectionInProgress;

	m_bUseTimeout = bUseTimeout;
	m_szIp = ip;
	m_szPort = port;
	m_bOutgoing = true;

#ifdef COMPILE_SSL_SUPPORT
	m_bUseSSL = bUseSSL;
#else
	if(bUseSSL)
		return KviError::NoSSLSupport;
#endif

	QTimer::singleShot(100, this, SLOT(doConnect()));
	return KviError::Success;
}

void DccMarshal::doConnect()
{
	if(m_pTimeoutTimer)
	{
		delete m_pTimeoutTimer;
		m_pTimeoutTimer = nullptr;
	}
	// Check the address type
	if(!KviNetUtils::isValidStringIp(m_szIp))
	{
#ifdef COMPILE_IPV6_SUPPORT
		if(!KviNetUtils::isValidStringIPv6(m_szIp))
		{
			emit error(KviError::InvalidIpAddress);
			return;
		}
		else
			m_bIPv6 = true;
#else
		emit error(KviError::InvalidIpAddress);
		return;
#endif
	}

	bool bOk;
	m_uPort = m_szPort.toUInt(&bOk);
	if(!bOk)
	{
		emit error(KviError::InvalidPortNumber);
		return;
	}

// create the socket
#ifdef COMPILE_IPV6_SUPPORT
	m_fd = kvi_socket_create(m_bIPv6 ? KVI_SOCKET_PF_INET6 : KVI_SOCKET_PF_INET,
	    KVI_SOCKET_TYPE_STREAM, KVI_SOCKET_PROTO_TCP);
#else
	m_fd = kvi_socket_create(KVI_SOCKET_PF_INET,
	    KVI_SOCKET_TYPE_STREAM, KVI_SOCKET_PROTO_TCP);
#endif
	if(m_fd == KVI_INVALID_SOCKET)
	{
		emit error(KviError::SocketCreationFailed);
		return;
	}

	// make it non blocking
	if(!kvi_socket_setNonBlocking(m_fd))
	{
		reset();
		emit error(KviError::AsyncSocketFailed);
		return;
	}

// fill the sockaddr structure

#ifdef COMPILE_IPV6_SUPPORT
	KviSockaddr sa(m_szIp.toUtf8().data(), m_uPort, m_bIPv6);
#else
	KviSockaddr sa(m_szIp.toUtf8().data(), m_uPort, false);
#endif

	if(!sa.socketAddress())
	{
		reset();
		emit error(KviError::SocketCreationFailed);
		return;
	}

	if(!kvi_socket_connect(m_fd, sa.socketAddress(), sa.addressLength()))
	{
		int err = kvi_socket_error();

		if(!kvi_socket_recoverableConnectError(err))
		{
			// Ops...
			int sockError = err;
			if(sockError == 0)
			{
				// Zero error ?...let's look closer
				int iSize = sizeof(int);
				if(!kvi_socket_getsockopt(m_fd, SOL_SOCKET, SO_ERROR,
				       (void *)&sockError, &iSize))
					sockError = 0;
			}
			// Die
			reset();
			// And declare problems :)
			if(sockError)
				emit error(KviError::translateSystemError(sockError));
			else
				emit error(KviError::UnknownError); //Error 0 ?
			return;
		}
	}

	// and setup the WRITE notifier...
	m_pSn = new QSocketNotifier(m_fd, QSocketNotifier::Write);
	QObject::connect(m_pSn, SIGNAL(activated(int)), this, SLOT(snActivated(int)));
	m_pSn->setEnabled(true);

	// set the timer
	if(KVI_OPTION_UINT(KviOption_uintDccSocketTimeout) < 5)
		KVI_OPTION_UINT(KviOption_uintDccSocketTimeout) = 5;

	if(m_pTimeoutTimer)
	{
		delete m_pTimeoutTimer;
		m_pTimeoutTimer = nullptr;
	}

	if(m_bUseTimeout)
	{
		m_pTimeoutTimer = new QTimer();
		connect(m_pTimeoutTimer, SIGNAL(timeout()), this, SLOT(connectionTimedOut()));
		m_pTimeoutTimer->setSingleShot(true);
		m_pTimeoutTimer->setInterval(KVI_OPTION_UINT(KviOption_uintDccSocketTimeout) * 1000);
		m_pTimeoutTimer->start();
	}

	// and wait for connect
	emit inProgress();
}

void DccMarshal::snActivated(int)
{
	if(m_pTimeoutTimer)
	{
		delete m_pTimeoutTimer;
		m_pTimeoutTimer = nullptr;
	}
#ifdef COMPILE_IPV6_SUPPORT
	struct sockaddr_in6 hostSockAddr6;
#endif
	struct sockaddr_in hostSockAddr;

	int size = sizeof(hostSockAddr);
	struct sockaddr * addr = (struct sockaddr *)&hostSockAddr;

#ifdef COMPILE_IPV6_SUPPORT
	if(m_bIPv6)
	{
		addr = (struct sockaddr *)&hostSockAddr6;
		size = sizeof(hostSockAddr6);
	}
#endif

	if(m_bOutgoing)
	{
		// outgoing connection (we have called connect())
		// Check for errors...
		int sockError;
		int iSize = sizeof(int);
		if(!kvi_socket_getsockopt(m_fd, SOL_SOCKET, SO_ERROR, (void *)&sockError, &iSize))
			sockError = -1;
		if(sockError != 0)
		{
			//failed
			KviError::Code eError;
			if(sockError > 0)
				eError = KviError::translateSystemError(sockError);
			else
				eError = KviError::UnknownError; //Error 0 ?
			reset();
			emit error(eError);
			return;
		}
		//Successfully connected...
		delete m_pSn;
		m_pSn = nullptr;
		// get the local address
		if(!kvi_socket_getsockname(m_fd, addr, &size))
		{
			m_szSecondaryIp = "localhost";
			m_szSecondaryPort = __tr2qs_ctx("unknown", "dcc");
		}
		else
		{
#ifdef COMPILE_IPV6_SUPPORT
			if(m_bIPv6)
			{
				m_szSecondaryPort.setNum(ntohs(((struct sockaddr_in6 *)addr)->sin6_port));
				if(!KviNetUtils::binaryIpToStringIp_V6(((struct sockaddr_in6 *)addr)->sin6_addr, m_szSecondaryIp))
					m_szSecondaryIp = "localhost";
			}
			else
			{
#endif
				m_szSecondaryPort.setNum(ntohs(((struct sockaddr_in *)addr)->sin_port));
				if(!KviNetUtils::binaryIpToStringIp(((struct sockaddr_in *)addr)->sin_addr, m_szSecondaryIp))
					m_szSecondaryIp = "localhost";
#ifdef COMPILE_IPV6_SUPPORT
			}
#endif
		}
	}
	else
	{
		// Incoming connection
		int newsock = kvi_socket_accept(m_fd, addr, &size);
		if(newsock != KVI_INVALID_SOCKET)
		{
			// Connected
			delete m_pSn;
			m_pSn = nullptr;
#ifdef COMPILE_IPV6_SUPPORT
			if(m_bIPv6)
			{
				m_szSecondaryPort.setNum(ntohs(((struct sockaddr_in6 *)addr)->sin6_port));
				if(!KviNetUtils::binaryIpToStringIp_V6(((struct sockaddr_in6 *)addr)->sin6_addr, m_szSecondaryIp))
					m_szSecondaryIp = __tr2qs_ctx("unknown", "dcc");
			}
			else
			{
#endif
				m_szSecondaryPort.setNum(ntohs(((struct sockaddr_in *)addr)->sin_port));
				if(!KviNetUtils::binaryIpToStringIp(((struct sockaddr_in *)addr)->sin_addr, m_szSecondaryIp))
					m_szSecondaryIp = __tr2qs_ctx("unknown", "dcc");
#ifdef COMPILE_IPV6_SUPPORT
			}
#endif
			kvi_socket_close(m_fd);
			m_fd = newsock;
			if(!kvi_socket_setNonBlocking(m_fd))
			{
				reset();
				emit error(KviError::AsyncSocketFailed);
				return;
			}
		}
		else
		{
			// Huh ?.. wait for the next notifier call
			return;
		}
	}

#ifdef COMPILE_SSL_SUPPORT
	// SSL Handshake needed ?
	if(m_bUseSSL)
	{
		m_pSSL = KviSSLMaster::allocSSL(m_pOutputContext->dccMarshalOutputWindow(), m_fd, m_bOutgoing ? KviSSL::Client : KviSSL::Server, m_pOutputContext->dccMarshalOutputContextString());

		if(m_pSSL)
		{
			emit startingSSLHandshake();
			doSSLHandshake(0);
		}
		else
		{
			reset();
			emit error(KviError::SSLError);
		}
		return;
	}
#endif

	emit connected();
}

void DccMarshal::doSSLHandshake(int)
{
#ifdef COMPILE_SSL_SUPPORT
	//	qDebug("DO SSL HANDSHAKE");
	if(m_pSn)
	{
		delete m_pSn;
		m_pSn = nullptr;
	}

	if(!m_pSSL)
	{
		qDebug("Oops! I've lost the SSL class?");
		reset();
		emit error(KviError::InternalError);
		return; // ops ?
	}

	KviSSL::Result r = m_bOutgoing ? m_pSSL->connect() : m_pSSL->accept();

	switch(r)
	{
		case KviSSL::Success:
			// done!
			//			qDebug("EMITTING CONNECTED");
			emit connected();
			//			qDebug("CONNECTED EMITTED");
			break;
		case KviSSL::WantRead:
			m_pSn = new QSocketNotifier((int)m_fd, QSocketNotifier::Read);
			QObject::connect(m_pSn, SIGNAL(activated(int)), this, SLOT(doSSLHandshake(int)));
			m_pSn->setEnabled(true);
			break;
		case KviSSL::WantWrite:
			m_pSn = new QSocketNotifier((int)m_fd, QSocketNotifier::Write);
			QObject::connect(m_pSn, SIGNAL(activated(int)), this, SLOT(doSSLHandshake(int)));
			m_pSn->setEnabled(true);
			break;
		case KviSSL::RemoteEndClosedConnection:
			reset();
			emit error(KviError::RemoteEndClosedConnection);
			break;
		case KviSSL::SyscallError:
		{
			// syscall problem
			int err = kvi_socket_error();
			if(kvi_socket_recoverableError(err))
			{
				// can recover ? (EAGAIN, EINTR ?)
				m_pSn = new QSocketNotifier((int)m_fd, QSocketNotifier::Write);
				QObject::connect(m_pSn, SIGNAL(activated(int)), this, SLOT(doSSLHandshake(int)));
				m_pSn->setEnabled(true);
				return;
			}
			else
			{
				// Declare problems :)
				reset();
				emit error(err ? KviError::translateSystemError(err) : KviError::UnknownError);
			}
		}
		break;
		default:
		{
			KviCString buffer;
			while(m_pSSL->getLastErrorString(buffer))
				emit sslError(buffer.ptr());
			reset();
			emit error(KviError::SSLError);
		}
		break;
	}
#else  //!COMPILE_SSL_SUPPORT
	qDebug("Oops! SSL handshake without SSL support! Aborting!");
	exit(-1);
#endif //!COMPILE_SSL_SUPPORT
}

void DccMarshal::abort()
{
	reset();
}

void DccMarshal::connectionTimedOut()
{
	reset();
	emit error(KviError::ConnectionTimedOut);
}
