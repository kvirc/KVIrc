//=============================================================================
//
//   File : KviKvsParser_lside.cpp
//   Creation date : Thu 03 Nov 2003 13.11 CEST by Szymon Stefanek
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
#include "KviLocale.h"

/*
	@doc: operators
	@title:
		Operators
	@keyterms:
		operator,operators,assignment,assign
	@type:
		language
	@short:
		Variable operators, assignments & co.
	@body:
		Operator constructs are commands just like the other ones.
		All the operators work on local or global variables and dictionaries.[br]
		The generic operator syntax is:[br]
		[br]
		[b]<left_operand> <operator> [right_operand][/b][br]
		[br]
		where <left_operand> and [right_operand] depend on the <operator>.[br]
		Some operators have no [right_operand] and these are called [b]unary operators[/b]:
		they operate directly on <left_operand>.[br]
		Some operators, like arithmetic ones, are typically used inside the special function
		[b]$(<expression>)[/b], called the [i][doc:expressioneval]Expression evaluation identifier[/doc][/i];
		This special function returns the result of the evaluation of the <expression>.
		In previous versions of KVIrc this function was called $calc().
		[table]
			[tr][td]Operator[/td][td]document[/td][/tr]
			[tr][td]=[/td][td][doc:assignment]assignment operator[/doc][/td][/tr]
			[tr][td]++[/td][td][doc:incrementdecrement]Increment and decrement operators[/doc][/td][/tr]
			[tr][td]--[/td][td][doc:incrementdecrement]Increment and decrement operators[/doc][/td][/tr]
			[tr][td]+=[/td][td][doc:selfarithmetic]Arithmetic self-operators[/doc][/td][/tr]
			[tr][td]-=[/td][td][doc:selfarithmetic]Arithmetic self-operators[/doc][/td][/tr]
			[tr][td]*=[/td][td][doc:selfarithmetic]Arithmetic self-operators[/doc][/td][/tr]
			[tr][td]/=[/td][td][doc:selfarithmetic]Arithmetic self-operators[/doc][/td][/tr]
			[tr][td]%=[/td][td][doc:selfarithmetic]Arithmetic self-operators[/doc][/td][/tr]
			[tr][td]|=[/td][td][doc:selfbitwise]Bitwise self-operators[/doc][/td][/tr]
			[tr][td]&=[/td][td][doc:selfbitwise]Bitwise self-operators[/doc][/td][/tr]
			[tr][td]^=[/td][td][doc:selfbitwise]Bitwise self-operators[/doc][/td][/tr]
			[tr][td]<<=[/td][td][doc:selfbitwise]Bitwise self-operators[/doc][/td][/tr]
			[tr][td]>>=[/td][td][doc:selfbitwise]Bitwise self-operators[/doc][/td][/tr]
			[tr][td].=[/td][td][doc:stringconcatenation]String concatenation operators[/doc][/td][/tr]
			[tr][td]<<[/td][td][doc:stringconcatenation]String concatenation operators[/doc][/td][/tr]
			[tr][td]<,[/td][td][doc:stringconcatenation]String concatenation operators[/doc][/td][/tr]
			[tr][td]<+[/td][td][doc:arrayconcatenation]Array concatenation[/doc][/td][/tr]
			[tr][td]=~[/td][td][doc:binding]Binding operator[/doc][/td][/tr]
		[/table]
*/

