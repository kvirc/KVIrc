//=============================================================================
//
//   File : KviApplication.cpp
//   Creation date : Sun Jun 18 2000 12:39:45 by Szymon Stefanek
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

#define _KVI_APP_CPP_

#include "kvi_socket.h"
#include "KviApplication.h"
#include "kvi_debug.h"
#include "KviMainWindow.h"
#include "KviMessageBox.h"
#include "KviIconManager.h"
#include "KviInput.h"
#include "KviInputHistory.h"
#include "KviConfigurationFile.h"
#include "KviColorSelectionWindow.h"
#include "KviWindow.h"
#include "KviIrcServerDataBase.h"
#include "KviProxyDataBase.h"
#include "KviMenuBar.h"
#include "KviOptions.h"
#include "KviIrcServerParser.h"
#include "KviModuleManager.h"
#include "KviMediaManager.h"
#include "KviRegisteredUserDataBase.h"
#include "KviThread.h"
#include "KviSharedFilesManager.h"
#include "kvi_confignames.h"
#include "KviWindowListBase.h"
#include "kvi_defaults.h"
#include "KviLocale.h"
#include "kvi_out.h"
#include "KviNickServRuleSet.h"
#include "KviIdentityProfileSet.h"
#include "KviDefaultScript.h"
#include "KviXlib.h"
#include "KviTextIconManager.h"
#include "KviTextIconWindow.h"
#include "KviHistoryWindow.h"
#include "KviCtcpPageDialog.h"
#include "KviRegisteredChannelDataBase.h"
#include "KviModuleExtension.h"
#include "KviInternalCommand.h"
#include "KviFileTransfer.h"
#include "KviControlCodes.h"
#include "KviIrcUrl.h"
#include "KviAvatarCache.h"
#include "KviActionManager.h"
#include "KviCustomToolBarManager.h"
#include "KviFileUtils.h"
#include "KviTimeUtils.h"
#include "KviStringConversion.h"
#include "KviUserIdentityManager.h"
#include "KviIrcView.h"
#include "KviEnvironment.h"
#include "KviAnimatedPixmapCache.h"
#include "KviKvs.h"
#include "KviKvsScript.h"
#include "KviKvsPopupManager.h"
#include "KviKvsKernel.h"
#include "KviKvsObjectController.h"
#include "KviKvsEventTriggers.h"
#include "kvi_sourcesdate.h"
#include "KviPointerHashTable.h"
#include "KviQueryWindow.h"
#include "KviCaster.h"
#include "KviSignalHandler.h"
#include "KviPtrListIterator.h"
#include "KviIrcNetwork.h"
#include "KviRuntimeInfo.h"

#include <QMenu>
#include <QPainter>
#include <algorithm>

#ifndef COMPILE_NO_IPC
#include "KviIpcSentinel.h"
#endif

#ifdef COMPILE_CRYPT_SUPPORT
#include "KviCryptEngineManager.h"
#endif

#ifdef COMPILE_SSL_SUPPORT
#include "KviSSL.h"
#endif

#include <QFileInfo>
#include <QSplitter>
#include <QClipboard>
#include <QMessageBox>
#include <QTextCodec>
#include <QMetaObject>
#include <QCommonStyle>

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#include <QPluginLoader>
#include <QtWin>
#endif

#ifdef COMPILE_DBUS_SUPPORT
#include <QDBusInterface>
#endif

#ifdef COMPILE_KDE_SUPPORT
#ifdef COMPILE_KDE4_SUPPORT
#include <KStandardDirs>
#else
#include <QStandardPaths>
#endif
#include <KNotification>
#endif

#ifdef COMPILE_QX11INFO_SUPPORT
#include <QX11Info>
#endif

/*
HACK These 2 hacks are defined because X11 defines Unsorted and None
which conflicts with QDir and KviApplication::KvircSubdir
DO NOT REMOVE THEM EVEN IF THEY ARE DEFINED ALSO IN KviApplication.h
*/
#ifdef Unsorted
#undef Unsorted
#endif

#ifdef None
#undef None
#endif

#include <QDir>

#include <cstdlib>  // rand & srand
#include <ctime>    // time() in srand()
#include <map>      // std::map<>

// Global application pointer
KVIRC_API KviApplication * g_pApp = nullptr;

KviConfigurationFile * g_pWinPropertiesConfig = nullptr;
KVIRC_API KviIrcServerDataBase * g_pServerDataBase = nullptr;
KVIRC_API KviProxyDataBase * g_pProxyDataBase = nullptr;

// Global windows
KVIRC_API KviColorWindow * g_pColorWindow = nullptr;
KVIRC_API KviTextIconWindow * g_pTextIconWindow = nullptr;
KVIRC_API QMenu * g_pInputPopup = nullptr;
KVIRC_API QStringList * g_pRecentTopicList = nullptr;
KVIRC_API std::map<QString, KviWindow *> g_pGlobalWindowDict;
KVIRC_API KviMediaManager * g_pMediaManager = nullptr;
KVIRC_API KviSharedFilesManager * g_pSharedFilesManager = nullptr;
KVIRC_API KviNickServRuleSet * g_pNickServRuleSet = nullptr;
KVIRC_API KviCtcpPageDialog * g_pCtcpPageDialog = nullptr;
KVIRC_API KviRegisteredChannelDataBase * g_pRegisteredChannelDataBase = nullptr;
KVIRC_API KviHistoryWindowWidget * g_pHistoryWindow = nullptr;

// this is eventually set by libkviident
KVIRC_API int g_iIdentDaemonRunningUsers = 0;

// Loaded and destroyed by KviIconManager
QPixmap * g_pUserChanStatePixmap = nullptr;
QPixmap * g_pActivityMeterPixmap = nullptr;

#ifdef COMPILE_PSEUDO_TRANSPARENCY

#include <QImage>

KVIRC_API QPixmap * g_pShadedParentGlobalDesktopBackground = nullptr; // the pixmap that we use for MdiManager
KVIRC_API QPixmap * g_pShadedChildGlobalDesktopBackground = nullptr;  // the pixmap that we use for MdiChild
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#include <winuser.h>
#include <QDesktopWidget>
#endif
#endif

#ifdef COMPILE_CRYPT_SUPPORT
#include "KviCryptEngine.h"
// global crypt engine manager
KVIRC_API KviCryptEngineManager * g_pCryptEngineManager = nullptr;
#endif

#include <QStyleFactory>

KviApplication::KviApplication(int & argc, char ** argv)
    : KviTalApplication(argc, argv)
{
	setApplicationName("KVIrc");
	setApplicationVersion(KVIRC_VERSION_RELEASE);
	setOrganizationDomain("kvirc.net");
	setOrganizationName("KVIrc");

	// Ok...everything begins here
	g_pApp = this;
	m_szConfigFile = QString();
	m_bCreateConfig = false;
	m_bUpdateGuiPending = false;
	m_pRecentChannelDict = nullptr;
#ifndef COMPILE_NO_IPC
	m_pIpcSentinel = nullptr;
#endif
	m_iHeartbeatTimerId = -1;
	m_fntDefaultFont = font();
	m_bSetupDone = false;
	m_bClosingDown = false;
	kvi_socket_flushTrafficCounters();
	// don't let qt quit the application by itself
	setQuitOnLastWindowClosed(false);
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	m_bPortable = KviFileUtils::fileExists(g_pApp->applicationDirPath() + KVI_PATH_SEPARATOR_CHAR + "portable");
#endif

//note: the early qApp->style() call leads to a crash on osx
#if !defined(COMPILE_ENABLE_GTKSTYLE) && !defined(COMPILE_ON_MAC)
	// workaround for gtk+ style forcing a crappy white background (ticket #777, #964, #1009, ..)
	if(QString("QGtkStyle").compare(qApp->style()->metaObject()->className()) == 0)
	{
		setStyle(QStyleFactory::create("Fusion"));
		setPalette(style()->standardPalette());
	}
#endif
}

