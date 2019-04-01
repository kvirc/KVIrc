#ifndef _CLASS_LINEEDIT_H_
#define _CLASS_LINEEDIT_H_
//=============================================================================
//
//   File : KvsObject_lineEdit.h
//   Creation date : Wed 20 Sep 2000 04:34:40 CEST by Krzysztof Godlewski
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

#include "KvsObject_widget.h"

//#warning "Signals!"
#include <QCompleter>
#include "object_macros.h"

class KvsObject_lineEdit : public KvsObject_widget
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KvsObject_lineEdit)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	QCompleter * m_pCompleter;

protected:
	bool init(KviKvsRunTimeContext * pContext, KviKvsVariantList * pParams) override;

	bool setCompleter(KviKvsObjectFunctionCall * c);
	bool enableCompleter(KviKvsObjectFunctionCall * c);
	bool disableCompleter(KviKvsObjectFunctionCall * c);
	bool unsetCompleter(KviKvsObjectFunctionCall * c);

	bool text(KviKvsObjectFunctionCall * c);
	bool setText(KviKvsObjectFunctionCall * c);
	bool maxLength(KviKvsObjectFunctionCall * c);
	bool setMaxLength(KviKvsObjectFunctionCall * c);
	bool frame(KviKvsObjectFunctionCall * c);
	bool setCursorPosition(KviKvsObjectFunctionCall * c);
	bool cursorPosition(KviKvsObjectFunctionCall * c);
	bool setFrame(KviKvsObjectFunctionCall * c);
	bool selectAll(KviKvsObjectFunctionCall * c);
	bool setSelection(KviKvsObjectFunctionCall * c);
	bool copy(KviKvsObjectFunctionCall * c);
	bool cut(KviKvsObjectFunctionCall * c);

	bool paste(KviKvsObjectFunctionCall * c);
	bool echoMode(KviKvsObjectFunctionCall * c);
	bool setEchoMode(KviKvsObjectFunctionCall * c);
	bool clear(KviKvsObjectFunctionCall * c);
	bool dragAndDrop(KviKvsObjectFunctionCall * c);
	bool setInputMask(KviKvsObjectFunctionCall * c);
	bool setReadOnly(KviKvsObjectFunctionCall * c);

	bool returnPressedEvent(KviKvsObjectFunctionCall * c);

	bool lostFocusEvent(KviKvsObjectFunctionCall * c);

	bool textChangedEvent(KviKvsObjectFunctionCall * c);

	bool setInputValidator(KviKvsObjectFunctionCall * c);
protected slots:
	void slotreturnPressed();
	void slotlostFocus();
	void slottextChanged(const QString &);
};

#endif // ! _CLASS_LINEEDIT_H_
