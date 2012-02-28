//=============================================================================
//
//   File : KviModuleManager.cpp
//   Creation date : Sat Aug 12 2000 20:32:11 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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


#define _KVI_MODULEMANAGER_CPP_

#include "KviModuleManager.h"
#include "KviFileUtils.h"
#include "KviApplication.h"
#include "KviOptions.h"
#include "KviMainWindow.h"
#include "KviConsoleWindow.h"
#include "KviLocale.h"
#include "kvi_out.h"

#include <QDir>
#include <QLibrary>

KviModuleManager * g_pModuleManager = 0;


KviModuleManager::KviModuleManager()
{
	m_pModuleDict = new KviPointerHashTable<QString,KviModule>(17,false);
	m_pModuleDict->setAutoDelete(false);

	m_pCleanupTimer = new QTimer(this);
	connect(m_pCleanupTimer,SIGNAL(timeout()),this,SLOT(cleanupUnusedModules()));
}

KviModuleManager::~KviModuleManager()
{
	unloadAllModules();
	delete m_pModuleDict;
	delete m_pCleanupTimer;
}


void KviModuleManager::loadModulesByCaps(const QString &caps,const QString &dir)
{
	QString szCapsPath = dir;
	szCapsPath += KVI_PATH_SEPARATOR_CHAR;
	szCapsPath += "caps";
	szCapsPath += KVI_PATH_SEPARATOR_CHAR;
	szCapsPath += caps;
	szCapsPath += KVI_PATH_SEPARATOR_CHAR;

	QDir d(szCapsPath);

	// FIXME: maybe check timestamps ? (old modules)

	QStringList sl = d.entryList(QDir::Files | QDir::Readable | QDir::NoSymLinks);
	for(QStringList::Iterator it = sl.begin();it != sl.end();++it)
	{
		QString modname = *it;
		KviQString::cutToLast(modname,KVI_PATH_SEPARATOR_CHAR);
		getModule(modname);
	}
}

void KviModuleManager::loadModulesByCaps(const QString &caps)
{
	QString szDir;
	g_pApp->getLocalKvircDirectory(szDir,KviApplication::Plugins);
	loadModulesByCaps(caps,szDir);
	g_pApp->getGlobalKvircDirectory(szDir,KviApplication::Plugins);
	loadModulesByCaps(caps,szDir);
}

void KviModuleManager::completeModuleNames(const QString &path,const QString &word,KviPointerList<QString> * matches)
{
	QDir d(path);
#if defined(COMPILE_ON_WINDOWS)
	d.setNameFilters(QStringList("kvi*.dll"));
#elif defined(COMPILE_ON_MINGW)
	d.setNameFilters(QStringList("libkvi*.dll"));
#else
	d.setNameFilters(QStringList("libkvi*.so"));
#endif
	// FIXME: maybe check timestamps ? (old modules)

	QStringList sl = d.entryList(QDir::Files | QDir::Readable | QDir::NoSymLinks);
	for(QStringList::Iterator it = sl.begin();it != sl.end();++it)
	{
		QString * modname = new QString(*it);
		KviQString::cutToLast(*modname,KVI_PATH_SEPARATOR_CHAR);
		KviQString::cutToFirst(*modname,"kvi");
		if(KviQString::equalCIN(word,*modname,word.length()))
		{
			#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
			KviQString::cutFromLast(*modname,".dll");
			#else
			KviQString::cutFromLast(*modname,".so");
			#endif
			if(!modname->isEmpty())
			{
				modname->append('.');
				matches->append(modname);
			} else delete modname;
		} else delete modname;
	}
}

void KviModuleManager::completeModuleNames(const QString &word,KviPointerList<QString> * matches)
{
	QString szDir;
	// FIXME: Should check for duplicate names here!
	g_pApp->getLocalKvircDirectory(szDir,KviApplication::Modules);
	completeModuleNames(szDir,word,matches);
	g_pApp->getGlobalKvircDirectory(szDir,KviApplication::Modules);
	completeModuleNames(szDir,word,matches);
}

