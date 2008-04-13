//
//   File : class_button.cpp
//   Creation date : Wed 13 Sep 2000 02:42:05 CEST by Krzysztof Godlewski
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

#include "kvi_debug.h"
#include "kvi_error.h"
#include "kvi_iconmanager.h"
#include "class_button.h"

#include <QIcon>
#include <QPushButton>

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_button,"button","widget")

	KVSO_REGISTER_HANDLER(KviKvsObject_button,"setText", functionSetText)
	KVSO_REGISTER_HANDLER(KviKvsObject_button,"text", functionText)
	KVSO_REGISTER_HANDLER(KviKvsObject_button,"setAutoDefault", functionSetAutoDefault)
	KVSO_REGISTER_HANDLER(KviKvsObject_button,"setToggleButton", functionSetToggleButton)
	KVSO_REGISTER_HANDLER(KviKvsObject_button,"setOn", functionSetOn)
	KVSO_REGISTER_HANDLER(KviKvsObject_button,"isOn", functionIsOn)
	KVSO_REGISTER_HANDLER(KviKvsObject_button,"toggle", functionToggle)
	KVSO_REGISTER_HANDLER(KviKvsObject_button,"setIsMenuButton", functionSetIsMenuButton)
	KVSO_REGISTER_HANDLER(KviKvsObject_button,"isMenuButton", functionIsMenuButton)
	KVSO_REGISTER_HANDLER(KviKvsObject_button,"clickEvent", functionclickEvent)
	KVSO_REGISTER_HANDLER(KviKvsObject_button,"setImage", functionSetImage)


KVSO_END_REGISTERCLASS(KviKvsObject_button)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_button,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_button)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_button)

KVSO_END_CONSTRUCTOR(KviKvsObject_button)

bool KviKvsObject_button::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{
	SET_OBJECT(QPushButton);
	connect(widget(),SIGNAL(clicked()),this,SLOT(slotClicked()));
	return true;
}

bool KviKvsObject_button::functionText(KviKvsObjectFunctionCall *c)
{
	if(widget()) c->returnValue()->setString(((QPushButton *)widget())->text());
	return true;
}

bool KviKvsObject_button::functionSetText(KviKvsObjectFunctionCall *c)
{
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QPushButton *)widget())->setText(szText);
	return true;
}
bool KviKvsObject_button::functionSetAutoDefault(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QPushButton *)widget())->setAutoDefault(bEnabled);
	return true;
}
bool KviKvsObject_button::functionSetToggleButton(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QPushButton *)widget())->setToggleButton(bEnabled);
	return true;
}
bool KviKvsObject_button::functionSetOn(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QPushButton *)widget())->setOn(bEnabled);
	return true;
}
// FIX ME
bool KviKvsObject_button::functionSetIsMenuButton(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	/*if(widget())
		((QPushButton *)widget())->setIsMenuButton(bEnabled);
		*/

	return true;
}
bool KviKvsObject_button::functionIsMenuButton(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setBoolean(((QPushButton *)widget())->isMenuButton());
	return true;
}
//
bool KviKvsObject_button::functionIsOn(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setBoolean(((QPushButton *)widget())->isOn());
	return true;
}
bool KviKvsObject_button::functionToggle(KviKvsObjectFunctionCall *c)
{
	if(widget()) ((QPushButton *)widget())->toggle();
	return true;
}
// FIX ME
bool KviKvsObject_button::functionSetImage(KviKvsObjectFunctionCall *c)
{
	QString icon;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("icon",KVS_PT_STRING,0,icon)
	KVSO_PARAMETERS_END(c)
	if (!widget()) return true;
	QPixmap * pix = g_pIconManager->getImage(icon);
	if(pix)	((QPushButton *)widget())->setIconSet(*pix);
	else((QPushButton *)widget())->setIcon(QIconSet());
	return true;
}
bool KviKvsObject_button::functionclickEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("clicked",c);
	return true;
}

void KviKvsObject_button::slotClicked()
{
	KviKvsVariantList *params=0;
	callFunction(this,"clickEvent",params);
}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_button.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

