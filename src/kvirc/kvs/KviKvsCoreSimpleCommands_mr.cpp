//=============================================================================
//
//   File : KviKvsCoreSimpleCommands_mr.cpp
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
#include "KviConsoleWindow.h"
#include "KviQueryWindow.h"
#include "KviIrcUserDataBase.h"
#include "kvi_out.h"
#include "KviLocale.h"
#include "KviApplication.h"
#include "KviOptions.h"
#include "KviFileUtils.h"
#include "KviFileDialog.h"
#include "KviIrcConnection.h"
#include "KviChannelWindow.h"
#include "KviIrcUrl.h"
#include "KviMainWindow.h"
#include "KviModuleManager.h"
#include "KviKvsModuleInterface.h"

#include "KviKvsVariantList.h"
#include "KviKvsScript.h"
#include "KviKvsPopupManager.h"

#include <QCursor>
#include <QProcess>
#include <QTimer>

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#include <shellapi.h>
#endif

namespace KviKvsCoreSimpleCommands
{
	/*
		@doc: map
		@type:
			command
		@title:
			map
		@syntax:
			map
		@short:
			Displays a map of connected servers
		@description:
			Displays a graphical map of servers along with the current number of users on each server.[br]
			Some IRCds will also display information such as lag, recent splits, and SID.[br]
		@seealso:
			[cmd]links[/cmd]
	*/
	// RFC2812 wrapper

	/*
		@doc: me
		@type:
			command
		@title:
			me
		@syntax:
			me <text:string>
		@short:
			Sends a CTCP ACTION
		@description:
			Sends a CTCP ACTION to the current channel, query or DCC chat.[br]
			If you execute it in any other window type, you will get an error.[br]
			If you want to use this command in a window that is not a channel
			query or DCC chat, you may use the [doc:command_rebinding]standard -r switch[/doc].
		@examples:
			[example]
				me is Hungry!
			[/example]
	*/

	KVSCSC(me)
	{
		QString szText;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("text", KVS_PT_STRING, KVS_PF_OPTIONAL | KVS_PF_APPENDREMAINING, szText)
		KVSCSC_PARAMETERS_END

		switch(KVSCSC_pWindow->type())
		{
			case KviWindow::Channel:
			case KviWindow::Query:
				KVSCSC_REQUIRE_CONNECTION
			case KviWindow::DccChat:
			case KviWindow::DccVideo:
				KVSCSC_pWindow->ownAction(szText);
				break;
			default:
				KVSCSC_pContext->warning(__tr2qs_ctx("/me can be used only in channels, queries and DCC chat windows", "kvs"));
				break;
		}

		return true;
	}

	/*
		@doc: mode
		@type:
			command
		@title:
			mode
		@syntax:
			mode <target> <modeflags> [mode parameters]
		@short:
			Sends a MODE IRC message
		@description:
			Sends a MODE IRC message to the server of the current IRC context.[br]
			The parameters are not modified in any way by KVIrc: so
			you should use the RFC1459 syntax.[br]
			This command is [doc:connection_dependent_commands]connection dependent[/doc].
		@examples:
			[example]
			mode #kvirc +oo-b Pragma Buti *!*root@*
			[/example]
	*/

	KVSCSC(mode)
	{
		QString szText;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("text", KVS_PT_STRING, KVS_PF_APPENDREMAINING, szText)
		KVSCSC_PARAMETERS_END

		KVSCSC_REQUIRE_CONNECTION

		QByteArray szTxt = KVSCSC_pConnection->encodeText(szText);

		if(!szTxt.isEmpty())
		{
			if(!KVSCSC_pConnection->sendFmtData("MODE %s", szTxt.data()))
				return KVSCSC_pContext->warningNoIrcConnection();
		}

		return true;
	}

	/*
		@doc: motd
		@type:
			command
		@title:
			motd
		@syntax:
			motd [target server]
		@short:
			Requests the Message of the day
		@description:
			Requests the Message of the day from the specified server or the current server if no [target server] is specified.[br]
			This command is an [doc:rfc2812wrappers]RFC2812 command wrapper[/doc]; see that document for more information.[br]
	*/
	// RCF2812 wrapper

	/*
		@doc: msg
		@type:
			command
		@title:
			msg
		@syntax:
			msg [-q] <target:string> <text:string>
		@short:
			Alias for privmsg
		@switches:
			!sw: -q | --quiet
			Do not print any output
		@description:
			Sends a private message to the specified <target>.
			<target> may be any PRIVMSG target allowed by the underlying
			IRC protocol (see RFC1459).
			This is really similar to [cmd]privmsg[/cmd] but also outputs the
			message locally (unless the [-q] switch is used).[br]
			This command is [doc:connection_dependent_commands]connection dependent[/doc].[br]
	*/
	// Internally aliased to privmsg

	/*
		@doc: nick
		@type:
			command
		@title:
			nick
		@syntax:
			nick <new nickname:string>
		@short:
			Changes your nickname
		@description:
			Changes your nickname in the connection associated to the
			current [b]IRC context[/b].[br] This command is "server based";
			this means that the effects will be visible only after the
			server has acknowledged the change.[br]
			This command is [doc:connection_dependent_commands]connection dependent[/doc].[br]
		@examples:
			[example]
				nick Pragma
			[/example]
	*/

	KVSCSC(nick)
	{
		QString szNick;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("nickname", KVS_PT_NONEMPTYSTRING, 0, szNick)
		KVSCSC_PARAMETERS_END

		KVSCSC_REQUIRE_CONNECTION

		QByteArray szData = KVSCSC_pConnection->encodeText(szNick);

		if(!KVSCSC_pConnection->sendFmtData("NICK %s", szData.data()))
			return KVSCSC_pContext->warningNoIrcConnection();

		return true;
	}

