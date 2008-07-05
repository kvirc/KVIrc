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

extern KVIRC_API KviServerDataBase * g_pServerDataBase;

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
	QString szNetwork;

	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("network_name",KVS_PT_STRING,0,szNetwork)
	KVSM_PARAMETERS_END(c)

	if(szNetwork.isEmpty())
	{
		c->error(__tr2qs_ctx("You must provide the network name as parameter","serverdb"));
		return false;
	}

	KviNetwork * pNetwork = g_pServerDataBase->findNetwork(szNetwork);
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
	@synthax:
		<bool> $serverdb.serverExists(<string:servername>[,<networkname>])
	@description:
		Checks if the server already exists for a network in the DB.[br]
		If no network name is provided, the check is made globally.[br]
		It returns 1 if the server exixts, 0 otherwise
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
static bool serverdb_kvs_fnc_serverExists(KviKvsModuleFunctionCall * c)
{
	QString szServer, szNetwork;
	bool bCheckAllNets = true;

	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("server_name",KVS_PT_STRING,0,szServer)
		KVSM_PARAMETER("network_name",KVS_PT_STRING,KVS_PF_OPTIONAL,szNetwork)
	KVSM_PARAMETERS_END(c)

	if(szServer.isEmpty())
	{
		c->error(__tr2qs_ctx("You must provide the server name as parameter","serverdb"));
		return false;
	}

	if(!szNetwork.isEmpty())
	{
		// Check in the given network
		KviServerDataBaseRecord * pRecord = g_pServerDataBase->findRecord(szNetwork);
		if(!pRecord)
		{
			debug("pRecord");
			c->returnValue()->setBoolean(false);
			return true;
		}

		KviServer * pServer = new KviServer();
		pServer->setHostName(szServer);
		debug("Server: %s",pServer->hostName().toUtf8().data());

		KviServer * pCheckServer = pRecord->findServer(pServer,true);
		if(!pCheckServer)
		{
			debug("pCheckServer");
			c->returnValue()->setBoolean(false);
			return true;
		}

		debug("Check server: %s",pCheckServer->hostName().toUtf8().data());
		c->returnValue()->setBoolean(true);
	} else {
		// FIXME: Check through all networks
		/*
		KviPointerHashTableIterator<QString,KviServerDataBaseRecord> it(*(g_pServerDataBase->recordDict()));
	
		while(KviServerDataBaseRecord * r = it.current())
		{
			net = new KviServerOptionsTreeWidgetItem(m_pTreeWidget,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_WORLD)),r->network());
			KviPointerList<KviServer> * sl = r->serverList();
			bool bCurrent = r->network()->name() == g_pServerDataBase->currentNetworkName().toUtf8().data();
			net->setExpanded(bCurrent);
			for(KviServer * s = sl->first();s;s = sl->next())
			{
				srv = new KviServerOptionsTreeWidgetItem(net,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SERVER)),s);
	
				if((s == r->currentServer()) && bCurrent)
				{
					srv->setSelected(true);
					cur = srv;
				}
			}
			++it;
		}
		*/
	}

	//c->returnValue()->setBoolean(true);
	return true;
}

#if 0
#define SERVERDB_GET_PROPERTY(__functionName,__callName) \
	static bool __functionName(KviKvsModuleFunctionCall * c) \
	{ \
		QString szName; \
		bool bServer = false; \
		\
		KVSM_PARAMETERS_BEGIN(c) \
			KVSM_PARAMETER("name",KVS_PT_STRING,0,szName) \
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
		if(!bServer) \
		{ \
			/* We want to get the property for the network*/ \
			KviNetwork * pNetwork = g_pServerDataBase->findNetwork(szName); \
			if(!pNetwork) \
			{ \
				if(c->switches()->find('q',"quiet")) return true; \
				c->error(__tr2qs_ctx("The specified network does not exist","serverdb")); \
				return false; \
			} \
			\
			c->returnValue()->setString(pNetwork->__callName()); \
		} else { \
			/* We want to get the property for the server*/ \
			KviServerDataBaseRecord * pRecord = g_pServerDataBase->findRecord(szName); \
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
			c->returnValue()->setString(pServer->__callName()); \
		} \
		\
		return true; \
	}


