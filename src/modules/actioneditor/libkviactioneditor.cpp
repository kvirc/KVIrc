//=============================================================================
//
//   File : libkviactioneditor.cpp
//   Creation date : Tue 29 Dec 2004 02:45:59 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "ActionEditor.h"

#include "KviModule.h"
#include "KviLocale.h"
#include "KviMainWindow.h"

ActionEditorWindow * g_pActionEditorWindow = nullptr;

/*
	@doc: actioneditor.open
	@type:
		command
	@title:
		actioneditor.open
	@short:
		Shows the action editor
	@syntax:
		actioneditor.open
	@description:
		Opens the script action editor dialog.
*/

static bool actioneditor_kvs_cmd_open(KviKvsModuleCommandCall *)
{
	if(!g_pActionEditorWindow)
	{
		g_pActionEditorWindow = new ActionEditorWindow();
		g_pMainWindow->addWindow(g_pActionEditorWindow);
	}

	g_pActionEditorWindow->delayedAutoRaise();
	return true;
}

static bool actioneditor_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m, "open", actioneditor_kvs_cmd_open);
	g_pActionEditorWindow = nullptr;
	return true;
}

static bool actioneditor_module_can_unload(KviModule *)
{
	return (g_pActionEditorWindow == nullptr);
}

static bool actioneditor_module_cleanup(KviModule *)
{
	if(g_pActionEditorWindow && g_pMainWindow)
		g_pMainWindow->closeWindow(g_pActionEditorWindow);
	g_pActionEditorWindow = nullptr;
	return true;
}

KVIRC_MODULE(
    "ActionEditor",                                                 // module name
    "4.0.0",                                                        // module version
    "Copyright (C) 2004 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
    "Editor for the script actions",
    actioneditor_module_init,
    actioneditor_module_can_unload,
    0,
    actioneditor_module_cleanup,
    "editor")
