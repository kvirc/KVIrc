//
//   File : class_socket.cpp
//   Creation date : Sun Nov 11 03:13:45 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"
#include "kvi_qstring.h"

#define _KVI_DEBUG_CHECK_RANGE_
#include "kvi_debug.h"


#include "kvi_locale.h"
#include "kvi_error.h"
#include "kvi_netutils.h"
#include "kvi_dns.h"
#include "kvi_error.h"
#include "kvi_socket.h"
#include "kvi_malloc.h"
#include "kvi_memmove.h"
#include "kvi_databuffer.h"
#include "kvi_qcstring.h"

#include "class_socket.h"

//#include <stdlib.h>

#define KVI_IN_BUFFER_ALLOC_CHUNK 4096
#define KVI_READ_CHUNK 1024
/*
static KviScriptObjectClass * g_pSocketClass = 0;

static KviScriptObject * socketClassCreateInstance(KviScriptObjectClass * cls,KviScriptObject *par,const char * nam)
{
	return new KviScriptSocketObject(cls,par,nam);
}

KviScriptSocketObject::KviScriptSocketObject(KviScriptObjectClass * cla,KviScriptObject * par,const char * nam)
: KviScriptObject(cla,par,nam)
{
	m_uConnectionId = 0;
	m_sock = KVI_INVALID_SOCKET;
	m_iStatus = KVI_SCRIPT_SOCKET_STATUS_DISCONNECTED;
	m_uRemotePort = 0;
	m_uSecondaryPort = 0;
	m_pSn = 0;
	m_pDns = 0;
	m_pDelayTimer = 0;
	m_uConnectTimeout = 60000;
	m_pInBuffer = 0;
	m_uInBufferLen = 0;
	m_uInDataLen = 0;
	m_pOutBuffer = new KviDataBuffer();
	m_pFlushTimer = new QTimer();
	m_bIpV6 = false;
	m_uLocalPort = 0;
	m_secondarySock = KVI_INVALID_SOCKET;
	connect(m_pFlushTimer,SIGNAL(timeout()),this,SLOT(tryFlush()));
}

KviScriptSocketObject::~KviScriptSocketObject()
{
	delete m_pOutBuffer;
	delete m_pFlushTimer;

	if(m_pInBuffer)kvi_free(m_pInBuffer);
	if(m_pDelayTimer)delete m_pDelayTimer;
	if(m_pDns)delete m_pDns;
	if(m_pSn)delete m_pSn;
	if(m_sock != KVI_INVALID_SOCKET)kvi_socket_close(m_sock);
	if(m_secondarySock != KVI_INVALID_SOCKET)kvi_socket_close(m_secondarySock);
}

void KviScriptSocketObject::reset()
{
	m_uConnectionId++; // this is the only place where this is incremented!
	if(m_pDelayTimer)
	{
		delete m_pDelayTimer;
		m_pDelayTimer = 0;
	}
	if(m_pDns)
	{
		delete m_pDns;
		m_pDns = 0;
	}
	if(m_pSn)
	{
		delete m_pSn;
		m_pSn = 0;
	}
	if(m_sock != KVI_INVALID_SOCKET)
	{
		kvi_socket_close(m_sock);
		m_sock = KVI_INVALID_SOCKET;
	}
	if(m_pInBuffer)
	{
		kvi_free(m_pInBuffer);
		m_pInBuffer = 0;
	}
	if(m_secondarySock)
	{
		kvi_socket_close(m_secondarySock);
		m_secondarySock = KVI_INVALID_SOCKET;
	}
	m_uSecondaryPort = 0;
	m_szSecondaryIp.clear();
	delete m_pOutBuffer;
	if(m_pFlushTimer->isActive())m_pFlushTimer->stop();
	m_pOutBuffer = new KviDataBuffer();
	m_uInBufferLen = 0;
	m_uInDataLen = 0;
	m_iStatus = KVI_SCRIPT_SOCKET_STATUS_DISCONNECTED;
	m_uRemotePort = 0;
	m_szRemoteIp.clear();
	m_uLocalPort = 0;
	m_szLocalIp.clear();
	m_bIpV6 = false;
}

/*
	@doc: socket
	@keyterms:
		socket object class
	@title:
		socket class
	@type:
		class
	@short:
		A Ipv4/Ipv6 TCP socket
	@inherits:
		[class]object[/class]
	@description:
		This class provides a standard TCP/IP socket functionality.[br]
		It can be used either for connecting to a remote host or to listening for incoming connections.[br]
		If the KVIrc executable has been compiled with the IPV6 protocol support , this socket also supports it.[br]
	@functions:
		!fn: $status()
		Returns the status of the socket :[br]
		4=connected[br]
		3=listening[br]
		2=connecting[br]
		1=in dns call[br]
		0=disconnected

		!fn: $connectTimeout()
		Returns the value of the connect attempt timeout in milliseconds.[br]
		This is the timeout after that a connection attempt will be considered as failed if the remote
		host is not responding. You can set it with [classfnc:socket]$setConnectTimeout[/classfnc]().

		!fn: $setConnectTimeout(<timeout>)
		Sets the connect timeout for this socket. <timeout> must be a positive value in milliseconds.[br]
		The default timeout is 60000.

		!fn: $connect(<host>,<port>)
		Attempts a connection to <host> on port <port>.[br]
		<host> can be a numeric internet address (either Ipv4 or Ipv6 (if supported)) or a hostname.[br]
		If a hostname is used, a DNS lookup is performed (the socket enters the "dns call" state).[br]
		This function returns 1 if the connect attempt can be succesfully initiated,
		0 otherwise.[br] In fact , this function returns 0 only if the supplied <port> parameter
		is invalid or the socket is in an incoherent state (already connected or listening):
		for a newly created socket and with a valid <port> number you can safely ignore
		the return value.[br]
		Please note that the connection is asynchronous: when this function returns
		the socket is NOT connected: it has just initiated the connect attempt
		and you will be notified of the attempt result by an asynchronous event call:
		in case of failure , $connectFailedEvent() will be called , in case of
		succes , $connectEvent() will be called.

		!fn: $listen([<port>[,<interface>[,<force_ipv6>]]])
		Attempts to listen on the specified <port> and <interface>.[br]
		If <port> is not passed it is assumed to be 0 , if <interface> is not passed , it is assumed to be
		"any interface" (INADDR_ANY).[br] Port 0 means that the kernel should choose a "random" port to listen on.[br]
		If the <interface> is recognized as IPV6 address , and IPV6 is supported , the socket listens
		in IPV6 mode. If <interface> is an empty string and <force_ipv6> is 1 the socket listens
		on "any ipv6 interface".[br]
		This function returns '1' in case of success and '0' in case of failure.[br]
		On some systems listening in the IPV6 namespace allows to accept also IPV4 connections (this includes
		linux but not windows afaik).[br]
		When an incoming connection will arrive , $incomingConnectionEvent() will be called.

		!fn: $accept(<socketId>)
		This function can be called only from inside $incomingConnectionEvent().[br]
		<socketId> must be a newly created socket sub-class instance object.[br]
		The passed object will be set up to handle the incoming connection and this object
		will remain in listening state (unless you explicitly close it).[br]
		This function returns '1' in case of success and '0' in case of failure.

		!fn: $connectEvent()
		This function is called when a connection attempt has been succesfully completed.
		The socket is actually connected to [classfnc:socket]$remoteIp[/classfnc]() on
		[classfnc:socket]$remotePort[/classfnc](). You can start
		writing data and you may expect [classfnc:socket]$dataAvailableEvent[/classfnc]() to be
		triggered.

		!fn: $incomingConnectionEvent()
		This function is called when an incoming connection arrives over a socket in listening state.[br]
		You must call [classfnc:socket]$accept[/classfnc]() passing a newly created socket object
		to accept and handle the connection.[br] If you don't call [classfnc:socket]$accept[/classfnc]()
		the incoming connection will be automatically terminated.

		!fn: $connectFailedEvent(<reason>)
		This function is called when a connection attempt fails for some reason. <reason> contains
		the error string.[br]
		This function may be called only between a call to [classfnc:socket]$connect[/classfnc]() and
		the [classfnc:socket]$connectEvent[/classfnc]().

		!fn: $disconnectEvent([error])
		This function is called when a connection is terminated either cleanly or because of an error.[br]
		[error] is an empty string in case of a "clean" termination (connection closed by the remote host)
		or is a message describing the socket error that caused the connection to be interrupted.

		!fn: $dataAvailableEvent(<data_length>)
		This function is called when some data is available to be read: the <data_length> parameter specifies
		the length of the available data in bytes.[br]
		You can use one of the $read* functions to obtain the data.

		!fn: $read(<length>)
		Reads at most <length> bytes of data from the socket. If <length> is anything "outside" the
		available data range (<length> < 0 or <length> > available_data_length), this function
		returns all the available data.[br]
		Please note that this function can't deal withi binary data: NULL characters are transformed to
		ASCII characters 255.

		!fn: $readHex(<length>)
		Reads at most <length> bytes of data from the socket. If <length> is anything "outside" the
		available data range (<length> < 0 or <length> > available_data_length), this function
		returns all the available data.[br]
		Returns the data encoded as hexadecimal digit string: this function can deal with binary data too.

		!fn: $write(<data>)
		Writes <data> to the socket.[br]
		This function can't deal with binary data (you can't send a NULL character)[br]
		Please note that when this function finishes it does not mean that the data has reached the remote end.[br]
		Basically it does not even mean that the data has been sent to the remote host.[br]
		The data is enqueued for sending and will be sent as soon as possible.[br]
		If you're going to [cmd]delete[/cmd] this object just after the $write call, you should
		call [classfnc:socket]$close[/classfnc]() just before [cmd]delete[/cmd] to ensure the data delivery.

		!fn: $writeHex(<hex_data>)
		Writes <data> to the socket.[br]
		<data> is expected to be a hexadecimal rappresentation of the bytes to be sent (two HEX characters
		for each byte). This means that the length of <hex_data> string must be a multiple of 2.[br]
		Returns the length of the actually decoded and sent data in bytes or -1 in case of error (the string
		was not a valid hexadecimla rappresentation).[br]
		Please note that when this function finishes it does not mean that the data has reached the remote end.[br]
		Basically it does not even mean that the data has been sent to the remote host.[br]
		The data is enqueued for sending and will be sent as soon as possible.[br]
		If you're going to [cmd]delete[/cmd] this object just after the $writeHex call, you should
		call [classfnc:socket]$close[/classfnc]() just before [cmd]delete[/cmd] to ensure the data delivery.

		!fn: $close()
		Resets this socket state: kills any pending or active connection. After a close() call
		the socket may be used for a new connection.[br]
		If there is an active connection, there is a last attempt to flush the pending outgoing data.[br]
		You don't need to call $close() if you [cmd]delete[/cmd] the socket: KVIrc will
		reset the socket state automatically and free the memory. But if you want to ensure data delivery
		after a $write call sequece and just before a [cmd]delete[/cmd], $close() is the only chance to do it.

		!fn: $remoteIp()
		Returns the IP address of the remote end of this socket.[br]
		The return value is meaningful only if the socket is in connected or connecting state.

		!fn: $setProtocol(<protocol>)
		Let KVIrc use TCP jr UDP protocol

		!fn: $remotePort()
		Returns the port of the remote end of this socket.[br]
		The return value is meaningful only if the socket is in connected or connecting state.

		!fn: $localIp()
		Returns the IP address of the local end of this socket.[br]
		The return value is meaningful only if the socket is in connected , listening or connecting state.

		!fn: $localPort()
		Returns the port of the local end of this socket.[br]
		The return value is meaningful only if the socket is in connected , listening or connecting state.

@examples:
		[example]
		#Server socket: listen 80 port and answer to requests (multi-threaded)[br]
[br]
		class (webserver,socket) {[br]
			constructor () {[br]
				$$->$listen(80, "127.0.0.1")[br]
			}[br]
			incomingConnectionEvent()[br]
			{[br]
				%tmp = $new(socket)[br]
				$$->$accept(%tmp)[br]
				echo "Webserver incoming Conection from: %tmp->$remoteIp : %tmp->$remotePort"[br]
				%tmp->$write("HTTP/1.1 200 OK\n\n<html><head></head><body><h1>KVIrc Webserver</h1></body></html>\n")[br]
				%tmp->$close()[br]
				delete %tmp[br]
			}[br]
		}[br]
		[br]
		#finally start webserver[br]
		%WebS = $new(webserver)[br]
		[br]
		[/example]
		[example]
		#Client socket - go to google and grab request header[br]
		class (httprequest,socket) {[br]
	constructor () [br]
	{[br]
		# connect to the server[br]
		$$->$connect("www.google.de",80)[br]
	}[br]
	destructor()[br]
	{[br]
		# if the socket is still open close it[br]
		if($$->$status() == 4) $$->$close()[br]
	}[br]
	connectFailedEvent() [br]
	{[br]
		# the connection to the server failed[br]
		echo  "Connection failed: "$0[br]
		delete $$[br]
	}[br]
	connectEvent() [br]
	{[br]
		# connection is complete[br]
		# send a request to receive the headers only from http://www.google.de/[br]
		$$->$write("HEAD / HTTP/1.1\r\nHost: www.google.de\r\nConnction: Close\r\nUser-Agent: KVIrc socket\r\n\r\n");[br]
	}[br]
	dataAvailableEvent() [br]
	{[br]
		# reading the received data[br]
		%newdata  = $$->$read($0)[br]
		echo %newdata[br]
		#close and delete the socket[br]
		$$->$close()[br]
		delete $$[br]
	}[br]
	disconnectEvent() [br]
	{[br]
		# connection has been closed[br]
		echo "Connection closed"[br]
		delete $$[br]
	}[br]
}[br]

#Creating the socket[br]
%Temp = $new(httprequest)[br]
		[/example]
*/

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_socket,"socket","object")

	KVSO_REGISTER_HANDLER(KviKvsObject_socket,"status",functionStatus)
	KVSO_REGISTER_HANDLER(KviKvsObject_socket,"remotePort",functionRemotePort)
	KVSO_REGISTER_HANDLER(KviKvsObject_socket,"remoteIp",functionRemoteIp)
	KVSO_REGISTER_HANDLER(KviKvsObject_socket,"localIp",functionLocalIp)
	KVSO_REGISTER_HANDLER(KviKvsObject_socket,"localPort",functionLocalPort)
	KVSO_REGISTER_HANDLER(KviKvsObject_socket,"connect",functionConnect)
	KVSO_REGISTER_HANDLER(KviKvsObject_socket,"connectTimeout",functionConnectTimeout)
	KVSO_REGISTER_HANDLER(KviKvsObject_socket,"setConnectTimeout",functionSetConnectTimeout)
	KVSO_REGISTER_HANDLER(KviKvsObject_socket,"close",functionClose)
	KVSO_REGISTER_HANDLER(KviKvsObject_socket,"read",functionRead)
	KVSO_REGISTER_HANDLER(KviKvsObject_socket,"readHex",functionReadHex)
	KVSO_REGISTER_HANDLER(KviKvsObject_socket,"write",functionWrite)
	KVSO_REGISTER_HANDLER(KviKvsObject_socket,"writeHex",functionWriteHex)
	KVSO_REGISTER_HANDLER(KviKvsObject_socket,"setProtocol",functionSetProtocol)
	KVSO_REGISTER_HANDLER(KviKvsObject_socket,"listen",functionListen)
	KVSO_REGISTER_HANDLER(KviKvsObject_socket,"accept",functionAccept)

	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_socket,"connectEvent")
	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_socket,"connectFailedEvent")
	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_socket,"disconnectEvent")
	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_socket,"dataAvailableEvent")
	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_socket,"incomingConnectionEvent")

