//=============================================================================
//
//   File : libkvisystem.cpp
//   Creation date : Fri Nov 16 03:50:12 2001 GMT by Szymon Stefanek
//
//   This system is part of the KVIrc irc client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright ©        2009 Kai Wasserbäch <debian@carbon-project.org>
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

#include "Plugin.h"

#include "kvi_settings.h"
#include "KviModule.h"
#include "KviCString.h"
#include "KviThread.h"
#include "KviLocale.h"
#include "KviApplication.h"
#include "KviEnvironment.h"
#include "KviRuntimeInfo.h"
#include "KviModuleManager.h"
#include "KviByteOrder.h"

#include <QClipboard>
#include <QByteArray>

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
#include <sys/utsname.h>
#include <cstdlib>
#include <unistd.h>
#endif

#ifdef COMPILE_DBUS_SUPPORT
#include <QtDBus/QtDBus>
#endif

#ifdef COMPILE_KDE_SUPPORT
#include <KToolInvocation> // invokeTerminal() for system.runcmd
#else                      // tools we need to work around the absence of
// invokeTerminal()
#include <QProcess>
#include <QStringList>
#endif

PluginManager * g_pPluginManager;

/*
	@doc: system.ostype
	@type:
		function
	@title:
		$system.ostype
	@short:
		Returns the type of the operating system
	@syntax:
		<string> $system.ostype()
	@description:
		Returns the current type of operating system: UNIX, macOS or Windows.[br]
*/

static bool system_kvs_fnc_ostype(KviKvsModuleFunctionCall * c)
{
	c->returnValue()->setString(KviRuntimeInfo::type());
	return true;
}
/*
	@doc: system.osname
	@type:
		function
	@title:
		$system.osname
	@short:
		Returns the name of the operating system
	@syntax:
		<string> $system.osname()
	@description:
		Returns the name of the operating system. On UNIX like machines
		this is the field sysname of the utsname structure returned by
		the uname() syscall.
*/

static bool system_kvs_fnc_osname(KviKvsModuleFunctionCall * c)
{
	c->returnValue()->setString(KviRuntimeInfo::name());
	return true;
}

/*
	@doc: system.osversion
	@type:
		function
	@title:
		$system.osversion
	@short:
		Returns the version of the operating system
	@syntax:
		<string> $system.osversion()
	@description:
		Returns the version of the operating system.[br]
*/

static bool system_kvs_fnc_osversion(KviKvsModuleFunctionCall * c)
{
	// no params to process
	c->returnValue()->setString(KviRuntimeInfo::version());
	return true;
}

/*
	@doc: system.osrelease
	@type:
		function
	@title:
		$system.osrelease
	@short:
		Returns the release of the operating system
	@syntax:
		<string> $system.osrelease()
	@description:
		Returns the release of the operating system.[br]
*/

static bool system_kvs_fnc_osrelease(KviKvsModuleFunctionCall * c)
{
	// no params to process
	c->returnValue()->setString(KviRuntimeInfo::release());
	return true;
}

/*
	@doc: system.osmachine
	@type:
		function
	@title:
		$system.osmachine
	@short:
		Returns the machine of the operating system
	@syntax:
		<string> $system.osmachine()
	@description:
		Returns the machine of the operating system.[br]
*/

static bool system_kvs_fnc_osmachine(KviKvsModuleFunctionCall * c)
{
	// no params to process
	c->returnValue()->setString(KviRuntimeInfo::machine());
	return true;
}

/*
	@doc: system.osnodename
	@type:
		function
	@title:
		$system.osnodename
	@short:
		Returns the nodename of the operating system
	@syntax:
		<string> $system.osnodename()
	@description:
		Returns the nodename of the operating system.[br]
*/

static bool system_kvs_fnc_osnodename(KviKvsModuleFunctionCall * c)
{
	// no params to process
	c->returnValue()->setString(KviRuntimeInfo::nodename());
	return true;
}

/*
	@doc: system.getenv
	@keyterms:
		Environment variables
	@type:
		function
	@title:
		$system.getenv
	@short:
		Returns the value of an environment variable
	@syntax:
		<string> $system.getenv(<variable:string>)
	@description:
		Returns the value of the environment <variable>.[br]
*/

