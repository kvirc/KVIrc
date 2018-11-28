//=============================================================================
//
//   File : KvsObject_socket.cpp
//   Creation date : Sun Nov 11 03:13:45 2001 GMT by Szymon Stefanek
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

#include "KvsObject_socket.h"
#include "KvsObject_memoryBuffer.h"
#include "KvsObject_file.h"

#include "kvi_settings.h"
#include "KviQString.h"

#include "kvi_debug.h"

#include "KviFile.h"
#include "KviFileUtils.h"
#include "KviLocale.h"
#include "KviNetUtils.h"
#include "KviDnsResolver.h"
#include "KviError.h"
#include "kvi_socket.h"
#include "KviMemory.h"
#include "KviDataBuffer.h"

#include <QByteArray>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QHostAddress>

const char * const sockstate_tbl[] = {
	"Unconnected",
	"HostLookUp",
	"Connecting",
	"Connected",
	"Bound",
	"Listening",
	"Closing"
};

const char * const sockerrors_tbl[] = {
	"ConnectionRefused",
	"RemoteHostClosed",
	"HostNotFound",
	"SocketAccess",
	"SocketResource",
	"SocketTimeout",
	"DatagramTooLarge",
	"Network",
	"AddressInUse",
	"SocketAddressNotAvailable",
	"UnsupportedSocketOperation",
	"ProxyAuthenticationRequired",
	"SslHandshakeFailed",
	"UnfinishedSocketOperation",
	"ProxyConnectionRefused",
	"ProxyConnectionClosed",
	"ProxyConnectionTimeout",
	"ProxyNotFound",
	"ProxyProtocol",
	"UnknownSocket"
};

