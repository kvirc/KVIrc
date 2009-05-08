#ifndef _KVI_KVS_TREENODE_INSTRUCTION_H_
#define _KVI_KVS_TREENODE_INSTRUCTION_H_
//=============================================================================
//
//   File : kvi_kvs_treenode.h
//   Creation date : Mon 06 Oct 2003 03.46 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2003 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_qstring.h"
#include "kvi_kvs_treenode_base.h"

class KviKvsRunTimeContext;


class KVIRC_API KviKvsTreeNodeInstruction : public KviKvsTreeNode
{
public:
	KviKvsTreeNodeInstruction(const QChar * pLocation)
	: KviKvsTreeNode(pLocation){};
	~KviKvsTreeNodeInstruction(){};
public:
	virtual void contextDescription(QString &szBuffer);
	virtual void dump(const char * prefix);
	
	// execute() is the main entry point to the tree visit
	// execute() returns false when the tree visiting has
	// to be stopped and returns true when it has been
	// succesfully completed. A false return value may
	// signal an error but is also used by break, halt, return
	// and similar. The error condition is signaled by
	// KviKvsRunTimeContext::error().
	virtual bool execute(KviKvsRunTimeContext * c) = 0;
};


#endif //!_KVI_KVS_TREENODE_H_
