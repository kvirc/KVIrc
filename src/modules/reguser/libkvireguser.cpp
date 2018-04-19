//=============================================================================
//
//   File : libkvireguser.cpp
//   Creation date : Fri Dec 01 2000 14:53:10 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "RegisteredUserEntryDialog.h"
#include "RegistrationWizard.h"
#include "KviIrcConnection.h"
#include "RegisteredUsersDialog.h"

#include "KviModule.h"
#include "KviRegisteredUserDataBase.h"
#include "KviIrcUserDataBase.h"
#include "kvi_out.h"
#include "KviControlCodes.h"
#include "KviWindow.h"
#include "KviLocale.h"
#include "KviApplication.h"
#include "KviWindow.h"
#include "KviMainWindow.h"
#include "KviPointerList.h"
#include "KviIrcMask.h"

//#warning "$reguser.matches..."
//#warning "$reguser.clear"

//#warning "THIS MODULE HAS TO BE REVISED (notify list consistency!!!)"

// KviApplication.cpp

extern KVIRC_API KviRegisteredUserDataBase * g_pRegisteredUserDataBase;

KviPointerList<RegistrationWizard> * g_pRegistrationWizardList = nullptr;

RegisteredUsersDialog * g_pRegisteredUsersDialog = nullptr;

/*
	@doc: reguser
	@type:
		module
	@short:
		Interface to the registered users database
	@title:
		The reguser module
	@body:
		The reguser module is the scripting interface to the [doc:registered_users]registered users database[/doc].[br]
		It provides the following set of commands:[br]
		[cmd]reguser.add[/cmd]: adds a user entry to the database[br]
		[cmd]reguser.addmask[/cmd]: adds a user's mask to the database[br]
		[cmd]reguser.remove[/cmd]: removes a user entry from the database[br]
		[fnc]$reguser.list[/fnc]: lists the entries[br]
		[cmd]reguser.setproperty[/cmd]: sets a property associated to an entry[br]
		[cmd]reguser.edit[/cmd]: shows an editor for a user entry
		[fnc]$reguser.match[/fnc](): finds an entry by matching masks[br]
		[fnc]$reguser.exactMatch[/fnc](): finds an entry by comparing masks[br]
		[fnc]$reguser.property[/fnc](): finds a property associated to an entry[br]
		[fnc]$reguser.matchProperty[/fnc](): shortcut for [fnc]$reguser.property[/fnc]([fnc]$reguser.match[/fnc]())[br]
		[fnc]$reguser.mask[/fnc](): returns the registration masks for an entry
*/

/*
	@doc: reguser.edit
	@type:
		command
	@title:
		reguser.edit
	@keyterms:
		registering users
	@short:
		Opens the registeredusers dialog
	@syntax:
		reguser.edit [-t]
	@switches:
		!sw: -t | --toplevel
		the editor is created as a toplevel window (dialog)
	@description:
		Shows the registered user database editor.[br]
		If the [-t] switch is passed, the editor is created as a toplevel window (dialog), otherwise
		it is created as part of the current main window.[br]
	@examples:
	@seealso:
		[module:reguser]Registered users database interface[/module],
		[doc:registered_users]Registered users database[/doc],
		[cmd]reguser.add[/cmd],
		[cmd]reguser.remove[/cmd],
		[cmd]reguser.addmask[/cmd],
		[fnc]$reguser.list[/fnc]
*/

static bool reguser_kvs_cmd_edit(KviKvsModuleCommandCall * c)
{
	if(g_pRegisteredUsersDialog)
	{
		if(c->hasSwitch('t', "toplevel"))
		{
			if(g_pRegisteredUsersDialog->parent())
			{
				g_pRegisteredUsersDialog->setParent(nullptr);
			}
		}
		else
		{
			if(g_pRegisteredUsersDialog->parent() != g_pMainWindow->splitter())
			{
				g_pRegisteredUsersDialog->setParent(g_pMainWindow->splitter());
			}
		}
	}
	else
	{
		if(c->hasSwitch('t', "toplevel"))
		{
			g_pRegisteredUsersDialog = new RegisteredUsersDialog(nullptr);
		}
		else
		{
			g_pRegisteredUsersDialog = new RegisteredUsersDialog(g_pMainWindow->splitter());
		}
	}
	g_pRegisteredUsersDialog->show();
	g_pRegisteredUsersDialog->raise();
	g_pRegisteredUsersDialog->setFocus();
	return true;
}

