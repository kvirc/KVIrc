#ifndef _CLASS_LISTVIEW_H_
#define	_CLASS_LISTVIEW_H_
//=================================================================================
//
//   File : class_listview.h
//   Creation date : Fri Jan 28 14:21:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2006 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2005-2006 Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
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
//=================================================================================

#include <qlistview.h>
#include <qdragobject.h>
#include "class_widget.h"
#include "class_listviewitem.h"

#include "object_macros.h"

class KviKvsObject_listview : public KviKvsObject_widget
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_listview)

public:
	QWidget * widget() { return (QWidget *)object(); };
	void fileDropped(QString &,QListViewItem *);
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
	bool function_returnPressedEvent(KviKvsObjectFunctionCall *c);
	bool function_spacePressedEvent(KviKvsObjectFunctionCall *c);
	bool function_onItemEvent(KviKvsObjectFunctionCall *c);
	bool function_itemExpandedEvent(KviKvsObjectFunctionCall *c);
	bool function_itemCollapsedEvent(KviKvsObjectFunctionCall *c);
	bool function_itemRenamedEvent(KviKvsObjectFunctionCall *c);
	bool function_rightButtonClickedEvent(KviKvsObjectFunctionCall *c);
	bool function_selectedItem(KviKvsObjectFunctionCall *c);
	bool function_currentItem(KviKvsObjectFunctionCall *c);
	bool function_setSelectionMode(KviKvsObjectFunctionCall *c);
	bool function_firstChild(KviKvsObjectFunctionCall *c);
	bool function_listViewHeaderIsVisible(KviKvsObjectFunctionCall *c);
	bool function_showListViewHeader(KviKvsObjectFunctionCall *c);
	bool function_hideListViewHeader(KviKvsObjectFunctionCall *c);

	bool function_setAcceptDrops(KviKvsObjectFunctionCall *c);

protected slots:
	void slotClicked(QListViewItem *);
	void slotSelectionChanged(QListViewItem *);
	void slotMultipleSelectionChanged();
	void slotCurrentChanged(QListViewItem *);
	void slotReturnPressed(QListViewItem *);
	void slotSpacePressed(QListViewItem *);
	void slotOnItem(QListViewItem *);
	void slotItemExpanded(QListViewItem *);
	void slotItemCollapsed(QListViewItem *);
	void slotItemRenamed(QListViewItem *,int,const QString &);
	void slotRightButtonClicked(QListViewItem * i,const QPoint &coor, int col);
};

class KviKvsMdmListView :  public QListView
{
	Q_OBJECT
public:
	KviKvsMdmListView(QWidget * par,const char * name,KviKvsObject_listview *);

	virtual ~KviKvsMdmListView();
protected:
	KviKvsObject_listview *m_pParentScript;
	void contentsDropEvent(QDropEvent *e);
	void contentsDragEnterEvent( QDragEnterEvent *e );

};
#endif	//!_CLASS_LISTVIEW_H_
