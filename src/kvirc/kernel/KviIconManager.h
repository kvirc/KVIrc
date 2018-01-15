#ifndef _KVI_ICONMANAGER_H_
#define _KVI_ICONMANAGER_H_
//=============================================================================
//
//   File : KviIconManager.h
//   Creation date : Sat Jun 24 2000 14:49:24 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2011 Elvio Basello (hellvis69 at gmail dot com)
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

/**
* \file KviIconManager.h
* \author Szymon Stefanek
* \brief Icon manager
*/

#include "kvi_settings.h"
#include "KviAvatar.h"
#include "KviCString.h"
#include "KviPointerHashTable.h"
#include "KviTimeUtils.h"

#include <QObject>
#include <QPixmap>
#include <QWidget>

#include <array>

#define KVI_BIGICON_DISCONNECTED "kvi_bigicon_disconnected.png"
#define KVI_BIGICON_CONNECTING "kvi_bigicon_connecting.png"
#define KVI_BIGICON_CONNECTED "kvi_bigicon_connected.png"
#define KVI_BIGICON_TOOLS "kvi_bigicon_tools.png"
#define KVI_BIGICON_ACTIONS "kvi_bigicon_actions.png"
#define KVI_BIGICON_USERACTION "kvi_bigicon_useraction.png"
#define KVI_BIGICON_FOLDER "kvi_bigicon_folder.png"
#define KVI_BIGICON_UNKNOWN "kvi_bigicon_unknown.png"
#define KVI_BIGICON_SEPARATOR "kvi_bigicon_separator.png"
#define KVI_BIGICON_KVS "kvi_bigicon_kvs.png"
#define KVI_BIGICON_THEME "kvi_bigicon_theme.png"
#define KVI_BIGICON_ADDONS "kvi_bigicon_addons.png"

#define KVI_BIGICON_HELP "kvi_bigicon_help.png"
#define KVI_BIGICON_HELPINDEX "kvi_bigicon_helpindex.png"
#define KVI_BIGICON_HELPBACK "kvi_bigicon_helpback.png"
#define KVI_BIGICON_HELPFORWARD "kvi_bigicon_helpforward.png"
#define KVI_BIGICON_HELPCLOSE "kvi_bigicon_helpclose.png"
#define KVI_BIGICON_HELPSEARCH "kvi_bigicon_helpsearch.png"

#define KVI_BIGICON_OPEN "kvi_bigicon_open.png"
#define KVI_BIGICON_SAVE "kvi_bigicon_save.png"
#define KVI_BIGICON_REMOVE "kvi_bigicon_remove.png"
#define KVI_BIGICON_WWW "kvi_bigicon_www.png"
#define KVI_BIGICON_PACK "kvi_bigicon_pack.png"
#define KVI_BIGICON_SCREENSHOT "kvi_bigicon_screenshot.png"
#define KVI_BIGICON_REGUSERS "kvi_bigicon_regusers.png"

#define KVI_SMALLICONS_SUBDIRECTORY "coresmall"
#define KVI_SMALLICONS_PREFIX "kcs_"

// FIXME: this should be removed
#define KVI_SMALLICONS_IMAGELIB_PREFIX "kvi_smallicon_"

#define KVI_USERCHANSTATE_IMAGE_NAME "kvi_userchanstate.png"
#define KVI_ACTIVITYMETER_IMAGE_NAME "kvi_activitymeter.png"

#define KVI_REFRESH_IMAGE_NAME "kvi_icon_refresh.png"

class KVIRC_API KviIconWidget;

/**
* \class KviCachedPixmap
* \brief Class for holding a cached pixmap
*/
class KVIRC_API KviCachedPixmap
{
public:
	/**
	* \brief Constructs the KviCachedPixmap object
	* \param pPix The image object
	* \param szPath The path of the image
	* \warning The pixmap MUST be allocated with new QPixmap(). This class takes the ownership
	* \return KviCachedPixmap
	*/
	KviCachedPixmap(QPixmap * pPix, const QString & szPath);

	/**
	* \brief Destroys the KviCachedPixmap object
	*/
	~KviCachedPixmap();

private:
	QString m_szPath;
	kvi_time_t m_tLastAccess;
	QPixmap * m_pPixmap = nullptr;
	unsigned int m_uSize;

public:
	/**
	* \brief Returns the image
	* \return QPixmap *
	*/
	QPixmap * pixmap() const { return m_pPixmap; }

