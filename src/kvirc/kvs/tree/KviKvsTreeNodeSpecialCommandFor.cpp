//=============================================================================
//
//   File : KviKvsTreeNodeSpecialCommandFor.cpp
//   Creation date : Fri 07 Nov 2003 11:48:11 by Szymon Stefanek
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

#include "KviKvsTreeNodeSpecialCommandFor.h"
#include "KviKvsTreeNodeExpression.h"
#include "KviKvsTreeNodeInstruction.h"
#include "KviKvsRunTimeContext.h"
#include "KviLocale.h"

KviKvsTreeNodeSpecialCommandFor::KviKvsTreeNodeSpecialCommandFor(const QChar * pLocation, KviKvsTreeNodeInstruction * pInit, KviKvsTreeNodeExpression * pCond, KviKvsTreeNodeInstruction * pUpd, KviKvsTreeNodeInstruction * pLoop)
    : KviKvsTreeNodeSpecialCommand(pLocation, "for")
{
	m_pInitialization = pInit;
	if(m_pInitialization)
		m_pInitialization->setParent(this);
	m_pCondition = pCond;
	if(m_pCondition)
		m_pCondition->setParent(this);
	m_pUpdate = pUpd;
	if(m_pUpdate)
		m_pUpdate->setParent(this);
	m_pLoop = pLoop;
	if(m_pLoop)
		m_pLoop->setParent(this);
}

KviKvsTreeNodeSpecialCommandFor::~KviKvsTreeNodeSpecialCommandFor()
{
	if(m_pInitialization)
		delete m_pInitialization;
	if(m_pCondition)
		delete m_pCondition;
	if(m_pUpdate)
		delete m_pUpdate;
	if(m_pLoop)
		delete m_pLoop;
}

void KviKvsTreeNodeSpecialCommandFor::contextDescription(QString & szBuffer)
{
	szBuffer = "Special Command \"for\"";
}

void KviKvsTreeNodeSpecialCommandFor::dump(const char * prefix)
{
	qDebug("%s SpecialCommandFor", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	if(m_pInitialization)
		m_pInitialization->dump(tmp.toUtf8().data());
	if(m_pCondition)
		m_pCondition->dump(tmp.toUtf8().data());
	if(m_pUpdate)
		m_pUpdate->dump(tmp.toUtf8().data());
	if(m_pLoop)
		m_pLoop->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeSpecialCommandFor::execute(KviKvsRunTimeContext * c)
{
	if(m_pInitialization)
	{
		if(!m_pInitialization->execute(c))
		{
			// break allowed also here
			if(c->error())
				return false;

			if(c->breakPending())
			{
				c->handleBreak();
				return true;
			}

			return false; // propagate false ret value
		}
	}

	for(;;)
	{
		if(m_pCondition)
		{
			KviKvsVariant v;
			if(!m_pCondition->evaluateReadOnly(c, &v))
				return false;
			if(!v.asBoolean())
				return true;
		}

		if(m_pLoop)
		{
			if(!m_pLoop->execute(c))
			{
				if(c->error())
					return false;

				if(c->breakPending())
				{
					c->handleBreak();
					return true;
				}

				if(c->continuePending())
					c->handleContinue();
				else
					return false; // propagate false ret value
			}
		}

		if(m_pUpdate)
		{
			if(!m_pUpdate->execute(c))
			{
				// break allowed also here
				if(c->error())
					return false;

				if(c->breakPending())
				{
					c->handleBreak();
					return true;
				}

				if(c->continuePending())
					c->handleContinue();

				return false; // propagate false ret value
			}
		}
	}
	// not reached
	return false;
}
