//=============================================================================
//
//   File : KviCoreActions.cpp
//   Creation date : Mon 22 Nov 2004 02:30:47 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviCoreActions.h"
#include "KviShortcut.h"
#include "KviMainWindow.h"
#include "KviLocale.h"
#include "KviIrcContext.h"
#include "KviIconManager.h"
#include "KviApplication.h"
#include "KviCustomToolBar.h"
#include "KviActionDrawer.h"
#include "KviActionManager.h"
#include "KviKvsAction.h"
#include "KviWindowStack.h"
#include "KviIrcToolBar.h"
#include "KviIrcUrl.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviCoreActionNames.h"
#include "KviIrcConnectionServerInfo.h"
#include "KviKvsScript.h"
#include "KviOptions.h"

#include <QObject>
#include <QMenu>

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

#define SCRIPT_ACTION(__name, __code, __visible, __description, __category, __icon, __smallicon, __flags, __accel) \
	a = new KviKvsAction(m,                                                                                        \
	    QString(__name),                                                                                           \
	    QString(__code),                                                                                           \
	    QString(__visible),                                                                                        \
	    QString(__description),                                                                                    \
	    __category,                                                                                                \
	    QString(__icon),                                                                                           \
	    __smallicon,                                                                                               \
	    __flags,                                                                                                   \
	    QKeySequence(__accel).toString());                                                                         \
	m->registerAction(a)

