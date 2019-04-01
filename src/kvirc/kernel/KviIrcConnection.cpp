//=============================================================================
//
//   File : KviIrcConnection.cpp
//   Creation date : Mon 03 May 2004 01:45:42 by Szymon Stefanek
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

#include "KviIrcConnection.h"
#include "KviIrcConnectionTarget.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviIrcConnectionServerInfo.h"
#include "KviIrcConnectionStateData.h"
#include "KviIrcConnectionAntiCtcpFloodData.h"
#include "KviIrcConnectionNetsplitDetectorData.h"
#include "KviIrcConnectionAsyncWhoisData.h"
#include "KviIrcConnectionRequestQueue.h"
#include "KviIrcConnectionStatistics.h"
#include "KviIrcLink.h"
#include "KviIrcSocket.h"
#include "KviLocale.h"
#include "KviIrcServerDataBase.h"
#include "KviIrcServerReconnectInfo.h"
#include "KviProxyDataBase.h"
#include "KviError.h"
#include "kvi_out.h"
#include "KviOptions.h"
#include "KviConsoleWindow.h"
#include "KviNetUtils.h"
#include "KviInternalCommand.h"
#include "KviMainWindow.h"
#include "KviMexLinkFilter.h"
#include "KviMemory.h"
#include "kvi_debug.h"
#include "KviChannelWindow.h"
#include "KviQueryWindow.h"
#include "KviApplication.h"
#include "KviDataBuffer.h"
#include "KviNotifyList.h"
#include "KviDnsResolver.h"
#include "kvi_defaults.h"
#include "KviIrcServerParser.h"
#include "KviIrcDataStreamMonitor.h"
#include "KviLagMeter.h"
#include "KviKvsEventTriggers.h"
#include "KviKvsScript.h"
#include "KviControlCodes.h"
#include "KviUserIdentity.h"
#include "KviIdentityProfileSet.h"
#include "KviSASL.h"
#include "KviNickColors.h"
#include "KviIrcNetwork.h"

#include <QTimer>
#include <QTextCodec>
#include <QtGlobal>

#include <algorithm>
#include <memory>

extern KVIRC_API KviIrcServerDataBase * g_pServerDataBase;
extern KVIRC_API KviProxyDataBase * g_pProxyDataBase;

KviIrcConnection::KviIrcConnection(KviIrcContext * pContext, KviIrcConnectionTarget * pTarget, KviUserIdentity * pIdentity)
    : QObject(), m_pContext(pContext), m_pTarget(pTarget), m_pUserIdentity(pIdentity)
{
	m_pConsole = pContext->console();
	m_pLink = new KviIrcLink(this);
	m_pUserDataBase = new KviIrcUserDataBase();
	m_pUserInfo = new KviIrcConnectionUserInfo();
	m_pServerInfo = new KviIrcConnectionServerInfo();
	m_pStateData = new KviIrcConnectionStateData();
	m_pAntiCtcpFloodData = new KviIrcConnectionAntiCtcpFloodData();
	m_pNetsplitDetectorData = new KviIrcConnectionNetsplitDetectorData();
	m_pAsyncWhoisData = new KviIrcConnectionAsyncWhoisData();
	m_pStatistics = std::make_unique<KviIrcConnectionStatistics>();
	m_pRequestQueue = new KviIrcConnectionRequestQueue();
	setupSrvCodec();
	setupTextCodec();
}

KviIrcConnection::~KviIrcConnection()
{
	if(m_bIdentdAttached)
		g_pMainWindow->executeInternalCommand(KVI_INTERNALCOMMAND_IDENT_STOP);

	m_bIdentdAttached = false;
	if(m_pLocalhostDns)
	{
		QObject::disconnect(m_pLocalhostDns, SIGNAL(lookupDone(KviDnsResolver *)), nullptr, nullptr);
		if(m_pLocalhostDns->isRunning())
			m_pLocalhostDns->deleteLater();
		else
			delete m_pLocalhostDns;
	}

	delete m_pNotifyListTimer;
	m_pNotifyListTimer = nullptr;

	delete m_pNotifyListManager; // destroy this before the userDb
	m_pNotifyListManager = nullptr;

	delete m_pLagMeter;
	m_pLagMeter = nullptr;

	delete m_pLink; // <-- this MAY trigger a linkTerminated() or something like this!
	delete m_pTarget;
	delete m_pUserDataBase;
	delete m_pUserInfo;
	delete m_pServerInfo;
	delete m_pStateData;
	delete m_pAntiCtcpFloodData;
	delete m_pNetsplitDetectorData;
	delete m_pAsyncWhoisData;
	delete m_pUserIdentity;
	m_pRequestQueue->deleteLater();
}

unsigned int KviIrcConnection::outputQueueSize()
{
	return m_pLink->outputQueueSize();
}

void KviIrcConnection::clearOutputQueue(bool bPrivateMessagesOnly)
{
	m_pLink->clearOutputQueue(bPrivateMessagesOnly);
}

void KviIrcConnection::setEncoding(const QString & szEncoding)
{
	QTextCodec * c = KviLocale::instance()->codecForName(szEncoding.toLatin1());
	if(c == m_pTextCodec)
		return;
	if(!c)
	{
		m_pConsole->output(KVI_OUT_SYSTEMERROR, __tr2qs("Failed to set the encoding to %Q: mapping not available."), &szEncoding);
		return;
	}

	QString szTmp = c->name();
	for(auto & ch : m_pChannelList)
	{
		if((ch->textCodec() != c) && (ch->textCodec() != ch->defaultTextCodec())) // actually not using the default!
		{
			ch->forceTextCodec(c);
			if(_OUTPUT_VERBOSE)
				ch->output(KVI_OUT_VERBOSE, __tr2qs("Changed text encoding to %Q"), &szTmp);
		}
	}

	for(auto & q : m_pQueryList)
	{
		if((q->textCodec() != c) && (q->textCodec() != q->defaultTextCodec())) // actually not using the default!
		{
			q->forceTextCodec(c);
			if(_OUTPUT_VERBOSE)
				q->output(KVI_OUT_VERBOSE, __tr2qs("Changed text encoding to %Q"), &szTmp);
		}
	}

	m_pSrvCodec = c;
	m_pTextCodec = c;
	m_pConsole->setTextEncoding(szEncoding);
}

void KviIrcConnection::setupSrvCodec()
{
	// grab the codec: first look it up in the server data
	m_pSrvCodec = nullptr;
	if(!m_pTarget->server()->encoding().isEmpty())
	{
		m_pSrvCodec = KviLocale::instance()->codecForName(m_pTarget->server()->encoding().toLatin1());
		if(!m_pSrvCodec)
			qDebug("KviIrcConnection: can't find QTextCodec for encoding %s", m_pTarget->server()->encoding().toUtf8().data());
	}

	if(!m_pSrvCodec)
	{
		// try the network
		if(!m_pTarget->network()->encoding().isEmpty())
		{
			m_pSrvCodec = KviLocale::instance()->codecForName(m_pTarget->network()->encoding().toLatin1());
			if(!m_pSrvCodec)
				qDebug("KviIrcConnection: can't find QTextCodec for encoding %s", m_pTarget->network()->encoding().toUtf8().data());
		}
	}

	if(!m_pSrvCodec)
		m_pSrvCodec = KviApplication::defaultSrvCodec();

	m_pConsole->setTextEncoding(QString(m_pSrvCodec->name()));
}

void KviIrcConnection::setupTextCodec()
{
	// grab the codec: first look it up in the server data
	m_pTextCodec = nullptr;
	if(!m_pTarget->server()->textEncoding().isEmpty())
	{
		m_pTextCodec = KviLocale::instance()->codecForName(m_pTarget->server()->textEncoding().toLatin1());
		if(!m_pTextCodec)
			qDebug("KviIrcConnection: can't find QTextCodec for encoding %s", m_pTarget->server()->textEncoding().toUtf8().data());
	}

	if(!m_pTextCodec)
	{
		// try the network
		if(!m_pTarget->network()->textEncoding().isEmpty())
		{
			m_pTextCodec = KviLocale::instance()->codecForName(m_pTarget->network()->textEncoding().toLatin1());
			if(!m_pTextCodec)
				qDebug("KviIrcConnection: can't find QTextCodec for encoding %s", m_pTarget->network()->textEncoding().toUtf8().data());
		}
	}

	if(!m_pTextCodec)
		m_pTextCodec = KviApplication::defaultTextCodec();
}

/*
 * We're asking the connection itself to encode/decode text: use server specific encoding,
 * instead of the "user text" encoding used in channels
 */
QByteArray KviIrcConnection::encodeText(const QString & szText)
{
	if(!m_pSrvCodec)
		return szText.toUtf8();
	return m_pSrvCodec->fromUnicode(szText);
}

QString KviIrcConnection::decodeText(const char * pcText)
{
	if(!m_pSrvCodec)
		return QString(pcText);
	return m_pSrvCodec->toUnicode(pcText);
}

