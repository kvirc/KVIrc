//
//   File : libkvidockwidget.cpp
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

#include "kvi_settings.h"
#ifndef COMPILE_USE_QT4

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
#include "kvi_imagelib.h"
#include "kvi_options.h"
#include "kvi_ircview.h"

#include "libkvidockwidget.h"
#include "kvi_doublebuffer.h"

#ifdef COMPILE_KDE_SUPPORT
	#include <kwin.h>
	#include <kpopupmenu.h>
#else
	#include <qlabel.h>
#endif

#include "kvi_tal_popupmenu.h"


#include <qpixmap.h>
#include <qpainter.h>
#include <qtimer.h>
#include <qevent.h>
#include <qregexp.h>

#include <stdlib.h>
#include <time.h>

#ifdef COMPILE_ON_WINDOWS
	#include <windows.h>

	#include <qbitmap.h>
	#include <qcursor.h>

	#define ID_DOCKWIDGET_TASKBAR_ICON 0xdeadbeef
	#define WM_KVIRC_NOTIFY_ICON_MESSAGE (WM_USER + 0xbeef)

	static UINT WM_KVIRC_TASKBAR_CREATED = 0;
#else
#ifndef Q_OS_MACX
	#include <X11/Xlib.h>

	//const int XFocusOut = FocusOut;
	const int XFocusIn = FocusIn;
	#undef FocusOut
	#undef FocusIn
	#undef KeyPress
	#undef KeyRelease

	//#warning "Later remove this stuff and use a wrapper for #include <X11/Xlib.h>"
	#ifdef Bool
		#undef Bool
	#endif
#endif
#endif

extern KVIRC_API KviAsciiDict<KviWindow> * g_pGlobalWindowDict;
static KviPtrList<KviDockWidget> * g_pDockWidgetList = 0;

static QPixmap * g_pDock1 = 0;
static QPixmap * g_pDock2 = 0;
static QPixmap * g_pDock3 = 0;

#ifdef COMPILE_ON_WINDOWS
	static HICON g_pCurrentIcon = 0;
	static QPixmap * g_pCurrentPixmap = 0;
#endif


KviDockWidget::KviDockWidget(KviFrame * frm,const char * name)
: QWidget(0,name)
{
	m_iConsoles = 0;
	m_iChannels = 0;
	m_iQueries  = 0;
	m_iOther    = 0;
	
	m_pFlashingTimer = new QTimer(this,"flashing_timer");
	connect( m_pFlashingTimer, SIGNAL(timeout()), this, SLOT(flashingTimerShot()) );
	m_bFlashed=0;
	g_pDockWidgetList->append(this);
	m_pFrm = frm;
	m_pFrm->setDockExtension(this);

#ifdef COMPILE_ON_WINDOWS
	// kode54
	if(!WM_KVIRC_TASKBAR_CREATED)WM_KVIRC_TASKBAR_CREATED = RegisterWindowMessage(TEXT("TaskbarCreated"));
	createTaskbarIcon();
#else //!COMPILE_ON_WINDOWS
	setMinimumSize(22,22);
	#ifndef COMPILE_USE_QT4
		setBackgroundMode(X11ParentRelative);
	#endif
	#ifdef COMPILE_KDE_SUPPORT
		KWin::setSystemTrayWindowFor(winId(),frm->winId());
	#endif
#endif //!COMPILE_ON_WINDOWS

#ifdef COMPILE_USE_QT4
	m_pSysTrayIcon = new QSystemTrayIcon(m_pFrm);
#endif

	m_pTip = new KviDynamicToolTip(this,"dock_tooltip");
	connect(m_pTip,SIGNAL(tipRequest(KviDynamicToolTip *,const QPoint &)),this,SLOT(tipRequest(KviDynamicToolTip *,const QPoint &)));
	
	m_pAwayPopup = new KviTalPopupMenu(this);
	
#ifdef COMPILE_KDE_SUPPORT
	m_pContextPopup = new KPopupMenu(this);
	m_pContextPopup->insertTitle(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_KVIRC)),__tr2qs("KVIrc"));
#else
	m_pContextPopup = new KviTalPopupMenu(this);
