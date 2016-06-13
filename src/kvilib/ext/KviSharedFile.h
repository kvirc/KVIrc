#ifndef _KVISHAREDFILE_H_
#define _KVISHAREDFILE_H_
//=============================================================================
//
//   File : KviSharedFile.h
//   Creation date : Wed Dec 29 2010 03:40:05 CEST by Elvio Basello
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

// this file was originally part of KviPixmap.h

#include "kvi_settings.h"
#include "KviHeapObject.h"
#include "KviPointerList.h"

#include <QString>

#include <time.h>

class KVILIB_API KviSharedFile : public KviHeapObject
{
public:
	KviSharedFile(const QString & szName, const QString & szAbsPath, const QString & szUserMask, time_t expireTime, unsigned int uFileSize);
	~KviSharedFile();

private:
	QString m_szName;
	QString m_szAbsFilePath;
	time_t m_expireTime;
	QString m_szUserMask;
	unsigned int m_uFileSize;
	unsigned int m_uWildCount;
	unsigned int m_uNonWildCount;

public:
	const QString & name() { return m_szName; };

	const QString & absFilePath() { return m_szAbsFilePath; };

	const QString & userMask() { return m_szUserMask; };

	time_t expireTime() { return m_expireTime; };
	bool expires() { return (m_expireTime != 0); };

	unsigned int fileSize() { return m_uFileSize; };

	unsigned int wildcardCount() { return m_uWildCount; };
	unsigned int nonWildcardCount() { return m_uNonWildCount; };
	int maskLength() { return m_szUserMask.length(); };
};

typedef KviPointerList<KviSharedFile> KviSharedFileList;

#endif // _KVISHAREDFILE_H_
