//=============================================================================
//
//   File : kvi_irccontext.cpp
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



#include "kvi_irccontext.h"
#include "kvi_console.h"
#include "kvi_channel.h"
#include "kvi_query.h"
#include "kvi_frame.h"
#include "kvi_debug.h"
#include "kvi_sparser.h"
#include "kvi_irclink.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnectiontarget.h"
#include "kvi_asynchronousconnectiondata.h"
#include "kvi_ircconnectionstatedata.h"
#include "kvi_ircconnectionuserinfo.h"
#include "kvi_ircconnectionrequestqueue.h"
#include "kvi_irctoolbar.h"
#include "kvi_out.h"
#include "kvi_ircserverdb.h"
#include "kvi_proxydb.h"
#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_ircdatastreammonitor.h"
#include "kvi_error.h"
#include "kvi_thread.h" // for KviThread::msleep()
#include "kvi_app.h"
#include "kvi_kvs_eventtriggers.h"
#include "kvi_kvs_script.h"
#include "kvi_netutils.h"
#include "kvi_ircurl.h"
#include "kvi_useridentity.h"

#define __KVI_DEBUG__
#include "kvi_debug.h"

#include <QTimer>
#include <QByteArray>

// the irc context identifiers start from 1
static unsigned int g_uNextIrcContextId = 1;

extern KVIRC_API KviServerDataBase * g_pServerDataBase;
extern KVIRC_API KviProxyDataBase * g_pProxyDataBase;


KviIrcContext::KviIrcContext(KviConsole * pConsole)
: QObject(0)
{
	m_uId = g_uNextIrcContextId;
	g_uNextIrcContextId++;

	m_pConsole = pConsole;

	m_pConnection = 0;

	m_pFrame = m_pConsole->frame();

	m_pDeadChannels = 0;
	m_pDeadQueries = 0;
	m_pContextWindows = 0;

	m_pLinksWindow = 0;
	m_pListWindow = 0;

	m_eState = Idle;

	m_pAsynchronousConnectionData = 0;
	m_pSavedAsynchronousConnectionData = 0;
	m_uConnectAttemptCount = 0;
	m_pMonitorList = 0;
	m_pReconnectTimer = 0;

	m_uConnectAttemptCount = 1;

	m_iHeartbeatTimerId = startTimer(5000);
}

KviIrcContext::~KviIrcContext()
{
	killTimer(m_iHeartbeatTimerId);

	while(m_pMonitorList)
	{
		KviIrcDataStreamMonitor * m = m_pMonitorList->first();
		if(m)m->die();
		else {
			delete m_pMonitorList;
			m_pMonitorList = 0;
		}
	}

	if(m_pReconnectTimer)delete m_pReconnectTimer;

	if(m_pLinksWindow)m_pLinksWindow->die();
	if(m_pListWindow)m_pListWindow->die();

	closeAllDeadChannels();
	closeAllDeadQueries();
	closeAllContextWindows();

	destroyConnection();
	if(m_pAsynchronousConnectionData)delete m_pAsynchronousConnectionData;
	if(m_pSavedAsynchronousConnectionData)delete m_pSavedAsynchronousConnectionData;
}

void KviIrcContext::unhighlightAllWindows()
{
	m_pFrame->unhighlightWindowsOfContext(this);
}

void KviIrcContext::registerDataStreamMonitor(KviIrcDataStreamMonitor * m)
{
	if(!m_pMonitorList)
	{
		m_pMonitorList = new KviPointerList<KviIrcDataStreamMonitor>;
		m_pMonitorList->setAutoDelete(false);
	}
	m_pMonitorList->append(m);
}

void KviIrcContext::unregisterDataStreamMonitor(KviIrcDataStreamMonitor *m)
{
	if(!m_pMonitorList)return;
	m_pMonitorList->removeRef(m);
	if(m_pMonitorList->isEmpty())
	{
		delete m_pMonitorList;
		m_pMonitorList = 0;
	}
}

void KviIrcContext::closeAllDeadChannels()
{
	while(m_pDeadChannels)
	{
		KviChannel * c = m_pDeadChannels->first();
		if(c)
		{
			m_pFrame->closeWindow(c);
		} else {
			// ops....
			delete m_pDeadChannels;
			m_pDeadChannels = 0;
		}
	}
}