#define SLOT_ACTION(__name, __object, __slot, __visible, __description, __category, __icon, __smallicon, __flags, __accel) \
	a = new KviAction(m,                                                                                                   \
	    QString(__name),                                                                                                   \
	    QString(__visible),                                                                                                \
	    QString(__description),                                                                                            \
	    __category,                                                                                                        \
	    QString(__icon),                                                                                                   \
	    __smallicon,                                                                                                       \
	    __flags,                                                                                                           \
	    QKeySequence(__accel).toString());                                                                                 \
	QObject::connect(a, SIGNAL(activated()), __object, __slot);                                                            \
	m->registerAction(a)

	SCRIPT_ACTION(
	    KVI_COREACTION_SERVEROPTIONS,
	    "options.edit -n OptionsWidget_servers",
	    __tr2qs("Configure Servers..."),
	    __tr2qs("Allows you to configure the servers and eventually to connect to them"),
	    KviActionManager::categorySettings(),
	    "kvi_bigicon_serverconfiguration.png",
	    KviIconManager::ServerConfiguration,
	    0,
	    KVI_SHORTCUTS_SERVERS);

	SCRIPT_ACTION(
	    KVI_COREACTION_MANAGEADDONS,
	    "addon.dialog -t",
	    __tr2qs("Manage Addons..."),
	    __tr2qs("Allows you to manage the script-based addons"),
	    KviActionManager::categorySettings(),
	    "kvi_bigicon_addons.png",
	    KviIconManager::Addons,
	    0,
	    KVI_SHORTCUTS_MANAGE_ADDONS);

	SCRIPT_ACTION(
	    KVI_COREACTION_EDITREGUSERS,
	    "reguser.edit -t",
	    __tr2qs("Configure Registered Users..."),
	    __tr2qs("Shows a dialog that allows editing the registered user entries"),
	    KviActionManager::categorySettings(),
	    "kvi_bigicon_regusers.png",
	    KviIconManager::RegUsers,
	    0,
	    KVI_SHORTCUTS_USERS);

	SCRIPT_ACTION(
	    KVI_COREACTION_IDENTITYOPTIONS,
	    "options.edit OptionsWidget_identity",
	    __tr2qs("Configure Identity..."),
	    __tr2qs("Allows you to configure nickname, username, avatar etc..."),
	    KviActionManager::categorySettings(),
	    "kvi_bigicon_identity.png",
	    KviIconManager::Identity,
	    0,
	    KVI_SHORTCUTS_IDENTITY);

	SCRIPT_ACTION(
	    KVI_COREACTION_SOCKETSPY,
	    "socketspy.open",
	    __tr2qs("Show Socket Spy"),
	    __tr2qs("Shows a window that allows monitoring the socket traffic"),
	    KviActionManager::categoryIrc(),
	    "kvi_bigicon_socketspy.png",
	    KviIconManager::Spy,
	    KviAction::NeedsContext,
	    QString());

	SCRIPT_ACTION(
	    KVI_COREACTION_NETWORKLINKS,
	    "links.open",
	    __tr2qs("Get Network Links"),
	    __tr2qs("Shows a window that allows viewing the network links"),
	    KviActionManager::categoryIrc(),
	    "kvi_bigicon_links.png",
	    KviIconManager::Links,
	    KviAction::NeedsContext,
	    QString());

	SCRIPT_ACTION(
	    KVI_COREACTION_CHANNELLIST,
	    "list.open",
	    __tr2qs("Get Channel List"),
	    __tr2qs("Shows a window that allows listing the network channels"),
	    KviActionManager::categoryIrc(),
	    "kvi_bigicon_channellist.png",
	    KviIconManager::List,
	    KviAction::NeedsContext,
	    QString());

	SCRIPT_ACTION(
	    KVI_COREACTION_GENERALOPTIONS,
	    "options.dialog",
	    __tr2qs("Configure KVIrc..."),
	    __tr2qs("Shows the general options dialog"),
	    KviActionManager::categorySettings(),
	    "kvi_bigicon_settings.png",
	    KviIconManager::Options,
	    0,
	    KVI_SHORTCUTS_OPTIONS);

	SCRIPT_ACTION(
	    KVI_COREACTION_THEMEOPTIONS,
	    "options.dialog theme",
	    __tr2qs("Configure Theme..."),
	    __tr2qs("Shows the theme options dialog"),
	    KviActionManager::categorySettings(),
	    "kvi_bigicon_themeoptions.png",
	    KviIconManager::ThemeOptions,
	    0,
	    KVI_SHORTCUTS_THEME);

	SCRIPT_ACTION(
	    KVI_COREACTION_MANAGETHEMES,
	    "theme.dialog -t",
	    __tr2qs("Manage Themes..."),
	    __tr2qs("Allows you to manage the themes"),
	    KviActionManager::categorySettings(),
	    "kvi_bigicon_theme.png",
	    KviIconManager::Theme,
	    0,
	    KVI_SHORTCUTS_MANAGE_THEMES);

	SCRIPT_ACTION(
	    KVI_COREACTION_TOOLBAREDITOR,
	    "toolbareditor.open -t",
	    __tr2qs("Customize Toolbars..."),
	    __tr2qs("Shows a window that allows editing script toolbars"),
	    KviActionManager::categorySettings(),
	    "kvi_bigicon_toolbareditor.png",
	    KviIconManager::ToolBarEditor,
	    0,
	    KVI_SHORTCUTS_EDITORS_TOOLBAR);

	SCRIPT_ACTION(
	    KVI_COREACTION_JOINCHANNELS,
	    "channelsjoin.open",
	    __tr2qs("Join Channels..."),
	    __tr2qs("Shows a dialog that allows you join channels"),
	    KviActionManager::categoryIrc(),
	    "kvi_bigicon_channels.png",
	    KviIconManager::Channel,
	    KviKvsAction::NeedsConnection | KviKvsAction::NeedsContext,
	    KVI_SHORTCUTS_JOIN);

	SCRIPT_ACTION(
	    KVI_COREACTION_ACTIONEDITOR,
	    "actioneditor.open",
	    __tr2qs("Edit Actions..."),
	    __tr2qs("Shows a window that allows editing actions"),
	    KviActionManager::categoryScripting(),
	    "kvi_bigicon_actioneditor.png",
	    KviIconManager::ActionEditor,
	    0,
	    KVI_SHORTCUTS_EDITORS_ACTION);

	SCRIPT_ACTION(
	    KVI_COREACTION_ALIASEDITOR,
	    "aliaseditor.open",
	    __tr2qs("Edit Aliases..."),
	    __tr2qs("Shows a window that allows editing aliases"),
	    KviActionManager::categoryScripting(),
	    "kvi_bigicon_aliaseditor.png",
	    KviIconManager::AliasEditor,
	    0,
	    KVI_SHORTCUTS_EDITORS_ALIAS);

	SCRIPT_ACTION(
	    KVI_COREACTION_CLASSEDITOR,
	    "classeditor.open",
	    __tr2qs("Edit Classes..."),
	    __tr2qs("Shows a window that allows editing classes"),
	    KviActionManager::categoryScripting(),
	    "kvi_bigicon_classeditor.png",
	    KviIconManager::ClassEditor,
	    0,
	    KVI_SHORTCUTS_EDITORS_CLASS);

	SCRIPT_ACTION(
	    KVI_COREACTION_EVENTEDITOR,
	    "eventeditor.open",
	    __tr2qs("Edit Events..."),
	    __tr2qs("Shows a window that allows editing script events"),
	    KviActionManager::categoryScripting(),
	    "kvi_bigicon_eventeditor.png",
	    KviIconManager::EventEditor,
	    0,
	    KVI_SHORTCUTS_EDITORS_EVENT);

	SCRIPT_ACTION(
	    KVI_COREACTION_POPUPEDITOR,
	    "popupeditor.open",
	    __tr2qs("Edit Popups..."),
	    __tr2qs("Shows a window that allows editing popup menus"),
	    KviActionManager::categoryScripting(),
	    "kvi_bigicon_popupeditor.png",
	    KviIconManager::PopupEditor,
	    0,
	    KVI_SHORTCUTS_EDITORS_POPUP);

	SCRIPT_ACTION(
	    KVI_COREACTION_RAWEDITOR,
	    "raweditor.open",
	    __tr2qs("Edit RAW Events..."),
	    __tr2qs("Shows a window that allows editing RAW scripting events"),
	    KviActionManager::categoryScripting(),
	    "kvi_bigicon_raweditor.png",
	    KviIconManager::RawEventEditor,
	    0,
	    KVI_SHORTCUTS_EDITORS_RAW);

	SCRIPT_ACTION(
	    KVI_COREACTION_CODETESTER,
	    "codetester.open",
	    __tr2qs("Script Tester"),
	    __tr2qs("Creates an embedded editor for long scripts"),
	    KviActionManager::categoryScripting(),
	    "kvi_bigicon_codetester.png",
	    KviIconManager::Bomb,
	    0,
	    KVI_SHORTCUTS_EDITORS_TESTER);

	SCRIPT_ACTION(
	    KVI_COREACTION_EXECUTEKVS,
	    "dialog.file(open,$tr(\"Select a File - KVIrc\")){ if(!$str.isEmpty($0))parse $0; }",
	    __tr2qs("Execute Script..."),
	    __tr2qs("Allows executing a KVS script from file"),
	    KviActionManager::categoryScripting(),
	    "kvi_bigicon_folder.png",
	    KviIconManager::Folder,
	    0,
	    KVI_SHORTCUTS_EXEC);

	SCRIPT_ACTION(
	    KVI_COREACTION_HELPINDEX,
	    "help.open -m",
	    __tr2qs("Help Index"),
	    __tr2qs("Shows the documentation index"),
	    KviActionManager::categoryGeneric(),
	    "kvi_bigicon_help.png",
	    KviIconManager::Help,
	    0,
	    KVI_SHORTCUTS_HELP);

	SCRIPT_ACTION(
	    KVI_COREACTION_KVIRCHOMEPAGE,
	    "openurl http://www.kvirc.net",
	    __tr2qs("KVIrc WWW"),
	    __tr2qs("Opens the KVIrc homepage"),
	    KviActionManager::categoryGeneric(),
	    "kvi_bigicon_homepage.png",
	    KviIconManager::HomePage,
	    0,
	    QString());

	SCRIPT_ACTION(
	    KVI_COREACTION_KVIRCRUHOMEPAGE,
	    "openurl http://www.kvirc.ru",
	    __tr2qs("KVIrc Russian WWW"),
	    __tr2qs("Opens the KVIrc homepage in Russian"),
	    KviActionManager::categoryGeneric(),
	    "kvi_bigicon_homepage.png",
	    KviIconManager::HomePage,
	    0,
	    QString());

	SCRIPT_ACTION(
	    KVI_COREACTION_SCREENSHOT,
	    "theme.screenshot",
	    __tr2qs("Acquire Screenshot"),
	    __tr2qs("Acquires a screenshot of the KVIrc main window"),
	    KviActionManager::categoryGeneric(),
	    "kvi_bigicon_screenshot.png",
	    KviIconManager::ScreenShot,
	    0,
	    QString());

	SCRIPT_ACTION(
	    KVI_COREACTION_FILETRANSFER,
	    "filetransferwindow.open",
	    __tr2qs("Manage File &Transfers"),
	    __tr2qs("Shows a window that lists file transfers"),
	    KviActionManager::categoryGeneric(),
	    "kvi_bigicon_filetransfer.png",
	    KviIconManager::FileTransfer,
	    0,
	    QString());

	SCRIPT_ACTION(
	    KVI_COREACTION_IOGRAPH,
	    "iograph.open",
	    __tr2qs("Show I/O &Traffic Graph"),
	    __tr2qs("Shows a graph representing I/O bandwidth traffic"),
	    KviActionManager::categoryIrc(),
	    "kvi_bigicon_sysmonitor.png",
	    KviIconManager::SysMonitor,
	    0,
	    QString());

	SCRIPT_ACTION(
	    KVI_COREACTION_LOGVIEWER,
	    "logview.open",
	    __tr2qs("Browse Log Files"),
	    __tr2qs("Shows a window that lists and filter log files"),
	    KviActionManager::categoryGeneric(),
	    "kvi_bigicon_log.png",
	    KviIconManager::Log,
	    0,
	    QString());

	SCRIPT_ACTION(
	    KVI_COREACTION_SHAREDFILES,
	    "sharedfileswindow.open",
	    __tr2qs("Manage S&hared Files"),
	    __tr2qs("Shows a window that list shared files"),
	    KviActionManager::categoryGeneric(),
	    "kvi_bigicon_sharedfiles.png",
	    KviIconManager::SharedFiles,
	    0,
	    QString());

	SCRIPT_ACTION(
	    KVI_COREACTION_URLLIST,
	    "url.list",
	    __tr2qs("View URL List"),
	    __tr2qs("Shows a window that lists caught URLs"),
	    KviActionManager::categoryGeneric(),
	    "kvi_bigicon_url.png",
	    KviIconManager::Url,
	    0,
	    QString());

	SLOT_ACTION(
	    KVI_COREACTION_NEWIRCCONTEXT,
	    g_pMainWindow,
	    SLOT(newConsole()),
	    __tr2qs("New IRC Context"),
	    __tr2qs("Creates a new IRC context console"),
	    KviActionManager::categoryIrc(),
	    "kvi_bigicon_newitem.png",
	    KviIconManager::NewItem,
	    0,
	    KVI_SHORTCUTS_CONTEXT);

	SLOT_ACTION(
	    KVI_COREACTION_QUITKVIRC,
	    g_pMainWindow,
	    SLOT(close()),
	    __tr2qs("&Quit KVIrc"),
	    __tr2qs("Quits KVIrc closing all the current connections"),
	    KviActionManager::categoryIrc(),
	    "kvi_bigicon_quitapp.png",
	    KviIconManager::QuitApp,
	    0,
	    KVI_SHORTCUTS_QUIT);
}

