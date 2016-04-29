//=============================================================================
//
//   File : KvsObject_spinBox.cpp
//   Creation date : Fri Mar 18 14:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVIrc IRC client distribution
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

#include "KvsObject_spinBox.h"
#include "KviError.h"
#include "kvi_debug.h"
#include "KviLocale.h"
#include "KviIconManager.h"

#include <QSpinBox>
/*
	@doc:	spinbox
	@keyterms:
		spinbox object class,
	@title:
		spinbox class
	@type:
		class
	@short:
		Provides a simple spin button.
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		This widget allows the user to choose a value either by clicking the up/down buttons
		to increase/decrease the value currently displayed or by typing the value directly into the spin box.
	@functions:
		!fn: $setValue(<value:integer>)
		Sets the value of the spin box.[br]
		See also [classfnc]$value[/classfnc]()
		!fn: $setMinValue(<min_value:integer>)
		Sets the minimum value of the spin box.
		See also [classfnc]$MinValue[/classfnc]()
		!fn: $setMaxValue(<max_value:integer>)
		Sets the maximum value of the spin box.[br]
		See also [classfnc]$maxValue[/classfnc]()
		!fn: $setLineStep(<line_step:integer>)
		Sets the  line step: when the user uses the arrows to change the spin box's value the value will be
		incremented/decremented by the amount of the line step.
		See also [classfnc]$lineStep[/classfnc]()
		!fn: $setSpecialValueText(<text:string>)
		Sets the special-value text.
		If set, the spin box will display this text instead of a numeric value whenever the current value is equal to [classfnc]$minValue[/classfnc]().
		See also [classfnc]$setSpecialValueText[/classfnc]()
		!fn: $setPrefix(<text:string>)
		This property holds the spin box's prefix.[br]
		The prefix is prepended to the start of the displayed value.[br]
		Typical use is to display a unit of measurement or a currency symbol.
		See also [classfnc]$setSuffix[/classfnc]()
		!fn: $setSuffix(<text:string>);
		This property holds the suffix of the spin box.[br]
		The suffix is appended to the end of the displayed value.[br]
		See also [classfnc]$setPrefix[/classfnc]()
		!fn: <integer> $value()
		Returns the value of the spin box.
		See also [classfnc]$setValue[/classfnc]()
		!fn: <integer> $minValue
		Returns the minimum value of the spin box.
		See also [classfnc]$setMinValue[/classfnc]()
		!fn: <integer> $maxValue
		Returns the maximum value of the spin box.
		See also [classfnc]$setMaxValue[/classfnc]()
		!fn: <integer> $lineStep()
		Return the linestep.
		!fn: <string> $specialValueText()
		Returns the special-value text.
		See also [classfnc]$setSpecialValueText[/classfnc]()
		!fn: $valueChangedEvent(<new value:integer>)
		This function is called by the framework when the spin box value is changed and return the new slider value as its argument.[br]
		The default implementation emits the [classfnc]$valueChanged[/classfnc]() signal,
		so it is easy to handle the values from many spin boxes without reimplementing
		the [classfnc]$valueChangedEvent[/classfnc]() for every one.[br]
		Note: If you reimplement this function to catch the spin box value, you will have to emit the signal by yourself (if you still need it, obviously).
	@signals:
		!sg: $valueChanged()
		This signal is emitted by the default implementation of [classfnc]valueChangedEvent[/classfnc]().[br]
		If you reimplement that function you will have to emit the signal manually (if you still need it).
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_spinBox, "spinbox", "widget")

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_spinBox, setValue);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_spinBox, setMinValue);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_spinBox, setMaxValue);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_spinBox, setLineStep);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_spinBox, setSpecialValueText);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_spinBox, value);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_spinBox, minValue);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_spinBox, maxValue);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_spinBox, lineStep);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_spinBox, specialValueText);

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_spinBox, setPrefix);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_spinBox, setSuffix);

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_spinBox, valueChangedEvent);
KVSO_END_REGISTERCLASS(KvsObject_spinBox)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_spinBox, KvsObject_widget)

KVSO_END_CONSTRUCTOR(KvsObject_spinBox)

KVSO_BEGIN_DESTRUCTOR(KvsObject_spinBox)

KVSO_END_CONSTRUCTOR(KvsObject_spinBox)

bool KvsObject_spinBox::init(KviKvsRunTimeContext *, KviKvsVariantList *)
{
	SET_OBJECT(QSpinBox)
	connect(widget(), SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
	return true;
}

KVSO_CLASS_FUNCTION(spinBox, setValue)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iValue;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("value", KVS_PT_INT, 0, iValue)
	KVSO_PARAMETERS_END(c)
	((QSpinBox *)widget())->setValue(iValue);
	return true;
}
KVSO_CLASS_FUNCTION(spinBox, setMinValue)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iMinvalue;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("min_value", KVS_PT_INT, 0, iMinvalue)
	KVSO_PARAMETERS_END(c)
	((QSpinBox *)widget())->setMinimum(iMinvalue);
	return true;
}
KVSO_CLASS_FUNCTION(spinBox, setMaxValue)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iMaxvalue;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("max_value", KVS_PT_INT, 0, iMaxvalue)
	KVSO_PARAMETERS_END(c)
	((QSpinBox *)widget())->setMaximum(iMaxvalue);
	return true;
}

KVSO_CLASS_FUNCTION(spinBox, setLineStep)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iLinestep;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("line_step", KVS_PT_INT, 0, iLinestep)
	KVSO_PARAMETERS_END(c)
	((QSpinBox *)widget())->setSingleStep(iLinestep);
	return true;
}

KVSO_CLASS_FUNCTION(spinBox, value)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((QSpinBox *)widget())->value());
	return true;
}

KVSO_CLASS_FUNCTION(spinBox, minValue)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((QSpinBox *)widget())->minimum());
	return true;
}
KVSO_CLASS_FUNCTION(spinBox, maxValue)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((QSpinBox *)widget())->maximum());
	return true;
}
KVSO_CLASS_FUNCTION(spinBox, lineStep)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((QSpinBox *)widget())->singleStep());
	return true;
}

KVSO_CLASS_FUNCTION(spinBox, specialValueText)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setString(((QSpinBox *)widget())->specialValueText());
	return true;
}

KVSO_CLASS_FUNCTION(spinBox, setSpecialValueText)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("text", KVS_PT_STRING, 0, szText)
	KVSO_PARAMETERS_END(c)
	((QSpinBox *)widget())->setSpecialValueText(szText);
	return true;
}
KVSO_CLASS_FUNCTION(spinBox, setPrefix)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szPrefix;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("text", KVS_PT_STRING, 0, szPrefix)
	KVSO_PARAMETERS_END(c)
	((QSpinBox *)widget())->setPrefix(szPrefix);
	return true;
}
KVSO_CLASS_FUNCTION(spinBox, setSuffix)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szSuffix;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("text", KVS_PT_STRING, 0, szSuffix)
	KVSO_PARAMETERS_END(c)
	((QSpinBox *)widget())->setSuffix(szSuffix);
	return true;
}

KVSO_CLASS_FUNCTION(spinBox, valueChangedEvent)
{
	emitSignal("valueChanged", c, c->params());
	return true;
}

void KvsObject_spinBox::valueChanged(int value)
{
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)value));
	callFunction(this, "valueChangedEvent", &params);
}