void KviIrcContext::closeAllDeadQueries()
{
	while(m_pDeadQueries)
	{
		KviQuery * q = m_pDeadQueries->first();
		if(q)
		{
			m_pFrame->closeWindow(q);
		} else {
			// ops....
			delete m_pDeadQueries;
			m_pDeadQueries = 0;
		}
	}
}

void KviIrcContext::closeAllContextWindows()
{
	while(m_pContextWindows)
	{
		KviWindow * w = m_pContextWindows->first();
		if(w)
		{
			m_pFrame->closeWindow(w);
		} else {
			// ops...
			delete m_pContextWindows;
			m_pContextWindows = 0;
		}
	}
}

KviChannel * KviIrcContext::findDeadChannel(const QString &name)
{
	if(!m_pDeadChannels)return 0;
	for(KviChannel * c = m_pDeadChannels->first();c;c = m_pDeadChannels->next())
	{
		__range_valid(c->isDeadChan());
		if(KviQString::equalCI(name,c->windowName()))return c;
	}
	return 0;
}

KviQuery * KviIrcContext::findDeadQuery(const QString &name)
{
	if(!m_pDeadQueries)return 0;
	for(KviQuery * c = m_pDeadQueries->first();c;c = m_pDeadQueries->next())
	{
		__range_valid(c->isDeadQuery());
		if(KviQString::equalCI(name,c->windowName()))return c;
	}
	return 0;
}

KviQuery * KviIrcContext::firstDeadQuery()
{
	if(!m_pDeadQueries)return 0;
	return m_pDeadQueries->first();
}

void KviIrcContext::registerContextWindow(KviWindow * pWnd)
{
	if(!m_pContextWindows)
	{
		m_pContextWindows = new KviPointerList<KviWindow>;
		m_pContextWindows->setAutoDelete(false);
	}
	m_pContextWindows->append(pWnd);
}

void KviIrcContext::registerDeadChannel(KviChannel * c)
{
	if(!m_pDeadChannels)
	{
		m_pDeadChannels = new KviPointerList<KviChannel>;
		m_pDeadChannels->setAutoDelete(false);
	}
	m_pDeadChannels->append(c);
}

void KviIrcContext::registerDeadQuery(KviQuery * q)
{
	if(!m_pDeadQueries)
	{
		m_pDeadQueries = new KviPointerList<KviQuery>;
		m_pDeadQueries->setAutoDelete(false);
	}
	m_pDeadQueries->append(q);
}

bool KviIrcContext::unregisterDeadChannel(KviChannel * c)
{
	// was a dead channel ?
	if(!m_pDeadChannels)return false;
	if(!m_pDeadChannels->removeRef(c))
	{
		return false;
	}
	if(m_pDeadChannels->isEmpty())
	{
		delete m_pDeadChannels;
		m_pDeadChannels = 0;
	}
	return true;
}

bool KviIrcContext::unregisterContextWindow(KviWindow * pWnd)
{
	if(!m_pContextWindows)return false;
	if(!m_pContextWindows->removeRef(pWnd))
	{
		return false;
	}
	if(m_pContextWindows->isEmpty())
	{
		delete m_pContextWindows;
		m_pContextWindows = 0;
	}
	return true;
}

bool KviIrcContext::unregisterDeadQuery(KviQuery * q)
{
	if(!m_pDeadQueries)return false;
	if(!m_pDeadQueries->removeRef(q))
	{
		return false;
	}
	if(m_pDeadQueries->isEmpty())
	{
		delete m_pDeadQueries;
		m_pDeadQueries = 0;
	}
	return true;
}

void KviIrcContext::createLinksWindow()
{
	if(m_pLinksWindow)return;
	KviKvsScript::run("links.open",m_pConsole);
}

void KviIrcContext::createListWindow()
{
	if(m_pListWindow)return;
	KviKvsScript::run("list.open",m_pConsole);
}

void KviIrcContext::destroyConnection()
{
	if(!m_pConnection)return;
	m_pConnection->closeAllChannels();
	m_pConnection->closeAllQueries();

	if(m_pLinksWindow)m_pLinksWindow->control(EXTERNAL_SERVER_DATA_PARSER_CONTROL_RESET);
	if(m_pListWindow)m_pListWindow->control(EXTERNAL_SERVER_DATA_PARSER_CONTROL_RESET);

	m_pConsole->connectionDetached();

	// make sure that m_pConnection is already 0 in any
	// event triggered by KviIrcConnection destructor
	KviIrcConnection * pTmp = m_pConnection;
	m_pConnection = 0;
	delete pTmp;
}

