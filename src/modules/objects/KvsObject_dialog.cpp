//=============================================================================
//
//   File : KvsObject_dialog.cpp
//   Creation date : Sun Apr 10 22:51:48 CEST 2002 by Szymon Stefanek
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

#include "KvsObject_dialog.h"
#include "KviError.h"
#include "kvi_debug.h"
#include "KviLocale.h"
#include "KviMainWindow.h"
#include "KviApplication.h"

#include <QDialog>

/*
	@doc: dialog
	@title:
		dialog class
	@type:
		class
	@short:
		A top-level dialog object class.
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		Represents a dialog object. The class is really
		similar to the widget class, it has only a couple of minor differences.
		A dialog is always a top-level widget, but if it has a parent, its default
		location is centered on top of the parent. It will also share the parent's window list entry.
		!fn; setModal (<boolean>)
		If you call $setModal(1) then the dialog will have non-blocking modal behaviour:
		it will appear above its parent widget and block its input until it's closed.
	@functions:
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_dialog, "dialog", "widget")
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_dialog, setModal)
KVSO_END_REGISTERCLASS(KvsObject_dialog)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_dialog, KvsObject_widget)

KVSO_END_CONSTRUCTOR(KvsObject_dialog)

KVSO_BEGIN_DESTRUCTOR(KvsObject_dialog)

KVSO_END_CONSTRUCTOR(KvsObject_dialog)

bool KvsObject_dialog::init(KviKvsRunTimeContext *, KviKvsVariantList *)
{
	QWidget * w = g_pApp->activeModalWidget();
	if(!w)
		w = g_pMainWindow;
	QDialog * d = new QDialog(parentScriptWidget() ? parentScriptWidget() : w);
	d->setObjectName(getName());
	//d->setModal(true);
	setObject(d);
	return true;
}
KVSO_CLASS_FUNCTION(dialog, setModal)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bEnabled", KVS_PT_BOOL, 0, bEnabled)
	KVSO_PARAMETERS_END(c)
	((QDialog *)widget())->setModal(bEnabled);
	return true;
}
