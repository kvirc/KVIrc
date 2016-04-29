//=============================================================================
//
//   File : KvsObject_radioButton.cpp
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

#include "KvsObject_radioButton.h"
#include "KviError.h"
#include "kvi_debug.h"
#include "KviLocale.h"
#include "KviIconManager.h"

#include <QRadioButton>

/*
	@doc: radiobutton
	@keyterms:
		radiobutton widget class
	@title:
		radiobutton class
	@type:
		class
	@short:
		A radiobutton class
	@inherits:
		[class]widget[/class]
	@description:
		The RadioButton widget provides a radio button with a text or pixmap label.
	@functions:
		!fn: $settext(<text:string>)
		Sets the text that will appear in the radiobutton.
		!fn: <boolean> $isChecked()
		Returns 1 (true) if the radio button is checked; otherwise returns 0 (false).
		!fn: $setChecked(<bChecked:boolean>)
		Sets whether the radio button is checked to check.
		!fn: $setPixmap(<image_id>)
		Sets the pixmap shown on the radiobutton. See the [doc:image_id]image identifier[/doc]
		documentation for the explanation of the <image_id> parameter.
		!fn: <bool>$toggleEvent()
		Called by KVIrc when the radiobutton state is toggled.
		The default implementation emits the toggled(<bool>) signal.
		!sg: $toggled()
		This signal is emitted by the default implementation of [classfnc]$toggleEvent[/classfnc]().[br]
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_radioButton, "radiobutton", "widget")

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_radioButton, setText)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_radioButton, setChecked)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_radioButton, isChecked)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_radioButton, setImage)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_radioButton, toggleEvent)

KVSO_END_REGISTERCLASS(KvsObject_radioButton)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_radioButton, KvsObject_widget)

KVSO_END_CONSTRUCTOR(KvsObject_radioButton)

KVSO_BEGIN_DESTRUCTOR(KvsObject_radioButton)

KVSO_END_CONSTRUCTOR(KvsObject_radioButton)

bool KvsObject_radioButton::init(KviKvsRunTimeContext *, KviKvsVariantList *)
{
	SET_OBJECT(QRadioButton)
	connect(widget(), SIGNAL(toggled(bool)), this, SLOT(toggled(bool)));
	return true;
}
KVSO_CLASS_FUNCTION(radioButton, setText)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("text", KVS_PT_STRING, 0, szText)
	KVSO_PARAMETERS_END(c)
	((QRadioButton *)widget())->setText(szText);
	return true;
}
KVSO_CLASS_FUNCTION(radioButton, isChecked)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setBoolean(((QRadioButton *)widget())->isChecked());
	return true;
}

KVSO_CLASS_FUNCTION(radioButton, setChecked)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bChecked;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bChecked", KVS_PT_BOOL, 0, bChecked)
	KVSO_PARAMETERS_END(c)
	((QRadioButton *)widget())->setChecked(bChecked);
	return true;
}
KVSO_CLASS_FUNCTION(radioButton, setImage)
{
	CHECK_INTERNAL_POINTER(widget())
	QString icon;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("icon_id", KVS_PT_STRING, 0, icon)
	KVSO_PARAMETERS_END(c)
	QPixmap * pix = g_pIconManager->getImage(icon);
	if(pix)
		((QRadioButton *)widget())->setIcon(*pix);
	return true;
}
KVSO_CLASS_FUNCTION(radioButton, toggleEvent)
{
	emitSignal("toggled", c, c->params());
	return true;
}

void KvsObject_radioButton::toggled(bool b)
{
	KviKvsVariantList params(new KviKvsVariant(b));
	callFunction(this, "toggleEvent", &params);
}
