//=============================================================================
//
//   File : kvi_tal_iconview.cpp
//   Creation date : Mon Jan 22 2007 11:25:08 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007-2008 Szymon Stefanek (pragma at kvirc dot net)
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


#include "kvi_tal_iconview.h"

#include <QTableWidget>
#include <QHeaderView>
#include <QSize>
#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QStyle>

KviTalIconView::KviTalIconView(QWidget * pParent, Qt::WFlags)
: QTableWidget(pParent)
{
	setSelectionMode(QAbstractItemView::SingleSelection);
	horizontalHeader()->hide();
	verticalHeader()->hide();
	setShowGrid(false);

	m_pDelegate = new KviTalIconViewItemDelegate(this);
	setItemDelegate(m_pDelegate);

	connect(this,SIGNAL(cellActivated(int,int)),this,SLOT(emitCellActivated(int,int)));
	connect(this,SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)),this,SLOT(emitCurrentItemChanged(QTableWidgetItem *, QTableWidgetItem *)));
}

KviTalIconView::~KviTalIconView()
{
}

void KviTalIconView::emitCellActivated(int iRow, int iCol)
{
	emit cellActivated((KviTalIconViewItem *)item(iRow,iCol));
}

void KviTalIconView::emitCurrentItemChanged(QTableWidgetItem * pItem, QTableWidgetItem * pPrev)
{
	emit currentItemChanged((KviTalIconViewItem *)pItem,(KviTalIconViewItem *)pPrev);
}


KviTalIconViewItem::KviTalIconViewItem(QString szText, const QIcon & icon)
: QTableWidgetItem(icon,szText)
{
	setSizeHint(QSize(30,20));
}

KviTalIconViewItem::~KviTalIconViewItem()
{
}


KviTalIconViewItemDelegate::KviTalIconViewItemDelegate(QTableWidget * pTableWidget)
: QItemDelegate(pTableWidget), m_pTableWidget(pTableWidget)
{
}

KviTalIconViewItemDelegate::~KviTalIconViewItemDelegate()
{
}

void KviTalIconViewItemDelegate::paint(QPainter * pPainter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	pPainter->save();

	QString szText = "<center>";
	szText += index.data(Qt::DisplayRole).toString();
	szText += "</center>";

	QPixmap pixmap;
	QRect decorationRect;
	QVariant value = index.data(Qt::DecorationRole);
	QStyle::State state = option.state;
	QRect rect = option.rect;

	int iIconx = option.rect.x()+(option.rect.width()/2);
	iIconx -= 8;
	QIcon ico = QIcon(value.value<QIcon>());
	QRect rect2 = QRect(QPoint(iIconx,option.rect.y()),QSize(16,16));
	pPainter->drawPixmap(rect2,ico.pixmap(QSize(16,16)));

	if(option.state & QStyle::State_Selected)
	{
		QPalette pal = option.palette;
		QBrush brush = pal.highlight();
		QColor col = brush.color();

		col.setAlpha(127);
		brush.setColor(col);
		pPainter->fillRect(rect2,brush);
	}

	pPainter->restore();
	pPainter->save();

	QAbstractTextDocumentLayout::PaintContext context;
	QTextDocument doc;
	doc.setHtml(szText);
	pPainter->translate(option.rect.x()+5,option.rect.y()+14);
	doc.setTextWidth(option.rect.width()-10);
	doc.documentLayout()->draw(pPainter,context);

	if(option.state & QStyle::State_Selected)
	{
		QPalette pal = option.palette;
		QBrush brush = pal.highlight();
		QColor col = brush.color();
		col.setAlpha(127);
		brush.setColor(col);
		QRect textRect = QRect(QPoint(0,2),QSize(doc.documentLayout()->documentSize().toSize().width(),doc.documentLayout()->documentSize().toSize().height()-4));
		pPainter->fillRect(textRect,brush);
	}
	pPainter->restore();
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "kvi_tal_iconview.moc"
#endif //COMPILE_USE_STANDALONE_MOC_SOURCES
