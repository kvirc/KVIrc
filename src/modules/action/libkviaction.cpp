//=============================================================================
//
//   File : libkviaction.cpp
//   Creation date : Tue 7 Dec 00:05:59 2002 GMT by Szymon Stefanek
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
#include "KviAction.h"
#include "KviActionManager.h"
#include "KviLocale.h"
#include "KviQString.h"
#include "KviCommandFormatter.h"
#include "KviQString.h"
#include "KviError.h"
#include "kvi_out.h"
#include "KviIconManager.h"
#include "KviControlCodes.h"
#include "KviKvsUserAction.h"
#include "KviKvsScript.h"

/*
	@doc: action.list
	@type:
		command
	@title:
		action.list
	@short:
		Lists the available actions
	@syntax:
		action.list
	@description:
		Lists the currently defined actions: nothing more, nothing less.
*/

static bool action_kvs_cmd_list(KviKvsModuleCommandCall * c)
{
	KviWindow * pOut = c->window();

	KviPointerHashTableIterator<QString, KviAction> it(*(KviActionManager::instance()->actions()));
	while(KviAction * a = it.current())
	{
		if(a->isKviUserActionNeverOverrideThis())
			pOut->output(KVI_OUT_VERBOSE, __tr2qs("%cCore action: %Q"), KviControlCodes::Bold, &(a->name()));
		else
			pOut->output(KVI_OUT_VERBOSE, __tr2qs("%cUser action: %Q"), KviControlCodes::Bold, &(a->name()));
		pOut->output(KVI_OUT_VERBOSE, __tr2qs("Label: %Q"), &(a->visibleName()));
		pOut->output(KVI_OUT_VERBOSE, __tr2qs("Category: %Q"), &(a->category()->visibleName()));
		pOut->output(KVI_OUT_VERBOSE, __tr2qs("Description: %Q"), &(a->description()));
		pOut->output(KVI_OUT_VERBOSE, "  "); // spacer
		++it;
	}
	return true;
}

/*
	@doc: action.trigger
	@type:
		command
	@title:
		action.trigger
	@short:
		Triggers an action
	@syntax:
		action.trigger [-q] <name:string>
	@switches:
		!sw: -q | --quiet
		Run in quiet mode (do not print any warnings)
	@description:
		Triggers the action specified by <name>, if that action exists and is enabled.
		If the -q switch is specified the command runs in quiet mode and
		does not print warnings if the action is not defined or disabled.
	@examples:
		[example]
			action.trigger connect
		[/example]
*/

static bool action_kvs_cmd_trigger(KviKvsModuleCommandCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("name", KVS_PT_NONEMPTYSTRING, 0, szName)
	KVSM_PARAMETERS_END(c)

	KviAction * a = KviActionManager::instance()->getAction(szName);
	if(a)
	{
		if(a->isEnabled())
		{
			a->activate();
		}
		else
		{
			if(!c->switches()->find('q', "quiet"))
				c->warning(__tr2qs("The action \"%1\" is disabled").arg(szName));
		}
	}
	else
	{
		c->warning(__tr2qs("The action \"%1\" doesn't exist").arg(szName));
	}

	return true;
}

/*
	@doc: action.enable
	@type:
		command
	@title:
		action.enable
	@short:
		Enables an action
	@syntax:
		action.enable [-q] <name:string>
	@switches:
		!sw: -q | --quiet
		Run quietly, do not print any warnings
	@description:
		Enables the action specified by <name>, if that action exists.
		If the action was previously enabled then this command does nothing.
		otherwise all the toolbar buttons and menu items belonging to this
		action will get enabled.[br]
		Since [cmd]action.create[/cmd] command allows specifying contexts
		for actions and the action engine is able to enable / disable the actions
		automatically on certain events you will probably never need to use this command.[br]
		If the -q switch is specified the command runs in quiet mode and
		does not print warnings if the action is not defined.
	@seealso:
		[cmd]action.disable[/cmd]
*/

static bool action_kvs_cmd_enable(KviKvsModuleCommandCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("name", KVS_PT_NONEMPTYSTRING, 0, szName)
	KVSM_PARAMETERS_END(c)

	KviAction * a = KviActionManager::instance()->getAction(szName);
	if(a)
	{
		if(!a->isEnabled())
			a->setEnabled(true);
	}
	else
	{
		if(!c->switches()->find('q', "quiet"))
			c->warning(__tr2qs("The action \"%1\" doesn't exist").arg(szName));
	}

	return true;
}

