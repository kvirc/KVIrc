//=============================================================================
//
//   File : KviKvsCoreCallbackCommands.cpp
//   Creation date : Fri 31 Oct 2003 04:07:58 by Szymon Stefanek
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

#include "KviKvsCoreCallbackCommands.h"

#include "KviKvsKernel.h"
#include "KviKvsTimerManager.h"
#include "KviKvsAliasManager.h"
#include "KviKvsVariantList.h"
#include "KviKvsAsyncDnsOperation.h"
#include "KviKvsEventManager.h"
#include "KviKvsProcessManager.h"
#include "KviKvsObjectController.h"

#include "KviCommandFormatter.h"
#include "KviIrcConnectionAsyncWhoisData.h"
#include "KviIrcConnection.h"
#include "KviScriptButton.h"
#include "KviIconManager.h"
#include "KviLocale.h"

#include "KviTalToolTip.h"

#include <QRegExp>

namespace KviKvsCoreCallbackCommands
{
	/*
		@doc: ahost
		@type:
			command
		@title:
			ahost
		@syntax:
			ahost [-i] [-a] (<dnsquery:string>[,<magicdata:variant>]){ <callback command> }
		@short:
			DNS lookup
		@switches:
			!sw: --ipv6 | -i
			Causes the command to run in IPv6 mode
			!sw: --any | -a
			Causes the command to run in unspecified mode and lookup both IPv4 and IPv6 addresses
		@description:
			Starts a DNS lookup for the <dnsquery> and reports the
			results by calling the callback routine.
			The -i switch causes the command to execute
			in IPv6 mode (and lookup ONLY IPv6 hosts!).[br]
			The -a switch causes the command to run in [i]unspecified[/i] mode
			and return any available address: IPv4 or Ipv6.[br]
			This command also performs reverse lookups (if you pass an IP address as <hostname>).[br]
			The callback command gets passed five parameters:[br]
			$0 contains the query string (<dnsquery> in fact)[br]
			$1 contains the value 1 if the query was successful.[br]
			In that case the remaining parameters are set as follows:[br]
			$2 contains the first IP address associated to the <dnsquery>[br]
			$3 contains the hostname associated to the <dnsquery>[br]
			$4 contains the eventual <magicdata> passed.[br]
			If $1 contains the value 0 then the query has failed and[br]
			$2 contains an error message explaining the failure.[br]
			$3 is empty[br]
			$4 contains the eventual <magicdata> passed.[br]
			Please note that if the DNS query fails to even start for some
			reason then your callback MAY be called even before ahost() returns.[br]
		@switches:
			!sw: -i
			Causes the command to execute in IPv6 mode (and lookup ONLY IPv6 hosts!).
			!sw: -a
			The -a switch causes the command to run in [i]unspecified[/i] mode
			and return any available address: IPv4 or IPv6.
		@examples:
			[example]
			ahost("localhost")
			{
				[cmd]echo[/cmd] "Lookup: "$0;
				if($1)
				{
					[cmd]echo[/cmd] "IP address: "$2;
					[cmd]echo[/cmd] "Hostname: "$3;
				} else {
					[cmd]echo[/cmd] "Error: $2";
				}
			}
			ahost -i ("irc.flashnet.it","Hello :)")
			{
				[cmd]echo[/cmd] "Lookup: "$0;
				[cmd]echo[/cmd] "Magic: $3";
				if($1)
				{
					[cmd]echo[/cmd] "IP address: "$2;
					[cmd]echo[/cmd] "Hostname: "$3;
				} else {
					[cmd]echo[/cmd] "Error: $2";
				}
			}
			ahost -a ("cafe:babe::dead:beef")
			{
				[cmd]echo[/cmd] "Lookup: "$0;
				[cmd]echo[/cmd] "Magic: $3";
				if($1)
				{
					[cmd]echo[/cmd] "IP address: "$2;
					[cmd]echo[/cmd] "Hostname: "$3;
				} else {
					[cmd]echo[/cmd] "Error: $2";
				}
			}
			[/example]
		@seealso:
			[cmd]host[/cmd]
	*/

	KVSCCC(ahost)
	{
		QString szQuery;
		KviKvsVariant * pMagicPtr;
		KVSCCC_PARAMETERS_BEGIN
		KVSCCC_PARAMETER("dnsquery", KVS_PT_NONEMPTYSTRING, 0, szQuery)
		KVSCCC_PARAMETER("magic", KVS_PT_VARIANT, KVS_PF_OPTIONAL, pMagicPtr)
		KVSCCC_PARAMETERS_END

		KviDnsResolver::QueryType queryType = KviDnsResolver::IPv4;
		if(KVSCCC_pSwitches->find('i', "ipv6"))
			queryType = KviDnsResolver::IPv6;
		if(KVSCCC_pSwitches->find('a', "any"))
			queryType = KviDnsResolver::Any;

		KviKvsVariant * pMagic = pMagicPtr ? new KviKvsVariant(*pMagicPtr) : new KviKvsVariant();

		new KviKvsAsyncDnsOperation(
		    KVSCCC_pContext->window(),
		    szQuery,
		    queryType,
		    new KviKvsScript(*KVSCCC_pCallback),
		    pMagic);

		return true;
	}

