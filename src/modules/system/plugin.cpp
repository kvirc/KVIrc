//=============================================================================
//
//   File : plugin.cpp
//   Creation date : Wed Apr 11 04 2007 00:54:00 GMT+1 by TheXception
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)
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

#include "plugin.h"

#include "kvi_module.h"
#include "kvi_string.h"
#include "kvi_library.h"
#include "kvi_thread.h"
#include "kvi_locale.h"
#include "kvi_app.h"
#include "kvi_fileutils.h"

#include <qdir.h>
#include <qfileinfo.h>
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
		TODO
		[b]Exported functions by dll[/b][br]
		[br]_free function (needed)[br]
		[example]
		int _free(void * p)[br]
		{[br]
			// Always free the memory here![br]
			free(p);[br]
			return 0;[br]
		}[br]
		[/example]
		
		[br]_load function (optional)[br]
		[example]
		int _load()[br]
		{[br]
			return 0;[br]
		}[br]
		[/example]		
		
		[br]_unload function (optional)[br]
		[example]
		int _unload()[br]
		{[br]
			return 0;[br]
		}[br]
		[/example]	
		
		[br]user function[br]
		[example]	
		int about(int argc, char * argv[], char ** pBuffer)[br]
		{[br]
		    *pBuffer = (char*)malloc(1024);[br]
		    sprintf((char*)*pBuffer, "Hello World"); [br]
		    return 1;[br]
		}[br]
		[/example]
*/

KviPlugin::KviPlugin(kvi_library_t pLib, const QString& name)
{
	m_Plugin = pLib;
	m_szName = name;
}

KviPlugin::~KviPlugin()
{
}

KviPlugin* KviPlugin::load(const QString& szFileName)
{
	kvi_library_t pLibrary = kvi_library_open(szFileName.local8Bit());
	if (!pLibrary)
	{
		return 0;
	} 

	KviPlugin* pPlugin = new KviPlugin(pLibrary,KviFileUtils::extractFileName(szFileName));
	
	plugin_load function_load;
	
	function_load = (plugin_unload)kvi_library_symbol(pLibrary,"_load");
	if (function_load)
	{
		//TODO: THREAD
		function_load();
	}
	return pPlugin;
}

bool KviPlugin::pfree(char * pBuffer)
{
	plugin_free function_free;
	
	function_free = (plugin_free)kvi_library_symbol(m_Plugin,"_free");
	if (function_free)
	{
		//TODO: THREAD
		if(pBuffer) function_free(pBuffer);
		return true;
	}
	return false;
}

bool KviPlugin::unload(bool forced)
{
	plugin_unload function_unload;
	
	function_unload = (plugin_unload)kvi_library_symbol(m_Plugin,"_unload");
	if (function_unload)
	{
		//TODO: THREAD
		if(!function_unload()) 
		{
			if(!forced) return false;
		}
	}
	
	if(m_Plugin)
	{
		kvi_library_close(m_Plugin);
	}
	
	return true;
}

int KviPlugin::call(const QString& pszFunctionName, int argc, char * argv[], char ** pBuffer)
{
	int r;
	plugin_function function_call;
	function_call = (plugin_function)kvi_library_symbol(m_Plugin,pszFunctionName.local8Bit());
	if (!function_call)
	{
		return -1;
	} else {
		//TODO: THREAD
		r = function_call(argc,argv,pBuffer);
	}
	if (r < 0) r = 0; // negative numbers are for error handling.
	return r;
}

QString KviPlugin::name()
{
	return m_szName;
}

void KviPlugin::setName(const QString& Name)
{
	m_szName = Name;
}


KviPluginManager::KviPluginManager()
{
	m_pPluginDict = new KviDict<KviPlugin>(5,false);
	m_pPluginDict->setAutoDelete(false);
	
	m_bCanUnload = true;
}

KviPluginManager::~KviPluginManager()
{
	delete 	m_pPluginDict;
}

