//=================================================================================
//
//   File : class_listview.cpp
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
#include <qheader.h>
#include "class_listview.h"
#include "kvi_error.h"
#include "kvi_debug.h"

#include "kvi_locale.h"
#include "kvi_iconmanager.h"



/*
	@doc: listview
	@keyterms:
		listview widget class
	@title:
		listview class
	@type:
		class
	@short:
		A listview class
	@inherits:
		[class]widget[/class]
	@description:
		It can display and control a hierarchy of multi-column items, and provides the ability to add new items at any time.
		The items are added by creating children [class]listviewitem[/class] objects: simply allocating them with $new
		will add the items to the listview and simply deleting them will remove them.
		Allocating a [class]listviewitem[/class] item2 as a child of item1 will insert it to the same listview creating
		a subtree of items spannig from item1. The subtree can be opened or closed by a simple click either
		on the parent item or on the little plus sign on the side of it (when [classfnc:listview]$setRootIsDecorated[/classfnc]
		is set to $true. The listview can be in Single, Multi, Extended or NoSelection selection mode.
		In single selection mode there can be only one selected item at a time and the selected item is also
		the current item (this mode is the default). In Multi and Extended selection mode there can be multiple selected items
		and the current item is one of them. The difference between Multi and Extended is in the way
		that items can be selected by the mouse actions: experiment with the two modes :).
		The NoSelection mode has obviously no selection at all.
	@functions:
		!fn: $addColumn(<text_label:string> <width:unsigned integer>)
		Adds a width pixels wide column with the column header label to the list view.

		!fn: $setSorting(<column:integer>,<bAscending:boolean>)
		Sets the list view to be sorted by column column in ascending order if ascending is 1 or descending order if it is FALSE.
		If column is -1, sorting is disabled and the user cannot sort columns by clicking on the column headers

		!fn: $hideListViewHeader()
		Hide the listview column header.

		!fn: $showListViewHeader()
		Show the listview column header.

		!fn: <boolean> $isListViewHeaderVisible()
		Returns '1' if the listview header is currently visible. Otherwise this function returns '0'.

		!fn: $setAllColumnsShowFocus(<bAllColumnsShowFocus:boolean>)
		When the argument is $true, causes the listview to display the focus and selection
		by highlighting all the columns of the item. When the argument is $false then
		only the first column is selected/highlighted.

		!fn: $setSelectionMode(<mode:string>)
		Sets the selection mode for this listview. <mode> can be one of "Single","NoSelection","Multi" or "Extended".

		!fn: <listviewitem> $selectedItem()
		Returns the currently selected [class]listviewitem[/class] or $null if no items are selected.
		This function works only if the list view is in single selection mode.

		!fn: <listviewitem> $currentItem()
		Returns the current [class]listviewitem[/class] or $null if no item is current at the moment.

		!fn: <listviewitem> $firstChild()
		Returns the first child [class]listviewitem[/class] of this listview or $null if there are no items at all.

		!fn: $clickEvent(<item:object>)
		This function is called when the user clicks in the list view.
		In its argument the [class]listviewitem[/class] object clicked or 0 if the user didn't click on an item.[br]
		The default implementation emits the [classfnc]$clicked[/classfnc]() signal.

		!fn: $selectionChangedEvent(<item:object>)
		This event handle whenever the set of selected items has changed.
		The argument is the newly selected item if the listview is in single selection mode.[br]
		When the listview is in Multi or Extended selection mode then item is always $null.[br]
		The default implementation emits the [classfnc]$selectionChanged[/classfnc]() signal.

		!fn: $currentChangedEvent(<item:object>)
		This event are called whenever the current item has changed.
		In its argument is the newly selected item or 0 if the change made no item current.[br]
		The default implementation emits the [classfnc]$currentChanged[/classfnc]() signal.

		!fn: $returnPressedEvent(<item:object>)
		This function is called by the framework when the enter key is pressed.[br]
		In its arument the currrent item.[br]
		The default implementation emits the [classfnc]$returnPressed[/classfnc]() signal.

		!fn: $spacePressedEvent(<item:object>)
		This function is called by the framework when the space key is pressed.[br]
		In its arument the currrent item.[br]
		The default implementation emits the [classfnc]$spacePressed[/classfnc]() signal.

		!fn: $onItemEvent(<item:object)
		This event is called by framwork when the user moves the mouse cursor onto item.
		The default implementation emits the [classfnc]$onItem[/classfnc]() signal.

		!fn: $itemExpandedEvent(<item:object>)
		This event is called when an item has been expanded, i.e. when the children of item are shown.
		The default implementation emits the [classfnc]$expanded[/classfnc]() signal.

		!fn: $itemCollapsedEvent(<item:object>)
		This event is called when an item has been collapsed, i.e. when the children of item are hidden.
		The default implementation emits the [classfnc]$collapsed[/classfnc]() signal.

		!fn: $itemRenamedEvent(<item:object>,<col:integer>,<text:string>)
		This event is called when the item has been renamed in text, e.g. by in in-place renaming, in column col.[br]
		The default implementation emits the [classfnc]$itemRenamed[/classfnc]() signal.

		!fn: $rightButtonClickEvent(<item:object>,<x:integer>,>y:integer>)
		This signal is emitted when the right button is clicked.[br]
		The arguments are the relevant item (may be 0), the point in global coordinates and the relevant column (or -1 if the click was outside the list).
		The default implementation emits the [classfnc]$rightButtonClicked[/classfnc]() signal.
	@signals:
		!sg: $clicked()
		This signal is emitted by the default implementation of [classfnc]$clickEvent[/classfnc]().
		!sg: $selectionChanged()
		This signal is emitted by the default implementation of [classfnc]$selectionChangedEvent[/classfnc]().
		!sg: $currentChanged()
		This signal is emitted by the default implementation of [classfnc]$currentChangedEvent[/classfnc]().
		!sg: $returnPressed()
		This signal is emitted by the default implementation of [classfnc]$returnPressedEvent[/classfnc]().
		!sg: $spacePressed()
		This signal is emitted by the default implementation of [classfnc]$spacePredssedEvent[/classfnc]().
		!sg: $onItem()
		This signal is emitted by the default implementation of [classfnc]$onItemEvent[/classfnc]().
		!sg: $itemExpanded()
		This signal is emitted by the default implementation of [classfnc]$itemExpandedEvent[/classfnc]().
		!sg: $itemCollapsed()
		This signal is emitted by the default implementation of [classfnc]$itemCollapsedEvent[/classfnc]().
		!sg: $itemRenamed()
		This signal is emitted by the default implementation of [classfnc]$itemRenamedEvent[/classfnc]().
		!sg: $rightButtonClicked()
		This signal is emitted by the default implementation of [classfnc]$rightButtonClickedEvent[/classfnc]().


*/