	/**
	* \brief Returns the path of the image
	* \return const QString &
	*/
	const QString & path() const { return m_szPath; }

	/**
	* \brief Returns the size of the image
	* \return unsigned int
	*/
	unsigned int size() const { return m_uSize; }

	/**
	* \brief Returns the time the image was last accessed
	* \return kvi_time_t
	*/
	kvi_time_t lastAccessTime() const { return m_tLastAccess; }

	/**
	* \brief Updates the time the image was last accessed
	* \return void
	*/
	void updateLastAccessTime();
};

/**
* \class KviIconManager
* \brief This class manages the images used by KVIrc.
*
* We handle three types of images: builtin small icons, builtin big icons and generic images.
* The builtin small icons are in the pics/coresmall directory (this is to optimize the directory
* size since it's a linear search...), are 16x16 in size and are indexed by numbers in a way that
* their access is really fast. The icons used here are used mostly in KviIrcView (but not
* exclusively).
* The builtin big icons are (actually) just generic images that are eventually scaled to 32x32
* if needed. One can request a builtin small icon indexed by number to be scaled to the size of
* 32x32 as a big icon too.
* The generic images are just "any" image that the underlying Qt engine is able to load. They are
* loaded by (relative) path and cached by name.
*/
class KVIRC_API KviIconManager : public QObject
{
	Q_OBJECT
public:
	/**
	* \enum SmallIcon
	* \brief Contains all KVIrc's small icons
	*/
	enum SmallIcon
	{
		None = 0,
		Close = 1,
		History = 2,
		HistoryOff = 3,
		UserMode = 4,
		DefaultIcon = 5,
		Dock = 6,
		UnDock = 7,
		QuitApp = 8,
		Console = 9,
		Save = 10,
		ParserError = 11,
		ParserWarning = 12,
		Server = 13,
		World = 14,
		Proxy = 15,
		KVIrc = 16,
		Cut = 17,
		Copy = 18,
		Paste = 19,
		Options = 20,
		SocketMessage = 21,
		SocketWarning = 22,
		SocketError = 23,
		SystemError = 24,
		Raw = 25,
		SystemWarning = 26,
		SystemMessage = 27,
		UnHandled = 28,
		ServerInfo = 29,
		Motd = 30,
		Channel = 31,
		HideDoubleView = 32,
		ShowDoubleView = 33,
		Op = 34,
		Voice = 35,
		ServerPing = 36,
		ShowListView = 37,
		HideListView = 38,
		Join = 39,
		Part = 40,
		Unrecognized = 41,
		Topic = 42,
		Accept = 43,
		Discard = 44,
		OwnPrivMsg = 45,
		ChanPrivMsg = 46,
		Query = 47,
		QueryPrivMsg = 48,
		Help = 49,
		QuestionMark = 50,
		CtcpReply = 51,
		CtcpRequestReplied = 52,
		CtcpRequestIgnored = 53,
		CtcpRequestFlood = 54,
		CtcpRequestUnknown = 55,
		Action = 56,
		Avatar = 57,
		Quit = 58,
		Split = 59,
		QuitSplit = 60,
		Nick = 61,
		DeOp = 62,
		DeVoice = 63,
		Mode = 64,
		Key = 65,
		Limit = 66,
		Ban = 67,
		UnBan = 68,
		BanExcept = 69,
		BanUnExcept = 70,
		InviteExcept = 71,
		InviteUnExcept = 72,
		ChanMode = 73,
		ChanModeHide = 74,
		Who = 75,
		Editor = 76,
		DccRequest = 77,
		DccMsg = 78,
		DccError = 79,
		IconManager = 80,
		ScriptCenter = 81,
		Bomb = 82,
		Event = 83,
		EventNoHandlers = 84,
		Handler = 85,
		HandlerDisabled = 86,
		NickNameProblem = 87,
		WhoisUser = 88,
		WhoisChannels = 89,
		WhoisIdle = 90,
		WhoisServer = 91,
		WhoisOther = 92,
		Time = 93,
		NotifyOnLine = 94,
		NotifyOffLine = 95,
		Locked = 96,
		UnLocked = 97,
		LockedOff = 98,
		UnLockedOff = 99,
		OwnPrivMsgCrypted = 100,
		ChanPrivMsgCrypted = 101,
		QueryPrivMsgCrypted = 102,
		DccChatMsg = 103,
		DccChatMsgCrypted = 104,
		Irc = 105,
		Folder = 106,
		Home = 107,
		BookMarks = 108,
		Spy = 109,
		Kick = 110,
		Linux = 111,
		Links = 112,
		RegUsers = 113,
		TrayIcon = 114,
		UnsetMode = 115,
		Favorite = 116,
		Toolbar = 117,
		ServerFavorite = 118,
		Log = 119,
		Remove = 120,
		File = 121,
		Icq = 122,
		IcqYellow = 123,
		IcqRed = 124,
		IcqBlue = 125,
		IcqLightGreen = 126,
		IcqLightYellow = 127,
		Message = 128,
		MessageSent = 129,
		BlueSquare = 130,
		VioletSquare = 131,
		YellowSquare = 132,
		GreenSquare = 133,
		BlackSquare = 134,
		RedSquare = 135,
		CyanSquare = 136,
		DarkGreenSquare = 137,
		Terminal = 138,
		WallOps = 139,
		Invisible = 140,
		ServerNotice = 141,
		Gnutella = 142,
		Search = 143,
		Files = 144,
		NewNetwork = 145,
		Package = 146,
		NewServer = 147,
		Idea = 148,
		Colors = 149,
		Gui = 150,
		IrcView = 151,
		Alias = 152,
		ChannelNotice = 153,
		ChannelNoticeCrypted = 154,
		QueryNotice = 155,
		QueryNoticeCrypted = 156,
		MenuBar = 157,
		Popup = 158,
		Prologue = 159,
		Epilogue = 160,
		SharedFiles = 161,
		CtcpReplyUnknown = 162,
		Canvas = 163,
		NickServ = 164,
		ChanServ = 165,
		DccVoice = 166,
		Play = 167,
		Record = 168,
		KickOff = 169,
		Away = 170,
		Ident = 171,
		HomePage = 172,
		List = 173,
		HalfOp = 174,
		HalfDeOp = 175,
		Invite = 176,
		MultiMedia = 177,
		User = 178,
		Input = 179,
		Messages = 180,
		QueryTrace = 181,
		NoChannel = 182,
		BroadcastPrivMsg = 183,
		BroadcastNotice = 184,
		Url = 185,
		RawEvent = 186,
		RawEventNoHandlers = 187,
		MeKick = 188,
		MeOp = 189,
		MeVoice = 190,
		MeDeOp = 191,
		MeDeVoice = 192,
		MeHalfOp = 193,
		MeDeHalfOp = 194,
		MeBan = 195,
		MeUnBan = 196,
		MeBanExcept = 197,
		MeBanUnExcept = 198,
		MeInviteExcept = 199,
		MeInviteUnExcept = 200,
		ClassicWindowList = 201,
		TreeWindowList = 202,
		Ignore = 203,
		UserList = 204,
		Stats = 205,
		PopupMenu = 206,
		ServerConfiguration = 207,
		Irc0 = 208,
		Irc1 = 209,
		Irc2 = 210,
		Irc3 = 211,
		Irc4 = 212,
		Irc5 = 213,
		Heart = 214,
		HeartBroken = 215,
		Rose = 216,
		BigGrin = 217,
		BigGrinGlasses = 218,
		BigGrinEyes = 219,
		TextExclamative = 220,
		TextPoints = 221,
		Kiss = 222,
		Surprised1 = 223,
		Ugly = 224,
		Angry = 225,
		Surprised2 = 226,
		Smile = 227,
		Tongue = 228,
		Ssl = 229,
		Cry = 230,
		Eye = 231,
		DeadChannel = 232,
		DeadQuery = 233,
		Sound = 234,
		ToolBarEditor = 235,
		TextEncoding = 236,
		NewItem = 237,
		NewItemByWizard = 238,
		DeleteItem = 239,
		EditItem = 240,
		FileTransfer = 241,
		ChanAdmin = 242,
		ChanUnAdmin = 243,
		MeChanAdmin = 244,
		MeChanUnAdmin = 245,
		UserOp = 246,
		DeUserOp = 247,
		MeUserOp = 248,
		MeDeUserOp = 249,
		Applet = 250,
		Spam = 251,
		Transparent = 252,
		Notifier = 253,
		UserWindow = 254,
		StatusBar = 255,
		NotAway = 256,
		Plus = 257,
		Minus = 258,
		BinaryText = 259,
		ChanOwner = 260,
		ChanUnOwner = 261,
		MeChanOwner = 262,
		MeChanUnOwner = 263,
		Afraid = 264,
		Teeth = 265,
		SysMonitor = 266,
		SayColors = 267,
		Finger = 268,
		HighlightText = 269,
		TearSmile = 270,
		Shy = 271,
		ServerError = 272,
		Cafe = 273,
		Addons = 274,
		ChanOwnerAway = 275,
		OpAway = 276,
		VoiceAway = 277,
		ChanAdminAway = 278,
		UserOpAway = 279,
		HalfOpAway = 280,
		ClassEditor = 281,
		Demoralized = 282,
		Slurp = 283,
		NameSpace = 284,
		SaySmile = 285,
		SayKvs = 286,
		ThemeOptions = 287,
		Bug = 288,
		Refresh = 289,
		Theme = 290,
		ScreenShot = 291,
		Update = 292,
		NotUpdate = 293,
		FailUpdate = 294,
		UnreadText = 295,
		IrcOp = 296,
		IrcOpAway = 297,
		DeIrcOp = 298,
		MeIrcOp = 299,
		MeDeIrcOp = 300,
		Angel = 301,
		Clown = 302,
		Devil = 303,
		InLove = 304,
		Ninja = 305,
		Pirate = 306,
		Puke = 307,
		Rage = 308,
		Class = 309,
		ClassNotBuilt = 310,
		Function = 311,
		SexMale = 312,
		SexFemale = 313,
		IrcBot = 314,
		AlienIrcOp = 315,
		MemoServ = 316,
		Info = 317,
		Warning = 318,
		Shield = 319,
		RawEventEditor = 320,
		EventEditor = 321,
		ActionEditor = 322,
		AliasEditor = 323,
		PopupEditor = 324,
		Dcc = 325,
		Socket = 326,
		Cake = 327,
		HandIly = 328,
		ThumbDown = 329,
		ThumbUp = 330,
		FacePalm = 331,
		Identity = 332,
		NickPopup = 333,
		Tools = 334,
		FavoriteOff = 335,
		NewProxy = 336,
		ActionCrypted = 337,
		TopicCrypted = 338,
		CtcpCrypted = 339,
		OwnAction = 340,
		OwnActionCrypted = 341,
		IconCount = 342
	};

