//=============================================================================
//
//   File : KviKvsCoreSimpleCommands_af.cpp
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

#include "KviWindow.h"
#include "kvi_out.h"
#include "KviLocale.h"
#include "KviApplication.h"
#include "KviOptions.h"
#include "KviIrcView.h" // this is only for KviIrcView::NoTimestamp
#include "KviDebugWindow.h"
#include "KviConsoleWindow.h"
#include "KviScriptButton.h"
#include "KviIconManager.h"
#include "KviChannelWindow.h"
#include "KviQueryWindow.h"

#include "KviKvsEventManager.h"
#include "KviKvsKernel.h"
#include "KviKvsObjectController.h"

#include <map>

#if defined(COMPILE_X11_SUPPORT) && defined(COMPILE_QX11INFO_SUPPORT)
#ifndef COMPILE_NO_X_BELL
#include "KviXlib.h" // XBell : THIS SHOULD BE INCLUDED AS LAST!
#include <unistd.h>  // for usleep();

#include <QX11Info>
#define get_xdisplay QX11Info::display
#endif
#else
#ifndef COMPILE_NO_X_BELL
#define COMPILE_NO_X_BELL
#endif
#endif
#include "KviTalToolTip.h"

// KviApplication.cpp
extern std::map<QString, KviWindow *> g_pGlobalWindowDict;

namespace KviKvsCoreSimpleCommands
{
	/*
		@doc: admin
		@type:
			command
		@title:
			admin
		@syntax:
			admin [target server]
		@short:
			Requests the admin info from a server
		@description:
			Requests admin information from the specified server or the current server if no [target server] is specified.[br]
			This command is an [doc:rfc2812wrappers]RFC2812 command wrapper[/doc]; see that document for more information.[br]
	*/
	// RFC2812 wrapper

	/*
		@doc: away
		@type:
			command
		@title:
			away
		@syntax:
			away [-a | --all-networks] [-d | --default-message] [<reason:string>]
		@short:
			Puts you into 'away' state
		@switches:
			!sw: -a | --all-networks
			Set away on all networks
			!sw: -d | --default-message
			If you do not specify a message, this switch overrides the boolUseAwayMessage option
			and sets you away with the default message.
		@description:
			Puts you into 'away' state in the connection associated to the
			current [b]IRC context[/b].[br] This command is "server based";
			this means that the effects will be visible only after the
			server has acknowledged the change.[br]
			When you use this command, other people will know that you are
			away from the keyboard, and they will know why you're not here.[br]
			To return from being away you must use [cmd]back[/cmd].[br]
			This command is [doc:connection_dependent_commands]connection dependent[/doc].[br]
			If no away reason is specified, the behaviour of this command depends from the value
			of the option boolUseAwayMessage: if enabled, user will be set away using the default
			away message (or [i]Away from keyboard[/i] if no default away message is set);
			otherwise, the user will return from a previous away state.
		@examples:
			[example]
				away I'm asleep. Don't wake me up.
			[/example]
	*/

	KVSCSC(away)
	{
		QString szReason;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("reason", KVS_PT_STRING, KVS_PF_OPTIONAL | KVS_PF_APPENDREMAINING, szReason)
		KVSCSC_PARAMETERS_END

		if(szReason.isEmpty())
		{
			if(KVI_OPTION_BOOL(KviOption_boolUseAwayMessage) || KVSCSC_pSwitches->find('d', "default-message"))
			{
				//user want to use its default away message
				szReason = KVI_OPTION_STRING(KviOption_stringAwayMessage);
				if(szReason.isEmpty())
					szReason = __tr2qs("Away from keyboard.");
			}
			else
			{
				//user want to /back
				if(KVSCSC_pSwitches->find('a', "all-networks"))
				{
					for(auto & wnd : g_pGlobalWindowDict)
					{
						if(wnd.second->type() == KviWindow::Console)
						{
							KviConsoleWindow * pConsole = (KviConsoleWindow *)wnd.second;
							if(pConsole->isConnected())
								pConsole->connection()->sendFmtData("AWAY");
						}
					}
				}
				else
				{
					KVSCSC_REQUIRE_CONNECTION

					if(!(KVSCSC_pConnection->sendFmtData("AWAY")))
						return KVSCSC_pContext->warningNoIrcConnection();
				}
				return true;
			}
		}

		if(KVSCSC_pSwitches->find('a', "all-networks"))
		{
			for(auto & wnd : g_pGlobalWindowDict)
			{
				if(wnd.second->type() == KviWindow::Console)
				{
					KviConsoleWindow * pConsole = (KviConsoleWindow *)wnd.second;
					if(pConsole->isConnected())
						pConsole->connection()->sendFmtData("AWAY :%s",
						    pConsole->connection()->encodeText(szReason).data());
				}
			}
		}
		else
		{
			KVSCSC_REQUIRE_CONNECTION

			QByteArray szR = KVSCSC_pConnection->encodeText(szReason);
			if(!(KVSCSC_pConnection->sendFmtData("AWAY :%s", szR.data())))
				return KVSCSC_pContext->warningNoIrcConnection();
		}

		return true;
	}

	/*
		@doc: back
		@type:
			command
		@title:
			back
		@syntax:
			back [-a | --all-networks]
		@switches:
			!sw: -a | --all-networks
			Set back on all networks
		@short:
			Allows you to return from being away
		@description:
			Using this command makes you return from being [cmd]away[/cmd] in the connection associated to the
			current [b]IRC context[/b].[br] This command is "server based";
			this means that the effects will be visible only after the
			server has acknowledged the change.[br]
			This command is [doc:connection_dependent_commands]connection dependent[/doc].[br]
		@examples:
			[example]
			back
			[/example]
	*/

	KVSCSC(back)
	{
		if(KVSCSC_pSwitches->find('a', "all-networks"))
		{
			for(auto & wnd : g_pGlobalWindowDict)
			{
				if(wnd.second->type() == KviWindow::Console)
				{
					KviConsoleWindow * pConsole = (KviConsoleWindow *)wnd.second;
					if(pConsole->isConnected())
						pConsole->connection()->sendFmtData("AWAY");
				}
			}
		}
		else
		{
			KVSCSC_REQUIRE_CONNECTION

			if(!(KVSCSC_pConnection->sendFmtData("AWAY")))
				return KVSCSC_pContext->warningNoIrcConnection();
		}

		return true;
	}

	/*
		@doc: ban
		@type:
			command
		@title:
			ban
		@syntax:
			ban <mask_list>
		@short:
			Sets ban masks for the channel
		@description:
			Sets the ban masks specified in the <mask_list>,
			which is a comma separated list of nicknames.
			This command works only if executed in a channel window.
			The command is translated to a set of MODE messages containing
			a variable number of +b flags.
			This command is [doc:connection_dependent_commands]connection dependent[/doc].
		@examples:
			[example]
			ban Maxim,Gizmo!*@*,*!root@*
			[/example]
		@seealso:
			[cmd]op[/cmd],
			[cmd]deop[/cmd],
			[cmd]voice[/cmd],
			[cmd]devoice[/cmd],
			[cmd]unban[/cmd]
	*/

