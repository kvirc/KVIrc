#ifndef _KVI_KVS_PARSER_H_
#define _KVI_KVS_PARSER_H_
//=============================================================================
//
//   File : KviKvsParser.h
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

#include "kvi_settings.h"
#include "KviQString.h"
#include "KviPointerList.h"
#include "KviPointerHashTable.h"

class KviKvsScript;
class KviKvsKernel;
class KviWindow;
class KviKvsTreeNode;
class KviKvsTreeNodeInstruction;
class KviKvsTreeNodeInstructionBlock;
class KviKvsTreeNodeCommand;
class KviKvsTreeNodeDataList;
class KviKvsTreeNodeData;
class KviKvsTreeNodeVariable;
class KviKvsTreeNodeVariableReference;
class KviKvsTreeNodeConstantData;
class KviKvsTreeNodeSwitchList;
class KviKvsTreeNodeSpecialCommand;
class KviKvsTreeNodeExpression;
class KviKvsTreeNodeExpressionBinaryOperator;
class KviKvsTreeNodeFunctionCall;
class KviKvsTreeNodeOperation;
class KviKvsTreeNodeSpecialCommandDefpopupLabelPopup;

// This is an ONE-TIME parser used by KviKvsScript

class KVIRC_API KviKvsParser
{
	friend class KviKvsKernel;

public:
	KviKvsParser(KviKvsScript * pScript, KviWindow * pOutputWindow);
	~KviKvsParser();

private:
	const QChar * m_pBuffer = nullptr; // the local pointer to the beginning of the buffer
	const QChar * m_ptr = nullptr;     // the parsing pointer
	// parsing state
	KviPointerHashTable<QString, QString> * m_pGlobals; // the dict of the vars declared with global in this script
	int m_iFlags = 0;                                   // the current parsing flags
	bool m_bError = false;                              // error(..) was called ?
	// this stuff is used only for reporting errors and warnings
	KviKvsScript * m_pScript; // parent script
	KviWindow * m_pWindow;    // output window
public:                       // public interface
	enum Flags
	{
		AssumeLocals = 1,
		Pedantic = 2
	};
	// was there an error ?
	bool error() const { return m_bError; };
	// parses the buffer pointed by pBuffer and returns
	// a syntax tree or 0 in case of failure
	// if the parsing fails, the error code can be retrieved by calling error()
	KviKvsTreeNodeInstruction * parse(const QChar * pBuffer, int iFlags = 0);
	KviKvsTreeNodeInstruction * parseAsExpression(const QChar * pBuffer, int iFlags = 0);
	KviKvsTreeNodeInstruction * parseAsParameter(const QChar * pBuffer, int iFlags = 0);

private: // parsing helpers
	// generic
	void skipSpaces();            // skips only spaces and tabs (eventually with \)
	bool skipSpacesAndNewlines(); // skips space and newlines
	void skipToNextLine();        // skips up to a new line
	// dedicated
	void skipToEndOfCStyleComment();
	// dedicated to parseSpecialCommandFor() : in KviKvsParser_specialCommands.cpp
	bool skipToEndOfForControlBlock();
	// error handlers
	void error(const QChar * pLocation, QString szMsgFmt, ...);
	void warning(const QChar * pLocation, QString szMsgFmt, ...);
	void errorBadChar(const QChar * pLocation, char cExpected, const char * szCommandName);

protected:
	// this is called by KviKvsKernel to register the parsing routines
	static void init();

private:
	// returns nullptr only in case of error
	// starts on the first char of a buffer
	// stops at the first null char encountered
	KviKvsTreeNodeInstruction * parseInstructionList();
	// may return nullptr (empty instruction), check error() for error conditions
	// starts on the first character of an instruction
	// if the first char is ';' '\n' or null it just returns nullptr without error
	// stops after the ending char of the instruction
	KviKvsTreeNodeInstruction * parseInstruction();
	// may return nullptr (empty block), check error() for error conditions
	// starts at the leading '{' of the block
	// stops after the trailing '}' of the block
	KviKvsTreeNodeInstruction * parseInstructionBlock();
	// returns nullptr only in case of error
	// starts on the first character of the parameters
	// ends after the end of the command
	KviKvsTreeNodeDataList * parseCommandParameterList();
	// returns nullptr only in case of error
	// starts on the leading '(' or a ',' in the middle of the list
	// ends after the trailing ')'
	// if started in the middle of the list returns only the remaining
	// parameters.
	KviKvsTreeNodeDataList * parseCommaSeparatedParameterList();
	KviPointerList<QString> * parseCommaSeparatedParameterListNoTree();
	// returns nullptr in case of error or if it starts on a terminating character (null parameter)
	// check error() to see if there was an error condition (unless you already know that
	// there was a valid first character)
	// start on the first character of the parameter
	// ends after the first character not included in the param (';','\n','\0',' ')
	// If bPreferNumeric is true then when a single literal parameter
	// is extracted an attempt to convert it to a numeric format is made.
	// This optimizes assignments, self-sums etc...
	KviKvsTreeNodeData * parseCommandParameter(bool bPreferNumeric = false);
	// returns nullptr only in case of error
	// start on the first character of the parameter
	// ends after the first character not included in the param (')','\n','\0',',')
	KviKvsTreeNodeData * parseCommaSeparatedParameter();
	// returns nullptr only in case of error
	// start on the first character of the parameter
	// ends after the first character not included in the param (')','\n','\0')
	KviKvsTreeNodeData * parseSingleParameterInParenthesis();
	// never returns nullptr
	KviKvsTreeNodeConstantData * parseCommandLiteralParameter();
	// never returns nullptr
	KviKvsTreeNodeConstantData * parseCommaSeparatedLiteralParameter();
	// never returns nullptr
	KviKvsTreeNodeConstantData * parseSingleLiteralParameterInParenthesis();
	// returns nullptr only in case of error
	// starts at the leading '"'
	// ends after the trailing '"'
	KviKvsTreeNodeData * parseStringParameter();
	// never returns nullptr
	KviKvsTreeNodeConstantData * parseStringLiteralParameter();
	// returns nullptr in case of error or of an empty switch list (check the error code!)
	// starts at the leading '-' of the first switch
	// ends after the last switch
	KviKvsTreeNodeSwitchList * parseCommandSwitchList();
	// returns nullptr only in case of error
	// starts at '%' or '$'
	// and ends after the end of the data reference
	// or just after the '%' or '$' if this was only a ConstandData (not a var or func)
	KviKvsTreeNodeData * parseParameterPercentOrDollar();
	// returns nullptr only in case of error
	// starts at '%' or '$'
	// ends after the end of the complete data reference (including scope operators!)
	KviKvsTreeNodeData * parsePercentOrDollar(bool bInObjScope = false);
	// returns nullptr only in case of error
	// starts at '%'
	// ends after the end of the structured data
	KviKvsTreeNodeVariable * parsePercent(bool bInObjectScope = false);
	// returns nullptr only in case of error
	KviKvsTreeNodeData * parseHashKey();
	// never returns nullptr
	KviKvsTreeNodeConstantData * parseHashKeyLiteralParameter();

	//
	// KviKvsParser_specialCommands.cpp
	//

	// return nullptr only in case of error
	// starts at the leading '(' of the if command (after the switches)
	// and stops after the end of the else block
	// if the first character is not '(' then this function fails with an error
	KviKvsTreeNodeCommand * parseSpecialCommandIf();
	// always returns nullptr
	// check error() for error conditions
	// starts after the switches of the "global" keyword
	// and stops at the end of the command
	// if the first character is not '%' of a variable then this function fails with an error
	KviKvsTreeNodeCommand * parseSpecialCommandGlobal();
	// returns nullptr only in case of error
	// starts at the leading '(' of the while command (after the switches)
	// and stops after the end of the command block
	// if the first character is not '(' then this function fails with an error
	KviKvsTreeNodeCommand * parseSpecialCommandWhile();
	// returns nullptr only in case of error
	// starts at the leading '(' of the while command (after the switches)
	// and stops after the end of the command block
	// if the first character is not '(' then this function fails with an error
	KviKvsTreeNodeCommand * parseSpecialCommandDo();
	// returns nullptr only in case of error
	// and stops after the end of the break command
	KviKvsTreeNodeCommand * parseSpecialCommandBreak();
	// returns nullptr only in case of error
	// and jumps to the next iteration after the end of the continue command
	KviKvsTreeNodeCommand * parseSpecialCommandContinue();
	// returns nullptr only in case of error
	// and stops after the end of the for command block
	KviKvsTreeNodeCommand * parseSpecialCommandFor();
	// returns nullptr only in case of error
	// and stops after the end of the foreach command block
	KviKvsTreeNodeCommand * parseSpecialCommandForeach();
	// returns nullptr only in case of error
	// and stops after the end of the switch command block
	KviKvsTreeNodeCommand * parseSpecialCommandSwitch();
	// returns nullptr only in case of error
	// and stops after the end of the defpopup command block
	KviKvsTreeNodeCommand * parseSpecialCommandUnset();
	// returns nullptr only in case of error
	// and stops after the end of the defpopup command block
	KviKvsTreeNodeCommand * parseSpecialCommandDefpopup();
	KviKvsTreeNodeSpecialCommandDefpopupLabelPopup * parseSpecialCommandDefpopupLabelPopup();
	// returns nullptr only in case of error
	// stops after the class command block
	KviKvsTreeNodeCommand * parseSpecialCommandClass();
	// returns nullptr only in case of error
	// stops after the perl.end statement
	KviKvsTreeNodeCommand * parseSpecialCommandPerlBegin();
	// returns nullptr only in case of error
	// stops after the perl.end statement
	KviKvsTreeNodeCommand * parseSpecialCommandPythonBegin();
	// returns nullptr only in case of error
	// and stops after the end of the help command
	KviKvsTreeNodeCommand * parseSpecialCommandHelp();

	//
	// KviKvsParser_command.cpp
	//

	// may return nullptr (empty command), check error() for error conditions
	// starts at the beginning of a command (can be non valid)
	// ends after the ending char of the command
	KviKvsTreeNodeCommand * parseCommand();

	//
	// KviKvsParser_comment.cpp
	//

	// always returns nullptr, and it CAN be an error!
	// starts at the beginning of a comment (must be '#' or '/')
	// ends after the ending char of the comment
	KviKvsTreeNode * parseComment();

	//
	// KviKvsParser_dollar.cpp
	//

	// returns nullptr only in case of error
	// starts at '$'
	// ends after the end of the function call
	KviKvsTreeNodeData * parseDollar(bool bInObjScope = false);

	// returns nullptr only in case of error
	// starts at '@'
	// ends after the end of the function call
	KviKvsTreeNodeData * parseAt(bool bInObjScope = false);

	//
	// KviKvsParser_lside.cpp
	//

	// returns nullptr only in case of error
	// returns after the command terminator
	KviKvsTreeNodeInstruction * parseVoidFunctionCallOrOperation();
	// returns nullptr only in case of error
	// returns after the command terminator
	KviKvsTreeNodeOperation * parseOperation();
	// returns nullptr only in case of error
	// returns after the command terminator
	// If bPreferNumeric is propagated to parseCommandParameter() function
	KviKvsTreeNodeData * parseOperationRightSide(bool bPreferNumeric = false);
	// return nullptr only in case of error
	// returns after the command terminator
	KviKvsTreeNodeOperation * parseBindingOperation();
	KviKvsTreeNodeConstantData * parseBindingOperationLiteralParameter();
	KviKvsTreeNodeData * parseBindingOperationParameter();

	//
	// KviKvsParser_expression.cpp
	//

	// returns nullptr only in case of error
	// starts AFTER the leading char of the expression
	// ends after the first terminator found
	KviKvsTreeNodeExpression * parseExpression(char terminator);
	KviKvsTreeNodeExpressionBinaryOperator * parseExpressionBinaryOperator();
	KviKvsTreeNodeExpression * parseExpressionOperand(char terminator);
	KviKvsTreeNodeExpression * parseExpressionOperandCore(char terminator);
	bool parseExpressionMightPointToOperator();

	void report(bool bError, const QChar * pLocation, const QString & szMsgFmt, kvi_va_list va);
};

#endif //!_KVI_KVS_PARSER_H_
