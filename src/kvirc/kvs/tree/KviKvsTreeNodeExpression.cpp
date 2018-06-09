//=============================================================================
//
//   File : KviKvsTreeNodeExpression.cpp
//   Creation date : Mon 06 Oct 2003 01.35 CEST by Szymon Stefanek
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

#include "KviKvsTreeNodeExpression.h"
#include "KviLocale.h"

#include <cmath>

KviKvsTreeNodeExpression::KviKvsTreeNodeExpression(const QChar * pLocation)
    : KviKvsTreeNodeData(pLocation)
{
	m_pParentExpression = nullptr;
}

KviKvsTreeNodeExpression::~KviKvsTreeNodeExpression()
    = default;

void KviKvsTreeNodeExpression::contextDescription(QString & szBuffer)
{
	szBuffer = "Expression Evaluation";
}

void KviKvsTreeNodeExpression::dump(const char * prefix)
{
	qDebug("%s Expression", prefix);
}

int KviKvsTreeNodeExpression::precedence()
{
	return PREC_MAXIMUM;
}

KviKvsTreeNodeExpression * KviKvsTreeNodeExpression::left()
{
	qDebug("KviKvsTreeNodeExpression::left() : should never end up here!");
	return nullptr;
}

KviKvsTreeNodeExpression * KviKvsTreeNodeExpression::right()
{
	qDebug("KviKvsTreeNodeExpression::right() : should never end up here!");
	return nullptr;
}

void KviKvsTreeNodeExpression::setLeft(KviKvsTreeNodeExpression *)
{
	qDebug("KviKvsTreeNodeExpression::setLeft() : should never end up here!");
}

void KviKvsTreeNodeExpression::setRight(KviKvsTreeNodeExpression *)
{
	qDebug("KviKvsTreeNodeExpression::setRight() : should never end up here!");
}

KviKvsTreeNodeExpression * KviKvsTreeNodeExpression::parentWithPrecedenceLowerThan(int iPrec)
{
	if(precedence() > iPrec)
		return this;
	if(!parentExpression())
		return nullptr;
	return parentExpression()->parentWithPrecedenceLowerThan(iPrec);
}

int KviKvsTreeNodeExpression::firstBinaryOperator()
{
	if(!left())
		return precedence();
	return left()->firstBinaryOperator();
}

KviKvsTreeNodeExpressionVariableOperand::KviKvsTreeNodeExpressionVariableOperand(const QChar * pLocation, KviKvsTreeNodeData * pData)
    : KviKvsTreeNodeExpression(pLocation)
{
	m_pData = pData;
	m_pData->setParent(this);
}

KviKvsTreeNodeExpressionVariableOperand::~KviKvsTreeNodeExpressionVariableOperand()
{
	delete m_pData;
}

void KviKvsTreeNodeExpressionVariableOperand::contextDescription(QString & szBuffer)
{
	szBuffer = "Expression Variable Operand Evaluation";
}

