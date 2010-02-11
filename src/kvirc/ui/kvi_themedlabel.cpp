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
#include "kvi_frame.h"
#include "kvi_window.h"

#include <QPainter>

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

KviThemedLabel::KviThemedLabel(QWidget * par, KviWindow * pWindow,const char * name)
: QLabel(par)
{
	setObjectName(name);
	m_pKviWindow = pWindow;
	setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
	setContentsMargins(2,2,2,2);
	setAutoFillBackground(false);

	QPalette pal = palette();
	pal.setBrush(QPalette::Active, QPalette::Base, Qt::transparent);
	pal.setBrush(QPalette::Inactive, QPalette::Base, Qt::transparent);
	pal.setBrush(QPalette::Disabled, QPalette::Base, Qt::transparent);
	setPalette(pal);

	applyOptions();
}

KviThemedLabel::~KviThemedLabel()
{
}

void KviThemedLabel::applyOptions()
{
	QPalette pal = palette();
	pal.setBrush(QLabel::foregroundRole(),KVI_OPTION_COLOR(KviOption_colorLabelForeground));
	setPalette(pal);

	setFont(KVI_OPTION_FONT(KviOption_fontLabel));
}

void KviThemedLabel::paintEvent(QPaintEvent *e)
{
	QPainter *p = new QPainter(this);
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing())
	{
		p->setCompositionMode(QPainter::CompositionMode_Source);
		QColor col=KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade);
		col.setAlphaF((float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) / (float)100));
		p->fillRect(contentsRect(), col);
	} else if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = m_pKviWindow->mdiParent() ? mapTo(g_pFrame, contentsRect().topLeft() + g_pFrame->mdiManager()->scrollBarsOffset()) : mapTo(m_pKviWindow, contentsRect().topLeft());
		p->drawTiledPixmap(contentsRect(),*(g_pShadedChildGlobalDesktopBackground), pnt);
	} else {
#endif
		QPixmap * pix = KVI_OPTION_PIXMAP(KviOption_pixmapTreeWindowListBackground).pixmap();
		if(pix)
		{
			KviPixmapUtils::drawPixmapWithPainter(p,pix,KVI_OPTION_UINT(KviOption_uintTreeWindowListPixmapAlign),contentsRect(),contentsRect().width(),contentsRect().height());
		} else {
			p->fillRect(contentsRect(),KVI_OPTION_COLOR(KviOption_colorTreeWindowListBackground));
		}
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif
	delete p;
	QLabel::paintEvent(e);
}

void KviThemedLabel::mouseDoubleClickEvent(QMouseEvent *)
{
	emit doubleClicked();
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_themedlabel.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
