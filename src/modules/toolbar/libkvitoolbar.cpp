//=============================================================================
//
//   File : libkvitoolbar.cpp
//   Creation date : Wed 6 Nov 22:51:59 2002 GMT by Szymon Stefanek
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

#include "KviModule.h"
#include "KviLocale.h"
#include "KviWindow.h"
#include "KviApplication.h"
#include "KviError.h"
#include "KviCommandFormatter.h"
#include "KviCustomToolBar.h"
#include "KviCustomToolBarDescriptor.h"
#include "KviCustomToolBarManager.h"
#include "KviActionManager.h"
#include "KviKvsUserAction.h"

/*
	@doc: toolbar.create
	@type:
		command
	@title:
		toolbar.create
	@keyterms:
		Creating toolbars
	@short:
		Creates a toolbar
	@syntax:
		toolbar.create [-p] <id:string> <label:string> [icon_id:string]
	@switches:
		!sw: -p | --preserve
		Preserve the existing toolbar, if any
	@description:
		Creates a toolbar with the specified <id> and with the visible <label> (caption).[br]
		If a toolbar with the same <id> already exists then this command is equivalent to the
		sequence of [cmd]toolbar.hide[/cmd], [cmd]toolbar.clear[/cmd] and [cmd]toolbar.setlabel[/cmd] unless the -p
		switch is specified that causes the [cmd]toolbar.clear[/cmd] call to be skipped (contents preserved).[br]
		This command doesn't show the toolbar (you need to use [cmd]toolbar.show[/cmd]).[br]
		Toolbars are saved and preserved across KVIrc sessions thus you don't need to create them
		on application startup: you only create them once.
		If [icon_id] is specified then it is interpreted as an [doc:image_id]image identifier[/doc]
		for the small (16x16) icon that will be put in the toolbar listing popup menu.
		The toolbar will exist until [cmd]toolbar.destroy[/cmd] is called (you might want to add
		an "uninstall" feature to your script).[br]
	@seealso:
		[fnc]$toolbar.exists[/fnc],
		[cmd]toolbar.remove[/cmd],
		[cmd]toolbar.show[/cmd],
		[cmd]toolbar.destroy[/cmd]
*/

static bool toolbar_kvs_cmd_create(KviKvsModuleCommandCall * c)
{
	QString szId, szLabel, szIconId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("id", KVS_PT_NONEMPTYSTRING, 0, szId)
	KVSM_PARAMETER("label", KVS_PT_STRING, KVS_PF_OPTIONAL, szLabel)
	KVSM_PARAMETER("icon_id", KVS_PT_NONEMPTYSTRING, KVS_PF_OPTIONAL, szIconId)
	KVSM_PARAMETERS_END(c)

	if(szLabel.isEmpty())
		szLabel = "$tr(Unnamed)";

	KviCustomToolBarDescriptor * d = KviCustomToolBarManager::instance()->find(szId);
	if(d)
	{
		if(!c->switches()->find('p', "preserve"))
			d->clear();
		d->rename(szLabel);
	}
	else
	{
		d = KviCustomToolBarManager::instance()->create(szId, szLabel);
	}

	if(!szIconId.isEmpty())
		d->setIconId(szIconId);

	return true;
}

/*
	@doc: toolbar.clear
	@type:
		command
	@title:
		toolbar.clear
	@short:
		Clears a toolbar
	@syntax:
		toolbar.clear [-q] <id:string>
	@switches:
		!sw: -q | --quiet
		Run quietly
	@description:
		Clears the contents of the toolbar specified by <id>. If the
		toolbar does not exist then a warning is printed unless the -q switch is used.[br]
	@seealso:
		[fnc]$toolbar.exists[/fnc],
		[cmd]toolbar.remove[/cmd],
		[cmd]toolbar.show[/cmd],
		[cmd]toolbar.destroy[/cmd]
*/

static bool toolbar_kvs_cmd_clear(KviKvsModuleCommandCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("id", KVS_PT_NONEMPTYSTRING, 0, szId)
	KVSM_PARAMETERS_END(c)

	KviCustomToolBarDescriptor * d = KviCustomToolBarManager::instance()->find(szId);
	if(d)
		d->clear();
	else
	{
		if(!c->switches()->find('q', "quiet"))
			c->warning(__tr2qs("The specified toolbar doesn't exist"));
	}

	return true;
}

