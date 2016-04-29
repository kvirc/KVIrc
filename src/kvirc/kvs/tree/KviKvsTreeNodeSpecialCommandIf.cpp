//=============================================================================
//
//   File : KviKvsTreeNodeSpecialCommandIf.cpp
//   Creation date : Thu 09 Oct 2003 02:40:00 by Szymon Stefanek
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

#include "KviKvsTreeNodeSpecialCommandIf.h"
#include "KviKvsTreeNodeExpression.h"
#include "KviKvsTreeNodeInstruction.h"
#include "KviKvsRunTimeContext.h"
#include "KviLocale.h"

KviKvsTreeNodeSpecialCommandIf::KviKvsTreeNodeSpecialCommandIf(const QChar * pLocation, KviKvsTreeNodeExpression * e, KviKvsTreeNodeInstruction * pIf, KviKvsTreeNodeInstruction * pElse)
    : KviKvsTreeNodeSpecialCommand(pLocation, "if")
{
	m_pExpression = e;
	m_pExpression->setParent(this);
	m_pIfInstruction = pIf;
	if(pIf)
		m_pIfInstruction->setParent(this);
	m_pElseInstruction = pElse;
	if(pElse)
		m_pElseInstruction->setParent(this);
}

KviKvsTreeNodeSpecialCommandIf::~KviKvsTreeNodeSpecialCommandIf()
{
	delete m_pExpression;
	if(m_pIfInstruction)
		delete m_pIfInstruction;
	if(m_pElseInstruction)
		delete m_pElseInstruction;
}

void KviKvsTreeNodeSpecialCommandIf::contextDescription(QString & szBuffer)
{
	szBuffer = "Special Command \"if\"";
}

void KviKvsTreeNodeSpecialCommandIf::dump(const char *)
{
}

bool KviKvsTreeNodeSpecialCommandIf::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariant v;
	if(!m_pExpression->evaluateReadOnly(c, &v))
		return false;

	if(v.asBoolean())
	{
		if(m_pIfInstruction)
		{
			return m_pIfInstruction->execute(c);
		}
	}
	else
	{
		if(m_pElseInstruction)
		{
			return m_pElseInstruction->execute(c);
		}
	}
	return true;
}
