//=============================================================================
//
//   File : KviKvsTreeNodeOperation.cpp
//   Creation date : Thu 18 Dec 2003 22:36:00 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviKvsTreeNodeOperation.h"
#include "KviKvsTreeNodeData.h"
#include "KviKvsRunTimeContext.h"
#include "KviLocale.h"

#include <QRegExp>

#include <cmath>

KviKvsTreeNodeOperation::KviKvsTreeNodeOperation(const QChar * pLocation)
    : KviKvsTreeNodeInstruction(pLocation)
{
}

KviKvsTreeNodeOperation::~KviKvsTreeNodeOperation()
{
	delete m_pTargetData;
}

void KviKvsTreeNodeOperation::setTargetVariableReference(KviKvsTreeNodeData * r)
{
	m_pTargetData = r;
	m_pTargetData->setParent(this);
}

void KviKvsTreeNodeOperation::contextDescription(QString & szBuffer)
{
	szBuffer = "Operation";
}

void KviKvsTreeNodeOperation::dump(const char * prefix)
{
	qDebug("%s Operation", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.toUtf8().data());
}

KviKvsTreeNodeOperationAssignment::KviKvsTreeNodeOperationAssignment(const QChar * pLocation, KviKvsTreeNodeData * pRightSide)
    : KviKvsTreeNodeOperation(pLocation)
{
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
}

KviKvsTreeNodeOperationAssignment::~KviKvsTreeNodeOperationAssignment()
{
	delete m_pRightSide;
}

void KviKvsTreeNodeOperationAssignment::contextDescription(QString & szBuffer)
{
	szBuffer = "Assignment";
}

void KviKvsTreeNodeOperationAssignment::dump(const char * prefix)
{
	qDebug("%s OperationAssignment", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.toUtf8().data());
	m_pRightSide->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeOperationAssignment::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c, &v))
		return false;

	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)
		return false;

	target->result()->takeFrom(v);
	delete target;
	return true;
}

KviKvsTreeNodeOperationDecrement::KviKvsTreeNodeOperationDecrement(const QChar * pLocation)
    : KviKvsTreeNodeOperation(pLocation)
{
}

KviKvsTreeNodeOperationDecrement::~KviKvsTreeNodeOperationDecrement()
    = default;

void KviKvsTreeNodeOperationDecrement::contextDescription(QString & szBuffer)
{
	szBuffer = "Operator \"--\"";
}