KviKvsTreeNodeData * KviKvsParser::parseOperationRightSide(bool bPreferNumeric)
{
	KviPointerList<KviKvsTreeNodeData> * l = new KviPointerList<KviKvsTreeNodeData>();
	l->setAutoDelete(true);

	const QChar * pBegin = KVSP_curCharPointer;

	for(;;)
	{
		switch(KVSP_curCharUnicode)
		{
			case 0:
				goto end_of_the_param;
				break;
			case '\n':
			case '\r':
			case ';':
				KVSP_skipChar;
				goto end_of_the_param;
				break;
			case ' ':
			case '\t':
				skipSpaces();
				if(KVSP_curCharIsEndOfCommand)
				{
					goto end_of_the_param;
				}
				else
				{
					// separate by single spaces
					bPreferNumeric = false; // this can't be a number
					l->append(new KviKvsTreeNodeConstantData(KVSP_curCharPointer, new KviKvsVariant(QString(" "))));
				}
				break;
			default:
				// anything else is a parameter
				KviKvsTreeNodeData * p = parseCommandParameter(bPreferNumeric);
				if(!p)
				{
					// this is an error
					delete l;
					return nullptr;
				}
				l->append(p);
				break;
		}
	}

end_of_the_param:
	if(l->count() > 1)
	{
		// complex parameter needed
		return new KviKvsTreeNodeCompositeData(pBegin, l);
	}
	else
	{
		if(l->count() > 0)
		{
			// a single parameter in the list
			l->setAutoDelete(false);
			KviKvsTreeNodeData * p = l->first();
			delete l;
			return p;
		}
		else
		{
			// empty (this should NEVER happen anyway)
			delete l;
			return new KviKvsTreeNodeConstantData(pBegin, new KviKvsVariant(QString("")));
		}
	}
	// never reached
	return nullptr;
}

/*
	@doc: assignment
	@title:
		Assignment operation
	@keyterms:
		assignment
	@type:
		language
	@short:
		Assignment operation
	@body:
		The assignment is the [i]plainest[/i] of the operators: it works just like in any other programming language.[br]
		The syntax is:[br]
		[br]
		[b]<target> = <source>[/b]
		[br][br]
		<target> must be a variable, <source> can be any parameter.[br]
		If the <target> variable doesn't exist, it is created.
		If it already exists, it is eventually converted to the type of <source> (scalar, hash or array).[br]
		If <source> evaluates to an empty value then the <target> variable is unset.
	@examples:
		[example]
			[comment]# Assigning a constant to the variable %Tmp[/comment]
			%Tmp = 1
			[cmd]echo[/cmd] %Tmp
			[comment]# Assigning a string constant to the variable %Tmp[/comment]
			%Tmp = some string
			[cmd]echo[/cmd] %Tmp
			[comment]# Assigning a string constant to the variable %Tmp[/comment]
			%Tmp = "some string with whitespace &nbsp; &nbsp; &nbsp; &nbsp; preserved"
			[cmd]echo[/cmd] %Tmp
			[comment]# Assigning a variable to another variable copies its contents[/comment]
			%Someothervariable = "Contents"
			%Tmp = %Someothervariable
			[cmd]echo[/cmd] %Tmp
			[comment]# Assigning a variable string to the variable %z[/comment]
			%color = blue
			%z = my eyes are %color
			[cmd]echo[/cmd] %z
			[comment]# Assigning a variable string (with a function call inside) to the variable %x[/comment]
			%x = the system os is [fnc]$system.osname[/fnc]
			[cmd]echo[/cmd] %x
			[comment]# Assigning an empty string to the local variable %y unsets %y[/comment]
			%x =
			[cmd]echo[/cmd] %y
			[comment]# This is equivalent to the above[/comment]
			%y = ""
			[comment]# This is equivalent too, if $function evaluates to an empty string[/comment]
			%y = $function()
			[comment]# Assigning a variable string to a hash entry[/comment]
			%Dict{key} = [fnc]$system.osname[/fnc]\ian
			[comment]# Unsetting an array entry[/comment]
			%mydict[23] = ""
			[comment]# Assigning a hash to another: %mydict[] becomes a copy of %anotherdict[][/comment]
			%anotherdict{"The key"} = "Some dummy value"
			%mydict = %anotherdict
			[cmd]echo[/cmd]%mydict{"The key"}
			[comment]# This will convert %mydict to be a scalar variable (deleting all the %mydict contents!)[/comment]
			%mydict = "some default value"
			[comment]# Unsetting a whole hash[/comment]
			%anotherdict =
		[/example]
*/

