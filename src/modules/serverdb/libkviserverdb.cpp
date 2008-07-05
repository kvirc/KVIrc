//
//   File : libkviserverdb.cpp
//   Creation date : Tue Jul  1 01:48:49 2008 GMT by Elvio Basello
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2008 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

#include "kvi_module.h"
#include "kvi_app.h"
#include "kvi_locale.h"
#include "kvi_kvs_script.h"
#include "kvi_ircserver.h"
#include "kvi_ircserverdb.h"

#include <QString>

extern KVIRC_API KviServerDataBase * g_pIrcServerDataBase;

/*
	@doc: serverdb.networkExists
	@type:
		function
	@title:
		$serverdb.networkExists
	@short:
		Checks if the network already exists in the DB
	@synthax:
		<bool> $serverdb.networkExists
	@description:
		Checks if the network already exists in the DB.[br]
		It returns 1 if the network exixts, 0 otherwise
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
static bool serverdb_kvs_fnc_networkExists(KviKvsModuleFunctionCall * c)
{
	QString szNetName, szNetwork;

	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("network_name",KVS_PT_STRING,0,szNetName)
	KVSM_PARAMETERS_END(c)

	if(szNetName.isEmpty())
	{
		c->error(__tr2qs_ctx("You must provide the network name as parameter","serverdb"));
		return false;
	}

	KviNetwork * pNetwork = g_pIrcServerDataBase->findNetwork(szNetName);
	if(!pNetwork)
	{
		c->returnValue()->setBoolean(false);
		return true;
	}

	c->returnValue()->setBoolean(true);

	return true;
}


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
		If the network already exixts, an error is printed unless the -q switch is used.
	@switches:
		!sw: -a | --autoconnect
		Autoconnect this network at KVIrc start.
	@examples:
		[example]
		if(![fnc]$serverdb.networkExists[/fnc](Freenode)) serverdb.addNetwork -q Freenode
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
static bool serverdb_kvs_cmd_addNetwork(KviKvsModuleCommandCall * c)
{
	QString szNetName, szNetwork;

	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("network_name",KVS_PT_STRING,0,szNetName)
	KVSM_PARAMETERS_END(c)

	if(szNetName.isEmpty())
	{
		c->error(__tr2qs_ctx("You must provide the network name as parameter","serverdb"));
		return false;
	}

	KviNetwork * pNetwork = g_pIrcServerDataBase->findNetwork(szNetName);
	if(pNetwork)
	{
		if(c->switches()->find('q',"quiet")) return true;
		c->error(__tr2qs_ctx("The network specified already exists","serverdb"));
		return false;
	}

	pNetwork = new KviNetwork(szNetName);
	if(c->switches()->find('a',"autoconnect")) pNetwork->setAutoConnect(true);

	KviServerDataBaseRecord * pRecord = g_pIrcServerDataBase->insertNetwork(pNetwork);
	if(!pRecord)
	{
		c->error("Internal error: KVIrc author screwed up","serverdb");
		return false;
	}

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
		serverdb.addServer [switches] <string:network> <string:server>
	@description:
		Adds the server <server> to the network <network>.
	@switches:
		!sw: -a | --autoconnect
		Autoconnect this server at KVIrc start.[br]

		!sw: -c | --cache-ip
		Caches the server IP at first connect to save bandwitch for future connects.[br]

		!sw: -i | --ipv6
		Use IPv6 socket to connect to the server.[br]

		!sw: -s | --ssl
		Use SSL (Secure Socket Layer) socket to connect to the server.[br]

		!sw: -p=<port> | --port=<port>
		Use the port <port> to connect to the server.[br]

		!sw: -q | --quiet
		Do not print errors if the server already exist.[br]

		!sw: -s | --ssl
		Use SSL (Secure Socket Layer) socket to connect to the server.

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
		KVSM_PARAMETER("network_name",KVS_PT_STRING,0,szNetwork)
		KVSM_PARAMETER("server_name",KVS_PT_STRING,0,szServer)
	KVSM_PARAMETERS_END(c)

	if(szNetwork.isEmpty())
	{
		c->error(__tr2qs_ctx("You must provide the network name as parameter","serverdb"));
		return false;
	}

	if(szServer.isEmpty())
	{
		c->error(__tr2qs_ctx("You must provide the server name as parameter","serverdb"));
		return false;
	}

	KviServerDataBaseRecord * pRecord = g_pIrcServerDataBase->findRecord(szNetwork);
	if(!pRecord)
	{
		// FIXME: default to orphan servers
		c->error(__tr2qs_ctx("The specified network does not exist","serverdb"));
		return false;
	}

	KviServer * pServer = new KviServer();
	pServer->setHostName(szServer);

	KviServer * pServerRecord = pRecord->findServer(pServer);
	if(pServerRecord)
	{
		if(c->switches()->find('q',"quiet")) return true;
		c->error(__tr2qs_ctx("The specified server already exists","serverdb"));
		return false;
	}

	if(c->switches()->find('a',"autoconnect")) pServer->setAutoConnect(true);
	if(c->switches()->find('c',"cache-ip")) pServer->setCacheIp(true);
	if(c->switches()->find('i',"ipv6")) pServer->setIPv6(true);
	if(c->switches()->find('s',"ssl")) pServer->setUseSSL(true);

	QString tmp;
	int iPort;
	if(c->switches()->getAsStringIfExisting('p',"port",tmp))
	{
		bool bOk;
		iPort = tmp.toInt(&bOk);
		if(!bOk) iPort = 6667;
		pServer->setPort(iPort);
	}

	debug("Network: %s\nServer: %s\nPort: %d",szNetwork.toUtf8().data(),szServer.toUtf8().data(),iPort);
	pRecord->insertServer(pServer);

	return true;
}

#define SERVERDB_SET_PROPERTY(__functionName,__callName) \
	static bool __functionName(KviKvsModuleCommandCall * c) \
	{ \
		QString szName, szPropertyName; \
		bool bServer = false; \
		\
		KVSM_PARAMETERS_BEGIN(c) \
			KVSM_PARAMETER("name",KVS_PT_STRING,0,szName) \
			KVSM_PARAMETER("property",KVS_PT_STRING,KVS_PF_APPENDREMAINING,szPropertyName) \
		KVSM_PARAMETERS_END(c) \
		\
		if(c->switches()->find('s',"server")) bServer = true; \
		\
		if(szName.isEmpty()) \
		{ \
			c->error(__tr2qs_ctx("You must provide the network/server name as parameter","serverdb")); \
			return false; \
		} \
		\
		if(szPropertyName.isEmpty()) \
		{ \
			c->error(__tr2qs_ctx("You must provide the nickname as parameter","serverdb")); \
			return false; \
		} \
		\
		if(!bServer) \
		{ \
			/* We want to set the nickname for the network*/ \
			KviNetwork * pNetwork = g_pIrcServerDataBase->findNetwork(szName); \
			if(!pNetwork) \
			{ \
				if(c->switches()->find('q',"quiet")) return true; \
				c->error(__tr2qs_ctx("The specified network does not exist","serverdb")); \
				return false; \
			} \
			\
			pNetwork->__callName(szPropertyName); \
		} else { \
			/* We want to set the nickname for the server*/ \
			KviServerDataBaseRecord * pRecord = g_pIrcServerDataBase->findRecord(szName); \
			if(!pRecord) \
			{ \
				if(c->switches()->find('q',"quiet")) return true; \
				c->error(__tr2qs_ctx("The specified server does not exist","serverdb")); \
				return false; \
			} \
			\
			/* Ensure we are manipulating a server*/ \
			KviServer * pServer = pRecord->currentServer(); \
			if(!pServer) \
			{ \
				if(c->switches()->find('q',"quiet")) return true; \
				c->error(__tr2qs_ctx("The specified server does not exist","serverdb")); \
				return false; \
			} \
			\
			pServer->__callName(szPropertyName); \
		} \
		\
		return true; \
	}

