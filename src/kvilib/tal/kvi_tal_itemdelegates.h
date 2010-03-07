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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

/**
* \file kvi_tal_itemdelegates.h
* \author Szymon Stefanek
* \brief Class for item delegates
*/

#include "kvi_settings.h"

#include <QStyledItemDelegate>
#include <QListWidget>
#include <QTreeWidget>
#include <QAbstractItemView>

#define LVI_ICON_SIZE 32
#define LVI_BORDER 4
#define LVI_SPACING 8
#define LVI_MINIMUM_TEXT_WIDTH 300
#define LVI_MINIMUM_CELL_WIDTH (LVI_MINIMUM_TEXT_WIDTH + LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING + LVI_BORDER)

#define LVI_AFTER_ICON (LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING)
/**
* \class KviTalIconAndRichTextItemDelegate
* \brief Toolkit Abstraction Layer: itemdelegate class
*/
class KVILIB_API KviTalIconAndRichTextItemDelegate : public QStyledItemDelegate
{
public:
	/**
	* \brief Constructs the icon and rich text item delegate object
	* \param pWidget The item which we have to delegate for the paint
	* \return KviTalIconAndRichTextItemDelegate
	*/
	KviTalIconAndRichTextItemDelegate(QAbstractItemView * pWidget = 0);

	/**
	* \brief Destroys the icon and rich text item delegate object
	*/
	~KviTalIconAndRichTextItemDelegate();
protected:
	QAbstractItemView * m_pParent;
	QPixmap           * m_pDefaultPix;
public:
	/**
	* \brief Returns the size hint for the view
	* \param option The style options for the item
	* \param index The model index for the item
	* \return QSize
	*/
	QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;

	/**
	* \brief Paints the view
	* \param pPainter The painter to draw the view
	* \param option The style options for the item
	* \param index The model index for the item
	* \return void
	*/
	void paint(QPainter * pPainter, const QStyleOptionViewItem & option, const QModelIndex & index) const;

	/**
	* \brief Sets the default icon
	* \param pPix The icon pixmap
	* \return void
	*/
	void setDefaultIcon(QPixmap * pPix){ m_pDefaultPix = pPix; };
};

#endif // _KVI_TAL_LISTWIDGET_H_
