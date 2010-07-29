//=============================================================================
//
//   File : class_socket.cpp
//   Creation date : Sun Nov 11 03:13:45 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "class_socket.h"
#include "class_memorybuffer.h"
#include "class_file.h"

#include "kvi_settings.h"
#include "kvi_qstring.h"

#define _KVI_DEBUG_CHECK_RANGE_
#include "kvi_debug.h"

#include "kvi_file.h"
#include "kvi_fileutils.h"
#include "kvi_locale.h"
#include "kvi_error.h"
#include "kvi_netutils.h"
#include "kvi_dns.h"
#include "kvi_error.h"
#include "kvi_socket.h"
#include "kvi_malloc.h"
#include "kvi_memmove.h"
#include "kvi_databuffer.h"

#include <QByteArray>

//#include <stdlib.h>

#define KVI_IN_BUFFER_ALLOC_CHUNK 4096
#define KVI_READ_CHUNK 1024

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
		This function returns 1 if the connect attempt can be successfully initiated,
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
		This function is called when a connection attempt has been successfully completed.
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

                !fn: $read(<length>[,<hobject>])
		Reads at most <length> bytes of data from the socket. If <length> is anything "outside" the
		available data range (<length> < 0 or <length> > available_data_length), this function
		returns all the available data.[br]
                By default this function can deal ascii data only: NULL characters are transformed to
                ASCII characters 255. You can pass a [class]memorybuffer[/class] object to read binary data.


                !fn: $write(<data, files or hobject>[,length])
		Writes <data> to the socket.[br]
                This function can deal with binary data passing  a [class]memorybuffer[/class] object[br]
		Please note that when this function finishes it does not mean that the data has reached the remote end.[br]
		Basically it does not even mean that the data has been sent to the remote host.[br]
		The data is enqueued for sending and will be sent as soon as possible.[br]
		If you're going to [cmd]delete[/cmd] this object just after the $write call, you should
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
                Let KVIrc use TCP or UDP protocol

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
		// Server socket: listen 8080 port and answer to requests (multi-threaded)
		class (webserver,socket)
		{
			constructor ()
			{
				@$listen(8080, "127.0.0.1")
			}
			incomingConnectionEvent()
			{
				%tmp = $new(socket)
				@$accept(%tmp)
				echo "Webserver incoming Conection from: %tmp->$remoteIp : %tmp->$remotePort"
				%tmp->$write("HTTP/1.1 200 OK\n\n<html><head></head><body><h1>KVIrc Webserver</h1></body></html>\n")
				%tmp->$close()
				delete %tmp
			}
		}
		// finally start webserver
		%WebS = $new(webserver)
		[/example]
		[example]
		// Client socket - go to google and grab request header[br]
		class (httprequest,socket)
		{
			constructor ()
			{
				// connect to the server
				@$connect("www.google.com",80)
			}
			destructor()
			{
				// if the socket is still open close it
				if(@$status() == 4) @$close()
			}
			connectFailedEvent()
			{
				// the connection to the server failed
				debug  "Connection failed: "$0
				delete $$
			}
			connectEvent()
			{
				// connection is complete
				// send a request to receive the headers only from http://www.google.com/
				@$write("HEAD / HTTP/1.1\r\nHost: www.google.de\r\nConnction: Close\r\nUser-Agent: KVIrc socket\r\n\r\n");
			}
			dataAvailableEvent()
			{
				// reading the received data
				%newdata  = @$read($0)
				echo %newdata
				#close and delete the socket
				@$close()
				delete $$
			}
			disconnectEvent()
			{
				// connection has been closed
				echo "Connection closed"
				delete $$
			}
		}
		%Temp = $new(httprequest)
		[/example]
*/

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_socket,"socket","object")

	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_socket,status)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_socket,remotePort)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_socket,remoteIp)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_socket,localIp)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_socket,localPort)
	KVSO_REGISTER_HANDLER(KviKvsObject_socket,"connect",functionConnect)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_socket,connectTimeout)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_socket,setConnectTimeout)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_socket,close)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_socket,read)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_socket,write)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_socket,setProtocol)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_socket,listen)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_socket,accept)

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
	m_bIPv6 = false;
	m_uLocalPort = 0;
	m_secondarySock = KVI_INVALID_SOCKET;
	connect(m_pFlushTimer,SIGNAL(timeout()),this,SLOT(tryFlush()));
