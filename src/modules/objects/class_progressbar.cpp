//
//   File : class_progressbar.cpp
//   Creation date : Fri Jan 28 14:21:48 CEST 2005 
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

#include "class_progressbar.h"
#include "kvi_error.h"
#include "kvi_debug.h"

#include "kvi_locale.h"
#include "kvi_iconmanager.h"
#include <q3progressbar.h>
#define KVI_PROGRESS_BAR Q3ProgressBar

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
		!fn: $setCenterIndicator(<bEnabled:boolean>)
		Sets whether the indicator string should be centered to on.
		See also [classfnc]$isCenterIndicator[/classfnc]() 
		!fn: <boolean> $isCenterIndicator()
		This property holds whether the indicator string should be centered.
		See also [classfnc]$setCenterIndicator[/classfnc]() 
		!fn: $setPercentageVisible(<bVisible,boolean>)
		Sets whether the current progress value is displayed.
		See also [classfnc]$isPercentageVisible[/classfnc]() 
		!fn: <boolean> $ispercentageVisible()
		This property holds whether the current progress value is displayed.
		See also [classfnc]$setPercentageVisible[/classfnc]() 
*/

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_progressbar,"progressbar","widget")
	KVSO_REGISTER_HANDLER(KviKvsObject_progressbar,"setProgress", functionSetProgress)
	KVSO_REGISTER_HANDLER(KviKvsObject_progressbar,"setTotalSteps", functionSetTotalSteps)
	KVSO_REGISTER_HANDLER(KviKvsObject_progressbar,"reset", functionReset)
	KVSO_REGISTER_HANDLER(KviKvsObject_progressbar,"setCenterIndicator", functionSetCenterIndicator)
	KVSO_REGISTER_HANDLER(KviKvsObject_progressbar,"setPercentageVisible", functionSetPercentageVisible)
	KVSO_REGISTER_HANDLER(KviKvsObject_progressbar,"isCenterIndicator", functionCenterIndicator)
	KVSO_REGISTER_HANDLER(KviKvsObject_progressbar,"isPercentageVisible", functionPercentageVisible)
KVSO_END_REGISTERCLASS(KviKvsObject_progressbar)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_progressbar,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_progressbar)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_progressbar)

KVSO_END_CONSTRUCTOR(KviKvsObject_progressbar)

bool KviKvsObject_progressbar::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{	
	Q3ProgressBar *pbar=new Q3ProgressBar(parentScriptWidget());
	pbar->setObjectName(name());
	setObject(pbar,true);
	return true;
}

bool KviKvsObject_progressbar::functionSetProgress(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t iProgress;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("step_value",KVS_PT_UNSIGNEDINTEGER,0,iProgress)
	KVSO_PARAMETERS_END(c)
	if (widget()) ((KVI_PROGRESS_BAR *)widget())->setProgress(iProgress);
    return true;
}
bool KviKvsObject_progressbar::functionSetTotalSteps(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t iSteps;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("total_steps",KVS_PT_UNSIGNEDINTEGER,0,iSteps)
	KVSO_PARAMETERS_END(c)
	if (widget()) ((KVI_PROGRESS_BAR *)widget())->setTotalSteps(iSteps);
    return true;
}

bool KviKvsObject_progressbar::functionReset(KviKvsObjectFunctionCall *c)
{
	if (widget()) ((KVI_PROGRESS_BAR *)widget())->reset();
	return true;
}

bool KviKvsObject_progressbar::functionSetCenterIndicator(KviKvsObjectFunctionCall *c)
{
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bFlag)
	KVSO_PARAMETERS_END(c)
	if(widget()) ((KVI_PROGRESS_BAR *)widget())->setCenterIndicator(bFlag);
	return true;
}
bool KviKvsObject_progressbar::functionCenterIndicator(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setBoolean(((KVI_PROGRESS_BAR *)widget())->centerIndicator());
	return true;
}

bool KviKvsObject_progressbar::functionSetPercentageVisible(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((KVI_PROGRESS_BAR *)widget())->setPercentageVisible(bEnabled);
	return true;
}
bool KviKvsObject_progressbar::functionPercentageVisible(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setBoolean(((KVI_PROGRESS_BAR *)widget())->percentageVisible());
	return true;
}
