//mdm:
//   File : class_workspace.cpp
//   Creation date : Sat Feb 12 23:01:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor).
//   Thanks to Lucia Papini (^ashura^) for English help.
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



#include <qworkspace.h>
#include "class_workspace.h"
#include "kvi_error.h"
#include "kvi_debug.h"

#include "kvi_locale.h"
#include "kvi_iconmanager.h"

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
		!fn: <object> $activeWindow()
		Returns the active window, or 0 if no window is active.
		!fn: <boolean> $scrollBarsEnabled()
		Returns TRUE if the workspace provides scrollbars; otherwise returns FALSE.
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
		Activates the prev window in the child window chain.[br]

		|----CLASS EXAMPLE----|[br]
		|-Start:[br]
		#Let's start.[br]
		#We start the main class creation, in the constructor we do the[br]
		#widget's  showing, to give a particular pop-up[br]
		#creation  appearance.[br]
		class (ws,widget)[br]
		{[br]
			constructor[br]
			{[br]
				$$->$setGeometry(%X,%Y,100,100)[br]
				$$->%label=$new(label,$$)[br]
				$$->%label->$settext("Another class by N\&G")[br]
				$$->%label->$setautoresize(1)[br]
				$$->$show()[br]
			}[br]
		}[br]
		#We create the new workspace, and we set a 640x480 size with widget $resize[br]
		#command.[br]
		%Workspace=$new(workspace)[br]
		%Workspace->$resize(640,480)[br]
		#Now we make a cycling costruction of the widgets(look at the class),[br]
		#and give to the widgets a random X and Y coordinates.[br]
		#It takes few seconds to show the effects, be patient.[br]
		%I=0[br]
		while (%I<100)[br]
		{[br]
			%X=$rand(500)[br]
			%Y=$rand(480)[br]
			%Widget=$new(ws,%Workspace)[br]
			%I++[br]
		}[br]
		#Let's show the fireworks! EnJoY![br]
		%Workspace->$show()[br]
		|-Start:[br]
		|-|EXAMPLE n2:[br]
		#This is like the first example but it has a particular animation effect.[br]
		%Hex[]=$array(0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F)[br]
		class (ws,widget)[br]
		{[br]
			constructor[br]
			{[br]
			       $$->$setGeometry(%X,%Y,100,100)[br]
			       $$->%lay=$new(layout,$$)[br]
			       %i=0[br]
			       while (%i<10)[br]
				{[br]
					$$->%label=$new(label,$$)[br]
					$$->%label->$settext("Another class by N\&G")[br]
					%color=%Hex[$rand(15)]%Hex[$rand(15)]%Hex[$rand(15)]%Hex[$rand(15)]%Hex[$rand(15)]%Hex[$rand(15)][br]
					$$->%label->$setforegroundcolor(%color)[br]
					$$->%label->$setautoresize(1)[br]
					$$->%lay->$addwidget($$->%label,%i,0)[br]
					%i++;[br]
				}[br]
				$$->$show()[br]
			}[br]
			mousepressevent[br]
			{[br]
				if ($istimer(cycle) == 1) killtimer cycle[br]
			}[br]
		}[br]
		%Workspace=$new(workspace)[br]
		%Workspace->$resize(640,480)[br]
		%Workspace->$setCaption("Hit the mouse to stop cycling windows....")[br]
		%I=0[br]
		%Cicle=1[br]
		while (%I<20)[br]
		{[br]
			%X=$rand(500)[br]
			%Y=$rand(480)[br]
			%Widget=$new(ws,%Workspace)[br]
			%I++[br]
		}[br]
		%Workspace->$show[br]
		timer (cycle,3000)[br]
		{[br]
			if (%Cicle==1) %Workspace->$tile()[br]
			if (%Cicle==2)[br]
			{[br]
				%Workspace->$cascade()[br]
				%Cicle=1[br]
				return[br]
			}[br]
			%Cicle++[br]
		}[br]
		privateimpl(%Workspace,mousepressevent)[br]
		{[br]
			if ($istimer(cycle) == 1) killtimer cycle[br]
		}[br]
		#-|End.[br]

*/

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_workspace,"workspace","widget")
	KVSO_REGISTER_HANDLER(KviKvsObject_workspace,"activeWindow", functionactiveWindow);
	KVSO_REGISTER_HANDLER(KviKvsObject_workspace,"scrollBarsEnabled", functionscrollBarsEnabled);
	KVSO_REGISTER_HANDLER(KviKvsObject_workspace,"setscrollBarsEnabled", functionsetscrollBarsEnabled);
	KVSO_REGISTER_HANDLER(KviKvsObject_workspace,"cascade", functioncascade);
	KVSO_REGISTER_HANDLER(KviKvsObject_workspace,"tile", functiontile);
	KVSO_REGISTER_HANDLER(KviKvsObject_workspace,"closeActiveWindow", functioncloseActiveWindow);
	KVSO_REGISTER_HANDLER(KviKvsObject_workspace,"closeAllWindows", functioncloseAllWindows);
	KVSO_REGISTER_HANDLER(KviKvsObject_workspace,"activateNextWindow", functionactivateNextWindow);
	KVSO_REGISTER_HANDLER(KviKvsObject_workspace,"activatePrevWindow", functionactivatePrevWindow);
KVSO_END_REGISTERCLASS(KviKvsObject_workspace)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_workspace,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_workspace)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_workspace)

KVSO_END_CONSTRUCTOR(KviKvsObject_workspace)

bool KviKvsObject_workspace::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{
	setObject(new QWorkspace(parentScriptWidget(), name()), true);
	return true;
}

bool KviKvsObject_workspace::functionactiveWindow(KviKvsObjectFunctionCall *c)
{
	//if(widget()) c->returnValue()->setHObject((unsigned long)(((QWorkspace *)widget())->activeWindow()));
	// FIXME!
	c->returnValue()->setHObject(0);
	return true; //?pFIX ME?
}

bool KviKvsObject_workspace::functionscrollBarsEnabled(KviKvsObjectFunctionCall *c)
{
	if(widget())
	c->returnValue()->setBoolean(((QWorkspace *)widget())->scrollBarsEnabled());
	return true;
}

bool KviKvsObject_workspace::functionsetscrollBarsEnabled(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QWorkspace *)widget())->setScrollBarsEnabled(bEnabled);
	return true;
}

bool KviKvsObject_workspace::functioncascade(KviKvsObjectFunctionCall *c)
{
	if(widget())
		((QWorkspace *)widget())->cascade();
	return true;
}

bool KviKvsObject_workspace::functiontile(KviKvsObjectFunctionCall *c)
{
	if(widget())
		((QWorkspace *)widget())->tile();
	return true;
}

bool KviKvsObject_workspace::functioncloseActiveWindow(KviKvsObjectFunctionCall *c)
{
	if(widget())
		((QWorkspace *)widget())->closeActiveWindow();
	return true;
}

bool KviKvsObject_workspace::functioncloseAllWindows(KviKvsObjectFunctionCall *c)
{
	if(widget())
		((QWorkspace *)widget())->closeAllWindows();
	return true;
}

bool KviKvsObject_workspace::functionactivateNextWindow(KviKvsObjectFunctionCall *c)
{
	if(widget())
		((QWorkspace *)widget())->activateNextWindow();
	return true;
}

bool KviKvsObject_workspace::functionactivatePrevWindow(KviKvsObjectFunctionCall *c)
{
	if(widget()){
			((QWorkspace *)widget())->activatePreviousWindow();
	}
		return true;
}
