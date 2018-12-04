//=============================================================================
//
//   File : KviKvsParser.cpp
//   Creation date : Thu 25 Sep 2003 05.12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviKvsParser.h"
#include "KviKvsTreeNode.h"
#include "KviKvsReport.h"
#include "KviKvsKernel.h"
#include "KviKvsScript.h"
#include "KviKvsParserMacros.h"
#include "KviLocale.h"
#include "KviOptions.h"

//FIXME: @ == $$-> == $this->

KviKvsParser::KviKvsParser(KviKvsScript * pScript, KviWindow * pOutputWindow)
{
	m_pGlobals = nullptr;
	m_pScript = pScript;
	m_pWindow = pOutputWindow;
}

KviKvsParser::~KviKvsParser()
{
	if(m_pGlobals)
		delete m_pGlobals;
}

void KviKvsParser::init()
{
	KviKvsKernel * pKern = KviKvsKernel::instance();

#define _REG_CNTRL_CMD(__cntrlCmdName, __parsingRoutine)                                 \
	{                                                                                    \
		KviKvsSpecialCommandParsingRoutine * r = new KviKvsSpecialCommandParsingRoutine; \
		r->proc = KVI_PTR2MEMBER(KviKvsParser::__parsingRoutine);                        \
		pKern->registerSpecialCommandParsingRoutine(QString(__cntrlCmdName), r);         \
	}

	_REG_CNTRL_CMD("break", parseSpecialCommandBreak);
	_REG_CNTRL_CMD("class", parseSpecialCommandClass);
	_REG_CNTRL_CMD("continue", parseSpecialCommandContinue);
	_REG_CNTRL_CMD("defpopup", parseSpecialCommandDefpopup);
	_REG_CNTRL_CMD("do", parseSpecialCommandDo);
	_REG_CNTRL_CMD("for", parseSpecialCommandFor);
	_REG_CNTRL_CMD("foreach", parseSpecialCommandForeach);
	_REG_CNTRL_CMD("global", parseSpecialCommandGlobal);
	_REG_CNTRL_CMD("help", parseSpecialCommandHelp);
	_REG_CNTRL_CMD("if", parseSpecialCommandIf);
	_REG_CNTRL_CMD("switch", parseSpecialCommandSwitch);
	_REG_CNTRL_CMD("unset", parseSpecialCommandUnset);
	_REG_CNTRL_CMD("while", parseSpecialCommandWhile);
#undef _REG_CNTRL_CMD
}

void KviKvsParser::report(bool bError, const QChar * pLocation, const QString & szMsgFmt, kvi_va_list va)
{
	QString szMsg;
	KviQString::vsprintf(szMsg, szMsgFmt, va);

	KviPointerList<QString> * pCodeListing = nullptr;
	QString szLocation;

	if(pLocation)
	{
		pCodeListing = new KviPointerList<QString>;
		pCodeListing->setAutoDelete(true);

		int iLine, iCol;

		KviKvsReport::findLineColAndListing(m_pBuffer, pLocation, iLine, iCol, pCodeListing);

		szLocation = QString(__tr2qs_ctx("line %1, near character %2", "kvs")).arg(iLine).arg(iCol);
	}
	else
	{
		szLocation = __tr2qs_ctx("beginning of input", "kvs");
	}

	KviKvsReport rep(bError ? KviKvsReport::ParserError : KviKvsReport::ParserWarning, m_pScript->name(), szMsg, szLocation, m_pWindow);
	if(pCodeListing)
		rep.setCodeListing(pCodeListing);

	KviKvsReport::report(&rep, m_pWindow);
}

void KviKvsParser::errorBadChar(const QChar * pLocation, char cExpected, const char * szCommandName)
{
	if(pLocation->unicode())
		error(pLocation, __tr2qs_ctx("Found character '%q' (Unicode 0x%x) where '%c' was expected: see \"/help %s\" for the command syntax", "kvs"),
		    pLocation, pLocation->unicode(), cExpected, szCommandName);
	else
		error(pLocation, __tr2qs_ctx("Found end of input where character '%c' was expected: see \"/help %s\" for the command syntax", "kvs"),
		    cExpected, szCommandName);
}

void KviKvsParser::error(const QChar * pLocation, QString szMsgFmt, ...)
{
	m_bError = true;

	kvi_va_list va;
	kvi_va_start(va, szMsgFmt);
	report(true, pLocation, szMsgFmt, va);
	kvi_va_end(va);
}

void KviKvsParser::warning(const QChar * pLocation, QString szMsgFmt, ...)
{
	kvi_va_list va;
	kvi_va_start(va, szMsgFmt);
	report(false, pLocation, szMsgFmt, va);
	kvi_va_end(va);
}

KviKvsTreeNodeInstruction * KviKvsParser::parse(const QChar * pBuffer, int iFlags)
{
	m_iFlags = iFlags;

	m_bError = false;
	if(m_pGlobals)
		m_pGlobals->clear(); // this shouldn't be needed since this is a one time parser

	m_pBuffer = pBuffer;
	m_ptr = pBuffer;

	if(!pBuffer)
	{
		error(nullptr, __tr2qs_ctx("Empty script", "kvs"));
		return nullptr;
	}
	return parseInstructionList();
}

KviKvsTreeNodeInstruction * KviKvsParser::parseAsExpression(const QChar * pBuffer, int iFlags)
{
	m_iFlags = iFlags;

	m_bError = false;
	if(m_pGlobals)
		m_pGlobals->clear(); // this shouldn't be needed since this is a one time parser

	m_pBuffer = pBuffer;
	m_ptr = pBuffer;

	if(!pBuffer)
	{
		error(nullptr, __tr2qs_ctx("Empty script", "kvs"));
		return nullptr;
	}

	KviKvsTreeNodeExpression * expr = parseExpression(0);
	if(!expr)
		return nullptr;
	return new KviKvsTreeNodeExpressionReturn(pBuffer, expr);
}

KviKvsTreeNodeInstruction * KviKvsParser::parseAsParameter(const QChar * pBuffer, int iFlags)
{
	m_iFlags = iFlags;

	m_bError = false;
	if(m_pGlobals)
		m_pGlobals->clear(); // this shouldn't be needed since this is a one time parser

	m_pBuffer = pBuffer;
	m_ptr = pBuffer;

	if(!pBuffer)
	{
		error(nullptr, __tr2qs_ctx("Empty script", "kvs"));
		return nullptr;
	}

	KviKvsTreeNodeDataList * l = parseCommandParameterList();
	if(!l)
		return nullptr;

	return new KviKvsTreeNodeParameterReturn(pBuffer, l);
}

//
// THE REAL KVS
//
// <script> ::= <instruction list> '\0'
//
// <instruction list> ::= <instruction> [ <instruction list> ]
// <instruction> ::= <instruction block> | <command> | <operation> | <comment>
// <instruction block> ::= '{' <instruction list> '}'
//
// <comment> ::= <bash style line comment> | <c++ style comment> | <c style comment>
// <bash style comment> ::='#' <any char not including newline or null> <newline or null>
// <c++ style comment> ::= '//' <any char not including newline or null> <newline or null>
// <c style comment> ::= '/*' <any char not including null or the sequence */> '*/'
//
// <command> ::= <simple command> | <callback command> | <control command>
// <simple command> ::= <core command> | <module command> | <alias command>
// <core command> ::= <command identifier> <switch list> <command parameter list> <command terminator>
// <switch list> ::= '-'<switch body> [<switch list>]
// <command parameter list> ::= <command parameter>[<space><command parameter>]
// <command parameter> ::= <command parameter part>[<command parameter>]
// <command parameter part> ::= <command literal parameter> | <string parameter> | <data evaluation>
// <command literal parameter> ::= <anything except space, null, newline, ;, ", $ or %>
// <string parameter> ::= '"'<string parameter body>'"'
// <string parameter body> ::= <anything except '"' or null>...
// <command terminator> ::= ';' | '\n' | '\0'
//

//
// <data> ::=
// <data_reference> ::= <function_call> | <structured_data>
// <structured_data> ::= <data_structure> | <variable> | <pointer_data>
// <data_structure> ::= <array> | <hash>
// <array> ::= '%'<identifier>'[]'
// <hash> ::= '%'<identifier>'{}'
// <variable> ::= '%'<identifier> | <array_element> | <hash_element>
// <array_element> ::= '%'<identifier>'['<expression>']'
// <hash_element> ::= '%'<identifier>'{'<key>'}'
// <function_call> ::= <simple_function_call> | <pointer_function_call>
// <simple_function_call> ::= '$'<identifier>'('<function_parameter_list>')'
// <pointer_function_call> ::= <variable>'->'<function_call> | <simple_function_call>'->'<function_call>
// <pointer_data> ::= <variable>'->'<data>' | <simple_function_call>'->'<data>
//

//
// This must evaluate SCOPE OBJECT operators!
// thus...first evaluate the <data> or <simple_function_call>
// If it was <simple_function_call> or <data> evaluation results in returning a <variable>
// then check if there is a scope operator
// if there is then take it as the top of the tree, the <variable> or <simple_function_call>
// go at the left param of the scope operator and re-evaluate the right side <data> or <simple_function_call>
//
//

/*
	@doc: kvs_introduction
	@type:
		language
	@keyterms:
		kvs, compilation
	@title:
		KVIrc scripting language introduction
	@short:
		KVS scripting language introduction
	@body:
		[b]KVS[/b] is the [b]KV[/b]Irc [b]S[/b]cripting language.
		It was inspired by C++, sh, Perl, PHP and mIRC scripting language implementations.
		It is a compromise between flexibility and speed, a [i]workaround[/i] for many intrinsic
		problems of an IRC-oriented scripting language.
		[br]
		KVS is semi-interpreted: the execution is done in two main stages.
		The first stage is the compilation where a syntactic tree is built.
		The second stage is the real execution and is performed by visiting the tree
		in the proper order. The syntactic trees are cached in memory so
		the next executions can jump directly into the second stage.
		This two-stage approach has been introduced in version 3.0.0, the previous
		versions of the language used a single-stage on-the-fly interpreter.
		[br]
		KVS allows you to:
		[ul]
		[li]Implement automated reactions to the events generated by an IRC network[/li]
		[li]Add new complex commands[/li]
		[li]Add interface elements like popups, toolbars, buttons...[/li]
		[li]Add advanced interface elements like complete dialogs or even widgets integrated in KVIrc[/li]
		[/ul]
		KVS contains all the common constructs of structured programming.
		You will find almost all the C control commands, sh/Perl-like variables, arrays and and functions.
		There are also some object-oriented characteristics: you will find C++ like
		objects with constructors, destructors and class inheritance.
		There are also more exotic concepts like the signal-slots inter-object communication.
		Obviously you will also find most of the RFC1459 IRC commands and
		other tools to [i]play[/i] with an IRC connection.[br]
		I'll try to explain the language by using examples instead of strict syntactic rules.[br]
		I have even tried to write the rules... take a look [doc:syntactic_rules]here[/doc][br][br]
		And please... forgive me for my [i]fantastic[/i] English :)
		Szymon Stefanek
*/

