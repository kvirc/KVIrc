//=============================================================================
//
//   File : libkviconfig.cpp
//   Creation date : Thu Jan 31 2002 22:50:12 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma@kvirc.net)
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
#include "KviConfigurationFile.h"
#include "KviFileUtils.h"
#include "KviApplication.h"
#include "KviLocale.h"
#include "KviPointerHashTable.h"

#include <QString>

static KviPointerHashTable<QString, KviConfigurationFile> * g_pConfigDict = nullptr;
static int g_iNextConfigId = 0;

/*
	@doc: config.open
	@type:
		function
	@title:
		$config.open
	@short:
		Opens a config file
	@syntax:
		$config.open(<filename:string>[,<flags:string>])
	@description:
		Opens a config file.
		If the file already exists, its contents
		are parsed and available for extracting by the [fnc]$config.read[/fnc]() function.[br]
		<filename> may be an absolute path or a relative path: if a relative path is used,
		KVIrc will complete it with a local KVIrc directory suitable for writing script config files.[br]
		If the config file doesn't exist, it is opened as empty config file.[br]
		Flags can contain a combination of letters [b]r[/b] and [b]w[/b].[br]
		If only [b]r[/b] is specified, the config file is opened in [i]read-only[/i] mode: no changes will be written to disk.[br]
		If only [b]w[/b] is specified, the config file is opened in [i]write-only[/i] mode: the contents of the file on disk
		are not read.[br]
		If <flags> are not specified then [b][i]rw[/i][/b] is assumed.[br]
		The function returns an identifier for the open config file. This identifier
		is a mandatory parameter in all the other config.* functions and commands.[br]
		The config section is set to the default section name: you can change it by using
		[cmd]config.setsection[/cmd].[br]
		After you have finished using the file, don't forget to call [cmd]config.close[/cmd]!
		KVIrc tries to not open a config file twice: if the file was already opened then
		the identifier of the open file is returned.[br]
		You can check if a config file is already open by using the
		[fnc]$config.id[/fnc]() function.[br]
	@examples:
		[example]
			[comment]# It is a good idea to store the returned ID to a variable :)[/comment]
			%cfg = $config.open(myscript.kvc)
			%num = [fnc]$config.read[/fnc](%cfg,Number,0)
			[cmd]echo[/cmd] Number of entries is %num
			%num++;
			[cmd]config.write[/cmd] %cfg Number %num
			[cmd]config.close[/cmd] %cfg
		[/example]
	@seealso:
		[module:config]Config module documentation[/module]
*/

static bool config_kvs_fnc_open(KviKvsModuleFunctionCall * c)
{
	QString szFile;
	QString szMode;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("filename", KVS_PT_STRING, 0, szFile)
	KVSM_PARAMETER("mode", KVS_PT_STRING, KVS_PF_OPTIONAL, szMode)
	KVSM_PARAMETERS_END(c)

	KviConfigurationFile::FileMode fileMode;

	if(szMode.contains('r'))
	{
		if(szMode.contains('w'))
			fileMode = KviConfigurationFile::ReadWrite;
		else
			fileMode = KviConfigurationFile::Read;
	}
	else
	{
		if(szMode.contains('w'))
			fileMode = KviConfigurationFile::Write;
		else
			fileMode = KviConfigurationFile::ReadWrite;
	}

	KviFileUtils::adjustFilePath(szFile);
	QString szAbsFile;

	if(KviFileUtils::isAbsolutePath(szFile))
		szAbsFile = szFile;
	else
		g_pApp->getLocalKvircDirectory(szAbsFile, KviApplication::ConfigScripts, szFile, true);

	KviPointerHashTableIterator<QString, KviConfigurationFile> it(*g_pConfigDict);
	while(it.current())
	{
		if(KviQString::equalCI(it.current()->fileName(), szAbsFile))
		{
			c->returnValue()->setString(it.currentKey());
			if(it.current()->readOnly() && (fileMode & KviConfigurationFile::Write))
			{
				it.current()->setReadOnly(false);
			}
			return true;
		}
		++it;
	}

	KviConfigurationFile * cfg = new KviConfigurationFile(szAbsFile, fileMode);
	g_iNextConfigId++;
	QString tmp = QString("%1").arg(g_iNextConfigId);
	g_pConfigDict->insert(tmp, cfg);
	c->returnValue()->setString(tmp);
	return true;
}

