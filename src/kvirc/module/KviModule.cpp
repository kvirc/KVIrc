//=============================================================================
//
//   File : KviModule.cpp
//   Creation date : Sat Aug 12 2000 20:30:29 by Szymon Stefanek
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

#include "KviModule.h"
#include "KviApplication.h"
#include "kvi_settings.h"
#include "KviKvsEventManager.h"
#include "KviKvsEventHandler.h"
#include "KviModuleManager.h"
#include "KviCString.h"
#include "KviPointerHashTable.h"

#include <QLibrary>

#include <ctime>

#ifdef COMPILE_CRYPT_SUPPORT
#include "KviCryptEngine.h"
#include "KviCryptEngineDescription.h"
#include "KviCryptEngineManager.h"

extern KviCryptEngineManager * g_pCryptEngineManager;
#endif

extern KVIRC_API KviModuleExtensionManager * g_pModuleExtensionManager;

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
		Starting from version 2.0.0, KVIrc includes support for external plugins:
		parts of code that can be loaded at runtime. This support was optional
		and the main KVIrc functionality was independent of that support.
		In version 3.0.0 the [i]modularity[/i] became one of the primary KVIrc features.
		These pieces of external code are now named [i]modules[/i]. The module
		support has been rewritten completely to extend the available features
		and remove some [i]basic[/i] problems that were present in 2.0.0.
		This caused the old [i]plugins[/i] to be binary incompatible with the new KVIrc
		executable; anyway, most old [i]plugins[/i] have been rewritten as [i]modules[/i]
		and included in the 3.0.0 distribution. Some of the basic KVIrc features
		are based on modules now; for example, the help browser is now in an
		external library called [i]libkvihelp.so[/i]. This allows to keep the basic
		KVIrc executable smaller and faster (in some phases).
		Module loading is now [i]transparent[/i] to the user. There are no
		[i]load[/i] and [i]unload[/i] commands: the modules are automatically
		loaded when the [i]core[/i] requests them and unloaded after some (configurable) time
		of inactivity.
		[big]Module naming convention[/big]
		Every KVIrc module must have an [i]unique[/i] name: the name is a single token,
		made only of [b]lowercase[/b] letters, digits and underscores.
		The real object file (library) is named [i]libkvi<name>.so[/i].
		This convention allows KVIrc to load modules when they are referenced
		by name in the scripting code (the reference in the code is case insensitive
		and remapped always to the lowercase version).
		[big]What a module can do[/big]
		Basically, a module exports parts of the scripting language features.
		For example, the module [i]about[/i] exports the [i]about.kvirc[/i] command, that
		shows the dialog that lists the KVIrc staff people. The dialog
		will be effectively activated only few times (maybe only once) by a single
		user: including it in a separate module allows keeping rarely used code out
		of the KVIrc executable and saving some memory.
		To activate the dialog you only need to execute the command:
		[example]
			about.kvirc
		[/example]
		That's all. KVIrc will load the necessary module, run the [i]/about.kvirc[/i] command,
		wait until the module is not used anymore and then unload it.
		[big]Transparent loading and unloading[/big]
		Any command that has the form <name>.<command> is assumed to be
		a module reference. KVIrc tries to locate the module named <name>, load it (if
		not already in memory) and execute the <command>.
		After the command execution KVIrc waits some user defined interval of time
		(typically 2-3 minutes) and then check the module state: if the module
		has not been referenced again, it is unloaded, otherwise it is kept in memory
		for another period of time. To prevent accidentally unloading a module
		that has some windows or dialogs open, a module can lock itself in memory.
		For example, the 'help' module lock itself when a help browser window is open
		and unlocks itself when the last help browser window is closed.[br]
		A module can export functions as well: the function names follow
		exactly the same rule as the commands: $<name>.<function> is assumed
		to be a reference to the <function> exported by module named <name>.
		[big]Forced loading and unloading[/big]
		All the modules export a [i]load[/i] and a [i]unload[/i] command.
		[example]
			about.load
		[/example]
		The example above will do nothing more than loading the [i]about[/i]
		module into the core memory.
		[example]
			about.unload
		[/example]
		The example above will forcibly unload the module from the core memory;
		even if it is still locked.[br]
		Please note that this can be dangerous in some situations... so
		better check the KVIrc behaviour twice before making public any script
		that uses this command.
*/

// FIXME: #warning "Move all the modules to the new locking method ?"

KviModule::KviModule(QLibrary * handle, KviModuleInfo * info, const QString & name, const QString & filename)
    : KviKvsModuleInterface()
{
	m_pLibrary = handle;
	m_pModuleInfo = info;
	m_szName = name;
	m_szFileName = filename;
	m_uLock = 0;
	m_lastAccessTime = (long int)time(nullptr);
}

KviModule::~KviModule()
{
#ifdef COMPILE_CRYPT_SUPPORT
	unregisterCryptEngines();
#endif
	unregisterAllExtensions();

	if(m_pLibrary->isLoaded())
		m_pLibrary->unload();
	delete m_pLibrary;
}

KviModuleExtensionDescriptor * KviModule::registerExtension(const KviCString & szType, const KviCString & szName, const QString & szVisibleName, KviModuleExtensionAllocRoutine r)
{
	QPixmap pix; // null
	return g_pModuleExtensionManager->registerExtension(this, szType, szName, szVisibleName, r, pix);
}

KviModuleExtensionDescriptor * KviModule::registerExtension(const KviCString & szType, const KviCString & szName, const QString & szVisibleName, KviModuleExtensionAllocRoutine r, const QPixmap & icon)
{
	return g_pModuleExtensionManager->registerExtension(this, szType, szName, szVisibleName, r, icon);
}

KviModuleExtensionDescriptor * KviModule::findExtensionDescriptor(const KviCString & szType, const KviCString & szName)
{
	return g_pModuleExtensionManager->findExtensionDescriptor(szType, szName);
}

void KviModule::unregisterAllExtensions()
{
	g_pModuleExtensionManager->unregisterExtensionsByModule(this);
}

void KviModule::updateAccessTime()
{
	m_lastAccessTime = (long int)time(nullptr);
}

unsigned int KviModule::secondsSinceLastAccess()
{
	return (unsigned int)(((long int)time(nullptr)) - m_lastAccessTime);
}

#ifdef COMPILE_CRYPT_SUPPORT

void KviModule::registerCryptEngine(KviCryptEngineDescription * d)
{
	d->m_providerHandle = (void *)this;
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
	return (void *)m_pLibrary->resolve(symname);
}

void KviModule::getDefaultConfigFileName(QString & szBuffer)
{
	QString tmp = "libkvi";
	tmp += m_szName;
	tmp += ".kvc";
	g_pApp->getLocalKvircDirectory(szBuffer, KviApplication::ConfigPlugins, tmp);
}

bool KviModule::ctrl(const char * operation, void * param)
{
	if(!(m_pModuleInfo->ctrl_routine))
		return false;
	return m_pModuleInfo->ctrl_routine(this, operation, param);
}