void KviIrcConnection::serverInfoReceived(const QString & szServerName, const QString & szUserModes, const QString & szChanModes)
{
	serverInfo()->setName(szServerName);
	serverInfo()->setSupportedUserModes(szUserModes);
	serverInfo()->setSupportedChannelModes(szChanModes);
	m_pConsole->updateCaption(); // for server name
	g_pMainWindow->childConnectionServerInfoChange(this);
}

const QString & KviIrcConnection::currentNetworkName() const
{
	return m_pServerInfo->networkName();
}

void KviIrcConnection::abort()
{
	// this WILL trigger linkAttemptFailed() or linkTerminated()
	m_pLink->abort();
}

void KviIrcConnection::start()
{
	m_eState = Connecting;
	if(KVI_OPTION_BOOL(KviOption_boolUseIdentService) && KVI_OPTION_BOOL(KviOption_boolUseIdentServiceOnlyOnConnect))
	{
		g_pMainWindow->executeInternalCommand(KVI_INTERNALCOMMAND_IDENT_START);
		m_bIdentdAttached = true;
	}
	m_pLink->start();
}

void KviIrcConnection::linkEstablished()
{
	m_eState = Connected;

	// setup reasonable defaults before notifying anyone
	m_pStatistics->setConnectionStartTime(kvi_unixTime());
	m_pStatistics->setLastMessageTime(kvi_unixTime());
	m_pServerInfo->setName(target()->server()->hostName());
	m_pServerInfo->setNetworkName(target()->network()->name());

	// FIXME: With STARTTLS this is called TWICE!
	for(auto & m : context()->monitorList())
		m->connectionInitiated();

	// FIXME: With STARTTLS this is called TWICE!
	context()->connectionEstablished();

	// Ok...we're logging in now
	resolveLocalHost();

	if(target()->server()->enabledCAP())
	{
		/*
		 * HACK: this is needed to avoid timeouts connecting to server that does not
		 * support the CAP extension (yet). Adding a somewhat broken message to a
		 * CAP LS request will force the server to output an error message.
		 *
		 * I'm currently aware of 2 methods:
		 * 1) using a broken nick (NICK -) to force a 437: ERR_UNAVAILRESOURCE
		 * 2) ping the server before registration to get a // 451: ERR_NOTREGISTERED
		 *
		 * Both method works, but the first method could compromise STARTTLS on some
		 * server that won't allow you to use tsl if the registration has already started
		 *
		 * Please note that IRC bouncers are currently broken for this setup: eg. psyBNC
		 * will let the user login with a "-" (single dash) nickname or executing any
		 * invalid command without throwing any error at all.
		 *
		 * This MUST go as one network packet to avoid possible handshake problems
		 * on slow connections.
		 */

		m_pStateData->setInsideInitialCapLs(true);
		m_pStateData->setIgnoreOneYouHaveNotRegisteredError(true);

		// FIXME: The PING method does NOT work with bouncers. We need a timeout here.

		if(sendFmtData("CAP LS\r\nPING :%Q", &(target()->server()->hostName())))
			return;

		//m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Failed to send the CAP LS request. Server capabilities will not be detected."));
	}

	if(!link() || !link()->socket())
		return;

	if((!link()->socket()->usingSSL()) && target()->server()->enabledSTARTTLS())
	{
#ifdef COMPILE_SSL_SUPPORT
		// STARTTLS without CAP (forced request)

		m_pStateData->setInsideInitialStartTls(true);
		m_pStateData->setIgnoreOneYouHaveNotRegisteredError(true);

		// STARTTLS requested but CAP not requested.
		if(trySTARTTLS(true))
			return; // STARTTLS negotiation in progress

		m_pStateData->setInsideInitialStartTls(false);
		m_pStateData->setIgnoreOneYouHaveNotRegisteredError(false);
#endif
	}

	loginToIrcServer();
}

#ifdef COMPILE_SSL_SUPPORT
bool KviIrcConnection::trySTARTTLS(bool bAppendPing)
{
	// Check if the server supports STARTTLS protocol and we want to
	// connect through it
	bool bRet = bAppendPing ? sendFmtData("STARTTLS\r\nPING :%Q", &(target()->server()->hostName())) : sendFmtData("STARTTLS");

	if(!bRet)
	{
		// Cannot send command
		m_pConsole->output(KVI_OUT_SOCKETERROR, __tr2qs("Impossible to send STARTTLS command to the IRC server. Your connection will NOT be encrypted"));
		return false;
	}

	m_pStateData->setSentStartTls();
	return true;
}

void KviIrcConnection::handleFailedInitialStartTls()
{
	m_pStateData->setInsideInitialStartTls(false);
	loginToIrcServer();
}

void KviIrcConnection::enableStartTlsSupport(bool bEnable)
{
	m_pStateData->setInsideInitialStartTls(false);

	if(bEnable)
	{
		// Ok, the server supports STARTTLS protocol
		// ssl handshake e switch del socket
		//qDebug("Starting SSL handshake...");
		link()->socket()->enterSSLMode(); // FIXME: this should be forwarded through KviIrcLink, probably
	}
	else
	{
		// The server does not support STARTTLS
		m_pConsole->output(KVI_OUT_SOCKETERROR, __tr2qs("The server does not support STARTTLS command. Your connection will NOT be encrypted"));
	}
}
#endif // COMPILE_SSL_SUPPORT

void KviIrcConnection::handleInitialCapLs()
{
	if(!m_pStateData->isInsideInitialCapLs())
		return; // We shouldn't be here...

	m_pStateData->setInsideInitialCapLs(false);

// STARTTLS support: this has to be checked first because it could imply
// a full cap renegotiation
#ifdef COMPILE_SSL_SUPPORT
	if((!link()->socket()->usingSSL()) && target()->server()->enabledSTARTTLS() && serverInfo()->supportedCaps().contains("tls", Qt::CaseInsensitive))
	{
		if(trySTARTTLS(false))
			return; // STARTTLS negotiation in progress
	}
#endif

	QString szRequests;

	auto cap_add = [&](const char * c) {
		if(serverInfo()->supportedCaps().contains(c, Qt::CaseInsensitive))
		{
			szRequests.append(c);
			szRequests.append(" ");
		}
	};

	if(target()->server()->enabledSASL())
		cap_add("sasl");

	cap_add("znc.in/server-time-iso");
	cap_add("server-time");
	cap_add("multi-prefix");
	cap_add("away-notify");
	cap_add("account-notify");
	cap_add("extended-join");
	cap_add("userhost-in-names");
	cap_add("chghost");
	cap_add("znc.in/self-message");

	if(szRequests.isEmpty())
	{
		endInitialCapNegotiation();
	}
	else
	{
		sendFmtData("CAP REQ :%s", szRequests.trimmed().toUtf8().data());
		m_pStateData->setInsideInitialCapReq(true);
	}
}

void KviIrcConnection::handleInitialCapAck()
{
	if(!m_pStateData->isInsideInitialCapReq())
		return; // We shouldn't be here

	m_pStateData->setInsideInitialCapReq(false);

	bool bUsed = false;

	//SASL
	if(target()->server()->enabledSASL() && m_pStateData->enabledCaps().contains("sasl", Qt::CaseInsensitive))
	{
		if(target()->server()->saslMethod() == QStringLiteral("EXTERNAL"))
		{
			if(KVI_OPTION_BOOL(KviOption_boolUseSSLCertificate) && link()->socket()->usingSSL())
			{
				bUsed = true;
				sendFmtData("AUTHENTICATE EXTERNAL");
				m_pStateData->setSentSaslMethod(QStringLiteral("EXTERNAL"));
			}
		}

		// Assume PLAIN if all other SASL methods are not chosen or we're attempting a fallback
		if(!bUsed && !target()->server()->saslNick().isEmpty() && !target()->server()->saslPass().isEmpty())
		{
			bUsed = true;
			sendFmtData("AUTHENTICATE PLAIN");
			m_pStateData->setSentSaslMethod(QStringLiteral("PLAIN"));
		}
	}

	if(bUsed)
		m_pStateData->setInsideAuthenticate(true);
	else
		endInitialCapNegotiation();
}

void KviIrcConnection::handleAuthenticate(KviCString & szAuth)
{
	//SASL
	if(!m_pStateData->isInsideAuthenticate())
		return;

	QByteArray szNick = encodeText(target()->server()->saslNick());
	QByteArray szPass = encodeText(target()->server()->saslPass());

	KviCString szOut;
	bool bSendString = false;
	if(m_pStateData->sentSaslMethod() == QStringLiteral("EXTERNAL"))
		bSendString = KviSASL::externalMethod(szAuth, szOut);
	else // Assume PLAIN
		bSendString = KviSASL::plainMethod(szAuth, szOut, szNick, szPass);

	if(bSendString)
		sendFmtData("AUTHENTICATE %s", szOut.ptr());
	else
		sendFmtData("AUTHENTICATE *");
}

void KviIrcConnection::handleInitialCapNak()
{
	endInitialCapNegotiation();
}