KVSO_END_REGISTERCLASS(KviKvsObject_socket)


KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_socket,KviKvsObject)


	m_bUdp = false;
	m_uConnectionId = 0;
	m_sock = KVI_INVALID_SOCKET;
	m_iStatus = KVI_SCRIPT_SOCKET_STATUS_DISCONNECTED;
	m_uRemotePort = 0;
	m_uSecondaryPort = 0;
	m_pSn = 0;
	m_pDns = 0;
	m_pDelayTimer = 0;
	m_uConnectTimeout = 60000;
	m_pInBuffer = 0;
	m_uInBufferLen = 0;
	m_uInDataLen = 0;
	m_pOutBuffer = new KviDataBuffer();
	m_pFlushTimer = new QTimer();
	m_bIpV6 = false;
	m_uLocalPort = 0;
	m_secondarySock = KVI_INVALID_SOCKET;
	connect(m_pFlushTimer,SIGNAL(timeout()),this,SLOT(tryFlush()));
KVSO_END_CONSTRUCTOR(KviKvsObject_socket)

KVSO_BEGIN_DESTRUCTOR(KviKvsObject_socket)
	delete m_pOutBuffer;
	delete m_pFlushTimer;

	if(m_pInBuffer)kvi_free(m_pInBuffer);
	if(m_pDelayTimer)delete m_pDelayTimer;
	if(m_pDns)delete m_pDns;
	if(m_pSn)delete m_pSn;
	if(m_sock != KVI_INVALID_SOCKET)kvi_socket_close(m_sock);
	if(m_secondarySock != KVI_INVALID_SOCKET)kvi_socket_close(m_secondarySock);
