//=============================================================================
//
//   File : kvi_module.cpp
//   Creation date : Sat Aug 12 2000 20:30:29 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2004 Szymon Stefanek (pragma at kvirc dot net)
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

#define __KVIRC__

#include "kvi_module.h"
#include "kvi_app.h"
#include "kvi_settings.h"
#include "kvi_kvs_eventmanager.h"
#include "kvi_kvs_eventhandler.h"
#include "kvi_modulemanager.h"

#include <time.h>

#ifdef COMPILE_CRYPT_SUPPORT
	extern KviCryptEngineManager * g_pCryptEngineManager;
#endif

extern KVIRC_API KviModuleExtensionManager    * g_pModuleExtensionManager;

/*
	@doc: modules
	@type:
		generic
	@keyterms:
		module loading, functions exported by modules, commands exported by modules,
		module naming convention, module locking, plugins, functions exported by modules,
		dots in command names,dots in function names
	@title:
		Loadable modules
	@short:
		KVIrc modules model
	@body:
		Starting from version 2.0.0, KVIrc included support for external plugins:
		parts of code that can be loaded at runtime. The support was optional
		and the main KVIrc functionality was indipendant of that support.
		In version 3.0.0 the "modularity" has becomed one of the primary KVIrc features.
		The pieces of external code are now named "modules". The module
		support has been rewritten completely to extend the available features
		and remove some "basic" problems that were present in 2.0.0.
		This caused the old "plugins" to be binary incompatible with the new KVIrc
		executable; anyway, most old "plugins" have been rewritten as "modules"
		and included in the 3.0.0 distribution. Some of the basic KVIrc features
		are based on modules now; for example, the help browser is now in an
		external library called libkvihelp.so. This allows to keep the basic
		KVIrc executable smaller and faster (in some phases).
		Module loading is now "transparent" to the user. There are no
		"load" and "unload" commands: the modules are automatically
		loaded when the "core" requests them and unloaded after some (configurable) time
		of inactivity.[br]

		[big]Module naming convention[/big][br]
		Every KVIrc module must have an "unique" name: the name is a single token,
		made only of [b]lowercase[/b] letters , digits and underscores.
		The real object file (library) is named "libkvi<name>.so".
		This convention allows KVIrc to load modules when they are referenced
		by name in the scripting code (the reference in the code is case insensitive
		and remapped always to the lowercase version).[br]
		[big]What a module can do[/big][br]
		Basically, a module exports parts of the scripting language features.
		For example, the module "about" exports the "aboutkvirc" command, that
		shows the dialog that lists the KVIrc staff people. The dialog
		will be effectively activated only few times (maybe only once) by a single
		user: including it in a separate module allows keeping rarely used code out
		of the KVIrc executable and saving some memory.
		To activate the dialog you only need to execute the command:[br]
		[example]
		about.aboutkvirc
		[/example]
		That's all. KVIrc will load the necessary module, run the "aboutkvirc" command,
		wait until the module is not used anymore and then unload it.[br]

		[big]Transparent loading and unloading[/big][br]
		Any command that has the form <name>.<command> is assumed to be
		a module reference. KVIrc tries to locate the module named <name>, load it (if
		not already in memory) and execute the <command>.
		After the command execution KVIrc waits some user defined interval of time
		(typically 2-3 minutes) and then check the module state: if the module
		has not been referenced again, it is unloaded, otherwise it is kept in memory
		for another period of time. To prevent accidentaly unloading a module
		that has some windows or dialogs open, a module can lock itself in memory.
		For example, the 'help' module lock itself when a help browser window is open
		and unlocks itself when the last help browser window is closed.[br]
		A module can export functions as well: the function names follow
		exactly the same rule as the commands: $<name>.<function> is assumed
		to be a reference to the <function> exported by module named <name>.

		[big]Forced loading and unloading[/big]
		All the modules export a "load" and a "unload" command.
		[example]
		about.load
		[/example]
		The example above will do nothing more than loading the "about"
		module into the core memory.
		[example]
		about.unload
		[/example]
		The example above will forcibly unload the module from the core memory;
		even if it is still locked.[br]
		Please note that this can be dangerous in some situations...so
		better check the KVIrc behaviour twice before making public any script
		that uses this command.
*/



