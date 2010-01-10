//=============================================================================
//
//   File : kvi_themedlabel.cpp
//   Creation date : Tue Aug 29 2000 21:17:01 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_themedlabel.h"
#include "kvi_options.h"
#include "kvi_settings.h"
#include "kvi_app.h"
#include "kvi_window.h"

#include <QPainter>

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

KviThemedLabel::KviThemedLabel(QWidget * par,const char * name)
: QFrame(par)
{
	setObjectName(name);
	setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
	applyOptions();
	m_bAutoHeight=0;
}

KviThemedLabel::~KviThemedLabel()
{
}

void KviThemedLabel::setText(const char * text)
{
	m_szText = QString(text);
	if(m_bAutoHeight)
	{
		int iHeight=fontMetrics().height()*m_szText.split('\n',QString::SkipEmptyParts).count()+4;
		setMinimumHeight(iHeight);
	}
	update();
}

void KviThemedLabel::setText(const QString& text)
{
	m_szText = text;
	if(m_bAutoHeight)
	{
		int iHeight=fontMetrics().height()*m_szText.split('\n',QString::SkipEmptyParts).count()+4;
		setMinimumHeight(iHeight);
	}
	update();
}

void KviThemedLabel::applyOptions()
{
	setFont(KVI_OPTION_FONT(KviOption_fontLabel));
	update();
}

void KviThemedLabel::paintEvent(QPaintEvent *)
{
	QPainter p(this);
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing())
	{
		p.save();
		p.setCompositionMode(QPainter::CompositionMode_Source);
		QColor col=KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade);
		col.setAlphaF((float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) / (float)100));
		p.fillRect(contentsRect(), col);
		p.restore();
	} else if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = mapToGlobal(contentsRect().topLeft());
		p.drawTiledPixmap(contentsRect(),*g_pShadedChildGlobalDesktopBackground,pnt);
	} else {
#endif

		if(KVI_OPTION_PIXMAP(KviOption_pixmapLabelBackground).pixmap())
		{
			p.drawTiledPixmap(contentsRect(),*(KVI_OPTION_PIXMAP(KviOption_pixmapLabelBackground).pixmap()));
		} else {
			p.fillRect(contentsRect(),KVI_OPTION_COLOR(KviOption_colorLabelBackground));
		}

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif

	QRect r = contentsRect();
	r.setLeft(r.left() + 2); // some margin

	p.setPen(KVI_OPTION_COLOR(KviOption_colorLabelForeground));

	p.drawText(r,Qt::AlignLeft | Qt::AlignVCenter,m_szText);
}

void KviThemedLabel::mouseDoubleClickEvent(QMouseEvent *)
{
	emit doubleClicked();
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_themedlabel.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
