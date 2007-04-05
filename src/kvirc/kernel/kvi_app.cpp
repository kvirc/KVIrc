//=============================================================================
//
//   File : kvi_app.cpp
//   Creation date : Sun Jun 18 2000 12:39:45 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#define __KVIRC__

#define _KVI_APP_CPP_

#include "kvi_socket.h"
#include "kvi_app.h"
#include "kvi_debug.h"
#include "kvi_frame.h"
#include "kvi_msgbox.h"
#include "kvi_iconmanager.h"
#include "kvi_input.h"
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
#include "kvi_taskbar.h"
#include "kvi_defaults.h"
#include "kvi_locale.h"
#include "kvi_out.h"
#include "kvi_splash.h"
#include "kvi_nickserv.h"
#include "kvi_xlib.h"
#include "kvi_garbage.h"
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
#include "kvi_doublebuffer.h"
#include "kvi_stringconversion.h"
#include "kvi_useridentity.h"

#ifndef COMPILE_NO_IPC
	#include "kvi_ipc.h"
#endif

#include "kvi_kvs_script.h"
#include "kvi_kvs_popupmanager.h"
#include "kvi_kvs_kernel.h"
#include "kvi_kvs_object_controller.h"
#include "kvi_kvs_eventtriggers.h"
#include "kvi_sourcesdate.h"

#include "kvi_tal_popupmenu.h"
#include <qsplitter.h>
#include <qstringlist.h>
#include "kvi_asciidict.h"
#include <qmime.h>
#ifdef COMPILE_USE_QT4
	#include <q3mimefactory.h>
#endif
#include "kvi_tal_listbox.h"
#include <qclipboard.h>
#include <qmessagebox.h>
#include <qtextcodec.h>
#include <qmetaobject.h>

#ifdef COMPILE_SSL_SUPPORT
	#include "kvi_ssl.h"
#endif



KVIRC_API KviApp                       * g_pApp                    = 0; // global application pointer

KviConfig                              * g_pWinPropertiesConfig    = 0;
KVIRC_API KviIrcServerDataBase         * g_pIrcServerDataBase      = 0;
KVIRC_API KviProxyDataBase             * g_pProxyDataBase          = 0;

// Global windows

KVIRC_API KviColorWindow               * g_pColorWindow               = 0;
KVIRC_API KviTextIconWindow            * g_pTextIconWindow            = 0;
KVIRC_API KviTalPopupMenu              * g_pInputPopup                = 0;
KVIRC_API QStringList                  * g_pRecentTopicList           = 0;
//KVIRC_API QStringList                  * g_pBookmarkList            = 0;
KVIRC_API KviAsciiDict<KviWindow>      * g_pGlobalWindowDict          = 0;
KVIRC_API KviMediaManager              * g_pMediaManager              = 0;
//KVIRC_API KviRegisteredUserDataBase    * g_pRegisteredUserDataBase    = 0;
KVIRC_API KviSharedFilesManager        * g_pSharedFilesManager        = 0;
KVIRC_API KviNickServRuleSet           * g_pNickServRuleSet           = 0;
//KVIRC_API KviTimerManager              * g_pTimerManager              = 0;
KVIRC_API KviGarbageCollector          * g_pGarbageCollector          = 0;
KVIRC_API KviCtcpPageDialog            * g_pCtcpPageDialog            = 0;
KVIRC_API KviRegisteredChannelDataBase * g_pRegisteredChannelDataBase = 0;
KVIRC_API KviInputHistory              * g_pInputHistory              = 0;
KVIRC_API KviHistoryWindow             * g_pHistoryWindow             = 0;


KVIRC_API int                            g_iIdentDaemonRunningUsers   = 0; // this is eventually set by libkviident

KVIRC_API KviSplashScreen       * g_pSplashScreen           = 0;


// Loaded and destroyed by KviIconManager
QPixmap                             * g_pUserChanStatePixmap    = 0;
QPixmap                             * g_pActivityMeterPixmap    = 0;


#ifdef COMPILE_PSEUDO_TRANSPARENCY

	#ifdef COMPILE_KDE_SUPPORT
		#include <ksharedpixmap.h>
		#include <netwm.h>
		#include <kimageeffect.h>
		#include <dcopclient.h>
		#include <qdatastream.h>
		#include <qcstring.h>

		KSharedPixmap         * g_pKdeDesktopBackground             = 0; // the shared pixmap that we get from KWin
	#endif

	#include <qimage.h>

	KVIRC_API QPixmap               * g_pShadedParentGlobalDesktopBackground = 0; // the pixmap that we use for MdiManager
	KVIRC_API QPixmap               * g_pShadedChildGlobalDesktopBackground  = 0; // the pixmap that we use for MdiChild
#endif

#ifdef COMPILE_CRYPT_SUPPORT
	#include "kvi_crypt.h"
	KVIRC_API KviCryptEngineManager * g_pCryptEngineManager = 0; // global crypt engine manager
