//=============================================================================
//
//   File : KviKvsTreeNodeModuleCallbackCommand.cpp
//   Creation date : Thu 09 Oct 2003 02:09:10 by Szymon Stefanek
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

#include "KviKvsTreeNodeModuleCallbackCommand.h"
#include "KviKvsTreeNodeDataList.h"
#include "KviKvsTreeNodeSwitchList.h"

#include "KviModuleManager.h"
#include "KviLocale.h"
#include "KviKvsModuleInterface.h"
#include "KviKvsRunTimeContext.h"

KviKvsTreeNodeModuleCallbackCommand::KviKvsTreeNodeModuleCallbackCommand(const QChar * pLocation, const QString & szModuleName, const QString & szCmdName, KviKvsTreeNodeDataList * params, KviKvsScript * pCallback)
    : KviKvsTreeNodeCallbackCommand(pLocation, szCmdName, params, pCallback)
{
	m_szModuleName = szModuleName;
}

KviKvsTreeNodeModuleCallbackCommand::~KviKvsTreeNodeModuleCallbackCommand()
    = default;

void KviKvsTreeNodeModuleCallbackCommand::contextDescription(QString & szBuffer)
{
	szBuffer = "Module Callback Command \"";
	szBuffer += m_szModuleName;
	szBuffer += ".";
	szBuffer += m_szCmdName;
	szBuffer += "\"";
}

void KviKvsTreeNodeModuleCallbackCommand::dump(const char * prefix)
{
	qDebug("%s ModuleCallbackCommand(%s.%s)", prefix, m_szModuleName.toUtf8().data(), m_szCmdName.toUtf8().data());
	dumpSwitchList(prefix);
	dumpParameterList(prefix);
	dumpCallback(prefix);
}

bool KviKvsTreeNodeModuleCallbackCommand::execute(KviKvsRunTimeContext * c)
{
	KviModule * m = g_pModuleManager->getModule(m_szModuleName);
	if(!m)
	{
		QString szErr = g_pModuleManager->lastError();
		c->error(this, __tr2qs_ctx("Module command call failed: can't load the module '%Q': %Q", "kvs"), &m_szModuleName, &szErr);
		return false;
	}

	KviKvsModuleCallbackCommandExecRoutine * proc = m->kvsFindCallbackCommand(m_szCmdName);
	if(!proc)
	{
		c->error(this, __tr2qs_ctx("Module command call failed: the module '%Q' doesn't export a callback command named '%Q'", "kvs"), &m_szModuleName, &m_szCmdName);
		return false;
	}

	KviKvsVariantList l;
	l.setAutoDelete(true);
	if(!(m_pParams->evaluate(c, &l)))
		return false;

	KviKvsSwitchList swl;
	if(m_pSwitches)
	{
		if(!(m_pSwitches->evaluate(c, &swl)))
			return false;
	}

	c->setDefaultReportLocation(this);

	KviKvsModuleCallbackCommandCall call(m, c, &l, &swl, m_pCallback, m_pParams);

	return (*proc)(&call);
}
