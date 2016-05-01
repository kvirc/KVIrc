//=============================================================================
//
//   File : KviKvsTreeNodeObjectFunctionCall.cpp
//   Creation date : Tue 07 Oct 2003 03:26:07 by Szymon Stefanek
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

#include "KviKvsTreeNodeObjectFunctionCall.h"

KviKvsTreeNodeObjectFunctionCall::KviKvsTreeNodeObjectFunctionCall(const QChar * pLocation, const QString & szFncName, KviKvsTreeNodeDataList * pParams)
    : KviKvsTreeNodeFunctionCall(pLocation, szFncName, pParams)
{
}

KviKvsTreeNodeObjectFunctionCall::~KviKvsTreeNodeObjectFunctionCall()
    = default;

void KviKvsTreeNodeObjectFunctionCall::contextDescription(QString & szBuffer)
{
	szBuffer = "Object Function Call \"";
	szBuffer += m_szFunctionName;
	szBuffer += "\"";
}

void KviKvsTreeNodeObjectFunctionCall::dump(const char * prefix)
{
	qDebug("%s ObjectFunctionCall(%s)", prefix, m_szFunctionName.toUtf8().data());
	QString tmp = prefix;
	tmp.append("  ");
	m_pParams->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeObjectFunctionCall::canEvaluateInObjectScope()
{
	return true;
}
