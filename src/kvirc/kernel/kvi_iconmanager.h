#ifndef _KVI_ICONMANAGER_H_
#define _KVI_ICONMANAGER_H_

//=============================================================================
//
//   File : kvi_iconmanager.h
//   Creation date : Sat Jun 24 2000 14:49:24 by Szymon Stefanek
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

#include "kvi_settings.h"
#include "kvi_string.h"
#include "kvi_avatar.h"
#include "kvi_time.h"
#include "kvi_pointerhashtable.h"

#include <QPixmap>
#include <QObject>
#include <QWidget>

#define KVI_SMALLICON_NONE 0
#define KVI_SMALLICON_CLOSE 1
#define KVI_SMALLICON_MAXIMIZE 2
#define KVI_SMALLICON_MINIMIZE 3
#define KVI_SMALLICON_RESTORE 4
#define KVI_SMALLICON_DEFAULTICON 5
#define KVI_SMALLICON_DOCK 6
#define KVI_SMALLICON_UNDOCK 7
#define KVI_SMALLICON_QUITAPP 8
#define KVI_SMALLICON_CONSOLE 9
#define KVI_SMALLICON_FLOPPY 10
#define KVI_SMALLICON_PARSERERROR 11
#define KVI_SMALLICON_PARSERWARNING 12
#define KVI_SMALLICON_SERVER 13
#define KVI_SMALLICON_WORLD 14
#define KVI_SMALLICON_PROXY 15
#define KVI_SMALLICON_KVIRC 16
#define KVI_SMALLICON_CUT 17
#define KVI_SMALLICON_COPY 18
#define KVI_SMALLICON_PASTE 19
#define KVI_SMALLICON_OPTIONS 20
#define KVI_SMALLICON_SOCKETMESSAGE 21
#define KVI_SMALLICON_SOCKETWARNING 22
#define KVI_SMALLICON_SOCKETERROR 23
#define KVI_SMALLICON_SYSTEMERROR 24
#define KVI_SMALLICON_RAW 25
#define KVI_SMALLICON_SYSTEMWARNING 26
#define KVI_SMALLICON_SYSTEMMESSAGE 27
#define KVI_SMALLICON_UNHANDLED 28
#define KVI_SMALLICON_SERVERINFO 29
#define KVI_SMALLICON_MOTD 30
#define KVI_SMALLICON_CHANNEL 31
#define KVI_SMALLICON_HIDEDOUBLEVIEW 32
#define KVI_SMALLICON_SHOWDOUBLEVIEW 33
#define KVI_SMALLICON_OP 34
#define KVI_SMALLICON_VOICE 35
#define KVI_SMALLICON_SERVERPING 36
#define KVI_SMALLICON_SHOWLISTVIEW 37
#define KVI_SMALLICON_HIDELISTVIEW 38
#define KVI_SMALLICON_JOIN 39
#define KVI_SMALLICON_PART 40
#define KVI_SMALLICON_UNRECOGNIZED 41
#define KVI_SMALLICON_TOPIC 42
#define KVI_SMALLICON_ACCEPT 43
#define KVI_SMALLICON_DISCARD 44
#define KVI_SMALLICON_OWNPRIVMSG 45
#define KVI_SMALLICON_CHANPRIVMSG 46
#define KVI_SMALLICON_QUERY 47
#define KVI_SMALLICON_QUERYPRIVMSG 48
#define KVI_SMALLICON_HELP 49
#define KVI_SMALLICON_MDIHELP 50
#define KVI_SMALLICON_CTCPREPLY 51
#define KVI_SMALLICON_CTCPREQUESTREPLIED 52
#define KVI_SMALLICON_CTCPREQUESTIGNORED 53
#define KVI_SMALLICON_CTCPREQUESTFLOOD 54
#define KVI_SMALLICON_CTCPREQUESTUNKNOWN 55
#define KVI_SMALLICON_ACTION 56
#define KVI_SMALLICON_AVATAR 57
#define KVI_SMALLICON_QUIT 58
#define KVI_SMALLICON_SPLIT 59
#define KVI_SMALLICON_QUITSPLIT 60
#define KVI_SMALLICON_NICK 61
#define KVI_SMALLICON_DEOP 62
#define KVI_SMALLICON_DEVOICE 63
#define KVI_SMALLICON_MODE 64
#define KVI_SMALLICON_KEY 65
#define KVI_SMALLICON_LIMIT 66
#define KVI_SMALLICON_BAN 67
#define KVI_SMALLICON_UNBAN 68
#define KVI_SMALLICON_BANEXCEPT 69
#define KVI_SMALLICON_BANUNEXCEPT 70
#define KVI_SMALLICON_INVITEEXCEPT 71
#define KVI_SMALLICON_INVITEUNEXCEPT 72
#define KVI_SMALLICON_CHANMODE 73
#define KVI_SMALLICON_CHANMODEHIDE 74
#define KVI_SMALLICON_WHO 75
#define KVI_SMALLICON_EDITOR 76
#define KVI_SMALLICON_DCCREQUEST 77
#define KVI_SMALLICON_DCCMSG 78
#define KVI_SMALLICON_DCCERROR 79
#define KVI_SMALLICON_ICONMANAGER 80
#define KVI_SMALLICON_SCRIPTCENTER 81
#define KVI_SMALLICON_BOMB 82
#define KVI_SMALLICON_EVENT 83
#define KVI_SMALLICON_EVENTNOHANDLERS 84
#define KVI_SMALLICON_HANDLER 85
#define KVI_SMALLICON_HANDLERDISABLED 86
#define KVI_SMALLICON_NICKNAMEPROBLEM 87
#define KVI_SMALLICON_WHOISUSER 88
#define KVI_SMALLICON_WHOISCHANNELS 89
#define KVI_SMALLICON_WHOISIDLE 90
#define KVI_SMALLICON_WHOISSERVER 91
#define KVI_SMALLICON_WHOISOTHER 92
#define KVI_SMALLICON_TIME 93
#define KVI_SMALLICON_NOTIFYONLINE 94
#define KVI_SMALLICON_NOTIFYOFFLINE 95
#define KVI_SMALLICON_LOCKED 96
#define KVI_SMALLICON_UNLOCKED 97
#define KVI_SMALLICON_LOCKEDOFF 98
#define KVI_SMALLICON_UNLOCKEDOFF 99
#define KVI_SMALLICON_OWNPRIVMSGCRYPTED 100
#define KVI_SMALLICON_CHANPRIVMSGCRYPTED 101
#define KVI_SMALLICON_QUERYPRIVMSGCRYPTED 102
#define KVI_SMALLICON_DCCCHATMSG 103
#define KVI_SMALLICON_DCCCHATMSGCRYPTED 104
#define KVI_SMALLICON_IRC 105
#define KVI_SMALLICON_FOLDER 106
#define KVI_SMALLICON_HOME 107
#define KVI_SMALLICON_BOOKMARKS 108
#define KVI_SMALLICON_SPY 109
#define KVI_SMALLICON_KICK 110
#define KVI_SMALLICON_LINUX 111
#define KVI_SMALLICON_LINKS 112
#define KVI_SMALLICON_REGUSERS 113
#define KVI_SMALLICON_TRAYICON 114
#define KVI_SMALLICON_CASCADEWINDOWS 115
#define KVI_SMALLICON_MAXVERTICAL 116
#define KVI_SMALLICON_MAXHORIZONTAL 117
#define KVI_SMALLICON_TILEWINDOWS 118
#define KVI_SMALLICON_LOG 119
#define KVI_SMALLICON_SPAM 120
#define KVI_SMALLICON_FILE 121
#define KVI_SMALLICON_ICQ 122
#define KVI_SMALLICON_ICQYELLOW 123
#define KVI_SMALLICON_ICQRED 124
#define KVI_SMALLICON_ICQBLUE 125
#define KVI_SMALLICON_ICQLIGHTGREEN 126
#define KVI_SMALLICON_ICQLIGHTYELLOW 127
#define KVI_SMALLICON_MESSAGE 128
#define KVI_SMALLICON_MESSAGESENT 129
#define KVI_SMALLICON_BLUESQUARE 130
#define KVI_SMALLICON_VIOLETSQUARE 131
#define KVI_SMALLICON_YELLOWSQUARE 132
#define KVI_SMALLICON_GREENSQUARE 133
#define KVI_SMALLICON_BLACKSQUARE 134
#define KVI_SMALLICON_REDSQUARE 135
#define KVI_SMALLICON_CYANSQUARE 136
#define KVI_SMALLICON_DKGREENSQUARE 137
#define KVI_SMALLICON_TERMINAL 138
#define KVI_SMALLICON_WALLOPS 139
#define KVI_SMALLICON_INVISIBLE 140
#define KVI_SMALLICON_SERVERNOTICE 141
#define KVI_SMALLICON_GNUTELLA 142
#define KVI_SMALLICON_SEARCH 143
#define KVI_SMALLICON_FILES 144
#define KVI_SMALLICON_TRANSFER 145
#define KVI_SMALLICON_PACKAGE 146
#define KVI_SMALLICON_RETRY 147
#define KVI_SMALLICON_IDEA 148
#define KVI_SMALLICON_COLORS 149
#define KVI_SMALLICON_GUI 150
#define KVI_SMALLICON_IRCVIEW 151
#define KVI_SMALLICON_ALIAS 152
#define KVI_SMALLICON_CHANNELNOTICE 153
#define KVI_SMALLICON_CHANNELNOTICECRYPTED 154
#define KVI_SMALLICON_QUERYNOTICE 155
#define KVI_SMALLICON_QUERYNOTICECRYPTED 156
// FIXME: rename the icon 157: it was colliding with 141
#define KVI_SMALLICON_SERVNOTICE 157
#define KVI_SMALLICON_POPUP 158
#define KVI_SMALLICON_PROLOGUE 159
#define KVI_SMALLICON_EPILOGUE 160
#define KVI_SMALLICON_SHAREDFILES 161
#define KVI_SMALLICON_CTCPREPLYUNKNOWN 162
#define KVI_SMALLICON_CANVAS 163
#define KVI_SMALLICON_NICKSERV 164
#define KVI_SMALLICON_CHANSERV 165
#define KVI_SMALLICON_DCCVOICE 166
#define KVI_SMALLICON_PLAY 167
#define KVI_SMALLICON_RECORD 168
#define KVI_SMALLICON_AUTOTILEWINDOWS 169
#define KVI_SMALLICON_AWAY 170
#define KVI_SMALLICON_IDENT 171
#define KVI_SMALLICON_HOMEPAGE 172
#define KVI_SMALLICON_LIST 173
#define KVI_SMALLICON_HALFOP 174
#define KVI_SMALLICON_HALFDEOP 175
#define KVI_SMALLICON_INVITE 176
#define KVI_SMALLICON_MULTIMEDIA 177
#define KVI_SMALLICON_LOOK 178
#define KVI_SMALLICON_INPUT 179
#define KVI_SMALLICON_MESSAGES 180
#define KVI_SMALLICON_QUERYTRACE 181
#define KVI_SMALLICON_NOCHANNEL 182
#define KVI_SMALLICON_BROADCASTPRIVMSG 183
#define KVI_SMALLICON_BROADCASTNOTICE 184
#define KVI_SMALLICON_URL 185
#define KVI_SMALLICON_RAWEVENT 186
#define KVI_SMALLICON_RAWEVENTNOHANDLERS 187
#define KVI_SMALLICON_MEKICK 188
#define KVI_SMALLICON_MEOP 189
#define KVI_SMALLICON_MEVOICE 190
#define KVI_SMALLICON_MEDEOP 191
#define KVI_SMALLICON_MEDEVOICE 192
#define KVI_SMALLICON_MEHALFOP 193
#define KVI_SMALLICON_MEDEHALFOP 194
#define KVI_SMALLICON_MEBAN 195
#define KVI_SMALLICON_MEUNBAN 196
#define KVI_SMALLICON_MEBANEXCEPT 197
#define KVI_SMALLICON_MEBANUNEXCEPT 198
#define KVI_SMALLICON_MEINVITEEXCEPT 199
#define KVI_SMALLICON_MEINVITEUNEXCEPT 200
#define KVI_SMALLICON_CLASSICWINDOWLIST 201
#define KVI_SMALLICON_TREEWINDOWLIST 202
#define KVI_SMALLICON_IGNORE 203
#define KVI_SMALLICON_USERLIST 204
#define KVI_SMALLICON_STATS 205
#define KVI_SMALLICON_POPUPMENU 206
#define KVI_SMALLICON_XY 207
#define KVI_SMALLICON_IRC0 208
#define KVI_SMALLICON_IRC1 209
#define KVI_SMALLICON_IRC2 210
#define KVI_SMALLICON_IRC3 211
#define KVI_SMALLICON_IRC4 212
#define KVI_SMALLICON_IRC5 213
#define KVI_SMALLICON_HEART 214
#define KVI_SMALLICON_HEARTBROKEN 215
#define KVI_SMALLICON_ROSE 216
#define KVI_SMALLICON_BIGGRIN 217
#define KVI_SMALLICON_BIGGRINGLASSES 218
#define KVI_SMALLICON_BIGGRINEYES 219
#define KVI_SMALLICON_TEXTEXCLAMATIVE 220
#define KVI_SMALLICON_TEXTPOINTS 221
#define KVI_SMALLICON_KISS 222
#define KVI_SMALLICON_SURPRISED1 223
#define KVI_SMALLICON_UGLY 224
#define KVI_SMALLICON_ANGRY 225
#define KVI_SMALLICON_SURPRISED2 226
#define KVI_SMALLICON_SMILE 227
#define KVI_SMALLICON_TONGUE 228
#define KVI_SMALLICON_SSL 229
#define KVI_SMALLICON_CRY 230
#define KVI_SMALLICON_EYE 231
#define KVI_SMALLICON_DEADCHANNEL 232
#define KVI_SMALLICON_DEADQUERY 233
#define KVI_SMALLICON_SOUND 234
#define KVI_SMALLICON_TOOLBAR 235
#define KVI_SMALLICON_TEXTENCODING 236
#define KVI_SMALLICON_NEWITEM 237
#define KVI_SMALLICON_NEWITEMBYWIZARD 238
#define KVI_SMALLICON_DELETEITEM 239
#define KVI_SMALLICON_EDITITEM 240
#define KVI_SMALLICON_FILETRANSFER 241
#define KVI_SMALLICON_CHANADMIN 242
#define KVI_SMALLICON_CHANUNADMIN 243
#define KVI_SMALLICON_MECHANADMIN 244
#define KVI_SMALLICON_MECHANUNADMIN 245
#define KVI_SMALLICON_USEROP 246
#define KVI_SMALLICON_DEUSEROP 247
#define KVI_SMALLICON_MEUSEROP 248
#define KVI_SMALLICON_MEDEUSEROP 249
#define KVI_SMALLICON_APPLET 250
#define KVI_SMALLICON_CAPTION 251
#define KVI_SMALLICON_TRANSPARENT 252
#define KVI_SMALLICON_ALERT 253
#define KVI_SMALLICON_USERWINDOW 254
#define KVI_SMALLICON_STATUSBAR 255
#define KVI_SMALLICON_NOTAWAY 256
#define KVI_SMALLICON_PLUS 257
#define KVI_SMALLICON_MINUS 258
#define KVI_SMALLICON_BINARYTEXT 259
#define KVI_SMALLICON_CHANOWNER 260
#define KVI_SMALLICON_CHANUNOWNER 261
#define KVI_SMALLICON_MECHANOWNER 262
#define KVI_SMALLICON_MECHANUNOWNER 263
#define KVI_SMALLICON_AFRAID 264
#define KVI_SMALLICON_TONGUE2 265
#define KVI_SMALLICON_SAYICON 266
#define KVI_SMALLICON_SAYCOLORS 267
#define KVI_SMALLICON_FINGER 268
#define KVI_SMALLICON_SCRIPTACTION 269
#define KVI_SMALLICON_TEARSMILE 270
#define KVI_SMALLICON_SHY 271
#define KVI_SMALLICON_SERVERERROR 272
#define KVI_SMALLICON_CAFE 273
#define KVI_SMALLICON_ADDONS 274
#define KVI_SMALLICON_CHANOWNERAWAY 275
#define KVI_SMALLICON_OPAWAY 276
#define KVI_SMALLICON_VOICEAWAY 277
#define KVI_SMALLICON_CHANADMINAWAY 278
#define KVI_SMALLICON_USEROPAWAY 279
#define KVI_SMALLICON_HALFOPAWAY 280
#define KVI_SMALLICON_CLASSEDITOR 281
#define KVI_SMALLICON_DEMORALIZED 282
#define KVI_SMALLICON_SLURP 283
#define KVI_SMALLICON_NAMESPACE 284
#define KVI_SMALLICON_SAYSMILE 285
#define KVI_SMALLICON_SAYKVS 286
#define KVI_SMALLICON_THEMEOPTIONS 287
#define KVI_SMALLICON_BUG 288
#define KVI_SMALLICON_REFRESH 289
#define KVI_SMALLICON_THEME 290
#define KVI_SMALLICON_SCREENSHOT 291
#define KVI_SMALLICON_UPDATE 292
#define KVI_SMALLICON_NOTUPDATE 293
#define KVI_SMALLICON_FAILUPDATE 294
#define KVI_SMALLICON_UNREADTEXT 295
#define KVI_SMALLICON_IRCOP 296
#define KVI_SMALLICON_IRCOPAWAY 297
#define KVI_SMALLICON_DEIRCOP 298
#define KVI_SMALLICON_MEIRCOP 299
#define KVI_SMALLICON_MEDEIRCOP 300
#define KVI_SMALLICON_ANGEL 301
#define KVI_SMALLICON_CLOWN 302
#define KVI_SMALLICON_DEVIL 303
#define KVI_SMALLICON_INLOVE 304
#define KVI_SMALLICON_NINJA 305
#define KVI_SMALLICON_PIRATE 306
#define KVI_SMALLICON_PUKE 307
#define KVI_SMALLICON_RAGE 308

