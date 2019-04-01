//=============================================================================
//
//   File : KviStatusBarApplet.cpp
//   Creation date : Tue 07 Sep 2004 03:56:46 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004 Szymon Stefanek <pragma at kvirc dot net>
//   Copyright (C) 2008 Elvio Basello <hellvis69 at netsons dor org>
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

#include "KviStatusBarApplet.h"
#include "KviMainWindow.h"
#include "KviIconManager.h"
#include "KviWindow.h"
#include "KviIrcContext.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviIrcConnectionStatistics.h"
#include "KviLocale.h"
#include "KviApplication.h"
#include "KviConfigurationFile.h"
#include "KviModuleManager.h"
#include "KviConsoleWindow.h"
#include "KviLagMeter.h"
#include "KviOptions.h"
#include "KviKvsScript.h"
#include "KviTimeUtils.h"
#include "KviQString.h"
#include "KviBuildInfo.h"
#include "kvi_settings.h"
#include "KviMiscUtils.h"
#include "KviHttpRequest.h"
#include "KviUrl.h"
#include "KviTalToolTip.h"
#include "KviDataBuffer.h"

#include <QPainter>
#include <QStyle>
#include <QLayout>
#include <QTimer>
#include <QCursor>
#include <QPixmap>
#include <QFont>
#include <QEvent>
#include <QMouseEvent>
#include <QMenu>
#include <QDateTime>

KviStatusBarAppletDescriptor::KviStatusBarAppletDescriptor(const QString & szVisibleName, const QString & szInternalName, CreateAppletCallback pProc, const QString & szPreloadModule, const QPixmap & pixIcon)
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
	if(!pixIcon.isNull())
		m_pIcon = new QPixmap(pixIcon);
	else
		m_pIcon = nullptr;
}

KviStatusBarAppletDescriptor::~KviStatusBarAppletDescriptor()
{
	while(KviStatusBarApplet * a = m_pAppletList->first())
		delete a;
	delete m_pAppletList;
	if(m_pIcon)
		delete m_pIcon;
}

KviStatusBarApplet * KviStatusBarAppletDescriptor::create(KviStatusBar * pBar)
{
	return m_pProc(pBar, this);
}

void KviStatusBarAppletDescriptor::registerApplet(KviStatusBarApplet * a)
{
	m_pAppletList->append(a);
}

void KviStatusBarAppletDescriptor::unregisterApplet(KviStatusBarApplet * a)
{
	m_pAppletList->removeRef(a);
}

KviStatusBarApplet::KviStatusBarApplet(KviStatusBar * pParent, KviStatusBarAppletDescriptor * pDescriptor)
    : QLabel(pParent), m_pStatusBar(pParent), m_pDescriptor(pDescriptor)
{
	setAutoFillBackground(false);
	m_pDescriptor->registerApplet(this);
	m_pStatusBar->registerApplet(this);
}

KviStatusBarApplet::~KviStatusBarApplet()
{
	m_pDescriptor->unregisterApplet(this);
	m_pStatusBar->unregisterApplet(this);
}

QString KviStatusBarApplet::tipText(const QPoint &)
{
	return QString();
}

// Away applet
KviStatusBarAwayIndicator::KviStatusBarAwayIndicator(KviStatusBar * pParent, KviStatusBarAppletDescriptor * pDescriptor)
    : KviStatusBarApplet(pParent, pDescriptor)
{
	m_bAwayOnAllContexts = false;
	connect(pParent->frame(), SIGNAL(activeContextChanged()), this, SLOT(updateDisplay()));
	connect(pParent->frame(), SIGNAL(activeContextStateChanged()), this, SLOT(updateDisplay()));
	connect(pParent->frame(), SIGNAL(activeConnectionAwayStateChanged()), this, SLOT(updateDisplay()));

	updateDisplay();

	if(!pixmap())
		setPixmap(*(g_pIconManager->getSmallIcon(KviIconManager::NotAway)));
}

KviStatusBarAwayIndicator::~KviStatusBarAwayIndicator()
    = default;