KVSO_END_DESTRUCTOR(KviKvsObject_socket)
//----------------------


bool KviKvsObject_socket::functionStatus(KviKvsObjectFunctionCall *c)
{
	c->returnValue()->setInteger(m_iStatus);
	return true;
}

bool KviKvsObject_socket::functionClose(KviKvsObjectFunctionCall *c)
{
	if((m_pOutBuffer->size() != 0) && (m_iStatus == KVI_SCRIPT_SOCKET_STATUS_CONNECTED))
		tryFlush();

	reset();
	return true;
}

bool KviKvsObject_socket::functionConnectTimeout(KviKvsObjectFunctionCall *c)
{
	c->returnValue()->setInteger(m_uConnectTimeout);
	return true;
}

bool KviKvsObject_socket::functionSetConnectTimeout(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uTimeout;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("time_out",KVS_PT_UNSIGNEDINTEGER,0,uTimeout)
	KVSO_PARAMETERS_END(c)
	m_uConnectTimeout = uTimeout;
	return true;
}

bool KviKvsObject_socket::functionRemotePort(KviKvsObjectFunctionCall *c)
{
	c->returnValue()->setInteger(m_uRemotePort);
	return true;
}

bool KviKvsObject_socket::functionRemoteIp(KviKvsObjectFunctionCall *c)
{
	c->returnValue()->setString(m_szRemoteIp);
	return true;
}