/*
	@doc: socket
	@keyterms:
		socket object class
	@title:
		socket class
	@type:
		class
	@short:
		A IPv4/IPv6 TCP socket
	@inherits:
		[class]object[/class]
	@description:
		This class provides a standard TCP/IP socket functionality.[br]
		It can be used either for connecting to a remote host or to listening for incoming connections.[br]
		If the KVIrc executable has been compiled with the IPv6 protocol support, this socket also supports it.[br]
	@functions:
		!fn: <integer or string> $status(<asString:boolean>)
		Returns the status of the socket :[br]
		[pre]
			0 = Unconnected
			1 = HostLookUp
			2 = Connecting
			3 = Connected
			4 = Bound
			5 = Closing
			6 = Listening
		[/pre]
		!fn: $connect(<host>,<port>)
		Attempts a connection to <host> on port <port>.[br]
		<host> can be a numeric internet address (either IPv4 or IPv6 (if supported)) or a hostname.[br]
		If a hostname is used, a DNS lookup is performed (the socket enters the [i]DNS call[i/] state).[br]
		This function returns [b]1[/b] if the connect attempt can be successfully initiated, and
		[b]0[/b] otherwise.[br] In fact, this function returns [b]0[/b] only if the supplied <port> parameter
		is invalid or the socket is in an incoherent state (already connected or listening):
		for a newly created socket and with a valid <port> number you can safely ignore
		the return value.[br]
		Please note that the connection is asynchronous: when this function returns
		the socket is [b]not[/b] connected: it has just initiated the connect attempt
		and you will be notified of the attempt result by an asynchronous event call:
		in case of failure, $connectFailedEvent() will be called, in case of
		success, $connectEvent() will be called.
		!fn: $listen([<port>[,<interface>[,<force_ipv6>]]])
		Attempts to listen on the specified <port> and <interface>.[br]
		If <port> is not passed it is assumed to be [b]0[/b], if <interface> is not passed, it is assumed to be
		[i]any interface[/i] (INADDR_ANY).[br] Port 0 means that the kernel should choose a [i]random[/i] port to listen on.[br]
		If the <interface> is recognized as IPv6 address, and IPv6 is supported, the socket listens
		in IPv6 mode. If <interface> is an empty string and <force_ipv6> is 1 the socket listens
		on [i]any IPv6 interface[/i].[br]
		This function returns [b]1[/b] in case of success and [b]0[/b] in case of failure.[br]
		On some systems listening in the IPv6 namespace allows to accept also IPv4 connections (this includes
		Linux but not windows afaik).[br]
		When an incoming connection will arrive, $incomingConnectionEvent() will be called.
		!fn: $connectedEvent()
		This function is called when a connection attempt has been successfully completed.
		The socket is currently connected to [classfnc:socket]$remoteIp[/classfnc]() on
		[classfnc:socket]$remotePort[/classfnc](). You can start
		writing data and you may expect [classfnc:socket]$dataAvailableEvent[/classfnc]() to be
		triggered.
		!fn: $incomingConnectionEvent(<socket:h_object>)
		This function is called when an incoming connection arrives over a socket in listening state.[br]
		You must return [b]1[/b] if you to terminated this incoming connection call [classfnc:socket]$accept[/classfnc]() passing a newly created socket object
		to accept and handle the connection.[br] If you don't call [classfnc:socket]$accept[/classfnc]()
		the incoming connection will be automatically terminated.
		!fn: $connectFailedEvent(<reason>)
		This function is called when a connection attempt fails for some reason. <reason> contains
		the error string.[br]
		This function may be called only between a call to [classfnc:socket]$connect[/classfnc]() and
		the [classfnc:socket]$connectEvent[/classfnc]().
		!fn: $disconnectEvent([error])
		This function is called when a connection is terminated either cleanly or because of an error.[br]
		[error] is an empty string in case of a [i]clean[/i] termination (connection closed by the remote host)
		or is a message describing the socket error that caused the connection to be interrupted.
		!fn: $dataAvailableEvent(<data_length>)
		This function is called when some data is available to be read: the <data_length> parameter specifies
		the length of the available data in bytes.[br]
		You can use one of the $read* functions to obtain the data
		!fn: $read(<length>[,<hobject>])
		Reads at most <length> bytes of data from the socket. If <length> is anything [i]outside[/i] the
		available data range (<length> < 0 or <length> > available_data_length), this function
		returns all the available data.[br]
		By default this function can deal ASCII data only: NULL characters are transformed to
		ASCII characters 255. You can pass a [class]memorybuffer[/class] object to read binary data.
		!fn: $write(<data, array,files or hobject>[,length])
		Writes <data> to the socket.[br]
		This function can deal with binary data passing  a [class]memorybuffer[/class] object[br]
		Please note that when this function finishes it does not mean that the data has reached the remote end.[br]
		Basically it does not even mean that the data has been sent to the remote host.[br]
		The data is enqueued for sending and will be sent as soon as possible.[br]
		Using an array you can pass bytes or data string like this: @$write($array($(0xff),$(0xff),$(0xff),$(0xff),"This is an example"));
		If you're going to [cmd]delete[/cmd] this object just after the $write call, you should
		call [classfnc:socket]$close[/classfnc]() just before [cmd]delete[/cmd] to ensure the data delivery.
		!fn: $close()
		Resets this socket state: kills any pending or active connection. After a close() call
		the socket may be used for a new connection.[br]
		If there is an active connection, there is a last attempt to flush the pending outgoing data.[br]
		You don't need to call $close() if you [cmd]delete[/cmd] the socket: KVIrc will
		reset the socket state automatically and free the memory. But if you want to ensure data delivery
		after a $write call sequence and just before a [cmd]delete[/cmd], $close() is the only chance to do it.
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
		The return value is meaningful only if the socket is in connected, listening or connecting state.
		!fn: $localPort()
		Returns the port of the local end of this socket.[br]
		The return value is meaningful only if the socket is in connected, listening or connecting state.
	@examples:
		[example]
			[comment]# Server socket: listen 8080 port and answer to requests (multi-threaded)[/comment]
			class("webserver","socket")
			{
				function incomingConnectionEvent()
				{
				[comment]# incoming connection socket passed by the framework[/comment]
				%socket = $0
				debug "Webserver incoming Connection from: %socket->$remoteIp : %socket->$remotePort"
				%socket->$write("HTTP/1.0 200 OK\n\n<html><head></head><body><h1>KVIrc Webserver</h1></body></html>\n")
				[comment]# tells KVIrc no need this socket anymore[/comment]
				return $true()
				}
				function constructor()
				{
				debug listen @$listen(8080, "127.0.0.1")
				}
			}
			[comment]# finally start webserver[/comment]
			%WebS = $new(webserver)
		[/example]
		[example]
		[comment]# Client socket - go to google and grab request header[/comment][br]
			class("httprequest","socket")
			{
				function errorEvent()
				{
					[comment]# the connection to the server failed[/comment]
					debug  "Connection failed: "$0
					delete $$
				}
				function disconnectedEvent()
				{
					[comment]# connection has been closed[/comment]
					debug  "Connection is closed"
					delete $$
				}
				function destructor()
				{
					[comment]# if the socket is still open close it[/comment]
					if(@$status() == "Connected") @$close()
				}
				function stateChangedEvent()
				{
					debug socket state $0
				}
				function dataAvailableEvent()
				{
					[comment]# reading the received data[/comment]
					debug reading $0 bytes
					%newdata  = @$read($0)
					debug data:  %newdata
					[comment]# close and delete the socket[/comment]
					@$close()
					delete $$
				}
				function constructor()
				{
					[comment]# connect to the server[/comment]
					@$connect("www.google.com",80)
				}
				function connectedEvent()
				{
					[comment]# connection is complete[/comment]
					[comment]# send a request to receive the headers only from http://www.google.com/[/comment]
					debug connected
					debug written bytes @$write("HEAD / HTTP/1.1\r\nHost: www.google.de\r\nConnection: Close\r\nUser-Agent: KVIrc socket\r\n\r\n") on socket;
				}
			}
			%Temp = $new(httprequest)
		[/example]
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_socket, "socket", "object")

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_socket, status)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_socket, remotePort)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_socket, remoteIp)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_socket, localIp)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_socket, localPort)
KVSO_REGISTER_HANDLER(KvsObject_socket, "connect", functionConnect)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_socket, close)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_socket, read)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_socket, write)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_socket, bytesAvailable)

//KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_socket,setReadBufferSize)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_socket, setProtocol)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_socket, listen)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_socket, dataAvailableEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_socket, incomingConnectionEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_socket, connectedEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_socket, disconnectedEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_socket, errorEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_socket, hostFoundEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_socket, stateChangedEvent)

KVSO_END_REGISTERCLASS(KvsObject_socket)

bool KvsObject_socket::init(KviKvsRunTimeContext * c, KviKvsVariantList *)
{
	m_pSocket = new QTcpSocket();
	m_pServer = nullptr;
	m_pContext = c;
	makeConnections();
	bIsSetFromExternal = false;
	return true;
}

KVSO_BEGIN_CONSTRUCTOR(KvsObject_socket, KviKvsObject)
KVSO_END_CONSTRUCTOR(KvsObject_socket)

KVSO_BEGIN_DESTRUCTOR(KvsObject_socket)
if(m_pSocket && !bIsSetFromExternal)
{
	m_pSocket->close();
	delete m_pSocket;
}
m_pSocket = nullptr;
if(m_pServer)
	delete m_pServer;
m_pServer = nullptr;
KVSO_END_DESTRUCTOR(KvsObject_socket)

KVSO_CLASS_FUNCTION(socket, status)
{
	c->returnValue()->setString(getStateString(m_pSocket->state()));
	return true;
}

KVSO_CLASS_FUNCTION(socket, close)
{
	m_pSocket->disconnectFromHost();
	return true;
}

KVSO_CLASS_FUNCTION(socket, remotePort)
{
	c->returnValue()->setInteger(m_pSocket->peerPort());
	return true;
}

KVSO_CLASS_FUNCTION(socket, remoteIp)
{
	c->returnValue()->setString(m_pSocket->peerAddress().toString());
	return true;
}

KVSO_CLASS_FUNCTION(socket, localPort)
{
	c->returnValue()->setInteger(m_pSocket->localPort());
	return true;
}

KVSO_CLASS_FUNCTION(socket, bytesAvailable)
{
	c->returnValue()->setInteger(m_pSocket->bytesAvailable());
	return true;
}

KVSO_CLASS_FUNCTION(socket, localIp)
{
	c->returnValue()->setString(m_pSocket->localAddress().toString());
	return true;
}

KVSO_CLASS_FUNCTION(socket, read)
{
	kvs_int_t iLen;
	KviKvsObject * pObject;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("length", KVS_PT_INTEGER, KVS_PF_OPTIONAL, iLen)
	KVSO_PARAMETER("hobject", KVS_PT_HOBJECT, KVS_PF_OPTIONAL, hObject)
	KVSO_PARAMETERS_END(c)
	if(iLen > m_pSocket->bytesAvailable() || !iLen)
		iLen = m_pSocket->bytesAvailable();
	if(hObject)
	{
		pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
		if(!pObject)
		{
			c->warning(__tr2qs_ctx("Buffer parameter is not an object", "objects"));
			return true;
		}
		if(pObject->inheritsClass("memorybuffer"))
		{
			QByteArray * pBuffer = ((KvsObject_memoryBuffer *)pObject)->pBuffer();
			int oldsize = pBuffer->size();
			pBuffer->resize(oldsize + iLen);
			m_pSocket->read(pBuffer->data() + oldsize, iLen);
		}
		else if(pObject->inheritsClass("file"))
		{
			KviFile * pFile = ((KvsObject_file *)pObject)->file();
			if(!pFile->isOpen())
			{
				c->warning(__tr2qs_ctx("File is not open!", "objects"));
				return true;
			}
			pFile->write(m_pSocket->read(iLen));
		}
		else
		{
			c->warning(__tr2qs_ctx("Buffer parameter is not a memorybuffer object", "objects"));
			return true;
		}
		return true;
	}
	if(iLen > 0)
	{
		// convert NULLS to char 255
		char * buffer = (char *)KviMemory::allocate(iLen);
		m_pSocket->read(buffer, iLen);
		for(int i{}; i < iLen; i++)
		{
			if(!buffer[i])
				buffer[i] = (char)(255);
		}
		QString tmpBuffer = QString::fromUtf8(buffer, iLen);
		c->returnValue()->setString(tmpBuffer);
		KviMemory::free(buffer);
	}
	return true;
}

KVSO_CLASS_FUNCTION(socket, write)
{
	kvs_uint_t uLen;
	KviKvsObject * pObject;
	KviKvsVariant * pVariantData;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("data_or_file_or_memorybuffer", KVS_PT_VARIANT, 0, pVariantData)
	KVSO_PARAMETER("length", KVS_PT_UNSIGNEDINTEGER, KVS_PF_OPTIONAL, uLen)
	KVSO_PARAMETERS_END(c)
	if(pVariantData->isHObject())
	{
		pVariantData->asHObject(hObject);
		pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
		if(!pObject)
		{
			c->warning(__tr2qs_ctx("Buffer parameter is not an object", "objects"));
			return true;
		}
		if(pObject->inheritsClass("memorybuffer"))
		{
			QByteArray * p = ((KvsObject_memoryBuffer *)pObject)->pBuffer();
			m_pSocket->write(*p);
		}
		else if(pObject->inheritsClass("file"))
		{
			KviFile * pFile = ((KvsObject_file *)pObject)->file();
			if(!pFile->isOpen())
			{
				c->warning(__tr2qs_ctx("File is not open!", "objects"));
				return true;
			}
			if(!uLen)
				uLen = pFile->size();
			kvs_int_t size = pFile->size();
			pFile->flush();
			m_pSocket->write((const char *)pFile->read(uLen).data(), uLen);
			c->returnValue()->setBoolean((size - pFile->pos() == 0));
		}
		else
		{
			c->warning(__tr2qs_ctx("Buffer parameter is not a memorybuffer or file object", "objects"));
			return true;
		}
	}
	else
	{
		if(pVariantData->isArray())
		{
			KviKvsArray * pArray = pVariantData->array();
			for(unsigned int i = 0; i < pArray->size(); i++)
			{
				KviKvsVariant * pVar = pArray->at(i);
				kvs_int_t iValue;
				if(pVar->asInteger(iValue))
				{
					if(iValue < 256 && iValue >= 0)
					{
						m_pSocket->putChar(iValue);
						continue;
					}
					else
					{
						c->warning(__tr2qs_ctx("Only values in the range of 0-255 are allowed: integer %d is out of range", "objects"), iValue);
						return true;
					}
				}
				else
				{
					if(pVar->isString())
					{
						QString szData;
						pVar->asString(szData);
						QByteArray szData8 = szData.toUtf8();
						m_pSocket->write((const char *)szData8.data(), szData8.length());
					}
					else
					{
						c->warning(__tr2qs_ctx("Datatype not supported", "objects"));
						return true;
					}
				}
			}
			return true;
		}
		QString szData;
		pVariantData->asString(szData);
		if(!KviFileUtils::fileExists(szData))
		{
			QByteArray szData8 = szData.toUtf8();
			if(szData8.length() > 0)
			{
				qDebug("write on socket %s", szData8.data());
				kvs_int_t bytes = m_pSocket->write((const char *)szData8.data(), szData8.length());
				c->returnValue()->setInteger(bytes);
			}
		}
		else
		{
			KviFile f(szData);
			f.open(QIODevice::ReadOnly);
			QByteArray ar = f.readAll();
			m_pSocket->write((const char *)ar.data(), ar.size());
			f.close();
		}
	}
	return true;
}
KVSO_CLASS_FUNCTION(socket, setProtocol)
{
	QString szProto;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("protocol", KVS_PT_STRING, 0, szProto)
	KVSO_PARAMETERS_END(c)
	if(m_pSocket)
		delete m_pSocket;
	if(KviQString::equalCI(szProto, "udp"))
		m_pSocket = new QUdpSocket();
	else
		m_pSocket = new QTcpSocket();
	makeConnections();
	return true;
}
KVSO_CLASS_FUNCTION(socket, functionConnect)
{
	kvs_uint_t uRemotePort;
	QString szRemoteIp;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("remote_ip", KVS_PT_STRING, 0, szRemoteIp)
	KVSO_PARAMETER("remote_port", KVS_PT_UNSIGNEDINTEGER, 0, uRemotePort)
	KVSO_PARAMETERS_END(c)
	if(uRemotePort > 65535)
	{
		c->warning(__tr2qs_ctx("Value %d for port is out of range (values allowed are from 0 to 65535)", "objects"), uRemotePort);
		return true;
	}
	m_pSocket->connectToHost(szRemoteIp, uRemotePort);
	c->returnValue()->setBoolean(true);
	return true;
}

KVSO_CLASS_FUNCTION(socket, listen)
{
	kvs_uint_t uLocalPort;
	QString m_szLocalIp;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("local_port", KVS_PT_UNSIGNEDINTEGER, 0, uLocalPort)
	KVSO_PARAMETER("interface", KVS_PT_STRING, KVS_PF_OPTIONAL, m_szLocalIp)
	KVSO_PARAMETERS_END(c)
	if(uLocalPort > 65535)
	{
		c->warning(__tr2qs_ctx("Value %d for port is out of range (values allowed are from 0 to 65535): switch to random value", "objects"), uLocalPort);
		uLocalPort = 0;
	}
	if(m_pServer)
		delete m_pServer;
	m_pServer = new QTcpServer();
	QHostAddress address(m_szLocalIp);
	bool bOk = m_pServer->listen(address, uLocalPort);
	connect(m_pServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
	c->returnValue()->setBoolean(bOk);
	return true;
}
void KvsObject_socket::makeConnections()
{
	connect(m_pSocket, SIGNAL(connected()), this, SLOT(slotConnected()));
	connect(m_pSocket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
	connect(m_pSocket, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
	connect(m_pSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));
	connect(m_pSocket, SIGNAL(hostFound()), this, SLOT(slotHostFound()));
	connect(m_pSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(slotStateChanged(QAbstractSocket::SocketState)));
	//proxyAuthenticationRequired ( const QNetworkProxy & proxy, QAuthenticator * authenticator )
}
const char * KvsObject_socket::getStateString(QAbstractSocket::SocketState state)
{
	int idx = 0;
	switch(state)
	{
		case QAbstractSocket::UnconnectedState:
		{
			idx = 0;
			break;
		}
		case QAbstractSocket::HostLookupState:
		{
			idx = 1;
			break;
		}
		case QAbstractSocket::ConnectingState:
		{
			idx = 2;
			break;
		}
		case QAbstractSocket::ConnectedState:
		{
			idx = 3;
			break;
		}
		case QAbstractSocket::BoundState:
		{
			idx = 4;
			break;
		}
		case QAbstractSocket::ListeningState:
		{
			idx = 5;
			break;
		}
		case QAbstractSocket::ClosingState:
		{
			idx = 6;
			break;
		}
		default:
		{
			// internal state?
		}
	}
	return sockstate_tbl[idx];
}

// slots
void KvsObject_socket::slotReadyRead()
{
	KviKvsVariantList lParams;
	lParams.append(new KviKvsVariant((kvs_int_t)m_pSocket->bytesAvailable()));
	callFunction(this, "dataAvailableEvent", &lParams);
}

void KvsObject_socket::slotConnected()
{
	KviKvsVariantList * lParams = nullptr;
	callFunction(this, "connectedEvent", lParams);
}

void KvsObject_socket::slotDisconnected()
{
	KviKvsVariantList * lParams = nullptr;
	callFunction(this, "disconnectedEvent", lParams);
}

void KvsObject_socket::slotError(QAbstractSocket::SocketError socketError)
{
	KviKvsVariantList lParams;
	QString szError = sockerrors_tbl[socketError];
	lParams.append(new KviKvsVariant(szError));
	callFunction(this, "errorEvent", &lParams);
}

void KvsObject_socket::slotHostFound()
{
	KviKvsVariantList * lParams = nullptr;
	callFunction(this, "hostFoundEvent", lParams);
}

void KvsObject_socket::slotStateChanged(QAbstractSocket::SocketState socketState)
{
	KviKvsVariantList lParams;
	QString szState = getStateString(socketState);
	lParams.append(new KviKvsVariant(szState));
	callFunction(this, "stateChangedEvent", &lParams);
}

void KvsObject_socket::slotNewConnection()
{
	QTcpSocket * pSocket = m_pServer->nextPendingConnection();
	KviKvsObjectClass * pClass = KviKvsKernel::instance()->objectController()->lookupClass("socket");
	KviKvsVariantList lParams;
	KviKvsObject * pObject = pClass->allocateInstance(this, "internalsocket", m_pContext, &lParams);
	((KvsObject_socket *)pObject)->setInternalSocket(pSocket);
	kvs_hobject_t hobj = pObject->handle();
	KviKvsVariantList params(new KviKvsVariant(hobj));
	bool ret = false;
	KviKvsVariant retv(ret);
	callFunction(this, "incomingConnectionEvent", &retv, &params);
	ret = retv.asBoolean();
	if(ret)
	{
		pObject = KviKvsKernel::instance()->objectController()->lookupObject(hobj);
		if(pObject)
			pObject->dieNow();
	}
}
KVSO_CLASS_FUNCTION(socket, incomingConnectionEvent)
{
	emitSignal("incomingConnection", c, c->params());
	return true;
}
KVSO_CLASS_FUNCTION(socket, dataAvailableEvent)
{
	emitSignal("dataAvailable", c, c->params());
	return true;
}

KVSO_CLASS_FUNCTION(socket, connectedEvent)
{
	emitSignal("connected", c, c->params());
	return true;
}

KVSO_CLASS_FUNCTION(socket, disconnectedEvent)
{
	emitSignal("disconnected", c, c->params());
	return true;
}

KVSO_CLASS_FUNCTION(socket, errorEvent)
{
	emitSignal("error", c, c->params());
	return true;
}

KVSO_CLASS_FUNCTION(socket, hostFoundEvent)
{
	emitSignal("hostFound", c, c->params());
	return true;
}

KVSO_CLASS_FUNCTION(socket, stateChangedEvent)
{
	emitSignal("stateChanged", c, c->params());
	return true;
}
