//=============================================================================
//
//   File : KviKvsCoreFunctions_gl.cpp
//   Creation date : Fri 31 Oct 2003 01:52:04 by Szymon Stefanek
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

#include "KviKvsCoreFunctions.h"
#include "KviKvsKernel.h"
#include "KviKvsTimerManager.h"
#include "KviKvsArrayCast.h"
#include "KviLocale.h"
#include "KviInput.h"
#include "KviApplication.h"
#include "KviChannelWindow.h"
#include "KviConsoleWindow.h"
#include "KviIconManager.h"
#include "KviControlCodes.h"
#include "KviMainWindow.h"
#include "KviTimeUtils.h"
#include "KviKvsEventManager.h"
#include "KviKvsEventHandler.h"
#include "KviLagMeter.h"
#include "KviIrcUserEntry.h"

#include <QRegExp>

namespace KviKvsCoreFunctions
{
	/*
		@doc:	gender
		@type:
			function
		@title:
			$gender
		@short:
			Returns the gender of the specified user
		@syntax:
			<string> $gender(<nickname:string>)
		@description:
			Returns the gender, or 'unknown' if not set from remote user, of the specified IRC user IF it is known.[br]
			The gender is known if [fnc]$isWellKnown[/fnc] returns 1.[br]
			The gender is generally known if the user is on a channel with you
			or has an open query with you.[br]
			Detailed explanation:[br]
			KVIrc has an internal database of users that are currently
			visible by *this client*: this includes users on open channels
			and queries.[br] The other IRC users are [b]not[/b] in the database:
			this means that KVIrc knows [b]nothing[/b] about them and can't return
			any information immediately. In this case this function will return
			an EMPTY string.[br]
			If a user is in the database, at least his nickname is known.[br]
			The username and hostname are known only if the server provides that information
			spontaneously or after a KVIrc request.[br]
			KVIrc requests user information for all the users in open queries
			and channels. This information takes some time to be retrieved,
			in this interval of time KVIrc knows only the user's nickname.
			This function will return the string [b]*[/b] in this case.[br]
		@seealso:
			[fnc]$isWellKnown[/fnc], [fnc]$hostname[/fnc], [fnc]$realname[/fnc], [cmd]$username[/cmd]
	*/

	KVSCF(gender)
	{
		QString szNick;

		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("nick", KVS_PT_NONEMPTYSTRING, 0, szNick)
		KVSCF_PARAMETERS_END

		KviConsoleWindow * c = KVSCF_pContext->window()->console();
		if(c && c->isConnected())
		{
			KviIrcUserEntry * e = KVSCF_pContext->window()->connection()->userDataBase()->find(szNick);
			if(e)
			{
				switch(e->gender())
				{
					case KviIrcUserEntry::Male:
						KVSCF_pRetBuffer->setString(QStringLiteral("male"));
						break;
					case KviIrcUserEntry::Female:
						KVSCF_pRetBuffer->setString(QStringLiteral("female"));
						break;
					default:
						KVSCF_pRetBuffer->setString(QStringLiteral("unknown"));
				}
				return true;
			}
		}

		KVSCF_pRetBuffer->setNothing();
		return true;
	}

	/*
		@doc: globals
		@type:
			function
		@title:
			$globals
		@short:
			Dump globals variables
		@syntax:
			<hash> $globals()
		@description:
			Returns an hashtable of all global variables set.
		@examples:
			[example]
				echo $globals
			[/example]
		@seealso:
			[cmd]global[/cmd]
			[cmd]unset[/cmd]
			[fnc]$hash[/fnc]
	*/

	KVSCF(globals)
	{
		KVSCF_pRetBuffer->setHash(new KviKvsHash(*(KviKvsKernel::instance()->globalVariables())));
		return true;
	}

