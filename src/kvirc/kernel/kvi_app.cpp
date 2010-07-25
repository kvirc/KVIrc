//=============================================================================
//
//   File : kvi_app.cpp
//   Creation date : Sun Jun 18 2000 12:39:45 by Szymon Stefanek
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

#define _KVI_APP_CPP_

#include "kvi_socket.h"
#include "kvi_app.h"
#include "kvi_debug.h"
#include "kvi_frame.h"
#include "kvi_msgbox.h"
#include "kvi_iconmanager.h"
#include "kvi_input.h"
#include "kvi_input_history.h"
#include "kvi_config.h"
#include "kvi_colorwin.h"
#include "kvi_window.h"
#include "kvi_ircserverdb.h"
#include "kvi_proxydb.h"
#include "kvi_menubar.h"
#include "kvi_options.h"
#include "kvi_sparser.h"
#include "kvi_modulemanager.h"
#include "kvi_mediatype.h"
#include "kvi_regusersdb.h"
#include "kvi_thread.h"
#include "kvi_sharedfiles.h"
#include "kvi_confignames.h"
#include "kvi_windowlist.h"
#include "kvi_defaults.h"
#include "kvi_locale.h"
#include "kvi_out.h"
#include "kvi_splash.h"
#include "kvi_nickserv.h"
#include "kvi_identityprofile.h"
#include "kvi_xlib.h"
#include "kvi_texticonmanager.h"
#include "kvi_texticonwin.h"
#include "kvi_historywin.h"
#include "kvi_ctcppagedialog.h"
#include "kvi_regchan.h"
#include "kvi_moduleextension.h"
#include "kvi_internalcmd.h"
#include "kvi_filetransfer.h"
#include "kvi_kvs.h"
#include "kvi_ircurl.h"
#include "kvi_avatarcache.h"
#include "kvi_actionmanager.h"
#include "kvi_customtoolbarmanager.h"
#include "kvi_fileutils.h"
#include "kvi_time.h"
#include "kvi_stringconversion.h"
#include "kvi_useridentity.h"
#include "kvi_ircview.h"
#include "kvi_animatedpixmapcache.h"

#ifndef COMPILE_NO_IPC
	#include "kvi_ipc.h"
#endif

#include "kvi_kvs_script.h"
#include "kvi_kvs_popupmanager.h"
#include "kvi_kvs_kernel.h"
#include "kvi_kvs_object_controller.h"
#include "kvi_kvs_eventtriggers.h"
#include "kvi_sourcesdate.h"
#include "kvi_pointerhashtable.h"
#include "kvi_tal_popupmenu.h"

#include <QSplitter>
#include <QStringList>
#include <QClipboard>
#include <QMessageBox>
#include <QTextCodec>
#include <QMetaObject>
#include <QTextDocument>

/*
HACK These 2 hacks are defined because X11 defines Unsorted and None
which conflicts with QDir and KviApp::KvircSubdir
DO NOT REMOVE THEM EVEN IF THEY ARE DEFINED ALSO IN kvi_app.h
*/
#ifdef Unsorted
	#undef Unsorted
#endif

#ifdef None
	#undef None
#endif


#include <QDir>

#ifdef COMPILE_SSL_SUPPORT
	#include "kvi_ssl.h"
#endif

#ifdef COMPILE_ON_WINDOWS
	#include <QPluginLoader>
#endif


#include <stdlib.h> // rand & srand
#include <time.h> // time() in srand()

// Global application pointer
KVIRC_API KviApp                        * g_pApp                        = 0;

KviConfig                               * g_pWinPropertiesConfig        = 0;
KVIRC_API KviServerDataBase             * g_pServerDataBase             = 0;
KVIRC_API KviProxyDataBase              * g_pProxyDataBase              = 0;

// Global windows
KVIRC_API KviColorWindow                * g_pColorWindow                = 0;
KVIRC_API KviTextIconWindow             * g_pTextIconWindow             = 0;
KVIRC_API KviTalPopupMenu               * g_pInputPopup                 = 0;
KVIRC_API QStringList                   * g_pRecentTopicList            = 0;
KVIRC_API KviPointerHashTable<QString,KviWindow>  * g_pGlobalWindowDict = 0;
KVIRC_API KviMediaManager               * g_pMediaManager               = 0;
KVIRC_API KviSharedFilesManager         * g_pSharedFilesManager         = 0;
KVIRC_API KviNickServRuleSet            * g_pNickServRuleSet            = 0;
KVIRC_API KviCtcpPageDialog             * g_pCtcpPageDialog             = 0;
KVIRC_API KviRegisteredChannelDataBase  * g_pRegisteredChannelDataBase  = 0;
KVIRC_API KviHistoryWindowWidget        * g_pHistoryWindow              = 0;

// this is eventually set by libkviident
KVIRC_API int                             g_iIdentDaemonRunningUsers    = 0;

KVIRC_API KviSplashScreen               * g_pSplashScreen               = 0;


// Loaded and destroyed by KviIconManager
QPixmap                                 * g_pUserChanStatePixmap        = 0;
QPixmap                                 * g_pActivityMeterPixmap        = 0;

#ifdef COMPILE_PSEUDO_TRANSPARENCY

	#include <QImage>

	KVIRC_API QPixmap               * g_pShadedParentGlobalDesktopBackground = 0; // the pixmap that we use for MdiManager
	KVIRC_API QPixmap               * g_pShadedChildGlobalDesktopBackground  = 0; // the pixmap that we use for MdiChild
        #if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
                #include <winuser.h>
                #include <QDesktopWidget>
        #endif
#endif

#ifdef COMPILE_CRYPT_SUPPORT
	#include "kvi_crypt.h"
	// global crypt engine manager
	KVIRC_API KviCryptEngineManager * g_pCryptEngineManager = 0;
#endif

#include <QStyleFactory>

KviApp::KviApp(int &argc,char ** argv)
: KviTalApplication(argc,argv)
{
	setApplicationName("KVIrc");
	setApplicationVersion(KVIRC_VERSION_RELEASE);
	setOrganizationDomain("kvirc.net");
	setOrganizationName("KVIrc");

	// Ok...everything begins here
	g_pApp                  = this;
	m_szConfigFile          = QString();
	m_bCreateConfig         = false;
	m_bShowSplashScreen     = true;
	m_bUpdateGuiPending     = false;
	m_pPendingAvatarChanges = NULL;
	m_pRecentChannelDict   = NULL;
#ifndef COMPILE_NO_IPC
	m_pIpcSentinel          = NULL;
#endif
	m_iHeartbeatTimerId     = -1;
	m_fntDefaultFont        = font();
	// next step is setup()
	m_bSetupDone = false;
	kvi_socket_flushTrafficCounters();
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	m_bPortable = KviFileUtils::fileExists(g_pApp->applicationDirPath()+KVI_PATH_SEPARATOR_CHAR+"portable");
#endif
}

