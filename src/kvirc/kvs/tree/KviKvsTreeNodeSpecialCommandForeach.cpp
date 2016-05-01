//=============================================================================
//
//   File : KviKvsTreeNodeSpecialCommandForeach.cpp
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

#include "KviKvsTreeNodeSpecialCommandForeach.h"
#include "KviKvsTreeNodeData.h"
#include "KviKvsTreeNodeDataList.h"
#include "KviKvsTreeNodeInstruction.h"
#include "KviKvsTreeNodeSwitchList.h"
#include "KviKvsRunTimeContext.h"
#include "KviLocale.h"

KviKvsTreeNodeSpecialCommandForeach::KviKvsTreeNodeSpecialCommandForeach(const QChar * pLocation, KviKvsTreeNodeData * pVar, KviKvsTreeNodeDataList * pArgs, KviKvsTreeNodeInstruction * pLoop)
    : KviKvsTreeNodeSpecialCommand(pLocation, "foreach")
{
	m_pIterationVariable = pVar;
	m_pIterationVariable->setParent(this);
	m_pIterationData = pArgs;
	m_pIterationData->setParent(this);
	m_pLoop = pLoop;
	m_pLoop->setParent(this);
}

KviKvsTreeNodeSpecialCommandForeach::~KviKvsTreeNodeSpecialCommandForeach()
{
	delete m_pIterationVariable;
	delete m_pIterationData;
	delete m_pLoop;
}

void KviKvsTreeNodeSpecialCommandForeach::contextDescription(QString & szBuffer)
{
	szBuffer = "Special Command \"foreach\"";
}

void KviKvsTreeNodeSpecialCommandForeach::dump(const char * prefix)
{
	qDebug("%s SpecialCommandForeach", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pIterationVariable->dump(tmp.toUtf8().data());
	m_pIterationData->dump(tmp.toUtf8().data());
	m_pLoop->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeSpecialCommandForeach::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariantList l;
	l.setAutoDelete(true);
	if(!m_pIterationData->evaluate(c, &l))
		return false;

	KviKvsSwitchList swl;
	if(m_pSwitches)
	{
		if(!(m_pSwitches->evaluate(c, &swl)))
			return false;
	}

	bool bIncludeEmptyScalars = swl.find('a', "all") != nullptr;

	for(KviKvsVariant * pArg = l.first(); pArg; pArg = l.next())
	{
		switch(pArg->type())
		{
			case KviKvsVariantData::Array:
			{
				unsigned int uCnt = pArg->array()->size();
				unsigned int idx = 0;
				while(idx < uCnt)
				{
					// we evaluate this each time (as it may actually be killed at each iteration)
					// FIXME: maybe some kind of reference counting or a observer pattern might be a bit more efficient here
					//        (but might be far less efficient everywhere else...)
					KviKvsRWEvaluationResult * v = m_pIterationVariable->evaluateReadWrite(c);
					if(!v)
						return false;
					KviKvsVariant * pOne = pArg->array()->at(idx);
					if(pOne)
					{
						if(bIncludeEmptyScalars || (!pOne->isEmpty()))
						{
							v->result()->copyFrom(*pOne);
						}
						else
						{
							delete v; // we're done with it for this iteration
							idx++;
							continue;
						}
					}
					else
					{
						if(bIncludeEmptyScalars)
						{
							v->result()->setNothing();
						}
						else
						{
							delete v; // we're done with it for this iteration
							idx++;
							continue;
						}
					}
					delete v; // we're done with it for this iteration

					if(!m_pLoop->execute(c))
					{
						if(c->error())
							return false;

						// break allowed!
						if(c->breakPending())
						{
							c->handleBreak();
							return true;
						}

						if(c->continuePending())
						{
							c->handleContinue();
							idx++;
							continue;
						}

						return false; // propagate the false return value
					}

					idx++;
				}
			}
			break;
			case KviKvsVariantData::Hash:
			{
				KviKvsHashIterator it(*(pArg->hash()->dict()));
				while(KviKvsVariant * pOne = it.current())
				{
					// we evaluate this each time (as it may actually be killed at each iteration)
					// FIXME: maybe some kind of reference counting or a observer pattern might be a bit more efficient here
					//        (but might be far less efficient everywhere else...)
					KviKvsRWEvaluationResult * v = m_pIterationVariable->evaluateReadWrite(c);
					if(!v)
						return false;

					if(bIncludeEmptyScalars || (!pOne->isEmpty()))
					{
						v->result()->copyFrom(*pOne);
					}
					else
					{
						delete v; // we're done with it for this iteration
						++it;
						continue;
					}
					delete v; // we're done with it for this iteration

					if(!m_pLoop->execute(c))
					{
						if(c->error())
							return false;

						// break allowed!
						if(c->breakPending())
						{
							c->handleBreak();
							return true;
						}

						if(c->continuePending())
						{
							c->handleContinue();
							++it;
							continue;
						}

						return false; // propagate the false return value
					}

					++it;
				}
			}
			break;
			default:
				if(bIncludeEmptyScalars || (!pArg->isEqualToNothing()))
				{
					// we evaluate this each time (as it may actually be killed at each iteration)
					// FIXME: maybe some kind of reference counting or a observer pattern might be a bit more efficient here
					//        (but might be far less efficient everywhere else...)
					KviKvsRWEvaluationResult * v = m_pIterationVariable->evaluateReadWrite(c);
					if(!v)
						return false;
					v->result()->copyFrom(*pArg);
					delete v; // we're done with it for this iteration

					if(!m_pLoop->execute(c))
					{
						if(c->error())
							return false;

						// break allowed!
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

						return false; // propagate the false return value
					}
				}
				break;
		}
	}

	return true;
}