/*
	@doc: kvs_basicconcepts
	@type:
		language
	@keyterms:
		script
	@title:
		KVS basic concepts
	@short:
		KVS basic concepts
	@body:
		[big]Scripts[/big]
		You use KVS to implement [b]scripts[/b].
		A script is basically a finite list of KVS instructions.
		When you type a command in the KVIrc input window you in fact
		execute a small one-line script. You can store
		longer scripts in KVIrc memory and execute them at later time.
		Scripts can be also read from external files by the means of the
		[cmd]parse[/cmd] command.
		[br]
		There is an issue with the word [i]script[/i] that is worth clearing here.
		It is common usage to call [i]script[/i] a thing that is something more
		that a finite list of (some scripting language) instructions.
		In fact a set of scripts, documentation files, graphics or other multimedia
		files and sometimes executable binaries is still called a [i]script[/i]... just like
		the [i]PrincoScript[/i] or [i]dynamirc[/i] (tough this last one should be categorized as [i]malware[/i] instead).
		In KVIrc such a collection of items is called [i]addon[/i], but be prepared
		for both usages of the word in this documentation and around the web.[br]
		[doc:kvs_addons]Learn more about KVS addons[/doc].
		[big]Hello world![/big]
		This documentation contains a lot of script examples.
		They will appear like the following block of code:
		[example]
			[cmd]echo[/cmd] Hello world!
		[/example]
		The best way to experiment is to execute the scripts from an external file.
		Try to copy & paste the example above to a file and save it in
		a known place. Then in the command input window type
		[example]
			[b]/[/b][cmd]parse[/cmd] <filename>
		[/example]
		where <filename> stands for the name of the file you just have saved.
		Some simple examples (like the one above) can be also typed
		directly in the command input window.
		You must remember that the command input window needs
		a leading slash ('/') character to recognize a script.
		The command input window can also be put in multi-line mode by clicking
		on the button on the right.
		Another alternative for testing scripts is the code tester window.
		You can access it by selecting [i]New code tester[/i] from the Scripting menu
		at the top of the KVIrc window. You will soon have the opportunity to
		experiment with all the methods. Read on.
		[big]Basic syntax[/big]
		A script contains a list of instructions separated by newlines or ';' characters.
		Placing an instruction per line does not require a terminating character,
		placing more instructions in a single line require them to be separated by ';'.
		The most common instructions in KVS are [b]commands[/b]. A command is basically
		a keyword followed by a list of space separator parameters.
		The simplest (and the most useful) command in KVS is [cmd]echo[/cmd]; it prints
		all its parameters to a KVIrc window.[br]
		The following is an example of a valid script that uses only [cmd]echo[/cmd] commands.
		[example]
			echo "This is the first line"
			ECHO This is the second line;
			echo "This is the third line"; echo This is still on the third line;
			eChO "This is the fourth line"; Echo "This is still on the fourth line"
		[/example]
		You have probably noticed that the terminating ';' character is optional
		when the command is the last in a line.
		The commands are [b]case insensitive[/b]; 'echo' is equivalent to 'Echo',
		to 'ECHO' and to 'eChO'. In fact, most of KVS is case insensitive.
		(There are obvious unavoidable exceptions for this rule; for example,
		on UNIX systems file names are case sensitive and this must be
		also reflected in KVS).
		Another interesting thing is that when you execute the script you
		don't see the enclosing quotes around the printed text: more about this
		in the following sections.
		[note]
		Cryptic note (you may skip it for now):[br]
		Yes, the command terminator is a problem for those that want to use ';)' at the end
		of IRC commands like [cmd]msg[/cmd]. It is almost unavoidable (read: the cost for
		avoiding it is too high). Note that using '|' or any other character as command terminator
		will [b]not[/b] solve the problem: if the terminator is too difficult to type it will annoy the
		scripters (and me), if it is too easy then there will be always someone that wants to use it
		at the end (or in the middle) of a command with the original meaning.
		The solution is to escape the [b];[/b] character:
		[example]
			[cmd]echo[/cmd] You can do it now \;)
		[/example]
		[/note]
		[big]Parameter processing[/big]
		Most of the commands accept (and sometimes require) a list of parameters.
		For example, the [cmd]join[/cmd] command (that is used to join an IRC channel)
		accepts two parameters: the first one is the channel to join and the second is
		the password. The simplified syntax for join is:
		[example]
			[cmd]join[/cmd] <channel> [password]
		[/example]
		The line above is an example of syntax specification. All the commands
		are described by such syntax lines. [cmd]join[/cmd] is the command and it stands exactly
		for the literal string [i]join[/i] typed in a script. <channel> is in angular parenthesis
		and represents a mandatory parameter: you must substitute a real channel name in its place
		otherwise the command will fail and KVIrc will probably complain too.
		[password] is still a parameter but the square parentheses indicate that it is
		optional: if you specify it, then it will be interpreted as the channel password,
		if you don't then no password will be used.
		[note]
		The syntax is written in a simplified BNF. I say simplified because it is not
		totally strict around the KVIrc documentation. I just prefer the syntax to be
		clear and easy to read instead of being formally perfect.
		[/note]
		You can finally join a channel by writing:
		[example]
			[cmd]join[/cmd] #kvirc kvircrocks
		[/example]
		or, since #kvirc usually has no password, by writing:
		[example]
			[cmd]join[/cmd] #kvirc
		[/example]
		In the example above the optional parameter [password] is omitted.
		[note]
		In fact it is not really omitted: KVIrc interprets it as an empty string that later
		means [i]do not send the password to the server[/i].
		Empty strings are equivalent to omitted ones.
		[/note]
		[big]Parameters, spaces and quotes[/big]
		From the examples above is obvious that KVS command parameters are separated by spaces.
		What is not totally obvious is that multiple spaces are allowed but KVIrc
		will automatically reduce them to exactly one (just like HTML parsers or the shell
		interpreters do). This is an useful behaviour in an IRC client since spaces usually
		carry no information and in text oriented protocols make the parsing really harder (:D).
		[br]
		The spaces are simplified in normal processing but there are ways to force KVIrc
		to interpret the spaces just as they are.
		The first method are the quotation marks: all the spaces enclosed in quotation marks
		will be preserved.
		[example]
			[cmd]echo[/cmd] This &nbsp; &nbsp; text &nbsp; &nbsp; will &nbsp; &nbsp; have &nbsp; &nbsp; spaces &nbsp; &nbsp; simplified
			[cmd]echo[/cmd] But &nbsp; &nbsp; "this &nbsp; &nbsp; &nbsp; one &nbsp; &nbsp; not"
		[/example]
		The first example will print out with spaces simplified but the second not.
		The quotes are also a nice trick to embed spaces into a single parameter that
		would be obviously split in two or more.
		[example]
			[cmd]echo[/cmd] Parameter1 Parameter2 "Parameter  3 ( with spaces )" Parameter4
		[/example]
		By running the examples above you may have noticed that the spaces are preserved but the
		quotes are then stripped! Yes, this is another tricky behaviour. But don't be afraid:
		it is really easier to use than to explain.
		There is obviously a method to preserve the quotes too and it is also another
		method to preserve the spaces but that leads us to the next paragraph.
		[big]Escape character[/big]
		You may have already noticed that KVS treats some characters in a special way.
		For example the double-quote characters can be used to enclose strings
		and are stripped by the parser.
		Another example of a special character is the command terminator (';'):
		it has the [i]special[/i] meaning of terminating a command.
		If you want to enclose a literal quote in your text, you need to [b]escape[/b] it.
		Like in most other programming languages, the escaping character is the backslash ('\').
		[example]
			[cmd]echo[/cmd] You can smile this way too! \;)
		[/example]
		The above example will treat the ';' as a part of the parameters and print it.[br]
		In some languages the action of [i]escaping[/i] a character is called [i]quoting[/i].
		Although there is some confusion in this term, the meaning is to either use quotes
		or to use the escape character to remove special meaning from some characters.
		By quoting the spaces you can include them in a parameter, by escaping the quotes
		you can include them in a command.
		[example]
			[cmd]echo[/cmd] "And he said \"Hello world!\""
		[/example]
		The example above will have the internal quotes preserved.
		You can use the escape backslash to escape a newline:
		[example]
			[cmd]echo[/cmd] This text will be \
			&nbsp; &nbsp; printed on a single line!
		[/example]
		After an escaped newline all the leading space and tab characters are skipped,
		so you must include the needed spaces [b]before[/b] the escape character.
		The previous example will be printed as:[br][br]
		[i]This text will be printed on a single line[/i][br]
		Another example:
		[example]
			[cmd]echo[/cmd] "The new KVIrc &nbsp; &nbsp; &nbsp \
			&nbsp; &nbsp; IS OUT!"
			[cmd]echo[/cmd] Check it out at http://www.kvi \
			&nbsp; &nbsp; rc.net!
		[/example]
		This will be printed as:[br][br]
		[i]
		The new KVIrc &nbsp; &nbsp; &nbsp; IS OUT![br]
		Check it out at http://www.kvirc.net!
		[/i][br]
		Finally, you can escape an escape character to include it literally in a parameter (and
		this is really the end of these tricks :)
		Later we will discover other common usages of the backslash escape, such
		as preventing KVIrc from interpreting a literal percent character as a variable
		or separating variable names from the text.
		[big]Command switches[/big]
		Many commands accept switch parameters.
		[b]A switch modifies the behaviour of a command.[/b]
		Any switch can optionally accept a parameter, that must
		be specified after an equal ('=') sign.
		[example]
			[cmd]echo[/cmd] [b]-i = 2[/b] This text uses a specific color scheme
		[/example]
		The -i switch (just for example) changes the attributes
		and the icon of the printed text.
		[b]The switch must be specified immediately after the command keyword.[/b]
		[example]
			[cmd]echo[/cmd] This -i = 2 will obviously not work...
		[/example]
		If you want to start the first parameter of a command (that is not a switch)
		with a literal '-' you must again escape it:
		[example]
			[cmd]echo[/cmd] \--- This text has three minus signs on the left
		[/example]
		or use the quotes:
		[example]
			[cmd]echo[/cmd] "--- This text has three minus signs on the left"
		[/example]
		[big]Command blocks[/big]
		Commands can be 'grouped' in blocks by using the classic C++ braces.
		Here is a single line example:
		[example]
			{ [cmd]echo[/cmd] First command; [cmd]echo[/cmd] Second command; } [cmd]echo[/cmd] Third command
		[/example]
		Multi line example:
		[example]
			{
			    [cmd]echo[/cmd] First command
				[cmd]echo[/cmd] Second command
			}
			[cmd]echo[/cmd] Third command
		[/example]
		[note]
		Reminder: copy the example above to a text file
		and then use /[cmd]parse[/cmd] &lt;filename&gt;
		[/note]
		In this case the command block has no special meaning
		other than making the code more readable, but command blocks
		will be useful later (see [cmd]if[/cmd],[cmd]while[/cmd]...).
		[note]
		Unlike in C or C++, the braces do [b]not[/b] automatically define a variable scope
		(with few exceptions to this rule ... just to complicate the things a bit more).
		You will recall this last assertion later, when reading about [doc:data_structures]data structures[/doc].
		[/note]
		[big]Comments[/big]
		[p]
		KVIrc supports comments in command sequences.[br]
		A comment starts with the character '#' and terminates with a newline.
		You can start a comment anywhere a command can start.
		[example]
			# This is a comment, it occupies the whole line
			[cmd]echo[/cmd] After the comment!; # This is an end-line comment
		[/example]
		You can't escape newline characters in this case.
		(or better: escape characters have no meaning in comments...
		maybe one day I'll implement it).[br]
		Starting from version 3.0.0, KVIrc supports also C++ single line and C multi-line comments.[br]
		A C++ comment starts with two slashes [b]//[/b] and terminates with a newline.
		A multi-line C comment starts with [b]/ *[/b] and ends at the first [b]* /[/b] encountered.
		Since KVIrc has no pre-processor, the C/C++ comments usually can't be placed in the middle of a command:
		they must start where a command would start and end before the begin of another command.
		[big]Indentation[/big]
		You [b]should[/b] use spaces or [b]tabs[/b] to [b]indent[/b] your code. Note that the [b]should[/b]
		word is written in bold characters: I mean that you really should indent your code.
		Indenting helps both you (the script writer) and the reader (any other user that will
		read your script). A good indenting practice is the first step to become a great programmer :)
		[note]
		Please note that the command parameters should be separated by
		space characters (ASCII 32). Tabs are not granted to work as parameter separators.[br]
		[/note]
		[example]
			{
				&lt;tab&gt;[cmd]echo[/cmd] Indented command
				&lt;tab&gt;{
				&lt;tab&gt;&lt;tab&gt;# Comment
				&lt;tab&gt;&lt;tab&gt;[cmd]echo[/cmd] Really Really long indented \
				&lt;tab&gt;&lt;tab&gt;&lt;tab&gt;command
				&lt;tab&gt;}
			}
		[/example]
		Tabs behave better than spaces as indentation characters since other users can
		adjust the tab size to match their taste. I personally prefer 4 character tabs
		while most text/code editors usually come with 8 characters as default.
		[big]And now?[/big]
		[br]
		You're now ready to really start experimenting with KVS. You can take
		a look at the [doc:commands]command index[/doc] and start trying to use them
		while keeping in mind the rules described in this document.
		The next suggested lecture is the documentation about [doc:kvs_aliasesandfunctions]the aliases and the functions[/doc].
		Have fun :)
		[br]
*/

/*
	@doc: kvs_aliasesandfunctions
	@type:
		language
	@keyterms:
		aliases, functions
	@title:
		KVS Functions and aliases
	@short:
		KVS Functions and aliases
	@body:
		[big]Introduction[/big]
		Since you're here, you should already have read about the [doc:kvs_basicconcepts]KVS basic concepts[/doc]
		and have visited the [doc:commands]command index[/doc]. If you feel ready to take the next step
		then read on.
		[big]Functions[/big]
		KVS has many internal [doc]functions[/doc] that can be used as command parameters.[br]
		[b]All the function names start with a literal [b]$[/b] character.[/b]
		[example]
			[cmd]echo[/cmd] This window caption is [fnc]$window.caption[/fnc]
		[/example]
		The [fnc]$window.caption[/fnc] [doc:functions]function[/doc]
		is evaluated before the command executes,
		and it is changed into the current window caption text.
		The [doc]functions[/doc] can be used also as switch parameters.
		[example]
			[cmd]echo[/cmd] -w = [fnc]$window[/fnc] This text will be surely \
			&nbsp; &nbsp; printed in the current window
		[/example]
		The -w switch allows to redirect the echo text to a specified window --- in this
		case the one that you are typing in.[br]
		[i](Surprise: in this case the -w switch is useless,
		since echo prints text to the current window by default...
		but it will work correctly. :)[/i]
		[br]
		Normal function names can be made of [i]anycase[/i] letters, digits and underscores,
		with the restriction that the first character is not a digit.[br]
		Some kind of functions can contain a dot ([b].[/b]) character inside the name
		and these are assumed to be module references (see [doc:modules]the modules documentation[/doc]).[br]
		By now we have seen only simple functions, but there's more...[br]
		The functions can accept parameters; the general syntax for a function call is:[br]
		[b]$<function name>['('<parameter_list>')'][/b][br]
		where <parameter_list> is a list of comma separated parameters,
		eventually empty.
		[example]
			[cmd]echo[/cmd] The word 'neural' is [fnc]$str.len[/fnc](neural) characters long
		[/example]
		The function [fnc]$str.len[/fnc] accepts a single parameter and returns the
		length in characters of the parameter string. The returned value is always
		a string: in this case it can be also interpreted as a number.[br]
		When passing an empty list you can avoid the parenthesis.
		(And you have found the [i]simple[/i] functions shown above).
		So the following two calls are equal:
		[example]
			[cmd]echo[/cmd] [fnc]$window.caption[/fnc]
			[cmd]echo[/cmd] [fnc]$window.caption()[/fnc]
		[/example]
		If you want to pass an [i]empty[/i] string as the first parameter you have to use
		the following syntax:
		[example]
			[cmd]echo[/cmd] [fnc]$str.len[/fnc]("")
		[/example]
		Obviously a function is valid as a function parameter.
		[example]
			[cmd]echo[/cmd] [fnc]$str.len[/fnc]([fnc]$window.caption[/fnc])
		[/example]
		If you want to place a literal '(' or ')' in the function parameters
		you must escape it.
		A special case for when you want to use 'matching' parentheses:
		an opened '(' corresponds to a closed ')'.
		In this case you can omit the 'escape' character.
		[example]
			[cmd]echo[/cmd] The length of '(a+b)' is : [fnc]$str.len[/fnc]( (a+b) )
		[/example]
		This is useful for algebraic and boolean expressions, like the ones
		accepted by the special function $() (see next paragraphs).
		[big]Aliases[/big]
		An alias is a user defined command. It can be used to rename the builtin KVIrc commands or functions,
		to automate complex tasks or as a means for structured programming.
		Aliases can be created or destroyed by using the scriptcenter (graphic interface)
		or from the commandline (or script) by using the [cmd]alias[/cmd] command.
		Once created, an alias remains stored permanently in the KVIrc configuration files
		until it is explicitly deleted.
		A couple of examples will make the things clear.
		join is a really commonly used command - it might be a good idea to rename it to
		simply [b]j[/b] .. just to type it faster.
		Nothing easier in KVIrc: just try this commandline:
		[example]
			[cmd]alias[/cmd](j){ [cmd]join[/cmd] $0-; };
		[/example]
		This will create the alias [b]j[/b]. From this moment you can use /j as if it was a normal command.
		[example]
			j #kvirc
		[/example]
		You may have noticed the strange $0- function in the alias body - it stands for
		[i]all parameters passed to the alias[/i]. This means that when you call
		[example]
			j #kvirc testpassword
		[/example]
		then both the parameters (#kvirc and testpassword) are passed to the join command.
		The $N functions are special functions that return the positional parameters passed
		to the current script context. In an alias the script context is the script body and
		it is the alias caller that generates the parameters.
		$N (where N is a digit) returns the (N-1)-th positional parameter passed by the caller.
		It returns the parameter numbered N-1 and not N since the parameters are indexed starting
		from zero ($0 is the first parameter!).
		$N-M returns the parameters from (N-1)-th to the (M-1)-th (a parameter range) and $N- returns
		all the parameters from (N-1)-th to the last one. In the example above $0- stands for
		all the parameters starting from the first one.
		[br]
		To remove an alias, use the alias command again with an empty body:
		[example]
			[cmd]alias[/cmd](j){}
		[/example]
		This will remove the alias [b]j[/b] defined above.
		[br]
		A common task in channel management is the kick & ban action.
		You first ban a user from the channel and then eventually kick him
		(obviously assuming that he is actually on the channel).
		This involves using two commands - ban and then kick.
		It could be a nice idea to have a single [b]kb[/b] command to perform this action.
		Well...easy:
		[example]
			[cmd]alias[/cmd](kb){ [cmd]ban[/cmd] $0; [cmd]kick[/cmd] $0-; };
		[/example]
		This adds the [b]kb[/b] alias - it can be called as a normal command:
		[example]
			kb spammer You're not welcome here!
		[/example]
		This will first execute [i]ban spammer[/i] and then [i]kick spammer with; You're not welcome here[/i].
		Our kb is a really simple example... it doesn't check for the validity of the parameters -
		the server will warn us if the parameters passed to kb were empty.
		[br]
		The alias can be modified at any time by reusing the alias command.
		Let's make our [b]kb[/b] a bit more intelligent and add a check for the parameters.
		TIP: It is a good idea to write the following examples in a text file and then use /parse <filename> to execute it.
		[example]
			[cmd]alias[/cmd](kb)
			{
				[cmd]if[/cmd]("$0" == "")
				{
					[cmd]echo[/cmd] "Usage: /kb <nickname> <kick reason>"
					[cmd]return[/cmd]
				}
				[cmd]ban[/cmd] $0
				%reason = $1-
				[cmd]if[/cmd]("%reason" == "")%reason = "You're not welcome here!"
				[cmd]kick[/cmd] $0 %reason
			}
		[/example]
		The example above will first check the validity of the <nickname> passed to kb -
		if no nickname was passed, it will warn the user and stop.
		The next step will be the [i]ban <nickname>[/i] call. Another enhancement is the [i]default reason[/i] -
		we first assign the remaining parameters ($1- means [i]from $1 to the end[/i]) to a temporary variable,
		and if the variable is empty, a default kick reason is assigned.
		Finally the [i]kick <nickname> <reason>[/i] will be executed.
		Get used to looking at the single command documentation pages, they will give
		you the hints necessary to fully understand the above piece of code.
		[br]
		Aliases can be used as a means for structured programming.
		In large scripts you will [b]surely[/b] have [i]common tasks[/i] to perform (like having
		specially-colored output or calculating a value from a set of other values)...
		Aliases are the way of writing the common tasks - they are equivalent to the [i]procedures[/i]
		or [i]functions[/i] in many high-level programming languages.
		The alias as a procedure (subroutine or sub-task) has been shown in the [b]kb[/b] example above -
		it might be commonly called from more complex scripts or other aliases in the case that a
		kick & ban action is needed.
		[br]
		Aliases can be used also as functions.
		Assume that you often need to calculate the sum of three numbers - a function-alias is the way.
		[example]
			[cmd]alias[/cmd](sum3){ [cmd]return[/cmd] $($0 + $1 + $2); };
		[/example]
		This will add the alias [i]sum3[/i] and make it available both as a command and a function.
		The [i]return[/i] command sets the return value of a sequence of commands
		(an alias is a sequence of commands... remember?) and terminates the execution (by returning
		the control to the caller).
		So return $($0 + $1 + $2); will set the return value of the alias to the value
		computed by $($0 + $1 + $2), which is the sum of the first three parameters passed.
		You will then use it in the following way:
		[example]
			...
			%myfirstsum = $sum3(%somevalue,%someothervalue,4)
			%anothersum = $sum3(12,%somevalue,%anothervalue)
			...
		[/example]
		Oops! I've used some variables without actually explaining them, please forgive me and read on.
		This example is again really simple, but you might have more complex function-aliases.
		The function-aliases are also normal aliases... you can use it as a command:
		[example]
			/sum3 1 2 3
		[/example]
		The above is a perfectly valid call, however there will be no visible results
		(because a command call implies ignoring the return value.
		In fact there is no difference at all between function-aliases and normal-aliases -
		the caller makes the difference. By calling an alias as a command, the return value
		just disappears into hyperspace, however if you call an alias as a function, the return value
		is propagated (and in fact [i]used[/i]).
		There are some [i]nice[/i] exceptions to this rule...but you don't need to care about it, for now.
		If return is not called inside an alias body, the return value will be set to [fnc]$null[/fnc].
		[br]
		Aliases can accept switches just like any other command. The [fnc]$sw[/fnc] is there
		exactly for that purpose. Check it out.
		[big]Special functions[/big]
		We have already seen the positional parameter functions.
		The functions of type [b]$N[-[M]][/b] (where N and M are positive
		numbers starting from 0 and N < M) evaluate to the sequence of
		[b]positional parameters[/b] from Nth to Mth.[br]
		If M is omitted, the function evaluate to the sequence of [b]positional
		parameters[/b] from Nth to the last one. If the whole -M block is omitted
		the function evaluate to the Nth positional parameter.
		We will discover more on the [b]positional parameters[/b] when talking
		of aliases and events.
		[example]
			$0 evaluates to the 1st positional parameter
			$0-4 evaluates to the parameters from first to 5th
			$41- evaluates to the parameters from 41st to the last available
		[/example]
		The function [b]$#[/b] evaluates to the number of positional parameters available.
		The [b]positional parameter[/b] functions do not accept parameters.[br]
		The special function [b]$(<expression>)[/b], called the
		[i][doc:expressioneval]Expression evaluation identifier[/doc][/i], returns the result
		of the evaluation of the <expression>. In previous versions of KVIrc this
		function was called $calc().
		[example]
			[cmd]echo[/cmd] $(2 + (3 ^ 7) <= 1 * (3 && 2))
		[/example]
		The special function [b]${<command sequence>}[/b] evaluates to the
		return value of the <command sequence>.[br]
		The special function [b]$$[/b] evaluates to the current object ID,
		but it is too early to explain it here...
*/

