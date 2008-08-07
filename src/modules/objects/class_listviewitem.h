#ifndef	_CLASS_LISTVIEWITEM_H_
#define	_CLASS_LISTVIEWITEM_H_
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

#include "kvi_tal_treewidget.h"
#include "class_widget.h"



#include "object_macros.h"

class KviKvsObject_treewidgetitem : public KviKvsObject
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_treewidgetitem)
	void childDestroyed();
protected:
	KviTalTreeWidgetItem * m_pListViewItem;
protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);
protected:
	bool function_setText(KviKvsObjectFunctionCall *c);
	bool function_insertItem(KviKvsObjectFunctionCall *c);
	bool function_setPixmap(KviKvsObjectFunctionCall *c);
	bool function_setRenameEnabled(KviKvsObjectFunctionCall *c);
	bool function_setEnabled(KviKvsObjectFunctionCall *c);
	bool function_isEnabled(KviKvsObjectFunctionCall *c);
	bool function_setOpen(KviKvsObjectFunctionCall *c);
	bool function_isOpen(KviKvsObjectFunctionCall *c);
	bool function_text(KviKvsObjectFunctionCall *c);
	bool function_setCheckable(KviKvsObjectFunctionCall *c);
	bool function_isCheckable(KviKvsObjectFunctionCall *c);
	bool function_setChecked(KviKvsObjectFunctionCall *c);
	bool function_isChecked(KviKvsObjectFunctionCall *c);
	bool function_firstChild(KviKvsObjectFunctionCall *c);
	//bool function_nextSibling(KviKvsObjectFunctionCall *c);
	bool function_setFlags(KviKvsObjectFunctionCall *c);

public:
	static kvs_hobject_t itemToHandle(KviTalTreeWidgetItem * it);
};



class KviKvsMdmStandardListViewItem : public KviTalTreeWidgetItem
{
public:
	KviKvsMdmStandardListViewItem(KviKvsObject_treewidgetitem * ob,KviTalTreeWidget * par);
	KviKvsMdmStandardListViewItem(KviKvsObject_treewidgetitem * ob,KviTalTreeWidgetItem * par);
	virtual ~KviKvsMdmStandardListViewItem();
protected:
	KviKvsObject_treewidgetitem * m_pMasterObject;
public:
	KviKvsObject_treewidgetitem * masterObject(){ return m_pMasterObject; }
};
/*
class KviKvsMdmCheckListViewItem : public KviTalCheckListItem
{
public:
	KviKvsMdmCheckListViewItem(KviKvsObject_treewidgetitem * ob,KviTalTreeWidget * par);
	KviKvsMdmCheckListViewItem(KviKvsObject_treewidgetitem * ob,KviTalTreeWidgetItem * par);
	virtual ~KviKvsMdmCheckListViewItem();
protected:
	KviKvsObject_treewidgetitem * m_pMasterObject;
public:
	KviKvsObject_treewidgetitem * masterObject(){ return m_pMasterObject; }
};
*/
#endif	// !_CLASS_LISTVIEWITEM_H_
