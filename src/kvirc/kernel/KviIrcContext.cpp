//=============================================================================
//
//   File : KviIrcContext.cpp
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

#include "kvi_debug.h"
#include "kvi_out.h"
#include "KviIrcContext.h"
#include "KviConsoleWindow.h"
#include "KviChannelWindow.h"
#include "KviQueryWindow.h"
#include "KviMainWindow.h"
#include "KviIrcLink.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionTarget.h"
#include "KviAsynchronousConnectionData.h"
#include "KviIrcConnectionStateData.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviIrcConnectionRequestQueue.h"
#include "KviIrcToolBar.h"
#include "KviIrcServerDataBase.h"
#include "KviIrcServerReconnectInfo.h"
#include "KviIrcServerParser.h"
#include "KviProxyDataBase.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "KviIrcDataStreamMonitor.h"
#include "KviError.h"
#include "KviThread.h" // for KviThread::msleep()
#include "KviApplication.h"
#include "KviKvsEventTriggers.h"
#include "KviKvsScript.h"
#include "KviNetUtils.h"
#include "KviIrcUrl.h"
#include "KviUserIdentity.h"
#include "KviUserIdentityManager.h"
#include "KviPtrListIterator.h"
#include "KviIrcNetwork.h"

#include "kvi_debug.h"

#include <QTimer>
#include <QByteArray>
#include <algorithm>

// the irc context identifiers start from 1
static unsigned int g_uNextIrcContextId = 1;

extern KVIRC_API KviIrcServerDataBase * g_pServerDataBase;
extern KVIRC_API KviProxyDataBase * g_pProxyDataBase;

KviIrcContext::KviIrcContext(KviConsoleWindow * pConsole)
    : QObject(), m_pConsole(pConsole)
{
	m_uId = g_uNextIrcContextId;
	g_uNextIrcContextId++;

	m_iHeartbeatTimerId = startTimer(5000);
}

KviIrcContext::~KviIrcContext()
{
	killTimer(m_iHeartbeatTimerId);

	for(auto & m : m_pMonitorList)
	{
		if(m)
			m->die();
	}

	delete m_pReconnectTimer;

	if(m_pLinksWindow)
		m_pLinksWindow->die();
	if(m_pListWindow)
		m_pListWindow->die();

	closeAllDeadChannels();
	closeAllDeadQueries();
	closeAllContextWindows();

	destroyConnection();
	delete m_pAsynchronousConnectionData;
	delete m_pSavedAsynchronousConnectionData;
}

void KviIrcContext::registerDataStreamMonitor(KviIrcDataStreamMonitor * m)
{
	m_pMonitorList.push_back(m);
}

void KviIrcContext::unregisterDataStreamMonitor(KviIrcDataStreamMonitor * m)
{
	if(m_pMonitorList.empty())
		return;

	m_pMonitorList.erase(std::remove(m_pMonitorList.begin(), m_pMonitorList.end(), m), m_pMonitorList.end());
}

void KviIrcContext::closeAllDeadChannels()
{
	while(!m_DeadChannels.empty())
	{
		KviChannelWindow * chan = static_cast<KviChannelWindow *>(m_DeadChannels.front());
		g_pMainWindow->closeWindow(chan);
	}
}

void KviIrcContext::closeAllDeadQueries()
{
	while(!m_DeadQueries.empty())
	{
		KviQueryWindow * qWin = static_cast<KviQueryWindow *>(m_DeadQueries.front());
		g_pMainWindow->closeWindow(qWin);
	}
}

void KviIrcContext::closeAllContextWindows()
{
	while(!m_ContextWindows.empty())
	{
		KviWindow * wWin = static_cast<KviWindow *>(m_ContextWindows.front());
		g_pMainWindow->closeWindow(wWin);
	}
}

KviChannelWindow * KviIrcContext::findDeadChannel(const QString & name)
{
	for(auto & c : m_DeadChannels)
	{
		KVI_ASSERT(c->isDeadChan());
		if(KviQString::equalCI(name, c->windowName()))
			return c;
	}

	return nullptr;
}

