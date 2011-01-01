#ifndef _KVINOTIFIERWINDOWBORDER_H_
#define _KVINOTIFIERWINDOWBORDER_H_
//=============================================================================
//
//   File : NotifierWindowBorder.h
//   Creation date : Mon 03 Jan 2005 02:27:22 by Iacopo Palazzi
//
//   This file is part of the KVIrc distribution
//   Copyright (C) 2005-2008 Iacopo Palazzi < iakko(at)siena(dot)linux(dot)it >
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

#include "NotifierSettings.h"

#include <QSize>
#include <QRect>
#include <QPixmap>

class QPainter;

class NotifierWindowBorder
{
public:
	NotifierWindowBorder(QSize = QSize(WDG_MIN_WIDTH,WDG_MIN_HEIGHT));
	~NotifierWindowBorder();

// ================================
// Put members declaration below...
// ================================
private:
	QRect 	m_rct;
	QPoint	m_pnt;

	QRect	m_closeIconRect;

	QRect	m_captionRect;
	QRect	m_bodyRect;
	QRect	m_titleRect;

	// Pictures
	QPixmap * m_pixSX;
	QPixmap * m_pixDX;
	QPixmap * m_pixDWN;
	QPixmap * m_pixDWNSX;
	QPixmap * m_pixDWNDX;
	QPixmap * m_pixCaptionSX;
	QPixmap * m_pixCaptionDX;
	QPixmap * m_pixCaptionBKG;
	QPixmap * m_pixIconClose_out;
	QPixmap * m_pixIconClose_over;
	QPixmap * m_pixIconClose_clicked;
	QPixmap * m_pixIconClose;

	QPixmap m_pixSX_N;
	QPixmap m_pixDX_N;
	QPixmap m_pixDWN_N;
	QPixmap m_pixDWNSX_N;
	QPixmap m_pixDWNDX_N;
	QPixmap m_pixCaptionSX_N;
	QPixmap m_pixCaptionDX_N;
	QPixmap m_pixCaptionBKG_N;
	QPixmap m_pixIconClose_out_N;
	QPixmap m_pixIconClose_over_N;
	QPixmap m_pixIconClose_clicked_N;

	QPixmap m_pixSX_HL;
	QPixmap m_pixDX_HL;
	QPixmap m_pixDWN_HL;
	QPixmap m_pixDWNSX_HL;
	QPixmap m_pixDWNDX_HL;
	QPixmap m_pixCaptionSX_HL;
	QPixmap m_pixCaptionDX_HL;
	QPixmap m_pixCaptionBKG_HL;
	QPixmap m_pixIconClose_out_HL;
	QPixmap m_pixIconClose_over_HL;
	QPixmap m_pixIconClose_clicked_HL;

	int m_eIconState;
	int m_iRctWidth;
	int m_iRctHeight;
private:
	void loadImages();
	void recalculatePositions();

public:
	// reading methods...
	void setWidth(int w);
	void setHeight(int h);
	void resize(int w, int h) { setWidth(w); setHeight(h); };
	void resize(QSize r) { setWidth(r.width()); setHeight(r.height()); };
	void setGeometry(QRect r) { r.topLeft(); r.size(); 	/*qDebug("x,y: %d,%d", r.x(), r.y()); qDebug("w,h: %d,%d", r.width(), r.height());*/ };
	void setGeometry(QPoint p, QSize s) { setPoint (p.x(), p.y()); setWidth (s.width()); setHeight (s.height()); };

	void setPoint(int x, int y) { m_pnt.setX(x); m_pnt.setY(y); m_rct.setX(x); m_rct.setY(y);  };

	void setCloseIcon(int state);
	void resetIcons();

	// writing methods...
	inline int x() const { return m_pnt.x(); };
	inline int y() const { return m_pnt.y(); };
	inline int width() const { return m_rct.width(); };
	inline int height() const { return m_rct.height(); };
	inline int baseLine() const { return (y()+height()); };

	inline QRect bodyRect() const { return m_bodyRect; };
	inline QRect captionRect() const { return m_captionRect; };
	inline QRect rect() const { return m_rct; };
	inline QRect closeRect() const { return m_closeIconRect; };
	inline QRect titleRect() const { return m_titleRect; };

	void draw(QPainter *, bool b = false);
	void setPics(bool b = false);
};

#endif //!_KVINOTIFIERWINDOWBORDER_H_