void KviApplication::setup()
{
	// Here we start up all the substystems.
	// This is a really critical phase since in general subsystems depend
	// on each other and we must activate them in the right order.
	// Don't move stuff around unless you really know what you're doing.

	// Initialize the random number generator
	::srand(::time(nullptr));

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	// setup winsock.dll
	WSADATA w;
	WSAStartup(MAKELONG((unsigned short)2, (unsigned short)0), &w);
#endif

#ifndef COMPILE_NO_IPC
	// Create this very early
	// FIXME: this is still not early enough... we actually HAVE race conditions (should use a file for locking this ?)
	createIpcSentinel();
#endif

	// Initialize the thread manager early
	KviThreadManager::globalInit();

#ifdef COMPILE_SSL_SUPPORT
	KviSSL::globalInit();
#endif

	::kvi_signalHandlerSetup();

	// Setup our filesystem and initialize locale
	loadDirectories();
	KviStringConversion::init(m_szGlobalKvircDir, m_szLocalKvircDir);

	g_pIconManager = new KviIconManager();

	// add KVIrc common dirs to QT searchpath
	QString szPath;
	getLocalKvircDirectory(szPath, None, "avatars");
	QDir::addSearchPath("avatars", szPath);
	getLocalKvircDirectory(szPath, None, "pics");
	QDir::addSearchPath("pics", szPath);
	getLocalKvircDirectory(szPath, None, "audio");
	QDir::addSearchPath("audio", szPath);

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	// need to load image plugins :(
	QString szPluginsDir;
	getGlobalKvircDirectory(szPluginsDir, None, "qt-plugins/");
	addLibraryPath(szPluginsDir);
//MessageBox(0,QApplication::libraryPaths().join(";").toLocal8Bit().data(),"KVIrc",0);
//KviMessageBox::information(libraryPaths().join(";"));
//qDebug("%1",loader.isLoaded());
#endif

	// Make sure that the C strings are translated to utf8
	// This is a fallback solution anyway: we should use the appropriate
	// encoding every time.
	QTextCodec * pUTF8Codec = KviLocale::instance()->codecForName("UTF-8");
	if(pUTF8Codec)
		QTextCodec::setCodecForLocale(pUTF8Codec);
	else
		qDebug("Aaargh... have no UTF-8 codec?");

	QString szTmp;

	// Initialize the scripting engine
	KviKvs::init();

	// Initialize the action manager
	KviActionManager::init();

	// Create the module manager early: so the other managers can load modules
	g_pModuleExtensionManager = new KviModuleExtensionManager();

	g_pModuleManager = new KviModuleManager();

	if(getReadOnlyConfigPath(szTmp, KVI_CONFIGFILE_USERACTIONS))
		KviActionManager::instance()->load(szTmp);

	// Initialize and load the identities
	KviUserIdentityManager::init();

	if(getReadOnlyConfigPath(szTmp, KVI_CONFIGFILE_IDENTITIES))
		KviUserIdentityManager::instance()->load(szTmp);

	KviAnimatedPixmapCache::init();

	// Load the remaining configuration
	// Note that loadOptions() assumes that the current progress is 12 and
	// will bump it up to 45 in small steps
	loadOptions();

	// set the global font if needed
	updateApplicationFont();

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	updatePseudoTransparency();
#endif

	// enforce our "icon in popups" option - this is done also in each updateGui() call
	setAttribute(Qt::AA_DontShowIconsInMenus, !KVI_OPTION_BOOL(KviOption_boolShowIconsInPopupMenus));

	// Load the win properties config
	getLocalKvircDirectory(szTmp, Config, KVI_CONFIGFILE_WINPROPERTIES);
	g_pWinPropertiesConfig = new KviConfigurationFile(szTmp, KviConfigurationFile::ReadWrite);

	// Load the server database
	g_pServerDataBase = new KviIrcServerDataBase();
	if(getReadOnlyConfigPath(szTmp, KVI_CONFIGFILE_SERVERDB))
		g_pServerDataBase->load(szTmp);

	// Load the proxy database
	g_pProxyDataBase = new KviProxyDataBase();
	if(getReadOnlyConfigPath(szTmp, KVI_CONFIGFILE_PROXYDB))
		g_pProxyDataBase->load(szTmp);

	// Event manager
	if(getReadOnlyConfigPath(szTmp, KVI_CONFIGFILE_EVENTS))
		KviKvs::loadAppEvents(szTmp);

	if(getReadOnlyConfigPath(szTmp, KVI_CONFIGFILE_RAWEVENTS))
		KviKvs::loadRawEvents(szTmp);

	// Popup manager
	if(getReadOnlyConfigPath(szTmp, KVI_CONFIGFILE_POPUPS))
		KviKvs::loadPopups(szTmp);

	KviCustomToolBarManager::init();
	if(getReadOnlyConfigPath(szTmp, KVI_CONFIGFILE_CUSTOMTOOLBARS))
		KviCustomToolBarManager::instance()->load(szTmp);

	// Alias manager
	if(getReadOnlyConfigPath(szTmp, KVI_CONFIGFILE_ALIASES))
		KviKvs::loadAliases(szTmp);

	// Script addons manager (this in fact has delayed loading, so we don't even care
	// about showing up an entry in the splash screen)
	if(getReadOnlyConfigPath(szTmp, KVI_CONFIGFILE_SCRIPTADDONS))
		KviKvs::loadScriptAddons(szTmp);

	g_pTextIconManager = new KviTextIconManager();
	g_pTextIconManager->load();

	// load the recent data lists
	g_pRecentTopicList = new QStringList();
	//g_pBookmarkList = new QStringList();
	loadRecentEntries();

	// media manager
	g_pMediaManager = new KviMediaManager();
	g_pMediaManager->lock();
	if(getReadOnlyConfigPath(szTmp, KVI_CONFIGFILE_MEDIATYPES))
		g_pMediaManager->load(szTmp);
	g_pMediaManager->unlock();

	// registered user data base
	g_pRegisteredUserDataBase = new KviRegisteredUserDataBase();
	if(getReadOnlyConfigPath(szTmp, KVI_CONFIGFILE_REGUSERDB))
		g_pRegisteredUserDataBase->load(szTmp);

	// registered channel data base
	g_pRegisteredChannelDataBase = new KviRegisteredChannelDataBase();
	if(getReadOnlyConfigPath(szTmp, KVI_CONFIGFILE_REGCHANDB))
		g_pRegisteredChannelDataBase->load(szTmp);

	// file trader
	g_pSharedFilesManager = new KviSharedFilesManager();
	if(getReadOnlyConfigPath(szTmp, KVI_CONFIGFILE_SHAREDFILES))
		g_pSharedFilesManager->load(szTmp);

	// nick serv data base
	g_pNickServRuleSet = new KviNickServRuleSet();
	if(getReadOnlyConfigPath(szTmp, KVI_CONFIGFILE_NICKSERVDATABASE))
		g_pNickServRuleSet->load(szTmp);

	// Identity profiles database
	KviIdentityProfileSet::init();
	if(getReadOnlyConfigPath(szTmp, KVI_CONFIGFILE_PROFILESDATABASE))
		KviIdentityProfileSet::instance()->load(szTmp);

	KviAvatarCache::init();
	if(getReadOnlyConfigPath(szTmp, KVI_CONFIGFILE_AVATARCACHE))
		KviAvatarCache::instance()->load(szTmp);

	KviInputHistory::init();
	if(getReadOnlyConfigPath(szTmp, KVI_CONFIGFILE_INPUTHISTORY))
		KviInputHistory::instance()->load(szTmp);

	KviDefaultScriptManager::init();
	if(getReadOnlyConfigPath(szTmp, KVI_CONFIGFILE_DEFAULTSCRIPT))
		KviDefaultScriptManager::instance()->load(szTmp);
	else
		KviDefaultScriptManager::instance()->loadEmptyConfig();

// Eventually initialize the crypt engine manager
#ifdef COMPILE_CRYPT_SUPPORT
	g_pCryptEngineManager = new KviCryptEngineManager();
#endif

	// and the input popup
	g_pInputPopup = new QMenu();

	// create the server parser
	g_pServerParser = new KviIrcServerParser();

	// Script object controller
	//g_pScriptObjectController = new KviScriptObjectController(); gone

	// Cache the QStyle theme before it's overriden
	(void)KviRuntimeInfo::qtTheme();

	// create the frame window, we're almost up and running...
	createFrame();

	// ok, we also have an UI now

	// check if this is the first time this version of KVIrc runs...
	if(firstTimeRun())
	{
		// Finish the setup...
		setupFinish();
		// ensure mainwindow is visible
		g_pMainWindow->show();
	}

	// hello world!
	m_bSetupDone = true;

	// We're REALLY up and running!
	// kill the splash screen

	if(KVI_OPTION_BOOL(KviOption_boolShowServersConnectDialogOnStart))
		g_pMainWindow->executeInternalCommand(KVI_INTERNALCOMMAND_SERVERSJOIN_OPEN);

	// check if we're in trouble...
	checkSuggestRestoreDefaultScript();

	// start our heartbeat now
	m_iHeartbeatTimerId = startTimer(1000);
}

