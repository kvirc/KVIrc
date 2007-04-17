#ifndef _KVINOTIFIERWINDOWBODY_H_
#define _KVINOTIFIERWINDOWBODY_H_
//=============================================================================
//
//   File : kvinotifierwindowbody.h
//   Created on dom 02 gen 2005 17:53:29 by Iacopo Palazzi
//
//   This file is part of the KVIrc distribution
//   Copyright (C) 2005 Iacopo Palazzi < iakko(at)siena(dot)linux(dot)it >
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

#include <qevent.h>
#include <qrect.h>
#include <qpoint.h>
#include <qpixmap.h>

#include "notifiersettings.h"

/*
#ifdef QT3_SUPPORT
	#define QPainter Q3Painter
#endif
*/

class QPainter;

class KviNotifierWindowBody
{
public:
	KviNotifierWindowBody(QRect r);
	~KviNotifierWindowBody();

// ================================
// Put members declaration below...
// ================================
private:
	QRect 	m_rct;
	QPoint	m_pnt;
	
	QRect	m_textRect;
	
	QRect	m_rctWriteIcon;
	QRect	m_rctPrevIcon;
	QRect	m_rctNextIcon;
	QRect	m_rctCloseTabIcon;

	// Pictures
	QPixmap	m_pixDX;
	QPixmap	m_pixSX;
	QPixmap	m_pixDWNSX;
	QPixmap	m_pixDWNDX;
	QPixmap	m_pixDWN;
	QPixmap	m_pixKVIrcSX;
	QPixmap	m_pixKVIrcDWN;
	QPixmap	m_pixKVIrc;
	
	QPixmap m_pixIconPrev;
	QPixmap m_pixIconPrev_off;
	QPixmap m_pixIconPrev_on;
	QPixmap m_pixIconPrev_clicked;
	
	QPixmap m_pixIconNext;
	QPixmap m_pixIconNext_off;
	QPixmap m_pixIconNext_on;
	QPixmap m_pixIconNext_clicked;
	
	QPixmap m_pixIconWrite;
	QPixmap m_pixIconWrite_off;
	QPixmap m_pixIconWrite_on;
	QPixmap m_pixIconWrite_clicked;
	
	bool m_bNeedToRedraw;

	int m_prevIconState;
	int m_nextIconState;
	int m_writeIconState;

public:

protected:


// ================================
// Put methods declaration below...
// ================================
private:
	void loadImages();

public:
	void setWidth(int w);
	void setHeight(int h);
	void resize(int w, int h) { setWidth(w); setHeight(h); };
	void resize(QRect r) { setWidth(r.width()); setHeight(r.height()); };
	void setPoint(int x, int y) { m_pnt.setX(x); m_pnt.setY(y); };

	void setPrevIcon(int state);
	void setNextIcon(int state);
	void setWriteIcon(int state);
	
	int nextIconState() { return m_nextIconState; };
	int prevIconState() { return m_prevIconState; };
	int writeIconState() { return m_writeIconState; };

	int x() { return m_pnt.x(); };
	int y() { return m_pnt.y(); };
	int width() { return m_rct.width(); };
	int height() { return m_rct.height(); };
	int baseLine() { return (y()+height()); };
	
	QRect rect() { return m_rct; };
	QRect textRect() { return m_textRect; };
	QRect rctPrevIcon() { return m_rctPrevIcon; };
	QRect rctNextIcon() { return m_rctNextIcon; };
	QRect rctWriteIcon() { return m_rctWriteIcon; };

	void needToRedraw() { m_bNeedToRedraw = true; };

	void recalculatePositions();
	void touch() { m_bNeedToRedraw = true; };
	void draw(QPainter *);
	
	void mouseMoveEvent(QMouseEvent * e);
	void mousePressEvent(QMouseEvent * e);
	void mouseReleaseEvent(QMouseEvent * e);
	
	

protected:

};

class KviNotifierWindowProgressBar
{
public:
	KviNotifierWindowProgressBar(QRect r);
	~KviNotifierWindowProgressBar();
protected:
	QRect 	m_rect;
	double	m_dProgress;
public:
	void setProgress(double progress) { m_dProgress=progress; };
	void draw(QPainter *);
};
#endif //!_KVINOTIFIERWINDOWBODY_H_
