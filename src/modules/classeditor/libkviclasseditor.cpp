//=============================================================================
//
//   File : libkviclasseditor.cpp
//   CCreation date : Mon Feb 15 2010 14:35:55 CEST by Carbone Alessandro
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Alessandro Carbone (elfonol at gmail dot com)
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

#include "ClassEditorWindow.h"

#include "KviModule.h"
#include "KviLocale.h"
#include "KviMainWindow.h"

ClassEditorWindow * g_pClassEditorWindow = nullptr;
KviModule * g_pClassEditorModule = nullptr;

/*
	@doc: classeditor.open
	@type:
		command
	@title:
		classeditor.open
	@short:
                Shows the class editor
	@syntax:
		classeditor.open
	@description:
		Opens the script class editor dialog.
*/
static bool classeditor_kvs_cmd_open(KviKvsModuleCommandCall *)
{
	if(!g_pClassEditorWindow)
	{
		g_pClassEditorWindow = new ClassEditorWindow();
		g_pMainWindow->addWindow(g_pClassEditorWindow);
	}

	g_pClassEditorWindow->delayedAutoRaise();

	return true;
}

static bool classeditor_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m, "open", classeditor_kvs_cmd_open);
	g_pClassEditorWindow = nullptr;
	g_pClassEditorModule = m;
	return true;
}

static bool classeditor_module_can_unload(KviModule *)
{
	return (g_pClassEditorWindow == nullptr);
}

static bool classeditor_module_cleanup(KviModule *)
{
	if(g_pClassEditorWindow && g_pMainWindow)
		g_pMainWindow->closeWindow(g_pClassEditorWindow);
	g_pClassEditorWindow = nullptr;
	return true;
}

KVIRC_MODULE(
    "ClassEditor",                                                      // module name
    "4.0.0",                                                            // module version
    "Copyright (C) 2010 Carbone Alessandro (elfonol at gmail dot com)", // author & (C)
    "Editor for the script classes",
    classeditor_module_init,
    classeditor_module_can_unload,
    0,
    classeditor_module_cleanup,
    "editor")
