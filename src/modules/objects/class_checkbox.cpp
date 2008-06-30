//vim: ts=8
//   File : class_checkbox.cpp
//   Creation date : Wed Sep 20 23:01:47 CEST 2000 by Krzysztof Godlewski
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//


#include "kvi_error.h"
#include "kvi_debug.h"
#include "kvi_locale.h"
#include "class_checkbox.h"

#include <QCheckBox>

/*
	@doc:	checkbox
	@title:
		checkbox class
	@type:
		class
	@short:
		Check box with a text description
	@inherits:
		[class]object[/class]
		[class]button[/class]
	@description:
		This widget provides a check box - it is a kind of a toggle
		button. It can have two states: on (checked) and off
		(unchecked).
	@functions:
		!fn: <bool> $isChecked()
		Returns '1' if the check box is checked, '0' otherwise.[br]
		See also [classfnc]$setChecked[/classfnc]().
		!fn: $setChecked([<bChecked:bool>])
		Sets the check box 'checked state' to <bool>.
		!fn: $setText([<text:string>])
		Sets the check box 'checked state' to <bool>.[br]
		See also [classfnc]$isChecked[/classfnc]().
		!fn: $toggleEvent(<bToggled:bool>)
		Called by KVIrc when the checkbox state is toggled.
		The default implementation emits the toggled(<bool>) signal.
*/

//---------------------------------------------------------------------------------

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_checkbox,"checkbox","button")
	// appearance
	KVSO_REGISTER_HANDLER(KviKvsObject_checkbox,"setChecked",function_setChecked)
	KVSO_REGISTER_HANDLER(KviKvsObject_checkbox,"isChecked",function_isChecked)
	KVSO_REGISTER_HANDLER(KviKvsObject_checkbox,"toggleEvent",function_toggleEvent)
	KVSO_REGISTER_HANDLER(KviKvsObject_checkbox,"setText",function_setText)
KVSO_END_REGISTERCLASS(KviKvsObject_checkbox)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_checkbox,KviKvsObject_button)

KVSO_END_CONSTRUCTOR(KviKvsObject_checkbox)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_checkbox)

KVSO_END_CONSTRUCTOR(KviKvsObject_checkbox)

bool KviKvsObject_checkbox::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{
	QCheckBox  * cb = new QCheckBox(parentScriptWidget(), getName().toUtf8().data());
	setObject(cb, true);
	connect(cb,SIGNAL(toggled(bool)),this,SLOT(toggled(bool)));
	connect(widget(),SIGNAL(clicked()),this,SLOT(slotClicked()));
	return true;
}

bool KviKvsObject_checkbox::function_isChecked(KviKvsObjectFunctionCall *c)
{
	if(widget()) c->returnValue()->setBoolean(((QCheckBox *)widget())->isChecked());
	return true;
}

bool KviKvsObject_checkbox::function_setChecked(KviKvsObjectFunctionCall *c)
{
	bool bChecked;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bChecked",KVS_PT_BOOL,KVS_PF_OPTIONAL,bChecked)
	KVSO_PARAMETERS_END(c)
	if(widget()) ((QCheckBox *)widget())->setChecked(bChecked);
	return true;
}

bool KviKvsObject_checkbox::function_setText(KviKvsObjectFunctionCall *c)
{
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	if(widget()) ((QCheckBox *)widget())->setText(szText);
	return true;
}

bool KviKvsObject_checkbox::function_toggleEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("toggled",c,c->params());
	return true;
}

void KviKvsObject_checkbox::toggled(bool b)
{
	KviKvsVariantList params(new KviKvsVariant(b));
	callFunction(this,"toggleEvent",&params);
}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_checkbox.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