/*
	@doc: config.id
	@type:
		function
	@title:
		$config.id
	@short:
		Returns the ID of an open config file
	@syntax:
		$config.id(<filename>)
	@description:
		Returns the ID of an open config file specified by <filename>.[br]
		(This is the same ID that is returned by [fnc]$config.open[/fnc].[br]
		If no such file is open, [b]0[/b] is returned (that is an invalid config identifier).[br]
	@seealso:
		[module:config]Config module documentation[/module]
*/

static bool config_kvs_fnc_id(KviKvsModuleFunctionCall * c)
{
	QString szFile;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("file", KVS_PT_STRING, 0, szFile)
	KVSM_PARAMETERS_END(c)

	KviFileUtils::adjustFilePath(szFile);

	QString szAbsFile;

	if(KviFileUtils::isAbsolutePath(szFile))
		szAbsFile = szFile;
	else
		g_pApp->getLocalKvircDirectory(szAbsFile, KviApplication::ConfigScripts, szFile, true);

	KviPointerHashTableIterator<QString, KviConfigurationFile> it(*g_pConfigDict);
	while(it.current())
	{
		if(KviQString::equalCI(it.current()->fileName(), szAbsFile))
		{
			c->returnValue()->setString(it.currentKey());
			return true;
		}
		++it;
	}

	c->returnValue()->setString("0");

	return true;
}

/*
	@doc: config.read
	@type:
		function
	@title:
		$config.read
	@short:
		Reads an entry from a config file
	@syntax:
		$config.read(<id:string>,<key:string>[,<default:string>])
	@description:
		Reads and returns the value associated to <key> in the current section of the config
		file identified by <id>.[br]
		<id> must be a valid config identifier returned by [fnc]$config.open[/fnc]()
		If no value is associated with <key> in the current section the return value
		is an empty string or the string <default> if it has been passed.[br]
	@seealso:
		[module:config]Config module documentation[/module]
*/

static bool config_kvs_fnc_read(KviKvsModuleFunctionCall * c)
{
	QString szId;
	QString szKey;
	QString szDefault;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("id", KVS_PT_STRING, 0, szId)
	KVSM_PARAMETER("key", KVS_PT_STRING, 0, szKey)
	KVSM_PARAMETER("default", KVS_PT_STRING, KVS_PF_OPTIONAL, szDefault)
	KVSM_PARAMETERS_END(c)

	KviConfigurationFile * cfg = g_pConfigDict->find(szId);

	if(cfg)
	{
		c->returnValue()->setString(cfg->readEntry(szKey, szDefault));
	}
	else
	{
		c->warning(__tr2qs("The config file with ID '%Q' is not open"), &szId);
	}
	return true;
}

/*
	@doc: config.section
	@type:
		function
	@title:
		$config.section
	@short:
		Returns the name of the current section of a config file
	@syntax:
		$config.section(<id>)
	@description:
		Returns the name of the current section in the config file identifier by <id>.[br]
		<id> must be a valid config identifier returned by [fnc]$config.open[/fnc]()
	@seealso:
		[module:config]Config module documentation[/module]
*/

static bool config_kvs_fnc_section(KviKvsModuleFunctionCall * c)
{
	QString szId;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("id", KVS_PT_STRING, 0, szId)
	KVSM_PARAMETERS_END(c)

	KviConfigurationFile * cfg = g_pConfigDict->find(szId);

	if(cfg)
	{
		c->returnValue()->setString(cfg->group());
	}
	else
	{
		c->warning(__tr2qs("The config file with ID '%Q' is not open"), &szId);
	}
	return true;
}

/*
	@doc: config.readonly
	@type:
		function
	@title:
		$config.readonly
	@short:
		Checks whether a config file is opened in read-only mode
	@syntax:
		$config.readonly(<id>)
	@description:
		Returns [b]1[/b] if the config file identified by <id> is opened in read-only mode,
		and [b]0[/b] otherwise.[br]
		<id> must be a valid config identifier returned by [fnc]$config.open[/fnc]()
		If <id> does not identify an open config file, a warning is printed and [b]0[/b] is returned.[br]
	@seealso:
		[module:config]Config module documentation[/module]
*/

static bool config_kvs_fnc_readonly(KviKvsModuleFunctionCall * c)
{
	QString szId;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("id", KVS_PT_STRING, 0, szId)
	KVSM_PARAMETERS_END(c)

	KviConfigurationFile * cfg = g_pConfigDict->find(szId);

	if(cfg)
	{
		c->returnValue()->setBoolean(cfg->readOnly());
	}
	else
	{
		c->warning(__tr2qs("The config file with ID '%Q' is not open"), &szId);
		c->returnValue()->setInteger(false);
	}
	return true;
}

