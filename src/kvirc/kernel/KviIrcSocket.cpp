//=============================================================================
//
//   File : KviIrcSocket.cpp
//   Creation date : Tue Jul 30 19:25:18 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviIrcSocket.h"
#include "KviIrcServer.h"
#include "KviProxy.h"
#include "KviProxyDataBase.h"
#include "KviNetUtils.h"
#include "kvi_settings.h"
#include "KviLocale.h"
#include "KviMemory.h"
#include "kvi_debug.h"
#include "KviCString.h"
#include "KviOptions.h"
#include "KviConsoleWindow.h"
#include "kvi_out.h"
#include "KviIrcLink.h"
#include "KviIrcConnection.h"
#include "KviDataBuffer.h"

#ifdef COMPILE_SSL_SUPPORT
#include "KviSSLMaster.h"
#endif

#include <QTimer>
#include <QSocketNotifier>
#include <memory>

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
#include <unistd.h> //for gettimeofday()
#endif
//#include <fcntl.h>
//#include <errno.h>

// FIXME: #warning "Lag-o-meter"

unsigned int g_uNextIrcLinkId = 1;

KviIrcSocket::KviIrcSocket(KviIrcLink * pLink)
    : QObject(), m_pLink(pLink)
{
	m_uId = g_uNextIrcLinkId;
	g_uNextIrcLinkId++;

	m_pConsole = m_pLink->console();

	m_tAntiFloodLastMessageTime.tv_sec = 0;
	m_tAntiFloodLastMessageTime.tv_usec = 0;

	if(KVI_OPTION_UINT(KviOption_uintSocketQueueFlushTimeout) < 100)
		KVI_OPTION_UINT(KviOption_uintSocketQueueFlushTimeout) = 100; // this is our minimum, we don't want to lag the app

	m_pFlushTimer = std::make_unique<QTimer>(); // queue flush timer
	connect(m_pFlushTimer.get(), SIGNAL(timeout()), this, SLOT(flushSendQueue()));
}

KviIrcSocket::~KviIrcSocket()
{
	reset();
}

void KviIrcSocket::reset()
{
#ifdef COMPILE_SSL_SUPPORT
	if(m_pSSL)
	{
		KviSSLMaster::freeSSL(m_pSSL);
		m_pSSL = nullptr;
	}
#endif
	if(m_pIrcServer)
	{
		delete m_pIrcServer;
		m_pIrcServer = nullptr;
	}

	if(m_pProxy)
	{
		delete m_pProxy;
		m_pProxy = nullptr;
	}

	if(m_pRsn)
	{
		delete m_pRsn;
		m_pRsn = nullptr;
	}

	if(m_pWsn)
	{
		delete m_pWsn;
		m_pWsn = nullptr;
	}

	if(kvi_socket_isValid(m_sock))
	{
		kvi_socket_destroy(m_sock);
		m_sock = KVI_INVALID_SOCKET;
	}

	if(m_pTimeoutTimer)
	{
		m_pTimeoutTimer->stop();
		delete m_pTimeoutTimer;
		m_pTimeoutTimer = nullptr;
	}

	m_bInProcessData = false;

	m_uReadBytes = 0;
	m_uSentBytes = 0;
	m_uSentPackets = 0;
	m_tAntiFloodLastMessageTime.tv_sec = 0;
	m_tAntiFloodLastMessageTime.tv_usec = 0;

	m_bInProcessData = false;

	if(m_pFlushTimer->isActive())
		m_pFlushTimer->stop();

	queue_removeAllMessages();

	setState(Idle);
}

void KviIrcSocket::clearOutputQueue(bool bPrivateMessagesOnly)
{
	if(bPrivateMessagesOnly)
		queue_removePrivateMessages();
	else
		queue_removeAllMessages();
}

unsigned int KviIrcSocket::outputQueueSize()
{
	KviIrcSocketMsgEntry * pMsg = m_pSendQueueTail;
	if(!pMsg)
		return 0;

	unsigned int uCount = 0;

	do
	{
		uCount++;
		pMsg = pMsg->next_ptr;
	} while(pMsg);

	return uCount;
}

void KviIrcSocket::outputSSLMessage(const QString & szMsg)
{
	m_pConsole->output(KVI_OUT_SSL, __tr2qs("[SSL]: %Q"), &szMsg);
}

void KviIrcSocket::outputSSLError(const QString & szMsg)
{
	m_pConsole->output(KVI_OUT_SSL, __tr2qs("[SSL ERROR]: %Q"), &szMsg);
}

void KviIrcSocket::outputProxyMessage(const QString & szMsg)
{
	for(const auto & it : szMsg.split('\n', QString::SkipEmptyParts))
	{
		QString szTemporary = it.trimmed();
		m_pConsole->output(KVI_OUT_SOCKETMESSAGE, __tr2qs("[PROXY]: %Q"), &szTemporary);
	}
}

void KviIrcSocket::outputProxyError(const QString & szMsg)
{
	for(const auto & it : szMsg.split('\n', QString::SkipEmptyParts))
	{
		QString szTemporary = it.trimmed();
		m_pConsole->output(KVI_OUT_SOCKETERROR, __tr2qs("[PROXY ERROR]: %Q"), &szTemporary);
	}
}

void KviIrcSocket::outputSocketMessage(const QString & szMsg)
{
	m_pConsole->output(KVI_OUT_SOCKETMESSAGE, __tr2qs("[SOCKET]: %Q"), &szMsg);
}

void KviIrcSocket::outputSocketError(const QString & szMsg)
{
	m_pConsole->output(KVI_OUT_SOCKETERROR, __tr2qs("[SOCKET ERROR]: %Q"), &szMsg);
}

void KviIrcSocket::outputSocketWarning(const QString & szMsg)
{
	m_pConsole->output(KVI_OUT_SOCKETWARNING, __tr2qs("[SOCKET WARNING]: %Q"), &szMsg);
}

void KviIrcSocket::setState(SocketState state)
{
	if(state != m_state)
	{
		m_state = state;
		m_pLink->socketStateChange();
	}
}

void KviIrcSocket::raiseError(KviError::Code eError)
{
	m_eLastError = eError;
	//m_pConsole->socketError(iError);
	if((m_eLastError == KviError::RemoteEndClosedConnection) && (m_state == ProxyHttpError))
		outputSocketMessage(KviError::getDescription(eError));
	else
		outputSocketError(KviError::getDescription(eError));
}

