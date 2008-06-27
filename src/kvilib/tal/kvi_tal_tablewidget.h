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
	void repaint()
	{
		if(tableWidget())
			tableWidget()->viewport()->repaint(tableWidget()->visualItemRect(this));
	};
/*
	KviTalTableWidgetItem(KviTalTableWidgetItem * pParent,const QString &szLabel)
	: QTableWidgetItem(pParent) {
		setText(0,szLabel);
	};
	KviTalTableWidgetItem(KviTalTableWidget * pParent,const QString &szLabel)
		: QTableWidgetItem(pParent) {
	setText(0,szLabel);
	};

	KviTalTreeWidgetItem(KviTalTreeWidget * pParent,const QString &szLabel, const QString &szLabel1, const QString &szLabel2, const QString &szLabel3 )
	: QTreeWidgetItem(pParent) {
	setText(0,szLabel);
	setText(1,szLabel1);
	setText(2,szLabel2);
	setText(3,szLabel3);

	};
	KviTalTreeWidgetItem(KviTalTreeWidget * pParent,const QString &szLabel, const QString &szLabel1, const QString &szLabel2, const QString &szLabel3, const QString &szLabel4 )
	: QTreeWidgetItem(pParent) {
	setText(0,szLabel);
	setText(1,szLabel1);
	setText(2,szLabel2);
	setText(3,szLabel3);
	setText(4,szLabel4);
	};
		int compare ( KviTalTreeWidgetItem * i, int col, bool ascending ) const
	{
		QString key=text(col);
		return key.localeAwareCompare(i->text(col));
	};
	KviTalTreeWidget* treeWidget() { return (KviTalTreeWidget*) QTreeWidgetItem::treeWidget(); };
*/
};

#endif // _KVI_TAL_TABLEWIDGET_H_
