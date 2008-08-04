#ifndef _KVI_TAL_TREEWIDGET_H_
#define _KVI_TAL_TREEWIDGET_H_

//=============================================================================
//
//   File : kvi_tal_treewidget.h
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

#include "kvi_settings.h"
#include "kvi_qstring.h"

#include <QTreeWidget>
#include <QHeaderView>

class KviTalTreeWidgetItem;


class KVILIB_API KviTalTreeWidget : public QTreeWidget
{
	friend class KviTalTreeWidgetItem;
	Q_OBJECT
public:
	KviTalTreeWidget(QWidget * pParent);
	virtual ~KviTalTreeWidget() {};
public:
	void addColumn(const QString label){ setHeaderLabel(label); };
	void addColumn(const QString label, const int width ){ setHeaderLabel(label); setColumnWidth(0, width); };
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
};


class KVILIB_API KviTalTreeWidgetItem : public QTreeWidgetItem
{
public:
	KviTalTreeWidgetItem(KviTalTreeWidget * pParent)
	: QTreeWidgetItem(pParent) {};
	KviTalTreeWidgetItem(KviTalTreeWidgetItem * pParent)
	: QTreeWidgetItem(pParent) {};
	KviTalTreeWidgetItem(KviTalTreeWidgetItem * pParent,const QString &szLabel)
	: QTreeWidgetItem(pParent) {
		setText(0,szLabel);
	};
	KviTalTreeWidgetItem(KviTalTreeWidget * pParent,const QString &szLabel)
		: QTreeWidgetItem(pParent) {
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
	virtual ~KviTalTreeWidgetItem() {};
};

#endif // _KVI_TAL_TREEWIDGETW_H_
