#ifndef _CLASS_COMBOBOX_H_
#define _CLASS_COMBOBOX_H_
//=============================================================================
//
//   File : KvsObject_comboBox.h
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

#include <qcombobox.h>
#include "KvsObject_widget.h"
#include "object_macros.h"

class KvsObject_comboBox : public KvsObject_widget
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KvsObject_comboBox)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	bool init(KviKvsRunTimeContext * pContext, KviKvsVariantList * pParams) override;

	bool insertItem(KviKvsObjectFunctionCall * c);
	bool changeItem(KviKvsObjectFunctionCall * c);
	bool removeItem(KviKvsObjectFunctionCall * c);
	bool clear(KviKvsObjectFunctionCall * c);
	//#warning "$insertPixItem"
	//#warning "$changePixItem"

	bool count(KviKvsObjectFunctionCall * c);
	bool setMaxCount(KviKvsObjectFunctionCall * c);
	bool maxCount(KviKvsObjectFunctionCall * c);
	bool current(KviKvsObjectFunctionCall * c);
	bool currentItem(KviKvsObjectFunctionCall * c);
	bool setEditable(KviKvsObjectFunctionCall * c);
	bool editable(KviKvsObjectFunctionCall * c);
	bool setEditText(KviKvsObjectFunctionCall * c);
	bool textAt(KviKvsObjectFunctionCall * c);
	bool textLineEdit(KviKvsObjectFunctionCall * c);
	bool setTextLineEdit(KviKvsObjectFunctionCall * c);

	bool setInsertionPolicy(KviKvsObjectFunctionCall * c);

	bool setCurrentItem(KviKvsObjectFunctionCall * c);
	bool popup(KviKvsObjectFunctionCall * c);
	bool textChangedEvent(KviKvsObjectFunctionCall * c);
	bool activatedEvent(KviKvsObjectFunctionCall * c);
protected slots:
	void editTextChanged(const QString &);
	void slotActivated(int);
};

#endif // !_CLASS_COMBOBOX_H_
