//vim: ts=8
//   File : class_listbox.cpp
//   Creation date : Sat Oct 2 03:40:28 CET 2004 by Szymon Stefanek
//
//    This file is part of the KVirc irc client distribution
//   Copyright (C) 2004 Szymon Stefanek (pragma at kvirc dot net)
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


#include "kvi_error.h"
#include "kvi_locale.h"
#include "kvi_debug.h"
#include "kvi_tal_listbox.h"
#include "class_list.h"
#include "class_listbox.h"

/*
	@doc: listbox
	@keyterms:
		listbox object class
	@title:
		listbox class
	@type:
		class
	@short:
		A widget displaying a list of items
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
		In most situations you will operate the listbox in "single" mode
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
		!fn: $removeItem(<index:uindex>)
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
		!fn: <boolean> $isSelected(<index:integer>)
		Returns 1 if the item at <index> is selected and 0 otherwise
		!fn: $setSelected(<index:integer>,<bSelected:boolean>)
		Changes the selection status of the item at <index> to the value
		specified by <bSelected>. If <bSelected> is 1 the item is selected,
		if <bSelected> is 0 the item is unselected.
		!fn: $selectionChangeEvent()
		This function is called by KVIrc when the selection in the listbox changes.
		!fn: $currentItemChangeEvent()
		This function is called by KVIrc when the current item changes.
		!fn: <index:integer> $onItemEvent()
		This function is called by KVIrc when the current item pointed by the mouse changes and gives in $0 the item index.
		!fn: <array:x,y,width,height> $itemRect(<item:index>)
		Returns the rectangle on the screen that item occupies, or an invalid rectangle if item is 0 or is not currently visible. 

		@signals:
		!sg: <string> $currentItemChange()
		This signal is emitted by the default implementation of [classfnc]$currentItemChangeEvent[/classfnc]().[br]
		!sg: <string> $onItem()
		This signal is emitted by the default implementation of [classfnc]$onItemEvent[/classfnc]().[br]

		*/


KVSO_BEGIN_REGISTERCLASS(KviKvsObject_listbox,"listbox","widget")
	KVSO_REGISTER_HANDLER(KviKvsObject_listbox,"insertItem", functioninsertItem)
	KVSO_REGISTER_HANDLER(KviKvsObject_listbox,"changeItem", functionchangeItem)
	KVSO_REGISTER_HANDLER(KviKvsObject_listbox,"removeItem", functionremoveItem)
	KVSO_REGISTER_HANDLER(KviKvsObject_listbox,"clear", functionclear)
	KVSO_REGISTER_HANDLER(KviKvsObject_listbox,"count", functioncount)
	KVSO_REGISTER_HANDLER(KviKvsObject_listbox,"currentText", functioncurrentText)
	KVSO_REGISTER_HANDLER(KviKvsObject_listbox,"currentItem", functioncurrentItem)
	KVSO_REGISTER_HANDLER(KviKvsObject_listbox,"textAt", functiontextAt);
	KVSO_REGISTER_HANDLER(KviKvsObject_listbox,"itemAt", functionitemAt);
	KVSO_REGISTER_HANDLER(KviKvsObject_listbox,"itemRect", functionitemRect);
	KVSO_REGISTER_HANDLER(KviKvsObject_listbox,"setCurrentItem", functionsetCurrentItem);


	KVSO_REGISTER_HANDLER(KviKvsObject_listbox,"setSelectionMode",functionsetSelectionMode);
	KVSO_REGISTER_HANDLER(KviKvsObject_listbox,"selectionMode",functionselectionMode);
	KVSO_REGISTER_HANDLER(KviKvsObject_listbox,"setSelected",functionsetSelected);
	KVSO_REGISTER_HANDLER(KviKvsObject_listbox,"isSelected",functionisSelected);
	KVSO_REGISTER_HANDLER(KviKvsObject_listbox,"currentItemChangeEvent",functioncurrentItemChangeEvent);
	KVSO_REGISTER_HANDLER(KviKvsObject_listbox,"onItemEvent",functiononItemEvent);


	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_listbox,"selectionChangeEvent")


KVSO_END_REGISTERCLASS(KviKvsObject_listbox)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_listbox,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_listbox)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_listbox)

KVSO_END_CONSTRUCTOR(KviKvsObject_listbox)

