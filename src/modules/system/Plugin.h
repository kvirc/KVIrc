#ifndef _PLUGIN_H_
#define _PLUGIN_H_
//=============================================================================
//
//   File : Plugin.h
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

#include "KviModule.h"
#include "KviPointerHashTable.h"
#include <QLibrary>

typedef int (*plugin_function)(int argc, char * argv[], char ** buffer);
typedef int (*plugin_unload)();
typedef int (*plugin_canunload)();
typedef int (*plugin_load)();
typedef int (*plugin_free)(char * pBuffer);

class Plugin
{
protected:
	// You have to create plugin instance by calling Plugin::load()
	Plugin(QLibrary * pLibrary, const QString & name);

public:
	~Plugin();

private:
	// shared
	// internal
	QLibrary * m_pLibrary;
	QString m_szName;

public:
	static Plugin * load(const QString & szFileName);
	bool pfree(char * pBuffer);
	bool unload();
	bool canunload();
	int call(const QString & szFunctionName, int argc, char * argv[], char ** pBuffer);
	QString name() const;
	void setName(const QString & szName);

protected:
};

class PluginManager
{
public:
	PluginManager();
	~PluginManager();

private:
	// shared
	bool m_bCanUnload;
	// internal
	KviPointerHashTable<QString, Plugin> * m_pPluginDict;

public:
	bool pluginCall(KviKvsModuleFunctionCall * c);
	bool checkUnload();
	void unloadAll();

protected:
	bool findPlugin(QString & szName);
	bool isPluginLoaded(const QString & szFileNameOrPathToLoad);
	bool loadPlugin(const QString & szPluginPath);
	Plugin * getPlugin(const QString & szPluginPath);
};

#endif //_PLUGIN_H_