KviError::Code KviIrcSocket::startConnection(KviIrcServer * pServer, KviProxy * pProxy, const char * pcBindAddress)
{
	// Attempts to establish an IRC connection
	// to the server specified by *srv.
	// Uses the proxy *prx if not 0
	if(m_state != Idle)
		return KviError::AnotherConnectionInProgress;

	// Coherent state, thnx.
	reset();

#ifndef COMPILE_SSL_SUPPORT
	if(pServer->useSSL())
	{
		return KviError::NoSSLSupport;
	}
#endif //COMPILE_SSL_SUPPORT

	// Copy the server
	m_pIrcServer = new KviIrcServer(*pServer);

	bool bTargetIPv6 = false;
	bool bNeedServerIp = !pProxy;
	if(pProxy)
		bNeedServerIp = ((pProxy->protocol() != KviProxy::Http) && (pProxy->protocol() != KviProxy::Socks5));

	// We're going to check the addresses now

	// check the proxy stuff...
	if(pProxy)
	{
		// Yeah...here comes the proxy
		m_pProxy = new KviProxy(*pProxy);
		// check the proxy IP address
		if(m_pProxy->isIPv6())
		{
// IPv6 proxy :) (STILL QUITE UNTESTED ?)
#ifdef COMPILE_IPV6_SUPPORT
			bTargetIPv6 = true;
			if(!KviNetUtils::isValidStringIPv6(m_pProxy->ip()))
				return KviError::InvalidProxyAddress;
			// SOCKSv4 does not support IPv6 addresses
			if(m_pProxy->protocol() == KviProxy::Socks4)
				return KviError::SocksV4LacksIPv6Support;
#else
			return KviError::NoIPv6Support;
#endif
		}
		else
		{
			// IPv4 proxy
			if(!KviNetUtils::isValidStringIp(m_pProxy->ip()))
				return KviError::InvalidProxyAddress;
		}
	}

	if(bNeedServerIp)
	{
// check the IRC host IP
#ifdef COMPILE_IPV6_SUPPORT
		if(m_pIrcServer->isIPv6())
		{
			// We have an IPv6 server host (Interesting if proxy is IPv4)
			if(!KviNetUtils::isValidStringIPv6(m_pIrcServer->ip()))
				return KviError::InvalidIpAddress;
			if(!m_pProxy)
				bTargetIPv6 = true; // otherwise the proxy rules
		}
		else
		{
#endif
			// We have an IPv4 server host
			if(!KviNetUtils::isValidStringIp(m_pIrcServer->ip()))
				return KviError::InvalidIpAddress;
#ifdef COMPILE_IPV6_SUPPORT
		}
#endif
	}

	KviSockaddr sa(pProxy ? m_pProxy->ip().toUtf8().data() : m_pIrcServer->ip().toUtf8().data(), pProxy ? m_pProxy->port() : m_pIrcServer->port(), bTargetIPv6);

	if(!sa.socketAddress())
		return KviError::InvalidIpAddress;

// create the socket
#ifdef COMPILE_IPV6_SUPPORT
	m_sock = kvi_socket_create(bTargetIPv6 ? KVI_SOCKET_PF_INET6 : KVI_SOCKET_PF_INET, KVI_SOCKET_TYPE_STREAM, KVI_SOCKET_PROTO_TCP);
#else
	m_sock = kvi_socket_create(KVI_SOCKET_PF_INET, KVI_SOCKET_TYPE_STREAM, KVI_SOCKET_PROTO_TCP);
#endif

	if(m_sock < 0)
		return KviError::SocketCreationFailed;

	if(pcBindAddress)
	{
		// we have to bind the socket to a local address
		KviSockaddr localSa(pcBindAddress, 0, bTargetIPv6);
		bool bBindOk = localSa.socketAddress();

		if(bBindOk)
		{
			bBindOk = kvi_socket_bind(m_sock, localSa.socketAddress(), ((int)(localSa.addressLength())));
		}

		QString szTmp;
		if(bBindOk)
		{
			if(_OUTPUT_VERBOSE)
				szTmp = QString(__tr2qs("Binding to local address %1")).arg(pcBindAddress);
			outputSocketMessage(szTmp);
		}
		else
		{
			if(_OUTPUT_VERBOSE)
				szTmp = QString(__tr2qs("Binding to local address %1 failed: the kernel will choose the correct interface")).arg(pcBindAddress);
			outputSocketWarning(szTmp);
		}
	}

	// make it non blocking
	if(!kvi_socket_setNonBlocking(m_sock))
	{
		reset();
		return KviError::AsyncSocketFailed;
	}

	if(!kvi_socket_connect(m_sock, sa.socketAddress(), ((int)(sa.addressLength()))))
	{
		// Oops!
		int iErr = kvi_socket_error();

		if(!kvi_socket_recoverableConnectError(iErr))
		{
			// Oops!
			int iSockError = iErr;
			if(iSockError == 0)
			{
				// Zero error ?...let's look closer
				int iSize = sizeof(int);
				if(!kvi_socket_getsockopt(m_sock, SOL_SOCKET, SO_ERROR, (void *)&iSockError, &iSize))
					iSockError = 0;
			}
			// die :(
			reset();
			// And declare problems :)
			if(iSockError)
				return KviError::translateSystemError(iSockError);
			else
				return KviError::UnknownError; //Error 0 ?
		}
	}

	// and setup the WRITE notifier...
	m_pWsn = new QSocketNotifier((int)m_sock, QSocketNotifier::Write);
	QObject::connect(m_pWsn, SIGNAL(activated(int)), this, SLOT(writeNotifierFired(int)));
	m_pWsn->setEnabled(true);

	// set the timer
	if(KVI_OPTION_UINT(KviOption_uintIrcSocketTimeout) < 5)
		KVI_OPTION_UINT(KviOption_uintIrcSocketTimeout) = 5;

	m_pTimeoutTimer = new QTimer();
	QObject::connect(m_pTimeoutTimer, SIGNAL(timeout()), this, SLOT(connectionTimedOut()));
	m_pTimeoutTimer->setSingleShot(true);
	m_pTimeoutTimer->setInterval(KVI_OPTION_UINT(KviOption_uintIrcSocketTimeout) * 1000);
	m_pTimeoutTimer->start();

	// and wait for connect
	setState(Connecting);

	return KviError::Success;
}

void KviIrcSocket::connectionTimedOut()
{
	// the m_pTimeoutTimer fired :(
	raiseError(KviError::ConnectionTimedOut);
	reset();
}

void KviIrcSocket::writeNotifierFired(int)
{
	// kill the timeout timer
	if(m_pTimeoutTimer)
	{
		delete m_pTimeoutTimer;
		m_pTimeoutTimer = nullptr;
	}

	// Check for errors...
	int iSockError;
	int iSize = sizeof(int);
	if(!kvi_socket_getsockopt(m_sock, SOL_SOCKET, SO_ERROR, (void *)&iSockError, &iSize))
		iSockError = -1;

	//sockError = 0;
	if(iSockError != 0)
	{
		//failed
		KviError::Code eError;
		if(iSockError > 0)
			eError = KviError::translateSystemError(iSockError);
		else
			eError = KviError::UnknownError; //Error 0 ?

		raiseError(eError);
		reset();
		return;
	}

	// kill the write notifier
	delete m_pWsn;
	m_pWsn = nullptr;

	//Successfully connected...
	connectionEstablished();
}

void KviIrcSocket::connectionEstablished()
{
	if(m_sock == KVI_INVALID_SOCKET)
		return; // ops...disconnected in setState() ????

	if(m_pProxy)
		connectedToProxy();
	else
		connectedToIrcServer();
}

void KviIrcSocket::connectedToProxy()
{
	if(!m_pProxy)
		qDebug("WARNING: connectedToProxy() without a m_pProxy!");

	// FIXME: Do we want to support SSL proxies ?
	//        it would be just a matter of SSL handshaking
	//        with the proxy

	setState(ProxyLogin);

	if(m_pRsn)
	{
		delete m_pRsn;
		m_pRsn = nullptr;
	}

	m_pRsn = new QSocketNotifier((int)m_sock, QSocketNotifier::Read);

	QObject::connect(m_pRsn, SIGNAL(activated(int)), this, SLOT(readProxyData(int)));

	switch(m_pProxy->protocol())
	{
		case KviProxy::Http:
			proxyLoginHttp();
			break;
		case KviProxy::Socks5:
			proxyLoginV5();
			break;
		default:
			proxyLoginV4();
			break;
	}
}

void KviIrcSocket::readHttpProxyErrorData(int)
{
	char cBuffer[256];
	int iReadLength;

	iReadLength = kvi_socket_recv(m_sock, cBuffer, 255);
	if(iReadLength <= 0)
	{
		handleInvalidSocketRead(iReadLength);
		return;
	}

	outputProxyMessage(m_pConsole->decodeText(cBuffer));
}

void KviIrcSocket::connectedToIrcServer()
{
#ifdef COMPILE_SSL_SUPPORT
	if(m_pIrcServer->useSSL())
	{
		enterSSLMode();
		return;
	}
#endif //COMPILE_SSL_SUPPORT
	linkUp();
}

