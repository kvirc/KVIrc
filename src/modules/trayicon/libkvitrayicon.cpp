//
//   File : libKviTrayIcon.cpp
//   Creation date : Tue Jan 02 20001 14:34:12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2001 Szymon Stefanek (pragma at kvirc dot net)
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

#include "libkvitrayicon.h"

#include "kvi_settings.h"
#include "kvi_app.h"
#include "kvi_module.h"
#include "kvi_locale.h"
#include "kvi_memmove.h"
#include "kvi_taskbar.h"
#include "kvi_window.h"
#include "kvi_dynamictooltip.h"
#include "kvi_iconmanager.h"
#include "kvi_internalcmd.h"
#include "kvi_console.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnectionuserinfo.h"
#include "kvi_options.h"
#include "kvi_ircview.h"
#include "kvi_doublebuffer.h"
#include "kvi_tal_popupmenu.h"

#include <QLabel>
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

	m_pFlashingTimer = new QTimer(this,"flashing_timer");
	connect( m_pFlashingTimer, SIGNAL(timeout()), this, SLOT(flashingTimerShot()) );
	m_bFlashed=0;
	g_pTrayIconList->append(this);
	m_pFrm = frm;
	m_pFrm->setDockExtension(this);

	m_pTip = new KviDynamicToolTip(frm,"dock_tooltip");
	m_pAwayPopup = new KviTalPopupMenu(0);

	QLabel * l = new QLabel(__tr2qs("<b>KVIrc</b>"),m_pContextPopup);
	QPalette p;
	l->setStyleSheet("background-color: " + p.color(QPalette::Normal, QPalette::Mid).name());
	m_pContextPopup->insertItem(l);
	m_pContextPopup->setWindowTitle(__tr2qs("Context"));
	m_iAwayMenuId = m_pContextPopup->insertItem ( __tr2qs("Away"), m_pAwayPopup);
	m_pContextPopup->changeItem(m_iAwayMenuId,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_AWAY)),__tr2qs("Away"));

	int id = m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_OPTIONS)),__tr2qs("&Configure KVIrc..."),m_pFrm,SLOT(executeInternalCommand(int)));
	m_pContextPopup->setItemParameter(id,KVI_INTERNALCOMMAND_OPTIONS_DIALOG);
	id = m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_KVIRC)),__tr2qs("&About KVIrc"),m_pFrm,SLOT(executeInternalCommand(int)));
	m_pContextPopup->setItemParameter(id,KVI_INTERNALCOMMAND_ABOUT_ABOUTKVIRC);
	m_pContextPopup->insertSeparator();
	m_iToggleFrame = m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_RAW)),__tr2qs("Hide/Show"),this,SLOT(toggleParentFrame()));
	m_pContextPopup->insertSeparator();
	id = m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TRAYICON)),__tr2qs("Un&dock"),m_pFrm,SLOT(executeInternalCommand(int)));
	m_pContextPopup->setItemParameter(id,KVI_INTERNALCOMMAND_TRAYICON_HIDE);
	id = m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_QUITAPP)),__tr2qs("&Quit"),g_pApp,SLOT(quit()));
	m_pContextPopup->setAccel(__tr2qs("Ctrl+Q"),id);
	connect(m_pContextPopup,SIGNAL(aboutToShow()),this,SLOT(fillContextPopup()));

	QIcon icon(*g_pDock1);
	setIcon(icon);

	connect(this,SIGNAL(activated ( QSystemTrayIcon::ActivationReason )),this,SLOT(activatedSlot ( QSystemTrayIcon::ActivationReason )));
}