void KviKvsTreeNodeExpressionVariableOperand::dump(const char * prefix)
{
	qDebug("%s ExpressionVariableOperand", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pData->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeExpressionVariableOperand::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	return m_pData->evaluateReadOnly(c, pBuffer);
}

KviKvsTreeNodeExpressionConstantOperand::KviKvsTreeNodeExpressionConstantOperand(const QChar * pLocation, KviKvsVariant * pConstant)
    : KviKvsTreeNodeExpression(pLocation)
{
	m_pConstant = pConstant;
}

KviKvsTreeNodeExpressionConstantOperand::~KviKvsTreeNodeExpressionConstantOperand()
{
	delete m_pConstant;
}

void KviKvsTreeNodeExpressionConstantOperand::contextDescription(QString & szBuffer)
{
	szBuffer = "Expression Constant Operand Evaluation";
}

void KviKvsTreeNodeExpressionConstantOperand::dump(const char * prefix)
{
	qDebug("%s ExpressionConstantOperand", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pConstant->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeExpressionConstantOperand::evaluateReadOnly(KviKvsRunTimeContext *, KviKvsVariant * pBuffer)
{
	pBuffer->copyFrom(m_pConstant);
	return true;
}

KviKvsTreeNodeExpressionOperator::KviKvsTreeNodeExpressionOperator(const QChar * pLocation)
    : KviKvsTreeNodeExpression(pLocation)
{
}

KviKvsTreeNodeExpressionOperator::~KviKvsTreeNodeExpressionOperator()
    = default;

void KviKvsTreeNodeExpressionOperator::contextDescription(QString & szBuffer)
{
	szBuffer = "Expression Operator Evaluation";
}

void KviKvsTreeNodeExpressionOperator::dump(const char * prefix)
{
	qDebug("%s ExpressionOperator", prefix);
}

KviKvsTreeNodeExpressionUnaryOperator::KviKvsTreeNodeExpressionUnaryOperator(const QChar * pLocation, KviKvsTreeNodeExpression * pData)
    : KviKvsTreeNodeExpressionOperator(pLocation)
{
	m_pData = pData;
	m_pData->setParent(this);
	m_pData->setParentExpression(this);
}

KviKvsTreeNodeExpressionUnaryOperator::~KviKvsTreeNodeExpressionUnaryOperator()
{
	delete m_pData;
}

void KviKvsTreeNodeExpressionUnaryOperator::contextDescription(QString & szBuffer)
{
	szBuffer = "Expression Unary Operator Evaluation";
}

void KviKvsTreeNodeExpressionUnaryOperator::dump(const char * prefix)
{
	qDebug("%s ExpressionUnaryOperator", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pData->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeExpressionUnaryOperator::evaluateOperand(KviKvsRunTimeContext * c)
{
	KviKvsVariant v;
	if(!m_pData->evaluateReadOnly(c, &v))
		return false;

	if(!v.asNumber(m_nData))
	{
		c->error(this, __tr2qs_ctx("Operand of unary operator didn't evaluate to a number", "kvs"));
		return false;
	}
	return true;
}

KviKvsTreeNodeExpressionUnaryOperatorNegate::KviKvsTreeNodeExpressionUnaryOperatorNegate(const QChar * pLocation, KviKvsTreeNodeExpression * pData)
    : KviKvsTreeNodeExpressionUnaryOperator(pLocation, pData)
{
}

KviKvsTreeNodeExpressionUnaryOperatorNegate::~KviKvsTreeNodeExpressionUnaryOperatorNegate()
    = default;

void KviKvsTreeNodeExpressionUnaryOperatorNegate::contextDescription(QString & szBuffer)
{
	szBuffer = "Expression Unary Operator Negate";
}

void KviKvsTreeNodeExpressionUnaryOperatorNegate::dump(const char * prefix)
{
	qDebug("%s ExpressionUnaryOperatorNegate", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pData->dump(tmp.toUtf8().data());
}

int KviKvsTreeNodeExpressionUnaryOperatorNegate::precedence()
{
	return PREC_OP_NEGATE;
}

bool KviKvsTreeNodeExpressionUnaryOperatorNegate::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	if(!evaluateOperand(c))
		return false;
	if(m_nData.isReal())
		pBuffer->setReal(-m_nData.real());
	else
		pBuffer->setInteger(-m_nData.integer());
	return true;
}

KviKvsTreeNodeExpressionUnaryOperatorBitwiseNot::KviKvsTreeNodeExpressionUnaryOperatorBitwiseNot(const QChar * pLocation, KviKvsTreeNodeExpression * pData)
    : KviKvsTreeNodeExpressionUnaryOperator(pLocation, pData)
{
}

KviKvsTreeNodeExpressionUnaryOperatorBitwiseNot::~KviKvsTreeNodeExpressionUnaryOperatorBitwiseNot()
    = default;

void KviKvsTreeNodeExpressionUnaryOperatorBitwiseNot::contextDescription(QString & szBuffer)
{
	szBuffer = "Expression Unary Operator Bitwise Not";
}

void KviKvsTreeNodeExpressionUnaryOperatorBitwiseNot::dump(const char * prefix)
{
	qDebug("%s ExpressionUnaryOperatorBitwiseNot", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pData->dump(tmp.toUtf8().data());
}

int KviKvsTreeNodeExpressionUnaryOperatorBitwiseNot::precedence()
{
	return PREC_OP_BITWISENOT;
}

bool KviKvsTreeNodeExpressionUnaryOperatorBitwiseNot::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	if(!evaluateOperand(c))
		return false;
	if(m_nData.isReal())
		pBuffer->setInteger(~(int)(m_nData.real()));
	else
		pBuffer->setInteger(~m_nData.integer());
	return true;
}

KviKvsTreeNodeExpressionUnaryOperatorLogicalNot::KviKvsTreeNodeExpressionUnaryOperatorLogicalNot(const QChar * pLocation, KviKvsTreeNodeExpression * pData)
    : KviKvsTreeNodeExpressionUnaryOperator(pLocation, pData)
{
}

KviKvsTreeNodeExpressionUnaryOperatorLogicalNot::~KviKvsTreeNodeExpressionUnaryOperatorLogicalNot()
    = default;

void KviKvsTreeNodeExpressionUnaryOperatorLogicalNot::contextDescription(QString & szBuffer)
{
	szBuffer = "Expression Unary Operator Logical Not";
}

void KviKvsTreeNodeExpressionUnaryOperatorLogicalNot::dump(const char * prefix)
{
	qDebug("%s ExpressionUnaryOperatorLogicalNot", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pData->dump(tmp.toUtf8().data());
}

int KviKvsTreeNodeExpressionUnaryOperatorLogicalNot::precedence()
{
	return PREC_OP_LOGICALNOT;
}

bool KviKvsTreeNodeExpressionUnaryOperatorLogicalNot::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	KviKvsVariant v;
	if(!m_pData->evaluateReadOnly(c, &v))
		return false;
	//#warning "FIXME: We could use setNothing() for false and setInteger(1) for true: this would save memory allocations for false conditions"
	pBuffer->setBoolean(!v.asBoolean());
	return true;
}

KviKvsTreeNodeExpressionBinaryOperator::KviKvsTreeNodeExpressionBinaryOperator(const QChar * pLocation)
    : KviKvsTreeNodeExpressionOperator(pLocation)
{
	m_pLeft = nullptr;
	m_pRight = nullptr;
}

KviKvsTreeNodeExpressionBinaryOperator::~KviKvsTreeNodeExpressionBinaryOperator()
{
	if(m_pLeft)
		delete m_pLeft;
	if(m_pRight)
		delete m_pRight;
}

bool KviKvsTreeNodeExpressionBinaryOperator::evaluateOperands(KviKvsRunTimeContext * c)
{
	KviKvsVariant v1;
	if(!m_pLeft->evaluateReadOnly(c, &v1))
		return false;
	if(!v1.asNumber(m_nLeft))
	{
		c->error(this, __tr2qs_ctx("Left operand didn't evaluate to a number", "kvs"));
		return false;
	}
	KviKvsVariant v2;
	if(!m_pRight->evaluateReadOnly(c, &v2))
		return false;
	if(!v2.asNumber(m_nRight))
	{
		c->error(this, __tr2qs_ctx("Right operand didn't evaluate to a number", "kvs"));
		return false;
	}
	return true;
}

KviKvsTreeNodeExpression * KviKvsTreeNodeExpressionBinaryOperator::left()
{
	return m_pLeft;
}

KviKvsTreeNodeExpression * KviKvsTreeNodeExpressionBinaryOperator::right()
{
	return m_pRight;
}

int KviKvsTreeNodeExpressionBinaryOperator::firstBinaryOperator()
{
	return precedence();
}

void KviKvsTreeNodeExpressionBinaryOperator::setLeft(KviKvsTreeNodeExpression * pLeft)
{
	m_pLeft = pLeft;
	m_pLeft->setParent(this);
	m_pLeft->setParentExpression(this);
}

void KviKvsTreeNodeExpressionBinaryOperator::setRight(KviKvsTreeNodeExpression * pRight)
{
	m_pRight = pRight;
	m_pRight->setParent(this);
	m_pRight->setParentExpression(this);
}

void KviKvsTreeNodeExpressionBinaryOperator::dumpOperands(const char * prefix)
{
	QString tmp = prefix;
	tmp.append("  ");
	if(m_pLeft)
		m_pLeft->dump(tmp.toUtf8().data());
	if(m_pRight)
		m_pRight->dump(tmp.toUtf8().data());
}

void KviKvsTreeNodeExpressionBinaryOperator::contextDescription(QString & szBuffer)
{
	szBuffer = "Expression Binary Operator";
}

void KviKvsTreeNodeExpressionBinaryOperator::dump(const char * prefix)
{
	qDebug("%s ExpressionBinaryOperator", prefix);
	dumpOperands(prefix);
}

#define PREIMPLEMENT_BINARY_OPERATOR(__name, __stringname, __contextdescription, __precedence) \
	__name::__name(const QChar * pLocation)                                                    \
	    : KviKvsTreeNodeExpressionBinaryOperator(pLocation) {}                                 \
	__name::~__name() {}                                                                       \
	void __name::dump(const char * prefix)                                                     \
	{                                                                                          \
		qDebug("%s " __stringname, prefix);                                                    \
		dumpOperands(prefix);                                                                  \
	}                                                                                          \
	void __name::contextDescription(QString & szBuffer) { szBuffer = __contextdescription; }   \
	int __name::precedence() { return __precedence; };

PREIMPLEMENT_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorSum, "ExpressionBinaryOperatorSum", "Expression Binary Operator \"+\"", PREC_OP_SUM)

bool KviKvsTreeNodeExpressionBinaryOperatorSum::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	if(!evaluateOperands(c))
		return false;
	if(m_nLeft.isInteger())
	{
		if(m_nRight.isInteger())
			pBuffer->setInteger(m_nLeft.integer() + m_nRight.integer());
		else
			pBuffer->setReal(m_nLeft.integer() + m_nRight.real());
	}
	else
	{
		if(m_nRight.isInteger())
			pBuffer->setReal(m_nLeft.real() + m_nRight.integer());
		else
			pBuffer->setReal(m_nLeft.real() + m_nRight.real());
	}
	return true;
}

PREIMPLEMENT_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorSubtraction, "ExpressionBinaryOperatorSubtraction", "Expression Binary Operator \"-\"", PREC_OP_SUBTRACTION)

bool KviKvsTreeNodeExpressionBinaryOperatorSubtraction::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	if(!evaluateOperands(c))
		return false;
	if(m_nLeft.isInteger())
	{
		if(m_nRight.isInteger())
			pBuffer->setInteger(m_nLeft.integer() - m_nRight.integer());
		else
			pBuffer->setReal(((kvs_real_t)(m_nLeft.integer())) - m_nRight.real());
	}
	else
	{
		if(m_nRight.isInteger())
			pBuffer->setReal(m_nLeft.real() - ((kvs_real_t)(m_nRight.integer())));
		else
			pBuffer->setReal(m_nLeft.real() - m_nRight.real());
	}
	return true;
}

PREIMPLEMENT_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorMultiplication, "ExpressionBinaryOperatorMultiplication", "Expression Binary Operator \"*\"", PREC_OP_MULTIPLICATION)

