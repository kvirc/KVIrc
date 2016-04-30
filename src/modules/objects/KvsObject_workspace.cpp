//=============================================================================
//
//   File : KvsObject_workspace.cpp
//   Creation date : Fri Mar 18 14:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include "KvsObject_workspace.h"
#include "KviError.h"
#include "kvi_debug.h"
#include "KviLocale.h"
#include "KviIconManager.h"

#include <QMdiArea>
#include <QMdiSubWindow>
#include <QHash>
#include <QMimeData>

/*
	@doc:	workspace
	@keyterms:
		workspace object class,
	@title:
		workspace class
	@type:
		class
	@short:
		Provides a MDI workspace object
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		This widget provides a workspace window that can contain decorated window.
	@functions:
		!fn: $addSubWindow(<widget>)
		Adds a widget as a subwindow of this workspace.
		!fn: $removeSubWindow(<widget>)
		Removes a subwindow from this this workspace.
		!fn: <object> $activeWindow()
		Returns the active window, or 0 if no window is active.
		!fn: <boolean> $scrollBarsEnabled()
		Returns true if the workspace provides scrollbars; otherwise returns false.
		!fn: $setscrollBarsEnabled(<bEnabled:boolean>)
		Sets whether the workspace provides scrollbars to enable. (bEnabled 1 or 0)
		!fn: $cascade()
		Arranges all the child windows in a cascade pattern.
		!fn: $tile()
		Arranges all child windows in a tile pattern.
		!fn: $closeActiveWindow()
		Closes the child window that is currently active.
		!fn: $closeAllWindows()
		Closes all child windows.
		!fn: $activateNextWindow()
		Activates the next window in the child window chain.
		!fn: $activatePrevWindow()
		Activates the previous window in the child window chain.
	@examples:
		[example]
			[comment]# Let's start.
			# We start the main class creation, in the constructor we do the
			# widget's showing, to give a particular popup
			# creation appearance.
			[/comment]
			class (ws,widget)
			{
				constructor
				{
					$$->$setGeometry(%X,%Y,100,100)[br](KviKvsObjectFunctionCall *c
					$$->%label=$new(label,$$)
					$$->%label->$settext("Another class by N\&G")
					$$->%label->$setautoresize(1)
					$$->$show()
				}
			}
			[comment]# We create the new workspace, and we set a 640x480 size with widget $resize command[/comment]
			%Workspace=$new(workspace)
			%Workspace->$resize(640,480)
			[comment]# Now we make a cycling construction of the widgets (look at the class),
			# and give to the widgets a random X and Y coordinates.
			# It takes few seconds to show the effects, be patient.
			[/comment]
			%I=0
			while (%I<100)
				
				%X=$rand(500)
				%Y=$rand(480)
				%Widget=$new(ws,%Workspace)
				%I++
			}
			[comment]# Let's show the fireworks! EnJoY![/comment]
			%Workspace->$show()
		[/example]
			[b]Example 2:[/b]
		[example]
			[comment]# This is like the first example but it has a particular animation effect.[/comment]
			%Hex[]=$array(0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F)
			class (ws,widget)
			{
				constructor
				{
				$$->$setGeometry(%X,%Y,100,100)
				$$->%lay=$new(layout,$$)
				%i=0
				while (%i<10)
					{
						$$->%label=$new(label,$$)
						$$->%label->$settext("Another class by N\&G")
						%color=%Hex[$rand(15)]%Hex[$rand(15)]%Hex[$rand(15)]%Hex[$rand(15)]%Hex[$rand(15)]%Hex[$rand(15)]
						$$->%label->$setforegroundcolor(%color)
						$$->%label->$setautoresize(1)
						$$->%lay->$addwidget($$->%label,%i,0)
						%i++;
					}
					$$->$show()
				}
				mousepressevent
				{
					if ($istimer(cycle) == 1) killtimer cycle
				}
			}
			%Workspace=$new(workspace)
			%Workspace->$resize(640,480)
			%Workspace->$setWindowTitle("Hit the mouse to stop cycling windows...")
			%I=0
			%Cycle=1
			while (%I<20)
			{
				%X=$rand(500)
				%Y=$rand(480)
				%Widget=$new(ws,%Workspace)
				%I++
			}
			%Workspace->$show
			timer (cycle,3000)
			{
				if (%Cycle==1) %Workspace->$tile()
				if (%Cycle==2)
				{
					%Workspace->$cascade()
					%Cycle=1
					return
				}
				%Cycle++
			}
			privateimpl(%Workspace,mousepressevent)
			{
				if ($istimer(cycle) == 1) killtimer cycle
			}
		[/example]
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_workspace, "workspace", "widget")
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_workspace, addSubWindow);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_workspace, removeSubWindow);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_workspace, activeWindow);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_workspace, scrollBarsEnabled);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_workspace, setscrollBarsEnabled);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_workspace, cascade);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_workspace, tile);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_workspace, closeActiveWindow);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_workspace, closeAllWindows);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_workspace, activateNextWindow);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_workspace, activatePrevWindow);
KVSO_END_REGISTERCLASS(KvsObject_workspace)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_workspace, KvsObject_widget)
pWidgetDict = new QHash<kvs_hobject_t, QMdiSubWindow *>;
KVSO_END_CONSTRUCTOR(KvsObject_workspace)

KVSO_BEGIN_DESTRUCTOR(KvsObject_workspace)
if(pWidgetDict)
{
	pWidgetDict->clear();
	delete pWidgetDict;
	pWidgetDict = nullptr;
}
KVSO_END_CONSTRUCTOR(KvsObject_workspace)

bool KvsObject_workspace::init(KviKvsRunTimeContext *, KviKvsVariantList *)
{
	SET_OBJECT(QMdiArea);
	return true;
}

KVSO_CLASS_FUNCTION(workspace, addSubWindow)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject * pObject;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("widget", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETERS_END(c)

	pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	QMdiSubWindow * pMdi = ((QMdiArea *)object())->addSubWindow(((QWidget *)(pObject->object())));
	pWidgetDict->insert(hObject, pMdi);
	((QMdiArea *)object())->setActiveSubWindow(pMdi);
	return true;
}
KVSO_CLASS_FUNCTION(workspace, removeSubWindow)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject * pObject;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("widget", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETERS_END(c)

	pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	QMdiSubWindow * pMdiSubWindow = pWidgetDict->value(hObject);
	if(pMdiSubWindow)
	{
		((QMdiArea *)object())->removeSubWindow(pMdiSubWindow);
		pWidgetDict->remove(hObject);
	}
	else
	{
		c->warning(__tr2qs_ctx("The widget must be a subwindow of this workspace", "objects"));
	}
	return true;
}

KVSO_CLASS_FUNCTION(workspace, cascade)
{
	CHECK_INTERNAL_POINTER(widget())
	((QMdiArea *)widget())->cascadeSubWindows();
	return true;
}
KVSO_CLASS_FUNCTION(workspace, tile)
{
	CHECK_INTERNAL_POINTER(widget())
	((QMdiArea *)widget())->tileSubWindows();
	return true;
}
KVSO_CLASS_FUNCTION(workspace, closeActiveWindow)
{
	CHECK_INTERNAL_POINTER(widget())
	((QMdiArea *)widget())->closeActiveSubWindow();
	return true;
}

KVSO_CLASS_FUNCTION(workspace, closeAllWindows)
{
	CHECK_INTERNAL_POINTER(widget())
	((QMdiArea *)widget())->closeAllSubWindows();
	return true;
}

KVSO_CLASS_FUNCTION(workspace, activateNextWindow)
{
	CHECK_INTERNAL_POINTER(widget())
	((QMdiArea *)widget())->activateNextSubWindow();
	return true;
}

KVSO_CLASS_FUNCTION(workspace, activatePrevWindow)
{
	CHECK_INTERNAL_POINTER(widget())
	((QMdiArea *)widget())->activatePreviousSubWindow();
	return true;
}

KVSO_CLASS_FUNCTION(workspace, activeWindow)
{
	CHECK_INTERNAL_POINTER(widget())
	QMdiSubWindow * pActiveMdi = ((QMdiArea *)widget())->activeSubWindow();
	if(pActiveMdi)
	{
		QHashIterator<kvs_hobject_t, QMdiSubWindow *> t(*pWidgetDict);
		while(t.hasNext())
		{
			t.next();
			kvs_hobject_t hObject = t.key();
			QMdiSubWindow * pMdi = t.value();
			if(pMdi == pActiveMdi)
			{
				c->returnValue()->setHObject(hObject);
				break;
			}
		}
	}
	else
		c->returnValue()->setHObject((kvs_hobject_t) nullptr);
	return true;
}

KVSO_CLASS_FUNCTION(workspace, scrollBarsEnabled)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bEnabled;
	if(((QMdiArea *)widget())->verticalScrollBarPolicy() != Qt::ScrollBarAlwaysOff)
		bEnabled = true;
	else
		bEnabled = false;
	c->returnValue()->setBoolean(bEnabled);
	return true;
}

KVSO_CLASS_FUNCTION(workspace, setscrollBarsEnabled)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bEnabled", KVS_PT_BOOL, 0, bEnabled)
	KVSO_PARAMETERS_END(c)
	if(bEnabled)
	{
		((QMdiArea *)widget())->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		((QMdiArea *)widget())->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	}
	else
	{
		((QMdiArea *)widget())->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		((QMdiArea *)widget())->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}
	return true;
}