KviIrcContextDisplayAction::KviIrcContextDisplayAction(QObject * pParent)
    : KviAction(pParent,
          QString(KVI_COREACTION_IRCCONTEXTDISPLAY),
          __tr2qs("IRC Context Display"),
          __tr2qs("Shows a small display with IRC context information"),
          KviActionManager::categoryIrc(),
          "kvi_bigicon_irccontextdisplay.png",
          KviIconManager::PopupMenu,
          NeedsContext | NeedsConnection)
{
}

void KviIrcContextDisplayAction::setup()
{
	KviAction::setup();
	connect(g_pMainWindow, SIGNAL(activeConnectionServerInfoChanged()), this, SLOT(activeContextStateChanged()));
	connect(g_pMainWindow, SIGNAL(activeConnectionNickNameChanged()), this, SLOT(activeContextStateChanged()));
	connect(g_pMainWindow, SIGNAL(activeConnectionUserModeChanged()), this, SLOT(activeContextStateChanged()));
	connect(g_pMainWindow, SIGNAL(activeConnectionAwayStateChanged()), this, SLOT(activeContextStateChanged()));
	connect(g_pMainWindow, SIGNAL(activeConnectionLagChanged()), this, SLOT(activeContextStateChanged()));
}

bool KviIrcContextDisplayAction::addToPopupMenu(QMenu *)
{
	// QT4SUX: Widgets can be no longer added to popup menus.. what about labels ?
	return true;
}

