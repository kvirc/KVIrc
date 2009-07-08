#ifndef _KVI_PROCESS_H_
#define _KVI_PROCESS_H_
//=============================================================================
//
//   File : kvi_process.h
//   Creation date : Tue Jan 30 2007 04:05:41 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007-2008 Szymon Stefanek (pragma at kvirc dot net)
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

/**
* \file kvi_process.h
* \author Szymon Stefanek
* \brief A simple name redefinition
*
* \def KviProcess Defines KviProcess as QProcess
*/
#include "kvi_settings.h"

#include <QProcess>
#define KviProcess QProcess

#endif //_KVI_PROCESS_H_
