//=============================================================================
//
//   File : class_listwidget.cpp
//   Creation date : Sat Oct 2 03:40:28 CET 2004 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2004-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_error.h"
#include "kvi_locale.h"
#include "kvi_debug.h"
#include "class_list.h"
#include "class_listwidget.h"
#include <QListWidget>
/*
        @doc:listbox
        @title:
                listbox class
        @type:
                class
        @short:
                A widget displaying a list of items.
        @inherits:
                [class]object[/class]
                [class]widget[/class]
	@description:
                The listbox class is a widget displaying a list of string items.[br]
                The listbox can operate in four selection modes: single, multi, extended
		and none. In the "single" mode only one item can be selected at a time
		and that item is also the current one. In the "multi" mode multiple
		items can be selected and unselected and the current item may or may not
		be one of the selected items. In the "extended" mode multiple items
		can be selected but they must be selected in a single mouse action
		or by keeping the shift button pressed while clicking on the item.
		In this mode the current item is always one of the selected items.[br]
		In the "none" mode no items can be selected and the current item
		may be any item in the list.[br]
		In most situations you will operate the listwidget in "single" mode
		(and this is the default mode set at widget creation).
	@functions:
		!fn: $setSelectionMode(<selection_mode:string>)
		Sets the current selection mode specified by the parameter <mode>
		that can be one of the following string:[br]
		"single" : only one item can be selected at a time[br]
		"multi" : multiple items can be selected at a time[br]
		"extended" : multiple items can be selected but only in a single mouse interaction[br]
		"none" : no items can be selected[br]
		The default mode is "single"
		!fn: <string> $selectionMode()
		Returns the current selection mode
		!fn: $insertItem(<text:string>, <index:uint>)
		Inserts a text item at position <index>. If index is negative or
		not specified the item is appended.
		!fn: $changeItem(<text:string>, <index:uint>)
		Changes text of item at <index> to <text>.
		!fn: $removeItem(<index:iIndex>)
		Removes item at given index.
		!fn: $count()
		Returns number of items in the widget.
		!fn: <string> $currentText()
		Returns the text of the currently selected item.
		!fn: <integer> $currentItem()
		Returns index of current item or -1 if no item is current.
		!fn: $textAt(<index:integer>)
		Returns item at given index.
		!fn: $setCurrentItem(<index:integer>)
                Sets the current listbox item.
		!fn: $clear()
		Removes all the items
		!fn: $itemEnteredEvent()
		This function is called by KVIrc when the mouse cursor enters an item.
		!fn: $selectionChangeEvent()
                This function is called by KVIrc when the selection in the listbox changes.
		!fn: $currentItemChangeEvent()
		This function is called by KVIrc when the current item changes.
		!fn: <index:integer> $onItemEvent()
		This function is called by KVIrc when the current item pointed by the mouse changes and gives in $0 the item index.
		!fn: <array:x,y,width,height> $itemRect(<item:index>)
		Returns the rectangle on the screen that item occupies, or an invalid rectangle if item is 0 or is not currently visible.

		@signals:
		!sg: <string> $currentItemChanged()
		This signal is emitted by the default implementation of [classfnc]$currentItemChangeEvent[/classfnc]().[br]
		!sg: <string> $itemEntered()
		This signal is emitted by the default implementation of [classfnc]$itemEnteredEvent[/classfnc]().[br]

		*/


KVSO_BEGIN_REGISTERCLASS(KviKvsObject_listwidget,"listbox","widget")
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_listwidget,insertItem)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_listwidget,insertWidgetItem)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_listwidget,changeItem)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_listwidget,removeItem)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_listwidget,clear)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_listwidget,count)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_listwidget,currentText)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_listwidget,currentItem)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_listwidget,textAt);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_listwidget,itemAt);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_listwidget,itemRect);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_listwidget,setCurrentItem);

	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_listwidget,selectedItems)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_listwidget,setSelected)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_listwidget,isSelected)


	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_listwidget,setSelectionMode);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_listwidget,selectionMode);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_listwidget,currentItemChangeEvent);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_listwidget,itemEnteredEvent);


	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_listwidget,"selectionChangeEvent")


KVSO_END_REGISTERCLASS(KviKvsObject_listwidget)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_listwidget,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_listwidget)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_listwidget)

KVSO_END_CONSTRUCTOR(KviKvsObject_listwidget)

bool KviKvsObject_listwidget::init(KviKvsRunTimeContext *,KviKvsVariantList *)
{
	SET_OBJECT(QListWidget)
	obj->setSelectionMode(QAbstractItemView::SingleSelection);
	connect(obj,SIGNAL(itemSelectionChanged()),this,SLOT(selectionChanged()));
	connect(obj,SIGNAL(currentItemChanged(QListWidgetItem *,QListWidgetItem *)),this,SLOT(currentItemChanged(QListWidgetItem *,QListWidgetItem *)));
	connect(obj,SIGNAL(itemEntered(QListWidgetItem *)),this,SLOT(slotItemEntered(QListWidgetItem *)));
	return true;
}

