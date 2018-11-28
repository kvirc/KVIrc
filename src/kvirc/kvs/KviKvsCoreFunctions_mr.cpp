//=============================================================================
//
//   File : KviKvsCoreFunctions_mr.cpp
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
#include "KviKvsArrayCast.h"
#include "KviKvsObjectController.h"
#include "KviLocale.h"
#include "KviIrcConnection.h"
#include "KviControlCodes.h"
#include "KviOptions.h"
#include "KviConsoleWindow.h"
#include "KviIrcUserDataBase.h"
#include "KviIrcMask.h"
#include "kvi_socket.h"
#include "KviApplication.h"
#include "KviQueryWindow.h"

#include <cstdlib> // rand & srand

namespace KviKvsCoreFunctions
{
	/*
		@doc: mask
		@type:
			function
		@title:
			$mask
		@short:
			Retrieves the host mask of a user
		@syntax:
			$mask[(<nickname>[,<mask_type>])]
		@description:
			Returns the specified type of mask for the user with <nickname>.[br]
			If the host or username are not known, the mask may contain less information
			than requested.[br]
			If the <nickname> is not given it is assumed to be the current nickname.[br]
			If <mask_type> is not given or is invalid, it is assumed to be 0.[br]
			Available mask types:[br]
			0 : nick!user@machine.host.top	[b](nick!user@XXX.XXX.XXX.XXX) (default)[/b][br]
			1 : nick!user@*.abc.host.top	[b](nick!user@XXX.XXX.XXX.*)[/b][br]
			2 : nick!user@*[br]
			3 : nick!*@machine.host.top	[b](nick!user@XXX.XXX.XXX.XXX)[/b][br]
			4 : nick!*@*.abc.host.top	[b](nick!user@XXX.XXX.XXX.*)[/b][br]
			5 : nick!*@*[br]
			6 : *!user@machine.host.top	[b](*!user@XXX.XXX.XXX.XX)[/b][br]
			7 : *!user@*.abc.host.top	[b](*!user@XXX.XXX.XXX.*)[/b][br]
			8 : *!user@*[br]
			9 : *!*@machine.host.top	[b](*!*@XXX.XXX.XXX.XXX)[/b][br]
			10: *!*@*.abc.host.top		[b](*!*@XXX.XXX.XXX.*)[/b][br]
			11: nick!*user@machine.host.top [b](nick!*user@XXX.XXX.XXX.XXX)[/b][br]
			12: nick!*user@*.abc.host.top	[b](nick!*user@XXX.XXX.XXX.*)[/b][br]
			13: nick!*user@*[br]
			14: *!*user@machine.host.top    [b](*!*user@XXX.XXX.XXX.XXX)[/b][br]
			15: *!*user@*.abc.host.top	[b](*!*user@XXX.XXX.XXX.*)[/b][br]
			16: *!*user@*[br]
			17: nick!~user@*.host.top	[b](nick!~user@XXX.XXX.*)[/b][br]
			18: nick!*@*.host.top		[b](nick!*@XXX.XXX.*)[/b][br]
			19: *!~user@*.host.top		[b](*!~user@XXX.XXX.*)[/b][br]
			20: nick!*user@*.host.top	[b](nick!*user@XXX.XXX.*)[/b][br]
			21: *!*user@*.host.top		[b](*!user@*XXX.XXX.*)[/b][br]
			22: nick!~user@*.host.top	[b](nick!~user@XXX.XXX.*)[/b][br]
			23: nick!*@*.host.top		[b](nick!*@XXX.XXX.*)[/b][br]
			24: *!~user@*.host.top		[b](*!~user@XXX.XXX.*)[/b][br]
			25: nick!*user@*.host.top	[b](nick!*user@XXX.XXX.*)[/b][br]
			26: *!*user@*.host.top		[b](*!user@*XXX.XXX.*)[/b][br]
			If some data is missing, these types may change:[br]
			For example, if the hostname is missing, the mask type 3 or 4 may be reduced to type 5.[br]
			If the user with <nickname> is not found in the current IRC context user database,
			an empty string is returned.[br]
			The masks 22-26 are the smart versions of the masks 17-21 that try take care of masked IP addresses
			in the form xxx.xxx.INVALID-TOP-MASK. If a masked IP address is found then
			the XXX.XXX.* or XXX.* host mask is returned instead of the (wrong) *.INVALID-TOP-MASK
		@examples:
		@seealso:
	*/