/*
	@doc: reguser.add
	@type:
		command
	@title:
		reguser.add
	@keyterms:
		registering users
	@short:
		Registers a user
	@syntax:
		reguser.add [-r] [-f] [-q] [-g=group] <name> <mask>
	@description:
		Adds a new entry with the specified <name> to the database.[br]
		If the database contains an entry with the same <name>, this command just prints
		a warning and returns.[br]
		If no [mask] is given the new entry has no registration masks: so it can't be "matched" yet. You have to add
		at least one mask with [cmd]reguser.addmask[/cmd].
		If [mask] is given, then it is added to the entry mask list.[br]
		The <name> parameter may contain any character: even spaces are allowed (obviously you have to
		use quotes in that case).[br]
		If the '-r' switch is given, the new entry replaces any previous one with the same <name> (the old entry is removed).[br]
		If the '-f' switch is given, and there is an existing entry with the same name, no warning is printed
		and the old entry is treated just like it has been just added (thus the [mask] is eventually added to its mask list).[br]
		The '-q' switch causes the command to run in "quiet" mode and print no warning.[br]
	@examples:
		[example]
			reguser.add Pragma
		[/example]
	@seealso:
		[module:reguser]Registered users database interface[/module],
		[doc:registered_users]Registered users database[/doc],
		[cmd]reguser.remove[/cmd],
		[cmd]reguser.addmask[/cmd],
		[fnc]$reguser.list[/fnc]
*/

static bool reguser_kvs_cmd_add(KviKvsModuleCommandCall * c)
{
	QString szName, szMask;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("name", KVS_PT_STRING, 0, szName)
	KVSM_PARAMETER("mask", KVS_PT_STRING, 0, szMask)
	KVSM_PARAMETERS_END(c)
	if(szName.isEmpty())
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs_ctx("No name specified", "register"));
		return true;
	}

	if(c->hasSwitch('r', "replace"))
		g_pRegisteredUserDataBase->removeUser(szName);

	KviRegisteredUser * u = g_pRegisteredUserDataBase->addUser(szName);

	if(!u)
	{
		if(c->hasSwitch('f', "force"))
		{
			u = g_pRegisteredUserDataBase->findUserByName(szName);
		}
		else
		{
			if(!c->hasSwitch('q', "quiet"))
				c->warning(__tr2qs_ctx("User already registered: found exact name match", "register"));
		}
	}

	if(u)
	{
		if(c->hasSwitch('g', "group"))
		{
			QString group;
			c->switches()->getAsStringIfExisting('g', "group", group);
			u->setGroup(group);
		}
		if(!szMask.isEmpty())
		{
			KviIrcMask * m = new KviIrcMask(szMask);
			u = g_pRegisteredUserDataBase->addMask(u, m);
			if(u)
			{
				if(!c->hasSwitch('q', "quiet"))
					c->warning(__tr2qs_ctx("Mask %Q is already used to identify user %s", "register"), &szMask, u->name().toUtf8().data());
			}
		}
	}
	return true;
}

/*
	@doc: reguser.remove
	@type:
		command
	@title:
		reguser.remove
	@keyterms:
		unregistering users
	@short:
		Removes an entry from the regusers database
	@syntax:
		reguser.remove [-q] [-n] <name>
	@description:
		Removes the regusers database entry with the specified <name>.[br]
		If the -n switch is given, and there is a removed entry,
		this command will restart all the running "notify lists".
		If the -q switch is used, no warning is printed if the <name>
		does not identify an existing entry.
	@examples:
		[example]
			[cmd]reguser.add[/cmd] Pragma
			reguser.remove Pragma
		[example]
	@seealso:
		[module:reguser]Registered users database interface[/module],
		[doc:registered_users]Registered users database[/doc],
		[cmd]reguser.add[/cmd],
		[cmd]reguser.addmask[/cmd],
		[fnc]$reguser.list[/fnc]
*/

static bool reguser_kvs_cmd_remove(KviKvsModuleCommandCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("name", KVS_PT_STRING, 0, szName)
	KVSM_PARAMETERS_END(c)

	if(szName.isEmpty())
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs_ctx("No name specified", "register"));
		return true;
	}

	if(!(g_pRegisteredUserDataBase->removeUser(szName)))
	{
		c->warning(__tr2qs_ctx("User not found (%Q)", "register"), &szName);
	}
	else
	{
		if(c->hasSwitch('n', "restartnotifylists"))
			g_pApp->restartNotifyLists();
	}
	return true;
}

/*
	@doc: reguser.addmask
	@type:
		command
	@title:
		reguser.addmask
	@keyterms:
		registering users
	@short:
		Adds a mask to a registered user
	@syntax:
		reguser.addmask [-f] <name> <mask>
	@description:
		Adds a registration mask to the database entry with name <name>.[br]
		The <mask> has the common [doc:irc_masks]irc mask[/doc] format.[br]
		The only restriction on the <mask> is that it must be unique inside the
		regusers database: you can't match two users with the same mask (it would
		have rather undefined results).[br]
		If -f is used, KVIrc ensures that the mask is unique in the database
		and associates is to THIS registered user (breaking the previous associations).<br>
		Once at least one mask has been added, the user can be "matched"
		by using the [fnc]$reguser.match[/fnc]() function.[br]
		Each database entry can have an unlimited number of masks.[br]
	@examples:
		[example]
			[cmd]reguser.add[/cmd] "Szymon Stefanek"
			reguser.addmask "Szymon Stefanek" Pragma!*@*.it
		[/example]
	@seealso:
		[module:reguser]Registered users database interface[/module],
		[doc:registered_users]Registered users database[/doc],
		[cmd]reguser.add[/cmd],
		[cmd]reguser.remove[/cmd],
		[cmd]reguser.delmask[/cmd],
		[fnc]$reguser.list[/fnc],
		[fnc]$reguser.match[/fnc],
		[fnc]$reguser.exactMatch[/fnc]
*/

