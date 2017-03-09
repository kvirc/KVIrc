//=============================================================================
//
//   File : libkviserverdb.cpp
//   Creation date : Tue Jul  1 01:48:49 2008 GMT by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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
#include "KviApplication.h"
#include "KviLocale.h"
#include "KviIrcServer.h"
#include "KviIrcServerDataBase.h"
#include "KviIrcNetwork.h"

#include <QString>

extern KVIRC_API KviIrcServerDataBase * g_pServerDataBase;

/*
	@doc: serverdb
	@type:
		module
	@short:
		Interface to the servers database
	@title:
		The serverDB module
	@body:
		The serverDB module is the scripting interface to the servers and networks database.[br]
		It provides the following set of commands:[br]
		[cmd]serverdb.addNetwork[/cmd]: adds a network entry to the database[br]
		[cmd]serverdb.addServer[/cmd]: adds a server entry to the database[br]
		[cmd]serverdb.setNetworkConnectCommand[/cmd]: sets a "on connect" script for the network[br]
		[cmd]serverdb.setNetworkEncoding[/cmd]: sets the network encoding[br]
		[cmd]serverdb.setNetworkTextEncoding[/cmd]: sets the network text encoding[br]
		[cmd]serverdb.setNetworkDescription[/cmd]: sets the network description[br]
		[cmd]serverdb.setNetworkLoginCommand[/cmd]: sets a "on login" script for the network[br]
		[cmd]serverdb.setNetworkNickName[/cmd]: sets the network specific nickname[br]
		[cmd]serverdb.setNetworkRealName[/cmd]: sets the network specific realname[br]
		[cmd]serverdb.setNetworkUserName[/cmd]: sets the network specific username[br]
		[cmd]serverdb.setNetworkJoinChannels[/cmd]: sets the network autojoin channels list[br]
		[cmd]serverdb.setServerConnectCommand[/cmd]: sets a "on connect" script for the server[br]
		[cmd]serverdb.setServerEncoding[/cmd]: sets the server encoding[br]
		[cmd]serverdb.setServerTextEncoding[/cmd]: sets the server text encoding[br]
		[cmd]serverdb.setServerDescription[/cmd]: sets the server description[br]
		[cmd]serverdb.setServerLoginCommand[/cmd]: sets a "on login" script for the server[br]
		[cmd]serverdb.setServerNickName[/cmd]: sets the server specific nickname[br]
		[cmd]serverdb.setServerRealName[/cmd]: sets the server specific realname[br]
		[cmd]serverdb.setServerUserName[/cmd]: sets the server specific username[br]
		[cmd]serverdb.setServerJoinChannels[/cmd]: sets the server autojoin channels list[br]
		[br]
		It provides the following set of functions:[br]
		[fnc]$serverdb.favorite[/fnc]: returns if the server is favorited or not[br]
		[fnc]$serverdb.cacheIp[/fnc]: returns the cache-ip status for a server[br]
		[fnc]$serverdb.networkConnectCommand[/fnc]: returns the "on connect" script for a network[br]
		[fnc]$serverdb.networkDescription[/fnc]: returns the description for a network[br]
		[fnc]$serverdb.networkEncoding[/fnc]: returns the encoding for a network[br]
		[fnc]$serverdb.networkTextEncoding[/fnc]: returns the text encoding for a network[br]
		[fnc]$serverdb.networkLoginCommand[/fnc]: returns the "on login" script for a network[br]
		[fnc]$serverdb.networkName[/fnc]: returns the name of a network[br]
		[fnc]$serverdb.networkNickName[/fnc]: returns the specific nickname for a network[br]
		[fnc]$serverdb.networkRealName[/fnc]: returns the specific realname for a network[br]
		[fnc]$serverdb.networkUserName[/fnc]: returns the specific username for a network[br]
		[fnc]$serverdb.networkJoinChannels[/fnc]: returns the list of autojoin channels for a network[br]
		[fnc]$serverdb.serverConnectCommand[/fnc]: returns the "on connect" script for a server[br]
		[fnc]$serverdb.serverDescription[/fnc]: returns the description for a server[br]
		[fnc]$serverdb.serverEncoding[/fnc]: returns the encoding for a server[br]
		[fnc]$serverdb.serverTextEncoding[/fnc]: returns the text encoding for a server[br]
		[fnc]$serverdb.serverId[/fnc]: returns the unique ID for a server[br]
		[fnc]$serverdb.serverIp[/fnc]: returns the IP for a server[br]
		[fnc]$serverdb.serverLoginCommand[/fnc]: returns the "on login" script for a server[br]
		[fnc]$serverdb.serverNickName[/fnc]: returns the specific nickname for a server[br]
		[fnc]$serverdb.serverPassword[/fnc]: returns the password for a server[br]
		[fnc]$serverdb.serverPort[/fnc]: returns the port for a server[br]
		[fnc]$serverdb.serverRealName[/fnc]: returns the specific realname for a server[br]
		[fnc]$serverdb.serverUserName[/fnc]: returns the specific username for a server[br]
		[fnc]$serverdb.serverJoinChannels[/fnc]: returns the list of autojoin channels for a server[br]
		[fnc]$serverdb.isAutoConnect[/fnc]: returns the autoconnect status for a server[br]
		[fnc]$serverdb.isIPv6[/fnc]: returns the ipv6 status for a server[br]
		[fnc]$serverdb.isSSL[/fnc]: returns the SSL status for a server[br]
		[fnc]$serverdb.networkExists[/fnc]: checks if a network exists[br]
		[fnc]$serverdb.serverExists[/fnc]: checks if a server exists[br]
*/

