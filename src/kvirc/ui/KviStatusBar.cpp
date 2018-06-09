//=============================================================================
//
//   File : KviStatusBar.cpp
//   Creation date : Tue 07 Sep 2004 03:56:46 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
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

#include "KviStatusBar.h"
#include "KviApplication.h"
#include "KviConfigurationFile.h"
#include "KviDynamicToolTip.h"
#include "KviIconManager.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviIrcContext.h"
#include "KviLocale.h"
#include "KviMainWindow.h"
#include "KviModuleManager.h"
#include "KviStatusBarApplet.h"
#include "KviTimeUtils.h"
#include "KviWindow.h"

#include <QCursor>
#include <QDrag>
#include <QEvent>
#include <QFont>
#include <QHelpEvent>
#include <QLabel>
#include <QLayout>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QStyle>
#include <QTimer>
#include <QWidgetAction>

KviStatusBar::KviStatusBar(KviMainWindow * pFrame)
    : QStatusBar(pFrame)
{
	setAutoFillBackground(false);

	setProperty("name", "statusbar");
	m_pFrame = pFrame;
	// ugh :D
	setSizeGripEnabled(false);
	setAcceptDrops(true);

	m_pContextPopup = nullptr;
	m_pAppletsPopup = nullptr;
	m_pClickedApplet = nullptr;

	m_pAppletDescriptors = new KviPointerHashTable<QString, KviStatusBarAppletDescriptor>;
	m_pAppletDescriptors->setAutoDelete(true);

	KviStatusBarClock::selfRegister(this);
	KviStatusBarAwayIndicator::selfRegister(this);
	KviStatusBarLagIndicator::selfRegister(this);
	KviStatusBarConnectionTimer::selfRegister(this);
	KviStatusBarUpdateIndicator::selfRegister(this);
	KviStatusBarSeparator::selfRegister(this);

	m_pAppletList = new KviPointerList<KviStatusBarApplet>;
	m_pAppletList->setAutoDelete(false);

	m_pMessageQueue = new KviPointerList<KviStatusBarMessage>;
	m_pMessageQueue->setAutoDelete(true);

	m_pMessageTimer = nullptr;

	m_pMessageLabel = new QLabel("<b>[x]</b> x", this);
	m_pMessageLabel->setObjectName("msgstatuslabel");
	m_pMessageLabel->setMargin(1);
	insertWidget(0, m_pMessageLabel);
	m_iLastMinimumHeight = 0;
	m_bStopLayoutOnAddRemove = true;
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
	    this, SLOT(contextMenuRequested(const QPoint &)));

	connect(m_pFrame, SIGNAL(activeContextChanged()), this, SLOT(setPermanentMessage()));
	connect(m_pFrame, SIGNAL(activeContextStateChanged()), this, SLOT(setPermanentMessage()));
	connect(m_pFrame, SIGNAL(activeConnectionUserModeChanged()), this, SLOT(setPermanentMessage()));
	connect(m_pFrame, SIGNAL(activeConnectionNickNameChanged()), this, SLOT(setPermanentMessage()));
	setPermanentMessage();

	m_bStopLayoutOnAddRemove = false;
}

void KviStatusBar::dropEvent(QDropEvent * de)
{
	de->accept();
	m_pClickedApplet = nullptr;
}

void KviStatusBar::dragEnterEvent(QDragEnterEvent * event)
{
	event->acceptProposedAction();
}

void KviStatusBar::dragMoveEvent(QDragMoveEvent * de)
{
	// Unpack dropped data and handle it the way you want
	if(de->mimeData()->hasFormat("kvirc/statusbarapplet") && de->source() != nullptr && m_pClickedApplet && appletExists(m_pClickedApplet))
	{
		KviStatusBarApplet * pApplet = m_pAppletList->last();
		int oldIndex = m_pClickedApplet->index();
		int newIndex = pApplet->index();

		// move!
		while(pApplet)
		{
			if(de->pos().x() < (pApplet->x() + pApplet->width() - m_pClickedApplet->width()))
			{
				pApplet = m_pAppletList->prev();
			}
			else
			{
				newIndex = pApplet->index();
				break;
			}
		}
		if(!pApplet)
		{
			pApplet = m_pAppletList->first();
			newIndex = pApplet->index();
		}

		//swap indexes
		if(newIndex != oldIndex)
		{
			m_pClickedApplet->setIndex(newIndex);
			pApplet->setIndex(oldIndex);

			removeWidget(m_pClickedApplet); //Note: This function does not delete the widget but hides it. To add the widget again, you must call both the addWidget() and show() functions.
			insertPermanentWidget(newIndex, m_pClickedApplet);
			m_pClickedApplet->show();

			m_pAppletList->sort();
		}
	}
}