#define KVI_SMALLICON_CLASS 309
#define KVI_SMALLICON_CLASSNOTBUILT 310
#define KVI_SMALLICON_FUNCTION 311

#define KVI_NUM_SMALL_ICONS 312


#define KVI_BIGICON_DISCONNECTED "kvi_bigicon_disconnected.png"
#define KVI_BIGICON_CONNECTING "kvi_bigicon_connecting.png"
#define KVI_BIGICON_CONNECTED "kvi_bigicon_connected.png"
#define KVI_BIGICON_TOOLS "kvi_bigicon_tools.png"
#define KVI_BIGICON_ACTIONS "kvi_bigicon_actions.png"
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

class KviIconManager;

class KVIRC_API KviCachedPixmap
{
public:
	// the pixmap MUST be allocated with new QPixmap()
	// this calss takes the ownership
	KviCachedPixmap(QPixmap * ptr,const QString &szPath);
	~KviCachedPixmap();
private:
	QString      m_szPath;
	kvi_time_t   m_tLastAccess;
	QPixmap    * m_pPixmap;
	unsigned int m_uSize;
public:
	QPixmap       * pixmap(){ return m_pPixmap; };
	const QString & path(){ return m_szPath; };
	unsigned int    size(){ return m_uSize; };
	kvi_time_t      lastAccessTime(){ return m_tLastAccess; };
	void            updateLastAccessTime();
};

