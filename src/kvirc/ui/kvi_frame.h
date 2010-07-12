#ifndef _KVI_FRAME_H_
#define _KVI_FRAME_H_
//=============================================================================
//
//   File : kvi_frame.h
//   Creation date : Sun Jun 18 2000 17:59:02 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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

//=============================================================================
//
// KviFrame:
//     The main window for the KVIrc application
//
//=============================================================================

#include "kvi_settings.h"

#include "kvi_tal_mainwindow.h"
#include "kvi_qstring.h"
#include "kvi_pointerlist.h"
#include "kvi_mdimanager.h"

class KviMenuBar;
class KviMdiChild;
class KviWindow;
class KviConsole;
class KviWindowListBase;
class QSplitter;
class KviConfig;
class KviMexToolBar;
class KviIrcContext;
class KviIrcConnection;
class KviStatusBar;
class KviTalPopupMenu;

#ifdef COMPILE_ON_WINDOWS
	// MSCV has problems with KviPointerList<KviWindow> otherwise
	#include "kvi_window.h"
#endif

// base class for the dock extension applets..
// this should be probably moved out of here
class KVIRC_API KviDockExtension
{
protected:
	Qt::WindowStates m_uStoredWindowState;
public:
	KviDockExtension() : m_uStoredWindowState(0){};
	virtual ~KviDockExtension(){};
public:
	void setPrevWindowState(Qt::WindowStates state) { m_uStoredWindowState = state; };
	Qt::WindowStates getPrevWindowState() { return m_uStoredWindowState; };

	virtual void refresh(){};
	virtual void die(){ delete this; };
};



