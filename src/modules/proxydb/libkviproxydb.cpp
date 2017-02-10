//=============================================================================
//
//   File : libkviproxydb.cpp
//   Creation date : Wed Jul 16 16:36:27 2008 GMT by Elvio Basello
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
#include "KviProxy.h"
#include "KviProxyDataBase.h"

#include <QString>
#include <QHostAddress>

#include <memory>
#include <utility>

extern KVIRC_API KviProxyDataBase * g_pProxyDataBase;

/*
	@doc: proxydb.protocol
	@type:
		function
	@title:
		$proxydb.protocol
	@short:
		Returns the protocol
	@syntax:
		<string> $proxydb.protocol(<string:proxy>)
	@description:
		Returns the protocol of the proxy <proxy>
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

static bool proxydb_kvs_fnc_protocol(KviKvsModuleFunctionCall * c)
{
	QString szProxy;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("proxy", KVS_PT_STRING, 0, szProxy)
	KVSM_PARAMETERS_END(c)

	if(szProxy.isEmpty())
	{
		c->error(__tr2qs_ctx("You must provide the proxy hostname as parameter", "serverdb"));
		return false;
	}

	KviProxy objProxy;
	objProxy.setHostname(szProxy);

	KviProxy * pRecord = g_pProxyDataBase->findProxy(&objProxy, true);
	if(!pRecord)
	{
		c->error(__tr2qs_ctx("The specified proxy doesn't exist", "serverdb"));
		return false;
	}

	QString szProtocol;

	switch(pRecord->protocol())
	{
		case KviProxy::Socks4:
			szProtocol = "Socks4";
			break;
		case KviProxy::Socks5:
			szProtocol = "Socks5";
			break;
		case KviProxy::Http:
			szProtocol = "Http";
			break;
	}

	c->returnValue()->setString(szProtocol);

	return true;
}

static bool proxydb_get_helperer(KviKvsModuleFunctionCall * c, QString * szProxy)
{
	QString szTmp;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("proxy", KVS_PT_STRING, 0, szTmp)
	KVSM_PARAMETERS_END(c)

	*szProxy = szTmp;
	return true;
}

static KviProxy * proxydb_get_helper(KviKvsModuleFunctionCall * c)
{
	QString szProxy;

	if(!proxydb_get_helperer(c, &szProxy))
		return nullptr;

	if(szProxy.isEmpty())
	{
		c->error(__tr2qs_ctx("You must provide the proxy hostname as parameter", "serverdb"));
		return nullptr;
	}

	KviProxy objProxy;
	objProxy.setHostname(szProxy);

	KviProxy * pRecord = g_pProxyDataBase->findProxy(&objProxy, true);
	if(!pRecord)
	{
		c->error(__tr2qs_ctx("The specified proxy doesn't exist", "serverdb"));
		return nullptr;
	}

	return pRecord;
}

#define PROXYDB_GET_PROPERTY(__functionName, __callName, __variantSetCallName) \
	static bool __functionName(KviKvsModuleFunctionCall * c)                   \
	{                                                                          \
		KviProxy * pRecord = proxydb_get_helper(c);                            \
		if(pRecord == nullptr)                                                 \
			return false;                                                      \
		c->returnValue()->__variantSetCallName(pRecord->__callName());         \
		return true;                                                           \
	}

/*
	@doc: proxydb.hostname
	@type:
		function
	@title:
		$proxydb.hostname
	@short:
		Returns the hostname
	@syntax:
		<string> $proxydb.hostname(<string:proxy>)
	@description:
		Returns the hostname of the proxy <proxy>
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

PROXYDB_GET_PROPERTY(proxydb_kvs_fnc_hostname, hostname, setString)

/*
	@doc: proxydb.ip
	@type:
		function
	@title:
		$proxydb.ip
	@short:
		Returns the IP
	@syntax:
		<string> $proxydb.ip(<string:proxy>)
	@description:
		Returns the IP of the proxy <proxy>
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

PROXYDB_GET_PROPERTY(proxydb_kvs_fnc_ip, ip, setString)

/*
	@doc: proxydb.password
	@type:
		function
	@title:
		$proxydb.password
	@short:
		Returns the password
	@syntax:
		<string> $proxydb.password(<string:proxy>)
	@description:
		Returns the password of the proxy <proxy>, if set
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

PROXYDB_GET_PROPERTY(proxydb_kvs_fnc_password, pass, setString)

/*
	@doc: proxydb.username
	@type:
		function
	@title:
		$proxydb.username
	@short:
		Returns the username
	@syntax:
		<string> $proxydb.username(<string:proxy>)
	@description:
		Returns the username of the proxy <proxy>, if set
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/
PROXYDB_GET_PROPERTY(proxydb_kvs_fnc_username, user, setString)

/*
	@doc: proxydb.isIPv6
	@type:
		function
	@title:
		$proxydb.isIPv6
	@short:
		Returns the isIPv6
	@syntax:
		<bool> $proxydb.isIPv6(<string:proxy>)
	@description:
		Returns true if the proxy <proxy> uses IPv6 sockets
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

PROXYDB_GET_PROPERTY(proxydb_kvs_fnc_isIPv6, isIPv6, setBoolean)

/*
	@doc: proxydb.port
	@type:
		function
	@title:
		$proxydb.port
	@short:
		Returns the port
	@syntax:
		<bool> $proxydb.port(<string:proxy>)
	@description:
		Returns the port number of the proxy <proxy>
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

PROXYDB_GET_PROPERTY(proxydb_kvs_fnc_port, port, setInteger)

/*
	@doc: proxydb.addProxy
	@type:
		command
	@title:
		proxydb.addProxy
	@short:
		Adds a server
	@syntax:
		proxydb.addProxy [switches] <string:proxy>
	@description:
		Adds the proxy <proxy> to the proxy database.[br]
		Protocol can be Socks4, Socks5 or Http.
	@switches:
		!sw: -i | --ipv6
		Use IPv6 socket to connect to the proxy.[br]
		!sw: -p=<port> | --port=<port>
		Use the port <port> to connect to the proxy.[br]
		!sw: -q | --quiet
		Do not print errors if the proxy already exist.[br]
		!sw: -r=<protocol> | --protocol=<protocol>
		Use the protocol <protocol> to connect to the proxy.[br]
		!sw: -u=<user> | --user=<user>
		Use the username <user> to connect to the proxy.[br]
		!sw: -w=<password> | --password=<password>
		Use password <password> to connect to the proxy.
	@examples:
		[example]
			[comment]Adds the proxy test.com on port 8080 using IPv6 sockets[/comment][br]
			proxydb.addProxy -i -p=8080 test.com
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

static bool proxydb_kvs_cmd_addProxy(KviKvsModuleCommandCall * c)
{
	QString szProxy;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("proxy_name", KVS_PT_STRING, 0, szProxy)
	KVSM_PARAMETERS_END(c)

	if(szProxy.isEmpty())
	{
		c->error(__tr2qs_ctx("You must provide the proxy name as parameter", "serverdb"));
		return false;
	}

	std::unique_ptr<KviProxy> pProxy(new KviProxy());
	pProxy->setHostname(szProxy);

	KviProxy * pRecord = g_pProxyDataBase->findProxy(pProxy.get(), true);
	if(pRecord)
	{
		if(c->switches()->find('q', "quiet"))
			return true;

		c->error(__tr2qs_ctx("The specified proxy already exists", "serverdb"));
		return false;
	}

	if(c->switches()->find('i', "ipv6"))
		pProxy->setIPv6(true);

	QString tmp;

	if(c->switches()->getAsStringIfExisting('p', "port", tmp))
	{
		bool bOk;
		unsigned int uPort = tmp.toInt(&bOk);
		if(!bOk)
			uPort = 6667;
		pProxy->setPort(uPort);
	}

	if(c->switches()->getAsStringIfExisting('u', "user", tmp))
		pProxy->setUser(tmp);

	if(c->switches()->getAsStringIfExisting('w', "password", tmp))
		pProxy->setPass(tmp);

	if(c->switches()->getAsStringIfExisting('r', "protocol", tmp))
	{
		if(tmp == "Socks4")
			pProxy->setProtocol(KviProxy::Socks4);
		else if(tmp == "Socks5")
			pProxy->setProtocol(KviProxy::Socks5);
		else if(tmp == "Http")
			pProxy->setProtocol(KviProxy::Http);
		else
		{
			c->error(__tr2qs_ctx("The specified protocol doesn't exist", "serverdb"));
			return false;
		}
	}

	g_pProxyDataBase->insertProxy(std::move(pProxy));

	return true;
}

/*
	@doc: proxydb.setPort
	@type:
		command
	@title:
		proxydb.setPort
	@short:
		Sets the port for a proxy
	@syntax:
		proxydb.setPort [switches] <string:proxy> <uint:port>
	@description:
		Sets the port <port> for the proxy <proxy>
	@switches:
		!sw: -q | --quiet
		Do not print errors if the proxy already exist.[br]
	@examples:
		[example]
			[comment]Sets the port 8080 for the proxy test.com[/comment][br]
			proxydb.setPort test.com 8080
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

static bool proxydb_kvs_cmd_setPort(KviKvsModuleCommandCall * c)
{
	QString szProxy, szPort;
	unsigned int uPort;
	bool bOk;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("proxy", KVS_PT_STRING, 0, szProxy)
	KVSM_PARAMETER("port", KVS_PT_STRING, 0, szPort)
	KVSM_PARAMETERS_END(c)

	if(szProxy.isEmpty())
	{
		c->error(__tr2qs_ctx("You must provide the proxy hostname as parameter", "serverdb"));
		return false;
	}

	if(szPort.isEmpty())
	{
		c->error(__tr2qs_ctx("You must provide the port number as parameter", "serverdb"));
		return false;
	}

	KviProxy objProxy;
	objProxy.setHostname(szProxy);

	KviProxy * pRecord = g_pProxyDataBase->findProxy(&objProxy, true);
	if(!pRecord)
	{
		if(c->switches()->find('q', "quiet"))
			return true;
		c->error(__tr2qs_ctx("The specified proxy doesn't exist", "serverdb"));
		return false;
	}

	uPort = szPort.toInt(&bOk);
	if(!bOk)
		uPort = 1080;
	pRecord->setPort(uPort);

	return true;
}

/*
	@doc: proxydb.setIp
	@type:
		command
	@title:
		proxydb.setIp
	@short:
		Sets the IP for a proxy
	@syntax:
		proxydb.setIp [switches] <string:proxy> <string:ip>
	@description:
		Sets the IP <ip> for the proxy <proxy>
	@switches:
		!sw: -q | --quiet
		Do not print errors if the proxy already exist.[br]
	@examples:
		[example]
			[comment]Sets the IPv4 (format a.b.c.d) for the proxy test.com[/comment][br]
			proxydb.setIp test.com 1.2.3.4[br]
			[comment]Sets the extended IPv6 (format a:b:c:d:e:f:g:h) for the proxy text.com[/comment][br]
			proxydb.setIp test.com 1:2:3:4:5:6:7:8[br]
			[comment]Sets the compressed IPv6 (format a:b:...::h) for the proxy text.com[/comment][br]
			proxydb.setIp test.com 1:2:3:4::8[br]
			[comment]Sets the IPv4 mapped IPv6 (format a:b:c:d:e:f:w.x.y.z) for the proxy text.com[/comment][br]
			proxydb.setIp test.com 1:2:3:4:5:6:9.8.7.6[br]
			[comment]Sets the compressed IPv4 mapped IPv6 (a::b:w.x.y.z) for the proxy text.com[/comment][br]
			proxydb.setIp test.com 1::6:9.8.7.6
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

static bool proxydb_kvs_cmd_setIp(KviKvsModuleCommandCall * c)
{
	QString szProxy, szIp;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("proxy", KVS_PT_STRING, 0, szProxy)
	KVSM_PARAMETER("ip", KVS_PT_STRING, 0, szIp)
	KVSM_PARAMETERS_END(c)

	if(szProxy.isEmpty())
	{
		c->error(__tr2qs_ctx("You must provide the proxy hostname as parameter", "serverdb"));
		return false;
	}

	if(szIp.isEmpty())
	{
		c->error(__tr2qs_ctx("You must provide the IP as parameter", "serverdb"));
		return false;
	}

	KviProxy objProxy;
	objProxy.setHostname(szProxy);

	KviProxy * pRecord = g_pProxyDataBase->findProxy(&objProxy, true);
	if(!pRecord)
	{
		if(c->switches()->find('q', "quiet"))
			return true;
		c->error(__tr2qs_ctx("The specified proxy doesn't exist", "serverdb"));
		return false;
	}

	QHostAddress host;
	if(!host.setAddress(szIp))
	{
		if(c->switches()->find('q', "quiet"))
			return true;
		c->error(__tr2qs_ctx("The specified IP is not valid", "serverdb"));
		return false;
	}

	pRecord->setIp(szIp);

	return true;
}

/*
	@doc: proxydb.setIPv6
	@type:
		command
	@title:
		proxydb.setIPv6
	@short:
		Sets the port for a proxy
	@syntax:
		proxydb.setIPv6 [switches] <string:proxy> <bool:IPv6>
	@description:
		Sets the use of IPv6 sockets for the proxy <proxy> if <IPv6> is true
	@switches:
		!sw: -q | --quiet
		Do not print errors if the proxy already exist.[br]
	@examples:
		[example]
			[comment]Sets the IPv6 sockets for the proxy test.com[/comment][br]
			proxydb.setIPv6 test.com $true
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

static bool proxydb_kvs_cmd_setIPv6(KviKvsModuleCommandCall * c)
{
	QString szProxy;
	bool bIPv6;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("proxy", KVS_PT_STRING, 0, szProxy)
	KVSM_PARAMETER("ipv6", KVS_PT_BOOLEAN, 0, bIPv6)
	KVSM_PARAMETERS_END(c)

	if(szProxy.isEmpty())
	{
		c->error(__tr2qs_ctx("You must provide the proxy hostname as parameter", "serverdb"));
		return false;
	}

	KviProxy objProxy;
	objProxy.setHostname(szProxy);

	KviProxy * pRecord = g_pProxyDataBase->findProxy(&objProxy, true);
	if(!pRecord)
	{
		if(c->switches()->find('q', "quiet"))
			return true;
		c->error(__tr2qs_ctx("The specified proxy doesn't exist", "serverdb"));
		return false;
	}

	pRecord->setIPv6(bIPv6);

	return true;
}

/*
	@doc: proxydb.setProtocol
	@type:
		command
	@title:
		proxydb.setProtocol
	@short:
		Sets the protocol for a proxy
	@syntax:
		proxydb.setProtocol [switches] <string:proxy> <string:protocol>
	@description:
		Sets the protocol <protocol> for the proxy <proxy>.[br]
		The protocol can be Socks4, Socks5 or Http.
	@switches:
		!sw: -q | --quiet
		Do not print errors if the proxy already exist.[br]
	@examples:
		[example]
			[comment]Sets the protocol Socks5 for the proxy test.com[/comment][br]
			proxydb.setProtocol test.com Socks5
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

static bool proxydb_kvs_cmd_setProtocol(KviKvsModuleCommandCall * c)
{
	QString szProxy, szProtocol;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("proxy", KVS_PT_STRING, 0, szProxy)
	KVSM_PARAMETER("protocol", KVS_PT_STRING, 0, szProtocol)
	KVSM_PARAMETERS_END(c)

	if(szProxy.isEmpty())
	{
		c->error(__tr2qs_ctx("You must provide the proxy hostname as parameter", "serverdb"));
		return false;
	}

	if(szProtocol.isEmpty())
	{
		c->error(__tr2qs_ctx("You must provide the protocol as parameter", "serverdb"));
		return false;
	}

	KviProxy objProxy;
	objProxy.setHostname(szProxy);

	KviProxy * pRecord = g_pProxyDataBase->findProxy(&objProxy, true);
	if(!pRecord)
	{
		if(c->switches()->find('q', "quiet"))
			return true;
		c->error(__tr2qs_ctx("The specified proxy doesn't exist", "serverdb"));
		return false;
	}

	if(szProtocol == "Socks4")
		pRecord->setProtocol(KviProxy::Socks4);
	else if(szProtocol == "Socks5")
		pRecord->setProtocol(KviProxy::Socks5);
	else if(szProtocol == "Http")
		pRecord->setProtocol(KviProxy::Http);
	else
	{
		c->error(__tr2qs_ctx("The specified protocol doesn't exist", "serverdb"));
		return false;
	}

	return true;
}

#define PROXYDB_SET_PROPERTY(__functionName, __callName)                                           \
	static bool __functionName(KviKvsModuleCommandCall * c)                                        \
	{                                                                                              \
		QString szProxy, szPropertyName;                                                           \
                                                                                                   \
		KVSM_PARAMETERS_BEGIN(c)                                                                   \
		KVSM_PARAMETER("proxy", KVS_PT_STRING, 0, szProxy)                                         \
		KVSM_PARAMETER("property", KVS_PT_STRING, KVS_PF_APPENDREMAINING, szPropertyName)          \
		KVSM_PARAMETERS_END(c)                                                                     \
                                                                                                   \
		if(szProxy.isEmpty())                                                                      \
		{                                                                                          \
			c->error(__tr2qs_ctx("You must provide the proxy hostname as parameter", "serverdb")); \
			return false;                                                                          \
		}                                                                                          \
                                                                                                   \
		if(szPropertyName.isEmpty())                                                               \
		{                                                                                          \
			c->error(__tr2qs_ctx("You must provide the value as parameter", "serverdb"));          \
			return false;                                                                          \
		}                                                                                          \
                                                                                                   \
		KviProxy objProxy;                                                                         \
		objProxy.setHostname(szProxy);                                                             \
                                                                                                   \
		KviProxy * pRecord = g_pProxyDataBase->findProxy(&objProxy, true);                         \
		if(!pRecord)                                                                               \
		{                                                                                          \
			if(c->switches()->find('q', "quiet"))                                                  \
				return true;                                                                       \
			c->error(__tr2qs_ctx("The specified proxy doesn't exist", "serverdb"));                \
			return false;                                                                          \
		}                                                                                          \
                                                                                                   \
		pRecord->__callName(szPropertyName);                                                       \
                                                                                                   \
		return true;                                                                               \
	}

/*
	@doc: proxydb.setHostname
	@type:
		command
	@title:
		proxydb.setHostname
	@short:
		Sets the hostname for a proxy
	@syntax:
		proxydb.setHostname [switches] <string:proxy> <string:hostname>
	@description:
		Sets the hostname <hostname> for the proxy <proxy>
	@switches:
		!sw: -q | --quiet
		Do not print errors if the proxy already exist.[br]
	@examples:
		[example]
			[comment]Sets the hostname test1.com for the proxy test.com[/comment][br]
			proxydb.setHostname test.com test1.com
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

PROXYDB_SET_PROPERTY(proxydb_kvs_cmd_setHostname, setHostname)

/*
	@doc: proxydb.setPass
	@type:
		command
	@title:
		proxydb.setPass
	@short:
		Sets the password for a proxy
	@syntax:
		proxydb.setPass [switches] <string:proxy> <string:password>
	@description:
		Sets the password <password> for the proxy <proxy>
	@switches:
		!sw: -q | --quiet
		Do not print errors if the proxy already exist.[br]
	@examples:
		[example]
			[comment]Sets the password test for the proxy test.com[/comment][br]
			proxydb.setPass test.com test
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

PROXYDB_SET_PROPERTY(proxydb_kvs_cmd_setPass, setPass)

/*
	@doc: proxydb.setUser
	@type:
		command
	@title:
		proxydb.setUser
	@short:
		Sets the username for a proxy
	@syntax:
		proxydb.setUser [switches] <string:proxy> <string:user>
	@description:
		Sets the username <user> for the proxy <proxy>
	@switches:
		!sw: -q | --quiet
		Do not print errors if the proxy already exist.[br]
	@examples:
		[example]
			[comment]Sets the username KVIrc for the proxy test.com[/comment][br]
			proxydb.setUser test.com kvirc
		[/example]
	@seealso:
		[module:serverdb]ServerDB module documentation[/module]
*/