/*
	@doc: command_rebinding
	@type:
		language
	@keyterms:
		Rebinding commands to another window
	@title:
		Standard rebinding switch
	@short:
		Standard rebinding switch
	@syntax:
		<command> -r=<window_id> <parameters>
	@body:
		The -r switch is standardized along all the commands. It rebinds a command
		to the windows specified by <window_id>. It is useful to launch commands
		in windows that are not the current one. For example, you might want to
		say something in a specific channel while processing an event bound to
		a console, or say something in all the channels bound to the current IRC context.
		The examples below will make everything clear.
	@examples:
		[example]
		[comment]# Run a command in the console of the current IRC context[/comment]
		[cmd]echo[/cmd] -r=$console This command is executed in the console ($window.caption)
		[comment]# Say something to all the channels of the current IRC context[/comment]
		[cmd]foreach[/cmd](%w,[fnc]$window.list[/fnc](channel))[cmd]say[/cmd] -r=%w Hi ppl on [fnc]$chan.name[/fnc]
		[/example]
*/

/*
	@doc: window_naming_conventions
	@type:
		language
	@title:
		Window naming conventions
	@keyterms:
		IRC context, window ID, frame window, connection ID
	@short:
		KVIrc window structure and the window naming conventions
	@body:
		[big]Introduction[/big]
		Starting from the release 3.0.0, KVIrc window structure has
		grown in complexity. Older releases allowed one connection
		per [i]frame window[/i] and thus had a dedicated command parser
		for each connection. Finding a window in that scenario
		was quite easy: it was enough to designate it by [i]name[/i]
		(that was exactly the text displayed in the window caption).
		It was sufficient to have an [i]unique[/i] name for ever window;
		condition that was granted by the underlying IRC protocol
		and by the KVIrc core design.[br]
		In this version, the unique window names are impossible to be granted.
		[big]Scenario[/big]
		The command parser is now [i]global[/i] to the application.
		There can be two or more consoles in each frame and the user
		is able to join the same channel with two different nicknames
		using two separate connections.
		[ul]
		[li]
		Application (Unique command parser)
			[ul]
			[li]
			Frame X
				[ul]
				[li]
				Console M (IRC context)
					[ul]
					[li]Channel windows[/li]
					[li]Query windows[/li]
					[li]Other connection related windows[/li]
					[/ul]
				[/li]
				[li]
				Console N (IRC context)
					[ul]
					[li]Channel windows[/li]
					[li]Query windows[/li]
					[li]Other connection related windows[/li]
					[/ul]
				[/li]
				[li]
				Other windows
				[/li]
				[li]
				...
				[/li]
				[/ul]
			[/li]
			[li]
			Frame Y
				[ul]
				[li]
				Console O (IRC context)
					[ul]
					[li]Channel windows[/li]
					[li]Query windows[/li]
					[li]Other connection related windows[/li]
					[/ul]
				[/li]
				[li]
				Console P (IRC context)
					[ul]
					[li]Channel windows[/li]
					[li]Query windows[/li]
					[li]Other connection related windows[/li]
					[/ul]
				[/li]
				[li]
				Other windows
				[/li]
				[li]
				...
				[/li]
				[/ul]
			[/li]
			[li]
			...
			[/li]
			[/ul]
		[/li]
		[/ul]
		[br]
		A naming convention has become necessary to resolve ambiguities.
		[big]Basic assumptions[/big]
		Every KVIrc window has four main properties:[br]
		-[b]an unique numeric identifier[/b][br]
		-[b]the logical name[/b][br]
		-[b]the type identifier[/b][br]
		-[b]the caption text[/b][br]
		The [b]numeric identifier[/b] is unique to the whole application,
		and is the one returned by the [fnc]$window[/fnc] function.[br]
		The identifier is assigned by KVIrc when the window is created
		and is not changed until the window is destroyed.
		This identifier will be referred as [b]window ID[/b].[br]
		The [b]logical name[/b] is a property of some kind of windows.
		It usually corresponds to the first part of the window caption.
		For example, for channel windows it is the channel name, for
		queries it is the list of the targets. For some other windows
		the logical name corresponds to the caption text. This will be discussed later.[br]
		The [b]type identifier[/b] describes the properties of a certain window.
		For channel windows the type identifier is [i]channel[/i], for query windows is [i]query[/i],
		for console windows it is [i]console[/i], etc.
		[big]IRC Contexts[/big]
		The KVIrc frame windows are numbered starting from 0 and named
		[i]frame_<number>[/i]. Each frame can contain an unlimited number of consoles.[br]
		Each console is bound to an [b]IRC context[/b]. (The part [i]is bound to[/i] could
		be substituted by [i]defines[/i] or [i]is contained in[/i]).[br]
		[i]An [b]IRC context[/b] is a set of resources that can deal with a single
		IRC connection.[/i][br]
		The association between an [b]IRC context[/b]
		and a console is bijective: each [b]IRC context[/b] is associated
		to a single console window.[br]
		An [b]IRC context[/b] can be in connected or not-connected state.
		When in connected state, it contains a set of windows beside the console:
		mainly channels and query windows.
		The channels and query windows can exist [b]only[/b] if the associated
		[b]IRC context[/b] exists.[br]
		Channels and queries have unique names inside a connection so
		there is no way to confuse it. (Theoretically there can
		be more than one query window with the same name, but in fact
		all the windows refer to the same target so they are instances
		of the same resource).
		All this creates a sort of namespace: the channels and queries can be identified
		as [i]bound[/i] to a specific [b]IRC context[/b].[br]
		An [b]IRC context[/b] can [i]contain[/i] other windows, such as the [i]sockets[/i]
		window or the [i]list[/i] window. KVIrc takes care of making them
		unique inside the [b]IRC context[/b] namespace.[br]
		Each [b]IRC context[/b] has its own unique [b]IRC context ID[/b] (see [fnc]$context[/fnc]).[br]
		Since to a single [b]IRC context[/b] may correspond only a single IRC connection,
		when in connected state, the [b]IRC context[/b] may be referred also as [b]connection[/b]
		or [b]connection context[/b], and the associated [b]IRC context Id[/b] can be
		referred as [b]connection ID[/b] or [b]connection context ID[/b].[br]
		There are classes of windows that are not bound to any [b]IRC context[/b]:
		this includes user created windows, DCC windows, browsers etc.[br]
		KVIrc will try to keep that windows with unique logical names.
		[big]How to identify a window[/big]
		So what we have until now is:
		[ul]
		[li]Each window has its own unique [b]window ID[/b]: we
		will refer windows always using this identifier.[/li]
		[li]Each window has a set of properties including:
		window type, logical name.[/li]
		[li]Subsets of windows are bound to a single [b]IRC context[/b][/li]
		[/ul]
		The simplest (but also the less significant) method of looking for
		a window is to finding it by caption.[br]
		The [fnc]$window[/fnc] function finds the first KVIrc window matching
		the [i]caption text[/i] and returns its [b]window ID[/b].[br]
		This method will likely fail when there are more windows with the same
		caption text; for this reason several specific functions
		have been added to allow finding the correct window.[br]
		The [fnc]$console[/fnc] finds a console window bound to a specified
		[b]IRC context[/b].[br]
		The [fnc]$channel[/fnc] finds a channel window matching the specified
		name and bound to a specified [b]IRC context[/b].[br]
		The [fnc]$query[/fnc] finds a query window that has a specified target
		and is bound to a specified [b]IRC context[/b].[br]
*/

/*
	@doc: connection_dependent_commands
	@type:
		language
	@title:
		Connection dependent commands
	@keyterms:
		IRC context, connection dependent commands
	@body:
		Many KVIrc commands are connection dependent:
		you need an IRC connection to successfully execute them;
		usually because some data needs to be sent to the server.
		This includes commands like [cmd]whois[/cmd],[cmd]raw[/cmd],[cmd]query[/cmd],
		[cmd]msg[/cmd],[cmd]notice[/cmd],[cmd]op[/cmd],[cmd]ctcp[/cmd]...[br]
		These commands must be executed in a window that is bound to a
		[b]connected [doc:window_naming_conventions]IRC context[/doc][/b].
		You will obviously get an error message if you try to use them in a window
		that has no associated IRC connection.[br]
		For instance: [cmd]whois[/cmd] will work only if you execute it
		in a console, channel or query window.[br]
		If you want to use these commands in a window that is not associated to
		any IRC context you may use the [doc:command_rebinding]standard -r switch[/doc].
		You can use the same switch to execute a command in an [b]IRC context[/b] that is
		not the current one.
*/

/*
	@doc: aliases
	@type:
		language
	@keyterms:
		aliases
	@title:
		Aliases
	@short:
		Aliases : user definable command sequences
	@body:
		An alias is a user defined command.  It can be used to rename the builtin KVIrc commands or functions,
		to automate complex tasks or as structured programming mean.
		Aliases can be created or destroyed by using the scriptcenter (graphic interface)
		or from the commandline (or script) by using the [cmd]alias[/cmd] command.
		Once created, an alias remains stored permanently in the KVIrc configuration files
		until it is explicitly deleted.
		A couple of examples will make the things clear.
		join is a really commonly used command. It might be a good idea to rename it to
		simply [b]j[/b] .. just to type it faster.
		Nothing easier in KVIrc: just try this commandline:
		[example]
			[cmd]alias[/cmd](j){ [cmd]join[/cmd] $0-; };
		[/example]
		[br]
		This will create the alias [b]j[/b]. From this moment you can use /j as it was a normal command.
		[example]
		j #kvirc
		[/example]
		You may have notices the strange $0- function in the alias body: it stands for
		[i]all parameters passed to the alias[/i]. This means that when you call
		[example]
			j #kvirc testpassword
		[/example]
		then both the parameters (#kvirc and testpassword) are passed to the join command.
		The $N functions are special functions that return the positional parameters passed
		to the current script context. In an alias the script context is the script body and
		it is the alias caller that generates the parameters.
		$N (where N is a digit) returns the (N-1)-th positional parameter passed by the caller.
		It returns the parameter numbered N-1 and not N since the parameters are indexed starting
		from zero ($0 is the first parameter!).
		$N-M returns the parameters from (N-1)-th to the (M-1)-th (a parameter range) and $N- returns
		all the parameters from (N-1)-th to the last one. In the example above $0- stands for
		all the parameters starting from the first one.
		[br]
		To remove an alias use again the alias command with an empty body:
		[example]
			[cmd]alias[/cmd](j){}
		[/example]
		This will remove the alias [b]j[/b] defined above.
		[br]
		A common task in channel management is the kick & ban action.
		You first ban a user from the channel and then eventually kick him
		(obviously assuming that he is actually on the channel).
		This involves using two commands: ban and then kick.
		It could be a nice idea to have a single [b]kb[/b] command to perform this action.
		Well...easy:
		[example]
			[cmd]alias[/cmd](kb){ [cmd]ban[/cmd] $0; [cmd]kick[/cmd] $0-; };
		[/example]
		This adds the [b]kb[/b] alias: it can be called as a normal command:
		[example]
			kb spammer You're not welcome here!
		[/example]
		This will first execute [i]ban spammer[/i] and then [i]kick spammer with; You're not welcome here[/i].
		Our kb is a really simple example... it doesn't check for the validity of the parameters:
		the server will warn us if the parameters passed to kb were empty.
		[br]
		The alias can be modified at any time by re-using the alias command.
		Let's make our [b]kb[/b] a bit more intelligent and add a check for the parameters.
		TIP: It is a good idea to write the following examples in a text file and then use /parse <filename> to execute it.
		[example]
			[cmd]alias[/cmd](kb)
			{
				[cmd]if[/cmd]("$0" == "")
				{
					[cmd]echo[/cmd] "Usage: /kb <nickname> <kick reason>"
					[cmd]return[/cmd]
				}
				[cmd]ban[/cmd] $0
				%reason = $1-
				[cmd]if[/cmd]("%reason" == "")%reason = "You're not welcome here!"
				[cmd]kick[/cmd] $0 %reason
			}
		[/example]
		The example above will first check the validity of the <nickname> passed to kb:
		if no nickname was passed, it will warn the user and stop.
		The next step will be the [i]ban <nickname>[/i] call. Another enhancement is the [i]default reason[/i]:
		we first assign the remaining parameters ($1- means [i]from $1 to the end[/i]) to a temporary variable,
		if the variable is empty, a default kick reason is assigned.
		Finally the [i]kick <nickname> <reason>[/i] will be executed.
		Get used to looking at the single command documentation pages, they will give
		you the hints necessary to fully understand the above piece of code.
		[br]
		Aliases can be used as a mean for structured programming.
		In large scripts you will [b]surely[/b] have [i]common tasks[/i] to perform (like having specially
		colored output or calculating a value from a set of other values)...
		Aliases are the way of writing the common tasks: they are equivalent to the [i]procedures[/i]
		or [i]functions[/i] in many high-level programming languages.
		The alias as a procedure (subroutine or sub-task) has been shown in the [b]kb[/b] example above:
		it might be commonly called from more complex scripts or other aliases in case that a
		kick & ban action is needed.
		[br]
		The aliases can be used also as functions.
		Assume that you need really often to calculate the sum of three numbers: a function-alias is the way.
		[example]
			[cmd]alias[/cmd](sum3){ [cmd]return[/cmd] $($0 + $1 + $2); };
		[/example]
		This will add the alias [i]sum3[/i] and make it available both as a command and a function.
		The [i]return[/i] command sets the return value of a sequence of commands
		(an alias is a sequence of commands...remember?) and terminates the execution (by returning
		the control to the caller).
		So return $($0 + $1 + $2); will set the return value of the alias to the value
		computed by $($0 + $1 + $2) that actually is the sum of the first three parameters passed.
		You will then use it in the following way:
		[example]
			...
			%myfirstsum = $sum3(%somevalue,%someothervalue,4)
			%anothersum = $sum3(12,%somevalue,%anothervalue)
			...
		[/example]
		Oops! I've used some variables without actually explaining them, hehe... please forgive me and read on.
		This example is again really simple, but you might have more complex function-aliases.
		The function-aliases are also normal aliases... you can use it as a command:
		[example]
			/sum3 1 2 3
		[/example]
		Is a perfectly valid call... it's just that it will have no visible results
		(just because a command call implies ignoring the return value.
		In fact there is no difference al all between function-aliases and normal-aliases:
		the caller makes the difference: by calling an alias as a command the return value
		just disappears in hyperspace, by calling an alias as a function, the return value
		is propagated (and in fact [i]used[/i]).
		(There are some [i]nice[/i] exceptions to this rule... but you don't need to care about it, for now).
		If return is not called inside an alias body, the return value will be just a null value.
		[br]
		Aliases can accept switches just like any other command. The [fnc]$sw[/fnc] is there
		exactly for that purpose. Check it out.
		[br]
*/

