//=============================================================================
//
//   File : libkvipython.cpp
//   Creation date : Fri Nov 07 00:18:31 2008 GMT by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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

#include "kvi_settings.h"
#include "KviModule.h"
#include "KviModuleManager.h"
#include "KviLocale.h"

#ifdef COMPILE_PYTHON_SUPPORT
#include "../pythoncore/pythoncoreinterface.h"
#include "../pythoncore/pythonheaderwrapper.h"

static KviModule * g_pPythonCoreModule = nullptr;

#define KVS_CHECK_PYTHONCORE(_m, _c)                                                                                   \
	g_pPythonCoreModule = g_pModuleManager->getModule("pythoncore");                                                   \
	if(!g_pPythonCoreModule)                                                                                           \
	{                                                                                                                  \
		if(!_c->switches()->find('q', "quiet"))                                                                        \
		{                                                                                                              \
			_c->warning(__tr2qs_ctx("The pythoncore module can't be loaded: Python support not available", "python")); \
			_c->warning(__tr2qs_ctx("To see more details about loading failure try /pythoncore.load", "python"));      \
			return true;                                                                                               \
		}                                                                                                              \
	}

#define KVS_CHECK_MODULE_STATE(_m, _c) KVS_CHECK_PYTHONCORE(_m, _c)

#else // COMPILE_PYTHON_SUPPORT

#define KVS_CHECK_PYTHONCORE(_m, _c)

#define KVS_CHECK_MODULE_STATE(_m, _c)                                                                                  \
	if(!_c->switches()->find('q', "quiet"))                                                                             \
		_c->warning(__tr2qs_ctx("This KVIrc executable has been compiled without Python scripting support", "python")); \
	return true;

static KviModule * g_pPythonCoreModule = nullptr;
#endif // COMPILE_PYTHON_SUPPORT

