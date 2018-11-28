//=============================================================================
//
//   File : KviKvsCoreSimpleCommands_sz.cpp
//   Creation date : Fri 31 Oct 2003 00:04:25 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviKvsCoreSimpleCommands.h"
#include "KviKvsPopupManager.h"
#include "KviKvsScript.h"
#include "KviIrcServerReconnectInfo.h"
#include "KviIrcConnection.h"
#include "KviConsoleWindow.h"
#include "KviLocale.h"
#include "KviApplication.h"
#include "KviAsynchronousConnectionData.h"
#include "KviMainWindow.h"
#include "KviNetUtils.h"
#include "KviMenuBar.h"
#include "KviUserInput.h"
#include "KviChannelWindow.h"
#include "KviQueryWindow.h"
#include "kvi_out.h"

#include <cstdlib>

namespace KviKvsCoreSimpleCommands
{
	/*
		@doc: say
		@type:
			command
		@title:
			say
		@syntax:
			say [-x] [-q] [-r=<window_id>] <text:string>
		@switches:
			!sw: -q | --quiet
			Run quietly
			!sw: -r=<window_id> | --rebind=<window_id>
			Standard command rebinding switch
			!sw: -x | --allow-exec
			Allow the execution of commands by interpreting the leading slash in the <text>
		@short:
			Type text in a window
		@description:
			This command is more or less equivalent to typing text in the input
			bar of the current window. If the -r switch is used
			then the command is rebound to the window specified by <window_id>.
			The main difference is that the variables and identifiers in <text>
			are always parsed (when typing this happen only if the text is a command).[br]
			The switch -x will make say evaluate and execute arbitrary commands, too:
			if <text> begins with a slash then it will be treated as a command
			to be evaluated and executed (after parsing the identifiers etc.).[br]
			If this happens, the executed command will not be send to the active channel.
			[example]
				/say -x /[cmd]echo[/cmd] foo
			[/example]
			will just print "foo".[br]
			Please note that using /say -x with a <text> that isn't a constant
			in the script but comes from some unidentified external source (e.g. the network)
			is a potential security flaw as it enables anyone to execute arbitrary commands:
			don't ever do it.[br]
			When the -x switch is not used, the text is never interpreted as a command.
			-q causes the command to run quietly.[br]
			Please note that /say will also trigger the [event:ontextinput]OnTextInput[/event] event.
			[b]Handle with care.[/b]
	*/

	KVSCSC(say)
	{
		QString szText;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("text", KVS_PT_STRING, KVS_PF_APPENDREMAINING, szText)
		KVSCSC_PARAMETERS_END

		if(KVSCSC_pSwitches->find('x', "allow-exec"))
		{
			// allow execution of commands
			if(!KviUserInput::parse(szText, KVSCSC_pWindow, __tr2qs_ctx("Say: injected commandline", "kvs")))
			{
				if(!KVSCSC_pSwitches->find('q', "quiet"))
					KVSCSC_pContext->warning(__tr2qs_ctx("Say parse error: Broken command", "kvs"));
			}
		}
		else
		{
			KviUserInput::parseNonCommand(szText, KVSCSC_pWindow);
		}
		return true;
	}

