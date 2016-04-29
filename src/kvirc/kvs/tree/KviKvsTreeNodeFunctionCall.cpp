//=============================================================================
//
//   File : KviKvsTreeNodeFunctionCall.cpp
//   Creation date : Tue 07 Oct 2003 03:14:44 by Szymon Stefanek
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

#include "KviKvsTreeNodeFunctionCall.h"

KviKvsTreeNodeFunctionCall::KviKvsTreeNodeFunctionCall(const QChar * pLocation, const QString & szFunctionName, KviKvsTreeNodeDataList * pParams)
    : KviKvsTreeNodeData(pLocation)
{
	m_szFunctionName = szFunctionName;
	m_pParams = pParams;
	m_pParams->setParent(this);
}

KviKvsTreeNodeFunctionCall::~KviKvsTreeNodeFunctionCall()
{
	delete m_pParams;
}

void KviKvsTreeNodeFunctionCall::contextDescription(QString & szBuffer)
{
	szBuffer = "Function Call";
}

void KviKvsTreeNodeFunctionCall::dump(const char * prefix)
{
	qDebug("%s FunctionCall", prefix);
}

bool KviKvsTreeNodeFunctionCall::canEvaluateToObjectReference()
{
	return true;
}

bool KviKvsTreeNodeFunctionCall::isFunctionCall()
{
	return true;
}
