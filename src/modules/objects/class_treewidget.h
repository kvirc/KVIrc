#ifndef _CLASS_TREEWIDGET_H_
#define _CLASS_TREEWIDGET_H_
//=============================================================================
//
//   File : class_treewidget.h
//   Creation date : Fri Jan 28 14:21:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2005-2008 Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
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

#include "class_widget.h"
#include "class_treewidgetitem.h"
#include "object_macros.h"

#include "kvi_tal_treewidget.h"

class KviKvsObject_treewidget : public KviKvsObject_widget
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_treewidget)

public:
	QWidget * widget() { return (QWidget *)object(); };
	void fileDropped(QString &,KviTalTreeWidgetItem *);
protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);

	bool addColumn(KviKvsObjectFunctionCall *c);
	bool setColumnText(KviKvsObjectFunctionCall *c);
	bool takeItem(KviKvsObjectFunctionCall *c);
	bool setSorting(KviKvsObjectFunctionCall *c);
	bool setRootIsDecorated(KviKvsObjectFunctionCall *c);
	bool clear(KviKvsObjectFunctionCall *c);
	bool setAllColumnsShowFocus(KviKvsObjectFunctionCall *c);
	bool itemClickedEvent(KviKvsObjectFunctionCall *c);
	bool selectionChangedEvent(KviKvsObjectFunctionCall *c);
	bool currentChangedEvent(KviKvsObjectFunctionCall *c);
	bool itemActivatedEvent(KviKvsObjectFunctionCall *c);
	bool onItemEvent(KviKvsObjectFunctionCall *c);
	bool itemExpandedEvent(KviKvsObjectFunctionCall *c);
	bool itemCollapsedEvent(KviKvsObjectFunctionCall *c);
	bool itemChangedEvent(KviKvsObjectFunctionCall *c);
	bool customContextMenuRequestedEvent(KviKvsObjectFunctionCall *c);
	bool selectedItems(KviKvsObjectFunctionCall *c);
	bool currentItem(KviKvsObjectFunctionCall *c);
	bool setSelectionMode(KviKvsObjectFunctionCall *c);
	bool firstChild(KviKvsObjectFunctionCall *c);
	bool listViewHeaderIsVisible(KviKvsObjectFunctionCall *c);
	bool showListViewHeader(KviKvsObjectFunctionCall *c);
	bool hideListViewHeader(KviKvsObjectFunctionCall *c);
	bool setAcceptDrops(KviKvsObjectFunctionCall *c);
	bool setHeaderLabels(KviKvsObjectFunctionCall *c);
	bool setColumnCount(KviKvsObjectFunctionCall *c);
protected slots:
	void slotClicked(KviTalTreeWidgetItem *,int);
	void slotCustomContextMenuRequested(const QPoint &pnt);
	void slotSelectionChanged();
	void slotCurrentChanged(QTreeWidgetItem *,QTreeWidgetItem *);
	void slotItemActivated(QTreeWidgetItem *,int col);
	void slotOnItemEntered(QTreeWidgetItem *,int col);
	void slotItemExpanded(QTreeWidgetItem *);
	void slotItemCollapsed(QTreeWidgetItem *);
	void slotItemChanged(QTreeWidgetItem *,int);
};

class KviKvsTreeWidget : public KviTalTreeWidget
{
	Q_OBJECT
public:
	KviKvsTreeWidget(QWidget * par, const char * name, KviKvsObject_treewidget *);
	virtual ~KviKvsTreeWidget();
protected:
	KviKvsObject_treewidget * m_pParentScript;
protected:
	void dropEvent(QDropEvent * e);
	void dragEnterEvent(QDragEnterEvent * e);
};

#endif // _CLASS_TREEWIDGET_H_
