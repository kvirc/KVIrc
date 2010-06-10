#ifndef _KVI_APP_H_
#define _KVI_APP_H_

//=============================================================================
//
//   File : kvi_app.h
//   Creation date : Sun Jun 18 2000 12:40:00 CEST by Szymon Stefanek
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

#include <QFont>
#include "kvi_settings.h"
#include "kvi_string.h"
#include "kvi_qstring.h"
#include "kvi_tal_application.h"
#include "kvi_pointerlist.h"
#include "kvi_time.h"
#include "kvi_pointerhashtable.h" // ?

#ifdef COMPILE_ON_WINDOWS
	// The brain damaged MSVC compiler can't instantiate templates without this
	#include "kvi_frame.h"
	#include "kvi_window.h"
#else // !COMPILE_ON_WINDOWS
	class KviFrame;
	class KviWindow;
#endif // !COMPILE_ON_WINDOWS

#ifndef COMPILE_NO_IPC
	class KviIpcSentinel;
#endif // !COMPILE_NO_IPC

#ifdef COMPILE_X11_SUPPORT
	#include <QX11Info>
#endif

class KviTalPopupMenu;
class QPixmap;
class KviTalListBox;
class QTextCodec;
class QDomElement;

class KviConsole;
class KviConfig;
class KviRegisteredUser;
class KviIrcConnection;
class KviServerDataBase;

typedef struct _KviPendingAvatarChange
{
	KviConsole * pConsole;
	QString      szRemoteUrl;
	QString      szNick;
	QString      szUser;
	QString      szHost;
} KviPendingAvatarChange;

#ifdef Unsorted
	#undef Unsorted
#endif
#ifdef None
	#undef None
#endif

class KVIRC_API KviApp : public KviTalApplication
{
	friend class KviFrame;
	friend class KviWindow;
	friend class KviSetupWizard; // this resides in a module!
	Q_OBJECT
public:
	KviApp(int &argc,char ** argv);
	~KviApp();

protected:
	// directories
	QString                          m_szGlobalKvircDir;
	QString                          m_szLocalKvircDir;
	int				m_iHeartbeatTimerId;
	bool                            m_bFirstTimeRun;
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	bool				m_bPortable;
#endif
	KviWindow                     * m_pActiveWindow;
	bool                            m_bUpdateGuiPending;
	KviPointerList<KviPendingAvatarChange> * m_pPendingAvatarChanges;
	bool                            m_bSetupDone;
	KviPointerHashTable<QString,QStringList> * m_pRecentChannelDict;
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	bool                            m_bUpdatePseudoTransparencyPending;
#endif
#ifndef COMPILE_NO_IPC
	KviIpcSentinel                * m_pIpcSentinel;
#endif
	QFont                           m_fntDefaultFont;
public:
	// setup stuff (accessed from kvi_main.cpp: consider private othwerise)
	QString	          m_szConfigFile;        // setup
	bool              m_bCreateConfig;      // setup
	KviStr            m_szExecAfterStartup;
	bool              m_bShowSplashScreen;
public:
	// FIXME: "Plugins" and "ConfigPlugins" should become "Modules" and "ConfigModules"
	enum KvircSubdir
	{
		None,         Pics,          Config ,
		Help,         HelpEN,        HelpNoIntl,
		Log,          Incoming,      Audio,
		Scripts,      Plugins,       ConfigPlugins,
		Trash,        MsgColors,     Charmaps,
		Avatars,      DefScript,     License,
		Modules,      ConfigScripts, Filters,
		Tmp,          Locale,        Themes,
		Classes,      SmallIcons,    EasyPlugins
	};

public:
	void destroyFrame();
	void setup();                                  // THIS SHOULD BE PRIVATE! (but is accessed from kvi_main.cpp)

#ifndef COMPILE_NO_IPC
	void ipcMessage(char * message);
#endif

	static int getGloballyUniqueId(); // returns an unique integer identifier across the application

