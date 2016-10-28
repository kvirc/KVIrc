//=============================================================================
//
//   File : libkvichannelsjoin.cpp
//   Creation date : Thu Nov 06 2001 12:30:25 CEST by Juanjo Alvarez
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001 Juanjo Alvarez
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviModule.h"
#include "ChannelsJoinDialog.h"
#include "KviConfigurationFile.h"
#include "KviLocale.h"
#include "KviMainWindow.h"
#include "KviWindow.h"

#include <QSplitter>

ChannelsJoinDialog * g_pChannelsWindow = nullptr;
QRect g_rectChannelsJoinGeometry;

/*
 	@doc: channelsjoin.open
	@type:
		command
	@title:
		channelsjoin.open
	@short:
		Opens a window to join channels
	@syntax:
		channelsjoin.open
	@description:
		Shows a dialog that allows the user to enter channels in a visual manner.[br]
		This command is exported by the [b][i]channelsjoin[/i][/b] module.

*/

static bool channelsjoin_kvs_cmd_open(KviKvsModuleCommandCall * c)
{
	if(!g_pChannelsWindow)
		g_pChannelsWindow = new ChannelsJoinDialog("channelsjoin");

	g_pChannelsWindow->setConsole(c->window()->console());

	g_pChannelsWindow->show();
	g_pChannelsWindow->raise();
	g_pChannelsWindow->setFocus();

	return true;
}

static bool channelsjoin_module_init(KviModule * m)
{
	QString fName;
	m->getDefaultConfigFileName(fName);
	KviConfigurationFile cfg(fName, KviConfigurationFile::Read);

	g_rectChannelsJoinGeometry = cfg.readRectEntry("geometry", QRect(30, 30, 320, 410));

	KVSM_REGISTER_SIMPLE_COMMAND(m, "open", channelsjoin_kvs_cmd_open);
	return true;
}

static bool channelsjoin_module_cleanup(KviModule * m)
{
	QString fName;
	m->getDefaultConfigFileName(fName);
	KviConfigurationFile cfg(fName, KviConfigurationFile::Write);

	cfg.writeEntry("geometry", g_rectChannelsJoinGeometry);

	if(g_pChannelsWindow)
		delete g_pChannelsWindow;
	g_pChannelsWindow = nullptr;
	return true;
}

static bool channelsjoin_module_can_unload(KviModule *)
{
	return (!g_pChannelsWindow);
}

KVIRC_MODULE(
    "ChannelsJoin",
    "4.0.0",
    "Copyright (C) 2001-2006 Juanjo Alvarez (juanjux@yahoo.es), Alexey (wizard@opendoor.ru)",
    "Window to join channels in a GUI",
    channelsjoin_module_init,
    channelsjoin_module_can_unload,
    0,
    channelsjoin_module_cleanup,
    0)
