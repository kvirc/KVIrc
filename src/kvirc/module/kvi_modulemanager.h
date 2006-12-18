#ifndef _KVI_MODULEMANAGER_H_
#define _KVI_MODULEMANAGER_H_

//=============================================================================
//
//   File : kvi_modulemanager.h
//   Creation date : Sat Aug 12 2000 20:31:17 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2005 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"

#include <qobject.h>
#include <qasciidict.h>
#include <qtimer.h>
#include "kvi_list.h"

#include "kvi_module.h"

class KVIRC_API KviModuleManager : public QObject
{
	Q_OBJECT
public:
	KviModuleManager();
	~KviModuleManager();
private:
	QAsciiDict<KviModule>                        * m_pModuleDict;
	QTimer                                       * m_pCleanupTimer;
	KviStr                                         m_szLastError;
public:
	KviStr & lastError(){ return m_szLastError; };
	KviModule * findModule(const char * modName);
	KviModule * getModule(const char * modName);
	bool loadModule(const char * modName);
	bool unloadModule(const char * modName);
	bool unloadModule(KviModule * module);
	void unloadAllModules();
	//void registerDefaultCommands(KviModule * module);
	void loadModulesByCaps(const char * caps,const char * dir);
	void loadModulesByCaps(const char * caps);
	bool hasLockedModules();
	void completeModuleNames(const QString &word,KviPtrList<QString> * matches);
protected:
	void completeModuleNames(const QString &path,const QString &work,KviPtrList<QString> * matches);
public slots:
	void cleanupUnusedModules();
signals:
	void moduleAboutToUnload(KviModule * m);
};

extern KVIRC_API KviModuleManager * g_pModuleManager;

#endif //_KVI_MODULEMANAGER_H_
