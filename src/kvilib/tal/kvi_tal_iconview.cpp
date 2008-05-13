//=============================================================================
//
//   File : kvi_tal_iconview.cpp
//   Creation date : Mon Jan 22 2007 11:25:08 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)
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

#define __KVILIB__

#include "kvi_tal_iconview.h"
#include <QTableWidget>
#include <QHeaderView>
#include <QSize>
#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QStyle>

/*
void KviTalIconViewItemDelegate::drawDisplay ( QPainter * painter, const QStyleOptionViewItem & option, const QRect & rect, const QString & text ) const
{

	painter->save();
	QTextDocument doc;
	doc.setHtml( text );
	QAbstractTextDocumentLayout::PaintContext context;
	doc.setPageSize(rect.size());
	painter->translate(rect.x(),rect.y());
	doc.documentLayout()->draw(painter, context);
	painter->restore();
}
*/
void KviTalIconViewItemDelegate::paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	painter->save();
		QString text="<center>";
	text+=index.data(Qt::DisplayRole).toString();
	text +="</center>";

	QPixmap pixmap;
	QRect decorationRect;
	QVariant value = index.data(Qt::DecorationRole);
	QStyle::State state=option.state;
	QRect rect=option.rect;


	int iconx=option.rect.x()+(option.rect.width()/2);
	iconx-=8;
	QIcon ico=QIcon(value.value<QIcon>());
	QRect rect2=QRect(QPoint(iconx,option.rect.y()),QSize(16,16));
	painter->drawPixmap(rect2,ico.pixmap(QSize(16,16)));


	if (option.state & QStyle::State_Selected)
	{
		QPalette pal=option.palette;
		QBrush brush=pal.highlight();
		QColor col=brush.color();
		col.setAlpha(127);
		brush.setColor(col);
		painter->fillRect(rect2,brush);
	}

	painter->restore();
	painter->save();
	QAbstractTextDocumentLayout::PaintContext context;
	QTextDocument doc;
	doc.setHtml( text );
	painter->translate(option.rect.x()+5,option.rect.y()+14);
	doc.setTextWidth(option.rect.width()-10);
	doc.documentLayout()->draw(painter, context);

	if (option.state & QStyle::State_Selected)
	{
		QPalette pal=option.palette;
		QBrush brush=pal.highlight();
		QColor col=brush.color();
		col.setAlpha(127);
		brush.setColor(col);
		QRect textRect=QRect(QPoint(0,2),QSize(doc.documentLayout()->documentSize().width(),doc.documentLayout()->documentSize().height()-4));
		painter->fillRect(textRect,brush);
	}
	painter->restore();
}


/*
QSize KviTalIconViewItemDelegate::sizeHint( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{

}
*/

KviTalIconView::KviTalIconView(QWidget * pParent,Qt::WFlags f)
: QTableWidget(pParent)
{
	setSelectionMode(QAbstractItemView::SingleSelection);
	horizontalHeader()->hide();
	verticalHeader()->hide();
	setShowGrid(false);

	m_pDelegate=new KviTalIconViewItemDelegate(this);
	setItemDelegate(m_pDelegate);
	connect(this,SIGNAL(cellActivated(int,int)),this,SLOT(redirect_cellActivated(int,int)));
	connect(this,SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)),this,SLOT(redirect_currentItemChanged( QTableWidgetItem *, QTableWidgetItem *)));

}


void KviTalIconView::redirect_cellActivated(int row,int col)
{
	emit cellActivated((KviTalIconViewItem *)item(row,col));
}

void KviTalIconView::redirect_currentItemChanged(QTableWidgetItem * pItem,QTableWidgetItem * prev)
{
	emit currentItemChanged((KviTalIconViewItem *)pItem,(KviTalIconViewItem *)prev);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "kvi_tal_iconview.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