KviQueryWindow * KviIrcContext::findDeadQuery(const QString & name)
{
	for(auto & q : m_DeadQueries)
	{
		KVI_ASSERT(q->isDeadQuery());
		if(KviQString::equalCI(name, q->windowName()))
			return q;
	}

	return nullptr;
}

KviChannelWindow * KviIrcContext::firstDeadChannel()
{
	if(m_DeadChannels.empty())
		return nullptr;

	return m_DeadChannels.front();
}

KviQueryWindow * KviIrcContext::firstDeadQuery()
{
	if(m_DeadQueries.empty())
		return nullptr;

	return m_DeadQueries.front();
}

void KviIrcContext::registerContextWindow(KviWindow * pWnd)
{
	m_ContextWindows.push_back(pWnd);
}

void KviIrcContext::registerDeadChannel(KviChannelWindow * c)
{
	m_DeadChannels.push_back(c);
}

void KviIrcContext::registerDeadQuery(KviQueryWindow * q)
{
	m_DeadQueries.push_back(q);
}

bool KviIrcContext::unregisterDeadChannel(KviChannelWindow * c)
{
	// was a dead channel ?
	if(m_DeadChannels.empty())
		return false;

	auto result = std::find(m_DeadChannels.begin(), m_DeadChannels.end(), c);

	if(result != m_DeadChannels.end())
	{
		m_DeadChannels.erase(result);
		return true;
	}

	return false;
}

bool KviIrcContext::unregisterContextWindow(KviWindow * pWnd)
{
	if(m_ContextWindows.empty())
		return false;

	auto result = std::find(m_ContextWindows.begin(), m_ContextWindows.end(), pWnd);

	if(result != m_ContextWindows.end())
	{
		m_ContextWindows.erase(result);
		return true;
	}

	return false;
}

bool KviIrcContext::unregisterDeadQuery(KviQueryWindow * q)
{
	if(m_DeadQueries.empty())
		return false;

	auto result = std::find(m_DeadQueries.begin(), m_DeadQueries.end(), q);

	if(result != m_DeadQueries.end())
	{
		m_DeadQueries.erase(result);
		return true;
	}

	return false;
}

void KviIrcContext::createLinksWindow()
{
	if(m_pLinksWindow)
		return;
	KviKvsScript::run("links.open", m_pConsole);
}

void KviIrcContext::createListWindow()
{
	if(m_pListWindow)
		return;
	KviKvsScript::run("list.open", m_pConsole);
}

void KviIrcContext::destroyConnection()
{
	if(!m_pConnection)
		return;
	m_pConnection->closeAllChannels();
	m_pConnection->closeAllQueries();

	if(m_pLinksWindow)
		m_pLinksWindow->control(EXTERNAL_SERVER_DATA_PARSER_CONTROL_RESET);
	if(m_pListWindow)
		m_pListWindow->control(EXTERNAL_SERVER_DATA_PARSER_CONTROL_RESET);

	m_pConsole->connectionDetached();

	// make sure that m_pConnection is already nullptr in any
	// event triggered by KviIrcConnection destructor
	KviIrcConnection * pTmp = m_pConnection;
	m_pConnection = nullptr;
	delete pTmp;
}

void KviIrcContext::setState(State eState)
{
	if(m_eState == eState)
		return;
	m_eState = eState;

	g_pMainWindow->childContextStateChange(this);

	emit stateChanged();

	if(eState == KviIrcContext::Idle || eState == KviIrcContext::PendingReconnection)
		destroyConnection();

	m_pConsole->updateCaption();
}

void KviIrcContext::setAsynchronousConnectionData(KviAsynchronousConnectionData * d)
{
	if(m_pAsynchronousConnectionData)
		delete m_pAsynchronousConnectionData;
	m_pAsynchronousConnectionData = d;
}