void KviApp::setup()
{
	// Here we start up all the substystems.
	// This is a really critical phase since in general subsystems depend
	// on each other and we must activate them in the right order.
	// Don't move stuff around unless you really know what you're doing.

	// Initialize the random number generator
	::srand(::time(0));

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	// setup winsock.dll
	WSADATA w;
	WSAStartup(MAKELONG((unsigned short)2,(unsigned short)0),&w);
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

	// Setup our filesystem and initalize locale
	loadDirectories();
	KviStringConversion::init(m_szGlobalKvircDir,m_szLocalKvircDir);

	g_pIconManager = new KviIconManager();

	// add KVIrc common dirs to QT searchpath
	QString szPath;
	getLocalKvircDirectory(szPath,None,"avatars");
	QDir::addSearchPath("avatars",szPath);
	getLocalKvircDirectory(szPath,None,"pics");
	QDir::addSearchPath("pics",szPath);
	getLocalKvircDirectory(szPath,None,"audio");
	QDir::addSearchPath("audio",szPath);

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	//need to load image plugins:(
	QString szPluginsDir;
	getGlobalKvircDirectory(szPluginsDir,None,"qt-plugins/");
	setLibraryPaths(QStringList(szPluginsDir));
	//KviMessageBox::information(libraryPaths().join(";"));
	//debug("%1",loader.isLoaded());
#endif

	// check if we want to permanently disable the splash screen
	// we do it once for every version: the user should see the new splash screens at least once
	QString szSplashDisableFile;
	getLocalKvircDirectory(szSplashDisableFile,Pics,"disable-splash." KVI_VERSION);

	if(KviFileUtils::fileExists(szSplashDisableFile))
		m_bShowSplashScreen = false;

	// Now we can create the splash screen (we can locate the splash image)
	if(m_bShowSplashScreen)
		createSplashScreen();

	KVI_SPLASH_SET_PROGRESS(0)

	// Make sure that the C strings are translated to utf8
	// This is a fallback solution anyway: we should use the appropriate
	// encoding every time.
	QTextCodec::setCodecForCStrings(KviLocale::codecForName("UTF-8"));

	QString tmp;
/*
	// Set the default help files search path
	QStringList list;

	getLocalKvircDirectory(tmp,Help); // localized help/lang or help if help/lang doesn't exist
	list.append(tmp);
	getLocalKvircDirectory(tmp,HelpEN); // help/en
	list.append(tmp);
	getLocalKvircDirectory(tmp,HelpNoIntl); // just help/
	list.append(tmp);
	getGlobalKvircDirectory(tmp,Help);
	list.append(tmp);
	getGlobalKvircDirectory(tmp,HelpEN);
	list.append(tmp);
	getGlobalKvircDirectory(tmp,HelpNoIntl);
	list.append(tmp);
	QDir::setSearchPaths("help",list);
*/

	KVI_SPLASH_SET_PROGRESS(1)

	// Initialize the scripting engine
	KviKvs::init();

	KVI_SPLASH_SET_PROGRESS(2)

	// Initialize the action manager
	KviActionManager::init();

	KVI_SPLASH_SET_PROGRESS(3)

	// Create the module manager early: so the other managers can load modules
	g_pModuleExtensionManager = new KviModuleExtensionManager();

	KVI_SPLASH_SET_PROGRESS(4);

	g_pModuleManager = new KviModuleManager();

	KVI_SPLASH_SET_PROGRESS(5)

	if(getReadOnlyConfigPath(tmp,KVI_CONFIGFILE_USERACTIONS))
		KviActionManager::instance()->load(tmp);

	KVI_SPLASH_SET_PROGRESS(8);

	// Initialize and load the identities
	KviUserIdentityManager::init();

	KVI_SPLASH_SET_PROGRESS(9);

	if(getReadOnlyConfigPath(tmp,KVI_CONFIGFILE_IDENTITIES))
		KviUserIdentityManager::instance()->load(tmp);

	KVI_SPLASH_SET_PROGRESS(12);

	KviAnimatedPixmapCache::init();

	KVI_SPLASH_SET_PROGRESS(13);

	// Load the remaining configuration
	// Note that loadOptions() assumes that the current progress is 12 and
	// will bump it up to 45 in small steps
	loadOptions();

	KVI_SPLASH_SET_PROGRESS(47)

	// set the global font if needed
	updateApplicationFont();

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	updatePseudoTransparency();
#endif

	KVI_SPLASH_SET_PROGRESS(48)

	// Load the win properties config
	getLocalKvircDirectory(tmp,Config,KVI_CONFIGFILE_WINPROPERTIES);
	g_pWinPropertiesConfig = new KviConfig(tmp,KviConfig::ReadWrite);

	KVI_SPLASH_SET_PROGRESS(50)

	// Load the server database
	g_pServerDataBase   = new KviServerDataBase();
	if(getReadOnlyConfigPath(tmp,KVI_CONFIGFILE_SERVERDB))
		g_pServerDataBase->load(tmp);

	KVI_SPLASH_SET_PROGRESS(53)

	// Load the proxy database
	g_pProxyDataBase = new KviProxyDataBase();
	if(getReadOnlyConfigPath(tmp,KVI_CONFIGFILE_PROXYDB))
		g_pProxyDataBase->load(tmp);

	KVI_SPLASH_SET_PROGRESS(54)

	// Event manager
	if(getReadOnlyConfigPath(tmp,KVI_CONFIGFILE_EVENTS))
		KviKvs::loadAppEvents(tmp);

	KVI_SPLASH_SET_PROGRESS(59)

	if(getReadOnlyConfigPath(tmp,KVI_CONFIGFILE_RAWEVENTS))
		KviKvs::loadRawEvents(tmp);

	KVI_SPLASH_SET_PROGRESS(62)

	// Popup manager
	if(getReadOnlyConfigPath(tmp,KVI_CONFIGFILE_POPUPS))
		KviKvs::loadPopups(tmp);

	KVI_SPLASH_SET_PROGRESS(67)

	KviCustomToolBarManager::init();
	if(getReadOnlyConfigPath(tmp,KVI_CONFIGFILE_CUSTOMTOOLBARS))
		KviCustomToolBarManager::instance()->load(tmp);

	KVI_SPLASH_SET_PROGRESS(70)

	// Alias manager
	if(getReadOnlyConfigPath(tmp,KVI_CONFIGFILE_ALIASES))
		KviKvs::loadAliases(tmp);

	KVI_SPLASH_SET_PROGRESS(75)

	// Script addons manager (this in fact has delayed loading, so we don't even care
	// about showing up an entry in the splash screen)
	if(getReadOnlyConfigPath(tmp,KVI_CONFIGFILE_SCRIPTADDONS))
		KviKvs::loadScriptAddons(tmp);

	KVI_SPLASH_SET_PROGRESS(77)

	g_pTextIconManager = new KviTextIconManager();
	g_pTextIconManager->load();

	KVI_SPLASH_SET_PROGRESS(80)

	// load the recent data lists
	g_pRecentTopicList = new QStringList();
	//g_pBookmarkList = new QStringList();
	loadRecentEntries();

	KVI_SPLASH_SET_PROGRESS(81)

	// media manager
	g_pMediaManager = new KviMediaManager();
	g_pMediaManager->lock();
	if(getReadOnlyConfigPath(tmp,KVI_CONFIGFILE_MEDIATYPES))
		g_pMediaManager->load(tmp);
	g_pMediaManager->unlock();

	KVI_SPLASH_SET_PROGRESS(82)

	// registered user data base
	g_pRegisteredUserDataBase = new KviRegisteredUserDataBase();
	if(getReadOnlyConfigPath(tmp,KVI_CONFIGFILE_REGUSERDB))
		g_pRegisteredUserDataBase->load(tmp);

	KVI_SPLASH_SET_PROGRESS(84)

	// registered channel data base
	g_pRegisteredChannelDataBase = new KviRegisteredChannelDataBase();
	if(getReadOnlyConfigPath(tmp,KVI_CONFIGFILE_REGCHANDB))
		g_pRegisteredChannelDataBase->load(tmp);

	KVI_SPLASH_SET_PROGRESS(85)

	// file trader
	g_pSharedFilesManager = new KviSharedFilesManager();
	if(getReadOnlyConfigPath(tmp,KVI_CONFIGFILE_SHAREDFILES))
		g_pSharedFilesManager->load(tmp);

	KVI_SPLASH_SET_PROGRESS(86)

	// nick serv data base
	g_pNickServRuleSet = new KviNickServRuleSet();
	if(getReadOnlyConfigPath(tmp,KVI_CONFIGFILE_NICKSERVDATABASE))
		g_pNickServRuleSet->load(tmp);

	KVI_SPLASH_SET_PROGRESS(87)

	// Identity profiles database
	KviIdentityProfileSet::init();
	if(getReadOnlyConfigPath(tmp,KVI_CONFIGFILE_PROFILESDATABASE))
		KviIdentityProfileSet::instance()->load(tmp);

	KVI_SPLASH_SET_PROGRESS(88)

	KviAvatarCache::init();
	if(getReadOnlyConfigPath(tmp,KVI_CONFIGFILE_AVATARCACHE))
		KviAvatarCache::instance()->load(tmp);

	KVI_SPLASH_SET_PROGRESS(89)

	KviInputHistory::init();
	if(getReadOnlyConfigPath(tmp,KVI_CONFIGFILE_INPUTHISTORY))
		KviInputHistory::instance()->load(tmp);

	KVI_SPLASH_SET_PROGRESS(90)

	// Eventually initialize the crypt engine manager
#ifdef COMPILE_CRYPT_SUPPORT
	g_pCryptEngineManager = new KviCryptEngineManager();
#endif

	//g_pGarbageCollector = new KviGarbageCollector();

	// and the input popup
	g_pInputPopup = new KviTalPopupMenu();

	// create the server parser
	g_pServerParser = new KviServerParser();

	KVI_SPLASH_SET_PROGRESS(91)

	// Global window dictionary
	g_pGlobalWindowDict = new KviPointerHashTable<QString,KviWindow>(41);
	g_pGlobalWindowDict->setAutoDelete(false);
	// Script object controller
	//g_pScriptObjectController = new KviScriptObjectController(); gone

	KVI_SPLASH_SET_PROGRESS(92)

	QString szStylesheetFile;
	getGlobalKvircDirectory(szStylesheetFile,Config,"style.css");
	if(KviFileUtils::fileExists(szStylesheetFile))
	{
		QString szStyleData;
		KviFileUtils::readFile(szStylesheetFile,szStyleData);
		szStyleData.replace("global://",m_szGlobalKvircDir);
		szStyleData.replace("local://",m_szLocalKvircDir);
		setStyleSheet(szStyleData);
	}

	// create the frame window, we're almost up and running...
	createFrame();

	// ok, we also have an UI now

	// check if this is the first time this version of KVIrc runs...
	if(firstTimeRun())
	{
		// Finish the setup...
		setupFinish();
	}

	// hello world!
	m_bSetupDone = true;

	// We're REALLY up and running!
	// kill the splash screen

	KVI_SPLASH_SET_PROGRESS(100)
	//KVI_SPLASH_SET_TEXT(__tr2qs("Have fun! :)"))

	if(KVI_OPTION_BOOL(KviOption_boolShowServersConnectDialogOnStart))
			g_pFrame->executeInternalCommand(KVI_INTERNALCOMMAND_SERVERSJOIN_OPEN);

	destroySplashScreen();

	// check if we're in trouble...
	checkSuggestRestoreDefaultScript();

	// start our heartbeat now
	m_iHeartbeatTimerId = startTimer(1000);
}