	/*
		@doc: alias
		@title:
			alias
		@type:
			command
		@short:
			Adds a new alias or modifies an existing one
		@syntax:
			alias [-q] (<alias_name>) <implementation>
			alias [-q] (<alias_name>){}
		@switches:
			!sw: -q | --quiet
			Causes the command to run quietly
		@description:
			Adds the alias <alias_name> with the specified <implementation> code.
			The implementation code can be either a single KVS instruction
			or an instruction block (instruction list enclosed in braces).[br]
			If the alias already exists, it is replaced with the new implementation.[br]
			If the <implementation> is empty (e.g. [i]{}[/i] or just a [i];[/i])
			the alias <alias_name> is removed.
			If the [i]remove[/i] form is used but the specified <alias_name> does not exist
			in the alias store then a warning is printed unless
			the -q (--quiet) switch is used.
			If <alias_name> contains a [i]<name>::[/i] prefix, then the alias
			is created in the namespace specified by <name>.
			If the namespace does not exist, it is created.
			Any alias without the [i]<name>::[/i] prefix is created in the root
			namespace. Namespaces are useful to avoid collisions in alias names
			between scripts. Only really common aliases should be created
			in the root namespace - all of the internal functionality of your
			scripts should be hidden in your own namespace.
			The special syntax [i]<namespace>::[/i] can be used to remove all
			the aliases belonging to the specified namespace. When creating
			aliases this syntax is not allowed.
		@examples:
			[example]
				[comment]# Add the alias j[/comment]
				alias(j)
				{
					[cmd]join[/cmd] $0;
				}
				[comment]# Remove the alias j[/comment]
				alias(j){}
				[comment]# Add the alias j in namespace letters[/comment]
				alias(letters::j)
				{
					[cmd]echo[/cmd] "j"
				}
				[comment]# Kill the whole 'letters' namespace[/comment]
				alias(letters::){}
			[/example]
		@seealso:
			[doc:kvs_aliasesandfunctions]Aliases and functions[/doc]
	*/

	/*
		@doc: function
		@title:
			function
		@type:
			command
		@short:
			A synonym for alias
		@syntax:
			function [-q] (<function_name>) <implementation>
			function [-q] (<function_name>){}
		@switches:
			!sw: -q | --quiet
			Causes the command to run quietly
		@description:
			This command is a synonym for [cmd]alias[/cmd].
		@seealso:
			[doc:kvs_aliasesandfunctions]Aliases and functions[/doc]
	*/

	KVSCCC(alias)
	{
		KviKvsVariant * vName = KVSCCC_pParams->first();

		if(!vName || vName->isEmpty())
		{
			KVSCCC_pContext->error(__tr2qs_ctx("Missing alias name", "kvs"));
			return false;
		}

		QString szName;
		vName->asString(szName);

		// we allow only [\w:]+
		QRegExp re("[\\w:]+");
		if(!re.exactMatch(szName))
		{
			KVSCCC_pContext->error(__tr2qs_ctx("Alias names can contain only letters, digits, underscores and '::' namespace separators", "kvs"));
			return false;
		}

		// make sure that we have only doubled "::" and not ":" or ":::..."
		QString tmp = szName;
		tmp.replace("::", "@"); // @ is not allowed by the rule above
		if(tmp.indexOf(":") != -1)
		{
			KVSCCC_pContext->error(__tr2qs_ctx("Stray ':' character in alias name: did you mean ...<namespace>::<name>?", "kvs"));
			return false;
		}

		if((tmp.indexOf("@@") != -1) || tmp.startsWith("@"))
		{
			KVSCCC_pContext->error(__tr2qs_ctx("Found an empty namespace in alias name", "kvs"));
			return false;
		}

		if(KVSCCC_pCallback->code().isEmpty())
		{
			if(tmp.endsWith("@"))
			{
				if(!KviKvsAliasManager::instance()->removeNamespace(szName))
				{
					if(!KVSCCC_pSwitches->find('q', "quiet"))
						KVSCCC_pContext->warning(__tr2qs_ctx("The namespace '%Q' does not exist", "kvs"), &szName);
				}
			}
			else
			{
				if(!KviKvsAliasManager::instance()->remove(szName))
				{
					if(!KVSCCC_pSwitches->find('q', "quiet"))
						KVSCCC_pContext->warning(__tr2qs_ctx("The alias '%Q' does not exist", "kvs"), &szName);
				}
			}
		}
		else
		{

			if(tmp.endsWith("@"))
			{
				KVSCCC_pContext->error(__tr2qs_ctx("Found an empty alias name within a namespace", "kvs"));
				return false;
			}

			KviKvsScript * pScript = new KviKvsScript(*KVSCCC_pCallback);
			pScript->setName(szName);
			KviKvsAliasManager::instance()->add(szName, pScript);
		}
		return true;
	}

	/*
		@doc: awhois
		@type:
			command
		@title:
			awhois
		@syntax:
			awhois [-i] (<nickname:string>[,<magic:variant>])
			{
				<callback command>
			}
		@short:
			Asynchronous WHOIS
		@switches:
			!sw: -i | --idle-time
			Ask the WHOIS information to the server that <nickname> is
			connected to, effectively returning the user's idle time.
		@description:
			AWHOIS stands for Asynchronous WHOIS. It is used to obtain data for a specified
			irc user (designated by <nickname>). This command sends a WHOIS query to the
			server and silently awaits the sequence of replies. When the [i]End of WHOIS[/i] message
			is received from server the <callback command> is executed passing the WHOIS
			information as positional parameters.[br]
			The <magic> string is an optional string to be evaluated at AWHOIS execution time.
			It is passed as the last positional parameter.[br]
			Callback command parameters:[br]
			$0 = nickname[br]
			$1 = username[br]
			$2 = hostname[br]
			$3 = realname (may be empty)[br]
			$4 = server[br]
			$5 = idle time (may be empty)[br]
			$6 = signon time (may be empty)[br]
			$7 = channels (may be empty)[br]
			$8 = server that provided the information[br]
			$9 = away message (empty if user hasn't set an away message)[br]
			$10 = magic string evaluated at AWHOIS call (may be empty)[br]
			$11 = account the user is logged into (may be empty)[br]
			$12 = additional, server-specific info (may be empty)[br]
			If the -i switch is specified, the WHOIS message is sent to the server
			that the <nickname> user is connected to; in this way you will probably
			get the idle time of the user too.[br]
			If the server replies with a [i]No such nick/channel error message[/i] the
			<callback command> will be still triggered, but will have all the parameters
			empty with the exception of $0.[br]
			If the connection gets interrupted before all the information have been received,
			the <callback command> will never be triggered.[br]
			This command is [doc:connection_dependent_commands]connection dependent[/doc].[br]
		@examples:
			[example]
			awhois(pragma){ echo $0-; }
			[/example]
	*/

