//=============================================================================
//
//   File : libkvicodetester.cpp
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

#include "CodeTesterWindow.h"

#include "KviModule.h"
#include "KviLocale.h"
#include "KviMainWindow.h"
#include "KviPointerList.h"
#include <unordered_set>

std::unordered_set<CodeTesterWindow *> g_pCodeTesterWindowList;

/*
	@doc: codetester.open
	@type:
		command
	@title:
		codetester.open
	@short:
		Shows the code tester window
	@syntax:
		codetester.open
	@description:
		Opens a new code tester window
*/

static bool codetester_kvs_cmd_open(KviKvsModuleCommandCall *)
{
	CodeTesterWindow * w = new CodeTesterWindow();
	g_pMainWindow->addWindow(w);
	w->setFocus();
	return true;
}

static bool codetester_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m, "open", codetester_kvs_cmd_open);
	return true;
}

static bool codetester_module_can_unload(KviModule *)
{
	return g_pCodeTesterWindowList.empty();
}

static bool codetester_module_cleanup(KviModule *)
{
	while(!g_pCodeTesterWindowList.empty())
	{
		auto w = g_pCodeTesterWindowList.begin();

		if (w == g_pCodeTesterWindowList.end())
			break;

		(*w)->close(); // deleted path!
	}
	return true;
}

KVIRC_MODULE(
    "CodeTester",                                                   // module name
    "4.0.0",                                                        // module version
    "Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
    "Code tester window",
    codetester_module_init,
    codetester_module_can_unload,
    0,
    codetester_module_cleanup,
    "editor")
