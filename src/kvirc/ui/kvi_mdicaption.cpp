//=================================================================================================
//
//   File : kvi_mdicaption.cpp
//   Creation date : Tue Sep 2 2003 02:35:45 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2003 Szymon Stefanek (pragma at kvirc dot net)
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
//=================================================================================================

#define __KVIRC__

#define _KVI_DEBUG_CHECK_RANGE_
#include "kvi_debug.h"
#include "kvi_mdichild.h"
#include "kvi_mdimanager.h"
#include "kvi_string.h"
#include "kvi_locale.h"
#include "kvi_options.h"
#include "kvi_settings.h"
#include "kvi_iconmanager.h"
#include "kvi_window.h"
#include "kvi_mdicaption.h"
#include "kvi_pointerlist.h"
#include "kvi_tal_popupmenu.h"

#include <QCursor>
#include <QApplication>
#include <QFontMetrics>
#include <QPixmap>
#include <QStyle>
#include <QPainter>
//#include <QDrawUtil>
#include <QEvent>
#include <QPaintEvent>
#include <QStyleOption>
#include <QTextDocument>

#include <q3simplerichtext.h>
#define QSimpleRichText Q3SimpleRichText



KviMdiCaptionButton::KviMdiCaptionButton(const QPixmap &pix,QWidget * parent,const char * name)
: QToolButton(parent)
{
	setObjectName(name);
	setIcon(QIcon(pix));
	//setAutoRaise(true);
	setMinimumSize(18,18);
}

KviMdiCaptionButton::~KviMdiCaptionButton()
{
}

void KviMdiCaptionButton::paintEvent(QPaintEvent *e)
{
	QPainter painter(this);
	drawButton(&painter);
}

void KviMdiCaptionButton::drawButton(QPainter *p)
{
	QBrush b(parentWidget()->palette().window());
	
	if(isDown())
		qDrawShadePanel(p,0,0,width(),height(),palette(),true,1,&b);
	else
		p->fillRect(0,0,width(),height(),b);

	int x = (width() - 16) / 2;
	int y = (width() - 16) / 2;
	p->drawPixmap(x,y,16,16,icon().pixmap(16,16),0,0,16,16);
}


KviMdiCaption::KviMdiCaption(KviMdiChild * parent,const char * name)
: QWidget(parent)
{
	setObjectName(name);
	m_pMaximizeButton    = new KviMdiCaptionButton(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MAXIMIZE)),this,"maximize_button");
	connect(m_pMaximizeButton,SIGNAL(clicked()),parent,SLOT(maximize()));
	m_pMinimizeButton    = new KviMdiCaptionButton(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MINIMIZE)),this,"minimize_button");
	connect(m_pMinimizeButton,SIGNAL(clicked()),parent,SLOT(minimize()));
	m_pCloseButton       = new KviMdiCaptionButton(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CLOSE)),this,"close_button");
	connect(m_pCloseButton,SIGNAL(clicked()),parent,SLOT(closeRequest()));
	m_pSystemButton        = new KviMdiCaptionButton(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DEFAULTICON)),this,"icon_button");
	connect(m_pSystemButton,SIGNAL(clicked()),parent,SLOT(systemPopupSlot()));

	m_lastMousePos = QPoint(-1,-1);
	m_bMouseGrabbed = true;
	m_bActive = false;
	calcLineSpacing();
	setAutoFillBackground(false);
}

KviMdiCaption::~KviMdiCaption()
{
}

void KviMdiCaption::reloadImages()
{
	m_pMaximizeButton->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MAXIMIZE)));
	m_pMinimizeButton->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MINIMIZE)));
	m_pCloseButton->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CLOSE)));
}

void KviMdiCaption::calcLineSpacing()
{
	QFontMetrics fm(font());
	m_iLineSpacing = fm.lineSpacing() + 3;
	if(m_iLineSpacing < 20)m_iLineSpacing = 20;
}

int KviMdiCaption::heightHint()
{
	return m_iLineSpacing;
}

