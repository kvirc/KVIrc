//============================================================================
//
//   File : KviInputHistory.cpp
//   Creation date : Fri Sep 5 2008 17:26:34 by Elvio Basello
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//============================================================================
//   This file was originally part of KviInput.h
//============================================================================

#include "KviInputHistory.h"
#include "KviConfigurationFile.h"
#include "KviCString.h"

KviInputHistory * KviInputHistory::m_pSelf = NULL;
unsigned int KviInputHistory::m_uCount = 0;

KviInputHistory::KviInputHistory()
{
	m_pStringList = new KviPointerList<QString>;
	m_pStringList->setAutoDelete(true);
}

KviInputHistory::~KviInputHistory()
{
	delete m_pStringList;
}

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

void KviInputHistory::add(QString * szString)
{
	m_pStringList->insert(0,szString);
	if(m_pStringList->count() > KVI_INPUT_MAX_GLOBAL_HISTORY_ENTRIES)
		m_pStringList->removeLast();
}

void KviInputHistory::load(const QString & szFileName)
{
	KviConfigurationFile c(szFileName,KviConfigurationFile::Read);

	int iCount = c.readIntEntry("Count",0);

	if(iCount > KVI_INPUT_MAX_GLOBAL_HISTORY_ENTRIES)
		iCount = KVI_INPUT_MAX_GLOBAL_HISTORY_ENTRIES;

	KviCString szTmp;

	for(int i=0; i<iCount; i++)
	{
		szTmp.sprintf("S%d",i);
		QString szEntry = c.readEntry(szTmp.ptr(),"");
		if(!szEntry.isEmpty())
			add(new QString(szEntry));
	}
}

void KviInputHistory::save(const QString & szFileName)
{
	KviConfigurationFile c(szFileName,KviConfigurationFile::Write);
	c.clear();

	c.writeEntry("Count",m_pStringList->count());

	KviCString szTmp;
	int iIdx = 0;

	for(QString * szString = m_pStringList->first(); szString; szString = m_pStringList->next())
	{
		if(!szString->isEmpty())
		{
			szTmp.sprintf("S%d",iIdx);
			c.writeEntry(szTmp.ptr(),*szString);
			iIdx++;
		}
	}
}
