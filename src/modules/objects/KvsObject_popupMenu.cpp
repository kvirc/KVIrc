//=============================================================================
//
//   File : KvsObject_popupMenu.cpp
//   Creation date : Fri Mar 18 21:30:48 CEST 2005
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

#include "KvsObject_popupMenu.h"

#include "KviError.h"
#include "kvi_debug.h"
#include "KviLocale.h"
#include "KviIconManager.h"

#include <QCursor>
#include <QMenu>
#include <QAction>

/*
	@doc: popupmenu
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
		This widget can be used to display a popup menu. Technically, a popup menu consists of a list of menu items.[br]
		You add items with insertItem(). An item can be a string. In addition, items can have an optional icon drawn on the very left side.[br]
	@functions:
		!fn: $insertItem(<text:string>,[icon_id:string])
		Inserts menu items into a popup menu with optional icon and return the popup identifier.
		!fn: $addMenu(<popupmenu:hobject,[idx:uinteger])
		Add a popupmenu.
		With the optional parameter idx the popup will be inserted.
		!fn: $setTitle(<text:string>)
		Sets the popupmenu title to text.
		!fn: $exec([<widget:objects>,<x:uinteger>,<y:integer>])
		If called without parameters show the popup menu at the current pointer position.[br]
		With the optional parameters show the popup menu at the coordinate x,y widget parameter relative.
		!fn: $removeItem(<popup_id:Uinteger>)
		Removes the menu item that has the identifier ID.
		!fn: $removeItemAt(<index:uinteger>)
		Removes the menu item at position index.
		!fn: $insertSeparator(<index:uinteger>)
		Inserts a separator at position index.[br]
		If the index is negative the separator becomes the last menu item.
		!fn: $activatedEvent(<popup_id:uinteger>)
		This function is called when a menu item and return the the item ID.[br]
		The default implementation emits the [classfnc]$activated[/classfnc]() signal.
		!fn: $highligthtedEvent(<popup_id:uinteger>)
		This function is called when a menu item is highlighted (hovered) and return the item ID.[br]
		The default implementation emits the [classfnc]$highlighted[/classfnc]() signal.
	@signals:
		!sg: $activated()
		This signal is emitted by the default implementation of [classfnc]$activatedEvent[/classfnc]().
		!sg: $highlighted()
		This signal is emitted by the default implementation of [classfnc]$highlightedEvent[/classfnc]().
		[br]
	@examples:
		[example]
			[comment]# First we define a class inherited from popupmenu[/comment]
			class(menu,popupmenu)
			{
				constructor()
				{
					[comment]# We store the item's ID for checking in activatedEvent[/comment]
					@%tile_id=@$insertItem("Tile",118)
					@%cascade_id=@$insertItem("Cascade",115)
					@$insertSeparator(3)
					@%closeactw_id=@$insertItem("Close Active Window",08)
					@%closeallw_id=@$insertItem("Close All Window",58)
				}
				activatedEvent()
				{
					[comment]# Now we emit a signals to the relative to the user choice[/comment]
					%id=$0
					if (%id==@%tile_id) @$emit("tile")
					else if(%id==@%cascade_id) @$emit("cascade")
					else if (%id==@%closeactw_id) @$emit("closeactive")
					else @$emit("closeallwindows")
					[comment]# Deleting the popup[/comment]
					delete $$
				}
			}
			class (ws,widget)[br]
			{
				[comment]In the constructor we create everything that belong to the single widget.[/comment]
				constructor()
				{
					[comment]# Here we generate a loop to create our labels inside the widget.[/comment]
					%lay=$new(layout,$$)
					[comment]we use a vbox to managing labels in vertical orientation[/comment]
					%vb=$new(vbox,$$)
					[comment]# Then add the vbox to the main layout[/comment]
					%lay->$addWidget(%vb,0,0)
					[comment]# Let's create our colorful labels[/comment]
					for(%i=0;%i<15;%i++)
					{
						@%label=$new(label,%vb)
						@%label->$settext("Another class by N\&G")
						[comment]# We set our foreground's colors using the hex array in a random way.[/comment]
						@%label->$setforegroundcolor($array($rand(255),$rand(255),$rand(255)))
					}
				}
				customContextMenuRequestedEvent()
				{
					[comment]# We create the popupmenu relative to this widget at runtime.[/comment]
					%p=$new(menu,$$)
					objects.connect %p tile @$parent tile
					objects.connect %p cascade @$parent cascade
					objects.connect %p closeactive @$parent closeactivewindow
					objects.connect %p closeallwindows @$parent closeallwindows
					%p->$exec($$,$($0+10),$($1+10))
				}
			}
			[comment]# We create the workspace widget[/comment]
			%Workspace=$new(workspace)
			[comment]# We use as space as we have[/comment]
			%size[]=%Workspace->$screenResolution()
			[comment]# Resize it[/comment]
			%Workspace->$resize(%size[0],%size[1])
			[comment]# Then create 20 subwidget[/comment]
			for(%i=0;%i<20;%i++)
			{
				%w=$new(ws,%Workspace)
				[comment]# Let's add every widget to the workspace[/comment]
				%Workspace->$addSubWindow(%w)
				[comment]# Then "shake it" a little bit around :-)[/comment]
				%w->$move($rand($(%size[0]-50)),$rand($(%size[1]-50)))
			}
			[comment]# Reimplement closeEvent to delete all this :-)[/comment]
			privateimpl(%Workspace,closeEvent)
			{
				delete $$
			}
			[comment]# Let's show![/comment]
			%Workspace->$show()
		[/example]
*/