class KVIRC_API KviFrame : public KviTalMainWindow // , public KviIrcContextManager
{
	friend class KviWindow;
	friend class KviConsole;
	friend class KviApp;
	friend class KviServerParser;
	friend class KviMexToolBar;
	friend class KviMdiManager;
	friend class KviIrcContext;
	friend class KviIrcConnection;
	friend class KviLagMeter;
	friend class KviUserListView;
	friend class KviUserListViewArea;
	Q_OBJECT
public:
	KviFrame();
	~KviFrame();
protected:
	// subwindows
	QSplitter                             * m_pSplitter;                     // the frame is splitted vertically and thus can host widgets
	KviMenuBar                            * m_pMenuBar;                      // the main menu bar
	KviMdiManager                         * m_pMdi;                          // the mdi manager widget (child of the splitter)
	KviPointerList<KviMexToolBar>         * m_pModuleExtensionToolBarList;   // the module extension toolbars
	KviWindowListBase                        * m_pWindowList;                      // the WindowList
	KviStatusBar                          * m_pStatusBar;
	// the mdi workspace child windows
	KviPointerList<KviWindow>             * m_pWinList;                      // the main list of windows
	KviIrcContext                         * m_pActiveContext;                // the context of the m_pActiveWindow
	// other
	KviDockExtension                      * m_pDockExtension;                // the frame's dock extension: this should be prolly moved ?
public:
	// the mdi manager: handles mdi children
	KviMdiManager * mdiManager(){ return m_pMdi; };
	// the splitter is the central widget for this frame
	QSplitter * splitter(){ return m_pSplitter; };
	// KviWindowListBase is the base class for KviTreeWindowList and the KviClassicWindowList
	KviWindowListBase * windowListWidget(){ return m_pWindowList; };
	// well.. the menu bar :D
	KviMenuBar * mainMenuBar(){ return m_pMenuBar; };
	KviStatusBar * mainStatusBar(){ return m_pStatusBar; };
	// this function may return 0 if the active window has no irc context
	KviIrcContext * activeContext(){ return m_pActiveContext; };
	// shortcut to a = activeContext(); return a ? a->connection() : 0
	KviIrcConnection * activeConnection();
	// The list of the windows belonging to this frame
	// Note that the windows may be also undocked, but they are still owned by the frame
	KviPointerList<KviWindow> * windowList() { return m_pWinList; };
	// Sets the specified window to be the active one
	// Raises it and focuses it
	void setActiveWindow(KviWindow *wnd);
	// Adds a new KviWindow to this frame
	// This should be done just after the KviWindow constructor has returned
	// If bShow is false then the window is not explicitly shown
	// otherwise it is set as active window.
	void addWindow(KviWindow *wnd,bool bShow = true); // public for modules
	void closeWindow(KviWindow *wnd);
	// Checks if a specified window is still existing in this frame child
	// window list. This is useful for asynchronous functions
	// that keep a window pointer and need to ensure that it is still
	// valid after an uncontrolled delay. (Think of a /timer implementation)
	bool windowExists(KviWindow * wnd){ return (m_pWinList->findRef(wnd) != -1); };
	// The number of consoles in this frame
	unsigned int consoleCount();
	// Creates a new console window. DON'T use the KviConsole constructor directly.
	// (The script creation events are triggered from here)
	KviConsole * createNewConsole(bool bFirstInFrame = false);
	// Returns the first available console.
	// There is almost always an available console.
	// Exceptions are the startup and the shutdown (see activeWindow())
	KviConsole * firstConsole();
	// Returns the first console that has no connection in progress
	// This function CAN return 0 if all the consoles are connected
	KviConsole * firstNotConnectedConsole();
	// this is explicitly dedicated to the DockExtension applets
	void setDockExtension(KviDockExtension * e){ m_pDockExtension = e; };
	// returns the dockExtension applet. Useful for calling refresh() when
	// some particular event happens
	KviDockExtension * dockExtension(){ return m_pDockExtension; };
	// helper for saving the window properties
	void saveWindowProperties(KviWindow * wnd,const QString &szSection);
	// finds the module extension toolbar with the specified identifier
	// see kvi_moduleextension.h and kvi_mextoolbar.h
	KviMexToolBar * moduleExtensionToolBar(int extensionId);
	// Helper to fill the toolbars popup
	// it is used by KviToolBar and KviMenuBar
	void fillToolBarsPopup(KviTalPopupMenu * p);
	/**
	* \brief Sets the use of big pixmaps in toolbar
	* \param bUse Whether to use big pixmaps in toolbars
	* \return void
	*/
	void setUsesBigPixmaps(bool bUse);
	
public slots:
	void newConsole();
	void executeInternalCommand(int index);
	void toggleStatusBar();
	void customizeToolBars();
protected:
	void restoreModuleExtensionToolBars();
	void saveModuleExtensionToolBars();
	void registerModuleExtensionToolBar(KviMexToolBar * t);
	void unregisterModuleExtensionToolBar(KviMexToolBar * t);

	void unhighlightWindowsOfContext(KviIrcContext * c);

	void createWindowList();
	void recreateWindowList();

	KviMdiChild * dockWindow(KviWindow *wnd,bool bCascade = true,QRect * setGeom = 0);
	void undockWindow(KviWindow *wnd);

	// called by KviWindow
	void childWindowCloseRequest(KviWindow *wnd);
	void childWindowActivated(KviWindow *wnd, bool bForce = false);

	void childContextStateChange(KviIrcContext * c);
	void childConnectionNickNameChange(KviIrcConnection * c);
	void childConnectionAwayStateChange(KviIrcConnection * c);
	void childConnectionUserModeChange(KviIrcConnection * c);
	void childConnectionLagChange(KviIrcConnection * c);
	void childConnectionServerInfoChange(KviIrcConnection * c);
	void childWindowSelectionStateChange(KviWindow * pWnd,bool bGotSelectionNow);

	virtual void closeEvent(QCloseEvent *e);
	virtual void resizeEvent(QResizeEvent *e);
	virtual void moveEvent(QMoveEvent *e);
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

	void maximizeWindow();
	void minimizeWindow();

	void accelActivated();
	void toolbarsPopupSelected(int id);
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
	void signalMaximizeMdiChildWindow(KviMdiChild * lpC);
protected:
	void applyOptions();
private:
	void saveToolBarPositions();
	void restoreToolBarPositions();
};

#ifndef _KVI_FRAME_CPP_
	extern KVIRC_API KviFrame * g_pFrame;
#endif

#endif //_KVI_FRAME_H_
