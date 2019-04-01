//=============================================================================
//
//   File : KviKvsCoreSimpleCommands_gl.cpp
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
#include "KviKvsTimerManager.h"
#include "KviKvsAsyncDnsOperation.h"
#include "KviKvsScript.h"
#include "KviFileUtils.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionServerInfo.h"
#include "KviLocale.h"
#include "kvi_out.h"

namespace KviKvsCoreSimpleCommands
{
	/*
		@doc: halfop
		@type:
			command
		@title:
			halfop
		@syntax:
			halfop <nickname_list>
		@short:
			Sets halfop status to the specified users
		@description:
			Sets channel half operator status to the users specified in <nickname_list>,
			which is a comma separated list of nicknames.
			This command works only if executed in a channel window.
			The command is translated to a set of MODE messages containing
			a variable number of +h flags.
			This command is [doc:connection_dependent_commands]connection dependent[/doc].
		@examples:
			[example]
			halfop Pragma,Crocodile
			[/example]
		@seealso:
			[cmd]dehalfop[/cmd], [cmd]op[/cmd], [cmd]deop[/cmd], [cmd]voice[/cmd], [cmd]devoice[/cmd]
	*/

	KVSCSC(halfop)
	{
		return multipleModeCommand(__pContext, __pParams, __pSwitches, '+', 'h');
	}

	/*
		@doc: halt
		@type:
			command
		@title:
			halt
		@syntax:
			halt
		@short:
			Stops the execution of the current script
		@description:
			Stops the execution of the current script.[br]
			Note that this is different from [cmd]return[/cmd]: halt doesn't
			set the return value (use [cmd]setreturn[/cmd] for that) and
			in the event handlers this command may have additional semantics:
			refer to the specific event documentation for more info.[br]
			In fact, this command should be used [b]only[/b] in the event handlers,
			use [cmd]return[/cmd] in any other context.[br]
	*/

	KVSCSC(halt)
	{
		KVSCSC_pContext->setHaltCalled();
		return false;
	}

	/*
		@doc: help
		@type:
			command
		@title:
			help
		@syntax:
			help.open [-n] [-m] [document: string]
		@short:
			Shows a help document
		@description:
			Finds the first available help browser in the current frame
			then opens the specified [document].
			If no [document] is specified it the documentation index is shown.
			If no help browser is available, a new one is created.
			[document] can be an absolute path or a relative one: in this case
			the document is searched in the KVIrc documentation directory.[br]
			If no document has been found using absolute and relative paths,
			the first document matching [document] in the help search database
			is shown. Otherwise, an error page is displayed.[br/]
			The help browser has limited HTML browsing capabilities: you can
			use it to view simple HTML files on your filesystem.[br]
			This command is an alias for the [cmd]help.open[/cmd] command exported
			by the [i]help[/i] module.
		@switches:
			!sw: -m | --mdi
			The created browser is a MDI window,
			otherwise it is a static window.
			!sw: -n | --new
			The window creation is forced even
			if there are other help browsers already open.[br]
		@seealso:
			[cmd]help.open[/cmd]
		@examples:
			[example]
				help /home/pragma/myfile.html	//absolute path
				help cmd_snd.play.html		//relative path
				help "Binding operator"		//keyword search, remember quoting
				help OnNickServAuth		//keyword search for an event
				help \$my.user			//keyword search, $ needs to be escaped
			[/example]
	*/

	KVSCSC(help)
	{
		QString szParams;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("params", KVS_PT_STRING, KVS_PF_OPTIONAL | KVS_PF_APPENDREMAINING, szParams)
		KVSCSC_PARAMETERS_END

		// We just alias the help.open function
		QString szSwitches = "";
		if(KVSCSC_pSwitches->find('m', "mdi"))
			szSwitches.append("-m ");
		if(KVSCSC_pSwitches->find('n', "new"))
			szSwitches.append("-n ");

		if(szParams.endsWith("()"))
			szParams.chop(2);

		KviQString::escapeKvs(&szParams);
		KviKvsScript s("help", "help.open " + szSwitches + szParams);
		s.run(KVSCSC_pContext->window());

		return true;
	}

