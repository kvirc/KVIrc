//=============================================================================
//
//   File : KviIconManager.cpp
//   Creation date : Sat Jun 24 2000 14:50:11 by Szymon Stefanek
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

#define _KVI_ICONMANAGER_CPP_

#include "KviIconManager.h"
#include "KviApplication.h"
#include "kvi_settings.h"
#include "kvi_defaults.h"
#include "KviLocale.h"
#include "KviFileUtils.h"
#include "KviOptions.h"

#include <QCloseEvent>
#include <QCursor>
#include <QDir>
#include <QDrag>
#include <QEvent>
#include <QIcon>
#include <QLabel>
#include <QLayout>
#include <QMimeData>

/*
	@doc: image_id
	@title:
		The image identifier
	@keyterms:
		image identifier, using builtin images, how KVIrc locates image files
	@type:
		language
	@short:
		Semantics of the <image_id> parameter
	@body:
		Some KVIrc commands and functions accept
		the <image_id> as a parameter.
		The <image_id> indicates an image to be displayed
		by some GUI element. Since KVIrc has a set of
		builtin icons, we want the user to be able to use it :).[br]
		The <image_id> can be either a signed integer number
		or a filename.[br]
		If it is a signed integer it is interpreted as
		index of the internal KVIrc image to use:
		positive integers (starting at 0) indicate
		small (16x16) icons (the ones used in the view widget).[br]
		If <image_id> is anything else than a signed integer,
		it is interpreted as a filename.
		The filename can be an absolute file path or a relative one.
		In this last case KVIrc will look for the file in a predefined set
		of directories: First in the local [i]pics[/i] directory (usually
		in ~/kvirc-version/pics/), then in the local [i]incoming[/i] directory,
		then in the global [i]pics[/i] directory, then in the user home (~)
		and in the current directory.[br]
*/

// KviApplication.cpp
extern QPixmap * g_pUserChanStatePixmap;
extern QPixmap * g_pActivityMeterPixmap;

KviIconManager * g_pIconManager = nullptr;

