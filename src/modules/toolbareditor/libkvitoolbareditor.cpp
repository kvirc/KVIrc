//=============================================================================
//
//   File : libkvitoolbareditor.cpp
//   Creation date : Sun 10 Nov 2002 23:25:59 2002 GMT by Szymon Stefanek
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

#include "CustomizeToolBarsDialog.h"

#include "KviModule.h"
#include "KviLocale.h"
#include "KviMainWindow.h"
#include "KviActionManager.h"
#include "KviConfigurationFile.h"
#include "KviApplication.h"

QRect g_rectToolBarEditorDialogGeometry(0, 0, 0, 0);

/*
	@doc: toolbareditor.open
	@type:
		command
	@title:
		toolbareditor.open
	@short:
		Shows the script toolbar editor
	@syntax:
		toolbareditor.open [-t]
	@description:
		Opens the script toolbar editor dialog.[br]
		If the [-t] switch is used, the dialog is opened as toplevel window,
		otherwise it is opened as part of the current frame window.[br]
*/

static bool toolbareditor_kvs_cmd_open(KviKvsModuleCommandCall * c)
{
	CustomizeToolBarsDialog::display(c->hasSwitch('t', "toplevel"));
	return true;
}

static bool toolbareditor_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m, "open", toolbareditor_kvs_cmd_open);
	QString szBuf;
	m->getDefaultConfigFileName(szBuf);
	KviConfigurationFile cfg(szBuf, KviConfigurationFile::Read);
	g_rectToolBarEditorDialogGeometry = cfg.readRectEntry("EditorGeometry", QRect(10, 10, 390, 440));

	return true;
}

static bool toolbareditor_module_can_unload(KviModule *)
{
	return (!CustomizeToolBarsDialog::instance());
}

static bool toolbareditor_module_cleanup(KviModule * m)
{
	CustomizeToolBarsDialog::cleanup();

	QString szBuf;
	m->getDefaultConfigFileName(szBuf);
	KviConfigurationFile cfg(szBuf, KviConfigurationFile::Write);
	cfg.writeEntry("EditorGeometry", g_rectToolBarEditorDialogGeometry);

	return true;
}

KVIRC_MODULE(
    "ToolbarEditor",                                                // module name
    "4.0.0",                                                        // module version
    "Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
    "Editor for the scriptable toolbars",
    toolbareditor_module_init,
    toolbareditor_module_can_unload,
    0,
    toolbareditor_module_cleanup,
    "editor")
