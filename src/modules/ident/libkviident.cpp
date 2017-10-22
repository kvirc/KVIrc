//=============================================================================
//
//   File : libkviident.cpp
//   Creation date : Tue Oct  2 18:22:04 2001 GMT by Szymon Stefanek
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

#include "libkviident.h"

#include "KviModule.h"
#include "kvi_socket.h"
#include "KviApplication.h"
#include "kvi_out.h"
#include "KviNetUtils.h"
#include "KviLocale.h"
#include "KviWindow.h"
#include "KviOptions.h"
#include "kvi_defaults.h"

#define KVI_IDENT_THREAD_EVENT_EXITING KVI_THREAD_USER_EVENT_BASE + 111
#define KVI_IDENT_THREAD_EVENT_EXITING_ON_REQUEST KVI_THREAD_USER_EVENT_BASE + 112

// FIXME: Should have a timeout on the requests!!!

static KviIdentDaemon * g_pIdentDaemon = nullptr;
static KviIdentSentinel * g_pIdentSentinel = nullptr;

extern KVIRC_API int g_iIdentDaemonRunningUsers;

void startIdentService()
{
	if(!g_pIdentDaemon)
		g_pIdentDaemon = new KviIdentDaemon();
	if(!g_pIdentDaemon->isRunning())
		g_pIdentDaemon->start();
	while(g_pIdentDaemon->isStartingUp())
	{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		Sleep(10);
#else
		usleep(100);
#endif
	}
}

void stopIdentService()
{
	if(g_pIdentDaemon)
		delete g_pIdentDaemon;
	g_pIdentDaemon = nullptr;
}

KviIdentSentinel::KviIdentSentinel()
    : QObject(nullptr)
{
}

KviIdentSentinel::~KviIdentSentinel()
{
	KviThreadManager::killPendingEvents(this);
}

bool KviIdentSentinel::event(QEvent * e)
{
	if(KVI_OPTION_UINT(KviOption_uintIdentdOutputMode) == KviIdentdOutputMode::Quiet || !g_pActiveWindow)
		return QObject::event(e);

	KviWindow * pTarget = KVI_OPTION_UINT(KviOption_uintIdentdOutputMode) == KviIdentdOutputMode::ToActiveWindow ? (KviWindow *)g_pActiveWindow : (KviWindow *)g_pApp->activeConsole();

	if(e->type() == KVI_THREAD_EVENT)
	{
		if(((KviThreadEvent *)e)->id() == KVI_THREAD_EVENT_DATA)
		{
			KviIdentMessageData * d = ((KviThreadDataEvent<KviIdentMessageData> *)e)->getData();
			if(pTarget)
			{
				if(d->szHost.hasData())
				{
					if(d->szAux.hasData())
					{
						if(_OUTPUT_PARANOIC)
							pTarget->output(KVI_OUT_IDENT, __tr("%s (%s) (%s:%u)"), d->szMessage.ptr(), d->szAux.ptr(), d->szHost.ptr(), d->uPort);
						else
							pTarget->output(KVI_OUT_IDENT, __tr("%s (%s)"), d->szMessage.ptr(), d->szAux.ptr());
					}
					else
					{
						if(_OUTPUT_PARANOIC)
							pTarget->output(KVI_OUT_IDENT, __tr("%s (%s:%u)"), d->szMessage.ptr(), d->szHost.ptr(), d->uPort);
						else
							pTarget->output(KVI_OUT_IDENT, __tr("%s"), d->szMessage.ptr());
					}
				}
				else
				{
					pTarget->output(KVI_OUT_IDENT, __tr("[IDENT]: %s"), d->szMessage.ptr());
				}
			}
			delete d;
		}
		else if(((KviThreadEvent *)e)->id() == KVI_IDENT_THREAD_EVENT_EXITING)
		{
			if(_OUTPUT_VERBOSE)
				if(pTarget)
					pTarget->outputNoFmt(KVI_OUT_IDENT, __tr("Shutting down Ident service (spontaneous action)"));
			stopIdentService();
		}
		else if(((KviThreadEvent *)e)->id() == KVI_IDENT_THREAD_EVENT_EXITING_ON_REQUEST)
		{
			if(_OUTPUT_VERBOSE)
				if(pTarget)
					pTarget->outputNoFmt(KVI_OUT_IDENT, __tr("Shutting down Ident service (requested action)"));
		}

		return true;
	}

	return QObject::event(e);
}

