//=============================================================================
//
//   File : KviIrcLink.cpp
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

#include "KviIrcLink.h"
#include "KviDnsResolver.h"
#include "KviLocale.h"
#include "KviIrcServerDataBase.h"
#include "KviProxy.h"
#include "KviProxyDataBase.h"
#include "KviError.h"
#include "kvi_out.h"
#include "KviOptions.h"
#include "KviIrcSocket.h"
#include "KviConsoleWindow.h"
#include "KviNetUtils.h"
#include "KviInternalCommand.h"
#include "KviMainWindow.h"
#include "KviMexLinkFilter.h"
#include "KviMemory.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionTarget.h"
#include "KviIrcConnectionTargetResolver.h"
#include "KviDataBuffer.h"
#include "kvi_debug.h"

#include <QTimer>

extern KVIRC_API KviIrcServerDataBase * g_pServerDataBase;
extern KVIRC_API KviProxyDataBase * g_pProxyDataBase;

KviIrcLink::KviIrcLink(KviIrcConnection * pConnection)
    : QObject(), m_pConnection(pConnection)
{
	m_pTarget = pConnection->target();
	m_pConsole = m_pConnection->console();
}

KviIrcLink::~KviIrcLink()
{
	if(m_pResolver)
		delete m_pResolver;

	destroySocket();

	if(m_pReadBuffer)
		KviMemory::free(m_pReadBuffer);
}

//
// KviIrcSocket management
//

void KviIrcLink::linkFilterDestroyed()
{
	m_pLinkFilter = nullptr;
	m_pConsole->output(KVI_OUT_SYSTEMWARNING,
	    __tr2qs("Oops! For some reason the link filter object has been destroyed"));
}

void KviIrcLink::destroySocket()
{
	if(m_pLinkFilter)
	{
		QObject::disconnect(m_pLinkFilter, nullptr, this, nullptr);
		// the module extension server links must be destroyed in the module that provided it
		m_pLinkFilter->die();
		m_pLinkFilter = nullptr;
	}

	if(m_pSocket)
	{
		// We use deleteLater() here, since we could actually be inside an event
		// related to the QSocketNotifier that the socket is attached to...
		m_pSocket->deleteLater();
		m_pSocket = nullptr;
	}
}

void KviIrcLink::createSocket(const QString & szLinkFilterName)
{
	destroySocket(); // make sure we do not leak memory

	m_pSocket = new KviIrcSocket(this);

	if(szLinkFilterName.isEmpty())
		return;

	if(KviQString::equalCI(szLinkFilterName, "irc"))
		return;

	m_pLinkFilter = (KviMexLinkFilter *)g_pModuleExtensionManager->allocateExtension("linkfilter",
	    szLinkFilterName.toUtf8().data(), m_pConsole, nullptr, this, szLinkFilterName.toUtf8().data());

	if(m_pLinkFilter)
	{
		connect(m_pLinkFilter, SIGNAL(destroyed()), this, SLOT(linkFilterDestroyed()));
		m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,
		    __tr2qs("Using filtered IRC protocol: link filter is \"%Q\""), &szLinkFilterName);
		return;
	}

	m_pConsole->output(KVI_OUT_SYSTEMWARNING,
	    __tr2qs("Failed to set up the link filter \"%Q\", will try with plain IRC"), &szLinkFilterName);
}

//
// Connection related operations
//

void KviIrcLink::abort()
{
	if(m_pSocket)
	{
		m_pSocket->abort();
		return;
	}

	if(m_pResolver)
	{
		m_pResolver->abort();
		return;
	}
}

void KviIrcLink::start()
{
	m_eState = Connecting;
	if(m_pResolver)
		delete m_pResolver; // this should never happen

	m_pResolver = new KviIrcConnectionTargetResolver(m_pConnection);
	connect(m_pResolver, SIGNAL(terminated()), this, SLOT(resolverTerminated()));
	m_pResolver->start(m_pTarget);
}

void KviIrcLink::resolverTerminated()
{
	if(!m_pResolver)
	{
		qDebug("Oops! The resolverTerminated() was triggered without a resolver");
		return;
	}

	if(m_pResolver->status() != KviIrcConnectionTargetResolver::Success)
	{
		m_eState = Idle;
		m_pConnection->linkAttemptFailed(m_pResolver->lastError());
		return;
	}

	// resolver terminated successfully
	delete m_pResolver;
	m_pResolver = nullptr;

	createSocket(m_pTarget->server()->linkFilter());

	KviError::Code eError = m_pSocket->startConnection(m_pTarget->server(), m_pTarget->proxy(),
	    m_pTarget->bindAddress().isEmpty() ? nullptr : m_pTarget->bindAddress().toUtf8().data());

	if(eError != KviError::Success)
	{
		QString szStrDescription(KviError::getDescription(eError));
		m_pConsole->output(KVI_OUT_SYSTEMERROR,
		    __tr2qs("Failed to start the connection: %Q"),
		    &szStrDescription);
		//			&(KviError::getDescription(eError)));

		m_eState = Idle;
		m_pConnection->linkAttemptFailed(eError);
	}
}

