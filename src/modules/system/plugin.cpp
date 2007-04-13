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

KviPlugin::KviPlugin()
{
}

KviPlugin::~KviPlugin()
{
}

bool KviPlugin::load(QString& pszPluginName)
{
	m_Plugin = kvi_library_open(pszPluginName.local8Bit());
	if (!m_Plugin)
	{
		return false;
	} 
	
	plugin_load function_load;
	
	function_load = (plugin_unload)kvi_library_symbol(m_Plugin,"_load");
	if (function_load)
	{
		//TODO: THREAD
		function_load();
	}
	return true;
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

int KviPlugin::call(QString& pszFunctionName, int argc, char * argv[], char ** pBuffer)
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

KviStr KviPlugin::name()
{
	return m_szName;
}

void KviPlugin::setName(QString& Name)
{
	m_szName = Name;
}


KviPluginManager::KviPluginManager()
{
	m_pPluginDict = new KviAsciiDict<KviPlugin>(17,false);
	m_pPluginDict->setAutoDelete(false);
	
	m_bCanUnload = true;
}

KviPluginManager::~KviPluginManager()
{
	delete 	m_pPluginDict;
}

bool KviPluginManager::pluginCall(KviKvsModuleFunctionCall *c)
{
	QString szPluginName; //contains full path and plugin name like "c:/plugin.dll"
	QString szSinglePluginName; // contains only the name of the plugin like "plugin.dll"
	QString szFunctionName;
	
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("plugin",KVS_PT_NONEMPTYSTRING,0,szPluginName)
		KVSM_PARAMETER("function",KVS_PT_NONEMPTYSTRING,0,szFunctionName)
	KVSM_PARAMETERS_END(c)
	
	//Check if there is such a plugin
	if(!findPlugin(szPluginName, szSinglePluginName))
	{
		c->error(__tr2qs("Plugin not found. Please check the plugin-name and path."));
		return true;
	}
	
	// Check if plugin is already loaded
	if(!pluginIsLoaded(szSinglePluginName))
	{
		//If not loaded try to load now
		if(!loadPlugin(szPluginName,szSinglePluginName))
		{
			c->error(__tr2qs("Error while loading plugin."));
			return true;
		}
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
	
	plugin = getPlugin(szSinglePluginName);
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
	free(returnBuffer);
	return true;
}

bool KviPluginManager::checkUnload()
{
	/* 	
	Always called when system module should be unloaded
	Checking here if all small "modules" can be unloaded	
	*/
	KviAsciiDictIterator<KviPlugin> it(*m_pPluginDict);
	
	m_bCanUnload = true;
	
	while(it.current())
	{
		if(it.current()->unload(false))
		{
			m_pPluginDict->remove(it.current()->name());
		} else {
			m_bCanUnload = false;
		}
	}
	
	return m_bCanUnload;
}

void KviPluginManager::unloadAll(bool forced)
{
	KviAsciiDictIterator<KviPlugin> it(*m_pPluginDict);
	
	while(it.current())
	{
		it.current()->unload(true);
		m_pPluginDict->remove(it.current()->name());
	}
}

bool KviPluginManager::findPlugin(QString& pName, QString& pSingleName)
{
	QDir d;
	QString szPath;	
	
	//Trying to find the plugin directly
	d.setPath(pName);
	d.convertToAbs(); // To be sure that path is completly absolute!
	
	if(!KviFileUtils::fileExists(d.path()))
	{
		//Plugin not found in direct way. Looking in kvirc local dir
		g_pApp->getGlobalKvircDirectory(szPath,KviApp::None,"easyplugins/"+pName);
		
		d.setPath(szPath);
		d.convertToAbs(); // To be sure that path is completly absolute!
		
		if(!KviFileUtils::fileExists(d.path()))
		{
			//Plugin not found in kvirc local dir. Looking in kvirc global dir
			g_pApp->getLocalKvircDirectory(szPath,KviApp::None,"easyplugins/"+pName);
			
			d.setPath(szPath);
			d.convertToAbs(); // To be sure that path is completly absolute!
			
			if(!KviFileUtils::fileExists(d.path()))
			{
				return false;
			}
		}
	}
	
	pName  = d.path();
	pSingleName = KviFileUtils::extractFileName(pName);	//TODO
	
	
	return true;
}

bool KviPluginManager::pluginIsLoaded(QString& pSingleName)
{
	KviPlugin * p = m_pPluginDict->find(pSingleName.local8Bit());
	if (!p) return false; else return true;
}

bool KviPluginManager::loadPlugin(QString& pName, QString& pSingleName)
{
	KviPlugin * plugin = new KviPlugin();
	if(!plugin->load(pName)) return false;
	plugin->setName(pSingleName);
	m_pPluginDict->insert(pSingleName.local8Bit(),plugin);
	return true;
}

KviPlugin * KviPluginManager::getPlugin(QString& pSingleName)
{
	KviPlugin * p = m_pPluginDict->find(pSingleName.local8Bit());
	return p;
}