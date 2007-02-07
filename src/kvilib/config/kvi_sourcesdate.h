
#ifndef _KVI_SOURCESDATE_H_
#define _KVI_SOURCESDATE_H_

//=============================================================================
//
//   File : kvi_sourcesdate.h
//   Creation date : Sun Mar 5 2006 23:22:22 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2006 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"

#define KVI_SOURCES_DATE "20070207"
#define KVI_SOURCES_DATE_NUMERIC 0x20070207

// this is the date of the sources before that we should force a setup
// this is usually changed only when a setup is really needed because
// of an incompatible change in the configuration files
// .. so do NOT change it just when you change KVI_SOURCES_DATE_NUMERIC :)
#define KVI_SOURCES_DATE_NUMERIC_FORCE_SETUP 0x20060305

#endif //!_KVI_SOURCESDATE_H_