//
// Incoming data processing
//

void KviIrcLink::processData(char * buffer, int iLen)
{
	if(m_pLinkFilter)
	{
		m_pLinkFilter->processData(buffer, iLen);
		return;
	}

	char * p = buffer;
	char * cBeginOfCurData = buffer;
	int iBufLen = 0;
	char * cMessageBuffer = (char *)KviMemory::allocate(1);

	while(*p)
	{
		if((*p == '\r') || (*p == '\n'))
		{
			//found a CR or LF...
			//prepare a message buffer
			iBufLen = p - cBeginOfCurData;
			//check for previous unterminated data
			if(m_uReadBufferLen > 0)
			{
				KVI_ASSERT(m_pReadBuffer);
				cMessageBuffer = (char *)KviMemory::reallocate(cMessageBuffer, iBufLen + m_uReadBufferLen + 1);
				KviMemory::move(cMessageBuffer, m_pReadBuffer, m_uReadBufferLen);
				KviMemory::move((void *)(cMessageBuffer + m_uReadBufferLen), cBeginOfCurData, iBufLen);
				*(cMessageBuffer + iBufLen + m_uReadBufferLen) = '\0';
				m_uReadBufferLen = 0;
				KviMemory::free(m_pReadBuffer);
				m_pReadBuffer = nullptr;
			}
			else
			{
				KVI_ASSERT(!m_pReadBuffer);
				cMessageBuffer = (char *)KviMemory::reallocate(cMessageBuffer, iBufLen + 1);
				KviMemory::move(cMessageBuffer, cBeginOfCurData, iBufLen);
				*(cMessageBuffer + iBufLen) = '\0';
			}
			m_uReadPackets++;

			// FIXME: actually it can happen that the socket gets disconnected
			// in an incomingMessage() call.
			// The problem might be that some other parts of KVIrc assume
			// that the IRC context still exists after a failed write to the socket
			// (some parts don't even check the return value!)
			// If the problem presents itself again then the solution is:
			//   disable queue flushing for the "incomingMessage" call
			//   and just call queue_insertMessage()
			//   then after the call terminates flush the queue (eventually detecting
			//   the disconnect and thus destroying the IRC context).
			// For now we try to rely on the remaining parts to handle correctly
			// such conditions. Let's see...
			if(*cMessageBuffer != 0)
				m_pConnection->incomingMessage(cMessageBuffer);

			if(!m_pSocket || (m_pSocket->state() != KviIrcSocket::Connected))
			{
				// Disconnected in KviConsoleWindow::incomingMessage() call.
				// This may happen for several reasons (local event loop
				// with the user hitting the disconnect button, a scripting
				// handler event that disconnects explicitly)
				//
				// We handle it by simply returning control to readData() which
				// will return immediately (and safely) control to Qt
				KviMemory::free(cMessageBuffer);
				return;
			}

			while(*p && ((*p == '\r') || (*p == '\n')))
				p++;
			cBeginOfCurData = p;
		}
		else
			p++;
	}

	//now *p == '\0'
	//beginOfCurData points to '\0' if we have
	//no more stuff to parse, or points to something
	//different than '\r' or '\n'...
	if(*cBeginOfCurData)
	{
		//Have remaining data...in the local buffer
		iBufLen = p - cBeginOfCurData;
		if(m_uReadBufferLen > 0)
		{
			//and there was more stuff saved... (really slow connection)
			KVI_ASSERT(m_pReadBuffer);
			m_pReadBuffer = (char *)KviMemory::reallocate(m_pReadBuffer, m_uReadBufferLen + iBufLen);
			KviMemory::move((void *)(m_pReadBuffer + m_uReadBufferLen), cBeginOfCurData, iBufLen);
			m_uReadBufferLen += iBufLen;
		}
		else
		{
			//
			KVI_ASSERT(!m_pReadBuffer);
			m_uReadBufferLen = iBufLen;
			m_pReadBuffer = (char *)KviMemory::allocate(m_uReadBufferLen);
			KviMemory::move(m_pReadBuffer, cBeginOfCurData, m_uReadBufferLen);
		}
		//The m_pReadBuffer contains at max 1 IRC message...
		//that can not be longer than 510 bytes (the message is not CRLF terminated)
		// FIXME: Is this limit *really* valid on all servers ?
		if(m_uReadBufferLen > 510)
			qDebug("WARNING: receiving an invalid IRC message from server.");
	}
	KviMemory::free(cMessageBuffer);
}

//
// Outgoing data processing
//