	/*
		@doc: hash
		@type:
			function
		@title:
			$hash
		@short:
			Explicitly creates a hash
		@syntax:
			<hash> $hash([<key:string>,<value:variant>,[<key:string>,<value:variant>]],...)
		@description:
			Returns a hash with the specified items. Each <value> is indexed by the preceding <key>.
			This is just an explicit way of creating a hash with a defined set of items,
			useful for increasing readability.
		@examples:
			[example]
				[cmd]alias[/cmd](test){ [cmd]return[/cmd] $hash(1,X,2,Y,3,Z); };
				%x = $test();
				[cmd]foreach[/cmd](%y,%x)
				{
					[cmd]echo[/cmd] %y;
				}
				[cmd]foreach[/cmd](%y,[fnc]$keys[/fnc](%x))
				{
					[cmd]echo[/cmd] %y
				}
			[/example]
		@seealso:
			[fnc]$array[/fnc]
	*/

	KVSCF(hash)
	{
		KviKvsHash * a = new KviKvsHash();

		for(KviKvsVariant * key = KVSCF_pParams->first(); key; key = KVSCF_pParams->next())
		{
			KviKvsVariant * val = KVSCF_pParams->next();
			if(!val)
				break;
			QString sz;
			key->asString(sz);
			a->set(sz, new KviKvsVariant(*val));
		}

		KVSCF_pRetBuffer->setHash(a);
		return true;
	}

	/*
		@doc: hextoAscii
		@type:
			function
		@title:
			$hexToAscii
		@short:
			Returns a decoded hex string
		@syntax:
			$hexToAscii(<hex_encoded_string>)
		@description:
			Decodes the <hex_encoded_string> to its ASCII representation.
		@examples:
			[example]
				[cmd]echo[/cmd] $hexToAscii(6B76697263)
			[/example]
		@seealso:
			[fnc]$asciiToHex[/fnc]
	*/

	KVSCF(hexToAscii)
	{
		QString szHex;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("hex_encoded_string", KVS_PT_STRING, 0, szHex)
		KVSCF_PARAMETERS_END

		KviCString tmp1(szHex);
		char * buf;
		int len = tmp1.hexToBuffer(&buf, true);
		KVSCF_pRetBuffer->setString(QString(QByteArray(buf, len)));
		KviCString::freeBuffer(buf);
		return true;
	}

	/*
		@doc: hostname
		@type:
			function
		@title:
			$hostname
		@short:
			Returns the hostname of the specified user
		@syntax:
			<string> $hostname(<nickname:string>)
		@description:
			Returns the hostname of the specified IRC user IF it is known.[br]
			The hostname is known if [fnc]$isWellKnown[/fnc] returns 1.[br]
			The hostname is generally known if the user is on a channel with you
			or has an open query with you.[br]
			Detailed explanation:[br]
			KVIrc has an internal database of users that are currently
			visible by *this client*: this includes users on open channels
			and queries.[br] The other IRC users are [b]not[/b] in the database:
			this means that KVIrc knows NOTHING about them and can't return
			any information immediately. In this case this function will return
			an EMPTY string.[br]
			If a user is in the database, at least his nickname is known.[br]
			The username and hostname are known only if the server provides that information
			spontaneously or after a KVIrc request.[br]
			KVIrc requests user information for all the users in open queries
			and channels. This information takes some time to be retrieved,
			in this interval of time KVIrc knows only the user's nickname.
			This function will return the string [b]*[/b] in this case.[br]
		@seealso:
			[fnc]$isWellKnown[/fnc], [fnc]$username[/fnc], [cmd]awhois[/cmd]
	*/

	KVSCF(hostname)
	{
		QString szNick;

		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("nick", KVS_PT_NONEMPTYSTRING, 0, szNick)
		KVSCF_PARAMETERS_END

		KviConsoleWindow * c = KVSCF_pContext->window()->console();
		if(c && c->isConnected())
		{
			KviIrcUserEntry * e = KVSCF_pContext->window()->connection()->userDataBase()->find(szNick);
			if(e)
			{
				KVSCF_pRetBuffer->setString(e->host());
				return true;
			}
		}

		KVSCF_pRetBuffer->setNothing();
		return true;
	}

	/*
		@doc: lag
		@type:
			function
		@title:
			$lag
		@short:
			Returns the lag on the current server
		@syntax:
			<integer> $lag
		@description:
			This function returns the lag in the current server, in milliseconds.[br]
	*/

