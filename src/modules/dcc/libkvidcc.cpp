//=============================================================================
//
//   File : libkvidcc.cpp
//   Creation date : Wed Sep 09 2000 20:59:01 by Szymon Stefanek
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

#include "DccVoiceGsmCodec.h"
#include "DccBroker.h"
#include "DccVoiceWindow.h"
#ifndef COMPILE_DISABLE_DCC_VIDEO
#include "DccVideoWindow.h"
#endif
#include "DccUtils.h"
#include "DccFileTransfer.h"
#include "DccWindow.h"

#include "kvi_debug.h"
#include "kvi_settings.h"
#include "KviCString.h"
#include "KviModule.h"
#include "KviIrcServerParser.h"
#include "KviLocale.h"
#include "kvi_out.h"
#include "KviConsoleWindow.h"
#include "KviNetUtils.h"
#include "KviMainWindow.h"
#include "KviError.h"
#include "KviOptions.h"
#include "kvi_defaults.h"
#include "KviApplication.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionUserInfo.h"

#include <QFileInfo>

#ifdef COMPILE_ON_WINDOWS
// Ugly Windoze compiler...
#include "DccDialog.h"
#endif

//#warning "KviOption_boolIgnoreDccChat and other types too"

//extern KVIRC_API KviSharedFilesManager * g_pSharedFilesManager;

DccBroker * g_pDccBroker = nullptr;

static void dcc_module_set_dcc_type(DccDescriptor * d, const char * szBaseType)
{
	d->szType = szBaseType;
#ifdef COMPILE_SSL_SUPPORT
	if(d->bIsSSL)
		d->szType.prepend('S');
#endif
	if(d->bIsTdcc)
		d->szType.prepend('T');
}

static bool dcc_kvs_parse_default_parameters(DccDescriptor * d, KviKvsModuleCommandCall * c)
{
	d->bIsTdcc = c->switches()->find('t', "tdcc");

	KviKvsVariant * pSw = c->switches()->find('m', "minimize");

	if(pSw != nullptr)
		d->bOverrideMinimize = pSw->asBoolean();
	else
		d->bOverrideMinimize = false;

	if(!d->console())
	{
		// We don't need a console with -c and -n, otherwise we need it
		if(!(c->switches()->find('c', "connect") || c->switches()->find('n', "no-ctcp")))
		{
			delete d;
			c->error(__tr2qs_ctx("This window has no associated IRC context (an IRC context is required unless -c or -n are passed)", "dcc"));
			return false;
		}
		else
			d->setConsole(g_pMainWindow->firstConsole());
	}

	KVI_ASSERT(d->console());

	if(!d->console()->isConnected())
	{
		// We don't need a connection with -c and -n, otherwise we need it
		if(!(c->switches()->find('c', "connect") || c->switches()->find('n', "no-ctcp")))
		{
			delete d;
			c->error(__tr2qs_ctx("You're not connected to a server (an active connection is required unless -c or -n are passed)", "dcc"));
			return false;
		}
		else
		{
			// -c or -n, grab a local nick from somewhere
			d->szLocalNick = KVI_OPTION_STRING(KviOption_stringNickname1).trimmed();
			if(d->szLocalNick.isEmpty())
				d->szLocalNick = KVI_DEFAULT_NICKNAME1;
			d->szLocalUser = __tr2qs_ctx("unknown", "dcc"); // we can live without it
			d->szLocalHost = d->szLocalUser;                // we can live without it
		}
	}
	else
	{
		// We know everything
		d->szLocalNick = d->console()->connection()->userInfo()->nickName();
		d->szLocalUser = d->console()->connection()->userInfo()->userName();
		d->szLocalHost = d->console()->connection()->userInfo()->hostName();
	}

	if((pSw = c->switches()->find('i', "ip")))
	{
		pSw->asString(d->szListenIp);
		if(!(d->szListenIp.contains('.') || d->szListenIp.contains(':')))
		{
			// This will magically work with the same buffer as source and dst!
			if(!KviNetUtils::getInterfaceAddress(d->szListenIp, d->szListenIp))
			{
				c->error(__tr2qs_ctx("Unable to get address of interface %Q", "dcc"), &(d->szListenIp));
				delete d;
				return false;
			}
		}
	}
	else
	{
		QString tmp;
		if(!dcc_kvs_get_listen_ip_address(c, d->console(), tmp))
		{
			delete d;
			c->error(__tr2qs_ctx("No suitable interfaces to listen on, use -i", "dcc"));
			return false;
		}
		d->szListenIp = tmp;
	}

	if((pSw = c->switches()->find('p', "port")))
	{
		pSw->asString(d->szListenPort); // fixme!
	}
	else
		d->szListenPort = "0"; // any port is ok

	if((pSw = c->switches()->find('a', "fake-address")))
	{
		pSw->asString(d->szFakeIp);
	}
	else
	{
		if(KVI_OPTION_BOOL(KviOption_boolDccSendFakeAddressByDefault))
		{
			d->szFakeIp = KVI_OPTION_STRING(KviOption_stringDefaultDccFakeAddress);
			if(d->szFakeIp.isEmpty())
				KVI_OPTION_BOOL(KviOption_boolDccSendFakeAddressByDefault) = false;
		}
	}

	if((pSw = c->switches()->find('f', "fake-port")))
	{
		pSw->asString(d->szFakePort);
	}

	d->bDoTimeout = (!c->switches()->find('u', "unlimited"));
#ifdef COMPILE_SSL_SUPPORT
	d->bIsSSL = c->switches()->find('s', "ssl");
#else
	if(c->switches()->find('s', "ssl"))
		c->warning(__tr2qs_ctx("This executable was built without SSL support, -s switch ignored", "dcc"));
#endif

	return true;
}

/*
	@doc: dcc.chat
	@type:
		command
	@title:
		dcc.chat
	@short:
		Starts a DCC chat connection
	@syntax:
		dcc.chat [-s] [-n] [-c] [-u] [-m[=<boolean>]] [-i=<interface>] [-p=<port>] [-a=<fake address>] [-f=<fake port>] <nickname>
	@switches:
		!sw: -m[=<boolean>] | --minimize[=<boolean>]
		If the -m switch is passed, the default boolCreateMinimizedDccChat option
		is overridden with the <boolean> parameter passed. So actually
		by passing -m=1 will create a minimized DCC SEND even
		if the [fnc]$option[/fnc](boolCreateMinimizedDccChat) returns false.[br]
		In the same way, by passing -m=0 you will create a non minimized DCC SEND.
		If no <boolean> value is specified, it defaults to 1.[br]
		!sw: -n | --no-ctcp
		Do [b]not[/b] send the CTCP request to the target user, you will have to do it manually,
		or the remote user will have to connect manually (for example by using dcc.chat -c).[br]
		!sw: -c | --connect
		Attempt to CONNECT to the remote host specified as <interface> and <port>,
		instead of listening (active connection instead of a passive one).
		In this case the -i and -p switches are mandatory.[br]
		The [b]c[/b] switch takes precedence over [b]n[/b] (In fact both should
		be mutually exclusive).[br]
		If the [b]c[/b] and [b]n[/b] switches are missing, this commands
		needs to be executed in a window that is bound to a connected
		IRC context (you need a third entity to accomplish the negotiation).[br]
		!sw: -i=<interface> | --ip=<interface>
		Bind the local listening socket to the specified <interface> (which is an IP address, IPv4 or IPv6).
		If this switch is [b]not[/b] specified, the socket is bound to the interface of
		the current IRC connection (if any) or to [i]127.0.0.1[/i].[br]
		You can also specify a local interface name to get the address from (this works only for IPv4 interfaces
		since IPv6 ones seems to be unsupported by the system ioctl() calls at the moment (for Linux at least)).[br]
		Here go some examples:[br]
		-i=215.243.12.12: this will bind to the IPv4 interface with the specified address.[br]
		-i=3ffe:1001::1: this will bind to the IPv6 interface with the specified address.[br]
		-i=ppp0: this will bind to the IPv4 address of the interface ppp0 (if supported by the underlying system).[br]
		The -i switch parameter may serve also as a target address when the -c switch is used.[br]
		!sw: -p=<port> | --port=<port>
		Bind the local listening socket to the specified <port>.
		If this switch is [b]not[/b] specified, the port will be a [i]random[/i] one chosen by the kernel.[br]
		!sw: -a=<fake address> | --fake-address=<fake address>
		Send the <fake address> as target for the remote client in the requesting CTCP message.
		If this switch is not given, the CTCP will contain the real IP address of the listening
		interface.[br]
		!sw: -f=<fake port> | --fake-port=<fake port>
		Send the <fake port> as target port for the remote client in the requesting CTCP message.
		If this switch is not given, the CTCP will contain the real port of the listening socket.
		[br][br]
		All these switches are meant to allow maximum flexibility of the
		DCC negotiation, earlier KVIrc releases had serious problems
		with firewalled and/or masqueraded machines. With the -a and -f switches
		you can work around it.
		[br][br]
		!sw: -u | --unlimited
		If the 'u' switch is given, the connection attempt will
		never time out; this might be useful if you want to leave
		a listening socket for a friend of yours while you are sleeping
		and have the CTCP processing disabled. The 'u' switch works either
		in active and passive mode.[br]
		!sw: -s | --ssl
		Use a Secure Socket Layer for the transfer; the whole communication will be encrypted
		with a private key algorithm after a public key handshake.[br]
		This option will work only if the KVIrc executable has been compiled with SSL support
		and the remote end supports the SSL protocol too.[br]
		Please note that this will may down the transfer somewhat.[br]
		-s can be combined with -t.[br]
		The CTCP negotiation will use SSEND as parameter (or eventually TSSEND).[br]
		When requesting a SSL based DCC SEND to someone you probably will need a
		certificate. If you don't have one, create it (for example with CA.pl -newcert)
		and set it in the options dialog.
		!sw: -z | --zero-port
		Use the 0 port method. This is a dirty hack that allows you to use the CHAT
		protocol with mIrc receiving clients.
	@description:
		Attempts a DCC connection to <nickname>.[br]
		The simplest case [i]dcc.chat <nickname>[/i] will work just as in all
		the other IRC clients, but this command is really more powerful...[br]
		Before attempting to understand the possibilities of this command,
		be sure to know how [doc:dcc_connection]DCC negotiation and connections[/doc] work.
		If the [b]i[/b] switch is specified, the local listening socket
		will be bound to the specified <interface> (which is an IP address, IPv4 or IPv6),
		otherwise it will be bound to the interface of the current IRC connection.[br]
		You can also specify a local interface name to get the address from (this works only for IPv4 interfaces
		since IPv6 ones seem to be unsupported by the system ioctl() calls at the moment (in Linux at least)).[br]
		Here are some examples:[br]
		-i=215.243.12.12: This will bind to the IPv4 interface with the specified address.[br]
		-i=3ffe:1001::1: This will bind to the IPv6 interface with the specified address.[br]
		-i=ppp0: This will bind to the IPv4 address of the interface ppp0 (if supported by the underlying system).[br]
		The -i switch parameter may serve also as a target address when the -c switch is used.[br]
		If the [b]p[/b] switch is specified, the local listening socket
		will be bound to the <port>, otherwise it will be bound to
		a random port chosen by the kernel.[br]
		If the [b]a[/b] switch is specified, the requesting CTCP message
		will contain <fake address> as target for the remote user,
		otherwise the CTCP message will contain the real IP address
		of the listening interface.
		If the [b]f[/b] switch is specified, the requesting CTCP message
		will contain <fake port> as target for the remote user,
		otherwise the CTCP message will contain the real port of the
		listening socket.
		All these switches are meant to allow maximum flexibility of the
		DCC negotiation, earlier KVIrc releases had serious problems
		with firewalled and/or masqueraded machines. With the -a and -f switches
		you can workaround it.
		If the [b]n[/b] switch is specified, KVIrc will [b]not[/b] send the CTCP request
		to the target user; you will have to do it manually, or the remote user
		will have to connect manually (for example by using dcc.chat -c).
		If the [b]c[/b] switch is specified, KVIrc will attempt to connect
		to the remote host specified as <interface> and <port>, instead
		of listening (active connection instead of a passive one).
		In this case the -i and -p switches are mandatory.[br]
		The [b]c[/b] switch takes precedence over [b]n[/b] (In fact both should
		be mutually exclusive).[br]
		If the [b]c[/b] and [b]n[/b] switches are missing, this commands
		needs to be executed in a window that is bound to a connected
		IRC context (you need a third entity to accomplish the negotiation).[br]
		If the [b]u[/b] switch is given, the connection attempt will
		never time out; this might be useful if you want to leave
		a listening socket for a friend of yours while you are sleeping
		and have the CTCP processing disabled. The [b]u[/b] switch works either
		in active and passive mode.[br]
		If the -m switch is passed, the default boolCreateMinimizedDccChat option
		is overridden with the <boolean> parameter passed. So actually
		by passing -m=1 will create a minimized DCC chat even
		if the [fnc]$option[/fnc](boolCreateMinimizedDccChat) returns false.[br]
		In the same way, by passing -m=0 you will create a non minimized DCC chat.
		If no <boolean> value is specified, it defaults to 1.[br]
		-s will cause the DCC chat to be attempted in Secure Sockets Layer mode:
		the connection will be encrypted with a private key algorithm after a
		public key handshake. -s will work only if the KVIrc executable was compiled
		with SSL support enabled and if the remote end supports it.
		The eventual DCC request will contain the string SCHAT instead of CHAT.[br]
		When requesting a SSL based DCC chat to someone you probably will need a
		certificate. If you don't have one, create it (for example with CA.pl -newcert)
		and set it in the options dialog.
	@examples:
		Simple examples:
		[example]
			# Simple DCC chat to Pragma
			dcc.chat Pragma
			# DCC chat to Pragma, listen on address 127.0.0.1
			dcc.chat -i=127.0.0.1 Pragma
			# DCC chat to Pragma, listen on address 168.0.0.1 and port 1025
			dcc.chat -i=168.0.0.1 -p=1025 Pragma
		[/example]
		More tricky ones:
		[example]
			# DCC chat to Pragma, listen on address 127.0.0.1 and port 1080
			# but tell him to connect to address 212.134.22.11 port 1080
			dcc.chat -i=127.0.0.1 -p=1080 -a=212.134.22.11 Pragma
			# DCC chat to Pragma, listen on address 127.0.0.1 and port 1080
			# but tell him to connect to address 212.134.22.11 port 1090
			dcc.chat -i=127.0.0.1 -p=1080 -a=212.134.22.11 -f=1090 Pragma
		[/example]
		Now run completely out of bounds. Use dcc.chat connections
		to do various things:
		[example]
			# Tricky: simulate a HTTP server
			dcc.chat -n -i=127.0.0.1 -p=80 WebBrowser
			# Now open http://127.0.0.1 with your web browser
			# and type "<html><body>Hello!</body></html>" :)
			#
			# Tricky 2: simulate the Ident daemon (if you have none)
			dcc.chat -n -i=127.0.0.1 -p=113 Client
			#
			# Now a self-DCC connection without the IRC negotiation
			# Src: Setup a listening socket awaiting the "Destination" user connection
			dcc.chat -n -i=127.0.0.1 -p=1080 Dst
			# Dst: Connect to the listening socket at addr 127.0.0.1 and port 1080
			dcc.chat -c -i=127.0.0.1 -p=1080 Src
			# The above example will mess you a bit...
			# Try to guess why in both windows YOU have the same nickname
			# that is probably not Dst nor Src... :)
		[/example]
		Using the shell FTP program is too easy:
		we're [b]real hackers[/b] and want to do complicated things...
		[example]
			# Connect to the local FTP server and get the list of contents
			/dcc.chat -c -i=127.0.0.1 -p=21 FtpServer
			# Now login, type in the new window (the following lines are [b]not[/b] commands):
			USER youruser
			PASS yourpass
			# Now enter passive mode
			PASV
			# And watch the line that you have received...sth like
			# 227 Entering passive mode (127,0,0,1,210,195)
			/dcc.chat -c -i=127.0.0.1 -p=$((210 * 256) + 195) FtpList
			# (Change the port numbers accordingly)
			# And then type in the FTP server window (this is [b]not[/b] a KVIrc command):
			LIST
			# Then watch the ls output in the FtpList window. :)
			# In this way you can also read ASCII files by FTP:
			# Assume that in the previous ls output you have seen
			# a README file.
			# In the FTP server window type:
			PASV
			# Watch the message
			# 227 Entering passive mode (127,0,0,1,22,227)
			/dcc.chat -c -i=127.0.0.1 -p=$((22 * 256) + 227) README
			# In the FtpServer window type:
			RETR README
			# And read the file in the README window :)
		[/example]
*/

