//=============================================================================
//
//   File : KviKvsTreeNodeSwitchList.cpp
//   Creation date : Tue 07 Oct 2003 02:06:53 by Szymon Stefanek
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

#include "KviKvsTreeNodeSwitchList.h"
#include "KviKvsRunTimeContext.h"

KviKvsTreeNodeSwitchList::KviKvsTreeNodeSwitchList(const QChar * pLocation)
    : KviKvsTreeNode(pLocation)
{
	m_pShortSwitchDict = nullptr;
	m_pLongSwitchDict = nullptr;
}

KviKvsTreeNodeSwitchList::~KviKvsTreeNodeSwitchList()
{
	if(m_pShortSwitchDict)
		delete m_pShortSwitchDict;
	if(m_pLongSwitchDict)
		delete m_pLongSwitchDict;
}

void KviKvsTreeNodeSwitchList::contextDescription(QString & szBuffer)
{
	szBuffer = "Switch List Evaluation";
}

void KviKvsTreeNodeSwitchList::dump(const char * prefix)
{
	qDebug("%s SwitchList", prefix);
#if 0
	if(m_pShortSwitchDict)
	{
		KviPointerHashTableIterator<int,KviKvsTreeNodeData> it(*m_pShortSwitchDict);
		while(it.current())
		{
			QString tmp = prefix;
			tmp.append("  Sw(");
			QChar c((unsigned short)it.currentKey());
			tmp.append(c);
			tmp.append("): ");
			it.current()->dump(tmp.toUtf8().data());
			++it;
		}
	}
	if(m_pLongSwitchDict)
	{
		KviPointerHashTableIterator<QString,KviKvsTreeNodeData> it(*m_pLongSwitchDict);
		while(it.current())
		{
			QString tmp = prefix;
			tmp.append("  Sw(");
			tmp.append(it.currentKey());
			tmp.append("): ");
			it.current()->dump(tmp.toUtf8().data());
			++it;
		}
	}
#endif
}

void KviKvsTreeNodeSwitchList::addShort(int iShortKey, KviKvsTreeNodeData * p)
{
	if(!m_pShortSwitchDict)
	{
		m_pShortSwitchDict = new KviPointerHashTable<int, KviKvsTreeNodeData>(11);
		m_pShortSwitchDict->setAutoDelete(true);
	}

	m_pShortSwitchDict->replace(iShortKey, p);
	p->setParent(this);
}

void KviKvsTreeNodeSwitchList::addLong(const QString & szLongKey, KviKvsTreeNodeData * p)
{
	if(!m_pLongSwitchDict)
	{
		m_pLongSwitchDict = new KviPointerHashTable<QString, KviKvsTreeNodeData>(11);
		m_pLongSwitchDict->setAutoDelete(true);
	}

	m_pLongSwitchDict->replace(szLongKey, p);
	p->setParent(this);
}

bool KviKvsTreeNodeSwitchList::evaluate(KviKvsRunTimeContext * c, KviKvsSwitchList * pSwList)
{
	pSwList->clear();

	if(m_pShortSwitchDict)
	{
		KviPointerHashTableIterator<int, KviKvsTreeNodeData> it(*m_pShortSwitchDict);
		while(KviKvsTreeNodeData * d = it.current())
		{
			KviKvsVariant * v = new KviKvsVariant();
			if(!d->evaluateReadOnly(c, v))
			{
				delete v;
				return false;
			}
			pSwList->addShort(it.currentKey(), v);
			++it;
		}
	}
	if(m_pLongSwitchDict)
	{
		KviPointerHashTableIterator<QString, KviKvsTreeNodeData> it(*m_pLongSwitchDict);
		while(KviKvsTreeNodeData * d = it.current())
		{
			KviKvsVariant * v = new KviKvsVariant();
			if(!d->evaluateReadOnly(c, v))
			{
				delete v;
				return false;
			}
			pSwList->addLong(it.currentKey(), v);
			++it;
		}
	}
	return true;
}

KviKvsTreeNodeData * KviKvsTreeNodeSwitchList::getStandardRebindingSwitch()
{
	KviKvsTreeNodeData * d;
	if(m_pShortSwitchDict)
	{
		d = m_pShortSwitchDict->find('r');
		if(d)
		{
			m_pShortSwitchDict->setAutoDelete(false);
			m_pShortSwitchDict->remove('r');
			m_pShortSwitchDict->setAutoDelete(true);
			return d;
		}
	}
	if(m_pLongSwitchDict)
	{
		d = m_pLongSwitchDict->find("rebind");
		if(d)
		{
			m_pLongSwitchDict->setAutoDelete(false);
			m_pLongSwitchDict->remove("rebind");
			m_pLongSwitchDict->setAutoDelete(true);
			return d;
		}
	}
	return nullptr;
}
