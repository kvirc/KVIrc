//=============================================================================
//
//   File : KviKvsCoreFunctions_af.cpp
//   Creation date : Fri 31 Oct 2003 01:52:04 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek <pragma at kvirc dot net>
//   Copyright ©        2010 Kai Wasserbäch <debian@carbon-project.org>
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

#include "KviKvsCoreFunctions.h"
#include "KviKvsKernel.h"
#include "KviKvsObjectController.h"
#include "KviLocale.h"
#include "KviApplication.h"
#include "KviChannelWindow.h"
#include "KviConsoleWindow.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviControlCodes.h"
#include "KviAvatar.h"
#include "KviIrcUserDataBase.h"
#include "KviMainWindow.h"
#include "KviStatusBar.h"
#include "KviQString.h"
#include "KviSSLMaster.h"
#include "KviOptions.h"
#include "KviKvsAliasManager.h"
#include "KviKvsScript.h"
#include "KviBuildInfo.h"

#include <QChar>
#include <QDateTime>
#include <QString>
#include <QStringList>

namespace KviKvsCoreFunctions
{
	/*
		@doc: active
		@type:
			function
		@title:
			$active
		@short:
			Retrieves the window ID of the active window
		@syntax:
			<integer> $active[(<irc context id:integer>)]
		@description:
			Returns the [b]window ID[/b] of the active window
			bound to the IRC context specified by <irc context id>.
			If no window matches the specified IRC context, an invalid
			window ID is returned (0).[br]
			If no <irc context id> is specified, then
			the application active window is returned (the window
			that currently has the input focus).[br][br]Note that in this
			case the returned window may also belong to another IRC
			context or be not bound to any IRC context at all.
			In some extreme cases you may even get a window that
			has no output widget and thus has its output redirected.
			Using the [i]global[/i] active window should be used only
			for communicating something [b]really[/b] urgent (and maybe
			unrelated to a specific IRC connection) to the user.
		@seealso:
			[fnc]$window[/fnc]
	*/