	KVSCCC(awhois)
	{
		QString szNick;
		KviKvsVariant * pMagic;
		KVSCCC_PARAMETERS_BEGIN
		KVSCCC_PARAMETER("nickname", KVS_PT_NONEMPTYSTRING, 0, szNick)
		KVSCCC_PARAMETER("magic", KVS_PT_VARIANT, KVS_PF_OPTIONAL, pMagic)
		KVSCCC_PARAMETERS_END

		KVSCCC_REQUIRE_CONNECTION

		QByteArray szN = KVSCCC_pConnection->encodeText(szNick);

		KviAsyncWhoisInfo * info = new KviAsyncWhoisInfo();
		info->pCallback = new KviKvsScript(*KVSCCC_pCallback);
		info->pMagic = pMagic ? new KviKvsVariant(*pMagic) : new KviKvsVariant();
		info->szNick = szNick;
		info->pWindow = KVSCCC_pWindow;

		KVSCCC_pConnection->asyncWhoisData()->add(info);

		if(KVSCCC_pSwitches->find('i', "idle-time"))
			KVSCCC_pConnection->sendFmtData("WHOIS %s %s", szN.data(), szN.data());
		else
			KVSCCC_pConnection->sendFmtData("WHOIS %s", szN.data());

		return true;
	}

	/*
		@doc: button
		@title:
			button
		@type:
			command
		@short:
			Adds/removes/changes user definable buttons
		@syntax:
			button [-d] [-q] (<type_unused:variant>,<name:string>[,<image_id:string>[,<label_text:string>]])
			{
				<callback_code>
			}
		@switches:
			!sw: -d | --disabled
			Creates the button as disabled
			!sw: -q | --quiet
			Run quietly, print no warnings
		@description:
			Adds a new user defined button with the specified <name>.[br]
			[br]
			The <type_unused> parameter is ignored and is present only for
			backward compatibility.
			[br]
			The button image is specified by the [doc:image_id]<image_id>[/doc].[br]
			The optional button text is specified by <label_text>.[br]
			The <callback_code> will be executed as reaction to a button press.[br]
			[br]
			The [i]window[/i] type button can be added only to the windows that have a button container: this
			actually includes at least console, channels and queries.[br]
			The button is added to the current window; if you want to add it to a different
			window, use the [doc:command_rebinding]standard -r command rebinding[/doc] switch.[br]
			The <callback_code> will be executed as reaction to a button press; the
			code execution will be bound to the window that the button is attached to.[br]
			If a button with <name> already exists in the current window, its parameters are changed
			according to the passed values (<image_id>, <label_text> and <callback_code>).[br]
			[br]
			Passing an empty <callback_value> removes the button.[br]
			The callback parameters $0 and $1 will contain the screen coordinates of the bottom-left
			corner of the button: this is useful for showing a popup menu in response to the click.[br]
			If the -q switch is used, this command prints no warnings.[br]
			The -d switch causes the button to be disabled (grayed).[br]
		@examples:
			[example]
				button(w,test,-1,Test button){ echo Test!; }
				button(w,test){}
			[/example]

	*/

	KVSCCC(button)
	{
		KviKvsVariant * pUnused;
		QString szName, szIcon, szLabel;
		KVSCCC_PARAMETERS_BEGIN
		KVSCCC_PARAMETER("type_unused", KVS_PT_VARIANT, 0, pUnused)
		KVSCCC_PARAMETER("name", KVS_PT_NONEMPTYSTRING, 0, szName)
		KVSCCC_PARAMETER("icon", KVS_PT_NONEMPTYSTRING, KVS_PF_OPTIONAL, szIcon)
		KVSCCC_PARAMETER("label", KVS_PT_NONEMPTYSTRING, KVS_PF_OPTIONAL, szLabel)
		KVSCCC_PARAMETERS_END

		KviScriptUserButton * pButton = nullptr;

		if(!KVSCCC_pWindow->buttonContainer())
		{
			if(!KVSCCC_pSwitches->find('q', "quiet"))
				KVSCCC_pContext->warning(__tr2qs_ctx("The specified window has no button containers", "kvs"));
			return true;
		}

		//pButton = (KviScriptUserButton *)(KVSCCC_pWindow->buttonContainer())->child(szName.toUtf8().data(),"KviWindowScriptButton");
		pButton = (KviScriptUserButton *)(KVSCCC_pWindow->buttonContainer())->findChild<KviWindowScriptButton *>(szName);

		if(KVSCCC_pCallback->code().isEmpty())
		{
			if(pButton)
				delete pButton;
			else
			{
				if(!KVSCCC_pSwitches->find('q', "quiet"))
					KVSCCC_pContext->warning(__tr2qs_ctx("Window button '%Q' not found", "kvs"), &szName);
			}
			return true;
		}

		if(!pButton)
		{
			pButton = new KviWindowScriptButton(KVSCCC_pWindow->buttonContainer(), KVSCCC_pWindow, szName.toUtf8().data());
			pButton->show();
		}

		KviTalToolTip::remove(pButton);
		if(!szLabel.isEmpty())
		{
			pButton->setButtonText(szLabel);
			KviTalToolTip::add(pButton, szLabel);
		}

		pButton->setButtonCode(new KviKvsScript(*KVSCCC_pCallback));

		if(!szIcon.isEmpty())
		{
			QPixmap * pix = g_pIconManager->getImage(szIcon);
			if(pix)
			{
				pButton->setButtonPixmap(*pix);
			}
			else
			{
				if(!KVSCCC_pSwitches->find('q', "quiet"))
					KVSCCC_pContext->warning(__tr2qs_ctx("Can't find the icon '%Q'", "kvs"), &szIcon);
			}
		}

		pButton->setEnabled(!(KVSCCC_pSwitches->find('d', "disabled")));

		return true;
	}