KviStatusBar::~KviStatusBar()
{
	save();

	m_bStopLayoutOnAddRemove = true;

	if(m_pMessageTimer)
		delete m_pMessageTimer;
	delete m_pMessageQueue;
	delete m_pAppletDescriptors;
	delete m_pAppletList;
	delete m_pMessageLabel;
}

void KviStatusBar::load()
{
	QString szBuf;
	if(!g_pApp->getReadOnlyConfigPath(szBuf, "statusbar.kvc"))
		return; // no config file at all

	KviConfigurationFile cfg(szBuf, KviConfigurationFile::Read);
	cfg.setGroup("Applets");

	int iApplets = cfg.readIntEntry("Count", 0);
	for(int i = 0; i < iApplets; i++)
	{
		KviCString prefix(KviCString::Format, "Applet%d", i);
		KviCString tmp(KviCString::Format, "%s_InternalName", prefix.ptr());
		QString szInternalName = cfg.readEntry(tmp.ptr(), "");
		// 		qDebug("load applet %d %s",i,szInternalName.toUtf8().data());
		if(!szInternalName.isEmpty())
		{
			tmp.sprintf("%s_PreloadModule", prefix.ptr());
			QString szPreloadModule = cfg.readEntry(tmp.ptr(), "");
			if(!szPreloadModule.isEmpty())
				g_pModuleManager->getModule(szPreloadModule.toUtf8().data());

			KviStatusBarApplet * pApplet = createApplet(szInternalName);
			if(pApplet)
				pApplet->loadState(prefix.ptr(), &cfg);
			else
				qDebug("WARNING: failed to create applet %s (preload: %s)!",
				    szInternalName.toUtf8().data(), szPreloadModule.toUtf8().data());
		}
	}
}

void KviStatusBar::save()
{
	QString szBuf;
	g_pApp->getLocalKvircDirectory(szBuf, KviApplication::Config, "statusbar.kvc");

	KviConfigurationFile cfg(szBuf, KviConfigurationFile::Write);
	cfg.setGroup("Applets");

	cfg.writeEntry("Count", m_pAppletList->count());

	//ensure the applets are correctly ordered
	m_pAppletList->sort();

	int i = 0;
	for(KviStatusBarApplet * pApplet = m_pAppletList->first(); pApplet; pApplet = m_pAppletList->next())
	{
		// 		qDebug("save applet %d %s index %d",i,pApplet->descriptor()->internalName().toUtf8().data(),pApplet->index());
		KviCString prefix(KviCString::Format, "Applet%d", i);
		KviCString tmp(KviCString::Format, "%s_InternalName", prefix.ptr());
		cfg.writeEntry(tmp.ptr(), pApplet->descriptor()->internalName());
		pApplet->saveState(prefix.ptr(), &cfg);
		if(!(pApplet->descriptor()->preloadModule().isEmpty()))
		{
			tmp.sprintf("%s_PreloadModule", prefix.ptr());
			cfg.writeEntry(tmp.ptr(), pApplet->descriptor()->preloadModule());
		}
		i++;
	}
}

bool KviStatusBar::event(QEvent * e)
{
	if(e->type() == QEvent::ToolTip)
	{
		QHelpEvent * pHelpEvent = (QHelpEvent *)e;
		tipRequest(pHelpEvent);
	}
	return QStatusBar::event(e);
}

void KviStatusBar::recalcMinimumHeight()
{
	int iSize = 18;
	int iHeight = m_pMessageLabel->sizeHint().height();
	if(iHeight > iSize)
		iSize = iHeight;

	for(KviStatusBarApplet * pApplet = m_pAppletList->last(); pApplet; pApplet = m_pAppletList->prev())
	{
		iHeight = pApplet->sizeHint().height();
		if(iHeight > iSize)
			iSize = iHeight;
	}

	iSize += (VMARGIN * 2) + RICHTEXTLABELTRICK;
	if(m_iLastMinimumHeight != iSize)
	{
		m_iLastMinimumHeight = iSize;
		setMinimumHeight(iSize);
		QLayout * l = layout();
		if(l)
		{
			if(l->inherits("QBoxLayout"))
				((QBoxLayout *)l)->addStrut(iSize);
		}
	}
}

bool KviStatusBar::appletExists(KviStatusBarApplet * pApplet)
{
	return (m_pAppletList->findRef(pApplet) != -1);
}

