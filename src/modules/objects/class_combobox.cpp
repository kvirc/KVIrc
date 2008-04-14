//vim: ts=8
//   File : class_combobox.cpp
//   Creation date : Thu Mar 22 20:57:45 CET 2001 by Krzysztof Godlewski
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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
#include "class_list.h"
#include "class_combobox.h"

#include <QLineEdit>
#include <QString>
#include <QComboBox>

/*
	@doc:	combobox
	@keyterms:
		combobox object class, selection
	@title:
		combobox class
	@type:
		class
	@short:
		Combined button and popup list
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
	@functions:
		!fn: $insertItem(<text:string>, <index:int>)
		Inserts a text item at position <index>. If index is negative or
		not specified the item is appended.
		!fn: $changeItem(<text:string>, <index:uint>)
		Changes text of item at <index> to <text>.
		!fn: $removeItem(<index:uint>)
		Removes item at given index.
		!fn: $setMaxCount(<num:uint>)
		Sets the maximum number of items the combo box can hold to
		<num>. If <num> is smaller than the current number of items,
		the list is truncated at the end. There is no limit by default.
		!fn: <integer> $maxCount()
		Returns the current maximum number of items in the combo box.
		!fn: <integer> $count()
		Returns number of items in the widget.
		!fn: <string> $current()
		Returns currently selected item.
		!fn: <integer> $currentItem()
		Returns index of currently selected item.
		!fn: $setEditable(<bflag:boolean>)
		Make the input field editable, if <bflag>. Otherwise the
		user may only choose one of the items in the combo box.[br]
		If the parameter is ommited, it is assumed to be false.
		!fn: <boolean> $editable()
		Returns whether the combobox is editable or not.
		!fn: $setEditText(<text:string>,[<quiet:bool>])
		Sets the text in the embedded line edit to newText without
		changing the combo's contents. Does nothing if the combo
		isn't editable. If the optional quiet parameter is true no warning will be print if text will be empty.
		!fn: $clear()
		Removes all the items from the combo box
		!fn: $textAt(<index:uint>)
		Returns item at given index.
		!fn: $setCurrentItem(<index:uint>)
		Sets the current combobox item. This is the item to be
		displayed on the combobox button.
		!fn: $activatedEvent(<index:integer>)
		This function is called by the framework when a new item  has activated. The index value is the position of the new item.[br]
		The default implementation emits the [classfnc]$activated[/classfnc]() signal,
		!fn: $textChangedEvent(<text:string>)
		This function is called when the text in an editable combobox has changed.[br]
		The function return the new text in its argument.[br]
		The default implementation emits the [classfnc]$textChanged[/classfnc]() signal.
		@signals:
		!sg: $textChanged()
		This signal is emitted by the default implementation of [classfnc]$textChangedEvent[/classfnc]().
		!sg: $activated()
		This signal is emitted by the default implementation of [classfnc]$activatedEvent[/classfnc]().

*/
KVSO_BEGIN_REGISTERCLASS(KviKvsObject_combobox,"combobox","widget")
	KVSO_REGISTER_HANDLER(KviKvsObject_combobox,"insertItem", functioninsertItem)
	KVSO_REGISTER_HANDLER(KviKvsObject_combobox,"changeItem", functionchangeItem)
	KVSO_REGISTER_HANDLER(KviKvsObject_combobox,"removeItem", functionremoveItem)
	KVSO_REGISTER_HANDLER(KviKvsObject_combobox,"clear", functionclear)
	KVSO_REGISTER_HANDLER(KviKvsObject_combobox,"setMaxCount", functionsetMaxCount)
	KVSO_REGISTER_HANDLER(KviKvsObject_combobox,"maxCount", functionmaxCount)
	KVSO_REGISTER_HANDLER(KviKvsObject_combobox,"count", functioncount)
	KVSO_REGISTER_HANDLER(KviKvsObject_combobox,"current", functioncurrent)
	KVSO_REGISTER_HANDLER(KviKvsObject_combobox,"currentItem", functioncurrentItem)
	KVSO_REGISTER_HANDLER(KviKvsObject_combobox,"setEditable", functionsetEditable);
	KVSO_REGISTER_HANDLER(KviKvsObject_combobox,"editable", functioneditable)
	KVSO_REGISTER_HANDLER(KviKvsObject_combobox,"setEditText", functionsetEditText)
	KVSO_REGISTER_HANDLER(KviKvsObject_combobox,"textAt", functiontextAt)
	KVSO_REGISTER_HANDLER(KviKvsObject_combobox,"textLineEdit", functiontextLineEdit)
	KVSO_REGISTER_HANDLER(KviKvsObject_combobox,"setTextLineEdit", functionsetTextLineEdit)
	KVSO_REGISTER_HANDLER(KviKvsObject_combobox,"setCurrentItem", functionsetCurrentItem)
	KVSO_REGISTER_HANDLER(KviKvsObject_combobox,"popup", functionpopup)

	KVSO_REGISTER_HANDLER(KviKvsObject_combobox,"setInsertionPolicy", functionsetInsertionPolicy)
	KVSO_REGISTER_HANDLER(KviKvsObject_combobox,"textChangedEvent", functiontextChangedEvent)
	KVSO_REGISTER_HANDLER(KviKvsObject_combobox,"activatedEvent", functionactivatedEvent)

