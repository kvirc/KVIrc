//=============================================================================
//
//   File : KviKvsTreeNodeArrayElement.cpp
//   Creation date : Tue 07 Oct 2003 02:58:41 by Szymon Stefanek
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

#include "KviKvsTreeNodeArrayElement.h"
#include "KviKvsRunTimeContext.h"
#include "KviLocale.h"
#include "KviKvsArray.h"
#include "KviKvsObject.h"

KviKvsTreeNodeArrayElement::KviKvsTreeNodeArrayElement(const QChar * pLocation, KviKvsTreeNodeData * pSource, KviKvsTreeNodeExpression * pIndex)
    : KviKvsTreeNodeArrayOrHashElement(pLocation, pSource)
{
	m_pIndex = pIndex;
	m_pIndex->setParent(this);
}

KviKvsTreeNodeArrayElement::~KviKvsTreeNodeArrayElement()
{
	delete m_pIndex;
}

void KviKvsTreeNodeArrayElement::contextDescription(QString & szBuffer)
{
	szBuffer = "Array Element Evaluation";
}

void KviKvsTreeNodeArrayElement::dump(const char * prefix)
{
	qDebug("%s ArrayElement", prefix);
	QString szTmp = prefix;
	szTmp.append("  ");
	m_pSource->dump(szTmp.toUtf8().data());
	m_pIndex->dump(szTmp.toUtf8().data());
}

bool KviKvsTreeNodeArrayElement::evaluateIndex(KviKvsRunTimeContext * c, kvs_int_t & iVal)
{
	KviKvsVariant idx;
	if(!m_pIndex->evaluateReadOnly(c, &idx))
		return false;

	if(!idx.asInteger(iVal))
	{
		c->error(this, __tr2qs_ctx("Array index didn't evaluate to an integer", "kvs"));
		return false;
	}

	if(iVal < 0)
	{
		c->error(this, __tr2qs_ctx("Array index evaluated to a negative integer (non negative integer expected)", "kvs"));
		return false;
	}
	return true;
}

bool KviKvsTreeNodeArrayElement::evaluateReadOnlyInObjectScope(KviKvsObject * o, KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	kvs_int_t iVal;
	if(!evaluateIndex(c, iVal))
		return false;

	KviKvsVariant val;
	if(o)
	{
		if(!m_pSource->evaluateReadOnlyInObjectScope(o, c, &val))
			return false;
	}
	else
	{
		if(!m_pSource->evaluateReadOnly(c, &val))
			return false;
	}

	if(!val.isArray())
	{
		if(!val.isNothing())
		{
			QString szType;
			val.getTypeName(szType);
			c->warning(this, __tr2qs_ctx("The argument of the [] subscript didn't evaluate to an array: automatic conversion from %Q supplied", "kvs"), &szType);
		}
		pBuffer->setNothing();
		return true;
	}

	KviKvsVariant * v = val.array()->at(iVal);
	if(!v)
	{
		pBuffer->setNothing();
		return true;
	}

	pBuffer->copyFrom(v);
	return true;
}

KviKvsRWEvaluationResult * KviKvsTreeNodeArrayElement::evaluateReadWriteInObjectScope(KviKvsObject * o, KviKvsRunTimeContext * c)
{
	kvs_int_t iVal;
	if(!evaluateIndex(c, iVal))
		return nullptr;

	KviKvsRWEvaluationResult * result;
	if(o)
		result = m_pSource->evaluateReadWriteInObjectScope(o, c);
	else
		result = m_pSource->evaluateReadWrite(c);

	if(!result)
		return nullptr;

	if(!result->result()->isArray())
	{
		// convert to hash in some way
		if(!result->result()->isNothing())
		{
			QString szType;
			result->result()->getTypeName(szType);
			c->warning(this, __tr2qs_ctx("The argument of the [] subscript didn't evaluate to an array: automatic conversion from type '%Q' supplied", "kvs"), &szType);
		}
		result->result()->setArray(new KviKvsArray());
	}
	return new KviKvsArrayElement(result, result->result()->array()->getAt(iVal), result->result()->array(), iVal);
}

bool KviKvsTreeNodeArrayElement::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	return evaluateReadOnlyInObjectScope(nullptr, c, pBuffer);
}

KviKvsRWEvaluationResult * KviKvsTreeNodeArrayElement::evaluateReadWrite(KviKvsRunTimeContext * c)
{
	return evaluateReadWriteInObjectScope(nullptr, c);
}
