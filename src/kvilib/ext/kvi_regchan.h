#ifndef _KVI_REGCHAN_H_
#define _KVI_REGCHAN_H_
//=============================================================================
//
//   File : kvi_regchan.h
//   Creation date : Sat Jun 29 01:01:15 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"
#include "kvi_heapobject.h"
#include "kvi_string.h"
#include "kvi_pointerlist.h"

#include <QHash>

class KVILIB_API KviRegisteredChannel : public KviHeapObject
{
	friend class KviRegisteredChannelDataBase;
public:
	KviRegisteredChannel(const QString &name,const QString &netmask);
	~KviRegisteredChannel();
protected:
	QString                  m_szName;
	QString                  m_szNetMask;
	QHash<QString,QString> * m_pPropertyDict;
public:
	QHash<QString,QString> * propertyDict()
	{
		return m_pPropertyDict;
	}

	const QString & name()
	{
		return m_szName;
	}

	const QString & netMask()
	{
		return m_szNetMask;
	}

	QString property(const QString &name)
	{
		return m_pPropertyDict->value(name);
	}

	void setProperty(const QString &name,const QString &val)
	{
		m_pPropertyDict->insert(name,val);
	}

	void removeProperty(const QString &name)
	{
		m_pPropertyDict->remove(name);
	}
};

typedef KVILIB_API_TYPEDEF KviPointerList<KviRegisteredChannel> KviRegisteredChannelList;

class KVILIB_API KviRegisteredChannelDataBase
{
public:
	KviRegisteredChannelDataBase();
	~KviRegisteredChannelDataBase();
protected:
	QHash<QString,KviRegisteredChannelList *> * m_pChannelDict;
public:
	QHash<QString,KviRegisteredChannelList *> * channelDict()
	{
		return m_pChannelDict;
	}

	KviRegisteredChannel * find(const QString &name,const QString &net);

	KviRegisteredChannel * findExact(const QString &name,const QString &netmask);

	void remove(KviRegisteredChannel * c);
	void add(KviRegisteredChannel * c);

	void load(const QString &filename);
	void save(const QString &filename);
};


#endif //_KVI_REGCHAN_H_
