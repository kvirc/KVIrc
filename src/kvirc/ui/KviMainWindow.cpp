//=============================================================================
//
//   File : KviMainWindow.cpp
//   Creation date : Sun Jun 18 2000 17:59:02 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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


#define _KVI_FRAME_CPP_

#include "kvi_debug.h"
#include "KviApplication.h"
#include "kvi_settings.h"
#include "KviMainWindow.h"
#include "KviOptions.h"
#include "KviMenuBar.h"
#include "KviMdiManager.h"
#include "KviMdiChild.h"
#include "KviIconManager.h"
#include "KviWindow.h"
#include "KviWindowListBase.h"
#include "KviTreeWindowList.h"
#include "KviConsoleWindow.h"
#include "KviConfigurationFile.h"
#include "KviInternalCommand.h"
#include "KviConsoleWindow.h"
#include "kvi_debug.h"
#include "KviIrcToolBar.h"
#include "kvi_confignames.h"
#include "KviParameterList.h"
#include "KviModule.h"
#include "KviMexToolBar.h"
#include "KviLocale.h"
#include "KviIrcContext.h"
#include "KviStatusBar.h"
#include "KviCustomToolBar.h"
#include "KviCustomToolBarManager.h"
#include "KviCustomToolBarDescriptor.h"
#include "KviActionManager.h"
#include "kvi_defaults.h"
#include "KviIrcView.h"
#include "KviKvsScript.h"
#include "KviKvsEventTriggers.h"
#include "KviTalPopupMenu.h"
#include "KviTextIconManager.h"
#include "kvi_shortcuts.h"

#include <QSplitter>
#include <QVariant>
#include <QLineEdit>
#include <QMessageBox>
#include <QTimer>
#include <QLayout>
#include <QDesktopWidget>
#include <QEvent>
#include <QCloseEvent>
#include <QShortcut>
#include <QFile>

#include <time.h>

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	#include <QPixmap>
	// KviApplication.h
	extern QPixmap * g_pShadedParentGlobalDesktopBackground;
	extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

#define KVI_DEFAULT_FRAME_CAPTION "KVIrc " KVI_VERSION " " KVI_RELEASE_NAME

// Declared and managed by KviApplication (KviApplication.cpp)
extern KviConfigurationFile * g_pWinPropertiesConfig;
KVIRC_API KviMainWindow * g_pMainWindow = 0; // the one and only frame object

KviMainWindow::KviMainWindow()
: KviTalMainWindow(0,"kvirc_frame")
{
	g_pMainWindow = this;
	setAutoFillBackground(false);
	setAttribute(Qt::WA_TranslucentBackground);
	//disable this flag that gets enabled by qt when using Qt::WA_TranslucentBackground
	setAttribute(Qt::WA_NoSystemBackground, false);

	m_pWinList  = new KviPointerList<KviWindow>;
	setWindowTitle(KVI_DEFAULT_FRAME_CAPTION);
	m_pWinList->setAutoDelete(false);

	m_pModuleExtensionToolBarList = new KviPointerList<KviMexToolBar>;
	m_pModuleExtensionToolBarList->setAutoDelete(false);

	m_pActiveContext = 0;

	m_pDockExtension = 0;

	m_pSplitter = new QSplitter(Qt::Horizontal,this);
	m_pSplitter->setObjectName("main_frame_splitter");
	m_pSplitter->setChildrenCollapsible(false);

	setCentralWidget(m_pSplitter);

	setUsesBigPixmaps(KVI_OPTION_BOOL(KviOption_boolUseBigIcons));

	m_pMdi      = new KviMdiManager(m_pSplitter,this,"mdi_manager");

	// This theoretically had to exists before KviMdiManager (that uses enterSdiMode)
	m_pMenuBar   = new KviMenuBar(this,"main_menu_bar");
	setMenuWidget(m_pMenuBar);

	if(KVI_OPTION_BOOL(KviOption_boolStatusBarVisible))
	{
		m_pStatusBar = new KviStatusBar(this);
		setStatusBar(m_pStatusBar);
		// torque: moved out of status bar constructor
		// because module init functions exectued in load()
		// couldn't access the status bar via g_pMainWindow->mainStatusBar()
		// (assignment of m_pStatusBar happened after load() and
		// the init function)
		m_pStatusBar->load();

	}
	else
		m_pStatusBar = 0;

	m_pWindowList = 0;

	createWindowList();

	if((KVI_OPTION_RECT(KviOption_rectFrameGeometry).width() < 100) || (KVI_OPTION_RECT(KviOption_rectFrameGeometry).height() < 100) || (KVI_OPTION_RECT(KviOption_rectFrameGeometry).x() > g_pApp->desktop()->width()) || (KVI_OPTION_RECT(KviOption_rectFrameGeometry).y() > g_pApp->desktop()->height()))
	{
		// Try to find some reasonable defaults
		// prefer primary screen for first startup
		int primary_screen = g_pApp->desktop()->primaryScreen();
		QRect r = g_pApp->desktop()->screenGeometry(primary_screen);
		r.setLeft(r.left() + 10);
		r.setRight(r.right() - 100);
		r.setTop(r.top() + 10);
		r.setBottom(r.bottom() - 200);
		KVI_OPTION_RECT(KviOption_rectFrameGeometry) = r;
	}

	resize(KVI_OPTION_RECT(KviOption_rectFrameGeometry).width(),
		KVI_OPTION_RECT(KviOption_rectFrameGeometry).height());
	move(KVI_OPTION_RECT(KviOption_rectFrameGeometry).x(),
		KVI_OPTION_RECT(KviOption_rectFrameGeometry).y());

	if(KVI_OPTION_BOOL(KviOption_boolFrameIsMaximized)) showMaximized();

	applyOptions();

	installAccelerators();
}

