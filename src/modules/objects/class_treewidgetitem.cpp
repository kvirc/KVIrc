//=============================================================================
//
//   File : class_treewidgetitem.cpp
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

#include "class_treewidgetitem.h"
#include "class_pixmap.h"

#include "kvi_error.h"
#include "kvi_debug.h"
#include "kvi_locale.h"
#include "kvi_iconmanager.h"

const int item_flags[] = {
	Qt::ItemIsSelectable,
	Qt::ItemIsEditable,
	Qt::ItemIsDragEnabled,
	Qt::ItemIsDropEnabled,
	Qt::ItemIsUserCheckable,
	Qt::ItemIsEnabled,
	Qt::ItemIsTristate
};

const char * const itemflags_tbl[] = {
	"selectable",
	"editable",
	"dragEnabled",
	"dropEnabled",
	"userCheckable",
	"enabled",
	"tristate"
};

#define itemflags_num	(sizeof(itemflags_tbl) / sizeof(itemflags_tbl[0]))



/*
	@doc: treewidgetitem
	@keyterms:
		treewidget treewidgetitem class
	@title:
		treewidgetitem class
	@type:
		class
	@short:
		A treewidgetitem class
	@inherits:
		[class]treewidgetitem[/class]
	@description:
		The treewidgetitem class implements a list view item.
		A list view item is a multi-column object capable of displaying itself in a [class]treewidget[/class].
		To use this class you must instantiate it with another treewidgetitem or a [class]treewidget[/class]
		as parent. The item will be automatically displayed.
		You can set the text and a pixmap in each column and you can make it checkable
		with [classfnc:treewidgetitem]$setCheckable[/classfnc]().
		A checkable treewidgetitem will display a small check mark in the first column.
	@functions:
		!fn: $setText(<column:integer>,<text:string>)
		Sets the text in column column to text, if column is a valid column number and text is different from the existing text.

		!fn: <string> $text(<column:integer>)
		Returs the text of the specified column.

		!fn: $setPixmap(<column:integer>,<pixmap:hobject or imageID>)
		Sets the pixmap in column column to pm, if pm is non-null and different from the current pixmap, and if column is non-negative.
		Pixmap can be a Kvirc imageid, an image file or a [class]pixmap[/class] object.

		!fn: $setRenameEnabled(<col:integer>,<bEnabled:boolean>)
		If b is TRUE (1), this item can be in-place renamed in the column col by the user; otherwise it cannot be renamed in-place.

		!fn: $setEnabled(<bEnabled:boolean>)
		Enables or disables the item

		!fn: $isEnabled()
		Returns $true if this item is enabled and $false otherwise

		!fn: $setOpen(<bOpen:boolean>)
		Opens or closes the item to show its children items

		!fn: $isOpen()
		Returns the open state of this item

		!fn: $setCheckable(<bCheckable:boolean>)
		Makes this item checkable or not. This function should be called immediately
		after the item creation: changing this property later at runtime may have
		strange results (like the item being moved inside the list, text disappearing,
		hidden children etc... don't do it :D ).

		!fn: $isCheckable()
		Returns $true if this item is checkable and $false otherwise

		!fn: $setChecked(<bChecked:boolean>)
		Sets this item to be checked or not. [classfnc:treewidgetitem]$setCheckable[/classfnc]() must
		have been previously called.

		!fn: $isChecked()
		Returns the check status of this item. [classfnc:treewidgetitem]$setCheckable[/classfnc]() must
		have been previously called.

		!fn: <treewidgetitem> $firstChild()
		Returns the first child item of this treewidgetitem or $null if this item has no children.

		!fn: <treewidgetitem> $nextSibling()
		Returns the next sibling item of this treewidgetitem or $null if there are no sibling items.
		Next sibling stands for the item at the same tree level coming just after :)
*/


