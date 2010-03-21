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

#include "kvi_error.h"
#include "kvi_debug.h"
#include "kvi_locale.h"
#include "kvi_malloc.h"

#include <QTextBlock>
#include <QFile>
#include <QTextDocument>
#include <QTextEdit>
#include <QTextStream>
#include <QTextCursor>

#include "class_multilineedit.h"

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
		!fn: $insertLine(<text:string>, <line:integer>)
		Inserts <text> at line number <line>. If line is less than
		zero, or larger than the number of rows, the new text is
		put at the end. If txt contains newline characters, several
		lines are inserted. The <line> parameter is optional. If it is
		ommited, the line is put at end.
		!fn: $removeLine(<line:integer>)
		Removes line number <line>.
		!fn: $insertAt(<txt:string>, <line:uinteger>, <col:uinteger>)
		Inserts txt at line number line, after character number col in
		the line. If txt contains newline characters, new lines are
		inserted.
		!fn: <integer> $numLines()
		Returns number of lines in the widget.
		!fn: $setCursorPosition(<line:integer>, <col:uinteger>, <mark:boolean>)
		Sets the cursor position to character number col in line
		number line. The parameters are adjusted to lie within the
		legal range.[br]
		If mark is 1, the selection is cleared. otherwise it
		is extended
		!fn: <line,col:integer> $cursorPosition()
		Returns current cursor position in "<line:uinteger>, <col:uinteger>" format.
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
		!fn: $undo()
		Undoes the last operation.
		!fn: $redo()
		Redoes the last operation.
		!fn: $clear()
		Deletes all the text in the multiline edit.
		!fn: $setUndoRedoEnabled(<bUndoRedo:boolean>)
		Sets whether undo/redo is enabled to the bool value.
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
		Sets the text format. Correct values are RichText, PlainText.
		!fn: <string> $textFormat()
		Returns the text format: rich text or plain text.
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
		- Qt::AlignLeft - Aligns with the left edge.[br]
		- Qt::AlignRight - Aligns with the right edge.[br]
		- Qt::AlignCenter - Centers in both dimensions.
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


KVSO_BEGIN_REGISTERCLASS(KviKvsObject_textedit,"multilineedit","widget")
KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"addwidget", functionAddWidget)


	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"textLine", functionTextLine)
/*	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"insertLine", functionInsertLine)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"removeLine", functionRemoveLine)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"insertAt", functionInsertAt)

	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"setCursorPosition", functionSetCursorPosition)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"setWordWrap", functionSetWordWrap)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"wordWrap", functionWordWrap)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"length", functionLength);
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"indent", functionundoDepth)


	*/
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"atEnd", functionAtEnd)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"numLines", functionNumLines)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"atBeginning", functionAtBeginning)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"cursorPosition", functionCursorPosition)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"text", functionText)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"insert", functionInsert)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"maxLines", functionMaxLines)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"setMaxLines", functionSetMaxLines)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"append", functionAppend)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"copy", functionCopy)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"cut", functionCut)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"paste", functionPaste)

	//->Set Style
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"setFamily" , functionsetFamily)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"setItalic", functionsetItalic)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"setBold", functionsetBold)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"setUnderline", functionsetUnderline)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"italic", functionitalic)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"bold", functionbold)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"underline", functionunderline)

	//->Zoom In, Out, To
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"zoomIn", functionzoomIn)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"zoomOut", functionzoomOut)

	//->Undo & Redo
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"undo", functionundo)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"redo", functionredo)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"clear", functionclear)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"setUndoRedoEnabled", functionsetUndoRedoEnabled)


	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"isUndoRedoEnabled", functionisUndoRedoEnabled)

	//->Text color & others
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"setText", functionsetText)
	/*

	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"setColor", functionsetColor)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"setModified" , functionsetModified)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"setLinkUnderline" , functionsetLinkUnderline)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"setOverwriteMode" , functionsetOverwriteMode)
	*/
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"setPointSize", functionsetPointSize)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"setReadOnly",functionSetReadOnly)
	
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"setTextFormat" , functionsetTextFormat)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"textFormat" , functiontextFormat)
	/*
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"setParagraphBackgroundColor", functionsetParagraphBackgroundColor)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"clearParagraphBackgroundColor", functionsetParagraphBackgroundColor)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"loadFile" , functionloadFile); // let's fantasy
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"setAlignment" , functionsetAlignment)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"setAutoFormatting" , functionsetAutoFormatting)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"setVerticalAlignment" , functionsetVerticalAlignment)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"setWrapPolicy" , functionsetWrapPolicy)
	//->Paragraph & line
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"paragraphs" , functionparagraphs)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"paragraphLength" , functionparagraphLength)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"insertParagraph" , functioninsertParagraph)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"removeParagraph" , functionremoveParagraph)


	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"linesOfParagraph" , functionlinesOfParagraph)

	*/
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"lineOfChar" , functionlineOfChar)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"lines" ,functionlines)
	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"selectAll" ,functionselectAll)
