//=============================================================================
//
//   File : class_toolbar.cpp
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

#include "class_toolbar.h"
#include "kvi_error.h"
#include "kvi_debug.h"
#include "kvi_tal_toolbar.h"
#include "kvi_tal_mainwindow.h"
#include "kvi_locale.h"
#include "kvi_iconmanager.h"
#include "class_mainwindow.h"

/*
	@doc:	toolbar
	@keyterms:
		toolbar object class,
	@title:
		toolbar class
	@type:
		class
	@short:
		Provides a toolbar for mainwindow widget.
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		The toolbar class provides a movable panel containing widgets such as tool buttons.
	@functions:
		!fn: $addSeparator()
		Adds a separator to the right/bottom of the toolbar.
		!fn: $setLabel(<text:string>)
		Sets the toolbar's label.
		!fn: <string> $label()
		Returns the toolbar's label.
		!fn: $clear()
		Deletes all the toolbar's child widgets.

*/


KVSO_BEGIN_REGISTERCLASS(KviKvsObject_toolbar,"toolbar","widget")
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_toolbar,addSeparator)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_toolbar,setLabel)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_toolbar,label)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_toolbar,clear)
KVSO_END_REGISTERCLASS(KviKvsObject_toolbar)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_toolbar,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_toolbar)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_toolbar)

KVSO_END_CONSTRUCTOR(KviKvsObject_toolbar)

bool KviKvsObject_toolbar::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *)
{
	if (!parentObject())
	{
		pContext->warning(__tr2qs_ctx("The toolbar cannot be a parent-widget!","objects"));
		return true;
	}
	if(parentObject()->inheritsClass("mainwindow"))
	{
		setObject(new KviTalToolBar(getName(), ((KviTalMainWindow *)parentScriptWidget())), true);
	}
	else
	{
		pContext->warning(__tr2qs_ctx("The parent-widget isn't a MainWindow.","objects"));
	}

	return true;
}

KVSO_CLASS_FUNCTION(toolbar,addSeparator)
{
	CHECK_INTERNAL_POINTER(widget())
	((KviTalToolBar *)widget())->addSeparator();
	return true;
}
KVSO_CLASS_FUNCTION(toolbar,setLabel)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szLabel;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("label",KVS_PT_STRING,0,szLabel)
	KVSO_PARAMETERS_END(c)
	((KviTalToolBar *)widget())->setWindowTitle(szLabel);
	return true;
}
KVSO_CLASS_FUNCTION(toolbar,label)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setString(((KviTalToolBar *)widget())->windowTitle());
	return true;
}
KVSO_CLASS_FUNCTION(toolbar,clear)
{
	CHECK_INTERNAL_POINTER(widget())
	((KviTalToolBar *)object())->clear();
	return true;
}



