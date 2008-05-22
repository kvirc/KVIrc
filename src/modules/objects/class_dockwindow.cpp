//=============================================================================
//
//   File : class_dockwindow.cpp
//   Created on Thu 29 Dec 2005 23:45:11 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2005 Szymon Stefanek <pragma at kvirc dot net>
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

#include "class_dockwindow.h"

#include "kvi_frame.h"
#include "kvi_locale.h"
#include "kvi_qstring.h"

#include <QDockWidget>
#define QT_DOCK_WINDOW QDockWidget
#include <QLayout>

/*
	@doc: dockwindow
	@title:
		dockwindow class
	@type:
		class
	@short:
		A window dockable to the KVIrc main frame
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		A window dockable to the KVIrc main frame borders (like a toolbar).
		The window has an implicit layout that will automatically manage
		the children depending on the dock window's orientation.
	@functions:
		!fn: $addWidget(<widget:hobject>)
			Adds <widget> to the internal layout of this dock window.[br]
			The widget must be a child of this dock window (otherwise strange things may happen).
		!fn: <string> $orientation()
			Returns the string "vertical" or "horizontal" depending on the orientation of this dock window.
		!fn: $setOrientation(<orientation:string>)
			Forces the orentation of this dock window. <ordientation> must be either "horizontal" or "vertical".[br]
			Please note that KVIrc sets the orientation automatically when it is moved between the main frame docks by the user.
		!fn: <bool> $resizeEnabled()
			Returns $true if resizing of this window is enabled and false otherwise.
		!fn: $setResizeEnabled(<bEnabled:bool>)
			Enabled or disabled resizing of this window.
		!fn: $setAllowedDockAreas(<docks:string>)
			Sets the allowed main window dock areas for this dock window.[br]
			<docks> must be a combination of "l","r","t","b","f" and "m".[br]
			"l" stands for left dock area, "r" stands for right dock area, "t" stands for the top dock areas, "b" stands for the bottom dock area, "f" stands for "floating" and "m" for "minimized".[br]
			If a flag is present then the related block area is enabled,otherwise it is disabled.
		!fn: $dock(<dockarea:string>)
			Docks this dock window to the specified dockarea of the main KVIrc window which can be one of "l" (left dock area), "t" (top dock area), "r" (right dock area), "b" (bottom dock area), "f" (floating) and "m" (minimized).
*/


KVSO_BEGIN_REGISTERCLASS(KviKvsObject_dockwindow,"dockwindow","widget")
	KVSO_REGISTER_HANDLER(KviKvsObject_dockwindow,"addWidget",function_addWidget)
	KVSO_REGISTER_HANDLER(KviKvsObject_dockwindow,"orientation",function_orientation)
	KVSO_REGISTER_HANDLER(KviKvsObject_dockwindow,"setOrientation",function_setOrientation)
	KVSO_REGISTER_HANDLER(KviKvsObject_dockwindow,"resizeEnabled",function_resizeEnabled)
	KVSO_REGISTER_HANDLER(KviKvsObject_dockwindow,"setResizeEnabled",function_setResizeEnabled)
	KVSO_REGISTER_HANDLER(KviKvsObject_dockwindow,"setAllowedDockAreas",function_setAllowedDockAreas)
	KVSO_REGISTER_HANDLER(KviKvsObject_dockwindow,"dock",function_dock)
KVSO_END_REGISTERCLASS(KviKvsObject_dockwindow)


KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_dockwindow,KviKvsObject_widget)
KVSO_END_CONSTRUCTOR(KviKvsObject_dockwindow)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_dockwindow)
KVSO_END_DESTRUCTOR(KviKvsObject_dockwindow)

#define _pDockWindow ((QT_DOCK_WINDOW *)widget())

bool KviKvsObject_dockwindow::init(KviKvsRunTimeContext * pContext,KviKvsVariantList * pParams)
{
	QDockWidget * pWidget = new QDockWidget(g_pFrame);
	pWidget->setObjectName(getName());
	setObject(pWidget);
	return true;
}

bool KviKvsObject_dockwindow::function_addWidget(KviKvsObjectFunctionCall *c)
{
	kvs_hobject_t hWidget;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("widget",KVS_PT_HOBJECT,0,hWidget)
	KVSO_PARAMETERS_END(c)

	if(!widget())return true; // hum ? dead ?

	if(hWidget == (kvs_hobject_t)0)
	{
		// null widget ?
		c->warning(__tr2qs("Can't add a null object"));
		return true;
	}

	KviKvsObject * pWidget = KviKvsKernel::instance()->objectController()->lookupObject(hWidget);
	if(!pWidget)
	{
		c->warning(__tr2qs("Invalid object handle passed as parameter (the object is no longer existing ?)"));
		return true;
	}

	if(!pWidget->object())
	{
		c->warning(__tr2qs("Object in invalid state"));
		return true;
	}

	if(!pWidget->object()->isWidgetType())
	{
		c->warning(__tr2qs("Can't set a non-widget object to be the main widget of a dock window"));
		return true;
	}

	if(((QWidget *)(pWidget->object()))->parent() != (QObject *)_pDockWindow)
	{
		c->warning(__tr2qs("The added widget is not a child of this dock window"));
	}

	_pDockWindow->setWidget((QWidget *)(pWidget->object()));
	return true;
}