KviIdentRequest::KviIdentRequest(kvi_socket_t sock, const char * host, kvi_u32_t uPort)
{
	m_sock = sock;
	m_szHost = host;
	m_uPort = uPort;
	m_tStart = time(nullptr);
}

KviIdentRequest::~KviIdentRequest()
{
	kvi_socket_close(m_sock);
}

KviIdentDaemon::KviIdentDaemon()
    : KviSensitiveThread()
{
	m_szUser = KVI_OPTION_STRING(KviOption_stringIdentdUser);
	if(m_szUser.isEmpty())
		m_szUser = "kvirc";
	m_uPort = KVI_OPTION_UINT(KviOption_uintIdentdPort);
#ifdef COMPILE_IPV6_SUPPORT
	m_bEnableIPv6 = KVI_OPTION_BOOL(KviOption_boolIdentdEnableIPv6);
#else
	m_bEnableIPv6 = false;
#endif
	m_bIPv6ContainsIPv4 = KVI_OPTION_BOOL(KviOption_boolIdentdIPv6ContainsIPv4);
}

KviIdentDaemon::~KviIdentDaemon()
{
	terminate();
	g_iIdentDaemonRunningUsers = 0;

	g_pIdentDaemon = nullptr;
}

void KviIdentDaemon::postMessage(const char * message, KviIdentRequest * r, const char * szAux)
{
	KviThreadDataEvent<KviIdentMessageData> * e = new KviThreadDataEvent<KviIdentMessageData>(KVI_THREAD_EVENT_DATA);

	KviIdentMessageData * d = new KviIdentMessageData;

	d->szMessage = message;
	if(szAux)
		d->szAux = szAux;

	if(r)
	{
		d->szHost = r->m_szHost;
		d->uPort = r->m_uPort;
	}

	e->setData(d);
	postEvent(g_pIdentSentinel, e);
}