static bool dcc_kvs_cmd_chat(KviKvsModuleCommandCall * c)
{
	QString szTarget;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("target", KVS_PT_NONEMPTYSTRING, 0, szTarget)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * d = new DccDescriptor(c->window()->console());

	d->szNick = szTarget;                      // we always specify the nickname
	d->szUser = __tr2qs_ctx("unknown", "dcc"); // username is always unknown
	d->szHost = d->szUser;                     // host is always unknown

	if(!dcc_kvs_parse_default_parameters(d, c))
		return false;
	dcc_module_set_dcc_type(d, "CHAT");

	if(c->switches()->find('z', "zero-port"))
	{
		// we want to have it reversed... add a tag and send out the request
		KviDccZeroPortTag * t = g_pDccBroker->addZeroPortTag();

		d->console()->connection()->sendFmtData("PRIVMSG %s :%cDCC %s chat 127.0.0.1 0 %s%c",
		    d->console()->connection()->encodeText(d->szNick).data(),
		    0x01,
		    d->console()->connection()->encodeText(d->szType).data(),
		    d->console()->connection()->encodeText(t->m_szTag).data(),
		    0x01);

		return true;
	}

	if(c->switches()->find('c', "connect"))
	{
		if(!(c->switches()->find('i', "ip") && c->switches()->find('p', "port")))
		{
			delete d;
			c->error(__tr2qs_ctx("-c requires -i and -p", "dcc"));
			return false;
		}
		d->szIp = d->szListenIp;
		d->szPort = d->szListenPort;
		d->szListenIp = "";   // useless
		d->szListenPort = ""; // useless
		d->bActive = true;
	}
	else
	{
		d->szIp = __tr2qs_ctx("unknown", "dcc");
		d->szPort = d->szIp;
		d->bActive = false;
		d->bSendRequest = !c->switches()->find('n', "no-ctcp");
	}

	d->triggerCreationEvent();
	g_pDccBroker->executeChat(nullptr, d);

	return true;
}

/*
	@doc: dcc.send
	@type:
		command
	@title:
		dcc.send
	@short:
		Sends a file
	@syntax:
		dcc.send [-s] [-n] [-c] [-u] [-b] [-g[=<file size>]] [-t] [-m[=<boolean>]] [-i=<interface>] [-p=<port>] [-a=<fake address>] [-f=<fake port>] <nickname> [filename]
	@switches:
		!sw: -g[=<file size>] | --get[=<file size>]
		This switch is a dirty trick, you can use it to receive files from people
		behind a firewall with masquerading enabled.[br]
		It causes the transfer direction to be inverted; your client will receive
		the file from the remote host instead of sending it.[br]
		<file size> is the expected file size in bytes. This parameter can be omitted,
		and in this case the DCC will [i]blindly[/i] trust the remote end and assume
		that the file has been transferred correctly when the remote end closes the connection.[br]
		If you don't pass the -n option, the remote end will receive an informational DCC RECV request,
		specifying the IP address and the port to connect to.[br]
		-t can be used to prevent sending acknowledges to the remote end, and -u can be used
		to avoid the listening socket to timeout.[br]
		-a and -f can be used as well, but I see no real reason for that...[br]
		Here is an example of usage of this option:[br]
		spion can't accept connections (is behind a firewall with masquerading or some other reason...),
		to his machine.[br]
		spion wants to send the file important.jpg to Pragma.[br]
		spion tells to Pragma that he wants to send him the file and Pragma sets up a connection in the following way:[br]
		[b]dcc.send -g spion important.png[/b][br]
		spion will see the informational DCC RECV request with the IP address and port that Pragma is listening on.
		Assume that the address was 212.212.231.220 and the port 32344.[br]
		spion will then use the following command:[br]
		[b]dcc.send -c -i=212.212.231.220 -p=32344 Pragma /home/spion/important.jpg[/b][br]
		Hey presto! The file is transferring.[br]
		Pragma will see no file progress indication, since the file size is unknown on Pragma's side.[br]
		If spion had specified the file size, Pragma could use -g=<file size> while setting up the connection,
		to be able to see the progress indications.[br]
		If Pragma used the the -n option, the DCC RECV indication wouldn't have been sent, in this case
		Pragma would need to communicate the IP address and the port [i]manually[/i] to spion.[br]
		!sw: -b | --blind
		Assume that no acknowledges are sent.
		Assume that the transfer was successful when the whole file has been sent,
		then close the socket.[br]
		This is called a [i]blind[/i] DCC SEND.[br]
		!sw: -t | -tdcc
		Emulate the TDCC protocol: Use the TDCC CTCP message (DCC TSEND) for requesting the connection
		and assume that no acknowledges are sent. Wait for the remote end to close the connection.[br]
		!sw: -m[=<boolean>] | --minimize[=<boolean>]
		If the -m switch is passed, the default boolCreateMinimizedDccSend option
		is overridden with the <boolean> parameter passed. So actually
		by passing -m=1 will create a minimized DCC SEND even
		if the [fnc]$option[/fnc](boolCreateMinimizedDccSend) returns false.[br]
		In the same way, by passing -m=0 you will create a non minimized DCC SEND.
		If no <boolean> value is specified, it defaults to 1.[br]
		!sw: -n | --no-ctcp
		Do [b]not[/b] send the CTCP request to the target user, you will have to do it manually,
		or the remote user will have to connect manually (for example by using dcc.recv -c).[br]
		!sw: -c | --connect
		Attempt to CONNECT to the remote host specified as <interface> and <port>,
		instead of listening (active connection instead of a passive one).
		In this case the -i and -p switches are mandatory.[br]
		The 'c' switch takes precedence over 'n' (In fact both should
		be mutually exclusive).[br]
		If the 'c' and 'n' switches are missing, this commands
		needs to be executed in a window that is bound to a connected
		IRC context (you need a third entity to accomplish the negotiation).[br]
		!sw: -i=<interface> | --ip=<interface>
		Bind the local listening socket to the specified <interface> (which is an IP address, IPv4 or IPv6).
		If this switch is [b]not[/b] specified, the socket is bound to the interface of
		the current IRC connection (if any) or to [i]127.0.0.1[/i].[br]
		You can also specify a local interface name to get the address from (this works only for IPv4 interfaces
		since IPv6 ones seems to be unsupported by the system ioctl() calls at the moment (for Linux at least)).[br]
		Here go some examples:[br]
		-i=215.243.12.12: this will bind to the IPv4 interface with the specified address.[br]
		-i=3ffe:1001::1: this will bind to the IPv6 interface with the specified address.[br]
		-i=ppp0: this will bind to the IPv4 address of the interface ppp0 (if supported by the underlying system).[br]
		The -i switch parameter may serve also as a target address when the -c switch is used.[br]
		!sw: -p=<port> | --port=<port>
		Bind the local listening socket to the specified <port>.
		If this switch is [b]not[/b] specified, the port will be a [i]random[/i] one chosen by the kernel.[br]
		!sw: -a=<fake address> | --fake-address=<fake address>
		Send the <fake address> as target for the remote client in the requesting CTCP message.
		If this switch is not given, the CTCP will contain the real IP address of the listening
		interface.[br]
		!sw: -f=<fake port> | --fake-port=<fake port>
		Send the <fake port> as target port for the remote client in the requesting CTCP message.
		If this switch is not given, the CTCP will contain the real port of the listening socket.
		[br][br]
		All these switches are meant to allow maximum flexibility of the
		DCC negotiation, earlier KVIrc releases had serious problems
		with firewalled and/or masqueraded machines. With the -a and -f switches
		you can work around it.[br][/br]
		!sw: -u | --unlimited
		If the 'u' switch is given, the connection attempt will
		never time out; this might be useful if you want to leave
		a listening socket for a friend of yours while you are sleeping
		and have the CTCP processing disabled. The 'u' switch works either
		in active and passive mode.[br]
		!sw: -s | --ssl
		Use a Secure Socket Layer for the transfer; the whole communication will be encrypted
		with a private key algorithm after a public key handshake.[br]
		This option will work only if the KVIrc executable has been compiled with SSL support
		and the remote end supports the SSL protocol too.[br]
		Please note that this will may down the transfer somewhat.[br]
		-s can be combined with -t.[br]
		The CTCP negotiation will use SSEND as parameter (or eventually TSSEND).[br]
		When requesting a SSL based DCC SEND to someone you probably will need a
		certificate. If you don't have one, create it (for example with CA.pl -newcert)
		and set it in the options dialog.
	@description:
		Attempts to send the file <filename> to <nickname>.[br]
		If [filename] is specified it must be an absolute file path,
		otherwise a file selection dialog is opened.[br]
		The simplest case [i]dcc.send <nickname> <filename>[/i] will work just as in all
		the other IRC clients, but this command is really more powerful...[br]
		Before attempting to understand the possibilities of this command,
		be sure to know how a [doc:dcc_connection]DCC negotiation and connection[/doc] works.[br]
		The file will be sent as a sequence of packets which must
		be acknowledged one by one by the active client.[br]
		There is a special option (see $option()) called [i]fast send[/i] (also known
		as [i]send ahead[/i]) that makes KVIrc avoid to wait for the acknowledge
		of the last packet before sending the next one.[br]
		Anyway, the connection is declared as successful only
		when the whole file (all the packets) has been acknowledged.[br]
		If you need to send a file but you're firewalled, you might take
		a look at the [cmd]dcc.rsend[/cmd] command. It also handles
		the mIRC zero port protocol extension.
	@examples:
*/

//#warning "Example for -r"
//#warning "OPTION FOR NO GUI ? (this is hard...)"
//#warning "When in IPv6 mode, should be able to use the IPv4 binding!"

static bool dcc_kvs_cmd_send(KviKvsModuleCommandCall * c)
{
	QString szTarget, szFileName;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("target", KVS_PT_NONEMPTYSTRING, 0, szTarget)
	KVSM_PARAMETER("file name", KVS_PT_NONEMPTYSTRING, KVS_PF_OPTIONAL, szFileName)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * d = new DccDescriptor(c->window()->console());

	d->szNick = szTarget; // we always specify the nickname

	QString szTmp;
	KviKvsVariant * pSw = nullptr;

	if((pSw = c->switches()->find('g', "get")))
	{
		d->szFileName = QFileInfo(szFileName).fileName();

		if(!pSw->isBoolean())
		{
			kvs_int_t iSize;
			if(pSw->asInteger(iSize)) // is an integer
			{
				pSw->asString(szTmp);
				// avoid sprintf as long as possible
				d->szFileSize = szTmp;
			}
			else
			{
				d->szFileSize = __tr_ctx("<unknown size>", "dcc");
			}
		}
	}
	else
	{
		d->szFileName = szFileName;
		d->szLocalFileName = szFileName;
	}

	d->szUser = __tr2qs_ctx("unknown", "dcc"); // username is always unknown
	d->szHost = d->szUser;                     // host is always unknown
	d->bRecvFile = pSw != nullptr;
	d->bNoAcks = c->switches()->find('b', "blind") || c->switches()->find('t', "tdcc");
	d->bResume = false;
	d->bAutoAccept = pSw != nullptr;
	d->bIsIncomingAvatar = false;

	if(!dcc_kvs_parse_default_parameters(d, c))
		return false;

	if(c->switches()->find('c', "connect"))
	{
		if(!(c->switches()->find('i', "ip") && c->switches()->find('p', "port")))
		{
			delete d;
			c->error(__tr2qs_ctx("-c requires -i and -p", "dcc"));
			return false;
		}
		d->szIp = d->szListenIp;
		d->szPort = d->szListenPort;
		d->szListenIp = "";   // useless
		d->szListenPort = ""; // useless
		d->bActive = true;
	}
	else
	{
		d->szIp = __tr2qs_ctx("unknown", "dcc");
		d->szPort = d->szIp;
		d->bActive = false;
		d->bSendRequest = !c->switches()->find('n', "no-ctcp");
	}

	if(c->switches()->find('g', "get"))
	{
		dcc_module_set_dcc_type(d, "RECV");
		d->triggerCreationEvent();
		g_pDccBroker->recvFileManage(d);
	}
	else
	{
		dcc_module_set_dcc_type(d, "SEND");
		d->triggerCreationEvent();
		if(!d->szLocalFileName.isEmpty())
		{
			g_pDccBroker->sendFileExecute(nullptr, d);
		}
		else
		{
			g_pDccBroker->sendFileManage(d);
		}
	}

	return true;
}