bool KviKvsObject_socket::functionLocalPort(KviKvsObjectFunctionCall *c)
{
	c->returnValue()->setInteger(m_uLocalPort);
	return true;
}

bool KviKvsObject_socket::functionLocalIp(KviKvsObjectFunctionCall *c)
{
	c->returnValue()->setString(m_szLocalIp);
	return true;
}

bool KviKvsObject_socket::functionAccept(KviKvsObjectFunctionCall *c)
{
	KviKvsObject * pObject;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("object_socket",KVS_PT_HOBJECT,0,hObject)
	KVSO_PARAMETERS_END(c)

	pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObject);

	if(!pObject)
	{
		c->warning(__tr2qs("No socket object specified"));
		return true;
	}

	if(!pObject->inherits("KviKvsObject_socket"))
	{
		c->warning(__tr2qs("Invalid socket object specified (it doesn't inherit from socket)"));
		return true;
	}

	if(m_secondarySock != KVI_INVALID_SOCKET)
	{
		((KviKvsObject_socket *)pObject)->acceptConnection(m_secondarySock,m_uSecondaryPort,m_szSecondaryIp);

		m_secondarySock = KVI_INVALID_SOCKET;
		m_uSecondaryPort = 0;
		m_szSecondaryIp="";
	} else {
		c->warning(__tr2qs("There is no connection to accept!"));
	}
	return true;
}


bool  KviKvsObject_socket::functionSetProtocol(KviKvsObjectFunctionCall *c)
{
	QString m_szHex;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("hex_string",KVS_PT_STRING,0,m_szHex)
	KVSO_PARAMETERS_END(c)

	m_bUdp=KviQString::equalCI(m_szHex,"udp");
	return false;
}

bool  KviKvsObject_socket::functionWriteHex(KviKvsObjectFunctionCall *c)
{
	QString m_szHex;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("hex_string",KVS_PT_STRING,0,m_szHex)
	KVSO_PARAMETERS_END(c)

	if(m_szHex.length() > 0)
	{
		bool bOk;
		QString szTmp;
		szTmp=m_szHex.setNum(m_szHex.toInt(&bOk,16),16);
		if(szTmp.length()>0)
		{
			KviQCString szData8 = szTmp.utf8();
			m_pOutBuffer->append((const unsigned char *)(szData8.data()),szData8.length());
			delayedFlush(0);
			c->returnValue()->setInteger(szData8.length());
		}
	}

		/*
	if(pszData)
	{
		if(pszData->len() > 0)
		{
			char * ptr;
			len = pszData->hexToBuffer(&ptr);
			if(len > 0)
			{
				m_pOutBuffer->append((const unsigned char *)(ptr),len);
				delayedFlush(0);
				KviStr::freeBuffer(ptr);
			}
		}
	}
	buffer.append(KviStr::Format,"%d",len);
*/
  return true;
}





bool  KviKvsObject_socket::functionConnect(KviKvsObjectFunctionCall *c)
{
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("remote_ip",KVS_PT_STRING,0,m_szRemoteIp)
		KVSO_PARAMETER("remote_port",KVS_PT_UNSIGNEDINTEGER,0,m_uRemotePort)
	KVSO_PARAMETERS_END(c)
	debug ("Function connect");


	if (m_uRemotePort>65535)
	{
		c->warning(__tr2qs("Value %d for port is out of range (values allowed are from 0 to 65535"),m_uRemotePort);
		return true;
	}
	if((m_sock != KVI_INVALID_SOCKET) || (m_iStatus != KVI_SCRIPT_SOCKET_STATUS_DISCONNECTED))
	{
		c->warning(__tr2qs("Another connection in progress"));
		c->returnValue()->setBoolean(true);
		return true;
	}


#ifdef COMPILE_IPV6_SUPPORT
	if(kvi_isValidStringIp(m_szRemoteIp) || kvi_isValidStringIp_V6(m_szRemoteIp))
#else
	if(kvi_isValidStringIp(m_szRemoteIp))
#endif
	{
		debug ("ok connecting");
		debug ("connectinhg on ip %s ",m_szRemoteIp.latin1());
		debug ("non so ip");
		m_iStatus = KVI_SCRIPT_SOCKET_STATUS_CONNECTING;
		delayedConnect();
	} else {
		debug ("connectinhg on ip %s port %d",m_szRemoteIp.latin1(),m_uRemotePort);
		m_iStatus = KVI_SCRIPT_SOCKET_STATUS_DNS;
		delayedLookupRemoteIp();
	}
	c->returnValue()->setBoolean(true);
	return true;
}