void KviKvsTreeNodeOperationDecrement::dump(const char * prefix)
{
	qDebug("%s OperationDecrement", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeOperationDecrement::execute(KviKvsRunTimeContext * c)
{
	KviKvsRWEvaluationResult * v = m_pTargetData->evaluateReadWrite(c);
	if(!v)
		return false;

	kvs_int_t iVal;
	if(v->result()->asInteger(iVal))
	{
		v->result()->setInteger(iVal - 1);
		delete v;
		v = nullptr;
		return true;
	}

	kvs_real_t dVal;
	if(v->result()->asReal(dVal))
	{
		v->result()->setReal(dVal - 1.0);
		delete v;
		v = nullptr;
		return true;
	}

	c->error(this, __tr2qs_ctx("The target variable didn't evaluate to an integer or real value", "kvs"));
	delete v;
	return false;
}

KviKvsTreeNodeOperationIncrement::KviKvsTreeNodeOperationIncrement(const QChar * pLocation)
    : KviKvsTreeNodeOperation(pLocation)
{
}

KviKvsTreeNodeOperationIncrement::~KviKvsTreeNodeOperationIncrement()
    = default;

void KviKvsTreeNodeOperationIncrement::contextDescription(QString & szBuffer)
{
	szBuffer = "Operator \"++\"";
}

void KviKvsTreeNodeOperationIncrement::dump(const char * prefix)
{
	qDebug("%s OperationIncrement", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeOperationIncrement::execute(KviKvsRunTimeContext * c)
{
	KviKvsRWEvaluationResult * v = m_pTargetData->evaluateReadWrite(c);
	if(!v)
		return false;

	kvs_int_t iVal;
	if(v->result()->asInteger(iVal))
	{
		v->result()->setInteger(iVal + 1);
		delete v;
		v = nullptr;
		return true;
	}

	kvs_real_t dVal;
	if(v->result()->asReal(dVal))
	{
		v->result()->setReal(dVal + 1.0);
		delete v;
		v = nullptr;
		return true;
	}
	c->error(this, __tr2qs_ctx("The target variable didn't evaluate to an integer or real value", "kvs"));
	delete v;
	return false;
}

KviKvsTreeNodeOperationSelfAnd::KviKvsTreeNodeOperationSelfAnd(const QChar * pLocation, KviKvsTreeNodeData * pRightSide)
    : KviKvsTreeNodeOperation(pLocation)
{
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
}

KviKvsTreeNodeOperationSelfAnd::~KviKvsTreeNodeOperationSelfAnd()
{
	delete m_pRightSide;
}

void KviKvsTreeNodeOperationSelfAnd::contextDescription(QString & szBuffer)
{
	szBuffer = "Operator \"&=\"";
}

void KviKvsTreeNodeOperationSelfAnd::dump(const char * prefix)
{
	qDebug("%s OperationSelfAnd", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.toUtf8().data());
	m_pRightSide->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeOperationSelfAnd::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c, &v))
		return false;

	kvs_int_t iRVal;

	if(!v.asInteger(iRVal))
	{
		c->error(this, __tr2qs_ctx("The right side of operator '&=' didn't evaluate to an integer", "kvs"));
		return false;
	}

	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)
		return false;

	kvs_int_t iLVal;

	if(!target->result()->asInteger(iLVal))
	{
		c->error(this, __tr2qs_ctx("The left side of operator '&=' didn't evaluate to an integer", "kvs"));
		delete target;
		target = nullptr;
		return false;
	}

	target->result()->setInteger(iLVal & iRVal);
	delete target;
	return true;
}

KviKvsTreeNodeOperationSelfDivision::KviKvsTreeNodeOperationSelfDivision(const QChar * pLocation, KviKvsTreeNodeData * pRightSide)
    : KviKvsTreeNodeOperation(pLocation)
{
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
}

KviKvsTreeNodeOperationSelfDivision::~KviKvsTreeNodeOperationSelfDivision()
{
	delete m_pRightSide;
}

void KviKvsTreeNodeOperationSelfDivision::contextDescription(QString & szBuffer)
{
	szBuffer = "Operator \"/=\"";
}

void KviKvsTreeNodeOperationSelfDivision::dump(const char * prefix)
{
	qDebug("%s OperationSelfDivision", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.toUtf8().data());
	m_pRightSide->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeOperationSelfDivision::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c, &v))
		return false;

	KviKvsNumber rnum;

	if(!v.asNumber(rnum))
	{
		c->error(this, __tr2qs_ctx("The right side of operator '/=' didn't evaluate to a number", "kvs"));
		return false;
	}

	KviKvsNumber lnum;

	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)
		return false;

	if(!target->result()->asNumber(lnum))
	{
		c->error(this, __tr2qs_ctx("The left side of operator '/=' didn't evaluate to a number", "kvs"));
		delete target;
		target = nullptr;
		return false;
	}

	if(rnum.isInteger())
	{
		if(rnum.integer() == 0)
		{
			c->error(this, __tr2qs_ctx("Division by zero", "kvs"));
			delete target;
			target = nullptr;
			return false;
		}
		if(lnum.isInteger())
			target->result()->setInteger(lnum.integer() / rnum.integer());
		else
			target->result()->setReal(lnum.real() / (kvs_real_t)(rnum.integer()));
	}
	else
	{
		if(rnum.real() == 0.0)
		{
			c->error(this, __tr2qs_ctx("Division by zero", "kvs"));
			delete target;
			target = nullptr;
			return false;
		}
		if(lnum.isInteger())
			target->result()->setReal(((kvs_real_t)(lnum.integer())) / rnum.real());
		else
			target->result()->setReal(lnum.real() / rnum.real());
	}

	delete target;
	return true;
}