void KviIrcContext::setState(State eState)
{
	if(m_eState == eState)return;
	m_eState = eState;

	m_pFrame->childContextStateChange(this);

	emit stateChanged();

	if(eState == KviIrcContext::Idle)destroyConnection();

	m_pConsole->updateCaption();
}

void KviIrcContext::setAsynchronousConnectionData(KviAsynchronousConnectionData * d)
{
	if(m_pAsynchronousConnectionData)delete m_pAsynchronousConnectionData;
	m_pAsynchronousConnectionData = d;
}

void KviIrcContext::destroyAsynchronousConnectionData()
{
	if(!m_pAsynchronousConnectionData)return;
	delete m_pAsynchronousConnectionData;
	m_pAsynchronousConnectionData = 0;
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
			m_pReconnectTimer = 0;
			destroyAsynchronousConnectionData();

			m_pConsole->outputNoFmt(KVI_OUT_SYSTEMERROR,
				__tr2qs("Reconnect attempt aborted"));

			return;
		}

		// No connections in progress
		m_uConnectAttemptCount = 1;
		connectToCurrentServer();
	} else {
		// Sth is going on
		terminateConnectionRequest(false);
	}
}

void KviIrcContext::connectToCurrentServer()
{
	if(m_pReconnectTimer)
	{
		delete m_pReconnectTimer;
		m_pReconnectTimer = 0;
	}

	m_pConsole->outputNoFmt(KVI_OUT_SYSTEMMESSAGE," "); // spacer

	// No connection target specified.
	// If we have a saved target, reuse it
	if(!m_pAsynchronousConnectionData && m_pSavedAsynchronousConnectionData)
	{
			m_pAsynchronousConnectionData = m_pSavedAsynchronousConnectionData;
			m_pSavedAsynchronousConnectionData = 0;
	}

	if(m_pAsynchronousConnectionData)
	{
		// we have a specified connection target (either from outside or saved)
		if(m_pAsynchronousConnectionData->szServer.isEmpty())
		{
			// an empty server might mean "reuse the last server in context"
			if(
					m_pAsynchronousConnectionData->bUseLastServerInContext &&
					m_pSavedAsynchronousConnectionData
				)
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
					m_pAsynchronousConnectionData->m_pReconnectInfo = new KviServerReconnectInfo(*(m_pSavedAsynchronousConnectionData->m_pReconnectInfo));
				else m_pAsynchronousConnectionData->m_pReconnectInfo = 0;
					
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
			} else {
				m_pConsole->outputNoFmt(KVI_OUT_SYSTEMWARNING,__tr2qs("This is the first connection in this IRC context: using the global server setting"));
			}
		} else {
			// !m_pAsynchronousConnectionData->szServer.isEmpty()
			// ok , have a server to look for in the db
			// FIXME: this is a bit ugly... could it be managed in some completly different and nicer way ?
			KviServerDefinition d;
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
			if(!g_pServerDataBase->makeCurrentServer(&d,szError))
			{
				m_pConsole->outputNoFmt(KVI_OUT_SYSTEMERROR,szError);
				destroyAsynchronousConnectionData();
				return;
			}
		} // else we just connect to the globally selected irc server in the options dialog
	}

	KviNetwork * net = g_pServerDataBase->currentNetwork();
	KviServer  * srv = net ? net->currentServer() : 0;

	KviProxy   * prx = 0;

	if(!srv)
	{
		if(g_pServerDataBase->networkCount())
			KviKvsScript::run("options.edit KviServerOptionsWidget",m_pConsole);
		else
			m_pConsole->outputNoFmt(KVI_OUT_SYSTEMERROR,__tr2qs("No servers available. Check the options dialog or use the /SERVER command"));
		destroyAsynchronousConnectionData();
		return;
	}

	if(!net)
	{
		// BUG
		m_pConsole->outputNoFmt(KVI_OUT_SYSTEMERROR,__tr2qs("Ooops.. you've hit a bug in the servers database... I have found a server but not a network..."));
		destroyAsynchronousConnectionData();
		return;
	}


	prx = srv->proxyServer(g_pProxyDataBase);

	if(!prx && (srv->proxy()!=-1) && KVI_OPTION_BOOL(KviOption_boolUseProxyHost))
	{
		prx = g_pProxyDataBase->currentProxy();
		if(!prx)
		{
			m_pConsole->outputNoFmt(KVI_OUT_SYSTEMWARNING,__tr2qs("No proxy hosts available, resuming direct connection"));
		}
	}

	QString szBindAddress;

	srv->m_pReconnectInfo = 0;
	if(m_pAsynchronousConnectionData)
	{
		szBindAddress = m_pAsynchronousConnectionData->szBindAddress;
		if(m_pAsynchronousConnectionData->m_pReconnectInfo)
			srv->m_pReconnectInfo = new KviServerReconnectInfo(*(m_pAsynchronousConnectionData->m_pReconnectInfo));
	}

	// Find out the identity we'll be using in this connection
	// First check the server for one

	const KviUserIdentity * pIdentity = 0;

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

	if(m_pConnection) delete m_pConnection;
	m_pConnection = new KviIrcConnection(
		this,
		new KviIrcConnectionTarget(
			net,
			srv,
			prx,
			szBindAddress
		),
		new KviUserIdentity(*pIdentity)
	);

	setState(Connecting);

	if(m_pAsynchronousConnectionData)
	{
		m_pConnection->stateData()->setCommandToExecAfterConnect(m_pAsynchronousConnectionData->szCommandToExecAfterConnect);
		destroyAsynchronousConnectionData();
	}

	m_pConsole->connectionAttached();

	// save stuff for later

	// FIXME: this management of "next" connection should be reviewed a bit anyway
	if(m_pSavedAsynchronousConnectionData) delete m_pSavedAsynchronousConnectionData;
	m_pSavedAsynchronousConnectionData = new KviAsynchronousConnectionData();
	m_pSavedAsynchronousConnectionData->szServer = srv->m_szHostname;
	m_pSavedAsynchronousConnectionData->uPort = srv->port();
	m_pSavedAsynchronousConnectionData->bPortIsOk = true;
	m_pSavedAsynchronousConnectionData->bUseIPv6 = srv->isIPv6();
	m_pSavedAsynchronousConnectionData->bUseSSL = srv->useSSL();
	m_pSavedAsynchronousConnectionData->bSTARTTLS = srv->enabledSTARTTLS();
	m_pSavedAsynchronousConnectionData->szPass = srv->password();
	m_pSavedAsynchronousConnectionData->szServerId = srv->id();
	m_pSavedAsynchronousConnectionData->szInitUMode = srv->m_szInitUMode;
	if(srv->m_pReconnectInfo)
		m_pSavedAsynchronousConnectionData->m_pReconnectInfo = new KviServerReconnectInfo(*(srv->m_pReconnectInfo));
	else m_pSavedAsynchronousConnectionData->m_pReconnectInfo = 0;

	// this never fails!
	m_pConnection->start();
}

