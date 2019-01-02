//=============================================================================
//
//   File : KviMainWindow.cpp
//   Creation date : Sun Jun 18 2000 17:59:02 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#define _KVI_FRAME_CPP_

#include "kvi_debug.h"
#include "KviApplication.h"
#include "kvi_settings.h"
#include "KviMainWindow.h"
#include "KviMenuBar.h"
#include "KviWindowStack.h"
#include "KviIconManager.h"
#include "KviWindow.h"
#include "KviWindowListBase.h"
#include "KviTreeWindowList.h"
#include "KviConsoleWindow.h"
#include "KviConfigurationFile.h"
#include "KviInternalCommand.h"
#include "KviTrayIcon.h"
#include "KviIrcToolBar.h"
#include "kvi_confignames.h"
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
#include "KviTextIconManager.h"
#include "KviShortcut.h"

#define _WANT_OPTION_FLAGS_
#include "KviOptions.h"

#include <QCheckBox>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QEvent>
#include <QFile>
#include <QLayout>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QShortcut>
#include <QSplitter>
#include <QString>
#include <QTimer>
#include <QVariant>
#include <QWindowStateChangeEvent>

#include <algorithm>
#include <ctime>

#ifdef COMPILE_PSEUDO_TRANSPARENCY
#include <QPixmap>
// KviApplication.h
extern QPixmap * g_pShadedParentGlobalDesktopBackground;
extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

#define KVI_DEFAULT_FRAME_CAPTION "KVIrc " KVI_VERSION " " KVI_RELEASE_NAME

// Declared and managed by KviApplication (KviApplication.cpp)
extern KviConfigurationFile * g_pWinPropertiesConfig;
KVIRC_API KviMainWindow * g_pMainWindow = nullptr; // the one and only frame object

KviMainWindow::KviMainWindow(QWidget * pParent)
    : KviTalMainWindow(pParent, "kvirc_frame")
{
	g_pMainWindow = this;
	setAttribute(Qt::WA_DeleteOnClose);
	setAutoFillBackground(false);
	setAttribute(Qt::WA_TranslucentBackground);
	//disable this flag that gets enabled by qt when using Qt::WA_TranslucentBackground
	setAttribute(Qt::WA_NoSystemBackground, false);
#if !(defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW) || defined(COMPILE_KDE_SUPPORT))
	// Under windows, the icon is builtin; under kde, KApplication sets it for us
	// We try to avois this as much as possible, since it forces the use of the low-res 16x16 icon
	setWindowIcon(*(g_pIconManager->getSmallIcon(KviIconManager::KVIrc)));
#endif

	setWindowTitle(KVI_DEFAULT_FRAME_CAPTION);

	m_pSplitter = new QSplitter(Qt::Horizontal, this);
	m_pSplitter->setObjectName("main_frame_splitter");
	m_pSplitter->setChildrenCollapsible(false);

	setCentralWidget(m_pSplitter);

	setIconSize(KVI_OPTION_UINT(KviOption_uintToolBarIconSize));
	setButtonStyle(KVI_OPTION_UINT(KviOption_uintToolBarButtonStyle));

	m_pWindowStack = new KviWindowStack(m_pSplitter, "mdi_manager");

	// This theoretically had to exists before KviWindowStack (that uses enterSdiMode)
	m_pMenuBar = new KviMenuBar(this, "main_menu_bar");
	setMenuWidget(m_pMenuBar);
#ifndef COMPILE_ON_MAC
	m_pMenuBar->setVisible(KVI_OPTION_BOOL(KviOption_boolMenuBarVisible));
#endif

	if(KVI_OPTION_BOOL(KviOption_boolStatusBarVisible))
	{
		m_pStatusBar = new KviStatusBar(this);
		setStatusBar(m_pStatusBar);
		// torque: moved out of status bar constructor
		// because module init functions executed in load()
		// couldn't access the status bar via g_pMainWindow->mainStatusBar()
		// (assignment of m_pStatusBar happened after load() and
		// the init function)
		m_pStatusBar->load();
	}

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

	if(KVI_OPTION_BOOL(KviOption_boolFrameIsMaximized))
		showMaximized();

	applyOptions();

	installAccelerators();
}