	KVSCF(lag)
	{
		if(!KVSCF_pContext->window()->console())
			return KVSCF_pContext->errorNoIrcContext();
		if(!KVSCF_pContext->window()->console()->connection())
			return KVSCF_pContext->warningNoIrcConnection();
		if(!KVSCF_pContext->window()->console()->connection()->lagMeter())
		{
			KVSCF_pContext->warning(__tr2qs_ctx("Lag meter was not enabled", "kvs"));
			return false;
		}

		KVSCF_pRetBuffer->setInteger(KVSCF_pContext->window()->console()->connection()->lagMeter()->lag());
		return true;
	}

	/*
		@doc: hptimestamp
		@type:
			function
		@title:
			$hptimestamp
		@short:
			Returns a high precision timestamp
		@syntax:
			<float> $hptimestamp
		@description:
			Returns a high precision timestamp as a floating point value.
			The timestamp represents the number of seconds elapsed since
			a certain moment in the past. The number of seconds
			contains a [i]high precision[/i] fractional part. The [i]certain moment[/i] definition
			depends on the platform KVIrc is executed. This means that
			this timestamp is totally useless to represent a time value
			but is useful to compute time intervals with sub-second precision.
		@examples:
			[example]
				%tmp = $hptimestamp
				[cmd]echo[/cmd] $($hptimestamp - %tmp)
			[/example]
		@seealso:
			[fnc]$unixTime[/fnc], [fnc]$date[/fnc]
	*/

	KVSCF(hptimestamp)
	{
		struct timeval tv;
		kvi_gettimeofday(&tv);
		kvs_real_t dTimestamp = (kvs_real_t)(tv.tv_sec);
		dTimestamp += (((kvs_real_t)(tv.tv_usec)) / 1000000.0);
		KVSCF_pRetBuffer->setReal(dTimestamp);
		return true;
	}

	/*
		@doc: i
		@type:
			function
		@title:
			$i
		@short:
			Returns the ITALIC mIRC control character
		@syntax:
			<string> $i
		@description:
			Returns the ITALIC mIRC control character (Ctrl+I).[br]
		@seealso:
			[fnc]$k[/fnc], [fnc]$b[/fnc], [fnc]$u[/fnc], [fnc]$r[/fnc], [fnc]$o[/fnc]
	*/

	KVSCF(i)
	{
		KVSCF_pRetBuffer->setString(QString(QChar(KviControlCodes::Italic)));
		return true;
	}

	/*
		@doc: icon
		@type:
			function
		@title:
			$icon
		@short:
			Returns the ID of an icon
		@syntax:
			<integer> $icon(<iconname:string>)
		@description:
			Returns the ID of the icon <iconname>.
			You should always use this function where an <image_id> is required
			and you want to use an internal icon.
			See [fnc]$iconname[/fnc] for a list of the icon names supported by KVIrc.[br]
		@examples:
			[example]
				[cmd]echo[/cmd] $icon(linux)
			[/example]
		@seealso:
			[fnc]$iconName[/fnc]
	*/

	KVSCF(icon)
	{
		QString szName;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("iconName", KVS_PT_NONEMPTYSTRING, 0, szName)
		KVSCF_PARAMETERS_END

		KVSCF_pRetBuffer->setInteger(g_pIconManager->getSmallIconIdFromName(szName));
		return true;
	}

	/*
		@doc: iconname
		@type:
			function
		@title:
			$iconname
		@short:
			Returns the name of an icon
		@syntax:
			$iconname(<iconid>)
		@description:
			Returns the name of a builtin icon given an <iconid>.
			The <iconid> is returned by the function [fnc]$icon[/fnc].
			If the <iconid> is not valid, an empty name is returned.[br]
			The following code will list all the available icon names:
			[example]
				%i = 0
				[cmd]do[/cmd] {
					%name = $iconname(%i)
					echo The icon by ID %i is named %name
					%i++
				} [cmd]while[/cmd](%name != "")
			[/example]
		@examples:
			[example]
				[cmd]echo[/cmd] $iconname(24)
				[cmd]echo[/cmd] $iconname([fnc]$icon[/fnc](linux))
			[/example]
		@seealso:
			[fnc]$icon[/fnc]
	*/

	KVSCF(iconName)
	{
		kvs_uint_t uIco;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("iconid", KVS_PT_UINT, 0, uIco)
		KVSCF_PARAMETERS_END

		if(uIco < KviIconManager::IconCount)
			KVSCF_pRetBuffer->setString(g_pIconManager->getSmallIconName(uIco));
		else
			KVSCF_pRetBuffer->setNothing();
		return true;
	}

