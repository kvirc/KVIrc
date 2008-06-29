#ifndef _KVI_TAL_TABLEWIDGET_H_
#define _KVI_TAL_TABLEWIDGET_H_

//=============================================================================
//
//   File : kvi_tal_tablewidget.h
//   Creation date : Fri Jun 27 2008 10:00:08 by Fabio Bas
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2008 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_qstring.h"

#include <QTableWidget>
#include <QHeaderView>

class KviTalTableWidgetItem;

class KVILIB_API KviTalTableWidget : public QTableWidget
{
	friend class KviTalTableWidgetItem;
	Q_OBJECT
public:
	KviTalTableWidget(QWidget * pParent);
	virtual ~KviTalTableWidget() {};
public:
/*
signals:
	void currentItemChanged(KviTalTreeWidgetItem *,KviTalTreeWidgetItem *);
	void itemActivated(KviTalTreeWidgetItem *,int);
	void itemChanged(KviTalTreeWidgetItem *,int);
	void itemClicked(KviTalTreeWidgetItem *,int);
	
	void itemCollapsed(KviTalTreeWidgetItem *);
	void itemExpanded(KviTalTreeWidgetItem *);
	void itemDoubleClicked(KviTalTreeWidgetItem *,int);
	void itemEntered(KviTalTreeWidgetItem *,int);
	void itemPressed(KviTalTreeWidgetItem *,int);

protected slots:
	void redirect_currentItemChanged(QTreeWidgetItem *pItemCurr,QTreeWidgetItem *pItemPrev);
	void redirect_itemActivated(QTreeWidgetItem *pItem,int col);
	void redirect_itemChanged(QTreeWidgetItem *pItem,int col);
	void redirect_itemClicked(QTreeWidgetItem *pItem,int col);
	void redirect_itemCollapsed(QTreeWidgetItem *pItem);
	void redirect_itemExpanded(QTreeWidgetItem *pItem);
	void redirect_itemDoubleClicked(QTreeWidgetItem *pItem,int col);
	void redirect_itemEntered(QTreeWidgetItem *pItem,int col);
	void redirect_itemPressed(QTreeWidgetItem *pItem,int col);
*/
};


class KVILIB_API KviTalTableWidgetItem : public QTableWidgetItem
{
public:
	KviTalTableWidgetItem(KviTalTableWidget * pParent)
	: QTableWidgetItem()
	{
		pParent->insertRow(pParent->rowCount());
		pParent->setItem(pParent->rowCount()-1, 0, this);
	};

	KviTalTableWidgetItem(KviTalTableWidget * pParent, int row, int column)
	: QTableWidgetItem()
	{
		pParent->setItem(row, column, this);
	};

	KviTalTableWidgetItem(const KviTalTableWidgetItem & other)
	: QTableWidgetItem(other) {};
};

class KVILIB_API KviTalTableWidgetItemEx : public KviTalTableWidgetItem
{
public:
	KviTalTableWidgetItemEx(KviTalTableWidget * pParent)
	: KviTalTableWidgetItem(pParent) {};

	KviTalTableWidgetItemEx(KviTalTableWidget * pParent, int row, int column)
	: KviTalTableWidgetItem(pParent, row, column) {};

	KviTalTableWidgetItemEx(const KviTalTableWidgetItem & other)
	: KviTalTableWidgetItem(other) {};
	virtual void displayUpdate() = 0;
};

#endif // _KVI_TAL_TABLEWIDGET_H_
