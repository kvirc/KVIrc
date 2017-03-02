//=============================================================================
//
//   File : libkviraweditor.cpp
//   Creation date : Mon 23 Dec 2002 20:23:59 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "RawEditorWindow.h"

#include "KviModule.h"
#include "KviLocale.h"
#include "KviMainWindow.h"

RawEditorWindow * g_pRawEditorWindow = nullptr;

/*
	@doc: raweditor.open
	@type:
		command
	@title:
		raweditor.open
	@short:
		Shows the RAW events editor
	@syntax:
		toolbareditor.open
	@description:
		Opens the script RAW events editor dialog.
*/

static bool raweditor_kvs_cmd_open(KviKvsModuleCommandCall *)
{
	if(!g_pRawEditorWindow)
	{
		g_pRawEditorWindow = new RawEditorWindow();
		g_pMainWindow->addWindow(g_pRawEditorWindow);
	}
	g_pRawEditorWindow->delayedAutoRaise();
	return true;
}

static bool raweditor_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m, "open", raweditor_kvs_cmd_open);
	g_pRawEditorWindow = nullptr;
	return true;
}

static bool raweditor_module_can_unload(KviModule *)
{
	return (g_pRawEditorWindow == nullptr);
}

static bool raweditor_module_cleanup(KviModule *)
{
	if(g_pRawEditorWindow && g_pMainWindow)
		g_pMainWindow->closeWindow(g_pRawEditorWindow);
	g_pRawEditorWindow = nullptr;
	return true;
}

KVIRC_MODULE(
    "RawEditor",                                                    // module name
    "4.0.0",                                                        // module version
    "Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
    "Editor for the script RAW events",
    raweditor_module_init,
    raweditor_module_can_unload,
    0,
    raweditor_module_cleanup,
    "editor")
