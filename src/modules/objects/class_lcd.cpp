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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
		!fn: $setDisplayStr(<number:string>)
		Displays the number represented by the string s.
		!fn: $setDisplayInt(<number:integer>)
		Sets the displayed value rounded to the nearest integer to number.
		!fn: $setDisplayDouble(<number:double>)
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
	
	KVSO_REGISTER_HANDLER(KviKvsObject_lcd,"setDisplayStr", functiondisplayStr)
	KVSO_REGISTER_HANDLER(KviKvsObject_lcd,"setDisplayInt", functiondisplayInt)
	KVSO_REGISTER_HANDLER(KviKvsObject_lcd,"setDisplayDouble", functiondisplayDouble)
	KVSO_REGISTER_HANDLER(KviKvsObject_lcd,"setMode", functionsetMode)
	KVSO_REGISTER_HANDLER(KviKvsObject_lcd,"setSegmentStyle", functionsetSegmentStyle)
	KVSO_REGISTER_HANDLER(KviKvsObject_lcd,"setNumDigits", functionsetNumDigits)
	KVSO_REGISTER_HANDLER(KviKvsObject_lcd,"setSmallDecimalPoint",functionsetSmallDecimalPoint)
	KVSO_REGISTER_HANDLER(KviKvsObject_lcd,"checkOverflow", functioncheckOverflow)
KVSO_END_REGISTERCLASS(KviKvsObject_lcd)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_lcd,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_lcd)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_lcd)

KVSO_END_CONSTRUCTOR(KviKvsObject_lcd)

bool KviKvsObject_lcd::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{
	SET_OBJECT(QLCDNumber)
	return true;
}

bool KviKvsObject_lcd::functiondisplayInt(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iDigit;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("number",KVS_PT_INT,0,iDigit)
	KVSO_PARAMETERS_END(c)
    if(widget())((QLCDNumber *)widget())->display((int)iDigit);
    return true;
}

bool KviKvsObject_lcd::functiondisplayStr(KviKvsObjectFunctionCall *c)
{
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("number",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
    if(widget()) ((QLCDNumber *)widget())->display(szText);
    return true;
}

bool KviKvsObject_lcd::functiondisplayDouble(KviKvsObjectFunctionCall *c)
{
	kvs_real_t dDigit;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("number",KVS_PT_DOUBLE,0,dDigit)
	KVSO_PARAMETERS_END(c)
    if(widget()) ((QLCDNumber *)widget())->display(dDigit);
    return true;
}
bool KviKvsObject_lcd::functionsetMode(KviKvsObjectFunctionCall *c)
{
	QString szMode;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("mode",KVS_PT_STRING,0,szMode)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	if(KviQString::equalCI(szMode, "HEX")) ((QLCDNumber *)widget())->setHexMode();
	else if(KviQString::equalCI(szMode, "DEC")) ((QLCDNumber *)widget())->setDecMode();
	else if(KviQString::equalCI(szMode, "BIN")) ((QLCDNumber *)widget())->setBinMode();
	else if(KviQString::equalCI(szMode, "OCT")) ((QLCDNumber *)widget())->setOctMode();
	else c->warning( __tr2qs("Unknown mode "));
	return true;
}
bool KviKvsObject_lcd::functionsetSegmentStyle(KviKvsObjectFunctionCall *c)
{
	QString szStyle;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("style",KVS_PT_STRING,0,szStyle)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	if(KviQString::equalCI(szStyle, "Outline")) ((QLCDNumber *)widget())->setSegmentStyle(QLCDNumber::Outline);
	else if(KviQString::equalCI(szStyle, "Filled")) ((QLCDNumber *)widget())->setSegmentStyle(QLCDNumber::Filled);
	else if(KviQString::equalCI(szStyle, "Flat")) ((QLCDNumber *)widget())->setSegmentStyle(QLCDNumber::Flat);
	else c->warning( __tr2qs("Unknown segment style "));
	return true;
}
bool KviKvsObject_lcd::functionsetNumDigits(KviKvsObjectFunctionCall *c)
{
	kvs_real_t digit;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("digit",KVS_PT_DOUBLE,0,digit)
	KVSO_PARAMETERS_END(c)
    if(widget()) ((QLCDNumber *)widget())->setNumDigits(digit);
    return true;
}
bool KviKvsObject_lcd::functionsetSmallDecimalPoint(KviKvsObjectFunctionCall *c)
{
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bflag",KVS_PT_BOOLEAN,0,bFlag)
	KVSO_PARAMETERS_END(c)
    if(widget()) ((QLCDNumber *)widget())->setSmallDecimalPoint(bFlag);
	return true;
}
bool KviKvsObject_lcd::functioncheckOverflow(KviKvsObjectFunctionCall *c)
{
	kvs_real_t iDigit;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("digit",KVS_PT_DOUBLE,0,iDigit)
	KVSO_PARAMETERS_END(c)
    if(widget())c->returnValue()->setBoolean(((QLCDNumber *)widget())->checkOverflow(iDigit));
    return true;
}


