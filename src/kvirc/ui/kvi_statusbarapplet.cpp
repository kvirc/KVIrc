//=============================================================================//
//   File : kvi_statusbarapplet.cpp
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

#include "kvi_statusbarapplet.h"
#include "kvi_frame.h"
#include "kvi_iconmanager.h"
#include "kvi_window.h"
#include "kvi_irccontext.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnectionuserinfo.h"
#include "kvi_ircconnectionstatistics.h"
#include "kvi_locale.h"
#include "kvi_app.h"
#include "kvi_config.h"
#include "kvi_modulemanager.h"
#include "kvi_console.h"
#include "kvi_lagmeter.h"
#include "kvi_options.h"
#include "kvi_kvs_script.h"
#include "kvi_time.h"
#include "kvi_qstring.h"
#include "kvi_buildinfo.h"
#include "kvi_http.h"
#include "kvi_url.h"
#include "kvi_tal_popupmenu.h"

#include <QPainter>
#include <QStyle>
#include <QLayout>
#include <QTimer>
#include <QCursor>
#include <QPixmap>
#include <QFont>
#include <QMessageBox>
#include <QEvent>
#include <QMouseEvent>

// This class COULD be derived also from KStatusBar but in fact
// it adds no graphic functionality and it has only useless methods for us.
// ... for now let's keep it simple :)
// FIXME: Applets in modules SHOULD be unregistered automatically on unload!
/*
	IDEAS:
		- Countdown timer
*/

KviStatusBarAppletDescriptor::KviStatusBarAppletDescriptor(const QString &szVisibleName,const QString &szInternalName,CreateAppletCallback pProc,const QString &szPreloadModule,const QPixmap &pixIcon)
: KviHeapObject()
{
	static int s_iAppletDescriptorUniqueId = 0;
	m_iId = s_iAppletDescriptorUniqueId;
	s_iAppletDescriptorUniqueId++;
	m_szVisibleName = szVisibleName;
	m_szInternalName = szInternalName;
	m_szPreloadModule = szPreloadModule;
	m_pProc = pProc;
	m_pAppletList = new KviPointerList<KviStatusBarApplet>;
	m_pAppletList->setAutoDelete(false);
	if(!pixIcon.isNull())m_pIcon = new QPixmap(pixIcon);
	else m_pIcon = 0;
}

KviStatusBarAppletDescriptor::~KviStatusBarAppletDescriptor()
{
	while(KviStatusBarApplet * a = m_pAppletList->first())delete a;
	delete m_pAppletList;
	if(m_pIcon)delete m_pIcon;
}

KviStatusBarApplet * KviStatusBarAppletDescriptor::create(KviStatusBar * pBar)
{
	return m_pProc(pBar,this);
}

void KviStatusBarAppletDescriptor::registerApplet(KviStatusBarApplet * a)
{
	m_pAppletList->append(a);
}

void KviStatusBarAppletDescriptor::unregisterApplet(KviStatusBarApplet * a)
{
	m_pAppletList->removeRef(a);
}


KviStatusBarApplet::KviStatusBarApplet(KviStatusBar * pParent,KviStatusBarAppletDescriptor *pDescriptor)
: QLabel(pParent), m_pStatusBar(pParent), m_pDescriptor(pDescriptor)
{
	m_pDescriptor->registerApplet(this);
	m_pStatusBar->registerApplet(this);
	m_bSelected = false;
}

KviStatusBarApplet::~KviStatusBarApplet()
{
	m_pDescriptor->unregisterApplet(this);
	m_pStatusBar->unregisterApplet(this);
}

QString KviStatusBarApplet::tipText(const QPoint &)
{
	return QString::null;
}

void KviStatusBarApplet::paintEvent(QPaintEvent * e)
{
	QLabel::paintEvent(e);
	setFont(KVI_OPTION_FONT(KviOption_fontIrcToolBarApplet));
	if(m_bSelected)
	{
		QPainter p(this);
		p.setCompositionMode(QPainter::CompositionMode_SourceOut);
		p.fillRect(rect(),Qt::black);
		p.setCompositionMode(QPainter::CompositionMode_SourceOver);
	}
}

void KviStatusBarApplet::select(bool bSelect)
{
	if(m_bSelected == bSelect)return;
	m_bSelected = bSelect;
	update();
}