// FIXME: #warning "Move all the modules to the new locking method ?"


KviModule::KviModule(kvi_library_t handle,KviModuleInfo * info,const QString &name,const QString &filename)
: KviKvsModuleInterface()
{
	m_dlHandle = handle;
	m_pModuleInfo = info;
	m_szName = name;
	m_szFileName = filename;
// FIXME: this should become case insensitive and converted toUpper()
	/*
	m_pCommandDict = new KviPointerHashTable<const char *,KviModuleCommandParseProc>(17,false,true);
	m_pCommandDict->setAutoDelete(true);
	m_pFunctionDict = new KviPointerHashTable<const char *,KviModuleFunctionParseProc>(17,false,true);
	m_pFunctionDict->setAutoDelete(true);
	*/
	m_uLock = 0;
	m_lastAccessTime = (long int)time(0);
	/*
	m_pGenericCommandParseProc = 0;
	m_pGenericFunctionParseProc = 0;
	*/
}

KviModule::~KviModule()
{
#ifdef COMPILE_CRYPT_SUPPORT
	unregisterCryptEngines();
#endif
	unregisterAllExtensions();
	/*
	unregisterAllEventHandlers();
	delete m_pCommandDict;
	delete m_pFunctionDict;
	if(m_pGenericCommandParseProc)delete m_pGenericCommandParseProc;
	if(m_pGenericFunctionParseProc)delete m_pGenericFunctionParseProc;
	*/
}

KviModuleExtensionDescriptor * KviModule::registerExtension(const KviStr &szType,const KviStr &szName,const QString &szVisibleName,KviModuleExtensionAllocRoutine r)
{
	QPixmap pix; // null
	return g_pModuleExtensionManager->registerExtension(this,szType,szName,szVisibleName,r,pix);
}

KviModuleExtensionDescriptor * KviModule::registerExtension(const KviStr &szType,const KviStr &szName,const QString &szVisibleName,KviModuleExtensionAllocRoutine r,const QPixmap &icon)
{
	return g_pModuleExtensionManager->registerExtension(this,szType,szName,szVisibleName,r,icon);
}

KviModuleExtensionDescriptor * KviModule::findExtensionDescriptor(const KviStr &szType,const KviStr &szName)
{
	return g_pModuleExtensionManager->findExtensionDescriptor(szType,szName);
}

void KviModule::unregisterAllExtensions()
{
	g_pModuleExtensionManager->unregisterExtensionsByModule(this);
}
/*
void KviModule::setGenericCommandParseProc(KviModuleCommandParseProc proc)
{
	if(m_pGenericCommandParseProc)delete m_pGenericCommandParseProc;
	if(proc)
	{
		m_pGenericCommandParseProc = new KviModuleCommandParseProc(proc);
	} else {
		m_pGenericCommandParseProc = 0;
	}
}

void KviModule::setGenericFunctionParseProc(KviModuleFunctionParseProc proc)
{
	if(m_pGenericFunctionParseProc)delete m_pGenericFunctionParseProc;
	if(proc)
	{
		m_pGenericFunctionParseProc = new KviModuleFunctionParseProc(proc);
	} else {
		m_pGenericFunctionParseProc = 0;
	}
}

void KviModule::completeCommand(const QString &cmd,KviPointerList<QString> * matches)
{
	KviPointerHashTableIterator<const char *,KviModuleCommandParseProc> it(*m_pCommandDict);

	while(it.current())
	{
		if(KviQString::equalCIN(cmd,it.currentKey(),cmd.length()))
		{
			QString * s = new QString();
			KviQString::sprintf(*s,"%s.%s",name(),it.currentKey());
			matches->append(s);
		}
		++it;
	}
}

void KviModule::completeFunction(const QString &cmd,KviPointerList<QString> * matches)
{
	KviPointerHashTableIterator<const char *,KviModuleFunctionParseProc> it(*m_pFunctionDict);

	while(it.current())
	{
		if(KviQString::equalCIN(cmd,it.currentKey(),cmd.length()))
		{
			QString * s = new QString();
			KviQString::sprintf(*s,"%s.%s",name(),it.currentKey());
			matches->append(s);
		}
		++it;
	}
}


void KviModule::unregisterMetaObject(const char * metaObjName)
{
#if QT_VERSION < 300
// FIXME: 	#warning "We might need zeroing the d->slotAccess member of QMetaObject!"
	if(!objectDict)return;
	objectDict->remove(metaObjName);
#endif
}
*/
void KviModule::updateAccessTime()
{
	m_lastAccessTime = (long int)time(0);
}

