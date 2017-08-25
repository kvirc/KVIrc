//=============================================================================
//
//   File : KvsObject_multiLineEdit.cpp
//   Creation date : Mon Feb 19 2000 00:45:34 CET by Krzysztof Godlewski
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000 Krzysztof Godlewski
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include "KviError.h"
#include "kvi_debug.h"
#include "KviFileUtils.h"
#include "KviLocale.h"
#include "KviMemory.h"

#include <QTextBlock>
#include <QFile>
#include <QTextEdit>
#include <QTextStream>
#include <QTextCursor>

#include "KvsObject_multiLineEdit.h"

/*
	@doc: multilineedit
	@keyterms:
		multilineedit object class, line editor, input
	@title:
		multilineedit class
	@type:
		class
	@short:
		An editor for inputting larger portions of text
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		This is class an editor for inputting larger portions of text.
		Besides that, it has unfinished docs.
	@functions:
		!fn: <string> $textLine(<line:integer>)
		Returns the string at line <line>. Note that it can be an empty
		string.
		!fn: <integer> $numLines()
		Returns number of lines in the widget.
		!fn: <array> $cursorPosition()
		Returns current cursor position in "<line:uinteger>, <col:uinteger>" format. Both numbers start counting from zero.
		!fn: $setReadOnly(<bReadOnly:boolean>)
		Sets the editor to be read-only if bReadOnly is 1 or
		removes the read-only status if bReadOnly is 0
		!fn: <boolean> $atBeginning()
		Returns [b]1[/b] (true) if the cursor is placed at the beginning of the text; otherwise returns [b]0[/b] (false).
		!fn: <boolean> $atEnd()
		Returns [b]1[/b] (true) if the cursor is placed at the end of the text; otherwise returns [b]0[/b] (false).
		!fn: $setWordWrap(<wrap_mode:string>)
		Sets the word wrap mode to mode.[br]
		Valid Values are:
		[example]
			- NoWrap - Do not wrap the text.[br]
			- WidgetWidth - Wrap the text at the current width of the widget.[br]
			- FixedPixelWidth - Wrap the text at a fixed number of pixels from the widget's left side.[br]
			- FixedColumnWidth - Wrap the text at a fixed number of character columns from the widget's left side.
		[/example]
		!fn: <string> $wordWrap()
		Returns the word wrap mode. NoWrap, WidgetWidth, FixedPixelWidth, FixedColumnWidth.
		!fn: $setWordWrapWidth(<width:integer>)
		When [b]$wordWrap[/b] is set to FixedPixelWidth or FixedColumnWidth, sets the width at which
		text will be wrapped in columns or pixels.
		!fn: <integer> $wordWrapWidth()
		When [b]$wordWrap[/b] is set to FixedPixelWidth or FixedColumnWidth, returns the width at which
		text is wrapped in columns or pixels.
		!fn: <string> $text()
		Returns the multiline edits text.
		!fn: <string> $html()
		Returns the content of the widget as a HTML document.
		!fn: <integer> $length()
		Returns the number of characters in the text This function ignores newlines.
		!fn: $setMaxLines(<max_lines:integer>)
		Sets the max number of the lines to <max_lines>
		!fn: <integer> $maxLines()
		Returns the max number of the lines in the multiline edit.
		!fn: $insert(<text:string>)
		Inserts text at the current cursor position.
		!fn: $append(<text:string>)
		Appends a new paragraph with text to the end of the multiline edit.
		!fn: $copy()
		Copies any selected text from selection  to the clipboard.
		!fn: $cut()
		Copies the selected text from selection  to the clipboard and deletes it from the multiline edit.
		!fn: $paste()
		Pastes the text from the clipboard into the multiline edit at the current cursor position.
		!fn: $setFamily(<font_family:string>)
		Sets the font family of the current format to fontFamily.
		!fn: $setItalic(<bItalic:boolean>)
		If the bool value is [b]1[/b] sets the current format to italic; otherwise, if it's [b]0[/b] sets the current format to no-italic.
		!fn: $setBold(<bBold:boolean>)
		If the bool value is [b]1[/b] sets the current format to bold; otherwise, if it's [b]0[/b] sets the current format to no-bold.
		!fn: $setUnderline(<bUnderline:boolean>)
		If the bool value is [b]1[/b] sets the current format to underline; otherwise, if it's [b]0[/b] sets the current format to no-underline.
		!fn: <boolean> $italic()
		Returns [b]1[/b] (true) if the current format is italic; otherwise returns [b]0[/b] (false).
		!fn: <boolean> $bold()
		Returns [b]1[/b] (true) if the current format is bold; otherwise returns [b]0[/b] (false).
		!fn: <boolean> $underline()
		Returns [b]1[/b] (true) if the current format is underline; otherwise returns [b]0[/b] (false).
		!fn: $zoomIn(<zoom_range:integer>)
		Zooms in on the text by making the base font size range points larger.
		!fn: $zoomOut(<zoom_range:integer>)
		Zooms out on the text by making the base font size range points smaller.
		!fn: $undo()
		Undoes the last operation.
		!fn: $redo()
		Redoes the last operation.
		!fn: $clear()
		Deletes all the text in the multiline edit.
		!fn: $setUndoRedoEnabled(<bUndoRedo:boolean>)
		Sets whether undo/redo is enabled to the bool value.
		!fn: <boolean> $isUndoRedoEnabled()
		Returns [b]1[/b] (true) if undo/redo is enabled; otherwise returns [b]0[/b] (false).
		!fn: <integer> $undoDepth()
		Returns the depth of the undo history.
		!fn: $setText(<txt:string>)
		Sets the text edit's text to txt.
		!fn: $setColor(<rgb_value>)
		Sets the foreground color of this widget to <rgb_value>
		Valid values are:
		[example]
		- hex string: must be a string with 6 hexadecimal digits (like the ones used to[br]
		  specify colors in HTML pages). The first two digits specify[br]
		  the [b]red[/b] component, the third and fourth digit specify the [b]green[/b] component[br]
		  and the last two specify the [b]blue[/b] component.[br]
		- array(red:integer, green:integer, blue:integer)[br]
		- red:integer, green:integer, blue:integer.
		[/example]
		!fn: $setPointSize(<point_size:integer))
		Sets the point size of the font.
		!fn: $setLinkUnderline(<bLinkUnderline:boolean>)
		Sets to [b]1[/b] if you want that hypertext links will be underlined; otherwise sets to [b]0[/b].
		!fn: $setTextFormat(<textformat:string>)
		Sets the text format. Correct values are RichText, PlainText.
		!fn: <string> $textFormat()
		Returns the text format: RichText or PlainText.
		!fn: $loadFile(<path:string>[,<format:string>])
		Load the file specified in the <path>.[br]
		Valid values for <format> are:
		[example]
			- text - interprets the file as a plain text document.[br]
			- html - interprets the file as a HTML document.
		[/example]
		If <format> is omitted, the widget will try to guess whether or not the text is a HTML document.
		!fn: $saveFile(<path:string>[,<format:string>])
		Writes the content of the widget to the file specified by <path>.[br]
		Valid values for <format> are:
		[example]
			- text - writes the file as a plain text document (default).[br]
			- html - writes the file as a HTML document.
		[/example]
		The file is saved in UTF-8.
		!fn: <boolean> $isUndoAvailable ()
		Returns [b]1 (true)[/b] if undo is available; otherwise returns [b]0 (false)[/b].
		!fn: <boolean> $isRedoAvailable ()
		Returns [b]1 (true)[/b] if redo is available; otherwise returns [b]0 (false)[/b].
		!fn: <integer> $lines()
		Returns the number of lines in the multilineedit.
		!fn: <integer> $lineOfChar(<paragraph:integer>,<index:integer>)
		Returns the line number of the line in paragraph par in which the character at position index appears.[br]
		If there is no such paragraph or no such character at the index position [b]-1[/b] is returned.
		!fn: $setModified(<bModified:boolean>)
		Sets whether the document has been modified by the user. Valid Values are [b]1 (true)[/b] or [b]0 (false)[/b].
		!fn: $setAlignment(<alignment:string>)
		Sets the alignment of the current paragraph to <alignment>.[br]
		Valid values are:
		[example]
			- Auto - Aligns according to the language.[br]
			- Left - Aligns with the left edge.[br]
			- Right - Aligns with the right edge.[br]
			- Center - Centers in both dimensions.[br]
			- Justify - Justify the text.
		[/example]
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_textedit, "multilineedit", "widget")

KVSO_REGISTER_HANDLER(KvsObject_textedit, "textLine", functionTextLine)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "setWordWrap", functionSetWordWrap)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "wordWrap", functionWordWrap)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "setWordWrapWidth", functionSetWordWrapWidth)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "wordWrapWidth", functionWordWrapWidth)

KVSO_REGISTER_HANDLER(KvsObject_textedit, "atEnd", functionAtEnd)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "numLines", functionNumLines)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "atBeginning", functionAtBeginning)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "cursorPosition", functionCursorPosition)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "text", functionText)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "html", functionHtml)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "insert", functionInsert)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "maxLines", functionMaxLines)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "setMaxLines", functionSetMaxLines)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "append", functionAppend)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "copy", functionCopy)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "cut", functionCut)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "paste", functionPaste)

//->Set Style
KVSO_REGISTER_HANDLER(KvsObject_textedit, "setFamily", functionsetFamily)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "setItalic", functionsetItalic)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "setBold", functionsetBold)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "setUnderline", functionsetUnderline)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "italic", functionitalic)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "bold", functionbold)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "underline", functionunderline)

//->Zoom In, Out, To
KVSO_REGISTER_HANDLER(KvsObject_textedit, "zoomIn", functionzoomIn)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "zoomOut", functionzoomOut)

//->Undo & Redo
KVSO_REGISTER_HANDLER(KvsObject_textedit, "undo", functionundo)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "redo", functionredo)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "clear", functionclear)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "setUndoRedoEnabled", functionsetUndoRedoEnabled)

KVSO_REGISTER_HANDLER(KvsObject_textedit, "isUndoRedoEnabled", functionisUndoRedoEnabled)

//->Text color & others
KVSO_REGISTER_HANDLER(KvsObject_textedit, "setText", functionsetText)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "setColor", functionsetColor)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "setModified", functionsetModified)

KVSO_REGISTER_HANDLER(KvsObject_textedit, "setPointSize", functionsetPointSize)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "setReadOnly", functionSetReadOnly)

KVSO_REGISTER_HANDLER(KvsObject_textedit, "setTextFormat", functionsetTextFormat)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "textFormat", functiontextFormat)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "loadFile", functionloadFile);
KVSO_REGISTER_HANDLER(KvsObject_textedit, "saveFile", functionsaveFile);
KVSO_REGISTER_HANDLER(KvsObject_textedit, "setAlignment", functionsetAlignment)

KVSO_REGISTER_HANDLER(KvsObject_textedit, "lines", functionlines)
KVSO_REGISTER_HANDLER(KvsObject_textedit, "selectAll", functionselectAll)
KVSO_END_REGISTERCLASS(KvsObject_textedit)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_textedit, KvsObject_widget)

KVSO_END_CONSTRUCTOR(KvsObject_textedit)

KVSO_BEGIN_DESTRUCTOR(KvsObject_textedit)

KVSO_END_CONSTRUCTOR(KvsObject_textedit)

bool KvsObject_textedit::init(KviKvsRunTimeContext *, KviKvsVariantList *)
{
	SET_OBJECT(QTextEdit);
	return true;
}

bool KvsObject_textedit::functionSetReadOnly(KviKvsObjectFunctionCall * c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bEnabled", KVS_PT_BOOL, 0, bEnabled)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QTextEdit *)widget())->setReadOnly(bEnabled);
	return true;
}

bool KvsObject_textedit::functionTextLine(KviKvsObjectFunctionCall * c)
{
	kvs_int_t iBlock;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("line", KVS_PT_INT, 0, iBlock)
	KVSO_PARAMETERS_END(c)
	if(!widget())
		return true;
	if(iBlock > ((QTextEdit *)widget())->document()->blockCount() || iBlock < 0)
		c->warning(__tr2qs_ctx("No such line '%d'", "objects"), &iBlock);
	else
		c->returnValue()->setString(((QTextEdit *)widget())->document()->findBlockByNumber(iBlock).text());

	return true;
}
bool KvsObject_textedit::functionNumLines(KviKvsObjectFunctionCall * c)
{
	if(widget())
		c->returnValue()->setInteger(((QTextEdit *)widget())->document()->blockCount());
	return true;
}
bool KvsObject_textedit::functionCursorPosition(KviKvsObjectFunctionCall * c)
{
	if(!widget())
		return true;
	int iRow = ((QTextEdit *)widget())->textCursor().blockNumber();
	int iCol = ((QTextEdit *)widget())->textCursor().columnNumber();

	KviKvsArray * a = new KviKvsArray();
	a->set(0, new KviKvsVariant((kvs_int_t)iRow));
	a->set(1, new KviKvsVariant((kvs_int_t)iCol));
	c->returnValue()->setArray(a);
	return true;
}

bool KvsObject_textedit::functionAtBeginning(KviKvsObjectFunctionCall * c)
{
	if(!widget())
		return true;
	c->returnValue()->setBoolean(((QTextEdit *)widget())->textCursor().atStart());
	return true;
}

bool KvsObject_textedit::functionAtEnd(KviKvsObjectFunctionCall * c)
{
	if(!widget())
		return true;
	c->returnValue()->setBoolean(((QTextEdit *)widget())->textCursor().atEnd());
	return true;
}

bool KvsObject_textedit::functionSetWordWrap(KviKvsObjectFunctionCall * c)
{
	if(!widget())
		return true;
	QString szWrap;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("word_wrap", KVS_PT_STRING, 0, szWrap)
	KVSO_PARAMETERS_END(c)

	if(KviQString::equalCI(szWrap, "NoWrap"))
		((QTextEdit *)widget())->setLineWrapMode(QTextEdit::NoWrap);
	else if(KviQString::equalCI(szWrap, "WidgetWidth"))
		((QTextEdit *)widget())->setLineWrapMode(QTextEdit::WidgetWidth);
	else if(KviQString::equalCI(szWrap, "FixedPixelWidth"))
		((QTextEdit *)widget())->setLineWrapMode(QTextEdit::FixedPixelWidth);
	else if(KviQString::equalCI(szWrap, "FixedColumnWidth"))
		((QTextEdit *)widget())->setLineWrapMode(QTextEdit::FixedColumnWidth);
	else
		c->warning(__tr2qs_ctx("Unknown word wrap '%Q'", "objects"), &szWrap);
	return true;
}

bool KvsObject_textedit::functionWordWrap(KviKvsObjectFunctionCall * c)
{
	if(!widget())
		return true;
	QTextEdit::LineWrapMode mode = ((QTextEdit *)widget())->lineWrapMode();
	QString szWrapMode;
	if(mode == QTextEdit::NoWrap)
		szWrapMode = "NoWrap";
	else if(mode == QTextEdit::WidgetWidth)
		szWrapMode = "WidgetWidth";
	else if(mode == QTextEdit::FixedPixelWidth)
		szWrapMode = "FixedPixelWidth";
	else
		szWrapMode = "FixedColumnWidth";
	c->returnValue()->setString(szWrapMode);
	return true;
}

bool KvsObject_textedit::functionSetWordWrapWidth(KviKvsObjectFunctionCall * c)
{
	if(!widget())
		return true;
	kvs_int_t iWrap;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("word_wrap", KVS_PT_INTEGER, 0, iWrap)
	KVSO_PARAMETERS_END(c)

	((QTextEdit *)widget())->setLineWrapColumnOrWidth(iWrap);
	return true;
}

bool KvsObject_textedit::functionWordWrapWidth(KviKvsObjectFunctionCall * c)
{

	if(widget())
		c->returnValue()->setInteger(((QTextEdit *)widget())->lineWrapColumnOrWidth());
	return true;
}

bool KvsObject_textedit::functionText(KviKvsObjectFunctionCall * c)
{
	if(widget())
		c->returnValue()->setString(((QTextEdit *)widget())->document()->toPlainText());
	return true;
}

bool KvsObject_textedit::functionHtml(KviKvsObjectFunctionCall * c)
{
	if(widget())
		c->returnValue()->setString(((QTextEdit *)widget())->document()->toHtml(QByteArray("utf-8")));
	return true;
}

bool KvsObject_textedit::functionSetMaxLines(KviKvsObjectFunctionCall * c)
{
	kvs_uint_t imaxLines;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("max_lines", KVS_PT_UNSIGNEDINTEGER, 0, imaxLines)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QTextEdit *)widget())->document()->setMaximumBlockCount(imaxLines);
	return true;
}

bool KvsObject_textedit::functionMaxLines(KviKvsObjectFunctionCall * c)
{
	if(widget())
		c->returnValue()->setInteger(((QTextEdit *)widget())->document()->maximumBlockCount());
	return true;
}

bool KvsObject_textedit::functionInsert(KviKvsObjectFunctionCall * c)
{
	QString szInsert;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("text", KVS_PT_STRING, 0, szInsert)
	KVSO_PARAMETERS_END(c)
	if(widget())
	{
		QTextCursor cursor = ((QTextEdit *)widget())->textCursor();
		cursor.insertText(szInsert);
		((QTextEdit *)widget())->setTextCursor(cursor);
	}
	return true;
}

bool KvsObject_textedit::functionAppend(KviKvsObjectFunctionCall * c)
{
	QString szAppend;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("text", KVS_PT_STRING, 0, szAppend)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QTextEdit *)widget())->append(szAppend);
	return true;
}

bool KvsObject_textedit::functionCopy(KviKvsObjectFunctionCall *)
{
	if(widget())
		((QTextEdit *)widget())->copy();
	return true;
}

bool KvsObject_textedit::functionCut(KviKvsObjectFunctionCall *)
{
	if(widget())
		((QTextEdit *)widget())->cut();
	return true;
}

bool KvsObject_textedit::functionPaste(KviKvsObjectFunctionCall *)
{
	if(widget())
		((QTextEdit *)widget())->paste();
	return true;
}

//->Set Bold, Italic, Underline
bool KvsObject_textedit::functionsetBold(KviKvsObjectFunctionCall * c)
{
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bBold", KVS_PT_BOOL, 0, bFlag)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QTextEdit *)widget())->setFontWeight(bFlag ? QFont::Bold : QFont::Normal);
	return true;
}

bool KvsObject_textedit::functionsetUnderline(KviKvsObjectFunctionCall * c)
{
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bUnderline", KVS_PT_BOOL, 0, bFlag)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QTextEdit *)widget())->setFontUnderline(bFlag);
	return true;
}

bool KvsObject_textedit::functionsetItalic(KviKvsObjectFunctionCall * c)
{
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bItalic", KVS_PT_BOOL, 0, bFlag)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QTextEdit *)widget())->setFontItalic(bFlag);
	return true;
}

//-->Returns Italic, Bold, Underline.
bool KvsObject_textedit::functionbold(KviKvsObjectFunctionCall * c)
{
	if(widget())
		c->returnValue()->setBoolean(((QTextEdit *)widget())->fontWeight() >= QFont::Bold);
	return true;
}

bool KvsObject_textedit::functionitalic(KviKvsObjectFunctionCall * c)
{
	if(widget())
		c->returnValue()->setBoolean(((QTextEdit *)widget())->fontItalic());
	return true;
}

bool KvsObject_textedit::functionunderline(KviKvsObjectFunctionCall * c)
{
	if(widget())
		c->returnValue()->setBoolean(((QTextEdit *)widget())->fontUnderline());
	return true;
}

//->Zoom In, out at
bool KvsObject_textedit::functionzoomIn(KviKvsObjectFunctionCall * c)
{
	kvs_int_t iZoom;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("zoom_value", KVS_PT_INT, KVS_PF_OPTIONAL, iZoom)
	KVSO_PARAMETERS_END(c)
	if(!widget())
		return true;
	if(!iZoom)
		((QTextEdit *)object())->zoomIn();
	else
		((QTextEdit *)object())->zoomIn(iZoom);
	return true;
}

bool KvsObject_textedit::functionzoomOut(KviKvsObjectFunctionCall * c)
{
	kvs_int_t iZoom;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("zoom_value", KVS_PT_INT, KVS_PF_OPTIONAL, iZoom)
	KVSO_PARAMETERS_END(c)
	if(!widget())
		return true;
	if(!iZoom)
		((QTextEdit *)object())->zoomOut();
	else
		((QTextEdit *)object())->zoomOut(iZoom);
	return true;
}

//-> Undo & Redo functions
bool KvsObject_textedit::functionundo(KviKvsObjectFunctionCall *)
{
	if(widget())
		((QTextEdit *)widget())->undo();
	return true;
}

bool KvsObject_textedit::functionredo(KviKvsObjectFunctionCall *)
{
	if(widget())
		((QTextEdit *)widget())->redo();
	return true;
}

bool KvsObject_textedit::functionclear(KviKvsObjectFunctionCall *)
{
	if(widget())
		((QTextEdit *)widget())->clear();
	return true;
}

bool KvsObject_textedit::functionsetUndoRedoEnabled(KviKvsObjectFunctionCall * c)
{
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bUndoredo", KVS_PT_BOOL, 0, bFlag)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QTextEdit *)widget())->setUndoRedoEnabled(bFlag);
	return true;
}

bool KvsObject_textedit::functionisUndoRedoEnabled(KviKvsObjectFunctionCall * c)
{
	if(widget())
		c->returnValue()->setBoolean(((QTextEdit *)widget())->isUndoRedoEnabled());
	return true;
}

//-->Text & Color & Family
bool KvsObject_textedit::functionsetText(KviKvsObjectFunctionCall * c)
{
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("text", KVS_PT_STRING, 0, szText)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QTextEdit *)widget())->setText(szText);
	return true;
}

bool KvsObject_textedit::functionsetPlainText(KviKvsObjectFunctionCall * c)
{
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("text", KVS_PT_STRING, 0, szText)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QTextEdit *)widget())->setPlainText(szText);
	return true;
}

bool KvsObject_textedit::functionsetHtmlText(KviKvsObjectFunctionCall * c)
{
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("text", KVS_PT_STRING, 0, szText)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QTextEdit *)widget())->setHtml(szText);
	return true;
}

bool KvsObject_textedit::functionsetColor(KviKvsObjectFunctionCall * c)
{
	KviKvsVariant * pColArray;
	kvs_int_t iColR, iColG, iColB;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("red_or_array", KVS_PT_VARIANT, 0, pColArray)
	KVSO_PARAMETER("green", KVS_PT_INT, KVS_PF_OPTIONAL, iColG)
	KVSO_PARAMETER("blue", KVS_PT_INT, KVS_PF_OPTIONAL, iColB)
	KVSO_PARAMETERS_END(c)
	if(pColArray->isArray())
	{
		if(pColArray->array()->size() < 3)
		{
			c->error(__tr2qs_ctx("The array passed as parameter must contain at least 3 elements", "objects"));
			return false;
		}
		KviKvsVariant * pColR = pColArray->array()->at(0);
		KviKvsVariant * pColG = pColArray->array()->at(1);
		KviKvsVariant * pColB = pColArray->array()->at(2);

		if(!(pColR && pColG && pColB))
		{
			c->error(__tr2qs_ctx("One of the colors array parameters is empty", "objects"));
			return false;
		}
		if(!(pColR->asInteger(iColR) && pColG->asInteger(iColG) && pColB->asInteger(iColB)))
		{
			c->error(__tr2qs_ctx("One of the colors array parameters didn't evaluate to an integer", "objects"));
			return false;
		}
	}
	else
	{
		if(c->params()->count() == 1)
		{
			bool bOk, bOk1, bOk2;
			QString value;
			pColArray->asString(value);
			if(value.length() != 6)
			{
				c->warning(__tr2qs_ctx("A string of 6 hex digits is required", "objects"));
				return true;
			}
			QString buffer(value.mid(0, 2));
			iColR = buffer.toInt(&bOk, 16);
			buffer = value.mid(2, 2);
			iColG = buffer.toInt(&bOk1, 16);
			buffer = value.mid(4, 2);
			iColB = buffer.toInt(&bOk2, 16);
			if(!bOk || !bOk1 || !bOk2)
			{
				c->warning(__tr2qs_ctx("Not an hex digit", "objects"));
				return true;
			}
			if(widget())
				((QTextEdit *)widget())->setTextColor(QColor(iColR, iColG, iColB));
			return true;
		}
		if(c->params()->count() < 3)
		{
			c->error(__tr2qs_ctx("$setColor requires either an array as first parameter or three integers", "objects"));
			return false;
		}
		if(!pColArray->asInteger(iColR))
		{
			c->error(__tr2qs_ctx("The first parameter didn't evaluate to an array nor an integer", "objects"));
			return false;
		}
	}
	if(widget())
		((QTextEdit *)widget())->setTextColor(QColor(iColR, iColG, iColB));
	return true;
}

bool KvsObject_textedit::functionsetPointSize(KviKvsObjectFunctionCall * c)
{
	kvs_real_t dPointSize;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("point_size", KVS_PT_REAL, 0, dPointSize)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QTextEdit *)widget())->setFontPointSize(dPointSize);
	return true;
}

bool KvsObject_textedit::functionsetFamily(KviKvsObjectFunctionCall * c)
{
	QString szFamily;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("family", KVS_PT_STRING, 0, szFamily)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QTextEdit *)widget())->setFontFamily(szFamily);
	return true;
}

bool KvsObject_textedit::functionsetModified(KviKvsObjectFunctionCall * c)
{
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bModified", KVS_PT_BOOL, 0, bFlag)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QTextEdit *)widget())->document()->setModified(bFlag);
	return true;
}

bool KvsObject_textedit::functionModified(KviKvsObjectFunctionCall * c)
{
	if(widget())
		c->returnValue()->setBoolean(((QTextEdit *)widget())->document()->isModified());
	return true;
}

bool KvsObject_textedit::functiontextFormat(KviKvsObjectFunctionCall * c)
{
	if(!widget())
		return true;
	if(((QTextEdit *)widget())->acceptRichText())
		c->returnValue()->setString(QString("RichText"));
	else
		c->returnValue()->setString(QString("PlainText"));
	return true;
}

bool KvsObject_textedit::functionsetTextFormat(KviKvsObjectFunctionCall * c)
{
	QString szFormat;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("textformat", KVS_PT_STRING, 0, szFormat)
	KVSO_PARAMETERS_END(c)
	if(!widget())
		return true;
	if(KviQString::equalCI(szFormat, "PlainText"))
		((QTextEdit *)widget())->setAcceptRichText(false);
	else if(KviQString::equalCI(szFormat, "RichText"))
		((QTextEdit *)widget())->setAcceptRichText(true);
	else
		c->warning(__tr2qs_ctx("Unknown text format '%Q'", "objects"), &szFormat);
	return true;
}

bool KvsObject_textedit::functionloadFile(KviKvsObjectFunctionCall * c)
{
	QString szFile, szFormat;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("file_name", KVS_PT_STRING, 0, szFile)
	KVSO_PARAMETER("format", KVS_PT_STRING, KVS_PF_OPTIONAL, szFormat)
	KVSO_PARAMETERS_END(c)
	if(!QFile::exists(szFile))
	{
		c->warning(__tr2qs_ctx("I can't find the specified file '%Q'.", "objects"), &szFile);
		return true;
	}

	QFile file(szFile);
	if(!file.open(QIODevice::ReadOnly))
	{
		c->warning(__tr2qs_ctx("I can't read the file '%Q'.", "objects"), &szFile);
		return true;
	}

	QTextStream ts(&file);
	QString txt = ts.readAll();

	if (szFormat.isEmpty())
		//settext tries to understand if the text is html or plain
		((QTextEdit *)widget())->setText(txt);
	else if (KviQString::equalCI(szFormat, "text"))
		((QTextEdit *)widget())->setPlainText(txt);
	else if (KviQString::equalCI(szFormat, "html"))
		((QTextEdit *)widget())->setHtml(txt);
	else {
		c->warning(__tr2qs_ctx("Unknown text document format '%Q'", "objects"), &szFormat);
		((QTextEdit *)widget())->setText(txt);
	}

	file.close();
	return true;
}

bool KvsObject_textedit::functionsaveFile(KviKvsObjectFunctionCall * c)
{
	QString szFile, szFormat, szData;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("file_name", KVS_PT_STRING, 0, szFile)
	KVSO_PARAMETER("format", KVS_PT_STRING, KVS_PF_OPTIONAL, szFormat)
	KVSO_PARAMETERS_END(c)

	if (KviQString::equalCI(szFormat, "html"))
		szData = ((QTextEdit *)widget())->document()->toHtml(QByteArray("utf-8"));
	else {
		if (!szFormat.isEmpty() && !KviQString::equalCI(szFormat, "text"))
			c->warning(__tr2qs_ctx("Unknown text document format '%Q'. Writing the document as plain text.", "objects"), &szFormat);

		szData = ((QTextEdit *)widget())->toPlainText();
	}

	if(szFile.left(2) != "\\\\")
		KviFileUtils::adjustFilePath(szFile);

	bool bRet = KviFileUtils::writeFile(szFile, szData);
	if(!bRet)
		c->warning(__tr2qs("Failed to write to file '%Q': the destination couldn't be opened"), &szFile);
	return true;
}

bool KvsObject_textedit::functionsetAlignment(KviKvsObjectFunctionCall * c)
{
	QString szAlignment;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("alignment", KVS_PT_STRING, 0, szAlignment)
	KVSO_PARAMETERS_END(c)
	if(!widget())
		return true;
	if(KviQString::equalCI(szAlignment, "Left"))
		((QTextEdit *)widget())->setAlignment(Qt::AlignLeft);
	else if(KviQString::equalCI(szAlignment, "Right"))
		((QTextEdit *)widget())->setAlignment(Qt::AlignRight);
	else if(KviQString::equalCI(szAlignment, "Center"))
		((QTextEdit *)widget())->setAlignment(Qt::AlignCenter);
	else if(KviQString::equalCI(szAlignment, "Justify"))
		((QTextEdit *)widget())->setAlignment(Qt::AlignJustify);
	else
		c->warning(__tr2qs_ctx("Unknown alignment '%Q'", "objects"), &szAlignment);
	return true;
}

bool KvsObject_textedit::functionlines(KviKvsObjectFunctionCall * c)
{
	if(widget())
		c->returnValue()->setInteger(((QTextEdit *)widget())->document()->blockCount());
	return true;
}

bool KvsObject_textedit::functionselectAll(KviKvsObjectFunctionCall *)
{
	if(widget())
		((QTextEdit *)widget())->selectAll();
	return true;
}