/*
	@doc: dcc.recv
	@type:
		command
	@title:
		dcc.recv
	@short:
		Sets up a file receiving connection
	@syntax:
		dcc.recv [-s] [-t] [-u] [-b] [-n] [-c] [-i=<interface>] [-p=<port>] [-m[=<boolean>]] <nickname> <filename> <remote file size>
	@switches:
		!sw: -b | --blind
		Assume that no acknowledges are sent.
		Assume that the transfer was successful when the whole file has been sent,
		then close the socket.[br]
		This is called a [i]blind[/i] DCC SEND.[br]
		!sw: -t | -tdcc
		Emulate the TDCC protocol: Use the TDCC CTCP message (DCC TSEND) for requesting the connection
		and assume that no acknowledges are sent. Wait for the remote end to close the connection.[br]
		!sw: -m[=<boolean>] | --minimize[=<boolean>]
		If the -m switch is passed, the default boolCreateMinimizedDccSend option
		is overridden with the <boolean> parameter passed. So actually
		by passing -m=1 will create a minimized DCC SEND even
		if the [fnc]$option[/fnc](boolCreateMinimizedDccSend) returns false.[br]
		In the same way, by passing -m=0 you will create a non minimized DCC SEND.
		If no <boolean> value is specified, it defaults to 1.[br]
		!sw: -n | --no-ctcp
		Do [b]not[/b] send the CTCP request to the target user, you will have to do it manually,
		or the remote user will have to connect manually (for example by using dcc.recv -c).[br]
		!sw: -i=<interface> | --ip=<interface>
		Bind the local listening socket to the specified <interface> (which is an IP address, IPv4 or IPv6).
		If this switch is [b]not[/b] specified, the socket is bound to the interface of
		the current IRC connection (if any) or to [i]127.0.0.1[/i].[br]
		You can also specify a local interface name to get the address from (this works only for IPv4 interfaces
		since IPv6 ones seems to be unsupported by the system ioctl() calls at the moment (for Linux at least)).[br]
		Here go some examples:[br]
		-i=215.243.12.12: this will bind to the IPv4 interface with the specified address.[br]
		-i=3ffe:1001::1: this will bind to the IPv6 interface with the specified address.[br]
		-i=ppp0: this will bind to the IPv4 address of the interface ppp0 (if supported by the underlying system).[br]
		The -i switch parameter may serve also as a target address when the -c switch is used.[br]
		!sw: -p=<port> | --port=<port>
		Bind the local listening socket to the specified <port>.
		If this switch is [b]not[/b] specified, the port will be a [i]random[/i] one chosen by the kernel.[br]
		!sw: -a=<fake address> | --fake-address=<fake address>
		Send the <fake address> as target for the remote client in the requesting CTCP message.
		If this switch is not given, the CTCP will contain the real IP address of the listening
		interface.[br]
		!sw: -f=<fake port> | --fake-port=<fake port>
		Send the <fake port> as target port for the remote client in the requesting CTCP message.
		If this switch is not given, the CTCP will contain the real port of the listening socket.
		[br][br]
		All these switches are meant to allow maximum flexibility of the
		DCC negotiation, earlier KVIrc releases had serious problems
		with firewalled and/or masqueraded machines. With the -a and -f switches
		you can work around it.
		[br][br]
		!sw: -u | --unlimited
		If the [b][/b] switch is given, the connection attempt will
		never time out; this might be useful if you want to leave
		a listening socket for a friend of yours while you are sleeping
		and have the CTCP processing disabled. The 'u' switch works either
		in active and passive mode.[br]
		!sw: -s | --ssl
		Use a Secure Socket Layer for the transfer; the whole communication will be encrypted
		with a private key algorithm after a public key handshake.[br]
		This option will work only if the KVIrc executable has been compiled with SSL support
		and the remote end supports the SSL protocol too.[br]
		Please note that this will may down the transfer somewhat.[br]
		-s can be combined with -t.[br]
		The CTCP negotiation will use SSEND as parameter (or eventually TSSEND).[br]
		When requesting a SSL based DCC SEND to someone you probably will need a
		certificate. If you don't have one, create it (for example with CA.pl -newcert)
		and set it in the options dialog.
		!sw: -c | --connect
		Accepted for compatibility: don't use it!
	@description:
		Sets up a connection ready to receive a file.[br]
		In most 'common' cases you will not need this command, you might want to use [cmd]dcc.get[/cmd] instead.[br]
		This works like dcc.send but has two main differences: The file is INCOMING, and the CTCP sent to the other side
		is a CTCP RECV.[br]
		This command is the counterpart of [cmd]dcc.send[/cmd] and its parameters are exactly the same, so please refer to that
		help page for the full discussion. This help page contains only a brief resume of these parameters.[br]
		The [doc:dcc_connection]dcc documentation[/doc] explains the DCC RECV sub-protocol in detail.[br]
	@examples:

*/

//#warning "ENCODE THE CTCP'S!!!!!!!"
//#warning "DOCS FOR dcc.recv (examples!)"

static bool dcc_kvs_cmd_recv(KviKvsModuleCommandCall * c)
{
	QString szTarget, szFileName;
	kvs_uint_t uSize;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("target", KVS_PT_NONEMPTYSTRING, 0, szTarget)
	KVSM_PARAMETER("filename", KVS_PT_NONEMPTYSTRING, 0, szFileName)
	KVSM_PARAMETER("size", KVS_PT_UINT, 0, uSize)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * d = new DccDescriptor(c->window()->console());
	d->szNick = szTarget;
	d->szUser = __tr2qs_ctx("unknown", "dcc");
	d->szHost = d->szUser;
	d->szIp = __tr2qs_ctx("unknown", "dcc");
	d->szPort = d->szIp;

	// -c is senseless here...but we accept it for compatibility

	if(!dcc_kvs_parse_default_parameters(d, c))
		return false;

	d->szFileName = szFileName;
	d->szFileSize.setNum(uSize);

	d->bActive = false; // we have to listen!
	d->bResume = false;
	d->bRecvFile = true; // we have to receive the file!
	d->bSendRequest = !c->switches()->find('n', "no-ctcp");
	d->bNoAcks = d->bIsTdcc || c->switches()->find('b', "blind");
	d->bAutoAccept = KVI_OPTION_BOOL(KviOption_boolAutoAcceptDccSend);
	d->bIsIncomingAvatar = g_pApp->findPendingAvatarChange(d->console(), d->szNick, d->szFileName);

	if(KVI_OPTION_BOOL(KviOption_boolAutoAcceptIncomingAvatars))
		d->bAutoAccept = d->bAutoAccept || d->bIsIncomingAvatar;
	dcc_module_set_dcc_type(d, "RECV");
	d->triggerCreationEvent();
	g_pDccBroker->recvFileManage(d);

	return true;
}

/*
	@doc: dcc.rsend
	@type:
		command
	@title:
		dcc.rsend
	@short:
		Sends a file by using the Reverse DCC SEND protocol
	@syntax:
		dcc.rsend [-s] [-t] <nickname> [filename]
	@switches:
		!sw: -t | -tdcc
		Emulate the TDCC protocol.
		!sw: -s | --ssl
		Use a Secure Socket Layer for the transfer; the whole communication will be encrypted
		with a private key algorithm after a public key handshake.[br]
		This option will work only if the KVIrc executable has been compiled with SSL support
		and the remote end supports the SSL protocol too.[br]
		Please note that this will may down the transfer somewhat.[br]
		-s can be combined with -t.[br]
		The CTCP negotiation will use SSEND as parameter (or eventually TSSEND).[br]
		When requesting a SSL based DCC SEND to someone you probably will need a
		certificate. If you don't have one, create it (for example with CA.pl -newcert)
		and set it in the options dialog.
		!sw: -z | --zero-port
		Use the 0 port method. This is a dirty hack that allows you to use the RSEND
		protocol with mIrc receiving clients.
	@description:
		Sends a DCC RSEND request to <nickname> notifying him that you want to
		send him the file [filename].[br]
		The remote end may acknowledge the request by sending a DCC RECV request.
		This command effects are similar to [cmd]dcc.send[/cmd], but will work also on machines
		that can't accept incoming connections (firewalling or masquerading problems).[br]
		A 120 seconds file offer is added for the specified file and mask [i]<nickname>!*@*[/i].
	@examples:

*/

//#warning "ENCODE THE CTCP'S!!!!!!!"
//#warning "DOCS FOR dcc.rsend"

static bool dcc_kvs_cmd_rsend(KviKvsModuleCommandCall * c)
{
	QString szTarget, szFileName;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("target", KVS_PT_NONEMPTYSTRING, 0, szTarget)
	KVSM_PARAMETER("filename", KVS_PT_NONEMPTYSTRING, KVS_PF_OPTIONAL, szFileName)
	KVSM_PARAMETERS_END(c)

	KVSM_REQUIRE_CONNECTION(c)

	DccDescriptor * d = new DccDescriptor(c->window()->console());
	d->szNick = szTarget;
	d->szLocalFileName = szFileName;
	d->bIsTdcc = c->switches()->find('t', "tdcc");
#ifdef COMPILE_SSL_SUPPORT
	d->bIsSSL = c->switches()->find('s', "ssl");
#else
	if(c->switches()->find('s', "ssl"))
		c->warning(__tr2qs_ctx("This executable has been built without SSL support, -s switch ignored", "dcc"));
#endif //!COMPILE_SSL_SUPPORT

	if(c->switches()->find('z', "zero-port"))
	{
		dcc_module_set_dcc_type(d, "SEND");
		d->setZeroPortRequestTag("nonempty"); // just to tag it
	}
	else
		dcc_module_set_dcc_type(d, "RSEND");
	d->triggerCreationEvent();
	g_pDccBroker->rsendManage(d);

	return true;
}

/*
	@doc: dcc.get
	@type:
		command
	@title:
		dcc.get
	@short:
		Requests a file
	@syntax:
		dcc.get [-s] [-t] <nickname> <filename> [filesize]
	@description:
		Sends a CTCP DCC GET to <nickname> requesting the file <filename>.
		The remote end should reply with a DCC SEND request CTCP.
		<filename> must not contain any leading path.
		If the -t switch is given, the message is a DCC TGET, expecting
		a TSEND reply.[br]
		If the -s switch is given, the message will be a DCC SGET, expecting
		a SSEND reply.[br]
		-t and -s can be combined together to obtain a [i]Turbo[/i]+[i]SSL[/i] extension transfer.[br]
		-s will work only if the KVIrc executable has been compiled with SSL support and
		the remote client supports it.[br]
	@examples:

*/

//#warning "ENCODE THE CTCP'S!!!!!!!"
//#warning "DOCS FOR dcc.get"

static bool dcc_kvs_cmd_get(KviKvsModuleCommandCall * c)
{
	QString szTarget, szFileName;
	kvs_uint_t uSize;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("target", KVS_PT_NONEMPTYSTRING, 0, szTarget)
	KVSM_PARAMETER("filename", KVS_PT_NONEMPTYSTRING, 0, szFileName)
	KVSM_PARAMETER("size", KVS_PT_UINT, KVS_PF_OPTIONAL, uSize)
	KVSM_PARAMETERS_END(c)

	KVSM_REQUIRE_CONNECTION(c)

	KviQString::cutToLast(szFileName, '/');

	if(szFileName.contains(' '))
	{
		szFileName.prepend('"');
		szFileName.append('"');
	}

	KviCString szDCC("GET");
#ifdef COMPILE_SSL_SUPPORT
	if(c->switches()->find('s', "ssl"))
		szDCC.prepend('S');
#else
	if(c->switches()->find('s', "ssl"))
		c->warning(__tr2qs_ctx("This executable has no SSL support, -s switch ignored", "dcc"));
#endif
	if(c->switches()->find('t', "tdcc"))
		szDCC.prepend('T');

	if(uSize == 0)
	{
		c->window()->console()->connection()->sendFmtData("PRIVMSG %s :%cDCC %s %s%c",
		    c->window()->console()->connection()->encodeText(szTarget).data(),
		    0x01,
		    c->window()->console()->connection()->encodeText(szDCC.ptr()).data(),
		    c->window()->console()->connection()->encodeText(szFileName).data(),
		    0x01);
	}
	else
	{
		c->window()->console()->connection()->sendFmtData("PRIVMSG %s :%cDCC %s %s %s%c",
		    c->window()->console()->connection()->encodeText(szTarget).data(),
		    0x01,
		    c->window()->console()->connection()->encodeText(szDCC.ptr()).data(),
		    c->window()->console()->connection()->encodeText(szFileName).data(),
		    c->window()->console()->connection()->encodeText(QString::number(uSize)).data(),
		    0x01);
	}

	return true;
}

// FIXME: SSL support for DCC VOICE ? (has a sense ?)

