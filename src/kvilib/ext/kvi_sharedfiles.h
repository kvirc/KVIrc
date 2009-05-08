#ifndef _KVI_FILETRADER_H_
#define _KVI_FILETRADER_H_
//=============================================================================
//
//   File : kvi_filetrader.h
//   Creation date : Wed Aug 27 2000 10:28:51 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_ircmask.h"
#include "kvi_pointerlist.h"
#include "kvi_qstring.h"

#include "kvi_pointerhashtable.h"

#include <time.h>
#include <QTimer>


class KVILIB_API KviSharedFile : public KviHeapObject
{
public:
	KviSharedFile(const QString &szName,const QString &szAbsPath,const QString &szUserMask,time_t expireTime,unsigned int uFileSize);
	~KviSharedFile();
private:
	QString         m_szName;
	QString         m_szAbsFilePath;
	time_t          m_expireTime;
	QString         m_szUserMask;
	unsigned int    m_uFileSize;
	unsigned int    m_uWildCount;
	unsigned int    m_uNonWildCount;
public:
	const QString &name(){ return m_szName; };

	const QString &absFilePath(){ return m_szAbsFilePath; };

	const QString &userMask(){ return m_szUserMask; };

	time_t       expireTime(){ return m_expireTime; };
	bool         expires(){ return (m_expireTime != 0); };

	unsigned int fileSize(){ return m_uFileSize; };

	unsigned int wildcardCount(){ return m_uWildCount; };
	unsigned int nonWildcardCount(){ return m_uNonWildCount; };
	int          maskLength(){ return m_szUserMask.length(); };
};


typedef KviPointerList<KviSharedFile> KviSharedFileList;


class KVILIB_API KviSharedFilesManager : public QObject
{
	Q_OBJECT
public:
	KviSharedFilesManager();
	~KviSharedFilesManager();
private:
	QTimer                     * m_pCleanupTimer;
	KviPointerHashTable<QString,KviSharedFileList>   * m_pSharedListDict;
public:
	void addSharedFile(KviSharedFile * f);
	KviSharedFile * addSharedFile(const QString &szName,const QString &szAbsPath,const QString &szMask,int timeoutInSecs);
	KviSharedFile * lookupSharedFile(const QString &szName,KviIrcMask * mask,unsigned int uFileSize = 0);
	bool removeSharedFile(const QString &szName,const QString &szMask,unsigned int uFileSize);
	bool removeSharedFile(const QString &szName,KviSharedFile * off);
	void load(const QString &filename);
	void save(const QString &filename);
	void clear();
	KviPointerHashTable<QString,KviSharedFileList> * sharedFileListDict(){ return m_pSharedListDict; };
private:
	void doInsert(KviSharedFileList * l, KviSharedFile * o);
private slots:
	void cleanup();
signals:
	void sharedFilesChanged(); // emitted when the list is cleared at once
	void sharedFileAdded(KviSharedFile * f);
	void sharedFileRemoved(KviSharedFile * f);
};


/*
class KviSharedFile
{
	KviSharedFile();
	KviSharedFile(const KviStr &filePath,const KviStr &userMask);
	~KviSharedFile();
protected:
	KviStr             m_szFilePath;
	KviStr             m_szVisibleName;
	KviStr             m_szMd5Sum;
	KviStr             m_szUserMask;
	unsigned short int m_uWildCount;
	unsigned short int m_uNonWildCount;

	unsigned int       m_uFileSize;
	time_t             m_tExpireTime;
public:
	void setFilePath(const KviStr &filePath);
	void setUserMask(const KviStr &userMask);
	void setVisibleName(const KviStr &visibleName);
	void setMd5Sum(const KviStr &md5Sum);
	void setFileSize(unsigned int uFileSize);
	void setExpireTime(time_t expireTime);
	void doNotExpire(){ setExpireTime((time_t)0); };

	void computeMd5Sum();
};
*/

#endif //_KVI_FILETRADER_H_