void KviIrcContext::connectionFailed(int iError)
{
	if(!m_pConnection)return; // this may happen in the destructor!

	m_pConsole->output(KVI_OUT_SYSTEMERROR,
		__tr2qs("Connection attempt failed [%s]"),
		m_pConnection->target()->server()->m_szHostname.toUtf8().data());

	// if the connection has been aborted by the user then just go idle
	if(iError == KviError_operationAborted)
		goto enter_idle_state;

	// FIXME: this should stop on critical errors !
	if(KVI_OPTION_BOOL(KviOption_boolAutoReconnectOnUnexpectedDisconnect))
	{
		if((!KVI_OPTION_UINT(KviOption_uintMaxAutoReconnectAttempts) ||
			(m_uConnectAttemptCount < KVI_OPTION_UINT(KviOption_uintMaxAutoReconnectAttempts))))
		{
			m_uConnectAttemptCount++;
			//FIXME: Multiply the delay by (m_uConnectAttemptCount / 2) so later connects are less frequent.
			if(!_OUTPUT_MUTE)
			{
				QString tmp;
				KviQString::sprintf(tmp,__tr2qs("Will attempt to reconnect in %d seconds"),KVI_OPTION_UINT(KviOption_uintAutoReconnectDelay));
				QString num;

				if(!KVI_OPTION_UINT(KviOption_uintMaxAutoReconnectAttempts))
				    KviQString::sprintf(num,__tr2qs("%d"),m_uConnectAttemptCount);
				else
				    KviQString::sprintf(num,__tr2qs("%d of %d"),
						m_uConnectAttemptCount,KVI_OPTION_UINT(KviOption_uintMaxAutoReconnectAttempts));
				tmp += " [" + num + "]";
				m_pConsole->outputNoFmt(KVI_OUT_SYSTEMMESSAGE,tmp);
			}

			KviServer oldServer(*(connection()->target()->server()));
			QString oldNickname = connection()->userInfo()->isAway() ? connection()->userInfo()->nickNameBeforeAway() : connection()->userInfo()->nickName();

			KviAsynchronousConnectionData * d = new KviAsynchronousConnectionData();
			d->szServer = oldServer.m_szHostname;
			d->szServerId = oldServer.id();
			d->uPort = oldServer.port();
			d->bPortIsOk = true;
			d->bUseIPv6 = oldServer.isIPv6();
			d->bUseSSL = oldServer.useSSL();
			d->bSTARTTLS = oldServer.enabledSTARTTLS();
			d->szPass = oldServer.password();
			d->szNick = oldNickname;
			d->szInitUMode = oldServer.m_szInitUMode;
			d->szCommandToExecAfterConnect = "";
			setAsynchronousConnectionData(d);
			beginAsynchronousConnect(1000 * KVI_OPTION_UINT(KviOption_uintAutoReconnectDelay));

			setState(Idle); // destroy the actual connection

			return;
		} else {
			if(!_OUTPUT_MUTE)
				m_pConsole->output(KVI_OUT_SYSTEMERROR,__tr2qs("Maximum number of reconnect attempts reached (%d): giving up"),KVI_OPTION_UINT(KviOption_uintMaxAutoReconnectAttempts));
		}
	}

	// reset the attempt count
	m_uConnectAttemptCount = 1;

	if(connection()->target()->server()->cacheIp())
	{
		if((((int)iError) == KviError_connectionTimedOut) ||
			(((int)iError) == KviError_connectionRefused) ||
			(((int)iError) == KviError_networkUnreachable) ||
			(((int)iError) == KviError_hostUnreachable))
		{
			m_pConsole->output(KVI_OUT_SYSTEMWARNING,__tr2qs("The connection attempt failed while using a cached IP address for the current server"));
			m_pConsole->output(KVI_OUT_SYSTEMWARNING,__tr2qs("The problem *might* be caused by an updated DNS entry"));
			m_pConsole->output(KVI_OUT_SYSTEMWARNING,__tr2qs("Try reconnecting with caching disabled"));
		}
	}

enter_idle_state:
	setState(Idle);
}

