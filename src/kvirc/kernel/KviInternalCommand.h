#ifndef _KVI_INTERNALCMD_H_
#define _KVI_INTERNALCMD_H_
//=============================================================================
//
//   File : KviInternalCommand.h
//   Creation date : Sun Aug 13 2000 19:21:12 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"

#define KVI_INTERNALCOMMAND_ECHO_INTERNALERROR 0
#define KVI_INTERNALCOMMAND_HELP_NEWMDIWINDOW 1
#define KVI_INTERNALCOMMAND_HELP_NEWSTATICWINDOW 2
#define KVI_INTERNALCOMMAND_ABOUT_ABOUTKVIRC 3
#define KVI_INTERNALCOMMAND_OPTIONS_DIALOG 4
#define KVI_INTERNALCOMMAND_CODETESTER_OPEN 5
#define KVI_INTERNALCOMMAND_SOCKETSPY_OPEN 6
#define KVI_INTERNALCOMMAND_LINKS_OPEN 7
#define KVI_INTERNALCOMMAND_REGUSER_EDIT 8
#define KVI_INTERNALCOMMAND_TRAYICON_SHOW 9
#define KVI_INTERNALCOMMAND_TRAYICON_HIDE 10
#define KVI_INTERNALCOMMAND_TOOLBAREDITOR_OPEN 11
#define KVI_INTERNALCOMMAND_ALIASEDITOR_OPEN 12
#define KVI_INTERNALCOMMAND_TERM_OPEN 13
#define KVI_INTERNALCOMMAND_EVENTEDITOR_OPEN 14
#define KVI_INTERNALCOMMAND_BROWSER_OPEN_MDI 15
#define KVI_INTERNALCOMMAND_BROWSER_OPEN 16
#define KVI_INTERNALCOMMAND_IDENT_START 17
#define KVI_INTERNALCOMMAND_IDENT_STOP 18
#define KVI_INTERNALCOMMAND_TIP_OPEN 19
#define KVI_INTERNALCOMMAND_KVIRC_HOMEPAGE 20
#define KVI_INTERNALCOMMAND_LIST_OPEN 21
#define KVI_INTERNALCOMMAND_CHANNELSJOIN_OPEN 22
#define KVI_INTERNALCOMMAND_SERVERSJOIN_OPEN 23
#define KVI_INTERNALCOMMAND_URL_OPEN 24
#define KVI_INTERNALCOMMAND_OPENURL_KVIRC_THEMES 25
#define KVI_INTERNALCOMMAND_OPENURL_KVIRC_MAILINGLIST 26
#define KVI_INTERNALCOMMAND_OPENURL_KVIRC_BUGTRACK 27
#define KVI_INTERNALCOMMAND_RAWEDITOR_OPEN 28
#define KVI_INTERNALCOMMAND_POPUPEDITOR_OPEN 29
#define KVI_INTERNALCOMMAND_EXECUTE_SCRIPT_FROM_DISK 30
#define KVI_INTERNALCOMMAND_ACTIONEDITOR_OPEN 31
#define KVI_INTERNALCOMMAND_QUIT 32
#define KVI_INTERNALCOMMAND_KVIRC_HOMEPAGE_RU 33
#define KVI_INTERNALCOMMAND_OPENURL_KVIRC_ON_FREENODE 34
#define KVI_INTERNALCOMMAND_OPENURL_KVIRC_ON_IRCNET 35
#define KVI_INTERNALCOMMAND_OPENURL_KVIRC_IT_ON_AZZURRA 36
#define KVI_INTERNALCOMMAND_KVIRC_HOMEPAGE_FR 37
#define KVI_INTERNALCOMMAND_OPENURL_KVIRC_FR_ON_FREENODE 38
#define KVI_INTERNALCOMMAND_OPENURL_KVIRC_FR_ON_EUROPNET 39

#define KVI_NUM_INTERNAL_COMMANDS 40

extern KVIRC_API const char * kvi_getInternalCommandBuffer(int idx);

#endif //_KVI_INTERNALCMD_H_
