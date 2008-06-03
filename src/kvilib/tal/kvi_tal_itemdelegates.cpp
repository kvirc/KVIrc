//=============================================================================
//
//   File : kvi_tal_listbox.cpp
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
#include "kvi_tal_itemdelegates.h"
#include <QListWidget>
#include <QPainter>
#include <QAbstractTextDocumentLayout>

void KviTalIconAndRichTextItemDelegate::paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	painter->save();
	if (option.state & QStyle::State_Selected)
	{
		QPalette pal=option.palette;
		QBrush brush=pal.highlight();
		QColor col=brush.color();
		col.setAlpha(127);
		brush.setColor(col);
		painter->fillRect(option.rect,brush);
	}
	QString text=index.data(Qt::DisplayRole).toString();
	QPixmap pixmap;
	QRect decorationRect;
	QVariant value = index.data(Qt::DecorationRole);
	QStyle::State state=option.state;
	QRect rect=option.rect;
	int afterIcon = LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING;
	QIcon ico;
	if(value.canConvert<QIcon>())
	{
		ico= QIcon(value.value<QIcon>());
		if (ico.isNull())
		{
			if (m_pDefaultPix) painter->drawPixmap(option.rect.x()+LVI_BORDER,option.rect.y()+LVI_BORDER,*m_pDefaultPix);
		} else {
			painter->drawPixmap(option.rect.x()+LVI_BORDER,option.rect.y()+LVI_BORDER,ico.pixmap(LVI_ICON_SIZE,LVI_ICON_SIZE));
		}
	} else {
		painter->drawPixmap(option.rect.x()+LVI_BORDER,option.rect.y()+LVI_BORDER,ico.pixmap(LVI_ICON_SIZE,LVI_ICON_SIZE));
	}
	QTextDocument doc;
	doc.setHtml( text );
	doc.setDefaultFont(painter->font());
	painter->translate(option.rect.x()+afterIcon,option.rect.y()+LVI_BORDER);
	doc.setTextWidth(option.rect.width()-10);
	QRect cliprect=QRect(QPoint(0,0),QSize(option.rect.width()-afterIcon,option.rect.height()-(LVI_BORDER*2)-4));
	doc.drawContents(painter, cliprect);	
	painter->restore();
}

QSize KviTalIconAndRichTextItemDelegate::sizeHint( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	QString text=index.data(Qt::DisplayRole).toString();
	QTextDocument doc;
	doc.setHtml( text );
	int height=doc.documentLayout()->documentSize().height();
	if (height<(LVI_ICON_SIZE+(2 * LVI_BORDER))) height=LVI_ICON_SIZE;
	return QSize(((QListWidget*)parent())->viewport()->size().width(), height + (2 * LVI_BORDER));		
}



