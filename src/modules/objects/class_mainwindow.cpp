//mdm:
//   File : class_mainwindow.cpp
//   Creation date : Mon Feb 28 14:21:48 CEST 2005 
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

#include "kvi_tal_mainwindow.h"
#include "class_mainwindow.h"
#include "kvi_error.h"
#include "kvi_debug.h"
#include "kvi_locale.h"
#include "kvi_iconmanager.h"


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

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_mainwindow,"mainwindow","widget")
	KVSO_REGISTER_HANDLER(KviKvsObject_mainwindow,"setCentralWidget",functionsetCentralWidget)
	KVSO_REGISTER_HANDLER(KviKvsObject_mainwindow,"setDockEnabled",functionsetDockEnabled)
	KVSO_REGISTER_HANDLER(KviKvsObject_mainwindow,"isDockEnabled",functionisDockEnabled)

KVSO_END_REGISTERCLASS(KviKvsObject_mainwindow)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_mainwindow,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_mainwindow)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_mainwindow)

KVSO_END_CONSTRUCTOR(KviKvsObject_mainwindow)

bool KviKvsObject_mainwindow::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{
	setObject(new KviTalMainWindow(parentScriptWidget(), getName()), true);
	return true;
}

bool KviKvsObject_mainwindow::functionsetCentralWidget(KviKvsObjectFunctionCall *c)
{
	KviKvsObject * pObject;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("widget",KVS_PT_HOBJECT,0,hObject)
	KVSO_PARAMETERS_END(c)
	pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if (!pObject)
	{
		c->warning(__tr2qs("Widget parameter is not an object"));
		return true;
	}
	if (!pObject->object())
	{
		c->warning(__tr2qs("Widget parameter is not a valid object"));
		return true;
	}
	if(!pObject->object()->inherits("KviKvsObject_widget"))
    {
		c->warning(__tr2qs("Widget object required"));
        return TRUE;
    }
	if(widget()) ((KviTalMainWindow *)widget())->setCentralWidget(((QWidget  *)(pObject->object())));
	return true;	
}
//FIX ME
bool KviKvsObject_mainwindow::functionsetDockEnabled(KviKvsObjectFunctionCall *c)
{
	QString szDockarea;
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("dock area",KVS_PT_STRING,0,szDockarea)
		KVSO_PARAMETER("bool enabled",KVS_PT_BOOL,0,bFlag)
	KVSO_PARAMETERS_END(c)
	if(widget())
	{

	}
	return true;

}

//FIX ME
bool KviKvsObject_mainwindow::functionisDockEnabled(KviKvsObjectFunctionCall *c)
{
	
	QString szDockarea;
	bool bFlag = false;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("dock_area",KVS_PT_STRING,0,szDockarea)
	KVSO_PARAMETERS_END(c)
    if(!widget()) return true;
	c->returnValue()->setBoolean(bFlag);
	return true;		

}