void KviIrcLink::clearOutputQueue(bool bPrivateMessagesOnly)
{
	if(!m_pSocket)
		return; // we have no queue at all

	if(m_pLinkFilter)
	{
		m_pLinkFilter->clearOutputQueue(bPrivateMessagesOnly);
		return;
	}

	m_pSocket->clearOutputQueue(bPrivateMessagesOnly);
}

unsigned int KviIrcLink::outputQueueSize()
{
	if(!m_pSocket)
		return 0;
	if(m_pLinkFilter)
		return m_pLinkFilter->outputQueueSize();
	return m_pSocket->outputQueueSize();
}

bool KviIrcLink::sendPacket(KviDataBuffer * pData)
{
	if(!m_pSocket)
	{
		delete pData;
		pData = nullptr;
		return false;
	}

	// if we have a filter, let it do its job
	if(m_pLinkFilter)
		return m_pLinkFilter->sendPacket(pData);

	return m_pSocket->sendPacket(pData);
}

void KviIrcLink::socketStateChange()
{
	switch(m_pSocket->state())
	{
		case KviIrcSocket::Connected:
			m_eState = Connected;
			m_pConnection->linkEstablished();
			break;
		case KviIrcSocket::Idle:
		{
			State old = m_eState;
			m_eState = Idle;
			switch(old)
			{
				case Connecting:
					m_pConnection->linkAttemptFailed(m_pSocket->lastError());
					break;
				case Connected:
					m_pConnection->linkTerminated();
					break;
				default: // currently can be only Idle
					qDebug("Oops! Received a KviIrcSocket::Idle state change when KviIrcLink::m_eState was idle");
					break;
			}
		}
		break;
		case KviIrcSocket::Connecting:
			m_pConsole->output(KVI_OUT_CONNECTION, __tr2qs("Contacting %Q %s (%s) on port %u"),
			    connection()->target()->proxy() ? &(__tr2qs("proxy host")) : &(__tr2qs("IRC server")),
			    connection()->target()->proxy() ? connection()->target()->proxy()->hostname().toUtf8().data() : connection()->target()->server()->hostName().toUtf8().data(),
			    connection()->target()->proxy() ? connection()->target()->proxy()->ip().toUtf8().data() : connection()->target()->server()->ip().toUtf8().data(),
			    connection()->target()->proxy() ? connection()->target()->proxy()->port() : connection()->target()->server()->port());
			break;
		case KviIrcSocket::SSLHandshake:
			m_pConsole->output(KVI_OUT_CONNECTION, __tr2qs("Low-level transport connection established [%s (%s:%u)]"),
			    connection()->target()->proxy() ? connection()->target()->proxy()->hostname().toUtf8().data() : connection()->target()->server()->hostName().toUtf8().data(),
			    connection()->target()->proxy() ? connection()->target()->proxy()->ip().toUtf8().data() : connection()->target()->server()->ip().toUtf8().data(),
			    connection()->target()->proxy() ? connection()->target()->proxy()->port() : connection()->target()->server()->port());
			m_pConsole->outputNoFmt(KVI_OUT_CONNECTION, __tr2qs("Starting Secure Socket Layer handshake"));
			break;
		case KviIrcSocket::ProxyLogin:
			m_pConsole->output(KVI_OUT_CONNECTION, __tr2qs("%Q established [%s (%s:%u)]"),
			    connection()->link()->socket()->usingSSL() ? &(__tr2qs("Secure proxy connection")) : &(__tr2qs("Proxy connection")),
			    connection()->target()->proxy()->hostname().toUtf8().data(),
			    connection()->target()->proxy()->ip().toUtf8().data(),
			    connection()->target()->proxy()->port());
			m_pConsole->outputNoFmt(KVI_OUT_CONNECTION, __tr2qs("Negotiating relay information"));
			break;
		case KviIrcSocket::ProxyFinalV4:
			m_pConsole->outputNoFmt(KVI_OUT_CONNECTION, __tr2qs("Sent connection request, waiting for acknowledgement"));
			break;
		case KviIrcSocket::ProxyFinalV5:
			m_pConsole->outputNoFmt(KVI_OUT_CONNECTION, __tr2qs("Sent target host data, waiting for acknowledgement"));
			break;
		case KviIrcSocket::ProxySelectAuthMethodV5:
			m_pConsole->outputNoFmt(KVI_OUT_CONNECTION, __tr2qs("Sent auth method request, waiting for acknowledgement"));
			break;
		case KviIrcSocket::ProxyUserPassV5:
			m_pConsole->outputNoFmt(KVI_OUT_CONNECTION, __tr2qs("Sent username and password, waiting for acknowledgement"));
			break;
		case KviIrcSocket::ProxyFinalHttp:
			m_pConsole->outputNoFmt(KVI_OUT_CONNECTION, __tr2qs("Sent connection request, waiting for \"HTTP 200\" acknowledgement"));
			break;
		default:
			break;
	}
}