/*
	@doc: dcc.voice
	@type:
		command
	@title:
		dcc.voice
	@short:
		Starts a DCC Voice connection
	@syntax:
		dcc.voice [-g=<codec>] [-n] [-c] [-u] [-h=<sample_rate_in_hz>] [-m[=<boolean>]] [-i=<interface>] [-p=<port>] [-a=<fake address>] [-f=<fake port>] <nickname>
	@switches:
		!sw: -g=<codec> | --codec=<codec>
		Use the codec specified as parameter.
		Actually the supported codecs are [i]null[/i], [i]adpcm[/i] and [i]gsm[/i].
		!sw: -h=<rate> | --sample-rate=<rate>
		Use the sample rate specified by <rage>.
		Valid sample rates are 8000, 11025, 22050 and 44100 Hz.
		!sw: -m[=<boolean>] | --minimize[=<boolean>]
		If the -m switch is passed, the default boolCreateMinimizedDccSend option
		is overridden with the <boolean> parameter passed. So actually
		by passing -m=1 will create a minimized DCC SEND even
		if the [fnc]$option[/fnc](boolCreateMinimizedDccSend) returns false.[br]
		In the same way, by passing -m=0 you will create a non minimized DCC SEND.
		If no <boolean> value is specified, it defaults to 1.[br]
		!sw: -n | --no-ctcp
		Do [b]not[/b] send the CTCP request to the target user, you will have to do it manually,
		or the remote user will have to connect manually (for example by using dcc.recv -c).[br]
		!sw: -c | --connect
		Attempt to CONNECT to the remote host specified as <interface> and <port>,
		instead of listening (active connection instead of a passive one).
		In this case the -i and -p switches are mandatory.[br]
		The [b]c[/b] switch takes precedence over [b]n[/b] (In fact both should
		be mutually exclusive).[br]
		If the [b]c[/b] and [b]n[/b] switches are missing, this commands
		needs to be executed in a window that is bound to a connected
		IRC context (you need a third entity to accomplish the negotiation).[br]
		!sw: -i=<interface> | --ip=<interface>
		Bind the local listening socket to the specified <interface> (which is an IP address, IPv4 or IPv6).
		If this switch is [b]not[/b] specified, the socket is bound to the interface of
		the current IRC connection (if any) or to [i]127.0.0.1[/i].[br]
		You can also specify a local interface name to get the address from (this works only for IPv4 interfaces
		since IPv6 ones seems to be unsupported by the system ioctl() calls at the moment (for Linux at least)).[br]
		Here go some examples:[br]
		-i=215.243.12.12: this will bind to the IPv4 interface with the specified address.[br]
		-i=3ffe:1001::1: this will bind to the IPv6 interface with the specified address.[br]
		-i=ppp0: this will bind to the IPv4 address of the interface ppp0 (if supported by the underlying system).[br]
		The -i switch parameter may serve also as a target address when the -c switch is used.[br]
		!sw: -p=<port> | --port=<port>
		Bind the local listening socket to the specified <port>.
		If this switch is [b]not[/b] specified, the port will be a [i]random[/i] one chosen by the kernel.[br]
		!sw: -a=<fake address> | --fake-address=<fake address>
		Send the <fake address> as target for the remote client in the requesting CTCP message.
		If this switch is not given, the CTCP will contain the real IP address of the listening
		interface.[br]
		!sw: -f=<fake port> | --fake-port=<fake port>
		Send the <fake port> as target port for the remote client in the requesting CTCP message.
		If this switch is not given, the CTCP will contain the real port of the listening socket.
		[br][br]
		All these switches are meant to allow maximum flexibility of the
		DCC negotiation, earlier KVIrc releases had serious problems
		with firewalled and/or masqueraded machines. With the -a and -f switches
		you can work around it.[br]
		[br]
		!sw: -u | --unlimited
		If the 'u' switch is given, the connection attempt will
		never time out; this might be useful if you want to leave
		a listening socket for a friend of yours while you are sleeping
		and have the CTCP processing disabled. The 'u' switch works either
		in active and passive mode.[br]
	@description:
		Attempts a DCC Voice connection to <nickname>.[br]
		The -g option is used to select the GSM codec, available codecs are [i]gsm[/i], [i]adpcm[/i] and [i]null[/i].[br]
		The ADPCM codec is the one that was used in previous KVIrc versions, it provides a 1:4 compression rate
		and is designed for 8 KHz audio sampling rate (but will work also with other sampling rates).[br]
		The GSM codec offers 1:10 compression at the cost of some quality and CPU time. If you have a good
		CPU and plan to transmit voice only, use this codec.<br> The null codec
		offers no compression and may be used to transfer plain audio data over a fast connection (usually loopback
		connection or local networks). The null codec with 44100 sampling rate would provide CD quality audio
		streaming, but it is practically not usable (at the time I'm writing) since requires a
		monster bandwidth. If the -g switch is not present, the ADPCM codec is used by default (for compatibility reasons).[br]
 		The -h switch is used to select the sampling rate, if not given the sampling rate defaults to 8000 Hz.
		This switch accepts any value, but in fact the soundcards have limits on the values. Typically
		the lowest limit is 5 KHz and the upper limit is 44.1 KHz (but some soundcards support 96 KHz).
		It is also possible that the soundcard can't support a continuous range of frequencies and
		will select a discrete closest match instead.[br]
		The [i]commonly used[/i] sample rates are 8000, 11025, 22050 and 44100 Hz.[br]
		The remaining parameters are equivalent to the ones used in [cmd]dcc.send[/cmd], so please refer to that
		help page for the full discussion. This help page contains only a brief resume of these parameters.[br]
	@examples:
		[example]
			[comment]# Setup a DCC VOICE connection with Pragma (IRC user)[/comment]
			dcc.voice Pragma
			[comment]# Setup a DCC VOICE connection with Pragma and use the GSM codec[/comment]
			dcc.voice -g=gsm Pragma
			[comment]# Setup a DCC VOICE connection with Pragma, use the GSM codec and 22050 Hz sampling rate[/comment]
			dcc.voice -g=gsm -h=22050 Pragma
			[comment]# Setup a listening socket for a DCC VOICE connection on interface 127.0.0.1 and port 8088[/comment]
			dcc.voice -n -i=127.0.0.1 -p=8088 Pippo
			[comment]# Connect to the socket above[/comment]
			dcc.voice -c -i=127.0.0.1 -p=8088 Pluto
			[comment]# Same as above but use the NULL codec with 11025 Hz sampling rate[/comment]
			dcc.voice -g=null -h=11025 -n -i=127.0.0.1 -p=8088 Pippo
			[comment]# Connect to the socket above[/comment]
			dcc.voice -g=null -h=11025 -c -i=127.0.0.1 -p=8088 Pluto
		[/example]
*/

static bool dcc_kvs_cmd_voice(KviKvsModuleCommandCall * c)
{
	QString szTarget;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("target", KVS_PT_NONEMPTYSTRING, 0, szTarget)
	KVSM_PARAMETERS_END(c)

#ifdef COMPILE_DISABLE_DCC_VOICE
	c->warning(__tr2qs_ctx("DCC VOICE support not enabled at compilation time", "dcc"));
	return true;
#endif

	DccDescriptor * d = new DccDescriptor(c->window()->console());

	d->szNick = szTarget;                      // we always specify the nickname
	d->szUser = __tr2qs_ctx("unknown", "dcc"); // username is always unknown
	d->szHost = d->szUser;                     // host is always unknown
	d->iSampleRate = 8000;

	if(!dcc_kvs_parse_default_parameters(d, c))
		return false;

	if(KviKvsVariant * pSR = c->switches()->find('h', "sample-rate"))
	{
		kvs_int_t iSR;
		if(!pSR->asInteger(iSR))
		{
			c->warning(__tr2qs_ctx("Invalid sample rate specified, defaulting to 8000", "dcc"));
			d->iSampleRate = 8000;
		}
		else
		{
			d->iSampleRate = iSR;
		}
	}

	d->szCodec = "adpcm";

	if(KviKvsVariant * pCodec = c->switches()->find('g', "codec"))
	{
		QString szCodec;
		pCodec->asString(szCodec);

		if(!kvi_dcc_voice_is_valid_codec(szCodec.toUtf8().data()))
		{
			c->warning(__tr2qs_ctx("Invalid codec specified, defaulting to 'ADPCM'", "dcc"));
			d->szCodec = "adpcm";
		}
	}

	dcc_module_set_dcc_type(d, "VOICE");
	if(c->switches()->find('c', "connect"))
	{
		if(!(c->switches()->find('i', "ip") && c->switches()->find('p', "port")))
		{
			delete d;
			c->error(__tr2qs_ctx("-c requires -i and -p", "dcc"));
			return false;
		}
		d->szIp = d->szListenIp;
		d->szPort = d->szListenPort;
		d->szListenIp = "";   // useless
		d->szListenPort = ""; // useless
		d->bActive = true;

		d->triggerCreationEvent();
		g_pDccBroker->activeVoiceExecute(nullptr, d);
	}
	else
	{
		d->szIp = __tr2qs_ctx("unknown", "dcc");
		d->szPort = d->szIp;
		d->bActive = false;
		d->bSendRequest = !(c->switches()->find('n', "no-ctcp"));

		d->triggerCreationEvent();
		g_pDccBroker->passiveVoiceExecute(d);
	}

	return true;
}

static bool dcc_kvs_cmd_video(KviKvsModuleCommandCall * c)
{
	QString szTarget;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("target", KVS_PT_NONEMPTYSTRING, 0, szTarget)
	KVSM_PARAMETERS_END(c)

#ifdef COMPILE_DISABLE_DCC_VIDEO
	c->warning(__tr2qs_ctx("DCC VIDEO support not enabled at compilation time", "dcc"));
	return true;
#else

	DccDescriptor * d = new DccDescriptor(c->window()->console());

	d->szNick = szTarget;                      // we always specify the nickname
	d->szUser = __tr2qs_ctx("unknown", "dcc"); // username is always unknown
	d->szHost = d->szUser;                     // host is always unknown

	if(!dcc_kvs_parse_default_parameters(d, c))
		return false;

	d->szCodec = "sjpeg";

	if(KviKvsVariant * pCodec = c->switches()->find('g', "codec"))
	{
		QString szCodec;
		pCodec->asString(szCodec);

		if(!kvi_dcc_video_is_valid_codec(szCodec.toUtf8().data()))
		{
			c->warning(__tr2qs_ctx("Invalid codec specified", "dcc"));
			d->szCodec = "sjpeg";
		}
		else
		{
			d->szCodec = szCodec;
		}
	}

	dcc_module_set_dcc_type(d, "VIDEO");
	if(c->switches()->find('c', "connect"))
	{
		if(!(c->switches()->find('i', "ip") && c->switches()->find('p', "port")))
		{
			delete d;
			c->error(__tr2qs_ctx("-c requires -i and -p", "dcc"));
			return false;
		}
		d->szIp = d->szListenIp;
		d->szPort = d->szListenPort;
		d->szListenIp = "";   // useless
		d->szListenPort = ""; // useless
		d->bActive = true;

		d->triggerCreationEvent();
		g_pDccBroker->activeVideoExecute(0, d);
	}
	else
	{
		d->szIp = __tr2qs_ctx("unknown", "dcc");
		d->szPort = d->szIp;
		d->bActive = false;
		d->bSendRequest = !(c->switches()->find('n', "no-ctcp"));

		d->triggerCreationEvent();
		g_pDccBroker->passiveVideoExecute(d);
	}

	return true;
#endif
}

/*
static bool dcc_module_cmd_canvas(KviModule *m,KviCommand *c)
{
	ENTER_STACK_FRAME(c,"dcc_module_cmd_canvas");

	KviCString target;
	if(!g_pUserParser->parseCmdFinalPart(c,target))return false;

	if(target.isEmpty())return c->error(KviError_notEnoughParameters,"%s",__tr_ctx("Missing target nickname","dcc"));

	DccDescriptor * d = new DccDescriptor(c->window()->console());

	d->szNick       = target.ptr();              // we always specify the nickname
	d->szUser       = __tr2qs_ctx("unknown","dcc"); // username is always unknown
	d->szHost       = d->szUser;                 // host is always unknown
*/
/*
	d->bIsTdcc = c->hasSwitch('t');

	d->bOverrideMinimize = c->hasSwitch('m');

	if(d->bOverrideMinimize)
	{
		KviCString tmpVal;
		if(!(c->getSwitchValue('m',tmpVal)))d->bShowMinimized = false;
		else d->bShowMinimized = kvi_strEqualCI(tmpVal.ptr(),"1");
	}


	if(!d->console())
	{
		// We don't need a console with -c and -n, otherwise we need it
		if(!(c->hasSwitch('c') || c->hasSwitch('n')))return c->noIrcContext();
		else d->console() = g_pMainWindow->firstConsole();
	}

	KVI_ASSERT(d->console());

	if(!d->console()->isConnected())
	{
		// We don't need a connection with -c and -n, otherwise we need it
		if(!(c->hasSwitch('c') || c->hasSwitch('n')))return c->notConnectedToServer();
		else {
			// -c or -n, grab a local nick from somewhere
			d->szLocalNick  = KVI_OPTION_STRING(KviOption_stringNickname1).trimmed();
			if(d->szLocalNick.isEmpty())d->szLocalNick = KVI_DEFAULT_NICKNAME1;
			d->szLocalUser  = __tr("unknown"); // we can live without it
			d->szLocalHost  = d->szLocalUser;  // we can live without it
		}
	} else {
		// We know everything
		d->szLocalNick  = d->console()->currentNickName();
		d->szLocalUser  = d->console()->currentUserName();
		d->szLocalHost  = d->console()->currentLocalHostName();
	}


	if(c->hasSwitch('i'))
	{
		c->getSwitchValue('i',d->szListenIp);
		if(!(d->szListenIp.contains('.') || d->szListenIp.contains(':')))
		{
			// This will magically work with the same buffer as source and dst!
			if(!kvi_getInterfaceAddress(d->szListenIp.ptr(),d->szListenIp))
			{
				return c->error(KviError_invalidParameter,__tr("Can't get address of interface %s"),d->szListenIp.ptr());
			}
		}
	} else {
		if(d->console()->isConnected())
		{
			d->console()->socket()->getLocalHostIp(d->szListenIp,d->console()->isIPv6Connection());
		} else d->szListenIp = "127.0.0.1"; // huh ? :)
	}

	if(c->hasSwitch('p'))c->getSwitchValue('p',d->szListenPort);
	else d->szListenPort = "0"; // any port is ok

	if(c->hasSwitch('a'))c->getSwitchValue('a',d->szFakeIp);

	if(c->hasSwitch('f'))c->getSwitchValue('f',d->szFakePort);

	d->bDoTimeout = (!c->hasSwitch('u'));

*/
/*
	if(!dcc_module_parse_default_parameters(d,c))return false;
	dcc_module_set_dcc_type(d,"VOICE");

	if(c->hasSwitch('c'))
	{
		if(!(c->hasSwitch('i') && c->hasSwitch('p')))
		{
			delete d;
			return c->error(KviError_notEnoughParameters,__tr_ctx("-c requires -i and -p","dcc"));
		}
		d->szIp         = d->szListenIp;
		d->szPort       = d->szListenPort;
		d->szListenIp   = ""; // useless
		d->szListenPort = ""; // useless
		d->bActive      = true;

		d->triggerCreationEvent();
		g_pDccBroker->activeCanvasExecute(0,d);
	} else {
		d->szIp         = __tr2qs_ctx("unknown","dcc");
		d->szPort       = d->szIp;
		d->bActive      = false;
		d->bSendRequest = !c->hasSwitch('n');

		d->triggerCreationEvent();
		g_pDccBroker->passiveCanvasExecute(d);
	}

	return c->leaveStackFrame();
}
*/

