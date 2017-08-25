//=============================================================================
//
//   File : libkviaddon.cpp
//   Creation date : Tue 31 Mar 01:02:12 2005 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "AddonManagementDialog.h"
#include "AddonFunctions.h"

#include "KviModule.h"
#include "KviKvsScriptAddonManager.h"
#include "KviLocale.h"
#include "KviQString.h"
#include "KviCommandFormatter.h"
#include "KviError.h"
#include "kvi_out.h"
#include "KviIconManager.h"
#include "KviControlCodes.h"
#include "KviConfigurationFile.h"
#include "kvi_sourcesdate.h"
#include "KviMiscUtils.h"
#include "KviFileUtils.h"

#include <QFileInfo>
#include <QDir>

QRect g_rectManagementDialogGeometry(0, 0, 0, 0);

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
		Returns [b]1[/b] if the addon with the specified <id> is currently installed
		and [b]0[/b] otherwise. If <version> is specified then any addon with
		a version lower than <version> is ignored (so you can effectively
		check if a greater or equal version is present).
*/

static bool addon_kvs_fnc_exists(KviKvsModuleFunctionCall * c)
{
	QString szId;
	QString szVersion;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("id", KVS_PT_NONEMPTYSTRING, 0, szId)
	KVSM_PARAMETER("version", KVS_PT_STRING, KVS_PF_OPTIONAL, szVersion)
	KVSM_PARAMETERS_END(c)

	KviKvsScriptAddon * a = KviKvsScriptAddonManager::instance()->findAddon(szId);
	if(a)
	{
		if(szVersion.isEmpty())
		{
			c->returnValue()->setBoolean(true);
		}
		else
		{
			c->returnValue()->setBoolean(KviMiscUtils::compareVersions(a->version(), szVersion) < 0);
		}
	}
	else
	{
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
	KVSM_PARAMETER("id", KVS_PT_NONEMPTYSTRING, 0, szId)
	KVSM_PARAMETERS_END(c)

	KviKvsScriptAddon * a = KviKvsScriptAddonManager::instance()->findAddon(szId);
	if(a)
	{
		c->returnValue()->setString(a->version());
	}
	else
	{
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
	KviPointerHashTable<QString, KviKvsScriptAddon> * da = KviKvsScriptAddonManager::instance()->addonDict();

	int cnt = 0;
	KviPointerHashTableIterator<QString, KviKvsScriptAddon> it(*da);
	while(KviKvsScriptAddon * a = it.current())
	{
		c->window()->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs_ctx("%cAddon ID %Q, version %Q%c", "addon"), KviControlCodes::Bold, &(a->name()), &(a->version()), KviControlCodes::Bold);
		c->window()->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs_ctx("Name: %Q", "addon"), &(a->visibleName()));
		c->window()->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs_ctx("Description: %Q", "addon"), &(a->description()));

		++it;
		cnt++;
	}

	c->window()->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs_ctx("Total: %d addons installed", "addon"), cnt);
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
			Doesn't call the uninstall callback but only removes the registration entry.
		!sw: -q | --quiet
			Makes the command run quietly
	@description:
		Uninstalls the specified addon by executing its uninstall callback function
		and removing its installed files. It also removes the addon's registration entry.
		If the [b]-n[/b] switch is specified the uninstall callback is not called,
		only the registration entry is removed.
	@seealso:
		[cmd]addon.register[/cmd]
*/