void KviIrcContext::destroyAsynchronousConnectionData()
{
	if(!m_pAsynchronousConnectionData)
		return;
	delete m_pAsynchronousConnectionData;
	m_pAsynchronousConnectionData = nullptr;
}

void KviIrcContext::loginComplete()
{
	setState(Connected);
}

void KviIrcContext::connectButtonClicked()
{
	if(!connection())
	{
		if(m_pReconnectTimer)
		{
			// reconnection was in progress...
			delete m_pReconnectTimer;
			m_pReconnectTimer = nullptr;
			destroyAsynchronousConnectionData();

			m_pConsole->outputNoFmt(KVI_OUT_SYSTEMERROR,
			    __tr2qs("Reconnect attempt aborted"));

			if(m_eState == KviIrcContext::PendingReconnection)
				setState(Idle);

			return;
		}

		// No connections in progress
		m_uConnectAttemptCount = 1;
		connectToCurrentServer();
	}
	else
	{
		// Sth is going on
		terminateConnectionRequest(false);
	}
}

void KviIrcContext::connectToCurrentServer()
{
	if(m_pReconnectTimer)
	{
		delete m_pReconnectTimer;
		m_pReconnectTimer = nullptr;
	}

	m_pConsole->outputNoFmt(KVI_OUT_SYSTEMMESSAGE, " "); // spacer

	// No connection target specified.
	// If we have a saved target, reuse it
	if(!m_pAsynchronousConnectionData && m_pSavedAsynchronousConnectionData)
	{
		m_pAsynchronousConnectionData = m_pSavedAsynchronousConnectionData;
		m_pSavedAsynchronousConnectionData = nullptr;
	}

	if(m_pAsynchronousConnectionData)
	{
		// we have a specified connection target (either from outside or saved)
		if(m_pAsynchronousConnectionData->szServer.isEmpty())
		{
			// an empty server might mean "reuse the last server in context"
			if(m_pAsynchronousConnectionData->bUseLastServerInContext && m_pSavedAsynchronousConnectionData)
			{
				// reuse the saved connection data
				// the server for sure
				m_pAsynchronousConnectionData->szServer = m_pSavedAsynchronousConnectionData->szServer;
				m_pAsynchronousConnectionData->szServerId = m_pSavedAsynchronousConnectionData->szServerId;
				m_pAsynchronousConnectionData->uPort = m_pSavedAsynchronousConnectionData->uPort;
				m_pAsynchronousConnectionData->bPortIsOk = true;
				m_pAsynchronousConnectionData->bUseIPv6 = m_pSavedAsynchronousConnectionData->bUseIPv6;
				m_pAsynchronousConnectionData->bUseSSL = m_pSavedAsynchronousConnectionData->bUseSSL;
				m_pAsynchronousConnectionData->bSTARTTLS = m_pSavedAsynchronousConnectionData->bSTARTTLS;

				if(m_pSavedAsynchronousConnectionData->m_pReconnectInfo)
					m_pAsynchronousConnectionData->m_pReconnectInfo = new KviIrcServerReconnectInfo(*(m_pSavedAsynchronousConnectionData->m_pReconnectInfo));
				else
					m_pAsynchronousConnectionData->m_pReconnectInfo = nullptr;

				// and the other info, only if not overridden by the user
				if(m_pAsynchronousConnectionData->szBindAddress.isEmpty())
					m_pAsynchronousConnectionData->szBindAddress = m_pSavedAsynchronousConnectionData->szBindAddress;
				if(m_pAsynchronousConnectionData->szCommandToExecAfterConnect.isEmpty())
					m_pAsynchronousConnectionData->szCommandToExecAfterConnect = m_pSavedAsynchronousConnectionData->szCommandToExecAfterConnect;
				if(m_pAsynchronousConnectionData->szLinkFilter.isEmpty())
					m_pAsynchronousConnectionData->szLinkFilter = m_pSavedAsynchronousConnectionData->szLinkFilter;
				if(m_pAsynchronousConnectionData->szPass.isEmpty())
					m_pAsynchronousConnectionData->szPass = m_pSavedAsynchronousConnectionData->szPass;
				if(m_pAsynchronousConnectionData->szNick.isEmpty())
					m_pAsynchronousConnectionData->szNick = m_pSavedAsynchronousConnectionData->szNick;
				if(m_pAsynchronousConnectionData->szInitUMode.isEmpty())
					m_pAsynchronousConnectionData->szInitUMode = m_pSavedAsynchronousConnectionData->szInitUMode;
			}
			else
			{
				m_pConsole->outputNoFmt(KVI_OUT_SYSTEMWARNING, __tr2qs("This is the first connection in this IRC context: using the global server setting"));
			}
		}

		if(!m_pAsynchronousConnectionData->szServer.isEmpty())
		{
			// !m_pAsynchronousConnectionData->szServer.isEmpty()
			// ok, have a server to look for in the db
			// FIXME: this is a bit ugly... could it be managed in some completely different and nicer way ?
			KviIrcServerDefinition d;
			d.szServer = m_pAsynchronousConnectionData->szServer;
			d.bPortIsValid = m_pAsynchronousConnectionData->bPortIsOk;
			d.uPort = m_pAsynchronousConnectionData->uPort;
			d.bIPv6 = m_pAsynchronousConnectionData->bUseIPv6;
			d.bSSL = m_pAsynchronousConnectionData->bUseSSL;
			d.bSTARTTLS = m_pAsynchronousConnectionData->bSTARTTLS;
			d.szLinkFilter = m_pAsynchronousConnectionData->szLinkFilter;
			d.szPass = m_pAsynchronousConnectionData->szPass;
			d.szNick = m_pAsynchronousConnectionData->szNick;
			d.szInitUMode = m_pAsynchronousConnectionData->szInitUMode;
			d.szId = m_pAsynchronousConnectionData->szServerId;
			QString szError;
			if(!g_pServerDataBase->makeCurrentServer(&d, szError))
			{
				m_pConsole->outputNoFmt(KVI_OUT_SYSTEMERROR, szError);
				destroyAsynchronousConnectionData();
				return;
			}
		} // else we just connect to the globally selected IRC server in the options dialog
	}

	KviIrcNetwork * net = g_pServerDataBase->currentNetwork();
	KviIrcServer * srv = net ? net->currentServer() : nullptr;

	if(!srv)
	{
		if(g_pServerDataBase->networkCount())
			KviKvsScript::run("options.edit -n OptionsWidget_servers", m_pConsole);
		else
			m_pConsole->outputNoFmt(KVI_OUT_SYSTEMERROR, __tr2qs("No servers available. Check the options dialog or use the /SERVER command"));
		destroyAsynchronousConnectionData();
		return;
	}

	KviProxy * prx = srv->proxyServer(g_pProxyDataBase);

	if(!prx && (srv->proxy() != -1) && KVI_OPTION_BOOL(KviOption_boolUseProxyHost))
	{
		prx = g_pProxyDataBase->currentProxy();
		if(!prx)
		{
			m_pConsole->outputNoFmt(KVI_OUT_SYSTEMWARNING, __tr2qs("No proxy hosts available, resuming direct connection"));
		}
	}

	QString szBindAddress;

	srv->clearReconnectInfo();
	if(m_pAsynchronousConnectionData)
	{
		szBindAddress = m_pAsynchronousConnectionData->szBindAddress;
		if(m_pAsynchronousConnectionData->m_pReconnectInfo)
			srv->setReconnectInfo(new KviIrcServerReconnectInfo(*(m_pAsynchronousConnectionData->m_pReconnectInfo)));
	}

	// Find out the identity we'll be using in this connection
	// First check the server for one

	const KviUserIdentity * pIdentity = nullptr;

	QString szUserIdentityId = srv->userIdentityId();
	if(!szUserIdentityId.isEmpty())
		pIdentity = KviUserIdentityManager::instance()->findIdentity(szUserIdentityId);

	// If not found, look in the network instead
	if(!pIdentity)
		szUserIdentityId = net->userIdentityId();

	if(!szUserIdentityId.isEmpty())
		pIdentity = KviUserIdentityManager::instance()->findIdentity(szUserIdentityId);

	// If not found, get the default identity (this is GRANTED to be never null, eventually filled up with defaults)
	pIdentity = KviUserIdentityManager::instance()->defaultIdentity();

	if(m_pConnection)
		delete m_pConnection;

	m_pConnection = new KviIrcConnection(
	    this,
	    new KviIrcConnectionTarget(
	        net,
	        srv,
	        prx,
	        szBindAddress),
	    new KviUserIdentity(*pIdentity));

	setState(Connecting);

	if(m_pAsynchronousConnectionData)
	{
		m_pConnection->stateData()->setCommandToExecAfterConnect(m_pAsynchronousConnectionData->szCommandToExecAfterConnect);
		destroyAsynchronousConnectionData();
	}

	m_pConsole->connectionAttached();

	// save stuff for later

	// FIXME: this management of "next" connection should be reviewed a bit anyway
	if(m_pSavedAsynchronousConnectionData)
		delete m_pSavedAsynchronousConnectionData;

	m_pSavedAsynchronousConnectionData = new KviAsynchronousConnectionData();
	m_pSavedAsynchronousConnectionData->szServer = srv->hostName();
	m_pSavedAsynchronousConnectionData->uPort = srv->port();
	m_pSavedAsynchronousConnectionData->bPortIsOk = true;
	m_pSavedAsynchronousConnectionData->bUseIPv6 = srv->isIPv6();
	m_pSavedAsynchronousConnectionData->bUseSSL = srv->useSSL();
	m_pSavedAsynchronousConnectionData->bSTARTTLS = srv->enabledSTARTTLS();
	m_pSavedAsynchronousConnectionData->szPass = srv->password();
	m_pSavedAsynchronousConnectionData->szServerId = srv->id();
	m_pSavedAsynchronousConnectionData->szInitUMode = srv->initUMode();

	if(srv->reconnectInfo())
		m_pSavedAsynchronousConnectionData->m_pReconnectInfo = new KviIrcServerReconnectInfo(*(srv->reconnectInfo()));
	else
		m_pSavedAsynchronousConnectionData->m_pReconnectInfo = nullptr;

	// this never fails!
	m_pConnection->start();
}

