///////////////////////////////////////////////////////////////////////////////
//
//   File : libkviaddon.cpp
//   Creation date : Tue 31 Mar 01:02:12 2005 GMT by Szymon Stefanek
//
//   This toolbar is part of the KVirc irc client distribution
//   Copyright (C) 2005 Szymon Stefanek (pragma at kvirc dot net)
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
///////////////////////////////////////////////////////////////////////////////

#include "managementdialog.h"

#include "kvi_module.h"
#include "kvi_kvs_scriptaddonmanager.h"
#include "kvi_locale.h"
#include "kvi_qstring.h"
#include "kvi_parameterlist.h"
#include "kvi_cmdformatter.h"
#include "kvi_qstring.h"
#include "kvi_error.h"
#include "kvi_out.h"
#include "kvi_iconmanager.h"
#include "kvi_mirccntrl.h"
#include "kvi_config.h"
#include "kvi_sourcesdate.h"
#include "kvi_miscutils.h"

QRect g_rectManagementDialogGeometry(0,0,0,0);

/*
	@doc: addon.exists
	@type:
		function
	@title:
		$addon.exists
	@short:
		Checks if an addon is currently installed
	@syntax:
		<boolean> $addon.exists(<id:string>[,<version:string>])
	@description:
		Returns 1 if the addon with the specified <id> is currently installed
		and 0 otherwise. If <version> is specified then any addon with 
		a version lower than <version> is ignored (so you can effectively
		check if a greater or equal version is present).
*/

static bool addon_kvs_fnc_exists(KviKvsModuleFunctionCall * c)
{
	QString szId;
	QString szVersion;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("id",KVS_PT_NONEMPTYSTRING,0,szId)
		KVSM_PARAMETER("version",KVS_PT_STRING,KVS_PF_OPTIONAL,szVersion)
	KVSM_PARAMETERS_END(c)

	KviKvsScriptAddon * a = KviKvsScriptAddonManager::instance()->findAddon(szId);
	if(a)
	{
		if(szVersion.isEmpty())
		{
			c->returnValue()->setBoolean(true);
		} else {
			c->returnValue()->setBoolean(KviMiscUtils::compareVersions(a->version(),szVersion) < 0);
		}
	} else {
		c->returnValue()->setBoolean(false);
	}
	return true;
}

/*
	@doc: addon.version
	@type:
		function
	@title:
		$addon.version
	@short:
		Returns the version of an installed addon
	@syntax:
		<string> $addon.version(<id:string>)
	@description:
		Returns the version of the currently installed addon with the
		specified <id>. If the addon with the given <id> does not exist
		then an empty string is returned.
*/

static bool addon_kvs_fnc_version(KviKvsModuleFunctionCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("id",KVS_PT_NONEMPTYSTRING,0,szId)
	KVSM_PARAMETERS_END(c)

	KviKvsScriptAddon * a = KviKvsScriptAddonManager::instance()->findAddon(szId);
	if(a)
	{
		c->returnValue()->setString(a->version());
	} else {
		c->returnValue()->setNothing();
	}
	return true;
}

/*
	@doc: addon.list
	@type:
		command
	@title:
		addon.list
	@short:
		Lists the installed addons
	@syntax:
		addon.list
	@description:
		Lists the currently installed addons
	@seealso:
		[cmd]addon.register[/cmd]
*/

static bool addon_kvs_cmd_list(KviKvsModuleCommandCall * c)
{
	KviPointerHashTable<QString,KviKvsScriptAddon> * da = KviKvsScriptAddonManager::instance()->addonDict();

	int cnt = 0;
	KviPointerHashTableIterator<QString,KviKvsScriptAddon> it(*da);
	while(KviKvsScriptAddon * a = it.current())
	{
		c->window()->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("%cAddon id %Q, version %Q%c"),KVI_TEXT_BOLD,&(a->name()),&(a->version()),KVI_TEXT_BOLD);
		c->window()->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Name: %Q"),&(a->visibleName()));
		c->window()->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Description: %Q"),&(a->description()));
	
		++it;
		cnt++;
	}

	c->window()->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Total: %d addons installed"),cnt);
	return true;
}