void KviStatusBarAwayIndicator::updateDisplay()
{
	KviIrcContext * c = statusBar()->frame()->activeContext();

	if(c && c->isConnected())
	{
		setPixmap(c->connection()->userInfo()->isAway() ? *(g_pIconManager->getSmallIcon(KviIconManager::Away)) : *(g_pIconManager->getSmallIcon(KviIconManager::NotAway)));
	}
	else
	{
		// FIXME: We'd like to appear disabled here... but then we
		//        no longer get mouse events :/
		setPixmap(*(g_pIconManager->getSmallIcon(KviIconManager::NotAway)));
	}
}

void KviStatusBarAwayIndicator::toggleContext()
{
	m_bAwayOnAllContexts = !m_bAwayOnAllContexts;
}

void KviStatusBarAwayIndicator::fillContextPopup(QMenu * p)
{
	QAction * pAction = p->addAction(__tr2qs("Apply to all IRC contexts"), this, SLOT(toggleContext()));
	pAction->setCheckable(true),
	    pAction->setChecked(m_bAwayOnAllContexts);
}

void KviStatusBarAwayIndicator::loadState(const char * pcPrefix, KviConfigurationFile * pCfg)
{
	KviCString tmp(KviCString::Format, "%s_AwayOnAllContexts", pcPrefix);
	m_bAwayOnAllContexts = pCfg->readBoolEntry(tmp.ptr(), false);
}

void KviStatusBarAwayIndicator::saveState(const char * pcPrefix, KviConfigurationFile * pCfg)
{
	KviCString tmp(KviCString::Format, "%s_AwayOnAllContexts", pcPrefix);
	pCfg->writeEntry(tmp.ptr(), m_bAwayOnAllContexts);
}

KviStatusBarApplet * CreateStatusBarAwayIndicator(KviStatusBar * pBar, KviStatusBarAppletDescriptor * pDescriptor)
{
	KviStatusBarApplet * pApplet = new KviStatusBarAwayIndicator(pBar, pDescriptor);
	pApplet->setIndex(pBar->insertPermanentWidgetAtTheEnd(pApplet));

	return pApplet;
}

void KviStatusBarAwayIndicator::selfRegister(KviStatusBar * pBar)
{
	KviStatusBarAppletDescriptor * d = new KviStatusBarAppletDescriptor(
	    __tr2qs("Away Indicator"), "awayindicator", CreateStatusBarAwayIndicator, "", *(g_pIconManager->getSmallIcon(KviIconManager::Away)));
	pBar->registerAppletDescriptor(d);
}

// FIXME: Away on all context should know where user is not away/back before toggling status
void KviStatusBarAwayIndicator::mouseDoubleClickEvent(QMouseEvent * e)
{
	if(!(e->button() & Qt::LeftButton))
		return;
	KviIrcConnection * c = statusBar()->frame()->activeConnection();
	if(!c)
		return;
	if(c->state() != KviIrcConnection::Connected)
		return;
	QString command;
	if(m_bAwayOnAllContexts)
		command = "if($away)back -a; else away -a -d";
	else
		command = "if($away)back; else away -d";
	KviKvsScript::run(command, c->console());
}

QString KviStatusBarAwayIndicator::tipText(const QPoint &)
{
	KviIrcConnection * c = statusBar()->frame()->activeConnection();
	QString szRet = "<b>";

	if(!c)
		goto not_connected;
	if(c->state() != KviIrcConnection::Connected)
		goto not_connected;
	if(c->userInfo()->isAway())
	{
		QString szTmp = KviTimeUtils::formatTimeInterval(kvi_unixTime() - c->userInfo()->awayTime(), KviTimeUtils::NoLeadingEmptyIntervals);

		szRet += __tr2qs("Away since");
		szRet += ": ";
		szRet += szTmp;
		szRet += "</b><br><br>";
		szRet += __tr2qs("Double-click to leave away mode");
	}
	else
	{
		szRet += __tr2qs("Not away");
		szRet += "</b><br><br>";
		szRet += __tr2qs("Double-click to enter away mode");
	}
	return szRet;

not_connected:
	szRet += __tr2qs("Not connected");
	szRet += "</b>";
	return szRet;
}