static bool reguser_kvs_cmd_addmask(KviKvsModuleCommandCall * c)
{
	QString szName;
	QString szMask;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("name", KVS_PT_STRING, 0, szName)
	KVSM_PARAMETER("mask", KVS_PT_STRING, 0, szMask)
	KVSM_PARAMETERS_END(c)

	if(szName.isEmpty())
	{
		c->warning(__tr2qs_ctx("No name specified", "register"));
		return true;
	}

	if(szMask.isEmpty())
	{
		c->warning(__tr2qs_ctx("No mask specified", "register"));
		return true;
	}

	KviRegisteredUser * u = g_pRegisteredUserDataBase->findUserByName(szName);
	if(!u)
	{
		c->warning(__tr2qs_ctx("User %Q not found", "register"), &szName);
		return true;
	}

	KviIrcMask * mk = new KviIrcMask(szMask);

	if(c->hasSwitch('f', "force"))
		g_pRegisteredUserDataBase->removeMask(*mk);

	u = g_pRegisteredUserDataBase->addMask(u, mk);

	if(u != nullptr)
		c->warning(__tr2qs_ctx("Mask %Q already used to identify user %Q", "register"), &szMask, &(u->name()));
	return true;
}

/*
	@doc: reguser.delmask
	@type:
		command
	@title:
		reguser.delmask
	@keyterms:
		registering users
	@short:
		Removes a mask from the regusers database
	@syntax:
		reguser.delmask <mask>
	@description:
		Removes a mask from the regusers database.[br]
		Since masks are unique in the database, you don't need to pass
		an <user> parameter: if an <user> entry has the <mask>, removing that <mask>
		will surely remove it from that <user> entry. (contorsions ?)[br]
	@examples:
		[example]
			[cmd]reguser.add[/cmd] "Szymon Stefanek"
			[cmd]reguser.addmask[/cmd] "Szymon Stefanek" Pragma!*@*.it
			reguser.delmask Pragma!*@*.it
			[fnc]$reguser.list[/fnc]
		[/example]
	@seealso:
		[module:reguser]Registered users database interface[/module],
		[doc:registered_users]Registered users database[/doc],
		[cmd]reguser.add[/cmd],
		[cmd]reguser.remove[/cmd],
		[cmd]reguser.addmask[/cmd],
		[fnc]$reguser.list[/fnc],
		[fnc]$reguser.match[/fnc],
		[fnc]$reguser.exactMatch[/fnc]
*/

static bool reguser_kvs_cmd_delmask(KviKvsModuleCommandCall * c)
{
	QString szMask;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("mask", KVS_PT_STRING, 0, szMask)
	KVSM_PARAMETERS_END(c)

	if(szMask.isEmpty())
	{
		c->warning(__tr2qs_ctx("No mask specified", "register"));
		return true;
	}

	KviIrcMask mk(szMask);
	if(!g_pRegisteredUserDataBase->removeMask(mk))
	{
		c->warning(__tr2qs_ctx("Mask %Q not found", "register"), &szMask);
	}

	return true;
}

/*
	@doc: reguser.setIgnoreEnabled
	@type:
		command
	@title:
		reguser.setIgnoreEnabled
	@keyterms:
		enable/disable ignoring of this user
	@short:
		Enable/disable ignoring of this user
	@syntax:
		reguser.setIgnoreEnabled [-q] <name:string> <isEnabled:bool>
	@description:

	@examples:
		[example]
			reguser.setproperty "Alexey" $true
		[/example]
	@seealso:
		[module:reguser]Registered users database interface[/module],
		[doc:registered_users]Registered users database[/doc],
		[fnc]$reguser.matchProperty[/fnc],
		[fnc]$reguser.property[/fnc]
*/

static bool reguser_kvs_cmd_setIgnoreEnabled(KviKvsModuleCommandCall * c)
{
	QString szName;
	bool bEnabled;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("name", KVS_PT_STRING, 0, szName)
	KVSM_PARAMETER("isEnabled", KVS_PT_BOOL, 0, bEnabled)
	KVSM_PARAMETERS_END(c)

	if(szName.isEmpty())
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs_ctx("No name specified", "register"));
		return true;
	}

	KviRegisteredUser * u = g_pRegisteredUserDataBase->findUserByName(szName);
	if(!u)
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs_ctx("User %Q not found", "register"), &szName);
	}
	else
	{
		u->setIgnoreEnabled(bEnabled);
	}
	return true;
}