	/*
		@doc: event
		@title:
			event
		@type:
			command
		@short:
			Adds a new event handler
		@syntax:
			event [-q] (<event_name>,<handler_name>)
			{
				<implementation>
			}
		@switches:
			!sw: -q | --quiet
			Do not print any warnings
		@description:
			Adds the handler <handler_name> with <implementation> to
			the list of handlers for the event <event_name>.[br]
			If the <implementation> is empty
			the handler <handler_name> is removed from the handler
			list instead of being added.[br]
			The <event_name> may be one of the KVIrc builtin event names
			or a numeric code (from 0 to 999) of a RAW server message.[br]
			<handler_name> can only contain alphanumeric characters. If the
			provided handler name contains invalid characters, they are
			silently removed. If the provided handler name does not contain
			a single valid character, the handler will be named "unnamed".[br]
			If the -q switch is specified then the command runs in quiet mode.
		@seealso:
			[cmd]eventctl[/cmd] [fnc]$iseventenabled[/fnc]
	*/

	KVSCCC(event)
	{
		QString szEventName, szHandlerName;
		KVSCCC_PARAMETERS_BEGIN
		KVSCCC_PARAMETER("event_name", KVS_PT_NONEMPTYSTRING, 0, szEventName)
		KVSCCC_PARAMETER("handler_name", KVS_PT_NONEMPTYSTRING, 0, szHandlerName)
		KVSCCC_PARAMETERS_END

		bool bOk;
		int iNumber = szEventName.toInt(&bOk);
		bool bIsRaw = (bOk && (iNumber >= 0) && (iNumber < 1000));

		if(bIsRaw)
		{
			if(!KviKvsEventManager::instance()->isValidRawEvent(iNumber))
			{
				if(!KVSCCC_pSwitches->find('q', "quiet"))
					KVSCCC_pContext->warning(__tr2qs_ctx("No such event (%Q)", "kvs"), &szEventName);
				return true;
			}
		}
		else
		{
			KviKvsEventManager::instance()->cleanHandlerName(szHandlerName);
			iNumber = KviKvsEventManager::instance()->findAppEventIndexByName(szEventName);
			if(!KviKvsEventManager::instance()->isValidAppEvent(iNumber))
			{
				if(!KVSCCC_pSwitches->find('q', "quiet"))
					KVSCCC_pContext->warning(__tr2qs_ctx("No such event (%Q)", "kvs"), &szEventName);
				return true;
			}
		}

		if(KVSCCC_pCallback->code().isEmpty())
		{
			if(bIsRaw)
			{
				if(!KviKvsEventManager::instance()->removeScriptRawHandler(iNumber, szHandlerName))
				{
					if(!KVSCCC_pSwitches->find('q', "quiet"))
						KVSCCC_pContext->warning(__tr2qs_ctx("No handler '%Q' for RAW numeric event '%d'", "kvs"), &szHandlerName, iNumber);
				}
			}
			else
			{
				if(!KviKvsEventManager::instance()->removeScriptAppHandler(iNumber, szHandlerName))
				{
					if(!KVSCCC_pSwitches->find('q', "quiet"))
						KVSCCC_pContext->warning(__tr2qs_ctx("No handler '%Q' for event '%Q'", "kvs"), &szHandlerName, &szEventName);
				}
			}
		}
		else
		{
			if(bIsRaw)
			{
				// remove the old handler
				KviKvsEventManager::instance()->removeScriptRawHandler(iNumber, szHandlerName);
				QString szContext = QString("RawEvent%1::%2").arg(iNumber).arg(szHandlerName);
				KviKvsScriptEventHandler * pHandler = new KviKvsScriptEventHandler(szHandlerName, szContext, KVSCCC_pCallback->code());
				KviKvsEventManager::instance()->addRawHandler(iNumber, pHandler);
			}
			else
			{
				// remove the old handler
				KviKvsEventManager::instance()->removeScriptAppHandler(iNumber, szHandlerName);
				QString szContext = QString("%1::%2").arg(szEventName, szHandlerName);
				KviKvsScriptEventHandler * pHandler = new KviKvsScriptEventHandler(szHandlerName, szContext, KVSCCC_pCallback->code());
				KviKvsEventManager::instance()->addAppHandler(iNumber, pHandler);
			}
		}

		return true;
	}