/*
	@doc: config.filename
	@type:
		function
	@title:
		$config.filename
	@short:
		Returns the filename of an open config file
	@syntax:
		$config.filename(<id>)
	@description:
		Returns the filename of the [b]open[/b] config file identifier by <id>.[br]
		<id> must be a valid config identifier returned by [fnc]$config.open[/fnc]()
		If the <id> doesn't match any open file, an empty string is returned.[br]
	@seealso:
		[module:config]Config module documentation[/module]
*/

static bool config_kvs_fnc_filename(KviKvsModuleFunctionCall * c)
{
	QString szId;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("id", KVS_PT_STRING, 0, szId)
	KVSM_PARAMETERS_END(c)

	KviConfigurationFile * cfg = g_pConfigDict->find(szId);

	if(cfg)
	{
		c->returnValue()->setString(cfg->fileName());
	}
	return true;
}

/*
	@doc: config.hassection
	@type:
		function
	@title:
		$config.hassection
	@short:
		Checks whether a section is present in a config file
	@syntax:
		$config.hassection(<id>,<section_name>)
	@description:
		Returns [b]1[/b] if the section <section_name> exists in the config file identifier by <id>,
		and [b]0[/b] otherwise.[br]
		<id> must be a valid config identifier returned by [fnc]$config.open[/fnc]()
	@seealso:
		[module:config]Config module documentation[/module]
*/

static bool config_kvs_fnc_hassection(KviKvsModuleFunctionCall * c)
{
	QString szId;
	QString szSect;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("id", KVS_PT_STRING, 0, szId)
	KVSM_PARAMETER("id", KVS_PT_STRING, 0, szSect)
	KVSM_PARAMETERS_END(c)

	KviConfigurationFile * cfg = g_pConfigDict->find(szId);

	if(cfg)
	{
		c->returnValue()->setBoolean(cfg->hasGroup(szSect));
	}
	else
	{
		c->warning(__tr2qs("The config file with ID '%Q' is not open"), &szId);
	}
	return true;
}

/*
	@doc: config.sectionlist
	@type:
		function
	@title:
		$config.sectionlist
	@short:
		Returns a list of the sections in a config file
	@syntax:
		$config.sectionlist(<id>)
	@description:
		Returns an array of sections present in the config file identified by <id>.[br]
		<id> must be a valid config identifier returned by [fnc]$config.open[/fnc]()
	@seealso:
		[module:config]Config module documentation[/module]
*/

static bool config_kvs_fnc_sectionlist(KviKvsModuleFunctionCall * c)
{
	QString szId;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("id", KVS_PT_STRING, 0, szId)
	KVSM_PARAMETERS_END(c)

	KviConfigurationFile * cfg = g_pConfigDict->find(szId);

	if(cfg)
	{
		KviConfigurationFileIterator it(*(cfg->dict()));
		KviKvsArray * pArray = new KviKvsArray();
		int id = 0;
		while(it.current())
		{
			pArray->set(id++, new KviKvsVariant(it.currentKey()));
			++it;
		}
		c->returnValue()->setArray(pArray);
	}
	else
	{
		c->warning(__tr2qs("The config file with ID '%Q' is not open"), &szId);
	}
	return true;
}

/*
	@doc: config.keylist
	@type:
		function
	@title:
		$config.keylist
	@short:
		Returns a list of keys in a config file
	@syntax:
		$config.keylist(<id>)
	@description:
		Returns an array of keys present in the current section of the config file identified by <id>.[br]
		<id> must be a valid config identifier returned by [fnc]$config.open[/fnc]()
	@seealso:
		[module:config]Config module documentation[/module]
*/

static bool config_kvs_fnc_keylist(KviKvsModuleFunctionCall * c)
{
	QString szId;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("id", KVS_PT_STRING, 0, szId)
	KVSM_PARAMETERS_END(c)

	KviConfigurationFile * cfg = g_pConfigDict->find(szId);

	if(cfg)
	{
		KviConfigurationFileGroup * d = cfg->dict()->find(cfg->group());
		if(!d)
			return true;

		KviConfigurationFileGroupIterator it(*d);

		KviKvsArray * pArray = new KviKvsArray();
		int id = 0;

		while(it.current())
		{
			pArray->set(id++, new KviKvsVariant(it.currentKey()));
			++it;
		}
		c->returnValue()->setArray(pArray);
	}
	else
	{
		c->warning(__tr2qs("The config file with ID '%Q' is not open"), &szId);
	}
	return true;
}