void KviApp::frameDestructorCallback()
{
	// here we should kill anything that depends on g_pFrame , g_pActiveWindow and related being non zero...

	// kill all the objects, while we have windows...
	if(KviKvsKernel::instance())
	{
		if(KviKvsKernel::instance()->objectController())
		{
			KviKvsKernel::instance()->objectController()->clearInstances();
		}
	}
}

KviApp::~KviApp()
{
	// Another critical phase.
	// We shutdown our subsystems in the right order here.

#ifndef COMPILE_NO_IPC
	destroyIpcSentinel();
#endif

	if(g_pSplashScreen)
		delete g_pSplashScreen;
	if(g_pCtcpPageDialog)
		delete g_pCtcpPageDialog;

	// if we still have a frame: kill it
	if(g_pFrame)
		delete g_pFrame;
	g_pActiveWindow = 0; // .. but it should be already 0 anyway

	// execute pending deletes (this may still contain some UI elements)
	//delete g_pGarbageCollector;

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
	if(g_pTextIconWindow)delete g_pTextIconWindow;
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
	saveSharedFiles();
	delete g_pSharedFilesManager;
	saveAppEvents();
	saveRawEvents();
	saveToolBars();
	KviCustomToolBarManager::done();
	savePopups();
	saveAliases();
	delete g_pGlobalWindowDict;
	saveScriptAddons();
	// kill the remaining resources
	delete g_pColorWindow;
	if(g_pHistoryWindow)delete g_pHistoryWindow;
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
	if(m_pPendingAvatarChanges)
		delete m_pPendingAvatarChanges;
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

	KviLocale::done(this);

	// goodbye cruel world...
}

static int g_iGloballyUniqueId = 0;

int KviApp::getGloballyUniqueId()
{
	g_iGloballyUniqueId++;
	return g_iGloballyUniqueId;
}

typedef struct _NotifierMessageSupaDupaParameterStruct
{
	KviWindow * pWindow;
	QString szIcon;
	QString szMessage;
	unsigned int uMessageLifetime; // 0 means no hide
} NotifierMessageSupaDupaParameterStruct;

void KviApp::notifierMessage(KviWindow * pWnd,int iIconId,const QString &szMsg,unsigned int uMessageLifetime)
{
	KviModule * m = g_pModuleManager->getModule("notifier");
	if(!m)return;

	NotifierMessageSupaDupaParameterStruct s;
	s.pWindow = pWnd;
	s.szIcon.sprintf("%d",iIconId);
	s.szMessage = szMsg;
	s.uMessageLifetime = uMessageLifetime;

	m->ctrl("notifier::message",(void *)&s);
}

QTextCodec * KviApp::defaultTextCodec()
{
	QTextCodec * c = 0;
	if(!KVI_OPTION_STRING(KviOption_stringDefaultTextEncoding).isEmpty())
	{
		c = KviLocale::codecForName(KVI_OPTION_STRING(KviOption_stringDefaultTextEncoding).toLatin1());
		if(c)return c;
	}
	c = QTextCodec::codecForLocale();
	if(c)return c;
	c = KviLocale::codecForName("UTF-8");
	if(!c)debug("KviApp::defaultTextCodec(): cannot find a suitable text codec for locale :/");
	return c;
}