	/*
		@doc: exec
		@type:
			command
		@title:
			exec
		@syntax:
			exec [switches] (<commandline:string>[,<magic data:variant>])
			{
				<callback command>
			}
		@short:
			Asynchronous execution of external programs
		@switches:
			!sw: -q | --quiet
				Quiet: do not print any warnings
			!sw: -t | --trigger-termination
				Trigger the termination event
			!sw: -x | --trigger-startup
				Trigger the startup event
			!sw: -n | --no-stdout
				Do [b]not[/b] trigger any stdout events
			!sw: -e | --trigger-stderr
				Trigger stderr events
			!sw: -b | --output-block
				Trigger the <callback command> with the stdout and stderr events exactly once,
				passing the complete block of process output. The events are triggered even
				if the process output is empty.
			!sw: -k=<maximum run time> | --kill-after=<maximum run time>
				Kill the process unconditionally after <maximum run time> milliseconds.
				If the -t switch is used then the termination event will be
				triggered just after the process has been killed.
			!sw: -p=<timeout> | --trigger-ping=<timeout>
				Trigger <callback command> with [i]ping[/i] events every <timeout> milliseconds.
			!sw: -w | --bind-to-window
				Kill the process if the current window is closed. In this case the
				termination event is [b]not[/b] triggered (since the parent window has been lost).
				If this switch is not used then the process is rebound to
				the active console window and continues running.
			!sw: -s=<interpreter command> | --shell=<interpreter command>
				Use <interpreter command> instead of the default interpreter [i]sh -c[/i].
				The <interpreter command> should be able to launch the interpreter
				and should contain the necessary arguments in order to allow
				KVIrc to pass the [i]commandline[/i] by appending it as the last parameter.
			!sw: -d | --direct
				Use no command interpreter at all: run the command directly.
				Takes precedence over -s.
			!sw: -q | --quiet
				Run quietly
		@description:
			[b]Overview[/b][br]
			Executes the <commandline> by passing it to a command interpreter.
			The <commandline> is executed asynchronously: this means that
			when exec returns the control to the next command, <commandline>
			may be still running.[br]
			[br]
			[b]The callback[/b][br]
			The <callback command> is triggered on several events related to the
			child process and it gets passed the following parameters:[br]
			$0 = <event cause>[br]
			$1 = <event parameter>[br]
			$2 = <magic data>[br]
			The first parameter specifies the event cause and contains one of the
			following strings: [i]stdout[/i], [i]stderr[/i], [i]terminated[/i], [i]started[/i] and [i]ping[/i].
			[b]By default (if no switches are used) only [i]stdout[/i] type events are triggered[/b].
			The second parameter depends on the event cause and contains data sensible
			to each event type. The third parameter is the eventual <magic data>
			passed to the exec command call.[br]
			[br]
			[b]Interacting with the process[/b][br]
			If you use [cmd]halt[/cmd] to terminate
			the callback then the slave process is killed immediately and
			no other callback events are triggered.[br] If you return some non empty string
			then this string will be written to the process stdin stream. This trick
			can be used to control interactive processes. Please note that you must
			include all the relevant carriage returns and newlines in the return value
			(see [fnc]$cr[/fnc] and [fnc]$lf[/fnc]).[br]
			[br]
			[b]Startup event[/b][br]
			If the -x switch is used then the startup event is triggered
			just after the process has been successfully launched.
			The $0 parameter passed to the callback contains the string [i]started[/i].
			Parameter $1 contains the pid of the slave process.[br]
			[br]
			[b]Stdout data event[/b][br]
			The stdout data event is triggered when the process prints some output
			on its stdout stream. This event is triggered by default and to disable
			it you must use the -n switch. $0 contains the string [i]stdout[/i].
			If the -b switch is not used then $1 contains a single line of process
			output with the trailing carriage return and/or line feed stripped.
			If -b is used then $1 contains the whole process output
			block (eventually empty) with all the cr/lf pairs.[br]
			[br]
			[b]Stderr data event[/b][br]
			The stderr data event is similar to the stdout one but there are three differences.
			The first one is that the stderr event is [b]not[/b] triggered by default: you must
			use the -e switch to enable it. The second difference is that $0
			contains [i]stderr[/i] instead of [i]stdout[/i]. The last difference is that $1 contains data
			coming from the slave process stderr stream.[br]
			[br]
			[b]Termination event[/b][br]
			The termination event is triggered after the slave process has terminated its
			execution. You must use the -t switch to enable it since it is
			disabled by default. $0 contains the string [i]terminated[/i]. $1 contains the process exit
			status value. (Note that if the process has crashed or has been terminated
			by an external signal then this value will be 0).[br]
			[br]
			[b]Ping event[/b][br]
			The ping event is triggered only if the -p=<timeout> switch is passed.[br]
			This event may be useful to monitor the process status while it is not
			emitting any output, to write data to its stdin stream (by the means of [cmd]return[/cmd])
			or simply to give some feedback to the user while the slave process is
			doing a long computation.[br]
			[br]
			[b]The extended scope variables[/b][br]
			The <callback command> has a set of [doc:data_types]extended scope variables[/doc]
			that conserve their value during the whole life time of the slave process.[br]
			These variables can be accessed through the %:<varname> syntax and are
			useful to store process private data between multiple <callback command> calls.[br]
			[b]Some words about the switches[/b][br]
			If the -b switch is used then the <callback command> is called only once
			for the events stdout and stderr (if enabled) with the complete output block from the process.
			With the -b switch the events stdout and stderr are triggered once even if the process
			emits no output.
			The -s=<interpreter> switch may be used to specify the path of the command interpreter
			that is [i]sh -c[/i] by default on UNIX machines and [i]cmd.exe /c[/i] on Windows.
			The interpreter executable is searched on the system PATH.
			If the process can't be started then a warning message is printed in the current window
			unless the -q (quiet) flag is used.[br]
			[br]
		@examples:
			[example]
			[comment]# Really simple example: print only the stdout of a slave process[/comment]
			exec("cat /proc/cpuinfo"){ echo $1; };
			[comment]# Now print only stderr: enable stderr and disable stdout[/comment]
			exec -e -n ("sed -senseless"){ echo $1; };
			[comment]# Do it another way: enable stderr and filter out stdout[/comment]
			exec -e ("sed -senseless"){ if($0 == "stderr")echo $1; }
			[comment]# Now enable all (almost) events and print them[/comment]
			exec -e -t -x ("cat /proc/cpuinfo && sed -senseless"){ echo [event:$0] $1; }
			[comment]# Now see what happens if -b is used[/comment]
			exec -b -e -t -x ("cat /proc/cpuinfo && sed -senseless"){ echo [event:$0] $1; }
			[comment]# Run an iterative script and kill it after 20 seconds[/comment]
			exec -k=20000 ("while true; do sleep 1; echo \"Tic\"; done"){ echo [event:$0] $1; }
			[comment]# Run a blocking process, kill it after 20 seconds[/comment]
			[comment]# and give feedback to the user by the means of ping[/comment]
			exec -k=20000 -p=1000 -t ("cat")
			{
				if($0 == "ping")echo "[event:$0] Please wait while doing a huge computation ..."
				else if($0 == "terminated")echo "[event:$0] OK, done :)"
			}
			[comment]# Do the same but this time use the extended scope vars[/comment]
			[comment]# Use also a nicer syntax[/comment]
			exec -k=20000 -p=1000 -t ("cat")
			{
				switch($0)
				{
					case("ping"):
					{
						if(%:x == 1)
						{
							%:x = 0;
							echo "Tic!"
						} else {
							%:x = 1;
							echo "Tac!"
						}
					}
					break;
					case("terminated"):
					{
						echo "OK, done :)"
					}
					break;
				}
			}
			[comment]# Again do the same but kill the process explicitly[/comment]
			exec -x -p=1000 -t ("cat")
			{
				switch($0)
				{
					case("started"):
					{
						[comment]# Initialize the counter[/comment]
						%:x = 10;
					}
					break;
					case("ping"):
					{
						echo %:x
						%:x--
						[comment]# When the counter reaches zero, kill the process with halt[/comment]
						if(%:x == 0)halt;
					}
					break;
					case("terminated"):
					{
						echo "Boom!"
					}
					break;
				}
			}
			[comment]# Now play with an interactive process[/comment]
			[comment]# WARNING: Please note that spam is illegal and generates bad karma[/comment]
			[comment]# Try it only with your own e-mail address as recipient[/comment]
			exec -s -k=60000 -t ("telnet my.mail.server.com 25")
			{
				if($0 == "started")
				{
					%:state = 0
					[comment]# Returning an empty string does not write to stdin[/comment]
					return
				}

				if($1 == "stderr")
				{
					echo "[stderr] $1"
					return
				}

				if($1 == "terminated")
				{
					echo "[process terminated]"
					return
				}

				echo "[stdout] $1"

				switch(%:state)
				{
					case(0):
					{
						[comment]# Waiting for 220 (ready)[/comment]
						if($str.match("220*",$1))
						{
							%:state++
							echo "Sending HELO..."
							return "HELO myhostname$cr$lf";
						}
					}
					break
					case(1):
					{
						[comment]# Waiting for 250 (after the HELO)[/comment]
						if($str.match("250*",$1))
						{
							%:state++
							echo "Sending MAIL..."
							return "MAIL From: <myname@mydomain.com>$cr$lf"
						} else {
							echo "HELO command not accepted: $1"
							halt
						}
					}
					break;
					case(2):
					{
						[comment]# Waiting for another 250 (MAIL accepted)[/comment]
						if($str.match("250*",$1))
						{
							%:state++
							echo "Sending RCPT..."
							return "RCPT To: <me@myself.org>$cr$lf"
						} else {
							echo "MAIL command not accepted: $1"
							halt
						}
					}
					break;
					case(3):
					{
						[comment]# Waiting for another 250 (RCPT accepted)[/comment]
						if($str.match("250*",$1))
						{
							%:state++
							echo "Sending DATA..."
							return "DATA$cr$lf"
						} else {
							echo "RCPT not accepted: $1"
							halt
						}
					}
					break;
					case(4):
					{
						[comment]# Waiting for 354 (OK, go on)[/comment]
						if($str.match("354*",$1))
						{
							%:state++
							echo "Sending body..."
							return "This is a test message :)$cr$lf$cr$lf.$cr$lf"
						} else {
							echo "Mail body not accepted: $1"
							halt
						}
					}
					break;
					case(5):
					{
						[comment]# We don't wait anymore :)[/comment]
						%:state++
						echo "Sending QUIT..."
						return "QUIT$cr$lf"
					}
					break;
					default:
					{
						[comment]# Usually the mail server closes the connection[/comment]
						%:state++
						if(%:state > 10)
						{
							[comment]# But if it does not in few messages[/comment]
							[comment]# Then force the process to die[/comment]
							halt
						}
					}
				}
			}
			[/example]
	*/