#endif

KviApp::KviApp(int &argc,char ** argv)
: KviTalApplication(argc,argv)
{
	// Ok...everything begins here
	g_pApp                  = this;
	m_szConfigFile          = QString::null;
	m_bCreateConfig         = false;
	m_bShowSplashScreen     = true;
	m_bUpdateGuiPending     = false;
	m_pPendingAvatarChanges = 0;
	m_pRecentChannelsDict   = 0;
	m_pIpcSentinel          = 0;
	m_iHeartbeatTimerId     = -1;
	// next step is setup()
	m_bSetupDone = false;
	kvi_socket_flushTrafficCounters();
}

void KviApp::setup()
{
	// Here we start up all the substystems.
	// This is a really critical phase since in general subsystems depend
	// on each other and we must activate them in the right order.
	// Don't move stuff around unless you really know what you're doing.

#ifdef COMPILE_ON_WINDOWS
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

	// Set the default help files search path
	QStringList list;
	QString tmp;
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
#ifdef COMPILE_USE_QT4
	Q3MimeSourceFactory::defaultFactory()->setFilePath(list);
#else
	QMimeSourceFactory::defaultFactory()->setFilePath(list);
#endif

	KVI_SPLASH_SET_PROGRESS(1)

	// Initialize the scripting engine
	KviKvs::init();

	KVI_SPLASH_SET_PROGRESS(2)

	// Initialize the action manager
	KviActionManager::init();

	KVI_SPLASH_SET_PROGRESS(3)

	// Create the module manager early: so the other managers can load modules
	g_pModuleExtensionManager = new KviModuleExtensionManager();
	g_pModuleManager = new KviModuleManager();

	KVI_SPLASH_SET_PROGRESS(4);

	g_pIconManager = new KviIconManager();

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
	g_pIrcServerDataBase   = new KviIrcServerDataBase();
	if(getReadOnlyConfigPath(tmp,KVI_CONFIGFILE_SERVERDB))
		g_pIrcServerDataBase->load(tmp);

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
	// THIS IS A COMPATIBILITY ENTRY ADDED AT 3.0.2 TIME THAT SHOULD BE DROPPED IN A COUPLE OF VERSION BUMPS!
	getLocalKvircDirectory(tmp,Config,KVI_CONFIGFILE_SCRIPTTOOLBARS);
	if(KviFileUtils::fileExists(tmp))
		KviCustomToolBarManager::instance()->loadScripttoolbarsCompat(tmp);
	// EOF COMPATIBILITY

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

	KVI_SPLASH_SET_PROGRESS(88)

	g_pInputHistory = new KviInputHistory();
	if(getReadOnlyConfigPath(tmp,KVI_CONFIGFILE_INPUTHISTORY))
		g_pInputHistory->load(tmp);

	KVI_SPLASH_SET_PROGRESS(89)
	KviAvatarCache::init();
	if(getReadOnlyConfigPath(tmp,KVI_CONFIGFILE_AVATARCACHE))
		KviAvatarCache::instance()->load(tmp);

	KVI_SPLASH_SET_PROGRESS(90)

	// Eventually initialize the crypt engine manager
#ifdef COMPILE_CRYPT_SUPPORT
	g_pCryptEngineManager = new KviCryptEngineManager();
#endif

	g_pGarbageCollector = new KviGarbageCollector();

	// and the input popup
	g_pInputPopup = new KviTalPopupMenu();

	// create the server parser
	g_pServerParser = new KviServerParser();

	KVI_SPLASH_SET_PROGRESS(91)

	// Global window dictionary
	g_pGlobalWindowDict = new KviAsciiDict<KviWindow>(41);
	g_pGlobalWindowDict->setAutoDelete(false);
	// Script object controller
	//g_pScriptObjectController = new KviScriptObjectController(); gone

	KVI_SPLASH_SET_PROGRESS(92)

	KviDoubleBuffer::init();

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

	if(g_pSplashScreen)delete g_pSplashScreen;
	if(g_pCtcpPageDialog)delete g_pCtcpPageDialog;

	// if we still have a frame: kill it
	if(g_pFrame)delete g_pFrame;
	g_pActiveWindow = 0; // .. but it should be already 0 anyway

	// execute pending deletes (this may still contain some UI elements)
	delete g_pGarbageCollector;

	if(!m_bSetupDone)return; // killed with IPC (nothing except the m_pFrameList was created yet)

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
	if(m_pRecentChannelsDict) delete m_pRecentChannelsDict;
	// now kill the stuff that the frame depends on
	saveIrcServerDataBase();
	delete g_pIrcServerDataBase;
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
	delete g_pInputHistory;
	delete g_pInputPopup;
	//delete g_pScriptObjectController;
#ifdef COMPILE_CRYPT_SUPPORT
	delete g_pCryptEngineManager;
#endif
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	destroyPseudoTransparency();
#endif
	KviDoubleBuffer::done();
	if(m_pPendingAvatarChanges)delete m_pPendingAvatarChanges;
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

#ifdef COMPILE_ON_WINDOWS
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
		c = KviLocale::codecForName(KVI_OPTION_STRING(KviOption_stringDefaultTextEncoding).latin1());
		if(c)return c;
	}
	c = QTextCodec::codecForLocale();
	if(c)return c;
	c = KviLocale::codecForName("UTF-8");
	if(!c)debug("KviApp::defaultTextCodec(): cannot find a suitable text codec for locale :/");
	return c;
}

