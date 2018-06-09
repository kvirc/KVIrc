//=============================================================================
//
//   File : KvsObject_listWidget.cpp
//   Creation date : Sat Oct 2 03:40:28 CET 2004 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "KvsObject_listWidget.h"
#include "KvsObject_pixmap.h"
#include "KviIconManager.h"
#include <QListWidget>

const int item_flags[] = {
	Qt::NoItemFlags,
	Qt::ItemIsSelectable,
	Qt::ItemIsEditable,
	Qt::ItemIsDragEnabled,
	Qt::ItemIsDropEnabled,
	Qt::ItemIsUserCheckable,
	Qt::ItemIsEnabled,
	Qt::ItemIsTristate
};

const char * const itemflags_tbl[] = {
	"noitemflag",
	"selectable",
	"editable",
	"dragEnabled",
	"dropEnabled",
	"userCheckable",
	"enabled",
	"tristate"
};

#define itemflags_num (sizeof(itemflags_tbl) / sizeof(itemflags_tbl[0]))

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
		and none. In the [i]single[/i] mode only one item can be selected at a time
		and that item is also the current one. In the [i]multi[/i] mode multiple
		items can be selected and unselected and the current item may or may not
		be one of the selected items. In the [i]extended[/i] mode multiple items
		can be selected but they must be selected in a single mouse action
		or by keeping the shift button pressed while clicking on the item.
		In this mode the current item is always one of the selected items.[br]
		In the [i]none[/i] mode no items can be selected and the current item
		may be any item in the list.[br]
		In most situations you will operate the listwidget in [i]single[/i] mode
		(and this is the default mode set at widget creation).
	@functions:
		!fn: $setSelectionMode(<selection_mode:string>)
		Sets the current selection mode specified by the parameter <mode>
		that can be one of the following string:[br]
		[b]single[/b]: only one item can be selected at a time[br]
		[b]multi[/b]: multiple items can be selected at a time[br]
		[b]extended[/b]: multiple items can be selected but only in a single mouse interaction[br]
		[b]none[/b]: no items can be selected[br]
		The default mode is [i]"single"
		!fn: $setFlag(<idx:integer>,<flag1:string>, <flag2:string>, ...)
		Sets the flags for the item at index idx to the given flags. These determine whether the item can be selected or modified. This is often used to disable an item.[br]
		Supported flags are:
		[example]
			- noitemflag : no flag sets;[br]
			- selectable : item is selectable;[br]
			- editable : item is editable;[br]
			- dragEnabled : item can dragged;[br]
			- dropEnabled : item can used as drop target;[br]
			- userCheckable : item is checkable;[br]
			- enabled : item is enabled;[br]
			- tristate : item is checkable with three separate states.
		[/example]
		!fn: $setChecked(<checkstate:bool>)
		Sets the eventual item check box 'checked state' to <bool>.
		!fn: <bool> $isChecked()
		Returns '1' if the eventual item check box is checked, '0' otherwise.[br]
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
		!fn: $setFont(<idx:integer>,<family:string>,<size:integer>[,<style1:string>, <style2:string>, ...])
		Set the font's family, size and style, at index <idx>; valid flag for style are:[br]
		italic, bold, underline, overline, strikeout, fixedpitch  [br]
		If you just want to set a style without altering the preset font size and family, you can use $setFont() like this:[br]
		%widget->$setFont(0,0,"bold")
		!fn: <index:integer> $onItemEvent()
		This function is called by KVIrc when the current item pointed by the mouse changes and gives in $0 the item index.
		!fn: <array:x,y,width,height> $itemRect(<item:index>)
		Returns the rectangle on the screen that item occupies, or an invalid rectangle if item is 0 or is not currently visible.
		!fn: $itemEnteredEvent()
		This function is called by KVIrc when the mouse cursor enters an item.
		!fn: $selectionChangedEvent()
		This function is called by KVIrc when the selection in the listbox changes.
		!fn: $currentItemChangedEvent()
		This function is called by KVIrc when the current item changes.
		!fn: $itemChangedEvent()
		This function is called by KVIrc when the current data item changes (i.e. the user check a checkable item) .
	@signals:
		!sg: <string> $currentItemChanged()
		This signal is emitted by the default implementation of [classfnc]$currentItemChangedEvent[/classfnc]().[br]
		!sg: <string> $itemChanged()
		This signal is emitted by the default implementation of [classfnc]$itemChangedEvent[/classfnc]().[br]
		!sg: <string> $itemEntered()
		This signal is emitted by the default implementation of [classfnc]$itemEnteredEvent[/classfnc]().[br]
		!sg: <string> $selectionChanged()
		This signal is emitted by the default implementation of [classfnc]$selectionChangedEvent[/classfnc]().[br]
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_listWidget, "listbox", "widget")
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, insertItem)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, insertWidgetItem)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, changeItem)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, removeItem)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, clear)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, count)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, currentText)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, currentItem)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, textAt);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, itemAt);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, itemRect);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, setCurrentItem);

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, selectedItems);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, setSelected);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, isSelected);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, setFont);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, setIcon);

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, setFlags)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, setChecked);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, isChecked);

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, setSelectionMode);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, selectionMode);

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, setForeground);