/*
	@doc: action.disable
	@type:
		command
	@title:
		action.disable
	@short:
		Disables an action
	@syntax:
		action.disable [-q] [-f] <name:string>
	@switches:
		!sw: -q | --quiet
		Run quietly, do not print any warnings
	@description:
		Disables the action specified by <name>, if that action exists.
		If the action was previously disabled then this command does nothing.
		otherwise all the toolbar buttons and menu items belonging to this
		action will get disabled.[br]
		Since [cmd]action.create[/cmd] command allows specifying contexts
		for actions and the action engine is able to enable/disable the actions
		automatically on certain events you will probably never need to use this command.
		For the same reason the action may get automatically re-enabled when
		one of the enabling events specified at [cmd]action.create[/cmd] time happens.
		If the -q switch is specified the command runs in quiet mode and
		does not print warnings if the action is not defined.
	@seealso:
		[cmd]action.enable[/cmd]
*/

static bool action_kvs_cmd_disable(KviKvsModuleCommandCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("name", KVS_PT_NONEMPTYSTRING, 0, szName)
	KVSM_PARAMETERS_END(c)

	KviAction * a = KviActionManager::instance()->getAction(szName);
	if(a)
	{
		if(a->isEnabled())
			a->setEnabled(false);
	}
	else
	{
		if(!c->switches()->find('q', "quiet"))
			c->warning(__tr2qs("The action \"%1\" doesn't exist").arg(szName));
	}

	return true;
}

/*
	@doc: action.destroy
	@type:
		command
	@title:
		action.destroy
	@short:
		Destroy an action
	@syntax:
		action.destroy [-q] <name:string>
	@switches:
		!sw: -q | --quiet
		Run quietly, do not print any warnings
	@description:
		Destroys the action specified by <name>, if that action exists.[br]
		Destroying an action will remove it from any toolbar or popup.[br]
		If the -q switch is specified the command runs in quiet mode and
		does not print warnings if the action is not defined.
		Please note that you CAN'T destroy core actions, you can destroy
		only script actions that have been defined by the user (or other scripts).
	@seealso:
		[comment]This will trigger a warning and [b]not[/b] work[/comment]
		action.destroy connect
*/

static bool action_kvs_cmd_destroy(KviKvsModuleCommandCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("name", KVS_PT_NONEMPTYSTRING, 0, szName)
	KVSM_PARAMETERS_END(c)

	KviAction * a = KviActionManager::instance()->getAction(szName);
	if(a)
	{
		if(a->isKviUserActionNeverOverrideThis())
		{
			a->suicide();
		}
		else
		{
			if(!c->switches()->find('q', "quiet"))
				c->warning(__tr2qs("The action \"%1\" is a core action and can't be destroyed").arg(szName));
		}
	}
	else
	{
		if(!c->switches()->find('q', "quiet"))
			c->warning(__tr2qs("The action \"%1\" doesn't exist").arg(szName));
	}

	return true;
}

