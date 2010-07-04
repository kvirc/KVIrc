//=============================================================================
//
//   File : libkvipython.cpp
//   Creation date : Fri Nov 07 00:18:31 2008 GMT by Elvio Basello
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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
#include "kvi_modulemanager.h"
//#include "kvi_fileutils.h"
#include "kvi_locale.h"
//#include "kvi_app.h"
//#include "kvi_options.h"
//#include "kvi_out.h"

#ifdef COMPILE_PYTHON_SUPPORT
	#include "../pythoncore/pythoncoreinterface.h"
	#include <Python.h>

	static KviModule * g_pPythonCoreModule = 0;
	
	#define KVS_CHECK_PYTHONCORE(_m,_c) \
		g_pPythonCoreModule = g_pModuleManager->getModule("pythoncore"); \
		if(!g_pPythonCoreModule) \
		{ \
			if(!_c->switches()->find('q',"quiet")) \
			{ \
				_c->warning(__tr2qs_ctx("The pythoncore module can't be loaded: python support not available","python")); \
				_c->warning(__tr2qs_ctx("To see more details about loading failure try /pythoncore.load","python")); \
				return true; \
			} \
		}

	#define KVS_CHECK_MODULE_STATE(_m,_c) KVS_CHECK_PYTHONCORE(_m,_c)

#else // COMPILE_PYTHON_SUPPORT

	#define KVS_CHECK_PYTHONCORE(_m,_c)

	#define KVS_CHECK_MODULE_STATE(_m,_c) \
		if(!_c->switches()->find('q',"quiet")) \
			_c->warning(__tr2qs_ctx("This KVIrc executable has been compiled without python scripting support","python")); \
		return true;

	static KviModule * g_pPythonCoreModule = 0;
#endif // COMPILE_PYTHON_SUPPORT


/*
	@doc: python.begin
	@type:
		command
	@title:
		python.begin
	@keyterms:
		Including python code snippets in KVS
	@short:
		Starts a python code snippet
	@syntax:
		python.begin [-n] [-q] [(<python_context>[,<arg0>[,<arg1>[...]]])]
		<python code>
		python.end
	@switches:
		!sw: -q | --quiet
		Prevents the command from printing any warnings.
		!sw: -n | --no-return
		Prevents the python script return value to be propagated
		to the current context.
		!sw: -f | --fail-on-error
		Treat python errors as KVS errors and abort execution of the
		current script. Incompatible with -q
	@description:
		Indicates the beginning of a snippet of python code.
		The whole code part between python.begin and python.end
		is executed in a python interpreter.
		If python.end is omitted then it is implicitly assumed
		that the code from python.begin to the end of the command
		buffer is python.[br]
		Each python code execution is bound to a
		python context (that is in fact a particular instance
		of a python interpreter). If <python_context> is not specified
		or it is an empty string then temporary python interpreter is created
		and destroyed just after the code snippet has terminated execution.
		If <python_context> is specified then a python interpreter
		keyed to that context is used: if it was already existing
		then it is reused otherwise it is created.
		Any <python_context> is persistent: it mantains the function
		declarations and python variable states until explicitly
		destroyed with [cmd]python.destroy[/cmd] (or the pythoncore
		module is forcibly unloaded).[br]
		The <arg0>,<arg1>,... arguments, if present, are passed
		to the python code snippet in the aArgs list (accessible as aArgs[0],aArgs[1]...).[br]
		The return value of the python code is propagated to the current
		context (just like [cmd]setreturn[/cmd] was called on it) unless
		the -n switch is used.[br]
		The -q switch prevents from the command from printing any
		warning.[br]
		See the [doc:python_and_kvs]python scripting documentation[/doc]
		for more information.
	@examples:
		[example]
			python.begin
			KVIrc::eval("echo \"Hello World from python!\"");
			python.end
		[/example]
	@seealso:
*/

/*
	@doc: python.end
	@type:
		command
	@title:
		python.end
	@short:
		Ends a python code snippet
	@syntax:
		python.begin[(<python_context>)]
		<python code>
		python.end
	@description:
		Ends a python code snippet. See [cmd]python.begin[/cmd].
	@seealso:
		[cmd]python.begin[/cmd]
*/

