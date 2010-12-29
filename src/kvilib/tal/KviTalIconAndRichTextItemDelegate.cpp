//=============================================================================
//
//   File : KviTalIconAndRichTextItemDelegate.cpp
//   Creation date : Mon May 05 2008 11:25:08 by Alessandro Carbone
//
//   This file is part of the KVIrc irc client distribution
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

#include "KviTalIconAndRichTextItemDelegate.h"

#include <QListWidget>
#include <QPainter>
#include <QApplication>
#include <QAbstractTextDocumentLayout>

KviTalIconAndRichTextItemDelegate::KviTalIconAndRichTextItemDelegate(QAbstractItemView * pWidget)
: QStyledItemDelegate(pWidget), m_pParent(pWidget), m_pDefaultPix(0)
{
}

KviTalIconAndRichTextItemDelegate::~KviTalIconAndRichTextItemDelegate()
{
}

void KviTalIconAndRichTextItemDelegate::paint(QPainter * pPainter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	pPainter->save();
	QStyleOptionViewItemV4 opt = option;
	initStyleOption(&opt, index);

	if (opt.state & QStyle::State_Selected)
		QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem,&opt, pPainter);

	QString szText = index.data(Qt::DisplayRole).toString();
	QPixmap pixmap;
	QRect decorationRect;
	QVariant value = index.data(Qt::DecorationRole);

	QIcon ico;
	if(value.canConvert<QIcon>())
	{
		ico = QIcon(value.value<QIcon>());
		if(ico.isNull())
		{
			if(m_pDefaultPix)
				pPainter->drawPixmap(opt.rect.x()+LVI_BORDER,opt.rect.y()+LVI_BORDER,*m_pDefaultPix);
		} else {
			pPainter->drawPixmap(opt.rect.x()+LVI_BORDER,opt.rect.y()+LVI_BORDER,ico.pixmap(LVI_ICON_SIZE,LVI_ICON_SIZE));
		}
	} else {
		if(m_pDefaultPix)
			pPainter->drawPixmap(opt.rect.x()+LVI_BORDER,opt.rect.y()+LVI_BORDER,*m_pDefaultPix);
	}

	QTextDocument doc;
	doc.setHtml(szText);
	doc.setDefaultFont(opt.font);
	pPainter->translate(opt.rect.x()+LVI_AFTER_ICON,opt.rect.y()+LVI_BORDER);
	doc.setTextWidth(opt.rect.width()-LVI_AFTER_ICON);
	QRect cliprect = QRect(QPoint(0,0),QSize(opt.rect.width()-LVI_AFTER_ICON,opt.rect.height()));
	doc.drawContents(pPainter,cliprect);
	pPainter->restore();
}

QSize KviTalIconAndRichTextItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex & index) const
{
	QString szText = index.data(Qt::DisplayRole).toString();
	QTextDocument doc;
	doc.setHtml(szText);
	doc.setDefaultFont(option.font);
	doc.setTextWidth(option.rect.width()-LVI_AFTER_ICON);
	int iHeight = doc.documentLayout()->documentSize().toSize().height();
	if(iHeight < (LVI_ICON_SIZE+(2 * LVI_BORDER)))
		iHeight = LVI_ICON_SIZE;

	return QSize(((QListWidget *)parent())->viewport()->size().width(), iHeight + (2 * LVI_BORDER));
}
