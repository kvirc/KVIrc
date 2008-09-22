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
	inline KviTalTreeWidgetItem* topLevelItem(int i) { return (KviTalTreeWidgetItem*) QTreeWidget::topLevelItem(i); }
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
	KviTalTreeWidgetItem()
	: QTreeWidgetItem() {};
	KviTalTreeWidgetItem(KviTalTreeWidget * pParent)
	: QTreeWidgetItem(pParent) {};
	KviTalTreeWidgetItem(KviTalTreeWidgetItem * pParent)
	: QTreeWidgetItem(pParent) {};
	KviTalTreeWidgetItem(KviTalTreeWidget * pParent, KviTalTreeWidgetItem * preceding)
	: QTreeWidgetItem(pParent, preceding) {};
	KviTalTreeWidgetItem(KviTalTreeWidgetItem * pParent, KviTalTreeWidgetItem * preceding)
	: QTreeWidgetItem(pParent, preceding) {};

	KviTalTreeWidgetItem(KviTalTreeWidgetItem * pParent,const QString &szLabel, const QString &szLabel1 = QString(), const QString &szLabel2 = QString(), const QString &szLabel3 = QString(), const QString &szLabel4 = QString())
	: QTreeWidgetItem(pParent) {
		setText(0,szLabel);
		if(!szLabel1.isEmpty())
		{
			setText(1,szLabel1);
			if(!szLabel2.isEmpty())
			{
				setText(2,szLabel2);
				if(!szLabel3.isEmpty())
				{
					setText(3,szLabel3);
					if(!szLabel4.isEmpty())
					{
						setText(4,szLabel4);
					}
				}
			}
		}
	};
	KviTalTreeWidgetItem(KviTalTreeWidget * pParent,const QString &szLabel, const QString &szLabel1 = QString(), const QString &szLabel2 = QString(), const QString &szLabel3 = QString(), const QString &szLabel4 = QString())
	: QTreeWidgetItem(pParent) {
		setText(0,szLabel);
		if(!szLabel1.isEmpty())
		{
			setText(1,szLabel1);
			if(!szLabel2.isEmpty())
			{
				setText(2,szLabel2);
				if(!szLabel3.isEmpty())
				{
					setText(3,szLabel3);
					if(!szLabel4.isEmpty())
					{
						setText(4,szLabel4);
					}
				}
			}
		}
	};
	virtual ~KviTalTreeWidgetItem() {};
	int compare ( KviTalTreeWidgetItem * i, int col, bool ascending ) const
	{
		QString key=text(col);
		return key.localeAwareCompare(i->text(col));
	};
	inline KviTalTreeWidget* treeWidget() { return (KviTalTreeWidget*) QTreeWidgetItem::treeWidget(); };
	inline KviTalTreeWidgetItem* parent() { return (KviTalTreeWidgetItem*) QTreeWidgetItem::parent(); }
	inline KviTalTreeWidgetItem* child(int i) { return (KviTalTreeWidgetItem*) QTreeWidgetItem::child(i); }
};

#endif // _KVI_TAL_TREEWIDGETW_H_