void KviApplication::frameDestructorCallback()
{
	// here we should kill anything that depends on g_pMainWindow, g_pActiveWindow and related being non zero...
	// kill all the objects, while we have windows...
	if(KviKvsKernel::instance())
	{
		if(KviKvsKernel::instance()->objectController())
		{
			KviKvsKernel::instance()->objectController()->clearInstances();
		}
	}
}

KviApplication::~KviApplication()
{
	// Another critical phase.
	// We shutdown our subsystems in the right order here.

	m_bClosingDown = true;

#ifndef COMPILE_NO_IPC
	destroyIpcSentinel();
#endif

	// if we still have a frame: kill it
	if(g_pMainWindow)
		delete g_pMainWindow;
	g_pActiveWindow = nullptr; // .. but it should be already 0 anyway

	if(g_pCtcpPageDialog)
		delete g_pCtcpPageDialog;

	if(!m_bSetupDone)
		return; // killed with IPC (nothing except the m_pFrameList was created yet)

	KviFileTransferManager::cleanup();

	// No more events are triggered from now on and no KVS commands are executed

	// Dangerous part.... we're unloading all the modules
	// We need to unload them early since they may use other subsystems
	// that we are going to kill now.
	delete g_pModuleManager;
	delete g_pModuleExtensionManager;
	// No more external modules exist: all that happens from now is generated
	// from inside the kvirc core.

	// We should have almost no UI here: only certain dialogs or popup windows may
	// still exist: they should be harmless tough.
	saveOptions();
	saveIdentities();
	KviUserIdentityManager::done();
	if(m_pRecentChannelDict)
		delete m_pRecentChannelDict;
	// now kill the stuff that the frame depends on
	saveIrcServerDataBase();
	delete g_pServerDataBase;
	saveProxyDataBase();
	delete g_pProxyDataBase;
	delete g_pWinPropertiesConfig;
	saveTextIcons();
	if(g_pTextIconWindow)
		delete g_pTextIconWindow;
	delete g_pTextIconManager;
	delete g_pIconManager;
	delete g_pServerParser;
	saveMediaTypes();
	delete g_pMediaManager;
	saveRecentEntries();
	saveAvatarCache();
	KviAvatarCache::done();
	//delete g_pBookmarkList;
	delete g_pRecentTopicList;
	saveRegisteredUsers();
	delete g_pRegisteredUserDataBase;
	saveRegisteredChannels();
	delete g_pRegisteredChannelDataBase;
	saveNickServ();
	delete g_pNickServRuleSet;
	saveIdentityProfiles();
	KviIdentityProfileSet::done();
	saveDefaultScripts();
	KviDefaultScriptManager::done();
	saveSharedFiles();
	delete g_pSharedFilesManager;
	saveAppEvents();
	saveRawEvents();
	saveToolBars();
	KviCustomToolBarManager::done();
	savePopups();
	saveAliases();
	g_pGlobalWindowDict.clear();
	saveScriptAddons();
	// kill the remaining resources
	delete g_pColorWindow;
	if(g_pHistoryWindow)
		delete g_pHistoryWindow;
	saveInputHistory();
	KviInputHistory::done();
	delete g_pInputPopup;
//delete g_pScriptObjectController;
#ifdef COMPILE_CRYPT_SUPPORT
	delete g_pCryptEngineManager;
#endif
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	destroyPseudoTransparency();
#endif
	m_PendingAvatarChanges.clear();
	KviAnimatedPixmapCache::done();
// Kill the thread manager.... all the slave threads should have been already terminated ...
#ifdef COMPILE_SSL_SUPPORT
	KviSSL::globalDestroy();
#endif
	KviThreadManager::globalDestroy();
	// kill the scripting engine
	KviKvs::done();
	// shut up the action manager
	saveActions();
	KviActionManager::done();

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	WSACleanup();
#endif

	KviLocale::done();
	// goodbye cruel world...
}

static int g_iGloballyUniqueId = 0;

int KviApplication::getGloballyUniqueId()
{
	g_iGloballyUniqueId++;
	return g_iGloballyUniqueId;
}

bool KviApplication::supportsCompositing()
{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	//we need >= win2000
	return true;
#endif

#ifdef COMPILE_QX11INFO_SUPPORT
	// Qt5 does not support QX11Info::isCompositingManagerRunning()
	// Well...assume we're compositing capable, should be true on all recent Linux distros
	return true;
#endif // COMPILE_QX11INFO_SUPPORT
#ifdef COMPILE_ON_MAC
	return true;
#endif
	return false;
};