/*
	@doc: addon.uninstall
	@type:
		command
	@title:
		addon.uninstall
	@short:
		Uninstalls an addon
	@syntax:
		addon.uninstall [-q] [-n] <id:string>
	@switches:
		!sw: -n | --no-callback
			Doesn't call the uninstall callback but only removes the
			registration entry.
		!sw: -q | --quiet
			Makes the command run quietly
	@description:
		Uninstalls the specified addon by executing its uninstall callback function.
		It also removes the addon's registration entry.
		If the -n switch is specified the the uninstall callback is not called,
		only the registration entry is removed.
	@seealso:
		[cmd]addon.register[/cmd]
*/

static bool addon_kvs_cmd_uninstall(KviKvsModuleCommandCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("name",KVS_PT_NONEMPTYSTRING,0,szName)
	KVSM_PARAMETERS_END(c)

	KviKvsScriptAddon * a = KviKvsScriptAddonManager::instance()->findAddon(szName);
	if(a)
	{
		if(!c->switches()->find('q',"quiet"))
			c->window()->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Uninstalling existing addon version %Q"),&(a->version()));

		// uninstall the existing version
		KviKvsScriptAddonManager::instance()->unregisterAddon(szName,c->window(),!c->switches()->find('n',"no-callback"));
	} else {
		if(!c->switches()->find('q',"quiet"))
			c->warning(__tr2qs("The addon \"%1\" does not exist").arg(szName));
	}

	return true;
}

/*
	@doc: addon.configure
	@type:
		command
	@title:
		addon.configure
	@short:
		Executes a addon's configuration callback
	@syntax:
		addon.configure [-q] <id:string>
	@switches:
		!sw: -q | --quiet
			Makes the command run quietly
	@description:
		Executes the configuration callback of the specified addon.
	@seealso:
		[cmd]addon.register[/cmd]
		[cmd]addon.setconfigurecallback[/cmd]
*/

static bool addon_kvs_cmd_configure(KviKvsModuleCommandCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("name",KVS_PT_NONEMPTYSTRING,0,szName)
	KVSM_PARAMETERS_END(c)

	KviKvsScriptAddon * a = KviKvsScriptAddonManager::instance()->findAddon(szName);
	if(a)
	{
		QString ss = a->configureCallbackCode();
		if(ss.isEmpty())
		{
			if(!c->switches()->find('q',"quiet"))
				c->warning(__tr2qs("The addon \"%1\" has no configure callback set").arg(szName));
		} else {
			a->executeConfigureCallback(c->window());
		}
	} else {
		if(!c->switches()->find('q',"quiet"))
			c->warning(__tr2qs("The addon \"%1\" does not exist").arg(szName));
	}

	return true;
}

/*
	@doc: addon.help
	@type:
		command
	@title:
		addon.help
	@short:
		Executes a addon's help callback
	@syntax:
		addon.help [-q] <id:string>
	@switches:
		!sw: -q | --quiet
			Makes the command run quietly
	@description:
		Executes the help callback of the specified addon.
		It will usually display the addon's documentation in the help viewer.
	@seealso:
		[cmd]addon.register[/cmd]
		[cmd]addon.sethelpcallback[/cmd]
*/

static bool addon_kvs_cmd_help(KviKvsModuleCommandCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("name",KVS_PT_NONEMPTYSTRING,0,szName)
	KVSM_PARAMETERS_END(c)

	KviKvsScriptAddon * a = KviKvsScriptAddonManager::instance()->findAddon(szName);
	if(a)
	{
		QString ss = a->helpCallbackCode();
		if(ss.isEmpty())
		{
			if(!c->switches()->find('q',"quiet"))
				c->warning(__tr2qs("The addon \"%1\" has no help callback set").arg(szName));
		} else {
			a->executeHelpCallback(c->window());
		}
	} else {
		if(!c->switches()->find('q',"quiet"))
			c->warning(__tr2qs("The addon \"%1\" does not exist").arg(szName));
	}

	return true;
}

/*
	@doc: addon.setconfigurecallback
	@type:
		command
	@title:
		addon.setconfigurecallback
	@short:
		Sets a addon's configuration callback
	@syntax:
		addon.setconfigurecallback [-q] (<id:string>)
		{
			<configure_callback>
		}
	@switches:
		!sw: -q
			Makes the command run quietly
	@description:
		Sets the configure callback for the specified addon.
		The configure callback will be called by the user either by the
		means of [cmd]addon.configure[/cmd] or by accessing the
		proper function via GUI.
	@seealso:
		[cmd]addon.register[/cmd]
		[cmd]addon.configure[/cmd]
*/

