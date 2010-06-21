//=============================================================================
//
//   File : class_popupmenu.cpp
//   Creation date : Fri Mar 18 21:30:48 CEST 2005
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================


#include "class_popupmenu.h"

#include "kvi_error.h"
#include "kvi_debug.h"
#include "kvi_locale.h"
#include "kvi_iconmanager.h"
#include "kvi_tal_popupmenu.h"

#include <QCursor>
#include <QMenu>
#include <QAction>



/*
	@doc:   popupmenu
	@keyterms:
		popupmenu object class,
	@title:
		popupmenu class
	@type:
		class
	@short:
		Provides a popup menu.
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		This widget can be used to display a popup menu.Technically, a popup menu consists of a list of menu items.[br]
		You add items with insertItem(). An item is either a string. In addition, items can have an optional icon drawn on the very left side.[br]
	@functions:
		!fn: $insertItem(<text:string>,[icon_id:string])
		Inserts menu items into a popup menu with optional icon and return the popup identifier.
		!fn: $setTitle(<text:string>)
		Sets the popupmenu title to text.
		!fn: $exec([<widget:objects>,<x:uinteger>,<y:integer>])
		If called without paramaters show the popup menu at the current pointer position.[br]
		With the optional parameters show the popup menu at the coordinate x,y widget parameter relative.
		!fn: $removeItem(<popup_id:Uinteger>)
		Removes the menu item that has the identifier id.
		!fn: $removeItemAt(<index:uinteger>)
		Removes the menu item at position index.
		!fn: $insertSeparator(<index:uinteger>)
		Inserts a separator at position index.[br]
		If the index is negative the separator becomes the last menu item.
		!fn: $activatedEvent(<popup_id:uinteger>)
		This function is called when a menu item and return the the item id.[br]
		The default implementation emits the [classfnc]$activated[/classfnc]() signal.
		!fn: $highligthtedEvent(<popup_id:uinteger>)
		This function is called when a menu item is highlighted and return the item id.[br]
		The default implementation emits the [classfnc]$highlighted[/classfnc]() signal.
	@signals:
		!sg: $activated()
		This signal is emitted by the default implementation of [classfnc]$activatedEvent[/classfnc]().
		!sg: $highlighted()
		This signal is emitted by the default implementation of [classfnc]$highlightedEvent[/classfnc]().
		[br]
	@examples:
		[example]
		// first we define a class inherited from popupmenu
		class(menu,popupmenu)
		{
			constructor()
			{
				// we store the item's id for checkit in activatedEvent
				@%tile_id=@$insertItem("Tile",118)
				@%cascade_id=@$insertItem("Cascade",115)
				@$insertSeparator(3)
				@%closeactw_id=@$insertItem("Close Active Window",08)
				@%closeallw_id=@$insertItem("Close All Window",58)
			}
			activatedEvent()
			{
				// now we emit a signals to the relative to the user choice
				%id=$0
				if (%id==@%tile_id) @$emit("tile")
				else if(%id==@%cascade_id) @$emit("cascade")
				else if (%id==@%closeactw_id) @$emit("closeactive")
				else @$emit("closeallwindows")
				// deleting the popup
				delete $$
			}
		}
		class (ws,widget)[br]
		{
			#In the constructor we create everything that belong to the single widget.
			constructor()
			{
				#Here we generate a loop to create our labels inside the widget.
				%lay=$new(layout,$$)
				// we use a vbox to managing labels in vertical orientation
				%vb=$new(vbox,$$)
				// then add the vbox to the main layout
				%lay->$addWidget(%vb,0,0)
				// let's create our colorful labels
				for(%i=0;%i<15;%i++)
				{
					@%label=$new(label,%vb)
					@%label->$settext("Another class by N\&G")
					#We set our foreground's colors using the hex array in a random way.
					@%label->$setforegroundcolor($array($rand(255),$rand(255),$rand(255)))
				}
			}
			customContextMenuRequestedEvent()
			{
				#We create  the popupmenu relative to this widget at runtime.
				%p=$new(menu,$$)
				objects.connect %p tile @$parent tile
				objects.connect %p cascade @$parent cascade
				objects.connect %p closeactive @$parent closeactivewindow
				objects.connect %p closeallwindows @$parent closeallwindows
				%p->$exec($$,$($0+10),$($1+10))
			}
		}
		// we create the workspace widget
		%Workspace=$new(workspace)
		// we use as space as we have
		%size[]=%Workspace->$screenResolution()
		// resize it
		%Workspace->$resize(%size[0],%size[1])
		// then create 20 subwidget
		for(%i=0;%i<20;%i++)
		{
			%w=$new(ws,%Workspace)
			// let's add avery widget to the workspace
			%Workspace->$addSubWindow(%w)
			// then "shake it" a little bit around :-)
			%w->$move($rand($(%size[0]-50)),$rand($(%size[1]-50)))
		}
		// reimplement closeEvent to delete all this :-)
		privateimpl(%Workspace,closeEvent)
		{
			delete $$
		}
		#Let's show!
		%Workspace->$show()
		[/example]
*/

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_popupmenu,"popupmenu","widget")
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_popupmenu,insertItem)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_popupmenu,setTitle)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_popupmenu,exec)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_popupmenu,insertSeparator)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_popupmenu,removeItem)

	// events
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_popupmenu,highlightedEvent)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_popupmenu,activatedEvent)

