//vim: ts=8
//   File : class_multilineedit.cpp
//   Creation date : Mon Feb 19 00:45:34 CET by Krzysztof Godlewski
//
//	 Modification date: Thu 15 Feb 2005 22:00 :00
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
//


#include "kvi_error.h"
#include "kvi_debug.h"

#include "kvi_locale.h"
#include "kvi_malloc.h"
#include <qfile.h>


#include "class_multilineedit.h"

//->Tables for Text Format
static char * const mod_tbl[] =	{
					"PlainText",
					"RichText",
					"AutoText",
					"LogText"
				  };

static int mod_cod[] =	{
				Qt::PlainText,
				Qt::RichText,
				Qt::AutoText,
				Qt::LogText
			};

#define mod_num			(sizeof(mod_tbl) / sizeof(mod_tbl[0]))



/*
	@doc:	multilineedit
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
		!fn: $insertLine(<text:string>, <line:integer>)
		Inserts <text> at line number <line>. If line is less than
		zero, or larger than the number of rows, the new text is
		put at the end. If txt contains newline characters, several
		lines are inserted. The <line> parameter is optional. If it is
		ommited, the line is put at end.
		!fn: $removeLine(<line:integer>)
		Removes line number <line>.
		!fn: $insertAt(<txt:string>, <line:integer>, <col:integer>)
		Inserts txt at line number line, after character number col in
		the line. If txt contains newline characters, new lines are
		inserted.
		!fn: <integer> $numLines()
		Returns number of lines in the widget.
		!fn: $setCursorPosition(<line:integer>, <col:integer>, <mark:boolean>)
		Sets the cursor position to character number col in line
		number line. The parameters are adjusted to lie within the
		legal range.[br]
		If mark is 1, the selection is cleared. otherwise it
		is extended
		!fn: <line,col:integer> $cursorPosition()
		Returns current cursor position in "<line:integer>, <col:integer>" format.
		!fn: $setReadOnly(<bReadOnly:boolean>)
		Sets the editor to be read-only if bReadOnly is 1 or
		removes the read-only status is ReadOnly is 0
		!fn: <boolean> $atBeginning()
		Returns 1(TRUE) if the cursor is placed at the beginning of the text; otherwise returns 0(FALSE).
		!fn: <boolean> $atEnd()
		Returns 1(TRUE) if the cursor is placed at the end of the text; otherwise returns 0(FALSE).
		!fn: $setWordWrap(<wrap_mode:string>)
		Sets the word wrap mode to mode. Valid Values are:[br]
		- NoWrap - Do not wrap the text.[br]
		- WidgetWidth - Wrap the text at the current width of the widget.[br]
		- FixedPixelWidth - Wrap the text at a fixed number of pixels from the widget's left side.[br]
		- FixedColumnWidth - Wrap the text at a fixed number of character columns from the widget's left side.
		!fn: <string> $wordWrap()
		Returns the word wrap mode. NoWrap, WidgetWidth, FixedPixelWidth, FixedColumnWidth.
		!fn: <string> $text()
		Returns the multiline edit's text.
		!fn: <integer> $length()
		Returns the number of characters in the text This function ignores newlines.
		!fn: $setMaxLines(<mac_lines:integer>)
		Sets the max number of the lines to <a>
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
		If the bool value is 1 sets the current format to italic; otherwise, if it's 0 sets the current format to no-italic.
		!fn: $setBold(<bBold:boolean>)
		If the bool value is 1 sets the current format to bold; otherwise, if it's 0 sets the current format to no-bold.
		!fn: $setUnderline(<bUnderline:boolean>)
		If the bool value is 1 sets the current format to underline; otherwise, if it's 0 sets the current format to no-underline.
		!fn: <boolean> $italic()
		Returns 1(TRUE) if the current format is italic; otherwise returns 0(FALSE).
		!fn: <boolean> $bold()
		Returns 1(TRUE) if the current format is bold; otherwise returns 0(FALSE).
		!fn: <boolean> $underline()
		Returns 1(TRUE) if the current format is underline; otherwise returns 0(FALSE).
		!fn: $zoomIn(<zoom_range:integer>)
		Zooms in on the text by making the base font size range points larger.
		!fn: $zoomOut(<zoom_range:integer>)
		Zooms out on the text by making the base font size range points smaller.
		!fn: $zoomTo(<zoom_size:integer>)
		Zooms the text by making the base font size <size> points.
		!fn: $undo()
		Undoes the last operation.
		!fn: $redo()
		Redoes the last operation.
		!fn: $clear()
		Deletes all the text in the multiline edit.
		!fn: $setUndoRedoEnabled(<bUndoRedo:boolean>)
		Sets whether undo/redo is enabled to the bool value.
		!fn: $setUndoDepth(<undo_depth:integer>)
		Sets the depth of the undo history to x.
		!fn: <boolean> $isUndoRedoEnabled()
		Returns 1 (TRUE) if undo/redo is enabled; otherwise returns 0 (FALSE).
		!fn: <integer> $undoDepth()
		Returns the depth of the undo history.
		!fn: $indent()
		Re-indents the current paragraph.
		!fn: $setText(<txt:string>)
		Sets the text edit's text to txt.
		!fn: $setPointSize(<point_size:integer))
		Sets the point size of the font.
		// findme
		!fn: $setColor(<rgb_value>)
		Sets the foreground color of this widget to <rgb_value>:valid values are:
		- hex string: must be a string with 6 hexadecimal digits (like the ones used to
		specify colors in html pages). The first two digits specify
		the RED component, the third and fourth digit specify the GREEN component
		and the last two specify the BLUE component.
		For example "FFFF00" means full red, full green and no blue that gives
		a yellow color, "808000" designates a brown color (dark yellow),
		"A000A0" is a kind of violet.
		- array(red:integer,green:integer,blue:integer)
		- red:integer,green:integer,blue:integer.
		!fn: $setModified(<bModified:boolean>)
		Sets whether the document has been modified by the user. Valid Values are 1(TRUE) or 0(FALSE)
		!fn: $insertParagraph(<text:string>,<paragraph:integer>)
		Inserts text as a new paragraph at position <paragraph>.
		!fn: $removeParagraph(<paragraph:integer>)
		Removes the paragraph <paragraph>.
		!fn: $setLinkUnderline(<bLinkUnderline:boolean>)
		Sets to 1 if you want that hypertext links will be underlined; otherwise sets to 0.
		!fn: $setOverwriteMode(<bOverwrite:boolean>)
		Sets the Multilineedit edit's overwrite mode to b (1=Enabled or 0=Disabled) .
		!fn: $setTextFormat(<textformat:string>)
		Sets the text format. Correct values are RichText, PlainText, LogText or AutoText.
		!fn: <string> $textFormat()
		Returns the text format: rich text, plain text, log text or auto text.
		// findme
		!fn: $setParagraphBackgroundColor(<paragraph:integer>,<exadecimal color value>)
		Sets the background color of the paragraph <paragraph> to color value specified.[br]
		Example: %mymultiline->$setParagraphBackgroundColor(2,0C686F)
		!fn: $clearParagraphBackground(<paragraph:integer>)
		Clear the background color of the paragraph <paragraph>.
		!fn: $loadFile(<path:string>)
		Load the file specified in the <path>, also html files.
		!fn: <boolean> $isUndoAvailable ()
		Returns 1(TRUE) if undo is available; otherwise returns 0(FALSE).
		!fn: <boolean> $isRedoAvailable ()
		Returns 1(TRUE) if redo is available; otherwise returns 0(FALSE).
		!fn: $setAlignment(<alignment:string>)
		Sets the alignment of the current paragraph to <alignment>. Valid values are:[br]
		- AlignAuto - Aligns according to the language.[br]
		- AlignLeft - Aligns with the left edge.[br]
		- AlignRight - Aligns with the right edge.[br]
		- AlignCenter - Centers in both dimensions.
		!fn: $setVerticalAlignment(<vertical_alignment:string>)
		Sets the vertical alignment of the current format to <Valignemnt>. Valid Values are:[br]
		- AlignNormal - Normal alignment.[br]
		- AlignSuperScript - Superscript.[br]
		- AlignSubScript - Subscript.
		!fn: $setAutoFormatting(<afvalue:string>)
		Sets the enabled set of auto formatting features to afvalue. Valid values are: [br]
		- AutoNone - Do not perform any automatic formatting[br]
		- AutoBulletList - Only automatically format bulletted lists[br]
		- AutoAll - Apply all available autoformatting
		!fn: $setWrapPolicy(<wraph_policy:string>)
		Sets the word wrap policy, at <wrpl>. Valid values are:[br]
		- AtWhiteSpace - Don't use this deprecated value (it is a synonym for AtWordBoundary which you should use instead).[br]
		- Anywhere - Break anywhere, including within words.[br]
		- AtWordBoundary - Break lines at word boundaries, e.g. spaces or newlines[br]
		- AtWordOrDocumentBoundary - Break lines at whitespace, e.g. spaces or newlines if possible. Break it anywhere otherwise.[br]
		!fn: <integer> $paragraphs()
		Returns the number of paragraphs in the text.
		!fn: <integer> $lines()
		Returns the number of lines in the multilineedit.
		!fn: <integer> $linesOfParagraph(<paragraph:integer>)
		Returns the number of lines in paragraph <paragraph>, or -1 if there is no paragraph with index <paragraph>.
		!fn: <integer> $lineOfChar(<paragraph:integer>,<index:integer>)
		Returns the line number of the line in paragraph par in which the character at position index appears.[br]
		If there is no such paragraph or no such character at the index position  -1 is returned.
		!fn: <integer> $paragraphLenght(<paragraph:integer>)
		Returns the length of the paragraph par, or -1 if there is no paragraph with index par.

*/