#ifndef COMPILE_USE_QT4
	QLabel * l = new QLabel(__tr2qs("KVIrc"),m_pContextPopup);
	l->setFrameStyle(QFrame::Raised | QFrame::StyledPanel);
	m_pContextPopup->insertItem(l);
#endif
#endif
	m_pContextPopup->setCaption(__tr2qs("Context"));
	m_iAwayMenuId = m_pContextPopup->insertItem ( __tr2qs("Away"), m_pAwayPopup);
	m_pContextPopup->changeItem(m_iAwayMenuId,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_AWAY)),__tr2qs("Away"));
	
	int id = m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_OPTIONS)),__tr2qs("&Configure KVIrc..."),m_pFrm,SLOT(executeInternalCommand(int)));
	m_pContextPopup->setItemParameter(id,KVI_INTERNALCOMMAND_OPTIONS_DIALOG);
	id = m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_KVIRC)),__tr2qs("&About KVIrc"),m_pFrm,SLOT(executeInternalCommand(int)));
	m_pContextPopup->setItemParameter(id,KVI_INTERNALCOMMAND_ABOUT_ABOUTKVIRC);
	m_pContextPopup->insertSeparator();
	m_iToggleFrame = m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_RAW)),QString(""),this,SLOT(toggleParentFrame()));
	m_pContextPopup->insertSeparator();
	id = m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DOCKWIDGET)),__tr2qs("Un&dock"),m_pFrm,SLOT(executeInternalCommand(int)));
	m_pContextPopup->setItemParameter(id,KVI_INTERNALCOMMAND_DOCKWIDGET_HIDE);
	id = m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_QUITAPP)),__tr2qs("&Quit"),g_pApp,SLOT(quit()));
	m_pContextPopup->setAccel(__tr2qs("Ctrl+Q"),id);
	connect(m_pContextPopup,SIGNAL(aboutToShow()),this,SLOT(fillContextPopup()));
}


KviDockWidget::~KviDockWidget()
{
#ifdef COMPILE_ON_WINDOWS
	destroyTaskbarIcon();
#endif
	m_pFrm->setDockExtension(0);
	g_pDockWidgetList->removeRef(this);
}

void KviDockWidget::die()
{
	delete this;
}
void KviDockWidget::flashingTimerShot()
{
	m_bFlashed=!m_bFlashed;
#ifdef COMPILE_ON_WINDOWS
	updateTaskbarIcon();
#else
	update();
#endif
}

#ifdef COMPILE_ON_WINDOWS
void KviDockWidget::createTaskbarIcon()
{
	ICONINFO inf;
	g_pCurrentPixmap=new QPixmap(*g_pDock1);
	inf.hbmColor = g_pCurrentPixmap->hbm();
	if(!g_pCurrentPixmap->mask())g_pCurrentPixmap->setMask(g_pCurrentPixmap->createHeuristicMask());
	inf.hbmMask = g_pCurrentPixmap->mask()->hbm();
	g_pCurrentIcon=CreateIconIndirect(&inf);

	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(nid);
	nid.hWnd = winId();
	nid.uID = ID_DOCKWIDGET_TASKBAR_ICON;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_KVIRC_NOTIFY_ICON_MESSAGE;
	kvi_memmove(nid.szTip,"KVIrc",6);
	nid.hIcon  = g_pCurrentIcon;
	Shell_NotifyIcon(NIM_ADD,&nid);
}

void KviDockWidget::destroyTaskbarIcon()
{
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(nid);
	nid.hWnd = winId();
	nid.uID = ID_DOCKWIDGET_TASKBAR_ICON;
	nid.uFlags = 0;
	Shell_NotifyIcon(NIM_DELETE,&nid);
}

void KviDockWidget::updateTaskbarIcon()
{
	ICONINFO inf;
	QPixmap* pix = m_bFlashed ? g_pIconManager->getSmallIcon(KVI_SMALLICON_MESSAGE) : g_pCurrentPixmap;
	inf.hbmColor = pix->hbm();
	if(!pix->mask())pix->setMask(pix->createHeuristicMask());
	inf.hbmMask = pix->mask()->hbm();
	HICON hIcon=CreateIconIndirect(&inf);

	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(nid);
	nid.hWnd = winId();
	nid.uID = ID_DOCKWIDGET_TASKBAR_ICON;
	nid.uFlags = NIF_ICON;
	nid.hIcon = hIcon;
	Shell_NotifyIcon(NIM_MODIFY,&nid);
	DestroyIcon(g_pCurrentIcon);
	g_pCurrentIcon=hIcon;
}

