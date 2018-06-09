//=============================================================================
//
//   File : KviKvsTreeNodeThisObjectFunctionCall.cpp
//   Creation date : Tue 07 Oct 2003 03:28:10 by Szymon Stefanek
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

#include "KviKvsTreeNodeThisObjectFunctionCall.h"
#include "KviKvsObject.h"

KviKvsTreeNodeThisObjectFunctionCall::KviKvsTreeNodeThisObjectFunctionCall(const QChar * pLocation, const QString & szFncName, KviKvsTreeNodeDataList * pParams)
    : KviKvsTreeNodeObjectFunctionCall(pLocation, szFncName, pParams)
{
}

KviKvsTreeNodeThisObjectFunctionCall::~KviKvsTreeNodeThisObjectFunctionCall()
    = default;

void KviKvsTreeNodeThisObjectFunctionCall::contextDescription(QString & szBuffer)
{
	szBuffer = R"("This" Object Function Call ")";
	szBuffer += m_szFunctionName;
	szBuffer += "\"";
}

void KviKvsTreeNodeThisObjectFunctionCall::dump(const char * prefix)
{
	qDebug("%s ThisObjectFunctionCall(%s)", prefix, m_szFunctionName.toUtf8().data());
	QString tmp = prefix;
	tmp.append("  ");
	m_pParams->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeThisObjectFunctionCall::evaluateReadOnlyInObjectScope(KviKvsObject * o, KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	KviKvsVariantList l;
	if(!m_pParams->evaluate(c, &l))
		return false;
	pBuffer->setNothing();
	c->setDefaultReportLocation(this);
	return o->callFunction(c->thisObject(), m_szFunctionName, QString(), c, pBuffer, &l);
}