static const char * g_szIconNames[KviIconManager::IconCount] = {
	"none",                 // 0
	"close",                // 1
	"history",              // 2
	"historyoff",           // 3
	"usermode",             // 4
	"defaulticon",          // 5
	"dock",                 // 6
	"undock",               // 7
	"quitapp",              // 8
	"console",              // 9
	"save",                 // 10
	"parsererror",          // 11
	"parserwarning",        // 12
	"server",               // 13
	"world",                // 14
	"proxy",                // 15
	"kvirc",                // 16
	"cut",                  // 17
	"copy",                 // 18
	"paste",                // 19
	"options",              // 20
	"socketmessage",        // 21
	"socketwarning",        // 22
	"socketerror",          // 23
	"systemerror",          // 24
	"raw",                  // 25
	"systemwarning",        // 26
	"systemmessage",        // 27
	"unhandled",            // 28
	"serverinfo",           // 29
	"motd",                 // 30
	"channel",              // 31
	"hidedoubleview",       // 32
	"showdoubleview",       // 33
	"op",                   // 34
	"voice",                // 35
	"serverping",           // 36
	"showlistview",         // 37
	"hidelistview",         // 38
	"join",                 // 39
	"part",                 // 40
	"unrecognized",         // 41
	"topic",                // 42
	"accept",               // 43
	"discard",              // 44
	"ownprivmsg",           // 45
	"chanprivmsg",          // 46
	"query",                // 48
	"queryprivmsg",         // 49
	"help",                 // 49
	"questionmark",         // 50
	"ctcpreply",            // 51
	"ctcprequestreplied",   // 52
	"ctcprequestignored",   // 53
	"ctcprequestflood",     // 54
	"ctcprequestunknown",   // 55
	"action",               // 56
	"avatar",               // 57
	"quit",                 // 58
	"split",                // 59
	"quitsplit",            // 60
	"nick",                 // 61
	"deop",                 // 62
	"devoice",              // 63
	"mode",                 // 64
	"key",                  // 65
	"limit",                // 66
	"ban",                  // 67
	"unban",                // 68
	"banexcept",            // 69
	"banunexcept",          // 70
	"inviteexcept",         // 71
	"inviteunexcept",       // 72
	"chanmode",             // 73
	"chanmodehide",         // 74
	"who",                  // 75
	"editor",               // 76
	"dccrequest",           // 77
	"dccmsg",               // 78
	"dccerror",             // 79
	"iconmanager",          // 80
	"scriptcenter",         // 81
	"bomb",                 // 82
	"event",                // 83
	"eventnohandlers",      // 84
	"handler",              // 85
	"handlerdisabled",      // 86
	"nicknameproblem",      // 87
	"whoisuser",            // 88
	"whoischannels",        // 89
	"whoisidle",            // 90
	"whoisserver",          // 91
	"whoisother",           // 92
	"time",                 // 93
	"notifyonline",         // 94
	"notifyoffline",        // 95
	"locked",               // 96
	"unlocked",             // 97
	"lockedoff",            // 98
	"unlockedoff",          // 99
	"ownprivmsgcrypted",    // 100
	"chanprivmsgcrypted",   // 101
	"queryprivmsgcrypted",  // 102
	"dccchatmsg",           // 103
	"dccchatmsgcrypted",    // 104
	"irc",                  // 105
	"folder",               // 106
	"home",                 // 107
	"bookmarks",            // 108
	"spy",                  // 109
	"kick",                 // 110
	"linux",                // 111
	"links",                // 112
	"regusers",             // 113
	"trayicon",             // 114
	"unsetmode",            // 115
	"favorite",             // 116
	"toolbar",              // 117
	"serverfavorite",       // 118
	"log",                  // 119
	"remove",               // 120
	"file",                 // 121
	"icq",                  // 122
	"icqyellow",            // 123
	"icqred",               // 124
	"icqblue",              // 125
	"icqlightgreen",        // 126
	"icqlightyellow",       // 127
	"message",              // 128
	"messagesent",          // 129
	"bluesquare",           // 130
	"violetsquare",         // 131
	"yellowsquare",         // 132
	"greensquare",          // 133
	"blacksquare",          // 134
	"redsquare",            // 135
	"cyansquare",           // 136
	"dkgreensquare",        // 137
	"terminal",             // 138
	"wallops",              // 139
	"invisible",            // 140
	"servernotice",         // 141
	"gnutella",             // 142
	"search",               // 143
	"files",                // 144
	"newnetwork",           // 145
	"package",              // 146
	"newserver",            // 147
	"idea",                 // 148
	"colors",               // 149
	"gui",                  // 150
	"ircview",              // 151
	"alias",                // 152
	"channelnotice",        // 153
	"channelnoticecrypted", // 154
	"querynotice",          // 155
	"querynoticecrypted",   // 156
	"menubar",              // 157
	"popup",                // 158
	"prologue",             // 159
	"epilogue",             // 160
	"sharedfiles",          // 161
	"ctcpreplyunknown",     // 162
	"canvas",               // 163
	"nickserv",             // 164
	"chanserv",             // 165
	"dccvoice",             // 166
	"play",                 // 167
	"record",               // 168
	"kickoff",              // 169
	"away",                 // 170
	"ident",                // 171
	"homepage",             // 172
	"list",                 // 173
	"halfop",               // 174
	"halfdeop",             // 175
	"invite",               // 176
	"multimedia",           // 177
	"user",                 // 178
	"input",                // 179
	"messages",             // 180
	"querytrace",           // 181
	"nochannel",            // 182
	"broadcastprivmsg",     // 183
	"broadcastnotice",      // 184
	"url",                  // 185
	"rawevent",             // 186
	"raweventnohandlers",   // 187
	"mekick",               // 188
	"meop",                 // 189
	"mevoice",              // 190
	"medeop",               // 191
	"medevoice",            // 192
	"mehalfop",             // 193
	"medehalfop",           // 194
	"meban",                // 195
	"meunban",              // 196
	"mebanexcept",          // 197
	"mebanunexcept",        // 198
	"meinviteexcept",       // 199
	"meinviteunexcept",     // 200
	"classicwindowlist",    // 201
	"treewindowlist",       // 202
	"ignore",               // 203
	"userlist",             // 204
	"stats",                // 205
	"popupmenu",            // 206
	"serverconfiguration",  // 207
	"irc0",                 // 208
	"irc1",                 // 209
	"irc2",                 // 210
	"irc3",                 // 211
	"irc4",                 // 212
	"irc5",                 // 213
	"heart",                // 214
	"heartbroken",          // 215
	"rose",                 // 215
	"biggrin",              // 217
	"biggringlasses",       // 218
	"biggrineyes",          // 219
	"textexclamative",      // 220
	"textpoints",           // 221
	"kiss",                 // 222
	"surprised1",           // 223
	"ugly",                 // 224
	"angry",                // 225
	"surprised2",           // 226
	"smile",                // 227
	"tongue",               // 228
	"ssl",                  // 229
	"cry",                  // 230
	"eye",                  // 231
	"deadchannel",          // 232
	"deadquery",            // 233
	"sound",                // 234
	"toolbareditor",        // 235
	"textencoding",         // 236
	"newitem",              // 237
	"newitembywizard",      // 238
	"deleteitem",           // 239
	"edititem",             // 240
	"filetransfer",         // 241
	"chanadmin",            // 242
	"chanunadmin",          // 243
	"mechanadmin",          // 244
	"mechanunadmin",        // 245
	"userop",               // 246
	"deuserop",             // 247
	"meuserop",             // 248
	"medeuserop",           // 249
	"applet",               // 250
	"spam",                 // 251
	"transparent",          // 252
	"notifier",             // 253
	"userwindow",           // 254
	"statusbar",            // 255
	"notaway",              // 256
	"plus",                 // 257
	"minus",                // 258
	"binarytext",           // 259
	"chanowner",            // 260
	"chanunowner",          // 261
	"mechanowner",          // 262
	"mechanunowner",        // 263
	"afraid",               // 264
	"teeth",                // 265
	"sysmonitor",           // 266
	"saycolors",            // 267
	"finger",               // 268
	"highlighttext",        // 269
	"tearsmile",            // 270
	"shy",                  // 271
	"servererror",          // 272
	"cafe",                 // 273
	"addons",               // 274
	"chanowneraway",        // 275
	"opaway",               // 276
	"voiceaway",            // 277
	"chanadminaway",        // 278
	"useropaway",           // 279
	"halfopaway",           // 280
	"classeditor",          // 281
	"demoralized",          // 282
	"slurp",                // 283
	"namespace",            // 284
	"saysmile",             // 285
	"saykvs",               // 286
	"themeoptions",         // 287
	"bug",                  // 288
	"refresh",              // 289
	"theme",                // 290
	"screenshot",           // 291
	"update",               // 292
	"notupdate",            // 293
	"failupdate",           // 294
	"unreadtext",           // 295
	"ircop",                // 296
	"ircopaway",            // 297
	"deircop",              // 298
	"meircop",              // 299
	"medeircop",            // 300
	"angel",                // 301
	"clown",                // 302
	"devil",                // 303
	"inlove",               // 304
	"ninja",                // 305
	"pirate",               // 306
	"puke",                 // 307
	"rage",                 // 308
	"class",                // 309
	"classnotbuilt",        // 310
	"function",             // 311
	"sexmale",              // 312
	"sexfemale",            // 313
	"ircbot",               // 314
	"alienircop",           // 315
	"memoserv",             // 316
	"info",                 // 317
	"warning",              // 318
	"shield",               // 319
	"raweventeditor",       // 320
	"eventeditor",          // 321
	"actioneditor",         // 322
	"aliaseditor",          // 323
	"popupeditor",          // 324
	"dcc",                  // 325
	"socket",               // 326
	"cake",                 // 327
	"handily",              // 328
	"thumbdown",            // 329
	"thumbup",              // 330
	"facepalm",             // 331
	"identity",             // 332
	"nickpopup",            // 333
	"tools",                // 334
	"favoriteoff",          // 335
	"newproxy",             // 336
	"actioncrypted",        // 337
	"topiccrypted",         // 338
	"ctcpcrypted",          // 339
	"ownaction",            // 340
	"ownactioncrypted"      // 341
};

KviIconWidget::KviIconWidget(QWidget * pPar)
    : QWidget(pPar)
{
	setObjectName("global_icon_widget");
	init();
}