#ifdef COMPILE_SSL_SUPPORT
void KviIrcSocket::enterSSLMode()
{
	Q_ASSERT(!m_pSSL); // Don't call this function twice in a session

	m_pSSL = KviSSLMaster::allocSSL(m_pConsole, m_sock, KviSSL::Client);
	if(!m_pSSL)
	{
		raiseSSLError();
		raiseError(KviError::SSLError);
		reset();
		return;
	}
	setState(SSLHandshake);
	doSSLHandshake(0);
}
#endif //COMPILE_SSL_SUPPORT

void KviIrcSocket::readProxyData(int)
{
	char cBuffer[256];
	int iReadLength;
	/*
	// THIS IS WORKING CODE THAT SUPPORTS SSL PROXIES!
#ifdef COMPILE_SSL_SUPPORT
	if(m_pSSL)
	{
		readLength = m_pSSL->read(buffer,256);
		if(readLength <= 0)
		{
			// ssl error....?
			switch(m_pSSL->getProtocolError(readLength))
			{
				case KviSSL::ZeroReturn:
					readLength = 0;
				break;
				case KviSSL::WantRead:
				case KviSSL::WantWrite:
					// hmmm...
					return;
				break;
				case KviSSL::SyscallError:
				{
					int iE = m_pSSL->getLastError(true);
					if(iE != 0)
					{
						raiseSSLError();
						raiseError(KviError::SSLError);
						reset();
						return;
					}
				}
				break;
				case KviSSL::SSLError:
					raiseSSLError();
					raiseError(KviError::SSLError);
					reset();
					return;
				break;
				default:
					raiseError(KviError::SSLError);
					reset();
					return;
				break;

			}
			handleInvalidSocketRead(readLength);
			return;
		}
	} else {
#endif
*/
	iReadLength = kvi_socket_recv(m_sock, cBuffer, 255);
	if(iReadLength <= 0)
	{
		handleInvalidSocketRead(iReadLength);
		return;
	}
	/*
#ifdef COMPILE_SSL_SUPPORT
	}
#endif
*/
	// we need at least two bytes...
	if(iReadLength < 2)
	{
		// a single byte of reply means:
		// - connection through a 1 bps modem
		// - a totally blocked network
		// - remote host is not a SOCKS/HTTP server
		// Anyway....it is always a meaningless reply
		// better to try again later :)
		raiseError(KviError::UnrecognizedProxyReply);
		reset();
		return;
	}

	// handle the reply
	switch(m_state)
	{
		case ProxyFinalV4:
			//V4 final reply
			proxyHandleV4FinalReply((unsigned char)cBuffer[1]);
			break;
		case ProxySelectAuthMethodV5:
			//V5 method selection reply
			proxyHandleV5MethodReply((unsigned char)cBuffer[1]);
			break;
		case ProxyUserPassV5:
			//V5 user and pass reply
			proxyHandleV5AuthReply((unsigned char)cBuffer[1]);
			break;
		case ProxyFinalV5:
			//V5 final reply
			proxyHandleV5FinalReply((unsigned char)cBuffer[1]);
			break;
		case ProxyFinalHttp:
			//Http final reply
			cBuffer[iReadLength] = '\0';
			proxyHandleHttpFinalReply(cBuffer, iReadLength);
			break;
		default:
			// what ?
			raiseError(KviError::UnrecognizedProxyReply);
			reset();
			break;
	}
}

void KviIrcSocket::proxyLoginHttp()
{
	// Well..this is just plain and easy: connect to the proxy machine
	// and say "CONNECT <irc.server>:<port> HTTP/<version>\n\n"
	// if it requires auth then say Proxy-Authorization: Basic user:passwd
	// Then expect a server reply header (2 newlines)
	// HTTP 200 = Success
	// HTTP Anything else = Failure

	if(_OUTPUT_VERBOSE)
		outputProxyMessage(__tr2qs("Using HTTP protocol."));

	setState(ProxyFinalHttp);
	KviCString szTmp(KviCString::Format, "CONNECT %s:%u HTTP/1.0\r\n", m_pIrcServer->hostName().toUtf8().data(), (unsigned int)(m_pIrcServer->port()));

	szTmp.append(KviCString::Format, "User-Agent: KVIrc-ProxyClient/4.0\r\n");

	if(m_pProxy->hasUser())
	{
		KviCString szAuth(KviCString::Format, "%s:%s", m_pProxy->user().toUtf8().data(), m_pProxy->pass().toUtf8().data());
		KviCString szEncoded;
		szEncoded.bufferToBase64(szAuth.ptr(), szAuth.len());
		szTmp.append(KviCString::Format, "Proxy-Authorization: Basic %s\r\n\r\n", szEncoded.ptr());
	}
	else
	{
		szTmp.append("\r\n");
	}

	sendRawData(szTmp.ptr(), szTmp.len());
}

void KviIrcSocket::proxyLoginV4()
{
	// SOCKSv4 protocol
	//
	// 1) CONNECT
	//
	// The client connects to the SOCKS server and sends a CONNECT request when
	// it wants to establish a connection to an application server. The client
	// includes in the request packet the IP address and the port number of the
	// destination host, and userid, in the following format.
	//
	//                +----+----+----+----+----+----+----+----+----+----+....+----+
	//                | VN | CD | DSTPORT |      DSTIP        | USERID       |NULL|
	//                +----+----+----+----+----+----+----+----+----+----+....+----+
	// # of bytes:       1    1      2              4           variable       1
	//
	// VN is the SOCKS protocol version number and should be 4. CD is the
	// SOCKS command code and should be 1 for CONNECT request. NULL is a byte
	// of all zero bits.
	//
	// The SOCKS server checks to see whether such a request should be granted
	// based on any combination of source IP address, destination IP address,
	// destination port number, the userid, and information it may obtain by
	// consulting IDENT, cf. RFC 1413.  If the request is granted, the SOCKS
	// server makes a connection to the specified port of the destination host.
	// A reply packet is sent to the client when this connection is established,
	// or when the request is rejected or the operation fails.
	//
	if(_OUTPUT_VERBOSE)
		outputProxyMessage(__tr2qs("Using SOCKSv4 protocol."));

	m_pProxy->normalizeUserAndPass();
	// the protocol does not specify the "userid" format...
	// so build a userid from the pass and/or username...

	KviCString szUserAndPass = m_pProxy->user();
	if(m_pProxy->hasPass())
	{
		if(szUserAndPass.hasData())
			szUserAndPass.append(' ');
		szUserAndPass.append(m_pProxy->pass());
	}

	int iLen = szUserAndPass.len() + 9;

	// build the request packet
	char * pcBufToSend = new char[iLen];
	pcBufToSend[0] = (unsigned char)4; //Version 4
	pcBufToSend[1] = (unsigned char)1; //Connect

	quint16 port = (quint16)htons(m_pIrcServer->port());
	KviMemory::move((void *)(pcBufToSend + 2), (void *)&port, 2);

	struct in_addr ircInAddr;

	if(!KviNetUtils::stringIpToBinaryIp(m_pIrcServer->ip(), &ircInAddr))
		qDebug("SOCKET INTERNAL ERROR IN IPV4 (SOCKS4) ADDR CONVERSION");

	quint32 host = (quint32)ircInAddr.s_addr;
	KviMemory::move((void *)(pcBufToSend + 4), (void *)&host, 4);
	KviMemory::move((void *)(pcBufToSend + 8), (void *)(szUserAndPass.ptr()), szUserAndPass.len());

	pcBufToSend[iLen - 1] = '\0';

	// send it into hyperspace...
	setState(ProxyFinalV4);
	sendRawData(pcBufToSend, iLen);
	delete[] pcBufToSend;
	// and wait for reply...
}

