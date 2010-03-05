#ifndef _KVI_IRCCONNECTIONASYNCTEMPLATEDATA_H_
#define _KVI_IRCCONNECTIONASYNCTEMPLATEDATA_H_
//=============================================================================
//
//   File : kvi_ircconnectionasyncwhoisdata.h
//   Creation date : Sat 26 Jun 2004 19:40:20 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2008 Szymon Stefanek <pragma at kvirc dot net>
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
//=============================================================================

// This template class will be used to create some other classes for future commands
// KviIrcConnectionAsyncWhoData for creating an awho command
// KviIrcConnectionAsyncWhowasData for creating an awhowas command
// It is now also used by KviIrcConnectionAsyncWhoisData

template <class T>
class KVIRC_API KviIrcConnectionAsyncTemplateData
{
public:
	KviIrcConnectionAsyncTemplateData()
	{
		m_pInfoList = 0;
	}

	~KviIrcConnectionAsyncTemplateData()
	{
		if(m_pInfoList)delete m_pInfoList;
	}

protected:
	KviPointerList<T> * m_pInfoList; // awhois pending stuff
public:
	void add(T * i)
	{
		if(!m_pInfoList)
		{
			m_pInfoList = new KviPointerList<T>;
			m_pInfoList->setAutoDelete(true);
		}
		m_pInfoList->append(i);
	}

	T * lookup(const QString &nick)
	{
		if(!m_pInfoList)return 0;
		for(T * i = m_pInfoList->first();i;i = m_pInfoList->next())
		{
			if(KviQString::equalCI(nick,i->szNick))return i;
		}
		return 0;
	}

	void remove(T * i)
	{
		if(!m_pInfoList)return;
		m_pInfoList->removeRef(i);
		if(m_pInfoList->isEmpty())
		{
			delete m_pInfoList;
			m_pInfoList = 0;
		}
	}
};

#endif