bool KviKvsObject_socket::functionListen(KviKvsObjectFunctionCall *c)
{
	if((m_sock != KVI_INVALID_SOCKET) || (m_iStatus != KVI_SCRIPT_SOCKET_STATUS_DISCONNECTED))
	{
		c->warning(__tr2qs("Another connection in progress"));
		c->returnValue()->setBoolean(false);
		return true;
	}
	kvs_uint_t uLocalPort;
	QString m_szLocalIp;
	bool m_bIpV6;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("local_port",KVS_PT_UNSIGNEDINTEGER,0,uLocalPort)
		KVSO_PARAMETER("local_ip",KVS_PT_STRING,KVS_PF_OPTIONAL,m_szLocalIp)
		KVSO_PARAMETER("bool_ipV6",KVS_PT_BOOLEAN,KVS_PF_OPTIONAL,m_bIpV6)
	KVSO_PARAMETERS_END(c)
	m_uLocalPort=uLocalPort;

#ifndef COMPILE_IPV6_SUPPORT
	if(m_bIpV6)
	{
		c->warning(__tr2qs("No IPV6 support in this executable"));
		c->returnValue()->setBoolean(false);
		reset();
		return true;
	}
#endif

	bool bGotIp = false;

	if(!m_szLocalIp.isEmpty())
	{

		// Check the address type
		if(kvi_isValidStringIp(m_szLocalIp))bGotIp = true;
		else {
#ifdef COMPILE_IPV6_SUPPORT
			if(kvi_isValidStringIp_V6(m_szLocalIp))
			{
				bGotIp = true;
				m_bIpV6 = true;
			} else {
#else
				c->warning(__tr2qs("Invalid IP address specified ('%Q')"),&m_szLocalIp);
				c->returnValue()->setBoolean(false);
				reset();
				return true;
#endif
#ifdef COMPILE_IPV6_SUPPORT
			}
#endif
		}
	}


#ifdef COMPILE_IPV6_SUPPORT
	m_sock = kvi_socket_create(m_bIpV6 ? KVI_SOCKET_PF_INET6 : KVI_SOCKET_PF_INET,
								m_bUdp ? KVI_SOCKET_TYPE_DGRAM : KVI_SOCKET_TYPE_STREAM,KVI_SOCKET_PROTO_TCP);
#else
	m_sock = kvi_socket_create(KVI_SOCKET_PF_INET,m_bUdp ? KVI_SOCKET_TYPE_DGRAM : KVI_SOCKET_TYPE_STREAM,KVI_SOCKET_PROTO_TCP);
#endif

	if(m_sock == KVI_INVALID_SOCKET)
	{
		c->warning(__tr2qs("Socket creation failed"));
		c->returnValue()->setBoolean(false);
		reset();
		return true;
	}

	if(!m_szLocalIp.isEmpty())
	{
#ifdef COMPILE_IPV6_SUPPORT
		KviSockaddr sa(m_szLocalIp,m_uLocalPort,m_bIpV6,m_bUdp);
#else
		KviSockaddr sa(m_szLocalIp,m_uLocalPort,false,m_bUdp);
#endif
		if(!sa.socketAddress())
		{
			c->warning(__tr2qs("Invalid socket address"));
			reset();
			c->returnValue()->setBoolean(false);
			return true;
		}

		if(!kvi_socket_bind(m_sock,sa.socketAddress(),((int)(sa.addressLength()))))
		{
			c->warning(__tr2qs("Bind failure"));
			reset();
			c->returnValue()->setBoolean(false);
			return true;
		}
	} else {
#ifdef COMPILE_IPV6_SUPPORT
		KviSockaddr sa(m_uLocalPort,m_bIpV6,m_bUdp);
#else
		KviSockaddr sa(m_uLocalPort,false,m_bUdp);
#endif
		if(!sa.socketAddress())
		{
			c->warning(__tr2qs("Invalid socket address"));
			reset();
			c->returnValue()->setBoolean(false);
			return true;
		}

		if(!kvi_socket_bind(m_sock,sa.socketAddress(),((int)(sa.addressLength()))))
		{
			c->warning(__tr2qs("Bind failure"));
			reset();
			c->returnValue()->setBoolean(false);
			return true;
		}
	}


	if(!kvi_socket_listen(m_sock,5))
	{
		c->warning(__tr2qs("Listen failure"));
		reset();
		c->returnValue()->setBoolean(false);
		return true;
	}


	// Reread the port in case we're binding to a random one (0)

#ifdef COMPILE_IPV6_SUPPORT
	KviSockaddr sareal(0,m_bIpV6,m_bUdp);
#else
	KviSockaddr sareal(0,false,m_bUdp);
#endif

	int size = (int)sareal.addressLength();

	if(kvi_socket_getsockname(m_sock,sareal.socketAddress(),&size))
	{
		m_uLocalPort = sareal.port();
		sareal.getStringAddress(m_szLocalIp);
	}

	// and setup the READ notifier...
	m_pSn = new QSocketNotifier(m_sock,QSocketNotifier::Read);
	QObject::connect(m_pSn,SIGNAL(activated(int)),this,SLOT(incomingConnection(int)));
	m_pSn->setEnabled(true);

	m_iStatus = KVI_SCRIPT_SOCKET_STATUS_LISTENING;

	c->returnValue()->setBoolean(true);
	return true;
}

void KviKvsObject_socket::incomingConnection(int)
{
#ifdef COMPILE_IPV6_SUPPORT
	struct sockaddr_in6 hostSockAddr6;
#endif
	struct sockaddr_in  hostSockAddr;

	int size = sizeof(hostSockAddr);
	struct sockaddr * addr = (struct sockaddr *)&hostSockAddr;

#ifdef COMPILE_IPV6_SUPPORT
	if(m_bIpV6)
	{
		addr = (struct sockaddr *)&hostSockAddr6;
		size = sizeof(hostSockAddr6);
	}
#endif

	// Incoming connection
	m_secondarySock = kvi_socket_accept(m_sock,addr,&size);
	if(m_secondarySock != KVI_INVALID_SOCKET)
	{
		// Connected
#ifdef COMPILE_IPV6_SUPPORT
		if(m_bIpV6)
		{
			m_uSecondaryPort = ntohs(((struct sockaddr_in6 *)addr)->sin6_port);
			if(!kvi_binaryIpToStringIp_V6(((struct sockaddr_in6 *)addr)->sin6_addr,m_szSecondaryIp))
				m_szSecondaryIp = __tr("unknown");
		} else {
#endif
			m_uSecondaryPort = ntohs(((struct sockaddr_in *)addr)->sin_port);
			if(!kvi_binaryIpToStringIp(((struct sockaddr_in *)addr)->sin_addr,m_szSecondaryIp))
				m_szSecondaryIp = __tr("unknown");
#ifdef COMPILE_IPV6_SUPPORT
		}
#endif
		KviKvsVariantList *params=0;
		callFunction(this,"incomingConnectionEvent",params);
		if(m_secondarySock != KVI_INVALID_SOCKET)
		{
			kvi_socket_close(m_secondarySock);
			m_secondarySock = KVI_INVALID_SOCKET;
			m_uSecondaryPort = 0;
			m_szSecondaryIp="";
		}

	} // else..huh ?... wait for the next notifier call
}

