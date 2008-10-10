#ifndef _KVI_TAL_MAINWINDOW_H_
#define _KVI_TAL_MAINWINDOW_H_
//=============================================================================
//
//   File : kvi_tal_mainwindow.h
//   Creation date : Sun Aug 12 2001 04:41:08 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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

/**
* \file kvi_tal_mainwindow.h
* \author Szymon Stefanek
* \brief Switcher between Qt and KDE classes
*/

#include "kvi_settings.h"

#ifdef COMPILE_KDE_SUPPORT
	#include "kvi_tal_mainwindow_kde.h"
#else
	#include "kvi_tal_mainwindow_qt.h"
#endif

#endif // _KVI_TAL_MAINWINDOW_H_
