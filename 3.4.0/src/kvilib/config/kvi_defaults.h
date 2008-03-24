#ifndef _KVI_DEFAULTS_H_
#define _KVI_DEFAULTS_H_

//=============================================================================
//
//   File : kvi_defaults.h
//   Creation date : Wed Jun 21 2000 13:23:13 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2004 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

//=============================================================================
// This file contains (more or less) freely customizable compile time defaults
//
// You can safely play with the ones you understand the meaning of
//=============================================================================

#include "kvi_settings.h"

#ifdef COMPILE_ON_WINDOWS
	#define KVI_HOME_CONFIG_FILE_NAME "kvirc.ini"
#else
	#define KVI_HOME_CONFIG_FILE_NAME ".kvirc.rc"
#endif


#define KVI_DEFAULT_INCOMING_SUBDIRECTORY_NAME "download"

#define KVI_DEFAULT_NICKNAME1 "newbie"
#define KVI_DEFAULT_NICKNAME2 "[newbie]"
#define KVI_DEFAULT_NICKNAME3 "_newbie_"
#define KVI_DEFAULT_NICKNAME4 "newbie4"
#define KVI_DEFAULT_USERNAME "kvirc"
#define KVI_DEFAULT_REALNAME "KVIrc " VERSION " '" KVI_RELEASE_NAME "' http://kvirc.net/"

#define KVI_DEFAULT_PART_MESSAGE "No matter how dark the night, somehow the Sun rises once again"
#define KVI_DEFAULT_QUIT_MESSAGE "KVIrc $version $version(r) http://www.kvirc.net/"
#define KVI_DEFAULT_CTCP_USERINFO_REPLY "I'm too lazy to edit this field."
#define KVI_DEFAULT_CTCP_PAGE_REPLY "Your message has been received and logged"
#define KVI_DEFAULT_AWAY_MESSAGE "I'm off to see the wizard."
#define KVI_DEFAULT_AWAY_NICK "%nick%|NotHere"

#define KVI_MAX_PENDING_AVATARS 20
#define KVI_MAX_RECENT_SERVERS 10
#define KVI_MAX_RECENT_CHANNELS 30
#define KVI_MAX_RECENT_NICKNAMES 10
#define KVI_RECENT_TOPIC_ENTRIES 20

#endif //_KVI_DEFAULTS_H_