/*
	@doc: action.create
	@type:
		command
	@title:
		action.create
	@short:
		Defines a new action
	@syntax:
		action.create [-i] [-c] [-l] [-w=<window_types:string>] [-s] [-t=<category:string>] (<name:string>,<visible label:string>,<description:string>,<big_icon_id:string>[,<small_icon_id:string>])
		{
			<action body>
		}
	@switches:
		!sw: -i | --bind-to-context
		Specifies that the action needs an IRC context to be executed.
		When the current window does not belong to an IRC context
		the action is automatically disabled
		[br]
		!sw: -c | --bind-to-connection
		Specifies that the action needs an IRC connection to be executed.
		When the current window has no associated IRC connection
		the action is automatically disabled.
		This switch implies -i.
		[br]
		!sw: -l | --enable-at-login
		Specifies that the action needs to be enabled at login time, that is
		when a link to the server has been established but the login
		operations haven't been carried out yet (and thus there is no real IRC connection).
		This switch requires -c to work.
		[br]
		!sw: -w=<window_types> | --window-types=<window_type>
		Causes the action to be enabled only when the active window
		is one of the specified types. <window_types> may be any combination
		of the letters [b]q[/b] (query), [b]c[/b] (channel), [b]x[/b] (console), [b]d[/b] (dccchat).
		If this switch is omitted then the action is enabled in all the windows.[br]
		[br]
		!sw: -s | --selected-only
		Specifies that the action will be activated only if the active window
		has selected users in the userlist. This switch requires -w with a combination
		of flags [b]q[/b], [b]c/b] and [b]x[/b] (it doesn't work for DCC chat).
		[br]
		!sw: -t=<category> | --category=<category>
		Causes the action to belong to the specified category.
		<category> can be one of [i]IRC[/i], [i]scripting[/i], [i]settings[/i], [i]GUI[/i], [i]channel[/i], [i]tools[/i] and [i]generic[/i].
		If this switch is omitted the [i]generic[/i] category is automatically assumed.
		The actions failing in the [i]tools[/i] category will appear in the [i]Tools[/i] KVIrc menu too.
		[br]
		!sw: -k=<key sequence> | --key-sequence=<key sequence>
		Specifies that the action will be activated also by the <key sequence>
		which is a string formed from up to four keyboard codes separated by
		commas optionally combined with the modifiers [i]Alt[/i], [i]Ctrl[/i], [i]Shift[/i] and [i]Meta[/i].[br]
		Examples of such sequences are: [i]Ctrl+X", [i]Ctrl+Alt+F[/i] or [i]Ctrl+X,Alt+Space[/i].
	@description:
		[br]
		Defines a new script action.
		[br]
		Each action has an unique <name> that must not collide with any core action
		(i.e. don't use the [i]KVIrc.[/i] prefix).
		At any time you can check [cmd]action.list[/cmd] to verify that no core action
		is already using your <name>. If the <name> was already used for a script action
		then this action is simply replaced by the new one.
		[br]
		Each action has an associated <visible label> that is the name meant to be presented to the user,
		possibly even translated. This label will appear on the toolbar buttons, in the tooltips and
		in the popup menu items. The string will be evaluated just before the actions is displayed
		so the eventual runtime translation will fetch from the correct language catalogue.
		[br]
		<description> is the descriptive text that will be displayed in the action choice dialog
		(and maybe in other places).
		The string will be evaluated just before the actions is displayed
		so the eventual runtime translation will fetch from the correct language catalogue.
		[br]]
		<big_icon_id> is the [doc:image_id]image identifier[/doc] of the icon that will
		appear on the toolbar buttons and in the action choice dialog.
		[br]
		<small_icon_id> is optional and is the [doc:image_id]image identifier[/doc] of the icon
		that will appear in the menu items. Since menu items can be also iconless then
		this parameter is optional.
		[br]
		<action body> is the callback code snippet that will be triggered when this action is activated
		either by the means of [cmd]action.trigger[/cmd], a toolbar button or a menu item selection.
		An empty <action body> causes this command to behave like [cmd]action.destroy[/cmd] <name>.
		[br]
	@seealso:
		[cmd]action.destroy[/cmd], [cmd]action.trigger[/cmd]
	@examples:
		[example]
		[/example]
*/