/*
	@doc: reguser.setIgnoreFlags
	@type:
		command
	@title:
		reguser.setIgnoreFlags
	@keyterms:
		Sets ignore flags for registered user
	@short:
		Sets ignore flags for registered user
	@syntax:
		reguser.setIgnoreFlags [-p|--query] [-c|--channel] [-n|--notice] [-t|--ctcp] [-i|--invite] [-d|--dcc] [-h|--highlight] [-q] <name:string>
	@switches:
		!sw: -q | --quiet
		Don't warn if the specified user doesn't exist. Just continue silently.
		!sw: -p | --query
		Sets ignore for query messages and actions
		!sw: -c | --channel
		Sets ignore for channel messages and actions
		!sw: -n | --notice
		Sets ignore for notices
		!sw: -t | --ctcp
		Sets ignore for CTCP's
		!sw: -i | --invite
		Sets ignore for invites
		!sw: -d | --dcc
		Sets ignore for DCC's
		!sw: -h | --highlight
		Sets ignore for highlights

	@description:
		Sets ignore flags for registered user
	@examples:
		[example]
			reguser.setproperty "Alexey" $true
		[/example]
	@seealso:
		[module:reguser]Registered users database interface[/module],
		[doc:registered_users]Registered users database[/doc],
		[fnc]$reguser.matchProperty[/fnc],
		[fnc]$reguser.property[/fnc]
*/

static bool reguser_kvs_cmd_setIgnoreFlags(KviKvsModuleCommandCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("name", KVS_PT_STRING, 0, szName)
	KVSM_PARAMETERS_END(c)

	if(szName.isEmpty())
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs_ctx("No name specified", "register"));
		return true;
	}

	KviRegisteredUser * u = g_pRegisteredUserDataBase->findUserByName(szName);
	if(!u)
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs_ctx("User %Q not found", "register"), &szName);
	}
	else
	{
		int iIgnoreFlags = 0;
		if(c->hasSwitch('p', "query"))
			iIgnoreFlags |= KviRegisteredUser::Query;
		if(c->hasSwitch('c', "channel"))
			iIgnoreFlags |= KviRegisteredUser::Channel;
		if(c->hasSwitch('n', "notice"))
			iIgnoreFlags |= KviRegisteredUser::Notice;
		if(c->hasSwitch('t', "ctcp"))
			iIgnoreFlags |= KviRegisteredUser::Ctcp;
		if(c->hasSwitch('i', "invite"))
			iIgnoreFlags |= KviRegisteredUser::Invite;
		if(c->hasSwitch('d', "dcc"))
			iIgnoreFlags |= KviRegisteredUser::Dcc;
		if(c->hasSwitch('h', "highlight"))
			iIgnoreFlags |= KviRegisteredUser::Highlight;
		u->setIgnoreFlags(iIgnoreFlags);
	}
	return true;
}

/*
	@doc: reguser.getIgnoreFlags
	@type:
		function
	@title:
		$reguser.getIgnoreFlags
	@short:
		Returns ignore flags for registered user
	@syntax:
		$reguser.getIgnoreFlags(<name:string>)
	@description:
		Returns the ignore flags for registered user. Flags are string of letters:[br]
		p - query ignore[br]
		c - channel ignore[br]
		t - ctcp ignore[br]
		n - notice ignore[br]
		d - dcc ignore[br]
		i - invite ignore[br]
		h - highlight ignore[br]
	@seealso:
		[module:reguser]Registered users database interface[/module],
		[doc:registered_users]Registered users database[/doc],
		[cmd]reguser.add[/cmd],
		[cmd]reguser.remove[/cmd],
		[cmd]reguser.addmask[/cmd],
		[cmd]reguser.delmask[/cmd],
		[fnc]$reguser.list[/fnc],
		[fnc]$reguser.match[/fnc],
		[fnc]$reguser.matchProperty[/fnc]
*/

static bool reguser_kvs_fnc_getIgnoreFlags(KviKvsModuleFunctionCall * c)
{
	QString szName;
	QString szFlags;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("name", KVS_PT_STRING, 0, szName)
	KVSM_PARAMETERS_END(c)

	KviRegisteredUser * u = g_pRegisteredUserDataBase->findUserByName(szName);
	if(u)
	{
		if(u->ignoreFlags() & KviRegisteredUser::Query)
			szFlags += 'q';
		if(u->ignoreFlags() & KviRegisteredUser::Channel)
			szFlags += 'c';
		if(u->ignoreFlags() & KviRegisteredUser::Notice)
			szFlags += 'n';
		if(u->ignoreFlags() & KviRegisteredUser::Ctcp)
			szFlags += 't';
		if(u->ignoreFlags() & KviRegisteredUser::Invite)
			szFlags += 'i';
		if(u->ignoreFlags() & KviRegisteredUser::Dcc)
			szFlags += 'd';
		if(u->ignoreFlags() & KviRegisteredUser::Highlight)
			szFlags += 'h';
		c->returnValue()->setString(szFlags);
	}
	return true;
}

/*
	@doc: reguser.isIgnoreEnabled
	@type:
		function
	@title:
		$reguser.isIgnoreEnabled
	@short:
		Returns if ignore enabled registered user
	@syntax:
		$reguser.isIgnoreEnabled(<name:string>)
	@description:
		Returns if ignore enabled registered user
	@seealso:
		[module:reguser]Registered users database interface[/module],
		[doc:registered_users]Registered users database[/doc],
		[cmd]reguser.add[/cmd],
		[cmd]reguser.remove[/cmd],
		[cmd]reguser.addmask[/cmd],
		[cmd]reguser.delmask[/cmd],
		[fnc]$reguser.list[/fnc],
		[fnc]$reguser.match[/fnc],
		[fnc]$reguser.matchProperty[/fnc]
*/