bool KviDockWidget::winEvent(MSG * m)
{
	if(m->message == WM_KVIRC_NOTIFY_ICON_MESSAGE)
	{
		if(m->wParam == ID_DOCKWIDGET_TASKBAR_ICON)
		{
			switch(m->lParam)
			{
				case WM_MOUSEMOVE:
					tipRequest(m_pTip,QCursor::pos());
					break;
				case WM_LBUTTONDOWN:
					toggleParentFrame();
					break;
				case WM_RBUTTONDOWN:
					m_pContextPopup->popup(QCursor::pos());
					break;
			}
			return true;
		}
	} else if(m->message == WM_KVIRC_TASKBAR_CREATED)
	{
		// kode54 : when the window shell dies and is restarted
		// it sends the "TaskbarCreated" message to all the toplevel windows
		// We recreate the taskbar icon then
		createTaskbarIcon();
		refresh();
	}
	return false;
}

#endif

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

#ifdef COMPILE_KDE_SUPPORT
	extern Time qt_x_time;
#endif

void KviDockWidget::enterEvent( QEvent* )
{
#ifdef COMPILE_KDE_SUPPORT
	if(!g_pApp->focusWidget())
	{
		XEvent ev;
		kvi_memset(&ev, 0, sizeof(ev));
		ev.xfocus.display = qt_xdisplay();
		ev.xfocus.type = XFocusIn;
		ev.xfocus.window = winId();
		ev.xfocus.mode = NotifyNormal;
		ev.xfocus.detail = NotifyAncestor;
		Time time = qt_x_time;
		qt_x_time = 1;
		g_pApp->x11ProcessEvent( &ev );
		qt_x_time = time;
    }
#endif
}

void KviDockWidget::tipRequest(KviDynamicToolTip *tip,const QPoint &pnt)
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

#ifdef COMPILE_ON_WINDOWS

#else
	m_pTip->tip(rect(),tmp);
#endif
}

//int KviDockWidget::message(int,void *)
//{
//	debug("Message");
//	update();
//	return 0;
//}

void KviDockWidget::mousePressEvent(QMouseEvent *e)
{
	if(e->button() & Qt::LeftButton)toggleParentFrame();
	else if(e->button() & Qt::RightButton)
		m_pContextPopup->popup(mapToGlobal(e->pos()));
}

void KviDockWidget::doAway(int id)
{
	if(id<0)
	{
		KviAsciiDictIterator<KviWindow> it(*g_pGlobalWindowDict);
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

void KviDockWidget::fillContextPopup()
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
		
		KviAsciiDictIterator<KviWindow> it(*g_pGlobalWindowDict);
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
					m_pAwayPopup->setItemParameter(id,pConsole->ircContextId());
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

void KviDockWidget::toggleParentFrame()
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
	}
}

void KviDockWidget::refresh()
{
	grabActivityInfo();

#ifdef COMPILE_ON_WINDOWS
	// how to copy transparency????
	delete g_pCurrentPixmap;
	g_pCurrentPixmap=new QPixmap(16,16);
	bitBlt(g_pCurrentPixmap,0,0,m_iOther ? ((m_iOther == 2) ? g_pDock3 : g_pDock2) : g_pDock1,0,0,8,8,Qt::ClearROP);
	bitBlt(g_pCurrentPixmap,0,8,m_iConsoles ? ((m_iConsoles == 2) ? g_pDock3 : g_pDock2) : g_pDock1,0,8,8,8,Qt::ClearROP);
	bitBlt(g_pCurrentPixmap,8,0,m_iQueries ? ((m_iQueries == 2) ? g_pDock3 : g_pDock2) : g_pDock1,8,0,8,8,Qt::ClearROP);
	bitBlt(g_pCurrentPixmap,8,8,m_iChannels ? ((m_iChannels == 2) ? g_pDock3 : g_pDock2) : g_pDock1,8,8,8,8,Qt::ClearROP);
#endif	
	
	if( (m_iChannels == 2) || (m_iQueries == 2) ) 
	{
		if(!m_pFlashingTimer->isActive() && KVI_OPTION_BOOL(KviOption_boolEnableTrayIconFlashing) ) m_pFlashingTimer->start(1000);
	} else {
		if(m_pFlashingTimer->isActive()) m_pFlashingTimer->stop();
		m_bFlashed=false;
	}
	
#ifdef COMPILE_ON_WINDOWS
	updateTaskbarIcon();
#else
	update();
#endif
}

