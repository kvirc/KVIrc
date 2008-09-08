//=============================================================================
//
//   File : notifiermessage.cpp
//   Created on mar 02 nov 2004 02:41:18 by Iacopo
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2004 Iacopo <iakko@siena.linux.it>
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

#include "notifiermessage.h"
#include "notifierwindow.h"

#include "kvi_iconmanager.h"
#include "kvi_config.h"
#include "kvi_app.h"
#include "kvi_window.h"
#include "kvi_locale.h"
#include "kvi_frame.h"
#include "kvi_mirccntrl.h"
#include "kvi_options.h"

/*
#ifdef COMPILE_ON_WINDOWS
	#include <windows.h>
#endif

#ifdef COMPILE_KDE_SUPPORT
	#include <netwm.h>
#endif
*/

#include <QApplication>
#include <QPainter>
#include <QImage>
#include <QDesktopWidget>
#include <QToolTip>
#include "kvi_tal_popupmenu.h"


extern kvi_time_t g_tNotifierDisabledUntil;

KviNotifierMessage::KviNotifierMessage(KviNotifierWindow * pNotifierWindow, QPixmap * pImage, const QString &szText)
{
	m_pText = new QTextDocument();
	m_pText->setHtml(KviMircCntrl::stripControlBytes(szText));
	m_pText->setDefaultFont(pNotifierWindow->defaultFont());
	m_pText->setTextWidth(pNotifierWindow->textWidth());
	m_pImage = pImage;
	m_bHistoric = false;
}

KviNotifierMessage::~KviNotifierMessage()
{
	delete m_pText;
	if(m_pImage)delete m_pImage;
}

static void increase_transparency(QImage &buffer,QImage &srcimg,int iDivisor)
{
	buffer = QImage(srcimg.size(),QImage::Format_ARGB32);

	for(int y = 0;y < buffer.height();y++)
	{
		QRgb * dst = (QRgb *)buffer.scanLine(y);
		QRgb * end = dst + buffer.width();
		QRgb * src = (QRgb *)srcimg.scanLine(y);

		while(dst < end)
		{
			*dst = qRgba(qRed(*src),qGreen(*src),qBlue(*src),qAlpha(*src) / iDivisor);
			dst++;
			src++;
		}
	}
}

void KviNotifierMessage::setHistoric()
{
	m_bHistoric = true;
	if(!m_pImage)return;
	if(!m_pImage->hasAlphaChannel())return;
	QImage tmp;
	QImage out = m_pImage->toImage();
	increase_transparency(tmp,out,2);
	*m_pImage = QPixmap::fromImage(tmp);
}
