//=============================================================================
//
//   File : libkviperl.cpp
//   Creation date : Tue Jul 13 13:03:31 2004 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2004-2008 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "kvi_settings.h"
#include "kvi_module.h"
#include "kvi_fileutils.h"
#include "kvi_locale.h"
#include "kvi_app.h"
#include "kvi_options.h"
#include "kvi_modulemanager.h"
#include "kvi_out.h"

#ifdef COMPILE_PERL_SUPPORT
	#include "../perlcore/perlcoreinterface.h"

	static KviModule * g_pPerlCoreModule = 0;
#endif // COMPILE_PERL_SUPPORT

#ifdef COMPILE_PERL_SUPPORT
	#define KVS_CHECK_PERLCORE(_m,_c) \
		g_pPerlCoreModule = g_pModuleManager->getModule("perlcore"); \
		if(!g_pPerlCoreModule) \
		{ \
			if(!_c->switches()->find('q',"quiet")) \
			{ \
				_c->warning(__tr2qs_ctx("The perlcore module can't be loaded: perl support not available","perl")); \
				_c->warning(__tr2qs_ctx("To see more details about loading failure try /perlcore.load","perl")); \
				return true; \
			} \
		}
#else // !COMPILE_PERL_SUPPORT
	#define KVS_CHECK_PERLCORE(_m,_c)
#endif // !COMPILE_PERL_SUPPORT

#ifdef COMPILE_PERL_SUPPORT
	#define KVS_CHECK_MODULE_STATE(_m,_c) KVS_CHECK_PERLCORE(_m,_c)
#else // !COMPILE_PERL_SUPPORT
	#define KVS_CHECK_MODULE_STATE(_m,_c) \
		if(!_c->switches()->find('q',"quiet")) \
			_c->warning(__tr2qs_ctx("This KVIrc executable has been compiled without perl scripting support","perl")); \
		return true;
#endif // !COMPILE_PERL_SUPPORT