KVSO_END_CONSTRUCTOR(KviKvsObject_socket)

KVSO_BEGIN_DESTRUCTOR(KviKvsObject_socket)
	delete m_pOutBuffer;
	delete m_pFlushTimer;
	m_pOutBuffer=0;
	m_pFlushTimer=0;


	if(m_pInBuffer)kvi_free(m_pInBuffer);
	if(m_pDelayTimer)delete m_pDelayTimer;
	if(m_pDns)delete m_pDns;
	if(m_pSn)delete m_pSn;
	if(m_sock != KVI_INVALID_SOCKET)kvi_socket_close(m_sock);
	if(m_secondarySock != KVI_INVALID_SOCKET)kvi_socket_close(m_secondarySock);
KVSO_END_DESTRUCTOR(KviKvsObject_socket)
//----------------------


KVSO_CLASS_FUNCTION(socket,status)
{
        c->returnValue()->setInteger(m_iStatus);
	return true;
}

KVSO_CLASS_FUNCTION(socket,close)
{
	Q_UNUSED(c);

	if (!m_pOutBuffer) return true;
	if((m_pOutBuffer->size() != 0) && (m_iStatus == KVI_SCRIPT_SOCKET_STATUS_CONNECTED))
		tryFlush();

	reset();
	return true;
}

KVSO_CLASS_FUNCTION(socket,connectTimeout)
{
	c->returnValue()->setInteger(m_uConnectTimeout);
	return true;
}

KVSO_CLASS_FUNCTION(socket,setConnectTimeout)
{
	kvs_uint_t uTimeout;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("time_out",KVS_PT_UNSIGNEDINTEGER,0,uTimeout)
	KVSO_PARAMETERS_END(c)
	m_uConnectTimeout = uTimeout;
	return true;
}

KVSO_CLASS_FUNCTION(socket,remotePort)
{
	c->returnValue()->setInteger(m_uRemotePort);
	return true;
}

KVSO_CLASS_FUNCTION(socket,remoteIp)
{
	c->returnValue()->setString(m_szRemoteIp);
	return true;
}

KVSO_CLASS_FUNCTION(socket,localPort)
{
	c->returnValue()->setInteger(m_uLocalPort);
	return true;
}

KVSO_CLASS_FUNCTION(socket,localIp)
{
	c->returnValue()->setString(m_szLocalIp);
	return true;
}

unsigned int KviKvsObject_socket::readGetLength(KviKvsObjectFunctionCall * c)
{
	kvs_uint_t uLen;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("length",KVS_PT_UNSIGNEDINTEGER,KVS_PF_OPTIONAL,uLen)
	KVSO_PARAMETERS_END(c)
	if (uLen>m_uInDataLen || !uLen) return m_uInDataLen;
	else return uLen;
}