static QHash<int, QAction *> actionsDict;
static int iIdentifier = 0;

static int addActionToDict(QAction * pAction)
{
	actionsDict.insert(iIdentifier, pAction);
	iIdentifier++;
	return (iIdentifier - 1);
}
static void removeMenuAllActions(QMenu * pMenu)
{
	QList<QAction *> pActionsList = pMenu->actions();
	QList<QAction *> pActionsListDictValues = actionsDict.values();
	for(int i = 0; i < pActionsList.count(); i++)
	{
		int iIdx = pActionsListDictValues.indexOf(pActionsList.at(i));
		if(iIdx >= 0)
			actionsDict.remove(actionsDict.key(pActionsList.at(i)));
	}
}

static QAction * getAction(int idx)
{
	return actionsDict.value(idx);
}
static void removeAction(int idx)
{
	actionsDict.remove(idx);
}

KVSO_BEGIN_REGISTERCLASS(KvsObject_popupMenu, "popupmenu", "widget")
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_popupMenu, insertItem)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_popupMenu, setTitle)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_popupMenu, exec)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_popupMenu, insertSeparator)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_popupMenu, removeItem)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_popupMenu, addMenu)

// events
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_popupMenu, highlightedEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_popupMenu, activatedEvent)

KVSO_END_REGISTERCLASS(KvsObject_popupMenu)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_popupMenu, KvsObject_widget)

KVSO_END_CONSTRUCTOR(KvsObject_popupMenu)

KVSO_BEGIN_DESTRUCTOR(KvsObject_popupMenu)

KVSO_END_DESTRUCTOR(KvsObject_popupMenu)

bool KvsObject_popupMenu::init(KviKvsRunTimeContext *, KviKvsVariantList *)
{
	SET_OBJECT(QMenu)
	connect(widget(), SIGNAL(triggered(QAction *)), this, SLOT(slottriggered(QAction *)));
	connect(widget(), SIGNAL(destroyed(QObject *)), this, SLOT(aboutToDie(QObject *)));
	connect(widget(), SIGNAL(hovered(QAction *)), this, SLOT(slothovered(QAction *)));
	return true;
}

KVSO_CLASS_FUNCTION(popupMenu, insertItem)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szItem, szIcon;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("text", KVS_PT_STRING, 0, szItem)
	KVSO_PARAMETER("icon_id", KVS_PT_STRING, KVS_PF_OPTIONAL, szIcon)
	KVSO_PARAMETERS_END(c)

	QPixmap * pix = nullptr;
	QAction * pAction = nullptr;
	if(!szIcon.isEmpty())
	{
		pix = g_pIconManager->getImage(szIcon);
		if(pix)
			pAction = ((QMenu *)widget())->addAction(*pix, szItem);
		else
			c->warning(__tr2qs_ctx("Icon '%Q' doesn't exist", "objects"), &szIcon);
	}
	else
		pAction = ((QMenu *)widget())->addAction(szItem);
	int identifier = addActionToDict(pAction);
	c->returnValue()->setInteger((kvs_int_t)identifier);
	return true;
}

KVSO_CLASS_FUNCTION(popupMenu, setTitle)
{
	QString szTitle;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("title", KVS_PT_STRING, 0, szTitle)
	KVSO_PARAMETERS_END(c)
	if(!widget())
		return true;
	((QMenu *)widget())->setTitle(szTitle);

	return true;
}

