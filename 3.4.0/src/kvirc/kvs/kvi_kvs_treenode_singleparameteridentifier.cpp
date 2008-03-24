//=============================================================================
//
//   File : kvi_kvs_treenode_singleparameteridentifier.cpp
//   Created on Tue 07 Oct 2003 03:47:11 by Szymon Stefanek
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

#include "kvi_kvs_treenode_singleparameteridentifier.h"
#include "kvi_kvs_runtimecontext.h"
#include "kvi_kvs_variant.h"

KviKvsTreeNodeSingleParameterIdentifier::KviKvsTreeNodeSingleParameterIdentifier(const QChar * pLocation,int iStart)
: KviKvsTreeNodeData(pLocation)
{
#ifdef COMPILE_NEW_KVS
	m_iStart = iStart;
#endif
}

KviKvsTreeNodeSingleParameterIdentifier::~KviKvsTreeNodeSingleParameterIdentifier()
{
}

void KviKvsTreeNodeSingleParameterIdentifier::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Single Parameter Identifier \"$";
	KviQString::appendFormatted(szBuffer,"%d",m_iStart);
	szBuffer += "\"";
#endif
}


void KviKvsTreeNodeSingleParameterIdentifier::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s SingleParameterIdentifier(%d)",prefix,m_iStart);
#endif
}

bool KviKvsTreeNodeSingleParameterIdentifier::canEvaluateToObjectReference()
{
	return true;
}

bool KviKvsTreeNodeSingleParameterIdentifier::evaluateReadOnly(KviKvsRunTimeContext * c,KviKvsVariant * pBuffer)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant * v = c->parameterList()->at(m_iStart);
	if(!v)pBuffer->setNothing();
	else pBuffer->copyFrom(v);
#endif
	return true;
}

