//=============================================================================
//
//   File : kvi_coreactions.cpp
//   Creation date : Mon 22 Nov 2004 02:30:47 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2004-2008 Szymon Stefanek <pragma at kvirc dot net>
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



#include "kvi_coreactions.h"
#include "kvi_shortcuts.h"
#include "kvi_frame.h"
#include "kvi_locale.h"
#include "kvi_irccontext.h"
#include "kvi_iconmanager.h"
#include "kvi_app.h"
#include "kvi_customtoolbar.h"
#include "kvi_actiondrawer.h"
#include "kvi_actionmanager.h"
#include "kvi_kvs_action.h"
#include "kvi_mdimanager.h"
#include "kvi_irctoolbar.h"
#include "kvi_locale.h"
#include "kvi_ircurl.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnectionuserinfo.h"
#include "kvi_coreactionnames.h"
#include "kvi_ircconnectionserverinfo.h"
#include "kvi_kvs_script.h"
#include "kvi_tal_popupmenu.h"

#include <QObject>

void register_core_actions(KviActionManager * m)
{
	m->registerAction(new KviConnectAction(m)); // this as parent will destroy it on exit!
	m->registerAction(new KviSeparatorAction(m));
	m->registerAction(new KviIrcContextDisplayAction(m));
	m->registerAction(new KviJoinChannelAction(m));
	m->registerAction(new KviChangeNickAction(m));
	m->registerAction(new KviConnectToServerAction(m));
	m->registerAction(new KviChangeUserModeAction(m));
	m->registerAction(new KviGoAwayAction(m));
	m->registerAction(new KviIrcToolsAction(m));
	m->registerAction(new KviIrcOperationsAction(m));

	KviAction * a;

#define SCRIPT_ACTION(__name,__code,__visible,__description,__category,__icon,__smallicon,__flags,__accel) \
	a = new KviKvsAction(m, \
		QString(__name), \
		QString(__code), \
		QString(__visible), \
		QString(__description), \
		__category, \
		QString(__icon), \
		QString("%1").arg(__smallicon), \
		__flags, \
		__accel); \
	m->registerAction(a)

#define SLOT_ACTION(__name,__object,__slot,__visible,__description,__category,__icon,__smallicon,__flags,__accel) \
	a = new KviAction(m, \
		QString(__name), \
		QString(__visible), \
		QString(__description), \
		__category, \
		QString(__icon), \
		QString("%1").arg(__smallicon), \
		__flags, \
		__accel); \
	QObject::connect(a,SIGNAL(activated()),__object,__slot); \
	m->registerAction(a)

	SCRIPT_ACTION(
		KVI_COREACTION_SERVEROPTIONS,
		"options.edit KviServerOptionsWidget",
		__tr2qs("Configure Servers..."),
		__tr2qs("Allows you to configure the servers and eventually to connect to them"),
		KviActionManager::categorySettings(),
		"kvi_bigicon_servers.png",
		KVI_SMALLICON_SERVER,
		0,
		KVI_SHORTCUTS_SERVERS);

	SCRIPT_ACTION(
		KVI_COREACTION_MANAGEADDONS,
		"addon.dialog",
		__tr2qs("Manage Addons..."),
		__tr2qs("Allows you to manage the script-based addons"),
		KviActionManager::categorySettings(),
		"kvi_bigicon_addons.png",
		KVI_SMALLICON_ADDONS,
		0,
		KVI_SHORTCUTS_MANAGE_ADDONS);

	SCRIPT_ACTION(
		KVI_COREACTION_EDITREGUSERS,
		"reguser.edit -t",
		__tr2qs("Configure Registered Users..."),
		__tr2qs("Shows a dialog that allows editing the registered user entries"),
		KviActionManager::categorySettings(),
		"kvi_bigicon_regusers.png",
		KVI_SMALLICON_REGUSERS,
		0,
		KVI_SHORTCUTS_USERS);

	SCRIPT_ACTION(
		KVI_COREACTION_IDENTITYOPTIONS,
		"options.edit KviIdentityOptionsWidget",
		__tr2qs("Configure Identity..."),
		__tr2qs("Allows you to configure nickname, username, avatar etc..."),
		KviActionManager::categorySettings(),
		"kvi_bigicon_identity.png",
		KVI_SMALLICON_WHO,
		0,
		KVI_SHORTCUTS_IDENTITY);

	SCRIPT_ACTION(
		KVI_COREACTION_SOCKETSPY,
		"socketspy.open",
		__tr2qs("Show SocketSpy..."),
		__tr2qs("Shows a window that allows monitoring the socket traffic"),
		KviActionManager::categoryScripting(),
		"kvi_bigicon_socketspy.png",
		KVI_SMALLICON_SPY,
		KviAction::NeedsContext,
		QString());

	SCRIPT_ACTION(
		KVI_COREACTION_NETWORKLINKS,
		"links.open",
		__tr2qs("Get Network Links"),
		__tr2qs("Shows a window that allows viewing the network links"),
		KviActionManager::categoryIrc(),
		"kvi_bigicon_networklinks.png",
		KVI_SMALLICON_LINKS,
		KviAction::NeedsContext,
		QString());

	SCRIPT_ACTION(
		KVI_COREACTION_CHANNELLIST,
		"list.open",
		__tr2qs("Get Channel List"),
		__tr2qs("Shows a window that allows listing the network channels"),
		KviActionManager::categoryIrc(),
		"kvi_bigicon_channellist.png",
		KVI_SMALLICON_LIST,
		KviAction::NeedsContext,
		QString());

	SCRIPT_ACTION(
		KVI_COREACTION_GENERALOPTIONS,
		"options.dialog",
		__tr2qs("Configure KVIrc..."),
		__tr2qs("Shows the general options dialog"),
		KviActionManager::categorySettings(),
		"kvi_bigicon_settings.png",
		KVI_SMALLICON_OPTIONS,
		0,
		KVI_SHORTCUTS_OPTIONS);

	SCRIPT_ACTION(
		KVI_COREACTION_THEMEOPTIONS,
		"options.dialog theme",
		__tr2qs("Configure Theme..."),
		__tr2qs("Shows the theme options dialog"),
		KviActionManager::categorySettings(),
		"kvi_bigicon_themeoptions.png",
		KVI_SMALLICON_THEMEOPTIONS,
		0,
		KVI_SHORTCUTS_THEME);

	SCRIPT_ACTION(
		KVI_COREACTION_MANAGETHEMES,
		"theme.dialog",
		__tr2qs("Manage Themes..."),
		__tr2qs("Allows you to manage the themes"),
		KviActionManager::categorySettings(),
		"kvi_bigicon_theme.png",
		KVI_SMALLICON_THEME,
		0,
		KVI_SHORTCUTS_MANAGE_THEMES);

	SCRIPT_ACTION(
		KVI_COREACTION_TOOLBAREDITOR,
		"toolbareditor.open",
		__tr2qs("Customize Toolbars..."),
		__tr2qs("Shows a window that allows editing script toolbars"),
		KviActionManager::categorySettings(),
		"kvi_bigicon_toolbareditor.png",
		KVI_SMALLICON_TOOLBAR,
		0,
		KVI_SHORTCUTS_EDITORS_TOOLBAR);

	SCRIPT_ACTION(
		KVI_COREACTION_JOINCHANNELS,
		"channelsjoin.open",
		__tr2qs("Join Channels..."),
		__tr2qs("Shows a dialog that allows you join channels"),
		KviActionManager::categoryIrc(),
		"kvi_bigicon_channels.png",
		KVI_SMALLICON_CHANNEL,
		KviKvsAction::NeedsConnection | KviKvsAction::NeedsContext,
		KVI_SHORTCUTS_JOIN);

	SCRIPT_ACTION(
		KVI_COREACTION_ACTIONEDITOR,
		"actioneditor.open",
		__tr2qs("Edit Actions..."),
		__tr2qs("Shows a window that allows editing actions"),
		KviActionManager::categoryScripting(),
		"kvi_bigicon_actioneditor.png",
		KVI_SMALLICON_SCRIPTACTION,
		0,
		KVI_SHORTCUTS_EDITORS_ACTION);

	SCRIPT_ACTION(
		KVI_COREACTION_ALIASEDITOR,
		"aliaseditor.open",
		__tr2qs("Edit Aliases..."),
		__tr2qs("Shows a window that allows editing aliases"),
		KviActionManager::categoryScripting(),
		"kvi_bigicon_aliaseditor.png",
		KVI_SMALLICON_ALIAS,
		0,
		KVI_SHORTCUTS_EDITORS_ALIAS);

	SCRIPT_ACTION(
		KVI_COREACTION_CLASSEDITOR,
		"classeditor.open",
		__tr2qs("Edit Classes..."),
		__tr2qs("Shows a window that allows editing classes"),
		KviActionManager::categoryScripting(),
		"kvi_bigicon_classeditor.png",
		KVI_SMALLICON_CLASSEDITOR,
		0,
		KVI_SHORTCUTS_EDITORS_CLASS);

	SCRIPT_ACTION(
		KVI_COREACTION_EVENTEDITOR,
		"eventeditor.open",
		__tr2qs("Edit Events..."),
		__tr2qs("Shows a window that allows editing script events"),
		KviActionManager::categoryScripting(),
		"kvi_bigicon_eventeditor.png",
		KVI_SMALLICON_EVENT,
		0,
		KVI_SHORTCUTS_EDITORS_EVENT);

	SCRIPT_ACTION(
		KVI_COREACTION_POPUPEDITOR,
		"popupeditor.open",
		__tr2qs("Edit Popups..."),
		__tr2qs("Shows a window that allows editing popup menus"),
		KviActionManager::categoryScripting(),
		"kvi_bigicon_popupeditor.png",
		KVI_SMALLICON_POPUP,
		0,
		KVI_SHORTCUTS_EDITORS_POPUP);

	SCRIPT_ACTION(
		KVI_COREACTION_RAWEDITOR,
		"raweditor.open",
		__tr2qs("Edit Raw Events..."),
		__tr2qs("Shows a window that allows editing raw scripting events"),
		KviActionManager::categoryScripting(),
		"kvi_bigicon_raweditor.png",
		KVI_SMALLICON_RAWEVENT,
		0,
		KVI_SHORTCUTS_EDITORS_RAW);

	SCRIPT_ACTION(
		KVI_COREACTION_CODETESTER,
		"codetester.open",
		__tr2qs("Script Tester"),
		__tr2qs("Creates an embedded editor for long scripts"),
		KviActionManager::categoryScripting(),
		"kvi_bigicon_codetester.png",
		KVI_SMALLICON_BOMB,
		0,
		KVI_SHORTCUTS_EDITORS_TESTER);

	SCRIPT_ACTION(
		KVI_COREACTION_EXECUTEKVS,
		"dialog.file(open,$tr(\"Select a Script File\")){ if(!$str.isEmpty($0))parse $0; }",
		__tr2qs("Execute Script..."),
		__tr2qs("Allows executing a KVS script from file"),
		KviActionManager::categoryScripting(),
		"kvi_bigicon_folder.png",
		KVI_SMALLICON_FOLDER,
		0,
		KVI_SHORTCUTS_EXEC);

	SCRIPT_ACTION(
		KVI_COREACTION_HELPINDEX,
		"help -m",
		__tr2qs("Help Index"),
		__tr2qs("Shows the documentation index"),
		KviActionManager::categoryGeneric(),
		"kvi_bigicon_help.png",
		KVI_SMALLICON_HELP,
		0,
		KVI_SHORTCUTS_HELP);

	SCRIPT_ACTION(
		KVI_COREACTION_KVIRCMAILINGLIST,
		"openurl http://www.kvirc.net/?id=mailinglist",
		__tr2qs("Subscribe to the Mailing List"),
		__tr2qs("Allows subscribing to the KVIrc mailing list"),
		KviActionManager::categoryGeneric(),
		"kvi_bigicon_mailinglist.png",
		KVI_SMALLICON_MESSAGE,
		0,
		QString());

	SCRIPT_ACTION(
		KVI_COREACTION_KVIRCHOMEPAGE,
		"openurl http://www.kvirc.net",
		__tr2qs("KVIrc WWW"),
		__tr2qs("Opens the KVIrc homepage"),
		KviActionManager::categoryGeneric(),
		"kvi_bigicon_kvircwww.png",
		KVI_SMALLICON_KVIRC,
		0,
		QString());

	SCRIPT_ACTION(
		KVI_COREACTION_KVIRCRUHOMEPAGE,
		"openurl http://www.kvirc.ru",
		__tr2qs("KVIrc Russian WWW"),
		__tr2qs("Opens the KVIrc homepage in russian"),
		KviActionManager::categoryGeneric(),
		"kvi_bigicon_kvircwww.png",
		KVI_SMALLICON_KVIRC,
		0,
		QString());

	SCRIPT_ACTION(
		KVI_COREACTION_SCREENSHOT,
		"theme.screenshot",
		__tr2qs("Acquire Screenshot"),
		__tr2qs("Acquires a Screenshot of the KVIrc main window"),
		KviActionManager::categoryGeneric(),
		"kvi_bigicon_screenshot.png",
		KVI_SMALLICON_SCREENSHOT,
		0,
		QString());


	SLOT_ACTION(
		KVI_COREACTION_CASCADEWINDOWS,
		g_pFrame->mdiManager(),
		SLOT(cascadeMaximized()),
		__tr2qs("Cascade windows"),
		__tr2qs("Arranges the MDI windows in a maximized-cascade fashion"),
		KviActionManager::categoryGUI(),
		"kvi_bigicon_cascadewindows.png",
		KVI_SMALLICON_CASCADEWINDOWS,
		0,
		QString());

	SLOT_ACTION(
		KVI_COREACTION_TILEWINDOWS,
		g_pFrame->mdiManager(),
		SLOT(tile()),
		__tr2qs("Tile windows"),
		__tr2qs("Tiles the MDI windows with the currently selected tile method"),
		KviActionManager::categoryGUI(),
		"kvi_bigicon_tilewindows.png",
		KVI_SMALLICON_TILEWINDOWS,
		0,
		QString());

	SLOT_ACTION(
		KVI_COREACTION_MINIMIZEALLWINDOWS,
		g_pFrame->mdiManager(),
		SLOT(minimizeAll()),
		__tr2qs("Minimize all windows"),
		__tr2qs("Minimized all the currently visible MDI windows"),
		KviActionManager::categoryGUI(),
		"kvi_bigicon_minimizeallwindows.png",
		KVI_SMALLICON_MINIMIZE,
		0,
		QString());

	SLOT_ACTION(
		KVI_COREACTION_NEWIRCCONTEXT,
		g_pFrame,
		SLOT(newConsole()),
		__tr2qs("New IRC Context"),
		__tr2qs("Creates a new IRC context console"),
		KviActionManager::categoryIrc(),
		"kvi_bigicon_newirccontext.png",
		KVI_SMALLICON_CONSOLE,
		0,
		KVI_SHORTCUTS_CONTEXT);

	SLOT_ACTION(
		KVI_COREACTION_QUITKVIRC,
		g_pApp,
		SLOT(quit()),
		__tr2qs("Quit KVIrc"),
		__tr2qs("Quits KVIrc closing all the current connections"),
		KviActionManager::categoryGeneric(),
		"kvi_bigicon_quit.png",
		KVI_SMALLICON_QUITAPP,
		0,
		QString());
}


