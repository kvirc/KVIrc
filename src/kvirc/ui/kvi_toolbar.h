#ifndef _KVI_TOOLBAR_H_
#define _KVI_TOOLBAR_H_
//=============================================================================
//
//   File : kvi_toolbar.h
//   Creation date : Tue Sep 17 02:00:16 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2004 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_tal_toolbar.h"

class KviFrame;

class KVIRC_API KviToolBar : public KviTalToolBar
{
	Q_OBJECT
public:
	KviToolBar(const QString &label,QT_TOOLBARDOCK_TYPE dock = QT_DOCK_TOP,bool bNewLine = false,const char * nam = 0);
	~KviToolBar();
protected:
	virtual void mousePressEvent(QMouseEvent *e);
	void moveTo(QT_TOOLBARDOCK_TYPE dock);
public slots:
	void moveToTop();
	void moveToLeft();
	void moveToRight();
	void moveToBottom();
	void moveToMinimized();
	void moveToTornOff();
	void setBigIcons();
	void setSmallIcons();
};


#endif //_KVI_TOOLBAR_H_
