#ifndef _KVI_NETUTILS_H_
#define _KVI_NETUTILS_H_

//
//   File : kvi_netutlis.h
//   Creation date : Sun Jun 18 2000 18:37:27 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

#include "kvi_settings.h"
#include "kvi_inttypes.h"

#ifdef COMPILE_ON_WINDOWS
	#include <winsock2.h>
	#ifdef COMPILE_IPV6_SUPPORT
		#ifdef WIN2K
			#include <ws2ip6.h>
		#else
			#include <ws2tcpip.h>
			//#include <tpipv6.h>
			#define in6_addr in_addr6
		#endif
	#endif
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h> //in_addr
	#include <arpa/inet.h>  //inet_ntoa inet_ntop and inet_pton depend on this one.
#endif

#include "kvi_string.h"


KVILIB_API extern bool kvi_isValidStringIp(const char * szIp);
KVILIB_API extern bool kvi_stringIpToBinaryIp(const char * szIp,struct in_addr * address);
KVILIB_API extern bool kvi_binaryIpToStringIp(struct in_addr in,QString &szBuffer);

#ifdef COMPILE_IPV6_SUPPORT
	KVILIB_API extern bool kvi_isValidStringIp_V6(const char * szIp);
	KVILIB_API extern bool kvi_stringIpToBinaryIp_V6(const char * szIp,struct in6_addr * address);
	KVILIB_API extern bool kvi_binaryIpToStringIp_V6(struct in6_addr in,QString &szBuffer);
#endif

class KVILIB_API KviSockaddr
{
public:
	KviSockaddr(const char * szIpAddress,kvi_u32_t uPort,bool bIpV6);
	KviSockaddr(kvi_u32_t uPort,bool bIpV6); // passive
	~KviSockaddr();
private:
	void * m_pData; //addrinfo
public:
	struct sockaddr * socketAddress();
	size_t addressLength();
	int addressFamily();
	bool isIpV6();
	bool getStringAddress(QString &szBuffer);
	kvi_u32_t port();
	
};


KVILIB_API extern bool kvi_select(int fd,bool * bCanRead,bool * bCanWrite,int iUSecs = 0);
KVILIB_API extern bool kvi_getInterfaceAddress(const char * ifname,QString &buffer);

// Warning : NOT THREAD SAFE!
KVILIB_API extern bool kvi_getLocalHostAddress(QString &buffer);
KVILIB_API extern bool kvi_isRoutableIp(const char * ipaddr);
KVILIB_API extern bool kvi_isRoutableIpString(const char * ipstring);

namespace KviNetUtils
{
	KVILIB_API bool stringIpToBinaryIp(const QString &szStringIp,struct in_addr * address);
	KVILIB_API bool isValidStringIp(const QString &szStringIp);
	KVILIB_API bool binaryIpToStringIp(struct in_addr in,QString &szBuffer);
	KVILIB_API bool getInterfaceAddress(const QString &szInterfaceName,QString &szBuffer);
#ifdef COMPILE_IPV6_SUPPORT
	KVILIB_API bool isValidStringIp_V6(const QString &szStringIp);
	KVILIB_API bool stringIpToBinaryIp_V6(const QString &szStringIp,struct in6_addr * address);
	KVILIB_API bool binaryIpToStringIp_V6(struct in6_addr in,QString &szBuffer);
#endif
	KVILIB_API bool isRoutableIp(const char * ipaddr);
	KVILIB_API bool isRoutableIpString(const QString &szIpString);
	KVILIB_API void formatNetworkBandwidthString(QString &szBuffer,unsigned int uBytesPerSec);
};


#endif //!_KVI_NETUTILS_H_