//---------------------------------------------------------------------------------

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_listview,"listview","widget")

	KVSO_REGISTER_HANDLER(KviKvsObject_listview,"addColumn",function_addColumn)
	KVSO_REGISTER_HANDLER(KviKvsObject_listview,"setColumnText",function_setColumnText)
	KVSO_REGISTER_HANDLER(KviKvsObject_listview,"setSorting",function_setSorting)
	KVSO_REGISTER_HANDLER(KviKvsObject_listview,"setRootIsDecorated",function_setRootIsDecorated)
	KVSO_REGISTER_HANDLER(KviKvsObject_listview,"setAllColumnsShowFocus",function_setAllColumnsShowFocus)
	KVSO_REGISTER_HANDLER(KviKvsObject_listview,"clear",function_clear)
	KVSO_REGISTER_HANDLER(KviKvsObject_listview,"selectedItem",function_selectedItem)
	KVSO_REGISTER_HANDLER(KviKvsObject_listview,"currentItem",function_currentItem)
	KVSO_REGISTER_HANDLER(KviKvsObject_listview,"setSelectionMode",function_setSelectionMode)
	KVSO_REGISTER_HANDLER(KviKvsObject_listview,"listViewHeaderIsVisible",function_listViewHeaderIsVisible)


	KVSO_REGISTER_HANDLER(KviKvsObject_listview,"hideListViewHeader",function_hideListViewHeader)
	KVSO_REGISTER_HANDLER(KviKvsObject_listview,"showListViewHeader",function_showListViewHeader)
	KVSO_REGISTER_HANDLER(KviKvsObject_listview,"firstChild",function_firstChild)
	KVSO_REGISTER_HANDLER(KviKvsObject_listview,"selectionChangedEvent",function_selectionChangedEvent);
	KVSO_REGISTER_HANDLER(KviKvsObject_listview,"currentChangedEvent",function_currentChangedEvent);
	KVSO_REGISTER_HANDLER(KviKvsObject_listview,"returnPressedEvent",function_returnPressedEvent);
	KVSO_REGISTER_HANDLER(KviKvsObject_listview,"spacePressedEvent",function_spacePressedEvent);
	KVSO_REGISTER_HANDLER(KviKvsObject_listview,"onItemEvent",function_onItemEvent);
	KVSO_REGISTER_HANDLER(KviKvsObject_listview,"itemClickedEvent",function_itemClickedEvent);
	KVSO_REGISTER_HANDLER(KviKvsObject_listview,"itemExpandedEvent",function_itemExpandedEvent);
	KVSO_REGISTER_HANDLER(KviKvsObject_listview,"itemCollapsedEvent",function_itemCollapsedEvent);
	KVSO_REGISTER_HANDLER(KviKvsObject_listview,"itemRenamedEvent",function_itemRenamedEvent);
	KVSO_REGISTER_HANDLER(KviKvsObject_listview,"rightButtonClickedEvent",function_rightButtonClickedEvent);

	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_listview,"fileDroppedEvent")

