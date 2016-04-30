//=============================================================================
//
//   File : KviKvsTreeNodeAliasFunctionCall.cpp
//   Creation date : Tue 07 Oct 2003 03:21:23 by Szymon Stefanek
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

#include "KviKvsTreeNodeAliasFunctionCall.h"
#include "KviKvsVariantList.h"
#include "KviKvsAliasManager.h"
#include "KviLocale.h"

KviKvsTreeNodeAliasFunctionCall::KviKvsTreeNodeAliasFunctionCall(const QChar * pLocation, const QString & szAliasName, KviKvsTreeNodeDataList * pParams)
    : KviKvsTreeNodeFunctionCall(pLocation, szAliasName, pParams)
{
}

KviKvsTreeNodeAliasFunctionCall::~KviKvsTreeNodeAliasFunctionCall()
    = default;

void KviKvsTreeNodeAliasFunctionCall::contextDescription(QString & szBuffer)
{
	szBuffer = QString("Alias Function Call \"%1\"").arg(m_szFunctionName);
}

void KviKvsTreeNodeAliasFunctionCall::dump(const char * prefix)
{
	qDebug("%s AliasFunctionCall(%s)", prefix, m_szFunctionName.toUtf8().data());
	QString szTmp = prefix;
	szTmp.append("  ");
	m_pParams->dump(szTmp.toUtf8().data());
}

bool KviKvsTreeNodeAliasFunctionCall::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	KviKvsVariantList l;
	if(!m_pParams->evaluate(c, &l))
		return false;

	pBuffer->setNothing();

	const KviKvsScript * s = KviKvsAliasManager::instance()->lookup(m_szFunctionName);
	if(!s)
	{
		c->error(this, __tr2qs_ctx("Call to undefined function '%Q'", "kvs"), &m_szFunctionName);
		return false;
	}

	KviKvsScript copy(*s); // quick reference

	if(!copy.run(c->window(), &l, pBuffer, KviKvsScript::PreserveParams))
	{
		c->error(this, __tr2qs_ctx("Error in inner alias function call '%Q', called from this context", "kvs"), &m_szFunctionName);
		return false;
	}
	return true;
}
