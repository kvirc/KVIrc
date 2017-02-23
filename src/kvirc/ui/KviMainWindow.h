#ifndef _KVI_FRAME_H_
#define _KVI_FRAME_H_
//=============================================================================
//
//   File : KviMainWindow.h
//   Creation date : Sun Jun 18 2000 17:59:02 CEST by Szymon Stefanek
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

//
// KviMainWindow:
//     The main window for the KVIrc application
//

#include "kvi_settings.h"

#include "KviTalMainWindow.h"

#include <algorithm>
#include <list>
#include <unordered_set>
#include <vector>

class KviMenuBar;
class KviWindowStack;
class KviWindow;
class KviConsoleWindow;
class KviWindowListBase;
class QSplitter;
class KviConfigurationFile;
class KviMexToolBar;
class KviIrcContext;
class KviIrcConnection;
class KviStatusBar;
class QMenu;
class KviTrayIcon;
class QShortcut;
class QString;

class KVIRC_API KviMainWindow : public KviTalMainWindow //, public KviIrcContextManager
{
	friend class KviWindow;
	friend class KviConsoleWindow;
	friend class KviApplication;
	friend class KviIrcServerParser;
	friend class KviMexToolBar;
	friend class KviWindowStack;
	friend class KviIrcContext;
	friend class KviIrcConnection;
	friend class KviLagMeter;
	friend class KviUserListView;
	friend class KviUserListViewArea;
	Q_OBJECT
public:
	KviMainWindow(QWidget * pParent);
	~KviMainWindow();

protected:
	// subwindows
	QSplitter * m_pSplitter;                                       // the frame is splitted vertically and thus can host widgets
	KviMenuBar * m_pMenuBar;                                       // the main menu bar
	KviWindowStack * m_pWindowStack;                               // the mdi manager widget (child of the splitter)
	std::unordered_set<KviMexToolBar *> m_pModuleExtensionToolBarList; // the module extension toolbars
	KviWindowListBase * m_pWindowList;                             // the WindowList
	KviStatusBar * m_pStatusBar;
	// the mdi workspace child windows
	std::list<KviWindow *> m_WinList;             // the main list of windows
	KviIrcContext * m_pActiveContext;             // the context of the m_pActiveWindow
	// other
	KviTrayIcon * m_pTrayIcon;                    // the frame's dock extension: this should be prolly moved ?
	std::vector<QShortcut *> m_pAccellerators;    // global application accellerators
public:
	// the mdi manager: handles mdi children
	KviWindowStack * windowStack() { return m_pWindowStack; };
	// the splitter is the central widget for this frame
	QSplitter * splitter() { return m_pSplitter; };
	// KviWindowListBase is the base class for KviTreeWindowList and the KviClassicWindowList
	KviWindowListBase * windowListWidget() { return m_pWindowList; };
	// well.. the menu bar :D
	KviMenuBar * mainMenuBar() { return m_pMenuBar; };
	KviStatusBar * mainStatusBar() { return m_pStatusBar; };
	// this function may return 0 if the active window has no irc context
	KviIrcContext * activeContext() { return m_pActiveContext; };
	// shortcut to a = activeContext(); return a ? a->connection() : 0
	KviIrcConnection * activeConnection();
	// The list of the windows belonging to this frame
	// Note that the windows may be also undocked, but they are still owned by the frame
	std::list<KviWindow *> & windowList() { return m_WinList; };
	// Sets the specified window to be the active one
	// Raises it and focuses it
	void setActiveWindow(KviWindow * wnd);
	// Adds a new KviWindow to this frame
	// This should be done just after the KviWindow constructor has returned
	// If bShow is false then the window is not explicitly shown
	// otherwise it is set as active window.
	void addWindow(KviWindow * wnd, bool bShow = true); // public for modules
	void closeWindow(KviWindow * wnd);
	// Checks if a specified window is still existing in this frame child
	// window list. This is useful for asynchronous functions
	// that keep a window pointer and need to ensure that it is still
	// valid after an uncontrolled delay. (Think of a /timer implementation)
	bool windowExists(KviWindow * wnd) { return (std::find(m_WinList.begin(), m_WinList.end(), wnd) != m_WinList.end()); };
	// The number of consoles in this frame
	unsigned int consoleCount();
	// Creates a new console window. DON'T use the KviConsoleWindow constructor directly.
	// (The script creation events are triggered from here)
	KviConsoleWindow * createNewConsole(bool bFirstInFrame = false, bool bShowIt = true);
	// Returns the first available console.
	// There is almost always an available console.
	// Exceptions are the startup and the shutdown (see activeWindow())
	KviConsoleWindow * firstConsole();
	// Returns the first console that has no connection in progress
	// This function CAN return 0 if all the consoles are connected
	KviConsoleWindow * firstNotConnectedConsole();
	// this is explicitly dedicated to the TrayIcon module
	void setTrayIcon(KviTrayIcon * e) { m_pTrayIcon = e; };
	// returns the dockExtension applet. Useful for calling refresh() when
	// some particular event happens
	KviTrayIcon * trayIcon() { return m_pTrayIcon; };
	// helper for saving the window properties
	void saveWindowProperties(KviWindow * wnd, const QString & szSection);
	// finds the module extension toolbar with the specified identifier
	// see KviModuleExtension.h and KviMexToolBar.h
	KviMexToolBar * moduleExtensionToolBar(int extensionId);
	// Helper to fill the toolbars popup
	// it is used by KviToolBar and KviMenuBar
	void fillToolBarsPopup(QMenu * p);
	void fillIconSizePopup(QMenu * p);
	// Set the size of the icons used by the whole app
	// Allowed sizes are 16, 22, 32 and 48
	void setIconSize(unsigned int uSize);
	void setButtonStyle(unsigned int uStyle);
	// allows scripts and actions to override builtin accellerators, avoiding ambiguous events
	void freeAccelleratorKeySequence(QString & key);
	// called by children windows when they have updated their titles.
	void updateWindowTitle(KviWindow * wnd);
public slots:
	void newConsole();
	void executeInternalCommand(int index);
	void toggleStatusBar();
	void toggleMenuBar();
	void toggleWindowList();
	void customizeToolBars();

protected:
	void restoreModuleExtensionToolBars();
	void saveModuleExtensionToolBars();
	void registerModuleExtensionToolBar(KviMexToolBar * t);
	void unregisterModuleExtensionToolBar(KviMexToolBar * t);