	bool firstTimeRun(){ return m_bFirstTimeRun; };
	inline bool supportsCompositing()
	{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		//we need >= win2000
		return true;
#endif
#ifdef COMPILE_X11_SUPPORT
		return QX11Info::isCompositingManagerRunning();
#endif
#ifdef COMPILE_ON_MAC
		return true;
#endif
		return false;
	};
	void setupBegin();
	void setupFinish();

	/*
	Unused
	inline void emitRecentUrlsChanged() { emit(recentUrlsChanged()); };
	 */

	// kvi_app.cpp (Saving options)
	void saveIrcServerDataBase();
	void saveProxyDataBase();
	void saveRecentEntries();
	void saveAppEvents();
	void saveRawEvents();
	void saveMediaTypes();
	void saveRegisteredUsers();
	void saveSharedFiles();
	void savePopups();
	void saveAliases();
	void saveNickServ();
	void saveIdentityProfiles();
	void saveTextIcons();
	void saveRegisteredChannels();
	void saveInputHistory();
	void saveAvatarCache();
	void saveToolBars();
	void saveActions();
	void saveScriptAddons();
	void saveIdentities();

	// kvi_options.cpp : Options saving and loading (kvi.main.conf))
	void loadOptions();
	void saveOptions();

	void listAvailableOptions(KviWindow *wnd);
	bool getOptionString(const QString &optName,QString &buffer);
	bool setOptionValue(const QString &optName,const QString &value);
	bool setCommonOptionValue(const QString &optName,const QString &value);

	void optionResetUpdate(int flags);

	// kvi_app_fs.cpp : Filesystem thingies
	void getGlobalKvircDirectory(QString &szData,KvircSubdir dir,const QString &appendFile = QString());
	void getLocalKvircDirectory(QString &szData,KvircSubdir dir,const QString &appendFile = QString(),bool bCreateIfNeeded = true);

	void getTmpFileName(QString &szBuffer,const QString &szEndingFileName = QString());
	void getChannelDumpLogFileName(QString &str);

	static QTextCodec * defaultTextCodec();
	static QTextCodec * defaultSrvCodec();

	bool findImageInImageSearchPath(QString &szRetPath,const QString &filename);

	bool findImage(QString &szRetPath,const QString &filename);
	bool findImageThemeOnlyCompat(QString &szRetPath,const QString &filename); // temporary compat, will be removed soon (do not use)
	bool findSmallIcon(QString &szRetPath,const QString &filename);
	// tries to map the full path filename to one of the KVIrc's mapped directories
	// if it doesn't succeed it just returns the complete filename in szRetPath
	//bool mapImageFile(KviStr &szRetPath,const char * filename);
	bool mapImageFile(QString &szRetPath,const QString &filename);

	//void getDefaultDccSaveFilePath(KviStr &path,const char *filename);
	void completeDirectory(const QString &word,KviPointerList<QString> * matches);
	//
	// Returns a config path suitable for reading (at least)
	// First lookups the user local config directory,
	// if no config file is found, it earches in the global one
	// returns false if no config file has been found
	// If bNoFail is true, the buffer is filled with the file name even
	// if no config file has been found: in this case the local config
	// directory is used
	//
	bool getReadOnlyConfigPath(QString &buffer,const char *config_name,KvircSubdir sbd = Config,bool bNoFail = false);


	///
	/// Attempts to locate the audio file specified by szFileName and put it's
	/// absolute path in szRetPath. Returns true if the audio file is found and false otherwise.
	///
	/// If szFileName is already an absolute path then it's returned directly
	/// after testing for its existence. If szFileName is not an absolute path
	/// then it's searched in the local KVIrc "audio" directory, then in the global one
	/// and finally in the user's home.
	///
	bool findAudioFile(QString &szRetPath,const QString &szFileName);

	// kvi_app.cpp : Window stuff
	KviWindow       * findWindow(const QString &windowId);
	KviWindow       * findWindowByCaption(const QString &windowCaption,int iContextId=-1);
	KviConsole      * findConsole(unsigned int ircContextId);
	KviConsole      * findConsole(QString & servr,QString & nick);
	KviConsole      * topmostConnectedConsole();
	KviConsole      * activeConsole();
	bool              windowExists(KviWindow *wnd);
	bool              connectionExists(KviIrcConnection *cnn);
	unsigned int      windowCount();