KVSO_END_REGISTERCLASS(KviKvsObject_combobox)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_combobox,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_combobox)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_combobox)

KVSO_END_CONSTRUCTOR(KviKvsObject_combobox)

bool KviKvsObject_combobox::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{
	SET_OBJECT(QComboBox);
	connect (((QComboBox *)widget()),SIGNAL(activated( int )),this,SLOT(slotActivated( int )));
	connect (((QComboBox *)widget()),SIGNAL(editTextChanged(const QString & )),this,SLOT(editTextChanged(const QString & )));
	return true;
}

bool KviKvsObject_combobox::functioninsertItem(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iIndex;
	QString szItem;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("item",KVS_PT_STRING,0,szItem)
		KVSO_PARAMETER("index",KVS_PT_INT,KVS_PF_OPTIONAL,iIndex)
	KVSO_PARAMETERS_END(c)
	if(widget())
		if(c->paramCount()==1)
			((QComboBox *)widget())->addItem(szItem);
		else
			((QComboBox *)widget())->insertItem( iIndex,szItem);
	return true;
}
bool KviKvsObject_combobox::functionclear(KviKvsObjectFunctionCall *c)
{
	if (widget()) ((QComboBox *)widget())->clear();
	return true;
}
bool KviKvsObject_combobox::functionchangeItem(KviKvsObjectFunctionCall *c)
{

	kvs_uint_t uIndex,cnt;
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
		KVSO_PARAMETER("index",KVS_PT_UNSIGNEDINTEGER,0,uIndex)
	KVSO_PARAMETERS_END(c)
	if(!widget()) return true;
	if (szText.isEmpty()) c->warning(__tr2qs("No string parameter given - using empty string"));
	if(uIndex >= (cnt = ((QComboBox *)widget())->count()))
	{
		c->warning(__tr2qs("Item index [%d] is too big - defaulting to " \
			"$count() - 1 [%d]"), uIndex, cnt);
		uIndex = cnt - 1;
	}

	((QComboBox *)widget())->setItemText( uIndex,szText);

	return true;

}
bool KviKvsObject_combobox::functionremoveItem(KviKvsObjectFunctionCall *c)
{

	kvs_uint_t uIndex,cnt;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("index",KVS_PT_UNSIGNEDINTEGER,0,uIndex)
	KVSO_PARAMETERS_END(c)
	if(!widget()) return true;
	if(uIndex >= (cnt = ((QComboBox *)widget())->count()))
	{
		c->warning(__tr2qs("Item index [%d] is too big - defaulting to " \
			"$count() - 1 [%d]"), uIndex, cnt);
		uIndex = cnt - 1;
	}

	((QComboBox *)widget())->removeItem(uIndex);

	return true;

}
bool KviKvsObject_combobox::functionsetMaxCount(KviKvsObjectFunctionCall *c)
{

	kvs_uint_t iMax;
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("count",KVS_PT_UNSIGNEDINTEGER,0,iMax)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QComboBox *)widget())->setMaxCount(iMax);
	return true;
}
bool KviKvsObject_combobox::functionmaxCount(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setInteger(((QComboBox *)widget())->maxCount());
	return true;
}
bool KviKvsObject_combobox::functioncount(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setInteger(((QComboBox *)widget())->count());
	return true;
}
bool KviKvsObject_combobox::functioncurrent(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setString(((QComboBox *)widget())->currentText());
	return true;
}
bool KviKvsObject_combobox::functioncurrentItem(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setInteger(((QComboBox *)widget())->currentIndex());
	return true;
}
bool KviKvsObject_combobox::functiontextLineEdit(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setString(((QComboBox *)widget())->lineEdit()->text());
	return true;
}