/*
	@doc: incrementdecrement
	@title:
		Increment and decrement operations
	@keyterms:
		increment, decrement
	@type:
		language
	@short:
		Increment and decrement operations
	@body:
		These two operators work only on numeric operands.[br]
		The syntax is:[br]
		[br]
		[b]<target>++[/b][br]
		[b]<target>--[/b][br]
		[br]
		[b]++[/b] increments <target> by one, [b]--[/b] decrements <target> by one.[br]
		These are equivalent to [b]+= 1[/b] and [b]-= 1[/b].[br]
		<target> must be an existing variable and contain an integer value.[br]
		If <target> contains a real value then the real is truncated to the nearest
		integer and then incremented or decremented.[br]
	@examples:
		[example]
			%a=10
			[cmd]echo[/cmd] "Incrementing"
			[cmd]while[/cmd](%a < 20)
			{
				[cmd]echo[/cmd] %a
				[b]%a++[/b]
			}
			[cmd]echo[/cmd] "Decrementing"
			[cmd]while[/cmd](%a > 10)
			{
				[cmd]echo[/cmd] %a
				[b]%a--[/b]
			}
			[cmd]echo[/cmd] "Testing for loop"
			[cmd]for[/cmd](%a=0;%a < 10;[b]%a++[/b])
			{
				[cmd]echo[/cmd] %a
			}
		[/example]
	@seealso:
		[doc:operators]Operators[/doc]
*/

/*
	@doc: selfarithmetic
	@title:
		Arithmetic self-operators
	@type:
		language
	@short:
		Arithmetic self-operators
	@body:
		These operators work only on numeric operands.[br]
		The syntax is:[br]
		[br]
		[b]<target> += <right_operand>[/b][br]
		[b]<target> -= <right_operand>[/b][br]
		[b]<target> *= <right_operand>[/b][br]
		[b]<target> /= <right_operand>[/b][br]
		[b]<target> %= <right_operand>[/b][br]
		[br]
		<target> must be an existing variable and contain a numeric value.
		<right_operand> must evaluate to a numeric value.
		Note that if you want <right_operand> to be a result of an expression, you must
		enclose it in the $(*) expression evaluation call.[br]
		Operator += sums the <right_operand> value to the <target> value and stores the result in <target>.[br]
		Operator -= subtracts <right_operand> from <target> and stores the result in <target>.[br]
		Operator *= multiplies <target> by <right_operand> and stores the result in <target>.[br]
		Operator /= divides <target> by <right_operand> and stores the result in <target>.[br]
		Operator %= computes <target> modulus <right_operand> and stores the result in <target>.[br]
		The division and modulus operators fail with an error if <right_operand> is 0.[br]
		If both <target> and <right_operand> are integer values then the results of the division
		and modulus are integers (truncated for the division).[br]
		If <target> or <right_operand> or both are floating point values then the result is a floating point value.[br]
	@examples:
		[example]
			%a=10
			[cmd]echo[/cmd] %a
			%a+=20
			[cmd]echo[/cmd] %a
			%a-=$(%a - 1)
			[cmd]echo[/cmd] %a
			%a *= 10
			[cmd]echo[/cmd] %a
			%a /= 21
			[cmd]echo[/cmd] %a
			%a *= 20
			[cmd]echo[/cmd] %a
			%a /= 21.0
			[cmd]echo[/cmd] %a
			%b = 10.0
			%a %= %b
			[cmd]echo[/cmd] %a
			%a = 10
			%b = 3
			[comment]# nice trick[/comment]
			%a /= %b.0
			[cmd]echo[/cmd] %a
		[/example]
	@seealso:
		[doc:operators]Operators[/doc]
*/

/*
	@doc: selfbitwise
	@title:
		Bitwise self-operators
	@type:
		language
	@short:
		Bitwise self-operators
	@body:
		These operators work only on integer operands.[br]
		The syntax is:[br]
		[br]
		[b]<target> |= <right_operand>[/b][br]
		[b]<target> &= <right_operand>[/b][br]
		[b]<target> ^= <right_operand>[/b][br]
		[b]<target> >>= <right_operand>[/b][br]
		[b]<target> <<= <right_operand>[/b][br]
		[br]
		<target> must be an existing variable and contain a numeric value.
		<right_operand> must evaluate to a numeric value.
		If <target> or <right_operand> are floating point values then they are truncated
		and converted to integers.[br]
		Note that if you want <right_operand> to be a result of an expression, you must
		enclose it in the $(*) expression evaluation call.[br]
		Operator |= computes <target> bitwise-or <right_operand> and stores the result in <target>.[br]
		Operator &= computes <target> bitwise-and <right_operand> and stores the result in <target>.[br]
		Operator ^= computes <target> bitwise-xor <right_operand> and stores the result in <target>.[br]
		Operator >>= shifts <target> <right_operand> bits to the right and stores the result int <target>.[br]
		Operator <<= shifts <target> <right_operand> bits to the left and stores the result int <target>.[br]
		Note that "!=" is not available. You must use %a = $(!%b) to implement it.[br]
		For operators >>= and <<= <right_operand> must be a positive integer.[br]
	@examples:
		[example]
			%a = 1
			[cmd]echo[/cmd] %a
			%a |= 2
			[cmd]echo[/cmd] %a
			%a &= 2
			[cmd]echo[/cmd] %a
			%a ^= 1
			[cmd]echo[/cmd] %a
			%a >>= 2
			[cmd]echo[/cmd] %a
			%a <<= 1
			[cmd]echo[/cmd] %a
		[/example]
	@seealso:
		[doc:operators]Operators[/doc]
*/