void KviKvsObject_socket::acceptConnection(kvi_socket_t s,kvi_u32_t uPort,const char * szIp)
{
	reset();
	m_sock = s;
	m_uRemotePort = uPort;
	m_szRemoteIp = szIp;
#ifdef COMPILE_IPV6_SUPPORT
	KviSockaddr sareal(0,m_bIpV6,m_bUdp);
#else
	KviSockaddr sareal(0,false,m_bUdp);
#endif

	int size = (int)sareal.addressLength();

	if(kvi_socket_getsockname(m_sock,sareal.socketAddress(),&size))
	{
		m_uLocalPort = sareal.port();
		sareal.getStringAddress(m_szLocalIp);
	}

	m_iStatus = KVI_SCRIPT_SOCKET_STATUS_CONNECTED;

	m_pSn = new QSocketNotifier((int)m_sock,QSocketNotifier::Read);
	QObject::connect(m_pSn,SIGNAL(activated(int)),this,SLOT(readNotifierFired(int)));
	m_pSn->setEnabled(true);
}

void KviKvsObject_socket::delayedConnect()
{
	if(m_pDelayTimer)delete m_pDelayTimer;
	m_pDelayTimer = new QTimer();
	connect(m_pDelayTimer,SIGNAL(timeout()),this,SLOT(doConnect()));
	m_pDelayTimer->start(0,true);
}

void KviKvsObject_socket::doConnect()
{
	debug ("doConnect function");
	if(m_pDelayTimer)delete m_pDelayTimer;
	m_pDelayTimer = 0;


	KviSockaddr sa(m_szRemoteIp,m_uRemotePort,!kvi_isValidStringIp(m_szRemoteIp),m_bUdp);

	if(!sa.socketAddress())
	{
		unsigned int uOldConnectionId = m_uConnectionId;
		QString ipAddressError=__tr2qs("Invalid ip address ");

		ipAddressError.append(m_szRemoteIp);

		KviKvsVariantList params;
		QString tmp;
		KviQString::sprintf(tmp,__tr2qs("Invalid ip address (%Q)"),&m_szRemoteIp);
		params.append(new KviKvsVariant(tmp));
		callFunction(this,"connectFailedEvent",&params);
	/*	callEventFunction("connectFailedEvent",0,new KviParameterList(
			new KviStr(KviStr::Format,__tr("Invalid ip address (%s)"),m_szRemoteIp.ptr())));
	*/	if(m_uConnectionId == uOldConnectionId)reset();
		// else it has already been called!
		return;
	}
debug ("Socket created");

	// create the socket
#ifdef COMPILE_IPV6_SUPPORT
	m_bIpV6 = sa.isIpV6();
	m_sock = kvi_socket_create(sa.isIpV6() ? KVI_SOCKET_PF_INET6 : KVI_SOCKET_PF_INET,m_bUdp ? KVI_SOCKET_TYPE_DGRAM : KVI_SOCKET_TYPE_STREAM,KVI_SOCKET_PROTO_TCP);
#else
	m_bIpV6 = false;
	m_sock = kvi_socket_create(KVI_SOCKET_PF_INET,m_bUdp ? KVI_SOCKET_TYPE_DGRAM : KVI_SOCKET_TYPE_STREAM,KVI_SOCKET_PROTO_TCP);
#endif

	if(m_sock == KVI_INVALID_SOCKET)
	{
		unsigned int uOldConnectionId = m_uConnectionId;
		callFunction(this,"connectFailedEvent",new KviKvsVariantList(
			new KviKvsVariant(__tr2qs("Failed to create the socket"))));
		if(m_uConnectionId == uOldConnectionId)reset();
		// else it has already been called!
		return;
	}
	debug ("Valid socket");

	if(!kvi_socket_setNonBlocking(m_sock))
	{
		unsigned int uOldConnectionId = m_uConnectionId;

		callFunction(this,"connectFailedEvent",new KviKvsVariantList(
			new KviKvsVariant(__tr2qs("Failed to setup a nonblocking socket"))));
		if(m_uConnectionId == uOldConnectionId)reset();
		// else it has already been called!
		return;
	}

	if(!kvi_socket_connect(m_sock,sa.socketAddress(),((int)(sa.addressLength()))))
	{
		int err = kvi_socket_error();
		if(!kvi_socket_recoverableConnectError(err))
		{
			// Ops...
			int sockError=err;
			if(sockError==0)
			{
				// Zero error ?...let's look closer
				int iSize=sizeof(int);
				if(!kvi_socket_getsockopt(m_sock,SOL_SOCKET,SO_ERROR,
						(void *)&sockError,&iSize))sockError=0;
			}
			unsigned int uOldConnectionId = m_uConnectionId;
			QString callBackError=__tr2qs("Connect failure: ");

			callBackError.append((KviError::getDescription(KviError::translateSystemError(sockError)).utf8().data()));
			callFunction(this,"connectFailedEvent",new KviKvsVariantList(
			new KviKvsVariant(callBackError)));

/*			callEventFunction("connectFailedEvent",0,new KviParameterList(
				new KviStr(KviStr::Format,__tr("Connect failure: %s"),KviError::getDescription(KviError::translateSystemError(sockError)).utf8().data())));
*/			if(m_uConnectionId == uOldConnectionId)reset();
			// else it has already been called!
			return;
		}
	}
	debug ("Socket connected");
	m_pDelayTimer = new QTimer();
	connect(m_pDelayTimer,SIGNAL(timeout()),this,SLOT(connectTimeout()));
	m_pDelayTimer->start(m_uConnectTimeout,true);

	m_pSn = new QSocketNotifier((int)m_sock,QSocketNotifier::Write);
	QObject::connect(m_pSn,SIGNAL(activated(int)),this,SLOT(writeNotifierFired(int)));
	m_pSn->setEnabled(true);
}

void KviKvsObject_socket::connectTimeout()
{
	unsigned int uOldConnectionId = m_uConnectionId;

	callFunction(this,"connectFailedEvent",new KviKvsVariantList(
				new KviKvsVariant(__tr2qs("Connect attempt timed out"))));
	if(m_uConnectionId == uOldConnectionId)reset();
	// else it has already been called!
}