QAction * KviIrcContextDisplayAction::addToCustomToolBar(KviCustomToolBar * t)
{
	if(!setupDone())
		setup();
	KviIrcContextDisplay * w = new KviIrcContextDisplay(t, KVI_COREACTION_IRCCONTEXTDISPLAY);
	QAction * pAction = t->addWidget(w);
	// important: when customizing the toolbar, we'll get the action name from QAction::objectName();
	pAction->setObjectName(KVI_COREACTION_IRCCONTEXTDISPLAY);
	registerAction(pAction);
	return pAction;
}

void KviIrcContextDisplayAction::activeContextChanged()
{
	for(auto & pAction : m_pActionList)
	{
		QToolBar * t = (QToolBar *)pAction->parentWidget();
		if(t)
		{
			KviIrcContextDisplay * w = (KviIrcContextDisplay *)t->widgetForAction(pAction);
			if(w)
				w->update();
		}
	}
}

void KviIrcContextDisplayAction::activeContextStateChanged()
{
	for(auto & pAction : m_pActionList)
	{
		QToolBar * t = (QToolBar *)pAction->parentWidget();
		if(t)
		{
			KviIrcContextDisplay * w = (KviIrcContextDisplay *)t->widgetForAction(pAction);
			if(w)
				w->update();
		}
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
          KviIconManager::None,
          0)
{
}

bool KviSeparatorAction::addToPopupMenu(QMenu * p)
{
	if(!setupDone())
		setup();
	p->addSeparator();
	return true;
}

QAction * KviSeparatorAction::addToCustomToolBar(KviCustomToolBar * t)
{
	if(!setupDone())
		setup();
	QWidget * w = new KviCustomToolBarSeparator(t, KVI_COREACTION_SEPARATOR);
	QAction * pAction = t->addWidget(w);
	// important: when customizing the toolbar, we'll get the action name from QAction::objectName();
	pAction->setObjectName(KVI_COREACTION_SEPARATOR);
	registerAction(pAction);
	return pAction;
}

KviConnectAction::KviConnectAction(QObject * pParent)
    : KviAction(pParent,
          QString(KVI_COREACTION_CONNECT),
          __tr2qs("Connect/Disconnect"),
          __tr2qs("Allows you to connect to a server or to terminate the current connection"),
          KviActionManager::categoryIrc(),
          KVI_BIGICON_CONNECTED,
          KviIconManager::None, // no small icon for now
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

	std::unordered_set<QAction *> const & bl = actionList();
	if(bl.empty())
		return;

	bool bIconVisibleInMenu = KVI_OPTION_BOOL(KviOption_boolShowIconsInPopupMenus);
	QPixmap * p;
	QString txt;
	KviIrcContext * c = g_pMainWindow->activeContext();
	if(c)
	{
		switch(c->state())
		{
			case KviIrcContext::Idle:
				p = g_pIconManager->getBigIcon(KVI_BIGICON_DISCONNECTED);
				txt = m_szConnectString;
				break;
			case KviIrcContext::PendingReconnection:
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
				p = nullptr;
				break;
		}
	}
	else
	{
		p = nullptr;
	}

	if(p)
	{
		for(auto & b : bl)
		{
			if(!b->isEnabled())
				b->setEnabled(true);
			b->setIcon(QIcon(*p));
			b->setText(txt);
			b->setIconVisibleInMenu(bIconVisibleInMenu);
		}
	}
	else
	{
		for(auto & b : bl)
		{
			if(b->isEnabled())
				b->setEnabled(false);
			b->setIconVisibleInMenu(bIconVisibleInMenu);
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
	KviIrcContext * c = g_pMainWindow->activeContext();
	if(c)
		c->connectOrDisconnect();
}

bool KviConnectAction::addToPopupMenu(QMenu * p)
{
	if(!setupDone())
		setup();
	KviIrcContext * c = g_pMainWindow->activeContext();
	QAction * pAction;
	QString t;
	if(c)
	{
		switch(c->state())
		{
			case KviIrcContext::Idle:
				t = m_szConnectString;
				if(!m_szKeySequence.isEmpty())
					t += '\t' + m_szKeySequence;
				p->addAction(t, this, SLOT(activate()));
				break;
			case KviIrcContext::PendingReconnection:
			case KviIrcContext::Connecting:
			case KviIrcContext::LoggingIn:
				t = m_szAbortConnectionString;
				if(!m_szKeySequence.isEmpty())
					t += '\t' + m_szKeySequence;
				p->addAction(t, this, SLOT(activate()));
				break;
			case KviIrcContext::Connected:
				t = m_szDisconnectString;
				if(!m_szKeySequence.isEmpty())
					t += '\t' + m_szKeySequence;
				p->addAction(t, this, SLOT(activate()));
				break;
			default:
				t = m_szConnectString;
				if(!m_szKeySequence.isEmpty())
					t += '\t' + m_szKeySequence;
				pAction = p->addAction(t, this, SLOT(activate()));
				pAction->setEnabled(false);
				break;
		}
	}
	else
	{
		t = m_szConnectString;
		if(!m_szKeySequence.isEmpty())
			t += '\t' + m_szKeySequence;
		pAction = p->addAction(t, this, SLOT(activate()));
		pAction->setEnabled(false);
	}
	return true;
}

QAction * KviConnectAction::addToCustomToolBar(KviCustomToolBar * t)
{
	if(!setupDone())
		setup();
	QAction * pAction = new QAction(*(g_pIconManager->getBigIcon(KVI_BIGICON_DISCONNECTED)), m_szConnectString, t);
	pAction->setStatusTip(m_szConnectString);
	pAction->setObjectName(KVI_COREACTION_CONNECT);
	t->addAction(pAction);
	connect(pAction, SIGNAL(triggered()), this, SLOT(activate()));
	registerAction(pAction);
	activeContextStateChanged();
	return pAction;
}

KviSubmenuAction::KviSubmenuAction(
    QObject * pParent,
    const QString & szName,
    const QString & szScriptCode,
    const QString & szVisibleName,
    const QString & szDescription,
    KviActionCategory * pCategory,
    const QString & szBigIconId,
    KviIconManager::SmallIcon eSmallIcon,
    unsigned int uFlags)
    : KviKvsAction(
          pParent,
          szName,
          szScriptCode,
          szVisibleName,
          szDescription,
          pCategory,
          szBigIconId,
          eSmallIcon,
          uFlags)
{
	m_pPopup = nullptr;
}

KviSubmenuAction::~KviSubmenuAction()
{
	if(m_pPopup)
		delete m_pPopup;
}

void KviSubmenuAction::setup()
{
	KviKvsAction::setup();
	if(!m_pPopup)
	{
		m_pPopup = new QMenu();
		connect(m_pPopup, SIGNAL(aboutToShow()), this, SLOT(popupAboutToShow()));
		connect(m_pPopup, SIGNAL(triggered(QAction *)), this, SLOT(popupActivated(QAction *)));
	}
}

void KviSubmenuAction::popupAboutToShow()
{
}

void KviSubmenuAction::popupActivated(QAction *)
{
}

bool KviSubmenuAction::addToPopupMenu(QMenu * p)
{
	if(!setupDone())
		setup();
	QAction * pAction = p->addAction(*(smallIcon()), visibleName());
	pAction->setMenu(m_pPopup);
	if(!isEnabled())
		pAction->setEnabled(false);
	return true;
}

QAction * KviSubmenuAction::addToCustomToolBar(KviCustomToolBar * t)
{
	if(!setupDone())
		setup();
	QPixmap * p = bigIcon();

	QAction * pAction = new QAction(p ? *p : QPixmap(), visibleName(), t);
	pAction->setStatusTip(visibleName());
	pAction->setObjectName(name());
	t->addAction(pAction);
	connect(pAction, SIGNAL(triggered()), this, SLOT(activate()));
	pAction->setMenu(m_pPopup);

	QToolButton * pButton = (QToolButton *)t->widgetForAction(pAction);
	if(pButton)
		pButton->setPopupMode(scriptCode().isEmpty() ? QToolButton::InstantPopup : QToolButton::MenuButtonPopup);

	registerAction(pAction);
	if(!isEnabled())
		pAction->setEnabled(false);
	return pAction;
}

KviJoinChannelAction::KviJoinChannelAction(QObject * pParent)
    : KviSubmenuAction(pParent,
          QString(KVI_COREACTION_CHANNELMENU),
          QString("channelsjoin.open"),
          __tr2qs("Join Channel"),
          __tr2qs("Shows a popup menu that allows quickly selecting a channel to join"),
          KviActionManager::categoryIrc(),
          "kvi_bigicon_channelspopup.png",
          KviIconManager::Channel,
          NeedsConnection | NeedsContext)
{
}

void KviJoinChannelAction::popupAboutToShow()
{
	KviConsoleWindow * c = g_pActiveWindow->console();
	if(!c)
		return;

	g_pApp->fillRecentChannelsPopup(m_pPopup, c);

	m_pPopup->addSeparator();
	m_pPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Channel)), __tr2qs("Other..."));
	m_pPopup->addSeparator();
	m_pPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Remove)), __tr2qs("Clear Recent Channels List"));
}

