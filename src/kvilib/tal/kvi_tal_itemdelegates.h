#ifndef _KVI_TAL_DELEGATES_H_
#define _KVI_TAL_DELEGATES_H_

//=============================================================================
//
//   File : kvi_tal_itemdelegates.h
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "kvi_settings.h"

#include <QItemDelegate>
#include <QListWidget>
#include <QTreeWidget>
#include <QAbstractItemView>

#define LVI_ICON_SIZE 32
#define LVI_BORDER 4
#define LVI_SPACING 8
#define LVI_MINIMUM_TEXT_WIDTH 300
#define LVI_MINIMUM_CELL_WIDTH (LVI_MINIMUM_TEXT_WIDTH + LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING + LVI_BORDER)


class KVILIB_API KviTalIconAndRichTextItemDelegate : public QItemDelegate
{
public:
	KviTalIconAndRichTextItemDelegate(QAbstractItemView * pWidget=0)
		: QItemDelegate(pWidget), m_pDefaultPix(0), m_pParent(pWidget) {};
	~KviTalIconAndRichTextItemDelegate(){};
	 QSize sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const;
	 void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
	 void setDefaultIcon(QPixmap *pix){m_pDefaultPix=pix;};
protected:
	QAbstractItemView *m_pParent;
	QPixmap *m_pDefaultPix;
};


#endif // _KVI_TAL_LISTWIDGET_H_
