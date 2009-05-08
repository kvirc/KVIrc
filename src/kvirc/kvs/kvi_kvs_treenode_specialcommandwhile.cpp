//=============================================================================
//
//   File : kvi_kvs_treenode_specialcommandwhile.cpp
//   Created on Thu 06 Nov 2003 11:58:43 by Szymon Stefanek
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#define __KVIRC__

#include "kvi_kvs_treenode_specialcommandwhile.h"
#include "kvi_kvs_treenode_expression.h"
#include "kvi_kvs_treenode_instruction.h"
#include "kvi_kvs_runtimecontext.h"
#include "kvi_locale.h"

KviKvsTreeNodeSpecialCommandWhile::KviKvsTreeNodeSpecialCommandWhile(const QChar * pLocation,KviKvsTreeNodeExpression * e,KviKvsTreeNodeInstruction * i)
: KviKvsTreeNodeSpecialCommand(pLocation,"while")
{
#ifdef COMPILE_NEW_KVS
	m_pExpression = e;
	m_pExpression->setParent(this);
	m_pInstruction = i;
	if(i)m_pInstruction->setParent(this);
#endif
}

KviKvsTreeNodeSpecialCommandWhile::~KviKvsTreeNodeSpecialCommandWhile()
{
#ifdef COMPILE_NEW_KVS
	delete m_pExpression;
	if(m_pInstruction)delete m_pInstruction;
#endif
}

void KviKvsTreeNodeSpecialCommandWhile::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Special Command \"while\"";
#endif
}

void KviKvsTreeNodeSpecialCommandWhile::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s SpecialCommandWhile",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pExpression->dump(tmp);
	if(m_pInstruction)m_pInstruction->dump(tmp);
#endif
}

bool KviKvsTreeNodeSpecialCommandWhile::execute(KviKvsRunTimeContext * c)
{
#ifdef COMPILE_NEW_KVS
	for(;;)
	{
		KviKvsVariant v;
		if(!m_pExpression->evaluateReadOnly(c,&v))return false;
		
		if(!v.asBoolean())break;

		if(m_pInstruction)
		{
			if(!m_pInstruction->execute(c))
			{
				if(c->error())return false;

				if(c->breakPending())
				{
					c->handleBreak();
					return true;
				}
				return false;
			}
		}
	}
#endif
	return true;
}
