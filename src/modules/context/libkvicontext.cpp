//=============================================================================
//
//   File : libkvicontext.cpp
//   Creation date : Wed Jan 02 2007 03:04:12 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2007-2010 Szymon Stefanek (pragma at kvirc dot net)
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

//#warning: FIXME: Incomplete documentation ('example', etc)

#include "KviModule.h"
#include "KviLocale.h"
#include "KviQString.h"
#include "KviWindow.h"
#include "KviMainWindow.h"
#include "KviConsoleWindow.h"
#include "KviApplication.h"
#include "KviIrcServer.h"
#include "KviIrcNetwork.h"
#include "KviIrcContext.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionServerInfo.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviIrcConnectionTarget.h"
#include "KviIrcConnectionStatistics.h"
#include "KviIrcLink.h"
#include "KviIrcSocket.h"

#ifdef COMPILE_SSL_SUPPORT
#include "KviSSLMaster.h"
#endif

#define GET_CONSOLE_FROM_STANDARD_PARAMS                                       \
	kvs_uint_t iContextId;                                                     \
	KVSM_PARAMETERS_BEGIN(c)                                                   \
	KVSM_PARAMETER("irc_context_id", KVS_PT_UINT, KVS_PF_OPTIONAL, iContextId) \
	KVSM_PARAMETERS_END(c)                                                     \
	KviConsoleWindow * pConsole = nullptr;                                     \
	if(c->parameterCount() > 0)                                                \
		pConsole = g_pApp->findConsole(iContextId);                            \
	else                                                                       \
		pConsole = c->window()->console();

#define GET_CONNECTION_FROM_STANDARD_PARAMS \
	GET_CONSOLE_FROM_STANDARD_PARAMS          \
	KviIrcConnection * pConnection = nullptr; \
	if(pConsole)                              \
		pConnection = pConsole->context()->connection();

#define STANDARD_IRC_CONNECTION_TARGET_PARAMETER(_fncName, _setCall) \
	static bool _fncName(KviKvsModuleFunctionCall * c)               \
	{                                                                \
		GET_CONNECTION_FROM_STANDARD_PARAMS                          \
		if(pConnection)                                              \
		{                                                            \
			if(pConnection->target())                                \
			{                                                        \
				_setCall;                                            \
				return true;                                         \
			}                                                        \
		}                                                            \
		c->returnValue()->setNothing();                              \
		return true;                                                 \
	}