KviIrcContextDisplayAction::KviIrcContextDisplayAction(QObject * pParent)
: KviAction(pParent,
	QString(KVI_COREACTION_IRCCONTEXTDISPLAY),
	__tr2qs("IRC Context Display"),
	__tr2qs("Shows a small display with IRC context information"),
	KviActionManager::categoryIrc(),
	"kvi_bigicon_irccontextdisplay.png",
	KVI_SMALLICON_NONE,
	NeedsContext | NeedsConnection)
{
}

void KviIrcContextDisplayAction::setup()
{
	KviAction::setup();
	connect(g_pFrame,SIGNAL(activeConnectionServerInfoChanged()),this,SLOT(activeContextStateChanged()));
	connect(g_pFrame,SIGNAL(activeConnectionNickNameChanged()),this,SLOT(activeContextStateChanged()));
	connect(g_pFrame,SIGNAL(activeConnectionUserModeChanged()),this,SLOT(activeContextStateChanged()));
	connect(g_pFrame,SIGNAL(activeConnectionAwayStateChanged()),this,SLOT(activeContextStateChanged()));
	connect(g_pFrame,SIGNAL(activeConnectionLagChanged()),this,SLOT(activeContextStateChanged()));
}

bool KviIrcContextDisplayAction::addToPopupMenu(KviTalPopupMenu *)
{
	// QT4SUX: Widgets can be no longer added to popup menus.. what about labels ?
	return true;
}

