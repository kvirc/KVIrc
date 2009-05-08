//=============================================================================
//
//   File : class_window.cpp
//   Creation date : Tue Now 26 21:39:59 CEST 2002 by Szymon Stefanek
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


#include "class_window.h"

#include "kvi_error.h"
#include "kvi_debug.h"

#include "kvi_locale.h"
#include "kvi_frame.h"



KviKvsScriptWindowWindow::KviKvsScriptWindowWindow(KviFrame * pParent,const QString &szName)
: KviWindow(KVI_WINDOW_TYPE_SCRIPTOBJECT,pParent,szName)
{
	m_pCentralWidget = 0;

}

KviKvsScriptWindowWindow::~KviKvsScriptWindowWindow()
{
}

void KviKvsScriptWindowWindow::setCentralWidget(KviKvsObject_widget *o,QWidget * w)
{
	m_pCentralWidgetObject = o;
	m_pCentralWidget = w;
}

void KviKvsScriptWindowWindow::resizeEvent(QResizeEvent *)
{
	if(m_pCentralWidget)
		m_pCentralWidget->setGeometry(0,0,width(),height());
}

void KviKvsScriptWindowWindow::centralWidgetObjectDestroyed()
{
	m_pCentralWidget = 0;
	m_pCentralWidgetObject = 0;
}

void KviKvsScriptWindowWindow::centralWidgetDestroyed()
{
	m_pCentralWidget = 0;
	m_pCentralWidgetObject = 0;
}




/*
	@doc:	window
	@title:
		window class
	@type:
		class
	@short:
		A MDI window object
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		This class rappresents a MDI window in a KVIrc frame.
		Regardless of the parent object passed, the window will be always
		a "physical" child of the frame window: you can't embed a window object
		inside a widget or another window. The parent object will just "own" this
		window object and eventually destroy it when it dies.
		This class can manage only a single central child widget that must
		be set with $setCentralWidget().
		The central widget MUST be set before you initially call show()
	@functions:
*/


KVSO_BEGIN_REGISTERCLASS(KviKvsObject_window,"window","widget")
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_window,setWindowTitle)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_window,setCentralWidget)
KVSO_END_REGISTERCLASS(KviKvsObject_window)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_window,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_window)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_window)

KVSO_END_CONSTRUCTOR(KviKvsObject_window)

bool KviKvsObject_window::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *)
{
	KviWindow * w = new KviKvsScriptWindowWindow(pContext->window()->frame(),getName());
	setObject(w);
	pContext->window()->frame()->addWindow(w,false);
	w->minimize(); // must be minimized before children are added, otherwise the focus handling goes nuts...

	return true;
}
KVSO_CLASS_FUNCTION(window,setWindowTitle)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szCaption;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("caption",KVS_PT_STRING,0,szCaption)
	KVSO_PARAMETERS_END(c)
	((KviKvsScriptWindowWindow *)widget())->setWindowTitleString(szCaption);
	return true;
}
KVSO_CLASS_FUNCTION(window,setCentralWidget)
{
	CHECK_INTERNAL_POINTER(widget())

	KviKvsObject *ob;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("widget",KVS_PT_HOBJECT,0,hObject)
	KVSO_PARAMETERS_END(c)
	ob=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!ob->object()->isWidgetType())
	{
		c->warning(__tr2qs_ctx("Can't add a non-widget object","objects"));
		return true;
	}
	if(!ob->inheritsClass("widget"))
	{
		c->warning(__tr2qs_ctx("Can't add a non-widget object","objects"));
		return true;
	}
	((KviKvsScriptWindowWindow *)widget())->setCentralWidget((KviKvsObject_widget *)ob,((KviKvsObject_widget *)ob)->widget());
	return true;
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_window.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