void KviIrcConnection::endInitialCapNegotiation()
{
	m_pStateData->setInsideAuthenticate(false);
	sendFmtData("CAP END");
	loginToIrcServer();
}

void KviIrcConnection::handleFailedInitialCapLs()
{
	m_pConsole->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Extended Capabilities don't seem to be supported by the server"));

	m_pStateData->setInsideInitialCapLs(false);
	loginToIrcServer();
}

void KviIrcConnection::linkTerminated()
{
	if(m_bIdentdAttached)
	{
		g_pMainWindow->executeInternalCommand(KVI_INTERNALCOMMAND_IDENT_STOP);
		m_bIdentdAttached = false;
	}
	m_eState = Idle;

	delete m_pNotifyListManager;
	m_pNotifyListManager = nullptr;

	delete m_pLagMeter;
	m_pLagMeter = nullptr;

	for(auto & m : context()->monitorList())
		m->connectionTerminated();

	// Prepare data for an eventual reconnect
	context()->connectionTerminated();
}

void KviIrcConnection::linkAttemptFailed(int iError)
{
	if(m_bIdentdAttached)
	{
		g_pMainWindow->executeInternalCommand(KVI_INTERNALCOMMAND_IDENT_STOP);
		m_bIdentdAttached = false;
	}
	m_eState = Idle;
	context()->connectionFailed(iError);
}

KviChannelWindow * KviIrcConnection::findChannel(const QString & szName)
{
	for(auto & c : m_pChannelList)
	{
		if(KviQString::equalCI(szName, c->windowName()))
			return c;
	}
	return nullptr;
}

int KviIrcConnection::getCommonChannels(const QString & szNick, QString & szChansBuffer, bool bAddEscapeSequences)
{
	int iCount = 0;
	for(auto & c : m_pChannelList)
	{
		if(c->isOn(szNick))
		{
			if(!szChansBuffer.isEmpty())
				szChansBuffer.append(", ");

			char uFlag = c->getUserFlag(szNick);
			if(uFlag)
			{
				KviQString::appendFormatted(szChansBuffer, bAddEscapeSequences ? "%c\r!c\r%Q\r" : "%c%Q", uFlag, &(c->windowName()));
			}
			else
			{
				if(bAddEscapeSequences)
					KviQString::appendFormatted(szChansBuffer, "\r!c\r%Q\r", &(c->windowName()));
				else
					szChansBuffer.append(c->windowName());
			}
			iCount++;
		}
	}
	return iCount;
}

void KviIrcConnection::unhighlightAllChannels()
{
	for(auto & c : m_pChannelList)
		c->unhighlight();
}

void KviIrcConnection::unhighlightAllQueries()
{
	for(auto & c : m_pQueryList)
		c->unhighlight();
}

void KviIrcConnection::closeAllChannels()
{
	while(!m_pChannelList.empty())
	{
		auto & c = m_pChannelList.front();
		c->close();
		QApplication::processEvents(QEventLoop::ExcludeSocketNotifiers | QEventLoop::ExcludeUserInputEvents);
	}
}

void KviIrcConnection::closeAllQueries()
{
	while(!m_pQueryList.empty())
	{
		auto & q = m_pChannelList.front();
		q->close();
		QApplication::processEvents(QEventLoop::ExcludeSocketNotifiers | QEventLoop::ExcludeUserInputEvents);
	}
}

KviChannelWindow * KviIrcConnection::createChannel(const QString & szName)
{
	KviChannelWindow * c = m_pContext->findDeadChannel(szName);
	if(c)
	{
		c->setAliveChan();
		if(!KVI_OPTION_BOOL(KviOption_boolCreateMinimizedChannels))
		{
			g_pMainWindow->setActiveWindow(c);
		}
	}
	else
	{
		c = new KviChannelWindow(m_pConsole, szName);
		g_pMainWindow->addWindow(c, !KVI_OPTION_BOOL(KviOption_boolCreateMinimizedChannels));
	}
	return c;
}

KviQueryWindow * KviIrcConnection::createQuery(const QString & szNick, CreateQueryVisibilityMode eVisibilityMode)
{
	KviQueryWindow * q = m_pContext->findDeadQuery(szNick);
	if(!q)
	{
		q = findQuery(szNick);
		if(q)
			return q; // hm ?
	}

	bool bShowIt;

	// adjust visibility mode
	switch(eVisibilityMode)
	{
		//case CreateQueryFollowGlobalVisibilitySetting:
		case CreateQueryVisibilityMinimized:
			bShowIt = false;
			break;
		case CreateQueryVisibilityVisible:
			bShowIt = true;
			break;
		default:
			bShowIt = !KVI_OPTION_BOOL(KviOption_boolCreateIncomingQueriesAsMinimized);
			break;
	}

	if(q)
	{
		q->setAliveQuery();
		if(bShowIt)
		{
			g_pMainWindow->setActiveWindow(q);
		}
	}
	else
	{
		q = new KviQueryWindow(m_pConsole, szNick);
		g_pMainWindow->addWindow(q, bShowIt);
	}
	return q;
}

KviQueryWindow * KviIrcConnection::findQuery(const QString & szName)
{
	for(auto & q : m_pQueryList)
	{
		if(KviQString::equalCI(szName, q->windowName()))
			return q;
	}
	return nullptr;
}

void KviIrcConnection::registerChannel(KviChannelWindow * c)
{
	m_pChannelList.push_back(c);
	if(KVI_OPTION_BOOL(KviOption_boolLogChannelHistory))
		g_pApp->addRecentChannel(c->windowName(), m_pServerInfo->networkName());
	emit(channelRegistered(c));
	emit(chanListChanged());
}

void KviIrcConnection::unregisterChannel(KviChannelWindow * c)
{
	m_pChannelList.erase(std::remove(m_pChannelList.begin(), m_pChannelList.end(), c), m_pChannelList.end());
	requestQueue()->dequeueChannel(c);
	emit(channelUnregistered(c));
	emit(chanListChanged());
}

void KviIrcConnection::registerQuery(KviQueryWindow * q)
{
	m_pQueryList.push_back(q);
}

void KviIrcConnection::unregisterQuery(KviQueryWindow * q)
{
	m_pQueryList.erase(std::remove(m_pQueryList.begin(), m_pQueryList.end(), q), m_pQueryList.end());
}

void KviIrcConnection::keepChannelsOpenAfterDisconnect()
{
	while(!m_pChannelList.empty())
	{
		KviChannelWindow * c = m_pChannelList.front();
		c->outputNoFmt(KVI_OUT_SOCKETERROR, __tr2qs("Connection to server lost"));
		c->setDeadChan();
	}
}

void KviIrcConnection::keepQueriesOpenAfterDisconnect()
{
	while(!m_pQueryList.empty())
	{
		KviQueryWindow * q = m_pQueryList.front();
		q->outputNoFmt(KVI_OUT_SOCKETERROR, __tr2qs("Connection to server lost"));
		q->setDeadQuery();
	}
}

void KviIrcConnection::resurrectDeadQueries()
{
	while(KviQueryWindow * q = m_pContext->firstDeadQuery())
	{
		q->outputNoFmt(KVI_OUT_SOCKETMESSAGE, __tr2qs("Connection to server established"));
		q->setAliveQuery();
	}
}

//=== Message send stuff ====================================================//
// Max buffer that can be sent to an IRC server is 512 bytes
// including CRLF. (ircd simply 'cuts' messages to 512 bytes
// and discards the remainig part)
// Note that 510 bytes of data is a reasonably long message :)
//
// 01234567890123456789012345678901234567890123456789
// 01234567890123456789012345678901234567890123456789
// 01234567890123456789012345678901234567890123456789
// 01234567890123456789012345678901234567890123456789
// 01234567890123456789012345678901234567890123456789
// 01234567890123456789012345678901234567890123456789
// 01234567890123456789012345678901234567890123456789
// 01234567890123456789012345678901234567890123456789
// 01234567890123456789012345678901234567890123456789
// 01234567890123456789012345678901234567890123456789
// 0123456789\r\n
//
// We keep a list of data to send, and flush it as soon as we can.
//

bool KviIrcConnection::sendFmtData(const char * pcFmt, ...)
{
	KviDataBuffer * pData = new KviDataBuffer(512);
	kvi_va_list(list);
	kvi_va_start(list, pcFmt);
	bool bTruncated;
	//sprintf the buffer up to 512 chars (adds a CRLF too)
	int iLen = kvi_irc_vsnprintf((char *)(pData->data()), pcFmt, list, &bTruncated);
	kvi_va_end(list);

	//adjust the buffer size
	if(iLen < 512)
		pData->resize(iLen);

	if(bTruncated)
	{
		if(!_OUTPUT_MUTE)
			m_pConsole->outputNoFmt(KVI_OUT_SOCKETWARNING, __tr2qs("[LINK WARNING]: Socket message truncated to 512 bytes."));
	}

	QString szMsg = QString::fromLatin1((const char *)(pData->data()), iLen - 2);

	// notify the monitors
	for(auto & m : context()->monitorList())
	{
		if(m->outgoingMessage(szMsg.toLatin1().data()))
		{
			delete pData;
			return true;
		}
	}

	// Trigger OnOutboundTraffic event
	KVS_TRIGGER_EVENT_1(KviEvent_OnOutboundTraffic, m_pConsole->activeWindow(), szMsg);

	return m_pLink->sendPacket(pData);
}