KVSO_CLASS_FUNCTION(popupMenu, exec)
{
	CHECK_INTERNAL_POINTER(widget())
	if(!c->params()->count())
	{
		((QMenu *)widget())->exec(QCursor::pos());
		return true;
	}

	KviKvsObject * pObject;
	kvs_uint_t iX, iY;
	QString szLabel, szIcon;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("widget", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETER("x", KVS_PT_UNSIGNEDINTEGER, 0, iX)
	KVSO_PARAMETER("y", KVS_PT_UNSIGNEDINTEGER, 0, iY)
	KVSO_PARAMETERS_END(c)
	pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	((QMenu *)widget())->exec(((QWidget *)(pObject->object()))->mapToGlobal(QPoint(iX, iY)));

	return true;
}

KVSO_CLASS_FUNCTION(popupMenu, addMenu)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject * pObject;
	kvs_uint_t iIdx;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("popupmenu", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETER("index", KVS_PT_UNSIGNEDINTEGER, KVS_PF_OPTIONAL, iIdx)
	KVSO_PARAMETERS_END(c)

	pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!pObject)
	{
		c->warning(__tr2qs_ctx("Popup menu parameter is not an object", "objects"));
		return true;
	}
	if(!pObject->object())
	{
		c->warning(__tr2qs_ctx("Popup menu parameter is not a valid object", "objects"));
		return true;
	}
	if(!pObject->inheritsClass("popupmenu"))
	{
		c->warning(__tr2qs_ctx("Popupmenu object required", "objects"));
		return true;
	}
	QAction * pAction;
	if(iIdx)
	{
		QAction * pActionBefore = actionsDict[iIdx];
		pAction = ((QMenu *)widget())->insertMenu(pActionBefore, (QMenu *)pObject->object());
	}
	else
	{
		pAction = ((QMenu *)widget())->addMenu((QMenu *)pObject->object());
	}
	int identifier = addActionToDict(pAction);
	c->returnValue()->setInteger((kvs_int_t)identifier);
	identifier++;
	return true;
}

KVSO_CLASS_FUNCTION(popupMenu, removeItem)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iIdx;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("item_id", KVS_PT_INTEGER, 0, iIdx)
	KVSO_PARAMETERS_END(c)
	QAction * pAction = getAction(iIdx);
	if(pAction)
	{
		((QMenu *)widget())->removeAction(pAction);
		removeAction(iIdx);
	}
	return true;
}

KVSO_CLASS_FUNCTION(popupMenu, insertSeparator)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t iIndex;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("index", KVS_PT_UNSIGNEDINTEGER, 0, iIndex)
	KVSO_PARAMETERS_END(c)
	QAction * pAction = getAction(iIndex);
	if(pAction)
		((QMenu *)widget())->insertSeparator(pAction);
	return true;
}

void KvsObject_popupMenu::slothovered(QAction * a)
{
	QHashIterator<int, QAction *> i(actionsDict);
	bool bFound = false;
	while(i.hasNext())
	{
		i.next();
		if(i.value() == a)
		{
			bFound = true;
			break;
		}
	}

	// check if the action was created inside this class
	if(bFound)
	{
		KviKvsVariantList params(new KviKvsVariant((kvs_int_t)i.key()));
		callFunction(this, "highlightedEvent", &params);
	}
}
void KvsObject_popupMenu::aboutToDie(QObject * pObject)
{
	qDebug("Removing popup from KVS dict");
	removeMenuAllActions((QMenu *)pObject);
}
KVSO_CLASS_FUNCTION(popupMenu, highlightedEvent)
{
	emitSignal("highlighted", c, c->params());
	return true;
}

void KvsObject_popupMenu::slottriggered(QAction * a)
{
	QHashIterator<int, QAction *> i(actionsDict);
	bool bFound = false;
	while(i.hasNext())
	{
		i.next();
		if(i.value() == a)
		{
			bFound = true;
			break;
		}
	}

	// check if the action was created inside this class
	if(bFound)
	{
		KviKvsVariantList params(new KviKvsVariant((kvs_int_t)i.key()));
		callFunction(this, "activatedEvent", &params);
	}
}

KVSO_CLASS_FUNCTION(popupMenu, activatedEvent)
{
	emitSignal("activated", c, c->params());
	return true;
}