// Away applet
KviStatusBarAwayIndicator::KviStatusBarAwayIndicator(KviStatusBar * pParent,KviStatusBarAppletDescriptor *pDescriptor)
: KviStatusBarApplet(pParent,pDescriptor)
{
	m_bAwayOnAllContexts = false;
	connect(pParent->frame(),SIGNAL(activeContextChanged()),this,SLOT(updateDisplay()));
	connect(pParent->frame(),SIGNAL(activeContextStateChanged()),this,SLOT(updateDisplay()));
	connect(pParent->frame(),SIGNAL(activeConnectionAwayStateChanged()),this,SLOT(updateDisplay()));

	updateDisplay();

	if(!pixmap())
		setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NOTAWAY)));
}

KviStatusBarAwayIndicator::~KviStatusBarAwayIndicator()
{
}

void KviStatusBarAwayIndicator::updateDisplay()
{
	KviIrcContext * c = statusBar()->frame()->activeContext();

	if(c && c->isConnected())
	{
		setPixmap(c->connection()->userInfo()->isAway() ?
				*(g_pIconManager->getSmallIcon(KVI_SMALLICON_AWAY)) : *(g_pIconManager->getSmallIcon(KVI_SMALLICON_NOTAWAY)));
	} else {
		// FIXME: We'd like to appear disabled here... but then we
		//        no longer get mouse events :/
		setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NOTAWAY)));
	}
}

void KviStatusBarAwayIndicator::toggleContext()
{
	m_bAwayOnAllContexts = !m_bAwayOnAllContexts;
}

void KviStatusBarAwayIndicator::fillContextPopup(KviTalPopupMenu *p)
{
	int id = p->insertItem(__tr2qs("Apply to all IRC Contexts"),this,SLOT(toggleContext()));
	p->setItemChecked(id,m_bAwayOnAllContexts);	
}

void KviStatusBarAwayIndicator::loadState(const char * prefix,KviConfig *cfg)
{
	KviStr tmp(KviStr::Format,"%s_AwayOnAllContexts",prefix);
	m_bAwayOnAllContexts = cfg->readBoolEntry(tmp.ptr(),false);
}

void KviStatusBarAwayIndicator::saveState(const char * prefix,KviConfig *cfg)
{
	KviStr tmp(KviStr::Format,"%s_AwayOnAllContexts",prefix);
	cfg->writeEntry(tmp.ptr(),m_bAwayOnAllContexts);
}

KviStatusBarApplet * CreateStatusBarAwayIndicator(KviStatusBar * pBar,KviStatusBarAppletDescriptor *pDescriptor)
{
	return new KviStatusBarAwayIndicator(pBar,pDescriptor);
}

void KviStatusBarAwayIndicator::selfRegister(KviStatusBar * pBar)
{
	KviStatusBarAppletDescriptor * d = new KviStatusBarAppletDescriptor(
		__tr2qs("Away Indicator"),"awayindicator",CreateStatusBarAwayIndicator,"",*(g_pIconManager->getSmallIcon(KVI_SMALLICON_AWAY)));
	pBar->registerAppletDescriptor(d);
}

// FIXME: Away on all context should know where user is not away/back before toggling status
void KviStatusBarAwayIndicator::mouseDoubleClickEvent(QMouseEvent * e)
{
	if(!(e->button() & Qt::LeftButton))return;
	KviIrcConnection * c = statusBar()->frame()->activeConnection();
	if(!c)return;
	if(c->state() != KviIrcConnection::Connected)return;
	QString command;
	if(m_bAwayOnAllContexts)
		command = "if($away)back -a; else away -a";
	else
		command = "if($away)back; else away";
	KviKvsScript::run(command,c->console());
}

QString KviStatusBarAwayIndicator::tipText(const QPoint &)
{
	KviIrcConnection * c = statusBar()->frame()->activeConnection();
	QString ret = "<center><b>";
	if(!c)goto not_connected;
	if(c->state() != KviIrcConnection::Connected)goto not_connected;
	if(c->userInfo()->isAway())
	{
		QString tmp = KviTimeUtils::formatTimeInterval(kvi_unixTime() - c->userInfo()->awayTime(),KviTimeUtils::NoLeadingEmptyIntervals);
		ret += __tr2qs("Away since");
		ret += ' ';
		ret += tmp;
		ret += "</b><br>";
		ret += __tr2qs("Double click to leave away mode");
	} else {
		ret += __tr2qs("Not away");
		ret += "</b><br>";
		ret += __tr2qs("Double click to enter away mode");
	}
	ret += "</center>";
	return ret;
	
not_connected:
	ret +=  __tr2qs("Not connected");
	ret += "</b></center>";
	return ret;
}


