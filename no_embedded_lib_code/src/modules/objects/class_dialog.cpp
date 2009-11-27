//=============================================================================
//
//   File : class_dialog.cpp
//   Creation date : Sun Apr 10 22:51:48 CEST 2002 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2009 Szymon Stefanek (pragma at kvirc dot net)
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

#include "class_dialog.h"
#include "kvi_error.h"
#include "kvi_debug.h"
#include "kvi_locale.h"
#include "kvi_frame.h"
#include "kvi_app.h"

#include <QDialog>


/*
	@doc: dialog
	@title:
		dialog class
	@type:
		class
	@short:
		A toplevel dialog object
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		Rappresents a dialog object. The class is really
		similar to the widget class, it has only a couple of minor differences.
		A dialog is always a top-level widget, but if it has a parent, its default
		location is centered on top of the parent. It will also share the parent's windowlist entry.
                !fn; setModal (<boolean>)
		If you call $setModal(1) then the dialog will have non-blocking modal behaviour:
		it will appear above its parent widget and block its input until it's closed.
	@functions:
*/

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_dialog,"dialog","widget")
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_dialog,setModal)
KVSO_END_REGISTERCLASS(KviKvsObject_dialog)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_dialog,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_dialog)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_dialog)

KVSO_END_CONSTRUCTOR(KviKvsObject_dialog)

bool KviKvsObject_dialog::init(KviKvsRunTimeContext *,KviKvsVariantList *)
{
	QWidget * w = g_pApp->activeModalWidget();
	if(!w)w = g_pFrame;
	QDialog * d = new QDialog(parentScriptWidget() ? parentScriptWidget() : w);
	d->setObjectName(getName());
	//d->setModal(true);
	setObject(d);
	return true;
}
KVSO_CLASS_FUNCTION(dialog,setModal)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	((QDialog *)widget())->setModal(bEnabled);
	return true;
}


