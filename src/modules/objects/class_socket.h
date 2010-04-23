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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
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
	char				* m_pInBuffer;
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
	bool status(KviKvsObjectFunctionCall *c);
	bool remotePort(KviKvsObjectFunctionCall *c);
	bool remoteIp(KviKvsObjectFunctionCall *c);
	bool localPort(KviKvsObjectFunctionCall *c);
	bool localIp(KviKvsObjectFunctionCall *c);
	bool functionConnect(KviKvsObjectFunctionCall *c);
	bool setConnectTimeout(KviKvsObjectFunctionCall *c);
	bool connectTimeout(KviKvsObjectFunctionCall *c);

	bool close(KviKvsObjectFunctionCall *c);
	bool read(KviKvsObjectFunctionCall *c);
	bool write(KviKvsObjectFunctionCall *c);


	bool setProtocol(KviKvsObjectFunctionCall *c);

	bool listen(KviKvsObjectFunctionCall *c);
	bool accept(KviKvsObjectFunctionCall *c);

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
	void connectTimeoutSlot();
	void writeNotifierFired(int);
	void readNotifierFired(int);
	void tryFlush();
	void incomingConnection(int);
};
#endif //_CLASS_SOCKET_H_