	KVSCSC(ban)
	{
		return multipleModeCommand(__pContext, __pParams, __pSwitches, '+', 'b');
	}

	/*
	 	@doc: beep
		@type:
			command
		@title:
			beep
		@syntax:
			beep [-p=<pitch:uint>] [-d=<duration:uint>] [-s] [volume:uint]
		@short:
			Beep beep!
		@switches:
			!sw: -p=<pitch:uint> | --pitch=<pitch:uint>
			Sets the bell to the specified pitch, if possible
			!sw: -d=<duration:uint> | --duration=<duration:uint>
			Sets the duration of the beep to <duration> milliseconds
			!sw: -s | --synchronous
			Causes KVIrc to wait for completion of the beeping before
			returning from this command
		@description:
			Beeps (when possible :D)[br]
			...[br]
			No, really...[br]
			This command rings the bell on the keyboard (the PC speaker).
			The volume must be in range 0-100; the default is 100.[br]
			The pitch is specified in Hz and must be positive.[br]
			The duration is specified in milliseconds.[br]
			An invalid (or unspecified) pitch, volume or duration
			makes KVIrc to use the default values set by the system.[br]
			The duration of the bell is only indicative and
			can be shortened by a subsequent call to /beep (that
			will override the currently playing one).[br]
			If the -s switch is specified the bell becomes synchronous:
			KVIrc waits the bell to complete before continuing.[br]
			The precision of the bell pitch, duration and
			volume is strongly dependent on the system and the underlying hardware.[br][br]
			On Windows, the bell is always synchronous and it is not
			event granted that the bell will be a bell at all... you might
			get the system default sound instead...
			On some security-enhanced Windows variants (2008 Server, Seven and possibly
			some 64-bit Vista builds) the access to the PC speaker
			may be silently denied so don't be surprised too much if you don't hear any
			sound at all on such systems...[br][br]
			WARNING[br][br]
			The main KVIrc thread is stopped while a synchronous bell is playing
			so if you play long notes (duration > 100) the entire application may appear to
			freeze for a while. Be careful :)[br]
		@seealso:
			[cmd]snd.play[/cmd]
	*/

	KVSCSC(beep)
	{
		kvs_uint_t uVolume;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("volume", KVS_PT_UINT, KVS_PF_OPTIONAL, uVolume)
		KVSCSC_PARAMETERS_END

		kvs_int_t pitch = -1;
		kvs_int_t duration = -1;

		KviKvsVariant * pPitch = KVSCSC_pSwitches->find('p', "pitch");
		if(pPitch)
		{
			if(!pPitch->asInteger(pitch))
			{
				KVSCSC_pContext->warning(__tr2qs_ctx("Invalid pitch value: using default", "kvs"));
				pitch = -1;
			}
		}

		KviKvsVariant * pDuration = KVSCSC_pSwitches->find('d', "duration");
		if(pDuration)
		{
			if(!pDuration->asInteger(duration))
			{
				KVSCSC_pContext->warning(__tr2qs_ctx("Invalid duration value: using default", "kvs"));
				duration = -1;
			}
		}

		if((uVolume > 100) || (uVolume < 1))
			uVolume = 100;

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		Beep(pitch, duration);
#elif defined(COMPILE_X11_SUPPORT) && (!defined(COMPILE_NO_X_BELL))
		bool bSync = (KVSCSC_pSwitches->find('s', "sync") != nullptr);

		XKeyboardState st;
		XKeyboardControl ctl;

		XGetKeyboardControl(get_xdisplay(), &st);

		unsigned long mask = KBBellPercent;
		ctl.bell_percent = uVolume;
		if(pitch >= 0)
		{
			ctl.bell_pitch = pitch;
			mask |= KBBellPitch;
		}
		if(duration >= 0)
		{
			ctl.bell_duration = duration;
			mask |= KBBellDuration;
		}
		XChangeKeyboardControl(get_xdisplay(), mask, &ctl);

		XBell(get_xdisplay(), 100);

		if(bSync)
		{
			if(duration >= 0)
				usleep(duration * 1000);
			else
				usleep(st.bell_duration * 1000);
		}

		ctl.bell_pitch = st.bell_pitch;
		ctl.bell_duration = st.bell_duration;
		ctl.bell_percent = st.bell_percent;

		XChangeKeyboardControl(get_xdisplay(), mask, &ctl);
#endif //COMPILE_X11_SUPPORT

		return true;
	}

	/*
		@doc: buttonctl
		@title:
			buttonctl
		@type:
			command
		@short:
			changes user definable buttons
		@syntax:
			buttonctl [-q] <type:string> <name:string> <operation:string> [parameter:string]
		@switches:
			!sw: -q | --quiet
			Run quietly
		@description:
			Changes an existing user defined button.[br]
			<type_unused> is ignored and present only for backward compatibility.[br]
			<name> is the name of the button.[br]
			<operation> may be one of the constant strings [i]enable[/i], [i]disable[/i], [i]image[/i],
			[i]text"[/i].[br]
			Operations [i]enable[/i] and [i]disable[/i] do not require the fourth [parameter] and have
			the obvious meaning.[br] Operation [i]image[/i] requires the [parameter] to be
			a valid [doc:image_id]image_id[/doc] and sets the button image.[br]
			Operation [i]text[/i] requires the [parameter] (and in fact all the following ones)
			to be a string containing the button text label.[br]
			The <operation> constants may be abbreviated, even to the single letters [b]e[/b], [b]d[/b], [b]i[/b] and [b]t[/b].[br]
			The -q switch causes the command to be quiet about errors and warnings.[br]
		@seealso:
			[cmd]button[/cmd]
	*/

	//#warning	"ALSO /HELP must NOT interpret the identifiers!"
	//#warning	"ALSO /DEBUG that relays to the DEBUG WINDOW"