void KviKvsObject_socket::delayedLookupRemoteIp()
{
	if(m_pDelayTimer)delete m_pDelayTimer;
	m_pDelayTimer = new QTimer();
	connect(m_pDelayTimer,SIGNAL(timeout()),this,SLOT(lookupRemoteIp()));
	m_pDelayTimer->start(0,true);
}

void KviKvsObject_socket::lookupRemoteIp()
{
	debug ("Resolve dns");
	if(m_pDelayTimer)delete m_pDelayTimer;
	m_pDelayTimer = 0;
	if(m_pDns)delete m_pDns;
	m_pDns = new KviDns();
	connect(m_pDns,SIGNAL(lookupDone(KviDns *)),this,SLOT(lookupDone(KviDns *)));
	if(!m_pDns->lookup(m_szRemoteIp,KviDns::Any))
	{
		unsigned int uOldConnectionId = m_uConnectionId;

		callFunction(this,"connectFailedEvent",new KviKvsVariantList(
				new KviKvsVariant(__tr2qs("Can't start the DNS thread"))));
		if(m_uConnectionId == uOldConnectionId)reset();
		// else it has already been called!
	}
}

void KviKvsObject_socket::lookupDone(KviDns *pDns)
{

	if(pDns->state() != KviDns::Success)
	{
		unsigned int uOldConnectionId = m_uConnectionId;

		callFunction(this,"connectFailedEvent",new KviKvsVariantList(
			new KviKvsVariant(KviError::getDescription(pDns->error()))));

		if(m_uConnectionId == uOldConnectionId)reset();
		// else it has already been called!
		return;
	}
	m_szRemoteIp = pDns->firstIpAddress();
	debug ("Dns resolved in %s",m_szRemoteIp.latin1());

	delete m_pDns;
	m_pDns = 0;
	doConnect();
}

void KviKvsObject_socket::writeNotifierFired(int)
{
	debug ("Here in the writeNotifierFired");
	if(m_pSn)
	{
		delete m_pSn;
		m_pSn = 0;
	}
	if(m_pDelayTimer)
	{
		delete m_pDelayTimer;
		m_pDelayTimer = 0;
	}

	// Check for errors...
	int sockError;
	int iSize=sizeof(int);
	if(!kvi_socket_getsockopt(m_sock,SOL_SOCKET,SO_ERROR,(void *)&sockError,&iSize))sockError = -1;
	//sockError = 0;
	if(sockError != 0)
	{
		//debug("Failed here %d",sockError);
		//failed
		if(sockError > 0)sockError = KviError::translateSystemError(sockError);
		else sockError = KviError_unknownError; //Error 0 ?
		unsigned int uOldConnectionId = m_uConnectionId;
		callFunction(this,"connectFailedEvent",new KviKvsVariantList(
			new KviKvsVariant(KviError::getDescription(sockError))));
		if(m_uConnectionId == uOldConnectionId)reset();
		// else it has already been called!
	} else {
		// Succesfully connected
		// create the correct read notifier now...
		m_pSn = new QSocketNotifier((int)m_sock,QSocketNotifier::Read);
		QObject::connect(m_pSn,SIGNAL(activated(int)),this,SLOT(readNotifierFired(int)));
		m_pSn->setEnabled(true);

#ifdef COMPILE_IPV6_SUPPORT
		KviSockaddr sareal(0,m_bIpV6,m_bUdp);
#else
		KviSockaddr sareal(0,false,m_bUdp);
#endif
		int size = (int)sareal.addressLength();
		if(kvi_socket_getsockname(m_sock,sareal.socketAddress(),&size))
		{
			m_uLocalPort = sareal.port();
			sareal.getStringAddress(m_szLocalIp);
		}
		unsigned int uOldConnectionId = m_uConnectionId;
		KviKvsVariantList *params=0;
		callFunction(this,"connectEvent",params);
		if(m_uConnectionId == uOldConnectionId)
			m_iStatus = KVI_SCRIPT_SOCKET_STATUS_CONNECTED;
		// else the connection has been already changed!
	}
}

void KviKvsObject_socket::readNotifierFired(int)
{
	debug ("here in the readNotifierFired");
	//read data
	if((m_uInBufferLen - m_uInDataLen) < KVI_READ_CHUNK)
	{
		m_uInBufferLen += KVI_IN_BUFFER_ALLOC_CHUNK;
		m_pInBuffer = (char *)kvi_realloc(m_pInBuffer,m_uInBufferLen);
	}

	int readLength = kvi_socket_recv(m_sock,m_pInBuffer + m_uInDataLen,KVI_READ_CHUNK);

	if(readLength <= 0)
	{
		if(readLength==0)
		{
			unsigned int uOldConnectionId = m_uConnectionId;
			KviKvsVariantList *params=0;
			callFunction(this,"disconnectEvent",params);
			if(m_uConnectionId == uOldConnectionId)reset();
			// else it has already been called!
			return;
		} else {
			//check for transmission errors
			int err = kvi_socket_error();
#ifdef COMPILE_ON_WINDOWS
			if((err != EAGAIN) && (err != EINTR) && (err != WSAEWOULDBLOCK))
#else
			if((err != EAGAIN) && (err != EINTR))
#endif
			{
				unsigned int uOldConnectionId = m_uConnectionId;
				if(err > 0)

				{
					//	QString error=KviError::translateSystemError(err);
						callFunction(this,"disconnectEvent",new KviKvsVariantList(
						new KviKvsVariant((kvs_int_t)KviError::translateSystemError(err))));
				} else {
						callFunction(this,"disconnectEvent",new KviKvsVariantList(
						new KviKvsVariant(KviError::getDescription(KviError_remoteEndClosedConnection))));

				}
				if(m_uConnectionId == uOldConnectionId)reset();
				// else it has already been called!
				return;
			} //else transient error...wait again...
		}
		return; // can do nothing
	}
	// readLength > 0
	m_uInDataLen += readLength;

	//KviStr * s = new KviStr();
	QString s;
	s.setNum(m_uInDataLen);


	unsigned int uOldConnectionId = m_uConnectionId;
	callFunction(this,"dataAvailableEvent",new KviKvsVariantList(new KviKvsVariant(s)));
	if(m_uConnectionId == uOldConnectionId)
	{
		if(m_uInDataLen > (4096 * 1024)) // too much data in buffer (not reading)
		{
			callFunction(this,"disconnectEvent",new KviKvsVariantList(
				new KviKvsVariant(__tr2qs("Too much unprocessed incoming data (you've left this socket unmanaged ?)"))));
			reset();
		}
	}
}

