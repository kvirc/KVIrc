//=============================================================================
//
//   File : KviKvsParser_expression.cpp
//   Creation date : Mon 6 Oct 2003 01.31 CEST by Szymon Stefanek
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

//#warning "FIXME: expression eval doc!"

/*
	@doc: expressioneval
	@type:
		language
	@title:
		Expression evaluation identifier
	@syntax:
		$(<expression>)
	@keyterms:
		expressions
	@short:
		Expression evaluation identifier
	@description:
		Evaluates <expression> and returns its result.[br]
		If <expression> is a single string, array or hash, it is returned unmodified.[br]
		In any other case the expression evaluation returns a numeric value, either real or integer.[br]
		The expressions are really close to the C ones and have some minor extensions.[br]
		The supported operators are [b]+[/b], [b]-[/b], [b]*[/b], [b]/[/b], [b]|[/b], [b]&[/b], [b]^[/b], [b]||[/b], [b]&&[/b], [b]^^[/b], [b]>>[/b], [b]<<[/b], [b]<[/b], [b]>,[/b] [b]<=,[/b] [b]>=,[/b] [b]==,[/b] [b]!=[/b] and [b]<>[/b] (synonym for !=).[br]
		The following table describes their meaning.
		[table]
			[tr][td][b]Operator[/b][/td][td][b]Description[/b][/td][/tr]
			[tr][td]a + b[/td][td]Arithmetic sum: valid only for numeric operands[/td][/tr]
			[tr][td]a - b[/td][td]Arithmetic subtraction: valid only for numeric operands[/td][/tr]
			[tr][td]a / b[/td][td]Arithmetic division: valid only for numeric operands[/td][/tr]
			[tr][td]a * b[/td][td]Arithmetic multiplication: valid only for numeric operands[/td][/tr]
			[tr][td]a % b[/td][td]Arithmetic modulus: valid only for numeric operands[/td][/tr]
			[tr][td]a || b[/td][td]Logical or: valid only for boolean operands[/td][/tr]
			[tr][td]a && b[/td][td]Logical and: valid only for boolean operands[/td][/tr]
			[tr][td]a ^^ b[/td][td]Logical xor: valid only for boolean operands[/td][/tr]
			[tr][td]a >> b[/td][td]Bitwise shift right: valid only for integer operands[/td][/tr]
			[tr][td]a << b[/td][td]Bitwise shift left: valid only for integer operands[/td][/tr]
			[tr][td]a | b[/td][td]Bitwise or: valid only for integer operands[/td][/tr]
			[tr][td]a & b[/td][td]Bitwise and: valid only for integer operands[/td][/tr]
			[tr][td]a ^ b[/td][td]Bitwise xor: valid only for integer operands[/td][/tr]
			[tr][td]a > b[/td][td]Greater than: valid for numeric or string operands. Case sensitive[/td][/tr]
			[tr][td]a < b[/td][td]Lower than: valid for numeric or string operands. Case sensitive[/td][/tr]
			[tr][td]a >= b[/td][td]Greater or equal to: valid for numeric or string operands. Case sensitive[/td][/tr]
			[tr][td]a <= b[/td][td]Lower or equal to: valid for numeric or string operands. Case sensitive[/td][/tr]
			[tr][td]a != b[/td][td]Not equal to: valid for numeric or string operands. Case sensitive[/td][/tr]
			[tr][td]a == b[/td][td]Equal to: valid for numeric or string operands. Case sensitive[/td][/tr]
		[/table]
		The expressions can contain integer, real or string constants and variable operands.[br]
		The integer constants can be also specified as hexadecimal numbers by prefixing them by '0x'.[br]
		The string constants should be enclosed in quotes.[br]
	@examples:
		[example]
			echo $(10 + 5 * 100)
			echo $(10 / 3)
			echo $(10 / 3.0)
			echo $(10.0 + 5 * 100)
			echo $(145 & 2)
			echo $("hello" > "ciao")
			echo $(10 == "10")
			%a = 100
			%b = 50.3
			%c = "test"
			echo $(%a + %b)
			echo $("%a%b" + 1)
			echo $(%a + %b > %c)
			echo $(-(10 + 20) * 3)
			echo $(1 ^ 2)
			echo $(1 ^ 1)
			echo $(0xffff == 65535)
			...
		[/example]
*/

