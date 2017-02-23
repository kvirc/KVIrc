#ifndef _KVI_REGCHAN_H_
#define _KVI_REGCHAN_H_
//=============================================================================
//
//   File : KviRegisteredChannel.h
//   Creation date : Sat Jun 29 01:01:15 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "KviHeapObject.h"
#include "KviPointerList.h"

#include <QHash>
#include <QString>

class KVILIB_API KviRegisteredChannel : public KviHeapObject
{
	friend class KviRegisteredChannelDataBase;

public:
	KviRegisteredChannel(const QString & szName, const QString & szNetMask);
	~KviRegisteredChannel();

protected:
	QString m_szName;
	QString m_szNetMask;
	QHash<QString, QString> * m_pPropertyDict;

public:
	QHash<QString, QString> * propertyDict() { return m_pPropertyDict; }

	const QString & name() { return m_szName; }

	const QString & netMask() { return m_szNetMask; }

	QString property(const QString & szName) { return m_pPropertyDict->value(szName); }

	void setProperty(const QString & szName, const QString & szValue) { m_pPropertyDict->insert(szName, szValue); }

	void removeProperty(const QString & szName) { m_pPropertyDict->remove(szName); }
};

typedef KVILIB_API_TYPEDEF KviPointerList<KviRegisteredChannel> KviRegisteredChannelList;

#endif //_KVI_REGCHAN_H_
