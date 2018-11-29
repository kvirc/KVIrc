#ifndef _CLASS_SOCKET_H_
#define _CLASS_SOCKET_H_
//=============================================================================
//
//   File : KvsObject_socket.h
//   Creation date : Sun Nov 11 03:13:46 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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

#include <QAbstractSocket>
#include <QTcpServer>

#include "object_macros.h"

class KvsObject_socket : public KviKvsObject
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KvsObject_socket)
protected:
	QAbstractSocket * m_pSocket = nullptr;
	QTcpServer * m_pServer = nullptr;
	KviKvsRunTimeContext * m_pContext = nullptr;
	bool bIsSetFromExternal = false;

protected:
	bool init(KviKvsRunTimeContext * pContext, KviKvsVariantList * pParams) override;
	const char * getStateString(QAbstractSocket::SocketState);

	void setInternalSocket(QAbstractSocket * pSocket)
	{
		delete m_pSocket;
		m_pSocket = pSocket;
		bIsSetFromExternal = true;
		makeConnections();
	}

	bool status(KviKvsObjectFunctionCall * c);
	bool remotePort(KviKvsObjectFunctionCall * c);
	bool remoteIp(KviKvsObjectFunctionCall * c);
	bool localPort(KviKvsObjectFunctionCall * c);
	bool localIp(KviKvsObjectFunctionCall * c);
	bool functionConnect(KviKvsObjectFunctionCall * c);
	bool close(KviKvsObjectFunctionCall * c);
	bool read(KviKvsObjectFunctionCall * c);
	bool write(KviKvsObjectFunctionCall * c);
	bool bytesAvailable(KviKvsObjectFunctionCall * c);
	bool setProtocol(KviKvsObjectFunctionCall * c);
	//bool setReadBufferSize(KviKvsObjectFunctionCall *c);
	bool listen(KviKvsObjectFunctionCall * c);
	bool dataAvailableEvent(KviKvsObjectFunctionCall * c);
	bool incomingConnectionEvent(KviKvsObjectFunctionCall * c);
	bool connectedEvent(KviKvsObjectFunctionCall * c);
	bool disconnectedEvent(KviKvsObjectFunctionCall * c);
	bool errorEvent(KviKvsObjectFunctionCall * c);
	bool hostFoundEvent(KviKvsObjectFunctionCall * c);
	bool stateChangedEvent(KviKvsObjectFunctionCall * c);

	void makeConnections();
protected slots:
	void slotReadyRead();
	void slotNewConnection();
	void slotConnected();
	void slotDisconnected();
	void slotError(QAbstractSocket::SocketError socketError);
	void slotHostFound();
	void slotStateChanged(QAbstractSocket::SocketState socketState);
};
#endif //_CLASS_SOCKET_H_