static bool action_kvs_cmd_create(KviKvsModuleCallbackCommandCall * c)
{
	QString szName, szVisibleText, szDescription, szBigIconId, szSmallIconId;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("name", KVS_PT_NONEMPTYSTRING, 0, szName)
	KVSM_PARAMETER_IGNORED("visible_label")
	KVSM_PARAMETER_IGNORED("description")
	KVSM_PARAMETER("big_icon_id", KVS_PT_NONEMPTYSTRING, 0, szBigIconId)
	KVSM_PARAMETER("small_icon_id", KVS_PT_STRING, KVS_PF_OPTIONAL, szSmallIconId)
	KVSM_PARAMETERS_END(c)

	if(!(c->getParameterCode(1, szVisibleText) && c->getParameterCode(2, szDescription)))
	{
		c->error(__tr2qs("Internal error: call a head-shrinker"));
		return false;
	}

	QString szCategory, szWindows, szKeySequence;

	int iFlags = 0;

	if(c->switches()->find('i', "bind-to-context"))
		iFlags |= KviAction::NeedsContext;
	if(c->switches()->find('c', "bind-to-connection"))
		iFlags |= KviAction::NeedsConnection | KviAction::NeedsContext;
	if(c->switches()->find('l', "enable-at-login"))
	{
		if(iFlags & KviAction::NeedsConnection)
			iFlags |= KviAction::EnableAtLogin;
		else
			c->warning(__tr2qs("The switch -l requires -c"));
	}
	c->switches()->getAsStringIfExisting('t', "category", szCategory);
	if(szCategory.isEmpty())
		szCategory = "generic";
	c->switches()->getAsStringIfExisting('w', "window-types", szWindows);
	if(!szWindows.isEmpty())
	{
		if(szWindows.indexOf('c', Qt::CaseInsensitive) != -1)
			iFlags |= KviAction::WindowChannel;
		if(szWindows.indexOf('x', Qt::CaseInsensitive) != -1)
			iFlags |= KviAction::WindowConsole;
		if(szWindows.indexOf('d', Qt::CaseInsensitive) != -1)
			iFlags |= KviAction::WindowDccChat;
		if(szWindows.indexOf('q', Qt::CaseInsensitive) != -1)
			iFlags |= KviAction::WindowQuery;
	}
	if(c->switches()->find('s', "enable-on-selected"))
	{
		if(iFlags & (KviAction::WindowChannel | KviAction::WindowConsole | KviAction::WindowQuery))
			iFlags |= KviAction::WindowOnlyIfUsersSelected;
		else
			c->warning(__tr2qs("The switch -s requires -w with a combination of flags 'c','x' and 'q'"));
	}
	c->switches()->getAsStringIfExisting('k', "key-sequence", szKeySequence);

	KviAction * old = KviActionManager::instance()->getAction(szName);
	if(old)
	{
		if(old->isKviUserActionNeverOverrideThis())
			old->suicide();
		else
		{
			c->warning(__tr2qs("The action \"%1\" is already defined as core action and can't be overridden").arg(szName));
			return false;
		}
	}

	QString szCmd = c->callback()->code();

	if(szCmd.isEmpty())
	{
		// just killed it :)
		return true;
	}

	int iOldFlags = iFlags;
	iFlags = KviAction::validateFlags(iFlags);
	if(iFlags != iOldFlags)
		qDebug("action.validate has provided invalid flags: %d fixed to %d", iOldFlags, iFlags);

	KviKvsUserAction * a = KviKvsUserAction::createInstance(KviActionManager::instance(),
	    szName, szCmd, szVisibleText,
	    szDescription, szCategory, szBigIconId,
	    szSmallIconId, iFlags, szKeySequence);

	KviActionManager::instance()->registerAction(a);

	return true;
}

/*
	@doc: action.exists
	@type:
		function
	@title:
		$action.exists
	@short:
		Checks if an action is currently defined
	@syntax:
		<boolean> $action.exists(<name:string>)
	@description:
		Returns [b]1[/b] if the action with the specified <name> is currently defined
		and [b]0[/b] otherwise.
*/

static bool action_kvs_fnc_exists(KviKvsModuleFunctionCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("action_name", KVS_PT_NONEMPTYSTRING, 0, szName)
	KVSM_PARAMETERS_END(c)
	c->returnValue()->setBoolean(KviActionManager::instance()->getAction(szName) ? true : false);
	return true;
}

/*
	@doc: action.isEnabled
	@type:
		function
	@title:
		$action.isEnabled
	@short:
		Checks if an action is currently enabled.
	@syntax:
		<boolean> $action.isEnabled(<name:string>)
	@description:
		Returns [b]1[/b] if the action with the specified <name> is currently enabled
		and [b]0[/b] otherwise. If the action does not exist the function returns [b]0[/b].
*/

static bool action_kvs_fnc_isEnabled(KviKvsModuleFunctionCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("action_name", KVS_PT_NONEMPTYSTRING, 0, szName)
	KVSM_PARAMETERS_END(c)
	KviAction * a = KviActionManager::instance()->getAction(szName);
	if(a)
		c->returnValue()->setBoolean(a->isEnabled());
	else
		c->returnValue()->setBoolean(false);
	return true;
}

static bool action_module_init(KviModule * m)
{
	// setlabel, $label, $position, move, $itempos, $itemexists, $itemtype
	KVSM_REGISTER_SIMPLE_COMMAND(m, "list", action_kvs_cmd_list);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "trigger", action_kvs_cmd_trigger);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "enable", action_kvs_cmd_enable);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "disable", action_kvs_cmd_disable);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "destroy", action_kvs_cmd_destroy);

	KVSM_REGISTER_CALLBACK_COMMAND(m, "create", action_kvs_cmd_create);

	KVSM_REGISTER_FUNCTION(m, "exists", action_kvs_fnc_exists);
	KVSM_REGISTER_FUNCTION(m, "isEnabled", action_kvs_fnc_isEnabled);
	return true;
}

static bool action_module_cleanup(KviModule *)
{
	return true;
}

KVIRC_MODULE(
    "action",                                                       // module name
    "4.0.0",                                                        // module version
    "Copyright (C) 2004 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
    "Interface to the system actions",
    action_module_init,
    0,
    0,
    action_module_cleanup,
    0)