static bool reguser_kvs_fnc_isIgnoreEnabled(KviKvsModuleFunctionCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("name", KVS_PT_STRING, 0, szName)
	KVSM_PARAMETERS_END(c)

	KviRegisteredUser * u = g_pRegisteredUserDataBase->findUserByName(szName);
	if(u)
	{
		c->returnValue()->setBoolean(u->ignoreEnabled());
	}
	return true;
}

/*
	@doc: reguser.setproperty
	@type:
		command
	@title:
		reguser.setproperty
	@keyterms:
		setting user properties
	@short:
		Sets a property of a registered user
	@syntax:
		reguser.setproperty [-n] [-a] [-q] <name> <property> [value]
	@description:
		Adds a property to the registered users database entry with name <name>.[br]
		The property <property> is set to the specified <value> or is unset if <value>
		is not given (or is an empty string).[br]
		If the '-n' switch is used, and the user entry has been found in the database,
		all the [doc:notify_list]notify lists[/doc] are restarted.[br]
		If the '-a' switch is used, and there are users on IRC currently matched
		by this reguser entry then their avatar is reset.
		This is useful when you're changing someone's notify property.[br]
		If the -q switch is used, the command runs in "quiet" mode and prints no warnings.[br]
	@examples:
		[example]
			[cmd]reguser.add[/cmd] "Szymon Stefanek"
			reguser.setproperty -n "Szymon Stefanek" notify Pragma
			[fnc]$reguser.list[/fnc]
		[/example]
	@seealso:
		[module:reguser]Registered users database interface[/module],
		[doc:registered_users]Registered users database[/doc],
		[fnc]$reguser.matchProperty[/fnc],
		[fnc]$reguser.property[/fnc]
*/

static bool reguser_kvs_cmd_setproperty(KviKvsModuleCommandCall * c)
{
	QString szName;
	QString szProperty;
	QString szValue;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("mask", KVS_PT_STRING, 0, szName)
	KVSM_PARAMETER("property", KVS_PT_STRING, 0, szProperty)
	KVSM_PARAMETER("value", KVS_PT_STRING, KVS_PF_OPTIONAL, szValue)
	KVSM_PARAMETERS_END(c)

	if(szName.isEmpty())
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs_ctx("No name specified", "register"));
		return true;
	}

	if(szProperty.isEmpty())
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs_ctx("No property specified", "register"));
		return true;
	}

	KviRegisteredUser * u = g_pRegisteredUserDataBase->findUserByName(szName);
	if(!u)
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs_ctx("User %Q not found", "register"), &szName);
	}
	else
	{
		u->setProperty(szProperty, szValue);
		if(c->hasSwitch('n', "restartnotifylists"))
			g_pApp->restartNotifyLists();
		if(c->hasSwitch('a', "resetavatar"))
			g_pApp->resetAvatarForMatchingUsers(u);
	}
	return true;
}
/*
	@doc: reguser.list
	@type:
		function
	@title:
		$reguser.list
	@short:
		Get a list of registered user masks
	@syntax:
		$reguser.list([mask])
	@description:
		Returns an array of the entries in the registered users database.[br]
		If <mask> is specified, only entries matching the <mask>
		and the entries with no registration masks are listed.[br]
		Please note that the <mask> is a wildcard string that will match
		wildcard strings... don't get messed with that :D[br]
	@seealso:
		[module:reguser]Registered users database interface[/module],
		[doc:registered_users]Registered users database[/doc],
		[cmd]reguser.add[/cmd],
		[cmd]reguser.remove[/cmd],
		[cmd]reguser.addmask[/cmd],
		[fnc]$reguser.list[/fnc],
		[fnc]$reguser.exactMatch[/fnc]
*/

static bool reguser_kvs_fnc_list(KviKvsModuleFunctionCall * c)
{
	QString szMask;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("user_mask", KVS_PT_STRING, KVS_PF_OPTIONAL, szMask)
	KVSM_PARAMETERS_END(c)

	KviIrcMask mask(szMask);
	KviKvsArray * pArray = new KviKvsArray();
	int aid = 0;

	KviPointerHashTable<QString, KviRegisteredUser> * d = g_pRegisteredUserDataBase->userDict();
	KviPointerHashTableIterator<QString, KviRegisteredUser> it(*d);

	while(KviRegisteredUser * u = it.current())
	{
		KviPointerList<KviIrcMask> * ml = u->maskList();
		if(u->matches(mask) || (ml->count() == 0))
		{
			pArray->set(aid, new KviKvsVariant(u->name()));
			aid++;
		}
		++it;
	}
	c->returnValue()->setArray(pArray);
	return true;
}