void KviIrcSocket::proxyLoginV5()
{
	// SOCKSv5 protocol.
	//
	// When a TCP-based client wishes to establish a connection to an object
	// that is reachable only via a firewall (such determination is left up
	// to the implementation), it must open a TCP connection to the
	// appropriate SOCKS port on the SOCKS server system.  The SOCKS service
	// is conventionally located on TCP port 1080.  If the connection
	// request succeeds, the client enters a negotiation for the
	// authentication method to be used, authenticates with the chosen
	// method, then sends a relay request.  The SOCKS server evaluates the
	// request, and either establishes the appropriate connection or denies
	// it.
	//
	// The client connects to the server, and sends a version
	// identifier/method selection message:
	//
	//                    +----+----------+----------+
	//                    |VER | NMETHODS | METHODS  |
	//                    +----+----------+----------+
	//                    | 1  |    1     | 1 to 255 |
	//                    +----+----------+----------+
	//
	// The VER field is set to X'05' for this version of the protocol.  The
	// NMETHODS field contains the number of method identifier octets that
	// appear in the METHODS field.
	// The values currently defined for METHOD are:
	//
	//      o  X'00' NO AUTHENTICATION REQUIRED
	//      o  X'01' GSSAPI
	//      o  X'02' USERNAME/PASSWORD
	//      o  X'03' CHAP
	//      o  X'04' to X'7F' IANA ASSIGNED
	//      o  X'80' to X'FE' RESERVED FOR PRIVATE METHODS
	//      o  X'FF' NO ACCEPTABLE METHODS
	//

	if(_OUTPUT_VERBOSE)
		outputProxyMessage(__tr2qs("Using SOCKSv5 protocol."));

	m_pProxy->normalizeUserAndPass();
	// the protocol does not specify the "userid" format...
	// so build a userid from the pass and/or username...

	char cBufToSend[4];
	cBufToSend[0] = (unsigned char)5; //use version 5
	int iSendLen = 3;

	if(!(m_pProxy->hasUser() || m_pProxy->hasPass()))
	{
		// no auth needed.
		cBufToSend[1] = (unsigned char)1; //select one method
		cBufToSend[2] = (unsigned char)0; //select method 0 : no auth
		if(_OUTPUT_VERBOSE)
			outputProxyMessage(__tr2qs("We can accept auth method 0 (no auth)"));
	}
	else
	{
		// we can provide a password and username if needed...
		cBufToSend[1] = (unsigned char)2; //select from two methods
		cBufToSend[2] = (unsigned char)0; //method 0 or
		cBufToSend[3] = (unsigned char)2; //method 2 username/pass auth
		iSendLen = 4;
		if(_OUTPUT_VERBOSE)
			outputProxyMessage(__tr2qs("We can accept auth method 0 (no auth) or 2 (user/pass)"));
	}

	// notify the user before sending...since we may get disconnected
	setState(ProxySelectAuthMethodV5);
	sendRawData(cBufToSend, iSendLen);
	// and wait for response
}

void KviIrcSocket::proxyAuthUserPassV5()
{
	//   Once the SOCKS V5 server has started, and the client has selected the
	//   Username/Password Authentication protocol, the Username/Password
	//   subnegotiation begins.  This begins with the client producing a
	//   Username/Password request:
	//
	//           +----+------+----------+------+----------+
	//           |VER | ULEN |  UNAME   | PLEN |  PASSWD  |
	//           +----+------+----------+------+----------+
	//           | 1  |  1   | 1 to 255 |  1   | 1 to 255 |
	//           +----+------+----------+------+----------+
	//
	//   The VER field contains the current version of the subnegotiation,
	//   which is X'01'. The ULEN field contains the length of the UNAME field
	//   that follows. The UNAME field contains the username as known to the
	//   source operating system. The PLEN field contains the length of the
	//   PASSWD field that follows. The PASSWD field contains the password
	//   association with the given UNAME.
	//
	unsigned int uPass = (unsigned int)m_pProxy->passLen();
	unsigned int uUser = (unsigned int)m_pProxy->userLen();

	if(uPass > 255)
		uPass = 255;

	if(uUser > 255)
		uUser = 255;

	int iLen = uPass + uUser + 3;
	char * pcBufToSend = new char[iLen];

	//version x'01'
	pcBufToSend[0] = (unsigned char)1;

	//length of the username
	pcBufToSend[1] = (unsigned char)uUser;

	//username
	KviMemory::move((void *)(pcBufToSend + 2), (void *)m_pProxy->user().toUtf8().data(), uUser);

	//length of the password
	pcBufToSend[2 + uUser] = (unsigned char)uPass;
	KviMemory::move((void *)(pcBufToSend + 3 + uUser), (void *)m_pProxy->pass().toUtf8().data(), uPass);

	// spit out the buffer and wait
	setState(ProxyUserPassV5);
	sendRawData(pcBufToSend, iLen);
	delete[] pcBufToSend;
	// and wait for response...
}

void KviIrcSocket::proxySendTargetDataV5()
{
	//   Once the method-dependent subnegotiation has completed, the client
	//   sends the request details.  If the negotiated method includes
	//   encapsulation for purposes of integrity checking and/or
	//   confidentiality, these requests MUST be encapsulated in the method-
	//   dependent encapsulation.
	//
	//   The SOCKS request is formed as follows:
	//
	//           +----+-----+------+------+----------+----------+
	//           |VER | CMD | FLAG | ATYP | DST.ADDR | DST.PORT |
	//           +----+-----+------+------+----------+----------+
	//           | 1  |  1  |  1   |  1   | Variable |    2     |
	//           +----+-----+------+------+----------+----------+
	//
	//   Where:
	//
	//      o VER    protocol version: X'05'
	//      o CMD
	//         o CONNECT X'01'
	//         o BIND X'02'
	//         o UDP ASSOCIATE X'03'
	//         o  X'04' to X'7F' IANA ASSIGNED
	//         o  X'80' to X'FF' RESERVED FOR PRIVATE METHODS
	//      o FLAG   command dependent flag (defaults to X'00')
	//      o ATYP   address type of following address
	//        o IP V4 address: X'01'
	//        o DOMAINNAME: X'03'
	//        o IP V6 address: X'04'
	//      o DST.ADDR       desired destination address
	//      o DST.PORT desired destination port in network octet
	//         order
	//
	//      The SOCKS server will typically evaluate the request based on
	//      source and destination addresses, and return one or more reply
	//      messages, as appropriate for the request type.
	//
	//   In an address field (DST.ADDR, BND.ADDR), the ATYP field specifies
	//   the type of address contained within the field:
	//
	//             o  X'01'
	//
	//   The address is a version-4 IP address, with a length of 4 octets.
	//
	//             o  X'03'
	//
	//   The address field contains a fully-qualified domain name.  The first
	//   octet of the address field contains the number of octets of name that
	//   follow, there is no terminating NUL octet.
	//
	//             o  X'04'
	//
	//   The address is a version-6 IP address, with a length of 16 octets.
	bool bRemoteDns = !(
	    (
	        KviNetUtils::isValidStringIp(m_pIrcServer->ip())
#ifdef COMPILE_IPV6_SUPPORT
	        || KviNetUtils::isValidStringIPv6(m_pIrcServer->ip())
#endif
	            )

	    && m_pIrcServer->cacheIp());
	int iBufLen = bRemoteDns ? 4 + 1 + m_pIrcServer->hostName().toUtf8().length() + 2
	                         : (m_pIrcServer->isIPv6() ? 22 : 10);
	char * pcBufToSend = (char *)KviMemory::allocate(sizeof(char) * iBufLen);
	pcBufToSend[0] = (unsigned char)5; //Proto 5
	pcBufToSend[1] = (unsigned char)1; //CONNECT
	pcBufToSend[2] = (unsigned char)0; //RSV

	if(bRemoteDns)
	{
		bRemoteDns = true;
		pcBufToSend[3] = 3;
		pcBufToSend[4] = m_pIrcServer->hostName().toUtf8().length();
	}
	else
	{
		pcBufToSend[3] = (unsigned char)m_pIrcServer->isIPv6() ? 4 : 1; // IPv6 : IPv4
	}

	if(bRemoteDns)
	{
		KviMemory::move((void *)(pcBufToSend + 5),
		    (void *)(m_pIrcServer->hostName().toUtf8().data()),
		    m_pIrcServer->hostName().toUtf8().length());
		quint16 port = (quint16)htons(m_pIrcServer->port());
		KviMemory::move((void *)(pcBufToSend + 4 + 1 + m_pIrcServer->hostName().toUtf8().length()), (void *)&port, 2);
	}
	else if(m_pIrcServer->isIPv6())
	{
#ifdef COMPILE_IPV6_SUPPORT
		struct in6_addr ircInAddr;

		if(!KviNetUtils::stringIpToBinaryIp_V6(m_pIrcServer->ip(), &ircInAddr))
			qDebug("SOCKET INTERNAL ERROR IN IPV6 ADDR CONVERSION");
		KviMemory::move((void *)(pcBufToSend + 4), (void *)(&ircInAddr), 4);
		quint16 port = (quint16)htons(m_pIrcServer->port());
		KviMemory::move((void *)(pcBufToSend + 20), (void *)&port, 2);
#endif
	}
	else
	{
		struct in_addr ircInAddr;

		if(!KviNetUtils::stringIpToBinaryIp(m_pIrcServer->ip(), &ircInAddr))
			qDebug("SOCKET INTERNAL ERROR IN IPV4 ADDR CONVERSION");
		quint32 host = (quint32)ircInAddr.s_addr;
		KviMemory::move((void *)(pcBufToSend + 4), (void *)&host, 4);
		quint16 port = (quint16)htons(m_pIrcServer->port());
		KviMemory::move((void *)(pcBufToSend + 8), (void *)&port, 2);
	}

	// send it into hyperspace...
	setState(ProxyFinalV5);
	sendRawData(pcBufToSend, iBufLen);
	KviMemory::free(pcBufToSend);
	// and wait for reply...
}

