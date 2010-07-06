//=============================================================================
//
//   File : libkvitrayicon.cpp
//   Creation date : Tue Jan 02 2001 14:34:12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2007 Alexey Uzhva (wizard at opendoor dot ru)
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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

#include "libkvitrayicon.h"

#include "kvi_settings.h"
#include "kvi_app.h"
#include "kvi_module.h"
#include "kvi_locale.h"
#include "kvi_memmove.h"
#include "kvi_windowlist.h"
#include "kvi_window.h"
#include "kvi_dynamictooltip.h"
#include "kvi_iconmanager.h"
#include "kvi_internalcmd.h"
#include "kvi_console.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnectionuserinfo.h"
#include "kvi_options.h"
#include "kvi_ircview.h"
#include "kvi_tal_popupmenu.h"

#include <QPixmap>
#include <QPainter>
#include <QTimer>
#include <QEvent>
#include <QRegExp>

#include <stdlib.h>
#include <time.h>

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	#define ICON_SIZE 16
#else
	#define ICON_SIZE 22
#endif

extern KVIRC_API KviPointerHashTable<QString,KviWindow> * g_pGlobalWindowDict;
static KviPointerList<KviTrayIcon> * g_pTrayIconList = 0;

static QPixmap * g_pDock1 = 0;
static QPixmap * g_pDock2 = 0;
static QPixmap * g_pDock3 = 0;

KviTrayIcon::KviTrayIcon(KviFrame * frm)
: QSystemTrayIcon(frm), m_CurrentPixmap(ICON_SIZE,ICON_SIZE)
{
	m_pContextPopup = new KviTalPopupMenu(0);
	setContextMenu(m_pContextPopup);

	m_iConsoles = 0;
	m_iChannels = 0;
	m_iQueries  = 0;
	m_iOther    = 0;

	m_pFlashingTimer = new QTimer(this);
	m_pFlashingTimer->setObjectName("flashing_timer");
	connect( m_pFlashingTimer, SIGNAL(timeout()), this, SLOT(flashingTimerShot()) );
	m_bFlashed=0;
	g_pTrayIconList->append(this);
	m_pFrm = frm;
	m_pFrm->setDockExtension(this);

	m_pTip = new KviDynamicToolTip(frm,"dock_tooltip");
	m_pAwayPopup = new KviTalPopupMenu(0);

	m_pTitleLabel = new QLabel(__tr2qs("<b>KVIrc</b>"),m_pContextPopup);
	QPalette p;
	m_pTitleLabel->setStyleSheet("background-color: " + p.color(QPalette::Normal, QPalette::Mid).name());
	m_pContextPopup->insertItem(m_pTitleLabel);
	m_pContextPopup->setWindowTitle(__tr2qs("Context"));
	m_pAwayMenuId = m_pContextPopup->addMenu(m_pAwayPopup);
	m_pAwayMenuId->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_AWAY)));
	m_pAwayMenuId->setText(__tr2qs("Away"));

	QAction* id = m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_OPTIONS)),__tr2qs("&Configure KVIrc..."),this,SLOT(executeInternalCommand(bool)));
	id->setData(KVI_INTERNALCOMMAND_OPTIONS_DIALOG);

	id = m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_KVIRC)),__tr2qs("&About KVIrc"),this,SLOT(executeInternalCommand(bool)));
	id->setData(KVI_INTERNALCOMMAND_ABOUT_ABOUTKVIRC);

	m_pContextPopup->insertSeparator();
	m_pToggleFrame = m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_RAW)),__tr2qs("Hide/Show"),this,SLOT(toggleParentFrame()));

	m_pContextPopup->insertSeparator();

	id = m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TRAYICON)),__tr2qs("Un&dock"),this,SLOT(executeInternalCommand(bool)));
	id->setData(KVI_INTERNALCOMMAND_TRAYICON_HIDE);

	id = m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_QUITAPP)),__tr2qs("&Quit"),g_pFrame,SLOT(close()),QKeySequence(Qt::CTRL + Qt::Key_Q));

	connect(m_pContextPopup,SIGNAL(aboutToShow()),this,SLOT(fillContextPopup()));

	setIcon(*g_pDock1);

	connect(this,SIGNAL(activated ( QSystemTrayIcon::ActivationReason )),this,SLOT(activatedSlot ( QSystemTrayIcon::ActivationReason )));
}


