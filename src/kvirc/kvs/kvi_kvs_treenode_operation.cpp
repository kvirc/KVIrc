//=============================================================================
//
//   File : kvi_kvs_treenode_operation.cpp
//   Created on Thu 18 Dec 2003 22:36:00 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003 Szymon Stefanek <pragma at kvirc dot net>
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#define __KVIRC__

#include "kvi_kvs_treenode_operation.h"
#include "kvi_kvs_treenode_data.h"
#include "kvi_kvs_runtimecontext.h"
#include "kvi_locale.h"

#include <qregexp.h>

#include <math.h>


KviKvsTreeNodeOperation::KviKvsTreeNodeOperation(const QChar * pLocation)
: KviKvsTreeNodeInstruction(pLocation)
{
	//m_pTargetData = 0; no need to set it
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

void KviKvsTreeNodeOperation::contextDescription(QString &szBuffer)
{
	szBuffer = "Operation";
}

void KviKvsTreeNodeOperation::dump(const char * prefix)
{
	debug("%s Operation",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.utf8().data());
}





KviKvsTreeNodeOperationAssignment::KviKvsTreeNodeOperationAssignment(const QChar * pLocation,KviKvsTreeNodeData * pRightSide)
: KviKvsTreeNodeOperation(pLocation)
{
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
}

KviKvsTreeNodeOperationAssignment::~KviKvsTreeNodeOperationAssignment()
{
	delete m_pRightSide;
}

void KviKvsTreeNodeOperationAssignment::contextDescription(QString &szBuffer)
{
	szBuffer = "Assignment";
}


void KviKvsTreeNodeOperationAssignment::dump(const char * prefix)
{
	debug("%s OperationAssignment",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.utf8().data());
	m_pRightSide->dump(tmp.utf8().data());
}

bool KviKvsTreeNodeOperationAssignment::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c,&v))return false;

	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)return false;

	target->result()->takeFrom(v);
	delete target;
	return true;
}






KviKvsTreeNodeOperationDecrement::KviKvsTreeNodeOperationDecrement(const QChar * pLocation)
: KviKvsTreeNodeOperation(pLocation)
{
}

KviKvsTreeNodeOperationDecrement::~KviKvsTreeNodeOperationDecrement()
{
}

void KviKvsTreeNodeOperationDecrement::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Operator \"--\"";
#endif
}


void KviKvsTreeNodeOperationDecrement::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s OperationDecrement",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeOperationDecrement::execute(KviKvsRunTimeContext * c)
{
#ifdef COMPILE_NEW_KVS
	KviKvsRWEvaluationResult * v = m_pTargetData->evaluateReadWrite(c);
	if(!v)return false;

	kvs_int_t iVal;
	if(v->result()->asInteger(iVal))
	{
		v->result()->setInteger(iVal - 1);
		delete v;
        v = 0;
		return true;
	}

	kvs_real_t dVal;
	if(v->result()->asReal(dVal))
	{
		v->result()->setReal(dVal - 1.0);
		delete v;
        v = 0;
		return true;
	}

	c->error(this,__tr2qs("The target variable didn't evaluate to an integer or real value"));
	delete v;
#endif
	return false;
}







KviKvsTreeNodeOperationIncrement::KviKvsTreeNodeOperationIncrement(const QChar * pLocation)
: KviKvsTreeNodeOperation(pLocation)
{
}

KviKvsTreeNodeOperationIncrement::~KviKvsTreeNodeOperationIncrement()
{
}

void KviKvsTreeNodeOperationIncrement::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Operator \"++\"";
#endif
}


