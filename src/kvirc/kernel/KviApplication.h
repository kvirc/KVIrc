#ifndef _KVI_APP_H_
#define _KVI_APP_H_
//=============================================================================
//
//   File : KviApplication.h
//   Creation date : Sun Jun 18 2000 12:40:00 CEST by Szymon Stefanek
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

#include "kvi_settings.h"
#include "KviCString.h"
#include "KviQString.h"
#include "KviTalApplication.h"
#include "KviPointerList.h"
#include "KviTimeUtils.h"
#include "KviPointerHashTable.h" // ?

#include <QFont>
#include <QStringList>

#include <memory>
#include <unordered_map>
#include <vector>

#ifdef COMPILE_KDE_SUPPORT
#include <KAboutData>
#endif

#ifdef COMPILE_ON_WINDOWS
// The brain damaged MSVC compiler can't instantiate templates without this
#include "KviMainWindow.h"
#include "KviWindow.h"
#else  // !COMPILE_ON_WINDOWS
class KviMainWindow;
class KviWindow;
#endif // !COMPILE_ON_WINDOWS

#ifndef COMPILE_NO_IPC
class KviIpcSentinel;
#endif // !COMPILE_NO_IPC

class QMenu;
class KviTalListBox;
class KviConsoleWindow;
class KviConfigurationFile;
class KviRegisteredUser;
class KviIrcConnection;
class KviIrcServerDataBase;
class QPixmap;
class QTextCodec;
class QDomElement;
class QStringList;

struct KviPendingAvatarChange
{
	KviConsoleWindow * pConsole;
	QString szRemoteUrl;
	QString szNick;
	QString szUser;
	QString szHost;
};

/**
* \typedef KviNotifierMessageParam
* \struct _KviNotifierMessageParam
* \brief Defines a struct which holds information about the notifier message
*/
struct KviNotifierMessageParam
{
	KviWindow * pWindow;           /**< The window where the notifier was triggered */
	QString szIcon;                /**< The id of the icon (channel, query, ...) */
	QString szMessage;             /**< The message which triggered the notifier */
	unsigned int uMessageLifetime; /**< The timeout of the notifier; 0 means no hide */
};

struct KviDBusNotifierMessageQueue
{
	QStringList lMessages;
};

#ifdef Unsorted
#undef Unsorted
#endif
#ifdef None
#undef None
#endif

class KVIRC_API KviApplication : public KviTalApplication
{
	friend class KviMainWindow;
	friend class KviWindow;
	friend class SetupWizard; // this resides in a module!
	Q_OBJECT
public:
	// FIXME: "Plugins" and "ConfigPlugins" should become "Modules" and "ConfigModules"
	enum KvircSubdir
	{
		None,
		Pics,
		Config,
		Help,
		HelpEN,
		HelpNoIntl,
		Log,
		Incoming,
		Audio,
		Scripts,
		Plugins,
		ConfigPlugins,
		Trash,
		MsgColors,
		Charmaps,
		Avatars,
		DefScript,
		License,
		Modules,
		ConfigScripts,
		Filters,
		Tmp,
		Locale,
		Themes,
		Classes,
		SmallIcons,
		EasyPlugins
	};

	KviApplication(int & argc, char ** argv);
	~KviApplication();

public:
	// setup stuff (accessed from KviMain.cpp: consider private otherwise)
	QString m_szConfigFile; // setup
	bool m_bCreateConfig;   // setup
	QString m_szExecAfterStartup;

protected:
#ifdef COMPILE_KDE_SUPPORT
	KAboutData * m_pAboutData;
#endif
	// directories
	QString m_szGlobalKvircDir;
	QString m_szLocalKvircDir;
	int m_iHeartbeatTimerId;
	bool m_bFirstTimeRun = false;
	bool m_bClosingDown;
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	bool m_bPortable;
#endif
	KviWindow * m_pActiveWindow = nullptr;
	bool m_bUpdateGuiPending;
	std::unordered_map<KviPendingAvatarChange *, std::unique_ptr<KviPendingAvatarChange>> m_PendingAvatarChanges;
	bool m_bSetupDone;
	KviPointerHashTable<QString, QStringList> * m_pRecentChannelDict;
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	bool m_bUpdatePseudoTransparencyPending = false;
#endif
#ifndef COMPILE_NO_IPC
	KviIpcSentinel * m_pIpcSentinel;
#endif
	QFont m_fntDefaultFont;

public:
	void setup(); // THIS SHOULD BE PRIVATE! (but is accessed from KviMain.cpp)

#ifdef COMPILE_KDE_SUPPORT
	void setAboutData(KAboutData * pAboutData) { m_pAboutData = pAboutData; }
	KAboutData * aboutData() const { return m_pAboutData; }
#endif

#ifndef COMPILE_NO_IPC
	void ipcMessage(char * message);
#endif

