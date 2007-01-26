#ifndef _KVI_HISTORYWIN_CPP_
#define _KVI_HISTORYWIN_CPP_
//
//   File : kvi_historywin.cpp
//   Creation date : Mon Aug 19 01:34:48 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
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


#define __KVIRC__

#include "kvi_historywin.h"
#include "kvi_app.h"
#include "kvi_options.h"
#include "kvi_input.h"
#include "kvi_mirccntrl.h"

#include <qnamespace.h>

#include <ctype.h>

extern KviInputHistory * g_pInputHistory;

KviHistoryWindow::KviHistoryWindow()
#ifdef COMPILE_USE_QT4
: KviTalListBox(0,Qt::Popup)
#else
: KviTalListBox(0,Qt::WType_Popup)
#endif
{
	m_pOwner = 0;
	setHScrollBarMode(QScrollView::AlwaysOff);
	connect(this,SIGNAL(selected(const QString &)),this,SLOT(itemSelected(const QString &)));
}

KviHistoryWindow::~KviHistoryWindow()
{
	killTimers();
}

void KviHistoryWindow::fill()
{
	clear();
	for(QString * s = g_pInputHistory->list()->last();s;s = g_pInputHistory->list()->prev())
	{
		insertItem(*s);
	}
	if(count() > 0)setCurrentItem(count() - 1);
}

void KviHistoryWindow::popup(KviInput *owner)
{
	if(m_pOwner)disconnect(m_pOwner,SIGNAL(destroyed()),this,SLOT(ownerDead()));
	m_pOwner = owner;
	connect(m_pOwner,SIGNAL(destroyed()),this,SLOT(ownerDead()));
	fill();
	show();
}

void KviHistoryWindow::mousePressEvent(QMouseEvent *e)
{
	if(e->pos().x() < 0)goto hideme;
	if(e->pos().x() > width())goto hideme;
	if(e->pos().y() < 0)goto hideme;
	if(e->pos().y() > height())goto hideme;

	KviTalListBox::mousePressEvent(e);
	return;

hideme:
	doHide();
}

/*
bool KviHistoryWindow::findTypedSeq()
{
	int cnt = count();
	int max = 0;
	int mit = -1;
	bool bFullMax = false;
	for(int i=0;i<cnt;i++)
	{
		KviStr szIt = text(i);
		int j;
		for(j=0;j<szIt.len();j++)
		{
			if(tolower(*(szIt.ptr() + j)) != tolower(*(m_szTypedSeq.ptr() + j)))break;
		}
		if(j < max)
		{
			goto got_mit;
		} else {
			if(j >= max)
			{
				bFullMax = (j == szIt.len());
				max = j;
				mit = i;
			}
		}
	}
got_mit:
	setCurrentItem(mit);
	m_szCurFullSeq = text(mit);
	return bFullMax;
}
*/

void KviHistoryWindow::keyPressEvent(QKeyEvent *e)
{
	switch(e->key())
	{
		case Qt::Key_Up:
		case Qt::Key_Down:
		case Qt::Key_PageUp:
		case Qt::Key_PageDown:
		case Qt::Key_Return:
			KviTalListBox::keyPressEvent(e);
			return;
		break;
		case Qt::Key_Escape:
			doHide();
			return;
		break;
/*
		case Qt::Key_Backspace:
			if(m_szTypedSeq.hasData())
			{
				m_szTypedSeq.cutRight(1);
				findTypedSeq();
			} else {
				doHide();
				if(m_pOwner)g_pApp->sendEvent(m_pOwner,e);
			}
			return;
		break;
*/
/*
		case Qt::Key_Space:
			doHide();
			if(findTypedSeq())
			{
				KviStr szItem = m_szTypedSeq;
				szItem.append(' ');
				if(m_pOwner)m_pOwner->insertText(szItem);
			} else {
				if(m_pOwner)g_pApp->sendEvent(m_pOwner,e);
			}
			return;
		break;
*/
/*
		case Qt::Key_Tab:
			doHide();
			findTypedSeq();
			KviStr szItem = m_szCurFullSeq;
			szItem.append(KVI_TEXT_ICON);
			if(m_pOwner)m_pOwner->insertText(szItem);
			return;
		break;
*/
	}
/*
	int as = e->ascii();
	if((as >= 'a' && as <= 'z') || (as >= 'A' && as <= 'Z') || (as >= '0' && as <= '9')
		|| (as == '?') || (as == '$') || (as == '.') || (as == ',') || (as == '!') || (as =='&'))
	{
		m_szTypedSeq.append((char)as);
		findTypedSeq();
	} else {
*/
		if(m_pOwner)g_pApp->sendEvent(m_pOwner,e);
/*
	}
*/
}

void KviHistoryWindow::ownerDead()
{
	m_pOwner = 0;
	doHide();
}

void KviHistoryWindow::show()
{
	startTimer(100000); //100 sec ...seems enough
	QWidget::show();
}

void KviHistoryWindow::timerEvent(QTimerEvent *)
{
	doHide();
}

void KviHistoryWindow::doHide()
{
	killTimers();
	hide();
	if(m_pOwner)m_pOwner->setFocus();
}

void KviHistoryWindow::itemSelected(const QString &str)
{
	doHide();
	if(m_pOwner)m_pOwner->setText(str);
}


#include "kvi_historywin.moc"
#endif //_KVI_HISTORYWIN_CPP_
