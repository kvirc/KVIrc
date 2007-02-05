//=============================================================================
//
//   File : kvi_statusbar.cpp
//   Created on Tue 07 Sep 2004 03:56:46 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004 Szymon Stefanek <pragma at kvirc dot net>
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

#define __KVIRC__

#include "kvi_statusbar.h"
#include "kvi_frame.h"

#include <qlabel.h>

#include "kvi_iconmanager.h"
#include "kvi_window.h"
#include "kvi_irccontext.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnectionuserinfo.h"
#include "kvi_locale.h"
#include "kvi_app.h"
#include "kvi_config.h"
#include "kvi_modulemanager.h"
#include "kvi_statusbarapplet.h"
#include "kvi_dynamictooltip.h"

#include <qpainter.h>
#include <qstyle.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qcursor.h>
#include "kvi_tal_popupmenu.h"
#include <qpixmap.h>

#ifdef COMPILE_USE_QT4
	#include <qevent.h>
#endif

// This class COULD be derived also from KStatusBar but in fact
// it adds no graphic functionality and it has only useless methods for us.
// ... for now let's keep it simple :)


#include "kvi_time.h"
#include "kvi_qstring.h"

#include <qfont.h>

/*
	IDEAS:
		- Countdown timer
*/





KviStatusBar::KviStatusBar(KviFrame * pFrame)
: QStatusBar(pFrame)
{

	setProperty("name","statusbar");
	m_pFrame = pFrame;
	// ugh :D
	setSizeGripEnabled(false);
	
	m_pContextPopup = 0;
	m_pAppletsPopup = 0;
	
	m_pClickedApplet = 0;

	m_pAppletDescriptors = new KviDict<KviStatusBarAppletDescriptor>;
	m_pAppletDescriptors->setAutoDelete(true);

	KviStatusBarClock::selfRegister(this);
	KviStatusBarAwayIndicator::selfRegister(this);
	KviStatusBarLagIndicator::selfRegister(this);
	KviStatusBarConnectionTimer::selfRegister(this);
	KviStatusBarSeparator::selfRegister(this);

	m_pAppletList = new KviPtrList<KviStatusBarApplet>;
	m_pAppletList->setAutoDelete(false);

	m_pMessageQueue = new KviPtrList<KviStatusBarMessage>;
	m_pMessageQueue->setAutoDelete(true);

	m_pMessageTimer = 0;

	m_pMessageLabel = new QLabel("<b>[x]</b> x",this,"msgstatuslabel");
	m_pMessageLabel->setMargin(1);
#ifdef COMPILE_USE_QT4
	m_pMessageLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
#else
	m_pMessageLabel->setAlignment(SingleLine | AlignVCenter | AlignLeft);
#endif
	m_pMessageLabel->setMinimumWidth(350);
	
	m_iLastMinimumHeight = 0;
	m_bStopLayoutOnAddRemove = true;

	connect(m_pFrame,SIGNAL(activeContextChanged()),this,SLOT(setPermanentMessage()));
	connect(m_pFrame,SIGNAL(activeContextStateChanged()),this,SLOT(setPermanentMessage()));
	connect(m_pFrame,SIGNAL(activeConnectionUserModeChanged()),this,SLOT(setPermanentMessage()));
	connect(m_pFrame,SIGNAL(activeConnectionNickNameChanged()),this,SLOT(setPermanentMessage()));
	setPermanentMessage();
	
	m_bStopLayoutOnAddRemove = false;

	m_pToolTip = new KviDynamicToolTip(this);
	connect(m_pToolTip,SIGNAL(tipRequest(KviDynamicToolTip *,const QPoint &)),this,SLOT(tipRequest(KviDynamicToolTip *,const QPoint &)));

	updateLayout();
}

KviStatusBar::~KviStatusBar()
{
	save();

	m_bStopLayoutOnAddRemove = true;

	if(m_pMessageTimer)delete m_pMessageTimer;
	delete m_pMessageQueue;
	delete m_pAppletDescriptors;
	delete m_pAppletList;
}