/*
	@doc: reguser.showlist
	@type:
		command
	@title:
		reguser.showlist
	@keyterms:
		registered users
	@short:
		Lists the registered users
	@syntax:
		reguser.showlist [mask]
	@description:
		Lists the entries in the registered users database.[br]
		If <mask> is specified, only entries matching the <mask>
		and the entries with no registration masks are listed.[br]
		Please note that <mask> is a wildcard string that will match
		wildcard strings... don't get messed with that :D[br]
	@seealso:
		[module:reguser]Registered users database interface[/module],
		[doc:registered_users]Registered users database[/doc],
		[cmd]reguser.add[/cmd],
		[cmd]reguser.remove[/cmd],
		[fnc]$reguser.match[/fnc],
		[fnc]$reguser.exactMatch[/fnc]
*/

static bool reguser_kvs_cmd_showlist(KviKvsModuleCommandCall * c)
{
	QString szMask;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("mask", KVS_PT_STRING, KVS_PF_OPTIONAL, szMask)
	KVSM_PARAMETERS_END(c)

	KviIrcMask mask(szMask);
	c->window()->outputNoFmt(KVI_OUT_SYSTEMMESSAGE, __tr2qs_ctx("Registered users database dump:", "register"));

	int count = 0;

	KviPointerHashTable<QString, KviRegisteredUser> * d = g_pRegisteredUserDataBase->userDict();
	KviPointerHashTableIterator<QString, KviRegisteredUser> it(*d);
	while(KviRegisteredUser * u = it.current())
	{
		KviPointerList<KviIrcMask> * ml = u->maskList();
		if(u->matches(mask) || (ml->count() == 0))
		{
			c->window()->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs_ctx("User: %c%Q", "register"), KviControlCodes::Bold, &(u->name()));

			if(ml->count() == 0)
			{
				c->window()->output(KVI_OUT_SYSTEMWARNING, __tr2qs_ctx("Warning: this user has no registration masks", "register"));
			}
			else
			{
				for(KviIrcMask * m = ml->first(); m; m = ml->next())
				{
					c->window()->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs_ctx("Mask: %Q!%Q@%Q", "register"), &(m->nick()), &(m->user()), &(m->host()));
				}
			}

			KviPointerHashTable<QString, QString> * pd = u->propertyDict();
			if(pd)
			{
				KviPointerHashTableIterator<QString, QString> pdit(*pd);
				while(pdit.current())
				{
					QString key = pdit.currentKey();
					c->window()->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs_ctx("Property: %Q=%Q", "register"), &(key), pdit.current());
					++pdit;
				}
			}
			else
				c->window()->outputNoFmt(KVI_OUT_SYSTEMMESSAGE, __tr2qs_ctx("No properties", "register"));
			count++;
		}
		++it;
	}

	c->window()->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs_ctx("Total: %d matching users (of %d in the database)", "register"), count, d->count());
	return true;
}

/*
	@doc: reguser.match
	@type:
		function
	@title:
		$reguser.match
	@short:
		Returns an entry in the registered user database
	@syntax:
		$reguser.match(<user_mask>)
	@description:
		Returns the name of the first entry matching <user_mask> in the registered user database.[br]
		The <user_mask> has the common [doc:irc_masks]irc mask[/doc] format but can't contain wildcards.[br]
	@seealso:
		[module:reguser]Registered users database interface[/module],
		[doc:registered_users]Registered users database[/doc],
		[cmd]reguser.add[/cmd],
		[cmd]reguser.remove[/cmd],
		[cmd]reguser.addmask[/cmd],
		[fnc]$reguser.list[/fnc],
		[fnc]$reguser.exactMatch[/fnc]
*/

static bool reguser_kvs_fnc_match(KviKvsModuleFunctionCall * c)
{
	QString szMask;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("user_mask", KVS_PT_STRING, 0, szMask)
	KVSM_PARAMETERS_END(c)
	KviIrcMask mask(szMask);
	//FIXME: it crashes kvirc
	//KviRegisteredUser * u = c->context()->connection()->userDataBase()->registeredUser(mask.nick(),mask.user(),mask.host());
	KviRegisteredUser * u = g_pRegisteredUserDataBase->findMatchingUser(mask.nick(), mask.user(), mask.host());
	if(u)
		c->returnValue()->setString(u->name());
	return true;
}

/*
	@doc: reguser.exactmatch
	@type:
		function
	@title:
		$reguser.exactMatch
	@short:
		Returns an entry in the registered user database
	@syntax:
		$reguser.exactMatch(<user_mask>)
	@description:
		Returns the name of the registered user database entry that has <user_mask> in its registration mask list.[br]
		The <user_mask> has the common [doc:irc_masks]wild irc mask[/doc] format and can contain wildcards.[br]
	@seealso:
		[module:reguser]Registered users database interface[/module],
		[doc:registered_users]Registered users database[/doc],
		[cmd]reguser.add[/cmd],
		[cmd]reguser.remove[/cmd],
		[cmd]reguser.addmask[/cmd],
		[fnc]$reguser.list[/fnc],
		[fnc]$reguser.match[/fnc]
*/

static bool reguser_kvs_fnc_exactMatch(KviKvsModuleFunctionCall * c)
{
	QString szMask;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("user_mask", KVS_PT_STRING, 0, szMask)
	KVSM_PARAMETERS_END(c)
	KviIrcMask mask(szMask);
	KviRegisteredUser * u = g_pRegisteredUserDataBase->findUserWithMask(mask);
	if(u)
		c->returnValue()->setString(u->name());
	return true;
}