void KviKvsTreeNodeOperationIncrement::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s OperationIncrement",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeOperationIncrement::execute(KviKvsRunTimeContext * c)
{
#ifdef COMPILE_NEW_KVS
	KviKvsRWEvaluationResult * v = m_pTargetData->evaluateReadWrite(c);
	if(!v)return false;

	kvs_int_t iVal;
	if(v->result()->asInteger(iVal))
	{
		v->result()->setInteger(iVal + 1);
		delete v;
        v = 0;
		return true;
	}

	kvs_real_t dVal;
	if(v->result()->asReal(dVal))
	{
		v->result()->setReal(dVal + 1.0);
		delete v;
        v = 0;
		return true;
	}
	c->error(this,__tr2qs("The target variable didn't evaluate to an integer or real value"));
	delete v;
#endif
	return false;
}




KviKvsTreeNodeOperationSelfAnd::KviKvsTreeNodeOperationSelfAnd(const QChar * pLocation,KviKvsTreeNodeData * pRightSide)
: KviKvsTreeNodeOperation(pLocation)
{
#ifdef COMPILE_NEW_KVS
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
#endif
}

KviKvsTreeNodeOperationSelfAnd::~KviKvsTreeNodeOperationSelfAnd()
{
#ifdef COMPILE_NEW_KVS
	delete m_pRightSide;
#endif
}

void KviKvsTreeNodeOperationSelfAnd::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Operator \"&=\"";
#endif
}


void KviKvsTreeNodeOperationSelfAnd::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s OperationSelfAnd",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.utf8().data());
	m_pRightSide->dump(tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeOperationSelfAnd::execute(KviKvsRunTimeContext * c)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c,&v))return false;

	kvs_int_t iRVal;

	if(!v.asInteger(iRVal))
	{
		c->error(this,__tr2qs("The right side of operator '&=' didn't evaluate to an integer"));
		return false;
	}

	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)return false;
	
	kvs_int_t iLVal;
	
	if(!target->result()->asInteger(iLVal))
	{
		c->error(this,__tr2qs("The left side of operator '&=' didn't evaluate to an integer"));
		delete target;
        target = 0;
		return false;
	}
	
	target->result()->setInteger(iLVal & iRVal);
	delete target;
#endif
	return true;
}



KviKvsTreeNodeOperationSelfDivision::KviKvsTreeNodeOperationSelfDivision(const QChar * pLocation,KviKvsTreeNodeData * pRightSide)
: KviKvsTreeNodeOperation(pLocation)
{
#ifdef COMPILE_NEW_KVS
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
#endif
}

KviKvsTreeNodeOperationSelfDivision::~KviKvsTreeNodeOperationSelfDivision()
{
#ifdef COMPILE_NEW_KVS
	delete m_pRightSide;
#endif
}

void KviKvsTreeNodeOperationSelfDivision::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Operator \"/=\"";
#endif
}


void KviKvsTreeNodeOperationSelfDivision::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s OperationSelfDivision",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.utf8().data());
	m_pRightSide->dump(tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeOperationSelfDivision::execute(KviKvsRunTimeContext * c)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c,&v))return false;

	KviKvsNumber rnum;

	if(!v.asNumber(rnum))
	{
		c->error(this,__tr2qs("The right side of operator '/=' didn't evaluate to a number"));
		return false;
	}
	
	KviKvsNumber lnum;

	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)return false;
	
	if(!target->result()->asNumber(lnum))
	{
		c->error(this,__tr2qs("The left side of operator '/=' didn't evaluate to a number"));
		delete target;
        target = 0;
		return false;
	}

	if(rnum.isInteger())
	{
		if(rnum.integer() == 0)
		{
			c->error(this,__tr2qs("Division by zero"));
			delete target;
            target = 0;
			return false;
		}
		if(lnum.isInteger())
			target->result()->setInteger(lnum.integer() / rnum.integer());
		else
			target->result()->setReal(lnum.real() / (kvs_real_t)(rnum.integer()));
	} else {
		if(rnum.real() == 0.0)
		{
			c->error(this,__tr2qs("Division by zero"));
			delete target;
            target = 0;
			return false;
		}
		if(lnum.isInteger())
			target->result()->setReal(((kvs_real_t)(lnum.integer())) / rnum.real());
		else
			target->result()->setReal(lnum.real() / rnum.real());
	}

	delete target;
