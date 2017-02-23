#ifndef _MARSHAL_H_
#define _MARSHAL_H_
//=============================================================================
//
//   File : DccMarshal.h
//   Creation date : Sun Sep 17 2000 10:44:20 by Szymon Stefanek
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

#include "KviCString.h"
#include "KviError.h"
#include "kvi_sockettype.h"
#include "kvi_inttypes.h"

#include <QObject>
#include <QSocketNotifier>
#include <QTimer>

class KviWindow;
class DccMarshal;

#ifdef COMPILE_SSL_SUPPORT
#include "KviSSLMaster.h"
#endif

class DccMarshalOutputContext
{
	friend class DccMarshal;

public:
	DccMarshalOutputContext(){};
	virtual ~DccMarshalOutputContext(){};

protected:
	virtual KviWindow * dccMarshalOutputWindow() = 0;
	virtual const char * dccMarshalOutputContextString() = 0;
};

class DccMarshal : public QObject
{
	Q_OBJECT
public:
	DccMarshal(DccMarshalOutputContext * ctx);
	~DccMarshal();

protected:
	// DCC DESCRIPTOR
	QString m_szIp;   // Dcc initiator ip address (the one that listens)
	QString m_szPort; // Dcc initiator port (the one that listens)
	// other info
	bool m_bIPv6;              // Dcc mode
	kvi_u32_t m_uPort;         // Dcc initiator port
	bool m_bOutgoing;          // true if WE have connected to the remote host (so m_szIp is the remote host ip)
	QString m_szSecondaryIp;   // Ip of the client that has connected to the remote host
	QString m_szSecondaryPort; // Port of the client that has connected to the remote host
	// internals
	kvi_socket_t m_fd; // socket
	QSocketNotifier * m_pSn;
	bool m_bUseTimeout;
	QTimer * m_pTimeoutTimer;
#ifdef COMPILE_SSL_SUPPORT
	KviSSL * m_pSSL;
	bool m_bUseSSL;
#endif
	DccMarshalOutputContext * m_pOutputContext;

public:
	const QString & dccIp() const { return m_szIp; };
	const QString & dccPort() const { return m_szPort; };
	const QString & localIp() const { return m_bOutgoing ? m_szSecondaryIp : m_szIp; };
	const QString & localPort() const { return m_bOutgoing ? m_szSecondaryPort : m_szPort; };
	const QString & remoteIp() const { return m_bOutgoing ? m_szIp : m_szSecondaryIp; };
	const QString & remotePort() const { return m_bOutgoing ? m_szPort : m_szSecondaryPort; };
	KviError::Code dccListen(const QString & ip, const QString & port, bool bUseTimeout, bool bUseSSL = false);
	KviError::Code dccConnect(const char * ip, const char * port, bool bUseTimeout, bool bUseSSL = false);
	kvi_socket_t releaseSocket();
#ifdef COMPILE_SSL_SUPPORT
	KviSSL * releaseSSL();
#endif
	void abort();

private:
	void reset();
	//#ifdef COMPILE_SSL_SUPPORT
	//	bool trySSLCertificate();
	//#endif
private slots:
	void doSSLHandshake(int);
	//	void doListenSSLHandshake();
	void snActivated(int);
	void connectionTimedOut();
	void doListen();
	void doConnect();
signals:
	void startingSSLHandshake();
	void sslError(const char * msg);
	void connected();
	void inProgress();
	void error(KviError::Code);
};

#endif //_MARSHAL_H_
