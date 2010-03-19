//=============================================================================
//
//   File : class_lcd.cpp
//   Creation date : Mon Gen 27 14:30:48 CEST 2005 by Tonino Imbesi(Grifisx)
//         and Alessandro Carbone(Noldor)
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2005 Alessandro Carbone (elfonol at gmail dot com)
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

#include "class_lcd.h"
#include "kvi_error.h"
#include "kvi_debug.h"
#include "kvi_locale.h"
#include "kvi_iconmanager.h"

#include <QLCDNumber>

/*
	@doc: lcdnumber
	@keyterms:
		lcdnumber object class, lcd
	@title:
		lcdnumber class
	@type:
		class
	@short:
		The lcdNumber widget displays a number with LCD-like digits.
	@inherits:

	@description:
		It can display a number in just about any size. It can display decimal,
		hexadecimal, octal or binary numbers.
	@functions:
		!fn: $displayStr(<number:string>)
		Displays the number represented by the string s.
		!fn: $displayInt(<number:integer>)
		Sets the displayed value rounded to the nearest integer to number.
		!fn: $displayDouble(<number:double>)
		Sets the displayed value to number (double).
		!fn: $setMode(<mode:string>)
		Sets the current display mode (number base): valid mode are HEX,BIN,OCT,DEC.
		!fn: $setSegmentStyles(<style:string>)
		Sets the style of the lcdnumber, valid styles are: Outline,Filled,Flat.
		!fn: $setNumDigits(<number:integer>)
		Sets the lcd number value.
		!fn: <boolean> $checkOverflow()
		Returns TRUE if num is too big to be displayed in its entirety;
		otherwise returns FALSE.
		!fn: $setSmallDecimalPoint(<bflag:boolean>)
		Sets the the decimal point style, valid bool values are 1 or 0.
		!fn: <boolean> $checkOverflow(<number:integer>)
		Returns 1 (TRUE) if num is too big to be displayed in its entirety; otherwise returns 0 (FALSE).
*/

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_lcd,"lcdnumber","widget")

	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_lcd,displayStr)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_lcd,displayInt)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_lcd,displayDouble)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_lcd,setMode)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_lcd,setSegmentStyle)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_lcd,setNumDigits)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_lcd,setSmallDecimalPoint)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_lcd,checkOverflow)
KVSO_END_REGISTERCLASS(KviKvsObject_lcd)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_lcd,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_lcd)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_lcd)

KVSO_END_CONSTRUCTOR(KviKvsObject_lcd)

bool KviKvsObject_lcd::init(KviKvsRunTimeContext *,KviKvsVariantList *)
{
	SET_OBJECT(QLCDNumber)
	return true;
}

KVSO_CLASS_FUNCTION(lcd,displayInt)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iDigit;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("number",KVS_PT_INT,0,iDigit)
	KVSO_PARAMETERS_END(c)
	((QLCDNumber *)widget())->display((int)iDigit);
	return true;
}

KVSO_CLASS_FUNCTION(lcd,displayStr)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("number",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	((QLCDNumber *)widget())->display(szText);
	return true;
}

KVSO_CLASS_FUNCTION(lcd,displayDouble)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_real_t dDigit;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("number",KVS_PT_DOUBLE,0,dDigit)
	KVSO_PARAMETERS_END(c)
	((QLCDNumber *)widget())->display(dDigit);
	return true;
}

KVSO_CLASS_FUNCTION(lcd,setMode)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szMode;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("mode",KVS_PT_STRING,0,szMode)
	KVSO_PARAMETERS_END(c)
	if(KviQString::equalCI(szMode, "HEX")) ((QLCDNumber *)widget())->setHexMode();
	else if(KviQString::equalCI(szMode, "DEC")) ((QLCDNumber *)widget())->setDecMode();
	else if(KviQString::equalCI(szMode, "BIN")) ((QLCDNumber *)widget())->setBinMode();
	else if(KviQString::equalCI(szMode, "OCT")) ((QLCDNumber *)widget())->setOctMode();
	else c->warning( __tr2qs_ctx("Unknown mode '%Q'","objects"),&szMode);
	return true;
}

KVSO_CLASS_FUNCTION(lcd,setSegmentStyle)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szStyle;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("style",KVS_PT_STRING,0,szStyle)
	KVSO_PARAMETERS_END(c)
	if(KviQString::equalCI(szStyle, "Outline")) ((QLCDNumber *)widget())->setSegmentStyle(QLCDNumber::Outline);
	else if(KviQString::equalCI(szStyle, "Filled")) ((QLCDNumber *)widget())->setSegmentStyle(QLCDNumber::Filled);
	else if(KviQString::equalCI(szStyle, "Flat")) ((QLCDNumber *)widget())->setSegmentStyle(QLCDNumber::Flat);
	else c->warning( __tr2qs_ctx("Unknown segment style '%Q'","objects"),&szStyle);
	return true;
}

KVSO_CLASS_FUNCTION(lcd,setNumDigits)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_real_t digit;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("digit",KVS_PT_DOUBLE,0,digit)
	KVSO_PARAMETERS_END(c)
	((QLCDNumber *)widget())->setNumDigits((int)digit);
	return true;
}

KVSO_CLASS_FUNCTION(lcd,setSmallDecimalPoint)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bflag",KVS_PT_BOOLEAN,0,bFlag)
	KVSO_PARAMETERS_END(c)
	((QLCDNumber *)widget())->setSmallDecimalPoint(bFlag);
	return true;
}

KVSO_CLASS_FUNCTION(lcd,checkOverflow)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_real_t iDigit;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("digit",KVS_PT_DOUBLE,0,iDigit)
	KVSO_PARAMETERS_END(c)
	c->returnValue()->setBoolean(((QLCDNumber *)widget())->checkOverflow(iDigit));
	return true;
}