KVSO_CLASS_FUNCTION(socket,read)
{
	kvs_uint_t uLen;
	KviKvsObject * pObject;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("length",KVS_PT_UNSIGNEDINTEGER,KVS_PF_OPTIONAL,uLen)
		KVSO_PARAMETER("hobject",KVS_PT_HOBJECT,KVS_PF_OPTIONAL,hObject)
	KVSO_PARAMETERS_END(c)
	if (uLen>m_uInDataLen || !uLen) uLen=m_uInDataLen;
	if (hObject)
	{
		pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
		if (!pObject)
		{
			c->warning(__tr2qs_ctx("Buffer parameter is not an object","objects"));
			return true;
		}
		if (pObject->inheritsClass("memorybuffer"))
		{
			QByteArray *pBuffer=((KviKvsObject_memorybuffer *)pObject)->pBuffer();
			int oldsize=pBuffer->size();
			pBuffer->resize(oldsize+uLen);
			kvi_memmove(pBuffer->data()+oldsize,m_pInBuffer,uLen);

		}
		else if (pObject->inheritsClass("file"))
		{
			KviFile *pFile=((KviKvsObject_file *)pObject)->file();
			if (!pFile->isOpen())
			{
				c->warning(__tr2qs_ctx("File is not open!","objects"));
				return true;
			}
			pFile->write(m_pInBuffer,uLen);
		}
		else
		{
			c->warning(__tr2qs_ctx("Buffer parameter is not a memorybuffer object","objects"));
			return true;
		}
		eatInData(uLen);
		return true;
	}
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



KVSO_CLASS_FUNCTION(socket,write)
{
	kvs_uint_t uLen;
	KviKvsObject * pObject;
	KviKvsVariant * pVariantData;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
                        KVSO_PARAMETER("data_or_file_or_memorybuffer",KVS_PT_VARIANT,0,pVariantData)
			KVSO_PARAMETER("length",KVS_PT_UNSIGNEDINTEGER,KVS_PF_OPTIONAL,uLen)
	KVSO_PARAMETERS_END(c)
	if (pVariantData->isHObject())
	{
		pVariantData->asHObject(hObject);
		pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
		if (!pObject)
		{
			c->warning(__tr2qs_ctx("Buffer parameter is not an object","objects"));
			return true;
		}
		if (pObject->inheritsClass("memorybuffer"))
		{
			QByteArray *p=((KviKvsObject_memorybuffer *)pObject)->pBuffer();
			m_pOutBuffer->append((const unsigned char*)p->data(),p->size());
		}
		else if (pObject->inheritsClass("file"))
		{
			KviFile *pFile=((KviKvsObject_file *)pObject)->file();
			if (!pFile->isOpen())
			{
				c->warning(__tr2qs_ctx("File is not open!","objects"));
				return true;
			}
			if (!uLen) uLen=pFile->size();
                        kvs_int_t size=pFile->size();
			pFile->flush();
			m_pOutBuffer->append((const unsigned char*)pFile->read(uLen).data(),uLen);
                        c->returnValue()->setBoolean((size-pFile->pos()==0));
		}
		else
		{
			c->warning(__tr2qs_ctx("Buffer parameter is not a memorybuffer or file object","objects"));
			return true;
		}

	}
	else
	{
		QString szData;
		pVariantData->asString(szData);
		if(!KviFileUtils::fileExists(szData))
		{
			QByteArray szData8 = szData.toUtf8();
			if(szData8.length() > 0)
			{
                                        debug("write on socket %s",szData8.data());
					m_pOutBuffer->append((const unsigned char*)szData8.data(),szData8.length());
			}
		}
		else
		{
			KviFile f(szData);
			f.open(QIODevice::ReadOnly);
			QByteArray ar= f.readAll();
			m_pOutBuffer->append((const unsigned char*)ar.data(),ar.size());
			f.close();
		}
	}
	delayedFlush(0);
	return true;
}


KVSO_CLASS_FUNCTION(socket,accept)
{
	KviKvsObject * pObject;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("object_socket",KVS_PT_HOBJECT,0,hObject)
	KVSO_PARAMETERS_END(c)

	pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObject);

	if(!pObject)
	{
		c->warning(__tr2qs_ctx("No socket object specified","objects"));
		return true;
	}

	if(!pObject->inheritsClass("socket"))
	{
		c->warning(__tr2qs_ctx("Invalid socket object specified (it doesn't inherit from socket)","objects"));
		return true;
	}

	if(m_secondarySock != KVI_INVALID_SOCKET)
	{
		((KviKvsObject_socket *)pObject)->acceptConnection(m_secondarySock,m_uSecondaryPort,m_szSecondaryIp.toUtf8().data());

		m_secondarySock = KVI_INVALID_SOCKET;
		m_uSecondaryPort = 0;
		m_szSecondaryIp="";
	} else {
		c->warning(__tr2qs_ctx("There is no connection to accept!","objects"));
	}
	return true;
}