/*
	@doc: toolbar.destroy
	@type:
		command
	@title:
		toolbar.destroy
	@keyterms:
		Destroying a toolbar
	@short:
		Destroys a toolbar
	@syntax:
		toolbar.destroy [-q] <id:string>
	@switches:
		!sw: -q | --quiet
		Run quietly
	@description:
		Destroys the toolbar with the specified <id>.[br]
		The toolbar definition is definitively destroyed with all the items contained.[br]
		If the -q switch is specified then this command runs in quiet mode and
		does not warn if the toolbar was not existing.
	@seealso:
		[fnc]$toolbar.exists[/fnc],
		[cmd]toolbar.create[/cmd]
*/

static bool toolbar_kvs_cmd_destroy(KviKvsModuleCommandCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("id", KVS_PT_NONEMPTYSTRING, 0, szId)
	KVSM_PARAMETERS_END(c)

	if(!KviCustomToolBarManager::instance()->destroyDescriptor(szId))
	{
		if(!c->switches()->find('q', "quiet"))
			c->warning(__tr2qs("The specified toolbar doesn't exist"));
	}

	return true;
}

/*
	@doc: toolbar.show
	@type:
		command
	@title:
		toolbar.show
	@short:
		Shows a toolbar
	@syntax:
		toolbar.show [-q] <id:string>
	@switches:
		!sw: -q | --quiet
		Run quietly
	@description:
		Shows the toolbar with the specified <id>.[br]
		If -q is specified the command runs in quiet mode and does not complain if the
		specified toolbar doesn't exist.[br]
		Please note that you don't need to show the toolbar at application startup: KVIrc will
		take care of this for you (if the user chooses to keep the toolbar visible).[br]
		You should execute this command only if you create the toolbar after KVIrc has been
		started.[br]
	@seealso:
		[fnc]$toolbar.exists[/fnc],
		[cmd]toolbar.create[/cmd]
*/

static bool toolbar_kvs_cmd_show(KviKvsModuleCommandCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("id", KVS_PT_NONEMPTYSTRING, 0, szId)
	KVSM_PARAMETERS_END(c)

	KviCustomToolBarDescriptor * d = KviCustomToolBarManager::instance()->find(szId);

	if(!d)
	{
		if(!c->switches()->find('q', "quiet"))
			c->warning(__tr2qs("The specified toolbar doesn't exist"));
		return true;
	}

	if(!d->toolBar())
		d->createToolBar();

	return true;
}

/*
	@doc: toolbar.hide
	@type:
		command
	@title:
		toolbar.hide
	@short:
		Hides a toolbar
	@syntax:
		toolbar.hide [-q] <id:string>
	@switches:
		!sw: -q | --quiet
		Run quietly
	@description:
		Hides the toolbar with the specified <id>.[br]
		If -q is specified the command runs in quiet mode and does not complain if the
		specified toolbar doesn't exist.[br]
		Please note that this command doesn't destroy the toolbar definition: the toolbar
		can be shown again by using [cmd]toolbar.show[/cmd].[br]
		If you want to completely remove the toolbar definition use [cmd]toolbar.destroy[/cmd].[br]
	@seealso:
		[fnc]$toolbar.exists[/fnc],
		[cmd]toolbar.create[/cmd],
		[cmd]toolbar.destroy[/cmd]
*/

static bool toolbar_kvs_cmd_hide(KviKvsModuleCommandCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("id", KVS_PT_NONEMPTYSTRING, 0, szId)
	KVSM_PARAMETERS_END(c)

	KviCustomToolBarDescriptor * d = KviCustomToolBarManager::instance()->find(szId);

	if(!d)
	{
		if(!c->switches()->find('q', "quiet"))
			c->warning(__tr2qs("The specified toolbar doesn't exist"));
		return true;
	}

	if(d->toolBar())
		KviCustomToolBarManager::instance()->destroyDescriptor(d->id());

	return true;
}

/*
	@doc: toolbar.exists
	@type:
		function
	@title:
		$toolbar.exists
	@short:
		Checks if a toolbar exists
	@syntax:
		<boolean> $toolbar.exists(<id:string>)
	@description:
		Returns [b]1[/b] if the toolbar with the specified <id> has been defined.[br]
		Note that this doesn't mean that the toolbar is visible!
	@seealso:
		[cmd]toolbar.create[/cmd]
*/

static bool toolbar_kvs_fnc_exists(KviKvsModuleFunctionCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("id", KVS_PT_NONEMPTYSTRING, 0, szId)
	KVSM_PARAMETERS_END(c)

	c->returnValue()->setBoolean(KviCustomToolBarManager::instance()->find(szId) ? true : false);
	return true;
}