/*
	@doc: kvs_addons
	@type:
		language
	@keyterms:
		addons, addon
	@title:
		The KVIrc addon system
	@short:
		Writing KVIrc addons
	@body:
		[big]Introduction[/big]
		[br]
		An addon is basically a set of KVS scripts, multimedia, documentation
		and accessory files that implement a KVIrc feature.
		It might be a simple automatic-away subsystem, a GUI newsticker or a complex
		file sharing service (commonly called [i]fserve[/i]). Addons are sometimes called [i]scripts[/i].
		In fact a KVIrc addon is usually made of more than one KVS script.[br][br]
		KVIrc has a builtin addon management system that allows the users to create,
		install, configure and uninstall features with a nice graphical interface.
		The management system allows the addons to have documentation integrated in
		the KVIrc help and to be translated in several languages.
		[big]Addon installation[/big]
		The addons are usually shipped in compressed archives (.kva). KVIrc will look
		for the installer file called [i]install.kvs[/i] and executes it when the user will
		ask for your addon to be installed. The install.kvs contains the code for the
		[b]registration[/b] of your addon and will [cmd]include[/cmd] all the other
		necessary source files.
		[big]The minimal addon[/big]
		[br]
		The smallest addon that you can write is the one that does nothing.
		[example]
			[cmd]addon.register[/cmd]("MyAddon", \
							"1.0.0", \
							"My First Addon", \
							"An addon that is really cool but does
							simply nothing", \
							"4.0.0", \
							"MyAddon_32.png")
			{
			}
		[/example]
		The code above does nothing but registers the [i]MyAddon[/i] addon.
		[br]
		The first parameter is the internal addon ID which can be used to identify
		your addon inside KVIrc. The ID must be unique: two addons that share the same
		name cannot be installed. The second parameter is the addon version. It should
		be expressed in the classic format [major].[minor].[pathlevel] or something
		really similar (in fact KVIrc just expects the version to be a string composed
		of numbers separated by dots).[br][br]
		The version is compared when an addon is installed and KVIrc complains if the user
		tries to downgrade an addon (that is to install a less recent version over a more recent one).[br]
		The third parameter is the visible name of your addon: it will be displayed to the user in the
		addon management dialog. It can contain the [fnc]$tr[/fnc] function so you
		can have it translated to several languages. The fourth parameter
		is a short description of the feature that the addon implements; it can
		contain the $tr() function too. The fifth parameter is the minimal KVIrc
		version required to run the addon. The sixth parameter is the icon to show in
		the manager: it has to be 32x32 pixel big. There are also some switches that
		can be used to fiddle a little bit more :)
		[br]
		The callback instruction that follows the registration command is the
		uninstallation code. KVIrc will invoke it when the user will ask for your
		addon to be uninstalled. Don't assume that your addon will be never
		uninstalled: sooner or later it will be. For example, when upgrading an addon
		KVIrc will first uninstall the existing version and after that install the new
		one. The uninstallation process is a very important requisite for any program
		(in any programming language). In the example above there is nothing to
		uninstall (yet) so the callback code is empty, but if you continue reading we
		will soon fill it.
		To uninstall all files which are not automatically spotted by the installation
		process, you need to write an alias which handles them.
		[example]
			alias(MyAddon::uninstall::uninstall)
			{
				...
			}
		[/example]
		[big]A typical addon layout[/big]
		[br]
		As stated above, the addons are usually shipped in a compressed archive.
		Once uncompressed, the installer will check the directory tree containing the
		addon code and all the related files.
		In order to have uniformity the installer complains if the structure is not
		respected.
		[br]
		[pre]
			[b]name-version[/b]
			 +- init.kvs
			 +- [b]src[/b]
			 |	+- source1.kvs
			 |	+- source2.kvs
			 |	\- ...
			 +- [b]locale[/b]
			 |	+- name_it.mo
			 |	+- name_de.mo
			 |	\- ...
			 +- [b]config[/b]
			 |	+- config1.kvc
			 |	+- config2.kvc
			 |	\- ...
			 +- [b]sound[/b]
			 |	+- audio1.wav
			 |	+- audio2.wav
			 |	\- ...
			 +- [b]pics[/b]
			 |	+- pic1.png
			 |	+- pic2.png
			 |	\- ...
			 +- [b]help[/b]
			  	+- [b]en[/b]
			  	|   +- index.html
			  	|   +- hints.html
			  	|   \- ...
			  	+- [b]it[/b]
			  	    +- index.html
			  	    +- hints.html
			  	    \- ...
		[/pre]
		The entries in [b]bold[/b] are directories while the other are files.
		Please note that you need all of these directories or the routine that
		automagically creates the installer will fail.
		[br]
		The toplevel directory should be named with your addon name and version.
		Use no spaces in the directory entries (this will make the things simpler for
		people that want to use your addon).
		[br]
		Hint: Remember that your addon is going to be installed on different platforms
		(at least Linux, macOS and Windows based).
		The poor windows' notepad has serious problems with reading text
		files that contain only linefeeds as line separators. Keep it in mind...
		[br]
		The [b]initialization script[/b] has to be named init.kvs and must contain all the
		routines to register your addon.
		[example]
			[comment]# Register classes[/comment]
			MyAddon::classes::register

			[comment]# Initialize events[/comment]
			MyAddon::events::init

			[comment]# Load configuration[/comment]
			MyAddon::config::load

			[comment]# Setup popups[/comment]
			defpopup("MyAddon")
			{
				item($tr("Something","MyAddon"),110)
				{
					...
				}
			}

			[comment]# Set options[/comment]
			option boolAutoAcceptDccSend 1
			option boolShowMinimizedDebugWindow 1
		[/example]
		[/p]
		[p]
		The main [b]source directory[/b] for your addon have to be named [i]src[/i] and must
		contain the implementation of the feature(s) you're going to provide.
		File names should contain the namespace of the addon, the optional subnamespace
		and the name of the feature like $addonNS_$subNS_[$subNS_[...]]$name.kvs
		[example]
			[comment]# A class which handles a database[/comment]
			MyAddon_classes_database.kvs

			[comment]# A class which handles the options of our addon in a GUI[/comment]
			MyAddon_classes_gui_options.kvs

			[comment]# A script containing some logging functions[/comment]
			MyAddon_functions_logging.kvs
		[/example]
		[br]
		The [b]locale[/b] directory should contain the *.mo files for your translations.
		The localization process of a script is explained in [doc:localization]this document[/doc].
		Your *.mo filenames should be prefixed by your addon name.
		[br]
		The [b]configuration directory[/b] [i]config[/i] should contains only the files
		which store the configuration of your addon and must end with the .kvc
		extension.
		[br]
		The [b]pics[/b] and [b]sound[/b] (if relevant) directories should contain
		your multimedia files. I's a good idea to have your pics file in PNG format
		and sound files in WAV format.
		[br]
		The [b]help[/b] directory should contain subdirectories for each language
		your help files are written in. The languages dirs should be named
		with the language code also used for the translation files (like [i]en[/i], [i]it[/i]
		etc...).
		Please note that English is the default language and KVIrc will
		fallback to the [i]en[/i] subdirectory when no other language is found around...
		[big]Some examples[/big]
		The code below is just an example of how to write a useful initialization of
		your own addon. The name of the classes refer to the ones described above.
		[example]
			[comment]# Register the classes[/comment]
			alias(MyAddon::classes::register)
			{
				[comment]# Create an array with all the classes of our addon.[/comment]
				[comment]# In this way it's easy to add or remove classes to registering routine[/comment]
				%classes[] = [fnc]$array[/fnc]( \
					MyAddon::classes::database, \
					MyAddon::classes::gui::options, \
					...
					)

				[comment]# Scan the array and register the classes[/comment]
				for(%i=0; %i < [fnc]$length[/fnc](%classes[]); %i++)
				{
					if([fnc]$classDefined[/fnc]("%classes[%i]"))
					{
						[cmd]objects.killclass[/cmd] %classes[%i]
					}
					[cmd]eval[/cmd] %classes[%i]
				}
			}

			[comment]# Initialize events[/comment]
			alias(MyAddon::events::init)
			{
				event(OnKVIrcStartup,"MyAddon")
				{
					...
					[comment]# Load the catalogue (translation) file [i]myaddon[/i] from the path provided[/comment]
					[cmd]trload[/cmd] myaddon [fnc]$file.localdir[/fnc]("locale/MyAddon")
					MyAddon::config::load
					...
				}
				event(OnChannelMessage,"MyAddon_something")
				{
				...
				}
			}

			[comment]# Load configuration[/comment]
			alias(MyAddon::config::load)
			{
				[comment]# If the class ConfHandler is not defined, register all classes we have[/comment]
				if(![fnc]$classDefined[/fnc](MyAddon::classes::ConfHandler))
				{
					MyAddon::classes::register
				}

				[comment]# Sets some variables[/comment]
				%MyAddonConfig = [fnc]$new[/fnc](MyAddon::classes::ConfHandler)
				%MyAddonConfigPath = [fnc]$file.localdir[/fnc](config/scripts/MyAddon)

				[comment]# Open the configuration file and sets the section general[/comment]
				%c = [fnc]$config.open[/fnc](%MyAddonConfigPath/MyAddon.kvc,"r")
				[cmd]config.setsection[/cmd] %c general

				[comment]# Store the value of the key "Key" in the global variable %Key[/comment]
				%Key = [fnc]$config.read[/fnc](%c,"Key",2)
				...
			}
		[/example]
		[big]The help and configuration callbacks[/big]
		[br]
		Each addon can have a help and a configuration callback. These are set
		respectively by [cmd]addon.sethelpcallback[/cmd] and [cmd]addon.setconfigurecallback[/cmd].
		[br]
		The help callback will be invoked by KVIrc when the user will ask help for your addon (mainly
		from the addon management dialog, but not necessarily). It should call [cmd]help.open[/cmd]
		with the name of your documentation index html file (it should be relative
		to the help language directory: help.open myaddon/index.html will automatically
		lookup the right language). If you provide no help callback, the buttons
		for requesting help will be simply disabled. (A good an relatively complex addon
		*should* have at least a minimal help file explaining the features).
		[br]
		The configuration callback will be invoked when the user will try to configure
		your addon from the addon management dialog. This callback is useful
		mainly for more complex graphical scripts that can show up a dialog
		that allows configuring all of the addon features. To use this callback
		you will probably need some object scripting.
		[big]The real addon work[/big]
		[br]
		The real addon work is done by the scripts contained in the src directory.
		They will likely add aliases (maybe in a nice namespace named against your addon),
		register event handlers, create actions, timers, toolbars and object classes.
		You should install all of this stuff from your addon source files.
		Remember that your source files will [b]not[/b] be parsed every time KVIrc starts up:
		your stuff must be registered in KVIrc and be able to startup itself, if needed.
		Remember that you must clean up [b]everything[/b] in your uninstallation callback.
		This means that you must remove the aliases, unregister the event handlers,
		destroy the actions, kill the timers and the object classes you've created.
		Be a clean coder :)
		[big]Where to start[/big]
		[br]
		It is a good idea to start in the KVIrc scripts GitHub repository https://github.com/kvirc/kvirc-scripts. There are surely
		several addons to look at. Pick one that seems simple and analyze its layout and code
		(wow... amazing free software!). It will be easier to do than it was to explain it :D[br]
		Have fun! :)
		[br]
*/

/*
	@doc: kvs_codingtips
	@type:
		generic
	@title:
		Coding tips
	@keyterms:
		indentation,indent,readability
	@short:
		Generic coding tips for scripters (and not only)
	@body:
		Here comes a small list of [i]coding tips[/i].[br]
		These apply to programming in general, not only to KVIrc scripting.[br]
		[br]
		1. [b]Comment your code[/b][br]
		A well commented code is easy to maintain, and easy to read by others.[br]
		[br]
		2. [b]Indent your code[/b][br]
		Indentation increases the code readability; this is again for you and
		other developers that will be going to read your code.[br]
		[br]
		3. [b]Use TABS to indent your code[/b][br]
		...and use [b]only tabs[/b] to indent.[br]
		Tabs are better than space since most code editors allow you
		to set the tab since and thus to have the indentation steps smaller or bigger.[br]
		This is really important since the indentation size is really a matter of personal taste.[br]
		Mixing spaces and tabs is Evil (tm), since it makes the code look really
		ugly in editors that have the tab size different than yours; in some cases the
		code gets really unreadable.[br]
		[br]
		4. [b]Use descriptive variable names[/b][br]
		Using 'foo' as variable name implies tracing its semantic the next
		time that you're going to read the code that uses it.[br]
		This is really annoying and time-consuming, especially if the project
		is getting large.[br]
		Obviously using [i]thisIsACounterVariable[/i] as name for a simple counter
		is also a suicide.[br]
		A good convention on variable names can speed up writing, debugging and maintaining code.[br]
		Encoding the type of the variable in the variable name might be also a good idea,
		but this is a matter of taste; personally I feel really well with that.[br]
		Just as example, here go my fundamental convention rules for C++:[br]
		[br]
			- The type of the variable is encoded at the beginning of the variable name:[br]
		[br]
			- b prefix for the boolean variables[br]
			- i prefix for signed integers[br]
			- u prefix for unsigned integers[br]
			- f and d prefixes for floating point stuff[br]
			- sz prefix for strings (this is rather for string classes)[br]
			- ...[br]
		[br]
			- Pointers have a [b]p[/b] prefix prepended[br]
			- Global variables start with a [b]g_[/b] prefix[br]
			- Member variables start with a [b]m_[/b] prefix[br]
			- Exception comes for local variables with obvious semantics[br]
		[br]
			- i,j,k,l for local loop counters[br]
			- [i]aux[/i] and [i]tmp[/i] for local obvious short-term temporary variables[br]
		[br]
		So actually by [b]only[/b] reading [i]g_pszQuitMessage[/i] I know that this is a global pointer to a string variable
		containing a quit message. :)[br]
		[/p]
*/

// FIXME: #warning "FINISH THE SYNTACTIC RULES DOC"