static bool system_kvs_fnc_getenv(KviKvsModuleFunctionCall * c)
{
	QString szVariable;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("variable", KVS_PT_NONEMPTYSTRING, 0, szVariable)
	KVSM_PARAMETERS_END(c)

	c->returnValue()->setString(KviEnvironment::getVariable(szVariable));
	return true;
}

/*
	@doc: system.clipboard
	@keyterms:
		Clipboard management
	@type:
		function
	@title:
		$system.clipboard
	@short:
		Returns the value of the system clipboard
	@syntax:
		<string> $system.clipboard()
	@description:
		Returns the current value of the system clipboard.
		Please note that there are systems that have the concept
		of "selection" (most notably X11) which is [b]not[/b] the same as the clipboard.
		See [fnc]$system.selection[/fnc]().
	@seealso:
		[fnc]$system.selection[/fnc],
		[cmd]system.setClipboard[/cmd],
		[cmd]system.setSelection[/cmd]
*/

static bool system_kvs_fnc_clipboard(KviKvsModuleFunctionCall * c)
{
	c->returnValue()->setString(g_pApp->clipboard()->text(QClipboard::Clipboard));
	return true;
}

/*
	@doc: system.setClipboard
	@type:
		command
	@title:
		system.setClipboard
	@keyterms:
		Clipboard management
	@short:
		Sets the current system clipboard contents
	@syntax:
		system.setClipboard <data:string>
	@description:
		Sets the system clipboard contents to the string <data>.
		Please note that there are systems that have the concept
		of "selection" (most notably X11) which is [b]not[/b] the same as the clipboard.
		See [fnc]$system.selection[/fnc]().
	@seealso:
		[fnc]$system.selection[/fnc],
		[fnc]$system.clipboard[/fnc],
		[cmd]system.setSelection[/cmd]
*/

static bool system_kvs_cmd_setClipboard(KviKvsModuleCommandCall * c)
{
	QString szValue;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("data", KVS_PT_STRING, KVS_PF_OPTIONAL, szValue)
	KVSM_PARAMETERS_END(c)
	g_pApp->clipboard()->setText(szValue, QClipboard::Clipboard);
	return true;
}

/*
	@doc: system.setSelection
	@type:
		command
	@title:
		system.setSelection
	@keyterms:
		Clipboard management
	@short:
		Sets the current system selection contents
	@syntax:
		system.setSelection <data:string>
	@description:
		Sets the system selection contents to the string <data>.
		Please note that this command will work only on systems that have the concept
		of "selection" (most notably X11) which is [b]not[/b] the same as the clipboard.
	@seealso:
		[fnc]$system.selection[/fnc],
		[fnc]$system.clipboard[/fnc],
		[cmd]system.setClipboard[/cmd]
*/

static bool system_kvs_cmd_setSelection(KviKvsModuleCommandCall * c)
{
	QString szValue;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("data", KVS_PT_STRING, KVS_PF_OPTIONAL, szValue)
	KVSM_PARAMETERS_END(c)
	g_pApp->clipboard()->setText(szValue, QClipboard::Selection);
	return true;
}

/*
	@doc: system.selection
	@keyterms:
		Clipboard management
	@type:
		function
	@title:
		$system.selection
	@short:
		Returns the value of the system selection
	@syntax:
		<string> $system.selection()
	@description:
		Returns the current value of the system selection.
		This function will work only on systems that have the concept
		of "selection" (most notably X11) which is [b]not[/b] the same as clipboard.
		On other systems this function will always return an empty string.
		See [fnc]$system.clipboard[/fnc]().
	@seealso:
		[fnc]$system.clipboard[/fnc],
		[cmd]system.setClipboard[/cmd],
		[cmd]system.setSelection[/cmd]
*/

static bool system_kvs_fnc_selection(KviKvsModuleFunctionCall * c)
{
	c->returnValue()->setString(g_pApp->clipboard()->text(QClipboard::Selection));
	return true;
}

