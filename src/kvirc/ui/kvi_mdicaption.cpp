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

#include <qcursor.h>
#include <qnamespace.h>
#include <qapplication.h>
#include <qfontmetrics.h>
#include "kvi_list.h"
#include <qpixmap.h>
#include <qstyle.h>
#include <qpainter.h>
#include "kvi_tal_popupmenu.h"
#ifdef COMPILE_USE_QT4
	#include <q3simplerichtext.h>
	#define QSimpleRichText Q3SimpleRichText
	#include <qstyleoption.h>
#else
	#include <qsimplerichtext.h>
#endif
#include <qdrawutil.h>
#include <qevent.h>

KviMdiCaptionButton::KviMdiCaptionButton(const QPixmap &pix,QWidget * parent,const char * name)
: QToolButton(parent,name)
{
	setPixmap(pix);
	//setAutoRaise(true);
	setMinimumSize(18,18);
}

KviMdiCaptionButton::~KviMdiCaptionButton()
{
}


#ifdef COMPILE_USE_QT4
void KviMdiCaptionButton::paintEvent(QPaintEvent *e)
{
	QPainter painter(this);
	drawButton(&painter);
}
#endif

void KviMdiCaptionButton::drawButton(QPainter *p)
{
	QBrush b(parentWidget()->colorGroup().background());
	
	if(isDown())
		qDrawShadePanel(p,0,0,width(),height(),colorGroup(),true,1,&b);
	else
		p->fillRect(0,0,width(),height(),b);

#ifdef COMPILE_USE_QT4
	QStyleOptionButton opt;
	opt.initFrom(this);
	style()->drawControl(QStyle::CE_PushButtonLabel,&opt,p,this);
#else
	drawButtonLabel(p);
#endif
}







KviMdiCaption::KviMdiCaption(KviMdiChild * parent,const char * name)
: QWidget(parent,name)
{
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
}

KviMdiCaption::~KviMdiCaption()
{
}

void KviMdiCaption::reloadImages()
{
	m_pMaximizeButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MAXIMIZE)));
	m_pMinimizeButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MINIMIZE)));
	m_pCloseButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CLOSE)));
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
	debug("mouse press");
	m_bMouseGrabbed = true;
	m_lastMousePos = QCursor::pos();
#ifdef COMPILE_USE_QT4
	setCursor(Qt::SizeAllCursor);
#else
	setCursor(QCursor::sizeAllCursor);
#endif
	//grabMouse(Qt::sizeAllCursor);
	//e->ignore();
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
	QSimpleRichText rt(m_bActive ? ((KviMdiChild *)parent())->xmlActiveCaption() : ((KviMdiChild *)parent())->xmlInactiveCaption(),font());
	rt.draw(&p,height() + 2,-1,rect(),colorGroup());
}

void KviMdiCaption::mouseReleaseEvent(QMouseEvent *)
{
	m_bMouseGrabbed = false;
#ifdef COMPILE_USE_QT4
	setCursor(Qt::arrowCursor);
#else
	setCursor(QCursor::arrowCursor);
#endif
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
	setIconSet(img);
	setAutoRaise(true);
}

KviMenuBarToolButton::~KviMenuBarToolButton()
{
}

QSize KviMenuBarToolButton::sizeHint() const
{
	return QSize(20,20);
}

