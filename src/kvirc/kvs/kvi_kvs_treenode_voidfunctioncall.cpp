//=============================================================================
//
//   File : kvi_kvs_treenode_voidfunctioncall.cpp
//   Created on Thu 09 Oct 2003 01:47:16 by Szymon Stefanek
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

#include "kvi_kvs_treenode_voidfunctioncall.h"
#include "kvi_kvs_runtimecontext.h"

KviKvsTreeNodeVoidFunctionCall::KviKvsTreeNodeVoidFunctionCall(const QChar * pLocation,KviKvsTreeNodeFunctionCall * r)
: KviKvsTreeNodeInstruction(pLocation)
{
	m_pFunctionCall = r;
}

KviKvsTreeNodeVoidFunctionCall::~KviKvsTreeNodeVoidFunctionCall()
{
	delete m_pFunctionCall;
}

void KviKvsTreeNodeVoidFunctionCall::contextDescription(QString &szBuffer)
{
	szBuffer = "Void Function Call";
}

void KviKvsTreeNodeVoidFunctionCall::dump(const char * prefix)
{
	qDebug("%s VoidFunctionCall",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pFunctionCall->dump(tmp.utf8().data());
}

bool KviKvsTreeNodeVoidFunctionCall::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariant v;
	return m_pFunctionCall->evaluateReadOnly(c,&v);
}