void KviApp::contextSensitiveHelp()
{
	// this stuff doesn't work with Qt 4.x
#ifndef COMPILE_USE_QT4
	// the F1 Key has been pressed
	// try to pass it to the active widget or one of its parents
	QWidget * w = g_pApp->focusWidget();
	while(w)
	{
		//QVariant v = w->property("contextSensitiveHelp"); <-- this prints a lot of warnings: this is a QT BUG
		QMetaObject * o = w->metaObject();
		if(o)
		{
			int i = o->findProperty("contextSensitiveHelp",true);
			if(i != -1)
			{
				QVariant v = w->property("contextSensitiveHelp");
				if(v.isValid() && v.toBool())
					return; // done!
			}
		}
		w = w->parentWidget();
	}
	// no way
	// FIXME: just show up simple plain online help
	//debug("No way: found no focus widget for that...");
#endif
}


void KviApp::collectGarbage(QObject * garbage)
{
//	if(!g_pGarbageCollector)debug("Ops... no garbage collector ?");
	g_pGarbageCollector->collect(garbage);
}

void KviApp::loadDefaultScript()
{
	QString cmd;
	getGlobalKvircDirectory(cmd,KviApp::DefScript,"default.kvs");
	cmd.prepend("parse \"");
	cmd.append("\"");
#ifdef COMPILE_ON_WINDOWS
	cmd.replace("\\","\\\\");
#endif
	KviKvsScript::run(cmd,g_pFrame->firstConsole());
	// now line up the toolbars (they may get messed while loading the script)
#ifndef COMPILE_USE_QT4
	g_pFrame->lineUpDockWindows(); // missing on Qt 4.x
#endif
}

// 07.01.2005 06:01: Got this curious gcc error while writing
//                   the functions below...
//                   The problem is gone by simply typing make again.
//                   Memory/Disk transfer error ?
//
// if g++ -DHAVE_CONFIG_H -I. -I. -I../../../src  -I/usr/build/kvirccvs/kvirc/src/kvilib/include/ -I/usr/build/kvirccvs/kvirc/src/kvirc/include/ -I/opt/qt/include -I/usr/X11R6/include -I/usr/X11R6/include -I/opt/kde/include -D_REENTRANT -DREENTRANT  -I/usr/X11R6/include -DGLOBAL_KVIRC_DIR=\"/usr/local/share/kvirc/3.0.1.99\"   -g -MT kvi_ircconnection.o -MD -MP -MF ".deps/kvi_ircconnection.Tpo"
//  -c -o kvi_ircconnection.o `test -f '../kernel/kvi_ircconnection.cpp' || echo './'`../kernel/kvi_ircconnection.cpp;
// then mv -f ".deps/kvi_ircconnection.Tpo" ".deps/kvi_ircconnection.Po";
// else rm -f ".deps/kvi_ircconnection.Tpo"; exit 1;
// fi
// /opt/qt/include/qstringlist.h:62: sorry, not implemented: `
//   expr_with_file_location' not supported by dump_decl
// /opt/qt/include/qstringlist.h: In function `<declaration error>':
// /opt/qt/include/qstringlist.h:62: internal error: Segmentation fault
// Please submit a full bug report,
// with preprocessed source if appropriate.
// See <URL:http://bugzilla.redhat.com/bugzilla/> for instructions.


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
				"This may happen because you have upgraded to an unstable cvs version, " \
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


QString KviApp::getClipboardText()
{
	/*
	QString buffer;
#if QT_VERSION >= 300
	bool bOldMode = clipboard()->selectionModeEnabled();
	clipboard()->setSelectionMode(false);
#endif
	buffer = clipboard()->text();
#if QT_VERSION >= 300
	if(buffer.isEmpty())
	{
		// lookup the global clipboard
		clipboard()->setSelectionMode(true);
		buffer = clipboard()->text();
	}
	clipboard()->setSelectionMode(bOldMode);
#endif
	return buffer;
	*/
	
	QString buffer = clipboard()->text(QClipboard::Clipboard);
	if(buffer.isEmpty())return clipboard()->text(QClipboard::Selection);
	return buffer;
}


void KviApp::getClipboardText(KviStr &buffer)
{
	buffer = getClipboardText();
}