KVSO_END_REGISTERCLASS(KviKvsObject_listview)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_listview,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_listview)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_listview)

KVSO_END_CONSTRUCTOR(KviKvsObject_listview)

bool KviKvsObject_listview::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{
	setObject(new KviKvsMdmListView(parentScriptWidget(),name(),this),true);
	connect(widget(),SIGNAL(clicked(QListViewItem *)),this,SLOT(slotClicked(QListViewItem *)));
	connect(widget(),SIGNAL(selectionChanged(QListViewItem *)),this,SLOT(slotSelectionChanged(QListViewItem *)));
	connect(widget(),SIGNAL(selectionChanged()),this,SLOT(slotMultipleSelectionChanged()));
	connect(widget(),SIGNAL(currentChanged(QListViewItem *)),this,SLOT(slotCurrentChanged(QListViewItem *)));
	connect(widget(),SIGNAL(returnPressed(QListViewItem *)),this,SLOT(slotReturnPressed(QListViewItem *)));
	connect(widget(),SIGNAL(spacePressed(QListViewItem *)),this,SLOT(slotSpacePressed(QListViewItem *)));
	connect(widget(),SIGNAL(onItem(QListViewItem *)),this,SLOT(slotOnItem(QListViewItem *)));
	connect(widget(),SIGNAL(expanded(QListViewItem *)),this,SLOT(slotItemExpanded(QListViewItem *)));
	connect(widget(),SIGNAL(collapsed(QListViewItem *)),this,SLOT(slotItemCollapsed(QListViewItem *)));
	connect(widget(),SIGNAL(rightButtonClicked(QListViewItem *,const QPoint &,int)),this,SLOT(slotRightButtonClicked(QListViewItem *,const QPoint &,int)));
	connect(widget(),SIGNAL(itemRenamed(QListViewItem *,int,const QString &)),this,SLOT(slotItemRenamed(QListViewItem *,int,const QString &)));
	return true;
}

bool KviKvsObject_listview::function_addColumn(KviKvsObjectFunctionCall *c)
{
	QString szLabel;
	kvs_int_t iW;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("label",KVS_PT_STRING,0,szLabel)
		KVSO_PARAMETER("width",KVS_PT_INT,0,iW)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QListView *)object())->addColumn(szLabel,iW);
    return true;
}
/*
bool KviKvsObject_listview::function_setAcceptDrops(KviKvsObjectFunctionCall *c)
{
	bool bEnable;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnable",KVS_PT_BOOLEAN,0,bEnable)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QListView *)object())->setAcceptDrops(bEnable);
    return true;
}
*/
bool KviKvsObject_listview::function_clear(KviKvsObjectFunctionCall *c)
{
	if (widget())
		((QListView *)object())->clear();
	return true;
}

bool KviKvsObject_listview::function_selectedItem(KviKvsObjectFunctionCall *c)
{
	if(widget())
		c->returnValue()->setHObject(KviKvsObject_listviewitem::itemToHandle(((QListView *)widget())->selectedItem()));
	else
		c->returnValue()->setHObject((kvs_hobject_t)0);
	return true;
}

bool KviKvsObject_listview::function_firstChild(KviKvsObjectFunctionCall *c)
{
	if(widget())
		c->returnValue()->setHObject(KviKvsObject_listviewitem::itemToHandle(((QListView *)widget())->firstChild()));
	else
		c->returnValue()->setHObject((kvs_hobject_t)0);
	return true;
}

bool KviKvsObject_listview::function_currentItem(KviKvsObjectFunctionCall *c)
{
	if(widget())
		c->returnValue()->setHObject(KviKvsObject_listviewitem::itemToHandle(((QListView *)widget())->currentItem()));
	else
		c->returnValue()->setHObject((kvs_hobject_t)0);
	return true;
}