	/*
		@doc: insideAlias
		@type:
			function
		@title:
			$insideAlias
		@short:
			Returns true if kvs is executing an alias
		@syntax:
			<bool> $insideAlias()
		@description:
			This function can be used to know if the code is currently being
			executed in the context of an alias.
			If $insideAlias is called from inside an alias, it returns true.
			If $insideAlias is called from inside a function, event handler or
			everything else not being a direct /alias call, it will return false.
		@seealso:
			[fnc]$sw[/fnc]
	*/

	KVSCF(insideAlias)
	{
		KVSCF_pRetBuffer->setBoolean(KVSCF_pContext->aliasSwitchList());
		return true;
	}

	/*
		@doc: integer
		@type:
			function
		@title:
			$integer
		@short:
			Casts a variable to an integer
		@syntax:
			<integer> $integer(<data:variant>)
		@description:
			Forces <data> to be an integer data type with the following
			semantics:
			[ul]
				[li]If <data> is an integer, <data> itself is returned.[/li]
				[li]If <data> is a boolean, its numeric value is returned (either 1 or 0).[/li]
				[li]If <data> is a real, its integer part is returned.[/li]
				[li]If <data> is an array, the count of its items is returned.[/li]
				[li]If <data> is a hash, the count of its items is returned.[/li]
				[li]If <data> is a string, its length is returned.[/li]
				[li]If <data> is an object, 0 is returned if the reference is null (invalid) and 1 otherwise[/li]
			[/ul]
			This function is similar to the C (int) cast and is internally
			aliased to [fnc]$int[/fnc]() too.
			Note that since KVIrc does most of the casting work automatically
			you shouldn't need to use this function.
		@seealso:
			[fnc]$real[/fnc]
			[fnc]$boolean[/fnc]
	*/

	/*
		@doc: int
		@type:
			function
		@title:
			$int
		@short:
			Casts a variable to an integer
		@syntax:
			<integer> $int(<data:variant>)
		@description:
			This is an internal alias to [fnc]$integer[/fnc]().
		@seealso:
			[fnc]$real[/fnc]
			[fnc]$boolean[/fnc]
	*/

	KVSCF(integer)
	{
		KviKvsVariant * v;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("data", KVS_PT_VARIANT, 0, v)
		KVSCF_PARAMETERS_END

		kvs_int_t iVal;
		v->castToInteger(iVal);
		KVSCF_pRetBuffer->setInteger(iVal);
		return true;
	}

	/*
		@doc:	isbot
		@type:
			function
		@title:
			$isbot
		@short:
			Returns [b]1[/b] if the user is a bot. Otherwise return 0.
		@syntax:
			<string> $isbot(<nickname:string>)
		@description:
			Returns [b]1[/b] if the user is a bot. Otherwise return 0.[br]
			This info  is known if [fnc]$isWellKnown[/fnc] returns 1.[br]
			This info is generally known if the user is on a channel with you
			or has an open query with you.[br]
			Detailed explanation:[br]
			KVIrc has an internal database of users that are currently
			visible by *this client*: this includes users on open channels
			and queries.[br] The other IRC users are [b]not[/b] in the database:
			this means that KVIrc knows [b]nothing[/b] about them and can't return
			any information immediately. In this case this function will return
			an EMPTY string.[br]
			If a user is in the database, at least his nickname is known.[br]
			The username and hostname are known only if the server provides that information
			spontaneously or after a KVIrc request.[br]
			KVIrc requests user information for all the users in open queries
			and channels. This information takes some time to be retrieved,
			in this interval of time KVIrc knows only the user's nickname.
			This function will return the string [b]*[/b] in this case.[br]
		@seealso:
			[fnc]$isWellKnown[/fnc], [fnc]$hostname[/fnc], [fnc]$realname[/fnc], [cmd]$username[/cmd]
	*/

