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
	@doc: python_and_kvs
	@type:
		language
	@title:
		Using python from KVS and vice-versa.
	@short:
		How to use python from KVS and KVS from python.
	@body:
		[big]Introduction[/big][br]
		Starting from version 4.0.0 you can include python code snippets
		in KVS code and you can use KVS commands from within python.
		This feature is present only if a working python installation
		has been found at build time.[br]
		The python support is very similar to the perl support present
		since 3.x. So if you have used perl from kvirc before you'll
		find the api is almost the same.. otherwise read on :)
		[br]

		[big]Using python from KVS[/big][br]
		Using python from KVIrc is really easy: just enclose
		your python code snippet inside [cmd]python.begin[/cmd] and [cmd]python.end[/cmd].
		[example]
		[cmd]python.begin[/cmd]
		&lt;python code goes here&gt;
		[cmd]python.end[/cmd]
		[/example]
		For example:[br]
		[example]
		[cmd]python.begin[/cmd]
		f = open('myfile.txt', 'w')
		f.write('This is a test\n')
		f.close()
		[cmd]python.end[/cmd]
		[/example]
		A python code snippet can appear anywhere a KVS code snippet can
		with the only restriction that i must be enclosed in [cmd]python.begin[/cmd]
		and [cmd]python.end[/cmd]. This means that you can write python code
		in the commandline, in the aliases, the event handlers, popups...anywhere.[br]
		If you have already encountered the KVIrc's [cmd]eval[/cmd] command
		that you probably also know how to execute a python code snippet from a file :)[br]
		[br]

		[big]Using KVS from python[/big][br]
		KVIrc exports several commands to the python namespace
		that allow you to invoke KVIrc's functions from inside the python code snippet.[br]
		The nicest example is kvirc.echo():
		[example]
		[cmd]python.begin[/cmd]
		kvirc.echo("Hello KVIrc world from python!")
		[cmd]python.end[/cmd]
		[/example]
		kvirc.echo() is the counterpart of the [cmd]echo[/cmd].
		The exact syntax is:[br]
		&nbsp; &nbsp; [b]kvirc.echo(&lt;text&gt;[,&lt;colorset&gt;[,&lt;windowid&gt;]])[/b][br]
		&lt;text&gt; is obviously the text to be printed. &lt;colorset&gt; is
		the equivalent of the [cmd]echo[/cmd] -i option and &lt;windowid&gt;
		is the equivalent of the -w option. Both &lt;colorset&gt; and &lt;windowid&gt;
		can be omitted (in this case KVIrc will use a default colorset and the current window).[br]
		[br]

		[big]Python execution contexts[/big][br]
		The python code snippets are executed by the means of a python interpreter.
		Each python interpreter has its own context and thus it's own variables,
		own function namespace etc.[br]
		[br]
		In the example above KVIrc creates an interpreter when [cmd]python.begin[/cmd]
		is invoked and destroys it at [cmd]python.end[/cmd] parsing time.
		In fact, KVIrc can mantain multiple persistent interpreters that will
		allow you to preserve your context across [cmd]python.begin[/cmd] invocations.[br]
		You can invoke a specific python context by passing it as parameter to the [cmd]python.begin[/cmd]
		command.[br]
		[example]
		[cmd]python.begin("mycontext")[/cmd]
		myvariable = "mycontext"
		kvirc.echo("This python code is executed from " + myvariable)
		[cmd]python.end[/cmd]
		[/example]
		The nice thing is that at a later time you can invoke this context again
		and discover that mycontext has preserved its value:[br]
		[example]
		[cmd]python.begin("mycontext")[/cmd]
		kvirc.echo("myvariable is still equal to " + myvariable)
		[cmd]python.end[/cmd]
		[/example]
		The first time you invoke a named python context it gets automatically created and
		it persists until KVIrc terminates or the python context is explicitly destroyed
		by the means of [cmd]python.destroy[/cmd].[br]
		[br]
		In fact there is a third possibility to destroy a context: it's when the
		pythoncore module is forcibly unloaded (by the means of /pythoncore.unload) but
		this is really a rare case and should be threated just like a KVIrc restart (the
		user probably WANTS the contexts to be reinitialized).[br]
		[br]
		The nice thing is that not only your variables will get preserved
		but also any python function or class you declare in a context will persist.
		It's just like executing a long python script file with pauses inside.[br]
		[br]
		If you omit the python context name in the [cmd]python.begin[/cmd] command
		(or if you use an empty string in it's place)
		then KVIrc will create a temporary context for the snippet execution
		and will destroy it immediately after [cmd]python.end[/cmd] has been called.[br]
		[br]
		The major side effect of keeping persistent python contexts is that
		the python's symbol table will grow and if not used carefully the interpreter
		may become a memory hog. So if you're going to use persistent contexts
		either try to keep the symbol table clean or explicitly call [cmd]python.destroy[/cmd]
		once in a while to recreate the interpreter.[br]
		If you just execute occasional python code snippets and don't need to keep persistent variables
		then just use the nameless temporary context provided by [cmd]python.begin[/cmd]("").[br]
		[br]

		[big]Passing parameters to the python script[/big][br]
		The easiest way to pass parameters to the python code snippet
		is to put them as [cmd]python.begin[/cmd] arguments.
		In fact the complete syntax of [cmd]python.begin[/cmd] is:[br]
		[b]python.begin(&lt;python context&gt;,&lt;arg0&gt;,&lt;arg1&gt;,...)[/b][br]
		Where the &lt;arg0&gt;,&lt;arg1&gt;...&lt;argN&gt; parameters
		are passed to the python context as elements of the aArgs array.[br]
		[example]
		[cmd]python.begin[/cmd]("","Hello world!","Now I CAN",1,2,3)
		for(i=0;i<5;i++)
			kvirc.echo(aArgs[i],40)
		[cmd]python.end[/cmd]
		[/example]
		[br]

		[big]Accessing the KVIrc scripting context from python[/big][br]
		KVIrc exposes the following functions that manipulate the
		variables of the KVIrc's current KVS execution context.[br]
		&nbsp; &nbsp; [b]kvirc.getLocal(&lt;x&gt;)[/b][br]
		Returns the value of the KVIrc's local variable %x.[br]
		&nbsp; &nbsp; [b]kvirc.getGlobal(&lt;Y&gt;)[/b][br]
		Returns the value of the KVIrc's global variable %Y.[br]
		&nbsp; &nbsp; [b]kvirc.setLocal(&lt;x&gt;,&lt;value&gt;)[/b][br]
		Sets the KVIrc's global variable %x to &lt;value&gt;[br]
		&nbsp; &nbsp; [b]kvirc.setGlobal(&lt;Y&gt;,&lt;value&gt;)[/b][br]
		Sets the KVIrc's global variable %Y to &lt;value&gt;[br]
		The local variables interested belong to the current KVS exection context
		while the global variables are visible everywhere.[br]
		[example]
		%pippo = test
		%Pluto = 12345
		[cmd]python.begin[/cmd]
		mypippo = kvirc.getLocal("pippo")
		mypippo += " rox"
		mypluto = kvirc.getGlobal("Pluto")
		mypluto += " rox"
		kvirc.setLocal("pippo",mypluto)
		kvirc.setGlobal("Pluto",mypippo)
		[cmd]python.end[/cmd]
		[cmd]echo[/cmd] "\%pippo is" %pippo
		[cmd]echo[/cmd] "\%Pluto is" %Pluto
		[/example]
		[br]

		[big]Executing arbitrary KVIrc commands from python[/big][br]
		You can execute arbitrary KVS commands from python by the means of:[br]
		&nbsp; &nbsp; [b]kvirc.eval(&lt;code&gt;)[/b][br]
		This function behaves exactly like the ${ &lt;code&gt; } KVS construct:
		it executes &lt;code&gt; in a child context and returns it's evaluation retult.[br]
		The following two code snippets have equivalent visible effects:[br]
		[example]
		[cmd]echo[/cmd] ${ return "Yeah!"; }
		[/example]
		[example]
		[cmd]python.begin[/cmd]
		kvirc.echo(kvirc.eval("return \"Yeah!\""));
		[cmd]python.end[/cmd]
		[/example]
		You can "eval" composite command sequences and variable ones.[br]
		Remember that the python code snippet is evaluated in a child KVS context
		and thus the local variables are NOT visible!.
		The following code snippets may easily fool you:[br]
		[example]
		%x = 10
		[cmd]python.begin[/cmd]
		kvirc.eval("echo \"The value is %x\"")
		[cmd]python.end[/cmd]
		[/example]
		This will print "The value is " since %x is not accessible from the eval's context.
		If you have tried to write something like this then you probably need to rewrite it as:[br]
		[example]
		%x = 10
		[cmd]python.begin[/cmd]
		x = kvirc.getLocal("x")
		kvirc.eval("echo \"The value is ".$x."\"")
		[cmd]python.end[/cmd]
		[/example]
		[br]

		[big]A shortcut for kvirc.eval("/say...")[/big][br]
		Since kvirc.eval("/say...") is a common calling pattern then say
		has been added to the KVIrc python namespace. You can now call
		[example]
		kvirc.say("Hi all!")
		[/example]
		and that will mimic the behaviour of
		[example]
		/[cmd]say[/cmd] Hi all!
		[/example]
		The complete syntax for kvirc.say() is:[br]
		&nbsp; &nbsp; [b]kvirc.say(&lt;text&gt;[,&lt;windowid&gt;])[/b][br]
		and the semantics are obvious (see also /[cmd]say[/cmd]).
		[br]

		[big]The python script return values[/big][br]
		The [cmd]python.begin[/cmd] command propagates the python code return
		value to the KVIrc context (just like a setreturn() would do).[br]
		In fact the python snippet return value is the last "thing" that
		the interpreter evaluates.[br]
		In this way you can write python aliases that return values
		without doing any variable passing equilibrism.[br]
		[br]

		[big]Executing python scripts from files[/big][br]
		[example]
		[cmd]alias[/cmd](pythonexec)
		{
			%tmp = "python.begin(\"\",$1,$2,$3,$4,$5)";
			%tmp .= $file.read($0);
			%tmp .= "python.end";
			eval %tmp;
		}
		pythonexec "/home/pragma/mypythonscript.pl" "param1" "param2" "param3"
		[comment]# or even[/comment]
		[cmd]echo[/cmd] $pythonexec("/home/pragma/computeprimelargerthan.pl","10000")
		[/example]
		[br]

		[big]Curiosity[/big][br]
		The python support in KVIrc is implemented as a master-slave module pair.
		The python.* module is the master while pythoncore is the slave.
		When the python support isn't compiled in, the python.* commands
		print some warnings and exit gracefully while the pythoncore module
		refuses to be loaded. When python support is compiled in but
		for some reason the libpython.so can't be found or loaded
		then pythoncore fails the dynamic loading stage but python.* still fails
		gracefully with just some warning messages. This trick allows
		the scripters to check for python support with [fnc]python.isAvailable[/fnc]
		and to embed python code snippets in KVS even if the support is missing.
		The snippets will be just skipped.[br]
		[br]
		Happy python hacking :)[br]
*/

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
			kvirc.eval("echo \"Hello World from python!\"");
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