	KVSCF(mask)
	{
		QString szNick;
		kvs_uint_t maskType;

		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("nickname", KVS_PT_STRING, KVS_PF_OPTIONAL, szNick)
		KVSCF_PARAMETER("maskType", KVS_PT_UINT, KVS_PF_OPTIONAL, maskType)
		KVSCF_PARAMETERS_END

		if(maskType > 26)
			maskType = 0;

		KviConsoleWindow * c = KVSCF_pContext->window()->console();
		if(c && c->isConnected())
		{
			KviIrcUserEntry * e = KVSCF_pContext->window()->connection()->userDataBase()->find(szNick.isEmpty() ? KVSCF_pContext->window()->connection()->currentNickName() : szNick);
			if(e)
			{
				KviIrcMask u;
				u.setNick(szNick);
				u.setUsername(e->user());
				u.setHost(e->host());

				QString tmp;
				u.mask(tmp, (KviIrcMask::MaskType)maskType);
				KVSCF_pRetBuffer->setString(tmp);
				return true;
			}
		}
		KVSCF_pRetBuffer->setNothing();
		return true;
	}

	/*
		@doc: me
		@type:
			function
		@title:
			$me
		@short:
			Returns the current nickname
		@syntax:
			<string> $me[(<irc_context_id:uint>)]
		@description:
			Returns the current nickname used in the specified IRC context.[br]
			If no <irc_context_id> is specified, the current IRC context is used.[br]
			If you are not connected to a server, this function will return a null string.[br]
			If the current window is a DCC chat and no irc_context is specified,
			the local nickname is returned.[br]
			If the current window does not belong to any IRC context, no irc_context_id
			is specified, and the current window is not a DCC chat, a warning is printed.[br]
	*/