	KVSCF(active)
	{
		kvs_uint_t uContextId;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("context_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uContextId)
		KVSCF_PARAMETERS_END

		KviWindow * wnd = nullptr;
		if(KVSCF_pParams->count() > 0)
		{
			KviConsoleWindow * cons = g_pApp->findConsole(uContextId);
			if(cons)
				wnd = cons->activeWindow();
		}
		else
		{
			wnd = g_pActiveWindow;
		}

		KVSCF_pRetBuffer->setInteger((kvs_int_t)(wnd ? wnd->numericId() : 0));
		return true;
	}

	/*
		@doc: aliasBody
		@type:
			function
		@title:
			$aliasBody
		@short:
			Returns the body of an alias, if it exists
		@syntax:
			<string> $aliasBody(<alias_name:string>)
		@description:
			Returns the KVS body of the specified alias.
			If the alias doesn't exist it just returns an empty string.
		@seealso:
			[cmd]alias[/cmd]
	*/

	KVSCF(aliasBody)
	{
		QString szAliasName;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("alias_name", KVS_PT_STRING, 0, szAliasName)
		KVSCF_PARAMETERS_END

		const KviKvsScript * pAlias = KviKvsAliasManager::instance()->lookup(szAliasName);
		if(pAlias)
			KVSCF_pRetBuffer->setString(pAlias->code());
		else
			KVSCF_pRetBuffer->setString(QString());
		return true;
	}

	/*
		@doc: ascii
		@type:
			function
		@title:
			$ascii
		@short:
			Returns the Unicode code of a character
		@syntax:
			<variant> $ascii(<char:string>)
		@description:
			This function has been renamed to $unicode and is present
			only for backward compatibility.
		@seealso:
			[fnc]$cr[/fnc], [fnc]$lf[/fnc], [fnc]$char[/fnc]
	*/

	/*
		@doc: asciiToBase64
		@type:
			function
		@title:
			$asciiToBase64
		@short:
			Returns an encoded base64 string
		@syntax:
			$asciiToBase64(<ascii_string>)
		@description:
			Encodes an ASCII string to its base64 encoded representation
			Please note that since KVS is Unicode based, this function
			will first encode the string in UTF-8 and then base64-encode.
			This means that it is substantially only 7-bit safe (ASCII codes below 128).
		@examples:
			[example]
				[cmd]echo[/cmd] $asciiToBase64("Hello!")
			[/example]
		@seealso:
			[fnc]$base64toascii[/fnc]
	*/

	KVSCF(asciiToBase64)
	{
		QString szAscii;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("ascii_string", KVS_PT_STRING, 0, szAscii)
		KVSCF_PARAMETERS_END

		KviCString tmp1(szAscii);
		if(tmp1.len() > 0)
		{
			KviCString tmp2;
			tmp2.bufferToBase64(tmp1.ptr(), tmp1.len());
			KVSCF_pRetBuffer->setString(QString(tmp2.ptr()));
		}
		else
		{
			KVSCF_pRetBuffer->setString(QString());
		}
		return true;
	}

	/*
		@doc: asciiToHex
		@type:
			function
		@title:
			$asciiToHex
		@short:
			Returns an encoded hex string
		@syntax:
			<string> $asciiToHex(<ascii_string:string>)
		@description:
			Encodes an ASCII string to its hex encoded representation.
			Please note that since KVS is Unicode based, this function
			will first encode the string in UTF-8 and then hex-encode.
			This means that it is substantially only 7bit safe (ASCII codes below 128).
		@examples:
			[example]
				[cmd]echo[/cmd] $asciiToHex("Hello!")
			[/example]
		@seealso:
			[fnc]$hextoascii[/fnc]
	*/

	KVSCF(asciiToHex)
	{
		QString szAscii;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("ascii_string", KVS_PT_STRING, 0, szAscii)
		KVSCF_PARAMETERS_END

		KviCString tmp1(szAscii);
		if(tmp1.len() > 0)
		{
			KviCString tmp2;
			tmp2.bufferToHex(tmp1.ptr(), tmp1.len());
			KVSCF_pRetBuffer->setString(QString(tmp2.ptr()));
		}
		else
		{
			KVSCF_pRetBuffer->setString(QString());
		}
		return true;
	}

	/*
		@doc: array
		@type:
			function
		@title:
			$array
		@short:
			Explicitly creates an array
		@syntax:
			<array> $array(<item:variant>,<item:variant>,<item:variant>,....);
		@description:
			Returns an array with the specified items. The items are indexed starting from 0.
			This is just an explicit way of creating an array with a defined set of items,
			useful for increasing readability.
		@examples:
			[example]
				[cmd]alias[/cmd](test) {
					[cmd]return[/cmd] $array(1,2,3);
				}
				%x = $test();
				[cmd]foreach[/cmd](%y,%x) {
					[cmd]echo[/cmd] %y;
				}
			[/example]
		@seealso:
			[fnc]$hash[/fnc]
	*/

	KVSCF(array)
	{
		KviKvsArray * a = new KviKvsArray();

		for(KviKvsVariant * v = KVSCF_pParams->first(); v; v = KVSCF_pParams->next())
			a->append(new KviKvsVariant(*v));

		KVSCF_pRetBuffer->setArray(a);
		return true;
	}

	/*
		@doc: away
		@type:
			function
		@title:
			$away
		@short:
			Returns true if the current user is away
		@syntax:
			<boolean> $away
		@description:
			Returns true if the current user is away, else false.
			If the current IRC context is not connected at all, this function returns false.
	*/

	KVSCF(away)
	{
		kvs_uint_t uCntx;

		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("irc_context_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uCntx)
		KVSCF_PARAMETERS_END

		KviConsoleWindow * cns = nullptr;

		if(KVSCF_pParams->count() > 0)
		{
			cns = g_pApp->findConsole(uCntx);
			if(cns && cns->context()->isConnected())
				KVSCF_pRetBuffer->setBoolean(cns->connection()->userInfo()->isAway());
			else
				KVSCF_pRetBuffer->setNothing();
		}
		else
		{
			cns = KVSCF_pContext->window()->console();
			if(cns)
			{
				if(cns->context()->isConnected())
					KVSCF_pRetBuffer->setBoolean(cns->connection()->userInfo()->isAway());
				else
					KVSCF_pRetBuffer->setNothing();
			}
			else
			{
				KVSCF_pContext->warning(__tr2qs_ctx("This window has no associated IRC context", "kvs"));
				KVSCF_pRetBuffer->setNothing();
			}
		}
		return true;
	}

	/*
		@doc: b
		@type:
			function
		@title:
			$b
		@short:
			Returns the BOLD mIRC control character
		@syntax:
			<string> $b
		@description:
			Returns the BOLD mIRC control character (Ctrl+B).[br]
		@seealso:
			[fnc]$k[/fnc], [fnc]$i[/fnc], [fnc]$u[/fnc], [fnc]$r[/fnc], [fnc]$o[/fnc]
	*/

	KVSCF(b)
	{
		KVSCF_pRetBuffer->setString(QString(QChar(KviControlCodes::Bold)));
		return true;
	}

	/*
		@doc: base64toAscii
		@type:
			function
		@title:
			$base64ToAscii
		@short:
			Returns a decoded base64 string
		@syntax:
			<string> $base64ToAscii(<base_64_encoded_string:string>)
		@description:
			Decodes a base64 encoded string.
		@seealso:
			[fnc]$asciiToBase64[/fnc]
	*/

	KVSCF(base64ToAscii)
	{
		QString szBase64;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("base64_encoded_string", KVS_PT_STRING, 0, szBase64)
		KVSCF_PARAMETERS_END

		KviCString tmp1(szBase64);
		char * buf;
		int len = tmp1.base64ToBuffer(&buf, true);
		QString szRet(buf);
		szRet.truncate(len);
		KVSCF_pRetBuffer->setString(szRet);
		KviCString::freeBuffer(buf);
		return true;
	}

	/*
		@doc: boolean
		@type:
			function
		@title:
			$boolean
		@short:
			Casts a variable to a boolean
		@syntax:
			<integer> $boolean(<data:variant>)
		@description:
			Forces <data> to be a boolean data type by first casting
			to integer (see [fnc]$int[/fnc]()) and then comparing the result against zero.
			A zero integer will result in a false value while a non-zero one
			will result in a true value.
			This function is similar to the C++ (bool) cast and is internally
			aliased to [fnc]$bool[/fnc] too.
			Note that since KVIrc does most of the casting work automatically
			you shouldn't need to use this function.
		@seealso:
			[fnc]$real[/fnc]
			[fnc]$integer[/fnc]
	*/

	/*
		@doc: bool
		@type:
			function
		@title:
			$bool
		@short:
			Casts a variable to a boolean
		@syntax:
			<integer> $bool(<data:variant>)
		@description:
			This is an internal alias to [fnc]$boolean[/fnc]().
		@seealso:
			[fnc]$real[/fnc]
			[fnc]$integer[/fnc]
	*/

	KVSCF(boolean)
	{
		KviKvsVariant * v;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("data", KVS_PT_VARIANT, 0, v)
		KVSCF_PARAMETERS_END

		kvs_int_t iVal;
		v->castToInteger(iVal);
		KVSCF_pRetBuffer->setBoolean(iVal);
		return true;
	}

	/*
		@doc: certificate
		@type:
			function
		@title:
			$certificate
		@short:
			Returns information about the local certificate
		@syntax:
			<string> $certificate(<query:string>[,<param1:string>])
		@description:
			Returns the requested information about local certificate.[br]
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
		@seealso:
			[fnc]$str.evpSign[/fnc]
			[fnc]$str.evpVerify[/fnc]
			[fnc]$dcc.getSSLCertInfo[/fnc]
	*/

	KVSCF(certificate)
	{
		QString szQuery;
		QString szParam1;

		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("query", KVS_PT_STRING, 0, szQuery)
		KVSCF_PARAMETER("param1", KVS_PT_STRING, KVS_PF_OPTIONAL, szParam1)
		KVSCF_PARAMETERS_END

#ifndef COMPILE_SSL_SUPPORT
		KVSCF_pContext->warning(__tr2qs("This executable was built without SSL support"));
		return true;
#else
		KviSSL::globalSSLInit();
		X509 * cert = nullptr;

		if(!KVI_OPTION_BOOL(KviOption_boolUseSSLCertificate))
		{
			KVSCF_pContext->warning(__tr2qs("No public key certificate defined in KVIrc options."));
			KVSCF_pRetBuffer->setString("");
			return true;
		}

		FILE * f = fopen(KVI_OPTION_STRING(KviOption_stringSSLCertificatePath).toUtf8().data(), "r");
		if(!f)
		{
			KVSCF_pContext->warning(__tr2qs("File I/O error while trying to use the public key file %s"), KVI_OPTION_STRING(KviOption_stringSSLCertificatePath).toUtf8().data());
			KVSCF_pRetBuffer->setString("");
			return true;
		}

		QString szPass = KVI_OPTION_STRING(KviOption_stringSSLCertificatePass).toUtf8();
		PEM_read_X509(f, &cert, nullptr, szPass.data());

		fclose(f);

		if(!cert)
		{
			KVSCF_pContext->warning(__tr2qs("Can't load certificate from public key file %s"), KVI_OPTION_STRING(KviOption_stringSSLCertificatePath).toUtf8().data());
			KVSCF_pRetBuffer->setString("");
			return true;
		}

		// KviSSLCertificate takes ownership of the X509 structure, no need to free it
		KviSSLCertificate * pCert = new KviSSLCertificate(cert);

		if(!pCert)
		{
			KVSCF_pContext->warning(__tr2qs_ctx("Error retrieving information from the local certificate", "dcc"));
			KVSCF_pRetBuffer->setString("");
			return true;
		}

		if(KviSSLMaster::getSSLCertInfo(pCert, szQuery, szParam1, KVSCF_pRetBuffer))
			return true;

		KVSCF_pContext->warning(__tr2qs_ctx("Unable to get certificate information: query not recognized", "dcc"));
		KVSCF_pRetBuffer->setString("");
		return true;
#endif
	}

	/*
		@doc: channel
		@type:
			function
		@title:
			$channel
		@short:
			Retrieves the window ID of a specified channel
		@syntax:
			$channel[(<channel name>[,<irc context id>])]
		@description:
			Returns the [b]window ID[/b] of channel matching the
			<channel name> and bound to the connection specified by
			<irc context id>[br]
			If no window matches the specified name or connection, an invalid
			window ID is returned [b]0[/b].[br]
			If no <irc context id> is specified, this function looks for
			the channel in the current connection context (if any).[br]
			If no <channel name> is specified, this function returns the current
			channel window ID, if executed in a channel, and [b]0[/b] otherwise.[br]
		@seealso:
			[fnc]$window[/fnc],
			[fnc]$query[/fnc],
			[fnc]$console[/fnc],
			[doc:window_naming_conventions]Window naming conventions[/doc]
	*/

	KVSCF(channel)
	{
		QString szName;
		kvs_uint_t uContextId;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("channel_name", KVS_PT_NONEMPTYSTRING, KVS_PF_OPTIONAL, szName)
		KVSCF_PARAMETER("context_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uContextId)
		KVSCF_PARAMETERS_END

		KviWindow * wnd = nullptr;
		if(KVSCF_pParams->count() > 1)
		{
			KviConsoleWindow * cns = g_pApp->findConsole(uContextId);
			if(cns && cns->connection())
				wnd = cns->connection()->findChannel(szName);
			else if (!cns)
				KVSCF_pContext->warning(__tr2qs_ctx("No such IRC context (%u)", "kvs"), uContextId);
		}
		else if(KVSCF_pParams->count() == 1)
		{
			if(KVSCF_pContext->window()->connection())
				wnd = KVSCF_pContext->window()->connection()->findChannel(szName);
			else
			{
				if(!KVSCF_pContext->window()->console())
					KVSCF_pContext->warning(__tr2qs_ctx("This window is not associated to an IRC context", "kvs"));
			}
		}
		else
		{
			if(KVSCF_pContext->window()->type() == KviWindow::Channel)
				wnd = KVSCF_pContext->window();
		}

		KVSCF_pRetBuffer->setInteger((kvs_int_t)(wnd ? wnd->numericId() : 0));
		return true;
	}

	/*
		@doc: char
		@type:
			function
		@title:
			$char
		@short:
			Returns a character specified by Unicode
		@syntax:
			<string> $char(<unicode_value:integer>)
		@description:
			Returns a character corresponding to the Unicode code <unicode_value>.[br]
			This function can not return NUL character (Unicode 0). Basically
			you should never need it: if you do, drop me a mail.[br]
			If the <unicode_code> is not a valid Unicode code (or is 0), this function returns
			an empty string.[br]
		@seealso:
			[fnc]$cr[/fnc], [fnc]$lf[/fnc], [fnc]$unicode[/fnc]
	*/

	KVSCF(charCKEYWORDWORKAROUND)
	{
		kvs_uint_t ac;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("unicode_value", KVS_PT_UINT, 0, ac)
		KVSCF_PARAMETERS_END

		if(ac != 0 && ac < 65536)
			KVSCF_pRetBuffer->setString(QString(QChar((unsigned short)ac)));
		else
			KVSCF_pRetBuffer->setString(QString());
		return true;
	}

	/*
		@doc: classdefined
		@type:
			function
		@title:
			$classdefined
		@short:
			Checks if a class is defined
		@syntax:
			$classdefined(<class_name>)
		@description:
			Returns [b]1[/b] if the class <class_name> is defined, and [b]0[/b] otherwise.
	*/

	KVSCF(classDefined)
	{
		// prologue: parameter handling
		QString szClassName;

		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("className", KVS_PT_NONEMPTYSTRING, 0, szClassName)
		KVSCF_PARAMETERS_END
		KVSCF_pRetBuffer->setBoolean(KviKvsKernel::instance()->objectController()->lookupClass(szClassName) != nullptr);
		return true;
	}

	/*
		@doc: console
		@type:
			function
		@title:
			$console
		@short:
			Retrieves the window ID of a specified console
		@syntax:
			$console[(<irc context id>)]
		@description:
			Returns the [b]window ID[/b] of the console bound
			to the IRC context specified by <irc context id>.
			If no window matches the specified IRC context, an invalid
			window ID is returned (0).[br]
			If no <irc context id> is specified, this function looks for
			the console in the current IRC context (if any).[br]
		@seealso:
			[fnc]$window[/fnc],
			[fnc]$channel[/fnc],
			[fnc]$query[/fnc],
			[doc:window_naming_conventions]Window naming conventions[/doc]
	*/

	KVSCF(console)
	{
		kvs_uint_t uContextId;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("context_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uContextId)
		KVSCF_PARAMETERS_END

		KviConsoleWindow * cons;
		if(KVSCF_pParams->count() > 0)
		{
			cons = g_pApp->findConsole(uContextId);
		}
		else
		{
			cons = KVSCF_pContext->window()->console();
			if(!cons)
				KVSCF_pContext->warning(__tr2qs_ctx("This window is not associated to an IRC context", "kvs"));
		}

		KVSCF_pRetBuffer->setInteger((kvs_int_t)(cons ? cons->numericId() : 0));
		return true;
	}

	/*
		@doc: context
		@type:
			function
		@title:
			$context
		@short:
			Retrieves the ID of the specified IRC context
		@syntax:
			$context[(<server>,<nickname>)]
		@description:
			Returns the [b]IRC context ID[/b] of the IRC context that uses
			the specified <server> and local user's <nickname>.[br] This function can
			find only connected IRC contexts.
			If no context matches the server and nickname, an invalid
			[b]IRC context ID 0[/b] is returned.[br]
			If <server> is an empty string, the first context that matches
			the specified nickname is returned. If <nickname> is an empty string,
			the first context that uses the specified server is returned.
			If both parameters are missing this function returns the
			id of the current IRC context, or [b]0[/b] if the
			window that this call is executed in is not bound to any IRC context.
			Please note that in this last case you may find an [b]IRC context[/b]
			that is [i]not connected[/i].
			This can only happen if the current window is a console that is
			in the [i]idle[/i] state, with no connection established yet.[br]
			It is a good idea to take a look at the
			[doc:window_naming_conventions]window naming conventions[/doc].
		@seealso:
			[fnc]$window.context[/fnc]
	*/

	KVSCF(context)
	{
		QString szServer, szNick;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("server", KVS_PT_STRING, KVS_PF_OPTIONAL, szServer)
		KVSCF_PARAMETER("nick", KVS_PT_STRING, KVS_PF_OPTIONAL, szNick)
		KVSCF_PARAMETERS_END

		KviConsoleWindow * cons;
		if(!szServer.isEmpty() || !szNick.isEmpty())
			cons = g_pApp->findConsole(szServer, szNick);
		else
			cons = KVSCF_pContext->window()->console();

		KVSCF_pRetBuffer->setInteger(cons ? cons->context()->id() : 0);
		return true;
	}

	/*
		@doc: countStatusBarItems
		@type:
			function
		@title:
			$countStatusBarItems
		@short:
			Returns the number of items in the statusbar
		@syntax:
			<int> $countStatusBarItems
		@description:
			Returns the number of items in the statusbar.
		@seealso:
			[class]widget[/class]
	*/

	KVSCF(countStatusBarItems)
	{
		if(g_pMainWindow->mainStatusBar())
		{
			QList<QWidget *> widgets = g_pMainWindow->mainStatusBar()->findChildren<QWidget *>();
			KVSCF_pRetBuffer->setInteger(widgets.count());
		}
		else
			KVSCF_pRetBuffer->setInteger(0);
		return true;
	}

	/*
		@doc: cr
		@type:
			function
		@title:
			$cr
		@short:
			Returns a carriage return character
		@syntax:
			<string> $cr
		@description:
			Returns a carriage return character
		@seealso:
			[fnc]$lf[/fnc], [fnc]$ascii[/fnc], [fnc]$char[/fnc]
	*/

	KVSCF(cr)
	{
		KVSCF_pRetBuffer->setString(QString(QChar('\r')));
		return true;
	}

	// The formatting of the table contents has to remain this way (i.e. long lines),
	// else it adds long spaces between words in same sentence which looks awful on screen + harder to read.
	/*
		@doc: date
		@type:
			function
		@title:
			$date
		@short:
			Returns a date/time string using a specified format
		@syntax:
			<string> $date(<format:string>[,<unixtime:integer>])
		@description:
			Returns the string representation of <unixtime> or
			of the current time if <unixtime> is not given, based on <format>.[br]
			The <format string> should contain a set of characters
			that will be transformed according to the following rules:
			[table]
				[tr][td][b]a[/b][/td][td]The abbreviated weekday name according to the current locale.[/td][/tr]
				[tr][td][b]A[/b][/td][td]The full weekday name according to the current locale.[/td][/tr]
				[tr][td][b]b[/b][/td][td]The abbreviated month name according to the current locale.[/td][/tr]
				[tr][td][b]B[/b][/td][td]The full month name according to the current locale.[/td][/tr]
				[tr][td][b]c[/b][/td][td]The preferred date and time representation for the current locale.[/td][/tr]
				[tr][td][b]C[/b][/td][td]The century number (year/100) as a 2-digit integer. (SU)[/td][/tr]
				[tr][td][b]d[/b][/td][td]The day of the month as a decimal number (range 01 to 31).[/td][/tr]
				[tr][td][b]D[/b][/td][td]Equivalent to m/d/y.[/td][/tr]
				[tr][td][b]e[/b][/td][td]Like d, the day of the month as a decimal number, but a leading zero is replaced by a space. (SU)[/td][/tr]
				[tr][td][b]F[/b][/td][td]Equivalent to Y-m-d (the ISO 8601 date format). (C99)[/td][/tr]
				[tr][td][b]h[/b][/td][td]Equivalent to b.[/td][/tr]
				[tr][td][b]H[/b][/td][td]The hour as a decimal number using a 24-hour clock (range 00 to 23).[/td][/tr]
				[tr][td][b]I[/b][/td][td]The hour as a decimal number using a 12-hour clock (range 01 to 12).[/td][/tr]
				[tr][td][b]j[/b][/td][td]The day of the year as a decimal number (range 001 to 366).[/td][/tr]
				[tr][td][b]k[/b][/td][td]The hour (24-hour clock) as a decimal number (range 0 to 23); single digits are preceded by a blank. See also [b]H[/b].[/td][/tr]
				[tr][td][b]l[/b][/td][td]The hour (12-hour clock) as a decimal number (range 1 to 12); single digits are preceded by a blank. See also [b]I[/b].[/td][/tr]
				[tr][td][b]m[/b][/td][td]The month as a decimal number (range 01 to 12).[/td][/tr]
				[tr][td][b]M[/b][/td][td]The minute as a decimal number (range 00 to 59).[/td][/tr]
				[tr][td][b]n[/b][/td][td]A newline character. (SU)[/td][/tr]
				[tr][td][b]p[/b][/td][td]Either [b]AM[/b] or [b]PM[/b] according to the given time value, or the corresponding strings for the current locale. Noon is treated as [b]pm[/b] and midnight as [b]am[/b].[/td][/tr]
				[tr][td][b]r[/b][/td][td]The time in a.m. or p.m. notation. In the POSIX locale this is	equivalent to [b]I:M:S p[/b].[/td][/tr]
				[tr][td][b]s[/b][/td][td]The number of seconds since the Epoch, i.e., since 1970-01-01 00:00:00 UTC.[/td][/tr]
				[tr][td][b]S[/b][/td][td]The second as a decimal number (range 00 to 60). (The range is up to 60 to allow for occasional leap seconds.)[/td][/tr]
				[tr][td][b]t[/b][/td][td]A tab character.[/td][/tr]
				[tr][td][b]T[/b][/td][td]The time in 24-hour notation (H:M:S). (SU)[/td][/tr]
				[tr][td][b]u[/b][/td][td]The day of the week as a decimal, range 1 to 7, Monday being 1. See also [b]w[/b].[/td][/tr]
				[tr][td][b]V[/b][/td][td]The ISO 8601:1988 week number of the current year as a decimal number, range 01 to 53,	where week 1 is the first week that has at least 4 days in the current year, and with Monday as the first day of the week. See also [b]U[/b] and [/b]W.[/td][/tr]
				[tr][td][b]w[/b][/td][td]The day of the week as a decimal, range 0 to 6, Sunday being 0.[/td][/tr]
				[tr][td][b]W[/b][/td][td]The week number of the current year as a decimal number, range 00 to 53, starting with the first Monday as the first day of week 01.[/td][/tr]
				[tr][td][b]y[/b][/td][td]The year as a decimal number without a century (range 00 to 99).[/td][/tr]
				[tr][td][b]Y[/b][/td][td]The year as a decimal number including the century.[/td][/tr]
				[tr][td][b]z[/b][/td][td]The time-zone as hour offset from GMT. Required to emit RFC822-compliant dates (using [i]a, d b Y H:M:S z[/i]).[/td][/tr]
				[tr][td][b]Z[/b][/td][td]The time zone or name or abbreviation (not implemented yet).[/td][/tr]
			[/table]
		@examples:
			[example]
				[cmd]echo[/cmd] $date("d/m/Y H:M:S")
			[/example]
		@seealso:
			[fnc]$unixtime[/fnc], [fnc]$hptimestamp[/fnc]
		@author:
			Kai Wasserbäch <debian@carbon-project.org>
	*/

	KVSCF(date)
	{
		QString szFormat;
		kvs_int_t iTime;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("format", KVS_PT_NONEMPTYSTRING, 0, szFormat)
		KVSCF_PARAMETER("unixtime", KVS_PT_INT, KVS_PF_OPTIONAL, iTime)
		KVSCF_PARAMETERS_END

		// strftime() is not sufficient, as shown by #769, the following is
		// derived from KDateTime (not a copy, but the structure is similar,
		// which isn't surprising), but limited/extended to the required set of
		// functionality.
		// The reason for this is platform independent.
		QDateTime qDt;
		QString szFmtTime;
		int iLength, iVal, iTemp, iLocalTzH, iLocalTzM, iUtcH, iUtcM, iTzOffset;
		int iTzOffsetH, iTzOffsetM;
		QChar cDiv;

		if(KVSCF_pParams->count() > 1)
			qDt.setTime_t(iTime);
		else
			qDt = QDateTime::currentDateTime();

		if(!qDt.isValid())
		{
			KVSCF_pContext->warning(__tr2qs_ctx("Couldn't construct QDateTime object.", "kvs"));
			goto leavenow;
		}

		for(auto i : szFormat)
		{
			ushort ch = i.unicode();
			iLength = 2;
			iVal = 0x8000000;
			cDiv = ' ';

			switch(ch)
			{
				// FIXME: G, g, U, x, X, Z not implemented yet.
				//
				// E and O probably never will be implemented.
				case 'a': // the abbreviated localized day name (e.g.
					// 'Mon' to 'Sun'). Uses
					// QDate::shortDayName().
					szFmtTime += qDt.toString("ddd");
					break;
				case 'A': // the long localized day name (e.g.
					// 'Monday' to 'Qt::Sunday'). Uses
					// QDate::longDayName().
					szFmtTime += qDt.toString("dddd");
					break;
				case 'b': // the abbreviated localized month name
				case 'h': // (e.g. 'Jan' to 'Dec'). Uses
					// QDate::shortMonthName().
					szFmtTime += qDt.toString("MMM");
					break;
				case 'B': // the long localized month name (e.g.
					// 'January' to 'December'). Uses
					// QDate::longMonthName().
					szFmtTime += qDt.toString("MMMM");
					break;
				case 'c': // If the format is Qt::TextDate, the string
					// is formatted in the default way.
					szFmtTime += qDt.toString(Qt::TextDate);
					break;
				case 'C': // 2-digit "century" (yyyy/100)
					iVal = qDt.date().year() / 100;
					break;
				case 'd': // the day as number with a leading zero
					// (01 to 31)
					cDiv = '0';
				// break omitted on purpose, NEXT MUST BE 'e'
				case 'e': // 2-character wide day representation,
					// space padded
					iVal = qDt.date().day();
					break;
				case 'D': // American formatting
					szFmtTime += qDt.toString("MM/dd/yy");
					break;
				case 'F': // yyyy-MM-dd (ISO 8601)
					szFmtTime += qDt.date().toString(Qt::ISODate);
					break;
				case 'H': // the hour with a leading zero (00 to 23)
					cDiv = '0';
				// no break; on purpose (NEXT MUST BE case 'k')
				case 'k': // same as H, only that single-character values
					// are prefixed with a space
					iVal = qDt.time().hour();
					break;
				case 'I': // hour, 01 - 12
					cDiv = '0';
				// fall through to 'l'
				case 'l': // hour, 1 - 12
					iVal = (qDt.time().hour() + 11) % 12 + 1;
					break;
				case 'j': // day of the year (001 to 365/366)
					iVal = qDt.date().dayOfYear();
					iLength = 3;
					cDiv = '0';
					break;
				case 'M': // minutes (00 to 59)
					szFmtTime += qDt.toString("mm");
					break;
				case 'm': // month (01-12)
					szFmtTime += qDt.toString("MM");
					break;
				case 'n': // newline
					szFmtTime += "\n";
					break;
				case 'p': // AM/PM
					// FIXME: l10n for the am/pm? Maybe we can
					//        draw it from a different source?
					//        Qt doesn't offer l10n strings
					//        for this.
					if(qDt.time().hour() < 12)
						szFmtTime += "AM";
					else
						szFmtTime += "PM";
					break;
				case 'P': // am/pm, the same FIXME as for p applies.
					if(qDt.time().hour() < 12)
						szFmtTime += "am";
					else
						szFmtTime += "pm";
					break;
				case 'r': // "I:M:S p"
					szFmtTime += qDt.toString("hh:mm:ss AP");
					break;
				case 'R': // "H:M"
					szFmtTime += qDt.toString("hh:mm");
					break;
				case 's': // seconds since epoch (currently 1970-01-01 00:00:00 UTC)
					szFmtTime += QString::number(qDt.toTime_t());
					break;
				case 'S': // seconds (00-60)
					cDiv = '0';
					iVal = qDt.time().second();
					break;
				case 't': // Tab
					szFmtTime += "\t";
					break;
				case 'T': // H:M:S
					szFmtTime += qDt.toString("hh:mm:ss");
					break;
				case 'u': // day of the week (1-7)
					iVal = qDt.date().dayOfWeek();
					iLength = 1;
					break;
				case 'V': // week of the year (ISO 8601)
				case 'W': // W is not entirely correct, but that's a
					// lot easier this way.
					iVal = qDt.date().weekNumber();
					cDiv = '0';
					break;
				case 'w': // day of week (0-6, 0==Sunday)
					iTemp = qDt.date().dayOfWeek();
					if(iTemp == Qt::Sunday)
						iTemp = 0;
					szFmtTime += QString::number(iTemp);
					break;
				case 'y': // year (2-character)
					szFmtTime += qDt.toString("yy");
					break;
				case 'Y': // year (4-character)
					szFmtTime += qDt.toString("yyyy");
					break;
				case 'z': // numerical timezone offset
					iLocalTzH = qDt.time().hour();
					iLocalTzM = qDt.time().minute();
					iUtcH = qDt.toUTC().time().hour();
					iUtcM = qDt.toUTC().time().minute();
					iTzOffsetH = iLocalTzH - iUtcH;
					iTzOffsetM = iLocalTzM - iUtcM;
					iTzOffset = iTzOffsetH * 100 + iTzOffsetM;

					//clamp to -12:00 / +12:00 (ticket #924)
					if(iTzOffset < 1200)
						iTzOffset += 2400;

					if(iTzOffset > 1200)
						iTzOffset -= 2400;

					if(iTzOffset > 0)
						szFmtTime += "+";

					szFmtTime += QString("%1").arg(iTzOffset, 4, 10, QChar('0'));
					break;
				// FIXME
				// The abbrev. time zone name is a little bit trickier,
				// as I don't want to reimplement KTimeZone, but can't
				// use KTimeZone. I'll need to do some research on this.
				//case 'Z':   // TZ abbrev.
				//    szFmtTime +=
				default:
					szFmtTime += i;
			}

			if(iVal != 0x8000000)
			{
				if(iVal < 0)
				{
					iVal = -iVal;
					szFmtTime += "-";
				}
				szFmtTime += QString("%1").arg(iVal, iLength, 10, cDiv);
			}
		}

		KVSCF_pRetBuffer->setString(szFmtTime);

	leavenow:
		return true;
	}

	/*
		@doc: escape
		@type:
			function
		@title:
			$escape
		@short:
			Returns a kvs-escaped version of the string
		@syntax:
			<string> $escape(<text:string>)
		@description:
			In KVS some characters in a string have special meanings: [b]%[/b] marks the start of a variable name, [b]$[/b] the start of a function name, etc..[br]
			Sometimes you could need to escape them using a [b]\[/b] character to avoid KVIrc from interpreting the special meaning of these characters:
			this function will to the dirty job for you, returning a correctly kvs-escaped version of the string passed as a parameter.[br]
		@seealso:
			[cmd]eval[/cmd]
	*/

	KVSCF(escape)
	{
		QString szData;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("text", KVS_PT_STRING, KVS_PF_OPTIONAL, szData)
		KVSCF_PARAMETERS_END

		if(!szData.isEmpty())
			KviQString::escapeKvs(&szData);

		KVSCF_pRetBuffer->setString(szData);
		return true;
	}

	/*
		@doc: false
		@type:
			function
		@title:
			$false
		@short:
			The boolean false constant
		@syntax:
			<boolean> $false
		@description:
			Evaluates to the false boolean constant. False
			is equivalent to the integer [b]0[/b] too. This function/constant
			is useful to keep your code readable: when you
			have a variable that can assume boolean values it's
			nicer to use [fnc]$true[/fnc] and $false instead of
			the integer constants [b]1[/b] and [b]0[/b]. The reader will
			understand immediately that the variable simply can't
			assume any other value.
		@examples:
			[example]
				%a = $false
				[cmd]echo[/cmd] $typeof(%a)
				[cmd]echo[/cmd] $(%a + 1)
			[/example]
		@seealso:
			[fnc]$true[/fnc]
	*/

	KVSCF(falseCKEYWORDWORKAROUND)
	{
		KVSCF_pRetBuffer->setBoolean(false);
		return true;
	}

	/*
		@doc: features
		@type:
			function
		@title:
			$features
		@short:
			Returns the features that KVIrc supports
		@syntax:
			<array> $features()
			<boolean> $features(<test_feature:string>)
		@description:
			The parameterless form returns an array of feature description strings that this KVIrc executable supports.[br]
			This function is useful when some part of your script depends on
			an optional KVIrc feature (like SSL support or IPv6 support).[br]
			The returned value may be assigned to a dictionary too: it will be used to simulate an array.[br]
			The form with the [test_feature] parameter returns true if and only if [test_feature] is available.[br]
		@examples:
			[example]
				%myfeats[] = $features
				[cmd]echo[/cmd] %myfeats[]
				%i = %myfeats[]#
				[cmd]while[/cmd](%i > 0)
				{
					%i--;
					[cmd]echo[/cmd] "Supporting feature %myfeats[%i]"
				}
			[/example]
			Nearly the same loop, just really shorter:
			[example]
			[cmd]foreach[/cmd](%f,$features)
				[cmd]echo[/cmd] "Supporting feature %myfeats[%i]"
			[/example]
			You can test for a specific feature in the following way:
			[example]
				[cmd]if[/cmd]($features("SSL"))[cmd]echo[/cmd] "Yes! SSL is available";
			[/example]
			If used in [i]non-array[/i] context it returns just a comma separated list of entries:
			[example]
				[cmd]echo[/cmd] $features
			[/example]
		@seealso:
			[fnc]$version[/fnc]
	*/

	KVSCF(features)
	{
		QString szFeature;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("test_feature", KVS_PT_STRING, KVS_PF_OPTIONAL, szFeature)
		KVSCF_PARAMETERS_END

		if(!szFeature.isEmpty())
		{
			KVSCF_pRetBuffer->setBoolean(feature_list.contains(szFeature, Qt::CaseInsensitive));
		}
		else
		{
			KviKvsArray * a = new KviKvsArray();
			for(auto&& f : feature_list)
				a->append(new KviKvsVariant(f));
			KVSCF_pRetBuffer->setArray(a);
		}

		return true;
	}

	/*
		@doc: firstconnectedconsole
		@type:
			function
		@title:
			$firstConnectedConsole
		@short:
			Returns the window ID of the first connected console
		@syntax:
			<uint> $firstConnectedConsole()
		@description:
			Returns the window ID of the first connected console
			or 0 if no console is currently connected.
	*/

	KVSCF(firstConnectedConsole)
	{
		KviConsoleWindow * c = g_pApp->topmostConnectedConsole();
		KVSCF_pRetBuffer->setInteger(c ? c->numericId() : 0);
		return true;
	}

	/*
		@doc: flatten
		@type:
			function
		@title:
			$flatten
		@short:
			Returns a flattened array of items
		@syntax:
			<array> $flatten(<data1:variant>[,<data2:variant>[,...]])
		@description:
			Returns an array of items built from the passed arguments
			with the following rules:
			[ul]
				[li]If an argument is a scalar value then the argument itself is appended to the result.[/li]
				[li]If an argument is an array then each contained item is appended to the result.[/li]
				[li]If an argument is a hash then each contained value is appended to the result.[/li]
			[/ul]
			A simple example of usage is to merge N arrays into a new one.
			(Please note that for merging one array into another the
			[doc:arrayconcatenation]array concatenation operator[/doc] is more efficient).
	*/

	KVSCF(flatten)
	{
		KviKvsArray * a = new KviKvsArray();
		KVSCF_pRetBuffer->setArray(a);
		unsigned int uIdx = 0;
		for(KviKvsVariant * v = KVSCF_pParams->first(); v; v = KVSCF_pParams->next())
		{
			switch(v->type())
			{
				case KviKvsVariantData::Array:
				{
					KviKvsArray * z = v->array();
					unsigned int uSize = z->size();
					unsigned int uIdx2 = 0;
					while(uIdx2 < uSize)
					{
						KviKvsVariant * pInternal = z->at(uIdx2);
						if(pInternal)
							a->set(uIdx, new KviKvsVariant(*pInternal));
						// else
						//	don't set anything: just leave empty entry (nothing)
						uIdx++;
						uIdx2++;
					}
				}
				break;
				case KviKvsVariantData::Hash:
				{
					KviKvsHash * h = v->hash();
					KviKvsHashIterator it(*(h->dict()));
					while(KviKvsVariant * pInternal = it.current())
					{
						a->set(uIdx, new KviKvsVariant(*pInternal));
						uIdx++;
						++it;
					}
				}
				break;
				default:
					a->set(uIdx, new KviKvsVariant(*v));
					uIdx++;
					break;
			}
		}
		return true;
	}

	/*
		@doc: fmtlink
		@type:
			function
		@title:
			$fmtlink
		@short:
			Returns a formatted link buffer
		@syntax:
			<string> $fmtlink(<link_text:string>,<double_click_command:string>[,<tooltip_text:string>])
		@description:
			Returns a link formatted for the [cmd]echo[/cmd] command.[br]
			If you pass the returned string to the echo command, the string will be displayed
			as a link and will be highlighted when the user moves the mouse over it.[br]
			If the user leaves the mouse for a few seconds over the link, the <tooltip_text>
			will be displayed in a small tooltip window. If <tooltip_text> is not given,
			then no tooltip will be shown.[br]
			The <double_click_command> will be executed when the user will double click on the link.[br]
			Please remember that if <double_click_command> contains identifiers
			that must be evaluated at double-click time, you MUST escape them in the $fmtlink() call
			to prevent the evaluation.[br]
			You might also take a look at [doc:escape_sequences]the escape sequences documentation[/doc]
			to learn more about how the links are implemented and how to create more powerful links (add
			right and middle button actions, use predefined KVIrc links etc...). Also take a look at [fnc]$link[/fnc]
			which has related functionality.
		@seealso:
			[cmd]echo[/cmd], [doc:escape_sequences]the escape sequences documentation[/doc], [fnc]$link[/fnc]
	*/

	KVSCF(fmtlink)
	{
		QString szLinkText, szCmd, szToolTip;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("link_text", KVS_PT_NONEMPTYSTRING, 0, szLinkText)
		KVSCF_PARAMETER("double_click_command", KVS_PT_STRING, 0, szCmd)
		KVSCF_PARAMETER("tooltip_text", KVS_PT_STRING, KVS_PF_OPTIONAL, szToolTip)
		KVSCF_PARAMETERS_END

		QString szPart = QString("[!dbl]%1").arg(szCmd);
		if(!szToolTip.isEmpty())
			KviQString::appendFormatted(szPart, "[!txt]%Q", &szToolTip);
		QString szLink = QString("\r!%1\r%2\r").arg(szPart, szLinkText);

		KVSCF_pRetBuffer->setString(szLink);
		return true;
	}
};
