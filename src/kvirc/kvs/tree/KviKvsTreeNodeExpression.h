#ifndef _KVI_KVS_TREENODE_EXPRESSION_H_
#define _KVI_KVS_TREENODE_EXPRESSION_H_
//=============================================================================
//
//   File : KviKvsTreeNodeExpression.h
//   Creation date : Mon 06 Oct 2003 01.33 CEST by Szymon Stefanek
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

#include "KviQString.h"
#include "KviKvsVariant.h"
#include "KviKvsTreeNodeData.h"

// absolute precedence (~operand part)
#define PREC_MAXIMUM -10

#define PREC_OP_LOGICALNOT -3 /* !  */
#define PREC_OP_BITWISENOT -2 /* ~  */
#define PREC_OP_NEGATE -1     /* -  */
// high precedence
#define PREC_OP_BITWISEAND 0 /* &  */
#define PREC_OP_BITWISEOR 1  /* |  */
#define PREC_OP_BITWISEXOR 2 /* ^  */

#define PREC_OP_SHIFTRIGHT 3 /* >> */
#define PREC_OP_SHIFTLEFT 3  /* << */

#define PREC_OP_MULTIPLICATION 5 /* *  */
#define PREC_OP_DIVISION 5       /* /  */
#define PREC_OP_MODULUS 5        /* %  */

#define PREC_OP_SUM 8         /* +  */
#define PREC_OP_SUBTRACTION 8 /* -  */

#define PREC_OP_GREATERTHAN 11 /* >  */ /* Case sensitive (normal for numbers) */
#define PREC_OP_LOWERTHAN 11 /* <  */   /* Case sensitive (normal for numbers) */

#define PREC_OP_EQUALTO 12 /* == */ /* Case sensitive comparison for strings or normal comp.fr numbers */

#define PREC_OP_GREATEROREQUALTO 14 /* >= */ /* Case sensitive (normal for numbers) */
#define PREC_OP_LOWEROREQUALTO 14 /* <= */   /* Case sensitive (normal for numbers) */

#define PREC_OP_NOTEQUALTO 15 /* != */ /* Case sensitive (normal for numbers) */

#define PREC_OP_AND 16 /* && */
#define PREC_OP_OR 17  /* || */
#define PREC_OP_XOR 18 /* ^^ */
// low precedence

class KVIRC_API KviKvsTreeNodeExpression : public KviKvsTreeNodeData
{
	// this class is never instantiated
public:
	KviKvsTreeNodeExpression(const QChar * pLocation);
	~KviKvsTreeNodeExpression();

protected:
	KviKvsTreeNodeExpression * m_pParentExpression;

public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual int precedence();
	virtual KviKvsTreeNodeExpression * left();
	virtual KviKvsTreeNodeExpression * right();
	virtual void setLeft(KviKvsTreeNodeExpression * pLeft);
	virtual void setRight(KviKvsTreeNodeExpression * pRight);
	virtual int firstBinaryOperator();
	KviKvsTreeNodeExpression * parentExpression() { return m_pParentExpression; };
	void setParentExpression(KviKvsTreeNodeExpression * pParent) { m_pParentExpression = pParent; };
	virtual KviKvsTreeNodeExpression * parentWithPrecedenceLowerThan(int iPrec);
};

class KVIRC_API KviKvsTreeNodeExpressionVariableOperand : public KviKvsTreeNodeExpression
{
public:
	KviKvsTreeNodeExpressionVariableOperand(const QChar * pLocation, KviKvsTreeNodeData * pData);
	~KviKvsTreeNodeExpressionVariableOperand();

protected:
	KviKvsTreeNodeData * m_pData; // can't be null
public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pResult);
};

class KVIRC_API KviKvsTreeNodeExpressionConstantOperand : public KviKvsTreeNodeExpression
{
public:
	KviKvsTreeNodeExpressionConstantOperand(const QChar * pLocation, KviKvsVariant * pConstant);
	~KviKvsTreeNodeExpressionConstantOperand();

public:
	KviKvsVariant * m_pConstant;

public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pResult);
};

class KVIRC_API KviKvsTreeNodeExpressionOperator : public KviKvsTreeNodeExpression
{
public:
	KviKvsTreeNodeExpressionOperator(const QChar * pLocation);
	~KviKvsTreeNodeExpressionOperator();

public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
};