/*
	@doc: serverdb.networkExists
	@type:
		function
	@title:
		$serverdb.networkExists
	@short:
		Checks if the network already exists in the DB
	@syntax:
		<bool> $serverdb.networkExists(<network_name:string>)
	@description:
		Checks if the network already exists in the DB.[br]
		It returns [b]1[/b] if the network exists, and [b]0[/b] otherwise.
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
static bool serverdb_kvs_fnc_networkExists(KviKvsModuleFunctionCall * c)
{
	QString szNetwork;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("network_name", KVS_PT_STRING, 0, szNetwork)
	KVSM_PARAMETERS_END(c)

	if(szNetwork.isEmpty())
	{
		c->error(__tr2qs_ctx("You must provide the network name as parameter", "serverdb"));
		return false;
	}

	KviIrcNetwork * pNetwork = g_pServerDataBase->findNetwork(szNetwork);
	if(!pNetwork)
	{
		c->returnValue()->setBoolean(false);
		return true;
	}

	c->returnValue()->setBoolean(true);
	return true;
}

/*
	@doc: serverdb.serverExists
	@type:
		function
	@title:
		$serverdb.serverExists
	@short:
		Checks if the network already exists in the DB
	@syntax:
		<bool> $serverdb.serverExists(<servername:string>[,<networkname:string>])
	@description:
		Checks if the server already exists for a network in the DB.[br]
		If no network name is provided, the check is made globally.[br]
		It returns 1 if the server exists, and [b]0[/b] otherwise
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

static bool serverdb_kvs_fnc_serverExists(KviKvsModuleFunctionCall * c)
{
	QString szServer, szNetwork;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("server_name", KVS_PT_STRING, 0, szServer)
	KVSM_PARAMETER("network_name", KVS_PT_STRING, KVS_PF_OPTIONAL, szNetwork)
	KVSM_PARAMETERS_END(c)

	if(szServer.isEmpty())
	{
		c->error(__tr2qs_ctx("You must provide the server name as parameter", "serverdb"));
		return false;
	}

	if(!szNetwork.isEmpty())
	{
		// Check in the given network
		KviIrcNetwork * pRecord = g_pServerDataBase->findNetwork(szNetwork);
		if(!pRecord)
		{
			c->returnValue()->setBoolean(false);
			return true;
		}

		KviIrcServer * pCheckServer = pRecord->findServer(szServer);
		if(!pCheckServer)
		{
			c->returnValue()->setBoolean(false);
			return true;
		}

		c->returnValue()->setBoolean(true);
	}
	else
	{
		// Check through all networks
		KviPointerHashTableIterator<QString, KviIrcNetwork> it(*(g_pServerDataBase->recordDict()));

		while(KviIrcNetwork * r = it.current())
		{
			KviPointerList<KviIrcServer> * sl = r->serverList();

			for(KviIrcServer * s = sl->first(); s; s = sl->next())
			{
				if(QString::compare(s->hostName().toUtf8().data(), szServer, Qt::CaseInsensitive) == 0)
				{
					c->returnValue()->setBoolean(true);
					return true;
				}
			}
			++it;
		}

		c->returnValue()->setBoolean(false);
	}

	return true;
}

#define BEGIN_SERVERDB_GET_NETWORK_PROPERTY(__functionName)                                      \
	static bool __functionName(KviKvsModuleFunctionCall * c)                                     \
	{                                                                                            \
		QString szName;                                                                          \
                                                                                                 \
		KVSM_PARAMETERS_BEGIN(c)                                                                 \
		KVSM_PARAMETER("name", KVS_PT_STRING, 0, szName)                                         \
		KVSM_PARAMETERS_END(c)                                                                   \
                                                                                                 \
		if(szName.isEmpty())                                                                     \
		{                                                                                        \
			c->error(__tr2qs_ctx("You must provide the network name as parameter", "serverdb")); \
			return false;                                                                        \
		}                                                                                        \
                                                                                                 \
		KviIrcNetwork * pNetwork = g_pServerDataBase->findNetwork(szName);                       \
		if(!pNetwork)                                                                            \
		{                                                                                        \
			c->error(__tr2qs_ctx("The specified network doesn't exist", "serverdb"));            \
			return false;                                                                        \
		}

#define END_SERVERDB_GET_NETWORK_PROPERTY \
	return true;                          \
	}

#define SERVERDB_GET_NETWORK_PROPERTY(__functionName, __callName) \
	BEGIN_SERVERDB_GET_NETWORK_PROPERTY(__functionName)           \
                                                                  \
	c->returnValue()->setString(pNetwork->__callName());          \
                                                                  \
	END_SERVERDB_GET_NETWORK_PROPERTY

#define BEGIN_SERVERDB_GET_SERVER_PROPERTY(__functionName)                                       \
	static bool __functionName(KviKvsModuleFunctionCall * c)                                     \
	{                                                                                            \
		QString szNetName, szServName;                                                           \
                                                                                                 \
		KVSM_PARAMETERS_BEGIN(c)                                                                 \
		KVSM_PARAMETER("network_name", KVS_PT_STRING, 0, szNetName)                              \
		KVSM_PARAMETER("server_name", KVS_PT_STRING, 0, szServName)                              \
		KVSM_PARAMETERS_END(c)                                                                   \
                                                                                                 \
		if(szNetName.isEmpty())                                                                  \
		{                                                                                        \
			c->error(__tr2qs_ctx("You must provide the network name as parameter", "serverdb")); \
			return false;                                                                        \
		}                                                                                        \
                                                                                                 \
		if(szServName.isEmpty())                                                                 \
		{                                                                                        \
			c->error(__tr2qs_ctx("You must provide the server name as parameter", "serverdb"));  \
			return false;                                                                        \
		}                                                                                        \
                                                                                                 \
		KviIrcNetwork * pRecord = g_pServerDataBase->findNetwork(szNetName);                     \
		if(!pRecord)                                                                             \
		{                                                                                        \
			c->error(__tr2qs_ctx("The specified network doesn't exist", "serverdb"));            \
			return false;                                                                        \
		}                                                                                        \
                                                                                                 \
		KviIrcServer * pServer = pRecord->findServer(szServName);                                \
		if(!pServer)                                                                             \
		{                                                                                        \
			c->error(__tr2qs_ctx("The specified server doesn't exist", "serverdb"));             \
			return false;                                                                        \
		}

#define END_SERVERDB_GET_SERVER_PROPERTY \
	return true;                         \
	}

#define SERVERDB_GET_SERVER_PROPERTY(__functionName, __callName, __variantSetCallName) \
	BEGIN_SERVERDB_GET_SERVER_PROPERTY(__functionName)                                 \
	c->returnValue()->__variantSetCallName(pServer->__callName());                     \
	END_SERVERDB_GET_SERVER_PROPERTY

/*
	@doc: serverdb.networkNickName
	@type:
		function
	@title:
		$serverdb.networkNickName
	@short:
		Returns the nickname
	@syntax:
		<string> $serverdb.networkNickName(<network:string>)
	@description:
		Returns the nickname set for the network <network> if set
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_GET_NETWORK_PROPERTY(serverdb_kvs_fnc_networkNickName, nickName)

/*
	@doc: serverdb.networkUserName
	@type:
		function
	@title:
		$serverdb.networkUserName
	@short:
		Returns the username
	@syntax:
		<string> $serverdb.networkUserName(<network:string>)
	@description:
		Returns the username set for the network <network> if set.
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_GET_NETWORK_PROPERTY(serverdb_kvs_fnc_networkUserName, userName)

/*
	@doc: serverdb.networkRealName
	@type:
		function
	@title:
		$serverdb.networkRealName
	@short:
		Returns the real name
	@syntax:
		<string> $serverdb.networkRealName(<network:string>)
	@description:
		Returns the real name set for the network <network> if set
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_GET_NETWORK_PROPERTY(serverdb_kvs_fnc_networkRealName, realName)

/*
	@doc: serverdb.networkEncoding
	@type:
		function
	@title:
		$serverdb.networkEncoding
	@short:
		Returns the encoding
	@syntax:
		<string> $serverdb.networkEncoding(<network:string>)
	@description:
		Returns the encoding used for the network <network> for server specific messages, like channel and nick names, if set
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_GET_NETWORK_PROPERTY(serverdb_kvs_fnc_networkEncoding, encoding)

/*
	@doc: serverdb.networkTextEncoding
	@type:
		function
	@title:
		$serverdb.networkTextEncoding
	@short:
		Returns the encoding
	@syntax:
		<string> $serverdb.networkTextEncoding(<network:string>)
	@description:
		Returns the encoding used for the network <network> for text messages, if set
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_GET_NETWORK_PROPERTY(serverdb_kvs_fnc_networkTextEncoding, textEncoding)

/*
	@doc: serverdb.networkDescription
	@type:
		function
	@title:
		$serverdb.networkDescription
	@short:
		Returns the description
	@syntax:
		<string> $serverdb.networkDescription(<network:string>)
	@description:
		Returns the description set for the network <network> if set
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
SERVERDB_GET_NETWORK_PROPERTY(serverdb_kvs_fnc_networkDescription, description)

/*
	@doc: serverdb.networkConnectCommand
	@type:
		function
	@title:
		$serverdb.networkConnectCommand
	@short:
		Returns the connect command
	@syntax:
		<string> $serverdb.networkConnectCommand(<network:string>)
	@description:
		Returns the connect command set for the network <network> if set
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_GET_NETWORK_PROPERTY(serverdb_kvs_fnc_networkConnectCommand, onConnectCommand)

/*
	@doc: serverdb.networkLoginCommand
	@type:
		function
	@title:
		$serverdb.networkLoginCommand
	@short:
		Returns the login command
	@syntax:
		<string> $serverdb.networkLoginCommand(<network:string>)
	@description:
		Returns the login command set for the network <network> if set
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_GET_NETWORK_PROPERTY(serverdb_kvs_fnc_networkLoginCommand, onLoginCommand)

/*
	@doc: serverdb.networkName
	@type:
		function
	@title:
		$serverdb.networkName
	@short:
		Returns the name
	@syntax:
		<string> $serverdb.networkName(<network:string>)
	@description:
		Returns the name of the network <network>
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_GET_NETWORK_PROPERTY(serverdb_kvs_fnc_networkName, name)

/*
	@doc: serverdb.networkJoinChannels
	@type:
		function
	@title:
		$serverdb.networkJoinChannels
	@short:
		Returns the list of autojoin channels
	@syntax:
		<string> $serverdb.networkJoinChannels(<network:string>)
	@description:
		Returns an array of autojoin channels and their relative passwords set for the network <network>[br]
		Each item in the array is in the format <channel:string>:<password:string>
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

BEGIN_SERVERDB_GET_NETWORK_PROPERTY(serverdb_kvs_fnc_networkJoinChannels)

KviKvsArray * pArray = new KviKvsArray();

QStringList * pAutoJoinChannels = pNetwork->autoJoinChannelList();
if(pAutoJoinChannels)
{
	kvs_uint_t idx = 0;
	foreach(QString szEntry, *pAutoJoinChannels)
	{
		pArray->set(idx, new KviKvsVariant(szEntry));
		idx++;
	}
}

c->returnValue()->setArray(pArray);

END_SERVERDB_GET_NETWORK_PROPERTY

/*
	@doc: serverdb.serverNickName
	@type:
		function
	@title:
		$serverdb.serverNickName
	@short:
		Returns the nickname
	@syntax:
		<string> $serverdb.serverNickName(<network:string>,<server:string>)
	@description:
		Returns the nickname set for the server <server> of the network <network> if set
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_GET_SERVER_PROPERTY(serverdb_kvs_fnc_serverNickName, nickName, setString)

/*
	@doc: serverdb.serverUserName
	@type:
		function
	@title:
		$serverdb.serverUserName
	@short:
		Returns the username
	@syntax:
		<string> $serverdb.serverUserName(<network:string>,<server:string>)
	@description:
		Returns the username set for the server <server> of the network <network> if set
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_GET_SERVER_PROPERTY(serverdb_kvs_fnc_serverUserName, userName, setString)

/*
	@doc: serverdb.serverRealName
	@type:
		function
	@title:
		$serverdb.serverRealName
	@short:
		Returns the real name
	@syntax:
		<string> $serverdb.serverRealName(<network:string>,<server:string>)
	@description:
		Returns the real name set for the server <server> of the network <network> if set
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_GET_SERVER_PROPERTY(serverdb_kvs_fnc_serverRealName, realName, setString)

/*
	@doc: serverdb.serverEncoding
	@type:
		function
	@title:
		$serverdb.serverEncoding
	@short:
		Returns the encoding
	@syntax:
		<string> $serverdb.serverEncoding(<network:string>,<server:string>)
	@description:
		Returns the encoding used for the server <server> of the network <network> for server specific messages, like channel and nick names, if set
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_GET_SERVER_PROPERTY(serverdb_kvs_fnc_serverEncoding, encoding, setString)

/*
	@doc: serverdb.serverTextEncoding
	@type:
		function
	@title:
		$serverdb.serverTextEncoding
	@short:
		Returns the encoding
	@syntax:
		<string> $serverdb.serverTextEncoding(<network:string>,<server:string>)
	@description:
		Returns the encoding used for the server <server> of the network <network> for server specific messages, like channel and nick names, if set
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_GET_SERVER_PROPERTY(serverdb_kvs_fnc_serverTextEncoding, textEncoding, setString)

/*
	@doc: serverdb.serverDescription
	@type:
		function
	@title:
		$serverdb.serverDescription
	@short:
		Returns the description
	@syntax:
		<string> $serverdb.serverDescription(<network:string>,<server:string>)
	@description:
		Returns the description set for the server <server> of the network <network> if set
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_GET_SERVER_PROPERTY(serverdb_kvs_fnc_serverDescription, description, setString)

/*
	@doc: serverdb.serverConnectCommand
	@type:
		function
	@title:
		$serverdb.serverConnectCommand
	@short:
		Returns the connect command
	@syntax:
		<string> $serverdb.serverConnectCommand(<network:string>,<server:string>)
	@description:
		Returns the connect command set for the server <server> of the network <network> if set
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_GET_SERVER_PROPERTY(serverdb_kvs_fnc_serverConnectCommand, onConnectCommand, setString)

/*
	@doc: serverdb.serverLoginCommand
	@type:
		function
	@title:
		$serverdb.serverLoginCommand
	@short:
		Returns the login command
	@syntax:
		<string> $serverdb.serverLoginCommand(<network:string>,<server:string>)
	@description:
		Returns the login command set for the server <server> of the network <network> if set
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_GET_SERVER_PROPERTY(serverdb_kvs_fnc_serverLoginCommand, onLoginCommand, setString)

/*
	@doc: serverdb.serverIp
	@type:
		function
	@title:
		$serverdb.serverIp
	@short:
		Returns the IP address
	@syntax:
		<string> $serverdb.serverIp(<network:string>,<server:string>)
	@description:
		Returns the IP address of the server <server> of the network <network>
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_GET_SERVER_PROPERTY(serverdb_kvs_fnc_serverIp, ip, setString)

/*
	@doc: serverdb.serverId
	@type:
		function
	@title:
		$serverdb.serverId
	@short:
		Returns the ID
	@syntax:
		<string> $serverdb.serverId(<network:string>,<server:string>)
	@description:
		Returns the ID of the server <server> of the network <network>
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_GET_SERVER_PROPERTY(serverdb_kvs_fnc_serverId, id, setString)

/*
	@doc: serverdb.serverPassword
	@type:
		function
	@title:
		$serverdb.serverPassword
	@short:
		Returns the password
	@syntax:
		<string> $serverdb.serverPassword(<network:string>,<server:string>)
	@description:
		Returns the password of the server <server> of the network <network> if set
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_GET_SERVER_PROPERTY(serverdb_kvs_fnc_serverPassword, password, setString)

/*
	@doc: serverdb.serverPort
	@type:
		function
	@title:
		$serverdb.serverPort
	@short:
		Returns the port
	@syntax:
		<int> $serverdb.serverPort(<network:string>,<server:string>)
	@description:
		Returns the port of the server <server> of the network <network> if set
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_GET_SERVER_PROPERTY(serverdb_kvs_fnc_serverPort, port, setInteger)

/*
	@doc: serverdb.isAutoConnect
	@type:
		function
	@title:
		$serverdb.isAutoConnect
	@short:
		Returns the autoconnect status
	@syntax:
		<bool> $serverdb.isAutoConnect(<network:string>,<server:string>)
	@description:
		Returns true if the server <server> of the network <network> if set to autoconnect, false otherwise
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_GET_SERVER_PROPERTY(serverdb_kvs_fnc_serverAutoConnect, autoConnect, setBoolean)

/*
	@doc: serverdb.isIPv6
	@type:
		function
	@title:
		$serverdb.isIPv6
	@short:
		Returns the IPv6 status
	@syntax:
		<bool> $serverdb.isIPv6(<network:string>,<server:string>)
	@description:
		Returns true if the server <server> of the network <network> if set to connect using IPv6 sockets, false otherwise
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_GET_SERVER_PROPERTY(serverdb_kvs_fnc_serverIPv6, isIPv6, setBoolean)

/*
	@doc: serverdb.isSSL
	@type:
		function
	@title:
		$serverdb.isSSL
	@short:
		Returns the SSL status
	@syntax:
		<bool> $serverdb.isSSL(<network:string>,<server:string>)
	@description:
		Returns true if the server <server> of the network <network> if set to connect using SSL (Secure Socket Layer) sockets, false otherwise
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_GET_SERVER_PROPERTY(serverdb_kvs_fnc_serverSSL, useSSL, setBoolean)

/*
	@doc: serverdb.cacheIp
	@type:
		function
	@title:
		$serverdb.cacheIp
	@short:
		Returns the cache-ip status
	@syntax:
		<bool> $serverdb.cacheIp(<network:string>,<server:string>)
	@description:
		Returns true if KVIrc is set to cache the IP of the server <server> of the network <network>, false otherwise
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_GET_SERVER_PROPERTY(serverdb_kvs_fnc_serverCacheIp, cacheIp, setBoolean)

/*
	@doc: serverdb.favorite
	@type:
		function
	@title:
		$serverdb.favorite
	@short:
		Returns if the server is favorited
	@syntax:
		<bool> $serverdb.favorite(<network:string>,<server:string>)
	@description:
		Returns true if KVIrc has the server set to a favorite, false otherwise
*/

