#ifndef _KVI_IRCCONTEXT_H_
#define _KVI_IRCCONTEXT_H_
//=============================================================================
//
//   File : kvi_irccontext.h
//   Creation date : Sun 09 May 2004 20:37:46 by Szymon Stefanek
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
#include "kvi_pointerlist.h"
#include <QObject>

class KviChannel;
class KviQuery;
class KviQuery;
class KviFrame;
class KviIrcConnection;
class KviExternalServerDataParser;
class KviAsynchronousConnectionData;
class KviIrcDataStreamMonitor;
class KviWindow;
class QTimer;
class KviConsole;
class KviNetwork;
class KviServer;
/*
	KviIrcContext is the structure that groups the objects and resources
	usable for a single irc connection: a console, a set of channels and queries,
	and a KviIrcConnection object.

	The objects grouped here are mostly permanent across connections.

	Each irc context has its own unique numeric identifier. The identifiers start
	from 1 so 0 is an invalid irc context id (this is useful in the scripting engine).

	The irc context is created in the KviConsole constructor and destroyed
	in the KviConsole destructor. No other class can allocate KviIrcContext objects.
*/

class KVIRC_API KviIrcContext : public QObject
{
	friend class KviConsole;
	friend class KviChannel;
	friend class KviQuery;
	friend class KviIrcConnection;
	friend class KviIrcContextToolBar;
	Q_OBJECT
protected:
	KviIrcContext(KviConsole * pConsole); // only KviConsole can create this
public:
	~KviIrcContext();
public:
	enum State
	{
		Idle,         // connection() == 0
		Connecting,   // connection() != 0
		LoggingIn,    // connection() != 0
		Connected     // connection() != 0
	};
protected:
	KviConsole                       * m_pConsole;             // shallow, never null
	KviIrcConnection                 * m_pConnection;

	unsigned int                       m_uId;                  // this irc context id

	State                              m_eState;               // this context state

	// helper pointers
	KviFrame                         * m_pFrame;               // the owning frame, shallow, never null

	// permanent links and list window
	KviExternalServerDataParser      * m_pLinksWindow;
	KviExternalServerDataParser      * m_pListWindow;

	KviAsynchronousConnectionData    * m_pAsynchronousConnectionData; // owned, may be null
	KviAsynchronousConnectionData    * m_pSavedAsynchronousConnectionData; // owned, may be null, this is used to reconnect to the last server in this context

	unsigned int                       m_uConnectAttemptCount;
	QTimer                           * m_pReconnectTimer;

	KviPointerList<KviIrcDataStreamMonitor> * m_pMonitorList; // owned, may be null

	// dead channels and queries
	KviPointerList<KviChannel>           * m_pDeadChannels;
	KviPointerList<KviQuery>             * m_pDeadQueries;
	// other context bound windows
	KviPointerList<KviWindow>            * m_pContextWindows;

	int                                m_iHeartbeatTimerId;
public:
	inline unsigned int id(){ return m_uId; };
	// never null and always the same!
	inline KviConsole * console(){ return m_pConsole; };
	// never null and always the same
	inline KviFrame * frame(){ return m_pFrame; };
	// may be null and may change!
	inline KviIrcConnection * connection(){ return m_pConnection; };
	// state
	inline State state(){ return m_eState; };
	inline bool isConnected(){ return m_eState == Connected; };
	inline bool isLoggingIn(){ return m_eState == LoggingIn; };
	// dead channels and queries
	void closeAllDeadChannels();
	void closeAllDeadQueries();
	bool unregisterDeadChannel(KviChannel * c);
	bool unregisterDeadQuery(KviQuery * q);
	void registerDeadChannel(KviChannel * c);
	void registerDeadQuery(KviQuery * q);
	KviChannel * findDeadChannel(const QString &name);
	KviQuery * findDeadQuery(const QString &nick);
	KviQuery * firstDeadQuery();
	// other windows bound to the context
	void closeAllContextWindows();
	void registerContextWindow(KviWindow * pWnd);
	bool unregisterContextWindow(KviWindow * pWnd);

	inline KviPointerList<KviIrcDataStreamMonitor> * monitorList(){ return m_pMonitorList; };

	// links window
	void createLinksWindow();
	inline void setLinksWindowPointer(KviExternalServerDataParser * l){ m_pLinksWindow = l; };
	inline KviExternalServerDataParser * linksWindow(){ return m_pLinksWindow; };

	// list window
	void createListWindow();
	inline void setListWindowPointer(KviExternalServerDataParser * l){ m_pListWindow = l; };
	inline KviExternalServerDataParser * listWindow(){ return m_pListWindow; };

	void setAsynchronousConnectionData(KviAsynchronousConnectionData * d);
	inline KviAsynchronousConnectionData * asynchronousConnectionData(){ return m_pAsynchronousConnectionData; };
	void destroyAsynchronousConnectionData();
	// used by KviConsole (for now) and KviUserParser
	void connectToCurrentServer();

	void beginAsynchronousConnect(unsigned int uDelayInMSecs);

	void registerDataStreamMonitor(KviIrcDataStreamMonitor * m);
	void unregisterDataStreamMonitor(KviIrcDataStreamMonitor *m);

	void terminateConnectionRequest(bool bForce,const QString &szQuitMessage = QString(),bool bSimulateUnexpectedDisconnect = false);

protected:
	// called by KviIrcContextToolBar: this will DIE in favor of connectOrDisconnect()
	void connectButtonClicked();
	// used by KviConsole (for now)
	void destroyConnection();
	// for KviConsole (for now) . later will be used only internally
	void setState(State eState);
	// called by KviIrcConnection
	void loginComplete();
	// our heartbeat timer event
	virtual void timerEvent(QTimerEvent *e);
public:
	void connectOrDisconnect(){ connectButtonClicked(); };
protected:
	//
	// KviIrcConnection interface
	//
	void connectionFailed(int iError);
	void connectionEstabilished();
	void connectionTerminated();
signals:
	void stateChanged();
protected slots:
	void asynchronousConnect();
	void unhighlightAllWindows();
};


#endif //!_KVI_IRCCONTEXT_H_