/*
	@doc: config.filelist
	@type:
		function
	@title:
		$config.filelist
	@short:
		Returns a list of open config files
	@syntax:
		$config.filelist
	@description:
		Returns an array of open config files.[br]
	@seealso:
		[module:config]Config module documentation[/module]
*/

static bool config_kvs_fnc_filelist(KviKvsModuleFunctionCall * c)
{
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETERS_END(c)

	KviKvsArray * pArray = new KviKvsArray();
	int id = 0;

	KviPointerHashTableIterator<QString, KviConfigurationFile> it(*g_pConfigDict);
	while(it.current())
	{
		pArray->set(id++, new KviKvsVariant(it.currentKey()));
		++it;
	}
	c->returnValue()->setArray(pArray);
	return true;
}

/*
	@doc: config.close
	@type:
		command
	@title:
		config.close
	@keyterms:
		storing & reading configuration entries
	@short:
		Closes a config file
	@syntax:
		config.close [-q] <id>
	@description:
		Closes the config file identified by <id>.[br]
		<id> must be a a valid config file identifier returned by [fnc]$config.open[/fnc].[br]
		If the config file was opened as read-write (default), the changes will be stored
		to disk. If the config was opened as read-only, changes will not be written.[br]
		If the <id> does not match any open config file, a warning is printed unless
		the -q (quiet) switch is used.[br]
	@seealso:
		[module:config]Config module documentation[/module]
*/

static bool config_kvs_cmd_close(KviKvsModuleCommandCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("id", KVS_PT_STRING, 0, szId)
	KVSM_PARAMETERS_END(c)

	KviConfigurationFile * cfg = g_pConfigDict->find(szId);

	if(cfg)
	{
		if(cfg->readOnly())
		{
			if(cfg->dirty())
			{
				if(!c->hasSwitch('q', "quiet"))
					c->warning(__tr2qs("The config file '%Q' has been changed but is opened as read-only: changes will be lost"), &cfg->fileName());
			}
		}
		else
		{
			// we force a save here
			if(!cfg->sync())
				if(!c->hasSwitch('q', "quiet"))
					c->warning(__tr2qs("An error has occurred while trying to save the config file with ID '%Q'"), &szId);
		}
		g_pConfigDict->remove(szId);
	}
	else
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs("The config file with ID '%Q' is not open"), &szId);
	}

	return true;
}

/*
	@doc: config.flush
	@type:
		command
	@title:
		config.flush
	@keyterms:
		storing & reading configuration entries
	@short:
		Flushes a config file
	@syntax:
		config.flush <id>
	@description:
		Flushes the config file identified by <id>.[br]
		<id> must be a a valid config file identifier returned by [fnc]$config.open[/fnc].[br]
		If the config file was opened as read-write (default), the changes will be stored
		to disk. If the config was opened as read-only, an error is printed.[br]
		If the <id> does not match any open config file, a warning is printed.
	@seealso:
		[module:config]Config module documentation[/module]
*/

static bool config_kvs_cmd_flush(KviKvsModuleCommandCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("id", KVS_PT_STRING, 0, szId)
	KVSM_PARAMETERS_END(c)

	KviConfigurationFile * cfg = g_pConfigDict->find(szId);

	if(cfg)
	{
		if(cfg->readOnly())
			c->warning(__tr2qs("The config file with ID '%Q' is read-only"), &szId);
		else if(!cfg->sync())
			c->warning(__tr2qs("An error has occurred while trying to save the config file with ID '%Q'"), &szId);
	}
	else
	{
		c->warning(__tr2qs("The config file with ID '%Q' is not open"), &szId);
	}

	return true;
}

/*
	@doc: config.clear
	@type:
		command
	@title:
		config.clear
	@keyterms:
		storing & reading configuration entries
	@short:
		Clears a config file
	@syntax:
		config.clear <id>
	@description:
		Clears the contents of the config file identified by <id>.[br]
		All the sections are removed (except for the default section) and all the key=value
		pairs are destroyed.[br]
		<id> must be a a valid config file identifier returned by [fnc]$config.open[/fnc].[br]
		If the <id> does not match any open config file, a warning is printed.
	@seealso:
		[module:config]Config module documentation[/module]
*/

