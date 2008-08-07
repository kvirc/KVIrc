//mdm:
//   File : class_ListViewItem.h
//   Creation date : Thu Feb 1 14:39:48 CEST 2005
//	 by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
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

#include "class_listviewitem.h"
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
	KVSO_REGISTER_HANDLER(KviKvsObject_treewidgetitem,"setText",function_setText)
	KVSO_REGISTER_HANDLER(KviKvsObject_treewidgetitem,"text",function_text)
	KVSO_REGISTER_HANDLER(KviKvsObject_treewidgetitem,"setPixmap",function_setPixmap);
	KVSO_REGISTER_HANDLER(KviKvsObject_treewidgetitem,"setRenameEnabled",function_setRenameEnabled);
	//KVSO_REGISTER_HANDLER(KviKvsObject_treewidgetitem,"setEnabled",function_setEnabled);
	//KVSO_REGISTER_HANDLER(KviKvsObject_treewidgetitem,"isEnabled",function_isEnabled);
	KVSO_REGISTER_HANDLER(KviKvsObject_treewidgetitem,"setOpen",function_setOpen);
	KVSO_REGISTER_HANDLER(KviKvsObject_treewidgetitem,"isOpen",function_isOpen);
	//KVSO_REGISTER_HANDLER(KviKvsObject_treewidgetitem,"setCheckable",function_setCheckable);
	//KVSO_REGISTER_HANDLER(KviKvsObject_treewidgetitem,"isCheckable",function_isCheckable);
	KVSO_REGISTER_HANDLER(KviKvsObject_treewidgetitem,"setChecked",function_setChecked);
	KVSO_REGISTER_HANDLER(KviKvsObject_treewidgetitem,"isChecked",function_isChecked);
	KVSO_REGISTER_HANDLER(KviKvsObject_treewidgetitem,"firstChild",function_firstChild);
	KVSO_REGISTER_HANDLER(KviKvsObject_treewidgetitem,"setFlags",function_setFlags);

	//	KVSO_REGISTER_HANDLER(KviKvsObject_treewidgetitem,"nextSibling",function_nextSibling);
KVSO_END_REGISTERCLASS(KviKvsObject_treewidgetitem)


KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_treewidgetitem,KviKvsObject)

	m_pListViewItem = 0;

KVSO_END_CONSTRUCTOR(KviKvsObject_treewidgetitem)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_treewidgetitem)

	if(m_pListViewItem)delete m_pListViewItem;

KVSO_END_CONSTRUCTOR(KviKvsObject_treewidgetitem)

bool KviKvsObject_treewidgetitem::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{
	if (!parentObject())
	{
			pContext->error(__tr2qs("The treewidgetitem cannot be parentless"));
			return false;
	}
	if(parentObject()->inherits("KviKvsObject_treewidgetitem"))
	{
		 m_pListViewItem = new KviKvsMdmStandardListViewItem(this,((KviKvsObject_treewidgetitem *)parentObject())->m_pListViewItem);
	} else {
		if(parentObject()->inherits("KviKvsObject_treewidget"))
			m_pListViewItem = new KviKvsMdmStandardListViewItem(this,((KviTalTreeWidget *)parentScriptWidget()));
		else {
			pContext->error(__tr2qs("The parent of the treewidgetitem must be either another treewidgetitem or a treewidget"));
			return false;
		}
	}
	return true;
}

void KviKvsObject_treewidgetitem::childDestroyed()
{
	if(m_pListViewItem == 0)return;
	m_pListViewItem = 0;
	die();
}

KviKvsMdmStandardListViewItem::KviKvsMdmStandardListViewItem(KviKvsObject_treewidgetitem * ob,KviTalTreeWidget * par)
:KviTalTreeWidgetItem(par), m_pMasterObject(ob)
{
}

KviKvsMdmStandardListViewItem::KviKvsMdmStandardListViewItem(KviKvsObject_treewidgetitem * ob,KviTalTreeWidgetItem * par)
:KviTalTreeWidgetItem(par), m_pMasterObject(ob)
{
}

