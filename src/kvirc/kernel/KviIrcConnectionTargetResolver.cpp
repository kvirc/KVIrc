//=============================================================================
//
//   File : KviIrcConnectionTargetResolver.cpp
//   Creation date : Fri May 11 23:24:18 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviIrcConnectionTargetResolver.h"
#include "KviDnsResolver.h"
#include "KviLocale.h"
#include "KviIrcServerDataBase.h"
#include "KviProxy.h"
#include "KviProxyDataBase.h"
#include "kvi_out.h"
#include "KviOptions.h"
#include "KviIrcSocket.h"
#include "KviConsoleWindow.h"
#include "KviNetUtils.h"
#include "KviInternalCommand.h"
#include "KviMainWindow.h"
#include "KviMexLinkFilter.h"
#include "KviMemory.h"
#include "kvi_debug.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionTarget.h"
#include "KviIrcNetwork.h"

#include <QTimer>

#include <cstdlib>

extern KVIRC_API KviIrcServerDataBase * g_pServerDataBase;
extern KVIRC_API KviProxyDataBase * g_pProxyDataBase;

KviIrcConnectionTargetResolver::KviIrcConnectionTargetResolver(KviIrcConnection * pConnection)
    : QObject(), m_pConnection(pConnection)
{
	m_pConsole = m_pConnection->console();
}

KviIrcConnectionTargetResolver::~KviIrcConnectionTargetResolver()
{
	cleanup();
}

void KviIrcConnectionTargetResolver::cleanup()
{
	if(m_pProxyDns)
	{
		if(m_pProxyDns->isRunning())
		{
			// deleting a running dns may block
			// thus garbage-collect it and delete later
			//g_pGarbageCollector->collect(m_pProxyDns);
			m_pProxyDns->deleteLater();
		}
		else
		{
			// can't block : just delete it
			delete m_pProxyDns;
		}
		m_pProxyDns = nullptr;
	}
	if(m_pServerDns)
	{
		if(m_pServerDns->isRunning())
		{
			// deleting a running dns may block
			// thus garbage-collect it and delete later
			//g_pGarbageCollector->collect(m_pServerDns);
			m_pServerDns->deleteLater();
		}
		else
		{
			// can't block : just delete it
			delete m_pServerDns;
		}
		m_pServerDns = nullptr;
	}
	if(m_pStartTimer)
	{
		delete m_pStartTimer;
		m_pStartTimer = nullptr;
	}
}

void KviIrcConnectionTargetResolver::start(KviIrcConnectionTarget * t)
{
	KVI_ASSERT(m_eState == Idle);

	m_eState = Running;

	if(m_pStartTimer) // this should never happen I guess
	{
		delete m_pStartTimer;
		m_pStartTimer = nullptr;
	}
	m_pStartTimer = new QTimer(this);
	connect(m_pStartTimer, SIGNAL(timeout()), this, SLOT(asyncStartResolve()));

	m_pTarget = t;

	m_pStartTimer->start(0);
}

void KviIrcConnectionTargetResolver::abort()
{
	cleanup(); // do a cleanup to kill the timers and dns slaves

	if(m_eState == Terminated)
		return;

	m_pConsole->outputNoFmt(KVI_OUT_SYSTEMERROR, __tr2qs("Hostname resolution aborted"));
	terminate(Error, KviError::OperationAborted);
}

void KviIrcConnectionTargetResolver::asyncStartResolve()
{
	if(m_pStartTimer)
	{
		delete m_pStartTimer;
		m_pStartTimer = nullptr;
	}

	m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,
	    __tr2qs("Attempting %Q to %Q (%Q) on port %u"),
	    m_pTarget->server()->useSSL() ? &(__tr2qs("secure connection")) : &(__tr2qs("connection")),
	    &(m_pTarget->server()->hostName()),
	    &(m_pTarget->network()->name()),
	    m_pTarget->server()->port());

	if(m_pTarget->proxy())
	{
		m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,
		    __tr2qs("Attempting to 'bounce' on proxy %s on port %u (protocol %s)"),
		    m_pTarget->proxy()->hostname().toUtf8().data(),
		    m_pTarget->proxy()->port(),
		    m_pTarget->proxy()->protocolName().toUtf8().data());

		lookupProxyHostname();
	}
	else
	{
		lookupServerHostname();
	}
}

