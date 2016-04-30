//=============================================================================
//
//   File : KviKvsTreeNodeSingleParameterIdentifier.cpp
//   Creation date : Tue 07 Oct 2003 03:47:11 by Szymon Stefanek
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

#include "KviKvsTreeNodeSingleParameterIdentifier.h"
#include "KviKvsRunTimeContext.h"
#include "KviKvsVariant.h"

KviKvsTreeNodeSingleParameterIdentifier::KviKvsTreeNodeSingleParameterIdentifier(const QChar * pLocation, int iStart)
    : KviKvsTreeNodeData(pLocation)
{
	m_iStart = iStart;
}

KviKvsTreeNodeSingleParameterIdentifier::~KviKvsTreeNodeSingleParameterIdentifier()
    = default;

void KviKvsTreeNodeSingleParameterIdentifier::contextDescription(QString & szBuffer)
{
	szBuffer = "Single Parameter Identifier \"$";
	KviQString::appendFormatted(szBuffer, "%d", m_iStart);
	szBuffer += "\"";
}

void KviKvsTreeNodeSingleParameterIdentifier::dump(const char * prefix)
{
	qDebug("%s SingleParameterIdentifier(%d)", prefix, m_iStart);
}

bool KviKvsTreeNodeSingleParameterIdentifier::canEvaluateToObjectReference()
{
	return true;
}

bool KviKvsTreeNodeSingleParameterIdentifier::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	KviKvsVariant * v = c->parameterList()->at(m_iStart);
	if(!v)
		pBuffer->setNothing();
	else
		pBuffer->copyFrom(v);
	return true;
}