	/**
	* \brief Creates the icon manager object
	* \return KviIconManager
	*/
	KviIconManager();

	/**
	* \brief Destroys the icon manager object
	*/
	~KviIconManager();

private:
	std::array<QPixmap *,IconCount> m_smallIcons = { { nullptr } };
	KviIconWidget * m_pIconWidget = nullptr;
	KviPointerHashTable<QString, KviCachedPixmap> * m_pCachedImages = nullptr;
	KviPointerHashTable<QString, int> * m_pIconNames = nullptr;
	unsigned int m_uCacheTotalSize = 0;
	unsigned int m_uCacheMaxSize = 1024 * 1024; // 1 MiB

public:
	/**
	* \brief Returns the image
	* \param szId The ID of the image
	* This can be a filename or a number that indicates an internal pixmap. 0 stands
	* for "any"
	* \param bCanBeNumber Whether the image is specified by a number, especially for
	* scripts or popups
	* \param pRetPath Holds the path of the image
	* \warning Don't store this pointer!
	*
	* \return QPixmap *
	*/
	QPixmap * getImage(const QString & szId, bool bCanBeNumber = true, QString * pRetPath = nullptr);

	/**
	* \brief Returns the cached pixmap of the image
	* \param szName The name of the image
	* \warning Don't store this pointer!
	* The returned pointer is owned by the icon manager and can be deleted at any time
	* \return KviCachedPixmap *
	*/
	KviCachedPixmap * getPixmapWithCache(const QString & szName);

