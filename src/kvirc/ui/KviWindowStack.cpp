//=============================================================================
//
//   File : KviWindowStack.cpp
//   Creation date : Mon Sep 28 2015 17:28:04 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2008 TheXception (kvirc at thexception dot net)
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

/**
* \file KviWindowStack.cpp
* \brief The window manager
*/

#include "kvi_debug.h"
#include "kvi_settings.h"
#include "KviWindowStack.h"
#include "KviWindow.h"
#include "KviLocale.h"
#include "KviOptions.h"
#include "KviIconManager.h"
#include "KviMainWindow.h"
#include "KviMenuBar.h"
#include "KviApplication.h"
#include "KviTalHBox.h"

#include <QMenuBar>
#include <QLayout>
#include <QPainter>
#include <QCursor>
#include <QEvent>
#include <QMouseEvent>
#include <QLabel>
#include <QToolButton>
#include <QMenu>

#ifdef COMPILE_PSEUDO_TRANSPARENCY
#include <QPixmap>
extern QPixmap * g_pShadedParentGlobalDesktopBackground;
#endif

KviWindowStack::KviWindowStack(QWidget * parent, const char * pcName)
    : QStackedWidget(parent)
{
	setObjectName(pcName);
	setFrameShape(NoFrame);

	m_pWindowPopup = new QMenu(this);
	connect(m_pWindowPopup, SIGNAL(triggered(QAction *)), this, SLOT(menuActivated(QAction *)));
	connect(m_pWindowPopup, SIGNAL(aboutToShow()), this, SLOT(fillWindowPopup()));

	m_pWindowPopup->setStyleSheet("QMenu { menu-scrollable: 1; }");

	setAutoFillBackground(false);

	connect(this, SIGNAL(currentChanged(int)), this, SLOT(currentWindowChanged(int)));
}

KviWindowStack::~KviWindowStack()
    = default;

void KviWindowStack::currentWindowChanged(int iIdx)
{
	g_pMainWindow->windowActivated((iIdx >= 0) ? (KviWindow *)widget(iIdx) : nullptr);
}

void KviWindowStack::addWindow(KviWindow * pWnd)
{
	addWidget(pWnd);
}

void KviWindowStack::showAndActivate(KviWindow * pWnd)
{
	if(pWnd->isDocked())
		setCurrentWidget(pWnd);
	else
	{
		pWnd->raise();
		pWnd->activateWindow();
	}

	if(!pWnd->hasFocus())
		pWnd->setFocus();
}

void KviWindowStack::destroyWindow(KviWindow * pWnd)
{
	removeWidget(pWnd);
	delete pWnd;
}

void KviWindowStack::fillWindowPopup()
{
	m_pWindowPopup->clear();

	QString szItem;
	QString szCaption;

	KviWindow * pWnd;

	QAction * pAction;

	int idx = 0;
	int cnt = count();

	while(idx < cnt)
	{
		QWidget * w = widget(idx);
		if(!w->inherits("KviWindow"))
		{
			idx++;
			continue;
		}

		pWnd = (KviWindow *)w;

		szItem.setNum(((uint)idx) + 1);
		szItem += ". ";

		szCaption = pWnd->windowTitle();
		if(szCaption.length() > 60)
		{
			QString trail = szCaption.right(27);
			szCaption.truncate(27);
			szCaption += "...";
			szCaption += trail;
		}

		szItem += szCaption;

		const QPixmap * pix = pWnd->myIconPtr();

		if(pix && !(pix->isNull()))
			pAction = m_pWindowPopup->addAction(*pix, szItem);
		else
			pAction = m_pWindowPopup->addAction(szItem);

		pAction->setData(idx);

		if(w == currentWidget())
			pAction->setChecked(true);

		idx++;
	}
}

void KviWindowStack::menuActivated(QAction * pAction)
{
	bool bOk = false;
	int id = pAction->data().toInt(&bOk);
	if(!bOk)
		return;

	if(id > count())
		return;

	QWidget * w = widget(id);
	if(!w)
		return;

	if(!w->inherits("KviWindow"))
		return;

	showAndActivate((KviWindow *)w);
}
