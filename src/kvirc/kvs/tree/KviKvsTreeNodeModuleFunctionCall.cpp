//=============================================================================
//
//   File : KviKvsTreeNodeModuleFunctionCall.cpp
//   Creation date : Tue 07 Oct 2003 03:19:28 by Szymon Stefanek
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

#include "KviKvsTreeNodeModuleFunctionCall.h"
#include "KviKvsTreeNodeDataList.h"

#include "KviModuleManager.h"
#include "KviLocale.h"
#include "KviKvsModuleInterface.h"
#include "KviKvsRunTimeContext.h"

KviKvsTreeNodeModuleFunctionCall::KviKvsTreeNodeModuleFunctionCall(const QChar * pLocation, const QString & szModuleName, const QString & szFncName, KviKvsTreeNodeDataList * pParams)
    : KviKvsTreeNodeFunctionCall(pLocation, szFncName, pParams)
{
	m_szModuleName = szModuleName;
}

KviKvsTreeNodeModuleFunctionCall::~KviKvsTreeNodeModuleFunctionCall()
    = default;

void KviKvsTreeNodeModuleFunctionCall::contextDescription(QString & szBuffer)
{
	szBuffer = "Module Function Call \"";
	szBuffer += m_szModuleName;
	szBuffer += ".";
	szBuffer += m_szFunctionName;
	szBuffer += "\"";
}

void KviKvsTreeNodeModuleFunctionCall::dump(const char * prefix)
{
	qDebug("%s ModuleFunctionCall(%s.%s)", prefix, m_szModuleName.toUtf8().data(), m_szFunctionName.toUtf8().data());
	QString tmp = prefix;
	tmp.append("  ");
	m_pParams->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeModuleFunctionCall::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	KviModule * m = g_pModuleManager->getModule(m_szModuleName);
	if(!m)
	{
		QString szErr = g_pModuleManager->lastError();
		c->error(this, __tr2qs_ctx("Module function call failed: can't load the module '%Q': %Q", "kvs"), &m_szModuleName, &szErr);
		return false;
	}

	KviKvsModuleFunctionExecRoutine * proc = m->kvsFindFunction(m_szFunctionName);
	if(!proc)
	{
		c->error(this, __tr2qs_ctx("Module function call failed: the module '%Q' doesn't export a function named '%Q'", "kvs"), &m_szModuleName, &m_szFunctionName);
		return false;
	}

	KviKvsVariantList l;
	if(!m_pParams->evaluate(c, &l))
		return false;

	pBuffer->setNothing();
	c->setDefaultReportLocation(this);
	KviKvsModuleFunctionCall call(m, c, &l, pBuffer);

	return (*proc)(&call);
}
