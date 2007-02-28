//=============================================================================
//
//   File : libkvisystem.cpp
//   Creation date : Fri Nov 16 03:50:12 2001 GMT by Szymon Stefanek
//
//   This system is part of the KVirc irc client distribution
//   Copyright (C) 2001-2005 Szymon Stefanek (pragma at kvirc dot net)
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
//=============================================================================

#include "kvi_settings.h"
#include "kvi_module.h"
#include "kvi_string.h"

#include "kvi_locale.h"
#include "kvi_qcstring.h"
#include "kvi_app.h"
#include "kvi_env.h"
#include "kvi_osinfo.h"

#include <qclipboard.h>

#ifndef COMPILE_ON_WINDOWS
	#include <sys/utsname.h>
	#include <stdlib.h>
	#include <unistd.h>
#endif

#ifdef COMPILE_KDE_SUPPORT
	#include <dcopclient.h>
#endif

#include "kvi_modulemanager.h"

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
		Returns the current type of operating system: unix,macosx or windows.[br]
*/

static bool system_kvs_fnc_ostype(KviKvsModuleFunctionCall * c)
{
	c->returnValue()->setString(KviOsInfo::type());
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

static bool system_kvs_fnc_osname(KviKvsModuleFunctionCall *c)
{
	c->returnValue()->setString(KviOsInfo::name());
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

static bool system_kvs_fnc_osversion(KviKvsModuleFunctionCall *c)
{
	// no params to process
	c->returnValue()->setString(KviOsInfo::version());
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

static bool system_kvs_fnc_osrelease(KviKvsModuleFunctionCall *c)
{
	// no params to process
	c->returnValue()->setString(KviOsInfo::release());
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

static bool system_kvs_fnc_osmachine(KviKvsModuleFunctionCall *c)
{
	// no params to process
	c->returnValue()->setString(KviOsInfo::machine());
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

static bool system_kvs_fnc_osnodename(KviKvsModuleFunctionCall *c)
{
	// no params to process
	c->returnValue()->setString(KviOsInfo::nodename());
	return true;
}

/*
	@doc: system.getenv
	@keyterms:
		Enviroinement variables
	@type:
		function
	@title:
		$system.getenv
	@short:
		Returns the value of an enviroinement variable
	@syntax:
		<string> $system.getenv(<variable:string>)
	@description:
		Returns the value of the enviroinement <variable>.[br]
*/

static bool system_kvs_fnc_getenv(KviKvsModuleFunctionCall *c)
{
	QString szVariable;

	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("variable",KVS_PT_NONEMPTYSTRING,0,szVariable)
	KVSM_PARAMETERS_END(c)

	KviQCString szVar = szVariable.local8Bit();
#ifdef COMPILE_ON_WINDOWS
	QString env = getenv(szVar.data());
	QString def = __tr2qs("No environment variable found, please don't use the %% in the request");
	c->returnValue()->setString(env.isEmpty() ? QString::fromLocal8Bit(env) : QString::fromLocal8Bit(def));
#else
	char * b = kvi_getenv(szVar.data());
	c->returnValue()->setString(b ? QString::fromLocal8Bit(b) : QString::null);
#endif
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
		of "selection" (most notably X11) which is NOT the same as the clipboard.
		See [fnc]$system.selection[/fnc]().
	@seealso:
		[fnc]$system.selection[/fnc](),
		[cmd]system.setClipboard[/cmd],
		[cmd]system.setSelection[/cmd]
*/

static bool system_kvs_fnc_clipboard(KviKvsModuleFunctionCall *c)
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
		of "selection" (most notably X11) which is NOT the same as the clipboard.
		See [fnc]$system.selection[/fnc]().
	@seealso:
		[fnc]$system.selection[/fnc](),
		[fnc]$system.clipboard[/fnc](),
		[cmd]system.setSelection[/cmd]
*/

static bool system_kvs_cmd_setClipboard(KviKvsModuleCommandCall * c)
{
	QString szValue;

	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("data",KVS_PT_STRING,KVS_PF_OPTIONAL,szValue)
	KVSM_PARAMETERS_END(c)
	g_pApp->clipboard()->setText(szValue,QClipboard::Clipboard);
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
		of "selection" (most notably X11) which is NOT the same as the clipboard.
	@seealso:
		[fnc]$system.selection[/fnc](),
		[fnc]$system.clipboard[/fnc](),
		[cmd]system.setClipboard[/cmd]
*/

static bool system_kvs_cmd_setSelection(KviKvsModuleCommandCall * c)
{
	QString szValue;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("data",KVS_PT_STRING,KVS_PF_OPTIONAL,szValue)
	KVSM_PARAMETERS_END(c)
	g_pApp->clipboard()->setText(szValue,QClipboard::Selection);
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
		of "selection" (most notably X11) which is NOT the same as clipboard.
		On other systems this function will always return an empty string.
		See [fnc]$system.clipboard[/fnc]().
	@seealso:
		[fnc]$system.clipboard[/fnc](),
		[cmd]system.setClipboard[/cmd],
		[cmd]system.setSelection[/cmd]
*/

static bool system_kvs_fnc_selection(KviKvsModuleFunctionCall *c)
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
		$true if succesfull and $false otherwise. This can
		be effectively used to test if a KVIrc exension module
		is present on the system and can be loaded by the
		KVIrc engine.
*/

static bool system_kvs_fnc_checkModule(KviKvsModuleFunctionCall *c)
{
	QString szModuleName;

	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("module_name",KVS_PT_STRING,0,szModuleName)
	KVSM_PARAMETERS_END(c)

	c->returnValue()->setBoolean(g_pModuleManager->loadModule(szModuleName.utf8().data()));
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

static bool system_kvs_fnc_hostname(KviKvsModuleFunctionCall *c)
{
	// no params to process
	c->returnValue()->setString(KviOsInfo::hostname());
	return true;
}


/*
	@doc: system.dcop
	@keyterms:
		System information
	@type:
		function
	@title:
		$system.dcop
	@short:
		Performs a DCOP call
	@syntax:
		<variant> $system.dcop(<application:string>,<objectid:string>,<function:string>[,<parameter1:string>[,<parameter2:string>[,...]]])
	@description:
		This function allows performing simple DCOP calls without executing
		an external process. This feature is available ONLY when KDE support
		is compiled in the executable: this means that this function is absolutely
		non portable (don't use it in scripts that you're going to distribute).
		<application> is the name of the application being called, <objectid> is the
		identifier of the object called, <function> is the function to be executed
		on the remote object and <parameter1>,<parameter2>,... is the list of
		parameters to be passed. The <function> name must contain the
		trailing parenthesis and parameter specification (see examples).
		The parameters MUST be in the form "type=value"
		where "type" is the C++ type of the parameter and value
		is the string rappresentation of the parameter data. Currently
		KVIrc supports only QString,QCString,bool,int and uint data types.[br]
		The returned value is the string rappresentation of the returned
		data if the return type is known, otherwise it is the name of the data type returned.
		[br]
		If the application name is prefixed with "?" then the call is performed in "remote test"
		mode: no "remote" errors are printed and the function returns 1 if the call executed
		succesfully and 0 if the call failed. This can be used with the very first
		call to programmaticaly test if the remote application is running.
	@examples:
		[example]
			echo $system.dcop("kdesktop","KBackgroundIface","currentWallpaper(int)","int=0")
			echo $system.dcop("kdesktop","KScreensaverIface","lock()")
			# we can also ignore the return value in several ways
			%dummy = $system.dcop("kicker","kicker","showKMenu()")
			$system.dcop("kdesktop","KScreensaverIface","save()")
			$system.dcop("kicker","Panel","addBrowserButton(QString)","QString=/")
			# runtime test if a call would work (i.e. , kicker is running at all, parameters are right etc...)
			if($system.dcop("?kicker","kicker","showKMenu()"))echo "Can't make dcop calls to kicker!"
		[/example]
*/

static bool system_kvs_fnc_dcop(KviKvsModuleFunctionCall *c)
{
	bool bTestMode = false;

	KviQCString szApp,szObj,szFun;
	QStringList parms;
	
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("application",KVS_PT_NONEMPTYCSTRING,0,szApp)
		KVSM_PARAMETER("objectid",KVS_PT_NONEMPTYCSTRING,0,szObj)
		KVSM_PARAMETER("function",KVS_PT_NONEMPTYCSTRING,0,szFun)
		KVSM_PARAMETER("parameter_list",KVS_PT_STRINGLIST,KVS_PF_OPTIONAL,parms)
	KVSM_PARAMETERS_END(c)

	if((szApp.data()) && (szApp.length() > 1))
	{
		if(*(szApp.data()) == '?')
		{
			bTestMode = true;
			szApp.remove(0,1);
		}
	}

#ifdef COMPILE_KDE_SUPPORT

	QByteArray ba;
	QDataStream ds(ba, IO_WriteOnly);

	for ( QStringList::Iterator it = parms.begin(); it != parms.end(); ++it )
	{
		KviStr tmp = *it;
	
		if(tmp.isEmpty())
		{
			c->warning(__tr2qs("Invalid DCOP parameter syntax"));
			return false;
		}

		KviStr szType = tmp.leftToFirst('=',false);
		tmp.cutToFirst('=',true);
		if(szType.isEmpty())szType = "int";
		bool bOk;
		if(kvi_strEqualCI("int",szType.ptr()) || kvi_strEqualCI("long",szType.ptr()))
		{
			int iii = tmp.toInt(&bOk);
			if(!bOk)
			{
				c->warning(__tr2qs("The specified parameter is not an integer"));
				return false;
			}
			ds << iii;
		} else if(kvi_strEqualCI("QString",szType.ptr()))
		{
			QString ddd = tmp.ptr();
			ds << ddd;
		} else if(kvi_strEqualCI("QCString",szType.ptr()))
		{
			QCString qcs = tmp.ptr();
			ds << qcs;
		} else if(kvi_strEqualCI("bool",szType.ptr()))
		{
			bool bbb = kvi_strEqualCI(tmp.ptr(),"true");
			ds << bbb;
		} else if(kvi_strEqualCI("unsigned int",szType.ptr()) || kvi_strEqualCI("uint",szType.ptr()) || kvi_strEqualCI("Q_UINT32",szType.ptr()))
		{
			unsigned int uii = tmp.toUInt(&bOk);
			if(!bOk)
			{
				c->warning(__tr2qs("The specified parameter is not an integer"));
				return false;
			}
			ds << uii;
		} else {
			c->warning(__tr2qs("Unsupported DCOP parameter type %s"),tmp.ptr());
			return false;
		}
	}

	QByteArray rba;
	QCString szRetType;

	bool bRet = g_pApp->dcopClient()->call(szApp,szObj,szFun,ba,szRetType,rba);

	if(!bRet)
	{
		if(!bTestMode)
			c->warning(__tr2qs("DCOP call failed"));
		c->returnValue()->setInteger(0);
	} else {
		if(bTestMode)
			c->returnValue()->setInteger(1);
		else {
			QDataStream ret(rba, IO_ReadOnly);
			if(szRetType == "bool")
			{
				bool bqw;
				ret >> bqw;
				c->returnValue()->setInteger(bqw ? 1 : 0);
			} else if(szRetType == "QString")
			{
				QString szz;
				ret >> szz;
				c->returnValue()->setString(szz);
			} else if(szRetType == "QCString")
			{
				QCString sss;
				ret >> sss;
				c->returnValue()->setString(sss.data());
			} else if((szRetType == "uint") || (szRetType == "unsigned int") || (szRetType == "Q_UINT32"))
			{
				unsigned int ui3;
				ret >> ui3;
				c->returnValue()->setInteger(ui3);
			} else if((szRetType == "int") || (szRetType == "long"))
			{
				int iii;
				ret >> iii;
				c->returnValue()->setInteger(iii);
			} else if(szRetType == "QCStringList")
			{
				QCStringList csl;
				ret >> csl;
				KviKvsArray * arry = new KviKvsArray();
				int idx = 0;
				for(QCStringList::Iterator iter = csl.begin();iter != csl.end();++iter)
				{
					arry->set(idx,new KviKvsVariant(QString(*iter)));
					idx++;
				}
				c->returnValue()->setArray(arry);
			} else if(szRetType == "QStringList")
			{
				QStringList csl;
				ret >> csl;
				KviKvsArray * arry = new KviKvsArray();
				int idx = 0;
				for(QStringList::Iterator iter = csl.begin();iter != csl.end();++iter)
				{
					arry->set(idx,new KviKvsVariant(*iter));
					idx++;
				}
				c->returnValue()->setArray(arry);
			} else {
				c->returnValue()->setString(szRetType.data());
			}
		}
	}
#else
	if(!bTestMode)
		c->warning(__tr2qs("DCOP calls are available only when KDE support is compiled in"));
	c->returnValue()->setInteger(0);
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
		Enviroinement variables
	@short:
		Sets an enviroinement variable
	@syntax:
		system.setenv <variable:string> [<value:string>]
	@description:
		Sets the enviroinement <variable> to the <value> string.[br]
		If <value> is not given , the <variable> is unset.[br]
	@seealso:
		[fnc]$system.getenv[/fnc]()
*/


static bool system_kvs_cmd_setenv(KviKvsModuleCommandCall * c)
{
	QString szVariable,szValue;

	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("variable",KVS_PT_NONEMPTYSTRING,0,szVariable)
		KVSM_PARAMETER("value",KVS_PT_STRING,KVS_PF_OPTIONAL,szValue)
	KVSM_PARAMETERS_END(c)

	KviQCString szVar = szVariable.local8Bit();
	KviQCString szVal = szValue.local8Bit();

	if(szVal.isEmpty())kvi_unsetenv(szVar.data());
	else
	{
/*#ifdef COMPILE_ON_WINDOWS
		QString Var,Val,VarAndVal;
		Val			=	szVar.data();
		Var			=	szVal.data();
		VarAndVal	=	Var+"="+Val;
		putenv(VarAndVal);
#else*/ // <-- this stuff is implicit in kvi_setenv: that's why we have the kvi_ version.
		kvi_setenv(szVar.data(),szVal.data());
/*#endif*/
	}
	return true;
}


static bool system_module_init(KviModule * m)
{
	KVSM_REGISTER_FUNCTION(m,"ostype",system_kvs_fnc_ostype);
	KVSM_REGISTER_FUNCTION(m,"osname",system_kvs_fnc_osname);
	KVSM_REGISTER_FUNCTION(m,"osversion",system_kvs_fnc_osversion);
	KVSM_REGISTER_FUNCTION(m,"osrelease",system_kvs_fnc_osrelease);
	KVSM_REGISTER_FUNCTION(m,"osmachine",system_kvs_fnc_osmachine);
	KVSM_REGISTER_FUNCTION(m,"osnodename",system_kvs_fnc_osnodename);
	KVSM_REGISTER_FUNCTION(m,"getenv",system_kvs_fnc_getenv);
	KVSM_REGISTER_FUNCTION(m,"hostname",system_kvs_fnc_hostname);
	KVSM_REGISTER_FUNCTION(m,"dcop",system_kvs_fnc_dcop);
	KVSM_REGISTER_FUNCTION(m,"clipboard",system_kvs_fnc_clipboard);
	KVSM_REGISTER_FUNCTION(m,"selection",system_kvs_fnc_selection);
	KVSM_REGISTER_FUNCTION(m,"checkModule",system_kvs_fnc_checkModule);

	KVSM_REGISTER_SIMPLE_COMMAND(m,"setenv",system_kvs_cmd_setenv);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"setClipboard",system_kvs_cmd_setClipboard);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"setSelection",system_kvs_cmd_setSelection);

	return true;
}

static bool system_module_cleanup(KviModule *m)
{
	return true;
}

KVIRC_MODULE(
	"System",                                                 // module name
	"1.0.0",                                                // module version
	"Copyright	(C) 2001 Szymon Stefanek (pragma at kvirc dot net)" \
	"			(C) 2005 Tonino Imbesi (grifisx at barmes dot org)"\
	"			(C) 2005 Alessandro Carbone (noldor at barmes dot org)",// author & (C)
	"System informations module",
	system_module_init,
	0,
	0,
	system_module_cleanup
)
