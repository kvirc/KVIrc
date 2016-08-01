#ifndef _KVI_MODULEMANAGER_H_
#define _KVI_MODULEMANAGER_H_
//=============================================================================
//
//   File : KviModuleManager.h
//   Creation date : Sat Aug 12 2000 20:31:17 by Szymon Stefanek
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
#include "KviModule.h"
#include "KviPointerHashTable.h"

#include <QTimer>
#include <QObject>
#include <vector>

class KVIRC_API KviModuleManager : public QObject
{
	Q_OBJECT
public:
	KviModuleManager();
	~KviModuleManager();

private:
	KviPointerHashTable<QString, KviModule> * m_pModuleDict;
	QTimer * m_pCleanupTimer;
	QString m_szLastError;

public:
	QString & lastError() { return m_szLastError; };
	KviModule * findModule(const QString & modName);
	KviModule * getModule(const QString & modName);
	bool loadModule(const QString & modName);
	bool unloadModule(const QString & modName);
	bool unloadModule(KviModule * module);
	void unloadAllModules();
	//void registerDefaultCommands(KviModule * module);
	void loadModulesByCaps(const QString & caps, const QString & dir);
	void loadModulesByCaps(const QString & caps);
	bool hasLockedModules();
	void completeModuleNames(const QString & word, std::vector<QString> & matches);

protected:
	void completeModuleNames(const QString & path, const QString & work, std::vector<QString> & matches);
public slots:
	void cleanupUnusedModules();
signals:
	void moduleAboutToUnload(KviModule * m);
};

extern KVIRC_API KviModuleManager * g_pModuleManager;

#endif //_KVI_MODULEMANAGER_H_