#define STANDARD_SERVERINFO_TARGET_PARAMETER(_fncName, _setCall) \
	static bool _fncName(KviKvsModuleFunctionCall * c)           \
	{                                                            \
		GET_CONNECTION_FROM_STANDARD_PARAMS                      \
		if(pConnection)                                          \
		{                                                        \
			if(pConnection->serverInfo())                        \
			{                                                    \
				_setCall;                                        \
				return true;                                     \
			}                                                    \
		}                                                        \
		c->returnValue()->setNothing();                          \
		return true;                                             \
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
		The network name is the one that the server reports or
		the one stored in the server database if the server reports no
		network name. This is equivalent to [fnc]$my.network[/fnc].
	@seealso:
		[fnc]$context.serverHostName[/fnc]
		[fnc]$context.serverdbNetworkName[/fnc]
*/

STANDARD_IRC_CONNECTION_TARGET_PARAMETER(
    context_kvs_fnc_networkName,
    c->returnValue()->setString(pConnection->currentNetworkName()))

/*
	@doc: context.serverdbNetworkName
	@type:
		function
	@title:
		$context.serverdbNetworkName
	@short:
		Returns the original IRC network name of an IRC context
	@syntax:
		<string> $context.serverdbNetworkName
		<string> $context.serverdbNetworkName(<irc_context_id:uint>)
	@description:
		Returns the name of the network for the specified IRC context.
		If no irc_context_id is specified then the current irc_context is used.
		If the irc_context_id specification is not valid then this function
		returns nothing. If the specified IRC context is not currently connected
		then this function returns nothing.
		The returned network name is the one that has been specified
		in the server database and may not correspond to the real network
		name. However, this is the network name you want to use if
		you want to manipulate the serverDB entries related to the current
		connection.
	@seealso:
		[fnc]$context.serverHostName[/fnc]
		[fnc]$context.networkName[/fnc]
*/

STANDARD_IRC_CONNECTION_TARGET_PARAMETER(
    context_kvs_fnc_serverdbNetworkName,
    c->returnValue()->setString(pConnection->target()->network()->name()))

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
		the connection in the specified IRC context.
		If no irc_context_id is specified then the current irc_context is used.
		If the irc_context_id specification is not valid then this function
		returns nothing. If the specified IRC context is not currently connected
		then this function returns nothing.
		Please note that this function returns the name of the server as reported
		by the server itself. Some servers report a bogus value for this field.
		You should take a look at [fnc]$context.serverIpAddress[/fnc] if you want a value that
		can be used to really reconnect to this server. If you want a value
		to manipulate the server entry via the serverDB functions then
		you probably need [fnc]$context.serverdbServerHostName[/fnc].
	@seealso:
		[fnc]$context.serverdbServerHostName[/fnc]
		[fnc]$context.serverPort[/fnc],
		[fnc]$context.serverIpAddress[/fnc],
		[fnc]$context.serverPassword[/fnc]
*/

STANDARD_IRC_CONNECTION_TARGET_PARAMETER(
    context_kvs_fnc_serverHostName,
    c->returnValue()->setString(pConnection->currentServerName()))

/*
	@doc: context.serverdbServerHostName
	@type:
		function
	@title:
		$context.serverdbServerHostName
	@short:
		Returns the original IRC server name of an IRC context
	@syntax:
		<string> $context.serverdbServerHostName
		<string> $context.serverdbServerHostName(<irc_context_id:uint>)
	@description:
		Returns the host name of the IRC server that was used to perform
		the connection in the specified IRC context.
		If no irc_context_id is specified then the current irc_context is used.
		If the irc_context_id specification is not valid then this function
		returns nothing. If the specified IRC context is not currently connected
		then this function returns nothing.
		The returned server name is the one that has been specified
		in the server database and may refer to a round-robin DNS entry.
		However, this is the server name you want to use if you want to manipulate
		the serverDB entries related to the current connection.
	@seealso:
		[fnc]$context.serverHostName[/fnc],
		[fnc]$context.serverPort[/fnc],
		[fnc]$context.serverIpAddress[/fnc],
		[fnc]$context.serverPassword[/fnc]
*/

STANDARD_IRC_CONNECTION_TARGET_PARAMETER(
    context_kvs_fnc_serverdbServerHostName,
    c->returnValue()->setString(pConnection->target()->server()->hostName()))

/*
	@doc: context.serverIpAddress
	@type:
		function
	@title:
		$context.serverIpAddress
	@short:
		Returns the IRC server IP address of an IRC context
	@syntax:
		<string> $context.serverIpAddress
		<string> $context.serverIpAddress(<irc_context_id:uint>)
	@description:
		Returns the IP address of the IRC server for the specified IRC context.
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
    c->returnValue()->setString(pConnection->target()->server()->ip()))

/*
	@doc: context.serverIsIPV6
	@type:
		function
	@title:
		$context.serverIsIPV6
	@short:
		Returns the IPv6 state of an IRC context
	@syntax:
		<string> $context.serverIsIPV6
		<string> $context.serverIsIPV6(<irc_context_id:uint>)
	@description:
		Returns true if the current IRC context connection runs over IPv6.
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
    c->returnValue()->setBoolean(pConnection->target()->server()->isIPv6()))

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
		Returns true if the current IRC context connection runs over SSL.
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
    c->returnValue()->setBoolean(pConnection->target()->server()->useSSL()))

/*
	@doc: context.serverSoftware
	@type:
		function
	@title:
		$context.serverSoftware
	@short:
		Returns the software running on the IRC server, if recognized
	@syntax:
		<string> $context.serverSoftware
		<string> $context.serverSoftware(<irc_context_id:uint>)
	@description:
		Returns the software running on the IRC server, if it has been
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
    c->returnValue()->setString(pConnection->serverInfo()->software()))

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
		Returns the password used to login to the IRC server for the specified IRC context.
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
    c->returnValue()->setString(pConnection->target()->server()->password()))

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
		Returns the port of the IRC server for the specified IRC context.
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
    c->returnValue()->setInteger(pConnection->target()->server()->port()))

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
		Returns a string describing the state of the specified IRC context.
		The string will be either [i]idle[/i], [i]connecting[/i], [i]loggingin[/i] or [i]connected[/i].
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
			case KviIrcContext::PendingReconnection:
				c->returnValue()->setString(QString("pending"));
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
				echo "IRC context" %ic ": " $context.serverHostName(%ic)
		[/example]
*/

static bool context_kvs_fnc_list(KviKvsModuleFunctionCall * c)
{
	KviKvsArray * pArray = new KviKvsArray();

	int idx = 0;
	for(auto pWnd : g_pMainWindow->windowList())
	{
		if(pWnd->type() == KviWindow::Console)
		{
			pArray->set(idx, new KviKvsVariant((kvs_int_t)((KviConsoleWindow *)pWnd)->context()->id()));
			idx++;
		}
	}

	c->returnValue()->setArray(pArray);
	return true;
}

/*
	@doc: context.connectionStartTime
	@type:
		function
	@title:
		$context.connectionStartTime
	@short:
		Returns the connection start time of an IRC context
	@syntax:
		<string> $context.connectionStartTime
		<string> $context.connectionStartTime(<irc_context_id:uint>)
	@description:
		Returns the connection start time for the specified IRC context.
		If no irc_context_id is specified then the current irc_context is used.
		If the irc_context_id specification is not valid then this function
		returns nothing. If the specified IRC context is not currently connected
		then this function returns nothing.
	@seealso:
		[fnc]$context.lastMessageTime[/fnc]
*/

STANDARD_IRC_CONNECTION_TARGET_PARAMETER(
    context_kvs_fnc_connectionStartTime,
    c->returnValue()->setInteger((kvs_int_t)(pConnection->statistics()->connectionStartTime()));)

/*
	@doc: context.lastMessageTime
	@type:
		function
	@title:
		$context.lastMessageTime
	@short:
		Returns the last message time of an IRC context
	@syntax:
		<string> $context.lastMessageTime
		<string> $context.lastMessageTime(<irc_context_id:uint>)
	@description:
		Returns the last message time for the specified IRC context.
		If no irc_context_id is specified then the current irc_context is used.
		If the irc_context_id specification is not valid then this function
		returns nothing. If the specified IRC context is not currently connected
		then this function returns nothing.
	@seealso:
		[fnc]$context.connectionStartTime[/fnc]
*/

STANDARD_IRC_CONNECTION_TARGET_PARAMETER(
    context_kvs_fnc_lastMessageTime,
    c->returnValue()->setInteger((kvs_int_t)(pConnection->statistics()->lastMessageTime()));)

/*
	@doc: context.clearqueue
	@type:
		command
	@title:
		context.clearqueue
	@syntax:
		context.clearqueue [-a]
	@switches:
		!sw: -a | --all
		Remove ALL messages, not just the private messages.
	@short:
		Removes messages from the socked output queue.
	@description:
		Removes the PRIVMSG messages from the socket output queue.
		This can be used to save yourself in-extremis if you have pasted a huge amount of text
		but otherwise you shouldn't need it. This also makes sense
		only if the "limit outgoing traffic" option is currently set otherwise
		the data is immediately sent to the server and the queue is usually empty.
		Be aware that the -a switch [b]may confuse[/b] KVIrc quite a bit: do not use it unless
		you REALLY know what you're doing.
*/
static bool context_kvs_cmd_clearQueue(KviKvsModuleCommandCall * c)
{
	KVSM_REQUIRE_CONNECTION(c)

	c->window()->connection()->clearOutputQueue(!c->switches()->find('a', "all"));

	return true;
}

/*
	@doc: context.queueSize
	@type:
		function
	@title:
		$context.queueSize
	@short:
		Returns the size of the current server message queue
	@syntax:
		<integer> $context.queueSize
		<integer> $context.queueSize(<irc_context_id:uint>)
	@description:
		Returns the number of messages currently in the server message queue.
		If no irc_context_id is specified then the current irc_context is used.
		If the irc_context_id specification is not valid then this function
		returns 0.
	@seealso:
		[cmd]context.clearqueue[/cmd],
*/

static bool context_kvs_fnc_queueSize(KviKvsModuleFunctionCall * c)
{
	GET_CONNECTION_FROM_STANDARD_PARAMS;

	c->returnValue()->setInteger(pConnection ? pConnection->outputQueueSize() : 0);

	return true;
}

/*
	@doc: context.getSSLCertInfo
	@type:
		function
	@title:
		$context.getSSLCertInfo
	@short:
		Returns the requested information about certificates used in an SSL enabled IRC session
	@syntax:
		$context.getSSLCertInfo(<query:string>[,<type:string='remote'>[,<context_id:integer>[,<param1:string>]]])
	@description:
		Returns the requested information about certificates used in an SSL enabled IRC session.[br]
		The second <type> parameter can be "local" or "remote", and refers to the certificate you want
		to query the information from; if omitted, it defaults to "remote".[br]
		If <context_id> is omitted then the IRC context associated with the current window is assumed.[br]
		If <context_id> is not a valid IRC context identifier (or it is omitted and the current window
		has no associated IRC context) then this function prints a warning and returns an empty string.[br]
		If the IRC context is not using SSL then this function returns an empty string.[br]
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
		[li]fingerprintContents * accepts parameter interpreted as "digest name"[/li]
		[/ul]
	@examples:
		[example]
			[comment]# get a sha256 fingerprint of remote peer's certificate[/comment]
			$context.getSSLCertInfo(fingerprintContents,remote,$context,sha256)
		[/example]
	@seealso:
		[fnc]$certificate[/fnc]
		[fnc]$str.evpSign[/fnc]
		[fnc]$str.evpVerify[/fnc]
*/

static bool context_kvs_fnc_getSSLCertInfo(KviKvsModuleFunctionCall * c)
{
	kvs_uint_t uContextId;
	QString szQuery;
	QString szType;
	QString szParam1;
	bool bRemote;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("query", KVS_PT_STRING, 0, szQuery)
	KVSM_PARAMETER("type", KVS_PT_STRING, KVS_PF_OPTIONAL, szType)
	KVSM_PARAMETER("context_id", KVS_PT_UINT, KVS_PF_OPTIONAL, uContextId)
	KVSM_PARAMETER("param1", KVS_PT_STRING, KVS_PF_OPTIONAL, szParam1)
	KVSM_PARAMETERS_END(c)

#ifndef COMPILE_SSL_SUPPORT
	c->warning(__tr2qs("This executable was built without SSL support"));
	return true;
#else

	KviConsoleWindow * pConsole = nullptr;
	if(c->parameterCount() > 2)
		pConsole = g_pApp->findConsole(uContextId);
	else
		pConsole = c->window()->console();

	if(!pConsole)
	{
		if(c->parameterCount() > 2)
			c->warning(__tr2qs("Unable to find the specified context"));
		else
			c->warning(__tr2qs("Unable to find the current context"));
		c->returnValue()->setString("");
		return true;
	}

	if(szType.compare("local") == 0)
	{
		bRemote = false;
	}
	else if(szType.compare("remote") == 0 || szType.isEmpty())
	{
		bRemote = true;
	}
	else
	{
		c->warning(__tr2qs("You specified a bad string for the parameter \"type\""));
		c->returnValue()->setString("");
		return true;
	}

	//context is never null, connection can be null
	if(!pConsole->context()->connection())
	{
		if(c->parameterCount() > 2)
			c->warning(__tr2qs("The specified context is disconnected"));
		else
			c->warning(__tr2qs("The current context is disconnected"));
		c->returnValue()->setString("");
		return true;
	}
	// link is never null, socket can be null
	KviIrcSocket * pSocket = pConsole->context()->connection()->link()->socket();
	if(!pSocket)
	{
		if(c->parameterCount() > 2)
			c->warning(__tr2qs("The specified context's socket is disconnected"));
		else
			c->warning(__tr2qs("The current context's socket is disconnected"));
		c->returnValue()->setString("");
		return true;
	}

	if(!pSocket->usingSSL())
	{
		c->warning(__tr2qs("Unable to get SSL information: the IRC connection is not using SSL"));
		c->returnValue()->setString("");
		return true;
	}

	KviSSL * pSSL = pSocket->getSSL();
	if(!pSSL)
	{
		c->warning(__tr2qs("Unable to get SSL information: SSL non initialized yet in IRC connection"));
		c->returnValue()->setString("");
		return true;
	}

	KviSSLCertificate * pCert = bRemote ? pSSL->getPeerCertificate() : pSSL->getLocalCertificate();

	if(!pCert)
	{
		c->warning(__tr2qs("Unable to get SSL information: no peer certificate available"));
		c->returnValue()->setString("");
		return true;
	}

	if(KviSSLMaster::getSSLCertInfo(pCert, szQuery, szParam1, c->returnValue()))
		return true;

	c->warning(__tr2qs("Unable to get SSL information: query not recognized"));
	c->returnValue()->setString("");

	return true;
#endif
}

static bool context_module_init(KviModule * m)
{
	KVSM_REGISTER_FUNCTION(m, "serverHostName", context_kvs_fnc_serverHostName);
	KVSM_REGISTER_FUNCTION(m, "serverIpAddress", context_kvs_fnc_serverIpAddress);
	KVSM_REGISTER_FUNCTION(m, "serverPort", context_kvs_fnc_serverPort);
	KVSM_REGISTER_FUNCTION(m, "serverIsIPV6", context_kvs_fnc_serverIsIPV6);
	KVSM_REGISTER_FUNCTION(m, "serverIsSSL", context_kvs_fnc_serverIsSSL);
	KVSM_REGISTER_FUNCTION(m, "serverPassword", context_kvs_fnc_serverPassword);
	KVSM_REGISTER_FUNCTION(m, "networkName", context_kvs_fnc_networkName);
	KVSM_REGISTER_FUNCTION(m, "serverdbNetworkName", context_kvs_fnc_serverdbNetworkName);
	KVSM_REGISTER_FUNCTION(m, "serverdbServerHostName", context_kvs_fnc_serverdbServerHostName);
	KVSM_REGISTER_FUNCTION(m, "state", context_kvs_fnc_state);
	KVSM_REGISTER_FUNCTION(m, "list", context_kvs_fnc_list);
	KVSM_REGISTER_FUNCTION(m, "serverSoftware", context_kvs_fnc_serverSoftware);
	KVSM_REGISTER_FUNCTION(m, "connectionStartTime", context_kvs_fnc_connectionStartTime);
	KVSM_REGISTER_FUNCTION(m, "lastMessageTime", context_kvs_fnc_lastMessageTime);
	KVSM_REGISTER_FUNCTION(m, "queueSize", context_kvs_fnc_queueSize);
	KVSM_REGISTER_FUNCTION(m, "getSSLCertInfo", context_kvs_fnc_getSSLCertInfo);

	KVSM_REGISTER_SIMPLE_COMMAND(m, "clearQueue", context_kvs_cmd_clearQueue);

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
    "IRC Context Related Functions",
    context_module_init,
    0,
    0,
    context_module_cleanup,
    0)