KviKvsTreeNodeExpressionBinaryOperator * KviKvsParser::parseExpressionBinaryOperator()
{
	switch(KVSP_curCharUnicode)
	{
		case '=':
			KVSP_skipChar;
			if(KVSP_curCharUnicode == '=')
			{
				KVSP_skipChar;
				return new KviKvsTreeNodeExpressionBinaryOperatorEqualTo(KVSP_curCharPointer);
			}
			else
			{
				error(KVSP_curCharPointer, __tr2qs_ctx("Unknown binary operator '=%q': did you mean '=='?", "kvs"), KVSP_curCharPointer);
			}
			break;
		case '!':
			KVSP_skipChar;
			if(KVSP_curCharUnicode == '=')
			{
				KVSP_skipChar;
				return new KviKvsTreeNodeExpressionBinaryOperatorNotEqualTo(KVSP_curCharPointer);
			}
			break;
		case '+':
			KVSP_skipChar;
			return new KviKvsTreeNodeExpressionBinaryOperatorSum(KVSP_curCharPointer);
			break;
		case '-':
			KVSP_skipChar;
			return new KviKvsTreeNodeExpressionBinaryOperatorSubtraction(KVSP_curCharPointer);
			break;
		case '/':
			KVSP_skipChar;
			return new KviKvsTreeNodeExpressionBinaryOperatorDivision(KVSP_curCharPointer);
			break;
		case '%':
			KVSP_skipChar;
			return new KviKvsTreeNodeExpressionBinaryOperatorModulus(KVSP_curCharPointer);
			break;
		case '*':
			KVSP_skipChar;
			return new KviKvsTreeNodeExpressionBinaryOperatorMultiplication(KVSP_curCharPointer);
			break;
		case '&':
			KVSP_skipChar;
			if(KVSP_curCharUnicode == '&')
			{
				KVSP_skipChar;
				return new KviKvsTreeNodeExpressionBinaryOperatorAnd(KVSP_curCharPointer);
			}
			return new KviKvsTreeNodeExpressionBinaryOperatorBitwiseAnd(KVSP_curCharPointer);
			break;
		case '|':
			KVSP_skipChar;
			if(KVSP_curCharUnicode == '|')
			{
				KVSP_skipChar;
				return new KviKvsTreeNodeExpressionBinaryOperatorOr(KVSP_curCharPointer);
			}
			return new KviKvsTreeNodeExpressionBinaryOperatorBitwiseOr(KVSP_curCharPointer);
			break;
		case '^':
			KVSP_skipChar;
			if(KVSP_curCharUnicode == '^')
			{
				KVSP_skipChar;
				return new KviKvsTreeNodeExpressionBinaryOperatorXor(KVSP_curCharPointer);
			}
			return new KviKvsTreeNodeExpressionBinaryOperatorBitwiseXor(KVSP_curCharPointer);
			break;
		case '>':
			KVSP_skipChar;
			switch(KVSP_curCharUnicode)
			{
				case '>':
					KVSP_skipChar;
					return new KviKvsTreeNodeExpressionBinaryOperatorShiftRight(KVSP_curCharPointer);
					break;
				case '=':
					KVSP_skipChar;
					return new KviKvsTreeNodeExpressionBinaryOperatorGreaterOrEqualTo(KVSP_curCharPointer);
					break;
				default:
					return new KviKvsTreeNodeExpressionBinaryOperatorGreaterThan(KVSP_curCharPointer);
					break;
			}
			break;
		case '<':
			KVSP_skipChar;
			switch(KVSP_curCharUnicode)
			{
				case '>':
					KVSP_skipChar;
					return new KviKvsTreeNodeExpressionBinaryOperatorNotEqualTo(KVSP_curCharPointer);
					break;
				case '<':
					KVSP_skipChar;
					return new KviKvsTreeNodeExpressionBinaryOperatorShiftLeft(KVSP_curCharPointer);
					break;
				case '=':
					KVSP_skipChar;
					return new KviKvsTreeNodeExpressionBinaryOperatorLowerOrEqualTo(KVSP_curCharPointer);
					break;
				default:
					return new KviKvsTreeNodeExpressionBinaryOperatorLowerThan(KVSP_curCharPointer);
					break;
			}
			break;
	}

	error(KVSP_curCharPointer, __tr2qs_ctx("Unknown binary operator '%q'", "kvs"), KVSP_curCharPointer);
	return nullptr;
}