QTextCodec * KviApp::defaultSrvCodec()
{
	QTextCodec * c = 0;
	if(!KVI_OPTION_STRING(KviOption_stringDefaultSrvEncoding).isEmpty())
	{
		c = KviLocale::codecForName(KVI_OPTION_STRING(KviOption_stringDefaultSrvEncoding).toLatin1());
		if(c)return c;
	}
	c = QTextCodec::codecForLocale();
	if(c)return c;
	c = KviLocale::codecForName("UTF-8");
	if(!c)debug("KviApp::defaultSrcCodec(): cannot find a suitable text codec for locale :/");
	return c;
}

void KviApp::loadDefaultScript()
{
	QString cmd;
	getGlobalKvircDirectory(cmd,KviApp::DefScript,"default.kvs");
	cmd.prepend("parse \"");
	cmd.append("\"");
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	cmd.replace("\\","\\\\");
#endif
	KviKvsScript::run(cmd,g_pFrame->firstConsole());
}

void KviApp::checkSuggestRestoreDefaultScript()
{
	static bool bSuggestedOnce = false;
	if(KVI_OPTION_BOOL(KviOption_boolDoNotSuggestRestoreDefaultScript))return;
	if(bSuggestedOnce)return; // already suggested in this kvirc session

	int score = 0;

	if(KviCustomToolBarManager::instance()->descriptorCount() < 1)score += 100; // straight suggest
	else if(KviKvsPopupManager::instance()->popupCount() < 1)score += 100; // straight suggest
	else {
		// other stuff is not that critical
		if(!KviKvsEventManager::instance()->hasAppHandlers(KviEvent_OnChannelNickPopupRequest))score += 80;
		if(!KviKvsEventManager::instance()->hasAppHandlers(KviEvent_OnDCCChatPopupRequest))score += 20;
		if(!KviKvsEventManager::instance()->hasAppHandlers(KviEvent_OnConsolePopupRequest))score += 20;
		if(!KviKvsEventManager::instance()->hasAppHandlers(KviEvent_OnChannelPopupRequest))score += 20;
		if(!KviKvsEventManager::instance()->hasAppHandlers(KviEvent_OnChannelNickPopupRequest))score += 20;
		if(!KviKvsEventManager::instance()->hasAppHandlers(KviEvent_OnQueryPopupRequest))score += 20;
		if(!KviKvsEventManager::instance()->hasAppHandlers(KviEvent_OnQueryNickPopupRequest))score += 20;
		if(KviCustomToolBarManager::instance()->descriptorCount() < 4)score += 20;
		if(KviKvsPopupManager::instance()->popupCount() < 3)score += 20;
		if(KviCustomToolBarManager::instance()->visibleToolBarCount() < 2)
		{
			score += 20;
			if(KviCustomToolBarManager::instance()->visibleToolBarCount() < 1)score += 20;
		}
	}

	if(score < 100)return;

	bSuggestedOnce = true;

	switch(QMessageBox::question(0,__tr2qs("Installation problems ?"),
		__tr2qs("<b>Ooops...</b><br><br>" \
				"<b>There are some reasons that make me think that your KVIrc installation is incomplete.</b><br><br>" \
				"I might be wrong, but you seem to miss some of the features that the default KVIrc script provides." \
				"This may happen because you have upgraded to an unstable svn version, " \
				"because you have accidentally deleted or damaged your configuration files, " \
				"because you have installed an incomplete script " \
				"or because you have hit a bug in KVIrc.<br><br>" \
				"I can repeat the installation of the default script in order " \
				"to restore the missing features.<br>" \
				"<b>Do you want the default script to be restored ?</b><br><br>" \
				"<font size=\"-1\">Hint: If you're a scripter and have intentionally removed some of the scripting features " \
				"then you may safely click on \"No and Don't Ask Me Again\", otherwise it might be " \
				"a good idea to click \"Yes\". If you still want to choose \"No\" you can always restore the " \
				"default script by choosing the appropriate entry from the \"Scripting\" menu.</font>"),
		__tr2qs("No and Don't Ask Me Again"),
		__tr2qs("No"),
		__tr2qs("Yes"),
		1,
		1))
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

void KviApp::restoreDefaultScript()
{
	if(QMessageBox::warning(0,__tr2qs("Restore Default Script - KVIrc"),
			__tr2qs("You are about to restore the default script.<br>This will erase any script changes you have made.<br>Do you wish to proceed?"),
			QMessageBox::Yes,QMessageBox::No|QMessageBox::Default|QMessageBox::Escape) != QMessageBox::Yes)return;

	KviKvs::clearAliases();
	KviKvs::clearAppEvents();
	KviKvs::clearRawEvents();
	KviKvs::clearPopups();
	KviKvs::clearScriptAddons();

	KviActionManager::instance()->killAllKvsUserActions();
	KviCustomToolBarManager::instance()->clear();

	loadDefaultScript();

	// and save settings to be sure...
	saveOptions();
}

#ifndef COMPILE_NO_IPC


void KviApp::createIpcSentinel()
{
	m_pIpcSentinel = new KviIpcSentinel();
}

void KviApp::destroyIpcSentinel()
{
	if(m_pIpcSentinel)delete m_pIpcSentinel;
    m_pIpcSentinel = 0;
}

void KviApp::ipcMessage(char * message)
{
	if(!g_pFrame)return;
	KviConsole * cns = g_pFrame->firstConsole();
	if(!cns)return;
	if(_OUTPUT_VERBOSE)
	{
		KviStr cmd = message;
		if(cmd.len() > 30)cmd.cutRight(cmd.len() - 30);
		int idx = cmd.findFirstIdx('\n');
		if(idx != -1)cmd.cutRight(cmd.len() - (idx + 1));
		cns->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Remote command received (%s ...)"),cmd.ptr());
	}
	KviKvsScript::run(message,cns);
}
#endif // COMPILE_NO_IPC

void KviApp::createSplashScreen()
{
	g_pSplashScreen = new KviSplashScreen();
	g_pSplashScreen->show();
}

void KviApp::destroySplashScreen()
{
	if(g_pSplashScreen)g_pSplashScreen->die();
}


void KviApp::setAvatarFromOptions()
{
	KviPointerHashTableIterator<QString,KviWindow> it(*g_pGlobalWindowDict);

	while(it.current())
	{
		if(it.current()->type() == KVI_WINDOW_TYPE_CONSOLE)
		{
			((KviConsole *)it.current())->setAvatarFromOptions();
		}
		++it;
	}

}

void KviApp::setAvatarOnFileReceived(KviConsole * pConsole,const QString &szRemoteUrl,const QString &szNick,const QString &szUser,const QString &szHost)
{
	if(!m_pPendingAvatarChanges)
	{
		m_pPendingAvatarChanges = new KviPointerList<KviPendingAvatarChange>;
		m_pPendingAvatarChanges->setAutoDelete(true);
	}

	if(m_pPendingAvatarChanges->count() >= KVI_MAX_PENDING_AVATARS) // can't be...
	{
		m_pPendingAvatarChanges->removeFirst(); // kill the first entry
	}

	KviPendingAvatarChange * p = new KviPendingAvatarChange;
	p->pConsole = pConsole;
	p->szRemoteUrl = szRemoteUrl;
	p->szNick = szNick;
	p->szUser = szUser;
	p->szHost = szHost;

	m_pPendingAvatarChanges->append(p);
}

KviPendingAvatarChange * KviApp::findPendingAvatarChange(KviConsole * pConsole,const QString &szNick,const QString &szRemoteUrl)
{
	if(!m_pPendingAvatarChanges)return 0;

	KviPendingAvatarChange * p;

	for(p = m_pPendingAvatarChanges->first();p;p = m_pPendingAvatarChanges->next())
	{
		if(!pConsole || (p->pConsole == pConsole))
		{
			if(szNick.isNull() || (szNick == p->szNick))
			{
				if(szRemoteUrl == p->szRemoteUrl)return p;
			}
		}
	}

	return 0;
}

void KviApp::fileDownloadTerminated(bool bSuccess,const QString &szRemoteUrl,const QString &szLocalFileName,const QString &szNick,const QString &szError,bool bQuiet)
{
	KviPendingAvatarChange * p;

	if(m_pPendingAvatarChanges)
	{
		p = findPendingAvatarChange(0,szNick,szRemoteUrl);
	} else {
		p = 0;
	}

	if(!p)
	{
		// signal dcc completion only for NON-avatars
		// FIXME: This option is misnamed and misplaced in the options dialog :(
		//        it seems to refer only to DCC while it refers to any file transfer
		if(KVI_OPTION_BOOL(KviOption_boolNotifyDccSendSuccessInNotifier) && (!bQuiet))
		{
			if(!g_pActiveWindow)return;
			if(g_pActiveWindow->hasAttention())return;
			QString szMsg;
			int iIconId;
			if(!bSuccess)
			{
				iIconId = KVI_SMALLICON_DCCERROR;
				if(szNick.isEmpty())
					szMsg = __tr2qs("File download failed");
				else
					szMsg = __tr2qs("File download from %1 failed").arg(szNick);
				szMsg += ": ";
				szMsg += szError;
				szMsg += " (";
				szMsg += szLocalFileName;
				szMsg += ")";
			} else {
				iIconId = KVI_SMALLICON_DCCMSG;
				if(szNick.isEmpty())
					szMsg = __tr2qs("File download successfully complete");
				else
					szMsg = __tr2qs("File download from %1 successfully complete").arg(szNick);
				szMsg += " (";
				szMsg += szLocalFileName;
				szMsg += ")";
			}
			notifierMessage(0,iIconId,Qt::escape(szMsg),KVI_OPTION_UINT(KviOption_uintNotifierAutoHideTime));
		}
		return;
	}

	if(bSuccess)
	{
		if(windowExists(p->pConsole))
		{
			p->pConsole->setAvatar(p->szNick,p->szUser,p->szHost,
				szLocalFileName,KviQString::equalCIN("http://",szRemoteUrl,7) ? szRemoteUrl : QString());
		}
	} else {
		if((!_OUTPUT_MUTE) && (!bQuiet))
		{
			p->pConsole->output(KVI_OUT_AVATAR,__tr2qs("Avatar download failed for %Q!%Q@%Q and url %Q: %Q"),
				&(p->szNick),&(p->szUser),&(p->szHost),&(szRemoteUrl),&(szError));
		}
	}

	m_pPendingAvatarChanges->removeRef(p);

	if(m_pPendingAvatarChanges->count() == 0)
	{
		delete m_pPendingAvatarChanges;
		m_pPendingAvatarChanges = 0;
	}
}



#ifdef COMPILE_PSEUDO_TRANSPARENCY


	void KviApp::destroyPseudoTransparency()
	{
		if(g_pShadedParentGlobalDesktopBackground)
		{
			delete g_pShadedParentGlobalDesktopBackground;
			g_pShadedParentGlobalDesktopBackground = 0;
		}
		if(g_pShadedChildGlobalDesktopBackground)
		{
			delete g_pShadedChildGlobalDesktopBackground;
			g_pShadedChildGlobalDesktopBackground = 0;
		}
	}

	void KviApp::triggerUpdatePseudoTransparency()
	{
		if(m_bUpdatePseudoTransparencyPending)return;
		m_bUpdatePseudoTransparencyPending = true;
		QTimer::singleShot(0,this,SLOT(updatePseudoTransparency()));
	}

	//
	// This function is taken from the KDE3 kimageeffect.cpp
	// The authors listed at the top of the file are :)
	//    Copyright (C) 1998, 1999 Christian Tibirna <ctibirna@total.net>
	//              (C) 1998, 1999 Daniel M. Duley <mosfet@kde.org>
	//              (C) 1998, 1999 Dirk A. Mueller <mueller@kde.org>
	//              (C) 2000       Josef Weidendorfer <weidendo@in.tum.de>
	//

	QImage& kimageeffect_fade(QImage &img,float val,const QColor &color)
	{
		if(img.width() == 0 || img.height() == 0)return img;

		// We don't handle bitmaps
		if (img.depth() == 1)return img;

		unsigned char tbl[256];
		for (int i=0; i<256; i++)tbl[i] = (int) (val * i + 0.5);

		int red = color.red();
		int green = color.green();
		int blue = color.blue();

		QRgb col;
		int r, g, b, cr, cg, cb;

		if (img.depth() <= 8)
		{
			// pseudo color
			for(int i=0; i<img.numColors(); i++)
			{
				col = img.color(i);
				cr = qRed(col); cg = qGreen(col); cb = qBlue(col);
				if (cr > red)r = cr - tbl[cr - red];
				else r = cr + tbl[red - cr];
				if (cg > green)g = cg - tbl[cg - green];
				else g = cg + tbl[green - cg];
				if (cb > blue)b = cb - tbl[cb - blue];
				else b = cb + tbl[blue - cb];
				img.setColor(i, qRgb(r, g, b));
			}
		} else {
		// truecolor
			for(int y=0; y<img.height(); y++)
			{
				QRgb *data = (QRgb *) img.scanLine(y);
				for (int x=0; x<img.width(); x++)
				{
					col = *data;
					cr = qRed(col); cg = qGreen(col); cb = qBlue(col);
					if (cr > red)r = cr - tbl[cr - red];
					else r = cr + tbl[red - cr];
					if (cg > green)g = cg - tbl[cg - green];
					else g = cg + tbl[green - cg];
					if (cb > blue)b = cb - tbl[cb - blue];
					else b = cb + tbl[blue - cb];
					*data++ = qRgb(r, g, b);
				}
			}
		}
		return img;
	}

	void KviApp::createGlobalBackgrounds(QPixmap * pix)
	{
		// create shaded copies...
		if(g_pShadedParentGlobalDesktopBackground)delete g_pShadedParentGlobalDesktopBackground;
		if(g_pShadedChildGlobalDesktopBackground)delete g_pShadedChildGlobalDesktopBackground;
		g_pShadedParentGlobalDesktopBackground = new QPixmap();
		g_pShadedChildGlobalDesktopBackground = new QPixmap();
		QImage img = pix->toImage();
		// play with the fade factors
		KVI_OPTION_UINT(KviOption_uintGlobalTransparencyParentFadeFactor) %= 100;
		if(KVI_OPTION_UINT(KviOption_uintGlobalTransparencyParentFadeFactor) > 0)
		{
			*g_pShadedParentGlobalDesktopBackground = QPixmap::fromImage(
				kimageeffect_fade(img,
					(float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyParentFadeFactor) / (float)100),
					KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade)));
		}
		KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) %= 100;
		if(KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) > 0)
		{
			*g_pShadedChildGlobalDesktopBackground = QPixmap::fromImage(
				kimageeffect_fade(img,
					(float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) / (float)100),
					KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade)));
		}
		if(g_pFrame)g_pFrame->updatePseudoTransparency();
	}