void KviStatusBar::load()
{
	KviStr szBuf;
	if(!g_pApp->getReadOnlyConfigPath(szBuf,"statusbar.kvc"))return; // no config file at all

	KviConfig cfg(szBuf.ptr(),KviConfig::Read);
	cfg.setGroup("Applets");
	
	int nApplets = cfg.readIntEntry("Count",0);
	for(int i=0;i<nApplets;i++)
	{
		KviStr prefix(KviStr::Format,"Applet%d",i);
		KviStr tmp(KviStr::Format,"%s_InternalName",prefix.ptr());
		QString szInternalName = cfg.readEntry(tmp.ptr(),"");
		if(!szInternalName.isEmpty())
		{
			tmp.sprintf("%s_PreloadModule",prefix.ptr());
			QString szPreloadModule = cfg.readEntry(tmp.ptr(),"");
			if(!szPreloadModule.isEmpty())
				g_pModuleManager->getModule(szPreloadModule.utf8().data());

			KviStatusBarApplet * a = createApplet(szInternalName);
			if (a)
				a->loadState(prefix.ptr(),&cfg);
			else
				debug("warning: failed to create applet %s (preload: %s)!",
					szInternalName.utf8().data(), szPreloadModule.utf8().data());
		}
	}
}

void KviStatusBar::save()
{
	// FIXME: This will preserve the settings of the last saved KviFrame's statusbar only :/
	KviStr szBuf;
	g_pApp->getLocalKvircDirectory(szBuf,KviApp::Config,"statusbar.kvc");

	KviConfig cfg(szBuf.ptr(),KviConfig::Write);
	cfg.setGroup("Applets");
	
	cfg.writeEntry("Count",m_pAppletList->count());
	
	int i = 0;
	for(KviStatusBarApplet * a = m_pAppletList->first();a;a = m_pAppletList->next())
	{
		KviStr prefix(KviStr::Format,"Applet%d",i);
		KviStr tmp(KviStr::Format,"%s_InternalName",prefix.ptr());
		cfg.writeEntry(tmp.ptr(),a->descriptor()->internalName());
		a->saveState(prefix.ptr(),&cfg);
		if(!(a->descriptor()->preloadModule().isEmpty()))
		{
			tmp.sprintf("%s_PreloadModule",prefix.ptr());
			cfg.writeEntry(tmp.ptr(),a->descriptor()->preloadModule());
		}
		i++;
	}
}

#define VMARGIN 3
#define HMARGIN 4
#define SPACING 3
#define RICHTEXTLABELTRICK 2

void KviStatusBar::layoutChildren()
{
	int x = width() - HMARGIN;
	int h = height() - (VMARGIN * 2);
	for(KviStatusBarApplet * a = m_pAppletList->last();a;a = m_pAppletList->prev())
	{
		int w = a->sizeHint().width();
		x -= w;
		a->setGeometry(x,VMARGIN,w,h);
		x -= SPACING;
	}
	// trick to center vertically the rich text label: make it some pixels smaller
	m_pMessageLabel->setGeometry(HMARGIN,VMARGIN,x - HMARGIN,h - RICHTEXTLABELTRICK);
}

void KviStatusBar::resizeEvent(QResizeEvent * e)
{
	layoutChildren();
}

bool KviStatusBar::event(QEvent * e)
{
	if(e->type() == QEvent::LayoutHint)
	{
		updateLayout();
		return false; // send to parents too!
	}
	return QStatusBar::event(e);
}

void KviStatusBar::recalcMinimumHeight()
{
	int s = 18;
	int h = m_pMessageLabel->sizeHint().height();
	if(h > s)s = h;
	for(KviStatusBarApplet * a = m_pAppletList->last();a;a = m_pAppletList->prev())
	{
		h = a->sizeHint().height();
		if(h > s)s = h;
	}
	s += (VMARGIN * 2) + RICHTEXTLABELTRICK;
	if(m_iLastMinimumHeight != s)
	{
		m_iLastMinimumHeight = s;
		setMinimumHeight(s);
		QLayout * l = layout();
		if(l)
			if(l->inherits("QBoxLayout"))
				((QBoxLayout *)l)->addStrut(s);
		// FIXME: do QMainWindow need setUpLayout() here ?
	}
}

bool KviStatusBar::appletExists(KviStatusBarApplet * pApplet)
{
	return (m_pAppletList->findRef(pApplet) != -1);
}