	/*
		@doc: server
		@type:
			command
		@title:
			server
		@syntax:
			server [switches] [<server:string> [<port:uint>]]
		@keyterms:
			connecting to a server
		@short:
			Connects to the specified server
		@switches:
			!sw: -b=<local address:string> | --bind-address=<local address:string>
			Bind the outgoing connection to <local address>. The <local address> must be
			either the IP address of a local interface or a local interface name itself
			(e.g. eth0 under Linux, en0 under macOS etc.) suitable for contacting the
			remote <server>:
			this means that if you're going to use the -i switch, the <local address>
			[b]must[/b] be an IPv6 address.
			If using a local interface name as <local address> (working on most, but not all systems),
			the KVIrc network engine will automatically detect the interface address (i.e. the default
			address of that interface).
			!sw: -c=<command sequence:string> | --command=<command sequence:string>
			The <command sequence> will be executed just after the login operations
			have been completed.
			!sw: -f=<socket filter:string> | --filter=<socket filter:string>
			Activates <socket filter> on the connection.
			A socket filter is a plugin that can adapt the IRC protocol to other existing protocols such as OpenNap.
			For plain IRC it is not needed.
			!sw: -i | --ipv6
			Connects to the <server> via the IPv6 protocol
			(if IPv6 support has been compiled in).
			!sw: -n | --new-context
			Forces the connection to be attempted in a new IRC context (server window) instead of the current one.
			!sw: -m | --minimized
			If a new console window is created (due to -n or -u) then create it as minimized.
			!sw: -p=<password:string> | --password=<password:string>
			Uses <password> to login to the server (the password will be stored in the server
			entry too).
			!sw: -q=<nick:string> | --nickname=<nick:string>
			Uses <nick> to login to the server (the nickname will be stored in the server
			entry too).
			!sw: -s | --ssl
			Activates the SSL support for this connection (if OpenSSL support has been compiled in).
			If SSL is enabled and no port is specified, the connection will be made to port 6697.
			!sw: -u | --unused-context
			Forces the connection to be attempted in the first IRC context that has
			no connection in progress. If all the IRC contexts have connections in progress
			then a new one is created.
			!sw: -l | --last
			When <server> and <port> are empty use the last ones used in the IRC context.
		@description:
			Attempts a connection to the specified <server>
			on the specified <port>. If no <port> is specified
			the default 6667 is used. If no <server> is specified,
			the currently selected server from the server list is used
			unless the -l switch is passed: in this case the last server
			that was used in the IRC context is re-used.
			If the <server> is a valid IPv6 address, the -i switch is implied.[br]
			The <server> is added to the server list (if not already
			there) and set as current.[br]
			[br]
			If <server> is in the form [i]id:<some_string>[/i] then <some_string>
			is interpreted as the server's internal ID (specified in the options dialog).
			This is useful when you need to force KVIrc to choose between multiple
			server entries with the same hostname and port stored in the database.[br]
			Please note that this form causes most of the switches to have no effect
			since the entry in the database will override them. If no server
			entry with the specified identifier is found then an error will be generated
			and the connection attempt will stop.
			[br]
			If <server> doesn't seem to be a valid IP address or hostname (i.e. it contains no dots)
			and it doesn't look to be in the form [i]id:<some_string>[/i] then
			it is assumed to be a network name and if such a network is found
			in the server list then the best server for that network is contacted.
			(KVIrc tries to guess the round-robin (random) servers first).[br]
			This behaviour can be forced by adding the [i]net:[/i] prefix to the <server>
			(so it will work even if the network name contains a dot).[br]
			[br]
			The connection is attempted in the current IRC context. If there is already
			a connection in progress then it is [i]brutally[/i] terminated.[br]
		@examples:
			[example]
				server irc.tin.it
				server -i irc6.ircd.it
				server -i ircd.stealth.net 6667
				server irc.undernet.org 6665
				server -n irc.openprojects.net
				server -c="join #kvirc" irc.stealth.net 6665
				server -b=ppp0 irc.tin.it
				server undernet
				server id:mybouncerwithusername1
				server id:mybouncerwithusername2
				server net:under.net
			[/example]
	*/