	/*
		@doc: host
		@type:
			command
		@title:
			host
		@syntax:
			host [-i] [-a] <dnsquery:string>
		@short:
			DNS lookup
		@switches:
			!sw: --ipv6 | -i
			Causes the command to run in IPv6 mode
			!sw: --any | -a
			Causes the command to run in unspecified mode and lookup both IPv4 and IPv6 addresses
		@description:
			Starts a DNS lookup for the <dnsquery> (hostname) and prints
			the results in the current window.[br]
			The -i switch causes the command to execute
			in IPv6 mode (and lookup ONLY IPv6 hosts!).[br]
			The -a switch causes the command to run in [i]unspecified[/i] mode
			and return any available address: IPv4 or IPv6.[br]
			This command also performs reverse lookups (if you pass an IP address as <hostname>).[br]
		@examples:
			[example]
				host localhost
				host irc.flashnet.it
				host -i irc.flashnet.it
				host -i A0E0:FFFF:::1010:1111:E0E0:A0A0
				host -a 127.0.0.1
				host 255.41.31.23
				host -a irc.stealth.net
			[/example]
		@seealso:
			[cmd]ahost[/cmd]
	*/

	KVSCSC(host)
	{
		QString szQuery;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("dnsquery", KVS_PT_NONEMPTYSTRING, 0, szQuery)
		KVSCSC_PARAMETERS_END

		KviDnsResolver::QueryType queryType = KviDnsResolver::IPv4;
		if(KVSCSC_pSwitches->find('i', "ipv6"))
			queryType = KviDnsResolver::IPv6;
		if(KVSCSC_pSwitches->find('a', "any"))
			queryType = KviDnsResolver::Any;

		new KviKvsAsyncDnsOperation(
		    KVSCSC_pContext->window(),
		    szQuery,
		    queryType);

		return true;
	}

	/*
		@doc: include
		@type:
			command
		@title:
			include
		@syntax:
			include [-q] [-e] [-f] [-r] <filename:string> [<parameter1:variant> [<parameter2:variant> [...]]]
		@short:
			A builtin alias for parse
		@description:
			This is a builtin alias for [cmd]parse[/cmd] that gives scripts a [i]more C-ish[/i] look :)
		@seealso:
			[cmd]parse[/cmd]
	*/

	/*
		@doc: info
		@type:
			command
		@title:
			info
		@syntax:
			info [target server]
		@short:
			Requests the server daemon info from a server
		@description:
			Requests server daemon information from the specified server or the current server if no [target server] is specified.[br]
			This command is an [doc:rfc2812wrappers]RFC2812 command wrapper[/doc]; see that document for more information.[br]
	*/
	// RFC2812 wrapper

	/*
		@doc: inject
		@type:
			command
		@title:
			inject
		@syntax:
			inject <text>
		@short:
			Injects <text> to the socket
		@switches:
			!sw: -q | --quiet
			Does not print injection warnings on the console
		@description:
			Injects <text> to the socket as if it comes from the server.[br]
			It's useful to test scripts without spamming or flooding the server.[br]
			Each injection causes a warning to be displayed in the console window and in
			the socketspy window (if open). The -q switch suppresses only the console warnings.
		@examples:
			[example]
				[comment]Injects the string as we were sending a query message[/comment][br]
				inject :foo!bar@foobar PRIVMSG HelLViS69 :message
			[/example]
	*/