// Lag indicator applet
KviStatusBarLagIndicator::KviStatusBarLagIndicator(KviStatusBar * pParent,KviStatusBarAppletDescriptor *pDescriptor)
: KviStatusBarApplet(pParent,pDescriptor)
{
	connect(pParent->frame(),SIGNAL(activeContextChanged()),this,SLOT(updateDisplay()));
	connect(pParent->frame(),SIGNAL(activeContextStateChanged()),this,SLOT(updateDisplay()));
	connect(pParent->frame(),SIGNAL(activeConnectionLagChanged()),this,SLOT(updateDisplay()));

	updateDisplay();

	QFont f = font();
	f.setFixedPitch(true);
	f.setFamily("fixed");
	setFont(f);

	updateDisplay();
}

KviStatusBarLagIndicator::~KviStatusBarLagIndicator()
{
}

void KviStatusBarLagIndicator::mouseDoubleClickEvent(QMouseEvent *e)
{
	if(!(e->button() & Qt::LeftButton))return;

	KviIrcConnection * c = statusBar()->frame()->activeConnection();
	if(!c)return;
	if(c->state() != KviIrcConnection::Connected)return;
	if(!c->lagMeter())
	{
		KVI_OPTION_BOOL(KviOption_boolUseLagMeterEngine) = true;
		g_pApp->restartLagMeters();
	}
}


QString KviStatusBarLagIndicator::tipText(const QPoint &)
{
	KviIrcConnection * c = statusBar()->frame()->activeConnection();
	QString ret = "<center><b>";
	if(!c)goto not_connected;
	if(c->state() != KviIrcConnection::Connected)goto not_connected;
	if(c->lagMeter())
	{
		int lll;
		if((lll = c->lagMeter()->lag()) > 0)
		{
			int llls = lll / 1000;
			int llld = (lll % 1000) / 100;
			int lllc = (lll % 100) / 10;
			KviQString::appendFormatted(ret,__tr2qs("Lag: %d.%d%d"),llls,llld,lllc);
			ret += "</b><br>";
			int vss = c->lagMeter()->secondsSinceLastCompleted();
			int vmm = vss / 60;
			vss = vss % 60;
			KviQString::appendFormatted(ret,__tr2qs("Last checked %d mins %d secs ago"),vmm,vss);
		} else {
			ret += __tr2qs("Lag measure not available yet");
			ret += "</b>";
		}
	} else {
		ret += __tr2qs("Lag meter engine disabled");
		ret += "</b><br>";
		ret += __tr2qs("Double click to enable it");
	}
	ret += "</center>";
	return ret;
	
not_connected:
	ret +=  __tr2qs("Not connected");
	ret += "</b></center>";
	return ret;
}

void KviStatusBarLagIndicator::updateDisplay()
{
	KviIrcContext * c = statusBar()->frame()->activeContext();
	if(!c)return;
	if(c->isConnected())
	{
		KviIrcConnection * ic = c->connection();
		if(ic->lagMeter())
		{
			int lll;
			if((lll = ic->lagMeter()->lag()) > 0)
			{
				int llls = lll / 1000;
				int llld = (lll % 1000) / 100;
				int lllc = (lll % 100) / 10;
				QString tmp;
				KviQString::sprintf(tmp,__tr2qs("Lag: %d.%d%d"),llls,llld,lllc);
				if(lll > 60000)
				{
					// one minute lag!
					// paint it in red
					tmp.prepend("<nobr><font color=\"#e00000\">");
					tmp.append("</font></nobr>");
				}
				setText(tmp);
				return;
			}
		}
	}
	// no lag available
	setText(__tr2qs("Lag: ?.??"));
}

KviStatusBarApplet * CreateStatusBarLagIndicator(KviStatusBar * pBar,KviStatusBarAppletDescriptor *pDescriptor)
{
	return new KviStatusBarLagIndicator(pBar,pDescriptor);
}

void KviStatusBarLagIndicator::selfRegister(KviStatusBar * pBar)
{
	KviStatusBarAppletDescriptor * d = new KviStatusBarAppletDescriptor(
		__tr2qs("Lag Indicator"),"lagindicator",CreateStatusBarLagIndicator,"",*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SERVERPING)));
	pBar->registerAppletDescriptor(d);
}


