//=============================================================================
//
//   File : KviAction.cpp
//   Creation date : Sun 21 Nov 2004 03:36:34 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviAction.h"
#include "KviCustomToolBar.h"
#include "KviMainWindow.h"
#include "KviIrcContext.h"
#include "KviApplication.h"
#include "KviWindow.h"
#include "KviChannelWindow.h"
#include "KviQueryWindow.h"
#include "KviTalPopupMenu.h"


KviActionCategory::KviActionCategory(const QString & szName, const QString & szVisibleName, const QString & szDescription)
: m_szName(szName), m_szVisibleName(szVisibleName), m_szDescription(szDescription)
{
}

KviActionCategory::~KviActionCategory()
{
}


KviAction::KviAction(QObject * pParent, const QString & szName, const QString & szVisibleName, const QString & szDescription, KviActionCategory * pCategory, const QString & szBigIconId, const QString & szSmallIconId, unsigned int uFlags, const QString & szKeySequence)
: QObject(pParent),m_szName(szName),m_szVisibleName(szVisibleName),m_szDescription(szDescription),m_pCategory(pCategory),m_szBigIconId(szBigIconId),m_szSmallIconId(szSmallIconId),m_pWidgetList(NULL),m_uInternalFlags(KviAction::Enabled),m_uFlags(uFlags),m_szKeySequence(szKeySequence),m_pAccel(NULL)
{
}

KviAction::KviAction(QObject * pParent, const QString & szName, const QString & szVisibleName, const QString & szDescription, KviActionCategory * pCategory, const QString & szBigIconId, const KviIconManager::SmallIcon eIcon, unsigned int uFlags, const QString & szKeySequence)
: QObject(pParent),m_szName(szName),m_szVisibleName(szVisibleName),m_szDescription(szDescription),m_pCategory(pCategory),m_szBigIconId(szBigIconId),m_eIcon(eIcon),m_pWidgetList(NULL),m_uInternalFlags(KviAction::Enabled),m_uFlags(uFlags),m_szKeySequence(szKeySequence),m_pAccel(NULL)
{
}

KviAction::~KviAction()
{
	if(m_pWidgetList)
	{
		for(QWidget * pWidget = m_pWidgetList->first(); pWidget; pWidget = m_pWidgetList->next())
			disconnect(pWidget,SIGNAL(destroyed()),this,SLOT(widgetDestroyed()));
		m_pWidgetList->setAutoDelete(true);
		delete m_pWidgetList;
	}
	
	if(m_pAccel)
		unregisterAccelerator();
}

const QString & KviAction::visibleName()
{
	return m_szVisibleName;
}

const QString & KviAction::description()
{
	return m_szDescription;
}

bool KviAction::isKviUserActionNeverOverrideThis()
{
	return false;
}

void KviAction::registerAccelerator()
{
	if(m_pAccel)
		delete m_pAccel;

	if(!m_szKeySequence.isEmpty())
	{
		m_pAccel = new QShortcut(m_szKeySequence,g_pMainWindow,0,0,Qt::ApplicationShortcut);
		connect(m_pAccel,SIGNAL(activated()),this,SLOT(activate()));
		//no way to have Ctrl+Alt+Key events fired as no-ambiguous, probably qt bug
		connect(m_pAccel,SIGNAL(activatedAmbiguously()),this,SLOT(activate()));
	} else {
		m_pAccel = NULL;
	}
}

void KviAction::unregisterAccelerator()
{
	if(!m_pAccel)
		return;

	delete m_pAccel;

	m_pAccel = NULL;
}

void KviAction::setEnabled(bool bEnabled)
{
	if(bEnabled)
		m_uInternalFlags |= KviAction::Enabled;
	else
		m_uInternalFlags &= ~KviAction::Enabled;

	if(m_pWidgetList)
	{
		if(bEnabled)
		{
			for(QWidget * pWidget = m_pWidgetList->first(); pWidget; pWidget = m_pWidgetList->next())
			{
				if(!pWidget->isEnabled())
					pWidget->setEnabled(true);
			}
		} else {
			for(QWidget * pWidget = m_pWidgetList->first(); pWidget; pWidget = m_pWidgetList->next())
			{
				if(pWidget->isEnabled())
					pWidget->setEnabled(false);
			}
		}
	}
}

int KviAction::validateFlags(int iFlagsToValidate)
{
	if(iFlagsToValidate & EnableAtLogin)
		iFlagsToValidate |= NeedsConnection;
	if(iFlagsToValidate & NeedsConnection)
		iFlagsToValidate |= NeedsContext;
	if(iFlagsToValidate & WindowOnlyIfUsersSelected)
	{
		if(!(iFlagsToValidate & InternalWindowMask))
			iFlagsToValidate &= ~WindowOnlyIfUsersSelected;
	}
	return iFlagsToValidate;
}