bool KviKvsObject_listview::function_setColumnText(KviKvsObjectFunctionCall *c)
{
	kvs_int_t uCol;
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("column",KVS_PT_UNSIGNEDINTEGER,0,uCol)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
		KVSO_PARAMETERS_END(c)
	if (widget())((QListView *)widget())->setColumnText(uCol,szText);
    return true;
}

bool KviKvsObject_listview::function_setSelectionMode(KviKvsObjectFunctionCall *c)
{
	QString szMode;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("mode",KVS_PT_NONEMPTYSTRING,0,szMode)
		KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	if(KviQString::equalCI(szMode,"NoSelection"))
		((QListView *)widget())->setSelectionMode(QListView::NoSelection);
	else if(KviQString::equalCI(szMode,"Multi"))
		((QListView *)widget())->setSelectionMode(QListView::Multi);
	else if(KviQString::equalCI(szMode,"Extended"))
		((QListView *)widget())->setSelectionMode(QListView::Extended);
	else if(KviQString::equalCI(szMode,"Single"))
		((QListView *)widget())->setSelectionMode(QListView::Single);
	else c->warning(__tr2qs("Invalid selection mode 'Q'"),&szMode);
    return true;
}

bool KviKvsObject_listview::function_setSorting(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t iCol;
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("column",KVS_PT_UNSIGNEDINTEGER,0,iCol)
		KVSO_PARAMETER("benabled",KVS_PT_BOOL,0,bEnabled)
		KVSO_PARAMETERS_END(c)
	if (widget()) 	((QListView *)widget())->setSorting(iCol,bEnabled);
    return true;
}


bool KviKvsObject_listview::function_setRootIsDecorated(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if (widget()) 	((QListView *)widget())->setRootIsDecorated(bEnabled);
    return true;
}

bool KviKvsObject_listview::function_setAllColumnsShowFocus(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bAllColumnsShowFocus",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if (widget())((QListView *)widget())->setAllColumnsShowFocus(bEnabled);
	return true;
}

bool KviKvsObject_listview::function_hideListViewHeader(KviKvsObjectFunctionCall *c)
{
	((QListView *)widget())->header()->hide();
	return true;
}
bool KviKvsObject_listview::function_showListViewHeader(KviKvsObjectFunctionCall *c)
{
	((QListView *)widget())->header()->show();
	return true;
}
bool KviKvsObject_listview::function_listViewHeaderIsVisible(KviKvsObjectFunctionCall *c)
{
	c->returnValue()->setBoolean(((QListView *)widget())->header()->isVisible());
	return true;
}

bool KviKvsObject_listview::function_itemClickedEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("itemClicked",c,c->params());
	return true;
}

void KviKvsObject_listview::slotClicked(QListViewItem * i)
{
	KviKvsVariantList params(new KviKvsVariant(KviKvsObject_listviewitem::itemToHandle(i)));
	callFunction(this,"itemClickedEvent",0,&params);
}

bool KviKvsObject_listview::function_selectionChangedEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("selectionChanged",c,c->params());
	return true;
}

void KviKvsObject_listview::slotSelectionChanged(QListViewItem * i)
{
	KviKvsVariantList params(new KviKvsVariant(KviKvsObject_listviewitem::itemToHandle(i)));
	callFunction(this,"selectionChangedEvent",0,&params);
}

void KviKvsObject_listview::slotMultipleSelectionChanged()
{
	KviKvsVariantList params(new KviKvsVariant((kvs_hobject_t)0));
	callFunction(this,"selectionChangedEvent",0,&params);
}


bool KviKvsObject_listview::function_currentChangedEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("currentChanged",c,c->params());
	return true;
}

void KviKvsObject_listview::slotCurrentChanged(QListViewItem * i)
{
	KviKvsVariantList params(new KviKvsVariant(KviKvsObject_listviewitem::itemToHandle(i)));
	callFunction(this,"currentChangedEvent",0,&params);
}


bool KviKvsObject_listview::function_returnPressedEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("returnPressed",c,c->params());
	return true;
}

void KviKvsObject_listview::slotReturnPressed(QListViewItem * i)
{
	KviKvsVariantList params(new KviKvsVariant(KviKvsObject_listviewitem::itemToHandle(i)));
	callFunction(this,"returnPressedEvent",0,&params);
}

