//=============================================================================
//
//   File : libkvicontext.cpp
//   Creation date : Wed Jan 02 2007 03:04:12 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007-2008 Szymon Stefanek (pragma at kvirc dot net)
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

//#warning: FIXME: Incomplete documentation ('example', etc)

#include "kvi_module.h"
#include "kvi_locale.h"
#include "kvi_qstring.h"
#include "kvi_window.h"
#include "kvi_frame.h"
#include "kvi_console.h"
#include "kvi_app.h"
#include "kvi_ircserver.h"
#include "kvi_network.h"
#include "kvi_irccontext.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnectionserverinfo.h"
#include "kvi_ircconnectionuserinfo.h"
#include "kvi_ircconnectiontarget.h"

#define GET_CONSOLE_FROM_STANDARD_PARAMS \
	kvs_uint_t iContextId; \
	KVSM_PARAMETERS_BEGIN(c) \
		KVSM_PARAMETER("irc_context_id",KVS_PT_UINT,KVS_PF_OPTIONAL,iContextId) \
	KVSM_PARAMETERS_END(c) \
	KviConsole * pConsole = NULL; \
	if(c->parameterCount() > 0) \
		pConsole = g_pApp->findConsole(iContextId); \
	else \
		pConsole = c->window()->console();

#define GET_CONNECTION_FROM_STANDARD_PARAMS \
	GET_CONSOLE_FROM_STANDARD_PARAMS \
	KviIrcConnection * pConnection = NULL; \
	if(pConsole) \
		pConnection = pConsole->context()->connection();

#define STANDARD_IRC_CONNECTION_TARGET_PARAMETER(_fncName,_setCall) \
	static bool _fncName(KviKvsModuleFunctionCall * c) \
	{ \
		GET_CONNECTION_FROM_STANDARD_PARAMS \
		if(pConnection) \
		{ \
			if(pConnection->target()) \
			{ \
				_setCall; \
				return true; \
			} \
		} \
		c->returnValue()->setNothing(); \
		return true; \
	}

#define STANDARD_SERVERINFO_TARGET_PARAMETER(_fncName,_setCall) \
	static bool _fncName(KviKvsModuleFunctionCall * c) \
	{ \
		GET_CONNECTION_FROM_STANDARD_PARAMS \
		if(pConnection) \
		{ \
			if(pConnection->serverInfo()) \
			{ \
				_setCall; \
				return true; \
			} \
		} \
		c->returnValue()->setNothing(); \
		return true; \
	}

/*
	@doc: context.networkName
	@type:
		function
	@title:
		$context.networkName
	@short:
		Returns the IRC network name of an IRC context
	@syntax:
		<string> $context.networkName
		<string> $context.networkName(<irc_context_id:uint>)
	@description:
		Returns the name of the network for the specified IRC context.
		If no irc_context_id is specified then the current irc_context is used.
		If the irc_context_id specification is not valid then this function
		returns nothing. If the specified IRC context is not currently connected
		then this function returns nothing.
	@seealso:
		[fnc]$context.serverHostName[/fnc]
*/

STANDARD_IRC_CONNECTION_TARGET_PARAMETER(
		context_kvs_fnc_networkName,
		c->returnValue()->setString(pConnection->target()->network()->name())
	)


/*
	@doc: context.serverHostName
	@type:
		function
	@title:
		$context.serverHostName
	@short:
		Returns the IRC server name of an IRC context
	@syntax:
		<string> $context.serverHostName
		<string> $context.serverHostName(<irc_context_id:uint>)
	@description:
		Returns the host name of the IRC server that was used to perform
		the connection in the specified irc context.
		If no irc_context_id is specified then the current irc_context is used.
		If the irc_context_id specification is not valid then this function
		returns nothing. If the specified IRC context is not currently connected
		then this function returns nothing.
		If the returned value is non empty then it will always be a valid
		DNS hostname that can be used to perform a real connection.
		Please note that this is different from $my.server() which might
		return an invalid DNS entry.
	@seealso:
		[fnc]$context.serverPort[/fnc],
		[fnc]$context.serverIpAddress[/fnc],
		[fnc]$context.serverPassword[/fnc]
*/

STANDARD_IRC_CONNECTION_TARGET_PARAMETER(
		context_kvs_fnc_serverHostName,
		c->returnValue()->setString(pConnection->target()->server()->hostName())
	)