/*
	@doc: serverdb.setNickName
	@type:
		command
	@title:
		serverdb.setNickName
	@short:
		Sets the nickname
	@syntax:
		serverdb.setNickName [switches] <string:name> <string:nick>
	@description:
		Sets the nickname <nick> for the specified network/server <name>.
	@switches:
		!sw: -s | --server
		Sets the nickname for the server.[br]
		If this switch is not passed, the command will set the nickname for the network.[br]

		!sw: -q | --quiet
		Do not print errors if the network/server already exist.
	@examples:
		[example]
		[comment]Quietly sets the nickname HelLViS69 for the Freenode network[/comment][br]
		serverdb.setNickName -q Freenode HelLViS69[br]
		[comment]Sets the nickname HelLViS69 for the server irc.freenode.net[/comment][br]
		serverdb.setNickName -s irc.freenode.net HelLViS69
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
SERVERDB_SET_PROPERTY(serverdb_kvs_cmd_setNickName,setNickName)

/*
	@doc: serverdb.setUserName
	@type:
		command
	@title:
		serverdb.setUserName
	@short:
		Sets the username
	@syntax:
		serverdb.setUserName [switches] <string:name> <string:username>
	@description:
		Sets the username <username> for the specified network/server <name>.
	@switches:
		!sw: -s | --server
		Sets the username for the server.[br]
		If this switch is not passed, the command will set the username for the network.[br]

		!sw: -q | --quiet
		Do not print errors if the network/server already exist.
	@examples:
		[example]
		[comment]Quietly sets the username kvirc for the Freenode network[/comment][br]
		serverdb.setUserName -q Freenode kvirc[br]
		[comment]Sets the username kvirc for the server irc.freenode.net[/comment][br]
		serverdb.setUserName -s irc.freenode.net kvirc
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
SERVERDB_SET_PROPERTY(serverdb_kvs_cmd_setUserName,setUserName)

/*
	@doc: serverdb.setRealName
	@type:
		command
	@title:
		serverdb.setRealName
	@short:
		Sets the realname
	@syntax:
		serverdb.setRealName [switches] <string:name> <string:realname>
	@description:
		Sets the realname <realname> for the specified network/server <name>.
	@switches:
		!sw: -s | --server
		Sets the realname for the server.[br]
		If this switch is not passed, the command will set the realname for the network.[br]

		!sw: -q | --quiet
		Do not print errors if the network/server already exist.
	@examples:
		[example]
		[comment]Quietly sets the realname KVIrc 4.0 for the Freenode network[/comment][br]
		serverdb.setRealName -q Freenode KVIrc 4.0[br]
		[comment]Sets the realname KVIrc 4.0 for the server irc.freenode.net[/comment][br]
		serverdb.setRealName -s irc.freenode.net KVIrc 4.0
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
SERVERDB_SET_PROPERTY(serverdb_kvs_cmd_setRealName,setRealName)

/*
	@doc: serverdb.setEncoding
	@type:
		command
	@title:
		serverdb.setEncoding
	@short:
		Sets the encoding
	@syntax:
		serverdb.setEncoding [switches] <string:name> <string:encoding>
	@description:
		Sets the encoding <encoding> for the specified network/server <name>.
	@switches:
		!sw: -s | --server
		Sets the encoding for the server.[br]
		If this switch is not passed, the command will set the encoding for the network.[br]

		!sw: -q | --quiet
		Do not print errors if the network/server already exist.
	@examples:
		[example]
		[comment]Quietly sets the encoding UTF-8 for the Freenode network[/comment][br]
		serverdb.setEncoding -q Freenode UTF-8[br]
		[comment]Sets the realname UTF-8 for the server irc.freenode.net[/comment][br]
		serverdb.setEncoding -s irc.freenode.net UTF-8
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
SERVERDB_SET_PROPERTY(serverdb_kvs_cmd_setEncoding,setEncoding)

/*
	@doc: serverdb.setDescription
	@type:
		command
	@title:
		serverdb.setDescription
	@short:
		Sets the description
	@syntax:
		serverdb.setDescription [switches] <string:name> <string:description>
	@description:
		Sets the description <description> for the specified network/server <name>.
	@switches:
		!sw: -s | --server
		Sets the description for the server.[br]
		If this switch is not passed, the command will set the description for the network.[br]

		!sw: -q | --quiet
		Do not print errors if the network/server already exist.
	@examples:
		[example]
		[comment]Quietly sets the description for the Freenode network[/comment][br]
		serverdb.setDescription -q Freenode The FOSS Network[br]
		[comment]Sets the description for the server irc.freenode.net[/comment][br]
		serverdb.setDescription -s irc.freenode.net Main network router
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
SERVERDB_SET_PROPERTY(serverdb_kvs_cmd_setDescription,setDescription)

/*
	@doc: serverdb.setConnectCommand
	@type:
		command
	@title:
		serverdb.setConnectCommand
	@short:
		Sets the connect command
	@syntax:
		serverdb.setConnectCommand [switches] <string:name> <string:command>
	@description:
		Sets the command <command> to run on connect for the specified network/server <name>.
	@switches:
		!sw: -s | --server
		Sets the connect command for the server.[br]
		If this switch is not passed, the command will set the connect command for the network.[br]

		!sw: -q | --quiet
		Do not print errors if the network/server already exist.
	@examples:
		[example]
		[comment]Quietly sets the connect command for the Freenode network[/comment][br]
		serverdb.setConnectCommand -q Freenode ns identify HelLViS69 foobar[br]
		[comment]Sets the connect command for the server irc.freenode.net[/comment][br]
		serverdb.setConnectCommand -s irc.freenode.net ns identify HelLViS69 foobar
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
SERVERDB_SET_PROPERTY(serverdb_kvs_cmd_setConnectCommand,setOnConnectCommand)

/*
	@doc: serverdb.setLoginCommand
	@type:
		command
	@title:
		serverdb.setLoginCommand
	@short:
		Sets the login command
	@syntax:
		serverdb.setLoginCommand [switches] <string:name> <string:command>
	@description:
		Sets the command <command> to run on login for the specified network/server <name>.
	@switches:
		!sw: -s | --server
		Sets the login command for the server.[br]
		If this switch is not passed, the command will set the login command for the network.[br]

		!sw: -q | --quiet
		Do not print errors if the network/server already exist.
	@examples:
		[example]
		[comment]Quietly sets the login command for the Freenode network[/comment][br]
		serverdb.setLoginCommand -q Freenode join #KVIrc[br]
		[comment]Sets the login command for the server irc.freenode.net[/comment][br]
		serverdb.setLoginCommand -s irc.freenode.net join #KVIrc
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
SERVERDB_SET_PROPERTY(serverdb_kvs_cmd_setLoginCommand,setOnLoginCommand)

static bool serverdb_module_init(KviModule * m)
{
	/*
	KVSM_REGISTER_FUNCTION(m,"findByName",serverdb_kvs_fnc_findByName);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"updateList",serverdb_kvs_cmd_updateList);
	*/

	KVSM_REGISTER_FUNCTION(m,"networkExists",serverdb_kvs_fnc_networkExists);

	KVSM_REGISTER_SIMPLE_COMMAND(m,"addNetwork",serverdb_kvs_cmd_addNetwork);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"addServer",serverdb_kvs_cmd_addServer);

	KVSM_REGISTER_SIMPLE_COMMAND(m,"setConnectCommand",serverdb_kvs_cmd_setConnectCommand);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"setEncoding",serverdb_kvs_cmd_setEncoding);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"setDescription",serverdb_kvs_cmd_setDescription);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"setLoginCommand",serverdb_kvs_cmd_setLoginCommand);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"setNickName",serverdb_kvs_cmd_setNickName);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"setRealName",serverdb_kvs_cmd_setRealName);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"setUserName",serverdb_kvs_cmd_setUserName);

/*
<Pragma[Off2]> addNetwork "nome"
<Pragma[Off2]> addServer "nomenetwork" "nomeserver" "altriparametri"....
<Pragma[Off2]> setQualcosa "nomenetwork" "nomeserver" "qualcosa"
*/
	return true;
}

static bool serverdb_module_cleanup(KviModule *m)
{
	return true;
}

static bool serverdb_module_can_unload(KviModule *)
{
	return true;
}

KVIRC_MODULE(
	"ServerDB",                                              // module name
	"4.0.0",                                                // module version
	"Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)", // author & (C)
	"IRC server DB related functions",
	serverdb_module_init,
	serverdb_module_can_unload,
	0,
	serverdb_module_cleanup
)