KVSO_BEGIN_REGISTERCLASS(KviKvsObject_mledit,"multilineedit","widget")

	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"textLine", functionTextLine)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"insertLine", functionInsertLine)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"removeLine", functionRemoveLine)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"insertAt", functionInsertAt)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"numLines", functionNumLines)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"setCursorPosition", functionSetCursorPosition)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"cursorPosition", functionCursorPosition)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"atBeginning", functionAtBeginning)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"atEnd", functionAtEnd)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"setWordWrap", functionSetWordWrap)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"wordWrap", functionWordWrap)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"text", functionText)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"length", functionLength);
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"setMaxLines", functionSetMaxLines)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"maxLines", functionMaxLines)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"insert", functionInsert)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"append", functionAppend)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"copy", functionCopy)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"cut", functionCut)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"paste", functionPaste)

//->Set Style
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"setFamily" , functionsetFamily)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"setItalic", functionsetItalic)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"setBold", functionsetBold)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"setUnderline", functionsetUnderline)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"italic", functionitalic)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"bold", functionbold)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"underline", functionunderline)
//->Zoom In, Out, To
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"zoomIn", functionzoomIn)
    KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"zoomOut", functionzoomOut)
    KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"zoomTo", functionzoomTo)
//->Undo & Redo
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"undo", functionundo)
    KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"redo", functionredo)
    KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"del", functiondel)
    KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"clear", functionclear)
    KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"setUndoRedoEnabled", functionsetUndoRedoEnabled)
    KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"setUndoDepth", functionsetUndoDepth)
    KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"isUndoRedoEnabled", functionsetUndoRedoEnabled)
    KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"undoDepth", functionundoDepth)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"indent", functionundoDepth)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"isUndoAvailable", functionisUndoAvailable)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"isUndoRedoAvailable", functionisRedoAvailable)