void KviIrcConnectionTargetResolver::lookupProxyHostname()
{
	bool bValidIp;
#ifdef COMPILE_IPV6_SUPPORT
	if(m_pTarget->proxy()->isIPv6())
		bValidIp = KviNetUtils::isValidStringIPv6(m_pTarget->proxy()->ip());
	else
#endif
		bValidIp = KviNetUtils::isValidStringIp(m_pTarget->proxy()->ip());

	if(bValidIp)
	{
		if(!_OUTPUT_QUIET)
			m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,
			    __tr2qs("Using cached proxy IP address (%s)"),
			    m_pTarget->proxy()->ip().toUtf8().data());
		if(m_pTarget->proxy()->protocol() != KviProxy::Http
		    && m_pTarget->proxy()->protocol() != KviProxy::Socks5)
			lookupServerHostname();
		else
			terminate(Success, KviError::Success);
	}
	else
	{
#ifdef COMPILE_IPV6_SUPPORT
		if(m_pTarget->proxy()->isIPv6())
			bValidIp = KviNetUtils::isValidStringIPv6(m_pTarget->proxy()->hostname());
		else
#endif
			bValidIp = KviNetUtils::isValidStringIp(m_pTarget->proxy()->hostname());

		if(bValidIp)
		{
			m_pTarget->proxy()->setIp(m_pTarget->proxy()->hostname());
			if(m_pTarget->proxy()->protocol() != KviProxy::Http && m_pTarget->proxy()->protocol() != KviProxy::Socks5)
				lookupServerHostname();
			else
				terminate(Success, KviError::Success);
		}
		else
		{

			if(m_pProxyDns)
			{
				qDebug("Something weird is happening, m_pProxyDns is non-zero in lookupProxyHostname()");
				delete m_pProxyDns;
				m_pProxyDns = nullptr;
			}

			m_pProxyDns = new KviDnsResolver();
			connect(m_pProxyDns, SIGNAL(lookupDone(KviDnsResolver *)), this, SLOT(proxyLookupTerminated(KviDnsResolver *)));

			if(!m_pProxyDns->lookup(m_pTarget->proxy()->hostname(),
			       m_pTarget->proxy()->isIPv6() ? KviDnsResolver::IPv6 : KviDnsResolver::IPv4))
			{
				m_pConsole->outputNoFmt(KVI_OUT_SYSTEMWARNING,
				    __tr2qs("Unable to look up the IRC proxy hostname: Can't start the DNS slave"));
				m_pConsole->outputNoFmt(KVI_OUT_SYSTEMWARNING,
				    __tr2qs("Resuming direct server connection"));
				// FIXME: #warning "Option for resuming direct connection or not ?"
				delete m_pProxyDns;
				m_pProxyDns = nullptr;
				m_pTarget->clearProxy();
				lookupServerHostname();
			}
			else
			{
				if(!_OUTPUT_MUTE)
					m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,
					    __tr2qs("Looking up the proxy hostname (%s)..."),
					    m_pTarget->proxy()->hostname().toUtf8().data());
			}
		}
	}
}