// Lag indicator applet
KviStatusBarLagIndicator::KviStatusBarLagIndicator(KviStatusBar * pParent, KviStatusBarAppletDescriptor * pDescriptor)
    : KviStatusBarApplet(pParent, pDescriptor)
{
	connect(pParent->frame(), SIGNAL(activeContextChanged()), this, SLOT(updateDisplay()));
	connect(pParent->frame(), SIGNAL(activeContextStateChanged()), this, SLOT(updateDisplay()));
	connect(pParent->frame(), SIGNAL(activeConnectionLagChanged()), this, SLOT(updateDisplay()));

	updateDisplay();
}

void KviStatusBarLagIndicator::mouseDoubleClickEvent(QMouseEvent * e)
{
	if(!(e->button() & Qt::LeftButton))
		return;

	KviIrcConnection * c = statusBar()->frame()->activeConnection();

	if(!c)
		return;
	if(c->state() != KviIrcConnection::Connected)
		return;
	if(!c->lagMeter())
	{
		KVI_OPTION_BOOL(KviOption_boolUseLagMeterEngine) = true;
		g_pApp->restartLagMeters();
	}
}

QString KviStatusBarLagIndicator::tipText(const QPoint &)
{
	KviIrcConnection * c = statusBar()->frame()->activeConnection();
	QString szRet;

	if(!c)
		goto not_connected;
	if(c->state() != KviIrcConnection::Connected)
		goto not_connected;
	if(c->lagMeter())
	{
		int lll = c->lagMeter()->lag();
		if(lll > 0)
		{
			KviQString::appendFormatted(szRet, __tr2qs("Lag: <b>%d ms"), lll);
			szRet += "</b><br>";
			int vss = c->lagMeter()->secondsSinceLastCompleted();
			int vmm = vss / 60;
			vss = vss % 60;
			KviQString::appendFormatted(szRet, __tr2qs("Last checked: <b>%d mins %d secs ago</b>"), vmm, vss);
		}
		else
		{
			szRet += "<b>";
			szRet += __tr2qs("Lag measure not available yet");
			szRet += "</b>";
		}
	}
	else
	{
		szRet += "<b>";
		szRet += __tr2qs("Lag meter engine disabled");
		szRet += "</b><br>";
		szRet += __tr2qs("Double-click to enable it");
	}
	return szRet;

not_connected:
	szRet += "<b>";
	szRet += __tr2qs("Not connected");
	szRet += "</b>";
	return szRet;
}

void KviStatusBarLagIndicator::updateDisplay()
{
	KviIrcContext * c = statusBar()->frame()->activeContext();

	if(c && c->isConnected())
	{
		KviIrcConnection * ic = c->connection();
		if(ic->lagMeter())
		{
			int lll = ic->lagMeter()->lag();
			if(lll > 0)
			{
				QString szTmp = QString(__tr2qs("Lag: %1 ms")).arg(lll);
				if(lll > 60000)
				{
					// one minute lag!
					// paint it in red
					szTmp.prepend("<font color=\"#e00000\">");
					szTmp.append("</font>");
				}
				setText(szTmp);
				return;
			}
		}
	}
	// no lag available
	setText(__tr2qs("Lag: ???"));
}

KviStatusBarApplet * CreateStatusBarLagIndicator(KviStatusBar * pBar, KviStatusBarAppletDescriptor * pDescriptor)
{
	KviStatusBarApplet * pApplet = new KviStatusBarLagIndicator(pBar, pDescriptor);
	pApplet->setIndex(pBar->insertPermanentWidgetAtTheEnd(pApplet));
	return pApplet;
}

void KviStatusBarLagIndicator::selfRegister(KviStatusBar * pBar)
{
	KviStatusBarAppletDescriptor * d = new KviStatusBarAppletDescriptor(
	    __tr2qs("Lag Indicator"), "lagindicator", CreateStatusBarLagIndicator, "", *(g_pIconManager->getSmallIcon(KviIconManager::ServerPing)));
	pBar->registerAppletDescriptor(d);
}

// Clock applet
KviStatusBarClock::KviStatusBarClock(KviStatusBar * pParent, KviStatusBarAppletDescriptor * pDescriptor)
    : KviStatusBarApplet(pParent, pDescriptor)
{
	m_bUtc = false;
	m_b24h = true;
	m_iType = KviStatusBarClock::HMS;

	adjustMinWidth();
	startTimer(1000);
}

KviStatusBarClock::~KviStatusBarClock()
    = default;