KviMainWindow::~KviMainWindow()
{
	KVI_OPTION_BOOL(KviOption_boolFrameIsMaximized) = isMaximized();
	KVI_OPTION_RECT(KviOption_rectFrameGeometry) = QRect(pos().x(),pos().y(),
			size().width(),size().height());

	KVI_OPTION_BOOL(KviOption_boolUseBigIcons) = usesBigPixmaps();
	KVI_OPTION_BOOL(KviOption_boolMdiManagerInSdiMode) = m_pMdi->isInSDIMode();
	KVI_OPTION_BOOL(KviOption_boolStatusBarVisible) = m_pStatusBar ? true : false;

	KviCustomToolBarManager::instance()->storeVisibilityState();

	saveToolBarPositions();
	saveModuleExtensionToolBars();

#if 0
	// Pragma: This shouldn't be needed any more
	//         The objects module must call closeWindow() internally.
	//
	// close all the KviKvsScriptWindowWindow to avoid a race condition
	// where such a window will be free'd two times:
	// the first in the frameDestructorCallback (deleted as an object instance)
	// the second at the end of this function (deleted as a normal window)
	for(KviWindow * wnd = m_pWinList->first();wnd;wnd = m_pWinList->next())
	{
		if(wnd->inherits("KviKvsScriptWindowWindow"))
			closeWindow(wnd);
	}
#endif

	// Call the frame destructor callback AFTER saving the toolbar positions
	// This is because the destructor callback kills alls the KVS objects
	// and thus the eventual user toolbar objects too and their position
	// wouldn't be saved if they are shown at startup.

	g_pApp->frameDestructorCallback();

	// Now start killing stuff

	// Explicitly kill all the module extension toolbars: qt has NOT to delete them: we must call their "die" method
	while(KviMexToolBar * t = m_pModuleExtensionToolBarList->first())t->die();
	delete m_pModuleExtensionToolBarList;

	KVI_OPTION_BOOL(KviOption_boolShowDockExtension) = m_pDockExtension;

	if(m_pDockExtension)
	{
		m_pDockExtension->die();
		m_pDockExtension = 0;
	}

	if(m_pStatusBar)
	{
		delete m_pStatusBar;
		m_pStatusBar = 0;
	}

	//close all not console windows
	for(int i= m_pWinList->count()-1;i>=0;--i)
	{
		KviWindow* wnd=m_pWinList->at(i);
		if(wnd->type() != KviWindow::Console)
			closeWindow(wnd);
	}

	// close all the remaining windows (consoles)
	while(m_pWinList->first())
		closeWindow(m_pWinList->first());

	delete m_pWinList;

	g_pMainWindow = 0;

	g_pApp->quit();
}

void KviMainWindow::registerModuleExtensionToolBar(KviMexToolBar * t)
{
	m_pModuleExtensionToolBarList->append(t);
}

void KviMainWindow::unregisterModuleExtensionToolBar(KviMexToolBar * t)
{
	m_pModuleExtensionToolBarList->removeRef(t);
}

void KviMainWindow::restoreModuleExtensionToolBars()
{
	for(QStringList::Iterator it = KVI_OPTION_STRINGLIST(KviOption_stringlistModuleExtensionToolbars).begin();it != KVI_OPTION_STRINGLIST(KviOption_stringlistModuleExtensionToolbars).end();++it)
	{
		QString szEntry = *it;
		int idx = szEntry.indexOf(':');
		if(idx != -1)
		{
			QString szMod = szEntry.left(idx);
			szEntry.remove(0,idx + 1);
			g_pModuleExtensionManager->allocateExtension("toolbar",KviCString(szEntry),firstConsole(),0,0,szMod);
		}
	}
}

void KviMainWindow::saveModuleExtensionToolBars()
{
	KVI_OPTION_STRINGLIST(KviOption_stringlistModuleExtensionToolbars).clear();

	for(KviMexToolBar * t = m_pModuleExtensionToolBarList->first();t;t = m_pModuleExtensionToolBarList->next())
	{
		QString s = t->descriptor()->module()->name();
		s += ":";
		s += t->descriptor()->name().ptr();

		//qDebug("FOUND TOOLBAR %s",t->descriptor()->name().ptr());

		KVI_OPTION_STRINGLIST(KviOption_stringlistModuleExtensionToolbars).append(s);
	}
}