KviModule * KviModuleManager::findModule(const QString &modName)
{
	KviModule * m = m_pModuleDict->find(modName);
	if(m)m->updateAccessTime();
	return m;
}

KviModule * KviModuleManager::getModule(const QString &modName)
{
	KviModule * m = m_pModuleDict->find(modName);
	if(!m)
	{
		if(!loadModule(modName)) return 0;
		m = m_pModuleDict->find(modName);
	}
	if(m)m->updateAccessTime();
	return m;
}

bool KviModuleManager::loadModule(const QString &modName)
{
	if(findModule(modName))
	{
		//qDebug("MODULE %s ALREADY IN CORE MEMORY",modName);
		return true;
	}
	QString tmp;
	QString szName;
#if defined(COMPILE_ON_WINDOWS)
	KviQString::appendFormatted(szName,"kvi%Q.dll",&modName);
#elif defined(COMPILE_ON_MINGW)
	KviQString::appendFormatted(szName,"libkvi%Q.dll",&modName);
#else
	KviQString::appendFormatted(szName,"libkvi%Q.so",&modName);
#endif
	szName=szName.toLower();

	g_pApp->getLocalKvircDirectory(tmp,KviApplication::Modules,szName);
	if(!KviFileUtils::fileExists(tmp))
	{
		g_pApp->getGlobalKvircDirectory(tmp,KviApplication::Modules,szName);
	}

	QLibrary* pLibrary = new QLibrary(tmp);
	pLibrary->setLoadHints(QLibrary::ExportExternalSymbolsHint);

	if(!pLibrary->load())
	{
		m_szLastError = pLibrary->errorString();
		delete pLibrary;
		return false;
	}
	KviModuleInfo * info = (KviModuleInfo *)pLibrary->resolve(KVIRC_MODULE_STRUCTURE_SYMBOL);
	if(!info)
	{
		m_szLastError = __tr2qs("No " KVIRC_MODULE_STRUCTURE_SYMBOL " symbol exported: not a kvirc module ?");
		pLibrary->unload();
		delete pLibrary;
		return false;
	}
	if(!info->szKVIrcVersion)
	{
		m_szLastError = __tr2qs("This module has no version information: refusing to load it");
		pLibrary->unload();
		delete pLibrary;
		return false;
	}
	if(!KVI_OPTION_BOOL(KviOption_boolIgnoreModuleVersions))
	{
		if(!kvi_strEqualCS(info->szKVIrcVersion,KVI_VERSION))
		{
			m_szLastError = __tr2qs("This module was compiled for a different KVIrc version and can't be loaded");
			m_szLastError += " (";
			m_szLastError += info->szKVIrcVersion;
			m_szLastError += ")";
			pLibrary->unload();
			delete pLibrary;
			return false;
		}
	}
	KviModule * module = new KviModule(pLibrary,info,modName,szName.toUtf8().data());

	// the module is probably up.. the only thing can fail is the init_routine now
	
	// load the message catalogue if any
	if(info->szModuleContext)
	{
		QString szDir;
		// it's more probable to have the translations in the global directory
		// try it as first... (yes, catalogue overriding is impossible this way.. but, anybody cares ?)
		g_pApp->getGlobalKvircDirectory(szDir,KviApplication::Locale);

		if(!KviLocale::instance()->loadCatalogue(info->szModuleContext,szDir))
		{
			// try the local directory then
			g_pApp->getLocalKvircDirectory(szDir,KviApplication::Locale);
			KviLocale::instance()->loadCatalogue(info->szModuleContext,szDir);
		}
	}
	
	if(info->init_routine)
	{
		if(!((info->init_routine)(module)))
		{
			m_szLastError = __tr2qs("Failed to execute the init routine");
			//qDebug("ERROR IN LOADING MODULE %s (%s): failed to execute the init routine",modName,szName.ptr());
			delete module;
			// kill the message catalogue too then
			KviLocale::instance()->unloadCatalogue(modName);
			return false;
		}
	}
	m_pModuleDict->insert(modName,module);

	/*
	registerDefaultCommands(module);
	*/
	module->registerDefaultCommands();

	if(KVI_OPTION_BOOL(KviOption_boolCleanupUnusedModules))
	{
		if(!m_pCleanupTimer->isActive())
		{
			if(KVI_OPTION_UINT(KviOption_uintModuleCleanupTimerInterval) < 30)
				KVI_OPTION_UINT(KviOption_uintModuleCleanupTimerInterval) = 30;
			m_pCleanupTimer->start(KVI_OPTION_UINT(KviOption_uintModuleCleanupTimerInterval) * 1000);
		}
	}
	// be verbose if needed....just make sure that we're not shutting down...
	if(_OUTPUT_VERBOSE && !g_pApp->kviClosingDown())
	{
		if(g_pMainWindow)
		{
			KviConsoleWindow * pWnd = g_pMainWindow->firstConsole();
			if(pWnd) // this may be NULL when the app is starting up
				pWnd->output(
						KVI_OUT_VERBOSE,
						__tr2qs("Loaded module '%s' (%s)"),
						modName.toUtf8().data(),
						szName.toUtf8().data()
					);
		}
	}
	return true;
}

