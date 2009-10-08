#ifndef _KVI_IRCCONNECTIONTARGETRESOLVER_H_
#define _KVI_IRCCONNECTIONTARGETRESOLVER_H_
//=============================================================================
//
//   File : kvi_ircconnectiontargetresolver.h
//   Creation date : Fri 11 May 2004 23:17:42 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2008 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_settings.h"
#include "kvi_qstring.h"

#include <QObject>

class KviConsole;
class KviDns;
class KviIrcConnection;
class KviIrcConnectionTarget;

class QTimer;

class KVIRC_API KviIrcConnectionTargetResolver : public QObject
{
	friend class KviIrcLink; // upper protocol in the stack
	Q_OBJECT
protected:
	KviIrcConnectionTargetResolver(KviIrcConnection * pConnection);
	~KviIrcConnectionTargetResolver();
public:
	enum Status { Success, Error };
	enum State { Idle, Running, Terminated };
private:
	KviIrcConnection             * m_pConnection;       // shallow, never null
	KviIrcConnectionTarget       * m_pTarget;           // shallow, never null
	KviConsole                   * m_pConsole;          // shallow, never null
	Status                         m_eStatus;
	State                          m_eState;

	// Auxiliary stuff
	QTimer                       * m_pStartTimer;       // timer used to start the connection
	KviDns                       * m_pProxyDns;         // the dns object for the proxy hostnames
	KviDns                       * m_pServerDns;        // the dns object for the server hostnames

	char                         * m_pReadBuffer;
	unsigned int                   m_uReadBufferLen;
	unsigned int                   m_uReadPackets;

	int                            m_iLastError;
public:
	void start(KviIrcConnectionTarget * t);
	// valid only after the terminated() signal
	Status status(){ return m_eStatus; };
	int lastError(){ return m_iLastError; };
	// causes the resolver to terminate with iLastError == KviError_operationAborted
	// the terminated() signal is emitted.
	void abort();
protected slots:
	void asyncStartResolve();
	void serverLookupTerminated(KviDns *);
	void proxyLookupTerminated(KviDns *);
private:
	void cleanup();
	void lookupProxyHostname();
	void lookupServerHostname();
	void haveServerIp();
	bool validateLocalAddress(const QString &szAddress,QString &szBuffer);
	void terminate(Status s,int iLastError);
signals:
	void terminated();
};

#endif //!_KVI_IRCCONNECTIONTARGETRESOLVER_H_
