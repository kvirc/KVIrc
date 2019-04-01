//=============================================================================
//
//   File : NotifierWindowTab.cpp
//   File : NotifierWindowTab.h
//   Creation date : Tue 07 Jul 2009 10:28 by Fabio Bas
//
//   This file is part of the KVIrc distribution
//   Copyright (C) 2009 Fabio Bas < ctrlaltca at gmail dot com >
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

#include "NotifierMessage.h"
#include "NotifierWindowTab.h"
#include "NotifierWindow.h"
#include "NotifierSettings.h"

#include "KviApplication.h"
#include "KviConfigurationFile.h"
#include "KviLocale.h"
#include "KviMainWindow.h"
#include "KviOptions.h"
#include "KviPixmapUtils.h"
#include "KviWindow.h"

#include <QPainter>
#include <QResizeEvent>
#include <QScrollBar>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

#ifdef COMPILE_PSEUDO_TRANSPARENCY
#include <QPixmap>
extern KVIRC_API QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

extern NotifierWindow * g_pNotifierWindow;

NotifierWindowTab::NotifierWindowTab(KviWindow * pWnd, QTabWidget * pParent)
    : QScrollArea(pParent), m_pWnd{pWnd}, m_pParent{pParent}
{
	if(m_pWnd)
	{
		m_szLabel = m_pWnd->windowName();
		connect(pWnd, SIGNAL(windowNameChanged()), this, SLOT(labelChanged()));
		connect(pWnd, SIGNAL(destroyed()), this, SLOT(closeMe()));
	}
	else
	{
		m_szLabel = "----";
	}

	if(m_pParent)
		m_pParent->addTab(this, m_szLabel);

	setFocusPolicy(Qt::NoFocus);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	if(verticalScrollBar())
		connect(verticalScrollBar(), SIGNAL(rangeChanged(int, int)), this, SLOT(scrollRangeChanged(int, int)));

	QPalette pal = palette();
	pal.setColor(backgroundRole(), Qt::transparent);
	setPalette(pal);

	m_pVWidget = new QWidget(viewport());
	m_pVBox = new QVBoxLayout(m_pVWidget);
	m_pVBox->setSizeConstraint(QLayout::SetFixedSize);
	m_pVBox->setSpacing(SPACING);
	m_pVBox->setMargin(SPACING);

	setWidget(m_pVWidget);
}

NotifierWindowTab::~NotifierWindowTab()
{
	if(m_pVBox)
		m_pVBox->deleteLater();
	if(m_pVWidget)
		m_pVWidget->deleteLater();
}

void NotifierWindowTab::appendMessage(NotifierMessage * pMessage)
{
	m_pVBox->addWidget(pMessage);
	pMessage->setFixedWidth(viewport()->width());

	while(m_pVBox->count() > MAX_MESSAGES_IN_WINDOW)
	{
		QLayoutItem * pTmp = m_pVBox->takeAt(0);
		NotifierMessage * pTmp2 = (NotifierMessage *)pTmp->widget();
		if(pTmp2)
			pTmp2->deleteLater();
	}
}

void NotifierWindowTab::updateGui()
{
	for(int i = 0; i < m_pVBox->count(); ++i)
	{
		QLayoutItem * pTmp = m_pVBox->itemAt(i);
		NotifierMessage * pTmp2 = (NotifierMessage *)pTmp->widget();
		if(pTmp2)
			pTmp2->updateGui();
	}
}

void NotifierWindowTab::scrollRangeChanged(int, int)
{
	//this is the autoscroll
	if(verticalScrollBar())
		verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
}

void NotifierWindowTab::labelChanged()
{
	if(!m_pWnd || !m_pParent)
		return;

	int iIdx = m_pParent->indexOf(this);
	m_szLabel = m_pWnd->windowName();
	if(iIdx > -1)
		m_pParent->setTabText(iIdx, m_szLabel);
}

void NotifierWindowTab::mouseDoubleClickEvent(QMouseEvent *)
{
	if(!m_pWnd || !g_pNotifierWindow)
		return;
	if(!g_pApp->windowExists(m_pWnd))
		return;

	g_pNotifierWindow->hideNow();

	if(m_pWnd->isDocked())
	{
		g_pMainWindow->raise();
		g_pMainWindow->setFocus();
		g_pMainWindow->setWindowState((g_pMainWindow->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
		if(!g_pMainWindow->isVisible())
			g_pMainWindow->show();
	}

	g_pMainWindow->setActiveWindow(m_pWnd);
}

void NotifierWindowTab::closeMe()
{
	//our window has been closed
	if(m_pParent && g_pNotifierWindow)
	{
		int iIdx = m_pParent->indexOf(this);
		if(iIdx != -1)
			g_pNotifierWindow->slotTabCloseRequested(iIdx);
	}
}

void NotifierWindowTab::resizeEvent(QResizeEvent *)
{
	int iWidth = viewport()->width();
	for(int i = 0; i < m_pVBox->count(); i++)
	{
		NotifierMessage * pMessage = (NotifierMessage *)m_pVBox->itemAt(i)->widget();
		if(pMessage)
			pMessage->setFixedWidth(iWidth);
	}
}

void NotifierWindowTab::paintEvent(QPaintEvent * e)
{
	QPainter * pPainter = new QPainter(viewport());

//make sure you clean your widget with a transparent
//  color before doing any rendering
//  note the usage of a composition mode Source
//  it's important!

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing())
	{
		pPainter->save();
		pPainter->setCompositionMode(QPainter::CompositionMode_Source);
		QColor col = KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade);
		col.setAlphaF((float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) / (float)100));
		pPainter->fillRect(e->rect(), col);
		pPainter->restore();
	}
	else if(g_pShadedChildGlobalDesktopBackground) // This doesn't work as the text doesn't actually use the right foreground color
	{
		QPoint pnt = mapToGlobal(e->rect().topLeft());
		pPainter->drawTiledPixmap(e->rect(), *(g_pShadedChildGlobalDesktopBackground), pnt);
	}
	else
	{
#endif
		QPixmap * pPix = KVI_OPTION_PIXMAP(KviOption_pixmapNotifierBackground).pixmap();

		if(pPix)
			KviPixmapUtils::drawPixmapWithPainter(pPainter, pPix, KVI_OPTION_UINT(KviOption_uintNotifierPixmapAlign), e->rect(), e->rect().width(), e->rect().height());
		else
			pPainter->fillRect(e->rect(), KVI_OPTION_COLOR(KviOption_colorNotifierBackground));
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif

	delete pPainter;
	e->ignore();
}