	/**
	* \brief Returns the cached pixmap of the image and scales it on load
	* \param szName The name of the image
	* \param iMaxWidth The max width to scale
	* \param iMaxHeight The max height to scale
	* \warning Don't store this pointer!
	* The returned pointer is owned by the icon manager and can be deleted at any time
	* \return KviCachedPixmap *
	*/
	KviCachedPixmap * getPixmapWithCacheScaleOnLoad(const QString & szName, int iMaxWidth, int iMaxHeight);

	/**
	* \brief Returns the pixmap of the image
	* \param szName The name of the image
	* \warning Don't store this pointer!
	* The returned pointer is owned by the icon manager and can be deleted at any time
	* \return QPixmap *
	*/
	QPixmap * getPixmap(const QString & szName)
	{
		KviCachedPixmap * pPix = getPixmapWithCache(szName);
		return pPix ? pPix->pixmap() : nullptr;
	}

	/**
	* \brief Returns the big icon
	* \param szName The icon to get
	* \note This one never fails... if the icon isn't there, then a default 32x32 image
	* is returned
	* \return QPixmap *
	*/
	QPixmap * getBigIcon(const QString & szName);

	/**
	* \brief Returns the small icon
	* \param eIcon The icon to get
	* \note This one never fails... if the icon isn't there, then a default 16x16 image
	* is returned
	* \return QPixmap *
	*/
	QPixmap * getSmallIcon(SmallIcon eIcon) { return eIcon < IconCount ? (m_smallIcons[eIcon] ? m_smallIcons[eIcon] : loadSmallIcon(eIcon)) : nullptr; }

