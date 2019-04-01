//=============================================================================
//
//   File : KvsObject_window.cpp
//   Creation date : Tue Now 26 21:39:59 CEST 2002 by Szymon Stefanek
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

#include "KvsObject_window.h"

#include "KviError.h"
#include "kvi_debug.h"

#include "KviIconManager.h"
#include "KviLocale.h"
#include "KviMainWindow.h"

KviKvsScriptWindowWindow::KviKvsScriptWindowWindow(const QString & szName)
    : KviWindow(KviWindow::ScriptObject, szName)
{
}

KviKvsScriptWindowWindow::~KviKvsScriptWindowWindow()
    = default;

void KviKvsScriptWindowWindow::setCentralWidget(KvsObject_widget * o, QWidget * w)
{
	m_pCentralWidgetObject = o;
	m_pCentralWidget = w;
}

void KviKvsScriptWindowWindow::resizeEvent(QResizeEvent *)
{
	if(m_pCentralWidget)
		m_pCentralWidget->setGeometry(0, 0, width(), height());
}

void KviKvsScriptWindowWindow::centralWidgetObjectDestroyed()
{
	m_pCentralWidget = nullptr;
	m_pCentralWidgetObject = nullptr;
}

void KviKvsScriptWindowWindow::centralWidgetDestroyed()
{
	m_pCentralWidget = nullptr;
	m_pCentralWidgetObject = nullptr;
}

QPixmap * KviKvsScriptWindowWindow::myIconPtr()
{
	if(m_pIcon)
		return m_pIcon;
	else
		return g_pIconManager->getSmallIcon(KviIconManager::DefaultIcon);
}

/*
	@doc:	window
	@title:
		window class
	@type:
		class
	@short:
		A window object
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		This class represents a window in a KVIrc frame.
		Regardless of the parent object passed, the window will be always
		a [i]physical[/i] child of the frame window: you can't embed a window object
		inside a widget or another window. The parent object will just [i]own[/i] this
		window object and eventually destroy it when it dies.
		This class can manage only a single central child widget that must
		be set with $setCentralWidget().
		The central widget [b]must[/b] be set before you initially call show()
	@functions:
		!fn: $setCentralWidget(<widget:object>)
		Sets the central widget for the window to <wid>.
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_window, "window", "widget")
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_window, setWindowTitle)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_window, setIcon)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_window, setCentralWidget)
KVSO_END_REGISTERCLASS(KvsObject_window)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_window, KvsObject_widget)

KVSO_END_CONSTRUCTOR(KvsObject_window)

KVSO_BEGIN_DESTRUCTOR(KvsObject_window)
if(widget())
	g_pMainWindow->closeWindow(((KviKvsScriptWindowWindow *)widget()));
KVSO_END_CONSTRUCTOR(KvsObject_window)

bool KvsObject_window::init(KviKvsRunTimeContext *, KviKvsVariantList *)
{
	KviKvsScriptWindowWindow * w = new KviKvsScriptWindowWindow(getName());
	setObject(w);
	g_pMainWindow->addWindow(w, false);
	//w->minimize(); // must be minimized before children are added, otherwise the focus handling goes nuts...

	return true;
}
KVSO_CLASS_FUNCTION(window, setWindowTitle)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szCaption;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("caption", KVS_PT_STRING, 0, szCaption)
	KVSO_PARAMETERS_END(c)
	((KviKvsScriptWindowWindow *)widget())->setWindowTitleString(szCaption);
	return true;
}

KVSO_CLASS_FUNCTION(window, setIcon)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szIcon;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("icon", KVS_PT_STRING, 0, szIcon)
	KVSO_PARAMETERS_END(c)
	QPixmap * pix = g_pIconManager->getImage(szIcon);
	if(pix)
		((KviKvsScriptWindowWindow *)widget())->setIcon(pix);
	return true;
}

KVSO_CLASS_FUNCTION(window, setCentralWidget)
{
	CHECK_INTERNAL_POINTER(widget())

	KviKvsObject * ob;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("widget", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETERS_END(c)
	ob = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!ob->object()->isWidgetType())
	{
		c->warning(__tr2qs_ctx("Can't add a non-widget object", "objects"));
		return true;
	}
	if(!ob->inheritsClass("widget"))
	{
		c->warning(__tr2qs_ctx("Can't add a non-widget object", "objects"));
		return true;
	}
	((KviKvsScriptWindowWindow *)widget())->setCentralWidget((KvsObject_widget *)ob, ((KvsObject_widget *)ob)->widget());
	return true;
}