	KVSCSC(server)
	{
		QString szServer;
		kvs_uint_t uPort;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("server", KVS_PT_STRING, KVS_PF_OPTIONAL, szServer)
		KVSCSC_PARAMETER("port", KVS_PT_UINT, KVS_PF_OPTIONAL, uPort)
		KVSCSC_PARAMETERS_END

		KviConsoleWindow * console = nullptr;
		if(KVSCSC_pSwitches->find('n', "new-context"))
			console = g_pMainWindow->createNewConsole(false, !KVSCSC_pSwitches->find('m', "minimized"));
		else
		{
			if(KVSCSC_pSwitches->find('u', "unused-context"))
			{
				// first try the current console
				console = KVSCSC_pWindow->console();
				if(console)
				{
					// if there is a connection in progress, proceed in searching
					if(console->connectionInProgress())
					{
						console = nullptr;
					}
				}
				if(!console)
				{
					// yep, have to search
					console = g_pMainWindow->firstNotConnectedConsole();
					if(!console)
					{
						console = g_pMainWindow->createNewConsole(false, !KVSCSC_pSwitches->find('m', "minimized"));
					}
				}
			}
			else
				console = KVSCSC_pWindow->console();
		}

		QString szCmd, szBindAddress, szSocketFilter, szPassword, szNick;
		KVSCSC_pSwitches->getAsStringIfExisting('c', "command", szCmd);
		KVSCSC_pSwitches->getAsStringIfExisting('b', "bind-address", szBindAddress);
		KVSCSC_pSwitches->getAsStringIfExisting('f', "filter", szSocketFilter);
		KVSCSC_pSwitches->getAsStringIfExisting('p', "password", szPassword);
		KVSCSC_pSwitches->getAsStringIfExisting('q', "nickname", szNick);

		if(!console)
		{
			KVSCSC_pContext->error(__tr2qs_ctx("Couldn't find a suitable IRC context for the connection, try using -n or -u", "kvs"));
			return false;
		}

		if(szServer.isEmpty())
		{
			// just as hitting "connect"
			if(console->connectionInProgress())
			{
				KVSCSC_pContext->error(__tr2qs_ctx("Another connection is already in progress in the selected IRC context", "kvs"));
				return false;
			}

			KviAsynchronousConnectionData * d = new KviAsynchronousConnectionData();
			d->bUseLastServerInContext = KVSCSC_pSwitches->find('l', "last") != nullptr;
			d->szCommandToExecAfterConnect = szCmd;
			d->szBindAddress = szBindAddress;
			d->szPass = szPassword;
			d->szNick = szNick;
			d->szLinkFilter = szSocketFilter;

			d->m_pReconnectInfo = new KviIrcServerReconnectInfo();
			d->m_pReconnectInfo->m_szNick = szNick;
			d->m_pReconnectInfo->m_szPass = szPassword;

			console->context()->setAsynchronousConnectionData(d);
			console->context()->connectToCurrentServer();
		}
		else
		{
			// server is not empty.
			KviAsynchronousConnectionData * d = new KviAsynchronousConnectionData();
			d->bUseSSL = (KVSCSC_pSwitches->find('s', "ssl") != nullptr);
			d->bSTARTTLS = false;
			d->szServer = szServer;
			// if the user wants to connect using ssl but didn't specify a port, default to 6697
			if (d->bUseSSL && !(uPort > 0))uPort = 6697;
			d->uPort = (kvi_u32_t)uPort;
			d->szLinkFilter = szSocketFilter;
			d->bPortIsOk = (uPort > 0);
#ifdef COMPILE_IPV6_SUPPORT
			d->bUseIPv6 = (KVSCSC_pSwitches->find('i', "ipv6") != nullptr) || KviNetUtils::isValidStringIPv6(szServer);
#else
			d->bUseIPv6 = false;
#endif
			d->szCommandToExecAfterConnect = szCmd;
			d->szBindAddress = szBindAddress;
			d->szPass = szPassword;
			d->szNick = szNick;

			d->m_pReconnectInfo = new KviIrcServerReconnectInfo();
			d->m_pReconnectInfo->m_szNick = szNick;
			d->m_pReconnectInfo->m_szPass = szPassword;

			console->context()->setAsynchronousConnectionData(d);
			console->context()->beginAsynchronousConnect(0);
		}

		return true;
	};

	/*
		@doc: service
		@type:
			command
		@title:
			service
		@syntax:
			service <nickname> <reserved> <distribution> <type> <reserved> [:]<info>
		@short:
			Registers a new service
		@description:
			Registers a new IRC service: this is for IRC operators only.[br]
			This command is an [doc:rfc2812wrappers]RFC2812 command wrapper[/doc]; see that document for more information.[br]
			WARNING: the <info> parameter may contain spaces: in that case you [b]must[/b] add the leading [b]]:[/b]] character.[br]
	*/
	// RFC2921 wrapper

	/*
		@doc: setmenu
		@type:
			command
		@title:
			setmenu
		@syntax:
			setmenu [-q] [-i=<index:integer>] <visible_text:string> [menu_name:string]
		@short:
			Adds a popup menu to the menu bar
		@switches:
			!sw: -q | --quiet
			Run quietly: do not print any warnings
			!sw: -i=<index:integer> | --index=<index:integer>
			Place the popup menu on the left of the existing menubar item at the specified index
		@description:
			Adds an [b]existing[/b] popup menu to the current frame's menu bar.[br]
			<visible_text> is used as menu identification and obviously as the visible
			menu bar text for the new popup.[br]
			If a popup menu with the same <visible_text> or the same <menu_name> already exists in
			the current frame, it is removed first.[br]
			<visible_text> can optionally contain the [b]]&[/b]] character to identify
			the popup menu accelerator.[br]
			[menu_name], if given, must be a valid name of a popup menu created earlier
			with [cmd]defpopup[/cmd] (or the popup editor).[br]
			If [menu_name] is not given, the popup menu identified by <visible_text> is removed.[br]
			If the -i switch is used, the new popup menu will be placed to the left of the default
			menu-bar item specified in the [b]zero based[/b] <index>.[br]
			menu-bar text item: the new popup menu will be placed on the left of that default item.[br]
			If -q is used, this command prints no warnings (intended for use when you are unsure that
			the menu exists - normal warning messages are debugging aids and have no consequence).[br]
		@seealso:
			[cmd]defpopup[/cmd]
		@examples:
			[example]
				setmenu -q -i=2 Test a popup
				setmenu Test
				setmenu ThisDoesNotExist
				setmenu -q ThisDoesNotExist
			[/example]
	*/

