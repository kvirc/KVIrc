//=============================================================================
//
//   File : kvinotifierwindowborder.cpp
//   Created on lun 03 gen 2005 02:27:22 by Iacopo Palazzi
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

#include "notifierwindowborder.h"
#include "notifierwindowtabs.h"
#include "kvi_iconmanager.h"

#include <qpainter.h>

KviNotifierWindowBorder::KviNotifierWindowBorder(QSize s)
{
	loadImages();
	resize(s);
}

KviNotifierWindowBorder::~KviNotifierWindowBorder()
{
}

void KviNotifierWindowBorder::loadImages() {

	
	
}

void KviNotifierWindowBorder::setPics(bool b)
{
	
}

void KviNotifierWindowBorder::setWidth(int w) {
	m_iRctWidth = w;
	recalculatePositions();
}

void KviNotifierWindowBorder::setHeight(int h) {
	m_iRctHeight = h;
	recalculatePositions();
}

void KviNotifierWindowBorder::recalculatePositions()
{
	m_rct.setHeight(m_iRctHeight);
	m_rct.setWidth(m_iRctWidth);
	m_rct.setTopLeft(QPoint(0,0));

	m_tabsRect.setTopLeft( QPoint(9,9) );
	m_tabsRect.setSize( QSize(m_rct.width()-9*2,14) );
	
	m_bodyRect.setTopLeft( QPoint(9,9+14) );
	m_bodyRect.setSize( QSize(m_rct.width()-9*2,m_rct.height()-2*9-14) );

	m_bNeedToRedraw = true;
}

void KviNotifierWindowBorder::setCloseIcon(int state) { 

}

void KviNotifierWindowBorder::resetIcons() {
	setCloseIcon(WDG_ICON_OUT);
}

void KviNotifierWindowBorder::draw(QPainter * p, bool b) {

	if (m_bNeedToRedraw) {
	
		m_bNeedToRedraw = false;
	}
}
