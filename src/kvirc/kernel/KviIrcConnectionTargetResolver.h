#ifndef _KVI_IRCCONNECTIONTARGETRESOLVER_H_
#define _KVI_IRCCONNECTIONTARGETRESOLVER_H_
//=============================================================================
//
//   File : KviIrcConnectionTargetResolver.h
//   Creation date : Fri 11 May 2004 23:17:42 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_settings.h"
#include "KviError.h"
#include "KviQString.h"

#include <QObject>

#ifdef Status
#undef Status
#endif

#ifdef Success
#undef Success
#endif

#ifdef Error
#undef Error
#endif

class KviConsoleWindow;
class KviDnsResolver;
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
	enum Status
	{
		Success,
		Error
	};
	enum State
	{
		Idle,
		Running,
		Terminated
	};

private:
	KviIrcConnection * m_pConnection;             // shallow, never null
	KviIrcConnectionTarget * m_pTarget = nullptr; // shallow, never null
	KviConsoleWindow * m_pConsole;                // shallow, never null
	Status m_eStatus = Success;
	State m_eState = Idle;

	// Auxiliary stuff
	QTimer * m_pStartTimer = nullptr;        // timer used to start the connection
	KviDnsResolver * m_pProxyDns = nullptr;  // the dns object for the proxy hostnames
	KviDnsResolver * m_pServerDns = nullptr; // the dns object for the server hostnames

	int m_iLastError = KviError::Success;

public:
	void start(KviIrcConnectionTarget * t);
	// valid only after the terminated() signal
	Status status() const { return m_eStatus; }
	int lastError() const { return m_iLastError; }
	// causes the resolver to terminate with iLastError == KviError_operationAborted
	// the terminated() signal is emitted.
	void abort();
protected slots:
	void asyncStartResolve();
	void serverLookupTerminated(KviDnsResolver *);
	void proxyLookupTerminated(KviDnsResolver *);

private:
	void cleanup();
	void lookupProxyHostname();
	void lookupServerHostname();
	void haveServerIp();
	bool validateLocalAddress(const QString & szAddress, QString & szBuffer);
	void terminate(Status s, int iLastError);
signals:
	void terminated();
};

#endif //!_KVI_IRCCONNECTIONTARGETRESOLVER_H_