KviMexToolBar * KviMainWindow::moduleExtensionToolBar(int extensionId)
{
	for(KviMexToolBar * t = m_pModuleExtensionToolBarList->first();t;t = m_pModuleExtensionToolBarList->next())
	{
		if(extensionId == t->descriptor()->id())return t;
	}
	return 0;
}

void KviMainWindow::installAccelerators()
{
	new QShortcut(QKeySequence(KVI_SHORTCUTS_WIN_PREV),this,SLOT(switchToPrevWindow()),0,Qt::ApplicationShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_WIN_NEXT),this,SLOT(switchToNextWindow()),0,Qt::ApplicationShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_WIN_PREV_CONTEXT),this,SLOT(switchToPrevWindowInContext()),0,Qt::ApplicationShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_WIN_NEXT_CONTEXT),this,SLOT(switchToNextWindowInContext()),0,Qt::ApplicationShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_WIN_PREV_HIGHLIGHT),this,SLOT(switchToPrevHighlightedWindow()),0,Qt::ApplicationShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_WIN_NEXT_HIGHLIGHT),this,SLOT(switchToNextHighlightedWindow()),0,Qt::ApplicationShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_WIN_MAXIMIZE),this,SLOT(maximizeWindow()),0,Qt::ApplicationShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_WIN_MINIMIZE),this,SLOT(minimizeWindow()),0,Qt::ApplicationShortcut);

	static int accel_table[] = {
		Qt::Key_1 + Qt::ControlModifier,       // script accels...
		Qt::Key_2 + Qt::ControlModifier,
		Qt::Key_3 + Qt::ControlModifier,
		Qt::Key_4 + Qt::ControlModifier,
		Qt::Key_5 + Qt::ControlModifier,
		Qt::Key_6 + Qt::ControlModifier,
		Qt::Key_7 + Qt::ControlModifier,
		Qt::Key_8 + Qt::ControlModifier,
		Qt::Key_9 + Qt::ControlModifier,
		Qt::Key_0 + Qt::ControlModifier,
		Qt::Key_F2,
		Qt::Key_F3,
		Qt::Key_F4,
		Qt::Key_F5,
		Qt::Key_F6,
		Qt::Key_F7,
		Qt::Key_F8,
		Qt::Key_F9,
		Qt::Key_F10,
		Qt::Key_F11,
		Qt::Key_F12,
		0
	};

	int i=0, keys=0;
	while((keys = accel_table[i]))
	{
		new QShortcut(QKeySequence(keys),this,SLOT(accelActivated()),SLOT(accelActivated()),Qt::ApplicationShortcut);
		i++;
	}
}

void KviMainWindow::accelActivated()
{
	KVS_TRIGGER_EVENT_1(KviEvent_OnAccelKeyPressed,g_pActiveWindow,(QString)(((QShortcut *)sender())->key()));
}

void KviMainWindow::executeInternalCommand(int index)
{
	KviKvsScript::run(kvi_getInternalCommandBuffer(index),firstConsole());
}


void KviMainWindow::saveWindowProperties(KviWindow * wnd,const QString &szSection)
{
	g_pWinPropertiesConfig->setGroup(szSection);
	g_pWinPropertiesConfig->writeEntry("EntryTimestamp",(unsigned int)time(0));

	// Allow max 80 window properties to be floating around
	while(g_pWinPropertiesConfig->groupsCount() > 80)
	{
		// Kill the oldest group
		KviConfigurationFileIterator it(*(g_pWinPropertiesConfig->dict()));
		QString minKey;
		unsigned int minVal = time(0);
		while(it.current() && minVal)
		{
			QString * pVal = it.current()->find("EntryTimestamp");
			if(pVal)
			{
				bool bOk;
				unsigned int uVal = pVal->toUInt(&bOk);
				if(bOk)
				{
					if(uVal < minVal)
					{
						minVal = uVal;
						minKey = it.currentKey();
					}
				} else {
					minVal = 0;
					minKey = it.currentKey();
				}
			} else {
				minVal = 0;
				minKey = it.currentKey();
			}
			++it;
		}

		if(!minKey.isEmpty())g_pWinPropertiesConfig->clearGroup(minKey);
		else qDebug("Oops...no minimum key found!");
	}

	// The following line should NOT be needed...but just to be sure...
	g_pWinPropertiesConfig->setGroup(szSection);

	g_pWinPropertiesConfig->writeEntry("IsDocked",wnd->isDocked());

	if (!wnd->isDocked() && wnd->mdiParent())
	{
		g_pWinPropertiesConfig->writeEntry("IsMaximized",wnd->mdiParent()->isMaximized());
	} else {
		g_pWinPropertiesConfig->writeEntry("IsMaximized",wnd->isMaximized());
	}

	g_pWinPropertiesConfig->writeEntry("WinRect",wnd->externalGeometry());

	wnd->saveProperties(g_pWinPropertiesConfig);
}

