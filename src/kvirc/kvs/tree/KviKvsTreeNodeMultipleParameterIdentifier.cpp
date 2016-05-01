//=============================================================================
//
//   File : KviKvsTreeNodeMultipleParameterIdentifier.cpp
//   Creation date : Tue 07 Oct 2003 03:49:31 by Szymon Stefanek
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

#include "KviKvsTreeNodeMultipleParameterIdentifier.h"
#include "KviKvsRunTimeContext.h"
#include "KviKvsVariant.h"

KviKvsTreeNodeMultipleParameterIdentifier::KviKvsTreeNodeMultipleParameterIdentifier(const QChar * pLocation, int iStart, int iEnd)
    : KviKvsTreeNodeData(pLocation)
{
	m_iStart = iStart;
	m_iEnd = iEnd;
}

KviKvsTreeNodeMultipleParameterIdentifier::~KviKvsTreeNodeMultipleParameterIdentifier()
    = default;

void KviKvsTreeNodeMultipleParameterIdentifier::contextDescription(QString & szBuffer)
{
	szBuffer = "Multiple Parameter Identifier \"$";
	if(m_iEnd < m_iStart)
		KviQString::appendFormatted(szBuffer, "%d-", m_iStart);
	else
		KviQString::appendFormatted(szBuffer, "%d-%d", m_iStart, m_iEnd);
	szBuffer += "\"";
}

void KviKvsTreeNodeMultipleParameterIdentifier::dump(const char * prefix)
{
	if(m_iEnd < m_iStart)
		qDebug("%s MultipleParameterIdentifier(%d-)", prefix, m_iStart);
	else
		qDebug("%s MultipleParameterIdentifier(%d-%d)", prefix, m_iStart, m_iEnd);
}

bool KviKvsTreeNodeMultipleParameterIdentifier::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	KviKvsVariant * v = c->parameterList()->at(m_iStart);
	if(!v)
	{
		pBuffer->setNothing();
		return true;
	}

	QString sz;
	v->asString(sz);

	if(m_iEnd >= m_iStart)
	{
		// only up to m_iEnd
		int idx = m_iStart;
		for(v = c->parameterList()->next(); v && (idx < m_iEnd); v = c->parameterList()->next())
		{
			sz += QChar(' ');
			v->appendAsString(sz);
			idx++;
		}
	}
	else
	{
		// all up to the end of the list
		for(v = c->parameterList()->next(); v; v = c->parameterList()->next())
		{
			sz += QChar(' ');
			v->appendAsString(sz);
		}
	}
	pBuffer->setString(sz);
	return true;
}
