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
void KviTalIconViewItemDelegate::drawDisplay ( QPainter * painter, const QStyleOptionViewItem & option, const QRect & rect, const QString & text ) const
{
	painter->save();
	QTextDocument doc;
	doc.setHtml( text );
	QAbstractTextDocumentLayout::PaintContext context;
	// option.rect.size()
    doc.setPageSize(rect.size());
    painter->translate(rect.x(),rect.y()+5);
	//(option.rect.x(), option.rect.y()
	//	doc.drawContents(painter);//
	doc.documentLayout()->draw(painter, context);
    painter->restore();
}

void KviTalIconViewItemDelegate::paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
/*	debug ("INTO DELEGATE col %d",index.column());
	painter->save();
	QPixmap pixmap;
    QRect decorationRect;
    QVariant value = index.data(Qt::DecorationRole);
	value.convert(QVariant::Pixmap);
	QPixmap *pix=(QPixmap*)value.data();
	painter->drawPixmap(int(1),int(1),*pix);
	painter->restore();
	*/
	QItemDelegate::paint(painter,option,index);
}

QSize KviTalIconViewItemDelegate::sizeHint( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	// FIXME: maybe we need an accurate value?
	return QSize(70,40);
	
}


KviTalIconView::KviTalIconView(QWidget * pParent,Qt::WFlags f)
: QTableWidget(pParent)
{
	setWindowFlags(f);
	horizontalHeader()->hide();
	verticalHeader()->hide();
	setShowGrid(false);
	m_pDelegate=new KviTalIconViewItemDelegate(this);
	setItemDelegate(m_pDelegate);
	connect(this,SIGNAL(itemDoubleClicked(QTableWidgetItem *)),this,SLOT(redirect_doubleClicked(QTableWidgetItem *)));
	
/*
	connect(this,SIGNAL(clicked(QListWidgetItem *)),this,SLOT(redirect_clicked(QListWidgetItem *)));
	

	connect(this,SIGNAL(selectionChanged(Q3IconViewItem *)),this,SLOT(redirect_selectionChanged(Q3IconViewItem *)));
	connect(this,SIGNAL(currentChanged(Q3IconViewItem *)),this,SLOT(redirect_currentChanged(Q3IconViewItem *)));
	connect(this,SIGNAL(clicked(Q3IconViewItem *,const QPoint &)),this,SLOT(redirect_clicked(Q3IconViewItem *,const QPoint &)));
	connect(this,SIGNAL(pressed(Q3IconViewItem *)),this,SLOT(redirect_pressed(Q3IconViewItem *)));
	connect(this,SIGNAL(pressed(Q3IconViewItem *,const QPoint &)),this,SLOT(redirect_pressed(Q3IconViewItem *,const QPoint &)));
	connect(this,SIGNAL(doubleClicked(Q3IconViewItem *)),this,SLOT(redirect_doubleClicked(Q3IconViewItem *)));
	connect(this,SIGNAL(returnPressed(Q3IconViewItem *)),this,SLOT(redirect_returnPressed(Q3IconViewItem *)));
	connect(this,SIGNAL(rightButtonClicked(Q3IconViewItem *,const QPoint &)),this,SLOT(redirect_rightButtonClicked(Q3IconViewItem *,const QPoint &)));
	connect(this,SIGNAL(rightButtonPressed(Q3IconViewItem *,const QPoint &)),this,SLOT(redirect_rightButtonPressed(Q3IconViewItem *,const QPoint &)));
	connect(this,SIGNAL(mouseButtonClicked(int,Q3IconViewItem *,const QPoint &)),this,SLOT(redirect_mouseButtonClicked(int,Q3IconViewItem *,const QPoint &)));
	connect(this,SIGNAL(mouseButtonPressed(int,Q3IconViewItem *,const QPoint &)),this,SLOT(redirect_mouseButtonPressed(int,Q3IconViewItem *,const QPoint &)));
	connect(this,SIGNAL(contextMenuRequested(Q3IconViewItem *,const QPoint &)),this,SLOT(redirect_contextMenuRequested(Q3IconViewItem *,const QPoint &)));
	connect(this,SIGNAL(onItem(Q3IconViewItem *)),this,SLOT(redirect_onItem(Q3IconViewItem *)));
	*/
}
void KviTalIconView::redirect_doubleClicked(QTableWidgetItem * pItem)
{
	emit doubleClicked((KviTalIconViewItem *)pItem);
}
/*
void KviTalIconView::redirect_selectionChanged(Q3IconViewItem * pItem)
{
	emit selectionChanged((KviTalIconViewItem *)pItem);
}

void KviTalIconView::redirect_currentChanged(Q3IconViewItem * pItem)
{
	emit currentChanged((KviTalIconViewItem *)pItem);
}

void KviTalIconView::redirect_clicked(Q3IconViewItem * pItem)
{
	emit clicked((KviTalIconViewItem *)pItem);
}

void KviTalIconView::redirect_clicked(Q3IconViewItem * pItem,const QPoint &pnt)
{
	emit clicked((KviTalIconViewItem *)pItem,pnt);
}

void KviTalIconView::redirect_pressed(Q3IconViewItem * pItem)
{
	emit pressed((KviTalIconViewItem *)pItem);
}

void KviTalIconView::redirect_pressed(Q3IconViewItem * pItem,const QPoint &pnt)
{
	emit pressed((KviTalIconViewItem *)pItem,pnt);
}

void KviTalIconView::redirect_doubleClicked(Q3IconViewItem * pItem)
{
	emit doubleClicked((KviTalIconViewItem *)pItem);
}

void KviTalIconView::redirect_returnPressed(Q3IconViewItem * pItem)
{
	emit returnPressed((KviTalIconViewItem *)pItem);
}

void KviTalIconView::redirect_rightButtonClicked(Q3IconViewItem * pItem,const QPoint &pnt)
{
	emit rightButtonClicked((KviTalIconViewItem *)pItem,pnt);
}

void KviTalIconView::redirect_rightButtonPressed(Q3IconViewItem * pItem,const QPoint &pnt)
{
	emit rightButtonPressed((KviTalIconViewItem *)pItem,pnt);
}

void KviTalIconView::redirect_mouseButtonClicked(int iButton,Q3IconViewItem * pItem,const QPoint &pnt)
{
	emit mouseButtonClicked(iButton,(KviTalIconViewItem *)pItem,pnt);
}

void KviTalIconView::redirect_mouseButtonPressed(int iButton,Q3IconViewItem * pItem,const QPoint &pnt)
{
	emit mouseButtonPressed(iButton,(KviTalIconViewItem *)pItem,pnt);
}

void KviTalIconView::redirect_contextMenuRequested(Q3IconViewItem * pItem,const QPoint &pnt)
{
	emit contextMenuRequested((KviTalIconViewItem *)pItem,pnt);
}

void KviTalIconView::redirect_onItem(Q3IconViewItem * pItem)
{
	emit onItem((KviTalIconViewItem *)pItem);
}

*/
#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "kvi_tal_iconview.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
