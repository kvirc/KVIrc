//=============================================================================
//
//   File : libkviclasseditor.cpp
//   CCreation date : Mon Feb 15 2010 14:35:55 CEST by Carbone Alessandro
//
//   This toolbar is part of the KVirc irc client distribution
//   Copyright (C) 2010 Alessandro Carbone (elfonol at gmail dot com)
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "classeditor.h"

#include "kvi_module.h"
#include "kvi_locale.h"
#include "kvi_frame.h"


KviClassEditorWindow * g_pClassEditorWindow = 0;
KviModule * g_pClassEditorModule = 0;


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

static bool classeditor_kvs_cmd_open(KviKvsModuleCommandCall * c)
{
	if(!g_pClassEditorWindow)
	{
		g_pClassEditorWindow = new KviClassEditorWindow(c->window()->frame());
		c->window()->frame()->addWindow(g_pClassEditorWindow);
	}

	g_pClassEditorWindow->setFocus();

	return true;
}

static bool classeditor_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m,"open",classeditor_kvs_cmd_open);
	g_pClassEditorWindow = 0;
	g_pClassEditorModule = m;
	return true;
}

static bool classeditor_module_can_unload(KviModule * )
{
	return (g_pClassEditorWindow == 0);
}

static bool classeditor_module_cleanup(KviModule *)
{
	if(g_pClassEditorWindow && g_pFrame)
		g_pFrame->closeWindow(g_pClassEditorWindow);
	g_pClassEditorWindow = 0;
	return true;
}

KVIRC_MODULE(
	"ClassEditor",                                                       // module name
	"4.0.0",                                                             // module version
	"Copyright (C) 2010 Carbone Alessandr (elfonol at gmail dot com)", // author & (C)
	"Editor for the script classes",
	classeditor_module_init,
	classeditor_module_can_unload,
	0,
	classeditor_module_cleanup,
	"editor"
)