#endif
	return true;
}

KviKvsTreeNodeOperationSelfModulus::KviKvsTreeNodeOperationSelfModulus(const QChar * pLocation,KviKvsTreeNodeData * pRightSide)
: KviKvsTreeNodeOperation(pLocation)
{
#ifdef COMPILE_NEW_KVS
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
#endif
}

KviKvsTreeNodeOperationSelfModulus::~KviKvsTreeNodeOperationSelfModulus()
{
#ifdef COMPILE_NEW_KVS
	delete m_pRightSide;
#endif
}

void KviKvsTreeNodeOperationSelfModulus::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Operator \"Self Modulo\"";
#endif
}


void KviKvsTreeNodeOperationSelfModulus::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s OperationSelfModulus",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.utf8().data());
	m_pRightSide->dump(tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeOperationSelfModulus::execute(KviKvsRunTimeContext * c)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c,&v))return false;
	KviKvsNumber rnum;

	if(!v.asNumber(rnum))
	{
		c->error(this,__tr2qs("The right side of operator '%=' didn't evaluate to a number"));
		return false;
	}
	
	KviKvsNumber lnum;

	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)return false;
	
	if(!target->result()->asNumber(lnum))
	{
		c->error(this,__tr2qs("The left side of operator '%=' didn't evaluate to a number"));
		delete target;
        target = 0;
		return false;
	}

	if(rnum.isInteger())
	{
		if(rnum.integer() == 0)
		{
			c->error(this,__tr2qs("Division by zero"));
			delete target;
            target = 0;
			return false;
		}
		if(lnum.isInteger())
			target->result()->setInteger(lnum.integer() % rnum.integer());
		else
			target->result()->setReal(fmod(lnum.real(),(kvs_real_t)(rnum.integer())));
	} else {
		if(rnum.real() == 0.0)
		{
			c->error(this,__tr2qs("Division by zero"));
			delete target;
            target = 0;
			return false;
		}
		if(lnum.isInteger())
			target->result()->setReal(fmod(((kvs_real_t)(lnum.integer())),rnum.real()));
		else
			target->result()->setReal(fmod(lnum.real(),rnum.real()));
	}
#endif
	return true;
}





KviKvsTreeNodeOperationSelfMultiplication::KviKvsTreeNodeOperationSelfMultiplication(const QChar * pLocation,KviKvsTreeNodeData * pRightSide)
: KviKvsTreeNodeOperation(pLocation)
{
#ifdef COMPILE_NEW_KVS
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
#endif
}

KviKvsTreeNodeOperationSelfMultiplication::~KviKvsTreeNodeOperationSelfMultiplication()
{
#ifdef COMPILE_NEW_KVS
	delete m_pRightSide;
#endif
}

void KviKvsTreeNodeOperationSelfMultiplication::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Operator \"*=\"";
#endif
}


void KviKvsTreeNodeOperationSelfMultiplication::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s OperationSelfMultiplication",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.utf8().data());
	m_pRightSide->dump(tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeOperationSelfMultiplication::execute(KviKvsRunTimeContext * c)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c,&v))return false;
	KviKvsNumber rnum;

	if(!v.asNumber(rnum))
	{
		c->error(this,__tr2qs("The right side of operator '*=' didn't evaluate to a number"));
		return false;
	}
	
	KviKvsNumber lnum;

	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)return false;
	
	if(!target->result()->asNumber(lnum))
	{
		c->error(this,__tr2qs("The left side of operator '*=' didn't evaluate to a number"));
		delete target;
		return false;
	}

	if(rnum.isInteger())
	{
		if(lnum.isInteger())
			target->result()->setInteger(lnum.integer() * rnum.integer());
		else
			target->result()->setReal(lnum.real() * (kvs_real_t)(rnum.integer()));
	} else {
		if(lnum.isInteger())
			target->result()->setReal(((kvs_real_t)(lnum.integer())) * rnum.real());
		else
			target->result()->setReal(lnum.real() * rnum.real());
	}