// events
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, currentItemChangedEvent);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, itemChangedEvent);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_listWidget, itemEnteredEvent);
KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KvsObject_listWidget, "selectionChangedEvent")

KVSO_END_REGISTERCLASS(KvsObject_listWidget)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_listWidget, KvsObject_widget)

KVSO_END_CONSTRUCTOR(KvsObject_listWidget)

KVSO_BEGIN_DESTRUCTOR(KvsObject_listWidget)

KVSO_END_CONSTRUCTOR(KvsObject_listWidget)

bool KvsObject_listWidget::init(KviKvsRunTimeContext *, KviKvsVariantList *)
{
	SET_OBJECT(QListWidget)
	obj->setSelectionMode(QAbstractItemView::SingleSelection);
	connect(obj, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
	connect(obj, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(currentItemChanged(QListWidgetItem *, QListWidgetItem *)));
	connect(obj, SIGNAL(itemEntered(QListWidgetItem *)), this, SLOT(slotItemEntered(QListWidgetItem *)));
	connect(obj, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(slotItemChanged(QListWidgetItem *)));

	return true;
}

KVSO_CLASS_FUNCTION(listWidget, insertItem)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iRow;
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("text", KVS_PT_STRING, 0, szText)
	KVSO_PARAMETER("index", KVS_PT_INT, KVS_PF_OPTIONAL, iRow)
	KVSO_PARAMETERS_END(c)
	if(c->parameterCount() == 2)
		((QListWidget *)widget())->insertItem(iRow, szText);
	else
		((QListWidget *)widget())->addItem(szText);
	return true;
}

KVSO_CLASS_FUNCTION(listWidget, setFlags)
{
	CHECK_INTERNAL_POINTER(widget())
	QStringList itemflags;
	kvs_uint_t iIdx;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("index", KVS_PT_UNSIGNEDINTEGER, 0, iIdx)
	KVSO_PARAMETER("flags", KVS_PT_STRINGLIST, KVS_PF_OPTIONAL, itemflags)
	KVSO_PARAMETERS_END(c)
	int flag, sum = 0;
	QListWidgetItem * pItem = ((QListWidget *)widget())->item(iIdx);
	if(!pItem)
		return true;
	for(int i = 0; i < itemflags.count(); i++)
	{
		flag = 0;
		for(unsigned int j = 0; j < itemflags_num; j++)
		{
			if(KviQString::equalCI(itemflags.at(i), itemflags_tbl[j]))
			{
				flag = item_flags[j];
				break;
			}
		}
		if(flag)
		{
			if(flag == Qt::ItemIsUserCheckable)
				pItem->setCheckState(Qt::Unchecked);
			sum = sum | flag;
		}
		else
			c->warning(__tr2qs_ctx("Unknown item flag '%Q'", "objects"), &itemflags.at(i));
	}
	pItem->setFlags((Qt::ItemFlags)sum);
	return true;
}
KVSO_CLASS_FUNCTION(listWidget, setChecked)
{
	bool bChecked;
	kvs_uint_t iIdx;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("index", KVS_PT_UNSIGNEDINTEGER, 0, iIdx)
	KVSO_PARAMETER("bChecked", KVS_PT_BOOL, 0, bChecked)
	KVSO_PARAMETERS_END(c)
	QListWidgetItem * pItem = ((QListWidget *)widget())->item(iIdx);
	if(!pItem)
		return true;
	pItem->setCheckState(bChecked ? Qt::Checked : Qt::Unchecked);
	return true;
}

