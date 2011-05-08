//=============================================================================
//
//   File : KvsObject_wrapper.cpp
//   Creation date : Fri Mar 18 14:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVIrc irc client distribution
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "KvsObject_wrapper.h"
#include "KviError.h"
#include "kvi_debug.h"

#include "KviLocale.h"
#include "KviIconManager.h"

#include "KvsObject_widget.h"
#include <qwidget.h>
#include "KviApplication.h"
#include "KviMainWindow.h"


/*
	@doc: wrapper
	@keyterms:
		wrapper object class,
	@title:
		wrapper class
	@type:
		class
	@short:
		Provides a wrapper class that hooks to an existing Qt graphic object modifying it.
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		This is a class for advanced KVIrc scripting.[br]
		It can wrap any existing KVIrc widget.[br]
		This class allows some unbelievable changes to the whole graphic environment of the kvirc,[br]
		in fact, could hook every widget, you can modify the property or you can be inserted other widget as child of this...[br]
		in short you have absolute liberty. [br]
		The power of this class is fantastic, because it allows the change at "run time" of almost all the elements of the Kvirc.[br]
		But to use this class in optimal way and to exploit its power, you have to know the Qt.... so you won't have limits [br]
		The KVIrc Qt widgets are arranged in trees (just as the objects).[br]
		The difference is that there are more toplevel widgets (and so more than one tree).[br]
		You can use [fnc]$objects.dump()[/fnc] to take a look at the KVIrc Qt objects tree.[br]
		Here is a part of the tree:[br][br]
		Ptr 14332520: top level object: kvirc_frame, class KviMainWindow, visible, rect = -4, -4, 1024, 708
		Ptr 17296024: object: qt_top_dock, class QDockArea
		Ptr 14882136: object: qt_custom_toolbar, class KviCustomToolBar
		Ptr 18143368: object: qt_dockwidget_internal, class QDockWindowTitleBar
		[br][br]
		Every listed widget has a "name", a "class" and a set of properties.[br]
		The first entry in this example is a KVIrc server window, class "KviMainWindow" and name "kvirc_frame":
		it is a toplevel widget.[br]
		The "qt_top_dock", "qt_custom_toolbar" and the "qt_dockwidget_internal" are
		direct children of that widget.[br]
		To indicate a specific widget we will use the "class::name" form.[br]
		So to indicate the main KVIrc frame you will use "KviMainWindow::kvirc_main_frame".
		Look at this example:[br]
		%A=$new(wrapper,0,test,KviMainWindow::kvirc_frame,KviStatusBar::unnamed)[br]
		%A->$setBackGroundColor(FFFFFF)[br]
		For first thing we create an object type wrapper,then we flow the tree that there from the command /object.dump and we will have:[br]
		.  [br]
		.  [br]
		Ptr 14196288: top level object: kvirc_frame, class KviMainWindow, visible, rect = -4, -4, 1024, 712  [br]
.                  [br]
		Ptr 17197360: object: unnamed, class KviStatusBar  [br]
		.  [br]
		.  [br]
		in this way we can follow the order, father->child from the top-level widget(KviMainWindow::kvirc_frame)[br]
		to reach the child that interests us (KviStatusBar::unnamed)[br]
		Then, following the syntax we will have:
		%A=$new(wrapper,0,test,KviMainWindow::kvirc_frame,KviStatusBar::unnamed)
		Now %A. will be point to the wrapped object, therefore we could modify its property or to consider it as if it were an object created by us in scripting. [br]
		Obviously, deleting the created object (for example %A) you don't will delete the object of Kvirc (in this case the statusbar). [br]
		Another example could be this:[br]
		%A=$new(wrapper,0,test,KviMainWindow::kvirc_frame,QToolButton::kvirc.identityoptions)[br]
		%A->$setProperty(usesBigPixmap,0)[br]
		In this fool example with the function $setProperty, we has setted the property usesBigPixmap to False, making to become the small icons of the ToolBar.[br]
		[br]
		The wrapping object search can optionally start with a window
		identifier with the following syntax: WinId::<window_id>.

*/



KVSO_BEGIN_REGISTERCLASS(KvsObject_wrapper,"wrapper","widget")

KVSO_END_REGISTERCLASS(KvsObject_wrapper)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_wrapper,KvsObject_widget)

KVSO_END_CONSTRUCTOR(KvsObject_wrapper)


KVSO_BEGIN_DESTRUCTOR(KvsObject_wrapper)

KVSO_END_CONSTRUCTOR(KvsObject_wrapper)

bool KvsObject_wrapper::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{
	if( !pParams ) return false;
	QWidget *pWidget = 0;
	unsigned int i=0;
	while(i!=pParams->count())
	{
		QString szClass;
		QString szName;
		QString s=0;
		pParams->at(i)->asString(s);
		if (!s.isEmpty())
		{
			int idx = s.indexOf("::");
			if( idx != -1 )
			{
				szClass = s.left(idx);
				szName  = s.right(s.length() - idx - 2);
			} else {
				szClass = s;
				szName  = "";
			}
			if(KviQString::equalCI(szClass,"WinId"))
			{
				if(pWidget)
				{
					pContext->warning(__tr2qs_ctx("The window identifier preceded by WinId must be the first object in the search path","objects"));
					return false;
				} else {
					pWidget = g_pApp->findWindow(szName);
				}
			} else {
				if(pWidget)
				{
					pWidget = findWidgetToWrap(!szClass.isEmpty() ? szClass.toUtf8().data() : "", !szName.isEmpty() ? szName.toUtf8().data() : "", pWidget);
				} else {
					pWidget = findTopLevelWidgetToWrap(!szClass.isEmpty() ? szClass.toUtf8().data() : "", !szName.isEmpty() ? szName.toUtf8().data() : "");
				}
			}
			if( !pWidget )
			{
				pContext->warning(__tr2qs_ctx("Failed to find one of the wrap path widgets ('%Q::%Q')","objects"),&szClass,&szName);
				return false;
			}
		}
		i++;
	}
	if( !pWidget )
	{
		pContext->warning(__tr2qs_ctx("Failed to find the widget to wrap","objects"));
		return false;
	}
	setObject(pWidget,false);
	return true;
}

QWidget *KvsObject_wrapper::findTopLevelWidgetToWrap(const QString szClass, const QString szName)
{
	QWidgetList list = g_pApp->topLevelWidgets();
	if( !list.count() ) return 0;
	for(int idx=0;idx<list.count();idx++)
	{
		bool bNameMatch  = false;
		bool bClassMatch = false;
		if( !szName.isEmpty() )
			bNameMatch = KviQString::equalCI(list.at(idx)->objectName(), szName);
		else
			bNameMatch = true;
		if( !szClass.isEmpty())
			bClassMatch = KviQString::equalCI(list.at(idx)->metaObject()->className(), szClass);
		else
			bClassMatch = true;
		if( bNameMatch && bClassMatch ) {
			QWidget *w = list.at(idx);
			return w;
		}
	}
	return 0;
}

QWidget *KvsObject_wrapper::findWidgetToWrap(const QString szClass, const QString szName, QWidget *childOf)
{
	QList<QObject *> list=childOf->children();
	if( !list.count() ) return 0;
	for(int idx=0;idx<list.count();idx++)
	{
		if( list.at(idx)->isWidgetType() )
		{
			QWidget *w = (QWidget *)list.at(idx);
			if (KviQString::equalCI(w->metaObject()->className(),szClass) &&KviQString::equalCI(w->objectName(),szName))return w;
		}
	}
	return 0;
}
