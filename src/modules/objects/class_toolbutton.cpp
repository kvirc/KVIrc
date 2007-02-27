//mdm:
//   File : class_toolbutton.cpp
//   Creation date : Wed Feb 23 16:39:48 CEST 2005
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

#include "class_toolbutton.h"
#include "kvi_error.h"
#include "kvi_debug.h"

#include "kvi_locale.h"
#include "kvi_iconmanager.h"
#include "kvi_tal_popupmenu.h"
/*
	@doc:	toolbutton
	@keyterms:
		toolbutton object class,
	@title:
		toolbutton class
	@type:
		class
	@short:
		Provides a toolbutton for toolbar widget.
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		The toolbutton class provides a quick-access button to commands or options, usually used inside a ToolBar.
	@functions:
		!fn:$setImage(<icon_id:string>)
		Sets the image for this button.
		!fn:$setUsesBigPixmap(<bEnabled:boolean>)
		Sets whether this toolbutton uses big pixmaps to enable.
		!fn:$usesBigPixmap(<bEnabled:boolean>)
		This property holds whether this toolbutton uses big pixmaps.
		!fn:setUsesTextLabel(<bEnabled:boolean>)
		Sets whether the toolbutton show a texlabel below the pixmap of the button.
		!fn:$usesTextLabel()
		Return 1 if the setUsesTextLabel is enabled, otherwise return 0.
		!fn:$setTextLabel(<text:String>,[tooltip:string])
		Sets the label of this button to <text> and automatically sets it as a tool tip if <tip> is TRUE.
		!fn:<string> $textLabel()
		Returns the label of tthe button.
		!fn:$setAutoRaise(<bAutoraise:boolean>)
		Sets whether auto-raising is enabled.
		!fn:<boolean> $autoRaise()
		Returns 1 if autoRaise is enabled, otherwise returns 0.
		!fn:$setToggleButton(<bToggle:boolean>)
		Sets whether the tool button is a toggle button 1 enable, 0 disable.
		!fn:<boolean> $toggle()
		Toggles the state of the tool button.
		!fn:$setOn(<bEnabled:boolean>)
		Sets whether the tool button is on to the bool value: 1 enable, 0 disable.
		// findme
		!fn:$setPopup (<toolbutton:object>)
		!fn:$setPopupDelay (<int delay>)
		Sets the time delay between pressing the button and the appearance of the associated popup menu in milliseconds to delay.[br]
		A good value is 100.
		!fn:<integer> $pupupDelay()
		Returns the time delay between pressing the button and the appearance of the associated popup menu in milliseconds.
		!fn:$openPopup()
		Opens the associated popup menu. If there is no such menu, this function does nothing.
		!fn:setTextPosition(<text_position:string>)
		Sets the position of the tool button's textLabel in relation to the tool button's icon.[br]
		Valid texpos values are:[br]
		- BesideIcon : The text appears beside the icon.[br]
		- BelowIcon  : The text appears below the icon.
		!fn:<string> $textPosition()
		Returns the position of the text label of this button.
		!fn: $clickEvent()
		This function is called when the toolbutton is clicked.[br]
		You can reimplement it to handle the user click events.[br]
		The default implementation emits the [classfnc]$clicked[/classfnc]() signal.
	@signals:
		!sg: $clicked()
		This signal is emitted by the default implementation of [classfnc]$clickEvent[/classfnc]().[br]
*/

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_toolbutton,"toolbutton","widget")
	KVSO_REGISTER_HANDLER(KviKvsObject_toolbutton,"setImage", functionsetImage)
	KVSO_REGISTER_HANDLER(KviKvsObject_toolbutton,"setUsesBigPixmap", functionsetUsesBigPixmap)
	KVSO_REGISTER_HANDLER(KviKvsObject_toolbutton,"usesBigPixmap", functionusesBigPixmap)
	KVSO_REGISTER_HANDLER(KviKvsObject_toolbutton,"setUsesTextLabel", functionsetUsesTextLabel)
	KVSO_REGISTER_HANDLER(KviKvsObject_toolbutton,"usesTextLabel", functionusesTextLabel)
	KVSO_REGISTER_HANDLER(KviKvsObject_toolbutton,"setAutoRaise", function_setAutoRaise)
	KVSO_REGISTER_HANDLER(KviKvsObject_toolbutton,"autoRaise", function_autoRaise)
	KVSO_REGISTER_HANDLER(KviKvsObject_toolbutton,"setTextLabel", functionsetTextLabel)
	KVSO_REGISTER_HANDLER(KviKvsObject_toolbutton,"textLabel", functiontextLabel)
	KVSO_REGISTER_HANDLER(KviKvsObject_toolbutton,"setPopup", functionsetPopup)
	KVSO_REGISTER_HANDLER(KviKvsObject_toolbutton,"openPopup", functionopenPopup)

	KVSO_REGISTER_HANDLER(KviKvsObject_toolbutton,"setToggleButton", functionsetToggleButton)
	KVSO_REGISTER_HANDLER(KviKvsObject_toolbutton,"toggle", functiontoggle)
	KVSO_REGISTER_HANDLER(KviKvsObject_toolbutton,"setOn", functionsetOn)

	KVSO_REGISTER_HANDLER(KviKvsObject_toolbutton,"setPopupDelay", functionsetPopupDelay)
	KVSO_REGISTER_HANDLER(KviKvsObject_toolbutton,"popupDelay", functionpopupDelay)
	KVSO_REGISTER_HANDLER(KviKvsObject_toolbutton,"setTextPosition", functionsetTextPosition)
	KVSO_REGISTER_HANDLER(KviKvsObject_toolbutton,"textPosition", functiontextPosition)

	KVSO_REGISTER_HANDLER(KviKvsObject_toolbutton,"clickEvent",function_clickEvent)