void KviIrcContext::connectionEstabilished()
{
	//qDebug("context::connectionEstabilished");
	//
	// The connection has been estabilished, the
	// KviIrcConnection will attempt to login now
	//
	m_uConnectAttemptCount = 1;

	bool bStopOutput = false;

	setState(LoggingIn); // this must be set in order for $server and other functions to return the correct values

	bStopOutput = KVS_TRIGGER_EVENT_0_HALTED(KviEvent_OnIRCConnectionEstabilished,m_pConsole);

	if(!bStopOutput)
	{
		m_pConsole->output(KVI_OUT_CONNECTION,__tr2qs("%Q established [%s (%s:%u)]"),
			connection()->link()->socket()->usingSSL() ? &(__tr2qs("Secure connection")) : &(__tr2qs("Connection")),
			connection()->target()->server()->m_szHostname.toUtf8().data(),
			connection()->target()->server()->m_szIp.toUtf8().data(),
			connection()->target()->server()->m_uPort);
	}

	// Add to recent server list (build the url of type irc[6]://<server>:<port>
	QString url;
	KviIrcUrl::join(url,connection()->target()->server());
	g_pApp->addRecentServer(url);

	// save the last server this console used
	//if(m_pLastIrcServer)delete m_pLastIrcServer;
	//m_pLastIrcServer = new KviServer(*(connection()->server()));
}