void KviMainWindow::closeWindow(KviWindow *wnd)
{
	if (wnd->inherits("KviConsoleWindow"))
	{
		if (consoleCount() <= 1)
		{
			KVS_TRIGGER_EVENT_0(KviEvent_OnFrameWindowDestroyed,wnd);
			KVS_TRIGGER_EVENT_0(KviEvent_OnKVIrcShutdown,wnd);
		}
	}
	// notify the destruction
	wnd->triggerDestructionEvents();

	// save it's properties
	if(KVI_OPTION_BOOL(KviOption_boolWindowsRememberProperties)) // && (wnd->type() == KviWindow::Channel))
	{
		QString group;
		wnd->getConfigGroupName(group);

		// not uses default settings : store it always
		saveWindowProperties(wnd,group);
	}

	// forget it...
	m_pWinList->removeRef(wnd);

	// hide it
	if(wnd->mdiParent())
		m_pMdi->hideChild(wnd->mdiParent());
	else
		wnd->hide();

	if(wnd == g_pActiveWindow)
	{
		if(!g_pApp->kviClosingDown())
		{
			qDebug("Have no active window: trying to pick one");
		
			// this happens when there are only minimized windows in the Mdi area
			// just pick another window to be the active one
			bool bGotIt = false;
			for(KviWindow * pOther = m_pWinList->first();pOther;pOther = m_pWinList->next())
			{
				if(wnd != pOther)
				{
					childWindowActivated(pOther);
					bGotIt = true;
					break;
				}
			}
			
			if(!bGotIt)
			{
				// :/
				g_pActiveWindow = NULL;
				m_pActiveContext = NULL;
			}

		} else {
			g_pActiveWindow = NULL;
			m_pActiveContext = NULL;
		}
	}

	// and shut it down...
	// KviWindow will call childWindowDestroyed() here
	if(wnd->mdiParent())
	{
		//this deletes the wnd, too
		m_pMdi->destroyChild(wnd->mdiParent());
	} else {
		delete wnd;
	}
}

void KviMainWindow::addWindow(KviWindow *wnd,bool bShow)
{
	m_pWinList->append(wnd);
	wnd->createWindowListItem(); // create the window WindowList item AFTER it has been constructed

	QString group;
	wnd->getConfigGroupName(group);

	bool bGroupSettings = false;

	if(g_pWinPropertiesConfig->hasGroup(group))
	{
		g_pWinPropertiesConfig->setGroup(group);
	} else {
		bGroupSettings = true;
		if(g_pWinPropertiesConfig->hasGroup(wnd->typeString()))
		{
			g_pWinPropertiesConfig->setGroup(wnd->typeString());
		} else {
			g_pWinPropertiesConfig->setGroup("no_settings_group");
			wnd->loadProperties(g_pWinPropertiesConfig); // load it anyway (will set defaults if windows don't remember properties)
			goto default_docking; // no settings stored
		}
	}

	{
		wnd->loadProperties(g_pWinPropertiesConfig); // load it anyway (will set defaults if windows don't remember properties)

		if(KVI_OPTION_BOOL(KviOption_boolWindowsRememberProperties))
		{
			bool bDocked    = g_pWinPropertiesConfig->readBoolEntry("IsDocked",true);
			bool bMaximized = g_pWinPropertiesConfig->readBoolEntry("IsMaximized",false);

			QRect rect      = g_pWinPropertiesConfig->readRectEntry("WinRect",QRect(10,10,500,380));

			if(bDocked)
			{
				// when group settings are used, we always cascade the windows
				// this means that windows that have no specialized config group name
				// are always cascaded : this is true for consoles, queries (and other windows) but not channels (and some other windows)
				// FIXME: Since the introduction of QMdiArea cascading (and positioning of windows in general) no longer works
				KviMdiChild * lpC = dockWindow(wnd,bGroupSettings,&rect);
				lpC->setGeometry(rect);
				wnd->triggerCreationEvents();
				if(bShow)
				{
					m_pMdi->showAndActivate(lpC);
					// Handle the special case of this top level widget not being the active one.
					// In this situation the child will not get the focusInEvent
					// and thus will not call out childWindowActivated() method
					if(!isActiveWindow())
						childWindowActivated(wnd);
				}
			} else {
				wnd->setGeometry(rect);
				wnd->triggerCreationEvents();

				if(bShow)
				{
					if(bMaximized)
						wnd->maximize();
				} else {
					wnd->setWindowState(wnd->windowState() | Qt::WindowMinimized);
					if(bMaximized)
						wnd->setWindowState(wnd->windowState() | Qt::WindowMaximized);
				}
				wnd->show();
				wnd->youAreUndocked();
				if(bShow)
				{
					wnd->raise();
					wnd->setFocus();
				}
			}
			goto docking_done;
		}
	}

default_docking:
	{
		KviMdiChild * lpC = dockWindow(wnd);
		// FIXME: Since the introduction of QMdiArea cascading (and positioning of windows in general) no longer works
		// Emulate cascading in some way....
		int offset = (m_pWinList->count() * 10) % 100;
		lpC->setGeometry(QRect(offset,offset,500,400));
		wnd->triggerCreationEvents();
		if(bShow)
		{
			m_pMdi->showAndActivate(lpC);
			// Handle the special case of this top level widget not being the active one.
			// In this situation the child will not get the focusInEvent
			// and thus will not call out childWindowActivated() method
			if(!isActiveWindow())
				childWindowActivated(wnd);
		}
	}
docking_done:
	// we like to have an active window.. but don't trigger the events until it is really shown
	if(!g_pActiveWindow)
	{
		g_pActiveWindow = wnd;
		m_pActiveContext = wnd->context();
	}
}