class KVIRC_API KviIconWidget : public QWidget
{
	Q_OBJECT
public:
	KviIconWidget();
	KviIconWidget(QWidget * par);
	~KviIconWidget();
protected:
	void init();
	virtual void closeEvent(QCloseEvent *e);
	virtual bool eventFilter(QObject * o,QEvent *e);
signals:
	void closed();
	void selected(int iIcon);
};



//
// This class manages the images used by KVIrc.
//
// We handle three types of images: builtin small icons, builtin big icons and generic images.
//
// The builtin small icons are in the pics/coresmall directory (this is to optimize the directory size
//      since it's a linear search...), are 16x16 in size and are indexed by numbers in a way that their
//      access is really fast. The icons used here are used mostly in KviIrcView (but not exclusively).
//
// The generic images are just "any" image that the underlying Qt engine
//      is able to load. They are loaded by (relative) path and
//      cached by name.
//
// The builtin big icons are (actually) just generic images
//      that are eventually scaled to 32x32 if needed. One can request
//      a builtin small icon indexed by number to be scaled
//      to the size of 32x32 as a big icon too.
//

class KVIRC_API KviIconManager : public QObject
{
	Q_OBJECT
public:
	KviIconManager();
	~KviIconManager();
private:
	QPixmap                     * m_smallIcons[KVI_NUM_SMALL_ICONS];
	KviIconWidget               * m_pIconWidget;