//	KVSO_REGISTER_HANDLER(KviKvsObject_textedit,"insertTable",functionInsertTable)
KVSO_END_REGISTERCLASS(KviKvsObject_textedit)


KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_textedit,KviKvsObject_widget)


KVSO_END_CONSTRUCTOR(KviKvsObject_textedit)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_textedit)

KVSO_END_CONSTRUCTOR(KviKvsObject_textedit)

bool KviKvsObject_textedit::init(KviKvsRunTimeContext *,KviKvsVariantList *)
{
	SET_OBJECT(QTextEdit);
	return true;
}

bool KviKvsObject_textedit::functionAddWidget(KviKvsObjectFunctionCall * c)
{
	KviKvsObject * pObject;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("widget",KVS_PT_HOBJECT,0,hObject)
	KVSO_PARAMETERS_END(c)
	pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!widget())return true;
	CHECK_HOBJECT_IS_WIDGET(pObject)
	((QTextEdit *)object())->addScrollBarWidget(((QWidget *)(pObject->object())),Qt::AlignLeft);
	return true;
}
bool KviKvsObject_textedit::functionInsertTable(KviKvsObjectFunctionCall *)
{
	if(widget()){
		QTextCursor cur= ((QTextEdit *)widget())->textCursor();
		QTextTableFormat tableModel;
		tableModel.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
		tableModel.setCellPadding(0);
		tableModel.setCellSpacing(0);
		tableModel.setBorderBrush(QColor(255,255,255));

		cur.insertTable(3,3,tableModel);
		((QTextEdit *)widget())->setTextCursor(cur);
	}
	return true;
}
bool KviKvsObject_textedit::functionSetReadOnly(KviKvsObjectFunctionCall * c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QTextEdit *)widget())->setReadOnly(bEnabled);
	return true;
}

bool KviKvsObject_textedit::functionTextLine(KviKvsObjectFunctionCall * c)
{
	kvs_int_t iBlock;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("line",KVS_PT_INT,0,iBlock)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	if(iBlock > ((QTextEdit *)widget())->document()->blockCount() || iBlock < 0) c->warning(__tr2qs_ctx("No such line '%d'","objects"),&iBlock);
	else
		c->returnValue()->setString(((QTextEdit *)widget())->document()->findBlockByNumber(iBlock).text());

	return true;
}

bool KviKvsObject_textedit::functionInsertLine(KviKvsObjectFunctionCall * c)
{
	kvs_int_t iLine;
	QString szInsert;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szInsert)
		KVSO_PARAMETER("line",KVS_PT_INT,KVS_PF_OPTIONAL,iLine)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	//((QTextEdit *)widget())->insertLine(szInsert,iLine);
	return true;
}

bool KviKvsObject_textedit::functionRemoveLine(KviKvsObjectFunctionCall * c)
{
	kvs_int_t iLine;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("line",KVS_PT_INT,0,iLine)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	if(iLine > ((QTextEdit *)widget())->document()->blockCount() || iLine < 0)
			c->warning("No such line number");
	//else ((QTextEdit *)widget())->removeLine(iLine);
	return true;
}

