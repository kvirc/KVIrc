//============================================================================
//
//   File : KviInputHistory.cpp
//   Creation date : Fri Sep 5 2008 17:26:34 by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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
//============================================================================

//
//   This file was originally part of KviInput.h
//

#include "KviInputHistory.h"
#include "KviConfigurationFile.h"
#include "KviCString.h"

#include <QString>

KviInputHistory * KviInputHistory::m_pSelf = nullptr;
unsigned int KviInputHistory::m_uCount = 0;

void KviInputHistory::init()
{
	if((!m_pSelf) && (m_pSelf->count() == 0))
	{
		m_pSelf = new KviInputHistory();
		addRef();
	}
}

void KviInputHistory::done()
{
	delRef();
	if(m_pSelf->count() == 0)
		delete m_pSelf;
}

void KviInputHistory::addRef()
{
	m_uCount++;
}

void KviInputHistory::delRef()
{
	m_uCount--;
}

void KviInputHistory::add(const QString & szString)
{
	if(!m_StringList.empty() && m_StringList.back() == szString)
		return;

	m_StringList.push_back(szString);
	if(m_StringList.size() > KVI_INPUT_MAX_GLOBAL_HISTORY_ENTRIES)
		m_StringList.erase(m_StringList.begin());
}

void KviInputHistory::load(const QString & szFileName)
{
	KviConfigurationFile c(szFileName, KviConfigurationFile::Read);

	int iCount = c.readIntEntry("Count", 0);

	if(iCount > KVI_INPUT_MAX_GLOBAL_HISTORY_ENTRIES)
		iCount = KVI_INPUT_MAX_GLOBAL_HISTORY_ENTRIES;

	KviCString szTmp;

	for(int i = 0; i < iCount; i++)
	{
		szTmp.sprintf("S%d", i);
		QString szEntry = c.readEntry(szTmp.ptr(), "");
		if(!szEntry.isEmpty())
			add(szEntry);
	}
}

void KviInputHistory::save(const QString & szFileName)
{
	KviConfigurationFile c(szFileName, KviConfigurationFile::Write);
	c.clear();

	KviCString szTmp;
	int iIdx = 0;

	for(auto & szString : m_StringList)
	{
		if(!szString.isEmpty())
		{
			szTmp.sprintf("S%d", iIdx);
			c.writeEntry(szTmp.ptr(), szString);
			iIdx++;
		}
	}

	c.writeEntry("Count", iIdx);
}
