//=============================================================================
//
//   File : KviKvsTreeNodeSpecialCommandWhile.cpp
//   Creation date : Thu 06 Nov 2003 11:58:43 by Szymon Stefanek
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

#include "KviKvsTreeNodeSpecialCommandWhile.h"
#include "KviKvsTreeNodeExpression.h"
#include "KviKvsTreeNodeInstruction.h"
#include "KviKvsRunTimeContext.h"
#include "KviLocale.h"

KviKvsTreeNodeSpecialCommandWhile::KviKvsTreeNodeSpecialCommandWhile(const QChar * pLocation, KviKvsTreeNodeExpression * e, KviKvsTreeNodeInstruction * i)
    : KviKvsTreeNodeSpecialCommand(pLocation, "while")
{
	m_pExpression = e;
	m_pExpression->setParent(this);
	m_pInstruction = i;
	if(i)
		m_pInstruction->setParent(this);
}

KviKvsTreeNodeSpecialCommandWhile::~KviKvsTreeNodeSpecialCommandWhile()
{
	delete m_pExpression;
	if(m_pInstruction)
		delete m_pInstruction;
}

void KviKvsTreeNodeSpecialCommandWhile::contextDescription(QString & szBuffer)
{
	szBuffer = "Special Command \"while\"";
}

void KviKvsTreeNodeSpecialCommandWhile::dump(const char *)
{
}

bool KviKvsTreeNodeSpecialCommandWhile::execute(KviKvsRunTimeContext * c)
{
	for(;;)
	{
		KviKvsVariant v;
		if(!m_pExpression->evaluateReadOnly(c, &v))
			return false;

		if(!v.asBoolean())
			break;

		if(m_pInstruction)
		{
			if(!m_pInstruction->execute(c))
			{
				if(c->error())
					return false;

				if(c->breakPending())
				{
					c->handleBreak();
					return true;
				}

				if(c->continuePending())
				{
					c->handleContinue();
					continue;
				}
				return false;
			}
		}
	}
	return true;
}
