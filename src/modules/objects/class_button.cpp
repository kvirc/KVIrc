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
#include <qiconset.h>
#include "kvi_iconmanager.h"

#include "class_button.h"
/*
static KviScriptObjectClass * g_pButtonClass = 0;

static KviScriptObject * buttonClassCreateInstance(KviScriptObjectClass * c,
	KviScriptObject * p, const char * n)
{
	return new KviScriptButtonObject(c, p, n);
}

KviScriptButtonObject::KviScriptButtonObject(KviScriptObjectClass * c, KviScriptObject * p,
	const char * n) : KviScriptWidgetObject(c, p, n)
{
}

KviScriptButtonObject::~KviScriptButtonObject()
{
//	debug("KviScriptButtonObject::~KviScriptButtonObject(%s)",id());
}


/*
	@doc: button
	@keyterms:
		button object class
	@title:
		button class
	@type:
		class
	@short:
		A simple, well-known button
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		A button - nothing more, nothing else...
	@functions:
		!fn: $setText(<text:string>)
		This function sets the text for this button.
		!fn: <string> $text()
		Returns the current text of the button
		!fn: $setImage(<image_id:string>)
		Sets the image to be displayed on this label.
		Giving empty argument clears the pixmap[br]
		See the [doc:image_id]image identifier[/doc] documentation for
		the explaination of the <image_id> parameter.
		!fn: $clickEvent()
		This function is called by the framework when the button is clicked.[br]
		You can reimplement it to handle the user click events.[br]
		The default implementation emits the [classfnc]$clicked[/classfnc]() signal,
		so it is easy to handle the clicks from many buttons without reimplementing
		the $clickEvent() for every one.[br]
		Note:[br]
		If you reimplement this function to catch the user click events, you will have
		to emit the signal by yourself (if you still need it , obviously).
	@signals:
		!sg: $clicked()
		This signal is emitted by the default implementation of [classfnc]$clickEvent[/classfnc]().[br]
		If you reimplement that function you will have to emit the signal manually (if you still need it).

*/
/*
bool KviScriptButtonObject::init(KviCommand *,KviParameterList *params)
{
	setObject(new QPushButton(parentScriptWidget(),name()),true);
	connect(widget(),SIGNAL(clicked()),this,SLOT(slotClicked()));
	return true;
}

#define buttonFuncReg(__nam, __func) \
	g_pButtonClass->registerFunctionHandler(__nam, \
	(KviScriptObjectFunctionHandlerProc)(KVI_PTR2MEMBER(KviScriptButtonObject::__func)), \
	0, true);

void KviScriptButtonObject::registerSelf()
{
	KviScriptObjectClass * base = g_pScriptObjectController->
		lookupClass("widget");
	__range_valid(base);

	g_pButtonClass = new KviScriptObjectClass(base, "button",
		buttonClassCreateInstance, true);

	buttonFuncReg("setText", functionSetText);
	buttonFuncReg("text", functionText);
	buttonFuncReg("setAutoDefault", functionSetAutoDefault);
	buttonFuncReg("setToggleButton", functionSetToggleButton);
	buttonFuncReg("setOn", functionSetOn);
	buttonFuncReg("isOn", functionIsOn);
	buttonFuncReg("toggle", functionToggle);
	buttonFuncReg("setIsMenuButton", functionSetIsMenuButton);
	buttonFuncReg("isMenuButton", functionIsMenuButton);
	buttonFuncReg("clickEvent", functionClickEvent);
    buttonFuncReg("setImage", functionSetImage);

//	g_pButtonClass->registerFunctionHandler("clickEvent",0,"emit clicked()");
// slots
//	buttonFuncReg("slotClicked", functionSlotClicked);
//	buttonFuncReg("slotClicked", __slotClicked);
	//g_pButtonClass->registerEmptyFunctionHandler("slotClicked");
}

void KviScriptButtonObject::unregisterSelf()
{
	delete g_pButtonClass;
    g_pButtonClass = 0;
}

bool KviScriptButtonObject::functionSetText(KviCommand *, KviParameterList * p, \
	KviStr &)
{
	if(widget())
	{
		if(p->first())
			((QPushButton *)widget())->setText(p->first()->ptr());
		else
			((QPushButton *)widget())->setText("");
	}
	return true;
}

bool KviScriptButtonObject::functionText(KviCommand *, KviParameterList *, KviStr & b)
{
	if(widget())
	{
		KviStr s = ((QPushButton *)widget())->text();
		b.append(s);
	}
	return true;
}

bool KviScriptButtonObject::functionSetAutoDefault(KviCommand *, KviParameterList * p,
	KviStr &)
{
	if(widget())
	{
		((QPushButton *)widget())->setAutoDefault(p->getBool());
	}
	return true;
}

bool KviScriptButtonObject::functionSetToggleButton(KviCommand *,KviParameterList * p,
	KviStr &)
{
	if(widget())
	{
		((QPushButton *)widget())->setToggleButton(p->getBool());
	}
	return true;
}

bool KviScriptButtonObject::functionSetOn(KviCommand *,KviParameterList * p, KviStr &)
{
	if(widget())
	{
		((QPushButton *)widget())->setOn(p->getBool());
	}
	return true;
}

bool KviScriptButtonObject::functionIsOn(KviCommand *, KviParameterList *, KviStr & b)
{
	if(widget())
	{
		bool t = ((QPushButton *)widget())->isOn();
		b.append( t ? '1' : '0' );
	}
	return true;
}

bool KviScriptButtonObject::functionToggle(KviCommand *,KviParameterList * p,
	KviStr &)
{
	if(widget())
	{
		((QPushButton *)widget())->toggle();
	}
	return true;
}

bool KviScriptButtonObject::functionSetIsMenuButton(KviCommand *,KviParameterList * p,
	KviStr &)
{
	if(widget())
	{
		((QPushButton *)widget())->setIsMenuButton(p->getBool());
	}
	return true;
}

bool KviScriptButtonObject::functionIsMenuButton(KviCommand *,KviParameterList *,
	KviStr & b)
{
	if(widget())
	{
		bool t = ((QPushButton *)widget())->isMenuButton();
		b.append( t ? '1' : '0' );
	}
	return true;
}
bool KviScriptButtonObject::functionSetImage(KviCommand * c, KviParameterList * p,
  	         KviStr & b)
{
    QPixmap *pix = 0;

    if(!widget())return true;

    if(p->first())pix = g_pIconManager->getImage(p->first()->ptr());

    if(pix)
    {
	    ((QPushButton *)widget())->setIconSet(QIconSet(*pix,QIconSet::Small));
  	 } else {
  	((QPushButton *)widget())->setIconSet(QIconSet());
  	   }
  	    return true;
  }


bool KviScriptButtonObject::functionClickEvent(KviCommand *c,KviParameterList *,KviStr &)
{
	ENTER_STACK_FRAME(c,"button::clickEvent");
	emitSignal("clicked",0,0,c);
	return c->leaveStackFrame();
}

void KviScriptButtonObject::slotClicked()
{
	callEventFunction("clickEvent");
}
*/
//---------------------------------------------------------------------------------

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
	setObject(new QPushButton(parentScriptWidget(),name()),true);
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
bool KviKvsObject_button::functionSetIsMenuButton(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QPushButton *)widget())->setIsMenuButton(bEnabled);
	return true;
}
bool KviKvsObject_button::functionIsMenuButton(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setBoolean(((QPushButton *)widget())->isMenuButton());
	return true;
}
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
bool KviKvsObject_button::functionSetImage(KviKvsObjectFunctionCall *c)
{
	QString icon;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("icon",KVS_PT_STRING,0,icon)
	KVSO_PARAMETERS_END(c)
	if (!widget()) return true;
	QPixmap * pix = g_pIconManager->getImage(icon);
	if(pix)
		((QPushButton *)widget())->setIconSet(QIconSet(*pix,QIconSet::Small));
	else
		((QPushButton *)widget())->setIconSet(QIconSet());
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


#include "m_class_button.moc"