bool KviKvsObject_listview::function_spacePressedEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("spacePressed",c,c->params());
	return true;
}

void KviKvsObject_listview::slotSpacePressed(QListViewItem * i)
{
	KviKvsVariantList params(new KviKvsVariant(KviKvsObject_listviewitem::itemToHandle(i)));
	callFunction(this,"spacePressedEvent",0,&params);
}

bool KviKvsObject_listview::function_onItemEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("onItem",c,c->params());
	return true;
}

void KviKvsObject_listview::slotOnItem(QListViewItem * i)
{
	KviKvsVariantList params(new KviKvsVariant(KviKvsObject_listviewitem::itemToHandle(i)));
	callFunction(this,"onItemEvent",0,&params);
}


bool KviKvsObject_listview::function_itemExpandedEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("itemExpanded",c,c->params());
	return true;
}

void KviKvsObject_listview::slotItemExpanded(QListViewItem * i)
{
	KviKvsVariantList params(new KviKvsVariant(KviKvsObject_listviewitem::itemToHandle(i)));
	callFunction(this,"itemExpandedEvent",0,&params);
}

bool KviKvsObject_listview::function_itemCollapsedEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("itemCollapsed",c,c->params());
	return true;
}

void KviKvsObject_listview::slotItemCollapsed(QListViewItem * i)
{
	KviKvsVariantList params(new KviKvsVariant(KviKvsObject_listviewitem::itemToHandle(i)));
	callFunction(this,"itemCollapsedEvent",0,&params);
}

bool KviKvsObject_listview::function_rightButtonClickedEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("rightButtonClicked",c,c->params());
	return true;
}

void KviKvsObject_listview::slotRightButtonClicked(QListViewItem * i,const QPoint &coor, int col)
{
	KviKvsVariant *xpos=new KviKvsVariant((kvs_int_t)coor.x());
	KviKvsVariant *ypos=new KviKvsVariant((kvs_int_t)coor.y());
	KviKvsVariant *column=new KviKvsVariant((kvs_int_t)col);
	KviKvsVariantList params(new KviKvsVariant(KviKvsObject_listviewitem::itemToHandle(i)),xpos,ypos,column);
	callFunction(this,"rightButtonClickedEvent",0,&params);
}

bool KviKvsObject_listview::function_itemRenamedEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("itemRenamed",c,c->params());
	return true;
}

void KviKvsObject_listview::slotItemRenamed(QListViewItem *item,int col,const QString & szText)
{
	KviKvsVariant *column=new KviKvsVariant((kvs_int_t)col);
	KviKvsVariant *txt=new KviKvsVariant(szText);
	KviKvsVariantList params(new KviKvsVariant(KviKvsObject_listviewitem::itemToHandle(item)),column,txt);
	callFunction(this,"itemRenamedEvent",0,&params);
}
void KviKvsObject_listview::fileDropped(QString &szFile,QListViewItem *item)
{
	KviKvsVariant *file=new KviKvsVariant(szFile);
	KviKvsVariantList params(new KviKvsVariant(KviKvsObject_listviewitem::itemToHandle(item)),file);
	callFunction(this,"fileDroppedEvent",0,&params);
}

KviKvsMdmListView::KviKvsMdmListView(QWidget * par,const char * name,KviKvsObject_listview *parent)
:QListView( par,name)
{
	m_pParentScript=parent;
	setAcceptDrops(true);
	viewport()->setAcceptDrops( TRUE );

}
KviKvsMdmListView::~KviKvsMdmListView()
{
}

void KviKvsMdmListView::contentsDragEnterEvent( QDragEnterEvent *e )
{
	debug ("DragEnterEvent");
    if ( !QUriDrag::canDecode(e) )
	{
		e->ignore();
		return;
    }

}


void KviKvsMdmListView::contentsDropEvent(QDropEvent *e)
{
	QStringList list;
	if(QUriDrag::decodeLocalFiles(e,list))
	{
		if(!list.isEmpty())
		{
			QStringList::ConstIterator it = list.begin(); //kewl ! :)
			for( ; it != list.end(); ++it )
			{
				QString tmp = *it; //wow :)
				#ifndef COMPILE_ON_WINDOWS
					if(tmp[0] != '/')tmp.prepend("/"); //HACK HACK HACK for Qt bug (?!?)
				#endif
				QListViewItem *i = itemAt( contentsToViewport(e->pos()) );
				m_pParentScript->fileDropped(tmp,i);
			}
		}
	}

}
#include "m_class_listview.moc"