void KviJoinChannelAction::popupActivated(QAction * pAction)
{
	KviConsoleWindow * c = g_pActiveWindow->console();
	if(!c)
		return;

	QString szItemText = pAction->text();
	if(!szItemText.isEmpty())
	{
		QString szText;
		if(szItemText == __tr2qs("Other..."))
		{
			szText = "channelsjoin.open";
		}
		else if(szItemText == __tr2qs("Clear Recent Channels List"))
		{
			szText = "option stringlistRecentChannels";
		}
		else
		{
			szText = szItemText;
			szText.prepend("join ");
		}
		KviKvsScript::run(szText, c);
	}
}

KviChangeNickAction::KviChangeNickAction(QObject * pParent)
    : KviSubmenuAction(
          pParent,
          QString(KVI_COREACTION_NICKNAMEMENU),
          QString("dialog.textinput(\"" + __tr2qs("Change Nickname") + "\",\"" + __tr2qs("Please enter the new nickname") + "\",\"" + __tr2qs("OK") + "\",\"" + __tr2qs("Cancel") + R"("){ if($0 == 0 && $1 != "")nick $1; })"),
          __tr2qs("Change Nickname"),
          __tr2qs("Shows a popup menu that allows quickly changing the nickname"),
          KviActionManager::categoryIrc(),
          "kvi_bigicon_nickpopup.png",
          KviIconManager::NickPopup,
          NeedsConnection | NeedsContext)
{
}

