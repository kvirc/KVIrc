#ifndef _CLASS_MULTILINEEDIT_H_
#define _CLASS_MULTILINEEDIT_H_
//=============================================================================
//
//   File : class_multilineedit.cpp
//   Creation date : Mon Feb 19 2000 00:45:34 CET by Krzysztof Godlewski
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000 Krzysztof Godlewski
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================


#include "class_widget.h"
#include "object_macros.h"

class KviKvsObject_textedit : public KviKvsObject_widget
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_textedit)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);
	bool functionTextLine(KviKvsObjectFunctionCall *c);
	bool functionInsertLine(KviKvsObjectFunctionCall *c);
	bool functionRemoveLine(KviKvsObjectFunctionCall *c);
	bool functionNumLines(KviKvsObjectFunctionCall *c);
	bool functionAtBeginning(KviKvsObjectFunctionCall *c);
	bool functionAtEnd(KviKvsObjectFunctionCall *c);
	bool functionSetWordWrap(KviKvsObjectFunctionCall *c);
	bool functionWordWrap(KviKvsObjectFunctionCall *c);
	bool functionCursorPosition(KviKvsObjectFunctionCall *c);
	bool functionText(KviKvsObjectFunctionCall *c);
	bool functionLength(KviKvsObjectFunctionCall *c);
	bool functionSetMaxLines(KviKvsObjectFunctionCall *c);
	bool functionMaxLines(KviKvsObjectFunctionCall *c);
	bool functionSetReadOnly(KviKvsObjectFunctionCall *c);
	bool functionInsert(KviKvsObjectFunctionCall *c);
	bool functionAppend(KviKvsObjectFunctionCall *c);
	bool functionCopy(KviKvsObjectFunctionCall *c);
	bool functionPaste(KviKvsObjectFunctionCall *c);
	bool functionCut(KviKvsObjectFunctionCall *c);
	bool functionsetItalic(KviKvsObjectFunctionCall *c);
	bool functionitalic(KviKvsObjectFunctionCall *c);
	bool functionsetBold(KviKvsObjectFunctionCall *c);
	bool functionbold(KviKvsObjectFunctionCall *c);
	bool functionsetUnderline(KviKvsObjectFunctionCall *c);
	bool functionunderline(KviKvsObjectFunctionCall *c);
	bool functionzoomIn(KviKvsObjectFunctionCall *c);
	bool functionzoomOut(KviKvsObjectFunctionCall *c);
	bool functionundo(KviKvsObjectFunctionCall *c);
	bool functionredo(KviKvsObjectFunctionCall *c);
	bool functiondel(KviKvsObjectFunctionCall *c);
	bool functionclear(KviKvsObjectFunctionCall *c);
	bool functionsetUndoRedoEnabled(KviKvsObjectFunctionCall *c);
	bool functionisUndoRedoEnabled(KviKvsObjectFunctionCall *c);
	bool functionsetText(KviKvsObjectFunctionCall *c);
	bool functionsetHtmlText(KviKvsObjectFunctionCall *c);
	bool functionsetPlainText(KviKvsObjectFunctionCall *c);
	bool functionsetPointSize(KviKvsObjectFunctionCall *c);
	bool functionsetColor(KviKvsObjectFunctionCall *c);
	bool functionisUndoAvailable(KviKvsObjectFunctionCall *c);
	bool functionisRedoAvailable(KviKvsObjectFunctionCall *c);
	bool functionsetFamily(KviKvsObjectFunctionCall *c);
	bool functionsetTextFormat(KviKvsObjectFunctionCall *c);
	bool functiontextFormat(KviKvsObjectFunctionCall *c);
	bool functionloadFile(KviKvsObjectFunctionCall *c);
	bool functionlines(KviKvsObjectFunctionCall *c);
	bool functionselectAll(KviKvsObjectFunctionCall *c);
	bool functionsetModified(KviKvsObjectFunctionCall *c);
	bool functionModified(KviKvsObjectFunctionCall *c);
	bool functionsetAlignment(KviKvsObjectFunctionCall *c);

};

#endif	//!_CLASS_MULTILINEEDIT_H_