	KVSCSC(buttonctl)
	{
		QString tbTypeUnused, tbName, tbOp, tbPar;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("type", KVS_PT_STRING, 0, tbTypeUnused)
		KVSCSC_PARAMETER("name", KVS_PT_STRING, 0, tbName)
		KVSCSC_PARAMETER("operation", KVS_PT_STRING, 0, tbOp)
		KVSCSC_PARAMETER("parameter", KVS_PT_STRING, KVS_PF_OPTIONAL, tbPar)
		KVSCSC_PARAMETERS_END

		KviScriptUserButton * pButton = nullptr;

		if(!KVSCSC_pWindow->buttonContainer())
		{
			if(!KVSCSC_pSwitches->find('q', "quiet"))
				KVSCSC_pContext->warning(__tr2qs_ctx("The specified window has no button containers", "kvs"));
			return true;
		}

		//pButton = (KviScriptUserButton *)(KVSCSC_pWindow->buttonContainer())->child(tbName.toUtf8().data(),"KviWindowScriptButton");
		pButton = (KviScriptUserButton *)(KVSCSC_pWindow->buttonContainer())->findChild<KviWindowScriptButton *>(tbName);

		if(!pButton)
		{
			if(!KVSCSC_pSwitches->find('q', "quiet"))
				KVSCSC_pContext->warning(__tr2qs_ctx("No button with type %Q named %Q", "kvs"), &tbTypeUnused, &tbName);
			return true;
		}
		QChar o;
		if(tbOp.length() > 0)
			o = tbOp[0];
		else
			o = QChar('x');

		//	QChar o = tbOp.length() > 0 ? tbOp[0] : QChar('x');

		switch(o.unicode())
		{
			case 't':
				KviTalToolTip::remove(pButton);
				KviTalToolTip::add(pButton, tbPar);
				pButton->setButtonText(tbPar);
				break;
			case 'i':
				if(!tbPar.isEmpty())
				{
					QPixmap * pix = g_pIconManager->getImage(tbPar);
					if(pix)
					{
						pButton->setButtonPixmap(*pix);
					}
					else
					{
						if(!KVSCSC_pSwitches->find('q', "quiet"))
							KVSCSC_pContext->warning(__tr2qs_ctx("Can't find the icon '%Q'", "kvs"), &tbPar);
					}
				}
				break;
			case 'e':
				pButton->setEnabled(true);
				break;
			case 'd':
				pButton->setEnabled(false);
				break;
		}
		return true;
	}

	/*
		@doc: cap
		@type:
			command
		@title:
			cap
		@syntax:
			cap <command:string> [parameters:string]
		@short:
			Sends a CAP command
		@description:
			Sends a CAP command to the server.[br]
			The <command> may be one of LS,REQ,LIST and ACK as per CAP specification (Google for draft-mitchell-irc-capabilities-02).
			The <parameters> string usually contains a list of capabilities to be activated or deactivated (deactivation is triggered
			by prepending a - prefix to the capability name.[br]
			This command is [doc:connection_dependent_commands]connection dependent[/doc] and actually works only on servers
			that implement the CAP capability.
		@examples:
			[example]
				cap LS
				cap REQ identify-msg
				cap LIST
				cap REQ -identify-msg
			[/example]
	*/

	KVSCSC(cap)
	{
		QString szCommand, szParams;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("command", KVS_PT_NONEMPTYSTRING, 0, szCommand)
		KVSCSC_PARAMETER("parameters", KVS_PT_STRING, KVS_PF_OPTIONAL | KVS_PF_APPENDREMAINING, szParams)
		KVSCSC_PARAMETERS_END

		KVSCSC_REQUIRE_CONNECTION

		QByteArray szEncodedCommand = KVSCSC_pConnection->encodeText(szCommand);
		QByteArray szEncodedParams = KVSCSC_pConnection->encodeText(szParams);

		if(szEncodedParams.isEmpty())
		{
			if(!(KVSCSC_pConnection->sendFmtData("CAP %s", szEncodedCommand.data())))
				return KVSCSC_pContext->warningNoIrcConnection();
		}
		else
		{
			if(!(KVSCSC_pConnection->sendFmtData("CAP %s :%s", szEncodedCommand.data(), szEncodedParams.data())))
				return KVSCSC_pContext->warningNoIrcConnection();
		}

		return true;
	}

	/*
		@doc: chanadmin
		@type:
			command
		@title:
			chanadmin
		@syntax:
			chanadmin <nickname_list>
		@short:
			Sets chan admin status from the specified users
		@description:
			Sets channel administrator status to the users specified in <nickname_list>,
			which is a comma separated list of nicknames.
			This command works only if executed in a channel window.
			The command is translated to a set of MODE messages containing
			a variable number of +a flags.
			This command is [doc:connection_dependent_commands]connection dependent[/doc].
		@examples:
			[example]
				chanadmin Pragma,Crocodile
			[/example]
		@seealso:
			[cmd]chanowner[/cmd], [cmd]dechanowner[/cmd], [cmd]dechanadmin[/cmd], [cmd]op[/cmd], [cmd]deop[/cmd], [cmd]halfop[/cmd], [cmd]dehalfop[/cmd], [cmd]voice[/cmd], [cmd]devoice[/cmd], [cmd]userop[/cmd], [cmd]deuserop[/cmd]
	*/

	KVSCSC(chanadmin)
	{
		return multipleModeCommand(__pContext, __pParams, __pSwitches, '+', 'a');
	}

	/*
		@doc: chanowner
		@type:
			command
		@title:
			chanowner
		@syntax:
			chanowner <nickname_list>
		@short:
			Sets chan owner status from the specified users
		@description:
			Sets channel owner status to the users specified in <nickname_list>,
			which is a comma separated list of nicknames.
			This command works only if executed in a channel window.
			The command is translated to a set of MODE messages containing
			a variable number of +q flags.
			This command is [doc:connection_dependent_commands]connection dependent[/doc].
		@examples:
			[example]
				chanowner Pragma,Crocodile
			[/example]
		@seealso:
			[cmd]dechanowner[/cmd], [cmd]chanadmin[/cmd], [cmd]dechanadmin[/cmd], [cmd]op[/cmd], [cmd]deop[/cmd], [cmd]halfop[/cmd], [cmd]dehalfop[/cmd], [cmd]voice[/cmd], [cmd]devoice[/cmd], [cmd]userop[/cmd], [cmd]deuserop[/cmd]
	*/

	KVSCSC(chanowner)
	{
		return multipleModeCommand(__pContext, __pParams, __pSwitches, '+', 'q');
	}

	/*
		@doc: codepage
		@type:
			command
		@title:
			codepage
		@syntax:
			codepage <encoding name>
		@short:
			Tries to set the codepage on server
		@description:
			This is a not-widely implemented extension
			that allows the user to set the codepage mapping
			on server.
			This command is an [doc:rfc2812wrappers]RFC2812 command wrapper[/doc]; see that document for more information.[br]
	*/
	// RFC2812 wrapper

	/*
		@doc: ctcp
		@type:
			command
		@title:
			CTCP
		@syntax:
			ctcp [-n] <target:string> <ctcp_data:string>
		@short:
			Sends a CTCP message
		@description:
			Sends a CTCP message to the specified <target>.[br]
			The target may be a nickname, a channel, or a comma separated list of nicknames.[br]
			The <ctcp_data> is a string containing the CTCP type followed by the CTCP parameters.[br]
			For more info take a look at the [doc:ctcp_handling]CTCP protocol implementation notes[/doc].[br]
			The CTCP message will be a request (sent through a PRIVMSG) unless the -n switch
			specified: in that case it will be a reply (sent through a NOTICE).[br]
			If <ctcp_data> is the single string [i]ping[/i] then a trailing time string argument
			is added in order to determine the round trip time when the ping reply comes back.
			To override this behaviour simply specify your own time string parameter.[br]
			This command is [doc:connection_dependent_commands]connection dependent[/doc].[br]
		@examples:
			[example]
				ctcp Pragma VERSION
			[/example]
	*/

