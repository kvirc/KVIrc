//=============================================================================
//
//   File : kvi_dns.cpp
//   Creation date : Sat Jul 21 2000 17:19:31 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_dns.h"
#include "kvi_error.h"
#include "kvi_netutils.h"

#include <errno.h>

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



KviDnsResult::KviDnsResult()
{
	m_iError = KviError_success;
	m_pHostnameList = new KviPointerList<QString>;
	m_pHostnameList->setAutoDelete(true);
	m_pIpAddressList = new KviPointerList<QString>;
	m_pIpAddressList->setAutoDelete(true);

}

KviDnsResult::~KviDnsResult()
{
	delete m_pHostnameList;
	delete m_pIpAddressList;
}

void KviDnsResult::appendHostname(const QString &host)
{
	m_pHostnameList->append(new QString(host));
}


void KviDnsResult::appendAddress(const QString &addr)
{
	m_pIpAddressList->append(new QString(addr));
}



KviDnsThread::KviDnsThread(KviDns * pDns)
{
	m_pParentDns = pDns;
}

KviDnsThread::~KviDnsThread()
{
}

int KviDnsThread::translateDnsError(int iErr)
{
#if defined(COMPILE_IPV6_SUPPORT) || (!defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW))

	switch(iErr)
	{
		case EAI_FAMILY:     return KviError_unsupportedAddressFamily; break;
#if (!defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)) && defined(EAI_ADDRFAMILY) && (EAI_ADDRFAMILY != EAI_FAMILY)
		case EAI_ADDRFAMILY: return KviError_unsupportedAddressFamily; break;
#endif
// NOT FreeBSD ARE WE?
#if defined(EAI_NODATA) && (EAI_NODATA != EAI_NONAME)
// YARR
		case EAI_NODATA:     return KviError_validNameButNoIpAddress; break;
#endif
		case EAI_FAIL:       return KviError_unrecoverableNameserverError; break;
		case EAI_AGAIN:      return KviError_dnsTemporaneousFault; break;
		// this should never happen
		case EAI_BADFLAGS:   return KviError_dnsInternalErrorBadFlags; break;
		case EAI_MEMORY:     return KviError_dnsInternalErrorOutOfMemory; break;
		// got this when experimenting with protocols
		case EAI_SERVICE:    return KviError_dnsInternalErrorServiceNotSupported; break;
#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
		case EAI_NONAME:     return KviError_dnsNoName; break;
#endif
		// got this when experimenting with protocols
		case EAI_SOCKTYPE:   return KviError_dnsInternalErrorUnsupportedSocketType; break;
#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
		case EAI_SYSTEM:     return -errno;
#endif
	}

#endif
	return KviError_dnsQueryFailed;
}

void KviDnsThread::postDnsError(KviDnsResult * dns,int iErr)
{
	dns->setError(iErr);
	KviThreadDataEvent<KviDnsResult> * e = new KviThreadDataEvent<KviDnsResult>(KVI_DNS_THREAD_EVENT_DATA);
	e->setData(dns);
	postEvent(m_pParentDns,e);
}