//===========================================================================================

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_treewidgetitem,"listviewitem","object")
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_treewidgetitem,setText)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_treewidgetitem,text)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_treewidgetitem,setPixmap);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_treewidgetitem,setRenameEnabled);
	//KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_treewidgetitem,"setEnabled",function_setEnabled);
	//KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_treewidgetitem,"isEnabled",function_isEnabled);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_treewidgetitem,setOpen);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_treewidgetitem,isOpen);
	//KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_treewidgetitem,"setCheckable",function_setCheckable);
	//KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_treewidgetitem,"isCheckable",function_isCheckable);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_treewidgetitem,setChecked);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_treewidgetitem,isChecked);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_treewidgetitem,firstChild);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_treewidgetitem,setFlags);

	//KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_treewidgetitem,"nextSibling",function_nextSibling);
KVSO_END_REGISTERCLASS(KviKvsObject_treewidgetitem)


KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_treewidgetitem,KviKvsObject)

	m_pTreeWidgetItem = 0;

KVSO_END_CONSTRUCTOR(KviKvsObject_treewidgetitem)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_treewidgetitem)

	if(m_pTreeWidgetItem)delete m_pTreeWidgetItem;

KVSO_END_CONSTRUCTOR(KviKvsObject_treewidgetitem)

bool KviKvsObject_treewidgetitem::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *)
{
	if (!parentObject())
	{
			pContext->error(__tr2qs("The listviewitem cannot be parentless"));
			return false;
	}
	if(parentObject()->inheritsClass("listviewitem"))
	{
		 m_pTreeWidgetItem = new KviKvsStandardTreeWidgetItem(this,((KviKvsObject_treewidgetitem *)parentObject())->m_pTreeWidgetItem);
	} else {
		if(parentObject()->inheritsClass("listview"))
			m_pTreeWidgetItem = new KviKvsStandardTreeWidgetItem(this,((KviTalTreeWidget *)parentScriptWidget()));
		else {
			pContext->error(__tr2qs("The parent of the listwidgetitem must be either another listwidgetitem or a listwidget"));
			return false;
		}
	}
	return true;
}

void KviKvsObject_treewidgetitem::childDestroyed()
{
	if(m_pTreeWidgetItem == 0)return;
	m_pTreeWidgetItem = 0;
	die();
}

KviKvsStandardTreeWidgetItem::KviKvsStandardTreeWidgetItem(KviKvsObject_treewidgetitem * ob,KviTalTreeWidget * par)
:KviTalTreeWidgetItem(par), m_pMasterObject(ob)
{
}

KviKvsStandardTreeWidgetItem::KviKvsStandardTreeWidgetItem(KviKvsObject_treewidgetitem * ob,KviTalTreeWidgetItem * par)
:KviTalTreeWidgetItem(par), m_pMasterObject(ob)
{
}

KviKvsStandardTreeWidgetItem::~KviKvsStandardTreeWidgetItem()
{
	if(m_pMasterObject)m_pMasterObject->childDestroyed();
}

kvs_hobject_t KviKvsObject_treewidgetitem::itemToHandle(KviTalTreeWidgetItem * it)
{
	if(!it)return (kvs_hobject_t)0;
	KviKvsObject_treewidgetitem * pObject;
	pObject = ((KviKvsStandardTreeWidgetItem *)it)->masterObject();
	if(!pObject)return (kvs_hobject_t)0;
	return pObject->handle();
}

KVSO_CLASS_FUNCTION(treewidgetitem,setText)
{
	kvs_uint_t uCol;
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("column",KVS_PT_UNSIGNEDINTEGER,0,uCol)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
		KVSO_PARAMETERS_END(c)
	if(m_pTreeWidgetItem)
		m_pTreeWidgetItem->setText(uCol,szText);
	return true;
}

