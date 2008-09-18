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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
		!fn: $setStretchableWidget(<widget:object>)
		Sets the <widget> to be expanded.
		!fn: $clear()
		Deletes all the toolbar's child widgets.

*/


KVSO_BEGIN_REGISTERCLASS(KviKvsObject_toolbar,"toolbar","object")
	KVSO_REGISTER_HANDLER(KviKvsObject_toolbar,"addSeparator", functionaddSeparator)
	KVSO_REGISTER_HANDLER(KviKvsObject_toolbar,"setLabel", functionsetLabel)
	KVSO_REGISTER_HANDLER(KviKvsObject_toolbar,"label", functionlabel)
	KVSO_REGISTER_HANDLER(KviKvsObject_toolbar,"setStretchableWidget", functionsetStretchableWidget)
	KVSO_REGISTER_HANDLER(KviKvsObject_toolbar,"clear", functionclear)
KVSO_END_REGISTERCLASS(KviKvsObject_toolbar)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_toolbar,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_toolbar)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_toolbar)

KVSO_END_CONSTRUCTOR(KviKvsObject_toolbar)

bool KviKvsObject_toolbar::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{
	if (!parentObject())
	{	
		pContext->warning(__tr2qs("the toolbar cannot be a parent-widget!"));
		return true;
	}
	if(parentObject()->inherits("KviKvsObject_mainwindow"))
	{ 
		setObject(new KviTalToolBar(getName(), ((KviTalMainWindow *)parentScriptWidget())), true);
	}
	else
	{
		pContext->warning(__tr2qs("The parent-widget isn't a MainWindow."));
	}
	
	return true;
}

bool KviKvsObject_toolbar::functionaddSeparator(KviKvsObjectFunctionCall *c)
{
	if(widget()) ((KviTalToolBar *)widget())->addSeparator();
	return true;
}
bool KviKvsObject_toolbar::functionsetLabel(KviKvsObjectFunctionCall *c)
{
	QString szLabel;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("label",KVS_PT_STRING,0,szLabel)
	KVSO_PARAMETERS_END(c)
	if (widget()) ((KviTalToolBar *)widget())->setWindowTitle(szLabel);
	return true;
}
bool KviKvsObject_toolbar::functionlabel(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setString(((KviTalToolBar *)widget())->windowTitle());
	return true;
}
//FIX ME
bool KviKvsObject_toolbar::functionsetStretchableWidget(KviKvsObjectFunctionCall *c)
{
	KviKvsObject *ob;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("widget",KVS_PT_HOBJECT,0,hObject)
	KVSO_PARAMETERS_END(c)
	ob=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if (!widget()) return true;
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
	if(!ob->object()->inherits("KviKvsObject_widget"))
    {
		c->warning(__tr2qs("Widget object required"));
        return TRUE;
    }
#ifndef COMPILE_USE_QT4
	// FIXME: no support for this in Qt4 ?
	if(widget()) ((KviTalToolBar *)widget())->setStretchableWidget(((QWidget  *)(ob->object())));
#endif //!COMPILE_USE_QT4
	return true;	
}
bool KviKvsObject_toolbar::functionclear(KviKvsObjectFunctionCall *c)
{
	if (widget())
		((KviTalToolBar *)object())->clear();
	return true;
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_toolbar.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES


