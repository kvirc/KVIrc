//=============================================================================
//
//   File : kvi_ircconnectionasyncwhoisdata.cpp
//   Created on Sat 26 Jun 2004 19:40:20 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_ircconnectionasyncwhoisdata.h"
#include "kvi_kvs_script.h"

KviAsyncWhoisInfo::KviAsyncWhoisInfo()
{
	pCallback = 0;
	pMagic = 0;
}

KviAsyncWhoisInfo::~KviAsyncWhoisInfo()
{
	if(pCallback)delete pCallback;
	if(pMagic)delete pMagic;
}

KviIrcConnectionAsyncWhoisData::KviIrcConnectionAsyncWhoisData()
{
	m_pWhoisInfoList = 0;
}

KviIrcConnectionAsyncWhoisData::~KviIrcConnectionAsyncWhoisData()
{
	if(m_pWhoisInfoList)delete m_pWhoisInfoList;
}

void KviIrcConnectionAsyncWhoisData::add(KviAsyncWhoisInfo * i)
{
	if(!m_pWhoisInfoList)
	{
		m_pWhoisInfoList = new KviPointerList<KviAsyncWhoisInfo>;
		m_pWhoisInfoList->setAutoDelete(true);
	}
	m_pWhoisInfoList->append(i);
}

KviAsyncWhoisInfo * KviIrcConnectionAsyncWhoisData::lookup(const QString &nick)
{
	if(!m_pWhoisInfoList)return 0;
	for(KviAsyncWhoisInfo * i = m_pWhoisInfoList->first();i;i = m_pWhoisInfoList->next())
	{
		if(KviQString::equalCI(nick,i->szNick))return i;
	}
	return 0;
}

void KviIrcConnectionAsyncWhoisData::remove(KviAsyncWhoisInfo * i)
{
	if(!m_pWhoisInfoList)return;
	m_pWhoisInfoList->removeRef(i);
	if(m_pWhoisInfoList->isEmpty())
	{
		delete m_pWhoisInfoList;
		m_pWhoisInfoList = 0;
	}
}