//->Text color & others
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"setText", functionsetText)
    KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"setPointSize", functionsetPointSize)
    KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"setColor", functionsetColor)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"setModified" , functionsetModified)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"setLinkUnderline" , functionsetLinkUnderline)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"setOverwriteMode" , functionsetOverwriteMode)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"setReadOnly",functionSetReadOnly)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"setTextFormat" , functionsetTextFormat)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"textFormat" , functiontextFormat)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"setParagraphBackgroundColor", functionsetParagraphBackgroundColor)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"clearParagraphBackgroundColor", functionsetParagraphBackgroundColor)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"loadFile" , functionloadFile); // let's fantasy
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"setAlignment" , functionsetAlignment)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"setAutoFormatting" , functionsetAutoFormatting)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"setVerticalAlignment" , functionsetVerticalAlignment)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"setWrapPolicy" , functionsetWrapPolicy)
//->Paragraph & line
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"paragraphs" , functionparagraphs)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"paragraphLength" , functionparagraphLength)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"insertParagraph" , functioninsertParagraph)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"removeParagraph" , functionremoveParagraph)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"lines" , functionlines)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"lineOfChar" , functionlineOfChar)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"linesOfParagraph" , functionlinesOfParagraph)
	KVSO_REGISTER_HANDLER(KviKvsObject_mledit,"selectAll" , functionselectAll)


KVSO_END_REGISTERCLASS(KviKvsObject_mledit)


KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_mledit,KviKvsObject_widget)


KVSO_END_CONSTRUCTOR(KviKvsObject_mledit)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_mledit)

KVSO_END_CONSTRUCTOR(KviKvsObject_mledit)

bool KviKvsObject_mledit::init(KviKvsRunTimeContext * pContext,KviKvsVariantList * pParams)
{
	setObject(new QMultiLineEdit(parentScriptWidget(),getName()),true);
	return true;
}


bool KviKvsObject_mledit::functionSetReadOnly(KviKvsObjectFunctionCall *c)

{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QMultiLineEdit *)widget())->setReadOnly(bEnabled);
	return true;
}


bool KviKvsObject_mledit::functionTextLine(KviKvsObjectFunctionCall *c)

{
	kvs_int_t iLine;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("line",KVS_PT_INT,0,iLine)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	if(iLine > ((QMultiLineEdit *)widget())->numLines() || iLine < 0) c->warning(__tr2qs("No such line number"));
	else
		c->returnValue()->setString(((QMultiLineEdit *)widget())->textLine(iLine));
	return true;
}

bool KviKvsObject_mledit::functionInsertLine(KviKvsObjectFunctionCall *c)

{
	kvs_int_t iLine;
	QString szInsert;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szInsert)
		KVSO_PARAMETER("line",KVS_PT_INT,KVS_PF_OPTIONAL,iLine)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	((QMultiLineEdit *)widget())->insertLine(szInsert,iLine);
	return true;
}

bool KviKvsObject_mledit::functionRemoveLine(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iLine;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("line",KVS_PT_INT,0,iLine)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	if(iLine > ((QMultiLineEdit *)widget())->numLines() || iLine < 0)
			c->warning("No such line number");
	else ((QMultiLineEdit *)widget())->removeLine(iLine);
	return true;
}

bool KviKvsObject_mledit::functionInsertAt(KviKvsObjectFunctionCall *c)

{
	kvs_int_t iLine,iCol;
	QString szInsert;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szInsert)
		KVSO_PARAMETER("line",KVS_PT_INT,0,iLine)
		KVSO_PARAMETER("col",KVS_PT_INT,0,iCol)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	if(iLine < 0 || iCol < 0 || iLine > ((QMultiLineEdit *)widget())->numLines())
			c->warning("No such line number");
	else
		((QMultiLineEdit *)widget())->insertAt(szInsert,iLine,iCol);
	return true;
}

bool KviKvsObject_mledit::functionNumLines(KviKvsObjectFunctionCall *c)

{
	if(widget()) c->returnValue()->setInteger(((QMultiLineEdit *)widget())->numLines());
	return true;
}