/*
	@doc: stringconcatenation
	@title:
		String concatenation operators
	@type:
		language
	@short:
		String concatenation operators
	@body:
		These operators concatenate strings.
		The syntax is:[br]
		[br]
		[b]<target> .= <right_operand>[/b][br]
		[b]<target> << <right_operand>[/b][br]
		[b]<target> <, <right_operand>[/b][br]
		[br]
		Operator [b].=[/b] appends <right_operand> to <target>.
		Operator [b]<<[/b] appends a space followed by <right_operand> to <target> if <target> is non empty,
		otherwise sets <target> to <right_operand>.
		Operator [b]<,[/b] is similar to [b]<<[/b] but uses a comma to separate the two variable contents.
		The last two operators are useful in creating space-separated or comma-separated lists.
	@examples:
		[example]
			%a = ""
			%a << free
			[cmd]echo[/cmd] %a
			%a .= bsd
			[cmd]echo[/cmd] %a
			%a << rox
			[cmd]echo[/cmd] %a
			%a <, but linux is better!
			[cmd]echo[/cmd] %a
		[/example]
	@seealso:
		[doc:operators]Operators[/doc]
*/

/*
	@doc: arrayconcatenation
	@title:
		Array concatenation operator
	@type:
		language
	@short:
		Array concatenation operator
	@body:
		This operator concatenates scalars, arrays or hashes to a target array
		- if the target is not an array, it is converted to one first.[br]
		The syntax is:[br]
		[br]
		[b]<target> <+ <right_operand>[/b][br]
		[br]
		If <right_operand> is a scalar then it is appended to <target>,
		otherwise the constituent items of the array or values of the hash are appended.
	@seealso:
		[doc:operators]Operators[/doc]
*/

