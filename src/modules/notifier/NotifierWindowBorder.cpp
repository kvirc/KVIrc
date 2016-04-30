//=============================================================================
//
//   File : NotifierWindowBorder.cpp
//   Creation date : Mon 03 Jan 2005 02:27:22 by Iacopo Palazzi
//
//   This file is part of the KVIrc distribution
//   Copyright (C) 2005-2008 Iacopo Palazzi < iakko(at)siena(dot)linux(dot)it >
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
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

#include "NotifierWindowBorder.h"
#include "NotifierWindowTab.h"

#include "KviIconManager.h"

#include <QPainter>

NotifierWindowBorder::NotifierWindowBorder(QSize s)
{
	loadImages();
	resize(s);
}

NotifierWindowBorder::~NotifierWindowBorder()
    = default;

void NotifierWindowBorder::loadImages()
{
	// Normal Border

	QPixmap * p;

	if((p = g_pIconManager->getPixmap("notifier_pix_wnd_sx.png")))
		m_pixSX_N = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_wnd_dx.png")))
		m_pixDX_N = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_wnd_dwn.png")))
		m_pixDWN_N = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_wnd_dwndx.png")))
		m_pixDWNDX_N = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_wnd_dwnsx.png")))
		m_pixDWNSX_N = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_caption_sx.png")))
		m_pixCaptionSX_N = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_caption_dx.png")))
		m_pixCaptionDX_N = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_caption_bkg.png")))
		m_pixCaptionBKG_N = *p;

	if((p = g_pIconManager->getPixmap("notifier_icon_close_out.png")))
		m_pixIconClose_out_N = *p;
	if((p = g_pIconManager->getPixmap("notifier_icon_close_over.png")))
		m_pixIconClose_over_N = *p;
	if((p = g_pIconManager->getPixmap("notifier_icon_close_clicked.png")))
		m_pixIconClose_clicked_N = *p;

	// Highlighted Border

	if((p = g_pIconManager->getPixmap("notifier_pix_wnd_sx_hl.png")))
		m_pixSX_HL = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_wnd_dx_hl.png")))
		m_pixDX_HL = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_wnd_dwn_hl.png")))
		m_pixDWN_HL = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_wnd_dwndx_hl.png")))
		m_pixDWNDX_HL = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_wnd_dwnsx_hl.png")))
		m_pixDWNSX_HL = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_caption_sx_hl.png")))
		m_pixCaptionSX_HL = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_caption_dx_hl.png")))
		m_pixCaptionDX_HL = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_caption_bkg_hl.png")))
		m_pixCaptionBKG_HL = *p;

	if((p = g_pIconManager->getPixmap("notifier_icon_close_out_hl.png")))
		m_pixIconClose_out_HL = *p;
	if((p = g_pIconManager->getPixmap("notifier_icon_close_over_hl.png")))
		m_pixIconClose_over_HL = *p;
	if((p = g_pIconManager->getPixmap("notifier_icon_close_clicked_hl.png")))
		m_pixIconClose_clicked_HL = *p;

	setPics();
	setCloseIcon(WDG_ICON_OUT);

	if(m_pixCaptionDX->height() == m_pixCaptionSX->height() && m_pixCaptionDX->height() == m_pixCaptionBKG->height()) // just to be sure that the height is fitting
		m_captionRect.setHeight(m_pixCaptionDX->height());
}

void NotifierWindowBorder::setPics(bool b)
{
	if(b)
	{
		m_pixSX = &m_pixSX_HL;
		m_pixDX = &m_pixDX_HL;
		m_pixDWN = &m_pixDWN_HL;
		m_pixDWNSX = &m_pixDWNSX_HL;
		m_pixDWNDX = &m_pixDWNDX_HL;
		m_pixCaptionSX = &m_pixCaptionSX_HL;
		m_pixCaptionDX = &m_pixCaptionDX_HL;
		m_pixCaptionBKG = &m_pixCaptionBKG_HL;
		m_pixIconClose_out = &m_pixIconClose_out_HL;
		m_pixIconClose_over = &m_pixIconClose_over_HL;
		m_pixIconClose_clicked = &m_pixIconClose_clicked_HL;
	}
	else
	{
		m_pixSX = &m_pixSX_N;
		m_pixDX = &m_pixDX_N;
		m_pixDWN = &m_pixDWN_N;
		m_pixDWNSX = &m_pixDWNSX_N;
		m_pixDWNDX = &m_pixDWNDX_N;
		m_pixCaptionSX = &m_pixCaptionSX_N;
		m_pixCaptionDX = &m_pixCaptionDX_N;
		m_pixCaptionBKG = &m_pixCaptionBKG_N;
		m_pixIconClose_out = &m_pixIconClose_out_N;
		m_pixIconClose_over = &m_pixIconClose_over_N;
		m_pixIconClose_clicked = &m_pixIconClose_clicked_N;
	}
}

