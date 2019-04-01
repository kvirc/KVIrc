//=============================================================================
//
//   File : KvsObject_comboBox.cpp
//   Creation date : Wed 13 Sep 2000 02:42:05 CEST by Krzysztof Godlewski
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000 Krzysztof Godlewski
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviError.h"
#include "KviLocale.h"
#include "kvi_debug.h"
#include "KvsObject_list.h"
#include "KvsObject_comboBox.h"

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
		If the parameter is omitted, it is assumed to be false.
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
		Sets the current combobox item. This is the item to be displayed on the combobox button.
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

KVSO_BEGIN_REGISTERCLASS(KvsObject_comboBox, "combobox", "widget")
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_comboBox, insertItem)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_comboBox, changeItem)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_comboBox, removeItem)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_comboBox, clear)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_comboBox, setMaxCount)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_comboBox, maxCount)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_comboBox, count)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_comboBox, current)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_comboBox, currentItem)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_comboBox, setEditable);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_comboBox, editable)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_comboBox, setEditText)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_comboBox, textAt)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_comboBox, textLineEdit)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_comboBox, setTextLineEdit)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_comboBox, setCurrentItem)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_comboBox, popup)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_comboBox, setInsertionPolicy)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_comboBox, textChangedEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_comboBox, activatedEvent)

KVSO_END_REGISTERCLASS(KvsObject_comboBox)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_comboBox, KvsObject_widget)

KVSO_END_CONSTRUCTOR(KvsObject_comboBox)

KVSO_BEGIN_DESTRUCTOR(KvsObject_comboBox)

KVSO_END_CONSTRUCTOR(KvsObject_comboBox)

bool KvsObject_comboBox::init(KviKvsRunTimeContext *, KviKvsVariantList *)
{
	SET_OBJECT(QComboBox);
	connect(((QComboBox *)widget()), SIGNAL(activated(int)), this, SLOT(slotActivated(int)));
	connect(((QComboBox *)widget()), SIGNAL(editTextChanged(const QString &)), this, SLOT(editTextChanged(const QString &)));
	return true;
}

KVSO_CLASS_FUNCTION(comboBox, insertItem)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iIndex;
	QString szItem;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("item", KVS_PT_STRING, 0, szItem)
	KVSO_PARAMETER("index", KVS_PT_INT, KVS_PF_OPTIONAL, iIndex)
	KVSO_PARAMETERS_END(c)
	if(c->paramCount() == 1)
		((QComboBox *)widget())->addItem(szItem);
	else
		((QComboBox *)widget())->insertItem(iIndex, szItem);
	return true;
}
KVSO_CLASS_FUNCTION(comboBox, clear)
{
	CHECK_INTERNAL_POINTER(widget())
	((QComboBox *)widget())->clear();
	return true;
}
KVSO_CLASS_FUNCTION(comboBox, changeItem)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uIndex, cnt;
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("text", KVS_PT_STRING, 0, szText)
	KVSO_PARAMETER("index", KVS_PT_UNSIGNEDINTEGER, 0, uIndex)
	KVSO_PARAMETERS_END(c)
	if(szText.isEmpty())
		c->warning(__tr2qs_ctx("No string parameter given - using empty string", "objects"));
	if(uIndex >= (cnt = ((QComboBox *)widget())->count()))
	{
		c->warning(__tr2qs_ctx("Item index [%d] is too big - defaulting to "
		                       "$count() - 1 [%d]",
		               "objects"),
		    uIndex, cnt);
		uIndex = cnt - 1;
	}
	((QComboBox *)widget())->setItemText(uIndex, szText);
	return true;
}
KVSO_CLASS_FUNCTION(comboBox, removeItem)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uIndex, cnt;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("index", KVS_PT_UNSIGNEDINTEGER, 0, uIndex)
	KVSO_PARAMETERS_END(c)
	if(uIndex >= (cnt = ((QComboBox *)widget())->count()))
	{
		c->warning(__tr2qs_ctx("Item index [%d] is too big - defaulting to "
		                       "$count() - 1 [%d]",
		               "objects"),
		    uIndex, cnt);
		uIndex = cnt - 1;
	}

	((QComboBox *)widget())->removeItem(uIndex);

	return true;
}
KVSO_CLASS_FUNCTION(comboBox, setMaxCount)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t iMax;
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("count", KVS_PT_UNSIGNEDINTEGER, 0, iMax)
	KVSO_PARAMETERS_END(c)
	((QComboBox *)widget())->setMaxCount(iMax);
	return true;
}
KVSO_CLASS_FUNCTION(comboBox, maxCount)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((QComboBox *)widget())->maxCount());
	return true;
}
KVSO_CLASS_FUNCTION(comboBox, count)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((QComboBox *)widget())->count());
	return true;
}
KVSO_CLASS_FUNCTION(comboBox, current)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setString(((QComboBox *)widget())->currentText());
	return true;
}
KVSO_CLASS_FUNCTION(comboBox, currentItem)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((QComboBox *)widget())->currentIndex());
	return true;
}
KVSO_CLASS_FUNCTION(comboBox, textLineEdit)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setString(((QComboBox *)widget())->lineEdit()->text());
	return true;
}