/*
	@doc: dcc_connection
	@type:
		generic
	@title:
		DCC negotiation and connection
	@short:
		Overview of the DCC internals
	@keyterms:
		DCC without IRC
	@body:
		[big]What is DCC?[/big]
		'DCC' stands for Direct Client Connection, it is used to exchange data
		directly between two IRC clients (with no IRC server in the middle).[br]
		DCC itself is not a well-defined protocol, but rather a set of
		sub-protocols with (more or less) standardized rules.[br]
		Sub-protocols are also (historically) called [b]DCC types[/b]; this term often
		leads to confusion and it will become clear later.[br]
		Each sub-protocol has two main parts: The [b]DCC negotiation[/b] and the [b]DCC transfer[/b].[br]
		The [b]DCC negotiation[/b] part is used to request the [b]DCC transfer[/b] and define its necessary parameters,[br]
		while the [b]DCC transfer[/b] part is the real data transfer between clients.[br]
		The [b]DCC negotiation[/b] requires a third entity that routes the negotiation data between clients,
		this is usually an IRC server.
		[big]DCC Negotiation[/big]
		This part of the protocol is the most tricky and difficult one, and is different for almost every DCC sub-protocol.[br]
		The [i]constant[/i] scenario of the negotiation is more or less the following:[br]
		There are two IRC clients connected to the same IRC network and they want to exchange some data in
		a direct client connection.[br]
		Each client knows the other by nickname only (and eventually by the host displayed by the IRC server,
		but this cannot be trusted for several reasons), and can send text messages to each other by using the
		IRC network as data channel.[br]
		To initiate a direct client connection, one of the clients must start listening on some port (this is called [b]passive client[/b])
		and the other must connect to that port on the first client's machine (this is the [b]active client[/b]).[br]
		Both clients must agree on who is the passive and who is the active client. The active client must also
		know the passive client's IP address and port (in order to be able to contact it).[br]
		Finally, both clients must agree on the transfer type that has to be initiated.[br]
		The negotiation exchanges these information between clients by using IRC as channel and CTCP messages
		as encoding method.[br]
		An example will make things clearer:[br]
		DCC chat is the simplest (and most widely implemented) DCC sub-protocol:
		it is used to exchange <cr><lf> separated text data between clients.[br]
		Assume that you want to establish a DCC chat
		connection to 'Sarah' that is currently connected to your IRC network (so
		she/he is an IRC user just like you).
		All you have to do is type sth as [i]/dcc chat Sarah[/i] in your IRC client.
		The client will setup a listening socket on a random port chosen
		usually by the kernel of your OS. In this case YOU are the [b]passive client[/b], and Sarah is the active one.[br]
		Once the socket is ready to accept connections,
		your client will send a [doc:ctcp_handling]CTCP message[/doc] to Sarah using the IRC connection (and protocol) as channel:[br]
		PRIVMSG Sarah :<0x01>DCC CHAT chat <ip_address> <port><0x01>[br]
		where <ip_address> is the address of the listening socket and <port>
		is the port that it has been bound to (these information are obtained
		after the socket has been setup). Once Sarah has received the CTCP message,
		and agreed to connect, her (active) client will attempt to connect to the
		specified <ip_address> and <port> (e.g. to your listening socket).[br]
		Once the connection has been established, it continues using the
		specific CHAT transfer protocol.[br]
		Some IRC clients allow modifications of this procedure:[br]
		First of all, the port to listen on can be specified by the user
		and not by the kernel; this is useful when the passive client
		is behind a firewall that [i]shades[/i] some sets of ports.
		The IP address for the listening socket
		can be specified by the user as well (especially when the machine has more than one network interface).[br]
		A more challenging trick is to listen on a specified IP address and port
		and notify different ones to the remote user (e.g. <ip_address> and <port>
		parameters of the CTCP message are not the ones that the client is listening on).
		This is especially useful with [i]transparent proxy[/i] firewalls that
		often are not transparent enough to allow the DCC connections.
		(If you have one of these firewalls you know what I'm talking about,
		otherwise just read on). KVIrc allows to avoid the usage of a third entity
		for the protocol negotiation too.
		You can setup a listening socket on a specified port and IP address
		without notifying anyone of this.
		You can also manually connect to a specified port and IP address without
		having been notified of a DCC request.[br][br][br]
		Is everything clear?...I don't think so... my English is really bad...
		[big]DCC Transfer[/big]
		The DCC transfer part is different for every DCC sub-protocol, but
		it always happens over a direct client to client TCP connection.
		[big]DCC Sub-protocols[/big]
		There are two main standardized DCC sub-protocols that are widely implemented in IRC clients:
		[b]DCC chat[/b] and [b]DCC SEND[/b].[br]
		DCC chat is quite simple and the protocol is more or less completely defined.[br]
		DCC SEND is a *real mess*, the original definition was not very flexible
		so many IRC clients tried to enhance both the negotiation and the transfer, leading
		often to incompatible implementations. (I can remember the Turbo File Transfer implemented
		by VIrc, the Send-Ahead enhancement implemented in many clients, the RESUME facility...)[br]
		Many clients introduced new DCC sub-protocols with non-standard implementations,
		leading again to client incompatibility.[br]
		Some of the notable sub-protocols are DCC Voice, DCC Draw, DCC Whiteboard...
		[big]DCC Chat[/big]
		This is the simplest and most standardized DCC sub-protocol. Almost every IRC client implements it.[br]
		It is used to exchange lines of text between the two clients.[br]
		The negotiation is quite simple, we assume that [b]client A[/b] wants to establish a DCC chat connection to [b]client B[/b].
		[b]client A[/b] sets up a listening socket and retrieves its address (IP address and port).[br]
		Once the socket is ready [b]client A[/b] sends a CTCP request to B, in the following form:[br]
		[b]DCC CHAT chat <ipaddress> <port>[/b][br]
		Where <ipaddress> is a string representing an positive integer that is the A socket's IP address
		in network byte order, and where <port> is a string representing an positive integer that is the
		A socket's port.[br]
		The original purpose of the second [i]chat[/i] string in the CTCP request is quite obscure, it was probably
		introduced to have the <ipaddress> as second parameter, as in the DCC SEND sub-protocol.[br]
		[b]client B[/b] receives the CTCP, parses it, eventually asks the user for permission and connects
		to the specified IP address and port.
		The transfer protocol is quite simple, both clients can send text lines separated by <cr><lf> pairs.[br]
		Some clients use only <lf> as line terminator so the general idea is that one of <cr> <cr><lf> or <lf>
		can be used as line terminator.[br]
		As extension to the protocol, KVIrc allows <ipaddress> to be an IPv6 address in the standard hexadecimal
		notation, the connection will be made over the IPv6 protocol in this case (obviously if both clients
		support this feature).[br]
		(It is not clear why the original DCC specification used the unsigned int format instead of a
		standard string representation of the IP address... missing inet_aton() function on the target system?).[br]
		KVIrc adds the Secure Sockets Layer to the DCC chat protocol. In this case the negotiation string becomes:[br]
		[b]DCC SCHAT chat <ipaddress> <port>[/b][br]
		where [i]SCHAT[/i] stands for Secure CHAT.[br] The external protocol is exactly the same but is built on top of a Secure Sockets Layer
		implementation (specifically OpenSSL). The connection will be encrypted with a private key algorithm after
		a public key handshake.
		[big]DCC SEND[/big]
		DCC SEND is another standard sub-protocol. Most clients implement this as well, many have tried
		to enhance it.[br]
		The basic DCC SEND protocol allows transferring a file from the requesting client to the receiving client.[br]
		The requesting client (the one that sends the file) is always passive and the receiving client is always active.[br]
		This is a huge protocol limitation since firewalled clients are often unable to accept incoming connections.[br]
		The negotiation protocol is more complex than DCC chat; we assume that [b]client A[/b] wants to send the file F to [b]client B[/b].[br]
		[b]client A[/b] sets up a listening socket and retrieves its IP address and port.[br]
		[b]client A[/b] sends a CTCP request to [b]client B[/b] in the following form:[br]
		[b]DCC SEND <filename> <ipaddress> <port> <filesize>[/b][br]
		<ipaddress> and <port> have the same semantics as in the DCC chat sub-protocol.[br]
		<filename> is the name (without path!) of the file to be sent, and <filesize> is (yeah), the file size.[br]
		[b]client B[/b] receives the CTCP, parses it, eventually asks the user for confirmation and connects to the
		specified IP address and port; the transfer then begins.[br]
		[b]client A[/b] sends blocks of data (usually 1-2 KB) and at every block awaits confirmation from the [b]client B[/b],
		that when receiving a block should reply 4 bytes containing an positive number specifying the total size
		of the file received up to that moment.[br]
		The transmission closes when the last acknowledge is received by [b]client A[/b].[br]
		The acknowledges were meant to include some sort of coherency check in the transmission, but in fact
		no client can [i]recover[/i] from an acknowledge error/desync, all of them just close the connection declaring the
		transfer as failed (the situation is even worse in fact, often acknowledge errors aren't even detected!).[br]
		Since the packet-acknowledge round trip eats a lot of time, many clients included
		the [i]send-ahead[/i] feature; the [b]client A[/b] does [b]not[/b] wait for the acknowledge of the first packet before sending the second one.[br]
		The acknowledges are still sent, but just a reverse independent stream.[br] This makes the DCC SEND considerably faster.[br]
		Since the acknowledge stream has non-zero bandwidth usage, no client can recover from an acknowledge error and
		having them as an independent stream is more or less like having no acknowledges, the [i]Turbo[/i] ( :) ) extension has been added:
		[b]client B[/b] will send no acknowledges and will just close the connection when he has received all the expected data.[br]
		This makes the DCC SEND as fast as FTP transfers.[br]
		The [i]Turbo[/i] extension is specified during the negotiation phase, bu using TSEND as DCC message type (instead of SEND).[br]
		The [i]Turbo[/i] extension is not widely implemented.[br]
		Later implementations have added the support for resuming interrupted DCC SEND transfers:[br]
		[b]client A[/b] sets up the socket and sends the CTCP request as before.[br]
		If [b]client B[/b] discovers that the file has been partially received in a previous DCC SEND session it sends
		a resume request in the following form:[br]
		[b]DCC RESUME <filename> <port> <resume position>[/b][br]
		Where <port> is the <port> sent in the DCC SEND request and <resume position> is the position in the file
		from where the transfer should start.[br]
		[b]client A[/b] receives the request, parses it and eventually replies with:[br]
		[b]DCC ACCEPT <filename> <port> <resume position>[/b][br]
		[b]client B[/b] receives the ACCEPT message, connects to [b]client A[/b] and the transfer initiates as before.[br]
		The [i]Send-ahead[/i] and [i]Turbo[/i] extensions can obviously be used also in this case (But [b]T[/b] is [b]not[/b] prepended to the RESUME and ACCEPT messages).[br]
		The IPv6 extension can be used also in this sub-protocol, so <ipaddress> can be also an IPv6 address in hexadecimal notation.[br]
		KVIrc introduces the SSL extension also to DCC SEND. The protocol remains the same again but it is built on top of
		a Secure Sockets Layer implementation just like DCC chat.[br]
		With SSL the negotiation string becomes:[br]
		[b]DCC SSEND <filename> <ipaddress> <port> <filesize>[/b][br]
		where [i]SSEND[/i] stands for Secure SEND.[br]
		The [i]Turbo[/i] extension can be combined with the SSL extension too. In this case the second parameter
		of the negotiation string must be [i]TSSEND[/i] or [i]STSEND[/i].
		[big]DCC RECV[/big]
		DCC RECV is the counterpart of DCC SEND. This is a KVIrc extension and is not standard yet.[br]
		The purpose of this sub-protocol will not be immediately clear, but read on for an explanation.[br]
		It is used to request a file from another client; we assume that [b]client A[/b] knows that [b]client B[/b] has
		a specific file and is able/wants to send it.[br]
		[b]client A[/b] sets up a listening socket, retrieves its address and port and then
		sends a CTCP request to [b]client B[/b] in the following form:[br]
		[b]DCC RECV <filename> <ipaddress> <port> <resume position>[/b][br]
		where <filename> is the name of the requested file without path, <ipaddress> and <port> have the usual meaning and <resume position>
		is the position from that the transfer should start from.[br]
		<ipaddress> can be an IPv6 address as well.[br]
		[b]client B[/b] receives the CTCP message, parses it, looks for the file to send (in some unspecified way)
		and connects to the specified IP address and port. The transfer then begins just as in the DCC SEND, but in the inverse way:
		[b]client B[/b] sends blocks of data to [b]client A[/b] and [b]client B[/b] sends back acknowledges.[br]
		This sub-protocol is useful in transferring data from clients that are behind a firewall and are not able to accept
		incoming connections (this is not possible with a normal DCC SEND). In this case the client that receives
		the file is passive and the client that sends it is active (as opposite to DCC SEND).[br]
		The [i]Send ahead[/i] extension can be used also in this case and the [i]Turbo[/i] extension is activated by prepending a [b]T[/b] to the
		DCC message, [i]TRECV[/i] instead of [i]RECV[/i]. The SSL extension is activated by prepending an [b]S[/b] to the
		DCC message, [i]SRECV", [i]STRECV[/i] or [i]TSRECV[/i].[br]
		This sub-protocol has an implicit resume capability and thus has no need for RESUME and ACCEPT messages.[br]
		DCC RECV requires the initiating (passive) client to know that the file to be transferred is available on the B's side
		and probably also know the file size. This sub-protocol does not specify how this information is obtained, but it
		will become clear soon that it can be obtained either manually (User B can simply tell the info to User A),
		or automatically (as in the DCC RSEND sub-protocol (keep reading)).
		[big]DCC RSend[/big]
		DCC RSend stands for Reverse Send. This is a KVIrc extension to the SEND protocol to allow firewalled clients
		to send files.[br] In fact, this is a [i]half[/i] sub-protocol, since it defines only a part of the DCC negotiation;
		the transfer is defined by another sub-protocol (and specifically bu DCC RECV).[br]
		The requesting client (the one that sends the file) is active and the receiving client is passive.[br]
		Assume that [b]client A[/b] wants to send a file to [b]client B[/b] and that [b]client A[/b] cannot accept incoming connections.[br]
		[b]client A[/b] sends a CTCP request to [b]client B[/b] in the following form:[br]
		[b]DCC RSEND <filename> <filesize>[/b][br]
		[b]client B[/b] receives the request, parses it, eventually asks the user for confirmation, sets up a listening socket, retrieves
		its IP address and port and switches to the DCC RECV sub-protocol by effectively sending the following CTCP message:[br]
		[b]DCC RECV <filename> <ipaddress> <port> <resume position>[/b][br]
		The rest of the transfer is defined by the DCC RECV sub-protocol.[br]
		The [i]Turbo[/i] extension is again activated by prepending a [b]T[/b] to the RSEND string, so the initial CTCP will become:[br]
		[b]DCC TRSEND <filename> <filesize>[/b][br]
		The [i]SSL[/i] extension is also activated by prepending an [b]S[/b] to the RSEND string. It can be again combined
		with the [i]Turbo[/i] extension. The negotiation parameter becomes then [i]SRSEND[/i], [i]TSRSEND[/i] or [i]STRSEND[/i].[br]
		Easy, no? :)
		[big]DCC Get[/big]
		This is again a [i]half[/i] sub-protocol in fact since it defines only a part of the negotiation for file transfers.[br]
		It is also NON standard, since actually no client except KVIrc implements it (AFAIK).[br]
		DCC Get is used to request a file from a remote client. Assume that [b]client A[/b] wants to request a file from [b]client B[/b]
		(and assume that [b]client A[/b] knows that B has that file and wants to send it).[br]
		[b]client A[/b] sends a CTCP message to [b]client B[/b] in the following form:[br]
		[b]DCC GET <filename>[/b][br]
		Where <filename> is a name of a file without path.[br]
		[b]client B[/b] receives the message, parses it, looks for an association of the <filename> to a real filesystem file
		and starts one of the two DCC file transfer sub-protocols, DCC SEND or DCC RSEND.[br]
		[b]client B[/b] should prefer the DCC SEND method and choose DCC RSEND only if it is not able to accept incoming connections.[br]
		This sub-protocol can be used by firewalled clients that can't accept connections but still want to request a file
		from another client, this one can fail only if both clients are firewalled (in this case no DCC transfer is possible at all).[br]
		This sub-protocol also does not need to [i]magically[/i] know the file size, the size definition
		is found in the sub-protocol that the remote client will choose.[br]
		The association of <filename> with a real file on the B's machine is not explicitly defined by the sub-protocol;
		KVIrc uses an internal [i]file-offer[/i] table with a list of files that are available for download.[br]
		The [i]Turbo[/i] and [i]SSL[/i] extensions are activated as usual, [i]TGET[/i], [i]SGET[/i], [i]TSGET[/i] and [i]STGET[/i] are supported.
		[big]DCC File Transfer[/big]
		DCC SEND: Send a file, sender is passive, receiver is active (not good for firewalled senders)[br]
		DCC RECV: Receive a file, sender is active, receiver is passive (not good for firewalled receivers)[br]
		DCC RSEND: Send a file, sender is active, receiver is passive (not good for firewalled receivers)[br]
		DCC GGET: Receive a file, sender is passive if not firewalled, receiver active if sender not firewalled (will fail only if both are firewalled)[br]
		The [i]Turbo[/i] extension disables the stream of acknowledges and is activated by prepending the 'T' character to the DCC sub-protocol name[br]
		The [i]SSL[/i] extension causes a Secure Socket Layer to be used and is activated by prepending the 'S' character to the DCC sub-protocol name
		[big]DCC Voice[/big]
		DCC Voice is a KVIrc extension (there is a Windows client called VIrc that implements such
		a protocol, but it is incompatible with KVIrc).[br]
		DCC Voice allows audio level communication between two clients, the audio stream is compressed
		with a specified codec.[br]
		KVIrc currently supports the ADPCM (core support) and the GSM codec (if the libgsm is available on the target system).[br]
		[b]TODO: Finish the DCC Voice doc :)[/b]
		[big]More tricks[/big]
		KVIrc supports another [i]hack[/i] to the DCC negotiation, it recognizes [i]XDCC[/i] as
		a DCC negotiation CTCP parameter.[br]
		This can be used to circumvent limitations of some IRC clients (read mIRC) that will not allow
		you to send a /DCC GET since it is an unrecognized DCC type.[br]
		[i]XDCC[/i] has exactly the same meaning as [i]DCC[/i] (at least in KVIrc).[br]
*/