bool KviIrcConnection::sendData(const char * pcBuffer, int iBuflen)
{
	if(iBuflen < 0)
		iBuflen = (int)strlen(pcBuffer);
	if(iBuflen > 510)
	{
		iBuflen = 510;
		if(!_OUTPUT_MUTE)
			m_pConsole->outputNoFmt(KVI_OUT_SOCKETWARNING, __tr2qs("[LINK WARNING]: Socket message truncated to 512 bytes."));
	}

	KviDataBuffer * pData = new KviDataBuffer(iBuflen + 2);
	KviMemory::move(pData->data(), pcBuffer, iBuflen);
	*(pData->data() + iBuflen) = '\r';
	*(pData->data() + iBuflen + 1) = '\n';

	QString szMsg = (const char *)(pData->data());
	szMsg.truncate(iBuflen);

	// notify the monitors
	for(auto & m : context()->monitorList())
	{
		if(m->outgoingMessage(szMsg.toUtf8().data()))
		{
			delete pData;
			return true;
		}
	}

	// Trigger OnOutboundTraffic event
	KVS_TRIGGER_EVENT_1(KviEvent_OnOutboundTraffic, m_pConsole->activeWindow(), szMsg);

	return m_pLink->sendPacket(pData);
}

//
// notify list management
//

void KviIrcConnection::delayedStartNotifyList()
{
	KVI_ASSERT(!m_pNotifyListTimer);

	delete m_pNotifyListTimer;

	m_pNotifyListTimer = new QTimer();
	m_pNotifyListTimer->setInterval(15000);
	m_pNotifyListTimer->setSingleShot(true);
	m_pNotifyListTimer->start();
	connect(m_pNotifyListTimer, SIGNAL(timeout()), this, SLOT(restartNotifyList()));

	// This delay is large enough to fire after the MOTD has been sent,
	// even on the weirdest network.
	// If there is no MOTD, this timer will fire after 15 secs,
	// If there is a MOTD, restartNotifyList() will be triggered by RPL_ENDOFMOTD and
	// will kill the timer before it has fired.
}

void KviIrcConnection::endOfMotdReceived()
{
	// if the timer is still there running then just
	if(m_pNotifyListTimer)
		restartNotifyList();
}

void KviIrcConnection::restartNotifyList()
{
	delete m_pNotifyListTimer;
	m_pNotifyListTimer = nullptr;

	// clear it
	if(m_pNotifyListManager)
	{
		m_pNotifyListManager->stop(); // may need to remove watch entries
		delete m_pNotifyListManager;
		m_pNotifyListManager = nullptr;
	}

	if(!KVI_OPTION_BOOL(KviOption_boolUseNotifyList))
		return;

	if(serverInfo()->supportsWatchList() && KVI_OPTION_BOOL(KviOption_boolUseWatchListIfAvailable))
	{
		if(_OUTPUT_VERBOSE)
			m_pConsole->output(KVI_OUT_VERBOSE, __tr2qs("The server seems to support the WATCH notify list method, will try to use it"));
		m_pNotifyListManager = new KviWatchNotifyListManager(this);
	}
	else
	{
		if(KVI_OPTION_BOOL(KviOption_boolUseIntelligentNotifyListManager))
			m_pNotifyListManager = new KviIsOnNotifyListManager(this);
		else
			m_pNotifyListManager = new KviStupidNotifyListManager(this);
	}
	m_pNotifyListManager->start();
}

void KviIrcConnection::restartLagMeter()
{
	delete m_pLagMeter;
	m_pLagMeter = nullptr;

	if(!KVI_OPTION_BOOL(KviOption_boolUseLagMeterEngine))
		return;
	m_pLagMeter = new KviLagMeter(this);
}

void KviIrcConnection::resolveLocalHost()
{
	QString szIp;

	if(!link()->socket()->getLocalHostIp(szIp, target()->server()->isIPv6()))
	{
		bool bGotIp = false;
		if(!KVI_OPTION_STRING(KviOption_stringLocalHostIp).isEmpty())
		{
#ifdef COMPILE_IPV6_SUPPORT
			if(target()->server()->isIPv6())
			{
				if(KviNetUtils::isValidStringIPv6(KVI_OPTION_STRING(KviOption_stringLocalHostIp)))
					bGotIp = true;
			}
			else
			{
#endif
				if(KviNetUtils::isValidStringIp(KVI_OPTION_STRING(KviOption_stringLocalHostIp)))
					bGotIp = true;
#ifdef COMPILE_IPV6_SUPPORT
			}
#endif
		}

		if(bGotIp)
		{
			m_pUserInfo->setLocalHostIp(KVI_OPTION_STRING(KviOption_stringLocalHostIp));
			if(!_OUTPUT_MUTE)
				m_pConsole->output(KVI_OUT_SYSTEMWARNING, __tr2qs("Can't resolve local host address, using user supplied one (%Q)"),
				    &(m_pUserInfo->localHostIp()));
		}
		else
		{
			// FIXME : Maybe check for IPv6 here too ?
			m_pUserInfo->setLocalHostIp("127.0.0.1");
			if(!_OUTPUT_MUTE)
				m_pConsole->output(KVI_OUT_SYSTEMWARNING, __tr2qs("Can't resolve local host address, using default 127.0.0.1"),
				    &(m_pUserInfo->localHostIp()));
		}
	}
	else
	{
		m_pUserInfo->setLocalHostIp(szIp);
		if(!_OUTPUT_QUIET)
			m_pConsole->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Local host address is %Q"),
			    &(m_pUserInfo->localHostIp()));
	}

	// For now this is the only we know
	m_pUserInfo->setHostName(m_pUserInfo->localHostIp());
	m_pUserInfo->setHostIp(m_pUserInfo->localHostIp());
}

void KviIrcConnection::changeAwayState(bool bAway)
{
	if(bAway)
		m_pUserInfo->setAway();
	else
		m_pUserInfo->setBack();

	// Update the user entry
	KviIrcUserEntry * e = userDataBase()->find(userInfo()->nickName());
	if(e)
	{
		e->setAway(bAway);
		// immediately update the userlist of the current channel
		if(g_pActiveWindow->isChannel())
			((KviChannelWindow *)g_pActiveWindow)->userListView()->updateArea();
	}

	m_pConsole->updateCaption();
	g_pMainWindow->childConnectionAwayStateChange(this);

	emit awayStateChanged();
}

void KviIrcConnection::userInfoReceived(const QString & szUserName, const QString & szHostName)
{
	userInfo()->setUserName(szUserName);
	QString szUnmaskedHost = m_pUserInfo->unmaskedHostName();
	// Update the user entry
	KviIrcUserEntry * e = userDataBase()->find(userInfo()->nickName());
	if(e) // should be there! (we have the permanent entry in the notify list view)
	{
		e->setUser(szUserName);
		if(!szHostName.isEmpty())
			e->setHost(szHostName);
	} // else buuug

	if(szHostName.isEmpty())
		return; // nothing to do anyway

	if(KviQString::equalCS(m_pUserInfo->hostName(), szHostName))
		return; // again nothing to do

	static bool warned_once = false;

	if(!warned_once)
	{
		if(!(m_pUserInfo->hostName().isEmpty() || KviQString::equalCS(m_pUserInfo->hostName(), m_pUserInfo->localHostIp())))
		{
			// ok, something weird is probably going on
			// is is non-empty and it is NOT the IP address we have set
			// at connection startup...
			// ...the server (or more likely the bouncer) must have changed his mind...
			if(!_OUTPUT_MUTE)
			{
				m_pConsole->output(KVI_OUT_SYSTEMWARNING, __tr2qs("The server reports a hostname differing from what KVIrc set for the local hostname."));
				m_pConsole->output(KVI_OUT_SYSTEMWARNING, __tr2qs("This can happen if you use a broken bouncer, the IRC server masks your IP/Hostname or something weird is happening on the IRC server."));
			}
			warned_once = true;
		}
	}

	// set it
	m_pUserInfo->setHostName(szHostName);

	bool bChangeIp = true;

// if we don't have any routable IP yet, then it is worth to lookup the new hostname

#ifdef COMPILE_IPV6_SUPPORT
	if((KviNetUtils::isValidStringIp(m_pUserInfo->hostIp()) && KviNetUtils::isRoutableIpString(m_pUserInfo->hostIp())) || KviNetUtils::isValidStringIPv6(m_pUserInfo->hostIp()))
#else
	if((KviNetUtils::isValidStringIp(m_pUserInfo->hostIp()) && KviNetUtils::isRoutableIpString(m_pUserInfo->hostIp())))
#endif
	{
		if(KVI_OPTION_BOOL(KviOption_boolDccGuessIpFromServerWhenLocalIsUnroutable) && KVI_OPTION_BOOL(KviOption_boolDccBrokenBouncerHack))
		{
			if(!_OUTPUT_MUTE)
				m_pConsole->outputNoFmt(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Here goes your \"broken bouncer hack\": The server has changed the hostname but I'll ignore the IP address change"));
			bChangeIp = false;
		}
	}

	if(bChangeIp)
	{
// lookup the new hostname then...
#ifdef COMPILE_IPV6_SUPPORT
		if(KviNetUtils::isValidStringIp(szHostName) || KviNetUtils::isValidStringIPv6(szHostName))
#else
		if(KviNetUtils::isValidStringIp(szHostName))
#endif
		{
			if(!_OUTPUT_MUTE)
				m_pConsole->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("The local IP address as seen by the IRC server is %Q"), &szHostName);
			m_pUserInfo->setHostIp(szHostName);
		}
		else
