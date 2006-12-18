#ifndef _KVI_TAL_TOOLBARDOCKTYPE_H_
#define _KVI_TAL_TOOLBARDOCKTYPE_H_
//
//   File : kvi_tal_toolbardocktype.h
//   Creation date : Tue Sep 17 02:11:28 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"


#if QT_VERSION >= 300
	#define QT_TOOLBARDOCK_TYPE Qt::Dock
	#define QT_DOCK_TOP Qt::DockTop
	#define QT_DOCK_LEFT Qt::DockLeft
	#define QT_DOCK_RIGHT Qt::DockRight
	#define QT_DOCK_BOTTOM Qt::DockBottom
	#define QT_DOCK_MINIMIZED Qt::DockMinimized
	#define QT_DOCK_TORNOFF Qt::DockTornOff
	#define QT_DOCK_UNMANAGED Qt::DockUnmanaged
#else
	#define QT_TOOLBARDOCK_TYPE QMainWindow::ToolBarDock
	#define QT_DOCK_TOP QMainWindow::Top
	#define QT_DOCK_LEFT QMainWindow::Left
	#define QT_DOCK_RIGHT QMainWindow::Right
	#define QT_DOCK_BOTTOM QMainWindow::Bottom
	#define QT_DOCK_MINIMIZED QMainWindow::Minimized
	#define QT_DOCK_TORNOFF QMainWindow::TornOff
	#define QT_DOCK_UNMANAGED QMainWindow::Unmanaged
#endif

#endif //_KVI_TAL_TOOLBARDOCKTYPE_H_