KviMainWindow::~KviMainWindow()
{
	KVI_OPTION_BOOL(KviOption_boolFrameIsMaximized) = isMaximized();
	KVI_OPTION_RECT(KviOption_rectFrameGeometry) = QRect(pos().x(), pos().y(),
	    size().width(), size().height());

	KVI_OPTION_BOOL(KviOption_boolStatusBarVisible) = m_pStatusBar ? true : false;

	KviCustomToolBarManager::instance()->storeVisibilityState();

	saveToolBarPositions();
	saveModuleExtensionToolBars();

	// Call the frame destructor callback AFTER saving the toolbar positions
	// This is because the destructor callback kills alls the KVS objects
	// and thus the eventual user toolbar objects too and their position
	// wouldn't be saved if they are shown at startup.

	g_pApp->frameDestructorCallback();

	// Now start killing stuff

	// Explicitly kill all the module extension toolbars: qt has NOT to delete them: we must call their "die" method
	while(!m_pModuleExtensionToolBarList.empty())
	{
		auto & t = *m_pModuleExtensionToolBarList.begin();
		t->die();
	}

	KVI_OPTION_BOOL(KviOption_boolShowDockExtension) = m_pTrayIcon != nullptr;

	delete m_pTrayIcon;
	m_pTrayIcon = nullptr;

	delete m_pStatusBar;
	m_pStatusBar = nullptr;

	std::vector<KviWindow *> lWinListCopy(m_WinList.begin(), m_WinList.end());
	// Sort the console windows to the end so they are closed last
	std::sort(begin(lWinListCopy), end(lWinListCopy), [](KviWindow * a, KviWindow * b){
		return !a->isConsole() && b->isConsole();
	});

	for(auto & i : lWinListCopy)
		closeWindow(i);

	g_pMainWindow = nullptr;
}

void KviMainWindow::registerModuleExtensionToolBar(KviMexToolBar * t)
{
	m_pModuleExtensionToolBarList.insert(t);
}

void KviMainWindow::unregisterModuleExtensionToolBar(KviMexToolBar * t)
{
	m_pModuleExtensionToolBarList.erase(t);
}

void KviMainWindow::restoreModuleExtensionToolBars()
{
	for(auto szEntry : KVI_OPTION_STRINGLIST(KviOption_stringlistModuleExtensionToolbars))
	{
		int idx = szEntry.indexOf(':');
		if(idx != -1)
		{
			QString szMod = szEntry.left(idx);
			szEntry.remove(0, idx + 1);
			g_pModuleExtensionManager->allocateExtension("toolbar", KviCString(szEntry), firstConsole(), nullptr, nullptr, szMod);
		}
	}
}

void KviMainWindow::saveModuleExtensionToolBars()
{
	KVI_OPTION_STRINGLIST(KviOption_stringlistModuleExtensionToolbars).clear();

	for(auto & t : m_pModuleExtensionToolBarList)
	{
		QString s = t->descriptor()->module()->name();
		s += ':';
		s += t->descriptor()->name().ptr();

		KVI_OPTION_STRINGLIST(KviOption_stringlistModuleExtensionToolbars).append(s);
	}
}

KviMexToolBar * KviMainWindow::moduleExtensionToolBar(int extensionId)
{
	for(auto & t : m_pModuleExtensionToolBarList)
		if(extensionId == t->descriptor()->id())
			return t;

	return nullptr;
}

void KviMainWindow::installAccelerators()
{
	m_pAccellerators.push_back(KviShortcut::create(KVI_SHORTCUTS_WIN_PREV, this, SLOT(switchToPrevWindow()), nullptr, Qt::ApplicationShortcut));
	m_pAccellerators.push_back(KviShortcut::create(KVI_SHORTCUTS_WIN_NEXT, this, SLOT(switchToNextWindow()), nullptr, Qt::ApplicationShortcut));
	m_pAccellerators.push_back(KviShortcut::create(KVI_SHORTCUTS_WIN_PREV_CONTEXT, this, SLOT(switchToPrevWindowInContext()), nullptr, Qt::ApplicationShortcut));
	m_pAccellerators.push_back(KviShortcut::create(KVI_SHORTCUTS_WIN_NEXT_CONTEXT, this, SLOT(switchToNextWindowInContext()), nullptr, Qt::ApplicationShortcut));
	m_pAccellerators.push_back(KviShortcut::create(KVI_SHORTCUTS_WIN_PREV_HIGHLIGHT, this, SLOT(switchToPrevHighlightedWindow()), nullptr, Qt::ApplicationShortcut));
	m_pAccellerators.push_back(KviShortcut::create(KVI_SHORTCUTS_WIN_NEXT_HIGHLIGHT, this, SLOT(switchToNextHighlightedWindow()), nullptr, Qt::ApplicationShortcut));
	m_pAccellerators.push_back(KviShortcut::create(KVI_SHORTCUTS_WIN_CLOSE, this, SLOT(closeActiveWindow()), nullptr, Qt::ApplicationShortcut));
	m_pAccellerators.push_back(KviShortcut::create(KVI_SHORTCUTS_WIN_PREV_TAB, this, SLOT(switchToPrevWindow()), nullptr, Qt::ApplicationShortcut));
	m_pAccellerators.push_back(KviShortcut::create(KVI_SHORTCUTS_WIN_NEXT_TAB, this, SLOT(switchToNextWindow()), nullptr, Qt::ApplicationShortcut));

	static int accel_table[] = {
		Qt::Key_1 + Qt::ControlModifier, // script accels...
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
		Qt::Key_F1 + Qt::ShiftModifier,
		Qt::Key_F2 + Qt::ShiftModifier,
		Qt::Key_F3 + Qt::ShiftModifier,
		Qt::Key_F4 + Qt::ShiftModifier,
		Qt::Key_F5 + Qt::ShiftModifier,
		Qt::Key_F6 + Qt::ShiftModifier,
		Qt::Key_F7 + Qt::ShiftModifier,
		Qt::Key_F8 + Qt::ShiftModifier,
		Qt::Key_F9 + Qt::ShiftModifier,
		Qt::Key_F10 + Qt::ShiftModifier,
		Qt::Key_F11 + Qt::ShiftModifier,
		Qt::Key_F12 + Qt::ShiftModifier
	};

	for(auto key : accel_table)
		m_pAccellerators.push_back(KviShortcut::create(key, this, SLOT(accelActivated()), SLOT(accelActivated()), Qt::ApplicationShortcut));
}