/*
	@doc: context.serverIpAddress
	@type:
		function
	@title:
		$context.serverIpAddress
	@short:
		Returns the IRC server ip address of an IRC context
	@syntax:
		<string> $context.serverIpAddress
		<string> $context.serverIpAddress(<irc_context_id:uint>)
	@description:
		Returns the ip address of the IRC server for the specified irc context.
		If no irc_context_id is specified then the current irc_context is used.
		If the irc_context_id specification is not valid then this function
		returns nothing. If the specified IRC context is not currently connected
		then this function returns nothing.
	@seealso:
		[fnc]$context.serverPort[/fnc],
		[fnc]$context.serverHostName[/fnc],
		[fnc]$context.serverPassword[/fnc]
*/

STANDARD_IRC_CONNECTION_TARGET_PARAMETER(
		context_kvs_fnc_serverIpAddress,
		c->returnValue()->setString(pConnection->target()->server()->ip())
	)

/*
	@doc: context.serverIsIPV6
	@type:
		function
	@title:
		$context.serverIsIPV6
	@short:
		Returns the IPV6 state of an IRC context
	@syntax:
		<string> $context.serverIsIPV6
		<string> $context.serverIsIPV6(<irc_context_id:uint>)
	@description:
		Returns true if the current irc context connection runs over IPV6.
		If no irc_context_id is specified then the current irc_context is used.
		If the irc_context_id specification is not valid then this function
		returns nothing (that evaluates to false). If the specified IRC context
		is not currently connected then this function returns nothing (that
		evaluates to false).
	@seealso:
		[fnc]$context.serverPort[/fnc],
		[fnc]$context.serverHostName[/fnc],
		[fnc]$context.serverPassword[/fnc]
*/

STANDARD_IRC_CONNECTION_TARGET_PARAMETER(
		context_kvs_fnc_serverIsIPV6,
		c->returnValue()->setBoolean(pConnection->target()->server()->isIPv6())
	)

/*
	@doc: context.serverIsSSL
	@type:
		function
	@title:
		$context.serverIsSSL
	@short:
		Returns the SSL state of an IRC context
	@syntax:
		<string> $context.serverIsSSL
		<string> $context.serverIsSSL(<irc_context_id:uint>)
	@description:
		Returns true if the current irc context connection runs over SSL.
		If no irc_context_id is specified then the current irc_context is used.
		If the irc_context_id specification is not valid then this function
		returns nothing (that evaluates to false). If the specified IRC context
		is not currently connected then this function returns nothing (that
		evaluates to false).
	@seealso:
		[fnc]$context.serverPort[/fnc],
		[fnc]$context.serverHostName[/fnc],
		[fnc]$context.serverPassword[/fnc]
*/

STANDARD_IRC_CONNECTION_TARGET_PARAMETER(
		context_kvs_fnc_serverIsSSL,
		c->returnValue()->setBoolean(pConnection->target()->server()->useSSL())
	)

/*
	@doc: context.serverSoftware
	@type:
		function
	@title:
		$context.serverSoftware
	@short:
		Returns the software running on the irc server, if recognized
	@syntax:
		<string> $context.serverSoftware
		<string> $context.serverSoftware(<irc_context_id:uint>)
	@description:
		Returns the software running on the irc server, if it has been
		recognized at connection time.
		If no irc_context_id is specified then the current irc_context is used.
		If the irc_context_id specification is not valid then this function
		returns nothing (that evaluates to false). If the specified IRC context
		is not currently connected then this function returns nothing (that
		evaluates to false).
	@seealso:
		[fnc]$context.serverPort[/fnc],
		[fnc]$context.serverHostName[/fnc],
		[fnc]$context.serverPassword[/fnc]
*/

STANDARD_SERVERINFO_TARGET_PARAMETER(
		context_kvs_fnc_serverSoftware,
		c->returnValue()->setString(pConnection->serverInfo()->software())
	)

/*
	@doc: context.serverPassword
	@type:
		function
	@title:
		$context.serverPassword
	@short:
		Returns the password used to login to the server of an IRC context
	@syntax:
		<string> $context.serverPassword
		<string> $context.serverPassword(<irc_context_id:uint>)
	@description:
		Returns the password used to login to the IRC server for the specified irc context.
		If no irc_context_id is specified then the current irc_context is used.
		If the irc_context_id specification is not valid then this function
		returns nothing. If the specified IRC context is not currently connected
		then this function returns nothing.
	@seealso:
		[fnc]$context.serverHostName[/fnc],
		[fnc]$context.serverIpAddress[/fnc],
		[fnc]$context.serverPort[/fnc]
*/

STANDARD_IRC_CONNECTION_TARGET_PARAMETER(
		context_kvs_fnc_serverPassword,
		c->returnValue()->setString(pConnection->target()->server()->password())
	)