static bool python_kvs_cmd_begin(KviKvsModuleCommandCall * c)
{
	// This command is somewhat special in the fact that has a dedicated
	// parsing routine in the KVS core parser.
	// The parser sets the python code as the first parameter of our call,
	// the remaining params are the context name and the arguments

	QString szCode,szContext;
	KviKvsVariantList vList;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("code",KVS_PT_STRING,0,szCode)
		KVSM_PARAMETER("context",KVS_PT_STRING,KVS_PF_OPTIONAL,szContext)
		KVSM_PARAMETER("args",KVS_PT_VARIANTLIST,KVS_PF_OPTIONAL,vList)
	KVSM_PARAMETERS_END(c)

	KVS_CHECK_MODULE_STATE(m,c)

#ifdef COMPILE_PYTHON_SUPPORT
	KviPythonCoreCtrlCommand_execute ex;
	ex.uSize = sizeof(KviPythonCoreCtrlCommand_execute);
	ex.pKvsContext = c->context();
	ex.szContext = szContext;
	ex.szCode = szCode;
	for(KviKvsVariant * v = vList.first();v;v = vList.next())
	{
		QString tmp;
		v->asString(tmp);
		ex.lArgs.append(tmp);
	}
	ex.bQuiet = c->switches()->find('q',"quiet");

	if(!g_pPythonCoreModule->ctrl(KVI_PYTHONCORECTRLCOMMAND_EXECUTE,&ex))
	{
		if(!c->switches()->find('q',"quiet"))
			c->warning(__tr2qs_ctx("The pythoncore module failed to execute the code: something is wrong with the python support","python"));
		return true;
	}

	if(!ex.lWarnings.isEmpty())
	{
		for(QStringList::Iterator it = ex.lWarnings.begin();it != ex.lWarnings.end();++it)
			c->warning(*it);
	}

	if(!ex.bExitOk)
	{
		if(!c->switches()->find('q',"quiet"))
		{

			if(c->switches()->find('f',"fail-on-error"))
			{
				c->warning(__tr2qs_ctx("Python execution error:","python"));
				c->warning(ex.szError);
				return false;
			} else {
				c->warning(__tr2qs_ctx("Python execution error:","python"));
				c->error(ex.szError);
			}
		}
	}

	if(!c->switches()->find('n',"no-return"))
		c->context()->returnValue()->setString(ex.szRetVal);

#endif //COMPILE_PYTHON_SUPPORT


	return true;
}

static bool python_kvs_cmd_destroy(KviKvsModuleCommandCall * c)
{
	QString szContext;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("context",KVS_PT_NONEMPTYSTRING,0,szContext)
	KVSM_PARAMETERS_END(c)

	KVS_CHECK_MODULE_STATE(m,c)

#ifdef COMPILE_PYTHON_SUPPORT
	KviPythonCoreCtrlCommand_destroy ex;
	ex.uSize = sizeof(KviPythonCoreCtrlCommand_destroy);
	ex.szContext = szContext;

	if(!g_pPythonCoreModule->ctrl(KVI_PYTHONCORECTRLCOMMAND_DESTROY,&ex))
	{
		if(!c->switches()->find('q',"quiet"))
			c->warning(__tr2qs_ctx("The pythoncore module failed to execute the code: something is wrong with the python support","python"));
	}
#endif //COMPILE_PYTHON_SUPPORT

	return true;
}

/*
	@doc: python.isAvailable
	@type:
		function
	@title:
		$python.isAvailable
	@short:
		Check if python scripting support is available
	@syntax:
		$python.isAvailable
	@description:
		Returns 1 if the python scripting support is available and 0 otherwise.
*/
static bool python_kvs_fnc_isAvailable(KviKvsModuleFunctionCall * c)
{
#ifdef COMPILE_PYTHON_SUPPORT
	g_pPythonCoreModule = g_pModuleManager->getModule("pythoncore");
	c->returnValue()->setBoolean(g_pPythonCoreModule ? true : false);
#else
	c->returnValue()->setBoolean(false);
#endif
	return true;
}

static bool python_module_init(KviModule * m)
{
	// register the command anyway
	KVSM_REGISTER_SIMPLE_COMMAND(m,"begin",python_kvs_cmd_begin);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"destroy",python_kvs_cmd_destroy);

	KVSM_REGISTER_FUNCTION(m,"isAvailable",python_kvs_fnc_isAvailable);
#ifdef COMPILE_PYTHON_SUPPORT
	g_pPythonCoreModule = g_pModuleManager->getModule("pythoncore");
#endif
	return true;
}

static bool python_module_cleanup(KviModule *)
{
	return true;
}

KVIRC_MODULE(
	"Python",                                                 // module name
	"4.0.0",                                                // module version
	"Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org", // author & (C)
	"Python Scripting Engine",
	python_module_init,
	0,
	0,
	python_module_cleanup,
	"python"
)
