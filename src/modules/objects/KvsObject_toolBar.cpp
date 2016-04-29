//=============================================================================
//
//   File : KvsObject_toolBar.cpp
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

#include "KvsObject_toolBar.h"
#include "KviError.h"
#include "kvi_debug.h"
#include "KviTalMainWindow.h"
#include "KviLocale.h"
#include "KviIconManager.h"
#include "KvsObject_mainWindow.h"

#include <QToolBar>

/*
	@doc:	toolbar
	@keyterms:
		toolbar object class,
	@title:
		toolbar class
	@type:
		class
	@short:
		Provides a toolbar for widget.
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		The toolbar class provides a movable panel containing widgets such as tool buttons.
	@functions:
		!fn: $addSeparator()
		Adds a separator to the right/bottom of the toolbar.
		!fn: $setLabel(<text:string>)
		Sets the toolbars label.
		!fn: <string> $label()
		Returns the toolbars label.
		!fn: $clear()
		Deletes all the toolbars child widgets.
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_toolBar, "toolbar", "widget")
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_toolBar, addSeparator)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_toolBar, setLabel)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_toolBar, label)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_toolBar, clear)
KVSO_END_REGISTERCLASS(KvsObject_toolBar)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_toolBar, KvsObject_widget)

KVSO_END_CONSTRUCTOR(KvsObject_toolBar)

KVSO_BEGIN_DESTRUCTOR(KvsObject_toolBar)

KVSO_END_CONSTRUCTOR(KvsObject_toolBar)

bool KvsObject_toolBar::init(KviKvsRunTimeContext *, KviKvsVariantList *)
{
	SET_OBJECT(QToolBar)
	return true;
}

KVSO_CLASS_FUNCTION(toolBar, addSeparator)
{
	CHECK_INTERNAL_POINTER(widget())
	((QToolBar *)widget())->addSeparator();
	return true;
}
KVSO_CLASS_FUNCTION(toolBar, setLabel)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szLabel;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("label", KVS_PT_STRING, 0, szLabel)
	KVSO_PARAMETERS_END(c)
	((QToolBar *)widget())->setWindowTitle(szLabel);
	return true;
}
KVSO_CLASS_FUNCTION(toolBar, label)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setString(((QToolBar *)widget())->windowTitle());
	return true;
}
KVSO_CLASS_FUNCTION(toolBar, clear)
{
	CHECK_INTERNAL_POINTER(widget())
	((QToolBar *)object())->clear();
	return true;
}
