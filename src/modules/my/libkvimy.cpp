//=============================================================================
//
//   File : libkvimy.cpp
//   Creation date : Mon Jul  1 02:46:49 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviModule.h"
#include "KviConsoleWindow.h"
#include "KviApplication.h"
#include "KviLocale.h"
#include "KviIrcLink.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviIrcConnectionServerInfo.h"
#include "KviIrcServer.h"

#include "Idle.h"
Idle * g_pIdle;

#define GET_KVS_CONSOLE                                                       \
	kvs_uint_t uiWnd;                                                         \
	KviConsoleWindow * wnd = nullptr;                                         \
	KVSM_PARAMETERS_BEGIN(c)                                                  \
	KVSM_PARAMETER("context_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uiWnd)         \
	KVSM_PARAMETERS_END(c)                                                    \
	if(!c->parameterList()->count())                                          \
	{                                                                         \
		if(c->window()->console())                                            \
			wnd = c->window()->console();                                     \
		else                                                                  \
			c->warning(__tr2qs("This window has no associated IRC context")); \
	}                                                                         \
	else                                                                      \
	{                                                                         \
		wnd = g_pApp->findConsole(uiWnd);                                     \
		if(!wnd)                                                              \
			c->warning(__tr2qs("No such IRC context (%d)"), uiWnd);           \
	}

/*
	@doc: my.nick
	@type:
		function
	@title:
		$my.nick
	@short:
		Returns the current nickname
	@syntax:
		<string> $my.nick([irc_context_id:uint])
	@description:
		Returns the nickname of the current IRC context.[br]
		If the IRC context is not connected then an empty string is returned.[br]
		If <irc_context_id> is specified this function returns acts as it was called
		in that irc_context.[br]
		Note that this function is different from [fnc]$me[/fnc] that will work also in a DCC CHAT.[br]
*/

static bool my_kvs_fnc_nick(KviKvsModuleFunctionCall * c)
{
	GET_KVS_CONSOLE
	if(wnd)
	{
		if(wnd->connection())
			c->returnValue()->setString(wnd->connection()->userInfo()->nickName());
	}
	return true;
}

/*
	@doc: my.startIdleTimer
	@type:
		command
	@title:
		my.startIdleTimer
	@short:
		Starts a global idle hook
	@syntax:
		my.startIdleTimer()
	@description:
		Starts a global idle hook. Allows to detect a global user idle time
*/

static bool my_kvs_cmd_startIdleTimer(KviKvsModuleCommandCall *)
{
	if(!g_pIdle)
		g_pIdle = new Idle();
	g_pIdle->start();
	return true;
}

/*
	@doc: my.stopIdleTimer
	@type:
		command
	@title:
		my.stopIdleTimer
	@short:
		Stops a global idle hook
	@syntax:
		my.stopIdleTimer()
	@description:
		Stops a global idle hook. Allows to detect a global user idle time
*/

static bool my_kvs_cmd_stopIdleTimer(KviKvsModuleCommandCall *)
{
	if(!g_pIdle)
		return true;
	g_pIdle->stop();
	//delete g_pIdle;
	return true;
}

/*
	@doc: my.globalIdle
	@type:
		function
	@title:
		$my.globalIdle
	@short:
		Returns the current user global idle time
	@syntax:
		<int> $my.globalIdle()
	@description:
		Returns the current user global idle time.
		You must execute [cmd]my.startIdleTimer[/cmd] before using this function.
*/

static bool my_kvs_fnc_globalIdle(KviKvsModuleFunctionCall * c)
{
	if(g_pIdle)
	{
		c->returnValue()->setInteger(g_pIdle->secondsIdle());
	}
	else
	{
		c->error(__tr2qs("Global idle timer must be started before using $my.globalIdle function"));
	}
	return true;
}

/*
	@doc: my.umode
	@type:
		function
	@title:
		$my.umode
	@short:
		Returns the current user mode flags
	@syntax:
		<string> $my.umode([irc_context_id:uint])
	@description:
		Returns the user mode flags of the current IRC context.[br]
		If the IRC context is not connected then an empty string is returned.[br]
		If <irc_context_id> is specified this function returns acts as it was called
		in that irc_context.[br]
*/

static bool my_kvs_fnc_umode(KviKvsModuleFunctionCall * c)
{
	GET_KVS_CONSOLE
	if(wnd)
	{
		if(wnd->connection())
			c->returnValue()->setString(wnd->connection()->userInfo()->userMode());
	}
	return true;
}

/*
	@doc: my.user
	@type:
		function
	@title:
		$my.user
	@short:
		Returns the current username
	@syntax:
		<string> $my.user([irc_context_id:uint])
	@description:
		Returns the username of the current IRC context.[br]
		If the IRC context is not connected then an empty string is returned.[br]
		If <irc_context_id> is specified this function returns acts as it was called
		in that irc_context.[br]
*/

static bool my_kvs_fnc_user(KviKvsModuleFunctionCall * c)
{
	GET_KVS_CONSOLE
	if(wnd)
	{
		if(wnd->connection())
			c->returnValue()->setString(wnd->connection()->userInfo()->userName());
	}
	return true;
}

/*
	@doc: my.host
	@type:
		function
	@title:
		$my.host
	@short:
		Returns the current hostname
	@syntax:
		<string> $my.host([irc_context_id:uint])
	@description:
		Returns the hostname of the current IRC context as known by the IRC server.[br]
		If the IRC context is not connected then an empty string is returned.[br]
		If <irc_context_id> is specified this function returns acts as it was called
		in that irc_context.[br]
*/

static bool my_kvs_fnc_host(KviKvsModuleFunctionCall * c)
{
	GET_KVS_CONSOLE
	if(wnd)
	{
		if(wnd->connection())
			c->returnValue()->setString(wnd->connection()->userInfo()->hostName());
	}
	return true;
}

/*
	@doc: my.ip
	@type:
		function
	@title:
		$my.ip
	@short:
		Returns the current IP address
	@syntax:
		<string> $my.ip([irc_context_id:uint])
	@description:
		Returns the IP address of the current IRC context as known by the IRC server.[br]
		The IP address is resolved as soon as the hostname is received from the server.
		If the hostname is masked or there is an error in the DNS lookup then
		you may get the real local host IP address as determined at connection startup.[br]
		If the IRC context is not connected then an empty string is returned.[br]
		If <irc_context_id> is specified this function returns acts as it was called
		in that irc_context.[br]
*/

static bool my_kvs_fnc_ip(KviKvsModuleFunctionCall * c)
{
	GET_KVS_CONSOLE
	if(wnd)
	{
		if(wnd->connection())
			c->returnValue()->setString(wnd->connection()->userInfo()->hostIp());
	}
	return true;
}

/*
	@doc: my.serverIsIPV6
	@type:
		function
	@title:
		$my.serverIsIPV6
	@short:
		Returns [b]1[/b] if the current server connection use IPv6.
	@syntax:
		<boolean> $my.serverIsIPV6([irc_context_id:uint])
	@description:
		Returns [b]1[/b] if the server connection use IPv6.[br]
		If the IRC context is not connected then an empty string is returned.[br]
		If <irc_context_id> is specified this function returns acts as it was called
		in that irc_context.[br]
*/

static bool my_kvs_fnc_serverIsIPV6(KviKvsModuleFunctionCall * c)
{

	GET_KVS_CONSOLE
	if(wnd)
	{
		if(wnd->connection())
			c->returnValue()->setBoolean(wnd->connection()->target()->server()->isIPv6());
	}
	return true;
}

/*
	@doc: my.serverIsSSL
	@type:
		function
	@title:
		$my.serverIsSSL
	@short:
		Returns [b]1[/b] if the current server connection uses SSL
	@syntax:
		<boolean> $my.serverIsSSL([irc_context_id:uint])
	@description:
		Returns [b]1[/b] if the server connection uses SSL.[br]
		If the IRC context is not connected then an empty string is returned.[br]
		If <irc_context_id> is specified this function returns acts as it was called
		in that irc_context.[br]
*/

static bool my_kvs_fnc_serverIsSSL(KviKvsModuleFunctionCall * c)
{

	GET_KVS_CONSOLE
	if(wnd)
	{
		if(wnd->connection())
			c->returnValue()->setBoolean(wnd->connection()->link()->socket()->usingSSL());
	}
	return true;
}

/*
	@doc: my.server
	@type:
		function
	@title:
		$my.server
	@short:
		Returns the current server name
	@syntax:
		<string> $my.server([irc_context_id:uint])
	@description:
		Returns the server name of the current IRC context.[br]
		If the IRC context is not connected then an empty string is returned.[br]
		If <irc_context_id> is specified this function returns acts as it was called
		in that irc_context.[br]
		Please note that this function returns the name of the server as reported
		by the server itself. Some servers report a bogus value for this field.
		You should take a look at [fnc]$context.serverIpAddress[/fnc] if you want a value that
		can be used to really reconnect to this server. If you want a value
		to manipulate the server entry via the serverDB functions then
		you probably need [fnc]$context.serverdbServerHostName[/fnc].
*/

static bool my_kvs_fnc_server(KviKvsModuleFunctionCall * c)
{
	GET_KVS_CONSOLE
	if(wnd)
	{
		if(wnd->connection())
			c->returnValue()->setString(wnd->connection()->currentServerName());
	}
	return true;
}

/*
	@doc: my.network
	@type:
		function
	@title:
		$my.network
	@short:
		Returns the current network name
	@syntax:
		<string> $my.network([irc_context_id:uint])
	@description:
		Returns the network name of the current IRC context.[br]
		If the IRC context is not connected then an empty string is returned.[br]
		If <irc_context_id> is specified this function returns acts as it was called
		in that irc_context.[br]
*/

static bool my_kvs_fnc_network(KviKvsModuleFunctionCall * c)
{
	GET_KVS_CONSOLE
	if(wnd)
	{
		if(wnd->connection())
			c->returnValue()->setString(wnd->currentNetworkName());
	}
	return true;
}

static bool my_module_init(KviModule * m)
{
	g_pIdle = nullptr;
	KVSM_REGISTER_FUNCTION(m, "nick", my_kvs_fnc_nick);
	KVSM_REGISTER_FUNCTION(m, "user", my_kvs_fnc_user);
	KVSM_REGISTER_FUNCTION(m, "host", my_kvs_fnc_host);
	KVSM_REGISTER_FUNCTION(m, "ip", my_kvs_fnc_ip);
	KVSM_REGISTER_FUNCTION(m, "server", my_kvs_fnc_server);
	KVSM_REGISTER_FUNCTION(m, "network", my_kvs_fnc_network);
	KVSM_REGISTER_FUNCTION(m, "umode", my_kvs_fnc_umode);
	KVSM_REGISTER_FUNCTION(m, "serverIsSSL", my_kvs_fnc_serverIsSSL);
	KVSM_REGISTER_FUNCTION(m, "serverIsIPV6", my_kvs_fnc_serverIsIPV6);
	KVSM_REGISTER_FUNCTION(m, "globalIdle", my_kvs_fnc_globalIdle);

	KVSM_REGISTER_SIMPLE_COMMAND(m, "stopIdleTimer", my_kvs_cmd_stopIdleTimer);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "startIdleTimer", my_kvs_cmd_startIdleTimer);
	return true;
}

static bool my_module_cleanup(KviModule *)
{
	if(g_pIdle)
		delete g_pIdle;
	g_pIdle = nullptr;
	return true;
}

static bool my_module_can_unload(KviModule *)
{
	return !g_pIdle;
}

KVIRC_MODULE(
    "My",    // module name
    "4.0.0", // module version
    "Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)"
    "	(C) 2005 Tonino Imbesi (grifisx at barmes dot net)"
    "	(C) 2005 Alessandro Carbone (elfonol at gmail dot com)", // author & (C)
    "Scripting irc-context related functions",
    my_module_init,
    my_module_can_unload,
    0,
    my_module_cleanup,
    0)
