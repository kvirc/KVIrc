//=============================================================================
//
//   File : kvi_internalcmd.cpp
//   Creation date : Sun Aug 13 2000 19:22:45 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#define _KVI_INTERNALCMD_CPP_

#include "kvi_internalcmd.h"

// FIXME: #warning "LOCALIZE THIS!"

static const char * internalCommandTable[KVI_NUM_INTERNAL_COMMANDS]=
{
	"echo INTERNAL COMMAND ERROR: INDEX OUT OF RANGE",
	"help.open -n -m",
	"help.open -n",
	"about.kvirc",
	"if($option(boolShowGeneralOptionsDialogAsToplevel))options.dialog -t; else options.dialog",
	"codetester.open",
	"socketspy.open",
	"links.open",
	"if($option(boolShowRegisteredUsersDialogAsToplevel))reguser.edit -t; else reguser.edit",
	"trayicon.show",
	"trayicon.hide",
	"toolbareditor.open",
	"aliaseditor.open",
	"term.open -m",
	"eventeditor.open",
	"dirbrowser.open -m",
	"dirbrowser.open",
	"ident.start",
	"ident.stop",
	"tip.open" ,
	"openurl http://www.kvirc.net",
	"list.open",
	"if($option(boolShowChannelsJoinDialogAsToplevel))channelsjoin.open -t;else channelsjoin.open",
	"if($option(boolShowServersConnectDialogAsToplevel))options.edit -t KviServerOptionsWidget;else options.edit KviServerOptionsWidget",
	"url.list",
	"openurl http://www.kvirc.net/?id=themes",
	"openurl http://www.kvirc.net/?id=mailinglist",
	"openurl https://svn.kvirc.de/kvirc/wiki#Bugreportticket",
	"raweditor.open",
	"popupeditor.open",
	"dialog.file(open,\"Select a Script File\"){ if(!$str.isEmpty($0))parse $0; }",
	"actioneditor.open",
	"quit",
	"openurl http://www.kvirc.ru",
	"openurl irc://freenode/kvirc",
	"openurl irc://ircnet/kvirc",
	"openurl irc://azzurra/kvirc.net",
	"openurl http://kvirc-fr.info",
	"openurl irc://freenode/kvirc-fr",
	"openurl irc://irc.europnet.org/kvirc"
};


const char * kvi_getInternalCommandBuffer(int idx)
{
	if(idx > 0 && idx < KVI_NUM_INTERNAL_COMMANDS)
	{
		return internalCommandTable[idx];
	}
	return internalCommandTable[0];
}