/*
	@doc: perl_and_kvs
	@type:
		language
	@title:
		Using perl from KVS and vice-versa.
	@short:
		How to use perl from KVS and KVS from perl.
	@body:
		[big]Introduction[/big][br]
		Starting from version 3.0.2 you can include perl code snippets
		in KVS code and you can use KVS commands from within perl.
		This feature is present only if a working perl installation
		has been found at build time.[br]
		[br]

		[big]Using perl from KVS[/big][br]
		Using perl from KVIrc is really easy: just enclose
		your perl code snippet inside [cmd]perl.begin[/cmd] and [cmd]perl.end[/cmd].
		[example]
		[cmd]perl.begin[/cmd]
		&lt;perl code goes here&gt;
		[cmd]perl.end[/cmd]
		[/example]
		For example:[br]
		[example]
		[cmd]perl.begin[/cmd]
		open(MYFILE,'>>myfile.txt') or die "Can't open myfile.txt!";
		print MYFILE "foo!\n";
		close(MYFILE);
		[cmd]perl.end[/cmd]
		[/example]
		A perl code snippet can appear anywhere a KVS code snippet can
		with the only restriction that i must be enclosed in [cmd]perl.begin[/cmd]
		and [cmd]perl.end[/cmd]. This means that you can write perl code
		in the commandline, in the aliases, the event handlers, popups...anywhere.[br]
		If you have already encountered the KVIrc's [cmd]eval[/cmd] command
		that you probably also know how to execute a perl code snippet from a file :)[br]
		[br]

		[big]Using KVS from perl[/big][br]
		KVIrc exports several commands to the perl namespace
		that allow you to invoke KVIrc's functions from inside the perl code snippet.[br]
		The nicest example is KVIrc::echo():
		[example]
		[cmd]perl.begin[/cmd]
		KVIrc::echo("Hello KVIrc world from perl!");
		[cmd]perl.end[/cmd]
		[/example]
		KVIrc::echo() is the counterpart of the [cmd]echo[/cmd].
		The exact syntax is:[br]
		&nbsp; &nbsp; [b]KVIrc::echo(&lt;text&gt;[,&lt;colorset&gt;[,&lt;windowid&gt;]])[/b][br]
		&lt;text&gt; is obviously the text to be printed. &lt;colorset&gt; is
		the equivalent of the [cmd]echo[/cmd] -i option and &lt;windowid&gt;
		is the equivalent of the -w option. Both &lt;colorset&gt; and &lt;windowid&gt;
		can be omitted (in this case KVIrc will use a default colorset and the current window).[br]
		[br]

		[big]Perl execution contexts[/big][br]
		The perl code snippets are executed by the means of a perl interpreter.
		Each perl interpreter has its own context and thus it's own variables,
		own function namespace etc.[br]
		[br]
		In the example above KVIrc creates an interpreter when [cmd]perl.begin[/cmd]
		is invoked and destroys it at [cmd]perl.end[/cmd] parsing time.
		In fact, KVIrc can mantain multiple persistent interpreters that will
		allow you to preserve your context across [cmd]perl.begin[/cmd] invocations.[br]
		You can invoke a specific perl context by passing it as parameter to the [cmd]perl.begin[/cmd]
		command.[br]
		[example]
		[cmd]perl.begin("mycontext")[/cmd]
		$myvariable = "mycontext";
		KVIrc::echo("This perl code is executed from ".$myvariable);
		[cmd]perl.end[/cmd]
		[/example]
		The nice thing is that at a later time you can invoke this context again
		and discover that $mycontext has preserved its value:[br]
		[example]
		[cmd]perl.begin("mycontext")[/cmd]
		KVIrc::echo("myvariable is still equal to ".$myvariable);
		[cmd]perl.end[/cmd]
		[/example]
		The first time you invoke a named perl context it gets automatically created and
		it persists until KVIrc terminates or the perl context is explicitly destroyed
		by the means of [cmd]perl.destroy[/cmd].[br]
		[br]
		In fact there is a third possibility to destroy a context: it's when the
		perlcore module is forcibly unloaded (by the means of /perlcore.unload) but
		this is really a rare case and should be threated just like a KVIrc restart (the
		user probably WANTS the contexts to be reinitialized).[br]
		[br]
		The nice thing is that not only your variables will get preserved
		but also any perl function or class you declare in a context will persist.
		It's just like executing a long perl script file with pauses inside.[br]
		[br]
		If you omit the perl context name in the [cmd]perl.begin[/cmd] command
		(or if you use an empty string in it's place)
		then KVIrc will create a temporary context for the snippet execution
		and will destroy it immediately after [cmd]perl.end[/cmd] has been called.[br]
		[br]
		The major side effect of keeping persistent perl contexts is that
		the perl's symbol table will grow and if not used carefully the interpreter
		may become a memory hog. So if you're going to use persistent contexts
		either try to keep the symbol table clean or explicitly call [cmd]perl.destroy[/cmd]
		once in a while to recreate the interpreter.[br]
		If you just execute occasional perl code snippets and don't need to keep persistent variables
		then just use the nameless temporary context provided by [cmd]perl.begin[/cmd]("").[br]
		[br]

		[big]Passing parameters to the perl script[/big][br]
		The easiest way to pass parameters to the perl code snippet
		is to put them as [cmd]perl.begin[/cmd] arguments.
		In fact the complete syntax of [cmd]perl.begin[/cmd] is:[br]
		[b]perl.begin(&lt;perl context&gt;,&lt;arg0&gt;,&lt;arg1&gt;,...)[/b][br]
		Where the &lt;arg0&gt;,&lt;arg1&gt;...&lt;argN&gt; parameters
		are passed to the perl context as elements of the $_[] array.[br]
		[example]
		[cmd]perl.begin[/cmd]("","Hello world!","Now I CAN",1,2,3)
		for($i=0;$i<5;$i++)
			KVIrc::echo($_[i],40);
		[cmd]perl.end[/cmd]
		[/example]
		[br]

		[big]Accessing the KVIrc scripting context from perl[/big][br]
		KVIrc exposes the following functions that manipulate the
		variables of the KVIrc's current KVS execution context.[br]
		&nbsp; &nbsp; [b]KVIrc::getLocal(&lt;x&gt;)[/b][br]
		Returns the value of the KVIrc's local variable %x.[br]
		&nbsp; &nbsp; [b]KVIrc::getGlobal(&lt;Y&gt;)[/b][br]
		Returns the value of the KVIrc's global variable %Y.[br]
		&nbsp; &nbsp; [b]KVIrc::setLocal(&lt;x&gt;,&lt;value&gt;)[/b][br]
		Sets the KVIrc's global variable %x to &lt;value&gt;[br]
		&nbsp; &nbsp; [b]KVIrc::setGlobal(&lt;Y&gt;,&lt;value&gt;)[/b][br]
		Sets the KVIrc's global variable %Y to &lt;value&gt;[br]
		The local variables interested belong to the current KVS exection context
		while the global variables are visible everywhere.[br]
		[example]
		%pippo = test
		%Pluto = 12345
		[cmd]perl.begin[/cmd]
		$mypippo = KVIrc::getLocal("pippo");
		$mypippo =~ s/^pi/ze/g;
		$mypluto = KVIrc::getGlobal("Pluto");
		$mypluto =~ s/23/xx/g;
		KVIrc::setLocal("pippo",$mypluto);
		KVIrc::setGlobal("Pluto",$mypippo);
		[cmd]perl.end[/cmd]
		[cmd]echo[/cmd] "\%pippo is" %pippo
		[cmd]echo[/cmd] "\%Pluto is" %Pluto
		[/example]
		[br]

		[big]Executing arbitrary KVIrc commands from perl[/big][br]
		You can execute arbitrary KVS commands from perl by the means of:[br]
		&nbsp; &nbsp; [b]KVIrc::eval(&lt;code&gt;)[/b][br]
		This function behaves exactly like the ${ &lt;code&gt; } KVS construct:
		it executes &lt;code&gt; in a child context and returns it's evaluation retult.[br]
		The following two code snippets have equivalent visible effects:[br]
		[example]
		[cmd]echo[/cmd] ${ return "Yeah!"; }
		[/example]
		[example]
		[cmd]perl.begin[/cmd]
		KVIrc::echo(KVIrc::eval("return \"Yeah!\""));
		[cmd]perl.end[/cmd]
		[/example]
		You can "eval" composite command sequences and variable ones.[br]
		Remember that the perl code snippet is evaluated in a child KVS context
		and thus the local variables are NOT visible!.
		The following code snippets may easily fool you:[br]
		[example]
		%x = 10
		[cmd]perl.begin[/cmd]
		KVIrc::eval("echo \"The value is %x\"");
		[cmd]perl.end[/cmd]
		[/example]
		This will print "The value is " since %x is not accessible from the eval's context.
		If you have tried to write something like this then you probably need to rewrite it as:[br]
		[example]
		%x = 10
		[cmd]perl.begin[/cmd]
		$x = KVIrc::getLocal("x");
		KVIrc::eval("echo \"The value is ".$x."\"");
		[cmd]perl.end[/cmd]
		[/example]
		[br]
		Note also that you must either escape the $ at the beginning of the KVIrc identifiers
		or use the single quotes to prevent perl from catching the $ as the beginning of
		a variable.
		[example]
		[comment]# This will not work as expected[/comment]
		[cmd]perl.begin[/cmd]
		KVIrc::echo(KVIrc::eval("return $window.caption"));
		[cmd]perl.end[/cmd]
		[comment]# But these will do[/comment]
		[cmd]perl.begin[/cmd]
		KVIrc::echo(KVIrc::eval("return \$window.caption"));
		KVIrc::echo(KVIrc::eval('return $window.caption'));
		[cmd]perl.end[/cmd]
		[/example]

		[big]A shortcut for KVIrc::eval("/say...")[/big][br]
		Since KVIrc::eval("/say...") is a common calling pattern then say
		has been added to the KVIrc perl namespace. You can now call
		[example]
		KVIrc::say("Hi all!");
		[/example]
		and that will mimic the behaviour of
		[example]
		/[cmd]say[/cmd] Hi all!
		[/example]
		The complete syntax for KVIrc::say() is:[br]
		&nbsp; &nbsp; [b]KVIrc::say(&lt;text&gt;[,&lt;windowid&gt;])[/b][br]
		and the semantics are obvious (see also /[cmd]say[/cmd]).
		[br]

		[big]The perl script return values[/big][br]
		The [cmd]perl.begin[/cmd] command propagates the perl code return
		value to the KVIrc context (just like a setreturn() would do).[br]
		In fact the perl snippet return value is the last "thing" that
		the interpreter evaluates.[br]
		In this way you can write perl aliases that return values
		without doing any variable passing equilibrism.[br]
		[br]

		[big]Executing perl scripts from files[/big][br]
		[example]
		[cmd]alias[/cmd](perlexec)
		{
			%tmp = "perl.begin(\"\",$1,$2,$3,$4,$5)";
			%tmp .= $file.read($0);
			%tmp .= "perl.end";
			eval %tmp;
		}
		perlexec "/home/pragma/myperlscript.pl" "param1" "param2" "param3"
		[comment]# or even[/comment]
		[cmd]echo[/cmd] $perlexec("/home/pragma/computeprimelargerthan.pl","10000")
		[/example]
		[br]

		[big]Other tricks[/big][br]
		An interesting feature of the persistent perl contexts is
		that you can prepare a context for a later fast execution.[br]
		The idea is to declare perl functions in a single perl code snippet
		and to call the single functions when a fast execution is needed.[br]
		For example you might parse the following snippet at KVIrc's startup:[br]
		[example]
		[cmd]perl.begin[/cmd]("persistent")
		sub handler_for_event_1
		{
			do_complex_perl_stuff_here
		}
		sub handler_for_event_2
		{
			do_complex_perl_stuff_here
		}
		[cmd]perl.end[/cmd]
		[/example]
		and later simply call:
		[example]
		[cmd]perl.begin[/cmd]("persistent",param1,param2)
		handler_for_event_1($_[0],$_[1])
		[cmd]perl.end[/cmd]
		[/example]
		[br]

		[big]Curiosity[/big][br]
		The perl support in KVIrc is implemented as a master-slave module pair.
		The perl.* module is the master while perlcore is the slave.
		When the perl support isn't compiled in, the perl.* commands
		print some warnings and exit gracefully while the perlcore module
		refuses to be loaded. When perl support is compiled in but
		for some reason the libperl.so can't be found or loaded
		then perlcore fails the dynamic loading stage but perl.* still fails
		gracefully with just some warning messages. This trick allows
		the scripters to check for perl support with [fnc]perl.isAvailable[/fnc]
		and to embed perl code snippets in KVS even if the support is missing.
		The snippets will be just skipped.[br]
		[br]
		Happy perl hacking :)[br]
*/