QWidget * KviIrcContextDisplayAction::addToCustomToolBar(KviCustomToolBar *t)
{
	if(!setupDone()) setup();
	KviIrcContextDisplay * w = new KviIrcContextDisplay(t,"kvirc.irccontextdisplay");
	t->addWidget(w);
	registerWidget(w);
	return w;
}

void KviIrcContextDisplayAction::activeContextChanged()
{
	if(m_pWidgetList)
	{
		for(QWidget * w = m_pWidgetList->first();w;w = m_pWidgetList->next())
			w->update();
	}
}

void KviIrcContextDisplayAction::activeContextStateChanged()
{
	if(m_pWidgetList)
	{
		for(QWidget * w = m_pWidgetList->first();w;w = m_pWidgetList->next())
			w->update();
	}
}

void KviIrcContextDisplayAction::setEnabled(bool)
{
	// does nothing here : always enabled!
}




KviSeparatorAction::KviSeparatorAction(QObject * pParent)
: KviAction(pParent,
	QString(KVI_COREACTION_SEPARATOR),
	__tr2qs("Separator"),
	__tr2qs("Acts as a separator for other items: performs no action"),
	KviActionManager::categoryGeneric(),
	KVI_BIGICON_SEPARATOR,
	KVI_SMALLICON_NONE,
	0)
{
}


