//=============================================================================
//
//   File : KviKvsEventTable.cpp
//   Creation date : Mon 23 Feb 2004 03:23:18 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviKvsEvent.h"
#include "KviKvsEventTable.h"
#include "KviKvsEventManager.h"

/*
 * Note: the definition order of the EVENT(..) entries in this file
 * must match the order of event ids defined in the header file.
 */

#define EVENT(_name, _parm) KviKvsEvent(_name, _parm)

KviKvsEvent KviKvsEventManager::m_appEventTable[KVI_KVS_NUM_APP_EVENTS] = {
	// Application
	/*
		@doc: onkvircstartup
		@type:
			event
		@title:
			OnKVIrcStartup
		@short:
			Triggered on KVIrc startup
		@parameters:
			None
		@window:
			Console of the first IRC context
		@description:
			This event is triggered on KVIrc startup.
			It is bound to the console of the first created IRC context.
			This is a little tricky, since this event is triggered
			before the related [event:onirccontextcreated]OnIRCContextCreated[/event] event.
			This is to make OnApplicationStartup be the absolutely first
			event triggered at all.
			This event is a good place to load your script configuration
			and eventually initialize your variables.
		@seealso:
			[event:onkvircshutdown]OnKVIrcShutdown[/event]
	*/

	EVENT("OnKVIrcStartup", ""),

	/*
		@doc: onkvircshutdown
		@type:
			event
		@title:
			OnKVIrcShutdown
		@short:
			Triggered on KVIrc shutdown
		@parameters:
			None
		@window:
			Console of the last IRC context
		@description:
			This event is triggered on KVIrc shutdown.
			It is bound to the console of the last existing IRC context.
			This is a little tricky, since this event is triggered
			immediately after the on [event:onirccontextdestroyed]OnIRCContextDestroyed[/event] event,
			so in theory the console should be already destroyed.
			This is to make OnApplicationShutdown be the absolutely last event
			triggered at all.
			This event is a good place to save your script configuration.
		@seealso:
			[event:onkvircstartup]OnKVIrcStartup[/event]
	*/

	EVENT("OnKVIrcShutdown", ""),

	/*
		@doc: onirccontextcreated
		@type:
			event
		@title:
			OnIRCContextCreated
		@short:
			Triggered when a new IRC context is created
		@parameters:
			None
		@window:
			Console of the created IRC context
		@description:
			This event is triggered when a new IRC context is created.
			It is bound to the console of the related IRC context,
			that is the first window that is created.
		@seealso:
			[event:onirccontextdestroyed]OnIRCContextDestroyed[/event]
	*/

	EVENT("OnIRCContextCreated", ""),

	/*
		@doc: onirccontextdestroyed
		@type:
			event
		@title:
			OnIRCContextDestroyed
		@short:
			Triggered when an IRC context is destroyed
		@parameters:
			None
		@window:
			Console of the destroyed IRC context
		@description:
			This event is triggered when an IRC context is being destroyed.
			It is bound to the console of the related IRC context,
			that is the last window of the context that is destroyed.
		@seealso:
			[event:onirccontextcreated]OnIRCContextCreated[/event]
	*/

	EVENT("OnIRCContextDestroyed", ""),

	/*
		@doc: onframewindowcreated
		@type:
			event
		@title:
			OnFrameWindowCreated
		@short:
			Triggered when a new frame window is opened
		@parameters:
			None
		@window:
			First console in the frame window
		@description:
			This event is triggered in the first console of a new frame window.
			A frame window is the toplevel window that contains the consoles
			and the other MDI child windows.
		@seealso:
			[event:onframewindowdestroyed]OnFrameWindowDestroyed[/event]
	*/

	EVENT("OnFrameWindowCreated", ""),

	/*
		@doc: onframewindowdestroyed
		@type:
			event
		@title:
			OnFrameWindowDestroyed
		@short:
			Triggered when a frame window is being destroyed
		@parameters:
			None
		@window:
			Last console in the frame window
		@description:
			This event is triggered in the last console of a frame window that is
			being destroyed. In fact the frame window is not destroyed yet, but it
			is going to: there would be no way to trigger this event after the
			last frame window destruction.
		@seealso:
			[event:onframewindowcreated]OnFrameWindowCreated[/event]
	*/

	EVENT("OnFrameWindowDestroyed", ""),

	/*
		@doc: onurl
		@type:
			event
		@title:
			OnURL
		@short:
			Triggered when an URL appears in one of the output windows
		@parameters:
			$0 = URL
		@window:
			The window that the URL appeared in
		@description:
			This event is triggered every time that an URL is going to appear
			in a KVIrc window output widget. You cannot stop it or modify it.
			This event is good for implementing an URL catcher.[br]
			WARNING: If you attempt to [i]echo $0[/i] inside this event, you will cause an infinite loop.
	*/

	EVENT("OnURL",
	    "$0 = URL"),

	/*
		@doc: onerror
		@type:
			event
		@title:
			OnError
		@short:
			Triggered when an ERROR message is received from the server
		@parameters:
			$0 = Message source
			$1 = Error reason
		@window:
			Console
		@description:
			Triggered when an ERROR message is received from the server.[br]
			$0 is the message (server) source but be aware that it often happens to be empty.[br]
			Calling 'halt' in this event will stop the default output.[br]
	*/

	EVENT("OnError",
	    "$0 = Message source\n"
	    "$1 = Error reason"),

	/*
		@doc: onaccelkeypressed
		@type:
			event
		@title:
			OnAccelKeyPressed
		@short:
			Triggered when an accelerator key was pressed
		@parameters:
			$0 = Keys
		@window:
			Active window
		@description:
			An accelerator key has been pressed by the user.[br]
			The key sequence that the user has pressed is passed in $0 as a
			human readable string.[br]
			The keys that actually work with KVIrc are:[br]
			F2 to F12, F1 to F12 with Shift key pressed, 0 to 9 with Ctrl key pressed.[br]
			This means that you can set up 33 [i]quick[/i] aliases... that's really more than
			a user can remember.[br]
			Note: this event won't be called for keys already bound to actions.
			If you need more keys, mail me, I'll see what I can do :)[br]
	*/

	EVENT("OnAccelKeyPressed",
	    "$0 = Keys"),

	/*
		@doc: onhighlight
		@type:
			event
		@title:
			OnHighlight
		@short:
			Triggered when a channel or query message is going to be highlighted
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Message
			$4 = Highlight word
			$5 = Message type
			$6 = Is action
		@window:
			Channel or query window
		@description:
			A channel or query message is going to be highlighted.[br]
			The message may be either a PRIVMSG, a NOTICE or a CTCP ACTION.[br]
			The highlight may be either caused by a [i]highlight word[/i] or by your own nickname
			(if that option was specified).[br]
			$5 contains the original color/icon set of the message that depends on
			the type of the message itself.[br]
			Since the CTCP ACTION messages generally have different format of output,
			for your convenience $6 contains 1 if the message was caused by a
			CTCP ACTION or 0 otherwise.[br]
			If you call [cmd]halt[/cmd] in this event the message output will be completely disabled
			(thus, KVIrc assumes that YOU have provided the necessary output and feedback to the user).[br]
			A common error here is to call halt and re-echo the unmodified output; [b]this
			can cause another OnHighlight event to be triggered and create an infinite loop![/b]
			[cmd]echoprivmsg[/cmd] [b]-n[/b] is the command you probably want.
			To discriminate if this event has been triggered within a channel or in a private message,
			check the values of [fnc]$channel[/fnc] or [fnc]$query[/fnc].
	*/

	EVENT("OnHighlight",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Message\n"
	    "$4 = Highlight word\n"
	    "$5 = Message type\n"
	    "$6 = Is action"),

	/*
		@doc: onwindowactivated
		@type:
			event
		@title:
			OnWindowActivated
		@short:
			Triggered when a window has just been activated
		@parameters:
			none
		@window:
			Any window
		@description:
			This event is triggered when a KVIrc MDI window gets activated.[br]
			This might be a good place to update your toolbar button status (for example).[br]
	*/

	EVENT("OnWindowActivated", ""),

	/*
		@doc: onnotifyonline
		@type:
			event
		@title:
			OnNotifyOnline
		@short:
			A user in the notify list is on-line
		@parameters:
			$0 = Nickname
		@window:
			Console or active window of the IRC context
		@description:
			This event is triggered when one of the users in your [doc:notify_list]notify[/doc] lists
			is detected to be ON-Line.[br]
			The event is triggered in the console window or the active window of the IRC context depending
			on the user options.[br]
			(Note: If you call [cmd]halt[/cmd] in this event, you will stop the [i]Nick is on IRC[/i] output,
			but this is rather a bad idea since KVIrc gives a lot of additional information to the
			user with some notify-methods).[br]
			This is a good place to play a sound or attract the user attention in some other way.[br]

	*/

	EVENT("OnNotifyOnline",
	    "$0 = Nickname"),

	/*
		@doc: onnotifyoffline
		@type:
			event
		@title:
			OnNotifyOffline
		@short:
			A user in the notify list went off-line
		@parameters:
			$0 = Nickname
		@window:
			Console or active window of the IRC context
		@description:
			This event is triggered when one of the users in your [doc:notify_list]notify[/doc] lists
			has just left IRC.[br]
			The event is triggered in the console window or the active window of the IRC context depending
			on the user options.[br]
			(Note: If you call [cmd]halt[/cmd] in this event, you will stop the [i]Nick has left IRC[/i] output,
			but this is rather a bad idea since KVIrc gives a lot of additional information to the
			user with some notify-methods).[br]
			This is a good place to play a sound or attract the user attention in some other way.[br]
	*/

	EVENT("OnNotifyOffline",
	    "$0 = Nickname"),

	/*
		@doc: onping
		@type:
			event
		@title:
			OnPing
		@short:
			Server ping handler
		@parameters:
			$0 = server
			$1 = Ping message parameters
		@window:
			Console
		@description:
			Triggered when a PING message was received from a server.[br]
			Calling 'halt' in this event will stop the informational message about 'ping->pong'.[br]
	*/

	EVENT("OnPing",
	    "$0 = Server\n"
	    "$1 = Message parameters"),

	/*
		@doc: onpong
		@type:
			event
		@title:
			OnPong
		@short:
			Server pong handler
		@parameters:
			$0 = server
			$1 = Pong message parameters
		@window:
			Console
		@description:
			Triggered when a PONG message was received from a server.[br]
			Calling 'halt' in this event will stop the informational message about the pong'.[br]
	*/

	EVENT("OnPong",
	    "$0 = Server\n"
	    "$1 = Pong message parameters"),

	/*
		@doc: onlagalarmtimeup
		@type:
			event
		@title:
			OnLagAlarmTimeUp
		@short:
			High-Lag Alarm Started
		@parameters:
			$0 = server
			$1 = Lag value in milliseconds
		@window:
			Console
		@description:
		Triggered when the Lag on the server tied to this console is higher than the configured Lag-Alarm time.
	*/

	EVENT("OnLagAlarmTimeUp",
	    "$0 = Server\n"
	    "$1 = Lag time in milliseconds"),

	/*
		@doc: onlagalarmtimedown
		@type:
			event
		@title:
			OnLagAlarmTimeDown
		@short:
			High-Lag Alarm Stopped
		@parameters:
			$0 = server
			$1 = Lag value in milliseconds
		@window:
			Console
		@description:
		Triggered when the Lag on the server tied to this console is lower than the configured Lag-Alarm time but was higher in the previous lag check.
	*/

	EVENT("OnLagAlarmTimeDown",
	    "$0 = Server\n"
	    "$1 = Lag time in milliseconds"),

	/*
		@doc: onlagcheck
		@type:
			event
		@title:
			OnLagCheck
		@short:
			The lag metering engine has checked the lag
		@parameters:
			$0 = server
			$1 = Lag value in milliseconds
		@window:
			Console
		@description:
			Triggered when the lag metering engine periodically calculates the current lag (the interval is configurable under settings -> IRC -> Tools -> Lag).
	*/

	EVENT("OnLagCheck",
	    "$0 = Server\n"
	    "$1 = Lag time in milliseconds"),

	/*
		@doc: ontextinput
		@type:
			event
		@title:
			OnTextInput
		@short:
			The user has just entered some text in the text input
		@parameters:
			$0 = Text
		@window:
			window in that the text is entered
		@description:
			Triggered when the user enters some text in the text input
			window and presses return.
			Calling [cmd]halt[/cmd] will stop the automatic text processing
			and the result depends on the window the text is typed in.
			In channel/query and console windows the text will [b]not[/b] be sent to
			the IRC server. This event might be useful for performing
			substitutions in the text sent to the server. If you stop the
			text processing in a channel or query window then the local
			echo of the message will be omitted too. You might want to
			realize the echo by the means of the [cmd]echoprivmsg[/cmd] command.
			This event is [b]not[/b] triggered when the user enters a command
			in the input window.
		@seealso:
			[cmd]echoprivmsg[/cmd]
	*/

	EVENT("OnTextInput",
	    "$0 = Text"),

	/*
		@doc: onnotifylistdefaultactionrequest
		@type:
			event
		@title:
			OnNotifyListDefaultActionRequest
		@short:
			Notify list double click customization
		@parameters:
			$0 = Nickname
		@window:
			The window that the user has clicked in
		@description:
			This event is triggered when a user has double clicked on a nickname in
			the notify list (console window).[br]
			You should use this event to do some significant action associated with the double click.[br]
			A good example might be a [cmd]whois[/cmd] query or a [cmd]dcc.chat[/cmd]
	*/

	EVENT("OnNotifyListDefaultActionRequest",
	    "$0 = Nickname"),

	/*
		@doc: onwallops
		@type:
			event
		@title:
			OnWallops
		@short:
			Triggered when a WALLOPS message is received from the server
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Message text
		@window:
			channel
		@description:
			Triggered when a WALLOPS message is received from the server.
			If you call [cmd]halt[/cmd] the message output will be stopped.
		@seealso:
	*/

	EVENT("OnWallops",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Message text"),

	/*
		@doc: OnIgnoredMessage
		@type:
			event
		@title:
			OnIgnoredMessage
		@short:
			Triggered when a message is ignored.
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Message target
			$4 = Message
			$5 = Message tags
		@window:
			any
		@description:
			Triggered when a message is ignored.
	*/

	EVENT("OnIgnoredMessage",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Message target\n"
	    "$4 = Message\n"
	    "$5 = Message tags"),

	/*
		@doc: onservernotice
		@type:
			event
		@title:
			OnServerNotice
		@short:
			A server NOTICE have been issued.
		@parameters:
			$0 = Source nickname
			$1 = Message
		@window:
			Console window
		@description:
			Someone has issued a server NOTICE.
		@seealso:
			[event:onchannelnotice]OnChannelNotice[/event]
	*/

	EVENT("OnServerNotice",
	    "$0 = Source nickname\n"
	    "$1 = Message"),

	// Connection
	/*
		@doc: onircconnectionestablished
		@type:
			event
		@title:
			OnIRCConnectionEstablished
		@short:
			Triggered when a new IRC connection has been established
		@parameters:
			None
		@window:
			Console of the related IRC context
		@description:
			This event is triggered immediately after an IRC connection
			has been established. No data has been sent to the server yet
			and you have not logged in: it is not a good idea
			attempting to send data to the server (READ: [b]This is NOT
			a good place for joining channels[/b]).
			Your nickname is still [i]undefined[/i] too.
			If you're looking for the appropriate moment for sending
			data to the server look at the [event:onirc]OnIRC[/event] event.
			This event is a good place for initializing connection related
			variables. By calling [cmd]halt[/cmd] in this event you will stop the
			[i]Connection established[/i] output.
		@seealso:
			[event:onircconnectionterminated]OnIRCConnectionTerminated[/event]
	*/

	EVENT("OnIRCConnectionEstablished", ""),

	/*
		@doc: onircconnectionterminated
		@type:
			event
		@title:
			OnIRCConnectionTerminated
		@short:
			Triggered when an IRC connection has been terminated
		@parameters:
			None
		@window:
			Console of the related IRC context
		@description:
			This event is triggered immediately after an IRC connection
			has been terminated. The socket is already closed, so you
			can't send data to the server. You can still read
			the connection parameters like the server name or
			network name. By calling [cmd]halt[/cmd] in this
			event you will stop the [i]Connection terminated...[/i] output.
		@seealso:
			[event:onircconnectionestablished]OnIRCConnectionEstablished[/event]
	*/

	EVENT("OnIRCConnectionTerminated", ""),

	/*
		@doc: onirc
		@type:
			event
		@title:
			OnIRC
		@short:
			Triggered when the login operations are terminated
		@parameters:
			None
		@window:
			Console of the related IRC context
		@description:
			This event is triggered immediately after the login
			operations have been terminated. From now on you are
			[i]completely on IRC[/i]: your nickname has been defined
			and the server is ready to accept commands from you.
			This is a good place to join the [i]preferred channels[/i].
			By calling [cmd]halt[/cmd] in this event you will
			stop the [i]Login operations complete...[/i] output.
		@seealso:
			[event:onircconnectionestablished]OnIRCConnectionEstablished[/event]
			[event:onircconnectionterminated]OnIRCConnectionTerminated[/event]
	*/

	EVENT("OnIRC", ""),

	/*
		@doc: onnetsplit
		@type:
			event
		@title:
			OnNetsplit
		@short:
			A netsplit has been detected
		@parameters:
			$0 = Server1
			$1 = Server2
		@window:
			console
		@description:
			Triggered when KVIrc detects a netsplit by looking at a user's quit message.
			The two parameters passed are the two servers that lost the link and were
			signaled in that particular quit message.[br]
			The actual netsplit detector in the KVIrc core uses the following rule
			(extracted from the eggdrop bot code): [i]determine if signoff string matches "%.% %.%",
			and only one space[/i].[br]
			[b]Note that this may be not accurate and KVIrc CAN be fooled by specially
			formatted quit messages that aren't really netsplits.[/b][br]
			KVIrc tries to avoid duplicate netsplit warnings thus this event will NOT
			be triggered for EVERY quit in a netsplit: you will get it once for every
			different set of servers that lost the link.[br]
			Determining where the network link has been exactly broken may be a hard
			task since the quit messages you see may not involve
			the servers on the split boundary.
	*/

	EVENT("OnNetsplit",
	    "$0 = Server1\n"
	    "$1 = Server2"),

	/*
		@doc: ondisconnectrequest
		@type:
			event
		@title:
			OnDisconnectRequest
		@short:
			Triggered when the user requests a disconnection
		@parameters:
			None
		@window:
			console
		@description:
			Triggered when the user requests a disconnection from
			a server by pressing the [i]disconnect[/i] button, by closing
			the console or closing KVIrc. It is your last chance
			to send some meaningful data to the server before the QUIT message
			is sent. You can't prevent the user from disconnecting - [cmd]halt[/cmd] does nothing here.[br]
			This event is useful to either give a salute to all the users
			you were talking with or maybe to send some commands to the
			bouncer you're connected to. Note that you can override the QUIT
			message sent by KVIrc by sending it yourself first.[br]
		@seealso:
	*/

	EVENT("OnDisconnectRequest", ""),

	/*
		@doc: OnHTTPGetTerminated
		@type:
			event
		@title:
			OnHTTPGetTerminated
		@short:
			A http.get command has terminated execution
		@parameters:
			$0 = Status
			$1 = Remote URL
			$2 = Local file
			$3 = Magic identifier
		@window:
			Console
		@description:
			Triggered when a [cmd]http.get[/cmd] file download has terminated execution.
			If the transfer has been successful then $0 will contain the value 1 (true),
			otherwise will contain the value 0. $1 contains the remote URL and $2 the local
			path on disk of the saved file. $3 contains the magic identifier passed to
			the http.get command by the means of the -i switch.
	*/

	EVENT("OnHTTPGetTerminated",
	    "$0 = Status\n"
	    "$1 = Remote URL\n"
	    "$2 = Local file\n"
	    "$3 = Magic identifier"),

	/*
		@doc: onunhandledliteral
		@type:
			event
		@title:
			OnUnhandledLiteral
		@short:
			An unhandled literal server message has been received
		@parameters:
			$0 = Source mask
			$1 = Message (literal)
			$2- = Parameters
		@window:
			Console window
		@description:
			An unhandled/unrecognized literal server message has been received.[br]
			The KVIrc core code hasn't been able to recognize it nor handle it in any way.[br]
			If you call [cmd]halt[/cmd] you will stop the standard or warning message output.[br]
			For server numerics there is no [i]unhandled[/i] event, you should use the RAW numeric events to handle them.[br]
		@seealso:
	*/

	EVENT("OnUnhandledLiteral",
	    "$0 = Source mask\n"
	    "$1 = Message\n"
	    "$2- = Parameters"),

	/*
		@doc: onoutboundtraffic
		@type:
			event
		@title:
			OnOutboundTraffic
		@short:
			Triggered on outbound traffic.[br]
			USE THIS EVENT WITH REAL CAUTION: an improper use can lead to many problems to KVIrc; especially avoid the use of the 'halt' command.
		@parameters:
			$0 = Outbound string
		@window:
			any
		@description:
			This event is triggered on outbound traffic.
		@seealso:
			[event:ontextinput]OnTextInput[/event]
			[event:onchannelmessage]OnChannelMessage[/event]
	*/

	EVENT("OnOutboundTraffic",
	    "$0 = Outbound string"),

	// Popups
	/*
		@doc: onchannelpopuprequest
		@type:
			event
		@title:
			OnChannelPopupRequest
		@short:
			Triggered when the channel text view is right-clicked
		@parameters:
			None
		@window:
			Channel window
		@description:
			This event is triggered when the user clicks the channel
			text view with the right mouse button. It is a good place
			to activate a [cmd]popup[/cmd] menu :)
		@seealso:
			[event:ondccchatpopuprequest]OnDCCChatPopupRequest[/event],
			[event:onconsolepopuprequest]OnConsolePopupRequest[/event],
			[event:onquerypopuprequest]OnQueryPopupRequest[/event]
	*/

	EVENT("OnChannelPopupRequest", ""),

	/*
		@doc: onconsolepopuprequest
		@type:
			event
		@title:
			OnConsolePopupRequest
		@short:
			Triggered when the console text view is right-clicked
		@parameters:
			None
		@window:
			Console window
		@description:
			This event is triggered when the user clicks the console
			text view with the right mouse button. It is a good place
			to activate a [cmd]popup[/cmd] menu :)
		@seealso:
			[event:ondccchatpopuprequest]OnDCCChatPopupRequest[/event],
			[event:onchannelpopuprequest]OnChannelPopupRequest[/event],
			[event:onquerypopuprequest]OnQueryPopupRequest[/event]
	*/

	EVENT("OnConsolePopupRequest", ""),

	/*
		@doc: onquerypopuprequest
		@type:
			event
		@title:
			OnQueryPopupRequest
		@short:
			Triggered when the query text view is right-clicked
		@parameters:
			None
		@window:
			Query
		@description:
			This event is triggered when the user clicks the query
			text view with the right mouse button. It is a good place
			to activate a [cmd]popup[/cmd] menu :)
		@seealso:
			[event:ondccchatpopuprequest]OnDCCChatPopupRequest[/event],
			[event:onconsolepopuprequest]OnConsolePopupRequest[/event],
			[event:onchannelpopuprequest]OnChannelPopupRequest[/event],
	*/

	EVENT("OnQueryPopupRequest", ""),

	/*
		@doc: onwindowpopuprequest
		@type:
			event
		@title:
			OnWindowPopupRequest
		@short:
			Triggered when the user middle-clicks in a text view (or right-clicks with the Ctrl key pressed).
		@parameters:
			None
		@window:
			The window that the user has clicked in.
		@description:
			This event is here to allow easy customization of the window popup (it was builtin in KVIrc version prior to 3.0.0).[br]
 			Actually it is triggered when the user middle-clicks in a text view (or right-clicks with the Ctrl key pressed).[br]
 			You should use it to popup your own version of the window popup that should interface the logging and window-behaviour functions.
	*/

	EVENT("OnWindowPopupRequest", ""),

	/*
		@doc: onhostlinkpopuprequest
		@type:
			event
		@title:
			OnHostLinkPopupRequest
		@short:
			Triggered when a user right clicks on a hostname link in the text view.
		@parameters:
			$0 = Actual name
			$1 = Visible name
			$2 = Command name
		@window:
			The window that the user has clicked in.
		@description:
			This event is triggered when a user has right clicked on a hostname link in the text view.
			The double click link action is builtin and it causes KVIrc to perform a DNS query on the hostname.
			This is a good place to show a popup menu with some operations that can be done on the hostname like
			[i]telnet[/i], [i]traceroute[/i], some special kind of DNS lookup (maybe an xterm with nslookup or sth).
	*/

	EVENT("OnHostLinkPopupRequest",
	    "$0 = Actual name\n"
	    "$1 = Visible name\n"
	    "$2 = Command name"),

	/*
		@doc: onurllinkpopuprequest
		@type:
			event
		@title:
			OnURLLinkPopupRequest
		@short:
			URL link popup customization
		@parameters:
			$0 = Actual name
			$1 = Visible name
			$2 = Command name
		@window:
			The window that the user has clicked in
		@description:
			Actual name is equal to the command name if command name is not null.
			Visible name is the link text (as it shown by user)
			Command name is set if the visible name differs with the actual link target (e.g. probably contains mIRC colors). It is equal to the actual target name.
			This event is triggered when a user has right clicked on an URL link in
			the text view. The double click link action is builtin and it causes KVIrc
			to open the URL with a browser (typically your OS default browser).[br]
			This is a good place to show a [cmd]popup[/cmd] menu with some operations
			that can be performed with the URL, like bookmarking in some way or opening
			with a specific browser.[br]
	*/

	EVENT("OnURLLinkPopupRequest",
	    "$0 = Actual name\n"
	    "$1 = Visible name\n"
	    "$2 = Command name"),

	/*
		@doc: onserverlinkpopuprequest
		@type:
			event
		@title:
			OnServerLinkPopupRequest
		@short:
			Server link popup customization
		@parameters:
			$0 = Actual name
			$1 = Visible name
			$2 = Command name
		@window:
			The window that the user has clicked in
		@description:
			Actual name is equal to the command name if command name is not null.
			Visible name is the link text (as it shown by user)
			Command name is set if the visible name differs with the actual link target (e.g. probably contains mIRC colors). It is equal to the actual target name.
			This event is triggered when a user has right clicked on an server name link in
			the text view. The double click link action is builtin and it causes KVIrc
			to query the MOTD of the server.[br]
			This is a good place to show a [cmd]popup[/cmd] menu with some operations
			that can be performed with the server name, like querying stats or links.[br]
			Please note that the server name is generally [b]not[/b] a hostname: some parts of
			it are often hidden. The name of the server is generally valid only within
			the IRC network that you are connected to.[br]
	*/

	EVENT("OnServerLinkPopupRequest",
	    "$0 = Actual name\n"
	    "$1 = Visible name\n"
	    "$2 = Command name"),

	/*
		@doc: onchannellinkpopuprequest
		@type:
			event
		@title:
			OnChannelLinkPopupRequest
		@short:
			Channel link popup customization
		@parameters:
			$0 = Actual name
			$1 = Visible name
			$2 = Command name
		@window:
			The window that the user has clicked in
		@description:
			Actual name is equal to the command name if command name is not null.
			Visible name is the link text (as it shown by user)
			Command name is set if the visible name differs with the actual link target (e.g. probably contains mIRC colors). It is equal to the actual target name.
			This event is triggered when a user has right clicked on an channel link in
			the text view. The double click link action is builtin and it causes KVIrc
			to join the channel (unless the user is already on).[br]
			This is a good place to show a [cmd]popup[/cmd] menu with some operations
			that can be performed with the channel name like bookmarking, joining with a password,
			performing a [cmd]who[/cmd] etc..
	*/

	EVENT("OnChannelLinkPopupRequest",
	    "$0 = Actual name\n"
	    "$1 = Visible name\n"
	    "$2 = Command name"),

	/*
		@doc: onnicklinkpopuprequest
		@type:
			event
		@title:
			OnNickLinkPopupRequest
		@short:
			Nick link popup customization
		@parameters:
			$0 = Actual name
			$1 = Visible name
			$2 = Command name
		@window:
			The window that the user has clicked in
		@description:
			Actual name is equal to the command name if command name is not null.
			Visible name is the link text (as it shown by user)
			Command name is set if the visible name differs with the actual link target (e.g. probably contains mIRC colors). It is equal to the actual target name.
			This event is triggered when a user has right clicked on an [i]unknown[/i] nickname link in
			the text view.[br]
			This is a good place to show a [cmd]popup[/cmd] menu with some operations
			that can be performed with the nickname like querying, sending a file by DCC or opening a dcc.chat.
			Please note that [b]not all[/b] the nickname links will receive this event.[br]
			The nickname links that appear in the text view of the channel that they are actually in
			are handled by the [event:onchannelnickpopuprequest]OnChannelNickPopupRequest[/event] event.[br]
			The nickname links that appear in the text view of the query in that they are a remote end
			are handled by the [event:onquerynickpopuprequest]OnQueryNickPopupRequest[/event] event.[br]
	*/

	EVENT("OnNickLinkPopupRequest",
	    "$0 = Actual name\n"
	    "$1 = Visible name\n"
	    "$2 = Command name"),

	/*
		@doc: onquerynickpopuprequest
		@type:
			event
		@title:
			OnQueryNickPopupRequest
		@short:
			Query nicklist popup customization
		@parameters:
			$0 = Actual name
			$1 = Visible name
			$2 = Command name
		@window:
			The window that the user has clicked in
		@description:
			Actual name is equal to the command name if command name is not null.
			Visible name is the link text (as it shown by user)
			Command name is set if the visible name differs with the actual link target (e.g. probably contains mIRC colors). It is equal to the actual target name.
			This event is triggered when a user has right clicked on a set of nicknames in
			the query target list.[br]
			This is a good place to show a [cmd]popup[/cmd] menu with some operations
			that can be performed with the nickname like sending a file by DCC, opening a dcc.chat or
			performing a WHOIS.[br]
	*/

	EVENT("OnQueryNickPopupRequest",
	    "$0 = Actual name\n"
	    "$1 = Visible name\n"
	    "$2 = Command name"),

	/*
		@doc: onchannelnickpopuprequest
		@type:
			event
		@title:
			OnChannelNickPopupRequest
		@short:
			Allows customization of the nick-on-channel context popup
		@parameters:
			$0 = Nick list
		@window:
			Channel window
		@description:
			This event is triggered when the user right-clicks in the channel user list
			or on a nickname link in the channel text view.[br]
			The first parameter ($0) contains a comma separated list of selected nicknames (if it is a user list click)
			or a single nickname (if it is a link).[br]
			In both cases all the nicknames are currently on the channel.[br]
			You should popup a popup-menu with a set of possible channel-related actions,
			like op/deop, voice/devoice, whois etc...
		@seealso:
			[event:onchannelnickdefaultactionrequest]OnChannelNickDefaultActionRequest[/event]
	*/

	EVENT("OnChannelNickPopupRequest",
	    "$0 = Nickname list"),

	/*
		@doc: onnotifylistpopuprequest
		@type:
			event
		@title:
			OnNotifyListPopupRequest
		@short:
			Notify list context popup customization
		@parameters:
			$0 = Nickname list
		@window:
			The window that the user has clicked in
		@description:
			This event is triggered when a user has right clicked on a list of nicknames in
			the notify list (console window).[br]
			This is a good place to show a [cmd]popup[/cmd] with some actions that can be performed
			on the nicknames like querying, performing a WHOIS lookup or something similar.[br]
	*/

	EVENT("OnNotifyListPopupRequest",
	    "$0 = Nickname list"),

	/*
		@doc: ondccchatpopuprequest
		@type:
			event
		@title:
			OnDCCChatPopupRequest
		@short:
			Triggered when the DCC chat text view is right-clicked
		@parameters:
			$0 = DCC session ID
		@window:
			DCC Chat window
		@description:
			This event is triggered when the user clicks the DCC chat
			text view with the right mouse button. It is a good place
			to activate a [cmd]popup[/cmd] menu :)
			You can retrieve the session information by calling the [module:dcc]dcc[/module].*
			module functions and by passing the DCC session ID parameter.
		@seealso:
			[event:onconsolepuprequest]OnConsolePopupRequest[/event],
			[event:onchannelpopuprequest]OnChannelPopupRequest[/event],
			[event:onquerypopuprequest]OnQueryPopupRequest[/event]
	*/

	EVENT("OnDCCChatPopupRequest",
	    "$0 = DCC session ID"),

	// Channels
	/*
		@doc: onchannelwindowcreated
		@type:
			event
		@title:
			OnChannelWindowCreated
		@short:
			A channel window has just been opened
		@parameters:
			None
		@window:
			Channel window
		@description:
			Triggered when a channel window has just been created
			Note that the channel is still empty: no users have joined it.
		@seealso:
			[event:onquerywindowcreated]OnQueryWindowCreated[/event]
	*/

	EVENT("OnChannelWindowCreated", ""),

	/*
		@doc: onchannelsync
		@type:
			event
		@title:
			OnChannelSync
		@short:
			Triggered when all channel information have been received
		@parameters:
			$0 = Channel sync time in milliseconds
		@window:
			Channel
		@description:
			This event is triggered after you have joined a channel
			and all information have been received.[br]
			This includes the channel users list, the ban list,
			the channel mode, the topic and eventually
			the invite and ban exception lists.[br]
			Calling halt in this even stops the [i]channel synchronized[/i] output.
			(the output will be halted anyway if [fnc]$option[/fnc](boolShowChannelSyncTime) evaluates to false)
		@seealso:
			[event:ondccchatpopuprequest]OnDCCChatPopupRequest[/event],
			[event:onconsolepopuprequest]OnConsolePopupRequest[/event],
			[event:onchannelpopuprequest]OnChannelPopupRequest[/event]
	*/

	EVENT("OnChannelSync",
	    "$0 = Channel sync time in milliseconds"),

	/*
		@doc: onchannelmessage
		@type:
			event
		@title:
			OnChannelMessage
		@short:
			A message has been received from the channel
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Message
			$4 = Target mode prefixes
			$5 = Was encrypted
			$6 = Message tags
		@window:
			Channels window
		@description:
			Triggered when a channel message is received
			$0!$1@$2 is the source of the message, $3 is the message text
			and $4 are the eventual mode prefixes added to the target channel (i.e:
			if the message is only for channel operators then you will get the string @ in $4).
			$5 will be [b]1[/b] if the message was encrypted and [b]0[/b] otherwise.
			$6 is a hash of the current message tags, if available.
		@seealso:
			[event:onquerymessage]OnQueryMessage[/event]
			[event:ondccchatmessage]OnDCCChatMessage[/event]
	*/

	EVENT("OnChannelMessage",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Message\n"
	    "$4 = Target mode prefixes\n"
	    "$5 = Was encrypted\n"
	    "$6 = Message tags"),

	/*
		@doc: onchannelnotice
		@type:
			event
		@title:
			OnChannelNotice
		@short:
			A NOTICE have been issued to a channel
		@parameters:
			$0 = Source nickname
			$1 = Message
			$2 = Target
			$3 = Was encrypted
			$4 = Message tags
		@window:
			Channel window
		@description:
			Someone has issued a NOTICE with a channel as his target
			For normal channel notices $2 parameter will only a channel name.
			For channel op or channel voice notices it will contain also the leading '@' or '+' flag.
			Please note that this convention is different from the one used in [event:onchannelprivmsg]OnChannelMessage[/event]:
			the incompatibility is here for historical reasons: it is not a big deal so we're not fighting it :)
			$3 will be [b]1[/b] if the message was encrypted and [b]0[/b] otherwise.
			$4 is a hash of the current message tags, if available.
		@seealso:
			[event:onservernotice]OnServerNotice[/event]
	*/

	EVENT("OnChannelNotice",
	    "$0 = Source nickname\n"
	    "$1 = Message\n"
	    "$2 = Target\n"
	    "$3 = Was encrypted\n"
	    "$4 = Message tags"),

	// Queries
	/*
		@doc: onquerymessage
		@type:
			event
		@title:
			OnQueryMessage
		@short:
			A message has been received from a query
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Message
			$4 = Was encrypted
			$5 = Message tags
		@window:
			Query or console window.
		@description:
			Triggered when a query message is received.[br]
			If you [cmd]return[/cmd] [fnc]$false[/fnc] in this event, the message output will be stopped.[br]
			This event is triggered in the query window associated to the source nickname, or in
			the IRC context console window if the query window has not been created for some reason.
			Note that you can't stop the query window creation with this event: you must
			use [event:onquerywindowrequest]OnQueryWindowRequest[/event] for that purpose.
			Note also that you may not receive this event if the anti-spam option is in use and
			KVIrc has detected that this might be a spam message. This is usually not an issue
			but if you care about it then write to pragma at kvirc dot net asking to add a OnSpam event.
			$4 will be [b]1[/b] if the message is encrypted and [b]0[/b] otherwise.
		@seealso:
			[event:onquerywindowrequest]OnQueryWindowRequest[/event]
			[event:onquerywindowcreated]OnQueryWindowCreated[/event]
			[event:onquerytargetadded]OnQueryTargetAdded[/event]
			[event:onchannelmessage]OnChannelMessage[/event]
			[event:ondccchatmessage]OnDCCChatMessage[/event]
	*/

	EVENT("OnQueryMessage",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Message\n"
	    "$4 = Was encrypted\n"
	    "$5 = Message tags"),

	/*
		@doc: onquerynotice
		@type:
			event
		@title:
			OnQueryNotice
		@short:
			A private notice has been received
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Message
			$4 = Was encrypted
			$5 = Message tags
		@window:
			Query window or console
		@description:
			Triggered when a private notice is received.[br]
			If you call [cmd]halt[/cmd] in this event, the message output will be stopped and if the message was going to cause a query window creation, the window will [b]not[/b] be created.
			$4 will be [b]1[/b] if the message is encrypted and [b]0[/b] otherwise.
		@seealso:
			[event:onchannelmessage]OnChannelMessage[/event]
			[event:onchannelnotice]OnChannelNotice[/event]
	*/

	EVENT("OnQueryNotice",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Message\n"
	    "$4 = Was encrypted\n"
	    "$5 = Message tags"),

	/*
		@doc: onquerywindowrequest
		@type:
			event
		@title:
			OnQueryWindowRequest
		@short:
			A new query window is going to be opened because a remote message has been received
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Message
			$4 = Message tags
		@window:
			console window
		@description:
			Triggered when a PRIVMSG or NOTICE has been received from a remote user and a query
			for that user doesn't exist yet. KVIrc has already checked its internal conditions
			and you have the last word on the query creation. If you call halt in this event
			you will prevent the creation of the query window, in all other cases the window
			will be created. This is useful for filtering out spam messages that popup unwanted
			windows in front of the user. You have also the option of creating the query window
			yourself (by calling [cmd]query[/cmd]). This may be useful if you want to create
			the query windows in a particular state (minimized, positioned in some special place
			or maybe undocked if some conditions are verified).
			The next event you will catch related to this query window will be
			[event:onquerywindowcreated]OnQueryWindowCreated[/event] and [event:onquerytargetadded]OnQueryTargetAdded[/event].
			After that you will get [event:onquerymessage]OnQueryMessage[/event] with exactly
			the parameters passed to this event.
	*/

	EVENT("OnQueryWindowRequest",
	    "$0 = Source nickname\n"
	    "$1 = Source user\n"
	    "$2 = Source hostname\n"
	    "$3 = Message\n"
	    "$4 = Message tags"),

	/*
		@doc: onquerywindowcreated
		@type:
			event
		@title:
			OnQueryWindowCreated
		@short:
			A query window has just been opened
		@parameters:
			None
		@window:
			Query window
		@description:
			Triggered when a query window has just been created
			Note that the query is still empty: it has no targets.
			If you want to trigger some events in response to a query
			creation by some remote user then you probably need
			[event:onquerywindowrequest]OnQueryWindowRequest[/event] or
			[event:onquerytargetadded]OnQueryTargetAdded[/event].
		@seealso:
			[event:onchannelwindowcreated]OnChannelWindowCreated[/event]
			[event:onquerywindowrequest]OnQueryWindowRequest[/event]
			[event:onquerytargetadded]OnQueryTargetAdded[/event]
	*/

	EVENT("OnQueryWindowCreated", ""),

	/*
		@doc: onquerytargetadded
		@type:
			event
		@title:
			OnQueryTargetAdded
		@short:
			A new query target has been added
		@parameters:
			$0 = Nickname
			$1 = Username (may be *)
			$2 = Hostname (may be *)
		@window:
			query window
		@description:
			Triggered when a new remote target has been added to a query window.
			This is the event to catch when you want to perform some specific actions
			when someone queries you.
			KVIrc supports multiple target queries and thus, this event is triggered also
			when a new query target is added by using [cmd]addtarget[/cmd].
	*/

	EVENT("OnQueryTargetAdded",
	    "$0 = Nickname\n"
	    "$1 = Username (may be *)\n"
	    "$2 = Hostname (may be *)"),

	/*
		@doc: OnQueryFileDropped
		@type:
			event
		@title:
			OnQueryFileDropped
		@short:
			A file is dropped in a query
		@parameters:
			$0 = Source nickname
			$1 = File dropped
		@window:
			Query
		@description:
			Triggered when a file is dropped in a query window.[br]
			$target is the nick.[br]
	*/

	EVENT("OnQueryFileDropped",
	    "$0 = Source nickname\n"
	    "$1 = File dropped"),

	// Actions
	/*
		@doc: onmeaway
		@type:
			event
		@title:
			OnMeAway
		@short:
			Local user sets the AWAY status
		@parameters:
			$0 = Server message trailing
		@window:
			Console
		@description:
			Triggered when the local user enters the AWAY state.
			Calling 'halt' in this event disables the server [i]You have been marked as being away[/i] message.[br]
		@seealso:
			[event:onmeback]OnMeBack[/event]
	*/

	EVENT("OnMeAway",
	    "$0- = Server message trailing"),

	/*
		@doc: onmeback
		@type:
			event
		@title:
			OnMeBack
		@short:
			Local user cancels the AWAY status
		@parameters:
			$0 = Away start time
			$1- = Server message trailing
		@window:
			Console
		@description:
			Triggered when the local user leaves the AWAY state.[br]
			The $0 parameter contains the [b]UNIX time[/b] of the away status start.[br]
			You can obtain the current [b]UNIX time[/b] from the function [fnc]$unixtime[/fnc].[br]
			Calling 'halt' in this event disables the server [i]You are no longer marked as being away[/i] message.[br]
		@seealso:
			[event:onmeaway]OnMeAway[/event]
	*/

	EVENT("OnMeBack",
	    "$0 = Away start time\n"
	    "$1- = Server message trailing"),

	/*
		@doc: onjoin
		@type:
			event
		@title:
			OnJoin
		@short:
			Non-local user joining a channel
		@parameters:
			$0 = Nickname
			$1 = Username
			$2 = Hostname
		@window:
			Channel
		@description:
			Triggered just after a non-local user has joined a channel.[br]
			Since this event is triggered in the channel window,
			you can get the channel name by using the [fnc]$target[/fnc] function.[br]
			Calling 'halt' in this event will stop the join message output.[br]
		@seealso:
			[event:onmejoin]OnMeJoin[/event]
	*/

	EVENT("OnJoin",
	    "$0 = Nickname\n"
	    "$1 = Username\n"
	    "$2 = Hostname"),

	/*
		@doc: onmejoin
		@type:
			event
		@title:
			OnMeJoin
		@short:
			Local user joining a channel
		@parameters:
			None
		@window:
			Channel
		@description:
			Triggered just after the local user has joined a channel.[br]
			Since this event is triggered in the channel window,
			you can get the channel name by using the [fnc]$target[/fnc] function.[br]
			Calling 'halt' in this event will stop the join message output.[br]
		@seealso:
			[event:onjoin]OnJoin[/event]
	*/

	EVENT("OnMeJoin", ""),

	/*
		@doc: onpart
		@type:
			event
		@title:
			OnPart
		@short:
			Non-local user is leaving a channel
		@parameters:
			$0 = Nickname
			$1 = Username
			$2 = Hostname
			$3 = Part message
		@window:
			Channel
		@description:
			Triggered when a NON-local user is leaving a channel.[br]
			The PART message has already been processed by the server, so
			the user is effectively no longer on the channel.
			On the other side, it might be useful to access the user-related channel data from
			this event, thus the handler is called effectively just BEFORE the
			user effectively leaves the channel (and thus BEFORE the user-related channel data gets
			destroyed).[br]
			Since this event is triggered in the channel window,
			you can get the channel name by using the [fnc]$target[/fnc] function.[br]
			Calling 'halt' in this event will stop the part message output.[br]
		@seealso:
			[event:onmejoin]OnMeJoin[/event], [event:OnJoin]OnJoin[/event],
			[event:onmepart]OnMePart[/event]
	*/

	EVENT("OnPart",
	    "$0 = Nickname\n"
	    "$1 = Username\n"
	    "$2 = Hostname\n"
	    "$3 = Part message"),

	/*
		@doc: onmepart
		@type:
			event
		@title:
			OnMePart
		@short:
			Local user is leaving a channel
		@parameters:
			$0 = Part message
		@window:
			Channel
		@description:
			Triggered when the local user is leaving a channel.[br]
			The PART message has already been processed by the server, so
			the user is effectively no longer on the channel.
			On the other side, it might be useful to access the channel data from
			this event, thus the handler is called effectively just BEFORE the
			user effectively leaves the channel (and thus BEFORE the channel window gets
			destroyed).[br]
			Since this event is triggered in the channel window,
			you can get the channel name by using the [fnc]$target[/fnc] function.[br]
			Calling 'halt' in this event will stop the part message output.[br]
		@seealso:
			[event:onmejoin]OnMeJoin[/event], [event:OnJoin]OnJoin[/event],
			[event:onpart]OnPart[/event]
	*/

	EVENT("OnMePart",
	    "$0 = Part message"),

	/*
		@doc: onkick
		@type:
			event
		@title:
			OnKick
		@short:
			Non-local user is being kicked from a channel
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Kicked nickname
			$4 = Kick reason
		@window:
			Channel
		@description:
			Triggered when a non-local user is being kicked from a channel.[br]
			The KICK message has already been processed by the server, so
			the user is effectively no longer on the channel.
			On the other side, it might be useful to access the user-related channel data from
			this event, thus the handler is called effectively just BEFORE the
			user is effectively removed from the channel internal structures.[br]
			Since this event is triggered in the channel window,
			you can get the channel name by using the [fnc]$target[/fnc] function.[br]
			Calling 'halt' in this event will stop the kick message output.[br]
		@seealso:
			[event:onmekick]OnMeKick[/event]
	*/

	EVENT("OnKick",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Kicked nickname\n"
	    "$4 = Reason"),

	/*
		@doc: onmekick
		@type:
			event
		@title:
			OnMeKick
		@short:
			Local user is being kicked from a channel
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Kick reason
		@window:
			Channel
		@description:
			Triggered when the local user has been kicked from a channel.[br]
			The KICK message has already been processed by the server, so
			the user is effectively no longer on the channel.
			On the other side, it might be useful to access the user-related channel data from
			this event, thus the handler is called effectively just BEFORE the
			user is effectively removed from the channel internal structures
			(and thus BEFORE the channel data itself gets destroyed).[br]
			Since this event is triggered in the channel window,
			you can get the channel name by using the [fnc]$target[/fnc] function.[br]
			Calling 'halt' in this event will stop the kick message output.[br]
		@seealso:
			[event:onkick]OnKick[/event]
	*/

	EVENT("OnMeKick",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Reason"),

	/*
		@doc: ontopic
		@type:
			event
		@title:
			OnTopic
		@short:
			Topic change on a channel
		@parameters:
			$0 = Nickname
			$1 = Username
			$2 = Hostname
			$3 = New topic string
		@window:
			Channel
		@description:
			Triggered when a TOPIC message is received for a given channel.[br]
			The topic change has not been executed effectively in the current window
			so you can still access the old topic by the channel functions.[br]
			The new topic is passed as fourth parameter and will effectively
			become the current topic just after this event handler has been executed.[br]
			Since this event is triggered in the channel window,
			you can get the channel name by using the [fnc]$target[/fnc] function.[br]
			Calling 'halt' in this event will stop the topic message output.[br]
		@seealso:
			[event:onmejoin]OnMeJoin[/event]
	*/

	EVENT("OnTopic",
	    "$0 = Nickname\n"
	    "$1 = Username\n"
	    "$2 = Hostname\n"
	    "$3 = Topic"),

	/*
		@doc: onquit
		@type:
			event
		@title:
			OnQuit
		@short:
			An user is leaving IRC
		@parameters:
			$0 = Nickname
			$1 = Username
			$2 = Hostname
			$3 = Quit message
			$4 = Channels
		@window:
			Console
		@description:
			Triggered when a user is quitting IRC.[br]
			The QUIT message has already been processed by the server, so
			the user is effectively no longer on IRC.
			On the other side, it might be useful to access the user-related data from
			this event, thus the handler is called effectively just BEFORE the
			user is effectively removed from the IRC user list (and thus from the channels and query windows).[br]
			The fifth parameter ($4) contains the list of channels on that the user was
			just before quitting.
			Calling 'halt' in this event will stop the quit message output (in ALL the windows!).[br]
		@seealso:
			[event:onpart]OnPart[/event],
			[event:onmepart]OnMePart[/event]
		@examples:
			[example]
				foreach(%cname, $str.split(",", $4))echo -w=$channel(%cname, $context) User $0 is quitting...
			[/example]
	*/

	EVENT("OnQuit",
	    "$0 = Nickname\n"
	    "$1 = Username\n"
	    "$2 = Hostname\n"
	    "$3 = Quit message\n"
	    "$4 = Channels"),

	// IRC modes
	/*
		@doc: OnChannelModeChange
		@type:
			event
		@title:
			OnChannelModeChange
		@short:
			The channel mode has been changed
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Mode flags
			$4 = Mode parameter string
		@window:
			channel
		@description:
			Triggered when a channel mode has been changed, AFTER all the mode specific events
			were triggered. $3 contains the unparsed mode flags (-o+b) and $4 contains
			the unparsed mode parameter string (you need to split it!).
	*/

	EVENT("OnChannelModeChange",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Mode flags\n"
	    "$4 = Mode params"),

	/*
		@doc: onusermodechange
		@type:
			event
		@title:
			OnUserModeChange
		@short:
			The local user has changed mode
		@parameters:
			$0 = Mode flags
		@window:
			Console
		@description:
			Triggered when the local user has changed his user mode.[br]
			The first parameter is the changed mode flag with a leading '+' or '-' sign:
			if the mode was removed it will be something like [i]-x[/i], otherwise something like [i]+x[/i].[br]
			Calling 'halt' in this event will stop the [i]mode[/i] message output.[br]
	*/

	EVENT("OnUserModeChange",
	    "$0 = Mode flags"),

	/*
		@doc: onban
		@type:
			event
		@title:
			OnBan
		@short:
			Someone has set a +b flag on the channel
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Ban mask
		@window:
			Channel window
		@description:
			Triggered when someone sets a +b flag on the channel
		@seealso:
			[event:onunban]OnUnban[/event]
	*/

	EVENT("OnBan",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Ban mask"),

	/*
		@doc: onunban
		@type:
			event
		@title:
			OnUnban
		@short:
			Someone has set a -b flag on the channel
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Ban mask
		@window:
			Channel window
		@description:
			Triggered when someone sets a -b flag on the channel
		@seealso:
			[event:onban]OnBan[/event]
	*/

	EVENT("OnUnBan",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Ban mask"),

	/*
		@doc: onmeban
		@type:
			event
		@title:
			OnMeBan
		@short:
			Local user is being banned in a channel
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Ban mask
		@window:
			Channel
		@description:
			Triggered when someone has set a +b flag that matches your mask (bans you) in the channel.[br]
		@seealso:
			[event:onmeunban]OnMeUnban[/event]
	*/

	EVENT("OnMeBan",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Ban mask"),

	/*
		@doc: onmeunban
		@type:
			event
		@title:
			OnMeUnban
		@short:
			Local user is being de-banned in a channel
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Ban mask
		@window:
			Channel
		@description:
			Triggered when someone has set a -b flag that matches your mask (unbans you) in the channel.[br]
		@seealso:
			[event:onmeban]OnMeBan[/event]
	*/

	EVENT("OnMeUnban",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Ban mask"),

	/*
		@doc: onbanexception
		@type:
			event
		@title:
			OnBanException
		@short:
			Someone has set a +e flag on the channel
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Ban exception mask
		@window:
			Channel window
		@description:
			Triggered when someone sets a +e (Ban Exception) flag on the channel
		@seealso:
			[event:onbanexceptionremove]OnBanExceptionRemove[/event]
	*/

	EVENT("OnBanException",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Ban exception mask"),

	/*
		@doc: onbanexceptionremove
		@type:
			event
		@title:
			OnBanExceptionRemove
		@short:
			Someone has set a -e flag on the channel
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Ban exception mask
		@window:
			Channel window
		@description:
			Triggered when someone sets a -e (Removed a Ban Exception) flag on the channel
		@seealso:
			[event:onbanexception]OnBanException[/event]
	*/

	EVENT("OnBanExceptionRemove",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Ban exception mask"),

	/*
		@doc: onmebanexception
		@type:
			event
		@title:
			OnMeBanException
		@short:
			Someone has set a +e flag on the local user
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Mask
		@window:
			Channel window
		@description:
			Triggered when someone sets a +e (puts a Ban Exception) flag on the local user in the active channel
		@seealso:
			[event:onmebanexceptionremove]OnMeBanExceptionRemove[/event]
	*/

	EVENT("OnMeBanException",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Mask"),

	/*
		@doc: onmebanexceptionremove
		@type:
			event
		@title:
			OnMeBanExceptionRemove
		@short:
			Someone has set a -e flag on the local user
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Mask
		@window:
			Channel window
		@description:
			Triggered when someone sets a -e (removes a Ban Exception) flag on the local user in the active channel
		@seealso:
			[event:onmebanexception]OnMeBanException[/event]
	*/

	EVENT("OnMeBanExceptionRemove",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Mask"),

	/*
		@doc: oninvite
		@type:
			event
		@title:
			OnInvite
		@short:
			The local user has received an invitation
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Channel
		@window:
			Active window/console
		@description:
			Triggered when someone invites the local user to join a channel
	*/

	EVENT("OnInvite",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Channel"),

	/*
		@doc: oninviteexception
		@type:
			event
		@title:
			OnInviteException
		@short:
			A +I mode has been set
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Target mask
		@window:
			Channels window
		@description:
			Someone has set mode +I on a channel
		@seealso:
			[event:oninviteexceptionremove]OnInviteExceptionRemove[/event]
	*/

	EVENT("OnInviteException",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Target mask"),

	/*
		@doc: oninviteexceptionremove
		@type:
			event
		@title:
			OnInviteExceptionRemove
		@short:
			Mode -I has been set
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Target mask
		@window:
			Channels window
		@description:
			Someone has set mode -I on a channel
		@seealso:
			[event:oninviteexceptionremove]OnInviteExceptionRemove[/event]
	*/

	EVENT("OnInviteExceptionRemove",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Target mask"),

	/*
		@doc: onmeinviteexception
		@type:
			event
		@title:
			OnMeInviteException
		@short:
			A +I mode has been set on the local user
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Mask
		@window:
			Channels window
		@description:
			Someone has set mode +I on a channel with a target mask that matches
			the local user mask
		@seealso:
			[event:onmeinviteexceptionremove]OnMeInviteExceptionRemove[/event]
	*/

	EVENT("OnMeInviteException",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Mask"),

	/*
		@doc: onmeinviteexceptionremove
		@type:
			event
		@title:
			OnMeInviteExceptionRemove
		@short:
			Move -I has been set
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Mask
		@window:
			Channels window
		@description:
			Someone has set mode -I on a channel with a target mask that matches
			the local user mask
		@seealso:
			[event:onmeinviteexceptionremove]OnMeInviteExceptionRemove[/event]
	*/

	EVENT("OnMeInviteExceptionRemove",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Mask"),

	/*
		@doc: onlimitset
		@type:
			event
		@title:
			OnLimitSet
		@short:
			The channel users limit has been set
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Limit
		@window:
			Channels window
		@description:
			Triggered when someone change or sets the active channel user limit (+l)
		@seealso:
			[event:onlimitunset]OnLimitUnset[/event]
	*/

	EVENT("OnLimitSet",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Limit"),

	/*
		@doc: onlimitunset
		@type:
			event
		@title:
			OnLimitUnSet
		@short:
			The channel users limit has been unset
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
		@window:
			Channels window
		@description:
			Triggered when someone unsets the active channel user limit (-l)
		@seealso:
			[event:onlimitunset]OnLimitUnset[/event]
	*/

	EVENT("OnLimitUnset",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname"),

	/*
		@doc: onkeyset
		@type:
			event
		@title:
			OnKeySet
		@short:
			The channel users key has been set
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Key
		@window:
			Channels window
		@description:
			Triggered when someone change or sets the active channel access key (+k)
		@seealso:
			[event:onkeyunset]OnKeyUnset[/event]
	*/

	EVENT("OnKeySet",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Key"),

	/*
		@doc: onkeyunset
		@type:
			event
		@title:
			OnKeyUnset
		@short:
			The channel users key has been unset
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
		@window:
			Channels window
		@description:
			Triggered when someone unsets the active channel access key (-k)
		@seealso:
			[event:onkeyunset]OnKeyUnset[/event]
	*/

	EVENT("OnKeyUnset",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname"),

	/*
		@doc: onnickchange
		@type:
			event
		@title:
			OnNickChange
		@short:
			Someone has changed his nickname
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = New nickname
		@window:
			Console window
		@description:
			Triggered when someone has changed his nickname.
			The change has already been processed by the server, but not by the local KVIrc database.
		@seealso:
			[event:onmenickchange]OnMeNickChange[/event]
	*/

	EVENT("OnNickChange",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = New nickname"),

	/*
		@doc: onmenickchange
		@type:
			event
		@title:
			OnMeNickChange
		@short:
			The local user has changed his nickname
		@parameters:
			$0 = Old nickname
			$1 = New nickname
		@window:
			Console window
		@description:
			Triggered when the local user is going to change his nickname.
			The change has already been processed by the server, but not by the local KVIrc database.
		@seealso:
			[event:onnickchange]OnNickChange[/event]
	*/

	EVENT("OnMeNickChange",
	    "$0 = Old nickname\n"
	    "$1 = New nickname"),

	/*
		@doc: onchanowner
		@type:
			event
		@title:
			OnChanOwner
		@short:
			Someone has given a user the +q flag
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Target nickname
		@window:
			Channels window
		@description:
			Triggered when someone sets a +q (has made Channel Owner) flag on a user of the active channel.
		@seealso:
			[event:ondechanowner]OnDeChanOwner[/event]
	*/

	EVENT("OnChanOwner",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Target nickname"),

	/*
		@doc: ondechanowner
		@type:
			event
		@title:
			OnDeChanOwner
		@short:
			Someone has given a user the -q flag
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Target nickname
		@window:
			Channels window
		@description:
			Triggered when someone sets a -q (DeChanOwner) flag on a user in the active channel.
		@seealso:
			[event:onchanowner]OnChanOwner[/event]
	*/

	EVENT("OnDeChanOwner",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Target nickname"),

	/*
		@doc: onmechanowner
		@type:
			event
		@title:
			OnMeChanOwner
		@short:
			Someone has given the +q flag to the local user
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
		@window:
			Channels window
		@description:
			Triggered when someone sets a +q (has made Channel owner) flag on the local user in the active channel.
		@seealso:
			[event:onmedechanowner]OnMeDeChanOwner[/event]
	*/

	EVENT("OnMeChanOwner",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname"),

	/*
		@doc: onmedechanowner
		@type:
			event
		@title:
			OnMeDeChanOwner
		@short:
			Someone has given the -q flag to the local user
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
		@window:
			Channels window
		@description:
			Triggered when someone sets a -q (has removed channel owner status) flag on the local user in the active channel.
		@seealso:
			[event:onmechanowner]OnMeChanOwner[/event]
	*/

	EVENT("OnMeDeChanOwner",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname"),

	/*
		@doc: onchanadmin
		@type:
			event
		@title:
			OnChanAdmin
		@short:
			Someone has given a user the +a flag
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Target nickname
		@window:
			Channels window
		@description:
			Triggered when someone sets a +a (has made Channel administrator) flag on a user of the active channel.
		@seealso:
			[event:ondechanadmin]OnDeChanAdmin[/event]
	*/

	EVENT("OnChanAdmin",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Target nickname"),

	/*
		@doc: ondechanadmin
		@type:
			event
		@title:
			OnDeChanAdmin
		@short:
			Someone has given a user the -a flag
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Target nickname
		@window:
			Channels window
		@description:
			Triggered when someone sets a -a (DeChanAdmin) flag on a user in the active channel.
		@seealso:
			[event:onchanadmin]OnChanAdmin[/event]
	*/

	EVENT("OnDeChanAdmin",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Target nickname"),

	/*
		@doc: onmechanadmin
		@type:
			event
		@title:
			OnMeChanAdmin
		@short:
			Someone has given the +a flag to the local user
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
		@window:
			Channels window
		@description:
			Triggered when someone sets a +a (has made Channel administrator) flag on the local user in the active channel.
		@seealso:
			[event:onmedeop]OnMeDeChanAdmin[/event]
	*/

	EVENT("OnMeChanAdmin",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname"),

	/*
		@doc: onmedechanadmin
		@type:
			event
		@title:
			OnMeDeChanAdmin
		@short:
			Someone has given the -a flag to the local user
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
		@window:
			Channels window
		@description:
			Triggered when someone sets a -a (has removed channel administrator status) flag on the local user in the active channel.
		@seealso:
			[event:onmeop]OnMeOp[/event]
	*/

	EVENT("OnMeDeChanAdmin",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname"),

	/*
		@doc: onop
		@type:
			event
		@title:
			OnOp
		@short:
			Someone has given a user the +o flag
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Opped nickname
		@window:
			Channels window
		@description:
			Triggered when someone sets a +o (has made Op) flag on a user of the active channel.
		@seealso:
			[event:ondeop]OnDeOp[/event]
	*/

	EVENT("OnOp",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Opped nickname"),

	/*
		@doc: ondeop
		@type:
			event
		@title:
			OnDeOp
		@short:
			Someone has given a user the -o flag
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Deopped nickname
		@window:
			Channels window
		@description:
			Triggered when someone sets a -o (DeOp) flag on a user in the active channel.
		@seealso:
			[event:onop]OnOp[/event]
	*/

	EVENT("OnDeOp",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Deopped nickname"),

	/*
		@doc: onmeop
		@type:
			event
		@title:
			OnMeOp
		@short:
			Someone has given the +o flag to the local user
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
		@window:
			Channels window
		@description:
			Triggered when someone sets a +o (has made Op) flag on the local user in the active channel.
		@seealso:
			[event:onmedeop]OnMeDeOp[/event]
	*/

	EVENT("OnMeOp",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname"),

	/*
		@doc: onmedeop
		@type:
			event
		@title:
			OnMeDeOp
		@short:
			Someone has given the -o flag to the local user
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
		@window:
			Channels window
		@description:
			Triggered when someone sets a -o (has deoped) flag on the local user in the active channel.
		@seealso:
			[event:onmeop]OnMeOp[/event]
	*/

	EVENT("OnMeDeOp",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname"),

	/*
		@doc: onhalfop
		@type:
			event
		@title:
			OnHalfOp
		@short:
			Someone has given a user the +h flag
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Halfopped nickname
		@window:
			Channels window
		@description:
			Triggered when someone sets a +h flag on a user of the active channel.
		@seealso:
			[event:ondehalfop]OnDeHalfOp[/event]
	*/

	EVENT("OnHalfOp",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Halfopped nickname"),

	/*
		@doc: ondehalfop
		@type:
			event
		@title:
			OnDeHalfOp
		@short:
			Someone has given a user the -h flag
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Dehalfopped nickname
		@window:
			Channels window
		@description:
			Triggered when someone sets a -h (dehalfop) flag on a user in the active channel.
		@seealso:
			[event:onhalfop]OnHalfOp[/event]
	*/

	EVENT("OnDeHalfOp",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Dehalfopped nickname"),

	/*
		@doc: onmehalfop
		@type:
			event
		@title:
			OnMeHalfOp
		@short:
			Someone has given the local user the +h flag
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
		@window:
			Channels window
		@description:
			Triggered when someone sets a +h (halfop) flag for the local user in the active channel.
		@seealso:
			[event:onmedehalfop]OnMeDeHalfOp[/event]
	*/

	EVENT("OnMeHalfOp",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname"),

	/*
		@doc: onmedehalfop
		@type:
			event
		@title:
			OnMeDeHalfOp
		@short:
			Someone has given the local user the -h flag
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
		@window:
			Channels window
		@description:
			Triggered when someone sets a -h (dehalfop) flag for the local user in the active channel.
		@seealso:
			[event:onmehalfop]OnMeHalfOp[/event]
	*/

	EVENT("OnMeDeHalfOp",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname"),

	/*
		@doc: onvoice
		@type:
			event
		@title:
			OnVoice
		@short:
			Someone has given a user the +v flag
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Voiced nickname
		@window:
			Channels window
		@description:
			Triggered when someone sets a +v (voice) flag to someone in the active channel.
		@seealso:
			[event:ondevoice]OnDeVoice[/event]
	*/

	EVENT("OnVoice",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Voiced nickname"),

	/*
		@doc: ondevoice
		@type:
			event
		@title:
			OnDeVoice
		@short:
			Someone has given a user the -v flag
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Devoiced nickname
		@window:
		Channels window
		@description:
			Triggered when someone sets a -v (devoice) flag to someone in the active channel.
		@seealso:
			[event:ondevoice]OnDeVoice[/event]
	*/

	EVENT("OnDeVoice",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Devoiced nickname"),

	/*
		@doc: onmevoice
		@type:
			event
		@title:
			OnMeVoice
		@short:
			Someone has give the local user a +v flag
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
		@window:
			Channels window
		@description:
			Triggered when someone sets a +v (devoice) flag for the local user in the active channel.
		@seealso:
			[event:onmedevoice]OnMeDeVoice[/event]
	*/

	EVENT("OnMeVoice",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname"),

	/*
		@doc: onmedevoice
		@type:
			event
		@title:
			OnMeDeVoice
		@short:
			Someone has give the local user a -v flag
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
		@window:
			Channels window
		@description:
			Triggered when someone sets a -v (devoice) flag for the local user in the active channel.
		@seealso:
			[event:onmevoice]OnMeVoice[/event]
	*/

	EVENT("OnMeDeVoice",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname"),

	/*
		@doc: onuserop
		@type:
			event
		@title:
			OnUserOp
		@short:
			Someone has given a user the +u flag
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Useropped nickname
		@window:
			Channels window
		@description:
			Triggered when someone sets a +u flag on a user of the active channel.
		@seealso:
			[event:ondeuserop]OnDeUserOp[/event]
	*/

	EVENT("OnUserOp",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Useropped nickname"),

	/*
		@doc: ondeuserop
		@type:
			event
		@title:
			OnDeUserOp
		@short:
			Someone has given a user a -u flag
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Deuseropped nickname
		@window:
			Channels window
		@description:
			Triggered when someone sets a -u (deuserop) flag on a user in the active channel.
		@seealso:
			[event:onuserop]OnUserOp[/event]
	*/

	EVENT("OnDeUserOp",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Deuseropped nickname"),

	/*
		@doc: onmeuserop
		@type:
			event
		@title:
			OnMeUserOp
		@short:
			Someone has given the local user the +u flag
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
		@window:
			Channels window
		@description:
			Triggered when someone sets a +u (userop) flag for the local user in the active channel.
		@seealso:
			[event:onmedeuserop]OnMeDeUserOp[/event]
	*/

	EVENT("OnMeUserOp",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname"),

	/*
		@doc: onmedeuserop
		@type:
			event
		@title:
			OnMeDeUserOp
		@short:
			Someone has given the local user the -u flag
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
		@window:
			Channels window
		@description:
			Triggered when someone sets a -u (deuserop) flag for the local user in the active channel.
		@seealso:
			[event:onmeuserop]OnMeUserOp[/event]
	*/

	EVENT("OnMeDeUserOp",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname"),

	/*
		@doc: onircop
		@type:
			event
		@title:
			OnIrcOp
		@short:
			Someone has given a user the +O flag
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Ircopped nickname
		@window:
			Channels window
		@description:
			Triggered when someone sets a +O (has made IrcOp) flag on a user of the active channel.
		@seealso:
			[event:ondeircop]OnDeIrcOp[/event]
	*/

	EVENT("OnIrcOp",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Ircopped nickname"),

	/*
		@doc: ondeircop
		@type:
			event
		@title:
			OnDeIrcOp
		@short:
			Someone has given a user the -O flag
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Deircopped nickname
		@window:
			Channels window
		@description:
			Triggered when someone sets a -O (DeIrcOp) flag on a user in the active channel.
		@seealso:
			[event:onircop]OnIrcOp[/event]
	*/

	EVENT("OnDeIrcOp",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Deircopped nickname"),

	/*
		@doc: onmeircop
		@type:
			event
		@title:
			OnMeIrcOp
		@short:
			Someone has given the +O flag to the local user
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
		@window:
			Channels window
		@description:
			Triggered when someone sets a +O (has made IrcOp) flag on the local user in the active channel.
		@seealso:
			[event:onmedeircop]OnMeDeIrcOp[/event]
	*/

	EVENT("OnMeIrcOp",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname"),

	/*
		@doc: onmedeircop
		@type:
			event
		@title:
			OnMeDeIrcOp
		@short:
			Someone has given the -O flag to the local user
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
		@window:
			Channels window
		@description:
			Triggered when someone sets a -O (DeIrcOp) flag on the local user in the active channel.
		@seealso:
			[event:onmeircop]OnMeIrcOp[/event]
	*/

	EVENT("OnMeDeIrcOp",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname"),

	// Services
	/*
		@doc: onchanservnotice
		@type:
			event
		@title:
			OnChanServNotice
		@short:
			A notice from ChanServ has been received
		@parameters:
			$0 = ChanServ nickname
			$1 = ChanServ username
			$2 = ChanServ hostname
			$3 = Message
			$4 = Message tags
		@window:
		console
		@description:
			Triggered when a notice is received and the source user is recognized as
			the ChanServ service.<br>
			Calling [cmd]halt[/cmd] stops the message output.
		@seealso:
			[event:onquerynotice]OnQueryNotice[/event]
			[event:onnickservnotice]OnNickServNotice[/event]
			[event:onmemoservnotice]OnMemoServNotice[/event]
	*/

	EVENT("OnChanServNotice",
	    "$0 = ChanServ nickname\n"
	    "$1 = ChanServ username\n"
	    "$2 = ChanServ hostname\n"
	    "$3 = Message\n"
	    "$4 = Message tags"),

	/*
		@doc: onnickservnotice
		@type:
			event
		@title:
			OnNickServNotice
		@short:
			A notice from NickServ has been received
		@parameters:
			$0 = NickServ nickname
			$1 = NickServ username
			$2 = NickServ hostname
			$3 = Message
			$4 = Message tags
		@window:
			console
		@description:
			Triggered when a notice is received and the source user is recognized as
			the NickServ service (the source nickname is 'NickServ' or its mask matches
			a mask defined in the Nickserver identify options).[br]
			Calling [cmd]halt[/cmd] stops the message output.
		@seealso:
			[event:onquerynotice]OnQueryNotice[/event]
			[event:onchanservnotice]OnChanServNotice[/event]
			[event:onmemoservnotice]OnMemoServNotice[/event]
	*/

	EVENT("OnNickServNotice",
	    "$0 = NickServ nickname\n"
	    "$1 = NickServ username\n"
	    "$2 = NickServ hostname\n"
	    "$3 = Message\n"
	    "$4 = Message tags"),

	/*
		@doc: OnNickServAuth
		@type:
			event
		@title:
			OnNickServAuth
		@short:
			IRCd set a registered user mode
		@parameters:
		@window:
			any
		@description:
			Triggered when the IRCd sets a registered user mode.
	*/

	EVENT("OnNickServAuth", ""),

	// CTCPs
	/*
		@doc: onaction
		@type:
			event
		@title:
			OnAction
		@short:
			Someone issued a CTCP ACTION
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Message target
			$4 = Action message text
			$5 = Message tags
			$6 = Was encrypted
		@window:
			Channels and query windows
		@description:
			This event is triggered when a remote user issues a CTCP ACTION.[br]
			(The CTCP ACTION is usually triggered by the /me command).[br]
			The parameters $0-$2 identify the source user.[br]
			$3 identifies the message target and can be one of the following:[br]
			- The local user nickname : this is a private CTCP ACTION and is triggered inside a query window.[br]
			- A channel name : this is a channel CTCP ACTION and is triggered in the channel window.[br]
			- Any other string : this is either a broadcast CTCP ACTION (sent by a privileged user
				to a subset of clients) or a sign of the server brain damage :D In this
				case the event is triggered in the console window.[br]
			$5 is a hash of the current message tags, if available.[br]
			$6 will be [b]1[/b] if the message was encrypted and [b]0[/b] otherwise.[br]
			Calling [cmd]halt[/cmd] in this event stops the message output.[br]
	*/

	EVENT("OnAction",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Message target\n"
	    "$4 = Action message text\n"
	    "$5 = Message tags\n"
	    "$6 = Was encrypted"),

	/*
		@doc: onmeaction
		@type:
			event
		@title:
			OnMeAction
		@short:
			User issued a CTCP ACTION
		@parameters:
			$0 = Action message text
			$1 = Action target (channel or nickname)
		@window:
			Channels and query windows
		@description:
			This event is triggered when user issues a CTCP ACTION.[br]
			(The CTCP ACTION is usually triggered by the /me command).[br]
			Warning: don't use a /me inside this event handler, or you'll get an infinite recursion.
			If you need to halt the action (e.g. to do some text substitution), follow the example code.
		@examples:
			[example]
				ctcp $1 action "this text was changed in scripting, old one was $0";
				echo -i=$msgtype(action) -w=$active "$me this text was changed in scripting, old one was $0";
				halt;
			[/example]
	*/

	EVENT("OnMeAction",
	    "$0 = Action message text\n"
	    "$1 = Action target"),

	/*
		@doc: onctcprequest
		@type:
			event
		@title:
			OnCTCPRequest
		@short:
			A CTCP request has been received
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Target
			$4 = CTCP type
			$5- = CTCP parameters
		@window:
			Console window
		@description:
			A CTCP request has been received.[br]
			If you call [cmd]halt[/cmd] in this event, you will stop the further processing of the CTCP, thus, you can disable some of the KVIrc features.[br]
			Be careful when using this.
		@seealso:
			[event:onctcpreply]OnCTCPReply[/event]
	*/

	EVENT("OnCTCPRequest",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Target\n"
	    "$4 = CTCP type\n"
	    "$5- = CTCP parameters"),

	/*
		@doc: onctcpreply
		@type:
			event
		@title:
			OnCTCPReply
		@short:
			A CTCP reply has been received
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Target
			$4 = CTCP type
			$5- = CTCP parameters
		@window:
			Console window
		@description:
			A CTCP reply has been received.[br]
			If you call [cmd]halt[/cmd] in this event, you will stop the further processing of the CTCP reply, thus, you can disable some of the KVIrc features.[br]
			Be careful when using this.
		@seealso:
			[event:onctcpreply]OnCTCPReply[/event]
	*/

	EVENT("OnCTCPReply",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Target\n"
	    "$4 = CTCP type\n"
	    "$5- = CTCP parameters"),

	/*
		@doc: onctcpflood
		@type:
			event
		@title:
			OnCTCPFlood
		@short:
			Someone tried to flood you with CTCP requests
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Target
			$4 = CTCP type
			$5- = CTCP parameters
		@window:
			Console window
		@description:
			On IRC there is a kind of attack called 'CTCP Flood'. It consist of a lot of CTCP request in a short period of time, trying to made your client reply to them and made the server kick you. This event is triggered when one of such attacks is detected.
		@seealso:
			[event:onctcprequest]OnCTCPRequest[/event]
			[event:onctcpreply]OnCTCPReply[/event]
	*/

	EVENT("OnCTCPFlood",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Target\n"
	    "$4 = CTCP type\n"
	    "$5- = CTCP parameters"),

	/*
		@doc: ondccsessioncreated
		@type:
			event
		@title:
			OnDCCSessionCreated
		@short:
			A new DCC session has just been created
		@parameters:
			$0 = DCC session ID
		@window:
			console
		@description:
			Triggered when a new DCC session has just been created.
			You can retrieve the session information by calling the [module:dcc]dcc[/module].*
			module functions.
	*/

	EVENT("OnDCCSessionCreated",
	    "$0 = DCC session ID"),

	/*
		@doc: ondccsessiondestroyed
		@type:
			event
		@title:
			OnDCCSessionDestroyed
		@short:
			A new DCC session has just been destroyed
		@parameters:
			$0 = DCC session ID
		@window:
			console
		@description:
			Triggered when a new DCC session has just been destroyed.
			You can retrieve the session information by calling the [module:dcc]dcc[/module].*
			module functions and by passing the DCC session ID parameter.
	*/

	EVENT("OnDCCSessionDestroyed",
	    "$0 = DCC session ID"),

	/*
		@doc: ondccchatwindowcreated
		@type:
			event
		@title:
			OnDCCChatWindowCreated
		@short:
			A DCC Chat window has been just created
		@parameters:
			$0 = DCC session ID
		@window:
			DCC Chat
		@description:
			A DCC Chat window has been just created.
			The [module:dcc]dcc[/module] module functions
			can be used to retrieve the information about the
			window and the data transfer.[br]
			The DCC Chat connection attempt starts from here:
			you can get [event:ondccchatterminated]OnDCCChatError[/event]
			in any moment
		@seealso:
			[event:ondccchatterminated]OnDCCChatTerminated[/event]
	*/

	EVENT("OnDCCChatWindowCreated",
	    "$0 = DCC session ID"),

	/*
		@doc: ondccchatconnectioninprogress
		@type:
			event
		@title:
			OnDCCChatConnectionInProgress
		@short:
			A DCC Chat connection has just been started
		@parameters:
			$0 = DCC session ID
		@window:
			DCC Chat
		@description:
			A DCC Chat connection has just been started successfully.
			The [module:dcc]dcc[/module] module functions
			can be used to retrieve the information about the
			window and the data transfer.
			In case of a passive DCC Chat, the local ip and port
			are known from the moment that this event triggers.
		@seealso:
	*/

	EVENT("OnDCCChatConnectionInProgress",
	    "$0 = DCC session ID"),

	/*
		@doc: ondccchatconnected
		@type:
			event
		@title:
			OnDCCChatConnected
		@short:
			A DCC Chat connection has been established
		@parameters:
			$0 = DCC session ID
		@window:
			DCC Chat
		@description:
			A DCC Chat connection has just been established.
			The [module:dcc]dcc[/module] module functions
			can be used to retrieve the information about the
			window and the data transfer.
		@seealso:
	*/

	EVENT("OnDCCChatConnected",
	    "$0 = DCC session ID"),

	/*
		@doc: ondccchatmessage
		@type:
			event
		@title:
			OnDCCChatMessage
		@short:
			A DCC Chat message has just been received
		@parameters:
			$0 = Message text
			$1 = DCC session ID
		@window:
			DCC Chat
		@description:
			A DCC Chat message has just been received.
			The [module:dcc]dcc[/module] module functions
			can be used to retrieve the information about the
			window and the data transfer.
		@seealso:
	*/

	EVENT("OnDCCChatMessage",
	    "$0 = Message text\n"
	    "$1 = DCC session ID"),

	/*
		@doc: ondccchaterror
		@type:
			event
		@title:
			OnDCCChatError
		@short:
			A DCC Chat error has just been triggered
		@parameters:
			$0 = Error message
			$1 = DCC session ID
		@window:
			DCC Chat
		@description:
			A DCC Chat error has just been triggered
			The [module:dcc]dcc[/module] module functions
			can be used to retrieve the information about the
			window and the data transfer.
			A DCC Chat can terminate because an error has
			interrupted the connection or because
			the local user has closed the window forcibly.
			The remote end closing the connection is considered
			an error.
		@seealso:
	*/

	EVENT("OnDCCChatError",
	    "$0 = Error message\n"
	    "$1 = DCC session ID"),

	/*
		@doc: ondccchatdisconnected
		@type:
			event
		@title:
			OnDCCChatDisconnected
		@short:
			A DCC Chat has been just disconnected
		@parameters:
			$0 = DCC session ID
		@window:
			DCC Chat
		@description:
			A DCC Chat has just been disconnected for some reason.
			The [module:dcc]dcc[/module] module functions
			can be used to retrieve the information about the
			window and the data transfer.
		@seealso:
	*/

	EVENT("OnDCCChatDisconnected",
	    "$0 = DCC session ID"),

	/*
		@doc: ondccchatwindowclosing
		@type:
			event
		@title:
			OnDCCChatWindowClosing
		@short:
			A DCC Chat is going to be closed
		@parameters:
			$0 = DCC session ID
		@window:
			DCC Chat
		@description:
			A DCC Chat window is going to be closed.
			The [module:dcc]dcc[/module] module functions
			can be used to retrieve the information about the
			window and the data transfer.
		@seealso:
	*/

	EVENT("OnDCCChatWindowClosing",
	    "$0 = DCC session ID"),

	/*
		@doc: ondccfiletransferconnectioninprogress
		@type:
			event
		@title:
			OnDCCFileTransferConnectionInProgress
		@short:
			Triggered when a DCC File transfer is waiting for a connection
		@parameters:
			$0 = DCC session ID
		@window:
			unspecified
		@description:
			Triggered when a DCC File transfer has been set up and
			is waiting for the connection to be established.[br]
			Note that the real data transfer is not started until
			[event:ondccfiletransferbegin]OnDCCFileTransferBegin[/event] is triggered
			but from this moment on you will be notified of success
			or failure by the means of [event:ondccfiletransfersuccess]OnDCCFileTransferSuccess[/event]
			or [event:ondccfiletransferfailed]OnDCCFileTransferFailed[/event].[br]
			You can retrieve the session information by calling the [module:dcc]dcc[/module].*
			module functions and by passing the DCC session ID parameter.
		@seealso:
			[event:ondccfiletransferfailed]OnDCCFileTransferFailed[/event],
			[event:ondccfiletransfersuccess]OnDCCFileTransferSuccess[/event],
			[event:ondccfiletransferbegin]OnDCCFileTransferBegin[/event]
	*/

	EVENT("OnDCCFileTransferConnectionInProgress",
	    "$0 = DCC session ID"),

	/*
		@doc: ondccfiletransferbegin
		@type:
			event
		@title:
			OnDCCFileTransferBegin
		@short:
			Triggered when a DCC File transfer begins
		@parameters:
			$0 = DCC session ID
		@window:
			unspecified
		@description:
			Triggered when a DCC file transfer connection is established
			and the real data transfer starts.[br]
			You can retrieve the session information by calling the [module:dcc]dcc[/module].*
			module functions and by passing the DCC session ID parameter.
		@seealso:
			[event:ondccfiletransferconnectioninprogress]OnDCCFileTransferConnectionInProgress[/event],
			[event:ondccfiletransfersuccess]OnDCCFileTransferSuccess[/event],
			[event:ondccfiletransferfailed]OnDCCFileTransferFailed[/event]
	*/

	EVENT("OnDCCFileTransferBegin",
	    "$0 = DCC session ID"),

	/*
		@doc: ondccfiletransfersuccess
		@type:
			event
		@title:
			OnDCCFileTransferSuccess
		@short:
			Triggered when a DCC File transfer succeeds
		@parameters:
			$0 = Bytes transferred
			$1 = DCC session ID
		@window:
			unspecified
		@description:
			Triggered when a DCC File transfer terminates successfully.[br]
			$0 contains the number of bytes transferred in the session.[br]
			You can retrieve the session information by calling the [module:dcc]dcc[/module].*
			module functions and by passing the DCC session ID parameter.
		@seealso:
			[event:ondccfiletransferconnectioninprogress]OnDCCFileTransferConnectionInProgress[/event],
			[event:ondccfiletransferfailed]OnDCCFileTransferFailed[/event],
			[event:ondccfiletransferbegin]OnDCCFileTransferBegin[/event]
	*/

	EVENT("OnDCCFileTransferSuccess",
	    "$0 = Bytes transferred\n"
	    "$1 = DCC session ID"),

	/*
		@doc: ondccfiletransferfailed
		@type:
			event
		@title:
			OnDCCFileTransferFailed
		@short:
			Triggered when a DCC File transfer fails
		@parameters:
			$0 = Failure reason message
			$1 = Bytes transferred
			$2 = DCC session ID
		@window:
			unspecified
		@description:
			Triggered when a DCC File transfer fails. $0 contains a message
			explaining the reason of the failure.[br]
			$1 contains the number of bytes transferred in the session, eventually 0.[br]
			You can retrieve the session information by calling the [module:dcc]dcc[/module].*
			module functions and by passing the DCC session ID parameter.
		@seealso:
			[event:ondccfiletransferconnectioninprogress]OnDCCFileTransferConnectionInProgress[/event],
			[event:ondccfiletransfersuccess]OnDCCFileTransferSuccess[/event],
			[event:ondccfiletransferbegin]OnDCCFileTransferBegin[/event]
	*/

	EVENT("OnDCCFileTransferFailed",
	    "$0 = Failure reason\n"
	    "$1 = Bytes transferred\n"
	    "$2 = DCC session ID"),

	// Link clicks
	/*
		@doc: OnChannelNickLinkClick
		@type:
			event
		@title:
			OnChannelNickLinkClick
		@short:
			Clicked on nickname in channel
		@parameters:
			$0 = Nickname list
		@window:
			channel
		@description:
			Triggered when user clicks on nickname in channel window
			It will be triggered only at the left mouse button click
	*/

	EVENT("OnChannelNickLinkClick",
	    "$0 = Nickname list"),

	/*
		@doc: OnQueryNickLinkClick
		@type:
			event
		@title:
			OnQueryNickLinkClick
		@short:
			Triggered when user clicks on nickname in query window
		@parameters:
			$0 = Nickname list
		@window:
			query
		@description:
			Triggered when user clicks on nickname in query window
			It will be triggered only at the left mouse button click
	*/

	EVENT("OnQueryNickLinkClick",
	    "$0 = Nickname list"),

	/*
		@doc: OnConsoleNickLinkClick
		@type:
			event
		@title:
			OnConsoleNickLinkClick
		@short:
			Triggered when user clicks on nickname in console window
		@parameters:
			$0 = Nickname
		@window:
			console
		@description:
			Triggered when user clicks on nickname in console window
			It will be triggered only at the left mouse button click
	*/

	EVENT("OnConsoleNickLinkClick",
	    "$0 = Nickname"),

	/*
		@doc: OnHostLinkClick
		@type:
			event
		@title:
			OnHostLinkClick
		@short:
			Triggered when user clicks on any hostname in any window
		@parameters:
			$0 = Hostname
		@window:
			any
		@description:
			Triggered when user clicks on any hostname in any window
			It will be triggered only at the left mouse button click
	*/

	EVENT("OnHostLinkClick",
	    "$0 = Hostname"),

	/*
		@doc: OnURLLinkClick
		@type:
			event
		@title:
			OnURLLinkClick
		@short:
			Triggered when user clicks on any URL link in any window
		@parameters:
			$0 = URL
		@window:
			any
		@description:
			Triggered when user clicks on any URL link in any window
			It will be triggered only at the left mouse button click
	*/

	EVENT("OnURLLinkClick",
	    "$0 = URL"),

	/*
		@doc: OnChannelLinkClick
		@type:
			event
		@title:
			OnChannelLinkClick
		@short:
			Triggered when user clicks on any channel name in any window
		@parameters:
			$0 = Channel name
		@window:
			any
		@description:
			Triggered when user clicks on any channel name in any window
			It will be triggered only at the left mouse button click
	*/

	EVENT("OnChannelLinkClick",
	    "$0 = Channel name"),

	/*
		@doc: OnServerLinkClick
		@type:
			event
		@title:
			OnServerLinkClick
		@short:
			Triggered when user clicks on server name in any window
		@parameters:
			$0 = Server name
		@window:
			any
		@description:
			Triggered when user clicks on server name in any window
			It will be triggered only at the left mouse button click
	*/

	EVENT("OnServerLinkClick",
	    "$0 = Server name"),

	/*
		@doc: ontextviewdoublecliked
		@type:
			event
		@title:
			OnTextViewDoubleClicked
		@short:
			Text view link double click customization
		@parameters:
			$0 = Actual name
			$1 = Visible name
			$2 = Command name
		@window:
			The window that the user has clicked in
		@description:
			Actual name is equal to the command name if command name is not null.
			Visible name is the link text (as it shown by user).
			Command name is set if the visible name differs with the actual link target  (e.g. probably contains mIRC colors). It is equal to the actual target name.
			This event is triggered when a user has double clicked on a text view and has not hit any link.[br]
			I have no idea what you can do in this event, maybe some sort of channel-central dialog? :)
	*/

	EVENT("OnTextViewDoubleClicked", ""),

	/*
		@doc: onnicklinkdefaultactionrequest
		@type:
			event
		@title:
			OnNickLinkDefaultActionRequest
		@short:
			Nick link double click customization
		@parameters:
			$0 = Actual name
			$1 = Visible name
			$2 = Command name
		@window:
			The window that the user has clicked in
		@description:
			Actual name is equal to the command name if command name is not null.
			Visible name is the link text (as it shown by user)
			Command name is set if the visible name differs with the actual link target (e.g. probably contains mIRC colors). It is equal to the actual target name.
			This event is triggered when a user has double clicked on a nickname link.[br]
			You should use this event to perform some significant action associated with the double click.[br]
			A good example might be a [cmd]whois[/cmd] or a [cmq]query[/cmd] call.[br]
			Please note that [b]not all[/b] nickname links will cause this event to be triggered: some nickname
			links default actions are actually handled by the
			[event:onquerynickdefaultactionrequest]OnQueryNickDefaultActionRequest[/event] and
			[event:onchannelnickdefatulactionrequest]OnChannelNickDefaultActionRequest[/event] events.[br]
	*/

	EVENT("OnNickLinkDefaultActionRequest",
	    "$0 = Actual name\n"
	    "$1 = Visible name\n"
	    "$2 = Command name"),

	/*
		@doc: onchannelnickdefaultactionrequest
		@type:
			event
		@title:
			OnChannelNickDefaultActionRequest
		@short:
			Allows customization of the double click on nicknames
		@parameters:
			$0 = Nickname list
		@window:
			Channel window
		@description:
		@seealso:
			[event:onchannelnickpopuprequest]OnChannelNickPopupRequest[/event]
	*/

	EVENT("OnChannelNickDefaultActionRequest",
	    "$0 = Nickname list"),

	/*
		@doc: onquerynickdefaultactionrequest
		@type:
			event
		@title:
			OnQueryNickDefaultActionRequest
		@short:
			Query double click customization
		@parameters:
			$0 = Actual name
			$1 = Visible name
			$2 = Command name
		@window:
			The window that the user has clicked in
		@description:
			Actual name is equal to the command name if command name is not null.
			Visible name is the link text (as it shown by user)
			Command name is set if the visible name differs with the actual link target (e.g. probably contains mIRC colors). It is equal to the actual target name.
			This event is triggered when a user has double clicked on a nickname in
			the query target list.[br]
			You should use this event to do some significant action associated with the double click.[br]
			A good example might be a [cmd]WHOIS[/cmd] query or a [cmd]dcc.chat[/cmd]
	*/

	EVENT("OnQueryNickDefaultActionRequest",
	    "$0 = Actual name\n"
	    "$1 = Visible name\n"
	    "$2 = Command name"),

	/*
		@doc: onwindowtitlerequest
		@type:
			event
		@title:
			OnWindowTitleRequest
		@short:
			Triggered when a window title needs to be set
		@parameters:
			$0 = Window ID
			$1 = Default window title
		@window:
			Any window
		@description:
			This event is triggered when a KVIrc MDI or undocked window is in need of a window title change.[br]
			You can set a customized window title and halt the default title from being set.[br]
		@seealso:
			[cmd]window.setWindowTitle[/cmd]
	*/

	EVENT("OnWindowTitleRequest",
	    "$0 = Window ID\n"
	    "$1 = Default window title"),

	/*
		@doc: oninputbarpaste
		@type:
			event
		@title:
			OnInputBarPaste
		@short:
			Triggered when the user paste some text on the inputbar
		@parameters:
			$0 = Window ID
			$1 = Pasted text
		@window:
			Any window
		@description:
			This event is triggered when the user paste some text on the input bar.[br]
			You can check the text, insert any other text and halt the original from being inserted in the input bar.[br]
			Note that the text can contains binary data, end-of-line or other characters that could need a special treatment then parsed.
		@seealso:
			[cmd]window.insertInInputText[/cmd]
	*/

	EVENT("OnInputBarPaste",
	    "$0 = Window ID\n"
	    "$1 = Pasted text"),

	/*
		@doc: onmemoservnotice
		@type:
			event
		@title:
			OnMemoServNotice
		@short:
			A notice from MemoServ has been received
		@parameters:
			$0 = MemoServ nickname
			$1 = MemoServ username
			$2 = MemoServ hostname
			$3 = Message
			$4 = Message tags
		@window:
		console
		@description:
			Triggered when a notice is received and the source user is recognized as
			the MemoServ service.<br>
			Calling [cmd]halt[/cmd] stops the message output.
		@seealso:
			[event:onquerynotice]OnQueryNotice[/event]
			[event:onnickservnotice]OnNickServNotice[/event]
			[event:onchanservnotice]OnChanServNotice[/event]
	*/

	EVENT("OnMemoServNotice",
	    "$0 = MemoServ nickname\n"
	    "$1 = MemoServ username\n"
	    "$2 = MemoServ hostname\n"
	    "$3 = Message\n"
	    "$4 = Message tags"),

	/*
		@doc: onbroadcastnotice
		@type:
			event
		@title:
			OnBroadcastNotice
		@short:
			A broadcast NOTICE has been received.
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Target
			$4 = Message
		@window:
			Console window
		@description:
			A NOTICE directed to a special target has been received.
			A special target is actually anything that KVIrc couldn't
			recognize: not the local user nor a channel.
		@seealso:
			[event:onservernotice]OnServerNotice[/event]
	*/

	EVENT("OnBroadcastNotice",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Target\n"
	    "$4 = Message"),

	/*
		@doc: onquietban
		@type:
			event
		@title:
			OnQuietBan
		@short:
			Someone has set a +q flag on the channel
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Ban mask
		@window:
			Channel window
		@description:
			Triggered when someone sets a +q flag on the channel
		@seealso:
			[event:onquietunban]OnQuietUnban[/event]
	*/

	EVENT("OnQuietBan",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Ban mask"),

	/*
		@doc: onquietunban
		@type:
			event
		@title:
			OnQuietUnban
		@short:
			Someone has set a -q flag on the channel
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Ban mask
		@window:
			Channel window
		@description:
			Triggered when someone sets a -q flag on the channel
		@seealso:
			[event:onquietban]OnQuietBan[/event]
	*/

	EVENT("OnQuietUnban",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Ban mask"),

	/*
		@doc: onmequietban
		@type:
			event
		@title:
			OnMeQuietBan
		@short:
			Local user is being quiet-banned (+q) in a channel
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Ban mask
		@window:
			Channel
		@description:
			Triggered when someone has set a +q flag that matches your mask (quiet bans you) in the channel.[br]
		@seealso:
			[event:onmequietunban]OnMeQuietUnban[/event]
	*/

	EVENT("OnMeQuietBan",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Ban mask"),

	/*
		@doc: onmequietunban
		@type:
			event
		@title:
			OnMeQuietUnban
		@short:
			Local user is being quiet-debanned (-q) in a channel
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Ban mask
		@window:
			Channel
		@description:
			Triggered when someone has set a -q flag that matches your mask (quiet unbans you) in the channel.[br]
		@seealso:
			[event:onmequietban]OnMeQuietBan[/event]
	*/

	EVENT("OnMeQuietUnban",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Ban mask"),

	/*
		@doc: onmehostchange
		@type:
			event
		@title:
			OnMeHostChange
		@short:
			Local user's visible hostname is changed
		@parameters:
			$0 = Source server
			$1 = New hostname
		@window:
			Console
		@description:
			Triggered when your visible hostname has been changed. The source could either be services or the IRCd.
	*/

	EVENT("OnMeHostChange",
	    "$0 = Source server\n"
	    "$1 = New hostname"),

	/*
		@doc: onhostchange
		@type:
			event
		@title:
			OnHostChange
		@short:
			Someone's visible hostname has changed
		@parameters:
			$0 = Source nickname
			$1 = Source old username
			$2 = Source old hostname
			$3 = Source new username
			$4 = Source new hostname
		@window:
			Console window
		@description:
			Triggered when someone has had their username and/or hostname changed.[br]
			Note: This requires Extended Capabilities to be enabled, as well as
			CAP CHGHOST support from the server.
		@seealso:
			[event:onmehostchange]OnMeHostChange[/event]
	*/

	EVENT("OnHostChange",
	    "$0 = Source nickname\n"
	    "$1 = Source old username\n"
	    "$2 = Source old hostname\n"
	    "$3 = Source new username\n"
	    "$4 = Source new hostname"),

	/*
		@doc: onaccount
		@type:
			event
		@title:
			OnAccount
		@short:
			Someone's account name has changed
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Source account name
		@window:
			Console window
		@description:
			Triggered when someone has had their account name changed.[br]
			Note: This requires Extended Capabilities to be enabled, as well as
			CAP ACCOUNT-NOTIFY support from the server.
	*/

	EVENT("OnAccount",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Source account name"),

	/*
		@doc: onaway
		@type:
			event
		@title:
			OnAway
		@short:
			Someone's away status has changed
		@parameters:
			$0 = Source nickname
			$1 = Source username
			$2 = Source hostname
			$3 = Away message
		@window:
			Console window
		@description:
			Triggered when someone has changed their away status. If the away message is blank,
			then they are returning from away, otherwise they are either setting themselves to
			away or changing their away message.[br]
			Note: This requires Extended Capabilities to be enabled, as well as
			CAP AWAY-NOTIFY support from the server.
	*/

	EVENT("OnAway",
	    "$0 = Source nickname\n"
	    "$1 = Source username\n"
	    "$2 = Source hostname\n"
	    "$3 = Away message"),

	/*
		@doc: oncap
		@type:
			event
		@title:
			OnCap
		@short:
			Received a capability update from the IRCd
		@parameters:
			$0 = Server
			$1 = Command
			$2 = Protocols
		@window:
			Console window
		@description:
			Triggered when a CAP message was received from the server
	*/

	EVENT("OnCap",
	    "$0 = Server\n"
	    "$1 = Command\n"
	    "$2 = Protocols"),
};