	/**
	* \brief Returns the small icon
	* \param iIcon The icon to get
	* \note This one never fails... if the icon isn't there, then a default 16x16 image
	* is returned. This is provided for convenience
	* \return QPixmap *
	*/
	QPixmap * getSmallIcon(int iIcon) { return iIcon < IconCount ? (m_smallIcons[iIcon] ? m_smallIcons[iIcon] : loadSmallIcon(iIcon)) : nullptr; }

	/**
	* \brief Returns the name of the small icon
	* \param eIcon The icon to get
	* \return const char *
	*/
	const char * getSmallIconName(SmallIcon eIcon);

	/**
	* \brief Returns the name of the small icon
	* \param iIcon The icon to get
	* \note This is provided for convenience
	* \return const char *
	*/
	const char * getSmallIconName(int iIcon);

	/**
	* \brief Returns the resource name of the small icon
	* \param eIcon The icon to get
	* \return QString
	*/
	QString getSmallIconResourceName(SmallIcon eIcon);

	/**
	* \brief Returns the name of the small icon
	* \param iIcon The icon to get
	* \return KviIconManager::SmallIcon
	*/
	SmallIcon iconName(int iIcon);

	/**
	* \brief Returns the index of the small icon
	* \param szName The name of the icon
	* \return int
	*/
	int getSmallIconIdFromName(const QString & szName);

	/**
	* \brief Returns the avatar
	* If szLocalPath is empty then szName can be the identification string for the
	* avatar
	* If szName is empty then it is found from szLocalPath
	* \param szLocalPath The local path of the avatar
	* \param szName The name of the avatar
	* \return KviAvatar *
	*/
	KviAvatar * getAvatar(const QString & szLocalPath, const QString & szName);

	/**
	* \brief Returns the url of the image in cache
	* \param szName The path of the image
	* \return void
	*/
	void urlToCachedFileName(QString & szName);

	/**
	* \brief Clears the cache!
	* \return void
	*/
	void clearCache();

	/**
	* \brief Reloads all images
	* \return void
	*/
	void reloadImages();

	//void cacheCleanup();
protected:
	void addToCache(const QString & szName, KviCachedPixmap * pPix);

	/**
	* \brief Returns the icon
	* \param iIdx The ID of the icon
	* \return QPixmap *
	*/
	QPixmap * loadSmallIcon(int iIdx);

	/**
	* \brief Initializes the Qt resource backend
	* \return void
	*/
	void initQResourceBackend();
public slots:
	/**
	* \brief Shows the table of icons
	* \return void
	*/
	void showIconWidget();
protected slots:
	/**
	* \brief Called when we close the table of icons
	* \return void
	*/
	void iconWidgetClosed();
};

/**
* \class KviIconWidget
* \brief The widget with holds the table of icons
*/
class KVIRC_API KviIconWidget : public QWidget
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the icon table widget
	* \param pPar The parent object
	* \return KviIconWidget
	*/
	KviIconWidget(QWidget * pPar = nullptr);

	/**
	* \brief Destroys the icon table widget
	*/
	~KviIconWidget();

protected:
	/**
	* \brief Initializes the table containing the icons
	* \return void
	*/
	void init();

	void closeEvent(QCloseEvent * pEvent) override;
	bool eventFilter(QObject * pObject, QEvent * pEvent) override;
signals:
	/**
	* \brief Emitted when we close the table widget
	* \return void
	*/
	void closed();

	/**
	* \brief Emitted when we select an icon from the table
	* \param eIcon The index of the icon selected
	* \return void
	*/
	void selected(KviIconManager::SmallIcon eIcon);
};

extern KVIRC_API KviIconManager * g_pIconManager;

#endif //_KVI_ICONMANAGER_H_