void KviDnsThread::run()
{
	KviDnsResult * dns = new KviDnsResult();

	dns->setQuery(m_szQuery);

	if(m_szQuery.isEmpty())
	{
		postDnsError(dns,KviError_noHostToResolve);
		return;
	}

#ifndef COMPILE_IPV6_SUPPORT
	if(m_queryType != KviDns::IPv4)
	{
		if(m_queryType == KviDns::IPv6)
		{
			postDnsError(dns,KviError_noIPv6Support);
			return;
		}
		m_queryType = KviDns::IPv4;
	}
#endif

#if (defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)) && !defined(COMPILE_IPV6_SUPPORT)

	if(m_queryType == KviDns::IPv6)
	{
		postDnsError(dns,KviError_noIPv6Support);
		return;
	}

	// gethostbyaddr and gethostbyname are thread-safe on Windoze
	struct in_addr inAddr;
	struct hostent *pHostEntry = 0;


	// DIE DIE!....I hope that this stuff will disappear sooner or later :)

	if(KviNetUtils::stringIpToBinaryIp(m_szQuery,&inAddr))
	{
		pHostEntry = gethostbyaddr((const char *)&inAddr,sizeof(inAddr),AF_INET);
	} else {
		pHostEntry = gethostbyname(m_szQuery.toUtf8().data());
	}

	if(!pHostEntry)
	{
		switch(h_errno)
		{
			case HOST_NOT_FOUND: dns->setError(KviError_hostNotFound); break;
			case NO_ADDRESS:     dns->setError(KviError_validNameButNoIpAddress); break;
			case NO_RECOVERY:    dns->setError(KviError_unrecoverableNameserverError); break;
			case TRY_AGAIN:      dns->setError(KviError_dnsTemporaneousFault); break;
			default:             dns->setError(KviError_dnsQueryFailed); break;
		}
	} else {
		dns->appendHostname(pHostEntry->h_name);
		QString szIp;
		KviNetUtils::binaryIpToStringIp(* ((struct in_addr*)(pHostEntry->h_addr)),szIp);
		dns->appendAddress(szIp);

		int idx = 1;
		while(pHostEntry->h_addr_list[idx])
		{
			QString tmp;
			KviNetUtils::binaryIpToStringIp(* ((struct in_addr*)(pHostEntry->h_addr_list[idx])),tmp);
			if(!tmp.isEmpty())dns->appendAddress(tmp);
			++idx;
		}
		if(pHostEntry->h_aliases[0])
		{
			dns->appendHostname(QString::fromUtf8(pHostEntry->h_aliases[0]));
			if(pHostEntry->h_aliases[1])dns->appendHostname(QString::fromUtf8(pHostEntry->h_aliases[1]));
		}
	}


#else

	int retVal;


//#ifdef HAVE_GETNAMEINFO
	struct sockaddr_in ipv4Addr;

#ifdef COMPILE_IPV6_SUPPORT
	struct sockaddr_in6 ipv6Addr;
	bool bIsIPv6Ip = false;
#endif

	bool bIsIPv4Ip = KviNetUtils::stringIpToBinaryIp(m_szQuery,(struct in_addr *)&(ipv4Addr.sin_addr));

#ifdef COMPILE_IPV6_SUPPORT
	if(!bIsIPv4Ip)bIsIPv6Ip = KviNetUtils::stringIpToBinaryIp_V6(m_szQuery,(struct in6_addr *)&(ipv6Addr.sin6_addr));
#endif

//#ifdef HAVE_GETNAMEINFO

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
			retVal = getnameinfo((struct sockaddr *)&ipv4Addr,sizeof(ipv4Addr),retname,1025,0,0,NI_NAMEREQD);
#ifdef COMPILE_IPV6_SUPPORT
		} else {
			ipv6Addr.sin6_family = AF_INET6;
			ipv6Addr.sin6_port = 0;
			retVal = getnameinfo((struct sockaddr *)&ipv6Addr,sizeof(ipv6Addr),retname,1025,0,0,NI_NAMEREQD);
		}
#endif

		if(retVal != 0)dns->setError(translateDnsError(retVal));
		else {
			dns->appendHostname(retname);
			dns->appendAddress(m_szQuery);
		}

	} else {
//#endif //HAVE_GETNAMEINFO


//#ifdef COMPILE_IPV6_SUPPORT
//		struct in6_addr in6Addr;
//#endif
		struct addrinfo * pRet = 0;
		struct addrinfo * pNext;
		struct addrinfo hints;
		hints.ai_flags     = 0; //AI_CANONNAME; <-- for IPV6 it makes cannoname to point to the IP address!
#ifdef COMPILE_IPV6_SUPPORT
		hints.ai_family    = (m_queryType == KviDns::IPv6) ? PF_INET6 : ((m_queryType == KviDns::IPv4) ? PF_INET : PF_UNSPEC);
#else
		hints.ai_family    = PF_INET;
#endif
		hints.ai_socktype  = SOCK_STREAM;
		hints.ai_protocol  = 0;
		hints.ai_addrlen   = 0;
		hints.ai_canonname = 0;
		hints.ai_addr      = 0;
		hints.ai_next      = 0;

		retVal = getaddrinfo(KviQString::toUtf8(m_szQuery).data(),0,&hints,&pRet);

		if(retVal != 0)dns->setError(translateDnsError(retVal));
		else {
			dns->appendHostname(pRet->ai_canonname ? QString::fromUtf8(pRet->ai_canonname) : m_szQuery);
			QString szIp;
#ifdef COMPILE_IPV6_SUPPORT
			if(pRet->ai_family == PF_INET6)KviNetUtils::binaryIpToStringIp_V6(((sockaddr_in6 *)(pRet->ai_addr))->sin6_addr,szIp);
			else {
#endif
				KviNetUtils::binaryIpToStringIp(((sockaddr_in *)(pRet->ai_addr))->sin_addr,szIp);
#ifdef COMPILE_IPV6_SUPPORT
			}
#endif
			dns->appendAddress(szIp);

			pNext = pRet->ai_next;
			while(pNext)
			{
				QString tmp;
#ifdef COMPILE_IPV6_SUPPORT
				if(pNext->ai_family == PF_INET6)KviNetUtils::binaryIpToStringIp_V6(((sockaddr_in6 *)(pNext->ai_addr))->sin6_addr,tmp);
				else {
#endif
					KviNetUtils::binaryIpToStringIp(((sockaddr_in *)(pNext->ai_addr))->sin_addr,tmp);
#ifdef COMPILE_IPV6_SUPPORT
				}
#endif
				if(!tmp.isEmpty())dns->appendAddress(tmp);

				if(pNext->ai_canonname)
				{
					// FIXME: only of not equal to other names ?
					dns->appendHostname(QString::fromUtf8(pNext->ai_canonname));
				}

				pNext = pNext->ai_next;

			}
		}
		if(pRet)freeaddrinfo(pRet);
//#ifdef HAVE_GETNAMEINFO
	}
