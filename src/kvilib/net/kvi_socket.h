#ifndef _KVI_SOCKET_H_
#define _KVI_SOCKET_H_
//=============================================================================
//
//   File : kvi_socket.h
//   Creation date : Thu Sep 20 03:50:22 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

//
// Socket stuff abstraction layer
//

#include "kvi_settings.h"
#include "kvi_sockettype.h" // <--- this includes <winsock2.h> if needed

#include <errno.h>

#include "kvi_inttypes.h"

//#ifndef _KVI_SOCKET_CPP_
extern KVILIB_API kvi_u64_t g_uOutgoingTraffic;
extern KVILIB_API kvi_u64_t g_uIncomingTraffic;
//#endif //!_KVI_SOCKET_CPP_

#if defined(COMPILE_ON_WINDOWS) || (defined(COMPILE_ON_MINGW) && !defined(OS2))

#define KVI_INVALID_SOCKET INVALID_SOCKET

//every decent win version should contain IPPROTO_IPV6

//mingw win32 headers do not have this, mingw-w64-headers do however
#if defined(COMPILE_ON_MINGW)
#ifndef IPV6_PROTECTION_LEVEL
#define IPV6_PROTECTION_LEVEL 23
#define PROTECTION_LEVEL_UNRESTRICTED 10 /* for peer-to-peer apps  */
#define PROTECTION_LEVEL_DEFAULT 20      /* default level          */
#define PROTECTION_LEVEL_RESTRICTED 30   /* for Intranet apps      */
#endif
#endif

#define KVI_IPV6_PROTECTION_LEVEL IPV6_PROTECTION_LEVEL
#define KVI_PROTECTION_LEVEL_RESTRICTED PROTECTION_LEVEL_RESTRICTED
#define KVI_PROTECTION_LEVEL_DEFAULT PROTECTION_LEVEL_DEFAULT
#define KVI_PROTECTION_LEVEL_UNRESTRICTED PROTECTION_LEVEL_UNRESTRICTED
#define KVI_IPPROTO_IPV6 IPPROTO_IPV6

#else //!(defined(COMPILE_ON_WINDOWS) || (defined(COMPILE_ON_MINGW) && !defined(OS2))

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

#define KVI_INVALID_SOCKET (-1)
#endif //!(defined(COMPILE_ON_WINDOWS) || (defined(COMPILE_ON_MINGW) && !defined(OS2))

#ifndef MSG_NOSIGNAL
// At least solaris seems to not have it
#define MSG_NOSIGNAL 0
#endif //!MSG_NOSIGNAL

//
// Constants for kvi_socket_create
//

#define KVI_SOCKET_PF_INET PF_INET
#define KVI_SOCKET_PF_INET6 PF_INET6
#define KVI_SOCKET_PF_UNIX PF_UNIX

#define KVI_SOCKET_TYPE_STREAM SOCK_STREAM
#define KVI_SOCKET_TYPE_DGRAM SOCK_DGRAM

#define KVI_SOCKET_PROTO_TCP 0

//
// kvi_socket_create
// kvi_socket_open
//
//   Open a socket of the specified protocol family, type and protocol
//   You should always use the KVI_SOCKET_* constants as parameters
//   Returns KVI_INVALID_SOCKET if the socket creation has failed.
//   The returned socket is in blocking mode!
//

#define kvi_socket_open kvi_socket_create

inline kvi_socket_t kvi_socket_create(int pf, int type, int proto)
{
#ifdef COMPILE_ON_MAC
	/*
	 * Ignore SIGPIPE. Under linux we set MSG_NOSIGNAL on send and recv calls,
	 * but under OSX (and probably other *BSD) we need to set SO_NOSIGPIPE
	 */
	kvi_socket_t fd = (kvi_socket_t)socket(pf, type, proto);
	int set = 1;
	setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
	return fd;
#else
	return (kvi_socket_t)socket(pf, type, proto);
#endif
}

//
// kvi_socket_isValid
//
//   Check if a socket is valid or not
//

inline void kvi_socket_flushTrafficCounters()
{
	g_uOutgoingTraffic = 0;
	g_uIncomingTraffic = 0;
}

inline bool kvi_socket_isValid(kvi_socket_t sock)
{
	return (sock != ((kvi_socket_t)(KVI_INVALID_SOCKET)));
}

//
// kvi_socket_destroy
// kvi_socket_close
//
//   Close a socket...that's all :)
//

#define kvi_socket_close kvi_socket_destroy

inline void kvi_socket_destroy(kvi_socket_t sock)
{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	closesocket(sock);
#else
	close(sock);
#endif
}

//
// kvi_socket_setNonBlocking
//
//   Sets the socket in nonBlocking mode. Obviously returns false in case of failure
//

inline bool kvi_socket_setNonBlocking(kvi_socket_t sock)
{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	unsigned long arg = 1;
	return (ioctlsocket(sock, FIONBIO, (unsigned long FAR *)&arg) == 0);
#else
	return (fcntl(sock, F_SETFL, O_NONBLOCK) == 0);
#endif
}

//
// kvi_socket_bind
//
//   Standard bind() call on the socket. Returns false in case of failure
//

inline bool kvi_socket_bind(kvi_socket_t sock, const struct sockaddr * sa, int salen)
{
	return (::bind(sock, sa, salen) == 0);
}