#endif
	return true;
}




KviKvsTreeNodeOperationSelfOr::KviKvsTreeNodeOperationSelfOr(const QChar * pLocation,KviKvsTreeNodeData * pRightSide)
: KviKvsTreeNodeOperation(pLocation)
{
#ifdef COMPILE_NEW_KVS
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
#endif
}

KviKvsTreeNodeOperationSelfOr::~KviKvsTreeNodeOperationSelfOr()
{
#ifdef COMPILE_NEW_KVS
	delete m_pRightSide;
#endif
}

void KviKvsTreeNodeOperationSelfOr::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Operator \"|=\"";
#endif
}


void KviKvsTreeNodeOperationSelfOr::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s OperationSelfOr",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.utf8().data());
	m_pRightSide->dump(tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeOperationSelfOr::execute(KviKvsRunTimeContext * c)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c,&v))return false;

	kvs_int_t iRVal;
	if(!v.asInteger(iRVal))
	{
		c->error(this,__tr2qs("The right side of operator '|=' didn't evaluate to an integer"));
		return false;
	}
	
	kvs_int_t iLVal;

	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)return false;
	
	if(!target->result()->asInteger(iLVal))
	{
		c->error(this,__tr2qs("The left side of operator '|=' didn't evaluate to an integer"));
		delete target;
        target = 0;
		return false;
	}
	
	target->result()->setInteger(iLVal | iRVal);
	delete target;
#endif
	return true;
}




KviKvsTreeNodeOperationSelfShl::KviKvsTreeNodeOperationSelfShl(const QChar * pLocation,KviKvsTreeNodeData * pRightSide)
: KviKvsTreeNodeOperation(pLocation)
{
#ifdef COMPILE_NEW_KVS
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
#endif
}

KviKvsTreeNodeOperationSelfShl::~KviKvsTreeNodeOperationSelfShl()
{
#ifdef COMPILE_NEW_KVS
	delete m_pRightSide;
#endif
}

void KviKvsTreeNodeOperationSelfShl::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Operator \"<<=\"";
#endif
}


void KviKvsTreeNodeOperationSelfShl::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s OperationSelfShl",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.utf8().data());
	m_pRightSide->dump(tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeOperationSelfShl::execute(KviKvsRunTimeContext * c)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c,&v))return false;

	kvs_int_t iRVal;

	if(!v.asInteger(iRVal))
	{
		c->error(this,__tr2qs("The right side of operator '<<=' didn't evaluate to an integer"));
		return false;
	}
	
	kvs_int_t iLVal;

	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)return false;
	
	if(!target->result()->asInteger(iLVal))
	{
		c->error(this,__tr2qs("The left side of operator '<<=' didn't evaluate to an integer"));
		delete target;
        target = 0;
		return false;
	}
	
	target->result()->setInteger(iLVal << iRVal);
	delete target;
#endif
	return true;
}





KviKvsTreeNodeOperationSelfShr::KviKvsTreeNodeOperationSelfShr(const QChar * pLocation,KviKvsTreeNodeData * pRightSide)
: KviKvsTreeNodeOperation(pLocation)
{
#ifdef COMPILE_NEW_KVS
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
#endif
}

KviKvsTreeNodeOperationSelfShr::~KviKvsTreeNodeOperationSelfShr()
{
#ifdef COMPILE_NEW_KVS
	delete m_pRightSide;
#endif
}

void KviKvsTreeNodeOperationSelfShr::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Operator \">>=\"";
#endif
}


void KviKvsTreeNodeOperationSelfShr::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s OperationSelfShr",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.utf8().data());
	m_pRightSide->dump(tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeOperationSelfShr::execute(KviKvsRunTimeContext * c)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c,&v))return false;

	kvs_int_t iRVal;

	if(!v.asInteger(iRVal))
	{
		c->error(this,__tr2qs("The right side of operator '>>=' didn't evaluate to an integer"));
		return false;
	}
	
	kvs_int_t iLVal;

	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)return false;
	
	if(!target->result()->asInteger(iLVal))
	{
		c->error(this,__tr2qs("The left side of operator '>>=' didn't evaluate to an integer"));
		delete target;
        target = 0;
		return false;
	}
	
	target->result()->setInteger(iLVal >> iRVal);
	delete target;