	KVSCSC(setmenu)
	{
		QString szVisibleText, szPopupName;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("visible_text", KVS_PT_NONEMPTYSTRING, 0, szVisibleText)
		KVSCSC_PARAMETER("popup_name", KVS_PT_STRING, KVS_PF_OPTIONAL, szPopupName)
		KVSCSC_PARAMETERS_END

		if(szPopupName.isEmpty())
		{
			if(!(g_pMainWindow->mainMenuBar()->removeMenu(szVisibleText)))
			{
				if(!KVSCSC_pSwitches->find('q', "quiet"))
					KVSCSC_pContext->warning(__tr2qs_ctx("No menu bar item with text '%Q'", "kvs"), &szVisibleText);
			}
			return true;
		}

		KviKvsPopupMenu * p = KviKvsPopupManager::instance()->lookup(szPopupName);
		int idx = -1;
		if(KviKvsVariant * pIdx = KVSCSC_pSwitches->find('i', "index"))
		{
			kvs_int_t iIdx;
			if(pIdx->asInteger(iIdx))
				idx = iIdx;
			else
			{
				if(!KVSCSC_pSwitches->find('q', "quiet"))
					KVSCSC_pContext->warning(__tr2qs_ctx("Invalid index specified: ignored", "kvs"));
			}
		}
		g_pMainWindow->mainMenuBar()->removeMenu(szVisibleText);

		idx = g_pMainWindow->mainMenuBar()->getDefaultItemRealIndex(idx);

		if(p)
		{
			g_pMainWindow->mainMenuBar()->addMenu(szVisibleText, p, idx);
		}
		else
		{
			if(!KVSCSC_pSwitches->find('q', "quiet"))
				KVSCSC_pContext->warning(__tr2qs_ctx("The popup '%Q' is not defined", "kvs"), &szPopupName);
		}

		return true;
	}

	/*
		@doc: setreturn
		@type:
			command
		@title:
			setreturn
		@syntax:
			setreturn <string>
		@short:
			Sets the return value of the current command sequence
		@description:
			Sets the return value of the current command sequence to <string>.
			You can call this command multiple times in a command sequence
			(so in fact use it as a write-only variable).
		@examples:
		@seealso:
			[cmd]return[/cmd]
	*/

	KVSCSC(setreturn)
	{
		if(KVSCSC_pParams->count() == 0)
		{
			KVSCSC_pContext->returnValue()->setNothing();
			return false;
		}
		if(KVSCSC_pParams->count() == 1)
		{
			KVSCSC_pContext->returnValue()->copyFrom(*(KVSCSC_pParams->first()));
			return false;
		}

		QString all;
		KVSCSC_pParams->allAsString(all);
		KVSCSC_pContext->returnValue()->setString(all);
		return true;
	}

	/*
		@doc: sockets
		@type:
			command
		@title:
			sockets
		@syntax:
			sockets [[remote server] server mask]
		@short:
			Requests the sockets info from a server
		@description:
			Requests the sockets information from the specified server or the current server if no [remote server] is specified.[br]
			The reply can be filtered by <server mask>.[br]
			This command is an [doc:rfc2812wrappers]RFC2812 command wrapper[/doc]; see that document for more information.[br]
	*/
	// RFC2812 wrapper

	/*
		@doc: squery
		@type:
			command
		@title:
			squery
		@syntax:
			squery <target> <text>
		@switches:
			!sw: -q | --quiet
			Do not print any output
		@short:
			Sends a message to a service
		@description:
			Sends a message to a service in a form similar to [cmd]privmsg[/cmd].[br]
			<target> must be a service(see RFC2812).
			This command is [doc:connection_dependent_commands]connection dependent[/doc].[br]
		@examples:
			[example]
				squery Alis LIST * -min 100
			[/example]
	*/

	KVSCSC(squery)
	{
		QString szTarget, szText;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("target", KVS_PT_NONEMPTYSTRING, 0, szTarget)
		KVSCSC_PARAMETER("text", KVS_PT_STRING, KVS_PF_OPTIONAL | KVS_PF_APPENDREMAINING, szText)
		KVSCSC_PARAMETERS_END

		KVSCSC_REQUIRE_CONNECTION

		KviWindow * w = KVSCSC_pConnection->findChannel(szTarget);
		if(!w)
			w = KVSCSC_pConnection->findQuery(szTarget);

		if(w)
			w->ownMessage(szText, !KVSCSC_pSwitches->find('q', "quiet"));
		else
		{
			QByteArray szT = KVSCSC_pConnection->encodeText(szTarget);
			QByteArray szD = KVSCSC_pConnection->encodeText(szText);

			if(!(KVSCSC_pConnection->sendFmtData("SQUERY %s :%s", szT.data(), szD.data())))
				return KVSCSC_pContext->warningNoIrcConnection();

			if(!KVSCSC_pSwitches->find('q', "quiet"))
				KVSCSC_pWindow->output(KVI_OUT_OWNPRIVMSG, "[SQUERY >>> %Q]: %Q", &szTarget, &szText);
		}

		return true;
	}