	KVSCSC(ctcp)
	{
		QString szTarget, szCtcpCmd, szCtcpData;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("target", KVS_PT_NONEMPTYSTRING, 0, szTarget)
		KVSCSC_PARAMETER("ctcp_cmd", KVS_PT_STRING, KVS_PF_OPTIONAL, szCtcpCmd)
		KVSCSC_PARAMETER("ctcp_data", KVS_PT_STRING, KVS_PF_OPTIONAL | KVS_PF_APPENDREMAINING, szCtcpData)
		KVSCSC_PARAMETERS_END

		KVSCSC_REQUIRE_CONNECTION

		QString szData = szCtcpCmd;

		if(szCtcpCmd.compare("PING", Qt::CaseInsensitive) == 0 && szCtcpData.isEmpty())
		{
			struct timeval tv;
			kvi_gettimeofday(&tv);
			KviQString::appendFormatted(szCtcpData, "%d.%d", tv.tv_sec, tv.tv_usec);
		}
		else if (szCtcpCmd.compare("ACTION", Qt::CaseInsensitive) == 0 && !KVSCSC_pSwitches->find('n', "notice"))
		{
			KviWindow * w = KVSCSC_pConnection->findChannel(szTarget);
			if(!w)
				w = KVSCSC_pConnection->findQuery(szTarget);
			if(w)
			{
				w->ownAction(szCtcpData);
				return true;
			}
		}


		if(!szCtcpData.isEmpty())
			szData += " " + szCtcpData;

		QByteArray szT = KVSCSC_pConnection->encodeText(szTarget);
		QByteArray szD = KVSCSC_pConnection->encodeText(szData);

		if(!(KVSCSC_pConnection->sendFmtData("%s %s :%c%s%c",
		       KVSCSC_pSwitches->find('n', "notice") ? "NOTICE" : "PRIVMSG", szT.data(), 0x01, szD.data(), 0x01)))
			return KVSCSC_pContext->warningNoIrcConnection();

		return true;
	}

	/*
		@doc: debug
		@type:
			command
		@title:
			debug
		@syntax:
			debug [-s] [-c] <text:string>
		@short:
			Outputs text to the debug window
		@switches:
			!sw: -s | --system-console
			Output to the system console instead of the debug window.
			!sw: -c | --scriptcontext-name
			Prepend the script's context name to the output.
		@description:
			If the -s switch is not specified then outputs the &lt;text&gt; to the debug window.
			If the -s switch is used then the output is sent to the system console instead.
			If the -c switch is used then the script's context name will be prepended to the output.
			This is useful if you need to debug destructors of objects
			or events that are triggered at KVIrc shutdown.
		@seealso:
	*/

	KVSCSC(debugCKEYWORDWORKAROUND)
	{
		QString szAll;
		KVSCSC_pParams->allAsString(szAll);
		if(KVSCSC_pSwitches->find('c', "scriptcontext-name"))
			szAll.prepend(__pContext->script()->name() + ": ");
		if(KVSCSC_pSwitches->find('s', "system-console"))
			qDebug("%s", szAll.toUtf8().data());
		else
		{
			KviWindow * pWnd = KviDebugWindow::getInstance();
			pWnd->outputNoFmt(KVI_OUT_NONE, szAll);
		}
		return true;
	}

	/*
		@doc: dechanadmin
		@type:
			command
		@title:
			dechanadmin
		@syntax:
			dechanadmin <nickname_list>
		@short:
			Removes chan admin status from the specified users
		@description:
			Removes channel admin status to the users specified in <nickname_list>,
			which is a comma separated list of nicknames.
			This command works only if executed in a channel window.
			The command is translated to a set of MODE messages containing
			a variable number of -a flags.
			This command is [doc:connection_dependent_commands]connection dependent[/doc].
		@examples:
			[example]
				dechanadmin Pragma,Crocodile
			[/example]
		@seealso:
			[cmd]chanowner[/cmd], [cmd]dechanowner[/cmd], [cmd]chanadmin[/cmd], [cmd]op[/cmd], [cmd]deop[/cmd], [cmd]halfop[/cmd], [cmd]dehalfop[/cmd], [cmd]voice[/cmd], [cmd]devoice[/cmd], [cmd]userop[/cmd], [cmd]deuserop[/cmd]
	*/

	KVSCSC(dechanadmin)
	{
		return multipleModeCommand(__pContext, __pParams, __pSwitches, '-', 'a');
	}

	/*
		@doc: dechanowner
		@type:
			command
		@title:
			dechanowner
		@syntax:
			dechanowner <nickname_list>
		@short:
			Removes chan owner status from the specified users
		@description:
			Removes channel owner status to the users specified in <nickname_list>,
			which is a comma separated list of nicknames.
			This command works only if executed in a channel window.
			The command is translated to a set of MODE messages containing
			a variable number of -q flags.
			This command is [doc:connection_dependent_commands]connection dependent[/doc].
		@examples:
			[example]
				dechanowner Pragma,Crocodile
			[/example]
		@seealso:
			[cmd]chanowner[/cmd], [cmd]chanadmin[/cmd], [cmd]dechanadmin[/cmd], [cmd]op[/cmd], [cmd]deop[/cmd], [cmd]halfop[/cmd], [cmd]dehalfop[/cmd], [cmd]voice[/cmd], [cmd]devoice[/cmd], [cmd]userop[/cmd], [cmd]deuserop[/cmd]
	*/

	KVSCSC(dechanowner)
	{
		return multipleModeCommand(__pContext, __pParams, __pSwitches, '-', 'q');
	}

	/*
		@doc: dehalfop
		@type:
			command
		@title:
			dehalfop
		@syntax:
			dehalfop <nickname_list>
		@short:
			Removes half op status from the specified users
		@description:
			Removes channel half operator status to the users specified in <nickname_list>,
			which is a comma separated list of nicknames.
			This command works only if executed in a channel window.
			The command is translated to a set of MODE messages containing
			a variable number of -h flags.
			This command is [doc:connection_dependent_commands]connection dependent[/doc].
		@examples:
			[example]
				dehalfop Pragma,Crocodile
			[/example]
		@seealso:
			[cmd]op[/cmd], [cmd]deop[/cmd], [cmd]halfop[/cmd], [cmd]voice[/cmd], [cmd]devoice[/cmd]
	*/