KVSO_CLASS_FUNCTION(listWidget, isChecked)
{
	kvs_uint_t iIdx;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("index", KVS_PT_UNSIGNEDINTEGER, 0, iIdx)
	KVSO_PARAMETERS_END(c)
	QListWidgetItem * pItem = ((QListWidget *)widget())->item(iIdx);
	if(!pItem)
		return true;
	c->returnValue()->setBoolean(pItem->checkState() == Qt::Checked ? true : false);
	return true;
}

KVSO_CLASS_FUNCTION(listWidget, setFont)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szFamily;
	QStringList szListStyle;
	kvs_int_t iSize, iIdx;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("index", KVS_PT_INTEGER, 0, iIdx)
	KVSO_PARAMETER("size", KVS_PT_INTEGER, 0, iSize)
	KVSO_PARAMETER("family", KVS_PT_STRING, 0, szFamily)
	KVSO_PARAMETER("style", KVS_PT_STRINGLIST, KVS_PF_OPTIONAL, szListStyle)
	KVSO_PARAMETERS_END(c)
	QListWidgetItem * pItem = ((QListWidget *)widget())->item(iIdx);
	if(!pItem)
		return true;
	QFont font = widget()->font();
	if(!szFamily.isEmpty())
		font.setFamily(szFamily);
	if(iSize)
		font.setPointSize(iSize);
	QString szStyle;
	for(int i = 0; i < szListStyle.length(); i++)
	{
		szStyle = szListStyle.at(i);
		if(KviQString::equalCI(szStyle, "italic"))
			font.setItalic(true);
		else if(KviQString::equalCI(szStyle, "bold"))
			font.setBold(true);
		else if(KviQString::equalCI(szStyle, "underline"))
			font.setUnderline(true);
		else if(KviQString::equalCI(szStyle, "overline"))
			font.setOverline(true);
		else if(KviQString::equalCI(szStyle, "strikeout"))
			font.setStrikeOut(true);
		else if(KviQString::equalCI(szStyle, "fixedpitch"))
			font.setFixedPitch(true);
		else
			c->warning(__tr2qs_ctx("Unknown style '%Q'", "objects"), &szStyle);
	}
	pItem->setFont(font);
	return true;
}
KVSO_CLASS_FUNCTION(listWidget, setIcon)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uIdx;
	KviKvsObject * obPixmap;
	kvs_hobject_t obHpixmap;
	KviKvsVariant * vPixmap;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("column", KVS_PT_UNSIGNEDINTEGER, 0, uIdx)
	KVSO_PARAMETER("pixmap", KVS_PT_VARIANT, 0, vPixmap)
	KVSO_PARAMETERS_END(c)
	QListWidgetItem * pItem = ((QListWidget *)widget())->item(uIdx);
	if(!pItem)
		return true;

	QPixmap * pix = nullptr;
	if(vPixmap->isEmpty())
	{
		pItem->setIcon(QIcon());
		return true;
	}
	if(vPixmap->isHObject())
	{
		vPixmap->asHObject(obHpixmap);
		obPixmap = KviKvsKernel::instance()->objectController()->lookupObject(obHpixmap);
		if(!obPixmap->inheritsClass("pixmap"))
		{
			c->warning(__tr2qs_ctx("Pixmap object or image ID required", "objects"));
			return true;
		}
		pix = ((KvsObject_pixmap *)obPixmap)->getPixmap();
	}
	else
	{
		QString szPix;
		vPixmap->asString(szPix);
		pix = g_pIconManager->getImage(szPix);
		if(!pix)
		{
			c->warning(__tr2qs_ctx("Error occurred: the suitable file '%Q' is not of the correct format or it is not a valid icon number.", "objects"), &szPix);
			return true;
		}
	}
	if(pix)
	{
		pItem->setIcon(*pix);
	}
	else
	{
		pItem->setIcon(QIcon());
	}
	return true;
}
KVSO_CLASS_FUNCTION(listWidget, setForeground)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iCol1, iCol2, iCol3, iOpacity, iIdx;
	QString szColorMode, szColor;
	KviKvsVariant *var1, *var2, *var3;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("index", KVS_PT_INTEGER, 0, iIdx)
	KVSO_PARAMETER("Color_1_Or_Colorname", KVS_PT_VARIANT, 0, var1)
	KVSO_PARAMETER("Color_2", KVS_PT_VARIANT, KVS_PF_OPTIONAL, var2)
	KVSO_PARAMETER("Colo3_3", KVS_PT_VARIANT, KVS_PF_OPTIONAL, var3)
	KVSO_PARAMETER("color_mode", KVS_PT_STRING, KVS_PF_OPTIONAL, szColorMode)
	KVSO_PARAMETER("opacity", KVS_PT_INT, KVS_PF_OPTIONAL, iOpacity)
	KVSO_PARAMETERS_END(c)
	QListWidgetItem * pItem = ((QListWidget *)widget())->item(iIdx);
	if(!pItem)
		return true;
	QColor col;
	if(!var1->asInteger(iCol1))
	{
		var1->asString(szColor);
		if(c->paramCount() < 3)
			iOpacity = 255;
		else
		{
			if(!var2->asInteger(iOpacity))
			{
				c->warning(__tr2qs_ctx("The opacity parameter didn't evaluate to integer", "objects"));
				return true;
			}
		}
		col.setNamedColor(szColor);
		col.setAlpha(iOpacity);
	}
	else
	{
		if(c->paramCount() < 4)
		{
			c->error(__tr2qs_ctx("Color name or triplet RGB/HSV value required", "objects"));
			return true;
		}
		if(!var2->asInteger(iCol2) || !var3->asInteger(iCol3))
		{
			c->error(__tr2qs_ctx("One of the triplet parameters didn't evaluate to an integer", "objects"));
			return true;
		}
		if(c->paramCount() < 5)
			iOpacity = 255;
		if(KviQString::equalCI(szColorMode, "HSV"))
			col.setHsv(iCol1, iCol2, iCol3, iOpacity);
		else
			col.setRgb(iCol1, iCol2, iCol3, iOpacity);
	}
	QBrush brush = pItem->foreground();
	brush.setColor(col);
	pItem->setForeground(brush);
	return true;
}
KVSO_CLASS_FUNCTION(listWidget, clear)
{
	CHECK_INTERNAL_POINTER(widget())
	((QListWidget *)widget())->clear();
	return true;
}