/*
	@doc: python_and_kvs
	@type:
		language
	@title:
		Using Python from KVS and vice-versa.
	@short:
		How to use Python from KVS and KVS from Python.
	@body:
		[big]Introduction[/big]
		Starting from version 4.0.0 you can include Python code snippets
		in KVS code and you can use KVS commands from within Python.
		This feature is present only if a working Python installation
		has been found at build time.[br]
		The Python support is very similar to the Perl support present
		since 3.x, so if you have used Perl from KVIrc before you'll
		find the API is almost the same.. otherwise read on :)
		[big]Using Python from KVS[/big]
		Using Python from KVIrc is really easy - just enclose
		your Python code snippet inside [cmd]python.begin[/cmd] and [cmd]python.end[/cmd].
		[example]
			[cmd]python.begin[/cmd]
			&lt;python code goes here&gt;
			[cmd]python.end[/cmd]
		[/example]
		For example:
		[example]
			[cmd]python.begin[/cmd]
			f = open('myfile.txt', 'w')
			f.write('This is a test\n')
			f.close()
			[cmd]python.end[/cmd]
			[/example]
		A Python code snippet can appear anywhere a KVS code snippet can
		with the only restriction that it must be enclosed in [cmd]python.begin[/cmd]
		and [cmd]python.end[/cmd]. This means that you can write Python code
		in the commandline, in the aliases, the event handlers, popups... anywhere.[br]
		If you have already encountered KVIrc's [cmd]eval[/cmd] command
		then you probably also know how to execute a Python code snippet from a file :)
		[big]Using KVS from python[/big]
		KVIrc exports several commands to the Python namespace
		that allow you to invoke KVIrc's functions from inside the Python code snippet.[br]
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
		can be omitted (in this case KVIrc will use a default colorset and the current window).
		[big]Python execution contexts[/big]
		The Python code snippets are executed by a Python interpreter - each
		interpreter has its own context and thus its own variables,
		own function namespace etc.[br]
		[br]
		In the example above, KVIrc creates an interpreter when [cmd]python.begin[/cmd]
		is invoked and destroys it at [cmd]python.end[/cmd] parsing time.
		In fact, KVIrc can maintain multiple persistent interpreters that will
		allow you to preserve your context across [cmd]python.begin[/cmd] invocations.[br]
		[br]
		You can invoke a specific Python context by passing it as parameter to the [cmd]python.begin[/cmd]
		command:
		[example]
			[cmd]python.begin("mycontext")[/cmd]
			myvariable = "mycontext"
			kvirc.echo("This Python code is executed from " + myvariable)
			[cmd]python.end[/cmd]
		[/example]
		The nice thing is that at a later time you can invoke this context again
		and discover that mycontext has preserved its value:
		[example]
			[cmd]python.begin("mycontext")[/cmd]
			kvirc.echo("myvariable is still equal to " + myvariable)
			[cmd]python.end[/cmd]
		[/example]
		The first time you invoke a named Python context it is automatically created and
		persists until KVIrc terminates or the Python context is explicitly destroyed
		by [cmd]python.destroy[/cmd].[br]
		[br]
		There is a third possibility to destroy a context - when the
		pythoncore module is forcibly unloaded (by /pythoncore.unload). This
		is however a rare case and should be treated just like a KVIrc restart (the
		user probably WANTS the contexts to be reinitialized).[br]
		[br]
		The nice thing is that not only will your variables be preserved, any python
		function or class you declare in a context will persist.
		It's just like executing a long Python script file with pauses inside.[br]
		[br]
		If you omit the Python context name in the [cmd]python.begin[/cmd] command
		(or if you use an empty string in its place)
		then KVIrc will create a temporary context for the snippet execution
		and will destroy it immediately after [cmd]python.end[/cmd] has been called.[br]
		[br]
		The major side effect of keeping persistent Python contexts is that
		python's symbol table will grow, and if not used carefully, the interpreter
		may become a memory hog. So if you're going to use persistent contexts,
		either try to keep the symbol table clean or explicitly call [cmd]python.destroy[/cmd]
		once in a while to recreate the interpreter.[br]
		If you just execute occasional Python code snippets and don't need to keep
		persistent variables, then just use the nameless temporary context provided
		by [cmd]python.begin[/cmd]("").
		[big]Passing parameters to the Python script[/big]
		The easiest way to pass parameters to the Python code snippet
		is to put them as [cmd]python.begin[/cmd] arguments.
		In fact the complete syntax of [cmd]python.begin[/cmd] is:[br]
		[b]python.begin(&lt;python context&gt;,&lt;arg0&gt;,&lt;arg1&gt;,...)[/b][br]
		Where the &lt;arg0&gt;,&lt;arg1&gt;...&lt;argN&gt; parameters
		are passed to the Python context as elements of the aArgs array.
		[example]
			[cmd]python.begin[/cmd]("","Hello world!","Now I CAN",1,2,3)
			for l in range(0,5):
			    kvirc.echo(aArgs[l])
			[cmd]python.end[/cmd]
		[/example]
		[big]Accessing the KVIrc scripting context from python[/big]
		KVIrc exposes the following functions that manipulate
		variables of KVIrc's current KVS execution context:[br]
		&nbsp; &nbsp; [b]kvirc.getLocal(&lt;x&gt;)[/b][br]
		Returns the value of KVIrc's local variable %x.[br]
		&nbsp; &nbsp; [b]kvirc.getGlobal(&lt;Y&gt;)[/b][br]
		Returns the value of KVIrc's global variable %Y.[br]
		&nbsp; &nbsp; [b]kvirc.setLocal(&lt;x&gt;,&lt;value&gt;)[/b][br]
		Sets KVIrc's local variable %x to &lt;value&gt;[br]
		&nbsp; &nbsp; [b]kvirc.setGlobal(&lt;Y&gt;,&lt;value&gt;)[/b][br]
		Sets KVIrc's global variable %Y to &lt;value&gt;[br]
		The local variables referenced belong to the current KVS execution context
		while the global variables are visible everywhere.
		[example]
			%pippo = test
			%Pluto = 12345
			[cmd]python.begin[/cmd]
			mypippo = kvirc.getLocal("pippo")
			mypippo += " rox"
			mypluto = kvirc.getGlobal("Pluto")
			mypluto += " rox"
			kvirc.setLocal("pippo",mypippo)
			kvirc.setGlobal("Pluto",mypluto)
			[cmd]python.end[/cmd]
			[cmd]echo[/cmd] "\%pippo is" %pippo
			[cmd]echo[/cmd] "\%Pluto is" %Pluto
		[/example]
		[big]Executing arbitrary KVIrc commands from python[/big]
		You can execute arbitrary KVS commands from Python by means of:[br]
		&nbsp; &nbsp; [b]kvirc.eval(&lt;code&gt;)[/b][br]
		This function behaves exactly like the ${ &lt;code&gt; } KVS construct -
		it executes &lt;code&gt; in a child context and returns its evaluation result.[br]
		The following two code snippets have equivalent visible effects:
		[example]
			[cmd]echo[/cmd] ${ return "Yeah!"; }
		[/example]
		[example]
			[cmd]python.begin[/cmd]
			kvirc.echo(kvirc.eval("return \"Yeah!\""));
			[cmd]python.end[/cmd]
		[/example]
		You can "eval" compound command sequences and variable ones.[br]
		Remember that the Python code snippet is evaluated in a child KVS context
		and thus the local variables are NOT visible!
		The following code snippets may easily fool you:
		[example]
			%x = 10
			[cmd]python.begin[/cmd]
			kvirc.eval("echo \"The value is %x\"")
			[cmd]python.end[/cmd]
		[/example]
		This will print "The value is " since %x is not accessible from the eval's context.
		If you have tried to write something like this then you probably need to rewrite it as:
		[example]
			%x = 10
			[cmd]python.begin[/cmd]
			x = kvirc.getLocal("x")
			kvirc.eval("echo \"The value is ".$x."\"")
			[cmd]python.end[/cmd]
		[/example]
			[big]A shortcut for kvirc.eval("/say...")[/big]
			Since kvirc.eval("/say...") is a common calling pattern, say has been added
			to the KVIrc Python namespace. You can now call
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
		[big]Python script return values[/big]
		The [cmd]python.begin[/cmd] command propagates the Python code return
		value to the KVIrc context (just like a [cmd]setreturn[/cmd]() would do)
		- this makes it easier to create an alias that executes a Python script and
		returns its result.[br]
		[br]
		Without this automatic propagation, you would be forced to play with variables:
		[ul]
		[li]First use [b]kvirc.setLocal('var', '123')[/b] from inside the
		python script;[/li]
		[li]Then, from the KVIrc script after [cmd]python.end[/cmd], retrieve the
		%var variable, check its value and call [cmd]setreturn[/cmd]() on it.[/li]
		[/ul]
		[big]Executing Python scripts from files[/big]
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
		[big]Curiosity[/big]
		The Python support in KVIrc is implemented as a master-slave module pair.
		The python.* module is the master while pythoncore is the slave.
		When Python support isn't compiled in, the python.* commands
		print some warnings and exit gracefully while the pythoncore module
		refuses to be loaded. When Python support is compiled in but
		for some reason the libpython.so can't be found or loaded, pythoncore fails
		the dynamic loading stage, however python.* only fails gracefully with warning
		messages. This trick allows scripters to check for python
		support with [fnc]python.isAvailable[/fnc] and to embed Python code snippets
		in KVS even if the support is missing - the snippets will be just skipped.[br]
		[br]
		Happy Python hacking :)[br]
*/

