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

#include "kvi_config.h"
#include "kvi_locale.h"
#include "kvi_frame.h"
#include "kvi_options.h"
#include "kvi_window.h"

#include <QScrollBar>

extern KviNotifierWindow * g_pNotifierWindow;

KviNotifierWindowTab::KviNotifierWindowTab(KviWindow * pWnd, QTabWidget *parent)
{
	m_pWnd = pWnd;
	if(m_pWnd)
	{
		m_label = m_pWnd->windowName();
		connect(pWnd,SIGNAL(windowNameChanged()),this,SLOT(labelChanged()));
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
	while(m_pVBox->count() > MAX_MESSAGES_IN_WINDOW)
	{
		QLayoutItem* tmp=m_pVBox->takeAt(0);
		KviNotifierMessage* tmp2=(KviNotifierMessage*)tmp->widget();
		if(tmp2)
			tmp2->deleteLater();
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
		if(!m_pWnd->frame()->isVisible())
			m_pWnd->frame()->show();
	}

	m_pWnd->frame()->setActiveWindow(m_pWnd);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_notifierwindowtab.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
