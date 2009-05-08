//vim: ts=8
//   File : class_lineedit.cpp
//   Creation date : Wed 20 Sep 2000 04:34:40 CEST by Krzysztof Godlewski
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//


#include "kvi_error.h"
#include "kvi_debug.h"

#include "kvi_locale.h"

#include "class_lineedit.h"

static const char * mode_tbl[] = {
			"Normal",
			"NoEcho",
			"Password"
			  };

static const int mode_cod[] =  {
		QLineEdit::Normal,
		QLineEdit::NoEcho,
		QLineEdit::Password
			};

#define mode_num	(sizeof(mode_tbl) / sizeof(mode_tbl[0]))


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
		-Normal: display chars as they entered[br]
		-Noecho : do not display anything[br]
		-Password : display asterisks instead of the characters actually entered[br]
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
		Sets the lineedit to read only mode.
		!fn: $setInputMask(<mask:string>)
		Sets the validation input mask to inputMask.[br]
		Example:[br]
		[br]
		%esempio->$setInputMask( "+99 99 99 99 99;_" );[br]
		%esempio->$setInputMask( "000.000.000.000;_" );[br]
		%esempio->Ip Number Mask.[br]
		%esempio->setInputMask( ">AAAAA-AAAAA-AAAAA-AAAAA-AAAAA;#" );[br]
		[br]
		The mask format understands these mask characters:[br]
		Character		Meaning[br]
		A			ASCII alphabetic character required. A-Z, a-z.[br]
		a			ASCII alphabetic character permitted but not required.[br]
		N			ASCII alphanumeric character required. A-Z, a-z, 0-9.[br]
		n			ASCII alphanumeric character permitted but not required.[br]
		X			Any character required.[br]
		x			Any character permitted but not required.[br]
		9			ASCII digit required. 0-9.[br]
		0			ASCII digit permitted but not required.[br]
		D			ASCII digit required. 1-9.[br]
		d			ASCII digit permitted but not required.[br]
		#			ASCII digit or plus/minus sign permitted but not required.[br]
		>			All following alphabetic characters are uppercased.[br]
		<			All following alphabetic characters are lowercased.[br]
		!			Switch off case conversion.[br]
		\			Use \ to escape the special characters listed above to use them as separators.[br]
		[br]
		The mask consists of a string of mask characters and separators, optionally[br]
		followed by a semi-colon and the character used for blanks: the blank characters [br]
		are always removed from the text after editing. The default blank character is space. [br]
		!fn: $returnPressedEvent()
		This function is called by the framework when the enter key is pressed.
		!fn: $lostFocusEvent()
		Called when the lineedit lost focus.
		!fn: $textChangedEvent(<new text:string>)
		This event is called when the text changed, In $0 there is the new text.
	@signals:
		!sg: $returnPressed()
		This signal is emitted by the default implementation of [classfnc]$returnPressedEvent[/classfnc]().
		!sg: $lostFocus()
		This signal is emitted by the default implementation of [classfnc]$lostFocusEvent[/classfnc]().
		!sg: $textChanged()
		This signal is emitted by the default implementation of [classfnc]$textChangedEvent[/classfnc]().

*/



