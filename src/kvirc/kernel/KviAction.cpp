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

#include "KviAction.h"
#include "KviCustomToolBar.h"
#include "KviMainWindow.h"
#include "KviIrcContext.h"
#include "KviApplication.h"
#include "KviWindow.h"
#include "KviChannelWindow.h"
#include "KviQueryWindow.h"
#include "KviOptions.h"

#include <QAction>
#include <QMenu>

KviActionCategory::KviActionCategory(const QString & szName, const QString & szVisibleName, const QString & szDescription)
: m_szName(szName), m_szVisibleName(szVisibleName), m_szDescription(szDescription)
{
}

KviActionCategory::~KviActionCategory()
{
}


KviAction::KviAction(QObject * pParent, const QString & szName, const QString & szVisibleName, const QString & szDescription, KviActionCategory * pCategory, const QString & szBigIconId, const QString & szSmallIconId, unsigned int uFlags, const QString & szKeySequence)
: QObject(pParent),
	m_szName(szName),
	m_szVisibleName(szVisibleName),
	m_szDescription(szDescription),
	m_pCategory(pCategory),
	m_szBigIconId(szBigIconId),
	m_szSmallIconId(szSmallIconId),
	m_eSmallIcon(KviIconManager::None),
	m_pActionList(NULL),
	m_uInternalFlags(KviAction::Enabled),
	m_uFlags(uFlags),
	m_szKeySequence(szKeySequence),
	m_pAccel(NULL)
{
}

KviAction::KviAction(QObject * pParent, const QString & szName, const QString & szVisibleName, const QString & szDescription, KviActionCategory * pCategory, const QString & szBigIconId, KviIconManager::SmallIcon eSmallIcon, unsigned int uFlags, const QString & szKeySequence)
: QObject(pParent),
	m_szName(szName),
	m_szVisibleName(szVisibleName),
	m_szDescription(szDescription),
	m_pCategory(pCategory),
	m_szBigIconId(szBigIconId),
	m_eSmallIcon(eSmallIcon),
	m_pActionList(NULL),
	m_uInternalFlags(KviAction::Enabled),
	m_uFlags(uFlags),
	m_szKeySequence(szKeySequence),
	m_pAccel(NULL)
{
}

KviAction::~KviAction()
{
	if(m_pActionList)
	{
		for(QAction * pAction = m_pActionList->first(); pAction; pAction = m_pActionList->next())
			disconnect(pAction,SIGNAL(destroyed()),this,SLOT(actionDestroyed()));
		m_pActionList->setAutoDelete(true);
		delete m_pActionList;
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
		g_pMainWindow->freeAccelleratorKeySequence(m_szKeySequence);

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

	if(m_pActionList)
	{
		if(bEnabled)
		{
			for(QAction * pAction = m_pActionList->first(); pAction; pAction = m_pActionList->next())
			{
				if(!pAction->isEnabled())
					pAction->setEnabled(true);
			}
		} else {
			for(QAction * pAction = m_pActionList->first(); pAction; pAction = m_pActionList->next())
			{
				if(pAction->isEnabled())
					pAction->setEnabled(false);
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
	if(!m_szSmallIconId.isEmpty())
	{
		QPixmap * pPix = g_pIconManager->getImage(m_szSmallIconId);
		if(!pPix)
			return NULL;
	}
	if(m_eSmallIcon != KviIconManager::None)
		return g_pIconManager->getSmallIcon(m_eSmallIcon);
	return NULL;
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
	if(!m_pActionList)
		return;

	QPixmap * pBigPix = bigIcon();
	QPixmap * pSmallPix = smallIcon();

	QIcon icon;
	if(pBigPix)
		icon.addPixmap(*pBigPix);
	if(pSmallPix)
		icon.addPixmap(*pSmallPix);

	bool bIconVisibleInMenu = KVI_OPTION_BOOL(KviOption_boolShowIconsInPopupMenus);

	for(QAction * pAction = m_pActionList->first(); pAction; pAction = m_pActionList->next())
	{
		pAction->setIcon(icon);
		pAction->setIconVisibleInMenu(bIconVisibleInMenu);
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

bool KviAction::addToPopupMenu(QMenu * pMenu)
{
	if(!setupDone())
		setup();
	QPixmap * pPix = smallIcon();
	QString szTmp = visibleName();

	QAction *pAction;
	if(pPix)
        pAction = pMenu->addAction(*pPix,szTmp,this,SLOT(activate()));
	else
        pAction = pMenu->addAction(szTmp,this,SLOT(activate()));

	// avoid Qt to put this action in the application menu based on its title
	pAction->setMenuRole(QAction::NoRole);

	if(!m_szKeySequence.isEmpty())
		pAction->setShortcut(m_szKeySequence);

	if(!isEnabled())
		pAction->setEnabled(false);
	registerAction(pAction);
	return true;
}

void KviAction::actionDestroyed()
{
	if(!m_pActionList)
		return;
	QAction * pAction = (QAction *)sender();
	m_pActionList->removeRef(pAction);
}

void KviAction::registerAction(QAction * pAction)
{
	connect(pAction,SIGNAL(destroyed()),this,SLOT(actionDestroyed()));
	if(!m_pActionList)
	{
		m_pActionList = new KviPointerList<QAction>;
		m_pActionList->setAutoDelete(false);
	}
	m_pActionList->append(pAction);
}

QAction * KviAction::addToCustomToolBar(KviCustomToolBar * pParentToolBar)
{
	if(!setupDone())
		setup();

	QPixmap * pPix = bigIcon();

	QAction * pAction = new QAction(pPix ? *pPix : QPixmap(), visibleName(), pParentToolBar);
	// important: when customizing the toolbar, we'll get the action name from QAction::objectName();
	pAction->setObjectName(m_szName);
	pParentToolBar->addAction(pAction);

	connect(pAction,SIGNAL(triggered()),this,SLOT(activate()));

	pAction->setVisible(true);
	if(!isEnabled())
		pAction->setEnabled(false);
	registerAction(pAction);
	return pAction;
}

void KviAction::activate()
{
	if(isEnabled())
		emit activated();
}
