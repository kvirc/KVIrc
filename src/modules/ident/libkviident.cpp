//
//   File : libkviident.cpp
//   Creation date : Tue Oct  2 18:22:04 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_module.h"

#include "libkviident.h"

#include "kvi_socket.h"
#include "kvi_app.h"
#include "kvi_out.h"
#include "kvi_netutils.h"
#include "kvi_locale.h"
#include "kvi_window.h"

#include "kvi_options.h"
#include "kvi_defaults.h"

#define KVI_IDENT_THREAD_EVENT_EXITING KVI_THREAD_USER_EVENT_BASE + 111
#define KVI_IDENT_THREAD_EVENT_EXITING_ON_REQUEST KVI_THREAD_USER_EVENT_BASE + 112


// FIXME: Should have a timeout on the requests!!!

static KviIdentDaemon * g_pIdentDaemon = 0;
static KviIdentSentinel * g_pIdentSentinel = 0;

extern KVIRC_API int g_iIdentDaemonRunningUsers;

void startIdentService()
{
//	debug("Stargin");
	if(!g_pIdentDaemon)g_pIdentDaemon = new KviIdentDaemon();
	if(!g_pIdentDaemon->isRunning())g_pIdentDaemon->start();
	while(g_pIdentDaemon->isStartingUp())
	{
#ifdef COMPILE_ON_WINDOWS
		Sleep(10);
#else
		usleep(100);
#endif
	}
//	debug("Service started");
}

void stopIdentService()
{
//	debug("Stopping");
	if(g_pIdentDaemon)delete g_pIdentDaemon;
    g_pIdentDaemon = 0;
//	debug("Stopped");
}

KviIdentSentinel::KviIdentSentinel()
: QObject(0)
{
}

KviIdentSentinel::~KviIdentSentinel()
{
	KviThreadManager::killPendingEvents(this);
}

bool KviIdentSentinel::event(QEvent *e)
{
	if(KVI_OPTION_UINT(KviOption_uintIdentdOutputMode)==KviIdentdOutputMode::Quiet || !g_pActiveWindow)
		return QObject::event(e);

	KviWindow * pTarget = KVI_OPTION_UINT(KviOption_uintIdentdOutputMode)==KviIdentdOutputMode::ToActiveWindow ?
		(KviWindow *)g_pActiveWindow : (KviWindow *)g_pApp->activeConsole();

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
							pTarget->output(KVI_OUT_IDENT,__tr("%s (%s) (%s:%u)"),d->szMessage.ptr(),d->szAux.ptr(),d->szHost.ptr(),d->uPort);
						else
							pTarget->output(KVI_OUT_IDENT,__tr("%s (%s)"),d->szMessage.ptr(),d->szAux.ptr());
					} else {
						if(_OUTPUT_PARANOIC)
							pTarget->output(KVI_OUT_IDENT,__tr("%s (%s:%u)"),d->szMessage.ptr(),d->szHost.ptr(),d->uPort);
						else
							pTarget->output(KVI_OUT_IDENT,__tr("%s"),d->szMessage.ptr());
					}
				} else {
						pTarget->output(KVI_OUT_IDENT,__tr("[IDENT]: %s"),d->szMessage.ptr());
				}
			}
			delete d;
		} else if(((KviThreadEvent *)e)->id() == KVI_IDENT_THREAD_EVENT_EXITING)
		{
			if(_OUTPUT_VERBOSE)
				if(pTarget)pTarget->outputNoFmt(KVI_OUT_IDENT,__tr("Shutting down identd service (spontaneous action)"));
			stopIdentService();
		} else if(((KviThreadEvent *)e)->id() == KVI_IDENT_THREAD_EVENT_EXITING_ON_REQUEST)
		{
			if(_OUTPUT_VERBOSE)
				if(pTarget)pTarget->outputNoFmt(KVI_OUT_IDENT,__tr("Shutting down identd service (requested action)"));
		}

		return true;
	}

	return QObject::event(e);
}


KviIdentRequest::KviIdentRequest(kvi_socket_t sock,const char * host,kvi_u32_t uPort)
{
	m_sock = sock;
	m_szHost = host;
	m_uPort = uPort;
	m_tStart = time(0);
}

KviIdentRequest::~KviIdentRequest()
{
	kvi_socket_close(m_sock);
}



