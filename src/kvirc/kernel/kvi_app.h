#ifndef _KVI_APP_H_
#define _KVI_APP_H_

//=============================================================================
//
//   File : kvi_app.h
//   Creation date : Sun Jun 18 2000 12:40:00 CEST by Szymon Stefanek
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

#include "kvi_settings.h"
#include "kvi_string.h"
#include "kvi_qstring.h"
#include "kvi_tal_application.h"
#include "kvi_pointerlist.h"
#include "kvi_time.h"
#include "kvi_pointerhashtable.h" // ?

#define KVI_RECENT_CHANNELS_SEPARATOR ":"

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

class KviTalPopupMenu;
class QPixmap;
class KviTalListBox;
class QTextCodec;
class QDomElement;

class KviConsole;
class KviConfig;
class KviRegisteredUser;
class KviIrcConnection;
class KviIrcServerDataBase;

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
	KviWindow                     * m_pActiveWindow;
	bool                            m_bUpdateGuiPending;
	KviPointerList<KviPendingAvatarChange> * m_pPendingAvatarChanges;
	bool                            m_bSetupDone;
	KviPointerHashTable<const char *,QStringList>     * m_pRecentChannelsDict;
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	bool                            m_bUpdatePseudoTransparencyPending;
#endif
#ifndef COMPILE_NO_IPC
	KviIpcSentinel                * m_pIpcSentinel;
#endif
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

	void getClipboardText(KviStr &buffer);
	void setClipboardText(const KviStr &str);

	QString getClipboardText();
	void setClipboardText(const QString &str);


	void collectGarbage(QObject * garbage);

	void destroyFrame();
	void setup();                                  // THIS SHOULD BE PRIVATE! (but is accessed from kvi_main.cpp)

#ifndef COMPILE_NO_IPC
	void ipcMessage(char * message);
#endif

	//static QTextCodec * defaultTextCodec();
	//static const char * defaultTextEncoding();

	static int getGloballyUniqueId(); // returns an unique integer identifier across the application

	bool firstTimeRun(){ return m_bFirstTimeRun; };
	void setupBegin();
	void setupFinish();
	inline void emitRecentUrlsChanged() { emit(recentUrlsChanged()); };

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
	void optionResetUpdate(int flags);

	void contextSensitiveHelp();

	// kvi_app_fs.cpp : Filesystem thingies
	void getGlobalKvircDirectory(QString &szData,KvircSubdir dir,const QString &appendFile = QString::null);
	void getGlobalKvircDirectory(KviStr &szData,KvircSubdir dir,const QString &appendFile = QString::null);
	void getLocalKvircDirectory(KviStr  &szData,KvircSubdir dir,const QString &appendFile = QString::null,bool bCreateIfNeeded = true);
	void getLocalKvircDirectory(QString &szData,KvircSubdir dir,const QString &appendFile = QString::null,bool bCreateIfNeeded = true);
	void getTmpFileName(QString &szBuffer,const QString &szEndingFileName = QString::null);
	bool trashFile(const char *filename);
	void getChannelDumpLogFileName(QString &str);

	static QTextCodec * defaultTextCodec();

	// if the mediatype can be guessed ,it is looked up in the media type save path
	// if found , true is returned
	// if not found or the mediatype can't be guessed then false is returned
	bool findFileByMediaType(KviStr &szRetPath,const char * filename) KVI_DEPRECATED;
	
	bool findImageInImageSearchPath(KviStr &szRetPath,const QString &filename);
	bool findImageInImageSearchPath(QString &szRetPath,const QString &filename);
	
	bool findUserFile(KviStr &szRetPath,const char *filename) KVI_DEPRECATED;
	
	bool findImage(KviStr &szRetPath,const char *filename);
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
	bool getReadOnlyConfigPath(KviStr &buffer,const char *config_name,KvircSubdir sbd = Config,bool bNoFail = false);
	bool getReadOnlyConfigPath(QString &buffer,const char *config_name,KvircSubdir sbd = Config,bool bNoFail = false);

	// kvi_app.cpp : Window stuff
	KviWindow       * findWindow(const QString &windowId);
	KviWindow       * findWindowByCaption(const QString &windowCaption,int iContextId=-1);
	KviConsole      * findConsole(unsigned int ircContextId);
	KviConsole      * findConsole(KviStr & server,KviStr & nick);
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
	void fileDownloadTerminated(bool bSuccess,const QString &szRemoteUrl,const QString &szLocalFileName,const QString &szNick = QString::null,const QString &szError = QString::null,bool bQuiet = false);

	void setAvatarFromOptions();

	bool playFile(const char * filename,KviStr &error,KviWindow * w = 0);

	// uMessageLifetime is in seconds! and 0 means "forever"
	void notifierMessage(KviWindow * pWnd,int iIconId,const QString &szMsg,unsigned int uMessageLifetime);


	void addRecentNickname(const char * newNick);
	void addRecentNickname(const QString& newNick);
	void addRecentChannel(const QString& chan,const QString& net);
	QStringList* getRecentChannels(const QString& net);
	
	void addRecentServer(const QString& server);
	void fillRecentServersPopup(KviTalPopupMenu * m);
	void fillRecentNicknamesPopup(KviTalPopupMenu * m,KviConsole * pConsole);
	void fillRecentChannelsPopup(KviTalPopupMenu * m,KviConsole * pConsole);
	
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
	void kdeRootPixmapDownloadComplete(bool bSuccess);
	void kdeRootPixmapChanged(int iDesktop);
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
	bool checkLocalKvircDirectory(const QString);
	bool checkFileAssociations();
	void setupFileAssociations();

	bool checkUriAssociations(const char * proto);
	void setupUriAssociations(char * proto);

	void saveKvircDirectory();

	// kvi_app.cpp : GUI stuff
	void updateApplicationFont();

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	void createGlobalBackgrounds(QPixmap * pix);
	void destroyPseudoTransparency();
	#ifdef COMPILE_KDE3_SUPPORT
		void downloadKdeRootPixmap();
	#endif //COMPILE_KDE3_SUPPORT
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
	void recentUrlsChanged();
};

#ifndef _KVI_APP_CPP_
	extern KVIRC_API KviApp * g_pApp;
	extern KVIRC_API KviIrcServerDataBase * g_pIrcServerDataBase;
#endif

#endif //_KVI_APP_H_