bool KviKvsObject_mledit::functionSetCursorPosition(KviKvsObjectFunctionCall *c)
{

	bool bFlag;
	kvs_int_t iLine,iCol;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("line",KVS_PT_UNSIGNEDINTEGER,0,iLine)
		KVSO_PARAMETER("col",KVS_PT_UNSIGNEDINTEGER,0,iCol)
		KVSO_PARAMETER("mark",KVS_PT_BOOL,0,bFlag)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QMultiLineEdit *)widget())->setCursorPosition(iLine, iCol, bFlag);
	return true;
}

bool KviKvsObject_mledit::functionCursorPosition(KviKvsObjectFunctionCall *c)
{
	if(!widget()) return true;
	int line, col;
	#if QT_VERSION >= 300
		((QMultiLineEdit *)widget())->getCursorPosition(&line, &col);
	#else
		((QMultiLineEdit *)widget())->cursorPosition(&line, &col);
	#endif
	KviKvsArray * a = new KviKvsArray();
	a->set(0,new KviKvsVariant((kvs_int_t)line));
	a->set(1,new KviKvsVariant((kvs_int_t)col));
	c->returnValue()->setArray(a);
	return true;
}

bool KviKvsObject_mledit::functionAtBeginning(KviKvsObjectFunctionCall *c)
{
	if(!widget()) return true;
	c->returnValue()->setBoolean(((QMultiLineEdit *)widget())->atBeginning());
	return true;
}

bool KviKvsObject_mledit::functionAtEnd(KviKvsObjectFunctionCall *c)
{
	if(!widget()) return true;
	c->returnValue()->setBoolean(((QMultiLineEdit *)widget())->atEnd());
	return true;
}

bool KviKvsObject_mledit::functionSetWordWrap(KviKvsObjectFunctionCall *c)
{
	if(!widget())return true;
	QString szWrap;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("word_wrap",KVS_PT_STRING,0,szWrap)
	KVSO_PARAMETERS_END(c)
	if(KviQString::equalCI(szWrap,"NoWrap"))
			((QMultiLineEdit *)widget())->setWordWrap(QTextEdit::NoWrap);
	else if(KviQString::equalCI(szWrap,"WidgetWidth"))
			((QMultiLineEdit *)widget())->setWordWrap(QTextEdit::WidgetWidth);
	else if(KviQString::equalCI(szWrap,"FixedPixelWidth"))
			((QMultiLineEdit *)widget())->setWordWrap(QTextEdit::FixedPixelWidth);
	else if(KviQString::equalCI(szWrap,"FixedColumnWidth"))
			((QMultiLineEdit *)widget())->setWordWrap(QTextEdit::FixedColumnWidth);
	else c->warning(__tr2qs("Unknown word wrap '%Q'"),&szWrap);
	return true;
}

bool KviKvsObject_mledit::functionsetWrapPolicy(KviKvsObjectFunctionCall *c)
{
	if(!widget())return true;
	QString szPolicy;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("wrap_policy",KVS_PT_STRING,0,szPolicy)
	KVSO_PARAMETERS_END(c)
	if(KviQString::equalCI(szPolicy,"AtWhiteSpace)"))
			((QMultiLineEdit *)widget())->setWrapPolicy(QTextEdit::AtWhiteSpace);
	else if(KviQString::equalCI(szPolicy,"Anywhere"))
			((QMultiLineEdit *)widget())->setWrapPolicy(QTextEdit::Anywhere);
	else if(KviQString::equalCI(szPolicy,"AtWordBoundary"))
			((QMultiLineEdit *)widget())->setWrapPolicy(QTextEdit::AtWordBoundary);
	else if(KviQString::equalCI(szPolicy,"AtWordOrDocumentBoundary"))
			((QMultiLineEdit *)widget())->setWrapPolicy(QTextEdit::AtWordOrDocumentBoundary);
	else c->warning(__tr2qs("Unknown wrap policy'%Q'"),&szPolicy);
	return true;
}

bool KviKvsObject_mledit::functionWordWrap(KviKvsObjectFunctionCall *c)
{
	if(widget())
		c->returnValue()->setBoolean(((QMultiLineEdit *)widget())->wordWrap() & QMultiLineEdit::WidgetWidth);
	return true;
}

bool KviKvsObject_mledit::functionText(KviKvsObjectFunctionCall *c)
{
	if(widget())
		c->returnValue()->setString(((QMultiLineEdit *)widget())->text());
	return true;
}

bool KviKvsObject_mledit::functionLength(KviKvsObjectFunctionCall *c)
{
	if(widget())
			c->returnValue()->setInteger(((QMultiLineEdit *)widget())->length());
	return true;
}

bool KviKvsObject_mledit::functionSetMaxLines(KviKvsObjectFunctionCall *c)
{

	kvs_int_t imaxLines;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("max_lines",KVS_PT_UNSIGNEDINTEGER,0,imaxLines)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QMultiLineEdit *)widget())->setMaxLines(imaxLines);
	return true;
}

bool KviKvsObject_mledit::functionMaxLines(KviKvsObjectFunctionCall *c)

{
	if(widget())
		c->returnValue()->setInteger(((QMultiLineEdit *)widget())->maxLines());
	return true;
}

bool KviKvsObject_mledit::functionInsert(KviKvsObjectFunctionCall *c)