KviTrayIcon::~KviTrayIcon()
{
	m_pFrm->setDockExtension(0);
	g_pTrayIconList->removeRef(this);
	delete m_pAwayPopup;
	delete m_pTitleLabel;
	delete m_pTip;
	delete m_pFlashingTimer;
	delete m_pContextPopup;
}


void KviTrayIcon::executeInternalCommand(bool)
{
	int iCmd;
	bool bOk;
	iCmd=(((QAction*)QObject::sender())->data()).toInt(&bOk);
	if(m_pFrm && bOk)
		m_pFrm->executeInternalCommand(iCmd);
}
void KviTrayIcon::die()
{
	delete this;
}

void KviTrayIcon::flashingTimerShot()
{
	m_bFlashed=!m_bFlashed;
	refresh();
}

#define NIDLEMSGS 18

static const char * idlemsgs[NIDLEMSGS]=
{
	__tr("Nothing is happening...") ,
	__tr("Just idling...") ,
	__tr("Dum de dum de dum...") ,
	__tr("Hey man... do something!") ,
	__tr("Umpf!") ,
	__tr("Silence speaking") ,
	__tr("Are ya here?") ,
	__tr("The world has stopped?") ,
	__tr("Everything is all right") ,
	__tr("idle()") ,
	__tr("It's so cold here...") ,
	__tr("Do not disturb... watching TV") ,
	__tr("Just vegetating") ,
	__tr("Hey... are ya sure that your network is up?") ,
	__tr("Seems like the world has stopped spinning") ,
	__tr("This silence is freaking me out!") ,
	__tr("Mieeeeeowww!") ,
	__tr("idle idle idle idle!")
};

bool KviTrayIcon::event(QEvent *e)
{
	if(e->type()==QEvent::ToolTip)
	{
		QPoint pos= m_pFrm->mapFromGlobal(QCursor::pos());
		QString tmp;

		KviWindowListBase * t = m_pFrm->windowListWidget();

		QString line;

		for(KviWindowListItem * b = t->firstItem();b;b = t->nextItem())
		{

			if(b->kviWindow()->view())
			{
				if(b->kviWindow()->view()->haveUnreadedMessages())
				{
					line = b->kviWindow()->lastMessageText();
					if(!line.isEmpty())
					{
						line.replace(QChar('&'),"&amp;");
						line.replace(QChar('<'),"&lt;");
						line.replace(QChar('>'),"&gt;");
						tmp += "<b>";
						tmp += b->kviWindow()->plainTextCaption();
						tmp += "</b><br>";
						tmp += line;
						tmp += "<br><br>\n";
					}
				}
			}
		}


		srand(time(0));

		// We use the bad way to generate random numbers :)))))

		if(tmp.isEmpty())tmp = __tr2qs_no_xgettext(idlemsgs[(int)(rand() % NIDLEMSGS)]);

		m_pTip->tip(QRect(pos,QSize(0,0)),tmp);
		return true;
	}
	return false;
}

//int KviTrayIcon::message(int,void *)
//{
//	debug("Message");
//	update();
//	return 0;
//}

void KviTrayIcon::doAway(bool)
{
	int id;
	bool ok;
	QAction * act = (QAction*)QObject::sender();

	if(act)
	{
		id = act->data().toInt(&ok);
		if(!ok)
			return;
	} else {
		return;
	}

	if(id<0)
	{
		KviPointerHashTableIterator<QString,KviWindow> it(*g_pGlobalWindowDict);
		while(KviWindow * wnd = it.current())
		{
			if(wnd->type()==KVI_WINDOW_TYPE_CONSOLE)
			{
				KviConsole* pConsole=(KviConsole*)wnd;
				if(pConsole->isConnected())
				{
					if(id==-2)
					{
						pConsole->connection()->sendFmtData("AWAY");
					} else {
						pConsole->connection()->sendFmtData("AWAY :%s",
							pConsole->connection()->encodeText(KVI_OPTION_STRING(KviOption_stringAwayMessage)).data()
							);
					}
				}
			}
 			++it;
		}
	} else {
		KviConsole* pConsole=g_pApp->findConsole((unsigned int)id);
		if(pConsole)
		{
			if(pConsole->isConnected())
			{
				if(pConsole->connection()->userInfo()->isAway())
				{
					pConsole->connection()->sendFmtData("AWAY");
				} else {
					pConsole->connection()->sendFmtData("AWAY :%s",
						pConsole->connection()->encodeText(KVI_OPTION_STRING(KviOption_stringAwayMessage)).data()
						);
				}
			}
		}
	}
}