void KviIrcConnectionTargetResolver::proxyLookupTerminated(KviDnsResolver *)
{
	if(m_pProxyDns->state() != KviDnsResolver::Success)
	{
		QString szErr = m_pProxyDns->errorString();
		m_pConsole->output(KVI_OUT_SYSTEMERROR,
		    __tr2qs("Can't find the proxy IP address: %Q"),
		    &szErr);
		// FIXME: #warning "Option to resume the direct connection if proxy failed ?"
		m_pConsole->output(KVI_OUT_SYSTEMERROR,
		    __tr2qs("Resuming direct server connection"));
		m_pTarget->clearProxy();
	}
	else
	{
		QString szFirstIpAddress = m_pProxyDns->firstIpAddress();
		if(!_OUTPUT_MUTE)
			m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,
			    __tr2qs("Proxy hostname resolved to %Q"), &szFirstIpAddress);

		m_pTarget->proxy()->setIp(m_pProxyDns->firstIpAddress());
		g_pProxyDataBase->updateProxyIp(m_pTarget->proxy()->ip().toUtf8().data(), szFirstIpAddress.toUtf8().data());
	}

	delete m_pProxyDns;
	m_pProxyDns = nullptr;
	if(m_pTarget->proxy())
	{
		if(m_pTarget->proxy()->protocol() == KviProxy::Http
		    || m_pTarget->proxy()->protocol() == KviProxy::Socks5)
		{
			terminate(Success, KviError::Success);
			return;
		}
	}
	lookupServerHostname();
}

void KviIrcConnectionTargetResolver::lookupServerHostname()
{
	bool bValidIp;

#ifdef COMPILE_IPV6_SUPPORT
	if(m_pTarget->server()->isIPv6())
		bValidIp = KviNetUtils::isValidStringIPv6(m_pTarget->server()->ip());
	else
#endif
		bValidIp = KviNetUtils::isValidStringIp(m_pTarget->server()->ip());

	if(bValidIp && m_pTarget->server()->cacheIp())
	{
		if(!_OUTPUT_QUIET)
			m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,
			    __tr2qs("Using cached server IP address (%s)"),
			    m_pTarget->server()->ip().toUtf8().data());
		haveServerIp();
	}
	else
	{
#ifdef COMPILE_IPV6_SUPPORT
		if(m_pTarget->server()->isIPv6())
			bValidIp = KviNetUtils::isValidStringIPv6(m_pTarget->server()->hostName());
		else
#endif
			bValidIp = KviNetUtils::isValidStringIp(m_pTarget->server()->hostName());

		if(bValidIp)
		{
			m_pTarget->server()->setIp(m_pTarget->server()->hostName());
			haveServerIp();
		}
		else
		{
			if(m_pServerDns)
			{
				qDebug("Something weird is happening, m_pServerDns is non-zero in lookupServerHostname()");
				delete m_pServerDns;
				m_pServerDns = nullptr;
			}
			m_pServerDns = new KviDnsResolver();
			connect(m_pServerDns, SIGNAL(lookupDone(KviDnsResolver *)), this,
			    SLOT(serverLookupTerminated(KviDnsResolver *)));
			if(!m_pServerDns->lookup(m_pTarget->server()->hostName(),
			       m_pTarget->server()->isIPv6() ? KviDnsResolver::IPv6 : KviDnsResolver::IPv4))
			{
				m_pConsole->outputNoFmt(KVI_OUT_SYSTEMERROR,
				    __tr2qs("Unable to look up the server hostname: Can't start the DNS slave"));
				terminate(Error, KviError::InternalError);
			}
			else
			{
				if(!_OUTPUT_MUTE)
					m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,
					    __tr2qs("Looking up the server hostname (%s)..."),
					    m_pTarget->server()->hostName().toUtf8().data());
			}
		}
	}
}

