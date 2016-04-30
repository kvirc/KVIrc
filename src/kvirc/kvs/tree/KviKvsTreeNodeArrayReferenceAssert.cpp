//=============================================================================
//
//   File : KviKvsTreeNodeArrayReferenceAssert.cpp
//   Creation date : Thu 16 Oct 2003 23:46:12 by Szymon Stefanek
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

#include "KviKvsTreeNodeArrayReferenceAssert.h"
#include "KviKvsRWEvaluationResult.h"
#include "KviKvsRunTimeContext.h"
#include "KviKvsVariant.h"
#include "KviLocale.h"
#include "KviKvsObject.h"

KviKvsTreeNodeArrayReferenceAssert::KviKvsTreeNodeArrayReferenceAssert(const QChar * pLocation, KviKvsTreeNodeData * pSource)
    : KviKvsTreeNodeIndirectData(pLocation, pSource)
{
}

KviKvsTreeNodeArrayReferenceAssert::~KviKvsTreeNodeArrayReferenceAssert()
    = default;

bool KviKvsTreeNodeArrayReferenceAssert::isReadOnly()
{
	return m_pSource->isReadOnly();
}

void KviKvsTreeNodeArrayReferenceAssert::contextDescription(QString & szBuffer)
{
	szBuffer = "Array Reference Assert";
}

void KviKvsTreeNodeArrayReferenceAssert::dump(const char * prefix)
{
	qDebug("%s ArrayReferenceAssert", prefix);
}

bool KviKvsTreeNodeArrayReferenceAssert::evaluateReadOnlyInObjectScope(KviKvsObject * o, KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	if(o)
	{
		if(!m_pSource->evaluateReadOnlyInObjectScope(o, c, pBuffer))
			return false;
	}
	else
	{
		if(!m_pSource->evaluateReadOnly(c, pBuffer))
			return false;
	}

	if(!pBuffer->isArray())
	{
		if(!pBuffer->isNothing())
		{
			QString szType;
			pBuffer->getTypeName(szType);
			c->error(this, __tr2qs_ctx("Array reference assert failed: the variable evaluated to type '%Q'", "kvs"), &szType);
			return false;
		}
	}
	return true;
}

KviKvsRWEvaluationResult * KviKvsTreeNodeArrayReferenceAssert::evaluateReadWriteInObjectScope(KviKvsObject * o, KviKvsRunTimeContext * c)
{
	KviKvsRWEvaluationResult * r;
	if(o)
		r = m_pSource->evaluateReadWriteInObjectScope(o, c);
	else
		r = m_pSource->evaluateReadWrite(c);

	if(!r)
		return nullptr;

	return r;
}

bool KviKvsTreeNodeArrayReferenceAssert::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	return evaluateReadOnlyInObjectScope(nullptr, c, pBuffer);
}

KviKvsRWEvaluationResult * KviKvsTreeNodeArrayReferenceAssert::evaluateReadWrite(KviKvsRunTimeContext * c)
{
	return evaluateReadWriteInObjectScope(nullptr, c);
}