/*
	@doc: system.checkModule
	@keyterms:
		Module checking
	@type:
		function
	@title:
		$system.checkModule
	@short:
		Checks if a KVIrc module is loadable
	@syntax:
		<boolean> $system.checkModule(<module_name:string>)
	@description:
		Attempts to load the specified module and returns
		$true if successful and $false otherwise. This can
		be effectively used to test if a KVIrc extension module
		is present on the system and can be loaded by the
		KVIrc engine.
*/

static bool system_kvs_fnc_checkModule(KviKvsModuleFunctionCall * c)
{
	QString szModuleName;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("module_name", KVS_PT_STRING, 0, szModuleName)
	KVSM_PARAMETERS_END(c)

	c->returnValue()->setBoolean(g_pModuleManager->loadModule(szModuleName));
	return true;
}

/*
	@doc: system.hostname
	@keyterms:
		System information
	@type:
		function
	@title:
		$system.hostname
	@short:
		Returns the hostname of the machine that KVIrc is running on
	@syntax:
		<string> $system.hostname()
	@description:
		Returns the hostname of the machine that KVIrc is running on.[br]
*/

static bool system_kvs_fnc_hostname(KviKvsModuleFunctionCall * c)
{
	// no params to process
	c->returnValue()->setString(KviRuntimeInfo::hostname());
	return true;
}

/*
	@doc: system.dbus
	@keyterms:
		System information
	@type:
		function
	@title:
		$system.dbus
	@short:
		Performs a D-Bus call
	@syntax:
		<variant> $system.dbus(<service:string>,<path:string>,<interface:string>,<method:string>[,<bus_type:string>[,<parameter1:string>[,<parameter2:string>[,...]]]])
	@description:
		This function allows performing simple D-Bus calls without executing
		an external process. This feature is available ONLY under unix:
		this means that this function is absolutely
		non portable (don't use it in scripts that you're going to distribute).
		<application> is the name of the application being called, <objectid> is the
		identifier of the object called, <function> is the function to be executed
		on the remote object and <parameter1>,<parameter2>,... is the list of
		parameters to be passed. The <function> name must contain the
		trailing parenthesis and parameter specification (see examples).
		The <bus_type> specifies the bus to connect, use "system" for systemBus or
		"session" for sessionBus. If it's leaved blank, it will use sessionBus.
		The parameters MUST be in the form "type=value"
		where "type" is the C++ type of the parameter and value
		is the string representation of the parameter data. Currently
		KVIrc supports only QString,QByteArray,bool,int and uint data types.[br]
		The returned value is the string representation of the returned
		data if the return type is known, otherwise it is the name of the data type returned.
		[br]
		If the service name is prefixed with "?" then the call is performed in "remote test"
		mode: no "remote" errors are printed and the function returns 1 if the call executed
		successfully and 0 if the call failed. This can be used with the very first
		call to programmaticaly test if the remote application is running.
	@examples:
		[example]
			#get a string list of torrents currently in queue in ktorrent
			echo $system.dbus("org.ktorrent.ktorrent","/KTorrent", "org.ktorrent.KTorrent","torrents")
			#stop a torrent in ktorrent by its name
			echo $system.dbus("org.ktorrent.ktorrent","/KTorrent", "org.ktorrent.KTorrent","stop","QString=Torrent Name")
			#start the kde task manager
			echo $system.dbus("org.freedesktop.ScreenSaver","/App", "org.kde.krunner.App","showTaskManager")
			#get the clipboard contents using dbus
			echo $system.dbus("org.kde.klipper","/klipper", "org.kde.klipper.klipper","getClipboardContents")
		[/example]
*/

