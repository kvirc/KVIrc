//=============================================================================
//
//   File : notifierwindowtab.cpp
//   File : notifierwindowtab.h
//   Creation date : Tue 07 Jul 2009 10:28 by Fabio Bas
//
//   This file is part of the KVIrc distribution
//   Copyright (C) 2009 Fabio Bas < ctrlaltca at gmail dot com >
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

#include "notifiermessage.h"
#include "notifierwindowtab.h"
#include "notifierwindow.h"
#include "notifiersettings.h"

#include "kvi_app.h"
#include "kvi_config.h"
#include "kvi_locale.h"
#include "kvi_frame.h"
#include "kvi_options.h"
#include "kvi_window.h"

#include <QScrollBar>
#include <QResizeEvent>
#include <QPainter>

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	#include <QPixmap>
	extern QPixmap       * g_pShadedChildGlobalDesktopBackground;
#endif

extern KviNotifierWindow * g_pNotifierWindow;

KviNotifierWindowTab::KviNotifierWindowTab(KviWindow * pWnd, QTabWidget *parent)
	: QScrollArea(parent)
{
	m_pWnd = pWnd;
	if(m_pWnd)
	{
		m_label = m_pWnd->windowName();
		connect(pWnd,SIGNAL(windowNameChanged()),this,SLOT(labelChanged()));
		connect(pWnd,SIGNAL(destroyed()),this,SLOT(closeMe()));
	} else {
		m_label = (QString)"----";
	}

	if(parent)
	{
		m_pParent=parent;
		m_pParent->addTab(this, m_label);
	}

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	if(verticalScrollBar())
		connect(verticalScrollBar(),SIGNAL(rangeChanged(int, int)),this, SLOT(scrollRangeChanged(int, int)));

	QPalette pal=palette();
	pal.setColor(backgroundRole(), Qt::transparent);
	setPalette(pal);

	m_pVWidget = new QWidget(viewport());
	m_pVBox = new QVBoxLayout(m_pVWidget);
	m_pVBox->setSizeConstraint(QLayout::SetFixedSize);
	m_pVBox->setSpacing(SPACING);
	m_pVBox->setMargin(SPACING);

	setWidget(m_pVWidget);
}

KviNotifierWindowTab::~KviNotifierWindowTab()
{
	if(m_pVBox)
		m_pVBox->deleteLater();
	if(m_pVWidget)
		m_pVWidget->deleteLater();
}

void KviNotifierWindowTab::appendMessage(KviNotifierMessage * m)
{
	m_pVBox->addWidget(m);
	m->setFixedWidth(viewport()->width());

	while(m_pVBox->count() > MAX_MESSAGES_IN_WINDOW)
	{
		QLayoutItem* tmp=m_pVBox->takeAt(0);
		KviNotifierMessage* tmp2=(KviNotifierMessage*)tmp->widget();
		if(tmp2)
			tmp2->deleteLater();
	}
}

void KviNotifierWindowTab::updateGui()
{
	for(int i=0; i<m_pVBox->count(); ++i)
	{
		QLayoutItem* tmp=m_pVBox->itemAt(i);
		KviNotifierMessage* tmp2=(KviNotifierMessage*)tmp->widget();
		if(tmp2)
			tmp2->updateGui();
	}
}

void KviNotifierWindowTab::scrollRangeChanged(int, int)
{
	//this is the autoscroll
	if(verticalScrollBar())
		verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
}

void KviNotifierWindowTab::labelChanged()
{
	if(!m_pWnd || !m_pParent)return;

	int i = m_pParent->indexOf(this);
	m_label = m_pWnd->windowName();
	if(i>-1)
		m_pParent->setTabText (i, m_label);
}

void KviNotifierWindowTab::mouseDoubleClickEvent(QMouseEvent *)
{
	if(!m_pWnd || !g_pNotifierWindow)return;
	if(!g_pApp->windowExists(m_pWnd))return;

	g_pNotifierWindow->hideNow();

	if(m_pWnd->mdiParent())
	{
		m_pWnd->frame()->raise();
		m_pWnd->frame()->setFocus();
		m_pWnd->frame()->setWindowState((m_pWnd->frame()->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
		if(!m_pWnd->frame()->isVisible())
			m_pWnd->frame()->show();
	}

	m_pWnd->frame()->setActiveWindow(m_pWnd);
}

void KviNotifierWindowTab::closeMe()
{
	//our window has been closed
	if(m_pParent && g_pNotifierWindow)
	{
		int index = m_pParent->indexOf(this);
		if(index!=-1)
		{
			g_pNotifierWindow->slotTabCloseRequested(index);
		}
	}
}

void KviNotifierWindowTab::resizeEvent(QResizeEvent *)
{
	if(m_pVBox)
	{
		int iWidth = viewport()->width();
		KviNotifierMessage* m;
		for(int i=0;i<m_pVBox->count(); i++)
		{
			m = (KviNotifierMessage*)m_pVBox->itemAt(i)->widget();
			if(m)
				m->setFixedWidth(iWidth);
		}
	}
}

void KviNotifierWindowTab::paintEvent(QPaintEvent * e)
{
	QPainter *p = new QPainter(viewport());

	//make sure you clean your widget with a transparent
	//  color before doing any rendering
	//  note the usage of a composition mode Source
	//  it's important!

	#ifdef COMPILE_PSEUDO_TRANSPARENCY
		if(KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing())
		{
			p->save();
			p->setCompositionMode(QPainter::CompositionMode_Source);
			QColor col=KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade);
			col.setAlphaF((float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) / (float)100));
			p->fillRect(e->rect(), col);
			p->restore();
		} else if(g_pShadedChildGlobalDesktopBackground)
		{
			QPoint pnt = mapToGlobal(e->rect().topLeft());
			p->drawTiledPixmap(e->rect(),*(g_pShadedChildGlobalDesktopBackground), pnt);
		} else {
	#endif
			QPixmap * pPix = KVI_OPTION_PIXMAP(KviOption_pixmapNotifierBackground).pixmap();

			if(pPix) KviPixmapUtils::drawPixmapWithPainter(p,pPix,KVI_OPTION_UINT(KviOption_uintNotifierPixmapAlign),e->rect(),e->rect().width(),e->rect().height());
			else p->fillRect(e->rect(),KVI_OPTION_COLOR(KviOption_colorNotifierBackground));
	#ifdef COMPILE_PSEUDO_TRANSPARENCY
		}
	#endif
	
	delete p;
	e->ignore();
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_notifierwindowtab.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