KviStatusBarApplet * KviStatusBar::appletAt(const QPoint & pnt, bool bBestMatch)
{
	QPoint local = mapFromGlobal(pnt);
	if(bBestMatch)
	{
		for(KviStatusBarApplet * pApplet = m_pAppletList->first(); pApplet; pApplet = m_pAppletList->next())
		{
			if(local.x() <= (pApplet->x() + pApplet->width()))
				return pApplet;
		}
		return m_pAppletList->last(); // last anyway
	}

	for(KviStatusBarApplet * pApplet = m_pAppletList->first(); pApplet; pApplet = m_pAppletList->next())
	{
		if((local.x() >= pApplet->x()) && (local.y() >= pApplet->y()))
		{
			if((local.x() <= (pApplet->x() + pApplet->width())) && (local.y() <= (pApplet->y() + pApplet->height())))
			{
				return pApplet;
			}
		}
	}
	return nullptr;
}

void KviStatusBar::tipRequest(QHelpEvent * e)
{
	KviStatusBarApplet * pApplet = appletAt(mapToGlobal(e->pos()));
	QString szTip;
	if(pApplet)
	{
		szTip = R"(<table style="white-space: pre"><tr><td bgcolor="#303030" align="center"><font color="#ffffff"><b>)" + pApplet->descriptor()->visibleName() + "</b></font></td></tr>";

		QString szTipx = pApplet->tipText(pApplet->mapFromGlobal(mapToGlobal(e->pos())));
		if(!szTipx.isEmpty())
		{
			szTip += "<tr><td>";
			szTip += szTipx;
			szTip += "</td></tr><tr><td align=\"center\"><hr></td></tr>";
		}

		szTip += R"(<tr><td><font color="#636363" size="-1">)";
		szTip += __tr2qs("<b>Shift+Drag</b> or <b>Ctrl+Drag</b> to move the applet around");
		szTip += "<br>";
		szTip += __tr2qs("Right-click to see the other options");
		szTip += "</font></td></tr></table>";
	}
	else
	{
		szTip = "<table style=\"white-space: pre\">";

		KviIrcContext * c = m_pFrame->activeContext();
		if(c && (c->state() == KviIrcContext::Connected))
		{
			szTip += "<tr><td>";
			szTip += __tr2qs("Double-click to get network information");
			szTip += "</td></tr>";
		}

		szTip += "<tr><td>";
		szTip += __tr2qs("Right-click to add / remove applets");
		szTip += "</td></tr></table>";
	}
	QToolTip::showText(e->globalPos(), szTip);
}

void KviStatusBar::contextMenuRequested(const QPoint & pos)
{
	if(!m_pContextPopup)
	{
		m_pContextPopup = new QMenu(this);
		connect(m_pContextPopup, SIGNAL(aboutToShow()), this, SLOT(contextPopupAboutToShow()));
	}

	m_pClickedApplet = appletAt(QCursor::pos());
	m_pContextPopup->popup(mapToGlobal(pos));
}

void KviStatusBar::contextPopupAboutToShow()
{
	if(!m_pContextPopup)
		return;

	m_pContextPopup->clear();

	if(appletExists(m_pClickedApplet))
	{
		QString szApp = m_pClickedApplet->descriptor()->visibleName();

		QString szTmp = QString("<center><b>%1</b></center>").arg(szApp);

		QLabel * pLabel = new QLabel(szTmp, m_pContextPopup);
		QPalette p;
		pLabel->setStyleSheet("background-color: " + p.color(QPalette::Normal, QPalette::Mid).name());

		QWidgetAction * pWaction = new QWidgetAction(m_pContextPopup);
		pWaction->setDefaultWidget(pLabel);
		m_pContextPopup->addAction(pWaction);

		m_pClickedApplet->fillContextPopup(m_pContextPopup);

		szTmp = QString(__tr2qs("Remove %1")).arg(szApp);
		m_pContextPopup->addSeparator();
		m_pContextPopup->addAction(szTmp, this, SLOT(removeClickedApplet()));
	}

	if(!m_pAppletsPopup)
	{
		m_pAppletsPopup = new QMenu(this);
		connect(m_pAppletsPopup, SIGNAL(aboutToShow()), this, SLOT(appletsPopupAboutToShow()));
		connect(m_pAppletsPopup, SIGNAL(triggered(QAction *)), this, SLOT(appletsPopupActivated(QAction *)));
	}

	QAction * pAction = m_pContextPopup->addAction(__tr2qs("Add Applet"));
	pAction->setMenu(m_pAppletsPopup);
}