class KVIRC_API KviKvsTreeNodeExpressionUnaryOperator : public KviKvsTreeNodeExpressionOperator
{
public:
	KviKvsTreeNodeExpressionUnaryOperator(const QChar * pLocation, KviKvsTreeNodeExpression * pData);
	~KviKvsTreeNodeExpressionUnaryOperator();

protected:
	KviKvsTreeNodeExpression * m_pData; // can't be null!
	KviKvsNumber m_nData;               // result of the number evaluation
public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	bool evaluateOperand(KviKvsRunTimeContext * c);
};

class KVIRC_API KviKvsTreeNodeExpressionUnaryOperatorNegate : public KviKvsTreeNodeExpressionUnaryOperator
{
public:
	KviKvsTreeNodeExpressionUnaryOperatorNegate(const QChar * pLocation, KviKvsTreeNodeExpression * pData);
	~KviKvsTreeNodeExpressionUnaryOperatorNegate();

public:
	virtual int precedence();
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pResult);
};

class KVIRC_API KviKvsTreeNodeExpressionUnaryOperatorBitwiseNot : public KviKvsTreeNodeExpressionUnaryOperator
{
public:
	KviKvsTreeNodeExpressionUnaryOperatorBitwiseNot(const QChar * pLocation, KviKvsTreeNodeExpression * pData);
	~KviKvsTreeNodeExpressionUnaryOperatorBitwiseNot();

public:
	virtual int precedence();
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pResult);
};

class KVIRC_API KviKvsTreeNodeExpressionUnaryOperatorLogicalNot : public KviKvsTreeNodeExpressionUnaryOperator
{
public:
	KviKvsTreeNodeExpressionUnaryOperatorLogicalNot(const QChar * pLocation, KviKvsTreeNodeExpression * pData);
	~KviKvsTreeNodeExpressionUnaryOperatorLogicalNot();

public:
	virtual int precedence();
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pResult);
};

class KVIRC_API KviKvsTreeNodeExpressionBinaryOperator : public KviKvsTreeNodeExpressionOperator
{
public:
	KviKvsTreeNodeExpressionBinaryOperator(const QChar * pLocation);
	~KviKvsTreeNodeExpressionBinaryOperator();

protected:
	KviKvsTreeNodeExpression * m_pLeft;  // can be zero only during parsing (and thus when deleting)
	KviKvsTreeNodeExpression * m_pRight; // can be zero only during parsing (and thus when deleting)
	KviKvsNumber m_nLeft;                // buffer used to evaluate numeric operands
	KviKvsNumber m_nRight;               // buffer used to evaluate numeric operands
public:
	virtual KviKvsTreeNodeExpression * left();
	virtual KviKvsTreeNodeExpression * right();
	void setLeft(KviKvsTreeNodeExpression * pLeft);
	void setRight(KviKvsTreeNodeExpression * pRight);
	virtual int firstBinaryOperator();

public:
	void dumpOperands(const char * prefix);
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);

protected:
	bool evaluateOperands(KviKvsRunTimeContext * c);
};

#define DECLARE_BINARY_OPERATOR(__name)                                                   \
	class KVIRC_API __name : public KviKvsTreeNodeExpressionBinaryOperator                \
	{                                                                                     \
	public:                                                                               \
		__name(const QChar * pLocation);                                                  \
		~__name();                                                                        \
                                                                                          \
	public:                                                                               \
		virtual void contextDescription(QString & szBuffer);                              \
		virtual void dump(const char * prefix);                                           \
		virtual bool evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pResult); \
		virtual int precedence();                                                         \
	}

DECLARE_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorSum);
DECLARE_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorMultiplication);
DECLARE_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorSubtraction);
DECLARE_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorDivision);
DECLARE_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorModulus);
DECLARE_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorBitwiseAnd);
DECLARE_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorBitwiseOr);
DECLARE_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorBitwiseXor);
DECLARE_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorShiftLeft);
DECLARE_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorShiftRight);
DECLARE_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorAnd);
DECLARE_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorOr);
DECLARE_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorXor);
DECLARE_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorGreaterThan);
DECLARE_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorLowerThan);
DECLARE_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorGreaterOrEqualTo);
DECLARE_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorLowerOrEqualTo);
DECLARE_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorEqualTo);
DECLARE_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorNotEqualTo);

#endif //!_KVI_KVS_TREENODE_EXPRESSION_H_
