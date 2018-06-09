//=============================================================================
//
//   File : KviTalIconAndRichTextItemDelegate.cpp
//   Creation date : Mon May 05 2008 11:25:08 by Alessandro Carbone
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include "KviTalIconAndRichTextItemDelegate.h"

#include <QAbstractItemView>
#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QListWidget>
#include <QPainter>

#define LVI_AFTER_ICON (LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING)

KviTalIconAndRichTextItemDelegate::KviTalIconAndRichTextItemDelegate(QAbstractItemView * pWidget)
    : QStyledItemDelegate(pWidget), m_pParent(pWidget), m_oMinimumSize(0, 0), m_oIconSize(LVI_ICON_SIZE, LVI_ICON_SIZE)
{
}

KviTalIconAndRichTextItemDelegate::~KviTalIconAndRichTextItemDelegate()
    = default;

void KviTalIconAndRichTextItemDelegate::paint(QPainter * pPainter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	pPainter->save();
	QStyleOptionViewItem opt = option;
	initStyleOption(&opt, index);

	if(opt.state & QStyle::State_Selected)
		QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, pPainter);

	QString szText = index.data(Qt::DisplayRole).toString();
	QPixmap pixmap;
	QRect decorationRect;
	QVariant value = index.data(Qt::DecorationRole);

	QIcon ico;

	QPixmap pix;

	if(value.canConvert<QIcon>())
	{
		ico = QIcon(value.value<QIcon>());
		if(!ico.isNull())
			pix = ico.pixmap(m_oIconSize);
		else
			pix = m_oDefaultPix;
	}
	else
	{
		pix = m_oDefaultPix;
	}

	if(!pix.isNull())
	{
		int x = opt.rect.x() + LVI_BORDER;
		int y = opt.rect.y() + LVI_BORDER;
		int w = m_oIconSize.width();

		pPainter->drawPixmap(
		    x + ((w - pix.width()) / 2),
		    y,
		    pix);
	}

	QTextDocument doc;
	doc.setHtml(szText);
	doc.setDefaultFont(opt.font);

	int iIconAndSpace = LVI_BORDER + m_oIconSize.width() + LVI_SPACING;

	pPainter->translate(opt.rect.x() + iIconAndSpace, opt.rect.y() + LVI_BORDER);
	doc.setTextWidth(opt.rect.width() - iIconAndSpace - LVI_BORDER);
	QRect cliprect = QRect(QPoint(0, 0), QSize(opt.rect.width() - iIconAndSpace, opt.rect.height()));
	doc.drawContents(pPainter, cliprect);
	pPainter->restore();
}

QSize KviTalIconAndRichTextItemDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QString szText = index.data(Qt::DisplayRole).toString();
	QTextDocument doc;
	doc.setHtml(szText);
	doc.setDefaultFont(option.font);
	doc.setTextWidth(((QListWidget *)parent())->viewport()->width() - LVI_AFTER_ICON - LVI_BORDER);
	int iHeight = doc.documentLayout()->documentSize().toSize().height();

	//qDebug("Size hint (%d,%d)",((QListWidget *)parent())->minimumWidth(), iHeight + (2 * LVI_BORDER));

	int iIconWidth = m_oIconSize.width() + (2 * LVI_BORDER);
	int iIconHeight = m_oIconSize.height() + (2 * LVI_BORDER);

	int w = ((QListWidget *)parent())->minimumWidth();
	if(w < iIconWidth)
		w = iIconWidth;
	if(w < m_oMinimumSize.width())
		w = m_oMinimumSize.width();
	int h = iHeight + (2 * LVI_BORDER);
	if(h < iIconHeight)
		h = iIconHeight;
	if(h < m_oMinimumSize.height())
		h = m_oMinimumSize.height();

	return { w, h };
}