void KviStatusBarClock::adjustMinWidth()
{
	QFontMetrics fm(font());
	if(m_b24h)
	{
		if(m_iType == KviStatusBarClock::HMS)
			setFixedWidth(fm.width("00:00:00"));
		else
			setFixedWidth(fm.width("00:00"));
	}
	else
	{
		if(m_iType == KviStatusBarClock::HMS)
			setFixedWidth(fm.width("00:00:00 AM"));
		else
			setFixedWidth(fm.width("00:00 AM"));
	}
}

void KviStatusBarClock::timerEvent(QTimerEvent *)
{
	QDateTime time = m_bUtc ? QDateTime::currentDateTimeUtc() : QDateTime::currentDateTime();
	QString szTmp;

	switch(m_iType)
	{
		case KviStatusBarClock::HMS:
			szTmp = m_b24h ? time.toString("HH:mm:ss") : time.toString("hh:mm:ss AP");
			break;
		case KviStatusBarClock::HM:
			szTmp = m_b24h ? time.toString("HH:mm") : time.toString("hh:mm AP");
			break;
	}

	setText(szTmp);
}

void KviStatusBarClock::fillContextPopup(QMenu * p)
{
	QAction * pAction = p->addAction("UTC", this, SLOT(toggleUtc()));
	pAction->setCheckable(true);
	pAction->setChecked(m_bUtc);

	pAction = p->addAction("24h", this, SLOT(toggle24h()));
	pAction->setCheckable(true);
	pAction->setChecked(m_b24h);
	p->addSeparator();

	// Format menu
	QMenu * pMenu = new QMenu(p);
	pAction = pMenu->addAction("hh:mm:ss");
	pAction->setData(QVariant(KviStatusBarClock::HMS));

	pAction = pMenu->addAction("hh:mm");
	pAction->setData(QVariant(KviStatusBarClock::HM));
	connect(pMenu, SIGNAL(triggered(QAction *)), this, SLOT(changeFormat(QAction *)));

	pAction = p->addAction(__tr2qs("Format"));
	pAction->setMenu(pMenu);
	pAction->setEnabled(true);
}

void KviStatusBarClock::toggleUtc()
{
	m_bUtc = !m_bUtc;
	timerEvent(nullptr);
}

void KviStatusBarClock::toggle24h()
{
	m_b24h = !m_b24h;
	adjustMinWidth();
	timerEvent(nullptr);
}

void KviStatusBarClock::changeFormat(QAction * pAct)
{
	bool bOk;
	m_iType = pAct->data().toInt(&bOk);
	adjustMinWidth();
	if(!bOk)
		return;
}

void KviStatusBarClock::loadState(const char * pcPrefix, KviConfigurationFile * pCfg)
{
	KviCString tmp(KviCString::Format, "%s_Utc", pcPrefix);
	m_bUtc = pCfg->readBoolEntry(tmp.ptr(), false);
	KviCString tmp2(KviCString::Format, "%s_24h", pcPrefix);
	m_b24h = pCfg->readBoolEntry(tmp2.ptr(), false);
	KviCString tmp3(KviCString::Format, "%s_Format", pcPrefix);
	m_iType = pCfg->readIntEntry(tmp3.ptr(), KviStatusBarClock::HMS);
	adjustMinWidth();
}

void KviStatusBarClock::saveState(const char * pcPrefix, KviConfigurationFile * pCfg)
{
	KviCString tmp(KviCString::Format, "%s_Utc", pcPrefix);
	pCfg->writeEntry(tmp.ptr(), m_bUtc);
	KviCString tmp2(KviCString::Format, "%s_24h", pcPrefix);
	pCfg->writeEntry(tmp2.ptr(), m_b24h);
	KviCString tmp3(KviCString::Format, "%s_Format", pcPrefix);
	pCfg->writeEntry(tmp3.ptr(), m_iType);
}

KviStatusBarApplet * CreateStatusBarClock(KviStatusBar * pBar, KviStatusBarAppletDescriptor * pDescriptor)
{
	KviStatusBarApplet * pApplet = new KviStatusBarClock(pBar, pDescriptor);
	pApplet->setIndex(pBar->insertPermanentWidgetAtTheEnd(pApplet));
	return pApplet;
}