KviMdiChild * KviMainWindow::dockWindow(KviWindow * wnd, bool bCascade, QRect * setGeom)
{
	if(wnd->mdiParent())return wnd->mdiParent();
	KviMdiChild * lpC = new KviMdiChild(m_pMdi,"");
	lpC->setClient(wnd);
	lpC->setGeometry(wnd->geometry());

	wnd->youAreDocked();
	m_pMdi->manageChild(lpC,bCascade,setGeom);

	return lpC;
}

void KviMainWindow::undockWindow(KviWindow *wnd)
{
	if(!(wnd->mdiParent()))return;
	KviMdiChild * lpC = wnd->mdiParent();
	lpC->unsetClient();
	m_pMdi->destroyChild(lpC);
	wnd->show();
	wnd->youAreUndocked();
	wnd->raise();
	wnd->setFocus();
}


void KviMainWindow::newConsole()
{
	createNewConsole();
}

KviConsoleWindow * KviMainWindow::createNewConsole(bool bFirstInFrame,bool bShowIt)
{
	// the first console must be created BEFORE the toolbars visible
	// at startup otherwise we cannot execute script code
	// which is necessary for the actions that are going to be added
	// to the toolbars
	KviConsoleWindow * c = new KviConsoleWindow(this,bFirstInFrame ? KVI_CONSOLE_FLAG_FIRSTINFRAME : 0);
	addWindow(c,bShowIt);

	if(bFirstInFrame)
	{
		restoreModuleExtensionToolBars();
		KviCustomToolBarManager::instance()->createToolBarsVisibleAtStartup();
		KviActionManager::instance()->delayedRegisterAccelerators();
		restoreToolBarPositions();
	}

	return c;
}

unsigned int KviMainWindow::consoleCount()
{
	unsigned int count = 0;
	for(KviWindow * wnd = m_pWinList->first();wnd;wnd = m_pWinList->next())
	{
		if (wnd) if(wnd->type() == KviWindow::Console) count++;
	}
	return count;
}

KviConsoleWindow * KviMainWindow::firstConsole()
{
	for(KviWindow * wnd = m_pWinList->first();wnd;wnd = m_pWinList->next())
	{
		if(wnd->type() == KviWindow::Console)return (KviConsoleWindow *)wnd;
	}
	KVI_ASSERT(false);
	return 0; //should newer be here!.. but sometimes we are ?
}

KviConsoleWindow * KviMainWindow::firstNotConnectedConsole()
{
	for(KviWindow * wnd = m_pWinList->first();wnd;wnd = m_pWinList->next())
	{
		if(wnd->type() == KviWindow::Console)
		{
			if(!((KviConsoleWindow *)wnd)->connectionInProgress())
				return (KviConsoleWindow *)wnd;
		}
	}
	return 0;
}

void KviMainWindow::childWindowCloseRequest(KviWindow *wnd)
{
	closeWindow(wnd);
}

void KviMainWindow::setActiveWindow(KviWindow *wnd)
{
	// ASSERT(m_pWinList->findRef(wnd))
	if(wnd->isMinimized())
		wnd->restore();
	if(wnd->mdiParent())
		m_pMdi->showAndActivate(wnd->mdiParent());
	else wnd->delayedAutoRaise();
}

KviIrcConnection * KviMainWindow::activeConnection()
{
	return m_pActiveContext ? m_pActiveContext->connection() : 0;
}

void KviMainWindow::childWindowSelectionStateChange(KviWindow * pWnd,bool bGotSelectionNow)
{
	if(pWnd != g_pActiveWindow)return;
	emit activeWindowSelectionStateChanged(bGotSelectionNow);

}

void KviMainWindow::childContextStateChange(KviIrcContext * c)
{
	if(c != m_pActiveContext)return;
	emit activeContextStateChanged();
}

void KviMainWindow::childConnectionLagChange(KviIrcConnection * c)
{
	KviIrcContext * ctx = c->context();
	if(ctx != m_pActiveContext)return;
	emit activeConnectionLagChanged();
}

void KviMainWindow::childConnectionServerInfoChange(KviIrcConnection * c)
{
	KviIrcContext * ctx = c->context();
	if(ctx != m_pActiveContext)return;
	emit activeConnectionServerInfoChanged();
}