bool KviKvsObject_textedit::functionInsertAt(KviKvsObjectFunctionCall * c)
{
	kvs_uint_t iLine,iCol;
	QString szInsert;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szInsert)
		KVSO_PARAMETER("line",KVS_PT_UNSIGNEDINTEGER,0,iLine)
		KVSO_PARAMETER("col",KVS_PT_UNSIGNEDINTEGER,0,iCol)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	if(iLine > (uint) ((QTextEdit *)widget())->document()->blockCount())
		c->warning("No such line number");
	//else
	//	((QTextEdit *)widget())->insertAt(szInsert,iLine,iCol);
	return true;
}

bool KviKvsObject_textedit::functionNumLines(KviKvsObjectFunctionCall * c)
{
	if(widget()) c->returnValue()->setInteger(((QTextEdit *)widget())->document()->blockCount());
	return true;
}

bool KviKvsObject_textedit::functionSetCursorPosition(KviKvsObjectFunctionCall * c)
{
	kvs_uint_t iLine,iCol;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("line",KVS_PT_UNSIGNEDINTEGER,0,iLine)
		KVSO_PARAMETER("col",KVS_PT_UNSIGNEDINTEGER,0,iCol)
	KVSO_PARAMETERS_END(c)
	//if(widget())
	//	((QTextEdit *)widget())->setCursorPosition(iLine, iCol, bFlag);
	return true;
}

bool KviKvsObject_textedit::functionCursorPosition(KviKvsObjectFunctionCall * c)
{
	if(!widget()) return true;
	int iRow=((QTextEdit *)widget())->textCursor().blockNumber();
	int iCol=((QTextEdit *)widget())->textCursor().columnNumber();

	KviKvsArray * a = new KviKvsArray();
	a->set(0,new KviKvsVariant((kvs_int_t)iRow));
	a->set(1,new KviKvsVariant((kvs_int_t)iCol));
	c->returnValue()->setArray(a);
	return true;
}

bool KviKvsObject_textedit::functionAtBeginning(KviKvsObjectFunctionCall * c)
{
	if(!widget()) return true;
	c->returnValue()->setBoolean(((QTextEdit *)widget())->textCursor().atStart());
	return true;
}

bool KviKvsObject_textedit::functionAtEnd(KviKvsObjectFunctionCall * c)
{
	if(!widget()) return true;
	c->returnValue()->setBoolean(((QTextEdit *)widget())->textCursor().atEnd());
	return true;
}

bool KviKvsObject_textedit::functionSetWordWrap(KviKvsObjectFunctionCall * c)
{
	if(!widget())return true;
	QString szWrap;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("word_wrap",KVS_PT_STRING,0,szWrap)
	KVSO_PARAMETERS_END(c)

	if(KviQString::equalCI(szWrap,"NoWrap"))
		((QTextEdit *)widget())->setLineWrapColumnOrWidth(QTextEdit::NoWrap);
	else if(KviQString::equalCI(szWrap,"WidgetWidth"))
		((QTextEdit *)widget())->setLineWrapColumnOrWidth(QTextEdit::WidgetWidth);
	else if(KviQString::equalCI(szWrap,"FixedPixelWidth"))
		((QTextEdit *)widget())->setLineWrapColumnOrWidth(QTextEdit::FixedPixelWidth);
	else if(KviQString::equalCI(szWrap,"FixedColumnWidth"))
		((QTextEdit *)widget())->setLineWrapColumnOrWidth(QTextEdit::FixedColumnWidth);
        else c->warning(__tr2qs_ctx("Unknown word wrap '%Q'","objects"),&szWrap);
	return true;
}

bool KviKvsObject_textedit::functionsetWrapPolicy(KviKvsObjectFunctionCall * c)
{
	if(!widget())return true;
	QString szPolicy;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("wrap_policy",KVS_PT_STRING,0,szPolicy)
	KVSO_PARAMETERS_END(c)

/*	if(KviQString::equalCI(szPolicy,"AtWhiteSpace)"))
		((QTextEdit *)widget())->setWordWrapMode(QTextEdit::AtWhiteSpace);
	else if(KviQString::equalCI(szPolicy,"Anywhere"))
		((QTextEdit *)widget())->setWordWrapMode(QTextEdit::Anywhere);
	else if(KviQString::equalCI(szPolicy,"AtWordBoundary"))
		((QTextEdit *)widget())->setWordWrapMode(QTextOption::WordWrap);
	else if(KviQString::equalCI(szPolicy,"AtWordBoundaryOrAnywhere"))
		((QTextEdit *)widget())->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        else c->warning(__tr2qs_ctx("Unknown wrap policy'%Q'","objects"),&szPolicy);
*/
	return true;
}

