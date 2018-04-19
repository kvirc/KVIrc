//=============================================================================
//
//   File : Plugin.cpp
//   Creation date : Wed Apr 11 04 2007 00:54:00 GMT+1 by TheXception
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2007-2008 TheXception
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

#include "Plugin.h"

#include "KviModule.h"
#include "KviCString.h"
#include "KviThread.h"
#include "KviLocale.h"
#include "KviApplication.h"
#include "KviFileUtils.h"

#include <QDir>
#include <QFileInfo>

/*
	@doc: easyplugins
	@type:
		generic
	@keyterms:
		easyplugins
	@title:
		Easyplugins
	@short:
		Small plugins which can be called in scripts
	@body:
		If you want to know how to call easyplugins please have a look at: $system.call()[br]
		This part of the documentation handles only the way how to write an easyplugin. An easyplugin is simply a dll/so. You can create one like you normally make such so/dll files.[br]
		The important thing is that these so/dll-files export some of the following functions.
		[br][br]
		[b]Exported functions by easyplugin (C/C++-Examples):[/b][br]
		[b]_free function[/b] [i] (needed)[/i][br][br]
		This function is important! Since KVIrc can not free directly the memory of the dll, the plugins need the _free function so that the memory can be freed by the plugin to prevent memory-leaks.
		[example]
			int _free(void * p)
			{
				[comment]// Always free the memory here![/comment]
				free(p);
				return 0;
			}
		[/example]
		[b]_load function[/b] [i](optional)[/i][br][br]
		After the plugin has be loaded, KVIrc will call the _load-function. Here you can prepare your plugin stuff.
		[example]
			int _load()
			{
				return 0;
			}
		[/example]
		[b]_unload function[/b] [i]((optional)[/i][br][br]
		This function will be called before the plugins is unloaded. In this function you can clean up memory or other things.
		After this call there is no guarantee that the plugin will be kept in memory.
		[example]
			int _unload()
			{
				return 0;
			}
		[/example]
		[br][b]_canunload function[/b] [i](optional)[/i][br]
		The _canunload-function will be called by KVIrc to check if it may unload the plugin.
		If return value is true KVIrc will unload the plugin, false means he will try unloading it at the next check.[br]
		Important: KVIrc will ignore this if unload of plugins will be forced! So you have to be sure that the _unload function of your plugins cleans up!
		[example]
			int _canunload()
			{
				return 0;
			}
		[/example]
		[br][b]user function[/b][br]
		This is the general structure of a user function call.[br]
		The important thing here is the handling of return values. To return a value to KVIrc you have to allocate memory and write the pointer to it into pBuffer.[br]
		Have a look at the example for more details.
		[example]
			int about(int argc, char * argv[], char ** pBuffer)
			{
				*pBuffer = (char*)malloc(1024);
				sprintf((char*)*pBuffer, "Hello World");
				return 1;
			}
		[/example]
*/

Plugin::Plugin(QLibrary * pLibrary, const QString & name)
{
	m_pLibrary = pLibrary;
	m_szName = name;
}

Plugin::~Plugin()
{
	if(m_pLibrary->isLoaded())
		m_pLibrary->unload();
	delete m_pLibrary;
}

Plugin * Plugin::load(const QString & szFileName)
{
	QLibrary * pLibrary = new QLibrary(szFileName);
	if(!pLibrary->load())
	{
		delete pLibrary;
		return nullptr;
	}

	Plugin * pPlugin = new Plugin(pLibrary, KviFileUtils::extractFileName(szFileName));

	plugin_load function_load;

	function_load = (plugin_unload)pLibrary->resolve("_load");
	if(function_load)
	{
		//TODO: THREAD
		function_load();
	}
	return pPlugin;
}

bool Plugin::pfree(char * pBuffer)
{
	plugin_free function_free;

	function_free = (plugin_free)m_pLibrary->resolve("_free");
	if(function_free)
	{
		//TODO: THREAD
		if(pBuffer)
			function_free(pBuffer);
		return true;
	}
	return false;
}

bool Plugin::unload()
{
	plugin_unload function_unload;

	function_unload = (plugin_unload)m_pLibrary->resolve("_unload");
	if(function_unload)
	{
		//TODO: THREAD
		function_unload();
	}

	m_pLibrary->unload();
	return true;
}

bool Plugin::canunload()
{
	plugin_canunload function_canunload;

	function_canunload = (plugin_canunload)m_pLibrary->resolve("_canunload");
	if(function_canunload)
	{
		//TODO: THREAD
		return function_canunload();
	}
	return true;
}

int Plugin::call(const QString & pszFunctionName, int argc, char * argv[], char ** pBuffer)
{
	int r;
	plugin_function function_call;
	function_call = (plugin_function)m_pLibrary->resolve(pszFunctionName.toUtf8().data());
	if(!function_call)
	{
		return -1;
	}
	else
	{
		//TODO: THREAD
		r = function_call(argc, argv, pBuffer);
	}
	if(r < 0)
		r = 0; // negative numbers are for error handling.
	return r;
}

QString Plugin::name() const
{
	return m_szName;
}

void Plugin::setName(const QString & Name)
{
	m_szName = Name;
}

PluginManager::PluginManager()
{
	m_pPluginDict = new KviPointerHashTable<QString, Plugin>(5, false);
	m_pPluginDict->setAutoDelete(false);

	m_bCanUnload = true;
}