KviTrayIcon::~KviTrayIcon()
{
	m_pFrm->setDockExtension(0);
	g_pTrayIconList->removeRef(this);
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

void KviTrayIcon::tipRequest(KviDynamicToolTip *tip,const QPoint &pnt)
{
	QString tmp;

	KviTaskBarBase * t = m_pFrm->taskBar();

	QString line;

	for(KviTaskBarItem * b = t->firstItem();b;b = t->nextItem())
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

	//m_pTip->tip(rect(),tmp);
}

//int KviTrayIcon::message(int,void *)
//{
//	debug("Message");
//	update();
//	return 0;
//}

//FIXME: Qt4 port
/*void KviTrayIcon::mousePressEvent(QMouseEvent *e)
{
	if(e->button() & Qt::LeftButton)toggleParentFrame();
	else if(e->button() & Qt::RightButton)
		m_pContextPopup->popup(mapToGlobal(e->pos()));
}*/

void KviTrayIcon::doAway(int id)
{
	if(id<0)
	{
		KviPointerHashTableIterator<QString,KviWindow> it(*g_pGlobalWindowDict);
		while(KviWindow * wnd = it.current())
		{
			if(wnd->type()==KVI_WINDOW_TYPE_CONSOLE)
			{
				KviConsole* pConsole=(KviConsole*)wnd;
				if(pConsole->isConnected())
					if(id==-2)
						pConsole->connection()->sendFmtData("AWAY");
					else
						pConsole->connection()->sendFmtData("AWAY :%s",
							pConsole->connection()->encodeText(KVI_OPTION_STRING(KviOption_stringAwayMessage)).data()
							);
			}
 			++it;
		}
	} else {
		KviConsole* pConsole=g_pApp->findConsole((unsigned int)id);
		if(pConsole)
			if(pConsole->isConnected())
				if(pConsole->connection()->userInfo()->isAway())
					pConsole->connection()->sendFmtData("AWAY");
				else
					pConsole->connection()->sendFmtData("AWAY :%s",
						pConsole->connection()->encodeText(KVI_OPTION_STRING(KviOption_stringAwayMessage)).data()
						);
	}
}

void KviTrayIcon::fillContextPopup()
{
	m_pContextPopup->changeItem(m_iToggleFrame,m_pFrm->isVisible() ? __tr2qs("Hide Window") : __tr2qs("Show Window"));
	if(g_pApp->topmostConnectedConsole())
	{
		m_pContextPopup->setItemVisible(m_iAwayMenuId,true);
		m_pAwayPopup->clear();

		int iAllAway=m_pAwayPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CONSOLE)),__tr2qs("Away on all"),this,SLOT(doAway(int)));
		m_pAwayPopup->setItemParameter(iAllAway,-1);

		int iAllUnaway=m_pAwayPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CONSOLE)),__tr2qs("Back on all"),this,SLOT(doAway(int)));
		m_pAwayPopup->setItemParameter(iAllUnaway,-2);

		int iSeparator=m_pAwayPopup->insertSeparator();

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
					int id;
					if(pConsole->connection()->userInfo()->isAway())
					{
						id=m_pAwayPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CONSOLE)),__tr2qs("Back on %1").arg(pConsole->currentNetworkName()),this,SLOT(doAway(int)));
						bAllUnaway=0;
					} else {
						id=m_pAwayPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CONSOLE)),__tr2qs("Away on %1").arg(pConsole->currentNetworkName()),this,SLOT(doAway(int)));
						bAllAway=0;
					}
					m_pAwayPopup->setItemParameter(id,pConsole->context()->id());
					iNetCount++;
				}
			}
 			++it;
		}
		if(iNetCount==1)
		{
			m_pAwayPopup->setItemVisible(iAllAway,false);
			m_pAwayPopup->setItemVisible(iAllUnaway,false);
			m_pAwayPopup->setItemVisible(iSeparator,false);
		} else {
			m_pAwayPopup->setItemVisible(iAllAway,!bAllAway);
			m_pAwayPopup->setItemVisible(iAllUnaway,!bAllUnaway);
		}
	} else {
		m_pContextPopup->setItemVisible(m_iAwayMenuId,false);
	}
}

void KviTrayIcon::toggleParentFrame()
{
	QWidget *top_widget = m_pFrm->topLevelWidget();

	if(m_pFrm->isVisible()) {
		m_pFrm->hide();
	} else {
		top_widget->show();
		top_widget->raise();
		top_widget->setActiveWindow();
		if(g_pActiveWindow) g_pActiveWindow->setFocus();
		if(m_pFrm->isMinimized())
			m_pFrm->showNormal();
		else
			m_pFrm->show();
		m_pFrm->setWindowState(getPrevWindowState() & Qt::WindowMinimized);
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
	if(reason==QSystemTrayIcon::Trigger)
	{
		toggleParentFrame();
	}
}

void KviTrayIcon::grabActivityInfo()
{
	KviTaskBarBase * t = m_pFrm->taskBar();

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

	for(KviTaskBarItem * b = t->firstItem();b;b = t->nextItem())
	{
		if(KVI_OPTION_BOOL(KviOption_boolUseLevelBasedTrayNotification))
		{
			int iLevel = b->highlightLevel();
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
			int iLevel=0;
			if(b->kviWindow()->view())
			{
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
		Returns the state of the dock widget
	@syntax:
		trayicon.hidewindow
	@description:
		Hides the window, assotiated with trayicon
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

static bool trayicon_module_cleanup(KviModule *m)
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
	"Szymon Stefanek <pragma at kvirc dot net> and Alexey Uzhva <alexey at kvirc dot ru>",
	"exports the /trayicon.* interface\n",
	trayicon_module_init ,
	trayicon_module_can_unload,
	0,
	trayicon_module_cleanup
)

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "libkvitrayicon.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