void KviIrcContext::connectionTerminated()
{
	if(!m_pConnection)return; // this may happen in the destructor!

	KviServer oldServer(*(connection()->target()->server()));
	if(oldServer.m_pReconnectInfo)
	{
		KviServerReconnectInfo *pOldInfo = oldServer.m_pReconnectInfo;
		oldServer.m_pReconnectInfo = 0;
		delete pOldInfo;
	}

	KviServerReconnectInfo info, *pInfo(&info);
	pInfo->m_szNick = connection()->userInfo()->isAway() ? connection()->userInfo()->nickNameBeforeAway() : connection()->userInfo()->nickName();
	pInfo->m_bIsAway = connection()->userInfo()->isAway();
	pInfo->m_szAwayReason = connection()->userInfo()->awayReason();

	connection()->requestQueue()->clearAll();

	// we consider it unexpected when we haven't sent a QUIT message and we're connected
	// or alternatively when a simulation of such a termination is requested (this is used to keep the queries open etc..)
	bool bUnexpectedDisconnect = (!(connection()->stateData()->sentQuit())) && ((m_eState == KviIrcContext::Connected) || connection()->stateData()->simulateUnexpectedDisconnect());

	if(bUnexpectedDisconnect)
	{
		if(KVI_OPTION_BOOL(KviOption_boolAutoReconnectOnUnexpectedDisconnect))
		{
			if(KVI_OPTION_BOOL(KviOption_boolRejoinChannelsAfterReconnect))
			{
				// FIXME: THIS SHOULD BE A KviIrcConnection FUNCTION
				KviChannel * c;
				QString szChannels, szProtectedChannels, szPasswords, szCurPass, szCurChan;
				// first only chans without key, in groups of 4
				for(c = connection()->channelList()->first(); c; c = connection()->channelList()->next())
				{
					szCurPass = c->channelKey();
					szCurChan = c->windowName();
					if(szCurPass.isEmpty())
					{
						if(!szChannels.isEmpty())
							szChannels.append(",");
						szChannels.append(szCurChan);
					} else {
						if(!szProtectedChannels.isEmpty())
							szProtectedChannels.append(",");
						szProtectedChannels.append(szCurChan);
						if(!szPasswords.isEmpty())
							szPasswords.append(",");
						szPasswords.append(szCurPass);
					}
				}
				if((!szChannels.isEmpty()) || (!szProtectedChannels.isEmpty()))
				{
					pInfo->m_szJoinChannels.append(szProtectedChannels);
					if(!szProtectedChannels.isEmpty() && !szChannels.isEmpty())
						pInfo->m_szJoinChannels.append(',');
					pInfo->m_szJoinChannels.append(szChannels);
					pInfo->m_szJoinChannels.append(" ");
					pInfo->m_szJoinChannels.append(szPasswords);
				}
			}


			if(KVI_OPTION_BOOL(KviOption_boolReopenQueriesAfterReconnect))
			{
				for(KviQuery * q = connection()->queryList()->first(); q; q = connection()->queryList()->next())
				{
					pInfo->m_szOpenQueryes.append(q->target());
				}
			}
		}

		if(KVI_OPTION_BOOL(KviOption_boolKeepChannelsOpenOnUnexpectedDisconnect) || KVI_OPTION_BOOL(KviOption_boolKeepChannelsOpenOnDisconnect))
			connection()->keepChannelsOpenAfterDisconnect();

		if(KVI_OPTION_BOOL(KviOption_boolKeepQueriesOpenOnUnexpectedDisconnect) || KVI_OPTION_BOOL(KviOption_boolKeepQueriesOpenOnDisconnect))
			connection()->keepQueriesOpenAfterDisconnect();
	} else {
		if(KVI_OPTION_BOOL(KviOption_boolKeepChannelsOpenOnDisconnect))
			connection()->keepChannelsOpenAfterDisconnect();

		if(KVI_OPTION_BOOL(KviOption_boolKeepQueriesOpenOnDisconnect))
			connection()->keepQueriesOpenAfterDisconnect();
	}

	setState(Idle);

	bool bStopOutput = false;

	bStopOutput = KVS_TRIGGER_EVENT_0_HALTED(KviEvent_OnIRCConnectionTerminated,m_pConsole);

	if(!bStopOutput)
	{
		m_pConsole->output(KVI_OUT_CONNECTION,__tr2qs("Connection terminated [%s (%s:%u)]"),
				oldServer.hostName().toUtf8().data(),
				oldServer.ip().toUtf8().data(),
				oldServer.port());
	}

	// do reconnect

	if(bUnexpectedDisconnect && KVI_OPTION_BOOL(KviOption_boolAutoReconnectOnUnexpectedDisconnect))
	{
		//m_uConnectAttemptCount = 1;
		if(!_OUTPUT_MUTE)
			m_pConsole->output(KVI_OUT_CONNECTION,__tr2qs("The connection terminated unexpectedly. Trying to reconnect..."));
		KviAsynchronousConnectionData * d = new KviAsynchronousConnectionData();
		d->szServerId = oldServer.id();
		d->szServer = oldServer.m_szHostname;
		d->uPort = oldServer.port();
		d->bPortIsOk = true;
		d->bUseIPv6 = oldServer.isIPv6();
		d->bUseSSL = oldServer.useSSL();
		d->bSTARTTLS = oldServer.enabledSTARTTLS();
		d->szPass = oldServer.password();
		d->szInitUMode = oldServer.m_szInitUMode;
		d->m_pReconnectInfo = new KviServerReconnectInfo(*(pInfo));
		setAsynchronousConnectionData(d);

		beginAsynchronousConnect(1000 * KVI_OPTION_UINT(KviOption_uintAutoReconnectDelay));
	}
}