void KviApplication::notifierMessage(KviWindow * pWnd, int iIconId, const QString & szMsg, unsigned int uMessageLifetime)
{
	/*
	We have different behaviour depending on support enabled at compile time (env)
	and options enabled by the user (opt).
	Let's see the scheme to understand which is chosen:

	1: env: DBus, KDE
	   opt: a: enabled, enabled   -> KDE
	        b: enabled, disabled  -> DBus
	        c: disabled, disabled -> KVIrc

	2: env: DBus, no KDE
	   opt: a: enabled  -> DBus
	        b: disabled -> KVIrc

	3: env: no DBus, no KDE -> KVIrc
	 ______           __________           ______________
	|      |         | Want KDE |         |  Scheme 1a   |
	| KDE  |---YES-->| Notifier |---YES-->| KDE Notifier |
	|______|         |__________|         |______________|
	   |                  |
	   NO                 NO
	   |                  |
	 __V___          _____V_____           _______________
	|      |        | Want DBus |         | Schemes 1b-2a |
	| DBus |--YES-->| Notifier  |---YES-->| DBus Notifier |
	|______|        |___________|         |_______________|
	   |                  |
	   NO                 NO
	   |                  |
	 __V__________________V_____
	|    Schemes 1c - 2b - 3    |
	|           KVIrc           |
	|___________________________|

	:)
	*/

	if(!pWnd)
		return;

	QString szTitle = __tr2qs("KVIrc Messaging");
	QString szText = __tr2qs("Message from %1:\n\n").arg(pWnd->target());
	szText += KviControlCodes::stripControlBytes(szMsg);

#ifdef COMPILE_KDE_SUPPORT
	if(KVI_OPTION_BOOL(KviOption_boolUseKDENotifier))
	{
		// Scheme 1a: KDE
		static bool bKNotifyConfigFileChecked = false;

		if(!bKNotifyConfigFileChecked)
		{
#ifdef COMPILE_KDE4_SUPPORT
			QString szFileName = KStandardDirs::locateLocal("data", QString::fromUtf8("kvirc/kvirc.notifyrc"));
#else
			QString szFileName = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/" + QString::fromUtf8("kvirc/kvirc.notifyrc");
#endif
			if(szFileName.isEmpty())
				szFileName = QString::fromUtf8("%1/.kde/share/apps/kvirc/kvirc.notifyrc").arg(QDir::homePath());

			QFileInfo inf(szFileName);

			if(!inf.exists())
			{
				KviFileUtils::makeDir(inf.absolutePath());

				QString szKNotifyConfig = QString::fromUtf8(
				    "[Global]\n"
				    "IconName=kvirc\n"
				    "Comment=The K-Visual IRC Client\n"
				    "Name=kvirc\n"
				    "\n"
				    "[Event/incomingMessage]\n"
				    "Name=KVIrc\n"
				    "Comment=Someone sent us a message\n"
				    "Action=Popup|Taskbar\n"
				    "Persistent=true\n");

				KviFileUtils::writeFile(szFileName, szKNotifyConfig);
			}

			bKNotifyConfigFileChecked = true;
		}

		QStringList actions;
		actions << __tr2qs("View");
		actions << __tr2qs("Ignore");

		QPixmap * pIcon = nullptr;
		KviIconManager::SmallIcon eIcon = KviIconManager::None;
		switch(pWnd->type())
		{
			case KviWindow::Console:
				eIcon = KviIconManager::Links;
				break;
			case KviWindow::Channel:
				eIcon = KviIconManager::Channel;
				break;
			case KviWindow::Query:
			{
				KviQueryWindow * pQuery = KVI_DYNAMIC(KviQueryWindow *, pWnd);
				if(!pQuery)
					break;

				KviUserListEntry * pEntry = pQuery->userListView()->findEntry(pWnd->target());
				if(!pEntry)
					break;

				KviAvatar * pAvatar = pEntry->globalData()->avatar();
				if(!pAvatar)
					break;

				pIcon = pAvatar->pixmap();
				if(!pIcon)
					eIcon = KviIconManager::Query;
			}
			break;
			case KviWindow::SocketSpy:
				eIcon = KviIconManager::Spy;
				break;
			case KviWindow::DccChat:
				eIcon = KviIconManager::DccMsg;
				break;
			case KviWindow::DccTransfer:
				eIcon = KviIconManager::Dcc;
				break;
			case KviWindow::UserWindow:
				eIcon = KviIconManager::UserWindow;
				break;
			case KviWindow::Debug:
				eIcon = KviIconManager::Bomb;
				break;
			//KviWindow::DeadChannel
			//KviWindow::DeadQuery
			//KviWindow::Editor
			//KviWindow::Help
			//KviWindow::Terminal
			//KviWindow::Links
			//KviWindow::List
			//KviWindow::DccCanvas
			//KviWindow::DccVoice
			//KviWindow::DccVideo
			//KviWindow::Tool
			//KviWindow::IOGraph
			//KviWindow::ScriptEditor
			//KviWindow::ScriptObject
			//KviWindow::LogView
			//KviWindow::Offer
			case KviWindow::Unknown:
			default:
				eIcon = KviIconManager::KVIrc;
				break;
		}

		if(!pIcon)
			pIcon = g_pIconManager->getSmallIcon(eIcon);

		KNotification * pNotify = nullptr;
#if defined(COMPILE_KDE4_SUPPORT)
#if KDE_IS_VERSION(4, 4, 0)
		pNotify = new KNotification("incomingMessage", KNotification::CloseWhenWidgetActivated, this);
#else
		pNotify = new KNotification("incomingMessage", g_pMainWindow, KNotification::CloseWhenWidgetActivated);
#endif
#else
		pNotify = new KNotification("incomingMessage", g_pMainWindow, KNotification::CloseWhenWidgetActivated);
#endif
		pNotify->setFlags(KNotification::Persistent);
		pNotify->setTitle(szTitle);
		pNotify->setText(szText);
		pNotify->setActions(actions);
		pNotify->setPixmap(*pIcon);

#if defined(COMPILE_KDE4_SUPPORT)
		pNotify->setComponentData(KComponentData(aboutData()));
#else
		pNotify->setComponentName("kvirc");
#endif

		connect(pNotify, SIGNAL(activated()), this, SLOT(showParentFrame()));
		connect(pNotify, SIGNAL(action1Activated()), this, SLOT(showParentFrame()));
		connect(pNotify, SIGNAL(action2Activated()), pNotify, SLOT(close()));
		connect(pNotify, SIGNAL(ignored()), pNotify, SLOT(close()));

		pNotify->sendEvent();
	}
	else
	{
#endif // COMPILE_KDE_SUPPORT
#ifdef COMPILE_DBUS_SUPPORT
		if(KVI_OPTION_BOOL(KviOption_boolUseDBusNotifier))
		{
			// Scheme 1b-2a: DBus
			QString szIcon = g_pIconManager->getSmallIconName(KviIconManager::KVIrc);

			// org.freedesktop.Notifications.Notify
			QVariantList args;
			args << QString("KVIrc");                          // application name
			args << QVariant(QVariant::UInt);                  // notification id
			args << szIcon;                                    // application icon
			args << szTitle;                                   // summary text
			args << szText;                                    // detailed text
			args << QStringList();                             // actions, optional
			args << QVariantMap();                             // hints, optional
			args << static_cast<int>(uMessageLifetime * 1000); // timeout in msecs

			QDBusInterface * pNotify = new QDBusInterface("org.freedesktop.Notifications", "/org/freedesktop/Notifications", "org.freedesktop.Notifications", QDBusConnection::sessionBus(), this);
			QDBusMessage reply = pNotify->callWithArgumentList(QDBus::Block, "Notify", args);
			if(reply.type() == QDBusMessage::ErrorMessage)
			{
				QDBusError err = reply;
				qDebug("D-Bus notify error\nID: %u\nName: %s\nMessage: %s\n", reply.arguments().first().toUInt(), qPrintable(err.name()), qPrintable(err.message()));
			}
		}
		else
		{
#endif // COMPILE_DBUS_SUPPORT
			// Scheme 1c-2b-3: KVIrc
			KviModule * m = g_pModuleManager->getModule("notifier");
			if(!m)
				return;

			KviNotifierMessageParam s;
			s.pWindow = pWnd;
			s.szIcon.sprintf("%d", iIconId);
			s.szMessage = szMsg;
			s.uMessageLifetime = uMessageLifetime;

			m->ctrl("notifier::message", (void *)&s);
#ifdef COMPILE_DBUS_SUPPORT
		}
#endif // COMPILE_DBUS_SUPPORT
#ifdef COMPILE_KDE_SUPPORT
	}
#endif // COMPILE_KDE_SUPPORT
}

void KviApplication::showParentFrame()
{
	if(!g_pMainWindow)
		return;

	if(g_pMainWindow->isMinimized())
		g_pMainWindow->setWindowState(g_pMainWindow->windowState() & (~Qt::WindowMinimized | Qt::WindowActive));

	g_pMainWindow->showMaximized();
}

QTextCodec * KviApplication::defaultTextCodec()
{
	QTextCodec * pCodec = nullptr;
	if(!KVI_OPTION_STRING(KviOption_stringDefaultTextEncoding).isEmpty())
	{
		pCodec = KviLocale::instance()->codecForName(KVI_OPTION_STRING(KviOption_stringDefaultTextEncoding).toLatin1());
		if(pCodec)
			return pCodec;
	}
	pCodec = QTextCodec::codecForLocale();
	if(pCodec)
		return pCodec;
	pCodec = KviLocale::instance()->codecForName("UTF-8");
	if(!pCodec)
		qDebug("KviApplication::defaultTextCodec(): can't find a suitable text codec for locale :/");
	return pCodec;
}

QTextCodec * KviApplication::defaultSrvCodec()
{
	QTextCodec * pCodec = nullptr;
	if(!KVI_OPTION_STRING(KviOption_stringDefaultSrvEncoding).isEmpty())
	{
		pCodec = KviLocale::instance()->codecForName(KVI_OPTION_STRING(KviOption_stringDefaultSrvEncoding).toLatin1());
		if(pCodec)
			return pCodec;
	}
	pCodec = QTextCodec::codecForLocale();
	if(pCodec)
		return pCodec;
	pCodec = KviLocale::instance()->codecForName("UTF-8");
	if(!pCodec)
		qDebug("KviApplication::defaultSrcCodec(): can't find a suitable text codec for locale :/");
	return pCodec;
}

void KviApplication::loadDefaultScript(QString szItem)
{
	QString szCmd;

	if(szItem.isEmpty())
		szItem = "default";

	getGlobalKvircDirectory(szCmd, KviApplication::DefScript, QString("%1.kvs").arg(szItem));
	szCmd.prepend("parse \"");
	szCmd.append("\"");
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	szCmd.replace("\\", "\\\\");
#endif
	KviKvsScript::run(szCmd, g_pMainWindow->firstConsole());
}

