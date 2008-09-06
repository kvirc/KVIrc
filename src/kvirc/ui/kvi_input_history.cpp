//============================================================================
//
//   File : kvi_input_history.cpp
//   Creation date : Fri Sep 5 2008 17:26:34 by Elvio Basello
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2008 Szymon Stefanek (pragma at kvirc dot net)
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
//============================================================================

#include "kvi_input_history.h"
#include "kvi_config.h"
#include "kvi_string.h"
#include "kvi_app.h"
#include "kvi_confignames.h"

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

	QString tmp;
	if(g_pApp->getReadOnlyConfigPath(tmp,KVI_CONFIGFILE_INPUTHISTORY))
		instance()->load(tmp);
}

void KviInputHistory::done()
{
	QString tmp;
	if(g_pApp->getReadOnlyConfigPath(tmp,KVI_CONFIGFILE_INPUTHISTORY))
		instance()->save(tmp);

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
	KviConfig c(szFileName,KviConfig::Read);

	int cnt = c.readIntEntry("Count",0);

	if(cnt > KVI_INPUT_MAX_GLOBAL_HISTORY_ENTRIES)
		cnt = KVI_INPUT_MAX_GLOBAL_HISTORY_ENTRIES;

	KviStr tmp;

	for(int i=0;i<cnt;i++)
	{
		tmp.sprintf("S%d",i);
		QString szEntry = c.readQStringEntry(tmp.ptr(),"");
		if(!szEntry.isEmpty())
			add(new QString(szEntry));
	}
}

void KviInputHistory::save(const QString & szFileName)
{
	KviConfig c(szFileName,KviConfig::Write);
	c.clear();

	c.writeEntry("Count",m_pStringList->count());

	KviStr tmp;
	int idx = 0;

	for(QString * szString = m_pStringList->first(); szString; szString = m_pStringList->next())
	{
		if(!szString->isEmpty())
		{
			tmp.sprintf("S%d",idx);
			c.writeEntry(tmp.ptr(),*szString);
			idx++;
		}
	}
}