	/*
		@doc: squit
		@type:
			command
		@title:
			squit
		@syntax:
			squit <server> [:]<comment>
		@short:
			Disconnects a server socket (ircops only)
		@description:
			Disconnects a server socket: this is for IRC operators only.[br]
			This command is an [doc:rfc2812wrappers]RFC2812 command wrapper[/doc]; see that document for more information.[br]
			WARNING: the <comment> parameter may contain spaces: in that case you MUST add the leading [b]]:[/b]] character
			before.[br]
	*/
	// RFC2812 wrapper

	/*
		@doc: srand
		@type:
			command
		@title:
			srand
		@syntax:
			srand <seed:integer>
		@short:
			Initializes the random number generator
		@description:
			Initializes the random number generator [i]seeding[/i] it with
			<seed> which must be an integer. If no <seed> is passed it defaults to 1.
			Sequences of random numbers can be repeated by using the
			same <seed> value.
		@examples:
	*/

	KVSCSC(srand)
	{
		QString tmp;

		for(int i = 0; i < 10; i++)
			tmp.append(QChar((::rand() % 30) + 65));
		kvs_int_t iSeed;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("seed", KVS_PT_INT, KVS_PF_OPTIONAL, iSeed)
		KVSCSC_PARAMETERS_END
		if(KVSCSC_pParams->count() < 1)
			iSeed = 1;
		::srand(iSeed);
		return true;
	}

	/*
		@doc: stats
		@type:
			command
		@title:
			stats
		@syntax:
			stats [query [target server]]
		@short:
			Requests the stats info from  a server
		@description:
			Requests the stats information from the specified server or the current server if no [target server] is specified.[br]
			The query parameter is usually a letter indicating the type of the query requested: some query letters
			are defined by [doc]RFC2812[/doc] and some others are server implementation dependent.[br]
			This command is an [doc:rfc2812wrappers]RFC2812 command wrapper[/doc]; see that document for more information.[br]
	*/
	// RFC2812 wrapper

	/*
		@doc: time
		@type:
			command
		@title:
			time
		@syntax:
			time [target server]
		@short:
			Requests the current local time info from a server
		@description:
			Requests time information from the specified server or the current server if no [target server] is specified.[br]
			This command is an [doc:rfc2812wrappers]RFC2812 command wrapper[/doc]; see that document for more information.[br]
	*/
	// RFC2812 wrapper

	/*
		@doc: topic
		@type:
			command
		@title:
			topic
		@syntax:
			topic [<channel:string> [topic text:string]]
		@short:
			Changes topic for the specified channel
		@description:
			Changes the topic for the specified <channel> to <topic text>.
			If no topic is specified, the current channel topic will be returned.[br]
			This command is [i]]server based[/i]]; you will see its results only
			after the server acknowledges it.[br]
			This command is [doc:connection_dependent_commands]connection dependent[/doc].
		@examples:
			[example]
				topic #kvirc KVIrc 3.4.0 has been released!
			[/example]
	*/

	KVSCSC(topic)
	{
		QString szChannel;
		QString szTopic;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("channel", KVS_PT_NONEMPTYSTRING, KVS_PF_OPTIONAL, szChannel)
		KVSCSC_PARAMETER("topic", KVS_PT_STRING, KVS_PF_OPTIONAL | KVS_PF_APPENDREMAINING, szTopic)
		KVSCSC_PARAMETERS_END

		KVSCSC_REQUIRE_CONNECTION

		if(szChannel.isEmpty())
		{
			if(KVSCSC_pWindow->type() == KviWindow::Channel)
				szChannel = KVSCSC_pWindow->target();
			else
			{
				KVSCSC_pContext->error(__tr2qs_ctx("No target channel specified and the current window is not a channel", "kvs"));
				return false;
			}
		}

		QByteArray szC = KVSCSC_pConnection->encodeText(szChannel);
		QByteArray szT = KVSCSC_pConnection->encodeText(szTopic);
		if(szTopic.isEmpty())
		{
			if(!KVSCSC_pConnection->sendFmtData("TOPIC %s", szC.data()))
				return KVSCSC_pContext->warningNoIrcConnection();
		}
		else
		{
			if(!KVSCSC_pConnection->sendFmtData("TOPIC %s :%s", szC.data(), szT.data()))
				return KVSCSC_pContext->warningNoIrcConnection();
		}

		return true;
	}

	/*
		@doc: trace
		@type:
			command
		@title:
			trace
		@syntax:
			trace <target>
		@short:
			Requests trace information about a specified machine in the network
		@description:
			Requests trace information about a specified machine in the network.[br]
			This command is an [doc:rfc2812wrappers]RFC2812 command wrapper[/doc]; see that document for more information.[br]
	*/
	// RFC2812 wrapper