#endif
	return true;
}











KviKvsTreeNodeOperationSelfSubtraction::KviKvsTreeNodeOperationSelfSubtraction(const QChar * pLocation,KviKvsTreeNodeData * pRightSide)
: KviKvsTreeNodeOperation(pLocation)
{
#ifdef COMPILE_NEW_KVS
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
#endif
}

KviKvsTreeNodeOperationSelfSubtraction::~KviKvsTreeNodeOperationSelfSubtraction()
{
#ifdef COMPILE_NEW_KVS
	delete m_pRightSide;
#endif
}

void KviKvsTreeNodeOperationSelfSubtraction::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Operator \"-=\"";
#endif
}


void KviKvsTreeNodeOperationSelfSubtraction::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s OperationSelfSubtraction",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.utf8().data());
	m_pRightSide->dump(tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeOperationSelfSubtraction::execute(KviKvsRunTimeContext * c)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c,&v))return false;
	KviKvsNumber rnum;

	if(!v.asNumber(rnum))
	{
		c->error(this,__tr2qs("The right side of operator '-=' didn't evaluate to a number"));
		return false;
	}
	
	KviKvsNumber lnum;

	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)return false;
	
	if(!target->result()->asNumber(lnum))
	{
		c->error(this,__tr2qs("The left side of operator '-=' didn't evaluate to a number"));
		delete target;
        target = 0;
		return false;
	}

	if(rnum.isInteger())
	{
		if(lnum.isInteger())
			target->result()->setInteger(lnum.integer() - rnum.integer());
		else
			target->result()->setReal(lnum.real() - (kvs_real_t)(rnum.integer()));
	} else {
		if(lnum.isInteger())
			target->result()->setReal(((kvs_real_t)(lnum.integer())) - rnum.real());
		else
			target->result()->setReal(lnum.real() - rnum.real());
	}

	delete target;
#endif
	return true;
}





KviKvsTreeNodeOperationSelfSum::KviKvsTreeNodeOperationSelfSum(const QChar * pLocation,KviKvsTreeNodeData * pRightSide)
: KviKvsTreeNodeOperation(pLocation)
{
#ifdef COMPILE_NEW_KVS
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
#endif
}

KviKvsTreeNodeOperationSelfSum::~KviKvsTreeNodeOperationSelfSum()
{
#ifdef COMPILE_NEW_KVS
	delete m_pRightSide;
#endif
}

void KviKvsTreeNodeOperationSelfSum::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Operator \"+=\"";
#endif
}


void KviKvsTreeNodeOperationSelfSum::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s OperationSelfSum",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.utf8().data());
	m_pRightSide->dump(tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeOperationSelfSum::execute(KviKvsRunTimeContext * c)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c,&v))return false;
	KviKvsNumber rnum;

	if(!v.asNumber(rnum))
	{
		c->error(this,__tr2qs("The right side of operator '+=' didn't evaluate to a number"));
		return false;
	}
	
	KviKvsNumber lnum;

	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)return false;
	
	if(!target->result()->asNumber(lnum))
	{
		c->error(this,__tr2qs("The left side of operator '+=' didn't evaluate to a number"));
		delete target;
        target = 0;
		return false;
	}

	if(rnum.isInteger())
	{
		if(lnum.isInteger())
			target->result()->setInteger(lnum.integer() + rnum.integer());
		else
			target->result()->setReal(lnum.real() + (kvs_real_t)(rnum.integer()));
	} else {
		if(lnum.isInteger())
			target->result()->setReal(((kvs_real_t)(lnum.integer())) + rnum.real());
		else
			target->result()->setReal(lnum.real() + rnum.real());
	}

	delete target;