void KviTrayIcon::fillContextPopup()
{
	m_pToggleFrame->setText(m_pFrm->isVisible() ? __tr2qs("Hide Window") : __tr2qs("Show Window"));
	if(g_pApp->topmostConnectedConsole())
	{
		m_pAwayMenuId->setVisible(true);
		m_pAwayPopup->clear();

		QAction* pAllAway=m_pAwayPopup->addAction(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CONSOLE)),__tr2qs("Away on all"),this,SLOT(doAway(bool)));
		pAllAway->setData(-1);

		QAction* pAllUnaway=m_pAwayPopup->addAction(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CONSOLE)),__tr2qs("Back on all"),this,SLOT(doAway(bool)));
		pAllUnaway->setData(-2);

		QAction* pSeparator=m_pAwayPopup->addSeparator();

		KviPointerHashTableIterator<QString,KviWindow> it(*g_pGlobalWindowDict);
		bool bAllAway=1;
		bool bAllUnaway=1;
		int iNetCount=0;
		while(KviWindow * wnd = it.current())
		{
			if(wnd->type()==KVI_WINDOW_TYPE_CONSOLE)
			{
				KviConsole* pConsole=(KviConsole*)wnd;
				if(pConsole->isConnected())
				{
					QAction* id;
					if(pConsole->connection()->userInfo()->isAway())
					{
						id=m_pAwayPopup->addAction(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CONSOLE)),__tr2qs("Back on %1").arg(pConsole->currentNetworkName()),this,SLOT(doAway(bool)));
						id->setData(pConsole->context()->id());
						bAllUnaway=0;
					} else {
						id=m_pAwayPopup->addAction(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CONSOLE)),__tr2qs("Away on %1").arg(pConsole->currentNetworkName()),this,SLOT(doAway(bool)));
						id->setData(pConsole->context()->id());
						bAllAway=0;
					}
					id->setData(pConsole->context()->id());
					iNetCount++;
				}
			}
 			++it;
		}
		if(iNetCount==1)
		{
			pAllAway->setVisible(false);
			pAllUnaway->setVisible(false);
			pSeparator->setVisible(false);
		} else {
			pAllAway->setVisible(!bAllAway);
			pAllUnaway->setVisible(!bAllUnaway);
		}
	} else {
		m_pAwayMenuId->setVisible(false);
	}
}

void KviTrayIcon::toggleParentFrame()
{
	if(m_pFrm->isMinimized())
	{
		m_pFrm->setWindowState(m_pFrm->windowState() & (~Qt::WindowMinimized | Qt::WindowActive));

		if(KVI_OPTION_BOOL(KviOption_boolFrameIsMaximized))
			m_pFrm->showMaximized();
		else m_pFrm->show();
	} else if(!m_pFrm->isVisible())
	{
		//restore mainwindow
		if(KVI_OPTION_BOOL(KviOption_boolFrameIsMaximized))
			m_pFrm->showMaximized();
		else m_pFrm->show();
	} else {
		KVI_OPTION_BOOL(KviOption_boolFrameIsMaximized) = m_pFrm->isMaximized();
		m_pFrm->hide();
	}
}

void KviTrayIcon::refresh()
{
	grabActivityInfo();

	if( (m_iChannels == 2) || (m_iQueries == 2) )
	{
		if(!m_pFlashingTimer->isActive() && KVI_OPTION_BOOL(KviOption_boolEnableTrayIconFlashing) )
			m_pFlashingTimer->start(1000);
	} else {
		if(m_pFlashingTimer->isActive()) m_pFlashingTimer->stop();
		m_bFlashed=false;
	}

	m_CurrentPixmap.fill(Qt::transparent);
	QPainter thisRestrictionOfQt4IsNotNice(&m_CurrentPixmap);
	//thisRestrictionOfQt4IsNotNice.drawPixmap(0,0,22,22,*g_pDock1,0,0,22,22);

	if(m_bFlashed)
	{
		thisRestrictionOfQt4IsNotNice.drawPixmap((ICON_SIZE-16)/2,(ICON_SIZE-16)/2,16,16,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MESSAGE)),0,0,16,16);
	} else {
		thisRestrictionOfQt4IsNotNice.drawPixmap(0,0,ICON_SIZE/2,ICON_SIZE/2,
			m_iOther ?
				((m_iOther == 2) ? *g_pDock3 : *g_pDock2)
				: *g_pDock1,0,0,ICON_SIZE/2,ICON_SIZE/2);

		thisRestrictionOfQt4IsNotNice.drawPixmap(0,ICON_SIZE/2,ICON_SIZE/2,ICON_SIZE/2,
			m_iConsoles ?
				((m_iConsoles == 2) ? *g_pDock3 : *g_pDock2)
				: *g_pDock1,0,ICON_SIZE/2,ICON_SIZE/2,ICON_SIZE/2);

		thisRestrictionOfQt4IsNotNice.drawPixmap(ICON_SIZE/2,0,ICON_SIZE/2,ICON_SIZE/2,
			m_iQueries ?
			((m_iQueries == 2)
			? *g_pDock3 : *g_pDock2)
			: *g_pDock1,ICON_SIZE/2,0,ICON_SIZE/2,ICON_SIZE/2);

		thisRestrictionOfQt4IsNotNice.drawPixmap(ICON_SIZE/2,ICON_SIZE/2,ICON_SIZE/2,ICON_SIZE/2,
			m_iChannels ?
			((m_iChannels == 2) ? *g_pDock3 : *g_pDock2)
			: *g_pDock1
			,ICON_SIZE/2,ICON_SIZE/2,ICON_SIZE/2,ICON_SIZE/2);

	}
	updateIcon();
}

void KviTrayIcon::activatedSlot( QSystemTrayIcon::ActivationReason reason )
{
	if(reason==QSystemTrayIcon::Trigger && (KVI_OPTION_BOOL(KviOption_boolCloseInTray) || !m_pFrm->isVisible()))
	{
		toggleParentFrame();
	}
}

void KviTrayIcon::grabActivityInfo()
{
	KviWindowListBase * t = m_pFrm->windowListWidget();

	if(KVI_OPTION_BOOL(KviOption_boolUseLevelBasedTrayNotification))
	{
		if(KVI_OPTION_UINT(KviOption_uintMinTrayLowLevelMessage)>5) KVI_OPTION_UINT(KviOption_uintMinTrayLowLevelMessage)=5;
		if(KVI_OPTION_UINT(KviOption_uintMinTrayHighLevelMessage)>5) KVI_OPTION_UINT(KviOption_uintMinTrayHighLevelMessage)=5;

		if(KVI_OPTION_UINT(KviOption_uintMinTrayLowLevelMessage)<1) KVI_OPTION_UINT(KviOption_uintMinTrayLowLevelMessage)=1;
		if(KVI_OPTION_UINT(KviOption_uintMinTrayHighLevelMessage)<1) KVI_OPTION_UINT(KviOption_uintMinTrayHighLevelMessage)=1;

		if(KVI_OPTION_UINT(KviOption_uintMinTrayHighLevelMessage)<KVI_OPTION_UINT(KviOption_uintMinTrayLowLevelMessage))
			KVI_OPTION_UINT(KviOption_uintMinTrayHighLevelMessage)=KVI_OPTION_UINT(KviOption_uintMinTrayLowLevelMessage);
	}

	m_iConsoles = 0;
	m_iChannels = 0;
	m_iQueries  = 0;
	m_iOther    = 0;

	for(KviWindowListItem * b = t->firstItem();b;b = t->nextItem())
	{
		if(KVI_OPTION_BOOL(KviOption_boolUseLevelBasedTrayNotification))
		{
			unsigned int iLevel = b->highlightLevel();
			switch(b->kviWindow()->type())
			{
				case KVI_WINDOW_TYPE_CONSOLE:
					if(m_iConsoles < iLevel) m_iConsoles = iLevel;
				break;
				case KVI_WINDOW_TYPE_CHANNEL:
					if(m_iChannels < iLevel) m_iChannels = iLevel;
				break;
				case KVI_WINDOW_TYPE_QUERY:
					if(m_iQueries < iLevel) m_iQueries = iLevel;
				break;
				default:
					if(m_iOther < iLevel) m_iOther = iLevel;
				break;
			}
		} else {
			if(b->kviWindow()->view())
			{
				unsigned int iLevel=0;
				if(b->kviWindow()->view()->haveUnreadedHighlightedMessages())
				{
					iLevel=2;
				} else if(b->kviWindow()->view()->haveUnreadedMessages())
				{
					iLevel=1;
				}
				if(iLevel>0)
				switch(b->kviWindow()->type())
				{
					case KVI_WINDOW_TYPE_CONSOLE:
						if(m_iConsoles < iLevel) m_iConsoles = iLevel;
					break;
					case KVI_WINDOW_TYPE_CHANNEL:
						if(m_iChannels < iLevel) m_iChannels = iLevel;
					break;
					case KVI_WINDOW_TYPE_QUERY:
						if(m_iQueries < iLevel) m_iQueries = iLevel;
					break;
					default:
						if(m_iOther < iLevel) m_iOther = iLevel;
					break;
				}
			}
		}
	}

	if(KVI_OPTION_BOOL(KviOption_boolUseLevelBasedTrayNotification))
	{
		if(m_iConsoles >= KVI_OPTION_UINT(KviOption_uintMinTrayHighLevelMessage)) m_iConsoles=2;
		else if(m_iConsoles >= KVI_OPTION_UINT(KviOption_uintMinTrayLowLevelMessage)) m_iConsoles=1;
		else m_iConsoles=0;

		if(m_iChannels >= KVI_OPTION_UINT(KviOption_uintMinTrayHighLevelMessage)) m_iChannels=2;
		else if(m_iChannels >= KVI_OPTION_UINT(KviOption_uintMinTrayLowLevelMessage)) m_iChannels=1;
		else m_iChannels=0;

		if(m_iQueries >= KVI_OPTION_UINT(KviOption_uintMinTrayHighLevelMessage)) m_iQueries=2;
		else if(m_iQueries >= KVI_OPTION_UINT(KviOption_uintMinTrayLowLevelMessage)) m_iQueries=1;
		else m_iQueries=0;

		if(m_iOther >= KVI_OPTION_UINT(KviOption_uintMinTrayHighLevelMessage)) m_iOther=2;
		else if(m_iOther >= KVI_OPTION_UINT(KviOption_uintMinTrayLowLevelMessage)) m_iOther=1;
		else m_iOther=0;
	}
}

