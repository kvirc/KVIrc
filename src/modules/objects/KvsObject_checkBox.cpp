//=============================================================================
//
//   File : KvsObject_checkBox.cpp
//   Creation date : Wed 13 Sep 2000 02:42:05 CEST by Krzysztof Godlewski
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000 Krzysztof Godlewski
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviError.h"
#include "kvi_debug.h"
#include "KviLocale.h"
#include "KvsObject_checkBox.h"

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
		Returns [b]1[/b] if the check box is checked, and [b]0[/b] otherwise.[br]
		See also [classfnc]$setChecked[/classfnc]().
		!fn: $setChecked([<bChecked:bool>])
		Sets the check box 'checked state' to <bool>.
		See also [classfnc]$isChecked[/classfnc]().
		!fn: $setText([<text:string>])
		Sets the label text associated with the checkbox to <text>.[br]
		!fn: $toggleEvent(<bToggled:bool>)
		Called by KVIrc when the checkbox state is toggled.
		The default implementation emits the toggled(<bool>) signal.
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_checkBox, "checkbox", "button")
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_checkBox, setChecked)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_checkBox, isChecked)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_checkBox, toggleEvent)
KVSO_END_REGISTERCLASS(KvsObject_checkBox)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_checkBox, KvsObject_button)

KVSO_END_CONSTRUCTOR(KvsObject_checkBox)

KVSO_BEGIN_DESTRUCTOR(KvsObject_checkBox)

KVSO_END_CONSTRUCTOR(KvsObject_checkBox)

bool KvsObject_checkBox::init(KviKvsRunTimeContext *, KviKvsVariantList *)
{
	QCheckBox * cb = new QCheckBox(parentScriptWidget());
	cb->setObjectName(getName().toUtf8().data());
	setObject(cb, true);
	connect(cb, SIGNAL(toggled(bool)), this, SLOT(toggled(bool)));
	connect(widget(), SIGNAL(clicked()), this, SLOT(slotClicked()));
	return true;
}

KVSO_CLASS_FUNCTION(checkBox, isChecked)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setBoolean(((QCheckBox *)widget())->isChecked());
	return true;
}

KVSO_CLASS_FUNCTION(checkBox, setChecked)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bChecked;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bChecked", KVS_PT_BOOL, KVS_PF_OPTIONAL, bChecked)
	KVSO_PARAMETERS_END(c)
	((QCheckBox *)widget())->setChecked(bChecked);
	return true;
}

KVSO_CLASS_FUNCTION(checkBox, setText)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("<text>", KVS_PT_STRING, 0, szText)
	KVSO_PARAMETERS_END(c)
	((QCheckBox *)widget())->setText(szText);
	return true;
}

KVSO_CLASS_FUNCTION(checkBox, toggleEvent)
{
	emitSignal("toggled", c, c->params());
	return true;
}

//slots
void KvsObject_checkBox::toggled(bool b)
{
	KviKvsVariantList params(new KviKvsVariant(b));
	callFunction(this, "toggleEvent", &params);
}