void KviChangeNickAction::popupAboutToShow()
{
	KviConsoleWindow * c = g_pActiveWindow->console();
	if(!c)
		return;

	g_pApp->fillRecentNicknamesPopup(m_pPopup, c);

	m_pPopup->addSeparator();
	m_pPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::NickPopup)), __tr2qs("Other..."));

	m_pPopup->addSeparator();
	m_pPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Remove)), __tr2qs("Clear Recent Nicks List"));
}

void KviChangeNickAction::popupActivated(QAction * pAction)
{
	KviConsoleWindow * c = g_pActiveWindow->console();
	if(!c)
		return;

	QString text = pAction->text();
	if(!text.isEmpty())
	{
		QString szText;
		if(__tr2qs("Other...") == text)
		{
			activate();
		}
		else if(__tr2qs("Clear Recent Nicks List") == text)
		{
			KviKvsScript::run("option stringlistRecentNicknames", c);
		}
		else
		{
			szText = text;
			szText.prepend("nick ");
			KviKvsScript::run(szText, c);
		}
	}
}

KviConnectToServerAction::KviConnectToServerAction(QObject * pParent)
    : KviSubmenuAction(
          pParent,
          QString(KVI_COREACTION_SERVERMENU),
          QString("options.edit -n OptionsWidget_servers"),
          __tr2qs("Connect to"),
          __tr2qs("Shows a popup menu that allows quickly connecting to a server"),
          KviActionManager::categoryIrc(),
          "kvi_bigicon_server.png",
          KviIconManager::Server,
          NeedsContext)
{
}

void KviConnectToServerAction::popupAboutToShow()
{
	g_pApp->fillRecentServersPopup(m_pPopup);
	m_pPopup->addSeparator();
	m_pPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Server)), __tr2qs("Other..."));
	m_pPopup->addSeparator();
	m_pPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Remove)), __tr2qs("Clear Recent Servers List"));
}