#endif //COMPILE_PSEUDO_TRANSPARENCY


void KviApp::updatePseudoTransparency()
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	m_bUpdatePseudoTransparencyPending = false;
	if(KVI_OPTION_BOOL(KviOption_boolUseGlobalPseudoTransparency))
	{
        #if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
                //TODO this has been tested only under mingw!
                //TODO this needs winxp: #if (_WIN32_WINNT >= 0x0501)
                if(KVI_OPTION_BOOL(KviOption_boolUseWindowsDesktopForTransparency))
                {
                        QSize size = g_pApp->desktop()->screenGeometry(g_pApp->desktop()->primaryScreen()).size();
                        // get the Program Manager
                        HWND hWnd = FindWindow ("Progman", "Program Manager");
                        // Create and setup bitmap
                        HDC bitmap_dc = CreateCompatibleDC(qt_win_display_dc());
                        HBITMAP bitmap = CreateCompatibleBitmap(qt_win_display_dc(), size.width(), size.height());
                        HGDIOBJ null_bitmap = SelectObject(bitmap_dc, bitmap);

                        PrintWindow (hWnd, bitmap_dc, 0);

                        SelectObject(bitmap_dc, null_bitmap);
                        DeleteDC(bitmap_dc);

                        QPixmap pix = QPixmap::fromWinHBITMAP(bitmap);

                        DeleteObject(bitmap);

                        createGlobalBackgrounds(&pix);
                } else
        #endif

		if(KVI_OPTION_PIXMAP(KviOption_pixmapGlobalTransparencyBackground).pixmap())
		{
			createGlobalBackgrounds(KVI_OPTION_PIXMAP(KviOption_pixmapGlobalTransparencyBackground).pixmap());
		} else {
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
	} else {
		//transparency is disabled

		//destroy pseudo transparency pixmaps
		destroyPseudoTransparency();
		//make sure real transparency is disabled
		KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) = false;

		//update widgets
		if(g_pFrame)g_pFrame->updatePseudoTransparency();
	}
