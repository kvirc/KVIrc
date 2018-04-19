//=============================================================================
//
//   File : KvsObject_lineEdit.cpp
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

#include "KviError.h"
#include "kvi_debug.h"
#include "KviLocale.h"
#include "KvsObject_lineEdit.h"
#include <QRegExpValidator>
#include <QLineEdit>
#include <QCompleter>
static const char * mode_tbl[] = {
	"Normal",
	"NoEcho",
	"Password"
};

static const int mode_cod[] = {
	QLineEdit::Normal,
	QLineEdit::NoEcho,
	QLineEdit::Password
};

#define mode_num (sizeof(mode_tbl) / sizeof(mode_tbl[0]))

/*
	@doc:	lineedit
	@keyterms:
		lineedit object class, line editor, input
	@title:
		lineedit class
	@type:
		class
	@short:
		Simple line editor for inputting short text
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		This is a simple, one-line editor usually used for inputting
		short text like names, phone numbers etc. It can display text
		in three echo modes (see [classfnc]$setEchoMode[/classfnc]()
		for details).
	@functions:
		!fn: <string> $text()
		Returns text contained by the widget.[br]
		See also [classfnc]$setText[/classfnc]().
		!fn: $setText(<text:string>)
		Sets editor text to <text>.[br]
		See also [classfnc]$text[/classfnc]().
		!fn: $setEchoMode(<echo_mode:string>)
		Sets the line edit's echo mode. Possible value are:[br]
		[pre]
			-Normal: display chars as they entered
			-Noecho: do not display anything
			-Password: display asterisks instead of the characters actually entered
		[/pre]
		See also [classfnc]$echoMode[/classfnc]().
		!fn: <string> $echoMode()
		Return the line edit's echo mode.
		See also [classfnc]$setEchoMode[/classfnc]().
		!fn: <integer> $maxLength()
		Returns the current maximum length of the text that can be typed
		 in the editor.[br]
		See also [classfnc]$setMaxLength[/classfnc]().
		!fn: $setMaxLength(<max_lenght:integer>)
		Sets maximum length of the text that can be typed in the
		editor.[br]
		See also [classfnc]$maxLength[/classfnc]().
		!fn: <boolean> $frame()
		Returns '1' if the editor is drawn inside a frame, which is 2
		pixels wide, '0' otherwise.[br]
		See also [classfnc]$setFrame[/classfnc]().
		!fn: $setFrame(<bframe:boolean>)
		Sets editor frame drawing to <bool>.[br]
		See also [classfnc]$frame[/classfnc]().
		!fn: <integer> $cursorPosition()
		Returns current cursor position in the editor.[br]
		See also [classfnc]$setCursorPosition[/classfnc]().
		!fn: $setCursorPosition(<index:integer>)
		Moves the cursor to place pointed by <index>.[br]
		See also [classfnc]$cursorPosition[/classfnc]().
		!fn: $selectAll()
		Selects whole text inside the editor.[br]
		See also [classfnc]$setSelection[/classfnc](),
		[classfnc]$cut[/classfnc](), [classfnc]$copy[/classfnc](),
		[classfnc]$paste[/classfnc]().
		!fn: $setSelection(<start:integer>, <length:integer>)
		Selects <length> characters in the editor starting at position
		<start>.[br]
		See also [classfnc]$selectAll[/classfnc](),
		[classfnc]$cut[/classfnc](), [classfnc]$copy[/classfnc](),
		[classfnc]$paste[/classfnc]().
		!fn: $copy()
		Copies selected text in the editor to the clipboard.[br]
		See also [classfnc]$setSelection[/classfnc](),
		[classfnc]$cut[/classfnc](), [classfnc]$paste[/classfnc]().
		!fn: $cut()
		Cuts selected text from the editor to the clipboard.[br]
		See also [classfnc]$setSelection[/classfnc](),
		[classfnc]$copy[/classfnc](), [classfnc]$paste[/classfnc]().
		!fn: $paste()
		Pastes text from the clipboard into the editor. The pasted text
		will start at cursor position.[br]
		See also [classfnc]$setSelection[/classfnc](),
		[classfnc]$copy[/classfnc](), [classfnc]$cut[/classfnc]().
		!fn: $clear()
		Deletes all text from the editor. Equal to calling [classfnc]$setText[/classfnc]("").
		!fn: $setDragEnabled(<bEnabled:bool>)
		With this property user can drag text in the lineedit.
		!fn: $setReadOnly(<bReadonly:boolean>)
		Sets the lineedit to read-only mode.
		!fn: $setInputMask(<mask:string>)
		Sets the validation input mask to inputMask.[br]
		[b]Example:[/b]
		[example]
			%ledit_example->$setInputMask( "+99 99 99 99 99;_" );[br]
			%ledit_example->$setInputMask( "000.000.000.000;_" );[br]
			%ledit_example->IP Number Mask.[br]
			%ledit_example->setInputMask( ">AAAAA-AAAAA-AAAAA-AAAAA-AAAAA;#" );
		[/example]
		The mask format understands these mask characters:
		[example]
			[b][comment]Character	Meaning[/comment][/b][br]
			A	-	ASCII alphabetic character required. A-Z, a-z.[br]
			a	-	ASCII alphabetic character permitted but not required.[br]
			N	-	ASCII alphanumeric character required. A-Z, a-z, 0-9.[br]
			n	-	ASCII alphanumeric character permitted but not required.[br]
			X	-	Any character required.[br]
			x	-	Any character permitted but not required.[br]
			9	-	ASCII digit required. 0-9.[br]
			0	-	ASCII digit permitted but not required.[br]
			D	-	ASCII digit required. 1-9.[br]
			d	-	ASCII digit permitted but not required.[br]
			#	-	ASCII digit or plus/minus sign permitted but not required.[br]
			>	-	All following alphabetic characters are uppercased.[br]
			<	-	All following alphabetic characters are lowercased.[br]
			!	-	Switch off case conversion.[br]
			\	-	Use \ to escape the special characters listed above to use them as separators.
		[/example]
		The mask consists of a string of mask characters and separators, optionally[br]
		followed by a semi-colon and the character used for blanks: the blank characters[br]
		are always removed from the text after editing. The default blank character is space.[br]
		!fn: $returnPressedEvent()
		This function is called by the framework when the enter key is pressed.
		!fn: $lostFocusEvent()
		Called when the lineedit lost focus.
		!fn: $textChangedEvent(<new text:string>)
		This event is called when the text changed, In $0 there is the new text.
		!fn: $setCompleter(<completion_mode:string>,<completion_list:array>)
		Provides completions based on an array.
		Valid completion_mode are:
		PopupCompletion: Current completions are displayed in a popup below the lineedit.
		UnfilteredPopupCompletion: All possible completions are displayed in a popup window with the most likely suggestion selected.
		InlineCompletion: Completions appear as selected text.
		!fn: $disableCompleter()
		Disable temporally the completer.
		!fn: $enableCompleter()
		Enable the completer.
		!fn: $removeCompleter()
		Remove the completer.
	@signals:
		!sg: $returnPressed()
		This signal is emitted by the default implementation of [classfnc]$returnPressedEvent[/classfnc]().
		!sg: $lostFocus()
		This signal is emitted by the default implementation of [classfnc]$lostFocusEvent[/classfnc]().
		!sg: $textChanged()
		This signal is emitted by the default implementation of [classfnc]$textChangedEvent[/classfnc]().
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_lineEdit, "lineedit", "widget")

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, text)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, setText)

// completer
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, setCompleter)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, enableCompleter)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, disableCompleter)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, unsetCompleter)

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, maxLength)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, setMaxLength)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, frame)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, setCursorPosition)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, cursorPosition)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, setFrame)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, selectAll)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, setSelection)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, copy)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, cut)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, paste)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, echoMode)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, setEchoMode)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, clear)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, dragAndDrop)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, setInputMask)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, setReadOnly)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, returnPressedEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, lostFocusEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, textChangedEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_lineEdit, setInputValidator)

KVSO_END_REGISTERCLASS(KvsObject_lineEdit)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_lineEdit, KvsObject_widget)
m_pCompleter = nullptr;
KVSO_END_CONSTRUCTOR(KvsObject_lineEdit)

KVSO_BEGIN_DESTRUCTOR(KvsObject_lineEdit)
if(m_pCompleter)
{
	delete m_pCompleter;
	m_pCompleter = nullptr;
}
KVSO_END_CONSTRUCTOR(KvsObject_lineEdit)

bool KvsObject_lineEdit::init(KviKvsRunTimeContext *, KviKvsVariantList *)
{
	SET_OBJECT(QLineEdit)

	connect(widget(), SIGNAL(returnPressed()), this, SLOT(slotreturnPressed()));
	connect(widget(), SIGNAL(editingFinished()), this, SLOT(slotlostFocus()));
	connect(widget(), SIGNAL(textChanged(const QString &)), this, SLOT(slottextChanged(const QString &)));
	return true;
}

KVSO_CLASS_FUNCTION(lineEdit, text)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setString(((QLineEdit *)widget())->text());
	return true;
}

KVSO_CLASS_FUNCTION(lineEdit, setText)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("text", KVS_PT_STRING, 0, szText)
	KVSO_PARAMETERS_END(c)
	((QLineEdit *)widget())->setText(szText);
	return true;
}
KVSO_CLASS_FUNCTION(lineEdit, setInputValidator)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szReg;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("reg_expression", KVS_PT_STRING, 0, szReg)
	KVSO_PARAMETERS_END(c)
	((QLineEdit *)widget())->setValidator(new QRegExpValidator(QRegExp(szReg), ((QLineEdit *)widget())));
	return true;
}
KVSO_CLASS_FUNCTION(lineEdit, maxLength)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((QLineEdit *)widget())->maxLength());
	return true;
}

KVSO_CLASS_FUNCTION(lineEdit, setMaxLength)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t iMaxlen;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("maxlen", KVS_PT_UNSIGNEDINTEGER, 0, iMaxlen)
	KVSO_PARAMETERS_END(c)
	((QLineEdit *)widget())->setMaxLength(iMaxlen);
	return true;
}

KVSO_CLASS_FUNCTION(lineEdit, frame)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setBoolean(((QLineEdit *)widget())->hasFrame());
	return true;
}

KVSO_CLASS_FUNCTION(lineEdit, setFrame)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bFrame;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bFrame", KVS_PT_BOOL, 0, bFrame)
	KVSO_PARAMETERS_END(c)
	((QLineEdit *)widget())->setFrame(bFrame);
	return true;
}

KVSO_CLASS_FUNCTION(lineEdit, cursorPosition)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((QLineEdit *)widget())->cursorPosition());
	return true;
}

KVSO_CLASS_FUNCTION(lineEdit, setCursorPosition)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t iPos;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("position", KVS_PT_UNSIGNEDINTEGER, 0, iPos)
	KVSO_PARAMETERS_END(c)
	((QLineEdit *)widget())->setCursorPosition(iPos);
	return true;
}

KVSO_CLASS_FUNCTION(lineEdit, selectAll)
{
	CHECK_INTERNAL_POINTER(widget())
	((QLineEdit *)widget())->selectAll();
	return true;
}

KVSO_CLASS_FUNCTION(lineEdit, setSelection)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uStart, uLen;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("start", KVS_PT_UNSIGNEDINTEGER, 0, uStart)
	KVSO_PARAMETER("len", KVS_PT_UNSIGNEDINTEGER, 0, uLen)
	KVSO_PARAMETERS_END(c)
	((QLineEdit *)widget())->setSelection(uStart, uLen);
	return true;
}

KVSO_CLASS_FUNCTION(lineEdit, copy)
{
	CHECK_INTERNAL_POINTER(widget())
	((QLineEdit *)widget())->copy();
	return true;
}

KVSO_CLASS_FUNCTION(lineEdit, cut)
{
	CHECK_INTERNAL_POINTER(widget())
	((QLineEdit *)widget())->cut();
	return true;
}

KVSO_CLASS_FUNCTION(lineEdit, paste)
{
	CHECK_INTERNAL_POINTER(widget())
	((QLineEdit *)widget())->paste();
	return true;
}

KVSO_CLASS_FUNCTION(lineEdit, echoMode)
{
	CHECK_INTERNAL_POINTER(widget())
	int mode = ((QLineEdit *)widget())->echoMode();
	QString szEchomode = "";
	for(unsigned int i = 0; i < mode_num; i++)
	{
		if(mode == mode_cod[i])
		{
			szEchomode = mode_tbl[i];
			break;
		}
	}
	c->returnValue()->setString(szEchomode);
	return true;
}

KVSO_CLASS_FUNCTION(lineEdit, setEchoMode)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szMode;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("mode", KVS_PT_STRING, 0, szMode)
	KVSO_PARAMETERS_END(c)
	for(unsigned int i = 0; i < mode_num; i++)
	{
		if(KviQString::equalCI(szMode, mode_tbl[i]))
		{
			((QLineEdit *)widget())->setEchoMode(((QLineEdit::EchoMode)mode_cod[i]));
			return true;
		}
	}
	c->warning(__tr2qs_ctx("Unknown echo mode '%Q'", "objects"), &szMode);
	return true;
}

KVSO_CLASS_FUNCTION(lineEdit, clear)
{
	CHECK_INTERNAL_POINTER(widget())
	((QLineEdit *)widget())->clear();
	return true;
}

//-| Grifisx & Noldor |-Start:
KVSO_CLASS_FUNCTION(lineEdit, dragAndDrop)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bEnabled", KVS_PT_BOOL, 0, bEnabled)
	KVSO_PARAMETERS_END(c)
	((QLineEdit *)widget())->setDragEnabled(bEnabled);
	return true;
}

KVSO_CLASS_FUNCTION(lineEdit, setReadOnly)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bReadonly", KVS_PT_BOOL, 0, bEnabled)
	KVSO_PARAMETERS_END(c)
	((QLineEdit *)widget())->setReadOnly(bEnabled);
	return true;
}

KVSO_CLASS_FUNCTION(lineEdit, setInputMask)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szMask;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("mask", KVS_PT_STRING, 0, szMask)
	KVSO_PARAMETERS_END(c)
	((QLineEdit *)widget())->setInputMask(szMask);
	return true;
}

KVSO_CLASS_FUNCTION(lineEdit, setCompleter)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsArray * a;
	QString szMode;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("mode", KVS_PT_STRING, KVS_PF_OPTIONAL, szMode)
	KVSO_PARAMETER("completion_list", KVS_PT_ARRAY, 0, a)
	KVSO_PARAMETERS_END(c)
	if(m_pCompleter)
		delete m_pCompleter;
	QStringList szCompletionList;
	if(a)
	{
		kvs_int_t uIdx = 0;
		kvs_int_t uSize = a->size();
		while(uIdx < uSize)
		{
			KviKvsVariant * v = a->at(uIdx);
			if(v)
			{
				QString tmp;
				v->asString(tmp);
				szCompletionList.append(tmp);
			}
			else
				szCompletionList.append("");
			uIdx++;
		}
	}
	m_pCompleter = new QCompleter(szCompletionList);
	QCompleter::CompletionMode mode = QCompleter::PopupCompletion;
	if(KviQString::equalCI(szMode, "InlineCompletion"))
		mode = QCompleter::InlineCompletion;
	else if(KviQString::equalCI(szMode, "UnfilteredPopupCompletion"))
		mode = QCompleter::UnfilteredPopupCompletion;
	else if(!KviQString::equalCI(szMode, "PopupCompletion"))
		c->warning(__tr2qs_ctx("Unknown '%Q' completion mode, switching to default PopupCompletion", "objects"), &szMode);
	m_pCompleter->setCompletionMode(mode);
	((QLineEdit *)widget())->setCompleter(m_pCompleter);
	return true;
}
KVSO_CLASS_FUNCTION(lineEdit, enableCompleter)
{
	CHECK_INTERNAL_POINTER(widget())
	if(m_pCompleter)
		((QLineEdit *)widget())->setCompleter(m_pCompleter);
	return true;
}
KVSO_CLASS_FUNCTION(lineEdit, disableCompleter)
{
	CHECK_INTERNAL_POINTER(widget())
	((QLineEdit *)widget())->setCompleter(nullptr);
	return true;
}
KVSO_CLASS_FUNCTION(lineEdit, unsetCompleter)
{
	CHECK_INTERNAL_POINTER(widget())
	if(m_pCompleter)
	{
		((QLineEdit *)widget())->setCompleter(nullptr);
		delete m_pCompleter;
		m_pCompleter = nullptr;
	}
	return true;
}

KVSO_CLASS_FUNCTION(lineEdit, returnPressedEvent)
{
	emitSignal("returnPressed", c);
	return true;
}

void KvsObject_lineEdit::slotreturnPressed()
{
	KviKvsVariantList * params = nullptr;
	callFunction(this, "returnPressedEvent", params);
}

KVSO_CLASS_FUNCTION(lineEdit, lostFocusEvent)
{
	emitSignal("lostFocus", c);
	return true;
}

void KvsObject_lineEdit::slotlostFocus()
{
	KviKvsVariantList * params = nullptr;
	callFunction(this, "lostFocusEvent", params);
}

KVSO_CLASS_FUNCTION(lineEdit, textChangedEvent)
{
	emitSignal("textChanged", c, c->params());
	return true;
}

void KvsObject_lineEdit::slottextChanged(const QString & text)
{
	KviKvsVariantList params(new KviKvsVariant(text));
	callFunction(this, "textChangedEvent", &params);
}
