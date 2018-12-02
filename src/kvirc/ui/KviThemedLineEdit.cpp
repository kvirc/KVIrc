//=============================================================================
//
//   File : KviThemedLineEdit.cpp
//   Creation date : Sun Jan 10 2010 12:17:00 by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Fabio Bas (ctrlaltca at gmail dot com)
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

#include "KviThemedLineEdit.h"
#include "KviOptions.h"
#include "kvi_settings.h"
#include "KviApplication.h"
#include "KviMainWindow.h"
#include "KviWindow.h"
#include "kvi_out.h"
#include "KviWindowStack.h"

#include <QPainter>
#include <QStyleOptionFrame>
#include <QPaintEvent>

#include <qdrawutil.h> // qDrawShadePanel

#ifdef COMPILE_PSEUDO_TRANSPARENCY
extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

KviThemedLineEdit::KviThemedLineEdit(QWidget * par, KviWindow * pWindow, const char * name)
    : QLineEdit(par)
{
	setObjectName(name);
	m_pKviWindow = pWindow;

	setFrame(false);

	int l, t, r, b;
	getTextMargins(&l, &t, &r, &b);
	if(l < 4)
		l = 4;
	if(r < 4)
		r = 4;
	setTextMargins(l, t, r, b);

	setAutoFillBackground(false);
	applyOptions();
}

KviThemedLineEdit::~KviThemedLineEdit()
    = default;

void KviThemedLineEdit::applyOptions()
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	bool bIsTrasparent = (KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing()) || g_pShadedChildGlobalDesktopBackground;
#else
	bool bIsTrasparent = false;
#endif
	QString szStyle = QString("QLineEdit { background: %1; color: %2; font-family: %3; font-size: %4pt; font-weight: %5; font-style: %6; margin: 1px; }")
	                      .arg(bIsTrasparent ? "transparent" : KVI_OPTION_COLOR(KviOption_colorLabelBackground).name())
	                      .arg(bIsTrasparent ? getMircColor(KVI_OPTION_MSGTYPE(KVI_OUT_NONE).fore()).name() : KVI_OPTION_COLOR(KviOption_colorLabelForeground).name())
	                      .arg(KVI_OPTION_FONT(KviOption_fontLabel).family())
	                      .arg(KVI_OPTION_FONT(KviOption_fontLabel).pointSize())
	                      .arg(KVI_OPTION_FONT(KviOption_fontLabel).weight() == QFont::Bold ? "bold" : "normal")
	                      .arg(KVI_OPTION_FONT(KviOption_fontLabel).style() == QFont::StyleItalic ? "italic" : "normal");
	setStyleSheet(szStyle);
	update();
}

void KviThemedLineEdit::paintEvent(QPaintEvent * event)
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	QPainter * p = new QPainter(this);
	QPalette pal = palette();

	// In Qt5 QStyle::drawPrimitive seems to always overwrite the background, no matter what.
	qDrawShadePanel(p, 0, 0, width(), height(), palette(), true, 1, nullptr);

	QRect r(1, 1, width() - 2, height() - 2);

	if(KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing())
	{
		p->setCompositionMode(QPainter::CompositionMode_Source);
		QColor col = KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade);
		col.setAlphaF((float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) / (float)100));
		p->fillRect(r, col);
		p->restore();
	}
	else if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt;
		if(m_pKviWindow)
			pnt = m_pKviWindow->isDocked() ? mapTo(g_pMainWindow, r.topLeft()) : mapTo(m_pKviWindow, r.topLeft());
		else
			pnt = mapToGlobal(event->rect().topLeft());
		p->drawTiledPixmap(r, *(g_pShadedChildGlobalDesktopBackground), pnt);
	}
	delete p;
#endif

	QLineEdit::paintEvent(event);
}