/*
	@doc: perl.begin
	@type:
		command
	@title:
		perl.begin
	@keyterms:
		Including perl code snippets in KVS
	@short:
		Starts a perl code snippet
	@syntax:
		perl.begin [-n] [-q] [(<perl_context>[,<arg0>[,<arg1>[...]]])]
		<perl code>
		perl.end
	@switches:
		!sw: -q | --quiet
		Prevents the command from printing any warnings.
		!sw: -n | --no-return
		Prevents the perl script return value to be propagated
		to the current context.
		!sw: -f | --fail-on-error
		Treat perl errors as KVS errors and abort execution of the
		current script. Incompatible with -q
	@description:
		Indicates the beginning of a snippet of perl code.
		The whole code part between perl.begin and perl.end
		is executed in a perl interpreter.
		If perl.end is omitted then it is implicitly assumed
		that the code from perl.begin to the end of the command
		buffer is perl.[br]
		Each perl code execution is bound to a
		perl context (that is in fact a particular instance
		of a perl interpreter). If <perl_context> is not specified
		or it is an empty string then temporary perl interpreter is created
		and destroyed just after the code snippet has terminated execution.
		If <perl_context> is specified then a perl interpreter
		keyed to that context is used: if it was already existing
		then it is reused otherwise it is created.
		Any <perl_context> is persistent: it mantains the function
		declarations and perl variable states until explicitly
		destroyed with [cmd]perl.destroy[/cmd] (or the perlcore
		module is forcibly unloaded).[br]
		The <arg0>,<arg1>,... arguments, if present, are passed
		to the perl code snippet in the @_ array (accessible as $_[0],$_[1]...).[br]
		The return value of the perl code is propagated to the current
		context (just like [cmd]setreturn[/cmd] was called on it) unless
		the -n switch is used.[br]
		The -q switch prevents from the command from printing any
		warning.[br]
		See the [doc:perl_and_kvs]perl scripting documentation[/doc]
		for more information.
	@examples:
		[example]
			perl.begin
			KVIrc::eval("echo \"Hello World from perl!\"");
			perl.end
		[/example]
	@seealso:
*/