bool KviKvsObject_textedit::functionWordWrap(KviKvsObjectFunctionCall *)
{
	/*
	if(widget())
		c->returnValue()->setBoolean(((QTextEdit *)widget())->wordWrap() & QTextEdit::WidgetWidth);
	*/
	return true;
}

bool KviKvsObject_textedit::functionText(KviKvsObjectFunctionCall * c)
{
	if(widget())
		c->returnValue()->setString(((QTextEdit *)widget())->document()->toPlainText());
	return true;
}

bool KviKvsObject_textedit::functionSetMaxLines(KviKvsObjectFunctionCall * c)
{
	kvs_uint_t imaxLines;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("max_lines",KVS_PT_UNSIGNEDINTEGER,0,imaxLines)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QTextEdit *)widget())->document()->setMaximumBlockCount(imaxLines);
	return true;
}

bool KviKvsObject_textedit::functionMaxLines(KviKvsObjectFunctionCall * c)
{
	if(widget())
		c->returnValue()->setInteger(((QTextEdit *)widget())->document()->maximumBlockCount());
	return true;
}

bool KviKvsObject_textedit::functionInsert(KviKvsObjectFunctionCall * c)
{
	QString szInsert;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szInsert)
	KVSO_PARAMETERS_END(c)
	if (widget()){
		QTextCursor cursor=((QTextEdit *)widget())->textCursor();
		cursor.insertText(szInsert);
		((QTextEdit *)widget())->setTextCursor(cursor);
	}
	return true;
}

bool KviKvsObject_textedit::functionAppend(KviKvsObjectFunctionCall * c)
{
	QString szAppend;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szAppend)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QTextEdit *)widget())->append(szAppend);
	return true;
}

bool KviKvsObject_textedit::functionCopy(KviKvsObjectFunctionCall *)
{
	if(widget())
		((QTextEdit *)widget())->copy();
	return true;
}

bool KviKvsObject_textedit::functionCut(KviKvsObjectFunctionCall *)
{
	if(widget())
		((QTextEdit *)widget())->cut();
	return true;
}

bool KviKvsObject_textedit::functionPaste(KviKvsObjectFunctionCall *)
{
	if(widget())
		((QTextEdit *)widget())->paste();
	return true;
}

//->Set Bold, Italic, Underline
bool KviKvsObject_textedit::functionsetBold(KviKvsObjectFunctionCall * c)
{
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bBold",KVS_PT_BOOL,0,bFlag)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QTextEdit *)widget())->setFontWeight(bFlag?QFont::Bold:QFont::Normal);
	return true;
}

bool KviKvsObject_textedit::functionsetUnderline(KviKvsObjectFunctionCall * c)
{
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bUnderline",KVS_PT_BOOL,0,bFlag)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QTextEdit *)widget())->setFontUnderline(bFlag);
	return true;
}

bool KviKvsObject_textedit::functionsetItalic(KviKvsObjectFunctionCall * c)
{
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bItalic",KVS_PT_BOOL,0,bFlag)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QTextEdit *)widget())->setFontItalic(bFlag);
	return true;
}

//-->Returns Italic , Bold, Underline.
bool KviKvsObject_textedit::functionbold(KviKvsObjectFunctionCall * c)
{
	if(widget())
		c->returnValue()->setBoolean(((QTextEdit *)widget())->fontWeight() >= QFont::Bold );
	return true;
}

bool KviKvsObject_textedit::functionitalic(KviKvsObjectFunctionCall * c)
{
	if(widget())
		c->returnValue()->setBoolean(((QTextEdit *)widget())->fontItalic());
	return true;
}