void KviConnectToServerAction::popupActivated(QAction * pAction)
{
	KviConsoleWindow * c = g_pActiveWindow->console();
	if(!c)
		return;

	QString szItemText = pAction->text();
	if(!szItemText.isEmpty())
	{
		if(szItemText == __tr2qs("Other..."))
		{
			activate();
		}
		else if(szItemText == __tr2qs("Clear Recent Servers List"))
		{
			KviKvsScript::run("option stringlistRecentServers", c);
		}
		else
		{
			KviCString szCommand;
			QString szText = szItemText;
			if(KviIrcUrl::parse(szText.toUtf8().data(), szCommand, KVI_IRCURL_CONTEXT_FIRSTFREE))
			{
				KviKvsScript::run(szCommand.ptr(), c);
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
          KviIconManager::UserMode,
          NeedsContext | NeedsConnection)
{
}

void KviChangeUserModeAction::popupAboutToShow()
{
	KviConsoleWindow * c = g_pActiveWindow->console();
	if(!c)
		return;

	m_pPopup->clear();
	QAction * pAction;
	QString szModes = g_pActiveWindow->connection()->serverInfo()->supportedUserModes();

	pAction = m_pPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::WallOps)), __tr2qs("w: WALLOPS"));
	pAction->setCheckable(true);
	pAction->setChecked(c->connection()->userInfo()->hasUserMode('w'));

	if(szModes.contains('s'))
	{
		pAction = m_pPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::ServerNotice)), __tr2qs("s: Server Notices"));
		pAction->setCheckable(true);
		// Only if the IRCd allows us
		pAction->setEnabled(c->connection()->serverInfo()->getNeedsOperToSetS() ? c->connection()->userInfo()->hasUserMode('o') : true);
		pAction->setChecked(c->connection()->userInfo()->hasUserMode('s'));
	}

	pAction = m_pPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Invisible)), __tr2qs("i: Invisible"));
	pAction->setCheckable(true);
	pAction->setChecked(c->connection()->userInfo()->hasUserMode('i'));

	szModes.replace("w", "");
	szModes.replace("s", "");
	szModes.replace("i", "");
	while(!szModes.isEmpty())
	{
		QChar ccc = szModes[0];
		szModes.remove(0, 1);
		QString modeDesc = c->connection()->serverInfo()->getUserModeDescription(ccc);
		QChar requiredMode = c->connection()->serverInfo()->getUserModeRequirement(ccc);
		bool canCheck = true;
		bool isSet = c->connection()->userInfo()->hasUserMode(ccc.toLatin1());

		// See if the usermode is even settable by the user
		if(!requiredMode.isNull())
		{
			// Untouchable
			if(requiredMode == '!')
				canCheck = false;
			else
				canCheck = c->connection()->userInfo()->hasUserMode(requiredMode.toLatin1());
		}

		pAction = m_pPopup->addAction(*(g_pIconManager->getSmallIcon(isSet ? KviIconManager::ChanMode : KviIconManager::UnsetMode)),
		    !modeDesc.isEmpty() ? modeDesc : QString("%1: Unknown").arg(ccc));
		pAction->setEnabled(canCheck);
		pAction->setCheckable(true);
		pAction->setChecked(isSet);
	}
}

void KviChangeUserModeAction::popupActivated(QAction * pAction)
{
	KviConsoleWindow * c = g_pActiveWindow->console();
	if(!c)
		return;

	QString text = pAction->text();
	if(!c->isConnected())
		return;
	if(!text.isEmpty() && text[0].isLetter())
	{
		QChar m = text[0];
		QString szNick = c->connection()->userInfo()->nickName();
		KviQString::escapeKvs(&szNick);
		QString command = "mode ";
		command.append(szNick);
		command.append(c->connection()->userInfo()->hasUserMode(m) ? " -" : " +");
		command.append(m);
		KviKvsScript::run(command, c);
	}
}

KviGoAwayAction::KviGoAwayAction(QObject * pParent)
    : KviKvsAction(
          pParent,
          QString(KVI_COREACTION_AWAYBACK),
          QString("if($away())back; else away -d;"),
          __tr2qs("Away/Back"),
          __tr2qs("Allows entering and leaving away state"),
          KviActionManager::categoryIrc(),
          "kvi_bigicon_away.png",
          KviIconManager::Away,
          KviKvsAction::NeedsContext | KviKvsAction::NeedsConnection,
          KVI_SHORTCUTS_AWAY)
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
	std::unordered_set<QAction *> const & bl = actionList();
	if(bl.empty())
		return;

	bool bIconVisibleInMenu = KVI_OPTION_BOOL(KviOption_boolShowIconsInPopupMenus);
	QPixmap * p;
	QString txt;
	KviIrcContext * c = g_pMainWindow->activeContext();
	if(c)
	{
		if(c->state() == KviIrcContext::Connected)
		{
			if(c->connection()->userInfo()->isAway())
			{
				p = g_pIconManager->getBigIcon("kvi_bigicon_away.png");
				txt = m_szBackString;
			}
			else
			{
				p = g_pIconManager->getBigIcon("kvi_bigicon_notaway.png");
				txt = m_szAwayString;
			}
		}
		else
		{
			p = nullptr;
		}
	}
	else
	{
		p = nullptr;
	}

	if(p)
	{
		for(auto & b : bl)
		{
			if(!b->isEnabled())
				b->setEnabled(true);
			b->setIcon(QIcon(*p));
			b->setText(txt);
			b->setIconVisibleInMenu(bIconVisibleInMenu);
		}
		m_uInternalFlags |= KviAction::Enabled;
	}
	else
	{
		for(auto & b : bl)
		{
			if(b->isEnabled())
				b->setEnabled(false);
			b->setIconVisibleInMenu(bIconVisibleInMenu);
		}
		m_uInternalFlags &= ~KviAction::Enabled;
	}

	setEnabled(true);
}

void KviGoAwayAction::setup()
{
	KviKvsAction::setup();
	m_szAwayString = __tr2qs("Enter Away Mode");
	m_szBackString = __tr2qs("Leave Away Mode");
	connect(g_pMainWindow, SIGNAL(activeConnectionAwayStateChanged()), this, SLOT(activeContextStateChanged()));
}