KviIdentDaemon::KviIdentDaemon()
: KviSensitiveThread()
{
//	debug("Thread constructor");
	m_szUser = KVI_OPTION_STRING(KviOption_stringIdentdUser);
	if(m_szUser.isEmpty())m_szUser = "kvirc";
	m_uPort = KVI_OPTION_UINT(KviOption_uintIdentdPort);
#ifdef COMPILE_IPV6_SUPPORT
	m_bEnableIpV6 = KVI_OPTION_BOOL(KviOption_boolIdentdEnableIpV6);
#else
	m_bEnableIpV6 = false;
#endif
	m_bIpV6ContainsIpV4 = KVI_OPTION_BOOL(KviOption_boolIdentdIpV6ContainsIpV4);
//	debug("Thread constructor done");
}

KviIdentDaemon::~KviIdentDaemon()
{
//	debug("Thread destructor");
	terminate();
	g_iIdentDaemonRunningUsers = 0;

	g_pIdentDaemon = 0;
//	debug("Destructor gone");
}

void KviIdentDaemon::postMessage(const char * message,KviIdentRequest * r,const char * szAux)
{
	KviThreadDataEvent<KviIdentMessageData> * e = new KviThreadDataEvent<KviIdentMessageData>(KVI_THREAD_EVENT_DATA);

	KviIdentMessageData * d = new KviIdentMessageData;

	d->szMessage = message;
	if(szAux)d->szAux = szAux;

	if(r)
	{
		d->szHost = r->m_szHost;
		d->uPort  = r->m_uPort;
	}

	e->setData(d);
	postEvent(g_pIdentSentinel,e);
}

void KviIdentDaemon::run()
{
//	debug("RUN STARTED");
	m_sock  = KVI_INVALID_SOCKET;
	m_sock6 = KVI_INVALID_SOCKET;
	bool bEventPosted = false;

	m_pRequestList = new KviPointerList<KviIdentRequest>;
	m_pRequestList->setAutoDelete(true);

	KviPointerList<KviIdentRequest> dying;
	dying.setAutoDelete(false);

#ifdef COMPILE_IPV6_SUPPORT
	// If we have enabled ipv6 and we have to use a single socket: this one is IPV6
	// otherwise this one is IPV4
	KviSockaddr sa(m_uPort,m_bEnableIpV6 && m_bIpV6ContainsIpV4);
#else
	KviSockaddr sa(m_uPort,false);
#endif

	KviIdentRequest * r;

#ifdef COMPILE_IPV6_SUPPORT
	m_sock = kvi_socket_create((m_bEnableIpV6 && m_bIpV6ContainsIpV4) ? KVI_SOCKET_PF_INET6 : KVI_SOCKET_PF_INET,KVI_SOCKET_TYPE_STREAM,KVI_SOCKET_PROTO_TCP);
#else
	m_sock = kvi_socket_create(KVI_SOCKET_PF_INET,KVI_SOCKET_TYPE_STREAM,KVI_SOCKET_PROTO_TCP);
#endif

	if(m_sock == KVI_INVALID_SOCKET)
	{
		postMessage(__tr("Can't start the ident service : socket() failed"),0);
		goto exit_thread;
	}

	if(!kvi_socket_setNonBlocking(m_sock))
	{
		postMessage(__tr("Can't start the ident service : async setting failed"),0);
		goto exit_thread;
	}

	if(!sa.socketAddress())
	{
		postMessage(__tr("Can't enable the ident service : can't setup the listen address"),0);
		goto exit_thread;
	}

	if(!kvi_socket_bind(m_sock,sa.socketAddress(),((int)(sa.addressLength()))))
	{
		postMessage(__tr("Can't start the ident service : bind() failed"),0);
		goto exit_thread;
	}

	if(!kvi_socket_listen(m_sock,128))
	{
		postMessage(__tr("Can't start the ident service : listen() failed"),0);
		goto exit_thread;
	}


#ifdef COMPILE_IPV6_SUPPORT
	if(m_bEnableIpV6 && (!m_bIpV6ContainsIpV4))
	{
		// Need to start the IPV6 socket too
		KviSockaddr sa6(m_uPort,true);
		m_sock6 = kvi_socket_create(KVI_SOCKET_PF_INET6,KVI_SOCKET_TYPE_STREAM,KVI_SOCKET_PROTO_TCP);

		if(m_sock6 == KVI_INVALID_SOCKET)
		{
			postMessage(__tr("Can't start the ident service on IpV6 : socket() failed"),0);
			goto ipv6_failure;
		}

		if(!kvi_socket_setNonBlocking(m_sock6))
		{
			postMessage(__tr("Can't start the ident service on IpV6 : async setting failed"),0);
			kvi_socket_close(m_sock6);
			m_sock6 = KVI_INVALID_SOCKET;
			goto ipv6_failure;
		}

		if(!sa6.socketAddress())
		{
			postMessage(__tr("Can't enable the ident service on IpV6 : can't setup the listen address"),0);
			kvi_socket_close(m_sock6);
			m_sock6 = KVI_INVALID_SOCKET;
			goto ipv6_failure;
		}
	
		if(!kvi_socket_bind(m_sock6,sa6.socketAddress(),((int)(sa6.addressLength()))))
		{
			postMessage(__tr("Can't start the ident service on IpV6 : bind() failed"),0);
			kvi_socket_close(m_sock6);
			m_sock6 = KVI_INVALID_SOCKET;
			goto ipv6_failure;

		}
	
		if(!kvi_socket_listen(m_sock6,128))
		{
			postMessage(__tr("Can't start the ident service on IpV6 : listen() failed"),0);
			kvi_socket_close(m_sock6);
			m_sock6 = KVI_INVALID_SOCKET;
			goto ipv6_failure;
		}

	}
#endif

ipv6_failure:

#ifdef COMPILE_IPV6_SUPPORT
	if(m_bEnableIpV6)
	{
		if(m_sock6 != KVI_INVALID_SOCKET) {
			if(_OUTPUT_PARANOIC)
				postMessage(__tr("Starting identd service (IpV4/V6 on separate namespaces)"),0);
		} else {
			if(_OUTPUT_PARANOIC)
				postMessage(__tr("Starting identd service (IpV4/V6 in IpV6 namespace)"),0);
		}

	} else {
		if(_OUTPUT_PARANOIC)
			postMessage(__tr("Starting identd service (IpV4)"),0);
	}
#else //!COMPILE_IPV6_SUPPORT
	if(_OUTPUT_PARANOIC)
		postMessage(__tr("Service startup (IpV4)"),0);
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
			FD_SET(m_sock,&rs);
			if(((unsigned int)m_sock) > ((unsigned int)nmax))nmax = m_sock;
		}
