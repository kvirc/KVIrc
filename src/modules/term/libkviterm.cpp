//=============================================================================
//
//   File : libkviterm.cpp
//   Creation date : Wed Aug 30 2000 15:29:02 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
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

#include "kvi_settings.h"
#include "KviModule.h"
#include "KviWindow.h"
#include "KviMainWindow.h"

#include <QSplitter>

#ifdef COMPILE_KDE4_SUPPORT
#include "TermWidget.h"
#include "TermWindow.h"

#include <unordered_set>

std::unordered_set<TermWidget *> g_pTermWidgetList;
std::unordered_set<TermWindow *> g_pTermWindowList;
#endif

KviModule * g_pTermModule = nullptr;

/*
	@doc: term.open
	@type:
		command
	@title:
		term.open
	@short:
		Shows a terminal emulator
	@syntax:
		term.open [-d]
	@switches:
		!sw: -d | --dockable
		creates terminal as a docked window
	@description:
		Opens a new terminal window (If the service is available).
		if the -m switch is present, the created terminal as a dockable KVIrc window,
		otherwise it is a static window.[br]
		This command is exported by the "term" module.[br]
		Note: At the time that this module was written, the
		KDE terminal emulator module was quite unstable; no extensive
		testings have been made on this.
*/

static bool term_kvs_cmd_open(KviKvsModuleCommandCall * c)
{
#ifdef COMPILE_KDE4_SUPPORT
	c->module()->lock(); // multiple locks are allowed
	if(
	    c->hasSwitch('m', "mdi") || // compat only
	    c->hasSwitch('d', "dockable"))
	{
		TermWindow * w = new TermWindow("Terminal emulator");
		g_pMainWindow->addWindow(w);
	}
	else
	{
		TermWidget * w = new TermWidget(g_pMainWindow->splitter(), true);
		w->show();
	}
#else
	c->warning("Terminal emulation service not supported (non-KDE4 compilation)");
#endif
	return true;
}

static bool term_module_init(KviModule * m)
{
	g_pTermModule = m;

	KVSM_REGISTER_SIMPLE_COMMAND(m, "open", term_kvs_cmd_open);
	return true;
}

static bool term_module_cleanup(KviModule *)
{
#ifdef COMPILE_KDE4_SUPPORT
	while(!g_pTermWidgetList.empty())
		delete *g_pSocketSpyWindowList.begin();

	while(!g_pTermWindowList.empty())
		(*g_pTermWindowList.begin())->close();
#endif
	return true;
}

KVIRC_MODULE(
    "Term",                                                         // module name
    "4.0.0",                                                        // module version
    "Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
    "Terminal emulator extension based on the KonsolePart KDE class",
    term_module_init,
    0,
    0,
    term_module_cleanup,
    0)