	KviPointerHashTable<QString,KviCachedPixmap>      * m_pCachedImages;
	KviPointerHashTable<QString,int>                  * m_pIconNames;
	unsigned int                  m_uCacheTotalSize;
	unsigned int                  m_uCacheMaxSize;
public:
	// WARNING: Don't store this pointer!
	// id == filename | number that indicates an internal pixmap
	// 0 stands for "any"
	QPixmap * getImage(const QString &id,bool bCanBeNumber = true,QString* pRetPath = 0);

	// The returned pointer is owned by the icon manager
	// and can be deleted at any time : so don't store it
	KviCachedPixmap * getPixmapWithCache(const QString &szName);

	// The returned pointer is owned by the icon manager
	// and can be deleted at any time : so don't store it
	KviCachedPixmap * getPixmapWithCacheScaleOnLoad(const QString &szName,int iMaxWidth,int iMaxHeight);

	// The returned pointer is owned by the icon manager
	// and can be deleted at any time : so don't store it
	QPixmap * getPixmap(const QString &szName)
		{ KviCachedPixmap * p = getPixmapWithCache(szName); return p ? p->pixmap() : 0; };

	// this one never fails... if the image can't be found
	// a default 32x32 image is returned
	QPixmap * getBigIcon(const QString &szName);

	// this one never fails... if the icon isn't there
	// then a default 16x16 image is returned
	QPixmap * getSmallIcon(int idx){ return idx < KVI_NUM_SMALL_ICONS ? (m_smallIcons[idx] ? m_smallIcons[idx] : loadSmallIcon(idx) ) : 0; };

	const char * getSmallIconName(int idx);

	QString getSmallIconResourceName(int idx);

	int getSmallIconIdFromName(const QString &szName);

	// if szLocalPath is empty then szName can be the identification
	// string for the avatar
	// if szName is empty then it is found from szLocalPath
	KviAvatar * getAvatar(const QString &szLocalPath,const QString &szName);

	void urlToCachedFileName(QString &szFName);

	void clearCache();
	void reloadImages();

	void cacheCleanup();

protected:
	void addToCache(const QString &szName,KviCachedPixmap * p);
	//void loadSmallIcons();
	QPixmap * loadSmallIcon(int idx);

	void initQResourceBackend();
public slots:
	void showIconWidget();
protected slots:
	void iconWidgetClosed();
};

extern KVIRC_API KviIconManager * g_pIconManager;

#endif //_KVI_ICONMANAGER_H_