KviKvsMdmStandardListViewItem::~KviKvsMdmStandardListViewItem()
{
	if(m_pMasterObject)m_pMasterObject->childDestroyed();
}
/*


KviKvsMdmCheckListViewItem::KviKvsMdmCheckListViewItem(KviKvsObject_treewidgetitem * ob,KviTalTreeWidget * par)
:KviTalCheckListItem(par,QString::null,KviTalCheckListItem::CheckBox), m_pMasterObject(ob)
{
}

KviKvsMdmCheckListViewItem::KviKvsMdmCheckListViewItem(KviKvsObject_treewidgetitem * ob,KviTalTreeWidgetItem * par)
:KviTalCheckListItem(par,QString::null,KviTalCheckListItem::CheckBox), m_pMasterObject(ob)
{
}

KviKvsMdmCheckListViewItem::~KviKvsMdmCheckListViewItem()
{
	if(m_pMasterObject)m_pMasterObject->childDestroyed();
}
*/

kvs_hobject_t KviKvsObject_treewidgetitem::itemToHandle(KviTalTreeWidgetItem * it)
{
	if(!it)return (kvs_hobject_t)0;
	KviKvsObject_treewidgetitem * pObject;
//	if(it->rtti() == 1)pObject = ((KviKvsMdmCheckListViewItem *)it)->masterObject();
//	else 
	pObject = ((KviKvsMdmStandardListViewItem *)it)->masterObject();
	if(!pObject)return (kvs_hobject_t)0;
	return pObject->handle();
}


bool KviKvsObject_treewidgetitem::function_setText(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uCol;
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("column",KVS_PT_UNSIGNEDINTEGER,0,uCol)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
		KVSO_PARAMETERS_END(c)
	if(m_pListViewItem)
		m_pListViewItem->setText(uCol,szText);
	return true;
}

bool KviKvsObject_treewidgetitem::function_firstChild(KviKvsObjectFunctionCall *c)
{
	if(m_pListViewItem)
		c->returnValue()->setHObject(itemToHandle((KviTalTreeWidgetItem*)m_pListViewItem->child(0)));
	else
		c->returnValue()->setHObject((kvs_hobject_t)0);
	return true;
}
/*
bool KviKvsObject_treewidgetitem::function_nextSibling(KviKvsObjectFunctionCall *c)
{
	if(m_pListViewItem)
		c->returnValue()->setHObject(itemToHandle(m_pListViewItem->nextSibling()));
	else
		c->returnValue()->setHObject((kvs_hobject_t)0);
	return true;
}
*/

bool KviKvsObject_treewidgetitem::function_setRenameEnabled(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uCol;
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("column",KVS_PT_UNSIGNEDINTEGER,0,uCol)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
		KVSO_PARAMETERS_END(c)
	if(m_pListViewItem)
		m_pListViewItem->setFlags(m_pListViewItem->flags()|Qt::ItemIsEditable);
	return true;

}

bool KviKvsObject_treewidgetitem::function_setFlags(KviKvsObjectFunctionCall *c)
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
						m_pListViewItem->setCheckState(0,Qt::Unchecked);
				sum = sum | flag;
			}
			else
				c->warning(__tr2qs("Unknown item flag: %s"),&itemflags.at(i));
				
		}
	if(m_pListViewItem)
		m_pListViewItem->setFlags((Qt::ItemFlags)sum);
	
	return true;



}
/*
bool KviKvsObject_treewidgetitem::function_setEnabled(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
		KVSO_PARAMETERS_END(c)
	if(m_pListViewItem)
		m_pListViewItem->setEnabled(bEnabled);
	return true;
}

bool KviKvsObject_treewidgetitem::function_isEnabled(KviKvsObjectFunctionCall *c)
{
	if(!m_pListViewItem)
	{
		c->returnValue()->setBoolean(false);
		return true;
	}
	c->returnValue()->setBoolean(m_pListViewItem->isEnabled());
	return true;
}
*/
bool KviKvsObject_treewidgetitem::function_setOpen(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(m_pListViewItem)
		m_pListViewItem->setExpanded(bEnabled);
	return true;
}