/*
	@doc: syntactic_rules
	@type:
		language
	@keyterms:
		productions
	@title:
		Syntactic rules
	@short:
		Syntactic rules of the KVIrc scripting language
	@body:
		In the following table you can find a good part of the
		KVIrc scripting language syntactic rules.[br]
		[br]
		<entity> indicates an [b]entity that can appear exactly one time[/b].[br]
		[<entity>] indicates an [b]optional entity[/b].[br]
		{<entity>} indicates an [b]entity that can appear one or more times[/b].[br]
		'entity' indicates a [b]literal entity[/b]: written exactly as it is.[br]
		<entity1>|<entity2> indicates mutually exclusive choices.[br]
		The mutually exclusive choices are often separated in two or more
		rules (productions), to improve readability.
		[table]
		[tr]
		[td]<command buffer>[/td]
		[td][<whitespace>][<command block>]{<command buffer>}[/td]
		[/tr]
		[tr]
		[td]<command buffer>[/td]
		[td][<whitespace>][<single command>]{<command buffer>}[/td]
		[/tr]
		[tr]
		[td]<whitespace>[/td]
		[td]{<space>|<tab>|<newline>}['\'<newline>][<whitespace>][/td]
		[/tr]
		[tr]
		[td]<space>[/td]
		[td]' '['\'<newline>][<space>] (ASCII space character)[/td]
		[/tr]
		[tr]
		[td]<tab>[/td]
		[td]'\t' (ASCII horizontal tabulation character)[/td]
		[/tr]
		[tr]
		[td]<newline>[/td]
		[td]'\n' (ASCII line feed (LF) character)[/td]
		[/tr]
		[tr]
		[td]<command block>[/td]
		[td]'{' <command buffer>[<whitespace>] '}'[/td]
		[/tr]
		[tr]
		[td]<single command>[/td]
		[td]<comment>[/td]
		[/tr]
		[tr]
		[td]<single command>[/td]
		[td]<lvalue command> <command terminator>[/td]
		[/tr]
		[tr]
		[td]<single command>[/td]
		[td]<rvalue command> <command terminator>[/td]
		[/tr]
		[tr]
		[td]<comment>[/td]
		[td]'#' {<non comment terminator>} <comment terminator>[/td]
		[/tr]
		[tr]
		[td]<comment terminator>[/td]
		[td]<newline> | <end of string>[/td]
		[/tr]
		[tr]
		[td]<end of string>[/td]
		[td]No character (internally ASCII character 0)[/td]
		[/tr]
		[tr]
		[td]<command terminator>[/td]
		[td]<newline> | <end of string> | ';'[/td]
		[/tr]
		[tr]
		[td]<non comment-terminator>[/td]
		[td]Any ASCII character except <newline> and <end of string>[/td]
		[/tr]
		[tr]
		[td]<simple command>[/td]
		[td][<module name>'.']<command name>[<switch list>]{<space>}<command dependent part>[/td]
		[/tr]
		[tr]
		[td]<lvalue command>[/td]
		[td]<variable>[<space>]<operation>[/td]
		[/tr]
		[tr]
		[td]<lvalue command>[/td]
		[td]<variable>'->'<object command>[/td]
		[/tr]
		[tr]
		[td]<lvalue command>[/td]
		[td]<identifier>'->'<object command>[/td]
		[/tr]
		[tr]
		[td]<operation>[/td]
		[td]<one op operator>[/td]
		[/tr]
		[tr]
		[td]<operation>[/td]
		[td]<two op operator>[<space>]<param string>[/td]
		[/tr]
		[tr]
		[td]<switch list>[/td]
		[td]{<space>}'-'<alpha char>[{<space>}'='<single parameter>][<switch list>][/td]
		[/tr]
		[tr]
		[td]<command name>[/td]
		[td]<alphanumeric char>{<alphanumeric char>}[/td]
		[/tr]
		[tr]
		[td]<module name>[/td]
		[td]<alphanumeric char>{<alphanumeric char>}[/td]
		[/tr]
		[tr]
		[td]<alphanumeric char>[/td]
		[td]Ascii characters 'A' to 'Z', 'a' to 'z', '0' to '9' and '_'[/td]
		[/tr]
		[tr]
		[td]<variable>[/td]
		[td]<global variable> | <local variable>[/td]
		[/tr]
		[tr]
		[td]<global variable>[/td]
		[td]'%' <uppercase letter> [<alphanumeric char>]['['<param string>']'][/td]
		[/tr]
		[tr]
		[td]<local variable>[/td]
		[td]'%' <lowercase letter> [<alphanumeric char>]['['<param string>']'][/td]
		[/tr]
		[tr]
		[td]<param string>[/td]
		[td][<single parameter>][<space>[<param string>]][/td]
		[/tr]
		[tr]
		[td]<single parameter>[/td]
		[td]<variable> | <identifier> | <nonterminator token> | <string>[/td]
		[/tr]
		[tr]
		[td]<nonterminator token>[/td]
		[td]<nonterminator char>['\'<newline><nonterminator char>][/td]
		[/tr]
		[tr]
		[td]<nonterminator char>[/td]
		[td]Any ascii character except <space> and <command terminator>[/td]
		[/tr]
		[tr]
		[td]<command dependent part>[/td]
		[td]Production in each command help page[/td]
		[/tr]
		[/table]
		To be continued...
*/

/*
	@doc: kvs_datatypes
	@type:
		language
	@keyterms:
		global variable, global variables, local variable, local variables,
		variables, variable, array, hash, dictionary, global variables, local variables, variable evaluation,
		associative arrays, scalars, data types, percent sign and extended scope
	@title:
		Variables and Data types
	@short:
		All about the KVS variable and datatype management
	@body:
		[title]Basic syntax[/title]
		[br]
		A variable identifier is composed by a '%' (percent) sign followed
		by a sequence of letters, digits or underscores.
		Examples of valid variable names are:
		[example]
			%i
			%variable
			%MyVar
			%1
			%thisisavar
			%2ndName
			%_hidden
		[/example]
		[br]
		Variables are created when you assign something to them: there is no need
		for a declaration (unlike other languages such as C/C++, Java or VB).
		[example]
			[comment]# create a variable named %X by assigning the value 10 to it[/comment]
			%X = 10
			[comment]# use the variable[/comment]
			echo "The value of X is" %X
		[/example]
		[title]Local and global variables[/title]
		[br]
		Variables can be local or global.
		Local variables preserve their contents only inside the [b]scope[/b] of a single script.
		Global variables are shared between all the scripts and preserve their contents
		until they are explicitly unset or until KVIrc quits.
		[br]
		Local variables start with a [b]lowercase letter[/b] while the global ones with an [b]uppercase letter[/b].
		[example]
			%var = 10; [comment]# this is a local variable[/comment]
			%Var = 10; [comment]# this is a global variable[/comment]
		[/example]
		[br]
		You can also force a variable that start with a lowercase letter to be global
		by pre-declaring it with the [cmd]global[/cmd] keyword.
		[example]
			[comment]# copy this script to a file and run /[cmd]parse[/cmd] <filename>[/comment]
			global %a
			%a = "The contents of the variable a"
			%b = "The contents of the variable b"
			[comment]# %a is a global variable now : all the other scripts can see its value[/comment]
			[comment]# %b is a local variable and no other scripts can see its value[/comment]
		[/example]
		[br]
		If you have executed the example above from a file (by the means of [cmd]parse[/cmd])
		then now you can type
		[example]
			[cmd]echo[/cmd] %a
		[/example]
		[br]
		in the commandline to see the contents of the variable %a.
		If you also try
		[example]
			[cmd]echo[/cmd] %b
		[/example]
		[br]
		you will see nothing printed since %b was local to the parsed script.
		[title]Data types[/title]
		[br]
		KVS has three main categories of data types: scalars, arrays and associative
		arrays (also known as dictionaries or hashes).
		[subtitle]Scalars[/subtitle]
		[br]
		The scalars are simple variables containing a single value (a string or an integer).
		[example]
			[comment]# %a is a scalar variable[/comment]
			%a = "This is a string"
			[cmd]echo[/cmd] %a
			%a = 24.5
			[cmd]echo[/cmd] %a
		[/example]
		[subtitle]Arrays[/subtitle]
		[br]
		Arrays are collections of items indexed by integers. The array items
		are selected by placing the index in square brackets just after the array name.
		[example]
			%arrayName[index]
		[/example]
		[br]
		An easy way to create an array is to use the [fnc]$array[/fnc] function.
		[example]
			%a = $array("element1","element2","element3"); [comment]# Create an array with 3 items[/comment]
			[cmd]for[/cmd](%i=0;%i<3;%i++)
			{
				echo %a[%i]; [comment]# Accessing the %i'th element of the array[/comment]
			}
		[/example]
		[br]
		Note that in the example above %a refers to the whole array while %a[%i] refers
		to one of its elements, in particular the one with index %i.
		You also create an array by explicitly assigning to one of its elements:
		[example]
			%a[9] = "This is an array element";
		[/example]
		[br]
		Array indexes are zero-based so in the example above you have created an array
		with 10 items. You can find out an array's length with the [fnc]$length[/fnc]() function.
		[example]
			%a[9] = "This is an array element";
			echo $length(%a)
		[/example]
		[br]
		Be aware that by making such an assignment you implicitly consume some memory for
		all the preceding array items (even if they are unset). This means that
		a simple instruction like the following may eat a huge amount of memory at once:
		[example]
			%a[1000000] = "An array element faaaaaar away...";
			echo $length(%a)
		[/example]
		[note]
		[big]Food for thought:[/big]
		KVIrc allocates a pointer for each item in the array. The pointer is
		empty when the item is unset and points to an additional block
		of memory when the item is set. The size of a pointer is platform
		dependent: on the platforms supported by KVIrc it's either 32 or 64-bit.
		The size of the additional block depends both on the platform
		and on the contents of the item... it's average value may
		be around 16 bytes. The array size is determined by the last SET element index.
		All this this means that in the worst case (64 bit assumption) an array in
		that the highest indexed item set is N eats up at least N*8+16 bytes of memory.
		[/note]
		Besides the traditional indexed looping method you
		can also use the [cmd]foreach[/cmd] command to iterate the items of an array.
		Be aware that [cmd]foreach[/cmd] will [b]not[/b] iterate over unset items in the
		array unless you use the -a switch.
		[example]
			%Array[0]=Pippo
			%Array[1]=Pluto
			%Array[2]=Paperino
			%Array[5]=Prova
			[cmd]foreach[/cmd](%item,%Array)[cmd]echo[/cmd] Got Item: %item
		[/example]
		[br]
		Note that the items 3 and 4 are simply skipped.
		[subtitle]Hashes[/subtitle]
		[br]
		The hashes are collections of items indexed by strings: the word [i]hash[/i]
		is in fact a shortcut for [i]hashtable[/i]. In literature hashes are also called
		[i]associative arrays[/i], [i]dictionaries[/i] or [i]key-value pair sets[i].
		The hash items are selected by placing the key in curly brackets
		just after the hash name.
		[example]
			%hashName{key}
		[/example]
		[br]
		An easy way to create a hash is to use the [fnc]$hash[/fnc] function.
		[/p]
		[example]
			%a = $hash("key1","value1","key2","value2","key3","value3")
			[cmd]foreach[/cmd](%key,[fnc]$keys[/fnc](%a))
			{
				echo "KEY:" %key "VALUE:" %a{%key};
			}
		[/example]
		[br]
		Note that in the example above %a refers to the whole hash while %a{%i} refers
		to one of its elements, in particular the one with the key %key.
		You also create a hash by explicitly assigning to one of its elements:
		[example]
			%a{"MyKey"} = "MyValue"
		[/example]
		[br]
		You may have already noticed that the [fnc]$keys[/fnc]() function returns
		the array of the hash keys: it is useful to iterate over the hash items.
		[title]Mutability of variables[/title]
		[br]
		KVS is not strictly typed: any variable can assume different type identities at different times,
		even in the same script.
		[example]
			[comment]# %a is a scalar[/comment]
			%a = "This is a string"
			[comment]# %a becomes an array with 3 elements[/comment]
			%a = $array("element1","element2","element3");
			[comment]# %a becomes a hash with two values[/comment]
			%a = $hash("key1","value1","key2","value2");
		[/example]
		[br]
		In literature this kind of variable is called [b]variant[/b] and this is the
		term that you will find all around the documentation when an explicit
		data type is not requested.
		[br]
		Note that array and hash items are variants too. This means that you can have arrays
		of arrays, hashes of arrays of hashes and any other multidimensional combination you like.
		However remember that hash keys are strings and not variants so you can't use an array as hash key.
		[example]
			[comment]# here we eat 256 locations of memory at once :)[/comment]
			%a[16][16] = 10
			[comment]# a hash of hashes: here we eat just two memory locations[/comment]
			%a{"16"}{"16"} = 10
		[/example]
		[br]
		In most cases the KVS engine manages automatically the conversion between data types.
		For example, when you put an array in a place where a scalar is requested, KVIrc
		automatically transforms it to a scalar string by joining all the items with a comma.
		[example]
			%a = $array("element1","element2","element3");
			echo %a; [comment]# echo expects its arguments to be scalar[/comment]
		[/example]
		[br]
		Conversely, when you put a scalar in place of an array, KVIrc automatically
		transforms it to an array with a single item. In this way a function like
		[fnc]$sort[/fnc] works also with a scalar.
		[br]
		In literature the conversions between data types are called [b]casts[/b]. When
		the conversion is automatic the cast is said to be [b]implicit[/b].
		[br]
		KVS handles also the other possible implicit casts: scalar->hash,hash->scalar,array->hash,hash->array.
		Experiment with it.
		[title]More about scalars[/title]
		[br]
		Internally KVS is implicitly typed: the [i]scalar[/i] data type is in fact
		a set of types that KVIrc manages silently. The types are: integer, string, real, boolean and hobject.
		[br]
		Integers are non-floating point numbers. Their allowable range depends on the underlying
		platform integer size: usually 32 or 64 bit.
		[br]
		Reals are floating point numbers. Their allowable range and precision depends on the underlying
		platform.
		[br]
		Booleans are either true or false values. For boolean constants, use [fnc]$true[/fnc] or [fnc]$false[/fnc].
		Don't use [i]true[/i] or [i]false[/i], because the engine sees them as non-empty strings, which always evaluate as true.
		[br]
		hobject stands for Handle to Object and it is a sort of a C++ pointer.
		Detailed description of objects is in [doc:objects]this document[/doc].
		[br]
		Basically anything else fails in the [i]string[/i] category.
		[br]
		In most cases KVS manages all the conversions between data types automatically.
		For example an integer becomes a true boolean when it's non zero and a false boolean
		otherwise, a real becomes an integer by truncating it's fractional part...
		[br]
		You can find out the type of a specified variable by using the [fnc]$typeof[/fnc]() function.
		[example]
			%a = 1
			echo $typeof(%a)
			%a = 1.1
			echo $typeof(%a)
			%a = $true
			echo $typeof(%a)
			%a = "test"
			echo $typeof(%a)
		[/example]
		[br]
		There is also another subtle type of scalar called [i]nothing[/i]. It stands for an empty (unset) variable.
		[example]
			%a = $nothing
			echo $typeof(%a)
		[/example]
		[br]
		Nothing is something in between a data type and a special value for all the other data types:
		it represents absence of information.
		This may look a bit confusing but realize that all the unreferenced KVS variable are in fact of type [i]nothing[/i]:
		they just don't exist. This means that you can use [fnc]$nothing[/fnc]() to effectively
		unset a variable.
		[br]
		Again, when possible, the conversion between nothing and the other data types is
		performed automatically. Nothing becomes an empty string, a null object handle or an empty array.
		[title]Explicit casts[/title]
		[br]
		You can make explicit conversions between some data types by using the casting functions.
		[fnc]$integer[/fnc]() will attempt to convert the variant parameter to an integer, [fnc]$real[/fnc]()
		will cast to a floating point value, [fnc]$boolean[/fnc]() will convert to a
		true/false value, [fnc]$string[/fnc]() will explicitly convert to a string,
		[fnc]$array[/fnc]() will convert to an array and [fnc]$hash[/fnc] will return
		a dictionary. By assigning the special [fnc]$nothing[/fnc]() value you will
		convert to the nothing data type (or simply unset the variable).
		The only explicit conversion that is not possible is to hobject.
		[br]
		As stated several times in this document, KVS tries to manage the casts automatically
		so you usually don't need to care about it. The explicit casts are provided for
		the very few cases where an automatic conversion would lead to an unexpected value (for your script)
		and for writer's clarity.
		[title]More about variables life-cycle[/title]
		[br]
		As stated above variables start their existence when you assign something to them.
		After a variable has been created it persists until it goes out of his scope (remember
		about local and global variables?) or you explicitly destroy it. You will usually
		not care about it and just leave the KVS engine to do his cleaning job but it's still worth
		knowing that you actually can force KVIrc to free the memory used by a variable.
		[br]
		The first method to explicitly destroy a variable is to call [cmd]unset[/cmd] on it.
		[cmd]unset[/cmd] in fact accepts a list of variables so you can destroy more variables at once.
		[example]
			%a = [fnc]$array[/fnc]("data","for","a","really","huge","array","of","items")
			%b = 10
			%c = "just a string that eats memory"
			[cmd]unset[/cmd] %a,%b,%c
		[/example]
		[br]
		The KVS engine treats unset variables just like empty strings. The opposite is also valid: empty
		strings behave like empty (unset) variables. This means that you can assign an empty string
		to a variable to unset it.
		[example]
			%a = "test"; [comment]# %a starts his existence[/comment]
			%b = "test2";
			%a = ""; [comment]# %a is in fact unset[/comment]
			%b = ; [comment]# syntactically this is just the same as above[/comment]
		[/example]
		[br]
		Note that because of mutability of variables (explained above) you can use the empty string
		assignment also to free arrays and hashes.
		[title]Extended scope variables[/title]
		[br]
		Beside local and global variables there is a third family of them.
		Variables that have a ':' character just after the leading '%' are [b]extended scope[/b] variables.
		[i]%:index[/i], [i]%:Hello[/i], [i]%:something.else[/i] are all valid special scope variable names.
		They're actually used in popups and in timers (but later I might find other usages as well :).
		"Extended scope" means that these variables are somewhere in the middle between
		global and local variables. They normally act as local, but in some cases their [b]lifetime[/b] and [b]visibility[/b]
		may be extended.
		[br]
		For example, in the popups, all the special scope variables
		are visible during all the "lifetime" of a popup (so from the prologue code call to
		the moment when the user selects an item and the corresponding code is executed).
		This allows you to pre-calculate some data or conditions in the popup prologue
		and use this data in the popup item conditions and item handlers.
		[title]Variable evaluation[/title]
		[br]
		A variable can appear in every place where a parameter
		is expected: so after the command name, after a switch or inside
		an identifier parameters. The KVS parser will try to extract the longest possible variable
		name after a literal percent '%' sign everywhere in the parameter string. So the command sequence
		[example]
			%number = 1st; echo this is my %number variable test
		[/example]
		[br]
		will first assign [i]1st[/i] to the variable [i]%number[/i] and then execute
		"echo this is my 1st variable test". The following example will [b]not[/b] work as expected.
		[example]
			%number = 1; echo this is my %numberst variable test
		[/example]
		[br]
		KVS will assign [b]1[/b] to %number in this case but the next variable
		name extracted will be [i]%numberst[/i] that is actually empty; so finally
		"echo this is my variable test" will be executed.
		To avoid this problem you can use the backslash escape character:
		[example]
			%number = 1; echo this is my %number\st variable test
		[/example]
		[title]Putting it all together[/title]
		[br]
		Variables can be either local, global or have an extended scope. Their start to exist
		when you first assign something to them and they disappear when they go out of their
		scope or you explicitly destroy them.
		[br]
		KVS has 8 builtin data types: string, integer, real, boolean, hobject, nothing, array and hash.
		The first 6 are scalar data types while the last two are not.
		[br]
		When possible, KVS manages all the conversions between data types silently.
		In the few cases in that an implicit conversion is not possible you have to manage the conversion
		manually otherwise KVS will complain.
		[br]
*/