	/*
		@doc: trload
		@type:
			command
		@title:
			trload
		@syntax:
			trload [-v] <catalogue:string> [<catalogue_dir:string>]
		@switches:
			!sw: -v | --verbose
			Warn if a catalogue for the current language cannot be found
		@short:
			Loads a translation catalogue
		@description:
			Loads the specified translation <catalogue> in the current user language.[br]
			The translation catalogues are *.mo files compiled with the standard
			msgfmt program from the gettext package.[br]
			The syntax of the catalogue names is:[br]
			[b]<catalogue_dir>/<catalogue>_<language>.mo[/b][br]
			KVIrc looks for the catalogues in <catalogue_dir> (if specified)
			and in the locale subdirectory of the global and local KVIrc directories.[br]
			<language> is choose automatically by KVIrc depending on the user's locale.[br]
			Catalogues MUST be in UTF-8 encoding.[br]
			After a translation catalogue has been loaded you can
			access its strings by the means of [fnc]$tr[/fnc].[br]
			This function doesn't print any warning unless -v is specified.[br]
			Please note that KVIrc attempts to load the <catalogue> automatically
			when $tr() is used but it looks only in [fnc]$file.localdir[/fnc]/locale/
			and in [fnc]$file.globaldir[/fnc]/locale/. This command is then useful
			to load catalogues that reside in different directories.[br]
			[b]NOTE[/b][br]
			This feature is currently work in progress: the scripting engine
			is not yet fully Unicode and thus some more exotic translations may still suffer...
		@seealso:
			[fnc]$tr[/fnc], [cmd]trunload[/cmd]
	*/

	KVSCSC(trload)
	{
		QString szCatalogue, szDir;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("catalogue", KVS_PT_NONEMPTYSTRING, 0, szCatalogue)
		KVSCSC_PARAMETER("catalogueDir", KVS_PT_STRING, KVS_PF_OPTIONAL, szDir)
		KVSCSC_PARAMETERS_END

		if(!szDir.isEmpty())
		{
			if(KviLocale::instance()->loadCatalogue(szCatalogue, szDir))
				return true;
		}

		g_pApp->getLocalKvircDirectory(szDir, KviApplication::Locale);

		if(KviLocale::instance()->loadCatalogue(szCatalogue, szDir))
			return true;

		g_pApp->getGlobalKvircDirectory(szDir, KviApplication::Locale);

		if(KviLocale::instance()->loadCatalogue(szCatalogue, szDir))
			return true;

		if(KVSCSC_pSwitches->find('v', "verbose"))
			KVSCSC_pContext->warning(__tr2qs_ctx("No catalogue %Q for the current language found", "kvs"), &szCatalogue);

		return true;
	}

	/*
		@doc: trunload
		@type:
			command
		@title:
			trunload
		@syntax:
			trunload [-v] <catalogue:string>
		@switches:
			!sw: -v | --verbose
			Warn if the specified catalogue wasn't loaded at all
		@short:
			Unloads a translation catalogue
		@description:
			Unloads a specified translation catalogue.[br]
			When you no longer need the translation strings contained in the
			catalogue just unload it to free memory.[br]
			This command prints no warnings unload -v is used.[br]
		@seealso:
			[fnc]$tr[/fnc], [cmd]trload[/cmd]
	*/

	KVSCSC(trunload)
	{
		QString szCatalogue;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("catalogue", KVS_PT_NONEMPTYSTRING, 0, szCatalogue)
		KVSCSC_PARAMETERS_END

		if(!KviLocale::instance()->unloadCatalogue(szCatalogue))
		{
			if(KVSCSC_pSwitches->find('v', "verbose"))
				KVSCSC_pContext->warning(__tr2qs_ctx("The catalogue %Q was not loaded", "kvs"), &szCatalogue);
		}
		return true;
	}

	/*
		@doc: unban
		@type:
			command
		@title:
			unban
		@syntax:
			unban <mask_list>
		@short:
			Unsets ban masks for the channel
		@description:
			Removes the ban masks specified in the <mask_list>,
			which is a comma separated list of nicknames.
			This command works only if executed in a channel window.
			The command is translated to a set of MODE messages containing
			a variable number of -b flags.
			This command is [doc:connection_dependent_commands]connection dependent[/doc].
		@examples:
			[example]
				# Unban people on the current channel (say #kvirc)
				unban Maxim,Gizmo!*@*,*!root@*
				# Do the same but from another window belonging to this IRC context
				unban -r=[fnc]$channel[/fnc](#kvirc) Maxim,Gizmo!*@*,*!root@*
				# Do the same from any window
				unban -r=[fnc]$channel[/fnc](#kvirc,[fnc]$context[/fnc](irc.myirc.net,Pragma)) Maxim,Gizmo!*@*,*!root*@*
			[/example]
		@seealso:
			[cmd]op[/cmd],
			[cmd]deop[/cmd],
			[cmd]voice[/cmd],
			[cmd]devoice[/cmd],
			[cmd]ban[/cmd]
	*/