KviStatusBarApplet * KviStatusBar::appletAt(const QPoint &pnt,bool bBestMatch)
{
	QPoint local = mapFromGlobal(pnt);
	if(bBestMatch)
	{
		for(KviStatusBarApplet * a = m_pAppletList->first();a;a = m_pAppletList->next())
		{
			if(local.x() <= (a->x() + a->width()))return a;
		}
		return m_pAppletList->last(); // last anyway
	}
	
	for(KviStatusBarApplet * a = m_pAppletList->first();a;a = m_pAppletList->next())
	{
		if((local.x() >= a->x()) && (local.y() >= a->y()))
		{
			if((local.x() <= (a->x() + a->width())) && (local.y() <= (a->y() + a->height())))
			{
				return a;
			}
		}
	}
	return 0;
}

void KviStatusBar::tipRequest(KviDynamicToolTip *pTip,const QPoint &pnt)
{
	KviStatusBarApplet * a = appletAt(mapToGlobal(pnt));
	QString szTip;
	QRect r;
	if(a)
	{
		szTip = "<table width=\"100%\"><tr><td bgcolor=\"#303030\" align=\"center\"><font color=\"#ffffff\"><b>" + a->descriptor()->visibleName() + "</b></font></td></tr>";

		QString szTipx = a->tipText(a->mapFromGlobal(mapToGlobal(pnt)));
		if(!szTipx.isEmpty())
		{
			szTip += "<tr><td>";
			szTip += szTipx;
			szTip += "</td></tr><tr><td align=\"center\"><hr></td></tr>";
		}

		szTip += "<tr><td align=\"center\"><font color=\"#808080\" size=\"-1\">";
		szTip += __tr2qs("<b>Shift+Drag</b> or <b>Ctrl+Drag</b> to move the applet around<br><b>Right click</b> to see the other options");
		szTip += "</font></td></tr></table>";
		r = QRect(a->x(),a->y(),a->width(),a->height());
	} else {
		szTip = "<center>";
		szTip += __tr2qs("<b>Right click</b> to add/remove applets");
		szTip += "</center>";
		r = QRect(m_pMessageLabel->x(),m_pMessageLabel->y(),m_pMessageLabel->width(),m_pMessageLabel->height());
	}
	pTip->tip(r,szTip);
}

KviTalPopupMenu * KviStatusBar::contextPopup()
{
	if(!m_pContextPopup)
	{
		m_pContextPopup = new KviTalPopupMenu(this);
		connect(m_pContextPopup,SIGNAL(aboutToShow()),this,SLOT(contextPopupAboutToShow()));
	}
	m_pClickedApplet = appletAt(QCursor::pos());
	return m_pContextPopup;
}

void KviStatusBar::contextPopupAboutToShow()
{
	if(!m_pContextPopup)return;
	m_pContextPopup->clear();
	

	if(appletExists(m_pClickedApplet))
	{
		QString app = m_pClickedApplet->descriptor()->visibleName();

		QString tmp;
		KviQString::sprintf(tmp,"<center><b>%Q</b></center>",&app);

#ifndef COMPILE_USE_QT4
		// FIXME: This is not supported under Qt4.. :(
		QLabel * l = new QLabel(tmp,m_pContextPopup);
		l->setFrameStyle(QFrame::Raised | QFrame::StyledPanel);
		m_pContextPopup->insertItem(l);
#endif

		m_pClickedApplet->fillContextPopup(m_pContextPopup);
		
		KviQString::sprintf(tmp,__tr2qs("Remove %Q"),&app);
		m_pContextPopup->insertSeparator();
		m_pContextPopup->insertItem(tmp,this,SLOT(removeClickedApplet()));
	}

	if(!m_pAppletsPopup)
	{
		m_pAppletsPopup = new KviTalPopupMenu(this);
		connect(m_pAppletsPopup,SIGNAL(aboutToShow()),this,SLOT(appletsPopupAboutToShow()));
		connect(m_pAppletsPopup,SIGNAL(activated(int)),this,SLOT(appletsPopupActivated(int)));
	}
	
	m_pContextPopup->insertItem(__tr2qs("Add Applet"),m_pAppletsPopup);
}

void KviStatusBar::removeClickedApplet()
{
	if(!appletExists(m_pClickedApplet))return;
	delete m_pClickedApplet;
	m_pClickedApplet = 0;
}

