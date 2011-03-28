//=============================================================================
//
//   File : KviTextIconWindow.cpp
//   Creation date : Fri May 17 2002 02:35:20 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviTextIconWindow.h"
#include "KviTextIconManager.h"
#include "KviApplication.h"
#include "KviOptions.h"
#include "KviInput.h"
#include "KviTopicWidget.h"
#include "KviControlCodes.h"
#include "KviIconManager.h"

#include <QPainter>
#include <QLineEdit>
#include <QEvent>
#include <QKeyEvent>
#include <QHeaderView>
#include <QPalette>
#include <QScrollBar>

KviTextIconWindow::KviTextIconWindow()
: QWidget(0, Qt::Popup)
{
	m_pOwner   = 0;
	m_bAltMode = false;

	setFixedSize(KVI_TEXTICON_WIN_WIDTH,KVI_TEXTICON_WIN_HEIGHT);
	m_pTable = new QTableWidget(this);
	m_pTable->setFixedSize(KVI_TEXTICON_WIN_WIDTH,KVI_TEXTICON_WIN_HEIGHT);
	m_pTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pTable->horizontalHeader()->hide();
	m_pTable->verticalHeader()->hide();
	m_pTable->setShowGrid(false);
	m_pTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	fill();
	m_pTable->setFocus(Qt::PopupFocusReason);
	connect(g_pTextIconManager,SIGNAL(changed()),this,SLOT(fill()));
	connect(m_pTable,SIGNAL(cellClicked( int, int )),this,SLOT(cellSelected(int, int)));
}

KviTextIconWindow::~KviTextIconWindow()
{
	m_pTable->deleteLater();
}

void KviTextIconWindow::fill()
{
	m_pTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_pTable->clear();
	m_pTable->setRowCount(0);

	m_pTable->setColumnCount(KVI_TEXTICON_COLUMNS);
	int iColWidth=((width()-24)/KVI_TEXTICON_COLUMNS);
	for(int i=0; i<KVI_TEXTICON_COLUMNS;i++)
		m_pTable->setColumnWidth(i,iColWidth);

	KviPointerHashTable<QString,KviTextIcon> * pDict = g_pTextIconManager->textIconDict();
	KviPointerHashTableIterator<QString,KviTextIcon> it(*pDict);

	int iCol = KVI_TEXTICON_COLUMNS;
	QLabel* newItem;
	while(KviTextIcon * pIcon = it.current())
	{
		QPixmap * pPix = pIcon->pixmap();
		if(pPix)
		{
			newItem = new QLabel();
			newItem->setToolTip(it.currentKey());
			newItem->setPixmap(*pPix);
			newItem->setAlignment(Qt::AlignCenter);
			if(iCol == KVI_TEXTICON_COLUMNS)
			{
				iCol=0;
				m_pTable->insertRow(m_pTable->rowCount());
			}
			m_pTable->setCellWidget(m_pTable->rowCount()-1,iCol,newItem);
			iCol++;
		}
		++it;
	}

	m_pTable->sortItems(0,Qt::AscendingOrder);
	m_pTable->setCurrentItem(0);
}

void KviTextIconWindow::popup(QWidget * pOwner, bool bAltMode)
{
	m_bAltMode = bAltMode;

	if(m_pOwner)
		disconnect(m_pOwner,SIGNAL(destroyed()),this,SLOT(ownerDead()));

	m_pOwner = pOwner;
	connect(m_pOwner,SIGNAL(destroyed()),this,SLOT(ownerDead()));

	show();
}

void KviTextIconWindow::keyPressEvent(QKeyEvent * e)
{
	switch(e->key())
	{
		case Qt::Key_Space:
		case Qt::Key_Return:
		{
			cellSelected(m_pTable->currentRow(), m_pTable->currentColumn());
		}
		break;
		case Qt::Key_Tab:
			//avoid the text edit field to move to the icon cells using tab
			break;
		break;
		case Qt::Key_Escape:
			doHide();
			break;
		break;
		default:
			QWidget::keyPressEvent(e);
			break;
	}
}

void KviTextIconWindow::ownerDead()
{
	m_pOwner = 0;
	doHide();
}

void KviTextIconWindow::doHide()
{
	hide();

	if(m_pOwner)
		m_pOwner->setFocus();
}

void KviTextIconWindow::cellSelected(int row, int column)
{
	if(m_pTable->cellWidget(row, column))
	{
//		qDebug("%i %i %i %s",m_pOwner->inherits("KviInputEditor"),m_pOwner->inherits("KviInput"),m_pOwner->inherits("QLineEdit"),m_pOwner->className());
		QString szItem(m_pTable->cellWidget(row, column)->toolTip());
		szItem.append(' ');

		if(m_bAltMode)
			szItem.prepend(KviControlCodes::Icon);

		if(m_pOwner->inherits("KviInputEditor"))
			((KviInputEditor *)m_pOwner)->insertText(szItem);
		else if(m_pOwner->inherits("KviInput"))
			((KviInput *)m_pOwner)->insertText(szItem);
		else if(m_pOwner->inherits("QLineEdit"))
		{
			QString szTmp = ((QLineEdit *)m_pOwner)->text();
			szTmp.insert(((QLineEdit *)m_pOwner)->cursorPosition(),szItem);
			((QLineEdit *)m_pOwner)->setText(szTmp);
			((QLineEdit *)m_pOwner)->setCursorPosition(((QLineEdit *)m_pOwner)->cursorPosition() + szItem.length());
		}
		doHide();
	}
}



#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "KviTextIconWindow.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