/*
	@doc: binding
	@title:
		Binding operator
	@type:
		language
	@short:
		Binding operator
	@body:
		This operator is a really ugly, poor and clueless attempt to reach at least 1% of the
		power of the Perl [b]=~[/b] operator :D[br]
		It allows some complex string operations to be performed efficiently by operating directly
		on the left operand (in fact this is a lot faster in KVIrc since at least one step of parsing is skipped).[br]
		Its basic syntax is:[br]
		[br][b]<left_operand> =~ <operation>[parameters][/b][br][br]
		Where <operation> may be one of [b]t[/b], [b]s[/b] and parameters depend on it.[br]
		<left_operand> is the target of the <operation>.[br]
		If <left_operand> is an array or dictionary, the <operation> is executed on each item they contain.[br]
		Operation [b]t[/b] is the transliteration.[br]
		The complete syntax with parameters is:[br]
		[br][b]<left_operand> [b]=~[/b] t/<search characters>/<replacement characters>/[/b][br][br]
		where <search characters> is a string of characters that are replaced with the corresponding
		characters in <replacement characters>.[br]
		This operation can be also named [b]y[/b] or [b]tr[/b] (to preserve some compatibility with other languages).
		[example]
			%A=This is a test string
			echo %A
			%A=~ tr/abcdefghi/ABCDEFGHI/
			echo %A
		[/example]
		Operation [b]s[/b] is the substitution.[br]
		The complete syntax with parameters is:[br]
		[br][b]<left_operand> [b]=~[/b] s/<search pattern>/<replacement pattern>/[flags][/b][br][br]
		where <search pattern> is an extended regular expression to be matched in the <left_operand>
		and <replacement string> is a special pattern that will replace any occurrence found.[br]
		<search pattern> may contain parentheses to capture parts of the matched text.
		<replacement string> can contain the escape sequences \\N where N is a number between 1 and 9
		to be replaced by the captured text.[br]
		Please note that you need to double the backslashes in the search pattern
		since the KVS parser will first unquote the string and then pass it to the regexp engine.
		That's also why we use \\N and not \N.[br]
		\\0 is a special escape that will be replaced by the entire match (is always valid!).[br]
		[flags] may be a combination of the letters [b]g[/b], [b]i[/b] and [b]w[/b].[br]
		[b]g[/b] causes the search to be global and not stop after the first occurrence of <search pattern>.[br]
		[b]i[/b] causes the search to be case insensitive.[br]
		[b]m[/b] causes the search to be case minimal (non-greedy).[br]
		[b]w[/b] causes the search pattern to be interpreted as a simple wildcard regular expression.
	@examples:
		[example]
			%A=This is a test string
			echo %A
			%A=~ s/([a-z])i([a-z])/\\1I\\2/
			echo %A
			%A=~ s/([a-z])i([a-z])/\\1@\\2/gi
			echo %A
		[/example]
		[example]
			%a = ""
			%a << free
			[cmd]echo[/cmd] %a
			%a .= bsd
			[cmd]echo[/cmd] %a
			%a << rox
			[cmd]echo[/cmd] %a
			%a <, but Linux is better!
			[cmd]echo[/cmd] %a
		[/example]
	@seealso:
		[doc:operators]Operators[/doc]
*/

KviKvsTreeNodeData * KviKvsParser::parseBindingOperationParameter()
{
	KviPointerList<KviKvsTreeNodeData> * l = new KviPointerList<KviKvsTreeNodeData>;
	l->setAutoDelete(true);

	const QChar * pBegin = KVSP_curCharPointer;

	for(;;)
	{
		switch(KVSP_curCharUnicode)
		{
			case 0:
			case '/':
			case '\n':
			case '\r':
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
				l->append(parseBindingOperationLiteralParameter());
			}
			break;
		}
	}
end_of_function_parameter:
	if(l->count() > 1)
	{
		// complex parameter needed
		return new KviKvsTreeNodeCompositeData(pBegin, l);
	}
	else
	{
		// a single parameter in the list or empty list at all
		l->setAutoDelete(false);
		KviKvsTreeNodeData * p = l->first();
		delete l;
		if(!p)
			p = new KviKvsTreeNodeConstantData(KVSP_curCharPointer, new KviKvsVariant(QString("")));
		return p;
	}
	// never reached
	return nullptr;
}

