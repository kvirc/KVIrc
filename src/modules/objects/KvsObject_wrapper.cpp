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
		A wrapper for the existing Qt widgets.
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		[p]
		This class "wraps" existing kvirc widgets and allows using the [class]widget[/class]
		class API to manipulate them. You can use it, for example, to set the geometry of the
		kvirc main window or to apply some crazy graphical changes to the UI.
		[/p]
		[p]
		The KVIrc Qt widgets are arranged in trees (just as the objects). The difference is that there can be more than one
		toplevel widget and so more than one tree. You can use [fnc]$objects.dump()[/fnc] to take a look at the KVIrc Qt objects tree.
		[/p]
		[p]
		Here is a part of the tree:
		[/p]
		[example]
		Ptr 23786128: top level object: kvirc_frame, class KviMainWindow, visible, rect = 1678, -3, 1680, 1030
		>Ptr 23496976: object: qt_rubberband, class QRubberBand
		>Ptr 23536608: object: main_frame_splitter, class QSplitter
		>>Ptr 23795232: object: mdi_manager, class KviMdiManager
		>>>Ptr 23863200: object: qt_scrollarea_hcontainer, class QWidget
		>>>>Ptr 23418224: object: , class QScrollBar
		>>>Ptr 23864832: object: qt_scrollarea_vcontainer, class QWidget
		>>>>Ptr 22383424: object: , class QScrollBar
		>>Ptr 25750832: object: mdi_manager, class KviMdiManager
		>>>Ptr 26112928: object: , class QWidget
		>>>>Ptr 45381568: object: , class Oxygen::MdiWindowShadow
		>>>>Ptr 45952496: object: mdi_child_Azzurra_#kvirc, class KviMdiChild
		>>>>>Ptr 43714656: object: #kvirc, class KviChannelWindow
		>Ptr 18004432: object: , class KviStatusBar
		>>Ptr 18007408: object: msgstatuslabel, class QLabel
		>>>Ptr 24067088: object: , class Oxygen::TransitionWidget
		>Ptr 24503248: object: windowlist, class KviTreeWindowList
		>>Ptr 24459744: object: qt_dockwidget_floatbutton, class QDockWidgetTitleButton
		>>Ptr 24498560: object: qt_dockwidget_closebutton, class QDockWidgetTitleButton
		>>Ptr 23996288: object: tree_windowlist, class KviTreeWindowListTreeWidget
		[/example]
		[p]
		As you can see the objects are identified by their names (for example "mdi_manager") and
		by their class names (for example KviChannelWindow).
		To wrap a specific widget you must provide a path in the tree composed of search specifiers.
		Each search specifier can have one of the following forms:
		[/p]
		[example]
		(1) <class>
		(2) <class>::<name>
		(3) ::<name>
		(4) !Window::<window_identifier>
		(5) !Parent::N
		[/example]
		[p]
		The first three forms may be preceeded by the prefix '*' which will tell KVS to perform
		a recursive search from this point. Let's see some examples.
		[/p]
		[p]
		The form (1) mathches the first widget with the specified class name. For instance:
		[/p]
		[example]
		%Frame = $new(wrapper,0,test,KviMainWindow)
		[/example]
		[p]
		This will wrap the first top level object with class KviMainWindow. Now you can use
		any [class]widget[/class] or [class]object[/class] methods on it.
		[/p]
		[example]
		%Frame = $new(wrapper,0,test,KviMainWindow)
		%Frame->$setGeometry(20,20,400,400);
		[/example]
		[p]
		If you want to wrap the kvirc status bar you can use a composite path:
		[/p]
		[example]
		%StatusBar = $new(wrapper,0,test,KviMainWindow,KviStatusBar)
		%StatusBar->$setProperty(autoFillBackground,1)
		%StatusBar->$setBackgroundColor(80,80,0)
		[/example]
		[p]
		The form (2) matches both the class and the widget name. In this way you can differentiate
		between children that have the same class. For instance:
		[/p]
		[example]
		%VerticalScrollBar = $new(wrapper,0,test,KviMainWindow,QSplitter,KviMdiManager,QWidget::qt_scrollarea_vcontainer,QScrollBar)
		%VerticalScrollBar->$setProperty(invertedAppearance,1);
		[/example]
		[p]
		In this way KVS was able to pick the vertical scrollbar instead of the horizontal one (which comes first in the list).
		(Now try to move a window out of the MDI area: the vertical scroll bar will be inverted!).
		[/p]
		[p]
		The form (3) matches only the name and ignores the class. In our sample tree the following example is equivalent to the previous one.
		[/p]
		[example]
		%VerticalScrollBar = $new(wrapper,0,test,KviMainWindow,QSplitter,KviMdiManager,::qt_scrollarea_vcontainer,QScrollBar)
		%VerticalScrollBar->$setProperty(invertedAppearance,1);
		[/example]
		[p]
		If you don't want to specify the full path to the widget you can try to use a recursive search which may skip some levels.
		Keep in mind that the recursive search is breadth-first and will return the first widget that matches.
		In our sample tree the following would match the first widget with class KviChannelWindow.
		[/p]
		[example]
		%Chan = $new(wrapper,0,test,*KviChannelWindow)
		%Chan->$setBackgroundColor(80,0,0);
		[/example]
		[p]
		The following would match the first widget with name #kvirc
		[/p]
		[example]
		%Chan = $new(wrapper,0,test,*::#kvirc)
		%Chan->$setBackgroundColor(80,0,0);
		[/example]
		[p]
		The recursive search can start at any level, so if starting from the root does not work properly you might try
		specifying a part of the path and then searching recursively.
		[/p]
		[example]
		%Chan = $new(wrapper,0,test,KviMainWindow,*::#kvirc)
		%Chan->$setBackgroundColor(80,0,0);
		[/example]
		[p]
		The form (4) allows you to jump directly to a specific kvirc channel/query/console window,
		without the need of looking it up in the tree.
		[/p]
		[example]
		%Win = $new(wrapper,0,test,!Window::$window)
		%Win->$setBackgroundColor(80,0,0);
		[/example]
		[p]
		Finally the last form allows you to jump N levels up in the tree. If N is omitted it is assumed to be 1.
		[/p]
		[example]
		%Win = $new(wrapper,0,test,!Window::$window,!Parent::3)
		%Win->$setGeometry(10,10,40,40)
		[/example]
		[p]
		Experiment with it :)
		[/p]
*/



KVSO_BEGIN_REGISTERCLASS(KvsObject_wrapper,"wrapper","widget")

KVSO_END_REGISTERCLASS(KvsObject_wrapper)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_wrapper,KvsObject_widget)

