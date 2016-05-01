//=============================================================================
//
//   File : KviKvsTreeNodeInstructionBlock.cpp
//   Creation date : Thu 09 Oct 2003 01:44:31 by Szymon Stefanek
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

#include "KviKvsTreeNodeInstructionBlock.h"
#include "KviKvsRunTimeContext.h"

KviKvsTreeNodeInstructionBlock::KviKvsTreeNodeInstructionBlock(const QChar * pLocation)
    : KviKvsTreeNodeInstruction(pLocation)
{
	m_pInstructionList = new KviPointerList<KviKvsTreeNodeInstruction>;
	m_pInstructionList->setAutoDelete(true);
}

KviKvsTreeNodeInstructionBlock::~KviKvsTreeNodeInstructionBlock()
{
	delete m_pInstructionList;
}

void KviKvsTreeNodeInstructionBlock::contextDescription(QString & szBuffer)
{
	szBuffer = "Instruction Block";
}

void KviKvsTreeNodeInstructionBlock::dump(const char * prefix)
{
	qDebug("%s InstructionBlock", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	for(KviKvsTreeNodeInstruction * i = m_pInstructionList->first(); i; i = m_pInstructionList->next())
	{
		i->dump(tmp.toUtf8().data());
	}
}

KviKvsTreeNodeInstruction * KviKvsTreeNodeInstructionBlock::releaseFirst()
{
	m_pInstructionList->setAutoDelete(false);
	KviKvsTreeNodeInstruction * i = m_pInstructionList->first();
	if(i)
	{
		i->setParent(nullptr);
		m_pInstructionList->removeFirst();
	}
	m_pInstructionList->setAutoDelete(true);
	return i;
}

void KviKvsTreeNodeInstructionBlock::addInstruction(KviKvsTreeNodeInstruction * i)
{
	m_pInstructionList->append(i);
	i->setParent(this);
}

bool KviKvsTreeNodeInstructionBlock::execute(KviKvsRunTimeContext * c)
{
	// to accommodate recursion we need to use an iterator here
	KviPointerListIterator<KviKvsTreeNodeInstruction> it(*m_pInstructionList);
	while(KviKvsTreeNodeInstruction * i = it.current())
	{
		if(!i->execute(c))
			return false;
		++it;
	}
	return true;
}