KviKvsTreeNodeOperation * KviKvsParser::parseBindingOperation()
{
	// t or tr or y
	// s
	const QChar * pBegin = KVSP_curCharPointer;

	while(KVSP_curCharIsLetter)
		KVSP_skipChar;

	QString szOp = QString(pBegin, KVSP_curCharPointer - pBegin).toLower();

	skipSpaces();

	if(KVSP_curCharUnicode != '/')
	{
		error(KVSP_curCharPointer, __tr2qs_ctx("Found character '%q' (Unicode %x) where a slash '/' was expected", "kvs"), KVSP_curCharPointer, KVSP_curCharUnicode);
		return nullptr;
	}

	KVSP_skipChar;

	KviKvsTreeNodeData * pFirst = parseBindingOperationParameter();
	if(!pFirst)
		return nullptr;

	if(KVSP_curCharIsEndOfCommand)
	{
		error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of command in binding operation, at least two slashes are missing", "kvs"));
		delete pFirst;
		return nullptr;
	}

	if(KVSP_curCharUnicode != '/')
	{
		error(KVSP_curCharPointer, __tr2qs_ctx("Found character '%q' (Unicode %x) where a slash '/' was expected", "kvs"), KVSP_curCharPointer, KVSP_curCharUnicode);
		delete pFirst;
		return nullptr;
	}

	KVSP_skipChar;

	KviKvsTreeNodeData * pSecond = parseBindingOperationParameter();
	if(!pSecond)
	{
		delete pFirst;
		return nullptr;
	}

	if(KVSP_curCharIsEndOfCommand)
	{
		error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of command in binding operation, at least one slash is missing", "kvs"));
		delete pFirst;
		delete pSecond;
		return nullptr;
	}

	if(KVSP_curCharUnicode != '/')
	{
		error(KVSP_curCharPointer, __tr2qs_ctx("Found character '%q' (Unicode %x) where a slash '/' was expected", "kvs"), KVSP_curCharPointer, KVSP_curCharUnicode);
		delete pFirst;
		delete pSecond;
		return nullptr;
	}

	KVSP_skipChar;

	KviKvsTreeNodeData * pThird = parseCommandParameter();
	if(!pThird)
	{
		if(error())
		{
			delete pFirst;
			delete pSecond;
			return nullptr;
		}

		pThird = new KviKvsTreeNodeConstantData(KVSP_curCharPointer, new KviKvsVariant(QString("")));
	}

	while(!KVSP_curCharIsEndOfCommand)
		KVSP_skipChar;
	if(!KVSP_curCharIsEndOfBuffer)
		KVSP_skipChar;

	if((szOp == "t") || (szOp == "tr") || (szOp == "y"))
	{
		// transliteration  tr/szFirst/szSecond/szFlags
		return new KviKvsTreeNodeOperationStringTransliteration(pBegin, pFirst, pSecond, pThird);
	}
	else if(szOp == "s")
	{
		// regexp substitution s/szFirst/szSecond/szFlags
		return new KviKvsTreeNodeOperationStringSubstitution(pBegin, pFirst, pSecond, pThird);
	}

	delete pFirst;
	delete pSecond;
	delete pThird;

	error(KVSP_curCharPointer, __tr2qs_ctx("Unknown binding operation '%Q'", "kvs"), &szOp);
	return nullptr;
}