	KVSCCC(exec)
	{
		QString szCommandline;
		KviKvsVariant * pMagic;
		KVSCCC_PARAMETERS_BEGIN
		KVSCCC_PARAMETER("commandline", KVS_PT_NONEMPTYSTRING, 0, szCommandline)
		KVSCCC_PARAMETER("magic", KVS_PT_VARIANT, KVS_PF_OPTIONAL, pMagic)
		KVSCCC_PARAMETERS_END

		int f = 0;

		if(KVSCCC_pSwitches->find('t', "trigger-termination") != nullptr)
			f |= KVI_KVS_PROCESSDESCRIPTOR_TRIGGERTERMINATED;
		if(KVSCCC_pSwitches->find('n', "no-stdout") == nullptr)
			f |= KVI_KVS_PROCESSDESCRIPTOR_TRIGGERSTDOUT;
		if(KVSCCC_pSwitches->find('e', "trigger-stderr") != nullptr)
			f |= KVI_KVS_PROCESSDESCRIPTOR_TRIGGERSTDERR;
		if(KVSCCC_pSwitches->find('x', "trigger-startup") != nullptr)
			f |= KVI_KVS_PROCESSDESCRIPTOR_TRIGGERSTARTED;
		if(KVSCCC_pSwitches->find('b', "output-block") != nullptr)
			f |= KVI_KVS_PROCESSDESCRIPTOR_OUTPUTBYBLOCKS;
		if(KVSCCC_pSwitches->find('w', "bind-to-window") != nullptr)
			f |= KVI_KVS_PROCESSDESCRIPTOR_KILLIFNOWINDOW;
		if(KVSCCC_pSwitches->find('d', "direct") != nullptr)
			f |= KVI_KVS_PROCESSDESCRIPTOR_NOSHELL;

		QString szShell;
		KVSCCC_pSwitches->getAsStringIfExisting('s', "shell", szShell);

		kvs_int_t iPingTime = 0;
		kvs_int_t iMaxRunTime = 0;

		KviKvsVariant * pPing = KVSCCC_pSwitches->find('p', "trigger-ping");
		if(pPing)
		{
			if(!(pPing->asInteger(iPingTime) && iPingTime > 0))
			{
				KVSCCC_pContext->warning(__tr2qs_ctx("The specified ping time is invalid: assuming zero (no ping)", "kvs"));
				iPingTime = 0;
			}
		}

		KviKvsVariant * pKill = KVSCCC_pSwitches->find('k', "kill-after");
		if(pKill)
		{
			if(!(pKill->asInteger(iMaxRunTime) && iMaxRunTime > 0))
			{
				KVSCCC_pContext->warning(__tr2qs_ctx("The specified maximum run time is invalid: assuming zero (infinite)", "kvs"));
				iMaxRunTime = 0;
			}
		}

		KviKvsProcessDescriptorData * d = new KviKvsProcessDescriptorData;

		d->szCommandline = szCommandline;
		d->szShell = szShell;
		d->pWnd = KVSCCC_pContext->window();
		d->pMagic = pMagic ? new KviKvsVariant(*pMagic) : nullptr;
		d->iFlags = f;
		d->pCallback = new KviKvsScript(*KVSCCC_pCallback);
		d->iMaxRunTime = iMaxRunTime;
		d->iPingTimeout = iPingTime;

		KviKvsProcessAsyncOperation * op = new KviKvsProcessAsyncOperation(d);
		if(!op->start())
		{
			if(KVSCCC_pSwitches->find('q', "quiet") == nullptr)
				KVSCCC_pContext->warning(__tr2qs_ctx("Failed to start the process", "kvs"));
			delete op;
		}

		return true;
	}

