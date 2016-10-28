#ifndef _KVI_FILETRADER_H_
#define _KVI_FILETRADER_H_
//=============================================================================
//
//   File : KviSharedFilesManager.h
//   Creation date : Wed Aug 27 2000 10:28:51 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "KviPointerHashTable.h"
#include "KviSharedFile.h"

#include <QObject>

class KviIrcMask;
class QString;
class QTimer;

class KVILIB_API KviSharedFilesManager : public QObject
{
	Q_OBJECT
public:
	KviSharedFilesManager();
	~KviSharedFilesManager();

private:
	QTimer * m_pCleanupTimer;
	KviPointerHashTable<QString, KviSharedFileList> * m_pSharedListDict;

public:
	void addSharedFile(KviSharedFile * f);
	KviSharedFile * addSharedFile(const QString & szName, const QString & szAbsPath, const QString & szMask, int timeoutInSecs);
	KviSharedFile * lookupSharedFile(const QString & szName, KviIrcMask * mask, unsigned int uFileSize = 0);
	bool removeSharedFile(const QString & szName, const QString & szMask, unsigned int uFileSize);
	bool removeSharedFile(const QString & szName, KviSharedFile * off);
	void load(const QString & filename);
	void save(const QString & filename);
	void clear();
	KviPointerHashTable<QString, KviSharedFileList> * sharedFileListDict() { return m_pSharedListDict; };
private:
	void doInsert(KviSharedFileList * l, KviSharedFile * o);
private slots:
	void cleanup();
signals:
	void sharedFilesChanged(); // emitted when the list is cleared at once
	void sharedFileAdded(KviSharedFile * f);
	void sharedFileRemoved(KviSharedFile * f);
};

#endif //_KVI_FILETRADER_H_