	KVSCF(isBot)
	{
		QString szNick;

		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("nick", KVS_PT_NONEMPTYSTRING, 0, szNick)
		KVSCF_PARAMETERS_END

		KviConsoleWindow * c = KVSCF_pContext->window()->console();
		if(c && c->isConnected())
		{
			KviIrcUserEntry * e = KVSCF_pContext->window()->connection()->userDataBase()->find(szNick);
			if(e)
			{
				KVSCF_pRetBuffer->setBoolean(e->isBot());
				return true;
			}
		}
		KVSCF_pRetBuffer->setNothing();
		return true;
	}

	/*
		@doc: isMainWindowActive
		@type:
			function
		@title:
			$isMainWindowActive
		@short:
			Checks if main KVirc window is active
		@syntax:
			<boolean> $isMainWindowActive()
		@description:
			Returns true if the KVIrc's window is currently
			the active window on the current desktop.
	*/

	KVSCF(isMainWindowActive)
	{
		KVSCF_pRetBuffer->setBoolean(g_pMainWindow->isActiveWindow());
		return true;
	}

	/*
		@doc: isMainWindowMinimized
		@type:
			function
		@title:
			$isMainWindowMinimized
		@short:
			Checks if main KVirc window is minimized
		@syntax:
			<boolean> $isMainWindowMinimized()
		@description:
			Returns true if main KVIrc window is minimized and false otherwise.
	*/

	KVSCF(isMainWindowMinimized)
	{
		KVSCF_pRetBuffer->setBoolean(g_pMainWindow->isMinimized());
		return true;
	}

	/*
		@doc: isempty
		@type:
			function
		@title:
			$isEmpty
		@short:
			Checks if a variable is set (empty or non empty)
		@syntax:
			<boolean> $isEmpty(<data:variant>)
		@description:
			Returns 0 if <data> is actually set to some non-empty value
			and 1 otherwise. Since KVIrc treats unset variables as empty
			ones then this function is the exact opposite of [fnc]$isSet[/fnc].
		@seealso:
			[fnc]$isSet[/fnc]
	*/

	KVSCF(isEmpty)
	{
		KviKvsVariant * v;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("data", KVS_PT_VARIANT, 0, v)
		KVSCF_PARAMETERS_END

		KVSCF_pRetBuffer->setBoolean(v->isEmpty());
		return true;
	}
	//FIXME: documentation

	/*
		@doc: iseventenabled
		@type:
			function
		@title:
			$isEventEnabled
		@short:
			Checks if an event enabled
		@syntax:
			<boolean> $isEventEnabled(<event_name:string>,<handler_name:string>)
		@description:
			Returns [b]1[/b] if the event handler is enabled.
		@seealso:
			[cmd]event[/cmd] [cmd]eventctl[/cmd]
	*/

	KVSCF(isEventEnabled)
	{
		QString szEventName, szHandlerName;
		KviKvsScriptEventHandler * h = nullptr;

		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("event_name", KVS_PT_NONEMPTYSTRING, 0, szEventName)
		KVSCF_PARAMETER("handler_name", KVS_PT_NONEMPTYSTRING, 0, szHandlerName)
		KVSCF_PARAMETERS_END

		bool bOk;
		int iNumber = szEventName.toInt(&bOk);

		// is raw event?
		if(bOk && (iNumber >= 0) && (iNumber < 1000))
		{
			if(KviKvsEventManager::instance()->isValidRawEvent(iNumber))
				h = KviKvsEventManager::instance()->findScriptRawHandler(iNumber, szHandlerName);
			else
				KVSCF_pContext->warning(__tr2qs_ctx("No such event (%Q)", "kvs"), &szEventName);
		}
		else
		{
			iNumber = KviKvsEventManager::instance()->findAppEventIndexByName(szEventName);
			if(KviKvsEventManager::instance()->isValidAppEvent(iNumber))
				h = KviKvsEventManager::instance()->findScriptAppHandler(iNumber, szHandlerName);
			else
				KVSCF_pContext->warning(__tr2qs_ctx("No such event (%Q)", "kvs"), &szEventName);
		}

		if(h)
			KVSCF_pRetBuffer->setBoolean(h->isEnabled());
		else
			KVSCF_pContext->warning(__tr2qs_ctx("No such event handler (%Q) for event %Q", "kvs"), &szHandlerName, &szEventName);

		return true;
	}