	KVSCSC(dehalfop)
	{
		return multipleModeCommand(__pContext, __pParams, __pSwitches, '-', 'h');
	}

	/*
		@doc: delete
		@type:
			command
		@title:
			delete
		@syntax:
			delete [-q] [-i] <objectHandle>
		@short:
			Destroys an object
		@switches:
			!sw: -q | --quiet
			Causes the command to run quietly
		@description:
			Schedules for destruction the object designed by <objectHandle>.
			This command is internally aliased to [cmd]destroy[/cmd].
			Please note that the object is [b]not[/b] immediately destroyed:
			it will be destroyed when KVIrc returns to the main event loop,
			so after the current script code part has been executed.
			This behaviour makes the object destruction safe in any
			part of the script, but may lead to problems when
			using signals and slots.[br]
			For example, when you delete an object that emits some signals,
			the signals may be still emitted after the delete call.
			You have to disconnect the signals explicitly if you don't want it
			to happen.[br]
			The -q switch causes the command to run a bit more silently: it still
			complains if the parameter passed is not an object reference, but
			it fails silently if the reference just points to an inexistent object (or is null).
		@examples:
			[example]
			[/example]
	*/

	/*
		@doc: destroy
		@type:
			command
		@title:
			destroy
		@syntax:
			destroy [-q] <objectHandle>
		@short:
			Destroys an object
		@description:
			This is a builtin alias for the command [cmd]delete[/cmd]
	*/

	KVSCSC(deleteCKEYWORDWORKAROUND)
	{
		kvs_hobject_t hObject = (kvs_hobject_t) nullptr;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("objectHandle", KVS_PT_HOBJECT, 0, hObject)
		KVSCSC_PARAMETERS_END
		if(hObject == (kvs_hobject_t) nullptr)
		{
			if(!KVSCSC_pSwitches->find('q', "quiet"))
				KVSCSC_pContext->warning(__tr2qs_ctx("Can't delete a null object reference", "kvs"));
		}
		else
		{
			KviKvsObject * o = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
			if(!o)
			{
				if(!KVSCSC_pSwitches->find('q', "quiet"))
					KVSCSC_pContext->warning(__tr2qs_ctx("Can't delete a non-existent object", "kvs"));
			}
			else
			{
				// -i | --immediate was too annoying. People were writing self-destructive scripts
				// and then were complaining about -i causing their KVIrc to crash.
				//
				//if(KVSCSC_pSwitches->find('i', "immediate"))
				//	o->dieNow();
				//else
				
				o->die();
			}
		}
		return true;
	}

	/*
		@doc: delpopupitem
		@type:
			command
		@title:
			delpopupitem
		@syntax:
			delpopupitem [-d] [-q] <popupname:string> <item_id:string>
		@short:
			Deletes an item from a popup
		@switches:
			!sw: -q | --quiet
			Run quietly
			!sw: -d | --deep
			Search the whole popup tree instead of only the first level
		@description:
			This is an internal alias for [cmd]popup.delitem[/cmd].[br]
			This function is deprecated and its use is discouraged.
		@seealso:
			[cmd]popup.delitem[/cmd]
	*/

	KVSCSC(delpopupitem)
	{
		QString szParams;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("params", KVS_PT_STRING, KVS_PF_OPTIONAL | KVS_PF_APPENDREMAINING, szParams)
		KVSCSC_PARAMETERS_END

		// We just alias the popup.delitem function
		QString szSwitches = "";
		if(KVSCSC_pSwitches->find('d', "deep"))
			szSwitches.append("-d ");
		if(KVSCSC_pSwitches->find('q', "quiet"))
			szSwitches.append("-q ");

		KviQString::escapeKvs(&szParams);
		KviKvsScript s("delpopupitem", "popup.delitem " + szSwitches + szParams);
		s.run(KVSCSC_pContext->window());
		return true;
	}

	/*
		@doc: deop
		@type:
			command
		@title:
			deop
		@syntax:
			deop <nickname_list>
		@short:
			Removes chanop status from the specified users
		@description:
			Removes channel operator status to the users specified in <nickname_list>,
			which is a comma separated list of nicknames.
			This command works only if executed in a channel window.
			The command is translated to a set of MODE messages containing
			a variable number of -o flags.
			This command is [doc:connection_dependent_commands]connection dependent[/doc].
		@examples:
			[example]
			deop Pragma,Crocodile
			[/example]
		@seealso:
			[cmd]chanowner[/cmd], [cmd]dechanowner[/cmd], [cmd]chanadmin[/cmd], [cmd]dechanadmin[/cmd], [cmd]op[/cmd], [cmd]halfop[/cmd], [cmd]dehalfop[/cmd], [cmd]voice[/cmd], [cmd]devoice[/cmd], [cmd]userop[/cmd], [cmd]deuserop[/cmd]
	*/

	KVSCSC(deop)
	{
		return multipleModeCommand(__pContext, __pParams, __pSwitches, '-', 'o');
	}

	/*
		@doc: deuserop
		@type:
			command
		@title:
			deuserop
		@syntax:
			deuserop <nickname_list>
		@short:
			Removes user op status from the specified users
		@description:
			Removes channel user operator status to the users specified in <nickname_list>,
			which is a comma separated list of nicknames.
			This command works only if executed in a channel window.
			The command is translated to a set of MODE messages containing
			a variable number of -u flags.
			This command is [doc:connection_dependent_commands]connection dependent[/doc].
		@examples:
			[example]
			deuserop Pragma,Crocodile
			[/example]
		@seealso:
			[cmd]chanowner[/cmd], [cmd]dechanowner[/cmd], [cmd]chanadmin[/cmd], [cmd]dechanadmin[/cmd], [cmd]op[/cmd], [cmd]deop[/cmd], [cmd]halfop[/cmd], [cmd]dehalfop[/cmd], [cmd]voice[/cmd], [cmd]devoice[/cmd], [cmd]userop[/cmd]
	*/

	KVSCSC(deuserop)
	{
		return multipleModeCommand(__pContext, __pParams, __pSwitches, '-', 'u');
	}

	/*
		@doc: devoice
		@type:
			command
		@title:
			devoice
		@syntax:
			devoice <nickname_list>
		@short:
			Removes voice flag for the specified users
		@description:
			Removes the voice flag for the users specified in <nickname_list>,
			which is a comma separated list of nicknames.
			This command works only if executed in a channel window.
			The command is translated to a set of MODE messages containing
			a variable number of -v flags.
			This command is [doc:connection_dependent_commands]connection dependent[/doc].
		@examples:
			[example]
			devoice Pragma,Crocodile
			[/example]
		@seealso:
			[cmd]chanowner[/cmd], [cmd]dechanowner[/cmd], [cmd]chanadmin[/cmd], [cmd]dechanadmin[/cmd], [cmd]op[/cmd], [cmd]deop[/cmd], [cmd]halfop[/cmd], [cmd]dehalfop[/cmd], [cmd]voice[/cmd], [cmd]devoice[/cmd], [cmd]userop[/cmd]
	*/