	KVSCF(me)
	{
		kvs_uint_t uCntx;

		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("irc_context_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uCntx)
		KVSCF_PARAMETERS_END

		KviConsoleWindow * cns = nullptr;

		if(KVSCF_pParams->count() > 0)
		{
			cns = g_pApp->findConsole(uCntx);
			if(cns)
			{
				if(cns->isConnected())
					KVSCF_pRetBuffer->setString(cns->connection()->currentNickName());
				else
					KVSCF_pRetBuffer->setNothing();
			}
			else
			{
				KVSCF_pRetBuffer->setNothing();
			}
		}
		else
		{
			cns = KVSCF_pContext->window()->console();
			if(cns)
			{
				if(cns->isConnected())
					KVSCF_pRetBuffer->setString(cns->connection()->currentNickName());
				else
					KVSCF_pRetBuffer->setNothing();
			}
			else
			{
				if(KVSCF_pContext->window()->type() == KviWindow::DccChat)
				{
					KVSCF_pRetBuffer->setString(KVSCF_pContext->window()->localNick());
				}
				else
				{
					KVSCF_pContext->warning(__tr2qs_ctx("This window has no associated IRC context and is not a DCC chat", "kvs"));
					KVSCF_pRetBuffer->setNothing();
				}
			}
		}
		return true;
	}

	/*
		@doc: msgtype
		@type:
			function
		@title:
			$msgtype
		@short:
			Returns the ID of a message type color set
		@syntax:
			$msgtype(<message type color set name>)
		@description:
			Returns the ID of a message type color set used by the [cmd]echo[/cmd] command.
			You can take a look at the options dialog section related to the
			message output to see the list of available message type names.
		@examples:
			[example]
				[cmd]echo[/cmd] -i=$msgtype(Highlight) Highlighted text!
			[/example]
		@seealso:
			[cmd]echo[/cmd]
	*/

	KVSCF(msgtype)
	{
		QString szName;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("msg_type_color_set_name", KVS_PT_NONEMPTYSTRING, 0, szName)
		KVSCF_PARAMETERS_END

		QString tmp = "msgtype";
		tmp += szName;
		for(int i = 0; i < KVI_NUM_MSGTYPE_OPTIONS; i++)
		{
			if(KviQString::equalCI(tmp, g_msgtypeOptionsTable[i].name))
			{
				KVSCF_pRetBuffer->setInteger(i);
				return true;
			}
		}
		KVSCF_pRetBuffer->setInteger(0);
		return true;
	}

	/*
		@doc: new
		@type:
			function
		@title:
			$new
		@short:
			Creates a new object
		@syntax:
			$new(<className>,[<parentHandle> [,<name>[,<param>[,<param>[...]]]])
		@description:
			Creates a new instance of the object <class> with
			the parent object <parent_id> and the specified <name>.[br]
			<name> and <parent_id> are optional: if not specified, <name>
			is assumed to be an empty string and <parent_id> default to 0 (parentless object).[br]
			This function accepts a list of optional parameters that will be passed to the <class>'s constructor.
			Please see the [doc:objects]objects documentation[/doc] for more information.[br]
		@examples:
			[example]
				%myobj = $new(widget,0,pippo)
			[/example]
		@seealso:
			[doc:objects]Objects documentation[/doc], [cmd]delete[/cmd]
	*/

	KVSCF(newCKEYWORDWORKAROUND)
	{
		// prologue: parameter handling
		QString szClassName;
		QString szName;
		kvs_hobject_t hParent = nullptr;
		KviKvsVariantList vList;

		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("className", KVS_PT_NONEMPTYSTRING, 0, szClassName)
		KVSCF_PARAMETER("parentHandle", KVS_PT_HOBJECT, KVS_PF_OPTIONAL, hParent)
		KVSCF_PARAMETER("name", KVS_PT_STRING, KVS_PF_OPTIONAL, szName)
		KVSCF_PARAMETER("parameterList", KVS_PT_VARIANTLIST, KVS_PF_OPTIONAL, vList)
		KVSCF_PARAMETERS_END

		KviKvsObjectClass * pClass = KviKvsKernel::instance()->objectController()->lookupClass(szClassName);
		if(!pClass)
		{
			KVSCF_pContext->error(__tr2qs_ctx("Class '%Q' is not defined", "kvs"), &szClassName);
			return false;
		}

		KviKvsObject * pParent = nullptr;
		if(hParent != (kvs_hobject_t) nullptr)
		{
			pParent = KviKvsKernel::instance()->objectController()->lookupObject(hParent);
			if(!pParent)
			{
				KVSCF_pContext->error(__tr2qs_ctx("The specified parent object does not exist", "kvs"));
				return false;
			}
		}

		KviKvsObject * pObject = pClass->allocateInstance(pParent, szName, KVSCF_pContext, &vList);

		// epilogue: set the return value
		KVSCF_pRetBuffer->setHObject(pObject ? pObject->handle() : (kvs_hobject_t) nullptr);
		return true;
	}

	/*
		@doc: nothing
		@type:
			function
		@title:
			$nothing
		@short:
			Evaluates to an empty variable
		@syntax:
			<nothing> $nothing
		@description:
			Evaluates to an empty variable. This is a special value somewhat similar
			to the [fnc]$null[/fnc] object: it is used to denote absence
			of information. For example, a function might decide to return
			a meaningful value when the input parameters are correct and
			$nothing when the input parameters are wrong.
		@seealso:
			[fnc]$null[/fnc]
	*/

	KVSCF(nothing)
	{
		KVSCF_pRetBuffer->setNothing();
		return true;
	}

	/*
		@doc: null
		@type:
			function
		@title:
			$null
		@short:
			Evaluates to the null object handle.
		@syntax:
			<hobject> $null
		@description:
			Evaluates to the null object handle. This is a special
			object handle that is returned by some functions.
			Please note that writing $null is semantically equivalent to writing 0
			or even [fnc]$nothing[/fnc] in place of the handle, but $null increases code readability.
		@seealso:
			[fnc]$new[/fnc], [fnc]$nothing[/fnc]
	*/

	KVSCF(nullCKEYWORDWORKAROUND)
	{
		KVSCF_pRetBuffer->setHObject(nullptr);
		return true;
	}

	/*
		@doc: o
		@type:
			function
		@title:
			$o
		@short:
			Returns the RESET mIRC control character
		@syntax:
			<string> $o
		@description:
			Returns the RESET mIRC control character (Ctrl+O).[br]
		@seealso:
			[fnc]$k[/fnc], [fnc]$b[/fnc], [fnc]$i[/fnc], [fnc]$u[/fnc], [fnc]$r[/fnc]
	*/

	KVSCF(o)
	{
		KVSCF_pRetBuffer->setString(QString(QChar(KviControlCodes::Reset)));
		return true;
	}

	/*
		@doc: option
		@type:
			function
		@title:
			$option
		@short:
			Returns the value of an option
		@syntax:
			<string> $option(<optionName:string>)
		@description:
			Returns the current value of the internal option named <optionName>.
			See the [cmd]option[/cmd] command documentation for more info about options.
		@examples:
			[example]
				[cmd]echo[/cmd] $option(fontIrcView)
			[/example]
		@seealso:
			[cmd]option[/cmd],
	*/

	KVSCF(option)
	{
		QString szOpt;
		// FIXME: This should return a variant in general
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("optionName", KVS_PT_NONEMPTYSTRING, 0, szOpt)
		KVSCF_PARAMETERS_END
		QString tmp;
		if(g_pApp->getOptionString(szOpt, tmp))
			KVSCF_pRetBuffer->setString(tmp);
		else
		{
			KVSCF_pContext->warning(__tr2qs_ctx("There is no option named '%Q'", "kvs"), &szOpt);
			KVSCF_pRetBuffer->setNothing();
		}
		return true;
	}

	/*
		@doc: query
		@type:
			function
		@title:
			$query
		@short:
			Retrieves the window ID of a specified query
		@syntax:
			$query[(<target>[,<irc context id>])]
		@description:
			Returns the [b]window ID[/b] of the query that has <target>
			in the list of targets and is bound to the connection specified by
			<irc context id>[br][br]
			If no window matches the specified target or context, and invalid
			window ID is returned (0).[br]
			If no <irc context id> is specified, this function looks for
			the query in the current connection context (if any).[br]
			If no <target> is specified, this function returns the current
			query window ID, if executed in a query, and [b]0[/b] otherwise.[br]
		@seealso:
			[fnc]$window[/fnc],
			[fnc]$channel[/fnc],
			[fnc]$console[/fnc]
			[doc:window_naming_conventions]Window naming conventions[/doc]
	*/

	KVSCF(query)
	{
		QString szName;
		kvs_uint_t uContextId;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("query_name", KVS_PT_NONEMPTYSTRING, KVS_PF_OPTIONAL, szName)
		KVSCF_PARAMETER("context_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uContextId)
		KVSCF_PARAMETERS_END

		KviWindow * wnd = nullptr;
		if(KVSCF_pParams->count() > 1)
		{
			KviConsoleWindow * cns = g_pApp->findConsole(uContextId);
			if(cns && cns->connection())
				wnd = cns->connection()->findQuery(szName);
			else if (!cns)
				KVSCF_pContext->warning(__tr2qs_ctx("No such IRC context (%u)", "kvs"), uContextId);
		}
		else if (KVSCF_pParams->count() == 1)
		{
			if(KVSCF_pContext->window()->connection())
				wnd = KVSCF_pContext->window()->connection()->findQuery(szName);
			else
			{
				if(!KVSCF_pContext->window()->console())
					KVSCF_pContext->warning(__tr2qs_ctx("This window is not associated to an IRC context", "kvs"));
			}
		}
		else
		{
			if(KVSCF_pContext->window()->type() == KviWindow::Query)
				wnd = KVSCF_pContext->window();
		}

		KVSCF_pRetBuffer->setInteger((kvs_int_t)(wnd ? wnd->numericId() : 0));
		return true;
	}

	/*
		@doc: r
		@type:
			function
		@title:
			$r
		@short:
			Returns the REVERSE mIRC control character
		@syntax:
			<string> $r
		@description:
			Returns the REVERSE mIRC control character (Ctrl+R).[br]
		@seealso:
			[fnc]$k[/fnc], [fnc]$b[/fnc], [fnc]$i[/fnc], [fnc]$u[/fnc], [fnc]$o[/fnc]
	*/

	KVSCF(r)
	{
		KVSCF_pRetBuffer->setString(QString(QChar(KviControlCodes::Reverse)));
		return true;
	}

	/*
		@doc: rand
		@type:
			function
		@title:
			$rand
		@short:
			Generates a random number
		@syntax:
			$rand(<max>)
		@description:
			Returns a random integer number from 0 to max inclusive.
			You can repeat sequences of random numbers by calling [cmd]srand[/cmd]
			with the same seed value. If [cmd]srand[/cmd] has not been called
			$rand is automatically seeded with value of 1.
			If no <max> is specified, this function returns an integer between
			0 and RAND_MAX that is system dependent.
		@seealso:
			[cmd]srand[/cmd]
	*/

	KVSCF(rand)
	{
		// prologue: parameter handling
		kvs_uint_t uMax;

		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("max", KVS_PT_UINT, KVS_PF_OPTIONAL, uMax)
		KVSCF_PARAMETERS_END
		if(KVSCF_pParams->count() > 0)
			KVSCF_pRetBuffer->setInteger(::rand() % (uMax + 1));
		else
			KVSCF_pRetBuffer->setInteger(::rand());
		return true;
	}

	/*
		@doc: real
		@type:
			function
		@title:
			$real
		@short:
			Casts a variable to a real
		@syntax:
			<real> $real(<data:variant>)
		@description:
			Forces <data> to be a [i]real[/i] data type with the following
			semantics:
			[ul]
				[li]If <data> is a real, <data> itself is returned.[/li]
				[li]If <data> is an integer, <data> itself is returned, in its real form.[/li]
				[li]If <data> is a boolean, its numeric value is returned (either 1.0 or 0.0).[/li]
				[li]If <data> is an array, the count of its items is returned.[/li]
				[li]If <data> is a hash, the count of its items is returned.[/li]
				[li]If <data> is a string, its length is returned.[/li]
				[li]If <data> is an object, 0.0 is returned if the reference is null (invalid) and 1.0 otherwise[/li]
			[/ul]
			This function is similar to the C (double) cast.[br]
			Note that since KVIrc does most of the casting work automatically
			you shouldn't need to use this function.
		@seealso:
			[fnc]$int[/fnc]
	*/

	KVSCF(real)
	{
		KviKvsVariant * v;
		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("data", KVS_PT_VARIANT, 0, v)
		KVSCF_PARAMETERS_END

		kvs_real_t dVal;
		if(v->asReal(dVal))
			KVSCF_pRetBuffer->setReal(dVal);
		else
		{
			kvs_int_t iVal;
			v->castToInteger(iVal);
			KVSCF_pRetBuffer->setReal((double)iVal);
		}
		return true;
	}

	/*
		@doc: realname
		@type:
			function
		@title:
			$realname
		@short:
			Returns the realname of the specified user
		@syntax:
			<string> $realname(<nickname:string>)
		@description:
			Returns the realname of the specified IRC user IF it is known.[br]
			The realname is known if [fnc]$isWellKnown[/fnc] returns 1.[br]
			The realname is generally known if the user is on a channel with you
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
			[fnc]$isWellKnown[/fnc], [fnc]$hostname[/fnc], [fnc]$username[/fnc], [cmd]awhois[/cmd]
	*/

	KVSCF(realname)
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
				KVSCF_pRetBuffer->setString(e->realName());
				return true;
			}
		}