/*
	@doc: toolbar.isVisible
	@type:
		function
	@title:
		$toolbar.isVisible
	@short:
		Checks if a toolbar is visible
	@syntax:
		<boolean> $toolbar.isVisible(<id:string>)
	@description:
		Returns [b]1[/b] if the toolbar with the specified <id> is visible,
		or 0 when invisible or the given toolbar does not exist.
	@seealso:
		[cmd]toolbar.show[/cmd]
*/

static bool toolbar_kvs_fnc_isVisible(KviKvsModuleFunctionCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("id", KVS_PT_NONEMPTYSTRING, 0, szId)
	KVSM_PARAMETERS_END(c)
	KviCustomToolBarDescriptor * d = KviCustomToolBarManager::instance()->find(szId);
	c->returnValue()->setBoolean(d ? (d->toolBar() ? true : false) : false);
	return true;
}

/*
	@doc: toolbar.list
	@type:
		function
	@title:
		$toolbar.list
	@short:
		Returns a list of defined toolbars
	@syntax:
		<array> $toolbar.list()
	@description:
		Returns a list of defined toolbar identifiers -
		this can be used to loop through all defined toolbars.
	@seealso:
*/

static bool toolbar_kvs_fnc_list(KviKvsModuleFunctionCall * c)
{
	KviKvsArray * a = new KviKvsArray();

	KviPointerHashTableIterator<QString, KviCustomToolBarDescriptor> it(*(KviCustomToolBarManager::instance()->descriptors()));

	kvs_uint_t id = 0;

	while(KviCustomToolBarDescriptor * d = it.current())
	{
		a->set(id, new KviKvsVariant(d->id()));
		id++;
		++it;
	}

	c->returnValue()->setArray(a);
	return true;
}

/*
	@doc: toolbar.remove
	@type:
		command
	@title:
		toolbar.remove
	@keyterms:
		Removing toolbar items
	@short:
		Removes a toolbar item
	@syntax:
		toolbar.remove [-q] [-i] <toolbarid:string> <action:variant>
	@switches:
		!sw: -q | --quiet
		Run quietly
		!sw: -i | --index
		Treat <action> as the zero-based index of the action to be removed
		instead of the action name.
	@description:
		Removes the specified <action> from the toolbar with the specified <toolbarid>.[br]
		If no such item exists in the specified toolbar this command does nothing.[br]
		If the <toolbar> parameter doesn't identify an existing toolbar then
		a warning is printed unless the -q switch is used.[br]
		If the -i switch is specified the <action> is interpreted as zero based
		index of the action to be removed (instead of the action name).[br]
		Please note that this is an expensive operation if the toolbar is visible (basically
		the toolbar needs to be cleared and filled back again). It is a good idea to hide
		the toolbar before removing items.[br]
	@seealso:
*/

static bool toolbar_kvs_cmd_removeitem(KviKvsModuleCommandCall * c)
{
	QString szId;
	KviKvsVariant * pvAction;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("id", KVS_PT_NONEMPTYSTRING, 0, szId)
	KVSM_PARAMETER("action", KVS_PT_VARIANT, 0, pvAction)
	KVSM_PARAMETERS_END(c)

	KviCustomToolBarDescriptor * d = KviCustomToolBarManager::instance()->find(szId);

	if(!pvAction)
	{
		if(!c->switches()->find('q', "quiet"))
			c->error(__tr2qs("No action name/index specified")); // syntax error
		return false;
	}

	if(!d)
	{
		if(!c->switches()->find('q', "quiet"))
			c->warning(__tr2qs("The specified toolbar doesn't exist"));
		return true;
	}

	if(c->switches()->find('i', "index"))
	{
		kvs_int_t iAction;
		if(!pvAction->asInteger(iAction))
		{
			if(!c->switches()->find('q', "quiet"))
				c->warning(__tr2qs("The action parameter didn't evaluate to an index"));
		}
		else
		{
			if(!d->removeAction(iAction))
			{
				if(!c->switches()->find('q', "quiet"))
					c->warning(__tr2qs("The toolbar doesn't contain the specified item"));
			}
		}
	}
	else
	{
		QString szAction;
		pvAction->asString(szAction);
		if(!d->removeAction(szAction))
		{
			if(!c->switches()->find('q', "quiet"))
				c->warning(__tr2qs("The toolbar doesn't contain the specified item"));
		}
	}

	return true;
}

