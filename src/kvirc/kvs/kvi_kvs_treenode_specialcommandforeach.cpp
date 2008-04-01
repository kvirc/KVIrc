//=============================================================================
//
//   File : kvi_kvs_treenode_specialcommandforeach.cpp
//   Created on Fri 07 Nov 2003 11:48:11 by Szymon Stefanek
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

#include "kvi_kvs_treenode_specialcommandforeach.h"
#include "kvi_kvs_treenode_data.h"
#include "kvi_kvs_treenode_datalist.h"
#include "kvi_kvs_treenode_instruction.h"
#include "kvi_kvs_treenode_switchlist.h"
#include "kvi_kvs_runtimecontext.h"
#include "kvi_locale.h"

KviKvsTreeNodeSpecialCommandForeach::KviKvsTreeNodeSpecialCommandForeach(const QChar * pLocation,KviKvsTreeNodeData * pVar,KviKvsTreeNodeDataList * pArgs,KviKvsTreeNodeInstruction * pLoop)
: KviKvsTreeNodeSpecialCommand(pLocation,"foreach")
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

void KviKvsTreeNodeSpecialCommandForeach::contextDescription(QString &szBuffer)
{
	szBuffer = "Special Command \"foreach\"";
}

void KviKvsTreeNodeSpecialCommandForeach::dump(const char * prefix)
{
	qDebug("%s SpecialCommandForeach",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pIterationVariable->dump(tmp.utf8().data());
	m_pIterationData->dump(tmp.utf8().data());
	m_pLoop->dump(tmp.utf8().data());
}

bool KviKvsTreeNodeSpecialCommandForeach::execute(KviKvsRunTimeContext * c)
{
	KviKvsRWEvaluationResult * v = m_pIterationVariable->evaluateReadWrite(c);
	if(!v)return false;

	KviKvsVariantList l;
	l.setAutoDelete(true);
	if(!m_pIterationData->evaluate(c,&l))
	{
		delete v;
		return false;
	}

	KviKvsSwitchList swl;
	if(m_pSwitches)
	{
		if(!(m_pSwitches->evaluate(c,&swl)))return false;
	}

	bool bIncludeEmptyScalars = swl.find('a',"all") != 0;

	for(KviKvsVariant * pArg = l.first();pArg;pArg = l.next())
	{
		switch(pArg->type())
		{
			case KviKvsVariantData::Array:
			{
				unsigned int uCnt = pArg->array()->size();
				unsigned int idx = 0;
				while(idx < uCnt)
				{
					KviKvsVariant * pOne = pArg->array()->at(idx);
					if(pOne)
					{
						v->result()->copyFrom(*pOne);
					} else {
						v->result()->setNothing();
					}
					
					if(!m_pLoop->execute(c))
					{
						// break allowed!
	
						if(c->error())
						{
							delete v;
							return false;
						}
						// break allowed!
						if(c->breakPending())
						{
							c->handleBreak();
							delete v;
							return true;
						}
						delete v;
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
					v->result()->copyFrom(*pOne);

					if(!m_pLoop->execute(c))
					{
						// break allowed!
	
						if(c->error())
						{
							delete v;
							return false;
						}
						// break allowed!
						if(c->breakPending())
						{
							c->handleBreak();
							delete v;
							return true;
						}
						delete v;
						return false; // propagate the false return value
					}

					++it;
				}
			}
			break;
			default:
				if(bIncludeEmptyScalars || (!pArg->isNothing()))
				{
					v->result()->copyFrom(*pArg);
					if(!m_pLoop->execute(c))
					{
						// break allowed!
	
						if(c->error())
						{
							delete v;
							return false;
						}
						// break allowed!
						if(c->breakPending())
						{
							c->handleBreak();
							delete v;
							return true;
						}
						delete v;
						return false; // propagate the false return value
					}
				}
			break;
		}
	}
	delete v;
	return true;
}