bool KviGoAwayAction::addToPopupMenu(QMenu * p)
{
	if(!setupDone())
		setup();
	KviIrcContext * c = g_pMainWindow->activeContext();
	QAction * pAction;
	QString t;
	if(c)
	{
		if(c->state() == KviIrcContext::Connected)
		{
			if(c->connection()->userInfo()->isAway())
			{
				t = m_szBackString;
				if(!m_szKeySequence.isEmpty())
					t += '\t' + m_szKeySequence;
				p->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Away)), t, this, SLOT(activate()));
			}
			else
			{
				t = m_szAwayString;
				if(!m_szKeySequence.isEmpty())
					t += '\t' + m_szKeySequence;
				p->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::NotAway)), t, this, SLOT(activate()));
			}
		}
		else
		{
			t = m_szAwayString;
			if(!m_szKeySequence.isEmpty())
				t += '\t' + m_szKeySequence;
			pAction = p->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::NotAway)), t, this, SLOT(activate()));
			pAction->setEnabled(false);
		}
	}
	else
	{
		t = m_szAwayString;
		if(!m_szKeySequence.isEmpty())
			t += '\t' + m_szKeySequence;
		pAction = p->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::NotAway)), t, this, SLOT(activate()));
		pAction->setEnabled(false);
	}
	return true;
}

QAction * KviGoAwayAction::addToCustomToolBar(KviCustomToolBar * t)
{
	if(!setupDone())
		setup();

	QAction * pAction = new QAction(*(g_pIconManager->getBigIcon("kvi_bigicon_away.png")), m_szAwayString, t);
	pAction->setStatusTip(m_szAwayString);
	pAction->setObjectName(KVI_COREACTION_AWAYBACK);
	t->addAction(pAction);
	connect(pAction, SIGNAL(triggered()), this, SLOT(activate()));

	registerAction(pAction);
	activeContextStateChanged();
	return pAction;
}

KviIrcToolsAction::KviIrcToolsAction(QObject * pParent)
    : KviSubmenuAction(
          pParent,
          QString(KVI_COREACTION_IRCTOOLS),
          QString(""),
          __tr2qs("IRC Tools"),
          __tr2qs("Shows a popup menu with some IRC tools"),
          KviActionManager::categoryIrc(),
          "kvi_bigicon_tools.png",
          KviIconManager::Tools,
          NeedsConnection | NeedsContext)
{
}

void KviIrcToolsAction::popupAboutToShow()
{
	m_pPopup->clear();
	KviAction * a;
	a = KviActionManager::instance()->getAction(KVI_COREACTION_SOCKETSPY);
	if(a)
		a->addToPopupMenu(m_pPopup);
	a = KviActionManager::instance()->getAction(KVI_COREACTION_SEPARATOR);
	if(a)
		a->addToPopupMenu(m_pPopup);
	a = KviActionManager::instance()->getAction(KVI_COREACTION_CHANNELLIST);
	if(a)
		a->addToPopupMenu(m_pPopup);
	a = KviActionManager::instance()->getAction(KVI_COREACTION_NETWORKLINKS);
	if(a)
		a->addToPopupMenu(m_pPopup);
}

void KviIrcToolsAction::popupActivated(QAction *)
{
}

KviIrcOperationsAction::KviIrcOperationsAction(QObject * pParent)
    : KviSubmenuAction(
          pParent,
          QString(KVI_COREACTION_IRCACTIONS),
          QString(""),
          __tr2qs("IRC Actions"),
          __tr2qs("Shows a popup menu with some IRC actions"),
          KviActionManager::categoryIrc(),
          "kvi_bigicon_actions.png",
          KviIconManager::Action,
          NeedsConnection | NeedsContext)
{
}

void KviIrcOperationsAction::popupAboutToShow()
{
	m_pPopup->clear();
	KviAction * a;
	a = KviActionManager::instance()->getAction(KVI_COREACTION_AWAYBACK);
	if(a)
		a->addToPopupMenu(m_pPopup);
	a = KviActionManager::instance()->getAction(KVI_COREACTION_USERMODEMENU);
	if(a)
		a->addToPopupMenu(m_pPopup);
	a = KviActionManager::instance()->getAction(KVI_COREACTION_SEPARATOR);
	if(a)
		a->addToPopupMenu(m_pPopup);
	a = KviActionManager::instance()->getAction(KVI_COREACTION_CHANNELMENU);
	if(a)
		a->addToPopupMenu(m_pPopup);
	a = KviActionManager::instance()->getAction(KVI_COREACTION_NICKNAMEMENU);
	if(a)
		a->addToPopupMenu(m_pPopup);
	a = KviActionManager::instance()->getAction(KVI_COREACTION_SEPARATOR);
	if(a)
		a->addToPopupMenu(m_pPopup);
	a = KviActionManager::instance()->getAction(KVI_COREACTION_SERVERMENU);
	if(a)
		a->addToPopupMenu(m_pPopup);
}

void KviIrcOperationsAction::popupActivated(QAction *)
{
}