void KviStatusBar::appletsPopupAboutToShow()
{
	if(!m_pAppletsPopup)return;
	m_pAppletsPopup->clear();
	
	// FIXME: could we cache the module results in some way ?
	g_pModuleManager->loadModulesByCaps("statusbarapplet");
	
	KviDictIterator<KviStatusBarAppletDescriptor> it(*m_pAppletDescriptors);
	while(KviStatusBarAppletDescriptor * d = it.current())
	{
		int id;
		QPixmap * pix = d->icon();
		if(pix)id = m_pAppletsPopup->insertItem(*pix,d->visibleName());
		else id = m_pAppletsPopup->insertItem(d->visibleName());
		m_pAppletsPopup->setItemParameter(id,d->id());
		++it;
	}
}

KviStatusBarApplet * KviStatusBar::createApplet(const QString &szInternalName)
{
	KviStatusBarAppletDescriptor * d = m_pAppletDescriptors->find(szInternalName);
	if(!d)return 0;
	return d->create(this);
}

void KviStatusBar::showLayoutHelp()
{
	queueMessage(new KviStatusBarMessage(__tr2qs("Drag the applet while holding the Shift or Ctrl key to move it to the desired position")));
}

void KviStatusBar::appletsPopupActivated(int id)
{
	// FIXME: In fact the applet descriptors in modules could
	//        have been unloaded while the popup was being shown...
	//        For now we just assume that this never happens :D

	if(!m_pAppletsPopup)return;
	int par = m_pAppletsPopup->itemParameter(id);
	KviDictIterator<KviStatusBarAppletDescriptor> it(*m_pAppletDescriptors);
	while(KviStatusBarAppletDescriptor * d = it.current())
	{
		if(par == d->id())
		{
			if(m_pClickedApplet)
			{
				int idx = m_pAppletList->findRef(m_pClickedApplet);
				if(idx != -1)
				{
					// try to put the new applet just after the clicked one
					bool bSave = m_bStopLayoutOnAddRemove;
					m_bStopLayoutOnAddRemove = true;
					KviStatusBarApplet * a = d->create(this);
					m_pAppletList->removeRef(a);
					m_pAppletList->insert(idx + 1,a);
					m_bStopLayoutOnAddRemove = bSave;
					if(!m_bStopLayoutOnAddRemove)updateLayout();
					showLayoutHelp();
					return;
				}
			}
			d->create(this);
			showLayoutHelp();
			return;
		}
		++it;
	}
}

void KviStatusBar::registerAppletDescriptor(KviStatusBarAppletDescriptor * d)
{
	m_pAppletDescriptors->replace(d->internalName(),d);
}

void KviStatusBar::registerApplet(KviStatusBarApplet * a)
{
	m_pAppletList->append(a);
	if(!a->isVisible())a->show();
	if(!m_bStopLayoutOnAddRemove)updateLayout();
}

void KviStatusBar::unregisterApplet(KviStatusBarApplet * a)
{
	if(!a)return;
	m_pAppletList->removeRef(a);
	if(a->isVisible())a->hide();
	if(!m_bStopLayoutOnAddRemove)updateLayout();
}


void KviStatusBar::paintEvent(QPaintEvent * e)
{
	// avoid the ugly rectangle around the widgets painted by QStatusBar
//	QPainter p(this);
//	style().drawPrimitive(QStyle::PE_Panel,&p,rect(),colorGroup(),QStyle::Style_Raised,QStyleOption(1,1));
	//QStatusBar::paintEvent(e);
	//qDrawWinPanel(&p,0,0,width(),height(),colorGroup(),false,0);
}

void KviStatusBar::mousePressEvent(QMouseEvent * e)
{
	m_pClickedApplet = 0;
	if(e->button() & Qt::RightButton)
	{
		contextPopup()->popup(QCursor::pos());
		return;
	}
	if((e->button() & Qt::LeftButton) && (e->state() & (Qt::ShiftButton | Qt::ControlButton)))
	{
		// move!
		m_pClickedApplet = appletAt(mapToGlobal(e->pos()));
		if(!m_pClickedApplet)return;
		m_pClickedApplet->select();
#ifdef COMPILE_USE_QT4
		g_pApp->setOverrideCursor(Qt::SizeAllCursor);
#else
		g_pApp->setOverrideCursor(sizeAllCursor);
#endif
	}
}