/*
	@doc: toolbar.additem
	@type:
		command
	@title:
		toolbar.additem
	@keyterms:
		Adding toolbar items
	@short:
		Adds a new item to a toolbar
	@syntax:
		toolbar.additem [-q] <toolbarid:string> <action:string>
	@switches:
		!sw: -q | --quiet
		Run quietly
	@description:
		Adds the specified <action> from the toolbar with the specified <toolbarid>.[br]
		If no such action exists this command does nothing (beside printing a warning unless -q is used).[br]
		If the <toolbar> parameter doesn't identify an existing toolbar then
		a warning is printed unless the -q switch is used.[br]
	@examples:
		[example]
			toolbar.create test $tr("My Toolbar")
			toolbar.additem test connect
			toolbar.additem test separator
			toolbar.additem test optionsdialog
			toolbar.show test
		[/example]
	@seealso:
*/

static bool toolbar_kvs_cmd_additem(KviKvsModuleCommandCall * c)
{
	QString szId;
	QString szAction;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("id", KVS_PT_NONEMPTYSTRING, 0, szId)
	KVSM_PARAMETER("action", KVS_PT_NONEMPTYSTRING, 0, szAction)
	KVSM_PARAMETERS_END(c)

	KviCustomToolBarDescriptor * d = KviCustomToolBarManager::instance()->find(szId);

	if(!d)
	{
		if(!c->switches()->find('q', "quiet"))
			c->warning(__tr2qs("The specified toolbar doesn't exist"));
		return true;
	}

	if(!d->addAction(szAction))
	{
		if(!c->switches()->find('q', "quiet"))
			c->warning(__tr2qs("The action \"%Q\" doesn't exist"), &szAction);
	}

	return true;
}

/*
	@doc: toolbar.items
	@type:
		function
	@title:
		$toolbar.items
	@short:
		Returns the list of items in a specified toolbar
	@syntax:
		<array> $toolbar.items(<toolbarid:string>)
	@description:
		Returns the array of items in the specified toolbar.
		If the toolbar doesn't exist then an empty list is returned.
	@seealso:
*/

static bool toolbar_kvs_fnc_items(KviKvsModuleFunctionCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("id", KVS_PT_NONEMPTYSTRING, 0, szId)
	KVSM_PARAMETERS_END(c)

	KviCustomToolBarDescriptor * d = KviCustomToolBarManager::instance()->find(szId);

	if(!d)
	{
		c->warning(__tr2qs("The specified toolbar doesn't exist"));
		return true;
	}

	KviKvsArray * a = new KviKvsArray();

	KviPointerHashTableIterator<QString, KviCustomToolBarDescriptor> it(*(KviCustomToolBarManager::instance()->descriptors()));

	kvs_uint_t id = 0;

	for(QString * s = d->actions()->first(); s; s = d->actions()->next())
	{
		a->set(id, new KviKvsVariant(*s));
		id++;
		++it;
	}

	c->returnValue()->setArray(a);
	return true;
}

static bool toolbar_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m, "create", toolbar_kvs_cmd_create)
	KVSM_REGISTER_SIMPLE_COMMAND(m, "clear", toolbar_kvs_cmd_clear)
	KVSM_REGISTER_SIMPLE_COMMAND(m, "destroy", toolbar_kvs_cmd_destroy)
	KVSM_REGISTER_SIMPLE_COMMAND(m, "additem", toolbar_kvs_cmd_additem)
	KVSM_REGISTER_SIMPLE_COMMAND(m, "remove", toolbar_kvs_cmd_removeitem)
	KVSM_REGISTER_SIMPLE_COMMAND(m, "show", toolbar_kvs_cmd_show)
	KVSM_REGISTER_SIMPLE_COMMAND(m, "hide", toolbar_kvs_cmd_hide)

	KVSM_REGISTER_FUNCTION(m, "exists", toolbar_kvs_fnc_exists)
	KVSM_REGISTER_FUNCTION(m, "isVisible", toolbar_kvs_fnc_isVisible)
	KVSM_REGISTER_FUNCTION(m, "list", toolbar_kvs_fnc_list)
	KVSM_REGISTER_FUNCTION(m, "items", toolbar_kvs_fnc_items)

	return true;
}

static bool toolbar_module_cleanup(KviModule *)
{
	return true;
}

KVIRC_MODULE(
    "Toolbar",                                                      // module name
    "4.0.0",                                                        // module version
    "Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
    "Interface to the scriptable toolbars",
    toolbar_module_init,
    0,
    0,
    toolbar_module_cleanup,
    0)
