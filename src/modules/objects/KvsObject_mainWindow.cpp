//=============================================================================
//
//   File : KvsObject_mainWindow.cpp
//   Creation date : Mon Feb 28 14:21:48 CEST 2005
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

#include "KviTalMainWindow.h"
#include "KvsObject_mainWindow.h"
#include "KviError.h"
#include "kvi_debug.h"
#include "KviLocale.h"
#include "KviIconManager.h"

/*
	@doc:	mainwindow
	@keyterms:
		mainwindow object class,
	@title:
		mainwindow class
	@type:
		class
	@short:
		Provides a mainwindow.
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		The mainwindow class provides a main application window, with menubar, toolbars.
	@functions:
		!fn: $setCentralWidget(<widget:object>)
		Sets the central widget for the main window to <wid>.
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_mainWindow, "mainwindow", "widget")
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_mainWindow, setCentralWidget)

KVSO_END_REGISTERCLASS(KvsObject_mainWindow)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_mainWindow, KvsObject_widget)

KVSO_END_CONSTRUCTOR(KvsObject_mainWindow)

KVSO_BEGIN_DESTRUCTOR(KvsObject_mainWindow)

KVSO_END_CONSTRUCTOR(KvsObject_mainWindow)

bool KvsObject_mainWindow::init(KviKvsRunTimeContext *, KviKvsVariantList *)
{
	setObject(new KviTalMainWindow(parentScriptWidget(), getName().toUtf8().data()), true);
	return true;
}

KVSO_CLASS_FUNCTION(mainWindow, setCentralWidget)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject * pObject;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("widget", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETERS_END(c)
	pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!pObject)
	{
		c->warning(__tr2qs_ctx("Widget parameter is not an object", "objects"));
		return true;
	}
	if(!pObject->object())
	{
		c->warning(__tr2qs_ctx("Widget parameter is not a valid object", "objects"));
		return true;
	}
	if(!pObject->inheritsClass("widget"))
	{
		c->warning(__tr2qs_ctx("Widget object required", "objects"));
		return true;
	}
	((KviTalMainWindow *)widget())->setCentralWidget(((QWidget *)(pObject->object())));
	return true;
}
