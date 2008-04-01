/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//   File : kvi_ircsocket.cpp
//   Creation date : Tue Jul 30 19:25:18 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define __KVIRC__

#include "kvi_ircsocket.h"
#include "kvi_ircserver.h"
#include "kvi_proxydb.h"
#include "kvi_netutils.h"
#include "kvi_settings.h"
#include "kvi_error.h"
#include "kvi_locale.h"
#include "kvi_malloc.h"
#include "kvi_debug.h"
#include "kvi_string.h"
#include "kvi_options.h"
#include "kvi_memmove.h"
#include "kvi_socket.h"
#include "kvi_console.h"
#include "kvi_out.h"
#include "kvi_irclink.h"
#include "kvi_ircconnection.h"
#include "kvi_databuffer.h"

#ifdef COMPILE_SSL_SUPPORT
	#include "kvi_sslmaster.h"
#endif

#include <QTimer>
#include <QSocketNotifier>

#ifndef COMPILE_ON_WINDOWS
	#include <unistd.h> //for gettimeofday()
#endif
//#include <fcntl.h>
//#include <errno.h>

// FIXME: #warning "Lag-o-meter"

unsigned int g_uNextIrcLinkId = 1;



KviIrcSocket::KviIrcSocket(KviIrcLink * pLink)
: QObject()
{
	m_uId = g_uNextIrcLinkId;
	g_uNextIrcLinkId++;

	m_pLink = pLink;
	m_pConsole    = m_pLink->console();

	m_state          = Idle;        // current socket state

	m_pRsn           = 0;            // read socket notifier
	m_pWsn           = 0;            // write socket notifier
	m_sock           = KVI_INVALID_SOCKET;  // socket

	m_pIrcServer     = 0;            // current server data
	m_pProxy         = 0;            // current proxy data

	m_pTimeoutTimer  = 0;            // timeout for connect()

	m_uReadBytes     = 0;            // total read bytes per session
	m_uSentBytes     = 0;            // total sent bytes per session
	m_uSentPackets   = 0;            // total packets sent per session

	m_pSendQueueHead = 0;            // data queue
	m_pSendQueueTail = 0;            //

	m_iLastError     = KviError_success;

#ifdef COMPILE_SSL_SUPPORT
	m_pSSL           = 0;
#endif

	m_tAntiFloodLastMessageTime.tv_sec = 0;
	m_tAntiFloodLastMessageTime.tv_usec = 0;

	if(KVI_OPTION_UINT(KviOption_uintSocketQueueFlushTimeout) < 100)
		KVI_OPTION_UINT(KviOption_uintSocketQueueFlushTimeout) = 100; // this is our minimum , we don't want to lag the app

	m_bInProcessData = false;

	m_pFlushTimer    = new QTimer(); // queue flush timer
	connect(m_pFlushTimer,SIGNAL(timeout()),this,SLOT(flushSendQueue()));
}

KviIrcSocket::~KviIrcSocket()
{
	reset();
	delete m_pFlushTimer;
}


void KviIrcSocket::reset()
{
#ifdef COMPILE_SSL_SUPPORT
	if(m_pSSL)
	{
		KviSSLMaster::freeSSL(m_pSSL);
		m_pSSL = 0;
	}
#endif
	if(m_pIrcServer)
	{
		delete m_pIrcServer;
		m_pIrcServer = 0;
	}
	if(m_pProxy)
	{
		delete m_pProxy;
		m_pProxy = 0;
	}
	if(m_pRsn)
	{
		delete m_pRsn;
		m_pRsn = 0;
	}
	if(m_pWsn)
	{
		delete m_pWsn;
		m_pWsn = 0;
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
		m_pTimeoutTimer = 0;
	}

	m_bInProcessData             = false;
	
	m_uReadBytes                 = 0;
	m_uSentBytes                 = 0;
	m_uSentPackets               = 0;
	m_tAntiFloodLastMessageTime.tv_sec = 0;
	m_tAntiFloodLastMessageTime.tv_usec = 0;

	m_bInProcessData = false;

	if(m_pFlushTimer->isActive())m_pFlushTimer->stop();
	queue_removeAllMessages();

	setState(Idle);
}

void KviIrcSocket::outputSSLMessage(const QString &szMsg)
{
	m_pConsole->output(KVI_OUT_SSL,__tr2qs("[SSL]: %Q"),&szMsg);
}

void KviIrcSocket::outputSSLError(const QString &szMsg)
{
	m_pConsole->output(KVI_OUT_SSL,__tr2qs("[SSL ERROR]: %Q"),&szMsg);
}

void KviIrcSocket::outputProxyMessage(const QString &szMsg)
{
	QStringList list=QStringList::split("\n",szMsg);
	for(QStringList::Iterator it = list.begin(); it != list.end(); ++it)
	{
		QString szTemporary = (*it).stripWhiteSpace();
		m_pConsole->output(KVI_OUT_SOCKETMESSAGE,__tr2qs("[PROXY]: %Q"),&(szTemporary));
	}
}

void KviIrcSocket::outputProxyError(const QString &szMsg)
{
	QStringList list=QStringList::split("\n",szMsg);
	for(QStringList::Iterator it = list.begin(); it != list.end(); ++it)
	{
		QString szTemporary = (*it).stripWhiteSpace();
		m_pConsole->output(KVI_OUT_SOCKETERROR,__tr2qs("[PROXY ERROR]: %Q"),&(szTemporary));
	}
	
}

void KviIrcSocket::outputSocketMessage(const QString &szMsg)
{
	m_pConsole->output(KVI_OUT_SOCKETMESSAGE,__tr2qs("[SOCKET]: %Q"),&szMsg);
}

void KviIrcSocket::outputSocketError(const QString &szMsg)
{
	m_pConsole->output(KVI_OUT_SOCKETERROR,__tr2qs("[SOCKET ERROR]: %Q"),&szMsg);
}


void KviIrcSocket::outputSocketWarning(const QString &szMsg)
{
	m_pConsole->output(KVI_OUT_SOCKETWARNING,__tr2qs("[SOCKET WARNING]: %Q"),&szMsg);
}

void KviIrcSocket::setState(SocketState st)
{
	if(st != m_state)
	{
		m_state = st;
		m_pLink->socketStateChange();
	}
}

void KviIrcSocket::raiseError(int iError)
{
	m_iLastError = iError;
	//m_pConsole->socketError(iError);
	if( (m_iLastError==KviError_remoteEndClosedConnection) && ( m_state == ProxyHttpError) )
		outputSocketMessage(KviError::getDescription(iError));
	else
		outputSocketError(KviError::getDescription(iError));
}

