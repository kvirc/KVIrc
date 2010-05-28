//=============================================================================
//
//   File : kvi_irclink.cpp
//   Creation date : Mon 03 May 2004 01:45:42 by Szymon Stefanek
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


#include "kvi_irclink.h"
#include "kvi_dns.h"
#include "kvi_locale.h"
#include "kvi_ircserverdb.h"
#include "kvi_proxydb.h"
#include "kvi_error.h"
#include "kvi_out.h"
#include "kvi_options.h"
#include "kvi_ircsocket.h"
#include "kvi_console.h"
#include "kvi_netutils.h"
#include "kvi_internalcmd.h"
#include "kvi_frame.h"
#include "kvi_mexlinkfilter.h"
//#include "kvi_garbage.h"
#include "kvi_malloc.h"
#include "kvi_memmove.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnectiontarget.h"
#include "kvi_ircconnectiontargetresolver.h"
#include "kvi_ircsocket.h"
#include "kvi_databuffer.h"

#define __KVI_DEBUG__
#include "kvi_debug.h"

#include <QTimer>

extern KVIRC_API KviServerDataBase   * g_pServerDataBase;
extern KVIRC_API KviProxyDataBase    * g_pProxyDataBase;
//extern KVIRC_API KviGarbageCollector * g_pGarbageCollector;


KviIrcLink::KviIrcLink(KviIrcConnection * pConnection)
: QObject()
{
	m_pConnection = pConnection;
	m_pTarget = pConnection->target();
	m_pConsole = m_pConnection->console();

	m_pSocket = 0;
	m_pLinkFilter = 0;
	m_pResolver = 0;

	m_pReadBuffer    = 0;            // incoming data buffer
	m_uReadBufferLen = 0;            // incoming data buffer length
	m_uReadPackets   = 0;            // total packets read per session

	m_eState = Idle;
}

KviIrcLink::~KviIrcLink()
{
	if(m_pResolver)
		delete m_pResolver;

	destroySocket();

	if(m_pReadBuffer)
		kvi_free(m_pReadBuffer);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// KviIrcSocket management
//

void KviIrcLink::linkFilterDestroyed()
{
	m_pLinkFilter = 0;
	m_pConsole->output(KVI_OUT_SYSTEMWARNING,
		__tr2qs("Ops... for some reason the link filter object has been destroyed"));
}

void KviIrcLink::destroySocket()
{
	if(m_pLinkFilter)
	{
		QObject::disconnect(m_pLinkFilter,0,this,0);
		// the module extension server links must be destroyed in the module that provided it
		m_pLinkFilter->die();
		m_pLinkFilter = 0;
	}

	if(m_pSocket)
	{
		// We use deleteLater() here, since we could actually be inside an event
		// related to the QSocketNotifier that the socket is attached to...
		m_pSocket->deleteLater();
		m_pSocket = 0;
	}
}

void KviIrcLink::createSocket(const QString & szLinkFilterName)
{
	destroySocket(); // make sure we do not leak memory

	m_pSocket = new KviIrcSocket(this);

	if(szLinkFilterName.isEmpty())
		return;

	if(KviQString::equalCI(szLinkFilterName,"irc"))
		return;

	m_pLinkFilter = (KviMexLinkFilter *)g_pModuleExtensionManager->allocateExtension("linkfilter",
		szLinkFilterName.toUtf8().data(),m_pConsole,0,this,szLinkFilterName.toUtf8().data());

	if(m_pLinkFilter)
	{
		connect(m_pLinkFilter,SIGNAL(destroyed()),this,SLOT(linkFilterDestroyed()));
		m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,
			__tr2qs("Using filtered IRC protocol: Link filter is \"%Q\""),&szLinkFilterName);
		return;
	}

	m_pConsole->output(KVI_OUT_SYSTEMWARNING,
		__tr2qs("Failed to set up the link filter \"%Q\", will try with plain IRC"),&szLinkFilterName);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
	connect(m_pResolver,SIGNAL(terminated()),this,SLOT(resolverTerminated()));
	m_pResolver->start(m_pTarget);
}