void KviMainWindow::freeAccelleratorKeySequence(const QString & key)
{
	QKeySequence kS(key);
	for(auto & pS : m_pAccellerators)
	{
		if(pS->key() == kS)
		{
			m_pAccellerators.erase(
				std::remove(m_pAccellerators.begin(), m_pAccellerators.end(), pS),
				m_pAccellerators.end()
			);
			return;
		}
	}
}

void KviMainWindow::accelActivated()
{
	KVS_TRIGGER_EVENT_1(KviEvent_OnAccelKeyPressed, g_pActiveWindow, ((qobject_cast<QShortcut *>(sender()))->key()).toString());
}

void KviMainWindow::executeInternalCommand(int index)
{
	KviConsoleWindow * pConsole = nullptr;
	if(activeContext() && activeContext()->console())
		pConsole = activeContext()->console();
	else
		pConsole = firstConsole();
	KviKvsScript::run(kvi_getInternalCommandBuffer(index), pConsole);
}

void KviMainWindow::saveWindowProperties(KviWindow * wnd, const QString & szSection)
{
	g_pWinPropertiesConfig->setGroup(szSection);
	g_pWinPropertiesConfig->writeEntry("EntryTimestamp", (unsigned int)time(nullptr));

	// Allow max 80 window properties to be floating around
	while(g_pWinPropertiesConfig->groupsCount() > 80)
	{
		// Kill the oldest group
		KviConfigurationFileIterator it(*(g_pWinPropertiesConfig->dict()));
		QString minKey;
		unsigned int minVal = time(nullptr);
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
				}
				else
				{
					minVal = 0;
					minKey = it.currentKey();
				}
			}
			else
			{
				minVal = 0;
				minKey = it.currentKey();
			}
			++it;
		}

		if(!minKey.isEmpty())
			g_pWinPropertiesConfig->clearGroup(minKey);
		else
			qDebug("Oops! No minimum key found.");
	}

	// The following line should NOT be needed...but just to be sure...
	g_pWinPropertiesConfig->setGroup(szSection);

	g_pWinPropertiesConfig->writeEntry("IsDocked", wnd->isDocked());

	wnd->saveProperties(g_pWinPropertiesConfig);
}

void KviMainWindow::closeActiveWindow()
{
	if(!g_pActiveWindow)
		return;
	// Don't directly call closeWindow(g_pActiveWindow)
	// as it won't handle last console closing.
	g_pActiveWindow->delayedClose();
}

void KviMainWindow::closeWindow(KviWindow * wnd)
{
	if(wnd->isConsole() && consoleCount() <= 1)
	{
		KVS_TRIGGER_EVENT_0(KviEvent_OnFrameWindowDestroyed, wnd);
		KVS_TRIGGER_EVENT_0(KviEvent_OnKVIrcShutdown, wnd);
	}
	// notify the destruction
	wnd->triggerDestructionEvents();

	// save it's properties
	if(KVI_OPTION_BOOL(KviOption_boolWindowsRememberProperties))
	{
		QString group;
		wnd->getConfigGroupName(group);

		// not uses default settings : store it always
		saveWindowProperties(wnd, group);
	}

	// forget it...
	const auto iter = std::find(m_WinList.begin(), m_WinList.end(), wnd);
	if (iter != m_WinList.end())
		m_WinList.erase(iter);

	if(wnd == g_pActiveWindow)
	{
		if(!g_pApp->kviClosingDown())
		{
			qDebug("Have no active window: trying to pick one");

			// this happens when there are only minimized windows in the Mdi area
			// just pick another window to be the active one
			bool bGotIt = false;
			for(auto & pOther : m_WinList)
			{
				if(wnd != pOther)
				{
					windowActivated(pOther);
					bGotIt = true;
					break;
				}
			}

			if(!bGotIt)
			{
				g_pActiveWindow = nullptr;
				m_pActiveContext = nullptr;
			}
		}
		else
		{
			g_pActiveWindow = nullptr;
			m_pActiveContext = nullptr;
		}
	}

	// and shut it down...
	// KviWindow will call childWindowDestroyed() here
	if(wnd->isDocked())
	{
		//this deletes the wnd, too
		m_pWindowStack->destroyWindow(wnd);
	}
	else
	{
		delete wnd;
	}
}