	KVSCSC(inject)
	{
		QString szText;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("text", KVS_PT_NONEMPTYSTRING, KVS_PF_APPENDREMAINING, szText)
		KVSCSC_PARAMETERS_END

		KVSCSC_REQUIRE_CONNECTION

		if(!((KVSCSC_pWindow->type() == KviWindow::Channel) || (KVSCSC_pWindow->type() == KviWindow::Query) || (KVSCSC_pWindow->type() == KviWindow::Console)))
		{
			KVSCSC_pContext->warning(__tr2qs_ctx("The current window is not a channel, a query or a console", "kvs"));
			return false;
		}

		// Get the active window where we ran the command
		KviWindow * pActive = KVSCSC_pWindow->context()->console()->activeWindow();

		// Scan the windows
		for(auto pWnd : g_pMainWindow->windowList())
		{
			// Search for the right socketspy
			if((pWnd->type() == KviWindow::SocketSpy) && (pWnd->context() == pActive->context()))
			{
				// Ok, found... send the warning
				pWnd->outputNoFmt(KVI_OUT_SOCKETMESSAGE, __tr2qs_ctx("The following string was injected by the user:", "kvs"));
				break;
			}
		}

		// Send the warning to the right console
		if(!KVSCSC_pSwitches->find('q', "quiet"))
			pActive->console()->output(KviWindow::SocketSpy, __tr2qs_ctx("Injected incoming data: %1", "kvs").arg(szText));

		// Encode the text for the socket
		QByteArray szT = KVSCSC_pConnection->encodeText(szText);

		// Send the converted text to the socket
		KVSCSC_pConnection->incomingMessage(szT.constData());

		return true;
	}

	/*
		@doc: invite
		@type:
			command
		@title:
			invite
		@syntax:
			invite <nickname> <channel>
		@short:
			Invites a user to a channel
		@description:
			Invites the specified user to the <channel>.[br]
			This command is an [doc:rfc2812wrappers]RFC2812 command wrapper[/doc]; see that document for more information.[br]
	*/
	// RCF2812 wrapper

	/*
		@doc: ison
		@type:
			command
		@title:
			ison
		@syntax:
			ison <nickname>
		@short:
			Checks if a user is online
		@description:
			Checks to see if the specified nickname is currently connected to the network.[br]
			This command is an [doc:rfc2812wrappers]RFC2812 command wrapper[/doc]; see that document for more information.[br]
	*/
	// RFC2812 wrapper

	/*
		@doc: join
		@type:
			command
		@title:
			join
		@syntax:
			join <channel list> [key list]
		@short:
			Joins the specified channels
		@description:
			Joins the channels specified in <channel list> by using the
			optional keys in the [key list].[br]
			This command is "server based"; you will see its results only
			after the server acknowledges it.[br]
			This command is [doc:connection_dependent_commands]connection dependent[/doc].[br]
		@examples:
			[example]
				join #kvirc
				join #linux-it,#xmms,#kde linus
			[/example]
	*/

	KVSCSC(join)
	{
		QString szChans, szKeys;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("chans", KVS_PT_NONEMPTYSTRING, 0, szChans)
		KVSCSC_PARAMETER("keys", KVS_PT_STRING, KVS_PF_OPTIONAL, szKeys)
		KVSCSC_PARAMETERS_END

		KVSCSC_REQUIRE_CONNECTION

		QStringList slChans = szChans.split(",", QString::SkipEmptyParts);

		QString szChanTypes = KVSCSC_pConnection->serverInfo()->supportedChannelTypes();
		for(auto & slChan : slChans)
		{
			if(!slChan.isEmpty())
			{
				bool bFound = false;
				int id = 0;
				QChar f = slChan[0];
				while((id < szChanTypes.length()) && !bFound)
				{
					if(f == szChanTypes[id])
						bFound = true;
					id++;
				}
				if(!bFound)
					slChan.prepend("#");
			}
		}

		szChans = slChans.join(",");
		QByteArray szEncodedChans = KVSCSC_pConnection->encodeText(szChans);

		if(szKeys.isEmpty())
		{
			if(!(KVSCSC_pConnection->sendFmtData("JOIN %s", szEncodedChans.data())))
				return KVSCSC_pContext->warningNoIrcConnection();
		}
		else
		{
			QByteArray szEncodedKeys = KVSCSC_pConnection->encodeText(szKeys);
			if(!(KVSCSC_pConnection->sendFmtData("JOIN %s %s", szEncodedChans.data(), szEncodedKeys.data())))
				return KVSCSC_pContext->warningNoIrcConnection();
		}

		return true;
	}