bool KviKvsObject_listbox::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{
	KviTalListBox * b = new KviTalListBox(parentScriptWidget());
	b->setSelectionMode(KviTalListBox::Single);
	connect(b,SIGNAL(selectionChanged()),this,SLOT(selectionChanged()));
	connect(b,SIGNAL(currentChanged(KviTalListBoxItem *)),this,SLOT(currentItemChanged(KviTalListBoxItem *)));
	
	connect(b,SIGNAL(onItem(KviTalListBoxItem *)),this,SLOT(onItem(KviTalListBoxItem *)));

	setObject(b,true);;
	return true;
}

bool KviKvsObject_listbox::functioninsertItem(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iIndex;
	QString szItem;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szItem)
		KVSO_PARAMETER("index",KVS_PT_INT,KVS_PF_OPTIONAL,iIndex)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((KviTalListBox *)widget())->insertItem(szItem, iIndex);
	return true;
}
bool KviKvsObject_listbox::functionclear(KviKvsObjectFunctionCall *c)
{
	if (widget()) ((KviTalListBox *)widget())->clear();
	return true;
}
bool KviKvsObject_listbox::functionchangeItem(KviKvsObjectFunctionCall *c)
{

	kvs_uint_t uIndex,cnt;
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
		KVSO_PARAMETER("index",KVS_PT_UNSIGNEDINTEGER,0,uIndex)
	KVSO_PARAMETERS_END(c)
	if(!widget()) return true;
	if (szText.isEmpty()) c->warning(__tr2qs("No string parameter given - using empty string"));
	if(uIndex >= (cnt = ((KviTalListBox *)widget())->count()))
	{
		c->warning(__tr2qs("Item index [%d] is too big - defaulting to " \
			"$count() - 1 [%d]"), uIndex, cnt);
		uIndex = cnt - 1;
	}

	((KviTalListBox *)widget())->changeItem(szText, uIndex);

	return true;

}
bool KviKvsObject_listbox::functionremoveItem(KviKvsObjectFunctionCall *c)
{

	kvs_uint_t uIndex,cnt;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("index",KVS_PT_UNSIGNEDINTEGER,0,uIndex)
	KVSO_PARAMETERS_END(c)
	if(!widget()) return true;
	if(uIndex >= (cnt = ((KviTalListBox *)widget())->count()))
	{
		c->warning(__tr2qs("Item index [%d] is too big - defaulting to " \
			"$count() - 1 [%d]"), uIndex, cnt);
		uIndex = cnt - 1;
	}

	((KviTalListBox *)widget())->removeItem(uIndex);

	return true;

}

bool KviKvsObject_listbox::functioncount(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setInteger(((KviTalListBox *)widget())->count());
	return true;
}
bool KviKvsObject_listbox::functioncurrentText(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setString(((KviTalListBox *)widget())->currentText().toLocal8Bit().data());
	return true;
}
bool KviKvsObject_listbox::functioncurrentItem(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setInteger(((KviTalListBox *)widget())->currentItem());
	return true;
}

bool KviKvsObject_listbox::functiontextAt(KviKvsObjectFunctionCall *c)
{

	kvs_uint_t uIndex;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("index",KVS_PT_UNSIGNEDINTEGER,0,uIndex)
	KVSO_PARAMETERS_END(c)
	if(widget())
		c->returnValue()->setString(((KviTalListBox *)widget())->text(uIndex));
	return true;
}
bool KviKvsObject_listbox::functionsetCurrentItem(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uIndex;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("index",KVS_PT_UNSIGNEDINTEGER,0,uIndex)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((KviTalListBox *)widget())->setCurrentItem(uIndex);
	return true;
}
bool KviKvsObject_listbox::functionsetSelectionMode(KviKvsObjectFunctionCall *c)
{
	QString szMode;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("selection_mode",KVS_PT_STRING,0,szMode)
	KVSO_PARAMETERS_END(c)
	if(!widget()) return true;
	KviTalListBox::SelectionMode iMode = KviTalListBox::Single;

	if(KviQString::equalCI(szMode,"single")) iMode = KviTalListBox::Single;

	else if(KviQString::equalCI(szMode,"multi")) iMode = KviTalListBox::Multi;

	else if(KviQString::equalCI(szMode,"extended")) iMode = KviTalListBox::Extended;

	else if(KviQString::equalCI(szMode,"none")) iMode = KviTalListBox::NoSelection;

	else  c->warning(__tr2qs("Invalid selection mode '%Q' assuming single"),&szMode);
	((KviTalListBox *)widget())->setSelectionMode(iMode);

	return true;
}
bool KviKvsObject_listbox::functionselectionMode(KviKvsObjectFunctionCall *c)
{

	if(!widget()) return true;
	switch(((KviTalListBox *)widget())->selectionMode())
	{
		case KviTalListBox::Single: c->returnValue()->setString("single"); break;
		case KviTalListBox::Multi: c->returnValue()->setString("multi"); break;
		case KviTalListBox::Extended: c->returnValue()->setString("extended"); break;
		case KviTalListBox::NoSelection: c->returnValue()->setString("none"); break;
		default: c->returnValue()->setString("single"); break;
	}
	return true;
}

