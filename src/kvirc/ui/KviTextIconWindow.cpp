//=============================================================================
//
//   File : KviTextIconWindow.cpp
//   Creation date : Fri May 17 2002 02:35:20 by Szymon Stefanek
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

#include "KviTextIconWindow.h"
#include "KviApplication.h"
#include "KviCaster.h"
#include "KviControlCodes.h"
#include "KviIconManager.h"
#include "KviInput.h"
#include "KviInputEditor.h"
#include "KviOptions.h"
#include "KviTextIconManager.h"
#include "KviTopicWidget.h"

#include <QEvent>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPainter>
#include <QScrollBar>
#include <QTableWidget>
#include <limits>

KviTextIconWindow::KviTextIconWindow()
    : QWidget(nullptr, Qt::Popup)
{
	setFixedSize(KVI_TEXTICON_WIN_WIDTH, KVI_TEXTICON_WIN_HEIGHT);

	m_pTable = new QTableWidget(this);
	m_pTable->setFixedSize(KVI_TEXTICON_WIN_WIDTH, KVI_TEXTICON_WIN_HEIGHT);
	m_pTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pTable->horizontalHeader()->hide();
	m_pTable->verticalHeader()->hide();
	m_pTable->setShowGrid(false);
	m_pTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	fill();
	m_pTable->setFocus(Qt::PopupFocusReason);

	m_pTable->installEventFilter(this);

	connect(g_pTextIconManager, &KviTextIconManager::changed, this, &KviTextIconWindow::fill);
	connect(m_pTable, &QTableWidget::cellClicked, this, &KviTextIconWindow::cellSelected);
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
	int iColWidth = ((width() - 24) / KVI_TEXTICON_COLUMNS);
	for(int i = 0; i < KVI_TEXTICON_COLUMNS; i++)
		m_pTable->setColumnWidth(i, iColWidth);

	KviPointerHashTable<QString, KviTextIcon> * pDict = g_pTextIconManager->textIconDict();
	KviPointerHashTableIterator<QString, KviTextIcon> it(*pDict);

	int iCol = KVI_TEXTICON_COLUMNS;
	while(KviTextIcon * pIcon = it.current())
	{
		QPixmap * pPix = pIcon->pixmap();
		if(pPix)
		{
			QLabel * newItem = new QLabel;
			newItem->setToolTip(it.currentKey());
			newItem->setPixmap(*pPix);
			newItem->setAlignment(Qt::AlignCenter);
			if(iCol == KVI_TEXTICON_COLUMNS)
			{
				iCol = 0;
				m_pTable->insertRow(m_pTable->rowCount());
			}
			m_pTable->setCellWidget(m_pTable->rowCount() - 1, iCol, newItem);
			iCol++;
		}
		++it;
	}

	m_pTable->sortItems(0, Qt::AscendingOrder);
	m_pTable->setCurrentItem(nullptr);
}

void KviTextIconWindow::popup(QWidget * pOwner, bool bAltMode)
{
	m_bAltMode = bAltMode;

	if(m_pOwner)
		disconnect(m_pOwner, &QWidget::destroyed, this, &KviTextIconWindow::ownerDead);

	m_pOwner = pOwner;
	connect(m_pOwner, &QWidget::destroyed, this, &KviTextIconWindow::ownerDead);

	show();

	autoSelectBestMatchBasedOnOwnerText();
}

bool KviTextIconWindow::eventFilter(QObject * o, QEvent * e)
{
	if(o != m_pTable)
		return false;

	if((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease))
	{
		QKeyEvent * ev = KVI_DYNAMIC(QKeyEvent *, e);
		Q_ASSERT(ev);

		// Just in case...
		if(!ev)
			return false;

		switch(ev->key())
		{
			case Qt::Key_Left:
			case Qt::Key_Right:
			case Qt::Key_Up:
			case Qt::Key_Down:
				return false; // let it handle arrow navigation
				break;
			case Qt::Key_Space:
			case Qt::Key_Return:
				cellSelected(m_pTable->currentRow(), m_pTable->currentColumn());
				return false;
				break;
			case Qt::Key_Tab:
				//avoid the text edit field to move to the icon cells using tab
				return false;
				break;
			case Qt::Key_Escape:
				doHide();
				break;
			default:
				// redirect to owner
				if(KviInputEditor * pOwner = qobject_cast<KviInputEditor *>(m_pOwner); pOwner)
				{
					if(e->type() == QEvent::KeyPress)
						pOwner->keyPressEvent(ev);
					else
						pOwner->keyReleaseEvent(ev);
					autoSelectBestMatchBasedOnOwnerText();
					return true;
				}
				break;
		}
	}

	return false;
}

void KviTextIconWindow::autoSelectBestMatchBasedOnOwnerText()
{
	KviInputEditor * pOwner = qobject_cast<KviInputEditor *>(m_pOwner);
	if(!pOwner)
		return;

	QString szText = pOwner->textBeforeCursor();
	int idx = szText.lastIndexOf(QChar(KviControlCodes::Icon));
	if(idx < 0)
		return;

	QString szIco = szText.mid(idx + 1);

	if(szIco.isEmpty())
		return;

	int iRows = m_pTable->rowCount();
	int iCols = m_pTable->columnCount();

	int iBestR = -1;
	int iBestC = -1;
	int iBestLen = std::numeric_limits<int>::max();

	for(int r = 0; r < iRows; r++)
	{
		for(int c = 0; c < iCols; c++)
		{
			QWidget * it = m_pTable->cellWidget(r, c);
			if(!it)
				continue;
			QString txt = it->toolTip();
			if(!txt.startsWith(szIco))
				continue;

			// good.
			if(txt.length() < iBestLen)
			{
				iBestR = r;
				iBestC = c;
				iBestLen = txt.length();
			}
		}
	}

	if(iBestR > -1)
		m_pTable->setCurrentCell(iBestR, iBestC);
}

void KviTextIconWindow::ownerDead()
{
	m_pOwner = nullptr;
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
	if(!m_pTable->cellWidget(row, column))
		return;

	QString szItem = m_pTable->cellWidget(row, column)->toolTip();

	if(m_bAltMode)
		szItem.prepend(KviControlCodes::Icon);

	if(KviInputEditor * pOwner = qobject_cast<KviInputEditor *>(m_pOwner); pOwner)
		pOwner->insertIconCode(szItem);
	else if(KviInput * pOwner = qobject_cast<KviInput *>(m_pOwner); pOwner)
		pOwner->insertText(QString("%1 ").arg(szItem));
	else if(QLineEdit * pOwner = qobject_cast<QLineEdit *>(m_pOwner); pOwner)
	{
		szItem.append(' ');
		QString szTmp = pOwner->text();
		szTmp.insert(pOwner->cursorPosition(), szItem);
		pOwner->setText(szTmp);
		pOwner->setCursorPosition(pOwner->cursorPosition() + szItem.length());
	}
	doHide();
}