	KVSCSC(unban)
	{
		return multipleModeCommand(__pContext, __pParams, __pSwitches, '-', 'b');
	}

	/*
		@doc: userop
		@type:
			command
		@title:
			userop
		@syntax:
			userop <nickname_list>
		@short:
			Sets userop flag for the specified users
		@description:
			Sets the voice flag for the users specified in <nickname_list>,
			which is a comma separated list of nicknames.
			This command works only if executed in a channel window.
			The command is translated to a set of MODE messages containing
			a variable number of +u flags.
			This command is [doc:connection_dependent_commands]connection dependent[/doc].
		@examples:
			[example]
				userop Pragma,Crocodile
			[/example]
		@seealso:
			[cmd]chanowner[/cmd], [cmd]dechanowner[/cmd], [cmd]chanadmin[/cmd], [cmd]dechanadmin[/cmd], [cmd]op[/cmd], [cmd]halfop[/cmd], [cmd]dehalfop[/cmd], [cmd]voice[/cmd], [cmd]devoice[/cmd], [cmd]deuserop[/cmd]
	*/

	KVSCSC(userop)
	{
		return multipleModeCommand(__pContext, __pParams, __pSwitches, '+', 'u');
	}

	/*
		@doc: version
		@type:
			command
		@title:
			version
		@syntax:
			version [target server]
		@short:
			Requests the version of a server
		@description:
			Requests the version information from the specified server or the current server if no [target server] is specified.[br]
			This command is an [doc:rfc2812wrappers]RFC2812 command wrapper[/doc]; see that document for more information.[br]
	*/
	// RFC2812 wrapper

	/*
		@doc: voice
		@type:
			command
		@title:
			voice
		@syntax:
			voice <nickname_list>
		@short:
			Sets voice flag for the specified users
		@description:
			Sets the voice flag for the users specified in <nickname_list>,
			which is a comma separated list of nicknames.
			This command works only if executed in a channel window.
			The command is translated to a set of MODE messages containing
			a variable number of +v flags.
			This command is [doc:connection_dependent_commands]connection dependent[/doc].
		@examples:
			[example]
				voice Pragma,Crocodile
			[/example]
		@seealso:
			[cmd]chanowner[/cmd], [cmd]dechanowner[/cmd], [cmd]chanadmin[/cmd], [cmd]dechanadmin[/cmd], [cmd]op[/cmd], [cmd]halfop[/cmd], [cmd]dehalfop[/cmd], [cmd]devoice[/cmd], [cmd]userop[/cmd], [cmd]deuserop[/cmd]
	*/

	KVSCSC(voice)
	{
		return multipleModeCommand(__pContext, __pParams, __pSwitches, '+', 'v');
	}

	/*
		@doc: warning
		@type:
			command
		@title:
			warning
		@syntax:
			warning <message:text>
		@short:
			Prints a warning message
		@description:
			Prints a warning message with the appropriate color and message type.
			It doesn't alter the execution of the current script.
		@seealso:
			[cmd]error[/cmd]
	*/

	KVSCSC(warning)
	{
		QString szAll;
		KVSCSC_pParams->allAsString(szAll);
		KVSCSC_pContext->warning("%Q", &szAll);

		return true;
	}

	/*
		@doc: wallops
		@type:
			command
		@title:
			wallops
		@syntax:
			wallops <message:text>
		@short:
			Broadcasts a WALLOPS message
		@description:
			Broadcasts a WALLOPS message to all users whom are set
			to receive WALLOPS messages.
		@seealso:
			[cmd]operwall[/cmd]
	*/

	KVSCSC(wallops)
	{
		QString szMessage;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("message", KVS_PT_NONEMPTYSTRING, KVS_PF_APPENDREMAINING, szMessage)
		KVSCSC_PARAMETERS_END

		KVSCSC_REQUIRE_CONNECTION

		QByteArray szData = KVSCSC_pContext->connection()->encodeText(szMessage);

		if(!KVSCSC_pContext->connection()->sendFmtData("WALLOPS :%s", szData.data()))
			return KVSCSC_pContext->warningNoIrcConnection();

		return true;
	}