/*
	@doc: perl.end
	@type:
		command
	@title:
		perl.end
	@short:
		Ends a perl code snippet
	@syntax:
		perl.begin[(<perl_context>)]
		<perl code>
		perl.end
	@description:
		Ends a perl code snippet. See [cmd]perl.begin[/cmd].
	@seealso:
		[cmd]perl.begin[/cmd]
*/

static bool perl_kvs_cmd_begin(KviKvsModuleCommandCall * c)
{
	// This command is somewhat special in the fact that has a dedicated
	// parsing routine in the KVS core parser.
	// The parser sets the perl code as the first parameter of our call,
	// the remaining params are the context name and the arguments

	QString szCode,szContext;
	KviKvsVariantList vList;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("code",KVS_PT_STRING,0,szCode)
		KVSM_PARAMETER("context",KVS_PT_STRING,KVS_PF_OPTIONAL,szContext)
		KVSM_PARAMETER("args",KVS_PT_VARIANTLIST,KVS_PF_OPTIONAL,vList)
	KVSM_PARAMETERS_END(c)

	KVS_CHECK_MODULE_STATE(m,c)

#ifdef COMPILE_PERL_SUPPORT
	KviPerlCoreCtrlCommand_execute ex;
	ex.uSize = sizeof(KviPerlCoreCtrlCommand_execute);
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

	if(!g_pPerlCoreModule->ctrl(KVI_PERLCORECTRLCOMMAND_EXECUTE,&ex))
	{
		if(!c->switches()->find('q',"quiet"))
			c->warning(__tr2qs_ctx("The perlcore module failed to execute the code: something is wrong with the perl support","perl"));
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
				c->warning(__tr2qs_ctx("Perl execution error:","perl"));
				c->warning(ex.szError);
				return false;
			} else {
				c->warning(__tr2qs_ctx("Perl execution error:","perl"));
				c->error(ex.szError);
			}
		}
	}

	if(!c->switches()->find('n',"no-return"))
		c->context()->returnValue()->setString(ex.szRetVal);

