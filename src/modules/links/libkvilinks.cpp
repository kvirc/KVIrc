//=============================================================================
//
//   File : libkvilinks.cpp
//   Creation date : Thu Dec 21 2000 12:39:12 CEST by Szymon Stefanek
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

#include "LinksWindow.h"

#include "KviModule.h"
#include "KviMainWindow.h"
#include "KviWindow.h"
#include "KviLocale.h"

#include <QSplitter>
#include <unordered_set>

std::unordered_set<LinksWindow *> g_pLinksWindowList;

/*
	@doc: links.open
	@type:
		command
	@title:
		links.open
	@short:
		Opens a links window
	@syntax:
		links.open
	@description:
		Opens a links window attached to the current IRC context.[br]
		The links window handles the RPL_LINKS server replies and shows
		them in a [b]tree-view[/b] form: this is useful in visualizing the
		current network connections.[br]
		Please note that the total number of links received (and the
		total count of servers displayed when all the links have been received)
		may actually be less than the real number of IRC servers in the network.
		Servers that contain a wildcard in their name often act as gateways (hubs)
		for a [i]local IRC network[/i]; if you're requesting links from a server that is
		external to the gateway, the servers internal to the network [i]behind the gateway[/i]
		will not be shown; in the extreme case you will see the gateway as leaf node (and it isn't).
		To see the internal network server tree you might request links from the gateway server.[br]
*/

static bool links_kvs_cmd_open(KviKvsModuleCommandCall * c)
{

	// FIND ME
	if(!c->window()->context())
		return c->context()->errorNoIrcContext();

	if(!(c->window()->context()->linksWindow()))
	{
		LinksWindow * w = new LinksWindow(c->window()->console());
		g_pMainWindow->addWindow(w);
	}
	else
	{
		c->warning(__tr2qs("Links window already open for this IRC context"));
	}

	return true;
}

static bool links_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m, "open", links_kvs_cmd_open);
	return true;
}

static bool links_module_cleanup(KviModule *)
{
	while (!g_pLinksWindowList.empty())
		(*g_pLinksWindowList.begin())->die();
	return true;
}

static bool links_module_can_unload(KviModule *)
{
	return (g_pLinksWindowList.empty());
}

KVIRC_MODULE(
    "Links",                                                             // module name
    "4.0.0",                                                             // module version
    "Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
    "Links window extension",
    links_module_init,
    links_module_can_unload,
    0,
    links_module_cleanup,
    0)