SERVERDB_GET_PROPERTY(serverdb_kvs_fnc_getNickName,nickName)
SERVERDB_GET_PROPERTY(serverdb_kvs_fnc_getUserName,userName)
SERVERDB_GET_PROPERTY(serverdb_kvs_fnc_getRealName,realName)
SERVERDB_GET_PROPERTY(serverdb_kvs_fnc_getEncoding,encoding)
SERVERDB_GET_PROPERTY(serverdb_kvs_fnc_getDescription,description)
SERVERDB_GET_PROPERTY(serverdb_kvs_fnc_getConnectCommand,onConnectCommand)
SERVERDB_GET_PROPERTY(serverdb_kvs_fnc_getLoginCommand,onLoginCommand)
#endif

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

	KviNetwork * pNetwork = g_pServerDataBase->findNetwork(szNetName);
	if(pNetwork)
	{
		if(c->switches()->find('q',"quiet")) return true;
		c->error(__tr2qs_ctx("The network specified already exists","serverdb"));
		return false;
	}

	pNetwork = new KviNetwork(szNetName);
	if(c->switches()->find('a',"autoconnect")) pNetwork->setAutoConnect(true);

	KviServerDataBaseRecord * pRecord = g_pServerDataBase->insertNetwork(pNetwork);
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

	KviServerDataBaseRecord * pRecord = g_pServerDataBase->findRecord(szNetwork);
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

	unsigned int uPort;
	if(c->switches()->getAsStringIfExisting('p',"port",tmp))
	{
		bool bOk;
		uPort = tmp.toInt(&bOk);
		if(!bOk) uPort = 6667;
		pServer->setPort(uPort);
	}

	if(c->switches()->getAsStringIfExisting('w',"password",tmp)) pServer->setPassword(tmp);

	debug("Network: %s\nServer: %s\nPort: %d",szNetwork.toUtf8().data(),szServer.toUtf8().data(),uPort);
	pRecord->insertServer(pServer);

	return true;
}

#define SERVERDB_SET_NETWORK_PROPERTY(__functionName,__callName) \
	static bool __functionName(KviKvsModuleCommandCall * c) \
	{ \
		QString szName, szPropertyName; \
		\
		KVSM_PARAMETERS_BEGIN(c) \
			KVSM_PARAMETER("name",KVS_PT_STRING,0,szName) \
			KVSM_PARAMETER("property",KVS_PT_STRING,KVS_PF_APPENDREMAINING,szPropertyName) \
		KVSM_PARAMETERS_END(c) \
		\
		if(szName.isEmpty()) \
		{ \
			c->error(__tr2qs_ctx("You must provide the network name as parameter","serverdb")); \
			return false; \
		} \
		\
		if(szPropertyName.isEmpty()) \
		{ \
			c->error(__tr2qs_ctx("You must provide the value as parameter","serverdb")); \
			return false; \
		} \
		\
		KviNetwork * pNetwork = g_pServerDataBase->findNetwork(szName); \
		if(!pNetwork) \
		{ \
			if(c->switches()->find('q',"quiet")) return true; \
			c->error(__tr2qs_ctx("The specified network does not exist","serverdb")); \
			return false; \
		} \
		\
		pNetwork->__callName(szPropertyName); \
		\
		return true; \
	}

