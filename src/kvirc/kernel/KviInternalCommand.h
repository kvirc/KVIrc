#ifndef _KVI_INTERNALCMD_H_
#define _KVI_INTERNALCMD_H_
//=============================================================================
//
//   File : KviInternalCommand.h
//   Creation date : Sun Aug 13 2000 19:21:12 by Szymon Stefanek
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

// These unused options have been as such since at least KVIrc 3.x

#define KVI_INTERNALCOMMAND_ECHO_INTERNALERROR 0           // Unused
#define KVI_INTERNALCOMMAND_ABOUT_ABOUTKVIRC 1
#define KVI_INTERNALCOMMAND_OPTIONS_DIALOG 2
#define KVI_INTERNALCOMMAND_CODETESTER_OPEN 3              // Unused
#define KVI_INTERNALCOMMAND_SOCKETSPY_OPEN 4               // Unused
#define KVI_INTERNALCOMMAND_LINKS_OPEN 5                   // Unused
#define KVI_INTERNALCOMMAND_REGUSER_EDIT 6                 // Unused
#define KVI_INTERNALCOMMAND_TRAYICON_SHOW 7
#define KVI_INTERNALCOMMAND_TRAYICON_HIDE 8
#define KVI_INTERNALCOMMAND_TOOLBAREDITOR_OPEN 9           // Unused
#define KVI_INTERNALCOMMAND_ALIASEDITOR_OPEN 10            // Unused
#define KVI_INTERNALCOMMAND_TERM_OPEN 11                   // Unused
#define KVI_INTERNALCOMMAND_EVENTEDITOR_OPEN 12            // Unused
#define KVI_INTERNALCOMMAND_IDENT_START 13
#define KVI_INTERNALCOMMAND_IDENT_STOP 14
#define KVI_INTERNALCOMMAND_TIP_OPEN 15
#define KVI_INTERNALCOMMAND_KVIRC_HOMEPAGE 16
#define KVI_INTERNALCOMMAND_LIST_OPEN 17                   // Unused
#define KVI_INTERNALCOMMAND_CHANNELSJOIN_OPEN 18
#define KVI_INTERNALCOMMAND_SERVERSJOIN_OPEN 19
#define KVI_INTERNALCOMMAND_URL_OPEN 20                    // Unused
#define KVI_INTERNALCOMMAND_OPENURL_KVIRC_THEMES 21
#define KVI_INTERNALCOMMAND_OPENURL_KVIRC_BUGTRACK 22
#define KVI_INTERNALCOMMAND_RAWEDITOR_OPEN 23              // Unused
#define KVI_INTERNALCOMMAND_POPUPEDITOR_OPEN 24            // Unused
#define KVI_INTERNALCOMMAND_EXECUTE_SCRIPT_FROM_DISK 25    // Unused
#define KVI_INTERNALCOMMAND_ACTIONEDITOR_OPEN 26           // Unused
#define KVI_INTERNALCOMMAND_QUIT 27
#define KVI_INTERNALCOMMAND_KVIRC_HOMEPAGE_RU 28
#define KVI_INTERNALCOMMAND_JOIN_KVIRC_ON_FREENODE 29

#define KVI_NUM_INTERNAL_COMMANDS 30

extern KVIRC_API const char * kvi_getInternalCommandBuffer(int idx);

#endif //_KVI_INTERNALCMD_H_