KVSO_CLASS_FUNCTION(listWidget, changeItem)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t iIndex, cnt;
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("text", KVS_PT_STRING, 0, szText)
	KVSO_PARAMETER("index", KVS_PT_UNSIGNEDINTEGER, 0, iIndex)
	KVSO_PARAMETERS_END(c)
	if(szText.isEmpty())
		c->warning(__tr2qs_ctx("No string parameter given - using empty string", "objects"));
	if(iIndex >= (cnt = ((QListWidget *)widget())->count()))
	{
		c->warning(__tr2qs_ctx("Item index [%d] is too big - defaulting to "
		                       "$count() - 1 [%d]",
		               "objects"),
		    iIndex, cnt);
		iIndex = cnt - 1;
	}
	((QListWidget *)widget())->item(iIndex)->setText(szText);
	return true;
}

KVSO_CLASS_FUNCTION(listWidget, removeItem)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t iIndex, cnt;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("index", KVS_PT_UNSIGNEDINTEGER, 0, iIndex)
	KVSO_PARAMETERS_END(c)
	if(iIndex >= (cnt = ((QListWidget *)widget())->count()))
	{
		c->warning(__tr2qs_ctx("Item index [%d] is too big - defaulting to "
		                       "$count() - 1 [%d]",
		               "objects"),
		    iIndex, cnt);
		iIndex = cnt - 1;
	}
	delete((QListWidget *)widget())->takeItem(iIndex);
	return true;
}

