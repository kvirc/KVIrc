//=============================================================================
//
//   File : KviKvsTreeNodeCommand.cpp
//   Creation date : Thu 09 Oct 2003 01:49:40 by Szymon Stefanek
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

#include "KviKvsTreeNodeCommand.h"
#include "KviKvsTreeNodeSwitchList.h"

KviKvsTreeNodeCommand::KviKvsTreeNodeCommand(const QChar * pLocation, const QString & szCmdName)
    : KviKvsTreeNodeInstruction(pLocation)
{
	m_szCmdName = szCmdName;
	m_pSwitches = nullptr;
}

KviKvsTreeNodeCommand::~KviKvsTreeNodeCommand()
{
	if(m_pSwitches)
		delete m_pSwitches;
}

//#warning "All the dump() functions could be killed (or moved to print on the kvirc windows)"

void KviKvsTreeNodeCommand::contextDescription(QString & szBuffer)
{
	szBuffer = QString("Command \"%1\"").arg(m_szCmdName);
}

void KviKvsTreeNodeCommand::dump(const char * prefix)
{
	qDebug("%s Command(%s)", prefix, m_szCmdName.toUtf8().data());
	dumpSwitchList(prefix);
}

void KviKvsTreeNodeCommand::dumpSwitchList(const char * prefix)
{
	if(!m_pSwitches)
		return;
	QString tmp = prefix;
	tmp.append("  ");
	m_pSwitches->dump(tmp.toUtf8().data());
}

void KviKvsTreeNodeCommand::setSwitchList(KviKvsTreeNodeSwitchList * sw)
{
	m_pSwitches = sw;
	m_pSwitches->setParent(this);
}
