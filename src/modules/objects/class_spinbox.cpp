//
//   File : class_progressbar.cpp
//   Creation date : Fri Jan 30 14:21:48 CEST 2005 
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
// 
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2005 Szymon Stefanek (pragma at kvirc dot net)
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

#include <qspinbox.h>
#include "class_spinbox.h"
#include "kvi_error.h"
#include "kvi_debug.h"

#include "kvi_locale.h"
#include "kvi_iconmanager.h"

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
		This function is called by the framework when the spinbox value is changed and return the new slider value as its argument.[br]
		The default implementation emits the [classfnc]$valueChanged[/classfnc]() signal,
		so it is easy to handle the values from many spinboxes without reimplementing
		the [classfnc]$valueChangedEvent[/classfnc]() for every one.[br]
		Note: If you reimplement this function to catch the spinbox value, you will have to emit the signal by yourself (if you still need it, obviously).
		@signals:
		!sg: $valueChanged()
		This signal is emitted by the default implementation of [classfnc]valueChangedEvent[/classfnc]().[br]
		If you reimplement that function you will have to emit the signal manually (if you still need it).
*/


KVSO_BEGIN_REGISTERCLASS(KviKvsObject_spinbox,"spinbox","widget")



	KVSO_REGISTER_HANDLER(KviKvsObject_spinbox,"setValue", functionsetValue);
	KVSO_REGISTER_HANDLER(KviKvsObject_spinbox,"setMinValue", functionsetMinValue);
	KVSO_REGISTER_HANDLER(KviKvsObject_spinbox,"setMaxValue", functionsetMaxValue);
	KVSO_REGISTER_HANDLER(KviKvsObject_spinbox,"setLineStep", functionsetLineStep);
	KVSO_REGISTER_HANDLER(KviKvsObject_spinbox,"setSpecialValueText", functionsetSpecialValueText);
	KVSO_REGISTER_HANDLER(KviKvsObject_spinbox,"value",functionvalue);
	KVSO_REGISTER_HANDLER(KviKvsObject_spinbox,"minValue", functionminValue);
	KVSO_REGISTER_HANDLER(KviKvsObject_spinbox,"maxValue", functionmaxValue);
	KVSO_REGISTER_HANDLER(KviKvsObject_spinbox,"lineStep", functionlineStep);
	KVSO_REGISTER_HANDLER(KviKvsObject_spinbox,"specialValueText", functionspecialValueText);

	KVSO_REGISTER_HANDLER(KviKvsObject_spinbox,"setPrefix", functionsetPrefix);
	KVSO_REGISTER_HANDLER(KviKvsObject_spinbox,"setSuffix", functionsetSuffix);

	
	KVSO_REGISTER_HANDLER(KviKvsObject_spinbox,"valueChangedEvent", functionvalueChangedEvent);
KVSO_END_REGISTERCLASS(KviKvsObject_spinbox)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_spinbox,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_spinbox)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_spinbox)

KVSO_END_CONSTRUCTOR(KviKvsObject_spinbox)

bool KviKvsObject_spinbox::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{
	setObject(new QSpinBox(parentScriptWidget(), name()), true);
	connect(widget(),SIGNAL(valueChanged(int)),this,SLOT(valueChanged(int)));
	return true;
}



bool KviKvsObject_spinbox::functionsetValue(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iValue;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("value",KVS_PT_INT,0,iValue)
	KVSO_PARAMETERS_END(c)
    if (widget()) ((QSpinBox *)widget())->setValue(iValue);
	return true;
}
bool KviKvsObject_spinbox::functionsetMinValue(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iMinvalue;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("min_value",KVS_PT_INT,0,iMinvalue)
	KVSO_PARAMETERS_END(c)
    if (widget()) ((QSpinBox *)widget())->setMinValue(iMinvalue);
	return true;
}
bool KviKvsObject_spinbox::functionsetMaxValue(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iMaxvalue;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("max_value",KVS_PT_INT,0,iMaxvalue)
	KVSO_PARAMETERS_END(c)
    if (widget()) ((QSpinBox *)widget())->setMaxValue(iMaxvalue);
	return true;
}

bool KviKvsObject_spinbox::functionsetLineStep(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iLinestep;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("line_step",KVS_PT_INT,0,iLinestep)
	KVSO_PARAMETERS_END(c)
    if (widget()) ((QSpinBox *)widget())->setLineStep(iLinestep);
	return true;
}


bool KviKvsObject_spinbox::functionvalue(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setInteger(((QSpinBox *)widget())->value());
	return true;
}

bool KviKvsObject_spinbox::functionminValue(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setInteger(((QSpinBox *)widget())->minValue());
	return true;
}
bool KviKvsObject_spinbox::functionmaxValue(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setInteger(((QSpinBox *)widget())->maxValue());
	return true;
}
bool KviKvsObject_spinbox::functionlineStep(KviKvsObjectFunctionCall *c)
{
	if (widget())
	{
		#ifdef COMPILE_USE_QT4
			c->returnValue()->setInteger(((QSpinBox *)widget())->singleStep());
		#else
			c->returnValue()->setInteger(((QSpinBox *)widget())->lineStep());
		#endif
	}
	
	return true;
}


bool KviKvsObject_spinbox::functionspecialValueText(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setString(((QSpinBox *)widget())->specialValueText());
	return true;
}


bool KviKvsObject_spinbox::functionsetSpecialValueText(KviKvsObjectFunctionCall *c)
{
	
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QSpinBox *)widget())->setSpecialValueText(szText);
	return true;
}
bool KviKvsObject_spinbox::functionsetPrefix(KviKvsObjectFunctionCall *c)
{
	
	QString szPrefix;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szPrefix)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QSpinBox *)widget())->setPrefix(szPrefix);
	return true;
}
bool KviKvsObject_spinbox::functionsetSuffix(KviKvsObjectFunctionCall *c)
{
	
	QString szSuffix;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szSuffix)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QSpinBox *)widget())->setSuffix(szSuffix);
	return true;
}

bool KviKvsObject_spinbox::functionvalueChangedEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("valueChanged",c,c->params());
	return true;
}

void KviKvsObject_spinbox::valueChanged(int value)
{
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)value));
	callFunction(this,"valueChangedEvent",&params);
}

#include "m_class_spinbox.moc"