void KviApp::setClipboardText(const QString &str)
{
	/*
#if QT_VERSION >= 300
	if(clipboard()->supportsSelection())
	{
		bool bOldMode = clipboard()->selectionModeEnabled();
		clipboard()->setSelectionMode(true);
		clipboard()->setText(str);
		clipboard()->setSelectionMode(false);
		clipboard()->setText(str);
		clipboard()->setSelectionMode(bOldMode);
	} else {
#endif
		clipboard()->setText(str);
#if QT_VERSION >= 300
	}
#endif*/
	clipboard()->setText(str,QClipboard::Clipboard);
	clipboard()->setText(str,QClipboard::Selection);
}

void KviApp::setClipboardText(const KviStr &str)
{
	debug("WARNING : KviApp::setClipboardText(const KviStr &) is deprecated!");
	setClipboardText(QString(str.ptr()));
}

void KviApp::setAvatarFromOptions()
{
	KviAsciiDictIterator<KviWindow> it(*g_pGlobalWindowDict);

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
		m_pPendingAvatarChanges = new KviPtrList<KviPendingAvatarChange>;
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

	if(m_pPendingAvatarChanges)p = findPendingAvatarChange(0,szNick,szRemoteUrl);
	else p = 0;

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
			notifierMessage(0,iIconId,szMsg,30);
		}
		return;
	}

	if(bSuccess)
	{
		if(windowExists(p->pConsole))
		{
			p->pConsole->setAvatar(p->szNick,p->szUser,p->szHost,
				szLocalFileName,KviQString::equalCIN("http://",szRemoteUrl,7) ? szRemoteUrl : QString::null);
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

	#ifdef COMPILE_KDE_SUPPORT

        #include <netwm.h>

		void KviApp::downloadKdeRootPixmap()
		{
			if(g_pKdeDesktopBackground)
			{
				delete g_pKdeDesktopBackground;
                g_pKdeDesktopBackground = 0;
				// this signal shouldn't be connected ,but well.. let's make sure
				disconnect(this,SIGNAL(backgroundChanged(int)),this,SLOT(kdeRootPixmapChanged(int)));
			}
			g_pKdeDesktopBackground = 0;

			// This has been extracted from the KDELIBS KRootPixmap interface
#if 0
	#warning "The following warning will disappear soon..."
			// FIXME: this form should be used starting from KDE 3.2
			unsigned long l = NET::CurrentDesktop;
			NETRootInfo rinfo( qt_xdisplay(), &l ,1 ,-1,true);
#else
			NETRootInfo rinfo(qt_xdisplay(),NET::CurrentDesktop);
#endif
			rinfo.activate();

		    QString name = QString("DESKTOP%1").arg(rinfo.currentDesktop());

			g_pKdeDesktopBackground = new KSharedPixmap();
			connect(g_pKdeDesktopBackground,SIGNAL(done(bool)),this,SLOT(kdeRootPixmapDownloadComplete(bool)));

		    if(!(g_pKdeDesktopBackground->isAvailable(name)))
			{
				// Pixmap not available!!!
				delete g_pKdeDesktopBackground;
				g_pKdeDesktopBackground = 0;
				if(m_bUpdatePseudoTransparencyPending)return; // Already sent a request....
				// Send a request to kdesktop....
				DCOPClient *client = kapp->dcopClient();
				if(!client->isAttached())client->attach();
				QByteArray data;
				QDataStream args(data,IO_WriteOnly);
				args << 1;
				client->send("kdesktop","KBackgroundIface","setExport(int)",data);
				m_bUpdatePseudoTransparencyPending = true;
				// Now wait a while and try again....
				QTimer::singleShot(500,this,SLOT(updatePseudoTransparency()));
				return;
			}

			if(!g_pKdeDesktopBackground->loadFromShared(QString("DESKTOP%1").arg(rinfo.currentDesktop())))
			{
				debug("Can't load the KDE root background image...");
				delete g_pKdeDesktopBackground;
				g_pKdeDesktopBackground = 0;
			} //else {
				// debug("Root pixmap downalod initiated");
			//}
		}

	#endif //COMPILE_KDE_SUPPORT

	void KviApp::destroyPseudoTransparency()
	{
#ifdef COMPILE_KDE_SUPPORT
		if(g_pKdeDesktopBackground)
		{
			delete g_pKdeDesktopBackground;
			g_pKdeDesktopBackground = 0;
		}
		// forget the backgroundChanged signal (will do nothing if it is not connected)
		disconnect(this,SIGNAL(backgroundChanged(int)),this,SLOT(kdeRootPixmapChanged(int)));
#endif //COMPILE_KDE_SUPPORT
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

#ifdef COMPILE_KDE_SUPPORT

	#define kimageeffect_fade KImageEffect::fade

#else //!COMPILE_KDE_SUPPORT

	//
	// This function is taken from the KDE kimageeffect.cpp
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

#endif //!COMPILE_KDE_SUPPORT

	void KviApp::createGlobalBackgrounds(QPixmap * pix)
	{
		// create shaded copies...
		if(g_pShadedParentGlobalDesktopBackground)delete g_pShadedParentGlobalDesktopBackground;
		if(g_pShadedChildGlobalDesktopBackground)delete g_pShadedChildGlobalDesktopBackground;
		g_pShadedParentGlobalDesktopBackground = new QPixmap();
		g_pShadedChildGlobalDesktopBackground = new QPixmap();
		QImage img = pix->convertToImage();
		// play with the fade factors
		KVI_OPTION_UINT(KviOption_uintGlobalTransparencyParentFadeFactor) %= 100;
		if(KVI_OPTION_UINT(KviOption_uintGlobalTransparencyParentFadeFactor) > 0)
		{
			g_pShadedParentGlobalDesktopBackground->convertFromImage(
				kimageeffect_fade(img,
					(float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyParentFadeFactor) / (float)100),
					KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade)),0);
		}
		KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) %= 100;
		if(KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) > 0)
		{
			g_pShadedChildGlobalDesktopBackground->convertFromImage(
				kimageeffect_fade(img,
					(float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) / (float)100),
					KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade)),0);
		}
		if(g_pFrame)g_pFrame->updatePseudoTransparency();
	}