static bool addon_kvs_cmd_setconfigurecallback(KviKvsModuleCallbackCommandCall * c)
{
	QString szName;

	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("name",KVS_PT_NONEMPTYSTRING,0,szName)
	KVSM_PARAMETERS_END(c)

	KviKvsScriptAddon * a = KviKvsScriptAddonManager::instance()->findAddon(szName);
	if(a)
	{
		a->setConfigureCallback(c->callback()->code());
	} else {
		if(!c->switches()->find('q',"quiet"))
			c->warning(__tr2qs("The addon \"%1\" does not exist").arg(szName));
	}
	
	return true;
}

/*
	@doc: addon.sethelpcallback
	@type:
		command
	@title:
		addon.sethelpcallback
	@short:
		Sets a addon's configuration callback
	@syntax:
		addon.sethelpcallback(<id:string>)
		{
			<configure_callback>
		}
	@switches:
		!sw: -q
			Makes the command run quietly
	@description:
		Sets the help callback for the specified addon.
		The help callback will be called by the user either by the
		means of [cmd]addon.help[/cmd] or by accessing the
		proper function via GUI. It should display some sort
		of addon documentation, usually in the help browser.
	@seealso:
		[cmd]addon.register[/cmd]
		[cmd]addon.help[/cmd]
*/

static bool addon_kvs_cmd_sethelpcallback(KviKvsModuleCallbackCommandCall * c)
{
	QString szName;

	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("name",KVS_PT_NONEMPTYSTRING,0,szName)
	KVSM_PARAMETERS_END(c)

	KviKvsScriptAddon * a = KviKvsScriptAddonManager::instance()->findAddon(szName);
	if(a)
	{
		a->setHelpCallback(c->callback()->code());
	} else {
		if(!c->switches()->find('q',"quiet"))
			c->warning(__tr2qs("The addon \"%1\" does not exist").arg(szName));
	}
	
	return true;
}

