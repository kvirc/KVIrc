#ifndef _KVI_TAL_TOOLBAR_QT4_H_
#define _KVI_TAL_TOOLBAR_QT4_H_
//
//   File : kvi_tal_toolbar_qt4.h
//   Creation date : Wed Feb 1 2007 04:11:11 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)
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
#include <q3toolbar.h>
#include <q3mainwindow.h>

#include "kvi_tal_toolbardocktype.h"

class KVILIB_API KviTalToolBar : public Q3ToolBar
{
	Q_OBJECT
public:
	KviTalToolBar(const QString &label,Q3MainWindow *w,QT_TOOLBARDOCK_TYPE dock = QT_DOCK_TOP,bool bNewLine = false,const char * nam = 0);
	~KviTalToolBar();
};

#endif //_KVI_TAL_TOOLBAR_QT_H_