// Clock applet
KviStatusBarClock::KviStatusBarClock(KviStatusBar * pParent,KviStatusBarAppletDescriptor *pDescriptor)
: KviStatusBarApplet(pParent,pDescriptor)
{
	m_bUtc = false;

	startTimer(1000);

	QFont f = font();
	f.setFixedPitch(true);
	f.setFamily("fixed");
	setFont(f);
}

KviStatusBarClock::~KviStatusBarClock()
{
}

void KviStatusBarClock::timerEvent(QTimerEvent *)
{
	kvi_time_t tt = kvi_unixTime();
	struct tm * t = m_bUtc ? gmtime(&tt) : localtime(&tt);
	QString tmp;
	KviQString::sprintf(tmp,"%d%d:%d%d:%d%d",
		t->tm_hour / 10,
		t->tm_hour % 10,
		t->tm_min / 10,
		t->tm_min % 10,
		t->tm_sec / 10,
		t->tm_sec % 10);
	setText(tmp);
}

void KviStatusBarClock::fillContextPopup(KviTalPopupMenu * p)
{
	int id = p->insertItem("UTC",this,SLOT(toggleUtc()));
	p->setItemChecked(id,m_bUtc);
}

void KviStatusBarClock::toggleUtc()
{
	m_bUtc = !m_bUtc;
	timerEvent(0);
}

void KviStatusBarClock::loadState(const char * prefix,KviConfig *cfg)
{
	KviStr tmp(KviStr::Format,"%s_Utc",prefix);
	m_bUtc = cfg->readBoolEntry(tmp.ptr(),false);
}

void KviStatusBarClock::saveState(const char * prefix,KviConfig *cfg)
{
	KviStr tmp(KviStr::Format,"%s_Utc",prefix);
	cfg->writeEntry(tmp.ptr(),m_bUtc);
}

KviStatusBarApplet * CreateStatusBarClock(KviStatusBar * pBar,KviStatusBarAppletDescriptor *pDescriptor)
{
	return new KviStatusBarClock(pBar,pDescriptor);
}

void KviStatusBarClock::selfRegister(KviStatusBar * pBar)
{
	KviStatusBarAppletDescriptor * d = new KviStatusBarAppletDescriptor(
		__tr2qs("Simple Clock"),"clock",CreateStatusBarClock,"",*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TIME)));
	pBar->registerAppletDescriptor(d);
}


// Connection timer applet
KviStatusBarConnectionTimer::KviStatusBarConnectionTimer(KviStatusBar * pParent,KviStatusBarAppletDescriptor *pDescriptor)
: KviStatusBarApplet(pParent,pDescriptor)
{
	startTimer(1000);
	m_bTotal=0;
}

KviStatusBarConnectionTimer::~KviStatusBarConnectionTimer()
{
}
//g_pApp->topmostConnectedConsole()
void KviStatusBarConnectionTimer::timerEvent(QTimerEvent * e)
{
	if(m_bTotal)
	{
		setText(KviTimeUtils::formatTimeInterval(KVI_OPTION_UINT(KviOption_uintTotalConnectionTime)));
	} else {
		if(g_pActiveWindow)
		{
			KviIrcContext * c = g_pActiveWindow->context();
			if(c)
			{
				if(c->isConnected())
				{
					KviIrcConnection * cnn = c->connection();
					if(cnn)
					{
						setText(KviTimeUtils::formatTimeInterval(kvi_unixTime() - cnn->statistics()->connectionStartTime()));
						return;
					}
				}
			}
		}
	
		setText(KviTimeUtils::formatTimeInterval(0,KviTimeUtils::FillWithHypens));
	}
	return;
}

void KviStatusBarConnectionTimer::toggleTotal()
{
	m_bTotal = !m_bTotal;
}

void KviStatusBarConnectionTimer::fillContextPopup(KviTalPopupMenu *p)
{
	int id = p->insertItem(__tr2qs("Show total connection time"),this,SLOT(toggleTotal()));
	p->setItemChecked(id,m_bTotal);
}

void KviStatusBarConnectionTimer::loadState(const char * prefix,KviConfig *cfg)
{
	KviStr tmp(KviStr::Format,"%s_Total",prefix);
	m_bTotal = cfg->readBoolEntry(tmp.ptr(),false);
}

