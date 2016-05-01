//=============================================================================
//
//   File : KviKvsTreeNodeCoreCallbackCommand.cpp
//   Creation date : Thu 09 Oct 2003 02:06:26 by Szymon Stefanek
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

#include "KviKvsTreeNodeCoreCallbackCommand.h"
#include "KviKvsTreeNodeDataList.h"
#include "KviKvsTreeNodeSwitchList.h"
#include "KviKvsScript.h"
#include "KviKvsRunTimeContext.h"

KviKvsTreeNodeCoreCallbackCommand::KviKvsTreeNodeCoreCallbackCommand(const QChar * pLocation, const QString & szCmdName, KviKvsTreeNodeDataList * params, KviKvsCoreCallbackCommandExecRoutine * r, KviKvsScript * pCallback)
    : KviKvsTreeNodeCallbackCommand(pLocation, szCmdName, params, pCallback)
{
	m_pExecRoutine = r;
}

KviKvsTreeNodeCoreCallbackCommand::~KviKvsTreeNodeCoreCallbackCommand()
    = default;

void KviKvsTreeNodeCoreCallbackCommand::contextDescription(QString & szBuffer)
{
	szBuffer = QString("Core Callback Command \"%1\"").arg(m_szCmdName);
}

void KviKvsTreeNodeCoreCallbackCommand::dump(const char * prefix)
{
	qDebug("%s CoreCallbackCommand(%s)", prefix, m_szCmdName.toUtf8().data());
	dumpSwitchList(prefix);
	dumpParameterList(prefix);
	dumpCallback(prefix);
}

bool KviKvsTreeNodeCoreCallbackCommand::execute(KviKvsRunTimeContext * c)
{
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

	return m_pExecRoutine->proc(c, &l, &swl, m_pCallback);
}