void KviKvsParser::skipSpaces()
{
	while((KVSP_curCharUnicode == ' ') || (KVSP_curCharUnicode == '\t'))
	{
		KVSP_skipChar;
	}

	if(KVSP_curCharUnicode == '\\')
	{
		KVSP_skipChar;
		if(KVSP_curCharUnicode == '\n')
		{
			KVSP_skipChar;
			skipSpaces();
			return;
		}
		else if(KVSP_curCharUnicode == '\r')
		{
			KVSP_skipChar;
			if(KVSP_curCharUnicode == '\n')
			{
				KVSP_skipChar;
				skipSpaces();
				return;
			}
			else
			{
				KVSP_backChar;
				KVSP_backChar;
			}
		}
		else
		{
			KVSP_backChar;
		}
	}
}

bool KviKvsParser::skipSpacesAndNewlines()
{
	while((KVSP_curCharUnicode == ' ') || (KVSP_curCharUnicode == '\t') || (KVSP_curCharUnicode == '\n') || (KVSP_curCharUnicode == '\r'))
	{
		KVSP_skipChar;
	}

	switch(KVSP_curCharUnicode)
	{
		case '\\':
			KVSP_skipChar;
			if(KVSP_curCharUnicode == '\n')
			{
				KVSP_skipChar;
				return skipSpacesAndNewlines();
			}
			else if(KVSP_curCharUnicode == '\r')
			{
				KVSP_skipChar;
				if(KVSP_curCharUnicode == '\n')
				{
					KVSP_skipChar;
					return skipSpacesAndNewlines();
				}
				else
				{
					KVSP_backChar;
					KVSP_backChar;
				}
			}
			else
			{
				KVSP_backChar;
			}
			break;
		case '#':
		case '/':
			// we allow comments too!
			(void)parseComment(); // this will return 0 anyway (and never trigger an error here)
			if(error())
				return false;
			return skipSpacesAndNewlines();
			break;
	}
	return true;
}

void KviKvsParser::skipToNextLine()
{
	while((KVSP_curCharUnicode != 0) && (KVSP_curCharUnicode != '\n'))
		KVSP_skipChar;

	if(KVSP_curCharUnicode == '\n')
		KVSP_skipChar;
}

KviKvsTreeNodeInstruction * KviKvsParser::parseInstructionList()
{
	KviKvsTreeNodeInstructionBlock * l = new KviKvsTreeNodeInstructionBlock(KVSP_curCharPointer);

	for(;;)
	{
		if(!skipSpacesAndNewlines())
		{
			delete l;
			return nullptr;
		}

		if(KVSP_curCharUnicode != 0)
		{
			// instruction
			KviKvsTreeNodeInstruction * i = parseInstruction();
			if(i)
				l->addInstruction(i);
			else
			{
				if(error())
				{
					// ops...
					delete l;
					return nullptr;
				} // else empty instruction
			}
		}
		else
		{
			if(l->instructionCount() == 1)
			{
				// return the single instruction instead
				KviKvsTreeNodeInstruction * i = l->releaseFirst();
				delete l;
				return i;
			}
			// end of buffer
			return l;
		}
	}

	// never here
	KVSP_ASSERT(false);
	return nullptr;
}

KviKvsTreeNodeData * KviKvsParser::parseParameterPercentOrDollar()
{
	KVSP_ASSERT((KVSP_curCharUnicode == '%') || (KVSP_curCharUnicode == '$') || (KVSP_curCharUnicode == '@'));

	if(KVSP_curCharUnicode == '%')
	{
		KVSP_skipChar;
		if(!KVSP_curCharIsLetter && (KVSP_curCharUnicode != ':'))
		{
			// be flexible : allow an "alone" '%' char
			return new KviKvsTreeNodeConstantData(KVSP_curCharPointer - 1, new KviKvsVariant(QString("%")));
		}
		// this is surely a variable or function
		KVSP_backChar;
	}
	else if(KVSP_curCharUnicode == '$')
	{
		KVSP_skipChar;
		if(!KVSP_curCharIsFunctionStart)
		{
			// be flexible : allow an "alone" '$' char
			return new KviKvsTreeNodeConstantData(KVSP_curCharPointer - 1, new KviKvsVariant(QString("$")));
		}
		// this is surely a variable or function
		KVSP_backChar;
	}

	return parsePercentOrDollar();
}

KviKvsTreeNodeData * KviKvsParser::parsePercentOrDollar(bool bInObjScope)
{
	KVSP_ASSERT((KVSP_curCharUnicode == '%') || (KVSP_curCharUnicode == '$') || (KVSP_curCharUnicode == '@'));

	KviKvsTreeNodeData * r;
	const QChar * pBegin;

	if(KVSP_curCharUnicode == '%')
	{
		r = parsePercent(bInObjScope);
		if(!r)
			return nullptr;
	}
	else if(KVSP_curCharUnicode == '$')
	{
		r = parseDollar(bInObjScope);
		if(!r)
			return nullptr;
	}
	else
	{
		// this is @

		static QString szStrayAtRoutineName("@");
		static QString szMightBeStrayAtOrThisRoutineName("@?");

		pBegin = KVSP_curCharPointer;

		KVSP_skipChar;

		if(bInObjScope || ((KVSP_curCharUnicode != '$') && (KVSP_curCharUnicode != '%')))
		{
			// we're sure this is just a stray @
			// we use a trick here: when @ is not supposed to be an object scope call
			// then we create a function that will return the @ itself as a string
			KviKvsCoreFunctionExecRoutine * pRoutine = KviKvsKernel::instance()->findCoreFunctionExecRoutine(szStrayAtRoutineName);
			r = new KviKvsTreeNodeCoreFunctionCall(KVSP_curCharPointer, szStrayAtRoutineName, pRoutine, new KviKvsTreeNodeDataList(KVSP_curCharPointer));
			//KVSP_skipChar;
			return r;
		}
		// we're not in object scope and cur char is either $ or %
		// check for the common syntax $0!$1@$2 seen in hostmasks
		// @$<digit> is non valid anyway
		if(KVSP_curCharUnicode == '$')
		{
			KVSP_skipChar;
			if(KVSP_curCharIsNumber)
			{
				// again a stray @
				KVSP_backChar;
				KviKvsCoreFunctionExecRoutine * pRoutine = KviKvsKernel::instance()->findCoreFunctionExecRoutine(szStrayAtRoutineName);
				r = new KviKvsTreeNodeCoreFunctionCall(KVSP_curCharPointer, szStrayAtRoutineName, pRoutine, new KviKvsTreeNodeDataList(KVSP_curCharPointer));
				return r;
			}
			KVSP_backChar;
		}

		// now we're unsure: we will be able to decide only at runtime if it is a stray @ or the shortcut for $this
		// this design was a bit ugly.. I must admit it... but it is really useful when writing object classes...
		KviKvsCoreFunctionExecRoutine * pRoutine = KviKvsKernel::instance()->findCoreFunctionExecRoutine(szMightBeStrayAtOrThisRoutineName);
		// MUST BE THERE!
		// core function call
		r = new KviKvsTreeNodeCoreFunctionCall(pBegin, szMightBeStrayAtOrThisRoutineName, pRoutine, new KviKvsTreeNodeDataList(pBegin));

		skipSpaces();

		pBegin = KVSP_curCharPointer;

		goto handle_scope_operator;
	}

	pBegin = KVSP_curCharPointer;

	while((KVSP_curCharUnicode == '[') || (KVSP_curCharUnicode == '{'))
	{
		if(KVSP_curCharUnicode == '[')
		{
			// array index
			KVSP_skipChar;
			skipSpaces();
			if(KVSP_curCharUnicode == ']')
			{
				KVSP_skipChar;
				if(KVSP_curCharUnicode == '#')
				{
					// count
					KVSP_skipChar;
					return new KviKvsTreeNodeArrayCount(pBegin, r);
				}
				else
				{
					// a hash reference assert
					return new KviKvsTreeNodeArrayReferenceAssert(pBegin, r);
				}
			}

			KviKvsTreeNodeExpression * e = parseExpression(']');
			if(!e)
			{
				delete r;
				return nullptr;
			}

			r = new KviKvsTreeNodeArrayElement(pBegin, r, e);
		}
		else
		{
			// hash key
			KVSP_skipChar;
			skipSpaces();

			if(KVSP_curCharUnicode == '}')
			{
				// entire hash ?
				KVSP_skipChar;
				if(KVSP_curCharUnicode == '#')
				{
					KVSP_skipChar;
					return new KviKvsTreeNodeHashCount(pBegin, r);
				}
				return new KviKvsTreeNodeHashReferenceAssert(pBegin, r);
			}

			KviKvsTreeNodeData * i = parseHashKey();
			if(!i)
			{
				// error
				delete r;
				return nullptr;
			}

			KVSP_ASSERT(KVSP_curCharUnicode == '}');

			KVSP_skipChar;

			r = new KviKvsTreeNodeHashElement(pBegin, r, i);
		}
	}

	if(KVSP_curCharUnicode != '-')
	{
		return r;
	}

	if(!r->canEvaluateToObjectReference())
		return r; // FIXME: maybe print a warning ?

	// might be a scope operator

	KVSP_skipChar;
	if(KVSP_curCharUnicode != '>')
	{
		KVSP_backChar;
		return r;
	}

	KVSP_skipChar;
	skipSpaces();

	if((KVSP_curCharUnicode != '$') && (KVSP_curCharUnicode != '%'))
	{
		KVSP_setCurCharPointer(pBegin);
		return r;
	}

handle_scope_operator:

	// hmmm... there really seems to be a scope operator there...
	if(KVSP_curCharUnicode == '%')
	{
		KVSP_skipChar;
		if(!KVSP_curCharIsLetter)
		{
			// be flexible : allow an "alone" '%' char
			KVSP_setCurCharPointer(pBegin);
			return r;
		}
	}
	else
	{
		KVSP_skipChar;
		if(!KVSP_curCharIsFunctionStart)
		{
			// be flexible : allow an "alone" '$' char
			KVSP_setCurCharPointer(pBegin);
			return r;
		}
	}

	// ok : try the scope operator
	KVSP_backChar;

	pBegin = KVSP_curCharPointer;

	KviKvsTreeNodeData * r2 = parsePercentOrDollar(true);

	if(!r2)
	{
		// must be an error
		delete r;
		return nullptr;
	}

	if(!r2->canEvaluateInObjectScope())
	{
		// ops... it really wasn't
		delete r2;
		KVSP_setCurCharPointer(pBegin);
		return r;
	}

	return new KviKvsTreeNodeScopeOperator(pBegin, r, r2);
}

KviKvsTreeNodeVariable * KviKvsParser::parsePercent(bool bInObjScope)
{
	KVSP_ASSERT(KVSP_curCharUnicode == '%');

	const QChar * pBegin = KVSP_curCharPointer;

	KVSP_skipChar;

	bool bExtScope;

	if(KVSP_curCharUnicode == ':')
	{
		bExtScope = true;
		KVSP_skipChar;
	}
	else
	{
		bExtScope = false;
	}

	if(!((KVSP_curCharIsLetterOrNumber) || (KVSP_curCharUnicode == '_')))
	{
		error(KVSP_curCharPointer, __tr2qs_ctx("Syntax error after '%' variable prefix. If you want to use a plain '%' in the code you need to escape it", "kvs"));
		return nullptr;
	}

	const QChar * pIdBegin = KVSP_curCharPointer;

	while((KVSP_curCharIsLetterOrNumber) || (KVSP_curCharUnicode == '_'))
		KVSP_skipChar;

	QString szIdentifier(pIdBegin, KVSP_curCharPointer - pIdBegin);

	//#warning "ADD A KviKvsTreeNodeBuiltinCleanupVariablesCommand on this KviKvsParser object"
	//#warning "KviKvsParser will append it to the script"

	if(bExtScope)
	{
		if(bInObjScope)
		{
			error(KVSP_curCharPointer, __tr2qs_ctx("Objects have no extended scope variables", "kvs"));
			return nullptr;
		}
		return new KviKvsTreeNodeExtendedScopeVariable(pBegin, szIdentifier);
	}

	if(bInObjScope)
		return new KviKvsTreeNodeObjectField(pBegin, szIdentifier);

	// Check if explicitly declared as global
	if(m_pGlobals)
	{
		if(m_pGlobals->find(szIdentifier))
			return new KviKvsTreeNodeGlobalVariable(pBegin, szIdentifier);
	}

	if(m_iFlags & AssumeLocals)
		return new KviKvsTreeNodeLocalVariable(pBegin, szIdentifier);

	if(pIdBegin->category() == QChar::Letter_Uppercase)
	{
		//qDebug("Variable %s is global",szIdentifier.toUtf8().data());
		//if(m_iFlags & Pedantic)
		//	warning(pIdBegin,__tr2qs_ctx("Declaring global variables with an uppercase letter is deprecated. Global variables should be declared with 'global'","kvs"));
		return new KviKvsTreeNodeGlobalVariable(pBegin, szIdentifier);
	}

	return new KviKvsTreeNodeLocalVariable(pBegin, szIdentifier);
}

