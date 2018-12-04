//=============================================================================
//
//   File : KviDnsResolver.cpp
//   Creation date : Sat Jul 21 2000 17:19:31 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviDnsResolver.h"
#include "KviError.h"
#include "KviNetUtils.h"
#include "KviQString.h"

#include <QApplication>

#include <cerrno>

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#include <winsock2.h>

#ifdef COMPILE_IPV6_SUPPORT
#ifdef WIN2K
#include <ws2ip6.h>
#else
#include <ws2tcpip.h>
//#include <tpipv6.h>
#endif
#endif
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

// this is for FreeBSD
#ifndef EAI_ADDRFAMILY
#define EAI_ADDRFAMILY EAI_FAMILY
#endif

#ifndef EAI_NODATA
#define EAI_NODATA 0
#endif

KviDnsResolverResult::KviDnsResolverResult()
{
	m_eError = KviError::Success;
}

void KviDnsResolverResult::appendHostname(const QString & host)
{
	m_pHostnameList.push_back(host);
}

void KviDnsResolverResult::appendAddress(const QString & addr)
{
	m_pIpAddressList.push_back(addr);
}

KviDnsResolverThread::KviDnsResolverThread(KviDnsResolver * pDns)
    : QThread()
{
	m_pParentDns = pDns;
}

KviDnsResolverThread::~KviDnsResolverThread()
    = default;

KviError::Code KviDnsResolverThread::translateDnsError(int iErr)
{
#if defined(COMPILE_IPV6_SUPPORT) || (!defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW))

	switch(iErr)
	{
		case EAI_FAMILY:
			return KviError::UnsupportedAddressFamily;
			break;
#if(!defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)) && defined(EAI_ADDRFAMILY) && (EAI_ADDRFAMILY != EAI_FAMILY)
		case EAI_ADDRFAMILY:
			return KviError::UnsupportedAddressFamily;
			break;
#endif
// NOT FreeBSD ARE WE?
#if defined(EAI_NODATA) && (EAI_NODATA != EAI_NONAME)
		// YARR
		case EAI_NODATA:
			return KviError::ValidNameButNoIpAddress;
			break;
#endif
		case EAI_FAIL:
			return KviError::UnrecoverableNameserverError;
			break;
		case EAI_AGAIN:
			return KviError::DNSTemporaneousFault;
			break;
		// this should never happen
		case EAI_BADFLAGS:
			return KviError::DNSInternalErrorBadFlags;
			break;
		case EAI_MEMORY:
			return KviError::DNSInternalErrorOutOfMemory;
			break;
		// got this when experimenting with protocols
		case EAI_SERVICE:
			return KviError::DNSInternalErrorServiceNotSupported;
			break;
#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
		case EAI_NONAME:
			return KviError::DNSNoName;
			break;
#endif
		// got this when experimenting with protocols
		case EAI_SOCKTYPE:
			return KviError::DNSInternalErrorUnsupportedSocketType;
			break;
#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
		case EAI_SYSTEM:
			return KviError::DNSQueryFailed;
#endif
	}

#endif
	return KviError::DNSQueryFailed;
}

void KviDnsResolverThread::postDnsError(KviDnsResolverResult * pDns, KviError::Code error)
{
	pDns->setError(error);

	KviDnsResolverThreadEvent * pEvent = new KviDnsResolverThreadEvent(pDns);
	QApplication::postEvent(m_pParentDns, pEvent);
}

