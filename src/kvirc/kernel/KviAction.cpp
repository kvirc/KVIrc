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
#include "KviApplication.h"
#include "KviChannelWindow.h"
#include "KviCustomToolBar.h"
#include "KviIrcContext.h"
#include "KviMainWindow.h"
#include "KviOptions.h"
#include "KviQueryWindow.h"
#include "KviWindow.h"

#include <QAction>
#include <QMenu>

KviAction::KviAction(QObject * pParent, QString szName, QString szVisibleName, QString szDescription, KviActionCategory * pCategory, QString szBigIconId, QString szSmallIconId, unsigned int uFlags, QString szKeySequence)
    : QObject(pParent),
      m_szName(std::move(szName)),
      m_szVisibleName(std::move(szVisibleName)),
      m_szDescription(std::move(szDescription)),
      m_pCategory(pCategory),
      m_szBigIconId(std::move(szBigIconId)),
      m_szSmallIconId(std::move(szSmallIconId)),
      m_eSmallIcon(KviIconManager::None),
      m_uInternalFlags(KviAction::Enabled),
      m_uFlags(uFlags),
      m_szKeySequence(std::move(szKeySequence)),
      m_pAccel()
{
}

KviAction::KviAction(QObject * pParent, QString szName, QString szVisibleName, QString szDescription, KviActionCategory * pCategory, QString szBigIconId, KviIconManager::SmallIcon eSmallIcon, unsigned int uFlags, QString szKeySequence)
    : QObject(pParent),
      m_szName(std::move(szName)),
      m_szVisibleName(std::move(szVisibleName)),
      m_szDescription(std::move(szDescription)),
      m_pCategory(pCategory),
      m_szBigIconId(std::move(szBigIconId)),
      m_eSmallIcon(std::move(eSmallIcon)),
      m_uInternalFlags(KviAction::Enabled),
      m_uFlags(uFlags),
      m_szKeySequence(std::move(szKeySequence)),
      m_pAccel()
{
}

KviAction::~KviAction()
{
	for(auto & pAction : m_pActionList)
		disconnect(pAction, SIGNAL(destroyed()), this, SLOT(actionDestroyed()));

	this->unregisterAccelerator();
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
	this->unregisterAccelerator();
	if(!m_szKeySequence.isEmpty())
	{
		g_pMainWindow->freeAccelleratorKeySequence(m_szKeySequence);

		m_pAccel = new QShortcut(m_szKeySequence, g_pMainWindow, nullptr, nullptr, Qt::ApplicationShortcut);
		connect(m_pAccel, SIGNAL(activated()), this, SLOT(activate()));
		//no way to have Ctrl+Alt+Key events fired as no-ambiguous, probably qt bug
		connect(m_pAccel, SIGNAL(activatedAmbiguously()), this, SLOT(activate()));
	}
}

void KviAction::unregisterAccelerator()
{
	delete m_pAccel;
	m_pAccel = nullptr;
}

void KviAction::setEnabled(bool bEnabled)
{
	if(bEnabled)
		m_uInternalFlags |= KviAction::Enabled;
	else
		m_uInternalFlags &= ~KviAction::Enabled;

	for(auto & pAction : m_pActionList)
	{
		if (pAction->isEnabled() != bEnabled)
			pAction->setEnabled(bEnabled);
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
			return nullptr;
	}
	if(m_eSmallIcon != KviIconManager::None)
		return g_pIconManager->getSmallIcon(m_eSmallIcon);
	return nullptr;
}

QPixmap * KviAction::bigIcon()
{
	if(m_szBigIconId.isEmpty())
		return nullptr;
	return g_pIconManager->getImage(m_szBigIconId);
}