bool KviKvsObject_dockwindow::function_orientation(KviKvsObjectFunctionCall * c)
{
	if(!widget())return true; // hum ? dead ?
	c->returnValue()->setString(QString("horizontal"));
	return true;
}

bool KviKvsObject_dockwindow::function_setOrientation(KviKvsObjectFunctionCall * c)
{
	QString szOrientation;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("orientation",KVS_PT_NONEMPTYSTRING,0,szOrientation)
	KVSO_PARAMETERS_END(c)

	if(!widget())return true; // hum ? dead ?
#ifndef COMPILE_USE_QT4
	_pDockWindow->setOrientation(KviQString::equalCI(szOrientation,"vertical") ? Qt::Vertical : Qt::Horizontal);
#endif //!COMPILE_USE_QT4
	return true;
}
// Fix me
bool KviKvsObject_dockwindow::function_resizeEnabled(KviKvsObjectFunctionCall * c)
{
	if(!widget())return true; // hum ? dead ?
	c->returnValue()->setBoolean(false);
	return true;
}

// Fix me
bool KviKvsObject_dockwindow::function_setResizeEnabled(KviKvsObjectFunctionCall * c)
{
	bool bResizeEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bResizeEnabled)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true; // hum ? dead ?
	return true;
}

bool KviKvsObject_dockwindow::function_setAllowedDockAreas(KviKvsObjectFunctionCall * c)
{
	QString szFlags;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("docks",KVS_PT_STRING,0,szFlags)
	KVSO_PARAMETERS_END(c)

	if(!widget())return true; // hum ? dead ?

	Qt::DockWidgetAreas fAreas = Qt::NoDockWidgetArea;
	if(szFlags.indexOf('t',Qt::CaseInsensitive))fAreas |= Qt::TopDockWidgetArea;
	if(szFlags.indexOf('l',Qt::CaseInsensitive))fAreas |= Qt::LeftDockWidgetArea;
	if(szFlags.indexOf('r',Qt::CaseInsensitive))fAreas |= Qt::RightDockWidgetArea;
	if(szFlags.indexOf('b',Qt::CaseInsensitive))fAreas |= Qt::BottomDockWidgetArea;
	_pDockWindow->setAllowedAreas(fAreas);
	QDockWidget::DockWidgetFeatures fFeatures = _pDockWindow->features();
	if(szFlags.indexOf('f',Qt::CaseInsensitive))
		fFeatures |= QDockWidget::DockWidgetFloatable;
	else
		fFeatures &= ~QDockWidget::DockWidgetFloatable;
	// no support for minimized dock widgets
	_pDockWindow->setFeatures(fFeatures);


	return true;
}


bool KviKvsObject_dockwindow::function_dock(KviKvsObjectFunctionCall * c)
{
	QString szDock;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("dock",KVS_PT_STRING,0,szDock)
	KVSO_PARAMETERS_END(c)

	if(!widget())return true; // hum ? dead ?
	g_pFrame->removeDockWidget(_pDockWindow);
	if(szDock.indexOf('m',Qt::CaseInsensitive) == -1)_pDockWindow->setFloating(false);
	if(szDock.indexOf('t',Qt::CaseInsensitive) != -1)g_pFrame->addDockWidget(Qt::TopDockWidgetArea,_pDockWindow);
	else if(szDock.indexOf('l',Qt::CaseInsensitive) != -1)g_pFrame->addDockWidget(Qt::LeftDockWidgetArea,_pDockWindow);
	else if(szDock.indexOf('r',Qt::CaseInsensitive) != -1)g_pFrame->addDockWidget(Qt::RightDockWidgetArea,_pDockWindow);
	else if(szDock.indexOf('b',Qt::CaseInsensitive) != -1)g_pFrame->addDockWidget(Qt::BottomDockWidgetArea,_pDockWindow);
	else if(szDock.indexOf('f',Qt::CaseInsensitive) != -1)_pDockWindow->setFloating(true);
	else if(szDock.indexOf('m',Qt::CaseInsensitive) != -1)qDebug("Sorry: no support for minimized dock widgets in Qt4");
	else c->warning(__tr2qs("Invalid dock area specified"));
	return true;
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_dockwindow.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