KVSO_BEGIN_REGISTERCLASS(KviKvsObject_lineedit,"lineedit","widget")

	KVSO_REGISTER_HANDLER(KviKvsObject_lineedit,"text", functionText)
	KVSO_REGISTER_HANDLER(KviKvsObject_lineedit,"setText", functionSetText)
	KVSO_REGISTER_HANDLER(KviKvsObject_lineedit,"maxLength", functionMaxLength)
	KVSO_REGISTER_HANDLER(KviKvsObject_lineedit,"setMaxLength", functionSetMaxLength)
	KVSO_REGISTER_HANDLER(KviKvsObject_lineedit,"frame", functionFrame)
	KVSO_REGISTER_HANDLER(KviKvsObject_lineedit,"setCursorPosition", functionSetCursorPosition)
	KVSO_REGISTER_HANDLER(KviKvsObject_lineedit,"cursorPosition", functionCursorPosition)
	KVSO_REGISTER_HANDLER(KviKvsObject_lineedit,"setFrame", functionSetFrame)
	KVSO_REGISTER_HANDLER(KviKvsObject_lineedit,"selectAll" , functionSelectAll)
	KVSO_REGISTER_HANDLER(KviKvsObject_lineedit,"setSelection", functionSetSelection)
	KVSO_REGISTER_HANDLER(KviKvsObject_lineedit,"copy", functionCopy)
	KVSO_REGISTER_HANDLER(KviKvsObject_lineedit,"cut", functionCut)
	KVSO_REGISTER_HANDLER(KviKvsObject_lineedit,"paste", functionPaste)

	KVSO_REGISTER_HANDLER(KviKvsObject_lineedit,"echoMode", functionEchoMode)
	KVSO_REGISTER_HANDLER(KviKvsObject_lineedit,"setEchoMode", functionSetEchoMode)
	KVSO_REGISTER_HANDLER(KviKvsObject_lineedit,"clear", functionClear)
	KVSO_REGISTER_HANDLER(KviKvsObject_lineedit,"setDragEnabled", functionDragAndDrop)
	KVSO_REGISTER_HANDLER(KviKvsObject_lineedit,"setInputMask", functionSetInputMask)
	KVSO_REGISTER_HANDLER(KviKvsObject_lineedit,"setReadOnly", functionSetReadOnly)
	KVSO_REGISTER_HANDLER(KviKvsObject_lineedit,"returnPressedEvent", functionreturnPressedEvent)
	KVSO_REGISTER_HANDLER(KviKvsObject_lineedit,"lostFocusEvent", functionlostFocusEvent)
	KVSO_REGISTER_HANDLER(KviKvsObject_lineedit,"textChangedEvent", functiontextChangedEvent)


KVSO_END_REGISTERCLASS(KviKvsObject_lineedit)


KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_lineedit,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_lineedit)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_lineedit)

KVSO_END_CONSTRUCTOR(KviKvsObject_lineedit)


bool KviKvsObject_lineedit::init(KviKvsRunTimeContext * pContext,KviKvsVariantList * pParams)
{
	setObject(new QLineEdit(parentScriptWidget(),getName()),true);
	connect(widget(),SIGNAL(returnPressed()),this,SLOT(slotreturnPressed()));
	connect(widget(),SIGNAL(lostFocus()),this,SLOT(slotlostFocus()));
	connect(widget(),SIGNAL(textChanged(const QString & )),this,SLOT(slottextChanged(const QString & )));
	return true;
}

bool KviKvsObject_lineedit::functionText(KviKvsObjectFunctionCall *c)
{
	if(widget())
		c->returnValue()->setString(((QLineEdit *)widget())->text());
	return true;
}

bool KviKvsObject_lineedit::functionSetText(KviKvsObjectFunctionCall *c)
{
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QLineEdit *)widget())->setText(szText);
	return true;
}

bool KviKvsObject_lineedit::functionMaxLength(KviKvsObjectFunctionCall *c)
{
	if(widget())
		c->returnValue()->setInteger(((QLineEdit *)widget())->maxLength());
	return true;
}

bool KviKvsObject_lineedit::functionSetMaxLength(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t iMaxlen;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("maxlen",KVS_PT_UNSIGNEDINTEGER,0,iMaxlen)
	KVSO_PARAMETERS_END(c)
	if (widget())
			((QLineEdit *)widget())->setMaxLength(iMaxlen);
	return true;
}

bool KviKvsObject_lineedit::functionFrame(KviKvsObjectFunctionCall *c)
{
	if(widget())
		c->returnValue()->setBoolean(((QLineEdit *)widget())->frame());
	return true;
}

bool KviKvsObject_lineedit::functionSetFrame(KviKvsObjectFunctionCall *c)
{
	bool bFrame;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bframe",KVS_PT_BOOL,0,bFrame)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QLineEdit *)widget())->setFrame(bFrame);
	return true;
}

bool KviKvsObject_lineedit::functionCursorPosition(KviKvsObjectFunctionCall *c)
{
	if(widget())
		c->returnValue()->setInteger(((QLineEdit *)widget())->cursorPosition());
	return true;
}