KviKvsTreeNodeOperationSelfModulus::KviKvsTreeNodeOperationSelfModulus(const QChar * pLocation, KviKvsTreeNodeData * pRightSide)
    : KviKvsTreeNodeOperation(pLocation)
{
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
}

KviKvsTreeNodeOperationSelfModulus::~KviKvsTreeNodeOperationSelfModulus()
{
	delete m_pRightSide;
}

void KviKvsTreeNodeOperationSelfModulus::contextDescription(QString & szBuffer)
{
	szBuffer = "Operator \"Self Modulo\"";
}

void KviKvsTreeNodeOperationSelfModulus::dump(const char * prefix)
{
	qDebug("%s OperationSelfModulus", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.toUtf8().data());
	m_pRightSide->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeOperationSelfModulus::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c, &v))
		return false;
	KviKvsNumber rnum;

	if(!v.asNumber(rnum))
	{
		c->error(this, __tr2qs_ctx("The right side of operator '%=' didn't evaluate to a number", "kvs"));
		return false;
	}

	KviKvsNumber lnum;

	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)
		return false;

	if(!target->result()->asNumber(lnum))
	{
		c->error(this, __tr2qs_ctx("The left side of operator '%=' didn't evaluate to a number", "kvs"));
		delete target;
		target = nullptr;
		return false;
	}

	if(rnum.isInteger())
	{
		if(rnum.integer() == 0)
		{
			c->error(this, __tr2qs_ctx("Division by zero", "kvs"));
			delete target;
			target = nullptr;
			return false;
		}
		if(lnum.isInteger())
			target->result()->setInteger(lnum.integer() % rnum.integer());
		else
			target->result()->setReal(fmod(lnum.real(), (kvs_real_t)(rnum.integer())));
	}
	else
	{
		if(rnum.real() == 0.0)
		{
			c->error(this, __tr2qs_ctx("Division by zero", "kvs"));
			delete target;
			target = nullptr;
			return false;
		}
		if(lnum.isInteger())
			target->result()->setReal(fmod(((kvs_real_t)(lnum.integer())), rnum.real()));
		else
			target->result()->setReal(fmod(lnum.real(), rnum.real()));
	}
	return true;
}

KviKvsTreeNodeOperationSelfMultiplication::KviKvsTreeNodeOperationSelfMultiplication(const QChar * pLocation, KviKvsTreeNodeData * pRightSide)
    : KviKvsTreeNodeOperation(pLocation)
{
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
}

KviKvsTreeNodeOperationSelfMultiplication::~KviKvsTreeNodeOperationSelfMultiplication()
{
	delete m_pRightSide;
}

void KviKvsTreeNodeOperationSelfMultiplication::contextDescription(QString & szBuffer)
{
	szBuffer = "Operator \"*=\"";
}

void KviKvsTreeNodeOperationSelfMultiplication::dump(const char * prefix)
{
	qDebug("%s OperationSelfMultiplication", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.toUtf8().data());
	m_pRightSide->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeOperationSelfMultiplication::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c, &v))
		return false;
	KviKvsNumber rnum;

	if(!v.asNumber(rnum))
	{
		c->error(this, __tr2qs_ctx("The right side of operator '*=' didn't evaluate to a number", "kvs"));
		return false;
	}

	KviKvsNumber lnum;

	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)
		return false;

	if(!target->result()->asNumber(lnum))
	{
		c->error(this, __tr2qs_ctx("The left side of operator '*=' didn't evaluate to a number", "kvs"));
		delete target;
		return false;
	}

	if(rnum.isInteger())
	{
		if(lnum.isInteger())
			target->result()->setInteger(lnum.integer() * rnum.integer());
		else
			target->result()->setReal(lnum.real() * (kvs_real_t)(rnum.integer()));
	}
	else
	{
		if(lnum.isInteger())
			target->result()->setReal(((kvs_real_t)(lnum.integer())) * rnum.real());
		else
			target->result()->setReal(lnum.real() * rnum.real());
	}
	return true;
}