void KviStatusBar::mouseMoveEvent(QMouseEvent * e)
{
	if(!m_pClickedApplet)return;
	if(!appletExists(m_pClickedApplet))return;
	QPoint g = mapToGlobal(e->pos());
	KviStatusBarApplet * a = appletAt(g,true);
	if(a == m_pClickedApplet)return;
	// move!
	if(!a)
	{
		a = m_pAppletList->first();
		if(!a)return; // ops!

		if(e->pos().x() < (a->x() + a->width()))
		{
			if(a == m_pClickedApplet)return; // don't move
		} else {
			a = m_pAppletList->last();
			if(!a)return;
			if(a == m_pClickedApplet)return; // no way to move
		}
	}
	
	m_pAppletList->removeRef(m_pClickedApplet);
	int idx = m_pAppletList->findRef(a);
	if(idx == -1)m_pAppletList->append(m_pClickedApplet); // uhg ?
	else {
		QPoint p = a->mapFromGlobal(g);
		if(p.x() > (a->width() / 2))idx++; // just after
		m_pAppletList->insert(idx,m_pClickedApplet);
	}
	layoutChildren();
}

void KviStatusBar::mouseReleaseEvent(QMouseEvent * e)
{
	if(e->button() & Qt::LeftButton)
	{
		if(m_pClickedApplet && appletExists(m_pClickedApplet))
		{
			m_pClickedApplet->select(false);
			g_pApp->restoreOverrideCursor();
		}
	}
}

void KviStatusBar::queueMessage(KviStatusBarMessage * pMsg)
{
	// FIXME: the priority of the message!!!
	m_pMessageQueue->append(pMsg);
	if(!m_pMessageTimer)showFirstMessageInQueue();
	// else we wait for the message timer to shot
}

void KviStatusBar::messageTimerFired()
{
	if(m_pMessageTimer)m_pMessageTimer->stop();

	if(!m_pMessageQueue->isEmpty())
	{
		// the first message in queue is currently visible
		// kill it
		m_pMessageQueue->removeFirst();
		if(!m_pMessageQueue->isEmpty())
		{
			// something to show
			showFirstMessageInQueue();
		}
	}
	// nothing else to show
	delete m_pMessageTimer;
	m_pMessageTimer = 0;
	
	setPermanentMessage();
}

void KviStatusBar::showFirstMessageInQueue()
{
	KviStatusBarMessage * pMsg = m_pMessageQueue->first();
	if(!pMsg)
	{
		if(m_pMessageTimer)
		{
			delete m_pMessageTimer;
			m_pMessageTimer = 0;
		}
		return;
	}

	if(!m_pMessageTimer)
	{
		m_pMessageTimer = new QTimer(this);
		QObject::connect(m_pMessageTimer,SIGNAL(timeout()),this,SLOT(messageTimerFired()));
	} else {
		m_pMessageTimer->stop();
	}

	m_pMessageLabel->setText("<nobr>" + pMsg->text() + "</nobr>");
	m_pMessageTimer->start(pMsg->timeout());
}


void KviStatusBar::setPermanentMessage()
{
	if(m_pMessageTimer)return; // something is being actually shown!

	KviIrcContext * c = m_pFrame->activeContext();

	QString txt = "<nobr>";

	if(c)
	{
		switch(c->state())
		{
			case KviIrcContext::Connected:
				txt += "<b>[";
				txt += c->connection()->currentServerName();
				txt += "]</b> ";
				txt += c->connection()->currentNickName();
				if(!c->connection()->userInfo()->userMode().isEmpty())
				{
					txt += " (+";
					txt += c->connection()->userInfo()->userMode();
					txt += ")";
				}
			break;
			case KviIrcContext::Connecting:
				txt += __tr2qs("Connection in progress...");
			break;
			case KviIrcContext::LoggingIn:
				txt += "<b>[";
				txt += c->connection()->currentServerName();
				txt += "]</b> ";
				txt += __tr2qs("Login in progress...");
			break;
			default:
				txt += __tr2qs("Not connected");
			break;
		}
	} else {
		txt += __tr2qs("No IRC context");
	}

	txt += "</nobr>";

	m_pMessageLabel->setText(txt);
}