	/*
		@doc: kick
		@type:
			command
		@title:
			kick
		@syntax:
			kick <user:string> [reason:string]
		@short:
			Kicks a user from a channel
		@description:
			Kicks the specified <user> from the current channel.[br]
			This command is a simple wrapper over the IRC KICK command.
			This command is [doc:connection_dependent_commands]connection dependent[/doc].
		@examples:
			[example]
				kick Pragma Go coding in Pascal!
			[/example]
	*/

	KVSCSC(kick)
	{
		QString szUser;
		QString szReason;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("user", KVS_PT_NONEMPTYSTRING, 0, szUser)
		KVSCSC_PARAMETER("reason", KVS_PT_STRING, KVS_PF_OPTIONAL | KVS_PF_APPENDREMAINING, szReason)
		KVSCSC_PARAMETERS_END

		KVSCSC_REQUIRE_CONNECTION

		if(KVSCSC_pWindow->type() != KviWindow::Channel)
		{
			KVSCSC_pContext->warning(__tr2qs_ctx("The current window is not a channel", "kvs"));
			return false;
		}

		QByteArray szC = KVSCSC_pConnection->encodeText(KVSCSC_pWindow->target());
		QByteArray szU = KVSCSC_pConnection->encodeText(szUser);
		QByteArray szR = KVSCSC_pConnection->encodeText(szReason);

		if(szR.isEmpty())
		{
			if(!KVSCSC_pConnection->sendFmtData("KICK %s %s", szC.data(), szU.data()))
				return KVSCSC_pContext->warningNoIrcConnection();
		}
		else
		{
			if(!KVSCSC_pConnection->sendFmtData("KICK %s %s :%s", szC.data(), szU.data(), szR.data()))
				return KVSCSC_pContext->warningNoIrcConnection();
		}

		return true;
	}

	/*
		@doc: killtimer
		@title:
			killtimer
		@type:
			command
		@short:
			Stops a timer
		@syntax:
			killtimer -a
			killtimer [-q] [name]
		@switches:
			!sw: -a | --active
			Requests to kill all the active timers
			!sw: -q | --quiet
			Does not print warnings if the specified timer is not running
		@description:
			The first version (with -a) kill all the running timers.[br]
			The second version kills a running timer named [name].[br]
			The timer has been started by the [cmd]timer[/cmd].[br]
			[name] can be omitted if this command is called from a [cmd]timer[/cmd] callback:
			in this case the current timer will be scheduled for killing immediately
			after it has returned control to KVIrc.
		@seealso:
			[cmd]timer[/cmd], [fnc]$isTimer[/fnc], [cmd]listtimers[/cmd], [fnc]$listtimers[/fnc]
	*/

	KVSCSC(killtimer)
	{
		QString szName;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("name", KVS_PT_STRING, KVS_PF_OPTIONAL, szName)
		KVSCSC_PARAMETERS_END

		if(KVSCSC_pSwitches->find('a', "active"))
		{
			KviKvsTimerManager::instance()->deleteAllTimers();
			return true;
		}

		if(szName.isEmpty())
		{
			if(!KviKvsTimerManager::instance()->deleteCurrentTimer())
			{
				if(!KVSCSC_pSwitches->find('q', "quiet"))
					KVSCSC_pContext->warning(__tr2qs_ctx("Timer name omitted but there is not current timer (this is not a timer callback)", "kvs"));
			}
		}
		else
		{
			if(!KviKvsTimerManager::instance()->deleteTimer(szName))
			{
				if(!KVSCSC_pSwitches->find('q', "quiet"))
					KVSCSC_pContext->warning(__tr2qs_ctx("Can't kill the timer '%Q' since it is not running", "kvs"), &szName);
			}
		}
		return true;
	}