bool KviKvsObject_textedit::functionunderline(KviKvsObjectFunctionCall * c)
{
	if(widget())
		c->returnValue()->setBoolean(((QTextEdit *)widget())->fontUnderline());
	return true;
}

//->Zoom In, out at
bool KviKvsObject_textedit::functionzoomIn(KviKvsObjectFunctionCall * c)
{
	kvs_int_t iZoom;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("zoom_value",KVS_PT_INT,KVS_PF_OPTIONAL,iZoom)
	KVSO_PARAMETERS_END(c)
	if (!widget()) return true;
	if (!iZoom)
		((QTextEdit *)object())->zoomIn();
	else
		((QTextEdit *)object())->zoomIn(iZoom);
	return true;
}

bool KviKvsObject_textedit::functionzoomOut(KviKvsObjectFunctionCall * c)
{
	kvs_int_t iZoom;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("zoom_value",KVS_PT_INT,KVS_PF_OPTIONAL,iZoom)
	KVSO_PARAMETERS_END(c)
	if (!widget()) return true;
	if (!iZoom)
		((QTextEdit *)object())->zoomOut();
	else
		((QTextEdit *)object())->zoomOut(iZoom);
	return true;
}



//-> Undo & Redo functions
bool KviKvsObject_textedit::functionundo(KviKvsObjectFunctionCall *)
{
	if(widget())
		((QTextEdit *)widget())->undo();
	return true;
}

bool KviKvsObject_textedit::functionredo(KviKvsObjectFunctionCall *)
{
	if(widget())
		((QTextEdit *)widget())->redo();
	return true;
}

bool KviKvsObject_textedit::functionclear(KviKvsObjectFunctionCall *)
{
	if(widget())
		((QTextEdit *)widget())->clear();
	return true;
}

/*
bool KviKvsObject_textedit::functiondel(KviKvsObjectFunctionCall * c)
{
	if(widget())
		((QTextEdit *)widget())->del();
	return true;
}

bool KviKvsObject_textedit::functionindent(KviKvsObjectFunctionCall * c)
{
	if(widget())
		((QTextEdit *)widget())->indent();
	return true;
}
*/

bool KviKvsObject_textedit::functionsetUndoRedoEnabled(KviKvsObjectFunctionCall * c)
{
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bUndoredo",KVS_PT_BOOL,0,bFlag)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QTextEdit *)widget())->setUndoRedoEnabled(bFlag);
	return true;
}

bool KviKvsObject_textedit::functionisUndoRedoEnabled(KviKvsObjectFunctionCall * c)
{
	if(widget())
		c->returnValue()->setBoolean(((QTextEdit *)widget())->isUndoRedoEnabled());
	return true;
}


//-->Text & Color & Family
// Findme
bool KviKvsObject_textedit::functionsetText(KviKvsObjectFunctionCall * c)
{
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QTextEdit *)widget())->setText(szText);
	return true;
}

bool KviKvsObject_textedit::functionsetPlainText(KviKvsObjectFunctionCall * c)
{
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QTextEdit *)widget())->setPlainText(szText);
	return true;
}

bool KviKvsObject_textedit::functionsetHtmlText(KviKvsObjectFunctionCall * c)
{
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QTextEdit *)widget())->setHtml(szText);
	return true;
}

bool KviKvsObject_textedit::functionsetColor(KviKvsObjectFunctionCall * c)
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
                        c->error(__tr2qs_ctx("The array passed as parameter must contain at least 3 elements","objects"));
			return false;
		}
		KviKvsVariant * pColR = pColArray->array()->at(0);
		KviKvsVariant * pColG = pColArray->array()->at(1);
		KviKvsVariant * pColB = pColArray->array()->at(2);

		if(!(pColR && pColG && pColB))
		{
                        c->error(__tr2qs_ctx("One of the colors array parameters is empty","objects"));
			return false;
		}
		if(!(pColR->asInteger(iColR) && pColG->asInteger(iColG) && pColB->asInteger(iColB)))
		{
                        c->error(__tr2qs_ctx("One of the colors array parameters didn't evaluate to an integer","objects"));
			return false;
		}

	} else {
		if (c->params()->count()==1)
			{
				bool bOk,bOk1,bOk2;
				QString value;
				pColArray->asString(value);
				if (value.length()!=6)
				{
                                                c->warning(__tr2qs_ctx("A string of 6 hex digits is required","objects"));
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
                                        c->warning(__tr2qs_ctx("Not an hex digit","objects"));
				return true;
				}
	//		if (widget()) ((QTextEdit *)widget())->setColor(QColor(iColR,iColG,iColB));
			return true;
			}
		if(c->params()->count() < 3)
		{
                        c->error(__tr2qs_ctx("$setColor requires either an array as first parameter or three integers","objects"));
			return false;
		}
		if(!pColArray->asInteger(iColR))
		{
                        c->error(__tr2qs_ctx("The first parameter didn't evaluate to an array nor an integer","objects"));
			return false;
		}
	}
	if (widget()) ((QTextEdit *)widget())->setTextColor(QColor(iColR,iColG,iColB));
	return true;
}