void KviApplication::checkSuggestRestoreDefaultScript()
{
	static bool bSuggestedOnce = false;
	if(KVI_OPTION_BOOL(KviOption_boolDoNotSuggestRestoreDefaultScript))
		return;

	if(bSuggestedOnce)
		return; // already suggested in this kvirc session

	// first: check if the user configuration has ever been updated to the current version
	if(!KviDefaultScriptManager::instance()->isDefscriptUpToDate())
	{
		switch(
		    QMessageBox::question(nullptr, __tr2qs("Update Default Scripts - KVIrc"),
		        __tr2qs("<b>Your KVirc installation has been updated successfully.</b><br><br>"
		                "KVIrc's default scripts should also be updated. "
		                "<b>Do you want to restore the default scripts?</b><br><br>"
		                "Hint: If you choose \"No\" you can always restore the "
		                "default scripts by selecting the appropriate entry from the \"Scripting\" menu later."),
		        __tr2qs("No and Don't Ask Me Again"), __tr2qs("No"), __tr2qs("Yes"), 1, 1))
		{
			case 0:
				KVI_OPTION_BOOL(KviOption_boolDoNotSuggestRestoreDefaultScript) = true;
				return;
				break;
			case 1:
				// we want to execute the next checks, don't return now
				break;
			default:
				restoreDefaultScript();
				// we want to execute the next checks after the attempted restore, don't return now
				break;
		}
	}

	//second, check for some common corruptions in scripts
	int iScore = 0;

	if(KviCustomToolBarManager::instance()->descriptorCount() < 1)
		iScore += 100; // straight suggest
	else if(KviKvsPopupManager::instance()->popupCount() < 1)
		iScore += 100; // straight suggest
	else
	{
		// other stuff is not that critical
		if(!KviKvsEventManager::instance()->hasAppHandlers(KviEvent_OnChannelNickPopupRequest))
			iScore += 80;
		if(!KviKvsEventManager::instance()->hasAppHandlers(KviEvent_OnDCCChatPopupRequest))
			iScore += 20;
		if(!KviKvsEventManager::instance()->hasAppHandlers(KviEvent_OnConsolePopupRequest))
			iScore += 20;
		if(!KviKvsEventManager::instance()->hasAppHandlers(KviEvent_OnChannelPopupRequest))
			iScore += 20;
		if(!KviKvsEventManager::instance()->hasAppHandlers(KviEvent_OnChannelNickPopupRequest))
			iScore += 20;
		if(!KviKvsEventManager::instance()->hasAppHandlers(KviEvent_OnQueryPopupRequest))
			iScore += 20;
		if(!KviKvsEventManager::instance()->hasAppHandlers(KviEvent_OnQueryNickPopupRequest))
			iScore += 20;
		if(KviCustomToolBarManager::instance()->descriptorCount() < 4)
			iScore += 20;
		if(KviKvsPopupManager::instance()->popupCount() < 3)
			iScore += 20;
		if(KviCustomToolBarManager::instance()->visibleToolBarCount() < 2)
		{
			iScore += 20;
			if(KviCustomToolBarManager::instance()->visibleToolBarCount() < 1)
				iScore += 20;
		}
	}

	if(iScore < 100)
		return;

	bSuggestedOnce = true;

	switch(
	    QMessageBox::question(nullptr, __tr2qs("Detected Installation Issues - KVIrc"),
	        __tr2qs("<b>Your KVIrc installation is incomplete.</b><br><br>"
	                "You seem to be missing some of the features that the KVIrc default scripts provide. "
	                "<b>Do you want to restore the default scripts?</b><br><br>"
	                "Hint: If you choose \"No\" you can always restore the "
	                "default scripts by selecting the appropriate entry from the \"Scripting\" menu later."),
	        __tr2qs("No and Don't Ask Me Again"), __tr2qs("No"), __tr2qs("Yes"), 1, 1))
	{
		case 0:
			KVI_OPTION_BOOL(KviOption_boolDoNotSuggestRestoreDefaultScript) = true;
			return;
			break;
		case 1:
			return;
			break;
		default:
			restoreDefaultScript();
			break;
	}
}

void KviApplication::restoreDefaultScript()
{
	KviDefaultScriptManager::instance()->restore();
}

#ifndef COMPILE_NO_IPC
void KviApplication::createIpcSentinel()
{
	m_pIpcSentinel = new KviIpcSentinel();
}

void KviApplication::destroyIpcSentinel()
{
	delete m_pIpcSentinel;
	m_pIpcSentinel = nullptr;
}

void KviApplication::ipcMessage(char * pcMessage)
{
	if(!g_pMainWindow)
		return;
	KviConsoleWindow * pConsole = g_pMainWindow->firstConsole();
	if(!pConsole)
		return;
	if(_OUTPUT_VERBOSE)
	{
		KviCString szCmd = pcMessage;
		if(szCmd.len() > 30)
			szCmd.cutRight(szCmd.len() - 30);
		int iIdx = szCmd.findFirstIdx('\n');
		if(iIdx != -1)
			szCmd.cutRight(szCmd.len() - (iIdx + 1));
		pConsole->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Remote command received (%s ...)"), szCmd.ptr());
	}
	if (kvi_strEqualCIN(pcMessage, "openurl ", 8))
	{
		// there actually is no reliable way of raising the main window, but we try our best!
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		SetForegroundWindow((HWND)g_pMainWindow->winId());
#endif
		g_pMainWindow->activateWindow();
	}
	KviKvsScript::run(pcMessage, pConsole);
}
#endif // COMPILE_NO_IPC

void KviApplication::setAvatarFromOptions()
{
	for(auto & it : g_pGlobalWindowDict)
	{
		KviConsoleWindow * pWindow = dynamic_cast<KviConsoleWindow *>(it.second);
		if(pWindow)
			pWindow->setAvatarFromOptions();
	}
}

void KviApplication::setAvatarOnFileReceived(
    KviConsoleWindow * pConsole,
    const QString & szRemoteUrl,
    const QString & szNick,
    const QString & szUser,
    const QString & szHost)
{
	if(m_PendingAvatarChanges.size() >= KVI_MAX_PENDING_AVATARS) // can't be...
		// kill an entry to make space
		m_PendingAvatarChanges.erase(m_PendingAvatarChanges.begin());

	std::unique_ptr<KviPendingAvatarChange> pAvatar(new KviPendingAvatarChange());
	pAvatar->pConsole = pConsole;
	pAvatar->szRemoteUrl = szRemoteUrl;
	pAvatar->szNick = szNick;
	pAvatar->szUser = szUser;
	pAvatar->szHost = szHost;

	m_PendingAvatarChanges.emplace(pAvatar.get(), std::move(pAvatar));
}

KviPendingAvatarChange * KviApplication::findPendingAvatarChange(
    KviConsoleWindow * pConsole,
    const QString & szNick,
    const QString & szRemoteUrl)
{
	for(auto & upAvatarPair : m_PendingAvatarChanges)
	{
		KviPendingAvatarChange * pAvatar = upAvatarPair.second.get();
		if(!pConsole || (pAvatar->pConsole == pConsole))
		{
			if(szNick.isNull() || (szNick == pAvatar->szNick))
			{
				if(szRemoteUrl == pAvatar->szRemoteUrl)
					return pAvatar;
			}
		}
	}

	return nullptr;
}

void KviApplication::fileDownloadTerminated(
    bool bSuccess,
    const QString & szRemoteUrl,
    const QString & szLocalFileName,
    const QString & szNick,
    const QString & szError,
    bool bQuiet)
{

	KviPendingAvatarChange * pAvatar = findPendingAvatarChange(nullptr, szNick, szRemoteUrl);

	if(pAvatar == nullptr)
	{
		// signal dcc completion only for NON-avatars
		// FIXME: This option is misnamed and misplaced in the options dialog :(
		//        it seems to refer only to DCC while it refers to any file transfer
		if(KVI_OPTION_BOOL(KviOption_boolNotifyDccSendSuccessInNotifier) && (!bQuiet))
		{
			if(!g_pActiveWindow)
				return;
			if(g_pActiveWindow->hasAttention(KviWindow::MainWindowIsVisible))
				return;
			QString szMsg;
			int iIconId;
			if(!bSuccess)
			{
				iIconId = KviIconManager::DccError;
				if(szNick.isEmpty())
					szMsg = __tr2qs("File download failed");
				else
					szMsg = __tr2qs("File download from %1 failed").arg(szNick);
				szMsg += ": ";
				szMsg += szError;
				szMsg += " (";
				szMsg += szLocalFileName;
				szMsg += ")";
			}
			else
			{
				iIconId = KviIconManager::DccMsg;
				if(szNick.isEmpty())
					szMsg = __tr2qs("File download successfully complete");
				else
					szMsg = __tr2qs("File download from %1 successfully complete").arg(szNick);
				szMsg += " (";
				szMsg += szLocalFileName;
				szMsg += ")";
			}
			notifierMessage(nullptr, iIconId, KviQString::toHtmlEscaped(szMsg), KVI_OPTION_UINT(KviOption_uintNotifierAutoHideTime));
		}
		return;
	}

	if(bSuccess)
	{
		if(windowExists(pAvatar->pConsole))
		{
			pAvatar->pConsole->setAvatar(
			    pAvatar->szNick,
			    pAvatar->szUser,
			    pAvatar->szHost,
			    szLocalFileName,
			    (KviQString::equalCIN("http://", szRemoteUrl, 7) || KviQString::equalCIN("https://", szRemoteUrl, 8)) ? szRemoteUrl : QString());
		}
	}
	else
	{
		if((!_OUTPUT_MUTE) && (!bQuiet))
		{
			pAvatar->pConsole->output(KVI_OUT_AVATAR, __tr2qs("Avatar download failed for %Q!%Q@%Q and URL %Q: %Q"),
			    &(pAvatar->szNick), &(pAvatar->szUser), &(pAvatar->szHost), &(szRemoteUrl), &(szError));
		}
	}

	m_PendingAvatarChanges.erase(pAvatar);
}