#endif //COMPILE_PSEUDO_TRANSPARENCY
}

void KviApp::triggerUpdateGui()
{
	if(m_bUpdateGuiPending)return;
	m_bUpdateGuiPending = true;
	QTimer::singleShot(0,this,SLOT(updateGui()));
}

void KviApp::updateGui()
{
	m_bUpdateGuiPending = false;
	g_pFrame->applyOptions();
}

void KviApp::updateApplicationFont()
{
	if(KVI_OPTION_BOOL(KviOption_boolUseGlobalApplicationFont))
	{
			setFont(KVI_OPTION_FONT(KviOption_fontApplication));
			if(g_pFrame)
				g_pFrame->setFont(font());
	} else {
			setFont(m_fntDefaultFont);
			if(g_pFrame)
				g_pFrame->setFont(m_fntDefaultFont);
	}
}

void KviApp::loadRecentEntries()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp,Config,KVI_CONFIGFILE_RECENT);
	KviConfig cfg(szTmp,KviConfig::Read);
	*g_pRecentTopicList = cfg.readStringListEntry("RecentTopicList",QStringList());
	//*g_pBookmarkList = cfg.readStringListEntry("Bookmarks",QStringList());
}

void KviApp::saveRecentEntries()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp,Config,KVI_CONFIGFILE_RECENT);
	KviConfig cfg(szTmp,KviConfig::Write);
	cfg.writeEntry("RecentTopicList",*g_pRecentTopicList);
	//cfg.writeEntry("Bookmarks",*g_pBookmarkList);
}

void KviApp::saveAvatarCache()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp,Config,KVI_CONFIGFILE_AVATARCACHE);
	KviAvatarCache::instance()->save(szTmp);
}

void KviApp::saveToolBars()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp,Config,KVI_CONFIGFILE_CUSTOMTOOLBARS);
	KviCustomToolBarManager::instance()->save(szTmp);
}

void KviApp::savePopups()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp,Config,KVI_CONFIGFILE_POPUPS);
	KviKvs::savePopups(szTmp);
}

void KviApp::saveInputHistory()
{
	if(KVI_OPTION_BOOL(KviOption_boolEnableInputHistory))
	{
		QString szTmp;
		getLocalKvircDirectory(szTmp,Config,KVI_CONFIGFILE_INPUTHISTORY);
		KviInputHistory::instance()->save(szTmp);
	}
}

void KviApp::saveAliases()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp,Config,KVI_CONFIGFILE_ALIASES);
	KviKvs::saveAliases(szTmp);
}

void KviApp::saveScriptAddons()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp,Config,KVI_CONFIGFILE_SCRIPTADDONS);
	KviKvs::saveScriptAddons(szTmp);
}

void KviApp::saveTextIcons()
{
	g_pTextIconManager->save();
}

void KviApp::saveAppEvents()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp,Config,KVI_CONFIGFILE_EVENTS);
	KviKvs::saveAppEvents(szTmp);
}

void KviApp::saveRawEvents()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp,Config,KVI_CONFIGFILE_RAWEVENTS);
	KviKvs::saveRawEvents(szTmp);
}

void KviApp::saveMediaTypes()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp,Config,KVI_CONFIGFILE_MEDIATYPES);
	g_pMediaManager->lock();
	g_pMediaManager->save(szTmp);
	g_pMediaManager->unlock();
}

void KviApp::saveIrcServerDataBase()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp,Config,KVI_CONFIGFILE_SERVERDB);
	g_pServerDataBase->save(szTmp);
}

void KviApp::saveProxyDataBase()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp,Config,KVI_CONFIGFILE_PROXYDB);
	g_pProxyDataBase->save(szTmp);
}

void KviApp::saveRegisteredUsers()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp,Config,KVI_CONFIGFILE_REGUSERDB);
	g_pRegisteredUserDataBase->save(szTmp);
}

void KviApp::saveRegisteredChannels()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp,Config,KVI_CONFIGFILE_REGCHANDB);
	g_pRegisteredChannelDataBase->save(szTmp);
}

void KviApp::saveNickServ()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp,Config,KVI_CONFIGFILE_NICKSERVDATABASE);
	g_pNickServRuleSet->save(szTmp);
}

void KviApp::saveIdentityProfiles()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp,Config,KVI_CONFIGFILE_PROFILESDATABASE);
	KviIdentityProfileSet::instance()->save(szTmp);
}

void KviApp::saveSharedFiles()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp,Config,KVI_CONFIGFILE_SHAREDFILES);
	g_pSharedFilesManager->save(szTmp);
}

void KviApp::saveActions()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp,Config,KVI_CONFIGFILE_USERACTIONS);
	KviActionManager::instance()->save(szTmp);
}

void KviApp::saveIdentities()
{
	QString szTmp;
	getLocalKvircDirectory(szTmp,Config,KVI_CONFIGFILE_IDENTITIES);
	KviUserIdentityManager::instance()->save(szTmp);
}

void KviApp::saveConfiguration()
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