bool KviKvsObject_listbox::functionisSelected(KviKvsObjectFunctionCall *c)
{

	kvs_uint_t uIndex;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("index",KVS_PT_UNSIGNEDINTEGER,0,uIndex)
	KVSO_PARAMETERS_END(c)
	if(widget())c->returnValue()->setBoolean(((KviTalListBox *)widget())->isSelected(uIndex));
	return true;
}
bool KviKvsObject_listbox::functionsetSelected(KviKvsObjectFunctionCall *c)
{
	bool bSel;
	kvs_uint_t uIndex;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("index",KVS_PT_UNSIGNEDINTEGER,0,uIndex)
		KVSO_PARAMETER("benabled",KVS_PT_BOOLEAN,0,bSel)
	KVSO_PARAMETERS_END(c)
	if(widget()) ((KviTalListBox *)widget())->setSelected(uIndex,bSel);
	return true;
}
bool KviKvsObject_listbox::functionitemAt(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uX,uY;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("uX",KVS_PT_UNSIGNEDINTEGER,0,uX)
		KVSO_PARAMETER("uY",KVS_PT_UNSIGNEDINTEGER,0,uY)
	KVSO_PARAMETERS_END(c)
	if(widget()) 
		c->returnValue()->setInteger(((KviTalListBox *)widget())->index(((KviTalListBox *)widget())->itemAt(QPoint(uX,uY))));
	return true;
}

bool KviKvsObject_listbox::functioncurrentItemChangeEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("currentItemChange",c,c->params());

	return true;
}

void KviKvsObject_listbox::selectionChanged()
{
	KviKvsVariantList *params=0;
	callFunction(this,"selectionChangeEvent",0,0);
}

void KviKvsObject_listbox::currentItemChanged(KviTalListBoxItem *item)
{
	if (!item) callFunction(this,"currentItemChangeEvent",0,0);
	else
	{
		KviKvsVariantList params(new KviKvsVariant(item->text()));
		callFunction(this,"currentItemChangeEvent",0,&params);
	}

}
bool KviKvsObject_listbox::functiononItemEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("onItem",c,c->params());

	return true;
}

void KviKvsObject_listbox::onItem(KviTalListBoxItem *item)
{
		KviTalListBox *lbx = (KviTalListBox *)(item->listBox());
		KviKvsVariantList params(new KviKvsVariant((kvs_int_t)lbx->index(item)));
		callFunction(this,"onItemEvent",0,&params);
}


bool KviKvsObject_listbox::functionitemRect(KviKvsObjectFunctionCall *c)
{
kvs_uint_t uIndex;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("uIndex",KVS_PT_UNSIGNEDINTEGER,0,uIndex)
	KVSO_PARAMETERS_END(c)
	if(widget())
	{
		QRect rect=((KviTalListBox *)widget())->itemRect(((KviTalListBox *)widget())->item(uIndex));
		KviKvsArray * a = new KviKvsArray();
		a->set(0,new KviKvsVariant((kvs_int_t)rect.left()));
		a->set(1,new KviKvsVariant((kvs_int_t)rect.top()));
		a->set(2,new KviKvsVariant((kvs_int_t)rect.width()));
		a->set(3,new KviKvsVariant((kvs_int_t)rect.height()));
		c->returnValue()->setArray(a);
	}
		return true;
}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_listbox.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