KviKvsTreeNodeOperationSelfOr::KviKvsTreeNodeOperationSelfOr(const QChar * pLocation, KviKvsTreeNodeData * pRightSide)
    : KviKvsTreeNodeOperation(pLocation)
{
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
}

KviKvsTreeNodeOperationSelfOr::~KviKvsTreeNodeOperationSelfOr()
{
	delete m_pRightSide;
}

void KviKvsTreeNodeOperationSelfOr::contextDescription(QString & szBuffer)
{
	szBuffer = "Operator \"|=\"";
}

void KviKvsTreeNodeOperationSelfOr::dump(const char * prefix)
{
	qDebug("%s OperationSelfOr", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.toUtf8().data());
	m_pRightSide->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeOperationSelfOr::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c, &v))
		return false;

	kvs_int_t iRVal;
	if(!v.asInteger(iRVal))
	{
		c->error(this, __tr2qs_ctx("The right side of operator '|=' didn't evaluate to an integer", "kvs"));
		return false;
	}

	kvs_int_t iLVal;

	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)
		return false;

	if(!target->result()->asInteger(iLVal))
	{
		c->error(this, __tr2qs_ctx("The left side of operator '|=' didn't evaluate to an integer", "kvs"));
		delete target;
		target = nullptr;
		return false;
	}

	target->result()->setInteger(iLVal | iRVal);
	delete target;
	return true;
}

KviKvsTreeNodeOperationSelfShl::KviKvsTreeNodeOperationSelfShl(const QChar * pLocation, KviKvsTreeNodeData * pRightSide)
    : KviKvsTreeNodeOperation(pLocation)
{
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
}

KviKvsTreeNodeOperationSelfShl::~KviKvsTreeNodeOperationSelfShl()
{
	delete m_pRightSide;
}

void KviKvsTreeNodeOperationSelfShl::contextDescription(QString & szBuffer)
{
	szBuffer = "Operator \"<<=\"";
}

void KviKvsTreeNodeOperationSelfShl::dump(const char * prefix)
{
	qDebug("%s OperationSelfShl", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.toUtf8().data());
	m_pRightSide->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeOperationSelfShl::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c, &v))
		return false;

	kvs_int_t iRVal;

	if(!v.asInteger(iRVal))
	{
		c->error(this, __tr2qs_ctx("The right side of operator '<<=' didn't evaluate to an integer", "kvs"));
		return false;
	}

	kvs_int_t iLVal;

	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)
		return false;

	if(!target->result()->asInteger(iLVal))
	{
		c->error(this, __tr2qs_ctx("The left side of operator '<<=' didn't evaluate to an integer", "kvs"));
		delete target;
		target = nullptr;
		return false;
	}

	target->result()->setInteger(iLVal << iRVal);
	delete target;
	return true;
}

KviKvsTreeNodeOperationSelfShr::KviKvsTreeNodeOperationSelfShr(const QChar * pLocation, KviKvsTreeNodeData * pRightSide)
    : KviKvsTreeNodeOperation(pLocation)
{
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
}

KviKvsTreeNodeOperationSelfShr::~KviKvsTreeNodeOperationSelfShr()
{
	delete m_pRightSide;
}

void KviKvsTreeNodeOperationSelfShr::contextDescription(QString & szBuffer)
{
	szBuffer = "Operator \">>=\"";
}

void KviKvsTreeNodeOperationSelfShr::dump(const char * prefix)
{
	qDebug("%s OperationSelfShr", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.toUtf8().data());
	m_pRightSide->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeOperationSelfShr::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c, &v))
		return false;

	kvs_int_t iRVal;

	if(!v.asInteger(iRVal))
	{
		c->error(this, __tr2qs_ctx("The right side of operator '>>=' didn't evaluate to an integer", "kvs"));
		return false;
	}

	kvs_int_t iLVal;

	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)
		return false;

	if(!target->result()->asInteger(iLVal))
	{
		c->error(this, __tr2qs_ctx("The left side of operator '>>=' didn't evaluate to an integer", "kvs"));
		delete target;
		target = nullptr;
		return false;
	}

	target->result()->setInteger(iLVal >> iRVal);
	delete target;
	return true;
}

