#ifndef _CLASS_LISTBOX_H_
#define _CLASS_LISTBOX_H_
//=============================================================================
//
//   File : KvsObject_listWidget.h
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

#include "KvsObject_widget.h"
#include <QListWidget>
#include "object_macros.h"

class KvsObject_listWidget : public KvsObject_widget
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KvsObject_listWidget)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	bool init(KviKvsRunTimeContext * pContext, KviKvsVariantList * pParams) override;

	bool insertItem(KviKvsObjectFunctionCall * c);
	bool changeItem(KviKvsObjectFunctionCall * c);
	bool removeItem(KviKvsObjectFunctionCall * c);
	bool count(KviKvsObjectFunctionCall * c);
	bool currentText(KviKvsObjectFunctionCall * c);
	bool currentItem(KviKvsObjectFunctionCall * c);
	bool textAt(KviKvsObjectFunctionCall * c);
	bool setCurrentItem(KviKvsObjectFunctionCall * c);
	bool clear(KviKvsObjectFunctionCall * c);
	bool setSelectionMode(KviKvsObjectFunctionCall * c);
	bool selectionMode(KviKvsObjectFunctionCall * c);
	bool insertWidgetItem(KviKvsObjectFunctionCall * c);
	bool isSelected(KviKvsObjectFunctionCall * c);
	bool setSelected(KviKvsObjectFunctionCall * c);
	bool setFlags(KviKvsObjectFunctionCall * c);
	bool selectedItems(KviKvsObjectFunctionCall * c);
	bool setForeground(KviKvsObjectFunctionCall * c);
	bool setIcon(KviKvsObjectFunctionCall * c);
	bool setChecked(KviKvsObjectFunctionCall * c);
	bool setFont(KviKvsObjectFunctionCall * c);
	bool isChecked(KviKvsObjectFunctionCall * c);
	bool itemAt(KviKvsObjectFunctionCall * c);
	bool itemRect(KviKvsObjectFunctionCall * c);

	// events
	bool currentItemChangedEvent(KviKvsObjectFunctionCall * c);
	bool itemChangedEvent(KviKvsObjectFunctionCall * c);
	bool itemEnteredEvent(KviKvsObjectFunctionCall * c);

protected slots:
	void selectionChanged();
	void currentItemChanged(QListWidgetItem *, QListWidgetItem *);
	void slotItemEntered(QListWidgetItem * item);
	void slotItemChanged(QListWidgetItem * item);
};

#endif // !_CLASS_COMBOBOX_H_