bool KviKvsObject_textedit::functionsetPointSize(KviKvsObjectFunctionCall * c)
{
	kvs_real_t dPointSize;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("point_size",KVS_PT_REAL,0,dPointSize)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QTextEdit *)widget())->setFontPointSize(dPointSize);
	return true;
}

/*
bool KviKvsObject_textedit::functionsetLinkUnderline(KviKvsObjectFunctionCall * c)
{
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bLinkunderline",KVS_PT_BOOL,0,bFlag)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QTextEdit *)widget())->setLinkUnderline(bFlag);
	return true;
}
*/

bool KviKvsObject_textedit::functionsetFamily(KviKvsObjectFunctionCall * c)
{
	QString szFamily;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("family",KVS_PT_STRING,0,szFamily)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QTextEdit *)widget())->setFontFamily(szFamily);
	return true;
}

bool KviKvsObject_textedit::functionsetModified(KviKvsObjectFunctionCall * c)
{
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bModified",KVS_PT_BOOL,0,bFlag)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QTextEdit *)widget())->document()->setModified(bFlag);
	return true;
}

bool KviKvsObject_textedit::functioisModified(KviKvsObjectFunctionCall * c)
{
	if(widget())
		c->returnValue()->setBoolean(((QTextEdit *)widget())->document()->isModified());
	return true;
}

bool KviKvsObject_textedit::functioninsertParagraph(KviKvsObjectFunctionCall * c)
{
	kvs_int_t iParagraph;
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
		KVSO_PARAMETER("paragraph",KVS_PT_INT,0,iParagraph)
	KVSO_PARAMETERS_END(c)
	//if(widget())
	//	((QTextEdit *)widget())->insertParagraph(szText,iParagraph);
	return true;
}

bool KviKvsObject_textedit::functionremoveParagraph(KviKvsObjectFunctionCall * c)
{
	kvs_int_t iParagraph;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("paragraph",KVS_PT_INT,0,iParagraph)
	KVSO_PARAMETERS_END(c)
	//if(widget())
	//	((QTextEdit*)widget())->removeParagraph(iParagraph);
	return true;
}

bool KviKvsObject_textedit::functionsetOverwriteMode(KviKvsObjectFunctionCall * c)
{
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bOverwritemode",KVS_PT_BOOL,0,bFlag)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QTextEdit *)widget())->setOverwriteMode(bFlag);
	return true;
}

bool KviKvsObject_textedit::functiontextFormat(KviKvsObjectFunctionCall *c)
{
	if(!widget())return true;
	if(((QTextEdit *)widget())->acceptRichText())
		c->returnValue()->setString(QString("RichText"));
	else
		c->returnValue()->setString(QString("PlainText"));
	return true;
}

bool KviKvsObject_textedit::functionsetTextFormat(KviKvsObjectFunctionCall * c)
{
	QString szFormat;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("textformat",KVS_PT_STRING,0,szFormat)
	KVSO_PARAMETERS_END(c)
	if(!widget()) return true;
	if(KviQString::equalCI(szFormat,"PlainText"))
		((QTextEdit *)widget())->setAcceptRichText(false);
	else if(KviQString::equalCI(szFormat,"RichText"))
		((QTextEdit *)widget())->setAcceptRichText(true);
        else c->warning(__tr2qs_ctx("Unknown text format '%Q'","objects"),&szFormat);
	return true;
}