bool KviKvsTreeNodeExpressionBinaryOperatorMultiplication::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	if(!evaluateOperands(c))
		return false;
	if(m_nLeft.isInteger())
	{
		if(m_nRight.isInteger())
			pBuffer->setInteger(m_nLeft.integer() * m_nRight.integer());
		else
			pBuffer->setReal(((kvs_real_t)(m_nLeft.integer())) * m_nRight.real());
	}
	else
	{
		if(m_nRight.isInteger())
			pBuffer->setReal(m_nLeft.real() * ((kvs_real_t)(m_nRight.integer())));
		else
			pBuffer->setReal(m_nLeft.real() * m_nRight.real());
	}
	return true;
}

PREIMPLEMENT_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorDivision, "ExpressionBinaryOperatorDivision", "Expression Binary Operator \"/\"", PREC_OP_DIVISION)

bool KviKvsTreeNodeExpressionBinaryOperatorDivision::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	if(!evaluateOperands(c))
		return false;

	if(m_nRight.isInteger())
	{
		if(m_nRight.integer() == 0)
		{
			c->error(this, __tr2qs_ctx("Division by zero", "kvs"));
			return false;
		}
		if(m_nLeft.isInteger())
			pBuffer->setInteger(m_nLeft.integer() / m_nRight.integer());
		else
			pBuffer->setReal(m_nLeft.real() / ((kvs_real_t)(m_nRight.integer())));
	}
	else
	{
		if(m_nRight.real() == 0.0)
		{
			c->error(this, __tr2qs_ctx("Division by zero", "kvs"));
			return false;
		}
		if(m_nLeft.isInteger())
			pBuffer->setReal(((kvs_real_t)(m_nLeft.integer())) / m_nRight.real());
		else
			pBuffer->setReal(m_nLeft.real() / m_nRight.real());
	}
	return true;
}