	KVSCSC(devoice)
	{
		return multipleModeCommand(__pContext, __pParams, __pSwitches, '-', 'v');
	}

	/*
		@doc: die
		@type:
			command
		@title:
			die
		@syntax:
			die <message:text>
		@short:
			Prints an error message and stops the script
		@description:
			Prints an error message and stops the current script.
			This command is equivalent to [cmd]error[/cmd]
		@seealso:
			[cmd]warning[/cmd]
	*/
	// Internally aliased to error

	/*
		@doc: echo
		@type:
			command
		@title:
			echo
		@syntax:
			echo [-d] [-w=<window_id>] [-i=<color_set>] [-n] <text>
		@short:
			Outputs text to a KVIrc window
		@switches:
			!sw: -w=<window_id> | --window=<window_id>
			Causes the output to be redirected to the window specified by &lt;window_id&gt;
			!sw: -i=<color_set> | --color-set=<color_set>
			Causes the message to use the specified icon scheme (icon and colors).
			!sw: -n | --no-timestamp
			Disables the message timestamping
			!sw: -d | --debug
			Send the output to the debug window (takes precedence over -w)
		@description:
			Outputs the &lt;text&gt; to the current window.[br]
			If the 'w' switch is present, outputs the &lt;text&gt;
			to the specified window instead of the current one.
			The <window_id> parameter is the [doc:window_naming_conventions]global ID[/doc] of the window
			that has to be used.[br]
			If the 'i' switch is given, it uses the specified
			icon scheme (icon and colors), otherwise it uses
			the default one (0).[br]
			If the -d switch is used then the output is sent to a special
			window called [i]Debug[/i] (the window is created if not existing yet).
			This is useful for script debugging purposes (you get the output
			in Debug regardless of the window that the executed command is attached to).
			The KVIrc view widgets support clickable links that can be realized by using special [doc:escape_sequences]escape sequences[/doc].[br]
			The 'n' switch disables timestamping so you can output your own timestamp
			or not timestamp at all.[br]
		@examples:
			[example]
				echo Hey! this is my first echo test!
				echo -i=[fnc]$msgtype[/fnc](parsererror) this has the colors of the parser error messages
			[/example]
		@seealso:
			[fnc]$window[/fnc],
			[fnc]$msgtype[/fnc],
			[doc:window_naming_conventions]window naming conventions documentation[/doc]
	*/

	KVSCSC(echo)
	{
		QString szAll;
		KVSCSC_pParams->allAsString(szAll);

		kvs_int_t iMsgType = KVI_OUT_NONE;
		KviWindow * pWnd = KVSCSC_pContext->window();

		if(!KVSCSC_pSwitches->isEmpty())
		{
			KviKvsVariant * v;
			if((v = KVSCSC_pSwitches->find('w', "window")))
			{
				QString szWnd;
				v->asString(szWnd);
				//#warning "FIXME: the window database is not Unicode! (we even could keep integer window id's at this point!)"
				pWnd = g_pApp->findWindow(szWnd.toUtf8().data());
				if(!pWnd)
				{
					KVSCSC_pContext->warning(__tr2qs_ctx("The argument of the -w switch did not evaluate to a valid window ID: using default", "kvs"));
					pWnd = KVSCSC_pContext->window();
				}
			}

			if((v = KVSCSC_pSwitches->find('i', "color-set")))
			{
				if(!v->asInteger(iMsgType))
				{
					KVSCSC_pContext->warning(__tr2qs_ctx("The argument of the -i switch did not evaluate to a number: using default", "kvs"));
					iMsgType = KVI_OUT_NONE;
				}
				else
				{
					iMsgType = iMsgType % KVI_NUM_MSGTYPE_OPTIONS;
				}
			}

			if(KVSCSC_pSwitches->find('d', "debug"))
			{
				pWnd = KviDebugWindow::getInstance();
			}
		}

		int iFlags = KVSCSC_pSwitches->find('n', "no-timestamp") ? KviIrcView::NoTimestamp : 0;
		pWnd->outputNoFmt(iMsgType, szAll, iFlags);
		return true;
	}

	/*
		@doc: echoprivmsg
		@type:
			command
		@title:
			echoprivmsg
		@syntax:
			echoprivmsg [switches] <nick:string> <user:string> <host:string> <text:string>
		@short:
			Outputs text to a KVIrc window
		@switches:
			!sw: -p[=<nick_prefix>] | --prefix[=<prefix>]
			The message is printed with the specified custom nickname prefix.
			If <nick_prefix> is omitted then an empty string is assumed.
			!sw: -s[=<nick_suffix>] | --suffix[=<nicksuffix>]
			The message is printed with the specified custom nickname suffix.
			If <nick_suffix> is omitted then an empty string is assumed.
			!sw: -w=<window_id> | --window=<window_id>
			The message is printed to the window specified by [doc:window_naming_conventions]window_id[/doc]
			!sw: -i=<color_set> | --color-set=<color_set>
			Causes the message to use the specified icon scheme (icon and colors).
			!sw: -n | --no-highlighting
			Do not apply the highlighting rules
			!sw: -x | --no-notifier
			Never cause the notifier window to pop up
			!sw: -f | --no-flashing
			Never cause the window taskbar entry to flash (this works only on some platforms)
		@description:
			Outputs a <text> to the current window in the privmsg format.[br]
			The <nick> <user> and <host> parameters are formatted
			as specified by the user preferences (for example
			the nick may use smart colorization).
			If you don't know the username and host then just use '*' for
			that parameters.
			The message will also get the highlighting rules applied.
			If the 'w' switch is present, outputs <text>
			to the specified window instead of the current one.
			The <window_id> parameter is the [doc:window_naming_conventions]global ID[/doc] of the window
			that has to be used.[br]
			Please note that this is not the same as the standard
			[doc:command_rebinding]-r rebinding switch[/doc]:
			-w causes the specified window to be used only for output,
			but the command parameters are evaluated in the current window.[br]
			If the 'i' switch is given, it uses the specified
			icon scheme (icon and colors), otherwise it uses
			the default one.[br]
			If the -n switch is present then the highlighting rules
			are not applied.[br]
			If the -x switch is present then the message will never cause
			the notifier window to be popped up.[br]
			If the -f switch is present then the message will never cause
			the system taskbar to flash.[br]
			Actually -x and -f have a sense only if highlighting is used and thus -n is not present.[br]
			For more information about the icon/color schemes see [fnc]$msgtype[/fnc].
			The KVIrc view widgets support clickable sockets that can be realized by using special [doc:escape_sequences]escape sequences[/doc].[br]
		@examples:
			[example]
				echoprivmsg Test * * This is a test message
				echoprivmsg -i=$msgtype(ChanPrivmsgCrypted) Pragma pragma staff.kvirc.net Hi people! :)
			[/example]
		@seealso:
			[fnc]$window[/fnc],
			[fnc]$window.caption[/fnc],
			[fnc]$msgtype[/fnc],
			[cmd]echo[/cmd],
			[doc:window_naming_conventions]Window Naming Conventions[/doc]
	*/