// findme
bool KviKvsObject_textedit::functionsetParagraphBackgroundColor(KviKvsObjectFunctionCall * c)
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
                        c->error(__tr2qs_ctx("The array passed as parameter must contain at least 3 elements","objects"));
			return false;
		}
		KviKvsVariant * pColR = pColArray->array()->at(0);
		KviKvsVariant * pColG = pColArray->array()->at(1);
		KviKvsVariant * pColB = pColArray->array()->at(2);

		if(!(pColR && pColG && pColB))
		{
                        c->error(__tr2qs_ctx("One of the colors array parameters is empty","objects"));
			return false;
		}
		if(!(pColR->asInteger(iColR) && pColG->asInteger(iColG) && pColB->asInteger(iColB)))
		{
                        c->error(__tr2qs_ctx("One of the colors array parameters didn't evaluate to an integer","objects"));
			return false;
		}

	} else {
		if (c->params()->count()==1)
		{
			bool bOk,bOk1,bOk2;
			QString value;
			pColArray->asString(value);
			if (value.length()!=6)
			{
                                        c->warning(__tr2qs_ctx("A string of 6 hex digits is required","objects"));
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
                                c->warning(__tr2qs_ctx("Not an hex digit","objects"));
				return true;
			}
			//if (widget()) ((QTextEdit *)widget())->setParagraphBackgroundColor(iParagraph,QColor(iColR,iColG,iColB));
			return true;
		}
		if(c->params()->count() < 3)
		{
                        c->error(__tr2qs_ctx("$setColor requires either an array as first parameter or three integers","objects"));
			return false;
		}
		if(!pColArray->asInteger(iColR))
		{
                        c->error(__tr2qs_ctx("The first parameter didn't evaluate to an array nor an integer","objects"));
			return false;
		}
	}
	//if (widget()) ((QTextEdit *)widget())->setParagraphBackgroundColor(iParagraph,QColor(iColR,iColG,iColB));
	return true;
}

bool KviKvsObject_textedit::functionclearParagraphBackground(KviKvsObjectFunctionCall * c)
{
	kvs_int_t iParagraph;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("paragraph",KVS_PT_INT,0,iParagraph)
	KVSO_PARAMETERS_END(c)
	//if(widget())
	//	((QTextEdit*)widget())->clearParagraphBackground(iParagraph);
	return true;
}

bool KviKvsObject_textedit::functionloadFile(KviKvsObjectFunctionCall * c)
{
	QString szFile;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("file_name",KVS_PT_STRING,0,szFile)
	KVSO_PARAMETERS_END(c)
	if ( !QFile::exists(szFile))
	{
                c->warning(__tr2qs_ctx("I can't find the specified file '%Q'.","objects"),&szFile);
        return true;
	}

	QFile file( szFile );
	if ( !file.open( QIODevice::ReadOnly ) )
	{
                c->warning(__tr2qs_ctx("I cannot read the file '%Q'.","objects"),&szFile);
        return true;
	}

	QTextStream ts( &file );
	QString txt = ts.readAll();
	//settext tries to understand if the text is html or plain
	((QTextEdit *)widget())->setText( txt );

	file.close();
	return true;
}

bool KviKvsObject_textedit::functionsetAlignment(KviKvsObjectFunctionCall * c)
{
	QString szAlignment;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("alignment",KVS_PT_STRING,0,szAlignment)
	KVSO_PARAMETERS_END(c)
	if(!widget()) return true;
	if(KviQString::equalCI(szAlignment,"Left"))
		((QTextEdit *)widget())->setAlignment(Qt::AlignLeft);
	else if(KviQString::equalCI(szAlignment,"Right"))
		((QTextEdit *)widget())->setAlignment(Qt::AlignRight);
	else if(KviQString::equalCI(szAlignment,"Center"))
		((QTextEdit *)widget())->setAlignment(Qt::AlignCenter);
	else if(KviQString::equalCI(szAlignment,"Justify"))
		((QTextEdit *)widget())->setAlignment(Qt::AlignJustify);
        else c->warning(__tr2qs_ctx("Unknown alignment '%Q'","objects"),&szAlignment);
	return true;
}

