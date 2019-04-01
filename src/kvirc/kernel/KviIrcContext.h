#ifndef _KVI_IRCCONTEXT_H_
#define _KVI_IRCCONTEXT_H_
//=============================================================================
//
//   File : KviIrcContext.h
//   Creation date : Sun 09 May 2004 20:37:46 by Szymon Stefanek
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

#include <QObject>
#include <vector>

class KviChannelWindow;
class KviQueryWindow;
class KviQueryWindow;
class KviMainWindow;
class KviIrcConnection;
class KviExternalServerDataParser;
class KviAsynchronousConnectionData;
class KviIrcDataStreamMonitor;
class KviWindow;
class QTimer;
class KviConsoleWindow;
class KviIrcNetwork;
class KviIrcServer;
/*
	KviIrcContext is the structure that groups the objects and resources
	usable for a single irc connection: a console, a set of channels and queries,
	and a KviIrcConnection object.

	The objects grouped here are mostly permanent across connections.

	Each irc context has its own unique numeric identifier. The identifiers start
	from 1 so 0 is an invalid irc context id (this is useful in the scripting engine).

	The irc context is created in the KviConsoleWindow constructor and destroyed
	in the KviConsoleWindow destructor. No other class can allocate KviIrcContext objects.
*/

class KVIRC_API KviIrcContext : public QObject
{
	friend class KviConsoleWindow;
	friend class KviChannelWindow;
	friend class KviQueryWindow;
	friend class KviIrcConnection;
	friend class KviIrcContextToolBar;
	Q_OBJECT
protected:
	KviIrcContext(KviConsoleWindow * pConsole); // only KviConsoleWindow can create this
public:
	~KviIrcContext();

public:
	/* If you add further states, remember to  update the
	 * bUnexpectedDisconnect determination in KviIrcContext::connectionTerminated */
	enum State
	{
		Idle,                // connection() == 0
		PendingReconnection, // connection() == 0
		Connecting,          // connection() != 0
		LoggingIn,           // connection() != 0
		Connected            // connection() != 0
	};

protected:
	KviConsoleWindow * m_pConsole; // shallow, never null
	KviIrcConnection * m_pConnection = nullptr;

	unsigned int m_uId; // this irc context id

	State m_eState = Idle; // this context state

	// permanent links and list window
	KviExternalServerDataParser * m_pLinksWindow = nullptr;
	KviExternalServerDataParser * m_pListWindow = nullptr;

	KviAsynchronousConnectionData * m_pAsynchronousConnectionData = nullptr;      // owned, may be null
	KviAsynchronousConnectionData * m_pSavedAsynchronousConnectionData = nullptr; // owned, may be null, this is used to reconnect to the last server in this context

	unsigned int m_uConnectAttemptCount = 1;
	QTimer * m_pReconnectTimer = nullptr;

	std::vector<KviIrcDataStreamMonitor *> m_pMonitorList; // owned, may be empty

	// dead channels and queries
	std::vector<KviChannelWindow *> m_DeadChannels;
	std::vector<KviQueryWindow *> m_DeadQueries;
	// other context bound windows
	std::vector<KviWindow *> m_ContextWindows;

	int m_iHeartbeatTimerId;

public:
	unsigned int id() const { return m_uId; }
	// never null and always the same!
	KviConsoleWindow * console() const { return m_pConsole; }
	// may be null and may change!
	KviIrcConnection * connection() const { return m_pConnection; }
	// state
	State state() const { return m_eState; }
	bool isConnected() const { return m_eState == Connected; }
	bool isLoggingIn() const { return m_eState == LoggingIn; }
	// dead channels and queries
	bool unregisterDeadChannel(KviChannelWindow * c);
	bool unregisterDeadQuery(KviQueryWindow * q);
	void registerDeadChannel(KviChannelWindow * c);
	void registerDeadQuery(KviQueryWindow * q);
	KviChannelWindow * findDeadChannel(const QString & name);
	KviQueryWindow * findDeadQuery(const QString & nick);
	KviQueryWindow * firstDeadQuery();
	KviChannelWindow * firstDeadChannel();
	// other windows bound to the context
	void closeAllContextWindows();
	void registerContextWindow(KviWindow * pWnd);
	bool unregisterContextWindow(KviWindow * pWnd);

	std::vector<KviIrcDataStreamMonitor *> & monitorList() { return m_pMonitorList; }

	// links window
	void createLinksWindow();
	void setLinksWindowPointer(KviExternalServerDataParser * l) { m_pLinksWindow = l; }
	KviExternalServerDataParser * linksWindow() const { return m_pLinksWindow; }

	// list window
	void createListWindow();
	void setListWindowPointer(KviExternalServerDataParser * l) { m_pListWindow = l; }
	KviExternalServerDataParser * listWindow() const { return m_pListWindow; }

	void setAsynchronousConnectionData(KviAsynchronousConnectionData * d);
	KviAsynchronousConnectionData * asynchronousConnectionData() const { return m_pAsynchronousConnectionData; }
	void destroyAsynchronousConnectionData();
	// used by KviConsoleWindow (for now) and KviUserParser
	void connectToCurrentServer();

	void beginAsynchronousConnect(unsigned int uDelayInMSecs);

	void registerDataStreamMonitor(KviIrcDataStreamMonitor * m);
	void unregisterDataStreamMonitor(KviIrcDataStreamMonitor * m);

	void terminateConnectionRequest(bool bForce, const QString & szQuitMessage = QString(), bool bSimulateUnexpectedDisconnect = false);
public slots:
	void closeAllDeadChannels();
	void closeAllDeadQueries();

protected:
	// called by KviIrcContextToolBar: this will DIE in favor of connectOrDisconnect()
	void connectButtonClicked();
	// used by KviConsoleWindow (for now)
	void destroyConnection();
	// for KviConsoleWindow (for now) . later will be used only internally
	void setState(State eState);
	// called by KviIrcConnection
	void loginComplete();
	// our heartbeat timer event
	void timerEvent(QTimerEvent * e) override;

public:
	void connectOrDisconnect() { connectButtonClicked(); }
protected:
	//
	// KviIrcConnection interface
	//
	void connectionFailed(int iError);
	void connectionEstablished();
	void connectionTerminated();
signals:
	void stateChanged();
protected slots:
	void asynchronousConnect();
};

#endif //!_KVI_IRCCONTEXT_H_