	/*
		@doc: notice
		@type:
			command
		@title:
			notice
		@syntax:
			notice [-q] <target:string> <text:string>
		@short:
			Sends a private notice
		@switches:
			!sw: -q | --quiet
			Do not print any output
		@description:
			Sends a private notice to the specified <target>.
			<target> may be any NOTICE target allowed by the underlying
			IRC protocol (see RFC1459).
			If the [-q] switch is specified, no output is printed.
			This command is [doc:connection_dependent_commands]connection dependent[/doc].[br]
		@examples:
			[example]
				notice Pragma Hello!
				notice Pragma,Crocodile Hello to you both!
				notice #kvirc Hello from outside!
			[/example]
	*/

	KVSCSC(notice)
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

		QByteArray szT = KVSCSC_pConnection->encodeText(szTarget);
		QByteArray szD = w ? w->encodeText(szText) : KVSCSC_pConnection->encodeText(szText);

		if(!(KVSCSC_pConnection->sendFmtData("NOTICE %s :%s", szT.data(), szD.data())))
			return KVSCSC_pContext->warningNoIrcConnection();

		if(!KVSCSC_pSwitches->find('q', "quiet"))
			KVSCSC_pWindow->output(KVI_OUT_OWNPRIVMSG, "[NOTICE >>> \r!nc\r%Q\r]: %Q", &szTarget, &szText);