#endif //COMPILE_PSEUDO_TRANSPARENCY

void KviApp::kdeRootPixmapChanged(int iDesktop)
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	#ifdef COMPILE_KDE_SUPPORT
		if(!KVI_OPTION_BOOL(KviOption_boolUpdateKdeBackgroundOnChange))return;
		NETRootInfo rinfo(qt_xdisplay(),NET::CurrentDesktop);
		rinfo.activate();
		if(iDesktop == rinfo.currentDesktop())
			updatePseudoTransparency();
	#endif
#endif //COMPILE_PSEUDO_TRANSPARENCY
}

void KviApp::kdeRootPixmapDownloadComplete(bool bSuccess)
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	#ifdef COMPILE_KDE_SUPPORT
		if(!bSuccess)
		{
			debug("Failed to download the KDE root background image...");
		} else {
			// downloaded!
			// create shaded copies...
			createGlobalBackgrounds(g_pKdeDesktopBackground);
			// make sure that this signal is not connected twice
			disconnect(this,SIGNAL(backgroundChanged(int)),this,SLOT(kdeRootPixmapChanged(int)));
			// connect it
			connect(this,SIGNAL(backgroundChanged(int)),this,SLOT(kdeRootPixmapChanged(int)));
		}
		delete g_pKdeDesktopBackground;
		g_pKdeDesktopBackground = 0;
	#endif //COMPILE_KDE_SUPPORT
#endif //COMPILE_PSEUDO_TRANSPARENCY
}


void KviApp::updatePseudoTransparency()
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	m_bUpdatePseudoTransparencyPending = false;
	if(KVI_OPTION_BOOL(KviOption_boolUseGlobalPseudoTransparency))
	{
#ifdef COMPILE_KDE_SUPPORT
		if(KVI_OPTION_BOOL(KviOption_boolObtainGlobalBackgroundFromKde))
		{
			downloadKdeRootPixmap();
		} else {
#endif //COMPILE_KDE_SUPPORT
			if(KVI_OPTION_PIXMAP(KviOption_pixmapGlobalTransparencyBackground).pixmap())
			{
				createGlobalBackgrounds(KVI_OPTION_PIXMAP(KviOption_pixmapGlobalTransparencyBackground).pixmap());
			} else {
				destroyPseudoTransparency();
				KVI_OPTION_BOOL(KviOption_boolUseGlobalPseudoTransparency) = false;
			}
#ifdef COMPILE_KDE_SUPPORT
		}
#endif //COMPILE_KDE_SUPPORT
	} else {
		destroyPseudoTransparency();
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
		if(font() != KVI_OPTION_FONT(KviOption_fontApplication))
			setFont(KVI_OPTION_FONT(KviOption_fontApplication),true);
	}
	// FIXME: #warning "And what if this option is turned off ?...a reboot only"
}

void KviApp::loadRecentEntries()
{
	QString tmp;
	getLocalKvircDirectory(tmp,Config,KVI_CONFIGFILE_RECENT);
	KviConfig cfg(tmp,KviConfig::Read);
	*g_pRecentTopicList = cfg.readStringListEntry("RecentTopicList",QStringList());
	//*g_pBookmarkList = cfg.readStringListEntry("Bookmarks",QStringList());
}
void KviApp::saveRecentEntries()
{
	QString tmp;
	getLocalKvircDirectory(tmp,Config,KVI_CONFIGFILE_RECENT);
	KviConfig cfg(tmp,KviConfig::Write);
	cfg.writeEntry("RecentTopicList",*g_pRecentTopicList);
	//cfg.writeEntry("Bookmarks",*g_pBookmarkList);
}

