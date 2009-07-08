#ifndef _KVI_TAL_SCROLLVIEW_H_
#define _KVI_TAL_SCROLLVIEW_H_

//=============================================================================
//
//   File : kvi_tal_scrollview.h
//   Creation date : Mon May 05 2008 11:25:08 by Alessandro Carbone
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include "kvi_settings.h"

#include <QScrollArea>

class KVILIB_API KviTalScrollView : public QScrollArea
{
	Q_OBJECT
public:
	KviTalScrollView(QWidget * pParent)
	: QScrollArea(pParent) {};
	virtual ~KviTalScrollView() {};
	int childX(QWidget * child);
	int childY(QWidget * child);
	void moveChild(QWidget * child, int x,int y);
	void resizeContents(int w, int h);
	void addChild(QWidget * child, int x, int y);
	int visibleWidth();
	int visibleHeight();
};

#endif // _KVI_TAL_SCROLLVIEW_QT_H_