{
	QString szInsert;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szInsert)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QMultiLineEdit *)widget())->insert(szInsert);
	return true;
}

bool KviKvsObject_mledit::functionAppend(KviKvsObjectFunctionCall *c)

{
	QString szAppend;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szAppend)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QMultiLineEdit *)widget())->append(szAppend);
	return true;
}

bool KviKvsObject_mledit::functionCopy(KviKvsObjectFunctionCall *c)
{
	if(widget())
		((QMultiLineEdit *)widget())->copy();
	return true;
}

bool KviKvsObject_mledit::functionCut(KviKvsObjectFunctionCall *c)
{
	if(widget())
		((QMultiLineEdit *)widget())->cut();
	return true;
}

bool KviKvsObject_mledit::functionPaste(KviKvsObjectFunctionCall *c)
{
	if(widget())
		((QMultiLineEdit *)widget())->paste();
	return true;
}
//->Set Bold, Italic, Underline
bool KviKvsObject_mledit::functionsetBold(KviKvsObjectFunctionCall *c)
{
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bBold",KVS_PT_BOOL,0,bFlag)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QMultiLineEdit *)widget())->setBold(bFlag);
	return true;
}
bool KviKvsObject_mledit::functionsetUnderline(KviKvsObjectFunctionCall *c)
{
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bUnderline",KVS_PT_BOOL,0,bFlag)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QMultiLineEdit *)widget())->setUnderline(bFlag);
	return true;
}

bool KviKvsObject_mledit::functionsetItalic(KviKvsObjectFunctionCall *c)
{
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bItalic",KVS_PT_BOOL,0,bFlag)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QMultiLineEdit *)widget())->setItalic(bFlag);
	return true;
}


//-->Returns Italic , Bold, Underline.
bool KviKvsObject_mledit::functionbold(KviKvsObjectFunctionCall *c)
{
	if(widget())
		c->returnValue()->setBoolean(((QMultiLineEdit *)widget())->bold());
	return true;
}
bool KviKvsObject_mledit::functionitalic(KviKvsObjectFunctionCall *c)
{
	if(widget())
		c->returnValue()->setBoolean(((QMultiLineEdit *)widget())->italic());
	return true;
}
bool KviKvsObject_mledit::functionunderline(KviKvsObjectFunctionCall *c)
{
	if(widget())
		c->returnValue()->setBoolean(((QMultiLineEdit *)widget())->underline());
	return true;
}
//->Zoom In, out at
bool KviKvsObject_mledit::functionzoomIn(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iZoom;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("zoom_value",KVS_PT_INT,KVS_PF_OPTIONAL,iZoom)
	KVSO_PARAMETERS_END(c)
	if (!widget()) return true;
	if (!iZoom)
		((QMultiLineEdit *)object())->zoomIn();
    else
		((QMultiLineEdit *)object())->zoomIn(iZoom);
    return true;
}

bool KviKvsObject_mledit::functionzoomOut(KviKvsObjectFunctionCall *c)
{
    kvs_int_t iZoom;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("zoom_value",KVS_PT_INT,KVS_PF_OPTIONAL,iZoom)
	KVSO_PARAMETERS_END(c)
	if (!widget()) return true;
	if (!iZoom)
		((QMultiLineEdit *)object())->zoomOut();
    else
		((QMultiLineEdit *)object())->zoomOut(iZoom);
    return true;
}
bool KviKvsObject_mledit::functionzoomTo(KviKvsObjectFunctionCall *c)
{
    kvs_int_t iZoom;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("zoom_size",KVS_PT_INT,0,iZoom)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QMultiLineEdit *)object())->zoomTo(iZoom);
    return true;
}
//-> Undo & Redo functions

bool KviKvsObject_mledit::functionundo(KviKvsObjectFunctionCall *c)
{
    if(widget())
        ((QMultiLineEdit *)widget())->undo();
    return true;
}
bool KviKvsObject_mledit::functionredo(KviKvsObjectFunctionCall *c)
{
    if(widget())
        ((QMultiLineEdit *)widget())->redo();
    return true;
}
bool KviKvsObject_mledit::functionclear(KviKvsObjectFunctionCall *c)
{
    if(widget())
        ((QMultiLineEdit *)widget())->clear();
    return true;
}
bool KviKvsObject_mledit::functiondel(KviKvsObjectFunctionCall *c)
{
    if(widget())
		((QMultiLineEdit *)widget())->del();
    return true;
}
bool KviKvsObject_mledit::functionindent(KviKvsObjectFunctionCall *c)
{
    if(widget())
        ((QMultiLineEdit *)widget())->indent();
    return true;
}

bool KviKvsObject_mledit::functionsetUndoRedoEnabled(KviKvsObjectFunctionCall *c)
{
    bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bUndoredo",KVS_PT_BOOL,0,bFlag)
	KVSO_PARAMETERS_END(c)
	if (widget())
       ((QMultiLineEdit *)widget())->setUndoRedoEnabled(bFlag);
    return true;
}