KviKvsTreeNodeOperation * KviKvsParser::parseOperation()
{
	// find the operator
	const QChar * pBegin = KVSP_curCharPointer;

	switch(KVSP_curCharUnicode)
	{
		case '=':
		{
			KVSP_skipChar;
			if(KVSP_curCharUnicode == '~')
			{
				KVSP_skipChar;
				skipSpaces();
				if(KVSP_curCharIsEndOfCommand)
				{
					error(KVSP_curCharPointer, __tr2qs_ctx("Missing right side operand for the binding operator '=~'", "kvs"));
					return nullptr;
				}
				return parseBindingOperation();
			}
			else
			{
				skipSpaces();
				KviKvsTreeNodeData * d = parseOperationRightSide(true);
				if(!d)
					return nullptr; // error
				return new KviKvsTreeNodeOperationAssignment(pBegin, d);
			}
		}
		break;
		case '+':
			KVSP_skipChar;
			switch(KVSP_curCharUnicode)
			{
				case '+':
					// operator ++
					KVSP_skipChar;
					skipSpaces();
					if(!KVSP_curCharIsEndOfCommand)
					{
						warning(KVSP_curCharPointer, __tr2qs_ctx("Trailing garbage ignored after operator '++'", "kvs"));
					}
					while(!KVSP_curCharIsEndOfCommand)
						KVSP_skipChar;
					if(!KVSP_curCharIsEndOfBuffer)
						KVSP_skipChar;
					return new KviKvsTreeNodeOperationIncrement(pBegin);
					break;
				case '=':
					// operator +=
					KVSP_skipChar;
					skipSpaces();
					if(KVSP_curCharIsEndOfCommand)
					{
						error(KVSP_curCharPointer, __tr2qs_ctx("Missing right operand for operator '+='", "kvs"));
						return nullptr;
					}
					KviKvsTreeNodeData * d = parseOperationRightSide(true);
					if(!d)
						return nullptr; // error
					return new KviKvsTreeNodeOperationSelfSum(pBegin, d);
					break;
			}
			break;
		case '-':
			KVSP_skipChar;
			switch(KVSP_curCharUnicode)
			{
				case '-':
					KVSP_skipChar;
					// operator --
					skipSpaces();
					if(!KVSP_curCharIsEndOfCommand)
					{
						warning(KVSP_curCharPointer, __tr2qs_ctx("Trailing garbage ignored after operator '--'", "kvs"));
					}
					while(!KVSP_curCharIsEndOfCommand)
						KVSP_skipChar;
					if(!KVSP_curCharIsEndOfBuffer)
						KVSP_skipChar;
					return new KviKvsTreeNodeOperationDecrement(pBegin);
					break;
				case '>':
					warning(KVSP_curCharPointer, __tr2qs_ctx("This looks a lot like an object handle dereferencing operator '->' but in fact it isn't. Maybe you forgot a '$' just after?", "kvs"));
					break;
				case '=':
					// operator -=
					KVSP_skipChar;
					skipSpaces();
					if(KVSP_curCharIsEndOfCommand)
					{
						error(KVSP_curCharPointer, __tr2qs_ctx("Missing right operand for operator '-='", "kvs"));
						return nullptr;
					}
					KviKvsTreeNodeData * d = parseOperationRightSide(true);
					if(!d)
						return nullptr; // error
					return new KviKvsTreeNodeOperationSelfSubtraction(pBegin, d);
					break;
			}
			break;
		case '<':
			KVSP_skipChar;
			switch(KVSP_curCharUnicode)
			{
				case '<':
					KVSP_skipChar;
					if(KVSP_curCharUnicode == '=')
					{
						KVSP_skipChar;
						skipSpaces();
						if(KVSP_curCharIsEndOfCommand)
						{
							error(KVSP_curCharPointer, __tr2qs_ctx("Missing right operand for operator '<<='", "kvs"));
							return nullptr;
						}
						KviKvsTreeNodeData * d = parseOperationRightSide(true);
						if(!d)
							return nullptr; // error
						return new KviKvsTreeNodeOperationSelfShl(pBegin, d);
					}
					else
					{
						skipSpaces();
						if(KVSP_curCharIsEndOfCommand)
						{
							error(KVSP_curCharPointer, __tr2qs_ctx("Missing right operand for operator '<<'", "kvs"));
							return nullptr;
						}
						KviKvsTreeNodeData * d = parseOperationRightSide();
						if(!d)
							return nullptr; // error
						return new KviKvsTreeNodeOperationStringAppendWithSpace(pBegin, d);
					}
					break;
				case ',':
				{
					KVSP_skipChar;
					skipSpaces();
					if(KVSP_curCharIsEndOfCommand)
					{
						error(KVSP_curCharPointer, __tr2qs_ctx("Missing right operand for operator '<,'", "kvs"));
						return nullptr;
					}
					KviKvsTreeNodeData * d = parseOperationRightSide();
					if(!d)
						return nullptr; // error
					return new KviKvsTreeNodeOperationStringAppendWithComma(pBegin, d);
				}
				break;
				case '+':
				{
					KVSP_skipChar;
					skipSpaces();
					if(KVSP_curCharIsEndOfCommand)
					{
						error(KVSP_curCharPointer, __tr2qs_ctx("Missing right operand for operator '<+'", "kvs"));
						return nullptr;
					}
					KviKvsTreeNodeData * d = parseOperationRightSide();
					if(!d)
						return nullptr; // error
					return new KviKvsTreeNodeOperationArrayAppend(pBegin, d);
				}
				break;
			}
			break;
		case '>':
			KVSP_skipChar;
			switch(KVSP_curCharUnicode)
			{
				case '>':
					KVSP_skipChar;
					if(KVSP_curCharUnicode == '=')
					{
						KVSP_skipChar;
						skipSpaces();
						if(KVSP_curCharIsEndOfCommand)
						{
							error(KVSP_curCharPointer, __tr2qs_ctx("Missing right operand for operator '>>='", "kvs"));
							return nullptr;
						}
						KviKvsTreeNodeData * d = parseOperationRightSide(true);
						if(!d)
							return nullptr; // error
						return new KviKvsTreeNodeOperationSelfShr(pBegin, d);
					}
					break;
			}
			break;
		case '.':
			KVSP_skipChar;
			switch(KVSP_curCharUnicode)
			{
				case '=':
					KVSP_skipChar;
					skipSpaces();
					if(KVSP_curCharIsEndOfCommand)
					{
						error(KVSP_curCharPointer, __tr2qs_ctx("Missing right operand for operator '.='", "kvs"));
						return nullptr;
					}
					KviKvsTreeNodeData * d = parseOperationRightSide();
					if(!d)
						return nullptr; // error
					return new KviKvsTreeNodeOperationStringAppend(pBegin, d);
					break;
			}
			break;
#define SELF_OPERATOR(__opchar, __opstr, __class)                                                                        \
	case __opchar:                                                                                                       \
		KVSP_skipChar;                                                                                                   \
		switch(KVSP_curCharUnicode)                                                                                      \
		{                                                                                                                \
			case '=':                                                                                                    \
				KVSP_skipChar;                                                                                           \
				skipSpaces();                                                                                            \
				if(KVSP_curCharIsEndOfCommand)                                                                           \
				{                                                                                                        \
					error(KVSP_curCharPointer, __tr2qs_ctx("Missing right operand for operator '" __opstr "='", "kvs")); \
					return nullptr;                                                                                      \
				}                                                                                                        \
				KviKvsTreeNodeData * d = parseOperationRightSide(true);                                                  \
				if(!d)                                                                                                   \
					return nullptr;                                                                                      \
				return new __class(pBegin, d);                                                                           \
				break;                                                                                                   \
		}                                                                                                                \
		break;
			SELF_OPERATOR('*', "*", KviKvsTreeNodeOperationSelfMultiplication)
			SELF_OPERATOR('/', "/", KviKvsTreeNodeOperationSelfDivision)
			SELF_OPERATOR('%', "%", KviKvsTreeNodeOperationSelfModulus)
			SELF_OPERATOR('|', "|", KviKvsTreeNodeOperationSelfOr)
			SELF_OPERATOR('&', "&", KviKvsTreeNodeOperationSelfAnd)
			SELF_OPERATOR('^', "^", KviKvsTreeNodeOperationSelfXor)
	}

	error(pBegin, __tr2qs_ctx("Unknown operator", "kvs"));
	return nullptr;
}

