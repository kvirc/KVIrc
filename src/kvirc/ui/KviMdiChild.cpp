//=============================================================================
//
//   File : KviMdiChild.cpp
//   Creation date : Wed Jun 21 2000 17:35:45 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

/**
* \file KviMdiChild.cpp
* \brief MDI subwindow stuff
*/

#define _KVI_DEBUG_CHECK_RANGE_
#include "kvi_debug.h"
#include "KviMdiChild.h"
#include "KviMdiManager.h"
#include "KviCString.h"
#include "KviLocale.h"
#include "KviOptions.h"
#include "kvi_settings.h"
#include "KviIconManager.h"
#include "KviMainWindow.h"
#include "KviWindow.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionTarget.h"
#include "KviIrcContext.h"
#include "KviIrcNetwork.h"
#include "KviKvsEventTriggers.h"

#include <QTimer>
#include <QMenu>

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

KviMdiChild::KviMdiChild(KviMdiManager * par, const char * name)
: QMdiSubWindow(par)
{
	setObjectName(name ? name : "mdi_child");

	m_pManager = par;
	m_pClient = 0;

	connect(systemMenu(), SIGNAL(aboutToShow()), this, SLOT(updateSystemPopup()));
	QTimer::singleShot( 0, this, SLOT(transparencyWorkaround()));
}

KviMdiChild::~KviMdiChild()
{
	if(m_pClient)
	{
		delete m_pClient;
		m_pClient=0;
	}
	m_pManager->focusPreviousTopChild();
}

void KviMdiChild::transparencyWorkaround()
{
	setAutoFillBackground(true);
}

void KviMdiChild::closeEvent(QCloseEvent * e)
{
	//we let kviwindow handle the process
	KviWindow * pClient = (KviWindow*) widget();
	if(pClient)
	{
		e->ignore();
		pClient->delayedClose();
	}
}


KviMdiChild::MdiChildState KviMdiChild::state()
{
	//if(!isVisible())
	//	return Minimized;
	if(isMinimized())
		return Minimized;
	if(isMaximized())
		return Maximized;
	return Normal;
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

void KviMdiChild::setWindowTitle(const QString & plain)
{
	m_szPlainCaption = plain;
	QMdiSubWindow::setWindowTitle(plain);
}

void KviMdiChild::restore()
{
	if(isMinimized())
	{
		// we don't use showNormal here because it seems to enforce the previous window geometry:
		// since we were minimized in a shaded form, this creates ugly effects (like bug #823)
		setWindowState(windowState() & ~Qt::WindowMinimized);
		show();
		return;
	}

	if(isMaximized())
	{
		setWindowState(windowState() & ~Qt::WindowMaximized);
		showNormal();
		return;
	}

	if(!isVisible())
	{
		show();
		return;
	}
}

void KviMdiChild::maximize()
{
	if(isVisible())
	{
		showMaximized();
	} else {
		setWindowState(windowState() & Qt::WindowMaximized & ~Qt::WindowMaximized);
	}
}

void KviMdiChild::minimize()
{
	if(isVisible())
		showMinimized();
	else
		setWindowState((windowState() & Qt::WindowMinimized) & ~Qt::WindowMaximized);
}

void KviMdiChild::updateCaption()
{
	if(m_pClient)
		if(m_pClient->inherits("KviWindow"))
		{
			((KviWindow*)m_pClient)->updateCaption();
		}
}

void KviMdiChild::moveEvent(QMoveEvent *e)
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	//this is not needed for real transparency using compositing
	if(m_pClient && g_pShadedChildGlobalDesktopBackground)
	{
		if(m_pClient->inherits("KviWindow")) // actually this is always the case
		{
			((KviWindow *)m_pClient)->updateBackgrounds();
		}
	}
#endif
	QMdiSubWindow::moveEvent(e);

	if(!(windowState() & (Qt::WindowMinimized | Qt::WindowMaximized)))
	{
		m_rNormalizedGeometry.moveTopLeft(e->pos());
// 		qDebug("[%s] normalGeometry x=%d y=%d w=%d h=%d", m_szPlainCaption.toUtf8().data(), m_rNormalizedGeometry.x(), m_rNormalizedGeometry.y(), m_rNormalizedGeometry.width(), m_rNormalizedGeometry.height());
	}
}

void KviMdiChild::resizeEvent(QResizeEvent *e)
{
	QMdiSubWindow::resizeEvent(e);

	if(!(windowState() & (Qt::WindowMinimized | Qt::WindowMaximized)))
	{
		m_rNormalizedGeometry.setSize(e->size());
// 		qDebug("[%s] normalGeometry x=%d y=%d w=%d h=%d", m_szPlainCaption.toUtf8().data(), m_rNormalizedGeometry.x(), m_rNormalizedGeometry.y(), m_rNormalizedGeometry.width(), m_rNormalizedGeometry.height());
	}
}

void KviMdiChild::setClient(QWidget * w)
{
	KVI_ASSERT(m_pClient==0);
	KVI_ASSERT(w!=0);

	m_pClient = w;
	setWidget(w);
	QString tmp1;
	if(w->inherits("KviWindow"))
	{
		KviIrcConnection * pConnection=((KviWindow*)w)->connection();
		if(pConnection)
			tmp1.append(pConnection->target()->network()->name()+"_");
	}

	KviCString tmp(KviCString::Format,"mdi_child_%s%s",tmp1.toUtf8().data(),w->objectName().toUtf8().data());
	setObjectName(tmp.ptr());
}

void KviMdiChild::unsetClient()
{
	KVI_ASSERT(m_pClient!=0);
	if(!m_pClient)return;
	setWidget(0);
	m_pClient = 0;
	setObjectName("mdi_child");
}

void KviMdiChild::activate()
{
	m_pManager->setActiveSubWindow(this);
	//setFocus(); // should propagate to the client
}

void KviMdiChild::updateSystemPopup()
{
	if(m_pClient)
		if(m_pClient->inherits("KviWindow"))
		{
			systemMenu()->clear();
			//FIXME this is an hack
			KVS_TRIGGER_EVENT_0(KviEvent_OnWindowPopupRequest, ((KviWindow*) m_pClient));
		}
}

