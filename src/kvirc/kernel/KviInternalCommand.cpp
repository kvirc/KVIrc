//=============================================================================
//
//   File : KviInternalCommand.cpp
//   Creation date : Sun Aug 13 2000 19:22:45 by Szymon Stefanek
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

#define _KVI_INTERNALCMD_CPP_

#include "KviInternalCommand.h"

// FIXME: #warning "LOCALIZE THIS!"

#define JOIN_CHANNEL_ON_NETWORK(_szChan, _szNet)                         \
	"foreach(%ctx,$context.list)"                                        \
	"{"                                                                  \
	"	if("                                                               \
	"			$str.contains($my.server(%ctx),\"" _szNet "\",false) ||" \
	"			$str.contains($my.network(%ctx),\"" _szNet "\",false)"   \
	"		)"                                                                \
	"	{"                                                                 \
	"		foreach(%chan,$window.list(channel,%ctx))"                        \
	"		{"                                                                \
	"			if($target(%chan) == \"" _szChan "\")"                   \
	"			{"                                                               \
	"				window.activate %chan;"                                         \
	"				return;"                                                        \
	"			}"                                                               \
	"		}"                                                                \
	"		rebind $console(%ctx);"                                           \
	"		join \"" _szChan "\";"                                       \
	"		return;"                                                          \
	"	}"                                                                 \
	"}"                                                                  \
	"server -u -c=\"join " _szChan "\" net:" _szNet ""

static const char * internalCommandTable[KVI_NUM_INTERNAL_COMMANDS] = {
	"echo Internal command error: index out of range",
	"about.kvirc",
	"options.dialog",
	"codetester.open",
	"socketspy.open",
	"links.open",
	"reguser.edit",
	"trayicon.show",
	"trayicon.hide",
	"toolbareditor.open",
	"aliaseditor.open",
	"term.open -m",
	"eventeditor.open",
	"ident.start",
	"ident.stop",
	"tip.open",
	"openurl http://www.kvirc.net",
	"list.open",
	"channelsjoin.open",
	"options.edit -n OptionsWidget_servers",
	"url.list",
	"openurl http://www.kvirc.net/?id=themes",
	"openurl https://github.com/kvirc/KVIrc/issues",
	"raweditor.open",
	"popupeditor.open",
	"dialog.file(open,$tr(\"Select a File - KVIrc\")){ if(!$str.isEmpty($0))parse $0; }",
	"actioneditor.open",
	"quit",
	"openurl http://www.kvirc.ru",
	JOIN_CHANNEL_ON_NETWORK("#KVIrc", "freenode")
};

const char * kvi_getInternalCommandBuffer(int idx)
{
	if(idx > 0 && idx < KVI_NUM_INTERNAL_COMMANDS)
		return internalCommandTable[idx];
	return internalCommandTable[0];
}