PREIMPLEMENT_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorModulus, "ExpressionBinaryOperatorModulus", "Expression Binary Operator \"modulus\"", PREC_OP_MODULUS)

bool KviKvsTreeNodeExpressionBinaryOperatorModulus::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	if(!evaluateOperands(c))
		return false;

	if(m_nRight.isInteger())
	{
		if(m_nRight.integer() == 0)
		{
			c->error(this, __tr2qs_ctx("Division by zero", "kvs"));
			return false;
		}
		if(m_nLeft.isInteger())
			pBuffer->setInteger(m_nLeft.integer() % m_nRight.integer());
		else
			pBuffer->setReal(fmod(m_nLeft.real(), ((kvs_real_t)(m_nRight.integer()))));
	}
	else
	{
		if(m_nRight.real() == 0.0)
		{
			c->error(this, __tr2qs_ctx("Division by zero", "kvs"));
			return false;
		}
		if(m_nLeft.isInteger())
			pBuffer->setReal(fmod(((kvs_real_t)(m_nLeft.integer())), m_nRight.real()));
		else
			pBuffer->setReal(fmod(m_nLeft.real(), m_nRight.real()));
	}
	return true;
}

PREIMPLEMENT_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorBitwiseAnd, "ExpressionBinaryOperatorBitwiseAnd", "Expression Binary Operator \"&\"", PREC_OP_BITWISEAND)