static unsigned char binary_operator_initial_char[256] = {
	// clang-format off
	//	000 001 002 003 004 005 006 007   008 009 010 011 012 013 014 015
	//	NUL SOH STX ETX EOT ENQ ACK BEL   BS  HT  LF  VT  FF  CR  SO  SI
		0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
	//	016 017 018 019 020 021 022 023   024 025 026 027 028 029 030 031
	//	DLE DC1 DC2 DC3 DC4 NAK SYN ETB   CAN EM  SUB ESC FS  GS  RS  US
		0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
	//	032 033 034 035 036 037 038 039   040 041 042 043 044 045 046 047
	//	    !   "   #   $   %   &   '     (   )   *   +   ,   -   .   /
		0  ,1  ,0  ,0  ,0  ,1  ,1  ,0    ,0  ,0  ,1  ,1  ,0  ,1  ,0  ,1  ,
	//	048 049 050 051 052 053 054 055   056 057 058 059 060 061 062 063
	//	0   1   2   3   4   5   6   7     8   9   :   ;   <   =   >   ?
		0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,1  ,1  ,1  ,0  ,
	//	064 065 066 067 068 069 070 071   072 073 074 075 076 077 078 079
	//	@   A   B   C   D   E   F   G     H   I   J   K   L   M   N   O
		0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
	//	080 081 082 083 084 085 086 087   088 089 090 091 092 093 094 095
	//	P   Q   R   S   T   U   V   W     X   Y   Z   [   \   ]   ^   _
		0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,1  ,0  ,
	//	096 097 098 099 100 101 102 103   104 105 106 107 108 109 110 111
	//	`   a   b   c   d   e   f   g     h   i   j   k   l   m   n   o
		0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
	//	112 113 114 115 116 117 118 119   120 121 122 123 124 125 126 127
	//	p   q   r   s   t   u   v   w     x   y   z   {   |   }   ~   
		0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,1  ,0  ,0  ,0  ,
	//	128 129 130 131 132 133 134 135   136 137 138 139 140 141 142 143
	//
		0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
	//	144 145 146 147 148 149 150 151   152 153 154 155 156 157 158 159
	//
		0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
	//	160 161 162 163 164 165 166 167   168 169 170 171 172 173 174 175
	//
		0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
	//	176 177 178 179 180 181 182 183   184 185 186 187 188 189 190 191
	//
		0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
	//	192 193 194 195 196 197 198 199   200 201 202 203 204 205 206 207
	//	�  �  �  �  �  �  �  �    �  �  �  �  �  �  �  �
		0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
	//	208 209 210 211 212 213 214 215   216 217 218 219 220 221 222 223
	//	�  �  �  �  �  �  �  �    �  �  �  �  �  �  �  �
		0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
	//	224 225 226 227 228 229 230 231   232 233 234 235 236 237 238 239
	//	�  �  �  �  �  �  �  �    �  �  �  �  �  �  �  �
		0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
	//	240 241 242 243 244 245 246 247   248 249 250 251 252 253 254 255
	//	�  �  �  �  �  �  �  �
		0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0
	// clang-format on
};

bool KviKvsParser::parseExpressionMightPointToOperator()
{
	if(KVSP_curCharUnicode == '%')
	{
		KVSP_skipChar;
		if(KVSP_curCharIsLetter || (KVSP_curCharUnicode == '_'))
		{
			// a variable, probably
			KVSP_backChar;
			return false;
		}
		KVSP_backChar;
		return true;
	}
	if(KVSP_curCharUnicode > 255)
		return false;
	return binary_operator_initial_char[KVSP_curCharUnicode] != 0;
}