void KviIrcContext::connectionFailed(int iError)
{
	if(!m_pConnection)
		return; // this may happen in the destructor!

	m_pConsole->output(KVI_OUT_SYSTEMERROR,
	    __tr2qs("Connection attempt failed [%s]"),
	    m_pConnection->target()->server()->hostName().toUtf8().data());

	// if the connection has been aborted by the user then just go idle
	if(iError == KviError::OperationAborted)
		goto enter_idle_state;

	// FIXME: this should stop on critical errors !
	if(KVI_OPTION_BOOL(KviOption_boolAutoReconnectOnUnexpectedDisconnect))
	{
		if((!KVI_OPTION_UINT(KviOption_uintMaxAutoReconnectAttempts) || (m_uConnectAttemptCount < KVI_OPTION_UINT(KviOption_uintMaxAutoReconnectAttempts))))
		{
			m_uConnectAttemptCount++;
			//FIXME: Multiply the delay by (m_uConnectAttemptCount / 2) so later connects are less frequent.
			if(!_OUTPUT_MUTE)
			{
				QString szTmp = QString(__tr2qs("Will attempt to reconnect in %1 seconds")).arg(KVI_OPTION_UINT(KviOption_uintAutoReconnectDelay));
				QString szNum;

				if(!KVI_OPTION_UINT(KviOption_uintMaxAutoReconnectAttempts))
					szNum = QString(__tr2qs("%1")).arg(m_uConnectAttemptCount);
				else
					szNum = QString(__tr2qs("%1 of %2")).arg(m_uConnectAttemptCount).arg(KVI_OPTION_UINT(KviOption_uintMaxAutoReconnectAttempts));
				szTmp += " [" + szNum + "]";
				m_pConsole->outputNoFmt(KVI_OUT_SYSTEMMESSAGE, szTmp);
			}

			KviIrcServer oldServer(*(connection()->target()->server()));
			QString oldNickname = connection()->userInfo()->isAway() ? connection()->userInfo()->nickNameBeforeAway() : connection()->userInfo()->nickName();

			KviAsynchronousConnectionData * d = new KviAsynchronousConnectionData();
			d->szServer = oldServer.hostName();
			d->szServerId = oldServer.id();
			d->uPort = oldServer.port();
			d->bPortIsOk = true;
			d->bUseIPv6 = oldServer.isIPv6();
			d->bUseSSL = oldServer.useSSL();
			d->bSTARTTLS = oldServer.enabledSTARTTLS();
			d->szPass = oldServer.password();
			d->szNick = oldNickname;
			d->szInitUMode = oldServer.initUMode();
			d->szCommandToExecAfterConnect = "";
			setAsynchronousConnectionData(d);
			beginAsynchronousConnect(1000 * KVI_OPTION_UINT(KviOption_uintAutoReconnectDelay));

			setState(PendingReconnection); // destroy the actual connection

			return;
		}
		else
		{
			if(!_OUTPUT_MUTE)
				m_pConsole->output(KVI_OUT_SYSTEMERROR, __tr2qs("Maximum number of reconnect attempts reached (%d): giving up"), KVI_OPTION_UINT(KviOption_uintMaxAutoReconnectAttempts));
		}
	}

	// reset the attempt count
	m_uConnectAttemptCount = 1;

	if(connection()->target()->server()->cacheIp())
	{
		if((((int)iError) == KviError::ConnectionTimedOut) || (((int)iError) == KviError::ConnectionRefused) || (((int)iError) == KviError::NetworkUnreachable) || (((int)iError) == KviError::HostUnreachable))
		{
			m_pConsole->output(KVI_OUT_SYSTEMWARNING, __tr2qs("The connection attempt failed while using a cached IP address for the current server"));
			m_pConsole->output(KVI_OUT_SYSTEMWARNING, __tr2qs("The problem *might* be caused by an updated DNS entry"));
			m_pConsole->output(KVI_OUT_SYSTEMWARNING, __tr2qs("Try reconnecting with caching disabled"));
		}
	}

enter_idle_state:
	setState(Idle);
}