	KVSCSC(echoprivmsg)
	{
		QString szNick, szUser, szHost, szText;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("nick", KVS_PT_NONEMPTYSTRING, 0, szNick)
		KVSCSC_PARAMETER("user", KVS_PT_STRING, 0, szUser)
		KVSCSC_PARAMETER("host", KVS_PT_STRING, 0, szHost)
		KVSCSC_PARAMETER("text", KVS_PT_STRING, KVS_PF_OPTIONAL | KVS_PF_APPENDREMAINING, szText)
		KVSCSC_PARAMETERS_END

		int type = KVI_OUT_NONE;
		KviWindow * pWnd = KVSCSC_pWindow;
		KviConsoleWindow * pConsole = pWnd->console();
		if(!pConsole)
			pConsole = g_pApp->activeConsole();

		KviKvsVariant * v;

		if((v = KVSCSC_pSwitches->find('w', "window")))
		{
			QString szWin;
			v->asString(szWin);
			pWnd = g_pApp->findWindow(szWin);
			if(!pWnd)
			{
				KVSCSC_pContext->warning(__tr2qs_ctx("Window '%Q' not found, using current one", "kvs"), &szWin);
				pWnd = KVSCSC_pWindow;
			}
		}

		if((v = KVSCSC_pSwitches->find('i', "color-set")))
		{
			kvs_int_t msgType;
			if(v->asInteger(msgType))
			{
				if(msgType < 0)
					msgType = -msgType;
				type = (int)(msgType % KVI_NUM_MSGTYPE_OPTIONS);
			}
			else
				KVSCSC_pContext->warning(__tr2qs_ctx("Invalid color-set specification, using default", "kvs"));
		}
		else
		{
			bool bIsMe = pWnd->connection() && KviQString::equalCI(pWnd->connection()->currentNickName(), szNick);

			switch(pWnd->type())
			{
				case KviWindow::Channel:
					type = bIsMe ? KVI_OUT_OWNPRIVMSG : KVI_OUT_CHANPRIVMSG;
					break;
				case KviWindow::Query:
					type = bIsMe ? KVI_OUT_OWNPRIVMSG : KVI_OUT_QUERYPRIVMSG;
					break;
				default:
					// keep KVI_OUT_NONE
					break;
			}
		}

		QString szPrefix, szSuffix;
		bool bPrefix = false;
		bool bSuffix = false;

		if((v = KVSCSC_pSwitches->find('p', "prefix")))
		{
			v->asString(szPrefix);
			bPrefix = true;
		}
		if((v = KVSCSC_pSwitches->find('s', "suffix")))
		{
			v->asString(szSuffix);
			bSuffix = true;
		}

		int iFlags = 0;
		if(KVSCSC_pSwitches->find('n', "no-highlighting"))
			iFlags |= KviConsoleWindow::NoHighlighting;
		if(KVSCSC_pSwitches->find('f', "no-flashing"))
			iFlags |= KviConsoleWindow::NoWindowFlashing;
		if(KVSCSC_pSwitches->find('x', "no-notifier"))
			iFlags |= KviConsoleWindow::NoNotifier;

		pConsole->outputPrivmsg(pWnd, type,
		    szNick, szUser, szHost, szText,
		    iFlags,
		    bPrefix ? szPrefix : QString(), bSuffix ? szSuffix : QString());

		return true;
	}

	/*
		@doc: error
		@type:
			command
		@title:
			error
		@syntax:
			error <message:text>
		@short:
			Prints an error message and stops the script
		@description:
			Prints an error message and stops the current script.
			This command is equivalent to [cmd]die[/cmd]
		@seealso:
			[cmd]warning[/cmd]
	*/

	KVSCSC(error)
	{
		QString szAll;
		KVSCSC_pParams->allAsString(szAll);
		KVSCSC_pContext->error("%Q", &szAll);
		return false;
	}

	/*
		@doc: eval
		@type:
			command
		@title:
			eval
		@syntax:
			eval [-q] [-r=<window:integer>] <command:string>
		@switches:
			!sw: -q | --quiet
			Disable any error output
			!sw: -f | --force
			Continue execution even if <command> fails with an error
		@short:
			Change the behaviour of a set of commands
		@description:
			This command is useful to execute variable command sequences.[br]
			<command> is first evaluated as a normal parameter (thus identifiers
			and variables are substituted) then the evaluated string is executed
			as a command sequence.[br]
			-q causes eval to run quietly and don't display any errors in the inner command.[br]
			-f causes eval to ignore the errors inside <command> and continue execution.[br]
			This command may be used to rebind the <command> to a specified window.
			<command> shares the local variables with this command scope so you
			can easily exchange data with it.
			Remember that <command> is still a normal parameter and it must be
			enclosed in quotes if you want it to be a complex command sequence.
			eval propagates the <command> return value.[br]
			[br]
			[b]WARNING: If some part of the <command> comes from external input you MUST
			sanitize it by using [fnc]$escape[/fnc]() in order to avoid command injection.[/b]
			Note that nicknames, usernames, hostnames, channel names and any kind of text that
			isn't generated under your control may contain malicious code. For instance, if you
			try to eval the string built by concatenating [i]echo[/i] and the result of [fnc]$channel.name[/fnc]
			inside a channel named [i]#test;quit[/i] (yes, that's a valid channel name) you'll obtain
			a disconnection as a side effect. To avoid this you need to use [fnc]$escape[/fnc]() around
			[fnc]$chan.name[/fnc].
		@examples:
			[example]
				[comment]# evaluate a variable command[/comment]
				[cmd]if[/cmd](%somecondition)%tmp = "echo yeah"
				else %tmp = "echo -i=10 yeah"
				eval %tmp
				[comment]# Rebind the command to the #linux channel to get the user list[/comment]
				eval -r=[fnc]$channel[/fnc](#linux) "%Nicks[]=$chan.array;"
				[comment]# A nice alias that allows iterating commands through all the consoles[/comment]
				[comment]# This is by LatinSuD :)[/comment]
				[cmd]alias[/cmd](iterate)
				{
					%ctxt[]=[fnc]$window.list[/fnc](console,all)
					[cmd]for[/cmd](%i=0;%i<%ctxt[]#;%i++)
					{
						[cmd]eval[/cmd] -r=%ctxt[%i] $0-
					}
				}
				iterate [cmd]echo[/cmd] Hi ppl! :)
				[comment]# A little bit shorter (but less "colorful") version might be...[/comment]
				[cmd]alias[/cmd](iterate)
				{
					[cmd]foreach[/cmd](%x,[fnc]$window.list[/fnc](console,all))
							[cmd]eval[/cmd] -r=%x $0-;
				}
				iterate [cmd]echo[/cmd] Hi again!
				[comment]# Evaluate a command block[/comment]
				eval "{ echo First command!; echo Second command!; }"
			[/example]
		@seealso:
			[fnc]$escape[/fnc]
	*/

