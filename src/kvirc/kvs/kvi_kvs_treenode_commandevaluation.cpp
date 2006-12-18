//=============================================================================
//
//   File : kvi_kvs_treenode_commandevaluation.cpp
//   Created on Wed 05 Nov 2003 04:18:58 by Szymon Stefanek
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#define __KVIRC__

#include "kvi_kvs_treenode_commandevaluation.h"

#include "kvi_kvs_runtimecontext.h"
#include "kvi_kvs_variant.h"

KviKvsTreeNodeCommandEvaluation::KviKvsTreeNodeCommandEvaluation(const QChar * pLocation,KviKvsTreeNodeInstruction * pInstruction)
: KviKvsTreeNodeData(pLocation)
{
#ifdef COMPILE_NEW_KVS
	m_pInstruction = pInstruction;
	m_pInstruction->setParent(this);
#endif
}

KviKvsTreeNodeCommandEvaluation::~KviKvsTreeNodeCommandEvaluation()
{
#ifdef COMPILE_NEW_KVS
	delete m_pInstruction;
#endif
}


bool KviKvsTreeNodeCommandEvaluation::evaluateReadOnly(KviKvsRunTimeContext * c,KviKvsVariant * pBuffer)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant * pTmp = c->swapReturnValuePointer(pBuffer);
	bool bRet = m_pInstruction->execute(c);
	c->swapReturnValuePointer(pTmp);
	return bRet;
#else
	return false;
#endif
}