void KviIrcContext::connectionEstablished()
{
	//qDebug("context::connectionEstablished");
	//
	// The connection has been established, the
	// KviIrcConnection will attempt to login now
	//
	m_uConnectAttemptCount = 1;

	setState(LoggingIn); // this must be set in order for $server and other functions to return the correct values

	bool bStopOutput = KVS_TRIGGER_EVENT_0_HALTED(KviEvent_OnIRCConnectionEstablished, m_pConsole);

	if(!bStopOutput)
	{
		m_pConsole->output(KVI_OUT_CONNECTION, __tr2qs("%Q established [%s (%s:%u)]"),
		    connection()->link()->socket()->usingSSL() ? &(__tr2qs("Secure connection")) : &(__tr2qs("Connection")),
		    connection()->target()->server()->hostName().toUtf8().data(),
		    connection()->target()->server()->ip().toUtf8().data(),
		    connection()->target()->server()->port());
	}

	// Add to recent server list (build the URL of type irc[6]://<server>:<port>
	QString url;
	KviIrcUrl::join(url, connection()->target()->server());
	g_pApp->addRecentServer(url);

	// save the last server this console used
	//if(m_pLastIrcServer)delete m_pLastIrcServer;
	//m_pLastIrcServer = new KviIrcServer(*(connection()->server()));
}