KVSO_CLASS_FUNCTION(treewidgetitem,firstChild)
{
	if(m_pTreeWidgetItem)
		c->returnValue()->setHObject(itemToHandle((KviTalTreeWidgetItem*)m_pTreeWidgetItem->child(0)));
	else
		c->returnValue()->setHObject((kvs_hobject_t)0);
	return true;
}
/*
KVSO_CLASS_FUNCTION(treewidgetitem,nextSibling)
{
	if(m_pTreeWidgetItem)
		c->returnValue()->setHObject(itemToHandle(m_pTreeWidgetItem->nextSibling()));
	else
		c->returnValue()->setHObject((kvs_hobject_t)0);
	return true;
}
*/
KVSO_CLASS_FUNCTION(treewidgetitem,setRenameEnabled)
{
	kvs_uint_t uCol;
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("column",KVS_PT_UNSIGNEDINTEGER,0,uCol)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
		KVSO_PARAMETERS_END(c)
	if(m_pTreeWidgetItem)
		m_pTreeWidgetItem->setFlags(m_pTreeWidgetItem->flags()|Qt::ItemIsEditable);
	return true;
}

KVSO_CLASS_FUNCTION(treewidgetitem,setFlags)
{
	QStringList itemflags;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("flags",KVS_PT_STRINGLIST,KVS_PF_OPTIONAL,itemflags)
	KVSO_PARAMETERS_END(c)
	int flag,sum=0;
	for ( int i=0;i<itemflags.count();i++)
	{
		flag = 0;
		for(unsigned int j = 0; j < itemflags_num; j++)
		{
			if(KviQString::equalCI(itemflags.at(i), itemflags_tbl[j]))
			{
				flag=item_flags[j];
				break;
			}
		}
		if(flag){
			if (flag==Qt::ItemIsUserCheckable)
					m_pTreeWidgetItem->setCheckState(0,Qt::Unchecked);
			sum = sum | flag;
		}
		else
			c->warning(__tr2qs("Unknown item flag '%Q'"),&itemflags.at(i));
	}
	if(m_pTreeWidgetItem)
		m_pTreeWidgetItem->setFlags((Qt::ItemFlags)sum);

	return true;
}
/*
KVSO_CLASS_FUNCTION(treewidgetitem,setEnabled)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
		KVSO_PARAMETERS_END(c)
	if(m_pTreeWidgetItem)
		m_pTreeWidgetItem->setEnabled(bEnabled);
	return true;
}

KVSO_CLASS_FUNCTION(treewidgetitem,isEnabled)
{
	if(!m_pTreeWidgetItem)
	{
		c->returnValue()->setBoolean(false);
		return true;
	}
	c->returnValue()->setBoolean(m_pTreeWidgetItem->isEnabled());
	return true;
}
*/
KVSO_CLASS_FUNCTION(treewidgetitem,setOpen)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(m_pTreeWidgetItem)
		m_pTreeWidgetItem->setExpanded(bEnabled);
	return true;
}

KVSO_CLASS_FUNCTION(treewidgetitem,isOpen)
{
	if(!m_pTreeWidgetItem)
	{
		c->returnValue()->setBoolean(false);
		return true;
	}
	c->returnValue()->setBoolean(m_pTreeWidgetItem->isExpanded());
	return true;
}

KVSO_CLASS_FUNCTION(treewidgetitem,setChecked)
{
	bool bChecked;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bChecked",KVS_PT_BOOL,0,bChecked)
	KVSO_PARAMETERS_END(c)
	if(!m_pTreeWidgetItem)return true;
	//if(m_pTreeWidgetItem->rtti() != 1)return true; // not a QCheckListItem
	((KviTalTreeWidgetItem *)m_pTreeWidgetItem)->setCheckState(0,bChecked?Qt::Checked:Qt::Unchecked);
	return true;
}