KVSO_CLASS_FUNCTION(listwidget,insertItem)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iRow;
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
		KVSO_PARAMETER("index",KVS_PT_INT,KVS_PF_OPTIONAL,iRow)
	KVSO_PARAMETERS_END(c)
	if (c->parameterCount()==2) ((QListWidget *)widget())->insertItem(iRow, szText);
	else ((QListWidget *)widget())->addItem(szText);
	return true;
}

KVSO_CLASS_FUNCTION(listwidget,clear)
{
	CHECK_INTERNAL_POINTER(widget())
	((QListWidget *)widget())->clear();
	return true;
}

KVSO_CLASS_FUNCTION(listwidget,changeItem)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iIndex,cnt;
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
		KVSO_PARAMETER("index",KVS_PT_UNSIGNEDINTEGER,0,iIndex)
	KVSO_PARAMETERS_END(c)
	if (szText.isEmpty()) c->warning(__tr2qs_ctx("No string parameter given - using empty string","objects"));
	if(iIndex >= (cnt = ((QListWidget *)widget())->count()))
	{
		c->warning(__tr2qs_ctx("Item index [%d] is too big - defaulting to "
			"$count() - 1 [%d]","objects"), iIndex, cnt);
		iIndex = cnt - 1;
	}
	((QListWidget *)widget())->item(iIndex)->setText(szText);
	return true;
}


KVSO_CLASS_FUNCTION(listwidget,removeItem)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t iIndex,cnt;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("index",KVS_PT_UNSIGNEDINTEGER,0,iIndex)
	KVSO_PARAMETERS_END(c)
	if(iIndex >= (cnt = ((QListWidget *)widget())->count()))
	{
		c->warning(__tr2qs_ctx("Item index [%d] is too big - defaulting to " \
			"$count() - 1 [%d]","objects"), iIndex, cnt);
		iIndex = cnt - 1;
	}
	delete ((QListWidget *)widget())->takeItem(iIndex);
	return true;
}

KVSO_CLASS_FUNCTION(listwidget,count)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((QListWidget *)widget())->count());
	return true;
}
KVSO_CLASS_FUNCTION(listwidget,currentText)
{
	CHECK_INTERNAL_POINTER(widget())
	QString text;
	QListWidgetItem *item=((QListWidget *)widget())->currentItem();
	if (item) text=item->text();
	else text="";
	c->returnValue()->setString(text);
	return true;
}
KVSO_CLASS_FUNCTION(listwidget,currentItem)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((QListWidget *)widget())->currentRow());
	return true;
}

KVSO_CLASS_FUNCTION(listwidget,textAt)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t iIndex;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("index",KVS_PT_UNSIGNEDINTEGER,0,iIndex)
	KVSO_PARAMETERS_END(c)
	c->returnValue()->setString(((QListWidget *)widget())->item(iIndex)->text());
	return true;
}
KVSO_CLASS_FUNCTION(listwidget,setCurrentItem)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t iIndex;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("index",KVS_PT_UNSIGNEDINTEGER,0,iIndex)
	KVSO_PARAMETERS_END(c)
	((QListWidget *)widget())->setCurrentRow(iIndex);
	return true;
}
KVSO_CLASS_FUNCTION(listwidget,setSelectionMode)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szMode;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("selection_mode",KVS_PT_STRING,0,szMode)
	KVSO_PARAMETERS_END(c)
	QAbstractItemView::SelectionMode iMode = QAbstractItemView::SingleSelection;
	if(KviQString::equalCI(szMode,"single")) iMode = QAbstractItemView::SingleSelection;
	else if(KviQString::equalCI(szMode,"multi")) iMode = QAbstractItemView::MultiSelection;
	else if(KviQString::equalCI(szMode,"extended")) iMode = QAbstractItemView::ExtendedSelection;
	else if(KviQString::equalCI(szMode,"none")) iMode = QAbstractItemView::NoSelection;
	else  c->warning(__tr2qs_ctx("Invalid selection mode '%Q' assuming single","objects"),&szMode);
	((QListWidget *)widget())->setSelectionMode(iMode);
	return true;
}
KVSO_CLASS_FUNCTION(listwidget,selectionMode)
{
	CHECK_INTERNAL_POINTER(widget())
	switch(((QListWidget *)widget())->selectionMode())
	{
		case QAbstractItemView::SingleSelection: c->returnValue()->setString("single"); break;
		case QAbstractItemView::MultiSelection: c->returnValue()->setString("multi"); break;
		case QAbstractItemView::ExtendedSelection: c->returnValue()->setString("extended"); break;
		case QAbstractItemView::NoSelection: c->returnValue()->setString("none"); break;
		default: c->returnValue()->setString("single"); break;
	}
	return true;
}