static DccDescriptor * dcc_kvs_find_dcc_descriptor(const kvs_uint_t & uId, KviKvsModuleRunTimeCall * c, bool bWarn = true)
{
	DccDescriptor * dcc = nullptr;
	if(uId == 0)
	{
		if(c->window()->inherits("DccWindow"))
		{
			dcc = ((DccWindow *)(c->window()))->descriptor();
		}
		if((!dcc) && bWarn)
			c->warning(__tr2qs_ctx("The current window has no associated DCC session", "dcc"));
		return dcc;
	}
	dcc = DccDescriptor::find(uId);
	if((!dcc) && bWarn)
		c->warning(__tr2qs_ctx("The specified parameter is not a valid DCC identifier", "dcc"));
	return dcc;
}

/*
	@doc: dcc.abort
	@type:
		command
	@title:
		dcc.abort
	@short:
		Aborts a DCC session
	@syntax:
		dcc.abort [-q] [dcc_id:uint]
	@description:
		Terminates the DCC specified by <dcc_id>.[br]
		If <dcc_id> is omitted then the DCC Session associated
		with the current window is assumed.[br]
		If <dcc_id> is not a valid DCC session identifier (or it is omitted
		and the current window has no associated DCC session) then
		this function doesn't abort anything and prints a warning unless the -q switch is used.[br]
		If <dcc_id> refers to a file transfer then it the transfer is simply
		terminated. If <dcc_id> refers to a DCC chat then the result
		is equivalent to closing the related window.[br]
		See the [module:dcc]dcc module[/module] documentation for more information.[br]
	@examples:
*/

static bool dcc_kvs_cmd_abort(KviKvsModuleCommandCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c, !c->switches()->find('q', "quiet"));

	if(dcc)
	{
		if(dcc->transfer())
			dcc->transfer()->abort();
		else if(dcc->window())
			dcc->window()->delayedClose();
	}

	return true;
}

/*
	@doc: dcc.setBandwidthLimit
	@type:
		command
	@title:
		dcc.setBandwidthLimit
	@short:
		Set the bandwidthlimit of a dcc.send session.
	@syntax:
		dcc.setBandwidthLimit [-q] [dcc_id:uint]
	@description:
		Terminates the DCC specified by <dcc_id>.[br]
		If <dcc_id> is omitted then the DCC Session associated
		with the current window is assumed.[br]
		If <dcc_id> is not a valid DCC session identifier (or it is omitted
		and the current window has no associated DCC session) then
		this function  prints a warning unless the -q switch is used.[br]
		If <dcc_id> does not refers to a file transfer a warning will be printing unless the -q switch is used.[br]
		See the [module:dcc]dcc module[/module] documentation for more information.[br]
	@examples:
*/
static bool dcc_kvs_cmd_setBandwidthLimit(KviKvsModuleCommandCall * c)
{
	kvs_uint_t uDccId, uVal;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("limit_value", KVS_PT_UINT, 0, uVal)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c, !c->switches()->find('q', "quiet"));
	if(dcc)
	{
		if(dcc->transfer())
			dcc->transfer()->setBandwidthLimit(uVal);
		else if(!c->switches()->find('q', "quiet"))
			c->warning(__tr2qs_ctx("This DCC session is not a DCC transfer session", "dcc"));
	}
	return true;
}

/*
	@doc: dcc.protocol
	@type:
		function
	@title:
		$dcc.protocol
	@short:
		Returns the protocol of the specified DCC session
	@syntax:
		<string> $dcc.protocol
		<string> $dcc.protocol(<dcc_id:uint>)
	@description:
		Returns the string describing the protocol of the
		DCC specified by <dcc_id>.[br]
		If <dcc_id> is omitted then the DCC Session associated
		with the current window is assumed.[br]
		If <dcc_id> is not a valid DCC session identifier (or it is omitted
		and the current window has no associated DCC session) then
		this function prints a warning and returns an empty string.[br]
		See the [module:dcc]dcc module[/module] documentation for more information.[br]
*/

static bool dcc_kvs_fnc_protocol(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c);

	if(dcc)
		c->returnValue()->setString(dcc->protocol());
	return true;
}

/*
	@doc: dcc.connectionType
	@type:
		function
	@title:
		$dcc.connectionType
	@short:
		Returns the connection type of the specified DCC session
	@syntax:
		<string> $dcc.connectionType
		<string> $dcc.connectionType(<dcc_id:uint>)
	@description:
		Returns the connection type of the specified DCC session.[br]
		Returns the string [i]ACTIVE[/i] for active DCC connections
		and the string [i]PASSIVE[/i] for passive DCC connections.
		If <dcc_id> is omitted then the DCC Session associated
		with the current window is assumed.[br]
		If <dcc_id> is not a valid DCC session identifier (or it is omitted
		and the current window has no associated DCC session) then
		this function prints a warning and returns an empty string.[br]
		See the [module:dcc]dcc module[/module] documentation for more information.[br]
*/

static bool dcc_kvs_fnc_connectionType(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c);

	if(dcc)
		c->returnValue()->setString(dcc->isActive() ? "ACTIVE" : "PASSIVE");
	return true;
}

/*
	@doc: dcc.isFileTransfer
	@type:
		function
	@title:
		$dcc.isFileTransfer
	@short:
		Checks if a DCC is a file transfer
	@syntax:
		<boolean> $dcc.isFileTransfer
		<boolean> $dcc.isFileTransfer(<dcc_id:uint>)
	@description:
		Returns [b]1[/b] if the specified DCC
		is a file transfer and [b]0[/b] otherwise.[br]
		If <dcc_id> is omitted then the DCC Session associated
		with the current window is assumed.[br]
		If <dcc_id> is not a valid DCC session identifier (or it is omitted
		and the current window has no associated DCC session) then
		this and returns 0.[br]
		See the [module:dcc]dcc module[/module] documentation for more information.[br]
*/

static bool dcc_kvs_fnc_isFileTransfer(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c, false);

	if(dcc)
		c->returnValue()->setBoolean(dcc->isFileTransfer());
	return true;
}

/*
	@doc: dcc.isFileUpload
	@type:
		function
	@title:
		$dcc.isFileUpload
	@short:
		Checks if a DCC is an upload file transfer
	@syntax:
		<boolean> $dcc.isFileUpload
		<boolean> $dcc.isFileUpload(<dcc_id:uint>)
	@description:
		Returns [b]1[/b] if the specified DCC
		is an upload file transfer and [b]0[/b] otherwise.[br]
		If <dcc_id> is omitted then the DCC Session associated
		with the current window is assumed.[br]
		If <dcc_id> is not a valid DCC session identifier (or it is omitted
		and the current window has no associated DCC session) then
		this function prints a warning and returns 0.[br]
		See the [module:dcc]dcc module[/module] documentation for more information.[br]
*/

static bool dcc_kvs_fnc_isFileUpload(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c);

	if(dcc)
		c->returnValue()->setBoolean(dcc->isFileUpload());
	return true;
}

/*
	@doc: dcc.isFileDownload
	@type:
		function
	@title:
		$dcc.isFileDownload
	@short:
		Checks if a DCC is a download file transfer
	@syntax:
		<boolean> $dcc.isFileDownload
		<boolean> $dcc.isFileDownload(<dcc_id:uint>)
	@description:
		Returns [b]1[/b] if the specified DCC
		is a download file transfer and [b]0[/b] otherwise.[br]
		If <dcc_id> is omitted then the DCC Session associated
		with the current window is assumed.[br]
		If <dcc_id> is not a valid DCC session identifier (or it is omitted
		and the current window has no associated DCC session) then
		this function prints a warning and returns 0.[br]
		See the [module:dcc]dcc module[/module] documentation for more information.[br]
*/

static bool dcc_kvs_fnc_isFileDownload(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c);

	if(dcc)
		c->returnValue()->setBoolean(dcc->isFileDownload());
	return true;
}

/*
	@doc: dcc.localNick
	@type:
		function
	@title:
		$dcc.localNick
	@short:
		Returns the local nickname associated with the specified DCC session
	@syntax:
		<string> $dcc.localNick
		<string> $dcc.localNick(<dcc_id:uint>)
	@description:
		Returns the local nickname associated with the specified DCC session.[br]
		If <dcc_id> is omitted then the DCC Session associated
		with the current window is assumed.[br]
		If <dcc_id> is not a valid DCC session identifier (or it is omitted
		and the current window has no associated DCC session) then
		this function prints a warning and returns an empty string.[br]
		See the [module:dcc]dcc module[/module] documentation for more information.[br]
*/

static bool dcc_kvs_fnc_localNick(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c);

	if(dcc)
		c->returnValue()->setString(dcc->localNick());
	return true;
}

/*
	@doc: dcc.localUser
	@type:
		function
	@title:
		$dcc.localUser
	@short:
		Returns the local username associated with the specified DCC session
	@syntax:
		<string> $dcc.localUser
		<string> $dcc.localUser(<dcc_id:uint>)
	@description:
		Returns the local username associated with the specified DCC session.[br]
		If <dcc_id> is omitted then the DCC Session associated
		with the current window is assumed.[br]
		If <dcc_id> is not a valid DCC session identifier (or it is omitted
		and the current window has no associated DCC session) then
		this function prints a warning and returns an empty string.[br]
		See the [module:dcc]dcc module[/module] documentation for more information.[br]
*/

static bool dcc_kvs_fnc_localUser(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c);

	if(dcc)
		c->returnValue()->setString(dcc->localUser());
	return true;
}

/*
	@doc: dcc.localHost
	@type:
		function
	@title:
		$dcc.localHost
	@short:
		Returns the local hostname associated with the specified DCC session
	@syntax:
		<string> $dcc.localHost
		<string> $dcc.localHost(<dcc_id:uint>)
	@description:
		Returns the local hostname associated with the specified DCC session.[br]
		If <dcc_id> is omitted then the DCC Session associated
		with the current window is assumed.[br]
		If <dcc_id> is not a valid DCC session identifier (or it is omitted
		and the current window has no associated DCC session) then
		this function prints a warning and returns an empty string.[br]
		See the [module:dcc]dcc module[/module] documentation for more information.[br]
*/

static bool dcc_kvs_fnc_localHost(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c);

	if(dcc)
		c->returnValue()->setString(dcc->localHost());
	return true;
}

/*
	@doc: dcc.localIp
	@type:
		function
	@title:
		$dcc.localIp
	@short:
		Returns the local IP address associated with the specified DCC session
	@syntax:
		<string> $dcc.localIp
		<string> $dcc.localIp(<dcc_id:uint>)
	@description:
		Returns the local IP address associated with the specified DCC session.[br]
		If <dcc_id> is omitted then the DCC Session associated
		with the current window is assumed.[br]
		If <dcc_id> is not a valid DCC session identifier (or it is omitted
		and the current window has no associated DCC session) then
		this function prints a warning and returns an empty string.[br]
		See the [module:dcc]dcc module[/module] documentation for more information.[br]
*/

static bool dcc_kvs_fnc_localIp(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c);

	if(dcc)
		c->returnValue()->setString(dcc->localIp());
	return true;
}

/*
	@doc: dcc.localPort
	@type:
		function
	@title:
		$dcc.localPort
	@short:
		Returns the local port associated with the specified DCC session
	@syntax:
		<integer> $dcc.localPort
		<integer> $dcc.localPort(<dcc_id:uint>)
	@description:
		Returns the local port associated with the specified DCC session.[br]
		If <dcc_id> is omitted then the DCC Session associated
		with the current window is assumed.[br]
		If <dcc_id> is not a valid DCC session identifier then
		this function prints a warning and returns an empty string.[br]
		See the [module:dcc]dcc module[/module] documentation for more information.[br]
*/

static bool dcc_kvs_fnc_localPort(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c);

	if(dcc)
		c->returnValue()->setString(dcc->localPort());
	return true;
}

