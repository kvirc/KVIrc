//   File : libkvilag.cpp
//   Creation date :  Oct 10 01:06:09 CEST 2002 by Juanjo �varez
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Juanjo �varez (juanjux at yahoo.es)
//   Copyright (C) 2002 Szymon Stefanek (kvirc at tin.it)
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

// FIXME: This module should disappear: straight $lag should be available instead

#include "kvi_module.h"

#include "kvi_console.h"
#include "kvi_locale.h"

#include "kvi_ircconnection.h"
#include "kvi_lagmeter.h"


//-------------------------------------------------
/*
    @doc: $lag.lag
    @type:
        function
    @title:
        $lag.lag
    @short:
        Returns the lag on the current server
    @syntax:
        <integer> $lag.lag
    @description:
        This function returns the lag in the current server, in miliseconds.[br]
    @seealso:
        [cmd]lag.enable[/cmd]
        [cmd]lag.disable[/cmd]
        [cmd]lag.setinterval[/cmd]
        [cmd]lag.getinterval[/cmd]
        [cmd]lag.getalarmtime[/cmd]
        [cmd]lag.setalarmtime[/cmd]
        [cmd]lag.removelaget[/cmd]
*/
//-------------------------------------------
// lag.lag
//-------------------------------------------

static bool lag_kvs_fnc_lag(KviKvsModuleFunctionCall * c)
{
	if(!c->window()->console()) return c->context()->errorNoIrcContext();
	if(!c->window()->console()->connection())  return c->context()->warningNoIrcConnection();
	
	
	if(!c->window()->console()->connection()->lagMeter())
	{
		c->warning(__tr2qs("Lag meter was not enabled"));
		return false;
	}

	c->returnValue()->setInteger( c->window()->console()->connection()->lagMeter()->lag());
	return true;
}

//------------------------------------------------
static bool lag_module_init(KviModule * m)
{

	KVSM_REGISTER_FUNCTION(m,"lag",lag_kvs_fnc_lag);

	return true;
}
//-------------------------------------------------
static bool lag_module_cleanup(KviModule *m)
{
	return true;
}
//-------------------------------------------------
static bool lag_module_can_unload(KviModule *m)
{
    return true;
}
//-------------------------------------------------
KVIRC_MODULE(
	"Lag",                                                 // module name
	"1.0.0",                                                // module version
	"          (C) 2002 Juanjo Alvarez (juanjux@yahoo.es)", // author & (C)
	"Functions and commands to control the lag metering engine",
	lag_module_init,
	lag_module_can_unload,
	0,
	lag_module_cleanup
)