KVSO_END_CONSTRUCTOR(KvsObject_wrapper)


KVSO_BEGIN_DESTRUCTOR(KvsObject_wrapper)

KVSO_END_CONSTRUCTOR(KvsObject_wrapper)

bool KvsObject_wrapper::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{
	if(!pParams)
		return false;

	QWidget *pWidget = NULL;
	unsigned int i = 0;

	while(i < pParams->count())
	{
		QString szClass;
		QString szName;
		QString s=0;

		pParams->at(i)->asString(s);
		i++;

		if(s.isEmpty())
			continue;

		bool bRecursive = false;

		if(s.startsWith("*"))
		{
			s.remove(0,1);
			bRecursive = true;
			if(s.isEmpty())
			{
				pContext->error(__tr2qs_ctx("The search specifier can't be empty","objects"));
				return false;
			}
		}

		int idx = s.indexOf("::");
		if(idx != -1)
		{
			szClass = s.left(idx);
			szName  = s.right(s.length() - idx - 2);
		} else {
			szClass = s;
			szName  = "";
		}

		if(
				KviQString::equalCI(szClass,"!Window") ||
				KviQString::equalCI(szClass,"WinId") // compat
			)
		{
			if(pWidget)
				pContext->warning(__tr2qs_ctx("The window identifier preceded by '!Window' should be the first in the search path","objects"));

			pWidget = g_pApp->findWindow(szName);

		} else if(KviQString::equalCI(szClass,"!Parent"))
		{
			if(!pWidget)
			{
				pContext->warning(__tr2qs_ctx("The '!Parent' specifier can't be used as first in the search path","objects"));
				return false;
			}
			
			int iLevels = 1;
			if(!szName.isEmpty())
			{
				bool ok;
				int iLevels = szName.toInt(&ok);
				if(!ok)
				{
					pContext->warning(__tr2qs_ctx("Bad number of levels for the '!Parent' specifier","objects"));
					return false;
				}
			}

			while(iLevels > 0)
			{
				pWidget = pWidget->parentWidget();
	
				if(!pWidget)
				{
					pContext->warning(__tr2qs_ctx("The '!Parent' specifier was applied to a widget that has no parent","objects"));
					return false;
				}

				iLevels--;
			}
		} else {
			if(pWidget)
				pWidget = findWidgetToWrap(szClass,szName,pWidget,bRecursive);
			else
				pWidget = findTopLevelWidgetToWrap(szClass,szName,bRecursive);
		}

		if(!pWidget)
		{
			pContext->error(__tr2qs_ctx("Failed to find one of the wrap path widgets ('%Q::%Q')","objects"),&szClass,&szName);
			return false;
		}
	}

	if(!pWidget)
	{
		pContext->error(__tr2qs_ctx("Failed to find the widget to wrap","objects"));
		return false;
	}

	setObject(pWidget,false);
	return true;
}

QWidget * KvsObject_wrapper::findTopLevelWidgetToWrap(const QString &szClass,const QString &szName,bool bRecursive)
{
	QWidgetList list = g_pApp->topLevelWidgets();
	if(list.isEmpty())
		return NULL;

	Q_FOREACH(QWidget * w,list)
	{
		//qDebug("TLW: %s::%s (look for %s::%s)",w->metaObject()->className(),w->objectName().toUtf8().data(),szClass.toUtf8().data(),szName.toUtf8().data());
		if(
				(
					szClass.isEmpty() ||
					KviQString::equalCI(w->metaObject()->className(),szClass)
				) && (
					szName.isEmpty() ||
					KviQString::equalCI(w->objectName(),szName)
				)
			)
			return w;
	}

	if(bRecursive)
	{
		Q_FOREACH(QWidget * w,list)
		{
			w = findWidgetToWrap(szClass,szName,w,bRecursive);
			if(w)
				return w;
		}
	}
	
	return NULL;
}

QWidget * KvsObject_wrapper::findWidgetToWrap(const QString &szClass, const QString &szName, QWidget *pParent,bool bRecursive)
{
	QList<QObject *> list = pParent->children();
	if(list.isEmpty())
		return NULL;

	Q_FOREACH(QObject * obj,list)
	{
		if(!obj->isWidgetType())
			continue;

		QWidget *w = (QWidget *)obj;
		if(
				(
					szClass.isEmpty() ||
					KviQString::equalCI(w->metaObject()->className(),szClass)
				) && (
					szName.isEmpty() ||
					KviQString::equalCI(w->objectName(),szName)
				)
			)
			return w;
	}

	if(bRecursive)
	{
		Q_FOREACH(QObject * obj,list)
		{
			if(!obj->isWidgetType())
				continue;

			QWidget * w = findWidgetToWrap(szClass,szName,(QWidget *)obj,bRecursive);
			if(w)
				return w;
		}
	}

	return NULL;
}