	/*
		@doc: isnumeric
		@type:
			function
		@title:
			$isNumeric
		@syntax:
			<boolean> $isNumeric(<data:variant>)
		@short:
			Finds whether a variable contains a representation of a number
		@description:
			Returns [b]1[/b] if the <data> is an integer or a real number, and [b]0[/b] otherwise.
	*/

	KVSCF(isNumeric)
	{
		KviKvsVariant * v;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("data", KVS_PT_VARIANT, 0, v)
		KVSCF_PARAMETERS_END

		KviKvsNumber n;
		KVSCF_pRetBuffer->setBoolean(v->asNumber(n));
		return true;
	}

	/*
		@doc: isset
		@type:
			function
		@title:
			$isSet
		@short:
			Checks if a variable is set (non-empty)
		@syntax:
			<boolean> $isSet(<data:variant>)
		@description:
			Returns 1 if <data> is actually set to some non-empty value
			and [b]0[/b] otherwise. If <data> is a variable, then this function
			simply checks if the variable is set. If <data> is a constant
			then this function checks if the constant is non empty.
			Since KVIrc treats empty strings as [i]unset[/i] values then
			this function could be also called [i]isNonEmpty[/i] and it is
			the perfect opposite of [fnc]$isEmpty[/fnc]
		@seealso:
			[fnc]$isEmpty[/fnc], [cmd]unset[/cmd]
	*/

	KVSCF(isSet)
	{
		KviKvsVariant * v;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("data", KVS_PT_VARIANT, 0, v)
		KVSCF_PARAMETERS_END

		KVSCF_pRetBuffer->setBoolean(!v->isEmpty());
		return true;
	}

	/*
		@doc: istimer
		@type:
			function
		@title:
			$isTimer
		@short:
			Checks for a timer existence
		@syntax:
			<boolean> $istimer(<name:string>)
		@description:
			Returns [b]1[/b] if the timer named <name> is actually running, and [b]0[/b] otherwise.
		@seealso:
			[cmd]timer[/cmd], [cmd]killtimer[/cmd]
	*/

	KVSCF(isTimer)
	{
		QString szName;

		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("timerName", KVS_PT_NONEMPTYSTRING, 0, szName)
		KVSCF_PARAMETERS_END
		KVSCF_pRetBuffer->setBoolean(KviKvsTimerManager::instance()->timerExists(szName));
		return true;
	}

	/*
		@doc: iswellknown
		@type:
			function
		@title:
			$isWellKnown
		@short:
			Returns $true if the specified user is well known
		@syntax:
			<bool> $isWellKnown(<nickname:string>)
		@description:
			Returns 1 if KVIrc has the basic user information about the specified <nickname>.[br]
			The basic information include the username and hostname.[br]
			This is almost always true if the user is on a channel with you or
			you have an open query with him.[br]
			If $isWellKnown returns 0, [fnc]$username[/fnc] and [fnc]$hostname[/fnc]
			will return empty strings.[br]
			In this case you must use [cmd]awhois[/cmd] to obtain the user basic information.[br]
	*/

	KVSCF(isWellKnown)
	{
		QString szNick;

		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("nick", KVS_PT_NONEMPTYSTRING, 0, szNick)
		KVSCF_PARAMETERS_END

		KviConsoleWindow * c = KVSCF_pContext->window()->console();
		if(c && c->isConnected())
		{
			KviIrcUserEntry * e = KVSCF_pContext->window()->connection()->userDataBase()->find(szNick);
			if(e)
			{
				KVSCF_pRetBuffer->setBoolean(e->hasHost() && e->hasUser());
				return true;
			}
		}

		KVSCF_pRetBuffer->setBoolean(false);
		return true;
	}

	/*
		@doc: k
		@type:
			function
		@title:
			$k
		@short:
			Returns the COLOR mIRC control character
		@syntax:
			<string> $k(<foreground:integer>[,<background:integer>])
			<string> $k
		@description:
			Returns the COLOR mIRC control character (Ctrl+K).[br]
			If <foreground> and <background> are passed, a standard mIRC
			color escape is returned.[br]
		@seealso:
			[fnc]$b[/fnc], [fnc]$i[/fnc], [fnc]$u[/fnc], [fnc]$r[/fnc], [fnc]$o[/fnc]
	*/