KVSO_CLASS_FUNCTION(comboBox, setEditable)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bflag", KVS_PT_BOOLEAN, 0, bFlag)
	KVSO_PARAMETERS_END(c)
	((QComboBox *)widget())->setEditable(bFlag);
	return true;
}
KVSO_CLASS_FUNCTION(comboBox, setTextLineEdit)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("text", KVS_PT_STRING, 0, szText)
	KVSO_PARAMETERS_END(c)
	((QComboBox *)widget())->lineEdit()->setText(szText);
	return true;
}

KVSO_CLASS_FUNCTION(comboBox, editable)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setBoolean(((QComboBox *)widget())->isEditable());
	return true;
}
KVSO_CLASS_FUNCTION(comboBox, setEditText)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szText;
	bool bQuiet = false;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("text", KVS_PT_STRING, 0, szText)
	KVSO_PARAMETER("quiet", KVS_PT_BOOLEAN, KVS_PF_OPTIONAL, bQuiet)
	KVSO_PARAMETERS_END(c)
	if(szText.isEmpty() && !bQuiet)
		c->warning(__tr2qs_ctx("No string parameter given - using empty string", "objects"));
	((QComboBox *)widget())->setEditText(szText);
	return true;
}
KVSO_CLASS_FUNCTION(comboBox, textAt)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uIndex;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("index", KVS_PT_UNSIGNEDINTEGER, 0, uIndex)
	KVSO_PARAMETERS_END(c)
	c->returnValue()->setString(((QComboBox *)widget())->itemText(uIndex));
	return true;
}
KVSO_CLASS_FUNCTION(comboBox, setCurrentItem)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uIndex;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("index", KVS_PT_UNSIGNEDINTEGER, 0, uIndex)
	KVSO_PARAMETERS_END(c)
	((QComboBox *)widget())->setCurrentIndex(uIndex);
	return true;
}
KVSO_CLASS_FUNCTION(comboBox, popup)
{
	if(widget())
		((QComboBox *)widget())->showPopup();
	return true;
}
KVSO_CLASS_FUNCTION(comboBox, setInsertionPolicy)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szPolicy;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("policy", KVS_PT_STRING, 0, szPolicy)
	KVSO_PARAMETERS_END(c)
	if(KviQString::equalCI(szPolicy, "NoInsertion"))
		((QComboBox *)widget())->setInsertPolicy(QComboBox::NoInsert);
	else if(KviQString::equalCI(szPolicy, "AtTop"))
		((QComboBox *)widget())->setInsertPolicy(QComboBox::InsertAtTop);
	else if(KviQString::equalCI(szPolicy, "AtBotton"))
		((QComboBox *)widget())->setInsertPolicy(QComboBox::InsertAtBottom);
	else if(KviQString::equalCI(szPolicy, "AtCurrent"))
		((QComboBox *)widget())->setInsertPolicy(QComboBox::InsertAtCurrent);
	else if(KviQString::equalCI(szPolicy, "AfterCurrent"))
		((QComboBox *)widget())->setInsertPolicy(QComboBox::InsertAfterCurrent);
	else if(KviQString::equalCI(szPolicy, "BeforeCurrent"))
		((QComboBox *)widget())->setInsertPolicy(QComboBox::InsertBeforeCurrent);
	else
		c->warning(__tr2qs_ctx("Invalid insertion policy '%Q'", "objects"), &szPolicy);
	return true;
}

KVSO_CLASS_FUNCTION(comboBox, textChangedEvent)
{
	emitSignal("textChanged", c, c->params());
	return true;
}

KVSO_CLASS_FUNCTION(comboBox, activatedEvent)
{
	emitSignal("activated", c, c->params());
	return true;
}

//slots
void KvsObject_comboBox::editTextChanged(const QString & text)
{
	KviKvsVariantList params(new KviKvsVariant(text));
	callFunction(this, "textChangedEvent", &params);
}

void KvsObject_comboBox::slotActivated(int i)
{
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)i));
	callFunction(this, "activatedEvent", &params);
}