#ifdef COMPILE_IPV6_SUPPORT
		if(m_sock6 != KVI_INVALID_SOCKET)
		{
			FD_SET(m_sock6,&rs);
			if(((unsigned int)m_sock6) > ((unsigned int)nmax))nmax = m_sock6;
		}
#endif

		for(r = m_pRequestList->first();r;r = m_pRequestList->next())
		{
			FD_SET(r->m_sock,&rs);
			if(((unsigned int)r->m_sock) > ((unsigned int)nmax))nmax = r->m_sock;
		}

		// FIXME: SO_REUSEADDR ?


		int ret = kvi_socket_select(nmax + 1,&rs,0,0,&t);


		if(ret == 0)msleep(100);
		else {


			if(m_sock != KVI_INVALID_SOCKET)
			{
				if(FD_ISSET(m_sock,&rs))
				{
#ifdef COMPILE_IPV6_SUPPORT
					KviSockaddr satmp(0,m_bEnableIpV6 && m_bIpV6ContainsIpV4);
#else
					KviSockaddr satmp(0,false);
#endif
					int salen = (int)satmp.addressLength();

					kvi_socket_t t = kvi_socket_accept(m_sock,satmp.socketAddress(),&salen);
					if(t != KVI_INVALID_SOCKET)
					{
						QString szHost;
						if(!satmp.getStringAddress(szHost))szHost = "unknown";
						KviIdentRequest * r = new KviIdentRequest(t,szHost.utf8().data(),satmp.port());
						m_pRequestList->append(r);
						postMessage(__tr("Identd accepting connection"),r);
					}
				}
			}

#ifdef COMPILE_IPV6_SUPPORT
			if(m_sock6 != KVI_INVALID_SOCKET)
			{
				if(FD_ISSET(m_sock6,&rs))
				{
					KviSockaddr satmp(0,true);
					int salen = (int)satmp.addressLength();

					kvi_socket_t t = kvi_socket_accept(m_sock6,satmp.socketAddress(),&salen);
					if(t != KVI_INVALID_SOCKET)
					{
						QString szHost;
						if(!satmp.getStringAddress(szHost))szHost = "unknown";
						KviIdentRequest * r = new KviIdentRequest(t,szHost.utf8().data(),satmp.port());
						m_pRequestList->append(r);
						postMessage(__tr("Identd accepting connection"),r);
					}
				}
			}
#endif

			for(r = m_pRequestList->first();r;r = m_pRequestList->next())
			{
				if(FD_ISSET(r->m_sock,&rs))
				{
					char buffer[1025];
					int readed = kvi_socket_recv(r->m_sock,buffer,1024);
					if(readed > 0)
					{
						buffer[readed] = '\0';
						r->m_szData.append(buffer);
					} else {
						// error ?
						if(readed < 0)
						{
							int err = kvi_socket_error();
							if(!kvi_socket_recoverableConnectError(err))
							{
								postMessage(__tr("Identd socket error : dropping connection"),r);
								dying.append(r);
							}
						} else {
							// connection closed
							postMessage(__tr("Identd connection closed by remote host"),r);
							dying.append(r);
						}
					}
				}
			}

			for(r = m_pRequestList->first();r;r = m_pRequestList->next())
			{
	
				int idx = r->m_szData.findFirstIdx('\n');
	
				if(idx != -1)
				{
					// Ok...parse the request
					KviStr szReq = r->m_szData.left(idx);
					r->m_szData.cutLeft(idx + 1);
					szReq.stripWhiteSpace();
	
					if(szReq.hasData())
					{
						postMessage(__tr("Identd processing request"),r,szReq.ptr());

						if(kvi_strEqualCI("VERSION",szReq.ptr()))
						{
							KviStr reply("Quad-Echelon 7.12-r-244");
							kvi_socket_write(r->m_sock,reply.ptr(),reply.len());
						} else {
							KviStr reply(KviStr::Format,"%s : USERID : UNIX : %s\r\n",szReq.ptr(),m_szUser.ptr());
							kvi_socket_write(r->m_sock,reply.ptr(),reply.len());
						}
	
						dying.append(r);
	
					} else {
	
						postMessage(__tr("Empty request (EOT ?)"),r,szReq.ptr());
	
						dying.append(r);
					}
	
				} else {
					//				debug("Data is : (%s)",r->m_szData.ptr());
					if(r->m_szData.len() > 1024)
					{
						// request too long...kill it
						dying.append(r);
						postMessage(__tr("Dropping connection (request too long)"),r);
					}
				}
	
			}
		}

		time_t curTime = time(0);

		for(r = m_pRequestList->first();r;r = m_pRequestList->next())
		{
			if((unsigned int)(curTime - r->m_tStart) > 30)
			{
				postMessage(__tr("Timed out while waiting for the request : dropping connection"),r);
				dying.append(r);
			}
		}

		for(KviIdentRequest * ir = dying.first();ir;ir = dying.next())
				m_pRequestList->removeRef(ir);

		dying.clear();

	}