	/*
		@doc: privateimpl
		@title:
			privateimpl
		@type:
			command
		@short:
			Adds a private implementation of a function
		@syntax:
			privateimpl(<object_handle>,<function_name>)<implementation>
		@description:
			Adds a private implementation of function <function_name> to the
			existing object designed by <object_handle>.
			<implementation> must be a valid command sequence.[br]
			Side note:[br]
			This command can not successfully implement
			the [i]constructor[/i] function since it must be called
			after this one has already been executed.[br]
			To implement a constructor you MUST write your own class definition.[br]
		@seealso:
			[cmd]class[/cmd],
			[doc:objects]Objects documentation[/doc]
	*/

	KVSCCC(privateimpl)
	{
		kvs_hobject_t hObject;
		QString szFunctionName;
		KVSCCC_PARAMETERS_BEGIN
		KVSCCC_PARAMETER("object_handle", KVS_PT_HOBJECT, 0, hObject)
		KVSCCC_PARAMETER("function_name", KVS_PT_NONEMPTYSTRING, 0, szFunctionName)
		KVSCCC_PARAMETERS_END

		KviKvsObject * o = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
		if(!o)
		{
			KVSCCC_pContext->error(__tr2qs_ctx("The specified object does not exist", "kvs"));
			return false;
		}

		o->registerPrivateImplementation(szFunctionName, KVSCCC_pCallback->code());

		return true;
	}