#endif //COMPILE_PERL_SUPPORT

	return true;
}

/*
	@doc: perl.destroy
	@type:
		command
	@title:
		perl.destroy
	@short:
		Destroys a perl execution context
	@syntax:
		perl.destroy [-q] <context_name:string>
	@description:
		Destroys the perl execution context <context_name>.
		If the context is not existing then a warning is printed unless the
		-q switch is used.[br]
		The destruction will clear any state associated to the context
		including the stored functions, classes and variable symbols.
		You may want to destroy a context to re-initialize its state
		or to simply clear it's memory when it's no longer needed.
	@seealso:
		[cmd]perl.begin[/cmd]
*/

static bool perl_kvs_cmd_destroy(KviKvsModuleCommandCall * c)
{
	QString szContext;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("context",KVS_PT_NONEMPTYSTRING,0,szContext)
	KVSM_PARAMETERS_END(c)

	KVS_CHECK_MODULE_STATE(m,c)

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

	return true;
}


/*
	@doc: perl.isAvailable
	@type:
		function
	@title:
		$perl.isAvailable
	@short:
		Check if perl scripting support is available
	@syntax:
		$perl.isAvailable
	@description:
		Returns 1 if the perl scripting support is available and 0 otherwise.
*/

static bool perl_kvs_fnc_isAvailable(KviKvsModuleFunctionCall * c)
{
#ifdef COMPILE_PERL_SUPPORT
	g_pPerlCoreModule = g_pModuleManager->getModule("perlcore");
	c->returnValue()->setBoolean(g_pPerlCoreModule ? true : false);
#else // COMPILE_PERL_SUPPORT
	c->returnValue()->setBoolean(false);
#endif // COMPILE_PERL_SUPPORT
	return true;
}

static bool perl_module_init(KviModule * m)
{
	// register the command anyway
	KVSM_REGISTER_SIMPLE_COMMAND(m,"begin",perl_kvs_cmd_begin);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"destroy",perl_kvs_cmd_destroy);

	KVSM_REGISTER_FUNCTION(m,"isAvailable",perl_kvs_fnc_isAvailable);

	// FIXME: perl.isSupported()
#ifdef COMPILE_PERL_SUPPORT
	g_pPerlCoreModule = g_pModuleManager->getModule("perlcore");
#endif // COMPILE_PERL_SUPPORT
	return true;
}

static bool perl_module_cleanup(KviModule *)
{
	return true;
}

KVIRC_MODULE(
	"Perl",                                                 // module name
	"4.0.0",                                                // module version
	"Copyright (C) 2004 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
	"Perl scripting engine",
	perl_module_init,
	0,
	0,
	perl_module_cleanup,
	"perl"
)