KVSO_END_REGISTERCLASS(KviKvsObject_toolbutton)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_toolbutton,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_toolbutton)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_toolbutton)

KVSO_END_CONSTRUCTOR(KviKvsObject_toolbutton)

bool KviKvsObject_toolbutton::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{
	setObject(new QToolButton(parentScriptWidget(), name()), true);
	connect(widget(),SIGNAL(clicked()),this,SLOT(slotClicked()));
	return true;
}

bool KviKvsObject_toolbutton::functionsetImage(KviKvsObjectFunctionCall *c)
{
	QString icon;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("icon_id",KVS_PT_STRING,0,icon)
	KVSO_PARAMETERS_END(c)
	if (!widget()) return true;
	QPixmap * pix = g_pIconManager->getImage(icon);
	if(pix){
		#ifdef COMPILE_USE_QT4
			((QToolButton *)widget())->setIconSet(QIconSet(*pix));
		#else
			((QToolButton *)widget())->setIconSet(QIconSet(*pix,QIconSet::Small));
		#endif
	}
	else
		((QToolButton *)widget())->setIconSet(QIconSet());
	return true;
}
bool KviKvsObject_toolbutton::functionsetUsesBigPixmap(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QToolButton *)widget())->setUsesBigPixmap(bEnabled);
	return true;
}
bool KviKvsObject_toolbutton::functionusesBigPixmap(KviKvsObjectFunctionCall *c)
{
	if (widget())
		c->returnValue()->setBoolean(((QToolButton *)widget())->usesBigPixmap());
	return true;
}
bool KviKvsObject_toolbutton::functionsetUsesTextLabel(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QToolButton *)widget())->setUsesTextLabel(bEnabled);
	return true;
}
bool KviKvsObject_toolbutton::functionusesTextLabel(KviKvsObjectFunctionCall *c)
{
	if (widget())
		c->returnValue()->setBoolean(((QToolButton *)widget())->usesTextLabel());
	return true;
}
bool KviKvsObject_toolbutton::function_setAutoRaise(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QToolButton *)widget())->setAutoRaise(bEnabled);
	return true;
}
bool KviKvsObject_toolbutton::function_autoRaise(KviKvsObjectFunctionCall *c)
{
	if(widget())
		c->returnValue()->setBoolean(((QToolButton *)widget())->autoRaise());
	return true;
}
bool KviKvsObject_toolbutton::functionsetOn(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QToolButton *)widget())->setOn(bEnabled);
	return true;
}