void KviIrcSocket::proxyHandleV5AuthReply(unsigned char cReply)
{
	//   The server verifies the supplied UNAME and PASSWD, and sends the
	//   following response:
	//
	//                        +----+--------+
	//                        |VER | STATUS |
	//                        +----+--------+
	//                        | 1  |   1    |
	//                        +----+--------+
	//
	//   A STATUS field of X'00' indicates success. If the server returns a
	//   `failure' (STATUS value other than X'00') status, it MUST close the
	//   connection.
	//
	if(cReply == 0)
	{
		if(_OUTPUT_VERBOSE)
			outputProxyMessage(__tr2qs("Proxy response: auth OK: access granted"));
		proxySendTargetDataV5();
		return;
	}
	raiseError(KviError::ProxyAuthFailed);
	reset();
}

void KviIrcSocket::proxyHandleV5MethodReply(unsigned char cReply)
{
	//   The server selects from one of the methods given in METHODS, and
	//   sends a METHOD selection message:
	//
	//                            +----+--------+
	//                            |VER | METHOD |
	//                            +----+--------+
	//                            | 1  |   1    |
	//                            +----+--------+
	//
	//   If the selected METHOD is X'FF', none of the methods listed by the
	//   client are acceptable, and the client MUST close the connection.
	//
	// The values currently defined for METHOD are:
	//
	//      o  X'00' NO AUTHENTICATION REQUIRED
	//      o  X'01' GSSAPI
	//      o  X'02' USERNAME/PASSWORD
	//      o  X'03' CHAP
	//      o  X'04' to X'7F' IANA ASSIGNED
	//      o  X'80' to X'FE' RESERVED FOR PRIVATE METHODS
	//      o  X'FF' NO ACCEPTABLE METHODS
	//
	if(cReply == 0)
	{
		if(_OUTPUT_VERBOSE)
			outputProxyMessage(__tr2qs("Proxy response: auth method OK: using method 0 (no auth)"));
		proxySendTargetDataV5();
		return;
	}

	if(cReply == 2)
	{
		if(_OUTPUT_VERBOSE)
			outputProxyMessage(__tr2qs("Proxy response: auth method OK: using method 2 (user/pass)"));
		proxyAuthUserPassV5();
		return;
	}

	//Request rejected
	if(cReply == 0xFF)
	{
		raiseError(KviError::ProxyNoAcceptableAuthMethod);
		reset();
	}
	else
	{
		// unrecognized...
		raiseError(KviError::UnrecognizedProxyReply);
		reset();
	}
}

void KviIrcSocket::proxyHandleV5FinalReply(unsigned char cReply)
{
	//
	//   The SOCKS request information is sent by the client as soon as it has
	//   established a connection to the SOCKS server, and completed the
	//   authentication negotiations.  The server evaluates the request, and
	//   returns a reply formed as follows:
	//
	//           +----+-----+------+------+----------+----------+
	//           |VER | REP | FLAG | ATYP | BND.ADDR | BND.PORT |
	//           +----+-----+------+------+----------+----------+
	//           | 1  |  1  |  1   |  1   | Variable |    2     |
	//           +----+-----+------+------+----------+----------+
	//
	//   Where:
	//             o  VER    protocol version: X'05'
	//             o  REP    Reply field:
	//                o  X'00' succeeded
	//                o  X'01' general SOCKS server failure
	//                o  X'02' connection not allowed by ruleset
	//                o  X'03' Network unreachable
	//                o  X'04' Host unreachable
	//                o  X'05' Connection refused
	//                o  X'06' TTL expired
	//                o  X'07' Command not supported
	//                o  X'08' Address type not supported
	//                o  X'09' Invalid address
	//                o  X'0A' to X'FF' unassigned
	//             o  FLAG   command dependent flag
	//             o  ATYP   address type of following address
	//                o  IP V4 address: X'01'
	//                o  DOMAINNAME: X'03'
	//                o  IP V6 address: X'04'
	//             o  BND.ADDR       server bound address
	//             o  BND.PORT       server bound port in network octet order
	//
	if(cReply == 0)
	{
		// Request granted
		if(_OUTPUT_VERBOSE)
			outputProxyMessage(__tr2qs("Proxy response: target data OK: request granted"));
		connectedToIrcServer();
	}
	else
	{
		//Request rejected
		KviError::Code eError;
		switch(cReply)
		{
			case 1:
				eError = KviError::ProxyReply01GeneralSOCKSFailure;
				break;
			case 2:
				eError = KviError::ProxyReply02ConnectionNotAllowed;
				break;
			case 3:
				eError = KviError::ProxyReply03NetworkUnreachable;
				break;
			case 4:
				eError = KviError::ProxyReply04HostUnreachable;
				break;
			case 5:
				eError = KviError::ProxyReply05ConnectionRefused;
				break;
			case 6:
				eError = KviError::ProxyReply06TTLExpired;
				break;
			case 7:
				eError = KviError::ProxyReply07CommandNotSupported;
				break;
			case 8:
				eError = KviError::ProxyReply08AddressTypeNotSupported;
				break;
			case 9:
				eError = KviError::ProxyReply09InvalidAddress;
				break;
			default:
				eError = KviError::UnrecognizedProxyReply;
				break;
		}
		raiseError(eError);
		reset();
	}
}