KVSO_CLASS_FUNCTION(treewidgetitem,isChecked)
{
	if(!m_pTreeWidgetItem)
	{
		c->returnValue()->setBoolean(false);
		return true;
	}
	/*if(m_pTreeWidgetItem->rtti() != 1)
	{
		c->returnValue()->setBoolean(false);
		return true;
	}
	*/
	c->returnValue()->setBoolean(((KviTalTreeWidgetItem *)m_pTreeWidgetItem)->checkState(0)==Qt::Checked?1:0);
	return true;
}
/*
KVSO_CLASS_FUNCTION(treewidgetitem,setCheckable)
{
	bool bCheckable;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bCheckable",KVS_PT_BOOL,0,bCheckable)
	KVSO_PARAMETERS_END(c)
	if(!m_pTreeWidgetItem)return true;
	if(bCheckable)
	{
		if(m_pTreeWidgetItem->rtti() == 1)return true; // a QCheckListItem already
		KviTalTreeWidgetItem * pParent = m_pTreeWidgetItem->parent();
		KviTalTreeWidget * pLV = (KviTalTreeWidget *)m_pTreeWidgetItem->listView();
		// swap the items, so we don't die now
		KviTalTreeWidgetItem * pThis = m_pTreeWidgetItem;
		m_pTreeWidgetItem = 0;
		delete pThis;
		if(pParent)
			m_pTreeWidgetItem = new KviKvsCheckTreeWidgetItem(this,pParent);
		else
			m_pTreeWidgetItem = new KviKvsCheckTreeWidgetItem(this,pLV);
	} else {
		if(m_pTreeWidgetItem->rtti() != 1)return true; // not a QCheckListItem yet
		KviTalTreeWidgetItem * pParent = m_pTreeWidgetItem->parent();
		KviTalTreeWidget * pLV = (KviTalTreeWidget *)m_pTreeWidgetItem->listView();
		// swap the items, so we don't die now
		KviTalTreeWidgetItem * pThis = m_pTreeWidgetItem;
		m_pTreeWidgetItem = 0;
		delete pThis;
		if(pParent)
			m_pTreeWidgetItem = new KviKvsStandardTreeWidgetItem(this,pParent);
		else
			m_pTreeWidgetItem = new KviKvsStandardTreeWidgetItem(this,pLV);
	}
	return true;
}

KVSO_CLASS_FUNCTION(treewidgetitem,isCheckable)
{
	if(!m_pTreeWidgetItem)
	{
		c->returnValue()->setBoolean(false);
		return true;
	}
	c->returnValue()->setBoolean(m_pTreeWidgetItem->rtti() == 1);
	return true;
}
*/
KVSO_CLASS_FUNCTION(treewidgetitem,text)
{
	kvs_uint_t uCol;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("column",KVS_PT_UNSIGNEDINTEGER,0,uCol)
	KVSO_PARAMETERS_END(c)
	if(m_pTreeWidgetItem)
		c->returnValue()->setString(m_pTreeWidgetItem->text(uCol));
	return true;
}

KVSO_CLASS_FUNCTION(treewidgetitem,setPixmap)
{
	kvs_uint_t uCol;
	KviKvsObject *obPixmap;
	kvs_hobject_t obHpixmap;
	KviKvsVariant * vPixmap;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("column",KVS_PT_UNSIGNEDINTEGER,0,uCol)
		KVSO_PARAMETER("pixmap",KVS_PT_VARIANT,0,vPixmap)
	KVSO_PARAMETERS_END(c)
	QPixmap *pix = 0;
	if(vPixmap->isHObject())
	{
		vPixmap->asHObject(obHpixmap);
		obPixmap=KviKvsKernel::instance()->objectController()->lookupObject(obHpixmap);
		if (!obPixmap->inheritsClass("pixmap"))
		{
			c->warning(__tr2qs("Pixmap object or image Id required"));
			return true;
		}
		pix=((KviKvsObject_pixmap *)obPixmap)->getPixmap();
	} else {
		QString szPix;
		vPixmap->asString(szPix);
		pix=g_pIconManager->getImage(szPix);
		if(!pix)
		{
			c->warning(__tr2qs("Error occured: the suitable file '%Q' is not of the correct format or it is not a valid icon number."),&szPix);
			return true;
		}
	}
	m_pTreeWidgetItem->setIcon(uCol,QIcon(*pix));
	return true;
}