void KviStatusBar::removeClickedApplet()
{
	if(!appletExists(m_pClickedApplet))
		return;

	delete m_pClickedApplet;
	m_pClickedApplet = nullptr;
}

void KviStatusBar::appletsPopupAboutToShow()
{
	if(!m_pAppletsPopup)
		return;

	m_pAppletsPopup->clear();

	// FIXME: could we cache the module results in some way ?
	g_pModuleManager->loadModulesByCaps("statusbarapplet");

	QAction * pAction = nullptr;
	KviPointerHashTableIterator<QString, KviStatusBarAppletDescriptor> it(*m_pAppletDescriptors);
	while(KviStatusBarAppletDescriptor * d = it.current())
	{
		QPixmap * pix = d->icon();
		if(pix)
			pAction = m_pAppletsPopup->addAction(*pix, d->visibleName());
		else
			pAction = m_pAppletsPopup->addAction(d->visibleName());
		pAction->setData(d->id());
		++it;
	}
}

KviStatusBarApplet * KviStatusBar::createApplet(const QString & szInternalName)
{
	KviStatusBarAppletDescriptor * d = m_pAppletDescriptors->find(szInternalName);
	if(!d)
		return nullptr;

	return d->create(this);
}

void KviStatusBar::showLayoutHelp()
{
	queueMessage(new KviStatusBarMessage(__tr2qs("Drag the applet while holding the Shift or Ctrl key to move it to the desired position")));
}