KviKvsTreeNodeInstruction * KviKvsParser::parseVoidFunctionCallOrOperation()
{
	KVSP_ASSERT((KVSP_curCharUnicode == '$') || (KVSP_curCharUnicode == '%') || (KVSP_curCharUnicode == '@'));

	const QChar * pBegin = KVSP_curCharPointer;

	KviKvsTreeNodeData * r = parsePercentOrDollar();

	if(!r)
	{
		// must be an error
		return nullptr;
	}

	skipSpaces();

	if(KVSP_curCharIsEndOfCommand)
	{
		// the end of the command
		if(!r->isFunctionCall())
		{
			if(r->isReadOnly())
			{
				warning(pBegin, __tr2qs_ctx("Unexpected (and senseless) read-only data evaluation", "kvs"));
				error(KVSP_curCharPointer, __tr2qs_ctx("Syntax error: confused by earlier errors: bailing out", "kvs"));
			}
			else
			{
				error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of script after a variable reference: expected operator", "kvs"));
			}
			delete r;
			return nullptr;
		}
		else
		{
			if(!KVSP_curCharIsEndOfBuffer)
				KVSP_skipChar;
			return new KviKvsTreeNodeVoidFunctionCall(r->location(), (KviKvsTreeNodeFunctionCall *)r);
		}
	}

	// not the end of a command : an operation
	if(r->isReadOnly())
	{
		// must be followed by the end of a command
		if(r->isFunctionCall())
		{
			error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected character '%q' (Unicode %x) after a void function call: end of instruction expected", "kvs"), KVSP_curCharPointer, KVSP_curCharUnicode);
		}
		else
		{
			warning(pBegin, __tr2qs_ctx("Unexpected (and senseless) read-only data evaluation", "kvs"));
			warning(pBegin, __tr2qs_ctx("Unexpected character '%q' (Unicode %x)", "kvs"), KVSP_curCharPointer, KVSP_curCharUnicode);
			error(KVSP_curCharPointer, __tr2qs_ctx("Syntax error: confused by earlier errors: bailing out", "kvs"));
		}
		delete r;
		return nullptr;
	}

	// ok.. parse the operation
	KviKvsTreeNodeOperation * op = parseOperation();
	if(!op)
	{
		delete r;
		return nullptr;
	}

	op->setTargetVariableReference(r);
	return op;
}