void KviStatusBarClock::selfRegister(KviStatusBar * pBar)
{
	KviStatusBarAppletDescriptor * d = new KviStatusBarAppletDescriptor(
	    __tr2qs("Simple Clock"), "clock", CreateStatusBarClock, "", *(g_pIconManager->getSmallIcon(KviIconManager::Time)));
	pBar->registerAppletDescriptor(d);
}

// Connection timer applet
KviStatusBarConnectionTimer::KviStatusBarConnectionTimer(KviStatusBar * pParent, KviStatusBarAppletDescriptor * pDescriptor)
    : KviStatusBarApplet(pParent, pDescriptor)
{
	startTimer(1000);
	m_bTotal = false;

	QFontMetrics fm(font());
	setFixedWidth(fm.width("000 d 00 h 00 m 00 s"));
}

KviStatusBarConnectionTimer::~KviStatusBarConnectionTimer()
    = default;
//g_pApp->topmostConnectedConsole()
void KviStatusBarConnectionTimer::timerEvent(QTimerEvent *)
{
	if(m_bTotal)
	{
		setText(KviTimeUtils::formatTimeInterval(KVI_OPTION_UINT(KviOption_uintTotalConnectionTime)));
	}
	else
	{
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

		setText(KviTimeUtils::formatTimeInterval(0, KviTimeUtils::FillWithHypens));
	}
	return;
}

void KviStatusBarConnectionTimer::toggleTotal()
{
	m_bTotal = !m_bTotal;
}

void KviStatusBarConnectionTimer::fillContextPopup(QMenu * p)
{
	QAction * pAction = p->addAction(__tr2qs("Show total connection time"), this, SLOT(toggleTotal()));
	pAction->setCheckable(true);
	pAction->setChecked(m_bTotal);
}

void KviStatusBarConnectionTimer::loadState(const char * pcPrefix, KviConfigurationFile * pCfg)
{
	KviCString tmp(KviCString::Format, "%s_Total", pcPrefix);
	m_bTotal = pCfg->readBoolEntry(tmp.ptr(), false);
}

void KviStatusBarConnectionTimer::saveState(const char * pcPrefix, KviConfigurationFile * pCfg)
{
	KviCString tmp(KviCString::Format, "%s_Total", pcPrefix);
	pCfg->writeEntry(tmp.ptr(), m_bTotal);
}

KviStatusBarApplet * CreateStatusBarConnectionTimer(KviStatusBar * pBar, KviStatusBarAppletDescriptor * pDescriptor)
{
	KviStatusBarApplet * pApplet = new KviStatusBarConnectionTimer(pBar, pDescriptor);
	pApplet->setIndex(pBar->insertPermanentWidgetAtTheEnd(pApplet));
	return pApplet;
}

void KviStatusBarConnectionTimer::selfRegister(KviStatusBar * pBar)
{
	KviStatusBarAppletDescriptor * d = new KviStatusBarAppletDescriptor(
	    __tr2qs("Connection Timer"), "connectiontimer", CreateStatusBarConnectionTimer, "", *(g_pIconManager->getSmallIcon(KviIconManager::Time)));
	pBar->registerAppletDescriptor(d);
}

// Separator applet
KviStatusBarSeparator::KviStatusBarSeparator(KviStatusBar * pParent, KviStatusBarAppletDescriptor * pDescriptor)
    : KviStatusBarApplet(pParent, pDescriptor)
{
	setFrameStyle(QFrame::VLine | QFrame::Sunken);
}

KviStatusBarSeparator::~KviStatusBarSeparator()
    = default;

KviStatusBarApplet * CreateStatusBarSeparator(KviStatusBar * pBar, KviStatusBarAppletDescriptor * pDescriptor)
{
	KviStatusBarApplet * pApplet = new KviStatusBarSeparator(pBar, pDescriptor);
	pApplet->setIndex(pBar->insertPermanentWidgetAtTheEnd(pApplet));
	return pApplet;
}

void KviStatusBarSeparator::selfRegister(KviStatusBar * pBar)
{
	KviStatusBarAppletDescriptor * d = new KviStatusBarAppletDescriptor(
	    __tr2qs("Separator"), "separator", CreateStatusBarSeparator);
	pBar->registerAppletDescriptor(d);
}