int KviIrcSocket::startConnection(KviIrcServer *srv,KviProxy * prx,const char * bindAddress)
{
	// Attempts to estabilish an IRC connection
	// to the server specified by *srv.
	// Uses the proxy *prx if not 0
	if(m_state != Idle)return KviError_anotherConnectionInProgress;

	// Coherent state, thnx.
	reset();

	if(srv->useSSL())
	{
#ifndef COMPILE_SSL_SUPPORT
		return KviError_noSSLSupport;
#endif //!COMPILE_SSL_SUPPORT
	}

	// Copy the server
	m_pIrcServer = new KviIrcServer(*srv);

	bool bTargetIpV6 = false;
	bool bNeedServerIp = !prx;
	if(prx) bNeedServerIp = (
		prx->protocol() != KviProxy::Http && prx->protocol() != KviProxy::Socks5
		);
	
	// We're going to check the addresses now

	// check the proxy stuff...
	if(prx)
	{
		// Yeah...here comes the proxy
		m_pProxy = new KviProxy(*prx);
		// check the proxy IP address
		if(m_pProxy->isIpV6())
		{
			// IpV6 proxy :) (STILL QUITE UNTESTED ?)
#ifdef COMPILE_IPV6_SUPPORT
			bTargetIpV6 = true;
			if(!kvi_isValidStringIp_V6(m_pProxy->ip()))return KviError_invalidProxyAddress;
			// SOCKSV4 does not support IPV6 addresses
			if(m_pProxy->protocol() == KviProxy::Socks4)return KviError_socksV4LacksIpV6Support;
#else
			return KviError_noIpV6Support;
#endif
		} else {
			// IpV4 proxy
			if(!kvi_isValidStringIp(m_pProxy->ip()))return KviError_invalidProxyAddress;
		}
	}
	
	if(bNeedServerIp)
	{
		// check the irc host ip
#ifdef COMPILE_IPV6_SUPPORT
		if(m_pIrcServer->isIpV6())
		{
			// We have an IpV6 server host (Interesting if proxy is IpV4)
			if( !KviNetUtils::isValidStringIp_V6(m_pIrcServer->ip()) )return KviError_invalidIpAddress;
			if(!m_pProxy)bTargetIpV6 = true; // otherwise the proxy rules
		} else {
#endif
			// We have an IpV4 server host
			if(!KviNetUtils::isValidStringIp(m_pIrcServer->ip())) return KviError_invalidIpAddress;
#ifdef COMPILE_IPV6_SUPPORT
		}
#endif
	}

	KviSockaddr sa(prx ? m_pProxy->ip() : m_pIrcServer->ip().utf8().data(),prx ? m_pProxy->port() : m_pIrcServer->port(),bTargetIpV6);

	if(!sa.socketAddress())return KviError_invalidIpAddress;

	// create the socket
#ifdef COMPILE_IPV6_SUPPORT
	m_sock = kvi_socket_create(bTargetIpV6 ? KVI_SOCKET_PF_INET6 : KVI_SOCKET_PF_INET,KVI_SOCKET_TYPE_STREAM,KVI_SOCKET_PROTO_TCP);
#else
	m_sock = kvi_socket_create(KVI_SOCKET_PF_INET,KVI_SOCKET_TYPE_STREAM,KVI_SOCKET_PROTO_TCP);
#endif

	if(m_sock < 0)return KviError_socketCreationFailed;

	if(bindAddress)
	{
		// we have to bind the socket to a local address
		KviSockaddr localSa(bindAddress,0,bTargetIpV6);
		bool bBindOk = localSa.socketAddress();

		if(bBindOk)
		{
			bBindOk = kvi_socket_bind(m_sock,localSa.socketAddress(),((int)(localSa.addressLength())));
		}

		QString tmp;
		if(bBindOk)
		{
			if(_OUTPUT_VERBOSE)
				KviQString::sprintf(tmp,__tr2qs("Binding to local address %s"),bindAddress);
			outputSocketMessage(tmp);
		} else {
			if(_OUTPUT_VERBOSE)
				KviQString::sprintf(tmp,__tr2qs("Binding to local address %s failed: the kernel will choose the correct interface"),bindAddress);
			outputSocketWarning(tmp);
		}
	}

	// make it non blocking
	if(!kvi_socket_setNonBlocking(m_sock))
	{
		reset();
		return KviError_asyncSocketFailed;
	}

	if(!kvi_socket_connect(m_sock,sa.socketAddress(),((int)(sa.addressLength()))))
	{
		// ops...
		int err = kvi_socket_error();

		if(!kvi_socket_recoverableConnectError(err))
		{
			// Ops...
			int sockError=err;
			if(sockError==0)
			{
				// Zero error ?...let's look closer
				int iSize=sizeof(int);
				if(!kvi_socket_getsockopt(m_sock,SOL_SOCKET,SO_ERROR,(void *)&sockError,&iSize))sockError=0;
			}
			// die :(
			reset();
			// And declare problems :)
			if(sockError)return KviError::translateSystemError(sockError);
			else return KviError_unknownError; //Error 0 ?
		}
	}

	// and setup the WRITE notifier...
	m_pWsn = new QSocketNotifier((int)m_sock,QSocketNotifier::Write);
	QObject::connect(m_pWsn,SIGNAL(activated(int)),this,SLOT(writeNotifierFired(int)));
	m_pWsn->setEnabled(true);

	// set the timer
	if(KVI_OPTION_UINT(KviOption_uintIrcSocketTimeout) < 5)KVI_OPTION_UINT(KviOption_uintIrcSocketTimeout) = 5;
	m_pTimeoutTimer = new QTimer();
	QObject::connect(m_pTimeoutTimer,SIGNAL(timeout()),this,SLOT(connectionTimedOut()));
	m_pTimeoutTimer->start(KVI_OPTION_UINT(KviOption_uintIrcSocketTimeout) * 1000,true);

	// and wait for connect
	setState(Connecting);

	return KviError_success;
}

void KviIrcSocket::connectionTimedOut()
{
	// the m_pTimeoutTimer fired :(
	raiseError(KviError_connectionTimedOut);
	reset();
}