void KviIrcSocket::proxyHandleV4FinalReply(unsigned char cReply)
{
	// If the request is granted, the SOCKS
	// server makes a connection to the specified port of the destination host.
	// A reply packet is sent to the client when this connection is established,
	// or when the request is rejected or the operation fails.
	//
	//
	//                +----+----+----+----+----+----+----+----+
	//                | VN | CD | DSTPORT |      DSTIP        |
	//                +----+----+----+----+----+----+----+----+
	// # of bytes:       1    1      2              4
	//
	// VN is the version of the reply code and should be 0. CD is the result
	// code with one of the following values:
	//
	//        90: request granted
	//        91: request rejected or failed
	//        92: request rejected because SOCKS server cannot connect to
	//            identd on the client
	//        93: request rejected because the client program and identd
	//            report different user-ids
	//
	// The remaining fields are ignored.
	//
	// The SOCKS server closes its connection immediately after notifying
	// the client of a failed or rejected request. For a successful request,
	// the SOCKS server gets ready to relay traffic on both directions. This
	// enables the client to do I/O on its connection as if it were directly
	// connected to the application server.
	if(cReply == 90)
	{
		// Request granted
		if(_OUTPUT_VERBOSE)
			outputProxyMessage(__tr2qs("Proxy response: target data OK: request granted"));
		connectedToIrcServer();
	}
	else
	{
		//Request rejected
		KviError::Code eError;
		switch(cReply)
		{
			case 91:
				eError = KviError::ProxyReply91RequestFailed;
				break;
			case 92:
				eError = KviError::ProxyReply92IdentFailed;
				break;
			case 93:
				eError = KviError::ProxyReply93IdentNotMatching;
				break;
			default:
				eError = KviError::UnrecognizedProxyReply;
				break;
		}
		raiseError(eError);
		reset();
	}
	// Just looked out of the window...
	// Hmmmm...strange light outside...
	// Looked at the clock...6:34 !
	// I think I'll go sleep.... :)
}

void KviIrcSocket::proxyHandleHttpFinalReply(const char * pcBuffer, int)
{
	// Escape character is '^]'.
	// CONNECT warszawa.irc.pl:6667 HTTP/1.0
	//
	// HTTP/1.0 200 Connection established

	KviCString szTmp = pcBuffer;
	// FIXME: #warning "We could even show the proxy output here...!"
	szTmp.cutFromFirst('\n');
	szTmp.trim();

	if(kvi_strEqualCIN(szTmp.ptr(), "HTTP", 4))
	{
		int iIdx = szTmp.findFirstIdx(" 200 ");
		if(iIdx != -1)
		{
			if(iIdx == szTmp.findFirstIdx(' '))
			{
				QString szMsg = __tr2qs("Proxy response: ");
				szMsg += szTmp.ptr();
				if(_OUTPUT_VERBOSE)
					outputProxyMessage(szMsg);
				connectedToIrcServer();
				return;
			}
		}
	}

	outputProxyError(__tr2qs("Proxy said something about: \n"));
	outputProxyMessage(m_pConsole->decodeText(pcBuffer));

	//Read HTTP error page and show it

	if(m_pWsn)
	{
		delete m_pWsn;
		m_pWsn = nullptr;
	}

	if(m_pRsn)
	{
		delete m_pRsn;
		m_pRsn = nullptr;
	}

	m_pRsn = new QSocketNotifier((int)m_sock, QSocketNotifier::Read);
	QObject::connect(m_pRsn, SIGNAL(activated(int)), this, SLOT(readHttpProxyErrorData(int)));
	m_pRsn->setEnabled(true);

	setState(ProxyHttpError);

}

//
// SSL HANDSHAKE
//

#ifdef COMPILE_SSL_SUPPORT
void KviIrcSocket::printSSLPeerCertificate()
{
	KviSSLCertificate * c = m_pSSL->getPeerCertificate();
	if(c)
	{
		//m_pConsole->socketEvent(SSLCertificate,(void *)c);
		if(_OUTPUT_VERBOSE)
			KviSSLMaster::printSSLCertificate(m_pConsole, __tr("Server X509 certificate"), c);
		delete c;
	}
	else
	{
		if(_OUTPUT_VERBOSE)
			outputSSLMessage(__tr2qs("The server didn't provide a certificate"));
	}
}

void KviIrcSocket::printSSLCipherInfo()
{
	KviSSLCipherInfo * ci = m_pSSL->getCurrentCipherInfo();
	if(ci)
	{
		KviSSLMaster::printSSLCipherInfo(m_pConsole, __tr("Current transmission cipher"), ci);
		delete ci;
	}
	else
	{
		if(_OUTPUT_VERBOSE)
			outputSSLMessage(__tr2qs("Unable to determine the current cipher"));
	}
}

void KviIrcSocket::raiseSSLError()
{
	KviCString szBuffer;
	while(m_pSSL->getLastErrorString(szBuffer))
	{
		outputSSLError(szBuffer.ptr());
	}
}
#endif // COMPILE_SSL_SUPPORT

void KviIrcSocket::doSSLHandshake(int)
{
#ifdef COMPILE_SSL_SUPPORT
	KVI_ASSERT(m_pSSL);

	if(m_pRsn)
	{
		delete m_pRsn;
		m_pRsn = nullptr;
	}

	if(m_pWsn)
	{
		delete m_pWsn;
		m_pWsn = nullptr;
	}

	if(!m_pSSL)
	{
		qDebug("Oops! Have I lost the SSL class?");
		reset();
		return; // ops ?
	}

	switch(m_pSSL->connect())
	{
		case KviSSL::Success:
			// done!
			printSSLCipherInfo();
			printSSLPeerCertificate();
			linkUp();
			break;
		case KviSSL::WantRead:
			m_pRsn = new QSocketNotifier((int)m_sock, QSocketNotifier::Read);
			QObject::connect(m_pRsn, SIGNAL(activated(int)), this, SLOT(doSSLHandshake(int)));
			m_pRsn->setEnabled(true);
			break;
		case KviSSL::WantWrite:
			m_pWsn = new QSocketNotifier((int)m_sock, QSocketNotifier::Write);
			QObject::connect(m_pWsn, SIGNAL(activated(int)), this, SLOT(doSSLHandshake(int)));
			m_pWsn->setEnabled(true);
			break;
		case KviSSL::RemoteEndClosedConnection:
			raiseError(KviError::RemoteEndClosedConnection);
			reset();
			break;
		case KviSSL::SSLError:
			raiseSSLError();
			raiseError(KviError::SSLError);
			reset();
			break;
		case KviSSL::SyscallError:
		{
			// syscall problem
			int iErr = kvi_socket_error();
			if(!kvi_socket_recoverableError(iErr))
			{
				// Declare problems :)
				raiseError((iErr ? KviError::translateSystemError(iErr) : KviError::UnknownError));
			}
			else
			{
				// can recover ? (EAGAIN, EINTR ?)
				m_pWsn = new QSocketNotifier((int)m_sock, QSocketNotifier::Write);
				QObject::connect(m_pWsn, SIGNAL(activated(int)), this, SLOT(doSSLHandshake(int)));
				m_pWsn->setEnabled(true);
				return;
			}
			reset();
		}
		break;
		default:
			raiseError(KviError::SSLError);
			reset();
			break;
	}

#else  //COMPILE_SSL_SUPPORT
	qDebug("Oops! SSL handshake without SSL support. Aborting!");
	exit(-1);
#endif //COMPILE_SSL_SUPPORT
}

//
// LINK UP
//