	/*
		@doc: who
		@type:
			command
		@title:
			who
		@syntax:
			who <filter>
		@short:
			Requests WHO information
		@description:
			Requests WHO information about users/channels.[br]
			The <filter> is a string specifying the subject of the query.
			In the standard IRC protocol the subject can be a nickname or a channel.
			If a channel name is used as filter then the list of all the users on that channel is returned.
			Recent IRC servers support the WHOX extension which specifies that the <filter> can have
			a much more complex syntax: <mask1> <options> [<mask2>].
			<mask1> is a comma separated list of filters that can contain wildcards (* and ?),
			<options> is a set of characters specifying what <mask1> is matching (nicknames, channel
			names, user names, real names, IP addresses etc) and what kind of information should be returned.
			Look for the extended WHOX syntax on your favorite search engine.[br]
			If no parameter is specified at all, it requests a WHO information about the current
			channel.[br]
			This command is [doc:connection_dependent_commands]connection dependent[/doc].
		@examples:
			[example]
				[comment]# Lists users on #kvirc[/comment]
				who #kvirc
				[comment]# WHOX: Get a list of +o users, aka ircops (works on unreal, bahamut, others)[/comment]
				who 0 o%nuhs
			[/example]
		@seealso:
			[cmd]names[/cmd]
	*/

	KVSCSC(who)
	{
		QString szData;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("filter", KVS_PT_NONEMPTYSTRING, KVS_PF_OPTIONAL | KVS_PF_APPENDREMAINING, szData)
		KVSCSC_PARAMETERS_END

		KVSCSC_REQUIRE_CONNECTION

		if(szData.isEmpty())
		{
			if(KVSCSC_pWindow->type() != KviWindow::Channel)
			{
				KVSCSC_pContext->error(__tr2qs_ctx("No target mask/channel specified and the current window is not a channel", "kvs"));
				return false;
			}

			szData = KVSCSC_pWindow->target();
		}

		QByteArray szData2 = KVSCSC_pConnection->encodeText(szData);

		if(!KVSCSC_pConnection->sendFmtData("WHO %s", szData2.data()))
			return KVSCSC_pContext->warningNoIrcConnection();

		return true;
	}

	/*
		@doc: whois
		@type:
			command
		@title:
			whois
		@syntax:
			whois [server] <nickname>
		@short:
			Requests user information
		@description:
			Requests information about an IRC user specified by <nickname>.[br]
			If [server] is specified, the request is directed to that one. [server]
			may be a nickname so that the request is redirected to the server that
			the user with that nickname is connected to.[br]
			An useful trick is to use the following syntax:[br]
			WHOIS <nickname> <nickname>[br]
			This will requests information about the user <nickname> to the server that he is currently
			connected to; returning the idle time among the other common information.[br]
			This command is [doc:connection_dependent_commands]connection dependent[/doc].
		@examples:
			[example]
			[comment]# Query info about Pragma[/comment]
				whois Pragma
				[comment]# Get the Pragma idle time[/comment]
				whois Pragma Pragma
			[/example]
		@seealso:
			[cmd]whowas[/cmd]
	*/

	KVSCSC(whois)
	{
		QString szNick;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("nickname", KVS_PT_NONEMPTYSTRING, KVS_PF_APPENDREMAINING, szNick)
		KVSCSC_PARAMETERS_END

		KVSCSC_REQUIRE_CONNECTION

		QByteArray szData = KVSCSC_pContext->connection()->encodeText(szNick);

		if(!KVSCSC_pContext->connection()->sendFmtData("WHOIS %s", szData.data()))
			return KVSCSC_pContext->warningNoIrcConnection();

		return true;
	}

	/*
		@doc: whowas
		@type:
			command
		@title:
			whowas
		@syntax:
			whowas <nickname>
		@short:
			Requests user information
		@description:
			Requests information about an IRC user specified by <nickname>.[br]
			WHOWAS requests [i]history[/i] records. See [doc:rfc2812]RFC2812[/doc] for more info.[br]
			This command is [doc:connection_dependent_commands]connection dependent[/doc].
		@examples:
			[example]
				[comment]# Query past info about Pragma[/comment]
				whowas Pragma
			[/example]
		@seealso:
			[cmd]whois[/cmd]
	*/

	KVSCSC(whowas)
	{
		QString szNick;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("nickname", KVS_PT_NONEMPTYSTRING, KVS_PF_APPENDREMAINING, szNick)
		KVSCSC_PARAMETERS_END

		KVSCSC_REQUIRE_CONNECTION

		QByteArray szData = KVSCSC_pContext->connection()->encodeText(szNick);

		if(!KVSCSC_pContext->connection())
			return KVSCSC_pContext->warningNoIrcConnection();
		if(!KVSCSC_pContext->connection()->sendFmtData("WHOWAS %s", szData.data()))
			return KVSCSC_pContext->warningNoIrcConnection();

		return true;
	}
};
