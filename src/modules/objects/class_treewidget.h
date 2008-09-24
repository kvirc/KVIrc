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

	bool function_addColumn(KviKvsObjectFunctionCall *c);
	bool function_setColumnText(KviKvsObjectFunctionCall *c);
	bool function_takeItem(KviKvsObjectFunctionCall *c);
	bool function_setSorting(KviKvsObjectFunctionCall *c);
	bool function_setRootIsDecorated(KviKvsObjectFunctionCall *c);
	bool function_clear(KviKvsObjectFunctionCall *c);
	bool function_setAllColumnsShowFocus(KviKvsObjectFunctionCall *c);
	bool function_itemClickedEvent(KviKvsObjectFunctionCall *c);
	bool function_selectionChangedEvent(KviKvsObjectFunctionCall *c);
	bool function_currentChangedEvent(KviKvsObjectFunctionCall *c);
	bool function_itemActivatedEvent(KviKvsObjectFunctionCall *c);
	bool function_onItemEvent(KviKvsObjectFunctionCall *c);
	bool function_itemExpandedEvent(KviKvsObjectFunctionCall *c);
	bool function_itemCollapsedEvent(KviKvsObjectFunctionCall *c);
	bool function_itemChangedEvent(KviKvsObjectFunctionCall *c);
	bool function_customContextMenuRequestedEvent(KviKvsObjectFunctionCall *c);
	bool function_selectedItems(KviKvsObjectFunctionCall *c);
	bool function_currentItem(KviKvsObjectFunctionCall *c);
	bool function_setSelectionMode(KviKvsObjectFunctionCall *c);
	bool function_firstChild(KviKvsObjectFunctionCall *c);
	bool function_listViewHeaderIsVisible(KviKvsObjectFunctionCall *c);
	bool function_showListViewHeader(KviKvsObjectFunctionCall *c);
	bool function_hideListViewHeader(KviKvsObjectFunctionCall *c);
	bool function_setAcceptDrops(KviKvsObjectFunctionCall *c);
	bool function_setHeaderLabels(KviKvsObjectFunctionCall *c);
	bool function_setColumnCount(KviKvsObjectFunctionCall *c);
protected slots:
	void slotClicked(KviTalTreeWidgetItem *,int);
	void customContextMenuRequested(const QPoint &pnt);
	void slotSelectionChanged();
	void slotCurrentChanged(KviTalTreeWidgetItem *,KviTalTreeWidgetItem *);
	void slotItemActivated(KviTalTreeWidgetItem *,int col);
	void slotOnItemEntered(KviTalTreeWidgetItem *,int col);
	void slotItemExpanded(KviTalTreeWidgetItem *);
	void slotItemCollapsed(KviTalTreeWidgetItem *);
	void slotItemChanged(KviTalTreeWidgetItem *,int);
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