void KviTrayIcon::updateIcon()
{
	setIcon(QIcon(m_CurrentPixmap));
}

static KviTrayIcon * trayicon_find(KviFrame *f)
{
	if(!g_pTrayIconList)return 0;
	for(KviTrayIcon * w = g_pTrayIconList->first();w;w = g_pTrayIconList->next())
	{
		if(w->frame() == f)return w;
	}
	return 0;
}

/*
	@doc: trayicon.show
	@type:
		command
	@title:
		trayicon.show
	@short:
		Shows the dock widget for the current frame window
	@keyterms:
		dock widget , system tray
	@syntax:
		trayicon.show
	@description:
		Shows the dock widget for the current frame window.[br]
		The dock widget is a small widget that docks in the KDE panel.[br]
		It shows a small icon of the earth and eventually displays four squares
		that cover this icon: the bottom left square appears when there is some new
		text in any console window, the square becomes red if the text is highlighted.[br]
		The bottom right square appears when there is some new text in any channel window,
		and it becomes red when the text is highlighted.[br] The upper right square refers to
		query windows and the upper left one to any other kind of window (dcc , links...).[br]
		If you move the mouse over the dock widget a tooltip will show you the last lines
		of the "new" text in all these windows.[br]
		This is useful when you keep the main KVIrc window minimized and you're working on something else:
		if the dock widget shows nothing but the earth icon , nothing is happening in the main KVIrc window.
		If the dock widget shows one or more white (or red) squares , you can move the mouse over
		and check what's happened exactly and eventually bring up the main KVIrc window by clicking on the widget.[br]
		[big]tecnical details[/big]
		The dock widget is currently working in KDE compilation mode only:
		it relies on the KWin implementation of the Window Manager interaction protocol.
	@seealso:
		[cmd]trayicon.hide[/cmd]
*/

static bool trayicon_kvs_cmd_show(KviKvsModuleCommandCall * c)
{
	if(!(trayicon_find(c->window()->frame())))
	{
		KviTrayIcon * w = new KviTrayIcon(c->window()->frame());
		w->show();
	}
	return true;
}