void KviIrcLink::resolverTerminated()
{
	if(!m_pResolver)
	{
		debug("Oops... resoverTerminated() triggered without a resolver ?");
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
	m_pResolver = 0;

	createSocket(m_pTarget->server()->linkFilter());

	int iErr = m_pSocket->startConnection(m_pTarget->server(),m_pTarget->proxy(),
		m_pTarget->bindAddress().isEmpty() ? 0 : m_pTarget->bindAddress().toUtf8().data());

	if(iErr != KviError_success)
	{
		QString szStrDescription(KviError::getDescription(iErr));
		m_pConsole->output(KVI_OUT_SYSTEMERROR,
			__tr2qs("Failed to start the connection: %Q"),
			&szStrDescription);
//			&(KviError::getDescription(iErr)));

		m_eState = Idle;
		m_pConnection->linkAttemptFailed(iErr);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Incoming data processing
//

void KviIrcLink::processData(char * buffer, int iLen)
{
	if(m_pLinkFilter)
	{
		m_pLinkFilter->processData(buffer,iLen);
		return;
	}

	register char * p = buffer;
	char * cBeginOfCurData = buffer;
	int iBufLen = 0;
	char * cMessageBuffer = (char *)kvi_malloc(1);

	while(*p)
	{
		if((*p == '\r' )||(*p == '\n'))
		{
			//found a CR or LF...
			//prepare a message buffer
			iBufLen = p - cBeginOfCurData;
			//check for previous unterminated data
			if(m_uReadBufferLen > 0)
			{
				__range_valid(m_pReadBuffer);
				cMessageBuffer = (char *)kvi_realloc(cMessageBuffer,iBufLen + m_uReadBufferLen + 1);
				kvi_memmove(cMessageBuffer,m_pReadBuffer,m_uReadBufferLen);
				kvi_memmove((void *)(cMessageBuffer + m_uReadBufferLen),cBeginOfCurData,iBufLen);
				*(cMessageBuffer + iBufLen + m_uReadBufferLen) = '\0';
				m_uReadBufferLen = 0;
				kvi_free(m_pReadBuffer);
				m_pReadBuffer = 0;
			} else {
				__range_invalid(m_pReadBuffer);
				cMessageBuffer = (char *)kvi_realloc(cMessageBuffer,iBufLen + 1);
				kvi_memmove(cMessageBuffer,cBeginOfCurData,iBufLen);
				*(cMessageBuffer + iBufLen) = '\0';
			}
			m_uReadPackets++;

			// FIXME: actually it can happen that the socket gets disconnected
			// in a incomingMessage() call.
			// The problem might be that some other parts of kvirc assume
			// that the irc context still exists after a failed write to the socket
			// (some parts don't even check the return value!)
			// If the problem presents itself again then the solution is:
			//   disable queue flushing for the "incomingMessage" call
			//   and just call queue_insertMessage()
			//   then after the call terminates flush the queue (eventually detecting
			//   the disconnect and thus destroying the irc context).
			// For now we try to rely on the remaining parts to handle correctly
			// such conditions. Let's see...
			if(strlen(cMessageBuffer) > 0)
				m_pConnection->incomingMessage(cMessageBuffer);

			if(m_pSocket->state() != KviIrcSocket::Connected)
			{
				// Disconnected in KviConsole::incomingMessage() call.
				// This may happen for several reasons (local event loop
				// with the user hitting the disconnect button, a scripting
				// handler event that disconnects explicitly)
				//
				// We handle it by simply returning control to readData() which
				// will return immediately (and safely) control to Qt
				kvi_free(cMessageBuffer);
				return;
			}

			while(*p && ((*p=='\r')||(*p=='\n')) )p++;
			cBeginOfCurData = p;

		} else p++;
	}

	//now *p == '\0'
	//beginOfCurData points to '\0' if we have
	//no more stuff to parse , or points to something
	//different than '\r' or '\n'...
	if(*cBeginOfCurData)
	{
		//Have remaining data...in the local buffer
		iBufLen = p - cBeginOfCurData;
		if(m_uReadBufferLen > 0)
		{
			//and there was more stuff saved... (really slow connection)
			__range_valid(m_pReadBuffer);
			m_pReadBuffer =(char *)kvi_realloc(m_pReadBuffer,m_uReadBufferLen + iBufLen);
			kvi_memmove((void *)(m_pReadBuffer+m_uReadBufferLen),cBeginOfCurData,iBufLen);
			m_uReadBufferLen += iBufLen;
		} else {
			//
			__range_invalid(m_pReadBuffer);
			m_uReadBufferLen = iBufLen;
			m_pReadBuffer =(char *)kvi_malloc(m_uReadBufferLen);
			kvi_memmove(m_pReadBuffer,cBeginOfCurData,m_uReadBufferLen);
		}
		//The m_pReadBuffer contains at max 1 irc message...
		//that can not be longer than 510 bytes (the message is not CRLF terminated)
		// FIXME: Is this limit *really* valid on all servers ?
		if(m_uReadBufferLen > 510) debug("WARNING: Receiving an invalid irc message from server.");
	}
	kvi_free(cMessageBuffer);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Outgoing data processing
//

bool KviIrcLink::sendPacket(KviDataBuffer * pData)
{
	if(!m_pSocket)
	{
		delete pData;
		pData = 0;
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
			m_pConnection->linkEstabilished();
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
					debug("Ooops... got a KviIrcSocket::Idle state change when KviIrcLink::m_eState was Idle");
				break;
			}
		}
		break;
		case KviIrcSocket::Connecting:
			m_pConsole->output(KVI_OUT_CONNECTION,__tr2qs("Contacting %Q %s (%s) on port %u"),
				connection()->target()->proxy() ? &(__tr2qs("proxy host")) : &(__tr2qs("IRC server")),
				connection()->target()->proxy() ? connection()->target()->proxy()->m_szHostname.toUtf8().data() : connection()->target()->server()->m_szHostname.toUtf8().data(),
				connection()->target()->proxy() ? connection()->target()->proxy()->m_szIp.toUtf8().data() : connection()->target()->server()->m_szIp.toUtf8().data(),
				connection()->target()->proxy() ? connection()->target()->proxy()->m_uPort : connection()->target()->server()->m_uPort);
		break;
		case KviIrcSocket::SSLHandshake:
			m_pConsole->output(KVI_OUT_CONNECTION,__tr2qs("Low-level transport connection established [%s (%s:%u)]"),
				connection()->target()->proxy() ? connection()->target()->proxy()->m_szHostname.toUtf8().data() : connection()->target()->server()->m_szHostname.toUtf8().data(),
				connection()->target()->proxy() ? connection()->target()->proxy()->m_szIp.toUtf8().data() : connection()->target()->server()->m_szIp.toUtf8().data(),
				connection()->target()->proxy() ? connection()->target()->proxy()->m_uPort : connection()->target()->server()->m_uPort);
			m_pConsole->outputNoFmt(KVI_OUT_CONNECTION,__tr2qs("Starting Secure Socket Layer handshake"));
		break;
		case KviIrcSocket::ProxyLogin:
			m_pConsole->output(KVI_OUT_CONNECTION,__tr2qs("%Q established [%s (%s:%u)]"),
				connection()->link()->socket()->usingSSL() ? &(__tr2qs("Secure proxy connection")) : &(__tr2qs("Proxy connection")),
				connection()->target()->proxy()->m_szHostname.toUtf8().data(),
				connection()->target()->proxy()->m_szIp.toUtf8().data(),
				connection()->target()->proxy()->m_uPort);
			m_pConsole->outputNoFmt(KVI_OUT_CONNECTION,__tr2qs("Negotiating relay information"));
		break;
		case KviIrcSocket::ProxyFinalV4:
			m_pConsole->outputNoFmt(KVI_OUT_CONNECTION,__tr2qs("Sent connection request, waiting for acknowledgement"));
		break;
		case KviIrcSocket::ProxyFinalV5:
			m_pConsole->outputNoFmt(KVI_OUT_CONNECTION,__tr2qs("Sent target host data, waiting for acknowledgement"));
		break;
		case KviIrcSocket::ProxySelectAuthMethodV5:
			m_pConsole->outputNoFmt(KVI_OUT_CONNECTION,__tr2qs("Sent auth method request, waiting for acknowledgement"));
		break;
		case KviIrcSocket::ProxyUserPassV5:
			m_pConsole->outputNoFmt(KVI_OUT_CONNECTION,__tr2qs("Sent username and password, waiting for acknowledgement"));
		break;
		case KviIrcSocket::ProxyFinalHttp:
			m_pConsole->outputNoFmt(KVI_OUT_CONNECTION,__tr2qs("Sent connection request, waiting for \"HTTP 200\" acknowledgement"));
		break;
		default:
		break;
	}
}