bool KviSeparatorAction::addToPopupMenu(KviTalPopupMenu * p)
{
	if(!setupDone()) setup();
	p->insertSeparator();
	return true;
}

QWidget * KviSeparatorAction::addToCustomToolBar(KviCustomToolBar * t)
{
	if(!setupDone()) setup();
	QWidget * w = new KviCustomToolBarSeparator(t,"kvirc.separator");
	registerWidget(w);
	t->addWidget(w);
	return w;
}


KviConnectAction::KviConnectAction(QObject * pParent)
: KviAction(pParent,
	QString(KVI_COREACTION_CONNECT),
	__tr2qs("Connect/Disconnect"),
	__tr2qs("Allows you to connect to a server or to terminate the current connection"),
	KviActionManager::categoryIrc(),
	KVI_BIGICON_CONNECTED,
	KVI_SMALLICON_NONE, // no small icon for now
	NeedsContext | NeedsConnection,
	QString(KVI_SHORTCUTS_CONNECT))
{
}


void KviConnectAction::reloadImages()
{
	activeContextStateChanged();
}

void KviConnectAction::activeContextChanged()
{
	activeContextStateChanged();
}

void KviConnectAction::activeContextStateChanged()
{

	KviPointerList<QWidget> * bl = widgetList();
	if(!bl)return;

	QPixmap * p;
	QString txt;
	KviIrcContext * c = g_pFrame->activeContext();
	if(c)
	{
		switch(c->state())
		{
			case KviIrcContext::Idle:
				p = g_pIconManager->getBigIcon(KVI_BIGICON_DISCONNECTED);
				txt = m_szConnectString;
			break;
			case KviIrcContext::Connecting:
			case KviIrcContext::LoggingIn:
				p = g_pIconManager->getBigIcon(KVI_BIGICON_CONNECTING);
				txt = m_szAbortConnectionString;
			break;
			case KviIrcContext::Connected:
				p = g_pIconManager->getBigIcon(KVI_BIGICON_CONNECTED);
				txt = m_szDisconnectString;
			break;
			default:
				p = 0;
			break;
		}
	} else {
		p = 0;
	}

	if(p)
	{
		for(QToolButton * b = (QToolButton *)bl->first();b;b =(QToolButton *)bl->next())
		{
			if(!b->isEnabled())b->setEnabled(true);
			b->setIcon(QIcon(*p));
			b->setText(txt);
		}
	} else {
		for(QToolButton * b = (QToolButton *)bl->first();b;b = (QToolButton *)bl->next())
		{
			if(b->isEnabled())b->setEnabled(false);
		}
	}
}

