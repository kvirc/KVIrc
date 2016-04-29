#ifndef _KVI_KVS_TREENODE_INSTRUCTIONBLOCK_H_
#define _KVI_KVS_TREENODE_INSTRUCTIONBLOCK_H_
//=============================================================================
//
//   File : KviKvsTreeNodeInstructionBlock.h
//   Creation date : Thu 09 Oct 2003 01:44:31 by Szymon Stefanek
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

#include "kvi_settings.h"
#include "KviQString.h"
#include "KviPointerList.h"

#include "KviKvsTreeNodeInstruction.h"

class KviKvsRunTimeContext;

class KVIRC_API KviKvsTreeNodeInstructionBlock : public KviKvsTreeNodeInstruction
{
public:
	KviKvsTreeNodeInstructionBlock(const QChar * pLocation);
	~KviKvsTreeNodeInstructionBlock();

protected:
	KviPointerList<KviKvsTreeNodeInstruction> * m_pInstructionList;

public:
	unsigned int instructionCount() { return m_pInstructionList->count(); };
	void addInstruction(KviKvsTreeNodeInstruction * i);
	KviKvsTreeNodeInstruction * releaseFirst();
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);

	virtual bool execute(KviKvsRunTimeContext * c);
};

#endif //!_KVI_KVS_TREENODE_INSTRUCTIONBLOCK_H_
