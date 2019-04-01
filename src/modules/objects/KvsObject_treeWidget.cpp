//=============================================================================
//
//   File : KvsObject_treeWidget.cpp
//   Creation date : Fri Jan 28 14:21:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2008 Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
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

#include "KvsObject_treeWidget.h"

#include "KviError.h"
#include "kvi_debug.h"
#include "KviLocale.h"
#include "KviIconManager.h"
#include <QTreeWidget>

#include <QEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QHeaderView>
#include <QUrl>
#include <QMimeData>

/*
	@doc: listview
	@keyterms:
		listview widget class
	@title:
		listview class
	@type:
		class
	@short:
		A listview widget class
	@inherits:
		[class]widget[/class]
	@description:
		It can display and control a hierarchy of multi-column items, and provides the ability to add new items at any time.
		The items are added by creating children [class]listviewitem[/class] objects: simply allocating them with $new
		will add the items to the listview and simply deleting them will remove them.
		Allocating a [class]listviewtitem[/class] item2 as a child of item1 will insert it to the same listview creating
		a subtree of items spanning from item1. The subtree can be opened or closed by a simple click either
		on the parent item or on the little plus sign on the side of it (when [classfnc:listview]$setRootIsDecorated[/classfnc]
		is set to $true. The listview can be in Single, Multi, Extended or NoSelection selection mode.[br][br]
		In single selection mode there can be only one selected item at a time and the selected item is also
		the current item (this mode is the default). In Multi and Extended selection mode there can be multiple selected items
		and the current item is one of them. The difference between multi and extended is in the way
		that items can be selected by the mouse actions: experiment with the two modes :).
		The NoSelection mode has obviously no selection at all.
	@functions:
		!fn: $addColumn(<text_label:string>,[<width:integer>])
		Adds a width pixels wide column with the column header label to the list view.
		!fn: $setSorting(<column:integer>,<sort_order:string>)
		Sets the list view to be sorted by column in ascending order if sort_order is [i]ascending[/i] or descending order if it is [i]descending[/i].
		!fn: $setSortingEnabled(<bEnabled:boolean>)
		If <bEnabled> is true, user sorting is enabled for the tree. The default value is false.
		In order to avoid performance issues, it is recommended that sorting is enabled after inserting the items into the tree.
		!fn: $hideListViewHeader()
		Hide the listview column header.
		!fn: $showListViewHeader()
		Show the listview column header.
		!fn: <boolean> $isListViewHeaderVisible()
		Returns [b]1[/b] if the listview header is currently visible. Otherwise this function returns [b]0[/b].
		!fn: $setAllColumnsShowFocus(<bAllColumnsShowFocus:boolean>)
		When the argument is $true, causes the listview to display the focus and selection
		by highlighting all the columns of the item. When the argument is $false then
		only the first column is selected/highlighted.
		!fn: $setSelectionMode(<mode:string>)
		Sets the selection mode for this listview. <mode> can be one of [i]Single[/i], [i]NoSelection[/i], [i]Multi[/i] or [i]Extended[/i].
		!fn: <listviewitem> $selectedItems()
		Returns the currently selected [class]listviewitem[/class] or $null if no items are selected.
		This function works only if the list view is in single selection mode.
		!fn: <listviewitem:hobject> $itemAt(<x_global_pos:integer>,<y_global_pos:integer>)
		Returns the listviewitem object at the x,y globals coordinates or $null if no item at.
		!fn: <listviewitem> $currentItem()
		Returns the current [class]listviewitem[/class] or $null if no item is current at the moment.
		!fn: <listviewitem> $firstChild()
		Returns the first child [class]listviewitem[/class] of this listview or $null if there are no items at all.
		!fn: <listviewitem> $topLevelItem(<index:int>)
		Returns the <index> child [class]listviewitem[/class] of this listview or $null if it does not exist.
		!fn: <int> $topLevelItemCount()
		Returns the number of top level items of this listview.
		!fn: setAcceptDrops(<benabled:boolean>)
		If <bEnabled> is true, user can drop files for this listview. The default value is true.
		!fn: $clickEvent(<item:object>)
		This function is called when the user clicks in the list view.
		In its argument the [class]listviewitem[/class] object clicked or 0 if the user didn't click on an item.[br]
		The default implementation emits the [classfnc]$clicked[/classfnc]() signal.
		!fn: $selectionChangedEvent(<item:object>)
		This event handle whenever the set of selected items has changed.
		The argument is the newly selected item if the listview is in single selection mode.[br]
		When the listview is in multi or extended selection mode then item is always $null.[br]
		The default implementation emits the [classfnc]$selectionChanged[/classfnc]() signal.
		!fn: $currentChangedEvent(<item:object>)
		This event are called whenever the current item has changed.
		In its argument is the newly selected item or 0 if the change made no item current.[br]
		The default implementation emits the [classfnc]$currentChanged[/classfnc]() signal.
		!fn: $itemActivatedEvent(<item:object>)
		This signal is emitted when the user activates an item by single- or double-clicking or pressing Enter.[br]
		In its argument the current item.[br]
		The default implementation emits the [classfnc]$itemActivated[/classfnc]() signal.
		!fn: $spacePressedEvent(<item:object>)
		This function is called by the framework when the space key is pressed.[br]
		In its an argument in the current item.[br]
		The default implementation emits the [classfnc]$spacePressed[/classfnc]() signal.
		!fn: $onItemEvent(<item:object>)
		This event is called by framework when the user moves the mouse cursor onto item.
		The default implementation emits the [classfnc]$onItem[/classfnc]() signal.
		!fn: $itemExpandedEvent(<item:object>)
		This event is called when an item has been expanded, i.e. when the child of an item is shown.
		The default implementation emits the [classfnc]$expanded[/classfnc]() signal.
		!fn: $itemCollapsedEvent(<item:object>)
		This event is called when an item has been collapsed, i.e. when the child of an item is hidden.
		The default implementation emits the [classfnc]$collapsed[/classfnc]() signal.
		!fn: $itemChangedEvent(<item:object>,<col:integer>,<text:string>)
		This event is called when the item has been renamed in text, e.g. by in in-place renaming, in column col.[br]
		The default implementation emits the [classfnc]$itemChanged[/classfnc]() signal.
	@signals:
		!sg: $clicked()
		This signal is emitted by the default implementation of [classfnc]$clickEvent[/classfnc]().
		!sg: $selectionChanged()
		This signal is emitted by the default implementation of [classfnc]$selectionChangedEvent[/classfnc]().
		!sg: $currentChanged()
		This signal is emitted by the default implementation of [classfnc]$currentChangedEvent[/classfnc]().
		!sg: $itemActivated()
		This signal is emitted by the default implementation of [classfnc]$itemActivatedEvent[/classfnc]().
		!sg: $spacePressed()
		This signal is emitted by the default implementation of [classfnc]$spacePressedEvent[/classfnc]().
		!sg: $onItem()
		This signal is emitted by the default implementation of [classfnc]$onItemEvent[/classfnc]().
		!sg: $itemExpanded()
		This signal is emitted by the default implementation of [classfnc]$itemExpandedEvent[/classfnc]().
		!sg: $itemCollapsed()
		This signal is emitted by the default implementation of [classfnc]$itemCollapsedEvent[/classfnc]().
		!sg: $itemChanged()
		This signal is emitted by the default implementation of [classfnc]$itemChangedEvent[/classfnc]().
		!sg: $rightButtonClicked()
		This signal is emitted by the default implementation of [classfnc]$rightButtonClickedEvent[/classfnc]().
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_treeWidget, "listview", "widget")

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, setHeaderLabels)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, addColumn)

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, setColumnText)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, setColumnCount)

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, setSorting)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, setSortingEnabled)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, setRootIsDecorated)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, setAllColumnsShowFocus)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, clear)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, selectedItems)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, currentItem)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, setSelectionMode)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, listViewHeaderIsVisible)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, itemAt)

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, hideListViewHeader)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, showListViewHeader)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, firstChild)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, topLevelItem)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, topLevelItemCount)

//events
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, selectionChangedEvent);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, currentChangedEvent);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, itemActivatedEvent);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, onItemEvent);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, itemClickedEvent);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, itemExpandedEvent);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, itemCollapsedEvent);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, itemChangedEvent);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_treeWidget, setAcceptDrops);
KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KvsObject_treeWidget, "fileDroppedEvent")

KVSO_END_REGISTERCLASS(KvsObject_treeWidget)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_treeWidget, KvsObject_widget)

KVSO_END_CONSTRUCTOR(KvsObject_treeWidget)

KVSO_BEGIN_DESTRUCTOR(KvsObject_treeWidget)

KVSO_END_CONSTRUCTOR(KvsObject_treeWidget)

bool KvsObject_treeWidget::init(KviKvsRunTimeContext *, KviKvsVariantList *)
{
	setObject(new KviKvsTreeWidget(parentScriptWidget(), getName().toUtf8().data(), this), true);
	// hack for compatibility with "old" addColumn method;
	((QTreeWidget *)widget())->setColumnCount(0);

	connect(widget(), SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(slotClicked(QTreeWidgetItem *, int)));
	connect(widget(), SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
	connect(widget(), SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(slotCurrentChanged(QTreeWidgetItem *, QTreeWidgetItem *)));
	connect(widget(), SIGNAL(itemActivated(QTreeWidgetItem *, int)), this, SLOT(slotItemActivated(QTreeWidgetItem *, int)));
	connect(widget(), SIGNAL(itemEntered(QTreeWidgetItem *, int)), this, SLOT(slotOnItemEntered(QTreeWidgetItem *, int)));
	connect(widget(), SIGNAL(itemExpanded(QTreeWidgetItem *)), this, SLOT(slotItemExpanded(QTreeWidgetItem *)));
	connect(widget(), SIGNAL(itemCollapsed(QTreeWidgetItem *)), this, SLOT(slotItemCollapsed(QTreeWidgetItem *)));
	connect(widget(), SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(slotItemChanged(QTreeWidgetItem *, int)));
	return true;
}

KVSO_CLASS_FUNCTION(treeWidget, setHeaderLabels)
{
	QStringList columns;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("labels", KVS_PT_STRINGLIST, KVS_PF_OPTIONAL, columns)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QTreeWidget *)object())->setHeaderLabels(columns);
	return true;
}

KVSO_CLASS_FUNCTION(treeWidget, setColumnText)
{
	if(!widget())
		return true;
	QString szLabel;
	kvs_int_t iCol;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("column", KVS_PT_INT, 0, iCol)
	KVSO_PARAMETER("label", KVS_PT_STRING, 0, szLabel)
	KVSO_PARAMETERS_END(c)
	QTreeWidgetItem * header = ((QTreeWidget *)widget())->headerItem();
	header->setText(iCol, szLabel);
	return true;
}

KVSO_CLASS_FUNCTION(treeWidget, topLevelItem)
{
	if(!widget())
		return true;
	kvs_int_t iIdx;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("index", KVS_PT_INT, 0, iIdx)
	KVSO_PARAMETERS_END(c)
	QTreeWidgetItem * pItem = ((QTreeWidget *)widget())->topLevelItem(iIdx);
	if(!pItem)
		c->returnValue()->setHObject((kvs_hobject_t) nullptr);
	else
		c->returnValue()->setHObject(KvsObject_treeWidgetItem::itemToHandle(pItem));
	return true;
}
KVSO_CLASS_FUNCTION(treeWidget, itemAt)
{
	if(!widget())
		return true;
	kvs_int_t iXpos, iYpos;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x_pos", KVS_PT_INT, 0, iXpos)
	KVSO_PARAMETER("y_pos", KVS_PT_INT, 0, iYpos)
	KVSO_PARAMETERS_END(c)
	QPoint pPoint = ((QTreeWidget *)widget())->viewport()->mapFromGlobal(QPoint(iXpos, iYpos));
	QTreeWidgetItem * pItem = ((QTreeWidget *)widget())->itemAt(pPoint);
	if(!pItem)
		c->returnValue()->setHObject((kvs_hobject_t) nullptr);
	else
		c->returnValue()->setHObject(KvsObject_treeWidgetItem::itemToHandle(pItem));
	return true;
}
KVSO_CLASS_FUNCTION(treeWidget, topLevelItemCount)
{
	if(!widget())
		return true;
	c->returnValue()->setInteger(((QTreeWidget *)widget())->topLevelItemCount());
	return true;
}

KVSO_CLASS_FUNCTION(treeWidget, addColumn)
{
	if(!widget())
		return true;
	QString szLabel;
	kvs_int_t iW;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("label", KVS_PT_STRING, 0, szLabel)
	KVSO_PARAMETER("width", KVS_PT_INT, KVS_PF_OPTIONAL, iW)
	KVSO_PARAMETERS_END(c)
	int col = ((QTreeWidget *)widget())->columnCount();
	QTreeWidgetItem * header = ((QTreeWidget *)widget())->headerItem();
	header->setText(col, szLabel);
	if(iW)
		((QTreeWidget *)widget())->setColumnWidth(col, iW);
	col++;
	((QTreeWidget *)widget())->setColumnCount(col);
	//col++;
	return true;
}

KVSO_CLASS_FUNCTION(treeWidget, setAcceptDrops)
{
	bool bEnable;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bEnable", KVS_PT_BOOLEAN, 0, bEnable)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QTreeWidget *)object())->setAcceptDrops(bEnable);
	return true;
}

KVSO_CLASS_FUNCTION(treeWidget, clear)
{
	if(widget())
		((QTreeWidget *)object())->clear();
	return true;
}

KVSO_CLASS_FUNCTION(treeWidget, selectedItems)
{
	if(widget())
	{
		QList<QTreeWidgetItem *> list = ((QTreeWidget *)widget())->selectedItems();
		KviKvsArray * pArray = new KviKvsArray();
		c->returnValue()->setArray(pArray);
		for(int i = 0; i < list.count(); i++)
			pArray->set(i, new KviKvsVariant(KvsObject_treeWidgetItem::itemToHandle((QTreeWidgetItem *)list.at(i))));
	}
	else
		c->returnValue()->setHObject((kvs_hobject_t) nullptr);
	return true;
}

KVSO_CLASS_FUNCTION(treeWidget, firstChild)
{
	if(widget())
		c->returnValue()->setHObject(KvsObject_treeWidgetItem::itemToHandle((QTreeWidgetItem *)((QTreeWidget *)widget())->topLevelItem(0)));
	else
		c->returnValue()->setHObject((kvs_hobject_t) nullptr);
	return true;
}

KVSO_CLASS_FUNCTION(treeWidget, currentItem)
{
	if(widget())
		c->returnValue()->setHObject(KvsObject_treeWidgetItem::itemToHandle((QTreeWidgetItem *)((QTreeWidget *)widget())->currentItem()));
	else
		c->returnValue()->setHObject((kvs_hobject_t) nullptr);
	return true;
}

KVSO_CLASS_FUNCTION(treeWidget, setColumnCount)
{
	kvs_uint_t uCol;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("column", KVS_PT_UNSIGNEDINTEGER, 0, uCol)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QTreeWidget *)widget())->setColumnCount(uCol);
	return true;
}

KVSO_CLASS_FUNCTION(treeWidget, setSelectionMode)
{
	QString szMode;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("mode", KVS_PT_NONEMPTYSTRING, 0, szMode)
	KVSO_PARAMETERS_END(c)
	if(!widget())
		return true;
	if(KviQString::equalCI(szMode, "NoSelection"))
		((QTreeWidget *)widget())->setSelectionMode(QAbstractItemView::NoSelection);
	else if(KviQString::equalCI(szMode, "Multi"))
		((QTreeWidget *)widget())->setSelectionMode(QTreeWidget::MultiSelection);
	else if(KviQString::equalCI(szMode, "Extended"))
		((QTreeWidget *)widget())->setSelectionMode(QTreeWidget::ExtendedSelection);
	else if(KviQString::equalCI(szMode, "Single"))
		((QTreeWidget *)widget())->setSelectionMode(QTreeWidget::SingleSelection);
	else
		c->warning(__tr2qs_ctx("Invalid selection mode '%Q'", "objects"), &szMode);
	return true;
}

KVSO_CLASS_FUNCTION(treeWidget, setSorting)
{
	kvs_int_t iCol;
	QString szOrder;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("column", KVS_PT_INT, 0, iCol)
	KVSO_PARAMETER("sort_order", KVS_PT_STRING, 0, szOrder)
	KVSO_PARAMETERS_END(c)
	if(!widget())
		return true;
	if(KviQString::equalCI(szOrder, "ascending"))
		((QTreeWidget *)widget())->sortItems(iCol, Qt::AscendingOrder);
	else if(KviQString::equalCI(szOrder, "descending"))
		((QTreeWidget *)widget())->sortItems(iCol, Qt::DescendingOrder);
	else
		c->warning(__tr2qs_ctx("Unknown '%Q' sort order: switching to ascending order", "objects"), &szOrder);
	return true;
}
KVSO_CLASS_FUNCTION(treeWidget, setSortingEnabled)
{
	bool bEnables;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bEnables", KVS_PT_BOOLEAN, 0, bEnables)
	KVSO_PARAMETERS_END(c)
	if(!widget())
		return true;
	((QTreeWidget *)widget())->setSortingEnabled(bEnables);
	return true;
}

KVSO_CLASS_FUNCTION(treeWidget, setRootIsDecorated)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bEnabled", KVS_PT_BOOL, 0, bEnabled)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QTreeWidget *)widget())->setRootIsDecorated(bEnabled);
	return true;
}

KVSO_CLASS_FUNCTION(treeWidget, setAllColumnsShowFocus)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bAllColumnsShowFocus", KVS_PT_BOOL, 0, bEnabled)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QTreeWidget *)widget())->setAllColumnsShowFocus(bEnabled);
	return true;
}

KVSO_CLASS_FUNCTION(treeWidget, hideListViewHeader)
{
	((QTreeWidget *)widget())->header()->hide();
	return true;
}

KVSO_CLASS_FUNCTION(treeWidget, showListViewHeader)
{
	((QTreeWidget *)widget())->header()->show();
	return true;
}

KVSO_CLASS_FUNCTION(treeWidget, listViewHeaderIsVisible)
{
	c->returnValue()->setBoolean(((QTreeWidget *)widget())->header()->isVisible());
	return true;
}

KVSO_CLASS_FUNCTION(treeWidget, itemClickedEvent)
{
	emitSignal("itemClicked", c, c->params());
	return true;
}

void KvsObject_treeWidget::slotClicked(QTreeWidgetItem * i, int col)
{
	KviKvsVariant * column = new KviKvsVariant((kvs_int_t)col);

	KviKvsVariantList params(new KviKvsVariant(KvsObject_treeWidgetItem::itemToHandle(i)), column);
	callFunction(this, "itemClickedEvent", nullptr, &params);
}

KVSO_CLASS_FUNCTION(treeWidget, selectionChangedEvent)
{
	emitSignal("selectionChanged", c, c->params());
	return true;
}

void KvsObject_treeWidget::slotSelectionChanged()
{
	if(((QTreeWidget *)widget())->selectionMode() == QTreeWidget::SingleSelection)
	{
		QTreeWidgetItem * it = (QTreeWidgetItem *)((QTreeWidget *)widget())->currentItem();
		KviKvsVariantList params(new KviKvsVariant(KvsObject_treeWidgetItem::itemToHandle(it)));
		callFunction(this, "selectionChangedEvent", nullptr, &params);
	}
	else
	{
		KviKvsVariantList params(new KviKvsVariant((kvs_hobject_t) nullptr));
		callFunction(this, "selectionChangedEvent", nullptr, &params);
	}
}

KVSO_CLASS_FUNCTION(treeWidget, currentChangedEvent)
{
	emitSignal("currentChanged", c, c->params());
	return true;
}

void KvsObject_treeWidget::slotCurrentChanged(QTreeWidgetItem * i, QTreeWidgetItem * prev)
{
	KviKvsVariantList params(new KviKvsVariant(KvsObject_treeWidgetItem::itemToHandle(i)), new KviKvsVariant(KvsObject_treeWidgetItem::itemToHandle(prev)));
	callFunction(this, "currentChangedEvent", nullptr, &params);
}

KVSO_CLASS_FUNCTION(treeWidget, itemActivatedEvent)
{
	emitSignal("itemActivated", c, c->params());
	return true;
}

void KvsObject_treeWidget::slotItemActivated(QTreeWidgetItem * i, int col)
{
	KviKvsVariant * column = new KviKvsVariant((kvs_int_t)col);
	KviKvsVariantList params(new KviKvsVariant(KvsObject_treeWidgetItem::itemToHandle(i)), column);
	callFunction(this, "itemActivatedEvent", nullptr, &params);
}

KVSO_CLASS_FUNCTION(treeWidget, onItemEvent)
{
	emitSignal("onItem", c, c->params());
	return true;
}

void KvsObject_treeWidget::slotOnItemEntered(QTreeWidgetItem * i, int col)
{
	KviKvsVariant * column = new KviKvsVariant((kvs_int_t)col);

	KviKvsVariantList params(new KviKvsVariant(KvsObject_treeWidgetItem::itemToHandle(i)), column);
	callFunction(this, "onItemEvent", nullptr, &params);
}

KVSO_CLASS_FUNCTION(treeWidget, itemExpandedEvent)
{
	emitSignal("itemExpanded", c, c->params());
	return true;
}

void KvsObject_treeWidget::slotItemExpanded(QTreeWidgetItem * i)
{
	KviKvsVariantList params(new KviKvsVariant(KvsObject_treeWidgetItem::itemToHandle(i)));
	callFunction(this, "itemExpandedEvent", nullptr, &params);
}

KVSO_CLASS_FUNCTION(treeWidget, itemCollapsedEvent)
{
	emitSignal("itemCollapsed", c, c->params());
	return true;
}

void KvsObject_treeWidget::slotItemCollapsed(QTreeWidgetItem * i)
{
	KviKvsVariantList params(new KviKvsVariant(KvsObject_treeWidgetItem::itemToHandle(i)));
	callFunction(this, "itemCollapsedEvent", nullptr, &params);
}

KVSO_CLASS_FUNCTION(treeWidget, itemChangedEvent)
{
	emitSignal("itemChanged", c, c->params());
	return true;
}

void KvsObject_treeWidget::slotItemChanged(QTreeWidgetItem * item, int col)
{
	KviKvsVariant * column = new KviKvsVariant((kvs_int_t)col);
	KviKvsVariantList params(new KviKvsVariant(KvsObject_treeWidgetItem::itemToHandle(item)), column);
	callFunction(this, "itemChangedEvent", nullptr, &params);
}

void KvsObject_treeWidget::fileDropped(QString & szFile, QTreeWidgetItem * item)
{
	KviKvsVariant * file = new KviKvsVariant(szFile);
	KviKvsVariantList params(new KviKvsVariant(KvsObject_treeWidgetItem::itemToHandle(item)), file);
	callFunction(this, "fileDroppedEvent", nullptr, &params);
}

KviKvsTreeWidget::KviKvsTreeWidget(QWidget * par, const char *, KvsObject_treeWidget * parent)
    : QTreeWidget(par)
{
	m_pParentScript = parent;
	setAcceptDrops(true);
	setDragEnabled(true);
	setDropIndicatorShown(true);
	setDragDropMode(QAbstractItemView::DragDrop);
	viewport()->setAcceptDrops(true);
}

KviKvsTreeWidget::~KviKvsTreeWidget()
    = default;

void KviKvsTreeWidget::dragEnterEvent(QDragEnterEvent * e)
{
	if(!e->mimeData()->hasUrls())
	{
		e->ignore();
		return;
	}
	e->acceptProposedAction();
}

void KviKvsTreeWidget::dragMoveEvent(QDragMoveEvent *)
{
	// ?#!
}

void KviKvsTreeWidget::dropEvent(QDropEvent * e)
{
	qDebug("Drop event");
	QList<QUrl> list;
	if(e->mimeData()->hasUrls())
	{
		list = e->mimeData()->urls();

		if(!list.isEmpty())
		{
			QList<QUrl>::Iterator it = list.begin();
			for(; it != list.end(); ++it)
			{
				QUrl url = *it;
				QString path = url.toLocalFile();
				qDebug("path %s", path.toUtf8().data());
				QTreeWidgetItem * i = itemAt(e->pos());
				m_pParentScript->fileDropped(path, i);
			}
		}
	}
}