void KviStatusBar::appletsPopupActivated(QAction * pAction)
{
	// FIXME: In fact the applet descriptors in modules could
	//        have been unloaded while the popup was being shown...
	//        For now we just assume that this never happens :D

	bool bOk = false;
	int iPar = pAction->data().toInt(&bOk);
	if(!bOk)
		return;

	KviPointerHashTableIterator<QString, KviStatusBarAppletDescriptor> it(*m_pAppletDescriptors);
	while(KviStatusBarAppletDescriptor * d = it.current())
	{
		if(iPar == d->id())
		{
			if(m_pClickedApplet)
			{
				int iIdx = m_pAppletList->findRef(m_pClickedApplet);
				if(iIdx != -1)
				{
					// try to put the new applet just after the clicked one
					bool bSave = m_bStopLayoutOnAddRemove;
					m_bStopLayoutOnAddRemove = true;
					KviStatusBarApplet * pApplet = d->create(this);
					m_pAppletList->removeRef(pApplet);
					m_pAppletList->inSort(pApplet);
					m_bStopLayoutOnAddRemove = bSave;
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
	m_pAppletDescriptors->replace(d->internalName(), d);
}

void KviStatusBar::registerApplet(KviStatusBarApplet * pApplet)
{
	m_pAppletList->inSort(pApplet);
	if(!pApplet->isVisible())
		pApplet->show();
}

void KviStatusBar::unregisterApplet(KviStatusBarApplet * pApplet)
{
	if(!pApplet)
		return;

	m_pAppletList->removeRef(pApplet);
	if(pApplet->isVisible())
		pApplet->hide();
}

int KviStatusBar::insertPermanentWidgetAtTheEnd(QWidget * widget, int stretch)
{
	return insertPermanentWidget(m_pAppletList->count(), widget, stretch);
}

void KviStatusBar::mousePressEvent(QMouseEvent * e)
{
	m_pClickedApplet = nullptr;
	if((e->button() == Qt::LeftButton) && (e->modifiers() & (Qt::ShiftModifier | Qt::ControlModifier)))
	{
		// move!
		m_pClickedApplet = (KviStatusBarApplet *)childAt(e->pos());
		if(m_pClickedApplet)
		{
			QDrag * dr = new QDrag(this);
			// The data to be transferred by the drag and drop operation is contained in a QMimeData object
			QMimeData * data = new QMimeData;
			QString szIndex;
			szIndex.setNum(m_pClickedApplet->index());
			data->setData("kvirc/statusbarapplet", szIndex.toUtf8());
			// Assign ownership of the QMimeData object to the QDrag object.
			dr->setMimeData(data);
			QPixmap pixmap(m_pClickedApplet->size());
			m_pClickedApplet->render(&pixmap);
			dr->setPixmap(pixmap);
			// Start the drag and drop operation
			m_pAppletList->sort();
			dr->start();
		}
	}
}

void KviStatusBar::mouseDoubleClickEvent(QMouseEvent *)
{
	const QString szTitle = __tr2qs("Network Information - KVIrc");
	QString szText, szUserModes, szAway;

	KviIrcContext * c = m_pFrame->activeContext();

	if(!c)
		return;

	if(c->state() != KviIrcContext::Connected)
		return;

	// User modes
	if(!c->connection()->userInfo()->userMode().isEmpty())
	{
		szUserModes += "+";
		szUserModes += c->connection()->userInfo()->userMode();
	}

	// Away status
	if(c->connection()->userInfo()->isAway())
		szAway = __tr2qs("Yes");
	else
		szAway = __tr2qs("No");

	szText += __tr2qs("Network");
	szText += ": ";
	szText += c->connection()->currentNetworkName();
	szText += "\n";
	szText += __tr2qs("Server");
	szText += ": ";
	szText += c->connection()->currentServerName();
	szText += "\n";
	szText += __tr2qs("Nickname");
	szText += ": ";
	szText += c->connection()->userInfo()->nickName();
	szText += "\n";
	szText += __tr2qs("Username");
	szText += ": ";
	szText += c->connection()->userInfo()->userName();
	szText += "\n";
	szText += __tr2qs("Hostname");
	szText += ": ";
	szText += c->connection()->userInfo()->hostName();
	szText += "\n";
	szText += __tr2qs("Local IP");
	szText += ": ";
	szText += c->connection()->userInfo()->localHostIp();
	szText += "\n";
	szText += __tr2qs("User modes");
	szText += ": ";
	szText += szUserModes;
	szText += "\n";
	szText += __tr2qs("Away");
	szText += ": ";
	szText += szAway;

	QMessageBox * pMsgBox = new QMessageBox(QMessageBox::Information, szTitle, (const QString)szText, QMessageBox::Ok, this);
	pMsgBox->show();
}

void KviStatusBar::queueMessage(KviStatusBarMessage * pMsg)
{
	// Rearrange queue using the priority of the message
	if(m_pMessageQueue->count() > 0)
	{
		if(pMsg->priority() > m_pMessageQueue->first()->priority())
			m_pMessageQueue->prepend(pMsg);
		else
			m_pMessageQueue->append(pMsg);
	}
	else
		m_pMessageQueue->append(pMsg);

	if(!m_pMessageTimer)
		showFirstMessageInQueue();
	// else we wait for the message timer to shot
}

void KviStatusBar::messageTimerFired()
{
	if(m_pMessageTimer)
		m_pMessageTimer->stop();

	if(!m_pMessageQueue->isEmpty())
	{
		// the first message in queue is currently visible
		// kill it
		m_pMessageQueue->removeFirst();
		if(!m_pMessageQueue->isEmpty())
		{
			// somKviStatusBar::ething to show
			showFirstMessageInQueue();
		}
	}
	// nothing else to show
	delete m_pMessageTimer;
	m_pMessageTimer = nullptr;

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
			m_pMessageTimer = nullptr;
		}
		return;
	}

	if(!m_pMessageTimer)
	{
		m_pMessageTimer = new QTimer(this);
		QObject::connect(m_pMessageTimer, SIGNAL(timeout()), this, SLOT(messageTimerFired()));
	}
	else
	{
		m_pMessageTimer->stop();
	}

	m_pMessageLabel->setText(pMsg->text());
	m_pMessageTimer->start(pMsg->timeout());
}

void KviStatusBar::setPermanentMessage()
{
	if(m_pMessageTimer)
		return; // something is being actually shown!

	KviIrcContext * c = m_pFrame->activeContext();

	QString szTxt;

	if(c)
	{
		switch(c->state())
		{
			case KviIrcContext::Connected:
				if(!c->connection()->userInfo()->userMode().isEmpty())
				{
					szTxt += c->connection()->currentNickName();
					szTxt += " ";
					szTxt += "<b>+";
					szTxt += c->connection()->userInfo()->userMode();
					szTxt += "</b>";
				}
				else if(c->connection()->userInfo()->userMode().isEmpty())
					szTxt += c->connection()->currentNickName();

				szTxt += __tr2qs(" on ") + "<b>";
				szTxt += c->connection()->currentServerName();
				szTxt += "</b>";
				break;
			case KviIrcContext::Connecting:
				szTxt += "<b>";
				szTxt += __tr2qs("Connection in progress...");
				szTxt += "</b>";
				break;
			case KviIrcContext::LoggingIn:
				szTxt += "<b>";
				szTxt += c->connection()->currentServerName();
				szTxt += "</b> - ";
				szTxt += __tr2qs("Login in progress...");
				break;
			default:
				szTxt += "<b>";
				szTxt += __tr2qs("Not connected");
				szTxt += "</b>";
				break;
		}
	}
	else
	{
		szTxt += __tr2qs("No IRC context");
	}

	m_pMessageLabel->setText(szTxt);
}