void KviIrcSocket::writeNotifierFired(int)
{
	// kill the timeout timer
	if(m_pTimeoutTimer)
	{
		delete m_pTimeoutTimer;
		m_pTimeoutTimer = 0;
	}

	// Check for errors...
	int sockError;
	int iSize=sizeof(int);
	if(!kvi_socket_getsockopt(m_sock,SOL_SOCKET,SO_ERROR,(void *)&sockError,&iSize))sockError = -1;
	//sockError = 0;
	if(sockError != 0)
	{
		//failed
		if(sockError > 0)sockError = KviError::translateSystemError(sockError);
		else sockError = KviError_unknownError; //Error 0 ?
		raiseError(sockError);
		reset();
		return;
	}

	// kill the write notifier
	delete m_pWsn;
	m_pWsn = 0;

	//Succesfully connected...
	connectionEstabilished();
}

void KviIrcSocket::connectionEstabilished()
{
	// the connection with a remote end has been estabilished
	// if it is a proxy we need to perform the login operations
	// otherwise we're connected to the irc server
	if(m_sock == KVI_INVALID_SOCKET)return; // ops...disconnected in setState() ????

	if(m_pProxy)connectedToProxy();
	else connectedToIrcServer();
}

void KviIrcSocket::connectedToProxy()
{
	if(!m_pProxy)qDebug("WARNING: connectedToProxy() without a m_pProxy!");

	// FIXME: Do we want to support SSL proxies ?
	//        it would be just a matter of SSL handshaking
	//        with the proxy

	setState(ProxyLogin);

	if(m_pRsn)
	{
		delete m_pRsn;
		m_pRsn = 0;
	}

	m_pRsn = new QSocketNotifier((int)m_sock,QSocketNotifier::Read);
		
	QObject::connect(m_pRsn,SIGNAL(activated(int)),this,SLOT(readProxyData(int)));

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
	char buffer[256];
	int readLength;
	
	readLength = kvi_socket_recv(m_sock,buffer,255);
	if(readLength <= 0)
	{
		handleInvalidSocketRead(readLength);
		return;
	}
	
	outputProxyMessage(m_pConsole->decodeText(buffer));
}

void KviIrcSocket::connectedToIrcServer()
{
#ifdef COMPILE_SSL_SUPPORT
	if(m_pIrcServer->useSSL())
	{
		m_pSSL = KviSSLMaster::allocSSL(m_pConsole,m_sock,KviSSL::Client);
		if(!m_pSSL)
		{
			raiseSSLError();
			raiseError(KviError_SSLError);
			reset();
			return;
		}
		setState(SSLHandshake);
		doSSLHandshake(0);
		return;
	}
#endif
	linkUp();
}


void KviIrcSocket::readProxyData(int)
{
	char buffer[256];

	int readLength;
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
						raiseError(KviError_SSLError);
						reset();
						return;
					}
				}
				break;
				case KviSSL::SSLError:
					raiseSSLError();
					raiseError(KviError_SSLError);
					reset();
					return;
				break;
				default:
					raiseError(KviError_SSLError);
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
		readLength = kvi_socket_recv(m_sock,buffer,255);
		if(readLength <= 0)
		{
			handleInvalidSocketRead(readLength);
			return;
		}
/*
#ifdef COMPILE_SSL_SUPPORT
	}
#endif
*/
	// we need at least two bytes...
	if(readLength < 2)
	{
		// a single byte of reply means:
		// - connection through a 1 bps modem
		// - a totally blocked network
		// - remote host is not a SOCKS/HTTP server
		// Anyway....it is always a meaningless reply
		// better to try again later :)
		raiseError(KviError_unrecognizedProxyReply);
		reset();
		return;
	}
	// handle the reply
	switch(m_state)
	{
		case ProxyFinalV4:
			//V4 final reply
			proxyHandleV4FinalReply((unsigned char)buffer[1]);
			break;
		case ProxySelectAuthMethodV5:
			//V5 method selection reply
			proxyHandleV5MethodReply((unsigned char)buffer[1]);
			break;
		case ProxyUserPassV5:
			//V5 user and pass reply
			proxyHandleV5AuthReply((unsigned char)buffer[1]);
			break;
		case ProxyFinalV5:
			//V5 final reply
			proxyHandleV5FinalReply((unsigned char)buffer[1]);
			break;
		case ProxyFinalHttp:
			//Http final reply
			buffer[readLength] = '\0';
			proxyHandleHttpFinalReply(buffer,readLength);
			break;
		default:
			// what ?
			raiseError(KviError_unrecognizedProxyReply);
			reset();
			break;
	}
}



void KviIrcSocket::proxyLoginHttp()
{
	// Well..this is just plain and easy: connect to the proxy machine
	// and say "CONNECT <irc.server>:<port> HTTP/<version>\n\n"
	// if it requires auth than say Proxy-Authorization: Basic user:passwd
	// Then expect a server reply header (2 newlines)
	// HTTP 200 = Success
	// HTTP Anything else = Failure
	
	if(_OUTPUT_VERBOSE)
		outputProxyMessage(__tr2qs("Using HTTP protocol."));

	setState(ProxyFinalHttp);
	KviStr tmp(KviStr::Format,"CONNECT %s:%u HTTP/1.0\r\n",m_pIrcServer->hostName().utf8().data(),(unsigned int)(m_pIrcServer->port()));
	if(m_pProxy->hasUser())
	{
		KviStr auth(KviStr::Format,"%s:%s",m_pProxy->user(),m_pProxy->pass());
		KviStr encoded;
		encoded.bufferToBase64(auth.ptr(),auth.len());
		tmp.append(KviStr::Format,"Proxy-Authorization: Basic %s\r\n\r\n",encoded.ptr());
	} else {
		tmp.append("\r\n");
	}
//	debug(tmp.ptr());
	sendRawData(tmp.ptr(),tmp.len());
}

void KviIrcSocket::proxyLoginV4()
{
	// SOCKSV4 protocol
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
		outputProxyMessage(__tr2qs("Using SOCKSV4 protocol."));

	m_pProxy->normalizeUserAndPass();
	// the protocol does not specify the "userid" format...
	// so build an userid from the pass and/or username...

	KviStr szUserAndPass=m_pProxy->user();
	if(m_pProxy->hasPass()){
		if(szUserAndPass.hasData())szUserAndPass.append(' ');
		szUserAndPass.append(m_pProxy->pass());
	}
	int iLen = szUserAndPass.len()+9;
	// build the request packet
	char *bufToSend = new char[iLen];
	bufToSend[0]=(unsigned char)4; //Version 4
	bufToSend[1]=(unsigned char)1; //Connect
	Q_UINT16 port=(Q_UINT16)htons(m_pIrcServer->port());
	kvi_memmove((void *)(bufToSend+2),(void *)&port,2);

	struct in_addr ircInAddr;

	if(!kvi_stringIpToBinaryIp(m_pIrcServer->ip(),&ircInAddr))
		debug("SOCKET INTERNAL ERROR IN IPV4 (SOCKS4) ADDR CONVERSION");

	Q_UINT32 host=(Q_UINT32)ircInAddr.s_addr;
	kvi_memmove((void *)(bufToSend+4),(void *)&host,4);
	kvi_memmove((void *)(bufToSend+8),(void *)(szUserAndPass.ptr()),szUserAndPass.len());
	// send it into hyperspace...
	setState(ProxyFinalV4);
	sendRawData(bufToSend,iLen);
	delete[] bufToSend;
	// and wait for reply...
}

