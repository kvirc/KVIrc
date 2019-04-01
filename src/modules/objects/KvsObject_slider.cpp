//=============================================================================
//
//   File : KvsObject_slider.cpp
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

#include "KvsObject_slider.h"
#include "KviError.h"
#include "kvi_debug.h"
#include "KviLocale.h"
#include "KviIconManager.h"

#include <QSlider>

/*
	@doc: slider
	@keyterms:
		slider object class
	@title:
		slider class
	@type:
		class
	@short:
		The slider widget provides a vertical or horizontal slider.
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		The slider is the classic widget for controlling a bounded value.[br]
		It lets the user move a slider along a horizontal or vertical groove[br]
		and translates the slider's position into an integer value within the legal range.[/classfnc]()
	@functions:
		!fn: $setOrientation(<orientation:string>)
		Sets the slider's orientation.[br]
		Orientation value can be "Horizontal" or "Vertical".
		!fn: $setTracking(<bTracking:boolean>)
		Sets whether slider tracking is enabled to enable.[br]
		Value can be 1 or 0.
		!fn: $setMaxValue(<value:integer>)
		Sets maximum value for slider's range.
		!fn: $setMinValue(<minv_value:integer>)
		Sets minimum value for slider's range.
		!fn: $setLineStep(<line_step:integer>)
		Sets the  line step to <line_step>.
		!fn: $setPageStep(<page_step:integer>)
		Sets the  page step to <page_step>.
		!fn: $setTickmarks(<tick_marks:string>)
		Sets the tickmark settings for this slider.[br]
		Values are:[br]
		[pre]
			NoMarks - do not draw any tickmarks.
			Both - draw tickmarks on both sides of the groove.
			Above - draw tickmarks above the (horizontal) slider
			Below - draw tickmarks below the (horizontal) slider
			Left - draw tickmarks to the left of the (vertical) slider
			Right - draw tickmarks to the right of the (vertical) slider
		[/pre]
		!fn: $setTickInterval(<value>)
		Sets the interval between tickmarks.
		!fn: <integer> $value()
		Returns slider value.
		!fn: <integer> $minValue()
		Returns slider minValue.
		!fn: <integer> $maxValue()
		Returns slider maxValue.
		!fn: <integer> $lineStep()
		Returns slider lineStep value.
		!fn: <integer>$pageStep()
		Returns slider pageStep value.
		!fn: $valueChangedEvent(<new slider value:integer>)
		This function is called by the framework when the slider value is changed and return the new slider value as its argument.[br]
		The default implementation emits the [classfnc]$valueChanged[/classfnc]() signal,
		so it is easy to handle the values from many sliders without reimplementing
		the [classfnc]$valueChangedEvent[/classfnc]() for every one.[br]
		Note: If you reimplement this function to catch the slider value, you will have to emit the signal by yourself (if you still need it, obviously).
	@signals:
		!sg: $valueChanged()
		This signal is emitted by the default implementation of [classfnc]valueChangedEvent[/classfnc]().[br]
		If you reimplement that function you will have to emit the signal manually (if you still need it).
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_slider, "slider", "widget")

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_slider, setTracking);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_slider, setValue);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_slider, setMinValue);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_slider, setMaxValue);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_slider, setLineStep);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_slider, setPageStep);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_slider, setTickInterval);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_slider, value);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_slider, minValue);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_slider, maxValue);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_slider, lineStep);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_slider, pageStep);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_slider, setTickmarks);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_slider, setOrientation);

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_slider, valueChangedEvent);
KVSO_END_REGISTERCLASS(KvsObject_slider)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_slider, KvsObject_widget)

KVSO_END_CONSTRUCTOR(KvsObject_slider)

KVSO_BEGIN_DESTRUCTOR(KvsObject_slider)

KVSO_END_CONSTRUCTOR(KvsObject_slider)

bool KvsObject_slider::init(KviKvsRunTimeContext *, KviKvsVariantList *)
{
	SET_OBJECT(QSlider)
	connect(widget(), SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
	return true;
}

KVSO_CLASS_FUNCTION(slider, setTracking)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bEnabled", KVS_PT_BOOL, 0, bEnabled)
	KVSO_PARAMETERS_END(c)
	((QSlider *)widget())->setTracking(bEnabled);
	return true;
}

KVSO_CLASS_FUNCTION(slider, setValue)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iValue;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("value", KVS_PT_INT, 0, iValue)
	KVSO_PARAMETERS_END(c)
	((QSlider *)widget())->setValue(iValue);
	return true;
}
KVSO_CLASS_FUNCTION(slider, setMinValue)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iMinvalue;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("minimum_value", KVS_PT_INT, 0, iMinvalue)
	KVSO_PARAMETERS_END(c)
	((QSlider *)widget())->setMinimum(iMinvalue);
	return true;
}
KVSO_CLASS_FUNCTION(slider, setMaxValue)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iMaxvalue;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("max_value", KVS_PT_INT, 0, iMaxvalue)
	KVSO_PARAMETERS_END(c)
	((QSlider *)widget())->setMaximum(iMaxvalue);
	return true;
}

KVSO_CLASS_FUNCTION(slider, setLineStep)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iLinestep;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("line_step", KVS_PT_INT, 0, iLinestep)
	KVSO_PARAMETERS_END(c)
	((QSlider *)widget())->setSingleStep(iLinestep);
	return true;
}
KVSO_CLASS_FUNCTION(slider, setPageStep)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iPagestep;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("page_step", KVS_PT_INT, 0, iPagestep)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QSlider *)widget())->setPageStep(iPagestep);
	return true;
}

KVSO_CLASS_FUNCTION(slider, setTickInterval)
{
	kvs_int_t iInterval;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("tick_interval", KVS_PT_INT, 0, iInterval)
	KVSO_PARAMETERS_END(c)
	((QSlider *)widget())->setTickInterval(iInterval);
	return true;
}

KVSO_CLASS_FUNCTION(slider, value)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((QSlider *)widget())->value());
	return true;
}

KVSO_CLASS_FUNCTION(slider, minValue)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((QSlider *)widget())->minimum());
	return true;
}
KVSO_CLASS_FUNCTION(slider, maxValue)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((QSlider *)widget())->maximum());
	return true;
}
KVSO_CLASS_FUNCTION(slider, lineStep)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((QSlider *)widget())->singleStep());
	return true;
}

KVSO_CLASS_FUNCTION(slider, pageStep)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((QSlider *)widget())->pageStep());
	return true;
}

KVSO_CLASS_FUNCTION(slider, setTickmarks)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szTick;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("tick_marks", KVS_PT_STRING, 0, szTick)
	KVSO_PARAMETERS_END(c)
	if(KviQString::equalCI(szTick, "NoMarks"))
		((QSlider *)widget())->setTickPosition(QSlider::NoTicks);
	else if(KviQString::equalCI(szTick, "Both"))
		((QSlider *)widget())->setTickPosition(QSlider::TicksBothSides);
	else if(KviQString::equalCI(szTick, "Above"))
		((QSlider *)widget())->setTickPosition(QSlider::TicksAbove);
	else if(KviQString::equalCI(szTick, "Below"))
		((QSlider *)widget())->setTickPosition(QSlider::TicksBelow);
	else if(KviQString::equalCI(szTick, "Left"))
		((QSlider *)widget())->setTickPosition(QSlider::TicksLeft);
	else if(KviQString::equalCI(szTick, "Right"))
		((QSlider *)widget())->setTickPosition(QSlider::TicksRight);
	else
		c->warning(__tr2qs_ctx("Unknown tickmark '%Q'", "objects"), &szTick);
	return true;
}
KVSO_CLASS_FUNCTION(slider, setOrientation)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szOrientation;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("orientation", KVS_PT_STRING, 0, szOrientation)
	KVSO_PARAMETERS_END(c)
	if(KviQString::equalCI(szOrientation, "Horizontal"))
		((QSlider *)widget())->setOrientation(Qt::Horizontal);
	else if(KviQString::equalCI(szOrientation, "Vertical"))
		((QSlider *)widget())->setOrientation(Qt::Vertical);
	else
		c->warning(__tr2qs_ctx("Unknown orientation '%Q'", "objects"), &szOrientation);
	return true;
}
KVSO_CLASS_FUNCTION(slider, valueChangedEvent)
{
	emitSignal("valueChanged", c, c->params());
	return true;
}

void KvsObject_slider::valueChanged(int value)
{
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)value));
	callFunction(this, "valueChangedEvent", &params);
}