KviKvsTreeNodeOperationSelfSubtraction::KviKvsTreeNodeOperationSelfSubtraction(const QChar * pLocation, KviKvsTreeNodeData * pRightSide)
    : KviKvsTreeNodeOperation(pLocation)
{
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
}

KviKvsTreeNodeOperationSelfSubtraction::~KviKvsTreeNodeOperationSelfSubtraction()
{
	delete m_pRightSide;
}

void KviKvsTreeNodeOperationSelfSubtraction::contextDescription(QString & szBuffer)
{
	szBuffer = "Operator \"-=\"";
}

void KviKvsTreeNodeOperationSelfSubtraction::dump(const char * prefix)
{
	qDebug("%s OperationSelfSubtraction", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.toUtf8().data());
	m_pRightSide->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeOperationSelfSubtraction::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c, &v))
		return false;
	KviKvsNumber rnum;

	if(!v.asNumber(rnum))
	{
		c->error(this, __tr2qs_ctx("The right side of operator '-=' didn't evaluate to a number", "kvs"));
		return false;
	}

	KviKvsNumber lnum;

	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)
		return false;

	if(!target->result()->asNumber(lnum))
	{
		c->error(this, __tr2qs_ctx("The left side of operator '-=' didn't evaluate to a number", "kvs"));
		delete target;
		target = nullptr;
		return false;
	}

	if(rnum.isInteger())
	{
		if(lnum.isInteger())
			target->result()->setInteger(lnum.integer() - rnum.integer());
		else
			target->result()->setReal(lnum.real() - (kvs_real_t)(rnum.integer()));
	}
	else
	{
		if(lnum.isInteger())
			target->result()->setReal(((kvs_real_t)(lnum.integer())) - rnum.real());
		else
			target->result()->setReal(lnum.real() - rnum.real());
	}

	delete target;
	return true;
}

KviKvsTreeNodeOperationSelfSum::KviKvsTreeNodeOperationSelfSum(const QChar * pLocation, KviKvsTreeNodeData * pRightSide)
    : KviKvsTreeNodeOperation(pLocation)
{
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
}

KviKvsTreeNodeOperationSelfSum::~KviKvsTreeNodeOperationSelfSum()
{
	delete m_pRightSide;
}

void KviKvsTreeNodeOperationSelfSum::contextDescription(QString & szBuffer)
{
	szBuffer = "Operator \"+=\"";
}

void KviKvsTreeNodeOperationSelfSum::dump(const char * prefix)
{
	qDebug("%s OperationSelfSum", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.toUtf8().data());
	m_pRightSide->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeOperationSelfSum::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c, &v))
		return false;

	KviKvsNumber rnum;

	if(!v.asNumber(rnum))
	{
		c->error(this, __tr2qs_ctx("The right side of operator '+=' didn't evaluate to a number", "kvs"));
		return false;
	}

	KviKvsNumber lnum;

	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)
		return false;

	if(!target->result()->asNumber(lnum))
	{
		c->error(this, __tr2qs_ctx("The left side of operator '+=' didn't evaluate to a number", "kvs"));
		delete target;
		target = nullptr;
		return false;
	}

	if(rnum.isInteger())
	{
		if(lnum.isInteger())
			target->result()->setInteger(lnum.integer() + rnum.integer());
		else
			target->result()->setReal(lnum.real() + (kvs_real_t)(rnum.integer()));
	}
	else
	{
		if(lnum.isInteger())
			target->result()->setReal(((kvs_real_t)(lnum.integer())) + rnum.real());
		else
			target->result()->setReal(lnum.real() + rnum.real());
	}

	delete target;
	return true;
}

KviKvsTreeNodeOperationSelfXor::KviKvsTreeNodeOperationSelfXor(const QChar * pLocation, KviKvsTreeNodeData * pRightSide)
    : KviKvsTreeNodeOperation(pLocation)
{
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
}