void KviDockWidget::grabActivityInfo()
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

void KviDockWidget::paintEvent(QPaintEvent * event)
{
#ifdef COMPILE_USE_QT4
	QPainter thisRestrictionOfQt4IsNotNice(this);
	if(m_bFlashed)
	{
		erase();
		thisRestrictionOfQt4IsNotNice.drawPixmap(4,4,16,16,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MESSAGE)),0,0,16,16);
	} else {
		thisRestrictionOfQt4IsNotNice.drawPixmap(0,0,12,12,m_iOther ? ((m_iOther == 2) ? *g_pDock3 : *g_pDock2) : *g_pDock1,0,0,12,12);
		thisRestrictionOfQt4IsNotNice.drawPixmap(0,12,12,12,m_iConsoles ? ((m_iConsoles == 2) ? *g_pDock3 : *g_pDock2) : *g_pDock1,0,12,12,12);
		thisRestrictionOfQt4IsNotNice.drawPixmap(12,0,12,12,m_iQueries ? ((m_iQueries == 2) ? *g_pDock3 : *g_pDock2) : *g_pDock1,12,0,12,12);
		thisRestrictionOfQt4IsNotNice.drawPixmap(12,12,12,12,m_iChannels ? ((m_iChannels == 2) ? *g_pDock3 : *g_pDock2) : *g_pDock1,12,12,12,12);
	}
#else
	if(m_bFlashed)
	{
		erase();
		bitBlt(this,4,4,g_pIconManager->getSmallIcon(KVI_SMALLICON_MESSAGE),0,0,16,16);
	} else {
		bitBlt(this,0,0,m_iOther ? ((m_iOther == 2) ? g_pDock3 : g_pDock2) : g_pDock1,0,0,12,12,Qt::CopyROP,false);
		bitBlt(this,0,12,m_iConsoles ? ((m_iConsoles == 2) ? g_pDock3 : g_pDock2) : g_pDock1,0,12,12,12,Qt::CopyROP,false);
		bitBlt(this,12,0,m_iQueries ? ((m_iQueries == 2) ? g_pDock3 : g_pDock2) : g_pDock1,12,0,12,12,Qt::CopyROP,false);
		bitBlt(this,12,12,m_iChannels ? ((m_iChannels == 2) ? g_pDock3 : g_pDock2) : g_pDock1,12,12,12,12,Qt::CopyROP,false);
	}
#endif
}


static KviDockWidget * dockwidget_find(KviFrame *f)
{
	if(!g_pDockWidgetList)return 0;
	for(KviDockWidget * w = g_pDockWidgetList->first();w;w = g_pDockWidgetList->next())
	{
		if(w->frame() == f)return w;
	}
	return 0;
}

/*
	@doc: dockwidget.show
	@type:
		command
	@title:
		dockwidget.show
	@short:
		Shows the dock widget for the current frame window
	@keyterms:
		dock widget , system tray
	@syntax:
		dockwidget.show
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
		[cmd]dockwidget.hide[/doc]
*/

static bool dockwidget_kvs_cmd_show(KviKvsModuleCommandCall * c)
{ 
	if(!(dockwidget_find(c->window()->frame())))
	{
		KviDockWidget * w = new KviDockWidget(c->window()->frame(),"dock_widget");
#ifndef COMPILE_ON_WINDOWS
		w->show();
#else
		w->hide();
#endif
	}
	return true;
}