/*
	@doc: dcc.localFileName
	@type:
		function
	@title:
		$dcc.localFileName
	@short:
		Returns the local file name associated with the specified DCC session
	@syntax:
		<string> $dcc.localFileName(<dcc_id:uint>)
	@description:
		Returns the local file name associated with the specified DCC session.[br]
		If <dcc_id> does not identify a file transfer DCC then this
		function returns an empty string.
		If <dcc_id> is not a valid DCC identifier
		then this function prints a warning and returns an empty string.
*/

static bool dcc_kvs_fnc_localFileName(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c);

	if(dcc)
		c->returnValue()->setString(dcc->localFileName());
	return true;
}

/*
	@doc: dcc.localFileSize
	@type:
		function
	@title:
		$dcc.localFileSize
	@short:
		Returns the local file size associated with the specified DCC session
	@syntax:
		<integer> $dcc.localFileSize(<dcc_id:uint>)
	@description:
		Returns the local file size associated with the specified DCC session.[br]
		If <dcc_id> does not identify a file transfer DCC then this
		function returns '0'[br]
		If <dcc_id> is not a valid DCC identifier
		then this function prints a warning and returns '0'[br]
		In upload transfers the local file size represents the
		total size of the file to be transferred. In download transfers
		the local file size is non zero only if the transfer has to resume
		a file already existing on the local disk and it represents the
		size of that file (and thus the offset that the transfer started on).
*/

static bool dcc_kvs_fnc_localFileSize(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c);

	if(dcc)
		c->returnValue()->setString(dcc->localFileSize().isEmpty() ? QString("0") : dcc->localFileSize());
	return true;
}

/*
	@doc: dcc.remoteNick
	@type:
		function
	@title:
		$dcc.remoteNick
	@short:
		Returns the remote nickname associated with the specified DCC session
	@syntax:
		<string> $dcc.remoteNick
		<string> $dcc.remoteNick(<dcc_id:uint>)
	@description:
		Returns the remote nickname associated with the specified DCC session.[br]
		If <dcc_id> is omitted then the DCC Session associated
		with the current window is assumed.[br]
		If <dcc_id> is not a valid DCC session identifier (or it is omitted
		and the current window has no associated DCC session) then
		this function prints a warning and returns an empty string.[br]
		See the [module:dcc]dcc module[/module] documentation for more information.[br]
*/

static bool dcc_kvs_fnc_remoteNick(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c);

	if(dcc)
		c->returnValue()->setString(dcc->remoteNick());
	return true;
}

/*
	@doc: dcc.remoteUser
	@type:
		function
	@title:
		$dcc.remoteUser
	@short:
		Returns the remote username associated with the specified DCC session
	@syntax:
		<string> $dcc.remoteUser
		<string> $dcc.remoteUser(<dcc_id:uint>)
	@description:
		Returns the remote username associated with the specified DCC session.[br]
		If <dcc_id> is omitted then the DCC Session associated
		with the current window is assumed.[br]
		If <dcc_id> is not a valid DCC session identifier (or it is omitted
		and the current window has no associated DCC session) then
		this function prints a warning and returns an empty string.[br]
		See the [module:dcc]dcc module[/module] documentation for more information.[br]
*/

static bool dcc_kvs_fnc_remoteUser(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c);

	if(dcc)
		c->returnValue()->setString(dcc->remoteUser());
	return true;
}

/*
	@doc: dcc.remoteHost
	@type:
		function
	@title:
		$dcc.remoteHost
	@short:
		Returns the remote hostname associated with the specified DCC session
	@syntax:
		<string> $dcc.remoteHost
		<string> $dcc.remoteHost(<dcc_id:uint>)
	@description:
		Returns the remote hostname associated with the specified DCC session.[br]
		If <dcc_id> is omitted then the DCC Session associated
		with the current window is assumed.[br]
		If <dcc_id> is not a valid DCC session identifier (or it is omitted
		and the current window has no associated DCC session) then
		this function prints a warning and returns an empty string.[br]
		See the [module:dcc]dcc module[/module] documentation for more information.[br]
*/

static bool dcc_kvs_fnc_remoteHost(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c);

	if(dcc)
		c->returnValue()->setString(dcc->remoteHost());
	return true;
}

/*
	@doc: dcc.remoteIp
	@type:
		function
	@title:
		$dcc.remoteIp
	@short:
		Returns the remote IP address associated with the specified DCC session
	@syntax:
		<string> $dcc.remoteIp
		<string> $dcc.remoteIp(<dcc_id:uint>)
	@description:
		Returns the remote IP address associated with the specified DCC session.[br]
		If <dcc_id> is omitted then the DCC Session associated
		with the current window is assumed.[br]
		If <dcc_id> is not a valid DCC session identifier (or it is omitted
		and the current window has no associated DCC session) then
		this function prints a warning and returns an empty string.[br]
		See the [module:dcc]dcc module[/module] documentation for more information.[br]
*/

static bool dcc_kvs_fnc_remoteIp(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c);

	if(dcc)
		c->returnValue()->setString(dcc->remoteIp());
	return true;
}

/*
	@doc: dcc.remotePort
	@type:
		function
	@title:
		$dcc.remotePort
	@short:
		Returns the remote port associated with the specified DCC session
	@syntax:
		<integer> $dcc.remotePort
		<integer> $dcc.remotePort(<dcc_id:uint>)
	@description:
		Returns the remote port associated with the specified DCC session.[br]
		If <dcc_id> is omitted then the DCC Session associated
		with the current window is assumed.[br]
		If <dcc_id> is not a valid DCC session identifier (or it is omitted
		and the current window has no associated DCC session) then
		this function prints a warning and returns an empty string.[br]
		See the [module:dcc]dcc module[/module] documentation for more information.[br]
*/

static bool dcc_kvs_fnc_remotePort(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c);

	if(dcc)
		c->returnValue()->setString(dcc->remotePort());
	return true;
}

/*
	@doc: dcc.remoteFileName
	@type:
		function
	@title:
		$dcc.remoteFileName
	@short:
		Returns the remote file name associated with the specified DCC session
	@syntax:
		<string> $dcc.remoteFileName(<dcc_id:uint>)
	@description:
		Returns the remote file name associated with the specified DCC session.[br]
		If <dcc_id> does not identify a file transfer DCC then this
		function returns an empty string.
		If <dcc_id> is not a valid DCC identifier
		then this function prints a warning and returns an empty string.
*/

static bool dcc_kvs_fnc_remoteFileName(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c);

	if(dcc)
		c->returnValue()->setString(dcc->remoteFileName());
	return true;
}

/*
	@doc: dcc.remoteFileSize
	@type:
		function
	@title:
		$dcc.remoteFileSize
	@short:
		Returns the remote file size associated with the specified DCC session
	@syntax:
		<integer> $dcc.remoteFileSize(<dcc_id:uint>)
	@description:
		Returns the remote file size associated with the specified DCC session.[br]
		If <dcc_id> does not identify a file transfer DCC then this
		function returns '0'[br]
		If <dcc_id> is not a valid DCC identifier
		then this function prints a warning and returns '0'[br]
		In download transfers the remote file size represents the
		total size of the file to be transferred (advertised by the remote end).[br]
		In upload transfers the remote file size is non zero only if the
		remote user has issued a resume request and it represents the requested offset
		in bytes from which the transfer has started.
*/

static bool dcc_kvs_fnc_remoteFileSize(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c);

	if(dcc)
		c->returnValue()->setString(dcc->remoteFileSize().isEmpty() ? QString("0") : dcc->remoteFileSize());
	return true;
}

/*
	@doc: dcc.ircContext
	@type:
		function
	@title:
		$dcc.ircContext
	@short:
		Returns the ircContext from which this DCC has originated
	@syntax:
		<integer> $dcc.ircContext
		<integer> $dcc.ircContext(<dcc_id:uint>)
	@description:
		Returns the identifier of the IRC context from which
		the specified DCC session has been originated.[br]
		When the DCC is not originated from an IRC context
		then this function returns '0' : an invalid IRC context id.
		If <dcc_id> is omitted then the DCC Session associated
		with the current window is assumed.[br]
		If <dcc_id> is not a valid DCC session identifier (or it is omitted
		and the current window has no associated DCC session) then
		this function prints a warning and returns 0.[br]
		See the [module:dcc]dcc module[/module] documentation for more information.[br]
*/

static bool dcc_kvs_fnc_ircContext(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c);

	if(dcc)
	{
		KviWindow * pEventWindow = dcc->console();
		if(pEventWindow && (g_pApp->windowExists(pEventWindow)))
		{
			c->returnValue()->setInteger(dcc->console()->context()->id());
		}
		else
		{
			c->error(__tr2qs_ctx("The IRC context that originated the DCC doesn't exist anymore.", "dcc"));
			return false;
		}
	}
	return true;
}

/*
	@doc: dcc.transferStatus
	@type:
		function
	@title:
		$dcc.transferStatus
	@short:
		Returns the current status of a DCC file transfer
	@syntax:
		<string> $dcc.transferStatus
		<string> $dcc.transferStatus(<dcc_id:uint>)
	@description:
		Returns the status in the specified DCC session.[br]
		The status is one of the strings [i]connecting", [i]transferring[/i], [i]success[/i] and [i]failure[/i].
		[i]success[/i] and [i]failure[/i] are reported when the transfer is terminated.
		If <dcc_id> is omitted then the DCC session associated
		with the current window is assumed.[br]
		If <dcc_id> is not a valid DCC session identifier (or it is omitted
		and the current window has no associated DCC session) then
		this function prints a warning and returns an empty string.[br]
		If the DCC session does not refer to a file transfer then
		this function returns "".[br]
		See the [module:dcc]dcc module[/module] documentation for more information.[br]
*/

static bool dcc_kvs_fnc_transferStatus(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c);

	if(dcc)
	{
		if(dcc->transfer())
		{
			QString tmp;
			dcc->transfer()->fillStatusString(tmp);
			c->returnValue()->setString(tmp);
		}
	}
	return true;
}

/*
	@doc: dcc.transferredBytes
	@type:
		function
	@title:
		$dcc.transferredBytes
	@short:
		Returns the number of transferred bytes in a DCC file transfer
	@syntax:
		<integer> $dcc.transferredBytes
		<integer> $dcc.transferredBytes(<dcc_id:uint>)
	@description:
		Returns the number of transferred bytes in the specified DCC session.[br]
		If <dcc_id> is omitted then the DCC Session associated
		with the current window is assumed.[br]
		If <dcc_id> is not a valid DCC session identifier (or it is omitted
		and the current window has no associated DCC session) then
		this function prints a warning and returns an empty string.[br]
		If the DCC session does not refer to a file transfer then
		this function returns [b]0[/b].[br]
		See the [module:dcc]dcc module[/module] documentation for more information.[br]
*/

static bool dcc_kvs_fnc_transferredBytes(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c);

	if(dcc)
	{
		if(dcc->transfer())
		{
			c->returnValue()->setInteger(dcc->transfer()->transferredBytes());
		}
		else
		{
			c->returnValue()->setInteger(0);
		}
	}
	return true;
}

/*
	@doc: dcc.averageSpeed
	@type:
		function
	@title:
		$dcc.averageSpeed
	@short:
		Returns the average speed of a DCC file transfer
	@syntax:
		$dcc.averageSpeed
		$dcc.averageSpeed(<dcc_id>)
	@description:
		Returns the average speed (in bytes/sec) of the specified DCC session.[br]
		If <dcc_id> is omitted then the DCC Session associated
		with the current window is assumed.[br]
		If <dcc_id> is not a valid DCC session identifier (or it is omitted
		and the current window has no associated DCC session) then
		this function prints a warning and returns an empty string.[br]
		If the DCC session does not refer to a file transfer then
		this function returns [b]0[/b].[br]
		See the [module:dcc]dcc module[/module] documentation for more information.[br]
*/

static bool dcc_kvs_fnc_averageSpeed(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c);

	if(dcc)
	{
		if(dcc->transfer())
		{
			c->returnValue()->setInteger(dcc->transfer()->averageSpeed());
		}
		else
		{
			c->returnValue()->setInteger(0);
		}
	}
	return true;
}

/*
	@doc: dcc.currentSpeed
	@type:
		function
	@title:
		$dcc.currentSpeed
	@short:
		Returns the current speed of a DCC file transfer
	@syntax:
		$dcc.currentSpeed
		$dcc.currentSpeed(<dcc_id>)
	@description:
                Returns the current speed (in bytes/sec) of the specified DCC session.[br]
                If <dcc_id> is omitted then the DCC Session associated
                with the current window is assumed.[br]
                If <dcc_id> is not a valid DCC session identifier (or it is omitted
                and the current window has no associated DCC session) then
                this function prints a warning and returns an empty string.[br]
                If the DCC session does not refer to a file transfer then
                this function returns [b]0[/b].[br]
                See the [module:dcc]dcc module[/module] documentation for more information.[br]
*/

static bool dcc_kvs_fnc_currentSpeed(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c);

	if(dcc)
	{
		if(dcc->transfer())
		{
			c->returnValue()->setInteger(dcc->transfer()->instantSpeed());
		}
		else
		{
			c->returnValue()->setInteger(0);
		}
	}
	return true;
}

/*
	@doc: dcc.session
	@type:
		function
	@title:
		$dcc.session
	@short:
		Returns the DCC session identifier associated with a window
	@syntax:
		<uint> $dcc.session
		<uint> $dcc.session(<window_id>)
	@description:
		Returns the DCC session identifier associated with the DCC window specified
		by <window_id>. If <window_id> is omitted then the DCC session identifier
		associated with the current window is returned. If the specified window
		has no associated DCC session then a warning is printed and [b]0[/b] is returned.[br]
*/

static bool dcc_kvs_fnc_session(KviKvsModuleFunctionCall * c)
{
	QString szWinId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window_id", KVS_PT_STRING, KVS_PF_OPTIONAL, szWinId)
	KVSM_PARAMETERS_END(c)

	DccDescriptor * dcc = nullptr;
	if(szWinId.isEmpty())
	{
		if(c->window()->inherits("DccWindow"))
			dcc = ((DccWindow *)(c->window()))->descriptor();
		if(!dcc)
		{
			c->warning(__tr2qs_ctx("The current window has no associated DCC session", "dcc"));
			c->returnValue()->setInteger(0);
		}
		else
		{
			c->returnValue()->setInteger(dcc->id());
		}
		return true;
	}

	KviWindow * pWnd = g_pApp->findWindow(szWinId);
	if(!pWnd)
	{
		c->warning(__tr2qs_ctx("The specified window identifier is not valid", "dcc"));
		c->returnValue()->setInteger(0);
		return true;
	}

	if(pWnd->inherits("DccWindow"))
		dcc = ((DccWindow *)pWnd)->descriptor();
	if(!dcc)
	{
		c->warning(__tr2qs_ctx("The current window has no associated DCC session", "dcc"));
		c->returnValue()->setInteger(0);
	}
	else
	{
		c->returnValue()->setInteger(dcc->id());
	}
	return true;
}