void KviMainWindow::addWindow(KviWindow * wnd, bool bShow)
{
	m_WinList.push_back(wnd);
	wnd->createWindowListItem(); // create the window WindowList item AFTER it has been constructed

	QString group;
	wnd->getConfigGroupName(group);

	bool bDefaultDocking = false;

	if(g_pWinPropertiesConfig->hasGroup(group))
	{
		g_pWinPropertiesConfig->setGroup(group);
	}
	else if(wnd->isChannel() && g_pWinPropertiesConfig->hasGroup(group = wnd->windowName()))
	{
		// try to load pre-4.2 channel settings
		g_pWinPropertiesConfig->setGroup(group);
	}
	else
	{
		if(g_pWinPropertiesConfig->hasGroup(wnd->typeString()))
		{
			g_pWinPropertiesConfig->setGroup(wnd->typeString());
		}
		else
		{
			g_pWinPropertiesConfig->setGroup("no_settings_group");
			wnd->loadProperties(g_pWinPropertiesConfig); // load it anyway (will set defaults if windows don't remember properties)
			bDefaultDocking = true;                      // no settings stored
		}
	}

	wnd->loadProperties(g_pWinPropertiesConfig); // load it anyway (will set defaults if windows don't remember properties)

	if(KVI_OPTION_BOOL(KviOption_boolWindowsRememberProperties) && !bDefaultDocking)
	{
		bool bDocked = g_pWinPropertiesConfig->readBoolEntry("IsDocked", true);

		if(bDocked)
		{
			// when group settings are used, we always cascade the windows
			// this means that windows that have no specialized config group name
			// are always cascaded : this is true for consoles, queries (and other windows) but not channels (and some other windows)
			// FIXME: Since the introduction of QMdiArea cascading (and positioning of windows in general) no longer works
			dockWindow(wnd);
			wnd->triggerCreationEvents();
			if(bShow)
			{
				m_pWindowStack->showAndActivate(wnd);
				// Handle the special case of this top level widget not being the active one.
				// In this situation the child will not get the focusInEvent
				// and thus will not call out windowActivated() method
				if(!isActiveWindow())
					windowActivated(wnd);
			}
		}
		else
		{
			wnd->triggerCreationEvents();
			if(bShow)
				wnd->show();
			wnd->youAreUndocked();
			if(bShow)
			{
				wnd->raise();
				wnd->setFocus();
			}
		}
	}
	else
	{

		dockWindow(wnd);
		wnd->triggerCreationEvents();
		if(bShow)
		{
			m_pWindowStack->showAndActivate(wnd);
			// Handle the special case of this top level widget not being the active one.
			// In this situation the child will not get the focusInEvent
			// and thus will not call out windowActivated() method
			if(!isActiveWindow())
				windowActivated(wnd);
		}
	}

	// we like to have an active window.. but don't trigger the events until it is really shown
	if(!g_pActiveWindow)
	{
		g_pActiveWindow = wnd;
		m_pActiveContext = wnd->context();
	}
}

void KviMainWindow::dockWindow(KviWindow * wnd)
{
	if(wnd->parentWidget())
		return; // already docked

	m_pWindowStack->addWindow(wnd);
	wnd->youAreDocked();
}

void KviMainWindow::undockWindow(KviWindow * wnd)
{
	if(!(wnd->parentWidget()))
		return;

	m_pWindowStack->removeWidget(wnd);
	wnd->setParent(nullptr);

	wnd->show();
	wnd->youAreUndocked();
	wnd->raise();
	wnd->setFocus();
}

void KviMainWindow::newConsole()
{
	createNewConsole();
}

KviConsoleWindow * KviMainWindow::createNewConsole(bool bFirstInFrame, bool bShowIt)
{
	// the first console must be created BEFORE the toolbars visible
	// at startup otherwise we cannot execute script code
	// which is necessary for the actions that are going to be added
	// to the toolbars
	KviConsoleWindow * c = new KviConsoleWindow(bFirstInFrame ? KVI_CONSOLE_FLAG_FIRSTINFRAME : 0);
	addWindow(c, bShowIt);

	if(bFirstInFrame)
	{
		restoreModuleExtensionToolBars();
		KviCustomToolBarManager::instance()->createToolBarsVisibleAtStartup();
		KviActionManager::instance()->delayedRegisterAccelerators();
		restoreToolBarPositions();
	}

	return c;
}

int KviMainWindow::consoleCount()
{
	return std::count_if(begin(m_WinList), end(m_WinList), [](KviWindow * w){ return w->isConsole(); });
}

KviConsoleWindow * KviMainWindow::firstConsole()
{
	for(auto & wnd : m_WinList)
		if(wnd->isConsole())
			return qobject_cast<KviConsoleWindow *>(wnd);

	// We end up here when we have not console windows.
	// This may happen at early startup or late before shutdown.
	return nullptr;
}