static bool system_kvs_fnc_dbus(KviKvsModuleFunctionCall * c)
{
	QString szService, szPath, szInterface, szMethod, szBusType;
	QStringList parms;
	bool bRemoteTest = false;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("service", KVS_PT_NONEMPTYSTRING, 0, szService)
	KVSM_PARAMETER("path", KVS_PT_NONEMPTYSTRING, 0, szPath)
	KVSM_PARAMETER("interface", KVS_PT_NONEMPTYSTRING, 0, szInterface)
	KVSM_PARAMETER("method", KVS_PT_NONEMPTYSTRING, 0, szMethod)
	KVSM_PARAMETER("bus_type", KVS_PT_STRING, KVS_PF_OPTIONAL, szBusType)
	KVSM_PARAMETER("parameter_list", KVS_PT_STRINGLIST, KVS_PF_OPTIONAL, parms)
	KVSM_PARAMETERS_END(c)

#ifdef COMPILE_DBUS_SUPPORT

	if(szBusType.isEmpty())
		szBusType = "session";

	QDBusConnection busType("");

	if(szBusType == "system")
	{
		busType = QDBusConnection::systemBus();
	}
	else if(szBusType == "session")
	{
		busType = QDBusConnection::sessionBus();
	}
	else
	{
		c->warning(__tr2qs("No DBus type specified"));
		return false;
	}

	if(szService.left(1).compare("?") == 0)
	{
		szService.remove(0, 1);
		bRemoteTest = true;
	}

	QDBusInterface remoteApp(szService, szPath, szInterface, busType);
	if(!remoteApp.isValid())
	{
		if(bRemoteTest)
		{
			c->returnValue()->setInteger(0);
			return true;
		}
		else
		{
			c->warning(__tr2qs("Invalid D-Bus interface"));
			return false;
		}
	}

	if(bRemoteTest)
	{
		c->returnValue()->setInteger(1);
		return true;
	}

	QList<QVariant> ds;

	for(auto & parm : parms)
	{
		KviCString tmp = parm;

		if(tmp.isEmpty())
		{
			c->warning(__tr2qs("Invalid D-Bus parameter syntax"));
			return false;
		}

		KviCString szType = tmp.leftToFirst('=', false);
		tmp.cutToFirst('=', true);
		if(szType.isEmpty())
			szType = "int";
		bool bOk;
		if(kvi_strEqualCI("int", szType.ptr()) || kvi_strEqualCI("long", szType.ptr()))
		{
			int iii = tmp.toInt(&bOk);
			if(!bOk)
			{
				c->warning(__tr2qs("The specified parameter is not an integer"));
				return false;
			}
			ds << iii;
		}
		else if(kvi_strEqualCI("QString", szType.ptr()))
		{
			QString ddd = tmp.ptr();
			ds << ddd;
		}
		else if(kvi_strEqualCI("QByteArray", szType.ptr()))
		{
			QByteArray qcs = tmp.ptr();
			ds << qcs;
		}
		else if(kvi_strEqualCI("bool", szType.ptr()))
		{
			bool bbb = kvi_strEqualCI(tmp.ptr(), "true");
			ds << bbb;
		}
		else if(kvi_strEqualCI("unsigned int", szType.ptr()) || kvi_strEqualCI("uint", szType.ptr()) || kvi_strEqualCI("Q_UINT32", szType.ptr()))
		{
			unsigned int uii = tmp.toUInt(&bOk);
			if(!bOk)
			{
				c->warning(__tr2qs("The specified parameter is not an integer"));
				return false;
			}
			ds << uii;
		}
		else
		{
			c->warning(__tr2qs("Unsupported D-Bus parameter type %s"), tmp.ptr());
			return false;
		}
	}

	QDBusMessage reply = remoteApp.callWithArgumentList(QDBus::Block, szMethod, ds);

	if(reply.type() == QDBusMessage::ErrorMessage)
	{
		QDBusError err = reply;
		c->warning(__tr2qs("The D-Bus call returned an error \"%s\": %s"), qPrintable(err.name()), qPrintable(err.message()));
		return false;
	}

	QString szRetType;
	foreach(QVariant v, reply.arguments())
	{
		switch(v.type())
		{
			case QVariant::Bool:
				c->returnValue()->setInteger(v.toBool() ? 1 : 0);
				break;
			case QVariant::String:
				c->returnValue()->setString(v.toString());
				break;
			case QVariant::ByteArray:
				c->returnValue()->setString(v.toByteArray().data());
				break;
			case QVariant::UInt:
				c->returnValue()->setInteger(v.toUInt());
				break;
			case QVariant::Int:
				c->returnValue()->setInteger(v.toInt());
				break;
			case QVariant::StringList:
			{
				QStringList csl(v.toStringList());
				KviKvsArray * arry = new KviKvsArray();
				int idx = 0;
				for(auto & iter : csl)
				{
					arry->set(idx, new KviKvsVariant(iter));
					idx++;
				}
				c->returnValue()->setArray(arry);
				break;
			}
			case QVariant::Invalid:
				//method returns void
				c->returnValue()->setString("");
				break;
			default:
				c->warning(__tr2qs("Unsupported D-Bus call return type %s"), v.typeName());
				break;
		}
	}

#else
	c->warning(__tr2qs("D-Bus calls are available only under UNIX"));
#endif
	return true;
}

