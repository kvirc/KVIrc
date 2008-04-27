//=============================================================================
//
//   File : kvi_texticonwin.cpp
//   Creation date : Fri May 17 2002 02:35:20 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2004 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_texticonwin.h"
#include "kvi_texticonmanager.h"
#include "kvi_app.h"
#include "kvi_options.h"
#include "kvi_input.h"
#include "kvi_topicw.h"
#include "kvi_mirccntrl.h"
#include "kvi_iconmanager.h"

#include <QLineEdit>
#include <QEvent>
#include <QKeyEvent>
#include <QHeaderView>

#include <ctype.h>

KviTextIconWindow::KviTextIconWindow()
: KviTalIconView(0,Qt::Popup)
{
		m_iTimerId = -1;
//	setGridX ( 40 );
	setColumnCount(3);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setRowCount(10);
	setWordWrap(true);
	setFixedSize(KVI_TEXTICON_WIN_WIDTH,KVI_TEXTICON_WIN_HEIGHT);
	m_pOwner = 0;
	fill();
	//connect(g_pTextIconManager,SIGNAL(changed()),this,SLOT(fill()));
	connect(this,SIGNAL(doubleClicked( KviTalIconViewItem * )),this,SLOT(itemSelected(KviTalIconViewItem *)));
	//connect(this,SIGNAL(returnPressed ( KviTalIconViewItem * ) ),this,SLOT(itemSelected(KviTalIconViewItem *)));
	m_bAltMode = false;
	
}

KviTextIconWindow::~KviTextIconWindow()
{
	if(m_iTimerId != -1)
	{
		killTimer(m_iTimerId);
		m_iTimerId = -1;
	}
//	killTimers();
//	if(m_pOwner)m_pOwner->setFocus();
}

void KviTextIconWindow::fill()
{
	//setEditTriggers( QAbstractItemView::NoEditTriggers );
	clear();
	KviPointerHashTable<QString,KviTextIcon> * d = g_pTextIconManager->textIconDict();
	KviPointerHashTableIterator<QString,KviTextIcon> it(*d);
	
	int col=0;
	int row=0;
	while(KviTextIcon * i = it.current())
	{
		QPixmap *pix = i->pixmap();
		if(pix){
			setItem(row,col,new KviTalIconViewItem(it.currentKey(),*pix));
			col++;
			if (col==3){
				col=0;
				row++;
			}
		}
		++it;
	}
//	horizontalHeader()->setDefaultSectionSize(70);
//	horizontalHeader()->resizeSections(QHeaderView::Fixed);
	resizeColumnsToContents();
	resizeRowsToContents();
	sortItems ( 0, Qt::AscendingOrder );
	setCurrentItem(0);
}

void KviTextIconWindow::popup(QWidget *owner,bool bAltMode)
{
	debug("Show icon view widget");
	if(m_pOwner)disconnect(m_pOwner,SIGNAL(destroyed()),this,SLOT(ownerDead()));
	m_pOwner = owner;
	m_szTypedSeq = "";
	m_bAltMode = bAltMode;
	connect(m_pOwner,SIGNAL(destroyed()),this,SLOT(ownerDead()));
//	resizeColumnsToContents();
	show();
}

bool KviTextIconWindow::findTypedSeq()
{
/*
	int cnt = count();
	int max = 0;
	KviTalIconViewItem *mit = 0;
	bool bFullMax = false;
	KviTalIconViewItem *item;
	for ( item = (KviTalIconViewItem *)firstItem(); item; item = (KviTalIconViewItem *)item->nextItem() )
	{
		QString szIt = item->text();
		int j;
		for(j=0;j<((int)(szIt.length()));j++)
		{
			if(szIt[j].lower() != m_szTypedSeq[j].lower())break;
		}
		if(j < max)
		{
			goto got_mit;
		} else {
			if(j >= max)
			{
				bFullMax = (j == ((int)(szIt.length())));
				max = j;
				mit = item;
			}
		}
	}
got_mit:
	setCurrentItem(mit);
	m_szCurFullSeq = mit->text();
	return bFullMax;
	*/
	return true;
}

