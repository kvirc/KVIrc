#ifndef _KVI_KVS_TREENODE_REBINDINGSWITCH_H_
#define _KVI_KVS_TREENODE_REBINDINGSWITCH_H_
//=============================================================================
//
//   File : kvi_kvs_treenode_rebindingswitch.h
//   Creation date : Mon 15 Aug 2005 13:32:31 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2005-2008 Szymon Stefanek <pragma at kvirc dot net>
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "kvi_settings.h"
#include "kvi_kvs_treenode_command.h"
#include "kvi_qstring.h"

class KviKvsTreeNodeData;
class KviKvsRunTimeContext;

class KVIRC_API KviKvsTreeNodeRebindingSwitch : public KviKvsTreeNodeCommand
{
public:
	KviKvsTreeNodeRebindingSwitch(const QChar * pLocation,KviKvsTreeNodeData * pTargetWindow,KviKvsTreeNodeCommand * pChildCommand);
	~KviKvsTreeNodeRebindingSwitch();
protected:
	KviKvsTreeNodeData * m_pTargetWindow;
	KviKvsTreeNodeCommand * m_pChildCommand;
public:
	virtual void contextDescription(QString &szBuffer);
	virtual void dump(const char * prefix);
	const QString & commandName();
	virtual bool execute(KviKvsRunTimeContext * c);
};

#endif //!_KVI_KVS_TREENODE_REBINDINGSWITCH_H_