#ifdef COMPILE_PSEUDO_TRANSPARENCY
void KviApplication::destroyPseudoTransparency()
{
	if(g_pShadedParentGlobalDesktopBackground)
	{
		delete g_pShadedParentGlobalDesktopBackground;
		g_pShadedParentGlobalDesktopBackground = nullptr;
	}
	if(g_pShadedChildGlobalDesktopBackground)
	{
		delete g_pShadedChildGlobalDesktopBackground;
		g_pShadedChildGlobalDesktopBackground = nullptr;
	}
}

void KviApplication::triggerUpdatePseudoTransparency()
{
	if(m_bUpdatePseudoTransparencyPending)
		return;
	m_bUpdatePseudoTransparencyPending = true;
	QTimer::singleShot(0, this, SLOT(updatePseudoTransparency()));
}

void KviApplication::createGlobalBackgrounds(QPixmap * pix)
{
	// create shaded copies...
	if(g_pShadedParentGlobalDesktopBackground)
		delete g_pShadedParentGlobalDesktopBackground;
	if(g_pShadedChildGlobalDesktopBackground)
		delete g_pShadedChildGlobalDesktopBackground;
	g_pShadedParentGlobalDesktopBackground = new QPixmap(pix->width(), pix->height());
	g_pShadedChildGlobalDesktopBackground = new QPixmap(pix->width(), pix->height());
	QPainter p;
	p.begin(g_pShadedParentGlobalDesktopBackground);
	p.drawPixmap(0, 0, *pix);
	p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
	if(KVI_OPTION_UINT(KviOption_uintGlobalTransparencyParentFadeFactor) > 100)
		KVI_OPTION_UINT(KviOption_uintGlobalTransparencyParentFadeFactor) = 100;
	p.setOpacity((float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyParentFadeFactor) / (float)100));
	p.fillRect(0, 0, pix->width(), pix->height(), QBrush(KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade)));
	p.end();

	p.begin(g_pShadedChildGlobalDesktopBackground);
	p.drawPixmap(0, 0, *pix);
	p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
	if(KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) > 100)
		KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) = 100;
	p.setOpacity((float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) / (float)100));
	p.fillRect(0, 0, pix->width(), pix->height(), QBrush(KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade)));
	p.end();

	if(g_pMainWindow)
		g_pMainWindow->updatePseudoTransparency();
}
#endif //COMPILE_PSEUDO_TRANSPARENCY

void KviApplication::updatePseudoTransparency()
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	m_bUpdatePseudoTransparencyPending = false;
	if(KVI_OPTION_BOOL(KviOption_boolUseGlobalPseudoTransparency))
	{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		if(KVI_OPTION_BOOL(KviOption_boolUseWindowsDesktopForTransparency))
		{
			QSize size = g_pApp->desktop()->screenGeometry(g_pApp->desktop()->primaryScreen()).size();
			// get the Program Manager
			HWND hWnd = FindWindow(TEXT("Progman"), TEXT("Program Manager"));
			// Create and setup bitmap
			const HDC displayDc = GetDC(0);
			HDC bitmap_dc = CreateCompatibleDC(displayDc);
			HBITMAP bitmap = CreateCompatibleBitmap(displayDc, size.width(), size.height());
			HGDIOBJ null_bitmap = SelectObject(bitmap_dc, bitmap);

			PrintWindow(hWnd, bitmap_dc, 0);

			SelectObject(bitmap_dc, null_bitmap);
			DeleteDC(bitmap_dc);
			ReleaseDC(0, displayDc);
			QPixmap pix = QtWin::fromHBITMAP(bitmap);

			DeleteObject(bitmap);

			createGlobalBackgrounds(&pix);
		}
		else
#endif // COMPILE_ON_WINDOWS || COMPILE_ON_MINGW

		if(KVI_OPTION_PIXMAP(KviOption_pixmapGlobalTransparencyBackground).pixmap())
		{
			createGlobalBackgrounds(KVI_OPTION_PIXMAP(KviOption_pixmapGlobalTransparencyBackground).pixmap());
		}
		else
		{
			//destroy pseudo transparency pixmaps
			destroyPseudoTransparency();
//if we get here, no pseudo transparency method can be used / is enabled.
#ifdef COMPILE_X11_SUPPORT
			//under x11, we still have to check real transparency methods
			if(!KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency))
			{
				//real transparency is off => turn off transparency support at all
				KVI_OPTION_BOOL(KviOption_boolUseGlobalPseudoTransparency) = false;
			}
#else
			// on other platforms, no way to proceed
			KVI_OPTION_BOOL(KviOption_boolUseGlobalPseudoTransparency) = false;
#endif // COMPILE_X11_SUPPORT
		}
	}
	else
	{
		//transparency is disabled
		//destroy pseudo transparency pixmaps
		destroyPseudoTransparency();
		//make sure real transparency is disabled
		KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) = false;

		//update widgets
		if(g_pMainWindow)
			g_pMainWindow->updatePseudoTransparency();
	}
#endif //COMPILE_PSEUDO_TRANSPARENCY
}

void KviApplication::triggerUpdateGui()
{
	if(m_bUpdateGuiPending)
		return;
	m_bUpdateGuiPending = true;
	QTimer::singleShot(0, this, SLOT(updateGui()));
}

void KviApplication::updateGui()
{
	m_bUpdateGuiPending = false;
	// enforce our "icon in popups" option
	setAttribute(Qt::AA_DontShowIconsInMenus, !KVI_OPTION_BOOL(KviOption_boolShowIconsInPopupMenus));
	g_pMainWindow->applyOptions();
}

void KviApplication::updateApplicationFont()
{
	if(KVI_OPTION_BOOL(KviOption_boolUseGlobalApplicationFont))
	{
		setFont(KVI_OPTION_FONT(KviOption_fontApplication));
		if(g_pMainWindow)
			g_pMainWindow->setFont(font());
	}
	else
	{
		setFont(m_fntDefaultFont);
		if(g_pMainWindow)
			g_pMainWindow->setFont(m_fntDefaultFont);
	}
}

void KviApplication::loadRecentEntries()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp, Config, KVI_CONFIGFILE_RECENT);
	KviConfigurationFile cfg(szTmp, KviConfigurationFile::Read);
	*g_pRecentTopicList = cfg.readStringListEntry("RecentTopicList", QStringList());
}

void KviApplication::saveRecentEntries()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp, Config, KVI_CONFIGFILE_RECENT);
	KviConfigurationFile cfg(szTmp, KviConfigurationFile::Write);
	cfg.writeEntry("RecentTopicList", *g_pRecentTopicList);
}

void KviApplication::saveAvatarCache()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp, Config, KVI_CONFIGFILE_AVATARCACHE);
	KviAvatarCache::instance()->save(szTmp);
}

void KviApplication::saveToolBars()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp, Config, KVI_CONFIGFILE_CUSTOMTOOLBARS);
	KviCustomToolBarManager::instance()->save(szTmp);
}

void KviApplication::savePopups()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp, Config, KVI_CONFIGFILE_POPUPS);
	KviKvs::savePopups(szTmp);
}

void KviApplication::saveInputHistory()
{
	if(KVI_OPTION_BOOL(KviOption_boolEnableInputHistory))
	{
		QString szTmp;
		getLocalKvircDirectory(szTmp, Config, KVI_CONFIGFILE_INPUTHISTORY);
		KviInputHistory::instance()->save(szTmp);
	}
}

void KviApplication::saveAliases()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp, Config, KVI_CONFIGFILE_ALIASES);
	KviKvs::saveAliases(szTmp);
}

void KviApplication::saveScriptAddons()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp, Config, KVI_CONFIGFILE_SCRIPTADDONS);
	KviKvs::saveScriptAddons(szTmp);
}

void KviApplication::saveTextIcons()
{
	g_pTextIconManager->save();
}

void KviApplication::saveAppEvents()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp, Config, KVI_CONFIGFILE_EVENTS);
	KviKvs::saveAppEvents(szTmp);
}