KVSO_CLASS_FUNCTION(socket,setProtocol)
{
	QString m_szHex;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("hex_string",KVS_PT_STRING,0,m_szHex)
	KVSO_PARAMETERS_END(c)

	m_bUdp=KviQString::equalCI(m_szHex,"udp");
	return false;
}



KVSO_CLASS_FUNCTION(socket,functionConnect)
{
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("remote_ip",KVS_PT_STRING,0,m_szRemoteIp)
		KVSO_PARAMETER("remote_port",KVS_PT_UNSIGNEDINTEGER,0,m_uRemotePort)
	KVSO_PARAMETERS_END(c)
	debug ("Function connect");


	if (m_uRemotePort>65535)
	{
		c->warning(__tr2qs_ctx("Value %d for port is out of range (values allowed are from 0 to 65535)","objects"),m_uRemotePort);
		return true;
	}
	if((m_sock != KVI_INVALID_SOCKET) || (m_iStatus != KVI_SCRIPT_SOCKET_STATUS_DISCONNECTED))
	{
		c->warning(__tr2qs_ctx("Another connection in progress","objects"));
		c->returnValue()->setBoolean(true);
		return true;
	}


#ifdef COMPILE_IPV6_SUPPORT
	if(KviNetUtils::isValidStringIp(m_szRemoteIp) || KviNetUtils::isValidStringIPv6(m_szRemoteIp))
#else
	if(KviNetUtils::isValidStringIp(m_szRemoteIp))
#endif
	{
		debug ("ok connecting");
		debug ("connectinhg on ip %s ",m_szRemoteIp.toUtf8().data());
		debug ("non so ip");
		m_iStatus = KVI_SCRIPT_SOCKET_STATUS_CONNECTING;
		delayedConnect();
	} else {
		debug ("connectinhg on ip %s port %d",m_szRemoteIp.toUtf8().data(),(int) m_uRemotePort);
		m_iStatus = KVI_SCRIPT_SOCKET_STATUS_DNS;
		delayedLookupRemoteIp();
	}
	c->returnValue()->setBoolean(true);
	return true;
}