void KviMainWindow::childConnectionNickNameChange(KviIrcConnection * c)
{
	KviIrcContext * ctx = c->context();
	if(ctx != m_pActiveContext)return;
	emit activeConnectionNickNameChanged();
}

void KviMainWindow::childConnectionAwayStateChange(KviIrcConnection * c)
{
	KviIrcContext * ctx = c->context();
	if(ctx != m_pActiveContext)return;
	emit activeConnectionAwayStateChanged();
}

void KviMainWindow::childConnectionUserModeChange(KviIrcConnection * c)
{
	KviIrcContext * ctx = c->context();
	if(ctx != m_pActiveContext)return;
	emit activeConnectionUserModeChanged();
}

void KviMainWindow::childWindowActivated(KviWindow *wnd, bool bForce)
{
	// ASSERT(m_pWinList->findRef(wnd))
	// unless we want to bForce the active window to be re-activated
	if(g_pActiveWindow == wnd && !bForce)
		return;

	if(g_pActiveWindow != wnd)
	{
		if(g_pActiveWindow)
			g_pActiveWindow->lostUserFocus();
		g_pActiveWindow = wnd;
	}

	m_pWindowList->setActiveItem(wnd->windowListItem());

	bool bActiveContextChanged = (m_pActiveContext != wnd->context());
	m_pActiveContext = wnd->context();

	if(g_pActiveWindow->view())
		g_pActiveWindow->view()->clearUnreaded();

	emit activeWindowChanged();
	if(bActiveContextChanged)
		emit activeContextChanged();

	KVS_TRIGGER_EVENT_0(KviEvent_OnWindowActivated,wnd);
}

void KviMainWindow::changeEvent(QEvent * e)
{
	if (e->type() == QEvent::ActivationChange)
	{
		//WINDOW (DE)ACTIVATION
		// if we have just been activated by the WM
		// then update the active window task bar item
		// It will then reset its highlight state
		// and hopefully make the dock widget work correctly
		// in this case.
		// This will also trigger the OnWindowActivated event :)
		if(isActiveWindow())
		{
			if(g_pActiveWindow)
				childWindowActivated(g_pActiveWindow, true);
		} else {
			if(g_pActiveWindow)
				g_pActiveWindow->lostUserFocus();
		}
	}
	KviTalMainWindow::changeEvent(e);
}

void KviMainWindow::closeEvent(QCloseEvent *e)
{
	//check if the user just want us to minimize in tray; if we're not the sender
	//of this signal (sender!=0), it has been generated by a "quit" action in a menu
	if(KVI_OPTION_BOOL(KviOption_boolCloseInTray) && QObject::sender()==0 && e->spontaneous())
	{
		if(!dockExtension())
		{
		    executeInternalCommand(KVI_INTERNALCOMMAND_TRAYICON_SHOW);
		}
		if(dockExtension())
		{
			e->ignore();
			KVI_OPTION_BOOL(KviOption_boolFrameIsMaximized) = isMaximized();
			QTimer::singleShot( 0, this, SLOT(hide()) );
		}
		return;
	}

	if(KVI_OPTION_BOOL(KviOption_boolConfirmCloseWhenThereAreConnections))
	{
		// check for running connections

		bool bGotRunningConnection = false;
		for(KviWindow * w = m_pWinList->first();w;w = m_pWinList->next())
		{
			if(w->type() == KviWindow::Console)
			{
				if(((KviConsoleWindow *)w)->connectionInProgress())
				{
					bGotRunningConnection = true;
					break;
				}
			}
		}

		if(bGotRunningConnection)
		{
			QString txt = "<p>";
			txt += __tr2qs("There are active connections, are you sure you wish to ");
			txt += __tr2qs("quit KVIrc?");
			txt += "</p>";

			switch(QMessageBox::warning(this,__tr2qs("Confirmation - KVIrc"),txt,__tr2qs("&Yes"),__tr2qs("&Always"),__tr2qs("&No"),2,2))
			{
				case 0:
					// ok to close
				break;
				case 1:
					// ok to close but don't ask again
					KVI_OPTION_BOOL(KviOption_boolConfirmCloseWhenThereAreConnections) = false;
				break;
				case 2:
					e->ignore();
					return;
				break;
			}
		}
	}

	e->accept();

	if(g_pApp)
		g_pApp->destroyFrame();
}

void KviMainWindow::hideEvent(QHideEvent *e)
{
	if(KVI_OPTION_BOOL(KviOption_boolMinimizeInTray) && e->spontaneous())
	{
		if(!dockExtension())
		{
			executeInternalCommand(KVI_INTERNALCOMMAND_TRAYICON_SHOW);
		}
		if(dockExtension())
		{
			KVI_OPTION_BOOL(KviOption_boolFrameIsMaximized) = isMaximized();
			QTimer::singleShot( 0, this, SLOT(hide()) );
		}
		return;
	}
}