bool KviKvsObject_mledit::functionisUndoRedoEnabled(KviKvsObjectFunctionCall *c)
{
	if(widget())
		c->returnValue()->setBoolean(((QMultiLineEdit *)widget())->isUndoRedoEnabled());
    return true;
}
bool KviKvsObject_mledit::functionisUndoAvailable(KviKvsObjectFunctionCall *c)
{
	if(widget())
		c->returnValue()->setBoolean(((QMultiLineEdit *)widget())->isUndoAvailable());
    return true;
}
bool KviKvsObject_mledit::functionisRedoAvailable(KviKvsObjectFunctionCall *c)
{
	if(widget())
		c->returnValue()->setBoolean(((QMultiLineEdit *)widget())->isRedoAvailable());
    return true;
}
bool KviKvsObject_mledit::functionsetUndoDepth(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iDepth;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("undo_depth",KVS_PT_UNSIGNEDINTEGER,0,iDepth)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QMultiLineEdit *)object())->setUndoDepth(iDepth);
    return true;

}
bool KviKvsObject_mledit::functionundoDepth(KviKvsObjectFunctionCall *c)
{
    if(widget())
        c->returnValue()->setInteger(((QMultiLineEdit *)widget())->undoDepth());
    return true;
}

//-->Text & Color & Family
// Findme
bool KviKvsObject_mledit::functionsetText(KviKvsObjectFunctionCall *c)
{
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QMultiLineEdit *)widget())->setText(szText);
	return true;
}
bool KviKvsObject_mledit::functionsetColor(KviKvsObjectFunctionCall *c)
{
	KviKvsVariant * pColArray;
	kvs_int_t iColR,iColG,iColB;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("red_or_array",KVS_PT_VARIANT,0,pColArray)
		KVSO_PARAMETER("green",KVS_PT_INT,KVS_PF_OPTIONAL,iColG)
		KVSO_PARAMETER("blue",KVS_PT_INT,KVS_PF_OPTIONAL,iColB)
	KVSO_PARAMETERS_END(c)
	if(pColArray->isArray())
	{
		if(pColArray->array()->size() < 3)
		{
			c->error(__tr2qs("The array passed as parameter must contain at least 3 elements"));
			return false;
		}
		KviKvsVariant * pColR = pColArray->array()->at(0);
		KviKvsVariant * pColG = pColArray->array()->at(1);
		KviKvsVariant * pColB = pColArray->array()->at(2);

		if(!(pColR && pColG && pColB))
		{
			c->error(__tr2qs("One of the colors array parameters is empty"));
			return false;
		}
		if(!(pColR->asInteger(iColR) && pColG->asInteger(iColG) && pColB->asInteger(iColB)))
		{
			c->error(__tr2qs("One of the colors array parameters didn't evaluate to an integer"));
			return false;
		}

	} else {
		if (c->params()->count()==1)
			{
				bool bOk,bOk1,bOk2;
				QString value;
				pColArray->asString(value);
				int i=0;
				if (value.length()!=6)
				{
						c->warning(__tr2qs("A string of 6 digits hex is required"));
						return true;
				}
				QString buffer(value.mid(0,2));
				iColR=buffer.toInt(&bOk,16);
				buffer=value.mid(2,2);
				iColG=buffer.toInt(&bOk1,16);
				buffer=value.mid(4,2);
				iColB=buffer.toInt(&bOk2,16);
				if (!bOk || !bOk1 || !bOk2)
				{
					c->warning(__tr2qs("Not an hex digits"));
				return true;
				}
			if (widget()) ((QMultiLineEdit *)widget())->setColor(QColor(iColR,iColG,iColB));
			return true;
			}
		if(c->params()->count() < 3)
		{
			c->error(__tr2qs("$setColor requires either an array as first parameter or three integers"));
			return false;
		}
		if(!pColArray->asInteger(iColR))
		{
			c->error(__tr2qs("The first parameter didn't evaluate to an array nor an integer"));
			return false;
		}
	}
	if (widget()) ((QMultiLineEdit *)widget())->setColor(QColor(iColR,iColG,iColB));
	return true;
}

bool KviKvsObject_mledit::functionsetPointSize(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iPointSize;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("point_size",KVS_PT_UNSIGNEDINTEGER,0,iPointSize)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QMultiLineEdit *)widget())->setPointSize(iPointSize);
	return true;
}

bool KviKvsObject_mledit::functionsetLinkUnderline(KviKvsObjectFunctionCall *c)
{
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bLinkunderline",KVS_PT_BOOL,0,bFlag)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QMultiLineEdit *)widget())->setLinkUnderline(bFlag);
	return true;
}

bool KviKvsObject_mledit::functionsetFamily(KviKvsObjectFunctionCall *c)
{
	QString szFamily;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("family",KVS_PT_STRING,0,szFamily)
	KVSO_PARAMETERS_END(c)
	if (widget())
			((QMultiLineEdit *)widget())->setFamily(szFamily);
	return true;
}

bool KviKvsObject_mledit::functionsetModified(KviKvsObjectFunctionCall *c)
{
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bModified",KVS_PT_BOOL,0,bFlag)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QMultiLineEdit *)widget())->setModified(bFlag);
	return true;
}

bool KviKvsObject_mledit::functioninsertParagraph(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iParagraph;
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
		KVSO_PARAMETER("paragraph",KVS_PT_INT,0,iParagraph)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QMultiLineEdit *)widget())->insertParagraph(szText,iParagraph);
	return true;
}

