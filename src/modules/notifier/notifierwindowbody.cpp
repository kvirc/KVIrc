//=============================================================================
//
//   File : notifierwindowbody.cpp
//   Creation date : Sun 02 Jan 2005 17:53:29 by Iacopo Palazzi
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

#include "notifierwindowbody.h"

#include "kvi_iconmanager.h"

#include <QBrush>
#include <QColor>
#include <QPainter>

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
	if((p = g_pIconManager->getPixmap("notifier_pix_body_dx.png")))
		m_pixDX = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_body_sx.png")))
		m_pixSX = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_body_dwnsx.png")))
		m_pixDWNSX = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_body_dwndx.png")))
		m_pixDWNDX = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_body_dwn.png")))
		m_pixDWN = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_body_kvirc_sx.png")))
		m_pixKVIrcSX = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_body_kvirc_dwn.png")))
		m_pixKVIrcDWN = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_body_kvirc.png")))
		m_pixKVIrc = *p;

	if((p = g_pIconManager->getPixmap("notifier_icon_body_prev_off.png")))
		m_pixIconPrev_off = *p;
	if((p = g_pIconManager->getPixmap("notifier_icon_body_prev_on.png")))
		m_pixIconPrev_on = *p;
	if((p = g_pIconManager->getPixmap("notifier_icon_body_prev_clicked.png")))
		m_pixIconPrev_clicked = *p;

	m_pixIconPrev = m_pixIconPrev_off;

	if((p = g_pIconManager->getPixmap("notifier_icon_body_next_off.png")))
		m_pixIconNext_off = *p;
	if((p = g_pIconManager->getPixmap("notifier_icon_body_next_on.png")))
		m_pixIconNext_on = *p;
	if((p = g_pIconManager->getPixmap("notifier_icon_body_next_clicked.png")))
		m_pixIconNext_clicked = *p;

	m_pixIconNext = m_pixIconNext_off;

	if((p = g_pIconManager->getPixmap("notifier_icon_body_write_off.png")))
		m_pixIconWrite_off = *p;
	if((p = g_pIconManager->getPixmap("notifier_icon_body_write_on.png")))
		m_pixIconWrite_on = *p;
	if((p = g_pIconManager->getPixmap("notifier_icon_body_write_clicked.png")))
		m_pixIconWrite_clicked = *p;

	m_pixIconWrite = m_pixIconWrite_off;

	needToRedraw();

	m_prevIconState = WDG_ICON_OFF;
	m_nextIconState = WDG_ICON_OFF;
	m_writeIconState = WDG_ICON_OFF;

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
	m_textRect.setX(m_pnt.x()+m_pixSX.width());
	m_textRect.setY(m_pnt.y());
	m_textRect.setWidth(m_rct.width()-m_pixSX.width()-m_pixDX.width()-m_pixIconPrev.width());
	m_textRect.setHeight(m_rct.height()-m_pixDWN.height());

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
	//if (m_prevIconState==WDG_ICON_OFF && state!=WDG_ICON_OFF)
		if (m_prevIconState!=state) {
			switch (state) {
				case WDG_ICON_ON: m_pixIconPrev = m_pixIconPrev_on; break;
				case WDG_ICON_OFF: m_pixIconPrev = m_pixIconPrev_off; break;
				case WDG_ICON_CLICKED: m_pixIconPrev = m_pixIconPrev_clicked; break;
			};

			m_prevIconState=state;
			needToRedraw();
		}
}

void KviNotifierWindowBody::setNextIcon(int state)
{
	//if (m_nextIconState==WDG_ICON_OFF && state!=WDG_ICON_OFF)
		if (m_nextIconState!=state) {
			switch (state) {
				case WDG_ICON_ON: m_pixIconNext = m_pixIconNext_on; break;
				case WDG_ICON_OFF: m_pixIconNext = m_pixIconNext_off; break;
				case WDG_ICON_CLICKED: m_pixIconNext = m_pixIconNext_clicked; break;
			};

			needToRedraw();
			m_nextIconState=state;
		}
}

void KviNotifierWindowBody::setWriteIcon(int state)
{
	//if (m_writeIconState==WDG_ICON_OFF && state!=WDG_ICON_OFF)
		if (m_writeIconState!=state) {
			switch (state) {
				case WDG_ICON_ON: m_pixIconWrite = m_pixIconWrite_on; break;
				case WDG_ICON_OFF: m_pixIconWrite = m_pixIconWrite_off; break;
				case WDG_ICON_CLICKED: m_pixIconWrite = m_pixIconWrite_clicked; break;};

			needToRedraw();
			m_writeIconState=state;
		}
}

void KviNotifierWindowBody::draw(QPainter * p)
{
	if (m_bNeedToRedraw) {

		p->fillRect(QRect(m_pnt,m_rct.size()),m_mac_bkgColor);

		// Autotiled borders
		p->drawTiledPixmap(m_pnt.x(), m_pnt.y(), m_pixSX.width(), m_rct.height() - m_pixDWNSX.height() - m_pixKVIrcSX.height(), m_pixSX);
		p->drawTiledPixmap(m_pnt.x() + m_rct.width() - m_pixDX.width(), m_pnt.y(),m_pixDX.width(), m_rct.height() - m_pixDWNDX.height(), m_pixDX);
		p->drawTiledPixmap(m_pnt.x() + m_pixKVIrcDWN.width() + m_pixDWNSX.width(), m_pnt.y() + m_rct.height() - m_pixDWN.height(),  m_rct.width() - m_pixKVIrcDWN.width() - m_pixDWNSX.width() - m_pixDWNDX.width(), m_pixDWN.height(), m_pixDWN);

		// Bottom corners
		p->drawPixmap(m_pnt.x(), m_pnt.y() + m_rct.height() - m_pixDWNSX.height(), m_pixDWNSX);
		p->drawPixmap(m_pnt.x() + m_rct.width() - m_pixDWNSX.width(), m_pnt.y() + m_rct.height() - m_pixDWNDX.height(), m_pixDWNDX);

		// KVIrc image
		p->drawPixmap(m_pnt.x(), m_pnt.y() + m_rct.height() - m_pixKVIrcSX.height() - m_pixDWNSX.height(), m_pixKVIrcSX);
		p->drawPixmap(m_pnt.x() + m_pixKVIrcSX.width(), m_pnt.y() + m_rct.height() - m_pixKVIrcDWN.height(), m_pixKVIrcDWN);
		p->drawPixmap(m_pnt.x() + m_pixKVIrcSX.width(), m_pnt.y() + m_rct.height() - m_pixKVIrc.height() - m_pixKVIrcDWN.height(), m_pixKVIrc, 0, 0, m_pixKVIrc.width(), m_pixKVIrc.height());

		// Draw Icons
		p->drawPixmap(m_rctPrevIcon.x(), m_rctPrevIcon.y(), m_pixIconPrev);
		p->drawPixmap(m_rctNextIcon.x(), m_rctNextIcon.y(), m_pixIconNext);
		p->drawPixmap(m_rctWriteIcon.x(), m_rctWriteIcon.y(), m_pixIconWrite);

		m_bNeedToRedraw = false;
	}
}

void KviNotifierWindowBody::mouseMoveEvent(QMouseEvent *)
{

}

void KviNotifierWindowBody::mousePressEvent(QMouseEvent *)
{

}

void KviNotifierWindowBody::mouseReleaseEvent(QMouseEvent *)
{

}