//#endif //HAVE_GETNAMEINFO

#endif // !COMPILE_ON_WINDOWS


	KviThreadDataEvent<KviDnsResult> * e = new KviThreadDataEvent<KviDnsResult>(KVI_DNS_THREAD_EVENT_DATA);
	e->setData(dns);
	postEvent(m_pParentDns,e);
}


KviDns::KviDns()
: QObject()
{
	m_pSlaveThread = new KviDnsThread(this);
	m_pDnsResult = new KviDnsResult();
	m_pAuxData = 0;
	m_state = Idle;
}

KviDns::~KviDns()
{
	if(m_pSlaveThread)delete m_pSlaveThread; // will eventually terminate it (but it will also block us!!!)
	KviThreadManager::killPendingEvents(this);
	if(m_pDnsResult)delete m_pDnsResult;
	if(m_pAuxData)debug("You're leaking memory man! m_pAuxData is non 0!");
}

bool KviDns::isRunning() const
{
	return (m_state == Busy);
}

bool KviDns::lookup(const QString &query,QueryType type)
{
	if(m_state == Busy)return false;
	m_pSlaveThread->setQuery(KviQString::trimmed(query),type);
	bool bStarted = m_pSlaveThread->start();
	m_state = bStarted ? Busy : Failure;
	return bStarted;
}

int KviDns::error()
{
	if(!m_pDnsResult)return KviError_dnsQueryFailed;
	return m_pDnsResult->error();
}

KviDnsResult * KviDns::result()
{
	if(!m_pDnsResult)m_pDnsResult = new KviDnsResult();
	return m_pDnsResult;
}

KviPointerList<QString> * KviDns::hostnameList()
{
	return result()->hostnameList();
}

KviPointerList<QString> * KviDns::ipAddressList()
{
	return result()->ipAddressList();
}

int KviDns::hostnameCount()
{
	return result()->hostnameList()->count();
}

int KviDns::ipAddressCount()
{
	return result()->ipAddressList()->count();
}

const QString & KviDns::firstHostname()
{
	QString * pStr = result()->hostnameList()->first();
	if(pStr)return *pStr;
	return KviQString::Empty;
}

const QString & KviDns::firstIpAddress()
{
	QString * pStr = result()->ipAddressList()->first();
	if(pStr)return *pStr;
	return KviQString::Empty;
}

const QString & KviDns::query()
{
	return result()->query();
}

bool KviDns::event(QEvent *e)
{
	if(e->type() == KVI_THREAD_EVENT)
	{
		if(((KviThreadEvent *)e)->id() == KVI_DNS_THREAD_EVENT_DATA)
		{
			if(m_pDnsResult)delete m_pDnsResult;
			m_pDnsResult = ((KviThreadDataEvent<KviDnsResult> *)e)->getData();
			m_state = (m_pDnsResult->error() == KviError_success) ? Success : Failure;
			emit lookupDone(this);
			return true;
		} // else ops... unknown thread event ?
	}
	return QObject::event(e);
}