KviKvsTreeNodeExpression * KviKvsParser::parseExpressionOperand(char terminator)
{
	switch(KVSP_curCharUnicode)
	{
		case 0:
		case '\r':
		case '\n':
			error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of script in expression", "kvs"));
			return nullptr;
			break;
		case '(':
			KVSP_skipChar;
			skipSpaces();
			return parseExpression(')'); // sub expression
			break;
		case '-':
		{
			KVSP_skipChar;
			skipSpaces();
			KviKvsTreeNodeExpression * d = parseExpressionOperand(terminator);
			if(!d)
				return nullptr;
			return new KviKvsTreeNodeExpressionUnaryOperatorNegate(d->location(), d);
		}
		break;
		case '!':
		{
			KVSP_skipChar;
			skipSpaces();
			KviKvsTreeNodeExpression * d = parseExpressionOperand(terminator);
			if(!d)
				return nullptr;
			return new KviKvsTreeNodeExpressionUnaryOperatorLogicalNot(d->location(), d);
		}
		break;
		case '~':
		{
			KVSP_skipChar;
			skipSpaces();
			KviKvsTreeNodeExpression * d = parseExpressionOperand(terminator);
			if(!d)
				return nullptr;
			return new KviKvsTreeNodeExpressionUnaryOperatorBitwiseNot(d->location(), d);
		}
		break;
		default:
			// anything else at this point is an operand core
			return parseExpressionOperandCore(terminator);
			break;
	}

	// not reached
	KVSP_ASSERT(false);
	return nullptr;
}

KviKvsTreeNodeExpression * KviKvsParser::parseExpressionOperandCore(char terminator)
{
	KviPointerList<KviKvsTreeNodeData> * pDataList = new KviPointerList<KviKvsTreeNodeData>;
	pDataList->setAutoDelete(true);

	static QString szStaticSingleSpace(" ");

	const QChar * pOperandBegin = KVSP_curCharPointer;

	bool bHaveVariable = false;

	for(;;)
	{
		switch(KVSP_curCharUnicode)
		{
			case 0:
			case '\r':
			case '\n':
				error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of script in expression", "kvs"));
				delete pDataList;
				return nullptr;
				break;
			case ' ':
			case '\t':
				skipSpaces();
				if((KVSP_curCharUnicode == terminator) || parseExpressionMightPointToOperator())
					goto postprocess_operand;
				// separate by single spaces
				pDataList->append(new KviKvsTreeNodeConstantData(KVSP_curCharPointer, new KviKvsVariant(szStaticSingleSpace)));
				break;
			case '"':
			{
				// a string
				bHaveVariable = true;
				KviKvsTreeNodeData * d = parseStringParameter();
				if(!d)
				{
					delete pDataList;
					return nullptr;
				}
				pDataList->append(d);
			}
			break;
			case '%':
			case '$':
			case '@':
			{
				bHaveVariable = true;
				KviKvsTreeNodeData * d = parseParameterPercentOrDollar();
				if(!d)
				{
					delete pDataList;
					return nullptr;
				}
				pDataList->append(d);
			}
			break;
			default:
				// literal ?
				if(KVSP_curCharIsLetterOrNumber || (KVSP_curCharUnicode == '.') || (KVSP_curCharUnicode == '_'))
				{
					const QChar * pBegin = KVSP_curCharPointer;
					while(KVSP_curCharIsLetterOrNumber || (KVSP_curCharUnicode == '.') || (KVSP_curCharUnicode == '_'))
						KVSP_skipChar;
					QString tmp(pBegin, KVSP_curCharPointer - pBegin);
					bool bOk;
					kvs_int_t iVal = tmp.toLong(&bOk);
					if(bOk)
					{
						pDataList->append(new KviKvsTreeNodeConstantData(pBegin, new KviKvsVariant(iVal)));
					}
					else
					{
						// bOk is false
						if(pBegin->unicode() == '0')
						{
							if(tmp.length() > 2)
							{
								if((tmp[1] == 'x') || (tmp[1] == 'X'))
								{
									// hexadecimal constant ?
									QString hex = tmp.right(tmp.length() - 2);
									iVal = hex.toLong(&bOk, 16);
									if(bOk)
									{
										pDataList->append(new KviKvsTreeNodeConstantData(pBegin, new KviKvsVariant(iVal)));
									}
								}
							}
						}
						if(!bOk)
						{
							kvs_real_t dVal = tmp.toDouble(&bOk);
							if(bOk)
							{
								pDataList->append(new KviKvsTreeNodeConstantData(pBegin, new KviKvsVariant(dVal)));
							}
							else
							{
								pDataList->append(new KviKvsTreeNodeConstantData(pBegin, new KviKvsVariant(tmp)));
							}
						}
					}
				}
				else
				{
					error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected character %q (Unicode %h) in expression. If it meant to be a string use the quotes.", "kvs"), KVSP_curCharPointer, KVSP_curCharUnicode);
					delete pDataList;
					return nullptr;
				}
				break;
		}

		if((KVSP_curCharUnicode == terminator) || parseExpressionMightPointToOperator())
			break;
	}

postprocess_operand:

	if(pDataList->count() == 0)
	{
		delete pDataList;
		error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected empty expression operand", "kvs"));
		return nullptr;
	}

	if(pDataList->count() > 1)
		return new KviKvsTreeNodeExpressionVariableOperand(pOperandBegin, new KviKvsTreeNodeCompositeData(pOperandBegin, pDataList));

	KviKvsTreeNodeData * pUniqueData = pDataList->first();

	if(bHaveVariable)
	{
		pDataList->setAutoDelete(false);
		delete pDataList;
		return new KviKvsTreeNodeExpressionVariableOperand(pOperandBegin, pUniqueData);
	}

	// a single constant data element
	KviKvsTreeNodeExpressionConstantOperand * op = new KviKvsTreeNodeExpressionConstantOperand(pOperandBegin, new KviKvsVariant(*(((KviKvsTreeNodeConstantData *)pUniqueData)->value())));
	delete pDataList; // auto delete is true
	return op;
}