	KVSCSC(eval)
	{
		QString szCommands;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("commands", KVS_PT_STRING, KVS_PF_APPENDREMAINING, szCommands)
		KVSCSC_PARAMETERS_END

		KviKvsScript s("eval::inner", szCommands);
		int iRunFlags = 0;
		if(KVSCSC_pContext->reportingDisabled() || KVSCSC_pSwitches->find('q', "quiet"))
			iRunFlags |= KviKvsScript::Quiet;
		bool bRet = s.run(KVSCSC_pContext, iRunFlags) ? true : false;
		if(!bRet)
		{
			if(!KVSCSC_pSwitches->find('f', "force"))
				return false;
			KVSCSC_pContext->clearError();
		}
		return true;
	}

	/*
		@doc: eventctl
		@title:
			eventctl
		@type:
			command
		@short:
			Controls the execution of event handlers
		@syntax:
			eventctl [-u] [-e] [-d] [-q] <event_name:string> <handler_name:string> [parameters]
		@switches:
			!sw: -u | --unregister
			Unregisters the specified handler
			!sw: -e | --enable
			Enables the specified handler
			!sw: -d | --disable
			Disables the specified handler
			!sw: -q | --quiet
			Do not print any warnings
		@description:
			Performs control actions on the handler <handler_name> for
			the event <event_name>.
			Without any switch it triggers the handler for testing purposes,
			eventually passing [parameters].[br]
			With the -u switch the handler <handler_name> is unregistered.[br]
			With the -d swtich is is disabled (so it is never executed)
			and with -e is enabled again.[br]
			The <event_name> may be one of the kvirc-builtin event names
			or a numeric code (from 0 to 999) of a raw server message.[br]
		@seealso:
			[cmd]event[/cmd] [fnc]$iseventenabled[/fnc]
	*/
	KVSCSC(eventctl)
	{
		QString szEventName, szHandlerName;
		KviKvsVariantList vList;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("event_name", KVS_PT_NONEMPTYSTRING, 0, szEventName)
		KVSCSC_PARAMETER("handler_name", KVS_PT_NONEMPTYSTRING, 0, szHandlerName)
		KVSCSC_PARAMETER("parameters", KVS_PT_VARIANTLIST, KVS_PF_OPTIONAL, vList)
		KVSCSC_PARAMETERS_END

		bool bOk;
		int iNumber = szEventName.toInt(&bOk);
		bool bIsRaw = (bOk && (iNumber >= 0) && (iNumber < 1000));

		if(bIsRaw)
		{
			if(!KviKvsEventManager::instance()->isValidRawEvent(iNumber))
			{
				if(!KVSCSC_pSwitches->find('q', "quiet"))
					KVSCSC_pContext->warning(__tr2qs_ctx("No such event (%Q)", "kvs"), &szEventName);
				return true;
			}
		}
		else
		{
			KviKvsEventManager::instance()->cleanHandlerName(szHandlerName);
			iNumber = KviKvsEventManager::instance()->findAppEventIndexByName(szEventName);
			if(!KviKvsEventManager::instance()->isValidAppEvent(iNumber))
			{
				if(!KVSCSC_pSwitches->find('q', "quiet"))
					KVSCSC_pContext->warning(__tr2qs_ctx("No such event (%Q)", "kvs"), &szEventName);
				return true;
			}
		}

		if(KVSCSC_pSwitches->find('u', "unregister"))
		{
			// unregister it
			if(bIsRaw)
			{
				if(!KviKvsEventManager::instance()->removeScriptRawHandler(iNumber, szHandlerName))
					if(!KVSCSC_pSwitches->find('q', "quiet"))
						KVSCSC_pContext->warning(__tr2qs_ctx("No handler '%Q' for RAW numeric event '%d'", "kvs"), &szHandlerName, iNumber);
			}
			else
			{
				if(!KviKvsEventManager::instance()->removeScriptAppHandler(iNumber, szHandlerName))
					if(!KVSCSC_pSwitches->find('q', "quiet"))
						KVSCSC_pContext->warning(__tr2qs_ctx("No handler '%Q' for event '%Q'", "kvs"), &szHandlerName, &szEventName);
			}
		}
		else if(KVSCSC_pSwitches->find('e', "enable") || KVSCSC_pSwitches->find('d', "disable"))
		{
			// enable it
			if(bIsRaw)
			{
				if(!KviKvsEventManager::instance()->enableScriptRawHandler(iNumber, szHandlerName, KVSCSC_pSwitches->find('e', "enable")))
					if(!KVSCSC_pSwitches->find('q', "quiet"))
						KVSCSC_pContext->warning(__tr2qs_ctx("No handler '%Q' for RAW numeric event '%d'", "kvs"), &szHandlerName, iNumber);
			}
			else
			{
				if(!KviKvsEventManager::instance()->enableScriptAppHandler(iNumber, szHandlerName, KVSCSC_pSwitches->find('e', "enable")))
					if(!KVSCSC_pSwitches->find('q', "quiet"))
						KVSCSC_pContext->warning(__tr2qs_ctx("No handler '%Q' for event '%Q'", "kvs"), &szHandlerName, &szEventName);
			}
		}
		else
		{
			// trigger it
			KviKvsScriptEventHandler * h;
			QString code;

			if(bIsRaw)
			{
				h = KviKvsEventManager::instance()->findScriptRawHandler(iNumber, szHandlerName);
			}
			else
			{
				h = KviKvsEventManager::instance()->findScriptAppHandler(iNumber, szHandlerName);
			}

			if(h)
			{
				KviKvsScript * s = h->script();
				KviKvsScript copy(*s);
				KviKvsVariant retVal;
				copy.run(KVSCSC_pWindow, &vList, nullptr, KviKvsScript::PreserveParams);
			}
			else
			{
				if(!KVSCSC_pSwitches->find('q', "quiet"))
					KVSCSC_pContext->warning(__tr2qs_ctx("No handler '%Q' for event '%Q'", "kvs"), &szHandlerName, &szEventName);
			}
		}

		return true;
	}

	/*
		@doc: exit
		@type:
			command
		@title:
			exit
		@syntax:
			exit
		@switches:
		@short:
			Closes KVIrc
		@description:
			It closes the KVIrc application
	*/

	KVSCSC(exit)
	{
		g_pApp->quit();
		return true;
	}
};
