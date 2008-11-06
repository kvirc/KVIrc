//=============================================================================
//
//   File : kvi_mdichild.cpp
//   Creation date : Wed Jun 21 2000 17:35:45 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2008 TheXception (kvirc at thexception dot net)
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

/**
* \file kvi_mdichild.cpp
* \brief MDI subwindow stuff
*/

#define _KVI_DEBUG_CHECK_RANGE_
#include "kvi_debug.h"
#include "kvi_mdichild.h"
#include "kvi_mdimanager.h"
#include "kvi_string.h"
#include "kvi_locale.h"
#include "kvi_options.h"
#include "kvi_settings.h"
#include "kvi_iconmanager.h"
#include "kvi_window.h"
#include "kvi_pointerlist.h"
#include "kvi_tal_popupmenu.h"

#include <QCursor>
#include <QApplication>
#include <QFontMetrics>
#include <QPixmap>
#include <QStyle>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QBoxLayout>

#ifdef COMPILE_ON_MAC
	#include "kvi_app.h"  //Needed for g_pApp
	#include <QDesktopWidget>
#endif

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif


#define KVI_MDI_NORESIZE 0
#define KVI_MDI_RESIZE_TOP 1
#define KVI_MDI_RESIZE_LEFT 2
#define KVI_MDI_RESIZE_RIGHT 4
#define KVI_MDI_RESIZE_BOTTOM 8
#define KVI_MDI_RESIZE_TOPLEFT (1|2)
#define KVI_MDI_RESIZE_TOPRIGHT (1|4)
#define KVI_MDI_RESIZE_BOTTOMLEFT (8|2)
#define KVI_MDI_RESIZE_BOTTOMRIGHT (8|4)


KviMdiChild::KviMdiChild(KviMdiManager * par, const char * name)
: QMdiSubWindow()
{
	setObjectName(name ? name : "mdi_child");

	m_pManager = par;

	m_pClient = 0;

	connect(this, SIGNAL(windowStateChanged(Qt::WindowStates,Qt::WindowStates)), this, SLOT(windowStateChangedEvent(Qt::WindowStates,Qt::WindowStates)));
	connect(this, SIGNAL(minimizeSignal()), this, SLOT(minimize()), Qt::QueuedConnection);
	connect(this, SIGNAL(restoreSignal()), this, SLOT(restore()), Qt::QueuedConnection);
	connect(this, SIGNAL(maximizeSignal()), this, SLOT(maximize()), Qt::QueuedConnection);

	m_restoredGeometry   = QRect(10,10,320,240);
	setMinimumSize(KVI_MDICHILD_MIN_WIDTH,KVI_MDICHILD_MIN_HEIGHT);
	m_bCloseEnabled = true;
	m_State = Normal;

	connect(systemMenu(), SIGNAL(aboutToShow()), this, SLOT(updateSystemPopup()));

	setAutoFillBackground(true);
}

void KviMdiChild::setRestoredGeometry(const QRect &r)
{
	m_restoredGeometry = r;
	setGeometry(r);
}

KviMdiChild::~KviMdiChild()
{
	if(m_pClient) delete m_pClient;
}

void KviMdiChild::closeEvent(QCloseEvent * e)
{
	widget()->close();
	e->ignore();
}

QRect KviMdiChild::restoredGeometry()
{
	return geometry();
}

KviMdiChild::MdiChildState KviMdiChild::state()
{
	return m_State;
}

void KviMdiChild::setBackgroundRole(QPalette::ColorRole)
{
	// hack
	//QFrame::setBackgroundRole(QPalette::Window);
}

void KviMdiChild::setIcon(QPixmap pix)
{
	m_pIcon = pix;
	QMdiSubWindow::setWindowIcon(QIcon(pix));
}

const QPixmap * KviMdiChild::icon()
{
	return &m_pIcon;
}

void KviMdiChild::enableClose(bool bEnable)
{
	m_bCloseEnabled = bEnable;
}

bool KviMdiChild::closeEnabled()
{
	return m_bCloseEnabled;
}

void KviMdiChild::setWindowTitle(const QString & plain,const QString & xmlActive,const QString & xmlInactive)
{
	m_szPlainCaption = plain;
	m_szXmlActiveCaption = xmlActive;
	m_szXmlInactiveCaption = xmlInactive;
	QMdiSubWindow::setWindowTitle(plain);
}