void KviIrcSocket::linkUp()
{
	setState(Connected);

	// the last check
	if(m_sock == KVI_INVALID_SOCKET)
		return; // ops...disconnected in setState() ????

	// OK.. it seems that we're really up and running now!
	if(m_pWsn)
	{
		delete m_pWsn;
		m_pWsn = nullptr;
	}

	if(m_pRsn)
	{
		delete m_pRsn;
		m_pRsn = nullptr;
	}

	m_pRsn = new QSocketNotifier((int)m_sock, QSocketNotifier::Read);
	QObject::connect(m_pRsn, SIGNAL(activated(int)), this, SLOT(readData(int)));
	m_pRsn->setEnabled(true);
}

void KviIrcSocket::readData(int)
{
	//read data
	char cBuffer[1025];
	int iReadLength;
#ifdef COMPILE_SSL_SUPPORT
	if(m_pSSL)
	{
		iReadLength = m_pSSL->read(cBuffer, 1024);
		if(iReadLength <= 0)
		{
			// ssl error....?
			switch(m_pSSL->getProtocolError(iReadLength))
			{
				case KviSSL::ZeroReturn:
					iReadLength = 0;
					break;
				case KviSSL::WantRead:
				case KviSSL::WantWrite:
					// hmmm...
					return;
					break;
				case KviSSL::SyscallError:
				{
					int iE = m_pSSL->getLastError(true);
					if(iE != 0)
					{
						raiseSSLError();
						raiseError(KviError::SSLError);
						reset();
						return;
					}
				}
				break;
				case KviSSL::SSLError:
					raiseSSLError();
					raiseError(KviError::SSLError);
					reset();
					return;
					break;
				default:
					raiseError(KviError::SSLError);
					reset();
					return;
					break;
			}
			handleInvalidSocketRead(iReadLength);
			return;
		}
	}
	else
	{
#endif
		iReadLength = kvi_socket_recv(m_sock, cBuffer, 1024);
		if(iReadLength <= 0)
		{
			handleInvalidSocketRead(iReadLength);
			return;
		}
#ifdef COMPILE_SSL_SUPPORT
	}
#endif

	//terminate our buffer
	(*(cBuffer + iReadLength)) = '\0';

	m_uReadBytes += iReadLength;

	// Shut up the socket notifier
	// in case that we enter in a local loop somewhere
	// while processing data...
	m_pRsn->setEnabled(false);
	// shut also the flushing of the message queue
	// in this way we prevent disconnect detection
	// during the processing of a message effectively
	// making it always an asynchronous event.
	m_bInProcessData = true;

	m_pLink->processData(cBuffer, iReadLength);
	// after this line there should be nothing that relies
	// on the "connected" state of this socket.
	// It may happen that it has been reset() in the middle of the processData() call
	// and (unverified) it might have been even deleted.

	// re-enable the socket notifier... (if it's still there)
	if(m_pRsn)
		m_pRsn->setEnabled(true);
	// and the message queue flushing
	m_bInProcessData = false;
	// and flush the queue too!
	if(m_pSendQueueHead)
		flushSendQueue();
}

void KviIrcSocket::abort()
{
	// flush the send queue if possible (and if not yet disconnected in fact)
	if(m_state == Connected)
		flushSendQueue();
	if(m_state != Idle)
		raiseError(KviError::OperationAborted);
	// and reset
	reset();
}

void KviIrcSocket::handleInvalidSocketRead(int iReadLength)
{
	KVI_ASSERT(iReadLength <= 0);
	if(iReadLength == 0)
	{
		raiseError(KviError::RemoteEndClosedConnection);
		reset();
	}
	else
	{
		//check for transmission errors
		int iErr = kvi_socket_error();
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		if((iErr != EAGAIN) && (iErr != EINTR) && (iErr != WSAEWOULDBLOCK))
#else
		if((iErr != EAGAIN) && (iErr != EINTR))
#endif
		{
			if(iErr > 0)
				raiseError((KviError::translateSystemError(iErr)));
			else
				raiseError(KviError::RemoteEndClosedConnection);
			reset();
		} //else transient error...wait again...
	}
}

void KviIrcSocket::queue_insertMessage(KviIrcSocketMsgEntry * pMsg)
{
	KVI_ASSERT(pMsg);

	pMsg->next_ptr = nullptr;

	if(m_pSendQueueHead)
	{
		m_pSendQueueTail->next_ptr = pMsg;
		m_pSendQueueTail = pMsg;
	}
	else
	{
		m_pSendQueueHead = pMsg;
		m_pSendQueueTail = pMsg;
	}
}

void KviIrcSocket::free_msgEntry(KviIrcSocketMsgEntry * e)
{
	if(e->pData)
		delete e->pData;

	e->pData = nullptr;
	KviMemory::free(e);
}

bool KviIrcSocket::queue_removeMessage()
{
	KVI_ASSERT(m_pSendQueueTail);
	KVI_ASSERT(m_pSendQueueHead);

	if(m_pSendQueueHead->pData)
		delete m_pSendQueueHead->pData;

	KviIrcSocketMsgEntry * pEntry = m_pSendQueueHead;
	m_pSendQueueHead = pEntry->next_ptr;
	KviMemory::free((void *)pEntry);

	if(m_pSendQueueHead == nullptr)
	{
		m_pSendQueueTail = nullptr;
		return false;
	}
	else
		return true;
}

void KviIrcSocket::queue_removeAllMessages()
{
	if(m_pSendQueueHead)
		while(queue_removeMessage())
		{
		}
}

void KviIrcSocket::queue_removePrivateMessages()
{
	KviIrcSocketMsgEntry * pPrevEntry = nullptr;
	KviIrcSocketMsgEntry * pEntry = m_pSendQueueHead;
	while(pEntry)
	{
		if(pEntry->pData->size() > 7)
		{
			if(kvi_strEqualCIN((char *)(pEntry->pData->data()), "PRIVMSG", 7))
			{
				// remove it
				if(pPrevEntry)
				{
					pPrevEntry->next_ptr = pEntry->next_ptr;
					if(!pPrevEntry->next_ptr)
						m_pSendQueueTail = nullptr;
					free_msgEntry(pEntry);
					pEntry = pPrevEntry->next_ptr;
				}
				else
				{
					m_pSendQueueHead = pEntry->next_ptr;
					if(!m_pSendQueueHead)
						m_pSendQueueTail = nullptr;
					free_msgEntry(pEntry);
					pEntry = m_pSendQueueHead;
				}
				continue;
			}
		}
		pPrevEntry = pEntry;
		pEntry = pEntry->next_ptr;
	}
}