static bool addon_kvs_cmd_uninstall(KviKvsModuleCommandCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("name", KVS_PT_NONEMPTYSTRING, 0, szName)
	KVSM_PARAMETERS_END(c)

	KviKvsScriptAddon * a = KviKvsScriptAddonManager::instance()->findAddon(szName);
	if(a)
	{
		if(!c->switches()->find('q', "quiet"))
			c->window()->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs_ctx("Uninstalling existing addon version %Q", "addon"), &(a->version()));

		// uninstall the existing version
		KviKvsScriptAddonManager::instance()->unregisterAddon(szName, c->window(), !c->switches()->find('n', "no-callback"));
	}
	else
	{
		if(!c->switches()->find('q', "quiet"))
			c->warning(__tr2qs_ctx("The addon \"%1\" doesn't exist", "addon").arg(szName));
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
		Executes an addon's configuration callback
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
	KVSM_PARAMETER("name", KVS_PT_NONEMPTYSTRING, 0, szName)
	KVSM_PARAMETERS_END(c)

	KviKvsScriptAddon * a = KviKvsScriptAddonManager::instance()->findAddon(szName);
	if(a)
	{
		QString ss = a->configureCallbackCode();
		if(ss.isEmpty())
		{
			if(!c->switches()->find('q', "quiet"))
				c->warning(__tr2qs_ctx("The addon \"%1\" has no configure callback set", "addon").arg(szName));
		}
		else
		{
			a->executeConfigureCallback(c->window());
		}
	}
	else
	{
		if(!c->switches()->find('q', "quiet"))
			c->warning(__tr2qs_ctx("The addon \"%1\" doesn't exist", "addon").arg(szName));
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
		Executes an addon's help callback
	@syntax:
		addon.help [-q] <id:string>
	@switches:
		!sw: -q | --quiet
			Makes the command run quietly
	@description:
		Executes the help callback of the specified addon. It will usually
		display the addon's documentation in the help viewer.
	@seealso:
		[cmd]addon.register[/cmd]
		[cmd]addon.sethelpcallback[/cmd]
*/

static bool addon_kvs_cmd_help(KviKvsModuleCommandCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("name", KVS_PT_NONEMPTYSTRING, 0, szName)
	KVSM_PARAMETERS_END(c)

	KviKvsScriptAddon * a = KviKvsScriptAddonManager::instance()->findAddon(szName);
	if(a)
	{
		QString ss = a->helpCallbackCode();
		if(ss.isEmpty())
		{
			if(!c->switches()->find('q', "quiet"))
				c->warning(__tr2qs_ctx("The addon \"%1\" has no help callback set", "addon").arg(szName));
		}
		else
		{
			a->executeHelpCallback(c->window());
		}
	}
	else
	{
		if(!c->switches()->find('q', "quiet"))
			c->warning(__tr2qs_ctx("The addon \"%1\" doesn't exist", "addon").arg(szName));
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
		Sets an addon's configuration callback
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
		proper function via the GUI.
	@seealso:
		[cmd]addon.register[/cmd]
		[cmd]addon.configure[/cmd]
*/

static bool addon_kvs_cmd_setconfigurecallback(KviKvsModuleCallbackCommandCall * c)
{
	QString szName;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("name", KVS_PT_NONEMPTYSTRING, 0, szName)
	KVSM_PARAMETERS_END(c)

	KviKvsScriptAddon * a = KviKvsScriptAddonManager::instance()->findAddon(szName);
	if(a)
	{
		a->setConfigureCallback(c->callback()->code());
	}
	else
	{
		if(!c->switches()->find('q', "quiet"))
			c->warning(__tr2qs_ctx("The addon \"%1\" doesn't exist", "addon").arg(szName));
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
		Sets an addon's help callback
	@syntax:
		addon.sethelpcallback(<id:string>)
		{
			<help_callback>
		}
	@switches:
		!sw: -q
		Makes the command run quietly
	@description:
		Sets the help callback for the specified addon.
		The help callback will be called by the user either by the
		means of [cmd]addon.help[/cmd] or by accessing the
		proper function via the GUI. It should display some sort
		of addon documentation, usually in the help browser.
	@seealso:
		[cmd]addon.register[/cmd]
		[cmd]addon.help[/cmd]
*/

static bool addon_kvs_cmd_sethelpcallback(KviKvsModuleCallbackCommandCall * c)
{
	QString szName;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("name", KVS_PT_NONEMPTYSTRING, 0, szName)
	KVSM_PARAMETERS_END(c)

	KviKvsScriptAddon * a = KviKvsScriptAddonManager::instance()->findAddon(szName);
	if(a)
	{
		a->setHelpCallback(c->callback()->code());
	}
	else
	{
		if(!c->switches()->find('q', "quiet"))
			c->warning(__tr2qs_ctx("The addon \"%1\" doesn't exist", "addon").arg(szName));
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
		is highly discouraged (i.e. [b]use it only for debugging purposes
		on your own machine[/b]).
		!sw: -n | --no-uninstall
		Performs no uninstallation of existing versions of the addon:
		it simply replaces the registration entry with the new data.
		Again, [b]only use this switch for debugging purposes and on your own machine[/b].
		!sw: -q | --quiet
		Makes the command run quietly
	@description:
		Registers a script-based addon.[br][br]
		The registration process allows to [i]show[/i] the addon in the script-addon manager
		dialog and provides a standard way for the user to manage and uninstall the addons.
		You simply register your addon BEFORE attempting to install it.[br][br]
		A script-based addon is a set of scripts, icons, translations and possibly
		other data files that add functionality to the KVIrc program.
		The script-based addons are often simply called [i]scripts[/i] and
		we will adhere to that naming in certain parts of the documentation too.[br][br]
		Each script-based addon (a set of scripts) is identified by a UNIQUE
		<id>. Two addons with the same <id> can't co-exist in the same
		KVIrc installation (so be sure to choose a token unique enough
		to avoid collisions with others). The <id> itself is used only for
		identification purposes and the user will almost always see the <visible_name>
		instead, which can contain the [fnc]$tr[/fnc] function that will handle
		the translation for it.[br][br]
		Each addon also has a <version> which is a string in the form x.y.z
		where x, y and z are numbers (yes.. that's the standard major-minor-patch level
		version numbering scheme). A <version> of 2.4.23 is greater than 2.4.3
		even if 2.4.3 comes after when compared as a string.
		When an updated addon is installed over the same or previous version, the current version is first uninstalled.
		Installing a lower version over a greater one is not possible, unless
		the lower version one is uninstalled first.[br][br]
		<description> is another, possibly translated, string that will
		be presented to the user in the addon management dialog.[br][br]
		<minkvircversion> is the minimum KVIrc version required for the
		addon to run. If the version of the running KVIrc executable
		is lower than the requested one then the command will abort with an error.
		If you want to completely ignore the KVIrc versioning (don't do it),
		use [b][i]0.0.0[/i][/b] here. If you need fine tuning on git features you may also add
		the sources date tag at the end of the required version string (e.g 3.2.1.20060303).[br][br]
		<iconid> is the [doc:image_id]image identifier[/doc] of the icon
		that will be displayed in the addon management dialog.
		If not specified, a default icon will be used.[br][br]
		The <uninstall_callback> is a snippet of code that should
		wipe out the addon from the system. It is ALWAYS a good practice
		to write a complete uninstallation procedure (think that YOU like
		to be able to completely uninstall a program that you don't use anymore).
		The <uninstall_callback> will be called by KVIrc when the addon
		uninstallation is requested, either explicitly by using the GUI or the
		command [cmd]addon.uninstall[/cmd], or implicitly by installing
		a newer version of the addon (upgrading).[br][br]
		If the user's security configuration doesn't allow your addon to be installed,
		or a higher version of an addon with the same name already exists,
		the command will fail with an error (aborting the installation process).
		If you don't want to fail with an error but handle it gracefully instead,
		you should use [fnc]$addon.exists()[/fnc] to check if an
		addon with the same name and a greater version already exists.
		You can't gracefully handle security error conditions: your installation
		will be always aborted with an error in this case.[br][br]
		The addon can also define a configuration callback via [cmd]addon.setconfigurecallback[/cmd]
		and a help callback via [cmd]addon.sethelpcallback[/cmd]. The first
		will usually display a configuration dialog, the second will display
		some sort of addon documentation, usually in the help browser.[br][br]
		The registration process uninstalls any previous addon version
		by executing its uninstall callback routine. This is another reason that
		you should call addon.register BEFORE you attempt to install your addon.
		Failing to do this may result in the the old version uninstallation wiping
		out your newly installed files or code.
	@seealso:
		[cmd]addon.uninstall[/cmd], [fnc]$addon.exists[/fnc],
		[cmd]addon.setconfigurecallback[/cmd], [cmd]addon.configure[/cmd],
		[cmd]addon.sethelpcallback[/cmd], [cmd]addon.help[/cmd], [cmd]addon.installfiles[/cmd]
	@examples:
		[example]
		[/example]
*/

static bool addon_kvs_cmd_register(KviKvsModuleCallbackCommandCall * c)
{
	KviKvsScriptAddonRegistrationData rd;
	QString szMinKVIrcVersion;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("name", KVS_PT_NONEMPTYSTRING, 0, (rd.szName))
	KVSM_PARAMETER("version", KVS_PT_NONEMPTYSTRING, 0, (rd.szVersion))
	KVSM_PARAMETER_IGNORED("visible_text")
	KVSM_PARAMETER_IGNORED("description")
	KVSM_PARAMETER("min_kvirc_version", KVS_PT_NONEMPTYSTRING, 0, szMinKVIrcVersion)
	KVSM_PARAMETER("icon_id", KVS_PT_STRING, KVS_PF_OPTIONAL, (rd.szIconId))
	KVSM_PARAMETERS_END(c)

	if(!(c->getParameterCode(2, rd.szVisibleNameScript) && c->getParameterCode(3, rd.szDescriptionScript)))
	{
		c->error(__tr2qs_ctx("Internal error: call a head-shrinker", "addon"));
		return false;
	}

	if(c->callback())
		rd.szUninstallCallbackScript = c->callback()->code();

	if(!KviMiscUtils::isValidVersionString(rd.szVersion))
	{
		c->error(__tr2qs_ctx("The specified version \"%Q\" is not a valid version string", "addon"), &(rd.szVersion));
		return false;
	}

	if(!KviMiscUtils::isValidVersionString(szMinKVIrcVersion))
	{
		c->error(__tr2qs_ctx("The specified KVIrc version \"%Q\" is not a valid version string", "addon"), &szMinKVIrcVersion);
		return false;
	}

	if(KviMiscUtils::compareVersions(szMinKVIrcVersion, KVI_VERSION "." KVI_SOURCES_DATE) < 0)
	{
		c->error(__tr2qs_ctx("This KVIrc executable is too old to run this addon (minimum version required is %Q)", "addon"), &szMinKVIrcVersion);
		return false;
	}

	if(!c->switches()->find('q', "quiet"))
		c->window()->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs_ctx("Attempting to register addon \"%Q\" with version %Q", "addon"), &(rd.szName), &(rd.szVersion));

	KviKvsScriptAddon * a = KviKvsScriptAddonManager::instance()->findAddon(rd.szName);
	if(a)
	{
		// the same addon already exists
		if(KviMiscUtils::compareVersions(a->version(), rd.szVersion) < 0)
		{
			// and it has a higher version...
			// complain unless -f is used
			if(!c->switches()->find('f', "force"))
			{
				c->error(__tr2qs_ctx("The addon \"%Q\" already exists with version %Q which is higher than %Q", "addon"), &(rd.szName), &(a->version()), &(rd.szVersion));
				return false;
			}
		}

		if(!c->switches()->find('q', "quiet"))
			c->window()->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs_ctx("Uninstalling existing addon version %Q", "addon"), &(a->version()));

		bool bUninstall = !c->switches()->find('n', "no-uninstall");

		// uninstall the existing version
		KviKvsScriptAddonManager::instance()->unregisterAddon(rd.szName, c->window(), bUninstall, bUninstall);
	}

	if(!KviKvsScriptAddonManager::instance()->registerAddon(&rd))
	{
		c->error(__tr2qs_ctx("Addon registration failed", "addon"));
		return false;
	}

	if(!c->switches()->find('q', "quiet"))
		c->window()->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs_ctx("Addon successfully registered", "addon"));

	return true;
}

/*
	@doc: addon.installfiles
	@type:
		command
	@title:
		addon.installfiles
	@short:
		Installs a set of files for an addon
	@syntax:
		addon.installfiles <id:string> <target:string> [files]
	@switches:
		!sw: -q | --quiet
		Makes the command run quietly
		!sw: -s | --skip-nonexistent
		Skip nonexistent entries in the [files] list
	@description:
		Installs the [files] for the addon identified by the specified <id>.
		These files will be automatically removed when the addon is uninstalled.[br][br]
		<target> is the target path inside the local KVIrc directory. The following
		standard paths should be used:
		[ul]
		[li]"pics" for image files.[/li]
		[li]"locale" for translation *.mo files.[/li]
		[li]"audio" for sound files.[/li]
		[li]"config" for configuration files.[/li]
		[li]"help/<language>" for help files.[/li]
		[/ul]
		Other target paths are allowed and subdirectories are supported (e.g. [i]pics/myaddon[/i]).[br][br]
		[files] is a list of filenames or directory names.
		Each file will be copied to the specified target path in the local KVIrc directory.
		Filenames can contain wildcard characters in the last component.
	@seealso:
		[cmd]addon.register[/cmd]
		[cmd]addon.uninstall[/cmd]
*/

static bool addon_kvs_cmd_installfiles(KviKvsModuleCommandCall * c)
{
	QString szName;
	QString szTarget;
	QStringList lEntries;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("name", KVS_PT_NONEMPTYSTRING, 0, szName)
	KVSM_PARAMETER("target", KVS_PT_NONEMPTYSTRING, 0, szTarget)
	KVSM_PARAMETER("files", KVS_PT_STRINGLIST, 0, lEntries)
	KVSM_PARAMETERS_END(c)

	bool bQuiet = c->switches()->find('q', "quiet");
	// we had a typo here, support the old switch name for backward scripts compatibility
	bool bSkipNonExistent = c->switches()->find('i', "skip-nonexistent") || c->switches()->find('i', "skip-nonexisting");

	KviKvsScriptAddon * a = KviKvsScriptAddonManager::instance()->findAddon(szName);
	if(!a)
	{
		if(!bQuiet)
			c->warning(__tr2qs_ctx("The addon \"%1\" doesn't exist", "addon").arg(szName));
		return true;
	}

	if(szTarget.isEmpty())
		return c->error(__tr2qs_ctx("The target path can't be empty", "addon"));

	if(szTarget.indexOf("..") != -1)
		return c->error(__tr2qs_ctx("The target path can't contain ..", "addon"));

	if(lEntries.isEmpty())
	{
		if(!bQuiet)
			c->warning(__tr2qs_ctx("Empty file list", "addon"));
		return true; // nothing to do
	}

	QStringList lExpandedEntries;

	foreach(QString szEntry, lEntries)
	{
		KviFileUtils::adjustFilePath(szEntry);

		if(szEntry.contains("*"))
		{
			// filtered entry
			int idx = szEntry.lastIndexOf(QChar(KVI_PATH_SEPARATOR_CHAR));
			QString szPath;
			QString szFilter;
			if(idx < 0)
			{
				szPath = ".";
				szFilter = szEntry;
			}
			else
			{
				szPath = szEntry.left(idx);
				szFilter = szEntry.mid(idx + 1);
			}

			QDir dir(szPath);
			if(!dir.exists())
			{
				if(!bSkipNonExistent)
					return c->error(__tr2qs_ctx("The directory '%1' doesn't exist", "addon").arg(szPath));
				if(!bQuiet)
					c->warning(__tr2qs_ctx("Skipping non-existent entry '%1'", "addon").arg(szEntry));
				continue;
			}

			QStringList sl = dir.entryList(
			    QStringList(szFilter),
			    QDir::Files | QDir::NoSymLinks | QDir::Readable | QDir::Hidden | QDir::System,
			    QDir::Unsorted);

			foreach(QString sz, sl)
			{
				lExpandedEntries.append(QString("%1%2%3").arg(szPath).arg(QString(KVI_PATH_SEPARATOR_CHAR)).arg(sz));
			}
			continue;
		}

		QFileInfo inf(szEntry);
		if(!inf.exists())
		{
			if(!bSkipNonExistent)
				return c->error(__tr2qs_ctx("The file '%1' doesn't exist", "addon").arg(szEntry));
			if(!bQuiet)
				c->warning(__tr2qs_ctx("Skipping non-existent entry '%1'", "addon").arg(szEntry));
			continue;
		}

		if(!inf.isFile())
		{
			if(!bSkipNonExistent)
				return c->error(__tr2qs_ctx("The entry '%1' is not a file", "addon").arg(szEntry));
			if(!bQuiet)
				c->warning(__tr2qs_ctx("Skipping invalid entry '%1'", "addon").arg(szEntry));
			continue;
		}

		lExpandedEntries.append(szEntry);
	}

	// create target path
	QString szTargetPath;
	g_pApp->getLocalKvircDirectory(szTargetPath, KviApplication::None, szTarget);

	KviFileUtils::makeDir(szTargetPath);

	for(auto & lExpandedEntrie : lExpandedEntries)
	{
		QFileInfo inf(lExpandedEntrie);
		if(!inf.exists())
		{
			qDebug("ERROR: file %s doesn't exist, but it should...", inf.fileName().toUtf8().data());
			continue; // bleah.. should never happen
		}

		QString szEntry = QString("%1%2%3").arg(szTarget).arg(QString(KVI_PATH_SEPARATOR_CHAR)).arg(inf.fileName());
		g_pApp->getLocalKvircDirectory(szTargetPath, KviApplication::None, szEntry);

		if(!bQuiet)
			c->window()->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs_ctx("Installing file '%1' into '%2'", "addon").arg(lExpandedEntrie).arg(szTargetPath));

		KviFileUtils::copyFile(lExpandedEntrie, szTargetPath);

		a->addInstalledFile(szEntry);
	}

	return true;
}

/*
	@doc: addon.dialog
	@type:
		command
	@title:
		addon.dialog
	@short:
		Shows the addon management editor
	@syntax:
		addon.dialog [-t]
	@description:
		Shows the addon management editor.[br][br]
		If the [b]-t[/b] switch is used, the dialog is opened as toplevel window,
		otherwise it is opened as part of the current frame window.[br]
*/

static bool addon_kvs_cmd_dialog(KviKvsModuleCommandCall * c)
{

	AddonManagementDialog::display(c->hasSwitch('t', "toplevel"));
	return true;
}

/*
	@doc: addon.install
	@type:
		command
	@title:
		addon.install
	@short:
		Installs the addon
	@syntax:
		addon.install <package_path:string>
	@description:
		Attempts to install the addon in the package specified by <package_path>.
*/

static bool addon_kvs_cmd_install(KviKvsModuleCommandCall * c)
{
	QString szAddonPackFile;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("package_path", KVS_PT_STRING, 0, szAddonPackFile)
	KVSM_PARAMETERS_END(c)

	QString szError;
	if(!AddonFunctions::installAddonPackage(szAddonPackFile, szError))
	{
		c->error(__tr2qs_ctx("Error installing addon package: %Q", "addon"), &szError);
		return false;
	}

	return true;
}

/*
	@doc: addon.pack
	@type:
		command
	@title:
		addon.pack
	@short:
		Creates a kva package containing an addon
	@syntax:
		addon.pack <package_path> <addon_name> <addon_version> <description> <author> <min_kvirc_version> <image> <addon_path>
	@description:
		Creates a *.kva package containing a KVIrc addon.[br][br]
		<package_path> is the absolute path and file name of the package that should be saved.[br]
		<addon_name> is the visible name of the addon (something like [i][b]My Addon[/i][/b]).[br]
		<addon_version> is the version of the addon in the form X.Y.Z.[br]
		<description> is a textual description of the addon.[br]
		<author> is the name of the person that is creating the addon.[br]
		<min_kvirc_version> is the minimum KVIrc version that this addon supports. Pass an empty string if you want
		this to become the current KVIrc version.[br]
		<image> is the path of an image to be used in the installation dialog. Pass an empty string if you
		don't want an image to be stored in the package.[br]
		<addon_path> is a path to the directory containing the addon. It should contain an install.kvs file
		that calls [cmd]addon.register[/cmd] and then installs all the addon aliases, events and files via [cmd]addon.installfiles[/cmd].
*/
static bool addon_kvs_cmd_pack(KviKvsModuleCommandCall * c)
{
	AddonInfo info;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("package_path", KVS_PT_NONEMPTYSTRING, 0, info.szSavePath)
	KVSM_PARAMETER("addon_name", KVS_PT_NONEMPTYSTRING, 0, info.szName)
	KVSM_PARAMETER("addon_version", KVS_PT_NONEMPTYSTRING, 0, info.szVersion)
	KVSM_PARAMETER("description", KVS_PT_STRING, 0, info.szDescription)
	KVSM_PARAMETER("author", KVS_PT_NONEMPTYSTRING, 0, info.szAuthor)
	KVSM_PARAMETER("min_kvirc_version", KVS_PT_STRING, 0, info.szMinVersion)
	KVSM_PARAMETER("image", KVS_PT_STRING, 0, info.szImage)
	KVSM_PARAMETER("addon_path", KVS_PT_NONEMPTYSTRING, 0, info.szDirPath)
	KVSM_PARAMETERS_END(c)

	QString szError;

	if(AddonFunctions::pack(info, szError))
		return true;

	c->error(szError);
	return false;
}

static bool addon_module_init(KviModule * m)
{
	KVSM_REGISTER_FUNCTION(m, "exists", addon_kvs_fnc_exists);
	KVSM_REGISTER_FUNCTION(m, "version", addon_kvs_fnc_version);

	KVSM_REGISTER_SIMPLE_COMMAND(m, "dialog", addon_kvs_cmd_dialog);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "list", addon_kvs_cmd_list);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "install", addon_kvs_cmd_install);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "uninstall", addon_kvs_cmd_uninstall);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "configure", addon_kvs_cmd_configure);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "help", addon_kvs_cmd_help);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "installfiles", addon_kvs_cmd_installfiles);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "pack", addon_kvs_cmd_pack);

	KVSM_REGISTER_CALLBACK_COMMAND(m, "setconfigurecallback", addon_kvs_cmd_setconfigurecallback);
	KVSM_REGISTER_CALLBACK_COMMAND(m, "sethelpcallback", addon_kvs_cmd_sethelpcallback);
	KVSM_REGISTER_CALLBACK_COMMAND(m, "register", addon_kvs_cmd_register);

	QString szBuf;
	m->getDefaultConfigFileName(szBuf);
	KviConfigurationFile cfg(szBuf, KviConfigurationFile::Read);
	g_rectManagementDialogGeometry = cfg.readRectEntry("EditorGeometry", QRect(10, 10, 390, 440));

	return true;
}

static bool addon_module_cleanup(KviModule * m)
{
	AddonManagementDialog::cleanup();

	QString szBuf;
	m->getDefaultConfigFileName(szBuf);
	KviConfigurationFile cfg(szBuf, KviConfigurationFile::Write);
	cfg.writeEntry("EditorGeometry", g_rectManagementDialogGeometry);

	return true;
}

static bool addon_module_can_unload(KviModule *)
{
	return (!AddonManagementDialog::instance());
}

KVIRC_MODULE(
    "Addon", // module name
    "4.0.0", // module version
    "Copyright (C) 2005 Szymon Stefanek (pragma at kvirc dot net)\n"
    "              2008 Elvio Basello (hell at hellvis69 dot netsons dot org)", // author & (C)
    "Addon management functions for the KVS engine",
    addon_module_init,
    addon_module_can_unload,
    0,
    addon_module_cleanup,
    "addon")
