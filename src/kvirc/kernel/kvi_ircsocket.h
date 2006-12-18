#ifndef _KVI_IRCSOCKET_H_
#define _KVI_IRCSOCKET_H_
//==========================================================================================
//
//   File : kvi_ircsocket.h
//   Creation date : Tue Jul 30 19:25:17 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2004 Szymon Stefanek (pragma at kvirc dot net)
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
//==========================================================================================

#include "kvi_settings.h"
#include "kvi_string.h"
#include "kvi_sockettype.h"
#include "kvi_time.h"
#include "kvi_list.h"

#include <qobject.h>

class KviIrcServer;
class KviProxy;
class KviIrcConnection;
class KviIrcConnectionTarget;
class KviIrcLink;
class KviSSL;
class KviConsole;
class KviDataBuffer;

class QTimer;
class QSocketNotifier;

//
// This class is the lowest level of the KVIrc networking stack
// it sends and receives raw data packets from the underlying system
// socket.
// It should output only verbose "progress" messages.
//

typedef struct _KviIrcSocketMsgEntry
{
	KviDataBuffer * pData;
	struct _KviIrcSocketMsgEntry   * next_ptr;
} KviIrcSocketMsgEntry;


class KVIRC_API KviIrcSocket : public QObject
{
	Q_OBJECT
public:
	KviIrcSocket(KviIrcLink * pLink);
	~KviIrcSocket();
public:
	enum SocketState {
		Idle ,
		Connecting ,
		ProxyLogin ,
		ProxyHttpError,
		Connected ,
		ProxyFinalV4 ,
		ProxyFinalV5 ,
		ProxySelectAuthMethodV5 ,
		ProxyUserPassV5 ,
		ProxyFinalHttp ,
		SSLHandshake
	};
protected:
	unsigned int           m_uId;
	KviIrcLink           * m_pLink;
	KviConsole           * m_pConsole;
	kvi_socket_t           m_sock;
	SocketState              m_state;
	QSocketNotifier      * m_pWsn;
	QSocketNotifier      * m_pRsn;
	KviIrcServer         * m_pIrcServer;
	KviProxy             * m_pProxy;
	QTimer               * m_pTimeoutTimer;
	unsigned int           m_uReadBytes;
	unsigned int           m_uSentBytes;
	int                    m_iLastError;
	unsigned int           m_uSentPackets;
	KviIrcSocketMsgEntry * m_pSendQueueHead;
	KviIrcSocketMsgEntry * m_pSendQueueTail;
	QTimer               * m_pFlushTimer;
	struct timeval         m_tAntiFloodLastMessageTime;
	bool                   m_bInProcessData;
#ifdef COMPILE_SSL_SUPPORT
	KviSSL               * m_pSSL;
#endif
public:
	int lastError(){ return m_iLastError; };
	unsigned int id(){ return m_uId; };
	KviConsole * console(){ return m_pConsole; };
	KviIrcLink * link(){ return m_pLink; };
#ifdef COMPILE_SSL_SUPPORT
	bool usingSSL(){ return m_pSSL; };
#else
	bool usingSSL(){ return false; };
#endif
	unsigned int readBytes(){ return m_uReadBytes; };
	unsigned int sentBytes(){ return m_uSentBytes; };
	//unsigned int readPackets(){ return m_uReadPackets; };
	unsigned int sentPackets(){ return m_uSentPackets; };
	SocketState state(){ return m_state; };
	bool isConnected(){ return m_state == Connected; };
	int startConnection(KviIrcServer * srv,KviProxy * prx = 0,const char * bindAddress = 0);

	//bool sendData(const char *buffer,int buflen = -1) KVI_DEPRECATED;
	//bool sendFmtData(const char *fmt,...) KVI_DEPRECATED;
	bool sendPacket(KviDataBuffer * pData);
	void abort();
	bool getLocalHostIp(QString &szIp,bool bIpV6);
protected slots:
	void connectionTimedOut();
	void writeNotifierFired(int);
	void readData(int);
	void readProxyData(int);
	void readHttpProxyErrorData(int);
	void flushSendQueue();
	void doSSLHandshake(int);
protected:
#ifdef COMPILE_SSL_SUPPORT
	void raiseSSLError();
	void printSSLPeerCertificate();
	void printSSLCipherInfo();
#endif
	bool sendRawData(const char *buffer,int buflen);
	void raiseError(int iError);
	void connectionEstabilished();
	void connectedToProxy();
	void connectedToIrcServer();
	void proxyLoginHttp();
	void proxyLoginV4();
	void proxyLoginV5();
	void proxyAuthUserPassV5();
	void proxySendTargetDataV5();
	void proxyHandleV5AuthReply(unsigned char reply);
	void proxyHandleV5MethodReply(unsigned char reply);
	void proxyHandleV5FinalReply(unsigned char reply);
	void proxyHandleV4FinalReply(unsigned char reply);
	void proxyHandleHttpFinalReply(const char * buffer,int bufLen);
	void linkUp();
	void handleInvalidSocketRead(int readedLength);
	virtual void reset();
	void free_msgEntry(KviIrcSocketMsgEntry * e);
	bool queue_removeMessage();
	void queue_removeAllMessages();
	virtual void queue_insertMessage(KviIrcSocketMsgEntry *msg_ptr);
	virtual void setState(SocketState st);
private:
	void outputSSLMessage(const QString &szMsg);
	void outputSSLError(const QString &szMsg);
	void outputProxyMessage(const QString &szMsg);
	void outputProxyError(const QString &szMsg);
	void outputSocketMessage(const QString &szMsg);
	void outputSocketWarning(const QString &szMsg);
	void outputSocketError(const QString &szMsg);
};

#endif //_KVI_IRCSOCKET_H_