KviKvsTreeNodeInstruction * KviKvsParser::parseInstruction()
{
	switch(KVSP_curCharUnicode)
	{
		case '#':
		case '/':
			(void)parseComment(); // this will return 0 anyway
			return nullptr;
			break;
		case 0: // empty instruction
			return nullptr;
			break;
		case '\n':
		case '\r':
		case ';': // empty instruction
			KVSP_skipChar;
			return nullptr;
			break;
		case '{': // command block
			return parseInstructionBlock();
			break;
		case '$':
		case '%':
		case '@':
			return parseVoidFunctionCallOrOperation();
			break;
		default:
			if(KVSP_curCharIsLetter || (KVSP_curCharUnicode == '_'))
			{
				// must be a command
				return parseCommand();
			}
			else
			{
				// what the heck is this ?
				error(KVSP_curCharPointer, __tr2qs_ctx("Found character '%q' (Unicode %x) where an instruction was expected", "kvs"), KVSP_curCharPointer, KVSP_curCharUnicode);
				return nullptr;
			}
			break;
	}
	// never here
	KVSP_ASSERT(false);
	return nullptr;
}

KviKvsTreeNodeInstruction * KviKvsParser::parseInstructionBlock()
{
	KVSP_ASSERT(KVSP_curCharUnicode == '{');

	KVSP_skipChar;

	const QChar * pBegin = KVSP_curCharPointer;

	KviKvsTreeNodeInstructionBlock * b = new KviKvsTreeNodeInstructionBlock(pBegin - 1);

	for(;;)
	{
		if(!skipSpacesAndNewlines())
		{
			delete b;
			return nullptr;
		}

		switch(KVSP_curCharUnicode)
		{
			case 0:
				delete b;
				warning(pBegin, __tr2qs_ctx("Unterminated instruction block", "kvs"));
				error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of script in instruction block (missing closing brace)", "kvs"));
				return nullptr;
				break;
			case '}':
				KVSP_skipChar;
				if(b->instructionCount() <= 1)
				{
					if(b->instructionCount() < 1)
					{
						delete b;
						return nullptr; // just an empty block
					}
					// a single instruction
					KviKvsTreeNodeInstruction * i = b->releaseFirst();
					delete b;
					return i;
				}
				return b;
				break;
			default:
				// instruction
				KviKvsTreeNodeInstruction * i = parseInstruction();
				if(i)
					b->addInstruction(i);
				else
				{
					if(error())
					{
						// ops...
						delete b;
						return nullptr;
					} // else empty instruction
				}
				break;
		}
	}
	// never reached
	return nullptr;
}

KviKvsTreeNodeSwitchList * KviKvsParser::parseCommandSwitchList()
{
	KVSP_ASSERT(KVSP_curCharUnicode == '-');

	KviKvsTreeNodeSwitchList * sw = new KviKvsTreeNodeSwitchList(KVSP_curCharPointer);

	while(KVSP_curCharUnicode == '-')
	{
		const QChar * pBegin = KVSP_curCharPointer;
		KVSP_skipChar;

		bool bLong = false;

		if(KVSP_curCharUnicode == '-')
		{
			// long switch
			pBegin = KVSP_curCharPointer;
			KVSP_skipChar;
			bLong = true;
		}

		skipSpaces();
		if(!KVSP_curCharIsLetter)
		{
			if(KVSP_curCharIsNumber || KVSP_curCharIsEndOfCommand)
			{
				// a -digit : this is probably a negative number instead
				// or just a single dash (or couple of dashes)
				// go back to the initial dash and treat it as text...and return the current switch list
				KVSP_setCurCharPointer(pBegin);
				if(sw->isEmpty())
				{
					// not an error!
					delete sw;
					return nullptr;
				}
				return sw;
			}
			else
			{
				delete sw;
				warning(pBegin, __tr2qs_ctx("The dash after a command should be followed by a letter (switch), by a digit (negative number) or be escaped", "kvs"));

				if(KVSP_curCharUnicode == 0)
				{
					error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected character '%q' (Unicode %x) after a switch dash", "kvs"), KVSP_curCharPointer, KVSP_curCharUnicode);
				}
				else
				{
					error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of script after a switch dash", "kvs"));
				}
				return nullptr;
			}
		}

		const QChar * pSw = KVSP_curCharPointer;

		KVSP_skipChar;
		while((KVSP_curCharIsLetterOrNumber) || (KVSP_curCharUnicode == '-'))
			KVSP_skipChar;

		const QChar * pSwEnd = KVSP_curCharPointer;

		skipSpaces();

		if(KVSP_curCharUnicode == '=')
		{
			KVSP_skipChar;
			skipSpaces();
			KviKvsTreeNodeData * p = parseCommandParameter();
			if(!p)
			{
				// must be an error :(
				if(error())
				{
					error(pBegin, __tr2qs_ctx("The above problem might be related to the switch dash and the following equal sign", "kvs"));
					delete sw;
					return nullptr;
				}
				else
				{
					// assume empty string
					p = new KviKvsTreeNodeConstantData(KVSP_curCharPointer, new KviKvsVariant(QString("")));
				}
			}

			skipSpaces();

			if(bLong)
				sw->addLong(QString(pSw, pSwEnd - pSw), p);
			else
				sw->addShort(pSw->toLower().unicode(), p);
		}
		else
		{
			if(bLong)
				sw->addLong(QString(pSw, pSwEnd - pSw), new KviKvsTreeNodeConstantData(KVSP_curCharPointer, new KviKvsVariant(true))); // empty param
			else
				sw->addShort(pSw->toLower().unicode(), new KviKvsTreeNodeConstantData(KVSP_curCharPointer, new KviKvsVariant(true))); // empty param
		}
	}

	return sw;
}

KviKvsTreeNodeDataList * KviKvsParser::parseCommandParameterList()
{
	KviKvsTreeNodeDataList * l = new KviKvsTreeNodeDataList(KVSP_curCharPointer);

	for(;;)
	{
		skipSpaces();
		switch(KVSP_curCharUnicode)
		{
			case 0:
				return l;
				break;
			case '\r':
			case '\n':
			case ';':
				KVSP_skipChar;
				return l;
				break;
			default:
				// anything else is a parameter
				KviKvsTreeNodeData * p = parseCommandParameter();
				if(!p)
				{
					// this is an error
					delete l;
					return nullptr;
				}
				l->addItem(p);
				break;
		}
	}

	// never here
	KVSP_ASSERT(false);
	return nullptr;
}

KviPointerList<QString> * KviKvsParser::parseCommaSeparatedParameterListNoTree()
{
	KviPointerList<QString> * l = new KviPointerList<QString>;
	l->setAutoDelete(true);

	KVSP_skipChar;

	for(;;)
	{
		skipSpaces();
		switch(KVSP_curCharUnicode)
		{
			case 0:
				error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of script in parameter list", "kvs"));
				delete l;
				return nullptr;
				break;
			case '\r':
			case '\n':
				error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of line in parameter list", "kvs"));
				delete l;
				return nullptr;
				break;
			/*
			case ',':
				KVSP_skipChar;
			break;
			case ')':
				KVSP_skipChar;
				return l;
			break;
			*/
			default:
			{
				// anything else is a parameter
				const QChar * pBegin = KVSP_curCharPointer;
				KviKvsTreeNodeData * p = parseCommaSeparatedParameter();
				if(!p)
				{
					// this is an error
					delete l;
					return nullptr;
				}
				delete p;
				QString * s = new QString(QString(pBegin, KVSP_curCharPointer - pBegin).trimmed());
				l->append(s);

				switch(KVSP_curCharUnicode)
				{
					case ',':
						KVSP_skipChar;
						break;
					case ')':
						KVSP_skipChar;
						return l;
						break;
				}
			}
			break;
		}
	}

	// never here
	KVSP_ASSERT(false);
	return nullptr;
}

KviKvsTreeNodeDataList * KviKvsParser::parseCommaSeparatedParameterList()
{
	KviKvsTreeNodeDataList * l = new KviKvsTreeNodeDataList(KVSP_curCharPointer);

	KVSP_skipChar;

	for(;;)
	{
		skipSpaces();
		switch(KVSP_curCharUnicode)
		{
			case 0:
				error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of script in parameter list", "kvs"));
				delete l;
				return nullptr;
				break;
			case '\r':
			case '\n':
				error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of line in parameter list", "kvs"));
				delete l;
				return nullptr;
				break;
			/*
			case ',':
				KVSP_skipChar;
			break;
			case ')':
				KVSP_skipChar;
				return l;
			break;
			*/
			default:
				// anything else is a parameter
				KviKvsTreeNodeData * p = parseCommaSeparatedParameter();
				if(!p)
				{
					// this is an error
					delete l;
					return nullptr;
				}
				l->addItem(p);

				switch(KVSP_curCharUnicode)
				{
					case ',':
						KVSP_skipChar;
						break;
					case ')':
						KVSP_skipChar;
						return l;
						break;
				}
				break;
		}
	}

	// never here
	KVSP_ASSERT(false);
	return nullptr;
}

#define LITERAL_PARAM_PARSING_FUNCTION_BEGIN(__funcname)    \
	KviKvsTreeNodeConstantData * KviKvsParser::__funcname() \
	{                                                       \
		QString szValue;                                    \
                                                            \
		[[maybe_unused]] const QChar * pStart = KVSP_curCharPointer;         \
		const QChar * pBegin = KVSP_curCharPointer;         \
		int iLen = 0;                                       \
		[[maybe_unused]] int iNestedTerminators = 0;                         \
                                                            \
		for(;;)                                             \
		{                                                   \
			switch(KVSP_curCharUnicode)                     \
			{

#define LITERAL_PARAM_PARSING_FUNCTION_WARN_NESTED_TERMINATOR                                                                                                                                                      \
	if(!_OUTPUT_MUTE)                                                                                                                                                                                              \
		warning(KVSP_curCharPointer, __tr2qs_ctx("Nested character %q corresponding to expected terminator, this might confuse me a bit: it is a good idea to enclose it in quotes", "kvs"), KVSP_curCharPointer); \
	KVSP_skipChar;                                                                                                                                                                                                 \
	iNestedTerminators++;                                                                                                                                                                                          \
	iLen++;                                                                                                                                                                                                        \
	break;

#define LITERAL_PARAM_PARSING_FUNCTION_END_WITH_EXPECTED_TERMINATOR                                                           \
	if(iNestedTerminators > 0)                                                                                                \
	{                                                                                                                         \
		if(!_OUTPUT_MUTE)                                                                                                     \
			warning(KVSP_curCharPointer, __tr2qs_ctx("Skipping nested terminator character %q", "kvs"), KVSP_curCharPointer); \
		KVSP_skipChar;                                                                                                        \
		iNestedTerminators--;                                                                                                 \
		iLen++;                                                                                                               \
	}                                                                                                                         \
	else                                                                                                                      \
	{                                                                                                                         \
		if(iLen > 0)                                                                                                          \
			szValue.append(QString(pBegin, iLen));                                                                            \
		{                                                                                                                     \
			bool bOk;                                                                                                         \
			kvs_int_t iVal = szValue.toLong(&bOk);                                                                            \
			if(bOk)                                                                                                           \
				return new KviKvsTreeNodeConstantData(pBegin, new KviKvsVariant(iVal));                                       \
			kvs_real_t dVal = szValue.toDouble(&bOk);                                                                         \
			if(bOk)                                                                                                           \
				return new KviKvsTreeNodeConstantData(pBegin, new KviKvsVariant(dVal));                                       \
		}                                                                                                                     \
		return new KviKvsTreeNodeConstantData(pBegin, new KviKvsVariant(szValue));                                            \
	}                                                                                                                         \
	break;

#define LITERAL_PARAM_PARSING_FUNCTION_GENERIC_END                                                                \
	if(iLen > 0)                                                                                                  \
		szValue.append(QString(pBegin, iLen));                                                                    \
	return new KviKvsTreeNodeConstantData(pBegin, new KviKvsVariant(szValue));                                    \
	break;                                                                                                        \
	case '\\':                                                                                                    \
		if(iLen > 0)                                                                                              \
			szValue.append(QString(pBegin, iLen));                                                                \
		KVSP_skipChar;                                                                                            \
		switch(KVSP_curCharUnicode)                                                                               \
		{                                                                                                         \
			case 0:                                                                                               \
				warning(KVSP_curCharPointer - 1, __tr2qs_ctx("Stray backslash at the end of the script", "kvs")); \
				iLen = 0;                                                                                         \
				break;                                                                                            \
			case '\r':                                                                                            \
			case '\n':                                                                                            \
				KVSP_skipChar;                                                                                    \
				pBegin = KVSP_curCharPointer;                                                                     \
				iLen = 0;                                                                                         \
				break;                                                                                            \
			case 'r':                                                                                             \
				KVSP_skipChar;                                                                                    \
				pBegin = KVSP_curCharPointer;                                                                     \
				szValue.append(QChar('\r'));                                                                      \
				iLen = 0;                                                                                         \
				break;                                                                                            \
			case 'n':                                                                                             \
				KVSP_skipChar;                                                                                    \
				pBegin = KVSP_curCharPointer;                                                                     \
				szValue.append(QChar('\n'));                                                                      \
				iLen = 0;                                                                                         \
				break;                                                                                            \
			case 't':                                                                                             \
				KVSP_skipChar;                                                                                    \
				pBegin = KVSP_curCharPointer;                                                                     \
				szValue.append(QChar('\t'));                                                                      \
				iLen = 0;                                                                                         \
				break;                                                                                            \
			default:                                                                                              \
				pBegin = KVSP_curCharPointer;                                                                     \
				KVSP_skipChar;                                                                                    \
				iLen = 1;                                                                                         \
				break;                                                                                            \
		}                                                                                                         \
		break;                                                                                                    \
	default:                                                                                                      \
		KVSP_skipChar;                                                                                            \
		iLen++;                                                                                                   \
		break;                                                                                                    \
		}                                                                                                         \
		}                                                                                                         \
		KVSP_ASSERT(false);                                                                                       \
		return nullptr;                                                                                           \
		}

LITERAL_PARAM_PARSING_FUNCTION_BEGIN(parseCommandLiteralParameter)

case 0:
case '$':
case '%':
case '@':
case '\r':
case '\n':
case '"':
case ';':
case ' ':
case '\t':
	LITERAL_PARAM_PARSING_FUNCTION_GENERIC_END

	LITERAL_PARAM_PARSING_FUNCTION_BEGIN(parseStringLiteralParameter)

case 0:
case '$':
case '%':
case '@':
case '\r':
case '\n':
case '"':
	LITERAL_PARAM_PARSING_FUNCTION_GENERIC_END

	/*
LITERAL_PARAM_PARSING_FUNCTION_BEGIN(parseArrayIndexLiteralParameter)
			case '\t':
			case ' ':
			case ']':
LITERAL_PARAM_PARSING_FUNCTION_END
*/

	LITERAL_PARAM_PARSING_FUNCTION_BEGIN(parseHashKeyLiteralParameter)