void KviIdentDaemon::run()
{
	m_sock = KVI_INVALID_SOCKET;
	m_sock6 = KVI_INVALID_SOCKET;
	bool bEventPosted = false;

	m_pRequestList = new KviPointerList<KviIdentRequest>;
	m_pRequestList->setAutoDelete(true);

	KviPointerList<KviIdentRequest> dying;
	dying.setAutoDelete(false);

#ifdef COMPILE_IPV6_SUPPORT
	// If we have enabled ipv6 and we have to use a single socket: this one is IPV6
	// otherwise this one is IPV4
	KviSockaddr sa(m_uPort, m_bEnableIPv6 && m_bIPv6ContainsIPv4);
#else
	KviSockaddr sa(m_uPort, false);
#endif

	KviIdentRequest * r;

#ifdef COMPILE_IPV6_SUPPORT
	m_sock = kvi_socket_create((m_bEnableIPv6 && m_bIPv6ContainsIPv4) ? KVI_SOCKET_PF_INET6 : KVI_SOCKET_PF_INET, KVI_SOCKET_TYPE_STREAM, KVI_SOCKET_PROTO_TCP);
#else
	m_sock = kvi_socket_create(KVI_SOCKET_PF_INET, KVI_SOCKET_TYPE_STREAM, KVI_SOCKET_PROTO_TCP);
#endif

	if(m_sock == KVI_INVALID_SOCKET)
	{
		postMessage(__tr("Can't start the Ident service : socket() failed"), nullptr);
		goto exit_thread;
	}

	if(!kvi_socket_setNonBlocking(m_sock))
	{
		postMessage(__tr("Can't start the Ident service : async setting failed"), nullptr);
		goto exit_thread;
	}

	if(!sa.socketAddress())
	{
		postMessage(__tr("Can't enable the Ident service : can't setup the listen address"), nullptr);
		goto exit_thread;
	}

	if(!kvi_socket_bind(m_sock, sa.socketAddress(), ((int)(sa.addressLength()))))
	{
		postMessage(__tr("Can't start the Ident service : bind() failed"), nullptr);
		goto exit_thread;
	}

	if(!kvi_socket_listen(m_sock, 128))
	{
		postMessage(__tr("Can't start the Ident service : listen() failed"), nullptr);
		goto exit_thread;
	}

#ifdef COMPILE_IPV6_SUPPORT
	if(m_bEnableIPv6 && (!m_bIPv6ContainsIPv4))
	{
		// Need to start the IPV6 socket too
		KviSockaddr sa6(m_uPort, true);
		m_sock6 = kvi_socket_create(KVI_SOCKET_PF_INET6, KVI_SOCKET_TYPE_STREAM, KVI_SOCKET_PROTO_TCP);

		if(m_sock6 == KVI_INVALID_SOCKET)
		{
			postMessage(__tr("Can't start the Ident service on IPv6 : socket() failed"), nullptr);
			goto ipv6_failure;
		}

		if(!kvi_socket_setNonBlocking(m_sock6))
		{
			postMessage(__tr("Can't start the Ident service on IPv6 : async setting failed"), nullptr);
			kvi_socket_close(m_sock6);
			m_sock6 = KVI_INVALID_SOCKET;
			goto ipv6_failure;
		}

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		/*
			 * under windows, ipv6 hosts behind an ipv4 NAT can use Teredo to be reachable from the outer internet using their
                         * ipv6 address; the default protection level doesn't permit this, so we need to set this socket option
			 * see bug ticket #393
                         */
		int iPlu = KVI_PROTECTION_LEVEL_UNRESTRICTED;
		if(!kvi_socket_setsockopt(m_sock6, KVI_IPPROTO_IPV6, KVI_IPV6_PROTECTION_LEVEL, (const void *)&iPlu, sizeof(iPlu)))
		{
			//this is not a fatal error
			postMessage(__tr("Can't set the IPv6 Ident service protection level to unrestricted: the Ident service won't be exposed to the internet"), 0);
		}
#endif
		if(!sa6.socketAddress())
		{
			postMessage(__tr("Can't enable the Ident service on IPv6 : can't setup the listen address"), nullptr);
			kvi_socket_close(m_sock6);
			m_sock6 = KVI_INVALID_SOCKET;
			goto ipv6_failure;
		}

		if(!kvi_socket_bind(m_sock6, sa6.socketAddress(), ((int)(sa6.addressLength()))))
		{
			postMessage(__tr("Can't start the Ident service on IPv6 : bind() failed"), nullptr);
			kvi_socket_close(m_sock6);
			m_sock6 = KVI_INVALID_SOCKET;
			goto ipv6_failure;
		}

		if(!kvi_socket_listen(m_sock6, 128))
		{
			postMessage(__tr("Can't start the Ident service on IPv6 : listen() failed"), nullptr);
			kvi_socket_close(m_sock6);
			m_sock6 = KVI_INVALID_SOCKET;
			goto ipv6_failure;
		}
	}
#endif

ipv6_failure:

#ifdef COMPILE_IPV6_SUPPORT
	if(m_bEnableIPv6)
	{
		if(m_sock6 != KVI_INVALID_SOCKET)
		{
			if(_OUTPUT_PARANOIC)
				postMessage(__tr("Starting Ident service (IPv4/v6 on separate namespaces)"), nullptr);
		}
		else
		{
			if(_OUTPUT_PARANOIC)
				postMessage(__tr("Starting Ident service (IPv4/v6 in IPv6 namespace)"), nullptr);
		}
	}
	else
	{
		if(_OUTPUT_PARANOIC)
			postMessage(__tr("Starting Ident service (IPv4)"), nullptr);
	}
#else  //!COMPILE_IPV6_SUPPORT
	if(_OUTPUT_PARANOIC)
		postMessage(__tr("Service startup (IPv4)"), 0);
#endif //!COMPILE_IPV6_SUPPORT

	for(;;)
	{
		if(KviThreadEvent * e = dequeueEvent())
		{
			// This can be ONLY a terminate event
			delete e;
			goto exit_on_request;
		}

		struct timeval t;
		t.tv_sec = 0;
		t.tv_usec = 10000;

		int nmax = 0;

		fd_set rs;
		FD_ZERO(&rs);

		if(m_sock != KVI_INVALID_SOCKET)
		{
			FD_SET(m_sock, &rs);
			if(((unsigned int)m_sock) > ((unsigned int)nmax))
				nmax = m_sock;
		}
#ifdef COMPILE_IPV6_SUPPORT
		if(m_sock6 != KVI_INVALID_SOCKET)
		{
			FD_SET(m_sock6, &rs);
			if(((unsigned int)m_sock6) > ((unsigned int)nmax))
				nmax = m_sock6;
		}
#endif

		for(r = m_pRequestList->first(); r; r = m_pRequestList->next())
		{
			FD_SET(r->m_sock, &rs);
			if(((unsigned int)r->m_sock) > ((unsigned int)nmax))
				nmax = r->m_sock;
		}

		// FIXME: SO_REUSEADDR ?

		int ret = kvi_socket_select(nmax + 1, &rs, nullptr, nullptr, &t);

		if(ret == 0)
			msleep(100);
		else
		{

			if(m_sock != KVI_INVALID_SOCKET)
			{
				if(FD_ISSET(m_sock, &rs))
				{
#ifdef COMPILE_IPV6_SUPPORT
					KviSockaddr satmp(0, m_bEnableIPv6 && m_bIPv6ContainsIPv4);
#else
					KviSockaddr satmp(0, false);
#endif
					int salen = (int)satmp.addressLength();

					kvi_socket_t t = kvi_socket_accept(m_sock, satmp.socketAddress(), &salen);
					if(t != KVI_INVALID_SOCKET)
					{
						QString szHost;
						if(!satmp.getStringAddress(szHost))
							szHost = "unknown";
						KviIdentRequest * r = new KviIdentRequest(t, szHost.toUtf8().data(), satmp.port());
						m_pRequestList->append(r);
						postMessage(__tr("Ident service accepting connection"), r);
					}
				}
			}

#ifdef COMPILE_IPV6_SUPPORT
			if(m_sock6 != KVI_INVALID_SOCKET)
			{
				if(FD_ISSET(m_sock6, &rs))
				{
					KviSockaddr satmp(0, true);
					int salen = (int)satmp.addressLength();

					kvi_socket_t t = kvi_socket_accept(m_sock6, satmp.socketAddress(), &salen);
					if(t != KVI_INVALID_SOCKET)
					{
						QString szHost;
						if(!satmp.getStringAddress(szHost))
							szHost = "unknown";
						KviIdentRequest * r = new KviIdentRequest(t, szHost.toUtf8().data(), satmp.port());
						m_pRequestList->append(r);
						postMessage(__tr("Ident service accepting connection"), r);
					}
				}
			}
#endif

			for(r = m_pRequestList->first(); r; r = m_pRequestList->next())
			{
				if(FD_ISSET(r->m_sock, &rs))
				{
					char buffer[1025];
					int readed = kvi_socket_recv(r->m_sock, buffer, 1024);
					if(readed > 0)
					{
						buffer[readed] = '\0';
						r->m_szData.append(buffer);
					}
					else
					{
						// error ?
						if(readed < 0)
						{
							int err = kvi_socket_error();
							if(!kvi_socket_recoverableConnectError(err))
							{
								postMessage(__tr("Ident service socket error : dropping connection"), r);
								dying.append(r);
							}
						}
						else
						{
							// connection closed
							postMessage(__tr("Ident service connection closed by remote host"), r);
							dying.append(r);
						}
					}
				}
			}

			for(r = m_pRequestList->first(); r; r = m_pRequestList->next())
			{

				int idx = r->m_szData.findFirstIdx('\n');

				if(idx != -1)
				{
					// Ok...parse the request
					KviCString szReq = r->m_szData.left(idx);
					r->m_szData.cutLeft(idx + 1);
					szReq.trim();

					if(szReq.hasData())
					{
						postMessage(__tr("Ident service processing request"), r, szReq.ptr());

						if(kvi_strEqualCI("VERSION", szReq.ptr()))
						{
							KviCString reply("Quad-Echelon 7.12-r-244");
							kvi_socket_write(r->m_sock, reply.ptr(), reply.len());
						}
						else
						{
							KviCString reply(KviCString::Format, "%s : USERID : UNIX : %s\r\n", szReq.ptr(), m_szUser.ptr());
							kvi_socket_write(r->m_sock, reply.ptr(), reply.len());
						}

						dying.append(r);
					}
					else
					{

						postMessage(__tr("Empty request (EOT ?)"), r, szReq.ptr());

						dying.append(r);
					}
				}
				else
				{
					if(r->m_szData.len() > 1024)
					{
						// request too long...kill it
						dying.append(r);
						postMessage(__tr("Dropping connection (request too long)"), r);
					}
				}
			}
		}

		time_t curTime = time(nullptr);

		for(r = m_pRequestList->first(); r; r = m_pRequestList->next())
		{
			if((unsigned int)(curTime - r->m_tStart) > 30)
			{
				postMessage(__tr("Timed out while waiting for the request : dropping connection"), r);
				dying.append(r);
			}
		}

		for(KviIdentRequest * ir = dying.first(); ir; ir = dying.next())
			m_pRequestList->removeRef(ir);

		dying.clear();
	}

exit_on_request:

	postEvent(g_pIdentSentinel, new KviThreadEvent(KVI_IDENT_THREAD_EVENT_EXITING_ON_REQUEST));
	bEventPosted = true;

exit_thread:

	if(!bEventPosted)
		postEvent(g_pIdentSentinel, new KviThreadEvent(KVI_IDENT_THREAD_EVENT_EXITING));

	if(m_sock != KVI_INVALID_SOCKET)
		kvi_socket_close(m_sock);
	if(m_sock6 != KVI_INVALID_SOCKET)
		kvi_socket_close(m_sock6);
	delete m_pRequestList;
	m_pRequestList = nullptr;

}