/*
	@doc: system.setenv
	@type:
		command
	@title:
		system.setenv
	@keyterms:
		Environment variables
	@short:
		Sets an environment variable
	@syntax:
		system.setenv <variable:string> [<value:string>]
	@description:
		Sets the environment <variable> to the <value> string.[br]
		If <value> is not given, the <variable> is unset.[br]
	@seealso:
		[fnc]$system.getenv[/fnc]
*/

static bool system_kvs_cmd_setenv(KviKvsModuleCommandCall * c)
{
	QString szVariable, szValue;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("variable", KVS_PT_NONEMPTYSTRING, 0, szVariable)
	KVSM_PARAMETER("value", KVS_PT_STRING, KVS_PF_OPTIONAL, szValue)
	KVSM_PARAMETERS_END(c)

	if(szValue.isEmpty())
		KviEnvironment::unsetVariable(szVariable);
	else
		KviEnvironment::setVariable(szVariable, szValue);
	return true;
}

/*
	@doc: system.call
	@keyterms:
		call plugin
	@type:
		function
	@title:
		$system.call
	@short:
		Allows to call functions of a plugin
	@syntax:
		<string> $system.call(<plugin:string>, <function:string>[,<parameters:string>,...])
	@description:
		This function allows you to call simple functions of "easyplugins" (dll/so) and
		get the result of this function. The easyplugins have to be coded in a special way.
		If you want to write your own easyplugins, have a look on the easyplugins documentation.
		[br]
		[br]
		The function needs a minimum of 2 parameters. The others are optional and not limited.[br]
		The first one is the complete name like "example.dll" or "example.so"
		If you have a relative path or only the filename KVIrc looks for the easyplugins in the following dirs:[br]
		In a subdir called "easyplugins" in the local and global KVIrc directory.
		If you give an absolute path KVIrc will load it directly.
		[br]
		[br]
		You are free to add more parameters, they will be given to the easyplugin.[br]
		The easyplugin decides how much parameters you need in addition to the first 2.
		[br]
		[br]
		[b]Warning:  Only use easyplugins of known and trustable sources![/b]
	@examples:
		[example]
			echo $system.call("my.dll","myfunction")
			echo $system.call("my.dll","myfunction","parameter1","parameter2")
			echo $system.call("c:/my.dll","myfunction")
			echo $system.call("my.so","myfunction","parameter1")
		[/example]
*/

static bool system_kvs_fnc_plugin_call(KviKvsModuleFunctionCall * c)
{
	return g_pPluginManager->pluginCall(c);
}

/*
	@doc: system.runcmd
	@type:
		command
	@title:
		system.runcmd
	@short:
		Runs the specified command in a terminal
	@syntax:
		system.runcmd [-t <terminal:string>] <command:string>
	@switches:
		!sw: -t=<terminal> | --terminal=<terminal>
		Use the specified terminal to launch the command instead
		of the auto-detected one.
	@description:
		Tries to run the given command in the system's default (X)
		terminal (emulator) or in the terminal specified by the -t
		option.
		[br]
		[br]
		[b]Warning: Execute only harmless commands through this![/b]
	@examples:
		[example]
			system.runcmd "ping $0"
		[/example]
*/

// implements https://github.com/kvirc/KVIrc/issues/459
static bool system_kvs_cmd_runcmd(KviKvsModuleCommandCall * c)
{
	QString szCommand;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("command", KVS_PT_NONEMPTYSTRING, KVS_PF_APPENDREMAINING, szCommand)
	KVSM_PARAMETERS_END(c)

	if(szCommand.isEmpty())
		return c->error(__tr2qs("No command specified"));

	QString szTerminal;
	QStringList args;
	QProcess oProc;

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW) // Only »cmd.exe /k« in the list.
	args << "/k" << szCommand;
