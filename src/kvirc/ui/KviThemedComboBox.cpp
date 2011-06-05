//=============================================================================
//
//   File : KviThemedLineEdit.cpp
//   Creation date : Sun Jan 10 2010 12:17:00 by Fabio Bas
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2010 Fabio Bas (ctrlaltca at gmail dot com)
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

#include "KviThemedComboBox.h"
#include "KviOptions.h"
#include "kvi_settings.h"
#include "KviApplication.h"
#include "KviMainWindow.h"
#include "KviWindow.h"
#include "kvi_out.h"
#include "KviMdiManager.h"

#include <QPainter>
#include <QLineEdit>
#include <QStyleOptionFrameV2>

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

KviThemedComboBox::KviThemedComboBox(QWidget * par, KviWindow * pWindow, const char * name)
: QComboBox(par)
{
	setObjectName(name);
	m_pKviWindow = pWindow;
	setAutoFillBackground(true);
	applyOptions();
}

KviThemedComboBox::~KviThemedComboBox()
{
}

void KviThemedComboBox::applyOptions()
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	bool bIsTrasparent = (KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing()) || g_pShadedChildGlobalDesktopBackground;
#else
	bool bIsTrasparent = false;
#endif

	if(style()->objectName() == "oxygen" || style()->objectName().startsWith("ia-ora-"))
	{
		// workaround for broken oxygen in kde4.4: use palette() instead that stylesheet
		// ia-ora- are the mandriva default styles
		setFont(KVI_OPTION_FONT(KviOption_fontLabel));
		QPalette pal = palette();
		pal.setBrush(QPalette::Base, bIsTrasparent ? Qt::transparent : KVI_OPTION_COLOR(KviOption_colorLabelBackground));
		//qcombobox forces QPalette::Text as its forecolor
		pal.setBrush(QPalette::Text, bIsTrasparent ? KVI_OPTION_MIRCCOLOR(KVI_OPTION_MSGTYPE(KVI_OUT_NONE).fore()) : KVI_OPTION_COLOR(KviOption_colorLabelForeground));
		setPalette(pal);
	} else {
		QString szStyle = QString("QComboBox { background: %1; color: %2; font-family: %3; font-size: %4pt; font-weight: %5; font-style: %6;}")
			.arg(bIsTrasparent ? "transparent" : KVI_OPTION_COLOR(KviOption_colorLabelBackground).name())
			.arg(bIsTrasparent ? KVI_OPTION_MIRCCOLOR(KVI_OPTION_MSGTYPE(KVI_OUT_NONE).fore()).name() :
				KVI_OPTION_COLOR(KviOption_colorLabelForeground).name())
		.arg(KVI_OPTION_FONT(KviOption_fontLabel).family())
		.arg(KVI_OPTION_FONT(KviOption_fontLabel).pointSize())
		.arg(KVI_OPTION_FONT(KviOption_fontLabel).weight() == QFont::Bold ? "bold" : "normal")
		.arg(KVI_OPTION_FONT(KviOption_fontLabel).style() == QFont::StyleItalic ? "italic" : "normal");
		setStyleSheet(szStyle);
	}
	update();
}

void KviThemedComboBox::paintEvent ( QPaintEvent * event )
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	QPainter * p = new QPainter(this);
	QLineEdit *le = lineEdit();
	if(le)
	{
		QRect r = rect();
		QPalette pal = palette();
		QStyleOptionFrameV2 option;

		option.initFrom(this);
		option.rect = contentsRect();
		option.lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &option, le);
		option.midLineWidth = 0;
		option.state |= QStyle::State_Sunken;
		if(le->isReadOnly())
			option.state |= QStyle::State_ReadOnly;
		option.features = QStyleOptionFrameV2::None;

		r = style()->subElementRect(QStyle::SE_LineEditContents, &option, le);
		int left, right, top, bottom;
		le->getTextMargins(&left, &top, &right, &bottom);
		r.setX(r.x() + left);
		r.setY(r.y() + top);
		r.setRight(r.right() - right);
		r.setBottom(r.bottom() - bottom);
		p->setClipRect(r);
	} // else not editable

	if(KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing())
	{
		p->setCompositionMode(QPainter::CompositionMode_Source);
		QColor col=KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade);
		col.setAlphaF((float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) / (float)100));
		p->fillRect(contentsRect(), col);
	} else if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = m_pKviWindow->mdiParent() ? mapTo(g_pMainWindow, contentsRect().topLeft() + g_pMainWindow->mdiManager()->scrollBarsOffset()) : mapTo(m_pKviWindow, contentsRect().topLeft());
		p->drawTiledPixmap(contentsRect(),*(g_pShadedChildGlobalDesktopBackground), pnt);
	}
	delete p;
#endif
	QComboBox::paintEvent(event);
}

void KviThemedComboBox::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Return)
		emit returnPressed(this->currentText());

	QComboBox::keyPressEvent(e);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "KviThemedComboBox.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