/*
	@doc: ident.start
	@type:
		command
	@title:
		ident.start
	@short:
		Starts the builtin Ident service
	@syntax:
		ident.start
	@description:
		Starts the builtin Ident service.[br]
		WARNING: the KVIrc Ident service is just a partial implementation
		of the RFC specifications. You should use is ONLY if you can't get
		any other Ident daemon running on your machine.[br]
*/

static bool ident_kvs_cmd_start(KviKvsModuleCommandCall *)
{
	if(!g_iIdentDaemonRunningUsers)
		startIdentService();
	g_iIdentDaemonRunningUsers++;
	return true;
}

/*
	@doc: ident.stop
	@type:
		command
	@title:
		ident.stop
	@short:
		Stops the Ident service
	@syntax:
		ident.stop
	@description:
		Stops the Ident service
	@seealso:
		[cmd]ident.start[/cmd]
*/

static bool ident_kvs_cmd_stop(KviKvsModuleCommandCall *)
{
	if(g_iIdentDaemonRunningUsers)
		g_iIdentDaemonRunningUsers--;
	if(!g_iIdentDaemonRunningUsers)
		stopIdentService();
	return true;
}

static bool ident_module_init(KviModule * m)
{
	g_pIdentSentinel = new KviIdentSentinel();
	KVSM_REGISTER_SIMPLE_COMMAND(m, "start", ident_kvs_cmd_start);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "stop", ident_kvs_cmd_stop);
	return true;
}

static bool ident_module_cleanup(KviModule *)
{
	stopIdentService();
	delete g_pIdentSentinel;
	g_pIdentSentinel = nullptr;

	return true;
}

static bool ident_module_can_unload(KviModule *)
{
	return !g_pIdentDaemon;
}

KVIRC_MODULE(
    "Ident",                                                        // module name
    "4.0.0",                                                        // module version
    "Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
    "Ident service",
    ident_module_init,
    ident_module_can_unload,
    0,
    ident_module_cleanup,
    0)