#ifdef COMPILE_IPV6_SUPPORT
		    if(KviNetUtils::isValidStringIp(szUnmaskedHost) || KviNetUtils::isValidStringIPv6(szUnmaskedHost))
#else
		    if(KviNetUtils::isValidStringIp(szUnmaskedHost))
#endif
		{
			if(!_OUTPUT_MUTE)
				m_pConsole->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("The local IP address as seen by the IRC server is %Q"), &szUnmaskedHost);
			m_pUserInfo->setHostIp(szUnmaskedHost);
		}
		else
		{
			// look it up too
			delete m_pLocalhostDns; // it could be only another local host lookup
			m_pLocalhostDns = new KviDnsResolver();
			connect(m_pLocalhostDns, SIGNAL(lookupDone(KviDnsResolver *)), this, SLOT(hostNameLookupTerminated(KviDnsResolver *)));

			if(!m_pLocalhostDns->lookup(szHostName, KviDnsResolver::Any))
			{
				if(!_OUTPUT_MUTE)
				{
					// don't change the string to aid the translators
					QString szTmp = __tr2qs("Can't start the DNS slave thread");
					m_pConsole->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Unable to resolve the local hostname as seen by the IRC server: %Q"), &szTmp);
				}
				delete m_pLocalhostDns;
				m_pLocalhostDns = nullptr;
			}
			else
			{
				if(!_OUTPUT_MUTE)
					m_pConsole->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Looking up the local hostname as seen by the IRC server (%Q)"), &szHostName);
			}
		}
	}
}

void KviIrcConnection::hostNameLookupTerminated(KviDnsResolver *)
{
	if(!m_pLocalhostDns)
	{
		qDebug("Something weird is happening: pDns != 0 but m_pLocalhostDns == 0 :/");
		return;
	}

	if(m_pLocalhostDns->state() != KviDnsResolver::Success)
	{
		QString szErr = KviError::getDescription(m_pLocalhostDns->error());
		if(!m_pUserInfo->hostIp().isEmpty())
			m_pConsole->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Unable to resolve the local hostname as seen by the IRC server: %Q, using previously resolved %Q"),
			    &szErr, &(m_pUserInfo->hostIp()));
		else
			m_pConsole->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Unable to resolve the local hostname as seen by the IRC server: %Q"),
			    &szErr);
	}
	else
	{
		QString szIpAddr = m_pLocalhostDns->firstIpAddress();
		m_pConsole->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Local hostname as seen by the IRC server resolved to %Q"), &szIpAddr);
		m_pUserInfo->setHostIp(m_pLocalhostDns->firstIpAddress());
	}

	delete m_pLocalhostDns;
	m_pLocalhostDns = nullptr;
}

void KviIrcConnection::useRealName(const QString & szRealName)
{
	// Evaluate functions in the real name (so we can have $version() inside)

	QString szRealNameBuffer = szRealName;

	if(!szRealNameBuffer.isEmpty())
	{
		KviQString::escapeKvs(&szRealNameBuffer, KviQString::PermitVariables | KviQString::PermitFunctions);

		KviKvsVariant vRet;
		if(KviKvsScript::evaluate(szRealNameBuffer, console(), nullptr, &vRet))
			vRet.asString(szRealNameBuffer);
	}

	m_pUserInfo->setRealName(szRealNameBuffer);
}

void KviIrcConnection::useProfileData(KviIdentityProfileSet * pSet, const QString & szNetwork)
{
	KviIdentityProfile * pProfile = pSet->findNetwork(szNetwork);
	if(!pProfile)
		return;

	// Update connection data
	//m_pUserInfo->setNickName(pProfile->nick());
	m_pUserInfo->setUserName(pProfile->userName());
	useRealName(pProfile->realName());
}

QString KviIrcConnection::pickNextLoginNickName(bool bForceDefaultIfPrimaryNicknamesEmpty, const QString & szBaseNickForRandomChoices, QString & szChoiceDescriptionBuffer)
{
	QString szNick;

	KVI_ASSERT(target());

	// try profiles first

	if((int)(m_pStateData->loginNickNameState()) < (int)(KviIrcConnectionStateData::UsedProfileSpecificNickName))
	{
		KviIrcNetwork * pNetwork = target()->network();
		KVI_ASSERT(pNetwork);

		KviIdentityProfileSet * pSet = KviIdentityProfileSet::instance();
		bool bProfilesEnabled = pSet ? (pSet->isEnabled() && !pSet->isEmpty()) : false;
		if(bProfilesEnabled)
		{
			KviIdentityProfile * pProfile = pSet->findNetwork(pNetwork->name());
			if(pProfile)
			{
				szNick = pProfile->nick().trimmed();
				if(!szNick.isEmpty())
				{
					szChoiceDescriptionBuffer = __tr2qs("profile specific option");
					m_pStateData->setLoginNickNameState(KviIrcConnectionStateData::UsedProfileSpecificNickName);
					return szNick;
				}
			}
		}
	}

	if((int)(m_pStateData->loginNickNameState()) < (int)(KviIrcConnectionStateData::UsedAlternativeProfileSpecificNickName))
	{
		KviIrcNetwork * pNetwork = target()->network();
		KVI_ASSERT(pNetwork);

		KviIdentityProfileSet * pSet = KviIdentityProfileSet::instance();
		bool bProfilesEnabled = pSet ? (pSet->isEnabled() && !pSet->isEmpty()) : false;
		if(bProfilesEnabled)
		{
			KviIdentityProfile * pProfile = pSet->findNetwork(pNetwork->name());
			if(pProfile)
			{
				szNick = pProfile->nick().trimmed();
				if(!szNick.isEmpty())
				{
					szChoiceDescriptionBuffer = __tr2qs("profile specific alternative option");
					m_pStateData->setLoginNickNameState(KviIrcConnectionStateData::UsedAlternativeProfileSpecificNickName);
					return szNick;
				}
			}
		}
	}

	// try server specific choices

	if((int)(m_pStateData->loginNickNameState()) < (int)(KviIrcConnectionStateData::UsedServerSpecificNickName))
	{
		KviIrcServer * pServer = target()->server();
		KVI_ASSERT(pServer);

		szNick = pServer->nickName().trimmed();
		if(!szNick.isEmpty())
		{
			szChoiceDescriptionBuffer = __tr2qs("server specific");
			m_pStateData->setLoginNickNameState(KviIrcConnectionStateData::UsedServerSpecificNickName);
			return szNick;
		}
	}

	if((int)(m_pStateData->loginNickNameState()) < (int)(KviIrcConnectionStateData::UsedAlternativeServerSpecificNickName))
	{
		KviIrcServer * pServer = target()->server();
		KVI_ASSERT(pServer);

		szNick = pServer->alternativeNickName().trimmed();
		if(!szNick.isEmpty())
		{
			szChoiceDescriptionBuffer = __tr2qs("server specific alternative option");
			m_pStateData->setLoginNickNameState(KviIrcConnectionStateData::UsedAlternativeServerSpecificNickName);
			return szNick;
		}
	}

	// then try network specific ones

	if((int)(m_pStateData->loginNickNameState()) < (int)(KviIrcConnectionStateData::UsedNetworkSpecificNickName))
	{
		KviIrcNetwork * pNetwork = target()->network();
		KVI_ASSERT(pNetwork);

		szNick = pNetwork->nickName().trimmed();
		if(!szNick.isEmpty())
		{
			szChoiceDescriptionBuffer = __tr2qs("network specific option");
			m_pStateData->setLoginNickNameState(KviIrcConnectionStateData::UsedNetworkSpecificNickName);
			return szNick;
		}
	}

	if((int)(m_pStateData->loginNickNameState()) < (int)(KviIrcConnectionStateData::UsedAlternativeNetworkSpecificNickName))
	{
		KviIrcNetwork * pNetwork = target()->network();
		KVI_ASSERT(pNetwork);

		szNick = pNetwork->alternativeNickName().trimmed();
		if(!szNick.isEmpty())
		{
			szChoiceDescriptionBuffer = __tr2qs("network specific alternative option");
			m_pStateData->setLoginNickNameState(KviIrcConnectionStateData::UsedAlternativeNetworkSpecificNickName);
			return szNick;
		}
	}

	// look in global options

	if((int)(m_pStateData->loginNickNameState()) < (int)(KviIrcConnectionStateData::UsedGlobalNickName1))
	{
		szNick = KVI_OPTION_STRING(KviOption_stringNickname1).trimmed();
		if(bForceDefaultIfPrimaryNicknamesEmpty && szNick.isEmpty())
		{
			KVI_OPTION_STRING(KviOption_stringNickname1) = QString::fromUtf8(KVI_DEFAULT_NICKNAME1);
			szNick = KVI_OPTION_STRING(KviOption_stringNickname1).trimmed();
		}
		if(!szNick.isEmpty())
		{
			szChoiceDescriptionBuffer = __tr2qs("primary nickname specified in options");
			m_pStateData->setLoginNickNameState(KviIrcConnectionStateData::UsedGlobalNickName1);
			return szNick;
		}
	}

	if((int)(m_pStateData->loginNickNameState()) < (int)(KviIrcConnectionStateData::UsedGlobalNickName2))
	{
		szNick = KVI_OPTION_STRING(KviOption_stringNickname2).trimmed();
		if(!szNick.isEmpty())
		{
			szChoiceDescriptionBuffer = __tr2qs("alternative nickname specified in options");
			m_pStateData->setLoginNickNameState(KviIrcConnectionStateData::UsedGlobalNickName2);
			return szNick;
		}
	}

	if((int)(m_pStateData->loginNickNameState()) < (int)(KviIrcConnectionStateData::UsedGlobalNickName3))
	{
		szNick = KVI_OPTION_STRING(KviOption_stringNickname3).trimmed();
		if(!szNick.isEmpty())
		{
			szChoiceDescriptionBuffer = __tr2qs("second alternative nickname specified in options");
			m_pStateData->setLoginNickNameState(KviIrcConnectionStateData::UsedGlobalNickName3);
			return szNick;
		}
	}

	if((int)(m_pStateData->loginNickNameState()) < (int)(KviIrcConnectionStateData::UsedGlobalNickName4))
	{
		szNick = KVI_OPTION_STRING(KviOption_stringNickname4).trimmed();
		if(!szNick.isEmpty())
		{
			szChoiceDescriptionBuffer = __tr2qs("third alternative nickname specified in options");
			m_pStateData->setLoginNickNameState(KviIrcConnectionStateData::UsedGlobalNickName4);
			return szNick;
		}
	}

	// fallback to 4 random alternatives
	if((int)(m_pStateData->loginNickNameState()) < (int)(KviIrcConnectionStateData::UsedRandomNickName4))
	{
		const QString nickList[] = {
			szBaseNickForRandomChoices,
			KVI_OPTION_STRING(KviOption_stringNickname1),
			KVI_OPTION_STRING(KviOption_stringNickname2),
			KVI_OPTION_STRING(KviOption_stringNickname3),
			KVI_OPTION_STRING(KviOption_stringNickname4),
			QString::fromUtf8(KVI_DEFAULT_NICKNAME1)
		};

		for(const auto & ii : nickList)
		{
			szNick = ii.trimmed();

			if(!szNick.isEmpty())
				break;
		}

		szNick = szNick.left(7);
		while(szNick.length() < 8)
		{
			QString num;
			num.setNum(rand() % 10);
			szNick.append(num);
		}

		szChoiceDescriptionBuffer = __tr2qs("random nickname");

		if((int)(m_pStateData->loginNickNameState()) < (int)(KviIrcConnectionStateData::UsedRandomNickName1))
			m_pStateData->setLoginNickNameState(KviIrcConnectionStateData::UsedRandomNickName1);
		else if((int)(m_pStateData->loginNickNameState()) < (int)(KviIrcConnectionStateData::UsedRandomNickName2))
			m_pStateData->setLoginNickNameState(KviIrcConnectionStateData::UsedRandomNickName2);
		else if((int)(m_pStateData->loginNickNameState()) < (int)(KviIrcConnectionStateData::UsedRandomNickName3))
			m_pStateData->setLoginNickNameState(KviIrcConnectionStateData::UsedRandomNickName3);
		else
			m_pStateData->setLoginNickNameState(KviIrcConnectionStateData::UsedRandomNickName4);
		return szNick;
	}

	// give up

	m_pStateData->setLoginNickNameState(KviIrcConnectionStateData::UsedManualNickname);
	return szNick; // empty
}