	/*
		@doc: timer
		@title:
			timer
		@type:
			command
		@short:
			Starts a timer
		@syntax:
			timer [-s] [-p] (<name>,<delay_in_msecs>[,<callback_param1>[,<callback_param2>[,...]]])
			{
				<callback_command>
			}
		@switches:
			!sw: -s | --single-shot
			Causes the timer to trigger only once (single shot timer)
			!sw: -p | --persistent
			Creates a persistent timer bound to any existing window
		@description:
			Starts a new timer named <name> with the specified delay (in milliseconds).[br]
			The timer periodically calls the specified <callback_command> code passing the
			eventual <callback_param> strings as positional parameters.[br]
			If a timer with the same name already exists, it is replaced by this one.[br]
			[b]The <callback_command> is evaluated at timer [i]shot[/i] time and [b]not[/b] while
			this command is being parsed. This means that the identifiers that you put
			inside <callback_command> will [b]not[/b] have the current values.[/b]
			The values will be assigned at timer [i]shot[/i] time.[br]
			This is a common scripters error and problem: if it is not clear, look at the examples below.[br]
			The timer is bound to the window in that this command is executed in.[br]
			If the window gets destroyed, the timer is stopped; unless the -p switch is used.[br]
			The -p switch causes the timer to be persistent across the application and exists until
			the last window has been closed: it is basically rebound to another (random) window when the
			original window is destroyed.[br]
			The -s switch causes this timer to trigger only once: it will be automatically destroyed after that.[br]
			The time has an associated set of [doc:data_structures]extended scope variables[/doc]:
			the variables that begin with [i]%:[/i] have their life extended to the whole [i]life[/i] of the timer.[br]
			Using a very low delay is a common method to perform some background processing: you
			basically split a huge job in small slices and execute them when the timer is triggered
			until you run out of slices. A delay of 0 will cause the timer to be called whenever
			KVIrc has some [i]idle time[/i] to spend.
			On the other hand, remember that timers are precious resources: many timers running
			with a very low delay will cause KVIrc to slow down.[br]
			Since all the KVIrc timers share the same namespace it is a good idea to use
			descriptive timer names: a timer named [i]a[/i] is likely to be used by two or more scripts
			at once causing one (or both) of them to fail.[br]
			A timer can be stopped at any time by using the [cmd]killtimer[/cmd] command.
		@seealso:
			[cmd]killtimer[/cmd]
		@examples:
			[example]
				[comment]# Just a plain timer[/comment]
				timer(test,1000){ echo "Hello!"; }
				[comment]# Now watch the timer running[/comment]
				killtimer test
				[comment]# Single shot timer[/comment]
				timer -s (test,1000){ echo "This will fire only once!"; }
				[comment]# The call above is equivalent to[/comment]
				timer(test,1000){ echo "This will file only once!"; killtimer test; }
				[comment]# Callback parameters: consider the following code[/comment]
				%parameter = "some string value"
				echo "Before calling /timer \%parameter is \"%parameter\""
				timer -s (test,1000,%parameter){ echo "inside the callback \%parameter is \"%parameter\" but \$0 is \"$0\""; }
				[comment]# watch the timer running, and note the behaviour of the %parameter variable[/comment]
				killtimer test
				[comment]# Use the extended scope timer variables[/comment]
				timer(test,1000)
				{
					[comment]# Use the extended scope %:count variable to keep track[/comment]
					[comment]# of the times that this timer has been called[/comment]
					[cmd]if[/cmd]("%:count" == "")%:count = 1
					else %:count++
					[cmd]echo[/cmd] "This timer has fired %:count times"
					if(%:count == 10)
					{
						# This will kill the current timer, we don't need to specify the name
						[cmd]killtimer[/cmd]
					}
				}
				[comment]# Use isTimer to check if the timer exists[/comment]
				[cmd]echo[/cmd] [fnc]$isTimer[/fnc](test)
				[comment]# Repeat the command above after the 10th timeout...[/comment]
			[/example]
			[example]
				[comment]# Execute a command at a precise time[/comment]
				%secsfrom= $($date("H")*3600+$date("M")*60+$date("S")); #now
				%secsto= $(7*3600+10*60+0); #end time: 07:10:00
				%secsdiff=$(%secsto-%secsfrom); #difference
				if(%secsdiff <= 0)
				{
					#we're past that time for today
					%secsdiff+=86400; #60 secs * 60 mins * 24 hours
				}
				# timer wants msecs
				%msecs = $(%secsdiff*1000);
				timer -s -p (reminder, %msecs)
				{
					echo "Hey man it's ten past seven am, time to wake up!"
				}
			[/example]
	*/

	KVSCCC(timer)
	{
		KviKvsVariant * vName = KVSCCC_pParams->first();
		KviKvsVariant * vDelay = KVSCCC_pParams->next();

		if(!vName || vName->isEmpty())
		{
			KVSCCC_pContext->error(__tr2qs_ctx("Missing timer name", "kvs"));
			return false;
		}

		QString szName;
		vName->asString(szName);

		if(!vDelay)
		{
			KVSCCC_pContext->error(__tr2qs_ctx("Missing timeout delay", "kvs"));
			return false;
		}

		kvs_int_t iDelay;
		if(!vDelay->asInteger(iDelay))
		{
			KVSCCC_pContext->error(__tr2qs_ctx("The timeout delay didn't evaluate to an integer", "kvs"));
			return false;
		}

		KviKvsTimer::Lifetime lt;

		if(KVSCCC_pSwitches->find('s', "single-shot"))
			lt = KviKvsTimer::SingleShot;
		else if(KVSCCC_pSwitches->find('p', "persistent"))
			lt = KviKvsTimer::Persistent;
		else
			lt = KviKvsTimer::WindowLifetime;

		// prepare the callback parameters
		KviKvsVariantList * l = new KviKvsVariantList();
		l->setAutoDelete(true);

		KviKvsVariant * v = KVSCCC_pParams->next();
		while(v)
		{
			l->append(new KviKvsVariant(*v)); // copy
			v = KVSCCC_pParams->next();
		}

		if(!KviKvsTimerManager::instance()->addTimer(szName, lt, KVSCCC_pContext->window(), iDelay, new KviKvsScript(*KVSCCC_pCallback), l))
		{
			KVSCCC_pContext->error(__tr2qs_ctx("Unable to add the timer: insufficient system resources", "kvs"));
			return false;
		}

		return true;
	}

	void init()
	{
		KviKvsKernel * pKern = KviKvsKernel::instance();

#define _REGCMD(__cmdName, __routine)                                                        \
	{                                                                                        \
		KviKvsCoreCallbackCommandExecRoutine * r = new KviKvsCoreCallbackCommandExecRoutine; \
		r->proc = KVI_PTR2MEMBER(KviKvsCoreCallbackCommands::__routine);                     \
		pKern->registerCoreCallbackCommandExecRoutine(QString(__cmdName), r);                \
	}

		_REGCMD("ahost", ahost);
		_REGCMD("awhois", awhois);
		_REGCMD("alias", alias);
		_REGCMD("button", button);
		_REGCMD("event", event);
		_REGCMD("exec", exec);
		_REGCMD("privateimpl", privateimpl);
		_REGCMD("function", alias);
		_REGCMD("timer", timer);

#undef _REGCMD
	}
};