void KviIrcConnectionTargetResolver::serverLookupTerminated(KviDnsResolver *)
{
	if(m_pServerDns->state() != KviDnsResolver::Success)
	{
		QString szErr = m_pServerDns->errorString();
		m_pConsole->output(KVI_OUT_SYSTEMERROR,
		    __tr2qs("Can't find the server IP address: %Q"),
		    &szErr);

#ifdef COMPILE_IPV6_SUPPORT
		if(!(m_pTarget->server()->isIPv6()))
		{
			m_pConsole->output(KVI_OUT_SYSTEMERROR,
			    __tr2qs("If this server is an IPv6 one, try /server -i %Q"),
			    &(m_pTarget->server()->hostName()));
		}
#endif
		terminate(Error, m_pServerDns->error());
		return;
	}

	QString szIpAddress;

	if(m_pServerDns->ipAddressCount() > 1)
	{
		if(KVI_OPTION_BOOL(KviOption_boolPickRandomIpAddressForRoundRobinServers))
		{
			if(!_OUTPUT_MUTE)
				m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,
				    __tr2qs("Server has %d IP addresses, picking a random one"),
				    m_pServerDns->ipAddressCount());

			int r = ::rand() % m_pServerDns->ipAddressCount();
			szIpAddress = m_pServerDns->ipAddressList()[r];
		}
		else
		{
			if(!_OUTPUT_MUTE)
				m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,
				    __tr2qs("Server has %d IP addresses, using the first one"),
				    m_pServerDns->ipAddressCount());
			szIpAddress = m_pServerDns->firstIpAddress();
		}
	}
	else
	{
		szIpAddress = m_pServerDns->firstIpAddress();
	}

	if(!_OUTPUT_MUTE)
		m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,
		    __tr2qs("Server hostname resolved to %Q"),
		    &szIpAddress);
	m_pTarget->server()->setIp(szIpAddress);

	const QString & szFirstHostname = m_pServerDns->hostName();

	if(!KviQString::equalCI(m_pTarget->server()->hostName(), m_pServerDns->hostName()))
	{
		if(!_OUTPUT_QUIET)
			m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,
			    __tr2qs("Real hostname for %Q is %Q"),
			    &(m_pTarget->server()->hostName()),
			    &szFirstHostname);
		m_pTarget->server()->setHostName(szFirstHostname);
	}

	m_pTarget->server()->setIp(szIpAddress);

	delete m_pServerDns;
	m_pServerDns = nullptr;
	haveServerIp();
}

bool KviIrcConnectionTargetResolver::validateLocalAddress(const QString & szAddress, QString & szBuffer)
{
// szAddress may be an ip address or an interface name
#ifdef COMPILE_IPV6_SUPPORT
	if(m_pTarget->server()->isIPv6())
	{
		if(KviNetUtils::isValidStringIPv6(szAddress))
		{
			szBuffer = szAddress;
			return true;
		}
	}
	else
	{
#endif
		if(KviNetUtils::isValidStringIp(szAddress))
		{
			szBuffer = szAddress;
			return true;
		}
#ifdef COMPILE_IPV6_SUPPORT
	}
#endif

	// is it an interface name ?
	return KviNetUtils::getInterfaceAddress(szAddress, szBuffer);
}