/*
	@doc: dcc.sessionList
	@type:
		function
	@title:
		$dcc.sessionList
	@short:
		List the existing DCC session identifiers
	@syntax:
		<array> $dcc.sessionList
		<array> $dcc.sessionList(<filter:string>)
	@description:
		The first form returns an array with all the currently existing DCC session
		identifiers. The second form returns an array with the session types specified
		in <filter> which may be a combination of the flags 'u' (for file upload),
		'd' (for file download) and 'c' (for DCC chat). To select all the file transfers
		please use the combination 'ud'.[br]
		See the [module:dcc]dcc module[/module] documentation for more information.[br]
*/

static bool dcc_kvs_fnc_sessionList(KviKvsModuleFunctionCall * c)
{
	QString szFlags;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("filter", KVS_PT_STRING, KVS_PF_OPTIONAL, szFlags)
	KVSM_PARAMETERS_END(c)

	KviKvsArray * a = new KviKvsArray();
	c->returnValue()->setArray(a);

	KviPointerHashTable<int, DccDescriptor> * dict = DccDescriptor::descriptorDict();
	if(!dict)
		return true;

	KviPointerHashTableIterator<int, DccDescriptor> it(*dict);

	int idx = 0;

	if(szFlags.isEmpty())
	{
		// all
		while(DccDescriptor * dcc = it.current())
		{
			a->set(idx++, new KviKvsVariant((kvs_int_t)(dcc->id())));
			++it;
		}
	}
	else
	{
		bool bWantFileUploads = szFlags.contains('u', Qt::CaseInsensitive);
		bool bWantFileDownloads = szFlags.contains('d', Qt::CaseInsensitive);
		bool bWantChats = szFlags.contains('c', Qt::CaseInsensitive);

		while(DccDescriptor * dcc = it.current())
		{
			if((dcc->isFileUpload() && bWantFileUploads) || (dcc->isFileDownload() && bWantFileDownloads) || (dcc->isDccChat() && bWantChats))
			{
				a->set(idx++, new KviKvsVariant((kvs_int_t)(dcc->id())));
			}
			++it;
		}
	}

	return true;
}

/*
	@doc: dcc.getSSLCertInfo
	@type:
		function
	@title:
		$dcc.getSSLCertInfo
	@short:
		Returns the requested information about certificates used in an SSL enabled DCC session
	@syntax:
		$dcc.getSSLCertInfo(<query:string>[,<type:string='remote'>[,<dcc_id:integer>[,<param1:string>]]])
	@description:
		Returns the requested information about certificates used in an SSL enabled DCC session.[br]
		The second <type> parameter can be [i]local[/i] or [i]remote[/i], and refers to the certificate you want
		to query the information from; if omitted, it defaults to [i]remote[/i].[br]
		If <dcc_id> is omitted then the DCC Session associated with the current window is assumed.[br]
		If <dcc_id> is not a valid DCC session identifier (or it is omitted and the current window
		has no associated DCC session) then this function prints a warning and returns an empty string.[br]
		If the DCC session is not using SSL then this function returns an empty string.[br]
		Some queries can accept an optional parameter <param1>.[br]
		Available query strings are:
		[ul]
		[li]signatureType[/li]
		[li]signatureContents[/li]
		[li]subjectCountry[/li]
		[li]subjectStateOrProvince[/li]
		[li]subjectLocality[/li]
		[li]subjectOrganization[/li]
		[li]subjectOrganizationalUnit[/li]
		[li]subjectCommonName[/li]
		[li]issuerCountry[/li]
		[li]issuerStateOrProvince[/li]
		[li]issuerLocality[/li]
		[li]issuerOrganization[/li]
		[li]issuerOrganizationalUnit[/li]
		[li]issuerCommonName[/li]
		[li]publicKeyBits[/li]
		[li]publicKeyType[/li]
		[li]serialNumber[/li]
		[li]pemBase64[/li]
		[li]version[/li]
		[li]fingerprintIsValid[/li]
		[li]fingerprintDigestId[/li]
		[li]fingerprintDigestStr[/li]
		[li]fingerprintContents * accepts parameter interpreted as [i]digest name[/i][/li]
		[/ul]
	@examples:
		[example]
			[comment]# get a sha256 fingerprint of remote peer's certificate[/comment]
			$dcc.getSSLCertInfo(fingerprintContents,remote,$dcc.session,sha256)
		[/example]
	@seealso:
		[fnc]$certificate[/fnc]
		[fnc]$str.evpSign[/fnc]
		[fnc]$str.evpVerify[/fnc]
		[module:dcc]dcc module[/module]
*/

static bool dcc_kvs_fnc_getSSLCertInfo(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uDccId;
	QString szQuery;
	QString szType;
	QString szParam1;
	bool bRemote = true;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("query", KVS_PT_STRING, 0, szQuery)
	KVSM_PARAMETER("type", KVS_PT_STRING, KVS_PF_OPTIONAL, szType)
	KVSM_PARAMETER("dcc_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uDccId)
	KVSM_PARAMETER("param1", KVS_PT_STRING, KVS_PF_OPTIONAL, szParam1)
	KVSM_PARAMETERS_END(c)

#ifndef COMPILE_SSL_SUPPORT
	c->warning(__tr2qs_ctx("This executable was built without SSL support", "dcc"));
	return true;
#else

	if(szType.compare("local") == 0)
	{
		bRemote = false;
	}
	else
	{
		// already defaults to true, we only catch the error condition
		if(szType.compare("remote") != 0)
		{
			c->warning(__tr2qs_ctx("You specified a bad string for the parameter \"type\"", "dcc"));
			c->returnValue()->setString("");
			return true;
		}
	}
	DccDescriptor * dcc = dcc_kvs_find_dcc_descriptor(uDccId, c);

	if(dcc)
	{
		if(!dcc->isSSL())
		{
			c->warning(__tr2qs_ctx("Unable to get SSL information: DCC session is not using SSL", "dcc"));
			c->returnValue()->setString("");
			return true;
		}

		DccThread * pSlaveThread = nullptr;
		if(dcc->window())
			pSlaveThread = dcc->window()->getSlaveThread();
		else if(dcc->transfer())
			pSlaveThread = dcc->transfer()->getSlaveThread();

		if(!pSlaveThread)
		{
			c->warning(__tr2qs_ctx("Unable to get SSL information: DCC session not initialized yet", "dcc"));
			c->returnValue()->setString("");
			return true;
		}

		KviSSL * pSSL = pSlaveThread->getSSL();
		if(!pSSL)
		{
			c->warning(__tr2qs_ctx("Unable to get SSL information: SSL non initialized yet in DCC session", "dcc"));
			c->returnValue()->setString("");
			return true;
		}

		KviSSLCertificate * pCert = bRemote ? pSSL->getPeerCertificate() : pSSL->getLocalCertificate();

		if(!pCert)
		{
			c->warning(__tr2qs_ctx("Unable to get SSL information: no peer certificate available", "dcc"));
			c->returnValue()->setString("");
			return true;
		}

		if(KviSSLMaster::getSSLCertInfo(pCert, szQuery, szParam1, c->returnValue()))
			return true;

		c->warning(__tr2qs_ctx("Unable to get SSL information: query not recognized", "dcc"));
		c->returnValue()->setString("");
		return true;
	}
	return true;
#endif
}

/*
	@doc: dcc
	@type:
		module
	@short:
		Direct Client Connection
	@title:
		The DCC module
	@body:
		[big]Overview[/big]
		The DCC module handles the Direct Client Connection
		protocol layer and all it's sub-protocols.[br]
		The sub-protocols include the standard CHAT
		the standard SEND and its variants plus several
		KVIrc extensions like RECV, RSEND, GET and VOICE.
		[big]Initiating a DCC negotiation[/big]
		The following commands initiate a specific DCC session
		with a remote client:[br]
		[cmd]dcc.chat[/cmd][br]
		[cmd]dcc.send[/cmd][br]
		[cmd]dcc.rsend[/cmd][br]
		[cmd]dcc.recv[/cmd][br]
		[cmd]dcc.get[/cmd][br]
		[cmd]dcc.voice[/cmd]
		[big]Handling the DCC events[/big]
		Each DCC session has an associated unique identifier (&lt;dcc_id&gt;).[br]
		You can interact with the session by using several commands
		and functions exported by this module and by passing the above session
		id as parameter.[br]
		The session related commands and functions are the following:[br]
		[fnc]$dcc.sessionList[/fnc][br]
		[fnc]$dcc.protocol[/fnc][br]
		[fnc]$dcc.connectionType[/fnc][br]
		[fnc]$dcc.transferStatus[/fnc][br]
		[fnc]$dcc.isFileTransfer[/fnc][br]
		[fnc]$dcc.isFileUpload[/fnc][br]
		[fnc]$dcc.isFileDownload[/fnc][br]
		[fnc]$dcc.localNick[/fnc][br]
		[fnc]$dcc.localUser[/fnc][br]
		[fnc]$dcc.localHost[/fnc][br]
		[fnc]$dcc.localIp[/fnc][br]
		[fnc]$dcc.localPort[/fnc][br]
		[fnc]$dcc.localFileName[/fnc][br]
		[fnc]$dcc.localFileSize[/fnc][br]
		[fnc]$dcc.remoteNick[/fnc][br]
		[fnc]$dcc.remoteUser[/fnc][br]
		[fnc]$dcc.remoteHost[/fnc][br]
		[fnc]$dcc.remoteIp[/fnc][br]
		[fnc]$dcc.remotePort[/fnc][br]
		[fnc]$dcc.remoteFileName[/fnc][br]
		[fnc]$dcc.remoteFileSize[/fnc][br]
		[fnc]$dcc.ircContext[/fnc][br]
		[fnc]$dcc.session[/fnc][br]
		[fnc]$dcc.getSSLCertInfo[/fnc][br]
*/

static bool dcc_module_init(KviModule * m)
{
	g_pDccBroker = new DccBroker();

	KVSM_REGISTER_SIMPLE_COMMAND(m, "send", dcc_kvs_cmd_send);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "chat", dcc_kvs_cmd_chat);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "video", dcc_kvs_cmd_video);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "voice", dcc_kvs_cmd_voice);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "recv", dcc_kvs_cmd_recv);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "rsend", dcc_kvs_cmd_rsend);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "get", dcc_kvs_cmd_get);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "abort", dcc_kvs_cmd_abort);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setBandwidthLimit", dcc_kvs_cmd_setBandwidthLimit);

	// FIXME: file upload / download state ?

	KVSM_REGISTER_FUNCTION(m, "transferStatus", dcc_kvs_fnc_transferStatus);
	KVSM_REGISTER_FUNCTION(m, "protocol", dcc_kvs_fnc_protocol);
	KVSM_REGISTER_FUNCTION(m, "connectionType", dcc_kvs_fnc_connectionType);
	KVSM_REGISTER_FUNCTION(m, "isFileTransfer", dcc_kvs_fnc_isFileTransfer);
	KVSM_REGISTER_FUNCTION(m, "isFileUpload", dcc_kvs_fnc_isFileUpload);
	KVSM_REGISTER_FUNCTION(m, "isFileDownload", dcc_kvs_fnc_isFileDownload);
	KVSM_REGISTER_FUNCTION(m, "localNick", dcc_kvs_fnc_localNick);
	KVSM_REGISTER_FUNCTION(m, "localUser", dcc_kvs_fnc_localUser);
	KVSM_REGISTER_FUNCTION(m, "localHost", dcc_kvs_fnc_localHost);
	KVSM_REGISTER_FUNCTION(m, "localIp", dcc_kvs_fnc_localIp);
	KVSM_REGISTER_FUNCTION(m, "localPort", dcc_kvs_fnc_localPort);
	KVSM_REGISTER_FUNCTION(m, "localFileName", dcc_kvs_fnc_localFileName);
	KVSM_REGISTER_FUNCTION(m, "localFileSize", dcc_kvs_fnc_localFileSize);
	KVSM_REGISTER_FUNCTION(m, "remoteNick", dcc_kvs_fnc_remoteNick);
	KVSM_REGISTER_FUNCTION(m, "remoteUser", dcc_kvs_fnc_remoteUser);
	KVSM_REGISTER_FUNCTION(m, "remoteHost", dcc_kvs_fnc_remoteHost);
	KVSM_REGISTER_FUNCTION(m, "remoteIp", dcc_kvs_fnc_remoteIp);
	KVSM_REGISTER_FUNCTION(m, "remotePort", dcc_kvs_fnc_remotePort);
	KVSM_REGISTER_FUNCTION(m, "remoteFileName", dcc_kvs_fnc_remoteFileName);
	KVSM_REGISTER_FUNCTION(m, "remoteFileSize", dcc_kvs_fnc_remoteFileSize);
	KVSM_REGISTER_FUNCTION(m, "averageSpeed", dcc_kvs_fnc_averageSpeed);
	KVSM_REGISTER_FUNCTION(m, "currentSpeed", dcc_kvs_fnc_currentSpeed);
	KVSM_REGISTER_FUNCTION(m, "transferredBytes", dcc_kvs_fnc_transferredBytes);
	KVSM_REGISTER_FUNCTION(m, "ircContext", dcc_kvs_fnc_ircContext);
	KVSM_REGISTER_FUNCTION(m, "session", dcc_kvs_fnc_session);
	KVSM_REGISTER_FUNCTION(m, "sessionList", dcc_kvs_fnc_sessionList);
	KVSM_REGISTER_FUNCTION(m, "getSSLCertInfo", dcc_kvs_fnc_getSSLCertInfo);

	return true;
}

static bool dcc_module_cleanup(KviModule *)
{
	delete g_pDccBroker;
	g_pDccBroker = nullptr;
#ifdef COMPILE_USE_GSM
	kvi_gsm_codec_done();
#endif

	return true;
}

static bool dcc_module_can_unload(KviModule *)
{
	return g_pDccBroker ? g_pDccBroker->canUnload() : true;
}

KVIRC_MODULE(
    "DCC",
    "4.0.0",
    "Copyright (C) 2000-2004:\n"
    "Szymon Stefanek (pragma at kvirc dot net)\n",
    "DCC extension for KVIrc\n",
    dcc_module_init,
    dcc_module_can_unload,
    0,
    dcc_module_cleanup,
    "dcc")
