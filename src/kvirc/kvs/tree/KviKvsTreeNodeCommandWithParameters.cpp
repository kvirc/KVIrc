//=============================================================================
//
//   File : KviKvsTreeNodeCommandWithParameters.cpp
//   Creation date : Thu 09 Oct 2003 01:57:10 by Szymon Stefanek
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

#include "KviKvsTreeNodeCommandWithParameters.h"
#include "KviKvsTreeNodeDataList.h"

KviKvsTreeNodeCommandWithParameters::KviKvsTreeNodeCommandWithParameters(const QChar * pLocation, const QString & szCmdName, KviKvsTreeNodeDataList * params)
    : KviKvsTreeNodeCommand(pLocation, szCmdName)
{
	m_pParams = params;
	m_pParams->setParent(this);
}

KviKvsTreeNodeCommandWithParameters::~KviKvsTreeNodeCommandWithParameters()
{
	delete m_pParams;
}

void KviKvsTreeNodeCommandWithParameters::contextDescription(QString & szBuffer)
{
	szBuffer = QString("Command With Parameters \"%1\"").arg(m_szCmdName);
}

void KviKvsTreeNodeCommandWithParameters::dump(const char * prefix)
{
	qDebug("%s CommandWithParameters(%s)", prefix, m_szCmdName.toUtf8().data());
	dumpSwitchList(prefix);
	dumpParameterList(prefix);
}

void KviKvsTreeNodeCommandWithParameters::dumpParameterList(const char * prefix)
{
	QString tmp = prefix;
	tmp.append("  ");
	m_pParams->dump(tmp.toUtf8().data());
}