bool KviKvsObject_combobox::functionsetEditable(KviKvsObjectFunctionCall *c)
{
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bflag",KVS_PT_BOOLEAN,0,bFlag)
	KVSO_PARAMETERS_END(c)
	if(widget())
	{
		((QComboBox *)widget())->setEditable(bFlag);
	}

	return true;
}
bool KviKvsObject_combobox::functionsetTextLineEdit(KviKvsObjectFunctionCall *c)
{
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QComboBox *)widget())->lineEdit()->setText(szText);
	return true;
}

bool KviKvsObject_combobox::functioneditable(KviKvsObjectFunctionCall *c)
{
	if(widget())
		c->returnValue()->setBoolean(((QComboBox *)widget())->isEditable());
	return true;
}
bool KviKvsObject_combobox::functionsetEditText(KviKvsObjectFunctionCall *c)
{
	QString szText;
	bool bQuiet=false;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
		KVSO_PARAMETER("quiet",KVS_PT_BOOLEAN,KVS_PF_OPTIONAL,bQuiet)
	KVSO_PARAMETERS_END(c)
	if(!widget()) return true;
	if (szText.isEmpty() && !bQuiet) c->warning("No string parameter given - using empty string");
	((QComboBox *)widget())->setEditText(szText);
	return true;
}
bool KviKvsObject_combobox::functiontextAt(KviKvsObjectFunctionCall *c)
{

	kvs_uint_t uIndex;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("index",KVS_PT_UNSIGNEDINTEGER,0,uIndex)
	KVSO_PARAMETERS_END(c)
	if(widget())
		c->returnValue()->setString(((QComboBox *)widget())->itemText(uIndex));
	return true;
}
bool KviKvsObject_combobox::functionsetCurrentItem(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uIndex;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("index",KVS_PT_UNSIGNEDINTEGER,0,uIndex)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QComboBox *)widget())->setCurrentIndex(uIndex);
	return true;
}
bool KviKvsObject_combobox::functionpopup(KviKvsObjectFunctionCall *c)
{
	if(widget())
	   ((QComboBox *)widget())->showPopup();
	return true;
}
bool KviKvsObject_combobox::functionsetInsertionPolicy(KviKvsObjectFunctionCall *c)
{
	QString szPolicy;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("policy",KVS_PT_STRING,0,szPolicy)
	KVSO_PARAMETERS_END(c)
	if(!widget()) return true;
	if(KviQString::equalCI(szPolicy,"NoInsertion"))
			((QComboBox *)widget())->setInsertPolicy(QComboBox::NoInsertion);
	else if(KviQString::equalCI(szPolicy,"AtTop"))
			((QComboBox *)widget())->setInsertPolicy(QComboBox::AtTop);
	else if(KviQString::equalCI(szPolicy,"AtBotton"))
			((QComboBox *)widget())->setInsertPolicy(QComboBox::AtBottom);
	else if(KviQString::equalCI(szPolicy,"AtCurrent"))
			((QComboBox *)widget())->setInsertPolicy(QComboBox::AtCurrent);
	else if(KviQString::equalCI(szPolicy,"AfterCurrent"))
			((QComboBox *)widget())->setInsertPolicy(QComboBox::AfterCurrent);
	else if(KviQString::equalCI(szPolicy,"BeforeCurrent"))
			((QComboBox *)widget())->setInsertPolicy(QComboBox::BeforeCurrent);
	else c->warning(__tr2qs("Invalid insertion Policy %Q"),&szPolicy);
	return true;
}



bool KviKvsObject_combobox::functiontextChangedEvent(KviKvsObjectFunctionCall *c)
{

	emitSignal("textChanged",c,c->params());
	return true;

}

void KviKvsObject_combobox::editTextChanged(const QString &text)
{
	KviKvsVariantList params(new KviKvsVariant(text));
	callFunction(this,"textChangedEvent",&params);
}

bool KviKvsObject_combobox::functionactivatedEvent(KviKvsObjectFunctionCall *c)
{

	emitSignal("activated",c,c->params());
	return true;

}

void KviKvsObject_combobox::slotActivated(int i)
{
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)i));
	callFunction(this,"activatedEvent",&params);
}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_combobox.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