void KviIconWidget::init()
{
	setWindowTitle(__tr2qs("Icon Table - KVIrc"));
	setWindowIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::IconManager))));

	int iRows = KviIconManager::IconCount / 20;
	if((iRows * 20) < KviIconManager::IconCount)
		iRows++;

	QGridLayout * pLayout = new QGridLayout(this);
	for(int i = 0; i < 20; i++)
	{
		KviCString szTmp(KviCString::Format, "%d", i);
		QLabel * pLabel = new QLabel(szTmp.ptr(), this);
		pLayout->addWidget(pLabel, 0, i + 1);
	}
	for(int i = 0; i < iRows; i++)
	{
		KviCString szTmp(KviCString::Format, "%d", i * 20);
		QLabel * pLabel = new QLabel(szTmp.ptr(), this);
		pLayout->addWidget(pLabel, i + 1, 0);
	}
	for(int i = 0; i < KviIconManager::IconCount; i++)
	{
		KviCString szTmp(KviCString::Format, "%d", i);
		QLabel * pLabel = new QLabel(this);
		pLabel->setObjectName(szTmp.ptr());
		pLabel->setPixmap(*(g_pIconManager->getSmallIcon(i)));
		pLabel->installEventFilter(this);
		pLabel->setAcceptDrops(true);
		pLayout->addWidget(pLabel, (i / 20) + 1, (i % 20) + 1);
	}
}

KviIconWidget::~KviIconWidget()
    = default;

void KviIconWidget::closeEvent(QCloseEvent * pEvent)
{
	pEvent->ignore();
	emit closed();
}

bool KviIconWidget::eventFilter(QObject * pObject, QEvent * pEvent)
{
	if(pEvent->type() == QEvent::MouseButtonPress)
	{
		if(pObject->inherits("QLabel"))
		{
			KviCString szName = ((QLabel *)pObject)->objectName();
			bool bOk;
			KviIconManager::SmallIcon eIcon = (KviIconManager::SmallIcon)szName.toInt(&bOk);
			if(bOk)
				emit selected(eIcon);

			if(parentWidget() && isVisible() && parentWidget()->inherits("QMenu"))
			{
				parentWidget()->close();
				return true;
			}
			else
			{
				if(const QPixmap * pPix = ((QLabel *)pObject)->pixmap())
				{
					QDrag * pDrag = new QDrag(this);
					QMimeData * pMime = new QMimeData;

					pMime->setText(pObject->objectName());
					pMime->setImageData(*pPix);
					pDrag->setMimeData(pMime);
					pDrag->setPixmap(*pPix);
				}
			}
		}
	}
	else if(pEvent->type() == QEvent::DragEnter)
	{
		((QDragEnterEvent *)pEvent)->setAccepted(true);
		return true;
	}
	return QWidget::eventFilter(pObject, pEvent);
}

KviCachedPixmap::KviCachedPixmap(QPixmap * pPix, const QString & szPath)
{
	m_szPath = szPath;
	m_tLastAccess = kvi_unixTime();
	m_pPixmap = pPix;
	m_uSize = m_pPixmap->width() * m_pPixmap->height() * (m_pPixmap->depth() / 8);
}

KviCachedPixmap::~KviCachedPixmap()
{
	delete m_pPixmap;
}

void KviCachedPixmap::updateLastAccessTime()
{
	m_tLastAccess = kvi_unixTime();
}

KviIconManager::KviIconManager()
{
	initQResourceBackend();

	m_pCachedImages = new KviPointerHashTable<QString, KviCachedPixmap>(21, true);
	m_pCachedImages->setAutoDelete(true);

	QString szBuffer;

	// Load the userchanstate image
	g_pApp->findImage(szBuffer, KVI_USERCHANSTATE_IMAGE_NAME);
	g_pUserChanStatePixmap = new QPixmap(szBuffer);

	g_pApp->findImage(szBuffer, KVI_ACTIVITYMETER_IMAGE_NAME);
	g_pActivityMeterPixmap = new QPixmap(szBuffer);
}

KviIconManager::~KviIconManager()
{
	delete g_pUserChanStatePixmap;
	delete g_pActivityMeterPixmap;

	if(m_pIconWidget)
		delete m_pIconWidget;

	for(int i = 0; i < KviIconManager::IconCount; i++)
	{
		if(m_smallIcons[i])
			delete m_smallIcons[i];
	}
	delete m_pCachedImages;
	if(m_pIconNames)
		delete m_pIconNames;
}

void KviIconManager::initQResourceBackend()
{
	QString szPath;

	g_pApp->getLocalKvircDirectory(szPath, KviApplication::Pics);
	QDir::addSearchPath("icons", szPath);

	g_pApp->getGlobalKvircDirectory(szPath, KviApplication::Pics);
	QDir::addSearchPath("icons", szPath);

	g_pApp->getLocalKvircDirectory(szPath, KviApplication::SmallIcons);
	QDir::addSearchPath("smallicons", szPath);

	g_pApp->getGlobalKvircDirectory(szPath, KviApplication::SmallIcons);
	QDir::addSearchPath("smallicons", szPath);
}

QString KviIconManager::getSmallIconResourceName(SmallIcon eIcon)
{
	QString szName("smallicons:kcs_");
	szName.append(g_szIconNames[eIcon]);
	szName.append(".png");
	return szName;
}