void KviIrcContext::connectionTerminated()
{
	if(!m_pConnection)
		return; // this may happen in the destructor!

	KviIrcServer oldServer(*(connection()->target()->server()));

	oldServer.clearReconnectInfo();

	KviIrcServerReconnectInfo info;

	info.m_szNick = connection()->userInfo()->isAway() ? connection()->userInfo()->nickNameBeforeAway() : connection()->userInfo()->nickName();
	info.m_szPass = connection()->userInfo()->password();
	info.m_bIsAway = connection()->userInfo()->isAway();
	info.m_szAwayReason = connection()->userInfo()->awayReason();

	connection()->requestQueue()->clearAll();

	// we consider it unexpected when we haven't sent a QUIT message and we're connected
	// or alternatively when a simulation of such a termination is requested (this is used to keep the queries open etc.)
	bool bUnexpectedDisconnect = ((!(connection()->stateData()->sentQuit())) && ((m_eState == KviIrcContext::Connected) || (m_eState == KviIrcContext::Connecting) || (m_eState == KviIrcContext::LoggingIn))) || connection()->stateData()->simulateUnexpectedDisconnect();

	if(bUnexpectedDisconnect)
	{
		if(KVI_OPTION_BOOL(KviOption_boolAutoReconnectOnUnexpectedDisconnect))
		{
			if(KVI_OPTION_BOOL(KviOption_boolRejoinChannelsAfterReconnect))
				connection()->gatherChannelAndPasswordPairs(info.m_lJoinChannels);

			if(KVI_OPTION_BOOL(KviOption_boolReopenQueriesAfterReconnect))
				connection()->gatherQueryNames(info.m_lOpenQueries);
		}

		if(KVI_OPTION_BOOL(KviOption_boolKeepChannelsOpenOnUnexpectedDisconnect) || KVI_OPTION_BOOL(KviOption_boolKeepChannelsOpenOnDisconnect))
			connection()->keepChannelsOpenAfterDisconnect();

		if(KVI_OPTION_BOOL(KviOption_boolKeepQueriesOpenOnUnexpectedDisconnect) || KVI_OPTION_BOOL(KviOption_boolKeepQueriesOpenOnDisconnect))
			connection()->keepQueriesOpenAfterDisconnect();
	}
	else
	{
		if(KVI_OPTION_BOOL(KviOption_boolKeepChannelsOpenOnDisconnect))
			connection()->keepChannelsOpenAfterDisconnect();

		if(KVI_OPTION_BOOL(KviOption_boolKeepQueriesOpenOnDisconnect))
			connection()->keepQueriesOpenAfterDisconnect();
	}

	bool bStopOutput = false;

	bStopOutput = KVS_TRIGGER_EVENT_0_HALTED(KviEvent_OnIRCConnectionTerminated, m_pConsole);

	if(!bStopOutput)
	{
		m_pConsole->output(KVI_OUT_CONNECTION, __tr2qs("Connection terminated [%s (%s:%u)]"),
		    oldServer.hostName().toUtf8().data(),
		    oldServer.ip().toUtf8().data(),
		    oldServer.port());
	}

	// do reconnect

	if(bUnexpectedDisconnect && KVI_OPTION_BOOL(KviOption_boolAutoReconnectOnUnexpectedDisconnect))
	{
		setState(PendingReconnection);

		//m_uConnectAttemptCount = 1;
		if(!_OUTPUT_MUTE)
			m_pConsole->output(KVI_OUT_CONNECTION, __tr2qs("The connection terminated unexpectedly. Trying to reconnect..."));

		KviAsynchronousConnectionData * d = new KviAsynchronousConnectionData();
		d->szServerId = oldServer.id();
		d->szServer = oldServer.hostName();
		d->uPort = oldServer.port();
		d->bPortIsOk = true;
		d->bUseIPv6 = oldServer.isIPv6();
		d->bUseSSL = oldServer.useSSL();
		d->bSTARTTLS = oldServer.enabledSTARTTLS();
		d->szPass = oldServer.password();
		d->szInitUMode = oldServer.initUMode();
		d->m_pReconnectInfo = new KviIrcServerReconnectInfo(info);

		setAsynchronousConnectionData(d);

		beginAsynchronousConnect(1000 * KVI_OPTION_UINT(KviOption_uintAutoReconnectDelay));
	}
	else
	{
		setState(Idle);
	}
}

