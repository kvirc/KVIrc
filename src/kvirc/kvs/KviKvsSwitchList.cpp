//=============================================================================
//
//   File : KviKvsSwitchList.cpp
//   Creation date : Mon 27 Oct 2003 03:47:48 by Szymon Stefanek
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

#include "KviKvsSwitchList.h"

KviKvsSwitchList::KviKvsSwitchList()
{
	m_pShortSwitchDict = nullptr;
	m_pLongSwitchDict = nullptr;
}

KviKvsSwitchList::~KviKvsSwitchList()
{
	if(m_pShortSwitchDict)
		delete m_pShortSwitchDict;
	if(m_pLongSwitchDict)
		delete m_pLongSwitchDict;
}

void KviKvsSwitchList::clear()
{
	if(m_pShortSwitchDict)
	{
		delete m_pShortSwitchDict;
		m_pShortSwitchDict = nullptr;
	}
	if(m_pLongSwitchDict)
	{
		delete m_pLongSwitchDict;
		m_pLongSwitchDict = nullptr;
	}
}

void KviKvsSwitchList::addShort(unsigned short uShortKey, KviKvsVariant * pVariant)
{
	if(!m_pShortSwitchDict)
	{
		m_pShortSwitchDict = new KviPointerHashTable<unsigned short, KviKvsVariant>(11);
		m_pShortSwitchDict->setAutoDelete(true);
	}
	m_pShortSwitchDict->replace(uShortKey, pVariant);
}

void KviKvsSwitchList::addLong(const QString & szLongKey, KviKvsVariant * pVariant)
{
	if(!m_pLongSwitchDict)
	{
		m_pLongSwitchDict = new KviPointerHashTable<QString, KviKvsVariant>(11);
		m_pLongSwitchDict->setAutoDelete(true);
	}
	m_pLongSwitchDict->replace(szLongKey, pVariant);
}