#endif
	return true;
}







KviKvsTreeNodeOperationSelfXor::KviKvsTreeNodeOperationSelfXor(const QChar * pLocation,KviKvsTreeNodeData * pRightSide)
: KviKvsTreeNodeOperation(pLocation)
{
#ifdef COMPILE_NEW_KVS
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
#endif
}

KviKvsTreeNodeOperationSelfXor::~KviKvsTreeNodeOperationSelfXor()
{
#ifdef COMPILE_NEW_KVS
	delete m_pRightSide;
#endif
}

void KviKvsTreeNodeOperationSelfXor::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Operator \"^=\"";
#endif
}


void KviKvsTreeNodeOperationSelfXor::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s OperationSelfXor",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.utf8().data());
	m_pRightSide->dump(tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeOperationSelfXor::execute(KviKvsRunTimeContext * c)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c,&v))return false;

	kvs_int_t iRVal;

	if(!v.asInteger(iRVal))
	{
		c->error(this,__tr2qs("The right side of operator '^=' didn't evaluate to an integer"));
		return false;
	}
	
	kvs_int_t iLVal;

	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)return false;
	
	if(!target->result()->asInteger(iLVal))
	{
		c->error(this,__tr2qs("The left side of operator '^=' didn't evaluate to an integer"));
		delete target;
        target = 0;
		return false;
	}
	
	target->result()->setInteger(iLVal ^ iRVal);
	delete target;
#endif
	return true;
}







KviKvsTreeNodeOperationStringAppend::KviKvsTreeNodeOperationStringAppend(const QChar * pLocation,KviKvsTreeNodeData * pRightSide)
: KviKvsTreeNodeOperation(pLocation)
{
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
}

KviKvsTreeNodeOperationStringAppend::~KviKvsTreeNodeOperationStringAppend()
{
	delete m_pRightSide;
}

void KviKvsTreeNodeOperationStringAppend::contextDescription(QString &szBuffer)
{
	szBuffer = "Operator \"<<\"";
}


void KviKvsTreeNodeOperationStringAppend::dump(const char * prefix)
{
	debug("%s OperationStringAppend",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.utf8().data());
	m_pRightSide->dump(tmp.utf8().data());
}

bool KviKvsTreeNodeOperationStringAppend::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c,&v))return false;
	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)return false;
	
	QString sz1;
	target->result()->asString(sz1);
	v.appendAsString(sz1);
	target->result()->setString(sz1);
	delete target;
	return true;
}






KviKvsTreeNodeOperationArrayAppend::KviKvsTreeNodeOperationArrayAppend(const QChar * pLocation,KviKvsTreeNodeData * pRightSide)
: KviKvsTreeNodeOperation(pLocation)
{
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
}

KviKvsTreeNodeOperationArrayAppend::~KviKvsTreeNodeOperationArrayAppend()
{
	delete m_pRightSide;
}

void KviKvsTreeNodeOperationArrayAppend::contextDescription(QString &szBuffer)
{
	szBuffer = "Operator \"<+\"";
}


void KviKvsTreeNodeOperationArrayAppend::dump(const char * prefix)
{
	debug("%s OperationArrayAppend",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.utf8().data());
	m_pRightSide->dump(tmp.utf8().data());
}

bool KviKvsTreeNodeOperationArrayAppend::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c,&v))return false;
	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)return false;

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
					a->set(a->size(),new KviKvsVariant(*pInternal));
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
				a->set(a->size(),new KviKvsVariant(*pInternal));
				++it;
			}
		}
		break;
		default:
			a->set(a->size(),new KviKvsVariant(v));
		break;
	}
	delete target;

	return true;
}







KviKvsTreeNodeOperationStringAppendWithComma::KviKvsTreeNodeOperationStringAppendWithComma(const QChar * pLocation,KviKvsTreeNodeData * pRightSide)
: KviKvsTreeNodeOperation(pLocation)
{
#ifdef COMPILE_NEW_KVS
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
#endif
}

