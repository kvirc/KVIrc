//=============================================================================
//
//   File : class_menubar.cpp
//   Creation date : Tue Now 26 13:16:59 CEST 2002 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
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


#include "class_menubar.h"
#include "kvi_error.h"
#include "kvi_debug.h"
#include "kvi_locale.h"

/*
	@doc:	menubar
	@title:
		menubar class
	@type:
		class
	@short:
		A menu bar object implementation
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:

	@functions:
		!fn: <integer> $insertItem(<text:string>)
		Returns an integer that identifies the inserted item.
*/

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_menubar,"menubar","widget")
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_menubar,insertItem)
KVSO_END_REGISTERCLASS(KviKvsObject_menubar)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_menubar,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_menubar)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_menubar)

KVSO_END_CONSTRUCTOR(KviKvsObject_menubar)

bool KviKvsObject_menubar::init(KviKvsRunTimeContext *,KviKvsVariantList *)
{
	SET_OBJECT(QMenuBar)
	return true;
}

KVSO_CLASS_FUNCTION(menubar,insertItem)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szItem;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("item",KVS_PT_STRING,0,szItem)
	KVSO_PARAMETERS_END(c)
	((QMenuBar *)widget())->addAction(szItem);
	return true;
}
