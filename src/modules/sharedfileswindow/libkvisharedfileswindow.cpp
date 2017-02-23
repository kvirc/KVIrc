//=============================================================================
//
//   File : libkvisharedfileswindow.cpp
//   Creation date : Mon Apr 21 2003 23:14:12 CEST by Szymon Stefanek
//
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "SharedFilesWindow.h"

#include "KviModule.h"
#include "KviApplication.h"
#include "KviIconManager.h"
#include "KviLocale.h"
#include "KviMainWindow.h"
#include "KviWindow.h"

#define KVI_SHARED_FILES_WINDOW_EXTENSION_NAME "Shared files window extension"

SharedFilesWindow * g_pSharedFilesWindow = nullptr;

/*
	@doc: sharedfileswindow.open
	@type:
		command
	@title:
		sharedfileswindow.open
	@short:
		Opens the shared files window
	@syntax:
		sharedfileswindow.open [-m] [-n]
	@switches:
		!sw: -m
		Causes the window to be created as minimized
		!sw: -n
		Causes the window to be not raised if already open
	@description:
		Opens the shared files window
*/

static bool sharedfileswindow_kvs_cmd_open(KviKvsModuleCommandCall * c)
{
	if(!g_pSharedFilesWindow)
	{
		bool bCreateMinimized = c->hasSwitch('m', "minimized");
		g_pSharedFilesWindow = new SharedFilesWindow();
		g_pMainWindow->addWindow(g_pSharedFilesWindow, !bCreateMinimized);
		return true;
	}

	if(!c->hasSwitch('n', "noraise"))
		g_pSharedFilesWindow->delayedAutoRaise();
	return true;
}

static bool sharedfileswindow_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m, "open", sharedfileswindow_kvs_cmd_open);
	return true;
}

static bool sharedfileswindow_module_cleanup(KviModule *)
{
	if(g_pSharedFilesWindow && g_pMainWindow)
		g_pMainWindow->closeWindow(g_pSharedFilesWindow);
	g_pSharedFilesWindow = nullptr;
	return true;
}

static bool sharedfileswindow_module_can_unload(KviModule *)
{
	return (!g_pSharedFilesWindow);
}

KVIRC_MODULE(
    "SharedFilesWindow",
    "4.0.0",
    "Copyright (C) 2003 Szymon Stefanek (pragma at kvirc dot net)",
    "Shared files window extension",
    sharedfileswindow_module_init,
    sharedfileswindow_module_can_unload,
    0,
    sharedfileswindow_module_cleanup,
    "sharedfileswindow")