KVSO_CLASS_FUNCTION(socket,listen)
{
	if((m_sock != KVI_INVALID_SOCKET) || (m_iStatus != KVI_SCRIPT_SOCKET_STATUS_DISCONNECTED))
	{
		c->warning(__tr2qs_ctx("Another connection in progress","objects"));
		c->returnValue()->setBoolean(false);
		return true;
	}
	kvs_uint_t uLocalPort;
	QString m_szLocalIp;
	bool m_bIPv6;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("local_port",KVS_PT_UNSIGNEDINTEGER,0,uLocalPort)
		KVSO_PARAMETER("local_ip",KVS_PT_STRING,KVS_PF_OPTIONAL,m_szLocalIp)
		KVSO_PARAMETER("bool_ipV6",KVS_PT_BOOLEAN,KVS_PF_OPTIONAL,m_bIPv6)
	KVSO_PARAMETERS_END(c)
	m_uLocalPort=uLocalPort;

#ifndef COMPILE_IPV6_SUPPORT
	if(m_bIPv6)
	{
		c->warning(__tr2qs_ctx("No IPv6 support in this executable","objects"));
		c->returnValue()->setBoolean(false);
		reset();
		return true;
	}
#endif

	bool bGotIp = false;

	if(!m_szLocalIp.isEmpty())
	{

		// Check the address type
		if(KviNetUtils::isValidStringIp(m_szLocalIp))bGotIp = true;
		else {
#ifdef COMPILE_IPV6_SUPPORT
			if(KviNetUtils::isValidStringIPv6(m_szLocalIp))
			{
				bGotIp = true;
				m_bIPv6 = true;
			} else {
#else
				c->warning(__tr2qs_ctx("Invalid IP address (%Q)","objects"),&m_szLocalIp);
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
	m_sock = kvi_socket_create(m_bIPv6 ? KVI_SOCKET_PF_INET6 : KVI_SOCKET_PF_INET,
								m_bUdp ? KVI_SOCKET_TYPE_DGRAM : KVI_SOCKET_TYPE_STREAM,KVI_SOCKET_PROTO_TCP);
#else
	m_sock = kvi_socket_create(KVI_SOCKET_PF_INET,m_bUdp ? KVI_SOCKET_TYPE_DGRAM : KVI_SOCKET_TYPE_STREAM,KVI_SOCKET_PROTO_TCP);
#endif

	if(m_sock == KVI_INVALID_SOCKET)
	{
		c->warning(__tr2qs_ctx("Socket creation failed","objects"));
		c->returnValue()->setBoolean(false);
		reset();
		return true;
	}

	if(!m_szLocalIp.isEmpty())
	{
#ifdef COMPILE_IPV6_SUPPORT
		KviSockaddr sa(m_szLocalIp.toUtf8().data(),m_uLocalPort,m_bIPv6,m_bUdp);
#else
		KviSockaddr sa(m_szLocalIp.toUtf8().data(),m_uLocalPort,false,m_bUdp);
#endif
		if(!sa.socketAddress())
		{
			c->warning(__tr2qs_ctx("Invalid socket address","objects"));
			reset();
			c->returnValue()->setBoolean(false);
			return true;
		}

		if(!kvi_socket_bind(m_sock,sa.socketAddress(),((int)(sa.addressLength()))))
		{
			c->warning(__tr2qs_ctx("Bind failure","objects"));
			reset();
			c->returnValue()->setBoolean(false);
			return true;
		}
	} else {
#ifdef COMPILE_IPV6_SUPPORT
		KviSockaddr sa(m_uLocalPort,m_bIPv6,m_bUdp);
#else
		KviSockaddr sa(m_uLocalPort,false,m_bUdp);
#endif
		if(!sa.socketAddress())
		{
			c->warning(__tr2qs_ctx("Invalid socket address","objects"));
			reset();
			c->returnValue()->setBoolean(false);
			return true;
		}

		if(!kvi_socket_bind(m_sock,sa.socketAddress(),((int)(sa.addressLength()))))
		{
			c->warning(__tr2qs_ctx("Bind failure","objects"));
			reset();
			c->returnValue()->setBoolean(false);
			return true;
		}
	}


	if(!kvi_socket_listen(m_sock,5))
	{
		c->warning(__tr2qs_ctx("Listen failure","objects"));
		reset();
		c->returnValue()->setBoolean(false);
		return true;
	}


	// Reread the port in case we're binding to a random one (0)

#ifdef COMPILE_IPV6_SUPPORT
	KviSockaddr sareal(0,m_bIPv6,m_bUdp);
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
	if(m_bIPv6)
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
		if(m_bIPv6)
		{
			m_uSecondaryPort = ntohs(((struct sockaddr_in6 *)addr)->sin6_port);
			if(!KviNetUtils::binaryIpToStringIp_V6(((struct sockaddr_in6 *)addr)->sin6_addr,m_szSecondaryIp))
				m_szSecondaryIp = __tr("unknown");
		} else {
#endif
			m_uSecondaryPort = ntohs(((struct sockaddr_in *)addr)->sin_port);
			if(!KviNetUtils::binaryIpToStringIp(((struct sockaddr_in *)addr)->sin_addr,m_szSecondaryIp))
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
	KviSockaddr sareal(0,m_bIPv6,m_bUdp);
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
	m_pDelayTimer->setInterval(0);
	m_pDelayTimer->setSingleShot(true);
	m_pDelayTimer->start();
}

void KviKvsObject_socket::doConnect()
{
	debug ("doConnect function");
	if(m_pDelayTimer)delete m_pDelayTimer;
	m_pDelayTimer = 0;


	KviSockaddr sa(m_szRemoteIp.toUtf8().data(),m_uRemotePort,!KviNetUtils::isValidStringIp(m_szRemoteIp),m_bUdp);

	if(!sa.socketAddress())
	{
		unsigned int uOldConnectionId = m_uConnectionId;
		QString ipAddressError=__tr2qs_ctx("Invalid IP address ","objects");

		ipAddressError.append(m_szRemoteIp);

		KviKvsVariantList params;
		QString tmp;
		KviQString::sprintf(tmp,__tr2qs_ctx("Invalid IP address (%Q)","objects"),&m_szRemoteIp);
		params.append(new KviKvsVariant(tmp));
		callFunction(this,"connectFailedEvent",&params);
		if(m_uConnectionId == uOldConnectionId)reset();
		// else it has already been called!
		return;
	}
debug ("Socket created");

	// create the socket
#ifdef COMPILE_IPV6_SUPPORT
	m_bIPv6 = sa.isIPv6();
	m_sock = kvi_socket_create(sa.isIPv6() ? KVI_SOCKET_PF_INET6 : KVI_SOCKET_PF_INET,m_bUdp ? KVI_SOCKET_TYPE_DGRAM : KVI_SOCKET_TYPE_STREAM,KVI_SOCKET_PROTO_TCP);
#else
	m_bIPv6 = false;
	m_sock = kvi_socket_create(KVI_SOCKET_PF_INET,m_bUdp ? KVI_SOCKET_TYPE_DGRAM : KVI_SOCKET_TYPE_STREAM,KVI_SOCKET_PROTO_TCP);
#endif

	if(m_sock == KVI_INVALID_SOCKET)
	{
		unsigned int uOldConnectionId = m_uConnectionId;
		KviKvsVariantList lParams;
		lParams.append(new KviKvsVariant(__tr2qs_ctx("Failed to create the socket","objects")));
		callFunction(this,"connectFailedEvent",&lParams);
		if(m_uConnectionId == uOldConnectionId)reset();
		// else it has already been called!
		return;
	}
	debug ("Valid socket");

	if(!kvi_socket_setNonBlocking(m_sock))
	{
		unsigned int uOldConnectionId = m_uConnectionId;
		KviKvsVariantList lParams;
		lParams.append(new KviKvsVariant(__tr2qs_ctx("Failed to setup a nonblocking socket","objects")));
		callFunction(this,"connectFailedEvent",&lParams);
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
			QString callBackError=__tr2qs_ctx("Connect failure: ","objects");

			callBackError.append((KviError::getDescription(KviError::translateSystemError(sockError)).toUtf8().data()));
			KviKvsVariantList lParams;
			lParams.append(new KviKvsVariant(callBackError));
			callFunction(this,"connectFailedEvent",&lParams);
			if(m_uConnectionId == uOldConnectionId)reset();
			// else it has already been called!
			return;
		}
	}
	debug ("Socket connected");
	m_pDelayTimer = new QTimer();
	connect(m_pDelayTimer,SIGNAL(timeout()),this,SLOT(connectTimeoutSlot()));
	m_pDelayTimer->setInterval(m_uConnectTimeout);
	m_pDelayTimer->setSingleShot(true);
	m_pDelayTimer->start();

	m_pSn = new QSocketNotifier((int)m_sock,QSocketNotifier::Write);
	QObject::connect(m_pSn,SIGNAL(activated(int)),this,SLOT(writeNotifierFired(int)));
	m_pSn->setEnabled(true);
}

void KviKvsObject_socket::connectTimeoutSlot()
{
	unsigned int uOldConnectionId = m_uConnectionId;
	KviKvsVariantList lParams;
	lParams.append(new KviKvsVariant(__tr2qs_ctx("Connect attempt timed out","objects")));
	callFunction(this,"connectFailedEvent",&lParams);
	if(m_uConnectionId == uOldConnectionId)reset();
	// else it has already been called!
}

void KviKvsObject_socket::delayedLookupRemoteIp()
{
	if(m_pDelayTimer)delete m_pDelayTimer;
	m_pDelayTimer = new QTimer();
	connect(m_pDelayTimer,SIGNAL(timeout()),this,SLOT(lookupRemoteIp()));

	m_pDelayTimer->setInterval(0);
	m_pDelayTimer->setSingleShot(true);
	m_pDelayTimer->start();
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
		KviKvsVariantList lParams;
		lParams.append(new KviKvsVariant(__tr2qs_ctx("Can't start the DNS thread","objects")));
		callFunction(this,"connectFailedEvent",&lParams);
		if(m_uConnectionId == uOldConnectionId)reset();
		// else it has already been called!
	}
}

void KviKvsObject_socket::lookupDone(KviDns *pDns)
{

	if(pDns->state() != KviDns::Success)
	{
		unsigned int uOldConnectionId = m_uConnectionId;
		KviKvsVariantList lParams;
		lParams.append(new KviKvsVariant(KviError::getDescription(pDns->error())));
		callFunction(this,"connectFailedEvent",&lParams);

		if(m_uConnectionId == uOldConnectionId)reset();
		// else it has already been called!
		return;
	}
	m_szRemoteIp = pDns->firstIpAddress();
	debug ("Dns resolved in %s",m_szRemoteIp.toUtf8().data());

	delete m_pDns;
	m_pDns = 0;
	doConnect();
}

void KviKvsObject_socket::writeNotifierFired(int)
{
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
		KviKvsVariantList lParams;
		lParams.append(new KviKvsVariant(KviError::getDescription(sockError)));
		callFunction(this,"connectFailedEvent",&lParams);
		if(m_uConnectionId == uOldConnectionId)reset();
		// else it has already been called!
	} else {
		// Succesfully connected
		// create the correct read notifier now...
		m_pSn = new QSocketNotifier((int)m_sock,QSocketNotifier::Read);
		QObject::connect(m_pSn,SIGNAL(activated(int)),this,SLOT(readNotifierFired(int)));
		m_pSn->setEnabled(true);

#ifdef COMPILE_IPV6_SUPPORT
		KviSockaddr sareal(0,m_bIPv6,m_bUdp);
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
	//read da
	if((m_uInBufferLen - m_uInDataLen) < KVI_READ_CHUNK)
	{
		m_uInBufferLen += KVI_IN_BUFFER_ALLOC_CHUNK;
		//m_pInBuffer->resize(m_uInBufferLen);
		m_pInBuffer = (char *)kvi_realloc(m_pInBuffer,m_uInBufferLen);
	}

	//int readLength = kvi_socket_recv(m_sock,m_pInBuffer->data() + m_uInDataLen,KVI_READ_CHUNK);
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
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
			if((err != EAGAIN) && (err != EINTR) && (err != WSAEWOULDBLOCK))
#else
			if((err != EAGAIN) && (err != EINTR))
#endif
			{
				unsigned int uOldConnectionId = m_uConnectionId;
				if(err > 0)

				{
					//	QString error=KviError::translateSystemError(err);
						KviKvsVariantList lParams;
						lParams.append(new KviKvsVariant((kvs_int_t)KviError::translateSystemError(err)));
						callFunction(this,"disconnectEvent",&lParams);
				} else {
						KviKvsVariantList lParams;
						lParams.append(new KviKvsVariant(KviError::getDescription(KviError_remoteEndClosedConnection)));
						callFunction(this,"disconnectEvent",&lParams);

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


	unsigned int uOldConnectionId = m_uConnectionId;
	KviKvsVariantList lParams;
	lParams.append(new KviKvsVariant((kvs_int_t)readLength));
	callFunction(this,"dataAvailableEvent",&lParams);
	if(m_uConnectionId == uOldConnectionId)
	{
		if(m_uInDataLen > (4096 * 1024)) // too much data in buffer (not reading)
		{
			KviKvsVariantList lParams;
			lParams.append(new KviKvsVariant(new KviKvsVariant(__tr2qs_ctx("Too much unprocessed incoming data (you've left this socket unmanaged ?)","objects"))));
			callFunction(this,"disconnectEvent",&lParams);
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
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
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
			KviKvsVariantList lParams;
			lParams.append(new KviKvsVariant(KviError::getDescription(KviError::translateSystemError(err))));
			callFunction(this,"disconnectEvent",&lParams);
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
	m_bIPv6 = false;
}



//--------------

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_socket.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES




