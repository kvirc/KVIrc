//=============================================================================
//
//   File : KviKvsParser_dollar.cpp
//   Creation date : Thu 5 Oct 2003 20.20 CEST by Szymon Stefanek
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

KviKvsTreeNodeData * KviKvsParser::parseDollar(bool bInObjScope)
{
	KVSP_ASSERT(KVSP_curCharUnicode == '$');

	const QChar * pDollarBegin = KVSP_curCharPointer;
	const QChar * pBegin = KVSP_curCharPointer;

	KVSP_skipChar;

	if(!KVSP_curCharIsFunctionStart)
	{
		if(KVSP_curCharUnicode == 0)
			warning(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of script after '$' function call prefix", "kvs"));
		else
			warning(KVSP_curCharPointer, __tr2qs_ctx("Unexpected character %q (Unicode %x) after '$' function call prefix", "kvs"), KVSP_curCharPointer, KVSP_curCharUnicode);
		error(KVSP_curCharPointer, __tr2qs_ctx("Syntax error after '$' function call prefix. If you want to use a plain '$' in the code you need to escape it", "kvs"));
		return nullptr;
	}

	if(KVSP_curCharUnicode == '(')
	{
		// expression eval
		if(bInObjScope)
		{
			error(KVSP_curCharPointer, __tr2qs_ctx("Invalid expression evaluation in object scope", "kvs"));
			return nullptr;
		}

		KVSP_skipChar;
		skipSpaces();

		return parseExpression(')');
	}

	if(KVSP_curCharUnicode == '{')
	{
		// command block eval <--- senseless ???
		if(bInObjScope)
		{
			error(KVSP_curCharPointer, __tr2qs_ctx("Invalid command evaluation in object scope", "kvs"));
			return nullptr;
		}

		KviKvsTreeNodeInstruction * i = parseInstructionBlock();
		if(!i)
		{
			if(error())
				return nullptr;
			// trigger an error anyway: this is abused syntax :D
			error(KVSP_curCharPointer, __tr2qs_ctx("Empty instruction block for command evaluation", "kvs"));
			return nullptr;
		}

		return new KviKvsTreeNodeCommandEvaluation(pDollarBegin, i);
	}

	if(KVSP_curCharUnicode == '#')
	{
		// parameter count
		if(bInObjScope)
		{
			error(KVSP_curCharPointer, __tr2qs_ctx("Parameter identifiers are forbidden in object scope (after the '->' operator)", "kvs"));
			return nullptr;
		}

		KVSP_skipChar;

		return new KviKvsTreeNodeParameterCount(pDollarBegin);
	}

	if(KVSP_curCharIsNumber)
	{
		// this is a parameter identifier
		// $1-4 $1- $3

		if(bInObjScope)
		{
			error(KVSP_curCharPointer, __tr2qs_ctx("Parameter identifiers are forbidden in object scope (after the '->' operator)", "kvs"));
			return nullptr;
		}

		pBegin = KVSP_curCharPointer;

		while(KVSP_curCharIsNumber)
			KVSP_skipChar;

		QString szNum1(pBegin, KVSP_curCharPointer - pBegin);
		bool bOk;
		int iNum1 = szNum1.toInt(&bOk);
		if(!bOk)
			qDebug("Oops! A non-number made by numbers?");

		if(KVSP_curCharUnicode != '-')
		{
			// end
			return new KviKvsTreeNodeSingleParameterIdentifier(pDollarBegin, iNum1);
		}

		// dash... make sure it's not $N->$something
		KVSP_skipChar;
		if(KVSP_curCharUnicode == '>')
		{
			// object scope operator in fact...
			// go back to the - and return a single parameter identifier
			KVSP_backChar;
			return new KviKvsTreeNodeSingleParameterIdentifier(pDollarBegin, iNum1);
		}

		if(!KVSP_curCharIsNumber)
		{
			// from iNum1 to the end
			return new KviKvsTreeNodeMultipleParameterIdentifier(pDollarBegin, iNum1, -1);
		}

		pBegin = KVSP_curCharPointer;
		while(KVSP_curCharIsNumber)
			KVSP_skipChar;

		QString szNum2(pBegin, KVSP_curCharPointer - pBegin);
		int iNum2 = szNum2.toInt(&bOk);
		if(!bOk)
			qDebug("Oops! A non-number made by numbers (2)?");

		if(iNum1 < iNum2)
			return new KviKvsTreeNodeMultipleParameterIdentifier(pDollarBegin, iNum1, iNum2);
		else
		{
			warning(pBegin, __tr2qs_ctx("Ending index of a multiple parameter identifier is lower or equal to the starting index. This will evaluate to a single parameter identifier.", "kvs"));
			return new KviKvsTreeNodeSingleParameterIdentifier(pDollarBegin, iNum1);
		}
	}

	pBegin = KVSP_curCharPointer;

	//KVSP_skipChar;

	bool bHasNamespaceNotInObjScopeSoMustBeAlias = false; // ;D

	if(KVSP_curCharUnicode == '$')
	{
		if(bInObjScope)
		{
			error(KVSP_curCharPointer, __tr2qs_ctx("Syntax error: invalid $$ ($this) function call in object scope", "kvs"));
			return nullptr;
		}
		// handle $$
		KVSP_skipChar;
	}
	else
	{
		while((KVSP_curCharIsLetterOrNumber) || (KVSP_curCharUnicode == '_'))
			KVSP_skipChar;
		if(!bInObjScope)
		{
			while(KVSP_curCharUnicode == ':')
			{
				// check for namespaces

				// here we allow the syntax of the form
				// <namespace>::{<namespace>::}<alias_name>

				bHasNamespaceNotInObjScopeSoMustBeAlias = true; // ;D

				KVSP_skipChar;
				if(KVSP_curCharUnicode == ':')
				{
					KVSP_skipChar;

					if(!KVSP_curCharIsLetter)
					{
						warning(KVSP_curCharPointer - 1, __tr2qs_ctx("Stray '::' sequence or invalid following alias name", "kvs"));
						error(KVSP_curCharPointer, __tr2qs_ctx("Syntax error: malformed alias function call identifier", "kvs"));
						return nullptr;
					}

					KVSP_skipChar;
					while(KVSP_curCharIsLetterOrNumber || (KVSP_curCharUnicode == '_'))
						KVSP_skipChar;
				}
				else
				{
					warning(KVSP_curCharPointer - 1, __tr2qs_ctx("Stray ':' character: did you mean '...<namespace>::<alias_name>'?", "kvs"));
					error(KVSP_curCharPointer, __tr2qs_ctx("Syntax error: malformed (alias?) function call identifier", "kvs"));
					return nullptr;
				}
			}
		}
	}

	QString szIdentifier1(pBegin, KVSP_curCharPointer - pBegin);

	const QChar * pId2 = nullptr;
	int iId2Len = 0;
	bool bModuleFunctionCall = false;

	if(!bHasNamespaceNotInObjScopeSoMustBeAlias)
	{
		if(!bInObjScope)
		{
			if(KVSP_curCharUnicode == '.')
			{
				KVSP_skipChar;
				if(KVSP_curCharIsLetter)
				{
					pId2 = KVSP_curCharPointer;
					while((KVSP_curCharIsLetterOrNumber) || (KVSP_curCharUnicode == '_'))
						KVSP_skipChar;
					iId2Len = KVSP_curCharPointer - pId2;
					bModuleFunctionCall = true;
				}
				else
				{
					KVSP_backChar;
				}
			}
		}
		else
		{
			// object scope, check for "class name" namespace
			// the class name namespace has the format "<namespace>::<namespace>::..::<classname>
			// so the last :: is the delimiter of the function name
			const QChar * pOriginalEndOfId1 = KVSP_curCharPointer;
			const QChar * pEndOfId1 = pOriginalEndOfId1;

			while(KVSP_curCharUnicode == ':')
			{
				// base class function call ?
				KVSP_skipChar;
				if(KVSP_curCharUnicode == ':')
				{
					KVSP_skipChar;
					if(KVSP_curCharIsLetter)
					{
						pId2 = KVSP_curCharPointer;
						while((KVSP_curCharIsLetterOrNumber) || (KVSP_curCharUnicode == '_'))
							KVSP_skipChar;
						iId2Len = KVSP_curCharPointer - pId2;
					}
					else
					{
						KVSP_setCurCharPointer(pOriginalEndOfId1);
						pId2 = nullptr;
						iId2Len = 0;
						break;
					}
				}
				else
				{
					KVSP_setCurCharPointer(pOriginalEndOfId1);
					pId2 = nullptr;
					iId2Len = 0;
					break;
				}
			}
			if(pId2)
			{
				// yes, that's fine: reset it
				szIdentifier1.setUnicode(pBegin, pEndOfId1 - pBegin);
			}
		}
	}

	KviKvsTreeNodeDataList * l;

	if(KVSP_curCharUnicode != '(')
	{
		// no parameters passed
		//KVSP_setCurCharPointer(pBegin);
		// will get an empty data list
		l = new KviKvsTreeNodeDataList(pDollarBegin);
	}
	else
	{
		// check for the special syntax ()
		KVSP_skipChar;
		if(KVSP_curCharUnicode == ')')
		{
			// $call(), assume no parameters passed
			l = new KviKvsTreeNodeDataList(pDollarBegin);
			KVSP_skipChar;
		}
		else
		{
			KVSP_backChar;
			l = parseCommaSeparatedParameterList();
			if(!l)
				return nullptr; // error
		}
	}

	if(bHasNamespaceNotInObjScopeSoMustBeAlias)
	{
		// namespace::alias function call
		return new KviKvsTreeNodeAliasFunctionCall(pDollarBegin, szIdentifier1, l);
	}
	else if(bModuleFunctionCall)
	{
		// module function call
		return new KviKvsTreeNodeModuleFunctionCall(pDollarBegin, szIdentifier1, QString(pId2, iId2Len), l);
	}
	else
	{

		if(bInObjScope)
		{
			// object function call (our parent will be a scope operator)
			if(pId2)
			{
				// base class object function call
				return new KviKvsTreeNodeBaseObjectFunctionCall(pDollarBegin, szIdentifier1, QString(pId2, iId2Len), l);
			}
			else
			{
				// plain object function call
				return new KviKvsTreeNodeThisObjectFunctionCall(pDollarBegin, szIdentifier1, l);
			}
		}
		else
		{
			// core or alias function call
			KviKvsCoreFunctionExecRoutine * r = KviKvsKernel::instance()->findCoreFunctionExecRoutine(szIdentifier1);
			if(r)
			{
				// core function call
				return new KviKvsTreeNodeCoreFunctionCall(pDollarBegin, szIdentifier1, r, l);
			}
			else
			{
				// root namespace alias function call
				return new KviKvsTreeNodeAliasFunctionCall(pDollarBegin, szIdentifier1, l);
			}
		}
	}

	// not reached
	KVSP_ASSERT(false);
	return nullptr;
}
