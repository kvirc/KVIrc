#ifndef _KVI_HISTORYWIN_CPP_
#define _KVI_HISTORYWIN_CPP_
//=============================================================================
//
//   File : KviHistoryWindow.cpp
//   Creation date : Mon Aug 19 01:34:48 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviHistoryWindow.h"
#include "KviApplication.h"
#include "KviInput.h"
#include "KviInputHistory.h"
#include "KviOptions.h"

#include <QEvent>
#include <QListWidget>
#include <QMouseEvent>

#include <cctype>

KviHistoryWindow::KviHistoryWindow(QWidget * pParent)
    : QListWidget(pParent)
{
	m_pParent = pParent;
	m_pOwner = nullptr;
	setSelectionMode(QAbstractItemView::SingleSelection);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	connect(this, SIGNAL(itemActivated(QListWidgetItem *)), this, SLOT(itemSelected(QListWidgetItem *)));

	m_iTimerId = -1;
}

KviHistoryWindow::~KviHistoryWindow()
{
	if(m_iTimerId != -1)
	{
		killTimer(m_iTimerId);
		m_iTimerId = -1;
	}
}

void KviHistoryWindow::fill()
{
	clear();

	for(auto & szTmp : KviInputHistory::instance()->list())
		addItem(szTmp);

	if(count() > 0)
		setCurrentItem(item(count() - 1));
}

void KviHistoryWindow::popup(KviInput * pOwner)
{
	if(m_pOwner)
		disconnect(m_pOwner, SIGNAL(destroyed()), this, SLOT(ownerDead()));

	m_pOwner = pOwner;
	connect(m_pOwner, SIGNAL(destroyed()), this, SLOT(ownerDead()));

	fill();
	show();
}

void KviHistoryWindow::mousePressEvent(QMouseEvent * e)
{
	if(
	    (e->pos().x() < 0) || (e->pos().x() > width()) || (e->pos().y() < 0) || (e->pos().y() > height()))
		goto hideme;

	QListWidget::mousePressEvent(e);
	e->accept();
	return;

hideme:
	doHide();
}

void KviHistoryWindow::keyPressEvent(QKeyEvent * e)
{
	switch(e->key())
	{
		case Qt::Key_Up:
		case Qt::Key_Down:
		case Qt::Key_PageUp:
		case Qt::Key_PageDown:
		case Qt::Key_Return:
			QListWidget::keyPressEvent(e);
			return;
			break;
		case Qt::Key_Escape:
			doHide();
			return;
			break;
	}

	if(m_pOwner)
		g_pApp->sendEvent(m_pOwner, e);
}

void KviHistoryWindow::ownerDead()
{
	m_pOwner = nullptr;
	doHide();
}

void KviHistoryWindow::show()
{
	m_iTimerId = startTimer(100000); //100 sec ...seems enough
	QWidget::show();
}

void KviHistoryWindow::timerEvent(QTimerEvent * e)
{
	if(e->timerId() != m_iTimerId)
	{
		QListWidget::timerEvent(e);
		return;
	}

	m_pOwner = nullptr; // do not setFocus() to the owner after the timeout
	doHide();
}

void KviHistoryWindow::doHide()
{
	if(m_iTimerId != -1)
	{
		killTimer(m_iTimerId);
		m_iTimerId = -1;
	}

	m_pParent->hide();

	if(m_pOwner)
		m_pOwner->setFocus();
}

void KviHistoryWindow::itemSelected(QListWidgetItem * pItem)
{
	doHide();
	if(m_pOwner && pItem)
		m_pOwner->setText(pItem->text());
}

void KviHistoryWindow::hideEvent(QHideEvent *)
{
	if(m_iTimerId != -1)
	{
		killTimer(m_iTimerId);
		m_iTimerId = -1;
	}
}

KviHistoryWindowWidget::KviHistoryWindowWidget()
    : QWidget(nullptr)
{
	setWindowFlags(Qt::Popup);
	m_pWindow = new KviHistoryWindow(this);
}

KviHistoryWindowWidget::~KviHistoryWindowWidget()
    = default;

void KviHistoryWindowWidget::popup(KviInput * pOwner)
{
	m_pWindow->popup(pOwner);
	m_pWindow->setFixedSize(width(), height());
	show();
}

#endif //_KVI_HISTORYWIN_CPP_
