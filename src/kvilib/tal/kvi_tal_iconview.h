#ifndef _KVI_TAL_ICONVIEW_H_
#define _KVI_TAL_ICONVIEW_H_

//=============================================================================
//
//   File : kvi_tal_iconview.h
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

/**
* \file kvi_tal_iconview.h
* \author Szymon Stefanek
* \brief Class for iconview
*/

#include "kvi_settings.h"
#include "kvi_qstring.h"

#include <QTableWidget>
#include <QItemDelegate>

class KviTalIconViewItem;
class KviTalIconViewItemDelegate;

/**
* \class KviTalApplication
* \brief Toolkit Abstraction Layer: iconview class
*/
class KVILIB_API KviTalIconView : public QTableWidget
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the iconview object
	* \param pParent The parent object
	* \param f The Qt flags
	* \return KviTalIconView
	*/
	KviTalIconView(QWidget * pParent, Qt::WFlags f = 0);

	/**
	* \brief Destroys the iconview object
	*/
	virtual ~KviTalIconView();
protected:
	KviTalIconViewItemDelegate * m_pDelegate;
public:

	/**
	* \brief Sets the pixmap for the object
	* \param pPix The pixmap :)
	* \return void
	*/
	void setPixmap(QPixmap * pPix);
protected slots:
	/**
	* \brief Called when a cell is activated
	* \param iRow The row of the cell
	* \param iCol The column of the cell
	* \return void
	*/
	void emitCellActivated(int iRow, int iCol);

	/**
	* \brief Called when the current item changes
	* \param pItem The new item
	* \param pPrev The previous item
	* \return void
	*/
	void emitCurrentItemChanged(QTableWidgetItem * pItem, QTableWidgetItem * pPrev);
signals:
	/**
	* \brief Emitted when a cell is activated
	* \param pItem The activated item
	* \return void
	*/
	void cellActivated(KviTalIconViewItem * pItem);

	/**
	* \brief Emitted when the current item changes
	* \param pItem The new item
	* \param pPrev The previous item
	* \return void
	*/
	void currentItemChanged(KviTalIconViewItem * pItem, KviTalIconViewItem * pPrev);
};

/**
* \class KviTalIconViewItem
* \brief Toolkit Abstraction Layer: iconviewitem class
*/
class KVILIB_API KviTalIconViewItem : public QTableWidgetItem
{
public:
	/**
	* \brief Constructs an iconviewitem object
	* \param szText The text of the cell item
	* \param icon The icon of the item
	* \return KviTalIconViewItem
	*/
	KviTalIconViewItem(QString szText, const QIcon & icon);

	/**
	* \brief Destroys an iconviewitem object
	*/
	~KviTalIconViewItem();
};

/**
* \class KviTalIconViewItemDelegate
* \brief Toolkit Abstraction Layer: iconviewitemdelegate class
*/
class KviTalIconViewItemDelegate : public QItemDelegate
{
public:
	/**
	* \brief Constructs an iconviewitemdelegate object
	* \param pTableWidget The parent tablewidget object
	* \return KviTalIconViewItemDelegate
	*/
	KviTalIconViewItemDelegate(QTableWidget * pTableWidget);

	/**
	* \brief Destroys an iconviewitemdelegate object
	*/
	~KviTalIconViewItemDelegate();
protected:
	QTableWidget * m_pTableWidget;
public:
	/**
	* \brief Paints the view
	* \param pPainter The painter to draw the view
	* \param option The options of the item
	* \param index The model index of the item
	* \return void
	*/
	void paint(QPainter * pPainter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
};
#endif // _KVI_TAL_ICONVIEW_H_
