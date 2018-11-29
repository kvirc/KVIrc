#ifndef _CLASS_LCD_H_
#define _CLASS_LCD_H_
//=============================================================================
//
//   File : KvsObject_lcd.h
//   Creation date : Mon Gen 27 14:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005 Alessandro Carbone (elfonol at gmail dot com)
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

class KvsObject_lcd : public KvsObject_widget
{
public:
	KVSO_DECLARE_OBJECT(KvsObject_lcd)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	bool init(KviKvsRunTimeContext * pContext, KviKvsVariantList * pParams) override;

	bool displayStr(KviKvsObjectFunctionCall * c);
	bool setMode(KviKvsObjectFunctionCall * c);
	bool setSegmentStyle(KviKvsObjectFunctionCall * c);
	bool setNumDigits(KviKvsObjectFunctionCall * c);
	bool setSmallDecimalPoint(KviKvsObjectFunctionCall * c);
	bool checkOverflow(KviKvsObjectFunctionCall * c);
	bool displayInt(KviKvsObjectFunctionCall * c);
	bool displayDouble(KviKvsObjectFunctionCall * c);
};

#endif // !_CLASS_LCD_H_
