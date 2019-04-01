#ifndef _CLASS_TREEWIDGETITEM_H_
#define _CLASS_TREEWIDGETITEM_H_
//=============================================================================
//
//   File : KvsObject_treeWidgeteItem.h
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

#include "KvsObject_widget.h"
#include "object_macros.h"

#include <QTreeWidget>

class KvsObject_treeWidgetItem : public KviKvsObject
{
public:
	KVSO_DECLARE_OBJECT(KvsObject_treeWidgetItem)
	void childDestroyed();

protected:
	QTreeWidgetItem * m_pTreeWidgetItem;

protected:
	bool init(KviKvsRunTimeContext * pContext, KviKvsVariantList * pParams) override;

protected:
	bool setText(KviKvsObjectFunctionCall * c);
	bool insertItem(KviKvsObjectFunctionCall * c);
	bool setPixmap(KviKvsObjectFunctionCall * c);
	bool setItemEditable(KviKvsObjectFunctionCall * c);
	bool isItemEditable(KviKvsObjectFunctionCall * c);
	bool setEnabled(KviKvsObjectFunctionCall * c);
	bool isEnabled(KviKvsObjectFunctionCall * c);
	bool setOpen(KviKvsObjectFunctionCall * c);
	bool isOpen(KviKvsObjectFunctionCall * c);
	bool text(KviKvsObjectFunctionCall * c);
	bool setCheckable(KviKvsObjectFunctionCall * c);
	bool isCheckable(KviKvsObjectFunctionCall * c);
	bool setChecked(KviKvsObjectFunctionCall * c);
	bool isChecked(KviKvsObjectFunctionCall * c);
	bool setFlags(KviKvsObjectFunctionCall * c);

public:
	static kvs_hobject_t itemToHandle(QTreeWidgetItem * it);
};

class KviKvsStandardTreeWidgetItem : public QTreeWidgetItem
{
public:
	KviKvsStandardTreeWidgetItem(KvsObject_treeWidgetItem * ob, QTreeWidget * par);
	KviKvsStandardTreeWidgetItem(KvsObject_treeWidgetItem * ob, QTreeWidgetItem * par);
	~KviKvsStandardTreeWidgetItem();

protected:
	KvsObject_treeWidgetItem * m_pMasterObject;

public:
	KvsObject_treeWidgetItem * masterObject() { return m_pMasterObject; }
};

#endif // _CLASS_TREEWIDGETITEM_H_