void KviApp::autoConnectToServers()
{
	KviPointerList<KviServer> * l = g_pServerDataBase->autoConnectOnStartupServers();
	if(l)
	{
		for(KviServer * s = l->first();s;s = l->next())
		{
			QString szCommand = "server -u \"id:";
			if(s->id().isEmpty())s->generateUniqueId();
			szCommand += s->id();
			szCommand += "\"";
			KviKvsScript::run(szCommand,activeConsole());
		}
		g_pServerDataBase->clearAutoConnectOnStartupServers();
	}

	KviPointerList<KviNetwork> * lr = g_pServerDataBase->autoConnectOnStartupNetworks();
	if(lr)
	{
		for(KviNetwork * r = lr->first();r;r = lr->next())
		{
			QString szCommandx = "server -u \"net:";
			szCommandx += r->name();
			szCommandx += "\"";
			KviKvsScript::run(szCommandx,activeConsole());
		}
		g_pServerDataBase->clearAutoConnectOnStartupNetworks();
	}
}

void KviApp::createFrame()
{
	if(g_pFrame)qDebug("WARNING: Creating the main frame twice!");
	g_pFrame = new KviFrame();
	g_pFrame->createNewConsole(true);

	if(m_szExecAfterStartup.hasData())
	{
		// FIXME , this should be a QString
		KviKvsScript::run(m_szExecAfterStartup.ptr(),g_pFrame->firstConsole());
		m_szExecAfterStartup = "";
	}

	// auto connect to servers if needed
	if(g_pServerDataBase->autoConnectOnStartupServers() || g_pServerDataBase->autoConnectOnStartupNetworks())
	{
		autoConnectToServers();
	}

	if(KVI_OPTION_BOOL(KviOption_boolShowDockExtension))
		g_pFrame->executeInternalCommand(KVI_INTERNALCOMMAND_TRAYICON_SHOW);

	if(KVI_OPTION_BOOL(KviOption_boolStartupMinimized))
	{
		g_pFrame->showMinimized();
	} else {
		g_pFrame->show();
	}
}

void KviApp::destroyFrame()
{
	if(g_pFrame) g_pFrame->deleteLater();
	g_pActiveWindow = 0;
	quit();
}

bool KviApp::connectionExists(KviIrcConnection *cnn)
{
	KviPointerHashTableIterator<QString,KviWindow> it(*g_pGlobalWindowDict);

	while(it.current())
	{
		if(it.current()->connection() == cnn)return true;
		++it;
	}
	return false;
}

bool KviApp::windowExists(KviWindow *wnd)
{
	KviPointerHashTableIterator<QString,KviWindow> it(*g_pGlobalWindowDict);

	while(it.current())
	{
		if(it.current() == wnd)return true;
		++it;
	}
	return false;
}

unsigned int KviApp::windowCount()
{
	return g_pGlobalWindowDict->count();
}

KviConsole * KviApp::findConsole(QString &server,QString &nick)
{
	KviPointerHashTableIterator<QString,KviWindow> it(*g_pGlobalWindowDict);

	while(it.current())
	{
		if(it.current()->type() == KVI_WINDOW_TYPE_CONSOLE)
		{
			if(((KviConsole *)it.current())->isConnected())
			{
				if(!server.isEmpty())
				{
					if(KviQString::equalCI(server,
						((KviConsole *)it.current())->connection()->currentServerName()))
					{
						if(!nick.isEmpty())
						{
							if(KviQString::equalCI(nick,
								((KviConsole *)it.current())->connection()->currentNickName()))
									return ((KviConsole *)it.current());
						} else return ((KviConsole *)it.current());
					}
				} else {
					if(!nick.isEmpty())
					{
						if(KviQString::equalCI(nick,
							((KviConsole *)it.current())->connection()->currentNickName()))
								return ((KviConsole *)it.current());
					}
				}
			}
		}
		++it;
	}
	return 0;
}

void KviApp::restartLagMeters()
{
	KviPointerHashTableIterator<QString,KviWindow> it(*g_pGlobalWindowDict);

	while(it.current())
	{
		if(it.current()->type() == KVI_WINDOW_TYPE_CONSOLE)
		{
			if(((KviConsole *)it.current())->connection())
				((KviConsole *)it.current())->connection()->restartLagMeter();
		}
		++it;
	}
}

void KviApp::restartNotifyLists()
{
	KviPointerHashTableIterator<QString,KviWindow> it(*g_pGlobalWindowDict);

	while(it.current())
	{
		if(it.current()->type() == KVI_WINDOW_TYPE_CONSOLE)
		{
			if(((KviConsole *)it.current())->connection())
				((KviConsole *)it.current())->connection()->restartNotifyList();
		}
		++it;
	}
}

void KviApp::resetAvatarForMatchingUsers(KviRegisteredUser * u)
{
	KviPointerHashTableIterator<QString,KviWindow> it(*g_pGlobalWindowDict);

	while(it.current())
	{
		if(it.current()->type() == KVI_WINDOW_TYPE_CONSOLE)
		{
			((KviConsole *)it.current())->resetAvatarForMatchingUsers(u);
		}
		++it;
	}
}

KviConsole * KviApp::findConsole(unsigned int ircContextId)
{
	KviPointerHashTableIterator<QString,KviWindow> it(*g_pGlobalWindowDict);

	while(it.current())
	{
		if(it.current()->type() == KVI_WINDOW_TYPE_CONSOLE)
		{
			if(((KviConsole *)it.current())->context()->id() == ircContextId)
				return ((KviConsole *)it.current());
		}
		++it;
	}
	return 0;
}


KviConsole * KviApp::topmostConnectedConsole()
{
	// check the foreground window console

	KviConsole * c = activeConsole();
	if(!c)return 0;
	if(c->isConnected())return c;

	// try ANY connected console

	KviPointerHashTableIterator<QString,KviWindow> it(*g_pGlobalWindowDict);

	while(it.current())
	{
		if(it.current()->type() == KVI_WINDOW_TYPE_CONSOLE)
		{
			if(((KviConsole *)it.current())->isConnected())return (KviConsole *)(it.current());
		}
		++it;
	}

	return 0;
}

KviWindow * KviApp::findWindow(const QString &windowId)
{
	return g_pGlobalWindowDict->find(windowId);
}

KviWindow * KviApp::findWindowByCaption(const QString &windowCaption,int iContextId)
{
	KviPointerHashTableIterator<QString,KviWindow> it(*g_pGlobalWindowDict);

	while(it.current())
	{
		if(KviQString::equalCI(windowCaption,it.current()->plainTextCaption()) &&
			(iContextId==-1 || it.current()->context()->id() == (uint) iContextId))
				return it.current();
		++it;
	}
	return 0;
}

void KviApp::registerWindow(KviWindow *wnd)
{
	g_pGlobalWindowDict->insert(wnd->id(),wnd);
}

void KviApp::unregisterWindow(KviWindow *wnd)
{
	g_pGlobalWindowDict->remove(wnd->id());
}


KviConsole * KviApp::activeConsole()
{
	if(!g_pFrame)return 0;
	if(g_pActiveWindow)
	{
		if(g_pActiveWindow->console()) {
			return g_pActiveWindow->console();
		}
	}
	return g_pFrame->firstConsole();
}


/////////////////// RECENT STUFF LISTS & POPUPS HANDLING