KviKvsTreeNodeExpression * KviKvsParser::parseExpression(char terminator)
{
	// we're inside the expression now
	skipSpaces();

	if(KVSP_curCharUnicode == terminator)
	{
		// empty expression
		// constant 0 ?
		KVSP_skipChar;
		return new KviKvsTreeNodeExpressionConstantOperand(KVSP_curCharPointer, new KviKvsVariant((kvs_int_t)0));
	}

	KviKvsTreeNodeExpression * left = parseExpressionOperand(terminator);
	if(!left)
		return nullptr;
	QString sz;
	left->contextDescription(sz);

	skipSpaces();

	if(KVSP_curCharUnicode == terminator)
	{
		KVSP_skipChar;
		return left;
	}

	// not a terminator... must be an operator (or an error, eventually)

	KviKvsTreeNodeExpression * curTopOperator = parseExpressionBinaryOperator();
	if(!curTopOperator)
	{
		delete left;
		return nullptr; // error
	}
	curTopOperator->contextDescription(sz);

	curTopOperator->setLeft(left);

	// ok.. parse the right side
	// Now curTopOperator has the left subtree (one node) set
	// and it points to the TOP (=ROOT) node
	// Evaluate the rest

	KviKvsTreeNodeExpression * operand;
	KviKvsTreeNodeExpression * incompleteOperator = curTopOperator;
	KviKvsTreeNodeExpression * auxOperator;

	for(;;)
	{
		skipSpaces();

		operand = parseExpressionOperand(terminator);
		if(!operand)
		{
			delete curTopOperator;
			return nullptr;
		}
		operand->contextDescription(sz);

		skipSpaces();

		if(KVSP_curCharUnicode == terminator)
		{
			KVSP_skipChar;
			incompleteOperator->setRight(operand);
			return curTopOperator;
		}

		auxOperator = parseExpressionBinaryOperator();
		if(!auxOperator)
		{
			delete curTopOperator;
			delete operand;
			return nullptr;
		}

		auxOperator->contextDescription(sz);

		//now compare operators...
		if(incompleteOperator->precedence() > auxOperator->precedence())
		{
			// This in fact means that incomplete has LOWER precedence than
			// aux and thus aux should be done first.
			incompleteOperator->setRight(auxOperator);
			auxOperator->setLeft(operand);
		}
		else
		{
			// incomplete has GREATER precedence than aux and thus aux should be done first
			incompleteOperator->setRight(operand); //right tree complete
			// go up until we find an operator with lower precedence than auxOperator (>=)
			KviKvsTreeNodeExpression * tempOperator = incompleteOperator->parentWithPrecedenceLowerThan(auxOperator->precedence());
			if(tempOperator == nullptr)
			{
				auxOperator->setLeft(curTopOperator);
				curTopOperator = auxOperator;
			}
			else
			{
				KVSP_ASSERT(tempOperator->right());
				auxOperator->setLeft(tempOperator->right());
				tempOperator->setRight(auxOperator);
			}
		}
		incompleteOperator = auxOperator;
		KVSP_ASSERT(incompleteOperator->right() == nullptr);
	}

	KVSP_ASSERT(false);

	return nullptr; //newer here
}