	static int getGloballyUniqueId(); // returns an unique integer identifier across the application

	bool firstTimeRun() const { return m_bFirstTimeRun; }
	bool kviClosingDown() const { return m_bClosingDown; }
	void setKviClosingDown() { m_bClosingDown = true; }

	bool supportsCompositing();

	void setupBegin();
	void setupFinish();

	/*
	Unused
	void emitRecentUrlsChanged() { emit(recentUrlsChanged()); }
	 */

	// KviApplication.cpp (Saving options)
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
	void saveDefaultScripts();
	void saveTextIcons();
	void saveRegisteredChannels();
	void saveInputHistory();
	void saveAvatarCache();
	void saveToolBars();
	void saveActions();
	void saveScriptAddons();
	void saveIdentities();

	// KviOptions.cpp : Options saving and loading (kvi.main.conf))
	void loadOptions();
	void saveOptions();

	void listAvailableOptions(KviWindow * wnd);
	bool getOptionString(const QString & optName, QString & buffer);
	bool setOptionValue(const QString & optName, const QString & value);
	bool setCommonOptionValue(const QString & optName, const QString & value);

	void optionResetUpdate(int flags);

	// KviApplication_filesystem.cpp : Filesystem thingies
	void getGlobalKvircDirectory(QString & szData, KvircSubdir dir, const QString & appendFile = QString());
	void getLocalKvircDirectory(QString & szData, KvircSubdir dir, const QString & appendFile = QString(), bool bCreateIfNeeded = true);

	void getTmpFileName(QString & szBuffer, const QString & szEndingFileName = QString());
	void getChannelDumpLogFileName(QString & str);

	static QTextCodec * defaultTextCodec();
	static QTextCodec * defaultSrvCodec();

	bool findImageInImageSearchPath(QString & szRetPath, const QString & filename);

	bool findImage(QString & szRetPath, const QString & filename);
	bool findImageThemeOnlyCompat(QString & szRetPath, const QString & filename); // temporary compat, will be removed soon (do not use)
	bool findSmallIcon(QString & szRetPath, const QString & filename);
	// tries to map the full path filename to one of the KVIrc's mapped directories
	// if it doesn't succeed it just returns the complete filename in szRetPath
	//bool mapImageFile(KviCString &szRetPath,const char * filename);
	bool mapImageFile(QString & szRetPath, const QString & filename);

	//void getDefaultDccSaveFilePath(KviCString &path,const char *filename);
	void completeDirectory(const QString & word, std::vector<QString> & matches);
	//
	// Returns a config path suitable for reading (at least)
	// First lookups the user local config directory,
	// if no config file is found, it searches in the global one
	// returns false if no config file has been found
	// If bNoFail is true, the buffer is filled with the file name even
	// if no config file has been found: in this case the local config
	// directory is used
	//
	bool getReadOnlyConfigPath(QString & buffer, const char * config_name, KvircSubdir sbd = Config, bool bNoFail = false);

	///
	/// Attempts to locate the audio file specified by szFileName and put it's
	/// absolute path in szRetPath. Returns true if the audio file is found and false otherwise.
	///
	/// If szFileName is already an absolute path then it's returned directly
	/// after testing for its existence. If szFileName is not an absolute path
	/// then it's searched in the local KVIrc "audio" directory, then in the global one
	/// and finally in the user's home.
	///
	bool findAudioFile(QString & szRetPath, const QString & szFileName);