/*
	@doc: addon.register
	@type:
		command
	@title:
		addon.register
	@short:
		Registers a script-based addon
	@syntax:
		addon.register [-f] [-n] [-q] (<id:string>,<version:string>,<visible_name:string>,<description:string>,<minkvircverion:string>[,<iconid:string>])
		{
			<uninstall callback>
		}
	@switches:
		!sw: -f | --force
			Registers the addon even if an addon with the same <id> and
			a higher version already exists. The usage of this flag
			is highly discouraged (i.e. use it only for debugging purposes
			on your own machine).
		!sw: -n | --no-uninstall
			Performs no uninstallation of existing versions of the addon:
			it simply replaces the registration entry with the new data.
			Again, do NOT use this switch if not for debugging purposes
			and on your own machine.
		!sw: -q | --quiet
			Makes the command run quietly
	@description:
		[p]
		Registers a script-based addon.
		[/p]
		[p]
		The registration process allows to "show" the addon in the script-addon manager
		dialog and provides a standard way for the user to manage and uninstall the addons.
		You simply register your addon BEFORE attempting to install it.
		[/p]
		[p]
		A script-based addon is a set of scripts, icons, translations and possibly
		other data files that add functionality to the KVIrc program.
		The script-based addons are often simply called "scripts" and
		we will adhere to that naming in certain parts of the documentation too.
		[/p]
		[p]
		Each script-based addon (a set of scripts) is identified by an UNIQUE
		<id>. Two addons with the same <id> can't co-exist in the same
		KVIrc installation (so be sure to choose a token characteristic enough
		to avoid collisions with others). The <id> itself is used only for
		identification purposes and the user will almost always see the <visible_name>
		instead, which can contain the [fnc]$tr[/fnc] function that will handle
		the translation for it.
		[/p]
		[p]
		Each addon has also a <version> which is a string in the form x.y.z
		where x, y and z are numbers (yes.. that's the standard major-minor-patch level
		version numbering scheme). A <version> of 2.4.23 is greater than 2.4.3
		even if 2.4.3 comes after when compared as a string.
		When an addon with a greater or equal version is installed over
		an addon with a lower version, the lower one gets uninstalled first.
		Installing a lower version over a greater one is not possible, unless
		the lower versioned one is uninstalled first.
		[/p]
		[p]
		<description> is another, possibly translated, string that will
		be presented to the user in the addon management dialog.
		[/p]
		[p]
		<minkvircversion> is the minimum KVIrc version required for the
		addon to run. If the version of the running KVIrc executable
		is lower than the requested one then the command will abort with an error.
		If you want to completly ignore the KVIrc versioning (don't do it),
		use "0.0.0" here. If you need fine tuning on cvs features you may add also
		the sources date tag at the end of the required version string (e.g 3.2.1.20060303).
		[/p]
		[p]
		<iconid> is the [doc:image_id]image identifier[/doc] of the icon
		that will be displayed in the addon management dialog.
		If not specified, a default icon will be used.
		[/p]
		[p]
		The <uninstall_callback> is a snippet of code that should
		wipe out the addon from the system. It is ALWAYS a good practice
		to write a complete uninstallation procedure (think that YOU like
		to be able to completly uninstall a program that you don't use anymore).
		The <uninstall_callback> will be called by KVIrc when the addon
		uninstallation is requested, either explicitly by using the GUI or the
		command [cmd]addon.uninstall[/cmd], or implicitly by installing
		a newer version of the addon (upgrading).
		[/p]
		[p]
		If the user security rules don't allow your addon to be installed
		or a higher version of a addon with the same name already exists
		the command will fail with an error (aborting your whole intallation addon).
		If you don't want to fail with an error but handle it gracefully instead
		then you should use [fnc]$addon.exists()[/fnc] to check if a
		addon with the same name and a greater version already exists.
		You can't gracefully handle security error conditions: your installation
		will be always aborted with an error in this case.
		[/p]
		[p]
		The addon can also have a configuration callback settable with [cmd]addon.setconfigurecallback[/cmd]
		and a help callback settable with [cmd]addon.sethelpcallback[/cmd]. The first
		will usually display a configuration dialog, the second will display
		some sort of addon's documentation, usually in the help browser.
		[/p]
		[p]
		The registration process uninstalls any previous addon version
		by executing its uninstall callback routine. This is another reason for that
		you should call addon.register BEFORE you attempt to install your addon.
		Failing to do that may cause your old uninstalled to wipe out your newly
		installed files or code.
		[/p]
	@seealso:
		[cmd]addon.uninstall[/cmd], [fnc]$addon.exists[/fnc],
		[cmd]addon.setconfigurecallback[/cmd], [cmd]addon.configure[/cmd],
		[cmd]addon.sethelpcallback[/cmd], [cmd]addon.help[/cmd]
	@examples:
		[example]
		
		[/example]
*/