void KviConnectAction::setup()
{
	KviAction::setup();
	m_szConnectString = __tr2qs("Connect");
	m_szAbortConnectionString = __tr2qs("Abort Connection");
	m_szDisconnectString = __tr2qs("Disconnect");
}

void KviConnectAction::activate()
{
	KviIrcContext * c = g_pFrame->activeContext();
	if(c)c->connectOrDisconnect();
}

bool KviConnectAction::addToPopupMenu(KviTalPopupMenu *p)
{
	if(!setupDone())setup();
	KviIrcContext * c = g_pFrame->activeContext();
	int id;


	QString t;
	if(c)
	{
		switch(c->state())
		{
			case KviIrcContext::Idle:
				t = m_szConnectString;
				if(!m_szKeySequence.isEmpty())t += '\t' + m_szKeySequence;
				p->insertItem(t,this,SLOT(activate()));
			break;
			case KviIrcContext::Connecting:
			case KviIrcContext::LoggingIn:
				t = m_szAbortConnectionString;
				if(!m_szKeySequence.isEmpty())t += '\t' + m_szKeySequence;
				p->insertItem(t,this,SLOT(activate()));
			break;
			case KviIrcContext::Connected:
				t = m_szDisconnectString;
				if(!m_szKeySequence.isEmpty())t += '\t' + m_szKeySequence;
				p->insertItem(t,this,SLOT(activate()));
			break;
			default:
				t = m_szConnectString;
				if(!m_szKeySequence.isEmpty())t += '\t' + m_szKeySequence;
				id = p->insertItem(t,this,SLOT(activate()));
				p->setItemEnabled(id,false);
			break;
		}
	} else {
		t = m_szConnectString;
		if(!m_szKeySequence.isEmpty())t += '\t' + m_szKeySequence;
		id = p->insertItem(t,this,SLOT(activate()));
		p->setItemEnabled(id,false);
	}
	return true;
}

QWidget * KviConnectAction::addToCustomToolBar(KviCustomToolBar *t)
{
	if(!setupDone())setup();
	QToolButton * b = new QToolButton(t);
	b->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_DISCONNECTED)));
	b->setText(m_szConnectString);
	b->setStatusTip(m_szConnectString);
	b->setObjectName(name());
	connect(b, SIGNAL(clicked()), this, SLOT(activate()));
	registerWidget(b);
	t->addWidget(b);
	activeContextStateChanged();
	return b;
}


KviSubmenuAction::KviSubmenuAction(
		QObject * pParent,
		const QString &szName,
		const QString &szScriptCode,
		const QString &szVisibleName,
		const QString &szDescription,
		KviActionCategory * pCategory,
		const QString &szBigIconId,
		const QString &szSmallIconId,
		unsigned int uFlags
	)
: KviKvsAction(
		pParent,
		szName,
		szScriptCode,
		szVisibleName,
		szDescription,
		pCategory,
		szBigIconId,
		szSmallIconId,
		uFlags
	)
{
	m_pPopup = 0;
}

KviSubmenuAction::~KviSubmenuAction()
{
	if(m_pPopup)delete m_pPopup;
}

void KviSubmenuAction::setup()
{
	KviKvsAction::setup();
	if(!m_pPopup)
	{
		m_pPopup = new KviTalPopupMenu();
		connect(m_pPopup,SIGNAL(aboutToShow()),this,SLOT(popupAboutToShow()));
		connect(m_pPopup,SIGNAL(activated(int)),this,SLOT(popupActivated(int)));
	}
}

void KviSubmenuAction::popupAboutToShow()
{
}

void KviSubmenuAction::popupActivated(int)
{
}

bool KviSubmenuAction::addToPopupMenu(KviTalPopupMenu *p)
{
	if(!setupDone()) setup();
	int id = p->insertItem(*(smallIcon()),visibleName(),m_pPopup);
	if(!isEnabled()) p->setItemEnabled(id,false);
	return true;
}

QWidget * KviSubmenuAction::addToCustomToolBar(KviCustomToolBar *t)
{
	if(!setupDone()) setup();
	QToolButton * b = new QToolButton(t);
	b->setIcon(*(bigIcon()));
	b->setText(visibleName());
	b->setStatusTip(visibleName());
	b->setObjectName(name());
	connect(b, SIGNAL(clicked()), this, SLOT(activate()));

	b->setMenu(m_pPopup);

	b->setPopupMode( scriptCode().isEmpty() ? QToolButton::InstantPopup :  QToolButton::MenuButtonPopup);
	t->addWidget(b);
	registerWidget(b);
	if(!isEnabled()) b->setEnabled(false);
	return b;
}