SERVERDB_GET_SERVER_PROPERTY(serverdb_kvs_fnc_serverFavorite, favorite, setBoolean)

/*
	@doc: serverdb.serverJoinChannels
	@type:
		function
	@title:
		$serverdb.serverJoinChannels
	@short:
		Returns the list of autojoin channels
	@syntax:
		<array> $serverdb.serverJoinChannels(<network:string>,<server:string>)
	@description:
		Returns an array containing the autojoin channels and their relative passwords set for the server <server> of the network <network>[br]
		Each item in the array is in the format channel_name:password
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
BEGIN_SERVERDB_GET_SERVER_PROPERTY(serverdb_kvs_fnc_serverJoinChannels)

KviKvsArray * pArray = new KviKvsArray();

QStringList * pAutoJoinChannels = pServer->autoJoinChannelList();
if(pAutoJoinChannels)
{
	kvs_uint_t idx = 0;
	foreach(QString szEntry, *pAutoJoinChannels)
	{
		pArray->set(idx, new KviKvsVariant(szEntry));
		idx++;
	}
}

c->returnValue()->setArray(pArray);

END_SERVERDB_GET_SERVER_PROPERTY

/*
	@doc: serverdb.addNetwork
	@type:
		command
	@title:
		serverdb.addNetwork
	@short:
		Adds a network
	@syntax:
		serverdb.addNetwork <string:network name>
	@description:
		Adds the specified network <network name> to the DB.[br]
		If the network already exists, an error is printed unless the -q switch is used.
	@switches:
		!sw: -a | --autoconnect
		Autoconnect this network at KVIrc start.
	@examples:
		[example]
		if(![fnc]$serverdb.networkExists[/fnc](freenode)) serverdb.addNetwork -q freenode
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

static bool serverdb_kvs_cmd_addNetwork(KviKvsModuleCommandCall * c)
{
	QString szNetName, szNetwork;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("network_name", KVS_PT_STRING, 0, szNetName)
	KVSM_PARAMETERS_END(c)

	if(szNetName.isEmpty())
	{
		c->error(__tr2qs_ctx("You must provide the network name as parameter", "serverdb"));
		return false;
	}

	KviIrcNetwork * pNetwork = g_pServerDataBase->findNetwork(szNetName);
	if(pNetwork)
	{
		if(c->switches()->find('q', "quiet"))
			return true;
		c->error(__tr2qs_ctx("The network specified already exists", "serverdb"));
		return false;
	}

	pNetwork = new KviIrcNetwork(szNetName);
	if(c->switches()->find('a', "autoconnect"))
		pNetwork->setAutoConnect(true);

	g_pServerDataBase->addNetwork(pNetwork);
	return true;
}

/*
	@doc: serverdb.addServer
	@type:
		command
	@title:
		serverdb.addServer
	@short:
		Adds a server
	@syntax:
		serverdb.addServer [switches] <network:string> <server:string>
	@description:
		Adds the server <server> to the network <network>.
	@switches:
		!sw: -a | --autoconnect
		Autoconnect this server at KVIrc start.[br]

		!sw: -c | --cache-ip
		Caches the server IP at first connect to save bandwidth for future connects.[br]

		!sw: -f | --favorite
		Sets the server as a favorite.[br]

		!sw: -i | --ipv6
		Use IPv6 socket to connect to the server.[br]

		!sw: -p=<port> | --port=<port>
		Use the port <port> to connect to the server.[br]

		!sw: -q | --quiet
		Do not print errors if the server already exist.[br]

		!sw: -s | --ssl
		Use SSL (Secure Socket Layer) socket to connect to the server.[br]

		!sw: -w=<password> | --password=<password>
		Use password <password> to connect to the server.
	@examples:
		[example]
		[comment]Sets the nickname HelLViS69 for the server irc.freenode.net[/comment][br]
		serverdb.setNickName -s irc.freenode.net HelLViS69
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

static bool serverdb_kvs_cmd_addServer(KviKvsModuleCommandCall * c)
{
	QString szNetwork, szServer;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("network_name", KVS_PT_STRING, 0, szNetwork)
	KVSM_PARAMETER("server_name", KVS_PT_STRING, 0, szServer)
	KVSM_PARAMETERS_END(c)

	if(szNetwork.isEmpty())
	{
		c->error(__tr2qs_ctx("You must provide the network name as parameter", "serverdb"));
		return false;
	}

	if(szServer.isEmpty())
	{
		c->error(__tr2qs_ctx("You must provide the server name as parameter", "serverdb"));
		return false;
	}

	KviIrcNetwork * pRecord = g_pServerDataBase->findNetwork(szNetwork);
	if(!pRecord)
	{
		// FIXME: default to orphan servers
		c->error(__tr2qs_ctx("The specified network doesn't exist", "serverdb"));
		return false;
	}

	KviIrcServer * pServer = new KviIrcServer();
	pServer->setHostName(szServer);

	KviIrcServer * pServerRecord = pRecord->findServer(pServer);
	if(pServerRecord)
	{
		delete pServer;
		if(c->switches()->find('q', "quiet"))
			return true;
		c->error(__tr2qs_ctx("The specified server already exists", "serverdb"));
		return false;
	}

	if(c->switches()->find('a', "autoconnect"))
		pServer->setAutoConnect(true);
	if(c->switches()->find('c', "cache-ip"))
		pServer->setCacheIp(true);
	if(c->switches()->find('f', "favorite"))
		pServer->setFavorite(true);
	if(c->switches()->find('i', "ipv6"))
		pServer->setIPv6(true);
	if(c->switches()->find('s', "ssl"))
		pServer->setUseSSL(true);

	QString tmp;

	if(c->switches()->getAsStringIfExisting('p', "port", tmp))
	{
		bool bOk;
		unsigned int uPort = tmp.toInt(&bOk);
		if(!bOk)
			uPort = 6667;
		pServer->setPort(uPort);
	}

	if(c->switches()->getAsStringIfExisting('w', "password", tmp))
		pServer->setPassword(tmp);

	pRecord->insertServer(pServer);

	return true;
}

#define SERVERDB_SET_NETWORK_PROPERTY(__functionName, __callName)                                \
	static bool __functionName(KviKvsModuleCommandCall * c)                                      \
	{                                                                                            \
		QString szName, szPropertyValue;                                                         \
                                                                                                 \
		KVSM_PARAMETERS_BEGIN(c)                                                                 \
		KVSM_PARAMETER("name", KVS_PT_STRING, 0, szName)                                         \
		KVSM_PARAMETER("property", KVS_PT_STRING, KVS_PF_APPENDREMAINING, szPropertyValue)       \
		KVSM_PARAMETERS_END(c)                                                                   \
                                                                                                 \
		if(szName.isEmpty())                                                                     \
		{                                                                                        \
			c->error(__tr2qs_ctx("You must provide the network name as parameter", "serverdb")); \
			return false;                                                                        \
		}                                                                                        \
                                                                                                 \
		KviIrcNetwork * pNetwork = g_pServerDataBase->findNetwork(szName);                       \
		if(!pNetwork)                                                                            \
		{                                                                                        \
			if(c->switches()->find('q', "quiet"))                                                \
				return true;                                                                     \
			c->error(__tr2qs_ctx("The specified network doesn't exist", "serverdb"));            \
			return false;                                                                        \
		}                                                                                        \
                                                                                                 \
		pNetwork->__callName(szPropertyValue);                                                   \
                                                                                                 \
		return true;                                                                             \
	}

#define SERVERDB_SET_SERVER_PROPERTY(__functionName, __callName)                                 \
	static bool __functionName(KviKvsModuleCommandCall * c)                                      \
	{                                                                                            \
		QString szNetName, szServName, szPropertyValue;                                          \
                                                                                                 \
		KVSM_PARAMETERS_BEGIN(c)                                                                 \
		KVSM_PARAMETER("network_name", KVS_PT_STRING, 0, szNetName)                              \
		KVSM_PARAMETER("server_name", KVS_PT_STRING, 0, szServName)                              \
		KVSM_PARAMETER("property", KVS_PT_STRING, KVS_PF_APPENDREMAINING, szPropertyValue)       \
		KVSM_PARAMETERS_END(c)                                                                   \
                                                                                                 \
		if(szNetName.isEmpty())                                                                  \
		{                                                                                        \
			c->error(__tr2qs_ctx("You must provide the network name as parameter", "serverdb")); \
			return false;                                                                        \
		}                                                                                        \
                                                                                                 \
		if(szServName.isEmpty())                                                                 \
		{                                                                                        \
			c->error(__tr2qs_ctx("You must provide the server name as parameter", "serverdb"));  \
			return false;                                                                        \
		}                                                                                        \
                                                                                                 \
		KviIrcNetwork * pRecord = g_pServerDataBase->findNetwork(szNetName);                     \
		if(!pRecord)                                                                             \
		{                                                                                        \
			if(c->switches()->find('q', "quiet"))                                                \
				return true;                                                                     \
			c->error(__tr2qs_ctx("The specified network doesn't exist", "serverdb"));            \
			return false;                                                                        \
		}                                                                                        \
                                                                                                 \
		KviIrcServer * pServer = pRecord->findServer(szServName);                                \
		if(!pServer)                                                                             \
		{                                                                                        \
			if(c->switches()->find('q', "quiet"))                                                \
				return true;                                                                     \
			c->error(__tr2qs_ctx("The specified server doesn't exist", "serverdb"));             \
			return false;                                                                        \
		}                                                                                        \
                                                                                                 \
		pServer->__callName(szPropertyValue);                                                    \
                                                                                                 \
		return true;                                                                             \
	}

/*
	@doc: serverdb.setNetworkNickName
	@type:
		command
	@title:
		serverdb.setNetworkNickName
	@short:
		Sets the nickname
	@syntax:
		serverdb.setNetworkNickName [switches] <name:string> <nick:string>
	@description:
		Sets the nickname <nick> for the specified network <name>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the network already exists.
	@examples:
		[example]
		[comment]Quietly sets the nickname HelLViS69 for the freenode network[/comment][br]
		serverdb.setNetworkNickName -q freenode HelLViS69
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_SET_NETWORK_PROPERTY(serverdb_kvs_cmd_setNetworkNickName, setNickName)

/*
	@doc: serverdb.setNetworkUserName
	@type:
		command
	@title:
		serverdb.setNetworkUserName
	@short:
		Sets the username
	@syntax:
		serverdb.setNetworkUserName [switches] <name:string> <username:string>
	@description:
		Sets the username <username> for the specified network <name>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the network already exists.
	@examples:
		[example]
		[comment]Quietly sets the username kvirc for the freenode network[/comment][br]
		serverdb.setNetworkUserName -q freenode kvirc
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_SET_NETWORK_PROPERTY(serverdb_kvs_cmd_setNetworkUserName, setUserName)

/*
	@doc: serverdb.setNetworkRealName
	@type:
		command
	@title:
		serverdb.setNetworkRealName
	@short:
		Sets the realn ame
	@syntax:
		serverdb.setNetworkRealName [switches] <name:string> <realname:string>
	@description:
		Sets the real name <realname> for the specified network <name>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the network already exists.
	@examples:
		[example]
		[comment]Quietly sets the real name KVIrc 4.0 for the freenode network[/comment][br]
		serverdb.setNetworkRealName -q freenode KVIrc 4.0
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_SET_NETWORK_PROPERTY(serverdb_kvs_cmd_setNetworkRealName, setRealName)

/*
	@doc: serverdb.setNetworkEncoding
	@type:
		command
	@title:
		serverdb.setNetworkEncoding
	@short:
		Sets the encoding
	@syntax:
		serverdb.setNetworkEncoding [switches] <name:string> <encoding:string>
	@description:
		Sets the encoding <encoding> for the specified network <name>. This encoding will be used for server-specific
		text, like channel names and nicknames.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the network already exists.
	@examples:
		[example]
		[comment]Quietly sets the encoding UTF-8 for the freenode network[/comment][br]
		serverdb.setNetworkEncoding -q freenode UTF-8
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_SET_NETWORK_PROPERTY(serverdb_kvs_cmd_setNetworkEncoding, setEncoding)

/*
	@doc: serverdb.setNetworkTextEncoding
	@type:
		command
	@title:
		serverdb.setNetworkTextEncoding
	@short:
		Sets the encoding
	@syntax:
		serverdb.setNetworkTextEncoding [switches] <name:string> <encoding:string>
	@description:
		Sets the encoding <encoding> for the specified network <name>. This encoding will be used for text messages.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the network already exists.
	@examples:
		[example]
		[comment]Quietly sets the text encoding UTF-8 for the freenode network[/comment][br]
		serverdb.setNetworkTextEncoding -q freenode UTF-8
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_SET_NETWORK_PROPERTY(serverdb_kvs_cmd_setNetworkTextEncoding, setTextEncoding)

/*
	@doc: serverdb.setNetworkDescription
	@type:
		command
	@title:
		serverdb.setNetworkDescription
	@short:
		Sets the description
	@syntax:
		serverdb.setNetworkDescription [switches] <name:string> <description:string>
	@description:
		Sets the description <description> for the specified network <name>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the network already exists.
	@examples:
		[example]
		[comment]Quietly sets the description for the freenode network[/comment][br]
		serverdb.setNetworkDescription -q freenode The FOSS Network
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_SET_NETWORK_PROPERTY(serverdb_kvs_cmd_setNetworkDescription, setDescription)

/*
	@doc: serverdb.setNetworkConnectCommand
	@type:
		command
	@title:
		serverdb.setNetworkConnectCommand
	@short:
		Sets the connect command
	@syntax:
		serverdb.setNetworkConnectCommand [switches] <name:string> <command:string>
	@description:
		Sets the command <command> to run on connect for the specified network <name>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the network already exists.
	@examples:
		[example]
		[comment]Quietly sets the connect command for the freenode network[/comment][br]
		serverdb.setNetworkConnectCommand -q freenode ns identify HelLViS69 foobar
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_SET_NETWORK_PROPERTY(serverdb_kvs_cmd_setNetworkConnectCommand, setOnConnectCommand)

/*
	@doc: serverdb.setNetworkLoginCommand
	@type:
		command
	@title:
		serverdb.setNetworkLoginCommand
	@short:
		Sets the login command
	@syntax:
		serverdb.setNetworkLoginCommand [switches] <name:string> <command:string>
	@description:
		Sets the command <command> to run on login for the specified network <name>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the network already exists.
	@examples:
		[example]
		[comment]Quietly sets the login command for the freenode network[/comment][br]
		serverdb.setNetworkLoginCommand -q freenode join #KVIrc
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_SET_NETWORK_PROPERTY(serverdb_kvs_cmd_setNetworkLoginCommand, setOnLoginCommand)

/*
	@doc: serverdb.setNetworkJoinChannels
	@type:
		command
	@title:
		serverdb.setNetworkJoinChannels
	@short:
		Sets the autojoin channels list
	@syntax:
		serverdb.setNetworkJoinChannels <name:string> <channel_list:string>
	@description:
		Sets the autojoin channels list for the specified network <name>.[br]
		Items in the list have to be separated by a comma, and each item has to be in the form <channel:string>:<password:string>
	@examples:
		[example]
		[comment]Set two autojoin channels for freenode[/comment][br]
		serverdb.setNetworkJoinChannels freenode #kvirc:,#secretchan:password
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_SET_NETWORK_PROPERTY(serverdb_kvs_cmd_setNetworkJoinChannels, setAutoJoinChannelList)

/*
	@doc: serverdb.setServerNickName
	@type:
		command
	@title:
		serverdb.setServerNickName
	@short:
		Sets the nickname
	@syntax:
		serverdb.setServerNickName [switches] <network:string> <server:string> <nick:string>
	@description:
		Sets the nickname <nick> for the specified server <server> in the network <network>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the server already exists.
	@examples:
		[example]
		[comment]Quietly sets the nickname HelLViS69 for the freenode network[/comment][br]
		serverdb.setServerNickName -q irc.freenode.net HelLViS69
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_SET_SERVER_PROPERTY(serverdb_kvs_cmd_setServerNickName, setNickName)

/*
	@doc: serverdb.setServerUserName
	@type:
		command
	@title:
		serverdb.setServerUserName
	@short:
		Sets the username
	@syntax:
		serverdb.setServerUserName [switches] <network:string> <server:string> <username:string>
	@description:
		Sets the username <username> for the specified server <server> in the network <network>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the server already exists.
	@examples:
		[example]
		[comment]Quietly sets the username kvirc for the freenode server[/comment][br]
		serverdb.setServerUserName -q irc.freenode.net kvirc
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_SET_SERVER_PROPERTY(serverdb_kvs_cmd_setServerUserName, setUserName)

/*
	@doc: serverdb.setServerRealName
	@type:
		command
	@title:
		serverdb.setServerRealName
	@short:
		Sets the real name
	@syntax:
		serverdb.setServerRealName [switches] <network:string> <server:string> <realname:string>
	@description:
		Sets the real name <realname> for the specified server <server> in the network <network>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the server already exists.
	@examples:
		[example]
		[comment]Quietly sets the real name KVIrc 4.0 for the freenode server[/comment][br]
		serverdb.setServerRealName -q irc.freenode.net KVIrc 4.0
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_SET_SERVER_PROPERTY(serverdb_kvs_cmd_setServerRealName, setRealName)

/*
	@doc: serverdb.setServerEncoding
	@type:
		command
	@title:
		serverdb.setServerEncoding
	@short:
		Sets the encoding
	@syntax:
		serverdb.setServerEncoding [switches] <network:string> <server:string> <encoding:string>
	@description:
		Sets the encoding <encoding> for the specified server <server> in the network <network>. This encoding will be used for server-specific
		text, like channel names and nicknames.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the server already exists.
	@examples:
		[example]
		[comment]Quietly sets the encoding UTF-8 for the irc.freenode.net server[/comment][br]
		serverdb.setServerEncoding -q irc.freenode.net UTF-8
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_SET_SERVER_PROPERTY(serverdb_kvs_cmd_setServerEncoding, setEncoding)

/*
	@doc: serverdb.setServerTextEncoding
	@type:
		command
	@title:
		serverdb.setServerTextEncoding
	@short:
		Sets the text encoding
	@syntax:
		serverdb.setServerTextEncoding [switches] <network:string> <server:string> <encoding:string>
	@description:
		Sets the encoding <encoding> for the specified server <server> in the network <network>. This encoding will be used for text messages.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the server already exists.
	@examples:
		[example]
		[comment]Quietly sets the text encoding UTF-8 for the irc.freenode.net server[/comment][br]
		serverdb.setServerTextEncoding -q irc.freenode.net UTF-8
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_SET_SERVER_PROPERTY(serverdb_kvs_cmd_setServerTextEncoding, setTextEncoding)

/*
	@doc: serverdb.setServerDescription
	@type:
		command
	@title:
		serverdb.setServerDescription
	@short:
		Sets the description
	@syntax:
		serverdb.setServerDescription [switches] <network:string> <server:string> <description:string>
	@description:
		Sets the description <description> for the specified server <server> in the network <network>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the server already exists.
	@examples:
		[example]
		[comment]Quietly sets the description for the freenode server[/comment][br]
		serverdb.setServerDescription -q irc.freenode.net The FOSS Network
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_SET_SERVER_PROPERTY(serverdb_kvs_cmd_setServerDescription, setDescription)

/*
	@doc: serverdb.setServerConnectCommand
	@type:
		command
	@title:
		serverdb.setServerConnectCommand
	@short:
		Sets the connect command
	@syntax:
		serverdb.setServerConnectCommand [switches] <network:string> <server:string> <command:string>
	@description:
		Sets the command <command> to run on connect for the specified server <server> in the network <network>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the server already exists.
	@examples:
		[example]
		[comment]Quietly sets the connect command for the freenode server[/comment][br]
		serverdb.setServerConnectCommand -q irc.freenode.net ns identify HelLViS69 foobar
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_SET_SERVER_PROPERTY(serverdb_kvs_cmd_setServerConnectCommand, setOnConnectCommand)

/*
	@doc: serverdb.setServerLoginCommand
	@type:
		command
	@title:
		serverdb.setServerLoginCommand
	@short:
		Sets the login command
	@syntax:
		serverdb.setServerLoginCommand [switches] <network:string> <server:string> <command:string>
	@description:
		Sets the command <command> to run on login for the specified server <server> in the network <network>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the server already exists.
	@examples:
		[example]
		[comment]Quietly sets the login command for the freenode server[/comment][br]
		serverdb.setServerLoginCommand -q irc.freenode.net join #KVIrc
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_SET_SERVER_PROPERTY(serverdb_kvs_cmd_setServerLoginCommand, setOnLoginCommand)

/*
	@doc: serverdb.setServerJoinChannels
	@type:
		command
	@title:
		serverdb.setServerJoinChannels
	@short:
		Sets the autojoin channels list
	@syntax:
		serverdb.setServerJoinChannels <network:string> <server:string> <channel_list:string>
	@description:
		Sets the autojoin channels list for the specified server <server> in the network <network>.[br]
		Items in the list have to be separated by a comma, and each item has to be in the form <channel:string>:<password:string>
	@examples:
		[example]
		[comment]Set two autojoin channels for irc.freenode.net[/comment][br]
		serverdb.setServerJoinChannels irc.freenode.net #kvirc:,#secretchan:password
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

SERVERDB_SET_SERVER_PROPERTY(serverdb_kvs_cmd_setServerJoinChannels, setAutoJoinChannelList)

static bool serverdb_module_init(KviModule * m)
{
	/*
	KVSM_REGISTER_SIMPLE_COMMAND(m,"updateList",serverdb_kvs_cmd_updateList);
	*/

	KVSM_REGISTER_FUNCTION(m, "favorite", serverdb_kvs_fnc_serverFavorite);
	KVSM_REGISTER_FUNCTION(m, "cacheIp", serverdb_kvs_fnc_serverCacheIp);
	KVSM_REGISTER_FUNCTION(m, "networkConnectCommand", serverdb_kvs_fnc_networkConnectCommand);
	KVSM_REGISTER_FUNCTION(m, "networkDescription", serverdb_kvs_fnc_networkDescription);
	KVSM_REGISTER_FUNCTION(m, "networkEncoding", serverdb_kvs_fnc_networkEncoding);
	KVSM_REGISTER_FUNCTION(m, "networkTextEncoding", serverdb_kvs_fnc_networkTextEncoding);
	KVSM_REGISTER_FUNCTION(m, "networkLoginCommand", serverdb_kvs_fnc_networkLoginCommand);
	KVSM_REGISTER_FUNCTION(m, "networkName", serverdb_kvs_fnc_networkName);
	KVSM_REGISTER_FUNCTION(m, "networkNickName", serverdb_kvs_fnc_networkNickName);
	KVSM_REGISTER_FUNCTION(m, "networkRealName", serverdb_kvs_fnc_networkRealName);
	KVSM_REGISTER_FUNCTION(m, "networkUserName", serverdb_kvs_fnc_networkUserName);
	KVSM_REGISTER_FUNCTION(m, "networkJoinChannels", serverdb_kvs_fnc_networkJoinChannels);
	KVSM_REGISTER_FUNCTION(m, "serverConnectCommand", serverdb_kvs_fnc_serverConnectCommand);
	KVSM_REGISTER_FUNCTION(m, "serverDescription", serverdb_kvs_fnc_serverDescription);
	KVSM_REGISTER_FUNCTION(m, "serverEncoding", serverdb_kvs_fnc_serverEncoding);
	KVSM_REGISTER_FUNCTION(m, "serverTextEncoding", serverdb_kvs_fnc_serverTextEncoding);
	KVSM_REGISTER_FUNCTION(m, "serverId", serverdb_kvs_fnc_serverId)
	KVSM_REGISTER_FUNCTION(m, "serverIp", serverdb_kvs_fnc_serverIp)
	KVSM_REGISTER_FUNCTION(m, "serverLoginCommand", serverdb_kvs_fnc_serverLoginCommand);
	KVSM_REGISTER_FUNCTION(m, "serverNickName", serverdb_kvs_fnc_serverNickName);
	KVSM_REGISTER_FUNCTION(m, "serverPassword", serverdb_kvs_fnc_serverPassword);
	KVSM_REGISTER_FUNCTION(m, "serverPort", serverdb_kvs_fnc_serverPort);
	KVSM_REGISTER_FUNCTION(m, "serverRealName", serverdb_kvs_fnc_serverRealName);
	KVSM_REGISTER_FUNCTION(m, "serverUserName", serverdb_kvs_fnc_serverUserName);
	KVSM_REGISTER_FUNCTION(m, "serverJoinChannels", serverdb_kvs_fnc_serverJoinChannels);
	KVSM_REGISTER_FUNCTION(m, "isAutoConnect", serverdb_kvs_fnc_serverAutoConnect);
	KVSM_REGISTER_FUNCTION(m, "isIPv6", serverdb_kvs_fnc_serverIPv6);
	KVSM_REGISTER_FUNCTION(m, "isSSL", serverdb_kvs_fnc_serverSSL);
	KVSM_REGISTER_FUNCTION(m, "networkExists", serverdb_kvs_fnc_networkExists);
	KVSM_REGISTER_FUNCTION(m, "serverExists", serverdb_kvs_fnc_serverExists);

	KVSM_REGISTER_SIMPLE_COMMAND(m, "addNetwork", serverdb_kvs_cmd_addNetwork);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "addServer", serverdb_kvs_cmd_addServer);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setNetworkConnectCommand", serverdb_kvs_cmd_setNetworkConnectCommand);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setNetworkEncoding", serverdb_kvs_cmd_setNetworkEncoding);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setNetworkTextEncoding", serverdb_kvs_cmd_setNetworkTextEncoding);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setNetworkDescription", serverdb_kvs_cmd_setNetworkDescription);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setNetworkLoginCommand", serverdb_kvs_cmd_setNetworkLoginCommand);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setNetworkNickName", serverdb_kvs_cmd_setNetworkNickName);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setNetworkRealName", serverdb_kvs_cmd_setNetworkRealName);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setNetworkUserName", serverdb_kvs_cmd_setNetworkUserName);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setNetworkJoinChannels", serverdb_kvs_cmd_setNetworkJoinChannels);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setServerConnectCommand", serverdb_kvs_cmd_setServerConnectCommand);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setServerEncoding", serverdb_kvs_cmd_setServerEncoding);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setServerTextEncoding", serverdb_kvs_cmd_setServerTextEncoding);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setServerDescription", serverdb_kvs_cmd_setServerDescription);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setServerLoginCommand", serverdb_kvs_cmd_setServerLoginCommand);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setServerNickName", serverdb_kvs_cmd_setServerNickName);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setServerRealName", serverdb_kvs_cmd_setServerRealName);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setServerUserName", serverdb_kvs_cmd_setServerUserName);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setServerJoinChannels", serverdb_kvs_cmd_setServerJoinChannels);

	return true;
}

static bool serverdb_module_cleanup(KviModule *)
{
	return true;
}

static bool serverdb_module_can_unload(KviModule *)
{
	return true;
}

KVIRC_MODULE(
    "ServerDB",                                                        // module name
    "4.0.0",                                                           // module version
    "Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)", // author & (C)
    "IRC serverDB related functions",
    serverdb_module_init,
    serverdb_module_can_unload,
    0,
    serverdb_module_cleanup,
    "serverdb")