void KviIrcSocket::proxyLoginV5()
{
	// SOCKSV5 protocol.
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
	// so build an userid from the pass and/or username...

	char bufToSend[4];
	bufToSend[0]=(unsigned char)5; //use version 5
	int sendLen = 3;
	if(!(m_pProxy->hasUser() || m_pProxy->hasPass()))
	{
		// no auth needed.
		bufToSend[1]=(unsigned char)1; //select one method
		bufToSend[2]=(unsigned char)0; //select method 0 : no auth
		if(_OUTPUT_VERBOSE)
			outputProxyMessage(__tr2qs("We can accept auth method 0 (no auth)"));
	} else {
		// we can provide a password and username if needed...
		bufToSend[1]=(unsigned char)2; //select from two methods
		bufToSend[2]=(unsigned char)0; //method 0 or
		bufToSend[3]=(unsigned char)2; //method 2 username/pass auth
		sendLen = 4;
		if(_OUTPUT_VERBOSE)
			outputProxyMessage(__tr2qs("We can accept auth method 0 (no auth) or 2 (user/pass)"));
	}
	// notify the user before sending...since we may get disconnected
	setState(ProxySelectAuthMethodV5);
	sendRawData(bufToSend,sendLen);
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
	unsigned int lPass=(unsigned int)m_pProxy->passLen();
	if(lPass>255)lPass=255;
	unsigned int lUser=(unsigned int)m_pProxy->userLen();
	if(lUser>255)lUser=255;
	int iLen=lPass+lUser+3;
	char *bufToSend=new char[iLen];
	bufToSend[0]=(unsigned char)1;                           //version x'01'
	bufToSend[1]=(unsigned char)lUser;                       //length of the username
	kvi_memmove((void *)(bufToSend+2),(void *)m_pProxy->user(),lUser); //username
	bufToSend[2+lUser]=(unsigned char)lPass;             //length of the password
	kvi_memmove((void *)(bufToSend+3+lUser),(void *)m_pProxy->pass(),lPass);
	// spit out the buffer and wait
	setState(ProxyUserPassV5);
	sendRawData(bufToSend,iLen);
	delete[] bufToSend;
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
	bool bRemoteDns=!(
		
		(
		KviNetUtils::isValidStringIp(m_pIrcServer->ip()) 
		#ifdef COMPILE_IPV6_SUPPORT
			|| KviNetUtils::isValidStringIp_V6(m_pIrcServer->ip())
		#endif
		)

		&& m_pIrcServer->cacheIp()
		);
	int bufLen = bRemoteDns ? 4 + 1 + m_pIrcServer->hostName().utf8().length() + 2
		: (m_pIrcServer->isIpV6() ? 22 : 10);
	char * bufToSend = (char *)kvi_malloc(sizeof(char) * bufLen);
	bufToSend[0]=(unsigned char)5;           //Proto 5
	bufToSend[1]=(unsigned char)1;           //CONNECT
	bufToSend[2]=(unsigned char)0;           //RSV
	
	if(bRemoteDns)
	{
		bRemoteDns=true;
		bufToSend[3]=3;
		bufToSend[4]=m_pIrcServer->hostName().utf8().length();		
	} else {
		bufToSend[3]=(unsigned char)m_pIrcServer->isIpV6() ? 4 : 1; // IPV6 : IPV4
	}

	if(bRemoteDns)
	{
		kvi_memmove((void *)(bufToSend + 5),
			(void *)(m_pIrcServer->hostName().utf8().data()),
			m_pIrcServer->hostName().utf8().length());
		Q_UINT16 port = (Q_UINT16)htons(m_pIrcServer->port());
		kvi_memmove((void *)(bufToSend + 4 + 1 + m_pIrcServer->hostName().utf8().length()),(void *)&port,2);
	} else if(m_pIrcServer->isIpV6()) {
#ifdef COMPILE_IPV6_SUPPORT
		struct in6_addr ircInAddr;

		if(!kvi_stringIpToBinaryIp_V6(m_pIrcServer->ip(),&ircInAddr))debug("SOCKET INTERNAL ERROR IN IPV6 ADDR CONVERSION");
		kvi_memmove((void *)(bufToSend + 4),(void *)(&ircInAddr),4);
		Q_UINT16 port = (Q_UINT16)htons(m_pIrcServer->port());
		kvi_memmove((void *)(bufToSend + 20),(void *)&port,2);
#endif
	} else {
		struct in_addr ircInAddr;

		if(!kvi_stringIpToBinaryIp(m_pIrcServer->ip(),&ircInAddr))debug("SOCKET INTERNAL ERROR IN IPV4 ADDR CONVERSION");
		Q_UINT32 host = (Q_UINT32)ircInAddr.s_addr;
		kvi_memmove((void *)(bufToSend + 4),(void *)&host,4);
		Q_UINT16 port = (Q_UINT16)htons(m_pIrcServer->port());
		kvi_memmove((void *)(bufToSend + 8),(void *)&port,2);
	}

	// send it into hyperspace...
	setState(ProxyFinalV5);
	sendRawData(bufToSend,bufLen);
	kvi_free(bufToSend);
	// and wait for reply...
}


void KviIrcSocket::proxyHandleV5AuthReply(unsigned char reply)
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
	if(reply == 0)
	{
		if(_OUTPUT_VERBOSE)
			outputProxyMessage(__tr2qs("Proxy response: auth OK: access granted"));
		proxySendTargetDataV5();
		return;
	}
	raiseError(KviError_proxyAuthFailed);
	reset();
}