void KviApp::saveAvatarCache()
{
	QString tmp;
	getLocalKvircDirectory(tmp,Config,KVI_CONFIGFILE_AVATARCACHE);
	KviAvatarCache::instance()->save(tmp);
}


void KviApp::saveToolBars()
{
	QString tmp;
	getLocalKvircDirectory(tmp,Config,KVI_CONFIGFILE_CUSTOMTOOLBARS);
	KviCustomToolBarManager::instance()->save(tmp);
}

void KviApp::savePopups()
{
	QString tmp;
	getLocalKvircDirectory(tmp,Config,KVI_CONFIGFILE_POPUPS);
	KviKvs::savePopups(tmp);
}

void KviApp::saveInputHistory()
{
	if(!KVI_OPTION_BOOL(KviOption_boolDisableInputHistory))
	{
		QString tmp;
		getLocalKvircDirectory(tmp,Config,KVI_CONFIGFILE_INPUTHISTORY);
		g_pInputHistory->save(tmp);
	}
}


void KviApp::saveAliases()
{
	QString tmp;
	getLocalKvircDirectory(tmp,Config,KVI_CONFIGFILE_ALIASES);
	KviKvs::saveAliases(tmp);
}

void KviApp::saveScriptAddons()
{
	QString tmp;
	getLocalKvircDirectory(tmp,Config,KVI_CONFIGFILE_SCRIPTADDONS);
	KviKvs::saveScriptAddons(tmp);
}

void KviApp::saveTextIcons()
{
	g_pTextIconManager->save();
}

void KviApp::saveAppEvents()
{
	QString tmp;
	getLocalKvircDirectory(tmp,Config,KVI_CONFIGFILE_EVENTS);
	KviKvs::saveAppEvents(tmp);
}

void KviApp::saveRawEvents()
{
	QString tmp;
	getLocalKvircDirectory(tmp,Config,KVI_CONFIGFILE_RAWEVENTS);
	KviKvs::saveRawEvents(tmp);
}

void KviApp::saveMediaTypes()
{
	QString tmp;
	getLocalKvircDirectory(tmp,Config,KVI_CONFIGFILE_MEDIATYPES);
	g_pMediaManager->lock();
	g_pMediaManager->save(tmp);
	g_pMediaManager->unlock();
}

void KviApp::saveIrcServerDataBase()
{
	QString tmp;
	getLocalKvircDirectory(tmp,Config,KVI_CONFIGFILE_SERVERDB);
	g_pIrcServerDataBase->save(tmp);
}

void KviApp::saveProxyDataBase()
{
	QString tmp;
	getLocalKvircDirectory(tmp,Config,KVI_CONFIGFILE_PROXYDB);
	g_pProxyDataBase->save(tmp);
}

void KviApp::saveRegisteredUsers()
{
	QString tmp;
	getLocalKvircDirectory(tmp,Config,KVI_CONFIGFILE_REGUSERDB);
	g_pRegisteredUserDataBase->save(tmp);
}

void KviApp::saveRegisteredChannels()
{
	QString tmp;
	getLocalKvircDirectory(tmp,Config,KVI_CONFIGFILE_REGCHANDB);
	g_pRegisteredChannelDataBase->save(tmp);
}

void KviApp::saveNickServ()
{
	QString tmp;
	getLocalKvircDirectory(tmp,Config,KVI_CONFIGFILE_NICKSERVDATABASE);
	g_pNickServRuleSet->save(tmp);
}

void KviApp::saveSharedFiles()
{
	QString tmp;
	getLocalKvircDirectory(tmp,Config,KVI_CONFIGFILE_SHAREDFILES);
	g_pSharedFilesManager->save(tmp);
}

void KviApp::saveActions()
{
	QString tmp;
	getLocalKvircDirectory(tmp,Config,KVI_CONFIGFILE_USERACTIONS);
	KviActionManager::instance()->save(tmp);
}

void KviApp::saveIdentities()
{
	QString tmp;
	getLocalKvircDirectory(tmp,Config,KVI_CONFIGFILE_IDENTITIES);
	KviUserIdentityManager::instance()->save(tmp);
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
	KviPtrList<KviIrcServer> * l = g_pIrcServerDataBase->autoConnectOnStartupServers();
	if(l)
	{
		for(KviIrcServer * s = l->first();s;s = l->next())
		{
			QString szCommand = "server -u \"id:";
			if(s->id().isEmpty())s->generateUniqueId();
			szCommand += s->id();
			szCommand += "\"";
			KviKvsScript::run(szCommand,activeConsole());
		}
		g_pIrcServerDataBase->clearAutoConnectOnStartupServers();
	}
	
	KviPtrList<KviIrcServerDataBaseRecord> * lr = g_pIrcServerDataBase->autoConnectOnStartupNetworks();
	if(lr)
	{
		for(KviIrcServerDataBaseRecord * r = lr->first();r;r = lr->next())
		{
			QString szCommandx = "server -u \"net:";
			szCommandx += r->network()->name();
			szCommandx += "\"";
			KviKvsScript::run(szCommandx,activeConsole());
		}
		g_pIrcServerDataBase->clearAutoConnectOnStartupNetworks();
	}
}


