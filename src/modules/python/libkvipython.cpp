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

	static KviModule * g_pPythonCoreModule = 0;

#else // COMPILE_PYTHON_SUPPORT

	#define KVS_CHECK_PYTHONCORE(_m,_c)

	#define KVS_CHECK_MODULE_STATE(_m,_c) \
		if(!_c->switches()->find('q',"quiet")) \
			_c->warning(__tr2qs_ctx("This KVIrc executable has been compiled without python scripting support","python")); \
		return true;

	static KviModule * g_pPythonCoreModule = 0;
#endif // COMPILE_PYTHON_SUPPORT

static bool python_kvs_cmd_begin(KviKvsModuleCommandCall * c)
{
	// This command is somewhat special in the fact that has a dedicated
	// parsing routine in the KVS core parser.
	// The parser sets the python code as the first parameter of our call,
	// the remaining params are the arguments

	QString szCode;
	KviKvsVariantList vList;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("code",KVS_PT_STRING,0,szCode)
		KVSM_PARAMETER("args",KVS_PT_VARIANTLIST,KVS_PF_OPTIONAL,vList)
	KVSM_PARAMETERS_END(c)

	KVS_CHECK_MODULE_STATE(m,c)

#ifdef COMPILE_PYTHON_SUPPORT
	debug("Python module begin");

	// FIXME Test: this prints the sentence on the stdout, like Qt's debug()
	if(PyRun_SimpleString("print 'Test string'\n") < 0)
		c->warning("Non funza");

#endif // COMPILE_PYTHON_SUPPORT

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
/*
#ifdef COMPILE_PERL_SUPPORT
	KviPerlCoreCtrlCommand_destroy ex;
	ex.uSize = sizeof(KviPerlCoreCtrlCommand_destroy);
	ex.szContext = szContext;
	
	if(!g_pPerlCoreModule->ctrl(KVI_PERLCORECTRLCOMMAND_DESTROY,&ex))
	{
		if(!c->switches()->find('q',"quiet"))
			c->warning(__tr2qs_ctx("The perlcore module failed to execute the code: something is wrong with the perl support","perl"));
	}
#endif //COMPILE_PERL_SUPPORT
*/
#endif // COMPILE_PYTHON_SUPPORT

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

static bool python_module_cleanup(KviModule * m)
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
	python_module_cleanup
)