void KviIrcSocket::flushSendQueue()
{
	// If we're called from the flush timer, stop it
	if(m_pFlushTimer->isActive())
		m_pFlushTimer->stop();

	// OK...have something to send...
	KVI_ASSERT(m_state != Idle);

	struct timeval curTime;

	while(m_pSendQueueHead)
	{
		if(KVI_OPTION_BOOL(KviOption_boolLimitOutgoingTraffic))
		{
			kvi_gettimeofday(&curTime);

			int iTimeDiff = curTime.tv_usec - m_tAntiFloodLastMessageTime.tv_usec;
			iTimeDiff += (curTime.tv_sec - m_tAntiFloodLastMessageTime.tv_sec) * 1000000;

			if(((unsigned int)iTimeDiff) < KVI_OPTION_UINT(KviOption_uintOutgoingTrafficLimitUSeconds))
			{
				// need to wait for a while....
				m_pFlushTimer->start(((KVI_OPTION_UINT(KviOption_uintOutgoingTrafficLimitUSeconds) - iTimeDiff) / 1000) + 1);
				return;
			} // else can send
		}
		// Write one data buffer...
		int iResult;
#ifdef COMPILE_SSL_SUPPORT
		if(m_pSSL)
		{
			iResult = m_pSSL->write((char *)(m_pSendQueueHead->pData->data()), m_pSendQueueHead->pData->size());
		}
		else
		{
#endif
			iResult = kvi_socket_send(m_sock, (char *)(m_pSendQueueHead->pData->data()), m_pSendQueueHead->pData->size());
#ifdef COMPILE_SSL_SUPPORT
		}
#endif
		if(iResult == (int)m_pSendQueueHead->pData->size())
		{
			// Successful send...remove this data buffer
			m_uSentPackets++;
			m_uSentBytes += iResult;
			//if(m_pConsole->hasMonitors())outgoingMessageNotifyMonitors((char *)(m_pSendQueueHead->pData->data()),result);
			queue_removeMessage();
			if(KVI_OPTION_BOOL(KviOption_boolLimitOutgoingTraffic))
			{
				m_tAntiFloodLastMessageTime.tv_sec = curTime.tv_sec;
				m_tAntiFloodLastMessageTime.tv_usec = curTime.tv_usec;
			}
			// And try next buffer...
			continue;
		}
		else
		{
// Something wrong ?
#ifdef COMPILE_SSL_SUPPORT
			if(iResult <= 0)
			{
				if(m_pSSL)
				{
					// ops...might be an SSL error
					switch(m_pSSL->getProtocolError(iResult))
					{
						case KviSSL::WantWrite:
						case KviSSL::WantRead:
							// Async continue...
							m_pFlushTimer->start(KVI_OPTION_UINT(KviOption_uintSocketQueueFlushTimeout));
							return;
							break;
						case KviSSL::SyscallError:
							if(iResult == 0)
							{
								raiseSSLError();
								raiseError(KviError::RemoteEndClosedConnection);
								reset();
								return;
							}
							else
							{
								int iSSLErr = m_pSSL->getLastError(true);
								if(iSSLErr != 0)
								{
									raiseSSLError();
									raiseError(KviError::SSLError);
									reset();
									return;
								}
								else
								{
									goto handle_system_error;
								}
							}
							break;
						case KviSSL::SSLError:
							raiseSSLError();
							raiseError(KviError::SSLError);
							reset();
							return;
							break;
						default:
							raiseError(KviError::SSLError);
							reset();
							return;
							break;
					}
				}
			}
			else
			{
#else  // COMPILE_SSL_SUPPORT
			if(iResult >= 0)
			{
				if(iResult > 0)
				{
#endif // COMPILE_SSL_SUPPORT

				// Partial send...need to finish it later
				m_pSendQueueHead->pData->remove(iResult);

				m_uSentBytes += iResult;
				if(_OUTPUT_VERBOSE)
					outputSocketWarning(__tr2qs("Partial socket write: packet broken into smaller pieces."));
#ifndef COMPILE_SSL_SUPPORT
			}
#endif // COMPILE_SSL_SUPPORT
			// Async continue...
			m_pFlushTimer->start(KVI_OPTION_UINT(KviOption_uintSocketQueueFlushTimeout));
			return;
		}

	handle_system_error:
		// Oops...error ?
		int iErr = kvi_socket_error();
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		if((iErr == EAGAIN) || (iErr == EINTR) || (iErr == WSAEWOULDBLOCK))
#else
				if((iErr == EAGAIN) || (iErr == EINTR))
#endif
		{
			// Transient error...partial send as before...
			if(_OUTPUT_VERBOSE)
				outputSocketWarning(__tr2qs("Partial socket write: packet broken into smaller pieces."));
			// Async continue...
			m_pFlushTimer->start(KVI_OPTION_UINT(KviOption_uintSocketQueueFlushTimeout));
			return;
		}
		else
		{
			// Disconnected... :(
			raiseError((KviError::translateSystemError(iErr)));
			reset();
			return;
		}
	}
}
//flushed completely ...
}

bool KviIrcSocket::getLocalHostIp(QString & szIp, bool bIPv6)
{
	if(m_state != Connected)
		return false;

	if(bIPv6)
	{
#ifdef COMPILE_IPV6_SUPPORT
		struct sockaddr_in6 name;
		int iLen = sizeof(name);
		if(!kvi_socket_getsockname(m_sock, (struct sockaddr *)&name, &iLen))
			return false;
		//I assume that getsockname returns data in Network byte order...
		//The man page misses to specify that...
		if(!KviNetUtils::binaryIpToStringIp_V6(name.sin6_addr, szIp))
			return false;

		return true;
#else
			return false; // no support
#endif
	}

	struct sockaddr_in name;
	int iLen = sizeof(name);
	if(!kvi_socket_getsockname(m_sock, (struct sockaddr *)&name, &iLen))
		return false;
	//I assume that getsockname returns data in Network byte order...
	//The man page misses to specify that...
	if(!KviNetUtils::binaryIpToStringIp(name.sin_addr, szIp))
		return false;

	return true;
}

/*
bool KviIrcSocket::sendFmtData(const char *fmt,...)
{
	if(m_state != Connected)return false;
	//new buffer
	KviIrcSocketMsgEntry *ptr = (KviIrcSocketMsgEntry *)KviMemory::allocate(sizeof(KviIrcSocketMsgEntry));
	ptr->pData = new KviDataBuffer(512);
	kvi_va_list(list);
	kvi_va_start(list,fmt);
	bool bTruncated;
	//sprintf the buffer up to 512 chars (adds a CRLF too)
	int iLen = kvi_irc_vsnprintf((char *)(ptr->pData->data()),fmt,list,&bTruncated);
	kvi_va_end(list);
	//adjust the buffer size
	if(iLen < 512)ptr->pData->resize(iLen);
	if(bTruncated)
	{
		if(_OUTPUT_VERBOSE)
			outputSocketWarning(__tr2qs("Socket message truncated to 512 bytes."));
	}

	queue_insertMessage(ptr);
	if(!m_bInProcessData)flushSendQueue();
	return (m_state != Idle);
}
*/
/*
bool KviIrcSocket::sendData(const char *buffer,int buflen)
{
	if(m_state != Connected)return false;
	//new buffer
	KviIrcSocketMsgEntry *ptr = (KviIrcSocketMsgEntry *)KviMemory::allocate(sizeof(KviIrcSocketMsgEntry));
	if(buflen < 0)buflen = strlen(buffer);
	if(buflen > 510)
	{
		buflen = 510;
		if(_OUTPUT_VERBOSE)
			outputSocketWarning(__tr2qs("Socket message truncated to 512 bytes."));
	}
	ptr->pData = new KviDataBuffer(buflen + 2);
	KviMemory::move(ptr->pData->data(),buffer,buflen);
	*(ptr->pData->data()+buflen)='\r';
	*(ptr->pData->data()+buflen+1)='\n';

	queue_insertMessage(ptr);
	if(!m_bInProcessData)flushSendQueue();
	return (m_state != Idle);
}
*/

bool KviIrcSocket::sendRawData(const char * pcBuffer, int iBuflen)
{
	if((m_state == Idle) || (m_state == Connecting))
		return false;

	//new buffer
	KviIrcSocketMsgEntry * pEntry = (KviIrcSocketMsgEntry *)KviMemory::allocate(sizeof(KviIrcSocketMsgEntry));
	pEntry->pData = new KviDataBuffer(iBuflen);

	KviMemory::move(pEntry->pData->data(), pcBuffer, iBuflen);
	queue_insertMessage(pEntry);

	if(!m_bInProcessData)
		flushSendQueue();

	return (m_state != Idle);
}

bool KviIrcSocket::sendPacket(KviDataBuffer * pData)
{
	if(m_state != Connected)
	{
		delete pData;
		pData = nullptr;
		return false;
	}

	KviIrcSocketMsgEntry * pEntry = (KviIrcSocketMsgEntry *)KviMemory::allocate(sizeof(KviIrcSocketMsgEntry));
	pEntry->pData = pData;
	queue_insertMessage(pEntry);

	if(!m_bInProcessData)
		flushSendQueue();

	return (m_state != Idle);
}