bool KviPluginManager::pluginCall(KviKvsModuleFunctionCall *c)
{
	QString szPluginPath; //contains full path and plugin name like "c:/plugin.dll"
	QString szFunctionName;
	
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("plugin_path",KVS_PT_NONEMPTYSTRING,0,szPluginPath)
		KVSM_PARAMETER("function",KVS_PT_NONEMPTYSTRING,0,szFunctionName)
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


		
	if (iArgc > 0)
	{	
		int i = 2;
		QString tmp;
		int iSize = 0;
		
		//Calculate buffer size
		while (i < (iArgc + 2) )
		{
			c->params()->at(i)->asString(tmp);
			iSize += tmp.length()+1; //+1 for the \0 characters
			i++;
		}
		
		//Allocate buffer
		ppArgv = (char**)malloc(iArgc*sizeof(char*));
		pArgvBuffer = (char*)malloc(iSize);
		
		i = 2;
		char * x = 0;
		x = pArgvBuffer;
		while (i < (iArgc + 2) )
		{
			ppArgv[i-2] = x;
			c->params()->at(i)->asString(tmp);
			strcpy(x,tmp.local8Bit());
			x += tmp.length();

			*x = 0;
			x++;
			i++;
		}	

	} else {
		//Avoid using unfilled variables
		ppArgv = 0;
		iArgc = 0;
	}
	
	//Preparing return buffer
	char * returnBuffer;
	
	KviPlugin * plugin;
	
	plugin = getPlugin(szPluginPath);
	int r = plugin->call(szFunctionName,iArgc,ppArgv,&returnBuffer);
	
	if(r == -1)
	{
		c->error(__tr2qs("This plugin does not export the desired function."));
		return true;
	}
	if (r > 0)
	{
		c->returnValue()->setString(returnBuffer);
	}
	
	//Clean up
	free(pArgvBuffer);
	free(ppArgv);
	if (!plugin->pfree(returnBuffer))
	{
		c->warning(__tr2qs("The plugin has no function to free memory. Can result in Memory Leaks!"));
	}
	return true;
}

bool KviPluginManager::checkUnload()
{
	/* 	
	Always called when system module should be unloaded
	Checking here if all small "modules" can be unloaded	
	*/
	KviDictIterator<KviPlugin> it(*m_pPluginDict);
	
	m_bCanUnload = true;
	
	while(it.current())
	{
		if(it.current()->unload(false))
		{
			m_pPluginDict->remove(it.currentKey());
		} else {
			m_bCanUnload = false;
		}
	}
	
	return m_bCanUnload;
}

void KviPluginManager::unloadAll(bool forced)
{
	KviDictIterator<KviPlugin> it(*m_pPluginDict);
	
	while(it.current())
	{
		it.current()->unload(true);
		m_pPluginDict->remove(it.currentKey());
	}
}

bool KviPluginManager::findPlugin(QString& szPath)
{
	QString szFileName=KviFileUtils::extractFileName(szPath);
	if(KviFileUtils::isAbsolutePath(szPath) && KviFileUtils::fileExists(szPath))
	{
		// Ok, 
		return true;
	} else {
		//Plugin not found in direct way. Looking in kvirc local dir
		g_pApp->getGlobalKvircDirectory(szPath,KviApp::EasyPlugins,szFileName);
		
		if(!KviFileUtils::fileExists(szPath))
		{
			//Plugin not found in kvirc local dir. Looking in kvirc global dir
			g_pApp->getLocalKvircDirectory(szPath,KviApp::EasyPlugins,szFileName);
			
			if(!KviFileUtils::fileExists(szPath))
			{
				return false;
			}
		}
	}
	return true;
}

bool KviPluginManager::isPluginLoaded(const QString& pSingleName)
{
	KviPlugin * p = m_pPluginDict->find(pSingleName);
	if (!p)
		return false;
	else
		return true;
}

bool KviPluginManager::loadPlugin(const QString& szPluginPath)
{
	if(isPluginLoaded(szPluginPath))
	{
		return getPlugin(szPluginPath)!=0;
	} else {
		KviPlugin * plugin = KviPlugin::load(szPluginPath);
		if(plugin)
		{
			m_pPluginDict->insert(szPluginPath,plugin);
			return true;
		}
	}
	return false;
}

KviPlugin * KviPluginManager::getPlugin(const QString& szPluginPath)
{
	KviPlugin * p = m_pPluginDict->find(szPluginPath);
	return p;
}