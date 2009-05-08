//
//   File : class_radiobutton.cpp
//   Creation date : Thu Feb 08 14:21:48 CEST 2005 
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
#include "class_radiobutton.h"
#include "kvi_error.h"
#include "kvi_debug.h"

#include "kvi_locale.h"
#include "kvi_iconmanager.h"

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

	KVSO_REGISTER_HANDLER(KviKvsObject_radiobutton,"setText", functionSetText)
	KVSO_REGISTER_HANDLER(KviKvsObject_radiobutton,"setChecked", functionSetChecked)
	KVSO_REGISTER_HANDLER(KviKvsObject_radiobutton,"isChecked", functionIsChecked)
	KVSO_REGISTER_HANDLER(KviKvsObject_radiobutton,"setImage", functionSetImage)
	KVSO_REGISTER_HANDLER(KviKvsObject_radiobutton,"toggleEvent",function_toggleEvent)

KVSO_END_REGISTERCLASS(KviKvsObject_radiobutton)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_radiobutton,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_radiobutton)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_radiobutton)

KVSO_END_CONSTRUCTOR(KviKvsObject_radiobutton)

bool KviKvsObject_radiobutton::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{
	setObject(new QRadioButton(parentScriptWidget(),name()),true);
	connect(widget(),SIGNAL(toggled(bool)),this,SLOT(toggled(bool)));
	return true;
}
bool KviKvsObject_radiobutton::functionSetText(KviKvsObjectFunctionCall *c)
{
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QRadioButton *)widget())->setText(szText);
	return true;
}
bool KviKvsObject_radiobutton::functionIsChecked(KviKvsObjectFunctionCall *c)
{
	if(widget()) c->returnValue()->setBoolean(((QRadioButton *)widget())->isChecked());
	return true;
}

bool KviKvsObject_radiobutton::functionSetChecked(KviKvsObjectFunctionCall *c)
{
	bool bChecked;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bChecked",KVS_PT_BOOL,0,bChecked)
	KVSO_PARAMETERS_END(c)
	if(widget()) ((QRadioButton *)widget())->setChecked(bChecked);
	return true;
}
bool KviKvsObject_radiobutton::functionSetImage(KviKvsObjectFunctionCall *c)
{
	QString icon;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("icon_id",KVS_PT_STRING,0,icon)
	KVSO_PARAMETERS_END(c)
	if (!widget()) return true;
	QPixmap * pix = g_pIconManager->getImage(icon);
	if(pix)widget()->setIcon(*pix);
	return true;
}
bool KviKvsObject_radiobutton::function_toggleEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("toggled",c,c->params());
	return true;
}

void KviKvsObject_radiobutton::toggled(bool b)
{
	KviKvsVariantList params(new KviKvsVariant(b));
	callFunction(this,"toggleEvent",&params);
}


#include "m_class_radiobutton.moc"