#define SERVERDB_SET_SERVER_PROPERTY(__functionName,__callName) \
	static bool __functionName(KviKvsModuleCommandCall * c) \
	{ \
		QString szName, szPropertyName; \
		\
		KVSM_PARAMETERS_BEGIN(c) \
			KVSM_PARAMETER("name",KVS_PT_STRING,0,szName) \
			KVSM_PARAMETER("property",KVS_PT_STRING,KVS_PF_APPENDREMAINING,szPropertyName) \
		KVSM_PARAMETERS_END(c) \
		\
		if(szName.isEmpty()) \
		{ \
			c->error(__tr2qs_ctx("You must provide the network name as parameter","serverdb")); \
			return false; \
		} \
		\
		if(szPropertyName.isEmpty()) \
		{ \
			c->error(__tr2qs_ctx("You must provide the value as parameter","serverdb")); \
			return false; \
		} \
		\
		KviServerDataBaseRecord * pRecord = g_pServerDataBase->findRecord(szName); \
		if(!pRecord) \
		{ \
			if(c->switches()->find('q',"quiet")) return true; \
			c->error(__tr2qs_ctx("The specified server does not exist","serverdb")); \
			return false; \
		} \
		\
		KviServer * pServer = pRecord->currentServer(); \
		if(!pServer) \
		{ \
			if(c->switches()->find('q',"quiet")) return true; \
			c->error(__tr2qs_ctx("The specified server does not exist","serverdb")); \
			return false; \
		} \
		\
		pServer->__callName(szPropertyName); \
		\
		return true; \
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
		serverdb.setNetworkNickName [switches] <string:name> <string:nick>
	@description:
		Sets the nickname <nick> for the specified network <name>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the network already exists.
	@examples:
		[example]
		[comment]Quietly sets the nickname HelLViS69 for the Freenode network[/comment][br]
		serverdb.setNetworkNickName -q Freenode HelLViS69
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
SERVERDB_SET_NETWORK_PROPERTY(serverdb_kvs_cmd_setNetworkNickName,setNickName)

/*
	@doc: serverdb.setNetworkUserName
	@type:
		command
	@title:
		serverdb.setNetworkUserName
	@short:
		Sets the username
	@syntax:
		serverdb.setNetworkUserName [switches] <string:name> <string:username>
	@description:
		Sets the username <username> for the specified network <name>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the network already exists.
	@examples:
		[example]
		[comment]Quietly sets the username kvirc for the Freenode network[/comment][br]
		serverdb.setNetworkUserName -q Freenode kvirc
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
SERVERDB_SET_NETWORK_PROPERTY(serverdb_kvs_cmd_setNetworkUserName,setUserName)

/*
	@doc: serverdb.setNetworkRealName
	@type:
		command
	@title:
		serverdb.setNetworkRealName
	@short:
		Sets the realname
	@syntax:
		serverdb.setNetworkRealName [switches] <string:name> <string:realname>
	@description:
		Sets the realname <realname> for the specified network <name>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the network already exists.
	@examples:
		[example]
		[comment]Quietly sets the realname KVIrc 4.0 for the Freenode network[/comment][br]
		serverdb.setNetworkRealName -q Freenode KVIrc 4.0
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
SERVERDB_SET_NETWORK_PROPERTY(serverdb_kvs_cmd_setNetworkRealName,setRealName)

/*
	@doc: serverdb.setNetworkEncoding
	@type:
		command
	@title:
		serverdb.setNetworkEncoding
	@short:
		Sets the encoding
	@syntax:
		serverdb.setNetworkEncoding [switches] <string:name> <string:encoding>
	@description:
		Sets the encoding <encoding> for the specified network <name>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the network already exists.
	@examples:
		[example]
		[comment]Quietly sets the encoding UTF-8 for the Freenode network[/comment][br]
		serverdb.setNetworkEncoding -q Freenode UTF-8
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
SERVERDB_SET_NETWORK_PROPERTY(serverdb_kvs_cmd_setNetworkEncoding,setEncoding)

/*
	@doc: serverdb.setNetworkDescription
	@type:
		command
	@title:
		serverdb.setNetworkDescription
	@short:
		Sets the description
	@syntax:
		serverdb.setNetworkDescription [switches] <string:name> <string:description>
	@description:
		Sets the description <description> for the specified network <name>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the network already exists.
	@examples:
		[example]
		[comment]Quietly sets the description for the Freenode network[/comment][br]
		serverdb.setNetworkDescription -q Freenode The FOSS Network
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
SERVERDB_SET_NETWORK_PROPERTY(serverdb_kvs_cmd_setNetworkDescription,setDescription)

/*
	@doc: serverdb.setNetworkConnectCommand
	@type:
		command
	@title:
		serverdb.setNetworkConnectCommand
	@short:
		Sets the connect command
	@syntax:
		serverdb.setNetworkConnectCommand [switches] <string:name> <string:command>
	@description:
		Sets the command <command> to run on connect for the specified network <name>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the network already exists.
	@examples:
		[example]
		[comment]Quietly sets the connect command for the Freenode network[/comment][br]
		serverdb.setNetworkConnectCommand -q Freenode ns identify HelLViS69 foobar
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
SERVERDB_SET_NETWORK_PROPERTY(serverdb_kvs_cmd_setNetworkConnectCommand,setOnConnectCommand)

/*
	@doc: serverdb.setNetworkLoginCommand
	@type:
		command
	@title:
		serverdb.setNetworkLoginCommand
	@short:
		Sets the login command
	@syntax:
		serverdb.setNetworkLoginCommand [switches] <string:name> <string:command>
	@description:
		Sets the command <command> to run on login for the specified network <name>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the network already exists.
	@examples:
		[example]
		[comment]Quietly sets the login command for the Freenode network[/comment][br]
		serverdb.setNetworkLoginCommand -q Freenode join #KVIrc
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
SERVERDB_SET_NETWORK_PROPERTY(serverdb_kvs_cmd_setNetworkLoginCommand,setOnLoginCommand)

/*
	@doc: serverdb.setServerNickName
	@type:
		command
	@title:
		serverdb.setServerNickName
	@short:
		Sets the nickname
	@syntax:
		serverdb.setServerNickName [switches] <string:name> <string:nick>
	@description:
		Sets the nickname <nick> for the specified server <name>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the server already exists.
	@examples:
		[example]
		[comment]Quietly sets the nickname HelLViS69 for the Freenode network[/comment][br]
		serverdb.setServerNickName -q irc.freenode.net HelLViS69
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
SERVERDB_SET_SERVER_PROPERTY(serverdb_kvs_cmd_setServerNickName,setNickName)

/*
	@doc: serverdb.setServerUserName
	@type:
		command
	@title:
		serverdb.setServerUserName
	@short:
		Sets the username
	@syntax:
		serverdb.setServerUserName [switches] <string:name> <string:username>
	@description:
		Sets the username <username> for the specified server <name>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the server already exists.
	@examples:
		[example]
		[comment]Quietly sets the username kvirc for the Freenode server[/comment][br]
		serverdb.setServerUserName -q irc.freenode.net kvirc
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
SERVERDB_SET_SERVER_PROPERTY(serverdb_kvs_cmd_setServerUserName,setUserName)

/*
	@doc: serverdb.setServerRealName
	@type:
		command
	@title:
		serverdb.setServerRealName
	@short:
		Sets the realname
	@syntax:
		serverdb.setServerRealName [switches] <string:name> <string:realname>
	@description:
		Sets the realname <realname> for the specified server <name>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the server already exists.
	@examples:
		[example]
		[comment]Quietly sets the realname KVIrc 4.0 for the Freenode server[/comment][br]
		serverdb.setServerRealName -q irc.freenode.net KVIrc 4.0
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
SERVERDB_SET_SERVER_PROPERTY(serverdb_kvs_cmd_setServerRealName,setRealName)

/*
	@doc: serverdb.setServerEncoding
	@type:
		command
	@title:
		serverdb.setServerEncoding
	@short:
		Sets the encoding
	@syntax:
		serverdb.setServerEncoding [switches] <string:name> <string:encoding>
	@description:
		Sets the encoding <encoding> for the specified server <name>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the server already exists.
	@examples:
		[example]
		[comment]Quietly sets the encoding UTF-8 for the irc.freenode.net server[/comment][br]
		serverdb.setServerEncoding -q Freenode UTF-8
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
SERVERDB_SET_SERVER_PROPERTY(serverdb_kvs_cmd_setServerEncoding,setEncoding)

/*
	@doc: serverdb.setServerDescription
	@type:
		command
	@title:
		serverdb.setServerDescription
	@short:
		Sets the description
	@syntax:
		serverdb.setServerDescription [switches] <string:name> <string:description>
	@description:
		Sets the description <description> for the specified server <name>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the server already exists.
	@examples:
		[example]
		[comment]Quietly sets the description for the Freenode server[/comment][br]
		serverdb.setServerDescription -q irc.freenode.net The FOSS Network
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
SERVERDB_SET_SERVER_PROPERTY(serverdb_kvs_cmd_setServerDescription,setDescription)

/*
	@doc: serverdb.setServerConnectCommand
	@type:
		command
	@title:
		serverdb.setServerConnectCommand
	@short:
		Sets the connect command
	@syntax:
		serverdb.setServerConnectCommand [switches] <string:name> <string:command>
	@description:
		Sets the command <command> to run on connect for the specified server <name>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the server already exists.
	@examples:
		[example]
		[comment]Quietly sets the connect command for the Freenode server[/comment][br]
		serverdb.setServerConnectCommand -q irc.freenode.net ns identify HelLViS69 foobar
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
SERVERDB_SET_SERVER_PROPERTY(serverdb_kvs_cmd_setServerConnectCommand,setOnConnectCommand)

/*
	@doc: serverdb.setServerLoginCommand
	@type:
		command
	@title:
		serverdb.setServerLoginCommand
	@short:
		Sets the login command
	@syntax:
		serverdb.setServerLoginCommand [switches] <string:name> <string:command>
	@description:
		Sets the command <command> to run on login for the specified server <name>.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the server already exists.
	@examples:
		[example]
		[comment]Quietly sets the login command for the Freenode server[/comment][br]
		serverdb.setServerLoginCommand -q irc.freenode.net join #KVIrc
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
SERVERDB_SET_SERVER_PROPERTY(serverdb_kvs_cmd_setServerLoginCommand,setOnLoginCommand)

static bool serverdb_module_init(KviModule * m)
{
	/*
	KVSM_REGISTER_SIMPLE_COMMAND(m,"updateList",serverdb_kvs_cmd_updateList);
	*/
#if 0
	KVSM_REGISTER_FUNCTION(m,"getNickName",serverdb_kvs_fnc_getNickName);
	KVSM_REGISTER_FUNCTION(m,"getRealName",serverdb_kvs_fnc_getRealName);
	KVSM_REGISTER_FUNCTION(m,"getUserName",serverdb_kvs_fnc_getUserName);
	KVSM_REGISTER_FUNCTION(m,"getConnectCommand",serverdb_kvs_fnc_getConnectCommand);
	KVSM_REGISTER_FUNCTION(m,"getDescription",serverdb_kvs_fnc_getDescription);
	KVSM_REGISTER_FUNCTION(m,"getEncoding",serverdb_kvs_fnc_getEncoding);
	KVSM_REGISTER_FUNCTION(m,"getLoginCommand",serverdb_kvs_fnc_getLoginCommand);
#endif
	KVSM_REGISTER_FUNCTION(m,"networkExists",serverdb_kvs_fnc_networkExists);
	KVSM_REGISTER_FUNCTION(m,"serverExists",serverdb_kvs_fnc_serverExists);

	KVSM_REGISTER_SIMPLE_COMMAND(m,"addNetwork",serverdb_kvs_cmd_addNetwork);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"addServer",serverdb_kvs_cmd_addServer);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"setNetworkConnectCommand",serverdb_kvs_cmd_setNetworkConnectCommand);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"setNetworkEncoding",serverdb_kvs_cmd_setNetworkEncoding);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"setNetworkDescription",serverdb_kvs_cmd_setNetworkDescription);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"setNetworkLoginCommand",serverdb_kvs_cmd_setNetworkLoginCommand);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"setNetworkNickName",serverdb_kvs_cmd_setNetworkNickName);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"setNetworkRealName",serverdb_kvs_cmd_setNetworkRealName);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"setNetworkUserName",serverdb_kvs_cmd_setNetworkUserName);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"setServerConnectCommand",serverdb_kvs_cmd_setServerConnectCommand);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"setServerEncoding",serverdb_kvs_cmd_setServerEncoding);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"setServerDescription",serverdb_kvs_cmd_setServerDescription);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"setServerLoginCommand",serverdb_kvs_cmd_setServerLoginCommand);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"setServerNickName",serverdb_kvs_cmd_setServerNickName);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"setServerRealName",serverdb_kvs_cmd_setServerRealName);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"setServerUserName",serverdb_kvs_cmd_setServerUserName);

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
