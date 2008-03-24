#ifndef	_CLASS_COMBOBOX_H_
#define	_CLASS_COMBOBOX_H_

//vim: ts=8
//   File : class_combobox.h
//   Creation date : Thu Mar 22 20:57:28 CET 2001 by Krzysztof Godlewski
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


#include <qcombobox.h>
#include "class_widget.h"
#include "object_macros.h"

class KviKvsObject_combobox : public KviKvsObject_widget
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_combobox)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);
	
	bool functioninsertItem(KviKvsObjectFunctionCall *c);
	bool functionchangeItem(KviKvsObjectFunctionCall *c);
	bool functionremoveItem(KviKvsObjectFunctionCall *c);
	bool functionclear(KviKvsObjectFunctionCall *c);
//#warning "$insertPixItem"
//#warning "$changePixItem"

	bool functioncount(KviKvsObjectFunctionCall *c);
	bool functionsetMaxCount(KviKvsObjectFunctionCall *c);
	bool functionmaxCount(KviKvsObjectFunctionCall *c);
	bool functioncurrent(KviKvsObjectFunctionCall *c);
	bool functioncurrentItem(KviKvsObjectFunctionCall *c);
	bool functionsetEditable(KviKvsObjectFunctionCall *c);
	bool functioneditable(KviKvsObjectFunctionCall *c);
	bool functionsetEditText(KviKvsObjectFunctionCall *c);
	bool functiontextAt(KviKvsObjectFunctionCall *c);
	bool functiontextLineEdit(KviKvsObjectFunctionCall *c);
	bool functionsetTextLineEdit(KviKvsObjectFunctionCall *c);

	bool functionsetInsertionPolicy(KviKvsObjectFunctionCall *c);

	bool functionsetCurrentItem(KviKvsObjectFunctionCall *c);
	bool functionpopup(KviKvsObjectFunctionCall *c);
	bool functiontextChangedEvent(KviKvsObjectFunctionCall *c);
	bool functionactivatedEvent(KviKvsObjectFunctionCall *c);
protected slots:
	void slottextChanged(const QString &);
	void slotActivated(int);
};

#endif	// !_CLASS_COMBOBOX_H_