KviKvsTreeNodeOperationSelfXor::~KviKvsTreeNodeOperationSelfXor()
{
	delete m_pRightSide;
}

void KviKvsTreeNodeOperationSelfXor::contextDescription(QString & szBuffer)
{
	szBuffer = "Operator \"^=\"";
}

void KviKvsTreeNodeOperationSelfXor::dump(const char * prefix)
{
	qDebug("%s OperationSelfXor", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.toUtf8().data());
	m_pRightSide->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeOperationSelfXor::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c, &v))
		return false;

	kvs_int_t iRVal;

	if(!v.asInteger(iRVal))
	{
		c->error(this, __tr2qs_ctx("The right side of operator '^=' didn't evaluate to an integer", "kvs"));
		return false;
	}

	kvs_int_t iLVal;

	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)
		return false;

	if(!target->result()->asInteger(iLVal))
	{
		c->error(this, __tr2qs_ctx("The left side of operator '^=' didn't evaluate to an integer", "kvs"));
		delete target;
		target = nullptr;
		return false;
	}

	target->result()->setInteger(iLVal ^ iRVal);
	delete target;
	return true;
}

KviKvsTreeNodeOperationStringAppend::KviKvsTreeNodeOperationStringAppend(const QChar * pLocation, KviKvsTreeNodeData * pRightSide)
    : KviKvsTreeNodeOperation(pLocation)
{
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
}

KviKvsTreeNodeOperationStringAppend::~KviKvsTreeNodeOperationStringAppend()
{
	delete m_pRightSide;
}

void KviKvsTreeNodeOperationStringAppend::contextDescription(QString & szBuffer)
{
	szBuffer = "Operator \"<<\"";
}

void KviKvsTreeNodeOperationStringAppend::dump(const char * prefix)
{
	qDebug("%s OperationStringAppend", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.toUtf8().data());
	m_pRightSide->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeOperationStringAppend::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c, &v))
		return false;
	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)
		return false;

	QString sz1;
	target->result()->asString(sz1);
	v.appendAsString(sz1);
	target->result()->setString(sz1);
	delete target;
	return true;
}

KviKvsTreeNodeOperationArrayAppend::KviKvsTreeNodeOperationArrayAppend(const QChar * pLocation, KviKvsTreeNodeData * pRightSide)
    : KviKvsTreeNodeOperation(pLocation)
{
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
}

KviKvsTreeNodeOperationArrayAppend::~KviKvsTreeNodeOperationArrayAppend()
{
	delete m_pRightSide;
}

void KviKvsTreeNodeOperationArrayAppend::contextDescription(QString & szBuffer)
{
	szBuffer = "Operator \"<+\"";
}

void KviKvsTreeNodeOperationArrayAppend::dump(const char * prefix)
{
	qDebug("%s OperationArrayAppend", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.toUtf8().data());
	m_pRightSide->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeOperationArrayAppend::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c, &v))
		return false;
	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)
		return false;

	target->result()->convertToArray();
	KviKvsArray * a = target->result()->array();

	switch(v.type())
	{
		case KviKvsVariantData::Nothing:
			// do nothing
			break;
		case KviKvsVariantData::Array:
		{
			KviKvsArray * s = v.array();
			unsigned int uIdx = 0;
			unsigned int uSize = s->size();
			while(uIdx < uSize)
			{
				KviKvsVariant * pInternal = s->at(uIdx);
				if(pInternal)
					a->set(a->size(), new KviKvsVariant(*pInternal));
				// else
				//	leave an empty entry
				uIdx++;
			}
		}
		break;
		case KviKvsVariantData::Hash:
		{
			KviKvsHashIterator it(*(v.hash()->dict()));
			while(KviKvsVariant * pInternal = it.current())
			{
				a->set(a->size(), new KviKvsVariant(*pInternal));
				++it;
			}
		}
		break;
		default:
			a->set(a->size(), new KviKvsVariant(v));
			break;
	}
	delete target;

	return true;
}

