#ifndef _KVINOTIFIERWINDOWBORDER_H_
#define _KVINOTIFIERWINDOWBORDER_H_
//=============================================================================
//
//   File : notifierwindowborder.h
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "notifiersettings.h"

#include <QSize>
#include <QRect>
#include <QPixmap>

class QPainter;

class KviNotifierWindowBorder
{
public:
	KviNotifierWindowBorder(QSize = QSize(WDG_MIN_WIDTH,WDG_MIN_HEIGHT));
	~KviNotifierWindowBorder();

// ================================
// Put members declaration below...
// ================================
private:
	QRect 	m_rct;
	QPoint	m_pnt;

	QRect	m_closeIconRect;
	
	QRect	m_captionRect;
	QRect	m_bodyRect;
	QRect	m_tabsRect;
	QRect	m_titleRect;
	
	QRect	m_progressRect;

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
		
	int m_iTabsHeight;
	int m_iTitleFontHeight;
	int m_eIconState;
	int m_iRctWidth;
	int m_iRctHeight;
	
	bool m_bNeedToRedraw;
	
public:
protected:


// ================================
// Put methods declaration below...
// ================================
private:
	void loadImages();
	void recalculatePositions();

public:
	// reading methods...
	void setWidth(int w);
	void setHeight(int h);
	void resize(int w, int h) { setWidth(w); setHeight(h); };
	void resize(QSize r) { setWidth(r.width()); setHeight(r.height()); };
	void setGeometry(QRect r) { r.topLeft(); r.size(); 	/*debug("x,y: %d,%d", r.x(), r.y()); debug("w,h: %d,%d", r.width(), r.height());*/ };
	void setGeometry(QPoint p, QSize s) { setPoint (p.x(), p.y()); setWidth (s.width()); setHeight (s.height()); };
										
	void setPoint(int x, int y) { m_pnt.setX(x); m_pnt.setY(y); m_rct.setX(x); m_rct.setY(y);  };
	
	void setCloseIcon(int state);
	void resetIcons();

	// writing methods...	
	int x() { return m_pnt.x(); };
	int y() { return m_pnt.y(); };
	int width() { return m_rct.width(); };
	int height() { return m_rct.height(); };
	int baseLine() { return (y()+height()); };

	QRect bodyRect() { return m_bodyRect; };
	QRect tabsRect() { return m_tabsRect; };
	QRect captionRect() { return m_captionRect; };
	QRect rect() { return m_rct; };
	QRect closeRect() { return m_closeIconRect; };
	QRect titleRect() { return m_titleRect; };
	QRect progressRect() { return m_progressRect; };
	
	void centerTitle(int h) { m_iTitleFontHeight = h; };
	
	void touch() { m_bNeedToRedraw = true; };
	
	void draw(QPainter *, bool b = false);
	void setPics(bool b = false);

protected:

};

#endif //!_KVINOTIFIERWINDOWBORDER_H_