KVSO_CLASS_FUNCTION(listWidget, count)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((QListWidget *)widget())->count());
	return true;
}
KVSO_CLASS_FUNCTION(listWidget, currentText)
{
	CHECK_INTERNAL_POINTER(widget())
	QString text;
	QListWidgetItem * item = ((QListWidget *)widget())->currentItem();
	if(item)
		text = item->text();
	else
		text = "";
	c->returnValue()->setString(text);
	return true;
}
KVSO_CLASS_FUNCTION(listWidget, currentItem)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((QListWidget *)widget())->currentRow());
	return true;
}

KVSO_CLASS_FUNCTION(listWidget, textAt)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t iIndex;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("index", KVS_PT_UNSIGNEDINTEGER, 0, iIndex)
	KVSO_PARAMETERS_END(c)
	c->returnValue()->setString(((QListWidget *)widget())->item(iIndex)->text());
	return true;
}
KVSO_CLASS_FUNCTION(listWidget, setCurrentItem)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t iIndex;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("index", KVS_PT_UNSIGNEDINTEGER, 0, iIndex)
	KVSO_PARAMETERS_END(c)
	((QListWidget *)widget())->setCurrentRow(iIndex);
	return true;
}
KVSO_CLASS_FUNCTION(listWidget, setSelectionMode)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szMode;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("selection_mode", KVS_PT_STRING, 0, szMode)
	KVSO_PARAMETERS_END(c)
	QAbstractItemView::SelectionMode iMode = QAbstractItemView::SingleSelection;
	if(KviQString::equalCI(szMode, "single"))
		iMode = QAbstractItemView::SingleSelection;
	else if(KviQString::equalCI(szMode, "multi"))
		iMode = QAbstractItemView::MultiSelection;
	else if(KviQString::equalCI(szMode, "extended"))
		iMode = QAbstractItemView::ExtendedSelection;
	else if(KviQString::equalCI(szMode, "none"))
		iMode = QAbstractItemView::NoSelection;
	else
		c->warning(__tr2qs_ctx("Invalid selection mode '%Q' assuming single", "objects"), &szMode);
	((QListWidget *)widget())->setSelectionMode(iMode);
	return true;
}
KVSO_CLASS_FUNCTION(listWidget, selectionMode)
{
	CHECK_INTERNAL_POINTER(widget())
	switch(((QListWidget *)widget())->selectionMode())
	{
		case QAbstractItemView::SingleSelection:
			c->returnValue()->setString("single");
			break;
		case QAbstractItemView::MultiSelection:
			c->returnValue()->setString("multi");
			break;
		case QAbstractItemView::ExtendedSelection:
			c->returnValue()->setString("extended");
			break;
		case QAbstractItemView::NoSelection:
			c->returnValue()->setString("none");
			break;
		default:
			c->returnValue()->setString("single");
			break;
	}
	return true;
}

KVSO_CLASS_FUNCTION(listWidget, insertWidgetItem)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject * pObject;
	kvs_int_t iIndex;
	kvs_hobject_t hWid;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("widget", KVS_PT_HOBJECT, 0, hWid)
	KVSO_PARAMETER("index", KVS_PT_INT, 0, iIndex)
	KVSO_PARAMETERS_END(c)
	pObject = KviKvsKernel::instance()->objectController()->lookupObject(hWid);
	if(pObject == this)
	{
		c->warning(__tr2qs_ctx("Can't insert the listwidget itself!", "objects"));
		return true;
	}
	if(!pObject->object()->isWidgetType())
	{
		c->warning(__tr2qs_ctx("Can't insert a non-widget object", "objects"));
		return true;
	}
	QWidget * wi = ((QWidget *)(pObject->object()));
	QListWidgetItem * item = ((QListWidget *)widget())->item(iIndex);
	((QListWidget *)widget())->setItemWidget(item, wi);
	return true;
}
KVSO_CLASS_FUNCTION(listWidget, selectedItems)
{
	CHECK_INTERNAL_POINTER(widget())
	QList<QListWidgetItem *> list = ((QListWidget *)widget())->selectedItems();
	KviKvsArray * pArray = new KviKvsArray();
	c->returnValue()->setArray(pArray);
	for(int i = 0; i < list.count(); i++)
	{
		kvs_int_t index = ((QListWidget *)widget())->row(list.at(i));
		pArray->set(i, new KviKvsVariant(index));
	}
	return true;
}