KviKvsTreeNodeOperationStringAppendWithComma::KviKvsTreeNodeOperationStringAppendWithComma(const QChar * pLocation, KviKvsTreeNodeData * pRightSide)
    : KviKvsTreeNodeOperation(pLocation)
{
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
}

KviKvsTreeNodeOperationStringAppendWithComma::~KviKvsTreeNodeOperationStringAppendWithComma()
{
	delete m_pRightSide;
}

void KviKvsTreeNodeOperationStringAppendWithComma::contextDescription(QString & szBuffer)
{
	szBuffer = "Operator \"<,\"";
}

void KviKvsTreeNodeOperationStringAppendWithComma::dump(const char * prefix)
{
	qDebug("%s OperationStringAppendWithComma", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.toUtf8().data());
	m_pRightSide->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeOperationStringAppendWithComma::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c, &v))
		return false;
	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)
		return false;

	QString sz1;
	target->result()->asString(sz1);
	if(sz1.isEmpty())
	{
		v.asString(sz1);
	}
	else
	{
		QString sz2;
		v.asString(sz2);
		sz1 += QChar(',');
		sz1 += sz2;
	}
	target->result()->setString(sz1);
	delete target;
	return true;
}

KviKvsTreeNodeOperationStringAppendWithSpace::KviKvsTreeNodeOperationStringAppendWithSpace(const QChar * pLocation, KviKvsTreeNodeData * pRightSide)
    : KviKvsTreeNodeOperation(pLocation)
{
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
}

KviKvsTreeNodeOperationStringAppendWithSpace::~KviKvsTreeNodeOperationStringAppendWithSpace()
{
	delete m_pRightSide;
}

void KviKvsTreeNodeOperationStringAppendWithSpace::contextDescription(QString & szBuffer)
{
	szBuffer = "Operator \"<+\"";
}

void KviKvsTreeNodeOperationStringAppendWithSpace::dump(const char * prefix)
{
	qDebug("%s OperationStringAppendWithSpace", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.toUtf8().data());
	m_pRightSide->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeOperationStringAppendWithSpace::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c, &v))
		return false;
	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)
		return false;

	QString sz1;
	target->result()->asString(sz1);
	if(sz1.isEmpty())
	{
		v.asString(sz1);
	}
	else
	{
		QString sz2;
		v.asString(sz2);
		sz1 += QChar(' ');
		sz1 += sz2;
	}
	target->result()->setString(sz1);
	delete target;
	return true;
}

KviKvsTreeNodeOperationStringTransliteration::KviKvsTreeNodeOperationStringTransliteration(const QChar * pLocation, KviKvsTreeNodeData * pLeft, KviKvsTreeNodeData * pRight, KviKvsTreeNodeData * pFlags)
    : KviKvsTreeNodeOperation(pLocation)
{
	m_pLeft = pLeft;
	m_pLeft->setParent(this);
	m_pRight = pRight;
	m_pRight->setParent(this);
	m_pFlags = pFlags;
	m_pFlags->setParent(this);
}

KviKvsTreeNodeOperationStringTransliteration::~KviKvsTreeNodeOperationStringTransliteration()
{
	delete m_pLeft;
	delete m_pRight;
	delete m_pFlags;
}

void KviKvsTreeNodeOperationStringTransliteration::contextDescription(QString & szBuffer)
{
	szBuffer = "Binding Operator tr///";
}

void KviKvsTreeNodeOperationStringTransliteration::dump(const char * prefix)
{
	qDebug("%s OperationStringTransliteration", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pLeft->dump(tmp.toUtf8().data());
	m_pRight->dump(tmp.toUtf8().data());
	m_pFlags->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeOperationStringTransliteration::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariant vl;
	if(!m_pLeft->evaluateReadOnly(c, &vl))
		return false;
	KviKvsVariant vr;
	if(!m_pRight->evaluateReadOnly(c, &vr))
		return false;

	// for now we don't use the flags at all here...
	//KviKvsVariant vf;
	//if(!m_pFlags->evaluateReadOnly(c,&vf))return false;
	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)
		return false;

	QString szL, szR, szF;

	vl.asString(szL);
	vr.asString(szR);
	//vf.asString(szF);

	QString sz1;
	target->result()->asString(sz1);
	KviQString::transliterate(sz1, szL, szR);
	target->result()->setString(sz1);
	delete target;
	return true;
}

