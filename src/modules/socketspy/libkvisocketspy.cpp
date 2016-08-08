//=============================================================================
//
//   File : libkvisocketspy.cpp
//   Creation date : Sun Nov 26 2000 13:05:00 CEST by Szymon Stefanek
//
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "SocketSpyWindow.h"

#include "KviModule.h"
#include "KviMainWindow.h"
#include "KviWindow.h"
#include "KviLocale.h"

#include <QSplitter>
#include <unordered_set>

std::unordered_set<SocketSpyWindow *> g_pSocketSpyWindowList;

/*
	@doc: socketspy.open
	@type:
		command
	@title:
		socketspy.open
	@short:
		Opens a Socket Spy window
	@syntax:
		socketspy.open
	@description:
		Creates a new socket spy window. A Socket spy window
		monitors the incoming and outgoing RAW data on the
		IRC socket of the current IRC context.
*/

static bool socketspy_kvs_cmd_open(KviKvsModuleCommandCall * c)
{
	if(!c->window()->console())
		return c->context()->errorNoIrcContext();
	SocketSpyWindow * w = new SocketSpyWindow(c->window()->console());
	g_pMainWindow->addWindow(w);
	return true;
}

static bool socketspy_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m, "open", socketspy_kvs_cmd_open);
	return true;
}

static bool socketspy_module_cleanup(KviModule *)
{
	while(!g_pSocketSpyWindowList.empty())
		(*g_pSocketSpyWindowList.begin())->die();
	return true;
}

static bool socketspy_module_can_unload(KviModule *)
{
	return (g_pSocketSpyWindowList.empty());
}

KVIRC_MODULE(
    "SocketSpy",                                                    // module name
    "4.0.0",                                                        // module version
    "Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
    "SocketSpy IRC socket traffic monitor",
    socketspy_module_init,
    socketspy_module_can_unload,
    0,
    socketspy_module_cleanup,
    0)