bool KviKvsObject_treewidgetitem::function_isOpen(KviKvsObjectFunctionCall *c)
{
	if(!m_pListViewItem)
	{
		c->returnValue()->setBoolean(false);
		return true;
	}
	c->returnValue()->setBoolean(m_pListViewItem->isExpanded());
	return true;
}


bool KviKvsObject_treewidgetitem::function_setChecked(KviKvsObjectFunctionCall *c)
{
	bool bChecked;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bChecked",KVS_PT_BOOL,0,bChecked)
	KVSO_PARAMETERS_END(c)
	if(!m_pListViewItem)return true;
//	if(m_pListViewItem->rtti() != 1)return true; // not a QCheckListItem
	((KviTalTreeWidgetItem *)m_pListViewItem)->setCheckState(0,bChecked?Qt::Checked:Qt::Unchecked);
	return true;
}

bool KviKvsObject_treewidgetitem::function_isChecked(KviKvsObjectFunctionCall *c)
{
	if(!m_pListViewItem)
	{
		c->returnValue()->setBoolean(false);
		return true;
	}
	/*if(m_pListViewItem->rtti() != 1)
	{
		c->returnValue()->setBoolean(false);
		return true;
	}
	*/
	c->returnValue()->setBoolean(((KviTalTreeWidgetItem *)m_pListViewItem)->checkState(0)==Qt::Checked?1:0);
	return true;
}
/*
bool KviKvsObject_treewidgetitem::function_setCheckable(KviKvsObjectFunctionCall *c)
{
	bool bCheckable;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bCheckable",KVS_PT_BOOL,0,bCheckable)
	KVSO_PARAMETERS_END(c)
	if(!m_pListViewItem)return true;
	if(bCheckable)
	{
		if(m_pListViewItem->rtti() == 1)return true; // a QCheckListItem already
		KviTalTreeWidgetItem * pParent = m_pListViewItem->parent();
		KviTalTreeWidget * pLV = (KviTalTreeWidget *)m_pListViewItem->listView();
		// swap the items, so we don't die now
		KviTalTreeWidgetItem * pThis = m_pListViewItem;
		m_pListViewItem = 0;
		delete pThis;
		if(pParent)
			m_pListViewItem = new KviKvsMdmCheckListViewItem(this,pParent);
		else
			m_pListViewItem = new KviKvsMdmCheckListViewItem(this,pLV);
	} else {
		if(m_pListViewItem->rtti() != 1)return true; // not a QCheckListItem yet
		KviTalTreeWidgetItem * pParent = m_pListViewItem->parent();
		KviTalTreeWidget * pLV = (KviTalTreeWidget *)m_pListViewItem->listView();
		// swap the items, so we don't die now
		KviTalTreeWidgetItem * pThis = m_pListViewItem;
		m_pListViewItem = 0;
		delete pThis;
		if(pParent)
			m_pListViewItem = new KviKvsMdmStandardListViewItem(this,pParent);
		else
			m_pListViewItem = new KviKvsMdmStandardListViewItem(this,pLV);
	}
	return true;
}

bool KviKvsObject_treewidgetitem::function_isCheckable(KviKvsObjectFunctionCall *c)
{
	if(!m_pListViewItem)
	{
		c->returnValue()->setBoolean(false);
		return true;
	}
	c->returnValue()->setBoolean(m_pListViewItem->rtti() == 1);
	return true;
}
*/
bool KviKvsObject_treewidgetitem::function_text(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uCol;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("column",KVS_PT_UNSIGNEDINTEGER,0,uCol)
	KVSO_PARAMETERS_END(c)
	if(m_pListViewItem)
		c->returnValue()->setString(m_pListViewItem->text(uCol));
	return true;
}

bool KviKvsObject_treewidgetitem::function_setPixmap(KviKvsObjectFunctionCall *c)
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
		if (!obPixmap->inherits("KviKvsObject_pixmap"))
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
	m_pListViewItem->setIcon(uCol,QIcon(*pix));
	return true;
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_listviewitem.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