PROXYDB_SET_PROPERTY(proxydb_kvs_cmd_setUser, setUser)

static bool proxydb_module_init(KviModule * m)
{
	KVSM_REGISTER_FUNCTION(m, "hostname", proxydb_kvs_fnc_hostname);
	KVSM_REGISTER_FUNCTION(m, "ip", proxydb_kvs_fnc_ip);
	KVSM_REGISTER_FUNCTION(m, "isIPv6", proxydb_kvs_fnc_isIPv6);
	KVSM_REGISTER_FUNCTION(m, "password", proxydb_kvs_fnc_password);
	KVSM_REGISTER_FUNCTION(m, "port", proxydb_kvs_fnc_port);
	KVSM_REGISTER_FUNCTION(m, "protocol", proxydb_kvs_fnc_protocol);
	KVSM_REGISTER_FUNCTION(m, "username", proxydb_kvs_fnc_username);

	KVSM_REGISTER_SIMPLE_COMMAND(m, "addProxy", proxydb_kvs_cmd_addProxy);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setHostname", proxydb_kvs_cmd_setHostname);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setIp", proxydb_kvs_cmd_setIp);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setIPv6", proxydb_kvs_cmd_setIPv6);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setPass", proxydb_kvs_cmd_setPass);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setPort", proxydb_kvs_cmd_setPort);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setProtocol", proxydb_kvs_cmd_setProtocol);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setUser", proxydb_kvs_cmd_setUser);

	return true;
}

static bool proxydb_module_cleanup(KviModule *)
{
	return true;
}

static bool proxydb_module_can_unload(KviModule *)
{
	return true;
}

KVIRC_MODULE(
    "ProxyDB",                                                         // module name
    "4.0.0",                                                           // module version
    "Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)", // author & (C)
    "IRC proxy serverDB related functions",
    proxydb_module_init,
    proxydb_module_can_unload,
    0,
    proxydb_module_cleanup,
    "serverdb")