//
// kvi_socket_connect
//
//   Starts a connection to the specified remote address
//   returns false if the connection can not be started.
//   You might take a look at kvi_socket_errno() then.
//

inline bool kvi_socket_connect(kvi_socket_t sock, const struct sockaddr * sa, int salen)
{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	return (WSAConnect(sock, sa, salen, 0, 0, 0, 0) == 0);
#else
	return (::connect(sock, sa, salen) == 0);
#endif
}

inline bool kvi_socket_recoverableConnectError(int err)
{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	return ((err == WSAEINPROGRESS) || (err == WSAEWOULDBLOCK));
#else
	return (err == EINPROGRESS);
#endif
}

inline bool kvi_socket_recoverableError(int err)
{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	return ((err == WSAEWOULDBLOCK) || (err == EINTR) || (err == EAGAIN));
#else
	return ((err == EINTR) || (err == EAGAIN));
#endif
}

//
// kvi_socket_accept
//
//   Standard accept() call. Returns KVI_INVALID_SOCKET in case of failure
//   You should check kvi_socket_errno() then.
//

inline kvi_socket_t kvi_socket_accept(kvi_socket_t sock, struct sockaddr * sa, int * salen)
{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	return (kvi_socket_t)::accept(sock, sa, salen);
#else
	return (kvi_socket_t)::accept(sock, sa, (socklen_t *)salen);
#endif
}

//
// kvi_socket_listen
//
//   Standard listen() call. Returns false in case of failure
//   You should check kvi_socket_errno() then.
//

inline bool kvi_socket_listen(kvi_socket_t sock, int backlog)
{
	return (::listen(sock, backlog) == 0);
}

//
// kvi_socket_select
//
//   Standard select() call. This is complex so here is a mini-reminder:
//   nhpo is the number of the highest file descriptor in the sets plus one!
//   Returns the number of sockets with data available (or space available)
//   or something that is less than 0 in case of error. You should check kvi_socket_errno() then.
//

inline int kvi_socket_select(int nhpo, fd_set * r, fd_set * w, fd_set * e, struct timeval * t)
{
	return ::select(nhpo, r, w, e, t);
}

//
// kvi_socket_send
// kvi_socket_write
//
//   Standard send() call. On UNIX ignores SIGPIPE. Returns the number of bytes sent or
//   -1 in case of failure. You should check kvi_socket_errno() then.
//

#define kvi_socket_write kvi_socket_send

inline int kvi_socket_send(kvi_socket_t sock, const void * buf, int size)
{
	g_uOutgoingTraffic += size;
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	return ::send(sock, (const char *)buf, size, 0);
#else
	return ::send(sock, buf, size, MSG_NOSIGNAL | MSG_DONTWAIT);
#endif
}

//
// kvi_socket_recv
// kvi_socket_read
//
//   Standard read() call. On UNIX ignores SIGPIPE. Returns the number of bytes readed or
//   -1 in case of failure. You should check kvi_socket_errno() then.
//

#define kvi_socket_read kvi_socket_recv

inline int kvi_socket_recv(kvi_socket_t sock, void * buf, int maxlen)
{
	int iReceived;
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	iReceived = ::recv(sock, (char *)buf, maxlen, 0);
#else
	iReceived = ::recv(sock, buf, maxlen, MSG_NOSIGNAL);
#endif
	g_uIncomingTraffic += iReceived;
	return iReceived;
}

//
// kvi_socket_getsockopt
//
//   Standard getsockopt() call. Returns false in case of failure.
//   You should check kvi_socket_errno() then.
//

inline bool kvi_socket_getsockopt(kvi_socket_t sock, int level, int optname, void * optval, int * optlen)
{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	return (::getsockopt(sock, level, optname, (char FAR *)optval, optlen) == 0);
#else
	return (::getsockopt(sock, level, optname, optval, (socklen_t *)optlen) == 0);
#endif
}

//
// kvi_socket_setsockopt
//
//   Standard setsockopt() call. Returns false in case of failure.
//   You should check kvi_socket_errno() then.
//

inline bool kvi_socket_setsockopt(kvi_socket_t sock, int level, int optname, const void * optval, int optlen)
{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	return (::setsockopt(sock, level, optname, (char FAR *)optval, optlen) == 0);
#else
	return (::setsockopt(sock, level, optname, optval, optlen) == 0);
#endif
}

//
// kvi_socket_disableNagle
//
//   Disables the nagle algorithm (sets TCP_NODELAY)
//

/*
	unused for now
inline bool kvi_socket_disableNagle(kvi_socket_t sock)
{
	int opt = 1;
	return kvi_socket_setsockopt(sock,IPPROTO_TCP,TCP_NODELAY,&opt,sizeof(opt));
}
*/

//
// kvi_socket_getsockname
//
//   Standard getsockname() call. Returns false in case of failure.
//   You should check kvi_socket_errno() then.
//

inline bool kvi_socket_getsockname(kvi_socket_t sock, struct sockaddr * addr, int * addrlen)
{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	return (::getsockname(sock, addr, addrlen) == 0);
#else
	return (::getsockname(sock, addr, (socklen_t *)addrlen) == 0);
#endif
}

inline int kvi_socket_error()
{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	return WSAGetLastError();
#else
	return errno;
#endif
}

#endif //_KVI_SOCKET_H_