void KviIrcContext::beginAsynchronousConnect(unsigned int uDelayInMSecs)
{
	if(m_pReconnectTimer) delete m_pReconnectTimer;
	m_pReconnectTimer = new QTimer(this);
	connect(m_pReconnectTimer,SIGNAL(timeout()),this,SLOT(asynchronousConnect()));
	m_pReconnectTimer->start(uDelayInMSecs);
}

void KviIrcContext::asynchronousConnect()
{
	if(m_pReconnectTimer)
	{
		delete m_pReconnectTimer;
		m_pReconnectTimer = 0;
	}

	if(state() != Idle) // need a brutal disconnect here
		terminateConnectionRequest(true,"Changing server...");

	connectToCurrentServer();
}

void KviIrcContext::terminateConnectionRequest(bool bForce, const QString & szQuitMsg, bool bSimulateUnexpectedDisconnect)
{
	if(!connection())return; // hm ?

	if(bSimulateUnexpectedDisconnect)connection()->stateData()->setSimulateUnexpectedDisconnect();

	switch(m_eState)
	{
		case Connected:
		{
			// was connected : send a quit and abort the connection
			bool bWasSentQuit = true;
			if(!connection()->stateData()->sentQuit())
			{
				KVS_TRIGGER_EVENT_0(KviEvent_OnDisconnectRequest,m_pConsole);
				QString szQuit = szQuitMsg;
				if(szQuit.isEmpty())szQuit = KVI_OPTION_STRING(KviOption_stringQuitMessage);
				szQuit.replace(";","\\;");
				szQuit.replace("\n"," ");
				QString buffer;
				KviKvsVariant ret;
				if(KviKvsScript::evaluate(szQuit,console(),0,&ret))
					ret.asString(buffer);
				else
					buffer = szQuit;
				QByteArray dat = connection()->encodeText(buffer);
				bWasSentQuit = false;
				connection()->stateData()->setSentQuit();
				connection()->sendFmtData("QUIT :%s",dat.data() ? dat.data() : ""); // here theoretically we COULD get disconnected
			} // else it was already sent anyway

			if(KVI_OPTION_BOOL(KviOption_boolForceBrutalQuit) || bWasSentQuit || bForce)
			{
				if(!bWasSentQuit)
				{
					// idle for some milliseconds in order to allow the quit message to reach
					// the remote end without breaking the connection
					KviThread::msleep(100);
				}
				// and brutally abort the connection (if it still exists!!!)
				if(connection())connection()->abort();
			} else {
				if(!bWasSentQuit)
					m_pConsole->outputNoFmt(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Sent QUIT, waiting for the server to close the connection..."));
				// else it was already sent anyway
			}
		}
		break;
		case Connecting:
		case LoggingIn:
			// was waiting for connection or login, just abort it: it will trigger an error anyway
			connection()->abort();
		break;
		default:
			// should never end here!
			__ASSERT(false);
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