KviConsoleWindow * KviMainWindow::firstNotConnectedConsole()
{
	for(auto & wnd : m_WinList)
	{
		if(wnd->type() == KviWindow::Console)
		{
			if(!qobject_cast<KviConsoleWindow *>(wnd)->connectionInProgress())
				return qobject_cast<KviConsoleWindow *>(wnd);
		}
	}
	return nullptr;
}

void KviMainWindow::childWindowCloseRequest(KviWindow * wnd)
{
	closeWindow(wnd);
}

void KviMainWindow::setActiveWindow(KviWindow * wnd)
{
	m_pWindowStack->showAndActivate(wnd);
}

KviIrcConnection * KviMainWindow::activeConnection()
{
	return m_pActiveContext ? m_pActiveContext->connection() : nullptr;
}

void KviMainWindow::childWindowSelectionStateChange(KviWindow * pWnd, bool bGotSelectionNow)
{
	if(pWnd != g_pActiveWindow)
		return;
	emit activeWindowSelectionStateChanged(bGotSelectionNow);
}

void KviMainWindow::childContextStateChange(KviIrcContext * c)
{
	if(c != m_pActiveContext)
		return;
	emit activeContextStateChanged();
}

void KviMainWindow::childConnectionLagChange(KviIrcConnection * c)
{
	KviIrcContext * ctx = c->context();
	if(ctx != m_pActiveContext)
		return;
	emit activeConnectionLagChanged();
}

void KviMainWindow::childConnectionServerInfoChange(KviIrcConnection * c)
{
	KviIrcContext * ctx = c->context();
	if(ctx != m_pActiveContext)
		return;
	emit activeConnectionServerInfoChanged();
}

void KviMainWindow::childConnectionNickNameChange(KviIrcConnection * c)
{
	KviIrcContext * ctx = c->context();
	if(ctx != m_pActiveContext)
		return;
	emit activeConnectionNickNameChanged();
}

void KviMainWindow::childConnectionAwayStateChange(KviIrcConnection * c)
{
	KviIrcContext * ctx = c->context();
	if(ctx != m_pActiveContext)
		return;
	emit activeConnectionAwayStateChanged();
}

void KviMainWindow::childConnectionUserModeChange(KviIrcConnection * c)
{
	KviIrcContext * ctx = c->context();
	if(ctx != m_pActiveContext)
		return;
	emit activeConnectionUserModeChanged();
}

void KviMainWindow::windowActivated(KviWindow * wnd, bool bForce)
{
	if(!wnd)
		return; // this can happen?

	// unless we want to bForce the active window to be re-activated
	if(g_pActiveWindow == wnd && !bForce)
		return;

	if(g_pActiveWindow != wnd)
	{
		if(g_pActiveWindow)
			g_pActiveWindow->lostUserFocus();
		g_pActiveWindow = wnd;
	}

	updateWindowTitle(wnd);

	m_pWindowList->setActiveItem(wnd->windowListItem());

	bool bActiveContextChanged = (m_pActiveContext != wnd->context());
	m_pActiveContext = wnd->context();

	if(g_pActiveWindow->view())
		g_pActiveWindow->view()->clearUnreaded();

	emit activeWindowChanged();
	if(bActiveContextChanged)
		emit activeContextChanged();

	KVS_TRIGGER_EVENT_0(KviEvent_OnWindowActivated, wnd);
}

void KviMainWindow::changeEvent(QEvent * e)
{
#ifndef COMPILE_ON_MAC
	// For Qt5 this should be used to minimize to tray
	if((e->type() == QEvent::WindowStateChange) && (windowState() & Qt::WindowMinimized) && KVI_OPTION_BOOL(KviOption_boolMinimizeInTray) && e->spontaneous())
	{

		if(!trayIcon())
		{
			executeInternalCommand(KVI_INTERNALCOMMAND_TRAYICON_SHOW);
		}
		if(trayIcon())
		{
			QWindowStateChangeEvent * ev = static_cast<QWindowStateChangeEvent *>(e);
			KVI_OPTION_BOOL(KviOption_boolFrameIsMaximized) = ev->oldState() & Qt::WindowMaximized;
			QTimer::singleShot(0, this, &KviMainWindow::hide);
		}
		return;
	}
#endif
	if(e->type() == QEvent::ActivationChange)
	{
		//WINDOW (DE)ACTIVATION
		// if we have just been activated by the WM
		// then update the active window task bar item
		// It will then reset its highlight state
		// and hopefully make the dock widget work correctly
		// in this case.
		// This will also trigger the OnWindowActivated event :)
		if(g_pActiveWindow)
		{
			if(isActiveWindow())
				windowActivated(g_pActiveWindow, true);
			else
				g_pActiveWindow->lostUserFocus();
		}
	}
	KviTalMainWindow::changeEvent(e);
}