bool KviKvsObject_textedit::functionsetAutoFormatting(KviKvsObjectFunctionCall * c)
{
	QString szAutoformatting;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("autoformatting",KVS_PT_STRING,0,szAutoformatting)
	KVSO_PARAMETERS_END(c)
	if(!widget()) return true;
	/*
	if(KviQString::equalCI(szAutoformatting,"AutoNone"))
		((QTextEdit *)widget())->setAutoFormatting(QTEXTEDIT_AUTO_NONE);
	else if(KviQString::equalCI(szAutoformatting,"BulletList"))
		((QTextEdit *)widget())->setAutoFormatting(QTEXTEDIT_AUTO_BULLET_LIST);
	else if(KviQString::equalCI(szAutoformatting,"AutoAll"))
		((QTextEdit *)widget())->setAutoFormatting(QTEXTEDIT_AUTO_ALL);
        else c->warning(__tr2qs_ctx("Unknown auto formatting mode '%Q'","objects"),&szAutoformatting);
	*/
	return true;
}

bool KviKvsObject_textedit::functionsetVerticalAlignment(KviKvsObjectFunctionCall * c)
{
	QString szValignment;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("alignment",KVS_PT_STRING,0,szValignment)
	KVSO_PARAMETERS_END(c)
	if(!widget()) return true;
	/*
	if(KviQString::equalCI(szValignment,"Normal"))
		((QTextEdit *)widget())->setVerticalAlignment(QTextEdit::AlignNormal);
	else if(KviQString::equalCI(szValignment,"SuperScript"))
		((QTextEdit *)widget())->setVerticalAlignment(QTextEdit::AlignSuperScript);
	else if(KviQString::equalCI(szValignment,"SubScript"))
		((QTextEdit *)widget())->setVerticalAlignment(QTextEdit::AlignSubScript);
        else c->warning(__tr2qs_ctx("Unknown vertical alignment '%Q'","objects"),&szValignment);
	*/
	return true;
}

bool KviKvsObject_textedit::functionparagraphs(KviKvsObjectFunctionCall *)
{
	//if(widget())
	//	c->returnValue()->setInteger(((QTextEdit *)widget())->paragraphs());
	return true;
}

bool KviKvsObject_textedit::functionlines(KviKvsObjectFunctionCall * c)
{
	if(widget())
		c->returnValue()->setInteger(((QTextEdit *)widget())->document()->blockCount());
	return true;
}

bool KviKvsObject_textedit::functionlineOfChar(KviKvsObjectFunctionCall * c)
{
	kvs_int_t iPara,iIndex;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("paragraph",KVS_PT_INT,0,iPara)
		KVSO_PARAMETER("index",KVS_PT_INT,0,iIndex)
	KVSO_PARAMETERS_END(c)
	//if (widget())
	//	c->returnValue()->setInteger(((QTextEdit *)widget())->lineOfChar(iPara,iIndex));
	return true;
}

bool KviKvsObject_textedit::functionlinesOfParagraph(KviKvsObjectFunctionCall * c)
{
	kvs_int_t iLine;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("paragraph",KVS_PT_INT,0,iLine)
	KVSO_PARAMETERS_END(c)
	//if(widget())
	//	c->returnValue()->setInteger(((QTextEdit *)widget())->linesOfParagraph(iLine));
	return true;
}

bool KviKvsObject_textedit::functionparagraphLength(KviKvsObjectFunctionCall * c)
{
	kvs_int_t iParagraph;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("paragraph",KVS_PT_INT,0,iParagraph)
	KVSO_PARAMETERS_END(c)
	//if(widget())
	//	c->returnValue()->setInteger(((QTextEdit *)widget())->paragraphLength(iParagraph));
	return true;
}

bool KviKvsObject_textedit::functionselectAll(KviKvsObjectFunctionCall *)
{
	if (widget())
		((QTextEdit *)widget())->selectAll();
	return true;
}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_multilineedit.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