bool KviKvsObject_mledit::functionremoveParagraph(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iParagraph;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("paragraph",KVS_PT_INT,0,iParagraph)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QMultiLineEdit*)widget())->removeParagraph(iParagraph);
	return true;
}

bool KviKvsObject_mledit::functionsetOverwriteMode(KviKvsObjectFunctionCall *c)
{
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bOverwritemode",KVS_PT_BOOL,0,bFlag)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QMultiLineEdit *)widget())->setOverwriteMode(bFlag);
	return true;
}

bool KviKvsObject_mledit::functiontextFormat(KviKvsObjectFunctionCall *c)

{
	if(!widget())return true;
	int fstyle = ((QMultiLineEdit *)widget())->textFormat();
	QString format="";
	for(unsigned int i = 0; i < mod_num; i++)
	{
		if(fstyle & mod_cod[i])
		{
			format=mod_tbl[i];
		}
	}
	c->returnValue()->setString(format);
	return true;

}
bool KviKvsObject_mledit::functionsetTextFormat(KviKvsObjectFunctionCall *c)

{
	QString szFormat;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("textformat",KVS_PT_STRING,0,szFormat)
	KVSO_PARAMETERS_END(c)
	if(!widget()) return true;
	if(KviQString::equalCI(szFormat,"PlainText"))
			((QMultiLineEdit *)widget())->setTextFormat(Qt::PlainText);
	else if(KviQString::equalCI(szFormat,"RichText"))
			((QMultiLineEdit *)widget())->setTextFormat(Qt::RichText);
	else if(KviQString::equalCI(szFormat,"LogText"))
			((QMultiLineEdit *)widget())->setTextFormat(Qt::LogText);
	else if(KviQString::equalCI(szFormat,"AutoText"))
			((QMultiLineEdit *)widget())->setTextFormat(Qt::AutoText);
	else c->warning(__tr2qs("Unknown text format '%Q'"),&szFormat);
	return true;
}
// findme
bool KviKvsObject_mledit::functionsetParagraphBackgroundColor(KviKvsObjectFunctionCall *c)
{
    KviKvsVariant * pColArray;
	kvs_int_t iParagraph,iColR,iColG,iColB;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("paragraph",KVS_PT_INT,0,iParagraph)
		KVSO_PARAMETER("red_or_array",KVS_PT_VARIANT,0,pColArray)
		KVSO_PARAMETER("green",KVS_PT_INT,KVS_PF_OPTIONAL,iColG)
		KVSO_PARAMETER("blue",KVS_PT_INT,KVS_PF_OPTIONAL,iColB)
	KVSO_PARAMETERS_END(c)
	if(pColArray->isArray())
	{
		if(pColArray->array()->size() < 3)
		{
			c->error(__tr2qs("The array passed as parameter must contain at least 3 elements"));
			return false;
		}
		KviKvsVariant * pColR = pColArray->array()->at(0);
		KviKvsVariant * pColG = pColArray->array()->at(1);
		KviKvsVariant * pColB = pColArray->array()->at(2);

		if(!(pColR && pColG && pColB))
		{
			c->error(__tr2qs("One of the colors array parameters is empty"));
			return false;
		}
		if(!(pColR->asInteger(iColR) && pColG->asInteger(iColG) && pColB->asInteger(iColB)))
		{
			c->error(__tr2qs("One of the colors array parameters didn't evaluate to an integer"));
			return false;
		}

	} else {
		if (c->params()->count()==1)
			{
				bool bOk,bOk1,bOk2;
				QString value;
				pColArray->asString(value);
				int i=0;
				if (value.length()!=6)
				{
						c->warning(__tr2qs("A string of 6 digits hex is required"));
						return true;
				}
				QString buffer(value.mid(0,2));
				iColR=buffer.toInt(&bOk,16);
				buffer=value.mid(2,2);
				iColG=buffer.toInt(&bOk1,16);
				buffer=value.mid(4,2);
				iColB=buffer.toInt(&bOk2,16);
				if (!bOk || !bOk1 || !bOk2)
				{
					c->warning(__tr2qs("Not an hex digits"));
				return true;
				}
			if (widget()) ((QMultiLineEdit *)widget())->setParagraphBackgroundColor(iParagraph,QColor(iColR,iColG,iColB));
			return true;
			}
		if(c->params()->count() < 3)
		{
			c->error(__tr2qs("$setColor requires either an array as first parameter or three integers"));
			return false;
		}
		if(!pColArray->asInteger(iColR))
		{
			c->error(__tr2qs("The first parameter didn't evaluate to an array nor an integer"));
			return false;
		}
	}
   	if (widget()) ((QMultiLineEdit *)widget())->setParagraphBackgroundColor(iParagraph,QColor(iColR,iColG,iColB));
	return true;
}

bool KviKvsObject_mledit::functionclearParagraphBackground(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iParagraph;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("paragraph",KVS_PT_INT,0,iParagraph)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QMultiLineEdit*)widget())->clearParagraphBackground(iParagraph);
	return true;
}