bool KviModuleManager::unloadModule(const QString &modName)
{
	return unloadModule(findModule(modName));
}

bool KviModuleManager::unloadModule(KviModule * module)
{
	if(!module)return false;
	moduleAboutToUnload(module);

	if(module->moduleInfo()->cleanup_routine)
	{
		(module->moduleInfo()->cleanup_routine)(module);
	}
	QString szModName = module->name();
	module->handle()->unload();
	//qDebug("Closing module %s, dlclose returns %d",szModName.ptr(),dlclose(module->handle()));

	m_pModuleDict->remove(szModName);
	delete module;

	// unload the message catalogues, if any
	KviLocale::instance()->unloadCatalogue(szModName);

	if(m_pModuleDict->isEmpty())
	{
		if(m_pCleanupTimer->isActive())m_pCleanupTimer->stop();
	}

	if(_OUTPUT_VERBOSE && !g_pApp->kviClosingDown())
	{
		if(g_pMainWindow)g_pMainWindow->firstConsole()->output(KVI_OUT_VERBOSE,
			__tr2qs("Unloaded module '%s'"),szModName.toUtf8().data());
	}
	return true;
}

bool KviModuleManager::hasLockedModules()
{
	KviPointerHashTableIterator<QString,KviModule> it(*m_pModuleDict);
	while(KviModule * m = it.current())
	{
		if(m->isLocked())return true;
		++it;
	}
	return false;
}


void KviModuleManager::cleanupUnusedModules()
{
	KviPointerHashTableIterator<QString,KviModule> it(*m_pModuleDict);

	KviPointerList<KviModule> lModulesToUnload;
	lModulesToUnload.setAutoDelete(false);

	while(it.current())
	{
		if(it.current()->secondsSinceLastAccess() > KVI_OPTION_UINT(KviOption_uintModuleCleanupTimeout))
		{
			if(it.current()->moduleInfo()->can_unload)
			{
				if((it.current()->moduleInfo()->can_unload)(it.current()))
					lModulesToUnload.append(it.current());
				else {
					// the module don't want to be unloaded
					// keep it memory for a while
					it.current()->updateAccessTime();
				}
			} else {
				if(!(it.current()->isLocked()))
					lModulesToUnload.append(it.current());
			}
		}
		++it;
	}

	for(KviModule * pModule = lModulesToUnload.first();pModule;pModule = lModulesToUnload.next())
		unloadModule(pModule);
}

void KviModuleManager::unloadAllModules()
{
	KviPointerHashTableIterator<QString,KviModule> it(*m_pModuleDict);

	KviPointerList<KviModule> lModulesToUnload;
	lModulesToUnload.setAutoDelete(false);
	while(KviModule * pModule = it.current())
	{
		lModulesToUnload.append(pModule);
		++it;
	}

	for(KviModule * pModule = lModulesToUnload.first();pModule;pModule = lModulesToUnload.next())
		unloadModule(pModule);
}
