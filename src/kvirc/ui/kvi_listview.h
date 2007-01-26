#ifndef _KVI_LISTVIEW_H_
#define _KVI_LISTVIEW_H_

///////////////////////////////////////////////////////////////////////////////
//
//   File : kvi_listview.h
//   Creation date : 19  Jan 2006 GMT by Alexey Uzhva
//
//   This toolbar is part of the KVirc irc client distribution
//   Copyright (C) 2006 Alexey Uzhva
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
///////////////////////////////////////////////////////////////////////////////

#include "kvi_heapobject.h"

#include "kvi_tal_listview.h"

class QPainter;

class KVIRC_API KviListView : public KviTalListView
{
	Q_OBJECT
public:
	KviListView( QWidget * parent = 0, const char * name = 0);
	~KviListView();
protected:
	QPixmap      * m_pBackgroundOverlayPixmap;
	int            m_iBackgroundOverlayAlignment;
public:
	virtual void setBackgroundOverlayPixmap(QPixmap* pix,int iAlignmentFlags = Qt::AlignAuto);
public:
	void publicUpdateContents(){ updateContents(); }; // <-- unscrewContents(); :D
protected:
	virtual void paintEmptyArea(QPainter * p, const QRect & rect );
	virtual void drawContentsOffset(QPainter * p,int ox,int oy,int cx,int cy,int cw,int ch);
	virtual void resizeEvent(QResizeEvent * e);
#ifdef COMPILE_ON_WINDOWS
	virtual void focusInEvent(QFocusEvent * e);
	virtual void focusOutEvent(QFocusEvent * e);
#endif
};

#endif //!_KVI_LISTVIEW_H_
