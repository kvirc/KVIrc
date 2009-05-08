//=============================================================================
//
//   File : kvi_kvs_treenode_command.cpp
//   Created on Thu 09 Oct 2003 01:49:40 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003 Szymon Stefanek <pragma at kvirc dot net>
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#define __KVIRC__

#include "kvi_kvs_treenode_command.h"
#include "kvi_kvs_treenode_switchlist.h"

KviKvsTreeNodeCommand::KviKvsTreeNodeCommand(const QChar * pLocation,const QString &szCmdName)
: KviKvsTreeNodeInstruction(pLocation)
{
#ifdef COMPILE_NEW_KVS
	m_szCmdName = szCmdName;
	m_pSwitches = 0;
#endif
}

KviKvsTreeNodeCommand::~KviKvsTreeNodeCommand()
{
#ifdef COMPILE_NEW_KVS
	if(m_pSwitches)delete m_pSwitches;
#endif
}

//#warning "All the dump() functions could be killed (or moved to print on the kvirc windows)"

void KviKvsTreeNodeCommand::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	KviQString::sprintf(szBuffer,"Command \"%s\"",&m_szCmdName);
#endif
}

void KviKvsTreeNodeCommand::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s Command(%s)",prefix,m_szCmdName.utf8().data());
	dumpSwitchList(prefix);
#endif
}

void KviKvsTreeNodeCommand::dumpSwitchList(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	if(!m_pSwitches)return;
	QString tmp = prefix;
	tmp.append("  ");
	m_pSwitches->dump(tmp.utf8().data());
#endif
}

void KviKvsTreeNodeCommand::setSwitchList(KviKvsTreeNodeSwitchList * sw)
{
#ifdef COMPILE_NEW_KVS
	m_pSwitches = sw;
	m_pSwitches->setParent(this);
#endif
}