bool KviKvsTreeNodeExpressionBinaryOperatorBitwiseAnd::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	if(!evaluateOperands(c))
		return false;
	int iLeft = m_nLeft.isInteger() ? m_nLeft.integer() : (kvs_int_t)m_nLeft.real();
	int iRight = m_nRight.isInteger() ? m_nRight.integer() : (kvs_int_t)m_nRight.real();
	pBuffer->setInteger(iLeft & iRight);
	return true;
}

PREIMPLEMENT_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorBitwiseOr, "ExpressionBinaryOperatorBitwiseOr", "Expression Binary Operator \"|\"", PREC_OP_BITWISEOR)

bool KviKvsTreeNodeExpressionBinaryOperatorBitwiseOr::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	if(!evaluateOperands(c))
		return false;
	int iLeft = m_nLeft.isInteger() ? m_nLeft.integer() : (kvs_int_t)m_nLeft.real();
	int iRight = m_nRight.isInteger() ? m_nRight.integer() : (kvs_int_t)m_nRight.real();
	pBuffer->setInteger(iLeft | iRight);
	return true;
}

PREIMPLEMENT_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorBitwiseXor, "ExpressionBinaryOperatorBitwiseXor", "Expression Binary Operator \"^\"", PREC_OP_BITWISEXOR)