void KviIrcConnectionTargetResolver::haveServerIp()
{
	if(KVI_OPTION_BOOL(KviOption_boolUseIdentService) && !KVI_OPTION_BOOL(KviOption_boolUseIdentServiceOnlyOnConnect))
		g_pMainWindow->executeInternalCommand(KVI_INTERNALCOMMAND_IDENT_START);

	QString bindAddress;

	if(m_pTarget->hasBindAddress())
	{
		if(!validateLocalAddress(m_pTarget->bindAddress(), bindAddress))
		{
			const QString & szBindAddress = m_pTarget->bindAddress();
			if(szBindAddress.contains('.') || szBindAddress.contains(':'))
			{
				if(!_OUTPUT_MUTE)
					m_pConsole->output(KVI_OUT_SYSTEMWARNING,
					    __tr2qs("The specified bind address (%Q) is not valid"),
					    &szBindAddress);
			}
			else
			{
				if(!_OUTPUT_MUTE)
					m_pConsole->output(KVI_OUT_SYSTEMWARNING,
					    __tr2qs("The specified bind address (%Q) is not valid (the interface it refers to might be down)"),
					    &szBindAddress);
			}
		}
	}
	else
	{
// the options specify a bind address ?
#ifdef COMPILE_IPV6_SUPPORT
		if(m_pTarget->server()->isIPv6())
		{
			if(KVI_OPTION_BOOL(KviOption_boolBindIrcIPv6ConnectionsToSpecifiedAddress))
			{
				if(!KVI_OPTION_STRING(KviOption_stringIPv6ConnectionBindAddress).isEmpty())
				{
					if(!validateLocalAddress(KVI_OPTION_STRING(KviOption_stringIPv6ConnectionBindAddress), bindAddress))
					{
						// if it is not an interface name, kill it for now and let the user correct the address
						if(KVI_OPTION_STRING(KviOption_stringIPv6ConnectionBindAddress).contains(':'))
						{
							if(!_OUTPUT_MUTE)
								m_pConsole->output(KVI_OUT_SYSTEMWARNING,
								    __tr2qs("The system-wide IPv6 bind address (%s) is not valid"),
								    KVI_OPTION_STRING(KviOption_stringIPv6ConnectionBindAddress).toUtf8().data());
							KVI_OPTION_BOOL(KviOption_boolBindIrcIPv6ConnectionsToSpecifiedAddress) = false;
						}
						else
						{
							// this is an interface address: might be down
							if(!_OUTPUT_MUTE)
								m_pConsole->output(KVI_OUT_SYSTEMWARNING,
								    __tr2qs("The system-wide IPv6 bind address (%s) is not valid (the interface it refers to might be down)"),
								    KVI_OPTION_STRING(KviOption_stringIPv6ConnectionBindAddress).toUtf8().data());
						}
					}
				}
				else
				{
					// empty address....kill it
					KVI_OPTION_BOOL(KviOption_boolBindIrcIPv6ConnectionsToSpecifiedAddress) = false;
				}
			}
		}
		else
		{
#endif
			if(KVI_OPTION_BOOL(KviOption_boolBindIrcIPv4ConnectionsToSpecifiedAddress))
			{
				if(!KVI_OPTION_STRING(KviOption_stringIPv4ConnectionBindAddress).isEmpty())
				{
					if(!validateLocalAddress(KVI_OPTION_STRING(KviOption_stringIPv4ConnectionBindAddress), bindAddress))
					{
						// if it is not an interface name, kill it for now and let the user correct the address
						if(KVI_OPTION_STRING(KviOption_stringIPv4ConnectionBindAddress).contains(':'))
						{
							if(!_OUTPUT_MUTE)
								m_pConsole->output(KVI_OUT_SYSTEMWARNING,
								    __tr2qs("The system-wide IPv4 bind address (%s) is not valid"),
								    KVI_OPTION_STRING(KviOption_stringIPv4ConnectionBindAddress).toUtf8().data());
							KVI_OPTION_BOOL(KviOption_boolBindIrcIPv4ConnectionsToSpecifiedAddress) = false;
						}
						else
						{
							// this is an interface address: might be down
							if(!_OUTPUT_MUTE)
								m_pConsole->output(KVI_OUT_SYSTEMWARNING,
								    __tr2qs("The system-wide IPv4 bind address (%s) is not valid (the interface it refers to might be down)"),
								    KVI_OPTION_STRING(KviOption_stringIPv4ConnectionBindAddress).toUtf8().data());
						}
					}
				}
				else
				{
					// empty address....kill it
					KVI_OPTION_BOOL(KviOption_boolBindIrcIPv4ConnectionsToSpecifiedAddress) = false;
				}
			}
#ifdef COMPILE_IPV6_SUPPORT
		}
#endif
	}

	m_pTarget->setBindAddress(bindAddress);
	terminate(Success, KviError::Success);
}

void KviIrcConnectionTargetResolver::terminate(Status s, int iLastError)
{
	KVI_ASSERT(m_eState != Terminated);
	cleanup(); // do a cleanup anyway
	m_eState = Terminated;
	m_eStatus = s;
	m_iLastError = iLastError;
	emit terminated();
}