// Helper for KviApp::addRecent*()
static void merge_to_stringlist_option(const QString &item,int iOption,int iMaxEntries)
{
	for(QStringList::Iterator it = KVI_OPTION_STRINGLIST(iOption).begin();
			it != KVI_OPTION_STRINGLIST(iOption).end(); ++it)
	{
		// Do a case-insensitive search (for nicknames, servers, and channels)
		if(!QString::compare(item.toLower(),(*it).toLower()))
		{
		// In the recent list, remove and put as first so more recent items
		// are always first
			it = KVI_OPTION_STRINGLIST(iOption).erase(it);
			--it;
		}
	}
	while(KVI_OPTION_STRINGLIST(iOption).count() >= iMaxEntries)
	{
		KVI_OPTION_STRINGLIST(iOption).erase(KVI_OPTION_STRINGLIST(iOption).isEmpty()?KVI_OPTION_STRINGLIST(iOption).end():--KVI_OPTION_STRINGLIST(iOption).end());
	}
	KVI_OPTION_STRINGLIST(iOption).prepend(item);
}

void KviApp::addRecentUrl(const QString& text)
{
	merge_to_stringlist_option(text,KviOption_stringlistRecentIrcUrls,50);
	emit(recentUrlsChanged());
}

void KviApp::addRecentNickname(const QString& newNick)
{
	merge_to_stringlist_option(newNick,KviOption_stringlistRecentNicknames,KVI_MAX_RECENT_NICKNAMES);
}

void KviApp::addRecentChannel(const QString& szChan,const QString& net)
{
	if(!m_pRecentChannelDict)
		buildRecentChannels();

	QStringList * pList = m_pRecentChannelDict->find(net);
	if(!pList)
	{
		pList = new QStringList(szChan);
		m_pRecentChannelDict->insert(net,pList);
	}

	if(!pList->contains(szChan))
		pList->append(szChan);
}


void KviApp::buildRecentChannels()
{
	if(m_pRecentChannelDict)
		delete m_pRecentChannelDict;

	m_pRecentChannelDict = new KviPointerHashTable<QString,QStringList>;
	m_pRecentChannelDict->setAutoDelete(TRUE);

	QString szChan,szNet;

	for (
		QStringList::Iterator it = KVI_OPTION_STRINGLIST(KviOption_stringlistRecentChannels).begin();
		it != KVI_OPTION_STRINGLIST(KviOption_stringlistRecentChannels).end();
		++it
	)
	{
		if((*it).isEmpty())
			continue;
			
		szChan = (*it).section(KVI_RECENT_CHANNELS_SEPARATOR,0,0);
		szNet = (*it).section(KVI_RECENT_CHANNELS_SEPARATOR,1);

		if(szNet.isEmpty())
			continue;
			
		QStringList* pList = m_pRecentChannelDict->find(szNet);
		if(!pList)
		{
			pList=new QStringList(szChan);
			m_pRecentChannelDict->insert(szNet,pList);
		}

		if(!pList->contains(szChan))
			pList->append(szChan);
	}
}

void KviApp::saveRecentChannels()
{
	if(!m_pRecentChannelDict)
		return;

	QString szTmp;

	KVI_OPTION_STRINGLIST(KviOption_stringlistRecentChannels).clear();

	KviPointerHashTableIterator<QString,QStringList> it( *m_pRecentChannelDict );

	for( ; it.current(); ++it )
	{
		for(QStringList::Iterator it_str = it.current()->begin(); it_str != it.current()->end(); ++it_str)
		{
			szTmp=*it_str;
			szTmp.append(KVI_RECENT_CHANNELS_SEPARATOR);
			szTmp.append(it.currentKey());
			KVI_OPTION_STRINGLIST(KviOption_stringlistRecentChannels).append(szTmp);
		}
	}
}

QStringList * KviApp::recentChannelsForNetwork(const QString& net)
{
	if(!m_pRecentChannelDict)
		buildRecentChannels();
	return m_pRecentChannelDict->find(net);
}

void KviApp::addRecentServer(const QString& server)
{
	merge_to_stringlist_option(server,KviOption_stringlistRecentServers,KVI_MAX_RECENT_SERVERS);
}

void KviApp::fillRecentServersPopup(KviTalPopupMenu * m)
{
// FIXME: #warning "MAYBE DISABLE THE SERVERS THAT WE ARE ALREADY CONNECTED TO ?"
	m->clear();
	for(QStringList::Iterator it = KVI_OPTION_STRINGLIST(KviOption_stringlistRecentServers).begin(); it != KVI_OPTION_STRINGLIST(KviOption_stringlistRecentServers).end(); ++it)
	{
		if(*it == "") continue;
		m->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SERVER)),*it);
	}
}

void KviApp::fillRecentNicknamesPopup(KviTalPopupMenu * m,KviConsole * pConsole)
{
	m->clear();
	int id;
	bool bAlreadyFound = false;
	for(QStringList::Iterator it = KVI_OPTION_STRINGLIST(KviOption_stringlistRecentNicknames).begin(); it != KVI_OPTION_STRINGLIST(KviOption_stringlistRecentNicknames).end(); ++it)
	{
		if(*it == "") continue;
		id = m->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NICK)),*it);
		if(!pConsole->isConnected())m->setItemEnabled(id,false);
		else {
			if(!bAlreadyFound)
			{
				bool bIsCurrent = KviQString::equalCS(pConsole->connection()->currentNickName(),*it);
				m->setItemEnabled(id,!bIsCurrent);
				if(bIsCurrent)bAlreadyFound = true;
			}
		}
	}
}

void KviApp::fillRecentChannelsPopup(KviTalPopupMenu * m,KviConsole * pConsole)
{
	m->clear();
	int id;
	QStringList* pList = recentChannelsForNetwork(pConsole->currentNetworkName());
	if(pList)
	{
		for(QStringList::Iterator it = pList->begin(); it != pList->end(); ++it)
		{
			if(*it == "")
				continue; // ?
			id = m->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CHANNEL)),*it);
			if(!pConsole->isConnected())
				m->setItemEnabled(id,false);
			else
				m->setItemEnabled(id,!(pConsole->connection()->findChannel(*it)));
		}
	}
}


/*
void KviApp::fillRecentServersListBox(KviTalListBox * l)
{
	l->clear();
	for(QStringList::Iterator it = KVI_OPTION_STRINGLIST(KviOption_stringlistRecentServers).begin(); it != KVI_OPTION_STRINGLIST(KviOption_stringlistRecentServers).end(); ++it)
		l->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SERVER)),*it);
}
*/

//#include <QPixmapCache>

void KviApp::heartbeat(kvi_time_t tNow)
{
	const struct tm *pTm = localtime(&tNow);

	if(g_pApp->topmostConnectedConsole())
	{
		// FIXME: this has huge precision problems...
		KVI_OPTION_UINT(KviOption_uintTotalConnectionTime)++;
	}

	if (pTm && !pTm->tm_hour && !pTm->tm_min && !pTm->tm_sec)
	{
		KviPointerHashTableIterator<QString,KviWindow> it(*g_pGlobalWindowDict);
		while(it.current()) {
			if (it.current()->view() && it.current()->view()->isLogging())
				it.current()->view()->startLogging(0);
			++it;
		}
	}

	//qDebug("Clearing Qt pixmap cache...");
	//QPixmapCache::clear();
}

void KviApp::timerEvent(QTimerEvent *e)
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

// qvariant.h uses this , and it is included by the qt generated moc file for Qt >= 3.0.0
#ifdef Bool
	#undef Bool
#endif


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_app.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