bool KviKvsTreeNodeExpressionBinaryOperatorBitwiseXor::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	if(!evaluateOperands(c))
		return false;
	int iLeft = m_nLeft.isInteger() ? m_nLeft.integer() : (kvs_int_t)m_nLeft.real();
	int iRight = m_nRight.isInteger() ? m_nRight.integer() : (kvs_int_t)m_nRight.real();
	pBuffer->setInteger(iLeft ^ iRight);
	return true;
}

PREIMPLEMENT_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorShiftLeft, "ExpressionBinaryOperatorShiftLeft", "Expression Binary Operator \"<<\"", PREC_OP_SHIFTLEFT)

bool KviKvsTreeNodeExpressionBinaryOperatorShiftLeft::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	if(!evaluateOperands(c))
		return false;
	int iLeft = m_nLeft.isInteger() ? m_nLeft.integer() : (kvs_int_t)(m_nLeft.real());
	int iRight = m_nRight.isInteger() ? m_nRight.integer() : (kvs_int_t)(m_nRight.real());
	pBuffer->setInteger(iLeft << iRight);
	return true;
}

PREIMPLEMENT_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorShiftRight, "ExpressionBinaryOperatorShiftRight", "Expression Binary Operator \">>\"", PREC_OP_SHIFTRIGHT)

bool KviKvsTreeNodeExpressionBinaryOperatorShiftRight::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	if(!evaluateOperands(c))
		return false;
	int iLeft = m_nLeft.isInteger() ? m_nLeft.integer() : (kvs_int_t)(m_nLeft.real());
	int iRight = m_nRight.isInteger() ? m_nRight.integer() : (kvs_int_t)(m_nRight.real());
	pBuffer->setInteger(iLeft >> iRight);
	return true;
}

PREIMPLEMENT_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorAnd, "ExpressionBinaryOperatorAnd", "Expression Binary Operator \"&&\"", PREC_OP_AND)

bool KviKvsTreeNodeExpressionBinaryOperatorAnd::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	KviKvsVariant v1;
	KviKvsVariant v2;
	if(!m_pLeft->evaluateReadOnly(c, &v1))
		return false;
	//#warning "FIXME: We could use setNothing() as false: this would save memory allocations (and thus time)"
	if(!v1.asBoolean())
	{
		pBuffer->setBoolean(false);
		return true;
	}
	if(!m_pRight->evaluateReadOnly(c, &v2))
		return false;
	pBuffer->setBoolean(v2.asBoolean());
	return true;
}

PREIMPLEMENT_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorOr, "ExpressionBinaryOperatorOr", "Expression Binary Operator \"||\"", PREC_OP_OR)

bool KviKvsTreeNodeExpressionBinaryOperatorOr::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	KviKvsVariant v1;
	KviKvsVariant v2;
	if(!m_pLeft->evaluateReadOnly(c, &v1))
		return false;
	//#warning "FIXME: We could use setNothing() as false: this would save memory allocations (and thus time)"
	if(v1.asBoolean())
	{
		pBuffer->setBoolean(true);
		return true;
	}
	if(!m_pRight->evaluateReadOnly(c, &v2))
		return false;
	pBuffer->setBoolean(v2.asBoolean());
	return true;
}

PREIMPLEMENT_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorXor, "ExpressionBinaryOperatorXor", "Expression Binary Operator \"^^\"", PREC_OP_XOR)

bool KviKvsTreeNodeExpressionBinaryOperatorXor::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	KviKvsVariant v1;
	KviKvsVariant v2;
	if(!m_pLeft->evaluateReadOnly(c, &v1))
		return false;
	if(!m_pRight->evaluateReadOnly(c, &v2))
		return false;
	//#warning "FIXME: We could use setNothing() as false: this would save memory allocations (and thus time)"
	if(v1.asBoolean())
		pBuffer->setBoolean(!v2.asBoolean());
	else
	{
		if(v2.asBoolean())
			pBuffer->setBoolean(!v1.asBoolean());
		else
			pBuffer->setBoolean(false);
	}
	return true;
}