KviJoinChannelAction::KviJoinChannelAction(QObject * pParent)
: KviSubmenuAction(pParent,
	QString(KVI_COREACTION_CHANNELMENU),
	QString("channelsjoin.open"),
	__tr2qs("Join Channel"),
	__tr2qs("Shows a popup menu that allows quickly selecting a channel to join"),
	KviActionManager::categoryIrc(),
	"kvi_bigicon_channelspopup.png",
	QString("%1").arg(KVI_SMALLICON_CHANNEL),
	NeedsConnection | NeedsContext)
{
}

void KviJoinChannelAction::popupAboutToShow()
{
	KviConsole * c = g_pActiveWindow->console();
	if(!c) return;

	g_pApp->fillRecentChannelsPopup(m_pPopup,c);

	m_pPopup->insertSeparator();
	m_pPopup->insertItem(*(smallIcon()),__tr2qs("Other..."));
	m_pPopup->insertSeparator();
	m_pPopup->insertItem(__tr2qs("Clear Recent Channels List"));
}

void KviJoinChannelAction::popupActivated(int id)
{
	KviConsole * c = g_pActiveWindow->console();
	if(!c)return;

	QString szItemText = m_pPopup->text(id);
	if(!szItemText.isEmpty())
	{
		QString szText;
		if(szItemText == __tr2qs("Other..."))
		{
			szText = "channelsjoin.open";
		} else if(szItemText == __tr2qs("Clear Recent Channels List"))
		{
			szText = "option stringlistRecentChannels";
		} else {
			szText = szItemText;
			szText.prepend("join ");
		}
		KviKvsScript::run(szText,c);
	}
}



KviChangeNickAction::KviChangeNickAction(QObject * pParent)
: KviSubmenuAction(
	pParent,
	QString(KVI_COREACTION_NICKNAMEMENU),
	QString("dialog.textinput(\"Change Nickname\",\"Please enter the new nickname\",\"OK\",\"Cancel\"){ if($0 == 0 && $1 != \"\")nick $1; }"),
	__tr2qs("Change Nickname"),
	__tr2qs("Shows a popup menu that allows quickly changing the nickname"),
	KviActionManager::categoryIrc(),
	"kvi_bigicon_nickpopup.png",
	QString("%1").arg(KVI_SMALLICON_NICK),
	NeedsConnection | NeedsContext)
{
}

void KviChangeNickAction::popupAboutToShow()
{
	KviConsole * c = g_pActiveWindow->console();
	if(!c)return;

	g_pApp->fillRecentNicknamesPopup(m_pPopup,c);

	m_pPopup->insertSeparator();
	m_pPopup->insertItem(*(smallIcon()),__tr2qs("Other..."));

	m_pPopup->insertSeparator();
	m_pPopup->insertItem(*(smallIcon()),__tr2qs("Clear Recent Nicks List"));
}

void KviChangeNickAction::popupActivated(int id)
{
	KviConsole * c = g_pActiveWindow->console();
	if(!c)return;

	QString text = m_pPopup->text(id);
	if(!text.isEmpty())
	{
		QString szText;
		if(__tr2qs("Other...") == text)
		{
			activate();
		} else if(__tr2qs("Clear Recent Nicks List") == text)
		{
			KviKvsScript::run("option stringlistRecentNicknames",c);
		} else {
			szText = text;
			szText.prepend("nick ");
			KviKvsScript::run(szText,c);
		}
	}
}


KviConnectToServerAction::KviConnectToServerAction(QObject * pParent)
: KviSubmenuAction(
	pParent,
	QString(KVI_COREACTION_SERVERMENU),
	QString("options.edit KviServerOptionsWidget"),
	__tr2qs("Connect To"),
	__tr2qs("Shows a popup menu that allows quickly connecting to a server"),
	KviActionManager::categoryIrc(),
	"kvi_bigicon_server.png",
	QString("%1").arg(KVI_SMALLICON_SERVER),
	NeedsContext)
{
}

void KviConnectToServerAction::popupAboutToShow()
{
	g_pApp->fillRecentServersPopup(m_pPopup);
	m_pPopup->insertSeparator();
	m_pPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SERVER)),__tr2qs("Other..."));
	m_pPopup->insertSeparator();
	m_pPopup->insertItem(__tr2qs("Clear Recent Servers List"));
}

void KviConnectToServerAction::popupActivated(int id)
{
	KviConsole * c = g_pActiveWindow->console();
	if(!c)return;

	QString szItemText = m_pPopup->text(id);
	if(!szItemText.isEmpty())
	{
		if(szItemText == __tr2qs("Other..."))
		{
			activate();
		} else if(szItemText == __tr2qs("Clear Recent Servers List"))
		{
			KviKvsScript::run("option stringlistRecentServers",c);
		} else {
			KviStr szCommand;
			QString szText = szItemText;
			if(KviIrcUrl::parse(szText.toUtf8().data(),szCommand,KVI_IRCURL_CONTEXT_FIRSTFREE))
			{
				KviKvsScript::run(szCommand.ptr(),c);
			}
		}
	}
}


