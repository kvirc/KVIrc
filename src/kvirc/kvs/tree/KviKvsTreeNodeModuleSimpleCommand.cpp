//=============================================================================
//
//   File : KviKvsTreeNodeModuleSimpleCommand.cpp
//   Creation date : Thu 09 Oct 2003 02:18:27 by Szymon Stefanek
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

#include "KviKvsTreeNodeModuleSimpleCommand.h"
#include "KviKvsTreeNodeDataList.h"
#include "KviKvsTreeNodeSwitchList.h"

#include "KviModuleManager.h"
#include "KviLocale.h"
#include "KviKvsModuleInterface.h"
#include "KviKvsRunTimeContext.h"

KviKvsTreeNodeModuleSimpleCommand::KviKvsTreeNodeModuleSimpleCommand(const QChar * pLocation, const QString & szModuleName, const QString & szCmdName, KviKvsTreeNodeDataList * params)
    : KviKvsTreeNodeSimpleCommand(pLocation, szCmdName, params)
{
	m_szModuleName = szModuleName;
}

KviKvsTreeNodeModuleSimpleCommand::~KviKvsTreeNodeModuleSimpleCommand()
    = default;

void KviKvsTreeNodeModuleSimpleCommand::contextDescription(QString & szBuffer)
{
	szBuffer = "Module Callback Command \"";
	szBuffer += m_szModuleName;
	szBuffer += ".";
	szBuffer += m_szCmdName;
	szBuffer += "\"";
}

void KviKvsTreeNodeModuleSimpleCommand::dump(const char * prefix)
{
	qDebug("%s ModuleSimpleCommand(%s.%s)", prefix, m_szModuleName.toUtf8().data(), m_szCmdName.toUtf8().data());
	dumpSwitchList(prefix);
	dumpParameterList(prefix);
}

bool KviKvsTreeNodeModuleSimpleCommand::execute(KviKvsRunTimeContext * c)
{
	KviModule * m = g_pModuleManager->getModule(m_szModuleName);
	if(!m)
	{
		QString szErr = g_pModuleManager->lastError();
		c->error(this, __tr2qs_ctx("Module command call failed: can't load the module '%Q': %Q", "kvs"), &m_szModuleName, &szErr);
		return false;
	}

	KviKvsModuleSimpleCommandExecRoutine * proc = m->kvsFindSimpleCommand(m_szCmdName);
	if(!proc)
	{
		KviKvsModuleCallbackCommandExecRoutine * tmpProc = m->kvsFindCallbackCommand(m_szCmdName);
		if(tmpProc)
		{
			c->error(this, __tr2qs_ctx("Module command call failed, however the module '%Q' exports a callback command named '%Q' - possibly missing brackets in a callback command?", "kvs"), &m_szModuleName, &m_szCmdName);
		}
		else
		{
			c->error(this, __tr2qs_ctx("Module command call failed: the module '%Q' doesn't export a command named '%Q'", "kvs"), &m_szModuleName, &m_szCmdName);
		}
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

	KviKvsModuleCommandCall call(m, c, &l, &swl);

	return (*proc)(&call);
}
