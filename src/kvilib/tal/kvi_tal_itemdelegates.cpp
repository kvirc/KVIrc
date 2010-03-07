//=============================================================================
//
//   File : kvi_tal_itemdelegates.cpp
//   Creation date : Mon May 05 2008 11:25:08 by Alessandro Carbone
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include "kvi_tal_itemdelegates.h"

#include <QListWidget>
#include <QPainter>
#include <QAbstractTextDocumentLayout>

KviTalIconAndRichTextItemDelegate::KviTalIconAndRichTextItemDelegate(QAbstractItemView * pWidget)
: QItemDelegate(pWidget), m_pParent(pWidget), m_pDefaultPix(0)
{
}

KviTalIconAndRichTextItemDelegate::~KviTalIconAndRichTextItemDelegate()
{
}

void KviTalIconAndRichTextItemDelegate::paint(QPainter * pPainter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	pPainter->save();
	if(option.state & QStyle::State_Selected)
	{
		QPalette pal = option.palette;
		QBrush brush = pal.highlight();
		QColor col = brush.color();
		col.setAlpha(127);
		brush.setColor(col);
		pPainter->fillRect(option.rect,brush);
	}

	QString szText = index.data(Qt::DisplayRole).toString();
	QPixmap pixmap;
	QRect decorationRect;
	QVariant value = index.data(Qt::DecorationRole);
	QStyle::State state = option.state;
	QRect rect = option.rect;
	int iAfterIcon = LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING;

	QIcon ico;
	if(value.canConvert<QIcon>())
	{
		ico = QIcon(value.value<QIcon>());
		if(ico.isNull())
		{
			if(m_pDefaultPix)
				pPainter->drawPixmap(option.rect.x()+LVI_BORDER,option.rect.y()+LVI_BORDER,*m_pDefaultPix);
		} else {
			pPainter->drawPixmap(option.rect.x()+LVI_BORDER,option.rect.y()+LVI_BORDER,ico.pixmap(LVI_ICON_SIZE,LVI_ICON_SIZE));
		}
	} else {
		if(m_pDefaultPix)
			pPainter->drawPixmap(option.rect.x()+LVI_BORDER,option.rect.y()+LVI_BORDER,*m_pDefaultPix);
	}

	QTextDocument doc;
	doc.setHtml(szText);
	doc.setDefaultFont(option.font);
	pPainter->translate(option.rect.x()+iAfterIcon,option.rect.y()+LVI_BORDER);
	doc.setTextWidth(option.rect.width()-32);
	QRect cliprect = QRect(QPoint(0,0),QSize(option.rect.width()-iAfterIcon,option.rect.height()));
	doc.drawContents(pPainter,cliprect);
	pPainter->restore();
}

QSize KviTalIconAndRichTextItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex & index) const
{
	QString szText = index.data(Qt::DisplayRole).toString();
	QTextDocument doc;
	doc.setHtml(szText);
	doc.setDefaultFont(option.font);
	doc.setTextWidth(option.rect.width()-32);
	int iHeight = doc.documentLayout()->documentSize().toSize().height();
	if(iHeight < (LVI_ICON_SIZE+(2 * LVI_BORDER)))
		iHeight = LVI_ICON_SIZE;

	return QSize(((QListWidget *)parent())->viewport()->size().width(), iHeight + (2 * LVI_BORDER));
}