void KviIrcConnection::loginToIrcServer()
{
	KviIrcServer * pServer = target()->server();
	KviIrcNetwork * pNet = target()->network();

	// For now this is the only we know
	m_pServerInfo->setName(pServer->hostName());

	QString szTmpNick, szTmpUser, szTmpPass, szTmpName;
	// Username
	szTmpUser = pServer->userName().trimmed();
	if(!szTmpUser.isEmpty())
	{
		if(!_OUTPUT_MUTE)
			m_pConsole->output(KVI_OUT_VERBOSE, __tr2qs("Using server specific username (%Q)"), &(szTmpUser));
	}
	else
	{
		szTmpUser = pNet->userName().trimmed();
		if(!szTmpUser.isEmpty())
		{
			if(!_OUTPUT_MUTE)
				m_pConsole->output(KVI_OUT_VERBOSE, __tr2qs("Using network specific username (%Q)"), &(szTmpUser));
		}
		else
		{
			szTmpUser = KVI_OPTION_STRING(KviOption_stringUsername).trimmed();
			if(szTmpUser.isEmpty())
				szTmpUser = KVI_DEFAULT_USERNAME;
		}
	}

	m_pUserInfo->setUserName(szTmpUser);

	// Nick
	if(pServer->reconnectInfo())
		szTmpNick = pServer->reconnectInfo()->m_szNick;

	// set this one as default (also for pickNextLoginNickName() below)
	m_pStateData->setLoginNickNameState(KviIrcConnectionStateData::UsedConnectionSpecificNickName);

	QString szChoiceDescription;

	if(!szTmpNick.isEmpty())
	{
		szChoiceDescription = __tr2qs("connection specific");
	}
	else
	{
		szTmpNick = pickNextLoginNickName(true, QString(), szChoiceDescription);
		KVI_ASSERT(!szTmpNick.isEmpty());
	}

	if(!_OUTPUT_MUTE)
	{
		QString szOut = __tr2qs("Using %1 as nickname").arg(szTmpNick);
		if(_OUTPUT_VERBOSE)
			szOut += QString::fromLatin1(" (%1)").arg(szChoiceDescription);

		m_pConsole->outputNoFmt(KVI_OUT_VERBOSE, szOut);
	}

	m_pUserInfo->setNickName(szTmpNick);

	// Real name
	szTmpName = pServer->realName().trimmed();
	if(!szTmpName.isEmpty())
	{
		if(!_OUTPUT_MUTE)
			m_pConsole->output(KVI_OUT_VERBOSE, __tr2qs("Using server specific real name (%Q)"), &(szTmpName));
	}
	else
	{
		szTmpName = pNet->realName().trimmed();
		if(!szTmpName.isEmpty())
		{
			if(!_OUTPUT_MUTE)
				m_pConsole->output(KVI_OUT_VERBOSE, __tr2qs("Using network specific real name (%Q)"), &(szTmpName));
		}
		else
		{
			szTmpName = KVI_OPTION_STRING(KviOption_stringRealname);
		}
	}

	useRealName(szTmpName);

	// Pass
	if(pServer->reconnectInfo())
		szTmpPass = pServer->reconnectInfo()->m_szPass;

	if(!szTmpPass.isEmpty())
	{
		if(!_OUTPUT_MUTE)
		{
			QString szHidden = QString(szTmpPass.length(), QChar('*'));
			m_pConsole->output(KVI_OUT_VERBOSE, __tr2qs("Using connection specific password (%Q)"), &(szHidden));
		}
	}
	else
	{
		szTmpPass = pServer->password().trimmed();
		if(!szTmpPass.isEmpty())
		{
			if(!_OUTPUT_MUTE)
			{
				QString szHidden = QString(szTmpPass.length(), QChar('*'));
				m_pConsole->output(KVI_OUT_VERBOSE, __tr2qs("Using server specific password (%Q)"), &(szHidden));
			}
		}
		else
		{
			szTmpPass = pNet->password().trimmed();
			if(!szTmpPass.isEmpty())
			{
				if(!_OUTPUT_MUTE)
				{
					QString szHidden = QString(szTmpPass.length(), QChar('*'));
					m_pConsole->output(KVI_OUT_VERBOSE, __tr2qs("Using network specific password (%Q)"), &(szHidden));
				}
			}
		}
	}

	m_pUserInfo->setPassword(szTmpPass);

	// Check for identity profiles

	// FIXME: Shouldn't this be inside the code above ?
	KviIdentityProfileSet * pSet = KviIdentityProfileSet::instance();
	bool bProfilesEnabled = pSet ? (pSet->isEnabled() && !pSet->isEmpty()) : false;
	if(bProfilesEnabled)
		useProfileData(pSet, pNet->name());

	// FIXME: The server's encoding!
	setupTextCodec();
	QByteArray szNick = encodeText(m_pUserInfo->nickName()); // never empty
	QByteArray szUser = encodeText(m_pUserInfo->userName()); // never empty
	QByteArray szReal = encodeText(m_pUserInfo->realName()); // may be empty
	QByteArray szPass = encodeText(m_pUserInfo->password()); // may be empty

	if(!_OUTPUT_MUTE)
		m_pConsole->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Logging in as %Q!%Q :%Q"),
		    &(m_pUserInfo->nickName()), &(m_pUserInfo->userName()), &(m_pUserInfo->realName()));

	// spity, 27.03.2005: follow the RFC2812 suggested order for connection registration
	// first the PASS, then NICK and then USER

	// The pass ?
	if(!m_pUserInfo->password().isEmpty())
	{
		if(!sendFmtData("PASS :%s", szPass.data()))
		{
			// disconnected in the meantime
			return;
		}
	}

	// disconnected :(
	if(!sendFmtData("NICK %s", szNick.data()))
		return;

	unsigned int iGenderAvatarTag = 0;

	if(KVI_OPTION_BOOL(KviOption_boolPrependGenderInfoToRealname) && !KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender).isEmpty())
	{
		if(KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender).startsWith("m", Qt::CaseInsensitive))
			iGenderAvatarTag |= 1;
		else if(KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender).startsWith("f", Qt::CaseInsensitive))
			iGenderAvatarTag |= 2;
	}

	if(KVI_OPTION_BOOL(KviOption_boolPrependAvatarInfoToRealname) && !KVI_OPTION_STRING(KviOption_stringMyAvatar).isEmpty())
	{
		iGenderAvatarTag |= 4;
	}

	if(KVI_OPTION_BOOL(KviOption_boolPrependNickColorInfoToRealname) && KVI_OPTION_BOOL(KviOption_boolUseSpecifiedSmartColorForOwnNick))
	{
		QString szTags;
		int iBack = KVI_OPTION_UINT(KviOption_uintUserIrcViewOwnBackground);
		if(iBack != KviControlCodes::Transparent)
		{
			szTags.sprintf("%c%d,%d%c",
			    KviControlCodes::Color,
			    KVI_OPTION_UINT(KviOption_uintUserIrcViewOwnForeground),
			    iBack,
			    KviControlCodes::Reset);
		}
		szReal.prepend(szTags.toUtf8());
	}

	if(iGenderAvatarTag != 0)
	{
		QString szTags;
		szTags.sprintf("%c%d%c",
		    KviControlCodes::Color,
		    iGenderAvatarTag,
		    KviControlCodes::Reset);
		szReal.prepend(szTags.toUtf8());
	}

	// disconnected in the meantime!
	if(!sendFmtData("USER %s 0 %s :%s", szUser.data(), pServer->hostName().toUtf8().data(), szReal.data()))
		return;

	// permanent info in the user database
	m_pConsole->notifyListView()->join(m_pUserInfo->nickName(), "*", "*");

	// set own avatar if we have it
	KviIrcUserEntry * e = userDataBase()->find(userInfo()->nickName());

	// our nick should be there!
	if(e && !e->avatar())
	{
		KviAvatar * av = m_pConsole->defaultAvatarFromOptions();
		if(av)
		{
			e->setAvatar(av);
			m_pConsole->notifyListView()->avatarChanged(userInfo()->nickName());
		}
	} // else buuug, couldn't find our nick

	if(KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender).startsWith("m", Qt::CaseInsensitive))
		e->setGender(KviIrcUserEntry::Male);
	else if(KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender).startsWith("f", Qt::CaseInsensitive))
		e->setGender(KviIrcUserEntry::Female);

	// on connect stuff ?

	QString szTmp = pNet->onConnectCommand().trimmed();
	if(!szTmp.isEmpty())
	{
		if(_OUTPUT_VERBOSE)
			m_pConsole->output(KVI_OUT_VERBOSE, __tr2qs("Executing scheduled network specific \"on connect\" commands"));
		KviKvsScript::run(szTmp, m_pConsole);
	}

	szTmp = pServer->onConnectCommand().trimmed();
	if(!szTmp.isEmpty())
	{
		if(_OUTPUT_VERBOSE)
			m_pConsole->output(KVI_OUT_VERBOSE, __tr2qs("Executing scheduled server specific \"on connect\" commands"));
		KviKvsScript::run(szTmp, m_pConsole);
	}

	szTmp = m_pUserIdentity->onConnectCommand().trimmed();
	if(!szTmp.isEmpty())
	{
		if(_OUTPUT_VERBOSE)
			m_pConsole->output(KVI_OUT_VERBOSE, __tr2qs("Executing scheduled identity specific \"on connect\" commands"));
		KviKvsScript::run(szTmp, m_pConsole);
	}

	// and wait for the server to agree...
}