		KVSCF_pRetBuffer->setNothing();
		return true;
	}

	/*
		@doc: receivedBytes
		@type:
			function
		@title:
			$receivedBytes
		@short:
			Returns total received bytes
		@syntax:
			<uint> $receivedBytes()
		@description:
			Returns total received bytes
		@seealso:
			[fnc]$sentBytes[/fnc]
	*/

	KVSCF(receivedBytes)
	{
		KVSCF_pRetBuffer->setInteger(g_uIncomingTraffic);
		return true;
	}

	/*
		@doc: rsort
		@type:
			function
		@title:
			$rsort
		@short:
			Sorts an array in reverse order
		@syntax:
			<array> $rsort(<data:array>)
		@description:
			Sorts an array in descending order.
		@seealso:
			[fnc]$sort[/fnc]
	*/

	KVSCF(rsort)
	{
		KviKvsArrayCast a;

		KVSCF_PARAMETERS_BEGIN
		KVSCF_PARAMETER("data", KVS_PT_ARRAYCAST, 0, a)
		KVSCF_PARAMETERS_END

		if(a.array())
		{
			KviKvsArray * arry = new KviKvsArray(*(a.array()));
			arry->rsort();
			KVSCF_pRetBuffer->setArray(arry);
		}
		else
		{
			KVSCF_pRetBuffer->setArray(new KviKvsArray());
		}
		return true;
	}
};