/*
	@doc: reguser.mask
	@type:
		function
	@title:
		$reguser.mask
	@short:
		Returns the registration masks for a db entry
	@syntax:
		$reguser.mask(<name>[,<N>])
	@description:
		Returns the <N>th registration mask for the registered user database record identified
		by <name>. If <N> is not given, returns an array of registration masks for that entry.[br]
		If <name> does not identify a valid entry an empty string is returned;
		if <N> is out of range (or there are no masks at all), an empty string is returned.[br]
		<N> is a ZERO BASED index.[br]
	@examples:
		[example]
			[cmd]reguser.add[/cmd] Pragma
			[cmd]reguser.addmask[/cmd] Pragma Pragma!*@*.it
			[cmd]echo[/cmd] $reguser.mask(Pragma,0)
		[/example]
	@seealso:
		[module:reguser]Registered users database interface[/module],
		[doc:registered_users]Registered users database[/doc],
		[cmd]reguser.add[/cmd],
		[cmd]reguser.remove[/cmd],
		[cmd]reguser.addmask[/cmd],
		[cmd]reguser.delmask[/cmd],
		[fnc]$reguser.list[/fnc],
		[fnc]$reguser.match[/fnc],
		[fnc]$reguser.matchProperty[/fnc]
*/

static bool reguser_kvs_fnc_mask(KviKvsModuleFunctionCall * c)
{
	QString szName;
	QString szN;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("name", KVS_PT_STRING, 0, szName)
	KVSM_PARAMETER("N", KVS_PT_STRING, KVS_PF_OPTIONAL, szN)
	KVSM_PARAMETERS_END(c)

	KviRegisteredUser * u = g_pRegisteredUserDataBase->findUserByName(szName);
	if(u)
	{
		KviCString n = szName;
		if(n.hasData() && n.isUnsignedNum())
		{
			KviIrcMask * m = u->maskList()->at(n.toInt());
			if(m)
				c->returnValue()->setString(m->nick() + "!" + m->user() + "@" + m->host());
		}
		else
		{
			KviKvsArray * pArray = new KviKvsArray();
			int aid = 0;

			for(KviIrcMask * m = u->maskList()->first(); m; m = u->maskList()->next())
			{
				pArray->set(aid, new KviKvsVariant(QString(m->nick() + "!" + m->user() + "@" + m->host())));
				aid++;
			}
			c->returnValue()->setArray(pArray);
		}
	}
	return true;
}
/*
	@doc: reguser.property
	@type:
		function
	@title:
		$reguser.property
	@short:
		Returns a property of an entry in the registered user database
	@syntax:
		$reguser.property(<user_name>,<property_name>)
	@description:
		Returns the value of the property identified by <property_name> and
		bound to the entry identified by <user_name>.[br]
		If the property is not set or if there is no entry with <user_name> an empty string is returned.[br]
	@examples:
		[example]
			[cmd]reguser.add[/cmd] Pragma
			[cmd]reguser.addmask[/cmd] Pragma Pragma!*@*.it
			[cmd]reguser.setproperty[/cmd] Pragma isDev 1
			[cmd]echo[/cmd] $reguser.property(Pragma,isDev)
		[/example]
	@seealso:
		[module:reguser]Registered users database interface[/module],
		[doc:registered_users]Registered users database[/doc],
		[cmd]reguser.add[/cmd],
		[cmd]reguser.remove[/cmd],
		[cmd]reguser.addmask[/cmd],
		[fnc]$reguser.list[/fnc],
		[fnc]$reguser.match[/fnc],
		[fnc]$reguser.matchProperty[/fnc]
*/

static bool reguser_kvs_fnc_property(KviKvsModuleFunctionCall * c)
{
	QString szName;
	QString szProperty;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("user_name", KVS_PT_STRING, 0, szName)
	KVSM_PARAMETER("property_name", KVS_PT_STRING, KVS_PF_OPTIONAL, szProperty)
	KVSM_PARAMETERS_END(c)

	KviRegisteredUser * u = g_pRegisteredUserDataBase->findUserByName(szName);
	if(u)
	{
		QString tmp;
		u->getProperty(szProperty, tmp);
		c->returnValue()->setString(tmp);
	}
	return true;
}
/*
	@doc: reguser.matchproperty
	@type:
		function
	@title:
		$reguser.matchProperty
	@short:
		Returns a property of an entry in the registered user database
	@syntax:
		$reguser.matchProperty(<user_mask>,<property_name>)
	@description:
		Returns the value of the property identified by <property_name> and
		bound to the first entry matched by by <user_mask>.[br]
		If the property is not set, an empty string is returned.[br]
		If no entry matches <user_mask> this function returns an empty string
		and does not print any error.
		[example]
			%property = $reguser.matchProperty(<user_mask>,<property_name>)
		[/example]
		Is actually a shortcut for:
		[example]
			%tmp = [fnc]$reguser.match[/fnc](<user_mask>)
			if("%tmp" != "")%property = $reguser.property(%tmp,<property_name>)
		[/example]
	@seealso:
		[module:reguser]Registered users database interface[/module],
		[doc:registered_users]Registered users database[/doc],
		[cmd]reguser.add[/cmd],
		[cmd]reguser.remove[/cmd],
		[cmd]reguser.addmask[/cmd],
		[fnc]$reguser.list[/fnc],
		[fnc]$reguser.match[/fnc],
		[fnc]$reguser.property[/fnc]
*/