KVSO_CLASS_FUNCTION(listwidget,insertWidgetItem)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject *pObject;
	kvs_int_t iIndex;
	kvs_hobject_t hWid;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("widget",KVS_PT_HOBJECT,0,hWid)
		KVSO_PARAMETER("index",KVS_PT_INT,0,iIndex)
	KVSO_PARAMETERS_END(c)
	pObject=KviKvsKernel::instance()->objectController()->lookupObject(hWid);
	if(pObject==this)
	{
		c->warning(__tr2qs_ctx("Can't insert the listwidget itself!","objects"));
		return true;
	}
	if(!pObject->object()->isWidgetType())
	{
		c->warning(__tr2qs_ctx("Can't insert a non-widget object","objects"));
		return true;
	}
	QWidget *wi=((QWidget *)(pObject->object()));
	QListWidgetItem *item=((QListWidget *)widget())->item(iIndex);
	((QListWidget *)widget())->setItemWidget(item,wi);
	return true;
}
KVSO_CLASS_FUNCTION(listwidget,selectedItems)
{
	CHECK_INTERNAL_POINTER(widget())
	QList<QListWidgetItem *> list=((QListWidget *)widget())->selectedItems();
	KviKvsArray * pArray = new KviKvsArray();
	c->returnValue()->setArray(pArray);
	for (int i=0;i<list.count();i++)
	{
		kvs_int_t index=((QListWidget *)widget())->row(list.at(i));
		pArray->set(i,new KviKvsVariant(index));
	}
	return true;
}

KVSO_CLASS_FUNCTION(listwidget,setSelected)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bSel;
	kvs_uint_t uIndex;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("index",KVS_PT_UNSIGNEDINTEGER,0,uIndex)
		KVSO_PARAMETER("benabled",KVS_PT_BOOLEAN,0,bSel)
	KVSO_PARAMETERS_END(c)
	((QListWidget *)widget())->item(uIndex)->setSelected(bSel);
	return true;
}
KVSO_CLASS_FUNCTION(listwidget,isSelected)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uIndex;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("index",KVS_PT_UNSIGNEDINTEGER,0,uIndex)
	KVSO_PARAMETERS_END(c)
	c->returnValue()->setBoolean(((QListWidget *)widget())->item(uIndex)->isSelected());
	return true;
}
KVSO_CLASS_FUNCTION(listwidget,itemAt)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uX,uY;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("uX",KVS_PT_UNSIGNEDINTEGER,0,uX)
		KVSO_PARAMETER("uY",KVS_PT_UNSIGNEDINTEGER,0,uY)
	KVSO_PARAMETERS_END(c)
	c->returnValue()->setInteger(((QListWidget *)widget())->row(((QListWidget *)widget())->itemAt(QPoint(uX,uY))));
	return true;
}
KVSO_CLASS_FUNCTION(listwidget,itemRect)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uIndex;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("iIndex",KVS_PT_UNSIGNEDINTEGER,0,uIndex)
	KVSO_PARAMETERS_END(c)
	QRect rect=((QListWidget *)widget())->visualItemRect(((QListWidget *)widget())->item(uIndex));
	KviKvsArray * a = new KviKvsArray();
	a->set(0,new KviKvsVariant((kvs_int_t)rect.left()));
	a->set(1,new KviKvsVariant((kvs_int_t)rect.top()));
	a->set(2,new KviKvsVariant((kvs_int_t)rect.width()));
	a->set(3,new KviKvsVariant((kvs_int_t)rect.height()));
	c->returnValue()->setArray(a);
	return true;
}

KVSO_CLASS_FUNCTION(listwidget,currentItemChangeEvent)
{
	emitSignal("currentItemChanged",c,c->params());
	return true;
}

KVSO_CLASS_FUNCTION(listwidget,itemEnteredEvent)
{
	emitSignal("itemEntered",c,c->params());

	return true;
}

//slots
void KviKvsObject_listwidget::currentItemChanged(QListWidgetItem *currentItem,QListWidgetItem *)
{
	if (!currentItem) callFunction(this,"currentItemChangeEvent",0,0);
	else
	{
		KviKvsVariantList params(new KviKvsVariant(currentItem->text()));
		callFunction(this,"currentItemChangeEvent",0,&params);
	}

}
void KviKvsObject_listwidget::selectionChanged()
{
	callFunction(this,"selectionChangeEvent",0,0);
}


void KviKvsObject_listwidget::slotItemEntered(QListWidgetItem *item)
{
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)((QListWidget *)widget())->row(item)));
	callFunction(this,"itemEnteredEvent",0,&params);
}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_listwidget.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