	/*
		@doc: leave
		@type:
			command
		@title:
			leave
		@syntax:
			leave [-k] [-q] [-s] <channel list> [part message]
		@short:
			Alias for part
		@description:
			Builtin alias for the command [cmd]part[/cmd].
	*/
	// Internally aliased to leave

	/*
		@doc: links
		@type:
			command
		@title:
			links
		@syntax:
			links
		@short:
			Lists server links
		@description:
			List server links.[br]
			The syntax of the parameters of this command is different on many servers.[br]
			This command is an [doc:rfc2812wrappers]RFC2812 command wrapper[/doc]; see that document for more information.[br]
		@seealso:
			[cmd]map[/cmd]
	*/
	// RFC2812 wrapper

	/*
		@doc: list
		@type:
			command
		@title:
			list
		@syntax:
			list [ <channel> *( ',' <channel> ) [ <target> ] ]
		@short:
			Lists channels
		@description:
			List channels.[br]
			The syntax of the parameters of this command is different on many servers.[br]
			This command is an [doc:rfc2812wrappers]RFC2812 command wrapper[/doc]; see that document for more information.[br]
	*/
	// RFC2812 wrapper

	/*
		@doc: listtimers
		@title:
			listtimers
		@type:
			command
		@short:
			Lists the active timers
		@syntax:
			listtimers
		@description:
			Lists the currently active timers
		@seealso:
			[cmd]timer[/cmd], [fnc]$isTimer[/fnc], [cmd]killtimer[/cmd]
	*/

	KVSCSC(listtimers)
	{
		KviPointerHashTable<QString, KviKvsTimer> * pTimerDict = KviKvsTimerManager::instance()->timerDict();

		if(!pTimerDict)
			return true;

		KviPointerHashTableIterator<QString, KviKvsTimer> it(*pTimerDict);

		KVSCSC_pContext->window()->outputNoFmt(KVI_OUT_VERBOSE, __tr2qs_ctx("List of active timers", "kvs"));

		unsigned int uCnt = 0;

		while(KviKvsTimer * pTimer = it.current())
		{
			QString szName = pTimer->name();
			QString szLifetime;
			switch(pTimer->lifetime())
			{
				case KviKvsTimer::Persistent:
					szLifetime = __tr2qs_ctx("Persistent", "kvs");
					break;
				case KviKvsTimer::WindowLifetime:
					szLifetime = __tr2qs_ctx("Window Lifetime", "kvs");
					break;
				case KviKvsTimer::SingleShot:
					szLifetime = __tr2qs_ctx("Single Shot", "kvs");
					break;
				default:
					szLifetime = __tr2qs_ctx("Unknown", "kvs");
					break;
			}
			QString szDelay;
			szDelay.setNum(pTimer->delay());
			QString szWindow;
			szWindow = pTimer->window() ? pTimer->window()->id() : __tr2qs_ctx("None", "kvs");

			KVSCSC_pContext->window()->output(KVI_OUT_VERBOSE,
			    "Timer \"%Q\": Lifetime: %Q, Delay: %Q, Window: %Q",
			    &szName, &szLifetime, &szDelay, &szWindow);

			uCnt++;
			++it;
		}

		KVSCSC_pContext->window()->output(KVI_OUT_VERBOSE, __tr2qs_ctx("Total: %u timers running", "kvs"), uCnt);

		return true;
	}

	/*
		@doc: lusers
		@type:
			command
		@title:
			lusers
		@syntax:
			lusers [ <mask> [ <target> ] ]
		@short:
			Requests statistics about users on IRC
		@description:
			The LUSERS command is used to get statistics about the size of the
			IRC network.[br]If no parameter is given, the reply will be about the
			whole net.[br]If a <mask> is specified, then the reply will only
			concern the part of the network formed by the servers matching the
			mask.[br]Finally, if the <target> parameter is specified, the request
			is forwarded to that server which will generate the reply.[br]
			This command is an [doc:rfc2812wrappers]RFC2812 command wrapper[/doc]; see that document for more information.[br]
	*/
	// RFC2812 wrapper
};