/*
	@doc: python.begin
	@type:
		command
	@title:
		python.begin
	@keyterms:
		Including Python code snippets in KVS
	@short:
		Starts a Python code snippet
	@syntax:
		python.begin [-n] [-q] [(<python_context>[,<arg0>[,<arg1>[...]]])]
		<python code>
		python.end
	@switches:
		!sw: -q | --quiet
		Prevents the command from printing any warnings.
		!sw: -n | --no-return
		Prevents the Python script return value to be propagated
		to the current context.
		!sw: -f | --fail-on-error
		Treat Python errors as KVS errors and abort execution of the
		current script. Incompatible with -q
	@description:
		Indicates the beginning of a snippet of Python code.
		The whole code part between python.begin and python.end
		is executed in a Python interpreter.
		If python.end is omitted then it is implicitly assumed
		that the code from python.begin to the end of the command
		buffer is Python.[br]
		Each Python code execution is bound to a
		python context (that is in fact a particular instance
		of a Python interpreter). If <python_context> is not specified
		or it is an empty string then temporary Python interpreter is created
		and destroyed just after the code snippet has terminated execution.
		If <python_context> is specified then a Python interpreter
		keyed to that context is used: if it was already existing
		then it is reused otherwise it is created.
		Any <python_context> is persistent: it maintains the function
		declarations and Python variable states until explicitly
		destroyed with [cmd]python.destroy[/cmd] (or the pythoncore
		module is forcibly unloaded).[br]
		The <arg0>,<arg1>,... arguments, if present, are passed
		to the Python code snippet in the aArgs list (accessible as aArgs[0],aArgs[1]...).[br]
		The return value of the Python code is propagated to the current
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
		Ends a Python code snippet
	@syntax:
		python.begin[(<python_context>)]
		<python code>
		python.end
	@description:
		Ends a Python code snippet. See [cmd]python.begin[/cmd].
	@seealso:
		[cmd]python.begin[/cmd]
*/