case '{':
	LITERAL_PARAM_PARSING_FUNCTION_WARN_NESTED_TERMINATOR
case '}':
	LITERAL_PARAM_PARSING_FUNCTION_END_WITH_EXPECTED_TERMINATOR
case 0:
case '$':
case '%':
case '@':
case '\r':
case '\n':
case '"':
case '\t':
case ' ':
	LITERAL_PARAM_PARSING_FUNCTION_GENERIC_END

	LITERAL_PARAM_PARSING_FUNCTION_BEGIN(parseCommaSeparatedLiteralParameter)

case '(':
	LITERAL_PARAM_PARSING_FUNCTION_WARN_NESTED_TERMINATOR
case ')':
	LITERAL_PARAM_PARSING_FUNCTION_END_WITH_EXPECTED_TERMINATOR
case 0:
case '$':
case '%':
case '@':
case '\r':
case '\n':
case '"':
case ',':
case ' ':
case '\t':
	LITERAL_PARAM_PARSING_FUNCTION_GENERIC_END

	LITERAL_PARAM_PARSING_FUNCTION_BEGIN(parseSingleLiteralParameterInParenthesis)

case '(':
	LITERAL_PARAM_PARSING_FUNCTION_WARN_NESTED_TERMINATOR
case ')':
	LITERAL_PARAM_PARSING_FUNCTION_END_WITH_EXPECTED_TERMINATOR
case 0:
case '$':
case '%':
case '@':
case '\r':
case '\n':
case '"':
case ' ':
case '\t':
	LITERAL_PARAM_PARSING_FUNCTION_GENERIC_END

	LITERAL_PARAM_PARSING_FUNCTION_BEGIN(parseBindingOperationLiteralParameter)

case 0:
case '$':
case '%':
case '@':
case '\r':
case '\n':
case '"':
case '/':
	LITERAL_PARAM_PARSING_FUNCTION_GENERIC_END

	/*
KviKvsTreeNodeData * KviKvsParser::parseArrayIndex()
{
	KviPointerList<KviKvsTreeNodeData> * l = new KviPointerList<KviKvsTreeNodeData>();
	l->setAutoDelete(true);

	const QChar * pBegin = KVSP_curCharPointer;

	//KVSP_skipChar;

	for(;;)
	{
		switch(KVSP_curCharUnicode)
		{
			case 0:
				delete l;
				warning(pBegin,__tr2qs_ctx("Unterminated array index","kvs"));
				error(KVSP_curCharPointer,__tr2qs_ctx("Unexpected end of script in array index (missing ']' character?)","kvs"));
				return nullptr;
			break;
			case '\n':
				delete l;
				warning(pBegin,__tr2qs_ctx("Unterminated array index","kvs"));
				error(KVSP_curCharPointer,__tr2qs_ctx("Unexpected end of line in array index (missing ']' character or unescaped newline)","kvs"));
				return nullptr;
			break;
			case ' ':
			case '\t':
				skipSpaces();
				if(KVSP_curCharUnicode != ']')
				{
					delete l;
					warning(pBegin,__tr2qs_ctx("Unterminated array index","kvs"));
					switch(KVSP_curCharUnicode)
					{
						case 0:
							error(KVSP_curCharPointer,__tr2qs_ctx("Unexpected end of script in array index (missing ']' character?)","kvs"));
						break;
						case '\n':
							error(KVSP_curCharPointer,__tr2qs_ctx("Unexpected end of line in array index (missing ']' character or unescaped newline)","kvs"));
						break;
						default:
							error(KVSP_curCharPointer,__tr2qs_ctx("Unexpected character '%q' (Unicode %x) in array index: it should be already terminated","kvs"),KVSP_curCharPointer,KVSP_curCharUnicode);
						break;
					}
					return nullptr;
				}
				goto end_of_the_array_index;
			break;
			case '$':
			case '%':
			{
				// this may be a data reference
				KviKvsTreeNodeData * p = parseParameterPercentOrDollar();
				if(!p)
				{
					// this is an error
					delete l;
					return nullptr;
				}
				l->append(p);
			}
			break;
			case ']':
			{
				// end of the array index
				goto end_of_the_array_index;
			}
			break;
			case '"':
			{
				// string (should we parse strings in array indexes anyway ?).. well "1"$count might be a valid one in the end
				KviKvsTreeNodeData * p = parseStringParameter();
				if(!p)
				{
					// error
					delete l;
					return nullptr;
				}
				l->append(p);
			}
			break;
			default:
			{
				// anything else is a literal
				l->append(parseArrayIndexLiteralParameter());
			}
			break;
		}
	}
end_of_the_array_index:
	if(l->count() > 1)
	{
		// complex parameter needed
		return new KviKvsTreeNodeCompositeData(l);
	} else {
		// a single parameter in the list
		l->setAutoDelete(false);
		KviKvsTreeNodeData * p = l->first();
		delete l;
		return p;
	}

}
*/

	KviKvsTreeNodeData * KviKvsParser::parseHashKey()
	{
		KviPointerList<KviKvsTreeNodeData> * l = new KviPointerList<KviKvsTreeNodeData>();
		l->setAutoDelete(true);

		const QChar * pBegin = KVSP_curCharPointer;

		//KVSP_skipChar;

		for(;;)
		{
			switch(KVSP_curCharUnicode)
			{
				case 0:
					delete l;
					warning(pBegin, __tr2qs_ctx("Unterminated hash key", "kvs"));
					error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of script in hash key (missing '}' character?)", "kvs"));
					return nullptr;
					break;
				case '\r':
				case '\n':
					delete l;
					warning(pBegin, __tr2qs_ctx("Unterminated hash key", "kvs"));
					error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of line in hash key (missing '}' character or unescaped newline)", "kvs"));
					return nullptr;
					break;
				case ' ':
				case '\t':
					skipSpaces();
					if(KVSP_curCharUnicode != '}')
					{
						// separate by single spaces
						l->append(new KviKvsTreeNodeConstantData(KVSP_curCharPointer, new KviKvsVariant(QString(" "))));
					}
					else
					{
						goto end_of_the_hash_key;
					}
					break;
				case '$':
				case '%':
				case '@':
				{
					// this may be a data reference
					KviKvsTreeNodeData * p = parseParameterPercentOrDollar();
					if(!p)
					{
						// this is an error
						delete l;
						return nullptr;
					}
					l->append(p);
				}
				break;
				case '}':
				{
					// end of the array index
					goto end_of_the_hash_key;
				}
				break;
				case '"':
				{
					// string
					KviKvsTreeNodeData * p = parseStringParameter();
					if(!p)
					{
						// error
						delete l;
						return nullptr;
					}
					l->append(p);
				}
				break;
				default:
				{
					// anything else is a literal
					l->append(parseHashKeyLiteralParameter());
				}
				break;
			}
		}
	end_of_the_hash_key:
		if(l->count() > 1)
		{
			// complex parameter needed
			return new KviKvsTreeNodeCompositeData(pBegin, l);
		}
		else
		{
			// a single parameter in the list
			l->setAutoDelete(false);
			KviKvsTreeNodeData * p = l->first();
			delete l;
			return p;
		}
		// never reached
		return nullptr;
	}

	/*
PARENTHESIS_PARAMETER_PARSING_FUNCTION_BEGIN(parseCommaSeparatedParameter)
	case 0:
	case ',':
	case ')':
	case '\n':
PARENTHESIS_PARAMETER_PARSING_FUNCTION_END()

#define PARENTHESIS_PARAMETER_PARSING_FUNCTION_BEGIN(_name) \
*/

	KviKvsTreeNodeData * KviKvsParser::parseCommaSeparatedParameter()
	{
		KviPointerList<KviKvsTreeNodeData> * l = new KviPointerList<KviKvsTreeNodeData>;
		l->setAutoDelete(true);

		const QChar * pBegin = KVSP_curCharPointer;

		for(;;)
		{
			switch(KVSP_curCharUnicode)
			{
				case 0:
				case ',':
				case ')':
				case '\r':
				case '\n':
					// not a part of a parameter
					goto end_of_function_parameter;
					break;
				case '$':
				case '%':
				case '@':
				{
					// this may be a data reference
					KviKvsTreeNodeData * p = parseParameterPercentOrDollar();
					if(!p)
					{
						// this is an error
						delete l;
						return nullptr;
					}
					l->append(p);
				}
				break;
				case ' ':
				case '\t':
					skipSpaces();
					if((KVSP_curCharUnicode != ')') && (KVSP_curCharUnicode != ','))
					{
						// separate by single spaces
						l->append(new KviKvsTreeNodeConstantData(KVSP_curCharPointer, new KviKvsVariant(QString(" "))));
					}
					else
					{
						goto end_of_function_parameter;
					}
					break;
				case '"':
				{
					// this is a string
					KviKvsTreeNodeData * p = parseStringParameter();
					if(!p)
					{
						// this is an error
						delete l;
						return nullptr;
					}
					l->append(p);
				}
				break;
				default:
				{
					// anything else is a literal
					l->append(parseCommaSeparatedLiteralParameter());
				}
				break;
			}
		}
	end_of_function_parameter:
		if(l->count() > 1)
		{
			// complex parameter needed
			KviKvsTreeNodeData * p = new KviKvsTreeNodeCompositeData(pBegin, l);
			p->setEndingLocation(KVSP_curCharPointer);
			return p;
		}
		else
		{
			// a single parameter in the list, or no params at all
			l->setAutoDelete(false);
			KviKvsTreeNodeData * p = l->first();
			delete l;
			if(!p)
				p = new KviKvsTreeNodeConstantData(KVSP_curCharPointer, new KviKvsVariant());
			p->setEndingLocation(KVSP_curCharPointer);
			return p;
		}
		// never reached
		return nullptr;
	}

	KviKvsTreeNodeData * KviKvsParser::parseSingleParameterInParenthesis()
	{
		KviPointerList<KviKvsTreeNodeData> * l = new KviPointerList<KviKvsTreeNodeData>;
		l->setAutoDelete(true);

		const QChar * pBegin = KVSP_curCharPointer;

		for(;;)
		{
			switch(KVSP_curCharUnicode)
			{
				case ')':
					// not a part of a parameter
					KVSP_skipChar;
					goto end_of_function_parameter;
					break;
				case 0:
				case '\r':
				case '\n':
					// not a part of a parameter
					goto end_of_function_parameter;
					break;
				case '$':
				case '%':
				case '@':
				{
					// this may be a data reference
					KviKvsTreeNodeData * p = parseParameterPercentOrDollar();
					if(!p)
					{
						// this is an error
						delete l;
						return nullptr;
					}
					l->append(p);
				}
				break;
				case ' ':
				case '\t':
					skipSpaces();
					if((KVSP_curCharUnicode != ')') && (KVSP_curCharUnicode != ','))
					{
						// separate by single spaces
						l->append(new KviKvsTreeNodeConstantData(KVSP_curCharPointer, new KviKvsVariant(QString(" "))));
					}
					else
					{
						goto end_of_function_parameter;
					}
					break;
				case '"':
				{
					// this is a string
					KviKvsTreeNodeData * p = parseStringParameter();
					if(!p)
					{
						// this is an error
						delete l;
						return nullptr;
					}
					l->append(p);
				}
				break;
				default:
				{
					// anything else is a literal
					l->append(parseSingleLiteralParameterInParenthesis());
				}
				break;
			}
		}
	end_of_function_parameter:
		if(l->count() > 1)
		{
			// complex parameter needed
			KviKvsTreeNodeData * p = new KviKvsTreeNodeCompositeData(pBegin, l);
			p->setEndingLocation(KVSP_curCharPointer);
			return p;
		}
		else
		{
			// a single parameter in the list or list empty at all
			l->setAutoDelete(false);
			KviKvsTreeNodeData * p = l->first();
			if(p)
				p->setEndingLocation(KVSP_curCharPointer);
			delete l;
			return p;
		}
		// never reached
		return nullptr;
	}

	KviKvsTreeNodeData * KviKvsParser::parseStringParameter()
	{
		KVSP_ASSERT(KVSP_curCharUnicode == '"');

		KviPointerList<KviKvsTreeNodeData> * l = new KviPointerList<KviKvsTreeNodeData>();
		l->setAutoDelete(true);

		const QChar * pBegin = KVSP_curCharPointer;

		KVSP_skipChar;

		for(;;)
		{
			switch(KVSP_curCharUnicode)
			{
				case 0:
					delete l;
					warning(pBegin, __tr2qs_ctx("Unterminated string constant", "kvs"));
					error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of script in string constant (missing \" character?)", "kvs"));
					return nullptr;
					break;
				case '\r':
				case '\n':
					delete l;
					warning(pBegin, __tr2qs_ctx("Unterminated string constant", "kvs"));
					error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of line in string constant (missing \" character or unescaped newline)", "kvs"));
					return nullptr;
					break;
				case '$':
				case '%':
				case '@':
				{
					// this may be a data reference
					KviKvsTreeNodeData * p = parseParameterPercentOrDollar();
					if(!p)
					{
						// this is an error
						delete l;
						return nullptr;
					}
					l->append(p);
				}
				break;
				case '"':
				{
					// end of the string
					KVSP_skipChar;
					goto end_of_the_string;
				}
				break;
				default:
				{
					// anything else is a literal
					l->append(parseStringLiteralParameter());
				}
				break;
			}
		}
	end_of_the_string:
		if(l->count() > 1)
		{
			// complex parameter needed
			// it is also an implicit string cast
			return new KviKvsTreeNodeCompositeData(pBegin, l);
		}
		else
		{
			if(l->count() > 0)
			{
				// a single parameter in the list
				// we need an explicit string cast here (it is the most common cast)
				l->setAutoDelete(false);
				KviKvsTreeNodeData * p = l->first();
				delete l;
				return new KviKvsTreeNodeStringCast(pBegin, p);
			}
			else
			{
				// no parameters at all.. return straight empty string (no need to cast)
				delete l;
				return new KviKvsTreeNodeConstantData(pBegin, new KviKvsVariant(new QString()));
			}
		}
		// never reached
		return nullptr;
	}

	KviKvsTreeNodeData * KviKvsParser::parseCommandParameter(bool bPreferNumeric)
	{
		KviPointerList<KviKvsTreeNodeData> * l = new KviPointerList<KviKvsTreeNodeData>;
		l->setAutoDelete(true);

		bool bGotLiteral = false;

		const QChar * pBegin = KVSP_curCharPointer;

		for(;;)
		{
			switch(KVSP_curCharUnicode)
			{
				case 0:
				case ' ':
				case '\t':
				case '\r':
				case '\n':
				case ';':
					// not a part of a parameter
					goto jumpout;
					break;
				case '$':
				case '%':
				case '@':
				{
					// this may be a data reference
					KviKvsTreeNodeData * p = parseParameterPercentOrDollar();
					if(!p)
					{
						// this is an error
						delete l;
						return nullptr;
					}
					l->append(p);
				}
				break;
				case '"':
				{
					// this is a string
					KviKvsTreeNodeData * p = parseStringParameter();
					if(!p)
					{
						// this is an error
						delete l;
						return nullptr;
					}
					l->append(p);
				}
				break;
				default:
				{
					bGotLiteral = true;
					// anything else is a literal
					l->append(parseCommandLiteralParameter());
				}
				break;
			}
		}
	jumpout:
		if(l->count() > 1)
		{
			// complex parameter needed
			KviKvsTreeNodeData * p = new KviKvsTreeNodeCompositeData(pBegin, l);
			p->setEndingLocation(KVSP_curCharPointer);
			return p;
		}
		else
		{
			// a single parameter in the list or empty list at all
			l->setAutoDelete(false);
			KviKvsTreeNodeData * p = l->first();
			delete l;
			if(p)
			{
				if(bGotLiteral)
				{
					// a single literal parameter
					if(bPreferNumeric)
					{
						// attempt to convert to a numeric format if this is a constant data item
						p->convertStringConstantToNumeric();
					}
				}
				p->setEndingLocation(KVSP_curCharPointer);
			}
			return p;
		}
		// never reached
		return nullptr;
	}