KviChangeUserModeAction::KviChangeUserModeAction(QObject * pParent)
: KviSubmenuAction(
	pParent,
	QString(KVI_COREACTION_USERMODEMENU),
	QString(""),
	__tr2qs("User Mode"),
	__tr2qs("Shows a popup menu that allows quickly changing user modes"),
	KviActionManager::categoryIrc(),
	"kvi_bigicon_usermode.png",
	QString("%1").arg(KVI_SMALLICON_MODE),
	NeedsContext | NeedsConnection)
{
}

void KviChangeUserModeAction::popupAboutToShow()
{
	KviConsole * c = g_pActiveWindow->console();
	if(!c)return;

	m_pPopup->clear();
	int id;
	QString szModes = g_pActiveWindow->connection()->serverInfo()->supportedUserModes();

	id = m_pPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_WALLOPS)),__tr2qs("Wallops (+w)"));
	m_pPopup->setItemChecked(id,c->connection()->userInfo()->hasUserMode('w'));
	id = m_pPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SERVERNOTICE)),__tr2qs("Server Notices (+s)"));
	m_pPopup->setItemChecked(id,c->connection()->userInfo()->hasUserMode('s'));
	id = m_pPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_INVISIBLE)),__tr2qs("Invisible (+i)"));
	m_pPopup->setItemChecked(id,c->connection()->userInfo()->hasUserMode('i'));

	szModes.replace("w","");
	szModes.replace("s","");
	szModes.replace("i","");
	while(!szModes.isEmpty())
	{
		QChar ccc = szModes[0];
		szModes.remove(0,1);
		id = m_pPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MODE)),QString("+%1 Mode").arg(ccc));
		m_pPopup->setItemChecked(id,c->connection()->userInfo()->hasUserMode(ccc.toLatin1()));
	}
}

void KviChangeUserModeAction::popupActivated(int id)
{
	KviConsole * c = g_pActiveWindow->console();
	if(!c)return;

	QString text = m_pPopup->text(id);
	if(!c->isConnected())return;
	if(!text.isEmpty())
	{
		int idx = text.indexOf('+');
		if(idx != -1)
		{
			idx++;
			if(idx < text.length())
			{
				QChar m = text[idx];
				QString command = "mode ";
				command.append(c->connection()->userInfo()->nickName());
				command.append(c->connection()->userInfo()->hasUserMode(m) ? " -" : " +");
				command.append(m);
				KviKvsScript::run(command,c);
			}
		}
	}
}


KviGoAwayAction::KviGoAwayAction(QObject * pParent)
: KviKvsAction(
		pParent,
		QString(KVI_COREACTION_AWAYBACK),
		QString("if($away())back; else away;"),
		__tr2qs("Away/Back"),
		__tr2qs("Allows entering and leaving away state"),
		KviActionManager::categoryIrc(),
		"kvi_bigicon_nokeyboard.png",
		QString("%1").arg(KVI_SMALLICON_NOTAWAY),
		KviKvsAction::NeedsContext | KviKvsAction::NeedsConnection,
		KVI_SHORTCUTS_AWAY
	)
{
}

void KviGoAwayAction::reloadImages()
{
	activeContextStateChanged();
}

void KviGoAwayAction::activeContextChanged()
{
	activeContextStateChanged();
}

void KviGoAwayAction::activeContextStateChanged()
{
	KviPointerList<QWidget> * bl = widgetList();
	if(!bl)return;

	QPixmap * p;
	QString txt;
	KviIrcContext * c = g_pFrame->activeContext();
	if(c)
	{
		if(c->state() == KviIrcContext::Connected)
		{
			if(c->connection()->userInfo()->isAway())
			{
				p = g_pIconManager->getBigIcon("kvi_bigicon_keyboard.png");
				txt = m_szBackString;
			} else {
				p = g_pIconManager->getBigIcon("kvi_bigicon_nokeyboard.png");
				txt = m_szAwayString;
			}
		} else {
			p = 0;
		}
	} else {
		p = 0;
	}

	if(p)
	{
		for(QToolButton * b = (QToolButton *)bl->first();b;b =(QToolButton *)bl->next())
		{
			if(!b->isEnabled())b->setEnabled(true);
			b->setIcon(QIcon(*p));
			b->setText(txt);
		}
		m_uInternalFlags |= KVI_ACTION_FLAG_ENABLED;
		setEnabled(true);
	} else {
		for(QToolButton * b = (QToolButton *)bl->first();b;b = (QToolButton *)bl->next())
		{
			if(b->isEnabled())b->setEnabled(false);
		}
		m_uInternalFlags &= ~KVI_ACTION_FLAG_ENABLED;
		setEnabled(true);
	}
}

void KviGoAwayAction::setup()
{
	KviKvsAction::setup();
	m_szAwayString = __tr2qs("Enter Away Mode");
	m_szBackString = __tr2qs("Leave Away Mode");
	connect(g_pFrame,SIGNAL(activeConnectionAwayStateChanged()),this,SLOT(activeContextStateChanged()));
}

