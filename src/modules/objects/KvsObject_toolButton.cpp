//=============================================================================
//
//   File : KvsObject_toolButton.cpp
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

#include "KvsObject_toolButton.h"
#include "KviError.h"
#include "kvi_debug.h"

#include "KviLocale.h"
#include "KviIconManager.h"

#include <QMenu>

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
		The toolbutton class provides a quick-access button to commands or options, usually used inside a toolBar.
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
		Sets the label of this button to <text> and automatically sets it as a tooltip if <tip> is true.
		!fn:<string> $textLabel()
		Returns the label of the button.
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
		!fn:$setPopup (<popupmenu:object>)
		Associates the given <popupmenu> with this tool button.
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

KVSO_BEGIN_REGISTERCLASS(KvsObject_toolButton, "toolbutton", "widget")
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_toolButton, setImage)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_toolButton, setUsesBigPixmap)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_toolButton, usesBigPixmap)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_toolButton, setUsesTextLabel)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_toolButton, usesTextLabel)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_toolButton, setAutoRaise)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_toolButton, autoRaise)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_toolButton, setTextLabel)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_toolButton, textLabel)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_toolButton, setPopup)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_toolButton, openPopup)

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_toolButton, setToggleButton)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_toolButton, toggle)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_toolButton, setOn)

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_toolButton, setTextPosition)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_toolButton, textPosition)

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_toolButton, clickEvent)
KVSO_END_REGISTERCLASS(KvsObject_toolButton)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_toolButton, KvsObject_widget)

KVSO_END_CONSTRUCTOR(KvsObject_toolButton)

KVSO_BEGIN_DESTRUCTOR(KvsObject_toolButton)

KVSO_END_CONSTRUCTOR(KvsObject_toolButton)

bool KvsObject_toolButton::init(KviKvsRunTimeContext *, KviKvsVariantList *)
{
	SET_OBJECT(QToolButton);
	connect(widget(), SIGNAL(clicked()), this, SLOT(slotClicked()));
	return true;
}

KVSO_CLASS_FUNCTION(toolButton, setImage)
{
	CHECK_INTERNAL_POINTER(widget())
	QString icon;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("icon_id", KVS_PT_STRING, 0, icon)
	KVSO_PARAMETERS_END(c)
	QPixmap * pix = g_pIconManager->getImage(icon);
	if(pix)
		((QToolButton *)widget())->setIcon(QIcon(*pix));
	else
		((QToolButton *)widget())->setIcon(QIcon());
	return true;
}

KVSO_CLASS_FUNCTION(toolButton, setUsesBigPixmap)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bEnabled", KVS_PT_BOOL, 0, bEnabled)
	KVSO_PARAMETERS_END(c)
	((QToolButton *)widget())->setIconSize(bEnabled ? QSize(32, 32) : QSize(22, 22));
	return true;
}

KVSO_CLASS_FUNCTION(toolButton, usesBigPixmap)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setBoolean(((QToolButton *)widget())->iconSize().height() > 22);
	return true;
}

KVSO_CLASS_FUNCTION(toolButton, setUsesTextLabel)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bEnabled", KVS_PT_BOOL, 0, bEnabled)
	KVSO_PARAMETERS_END(c)
	((QToolButton *)widget())->setToolButtonStyle(bEnabled ? Qt::ToolButtonTextUnderIcon : Qt::ToolButtonIconOnly);
	return true;
}

KVSO_CLASS_FUNCTION(toolButton, usesTextLabel)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setBoolean(((QToolButton *)widget())->toolButtonStyle() != Qt::ToolButtonIconOnly);
	return true;
}

KVSO_CLASS_FUNCTION(toolButton, setAutoRaise)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bEnabled", KVS_PT_BOOL, 0, bEnabled)
	KVSO_PARAMETERS_END(c)
	((QToolButton *)widget())->setAutoRaise(bEnabled);
	return true;
}

KVSO_CLASS_FUNCTION(toolButton, autoRaise)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setBoolean(((QToolButton *)widget())->autoRaise());
	return true;
}

KVSO_CLASS_FUNCTION(toolButton, setOn)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bEnabled", KVS_PT_BOOL, 0, bEnabled)
	KVSO_PARAMETERS_END(c)
	((QToolButton *)widget())->setChecked(bEnabled);
	return true;
}

KVSO_CLASS_FUNCTION(toolButton, setToggleButton)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bEnabled", KVS_PT_BOOL, 0, bEnabled)
	KVSO_PARAMETERS_END(c)
	((QToolButton *)widget())->setCheckable(bEnabled);
	return true;
}

KVSO_CLASS_FUNCTION(toolButton, toggle)
{
	CHECK_INTERNAL_POINTER(widget())
	((QToolButton *)widget())->toggle();
	return true;
}

KVSO_CLASS_FUNCTION(toolButton, setTextLabel)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szLabel, szTip;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("label", KVS_PT_STRING, 0, szLabel)
	KVSO_PARAMETER("tooltip", KVS_PT_STRING, KVS_PF_OPTIONAL, szTip)
	KVSO_PARAMETERS_END(c)
	((QToolButton *)widget())->setText(szLabel);
	if(!szTip.isEmpty())
		((QToolButton *)widget())->setToolTip(szTip);
	return true;
}

KVSO_CLASS_FUNCTION(toolButton, textLabel)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setString(((QToolButton *)widget())->text());
	return true;
}

KVSO_CLASS_FUNCTION(toolButton, setPopup)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject * ob;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("widget", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETERS_END(c)
	ob = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!ob)
	{
		c->warning(__tr2qs_ctx("Widget parameter is not an object", "objects"));
		return true;
	}
	if(!ob->object())
	{
		c->warning(__tr2qs_ctx("Widget parameter is not a valid object", "objects"));
		return true;
	}
	if(!ob->inheritsClass("popupmenu"))
	{
		c->warning(__tr2qs_ctx("Can't add a non-popupmenu object", "objects"));
		return true;
	}
	((QToolButton *)widget())->setMenu(((QMenu *)(ob->object())));
	return true;
}

KVSO_CLASS_FUNCTION(toolButton, openPopup)
{
	CHECK_INTERNAL_POINTER(widget())
	((QToolButton *)widget())->showMenu();
	return true;
}

KVSO_CLASS_FUNCTION(toolButton, setTextPosition)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szPos;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("position", KVS_PT_STRING, 0, szPos)
	KVSO_PARAMETERS_END(c)
	if(KviQString::equalCI(szPos, "BesideIcon"))
		((QToolButton *)widget())->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	else if(KviQString::equalCI(szPos, "BelowIcon"))
		((QToolButton *)widget())->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	else
		c->warning(__tr2qs_ctx("Unknown text position '%Q'", "objects"), &szPos);
	return true;
}

KVSO_CLASS_FUNCTION(toolButton, textPosition)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szPos = "BelowIcon";
	if((((QToolButton *)widget())->toolButtonStyle()) == (Qt::ToolButtonTextBesideIcon))
		szPos = "BesideIcon";
	c->returnValue()->setString(szPos);
	return true;
}

KVSO_CLASS_FUNCTION(toolButton, clickEvent)
{
	emitSignal("clicked", c);
	return true;
}

void KvsObject_toolButton::slotClicked()
{
	KviKvsVariantList * params = nullptr;
	callFunction(this, "clickEvent", params);
}