bool KviKvsObject_lineedit::functionSetCursorPosition(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t iPos;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("position",KVS_PT_UNSIGNEDINTEGER,0,iPos)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QLineEdit *)widget())->setCursorPosition(iPos);
	return true;
}

bool KviKvsObject_lineedit::functionSelectAll(KviKvsObjectFunctionCall *c)
{
	if(widget())
		((QLineEdit *)widget())->selectAll();
	return true;
}

bool KviKvsObject_lineedit::functionSetSelection(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uStart,uLen;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("start",KVS_PT_UNSIGNEDINTEGER,0,uStart)
		KVSO_PARAMETER("len",KVS_PT_UNSIGNEDINTEGER,0,uLen)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QLineEdit *)widget())->setSelection(uStart, uLen);
	return true;
}

bool KviKvsObject_lineedit::functionCopy(KviKvsObjectFunctionCall *c)
{
	if(widget())
		((QLineEdit *)widget())->copy();
	return true;
}

bool KviKvsObject_lineedit::functionCut(KviKvsObjectFunctionCall *c)
{
	if(widget())
		((QLineEdit *)widget())->cut();
	return true;
}

bool KviKvsObject_lineedit::functionPaste(KviKvsObjectFunctionCall *c)
{
	if(widget())
		((QLineEdit *)widget())->paste();
	return true;
}

bool KviKvsObject_lineedit::functionEchoMode(KviKvsObjectFunctionCall *c)
{
	int mode = ((QLineEdit *)widget())->echoMode();
	QString szEchomode="";
	for(unsigned int i = 0; i < mode_num; i++)
	{
		if(mode == mode_cod[i])
		{
			szEchomode=mode_tbl[i];
			break;
		}
	}
	c->returnValue()->setString(szEchomode);
	return true;
}

bool KviKvsObject_lineedit::functionSetEchoMode(KviKvsObjectFunctionCall *c)
{
	QString szMode;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("mode",KVS_PT_STRING,0,szMode)
	KVSO_PARAMETERS_END(c)
	if (!widget()) return true;
	for(unsigned int i = 0; i < mode_num; i++)
	{
		if(KviQString::equalCI(szMode, mode_tbl[i]))
		{
			((QLineEdit *)widget())->setEchoMode( \
				((QLineEdit::EchoMode)mode_cod[i]));
			return true;
		}
	}
	c->warning(__tr2qs("Unknown echo mode %Q"),&szMode);
	return true;
}

bool KviKvsObject_lineedit::functionClear(KviKvsObjectFunctionCall *c)
{
	if(widget())
		((QLineEdit *)widget())->clear();
	return true;
}

//-| Grifisx & Noldor |-Start:
bool KviKvsObject_lineedit::functionDragAndDrop(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QLineEdit *)widget())->setDragEnabled(bEnabled);
	return true;
}

bool KviKvsObject_lineedit::functionSetReadOnly(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bReadonly",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QLineEdit *)widget())->setReadOnly(bEnabled);
	return true;
}

bool KviKvsObject_lineedit::functionSetInputMask(KviKvsObjectFunctionCall *c)
{
	QString szMask;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("mask",KVS_PT_STRING,0,szMask)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QLineEdit *)widget())->setInputMask(szMask);
	return true;
}

bool KviKvsObject_lineedit::functionreturnPressedEvent(KviKvsObjectFunctionCall *c)
{

	emitSignal("returnPressed",c);
	return true;

}

void KviKvsObject_lineedit::slotreturnPressed()
{
	KviKvsVariantList * params = 0;
	callFunction(this,"returnPressedEvent",params);

}
// FIND ME
bool KviKvsObject_lineedit::functionlostFocusEvent(KviKvsObjectFunctionCall *c)
{


	emitSignal("lostFocus",c);
	return true;

}

void KviKvsObject_lineedit::slotlostFocus()
{
	KviKvsVariantList * params = 0;
	callFunction(this,"lostFocusEvent",params);
}
/////
bool KviKvsObject_lineedit::functiontextChangedEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("textChanged",c,c->params());
	return true;

}

void KviKvsObject_lineedit::slottextChanged(const QString &text)
{
	KviKvsVariantList params(new KviKvsVariant(text));
	callFunction(this,"textChangedEvent",&params);
}


#include "m_class_lineedit.moc"
