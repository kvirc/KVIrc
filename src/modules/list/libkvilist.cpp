//=============================================================================
//
//   File : libkvilist.cpp
//   Creation date : Sun Oct 21 2001 13:29:12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "ListWindow.h"

#include "KviModule.h"
#include "KviMainWindow.h"
#include "KviWindow.h"
#include "KviLocale.h"

#include <QSplitter>

KviPointerList<ListWindow> * g_pListWindowList = nullptr;

/*
	@doc: list.open
	@type:
		command
	@title:
		list.open
	@short:
		Opens a list window
	@syntax:
		list.open
	@description:
		Opens the channel list window attached to the current IRC context.[br]
		The channel list window is opened automatically when a channel list
		is requested from the server.
*/

static bool list_kvs_cmd_open(KviKvsModuleCommandCall * c)
{

	if(!c->window()->console())
		return c->context()->errorNoIrcContext();

	if(!(c->window()->context()->listWindow()))
	{
		ListWindow * w = new ListWindow(c->window()->console());
		g_pMainWindow->addWindow(w);
	}
	else
	{
		c->warning(__tr2qs("List window already open for this IRC context"));
	}

	return true;
}

static bool list_module_init(KviModule * m)
{
	g_pListWindowList = new KviPointerList<ListWindow>;
	g_pListWindowList->setAutoDelete(false);

	KVSM_REGISTER_SIMPLE_COMMAND(m, "open", list_kvs_cmd_open);
	return true;
}

static bool list_module_cleanup(KviModule *)
{
	while(g_pListWindowList->first())
		g_pListWindowList->first()->die();
	delete g_pListWindowList;
	g_pListWindowList = nullptr;
	return true;
}

static bool list_module_can_unload(KviModule *)
{
	return (g_pListWindowList->isEmpty());
}

KVIRC_MODULE(
    "List",                                                              // module name
    "4.0.0",                                                             // module version
    "Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
    "List window extension",
    list_module_init,
    list_module_can_unload,
    0,
    list_module_cleanup,
    0)