static bool config_kvs_cmd_clear(KviKvsModuleCommandCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("id", KVS_PT_STRING, 0, szId)
	KVSM_PARAMETERS_END(c)

	KviConfigurationFile * cfg = g_pConfigDict->find(szId);

	if(cfg)
	{
		cfg->clear();
	}
	else
	{
		c->warning(__tr2qs("The config file with ID '%Q' is not open"), &szId);
	}

	return true;
}

/*
	@doc: config.clearsection
	@type:
		command
	@title:
		config.clearsection
	@keyterms:
		storing & reading configuration entries
	@short:
		Clears a section in a config file
	@syntax:
		config.clearsection <id> <section_name>
	@description:
		Clears the contents of the section <section_name> in the config file identified by <id>.[br]
		All the key=value pairs in the section are destroyed.[br]
		<id> must be a a valid config file identifier returned by [fnc]$config.open[/fnc].[br]
		If the <id> does not match any open config file, a warning is printed.
		If <section_name> is also the current section, the default section is made current.[br]
		No warning is printed if the section does not exist.[br]
	@seealso:
		[module:config]Config module documentation[/module]
*/

static bool config_kvs_cmd_clearsection(KviKvsModuleCommandCall * c)
{
	QString szId;
	QString szSect;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("id", KVS_PT_STRING, 0, szId)
	KVSM_PARAMETER("section", KVS_PT_STRING, 0, szSect)
	KVSM_PARAMETERS_END(c)

	KviConfigurationFile * cfg = g_pConfigDict->find(szId);

	if(cfg)
	{
		cfg->clearGroup(szSect);
	}
	else
	{
		c->warning(__tr2qs("The config file with ID '%Q' is not open"), &szId);
	}

	return true;
}

/*
	@doc: config.write
	@type:
		command
	@title:
		config.write
	@keyterms:
		storing & reading configuration entries
	@short:
		Writes a data field to a config file
	@syntax:
		config.write <id> <key> <value>
	@description:
		Writes the <key>=<value> pair to the config file identifier by <id>.[br]
		<id> must be a valid config file ID returned by [fnc]$config.open[/fnc]().[br]
		<key> and <value> can be any strings.[br]
		The <key>=<value> pair is written in the current section of the config file.[br]
		If <key> already exists in the current section of the config, the corresponding value is replaced with <value>.[br]
		If <value> is an empty string, the <key> is simply removed from the current section.[br]
		If a section remains empty (with no keys) at file write time, that section will be removed.[br]
		The changes are [b]not[/b] written to disk: you must call [cmd]config.flush[/cmd] or (better) [cmd]config.close[/cmd]
		for the changes to be effectively written.[br]
	@seealso:
		[module:config]Config module documentation[/module]
*/

static bool config_kvs_cmd_write(KviKvsModuleCommandCall * c)
{
	QString szId;
	QString szKey;
	QString szVal;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("id", KVS_PT_STRING, 0, szId)
	KVSM_PARAMETER("key", KVS_PT_STRING, 0, szKey)
	KVSM_PARAMETER("value", KVS_PT_STRING, 0, szVal)
	KVSM_PARAMETERS_END(c)

	KviConfigurationFile * cfg = g_pConfigDict->find(szId);

	if(cfg)
	{
		if(!szVal.isEmpty())
			cfg->writeEntry(szKey, szVal);
		else
			cfg->clearKey(szKey);
	}
	else
	{
		c->warning(__tr2qs("The config file with ID '%Q' is not open"), &szId);
	}

	return true;
}

/*
	@doc: config.setsection
	@type:
		command
	@title:
		config.setsection
	@keyterms:
		storing & reading configuration entries
	@short:
		Changes the current section of a config file
	@syntax:
		config.setsection <id> <section_name>
	@description:
		Changes the current section of the config file <id> to <section_name>.[br]
		Sections are effectively namespaces for the keys: each section can contain
		a set of keys and equal keys in different sections do not collide.[br]
		<id> must be a valid config file ID returned by [fnc]$config.open[/fnc]().[br]
	@seealso:
		[module:config]Config module documentation[/module]
*/

static bool config_kvs_cmd_setsection(KviKvsModuleCommandCall * c)
{
	QString szId;
	QString szSect;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("id", KVS_PT_STRING, 0, szId)
	KVSM_PARAMETER("section", KVS_PT_STRING, 0, szSect)
	KVSM_PARAMETERS_END(c)

	KviConfigurationFile * cfg = g_pConfigDict->find(szId);

	if(cfg)
	{
		cfg->setGroup(szSect);
	}
	else
	{
		c->warning(__tr2qs("The config file with ID '%Q' is not open"), &szId);
	}

	return true;
}

