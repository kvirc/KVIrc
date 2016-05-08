//=============================================================================
//
//   File : libkviabout.cpp
//   Creation date : Tue Jul 6 1999 03:54:25 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2000 Christian Strmmen
//   Copyright (C) 2000 Till Bush (buti at geocities dot com)
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

#include "AboutDialog.h"

#include "KviModule.h"
#include "KviLocale.h"

AboutDialog * g_pAboutDialog = nullptr;

/*
	@doc: about.kvirc
	@type:
		command
	@title:
		about.kvirc
	@short:
		Shows the About KVIrc dialog
	@syntax:
		about.kvirc
	@description:
		This command is exported by the [i]about[/i] module.
		This command opens a dialog which contains various tabs ranging from important to interesting information.
		[b]About[/b] tab: shows the current KVIrc base version, release name and splash screen.[br]
		[b]Executable Information[/b] tab: contains important system and program information you can [b]copy and paste[/b] into e.g. a bug report or support request.[br]
		[b]Honor & Glory[/b] tab: contains a non exhaustive list of the people who have contributed to help make KVIrc the IRC client you know and use today.[br]
		[b]Licence[/b] tab: contains a copy (if present in the KVIrc package) of the license type and details associated with the KVIrc package.
	@examples:
		[example]
			about.kvirc
		[/example]
*/

static bool about_kvs_command_kvirc(KviKvsModuleCommandCall *)
{
	if(!g_pAboutDialog)
	{
		g_pAboutDialog = new AboutDialog();
		g_pAboutDialog->show();
	}
	else
	{
		g_pAboutDialog->raise();
		g_pAboutDialog->show();
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// init routine
///////////////////////////////////////////////////////////////////////////////

static bool about_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m, "kvirc", about_kvs_command_kvirc)
	return true;
}

static bool about_module_cleanup(KviModule *)
{
	if(g_pAboutDialog)
		delete g_pAboutDialog;
	g_pAboutDialog = nullptr;
	return true;
}

static bool about_module_can_unload(KviModule *)
{
	return (!g_pAboutDialog);
}

KVIRC_MODULE(
    "KVIrc about dialog",
    "4.0.0",
    "Till Bush <buti@geocities.com>\n"
    "Szymon Stefanek <pragma at kvirc dot net>"
    "Elvio Basello <hellvis69 at netsons dot org>",
    "Exports the /about.kvirc command\n",
    about_module_init,
    about_module_can_unload,
    0,
    about_module_cleanup,
    "about")