void KviIrcSocket::proxyHandleV5MethodReply(unsigned char reply)
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
	if(reply == 0)
	{
		if(_OUTPUT_VERBOSE)
			outputProxyMessage(__tr2qs("Proxy response: Auth method OK: using method 0 (no auth)"));
		proxySendTargetDataV5();
		return;
	}
	if(reply == 2)
	{
		if(_OUTPUT_VERBOSE)
			outputProxyMessage(__tr2qs("Proxy response: Auth method OK: using method 2 (user/pass)"));
		proxyAuthUserPassV5();
		return;
	}
	//Request rejected
	if(reply == 0xFF)
	{
		raiseError(KviError_proxyNoAcceptableAuthMethod);
		reset();
	} else {
		// unrecognized...
		raiseError(KviError_unrecognizedProxyReply);
		reset();
	}
}

void KviIrcSocket::proxyHandleV5FinalReply(unsigned char reply)
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
	if(reply==0)
	{
		// Request granted
		if(_OUTPUT_VERBOSE)
			outputProxyMessage(__tr2qs("Proxy response: target data OK: request granted"));
		connectedToIrcServer();
	} else {
		//Request rejected
		int err;
		switch(reply)
		{
			case 1: err = KviError_proxyReply01GeneralSOCKSFailure; break;
			case 2: err = KviError_proxyReply02ConnectionNotAllowed; break;
			case 3: err = KviError_proxyReply03NetworkUnreachable; break;
			case 4: err = KviError_proxyReply04HostUnreachable; break;
			case 5: err = KviError_proxyReply05ConnectionRefused; break;
			case 6: err = KviError_proxyReply06TTLExpired; break;
			case 7: err = KviError_proxyReply07CommandNotSupported; break;
			case 8: err = KviError_proxyReply08AddressTypeNotSupported; break;
			case 9: err = KviError_proxyReply09InvalidAddress; break;
			default: err = KviError_unrecognizedProxyReply; break;
		}
		raiseError(err);
		reset();
	}
}

void KviIrcSocket::proxyHandleV4FinalReply(unsigned char reply)
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
	//        92: request rejected becasue SOCKS server cannot connect to
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
	if(reply==90)
	{
		// Request granted
		if(_OUTPUT_VERBOSE)
			outputProxyMessage(__tr2qs("Proxy response: target data OK: request granted"));
		connectedToIrcServer();
	} else {
		//Request rejected
		int err;
		switch(reply)
		{
			case 91: err = KviError_proxyReply91RequestFailed; break;
			case 92: err = KviError_proxyReply92IdentFailed; break;
			case 93: err = KviError_proxyReply93IdentNotMatching; break;
			default: err = KviError_unrecognizedProxyReply; break;
		}
		raiseError(err);
		reset();
	}
	// Just looked out of the window...
	// Hmmmm...strange light outside...
	// Looked at the clock...6:34 !
	// I think I'll go sleep.... :)
}

void KviIrcSocket::proxyHandleHttpFinalReply(const char * buffer,int bufLen)
{
	// Escape character is '^]'.
	// CONNECT warszawa.irc.pl:6667 HTTP/1.0
	//
	// HTTP/1.0 200 Connection established

	KviStr tmp = buffer;
	// FIXME: #warning "We could even show the proxy output here...!"
	tmp.cutFromFirst('\n');
	tmp.stripWhiteSpace();

	if(kvi_strEqualCIN(tmp.ptr(),"HTTP",4))
	{
		int idx = tmp.findFirstIdx(" 200 ");
		if(idx != -1)
		{
			if(idx == tmp.findFirstIdx(' '))
			{
				QString msg = __tr2qs("Proxy response: ");
				msg += tmp.ptr();
				if(_OUTPUT_VERBOSE)
					outputProxyMessage(msg);
				connectedToIrcServer();
				return;
			}
		}
		
	}

	outputProxyError(__tr2qs("Proxy said something about: \n"));
	outputProxyMessage(m_pConsole->decodeText(buffer));
	
	//Read HTTP error page and show it
	
	if(m_pWsn)
	{
		delete m_pWsn;
		m_pWsn = 0;
	}

	if(m_pRsn)
	{
		delete m_pRsn;
		m_pRsn = 0;
	}

	m_pRsn = new QSocketNotifier((int)m_sock,QSocketNotifier::Read);
	QObject::connect(m_pRsn,SIGNAL(activated(int)),this,SLOT(readHttpProxyErrorData(int)));
	m_pRsn->setEnabled(true);
	
	setState(ProxyHttpError);
	
//	raiseError(KviError_proxyHttpFailure);
//	reset();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SSL HANDSHAKE
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef COMPILE_SSL_SUPPORT

void KviIrcSocket::printSSLPeerCertificate()
{
	KviSSLCertificate * c = m_pSSL->getPeerCertificate();
	if(c)
	{
		//m_pConsole->socketEvent(SSLCertificate,(void *)c);
		if(_OUTPUT_VERBOSE)
			KviSSLMaster::printSSLCertificate(m_pConsole,__tr("Server X509 certificate"),c);
		delete c;
	} else {
		if(_OUTPUT_VERBOSE)
			outputSSLMessage(__tr2qs("The server didn't provide a certificate"));
	}
}

void KviIrcSocket::printSSLCipherInfo()
{
	KviSSLCipherInfo * ci = m_pSSL->getCurrentCipherInfo();
	if(ci)
	{
		//m_pConsole->socketEvent(SSLCipherInfo,(void *)ci);
		KviSSLMaster::printSSLCipherInfo(m_pConsole,__tr2qs("Current transmission cipher"),ci);
		delete ci;
	} else {
		if(_OUTPUT_VERBOSE)
			outputSSLMessage(__tr2qs("Unable to determine the current cipher"));
	}
}

void KviIrcSocket::raiseSSLError()
{
	KviStr buffer;
	while(m_pSSL->getLastErrorString(buffer))
	{
		outputSSLError(buffer.ptr());
	}
}

#endif


void KviIrcSocket::doSSLHandshake(int)
{
#ifdef COMPILE_SSL_SUPPORT
	__range_valid(m_pSSL);

	if(m_pRsn)
	{
		delete m_pRsn;
		m_pRsn = 0;
	}
	if(m_pWsn)
	{
		delete m_pWsn;
		m_pWsn = 0;
	}

	if(!m_pSSL)
	{
		qDebug("Ops... I've lost the SSL class ?");
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
			m_pRsn = new QSocketNotifier((int)m_sock,QSocketNotifier::Read);
			QObject::connect(m_pRsn,SIGNAL(activated(int)),this,SLOT(doSSLHandshake(int)));
			m_pRsn->setEnabled(true);
		break;
		case KviSSL::WantWrite:
			m_pWsn = new QSocketNotifier((int)m_sock,QSocketNotifier::Write);
			QObject::connect(m_pWsn,SIGNAL(activated(int)),this,SLOT(doSSLHandshake(int)));
			m_pWsn->setEnabled(true);
		break;
		case KviSSL::RemoteEndClosedConnection:
			raiseError(KviError_remoteEndClosedConnection);
			reset();
		break;
		case KviSSL::SSLError:
			raiseSSLError();
			raiseError(KviError_SSLError);
			reset();
		break;
		case KviSSL::SyscallError:
		{
			// syscall problem
			int err = kvi_socket_error();
			if(!kvi_socket_recoverableError(err))
			{
				// Declare problems :)
				raiseError((err ? KviError::translateSystemError(err) : KviError_unknownError));
			} else {
				// can recover ? (EAGAIN , EINTR ?)
				m_pWsn = new QSocketNotifier((int)m_sock,QSocketNotifier::Write);
				QObject::connect(m_pWsn,SIGNAL(activated(int)),this,SLOT(doSSLHandshake(int)));
				m_pWsn->setEnabled(true);
				return;
			}
			reset();
		}
		break;
		default:
			raiseError(KviError_SSLError);
			reset();
		break;
	}

#else  //!COMPILE_SSL_SUPPORT
	qDebug("Ops.. ssl handshake without ssl support!...aborting!");
	exit(-1);
#endif //!COMPILE_SSL_SUPPORT
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LINK UP
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void KviIrcSocket::linkUp()
{
	setState(Connected);

	// the last check
	if(m_sock == KVI_INVALID_SOCKET)return; // ops...disconnected in setState() ????

	// ok.. it seems that we're really up and running now!
	if(m_pWsn)
	{
		delete m_pWsn;
		m_pWsn = 0;
	}

	if(m_pRsn)
	{
		delete m_pRsn;
		m_pRsn = 0;
	}

	m_pRsn = new QSocketNotifier((int)m_sock,QSocketNotifier::Read);
	QObject::connect(m_pRsn,SIGNAL(activated(int)),this,SLOT(readData(int)));
	m_pRsn->setEnabled(true);
	
	// yahoo!
}

void KviIrcSocket::readData(int)
{
	//read data
	char buffer[1025];
	int readLength;
#ifdef COMPILE_SSL_SUPPORT
	if(m_pSSL)
	{
		readLength = m_pSSL->read(buffer,1024);
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
						raiseError(KviError_SSLError);
						reset();
						return;
					}
				}
				break;
				case KviSSL::SSLError:
					raiseSSLError();
					raiseError(KviError_SSLError);
					reset();
					return;
				break;
				default:
					raiseError(KviError_SSLError);
					reset();
					return;
				break;
	
			}
			handleInvalidSocketRead(readLength);
			return;
		}
	} else {
#endif
		readLength = kvi_socket_recv(m_sock,buffer,1024);
		if(readLength <= 0)
		{
			handleInvalidSocketRead(readLength);
			return;
		}
#ifdef COMPILE_SSL_SUPPORT
	}