// Update applet
KviStatusBarUpdateIndicator::KviStatusBarUpdateIndicator(KviStatusBar * pParent, KviStatusBarAppletDescriptor * pDescriptor)
    : KviStatusBarApplet(pParent, pDescriptor)
{
	m_bCheckDone = false;
	m_bCheckFailed = false;
	m_bUpdateStatus = false;
	m_bUpdateOnStartup = false;
	m_bUpdateRevision = false;
	m_pHttpRequest = nullptr;

	updateDisplay();

	if(!pixmap())
		setPixmap(*(g_pIconManager->getSmallIcon(KviIconManager::NotUpdate)));
}

KviStatusBarUpdateIndicator::~KviStatusBarUpdateIndicator()
    = default;

void KviStatusBarUpdateIndicator::updateDisplay()
{
	if(m_bCheckFailed)
		setPixmap(*(g_pIconManager->getSmallIcon(KviIconManager::FailUpdate)));
	else
		setPixmap(m_bUpdateStatus ? *(g_pIconManager->getSmallIcon(KviIconManager::Update)) : *(g_pIconManager->getSmallIcon(KviIconManager::NotUpdate)));
}

void KviStatusBarUpdateIndicator::toggleStartup()
{
	m_bUpdateOnStartup = !m_bUpdateOnStartup;
}

void KviStatusBarUpdateIndicator::toggleRevision()
{
	m_bUpdateRevision = !m_bUpdateRevision;
	m_bUpdateStatus = false;
	updateDisplay();
}

void KviStatusBarUpdateIndicator::fillContextPopup(QMenu * p)
{
	QAction * pAction = p->addAction(__tr2qs("Check on startup"), this, SLOT(toggleStartup()));
	pAction->setCheckable(true);
	pAction->setChecked(m_bUpdateOnStartup);

	pAction = p->addAction(__tr2qs("Check Git revisions"), this, SLOT(toggleRevision()));
	pAction->setCheckable(true);
	pAction->setChecked(m_bUpdateRevision);
}

void KviStatusBarUpdateIndicator::loadState(const char * pcPrefix, KviConfigurationFile * pCfg)
{
	KviCString tmp(KviCString::Format, "%s_UpdateOnStartup", pcPrefix);
	m_bUpdateOnStartup = pCfg->readBoolEntry(tmp.ptr(), false);
	KviCString tmp2(KviCString::Format, "%s_UpdateRevision", pcPrefix);
	m_bUpdateRevision = pCfg->readBoolEntry(tmp2.ptr(), false);

	if(m_bUpdateOnStartup)
		checkVersion();
}

void KviStatusBarUpdateIndicator::saveState(const char * pcPrefix, KviConfigurationFile * pCfg)
{
	KviCString tmp(KviCString::Format, "%s_UpdateOnStartup", pcPrefix);
	pCfg->writeEntry(tmp.ptr(), m_bUpdateOnStartup);
	KviCString tmp2(KviCString::Format, "%s_UpdateRevision", pcPrefix);
	pCfg->writeEntry(tmp2.ptr(), m_bUpdateRevision);
}

KviStatusBarApplet * CreateStatusBarUpdateIndicator(KviStatusBar * pBar, KviStatusBarAppletDescriptor * pDescriptor)
{
	KviStatusBarApplet * pApplet = new KviStatusBarUpdateIndicator(pBar, pDescriptor);
	pApplet->setIndex(pBar->insertPermanentWidgetAtTheEnd(pApplet));
	return pApplet;
}

void KviStatusBarUpdateIndicator::selfRegister(KviStatusBar * pBar)
{
	KviStatusBarAppletDescriptor * d = new KviStatusBarAppletDescriptor(
	    __tr2qs("Update Indicator"), "updateindicator", CreateStatusBarUpdateIndicator, "", *(g_pIconManager->getSmallIcon(KviIconManager::Update)));
	pBar->registerAppletDescriptor(d);
}

