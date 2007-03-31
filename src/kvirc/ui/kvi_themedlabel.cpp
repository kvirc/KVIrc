//
//   File : kvi_themedlabel.cpp
//   Creation date : Tue Aug 29 2000 21:17:01 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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
#define __KVIRC__
#include "kvi_themedlabel.h"

#include "kvi_options.h"
#include "kvi_settings.h"
#include "kvi_app.h"
#include "kvi_window.h"
#include <qpainter.h>

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

KviThemedLabel::KviThemedLabel(QWidget * par,const char * name)
: QFrame(par,name)
{
	setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
	applyOptions();
	m_bAutoHeight=0;
}

KviThemedLabel::~KviThemedLabel()
{
}

/*QSize KviThemedLabel::sizeHint()
{
	QSize size=QFrame::sizeHint();
	int iHeight=fontMetrics().height()*QStringList::split('\n',m_szText).count()+4;
	size.setHeight(iHeight);
	return size;
}*/

void KviThemedLabel::setText(const char * text)
{
	m_szText = QString(text);
	if(m_bAutoHeight)
	{
		int iHeight=fontMetrics().height()*QStringList::split('\n',m_szText).count()+4;
		setMinimumHeight(iHeight);
		//g_pApp->postEvent(parent(),new QEvent(QEvent::Resize));
	}
	update();
}

void KviThemedLabel::setText(const QString& text)
{
	m_szText = text;
	if(m_bAutoHeight)
	{
		int iHeight=fontMetrics().height()*QStringList::split('\n',m_szText).count()+4;
		setMinimumHeight(iHeight);
 		//g_pApp->postEvent(parent(),new QEvent(QEvent::Resize));
	}
	update();
}

void KviThemedLabel::applyOptions()
{
	setFont(KVI_OPTION_FONT(KviOption_fontLabel));
	update();
}

#ifdef COMPILE_USE_QT4
void KviThemedLabel::paintEvent ( QPaintEvent * event )
{
	QFrame::paintEvent(event);
	QPainter p(this);
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(g_pShadedChildGlobalDesktopBackground)
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
#else
void KviThemedLabel::drawContents(QPainter *p)
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = mapToGlobal(contentsRect().topLeft());
		p->drawTiledPixmap(contentsRect(),*g_pShadedChildGlobalDesktopBackground,pnt);
	} else {
#endif

		if(KVI_OPTION_PIXMAP(KviOption_pixmapLabelBackground).pixmap())
		{
			p->drawTiledPixmap(contentsRect(),*(KVI_OPTION_PIXMAP(KviOption_pixmapLabelBackground).pixmap()));
		} else {
			p->fillRect(contentsRect(),KVI_OPTION_COLOR(KviOption_colorLabelBackground));
		}

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif

	QRect r = contentsRect();
	r.setLeft(r.left() + 2); // some margin

	p->setPen(KVI_OPTION_COLOR(KviOption_colorLabelForeground));
	
	p->drawText(r,Qt::AlignLeft | Qt::AlignVCenter,m_szText);
}
#endif

void KviThemedLabel::mouseDoubleClickEvent(QMouseEvent *)
{
	emit doubleClicked();
}


#include "kvi_themedlabel.moc"