KVSO_CLASS_FUNCTION(listWidget, setSelected)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bSel;
	kvs_uint_t uIndex;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("index", KVS_PT_UNSIGNEDINTEGER, 0, uIndex)
	KVSO_PARAMETER("bEnabled", KVS_PT_BOOLEAN, 0, bSel)
	KVSO_PARAMETERS_END(c)
	((QListWidget *)widget())->item(uIndex)->setSelected(bSel);
	return true;
}
KVSO_CLASS_FUNCTION(listWidget, isSelected)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uIndex;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("index", KVS_PT_UNSIGNEDINTEGER, 0, uIndex)
	KVSO_PARAMETERS_END(c)
	c->returnValue()->setBoolean(((QListWidget *)widget())->item(uIndex)->isSelected());
	return true;
}
KVSO_CLASS_FUNCTION(listWidget, itemAt)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uX, uY;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("uX", KVS_PT_UNSIGNEDINTEGER, 0, uX)
	KVSO_PARAMETER("uY", KVS_PT_UNSIGNEDINTEGER, 0, uY)
	KVSO_PARAMETERS_END(c)
	c->returnValue()->setInteger(((QListWidget *)widget())->row(((QListWidget *)widget())->itemAt(QPoint(uX, uY))));
	return true;
}
KVSO_CLASS_FUNCTION(listWidget, itemRect)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uIndex;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("iIndex", KVS_PT_UNSIGNEDINTEGER, 0, uIndex)
	KVSO_PARAMETERS_END(c)
	QRect rect = ((QListWidget *)widget())->visualItemRect(((QListWidget *)widget())->item(uIndex));
	KviKvsArray * a = new KviKvsArray();
	a->set(0, new KviKvsVariant((kvs_int_t)rect.left()));
	a->set(1, new KviKvsVariant((kvs_int_t)rect.top()));
	a->set(2, new KviKvsVariant((kvs_int_t)rect.width()));
	a->set(3, new KviKvsVariant((kvs_int_t)rect.height()));
	c->returnValue()->setArray(a);
	return true;
}

KVSO_CLASS_FUNCTION(listWidget, currentItemChangedEvent)
{
	emitSignal("currentItemChanged", c, c->params());
	return true;
}
KVSO_CLASS_FUNCTION(listWidget, itemChangedEvent)
{
	emitSignal("itemChanged", c, c->params());
	return true;
}

KVSO_CLASS_FUNCTION(listWidget, itemEnteredEvent)
{
	emitSignal("itemEntered", c, c->params());

	return true;
}

//slots
void KvsObject_listWidget::currentItemChanged(QListWidgetItem * currentItem, QListWidgetItem *)
{
	if(!currentItem)
		callFunction(this, "currentItemChangedEvent", nullptr, nullptr);
	else
	{
		KviKvsVariantList params(new KviKvsVariant(currentItem->text()));
		callFunction(this, "currentItemChangedEvent", nullptr, &params);
	}
}
void KvsObject_listWidget::selectionChanged()
{
	callFunction(this, "selectionChangedEvent", nullptr, nullptr);
}

void KvsObject_listWidget::slotItemEntered(QListWidgetItem * item)
{
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)((QListWidget *)widget())->row(item)));
	callFunction(this, "itemEnteredEvent", nullptr, &params);
}

void KvsObject_listWidget::slotItemChanged(QListWidgetItem * item)
{
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)((QListWidget *)widget())->row(item)));
	callFunction(this, "itemChangedEvent", nullptr, &params);
}
