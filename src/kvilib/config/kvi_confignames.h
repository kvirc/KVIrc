#ifndef _KVI_CONFIGNAMES_H_
#define _KVI_CONFIGNAMES_H_
//=============================================================================
//
//   File : kvi_confignames.h
//   Creation date : Wed Oct 18 2000 14:16:22 CEST by Szymon Stefanek
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

/**
* \file kvi_confignames.h
* \author Szymon Stefanek
* \brief This file contains the config file names used in KVIrc
*
* You can play a bit with these, but be aware that some (bad) scripts may find
* themselves in trouble
*
* \def KVI_CONFIGFILE_MAIN Defines main.kvc
* \def KVI_CONFIGFILE_WINPROPERTIES Defines winproperties.kvc
* \def KVI_CONFIGFILE_SERVERDB Defines serverdb.kvc
* \def KVI_CONFIGFILE_PROXYDB Defines proxydb.kvc
* \def KVI_CONFIGFILE_EVENTS Defines events.kvc
* \def KVI_CONFIGFILE_RAWEVENTS Defines rawevents.kvc
* \def KVI_CONFIGFILE_POPUPS Defines popups.kvc
* \def KVI_CONFIGFILE_SCRIPTTOOLBARS Defines toolbars.kvc
* \def KVI_CONFIGFILE_CUSTOMTOOLBARS Defines customtoolbars.kvc
* \def KVI_CONFIGFILE_MEDIATYPES Defines mediatypes.kvc
* \def KVI_CONFIGFILE_REGUSERDB Defines reguserdb.kvc
* \def KVI_CONFIGFILE_SHAREDFILES Defines sharedfiles.kvc
* \def KVI_CONFIGFILE_RECENT Defines recent.kvc
* \def KVI_CONFIGFILE_TOOLBARS Defines toolbarpos.kvc
* \def KVI_CONFIGFILE_ALIASES Defines aliases.kvc
* \def KVI_CONFIGFILE_NICKSERVDATABASE Defines nickserv.kvc
* \def KVI_CONFIGFILE_PROFILESDATABASE Defines identityprofiles.kvc
* \def KVI_CONFIGFILE_TEXTICONS Defines texticons.kvc
* \def KVI_CONFIGFILE_REGCHANDB Defines regchan.kvc
* \def KVI_CONFIGFILE_INPUTHISTORY Defines inputhistory.kvc
* \def KVI_CONFIGFILE_AVATARCACHE Defines avatarcache.kvc
* \def KVI_CONFIGFILE_USERACTIONS Defines useractions.kvc
* \def KVI_CONFIGFILE_SCRIPTADDONS Defines scriptaddons.kvc
* \def KVI_CONFIGFILE_IDENTITIES Defines identities.kvc
* \def KVI_CONFIGFILE_DEFAULTSCRIPT Defines default.kvc
*/

#include "kvi_fileextensions.h"

#define KVI_CONFIGFILE_MAIN "main" KVI_FILEEXTENSION_CONFIG
#define KVI_CONFIGFILE_WINPROPERTIES "winproperties" KVI_FILEEXTENSION_CONFIG
#define KVI_CONFIGFILE_SERVERDB "serverdb" KVI_FILEEXTENSION_CONFIG
#define KVI_CONFIGFILE_PROXYDB "proxydb" KVI_FILEEXTENSION_CONFIG
#define KVI_CONFIGFILE_EVENTS "events" KVI_FILEEXTENSION_CONFIG
#define KVI_CONFIGFILE_RAWEVENTS "rawevents" KVI_FILEEXTENSION_CONFIG
#define KVI_CONFIGFILE_POPUPS "popups" KVI_FILEEXTENSION_CONFIG
#define KVI_CONFIGFILE_SCRIPTTOOLBARS "toolbars" KVI_FILEEXTENSION_CONFIG
#define KVI_CONFIGFILE_CUSTOMTOOLBARS "customtoolbars" KVI_FILEEXTENSION_CONFIG
#define KVI_CONFIGFILE_MEDIATYPES "mediatypes" KVI_FILEEXTENSION_CONFIG
#define KVI_CONFIGFILE_REGUSERDB "reguserdb" KVI_FILEEXTENSION_CONFIG
#define KVI_CONFIGFILE_SHAREDFILES "sharedfiles" KVI_FILEEXTENSION_CONFIG
#define KVI_CONFIGFILE_RECENT "recent" KVI_FILEEXTENSION_CONFIG
#define KVI_CONFIGFILE_TOOLBARS "toolbarpos" KVI_FILEEXTENSION_CONFIG
#define KVI_CONFIGFILE_ALIASES "aliases" KVI_FILEEXTENSION_CONFIG
#define KVI_CONFIGFILE_NICKSERVDATABASE "nickserv" KVI_FILEEXTENSION_CONFIG
#define KVI_CONFIGFILE_PROFILESDATABASE "identityprofiles" KVI_FILEEXTENSION_CONFIG
#define KVI_CONFIGFILE_TEXTICONS "texticons" KVI_FILEEXTENSION_CONFIG
#define KVI_CONFIGFILE_REGCHANDB "regchan" KVI_FILEEXTENSION_CONFIG
#define KVI_CONFIGFILE_INPUTHISTORY "inputhistory" KVI_FILEEXTENSION_CONFIG
#define KVI_CONFIGFILE_AVATARCACHE "avatarcache" KVI_FILEEXTENSION_CONFIG
#define KVI_CONFIGFILE_USERACTIONS "useractions" KVI_FILEEXTENSION_CONFIG
#define KVI_CONFIGFILE_SCRIPTADDONS "scriptaddons" KVI_FILEEXTENSION_CONFIG
#define KVI_CONFIGFILE_IDENTITIES "identities" KVI_FILEEXTENSION_CONFIG
#define KVI_CONFIGFILE_DEFAULTSCRIPT "default" KVI_FILEEXTENSION_CONFIG

#endif //_KVI_CONFIGNAMES_H_
