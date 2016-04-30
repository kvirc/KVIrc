//=============================================================================
//
//   File : KviKvsTreeNodeDataList.cpp
//   Creation date : Tue 07 Oct 2003 02:03:41 by Szymon Stefanek
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

#include "KviKvsTreeNodeDataList.h"
#include "KviKvsRunTimeContext.h"

#include "KviQString.h"

KviKvsTreeNodeDataList::KviKvsTreeNodeDataList(const QChar * pLocation)
    : KviKvsTreeNode(pLocation)
{
	m_pDataList = new KviPointerList<KviKvsTreeNodeData>();
	m_pDataList->setAutoDelete(true);
}

KviKvsTreeNodeDataList::~KviKvsTreeNodeDataList()
{
	delete m_pDataList;
}

KviKvsTreeNodeData * KviKvsTreeNodeDataList::releaseFirst()
{
	KviKvsTreeNodeData * d = m_pDataList->first();
	if(d)
	{
		m_pDataList->setAutoDelete(false);
		m_pDataList->removeFirst();
		m_pDataList->setAutoDelete(true);
	}
	return d;
}

KviKvsTreeNodeData * KviKvsTreeNodeDataList::item(unsigned int uIdx)
{
	if(uIdx >= m_pDataList->count())
		return nullptr;
	return m_pDataList->at(uIdx);
}

void KviKvsTreeNodeDataList::contextDescription(QString & szBuffer)
{
	szBuffer = "Data List Evaluation";
}

void KviKvsTreeNodeDataList::dump(const char * prefix)
{
	qDebug("%s DataList", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	for(KviKvsTreeNodeData * t = m_pDataList->first(); t; t = m_pDataList->next())
	{
		t->dump(tmp.toUtf8().data());
	}
}

bool KviKvsTreeNodeDataList::evaluate(KviKvsRunTimeContext * c, KviKvsVariantList * pBuffer)
{
	pBuffer->clear();

	// we use an iterator to accommodate recursion
	KviPointerListIterator<KviKvsTreeNodeData> it(*m_pDataList);
	while(KviKvsTreeNodeData * t = it.current())
	{
		KviKvsVariant * v = new KviKvsVariant();
		if(!t->evaluateReadOnly(c, v))
		{
			delete v;
			pBuffer->clear();
			return false;
		}
		pBuffer->append(v);
		++it;
	}
	return true;
}

void KviKvsTreeNodeDataList::addItem(KviKvsTreeNodeData * p)
{
	m_pDataList->append(p);
	p->setParent(this);
}

void KviKvsTreeNodeDataList::prependItem(KviKvsTreeNodeData * p)
{
	m_pDataList->prepend(p);
	p->setParent(this);
}