	void createWindowList();
	void recreateWindowList();

	void dockWindow(KviWindow * wnd);
	void undockWindow(KviWindow * wnd);

	// called by KviWindow
	void childWindowCloseRequest(KviWindow * wnd);
	void windowActivated(KviWindow * wnd, bool bForce = false);

	void childContextStateChange(KviIrcContext * c);
	void childConnectionNickNameChange(KviIrcConnection * c);
	void childConnectionAwayStateChange(KviIrcConnection * c);
	void childConnectionUserModeChange(KviIrcConnection * c);
	void childConnectionLagChange(KviIrcConnection * c);
	void childConnectionServerInfoChange(KviIrcConnection * c);
	void childWindowSelectionStateChange(KviWindow * pWnd, bool bGotSelectionNow);

	virtual void closeEvent(QCloseEvent * e);
	virtual void hideEvent(QHideEvent * e);
	virtual void resizeEvent(QResizeEvent * e);
	virtual void moveEvent(QMoveEvent * e);
	virtual bool focusNextPrevChild(bool next);
	virtual void changeEvent(QEvent * event);
	virtual void contextMenuEvent(QContextMenuEvent * event);

	void updatePseudoTransparency();
	void installAccelerators();
protected slots:
	void switchToNextWindow();
	void switchToPrevWindow();
	void switchToNextHighlightedWindow();
	void switchToPrevHighlightedWindow();
	void switchToNextWindowInContext();
	void switchToPrevWindowInContext();
	void closeActiveWindow();

	void accelActivated();
	void toolbarsPopupSelected(QAction * pAction);
	void iconSizePopupSelected(QAction * pAction);
	void buttonStylePopupSelected(QAction * pAction);
signals:
	void activeWindowChanged();       // almost never 0.. but may be
	void activeContextChanged();      // may be 0!
	void activeContextStateChanged(); // emitted only when the active context is non 0 and it changes state
	void activeConnectionNickNameChanged();
	void activeConnectionUserModeChanged();
	void activeConnectionAwayStateChanged();
	void activeConnectionServerInfoChanged();
	void activeConnectionLagChanged();
	void activeWindowSelectionStateChanged(bool bGotSelectionNow);

protected:
	void applyOptions();

private:
	void saveToolBarPositions();
	void restoreToolBarPositions();
};

#ifndef _KVI_FRAME_CPP_
extern KVIRC_API KviMainWindow * g_pMainWindow;
#endif

#endif //_KVI_FRAME_H_