static bool python_kvs_cmd_begin(KviKvsModuleCommandCall * c)
{
	// This command is somewhat special in the fact that has a dedicated
	// parsing routine in the KVS core parser.
	// The parser sets the Python code as the first parameter of our call,
	// the remaining params are the context name and the arguments

	QString szCode, szContext;
	KviKvsVariantList vList;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("code", KVS_PT_STRING, 0, szCode)
	KVSM_PARAMETER("context", KVS_PT_STRING, KVS_PF_OPTIONAL, szContext)
	KVSM_PARAMETER("args", KVS_PT_VARIANTLIST, KVS_PF_OPTIONAL, vList)
	KVSM_PARAMETERS_END(c)

	KVS_CHECK_MODULE_STATE(m, c)

#ifdef COMPILE_PYTHON_SUPPORT
	KviPythonCoreCtrlCommand_execute ex;
	ex.uSize = sizeof(KviPythonCoreCtrlCommand_execute);
	ex.pKvsContext = c->context();
	ex.szContext = szContext;
	ex.szCode = szCode;
	for(KviKvsVariant * v = vList.first(); v; v = vList.next())
	{
		QString tmp;
		v->asString(tmp);
		ex.lArgs.append(tmp);
	}
	ex.bQuiet = c->switches()->find('q', "quiet");

	if(!g_pPythonCoreModule->ctrl(KVI_PYTHONCORECTRLCOMMAND_EXECUTE, &ex))
	{
		if(!c->switches()->find('q', "quiet"))
			c->warning(__tr2qs_ctx("The pythoncore module failed to execute the code: something is wrong with the Python support", "python"));
		return true;
	}

	if(!ex.lWarnings.isEmpty())
	{
		for(const auto & it : ex.lWarnings)
			c->warning(it);
	}

	if(!ex.bExitOk && !c->switches()->find('q', "quiet"))
	{
		c->warning(__tr2qs_ctx("Python execution error:", "python"));
		c->warning(ex.szError);

		if(c->switches()->find('f', "fail-on-error"))
			return false;
	}

	if(!c->switches()->find('n', "no-return"))
		c->context()->returnValue()->setString(ex.szRetVal);

#endif //COMPILE_PYTHON_SUPPORT

	return true;
}

/*
	@doc: python.destroy
	@type:
		command
	@title:
		python.destroy
	@short:
		Destroys a Python execution context
	@syntax:
		python.destroy [-q] <context_name:string>
	@description:
		Destroys the Python execution context <context_name>.
		If the context does not exist then a warning is printed unless the
		-q switch is used.[br]
		The destruction will clear any state associated with the context
		including the stored functions, classes and variable symbols.
		You may want to destroy a context to re-initialize its state
		or to simply clear its memory when it is no longer needed.
	@seealso:
		[cmd]python.begin[/cmd]
*/

static bool python_kvs_cmd_destroy(KviKvsModuleCommandCall * c)
{
	QString szContext;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("context", KVS_PT_NONEMPTYSTRING, 0, szContext)
	KVSM_PARAMETERS_END(c)

	KVS_CHECK_MODULE_STATE(m, c)

#ifdef COMPILE_PYTHON_SUPPORT
	KviPythonCoreCtrlCommand_destroy ex;
	ex.uSize = sizeof(KviPythonCoreCtrlCommand_destroy);
	ex.szContext = szContext;

	if(!g_pPythonCoreModule->ctrl(KVI_PYTHONCORECTRLCOMMAND_DESTROY, &ex))
	{
		if(!c->switches()->find('q', "quiet"))
			c->warning(__tr2qs_ctx("The pythoncore module failed to execute the code: something is wrong with the Python support", "python"));
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
		Check if Python scripting support is available
	@syntax:
		$python.isAvailable
	@description:
		Returns [b]1[/b] if the Python scripting support is available and [b]0[/b] otherwise.
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
	KVSM_REGISTER_SIMPLE_COMMAND(m, "begin", python_kvs_cmd_begin);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "destroy", python_kvs_cmd_destroy);

	KVSM_REGISTER_FUNCTION(m, "isAvailable", python_kvs_fnc_isAvailable);
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
    "Python",                                                         // module name
    "4.0.0",                                                          // module version
    "Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org", // author & (C)
    "Python Scripting Engine",
    python_module_init,
    0,
    0,
    python_module_cleanup,
    "python")
