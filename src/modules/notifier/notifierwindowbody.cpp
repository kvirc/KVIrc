//=============================================================================
//
//   File : kvinotifierwindowbody.cpp
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

#include "notifierwindowbody.h"
#include "kvi_iconmanager.h"

#include <qpainter.h>
#include <qbrush.h>
#include <qcolor.h>

KviNotifierWindowBody::KviNotifierWindowBody(QRect r)
{
	loadImages();
	setHeight(r.height());
	setWidth(r.width());
	m_pnt.setX(r.x());
	m_pnt.setY(r.y());
	recalculatePositions();
}

KviNotifierWindowBody::~KviNotifierWindowBody()
{
}


void KviNotifierWindowBody::loadImages()
{
	QPixmap * p;

	if((p = g_pIconManager->getPixmap("notifier_up.png")))
		m_pixIconPrev = *p;
	
	if((p = g_pIconManager->getPixmap("notifier_down.png")))
		m_pixIconNext = *p;

	if((p = g_pIconManager->getPixmap("notifier_pen.png")))
		m_pixIconWrite = *p;
	
	needToRedraw();

}

void KviNotifierWindowBody::setWidth(int w)
{
	m_rct.setWidth(w);
	recalculatePositions();
}

void KviNotifierWindowBody::setHeight(int h)
{
	m_rct.setHeight(h);
	recalculatePositions();
}

void KviNotifierWindowBody::recalculatePositions()
{
	m_textRect.setX(m_pnt.x());
	m_textRect.setY(m_pnt.y());
	m_textRect.setWidth(m_rct.width()-m_pixIconPrev.width());
	m_textRect.setHeight(m_rct.height());

	m_rctWriteIcon.setX(m_textRect.x()+m_textRect.width());
	m_rctWriteIcon.setY(m_pnt.y()+m_textRect.height()-m_pixIconWrite.height());
	m_rctWriteIcon.setWidth(m_pixIconWrite.width());
	m_rctWriteIcon.setHeight(m_pixIconWrite.height());	

	m_rctPrevIcon.setX(m_textRect.x()+m_textRect.width());
	m_rctPrevIcon.setY(m_pnt.y()+2); //FIXME: Maybe the spacing should be calculated?
	m_rctPrevIcon.setWidth(m_pixIconPrev.width());
	m_rctPrevIcon.setHeight(m_pixIconPrev.height());
	
	m_rctNextIcon.setX(m_textRect.x()+m_textRect.width());
	m_rctNextIcon.setY(m_pnt.y()+m_textRect.height()-m_pixIconNext.height()-m_pixIconWrite.height());
	m_rctNextIcon.setWidth(m_pixIconNext.width());
	m_rctNextIcon.setHeight(m_pixIconNext.height());
	
	needToRedraw();
}

void KviNotifierWindowBody::setPrevIcon(int state)
{

};

void KviNotifierWindowBody::setNextIcon(int state)	{

};
									
void KviNotifierWindowBody::setWriteIcon(int state)	{

};

void KviNotifierWindowBody::draw(QPainter * p) {
	
	if (m_bNeedToRedraw) {

		// Draw Icons
		p->drawPixmap(m_rctPrevIcon.x(), m_rctPrevIcon.y(), m_pixIconPrev);
		p->drawPixmap(m_rctNextIcon.x(), m_rctNextIcon.y(), m_pixIconNext);
		p->drawPixmap(m_rctWriteIcon.x(), m_rctWriteIcon.y(), m_pixIconWrite);

		m_bNeedToRedraw = false;
		
	}
}

void KviNotifierWindowBody::mouseMoveEvent(QMouseEvent * e) {

}

void KviNotifierWindowBody::mousePressEvent(QMouseEvent * e) {

}

void KviNotifierWindowBody::mouseReleaseEvent(QMouseEvent * e) {

}

KviNotifierWindowProgressBar::KviNotifierWindowProgressBar(QRect r)
:m_rect(r)
{

}

KviNotifierWindowProgressBar::~KviNotifierWindowProgressBar()
{
	m_dProgress=0;
}

void KviNotifierWindowProgressBar::draw(QPainter *p)
{
	p->fillRect(m_rect,QColor(236,233,216));
	p->fillRect(
		QRect(
			m_rect.x()+1,
			m_rect.y()+1+m_rect.height()*(1-m_dProgress),
			m_rect.width()-2,
			(m_rect.height()-2)*m_dProgress
			),
		QColor(175,28,95)
		);
	p->drawRect(m_rect);
}