KviIconManager::SmallIcon KviIconManager::iconName(int iIcon)
{
	switch(iIcon)
	{
		case 1:
			return KviIconManager::Close;
			break;
		case 2:
			return KviIconManager::History;
			break;
		case 3:
			return KviIconManager::HistoryOff;
			break;
		case 4:
			return KviIconManager::UserMode;
			break;
		case 5:
			return KviIconManager::DefaultIcon;
			break;
		case 6:
			return KviIconManager::Dock;
			break;
		case 7:
			return KviIconManager::UnDock;
			break;
		case 8:
			return KviIconManager::QuitApp;
			break;
		case 9:
			return KviIconManager::Console;
			break;
		case 10:
			return KviIconManager::Save;
			break;
		case 11:
			return KviIconManager::ParserError;
			break;
		case 12:
			return KviIconManager::ParserWarning;
			break;
		case 13:
			return KviIconManager::Server;
			break;
		case 14:
			return KviIconManager::World;
			break;
		case 15:
			return KviIconManager::Proxy;
			break;
		case 16:
			return KviIconManager::KVIrc;
			break;
		case 17:
			return KviIconManager::Cut;
			break;
		case 18:
			return KviIconManager::Copy;
			break;
		case 19:
			return KviIconManager::Paste;
			break;
		case 20:
			return KviIconManager::Options;
			break;
		case 21:
			return KviIconManager::SocketMessage;
			break;
		case 22:
			return KviIconManager::SocketWarning;
			break;
		case 23:
			return KviIconManager::SocketError;
			break;
		case 24:
			return KviIconManager::SystemError;
			break;
		case 25:
			return KviIconManager::Raw;
			break;
		case 26:
			return KviIconManager::SystemWarning;
			break;
		case 27:
			return KviIconManager::SystemMessage;
			break;
		case 28:
			return KviIconManager::UnHandled;
			break;
		case 29:
			return KviIconManager::ServerInfo;
			break;
		case 30:
			return KviIconManager::Motd;
			break;
		case 31:
			return KviIconManager::Channel;
			break;
		case 32:
			return KviIconManager::HideDoubleView;
			break;
		case 33:
			return KviIconManager::ShowDoubleView;
			break;
		case 34:
			return KviIconManager::Op;
			break;
		case 35:
			return KviIconManager::Voice;
			break;
		case 36:
			return KviIconManager::ServerPing;
			break;
		case 37:
			return KviIconManager::ShowListView;
			break;
		case 38:
			return KviIconManager::HideListView;
			break;
		case 39:
			return KviIconManager::Join;
			break;
		case 40:
			return KviIconManager::Part;
			break;
		case 41:
			return KviIconManager::Unrecognized;
			break;
		case 42:
			return KviIconManager::Topic;
			break;
		case 43:
			return KviIconManager::Accept;
			break;
		case 44:
			return KviIconManager::Discard;
			break;
		case 45:
			return KviIconManager::OwnPrivMsg;
			break;
		case 46:
			return KviIconManager::ChanPrivMsg;
			break;
		case 47:
			return KviIconManager::Query;
			break;
		case 48:
			return KviIconManager::QueryPrivMsg;
			break;
		case 49:
			return KviIconManager::Help;
			break;
		case 50:
			return KviIconManager::QuestionMark;
			break;
		case 51:
			return KviIconManager::CtcpReply;
			break;
		case 52:
			return KviIconManager::CtcpRequestReplied;
			break;
		case 53:
			return KviIconManager::CtcpRequestIgnored;
			break;
		case 54:
			return KviIconManager::CtcpRequestFlood;
			break;
		case 55:
			return KviIconManager::CtcpRequestUnknown;
			break;
		case 56:
			return KviIconManager::Action;
			break;
		case 57:
			return KviIconManager::Avatar;
			break;
		case 58:
			return KviIconManager::Quit;
			break;
		case 59:
			return KviIconManager::Split;
			break;
		case 60:
			return KviIconManager::QuitSplit;
			break;
		case 61:
			return KviIconManager::Nick;
			break;
		case 62:
			return KviIconManager::DeOp;
			break;
		case 63:
			return KviIconManager::DeVoice;
			break;
		case 64:
			return KviIconManager::Mode;
			break;
		case 65:
			return KviIconManager::Key;
			break;
		case 66:
			return KviIconManager::Limit;
			break;
		case 67:
			return KviIconManager::Ban;
			break;
		case 68:
			return KviIconManager::UnBan;
			break;
		case 69:
			return KviIconManager::BanExcept;
			break;
		case 70:
			return KviIconManager::BanUnExcept;
			break;
		case 71:
			return KviIconManager::InviteExcept;
			break;
		case 72:
			return KviIconManager::InviteUnExcept;
			break;
		case 73:
			return KviIconManager::ChanMode;
			break;
		case 74:
			return KviIconManager::ChanModeHide;
			break;
		case 75:
			return KviIconManager::Who;
			break;
		case 76:
			return KviIconManager::Editor;
			break;
		case 77:
			return KviIconManager::DccRequest;
			break;
		case 78:
			return KviIconManager::DccMsg;
			break;
		case 79:
			return KviIconManager::DccError;
			break;
		case 80:
			return KviIconManager::IconManager;
			break;
		case 81:
			return KviIconManager::ScriptCenter;
			break;
		case 82:
			return KviIconManager::Bomb;
			break;
		case 83:
			return KviIconManager::Event;
			break;
		case 84:
			return KviIconManager::EventNoHandlers;
			break;
		case 85:
			return KviIconManager::Handler;
			break;
		case 86:
			return KviIconManager::HandlerDisabled;
			break;
		case 87:
			return KviIconManager::NickNameProblem;
			break;
		case 88:
			return KviIconManager::WhoisUser;
			break;
		case 89:
			return KviIconManager::WhoisChannels;
			break;
		case 90:
			return KviIconManager::WhoisIdle;
			break;
		case 91:
			return KviIconManager::WhoisServer;
			break;
		case 92:
			return KviIconManager::WhoisOther;
			break;
		case 93:
			return KviIconManager::Time;
			break;
		case 94:
			return KviIconManager::NotifyOnLine;
			break;
		case 95:
			return KviIconManager::NotifyOffLine;
			break;
		case 96:
			return KviIconManager::Locked;
			break;
		case 97:
			return KviIconManager::UnLocked;
			break;
		case 98:
			return KviIconManager::LockedOff;
			break;
		case 99:
			return KviIconManager::UnLockedOff;
			break;
		case 100:
			return KviIconManager::OwnPrivMsgCrypted;
			break;
		case 101:
			return KviIconManager::ChanPrivMsgCrypted;
			break;
		case 102:
			return KviIconManager::QueryPrivMsgCrypted;
			break;
		case 103:
			return KviIconManager::DccChatMsg;
			break;
		case 104:
			return KviIconManager::DccChatMsgCrypted;
			break;
		case 105:
			return KviIconManager::Irc;
			break;
		case 106:
			return KviIconManager::Folder;
			break;
		case 107:
			return KviIconManager::Home;
			break;
		case 108:
			return KviIconManager::BookMarks;
			break;
		case 109:
			return KviIconManager::Spy;
			break;
		case 110:
			return KviIconManager::Kick;
			break;
		case 111:
			return KviIconManager::Linux;
			break;
		case 112:
			return KviIconManager::Links;
			break;
		case 113:
			return KviIconManager::RegUsers;
			break;
		case 114:
			return KviIconManager::TrayIcon;
			break;
		case 115:
			return KviIconManager::UnsetMode;
			break;
		case 116:
			return KviIconManager::Favorite;
			break;
		case 117:
			return KviIconManager::Toolbar;
			break;
		case 118:
			return KviIconManager::ServerFavorite;
			break;
		case 119:
			return KviIconManager::Log;
			break;
		case 120:
			return KviIconManager::Remove;
			break;
		case 121:
			return KviIconManager::File;
			break;
		case 122:
			return KviIconManager::Icq;
			break;
		case 123:
			return KviIconManager::IcqYellow;
			break;
		case 124:
			return KviIconManager::IcqRed;
			break;
		case 125:
			return KviIconManager::IcqBlue;
			break;
		case 126:
			return KviIconManager::IcqLightGreen;
			break;
		case 127:
			return KviIconManager::IcqLightYellow;
			break;
		case 128:
			return KviIconManager::Message;
			break;
		case 129:
			return KviIconManager::MessageSent;
			break;
		case 130:
			return KviIconManager::BlueSquare;
			break;
		case 131:
			return KviIconManager::VioletSquare;
			break;
		case 132:
			return KviIconManager::YellowSquare;
			break;
		case 133:
			return KviIconManager::GreenSquare;
			break;
		case 134:
			return KviIconManager::BlackSquare;
			break;
		case 135:
			return KviIconManager::RedSquare;
			break;
		case 136:
			return KviIconManager::CyanSquare;
			break;
		case 137:
			return KviIconManager::DarkGreenSquare;
			break;
		case 138:
			return KviIconManager::Terminal;
			break;
		case 139:
			return KviIconManager::WallOps;
			break;
		case 140:
			return KviIconManager::Invisible;
			break;
		case 141:
			return KviIconManager::ServerNotice;
			break;
		case 142:
			return KviIconManager::Gnutella;
			break;
		case 143:
			return KviIconManager::Search;
			break;
		case 144:
			return KviIconManager::Files;
			break;
		case 145:
			return KviIconManager::NewNetwork;
			break;
		case 146:
			return KviIconManager::Package;
			break;
		case 147:
			return KviIconManager::NewServer;
			break;
		case 148:
			return KviIconManager::Idea;
			break;
		case 149:
			return KviIconManager::Colors;
			break;
		case 150:
			return KviIconManager::Gui;
			break;
		case 151:
			return KviIconManager::IrcView;
			break;
		case 152:
			return KviIconManager::Alias;
			break;
		case 153:
			return KviIconManager::ChannelNotice;
			break;
		case 154:
			return KviIconManager::ChannelNoticeCrypted;
			break;
		case 155:
			return KviIconManager::QueryNotice;
			break;
		case 156:
			return KviIconManager::QueryNoticeCrypted;
			break;
		case 157:
			return KviIconManager::MenuBar;
			break;
		case 158:
			return KviIconManager::Popup;
			break;
		case 159:
			return KviIconManager::Prologue;
			break;
		case 160:
			return KviIconManager::Epilogue;
			break;
		case 161:
			return KviIconManager::SharedFiles;
			break;
		case 162:
			return KviIconManager::CtcpReplyUnknown;
			break;
		case 163:
			return KviIconManager::Canvas;
			break;
		case 164:
			return KviIconManager::NickServ;
			break;
		case 165:
			return KviIconManager::ChanServ;
			break;
		case 166:
			return KviIconManager::DccVoice;
			break;
		case 167:
			return KviIconManager::Play;
			break;
		case 168:
			return KviIconManager::Record;
			break;
		case 169:
			return KviIconManager::KickOff;
			break;
		case 170:
			return KviIconManager::Away;
			break;
		case 171:
			return KviIconManager::Ident;
			break;
		case 172:
			return KviIconManager::HomePage;
			break;
		case 173:
			return KviIconManager::List;
			break;
		case 174:
			return KviIconManager::HalfOp;
			break;
		case 175:
			return KviIconManager::HalfDeOp;
			break;
		case 176:
			return KviIconManager::Invite;
			break;
		case 177:
			return KviIconManager::MultiMedia;
			break;
		case 178:
			return KviIconManager::User;
			break;
		case 179:
			return KviIconManager::Input;
			break;
		case 180:
			return KviIconManager::Messages;
			break;
		case 181:
			return KviIconManager::QueryTrace;
			break;
		case 182:
			return KviIconManager::NoChannel;
			break;
		case 183:
			return KviIconManager::BroadcastPrivMsg;
			break;
		case 184:
			return KviIconManager::BroadcastNotice;
			break;
		case 185:
			return KviIconManager::Url;
			break;
		case 186:
			return KviIconManager::RawEvent;
			break;
		case 187:
			return KviIconManager::RawEventNoHandlers;
			break;
		case 188:
			return KviIconManager::MeKick;
			break;
		case 189:
			return KviIconManager::MeOp;
			break;
		case 190:
			return KviIconManager::MeVoice;
			break;
		case 191:
			return KviIconManager::MeDeOp;
			break;
		case 192:
			return KviIconManager::MeDeVoice;
			break;
		case 193:
			return KviIconManager::MeHalfOp;
			break;
		case 194:
			return KviIconManager::MeDeHalfOp;
			break;
		case 195:
			return KviIconManager::MeBan;
			break;
		case 196:
			return KviIconManager::MeUnBan;
			break;
		case 197:
			return KviIconManager::MeBanExcept;
			break;
		case 198:
			return KviIconManager::MeBanUnExcept;
			break;
		case 199:
			return KviIconManager::MeInviteExcept;
			break;
		case 200:
			return KviIconManager::MeInviteUnExcept;
			break;
		case 201:
			return KviIconManager::ClassicWindowList;
			break;
		case 202:
			return KviIconManager::TreeWindowList;
			break;
		case 203:
			return KviIconManager::Ignore;
			break;
		case 204:
			return KviIconManager::UserList;
			break;
		case 205:
			return KviIconManager::Stats;
			break;
		case 206:
			return KviIconManager::PopupMenu;
			break;
		case 207:
			return KviIconManager::ServerConfiguration;
			break;
		case 208:
			return KviIconManager::Irc0;
			break;
		case 209:
			return KviIconManager::Irc1;
			break;
		case 210:
			return KviIconManager::Irc2;
			break;
		case 211:
			return KviIconManager::Irc3;
			break;
		case 212:
			return KviIconManager::Irc4;
			break;
		case 213:
			return KviIconManager::Irc5;
			break;
		case 214:
			return KviIconManager::Heart;
			break;
		case 215:
			return KviIconManager::HeartBroken;
			break;
		case 216:
			return KviIconManager::Rose;
			break;
		case 217:
			return KviIconManager::BigGrin;
			break;
		case 218:
			return KviIconManager::BigGrinGlasses;
			break;
		case 219:
			return KviIconManager::BigGrinEyes;
			break;
		case 220:
			return KviIconManager::TextExclamative;
			break;
		case 221:
			return KviIconManager::TextPoints;
			break;
		case 222:
			return KviIconManager::Kiss;
			break;
		case 223:
			return KviIconManager::Surprised1;
			break;
		case 224:
			return KviIconManager::Ugly;
			break;
		case 225:
			return KviIconManager::Angry;
			break;
		case 226:
			return KviIconManager::Surprised2;
			break;
		case 227:
			return KviIconManager::Smile;
			break;
		case 228:
			return KviIconManager::Tongue;
			break;
		case 229:
			return KviIconManager::Ssl;
			break;
		case 230:
			return KviIconManager::Cry;
			break;
		case 231:
			return KviIconManager::Eye;
			break;
		case 232:
			return KviIconManager::DeadChannel;
			break;
		case 233:
			return KviIconManager::DeadQuery;
			break;
		case 234:
			return KviIconManager::Sound;
			break;
		case 235:
			return KviIconManager::ToolBarEditor;
			break;
		case 236:
			return KviIconManager::TextEncoding;
			break;
		case 237:
			return KviIconManager::NewItem;
			break;
		case 238:
			return KviIconManager::NewItemByWizard;
			break;
		case 239:
			return KviIconManager::DeleteItem;
			break;
		case 240:
			return KviIconManager::EditItem;
			break;
		case 241:
			return KviIconManager::FileTransfer;
			break;
		case 242:
			return KviIconManager::ChanAdmin;
			break;
		case 243:
			return KviIconManager::ChanUnAdmin;
			break;
		case 244:
			return KviIconManager::MeChanAdmin;
			break;
		case 245:
			return KviIconManager::MeChanUnAdmin;
			break;
		case 246:
			return KviIconManager::UserOp;
			break;
		case 247:
			return KviIconManager::DeUserOp;
			break;
		case 248:
			return KviIconManager::MeUserOp;
			break;
		case 249:
			return KviIconManager::MeDeUserOp;
			break;
		case 250:
			return KviIconManager::Applet;
			break;
		case 251:
			return KviIconManager::Spam;
			break;
		case 252:
			return KviIconManager::Transparent;
			break;
		case 253:
			return KviIconManager::Notifier;
			break;
		case 254:
			return KviIconManager::UserWindow;
			break;
		case 255:
			return KviIconManager::StatusBar;
			break;
		case 256:
			return KviIconManager::NotAway;
			break;
		case 257:
			return KviIconManager::Plus;
			break;
		case 258:
			return KviIconManager::Minus;
			break;
		case 259:
			return KviIconManager::BinaryText;
			break;
		case 260:
			return KviIconManager::ChanOwner;
			break;
		case 261:
			return KviIconManager::ChanUnOwner;
			break;
		case 262:
			return KviIconManager::MeChanOwner;
			break;
		case 263:
			return KviIconManager::MeChanUnOwner;
			break;
		case 264:
			return KviIconManager::Afraid;
			break;
		case 265:
			return KviIconManager::Teeth;
			break;
		case 266:
			return KviIconManager::SysMonitor;
			break;
		case 267:
			return KviIconManager::SayColors;
			break;
		case 268:
			return KviIconManager::Finger;
			break;
		case 269:
			return KviIconManager::HighlightText;
			break;
		case 270:
			return KviIconManager::TearSmile;
			break;
		case 271:
			return KviIconManager::Shy;
			break;
		case 272:
			return KviIconManager::ServerError;
			break;
		case 273:
			return KviIconManager::Cafe;
			break;
		case 274:
			return KviIconManager::Addons;
			break;
		case 275:
			return KviIconManager::ChanOwnerAway;
			break;
		case 276:
			return KviIconManager::OpAway;
			break;
		case 277:
			return KviIconManager::VoiceAway;
			break;
		case 278:
			return KviIconManager::ChanAdminAway;
			break;
		case 279:
			return KviIconManager::UserOpAway;
			break;
		case 280:
			return KviIconManager::HalfOpAway;
			break;
		case 281:
			return KviIconManager::ClassEditor;
			break;
		case 282:
			return KviIconManager::Demoralized;
			break;
		case 283:
			return KviIconManager::Slurp;
			break;
		case 284:
			return KviIconManager::NameSpace;
			break;
		case 285:
			return KviIconManager::SaySmile;
			break;
		case 286:
			return KviIconManager::SayKvs;
			break;
		case 287:
			return KviIconManager::ThemeOptions;
			break;
		case 288:
			return KviIconManager::Bug;
			break;
		case 289:
			return KviIconManager::Refresh;
			break;
		case 290:
			return KviIconManager::Theme;
			break;
		case 291:
			return KviIconManager::ScreenShot;
			break;
		case 292:
			return KviIconManager::Update;
			break;
		case 293:
			return KviIconManager::NotUpdate;
			break;
		case 294:
			return KviIconManager::FailUpdate;
			break;
		case 295:
			return KviIconManager::UnreadText;
			break;
		case 296:
			return KviIconManager::IrcOp;
			break;
		case 297:
			return KviIconManager::IrcOpAway;
			break;
		case 298:
			return KviIconManager::DeIrcOp;
			break;
		case 299:
			return KviIconManager::MeIrcOp;
			break;
		case 300:
			return KviIconManager::MeDeIrcOp;
			break;
		case 301:
			return KviIconManager::Angel;
			break;
		case 302:
			return KviIconManager::Clown;
			break;
		case 303:
			return KviIconManager::Devil;
			break;
		case 304:
			return KviIconManager::InLove;
			break;
		case 305:
			return KviIconManager::Ninja;
			break;
		case 306:
			return KviIconManager::Pirate;
			break;
		case 307:
			return KviIconManager::Puke;
			break;
		case 308:
			return KviIconManager::Rage;
			break;
		case 309:
			return KviIconManager::Class;
			break;
		case 310:
			return KviIconManager::ClassNotBuilt;
			break;
		case 311:
			return KviIconManager::Function;
			break;
		case 312:
			return KviIconManager::SexMale;
			break;
		case 313:
			return KviIconManager::SexFemale;
			break;
		case 314:
			return KviIconManager::IrcBot;
			break;
		case 315:
			return KviIconManager::AlienIrcOp;
			break;
		case 316:
			return KviIconManager::MemoServ;
			break;
		case 317:
			return KviIconManager::Info;
			break;
		case 318:
			return KviIconManager::Warning;
			break;
		case 319:
			return KviIconManager::Shield;
			break;
		case 320:
			return KviIconManager::RawEventEditor;
			break;
		case 321:
			return KviIconManager::EventEditor;
			break;
		case 322:
			return KviIconManager::ActionEditor;
			break;
		case 323:
			return KviIconManager::AliasEditor;
			break;
		case 324:
			return KviIconManager::PopupEditor;
			break;
		case 325:
			return KviIconManager::Dcc;
			break;
		case 326:
			return KviIconManager::Socket;
			break;
		case 327:
			return KviIconManager::Cake;
			break;
		case 328:
			return KviIconManager::HandIly;
			break;
		case 329:
			return KviIconManager::ThumbDown;
			break;
		case 330:
			return KviIconManager::ThumbUp;
			break;
		case 331:
			return KviIconManager::FacePalm;
			break;
		case 332:
			return KviIconManager::Identity;
			break;
		case 333:
			return KviIconManager::NickPopup;
			break;
		case 334:
			return KviIconManager::Tools;
			break;
		case 335:
			return KviIconManager::FavoriteOff;
			break;
		case 336:
			return KviIconManager::NewProxy;
			break;
		case 337:
			return KviIconManager::ActionCrypted;
			break;
		case 338:
			return KviIconManager::TopicCrypted;
			break;
		case 339:
			return KviIconManager::CtcpCrypted;
			break;
		case 340:
			return KviIconManager::OwnAction;
			break;
		case 341:
			return KviIconManager::OwnActionCrypted;
			break;
		case 0:
		case KviIconManager::IconCount:
		default:
			return KviIconManager::None;
			break;
	}
}