void KviStatusBarConnectionTimer::saveState(const char * prefix,KviConfig *cfg)
{
	KviStr tmp(KviStr::Format,"%s_Total",prefix);
	cfg->writeEntry(tmp.ptr(),m_bTotal);
}

KviStatusBarApplet * CreateStatusBarConnectionTimer(KviStatusBar * pBar,KviStatusBarAppletDescriptor *pDescriptor)
{
	return new KviStatusBarConnectionTimer(pBar,pDescriptor);
}

void KviStatusBarConnectionTimer::selfRegister(KviStatusBar * pBar)
{
	KviStatusBarAppletDescriptor * d = new KviStatusBarAppletDescriptor(
		__tr2qs("Connection Timer"),"connectiontimer",CreateStatusBarConnectionTimer,"",*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TIME)));
	pBar->registerAppletDescriptor(d);
}


// Separator applet
KviStatusBarSeparator::KviStatusBarSeparator(KviStatusBar * pParent,KviStatusBarAppletDescriptor *pDescriptor)
: KviStatusBarApplet(pParent,pDescriptor)
{
	setFrameStyle(QFrame::VLine | QFrame::Sunken);
}

KviStatusBarSeparator::~KviStatusBarSeparator()
{
}

KviStatusBarApplet * CreateStatusBarSeparator(KviStatusBar * pBar,KviStatusBarAppletDescriptor *pDescriptor)
{
	return new KviStatusBarSeparator(pBar,pDescriptor);
}

void KviStatusBarSeparator::selfRegister(KviStatusBar * pBar)
{
	KviStatusBarAppletDescriptor * d = new KviStatusBarAppletDescriptor(
		__tr2qs("Separator"),"separator",CreateStatusBarSeparator);
	pBar->registerAppletDescriptor(d);
}


// Update applet
KviStatusBarUpdateIndicator::KviStatusBarUpdateIndicator(KviStatusBar * pParent,KviStatusBarAppletDescriptor *pDescriptor)
: KviStatusBarApplet(pParent,pDescriptor)
{
	m_bUpdateStatus = false;
	m_bUpdateOnStartup = false;
	m_pHttpRequest = 0;

	updateDisplay();

	if(!pixmap())
		setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NOTUPDATE)));

	connect(m_pHttpRequest,SIGNAL(resolvingHost(const QString &)),this,SLOT(hostResolved(const QString &)));
	connect(m_pHttpRequest,SIGNAL(connectionEstabilished()),this,SLOT(connectionEstabilished()));
	connect(m_pHttpRequest,SIGNAL(receivedResponse(const QString &)),this,SLOT(responseReceived(const QString &)));
	connect(m_pHttpRequest,SIGNAL(binaryData(const KviDataBuffer &)),this,SLOT(binaryDataReceived(const KviDataBuffer &)));
	connect(m_pHttpRequest,SIGNAL(terminated(bool)),this,SLOT(requestCompleted(bool)));
	qDebug("Connected signals");
}

KviStatusBarUpdateIndicator::~KviStatusBarUpdateIndicator()
{
}

void KviStatusBarUpdateIndicator::updateDisplay()
{
	setPixmap(m_bUpdateStatus ?
		*(g_pIconManager->getSmallIcon(KVI_SMALLICON_UPDATE)) : *(g_pIconManager->getSmallIcon(KVI_SMALLICON_NOTUPDATE)));
}

void KviStatusBarUpdateIndicator::toggleContext()
{
	m_bUpdateOnStartup = !m_bUpdateOnStartup;
}

void KviStatusBarUpdateIndicator::fillContextPopup(KviTalPopupMenu *p)
{
	int id = p->insertItem(__tr2qs("Check on startup"),this,SLOT(toggleContext()));
	p->setItemChecked(id,m_bUpdateOnStartup);
}

void KviStatusBarUpdateIndicator::loadState(const char * prefix,KviConfig *cfg)
{
	KviStr tmp(KviStr::Format,"%s_UpdateOnStartup",prefix);
	m_bUpdateOnStartup = cfg->readBoolEntry(tmp.ptr(),false);
}

void KviStatusBarUpdateIndicator::saveState(const char * prefix,KviConfig *cfg)
{
	KviStr tmp(KviStr::Format,"%s_UpdateOnStartup",prefix);
	cfg->writeEntry(tmp.ptr(),m_bUpdateOnStartup);
}