PREIMPLEMENT_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorLowerThan, "ExpressionBinaryOperatorLowerThan", "Expression Binary Operator \"<\"", PREC_OP_LOWERTHAN)

bool KviKvsTreeNodeExpressionBinaryOperatorLowerThan::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	KviKvsVariant v1;
	KviKvsVariant v2;
	if(!m_pLeft->evaluateReadOnly(c, &v1))
		return false;
	if(!m_pRight->evaluateReadOnly(c, &v2))
		return false;
	pBuffer->setBoolean(v1.compare(&v2, true) > 0);
	return true;
}

PREIMPLEMENT_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorGreaterThan, "ExpressionBinaryOperatorGreaterThan", "Expression Binary Operator \">\"", PREC_OP_GREATERTHAN)

bool KviKvsTreeNodeExpressionBinaryOperatorGreaterThan::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	KviKvsVariant v1;
	KviKvsVariant v2;
	if(!m_pLeft->evaluateReadOnly(c, &v1))
		return false;
	if(!m_pRight->evaluateReadOnly(c, &v2))
		return false;
	pBuffer->setBoolean(v1.compare(&v2, true) < 0);
	return true;
}

PREIMPLEMENT_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorLowerOrEqualTo, "ExpressionBinaryOperatorLowerOrEqualTo", "Expression Binary Operator \"<=\"", PREC_OP_LOWEROREQUALTO)

bool KviKvsTreeNodeExpressionBinaryOperatorLowerOrEqualTo::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	KviKvsVariant v1;
	KviKvsVariant v2;
	if(!m_pLeft->evaluateReadOnly(c, &v1))
		return false;
	if(!m_pRight->evaluateReadOnly(c, &v2))
		return false;
	pBuffer->setBoolean(v1.compare(&v2, true) >= 0);
	return true;
}

PREIMPLEMENT_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorGreaterOrEqualTo, "ExpressionBinaryOperatorGreaterOrEqualTo", "Expression Binary Operator \">=\"", PREC_OP_GREATEROREQUALTO)

bool KviKvsTreeNodeExpressionBinaryOperatorGreaterOrEqualTo::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	KviKvsVariant v1;
	KviKvsVariant v2;
	if(!m_pLeft->evaluateReadOnly(c, &v1))
		return false;
	if(!m_pRight->evaluateReadOnly(c, &v2))
		return false;
	pBuffer->setBoolean(v1.compare(&v2, true) <= 0);
	return true;
}

PREIMPLEMENT_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorEqualTo, "ExpressionBinaryOperatorEqualTo", "Expression Binary Operator \"==\"", PREC_OP_EQUALTO)

bool KviKvsTreeNodeExpressionBinaryOperatorEqualTo::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	KviKvsVariant v1;
	KviKvsVariant v2;
	if(!m_pLeft->evaluateReadOnly(c, &v1))
		return false;
	if(!m_pRight->evaluateReadOnly(c, &v2))
		return false;
	pBuffer->setBoolean(v1.compare(&v2, true) == 0);
	return true;
}

PREIMPLEMENT_BINARY_OPERATOR(KviKvsTreeNodeExpressionBinaryOperatorNotEqualTo, "ExpressionBinaryOperatorNotEqualTo", "Expression Binary Operator \"!=\"", PREC_OP_NOTEQUALTO)

bool KviKvsTreeNodeExpressionBinaryOperatorNotEqualTo::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	KviKvsVariant v1;
	KviKvsVariant v2;
	if(!m_pLeft->evaluateReadOnly(c, &v1))
		return false;
	if(!m_pRight->evaluateReadOnly(c, &v2))
		return false;
	pBuffer->setBoolean(v1.compare(&v2, true) != 0);
	return true;
}