	// kvi_app.cpp : Utilities
	void resetAvatarForMatchingUsers(KviRegisteredUser * u);
	void restartNotifyLists();
	void restartLagMeters();
	void triggerUpdateGui();
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	void triggerUpdatePseudoTransparency();
#endif

	// kvi_app.cpp : DCC (and CTCP AVATAR & SOUND) related stuff
	void setAvatarOnFileReceived(KviConsole * pConsole,const QString &szRemoteUrl,const QString &szNick,const QString &szUser,const QString &szHost);
	KviPendingAvatarChange * findPendingAvatarChange(KviConsole * pConsole,const QString &szNick,const QString &szRemoteUrl);
	void fileDownloadTerminated(bool bSuccess,const QString &szRemoteUrl,const QString &szLocalFileName,const QString &szNick = QString(),const QString &szError = QString(),bool bQuiet = false);

	void setAvatarFromOptions();

	// uMessageLifetime is in seconds! and 0 means "forever"
	void notifierMessage(KviWindow * pWnd,int iIconId,const QString &szMsg,unsigned int uMessageLifetime);

	void addRecentNickname(const QString& newNick);
	void addRecentChannel(const QString& chan,const QString& net);

	QStringList * recentChannelsForNetwork(const QString& net);

	KviPointerHashTable<QString,QStringList> * recentChannels() const
	{
		return m_pRecentChannelDict;
	}
	

	void addRecentServer(const QString& server);
	void fillRecentServersPopup(KviTalPopupMenu * m);
	void fillRecentNicknamesPopup(KviTalPopupMenu * m,KviConsole * pConsole);
	void fillRecentChannelsPopup(KviTalPopupMenu * m,KviConsole * pConsole);
//
	void autoConnectToServers();

	void checkSuggestRestoreDefaultScript();

	void loadDefaultScript();

protected:
	void buildRecentChannels();
	void saveRecentChannels();
	// kvi_app.cpp : KviWindow global database
	void registerWindow(KviWindow *wnd);
	void unregisterWindow(KviWindow *wnd);
	void frameDestructorCallback();
	void heartbeat(kvi_time_t tNow);
	virtual void timerEvent(QTimerEvent *e);
public slots:
	// kvi_app.cpp : Slots
	void saveConfiguration();
	void updateGui();
	void updatePseudoTransparency();
	void restoreDefaultScript();
	void addRecentUrl(const QString& text);
private:
	void createSplashScreen();
	void destroySplashScreen();

	// kvi_app_setup.cpp : Setup stuff
	void loadDirectories();
	void findGlobalKvircDirectory();
	bool findLocalKvircDirectory();
	bool checkGlobalKvircDirectory(const QString);
	bool checkLocalKvircDirectory(const QString &szSpecificPath);
	bool checkLocalKvircDirectory(); // check the m_szLocalKvircDirectory path
	bool checkFileAssociations();
	void setupFileAssociations();

	bool checkUriAssociations(const char * proto);
	void setupUriAssociations(const char * proto);

	void saveKvircDirectory();

	// kvi_app.cpp : GUI stuff
	void updateApplicationFont();

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	void createGlobalBackgrounds(QPixmap * pix);
	void destroyPseudoTransparency();
#endif //COMPILE_PSEUDO_TRANSPARENCY
private:
	// kvi_app.cpp : parts of setup()
	void loadRecentEntries();
#ifndef COMPILE_NO_IPC
	void createIpcSentinel();
	void destroyIpcSentinel();
#endif //!COMPILE_NO_IPC
	void createFrame();
signals:
	void reloadImages();
	void updateNotifier();
	void recentUrlsChanged();
};

#ifndef _KVI_APP_CPP_
	extern KVIRC_API KviApp * g_pApp;
	extern KVIRC_API KviServerDataBase * g_pServerDataBase;
#endif

#endif //_KVI_APP_H_
