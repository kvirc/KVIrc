#ifndef _KVI_TAL_TREEWIDGET_H_
#define _KVI_TAL_TREEWIDGET_H_

//=============================================================================
//
//   File : kvi_tal_treewidget.h
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

/**
* \file kvi_tal_treewidget.h
* \author Szymon Stefanek
* \brief Toolkit Abstraction Layer class
*/

#include "kvi_settings.h"

#include <QTreeWidget>

class KviTalTreeWidgetItem;

/**
* \class KviTalTreeWidget
* \brief Toolkit Abstraction Layer: TreeWidget class
*/

class KVILIB_API KviTalTreeWidget : public QTreeWidget
{
	friend class KviTalTreeWidgetItem;
	Q_OBJECT
public:
	/**
	* \brief Constructs the treewidget object
	* \param pParent The parent object
	* \return KviTalTreeWidget
	*/
	KviTalTreeWidget(QWidget * pParent);

	/**
	* \brief Destroys the treewidget object
	* \return KviTalTreeWidget
	*/
	virtual ~KviTalTreeWidget() {};
public:

	/**
	* \brief Returns the top level item at index iIndex
	* \param iIndex Index of the wanted top level item
	* \return KviTalTreeWidgetItem*
	*/
	inline KviTalTreeWidgetItem* topLevelItem(int iIndex) { return (KviTalTreeWidgetItem*) QTreeWidget::topLevelItem(iIndex); }
signals:
	void currentItemChanged(KviTalTreeWidgetItem *,KviTalTreeWidgetItem *);
	void selectionChanged();

	void itemActivated(KviTalTreeWidgetItem *,int);
	void itemChanged(KviTalTreeWidgetItem *,int);
	void itemClicked(KviTalTreeWidgetItem *,int);

	void itemCollapsed(KviTalTreeWidgetItem *);
	void itemExpanded(KviTalTreeWidgetItem *);
	void itemDoubleClicked(KviTalTreeWidgetItem *,int);
	void itemEntered(KviTalTreeWidgetItem *,int);
	void itemPressed(KviTalTreeWidgetItem *,int);

protected slots:
	void redirect_selectionChanged();
	void redirect_currentItemChanged(QTreeWidgetItem *pItemCurr,QTreeWidgetItem *pItemPrev);
	void redirect_itemActivated(QTreeWidgetItem *pItem,int col);
	void redirect_itemChanged(QTreeWidgetItem *pItem,int col);
	void redirect_itemClicked(QTreeWidgetItem *pItem,int col);
	void redirect_itemCollapsed(QTreeWidgetItem *pItem);
	void redirect_itemExpanded(QTreeWidgetItem *pItem);
	void redirect_itemDoubleClicked(QTreeWidgetItem *pItem,int col);
	void redirect_itemEntered(QTreeWidgetItem *pItem,int col);
	void redirect_itemPressed(QTreeWidgetItem *pItem,int col);
};


class KVILIB_API KviTalTreeWidgetItem : public QTreeWidgetItem
{
public:
	/**
	* \brief Constructs a treewidgetitem object without a parent
	* \return KviTalTreeWidgetItem
	*/
	KviTalTreeWidgetItem(): QTreeWidgetItem() {};

	/**
	* \brief Constructs a top-level treewidgetitem object
	* \param pParent The parent TreeWidget
	* \return KviTalTreeWidgetItem
	*/
	KviTalTreeWidgetItem(KviTalTreeWidget * pParent): QTreeWidgetItem(pParent) {};

	/**
	* \brief Constructs a child treewidgetitem object
	* \param pParent The parent TreeWidgetItem
	* \return KviTalTreeWidgetItem
	*/
	KviTalTreeWidgetItem(KviTalTreeWidgetItem * pParent): QTreeWidgetItem(pParent) {};

	/**
	* \brief Constructs a top-level treewidgetitem object, inserting it after an existing treewidgetitem
	* \param pParent The parent TreeWidget
	* \param pPreceding The already existing TreeWidgetItem you want the new item to be placed after.
	* \return KviTalTreeWidgetItem
	*/
	KviTalTreeWidgetItem(KviTalTreeWidget * pParent, KviTalTreeWidgetItem * pPreceding): QTreeWidgetItem(pParent, pPreceding) {};

	/**
	* \brief Destroys the treewidgetitem object
	* \return KviTalTreeWidgetItem
	*/
	virtual ~KviTalTreeWidgetItem() {};

	/**
	* \brief Returns the Treewidget that contains the item
	* \return KviTalTreeWidget
	*/
	inline KviTalTreeWidget* treeWidget() { return (KviTalTreeWidget*) QTreeWidgetItem::treeWidget(); };

	/**
	* \brief Returns the parent TreewidgetItem
	* \return KviTalTreeWidgetItem
	*/
	inline KviTalTreeWidgetItem* parent() { return (KviTalTreeWidgetItem*) QTreeWidgetItem::parent(); }

	/**
	* \brief Returns the child TreewidgetItem at index iIndex
	* \return KviTalTreeWidgetItem
	*/
	inline KviTalTreeWidgetItem* child(int iIndex) { return (KviTalTreeWidgetItem*) QTreeWidgetItem::child(iIndex); }
};

#endif // _KVI_TAL_TREEWIDGETW_H_
