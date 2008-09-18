#ifndef	_CLASS_LINEEDIT_H_
#define	_CLASS_LINEEDIT_H_
//=============================================================================
//
//   File : class_lineedit.h
//   Creation date : Wed 20 Sep 2000 04:34:40 CEST by Krzysztof Godlewski
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000 Krzysztof Godlewski
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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



#include "class_widget.h"

//#warning "Signals !"

#include "object_macros.h"

class KviKvsObject_lineedit : public KviKvsObject_widget
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_lineedit)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);

	

	bool functionText(KviKvsObjectFunctionCall *c);
	bool functionSetText(KviKvsObjectFunctionCall *c);
	bool functionMaxLength(KviKvsObjectFunctionCall *c);
	bool functionSetMaxLength(KviKvsObjectFunctionCall *c);
	bool functionFrame(KviKvsObjectFunctionCall *c);
	bool functionSetCursorPosition(KviKvsObjectFunctionCall *c);
	bool functionCursorPosition(KviKvsObjectFunctionCall *c);
	bool functionSetFrame(KviKvsObjectFunctionCall *c);
	bool functionSelectAll(KviKvsObjectFunctionCall *c);
	bool functionSetSelection(KviKvsObjectFunctionCall *c);
	bool functionCopy(KviKvsObjectFunctionCall *c);
	bool functionCut(KviKvsObjectFunctionCall *c);

	bool functionPaste(KviKvsObjectFunctionCall *c);
	bool functionEchoMode(KviKvsObjectFunctionCall *c);
	bool functionSetEchoMode(KviKvsObjectFunctionCall *c);
	bool functionClear(KviKvsObjectFunctionCall *c);
	bool functionDragAndDrop(KviKvsObjectFunctionCall *c);
	bool functionSetInputMask(KviKvsObjectFunctionCall *c);
	bool functionSetReadOnly(KviKvsObjectFunctionCall *c);

	bool functionreturnPressedEvent(KviKvsObjectFunctionCall *c);

	bool functionlostFocusEvent(KviKvsObjectFunctionCall *c);

	bool functiontextChangedEvent(KviKvsObjectFunctionCall *c);


	bool functionsetInputValidator(KviKvsObjectFunctionCall *c);
protected slots:
	void slotreturnPressed();
	void slotlostFocus();
	void slottextChanged(const QString &);

};


#endif	// ! _CLASS_LINEEDIT_H_
