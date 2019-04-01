//=============================================================================
//
//   File : KviKvsParser_specialCommands.cpp
//   Creation date : Thu 06 Now 2003 14.14 CEST by Szymon Stefanek
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
#include "KviKvsParserMacros.h"
#include "KviKvsObjectFunctionHandler.h"
#include "KviLocale.h"
#include "KviCommandFormatter.h"

/*
in fact this is not a fully special command
it is special only in the sense of parsing.
Once parsed, the command is routed to the perl module
with the interpreter code as FIRST parameter and the other parameters
of the command following.
the help page for perl.begin is in the perl module
the help page for python.begin is in the python module

perl.begin(context) <perl code> perl.end
python.begin <python code> python.end
*/
#define IMPLEMENT_EXTERNAL_INTERPRETER_BEGIN(__name)                                                                      \
	KviKvsTreeNodeCommand * KviKvsParser::parseSpecialCommand##__name##Begin()                                            \
	{                                                                                                                     \
		const QChar * pBegin = KVSP_curCharPointer;                                                                       \
                                                                                                                          \
		skipSpaces();                                                                                                     \
		KviKvsTreeNodeDataList * dl;                                                                                      \
		if(KVSP_curCharUnicode == '(')                                                                                    \
		{                                                                                                                 \
			dl = parseCommaSeparatedParameterList();                                                                      \
			if(!dl)                                                                                                       \
				return nullptr;                                                                                           \
		}                                                                                                                 \
		else                                                                                                              \
		{                                                                                                                 \
			dl = new KviKvsTreeNodeDataList(pBegin);                                                                      \
		}                                                                                                                 \
                                                                                                                          \
		if(!KVSP_curCharIsEndOfBuffer)                                                                                    \
			KVSP_skipChar;                                                                                                \
                                                                                                                          \
		if(!skipSpacesAndNewlines())                                                                                      \
		{                                                                                                                 \
			delete dl;                                                                                                    \
			return nullptr;                                                                                               \
		}                                                                                                                 \
                                                                                                                          \
		/* allow a ';' after [interpreter].begin */                                                                       \
		if(KVSP_curCharIsEndOfCommand && !KVSP_curCharIsEndOfBuffer)                                                      \
		{                                                                                                                 \
			KVSP_skipChar;                                                                                                \
			if(!skipSpacesAndNewlines())                                                                                  \
			{                                                                                                             \
				delete dl;                                                                                                \
				return nullptr;                                                                                           \
			}                                                                                                             \
		}                                                                                                                 \
                                                                                                                          \
		const QChar * pInterpreterBegin = KVSP_curCharPointer;                                                            \
		QString szName;                                                                                                   \
		szName.sprintf("%s", #__name);                                                                                    \
		szName = szName.toLower();                                                                                        \
                                                                                                                          \
		/* now look for [interpreter].end[terminator] */                                                                  \
		QString szInterpreterEnd(szName);                                                                                 \
		szInterpreterEnd += ".end";                                                                                       \
		const QChar * pInterpreterEnd;                                                                                    \
		for(;;)                                                                                                           \
		{                                                                                                                 \
			while(KVSP_curCharUnicode && (KVSP_curCharUnicode != 'p') && (KVSP_curCharUnicode != 'P'))                    \
				KVSP_skipChar;                                                                                            \
			if(KVSP_curCharIsEndOfBuffer)                                                                                 \
			{                                                                                                             \
				delete dl;                                                                                                \
				QString szErr = "Unexpected end of command buffer while looking for the ";                                \
				szErr += szInterpreterEnd;                                                                                \
				szErr += " statement";                                                                                    \
                                                                                                                          \
				error(KVSP_curCharPointer, __tr2qs_ctx(szErr.toUtf8().data(), "kvs"));                                    \
				return nullptr;                                                                                           \
			}                                                                                                             \
			pInterpreterEnd = KVSP_curCharPointer;                                                                        \
                                                                                                                          \
			if(KviQString::equalCIN(szInterpreterEnd, KVSP_curCharPointer, szInterpreterEnd.size()))                      \
			{                                                                                                             \
				KVSP_skipNChars(szInterpreterEnd.size());                                                                 \
				if(KVSP_curCharIsEndOfCommand || (KVSP_curCharUnicode == ' ') || (KVSP_curCharUnicode == '\t'))           \
				{                                                                                                         \
					/* yeah! */                                                                                           \
					QString szInterpreter(pInterpreterBegin, pInterpreterEnd - pInterpreterBegin);                        \
					dl->prependItem(new KviKvsTreeNodeConstantData(pInterpreterBegin, new KviKvsVariant(szInterpreter))); \
					while(!KVSP_curCharIsEndOfCommand)                                                                    \
						KVSP_skipChar;                                                                                    \
					if(!KVSP_curCharIsEndOfBuffer)                                                                        \
						KVSP_skipChar;                                                                                    \
					break;                                                                                                \
				}                                                                                                         \
				else                                                                                                      \
				{                                                                                                         \
					KVSP_backNChars(szInterpreterEnd.size() - 1);                                                         \
				}                                                                                                         \
			}                                                                                                             \
			else                                                                                                          \
			{                                                                                                             \
				KVSP_skipChar;                                                                                            \
			}                                                                                                             \
		}                                                                                                                 \
                                                                                                                          \
		return new KviKvsTreeNodeModuleSimpleCommand(pBegin, szName, "begin", dl);                                        \
	}

IMPLEMENT_EXTERNAL_INTERPRETER_BEGIN(Perl)
IMPLEMENT_EXTERNAL_INTERPRETER_BEGIN(Python)

KviKvsTreeNodeCommand * KviKvsParser::parseSpecialCommandBreak()
{
	/*
		@doc: break
		@type:
			command
		@title:
			break
		@syntax:
			break
		@short:
			Interrupts an iteration loop
		@description:
			Interrupts an iteration loop like [cmd]while[/cmd].[br]
			This command always jumps out of a single code block.[br]
			If called outside an iteration loop, will act just like [cmd]halt[/cmd]
			has been called but has no additional semantics for events.[br]
	*/
	const QChar * pBegin = KVSP_curCharPointer; // FIXME: this is not accurate at all : it may be even the end of the cmd
	skipSpaces();
	if(!KVSP_curCharIsEndOfCommand)
	{
		warning(KVSP_curCharPointer, __tr2qs_ctx("Trailing garbage at the end of the break command: ignored", "kvs"));
	}

	while(!KVSP_curCharIsEndOfCommand)
		KVSP_skipChar;
	if(!KVSP_curCharIsEndOfBuffer)
		KVSP_skipChar;
	return new KviKvsTreeNodeSpecialCommandBreak(pBegin);
}

KviKvsTreeNodeCommand * KviKvsParser::parseSpecialCommandContinue()
{
	/*
		@doc: continue
		@type:
			command
		@title:
			continue
		@syntax:
			continue
		@short:
			Continues an iteration loop
		@description:
			Continues an iteration loop like [cmd]while[/cmd].[br]
			This command always jumps to the next iteration in the code block.[br]
	*/
	const QChar * pBegin = KVSP_curCharPointer; // FIXME: this is not accurate at all : it may be even the end of the cmd
	skipSpaces();
	if(!KVSP_curCharIsEndOfCommand)
	{
		warning(KVSP_curCharPointer, __tr2qs_ctx("Trailing garbage at the end of the continue command: ignored", "kvs"));
	}

	while(!KVSP_curCharIsEndOfCommand)
		KVSP_skipChar;
	if(!KVSP_curCharIsEndOfBuffer)
		KVSP_skipChar;
	return new KviKvsTreeNodeSpecialCommandContinue(pBegin);
}

KviKvsTreeNodeCommand * KviKvsParser::parseSpecialCommandUnset()
{
	/*
		@doc: unset
		@type:
			command
		@title:
			unset
		@syntax:
			unset <variable_list>
		@keyterms:
			unsetting variables
		@short:
			Unsets a set of variables
		@description:
			Unsets the specified list of comma separated variables.
			It is equivalent to assigning the default empty value
			to each variable on its own: just does it all at once.
			Note that KVIrc automatically frees the local variable memory
			when they go out of scope and the global variable memory
			when KVIrc terminates.
		@examples:
			[example]
				%a = pippo
				%b = 1
				[cmd]echo[/cmd] %a %b
				unset %a %b
				[cmd]echo[/cmd] %a %b
			[/example]
	*/

	const QChar * pCmdBegin = KVSP_curCharPointer;

	KviPointerList<KviKvsTreeNodeVariable> * pVarList = new KviPointerList<KviKvsTreeNodeVariable>;
	pVarList->setAutoDelete(true);

	while(KVSP_curCharUnicode == '%')
	{
		KviKvsTreeNodeVariable * d = parsePercent();
		if(!d)
		{
			delete pVarList;
			return nullptr;
		}

		pVarList->append(d);

		skipSpaces();

		if(KVSP_curCharUnicode == ',')
		{
			KVSP_skipChar;
			skipSpaces();
		}
	}

	if(!KVSP_curCharIsEndOfCommand)
	{
		delete pVarList;
		warning(KVSP_curCharPointer, __tr2qs_ctx("The 'unset' command needs a variable list", "kvs"));
		error(KVSP_curCharPointer, __tr2qs_ctx("Found character %q (Unicode %x) where a variable was expected", "kvs"), KVSP_curCharPointer, KVSP_curCharUnicode);
		return nullptr;
	}

	if(!KVSP_curCharIsEndOfBuffer)
		KVSP_skipChar;

	if(pVarList->count() < 1)
	{
		delete pVarList;
		warning(KVSP_curCharPointer, __tr2qs_ctx("'unset' command used without a variable list", "kvs"));
		return nullptr; // null unset ?
	}
	return new KviKvsTreeNodeSpecialCommandUnset(pCmdBegin, pVarList);
}

KviKvsTreeNodeCommand * KviKvsParser::parseSpecialCommandGlobal()
{
	/*
		@doc: global
		@type:
			command
		@title:
			global
		@syntax:
			global <variable_list>
		@keyterms:
			explicitly declaring global variables
		@short:
			Explicitly declares global variables
		@description:
			Declares a list of global variables.
			Once a variable has been declared as global
			it refers to the global KVIrc instance for the scope of the script.[br]
			Global variables are shared between scripts and keep their
			value until they are explicitly [cmd]unset[/cmd] or KVIrc quits.[br]
			This command can be used to override the default behaviour of
			declaring global variables by starting them with an uppercase letter
			and declaring local variables by starting them with a lowercase one.[br][br]
			In any particular instance instance where a global variable may interfere with desired operation,
			you can specifically [b][cmd]unset[/cmd][/b] the desired variable.
		@examples:
			[example]
				global %a, %b, %c;
			[/example]
		@seealso:
			[fnc]$global[/fnc]
			[cmd]unset[/cmd]
			[fnc]$hash[/fnc]
	*/

	while(KVSP_curCharUnicode == '%')
	{
		KVSP_skipChar;
		const QChar * pBegin = KVSP_curCharPointer;

		while((KVSP_curCharIsLetterOrNumber) || (KVSP_curCharUnicode == '_'))
			KVSP_skipChar;

		QString szIdentifier(pBegin, KVSP_curCharPointer - pBegin);

		if(!m_pGlobals)
		{
			m_pGlobals = new KviPointerHashTable<QString, QString>(17, false);
			m_pGlobals->setAutoDelete(true);
		}
		m_pGlobals->replace(szIdentifier, new QString());

		skipSpaces();

		if(KVSP_curCharUnicode == ',')
		{
			KVSP_skipChar;
			skipSpaces();
		}
	}

	if(!KVSP_curCharIsEndOfCommand)
	{
		warning(KVSP_curCharPointer, __tr2qs_ctx("The 'global' command needs a list of variables", "kvs"));
		error(KVSP_curCharPointer, __tr2qs_ctx("Found character %q (Unicode %x) where a variable was expected", "kvs"), KVSP_curCharPointer, KVSP_curCharUnicode);
		return nullptr;
	}

	if(!KVSP_curCharIsEndOfBuffer)
		KVSP_skipChar;
	return nullptr;
}

KviKvsTreeNodeCommand * KviKvsParser::parseSpecialCommandClass()
{
	/*
		@doc: class
		@title:
			class
		@short:
			Defines a new object class
		@keyterms:
			defining an object class
		@type:
			command
		@syntax:
			class(<classname:string>[,<base_class_name:string>])
			{
				[internal] [function] <function_name>[([<parameter reminder>])]
				{
					<function body>
				}

				...
			}
		@description:
			Defines a new implementation of the class <classname>.
			If an implementation of that class was already existing
			it is removed with all the derived classes (and all the instances of this class
			and the derived ones are destroyed).
			<base_class_name> is the name of the class that the
			new class has to inherit from.[br]
			If <base_class_name> is omitted, the new class inherits automatically
			from [class:object]object[/class].[br]
			Note:[br]
			The keywords "function" and "event" that were used in KVIrc versions
			previous to 3.0.0 have been removed since they are "useless".[br]
			The function keyword, however, is still permitted.
			The keyword "internal" is useful when you want to hide
			certain function from the outside world. An internal function
			cannot be called by anyone else but the object instance itself. Note that
			this is different from the C++ "protected" or "private" keywords
			that refer to the object's class instead of the object instance.
			The <parameter reminder> part is an optional string
			that can be used to sign the parameters that the function expects;
			it acts as a programmer reminder or comment and it has no other
			meaning in KVIrc scripting. The <parameter reminder> respects the syntax
			of an expression, so it is terminated by a closed parenthesis.
			It's rather dangerous to use this command inside an object
			function handler: if the class definition <class> was already
			existing and it is a parent of the object's class, you might
			end up executing "non-existent" code.[br]
			As a thumb rule, use this command only outside object function handlers.[br]
			[br][br]
			Only for the curious: implementing protected and private access
			list on members would have a considerable runtime overhead because
			of the strange nature of the KVS language. Object member calls
			are resolved completely at runtime (and that permits a lot of funny tricks
			like [cmd]privateimpl[/cmd]) but unfortunately this also forces us
			to check access lists at runtime. OK, this would be a relatively small footprint for the "private"
			keyword where we need to run UP the called object inheritance hierarchy
			but would have a significant performance footprint for the "protected"
			keyword where we would need to traverse the [b]whole[/b] inheritance tree of the called and calling
			objects... "internal" still allows hiding members in a lot of situations
			and is really fast to verify at runtime: no inheritance tree traversal
			is needed and only object pointers are compared.
		@examples:
			[example]
				class(myclass,[class]object[/class])
				{
					constructor
					{
						[cmd]echo[/cmd] Hey this is my constructor
						[cmd]echo[/cmd] I have been just created
					}

					destructor
					{
						[cmd]echo[/cmd] Oops! being destroyed
					}

					sayHello(this function expects no parameters)
					{
						[cmd]echo[/cmd] Hello world!
					}
				}
			[/example]
		@seealso:
			[cmd]privateimpl[/cmd], [cmd]killclass[/cmd], [cmd]clearobjects[/cmd], [fnc]$classDefined[/fnc](),
			[doc:objects]Objects documentation[/doc]
	*/

	if(KVSP_curCharUnicode != '(')
	{
		error(KVSP_curCharPointer, __tr2qs_ctx("Found character %q (Unicode %x) where an open parenthesis was expected", "kvs"), KVSP_curCharPointer, KVSP_curCharUnicode);
		return nullptr;
	}

	const QChar * pBegin = KVSP_curCharPointer;

	KviKvsTreeNodeDataList * l = parseCommaSeparatedParameterList();
	if(!l)
		return nullptr;

	KviKvsTreeNodeSpecialCommandClass * pClass = new KviKvsTreeNodeSpecialCommandClass(pBegin, l);

	if(!skipSpacesAndNewlines())
	{
		delete pClass;
		return nullptr;
	}

	if(KVSP_curCharUnicode != '{')
	{
		errorBadChar(KVSP_curCharPointer, '{', "class");
		delete pClass;
		return nullptr;
	}

	KVSP_skipChar;

	if(!skipSpacesAndNewlines())
	{
		delete pClass;
		return nullptr;
	}

	while(KVSP_curCharUnicode != '}')
	{
		if((KVSP_curCharUnicode == '#') || (KVSP_curCharUnicode == '/'))
		{
			parseComment();
			if(error())
			{
				delete pClass;
				return nullptr;
			}
			if(!skipSpacesAndNewlines())
			{
				delete pClass;
				return nullptr;
			}
			continue;
		}

		const QChar * pLabelBegin = KVSP_curCharPointer;

		if(KVSP_curCharIsLetter)
		{
			KVSP_skipChar;
			while(KVSP_curCharIsLetterOrNumber)
				KVSP_skipChar;
		}

		if(KVSP_curCharIsEndOfBuffer)
		{
			error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of buffer in class definition", "kvs"));
			delete pClass;
			return nullptr;
		}

		if(KVSP_curCharPointer == pLabelBegin)
		{
			error(KVSP_curCharPointer, __tr2qs_ctx("Found character %q (Unicode %x) where a function name was expected", "kvs"), KVSP_curCharPointer, KVSP_curCharUnicode);
			delete pClass;
			return nullptr;
		}

		QString szLabel(pLabelBegin, KVSP_curCharPointer - pLabelBegin);

		unsigned int uHandlerFlags = 0;

		if(szLabel.toLower() == "internal")
		{
			uHandlerFlags |= KviKvsObjectFunctionHandler::Internal;
			skipSpaces();
			if(KVSP_curCharUnicode != '(')
			{
				pLabelBegin = KVSP_curCharPointer;

				while(KVSP_curCharIsLetterOrNumber)
					KVSP_skipChar;

				if(KVSP_curCharIsEndOfBuffer)
				{
					error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of buffer in class definition", "kvs"));
					delete pClass;
					return nullptr;
				}

				if(KVSP_curCharPointer == pLabelBegin)
				{
					error(KVSP_curCharPointer, __tr2qs_ctx("Found character %q (Unicode %x) where a function name was expected", "kvs"), KVSP_curCharPointer, KVSP_curCharUnicode);
					delete pClass;
					return nullptr;
				}
				szLabel = QString(pLabelBegin, KVSP_curCharPointer - pLabelBegin);
			}
		}

		if(szLabel.toLower() == "function")
		{
			skipSpaces();
			if(KVSP_curCharUnicode != '(')
			{
				pLabelBegin = KVSP_curCharPointer;

				while(KVSP_curCharIsLetterOrNumber)
					KVSP_skipChar;

				if(KVSP_curCharIsEndOfBuffer)
				{
					error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of buffer in class definition", "kvs"));
					delete pClass;
					return nullptr;
				}

				if(KVSP_curCharPointer == pLabelBegin)
				{
					error(KVSP_curCharPointer, __tr2qs_ctx("Found character %q (Unicode %x) where a function name was expected", "kvs"), KVSP_curCharPointer, KVSP_curCharUnicode);
					delete pClass;
					return nullptr;
				}
				szLabel = QString(pLabelBegin, KVSP_curCharPointer - pLabelBegin);
			}
		}

		if(!skipSpacesAndNewlines())
		{
			delete pClass;
			return nullptr;
		}
		QString szReminder;
		if(KVSP_curCharUnicode == '(')
		{
			KVSP_skipChar;
			const QChar * pReminderBegin = KVSP_curCharPointer;
			while((!(KVSP_curCharIsEndOfBuffer)) && (KVSP_curCharUnicode != ')'))
				KVSP_skipChar;

			if(KVSP_curCharIsEndOfBuffer)
			{
				error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of buffer in function parameter list reminder", "kvs"));
				delete pClass;
				return nullptr;
			}
			szReminder = QString(pReminderBegin, KVSP_curCharPointer - pReminderBegin);
			KVSP_skipChar;

			if(!skipSpacesAndNewlines())
			{
				delete pClass;
				return nullptr;
			}
		}

		if(KVSP_curCharIsEndOfBuffer)
		{
			error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of buffer in class definition", "kvs"));
			delete pClass;
			return nullptr;
		}

		if(KVSP_curCharUnicode != '{')
		{
			errorBadChar(KVSP_curCharPointer, '{', "class");
			delete pClass;
			return nullptr;
		}

		pBegin = KVSP_curCharPointer;
		KviKvsTreeNodeInstruction * pInstruction = parseInstruction();
		if(!pInstruction)
		{
			// may be an empty instruction
			if(error())
			{
				delete pClass;
				return nullptr;
			}
		}
		delete pInstruction;
		int iLen = KVSP_curCharPointer - pBegin;
		QString szInstruction;
		if(iLen > 0)
		{
			szInstruction = QString(pBegin, KVSP_curCharPointer - pBegin);
			KviCommandFormatter::bufferFromBlock(szInstruction);
		}

		pClass->addFunctionDefinition(new KviKvsTreeNodeSpecialCommandClassFunctionDefinition(pLabelBegin, szLabel, szInstruction, szReminder, uHandlerFlags));

		if(!skipSpacesAndNewlines())
		{
			delete pClass;
			return nullptr;
		}
	}

	KVSP_skipChar;

	return pClass;
}

KviKvsTreeNodeCommand * KviKvsParser::parseSpecialCommandWhile()
{
	/*
		@doc: while
		@type:
			command
		@title:
			while
		@syntax:
			while (<condition>) <command>
		@keyterms:
			iteration commands, flow control commands
		@short:
			Iteration command
		@description:
			Executes <command> while the <condition> evaluates
			to true (non zero result).[br]
			<command> may be either a single command or a block of commands.[br]
			It can contain the [cmd]break[/cmd] command: in that case the
			execution of the <command> will be immediately interrupted and the control
			transferred to the command following this while block.[br]
			It is valid for <command> to be an empty command terminated with a [b];[/b].
			<condition> is an expression as the ones evaluated by [doc:expressioneval]$(*)[/doc]
			with the following extensions:[br]
			If <condition> is a string, its length is evaluated: in this way a non-empty string
			causes the <condition> to be true, an empty string causes it to be false.[br]
			If <condition> is an array, its size is evaluated: in this way a non-empty array
			causes the <condition> to be true, an empty array causes it to be false.[br]
			If <condition> is a hash, the number of its entries is evaluated: in this way a non-empty hash
			causes the <condition> to be true, an empty hash causes it to be false.[br]
		@examples:
			[example]
				%i = 0;
				while(%i < 100)%i++
				while(%i > 0)
				{
					%i -= 10
					if(%i < 20)break;
				}
				echo %i
			[/example]
	*/

	if(KVSP_curCharUnicode != '(')
	{
		warning(KVSP_curCharPointer, __tr2qs_ctx("The while command needs an expression enclosed in parenthesis", "kvs"));
		error(KVSP_curCharPointer, __tr2qs_ctx("Found character %q (Unicode %x) where an open parenthesis was expected", "kvs"), KVSP_curCharPointer, KVSP_curCharUnicode);
		return nullptr;
	}

	const QChar * pBegin = KVSP_curCharPointer;

	KVSP_skipChar;

	KviKvsTreeNodeExpression * e = parseExpression(')');
	if(!e)
	{
		// must be an error
		return nullptr;
	}

	if(!skipSpacesAndNewlines())
	{
		delete e;
		return nullptr;
	}

	if(KVSP_curCharUnicode == 0)
	{
		warning(pBegin, __tr2qs_ctx("The last while command in the buffer has no conditional instructions: it's senseless", "kvs"));
		warning(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of script while looking for the instruction block of the while command", "kvs"));
	}

	KviKvsTreeNodeInstruction * i = parseInstruction();
	if(!i)
	{
		if(error())
		{
			delete e;
			return nullptr;
		}
	} // else, just an empty instruction

	return new KviKvsTreeNodeSpecialCommandWhile(pBegin, e, i);
}

KviKvsTreeNodeCommand * KviKvsParser::parseSpecialCommandDo()
{
	/*
		@doc: do
		@type:
			command
		@title:
			do
		@syntax:
			do <command> while (<condition>)
		@keyterms:
			iteration commands, flow control commands
		@short:
			Iteration command
		@description:
			Executes <command> once then evaluates the <condition>.
			If <condition> evaluates to true (non zero result) then repeats the execution again.[br]
			<command> may be either a single command or a block of commands.[br]
			It can contain the [cmd]break[/cmd] command: in that case the
			execution of the <command> will be immediately interrupted and the control
			transferred to the command following this while block.[br]
			It is valid for <command> to be an empty command terminated with a ';'.
			<condition> is an expression as the ones evaluated by [doc:expressioneval]$(*)[/doc]
			with the following extensions:[br]
			If <condition> is a string, its length is evaluated: in this way a non-empty string
			causes the <condition> to be true, an empty string causes it to be false.[br]
			If <condition> is an array, its size is evaluated: in this way a non-empty array
			causes the <condition> to be true, an empty array causes it to be false.[br]
			If <condition> is a hash, the number of its entries is evaluated: in this way a non-empty hash
			causes the <condition> to be true, an empty hash causes it to be false.[br]
		@examples:
			[example]
				%i = 0;
				do %i++; while(%i < 100);
				echo "After first execution:  %i";
				%i = 10
				do
				{
					echo "Executed!";
					%i++;
				}
				while(%i < 1)
				echo "After second execution:  %i";
			[/example]
		@seealso:
			[cmd]while[/cmd]
	*/

	const QChar * pBegin = KVSP_curCharPointer;

	KviKvsTreeNodeInstruction * i = parseInstruction();
	if(!i)
	{
		if(error())
			return nullptr;
	}

	if(!skipSpacesAndNewlines())
	{
		if(i)
			delete i;
		return nullptr;
	}

	static const unsigned short while_chars[10] = { 'W', 'w', 'H', 'h', 'I', 'i', 'L', 'l', 'E', 'e' };

	for(int j = 0; j < 10; j++)
	{
		if(KVSP_curCharUnicode != while_chars[j])
		{
			j++;
			if(KVSP_curCharUnicode != while_chars[j])
			{
				if(KVSP_curCharIsEndOfBuffer)
					error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of command after the 'do' command block: expected 'while' keyword", "kvs"));
				else
					error(KVSP_curCharPointer, __tr2qs_ctx("Found character %q (Unicode %x) where a 'while' keyword was expected", "kvs"), KVSP_curCharPointer, KVSP_curCharUnicode);
				if(i)
					delete i;
				return nullptr;
			}
		}
		else
			j++;
		KVSP_skipChar;
	}

	if(!skipSpacesAndNewlines())
	{
		if(i)
			delete i;
		return nullptr;
	}

	if(KVSP_curCharUnicode != '(')
	{
		warning(KVSP_curCharPointer, __tr2qs_ctx("The 'while' block of the 'do' command needs an expression enclosed in parenthesis", "kvs"));
		errorBadChar(KVSP_curCharPointer, '(', "do");
		if(i)
			delete i;
		return nullptr;
	}

	KVSP_skipChar;

	KviKvsTreeNodeExpression * e = parseExpression(')');
	if(!e)
	{
		// must be an error
		if(i)
			delete i;
		return nullptr;
	}

	skipSpaces();

	if(!KVSP_curCharIsEndOfCommand)
	{
		warning(KVSP_curCharPointer, __tr2qs_ctx("Garbage string after the expression in 'do' command: ignored", "kvs"));
		while(!KVSP_curCharIsEndOfCommand)
			KVSP_skipChar;
	}

	if(!KVSP_curCharIsEndOfBuffer)
		KVSP_skipChar;

	return new KviKvsTreeNodeSpecialCommandDo(pBegin, e, i);
}

KviKvsTreeNodeCommand * KviKvsParser::parseSpecialCommandIf()
{
	/*
		@doc: if
		@type:
			command
		@title:
			if
		@syntax:
			if (<condition>) <command1> [else <command2>]
		@keyterms:
			conditional commands, flow control commands
		@short:
			Flow control command
		@description:
			Executes <command1> if the <condition> evaluates to true (non zero result).[br]
			If the [i]else part[/i] is given, <command2> is executed.[br]
			If the <condition> evaluates to false (result == '0').[br]
			the [b]<condition>[/b] is an expression evaluated in the same way as [doc:expressioneval]$(*)[/doc]
			with the following extensions:[br]
			If the [b]<condition>[/b] is a string, its length is evaluated - in this way a non-empty string
			causes the <condition> to be true, and an empty string causes it to be false.[br]
			If the [b]<condition>[/b] is an array, its size is evaluated - a non-empty array is true, an empty array is false.[br]
			If the [b]<condition>[/b] is a hash, the number of its entries is evaluated - a non-empty hash is true, an empty hash is false.[br]
		@examples:
			[example]
				if(%a != 10)
					[cmd]echo[/cmd] \%a was != 10
				else
					[cmd]echo[/cmd] \%a was 10!
			[/example]
		@seealso:
			[doc:expressioneval]Expression evaluation identifier[/doc]
		
	*/

	if(KVSP_curCharUnicode != '(')
	{
		warning(KVSP_curCharPointer, __tr2qs_ctx("The 'if' command needs an expression enclosed in parenthesis", "kvs"));
		errorBadChar(KVSP_curCharPointer, '(', "if");
		return nullptr;
	}

	const QChar * pBegin = KVSP_curCharPointer;

	KVSP_skipChar;

	KviKvsTreeNodeExpression * e = parseExpression(')');
	if(!e)
	{
		// must be an error
		return nullptr;
	}

	if(!skipSpacesAndNewlines())
	{
		delete e;
		return nullptr;
	}

	if(KVSP_curCharUnicode == 0)
	{
		warning(pBegin, __tr2qs_ctx("The last if command in the buffer has no conditional instructions: it's senseless", "kvs"));
		warning(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of script while looking for the instruction block of the if command", "kvs"));
	}

	KviKvsTreeNodeInstruction * i = parseInstruction();
	if(!i)
	{
		if(error())
		{
			delete e;
			return nullptr;
		}
	} // else, just an empty instruction

	if(!skipSpacesAndNewlines())
	{
		if(i)
			delete i;
		return nullptr;
	}

	const QChar * pElse = KVSP_curCharPointer;

	if((KVSP_curCharUnicode != 'e') && (KVSP_curCharUnicode != 'E'))
		return new KviKvsTreeNodeSpecialCommandIf(pBegin, e, i, nullptr);
	KVSP_skipChar;
	if((KVSP_curCharUnicode != 'l') && (KVSP_curCharUnicode != 'L'))
	{
		KVSP_setCurCharPointer(pElse);
		return new KviKvsTreeNodeSpecialCommandIf(pBegin, e, i, nullptr);
	}
	KVSP_skipChar;
	if((KVSP_curCharUnicode != 's') && (KVSP_curCharUnicode != 'S'))
	{
		KVSP_setCurCharPointer(pElse);
		return new KviKvsTreeNodeSpecialCommandIf(pBegin, e, i, nullptr);
	}
	KVSP_skipChar;
	if((KVSP_curCharUnicode != 'e') && (KVSP_curCharUnicode != 'E'))
	{
		KVSP_setCurCharPointer(pElse);
		return new KviKvsTreeNodeSpecialCommandIf(pBegin, e, i, nullptr);
	}
	KVSP_skipChar;
	if(KVSP_curCharIsLetterOrNumber)
	{
		if((KVSP_curCharUnicode == 'i') || (KVSP_curCharUnicode == 'I'))
		{
			KVSP_skipChar;
			if((KVSP_curCharUnicode == 'f') || (KVSP_curCharUnicode == 'F'))
			{
				KVSP_skipChar;
				if(!KVSP_curCharIsLetterOrNumber)
				{
					// this is an "elseif"
					KVSP_backChar;
					KVSP_backChar;
					// point to if
					goto handle_else_instruction;
				}
				KVSP_backChar;
			}
			KVSP_backChar;
		}

		KVSP_setCurCharPointer(pElse);
		return new KviKvsTreeNodeSpecialCommandIf(pBegin, e, i, nullptr);
	}

handle_else_instruction:
	if(!skipSpacesAndNewlines())
	{
		delete e;
		if(i)
			delete i;
		return nullptr;
	}

	KviKvsTreeNodeInstruction * i2 = parseInstruction();
	if(!i2)
	{
		if(error())
		{
			delete e;
			if(i)
				delete i;
			return nullptr;
		}
	} // else, just an empty instruction

	return new KviKvsTreeNodeSpecialCommandIf(pBegin, e, i, i2);
}

bool KviKvsParser::skipToEndOfForControlBlock()
{
	bool bInString = false;
	int iParLevel = 0;

	for(;;)
	{
		switch(KVSP_curCharUnicode)
		{
			case '"':
				bInString = !bInString;
				KVSP_skipChar;
				break;
			case '(':
				if(!bInString)
					iParLevel++;
				KVSP_skipChar;
				break;
			case ')':
				if(!bInString)
				{
					if(iParLevel == 0)
						return true;
					else
						iParLevel--;
				}
				KVSP_skipChar;
				break;
			case 0:
				error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of buffer while looking for the closing ')' in the 'for' command", "kvs"));
				return false;
				break;
			//case '\n':
			// that's ok.. it may have a parenthesis on the next line
			//KVSP_skipChar;
			//break;
			default:
				KVSP_skipChar;
				break;
		}
	}
	// not reached
	KVSP_ASSERT(false);
	return false;
}

KviKvsTreeNodeCommand * KviKvsParser::parseSpecialCommandFor()
{
	/*
		@doc: for
		@type:
			command
		@title:
			for
		@syntax:
			for (<initialization>;<condition>;<update>) <command>
		@keyterms:
			iterational control commands
		@short:
			Iteration control command
		@description:
			Executes <initialization> once then runs the following iteration loop:
			if <condition> evaluates to true then <command> is executed followed
			by the <update> command. The iteration is repeated until <condition> evaluates to false.[br]
			<condition> is an expression as the ones evaluated by [doc:expressioneval]$(*)[/doc]
			with the following extensions:[br]
			If <condition> is a string, its length is evaluated: in this way a non-empty string
			causes the <condition> to be true, an empty string causes it to be false.[br]
			If <condition> is an array, its size is evaluated: in this way a non-empty array
			causes the <condition> to be true, an empty array causes it to be false.[br]
			If <condition> is a hash, the number of its entries is evaluated: in this way a non-empty hash
			causes the <condition> to be true, an empty hash causes it to be false.[br]
		@examples:
			[example]
				for(%a = 0;%a < 100;%a++)
					echo %a
			[/example]
	*/

	if(KVSP_curCharUnicode != '(')
	{
		warning(KVSP_curCharPointer, __tr2qs_ctx("The 'for' command needs an expression enclosed in parenthesis", "kvs"));
		errorBadChar(KVSP_curCharPointer, '(', "for");
		return nullptr;
	}

	const QChar * pForBegin = KVSP_curCharPointer;

	KVSP_skipChar;

	skipSpaces();

	KviKvsTreeNodeInstruction * i1 = parseInstruction();
	if(!i1)
	{
		if(error())
			return nullptr;
	} // else just empty instruction

	skipSpaces();

	KviKvsTreeNodeExpression * e = parseExpression(';');
	if(!e)
	{
		if(error())
		{
			if(i1)
				delete i1;
			return nullptr;
		}
	} // else just empty expression : assume true

	skipSpaces();

	// skip to the first non matching ')' that is not in a string

	const QChar * pBegin = KVSP_curCharPointer;

	if(!skipToEndOfForControlBlock())
	{
		if(error()) // <-- that's always true
		{
			if(i1)
				delete i1;
			if(e)
				delete e;
			return nullptr;
		}
	}

	// HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK
	// KVSP_curCharPointer is const!
	// we shouldn't be able to modify it
	QChar cSave = *(KVSP_curCharPointer);

	QChar * pHack = (QChar *)KVSP_curCharPointer;
	*pHack = QChar('\n');

	KVSP_curCharPointer = pBegin;

	KviKvsTreeNodeInstruction * i2 = parseInstruction();
	*pHack = cSave;

	KVSP_setCurCharPointer(pHack);
	// EOF HACK EOF HACK EOF HACK EOF HACK EOF HACK EOF HACK EOF HACK

	if(!i2)
	{
		if(error())
		{
			if(i1)
				delete i1;
			if(e)
				delete e;
			return nullptr;
		}
	} // else just empty instruction

	skipSpaces();

	if(KVSP_curCharUnicode != ')')
	{
		error(KVSP_curCharPointer, __tr2qs_ctx("Found char %q (Unicode %x) while looking for the terminating ')' in 'for' command", "kvs"), KVSP_curCharPointer, KVSP_curCharUnicode);
		if(i1)
			delete i1;
		if(e)
			delete e;
		if(i2)
			delete i2;
		return nullptr;
	}

	KVSP_skipChar;

	if(!skipSpacesAndNewlines())
	{
		if(i1)
			delete i1;
		if(e)
			delete e;
		if(i2)
			delete i2;
		return nullptr;
	}

	KviKvsTreeNodeInstruction * loop = parseInstruction();
	if(!loop)
	{
		if(error())
		{
			if(i1)
				delete i1;
			if(e)
				delete e;
			if(i2)
				delete i2;
			return nullptr;
		}

		if((!i1) && (!e) && (!i2))
		{
			error(pForBegin, __tr2qs_ctx("Empty infinite 'for' loop: fix the script", "kvs"));
			return nullptr;
		}
	} // else just an empty instruction

	return new KviKvsTreeNodeSpecialCommandFor(pForBegin, i1, e, i2, loop);
}

KviKvsTreeNodeCommand * KviKvsParser::parseSpecialCommandForeach()
{
	/*
		@doc: foreach
		@type:
			command
		@title:
			foreach
		@syntax:
			foreach [-a] (<variable>,[<item>[,<item>[,<item>[...]]]) <command>
		@keyterms:
			iteration commands, flow control commands
		@switches:
			!sw: -a | --all
			Include empty variables in the iteration loop.
		@short:
			Iteration command
		@description:
			Executed <command> while assigning to <variable> each <item>.[br]
			<item> may be a constant, a variable, an array, a dictionary or a function returning
			either a constant string an array reference or a dictionary reference.[br]
			If <item> is an array, a dictionary or a function that returns a dictionary or array reference
			the iteration is done through all the dictionary/array items.[br]
			Please note that the iteration order of dictionary items is undefined.[br]
			You can always break from the loop by using the [cmd]break[/cmd] command,
			or skip to the next item with continue.[br]
			foreach doesn't iterate over empty scalar variables (i.e. the ones set to [fnc]$nothing[/fnc])
			unless you use the -a switch. (Note that an array with *some* empty entries is [b]not[/b] empty so
			the iteration is in fact done).
		@examples:
			[example]
				foreach(%i,1,2,3,4,5,6,7,8,9)[cmd]echo[/cmd] %i
				foreach(%chan,[fnc]$window.list[/fnc](channel))[cmd]me[/cmd] -r=%chan This is a test!
				[comment]#  This will work too, and will do the same job[/comment]
				%windows[] = [fnc]$window.list[/fnc](channel)
				foreach(%chan,%windows[])[cmd]me[/cmd] -r=%chan This is a test!
				[comment]#  And this too[/comment]
				%windows[] = [fnc]$window.list[/fnc](channel)
				foreach(%key,[fnc]$keys[/fnc](%windows[]))[cmd]me[/cmd] -r=%windows[%key] This is a test!
				[comment]#  Another interesting example[/comment]
				[cmd]alias[/cmd](test){ [cmd]return[/cmd] [fnc]$hash[/fnc](1,a,2,b,3,c,4,d); };
				foreach(%x,[fnc]$keys[/fnc]($test)){ [cmd]echo[/cmd] %x, $test{%x}; }
			[/example]
	*/

	if(KVSP_curCharUnicode != '(')
	{
		warning(KVSP_curCharPointer, __tr2qs_ctx("The 'foreach' command needs an expression enclosed in parenthesis", "kvs"));
		errorBadChar(KVSP_curCharPointer, '(', "foreach");
		return nullptr;
	}

	const QChar * pForeachBegin = KVSP_curCharPointer;

	KVSP_skipChar;

	skipSpaces();

	if((KVSP_curCharUnicode != '%') && (KVSP_curCharUnicode != '$') && (KVSP_curCharUnicode != '@'))
	{
		warning(KVSP_curCharPointer, __tr2qs_ctx("The 'foreach' command expects a writable iteration variable as first parameter", "kvs"));
		error(KVSP_curCharPointer, __tr2qs_ctx("Found character '%q' (Unicode %x) where '%' or '$' was expected: see /help foreach for the command syntax", "kvs"), KVSP_curCharPointer, KVSP_curCharUnicode);
		return nullptr;
	}

	KviKvsTreeNodeData * d = parsePercentOrDollar();
	if(!d)
		return nullptr;

	if(d->isFunctionCall() || d->isReadOnly())
	{
		warning(KVSP_curCharPointer, __tr2qs_ctx("The 'foreach' command expects a writable iteration variable as first parameter", "kvs"));
		if(d->isFunctionCall())
			error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected function call as 'foreach' iteration variable", "kvs"));
		else
			error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected read-only variable as 'foreach' iteration variable", "kvs"));
		delete d;
		return nullptr;
	}

	skipSpaces();
	if(KVSP_curCharUnicode != ',')
	{
		if(KVSP_curCharUnicode == ')')
		{
			error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of 'foreach' parameters: at least one iteration data argument must be given", "kvs"));
			delete d;
			return nullptr;
		}
		warning(KVSP_curCharPointer, __tr2qs_ctx("The 'foreach' command expects a comma separated list of iteration data items after the first parameter", "kvs"));
		errorBadChar(KVSP_curCharPointer, ',', "foreach");
		return nullptr;
	}

	KviKvsTreeNodeDataList * l = parseCommaSeparatedParameterList();
	if(!l)
		return nullptr;

	if(!skipSpacesAndNewlines())
	{
		delete d;
		delete l;
		return nullptr;
	}

	const QChar * pLoopBegin = KVSP_curCharPointer;

	KviKvsTreeNodeInstruction * loop = parseInstruction();
	if(!loop)
	{
		if(error())
			return nullptr;
		warning(pLoopBegin, __tr2qs_ctx("Found empty 'foreach' execution block: maybe you need to fix your script?", "kvs"));
		loop = new KviKvsTreeNodeInstructionBlock(pLoopBegin);
	}

	return new KviKvsTreeNodeSpecialCommandForeach(pForeachBegin, d, l, loop);
}

KviKvsTreeNodeCommand * KviKvsParser::parseSpecialCommandSwitch()
{
	/*
		@doc: switch
		@type:
			command
		@title:
			switch
		@syntax:
			switch [-p] (<expression>)
			{
				case(<value>)[:]<command>
				[break]
				case(<value>)[:]<command>
				[break]
				....
				match(<wildcard_expression>)[:]<command>
				[break]
				....
				regexp(<regular_expression>)[:]<command>
				[break]
				....
				case(<value>)[:]<command>
				[break]
				....
				default[:]<command>
				[break]
			}
		@switches:
			!sw: -p | --passthrough
			Use c-style 'break' flow control
		@short:
			Another flow control command
		@description:
			The switch command is based on the standard C 'switch' keyword.
			It conditionally executes groups of commands chosen from a larger set of command groups.[br]
			First <expression> is evaluated (<expression> is any arithmetic or string expression).[br]
			Then the [b]match[/b], [b]regexp[/b], [b]case[/b] and [b]default[/b] labels are evaluated sequentially
			in the order of appearance.[br]
			[b]case(<value>)[:]<command>[/b][br]
			The <value> is evaluated and is compared against the result of <expression>.
			String comparison is case insensitive.[br]
			If <value> is equal to <expression> then <command> is executed.
			Please note that <command> must be either a single instruction or an instruction block [b]enclosed in braces[/b].
			During or after <command> execution, if a [cmd]break[/cmd] statement is encountered the execution of the switch
			is terminated, otherwise the next label is evaluated.[br]
			If the -p (--passthrough) option is enabled, than the switch command will execute all the instructions blocks
			until a [cmd]break[/cmd] statement is found.[br]
			[b]match(<value>)[:]<command>[/b][br]
			The <value> is expected to be a wildcard expression (wildcard characters being [b]*[/b] and [b]?[/b])
			that is matched in a case insensitive fashion against <expression>.[br]
			If a match occurs, the related <command> is executed.
			The [cmd]break[/cmd] statement is treated as in the case label.[br]
			[b]regexp(<value>)[:]<command>[/b][br]
			The <value> is expected to be a complete standard regular expression
			that is matched in a case insensitive fashion against <expression>.[br]
			If a match occurs, the related <command> is executed.
			The [cmd]break[/cmd] statement is treated as in the case label.[br]
			[b]default[:]<command>[/b][br]
			The default label is executed unconditionally if no previous label terminated execution with the [cmd]break[/cmd] statement.[br]
		@examples:
			[comment]# Try to change the 1 below to 2 or 3 to see the results[/comment]
			[example]
				%tmp = 1
				switch(%tmp)
				{
					case(1):
						echo \%tmp was 1!
					break;
					case(2)
						echo \%tmp was 2!
					break;
					default:
						echo \%tmp was not 1 nor 2: it was %tmp!
					break;
				}
			[/example]
			[example]
				[comment]# A more complex example: change the 1 in 2 or 3[/comment]
				%tmp = 1
				switch(%tmp)
				{
					case(1):
						echo \%tmp was 1!
				case(2)
						echo \%tmp was 2!
					break;
					default:
						echo \%tmp was either 1 or something different from 2 (%tmp)
					break;
				}
			[/example]
			[example]
				[comment]# An example of the -p switch[/comment]
				%tmp = 1
				switch -p (%tmp)
				{
					case(1):
						echo \%tmp was 1!
					case(2)
						echo \%tmp was 1 or 2!
						break;
					default:
						echo \%tmp was not 1 or 2 (%tmp)
					break;
				}
			[/example]
			[example]
				[comment]# An example with strings[/comment]
				%tmp = "This is a test"
				%tmp2 = "This is not a test"
				switch(%tmp)
				{
					case(%tmp2)
						echo \%tmp == \%tmp2
						break;
					case(%tmp)
					{
						[comment]# do not break here[/comment]
						echo "Yeah... It's stupid... \%tmp == \%tmp :D"
					}
					match("*TEST"):
						echo "Matched *TEST"
					regexp("[a-zA-Z ]*test"):
						echo "Matched [a-zA-Z ]*text"
					regexp("[a-zA-Z ]*not[a-zA-Z ]*"):
						echo "Matched [a-zA-Z ]*not[a-zA-Z ]*"
					default:
						echo This is executed anyway (unless some break was called)
					break;
				}
			[/example]
*/

	if(KVSP_curCharUnicode != '(')
	{
		warning(KVSP_curCharPointer, __tr2qs_ctx("The 'switch' command needs an expression enclosed in parenthesis", "kvs"));
		errorBadChar(KVSP_curCharPointer, '(', "switch");
		return nullptr;
	}

	const QChar * pBegin = KVSP_curCharPointer;

	KVSP_skipChar;

	KviKvsTreeNodeExpression * e = parseExpression(')');
	if(!e)
	{
		// must be an error
		return nullptr;
	}

	if(!skipSpacesAndNewlines())
	{
		delete e;
		return nullptr;
	}

	if(KVSP_curCharUnicode != '{')
	{
		errorBadChar(KVSP_curCharPointer, '{', "switch");
		delete e;
		return nullptr;
	}

	KVSP_skipChar;

	if(!skipSpacesAndNewlines())
	{
		delete e;
		return nullptr;
	}

	KviKvsTreeNodeSpecialCommandSwitch * pSwitch = new KviKvsTreeNodeSpecialCommandSwitch(pBegin, e);

	KviKvsTreeNodeSpecialCommandSwitchLabel * pLabel = nullptr;

	while(KVSP_curCharUnicode != '}')
	{
		// look for a 'case','match','default' or 'regexpr' label

		const QChar * pLabelBegin = KVSP_curCharPointer;
		while(KVSP_curCharIsLetter)
			KVSP_skipChar;

		if(KVSP_curCharIsEndOfBuffer)
		{
			error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of buffer in switch condition block", "kvs"));
			delete pSwitch;
			return nullptr;
		}

		if(KVSP_curCharPointer == pLabelBegin)
		{
			error(KVSP_curCharPointer, __tr2qs_ctx("Found character %q (Unicode %x) where a 'case','match','regexp','default' or 'break' label was expected", "kvs"), KVSP_curCharPointer, KVSP_curCharUnicode);
			delete pSwitch;
			return nullptr;
		}

		QString szLabel(pLabelBegin, KVSP_curCharPointer - pLabelBegin);
		QString szLabelLow = szLabel.toLower();

		bool bNeedParam = true;

		if(szLabelLow == "case")
		{
			pLabel = new KviKvsTreeNodeSpecialCommandSwitchLabelCase(pLabelBegin);
		}
		else if(szLabelLow == "match")
		{
			pLabel = new KviKvsTreeNodeSpecialCommandSwitchLabelMatch(pLabelBegin);
		}
		else if(szLabelLow == "regexp")
		{
			pLabel = new KviKvsTreeNodeSpecialCommandSwitchLabelRegexp(pLabelBegin);
		}
		else if(szLabelLow == "default")
		{
			pLabel = new KviKvsTreeNodeSpecialCommandSwitchLabelDefault(pLabelBegin);
			bNeedParam = false;
		}
		else if(szLabelLow == "break")
		{
			if(pLabel)
			{
				pLabel->setTerminatingBreak(true);
				skipSpaces();
				if(KVSP_curCharUnicode == ';')
					KVSP_skipChar;
				if(!skipSpacesAndNewlines())
				{
					delete pSwitch;
					delete pLabel;
					return nullptr;
				}
				continue;
			}
			else
			{
				error(pLabelBegin, __tr2qs_ctx("Found 'break' label where a 'case','match','regexp' or 'default' label was expected", "kvs"));
				delete pSwitch;
				return nullptr;
			}
		}
		else
		{
			error(pLabelBegin, __tr2qs_ctx("Found token '%Q' where a 'case','match','regexp','default' or 'break' label was expected", "kvs"), &szLabel);
			delete pSwitch;
			return nullptr;
		}

		if(bNeedParam)
		{
			skipSpaces();
			if(KVSP_curCharUnicode != '(')
			{
				errorBadChar(KVSP_curCharPointer, '(', "switch");
				delete pSwitch;
				delete pLabel;
				return nullptr;
			}
			KVSP_skipChar;

			KviKvsTreeNodeData * pParameter = parseSingleParameterInParenthesis();
			if(!pParameter)
			{
				delete pSwitch;
				delete pLabel;
				return nullptr;
			}

			pLabel->setParameter(pParameter);
		}

		skipSpaces();
		if(KVSP_curCharUnicode == ':')
			KVSP_skipChar;
		if(!skipSpacesAndNewlines())
		{
			delete pSwitch;
			delete pLabel;
			return nullptr;
		}

		KviKvsTreeNodeInstruction * pInstruction = parseInstruction();
		if(!pInstruction)
		{
			// may be an empty instruction
			if(error())
			{
				delete pSwitch;
				delete pLabel;
				return nullptr;
			}
		}

		pLabel->setInstruction(pInstruction);
		pSwitch->addLabel(pLabel);

		if(!skipSpacesAndNewlines())
		{
			delete pSwitch;
			return nullptr;
		}
	}

	KVSP_skipChar;

	if(pSwitch->isEmpty())
	{
		error(pBegin, __tr2qs_ctx("Senseless empty switch command: fix the script", "kvs"));
		delete pSwitch;
		return nullptr;
	}

	return pSwitch;
}

KviKvsTreeNodeSpecialCommandDefpopupLabelPopup * KviKvsParser::parseSpecialCommandDefpopupLabelPopup()
{
	if(KVSP_curCharUnicode != '{')
	{
		errorBadChar(KVSP_curCharPointer, '{', "defpopup");
		return nullptr;
	}

	std::unique_ptr<KviKvsTreeNodeSpecialCommandDefpopupLabelPopup> pPopup{new KviKvsTreeNodeSpecialCommandDefpopupLabelPopup(KVSP_curCharPointer)};

	KVSP_skipChar;

	if(!skipSpacesAndNewlines())
		return nullptr;

	while(KVSP_curCharUnicode != '}')
	{
		// look for 'label', 'prologue', 'epilogue', 'popup', 'item', 'separator', 'separatorid' or 'extpopup' label
		const QChar * pLabelBegin = KVSP_curCharPointer;
		while(KVSP_curCharIsLetter)
			KVSP_skipChar;

		if(KVSP_curCharIsEndOfBuffer)
		{
			error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of buffer in defpopup block", "kvs"));
			return nullptr;
		}

		if(KVSP_curCharPointer == pLabelBegin)
		{
			error(KVSP_curCharPointer, __tr2qs_ctx("Found character %q (Unicode %x) where a 'prologue', 'separator', 'separatorid', 'label', 'popup', 'item', 'extpopup' or 'epilogue' label was expected", "kvs"), KVSP_curCharPointer, KVSP_curCharUnicode);
			return nullptr;
		}

		QString szLabel(pLabelBegin, KVSP_curCharPointer - pLabelBegin);
		QString szLabelLow = szLabel.toLower();

		KviPointerList<QString> * pParameters = nullptr;

		QString szCondition;

#define EXTRACT_POPUP_LABEL_PARAMETERS                      \
	if(!skipSpacesAndNewlines())                            \
		return nullptr;                                     \
	if(KVSP_curCharUnicode != '(')                          \
	{                                                       \
		errorBadChar(KVSP_curCharPointer, '(', "defpopup"); \
		return nullptr;                                     \
	}                                                       \
	pParameters = parseCommaSeparatedParameterListNoTree(); \
	if(!pParameters)                                        \
		return nullptr;

#define EXTRACT_POPUP_LABEL_CONDITION                                  \
	if(!skipSpacesAndNewlines())                                       \
		return nullptr;                                                \
	if(KVSP_curCharUnicode == '(')                                     \
	{                                                                  \
		const QChar * pBegin = KVSP_curCharPointer;                    \
		KVSP_skipChar;                                                 \
		KviKvsTreeNodeExpression * pExpression = parseExpression(')'); \
		if(!pExpression)                                               \
		{                                                              \
			if(pParameters)                                            \
				delete pParameters;                                    \
			return nullptr;                                            \
		}                                                              \
		int cLen = (KVSP_curCharPointer - pBegin) - 2;                 \
		if(cLen > 0)                                                   \
			szCondition.setUnicode(pBegin + 1, cLen);                  \
		delete pExpression;                                            \
		if(!skipSpacesAndNewlines())                                   \
		{                                                              \
			if(pParameters)                                            \
				delete pParameters;                                    \
			return nullptr;                                            \
		}                                                              \
	}

		if((szLabelLow == "prologue") || (szLabelLow == "epilogue"))
		{
			bool bPrologue = (szLabelLow == "prologue");
			if(!skipSpacesAndNewlines())
				return nullptr;

			if(KVSP_curCharUnicode == '(')
			{
				EXTRACT_POPUP_LABEL_PARAMETERS
				if(!skipSpacesAndNewlines())
				{
					if(pParameters)
						delete pParameters;
					return nullptr;
				}
			}
			const QChar * pBegin = KVSP_curCharPointer;
			KviKvsTreeNodeInstruction * pInstruction = parseInstruction();
			if(pInstruction)
			{
				// in fact we don't need it at all, we just need the code buffer...
				delete pInstruction;
			}
			else
			{
				// may be an empty instruction
				if(error())
				{
					// error
					if(pParameters)
						delete pParameters;
					return nullptr;
				}
				// empty instruction
				if(bPrologue)
					warning(pBegin, __tr2qs_ctx("Found empty prologue block: maybe you need to fix the script?", "kvs"));
				else
					warning(pBegin, __tr2qs_ctx("Found empty epilogue block: maybe you need to fix the script?", "kvs"));
			}
			int iLen = KVSP_curCharPointer - pBegin;
			if(iLen > 0)
			{
				QString szInstruction(pBegin, KVSP_curCharPointer - pBegin);
				KviCommandFormatter::bufferFromBlock(szInstruction);
				QString * pItemName = pParameters ? pParameters->first() : nullptr;
				QString szItemName = pItemName ? *pItemName : QString();
				if(bPrologue)
					pPopup->addLabel(new KviKvsTreeNodeSpecialCommandDefpopupLabelPrologue(pLabelBegin, szInstruction, szItemName));
				else
					pPopup->addLabel(new KviKvsTreeNodeSpecialCommandDefpopupLabelEpilogue(pLabelBegin, szInstruction, szItemName));
			} // else the instruction was empty anyway: we don't need it in fact
			if(pParameters)
				delete pParameters;
		}
		else if(szLabelLow == "separator")
		{
			EXTRACT_POPUP_LABEL_CONDITION
			if(KVSP_curCharUnicode == ';')
				KVSP_skipChar;
			QString szItemName = "dummySeparator";
			pPopup->addLabel(new KviKvsTreeNodeSpecialCommandDefpopupLabelSeparator(pLabelBegin, szCondition, szItemName));
		}
		else if(szLabelLow == "separatorid")
		{
			EXTRACT_POPUP_LABEL_PARAMETERS
			EXTRACT_POPUP_LABEL_CONDITION
			if(KVSP_curCharUnicode == ';')
				KVSP_skipChar;
			QString * pItemName = pParameters->first();
			pPopup->addLabel(new KviKvsTreeNodeSpecialCommandDefpopupLabelSeparator(pLabelBegin, szCondition, pItemName ? *pItemName : QString()));
			delete pParameters;
		}
		else if(szLabelLow == "label")
		{
			EXTRACT_POPUP_LABEL_PARAMETERS
			EXTRACT_POPUP_LABEL_CONDITION

			QString * pText = pParameters->first();
			if(!pText)
			{
				error(pLabelBegin, __tr2qs_ctx("Unexpected empty <text> field in label parameters. See /help defpopup for the syntax", "kvs"));
				delete pParameters;
				return nullptr;
			}
			QString * pIcon = pParameters->next();
			if(KVSP_curCharUnicode == ';')
				KVSP_skipChar;
			QString * pItemName = pParameters->next();
			pPopup->addLabel(new KviKvsTreeNodeSpecialCommandDefpopupLabelLabel(pLabelBegin, szCondition, *pText, pIcon ? *pIcon : QString(), pItemName ? *pItemName : QString()));
			delete pParameters;
		}
		else if(szLabelLow == "popup")
		{
			EXTRACT_POPUP_LABEL_PARAMETERS
			EXTRACT_POPUP_LABEL_CONDITION

			QString * pText = pParameters->first();
			if(!pText)
			{
				error(pLabelBegin, __tr2qs_ctx("Unexpected empty <text> field in extpopup parameters. See /help defpopup for the syntax", "kvs"));
				delete pParameters;
				return nullptr;
			}
			QString * pIcon = pParameters->next();
			QString * pItemName = pParameters->next();

			KviKvsTreeNodeSpecialCommandDefpopupLabelPopup * pSubPopup = parseSpecialCommandDefpopupLabelPopup();
			if(!pSubPopup)
			{
				delete pParameters;
				return nullptr;
			}

			pSubPopup->setCondition(szCondition);
			pSubPopup->setText(*pText);
			pSubPopup->setItemName(pItemName ? *pItemName : QString());
			if(pIcon)
				pSubPopup->setIcon(*pIcon);
			pPopup->addLabel(pSubPopup);
			delete pParameters;
		}
		else if(szLabelLow == "item")
		{
			EXTRACT_POPUP_LABEL_PARAMETERS
			EXTRACT_POPUP_LABEL_CONDITION

			QString * pText = pParameters->first();
			if(!pText)
			{
				error(pLabelBegin, __tr2qs_ctx("Unexpected empty <text> field in extpopup parameters. See /help defpopup for the syntax", "kvs"));
				delete pParameters;
				return nullptr;
			}
			QString * pIcon = pParameters->next();
			QString * pItemName = pParameters->next();

			const QChar * pBegin = KVSP_curCharPointer;
			KviKvsTreeNodeInstruction * pInstruction = parseInstruction();
			if(pInstruction)
			{
				// in fact we don't need it: we just need the code block
				delete pInstruction;
			}
			else
			{
				// empty instruction or error ?
				if(error())
				{
					// error
					delete pParameters;
					return nullptr;
				}
				// empty instruction
				warning(pBegin, __tr2qs_ctx("Found empty instruction for popup item: maybe you need to fix the script?", "kvs"));
			}
			int iLen = KVSP_curCharPointer - pBegin;
			if(iLen > 0)
			{
				QString szInstruction(pBegin, KVSP_curCharPointer - pBegin);
				KviCommandFormatter::bufferFromBlock(szInstruction);
				pPopup->addLabel(new KviKvsTreeNodeSpecialCommandDefpopupLabelItem(pLabelBegin, szCondition, *pText, pIcon ? *pIcon : QString(), szInstruction, pItemName ? *pItemName : QString()));
			}
			else
			{
				// zero length instruction, but still add the item
				QString szInstruction = "";
				KviCommandFormatter::bufferFromBlock(szInstruction);
				pPopup->addLabel(new KviKvsTreeNodeSpecialCommandDefpopupLabelItem(pLabelBegin, szCondition, *pText, pIcon ? *pIcon : QString(), szInstruction, pItemName ? *pItemName : QString()));
			}
			delete pParameters;
		}
		else if(szLabelLow == "extpopup")
		{
			EXTRACT_POPUP_LABEL_PARAMETERS
			EXTRACT_POPUP_LABEL_CONDITION

			QString * pText = pParameters->first();
			if(!pText)
			{
				error(pLabelBegin, __tr2qs_ctx("Unexpected empty <text> field in extpopup parameters. See /help defpopup for the syntax", "kvs"));
				delete pParameters;
				return nullptr;
			}
			QString * pName = pParameters->next();
			if(!pName)
			{
				error(pLabelBegin, __tr2qs_ctx("Unexpected empty <name> field in extpopup parameters. See /help defpopup for the syntax", "kvs"));
				delete pParameters;
				return nullptr;
			}
			QString * pIcon = pParameters->next();
			QString * pItemName = pParameters->next();
			if(KVSP_curCharUnicode == ';')
				KVSP_skipChar;
			pPopup->addLabel(new KviKvsTreeNodeSpecialCommandDefpopupLabelExtpopup(pLabelBegin, szCondition, *pText, pIcon ? *pIcon : QString(), *pName, pItemName ? *pItemName : QString()));
			delete pParameters;
		}
		else
		{
			error(pLabelBegin, __tr2qs_ctx("Found token '%Q' where a 'prologue', 'separator', 'separatorid', 'label', 'popup', 'item', 'extpopup' or 'epilogue' label was expected", "kvs"), &szLabel);
			return nullptr;
		}

		if(!skipSpacesAndNewlines())
			return nullptr;
	}

	KVSP_skipChar;
	return pPopup.release();
}

KviKvsTreeNodeCommand * KviKvsParser::parseSpecialCommandDefpopup()
{
	// FIXME: This SHOULD be renamed to popup.create (NOT popup.define!)
	//        and internally aliased to defpopup as backward compat
	//        There should be then also popup.destroy etc..

	/*
		@doc: defpopup
		@type:
			command
		@title:
			defpopup
		@syntax:
			defpopup [-m] (<popup_name>)
			{
				prologue[(<id>)] <prologue_command>

				epilogue[(<id>)] <epilogue_command>

				label(<text>[,<id>])[(<expression>)][;]

				item(<text>[,<icon>[,<id>]])[(<expression>)]<command>

				popup(<text>[,<icon>[,<id>]])[(<expression>)]
				{
					<popup body>
				}

				extpopup(<text>,<name>[,<icon>[,<id>]])[(<expression>)][;]

				separator[(<expression>)][;]
				separatorid[(<id>)][(<expression>)][;]
				...
			}
		@short:
			Defines a popup menu
		@switches:
			!sw: -m | --merge
			Merges the new popup contents with the current named popup.
			If the named popup does not exist, the popup is created.
		@description:
			Defines the popup menu <popup_name>. If the -m switch is [b]not[/b] used
			the previous contents of the popups are cleared before updating.[br]
			The popup is generated 'on the fly' when the [cmd]popup[/cmd] command
			is called.[br]
			The [i]item[/i] keyword adds a menu item with visible <text>,
			the optional <icon> and <command> as code to be executed when the item
			is clicked. <text> is a string that is evaluated at [cmd]popup[/cmd]
			call time and may contain identifiers and variables. If <expression>
			is given, it is evaluated at [cmd]popup[/cmd] call time and if the result
			is 0, the item is not shown in the physical popup.[br]
			The [i]popup[/i] keyword adds a submenu with visible <text>, the optional
			<icon> and a popup body that has identical syntax to the defpopup body.
			The <expression> has the same meaning as with the [i]item[/i] keyword.[br]
			The 'extpopup' keyword adds a submenu with visible <text>, the optional
			icon and a popup body that is defined by the popup menu <name>. This
			basically allows to nest popup menus and define their parts separately.
			<icon> and <expression> have the same meaning as with the [i]item[/i] keyword.[br]
			The [i]separator[/i] keyword adds a straight line between items (separator).[br]
			The 'separatorid' keyword adds a straight line between items, but permits to
			specify a separator id.[br]
			The [i]label[/i] keyword adds a descriptive label that acts like a separator.[br]
			The [i]prologue[/i] keyword adds a <prologue_command> to be executed
			just before the popup is filled at [cmd]popup[/cmd] command call.[br]
			The [i]epilogue[/i] keyword adds an <epilogue_command> to be executed
			just after the popup has been filled at [cmd]popup[/cmd] command call.[br]
			There can be multiple prologue and epilogue commands: their execution order
			is undefined.[br]
			<icon> is always an [doc:image_id]image identifier[/doc].[br]
			<id> is an unique identifier that can be used to remove single items
			by the means of [cmd]delpopupitem[/cmd]. If <id> is omitted
			then it is automatically generated.
			Please note that using this command inside the prologue, epilogue
			or item code of the modified popup menu is forbidden.
			In other words: self modification of popup menus is [b]not[/b] allowed.[br]
			To remove a popup menu use this command with an empty body:
			[example]
				defpopup(test){}
			[/example]
			This will remove the popup [i]test[/i] and free its memory.
			Popups have a special kind of local variables that have an extended lifetime,
			called [i]extended scope[/i] - this is described in the [doc:data_structures]Data structures documentation[/doc].[br]
			The syntax for an "extended scope" variable is:[br]
			[b]%:<variable name>[/b][br]
			This type of variable is maintained during the entire [i]visible lifetime[/i] of the popup -
			from the [cmd]popup[/cmd] command call to the moment in that the user selects an item and the
			corresponding code is executed (essentially from a [cmd]popup[/cmd] call to the next one).[br]
			This allows you to pre-calculate data and conditions in the prologue of the popup
			and then use it in the item handlers or item conditions.[br]
		@seealso:
			[cmd]popup[/cmd]
		@examples:
	*/

	if(KVSP_curCharUnicode != '(')
	{
		warning(KVSP_curCharPointer, __tr2qs_ctx("The 'defpopup' command needs an expression enclosed in parenthesis", "kvs"));
		errorBadChar(KVSP_curCharPointer, '(', "defpopup");
		return nullptr;
	}

	const QChar * pBegin = KVSP_curCharPointer;

	KVSP_skipChar;

	KviKvsTreeNodeData * pPopupName = parseSingleParameterInParenthesis();
	if(!pPopupName)
		return nullptr;

	if(!skipSpacesAndNewlines())
	{
		delete pPopupName;
		return nullptr;
	}

	KviKvsTreeNodeSpecialCommandDefpopupLabelPopup * pMainPopup = parseSpecialCommandDefpopupLabelPopup();
	if(!pMainPopup)
	{
		delete pPopupName;
		return nullptr;
	}

	return new KviKvsTreeNodeSpecialCommandDefpopup(pBegin, pPopupName, pMainPopup);
}

KviKvsTreeNodeCommand * KviKvsParser::parseSpecialCommandHelp()
{
	// again not a fully special command: this routine just returns
	// a CoreSimpleCommand but parses the parameters as constants

	// we handle a single big parameter, with whitespace stripped
	// This is because we want the identifiers to be preserved
	// as unevaluated (i.e $function).

	skipSpaces();

	const QChar * pBegin = KVSP_curCharPointer;
	while(!KVSP_curCharIsEndOfCommand)
		KVSP_skipChar;

	if(!KVSP_curCharIsEndOfBuffer)
		KVSP_skipChar;

	QString tmp(pBegin, KVSP_curCharPointer - pBegin);
	tmp = tmp.trimmed();
	const QString szHelpName("help");

	KviKvsCoreSimpleCommandExecRoutine * r = KviKvsKernel::instance()->findCoreSimpleCommandExecRoutine(szHelpName);
	if(!r)
		return nullptr; // <--- internal error!

	KviKvsTreeNodeDataList * p = new KviKvsTreeNodeDataList(pBegin);
	p->addItem(new KviKvsTreeNodeConstantData(pBegin, new KviKvsVariant(tmp)));

	return new KviKvsTreeNodeCoreSimpleCommand(pBegin, szHelpName, p, r);
}
