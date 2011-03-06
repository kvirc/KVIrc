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
//   of the License, or (at your opinion) any later version.
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

#define EVENT(_name,_parm) KviKvsEvent(_name,_parm)

KviKvsEvent KviKvsEventManager::m_appEventTable[KVI_KVS_NUM_APP_EVENTS]=
{
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
	EVENT("OnKVIrcStartup",""),

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
	EVENT("OnKVIrcShutdown",""),

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
	EVENT("OnIRCContextCreated",""),

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
	EVENT("OnIRCContextDestroyed",""),

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
			and the other MDI children windows.
		@seealso:
			[event:onframewindowdestroyed]OnFrameWindowDestroyed[/event]
	*/
	EVENT("OnFrameWindowCreated",""),

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
	EVENT("OnFrameWindowDestroyed",""),

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
			WARNING: If you attempt to "echo $0" inside this event, you will cause an infinite loop.
	*/
	EVENT("OnURL", \
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
			$0 = message source
			$1 = error reason
		@window:
			Console
		@description:
			Triggered when an ERROR message is received from the server.[br]
			$0 is the message (server) source but be aware that it often happens to be empty.[br]
			Calling 'halt' in this event will stop the default output.[br]
	*/
	EVENT("OnError", \
		"$0 = message source\n" \
		"$1 = error reason"),

	/*
		@doc: onaccelkeypressed
		@type:
			event
		@title:
			OnAccelKeyPressed
		@short:
			Triggered when an accelerator key was pressed
		@parameters:
			$0 = keys
		@window:
			Active window
		@description:
			An accelerator key has been pressed by the user.[br]
			The key sequence that the user has pressed is passed in $0 as a
			human readable string.[br]
			The keys that actually work with KVIrc are:[br]
			F2 to F12, F1 to F12 with SHIFT key pressed, 0 to 9 with CTRL key pressed.[br]
			This means that you can set up 33 "quick" aliases...that's really more than
			an user can remember.[br]
			If you need more keys, mail me, I'll see what I can do :)[br]
	*/
	EVENT("OnAccelKeyPressed", \
		"$0 = keys"),

	/*
		@doc: onhighlight
		@type:
			event
		@title:
			OnHighlight
		@short:
			Triggered when a channel or query message is going to be highlighted
		@parameters:
			$0 = talker nick
			$1 = target username
			$2 = target hostname
			$3 = message
			$4 = highlight word
			$5 = message type
			$6 = is action
		@window:
			Channel or query window
		@description:
			A channel or query message is going to be highlighted.[br]
			The message may be either a PRIVMSG, a NOTICE or a CTCP ACTION.[br]
			The highlight may be either caused by a "highlight word" or by your own nickname
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
	EVENT("OnHighlight", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = message\n" \
		"$4 = highlight word"),

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
	EVENT("OnWindowActivated",""),

	/*
		@doc: onnotifyonline
		@type:
			event
		@title:
			OnNotifyOnline
		@short:
			A user in the notify list is on-line
		@parameters:
			$0 = nickname
		@window:
			Console or active window of the IRC context
		@description:
			This event is triggered when one of the users in your [doc:notify_list]notify[/doc] lists
			is detected to be ON-Line.[br]
			The event is triggered in the console window or the active window of the IRC context depending
			on the user options.[br]
			(Note: If you call [cmd]halt[/cmd] in this event, you will stop the "Nick is on irc" output,
			but this is rather a bad idea since KVIrc gives a lot of additional information to the
			user with some notify-methods).[br]
			This is a good place to play a sound or attract the user attention in some other way.[br]

	*/
	EVENT("OnNotifyOnline", \
		"$0 = nickname"),

	/*
		@doc: onnotifyoffline
		@type:
			event
		@title:
			OnNotifyOffline
		@short:
			A user in the notify list went off-line
		@parameters:
			$0 = nickname
		@window:
			Console or active window of the IRC context
		@description:
			This event is triggered when one of the users in your [doc:notify_list]notify[/doc] lists
			has just left IRC.[br]
			The event is triggered in the console window or the active window of the IRC context depending
			on the user options.[br]
			(Note: If you call [cmd]halt[/cmd] in this event, you will stop the "Nick has left irc" output,
			but this is rather a bad idea since KVIrc gives a lot of additional information to the
			user with some notify-methods).[br]
			This is a good place to play a sound or attract the user attention in some other way.[br]
	*/
	EVENT("OnNotifyOffline", \
		"$0 = nickname"),

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
			$1 = ping message parameters
		@window:
			Console
		@description:
			Triggered when a PING message was received from a server.[br]
			Calling 'halt' in this event will stop the informational message about 'ping->pong'.[br]
	*/
	EVENT("OnPing", \
		"$0 = server\n" \
		"$1 = message parameters"),

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
			$1 = pong message parameters
		@window:
			Console
		@description:
			Triggered when a PONG message was received from a server.[br]
			Calling 'halt' in this event will stop the informational message about the pong'.[br]
	*/
	EVENT("OnPong", \
		"$0 = server\n" \
		"$1 = pong message parameters"),

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
	EVENT("OnLagAlarmTimeUp", \
		"$0 = server\n" \
		"$1 = lag time in milliseconds"),

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
	EVENT("OnLagAlarmTimeDown", \
		"$0 = server\n" \
		"$1 = lag time in milliseconds"),

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
	EVENT("OnLagCheck", \
		"$0 = server\n" \
		"$1 = lag time in milliseconds"),

	/*
		@doc: ontextinput
		@type:
			event
		@title:
			OnTextInput
		@short:
			The user has just entered some text in the text input
		@parameters:
			$0 = text
		@window:
			window in that the text is entered
		@description:
			Triggered when the user enters some text in the text input
			window and presses return.
			Calling [cmd]halt[/cmd] will stop the automatic text processing
			and the result depends on the window the text is typed in.
			In channel/query and console windows the text will NOT be sent to
			the IRC server. This event might be useful for performing
			substitutions in the text sent to the server. If you stop the
			text processing in a channel or query window then the local
			echo of the message will be omitted too. You might want to
			realize the echo by the means of the [cmd]echoprivmsg[/cmd] command.
			This event is NOT triggered when the user enters a command
			in the input window.
		@seealso:
			[cmd]echoprivmsg[/cmd]
	*/
	EVENT("OnTextInput", \
		"$0 = text"),

	/*
		@doc: onnotifylistdefaultactionrequest
		@type:
			event
		@title:
			OnNotifyListDefaultActionRequest
		@short:
			Notify list double click customization
		@parameters:
			$0 = nickname
		@window:
			The window that the user has clicked in
		@description:
			This event is triggered when an user has double clicked on a nickname in
			the notify list (console window).[br]
			You should use this event to do some significant action associated with the double click.[br]
			A good example might be a [cmd]whois[/cmd] query or a [cmd]dcc.chat[/cmd]
	*/
	EVENT("OnNotifyListDefaultActionRequest", \
		"$0 = nickname"),

	/*
		@doc: onwallops
		@type:
			event
		@title:
			OnWallops
		@short:
			Triggered when a WALLOPS message is received from the server
		@parameters:
			$0 = source nick
			$1 = source user
			$2 = source host
			$3 = message text
		@window:
			channel
		@description:
			Triggered when a WALLOPS message is received from the server.
			If you call [cmd]halt[/cmd] the message output will be stopped.
		@seealso:
	*/
	EVENT("OnWallops", \
		"$0 = nick\n" \
		"$1 = user\n" \
		"$2 = host\n" \
		"$3 = message text"),

	/*
		@doc: OnIgnoredMessage
		@type:
			event
		@title:
			OnIgnoredMessage
		@short:
			Triggered when a message is ignored.
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = message target
			$4 = message
		@window:
			any
		@description:
			Triggered when a message is ignored.
	*/
	EVENT("OnIgnoredMessage", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source host\n" \
		"$3 = message target\n" \
		"$4 = message"),

	/*
		@doc: onservernotice
		@type:
			event
		@title:
			OnServerNotice
		@short:
			A server NOTICE have been issued.
		@parameters:
			$0 = source nick
			$1 = message
		@window:
			Console window
		@description:
			Someone has issued a server NOTICE.
		@seealso:
			[event:onchannelnotice]OnChannelNotice[/event]
	*/
	EVENT("OnServerNotice", \
		"$0 = source nick\n" \
		"$1 = message"),



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
			Your nickname is still "undefined" too.
			If you're looking for the appropriate moment for sending
			data to the server look at the [event:onirc]OnIRC[/event] event.
			This event is a good place for initializing connection related
			variables. By calling [cmd]halt[/cmd] in this event you will stop the
			"Connection established" output.
		@seealso:
			[event:onircconnectionterminated]OnIRCConnectionTerminated[/event]
	*/
	EVENT("OnIRCConnectionEstablished",""),

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
			event you will stop the "Connection terminated..." output.
		@seealso:
			[event:onircconnectionestablished]OnIRCConnectionEstablished[/event]
	*/
	EVENT("OnIRCConnectionTerminated",""),

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
			"completely on IRC": your nickname has been defined
			and the server is ready to accept commands from you.
			This is a good place to join the "preferred channels".
			By calling [cmd]halt[/cmd] in this event you will
			stop the "Login operations complete..." output.
		@seealso:
			[event:onircconnectionestablished]OnIRCConnectionEstablished[/event]
			[event:onircconnectionterminated]OnIRCConnectionTerminated[/event]
	*/
	EVENT("OnIRC",""),

	/*
		@doc: onnetsplit
		@type:
			event
		@title:
			OnNetsplit
		@short:
			A netsplit has been detected
		@parameters:
			$0 = server1
			$1 = server2
		@window:
			console
		@description:
			Triggered when KVIrc detects a netsplit by looking at an user's quit message.
			The two parameters passed are the two servers that lost the link and were
			signaled in that particular quit message.[br]
			The actual netsplit detector in the KVIrc core uses the following rule
			(extracted from the eggdrop bot code): "determine if signoff string matches "%.% %.%",
			and only one space".[br]
			[b]Note that this may be not accurate and KVIrc CAN be fooled by specially
			formatted quit messages that aren't really netsplits.[/b][br]
			KVIrc tries to avoid duplicate netsplit warnings thus this event will NOT
			be triggered for EVERY quit in a netsplit: you will get it once for every
			different set of servers that lost the link.[br]
			Determining where the network link has been exactly broken may be a hard
			task since the quit messages you see may not involve
			the servers on the split boundary.
	*/
	EVENT("OnNetsplit", \
		"$0 = server1\n" \
		"$1 = server2"),

	/*
		@doc: ondisconnectrequest
		@type:
			event
		@title:
			OnDisconnectRequest
		@short:
			Triggered when the user requests a disconnection
		@parameters:
			none
		@window:
			console
		@description:
			Triggered when the user requests a disconnection from
			a server by pressing the "disconnect" button, by closing
			the console or closing KVIrc. It is your last chance
			to send some meaningful data to the server before the QUIT message
			is sent. You can't prevent the user from disconnecting - [cmd]halt[/cmd] does nothing here.[br]
			This event is useful to either give a salute to all the users
			you were talking with or maybe to send some commands to the
			bouncer you're connected to. Note that you can override the QUIT
			message sent by KVIrc by sending it yourself first.[br]
		@seealso:
	*/
	EVENT("OnDisconnectRequest",""),

	/*
		@doc: OnHTTPGetTerminated
		@type:
			event
		@title:
			OnHTTPGetTerminated
		@short:
			A http.get command has terminated execution
		@parameters:
			$0 = status
			$1 = remote url
			$2 = local file
			$3 = magic identifier
		@window:
			Console
		@description:
			Triggered when a [cmd]http.get[/cmd] file download has terminated execution.
			If the transfer has been succesfull then $0 will contain the value 1 (true),
			otherwise will contain the value 0. $1 contains the remote url and $2 the local
			path on disk of the saved file. $3 contains the magic identifier passed to
			the http.get command by the means of the -i switch.
	*/
	EVENT("OnHTTPGetTerminated", \
		"$0 = status\n" \
		"$1 = remote url\n" \
		"$2 = local file\n" \
		"$3 = magic identifier"),

	/*
		@doc: onunhandledliteral
		@type:
			event
		@title:
			OnUnhandledLiteral
		@short:
			An unhandled literal server message has been received
		@parameters:
			$0 = source mask
			$1 = message (literal)
			$2- = parameters
		@window:
			Console window
		@description:
			An unhandled/unrecognized literal server message has been received.[br]
			The KVIrc core code hasn't been able to recognize it nor handle it in any way.[br]
			If you call [cmd]halt[/cmd] you will stop the standard or warning message output.[br]
			For server numerics there is no "unhandled" event, you should use the raw numeric events to handle them.[br]
		@seealso:
	*/
	EVENT("OnUnhandledLiteral", \
		"$0 = source mask\n" \
		"$1 = message\n" \
		"$2- parameters"),

	/*
		@doc: onoutboundtraffic
		@type:
			event
		@title:
			OnOutboundTraffic
		@short:
			Triggered on outbound traffic.[br]
			USE THIS EVENT WITH REAL CAUTION: an improper use can leads to many problems to KVIrc; especially avoid the use of 'halt' command.
		@parameters:
			$0 = outbound string
		@window:
			any
		@description:
			This event is triggered on outbound traffic.
		@seealso:
			[event:ontextinput]OnTextInput[/event]
			[event:onchannelmessage]OnChannelMessage[/event]
	*/
	EVENT("OnOutboundTraffic", \
		"$0 = outbound string"),

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
	EVENT("OnChannelPopupRequest",""),

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
	EVENT("OnConsolePopupRequest",""),

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
	EVENT("OnQueryPopupRequest",""),

	/*
		@doc: onwindowpopuprequest
		@type:
			event
		@title:
			OnWindowPopupRequest
		@short:
			Triggered when the user middle-clicks in a text view (or right-clicks with the CTRL key pressed).
		@parameters:
			none
		@window:
			The window that the user has clicked in.
		@description:
			This event is here to allow easy customization of the window popup (it was builtin in KVIrc version prior to 3.0.0).[br]
 			Actually it is triggered when the user middle-clicks in a text view (or right-clicks with the CTRL key pressed).[br]
 			You should use it to popup your own version of the window popup that should interface the logging and window-behaviour functions.
	*/
	EVENT("OnWindowPopupRequest",""),

		/*
		@doc: onhostlinkpopuprequest
		@type:
			event
		@title:
			OnHostLinkPopupRequest
		@short:
			Triggered when an user right clicks on a hostname link in the text view.
		@parameters:
			$0 = actual name
			$1 - visible name
			$2 - command name
		@window:
			The window that the user has clicked in.
		@description:
			This event is triggered when an user has right clicked on a hostname link in the text view.
			The double click link action is builtin and it causes KVIrc to perform a DNS query on the hostname.
			This is a good place to show a popup menu with some operations that can be done on the hostname like
			"telnet", "traceroute", some special kind of DNS lookup (maybe an xterm with nslookup or sth).
	*/
	EVENT("OnHostLinkPopupRequest", \
		"$0 = actual name\n" \
		"$1 - visible name\n" \
		"$2 - command name"),

	/*
		@doc: onurllinkpopuprequest
		@type:
			event
		@title:
			OnURLLinkPopupRequest
		@short:
			URL link popup customization
		@parameters:
			$0 = actual name
			$1 - visible name
			$2 - command name
		@window:
			The window that the user has clicked in
		@description:
			Actual name is equal to the command name if command name is not null.
			Visible name is the link text (as it shown by user)
			Command name is set if the visible name differs with the actual link target(probably contains mIRC colors,etc). It is equal to the actual target name.
			This event is triggered when an user has right clicked on an URL link in
			the text view. The double click link action is builtin and it causes KVIrc
			to open the URL with a browser (tipically your OS default browser).[br]
			This is a good place to show a [cmd]popup[/cmd] menu with some operations
			that can be performed with the URL, like bookmarking in some way or opening
			with a specific browser.[br]
	*/
	EVENT("OnURLLinkPopupRequest", \
		"$0 = actual name\n" \
		"$1 - visible name\n" \
		"$2 - command name"),

	/*
		@doc: onserverlinkpopuprequest
		@type:
			event
		@title:
			OnServerLinkPopupRequest
		@short:
			Server link popup customization
		@parameters:
			$0 = actual name
			$1 - visible name
			$2 - command name
		@window:
			The window that the user has clicked in
		@description:
			Actual name is equal to the command name if command name is not null.
			Visible name is the link text (as it shown by user)
			Command name is set if the visible name differs with the actual link target(probably contains mIRC colors,etc). It is equal to the actual target name.
			This event is triggered when an user has right clicked on an server name link in
			the text view. The double click link action is builtin and it causes KVIrc
			to query the MOTD of the server.[br]
			This is a good place to show a [cmd]popup[/cmd] menu with some operations
			that can be performed with the server name, like querying stats or links.[br]
			Please note that the server name is generally NOT a hostname: some parts of
			it are often hidden. The name of the server is generally valid only within
			the IRC network that you are connected to.[br]
	*/
	EVENT("OnServerLinkPopupRequest", \
		"$0 = actual name\n" \
		"$1 - visible name\n" \
		"$2 - command name"),

	/*
		@doc: onchannellinkpopuprequest
		@type:
			event
		@title:
			OnChannelLinkPopupRequest
		@short:
			Channel link popup customization
		@parameters:
			$0 = actual name
			$1 - visible name
			$2 - command name
		@window:
			The window that the user has clicked in
		@description:
			Actual name is equal to the command name if command name is not null.
			Visible name is the link text (as it shown by user)
			Command name is set if the visible name differs with the actual link target(probably contains mIRC colors,etc). It is equal to the actual target name.
			This event is triggered when an user has right clicked on an channel link in
			the text view. The double click link action is builtin and it causes KVIrc
			to join the channel (unless the user is already on).[br]
			This is a good place to show a [cmd]popup[/cmd] menu with some operations
			that can be performed with the channel name like bookmarking, joining with a password,
			performing a [cmd]who[/cmd] etc...
	*/
	EVENT("OnChannelLinkPopupRequest", \
		"$0 = actual name\n" \
		"$1 - visible name\n" \
		"$2 - command name"),

	/*
		@doc: onnicklinkpopuprequest
		@type:
			event
		@title:
			OnNickLinkPopupRequest
		@short:
			Nick link popup customization
		@parameters:
			$0 = actual name
			$1 - visible name
			$2 - command name
		@window:
			The window that the user has clicked in
		@description:
			Actual name is equal to the command name if command name is not null.
			Visible name is the link text (as it shown by user)
			Command name is set if the visible name differs with the actual link target(probably contains mIRC colors,etc). It is equal to the actual target name.
			This event is triggered when an user has right clicked on an "unknown" nickname link in
			the text view.[br]
			This is a good place to show a [cmd]popup[/cmd] menu with some operations
			that can be performed with the nickname like querying, sending a file by dcc or opening a dcc.chat.
			Please note that NOT ALL the nickname links will receive this event.[br]
			The nickname links that appear in the text view of the channel that they are actually in
			are handled by the [event:onchannelnickpopuprequest]OnChannelNickPopupRequest[/event] event.[br]
			The nickname links that appear in the text view of the query in that they are a remote end
			are handled by the [event:onquerynickpopuprequest]OnQueryNickPopupRequest[/event] event.[br]
	*/
	EVENT("OnNickLinkPopupRequest", \
		"$0 = actual name\n" \
		"$1 - visible name\n" \
		"$2 - command name"),

	/*
		@doc: onquerynickpopuprequest
		@type:
			event
		@title:
			OnQueryNickPopupRequest
		@short:
			Query nicklist popup customization
		@parameters:
			$0 = actual name
			$1 - visible name
			$2 - command name
		@window:
			The window that the user has clicked in
		@description:
			Actual name is equal to the command name if command name is not null.
			Visible name is the link text (as it shown by user)
			Command name is set if the visible name differs with the actual link target(probably contains mIRC colors,etc). It is equal to the actual target name.
			This event is triggered when an user has right clicked on a set of nicknames in
			the query target list.[br]
			This is a good place to show a [cmd]popup[/cmd] menu with some operations
			that can be performed with the nickname like sending a file by dcc, opening a dcc.chat or
			performing a whois.[br]
	*/
	EVENT("OnQueryNickPopupRequest", \
		"$0 = actual name\n" \
		"$1 - visible name\n" \
		"$2 - command name"),

	/*
		@doc: onchannelnickpopuprequest
		@type:
			event
		@title:
			OnChannelNickPopupRequest
		@short:
			Allows customization of the nick-on-channel context popup
		@parameters:
			$0 = nick list
		@window:
			Channel window
		@description:
			This event is triggered when the user right-clicks in the channel user list
			or on a nickname link in the channel text view.[br]
			The first parameter ($0) contains a comma separated list of selected nicknames (if it is an user list click)
			or a single nickname (if it is a link).[br]
			In both cases all the nicknames are currently on the channel.[br]
			You should popup a popup-menu with a set of possible channel-related actions,
			like op/deop, voice/devoice, whois etc...
		@seealso:
			[event:onchannelnickdefaultactionrequest]OnChannelNickDefaultActionRequest[/event]
	*/
	EVENT("OnChannelNickPopupRequest", \
		"$0 = nick list"),

	/*
		@doc: onnotifylistpopuprequest
		@type:
			event
		@title:
			OnNotifyListPopupRequest
		@short:
			Notify list context popup customization
		@parameters:
			$0 = nickname list
		@window:
			The window that the user has clicked in
		@description:
			This event is triggered when an user has right clicked on a list of nicknames in
			the notify list (console window).[br]
			This is a good place to show a [cmd]popup[/cmd] with some actions that can be performed
			on the nicknames like querying, performing a whois lookup or sth similar.[br]
	*/
	EVENT("OnNotifyListPopupRequest", \
		"$0 = nickname list"),

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
	EVENT("OnDCCChatPopupRequest", \
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
			none
		@window:
			Channel window
		@description:
			Triggered when a channel window has just been created
			Note that the channel is still empty: no users have joined it.
		@seealso:
			[event:onquerywindowcreated]OnQueryWindowCreated[/event]
	*/
	EVENT("OnChannelWindowCreated",""),

	/*
		@doc: onchannelsync
		@type:
			event
		@title:
			OnChannelSync
		@short:
			Triggered when all channel information have been received
		@parameters:
			$0 = channel sync time in milliseconds
		@window:
			Channel
		@description:
			This event is triggered after you have joined a channel
			and all information have been received.[br]
			This includes the channel users list, the ban list,
			the channel mode, the topic and eventually
			the invite and ban exception lists.[br]
			Calling halt in this even stops the "channel synchronized" output.
			(the output will be halted anyway if [fnc]$option[/fnc](boolShowChannelSyncTime) evaluates to false)
		@seealso:
			[event:ondccchatpopuprequest]OnDCCChatPopupRequest[/event],
			[event:onconsolepopuprequest]OnConsolePopupRequest[/event],
			[event:onchannelpopuprequest]OnChannelPopupRequest[/event]
	*/
	EVENT("OnChannelSync", \
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
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = message
			$4 = [target mode prefixes]
		@window:
			Channels window
		@description:
			Triggered when a channel message is received
			$0!$1@$2 is the source of the message, $3 is the message text
			and $4 are the eventual mode prefixes added to the target channel (i.e:
			if the message is only for channel operators then you will get the string @ in $4).
		@seealso:
			[event:onquerymessage]OnQueryMessage[/event]
			[event:ondccchatmessage]OnDCCChatMessage[/event]
	*/
	EVENT("OnChannelMessage", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = message\n" \
		"$4 = target mode prefixes"),

	/*
		@doc: onchannelnotice
		@type:
			event
		@title:
			OnChannelNotice
		@short:
			A NOTICE have been issued to a channel
		@parameters:
			$0 = source nick
			$1 = message
			$2 = target
		@window:
			Channel window
		@description:
			Someone has issued a NOTICE with a channel as his target
			For normal channel notices $2 parameter will only a channel name.
			For channel op or channel voice notices it will contain also the leading '@' or '+' flag.
			Please note that this convention is different from the one used in [event:onchannelprivmsg]OnChannelPrivmsg[/event]:
			the incompatibility is here for historical reasons: it is not a big deal so we're not fighting it :)
		@seealso:
			[event:onservernotice]OnServerNotice[/event]
	*/
	EVENT("OnChannelNotice", \
		"$0 = source nick\n" \
		"$1 = message\n" \
		"$2 = target"),

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
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = message
		@window:
			Query or console window.
		@description:
			Triggered when a query message is received.[br]
			If you [cmd]return[/cmd] [fnc]$false[/fnc] in this event, the message output will be stopped.[br]
			This event is triggered in the query window associated to the source nick, or in
			the IRC context console window if the query window has not been created for some reason.
			Note that you can't stop the query window creation with this event: you must
			use [event:onquerywindowrequest]OnQueryWindowRequest[/event] for that purpose.
			Note also that you may not receive this event if the anti-spam option is in use and
			KVIrc has detected that this might be a spam message. This is usually not an issue
			but if you care about it then write to pragma at kvirc dot net asking to add a OnSpam event.
		@seealso:
			[event:onquerywindowrequest]OnQueryWindowRequest[/event]
			[event:onquerywindowcreated]OnQueryWindowCreated[/event]
			[event:onquerytargetadded]OnQueryTargetAdded[/event]
			[event:onchannelmessage]OnChannelMessage[/event]
			[event:ondccchatmessage]OnDCCChatMessage[/event]
	*/
	EVENT("OnQueryMessage", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = message"),

	/*
		@doc: onquerynotice
		@type:
			event
		@title:
			OnQueryNotice
		@short:
			A private notice has been received
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = message
		@window:
			Query window or console
		@description:
			Triggered when a private notice is received.[br]
			If you call [cmd]halt[/cmd] in this event, the message output will be stopped and if the message was going to cause a query window creation, the window will NOT be created
		@seealso:
			[event:onchannelmessage]OnChannelMessage[/event]
			[event:onchannelnotice]OnChannelNotice[/event]
	*/
	EVENT("OnQueryNotice", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source host\n" \
		"$3 = message"),

	/*
		@doc: onquerywindowrequest
		@type:
			event
		@title:
			OnQueryWindowRequest
		@short:
			A new query window is going to be opened because a remote message has been received
		@parameters:
			$0 = source nickname
			$1 = source username
			$2 = source host
			$3 = message
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
	EVENT("OnQueryWindowRequest", \
		"$0 = source nick\n" \
		"$1 = source user\n" \
		"$2 = source host\n" \
		"$3 = message"),

	/*
		@doc: onquerywindowcreated
		@type:
			event
		@title:
			OnQueryWindowCreated
		@short:
			A query window has just been opened
		@parameters:
			none
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
	EVENT("OnQueryWindowCreated",""),

	/*
		@doc: onquerytargetadded
		@type:
			event
		@title:
			OnQueryTargetAdded
		@short:
			A new query target has been added
		@parameters:
			$0 = nickname
			$1 = username (may be *)
			$2 = host (may be *)
		@window:
			query window
		@description:
			Triggered when a new remote target has been added to a query window.
			This is the event to catch when you want to perform some specific actions
			when someone queries you.
			KVIrc supports multiple target queries and thus, this event is triggered also
			when a new query target is added by using [cmd]addtarget[/cmd].
	*/
	EVENT("OnQueryTargetAdded", \
		"$0 = nick\n" \
		"$1 = username (may be *)\n" \
		"$2 = hostname (may be *)"),

	/*
		@doc: OnQueryFileDropped
		@type:
			event
		@title:
			OnQueryFileDropped
		@short:
			A file is dropped in a query
		@parameters:
			$0 = source nick
			$1 = filedropped
		@window:
			Query
		@description:
			Triggered when a file is dropped in a query window.[br]
			$target is the nick.[br]
	*/
	EVENT("OnQueryFileDropped", \
		"$0 = nick\n" \
		"$1 = filedropped"),

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
			$0 = server message trailing
		@window:
			Console
		@description:
			Triggered when the local user enters the AWAY state.
			Calling 'halt' in this event disables the server "You have been marked as being away" message.[br]
		@seealso:
			[event:onmeback]OnMeBack[/event]
	*/
	EVENT("OnMeAway", \
		"$0- = server message trailing"),

	/*
		@doc: onmeback
		@type:
			event
		@title:
			OnMeBack
		@short:
			Local user cancels the AWAY status
		@parameters:
			$0 = away start time
			$1- = server message trailing
		@window:
			Console
		@description:
			Triggered when the local user leaves the AWAY state.[br]
			The $0 parameter contains the [b]unix time[/b] of the away status start.[br]
			You can obtain the current [b]unix time[/b] from the function [fnc]$unixtime[/fnc].[br]
			Calling 'halt' in this event disables the server "You are no longer marked as being away" message.[br]
		@seealso:
			[event:onmeaway]OnMeAway[/event]
	*/
	EVENT("OnMeBack", \
		"$0 = away start time\n"
		"$1- = server message trailing"),

	/*
		@doc: onjoin
		@type:
			event
		@title:
			OnJoin
		@short:
			Non-local user joining a channel
		@parameters:
			$0 = nickname
			$1 = username
			$2 = hostname
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
	EVENT("OnJoin", \
		"$0 = nickname\n" \
		"$1 = username\n" \
		"$2 = hostname"),

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
	EVENT("OnMeJoin",""),

	/*
		@doc: onpart
		@type:
			event
		@title:
			OnPart
		@short:
			Non-local user is leaving a channel
		@parameters:
			$0 = nickname
			$1 = username
			$2 = hostname
			$3 = part message
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
	EVENT("OnPart", \
		"$0 = nickname\n" \
		"$1 = username\n" \
		"$2 = host\n" \
		"$3 = part message"),

	/*
		@doc: onmepart
		@type:
			event
		@title:
			OnMePart
		@short:
			Local user is leaving a channel
		@parameters:
			$0 = part message
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
	EVENT("OnMePart", \
		"$0 = part message"),

	/*
		@doc: onkick
		@type:
			event
		@title:
			OnKick
		@short:
			Non-local user is being kicked from a channel
		@parameters:
			$0 = source nickname
			$1 = source username
			$2 = source hostname
			$3 = kicked nickname
			$4 = kick reason
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
	EVENT("OnKick", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = kicked nick\n" \
		"$4 = reason"),

	/*
		@doc: onmekick
		@type:
			event
		@title:
			OnMeKick
		@short:
			Local user is being kicked from a channel
		@parameters:
			$0 = source nickname
			$1 = source username
			$2 = source hostname
			$3 = kick reason
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
	EVENT("OnMeKick", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = reason"),

	/*
		@doc: ontopic
		@type:
			event
		@title:
			OnTopic
		@short:
			Topic change on a channel
		@parameters:
			$0 = nickname
			$1 = username
			$2 = hostname
			$3 = new topic string
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
	EVENT("OnTopic", \
		"$0 = nickname\n" \
		"$1 = username\n" \
		"$2 = hostname\n" \
		"$3 = topic"),

	/*
		@doc: onquit
		@type:
			event
		@title:
			OnQuit
		@short:
			An user is leaving IRC
		@parameters:
			$0 = nickname
			$1 = username
			$2 = hostname
			$3 = quit message
			$4 = channels
		@window:
			Console
		@description:
			Triggered when an user is quitting IRC.[br]
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
				foreach(%cname,$4)echo -r=$channel(%cname) User $0 is quitting...
			[/example]
	*/
	EVENT("OnQuit", \
		"$0 = nickname\n" \
		"$1 = username\n" \
		"$2 = hostname\n" \
		"$3 = part message\n" \
		"$4 = channels"),

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
			$0 = source nick
			$1 = source user
			$2 = source host
			$3 = modeflags
			$4 = mode parameter string
		@window:
			channel
		@description:
			Triggered when a channel mode has been changed, AFTER all the mode specific events
			were triggered. $3 contains the unparsed mode flags (-o+b...) and $4 contains
			the unparsed mode parameter string (you need to split it!).
	*/
	EVENT("OnChannelModeChange", \
		"$0 = sourcenick\n" \
		"$1 = sourceusername\n" \
		"$2 = sourcehost\n" \
		"$3 = modeflags\n" \
		"$4 = mode params"),

	/*
		@doc: onusermodechange
		@type:
			event
		@title:
			OnUserModeChange
		@short:
			The local user has changed mode
		@parameters:
			$0 = mode flags
		@window:
			Console
		@description:
			Triggered when the local user has changed his user mode.[br]
			The first parameter is the changed mode flag with a leading '+' or '-' sign:
			if the mode was removed it will be something like "-x", otherwise something like "+x".[br]
			Calling 'halt' in this event will stop the "mode" message output.[br]
	*/
	EVENT("OnUserModeChange", \
		"$0 = mode flags"),

	/*
		@doc: onban
		@type:
			event
		@title:
			OnBan
		@short:
			Someone has set a +b flag on the channel
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source hostname
			$3 = ban mask
		@window:
			Channel window
		@description:
			Triggered when someone sets a +b flag on the channel
		@seealso:
			[event:onunban]OnUnban[/event]
	*/
	EVENT("OnBan", \
		"$0 = source nick\n"
		"$1 = source username\n"
		"$2 = source hostname\n"
		"$3 = ban mask"),

	/*
		@doc: onunban
		@type:
			event
		@title:
			OnUnban
		@short:
			Someone has set a -b flag on the channel
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source hostname
			$3 = unban mask
		@window:
			Channel window
		@description:
			Triggered when someone sets a -b flag on the channel
		@seealso:
			[event:onban]OnBan[/event]
	*/
	EVENT("OnUnBan", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = ban mask"),

	/*
		@doc: onmeban
		@type:
			event
		@title:
			OnMeBan
		@short:
			Local user is being banned in a channel
		@parameters:
			$0 = source nickname
			$1 = source username
			$2 = source hostname
			$3 = ban mask
		@window:
			Channel
		@description:
			Triggered when someone has set a +b flag that matches your mask (bans you) in the channel.[br]
		@seealso:
			[event:onmeunban]OnMeUnban[/event]
	*/
	EVENT("OnMeBan", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = ban mask"),

	/*
		@doc: onmeunban
		@type:
			event
		@title:
			OnMeUnban
		@short:
			Local user is being de-banned in a channel
		@parameters:
			$0 = source nickname
			$1 = source username
			$2 = source hostname
			$3 = ban mask
		@window:
			Channel
		@description:
			Triggered when someone has set a -b flag that matches your mask (debans you) in the channel.[br]
		@seealso:
			[event:onmeban]OnMeBan[/event]
	*/
	EVENT("OnMeUnban", \
		"$0 = source nick\n"
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = ban mask"),

	/*
		@doc: onbanexception
		@type:
			event
		@title:
			OnBanException
		@short:
			Someone has set a +e flag on the channel
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source hostname
			$3 = banexception mask
		@window:
			Channel window
		@description:
			Triggered when someone sets a +e (Ban Exception) flag on the channel
		@seealso:
			[event:onbanexceptionremove]OnBanExceptionRemove[/event]
	*/
	EVENT("OnBanException", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = ban exception mask"),

	/*
		@doc: onbanexceptionremove
		@type:
			event
		@title:
			OnBanExceptionRemove
		@short:
			Someone has set a -e flag on the channel
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source hostname
			$3 = unban mask
		@window:
			Channel window
		@description:
			Triggered when someone sets a -e (Removed a Ban Exception) flag on the channel
		@seealso:
			[event:onbanexception]OnBanException[/event]
	*/
	EVENT("OnBanExceptionRemove", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = ban exception mask"),

	/*
		@doc: onmebanexception
		@type:
			event
		@title:
			OnMeBanException
		@short:
			Someone has set a +e flag on the local user
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source hostname
			$3 = mask
		@window:
			Channel window
		@description:
			Triggered when someone sets a +e (puts a Ban Exception) flag on the local user in the active channel
		@seealso:
			[event:onmebanexceptionremove]OnMeBanExceptionRemove[/event]
	*/
	EVENT("OnMeBanException", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = mask"),

	/*
		@doc: onmebanexceptionremove
		@type:
			event
		@title:
			OnMeBanExceptionRemove
		@short:
			Someone has set a -e flag on the local user
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source hostname
			$3 = mask
		@window:
			Channel window
		@description:
			Triggered when someone sets a -e (removes a Ban Exception) flag on the local user in the active channel
		@seealso:
			[event:onmebanexception]OnMeBanException[/event]
	*/
	EVENT("OnMeBanExceptionRemove", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = mask"),

	/*
		@doc: oninvite
		@type:
			event
		@title:
			OnInvite
		@short:
			The local user has received an invitation
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = channel
		@window:
			Active window/console
		@description:
			Triggered when someone invites the local user to join a channel
	*/
	EVENT("OnInvite", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = channel"),

	/*
		@doc: oninviteexception
		@type:
			event
		@title:
			OnInviteException
		@short:
			A +I mode has been set
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = target mask
		@window:
			Channels window
		@description:
			Someone has set mode +I on a channel
		@seealso:
			[event:oninviteexceptionremove]OnInviteExceptionRemove[/event]
	*/
	EVENT("OnInviteException", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = target mask"),

	/*
		@doc: oninviteexceptionremove
		@type:
			event
		@title:
			OnInviteExceptionRemove
		@short:
			Mode -I has been set
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = target mask
		@window:
			Channels window
		@description:
			Someone has set mode -I on a channel
		@seealso:
			[event:oninviteexceptionremove]OnInviteExceptionRemove[/event]
	*/
	EVENT("OnInviteExceptionRemove", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = target mask"),

	/*
		@doc: onmeinviteexception
		@type:
			event
		@title:
			OnMeInviteException
		@short:
			A +I mode has been set on the local user
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = mask
		@window:
			Channels window
		@description:
			Someone has set mode +I on a channel with a target mask that matches
			the local user mask
		@seealso:
			[event:onmeinviteexceptionremove]OnMeInviteExceptionRemove[/event]
	*/
	EVENT("OnMeInviteException", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = mask"),

	/*
		@doc: onmeinviteexceptionremove
		@type:
			event
		@title:
			OnMeInviteExceptionRemove
		@short:
			Move -I has been set
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = mask
		@window:
			Channels window
		@description:
			Someone has set mode -I on a channel with a target mask that matches
			the local user mask
		@seealso:
			[event:onmeinviteexceptionremove]OnMeInviteExceptionRemove[/event]
	*/
	EVENT("OnMeInviteExceptionRemove", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = mask"),

	/*
		@doc: onlimitset
		@type:
			event
		@title:
			OnLimitSet
		@short:
			The channel users limit has been set
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = limit
		@window:
			Channels window
		@description:
			Triggered when someone change or sets the active channel user limit (+l)
		@seealso:
			[event:onlimitunset]OnLimitUnset[/event]
	*/
	EVENT("OnLimitSet", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = limit"),

	/*
		@doc: onlimitunset
		@type:
			event
		@title:
			OnLimitUnSet
		@short:
			The channel users limit has been unset
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
		@window:
			Channels window
		@description:
			Triggered when someone unsets the active channel user limit (-l)
		@seealso:
			[event:onlimitunset]OnLimitUnset[/event]
	*/
	EVENT("OnLimitUnset", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname"),

	/*
		@doc: onkeyset
		@type:
			event
		@title:
			OnKeySet
		@short:
			The channel users key has been set
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = key
		@window:
			Channels window
		@description:
			Triggered when someone change or sets the active channel access key (+k)
		@seealso:
			[event:onkeyunset]OnKeyUnset[/event]
	*/
	EVENT("OnKeySet", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = key"),

	/*
		@doc: onkeyunset
		@type:
			event
		@title:
			OnKeyUnset
		@short:
			The channel users key has been unset
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
		@window:
			Channels window
		@description:
			Triggered when someone unsets the active channel access key (-k)
		@seealso:
			[event:onkeyunset]OnKeyUnset[/event]
	*/
	EVENT("OnKeyUnset", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname"),

	/*
		@doc: onnickchange
		@type:
			event
		@title:
			OnNickChange
		@short:
			Someone has changed his nickname
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = new nickname
		@window:
			Console window
		@description:
			Triggered when someone has changed his nickname.
			The change has already been processed by the server, but not by the local KVIrc database.
		@seealso:
			[event:onmenickchange]OnMeNickChange[/event]
	*/
	EVENT("OnNickChange", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = new nickname"),

	/*
		@doc: onmenickchange
		@type:
			event
		@title:
			OnMeNickChange
		@short:
			The local user has changed his nickname
		@parameters:
			$0 = old nick
			$1 = new nickname
		@window:
			Console window
		@description:
			Triggered when the local user is going to change his nickname.
			The change has already been processed by the server, but not by the local KVIrc database.
		@seealso:
			[event:onnickchange]OnNickChange[/event]
	*/
	EVENT("OnMeNickChange", \
		"$0 = old nick\n" \
		"$1 = new nick"),

	/*
		@doc: onchanowner
		@type:
			event
		@title:
			OnChanOwner
		@short:
			Someone has given a user the +q flag
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = target nick
		@window:
			Channels window
		@description:
			Triggered when someone sets a +q (has made Channel Owner) flag on a user of the active channel.
		@seealso:
			[event:ondechanowner]OnDeChanOwner[/event]
	*/
	EVENT("OnChanOwner", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = target nick"),

	/*
		@doc: ondechanowner
		@type:
			event
		@title:
			OnDeChanOwner
		@short:
			Someone has given a user the -q flag
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = target nick
		@window:
			Channels window
		@description:
			Triggered when someone sets a -q (DeChanOwner) flag on a user in the active channel.
		@seealso:
			[event:onchanowner]OnChanOwner[/event]
	*/
	EVENT("OnDeChanOwner", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = target nick"),

	/*
		@doc: onmechanowner
		@type:
			event
		@title:
			OnMeChanOwner
		@short:
			Someone has given the +q flag to the local user
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
		@window:
			Channels window
		@description:
			Triggered when someone sets a +q (has made Channel owner) flag on the local user in the active channel.
		@seealso:
			[event:onmedechanowner]OnMeDeChanOwner[/event]
	*/
	EVENT("OnMeChanOwner", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname"),

	/*
		@doc: onmedechanowner
		@type:
			event
		@title:
			OnMeDeChanOwner
		@short:
			Someone has given the -q flag to the local user
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
		@window:
			Channels window
		@description:
			Triggered when someone sets a -q (has removed channel owner status) flag on the local user in the active channel.
		@seealso:
			[event:onmechanowner]OnMeChanOwner[/event]
	*/
	EVENT("OnMeDeChanOwner", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname"),

	/*
		@doc: onchanadmin
		@type:
			event
		@title:
			OnChanAdmin
		@short:
			Someone has given a user the +a flag
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = target nick
		@window:
			Channels window
		@description:
			Triggered when someone sets a +a (has made Channel Administrator) flag on a user of the active channel.
		@seealso:
			[event:ondechanadmin]OnDeChanAdmin[/event]
	*/
	EVENT("OnChanAdmin", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = target nick"),

	/*
		@doc: ondechanadmin
		@type:
			event
		@title:
			OnDeChanAdmin
		@short:
			Someone has given a user the -a flag
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = target nick
		@window:
			Channels window
		@description:
			Triggered when someone sets a -a (DeChanAdmin) flag on a user in the active channel.
		@seealso:
			[event:onchanadmin]OnChanAdmin[/event]
	*/
	EVENT("OnDeChanAdmin", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = target nick"),

	/*
		@doc: onmechanadmin
		@type:
			event
		@title:
			OnMeChanAdmin
		@short:
			Someone has given the +a flag to the local user
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
		@window:
			Channels window
		@description:
			Triggered when someone sets a +a (has made Channel administrator) flag on the local user in the active channel.
		@seealso:
			[event:onmedeop]OnMeDeChanAdmin[/event]
	*/
	EVENT("OnMeChanAdmin", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname"),

	/*
		@doc: onmedechanadmin
		@type:
			event
		@title:
			OnMeDeChanAdmin
		@short:
			Someone has given the -a flag to the local user
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
		@window:
			Channels window
		@description:
			Triggered when someone sets a -a (has removed channel admin status) flag on the local user in the active channel.
		@seealso:
			[event:onmeop]OnMeOp[/event]
	*/
	EVENT("OnMeDeChanAdmin", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname"),

	/*
		@doc: onop
		@type:
			event
		@title:
			OnOp
		@short:
			Someone has given a user the +o flag
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = opped nick
		@window:
			Channels window
		@description:
			Triggered when someone sets a +o (has made Op) flag on a user of the active channel.
		@seealso:
			[event:ondeop]OnDeOp[/event]
	*/
	EVENT("OnOp", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = opped nick"),

	/*
		@doc: ondeop
		@type:
			event
		@title:
			OnDeOp
		@short:
			Someone has given a user the -o flag
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = deopped nick
		@window:
			Channels window
		@description:
			Triggered when someone sets a -o (DeOp) flag on a user in the active channel.
		@seealso:
			[event:onop]OnOp[/event]
	*/
	EVENT("OnDeOp", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = deopped nick"),

	/*
		@doc: onmeop
		@type:
			event
		@title:
			OnMeOp
		@short:
			Someone has given the +o flag to the local user
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
		@window:
			Channels window
		@description:
			Triggered when someone sets a +o (has made Op) flag on the local user in the active channel.
		@seealso:
			[event:onmedeop]OnMeDeOp[/event]
	*/
	EVENT("OnMeOp", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname"),

	/*
		@doc: onmedeop
		@type:
			event
		@title:
			OnMeDeOp
		@short:
			Someone has given the -o flag to the local user
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
		@window:
			Channels window
		@description:
			Triggered when someone sets a -o (has deoped) flag on the local user in the active channel.
		@seealso:
			[event:onmeop]OnMeOp[/event]
	*/
	EVENT("OnMeDeOp", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname"),

	/*
		@doc: onhalfop
		@type:
			event
		@title:
			OnHalfOp
		@short:
			Someone has given a user the +h flag
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = halfopped nick
		@window:
			Channels window
		@description:
			Triggered when someone sets a +h flag on a user of the active channel.
		@seealso:
			[event:ondehalfop]OnDeHalfOp[/event]
	*/
	EVENT("OnHalfOp", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = halfopped nick"),

	/*
		@doc: ondehalfop
		@type:
			event
		@title:
			OnDeHalfOp
		@short:
			Someone has given a user the -h flag
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = dehalfopped nick
		@window:
			Channels window
		@description:
			Triggered when someone sets a -h (dehalfop) flag on a user in the active channel.
		@seealso:
			[event:onhalfop]OnHalfOp[/event]
	*/
	EVENT("OnDeHalfOp", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = dehalfopped nick"),

	/*
		@doc: onmehalfop
		@type:
			event
		@title:
			OnMeHalfOp
		@short:
			Someone has given the local user the +h flag
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
		@window:
			Channels window
		@description:
			Triggered when someone sets a +h (halfop) flag for the local user in the active channel.
		@seealso:
			[event:onmedehalfop]OnMeDeHalfOp[/event]
	*/
	EVENT("OnMeHalfOp", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname"),

	/*
		@doc: onmedehalfop
		@type:
			event
		@title:
			OnMeDeHalfOp
		@short:
			Someone has given the local user the -h flag
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
		@window:
			Channels window
		@description:
			Triggered when someone sets a -h (dehalfop) flag for the local user in the active channel.
		@seealso:
			[event:onmehalfop]OnMeHalfOp[/event]
	*/
	EVENT("OnMeDeHalfOp", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname"),

	/*
		@doc: onvoice
		@type:
			event
		@title:
			OnVoice
		@short:
			Someone has given a user the +v flag
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = voiced nick
		@window:
			Channels window
		@description:
			Triggered when someone sets a +v (voice) flag to someone in the active channel.
		@seealso:
			[event:ondevoice]OnDeVoice[/event]
	*/
	EVENT("OnVoice", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = voiced nick"),

	/*
		@doc: ondevoice
		@type:
			event
		@title:
			OnDeVoice
		@short:
			Someone has given a user the -v flag
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = devoiced nick
		@window:
		Channels window
		@description:
			Triggered when someone sets a -v (devoice) flag to someone in the active channel.
		@seealso:
			[event:ondevoice]OnDeVoice[/event]
	*/
	EVENT("OnDeVoice", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = voiced nick"),

	/*
		@doc: onmevoice
		@type:
			event
		@title:
			OnMeVoice
		@short:
			Someone has give the local user a +v flag
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
		@window:
			Channels window
		@description:
			Triggered when someone sets a +v (devoice) flag for the local user in the active channel.
		@seealso:
			[event:onmedevoice]OnMeDeVoice[/event]
	*/
	EVENT("OnMeVoice", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname"),

	/*
		@doc: onmedevoice
		@type:
			event
		@title:
			OnMeDeVoice
		@short:
			Someone has give the local user a -v flag
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
		@window:
			Channels window
		@description:
			Triggered when someone sets a -v (devoice) flag for the local user in the active channel.
		@seealso:
			[event:onmevoice]OnMeVoice[/event]
	*/
	EVENT("OnMeDeVoice", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname"),

	/*
		@doc: onuserop
		@type:
			event
		@title:
			OnUserOp
		@short:
			Someone has given a user the +u flag
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = useropped nick
		@window:
			Channels window
		@description:
			Triggered when someone sets a +u flag on a user of the active channel.
		@seealso:
			[event:ondeuserop]OnDeUserOp[/event]
	*/
	EVENT("OnUserOp", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = useropped nick"),

	/*
		@doc: ondeuserop
		@type:
			event
		@title:
			OnDeUserOp
		@short:
			Someone has given a user a -u flag
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = deuseropped nick
		@window:
			Channels window
		@description:
			Triggered when someone sets a -u (deuserop) flag on a user in the active channel.
		@seealso:
			[event:onuserop]OnUserOp[/event]
	*/
	EVENT("OnDeUserOp", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = deuseropped nick"),

	/*
		@doc: onmeuserop
		@type:
			event
		@title:
			OnMeUserOp
		@short:
			Someone has given the local user the +u flag
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
		@window:
			Channels window
		@description:
			Triggered when someone sets a +u (userop) flag for the local user in the active channel.
		@seealso:
			[event:onmedeuserop]OnMeDeUserOp[/event]
	*/
	EVENT("OnMeUserOp", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname"),

	/*
		@doc: onmedeuserop
		@type:
			event
		@title:
			OnMeDeUserOp
		@short:
			Someone has given the local user the -u flag
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
		@window:
			Channels window
		@description:
			Triggered when someone sets a -u (deuserop) flag for the local user in the active channel.
		@seealso:
			[event:onmeuserop]OnMeUserOp[/event]
	*/
	EVENT("OnMeDeUserOp", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname"),

	/*
		@doc: onircop
		@type:
			event
		@title:
			OnIrcOp
		@short:
			Someone has given a user the +O flag
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = ircopped nick
		@window:
			Channels window
		@description:
			Triggered when someone sets a +O (has made IrcOp) flag on a user of the active channel.
		@seealso:
			[event:ondeircop]OnDeIrcOp[/event]
	*/
	EVENT("OnIrcOp", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = ircopped nick"),

	/*
		@doc: ondeircop
		@type:
			event
		@title:
			OnDeIrcOp
		@short:
			Someone has given a user the -O flag
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = deircopped nick
		@window:
			Channels window
		@description:
			Triggered when someone sets a -O (DeIrcOp) flag on a user in the active channel.
		@seealso:
			[event:onircop]OnIrcOp[/event]
	*/
	EVENT("OnDeIrcOp", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = deircopped nick"),

	/*
		@doc: onmeircop
		@type:
			event
		@title:
			OnMeIrcOp
		@short:
			Someone has given the +O flag to the local user
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
		@window:
			Channels window
		@description:
			Triggered when someone sets a +O (has made IrcOp) flag on the local user in the active channel.
		@seealso:
			[event:onmedeircop]OnMeDeIrcOp[/event]
	*/
	EVENT("OnMeIrcOp", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname"),

	/*
		@doc: onmedeircop
		@type:
			event
		@title:
			OnMeDeIrcOp
		@short:
			Someone has given the -O flag to the local user
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source host
		@window:
			Channels window
		@description:
			Triggered when someone sets a -O (DeIrcOp) flag on the local user in the active channel.
		@seealso:
			[event:onmeircop]OnMeIrcOp[/event]
	*/
	EVENT("OnMeDeIrcOp", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname"),

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
			$0 = ChanServ nick
			$1 = ChanServ username
			$2 = ChanServ host
			$3 = message
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
	EVENT("OnChanServNotice", \
		"$0 = ChanServ nick\n" \
		"$1 = ChanServ username\n" \
		"$2 = ChanServ host\n" \
		"$3 = message"),

	/*
		@doc: onnickservnotice
		@type:
			event
		@title:
			OnNickServNotice
		@short:
			A notice from NickServ has been received
		@parameters:
			$0 = NickServ nick
			$1 = NickServ username
			$2 = NickServ host
			$3 = message
		@window:
			console
		@description:
			Triggered when a notice is received and the source user is recognized as
			the NickServ service (the source nick is 'NickServ' or its mask matches 
			a mask defined in the Nickserver identify options).[br]
			Calling [cmd]halt[/cmd] stops the message output.
		@seealso:
			[event:onquerynotice]OnQueryNotice[/event]
			[event:onchanservnotice]OnChanServNotice[/event]
			[event:onmemoservnotice]OnMemoServNotice[/event]
	*/
	EVENT("OnNickServNotice", \
		"$0 = NickServ nick\n" \
		"$1 = NickServ username\n" \
		"$2 = NickServ host\n" \
		"$3 = message"),

	/*
		@doc: OnNickServAuth
		@type:
			event
		@title:
			OnNickServAuth
		@short:
			Triggered when ircd sets a registered user mode.
		@parameters:
		@window:
			any
		@description:
			Triggered when ircd sets a registered user mode.
	*/
	EVENT("OnNickServAuth",""),

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
			$0 = source nick
			$1 = source username
			$2 = source host
			$3 = message target
			$4 = action message text
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
			Calling [cmd]halt[/cmd] in this event stops the message output.[br]
	*/
	EVENT("OnAction", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = message target\n" \
		"$4 = action message text"),

	/*
		@doc: onmeaction
		@type:
			event
		@title:
			OnMeAction
		@short:
			User issued a CTCP ACTION
		@parameters:
			$0 = action message text
			$1 = action target (channel or nickname)
		@window:
			Channels and query windows
		@description:
			This event is triggered when user issues a CTCP ACTION.[br]
			(The CTCP ACTION is usually triggered by the /me command).[br]
			Warning: don't use a /me inside this event handler, or you'll get an infinite recursion.
			If you need to halt the action (eg. to do some text substitution), follow the example code.
		@examples:
			[example]
				ctcp $1 action "this text was changed in scripting, old one was $0";
				echo -i=$msgtype(action) -w=$active "$me this text was changed in scripting, old one was $0";
				halt;
			[/example]
	*/
	EVENT("OnMeAction", \
		"$0 = action message text\n" \
		"$1 = action target"),

	/*
		@doc: onctcprequest
		@type:
			event
		@title:
			OnCTCPRequest
		@short:
			A CTCP request has been received
		@parameters:
			$0 = source nick
			$1 = source user
			$2 = source host
			$3 = target
			$4 = ctcp type
			$5- = ctcp parameters
		@window:
			Console window
		@description:
			A CTCP request has been received.[br]
			If you call [cmd]halt[/cmd] in this event, you will stop the further processing of the CTCP (thus, you can disable some of the KVIrc features).[br]
			Be careful when using this.
		@seealso:
			[event:onctcpreply]OnCTCPReply[/event]
	*/
	EVENT("OnCTCPRequest", \
		"$0 = source nick\n" \
		"$1 = source user\n" \
		"$2 = source host\n" \
		"$3 = target\n" \
		"$4 = ctcp type\n" \
		"$5- = ctcp parameters"),

	/*
		@doc: onctcpreply
		@type:
			event
		@title:
			OnCTCPReply
		@short:
			A CTCP reply has been received
		@parameters:
			$0 = source nick
			$1 = source user
			$2 = source host
			$3 = target
			$4 = ctcp type
			$5- = ctcp parameters
		@window:
			Console window
		@description:
			A CTCP reply has been received.[br]
			If you call [cmd]halt[/cmd] in this event, you will stop the further processing of the CTCP (thus, you can disable some of the KVIrc features).[br]
			Be careful when using this.
		@seealso:
			[event:onctcpreply]OnCTCPReply[/event]
	*/
	EVENT("OnCTCPReply", \
		"$0 = source nick\n" \
		"$1 = source user\n" \
		"$2 = source host\n" \
		"$3 = target\n" \
		"$4 = ctcp type\n" \
		"$5- = ctcp parameters"),

	/*
		@doc: onctcpflood
		@type:
			event
		@title:
			OnCTCPFlood
		@short:
			Someone tried to flood you with CTCP requests
		@parameters:
			$0 = source nick
			$1 = source username
			$2 = source hostname
			$3 = target
			$4 = ctcp type
			$5- = ctcp parameters
		@window:
			Console window
		@description:
			On IRC there is a kind of attack called 'CTCP Flood'. It consist of a lot of CTCP request in a short period of time, trying to made your client reply to them and made the server kick you. This event is triggered when one of such attacks is detected.
		@seealso:
			[event:onctcprequest]OnCTCPRequest[/event]
			[event:onctcpreply]OnCTCPReply[/event]
	*/
	EVENT("OnCTCPFlood", \
		"$0 = source nick\n" \
		"$1 = source username\n" \
		"$2 = source hostname\n" \
		"$3 = target\n" \
		"$4 = ctcp type\n" \
		"$5- = ctcp parameters"),

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
	EVENT("OnDCCSessionCreated", \
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
	EVENT("OnDCCSessionDestroyed", \
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
	EVENT("OnDCCChatWindowCreated", \
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
	EVENT("OnDCCChatConnectionInProgress", \
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
	EVENT("OnDCCChatConnected", \
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
			$0 = message text
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
	EVENT("OnDCCChatMessage", \
		"$0 = text\n" \
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
			$0 = error message
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
	EVENT("OnDCCChatError", \
		"$0 = error message\n" \
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
	EVENT("OnDCCChatDisconnected", \
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
	EVENT("OnDCCChatWindowClosing", \
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
	EVENT("OnDCCFileTransferConnectionInProgress", \
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
	EVENT("OnDCCFileTransferBegin", \
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
			$0 = bytes transferred
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
	EVENT("OnDCCFileTransferSuccess", \
		"$0 = bytes transferred\n" \
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
			$0 = failure reason message
			$1 = bytes transferred
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
	EVENT("OnDCCFileTransferFailed", \
		"$0 = failure reason\n" \
		"$1 = bytes transferred\n" \
		"$2 = DCC session ID"),

	// Link clicks
	/*
		@doc: OnChannelNickLinkClick
		@type:
			event
		@title:
			OnChannelNickLinkClick
		@short:
			Clicked on nick in channel
		@parameters:
			$0 = nick list
		@window:
			channel
		@description:
			Triggered when user clicks on nickname in channel window
			It will be triggered only at the left mouse button click
	*/
	EVENT("OnChannelNickLinkClick", \
		"$0 = nick list"),

	/*
		@doc: OnQueryNickLinkClick
		@type:
			event
		@title:
			OnQueryNickLinkClick
		@short:
			Triggered when user clicks on nickname in query window
		@parameters:
			$0 = nickname list
		@window:
			query
		@description:
			Triggered when user clicks on nickname in query window
			It will be triggered only at the left mouse button click
	*/
	EVENT("OnQueryNickLinkClick", \
		"$0 = nickname list"),

	/*
		@doc: OnConsoleNickLinkClick
		@type:
			event
		@title:
			OnConsoleNickLinkClick
		@short:
			Triggered when user clicks on nickname in console window
		@parameters:
			$0 = nickname
		@window:
			console
		@description:
			Triggered when user clicks on nickname in console window
			It will be triggered only at the left mouse button click
	*/
	EVENT("OnConsoleNickLinkClick", \
		"$0 = nickname"),

	/*
		@doc: OnHostLinkClick
		@type:
			event
		@title:
			OnHostLinkClick
		@short:
			Triggered when user clicks on any hostname in any window
		@parameters:
			$0 = host name
		@window:
			any
		@description:
			Triggered when user clicks on any hostname in any window
			It will be triggered only at the left mouse button click
	*/
	EVENT("OnHostLinkClick", \
		"$0 = host name"),

	/*
		@doc: OnURLLinkClick
		@type:
			event
		@title:
			OnURLLinkClick
		@short:
			Triggered when user clicks on any URL link in any window
		@parameters:
			$0 = url
		@window:
			any
		@description:
			Triggered when user clicks on any URL link in any window
			It will be triggered only at the left mouse button click
	*/
	EVENT("OnURLLinkClick", \
		"$0 = url"),

	/*
		@doc: OnChannelLinkClick
		@type:
			event
		@title:
			OnChannelLinkClick
		@short:
			Triggered when user clicks on any channel name in any window
		@parameters:
			$0 = channel name
		@window:
			any
		@description:
			Triggered when user clicks on any channel name in any window
			It will be triggered only at the left mouse button click
	*/
	EVENT("OnChannelLinkClick", \
		"$0 = channel name"),

	/*
		@doc: OnServerLinkClick
		@type:
			event
		@title:
			OnServerLinkClick
		@short:
			Triggered when user clicks on server name in any window
		@parameters:
			$0 = server name
		@window:
			any
		@description:
			Triggered when user clicks on server name in any window
			It will be triggered only at the left mouse button click
	*/
	EVENT("OnServerLinkClick", \
		"$0 = server name"),

	/*
		@doc: ontextviewdoublecliked
		@type:
			event
		@title:
			OnTextViewDoubleClicked
		@short:
			Text view link double click customization
		@parameters:
			$0 = actual name
			$1 - visible name
			$2 - command name
		@window:
			The window that the user has clicked in
		@description:
			Actual name is equal to the command name if command name is not null.
			Visible name is the link text (as it shown by user).
			Command name is set if the visible name differs with the actual link target (probably contains mIRC colors,etc). It is equal to the actual target name.
			This event is triggered when an user has double clicked on a text view and has not hit any link.[br]
			I have no idea what you can do in this event....maybe some sort of channel-central dialog? :)
	*/
	EVENT("OnTextViewDoubleClicked",""),

	/*
		@doc: onnicklinkdefaultactionrequest
		@type:
			event
		@title:
			OnNickLinkDefaultActionRequest
		@short:
			Nick link double click customization
		@parameters:
			$0 = actual name
			$1 - visible name
			$2 - command name
		@window:
			The window that the user has clicked in
		@description:
			Actual name is equal to the command name if command name is not null.
			Visible name is the link text (as it shown by user)
			Command name is set if the visible name differs with the actual link target(probably contains mIRC colors,etc). It is equal to the actual target name.
			This event is triggered when an user has double clicked on a nickname link.[br]
			You should use this event to perform some significant action associated with the double click.[br]
			A good example might be a [cmd]whois[/cmd] or a [cmq]query[/cmd] call.[br]
			Please note that NOT ALL nickname links will cause this event to be triggered: some nickname
			links default actions are actually handled by the
			[event:onquerynickdefaultactionrequest]OnQueryNickDefaultActionRequest[/event] and
			[event:onchannelnickdefatulactionrequest]OnChannelNickDefaultActionRequest[/event] events.[br]
	*/
	EVENT("OnNickLinkDefaultActionRequest", \
		"$0 = actual name\n" \
		"$1 - visible name\n" \
		"$2 - command name"),

	/*
		@doc: onchannelnickdefaultactionrequest
		@type:
			event
		@title:
			OnChannelNickDefaultActionRequest
		@short:
			Allows customization of the double click on nicknames
		@parameters:
			$0 = nick list
		@window:
			Channel window
		@description:
		@seealso:
			[event:onchannelnickpopuprequest]OnChannelNickPopupRequest[/event]
	*/
	EVENT("OnChannelNickDefaultActionRequest", \
		"$0 = nick list"),

	/*
		@doc: onquerynickdefaultactionrequest
		@type:
			event
		@title:
			OnQueryNickDefaultActionRequest
		@short:
			Query double click customization
		@parameters:
			$0 = actual name
			$1 - visible name
			$2 - command name
		@window:
			The window that the user has clicked in
		@description:
			Actual name is equal to the command name if command name is not null.
			Visible name is the link text (as it shown by user)
			Command name is set if the visible name differs with the actual link target(probably contains mIRC colors,etc). It is equal to the actual target name.
			This event is triggered when an user has double clicked on a nickname in
			the query target list.[br]
			You should use this event to do some significant action associated with the double click.[br]
			A good example might be a [cmd]whois[/cmd] query or a [cmd]dcc.chat[/cmd]
	*/
	EVENT("OnQueryNickDefaultActionRequest", \
		"$0 = actual name\n" \
		"$1 - visible name\n" \
		"$2 - command name"),

	/*
		@doc: onwindowtitlerequest
		@type:
			event
		@title:
			OnWindowTitleRequest
		@short:
			Triggered when a window title needs to be set
		@parameters:
			none
		@window:
			Any window
		@description:
			This event is triggered when a KVIrc MDI or undocked window is in need of a window title change.[br]
			You can set a customized window title and halt the default title from being set.[br]
		@seealso:
			[cmd]window.setWindowTitle[cmd]
	*/
	EVENT("OnWindowTitleRequest", \
		"$0 = window id\n" \
		"$1 = default window title"),

	/*
		@doc: oninputbarpaste
		@type:
			event
		@title:
			OnInputBarPaste
		@short:
			Triggered when the user paste some text on the inputbar
		@parameters:
			none
		@window:
			Any window
		@description:
			This event is triggered when the user paste some text on the input bar.[br]
			You can check the text, insert any other text and halt the orginal from being inserted in the input bar.[br]
			Note that the text can contains binary data, end-of-line or other characters that could need a special treatment then parsed.
		@seealso:
			[cmd]window.insertInInputText[cmd]
	*/
	EVENT("OnInputBarPaste", \
		"$0 = window id\n" \
		"$1 = pasted text"),

	/*
		@doc: onmemoservnotice
		@type:
			event
		@title:
			OnMemoServNotice
		@short:
			A notice from MemoServ has been received
		@parameters:
			$0 = MemoServ nick
			$1 = MemoServ username
			$2 = MemoServ host
			$3 = message
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
	EVENT("OnMemoServNotice", \
		"$0 = MemoServ nick\n" \
		"$1 = MemoServ username\n" \
		"$2 = MemoServ host\n" \
		"$3 = message")

};