KviKvsTreeNodeOperationStringSubstitution::KviKvsTreeNodeOperationStringSubstitution(const QChar * pLocation, KviKvsTreeNodeData * pLeft, KviKvsTreeNodeData * pRight, KviKvsTreeNodeData * pFlags)
    : KviKvsTreeNodeOperation(pLocation)
{
	m_pLeft = pLeft;
	m_pLeft->setParent(this);
	m_pRight = pRight;
	m_pRight->setParent(this);
	m_pFlags = pFlags;
	m_pFlags->setParent(this);
}

KviKvsTreeNodeOperationStringSubstitution::~KviKvsTreeNodeOperationStringSubstitution()
{
	delete m_pLeft;
	delete m_pRight;
	delete m_pFlags;
}

void KviKvsTreeNodeOperationStringSubstitution::contextDescription(QString & szBuffer)
{
	szBuffer = "Binding Operator s///";
}

void KviKvsTreeNodeOperationStringSubstitution::dump(const char * prefix)
{
	qDebug("%s OperationStringSubstitution", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pLeft->dump(tmp.toUtf8().data());
	m_pRight->dump(tmp.toUtf8().data());
	m_pFlags->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeOperationStringSubstitution::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariant vl;
	if(!m_pLeft->evaluateReadOnly(c, &vl))
		return false;
	KviKvsVariant vr;
	if(!m_pRight->evaluateReadOnly(c, &vr))
		return false;
	KviKvsVariant vf;
	if(!m_pFlags->evaluateReadOnly(c, &vf))
		return false;
	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)
		return false;

	QString szL, szR, szF;

	vl.asString(szL);
	vr.asString(szR);
	vf.asString(szF);

	QString str;
	target->result()->asString(str);

	bool bGlobal = szF.indexOf('g', Qt::CaseInsensitive) != -1;

	//QRegExp re(szL,szF.indexOf('i',Qt::CaseInsensitive) == -1,szF.indexOf('w',Qt::CaseInsensitive) != -1);
	QRegExp re(szL, szF.indexOf('i', Qt::CaseInsensitive) == -1 ? Qt::CaseSensitive : Qt::CaseInsensitive, szF.indexOf('w', Qt::CaseInsensitive) != -1 ? QRegExp::Wildcard : QRegExp::RegExp);

	re.setMinimal(szF.indexOf('m', Qt::CaseInsensitive) != -1); // greedy or minimal ?

	int idx = 0;

	while((!str.isEmpty()) && (idx != -1))
	{
		idx = re.indexIn(str, idx);
		if(idx != -1)
		{
			int len = re.matchedLength();
			if(len == 0)
			{
				c->warning(this, __tr2qs_ctx("The regular expression matched a zero-length substring: this is not valid in the s/// operator, skipping", "kvs"));
				goto end_of_this_stuff;
			}
			str.remove(idx, len);

			QString szReplaced;
			const QChar * p2 = szR.constData();
			const QChar * begin = p2;
			if(p2) //see bug #228, second example
			{
				while(p2->unicode())
				{
					if(p2->unicode() == '\\')
					{
						p2++;
						if(p2->unicode() >= '0' && p2->unicode() <= '9')
						{
							szReplaced.append(QString(begin, (p2 - begin) - 1));
							szReplaced.append(re.cap(p2->unicode() - '0'));
							p2++;
							begin = p2;
						}
						else if(p2->unicode())
							p2++;
					}
					else
						p2++;
				}
				if(begin != p2)
					szReplaced.append(QString(begin, p2 - begin));
				str.insert(idx, szReplaced);
				idx += szReplaced.length();
			}
			if(!bGlobal)
				goto end_of_this_stuff;
		}
	}
end_of_this_stuff:
	target->result()->setString(str);
	delete target;
	return true;
}
