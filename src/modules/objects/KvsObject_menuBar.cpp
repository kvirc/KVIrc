//=============================================================================
//
//   File : KvsObject_menuBar.cpp
//   Creation date : Tue Now 26 13:16:59 CEST 2002 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KvsObject_menuBar.h"
#include "KviError.h"
#include "kvi_debug.h"
#include "KviLocale.h"

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

KVSO_BEGIN_REGISTERCLASS(KvsObject_menuBar, "menubar", "widget")
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_menuBar, insertItem)
KVSO_END_REGISTERCLASS(KvsObject_menuBar)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_menuBar, KvsObject_widget)

KVSO_END_CONSTRUCTOR(KvsObject_menuBar)

KVSO_BEGIN_DESTRUCTOR(KvsObject_menuBar)

KVSO_END_CONSTRUCTOR(KvsObject_menuBar)

bool KvsObject_menuBar::init(KviKvsRunTimeContext *, KviKvsVariantList *)
{
	SET_OBJECT(QMenuBar)
	return true;
}

KVSO_CLASS_FUNCTION(menuBar, insertItem)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szItem;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("item", KVS_PT_STRING, 0, szItem)
	KVSO_PARAMETERS_END(c)
	((QMenuBar *)widget())->addAction(szItem);
	return true;
}