#elif defined(COMPILE_ON_MAC)
	args << "-e" << QString("tell application \"Terminal\" to do script \"%1\"").arg(szCommand);
#else
	args << "-e" << szCommand;
#endif // endif COMPILE_ON_WINDOWS

	if(c->switches()->getAsStringIfExisting('t', "terminal", szTerminal))
	{
		if(!oProc.startDetached(szTerminal, args))
			return c->error(__tr2qs("Failed to start the terminal program"));
		return true;
	}

#ifdef COMPILE_KDE_SUPPORT // We have invokeTerminal().

	KToolInvocation::invokeTerminal(szCommand.toLocal8Bit());

#else                                                        // No invokeTerminal() for us, we'll use a
	// combination of QStringList and QProcess.
	QStringList szTerminals;
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW) // Only »cmd.exe /k« in the list.
	szTerminals.append("cmd.exe");
#elif defined(COMPILE_ON_MAC)
	szTerminals.append("osascript");
#else
	// Where the heck is xdg-terminal?
	// List: gnome-terminal, x-terminal-emulator, terminal, xterm
	szTerminals.append("gnome-terminal");
	szTerminals.append("x-terminal-emulator");
	szTerminals.append("terminal");
	szTerminals.append("xterm");
#endif // endif COMPILE_ON_WINDOWS

	QString szTerm;
	while(!szTerminals.isEmpty())
	{
		szTerm = szTerminals.takeFirst();
		if(oProc.startDetached(szTerm, args))
			return true;
	}
#endif // endif COMPILE_KDE_SUPPORT

	return c->error(__tr2qs("Failed to start the terminal program"));
}

/*
	@doc: system.ntohi
	@type:
		function
	@title:
		$system.ntohi
	@short:
		Convert an integer from network to host byte order.
	@syntax:
		<integer> $system.ntohi(<value:integer>)
		<integer> $system.ntohi(<value:integer>,<bytecount:integer>)
	@description:
		Converts the integer value in network byte order
		to it's host byte order counterpart.
		Since the conversion is carried at binary representation
		level you must specify the number of bytes you want
		the binary representation to have. Valid values are
		1 (no conversion), 2 (short), 4 (32 bit int) and 8 (64 bit int).
		If omitted, bytecount is assumed to be 4.
*/

static bool system_kvs_fnc_ntohi(KviKvsModuleFunctionCall * c)
{
	kvs_int_t iValue;
	kvs_uint_t uBytes;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("value", KVS_PT_INT, 0, iValue)
	KVSM_PARAMETER("bytecount", KVS_PT_UINT, KVS_PF_OPTIONAL, uBytes)
	KVSM_PARAMETERS_END(c)

	switch(uBytes)
	{
		case 0:
			c->returnValue()->setInteger((kvs_int_t)KviByteOrder::networkByteOrderToLocalCpu32((kvi_u32_t)iValue));
			break;
		case 1:
			c->returnValue()->setInteger((kvs_int_t)((kvi_u8_t)iValue));
			break;
		case 2:
			c->returnValue()->setInteger((kvs_int_t)KviByteOrder::networkByteOrderToLocalCpu16((kvi_u16_t)iValue));
			break;
		case 4:
			c->returnValue()->setInteger((kvs_int_t)KviByteOrder::networkByteOrderToLocalCpu32((kvi_u32_t)iValue));
			break;
		case 8:
			c->returnValue()->setInteger((kvs_int_t)KviByteOrder::networkByteOrderToLocalCpu64((kvi_u64_t)iValue));
			break;
		default:
			return c->error(__tr2qs("Bad bytecount specification: 1, 2, 4 and 8 are allowed"));
			break;
	}

	return true;
}

/*
	@doc: system.htoni
	@type:
		function
	@title:
		$system.htoni
	@short:
		Convert an integer from host to network byte order.
	@syntax:
		<integer> $system.htoni(<value:integer>)
		<integer> $system.htoni(<value:integer>,<bytecount:integer>)
	@description:
		Converts the integer value in host byte order
		to it's network byte order counterpart.
		Since the conversion is carried at binary representation
		level you must specify the number of bytes you want
		the binary representation to have. Valid values are
		1 (no conversion), 2 (short), 4 (32 bit int) and 8 (64 bit int).
		If omitted, bytecount is assumed to be 4.
*/

