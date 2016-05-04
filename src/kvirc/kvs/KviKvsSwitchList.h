#ifndef _KVI_KVS_SWITCHLIST_H_
#define _KVI_KVS_SWITCHLIST_H_
//=============================================================================
//
//   File : KviKvsSwitchList.h
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

#include "kvi_settings.h"
#include "KviQString.h"
#include "KviKvsVariant.h"

#include "KviPointerHashTable.h"

class KVIRC_API KviKvsSwitchList
{
public:
	KviKvsSwitchList();
	~KviKvsSwitchList();

protected:
	KviPointerHashTable<unsigned short, KviKvsVariant> * m_pShortSwitchDict;
	KviPointerHashTable<QString, KviKvsVariant> * m_pLongSwitchDict;

public:
	void clear();
	void addShort(unsigned short uShortKey, KviKvsVariant * pVariant);
	void addLong(const QString & szLongKey, KviKvsVariant * pVariant);

	bool isEmpty() { return ((m_pShortSwitchDict == 0) && (m_pLongSwitchDict == 0)); };

	KviKvsVariant * find(const QChar & c)
	{
		return m_pShortSwitchDict ? m_pShortSwitchDict->find(c.unicode()) : 0;
	};

	KviKvsVariant * find(unsigned short uShortKey)
	{
		return m_pShortSwitchDict ? m_pShortSwitchDict->find((int)uShortKey) : 0;
	};

	KviKvsVariant * find(const QString & szLongKey)
	{
		return m_pLongSwitchDict ? m_pLongSwitchDict->find(szLongKey) : 0;
	};

	KviKvsVariant * find(unsigned short uShortKey, const QString & szLongKey)
	{
		if(m_pLongSwitchDict)
		{
			KviKvsVariant * t;
			t = m_pLongSwitchDict->find(szLongKey);
			if(t)
				return t;
		}
		return m_pShortSwitchDict ? m_pShortSwitchDict->find((int)uShortKey) : 0;
	};

	bool getAsStringIfExisting(unsigned short uShortKey, const QString & szLongKey, QString & szBuffer)
	{
		KviKvsVariant * v = find(uShortKey, szLongKey);
		if(v)
		{
			v->asString(szBuffer);
			return true;
		}
		return false;
	};
};

#endif //!_KVI_KVS_SWITCHLIST_H_