KviKvsTreeNodeOperationStringAppendWithComma::~KviKvsTreeNodeOperationStringAppendWithComma()
{
#ifdef COMPILE_NEW_KVS
	delete m_pRightSide;
#endif
}

void KviKvsTreeNodeOperationStringAppendWithComma::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Operator \"<,\"";
#endif
}


void KviKvsTreeNodeOperationStringAppendWithComma::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s OperationStringAppendWithComma",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.utf8().data());
	m_pRightSide->dump(tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeOperationStringAppendWithComma::execute(KviKvsRunTimeContext * c)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c,&v))return false;
	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)return false;
	
	QString sz1;
	target->result()->asString(sz1);
	if(sz1.isEmpty())
	{
		v.asString(sz1);
	} else {
		QString sz2;
		v.asString(sz2);
		sz1 += QChar(',');
		sz1 += sz2;
	}
	target->result()->setString(sz1);
	delete target;
#endif
	return true;
}






KviKvsTreeNodeOperationStringAppendWithSpace::KviKvsTreeNodeOperationStringAppendWithSpace(const QChar * pLocation,KviKvsTreeNodeData * pRightSide)
: KviKvsTreeNodeOperation(pLocation)
{
#ifdef COMPILE_NEW_KVS
	m_pRightSide = pRightSide;
	m_pRightSide->setParent(this);
#endif
}

KviKvsTreeNodeOperationStringAppendWithSpace::~KviKvsTreeNodeOperationStringAppendWithSpace()
{
#ifdef COMPILE_NEW_KVS
	delete m_pRightSide;
#endif
}

void KviKvsTreeNodeOperationStringAppendWithSpace::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Operator \"<+\"";
#endif
}


void KviKvsTreeNodeOperationStringAppendWithSpace::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s OperationStringAppendWithSpace",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pTargetData->dump(tmp.utf8().data());
	m_pRightSide->dump(tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeOperationStringAppendWithSpace::execute(KviKvsRunTimeContext * c)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant v;
	if(!m_pRightSide->evaluateReadOnly(c,&v))return false;
	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)return false;
	
	QString sz1;
	target->result()->asString(sz1);
	if(sz1.isEmpty())
	{
		v.asString(sz1);
	} else {
		QString sz2;
		v.asString(sz2);
		sz1 += QChar(' ');
		sz1 += sz2;
	}
	target->result()->setString(sz1);
	delete target;
#endif
	return true;
}









KviKvsTreeNodeOperationStringTransliteration::KviKvsTreeNodeOperationStringTransliteration(const QChar * pLocation,KviKvsTreeNodeData * pLeft,KviKvsTreeNodeData * pRight,KviKvsTreeNodeData * pFlags)
: KviKvsTreeNodeOperation(pLocation)
{
#ifdef COMPILE_NEW_KVS
	m_pLeft = pLeft;
	m_pLeft->setParent(this);
	m_pRight = pRight;
	m_pRight->setParent(this);
	m_pFlags = pFlags;
	m_pFlags->setParent(this);
#endif
}

KviKvsTreeNodeOperationStringTransliteration::~KviKvsTreeNodeOperationStringTransliteration()
{
#ifdef COMPILE_NEW_KVS
	delete m_pLeft;
	delete m_pRight;
	delete m_pFlags;
#endif
}


void KviKvsTreeNodeOperationStringTransliteration::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Binding Operator tr///";
#endif
}