	KVSCF(k)
	{
		kvs_uint_t iFore, iBack;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("foreground", KVS_PT_UINT, KVS_PF_OPTIONAL, iFore)
		KVSCF_PARAMETER("background", KVS_PT_UINT, KVS_PF_OPTIONAL, iBack)
		KVSCF_PARAMETERS_END

		QString szRet = QChar(KviControlCodes::Color);
		if(KVSCF_pParams->count() > 0)
		{
			KviQString::appendFormatted(szRet, "%u", iFore);
			if(KVSCF_pParams->count() > 1)
				KviQString::appendFormatted(szRet, ",%u", iBack);
		}
		KVSCF_pRetBuffer->setString(szRet);
		return true;
	}

	/*
		@doc: keys
		@type:
			function
		@title:
			$keys
		@short:
			Returns the array of keys of a hash
		@syntax:
			<array> $keys(<hash_value:hash>)
		@description:
			Returns an array with the keys of the <hash> parameter.
			<hash> must be obviously a hash (or eventually an empty variable
			that is treated as an empty hash).
		@seealso:
			[cmd]foreach[/cmd]
	*/

	KVSCF(keys)
	{
		KviKvsHash * pHash = nullptr;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("hash", KVS_PT_HASH, KVS_PF_OPTIONAL, pHash)
		KVSCF_PARAMETERS_END

		KviKvsArray * a = new KviKvsArray();

		// we have to support an empty hash, returning an empty array (ticket #940)
		if(pHash)
		{
			KviKvsHashIterator it(*(pHash->dict()));
			while(it.current())
			{
				a->append(new KviKvsVariant(it.currentKey()));
				++it;
			}
		}
		KVSCF_pRetBuffer->setArray(a);
		return true;
	}

	/*
		@doc: lang
		@type:
			function
		@title:
			$lang
		@short:
			Returns name of currently used language
		@syntax:
			<string> $lang([<type:string>])
		@description:
			Returns the short name of currently used language.[br]
			Type <type> should be one of: [br]
			[i]full[/i]  - returns full locale name, such as ru_RU.UTF-8 (default).[br]
			[i]lang[/i]  - return language name, such as ru_RU.[br]
			[i]short[/i] - returns only language group such as ru.[br]
	*/

	KVSCF(lang)
	{
		QString szType;

		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("type", KVS_PT_NONEMPTYSTRING, KVS_PF_OPTIONAL, szType)
		KVSCF_PARAMETERS_END

		QString szLocale = KviLocale::instance()->localeName();
		if(szType == QLatin1String("lang"))
			KVSCF_pRetBuffer->setString(szLocale.left(5));
		else if(szType == QLatin1String("short"))
			KVSCF_pRetBuffer->setString(szLocale.left(2));
		else
			KVSCF_pRetBuffer->setString(szLocale);
		return true;
	}

	/*
		@doc: length
		@type:
			function
		@title:
			$length
		@short:
			Returns the length of a variable
		@syntax:
			<integer> $length(<value:variant>)
		@description:
			When <value> is an array or a hash, it returns the number
			of its elements. When <value> is an object reference it returns 0.
			In all the other cases <value> is interpreted
			as a string and its length is returned.
		@seealso:
			[fnc]$str.len[/fnc]
	*/

	KVSCF(length)
	{
		KviKvsVariant * pVar;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("value", KVS_PT_VARIANT, 0, pVar)
		KVSCF_PARAMETERS_END

		switch(pVar->type())
		{
			case KviKvsVariantData::Array:
				KVSCF_pRetBuffer->setInteger(pVar->array()->size());
				break;
			case KviKvsVariantData::Hash:
				KVSCF_pRetBuffer->setInteger(pVar->hash()->size());
				break;
			case KviKvsVariantData::HObject:
				KVSCF_pRetBuffer->setInteger(0);
				break;
			default:
			{
				QString tmp;
				pVar->asString(tmp);
				KVSCF_pRetBuffer->setInteger(tmp.length());
			}
		}
		return true;
	}

	/*
		@doc: lf
		@type:
			function
		@title:
			$lf
		@short:
			Returns a line feed character
		@syntax:
			<string> $lf
		@description:
			Returns a line feed character.
		@seealso:
			[fnc]$cr[/fnc], [fnc]$ascii[/fnc], [fnc]$char[/fnc]
	*/

