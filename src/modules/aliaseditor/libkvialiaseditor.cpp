//=============================================================================
//
//   File : libkvialiaseditor.cpp
//   Creation date : Mon 23 Dec 2002 14:30:59 2002 GMT by Szymon Stefanek
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

#include "AliasEditorWindow.h"

#include "KviModule.h"
#include "KviLocale.h"
#include "KviMainWindow.h"

AliasEditorWindow * g_pAliasEditorWindow = nullptr;
KviModule * g_pAliasEditorModule = nullptr;

/*
	@doc: aliaseditor.open
	@type:
		command
	@title:
		aliaseditor.open
	@short:
		Shows the alias editor
	@syntax:
		aliaseditor.open
	@description:
		Opens the script alias editor dialog.
*/

static bool aliaseditor_kvs_cmd_open(KviKvsModuleCommandCall *)
{
	if(!g_pAliasEditorWindow)
	{
		g_pAliasEditorWindow = new AliasEditorWindow();
		g_pMainWindow->addWindow(g_pAliasEditorWindow);
	}

	g_pAliasEditorWindow->delayedAutoRaise();

	return true;
}

static bool aliaseditor_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m, "open", aliaseditor_kvs_cmd_open);
	g_pAliasEditorWindow = nullptr;
	g_pAliasEditorModule = m;
	return true;
}

static bool aliaseditor_module_can_unload(KviModule *)
{
	return (g_pAliasEditorWindow == nullptr);
}

static bool aliaseditor_module_cleanup(KviModule *)
{
	if(g_pAliasEditorWindow && g_pMainWindow)
		g_pMainWindow->closeWindow(g_pAliasEditorWindow);
	g_pAliasEditorWindow = nullptr;
	return true;
}

KVIRC_MODULE(
    "AliasEditor",                                                       // module name
    "4.0.0",                                                             // module version
    "Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
    "Editor for the script aliases",
    aliaseditor_module_init,
    aliaseditor_module_can_unload,
    0,
    aliaseditor_module_cleanup,
    "editor")