/*
	@doc: config
	@type:
		module
	@short:
		Managing of configuration files
	@title:
		The config module
	@body:
		The config module is a powerful interface that manages text configuration files.[br]
		The files are arranged in named sections and the data is stored in key=value pairs
		belonging to each section.[br]
		[br]
		[section1_name][br]
		<key1>=<value1>[br]
		<key2>=<value2>[br]
		<key3>=<value3>[br]
		....[br]
		[section2_name][br]
		<key1>=<value1>[br]
		....[br]
		[br]
		Keys and values are plain text strings - newlines in values are encoded using
		a simple hexadecimal notation.[br]
		The config files can be also edited by using any text editor.[br]
		The config module works on config files loaded in memory. For this reason,
		you must first open a config file by [fnc]$config.open[/fnc]() - if the file exists
		on disk, its contents are loaded and you can access the data stored in it, otherwise
		an empty config file is created in memory.[br]
		The config file is opened in read-write mode by default. You can also choose
		the "read-only" method - a read-only config file is never flushed to disk (but you
		can still set values in memory!).[br]
		It is not a good idea to keep a config file open forever. A better approach is to
		keep settings in memory variables - when you want to write to disk, open the config
		file, write all your values and then close the file.[br]
		You can obviously keep the config file open for some time but remember that
		no changes are written to the disk until [cmd]config.flush[/cmd] or [cmd]config.close[/cmd] is called.[br]
		You write entries by using [cmd]config.write[/cmd] and read them by using [fnc]$config.read[/fnc]().[br]
		You can change the "current" config section by using [cmd]config.setsection[/cmd].[br]
		The following lists all commands and functions exported by this module:[br]
		[fnc]$config.open[/fnc][br]
		[fnc]$config.read[/fnc][br]
		[fnc]$config.section[/fnc][br]
		[fnc]$config.hassection[/fnc][br]
		[fnc]$config.sectionlist[/fnc][br]
		[fnc]$config.keylist[/fnc][br]
		[fnc]$config.filelist[/fnc][br]
		[fnc]$config.filename[/fnc][br]
		[fnc]$config.id[/fnc][br]
		[fnc]$config.readonly[/fnc][br]
		[cmd]config.close[/cmd][br]
		[cmd]config.flush[/cmd][br]
		[cmd]config.clear[/cmd][br]
		[cmd]config.clearsection[/cmd][br]
		[cmd]config.setsection[/cmd][br]
		[cmd]config.write[/cmd][br]
*/

static bool config_module_init(KviModule * m)
{
	g_pConfigDict = new KviPointerHashTable<QString, KviConfigurationFile>;
	g_pConfigDict->setAutoDelete(true);

	KVSM_REGISTER_FUNCTION(m, "open", config_kvs_fnc_open);
	KVSM_REGISTER_FUNCTION(m, "id", config_kvs_fnc_id);
	KVSM_REGISTER_FUNCTION(m, "read", config_kvs_fnc_read);
	KVSM_REGISTER_FUNCTION(m, "section", config_kvs_fnc_section);
	KVSM_REGISTER_FUNCTION(m, "hassection", config_kvs_fnc_hassection);
	KVSM_REGISTER_FUNCTION(m, "sectionlist", config_kvs_fnc_sectionlist);
	KVSM_REGISTER_FUNCTION(m, "keylist", config_kvs_fnc_keylist);
	KVSM_REGISTER_FUNCTION(m, "filelist", config_kvs_fnc_filelist);
	KVSM_REGISTER_FUNCTION(m, "filename", config_kvs_fnc_filename);
	KVSM_REGISTER_FUNCTION(m, "readonly", config_kvs_fnc_readonly);

	KVSM_REGISTER_SIMPLE_COMMAND(m, "close", config_kvs_cmd_close);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "flush", config_kvs_cmd_flush);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "clear", config_kvs_cmd_clear);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "clearsection", config_kvs_cmd_clearsection);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setsection", config_kvs_cmd_setsection);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "write", config_kvs_cmd_write);

	return true;
}

static bool config_module_cleanup(KviModule *)
{
	delete g_pConfigDict;
	g_pConfigDict = nullptr;
	return true;
}

static bool config_module_can_unload(KviModule *)
{
	return g_pConfigDict->isEmpty();
}

KVIRC_MODULE(
    "Config",                                                       // module name
    "4.0.0",                                                        // module version
    "Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
    "Config file system for KVIrc",
    config_module_init,
    config_module_can_unload,
    0,
    config_module_cleanup,
    0)