void KviDnsResolverThread::run()
{
	KviDnsResolverResult * dns = new KviDnsResolverResult();

	dns->setQuery(m_szQuery);

	if(m_szQuery.isEmpty())
	{
		postDnsError(dns, KviError::NoHostToResolve);
		return;
	}

#ifndef COMPILE_IPV6_SUPPORT
	if(m_queryType != KviDnsResolver::IPv4)
	{
		if(m_queryType == KviDnsResolver::IPv6)
		{
			postDnsError(dns, KviError::NoIPv6Support);
			return;
		}
		m_queryType = KviDnsResolver::IPv4;
	}
#endif

#if(defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)) && !defined(COMPILE_IPV6_SUPPORT)

	if(m_queryType == KviDnsResolver::IPv6)
	{
		postDnsError(dns, KviError::NoIPv6Support);
		return;
	}

	// gethostbyaddr and gethostbyname are thread-safe on Windoze
	struct in_addr inAddr;
	struct hostent * pHostEntry = nullptr;

	// DIE DIE!....I hope that this stuff will disappear sooner or later :)

	if(KviNetUtils::stringIpToBinaryIp(m_szQuery, &inAddr))
	{
		pHostEntry = gethostbyaddr((const char *)&inAddr, sizeof(inAddr), AF_INET);
	}
	else
	{
		pHostEntry = gethostbyname(m_szQuery.toUtf8().data());
	}

	if(!pHostEntry)
	{
		switch(h_errno)
		{
			case HOST_NOT_FOUND:
				dns->setError(KviError::HostNotFound);
				break;
			case NO_ADDRESS:
				dns->setError(KviError::ValidNameButNoIpAddress);
				break;
			case NO_RECOVERY:
				dns->setError(KviError::UnrecoverableNameserverError);
				break;
			case TRY_AGAIN:
				dns->setError(KviError::DNSTemporaneousFault);
				break;
			default:
				dns->setError(KviError::DNSQueryFailed);
				break;
		}
	}
	else
	{
		dns->appendHostname(pHostEntry->h_name);
		QString szIp;
		KviNetUtils::binaryIpToStringIp(*((struct in_addr *)(pHostEntry->h_addr)), szIp);
		dns->appendAddress(szIp);

		int idx = 1;
		while(pHostEntry->h_addr_list[idx])
		{
			QString tmp;
			KviNetUtils::binaryIpToStringIp(*((struct in_addr *)(pHostEntry->h_addr_list[idx])), tmp);
			if(!tmp.isEmpty())
				dns->appendAddress(tmp);
			++idx;
		}
		if(pHostEntry->h_aliases[0])
		{
			dns->appendHostname(QString::fromUtf8(pHostEntry->h_aliases[0]));
			if(pHostEntry->h_aliases[1])
				dns->appendHostname(QString::fromUtf8(pHostEntry->h_aliases[1]));
		}
	}

#else

	int retVal;

	struct sockaddr_in ipv4Addr;

#ifdef COMPILE_IPV6_SUPPORT
	struct sockaddr_in6 ipv6Addr;
	bool bIsIPv6Ip = false;
#endif

	bool bIsIPv4Ip = KviNetUtils::stringIpToBinaryIp(m_szQuery, (struct in_addr *)&(ipv4Addr.sin_addr));

#ifdef COMPILE_IPV6_SUPPORT
	if(!bIsIPv4Ip)
		bIsIPv6Ip = KviNetUtils::stringIpToBinaryIp_V6(m_szQuery, (struct in6_addr *)&(ipv6Addr.sin6_addr));
#endif


#ifdef COMPILE_IPV6_SUPPORT
	if(bIsIPv4Ip || bIsIPv6Ip)
	{
#else
	if(bIsIPv4Ip)
	{
#endif
		// use getnameinfo...
		char retname[1025]; // should be enough....

#ifdef COMPILE_IPV6_SUPPORT
		if(bIsIPv4Ip)
		{
#endif
			ipv4Addr.sin_family = AF_INET;
			ipv4Addr.sin_port = 0;
			// NI_NAMEREQD as last param ?
			retVal = getnameinfo((struct sockaddr *)&ipv4Addr, sizeof(ipv4Addr), retname, 1025, nullptr, 0, NI_NAMEREQD);
#ifdef COMPILE_IPV6_SUPPORT
		}
		else
		{
			ipv6Addr.sin6_family = AF_INET6;
			ipv6Addr.sin6_port = 0;
			retVal = getnameinfo((struct sockaddr *)&ipv6Addr, sizeof(ipv6Addr), retname, 1025, nullptr, 0, NI_NAMEREQD);
		}
#endif

		if(retVal != 0)
			dns->setError(translateDnsError(retVal));
		else
		{
			dns->appendHostname(retname);
			dns->appendAddress(m_szQuery);
		}
	}
	else
	{
		struct addrinfo * pRet = nullptr;
		struct addrinfo * pNext;
		struct addrinfo hints;
		hints.ai_flags = 0; //AI_CANONNAME; <-- for IPV6 it makes cannoname to point to the IP address!
#ifdef COMPILE_IPV6_SUPPORT
		hints.ai_family = (m_queryType == KviDnsResolver::IPv6) ? PF_INET6 : ((m_queryType == KviDnsResolver::IPv4) ? PF_INET : PF_UNSPEC);
#else
		hints.ai_family = PF_INET;
#endif
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = 0;
		hints.ai_addrlen = 0;
		hints.ai_canonname = nullptr;
		hints.ai_addr = nullptr;
		hints.ai_next = nullptr;

		retVal = getaddrinfo(m_szQuery.toUtf8().data(), nullptr, &hints, &pRet);

		if(retVal != 0)
		{
			dns->setError(translateDnsError(retVal));
		}
		else if(!pRet)
		{
			dns->setError(KviError::DNSQueryFailed);
		}
		else
		{
			dns->appendHostname(pRet->ai_canonname ? QString::fromUtf8(pRet->ai_canonname) : m_szQuery);
			QString szIp;
#ifdef COMPILE_IPV6_SUPPORT
			if(pRet->ai_family == PF_INET6)
				KviNetUtils::binaryIpToStringIp_V6(((sockaddr_in6 *)(pRet->ai_addr))->sin6_addr, szIp);
			else
			{
#endif
				KviNetUtils::binaryIpToStringIp(((sockaddr_in *)(pRet->ai_addr))->sin_addr, szIp);
#ifdef COMPILE_IPV6_SUPPORT
			}
#endif
			dns->appendAddress(szIp);

			pNext = pRet->ai_next;
			while(pNext)
			{
				QString tmp;
#ifdef COMPILE_IPV6_SUPPORT
				if(pNext->ai_family == PF_INET6)
					KviNetUtils::binaryIpToStringIp_V6(((sockaddr_in6 *)(pNext->ai_addr))->sin6_addr, tmp);
				else
				{
#endif
					KviNetUtils::binaryIpToStringIp(((sockaddr_in *)(pNext->ai_addr))->sin_addr, tmp);
#ifdef COMPILE_IPV6_SUPPORT
				}
#endif
				if(!tmp.isEmpty())
					dns->appendAddress(tmp);

				if(pNext->ai_canonname)
				{
					// FIXME: only of not equal to other names ?
					dns->appendHostname(QString::fromUtf8(pNext->ai_canonname));
				}

				pNext = pNext->ai_next;
			}
		}
		if(pRet)
			freeaddrinfo(pRet);
	}

#endif // !COMPILE_ON_WINDOWS

	KviDnsResolverThreadEvent * pEvent = new KviDnsResolverThreadEvent(dns);
	QApplication::postEvent(m_pParentDns, pEvent);
}

KviDnsResolver::KviDnsResolver()
    : QObject()
{
	m_pSlaveThread = new KviDnsResolverThread(this);
	m_pDnsResult = new KviDnsResolverResult();
	m_state = Idle;
}

KviDnsResolver::~KviDnsResolver()
{
	if(m_pSlaveThread)
	{
		if(!m_pSlaveThread->wait(30000))
			qDebug("Failed to wait for the slave DNS thread: we're probably going to crash!");
		delete m_pSlaveThread; // will eventually terminate it (but it will also block us!!!)
	}

	delete m_pDnsResult;
}

bool KviDnsResolver::isRunning() const
{
	return (m_state == Busy);
}

bool KviDnsResolver::lookup(const QString & szQuery, QueryType type)
{
	if(m_state == Busy)
		return false;
	m_pSlaveThread->setQuery(szQuery.trimmed(), type);
	m_pSlaveThread->start();
	m_state = Busy;
	return true;
}

KviError::Code KviDnsResolver::error()
{
	if(!m_pDnsResult)
		return KviError::DNSQueryFailed;
	return m_pDnsResult->error();
}

QString KviDnsResolver::errorString()
{
	return KviError::getDescription(error());
}

KviDnsResolverResult * KviDnsResolver::result()
{
	if(!m_pDnsResult)
		m_pDnsResult = new KviDnsResolverResult();
	return m_pDnsResult;
}

const std::vector<QString> & KviDnsResolver::hostnameList()
{
	return result()->hostnameList();
}

const std::vector<QString> & KviDnsResolver::ipAddressList()
{
	return result()->ipAddressList();
}

std::size_t KviDnsResolver::hostnameCount()
{
	return result()->hostnameList().size();
}

std::size_t KviDnsResolver::ipAddressCount()
{
	return result()->ipAddressList().size();
}

const QString & KviDnsResolver::hostName()
{
	const auto & list = result()->hostnameList();
	return list.empty() ? KviQString::Empty : list.front();
}

const QString & KviDnsResolver::firstIpAddress()
{
	const auto & list = result()->ipAddressList();
	return list.empty() ? KviQString::Empty : list.front();
}

const QString & KviDnsResolver::query()
{
	return result()->query();
}

bool KviDnsResolver::event(QEvent * e)
{
	if(e->type() == QEvent::User)
	{
		KviDnsResolverThreadEvent * pEvent = dynamic_cast<KviDnsResolverThreadEvent *>(e);
		if(pEvent)
		{
			delete m_pDnsResult;
			m_pDnsResult = pEvent->releaseResult();
			m_state = (m_pDnsResult->error() == KviError::Success) ? Success : Failure;
			emit lookupDone(this);
			return true;
		} // else oops... unknown thread event ?
	}
	return QObject::event(e);
}