void KviMdiCaption::setActive(bool bActive)
{
	m_bActive = bActive;

	QPalette pal(Qt::white,m_bActive ? KVI_OPTION_COLOR(KviOption_colorMdiCaptionActive) : KVI_OPTION_COLOR(KviOption_colorMdiCaptionInactive));
	setPalette(pal);
	//update();
	m_pSystemButton->update();
	m_pCloseButton->update();
	m_pMinimizeButton->update();
	m_pMaximizeButton->update();
}

void KviMdiCaption::fontChange(const QFont &old)
{
	calcLineSpacing();
	QWidget::fontChange(old);
	((KviMdiChild *)parent())->resizeEvent(0);
}

void KviMdiCaption::mousePressEvent(QMouseEvent *e)
{
	m_bMouseGrabbed = true;
	m_lastMousePos = QCursor::pos();
	setCursor(Qt::SizeAllCursor);
	((KviMdiChild *)parent())->activate(true);
}

void KviMdiCaption::mouseMoveEvent(QMouseEvent *)
{
	if(m_bMouseGrabbed)
	{
		QPoint p = QCursor::pos();
		int dx = m_lastMousePos.x() - p.x();
		int dy = m_lastMousePos.y() - p.y();
		KviMdiChild * c = (KviMdiChild *)parent();

		int nx = c->manager()->childX(c) - dx;
		int ny = c->manager()->childY(c) - dy;

		if((nx < 0) || (ny < 0))
		{
			int cx = p.x();
			int cy = p.y();
			if(nx < 0)
			{
				cx -= nx;
				nx = 0;
			}
			if(ny < 0)
			{
				cy -= ny;
				ny = 0;
			}

			QCursor::setPos(cx,cy);
			m_lastMousePos = QPoint(cx,cy);
		} else {
			m_lastMousePos = p;
		}

		c->manager()->moveChild(c,nx,ny);
		c->manager()->childMoved(c);
	}
}

void KviMdiCaption::mouseDoubleClickEvent(QMouseEvent *e)
{
	((KviMdiChild *)parent())->maximize();
}

void KviMdiCaption::paintEvent(QPaintEvent * e)
{
	QRect r = e->rect();
	QPainter p(this);
	p.fillRect(r,m_bActive ? KVI_OPTION_COLOR(KviOption_colorMdiCaptionActive) : KVI_OPTION_COLOR(KviOption_colorMdiCaptionInactive));
	//FIXME
	//QTextDocument rt(m_bActive ? ((KviMdiChild *)parent())->xmlActiveCaption() : ((KviMdiChild *)parent())->xmlInactiveCaption());
	//rt.setDefaultFont(font());
	//rt.drawContents(&p,rect());

	 QSimpleRichText rt(m_bActive ? ((KviMdiChild *)parent())->xmlActiveCaption() : ((KviMdiChild *)parent())->xmlInactiveCaption(),font()); 
    rt.draw(&p,height() + 2,-1,rect(),colorGroup()); 
}

void KviMdiCaption::mouseReleaseEvent(QMouseEvent *)
{
	m_bMouseGrabbed = false;
	setCursor(Qt::ArrowCursor);
//	releaseMouse();
}

void KviMdiCaption::setFocusProxy(QWidget * w)
{
	QWidget::setFocusProxy(w);
	m_pSystemButton->setFocusProxy(w);
	m_pMinimizeButton->setFocusProxy(w);
	m_pMaximizeButton->setFocusProxy(w);
	m_pCloseButton->setFocusProxy(w);
}

void KviMdiCaption::resizeEvent(QResizeEvent * e)
{
	int s = height() - 2;
	m_pSystemButton->setGeometry(1,1,s,s);
	m_pCloseButton->setGeometry(width() - (s + 1),
		1,s,s);
	m_pMaximizeButton->setGeometry(width() - ((s << 1) + 2),
		1,s,s);
	m_pMinimizeButton->setGeometry(width() - ((s * 3) + 3),
		1,s,s);
}



KviMenuBarToolButton::KviMenuBarToolButton(QWidget * par,const QPixmap &img, const char * name)
: KviStyledToolButton(par)
{
	setProperty("name","name");
	setIcon(img);
	setAutoRaise(true);
}

KviMenuBarToolButton::~KviMenuBarToolButton()
{
}

QSize KviMenuBarToolButton::sizeHint() const
{
	return QSize(20,20);
}