#endif


	//terminate our buffer
	(*(buffer+readLength))='\0';

	m_uReadBytes += readLength;

	// Shut up the socket notifier
	// in case that we enter in a local loop somewhere
	// while processing data...
	m_pRsn->setEnabled(false);
	// shut also the flushing of the message queue
	// in this way we prevent disconnect detection
	// during the processing of a message effectively
	// making it always an asynchronous event.
	m_bInProcessData = true;

	m_pLink->processData(buffer,readLength);
	// after this line there should be nothing that relies
	// on the "connected" state of this socket.
	// It may happen that it has been reset() in the middle of the processData() call
	// and (unverified) it might have been even deleted.

	// re-enable the socket notifier... (if it's still there)
	if(m_pRsn)m_pRsn->setEnabled(true);
	// and the message queue flushing
	m_bInProcessData = false;
	// and flush the queue too!
	if(m_pSendQueueHead)flushSendQueue();
}

/*
void KviIrcSocket::processData(char * buffer,int)
{
	register char *p=buffer;
	char *beginOfCurData = buffer;
	int   bufLen = 0;
	char *messageBuffer = (char *)kvi_malloc(1);

	// Shut up the socket notifier
	// in case that we enter in a local loop somewhere
	// while processing data...
	m_pRsn->setEnabled(false);
	// shut also the flushing of the message queue
	// in this way we prevent disconnect detection
	// during the processing of a message effectively
	// making it always an asynchronous event.
	m_bInProcessData = true;

	while(*p)
	{
		if((*p == '\r' )||(*p == '\n'))
		{
			//found a CR or LF...
			//prepare a message buffer
			bufLen = p - beginOfCurData;
			//check for previous unterminated data
			if(m_uReadBufferLen > 0){
				__range_valid(m_pReadBuffer);
				messageBuffer = (char *)kvi_realloc(messageBuffer,bufLen + m_uReadBufferLen + 1);
				kvi_memmove(messageBuffer,m_pReadBuffer,m_uReadBufferLen);
				kvi_memmove((void *)(messageBuffer + m_uReadBufferLen),beginOfCurData,bufLen);
				*(messageBuffer + bufLen + m_uReadBufferLen) = '\0';
				m_uReadBufferLen = 0;
				kvi_free(m_pReadBuffer);
				m_pReadBuffer = 0;
			} else {
				__range_invalid(m_pReadBuffer);
				messageBuffer = (char *)kvi_realloc(messageBuffer,bufLen + 1);
				kvi_memmove(messageBuffer,beginOfCurData,bufLen);
				*(messageBuffer + bufLen) = '\0';
			}
			m_uReadPackets++;

			// FIXME: actually it can happen that the socket gets disconnected
			// in a incomingMessage() call.
			// The problem might be that some other parts of kvirc assume
			// that the irc context still exists after a failed write to the socket
			// (some parts don't even check the return value!)
			// If the problem presents itself again then the solution is:
			//   disable queue flushing for the "incomingMessage" call
			//   and just call queue_insertMessage()
			//   then after the call terminates flush the queue (eventually detecting
			//   the disconnect and thus destroying the irc context).
			// For now we try to rely on the remaining parts to handle correctly
			// such conditions. Let's see...
			
			m_pConsole->incomingMessage(messageBuffer);

			if(m_state != Connected)
			{
				// Disconnected in KviConsole::incomingMessage() call.
				// This may happen for several reasons (local event loop
				// with the user hitting the disconnect button, a scripting
				// handler event that disconnects explicitly)
				//
				// We handle it by simply returning control to readData() which
				// will return immediately (and safely) control to Qt
				kvi_free(messageBuffer);
				m_bInProcessData = false;
				return;
			}

			while(*p && ((*p=='\r')||(*p=='\n')) )p++;
			beginOfCurData = p;

		} else p++;
	}

	//now *p == '\0'
	//beginOfCurData points to '\0' if we have
	//no more stuff to parse , or points to something
	//different than '\r' or '\n'...
	if(*beginOfCurData)
	{
		//Have remaining data...in the local buffer
		bufLen = p - beginOfCurData;
		if(m_uReadBufferLen > 0){
			//and there was more stuff saved... (really slow connection)
			__range_valid(m_pReadBuffer);
			m_pReadBuffer =(char *)kvi_realloc(m_pReadBuffer,m_uReadBufferLen + bufLen);
			kvi_memmove((void *)(m_pReadBuffer+m_uReadBufferLen),beginOfCurData,bufLen);
			m_uReadBufferLen += bufLen;
		} else {
			//
			__range_invalid(m_pReadBuffer);
			m_uReadBufferLen = bufLen;
			m_pReadBuffer =(char *)kvi_malloc(m_uReadBufferLen);
			kvi_memmove(m_pReadBuffer,beginOfCurData,m_uReadBufferLen);
		}
		//The m_pReadBuffer contains at max 1 irc message...
		//that can not be longer than 510 bytes (the message is not CRLF terminated)
		// FIXME: Is this limit *really* valid on all servers ?
		if(m_uReadBufferLen > 510)debug("WARNING : Receiving an invalid irc message from server.");
	}
	kvi_free(messageBuffer);

	// re-enable the socket notifier...
	m_pRsn->setEnabled(true);
	// and the message queue flushing
	m_bInProcessData = false;
	// and flush the queue too!
	if(m_pSendQueueHead)flushSendQueue();
}
*/