	KVSCF(lf)
	{
		KVSCF_pRetBuffer->setString(QString(QChar('\n')));
		return true;
	}

	/*
		@doc: link
		@type:
			function
		@title:
			$link
		@short:
			Returns the text specified as a link
		@syntax:
			<string> $link(<text:string>[,<type:string>])
		@description:
			Gets the text you pass as the first parameter and formats it
			so that KVIrc will display it as a link. You can specify the
			type of the link as the second parameter. Valid values for the
			second parameter are:
			[ul]
				[li]nick	link to a nickname[/li]
				[li]channel	link to a channel name[/li]
				[li]host	link to an host[/li]
				[li]URL		(default) link to an URL[/li]
			[/ul]
			Please note that the text you get as the output is valid only
			when interpreted locally. Sending this text to the server you can
			get unpredictable results, depending on the IRC client other people
			is using.
			The returned string format is described by [doc:escape_sequences]the escape sequences documentation[/doc]
			and you might also take a look at [fnc]$fmtlink[/fnc] which has related functionality.
		@examples:
			echo $link(pragma,nick)
		@seealso:
			[cmd]echo[/cmd], [fnc]$fmtlink[/fnc].
	*/

	KVSCF(link)
	{
		QString szData, szType;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("text", KVS_PT_NONEMPTYSTRING, 0, szData)
		KVSCF_PARAMETER("type", KVS_PT_NONEMPTYSTRING, KVS_PF_OPTIONAL, szType)
		KVSCF_PARAMETERS_END

		QString szRet("\r!");
		if(szType == QLatin1String("nick"))
			szRet.append("n\r");
		else if(szType == QLatin1String("channel"))
			szRet.append("c\r");
		else if(szType == QLatin1String("host"))
			szRet.append("h\r");
		else
			szRet.append("u\r");

		szRet.append(szData);
		szRet.append("\r");
		KVSCF_pRetBuffer->setString(szRet);
		return true;
	}

	/*
		@doc: listtimers
		@title:
			$listtimers
		@type:
			function
		@short:
			Returns a list of the active timers
		@syntax:
			<array> $listtimers([<name:string>][,<flags:string>])
		@description:
			Returns a list of all the active timers.[br]
			If you specify a <name>, only timers matching that name are returned;
			You can use the [b]*[/b] and [b]?[/b] wildcards in <name>.[br]
			<flags> can be any combination of the characters [b]s[/b] and [b]r[/b][br]
			If the flag [b]r[/b] is specified then <name> is assumed to be a standard regular expression.
			If the flag [b]s[/b] is specified the matches are case sensitive.[br]

		@seealso:
			[cmd]timer[/cmd], [fnc]$isTimer[/fnc], [cmd]killtimer[/cmd], [cmd]listtimers[/cmd]
	*/

	KVSCF(listtimers)
	{
		QString szText, szFlags;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("text", KVS_PT_STRING, KVS_PF_OPTIONAL, szText)
		KVSCF_PARAMETER("flags", KVS_PT_STRING, KVS_PF_OPTIONAL, szFlags)
		KVSCF_PARAMETERS_END

		bool bCaseSensitive = szFlags.indexOf('s', 0, Qt::CaseInsensitive) != -1;
		bool bRegexp = szFlags.indexOf('r', 0, Qt::CaseInsensitive) != -1;
		bool bMatch = !szText.isEmpty();

		KviPointerHashTable<QString, KviKvsTimer> * pTimerDict = KviKvsTimerManager::instance()->timerDict();

		if(!pTimerDict)
			return true;

		KviKvsArray * a = new KviKvsArray();
		QRegExp re(szText, bCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive, bRegexp ? QRegExp::RegExp : QRegExp::Wildcard);
		KviPointerHashTableIterator<QString, KviKvsTimer> it(*pTimerDict);

		while(KviKvsTimer * pTimer = it.current())
		{
			if(bMatch && !re.exactMatch(pTimer->name()))
			{
				++it;
				continue;
			}

			a->append(new KviKvsVariant(pTimer->name()));
			++it;
		}

		KVSCF_pRetBuffer->setArray(a);
		return true;
	}
};