bool KviKvsObject_toolbutton::functionsetToggleButton(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QToolButton *)widget())->setToggleButton(bEnabled);
	return true;
}
bool KviKvsObject_toolbutton::functiontoggle(KviKvsObjectFunctionCall *c)
{
	if (widget())
		((QToolButton *)widget())->toggle();
	return true;
}

bool KviKvsObject_toolbutton::functionsetTextLabel(KviKvsObjectFunctionCall *c)
{
	QString szLabel,szTip;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("label",KVS_PT_STRING,0,szLabel)
		KVSO_PARAMETER("tooltip",KVS_PT_STRING,KVS_PF_OPTIONAL,szTip)
	KVSO_PARAMETERS_END(c)
	if(!widget()) return true;

	#ifdef COMPILE_USE_QT4
		((QToolButton *)widget())->setText(szLabel);
		if (!szTip.isEmpty()) ((QToolButton *)widget())->setToolTip(szTip);
	#else
		if (szTip.isEmpty()) ((QToolButton *)widget())->setTextLabel(szLabel);
			else ((QToolButton *)widget())->setTextLabel(szLabel,szTip);
	#endif
	return true;
}
bool KviKvsObject_toolbutton::functiontextLabel(KviKvsObjectFunctionCall *c)
{
	if (widget())
		c->returnValue()->setString(((QToolButton *)widget())->textLabel());
	return true;
}
bool KviKvsObject_toolbutton::functionsetPopup(KviKvsObjectFunctionCall *c)
{
	KviKvsObject *ob;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("widget",KVS_PT_HOBJECT,0,hObject)
	KVSO_PARAMETERS_END(c)
	ob=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if (!ob)
	{
		c->warning(__tr2qs("Widget parameter is not an object"));
		return true;
	}
	if (!ob->object())
	{
		c->warning(__tr2qs("Widget parameter is not a valid object"));
		return true;
	}
	if(!ob->object()->inherits("KviKvsObject_popupmenu"))
    {
		c->warning(__tr2qs("Can't add a non - popupmenu  object"));
        return TRUE;
    }
	if(widget())
		((QToolButton *)widget())->setPopup(((KviTalPopupMenu  *)(ob->object())));
	return true;
}
bool KviKvsObject_toolbutton::functionopenPopup(KviKvsObjectFunctionCall *c)
{
	if(widget())
		((QToolButton *)widget())->openPopup();
	return true;
}
bool KviKvsObject_toolbutton::functionsetPopupDelay(KviKvsObjectFunctionCall *c)
{
	kvs_int_t uDelay;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("delay",KVS_PT_UNSIGNEDINTEGER,0,uDelay)
	KVSO_PARAMETERS_END(c)
	if (widget())
		  ((QToolButton *)widget())->setPopupDelay(uDelay);
	return true;
}
bool KviKvsObject_toolbutton::functionpopupDelay(KviKvsObjectFunctionCall *c)
{
	if (widget())
		c->returnValue()->setInteger(((QToolButton *)widget())->popupDelay());
	return true;
}
bool KviKvsObject_toolbutton::functionsetTextPosition(KviKvsObjectFunctionCall *c)
{
	QString szPos;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("position",KVS_PT_STRING,0,szPos)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	if(KviQString::equalCI(szPos,"BesideIcon"))
			((QToolButton *)widget())->setTextPosition(QToolButton::BesideIcon);
	else if(KviQString::equalCI(szPos,"BelowIcon"))
			((QToolButton *)widget())->setTextPosition(QToolButton::BelowIcon);
	else c->warning(__tr2qs("Unknown text position '%Q'"),&szPos);
	return true;
}
bool KviKvsObject_toolbutton::functiontextPosition(KviKvsObjectFunctionCall *c)
{
	if(!widget()) return true;
	QString szPos="BelowIcon";
	if ((((QToolButton *)widget())->textPosition())==(QToolButton::BesideIcon))
		szPos="BesideIcon";
	c->returnValue()->setString(szPos);
	return true;
}
bool KviKvsObject_toolbutton::function_clickEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("clicked",c);
	return true;
}

void KviKvsObject_toolbutton::slotClicked()
{
	KviKvsVariantList *params=0;
	callFunction(this,"clickEvent",params);
}

#include "m_class_toolbutton.moc"