/*
	@doc: trayicon.hide
	@type:
		command
	@title:
		trayicon.hide
	@short:
		Hides the dock widget for the current frame window
	@syntax:
		trayicon.hide
	@description:
		Hides the dock widget for the current frame window
	@seealso:
		[cmd]trayicon.show[/cmd]
*/

static bool trayicon_kvs_cmd_hide(KviKvsModuleCommandCall * c)
{
	KviTrayIcon * w= trayicon_find(c->window()->frame());
	if(w)delete w;
	// show the parent frame.. otherwise there will be no way to get it back
	if(!c->window()->frame()->isVisible())
	{
		c->window()->frame()->show();
	}
	return true;
}

/*
	@doc: trayicon.hidewindow
	@type:
		command
	@title:
		trayicon.hidewindow
	@short:
		Hides the window, associated with trayicon
	@syntax:
		trayicon.hidewindow
	@description
		Hides the window, associated with trayicon
	@seealso:
		[cmd]trayicon.show[/cmd], [cmd]trayicon.hide[/cmd]
*/

static bool trayicon_kvs_cmd_hidewindow(KviKvsModuleCommandCall * c)
{
	KviTrayIcon * w= trayicon_find(c->window()->frame());
	if(w)
	{
		c->window()->frame()->hide();
	}
	return true;
}

/*
	@doc: trayicon.isVisible
	@type:
		function
	@title:
		$trayicon.isVisible
	@short:
		Returns the state of the dock widget
	@syntax:
		$reguser.isVisible()
	@description:
		Returns 1 if the dock widget is actually visible , 0 otherwise.
	@seealso:
		[cmd]trayicon.show[/cmd]
*/

static bool trayicon_kvs_fnc_isvisible(KviKvsModuleFunctionCall * c)
{
	c->returnValue()->setBoolean(trayicon_find(c->window()->frame()));
	return true;
}

// =======================================
// init routine
// =======================================
static bool trayicon_module_init(KviModule * m)
{
	QString buffer;
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	g_pApp->findImage(buffer,"kvi_dock_win32-0.png");
#else
	g_pApp->findImage(buffer,"kvi_dock_part-0.png");
#endif
	g_pDock1 = new QPixmap(buffer);

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	g_pApp->findImage(buffer,"kvi_dock_win32-1.png");
#else
	g_pApp->findImage(buffer,"kvi_dock_part-1.png");
#endif
	g_pDock2 = new QPixmap(buffer);

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	g_pApp->findImage(buffer,"kvi_dock_win32-2.png");
#else
	g_pApp->findImage(buffer,"kvi_dock_part-2.png");
#endif
	g_pDock3 = new QPixmap(buffer);

	g_pTrayIconList = new KviPointerList<KviTrayIcon>;
	g_pTrayIconList->setAutoDelete(false);


	KVSM_REGISTER_SIMPLE_COMMAND(m,"hide",trayicon_kvs_cmd_hide);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"hidewindow",trayicon_kvs_cmd_hidewindow);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"show",trayicon_kvs_cmd_show);
	KVSM_REGISTER_FUNCTION(m,"isVisible",trayicon_kvs_fnc_isvisible);

	return true;
}

static bool trayicon_module_cleanup(KviModule *)
{
	while(g_pTrayIconList->first())delete g_pTrayIconList->first();
	delete g_pTrayIconList;
	g_pTrayIconList = 0;

	delete g_pDock1;
	g_pDock1 = 0;

	delete g_pDock2;
	g_pDock2 = 0;

	delete g_pDock3;
	g_pDock3 = 0;

	return true;
}

static bool trayicon_module_can_unload(KviModule *)
{
	return g_pTrayIconList->isEmpty();
}

// =======================================
// plugin definition structure
// =======================================
KVIRC_MODULE(
	"KVIrc Tray Icon Implementation",
	"4.0.0",
	"Copyright (C) 2001 Szymon Stefanek <pragma at kvirc dot net>" \
	"Copyright (C) 2007 Alexey Uzhva <alexey at kvirc dot ru>" \
	"Copyright (C) 2008 Elvio Basello <hellvis69 at netsons dot org>",
	"Exports the /trayicon.* interface\n",
	trayicon_module_init ,
	trayicon_module_can_unload,
	0,
	trayicon_module_cleanup,
	0
)

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "libkvitrayicon.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

