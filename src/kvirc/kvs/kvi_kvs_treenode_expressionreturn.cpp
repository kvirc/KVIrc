//=============================================================================
//
//   File : kvi_kvs_treenode_expressionreturn.cpp
//   Created on Fri 30 Jan 2004 01:27:09 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004 Szymon Stefanek <pragma at kvirc dot net>
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
#include "kvi_kvs_treenode_expressionreturn.h"
#include "kvi_kvs_treenode_expression.h"
#include "kvi_kvs_runtimecontext.h"
#include "kvi_locale.h"

KviKvsTreeNodeExpressionReturn::KviKvsTreeNodeExpressionReturn(const QChar * pLocation,KviKvsTreeNodeExpression * pExpression)
: KviKvsTreeNodeInstruction(pLocation)
{
#ifdef COMPILE_NEW_KVS
	m_pExpression = pExpression;
	m_pExpression->setParent(this);
#endif
}

KviKvsTreeNodeExpressionReturn::~KviKvsTreeNodeExpressionReturn()
{
#ifdef COMPILE_NEW_KVS
	delete m_pExpression;
#endif
}


void KviKvsTreeNodeExpressionReturn::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Expression Return Evaluation";
#endif
}

void KviKvsTreeNodeExpressionReturn::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s ExpressionReturn",prefix);
	QString tmp = prefix;
	tmp += "  ";
	m_pExpression->dump(tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeExpressionReturn::execute(KviKvsRunTimeContext * c)
{
	return m_pExpression->evaluateReadOnly(c,c->returnValue());
}