void KviKvsTreeNodeOperationStringTransliteration::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s OperationStringTransliteration",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pLeft->dump(tmp.utf8().data());
	m_pRight->dump(tmp.utf8().data());
	m_pFlags->dump(tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeOperationStringTransliteration::execute(KviKvsRunTimeContext * c)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant vl;
	if(!m_pLeft->evaluateReadOnly(c,&vl))return false;
	KviKvsVariant vr;
	if(!m_pRight->evaluateReadOnly(c,&vr))return false;

	// for now we don't use the flags at all here...
	//KviKvsVariant vf;
	//if(!m_pFlags->evaluateReadOnly(c,&vf))return false;
	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)return false;

	QString szL,szR,szF;

	vl.asString(szL);
	vr.asString(szR);
	//vf.asString(szF);

	QString sz1;
	target->result()->asString(sz1);
	KviQString::transliterate(sz1,szL,szR);
	target->result()->setString(sz1);
	delete target;
#endif
	return true;
}










KviKvsTreeNodeOperationStringSubstitution::KviKvsTreeNodeOperationStringSubstitution(const QChar * pLocation,KviKvsTreeNodeData * pLeft,KviKvsTreeNodeData * pRight,KviKvsTreeNodeData * pFlags)
: KviKvsTreeNodeOperation(pLocation)
{
#ifdef COMPILE_NEW_KVS
	m_pLeft = pLeft;
	m_pLeft->setParent(this);
	m_pRight = pRight;
	m_pRight->setParent(this);
	m_pFlags = pFlags;
	m_pFlags->setParent(this);
#endif
}

KviKvsTreeNodeOperationStringSubstitution::~KviKvsTreeNodeOperationStringSubstitution()
{
#ifdef COMPILE_NEW_KVS
	delete m_pLeft;
	delete m_pRight;
	delete m_pFlags;
#endif
}


void KviKvsTreeNodeOperationStringSubstitution::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Binding Operator s///";
#endif
}


void KviKvsTreeNodeOperationStringSubstitution::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s OperationStringSubstitution",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pLeft->dump(tmp.utf8().data());
	m_pRight->dump(tmp.utf8().data());
	m_pFlags->dump(tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeOperationStringSubstitution::execute(KviKvsRunTimeContext * c)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant vl;
	if(!m_pLeft->evaluateReadOnly(c,&vl))return false;
	KviKvsVariant vr;
	if(!m_pRight->evaluateReadOnly(c,&vr))return false;
	KviKvsVariant vf;
	if(!m_pFlags->evaluateReadOnly(c,&vf))return false;
	KviKvsRWEvaluationResult * target = m_pTargetData->evaluateReadWrite(c);
	if(!target)return false;

	QString szL,szR,szF;

	vl.asString(szL);
	vr.asString(szR);
	vf.asString(szF);

	QString str;
	target->result()->asString(str);

	bool bGlobal = szF.find('g',false) != -1;

	QRegExp re(szL,szF.find('i',false) == -1,szF.find('w',false) != -1);
	re.setMinimal(szF.find('m',false) != -1); // greedy or minimal ?

	int idx = 0;

	while((!str.isEmpty()) && (idx != -1))
	{
		int len;
		idx = re.search(str,idx);
		if(idx != -1)
		{
			len = re.matchedLength();
			if(len == 0)
			{
				c->warning(this,__tr2qs("The regular expression matched a zero-length substring: this is not valid in the s/// operator, skipping"));
				goto end_of_this_stuff;
			}
			str.remove(idx,len);

			QString szReplaced;
			const QChar * p2 = KviQString::nullTerminatedArray(szR);
			const QChar * begin = p2;
			while(p2->unicode())
			{
				if(p2->unicode() == '\\')
				{
					p2++;
					if(p2->unicode() >= '0' && p2->unicode() <= '9')
					{
						szReplaced.append(QString(begin,(p2 - begin) - 1));
						szReplaced.append(re.cap(p2->unicode() - '0'));
						p2++;
						begin = p2;
					} else if(p2->unicode())p2++;
				} else p2++;
			}
			if(begin != p2)szReplaced.append(QString(begin,p2 - begin));
			str.insert(idx,szReplaced);
			idx += szReplaced.length();

			if(!bGlobal)goto end_of_this_stuff;
		}
	}
end_of_this_stuff:
	target->result()->setString(str);
	delete target;
#endif
	return true;
}

