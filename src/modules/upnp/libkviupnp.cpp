//=============================================================================
//
//   File : libkviupnp.cpp
//   Creation date : Fri Aug 08 18:00:00 2008 GMT by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Fabio Bas (ctrlaltca at gmail dot com)
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

#include "kvi_settings.h"
#include "KviModule.h"
#include "KviCString.h"
#include "KviNetUtils.h"

#include "Manager.h"

// Please use our common variable naming conventions and coding style :)
// Let's be so:)
UPnP::Manager * g_pManager = nullptr;

/*
	@doc: upnp.getExternalIpAddress
	@type:
		function
	@title:
		$upnp.getExternalIpAddress
	@short:
		Return the external IP address using UPnP
	@syntax:
		<string> $upnp.getExternalIpAddress()
	@description:
		During the loading of the UPnP module (and during a [cmd]upnp.refresh[/cmd]), KVIrc searches the gateway of your local network. If a gateway is found, KVIrc requests it the external IP address associated to the router and caches it.[br]
		Using this function you can get this cached value.[br]
		Take care that if no gateway have been found or it returned no external IP address (this can happens if its upstream  (WAN) link is not connected), this function returns an empty string.[br]
		It's better to check is a gateway has been found using [fnc]$upnp.isGatewayAvailable[/fnc] before using this function.
	@seealso:
		[fnc]$upnp.isGatewayAvailable[/fnc]
*/

static bool upnp_kvs_fnc_getExternalIpAddress(KviKvsModuleFunctionCall * c)
{
	if(g_pManager)
	{
		c->returnValue()->setString(g_pManager->getExternalIpAddress());
	}
	return true;
}

/*
	@doc: upnp.isGatewayAvailable
	@type:
		function
	@title:
		$upnp.isGatewayAvailable
	@short:
		Returns if a UPnP-capable gateway has been found on the local network
	@syntax:
		<bool> $upnp.isGatewayAvailable()
	@description:
		Returns if a UPnP-capable gateway has been found on the local network.[br]
		If this function returns false (0), no other command or function from the UPnP module will work.
*/

static bool upnp_kvs_fnc_isGatewayAvailable(KviKvsModuleFunctionCall * c)
{
	if(g_pManager)
	{
		c->returnValue()->setBoolean(g_pManager->isGatewayAvailable());
	}
	return true;
}

/*
	@doc: upnp.addPortMapping
	@type:
		command
	@title:
		upnp.addPortMapping
	@short:
		Add a port mapping to gateway using UPnP
	@syntax:
		upnp.addPortMapping [-a=<fake address>] <port>
	@switches:
		!sw: -a=<fake address> | --fake-address=<fake address>
		Send the <fake address> as target for the port mapping
		If this switch is not given, the request will contain the real IP address of the listening
		interface.[br]
	@description:
		Makes a request to the gateway of the local network using UPnP; the request asks the gateway to add an entry in its port mapping table.[br]
		If the gateway accepts the request, it will forward packets received on <port> on its external (WAN) IP address to the host KVIrc is running at, on the same <port>.
		Depending on vendors, this is called with different names: Port forwarding, SUA, Virtual Server, PAT, ...
		It's better to check is a gateway has been found using [fnc]$upnp.isGatewayAvailable[/fnc] before using this function.
	@seealso:
		[fnc]$upnp.isGatewayAvailable[/fnc], [cmd]upnp.delPortMapping[/cmd]
*/

static bool upnp_kvs_cmd_addPortMapping(KviKvsModuleCommandCall * c)
{
	kvs_int_t iPort;
	QString szLocalIp;
	KviKvsVariant * pSw;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("port", KVS_PT_INT, 0, iPort)
	KVSM_PARAMETERS_END(c)

	if((pSw = c->switches()->find('a', "fake-address")))
	{
		pSw->asString(szLocalIp);
	}
	else
	{
		kvi_getLocalHostAddress(szLocalIp);
	}

	g_pManager->addPortMapping("TCP", "", iPort, szLocalIp, iPort, "KVIrc UPnP module", true, 0);
	return true;
}

/*
	@doc: upnp.delPortMapping
	@type:
		command
	@title:
		upnp.delPortMapping
	@short:
		Delete a port mapping on the gateway using UPnP
	@syntax:
		upnp.delPortMapping <port>
	@description:
		Makes a request to the gateway of the local network using UPnP; the request asks the gateway to delete an entry in its port mapping table.[br]
		If the gateway accepts the request, it will stop forward packets received on <port>.
		It's better to check is a gateway has been found using [fnc]$upnp.isGatewayAvailable[/fnc] before using this function.[br]
		Of course, such a port mapping have to be added using [cmd]upnp.addPortMapping[/cmd] before it can be deleted.
	@seealso:
		[fnc]$upnp.isGatewayAvailable[/fnc], [cmd]upnp.addPortMapping[/cmd]
*/

static bool upnp_kvs_cmd_delPortMapping(KviKvsModuleCommandCall * c)
{
	kvs_int_t iPort;
	QString tmp;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("port", KVS_PT_INT, 0, iPort)
	KVSM_PARAMETERS_END(c)

	g_pManager->deletePortMapping("TCP", "", iPort);
	return true;
}

/*
	@doc: upnp.refresh
	@type:
		command
	@title:
		upnp.refresh
	@short:
		Rescan the local network for a gateway using UPnP
	@syntax:
		upnp.refresh
	@description:
		Rescan the local network for a gateway using UPnP.[br]
		This command is automatically executed when KVIrc loads the UPnP module.
	@seealso:
		[fnc]$upnp.isGatewayAvailable[/fnc]
*/

static bool upnp_kvs_cmd_refresh(KviKvsModuleCommandCall *)
{
	if(g_pManager)
		delete g_pManager;
	g_pManager = nullptr;

	g_pManager = UPnP::Manager::instance();

	return true;
}

static bool upnp_module_init(KviModule * m)
{
	if(g_pManager)
		delete g_pManager;
	g_pManager = nullptr;

	g_pManager = UPnP::Manager::instance();

	KVSM_REGISTER_FUNCTION(m, "isGatewayAvailable", upnp_kvs_fnc_isGatewayAvailable);
	KVSM_REGISTER_FUNCTION(m, "getExternalIpAddress", upnp_kvs_fnc_getExternalIpAddress);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "addPortMapping", upnp_kvs_cmd_addPortMapping);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "delPortMapping", upnp_kvs_cmd_delPortMapping);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "refresh", upnp_kvs_cmd_refresh);

	return true;
}

static bool upnp_module_cleanup(KviModule *)
{
	delete g_pManager;
	g_pManager = nullptr;
	return true;
}

static bool upnp_module_can_unload(KviModule *)
{
	// if a gateway is available, don't unload me
	return !g_pManager->isGatewayAvailable();
}

KVIRC_MODULE(
    "UPnP",  // module name
    "4.0.0", // module version
    "Copyright (C) 2008 Fabio Bas (ctrlaltca at gmail dot com),",
    "Universal Plug and Play",
    upnp_module_init,
    upnp_module_can_unload,
    0,
    upnp_module_cleanup,
    0)