static bool reguser_kvs_fnc_matchProperty(KviKvsModuleFunctionCall * c)
{
	QString szMask;
	QString szProperty;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("user_mask", KVS_PT_STRING, 0, szMask)
	KVSM_PARAMETER("property_name", KVS_PT_STRING, KVS_PF_OPTIONAL, szProperty)
	KVSM_PARAMETERS_END(c)

	KviIrcMask mask(szMask);
	//FIXME: it crashes kvirc
	//KviRegisteredUser * u = c->context()->connection()->userDataBase()->registeredUser(mask.nick(),mask.user(),mask.host());
	KviRegisteredUser * u = g_pRegisteredUserDataBase->findMatchingUser(mask.nick(), mask.user(), mask.host());
	if(u)
	{
		QString tmp;
		u->getProperty(szProperty, tmp);
		c->returnValue()->setString(tmp);
	}
	return true;
}

/*
	@doc: reguser.wizard
	@type:
		command
	@title:
		reguser.wizard
	@keyterms:
		registering users, registration wizard
	@short:
		Shows a registration wizard dialog
	@syntax:
		reguser.wizard [mask]
	@description:
		Allows registering a user with an intuitive and easy to use interface.
		If [mask] is specified, it is used as initial mask in the dialog.
	@seealso:
		[module:reguser]Registered users database interface[/module],
		[doc:registered_users]Registered users database[/doc],
		[cmd]reguser.add[/cmd],
*/

static bool reguser_kvs_cmd_wizard(KviKvsModuleCommandCall * c)
{
	QString szMask;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("mask", KVS_PT_STRING, KVS_PF_OPTIONAL, szMask)
	KVSM_PARAMETERS_END(c)

	RegistrationWizard * w = new RegistrationWizard(szMask);
	w->show();
	return true;
}

static bool reguser_module_init(KviModule * m)
{
	g_pLocalRegisteredUserDataBase = nullptr;
	g_pRegistrationWizardList = new KviPointerList<RegistrationWizard>;
	g_pRegistrationWizardList->setAutoDelete(true);

	KVSM_REGISTER_SIMPLE_COMMAND(m, "add", reguser_kvs_cmd_add);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "remove", reguser_kvs_cmd_remove);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "addmask", reguser_kvs_cmd_addmask);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "delmask", reguser_kvs_cmd_delmask);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "edit", reguser_kvs_cmd_edit);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "showlist", reguser_kvs_cmd_showlist);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setproperty", reguser_kvs_cmd_setproperty);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "wizard", reguser_kvs_cmd_wizard);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setIgnoreEnabled", reguser_kvs_cmd_setIgnoreEnabled);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setIgnoreFlags", reguser_kvs_cmd_setIgnoreFlags);

	KVSM_REGISTER_FUNCTION(m, "match", reguser_kvs_fnc_match);
	KVSM_REGISTER_FUNCTION(m, "list", reguser_kvs_fnc_list);
	KVSM_REGISTER_FUNCTION(m, "exactMatch", reguser_kvs_fnc_exactMatch);
	KVSM_REGISTER_FUNCTION(m, "mask", reguser_kvs_fnc_mask);
	KVSM_REGISTER_FUNCTION(m, "property", reguser_kvs_fnc_property);
	KVSM_REGISTER_FUNCTION(m, "matchProperty", reguser_kvs_fnc_matchProperty);
	KVSM_REGISTER_FUNCTION(m, "getIgnoreFlags", reguser_kvs_fnc_getIgnoreFlags);
	KVSM_REGISTER_FUNCTION(m, "isIgnoreEnabled", reguser_kvs_fnc_isIgnoreEnabled);

	return true;
}

static bool reguser_module_cleanup(KviModule *)
{
	if(g_pRegisteredUsersDialog)
		delete g_pRegisteredUsersDialog;
	g_pRegisteredUsersDialog = nullptr;

	while(RegistrationWizard * w = g_pRegistrationWizardList->first())
		delete w;
	delete g_pRegistrationWizardList;
	g_pRegistrationWizardList = nullptr;

	// FIXME: UNREGISTER THE METAOBJECTS
	return true;
}

static bool reguser_module_can_unload(KviModule *)
{
	return ((g_pRegistrationWizardList->isEmpty()) && (g_pRegisteredUsersDialog == nullptr));
}

KVIRC_MODULE(
    "Reguser",                                                      // module name
    "4.0.0",                                                        // module version
    "Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
    "Script interface to the registered users database",
    reguser_module_init,
    reguser_module_can_unload,
    0,
    reguser_module_cleanup,
    "register")