void KviApplication::saveRawEvents()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp, Config, KVI_CONFIGFILE_RAWEVENTS);
	KviKvs::saveRawEvents(szTmp);
}

void KviApplication::saveMediaTypes()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp, Config, KVI_CONFIGFILE_MEDIATYPES);
	g_pMediaManager->lock();
	g_pMediaManager->save(szTmp);
	g_pMediaManager->unlock();
}

void KviApplication::saveIrcServerDataBase()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp, Config, KVI_CONFIGFILE_SERVERDB);
	g_pServerDataBase->save(szTmp);
}

void KviApplication::saveProxyDataBase()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp, Config, KVI_CONFIGFILE_PROXYDB);
	g_pProxyDataBase->save(szTmp);
}

void KviApplication::saveRegisteredUsers()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp, Config, KVI_CONFIGFILE_REGUSERDB);
	g_pRegisteredUserDataBase->save(szTmp);
}

void KviApplication::saveRegisteredChannels()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp, Config, KVI_CONFIGFILE_REGCHANDB);
	g_pRegisteredChannelDataBase->save(szTmp);
}

void KviApplication::saveNickServ()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp, Config, KVI_CONFIGFILE_NICKSERVDATABASE);
	g_pNickServRuleSet->save(szTmp);
}

void KviApplication::saveIdentityProfiles()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp, Config, KVI_CONFIGFILE_PROFILESDATABASE);
	KviIdentityProfileSet::instance()->save(szTmp);
}

void KviApplication::saveDefaultScripts()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp, Config, KVI_CONFIGFILE_DEFAULTSCRIPT);
	KviDefaultScriptManager::instance()->save(szTmp);
}

void KviApplication::saveSharedFiles()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp, Config, KVI_CONFIGFILE_SHAREDFILES);
	g_pSharedFilesManager->save(szTmp);
}

void KviApplication::saveActions()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp, Config, KVI_CONFIGFILE_USERACTIONS);
	KviActionManager::instance()->save(szTmp);
}

void KviApplication::saveIdentities()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp, Config, KVI_CONFIGFILE_IDENTITIES);
	KviUserIdentityManager::instance()->save(szTmp);
}

void KviApplication::saveConfiguration()
{
	// this is NOT called when the application is closing down
	KviCustomToolBarManager::instance()->storeVisibilityState();
	saveOptions();
	saveIdentities();
	saveActions();
	saveIrcServerDataBase();
	saveProxyDataBase();
	saveRecentEntries();
	saveAvatarCache();
	saveAppEvents();
	saveRawEvents();
	saveMediaTypes();
	saveRegisteredUsers();
	saveRegisteredChannels();
	saveNickServ();
	saveIdentityProfiles();
	saveDefaultScripts();
	saveSharedFiles();
	savePopups();
	saveToolBars();
	saveAliases();
	saveTextIcons();
	saveInputHistory();
	saveScriptAddons();
	KviKvs::flushUserClasses();
	g_pWinPropertiesConfig->sync();
}

void KviApplication::autoConnectToServers()
{
	KviPointerList<KviIrcServer> * pList = g_pServerDataBase->autoConnectOnStartupServers();
	if(pList)
	{
		for(auto & pServer : pList)
		{
			QString szCommand = "server -u \"id:";
			if(pServer.id().isEmpty())
				pServer.generateUniqueId();
			szCommand += pServer.id();
			szCommand += "\"";
			KviKvsScript::run(szCommand, activeConsole());
		}
		g_pServerDataBase->clearAutoConnectOnStartupServers();
	}

	KviPointerList<KviIrcNetwork> * pListNet = g_pServerDataBase->autoConnectOnStartupNetworks();
	if(pListNet)
	{
		for(auto & pNetwork : pListNet)
		{
			QString szCommandx = "server -u \"net:";
			szCommandx += pNetwork.name();
			szCommandx += "\"";
			KviKvsScript::run(szCommandx, activeConsole());
		}
		g_pServerDataBase->clearAutoConnectOnStartupNetworks();
	}
}

void KviApplication::createFrame()
{
	Q_ASSERT(g_pMainWindow == nullptr);

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	if(KVI_OPTION_BOOL(KviOption_boolShowTaskBarButton))
		new KviMainWindow(0);
	else
		new KviMainWindow(new QWidget(0, 0));
#else
	new KviMainWindow(nullptr);
#endif

	Q_ASSERT(g_pMainWindow != nullptr);

	g_pMainWindow->createNewConsole(true);

	if(!m_szExecAfterStartup.isEmpty())
	{
		KviKvsScript::run(m_szExecAfterStartup, g_pMainWindow->firstConsole());
		m_szExecAfterStartup = "";
	}

	// auto connect to servers if needed
	if(g_pServerDataBase->autoConnectOnStartupServers() || g_pServerDataBase->autoConnectOnStartupNetworks())
		autoConnectToServers();

	if(KVI_OPTION_BOOL(KviOption_boolShowDockExtension))
		g_pMainWindow->executeInternalCommand(KVI_INTERNALCOMMAND_TRAYICON_SHOW);

	if(KVI_OPTION_BOOL(KviOption_boolStartupMinimized))
		g_pMainWindow->showMinimized();
	else
		g_pMainWindow->show();
}

bool KviApplication::connectionExists(KviIrcConnection * pConn)
{
	for(auto & it : g_pGlobalWindowDict)
	{
		if(it.second->connection() == pConn)
			return true;
	}
	return false;
}

bool KviApplication::windowExists(KviWindow * pWnd)
{
	for(auto & it : g_pGlobalWindowDict)
	{
		if(it.second == pWnd)
			return true;
	}
	return false;
}

unsigned int KviApplication::windowCount()
{
	return g_pGlobalWindowDict.size();
}

KviConsoleWindow * KviApplication::findConsole(QString & szServer, QString & szNick)
{
	for(auto & it : g_pGlobalWindowDict)
	{
		KviConsoleWindow * pWindow = dynamic_cast<KviConsoleWindow *>(it.second);
		if(!(pWindow && pWindow->type() == KviWindow::Console && pWindow->isConnected()))
			continue;

		if(!szServer.isEmpty())
		{
			if(KviQString::equalCI(szServer, pWindow->connection()->currentServerName()))
			{
				if(szNick.isEmpty())
					return pWindow;

				if(KviQString::equalCI(szNick, pWindow->connection()->currentNickName()))
					return pWindow;
			}
		}
		else
		{
			if(!szNick.isEmpty())
			{
				if(KviQString::equalCI(szNick, pWindow->connection()->currentNickName()))
					return pWindow;
			}
		}
	}
	return nullptr;
}

void KviApplication::restartLagMeters()
{
	for(auto & it : g_pGlobalWindowDict)
	{
		KviConsoleWindow * pWindow = dynamic_cast<KviConsoleWindow *>(it.second);
		if(pWindow && pWindow->type() == KviWindow::Console && pWindow->connection())
			pWindow->connection()->restartLagMeter();
	}
}

void KviApplication::restartNotifyLists()
{
	for(auto & it : g_pGlobalWindowDict)
	{
		KviConsoleWindow * pWindow = dynamic_cast<KviConsoleWindow *>(it.second);
		if(pWindow && pWindow->type() == KviWindow::Console && pWindow->connection())
			pWindow->connection()->restartNotifyList();
	}
}

void KviApplication::resetAvatarForMatchingUsers(KviRegisteredUser * pUser)
{
	for(auto & it : g_pGlobalWindowDict)
	{
		KviConsoleWindow * pWindow = dynamic_cast<KviConsoleWindow *>(it.second);
		if(pWindow && pWindow->type() == KviWindow::Console)
			pWindow->resetAvatarForMatchingUsers(pUser);
	}
}

KviConsoleWindow * KviApplication::findConsole(unsigned int uIrcContextId)
{
	for(auto & it : g_pGlobalWindowDict)
	{
		KviConsoleWindow * pWindow = dynamic_cast<KviConsoleWindow *>(it.second);
		if(pWindow && pWindow->context()->id() == uIrcContextId)
			return pWindow;
	}
	return nullptr;
}

KviConsoleWindow * KviApplication::topmostConnectedConsole()
{
	// check the foreground window console

	KviConsoleWindow * pConsole = activeConsole();
	if(!pConsole)
		return nullptr;
	if(pConsole->isConnected())
		return pConsole;

	// try ANY connected console

	for(auto & it : g_pGlobalWindowDict)
	{
		KviConsoleWindow * pWindow = dynamic_cast<KviConsoleWindow *>(it.second);
		if(pWindow && pWindow->type() == KviWindow::Console && pWindow->isConnected())
			return pWindow;
	}

	return nullptr;
}