void KviIrcConnection::nickChange(const QString & szNewNick)
{
	// FIXME: should the new nickname be decoded in some way ?
	m_pConsole->notifyListView()->nickChange(m_pUserInfo->nickName(), szNewNick);
	m_pUserInfo->setNickName(szNewNick);
	m_pConsole->updateCaption();
	g_pMainWindow->childConnectionNickNameChange(this);
	emit nickNameChanged();
	g_pApp->addRecentNickname(szNewNick);
}

bool KviIrcConnection::changeUserMode(char cMode, bool bSet)
{
	if(bSet)
	{
		if(m_pUserInfo->hasUserMode(cMode))
			return false;
		m_pUserInfo->addUserMode(cMode);
	}
	else
	{
		if(!m_pUserInfo->hasUserMode(cMode))
			return false;
		m_pUserInfo->removeUserMode(cMode);
	}
	m_pConsole->updateCaption();
	g_pMainWindow->childConnectionUserModeChange(this);
	emit userModeChanged();
	return true;
}

void KviIrcConnection::gatherChannelAndPasswordPairs(std::vector<std::pair<QString, QString>> & lChannelsAndPasses)
{
	for(const auto & c : m_pChannelList)
		lChannelsAndPasses.emplace_back(
		    c->windowName(),
		    c->hasChannelMode('k') ? c->channelModeParam('k') : QString());
}

void KviIrcConnection::gatherQueryNames(QStringList & lQueryNames)
{
	for(const auto & q : m_pQueryList)
		lQueryNames.append(q->target());
}

void KviIrcConnection::joinChannels(const std::vector<std::pair<QString, QString>> & lChannelsAndPasses)
{
	if(lChannelsAndPasses.empty())
		return;

	// Sort the list so the channels with passwords come first
	std::vector<std::pair<QString, QString>> lSorted = lChannelsAndPasses;

	std::sort(lSorted.begin(), lSorted.end(),
		[](const std::pair<QString, QString> & left,
		   const std::pair<QString, QString> & right)
	{
		return left.second.count() > right.second.count();
	});

	// We send the channel list in chunks to avoid overflowing the 510 character limit on the message.
	QString szChans, szPasses;

	for(auto & oChanAndPass : lSorted)
	{

		if(!szChans.isEmpty())
			szChans.append(',');
		szChans.append(oChanAndPass.first);

		if(!oChanAndPass.second.isEmpty())
		{
			if(!szPasses.isEmpty())
				szPasses.append(',');
			szPasses.append(oChanAndPass.second);
		}

		// empirical limit
		if((szChans.length() + szPasses.length()) > 450)
		{
			QString szCommand = szChans;
			if(!szPasses.isEmpty())
			{
				szCommand.append(' ');
				szCommand.append(szPasses);
			}
			sendFmtData("JOIN %s", encodeText(szCommand).data());
			szChans.clear();
			szPasses.clear();
		}
	}

	QString szCommand = szChans;
	if(!szPasses.isEmpty())
	{
		szCommand.append(' ');
		szCommand.append(szPasses);
	}
	sendFmtData("JOIN %s", encodeText(szCommand).data());
}