void KviStatusBarUpdateIndicator::checkVersion()
{
	m_bCheckDone = true;
	QString szUrl, szFileName;

	if(m_bUpdateRevision)
		szUrl = "http://kvirc.net/checkversion.php?git=1";
	else
		szUrl = "http://kvirc.net/checkversion.php";

	KviUrl url(szUrl);

	m_pHttpRequest = new KviHttpRequest();
	connect(m_pHttpRequest, SIGNAL(receivedResponse(const QString &)), this, SLOT(responseReceived(const QString &)));
	connect(m_pHttpRequest, SIGNAL(binaryData(const KviDataBuffer &)), this, SLOT(binaryDataReceived(const KviDataBuffer &)));
	connect(m_pHttpRequest, SIGNAL(terminated(bool)), this, SLOT(requestCompleted(bool)));

	m_pHttpRequest->get(url, KviHttpRequest::WholeFile, szFileName);
}

void KviStatusBarUpdateIndicator::mouseDoubleClickEvent(QMouseEvent * e)
{
	if(!(e->button() & Qt::LeftButton))
		return;

	if(m_bUpdateStatus)
		getNewVersion();
	else
		checkVersion();
}

void KviStatusBarUpdateIndicator::responseReceived(const QString & szResponse)
{
	// Server response
	if(szResponse != "HTTP/1.1 200 OK")
	{
		m_szHttpResponse = szResponse;
		m_bCheckFailed = true;
		updateDisplay();
	}
}

void KviStatusBarUpdateIndicator::binaryDataReceived(const KviDataBuffer & buffer)
{
	// Got data
	if(!buffer.data() || buffer.size() <= 0)
		return;

	KviCString szData((const char *)buffer.data(), buffer.size());
	bool bRemoteNew = false;

	if(m_bUpdateRevision)
	{
		if(szData.toUInt() > KviBuildInfo::buildRevision().toUInt())
			bRemoteNew = true;
	}
	else
	{
		if(KviMiscUtils::compareVersions(szData.ptr(), KVI_VERSION) < 0)
			bRemoteNew = true;
	}

	if(bRemoteNew)
	{
		m_szNewVersion = QString(szData.ptr());
		m_bUpdateStatus = true;
		updateDisplay();

		if(!m_bUpdateOnStartup)
		{
			getNewVersion();
		}
	}
}

void KviStatusBarUpdateIndicator::requestCompleted(bool)
{
	// Data transfer terminated
	delete m_pHttpRequest;
}

void KviStatusBarUpdateIndicator::getNewVersion()
{
	// Set build platform
	QString szUrl;

	if(m_bUpdateRevision)
	{
		szUrl = "https://github.com/kvirc/KVIrc/releases/";
		szUrl += m_szNewVersion;
	}
	else
	{
		QString szSystem = KviBuildInfo::buildSystemName();

		if(szSystem == "Windows")
			szSystem = "win32";
		else if(szSystem == "Darwin")
			szSystem = "macosx";
		else
			szSystem = "unix";

		// Create page to link to
		szUrl = "http://kvirc.net/?id=releases&platform=";
		szUrl += szSystem;
		szUrl += "&version=";
		szUrl += m_szNewVersion;
	}

	// Create command to run
	QString szCommand = "openurl ";
	szCommand += szUrl;

	// Open the Git or the download page for the platform we're using
	KviKvsScript::run(szCommand, g_pActiveWindow);
}

QString KviStatusBarUpdateIndicator::tipText(const QPoint &)
{
	QString szRet = "<b>";
	if(!m_bCheckDone)
	{
		szRet += __tr2qs("Update missing");
		szRet += "</b><br>";
		szRet += __tr2qs("You haven't check yet. Should I check for updates?");
		szRet += "<br><br>";
		szRet += __tr2qs("Double-click to check now");
	}
	else if(m_bCheckFailed)
	{
		szRet += __tr2qs("Update failed");
		szRet += "</b><br>";
		szRet += __tr2qs("The remote server replied with ");
		szRet += m_szHttpResponse;
		szRet += "<br><br>";
		szRet += __tr2qs("Double-click to retry");
	}
	else if(!m_bUpdateStatus)
	{
		szRet += __tr2qs("No updates found");
		szRet += "</b><br><br>";
		szRet += __tr2qs("Double-click to retry");
	}
	else if(m_bUpdateStatus)
	{
		szRet += __tr2qs("New updates found");
		szRet += "</b><br><br>";
		szRet += __tr2qs("Double-click to get the latest version");
	}
	return szRet;
}
