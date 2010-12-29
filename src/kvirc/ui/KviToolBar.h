#ifndef _KVI_TOOLBAR_H_
#define _KVI_TOOLBAR_H_
//=============================================================================
//
//   File : KviToolBar.h
//   Creation date : Tue Sep 17 02:00:16 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviTalToolBar.h"

class KVIRC_API KviToolBar : public KviTalToolBar
{
	Q_OBJECT
public:
	KviToolBar(const QString & szLabel, Qt::ToolBarArea type = Qt::TopToolBarArea, bool bNewLine = false, const char * pcName = 0);
	~KviToolBar();
protected:
	virtual void mousePressEvent(QMouseEvent * e);
public slots:
	void setBigIcons();
	void setSmallIcons();
};

#endif //_KVI_TOOLBAR_H_
