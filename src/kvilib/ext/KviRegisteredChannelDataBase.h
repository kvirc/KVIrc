#ifndef _KVIREGCHANDB_H_
#define _KVIREGCHANDB_H_
//=============================================================================
//
//   File : KviRegisteredChannelDataBase.h
//   Creation date : Wed Dec 29 2010 03:28:05 CEST by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Elvio Basello (hellvis69 at gmail dot com)
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

// this file was originally part of KviRegisteredChannel.h

#include "kvi_settings.h"
#include "KviRegisteredChannel.h"

#include <QHash>

class QString;

class KVILIB_API KviRegisteredChannelDataBase
{
public:
	KviRegisteredChannelDataBase();
	~KviRegisteredChannelDataBase();

protected:
	QHash<QString, KviRegisteredChannelList *> * m_pChannelDict;

public:
	QHash<QString, KviRegisteredChannelList *> * channelDict() { return m_pChannelDict; }

	KviRegisteredChannel * find(const QString & szName, const QString & szNetMask);

	KviRegisteredChannel * findExact(const QString & szName, const QString & szNetMask);

	void remove(KviRegisteredChannel * pChan);
	void add(KviRegisteredChannel * pChan);

	void load(const QString & szFilename);
	void save(const QString & szFilename);
};

#endif // _KVIREGCHANDB_H_