/*
	@doc: dockwidget.hide
	@type:
		command
	@title:
		dockwidget.hide
	@short:
		Hides the dock widget for the current frame window
	@syntax:
		dockwidget.hide
	@description:
		Hides the dock widget for the current frame window
	@seealso:
		[cmd]dockwidget.show[/doc]
*/

static bool dockwidget_kvs_cmd_hide(KviKvsModuleCommandCall * c)
{ 
	KviDockWidget * w= dockwidget_find(c->window()->frame());
	if(w)delete w;
	// show the parent frame.. otherwise there will be no way to get it back
	if(!c->window()->frame()->isVisible())
	{
		c->window()->frame()->show();
	}
	return true;
}

/*
	@doc: dockwidget.hidewindow
	@type:
		command
	@title:
		dockwidget.hidewindow
	@short:
		Returns the state of the dock widget
	@syntax:
		dockwidget.hidewindow
	@description:
		Hides the window, assotiated with dockwidget
	@seealso:
		[cmd]dockwidget.show[/cmd] [cmd]dockwidget.hide[/cmd]
*/

static bool dockwidget_kvs_cmd_hidewindow(KviKvsModuleCommandCall * c)
{ 
	KviDockWidget * w= dockwidget_find(c->window()->frame());
	if(w)
	{
#if QT_VERSION > 0x030201
		w->setPrevWindowState(c->window()->frame()->windowState());
#endif
		c->window()->frame()->hide();
	}
	return true;
}

/*
	@doc: dockwidget.isvisible
	@type:
		function
	@title:
		$dockwidget.isVisible
	@short:
		Returns the state of the dock widget
	@syntax:
		$reguser.isVisible()
	@description:
		Returns 1 if the dock widget is actually visible , 0 otherwise.
	@seealso:
		[cmd]dockwidget.show[/cmd]
*/

static bool dockwidget_kvs_fnc_isvisible(KviKvsModuleFunctionCall * c)
{ 
	c->returnValue()->setBoolean(dockwidget_find(c->window()->frame()));
	return true;
}



// =======================================
// init routine
// =======================================
static bool dockwidget_module_init(KviModule * m)
{
	KviStr buffer;
#ifdef COMPILE_ON_WINDOWS
	g_pApp->findImage(buffer,"kvi_dock_win32.png");
	KviImageLibrary l1(buffer.ptr(),16,16);
#else
	g_pApp->findImage(buffer,"kvi_dock.png");
	KviImageLibrary l1(buffer.ptr(),22,22);
#endif
	g_pDock1 = new QPixmap(l1.getImage(0));
	g_pDock2 = new QPixmap(l1.getImage(1));
	g_pDock3 = new QPixmap(l1.getImage(2));

#ifdef COMPILE_ON_WINDOWS
	
#endif


	g_pDockWidgetList = new KviPtrList<KviDockWidget>;
	g_pDockWidgetList->setAutoDelete(false);


	KVSM_REGISTER_SIMPLE_COMMAND(m,"hide",dockwidget_kvs_cmd_hide);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"hidewindow",dockwidget_kvs_cmd_hidewindow);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"show",dockwidget_kvs_cmd_show);
	KVSM_REGISTER_FUNCTION(m,"isVisible",dockwidget_kvs_fnc_isvisible);

	return true;
}

static bool dockwidget_module_cleanup(KviModule *m)
{
	while(g_pDockWidgetList->first())delete g_pDockWidgetList->first();
	delete g_pDockWidgetList;
    g_pDockWidgetList = 0;

#ifdef COMPILE_ON_WINDOWS
	DestroyIcon(g_pCurrentIcon);
#endif

	delete g_pDock1;
    g_pDock1 = 0;
	delete g_pDock2;
    g_pDock2 = 0;
	delete g_pDock3;
    g_pDock3 = 0;

	return true;
}

static bool dockwidget_module_can_unload(KviModule *)
{
	return g_pDockWidgetList->isEmpty();
}

// =======================================
// plugin definition structure
// =======================================
KVIRC_MODULE(
    "KVIrc dock widget implementation",
	"1.0.0",
	"Szymon Stefanek <pragma at kvirc dot net>" ,
	"exports the /dockwidget.* interface\n",
    dockwidget_module_init ,
    dockwidget_module_can_unload,
	0,
	dockwidget_module_cleanup
)

#endif