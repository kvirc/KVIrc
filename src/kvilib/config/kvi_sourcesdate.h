#ifndef _KVI_SOURCESDATE_H_
#define _KVI_SOURCESDATE_H_
//=============================================================================
//
//   File : kvi_sourcesdate.h
//   Creation date : Sun Mar 5 2006 23:22:22 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2006-2010 Szymon Stefanek (pragma at kvirc dot net)
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
* \file kvi_sourcesdate.h
* \author Szymon Stefanek
* \brief Sources date
*/

#include "kvi_settings.h"
#include "kvi_sysbuildinfo.h"

/**
* \def KVI_SOURCES_DATE Defines the sources date
* \def KVI_SOURCES_DATE_NUMERIC Defines the sources date
*/
#ifdef KVIRC_SOURCES_DATE_GIT
#define KVI_SOURCES_DATE KVIRC_SOURCES_DATE_GIT
#define KVI_SOURCES_DATE_NUMERIC KVIRC_SOURCES_DATE_GIT_HEX
#else
#define KVI_SOURCES_DATE "20160102"
#define KVI_SOURCES_DATE_NUMERIC 0x20160102
#endif

/**
* \brief This is the date of the sources before that we should force a setup
*
* This is usually changed only when a setup is really needed because of an
* incompatible change in the configuration files
* .. so do NOT change it just when you change KVI_SOURCES_DATE_NUMERIC :)
* \def KVI_SOURCES_DATE_NUMERIC_FORCE_SETUP Defines a forced setup
*/
#define KVI_SOURCES_DATE_NUMERIC_FORCE_SETUP 0x20080324

#endif //!_KVI_SOURCESDATE_H_
