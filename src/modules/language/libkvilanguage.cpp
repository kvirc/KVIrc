//=============================================================================
//
//   File : libkvilanguage.cpp
//   Creation date : Fri 25 Dec 20:16:49 2004 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2004 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_module.h"
#include "kvi_app.h"


static bool language_module_init(KviModule * m)
{
	/*
	m->registerFunction("nick",my_module_fnc_nick);
	m->registerFunction("user",my_module_fnc_user);
	m->registerFunction("host",my_module_fnc_host);
	m->registerFunction("ip",my_module_fnc_ip);
	m->registerFunction("server",my_module_fnc_server);
	m->registerFunction("network",my_module_fnc_network);
	m->registerFunction("umode",my_module_fnc_umode);
	*/
	return true;
}

static bool language_module_cleanup(KviModule *m)
{
	return true;
}

KVIRC_MODULE(
	"Language",                                              // module name
	"4.0.0",                                                // module version
	"Copyright (C) 2004 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
	"A simple statistical language/encoding detector",
	language_module_init,
	0,
	0,
	language_module_cleanup
)