void KviApp::createFrame()
{
	if(g_pFrame)debug("WARNING: Creating the main frame twice!");
	g_pFrame = new KviFrame();
	g_pFrame->createNewConsole(true);

	if(m_szExecAfterStartup.hasData())
	{
		// FIXME , this should be a QString
		KviKvsScript::run(m_szExecAfterStartup.ptr(),g_pFrame->firstConsole());
		m_szExecAfterStartup = "";
	}
	
	// auto connect to servers if needed
	if(g_pIrcServerDataBase->autoConnectOnStartupServers() || g_pIrcServerDataBase->autoConnectOnStartupNetworks())
	{
		autoConnectToServers();
	}

	if(KVI_OPTION_BOOL(KviOption_boolShowDockExtension))
		g_pFrame->executeInternalCommand(KVI_INTERNALCOMMAND_DOCKWIDGET_SHOW);
	
	if(KVI_OPTION_BOOL(KviOption_boolStartupMinimized))
	{
		if(KVI_OPTION_BOOL(KviOption_boolMinimizeInTray) && KVI_OPTION_BOOL(KviOption_boolShowDockExtension))
		{
			g_pFrame->hide();
		} else {
			g_pFrame->showMinimized();
		}
	} else {
		g_pFrame->show();
	}
}

void KviApp::destroyFrame()
{
	if(g_pFrame)delete g_pFrame;
	g_pActiveWindow = 0;
	quit();
}

bool KviApp::connectionExists(KviIrcConnection *cnn)
{
	KviAsciiDictIterator<KviWindow> it(*g_pGlobalWindowDict);

	while(it.current())
	{
		if(it.current()->connection() == cnn)return true;
		++it;
	}
	return false;
}