QPixmap * KviAction::smallIcon()
{
	if(m_szSmallIconId.isEmpty())
		return NULL;
	return g_pIconManager->getImage(m_szSmallIconId);
}

QPixmap * KviAction::bigIcon()
{
	if(m_szBigIconId.isEmpty())
		return NULL;
	return g_pIconManager->getImage(m_szBigIconId);
}

void KviAction::setup()
{
	connect(g_pApp,SIGNAL(reloadImages()),this,SLOT(reloadImages()));
	if(m_uFlags & InternalWindowMask)
	{
		connect(g_pMainWindow,SIGNAL(activeWindowChanged()),this,SLOT(activeWindowChanged()));
		if((m_uFlags & WindowOnlyIfUsersSelected) && (m_uFlags & (WindowChannel | WindowConsole | WindowQuery)))
			connect(g_pMainWindow,SIGNAL(activeWindowSelectionStateChanged(bool)),this,SLOT(activeWindowSelectionStateChanged(bool)));
		activeWindowChanged();
	} else {
		if(m_uFlags & NeedsConnection)
		{
			connect(g_pMainWindow,SIGNAL(activeContextChanged()),this,SLOT(activeContextChanged()));
			connect(g_pMainWindow,SIGNAL(activeContextStateChanged()),this,SLOT(activeContextStateChanged()));
			KviIrcContext * pContext = g_pMainWindow->activeContext();
			if(!pContext)
				setEnabled(false);
			else {
				switch(pContext->state())
				{
					case KviIrcContext::LoggingIn:
						setEnabled(m_uFlags & EnableAtLogin);
					break;
					case KviIrcContext::Connected:
						setEnabled(true);
					break;
					default:
						setEnabled(false);
					break;
				}
			}
		} else {
			if(m_uFlags & NeedsContext)
			{
				connect(g_pMainWindow,SIGNAL(activeContextChanged()),this,SLOT(activeContextChanged()));
				if(!g_pMainWindow->activeContext())
					setEnabled(false);
				else
					setEnabled(true);
			}
		}
	}

	m_uInternalFlags |= KviAction::SetupDone;
}


void KviAction::reloadImages()
{
	if(!m_pWidgetList)
		return;
	QPixmap * pPix = bigIcon();
	for(QWidget * pWidget = m_pWidgetList->first(); pWidget; pWidget = m_pWidgetList->next())
	{
		if(pWidget->inherits("QToolButton"))
			((QToolButton *)pWidget)->setIcon(pPix ? *pPix : QPixmap());
	}
}

void KviAction::activeWindowChanged()
{
	if(m_uFlags & NeedsContext)
	{
		if(!g_pMainWindow->activeContext())
		{
			if(isEnabled())
				setEnabled(false);
			return;
		}
	}

	if(m_uFlags & NeedsConnection)
	{
		switch(g_pMainWindow->activeContext()->state())
		{
			case KviIrcContext::LoggingIn:
				if(!(m_uFlags & EnableAtLogin))
				{
					if(isEnabled())
						setEnabled(false);
					return;
				}
			break;
			case KviIrcContext::Connected:
				// this is ok
			break;
			default:
				if(isEnabled())
					setEnabled(false);
				return;
			break;
		}
	}

	if(!g_pActiveWindow)
	{
		if(isEnabled())
			setEnabled(false);
		return;
	}

	switch(g_pActiveWindow->type())
	{
		case KviWindow::Console:
			if(m_uFlags & WindowConsole)
			{
				if(m_uFlags & WindowOnlyIfUsersSelected)
				{
					bool bEnabled = ((KviConsoleWindow *)g_pActiveWindow)->selectedCount() > 0;
					if(bEnabled != isEnabled())
						setEnabled(bEnabled);
				} else {
					if(!isEnabled())
						setEnabled(true);
				}
			} else {
				if(isEnabled())
					setEnabled(false);
			}
			break;
		case KviWindow::Channel:
			if(m_uFlags & WindowChannel)
			{
				if(m_uFlags & WindowOnlyIfUsersSelected)
				{
					bool bEnabled = ((KviChannelWindow *)g_pActiveWindow)->selectedCount() > 0;
					if(bEnabled != isEnabled())
						setEnabled(bEnabled);
				} else {
					if(!isEnabled())
						setEnabled(true);
				}
			} else {
				if(isEnabled())
					setEnabled(false);
			}
			break;
		case KviWindow::Query:
			if(m_uFlags & WindowQuery)
			{
				if(m_uFlags & WindowOnlyIfUsersSelected)
				{
					bool bEnabled = ((KviQueryWindow *)g_pActiveWindow)->selectedCount() > 0;
					if(bEnabled != isEnabled())
						setEnabled(bEnabled);
				} else {
					if(!isEnabled())
						setEnabled(true);
				}
			} else {
				if(isEnabled())
					setEnabled(false);
			}
			break;
		case KviWindow::DccChat:
			if(m_uFlags & WindowDccChat)
			{
				if(!isEnabled())
					setEnabled(true);
			} else {
				if(isEnabled())
					setEnabled(false);
			}
			break;
		default:
			if(m_uFlags & InternalWindowMask)
			{
				if(isEnabled())
					setEnabled(false);
			} else {
				if(!isEnabled())
					setEnabled(true);
			}
			break;
	}
}

