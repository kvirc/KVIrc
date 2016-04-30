//=============================================================================
//
//   File : KviKvsTreeNodeArrayCount.cpp
//   Creation date : Tue 07 Oct 2003 03:00:52 by Szymon Stefanek
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

#include "KviKvsTreeNodeArrayCount.h"
#include "KviKvsVariant.h"
#include "KviKvsRunTimeContext.h"
#include "KviLocale.h"
#include "KviKvsObject.h"

KviKvsTreeNodeArrayCount::KviKvsTreeNodeArrayCount(const QChar * pLocation, KviKvsTreeNodeData * pSource)
    : KviKvsTreeNodeIndirectData(pLocation, pSource)
{
}

KviKvsTreeNodeArrayCount::~KviKvsTreeNodeArrayCount()
    = default;

void KviKvsTreeNodeArrayCount::contextDescription(QString & szBuffer)
{
	szBuffer = "Array Count Operator";
}

void KviKvsTreeNodeArrayCount::dump(const char * prefix)
{
	qDebug("%s ArrayCount", prefix);
}

bool KviKvsTreeNodeArrayCount::evaluateReadOnlyInObjectScope(KviKvsObject * o, KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
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
			c->warning(this, __tr2qs_ctx("The argument of the array count '#' operator didn't evaluate to an array: automatic conversion from type '%Q' supplied", "kvs"), &szType);
		}
		pBuffer->setInteger(0);
		return true;
	}

	pBuffer->setInteger(val.array()->size());
	return true;
}

bool KviKvsTreeNodeArrayCount::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	return evaluateReadOnlyInObjectScope(nullptr, c, pBuffer);
}