void KviIrcSocket::abort()
{
	// flush the send queue if possible (and if not yet disconnected in fact)
	if(m_state == Connected)flushSendQueue();
	if(m_state != Idle)
		raiseError(KviError_operationAborted);
	// and reset
	reset();
}

//=== handleInvalidSocketRead ===============================================//
//
// Checks if the socket error is a transient error
// If it is not a transient error it resets the socket
// and fires the appropriate event.
// Otherwise it does nothing.
//
void KviIrcSocket::handleInvalidSocketRead(int readedLength)
{
	__range_valid(readedLength <= 0);
	if(readedLength==0)
	{
		raiseError(KviError_remoteEndClosedConnection);
		reset();
	} else {
		//check for transmission errors
		int err = kvi_socket_error();
#ifdef COMPILE_ON_WINDOWS
		if((err != EAGAIN) && (err != EINTR) && (err != WSAEWOULDBLOCK))
#else
		if((err != EAGAIN) && (err != EINTR))
#endif
		{
			if(err > 0)raiseError((KviError::translateSystemError(err)));
			else raiseError(KviError_remoteEndClosedConnection);
			reset();
		} //else transient error...wait again...
	}
}
//=== data queue functions ==================================================//
//
// queue_insertMessage : appends a KviIrcSocketMsgEntry to the tail of
//     the message queue. The next_ptr for this message is set to 0.
// queue_removeMessage : removes a message from the head of the queue.
//

void KviIrcSocket::queue_insertMessage(KviIrcSocketMsgEntry *msg_ptr)
{
	__range_valid(msg_ptr);
	__range_valid(msg_ptr->data_ptr);
	__range_valid(msg_ptr->data_len);
	msg_ptr->next_ptr = 0;
	if(m_pSendQueueHead)
	{
		m_pSendQueueTail->next_ptr = msg_ptr;
		m_pSendQueueTail = msg_ptr;
	} else {
		m_pSendQueueHead = msg_ptr;
		m_pSendQueueTail = msg_ptr;
	}
}

void KviIrcSocket::free_msgEntry(KviIrcSocketMsgEntry * e)
{
	if(e->pData)delete e->pData;
    e->pData = 0;
	kvi_free(e);
}

bool KviIrcSocket::queue_removeMessage()
{
	__range_valid(m_pSendQueueTail);
	__range_valid(m_pSendQueueHead);
	if(m_pSendQueueHead->pData)delete m_pSendQueueHead->pData;
	KviIrcSocketMsgEntry *aux_ptr = m_pSendQueueHead;
	m_pSendQueueHead = aux_ptr->next_ptr;
	kvi_free((void *)aux_ptr);
	if(m_pSendQueueHead == 0)
	{
		m_pSendQueueTail = 0;
		return false;
	} else return true;
}

void KviIrcSocket::queue_removeAllMessages()
{
	if(m_pSendQueueHead)while(queue_removeMessage());
}

