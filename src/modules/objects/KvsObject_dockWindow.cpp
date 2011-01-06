//=============================================================================
//
//   File : KvsObject_dockWindow.cpp
//   Creation date : Thu 29 Dec 2005 23:45:11 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KvsObject_dockWindow.h"

#include "KviMainWindow.h"
#include "KviLocale.h"
#include "KviQString.h"

#include <QDockWidget>
#define QT_DOCK_WINDOW QDockWidget
#include <QLayout>

/*
        @doc:   dockwindow
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
	@functions:
		!fn: $addWidget(<widget:hobject>)
                Adds <widget> to the internal layout of this dock window.[br]
                The widget must be a child of this dock window (otherwise strange things may happen).
                !fn: $setAllowedDockAreas(<docks:string>)
                Sets the allowed main window dock areas for this dock window.[br]
                <docks> must be a combination of "l","r","t","b","f" and "m".[br]
                "l" stands for left dock area, "r" stands for right dock area, "t" stands for the top dock areas, "b" stands for the bottom dock area, "f" stands for "floating" and "m" for "minimized".[br]
                If a flag is present then the related block area is enabled,otherwise it is disabled.
		!fn: $dock(<dockarea:string>)
                Docks this dock window to the specified dockarea of the main KVIrc window which can be one of "l" (left dock area), "t" (top dock area), "r" (right dock area), "b" (bottom dock area), "f" (floating) and "m" (minimized).
*/


KVSO_BEGIN_REGISTERCLASS(KvsObject_dockWindow,"dockwindow","widget")
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_dockWindow,addWidget)
        KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_dockWindow,setAllowedDockAreas)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_dockWindow,dock)
KVSO_END_REGISTERCLASS(KvsObject_dockWindow)


KVSO_BEGIN_CONSTRUCTOR(KvsObject_dockWindow,KvsObject_widget)
KVSO_END_CONSTRUCTOR(KvsObject_dockWindow)


KVSO_BEGIN_DESTRUCTOR(KvsObject_dockWindow)
KVSO_END_DESTRUCTOR(KvsObject_dockWindow)

#define _pDockWindow ((QDockWidget *)widget())

bool KvsObject_dockWindow::init(KviKvsRunTimeContext *,KviKvsVariantList *)
{
	QDockWidget * pWidget = new QDockWidget(g_pMainWindow);
	pWidget->setObjectName(getName());
	setObject(pWidget);
	return true;
}

KVSO_CLASS_FUNCTION(dockWindow,addWidget)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_hobject_t hWidget;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("widget",KVS_PT_HOBJECT,0,hWidget)
	KVSO_PARAMETERS_END(c)
	if(hWidget == (kvs_hobject_t)0)
	{
		// null widget ?
		c->warning(__tr2qs_ctx("Can't add a null object","objects"));
		return true;
	}

	KviKvsObject * pWidget = KviKvsKernel::instance()->objectController()->lookupObject(hWidget);
	if(!pWidget)
	{
		c->warning(__tr2qs_ctx("Invalid object handle passed as parameter (the object is no longer existing ?)","objects"));
		return true;
	}

	if(!pWidget->object())
	{
		c->warning(__tr2qs_ctx("Object in invalid state","objects"));
		return true;
	}

	if(!pWidget->object()->isWidgetType())
	{
		c->warning(__tr2qs_ctx("Can't set a non-widget object to be the main widget of a dock window","objects"));
		return true;
	}

	if(((QWidget *)(pWidget->object()))->parent() != (QObject *)_pDockWindow)
	{
		c->warning(__tr2qs_ctx("The added widget is not a child of this dock window","objects"));
	}

	_pDockWindow->setWidget((QWidget *)(pWidget->object()));
	return true;
}


KVSO_CLASS_FUNCTION(dockWindow,setAllowedDockAreas)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szFlags;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("docks",KVS_PT_STRING,0,szFlags)
	KVSO_PARAMETERS_END(c)
	Qt::DockWidgetAreas fAreas = Qt::NoDockWidgetArea;
	if(szFlags.indexOf('t',Qt::CaseInsensitive) >= 0)
		fAreas |= Qt::TopDockWidgetArea;
	if(szFlags.indexOf('l',Qt::CaseInsensitive) >= 0)
		fAreas |= Qt::LeftDockWidgetArea;
	if(szFlags.indexOf('r',Qt::CaseInsensitive) >= 0)
		fAreas |= Qt::RightDockWidgetArea;
	if(szFlags.indexOf('b',Qt::CaseInsensitive) >= 0)
		fAreas |= Qt::BottomDockWidgetArea;
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


KVSO_CLASS_FUNCTION(dockWindow,dock)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szDock;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("dock",KVS_PT_STRING,0,szDock)
	KVSO_PARAMETERS_END(c)
	g_pMainWindow->removeDockWidget(_pDockWindow);
	if(szDock.indexOf('m',Qt::CaseInsensitive) == -1)_pDockWindow->setFloating(false);
	if(szDock.indexOf('t',Qt::CaseInsensitive) != -1)g_pMainWindow->addDockWidget(Qt::TopDockWidgetArea,_pDockWindow);
	else if(szDock.indexOf('l',Qt::CaseInsensitive) != -1)g_pMainWindow->addDockWidget(Qt::LeftDockWidgetArea,_pDockWindow);
	else if(szDock.indexOf('r',Qt::CaseInsensitive) != -1)g_pMainWindow->addDockWidget(Qt::RightDockWidgetArea,_pDockWindow);
	else if(szDock.indexOf('b',Qt::CaseInsensitive) != -1)g_pMainWindow->addDockWidget(Qt::BottomDockWidgetArea,_pDockWindow);
	else if(szDock.indexOf('f',Qt::CaseInsensitive) != -1)_pDockWindow->setFloating(true);
	else if(szDock.indexOf('m',Qt::CaseInsensitive) != -1)qDebug("Sorry: no support for minimized dock widgets in Qt4");
	else c->warning(__tr2qs_ctx("Invalid dock area specified","objects"));
	return true;
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_KvsObject_dockWindow.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