bool KviApp::windowExists(KviWindow *wnd)
{
	KviAsciiDictIterator<KviWindow> it(*g_pGlobalWindowDict);

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
	KviAsciiDictIterator<KviWindow> it(*g_pGlobalWindowDict);

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

KviConsole * KviApp::findConsole(KviStr &server,KviStr &nick)
{
	// DEPRECATED: TO BE KILLED (if nobody is using it)
	QString s = server.ptr();
	QString n = nick.ptr();
	return findConsole(s,n);
}

void KviApp::restartLagMeters()
{
	KviAsciiDictIterator<KviWindow> it(*g_pGlobalWindowDict);

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
	KviAsciiDictIterator<KviWindow> it(*g_pGlobalWindowDict);

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
	KviAsciiDictIterator<KviWindow> it(*g_pGlobalWindowDict);

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
	KviAsciiDictIterator<KviWindow> it(*g_pGlobalWindowDict);

	while(it.current())
	{
		if(it.current()->type() == KVI_WINDOW_TYPE_CONSOLE)
		{
			if(((KviConsole *)it.current())->ircContextId() == ircContextId)
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

	KviAsciiDictIterator<KviWindow> it(*g_pGlobalWindowDict);

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

KviWindow * KviApp::findWindow(const char * windowId)
{
	return g_pGlobalWindowDict->find(windowId);
}

KviWindow * KviApp::findWindowByCaption(const QString &windowCaption,int iContextId)
{
	KviAsciiDictIterator<KviWindow> it(*g_pGlobalWindowDict);

	while(it.current())
	{
		if(KviQString::equalCI(windowCaption,it.current()->plainTextCaption()) && 
			(iContextId==-1 || it.current()->context()->id() == iContextId))
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
		if(g_pActiveWindow->console())return g_pActiveWindow->console();
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
		if(!QString::compare(item.lower(),(*it).lower()))
		{
		// In the recent list, remove and put as first so more recent items
		// are always first
			it = KVI_OPTION_STRINGLIST(iOption).remove(it);
			--it;
		}
	}
	while(KVI_OPTION_STRINGLIST(iOption).count() >= (unsigned int)iMaxEntries)
	{
		KVI_OPTION_STRINGLIST(iOption).remove(KVI_OPTION_STRINGLIST(iOption).fromLast());
	}
	KVI_OPTION_STRINGLIST(iOption).prepend(item);
}

void KviApp::addRecentUrl(const QString& text)
{
	merge_to_stringlist_option(text,KviOption_stringlistRecentIrcUrls,50);
	emit(recentUrlsChanged());
}

void KviApp::addRecentNickname(const char * newNick)
{
	QString nk(newNick);
	merge_to_stringlist_option(nk,KviOption_stringlistRecentNicknames,KVI_MAX_RECENT_NICKNAMES);
}

void KviApp::addRecentNickname(const QString& newNick)
{
	
	merge_to_stringlist_option(newNick,KviOption_stringlistRecentNicknames,KVI_MAX_RECENT_NICKNAMES);
}

void KviApp::addRecentChannel(const QString& szChan,const QString& net)
{
	if(!m_pRecentChannelsDict)
		buildRecentChannels();
	QStringList* pList=m_pRecentChannelsDict->find(net.utf8().data());
	if(pList)
	{
		if(!pList->contains(szChan)) pList->append(szChan);
	}
	else
	{
		pList=new QStringList(szChan);
		m_pRecentChannelsDict->insert(net.utf8().data(),pList);
	}
}

void KviApp::buildRecentChannels()
{
	if(m_pRecentChannelsDict)
		delete m_pRecentChannelsDict;
	m_pRecentChannelsDict = new KviAsciiDict<QStringList>;
	m_pRecentChannelsDict->setAutoDelete(TRUE);
	QString szChan,szNet;
	for ( 
		QStringList::Iterator it = KVI_OPTION_STRINGLIST(KviOption_stringlistRecentChannels).begin();
		it != KVI_OPTION_STRINGLIST(KviOption_stringlistRecentChannels).end();
		++it
	) 
	{
		if(!(*it).isEmpty())
		{
			szChan = (*it).section( KVI_RECENT_CHANNELS_SEPARATOR, 0, 0 );
			szNet = (*it).section( KVI_RECENT_CHANNELS_SEPARATOR, 1 );
			if(!szNet.isEmpty())
			{
				QStringList* pList=m_pRecentChannelsDict->find(szNet.utf8().data());
				if(pList)
				{
					if(!pList->contains(szChan)) pList->append(szChan);
				}
				else
				{
					pList=new QStringList(szChan);
					m_pRecentChannelsDict->insert(szNet.utf8().data(),pList);
				}
			}
		}
	}
}

void KviApp::saveRecentChannels()
{
	if(!m_pRecentChannelsDict) return;
	QString szTmp;
	KVI_OPTION_STRINGLIST(KviOption_stringlistRecentChannels).clear();
	KviAsciiDictIterator<QStringList> it( *m_pRecentChannelsDict );
	for( ; it.current(); ++it )
	{
		for ( QStringList::Iterator it_str = it.current()->begin(); it_str != it.current()->end(); ++it_str ) {
			szTmp=*it_str;
			szTmp.append(KVI_RECENT_CHANNELS_SEPARATOR);
			szTmp.append(it.currentKey());
			KVI_OPTION_STRINGLIST(KviOption_stringlistRecentChannels).append(szTmp);
		}
        }
}

QStringList* KviApp::getRecentChannels(const QString& net)
{
	if(!m_pRecentChannelsDict) buildRecentChannels();
	return m_pRecentChannelsDict->find(net.utf8().data());
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
	QStringList* pList=getRecentChannels(pConsole->currentNetworkName());
	if(pList)
	{
		for(QStringList::Iterator it = pList->begin(); it != pList->end(); ++it)
		{
			id = m->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CHANNEL)),*it);
			if(!pConsole->isConnected())m->setItemEnabled(id,false);
			else {
				m->setItemEnabled(id,!(pConsole->connection()->findChannel(*it)));
			}
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




bool KviApp::playFile(const char * filename,KviStr &error,KviWindow * w)
{
	g_pMediaManager->lock();
	KviMediaType * m = g_pMediaManager->findMediaType(filename);
	if(m)
	{
		KviStr szCommandline = m->szCommandline;
		KviStr szDescription = m->szDescription;
		g_pMediaManager->unlock();
		if(szCommandline.hasData())
		{
			KviKvsVariantList l;
			l.append(new KviKvsVariant(QString(filename)));

			if(!KviKvsScript::run(szCommandline.ptr(),w ? w : g_pActiveWindow,&l))
			{
				error.sprintf(__tr("The commandline for media type '%s' seems to be broken"),szDescription.ptr());
				return false;
			}
		} else {
			error.sprintf(__tr("Media type of file %s matched to '%s' but no commandline specified"),filename,szDescription.ptr());
			return false;
		}
	} else {
		g_pMediaManager->unlock();
		error.sprintf(__tr("No idea on how to play file %s (no media type match)"),filename);
		return false;
	}
	return true;
}

void KviApp::heartbeat(kvi_time_t tNow)
{
	if(g_pApp->topmostConnectedConsole())
	{
		// FIXME: this has huge precision problems...
		KVI_OPTION_UINT(KviOption_uintTotalConnectionTime)++;
	}
	
	// the line below is an approximation of (tNow / 120) == 0
	// we don't need a really great precision here, so 128 is still ok
	if(!(tNow & 0x7f))
		KviDoubleBuffer::heartbeat();
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


#include "kvi_app.moc"