void KviIrcConnection::loginComplete(const QString & szNickName)
{
	if(context()->state() == KviIrcContext::Connected)
		return;

	// Stop ignoring ERR_NOTREGISTERED errors. This is related to the initial CAP LS message.
	// Well.. we should already have stopped ignoring the errors as we should have received
	// the one we expected (because of the PING after CAP LS). Moreover the server shouldn't
	// be sending these messages after the login has been completed.
	// ...but to be on the safe side we just disable the special handling here.
	m_pStateData->setIgnoreOneYouHaveNotRegisteredError(false);

	context()->loginComplete();

	if(m_bIdentdAttached)
	{
		g_pMainWindow->executeInternalCommand(KVI_INTERNALCOMMAND_IDENT_STOP);
		m_bIdentdAttached = false;
	}

	if(szNickName != m_pUserInfo->nickName())
	{
		m_pConsole->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("The server refused the suggested nickname (%s) and named you %s instead"), m_pUserInfo->nickName().toUtf8().data(), szNickName.toUtf8().data());
		m_pConsole->notifyListView()->nickChange(m_pUserInfo->nickName(), szNickName);
		m_pUserInfo->setNickName(szNickName);
	}

	g_pApp->addRecentNickname(szNickName);

	if(!KVS_TRIGGER_EVENT_0_HALTED(KviEvent_OnIRC, m_pConsole))
		m_pConsole->outputNoFmt(KVI_OUT_IRC, __tr2qs("Login operations complete, happy ircing!"));

	resurrectDeadQueries();

	// on connect stuff ?
	QString szTmp = target()->network()->onLoginCommand().trimmed();
	if(!szTmp.isEmpty())
	{
		if(_OUTPUT_VERBOSE)
			m_pConsole->output(KVI_OUT_VERBOSE, __tr2qs("Executing scheduled network specific \"on login\" commands"));
		KviKvsScript::run(szTmp, m_pConsole);
	}

	szTmp = target()->server()->onLoginCommand().trimmed();
	if(!szTmp.isEmpty())
	{
		if(_OUTPUT_VERBOSE)
			m_pConsole->output(KVI_OUT_VERBOSE, __tr2qs("Executing scheduled server specific \"on login\" commands"));
		KviKvsScript::run(szTmp, m_pConsole);
	}

	szTmp = m_pUserIdentity->onLoginCommand().trimmed();
	if(!szTmp.isEmpty())
	{
		if(_OUTPUT_VERBOSE)
			m_pConsole->output(KVI_OUT_VERBOSE, __tr2qs("Executing scheduled identity specific \"on login\" commands"));
		KviKvsScript::run(szTmp, m_pConsole);
	}

	// Set the configured umode
	QString szModeStr = target()->server()->initUMode();

	if(szModeStr.isEmpty())
		szModeStr = KVI_OPTION_STRING(KviOption_stringDefaultUserMode);

	if(!szModeStr.isEmpty()) // may be still empty if there are no default modes, if so, don't send the MODE command at all
	{
		if(_OUTPUT_VERBOSE)
			m_pConsole->output(KVI_OUT_VERBOSE, __tr2qs("Setting configured user mode"));
		sendFmtData("MODE %s +%s", encodeText(m_pUserInfo->nickName()).data(), encodeText(szModeStr).data());
	}

	delayedStartNotifyList();
	restartLagMeter();

	if(KVI_OPTION_BOOL(KviOption_boolShowChannelsJoinOnIrc))
		g_pMainWindow->executeInternalCommand(KVI_INTERNALCOMMAND_CHANNELSJOIN_OPEN);

	// join saved channels

	// FIXME: It should be possible to delay the channel join process after identification
	//        (or maybe just delay by a fixed amount ?)
	//        (or maybe retry joining after a while ?)
	//
	// It's quite hard to figure out when the identification has taken place.
	// There is no standard for this purpose. We might look at a special NOTICE by NickServ
	// but the format is not well defined and we would need to make it user configurable.
	// Not very reliable, actually :/

	QString szChannels, szProtectedChannels, szPasswords, szCurPass, szCurChan;

	if(!(m_pStateData->commandToExecAfterConnect().isEmpty()))
	{
		KviCString tmp = m_pStateData->commandToExecAfterConnect();
		KviKvsScript::run(tmp.ptr(), m_pConsole);
	}

	bool bJoinStdChannels = true;

	if(target()->server()->reconnectInfo())
	{
		if(!target()->server()->reconnectInfo()->m_lJoinChannels.empty())
		{
			bJoinStdChannels = false;
			joinChannels(target()->server()->reconnectInfo()->m_lJoinChannels);
		}

		KviQueryWindow * pQuery;

		for(const auto & it : target()->server()->reconnectInfo()->m_lOpenQueries)
		{
			QString szNick = it;
			pQuery = findQuery(szNick);
			if(!pQuery)
			{
				pQuery = createQuery(szNick);
				QString szUser;
				QString szHost;

				KviIrcUserDataBase * db = userDataBase();
				if(db)
				{
					KviIrcUserEntry * e = db->find(szNick);
					if(e)
					{
						szUser = e->user();
						szHost = e->host();
					}
				}
				pQuery->setTarget(szNick, szUser, szHost);
			}
		}
		target()->server()->clearReconnectInfo();
	}

	if(bJoinStdChannels)
	{
		std::vector<std::pair<QString, QString>> lChansAndPass;

		if(target()->network()->autoJoinChannelList())
		{
			if(_OUTPUT_VERBOSE)
				m_pConsole->output(KVI_OUT_VERBOSE, __tr2qs("Auto-joining network specific channels"));

			QStringList * l = target()->network()->autoJoinChannelList();
			for(const auto & it : *l)
			{
				szCurPass = it.section(':', 1);
				szCurChan = it.section(':', 0, 0);
				if(szCurChan.isEmpty())
					continue;
				if(!m_pServerInfo->isSupportedChannelType(szCurChan[0]))
					szCurChan.prepend('#');
				lChansAndPass.emplace_back(szCurChan, szCurPass);
			}
		}

		if(target()->server()->autoJoinChannelList())
		{
			if(_OUTPUT_VERBOSE)
				m_pConsole->output(KVI_OUT_VERBOSE, __tr2qs("Auto-joining server specific channels"));

			QStringList * l = target()->server()->autoJoinChannelList();
			for(const auto & it : *l)
			{
				szCurPass = it.section(':', 1);
				szCurChan = it.section(':', 0, 0);
				if(szCurChan.isEmpty())
					continue;
				if(!m_pServerInfo->isSupportedChannelType(szCurChan[0]))
					szCurChan.prepend('#');
				lChansAndPass.emplace_back(szCurChan, szCurPass);
			}
		}

		joinChannels(lChansAndPass);
	}
}

void KviIrcConnection::incomingMessage(const char * pcMessage)
{
	// A message has arrived from the current server
	// First of all, notify the monitors
	for(auto & m : context()->monitorList())
	{
		if(m->incomingMessage(pcMessage))
			return;
	}
	// set the last message time
	m_pStatistics->setLastMessageTime(kvi_unixTime());
	// and pass it to the server parser for processing
	g_pServerParser->parseMessage(pcMessage, this);
}

void KviIrcConnection::incomingMessageNoFilter(const char * pcMessage)
{
	// set the last message time
	m_pStatistics->setLastMessageTime(kvi_unixTime());
	// and pass it to the server parser for processing
	g_pServerParser->parseMessage(pcMessage, this);
}

void KviIrcConnection::heartbeat(kvi_time_t tNow)
{
	if(m_eState == Connected)
	{
		if(KVI_OPTION_BOOL(KviOption_boolEnableAwayListUpdates))
		{
			if(m_pStateData->enabledCaps().contains("away-notify"))
			{
				// no need to send WHO requests if the away-notify extension is enabled
				return;
			}
			// update the channel WHO lists (fixes users away state)
			// first of all, we send our request not more often than every 50 secs
			if((tNow - stateData()->lastSentChannelWhoRequest()) > 50)
			{
				// we also make sure that the last sent request is older than
				// the last received reply
				if(stateData()->lastSentChannelWhoRequest() <= stateData()->lastReceivedChannelWhoReply())
				{
					// find the channel that has the older list now
					kvi_time_t tOldest = tNow;
					KviChannelWindow * pOldest = nullptr;
					for(auto & pChan : m_pChannelList)
					{
						if(pChan->lastReceivedWhoReply() < tOldest)
						{
							pOldest = pChan;
							tOldest = pChan->lastReceivedWhoReply();
						}
					}
					// if the oldest chan who list is older than 150 secs, update it
					if((tNow - tOldest) > 150)
					{
						// ok, sent the request for this channel
						stateData()->setLastSentChannelWhoRequest(tNow);
						QString szChanName = encodeText(pOldest->windowName()).data();
						if(_OUTPUT_PARANOIC)
							console()->output(KVI_OUT_VERBOSE, __tr2qs("Updating away state for channel %Q"), &szChanName);
						if(lagMeter())
						{
							KviCString tmp;
							if(serverInfo()->supportsWhox())
								tmp.append(KviCString::Format, "WHO %s %acdfhlnrsu", encodeText(pOldest->windowName()).data());
							else
								tmp.append(KviCString::Format, "WHO %s", encodeText(pOldest->windowName()).data());
							lagMeter()->lagCheckRegister(tmp.ptr(), 70);
						}
						pOldest->setSentSyncWhoRequest();
						if(serverInfo()->supportsWhox())
						{
							if(!sendFmtData("WHO %s %acdfhlnrsu", encodeText(pOldest->windowName()).data()))
								return;
						}
						else
						{
							if(!sendFmtData("WHO %s", encodeText(pOldest->windowName()).data()))
								return;
						}
					}
				}
			}
		}
	}
}

const QString & KviIrcConnection::currentServerName() const
{
	return serverInfo()->name();
}

const QString & KviIrcConnection::currentNickName() const
{
	return userInfo()->nickName();
}

const QString & KviIrcConnection::currentUserName() const
{
	return userInfo()->userName();
}