KVSO_END_REGISTERCLASS(KviKvsObject_popupmenu)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_popupmenu,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_popupmenu)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_popupmenu)

KVSO_END_CONSTRUCTOR(KviKvsObject_popupmenu)

bool KviKvsObject_popupmenu::init(KviKvsRunTimeContext *,KviKvsVariantList *)
{
	identifier=0;
	SET_OBJECT(QMenu)
	connect(widget(),SIGNAL(triggered(QAction *)),this,SLOT(slottriggered(QAction *)));
	connect(widget(),SIGNAL(highlighted(int)),this,SLOT(slothighlighted(int)));
	return true;
}

KVSO_CLASS_FUNCTION(popupmenu,insertItem)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szItem,szIcon;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szItem)
		KVSO_PARAMETER("icon_id",KVS_PT_STRING,KVS_PF_OPTIONAL,szIcon)
	KVSO_PARAMETERS_END(c)
	QPixmap *pix = 0;
	QAction * action = 0;
	if(!szIcon.isEmpty())
	{
		pix = g_pIconManager->getImage(szIcon);
		if (pix) action=((QMenu *)widget())->addAction(*pix,szItem);
		else c->warning(__tr2qs_ctx("Icon '%Q' doesn't exist","objects"),&szIcon);
	}
	else
		action=((QMenu *)widget())->addAction(szItem);
	actionsDict[identifier]=action;
	c->returnValue()->setInteger(identifier);
	identifier++;
	return true;
}

KVSO_CLASS_FUNCTION(popupmenu,setTitle)
{
	QString szTitle;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("title",KVS_PT_STRING,0,szTitle)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	((QMenu *)widget())->setTitle(szTitle);

	return true;
}

KVSO_CLASS_FUNCTION(popupmenu,exec)
{
	CHECK_INTERNAL_POINTER(widget())
	if(!c->params()->count())
	{
		((QMenu *)widget())->exec(QCursor::pos());
		return true;
	}

	KviKvsObject *pObject;
	kvs_uint_t iX,iY;
	QString szLabel,szIcon;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("x",KVS_PT_UNSIGNEDINTEGER,0,iX)
		KVSO_PARAMETER("y",KVS_PT_UNSIGNEDINTEGER,0,iY)
	KVSO_PARAMETERS_END(c)
	pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	((QMenu *)widget())->exec(((QWidget *)(pObject->object()))->mapToGlobal(QPoint(iX,iY)) );

	return true;
}

KVSO_CLASS_FUNCTION(popupmenu,removeItem)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uItem;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("item_id",KVS_PT_UNSIGNEDINTEGER,0,uItem)
	KVSO_PARAMETERS_END(c)
	QAction * action=actionsDict.value(uItem);
	if(action)
	{
		((QMenu *)widget())->removeAction(action);
		identifier--;
	}
	return true;
}

KVSO_CLASS_FUNCTION(popupmenu,insertSeparator)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t iIndex;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("index",KVS_PT_UNSIGNEDINTEGER,0,iIndex)
	KVSO_PARAMETERS_END(c)
	QAction * action=actionsDict.value(iIndex);
	if(action)((QMenu *)widget())->insertSeparator(action);
	return true;
}

void KviKvsObject_popupmenu::slothighlighted(int i)
{
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)i));
	callFunction(this,"highlightedEvent",&params);
}

KVSO_CLASS_FUNCTION(popupmenu,highlightedEvent)
{
	emitSignal("highlighted",c,c->params());
	return true;
}

void KviKvsObject_popupmenu::slottriggered(QAction *a)
{
	QHashIterator<int, QAction *> i(actionsDict);
	kvs_int_t count=0;
	while (i.hasNext())
	{
		i.next();
		if (i.value()!= a) count++;
		else break;
	}
	KviKvsVariantList params(new KviKvsVariant(count));
	callFunction(this,"activatedEvent",&params);
}

KVSO_CLASS_FUNCTION(popupmenu,activatedEvent)
{
	emitSignal("activated",c,c->params());
	return true;
}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_popupmenu.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