void KviTextIconWindow::keyPressEvent(QKeyEvent *e)
{
	switch(e->key())
	{
		case Qt::Key_Up:
		case Qt::Key_Down:
		case Qt::Key_Left:
		case Qt::Key_Right:
		case Qt::Key_PageUp:
		case Qt::Key_PageDown:
		case Qt::Key_Return:
			KviTalIconView::keyPressEvent(e);
			return;
		break;
		case Qt::Key_Escape:
			doHide();
			return;
		break;
		case Qt::Key_Backspace:
			if(!m_szTypedSeq.isEmpty())
			{
				m_szTypedSeq.remove(m_szTypedSeq.length() - 1,1);
				findTypedSeq();
			} else {
				doHide();
				if(m_pOwner)g_pApp->sendEvent(m_pOwner,e);
			}
			return;
		break;
		case Qt::Key_Space:
			doHide();
			if(findTypedSeq())
			{
				QString szItem = m_szTypedSeq;
				szItem.append(' ');
				if(m_pOwner)
				{
					if(m_pOwner->inherits("KviInputEditor"))
						((KviInputEditor *)m_pOwner)->insertText(szItem);
					else if(m_pOwner->inherits("KviInput"))
						((KviInput *)m_pOwner)->insertText(szItem);
					else if(m_pOwner->inherits("QLineEdit"))
					{
						QString tmp = ((QLineEdit *)m_pOwner)->text();
						tmp.insert(((QLineEdit *)m_pOwner)->cursorPosition(),szItem);
						((QLineEdit *)m_pOwner)->setText(tmp);
						((QLineEdit *)m_pOwner)->setCursorPosition(((QLineEdit *)m_pOwner)->cursorPosition() + szItem.length());
					}
				}
			} else {
				if(m_pOwner)g_pApp->sendEvent(m_pOwner,e);
			}
			return;
		break;
		case Qt::Key_Tab:
			doHide();
			findTypedSeq();
			QString szItem = m_szCurFullSeq;
			szItem.append(' ');
			if(m_bAltMode)szItem.prepend(KVI_TEXT_ICON);
			if(m_pOwner->inherits("KviInputEditor"))
				((KviInputEditor *)m_pOwner)->insertText(szItem);
			else if(m_pOwner->inherits("KviInput"))
				((KviInput *)m_pOwner)->insertText(szItem);
			else if(m_pOwner->inherits("QLineEdit"))
			{
				QString tmp = ((QLineEdit *)m_pOwner)->text();
				tmp.insert(((QLineEdit *)m_pOwner)->cursorPosition(),szItem);
				((QLineEdit *)m_pOwner)->setText(tmp);
				((QLineEdit *)m_pOwner)->setCursorPosition(((QLineEdit *)m_pOwner)->cursorPosition() + szItem.length());
			}
		return;
		break;
	}

	int as = e->ascii();
	if((as >= 'a' && as <= 'z') || (as >= 'A' && as <= 'Z') || (as >= '0' && as <= '9')
		|| (as == '?') || (as == '$') || (as == '.') || (as == ',') || (as == '!') || (as =='&'))
	{
		m_szTypedSeq.append((char)as);
		findTypedSeq();
	} else {
		if(m_pOwner)g_pApp->sendEvent(m_pOwner,e);
	}
}

void KviTextIconWindow::ownerDead()
{
	m_pOwner = 0;
	doHide();
}

void KviTextIconWindow::show()
{
	m_iTimerId = startTimer(50000); //50 sec ...seems enough
	debug("Set timer with id %d",m_iTimerId);
	fill();
	QWidget::show();
}

void KviTextIconWindow::timerEvent(QTimerEvent *e)
{
	// FIXME: maybe a QT4 bug? (without this filter below KVIrc will be slow!)
	if (e->timerId()!=m_iTimerId) {debug ("Timer unknown with %d", e->timerId());killTimer(e->timerId());return;}
	debug("Timer stop");
	doHide();
}

void KviTextIconWindow::doHide()
{
	if(m_iTimerId != -1)
	{
		killTimer(m_iTimerId);
		m_iTimerId = -1;
	}
	hide();
	clear();
	if(m_pOwner)m_pOwner->setFocus();
}

void KviTextIconWindow::itemSelected(KviTalIconViewItem * item)
{
	debug("Into item selected");
	if(item)
	{
//		debug("%i %i %i %s",m_pOwner->inherits("KviInputEditor"),m_pOwner->inherits("KviInput"),m_pOwner->inherits("QLineEdit"),m_pOwner->className());
		QString szItem = item->text();
		szItem.append(' ');
		if(m_bAltMode)szItem.prepend(KVI_TEXT_ICON);
		if(m_pOwner->inherits("KviInputEditor"))
			((KviInputEditor *)m_pOwner)->insertText(szItem);
		else if(m_pOwner->inherits("KviInput"))
			((KviInput *)m_pOwner)->insertText(szItem);
		else if(m_pOwner->inherits("QLineEdit"))
		{
			QString tmp = ((QLineEdit *)m_pOwner)->text();
			tmp.insert(((QLineEdit *)m_pOwner)->cursorPosition(),szItem);
			((QLineEdit *)m_pOwner)->setText(tmp);
			((QLineEdit *)m_pOwner)->setCursorPosition(((QLineEdit *)m_pOwner)->cursorPosition() + szItem.length());
		}
		scroll(1,1);
			doHide();
 	
	}
}

void KviTextIconWindow::mousePressEvent(QMouseEvent *e)
{
	if(e->pos().x() < 0)goto hideme;
	if(e->pos().x() > width())goto hideme;
	if(e->pos().y() < 0)goto hideme;
	if(e->pos().y() > height())goto hideme;

	KviTalIconView::mousePressEvent(e);
	return;

hideme:
	debug("Mouse press");
	doHide();
}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_texticonwin.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