KviStatusBarApplet * CreateStatusBarUpdateIndicator(KviStatusBar * pBar,KviStatusBarAppletDescriptor *pDescriptor)
{
	return new KviStatusBarUpdateIndicator(pBar,pDescriptor);
}

void KviStatusBarUpdateIndicator::selfRegister(KviStatusBar * pBar)
{
	KviStatusBarAppletDescriptor * d = new KviStatusBarAppletDescriptor(
		__tr2qs("Update Indicator"),"updateindicator",CreateStatusBarUpdateIndicator,"",*(g_pIconManager->getSmallIcon(KVI_SMALLICON_UPDATE)));
	pBar->registerAppletDescriptor(d);
}

void KviStatusBarUpdateIndicator::mouseDoubleClickEvent(QMouseEvent * e)
{
	qDebug("Update mouse double click event");
	if(!(e->button() & Qt::LeftButton))return;

	QString szFileName;
	KviUrl url("http://kvirc.net/checkversion.phps");

	qDebug("Created http object");
	m_pHttpRequest = new KviHttpRequest();
	qDebug("Making http request");
	m_pHttpRequest->get(url,KviHttpRequest::WholeFile,szFileName);
}

void KviStatusBarUpdateIndicator::hostResolved(const QString &host)
{
	qDebug("Resolved host: %s",host.toUtf8().data());
}

void KviStatusBarUpdateIndicator::connectionEstabilished()
{
	qDebug("Connection established");
}

void KviStatusBarUpdateIndicator::responseReceived(const QString &response)
{
	qDebug("Remote response: %s",response.toUtf8().data());

	if(response != "HTTP/1.1 200 OK")
		QMessageBox::warning(this,"Update Indicator",response,QMessageBox::Ok);
}

void KviStatusBarUpdateIndicator::binaryDataReceived(const KviDataBuffer &buffer)
{
	// Got data
	KviStr szData((const char *)buffer.data(),buffer.size());
	qDebug("Data received: %s",szData.ptr());

	QString url = "http://kvirc.net/?id=releases&platform=";
	QString system = KviBuildInfo::buildSystemName();
	qDebug("System: %s",system.toUtf8().data());
	if(system == "Windows") system = "win32";
	else if(system == "Darwin") system = "macosx";
	else system = "unix";
	qDebug("System: %s",system.toUtf8().data());

	url += system;
	//url += "&version=";
	//url += version;
	qDebug("URL: %s",url.toUtf8().data());

	// Create command to run
	QString command = "openurl ";
	command += url;

	// Open the download page for the platform we're using
	//KviKvsScript::run(command,

	/*
	KviIrcConnection * c = statusBar()->frame()->activeConnection();
	if(!c)return;
	if(c->state() != KviIrcConnection::Connected)return;
	QString command;
	if(m_bAwayOnAllContexts)
		command = "if($away)back -a; else away -a";
	else
		command = "if($away)back; else away";
	KviKvsScript::run(command,c->console());
	*/
}

void KviStatusBarUpdateIndicator::requestCompleted(bool status)
{
	qDebug("Data transfer terminated");
	// this make kvirc segfault
	if(status) delete m_pHttpRequest;
	qDebug("Deleted http object");
}

QString KviStatusBarUpdateIndicator::tipText(const QPoint &)
{
	/*
	KviIrcConnection * c = statusBar()->frame()->activeConnection();
	QString ret = "<center><b>";
	if(!c)goto not_connected;
	if(c->state() != KviIrcConnection::Connected)goto not_connected;
	if(c->userInfo()->isAway())
	{
		QString tmp = KviTimeUtils::formatTimeInterval(kvi_unixTime() - c->userInfo()->awayTime(),KviTimeUtils::NoLeadingEmptyIntervals);
		ret += __tr2qs("Away since");
		ret += ' ';
		ret += tmp;
		ret += "</b><br>";
		ret += __tr2qs("Double click to leave away mode");
	} else {
		ret += __tr2qs("Not away");
		ret += "</b><br>";
		ret += __tr2qs("Double click to enter away mode");
	}
	ret += "</center>";
	return ret;
	
not_connected:
	ret +=  __tr2qs("Not connected");
	ret += "</b></center>";
	return ret;
	*/
	return QString::null;
}
