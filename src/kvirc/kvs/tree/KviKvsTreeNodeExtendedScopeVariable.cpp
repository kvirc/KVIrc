//=============================================================================
//
//   File : KviKvsTreeNodeExtendedScopeVariable.cpp
//   Creation date : Thu 16 Oct 2003 22:41:20 by Szymon Stefanek
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

#include "KviKvsTreeNodeExtendedScopeVariable.h"
#include "KviKvsRunTimeContext.h"
#include "KviLocale.h"

KviKvsTreeNodeExtendedScopeVariable::KviKvsTreeNodeExtendedScopeVariable(const QChar * pLocation, const QString & szIdentifier)
    : KviKvsTreeNodeVariable(pLocation, szIdentifier)
{
}

KviKvsTreeNodeExtendedScopeVariable::~KviKvsTreeNodeExtendedScopeVariable()
    = default;

void KviKvsTreeNodeExtendedScopeVariable::contextDescription(QString & szBuffer)
{
	szBuffer = "Extended Scope Variable \"";
	szBuffer += m_szIdentifier;
	szBuffer += "\"";
}

void KviKvsTreeNodeExtendedScopeVariable::dump(const char * prefix)
{
	qDebug("%s ExtendedScopeVariable(%s)", prefix, m_szIdentifier.toUtf8().data());
}

bool KviKvsTreeNodeExtendedScopeVariable::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	if(!c->extendedScopeVariables())
	{
		c->error(this, __tr2qs_ctx("There are no extended scope variables in this scope", "kvs"));
		return false;
	}

	KviKvsVariant * v = c->extendedScopeVariables()->find(m_szIdentifier);
	if(v)
	{
		pBuffer->copyFrom(v);
	}
	else
	{
		pBuffer->setNothing();
	}
	return true;
}

KviKvsRWEvaluationResult * KviKvsTreeNodeExtendedScopeVariable::evaluateReadWrite(KviKvsRunTimeContext * c)
{
	if(!c->extendedScopeVariables())
	{
		c->error(this, __tr2qs_ctx("There are no extended scope variables in this scope", "kvs"));
		return nullptr;
	}

	return new KviKvsHashElement(nullptr, c->extendedScopeVariables()->get(m_szIdentifier), c->extendedScopeVariables(), m_szIdentifier);
}