unsigned int KviModule::secondsSinceLastAccess()
{
	return (unsigned int)(((long int)time(0)) - m_lastAccessTime);
}
/*
void KviModule::registerCommand(const char * cmd,KviModuleCommandParseProc proc)
{
	if(m_pCommandDict->find(cmd))m_pCommandDict->remove(cmd);
	m_pCommandDict->insert(cmd,new KviModuleCommandParseProc(proc));
}

void KviModule::unregisterCommand(const char * cmd)
{
	m_pCommandDict->remove(cmd);
}

void KviModule::unregisterAllCommands()
{
	delete m_pCommandDict;
	m_pCommandDict = new KviPointerHashTable<const char *,KviModuleCommandParseProc>(17,false,true);
	m_pCommandDict->setAutoDelete(true);
}

void KviModule::registerEventHandler(int evIdx,KviModuleEventParseProc proc)
{
	KviKvsOldModuleEventHandler * h = new KviKvsOldModuleEventHandler(proc,this);
	KviKvsEventManager::instance()->addAppHandler(evIdx,h);
}

void KviModule::unregisterEventHandler(int evIdx)
{
	KviKvsEventManager::instance()->removeModuleAppHandler(evIdx,this);
}

void KviModule::registerRawNumericEventHandler(int evIdx,KviModuleEventParseProc proc)
{
	KviKvsOldModuleEventHandler * h = new KviKvsOldModuleEventHandler(proc,this);
	KviKvsEventManager::instance()->addRawHandler(evIdx,h);
}

void KviModule::unregisterRawNumericEventHandler(int evIdx)
{
	KviKvsEventManager::instance()->removeModuleRawHandler(evIdx,this);
}


void KviModule::unregisterAllEventHandlers()
{
	KviKvsEventManager::instance()->removeAllModuleHandlers(this);
}

void KviModule::registerFunction(const char * fnc,KviModuleFunctionParseProc proc)
{
	if(m_pFunctionDict->find(fnc))m_pFunctionDict->remove(fnc);
	m_pFunctionDict->insert(fnc,new KviModuleFunctionParseProc(proc));
}

void KviModule::unregisterFunction(const char * fnc)
{
	m_pFunctionDict->remove(fnc);
}

void KviModule::unregisterAllFunctions()
{
	delete m_pFunctionDict;
	m_pFunctionDict = new KviPointerHashTable<const char *,KviModuleFunctionParseProc>(17,false,true);
	m_pFunctionDict->setAutoDelete(true);
}
*/

#ifdef COMPILE_CRYPT_SUPPORT

void KviModule::registerCryptEngine(KviCryptEngineDescription * d)
{
	d->providerHandle = (void *)this;
	g_pCryptEngineManager->registerEngine(d);
}

void KviModule::unregisterCryptEngine(const char * szName)
{
	g_pCryptEngineManager->unregisterEngine(szName);
}

void KviModule::unregisterCryptEngines()
{
	g_pCryptEngineManager->unregisterEngines((void *)this);
}

#endif

void * KviModule::getSymbol(const char * symname)
{
	return kvi_library_symbol(handle(),symname);
}

void KviModule::getDefaultConfigFileName(KviStr &buffer)
{
	QString tmp = "libkvi";
	tmp += m_szName;
	tmp += ".kvc";
	g_pApp->getLocalKvircDirectory(buffer,KviApp::ConfigPlugins,tmp);
}

void KviModule::getDefaultConfigFileName(QString &szBuffer)
{
	QString tmp = "libkvi";
	tmp += m_szName;
	tmp += ".kvc";
	g_pApp->getLocalKvircDirectory(szBuffer,KviApp::ConfigPlugins,tmp);
}

bool KviModule::ctrl(const char * operation,void * param)
{
	if(!(m_pModuleInfo->ctrl_routine))return false;
	return m_pModuleInfo->ctrl_routine(this,operation,param);
}