	// KviApplication.cpp : Window stuff
	KviWindow * findWindow(const QString & windowId);
	KviWindow * findWindowByCaption(const QString & windowCaption, int iContextId = -1);
	KviConsoleWindow * findConsole(unsigned int ircContextId);
	KviConsoleWindow * findConsole(QString & servr, QString & nick);
	KviConsoleWindow * topmostConnectedConsole();
	KviConsoleWindow * activeConsole();
	bool windowExists(KviWindow * wnd);
	bool connectionExists(KviIrcConnection * cnn);
	unsigned int windowCount();

	// KviApplication.cpp : Utilities
	void resetAvatarForMatchingUsers(KviRegisteredUser * u);
	void restartNotifyLists();
	void restartLagMeters();
	void triggerUpdateGui();
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	void triggerUpdatePseudoTransparency();
#endif

	// KviApplication.cpp : DCC (and CTCP AVATAR & SOUND) related stuff
	void setAvatarOnFileReceived(KviConsoleWindow * pConsole, const QString & szRemoteUrl, const QString & szNick, const QString & szUser, const QString & szHost);
	KviPendingAvatarChange * findPendingAvatarChange(KviConsoleWindow * pConsole, const QString & szNick, const QString & szRemoteUrl);
	void fileDownloadTerminated(bool bSuccess, const QString & szRemoteUrl, const QString & szLocalFileName, const QString & szNick = QString(), const QString & szError = QString(), bool bQuiet = false);

	void setAvatarFromOptions();

	// uMessageLifetime is in seconds! and 0 means "forever"
	void notifierMessage(KviWindow * pWnd, int iIconId, const QString & szMsg, unsigned int uMessageLifetime);

	void addRecentNickname(const QString & newNick);
	void addRecentChannel(const QString & chan, const QString & net);

	QStringList * recentChannelsForNetwork(const QString & net);

	KviPointerHashTable<QString, QStringList> * recentChannels() const
	{
		return m_pRecentChannelDict;
	}

	void addRecentServer(const QString & server);
	void fillRecentServersPopup(QMenu * m);
	void fillRecentNicknamesPopup(QMenu * m, KviConsoleWindow * pConsole);
	void fillRecentChannelsPopup(QMenu * m, KviConsoleWindow * pConsole);
	//
	void autoConnectToServers();

	void checkSuggestRestoreDefaultScript();

	void loadDefaultScript(QString szItem = QString());
	QStyle * themedStyle();

protected:
	void buildRecentChannels();
	void saveRecentChannels();
	// KviApplication.cpp : KviWindow global database
	void registerWindow(KviWindow * wnd);
	void unregisterWindow(KviWindow * wnd);
	void frameDestructorCallback();
	void heartbeat(kvi_time_t tNow);
	void timerEvent(QTimerEvent * e) override;

private:
	// KviApplication_setup.cpp : Setup stuff
	void loadDirectories();
	void findGlobalKvircDirectory();
	bool findLocalKvircDirectory();
	bool checkGlobalKvircDirectory(const QString);
	bool checkLocalKvircDirectory(const QString & szSpecificPath);
	bool checkLocalKvircDirectory(); // check the m_szLocalKvircDirectory path
	void setupFileAssociations();
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	void setFileAssociation(const QString & szExtension, const QString & szClassName, const QString & szDescription, const int iIconIndex, const QString & szActionName);
#endif

	void setupUriAssociations(const QString & szProto);

	void saveKvircDirectory();

	// KviApplication.cpp : GUI stuff
	void updateApplicationFont();

	// KviApplication.cpp : parts of setup()
	void loadRecentEntries();
#ifndef COMPILE_NO_IPC
	void createIpcSentinel();
	void destroyIpcSentinel();
#endif // COMPILE_NO_IPC
	void createFrame();

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	void createGlobalBackgrounds(QPixmap * pix);
	void destroyPseudoTransparency();
#endif //COMPILE_PSEUDO_TRANSPARENCY
public slots:
	// KviApplication.cpp : Slots
	void saveConfiguration() override;
	void updateGui();
	void updatePseudoTransparency();
	void restoreDefaultScript();
	void addRecentUrl(const QString & text);
	void showParentFrame();
signals:
	void reloadImages();
	void updateNotifier();
	void recentUrlsChanged();
};

#ifndef _KVI_APP_CPP_
extern KVIRC_API KviApplication * g_pApp;
extern KVIRC_API KviIrcServerDataBase * g_pServerDataBase;
#endif

#endif //_KVI_APP_H_