PluginManager::~PluginManager()
{
	delete m_pPluginDict;
}

bool PluginManager::pluginCall(KviKvsModuleFunctionCall * c)
{
	//   /echo $system.call("traffic.dll",about)
	QString szPluginPath; //contains full path and plugin name like "c:/plugin.dll"
	QString szFunctionName;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("plugin_path", KVS_PT_NONEMPTYSTRING, 0, szPluginPath)
	KVSM_PARAMETER("function", KVS_PT_NONEMPTYSTRING, 0, szFunctionName)
	KVSM_PARAMETERS_END(c)

	//Check if there is such a plugin
	if(!findPlugin(szPluginPath))
	{
		c->error(__tr2qs("Plugin not found. Please check the plugin-name and path."));
		return true;
	}

	//Load plugin or check it in cache
	if(!loadPlugin(szPluginPath))
	{
		c->error(__tr2qs("Error while loading plugin."));
		return true;
	}

	//Parsing more Parameters
	int iArgc = 0;
	char ** ppArgv;
	char * pArgvBuffer;

	//Preparing argv buffer
	if(c->parameterCount() > 2)
	{
		iArgc = c->parameterCount() - 2;
	}

	if(iArgc > 0)
	{
		int i = 2;
		QString tmp;
		int iSize = 0;

		//Calculate buffer size
		while(i < (iArgc + 2))
		{
			c->params()->at(i)->asString(tmp);
			iSize += tmp.length() + 1; //+1 for the \0 characters
			i++;
		}

		//Allocate buffer
		ppArgv = (char **)malloc(iArgc * sizeof(char *));
		pArgvBuffer = (char *)malloc(iSize);

		i = 2;
		char * x = nullptr;
		x = pArgvBuffer;
		while(i < (iArgc + 2))
		{
			ppArgv[i - 2] = x;
			c->params()->at(i)->asString(tmp);
			strcpy(x, tmp.toLocal8Bit());
			x += tmp.length();

			*x = 0;
			x++;
			i++;
		}
	}
	else
	{
		//Avoid using unfilled variables
		ppArgv = nullptr;
		pArgvBuffer = nullptr;
		iArgc = 0;
	}

	//Preparing return buffer
	char * returnBuffer;
	Plugin * plugin;

	plugin = getPlugin(szPluginPath);
	int r = plugin->call(szFunctionName, iArgc, ppArgv, &returnBuffer);

	if(r == -1)
	{
		c->error(__tr2qs("This plugin doesn't export the desired function."));
		return true;
	}
	if(r > 0)
	{
		c->returnValue()->setString(QString::fromLocal8Bit(returnBuffer));
	}

	//Clean up
	if(pArgvBuffer)
		free(pArgvBuffer);
	if(ppArgv)
		free(ppArgv);
	if(returnBuffer)
	{
		if(!plugin->pfree(returnBuffer))
		{
			c->warning(__tr2qs("The plugin has no function to free memory. This can result in memory leaks!"));
		}
	}

	return true;
}

bool PluginManager::checkUnload()
{
	/*
	Always called when system module should be unloaded
	Checking here if all small "modules" can be unloaded
	*/
	KviPointerHashTableIterator<QString, Plugin> it(*m_pPluginDict);

	m_bCanUnload = true;

	while(it.current())
	{
		if(it.current()->canunload())
		{
			it.current()->unload();
			m_pPluginDict->remove(it.currentKey());
		}
		else
		{
			m_bCanUnload = false;
		}
		it.moveNext();
	}

	return m_bCanUnload;
}

void PluginManager::unloadAll()
{
	KviPointerHashTableIterator<QString, Plugin> it(*m_pPluginDict);

	while(it.current())
	{
		it.current()->unload();
		m_pPluginDict->remove(it.currentKey());
		it.moveNext();
	}
}

bool PluginManager::findPlugin(QString & szPath)
{
	QString szFileName(KviFileUtils::extractFileName(szPath));
	//	szFileName.detach();
	if(KviFileUtils::isAbsolutePath(szPath) && KviFileUtils::fileExists(szPath))
	{
		// Ok,
		return true;
	}
	else
	{
		//Plugin not found in direct way. Looking in kvirc local dir
		g_pApp->getGlobalKvircDirectory(szPath, KviApplication::EasyPlugins, szFileName);

		if(!KviFileUtils::fileExists(szPath))
		{
			//Plugin not found in kvirc local dir. Looking in kvirc global dir
			g_pApp->getLocalKvircDirectory(szPath, KviApplication::EasyPlugins, szFileName);

			if(!KviFileUtils::fileExists(szPath))
			{
				return false;
			}
		}
	}
	return true;
}

bool PluginManager::isPluginLoaded(const QString & pSingleName)
{
	Plugin * p = m_pPluginDict->find(pSingleName);
	if(!p)
		return false;
	else
		return true;
}

bool PluginManager::loadPlugin(const QString & szPluginPath)
{
	if(isPluginLoaded(szPluginPath))
	{
		return getPlugin(szPluginPath) != nullptr;
	}
	else
	{
		Plugin * plugin = Plugin::load(szPluginPath);
		if(plugin)
		{
			m_pPluginDict->insert(szPluginPath, plugin);
			return true;
		}
	}
	return false;
}

Plugin * PluginManager::getPlugin(const QString & szPluginPath)
{
	Plugin * p = m_pPluginDict->find(szPluginPath);
	return p;
}