KviWindow * KviApplication::findWindow(const QString & szWindowId)
{
	auto search = g_pGlobalWindowDict.find(szWindowId);
	return search != g_pGlobalWindowDict.end() ? search->second : nullptr;
}

KviWindow * KviApplication::findWindowByCaption(const QString & szWindowCaption, int iContextId)
{
	for(auto & it : g_pGlobalWindowDict)
	{
		if(KviQString::equalCI(szWindowCaption, it.second->plainTextCaption()) && (iContextId == -1 || it.second->context()->id() == static_cast<uint>(iContextId)))
			return it.second;
	}

	return nullptr;
}

void KviApplication::registerWindow(KviWindow * pWnd)
{
	g_pGlobalWindowDict.emplace(pWnd->id(), pWnd);
}

void KviApplication::unregisterWindow(KviWindow * pWnd)
{
	g_pGlobalWindowDict.erase(pWnd->id());
}

KviConsoleWindow * KviApplication::activeConsole()
{
	if(!g_pMainWindow)
		return nullptr;
	if(g_pActiveWindow)
	{
		if(g_pActiveWindow->console())
			return g_pActiveWindow->console();
	}
	return g_pMainWindow->firstConsole();
}

//
// RECENT STUFF LISTS & POPUPS HANDLING
//

// Helper for KviApplication::addRecent*()
static void merge_to_stringlist_option(const QString & szItem, int iOption, int iMaxEntries)
{
	for(QStringList::Iterator it = KVI_OPTION_STRINGLIST(iOption).begin();
	    it != KVI_OPTION_STRINGLIST(iOption).end(); ++it)
	{
		// Do a case-insensitive search (for nicknames, servers, and channels)
		if(!QString::compare(szItem.toLower(), (*it).toLower()))
		{
			// In the recent list, remove and put as first so more recent items
			// are always first
			it = KVI_OPTION_STRINGLIST(iOption).erase(it);
			--it;
		}
	}
	while(KVI_OPTION_STRINGLIST(iOption).count() >= iMaxEntries)
	{
		KVI_OPTION_STRINGLIST(iOption)
		    .erase(KVI_OPTION_STRINGLIST(iOption).isEmpty() ? KVI_OPTION_STRINGLIST(iOption).end() : --KVI_OPTION_STRINGLIST(iOption).end());
	}
	KVI_OPTION_STRINGLIST(iOption)
	    .prepend(szItem);
}

void KviApplication::addRecentUrl(const QString & szText)
{
	merge_to_stringlist_option(szText, KviOption_stringlistRecentIrcUrls, 50);
	emit(recentUrlsChanged());
}

void KviApplication::addRecentNickname(const QString & szNewNick)
{
	merge_to_stringlist_option(szNewNick, KviOption_stringlistRecentNicknames, KVI_MAX_RECENT_NICKNAMES);
}

void KviApplication::addRecentChannel(const QString & szChan, const QString & szNet)
{
	if(!m_pRecentChannelDict)
		buildRecentChannels();

	QStringList * pList = m_pRecentChannelDict->find(szNet);
	if(!pList)
	{
		pList = new QStringList(szChan);
		m_pRecentChannelDict->insert(szNet, pList);
	}

	if(!pList->contains(szChan))
		pList->append(szChan);
}

void KviApplication::buildRecentChannels()
{
	if(m_pRecentChannelDict)
		delete m_pRecentChannelDict;

	m_pRecentChannelDict = new KviPointerHashTable<QString, QStringList>;
	m_pRecentChannelDict->setAutoDelete(true);

	QString szChan, szNet;

	for(auto & it : KVI_OPTION_STRINGLIST(KviOption_stringlistRecentChannels))
	{
		if(it.isEmpty())
			continue;

		szChan = it.section(KVI_RECENT_CHANNELS_SEPARATOR, 0, 0);
		szNet = it.section(KVI_RECENT_CHANNELS_SEPARATOR, 1);

		if(szNet.isEmpty())
			continue;

		QStringList * pList = m_pRecentChannelDict->find(szNet);
		if(!pList)
		{
			pList = new QStringList(szChan);
			m_pRecentChannelDict->insert(szNet, pList);
		}

		if(!pList->contains(szChan))
			pList->append(szChan);
	}
}

void KviApplication::saveRecentChannels()
{
	if(!m_pRecentChannelDict)
		return;

	QString szTmp;

	KVI_OPTION_STRINGLIST(KviOption_stringlistRecentChannels)
	    .clear();

	KviPointerHashTableIterator<QString, QStringList> it(*m_pRecentChannelDict);

	for(; it.current(); ++it)
	{
		for(QStringList::Iterator it_str = it.current()->begin(); it_str != it.current()->end(); ++it_str)
		{
			szTmp = *it_str;
			szTmp.append(KVI_RECENT_CHANNELS_SEPARATOR);
			szTmp.append(it.currentKey());
			KVI_OPTION_STRINGLIST(KviOption_stringlistRecentChannels)
			    .append(szTmp);
		}
	}
}

QStringList * KviApplication::recentChannelsForNetwork(const QString & szNet)
{
	if(!m_pRecentChannelDict)
		buildRecentChannels();
	return m_pRecentChannelDict->find(szNet);
}

void KviApplication::addRecentServer(const QString & szServer)
{
	merge_to_stringlist_option(szServer, KviOption_stringlistRecentServers, KVI_MAX_RECENT_SERVERS);
}

void KviApplication::fillRecentServersPopup(QMenu * pMenu)
{
	// FIXME: #warning "MAYBE DISABLE THE SERVERS THAT WE ARE ALREADY CONNECTED TO ?"
	pMenu->clear();
	for(auto & it : KVI_OPTION_STRINGLIST(KviOption_stringlistRecentServers))
	{
		if(it == "")
			continue;
		pMenu->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Server)), it);
	}
}

void KviApplication::fillRecentNicknamesPopup(QMenu * pMenu, KviConsoleWindow * pConsole)
{
	pMenu->clear();
	QAction * pAction;
	bool bAlreadyFound = false;
	for(auto & it : KVI_OPTION_STRINGLIST(KviOption_stringlistRecentNicknames))
	{
		if(it == "")
			continue;
		pAction = pMenu->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Nick)), it);
		if(!pConsole->isConnected())
			pAction->setEnabled(false);
		else
		{
			if(!bAlreadyFound)
			{
				bool bIsCurrent = KviQString::equalCS(pConsole->connection()->currentNickName(), it);
				pAction->setEnabled(!bIsCurrent);
				if(bIsCurrent)
					bAlreadyFound = true;
			}
		}
	}
}

void KviApplication::fillRecentChannelsPopup(QMenu * pMenu, KviConsoleWindow * pConsole)
{
	pMenu->clear();
	QAction * pAction;
	QStringList * pList = recentChannelsForNetwork(pConsole->currentNetworkName());
	if(pList)
	{
		for(auto & it : *pList)
		{
			if(it == "")
				continue; // ?
			pAction = pMenu->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Channel)), it);
			if(!pConsole->isConnected())
				pAction->setEnabled(false);
			else
				pAction->setEnabled(!(pConsole->connection()->findChannel(it)));
		}
	}
}

void KviApplication::heartbeat(kvi_time_t tNow)
{
	const struct tm * pTm = localtime(&tNow);

	if(g_pApp->topmostConnectedConsole())
	{
		// FIXME: this has huge precision problems...
		KVI_OPTION_UINT(KviOption_uintTotalConnectionTime)
		++;
	}

	if(pTm && !pTm->tm_hour && !pTm->tm_min && !pTm->tm_sec)
	{
		for(auto & it : g_pGlobalWindowDict)
		{
			if(it.second->view() && it.second->view()->isLogging())
				it.second->view()->startLogging(nullptr);
		}
	}
}

void KviApplication::timerEvent(QTimerEvent * e)
{
	if(e->timerId() != m_iHeartbeatTimerId)
	{
		QObject::timerEvent(e);
		return;
	}

	// our heartbeat

	kvi_time_t tNow = kvi_unixTime();

	heartbeat(tNow);
}

// qvariant.h uses this, and it is included by the qt generated moc file for Qt >= 3.0.0
#ifdef Bool
#undef Bool
#endif
