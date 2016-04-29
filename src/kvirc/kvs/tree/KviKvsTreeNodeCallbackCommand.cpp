//=============================================================================
//
//   File : KviKvsTreeNodeCallbackCommand.cpp
//   Creation date : Thu 09 Oct 2003 02:02:42 by Szymon Stefanek
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

#include "KviKvsTreeNodeCallbackCommand.h"
#include "KviKvsTreeNodeDataList.h"
#include "KviKvsScript.h"

KviKvsTreeNodeCallbackCommand::KviKvsTreeNodeCallbackCommand(const QChar * pLocation, const QString & szCmdName, KviKvsTreeNodeDataList * params, KviKvsScript * pCallback)
    : KviKvsTreeNodeCommandWithParameters(pLocation, szCmdName, params)
{
	m_pCallback = pCallback;
}

KviKvsTreeNodeCallbackCommand::~KviKvsTreeNodeCallbackCommand()
{
	delete m_pCallback;
}

void KviKvsTreeNodeCallbackCommand::contextDescription(QString & szBuffer)
{
	szBuffer = QString("Callback Command \"%1\"").arg(m_szCmdName);
}

void KviKvsTreeNodeCallbackCommand::dump(const char * prefix)
{
	qDebug("%s CallbackCommand(%s)", prefix, m_szCmdName.toUtf8().data());
	dumpSwitchList(prefix);
	dumpParameterList(prefix);
	dumpCallback(prefix);
}

void KviKvsTreeNodeCallbackCommand::dumpCallback(const char * prefix)
{
	QString tmp = prefix;
	tmp.append("  ");
	m_pCallback->dump(tmp.toUtf8().data());
}
