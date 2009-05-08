//=============================================================================
//
//   File : class_progressbar.cpp
//   Creation date : Fri Mar 18 14:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVirc irc client distribution
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

#include "class_progressbar.h"
#include "kvi_error.h"
#include "kvi_debug.h"

#include "kvi_locale.h"
#include "kvi_iconmanager.h"
#include <QProgressBar>




/*
	@doc:	progressbar
	@keyterms:
		progressbar object class,
	@title:
		progressbar class
	@type:
		class
	@short:
		Provides a horizontal progress bar.
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		This widget can be used to display a horizontal progress bar.
	@functions:
		!fn: $setProgress(<steps_value:uinteger>)
		Set the amount of steps completed.
		See also [classfnc]$settotalSteps[/classfnc]()
		!fn: $settotalSteps(<total_steps:uinteger>)
		The progress bar uses the concept of steps; you give it the total number of steps
		and the number of steps completed so far and it will display the percentage of steps that have been completed.
		See also [classfnc]$setProgress[/classfnc]()
		!fn: $reset()
		Reset the progress bar.
		!fn: $setPercentageVisible(<bVisible,boolean>)
		Sets whether the current progress value is displayed.
		See also [classfnc]$isPercentageVisible[/classfnc]()
		!fn: <boolean> $ispercentageVisible()
		This property holds whether the current progress value is displayed.
		See also [classfnc]$setPercentageVisible[/classfnc]()
*/

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_progressbar,"progressbar","widget")
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_progressbar,setProgress)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_progressbar,setFormat)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_progressbar,setTotalSteps)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_progressbar,reset)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_progressbar,setPercentageVisible)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_progressbar,percentageVisible)
KVSO_END_REGISTERCLASS(KviKvsObject_progressbar)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_progressbar,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_progressbar)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_progressbar)

KVSO_END_CONSTRUCTOR(KviKvsObject_progressbar)

bool KviKvsObject_progressbar::init(KviKvsRunTimeContext *,KviKvsVariantList *)
{
	SET_OBJECT (QProgressBar)
	return true;
}
KVSO_CLASS_FUNCTION(progressbar,setProgress)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t iValue;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("step_value",KVS_PT_UNSIGNEDINTEGER,0,iValue)
	KVSO_PARAMETERS_END(c)
	((QProgressBar *)widget())->setValue(iValue);
    return true;
}

KVSO_CLASS_FUNCTION(progressbar,setTotalSteps)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t iMax;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("total_steps",KVS_PT_UNSIGNEDINTEGER,0,iMax)
	KVSO_PARAMETERS_END(c)
	((QProgressBar *)widget())->setMaximum(iMax);
    return true;
}

KVSO_CLASS_FUNCTION(progressbar,reset)
{
	CHECK_INTERNAL_POINTER(widget())
	((QProgressBar *)widget())->reset();
	return true;
}


KVSO_CLASS_FUNCTION(progressbar,setPercentageVisible)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	((QProgressBar *)widget())->setTextVisible(bEnabled);
	return true;
}
KVSO_CLASS_FUNCTION(progressbar,setFormat)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szFormat;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_STRING,0,szFormat)
	KVSO_PARAMETERS_END(c)
	((QProgressBar *)widget())->setFormat(szFormat);
	return true;
}
KVSO_CLASS_FUNCTION(progressbar,percentageVisible)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setBoolean(((QProgressBar *)widget())->isTextVisible());
	return true;
}


