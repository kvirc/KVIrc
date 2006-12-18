#ifndef	_CLASS_LISTBOX_H_
#define	_CLASS_LISTBOX_H_

//vim: ts=8
//   File : class_listbox.h
//   Creation date : Sat Oct 2 03:40:28 CET 2004 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
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


#include "class_widget.h"

class QListBox;
class QListBoxItem;

#include "object_macros.h"

class KviKvsObject_listbox: public KviKvsObject_widget
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_listbox)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);
	

	bool functioninsertItem(KviKvsObjectFunctionCall *c);
	bool functionchangeItem(KviKvsObjectFunctionCall *c);
	bool functionremoveItem(KviKvsObjectFunctionCall *c);
	bool functioncount(KviKvsObjectFunctionCall *c);
	bool functioncurrentText(KviKvsObjectFunctionCall *c);
	bool functioncurrentItem(KviKvsObjectFunctionCall *c);
	bool functiontextAt(KviKvsObjectFunctionCall *c);
	bool functionsetCurrentItem(KviKvsObjectFunctionCall *c);
	bool functionclear(KviKvsObjectFunctionCall *c);
	bool functionsetSelectionMode(KviKvsObjectFunctionCall *c);
	bool functionselectionMode(KviKvsObjectFunctionCall *c);
	bool functionsetSelected(KviKvsObjectFunctionCall *c);
	bool functionisSelected(KviKvsObjectFunctionCall *c);
	bool functioncurrentItemChangeEvent(KviKvsObjectFunctionCall *c);
protected slots:
	void selectionChanged();
	void currentItemChanged(QListBoxItem *);
};


#endif	// !_CLASS_COMBOBOX_H_