bool KviKvsObject_mledit::functionloadFile(KviKvsObjectFunctionCall *c)
{
	QString szFile;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("file_name",KVS_PT_STRING,0,szFile)
	KVSO_PARAMETERS_END(c)
	if ( !QFile::exists(szFile))
	{
		c->warning(__tr2qs(" I can't find the specified file '%Q'."),&szFile);
        return true;
	}

	QFile file( szFile );
    if ( !file.open( IO_ReadOnly ) )
	{
		c->warning(__tr2qs(" I cannot read the file %Q'."),&szFile);
        return true;
	}

	QTextStream ts( &file );
    QString txt = ts.read();
	if ( !QStyleSheet::mightBeRichText( txt ) )
	txt = QStyleSheet::convertFromPlainText( txt, QStyleSheetItem::WhiteSpacePre );
	((QMultiLineEdit *)widget())->setText( txt );
	file.close();
	return true;
}

bool KviKvsObject_mledit::functionsetAlignment(KviKvsObjectFunctionCall *c)

{
	QString szAlignment;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("alignment",KVS_PT_STRING,0,szAlignment)
	KVSO_PARAMETERS_END(c)
	if(!widget()) return true;
	if(KviQString::equalCI(szAlignment,"Left"))
		((QMultiLineEdit *)widget())->setAlignment(Qt::AlignLeft);
	else if(KviQString::equalCI(szAlignment,"Right"))
		((QMultiLineEdit *)widget())->setAlignment(Qt::AlignRight);
	else if(KviQString::equalCI(szAlignment,"Center"))
		((QMultiLineEdit *)widget())->setAlignment(Qt::AlignCenter);
	else if(KviQString::equalCI(szAlignment,"Justify"))
		((QMultiLineEdit *)widget())->setAlignment(Qt::AlignJustify);
	else c->warning(__tr2qs("Unknown alignment '%Q'"),&szAlignment);
	return true;
}

bool KviKvsObject_mledit::functionsetAutoFormatting(KviKvsObjectFunctionCall *c)

{
	QString szAutoformatting;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("autoformatting",KVS_PT_STRING,0,szAutoformatting)
	KVSO_PARAMETERS_END(c)
	if(!widget()) return true;
	if(KviQString::equalCI(szAutoformatting,"AutoNone"))
		((QMultiLineEdit *)widget())->setAlignment(QTextEdit::AutoNone);
	else if(KviQString::equalCI(szAutoformatting,"BulletList"))
		((QMultiLineEdit *)widget())->setAlignment(QTextEdit::AutoBulletList);
	else if(KviQString::equalCI(szAutoformatting,"AutoAll"))
		((QMultiLineEdit *)widget())->setAlignment(QTextEdit::AutoAll);
	else c->warning(__tr2qs("Unknown auto formatting mode '%Q'"),&szAutoformatting);
	return true;
}

bool KviKvsObject_mledit::functionsetVerticalAlignment(KviKvsObjectFunctionCall *c)

{
	QString szValignment;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("alignment",KVS_PT_STRING,0,szValignment)
	KVSO_PARAMETERS_END(c)
	if(!widget()) return true;
	if(KviQString::equalCI(szValignment,"Normal"))
		((QMultiLineEdit *)widget())->setVerticalAlignment(QTextEdit::AlignNormal);
	else if(KviQString::equalCI(szValignment,"SuperScript"))
		((QMultiLineEdit *)widget())->setVerticalAlignment(QTextEdit::AlignSuperScript);
	else if(KviQString::equalCI(szValignment,"SubScript"))
		((QMultiLineEdit *)widget())->setVerticalAlignment(QTextEdit::AlignSubScript);
	else c->warning(__tr2qs("Unknown vertical alignment '%Q'"),&szValignment);
	return true;
}

bool KviKvsObject_mledit::functionparagraphs(KviKvsObjectFunctionCall *c)
{
	if(widget())
		c->returnValue()->setInteger(((QMultiLineEdit *)widget())->paragraphs());
	return true;

}

bool KviKvsObject_mledit::functionlines(KviKvsObjectFunctionCall *c)
{
	if(widget())
		c->returnValue()->setInteger(((QMultiLineEdit *)widget())->lines());
	return true;
}

bool KviKvsObject_mledit::functionlineOfChar(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iPara,iIndex;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("paragraph",KVS_PT_INT,0,iPara)
		KVSO_PARAMETER("index",KVS_PT_INT,0,iIndex)
	KVSO_PARAMETERS_END(c)
	if (widget())
		c->returnValue()->setInteger(((QMultiLineEdit *)widget())->lineOfChar(iPara,iIndex));
	return true;
}

bool KviKvsObject_mledit::functionlinesOfParagraph(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iLine;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("paragraph",KVS_PT_INT,0,iLine)
	KVSO_PARAMETERS_END(c)
	if(widget())
		c->returnValue()->setInteger(((QMultiLineEdit *)widget())->linesOfParagraph(iLine));
	return true;
}


bool KviKvsObject_mledit::functionparagraphLength(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iParagraph;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("paragraph",KVS_PT_INT,0,iParagraph)
	KVSO_PARAMETERS_END(c)
	if(widget())
		c->returnValue()->setInteger(((QMultiLineEdit *)widget())->paragraphLength(iParagraph));
	return true;

}

bool KviKvsObject_mledit::functionselectAll(KviKvsObjectFunctionCall *c)
{
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bSelectall",KVS_PT_BOOL,0,bFlag)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QMultiLineEdit *)widget())->selectAll(bFlag);
	return true;
}



#include "m_class_multilineedit.moc"