void KviIrcContext::beginAsynchronousConnect(unsigned int uDelayInMSecs)
{
	if(m_pReconnectTimer)
		delete m_pReconnectTimer;
	m_pReconnectTimer = new QTimer(this);
	connect(m_pReconnectTimer, SIGNAL(timeout()), this, SLOT(asynchronousConnect()));
	m_pReconnectTimer->start(uDelayInMSecs);
}

void KviIrcContext::asynchronousConnect()
{
	if(m_pReconnectTimer)
	{
		delete m_pReconnectTimer;
		m_pReconnectTimer = nullptr;
	}

	if(state() != Idle) // need a brutal disconnect here
		terminateConnectionRequest(true, "Changing server...");

	connectToCurrentServer();
}

void KviIrcContext::terminateConnectionRequest(bool bForce, const QString & szQuitMsg, bool bSimulateUnexpectedDisconnect)
{
	if(!connection())
		return; // hm ?

	connection()->stateData()->setSimulateUnexpectedDisconnect(bSimulateUnexpectedDisconnect);

	switch(m_eState)
	{
		case Connected:
		{
			// was connected : send a quit and abort the connection
			bool bQuitSentJustNow = false;

			if(!connection()->stateData()->sentQuit())
			{
				KVS_TRIGGER_EVENT_0(KviEvent_OnDisconnectRequest, m_pConsole);
				QString szQuit = szQuitMsg;
				if(szQuit.isEmpty())
					szQuit = KVI_OPTION_STRING(KviOption_stringQuitMessage);
				KviQString::escapeKvs(&szQuit, KviQString::PermitVariables | KviQString::PermitFunctions);
				QString buffer;
				KviKvsVariant ret;
				if(KviKvsScript::evaluate(szQuit, console(), nullptr, &ret))
					ret.asString(buffer);
				else
					buffer = szQuit;
				QByteArray dat = connection()->encodeText(buffer);
				connection()->stateData()->setSentQuit();
				connection()->sendFmtData("QUIT :%s", dat.data() ? dat.data() : ""); // here theoretically we COULD get disconnected
				bQuitSentJustNow = true;
			} // else it was already sent anyway

			if(KVI_OPTION_BOOL(KviOption_boolForceBrutalQuit) || bForce || (!bQuitSentJustNow))
			{
				if(!bQuitSentJustNow)
				{
					// idle for some milliseconds in order to allow the quit message to reach
					// the remote end without breaking the connection
					KviThread::msleep(100);
				}

				// and brutally abort the connection (if it still exists!!!)
				if(connection())
					connection()->abort();
			}
			else
			{
				if(bQuitSentJustNow)
					m_pConsole->outputNoFmt(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Sent QUIT, waiting for the server to close the connection..."));
			}
		}
		break;
		case PendingReconnection:
		case Connecting:
		case LoggingIn:
			// was waiting for connection or login, just abort it: it will trigger an error anyway
			// though act as if we sent the quit message, so we'll not treat the disconnection as "unexpected".
			connection()->stateData()->setSentQuit();
			connection()->abort();
			break;
		default:
			// should never end here!
			KVI_ASSERT(false);
			break;
	}
}

void KviIrcContext::timerEvent(QTimerEvent * e)
{
	if(e->timerId() != m_iHeartbeatTimerId)
	{
		QObject::timerEvent(e);
		return;
	}

	// our heartbeat
	kvi_time_t tNow = kvi_unixTime();

	if(m_pConnection)
		m_pConnection->heartbeat(tNow);
}