/*
	@doc: context.serverPort
	@type:
		function
	@title:
		$context.serverPort
	@short:
		Returns the port of the IRC server of an IRC context
	@syntax:
		<uint> $context.serverPort
		<uint> $context.serverPort(<irc_context_id:uint>)
	@description:
		Returns the port of the IRC server for the specified irc context.
		If no irc_context_id is specified then the current irc_context is used.
		If the irc_context_id specification is not valid then this function
		returns nothing. If the specified IRC context is not currently connected
		then this function returns nothing.
	@seealso:
		[fnc]$context.serverHostName[/fnc],
		[fnc]$context.serverIpAddress[/fnc]
*/

STANDARD_IRC_CONNECTION_TARGET_PARAMETER(
		context_kvs_fnc_serverPort,
		c->returnValue()->setInteger(pConnection->target()->server()->port())
	)


/*
	@doc: context.state
	@type:
		function
	@title:
		$context.state
	@short:
		Returns the state of an IRC context
	@syntax:
		<string> $context.state
		<string> $context.state(<irc_context_id:uint>)
	@description:
		Returns a string describing the state of the specified irc context.
		The string will be either "idle","connecting","loggingin" or "connected".
		If no irc_context_id is specified then the current irc_context is used.
		If the irc_context_id specification is not valid then this function
		returns nothing.
	@seealso:
		[fnc]$context.serverHostName[/fnc],
		[fnc]$context.serverIpAddress[/fnc]
*/

static bool context_kvs_fnc_state(KviKvsModuleFunctionCall * c)
{
	GET_CONSOLE_FROM_STANDARD_PARAMS

	if(pConsole)
	{
		switch(pConsole->context()->state())
		{
			case KviIrcContext::Idle:
				c->returnValue()->setString(QString("idle"));
			break;
			case KviIrcContext::Connecting:
				c->returnValue()->setString(QString("connecting"));
			break;
			case KviIrcContext::LoggingIn:
				c->returnValue()->setString(QString("loggingin"));
			break;
			case KviIrcContext::Connected:
				c->returnValue()->setString(QString("connected"));
			break;
			default:
				c->returnValue()->setString(QString("unknown"));
			break;
		}
		return true;
	}

	c->returnValue()->setNothing();
	return true;
}


/*
	@doc: context.list
	@type:
		function
	@title:
		$context.list
	@short:
		Returns a list of existing IRC contexts
	@syntax:
		<array> $context.list
	@description:
		Returns the array of currently existing IRC context identifiers.
	@seealso:
	@examples:
		Print the names of the currently connected servers
		[example]
			foreach(%ic,$context.list)
				echo "IRC Context" %ic ": " $context.serverHostName
		[/example]
*/

static bool context_kvs_fnc_list(KviKvsModuleFunctionCall * c)
{
	KviKvsArray * pArray = new KviKvsArray();

	KviPointerList<KviWindow> * pWinList = g_pFrame->windowList();
	int idx = 0;
	for(KviWindow * pWnd = pWinList->first();pWnd;pWnd = pWinList->next())
	{
		if(pWnd->type() == KVI_WINDOW_TYPE_CONSOLE)
		{
			pArray->set(idx,new KviKvsVariant((kvs_int_t)((KviConsole *)pWnd)->context()->id()));
			idx++;
		}
	}

	c->returnValue()->setArray(pArray);
	return true;
}

static bool context_module_init(KviModule * m)
{
	KVSM_REGISTER_FUNCTION(m,"serverHostName",context_kvs_fnc_serverHostName);
	KVSM_REGISTER_FUNCTION(m,"serverIpAddress",context_kvs_fnc_serverIpAddress);
	KVSM_REGISTER_FUNCTION(m,"serverPort",context_kvs_fnc_serverPort);
	KVSM_REGISTER_FUNCTION(m,"serverIsIPV6",context_kvs_fnc_serverIsIPV6);
	KVSM_REGISTER_FUNCTION(m,"serverIsSSL",context_kvs_fnc_serverIsSSL);
	KVSM_REGISTER_FUNCTION(m,"serverPassword",context_kvs_fnc_serverPassword);
	KVSM_REGISTER_FUNCTION(m,"networkName",context_kvs_fnc_networkName);
	KVSM_REGISTER_FUNCTION(m,"state",context_kvs_fnc_state);
	KVSM_REGISTER_FUNCTION(m,"list",context_kvs_fnc_list);
	KVSM_REGISTER_FUNCTION(m,"serverSoftware",context_kvs_fnc_serverSoftware);

	return true;
}

static bool context_module_cleanup(KviModule *)
{
	return true;
}

KVIRC_MODULE(
	"context",
	"4.0.0",
	"Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)",
	"Irc Context Related Functions",
	context_module_init,
	0,
	0,
	context_module_cleanup,
	0
)
