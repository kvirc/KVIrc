#ifndef _CLASS_SOCKET_H_
#define _CLASS_SOCKET_H_
//=============================================================================
//
//   File : class_socket.h
//   Creation date : Sun Nov 11 03:13:46 2001 GMT by Szymon Stefanek
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
//=============================================================================

#include "kvi_socket.h"
#include "kvi_pointerlist.h"

#include <QSocketNotifier>
#include <QTimer>

#define KVI_SCRIPT_SOCKET_STATUS_DISCONNECTED 0
#define KVI_SCRIPT_SOCKET_STATUS_DNS 1
#define KVI_SCRIPT_SOCKET_STATUS_CONNECTING 2
#define KVI_SCRIPT_SOCKET_STATUS_LISTENING 3
#define KVI_SCRIPT_SOCKET_STATUS_CONNECTED 4

class KviDns;
class KviDataBuffer;
/*
class KviScriptSocketObject : public KviScriptObject
{
	Q_OBJECT
public:
	KviScriptSocketObject(KviScriptObjectClass * cla,KviScriptObject * par,const char * nam);
	virtual ~KviScriptSocketObject();
protected:
	kvi_socket_t       m_sock;
	int                m_iStatus;
	KviStr             m_szRemoteIp;
	unsigned short int m_uRemotePort;
	KviStr             m_szLocalIp;
	unsigned short int m_uLocalPort;
	unsigned int       m_uConnectTimeout;
	QSocketNotifier  * m_pSn;
	KviDns           * m_pDns;
	QTimer           * m_pDelayTimer;
	char             * m_pInBuffer;
	unsigned int       m_uInBufferLen;
	unsigned int       m_uInDataLen;
	bool               m_bIPv6;
	unsigned int       m_uConnectionId;

	KviDataBuffer    * m_pOutBuffer;
	QTimer           * m_pFlushTimer;

	kvi_socket_t       m_secondarySock;
	unsigned short int m_uSecondaryPort;
	KviStr             m_szSecondaryIp;
protected:
	bool functionStatus(KviCommand *c,KviParameterList * params,KviStr &buffer);
	bool functionRemotePort(KviCommand *c,KviParameterList * params,KviStr &buffer);
	bool functionRemoteIp(KviCommand *c,KviParameterList * params,KviStr &buffer);
	bool functionLocalPort(KviCommand *c,KviParameterList * params,KviStr &buffer);
	bool functionLocalIp(KviCommand *c,KviParameterList * params,KviStr &buffer);
	bool functionConnect(KviCommand *c,KviParameterList * params,KviStr &buffer);
	bool functionSetConnectTimeout(KviCommand *c,KviParameterList * params,KviStr &buffer);
	bool functionConnectTimeout(KviCommand *c,KviParameterList * params,KviStr &buffer);
	bool functionClose(KviCommand *c,KviParameterList * params,KviStr &buffer);
	bool functionRead(KviCommand *c,KviParameterList * params,KviStr &buffer);
	bool functionReadHex(KviCommand *c,KviParameterList * params,KviStr &buffer);
	bool functionWrite(KviCommand *c,KviParameterList * params,KviStr &buffer);
	bool functionWriteHex(KviCommand *c,KviParameterList * params,KviStr &buffer);
	bool functionListen(KviCommand *c,KviParameterList * params,KviStr &buffer);
	bool functionAccept(KviCommand *c,KviParameterList * params,KviStr &buffer);

	void delayedConnect();
	void delayedLookupRemoteIp();
	void delayedFlush(unsigned int uTimeout);

	void eatInData(unsigned int uLen);
	unsigned int readGetLength(KviParameterList * params);

	void acceptConnection(kvi_socket_t s,unsigned short int uPort,const char * szIp);

	void reset();
protected slots:
	void doConnect();
	void lookupRemoteIp();
	void lookupDone(KviDns * pDns);
	void connectTimeout();
	void writeNotifierFired(int);
	void readNotifierFired(int);
	void tryFlush();
	void incomingConnection(int);
public:
	static void registerSelf();
	static void unregisterSelf();
};
*/
//===============================================================
#include "object_macros.h"


class KviKvsObject_socket : public KviKvsObject
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_socket)
protected:
	bool               m_bUdp;  
	kvi_socket_t       m_sock;
	int                m_iStatus;
	QString             m_szRemoteIp;
//	unsigned short int m_uRemotePort;
	QString             m_szLocalIp;
	kvi_u32_t          m_uLocalPort;
	unsigned int       m_uConnectTimeout;
	QSocketNotifier  * m_pSn;
	KviDns           * m_pDns;
	QTimer           * m_pDelayTimer;
	char             * m_pInBuffer;
	unsigned int       m_uInBufferLen;
	unsigned int       m_uInDataLen;
	bool               m_bIPv6;
	unsigned int       m_uConnectionId;
	kvs_uint_t m_uRemotePort;
	KviDataBuffer    * m_pOutBuffer;
	QTimer           * m_pFlushTimer;

	kvi_socket_t       m_secondarySock;
	kvi_u32_t          m_uSecondaryPort;
	QString             m_szSecondaryIp;
protected:
	bool functionStatus(KviKvsObjectFunctionCall *c);
	bool functionRemotePort(KviKvsObjectFunctionCall *c);
	bool functionRemoteIp(KviKvsObjectFunctionCall *c);
	bool functionLocalPort(KviKvsObjectFunctionCall *c);
	bool functionLocalIp(KviKvsObjectFunctionCall *c);
	bool functionConnect(KviKvsObjectFunctionCall *c);
	bool functionSetConnectTimeout(KviKvsObjectFunctionCall *c);
	bool functionConnectTimeout(KviKvsObjectFunctionCall *c);

	bool functionClose(KviKvsObjectFunctionCall *c);
	bool functionRead(KviKvsObjectFunctionCall *c);
	bool functionReadHex(KviKvsObjectFunctionCall *c);
	bool functionWrite(KviKvsObjectFunctionCall *c);
	bool functionWriteHex(KviKvsObjectFunctionCall *c);

	bool functionSetProtocol(KviKvsObjectFunctionCall *c);

	bool functionListen(KviKvsObjectFunctionCall *c);
	bool functionAccept(KviKvsObjectFunctionCall *c);

	void delayedConnect();
	void delayedLookupRemoteIp();
	void delayedFlush(unsigned int uTimeout);

	void eatInData(unsigned int uLen);
	unsigned int readGetLength(KviKvsObjectFunctionCall * params);

	void acceptConnection(kvi_socket_t s,kvi_u32_t uPort,const char * szIp);

	void reset();
protected slots:
	void doConnect();
	void lookupRemoteIp();
	void lookupDone(KviDns * pDns);
	void connectTimeout();
	void writeNotifierFired(int);
	void readNotifierFired(int);
	void tryFlush();
	void incomingConnection(int);
};
#endif //_CLASS_SOCKET_H_