void NotifierWindowBorder::setWidth(int w)
{
	m_iRctWidth = w;
	recalculatePositions();
}

void NotifierWindowBorder::setHeight(int h)
{
	m_iRctHeight = h;
	recalculatePositions();
}

void NotifierWindowBorder::recalculatePositions()
{
	m_rct.setHeight(m_iRctHeight);
	m_rct.setWidth(m_iRctWidth);
	m_rct.setTopLeft(QPoint(0, 0));

	m_closeIconRect.setX(m_rct.width() - m_pixCaptionDX->width() - m_pixIconClose->width());
	m_closeIconRect.setY(2);
	m_closeIconRect.setWidth(m_pixIconClose->width());
	m_closeIconRect.setHeight(m_pixIconClose->height());

	m_captionRect.setTopLeft(m_rct.topLeft());
	m_captionRect.setSize(QSize(m_rct.width(), m_pixCaptionDX->height()));

	m_bodyRect.setTopLeft(QPoint(m_pixCaptionDX->width(), m_captionRect.height()));
	m_bodyRect.setSize(QSize(m_captionRect.width() - (2 * m_pixCaptionDX->width()), m_rct.height() - m_captionRect.height() - m_pixDWN->height()));

	m_titleRect.setTopLeft(QPoint(m_pixCaptionSX->width(), 0));
	m_titleRect.setSize(QSize(m_rct.width() - m_pixCaptionSX->width() - m_pixCaptionDX->width() - m_pixIconClose->width(), m_captionRect.height()));
}

void NotifierWindowBorder::setCloseIcon(int state)
{
	m_eIconState = state;
	switch(m_eIconState)
	{
		case WDG_ICON_OUT:
			m_pixIconClose = m_pixIconClose_out;
			break;
		case WDG_ICON_OVER:
			m_pixIconClose = m_pixIconClose_over;
			break;
		case WDG_ICON_CLICKED:
			m_pixIconClose = m_pixIconClose_clicked;
			break;
	};
}

void NotifierWindowBorder::resetIcons()
{
	setCloseIcon(WDG_ICON_OUT);
}

void NotifierWindowBorder::draw(QPainter * p, bool b)
{
	setPics(b);
	setCloseIcon(m_eIconState);

	// Draw the caption
	p->drawPixmap(m_captionRect.x(), m_captionRect.y(), *m_pixCaptionSX);
	p->drawTiledPixmap(m_pixCaptionSX->width(), 0, m_captionRect.width() - (m_pixCaptionSX->width() + m_pixCaptionDX->width()), m_captionRect.height(), *m_pixCaptionBKG);
	p->drawPixmap(m_captionRect.width() - m_pixCaptionDX->width(), 0, *m_pixCaptionDX);

	// Draw the tiled borders
	p->drawTiledPixmap(0, m_captionRect.height(), m_pixSX->width(), m_bodyRect.height(), *m_pixSX);
	p->drawTiledPixmap(m_bodyRect.width() + m_bodyRect.x(), m_captionRect.height(), m_pixDX->width(), m_bodyRect.height(), *m_pixDX);
	p->drawTiledPixmap(m_pixDWNSX->width(), m_captionRect.height() + m_bodyRect.height(), m_bodyRect.width() + 1, m_pixDWN->height(), *m_pixDWN);

	// Draw down corners
	p->drawPixmap(0, m_captionRect.height() + m_bodyRect.height(), *m_pixDWNSX);
	p->drawPixmap(m_bodyRect.width() + m_bodyRect.x(), m_captionRect.height() + m_bodyRect.height(), *m_pixDWNDX);

	//Drawing icons
	p->drawPixmap(m_closeIconRect.x(), m_closeIconRect.y(), *m_pixIconClose);
}