bool KviGoAwayAction::addToPopupMenu(KviTalPopupMenu *p)
{
	if(!setupDone())setup();
	KviIrcContext * c = g_pFrame->activeContext();
	int id;
	QString t;
	if(c)
	{
		if(c->state() == KviIrcContext::Connected)
		{
			if(c->connection()->userInfo()->isAway())
			{
				t = m_szBackString;
				if(!m_szKeySequence.isEmpty())t += '\t' + m_szKeySequence;
				p->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_AWAY)),t,this,SLOT(activate()));
			} else {
				t = m_szAwayString;
				if(!m_szKeySequence.isEmpty())t += '\t' + m_szKeySequence;
				p->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NOTAWAY)),t,this,SLOT(activate()));
			}
		} else {
			t = m_szAwayString;
			if(!m_szKeySequence.isEmpty())t += '\t' + m_szKeySequence;
			id = p->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NOTAWAY)),t,this,SLOT(activate()));
			p->setItemEnabled(id,false);
		}
	} else {
		t = m_szAwayString;
		if(!m_szKeySequence.isEmpty())t += '\t' + m_szKeySequence;
		id = p->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NOTAWAY)),t,this,SLOT(activate()));
		p->setItemEnabled(id,false);
	}
	return true;
}

QWidget * KviGoAwayAction::addToCustomToolBar(KviCustomToolBar *t)
{
	if(!setupDone())setup();

	QToolButton * b = new QToolButton(t);
	b->setIcon(*(g_pIconManager->getBigIcon("kvi_bigicon_nokeyboard.png")));
	b->setText(m_szAwayString);
	b->setStatusTip(m_szAwayString);
	b->setObjectName(name());
	connect(b, SIGNAL(clicked()), this, SLOT(activate()));

	registerWidget(b);
	t->addWidget(b);
	activeContextStateChanged();
	return b;
}


KviIrcToolsAction::KviIrcToolsAction(QObject * pParent)
: KviSubmenuAction(
	pParent,
	QString(KVI_COREACTION_IRCTOOLS),
	QString(""),
	__tr2qs("Irc Tools"),
	__tr2qs("Shows a popup menu with some IRC Tools"),
	KviActionManager::categoryIrc(),
	"kvi_bigicon_tools.png",
	QString("%1").arg(KVI_SMALLICON_NONE),
	NeedsConnection | NeedsContext)
{
}

void KviIrcToolsAction::popupAboutToShow()
{
	m_pPopup->clear();
	KviAction * a;
	a = KviActionManager::instance()->getAction(KVI_COREACTION_SOCKETSPY);
	if(a)a->addToPopupMenu(m_pPopup);
	a = KviActionManager::instance()->getAction(KVI_COREACTION_SEPARATOR);
	if(a)a->addToPopupMenu(m_pPopup);
	a = KviActionManager::instance()->getAction(KVI_COREACTION_CHANNELLIST);
	if(a)a->addToPopupMenu(m_pPopup);
	a = KviActionManager::instance()->getAction(KVI_COREACTION_NETWORKLINKS);
	if(a)a->addToPopupMenu(m_pPopup);
}

void KviIrcToolsAction::popupActivated(int)
{
}


KviIrcOperationsAction::KviIrcOperationsAction(QObject * pParent)
: KviSubmenuAction(
	pParent,
	QString(KVI_COREACTION_IRCACTIONS),
	QString(""),
	__tr2qs("Irc Actions"),
	__tr2qs("Shows a popup menu with some IRC Actions"),
	KviActionManager::categoryIrc(),
	"kvi_bigicon_actions.png",
	QString("%1").arg(KVI_SMALLICON_NONE),
	NeedsConnection | NeedsContext)
{
}

void KviIrcOperationsAction::popupAboutToShow()
{
	m_pPopup->clear();
	KviAction * a;
	a = KviActionManager::instance()->getAction(KVI_COREACTION_AWAYBACK);
	if(a)a->addToPopupMenu(m_pPopup);
	a = KviActionManager::instance()->getAction(KVI_COREACTION_USERMODEMENU);
	if(a)a->addToPopupMenu(m_pPopup);
	a = KviActionManager::instance()->getAction(KVI_COREACTION_SEPARATOR);
	if(a)a->addToPopupMenu(m_pPopup);
	a = KviActionManager::instance()->getAction(KVI_COREACTION_CHANNELMENU);
	if(a)a->addToPopupMenu(m_pPopup);
	a = KviActionManager::instance()->getAction(KVI_COREACTION_NICKNAMEMENU);
	if(a)a->addToPopupMenu(m_pPopup);
	a = KviActionManager::instance()->getAction(KVI_COREACTION_SEPARATOR);
	if(a)a->addToPopupMenu(m_pPopup);
	a = KviActionManager::instance()->getAction(KVI_COREACTION_SERVERMENU);
	if(a)a->addToPopupMenu(m_pPopup);
}

void KviIrcOperationsAction::popupActivated(int)
{
}