static bool addon_kvs_cmd_register(KviKvsModuleCallbackCommandCall * c)
{
	KviKvsScriptAddonRegistrationData rd;
	QString szMinKVIrcVersion;

	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("name",KVS_PT_NONEMPTYSTRING,0,(rd.szName))
		KVSM_PARAMETER("version",KVS_PT_NONEMPTYSTRING,0,(rd.szVersion))
		KVSM_PARAMETER_IGNORED("visible_text")
		KVSM_PARAMETER_IGNORED("description")
		KVSM_PARAMETER("min_kvirc_version",KVS_PT_NONEMPTYSTRING,0,szMinKVIrcVersion)
		KVSM_PARAMETER("icon_id",KVS_PT_STRING,KVS_PF_OPTIONAL,(rd.szIconId))
	KVSM_PARAMETERS_END(c)


	if(!(c->getParameterCode(2,rd.szVisibleNameScript) && c->getParameterCode(3,rd.szDescriptionScript)))
	{
		c->error(__tr2qs("Internal error: call a head-shrinker"));
		return false;
	}

	if(c->callback())
		rd.szUninstallCallbackScript = c->callback()->code();

	if(!KviMiscUtils::isValidVersionString(rd.szVersion))
	{
		c->error(__tr2qs("The specified version \"%Q\" is not a valid version string"),&(rd.szVersion));
		return false;
	}

	if(!KviMiscUtils::isValidVersionString(szMinKVIrcVersion))
	{
		c->error(__tr2qs("The specified KVIrc version \"%Q\" is not a valid version string"),&szMinKVIrcVersion);
		return false;
	}

	if(KviMiscUtils::compareVersions(szMinKVIrcVersion,KVI_VERSION "." KVI_SOURCES_DATE) < 0)
	{
		c->error(__tr2qs("This KVIrc executable is too old to run this addon (minimum version required is %Q)"),&szMinKVIrcVersion);
		return false;
	}

	if(!c->switches()->find('q',"quiet"))
		c->window()->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Attempting to register addon \"%Q\" with version %Q"),&(rd.szName),&(rd.szVersion));

	KviKvsScriptAddon * a = KviKvsScriptAddonManager::instance()->findAddon(rd.szName);
	if(a)
	{
		// the same addon already exists
		if(KviMiscUtils::compareVersions(a->version(),rd.szVersion) < 0)
		{
			// and it has a higher version...
			// complain unless -f is used
			if(!c->switches()->find('f',"force"))
			{
				c->error(__tr2qs("The script addon \"%Q\" already exists with version %Q which is higher than %Q"),&(rd.szName),&(a->version()),&(rd.szVersion));
				return false;
			}
		}

		if(!c->switches()->find('q',"quiet"))
			c->window()->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Uninstalling existing addon version %Q"),&(a->version()));

		// uninstall the existing version
		KviKvsScriptAddonManager::instance()->unregisterAddon(rd.szName,c->window(),!c->switches()->find('n',"no-uninstall"));
	}

	if(!KviKvsScriptAddonManager::instance()->registerAddon(&rd))
	{
		c->error(__tr2qs("Script registration failed"));
		return false;
	}

	if(!c->switches()->find('q',"quiet"))
		c->window()->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Script succesfully registered"));

	return true;
}

/*
	@doc: addon.dialog
	@type:
		command
	@title:
		addon.dialog
	@short:
		Shows the addon addon management editor
	@syntax:
		addon.dialog
	@description:
		Shows the addon addon management editor
*/

static bool addon_kvs_cmd_dialog(KviKvsModuleCommandCall * c)
{
	KviScriptManagementDialog::display();
	return true;
}

static bool addon_module_init(KviModule *m)
{
	KVSM_REGISTER_FUNCTION(m,"exists",addon_kvs_fnc_exists);
	KVSM_REGISTER_FUNCTION(m,"version",addon_kvs_fnc_version);

	KVSM_REGISTER_SIMPLE_COMMAND(m,"dialog",addon_kvs_cmd_dialog);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"list",addon_kvs_cmd_list);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"uninstall",addon_kvs_cmd_uninstall);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"configure",addon_kvs_cmd_configure);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"help",addon_kvs_cmd_help);
	
	KVSM_REGISTER_CALLBACK_COMMAND(m,"setconfigurecallback",addon_kvs_cmd_setconfigurecallback);
	KVSM_REGISTER_CALLBACK_COMMAND(m,"sethelpcallback",addon_kvs_cmd_sethelpcallback);
	KVSM_REGISTER_CALLBACK_COMMAND(m,"register",addon_kvs_cmd_register);

	QString szBuf;
	m->getDefaultConfigFileName(szBuf);
	KviConfig cfg(szBuf,KviConfig::Read);
	g_rectManagementDialogGeometry = cfg.readRectEntry("EditorGeometry",QRect(10,10,390,440));

	return true;
}

static bool addon_module_cleanup(KviModule *m)
{
	KviScriptManagementDialog::cleanup();

	QString szBuf;
	m->getDefaultConfigFileName(szBuf);
	KviConfig cfg(szBuf,KviConfig::Write);
	cfg.writeEntry("EditorGeometry",g_rectManagementDialogGeometry);

	return true;
}

static bool addon_module_can_unload(KviModule * m)
{
	return (!KviScriptManagementDialog::instance());
}


KVIRC_MODULE(
	"Addon",                                                      // module name
	"4.0.0",                                                        // module version
	"Copyright (C) 2005 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
	"Script management functions for the KVS engine",
	addon_module_init,
	addon_module_can_unload,
	0,
	addon_module_cleanup
)