void KviMainWindow::resizeEvent(QResizeEvent *e)
{
	KVI_OPTION_RECT(KviOption_rectFrameGeometry) = QRect(pos().x(),pos().y(),
			size().width(),size().height());
	KviTalMainWindow::resizeEvent(e);
}


void KviMainWindow::contextMenuEvent(QContextMenuEvent *)
{
	// do nothing! avoids builtin popup from qmainwindow
}

void KviMainWindow::updatePseudoTransparency()
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	uint uOpacity = KVI_OPTION_UINT(KviOption_uintGlobalWindowOpacityPercent) < 50 ? 50 : KVI_OPTION_UINT(KviOption_uintGlobalWindowOpacityPercent);
	setWindowOpacity((float) uOpacity / 100);
	if(g_pShadedParentGlobalDesktopBackground)m_pMdi->viewport()->update();

	if(g_pShadedChildGlobalDesktopBackground)
	{
		for(KviWindow * wnd = m_pWinList->first();wnd;wnd = m_pWinList->next())wnd->updateBackgrounds();
		m_pWindowList->updatePseudoTransparency();
	}
#endif
}

void KviMainWindow::moveEvent(QMoveEvent *e)
{
	KVI_OPTION_RECT(KviOption_rectFrameGeometry) = QRect(pos().x(),pos().y(),
			size().width(),size().height());
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	updatePseudoTransparency();
#endif
	KviTalMainWindow::moveEvent(e);
}

void KviMainWindow::applyOptions()
{
	m_pMdi->update();
	for(KviWindow * wnd = m_pWinList->first();wnd;wnd = m_pWinList->next())wnd->applyOptions();

	m_pWindowList->applyOptions();
	g_pTextIconManager->applyOptions();
}

void KviMainWindow::toggleStatusBar()
{
	if(m_pStatusBar)
	{
		delete m_pStatusBar;
		m_pStatusBar = 0;
	} else {
		m_pStatusBar = new KviStatusBar(this);
		m_pStatusBar->load();
		setStatusBar(m_pStatusBar);
		m_pStatusBar->show();
	}
}

void KviMainWindow::fillToolBarsPopup(KviTalPopupMenu * p)
{
	p->clear();

	disconnect(p,SIGNAL(activated(int)),this,SLOT(toolbarsPopupSelected(int))); // just to be sure
	connect(p,SIGNAL(activated(int)),this,SLOT(toolbarsPopupSelected(int)));

	int id;
	int cnt = 0;

	KviModuleExtensionDescriptorList * l = g_pModuleExtensionManager->getExtensionList("toolbar");
	if(l)
	{
		for(KviModuleExtensionDescriptor * d = l->first();d;d = l->next())
		{
			QString label = __tr2qs("Show %1").arg(d->visibleName());
			if(d->icon())id = p->insertItem(*(d->icon()),label);
			else id = p->insertItem(label);
			p->setItemChecked(id,moduleExtensionToolBar(d->id()));
			p->setItemParameter(id,d->id());
			cnt++;
		}
	}

	// FIXME: Should this display "Hide %1" when the toolbar is already visible ?
	KviPointerHashTableIterator<QString,KviCustomToolBarDescriptor> it2(*(KviCustomToolBarManager::instance()->descriptors()));
	if(it2.current())
	{
		if(cnt > 0)
			p->insertSeparator();

		while(KviCustomToolBarDescriptor * d = it2.current())
		{
			QString label = __tr2qs("Show %1").arg(d->label());
			QString ico = d->iconId();
			if(!ico.isEmpty())
			{
				QPixmap * pix = g_pIconManager->getImage(d->iconId());
				if(pix)
				{
					id = p->insertItem(*pix,label);
				} else {
					id = p->insertItem(label);
				}
			} else {
				id = p->insertItem(label);
			}
			p->setItemParameter(id,d->internalId());
			p->setItemChecked(id,d->toolBar());
			++it2;
			cnt++;
		}
	}

	if(cnt > 0)
		p->insertSeparator();

	p->insertItem(
			*(g_pIconManager->getSmallIcon(KviIconManager::ToolBar)),
			__tr2qs("Customize..."),
			this,
			SLOT(customizeToolBars())
		);
}

void KviMainWindow::customizeToolBars()
{
	KviKvsScript::run("toolbareditor.open",g_pActiveWindow);
}

void KviMainWindow::toolbarsPopupSelected(int id)
{
	const QObject * o = sender();
	if(!o)return;
	if(!o->inherits("KviTalPopupMenu"))return;

	int idext = ((KviTalPopupMenu *)o)->itemParameter(id);

	KviCustomToolBarDescriptor * dd = KviCustomToolBarManager::instance()->findDescriptorByInternalId(idext);
	if(dd)
	{
		if(dd->toolBar()) dd->toolBar()->deleteLater();
		else dd->createToolBar();
	}

	if(KviMexToolBar * t = moduleExtensionToolBar(idext))
	{
		t->die();
	} else {
		g_pModuleExtensionManager->allocateExtension("toolbar",idext,firstConsole());
	}
}



