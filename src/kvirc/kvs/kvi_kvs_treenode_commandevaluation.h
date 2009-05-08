#ifndef _KVI_KVS_TREENODE_COMMANDEVALUATION_H_
#define _KVI_KVS_TREENODE_COMMANDEVALUATION_H_
//=============================================================================
//
//   File : kvi_kvs_treenode_commandevaluation.h
//   Creation date : Wed 05 Nov 2003 04:18:58 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2008 Szymon Stefanek <pragma at kvirc dot net>
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
#include "kvi_qstring.h"
#include "kvi_kvs_treenode_data.h"
#include "kvi_kvs_treenode_instruction.h"

class KviKvsRunTimeContext;
class KviKvsVariant;

class KviKvsTreeNodeCommandEvaluation : public KviKvsTreeNodeData
{
public:
	KviKvsTreeNodeCommandEvaluation(const QChar * pLocation,KviKvsTreeNodeInstruction * pInstruction);
	~KviKvsTreeNodeCommandEvaluation();
protected:
	KviKvsTreeNodeInstruction * m_pInstruction; // owned, never 0
public:
	virtual bool evaluateReadOnly(KviKvsRunTimeContext * c,KviKvsVariant * pBuffer);
};

#endif //!_KVI_KVS_TREENODE_COMMANDEVALUATION_H_