		return true;
	}

	/*
		@doc: op
		@type:
			command
		@title:
			op
		@syntax:
			op <nickname_list>
		@short:
			Sets op status to the specified users
		@description:
			Sets channel operator status to the users specified in <nickname_list>,
			which is a comma separated list of nicknames.
			This command works only if executed in a channel window.
			The command is translated to a set of MODE messages containing
			a variable number of +o flags.
			This command is [doc:connection_dependent_commands]connection dependent[/doc].
		@examples:
			[example]
				op Pragma,Crocodile
			[/example]
		@seealso:
			[cmd]chanowner[/cmd], [cmd]dechanowner[/cmd], [cmd]chanadmin[/cmd], [cmd]dechanadmin[/cmd], [cmd]deop[/cmd], [cmd]halfop[/cmd], [cmd]dehalfop[/cmd], [cmd]voice[/cmd], [cmd]devoice[/cmd], [cmd]userop[/cmd], [cmd]deuserop[/cmd]
	*/

	KVSCSC(op)
	{
		return multipleModeCommand(__pContext, __pParams, __pSwitches, '+', 'o');
	}

	/*
		@doc: openurl
		@type:
			command
		@title:
			openurl
		@syntax:
			openurl <url:string>
		@short:
			Opens an URL
		@description:
			Opens the specified <url> with an appropriate handler.[br]
			The handlers for the supported URL types are specified in the options dialog.[br]
			Each handler is a KVIrc commandline that the URL will be passed to as the first parameter ($0).[br]
			The supported URL types are:[br]
			[b]HTTP[/b]: http://&lt;url&gt; or sth that begins with [b][i]www.[/b][/i][br]
			[b]HTTPS[/b]: https://&lt;url&gt;[br]
			[b]FILE[/b]: file://&lt;url&gt;[br]
			[b]IRC[/b]: irc[s][6]://<server>[:<port>][/<channel>[?<pass>]] (Handled internally)[br]
			[b]FTP[/b]: ftp://<url> or sth that begins with "ftp."[br]
			[b]MAIL[/b]: mailto:<mailaddress>[br]
		@examples:
			[example]
				openurl http://www.kvirc.net
				openurl https://www.secure.net
				openurl file://home/pragma/pippo.txt
				openurl irc://irc.eu.dal.net:6667
				openurl irc6://irc.ircd.it/#kvirc
				openurl ircs://crypto.azzurra.org:9999
				openurl ircs6://ngnet.azzurra.org:9999
				openurl ftp://ftp.localhost.net/pub/kvirc/
				openurl mailto:users@domain.extension
			[/example]
	*/

	KVSCSC(openurl)
	{
		QString szUrl;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("url", KVS_PT_NONEMPTYSTRING, KVS_PF_APPENDREMAINING, szUrl)
		KVSCSC_PARAMETERS_END

		QString szCommand;
		if(KviQString::equalCIN(szUrl, "www.", 4))
		{
			szCommand = KVI_OPTION_STRING(KviOption_stringUrlHttpCommand);
			szUrl.prepend("http://");
		}
		else if(KviQString::equalCIN(szUrl, "http:", 5))
		{
			szCommand = KVI_OPTION_STRING(KviOption_stringUrlHttpCommand);
		}
		else if(KviQString::equalCIN(szUrl, "https:", 6))
		{
			szCommand = KVI_OPTION_STRING(KviOption_stringUrlHttpsCommand);
		}
		else if(KviQString::equalCIN(szUrl, "ftp", 3) || KviQString::equalCIN(szUrl, "sftp", 4) ||
		        KviQString::equalCIN(szUrl, "ftps", 4) || KviQString::equalCIN(szUrl, "ftpes", 4))
		{
			szCommand = KVI_OPTION_STRING(KviOption_stringUrlFtpCommand);
			if(KviQString::equalCIN(szUrl, "ftp.", 4))
				szUrl.prepend("ftp://");
		}
		else if(KviQString::equalCIN(szUrl, "file", 4))
		{
			szCommand = KVI_OPTION_STRING(KviOption_stringUrlFileCommand);
		}
		else if(KviQString::equalCIN(szUrl, "irc", 3))
		{
			if(KviIrcUrl::run(szUrl, KviIrcUrl::TryEveryContext | KviIrcUrl::DoNotPartChans, KVSCSC_pContext->console()) & KviIrcUrl::InvalidProtocol)
			{
				KVSCSC_pContext->warning(__tr2qs_ctx("Invalid IRC URL (%Q)", "kvs"), &szUrl);
			}
			return true;
		}
		else if(KviQString::equalCIN(szUrl, "mailto", 6))
		{
			szCommand = KVI_OPTION_STRING(KviOption_stringUrlMailtoCommand);
		}
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		if(KVI_OPTION_BOOL(KviOption_boolUseSystemUrlHandlers))
		{
			intptr_t iRet = (intptr_t)::ShellExecute(nullptr, TEXT("open"), szUrl.toStdWString().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
			if(iRet <= 32)
			{
				/*
					The ShellExecute() function returns the value 31 if there is no association for the specified file
					type or if there is no association for the specified action within the file type.
					The other possible error values are as follows:

					Value          Meaning
					---------------------------------------------------------------------------
					   0             System was out of memory, executable file was corrupt, or
					                 relocations were invalid.
					   2             File was not found.
					   3             Path was not found.
					   5             Attempt was made to dynamically link to a task, or there
					                 was a sharing or network-protection error.
					   6             Library required separate data segments for each task.
					   8             There was insufficient memory to start the application.
					  10             Windows version was incorrect.
					  11             Executable file was invalid. Either it was not a Windows
					                 application, or there was an error in the .exe image.
					  12             Application was designed for a different operating system.
					  13             Application was designed for MS-DOS 4.0.
					  14             Type of executable file was unknown.
					  15             Attempt was made to load a real-mode application
					                (developed for an earlier version of Windows).
					  16             Attempt was made to load a second instance of an
					                 executable file containing multiple data segments that
					                 were not marked read-only.
					  19             Attempt was made to load a compressed executable file. The
					                 file must be decompressed before it can be loaded.
					  20             Dynamic-link library (DLL) file was invalid. One of the
					                 DLLs required to run this application was corrupt.
					  21             Application requires Microsoft Windows 32-bit extensions.
				*/
				// FIXME: Write a better error message
				KVSCSC_pContext->warning(__tr2qs_ctx("The system handler for the URL failed to execute: system error is %1", "kvs").arg(iRet));
			}
		}
		else
		{
#endif
			if(szCommand.isEmpty())
				szCommand = KVI_OPTION_STRING(KviOption_stringUrlUnknownCommand);

			if(!szCommand.isEmpty())
			{
				KviKvsVariantList vList;
				vList.append(new KviKvsVariant(szUrl));

				QString szName = "openurl::handler";
				KviKvsScript script(szName, szCommand);

				if(!script.run(KVSCSC_pWindow, &vList, nullptr, KviKvsScript::PreserveParams))
					KVSCSC_pContext->warning(__tr2qs_ctx("The commandline for this URL type seems to be broken (%Q)", "kvs"), &szUrl);
			}
			else
				KVSCSC_pContext->warning(__tr2qs_ctx("No commandline specified for this type of URL (%Q)", "kvs"), &szUrl);
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		}
#endif

		return true;
	}

	/*
		@doc: oper
		@type:
			command
		@title:
			oper
		@syntax:
			oper <name> <password>
		@short:
			Requests IRC operator status
		@description:
			Requests IRC operator status.[br]
			This command is an [doc:rfc2812wrappers]RFC2812 command wrapper[/doc]; see that document for more information.[br]
	*/
	// RFC2812 wrapper

	/*
		@doc: operwall
		@type:
			command
		@title:
			operwall
		@syntax:
			operwall <message:text>
		@short:
			Broadcasts an OPERWALL message
		@description:
			Broadcasts an OPERWALL message to all users whom are set
			to receive WALLOPS messages.
		@seealso:
			[cmd]wallops[/cmd]
	*/

	KVSCSC(operwall)
	{
		QString szMessage;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("message", KVS_PT_NONEMPTYSTRING, KVS_PF_APPENDREMAINING, szMessage)
		KVSCSC_PARAMETERS_END

		KVSCSC_REQUIRE_CONNECTION

		QByteArray szData = KVSCSC_pContext->connection()->encodeText(szMessage);

		if(!KVSCSC_pContext->connection()->sendFmtData("OPERWALL :%s", szData.data()))
			return KVSCSC_pContext->warningNoIrcConnection();

		return true;
	}

	/*
		@doc: option
		@type:
			command
		@title:
			option
		@syntax:
			option [<optName:string> <optValue:string>]
		@short:
			Sets an internal option
		@description:
			Sets an internal option named <optName> to the value <optValue>. The value
			must be appropriate for the type of option that you're going to set.[br]
			With no parameters this command lists all the available option names
			sorted by option type.[br] The possible option types are:[br]
			[b]Boolean[/b]: <optValue> must be [b]1[/b] or [b]0[/b][br]
			[b]String[/b]: <optValue> can be any string[br]
			[b]StringList[/b]: <optValue> must be a comma separated list of strings (eventually empty)[br]
			[b]Color[/b]: <optValue> must have the format #RRGGBB where R G and B are hex digits[br]
			[b]Font[/b]: <optValue> is a comma separated list of font properties:
				<family>,<pointsize>,<style>,<charset>,<weight>,<flags>. <family> is the font face name
				([i]arial[/i], [i]helvetica[/i] etc.), <pointsize> is more or less the indicator of the size of the font
				(try sth like 12,14), <style> is an integer that indicates the font style (fixed for example),
				<charset> is an integer that indicates the charset of the font (maybe one day I'll document it better),
				<weight> indicates the (em) weight of the font (50,100,150...matches Bold,DemiBold etc..),
				<flags> is a combination of 'b','i','u','s,' and 'f' that match respectively bold,italic
				underline,strikeout and fixed.[br]
			[b]MessageType[/b]: <optValue> is a comma separated list of message type properties:
				<icon>,<foreground>,<background>,<logBoolean>,<level>: <icon> is the index of the internal
				small icon that has to be shown with the message type, <foreground> is an integer
				indicating the mIRC color to be used for the message type text (0-15),
				<background> is similar to foreground and accepts also the value of 100 that means transparent.
				<logBoolean> is a boolean value (0/1) that indicates whether this message type has to be logged or not.
				<level> is the message level (actually from 0 to 5).[br]
			[b]Rectangle[/b]: <optValue> is a comma separated list of integer values that indicate <x>,<y>,<width> and <height>.[br]
			[b]Pixmap[/b]: <optValue> must be an ABSOLUTE path of the image that you want to load.[br]
			[b]Int[/b]: <optValue> must be an integer.[br]
			[b]Uint[/b]: <optValue> must be an UNSIGNED positive integer.[br]
			Almost all the options available in the option dialog can be set by this command.[br]
			Some GUI options might require a KVIrc restart to work properly (although I've tried to avoid that when possible).
		@examples:
			[example]
				[comment]# List available options[/comment]
				option
				[comment]# Set the mdi manager background image[/comment]
				option pixmapMdiBackground /home/pragma/myback1.png
				[comment]# Set the frame caption text[/comment]
				option stringFrameCaption KVIrc rulez!
				[comment]# Enable verbose mode[/comment]
				option boolBeVerbose 1
				[comment]# Set the IRC view font[/comment]
				option fontIrcView helvetica,24,5,1,50
			[/example]
		@seealso:
			[fnc]$option[/fnc]
	*/

	KVSCSC(option)
	{
		QString szName;
		QString szValue;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("optName", KVS_PT_STRING, KVS_PF_OPTIONAL, szName)
		KVSCSC_PARAMETER("optValue", KVS_PT_STRING, KVS_PF_OPTIONAL | KVS_PF_APPENDREMAINING, szValue)
		KVSCSC_PARAMETERS_END

		if(szName.isEmpty())
		{
			// list available options
			g_pApp->listAvailableOptions(KVSCSC_pWindow);
		}
		else
		{
			if(!g_pApp->setOptionValue(szName, szValue))
				KVSCSC_pContext->warning(__tr2qs_ctx("Option setting error: unknown option or invalid value for option type", "kvs"));
		}

		return true;
	}

	/*
		@doc: parse
		@type:
			command
		@title:
			parse
		@syntax:
			parse [-q] [-e] [-f] [-r] <filename:string> [<parameter1:variant> [<parameter2:variant> [...]]]
		@short:
			Executes commands from a file
		@switches:
			!sw: -e | --fail-on-load
			Causes the command to terminate with an error when the file
			can't be loaded: the error termination stops the parsing
			of the current script too. If the -e switch is not used then
			file load failures are eventually reported only as a warning
			or not reported at all (if -q is used).
			!sw: -f | --fail-on-error
			Causes the command to terminate with an error when the execution
			of the script loaded from disk generates an error.
			This error termination stops the parsing of the current script too.
			If the -f switch is not used then the loaded script errors
			do not stop the current script.
			!sw: -q | --quiet
			Causes the command to run quietly without printing warnings about
			empty <filename> and file loading errors.
			!sw: -r | --propagate-return
			Causes the return value of the script to be propagated to the
			calling context. This allows the usage of ${ } trick to extract
			this return value. See the examples section for a sample usage.
		@description:
			Executes commands from the external file <filename>.[br]
			<filename> can be an absolute or relative path.[br]
			If it is a relative path then KVIrc will try to look it up
			in the path of the calling script (if any) and in the current
			KVIrc working directory.
			The file must be UTF-8-encoded (see below).
			[parameters] is a space separated string of parameters to be
			passed to the script. The parameter $0 will contain the
			path of the file being parsed, the other parameters will
			be available starting from $1.[br]
			If <filename> is an existing directory name a file dialog
			will pop up, allowing you to choose a file to parse.
			The file dialog will start in the directory specified by <filename>.
			If you don't know the name of an existing directory, just pass '.'
			or '/'. The command just prints a warning if the file cannot be loaded for
			some reason. If you want the command to stop with an error in
			this case then use the -e switch.[br]
			This command has a builtin alias called [cmd]include[/cmd].
			[br][br]
			Note that script files, especially the ones that you distribute,
			MUST be encoded in UTF-8. Any other encoding (especially the exotic
			ones like KOI8R or Shift-JIS) may screw things up.
			The reason for such a constraint is simple: KVIrc has no means
			of knowing the encoding that one or other scripter uses to
			edit his files. Unlike other languages (like HTML, for example)
			there is no [i]external[/i] metadata that will allow for
			encoding detection before the data is actually decoded.
			UTF-8 is the right way (tm). Use it.
		@examples:
			[example]
				parse /home/pragma/myscript.kvs
			[/example]
			[example]
				[comment]# Here we are assuming that fetchdata.kvs returns a string[/comment]
				[comment]# We evaluate the return value through ${} and echo it[/comment]
				echo ${ parse -r /home/pragma/fetchdata.kvs; };
			[/example]
	*/

	KVSCSC(parse)
	{
		QString szFileName;
		KviKvsVariantList vList;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("filename", KVS_PT_NONEMPTYSTRING, 0, szFileName)
		KVSCSC_PARAMETER("params", KVS_PT_VARIANTLIST, KVS_PF_OPTIONAL, vList)
		KVSCSC_PARAMETERS_END

		KviFileUtils::adjustFilePath(szFileName);

		if(KviFileUtils::directoryExists(szFileName))
		{
			QString szTmp;
			KVSCSC_pContext->enterBlockingSection();

			bool bResult = KviFileDialog::askForOpenFileName(
			    szTmp,
			    __tr2qs_ctx("Choose a file to parse", "kvs"),
			    szFileName.toUtf8().data(),
			    "*.kvs",
			    false,
			    true,
			    g_pMainWindow);

			if(!KVSCSC_pContext->leaveBlockingSection())
				return false; // need to stop immediately
			if(!bResult)
				return true;

			szFileName = szTmp;

			if(szFileName.isEmpty())
				return true; // done
			KviFileUtils::adjustFilePath(szFileName);
		}
		else
		{
			QFileInfo inf(szFileName);
			if(inf.isRelative())
			{
				if(QString * pszPath = KVSCSC_pContext->scriptFilePath())
				{
					QFileInfo inf2(*pszPath);
					QString szCheckFileName = inf2.path();
					KviQString::ensureLastCharIs(szCheckFileName, KVI_PATH_SEPARATOR_CHAR);
					szCheckFileName += szFileName;
					if(KviFileUtils::fileExists(szCheckFileName))
						szFileName = szCheckFileName;
				}
			}
		}

		QFileInfo infz(szFileName);
		szFileName = infz.absoluteFilePath();

		QString szBuffer;
		if(!KviFileUtils::loadFile(szFileName, szBuffer, true)) // <-- scripts SHOULD be shipped in utf8 format
		{
			if(KVSCSC_pSwitches->find('e', "fail-on-load"))
			{
				KVSCSC_pContext->error(__tr2qs_ctx("Failed to load the file '%Q' for parsing", "kvs"), &szFileName);
				return false;
			}
			else
			{
				if(!KVSCSC_pSwitches->find('q', "quiet"))
					KVSCSC_pContext->warning(__tr2qs_ctx("Failed to load the file '%Q' for parsing", "kvs"), &szFileName);
				return true;
			}
		}

		KviKvsScript s(szFileName, szBuffer);

		KviKvsVariant * pRetVal = KVSCSC_pSwitches->find('r', "propagate-return") ? KVSCSC_pContext->returnValue() : nullptr;
		KviKvsVariant vFileName(szFileName);
		vList.prepend(&vFileName);

		KviKvsExtendedRunTimeData rtd(&szFileName);

		if(!s.run(KVSCSC_pContext->window(), &vList, pRetVal, KviKvsScript::PreserveParams, &rtd))
		{
			if(KVSCSC_pSwitches->find('f', "fail-on-error"))
				return false;
		}
		return true;
	}

	/*
		@doc: part
		@type:
			command
		@title:
			part
		@syntax:
			part [-k] [-q] [-s] [<channel list> [part message]]
		@short:
			Leaves the specified channels
		@switches:
			!sw: -k | --keep
			Keeps the channel windows open after the part.
			!sw: -q | --quiet
			Runs the command in quiet mode (no warnings printed)
			!sw: -s | --silent
			Does not show the part message in the channel window
		@description:
			Leaves the channels specified in the <channel list>.
			If no channels are specified, the current channel is used.
			This command is "server based"; you will see its results only
			after the server acknowledges it.[br]
			If the -k switch is specified then the channel windows are left open
			after the part (the channels become dead channels).[br]
			If the -s switch is specified then the part message is not shown
			in the channel window.[br]
			This command is [doc:connection_dependent_commands]connection dependent[/doc].[br]
			This command is equivalent to [cmd]leave[/cmd]
		@examples:
			[example]
				part #kvirc Byez!
				part #linux-it,#xmms,#kde Time to sleep
			[/example]
	*/

	KVSCSC(part)
	{
		QString szChans, szMsg;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("chans", KVS_PT_STRING, KVS_PF_OPTIONAL, szChans)
		KVSCSC_PARAMETER("part_message", KVS_PT_STRING, KVS_PF_OPTIONAL | KVS_PF_APPENDREMAINING, szMsg)
		KVSCSC_PARAMETERS_END

		KVSCSC_REQUIRE_CONNECTION

		if(szChans.isEmpty())
		{
			if(KVSCSC_pWindow->type() == KviWindow::Channel)
				szChans = KVSCSC_pWindow->target();
			else
			{
				if(!KVSCSC_pSwitches->find('q', "quiet"))
					KVSCSC_pContext->warning(__tr2qs_ctx("Missing channel list", "kvs"));
				return true;
			}
		}

		QByteArray szEncodedChans = KVSCSC_pConnection->encodeText(szChans);

		QStringList sl = szChans.split(",", QString::SkipEmptyParts);

		if(szMsg.isEmpty())
			szMsg = KVI_OPTION_STRING(KviOption_stringPartMessage);

		QByteArray szText;
		if(sl.count() == 1)
		{
			// single chan, use channel encoding if possible
			KviChannelWindow * ch = KVSCSC_pConnection->findChannel(szChans);
			if(ch)
				szText = ch->encodeText(szMsg);
			else
				szText = KVSCSC_pConnection->encodeText(szMsg);
		}
		else
		{
			// multiple chans, use connection encoding
			szText = KVSCSC_pConnection->encodeText(szMsg);
		}
		if(!(KVSCSC_pConnection->sendFmtData("PART %s :%s", szEncodedChans.data(), szText.data())))
			return KVSCSC_pContext->warningNoIrcConnection();

		for(auto & it : sl)
		{
			KviChannelWindow * ch = KVSCSC_pConnection->findChannel(it);
			if(ch)
			{
				bool bNoCloseOnPart = KVI_OPTION_BOOL(KviOption_boolKeepChannelOpenOnPart) || KVSCSC_pSwitches->find('k', "keep");
				ch->partMessageSent(!bNoCloseOnPart, !KVSCSC_pSwitches->find('s', "silent"));
			}
			else
			{
				if(!KVSCSC_pSwitches->find('q', "quiet"))
					KVSCSC_pContext->warning(__tr2qs_ctx("You don't appear to be on channel %s", "kvs"), it.toUtf8().data());
			}
		}

		return true;
	}

	/*
		@doc: pass
		@type:
			command
		@title:
			pass
		@syntax:
			pass <password>
		@short:
			Sends a password to the server
		@description:
			Sends a password to the server: usually KVIrc takes care of it, but well.. this is useful with bouncers.[br]
			This command is an [doc:rfc2812wrappers]RFC2812 command wrapper[/doc]; see that document for more information.[br]
	*/
	// RFC2812 wrapper

	/*
		@doc: play
		@type:
			command
		@title:
			play
		@syntax:
			play <filename:string>
		@short:
			DEPRECATED: Use $snd.play instead!
		@description:
			This command has been moved to the snd module.
			See [cmd]snd.play[/cmd]
	*/

	KVSCSC(play)
	{
		KviModule * m = g_pModuleManager->getModule("snd");
		if(!m)
		{
			KVSCSC_pContext->error(__tr2qs_ctx("Module command call failed: can't load the module 'snd'", "kvs"));
			return false;
		}

		KviKvsModuleSimpleCommandExecRoutine * proc = m->kvsFindSimpleCommand("play");
		if(!proc)
		{
			KVSCSC_pContext->error(__tr2qs_ctx("Module command call failed: the module 'snd' doesn't export a command named 'play'", "kvs"));
			return false;
		}

		KviKvsModuleCommandCall call(m, KVSCSC_pContext, KVSCSC_pParams, KVSCSC_pSwitches);

		return (*proc)(&call);
	}

	/*
		@doc: popup
		@type:
			command
		@title:
			popup
		@syntax:
			popup [-p=<screen_coordinates:string>] <popup_name:string> [<parameter1:variant> [<parameter2:variant> [...]]]
		@short:
			Shows a popup menu
		@switches:
			!sw: -p=<screen_coordinates:string> | --point=<screen_coordinates:string>
		@description:
			This is an internal alias for [cmd]popup.show[/cmd].[br]
			This function is deprecated and its use is discouraged.
		@seealso:
			[cmd]popup.show[/cmd]
	*/

	KVSCSC(popup)
	{
		KviKvsVariantList lParameters;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("params", KVS_PT_VARIANTLIST, KVS_PF_OPTIONAL | KVS_PF_APPENDREMAINING, lParameters)
		KVSCSC_PARAMETERS_END

		//c->warning(__tr2qs("/popup is deprecated: use /popup.show instead")); <-- do it sometime in the future

		// We just alias the popup.show function
		QString szSwitches = "";
		KviKvsVariant * pCoords = KVSCSC_pSwitches->find('p', "point");
		if(pCoords)
		{
			QString szCoords;
			pCoords->asString(szCoords);
			szSwitches.append(QString("-p=\"%1\" ").arg(szCoords));
		}

		QString szParams;
		for(unsigned int i = 0; i < lParameters.count(); i++)
			szParams.append(QString::fromLatin1(" $%1 ").arg(i));

		KviKvsScript s("popup", "popup.show " + szSwitches + szParams);
		s.run(KVSCSC_pContext->window(), &lParameters, nullptr, KviKvsScript::PreserveParams);
		return true;
	}

	/*
		@doc: privmsg
		@type:
			command
		@title:
			privmsg
		@syntax:
			privmsg <target:string> <text:string>
		@short:
			Sends a private message
		@switches:
			!sw: -q | --quiet
			Do not print any output
		@description:
			Sends a private message to the specified <target>.
			<target> may be any PRIVMSG target allowed by the underlying
			IRC protocol (see RFC1459).
			This command is really similar to [cmd]msg[/cmd] but it does not
			output the message locally.[br]
			This command is [doc:connection_dependent_commands]connection dependent[/doc].[br]
			This command is equivalent to [cmd]msg[/cmd]
		@examples:
			[example]
				privmsg Pragma Hello!
				privmsg Pragma,Crocodile Hello to you both!
				privmsg #kvirc Hello from outside!
			[/example]
	*/

	KVSCSC(privmsg)
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

			if(!(KVSCSC_pConnection->sendFmtData("PRIVMSG %s :%s", szT.data(), szD.data())))
				return KVSCSC_pContext->warningNoIrcConnection();

			if(!KVSCSC_pSwitches->find('q', "quiet"))
				KVSCSC_pWindow->output(KVI_OUT_OWNPRIVMSG, "[PRIVMSG >>> \r!nc\r%Q\r]: %Q", &szTarget, &szText);
		}

		return true;
	}

	/*
		@doc: query
		@type:
			command
		@title:
			query
		@syntax:
			query [-m] <nickname list> [text]
		@short:
			Opens one or more query windows
		@switches:
			!sw: -m | --minimized
			Create minimized queries
			!sw: -i | --incoming
			Assume that the query is "incoming".
			The initial visibility of the window is specified by global settings.
		@description:
			Opens a query window for each user specified in <nickname list>
			which is a [b]comma separated[/b] list of nicknames.[br]
			If [text] is specified, it is sent to the
			query window just as it would have been written in the query itself.
			If a query with one of the specified targets already exists,
			it is simply focused and the [text] is sent to the target.[br]
			This command is [doc:connection_dependent_commands]connection dependent[/doc].[br]
			Normally the new query windows opened with this command are created and immediately shown.
			If the -i switch is specified then the windows are created either as minimized or as visible
			depending on the global visibility setting specified by the boolCreateIncomingQueriesAsMinimized option.
			This switch should be used when automatically opening a query from a script without user
			intervention (as happens when the query is opened due to an incoming private message).
			If -i is not present but -m is used then the windows are always created as minimized.
		@examples:
			[example]
				[comment]# Open a single query to Pragma[/comment]
				query Pragma
				[comment]# Open a query to Pragma and to Crocodile, say "Hello!" in both windows[/comment]
				query Pragma,Crocodile Hello !
			[/example]
	*/

	KVSCSC(query)
	{
		QString szTargets, szText;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("targets", KVS_PT_NONEMPTYSTRING, 0, szTargets)
		KVSCSC_PARAMETER("text", KVS_PT_STRING, KVS_PF_OPTIONAL | KVS_PF_APPENDREMAINING, szText)
		KVSCSC_PARAMETERS_END

		KVSCSC_REQUIRE_CONNECTION

		KviQueryWindow * query;

		QStringList sl = szTargets.split(",", QString::SkipEmptyParts);
		for(auto szNick : sl)
		{
			if(szNick.isEmpty())
				KVSCSC_pContext->warning(__tr2qs_ctx("Empty target specified", "kvs"));
			else
			{
				query = KVSCSC_pWindow->connection()->findQuery(szNick);
				if(query)
				{
					if(!KVSCSC_pSwitches->find('m', "minimized"))
						g_pMainWindow->setActiveWindow(query);
				}
				else
				{
					query = KVSCSC_pWindow->connection()->createQuery(
					    szNick,
					    KVSCSC_pSwitches->find('i', "incoming") ? KviIrcConnection::CreateQueryVisibilityFollowSettings : (KVSCSC_pSwitches->find('m', "minimized") ? KviIrcConnection::CreateQueryVisibilityMinimized : KviIrcConnection::CreateQueryVisibilityVisible));
					QString user;
					QString host;
					KviIrcUserDataBase * db = KVSCSC_pWindow->connection()->userDataBase();
					if(db)
					{
						KviIrcUserEntry * e = db->find(szNick);
						if(e)
						{
							user = e->user();
							host = e->host();
						}
					}
					query->setTarget(szNick, user, host);
				}
				if(!szText.isEmpty())
					query->ownMessage(szText);
			}
		}

		return true;
	}

	/*
		@doc: quit
		@type:
			command
		@title:
			quit
		@syntax:
			quit [-f] [-u] [quit-message:string]
		quit -q
		@short:
			Terminates the current IRC connection or the entire application
		@switches:
			!sw: -q | --quit
			Terminates this KVIrc application instance
			!sw: -f | --force
			Forces the immediate termination of the current IRC session (The QUIT message may be not sent)
			!sw: -u | --unexpected
			Simulates an "unexpected disconnection"
		@description:
			Terminates the current IRC session.[br]
			By default this command sends a QUIT message
			and waits for the server to close the connection.[br]
			If you want to force KVIrc to close the connection
			immediately after sending the QUIT message you must use the -f switch.[br]
			Forcing the connection may cause your quit message to not be
			displayed to the other IRC users: most likely it will be replaced
			by a 'Connection reset by peer' or a 'EOF from client'.[br]
			If the -u switch is specified then an [i]unexpected disconnection[/i] will be simulated
			and all the related options will be applied (e.g. automatic reconnection,
			channels and queries kept open etc.). The QUIT message will be still sent unless
			the -f switch is used. You can control all the [i]unexpected disconnection[/i]
			options in the options dialog.
			If the -q switch is specified, this command terminates KVIrc immediately.[br]
		@examples:
			[example]
				quit Time to sleep
			[/example]
	*/

	KVSCSC(quit)
	{
		QString szReason;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("reason", KVS_PT_STRING, KVS_PF_OPTIONAL | KVS_PF_APPENDREMAINING, szReason)
		KVSCSC_PARAMETERS_END

		if(KVSCSC_pSwitches->find('q', "quit"))
		{
			QTimer::singleShot(0, g_pApp, SLOT(quit()));
		}
		else
		{
			KVSCSC_REQUIRE_CONNECTION
			KVSCSC_pWindow->context()->terminateConnectionRequest(KVSCSC_pSwitches->find('f', "force"), szReason, KVSCSC_pSwitches->find('u', "unexpected"));
		}
		return true;
	}

	/*
		@doc: quote
		@type:
			command
		@title:
			quote
		@syntax:
			quote [-q] <raw command:string>
		@short:
			Builtin alias for RAW
		@switches:
			!sw: -q | --quiet
			Be quiet: do echo the raw data.
		@description:
			Builtin alias for the command [cmd]raw[/cmd].
	*/

	/*
		@doc: raise
		@type:
			command
		@title:
			raise
		@syntax:
			raise
		@short:
			Raises the KVIrc frame window
		@description:
			Raises and activates the KVIrc frame window, assuming that your window manager supports it.[br]
	*/

	KVSCSC(raise)
	{
		if(!g_pMainWindow->isVisible())
			g_pMainWindow->show();
		g_pMainWindow->raise();
		//g_pMainWindow->setActiveWindow();
		//g_pMainWindow->setFocus();
		g_pMainWindow->activateWindow();
		return true;
	}

	/*
		@doc: raw
		@type:
			command
		@title:
			raw
		@syntax:
			raw [-q] <raw command:string>
		@short:
			Sends RAW data to the server
		@switches:
			!sw: -q | --quiet
			Be quiet: do echo the RAW data.
		@description:
			Sends a RAW data string to the server of the current [b]IRC context[/b].[br]
			If the -q switch (quiet) is specified, no output is printed.
			This command is [doc:connection_dependent_commands]connection dependent[/doc].[br]
			This command is equivalent to [cmd]quote[/cmd].
		@examples:
			[example]
				[comment]# Send a private message "by hand"[/comment]
				raw PRIVMSG Pragma :hello!
				[comment]# Send a private message through another connection[/comment]
				raw -r=[fnc]$console[/fnc]([fnc]$ic[/fnc](irc.otherserver.com,othernick)) PRIVMSG Pragma :heya on this side!
			[/example]
	*/

	KVSCSC(raw)
	{
		QString szRawCommand;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("raw_command", KVS_PT_STRING, KVS_PF_APPENDREMAINING, szRawCommand)
		KVSCSC_PARAMETERS_END

		KVSCSC_REQUIRE_CONNECTION

		QByteArray szData = KVSCSC_pConnection->encodeText(szRawCommand);

		if(!KVSCSC_pConnection->sendData(szData.data()))
			return KVSCSC_pContext->warningNoIrcConnection();

		if(!KVSCSC_pSwitches->find('q', "quiet"))
			KVSCSC_pWindow->output(KVI_OUT_RAW, __tr2qs_ctx("[RAW]: %Q", "kvs"), &szRawCommand);

		return true;
	}

	/*
		@doc: rebind
		@type:
			command
		@title:
			rebind
		@syntax:
			rebind [-q] <window_id:string>
		@short:
			Rebinds a command sequence to a specified window
		@switches:
			!sw: -q | --quiet
			Causes the command to run quietly, print no warnings
			and don't fail when the window identifier passed is empty.
		@description:
			Rebinds the current command sequence to the window specified by <window_id>.[br]
			The new window will be inherited by all the subcommands and aliases called.[br]
			-q causes the command to run quietly.[br]
			[b]Handle with care.[/b]
		@examples:
			[example]
				[comment]# Try this example in a channel or query window[/comment][br]
				[comment]# Remember the current window ID[/comment][br]
				%winid = $window[br]
				[comment]# Rebind to the console of the current IRC context[/comment][br]
				rebind $console[br]
				echo "Hello from the console :)"[br]
				echo "Hello again.. still in the console"[br]
				[comment]# Rebind back[/comment][br]
				rebind %winid[br]
				echo "Again in this window :)"
			[/example]
			[note]
				[big]Deprecated switches:[/big]
				From version 3.0.0, the standard -r switch support to commands was removed.
			[/note]
	*/

	KVSCSC(rebind)
	{
		QString szWinId;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("window_id", KVS_PT_NONEMPTYSTRING, 0, szWinId)
		KVSCSC_PARAMETERS_END

		KviWindow * pAux = g_pApp->findWindow(szWinId.toUtf8().data());
		if(pAux)
			KVSCSC_pContext->setWindow(pAux);
		else
		{
			if(!KVSCSC_pSwitches->find('q', "quiet"))
				KVSCSC_pContext->warning(__tr2qs_ctx("Window with ID %Q not found: no rebinding performed", "kvs"), &szWinId);
		}
		return true;
	}

	/*
		@doc: return
		@type:
			command
		@title:
			return
		@syntax:
			return <value:variant>
		@short:
			Returns from the current command sequence and sets it return value
		@description:
			Sets the return value of the current command sequence to <string>
			and stops the execution.[br]
			This is more or less equivalent to calling [cmd]setreturn[/cmd] <string>
			and then [cmd]halt[/cmd], but has no additional semantics in events.[br]
			Starting from version 3.0.0 of KVIrc you can also return
			arrays and hashes just like any other variable types.
		@examples:
			[example]
				return $array(item1,item2,3213,itemX);
			[/example]
		@seealso:
			[cmd]setreturn[/cmd], [cmd]break[/cmd], [cmd]halt[/cmd]
	*/

	KVSCSC(returnCKEYWORDWORKAROUND)
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
		return false;
	}

	/*
		@doc: run
		@type:
			command
		@title:
			run
		@syntax:
			run <commandline:string>
		@short:
			Runs an external process
		@description:
			Runs an external process. This command does not allow
			any other interaction with the process started. If you want
			to grab the process output you probably need to use [cmd]exec[/cmd].
			Run doesn't spawn a subshell in order to execute the <commandline>
			and thus if you need shell substitutions to be made you must invoke
			the shell by yourself (probably by passing the real command as the -c argument).
		@examples:
			[example]
				run xterm
				run xmessage -center *
				[comment]# note the difference in * processing[/comment]
				run /bin/bash -c "xmessage -center *"
			[/example]
	*/

	KVSCSC(run)
	{
		QString szCommand;
		QStringList l;
		KVSCSC_PARAMETERS_BEGIN
		KVSCSC_PARAMETER("command", KVS_PT_STRING, 0, szCommand)
		KVSCSC_PARAMETER("parameters", KVS_PT_STRINGLIST, KVS_PF_OPTIONAL, l)
		KVSCSC_PARAMETERS_END

		QProcess proc;
		proc.startDetached(szCommand, l);
		// FIXME: KVSCSC_pContext->warning(__tr2qs_ctx("Failed to execute command '%Q'","kvs"),&szCommand);
		return true;
	}
};