const char * KviIconManager::getSmallIconName(SmallIcon eIcon)
{
	return g_szIconNames[eIcon];
}

const char * KviIconManager::getSmallIconName(int iIcon)
{
	return g_szIconNames[iIcon];
}

int KviIconManager::getSmallIconIdFromName(const QString & szName)
{
	if(!m_pIconNames)
	{
		m_pIconNames = new KviPointerHashTable<QString, int>(257, false);
		m_pIconNames->setAutoDelete(true);

		for(int i = 0; i < IconCount; i++)
		{
			int * pInt = new int;
			*pInt = i;
			m_pIconNames->replace(QString(g_szIconNames[i]), pInt);
		}
	}
	int * pInt = m_pIconNames->find(szName);
	if(!pInt)
		return 0;
	return *pInt;
}

void KviIconManager::showIconWidget()
{
	if(!m_pIconWidget)
	{
		m_pIconWidget = new KviIconWidget();
		connect(m_pIconWidget, SIGNAL(closed()), this, SLOT(iconWidgetClosed()));
	}
	m_pIconWidget->show();
	m_pIconWidget->raise();
	m_pIconWidget->setFocus();
}

void KviIconManager::iconWidgetClosed()
{
	delete m_pIconWidget;
	m_pIconWidget = nullptr;
}