void KviMainWindow::closeEvent(QCloseEvent * e)
{
	//check if the user just want us to minimize in tray; if we're not the sender
	//of this signal (sender!=0), it has been generated by a "quit" action in a menu
	if(KVI_OPTION_BOOL(KviOption_boolCloseInTray) && QObject::sender() == nullptr && e->spontaneous())
	{
		if(!trayIcon())
		{
			executeInternalCommand(KVI_INTERNALCOMMAND_TRAYICON_SHOW);
		}
		if(trayIcon())
		{
			e->ignore();
			KVI_OPTION_BOOL(KviOption_boolFrameIsMaximized) = isMaximized();
			QTimer::singleShot(0, this, &KviMainWindow::hide);
		}
		return;
	}

	if(KVI_OPTION_BOOL(KviOption_boolConfirmCloseWhenThereAreConnections))
	{
		// check for running connections
		bool bGotRunningConnection = false;
		for(auto & w : m_WinList)
		{
			if(w->isConsole() && qobject_cast<KviConsoleWindow *>(w)->connectionInProgress())
			{
				bGotRunningConnection = true;
				break;
			}
		}

		if(bGotRunningConnection)
		{
			QString txt =  __tr2qs("There are active connections, are you sure you wish to quit KVIrc?");

			switch(QMessageBox::warning(this, __tr2qs("Confirm Close - KVIrc"), txt, __tr2qs("&Yes"), __tr2qs("&Always"), __tr2qs("&No"), 2, 2))
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
	{
		g_pApp->setKviClosingDown();
		g_pApp->quit();
	}
}

// For Qt4, see changeEvent method for Qt5
void KviMainWindow::hideEvent(QHideEvent * e)
{
#ifndef COMPILE_ON_MAC
	if(KVI_OPTION_BOOL(KviOption_boolMinimizeInTray) && e->spontaneous())
	{
		if(!trayIcon())
		{
			executeInternalCommand(KVI_INTERNALCOMMAND_TRAYICON_SHOW);
		}
		if(trayIcon())
		{
			KVI_OPTION_BOOL(KviOption_boolFrameIsMaximized) = isMaximized();
			QTimer::singleShot(0, this, &KviMainWindow::hide);
		}
		return;
	}
#endif
}

void KviMainWindow::resizeEvent(QResizeEvent * e)
{
	KVI_OPTION_RECT(KviOption_rectFrameGeometry) = QRect(pos().x(), pos().y(),
	    size().width(), size().height());
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
	setWindowOpacity(uOpacity / 100.f);
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#ifndef Q_WS_EX_LAYERED
#define Q_WS_EX_LAYERED WS_EX_LAYERED
#endif
	if(uOpacity < 100)
	{
		SetWindowLong(reinterpret_cast<HWND>(effectiveWinId()), GWL_EXSTYLE,
		    GetWindowLong(reinterpret_cast<HWND>(effectiveWinId()), GWL_EXSTYLE) | Q_WS_EX_LAYERED);
	}
	else
	{
		SetWindowLong(reinterpret_cast<HWND>(effectiveWinId()), GWL_EXSTYLE,
		    GetWindowLong(reinterpret_cast<HWND>(effectiveWinId()), GWL_EXSTYLE) & ~Q_WS_EX_LAYERED);
	}
#endif
	if(g_pShadedParentGlobalDesktopBackground)
		m_pWindowStack->update();

	if(g_pShadedChildGlobalDesktopBackground)
	{
		for(auto & wnd : m_WinList)
			wnd->updateBackgrounds();
		m_pWindowList->updatePseudoTransparency();
	}
#endif
}

void KviMainWindow::moveEvent(QMoveEvent * e)
{
	KVI_OPTION_RECT(KviOption_rectFrameGeometry) = QRect(pos().x(), pos().y(),
	    size().width(), size().height());
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	updatePseudoTransparency();
#endif
	KviTalMainWindow::moveEvent(e);
}

void KviMainWindow::applyOptions()
{
	m_pWindowStack->update();

	for(auto & wnd : m_WinList)
		wnd->applyOptions();

	m_pWindowList->applyOptions();
	g_pTextIconManager->applyOptions();

#ifndef COMPILE_ON_MAC
	m_pMenuBar->setVisible(KVI_OPTION_BOOL(KviOption_boolMenuBarVisible));
#endif
}

void KviMainWindow::toggleStatusBar()
{
	if(m_pStatusBar)
	{
		delete m_pStatusBar;
		m_pStatusBar = nullptr;
	}
	else
	{
		m_pStatusBar = new KviStatusBar(this);
		m_pStatusBar->load();
		setStatusBar(m_pStatusBar);
		m_pStatusBar->show();
	}
}

void KviMainWindow::toggleMenuBar()
{
	if(KVI_OPTION_BOOL(KviOption_boolMenuBarVisible) == true)
	{
		if(KVI_OPTION_BOOL(KviOption_boolWarnAboutHidingMenuBar))
		{
			// ah, we need an abstraction for this setCheckBox() qt5 shiny stuff
			QMessageBox pMsgBox;
			QCheckBox cb(__tr2qs("Do not show this message again"));
			pMsgBox.setText(__tr2qs("This will hide the menu bar completely. "
			                        "You can show it again by pressing %1.").arg(QString(KVI_SHORTCUTS_TOGGLE_MENU_BAR)));
			pMsgBox.setWindowTitle(__tr2qs("Hide Menu Bar - KVIrc"));
			pMsgBox.setIcon(QMessageBox::Icon::Information);
			pMsgBox.addButton(QMessageBox::Ok);
			pMsgBox.addButton(QMessageBox::Cancel);
			pMsgBox.setDefaultButton(QMessageBox::Ok);
			pMsgBox.setCheckBox(&cb);
			if(pMsgBox.exec() == QMessageBox::Ok)
			{
				if(cb.isChecked())
				{
					KVI_OPTION_BOOL(KviOption_boolWarnAboutHidingMenuBar) = false;
				}
			}
			else
			{
				return;
			}
		}
		m_pMenuBar->hide();
		KVI_OPTION_BOOL(KviOption_boolMenuBarVisible) = false;
	}
	else
	{
		m_pMenuBar->show();
		KVI_OPTION_BOOL(KviOption_boolMenuBarVisible) = true;
	}
}

void KviMainWindow::toggleWindowList()
{
	KVI_OPTION_BOOL(KviOption_boolUseTreeWindowList) = !KVI_OPTION_BOOL(KviOption_boolUseTreeWindowList);
	recreateWindowList();
	applyOptions();
}

void KviMainWindow::fillToolBarsPopup(QMenu * p)
{
	p->clear();

	disconnect(p, &QMenu::triggered, this, &KviMainWindow::toolbarsPopupSelected); // just to be sure
	connect(p, &QMenu::triggered, this, &KviMainWindow::toolbarsPopupSelected);

	int cnt = 0;

	KviModuleExtensionDescriptorList * l = g_pModuleExtensionManager->getExtensionList("toolbar");
	if(l)
	{
		for(KviModuleExtensionDescriptor * d = l->first(); d; d = l->next())
		{
			QString label = __tr2qs("Show %1").arg(d->visibleName());
			QAction * pAction;
			if(d->icon())
				pAction = p->addAction(*(d->icon()), label);
			else
				pAction = p->addAction(label);
			pAction->setCheckable(true);
			pAction->setChecked(moduleExtensionToolBar(d->id()));
			pAction->setData(d->id());
			cnt++;
		}
	}

	// FIXME: Should this display "Hide %1" when the toolbar is already visible ?
	KviPointerHashTableIterator<QString, KviCustomToolBarDescriptor> it2(*(KviCustomToolBarManager::instance()->descriptors()));
	if(it2.current())
	{
		if(cnt > 0)
			p->addSeparator();

		while(KviCustomToolBarDescriptor * d = it2.current())
		{
			QString label = __tr2qs("Show %1").arg(d->label());
			QString ico = d->iconId();
			QAction * pAction;
			if(!ico.isEmpty())
			{
				QPixmap * pix = g_pIconManager->getImage(d->iconId());
				if(pix)
					pAction = p->addAction(*pix, label);
				else
					pAction = p->addAction(label);
			}
			else
			{
				pAction = p->addAction(label);
			}
			pAction->setData(d->internalId());
			pAction->setCheckable(true);
			pAction->setChecked(d->toolBar());
			++it2;
			cnt++;
		}
	}

	if(cnt > 0)
		p->addSeparator();

	p->addAction(
	    *(g_pIconManager->getSmallIcon(KviIconManager::ToolBarEditor)),
	    __tr2qs("Customize..."),
	    this,
	    SLOT(customizeToolBars()));
}

void KviMainWindow::customizeToolBars()
{
	KviKvsScript::run("toolbareditor.open -t", g_pActiveWindow);
}

void KviMainWindow::toolbarsPopupSelected(QAction * pAction)
{
	bool bOk;
	int idext = pAction->data().toInt(&bOk);
	if(!bOk)
		return;

	KviCustomToolBarDescriptor * dd = KviCustomToolBarManager::instance()->findDescriptorByInternalId(idext);
	if(dd)
	{
		if(dd->toolBar())
			dd->toolBar()->deleteLater();
		else
			dd->createToolBar();
	}

	if(KviMexToolBar * t = moduleExtensionToolBar(idext))
		t->die();
	else
		g_pModuleExtensionManager->allocateExtension("toolbar", idext, firstConsole());
}

void KviMainWindow::iconSizePopupSelected(QAction * pAction)
{
	if(!pAction)
		return;

	bool bOk;
	uint uSize = pAction->data().toUInt(&bOk);
	if(!bOk)
		return;

	setIconSize(uSize);
	g_pApp->optionResetUpdate(KviOption_resetReloadImages);
}

void KviMainWindow::buttonStylePopupSelected(QAction * pAction)
{
	if(!pAction)
		return;

	bool bOk;
	uint uStyle = pAction->data().toUInt(&bOk);
	if(!bOk)
		return;

	setButtonStyle(uStyle);
}

bool KviMainWindow::focusNextPrevChild(bool next)
{
	if(QWidget * w = focusWidget(); w)
	{
		if(w->focusPolicy() == Qt::StrongFocus)
			return false;
		if(w->parent())
		{
			QVariant v = w->parent()->property("KviProperty_ChildFocusOwner");
			if(v.isValid())
				return false; // Do NOT change the focus widget!
		}
	}
	return KviTalMainWindow::focusNextPrevChild(next);
}

void KviMainWindow::updateWindowTitle(KviWindow * wnd)
{
	if(g_pActiveWindow == wnd)
	{
		QString szCaption = QString("%1 - %2").arg(wnd->windowTitle()).arg(KVI_DEFAULT_FRAME_CAPTION);
		setWindowTitle(szCaption);
	}
}

//
// Toolbar positioning stuff
//

void KviMainWindow::saveToolBarPositions()
{
	QString szTemp;
	g_pApp->getLocalKvircDirectory(szTemp, KviApplication::Config, KVI_CONFIGFILE_TOOLBARS);

	QFile f(szTemp);
	if(f.open(QIODevice::WriteOnly | QIODevice::Truncate))
		f.write(saveState(1));
}

void KviMainWindow::restoreToolBarPositions()
{
	QString szTemp;
	g_pApp->getLocalKvircDirectory(szTemp, KviApplication::Config, KVI_CONFIGFILE_TOOLBARS);

	QFile f(szTemp);

	if(f.open(QIODevice::ReadOnly))
	{
		if(!restoreState(f.readAll(), 1))
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

	if(KVI_OPTION_BOOL(KviOption_boolUseTreeWindowList))
	{
		m_pWindowList = new KviTreeWindowList();
		m_pWindowList->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		if((ePreviousArea != Qt::LeftDockWidgetArea) && (ePreviousArea != Qt::RightDockWidgetArea))
			ePreviousArea = Qt::LeftDockWidgetArea;
	}
	else
	{
		m_pWindowList = new KviClassicWindowList();
		m_pWindowList->setAllowedAreas(Qt::AllDockWidgetAreas);
		if(ePreviousArea == Qt::NoDockWidgetArea || ePreviousArea == Qt::LeftDockWidgetArea || ePreviousArea == Qt::RightDockWidgetArea)
			ePreviousArea = Qt::BottomDockWidgetArea;
	}
	addDockWidget(ePreviousArea, m_pWindowList);
}

void KviMainWindow::recreateWindowList()
{
	for(auto & w : m_WinList)
		w->destroyWindowListItem();

	createWindowList();

	for(auto & w : m_WinList)
		w->createWindowListItem();

	if(g_pActiveWindow)
		m_pWindowList->setActiveItem(g_pActiveWindow->windowListItem());
}

//
// Some accelerators
//

void KviMainWindow::switchToPrevWindow()
{
	m_pWindowList->switchWindow(false, false);
}

void KviMainWindow::switchToNextWindow()
{
	m_pWindowList->switchWindow(true, false);
}

void KviMainWindow::switchToPrevHighlightedWindow()
{
	m_pWindowList->switchWindow(false, false, true);
}

void KviMainWindow::switchToNextHighlightedWindow()
{
	m_pWindowList->switchWindow(true, false, true);
}

void KviMainWindow::switchToPrevWindowInContext()
{
	m_pWindowList->switchWindow(false, true);
}

void KviMainWindow::switchToNextWindowInContext()
{
	m_pWindowList->switchWindow(true, true);
}

void KviMainWindow::setIconSize(unsigned int uSize)
{
	if((uSize != 16) && (uSize != 22) && (uSize != 32) && (uSize != 48))
		uSize = 22;

	KVI_OPTION_UINT(KviOption_uintToolBarIconSize) = uSize;

	QSize sSize(uSize, uSize);

	KviTalMainWindow::setIconSize(sSize);

	for(auto & t : m_pModuleExtensionToolBarList)
	{
		t->setIconSize(sSize);
		t->update();
	}

	KviPointerHashTableIterator<QString, KviCustomToolBarDescriptor> it2(*(KviCustomToolBarManager::instance()->descriptors()));
	if(it2.current())
	{
		while(KviCustomToolBarDescriptor * d = it2.current())
		{
			if(d->toolBar())
			{
				d->toolBar()->setIconSize(sSize);
				d->toolBar()->update();
			}
			++it2;
		}
	}
}

void KviMainWindow::setButtonStyle(unsigned int uStyle)
{

	KVI_OPTION_UINT(KviOption_uintToolBarButtonStyle) = uStyle;

	for(auto & t : m_pModuleExtensionToolBarList)
	{
		t->setToolButtonStyle((Qt::ToolButtonStyle)uStyle);
		t->update();
	}

	KviPointerHashTableIterator<QString, KviCustomToolBarDescriptor> it2(*(KviCustomToolBarManager::instance()->descriptors()));
	if(it2.current())
	{
		while(KviCustomToolBarDescriptor * d = it2.current())
		{
			if(d->toolBar())
			{
				d->toolBar()->setToolButtonStyle((Qt::ToolButtonStyle)uStyle);
				d->toolBar()->update();
			}
			++it2;
		}
	}
}