void KviMdiChild::windowStateChangedEvent( Qt::WindowStates oldState, Qt::WindowStates newState )
{
	// check if window has been minmized
	if (!(oldState & Qt::WindowMinimized) && (newState & Qt::WindowMinimized))
	{
		m_LastState = m_State;
		m_State = Minimized;
		// notify KviMdiManager
		m_pManager->childMinimized(this,true);
		// call the queued event since we can't change state while being in another event
		emit minimizeSignal();
		// update channelist caption
		updateCaption();
		return;
	}

	// check if window has been maximized
	if (!(oldState & Qt::WindowMaximized) && newState & Qt::WindowMaximized)
	{
		m_LastState = m_State;
		m_State = Maximized;

		m_pManager->childMaximized(this);
		// update channelist caption
		updateCaption();
		return;
	}

	if ((oldState & Qt::WindowMaximized) && !(newState & Qt::WindowMaximized))
	{
		m_LastState = m_State;
		m_State = Normal;
		m_pManager->childRestored(this,true);
		// update channelist caption
		updateCaption();
		return;
	}

	if ((oldState & Qt::WindowMinimized) && !(newState & Qt::WindowMinimized))
	{
		m_LastState = m_State;
		m_State = Normal;
		// notify KviMdiManager
		m_pManager->childRestored(this,true);
		// update channelist caption
		updateCaption();
		return;
	}
}

void KviMdiChild::queuedMinimize()
{
	emit minimizeSignal();
}

void KviMdiChild::queuedRestore()
{
	emit restoreSignal();
}

void KviMdiChild::queuedMaximize()
{
	emit maximizeSignal();
}

void KviMdiChild::maximize()
{
	m_LastState = m_State;
	m_State = Maximized;

	showMaximized();
	m_pManager->childMaximized(this);

	updateCaption();
}

void KviMdiChild::restore()
{
	switch(state())
	{
		case Maximized:
			m_State = Normal;
			showNormal();
			m_pManager->childRestored(this,true);
		break;
		case Minimized:
			m_State = Normal;
			if (m_LastState == Normal)
			{
				showNormal();
			} else maximize();
			updateCaption();
			m_pManager->childRestored(this,false);
		break;
		case Normal:
			m_State = Normal;
			if(!isVisible())
			{
				show();
			}
			return;
		break;
	}
}

void KviMdiChild::minimize()
{
	m_LastState = m_State;
	switch(state())
	{
		case Maximized:
			m_State = Minimized;
			hide();
			m_pManager->childMinimized(this,true);
		break;
		case Normal:
			m_State = Minimized;
			hide();
			m_pManager->childMinimized(this,false);
		break;
		case Minimized:
			m_State = Minimized;
			if(isVisible())
			{
				hide();
			}
			return;
		break;
	}
	updateCaption();
}

void KviMdiChild::updateCaption()
{
	if (m_pClient && m_pClient->inherits("KviWindow"))
	{
		((KviWindow*)m_pClient)->updateCaptionListItem();
	}
}

void KviMdiChild::moveEvent(QMoveEvent *e)
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(m_pClient && g_pShadedChildGlobalDesktopBackground)
	{
		if(m_pClient->inherits("KviWindow")) // actually this is always the case
		{
			((KviWindow *)m_pClient)->updateBackgrounds();
		}
	}
#endif
	QMdiSubWindow::moveEvent(e);
}

void KviMdiChild::systemPopupSlot()
{
		emit systemPopupRequest(QCursor::pos());
}

void KviMdiChild::setClient(QWidget * w)
{
	__range_valid(m_pClient==0);
	__range_valid(w!=0);

	m_pClient = w;
	setWidget(w);

	KviStr tmp(KviStr::Format,"mdi_child_%s",w->objectName().toUtf8().data());
	w->setObjectName(tmp.ptr());
}

void KviMdiChild::unsetClient()
{
	__range_valid(m_pClient!=0);
	if(!m_pClient)return;
	setWidget(0);
	m_pClient = 0;
	setObjectName("mdi_child");
}

void KviMdiChild::activate(bool bSetFocus)
{
	if(m_pManager->topChild() != this)
		m_pManager->setTopChild(this,bSetFocus);
	else if(bSetFocus)setFocus();
}

void KviMdiChild::focusInEvent(QFocusEvent *)
{
}

void KviMdiChild::updateSystemPopup()
{
	if(m_pClient->inherits("KviWindow"))
	{
		systemMenu()->clear();
		QMenu * tmp = ((KviWindow*) m_pClient)->generatePopup();
		if (tmp)
		{
			systemMenu()->addActions(tmp->actions());
		}
	}

}