exit_on_request:

	postEvent(g_pIdentSentinel,new KviThreadEvent(KVI_IDENT_THREAD_EVENT_EXITING_ON_REQUEST));
	bEventPosted = true;

exit_thread:

	if(!bEventPosted)
		postEvent(g_pIdentSentinel,new KviThreadEvent(KVI_IDENT_THREAD_EVENT_EXITING));


	if(m_sock != KVI_INVALID_SOCKET)kvi_socket_close(m_sock);
	if(m_sock6 != KVI_INVALID_SOCKET)kvi_socket_close(m_sock6);
	delete m_pRequestList;
    m_pRequestList = 0;

//	debug("RUN EXITING");
}


/*
	@doc: ident.start
	@type:
		command
	@title:
		ident.start
	@short:
		Starts the builtin ident service
	@syntax:
		ident.start
	@description:
		Starts the builtin ident service.[br]
		WARNING: the kvirc ident service is just a partial implementation
		of the RFC specifications. You should use is ONLY if you can't get
		any other ident daemon running on your machine.[br]
*/

static bool ident_kvs_cmd_start(KviKvsModuleCommandCall * c)
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
		Stops the ident service
	@syntax:
		ident.stop
	@description:
		Stops the ident service
	@seealso:
		[cmd]ident.start[/cmd]
*/

static bool ident_kvs_cmd_stop(KviKvsModuleCommandCall * c)
{
	if(g_iIdentDaemonRunningUsers) g_iIdentDaemonRunningUsers--;
	if(!g_iIdentDaemonRunningUsers) stopIdentService();
	return true;
}

static bool ident_module_init(KviModule *m)
{
	g_pIdentSentinel = new KviIdentSentinel();
	KVSM_REGISTER_SIMPLE_COMMAND(m,"start",ident_kvs_cmd_start);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"stop",ident_kvs_cmd_stop);
	return true;
}

static bool ident_module_cleanup(KviModule *m)
{
	stopIdentService();
	delete g_pIdentSentinel;
	g_pIdentSentinel = 0;

	return true;
}

static bool ident_module_can_unload(KviModule *m)
{
	return !g_pIdentDaemon;
}

KVIRC_MODULE(
	"Ident",                                              // module name
	"1.0.0",                                                // module version
	"Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
	"Ident service",
	ident_module_init,
	ident_module_can_unload,
	0,
	ident_module_cleanup
)

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "libkviident.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

