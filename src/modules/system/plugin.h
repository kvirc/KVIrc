#ifndef _PLUGIN_H_
#define _PLUGIN_H_
//=============================================================================
//
//   File : plugin.h
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

#include "kvi_module.h"
#include "kvi_asciidict.h"

#define MAX_RETURN_BUFFER 512

typedef int (*plugin_function)(int argc, char* argv[], char ** buffer);
typedef int (*plugin_unload)();
typedef int (*plugin_load)();

class KviPlugin
{
	public:
		KviPlugin();
		~KviPlugin();
	private:
		// shared
		// internal
		KviStr m_szName;
		kvi_library_t m_Plugin;
	public:
		bool load(QString * pszPluginName);
		bool unload(bool forced);
		int call(QString * pszFunctionName, int argc, char * argv[], char ** pBuffer);
		KviStr name();
		void setName(QString * Name);
	protected:
};

class KviPluginManager
{
	public:
		KviPluginManager();
		~KviPluginManager();
	private:
		// shared
		bool m_bCanUnload;
		// internal
		KviAsciiDict<KviPlugin> * m_pPluginDict;
	public:
		bool PluginCall(KviKvsModuleFunctionCall *c);
		bool CheckUnload();
		void UnloadAll(bool forced);
	protected:
		bool FindPlugin(QString * pName, QString * pSingleName);
		bool PluginIsLoaded(QString * pSingleName);
		bool LoadPlugin(QString * pName, QString * pSingleName);
		KviPlugin * GetPlugin(QString * pSingleName);
};

#endif //_PLUGIN_H_