void KviKvsObject_socket::eatInData(unsigned int uLen)
{
	if(uLen > m_uInDataLen)uLen = m_uInDataLen;

	m_uInDataLen -= uLen;

	if(m_uInDataLen > 0)
	{
		kvi_memmove(m_pInBuffer,m_pInBuffer + uLen,m_uInDataLen);
	}

	unsigned int uSpace = m_uInBufferLen - m_uInDataLen;

	if(uSpace > KVI_IN_BUFFER_ALLOC_CHUNK)
	{
		m_uInBufferLen -= KVI_IN_BUFFER_ALLOC_CHUNK;
		m_pInBuffer = (char *)kvi_realloc(m_pInBuffer,m_uInBufferLen);
	}
}

void KviKvsObject_socket::delayedFlush(unsigned int uTimeout)
{
	if(m_pFlushTimer->isActive())m_pFlushTimer->stop();
	m_pFlushTimer->start(uTimeout);
}

void KviKvsObject_socket::tryFlush()
{
	if(m_pFlushTimer->isActive())m_pFlushTimer->stop();

	if(m_pOutBuffer->size() == 0)
	{
		return;
	}

	int result = kvi_socket_send(m_sock,m_pOutBuffer->data(),m_pOutBuffer->size());

	if(result >= 0)
	{
		if(result == m_pOutBuffer->size())
		{
			m_pOutBuffer->clear();
		} else {
			if(result > 0)m_pOutBuffer->remove(result);
			delayedFlush(500);
		}
	} else {
		// Oops...error ?
		int err = kvi_socket_error();
#ifdef COMPILE_ON_WINDOWS
		if((err == EAGAIN) || (err == EINTR) || (err = WSAEWOULDBLOCK))
#else
		if((err == EAGAIN)||(err == EINTR))
#endif
		{
			// Transient error...partial send as before...
			// Async continue...
			delayedFlush(500);
			return;
		} else {
			// Disconnected... :(
			unsigned int uOldConnectionId = m_uConnectionId;
			callFunction(this,"disconnectEvent",0,new KviKvsVariantList(
				new KviKvsVariant(KviError::getDescription(KviError::translateSystemError(err)))));
			if(m_uConnectionId == uOldConnectionId)reset();
			// else it has already been called!
			return;
		}
	}
}
void KviKvsObject_socket::reset()
{
	m_uConnectionId++; // this is the only place where this is incremented!
	if(m_pDelayTimer)
	{
		delete m_pDelayTimer;
		m_pDelayTimer = 0;
	}
	if(m_pDns)
	{
		delete m_pDns;
		m_pDns = 0;
	}
	if(m_pSn)
	{
		delete m_pSn;
		m_pSn = 0;
	}
	if(m_sock != KVI_INVALID_SOCKET)
	{
		kvi_socket_close(m_sock);
		m_sock = KVI_INVALID_SOCKET;
	}
	if(m_pInBuffer)
	{
		kvi_free(m_pInBuffer);
		m_pInBuffer = 0;
	}
	if(m_secondarySock)
	{
		kvi_socket_close(m_secondarySock);
		m_secondarySock = KVI_INVALID_SOCKET;
	}
	m_uSecondaryPort = 0;
	m_szSecondaryIp="";
	delete m_pOutBuffer;
	if(m_pFlushTimer->isActive())m_pFlushTimer->stop();
	m_pOutBuffer = new KviDataBuffer();
	m_uInBufferLen = 0;
	m_uInDataLen = 0;
	m_iStatus = KVI_SCRIPT_SOCKET_STATUS_DISCONNECTED;
	m_uRemotePort = 0;
	m_szRemoteIp="";
	m_uLocalPort = 0;
	m_szLocalIp="";
	m_bIpV6 = false;
}
unsigned int KviKvsObject_socket::readGetLength(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uLen;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("length",KVS_PT_UNSIGNEDINTEGER,KVS_PF_OPTIONAL,uLen)
	KVSO_PARAMETERS_END(c)
	if (uLen>m_uInDataLen || !uLen) return m_uInDataLen;
	else return uLen;
	}

bool KviKvsObject_socket::functionRead(KviKvsObjectFunctionCall *c)
{
	unsigned int uLen = readGetLength(c);

	if(uLen > 0)
	{
		// convert NULLS to char 255
		for(unsigned int i = 0;i < uLen;i++)
		{
			if(!m_pInBuffer[i])m_pInBuffer[i] = (char)(255);
		}
		QString tmpBuffer = QString::fromUtf8(m_pInBuffer,uLen);
		c->returnValue()->setString(tmpBuffer);

		eatInData(uLen);
	}
	return true;
}

bool KviKvsObject_socket::functionReadHex(KviKvsObjectFunctionCall *c)
{
	//??? funzioner�
	unsigned int uLen = readGetLength(c);
	bool bOk;
	if(uLen > 0)
	{
		QString szTmp;
		szTmp =m_pInBuffer;
		szTmp.toInt(&bOk,16);
		c->returnValue()->setString(szTmp);
		eatInData(uLen);
	}
	return true;
}

bool KviKvsObject_socket::functionWrite(KviKvsObjectFunctionCall *c)
{
 QString szData;
 KVSO_PARAMETERS_BEGIN(c)
  KVSO_PARAMETER("szData",KVS_PT_STRING,0,szData)
 KVSO_PARAMETERS_END(c)

KviQCString szData8 = szData.utf8();
 if(szData8.length() > 0)
 {
  m_pOutBuffer->append((const unsigned char*)szData8.data(),szData8.length());
  delayedFlush(0);
 }
 return true;
}



//--------------

#include "m_class_socket.moc"
