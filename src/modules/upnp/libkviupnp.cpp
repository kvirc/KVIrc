//=============================================================================
//
//   File : libkviupnp.cpp
//   Creation date : Fri Aug 08 18:00:00 2000 GMT by Szymon Stefanek
//
//   This math is part of the KVirc irc client distribution
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
//=============================================================================

#include "kvi_settings.h"
#include "kvi_module.h"
#include "kvi_string.h"

#include "manager.h"

UPnP::Manager* p_manager = 0;


static bool upnp_kvs_fnc_getExternalIpAddress(KviKvsModuleFunctionCall * c)
{
	QString buffer;

	if(p_manager)
		buffer = p_manager->getExternalIpAddress();

	c->returnValue()->setString(buffer);
	return true;
}

static bool upnp_kvs_cmd_test(KviKvsModuleCommandCall * c)
{
	return true;
}

static bool upnp_module_init(KviModule * m)
{
	p_manager = UPnP::Manager::instance();
	//p_manager->initialize();

	KVSM_REGISTER_FUNCTION(m,"getExternalIpAddress",upnp_kvs_fnc_getExternalIpAddress);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"test",upnp_kvs_cmd_test);

	return true;
}

static bool upnp_module_cleanup(KviModule *m)
{
	delete p_manager;
	p_manager = 0;
	return true;
}

KVIRC_MODULE(
	"Upnp",                                                 // module name
	"4.0.0",                                                // module version
	"Copyright (C) 2008 Szymon Stefanek (pragma at kvirc dot net),",
	"Universal Plug and Play",
	upnp_module_init,
	0,
	0,
	upnp_module_cleanup
)