KviCachedPixmap * KviIconManager::getPixmapWithCache(const QString & szName)
{
	if(szName.isEmpty())
		return nullptr;

	KviCachedPixmap * pCache = m_pCachedImages->find(szName);

	if(pCache)
	{
		pCache->updateLastAccessTime();
		return pCache;
	}

	QPixmap * pPix = nullptr;

	QString szRetPath;

	if(!g_pApp->findImage(szRetPath, szName))
		return nullptr;

	pPix = new QPixmap(szRetPath);
	if(pPix->isNull())
	{
		delete pPix; // it is not an valid image!!! (really bad situation...)
		pPix = nullptr;
		return nullptr;
	}

	pCache = new KviCachedPixmap(pPix, QString(szRetPath));
	addToCache(szName, pCache);

	return pCache;
}

KviCachedPixmap * KviIconManager::getPixmapWithCacheScaleOnLoad(const QString & szName, int iMaxWidth, int iMaxHeight)
{
	if(szName.isEmpty())
		return nullptr;

	KviCachedPixmap * pCache = m_pCachedImages->find(szName);

	if(pCache)
	{
		pCache->updateLastAccessTime();
		return pCache;
	}

	QPixmap * pPix = nullptr;

	QString szRetPath;

	if(!g_pApp->findImage(szRetPath, szName))
		return nullptr;

	pPix = new QPixmap(szRetPath);
	if(pPix->isNull())
	{
		delete pPix; // it is not an valid image!!! (really bad situation...)
		pPix = nullptr;
		return nullptr;
	}
	if((pPix->width() > iMaxWidth) || (pPix->height() > iMaxHeight))
	{
		// scale to fit
		int scaleW = iMaxWidth;
		int scaleH;
		scaleH = (pPix->height() * iMaxWidth) / pPix->width();
		if(scaleH > iMaxHeight)
		{
			scaleH = iMaxHeight;
			scaleW = (scaleH * pPix->width()) / pPix->height();
		}

		QImage img = pPix->toImage();
		*pPix = QPixmap::fromImage(img.scaled(scaleW, scaleH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
	}

	pCache = new KviCachedPixmap(pPix, QString(szRetPath));
	addToCache(szName, pCache);

	return pCache;
}

void KviIconManager::addToCache(const QString & szName, KviCachedPixmap * pCache)
{
	//if((m_uCacheTotalSize + pCache->size()) >= m_uCacheMaxSize)
	//	cacheCleanup();

	m_pCachedImages->insert(szName, pCache);
	m_uCacheTotalSize += pCache->size();
}

QPixmap * KviIconManager::getImage(const QString & szId, bool bCanBeNumber, QString * pszRetPath)
{
	if(szId.isEmpty())
		return nullptr;

	if(bCanBeNumber)
	{
		bool bOk;
		int iIdx = szId.toInt(&bOk);
		if(bOk)
		{
			// was a number : this is not a filename
			if(iIdx >= 0)
				return getSmallIcon(iIdx % KviIconManager::IconCount);
		}
		else
		{
			if(szId.startsWith("$icon"))
			{
				QString szTmp = szId.trimmed();
				szTmp.replace("$icon(", "");
				szTmp.replace(")", "");
				szTmp = szTmp.trimmed();
				szTmp.replace("\"", "");
				iIdx = getSmallIconIdFromName(szTmp.trimmed());
				if(iIdx >= 0)
					return getSmallIcon(iIdx % KviIconManager::IconCount);
			}
		}
	}

	KviCachedPixmap * pCache = getPixmapWithCache(szId);
	if(!pCache)
		return nullptr;

	if(pszRetPath)
		*pszRetPath = pCache->path();

	return pCache->pixmap();
}

QPixmap * KviIconManager::getBigIcon(const QString & szName)
{
	QPixmap * pPix = getPixmap(szName);
	if(pPix)
		return pPix;

	bool bOk;
	int iIdx = szName.toInt(&bOk);
	if(bOk && (iIdx >= 0))
	{
		// was a number : this is not a filename
		// it was a small icon: scale it and cache it
		QString szTmpName = szName;
		szTmpName += ".scaled16to32";
		pPix = getPixmap(szTmpName);
		if(pPix)
			return pPix;
		pPix = getSmallIcon(iIdx % KviIconManager::IconCount);
		if(pPix)
		{
			QImage tmp = pPix->toImage();
			QImage tmp2 = tmp.scaled(32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
			QPixmap * pPix2 = new QPixmap();
			*pPix2 = QPixmap::fromImage(tmp2);
			KviCachedPixmap * pCache = new KviCachedPixmap(pPix2, QString());
			addToCache(szTmpName, pCache);
			return pCache->pixmap();
		}
	}

	pPix = getPixmap("kvi_bigicon_unknown.png");
	if(pPix)
		return pPix;
	KviCachedPixmap * pCache = new KviCachedPixmap(new QPixmap(32, 32), QString());
	addToCache(szName, pCache);
	return pCache->pixmap();
}

void KviIconManager::urlToCachedFileName(QString & szName)
{
	szName.replace(":", "_");
	szName.replace(";", "_");
	szName.replace("/", "_");
	szName.replace("\\", "_");
	szName.replace("\"", "_");
	szName.replace("\'", "_");
	szName.replace("(", "_");
	szName.replace(")", "_");
	szName.replace("?", "_");
	szName.replace("___", ".");
	//cut filenames to 255 chars, trying to preserve file name/extension (bug #616)
	if(szName.size() > 255)
	{
		QString szExt = szName.right(55);
		szName.truncate(200);
		szName.append(szExt);
	}
}

KviAvatar * KviIconManager::getAvatar(const QString & szLocalPath, const QString & szName)
{
	QString szP, szN;

	if(szLocalPath.isEmpty())
	{
		//both path and name empty => fail
		if(szName.isEmpty())
			return nullptr;
		if(KviQString::equalCIN("http://", szName, 7) || KviQString::equalCIN("https://", szName, 8))
		{
			// it's a URL, cache it locally
			szN = szName;
			urlToCachedFileName(szN);
			g_pApp->getLocalKvircDirectory(szP, KviApplication::Avatars, szN);
			szN = szName; // keep the remote URL as name
		}
		else
		{
			//locally cached avatar, called only by its szName
			szN = KviFileUtils::extractFileName(szName);
			g_pApp->getLocalKvircDirectory(szP, KviApplication::Avatars, szN);
		}
	}
	else if(szName.isEmpty())
	{
		if(szLocalPath.isEmpty())
			return nullptr;
		szP = szLocalPath;
		szN = KviFileUtils::extractFileName(szLocalPath);
	}
	else
	{
		szP = szLocalPath;
		szN = szName;
	}

	KviAvatar * pResult = nullptr;

	if(KviFileUtils::fileExists(szP))
	{
		if(KVI_OPTION_BOOL(KviOption_boolScaleAvatarsOnLoad))
		{
			pResult = new KviAvatar(szP, szN,
			    QSize(
			                            KVI_OPTION_UINT(KviOption_uintScaleAvatarsOnLoadWidth),
			                            KVI_OPTION_UINT(KviOption_uintScaleAvatarsOnLoadHeight)));
		}
		else
		{
			pResult = new KviAvatar(szP, szN);
		}
	}

	//Can't load it
	if(pResult && !pResult->isValid())
	{
		delete pResult;
		pResult = nullptr;
	}

	return pResult;
}

void KviIconManager::clearCache()
{
	m_pCachedImages->clear();
}

void KviIconManager::reloadImages()
{
	clearCache();
	for(auto & m_smallIcon : m_smallIcons)
	{
		if(m_smallIcon)
			delete m_smallIcon;
		m_smallIcon = nullptr;
	}
}

QPixmap * KviIconManager::loadSmallIcon(int iIdx)
{
	if(iIdx >= KviIconManager::IconCount)
		return nullptr;
	if(iIdx < 0)
		return nullptr;

	QString szPath = QString(KVI_SMALLICONS_PREFIX "%1.png").arg(g_szIconNames[iIdx]);
	//KviQString::sprintf(szPath,KVI_SMALLICONS_PREFIX "%s.png",g_szIconNames[iIdx]);
	QString szBuffer;

	g_pApp->findSmallIcon(szBuffer, szPath);
	m_smallIcons[iIdx] = new QPixmap(szBuffer);

	return m_smallIcons[iIdx];
}

#ifdef __GNUC__
//#warning IMPLEMENT CLEANUP
#endif
/*
void KviIconManager::cacheCleanup()
{
	QStringList l;
	KviPointerHashTableIterator<QString,KviCachedPixmap> it(*m_pCachedImages);

	while(it.current())
	{
		kvi_time_t curTime = kvi_unixTime();

		if((((unsigned int)it.current()->timestamp) - ((unsigned int)curTime)) > 120)
		{
			// unused since 2 minutes
			// time to cleanup it...
			l.append(new KviCString(it.currentKey()));
		}
		++it;
	}

	for(QStringList::Iterator it = l.begin();it != l.end();++it)
	{
		m_pCachedImages->remove(*it);
	}

	for(KviCString * p =l.first();p;p = l.next())
	{
		m_pCachedImages->remove(p->ptr());
	}
	if(m_pCachedImages->isEmpty())m_pCleanupTimer->stop();
}
*/