//=== flushSendQueue ========================================================//
//
// Attempts to send as much as possible to the server
// If fails (happens only on really lagged servers)
// calls itself with a QTimer shot after KVI_OPTION_UINT(KviOption_uintSocketQueueFlushTimeout) ms
// to retry again...
//
void KviIrcSocket::flushSendQueue()
{
	// If we're called from the flush timer , stop it
	if(m_pFlushTimer->isActive())m_pFlushTimer->stop();

	// Ok...have something to send...
	__range_valid(m_state != Idle);

	struct timeval curTime;

	while(m_pSendQueueHead)
	{
		if(KVI_OPTION_BOOL(KviOption_boolLimitOutgoingTraffic))
		{
			kvi_gettimeofday(&curTime,0);

			int timeDiff = curTime.tv_usec - m_tAntiFloodLastMessageTime.tv_usec;
			timeDiff += (curTime.tv_sec - m_tAntiFloodLastMessageTime.tv_sec) * 1000000;


			if(((unsigned int)timeDiff) < KVI_OPTION_UINT(KviOption_uintOutgoingTrafficLimitUSeconds))
			{
				// need to wait for a while....
				m_pFlushTimer->start(((KVI_OPTION_UINT(KviOption_uintOutgoingTrafficLimitUSeconds) - timeDiff) / 1000) + 1);
				return;
			} // else can send
		}
		// Write one data buffer...
		int result;
#ifdef COMPILE_SSL_SUPPORT
		if(m_pSSL)
		{
			result = m_pSSL->write((char *)(m_pSendQueueHead->pData->data()),m_pSendQueueHead->pData->size());
		} else {
#endif
			result = kvi_socket_send(m_sock,(char *)(m_pSendQueueHead->pData->data()),m_pSendQueueHead->pData->size());
#ifdef COMPILE_SSL_SUPPORT
		}
#endif
		if(result == (int)m_pSendQueueHead->pData->size())
		{
			// Succesfull send...remove this data buffer
			m_uSentPackets++;
			m_uSentBytes += result;
			//if(m_pConsole->hasMonitors())outgoingMessageNotifyMonitors((char *)(m_pSendQueueHead->pData->data()),result);
			queue_removeMessage();
			if(KVI_OPTION_BOOL(KviOption_boolLimitOutgoingTraffic))
			{
				m_tAntiFloodLastMessageTime.tv_sec = curTime.tv_sec;
				m_tAntiFloodLastMessageTime.tv_usec = curTime.tv_usec;
			}
			// And try next buffer...
			continue;
		} else {
			// Something wrong ?
#ifdef COMPILE_SSL_SUPPORT
			if(result <= 0)
			{
				if(m_pSSL)
				{
					// ops...might be an SSL error
					switch(m_pSSL->getProtocolError(result))
					{
						case KviSSL::WantWrite:
						case KviSSL::WantRead:
							// Async continue...
							m_pFlushTimer->start(KVI_OPTION_UINT(KviOption_uintSocketQueueFlushTimeout));
							return;
							break;
						case KviSSL::SyscallError:
							if(result == 0)
							{
								raiseSSLError();
								raiseError(KviError_remoteEndClosedConnection);
								reset();
								return;
							} else {
								int iSSLErr = m_pSSL->getLastError(true);
								if(iSSLErr != 0)
								{
									raiseSSLError();
									raiseError(KviError_SSLError);
									reset();
									return;
								} else {
									goto handle_system_error;
								}
							}
						break;
						case KviSSL::SSLError:
							raiseSSLError();
							raiseError(KviError_SSLError);
							reset();
							return;
						break;
						default:
							raiseError(KviError_SSLError);
							reset();
							return;
						break;
					}
				}
			} else {
#else //!COMPILE_SSL_SUPPORT
			if(result >= 0)
			{
				if(result > 0)
				{
#endif //!COMPILE_SSL_SUPPORT

					// Partial send...need to finish it later
					m_pSendQueueHead->pData->remove(result);
					
					m_uSentBytes += result;
					if(_OUTPUT_VERBOSE)
						outputSocketWarning(__tr2qs("Partial socket write: packet broken into smaller pieces."));
#ifndef COMPILE_SSL_SUPPORT
				}
#endif //!COMPILE_SSL_SUPPORT
				// Async continue...
				m_pFlushTimer->start(KVI_OPTION_UINT(KviOption_uintSocketQueueFlushTimeout));
				return;
			}

handle_system_error:
			// Oops...error ?
			int err = kvi_socket_error();
#ifdef COMPILE_ON_WINDOWS
			if((err == EAGAIN) || (err == EINTR) || (err == WSAEWOULDBLOCK))
#else
			if((err == EAGAIN)||(err == EINTR))
#endif
			{
				// Transient error...partial send as before...
				if(_OUTPUT_VERBOSE)
					outputSocketWarning(__tr2qs("Partial socket write: packet broken into smaller pieces."));
				// Async continue...
				m_pFlushTimer->start(KVI_OPTION_UINT(KviOption_uintSocketQueueFlushTimeout));
				return;
			} else {
				// Disconnected... :(
				raiseError((KviError::translateSystemError(err)));
				reset();
				return;
			}
		}
	}
	//flushed completely ...
}

bool KviIrcSocket::getLocalHostIp(QString &szIp,bool bIpV6)
{
	if(m_state != Connected)return false;

	if(bIpV6)
	{
#ifdef COMPILE_IPV6_SUPPORT
		struct sockaddr_in6 name;
		int len = sizeof(name);
		if(!kvi_socket_getsockname(m_sock, (struct sockaddr *)&name,&len))return false;
		//I assume that getsockname returns data in Network byte order...
		//The man page misses to specify that...
		if(!kvi_binaryIpToStringIp_V6(name.sin6_addr,szIp))return false;
		return true;
#else
		return false; // no support
#endif
	}
	struct sockaddr_in name;
	int len = sizeof(name);
	if(!kvi_socket_getsockname(m_sock, (struct sockaddr *)&name,&len))return false;
	//I assume that getsockname returns data in Network byte order...
	//The man page misses to specify that...
	if(!kvi_binaryIpToStringIp(name.sin_addr,szIp))return false;
	return true;
}






/*
bool KviIrcSocket::sendFmtData(const char *fmt,...)
{
	if(m_state != Connected)return false;
	//new buffer
	KviIrcSocketMsgEntry *ptr = (KviIrcSocketMsgEntry *)kvi_malloc(sizeof(KviIrcSocketMsgEntry));
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
	KviIrcSocketMsgEntry *ptr = (KviIrcSocketMsgEntry *)kvi_malloc(sizeof(KviIrcSocketMsgEntry));
	if(buflen < 0)buflen = strlen(buffer);
	if(buflen > 510)
	{
		buflen = 510;
		if(_OUTPUT_VERBOSE)
			outputSocketWarning(__tr2qs("Socket message truncated to 512 bytes."));
	}
	ptr->pData = new KviDataBuffer(buflen + 2);
	kvi_memmove(ptr->pData->data(),buffer,buflen);
	*(ptr->pData->data()+buflen)='\r';
	*(ptr->pData->data()+buflen+1)='\n';

	queue_insertMessage(ptr);
	if(!m_bInProcessData)flushSendQueue();
	return (m_state != Idle);
}
*/

bool KviIrcSocket::sendRawData(const char *buffer,int buflen)
{
	if((m_state == Idle) || (m_state == Connecting))return false;
	//new buffer
	KviIrcSocketMsgEntry *ptr = (KviIrcSocketMsgEntry *)kvi_malloc(sizeof(KviIrcSocketMsgEntry));
	ptr->pData = new KviDataBuffer(buflen);
	kvi_memmove(ptr->pData->data(),buffer,buflen);
	queue_insertMessage(ptr);
	if(!m_bInProcessData)flushSendQueue();
	return (m_state != Idle);
}

bool KviIrcSocket::sendPacket(KviDataBuffer * pData)
{
	if(m_state != Connected)
	{
		delete pData;
        pData = 0;
		return false;
	}
	KviIrcSocketMsgEntry *ptr = (KviIrcSocketMsgEntry *)kvi_malloc(sizeof(KviIrcSocketMsgEntry));
	ptr->pData = pData;
	queue_insertMessage(ptr);
	if(!m_bInProcessData)flushSendQueue();
	return (m_state != Idle);
}