bool KviMainWindow::focusNextPrevChild(bool next)
{
	QWidget * w = focusWidget();
	if(w)
	{
		if(w->focusPolicy() == Qt::StrongFocus)return false;
		if(w->parent())
		{
			QVariant v = w->parent()->property("KviProperty_ChildFocusOwner");
			if(v.isValid())return false; // Do NOT change the focus widget!
		}
	}
	return KviTalMainWindow::focusNextPrevChild(next);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Toolbar positioning stuff
////////////////////////////////////////////////////////////////////////////////////////////////////


void KviMainWindow::saveToolBarPositions()
{
	QString szTemp;
	g_pApp->getLocalKvircDirectory(szTemp,KviApplication::Config,KVI_CONFIGFILE_TOOLBARS);

	QFile f(szTemp);
	if(f.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		f.write(saveState(1));
	}
}

void KviMainWindow::restoreToolBarPositions()
{
	QString szTemp;
	g_pApp->getLocalKvircDirectory(szTemp,KviApplication::Config,KVI_CONFIGFILE_TOOLBARS);

	QFile f(szTemp);

	if(f.open(QIODevice::ReadOnly))
	{
		if(!restoreState(f.readAll(),1))
			qDebug("Error while restoring toolbars position");
	}

	if(m_pWindowList->inherits("KviTreeWindowList"))
	{
		// ensure that it is not too wide
		m_pWindowList->setMaximumWidth(600);
		if(m_pWindowList->width() > 600)
			m_pWindowList->setFixedWidth(250);
	}
}

void KviMainWindow::createWindowList()
{
	Qt::DockWidgetArea ePreviousArea = Qt::NoDockWidgetArea;

	if(m_pWindowList)
	{
		ePreviousArea = m_pWindowList->currentDockArea();
		delete m_pWindowList;
	}

	if(KVI_OPTION_BOOL(KviOption_boolUseTreeWindowListWindowList))
	{
		m_pWindowList = new KviTreeWindowList();
		m_pWindowList->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		if((ePreviousArea != Qt::LeftDockWidgetArea) && (ePreviousArea != Qt::RightDockWidgetArea))
			ePreviousArea = Qt::LeftDockWidgetArea;
	} else {
		m_pWindowList = new KviClassicWindowList();
		m_pWindowList->setAllowedAreas(Qt::AllDockWidgetAreas);
		if(ePreviousArea == Qt::NoDockWidgetArea)
			ePreviousArea = Qt::BottomDockWidgetArea;
	}
	addDockWidget(ePreviousArea,m_pWindowList);
}

void KviMainWindow::recreateWindowList()
{
	KviWindow * w;
	for(w = m_pWinList->first();w;w = m_pWinList->next())
	{
		w->destroyWindowListItem();
	}

	createWindowList();

	for(w = m_pWinList->first();w;w = m_pWinList->next())
	{
		w->createWindowListItem();
	}

	if(g_pActiveWindow)
		m_pWindowList->setActiveItem(g_pActiveWindow->windowListItem());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Some accelerators
////////////////////////////////////////////////////////////////////////////////////////////////////

void KviMainWindow::maximizeWindow(void)
{
	if(!g_pActiveWindow)return;
	if(g_pActiveWindow->isMaximized())g_pActiveWindow->restore();
	else g_pActiveWindow->maximize();
}

void KviMainWindow::minimizeWindow(void)
{
	if(g_pActiveWindow)g_pActiveWindow->minimize();
}

void KviMainWindow::switchToPrevWindow(void)
{
	m_pWindowList->switchWindow(false,false);
}

void KviMainWindow::switchToNextWindow(void)
{
	m_pWindowList->switchWindow(true,false);
}

void KviMainWindow::switchToPrevHighlightedWindow(void)
{
	m_pWindowList->switchWindow(false,false,true);
}

void KviMainWindow::switchToNextHighlightedWindow(void)
{
	m_pWindowList->switchWindow(true,false,true);
}

void KviMainWindow::switchToPrevWindowInContext(void)
{
	m_pWindowList->switchWindow(false,true);
}

void KviMainWindow::switchToNextWindowInContext(void)
{
	m_pWindowList->switchWindow(true,true);
}

void KviMainWindow::setUsesBigPixmaps(bool bUse)
{
	KviTalMainWindow::setUsesBigPixmaps(bUse);

	KviPointerListIterator<KviMexToolBar> it(*(m_pModuleExtensionToolBarList));
	if(it.current())
	{
		while(KviMexToolBar * t = it.current())
		{
			t->setUsesBigPixmaps(bUse);
			t->update();
			++it;
		}
	}

	KviPointerHashTableIterator<QString,KviCustomToolBarDescriptor> it2(*(KviCustomToolBarManager::instance()->descriptors()));
	if(it2.current())
	{
		while(KviCustomToolBarDescriptor * d = it2.current())
		{
			if(d->toolBar())
			{
				d->toolBar()->setUsesBigPixmaps(bUse);
				d->toolBar()->update();
			}
			++it2;
		}
	}
}
#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "KviMainWindow.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