void KviAction::activeWindowSelectionStateChanged(bool bSelectedNow)
{
	// we jump here ONLY if m_uFlags & WindowOnlyIfUsersSelected
	// and thus also m_uFlags & InternalWindowMask
	switch(g_pActiveWindow->type())
	{
		case KviWindow::Console:
			if(m_uFlags & WindowConsole)
			{
				if(bSelectedNow != isEnabled())
					setEnabled(bSelectedNow);
			} else {
				if(isEnabled())
					setEnabled(false);
			}
			break;
		case KviWindow::Channel:
			if(m_uFlags & WindowChannel)
			{
				if(bSelectedNow != isEnabled())
					setEnabled(bSelectedNow);
			} else {
				if(isEnabled())
					setEnabled(false);
			}
			break;
		case KviWindow::Query:
			if(m_uFlags & WindowQuery)
			{
				if(bSelectedNow != isEnabled())
					setEnabled(bSelectedNow);
			} else {
				if(isEnabled())
					setEnabled(false);
			}
			break;
		case KviWindow::DccChat:
			if(m_uFlags & WindowDccChat)
			{
				if(!isEnabled())
					setEnabled(true);
			} else {
				if(isEnabled())
					setEnabled(false);
			}
			break;
		default:
			if(isEnabled())
				setEnabled(false);
			break;
	}
}

void KviAction::activeContextChanged()
{
	// works only if NeedsContext is specified!
	KviIrcContext * pContext = g_pMainWindow->activeContext();
	if(pContext)
	{
		if(m_uFlags & NeedsConnection)
			activeContextStateChanged();
		else
			if(!isEnabled())
				setEnabled(true);
	} else {
		if(isEnabled())
			setEnabled(false);
	}
}

void KviAction::activeContextStateChanged()
{
	KviIrcContext * pContext = g_pMainWindow->activeContext();
	if(pContext)
	{
		switch(pContext->state())
		{
			case KviIrcContext::Idle:
			case KviIrcContext::Connecting:
				if(isEnabled())
					setEnabled(false);
			break;
			case KviIrcContext::LoggingIn:
				if(m_uFlags & EnableAtLogin)
				{
					if(!isEnabled())
						setEnabled(true);
				} else {
					if(isEnabled())
						setEnabled(false);
				}
			break;
			case KviIrcContext::Connected:
				if(!isEnabled())
					setEnabled(true);
			break;
			default:
				if(isEnabled())
					setEnabled(false);
			break;
		}
	} else {
		if(isEnabled())
			setEnabled(false);
	}
}

bool KviAction::addToPopupMenu(KviTalPopupMenu * pMenu)
{
	if(!setupDone())
		setup();
	QPixmap * pPix = smallIcon();
	int iId;
	QString szTmp = visibleName();

	if(!m_szKeySequence.isEmpty())
		szTmp += '\t' + m_szKeySequence;

	if(pPix)
		iId = pMenu->insertItem(*pPix,szTmp,this,SLOT(activate()));
	else
		iId = pMenu->insertItem(szTmp,this,SLOT(activate()));

	if(!isEnabled())
		pMenu->setItemEnabled(iId,false);
	return true;
}

void KviAction::widgetDestroyed()
{
	if(!m_pWidgetList)
		return;
	QWidget * pWidget = (QWidget *)sender();
	m_pWidgetList->removeRef(pWidget);
}

void KviAction::registerWidget(QWidget * pWidget)
{
	connect(pWidget,SIGNAL(destroyed()),this,SLOT(widgetDestroyed()));
	if(!m_pWidgetList)
	{
		m_pWidgetList = new KviPointerList<QWidget>;
		m_pWidgetList->setAutoDelete(false);
	}
	m_pWidgetList->append(pWidget);
}

QWidget * KviAction::addToCustomToolBar(KviCustomToolBar * pParentToolBar)
{
	if(!setupDone())
		setup();
	QPixmap * pPix = bigIcon();
	QToolButton * pTool = new QToolButton(pParentToolBar);

	pTool->setIcon(pPix ? *pPix : QPixmap());
	pTool->setText(visibleName());
	pTool->setToolTip(visibleName());
	pTool->setObjectName(m_szName.toUtf8().data());
	connect(pTool,SIGNAL(clicked()),this,SLOT(activate()));

	QAction * pAction = pParentToolBar->addWidget(pTool);
	pAction->setVisible(true);
	if(!isEnabled())
		pTool->setEnabled(false);
	registerWidget(pTool);
	return pTool;
}

void KviAction::activate()
{
	if(isEnabled())
		emit activated();
}