void KviAction::setup()
{
	connect(g_pApp, SIGNAL(reloadImages()), this, SLOT(reloadImages()));
	if(m_uFlags & InternalWindowMask)
	{
		connect(g_pMainWindow, SIGNAL(activeWindowChanged()), this, SLOT(activeWindowChanged()));
		if((m_uFlags & WindowOnlyIfUsersSelected) && (m_uFlags & (WindowChannel | WindowConsole | WindowQuery)))
			connect(g_pMainWindow, SIGNAL(activeWindowSelectionStateChanged(bool)), this, SLOT(activeWindowSelectionStateChanged(bool)));
		activeWindowChanged();
	}
	else
	{
		if(m_uFlags & NeedsConnection)
		{
			connect(g_pMainWindow, SIGNAL(activeContextChanged()), this, SLOT(activeContextChanged()));
			connect(g_pMainWindow, SIGNAL(activeContextStateChanged()), this, SLOT(activeContextStateChanged()));
			KviIrcContext * pContext = g_pMainWindow->activeContext();
			if(!pContext)
				setEnabled(false);
			else
			{
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
		}
		else
		{
			if(m_uFlags & NeedsContext)
			{
				connect(g_pMainWindow, SIGNAL(activeContextChanged()), this, SLOT(activeContextChanged()));
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
	if(m_pActionList.empty())
		return;

	QPixmap * pBigPix = bigIcon();
	QPixmap * pSmallPix = smallIcon();

	QIcon icon;
	if(pBigPix)
		icon.addPixmap(*pBigPix);
	if(pSmallPix)
		icon.addPixmap(*pSmallPix);

	bool bIconVisibleInMenu = KVI_OPTION_BOOL(KviOption_boolShowIconsInPopupMenus);

	for(auto & pAction : m_pActionList)
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
				}
				else
				{
					if(!isEnabled())
						setEnabled(true);
				}
			}
			else
			{
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
				}
				else
				{
					if(!isEnabled())
						setEnabled(true);
				}
			}
			else
			{
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
				}
				else
				{
					if(!isEnabled())
						setEnabled(true);
				}
			}
			else
			{
				if(isEnabled())
					setEnabled(false);
			}
			break;
		case KviWindow::DccChat:
			if(m_uFlags & WindowDccChat)
			{
				if(!isEnabled())
					setEnabled(true);
			}
			else
			{
				if(isEnabled())
					setEnabled(false);
			}
			break;
		default:
			if(m_uFlags & InternalWindowMask)
			{
				if(isEnabled())
					setEnabled(false);
			}
			else
			{
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
			}
			else
			{
				if(isEnabled())
					setEnabled(false);
			}
			break;
		case KviWindow::Channel:
			if(m_uFlags & WindowChannel)
			{
				if(bSelectedNow != isEnabled())
					setEnabled(bSelectedNow);
			}
			else
			{
				if(isEnabled())
					setEnabled(false);
			}
			break;
		case KviWindow::Query:
			if(m_uFlags & WindowQuery)
			{
				if(bSelectedNow != isEnabled())
					setEnabled(bSelectedNow);
			}
			else
			{
				if(isEnabled())
					setEnabled(false);
			}
			break;
		case KviWindow::DccChat:
			if(m_uFlags & WindowDccChat)
			{
				if(!isEnabled())
					setEnabled(true);
			}
			else
			{
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
		else if(!isEnabled())
			setEnabled(true);
	}
	else
	{
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
				}
				else
				{
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
	}
	else
	{
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

	if(!m_szKeySequence.isEmpty())
		szTmp += '\t' + m_szKeySequence;

	QAction * pAction;
	if(pPix)
		pAction = pMenu->addAction(*pPix, szTmp, this, SLOT(activate()));
	else
		pAction = pMenu->addAction(szTmp, this, SLOT(activate()));

	// avoid Qt to put this action in the application menu based on its title
	pAction->setMenuRole(QAction::NoRole);

	if(!isEnabled())
		pAction->setEnabled(false);
	registerAction(pAction);
	return true;
}

void KviAction::actionDestroyed()
{
	QAction * pAction = (QAction *)sender();
	m_pActionList.erase(pAction);
}

void KviAction::registerAction(QAction * pAction)
{
	if(m_pActionList.count(pAction))
	{
		qDebug("Attempt to re-register pAction?");
		return;
	}

	connect(pAction, SIGNAL(destroyed()), this, SLOT(actionDestroyed()));
	m_pActionList.insert(pAction);
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

	connect(pAction, SIGNAL(triggered()), this, SLOT(activate()));

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