static bool system_kvs_fnc_htoni(KviKvsModuleFunctionCall * c)
{
	kvs_int_t iValue;
	kvs_uint_t uBytes;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("value", KVS_PT_INT, 0, iValue)
	KVSM_PARAMETER("bytecount", KVS_PT_UINT, KVS_PF_OPTIONAL, uBytes)
	KVSM_PARAMETERS_END(c)

	switch(uBytes)
	{
		case 0:
			c->returnValue()->setInteger((kvs_int_t)KviByteOrder::localCpuToNetworkByteOrder32((kvi_u32_t)iValue));
			break;
		case 1:
			c->returnValue()->setInteger((kvs_int_t)((kvi_u8_t)iValue));
			break;
		case 2:
			c->returnValue()->setInteger((kvs_int_t)KviByteOrder::localCpuToNetworkByteOrder16((kvi_u16_t)iValue));
			break;
		case 4:
			c->returnValue()->setInteger((kvs_int_t)KviByteOrder::localCpuToNetworkByteOrder32((kvi_u32_t)iValue));
			break;
		case 8:
			c->returnValue()->setInteger((kvs_int_t)KviByteOrder::localCpuToNetworkByteOrder64((kvi_u64_t)iValue));
			break;
		default:
			return c->error(__tr2qs("Bad bytecount specification: 1, 2, 4 and 8 are allowed"));
			break;
	}

	return true;
}

static bool system_module_init(KviModule * m)
{
	KVSM_REGISTER_FUNCTION(m, "ostype", system_kvs_fnc_ostype);
	KVSM_REGISTER_FUNCTION(m, "osname", system_kvs_fnc_osname);
	KVSM_REGISTER_FUNCTION(m, "osversion", system_kvs_fnc_osversion);
	KVSM_REGISTER_FUNCTION(m, "osrelease", system_kvs_fnc_osrelease);
	KVSM_REGISTER_FUNCTION(m, "osmachine", system_kvs_fnc_osmachine);
	KVSM_REGISTER_FUNCTION(m, "osnodename", system_kvs_fnc_osnodename);
	KVSM_REGISTER_FUNCTION(m, "getenv", system_kvs_fnc_getenv);
	KVSM_REGISTER_FUNCTION(m, "hostname", system_kvs_fnc_hostname);
	KVSM_REGISTER_FUNCTION(m, "dbus", system_kvs_fnc_dbus);
	KVSM_REGISTER_FUNCTION(m, "htoni", system_kvs_fnc_htoni);
	KVSM_REGISTER_FUNCTION(m, "ntohi", system_kvs_fnc_ntohi);
	KVSM_REGISTER_FUNCTION(m, "clipboard", system_kvs_fnc_clipboard);
	KVSM_REGISTER_FUNCTION(m, "selection", system_kvs_fnc_selection);
	KVSM_REGISTER_FUNCTION(m, "checkModule", system_kvs_fnc_checkModule);
	KVSM_REGISTER_FUNCTION(m, "call", system_kvs_fnc_plugin_call);

	KVSM_REGISTER_SIMPLE_COMMAND(m, "setenv", system_kvs_cmd_setenv);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setClipboard", system_kvs_cmd_setClipboard);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setSelection", system_kvs_cmd_setSelection);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "runcmd", system_kvs_cmd_runcmd);

	g_pPluginManager = new(PluginManager);

	return true;
}

static bool system_module_cleanup(KviModule *)
{
	g_pPluginManager->unloadAll();
	delete g_pPluginManager;
	return true;
}

static bool system_module_can_unload(KviModule *)
{
	if(!g_pPluginManager->checkUnload())
		return false;
	return true;
}

KVIRC_MODULE(
    "System", // module name
    "4.0.0",  // module version
    "Copyright	(C) 2001 Szymon Stefanek (pragma at kvirc dot net)"
    "		(C) 2005 Tonino Imbesi (grifisx at barmes dot org)"
    "		(C) 2005 Alessandro Carbone (elfonol at gmail dot com)", // author & (C)
    "System information module",
    system_module_init,
    system_module_can_unload,
    0,
    system_module_cleanup,
    0)
