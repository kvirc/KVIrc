//=============================================================================
//
//   File : class_radiobutton.cpp
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

#include "class_radiobutton.h"
#include "kvi_error.h"
#include "kvi_debug.h"
#include "kvi_locale.h"
#include "kvi_iconmanager.h"

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
		Returns 1 (TRUE) if the radio button is checked; otherwise returns 0 (FALSE).
		!fn: $setChecked(<bChecked:boolean>)
		Sets whether the radio button is checked to check.
		!fn: $setPixmap(<image_id>)
		Sets the pixmap shown on the radiobutton..See the [doc:image_id]image identifier[/doc] documentation for the explaination
		of the <image_id> parameter.
		!fn: <bool>$toggleEvent()
		Called by KVIrc when the radibutton state is toggled.
		The default implementation emits the toggled(<bool>) signal.
		!sg: $toggled()
		This signal is emitted by the default implementation of [classfnc]$toggleEvent[/classfnc]().[br]


*/


KVSO_BEGIN_REGISTERCLASS(KviKvsObject_radiobutton,"radiobutton","widget")

	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_radiobutton,setText)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_radiobutton,setChecked)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_radiobutton,isChecked)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_radiobutton,setImage)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_radiobutton,toggleEvent)

KVSO_END_REGISTERCLASS(KviKvsObject_radiobutton)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_radiobutton,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_radiobutton)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_radiobutton)

KVSO_END_CONSTRUCTOR(KviKvsObject_radiobutton)

bool KviKvsObject_radiobutton::init(KviKvsRunTimeContext *,KviKvsVariantList *)
{
	SET_OBJECT(QRadioButton)
	connect(widget(),SIGNAL(toggled(bool)),this,SLOT(toggled(bool)));
	return true;
}
KVSO_CLASS_FUNCTION(radiobutton,setText)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	((QRadioButton *)widget())->setText(szText);
	return true;
}
KVSO_CLASS_FUNCTION(radiobutton,isChecked)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setBoolean(((QRadioButton *)widget())->isChecked());
	return true;
}

KVSO_CLASS_FUNCTION(radiobutton,setChecked)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bChecked;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bChecked",KVS_PT_BOOL,0,bChecked)
	KVSO_PARAMETERS_END(c)
	((QRadioButton *)widget())->setChecked(bChecked);
	return true;
}
KVSO_CLASS_FUNCTION(radiobutton,setImage)
{
	CHECK_INTERNAL_POINTER(widget())
	QString icon;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("icon_id",KVS_PT_STRING,0,icon)
	KVSO_PARAMETERS_END(c)
	QPixmap * pix = g_pIconManager->getImage(icon);
	if(pix)((QRadioButton *)widget())->setIcon(*pix);
	return true;
}
KVSO_CLASS_FUNCTION(radiobutton,toggleEvent)
{
	emitSignal("toggled",c,c->params());
	return true;
}

void KviKvsObject_radiobutton::toggled(bool b)
{
	KviKvsVariantList params(new KviKvsVariant(b));
	callFunction(this,"toggleEvent",&params);
}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_radiobutton.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

