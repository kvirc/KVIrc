//=============================================================================
//
//   File : kvi_kvs_treenode_specialcommandif.cpp
//   Created on Thu 09 Oct 2003 02:40:00 by Szymon Stefanek
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

#include "kvi_kvs_treenode_specialcommandif.h"
#include "kvi_kvs_treenode_expression.h"
#include "kvi_kvs_treenode_instruction.h"
#include "kvi_kvs_runtimecontext.h"
#include "kvi_locale.h"

KviKvsTreeNodeSpecialCommandIf::KviKvsTreeNodeSpecialCommandIf(const QChar * pLocation,KviKvsTreeNodeExpression * e,KviKvsTreeNodeInstruction * pIf,KviKvsTreeNodeInstruction * pElse)
: KviKvsTreeNodeSpecialCommand(pLocation,"if")
{
#ifdef COMPILE_NEW_KVS
	m_pExpression = e;
	m_pExpression->setParent(this);
	m_pIfInstruction = pIf;
	if(pIf)m_pIfInstruction->setParent(this);
	m_pElseInstruction = pElse;
	if(pElse)m_pElseInstruction->setParent(this);
#endif
}

KviKvsTreeNodeSpecialCommandIf::~KviKvsTreeNodeSpecialCommandIf()
{
#ifdef COMPILE_NEW_KVS
	delete m_pExpression;
	if(m_pIfInstruction)delete m_pIfInstruction;
	if(m_pElseInstruction)delete m_pElseInstruction;
#endif
}

void KviKvsTreeNodeSpecialCommandIf::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Special Command \"if\"";
#endif
}

void KviKvsTreeNodeSpecialCommandIf::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s SpecialCommandIf",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pExpression->dump(tmp);
	if(m_pIfInstruction)m_pIfInstruction->dump(tmp);
	if(m_pElseInstruction)m_pElseInstruction->dump(tmp);
#endif
}

bool KviKvsTreeNodeSpecialCommandIf::execute(KviKvsRunTimeContext * c)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant v;
	if(!m_pExpression->evaluateReadOnly(c,&v))return false;
	
	if(v.asBoolean())
	{
		if(m_pIfInstruction)
		{
			return m_pIfInstruction->execute(c);
		}
	} else {
		if(m_pElseInstruction)
		{
			return m_pElseInstruction->execute(c);
		}
	}
#endif
	return true;
}
