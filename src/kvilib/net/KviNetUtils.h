#ifndef _KVI_NETUTILS_H_
#define _KVI_NETUTILS_H_
//=============================================================================
//
//   File : KviNetUtils.h
//   Creation date : Sun Jun 18 2000 18:37:27 by Szymon Stefanek
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

#include "kvi_settings.h"
#include "kvi_inttypes.h"

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#include <winsock2.h>
#ifdef COMPILE_IPV6_SUPPORT
#ifdef WIN2K
#include <ws2ip6.h>
#else
#include <ws2tcpip.h>
//#include <tpipv6.h>
#define in6_addr in_addr6
#endif
#else
#ifdef WIN2K
#include <wspiapi.h>
#else
#include <ws2tcpip.h>
#endif
#endif
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> //in_addr
#include <arpa/inet.h>  //inet_ntoa inet_ntop and inet_pton depend on this one.
#endif

class QString;

class KVILIB_API KviSockaddr
{
public:
	KviSockaddr(const char * szIpAddress, kvi_u32_t uPort, bool bIPv6, bool bUdp = false);
	KviSockaddr(kvi_u32_t uPort, bool bIPv6, bool bUdp = false); // passive
	~KviSockaddr();

private:
	struct addrinfo * m_pData;

public:
	struct sockaddr * socketAddress();
	size_t addressLength();
	int addressFamily();
	bool isIPv6();
	bool getStringAddress(QString & szBuffer);
	kvi_u32_t port();
};

KVILIB_API extern bool kvi_select(int fd, bool * bCanRead, bool * bCanWrite, int iUSecs = 0);

// Warning : NOT THREAD SAFE!
KVILIB_API extern bool kvi_getLocalHostAddress(QString & buffer);
KVILIB_API extern bool kvi_isRoutableIp(const char * ipaddr);
KVILIB_API extern bool kvi_isRoutableIpString(const char * ipstring);

namespace KviNetUtils
{
	KVILIB_API bool stringIpToBinaryIp(const QString & szStringIp, struct in_addr * address);
	KVILIB_API bool isValidStringIp(const QString & szStringIp);
	KVILIB_API bool binaryIpToStringIp(struct in_addr in, QString & szBuffer);
	KVILIB_API bool getInterfaceAddress(const QString & szInterfaceName, QString & szBuffer);
#ifdef COMPILE_IPV6_SUPPORT
	KVILIB_API bool isValidStringIPv6(const QString & szStringIp);
	KVILIB_API bool stringIpToBinaryIp_V6(const QString & szStringIp, struct in6_addr * address);
	KVILIB_API bool binaryIpToStringIp_V6(struct in6_addr in, QString & szBuffer);
#endif
	KVILIB_API bool isRoutableIp(const char * ipaddr);
	KVILIB_API bool isRoutableIpString(const QString & szIpString);
	KVILIB_API void formatNetworkBandwidthString(QString & szBuffer, unsigned int uBytesPerSec);
}

#endif //!_KVI_NETUTILS_H_
