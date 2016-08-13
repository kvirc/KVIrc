//=============================================================================
//
//   File : libkvieditor.cpp
//   Creation date : Mon Sep 11 2000 12:19:00 by Szymon Stefanek
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

#include "ScriptEditorImplementation.h"

#include "KviModule.h"
#include "KviWindow.h"

KviModule * g_pEditorModulePointer = nullptr;

std::set<ScriptEditorImplementation *> g_pScriptEditorWindowList;

static bool editor_module_init(KviModule * m)
{
	g_pEditorModulePointer = m;
	return true;
}

static bool editor_module_cleanup(KviModule *)
{
	/*
	 * This causes 2 crashes: one in KviApplication destructor (closing windows needs
	 * g_pMainWindow, that is deleted before this unloading routine) and the second in
	 * the codetester window (it deletes us in its denstructor, and we tries to back-delete it)
	 * So it's commented out by now..
	 */

	while(!g_pScriptEditorWindowList.empty())
	{
		auto & t = *g_pScriptEditorWindowList.begin();
		QObject * w = t->parent();
		while(w)
		{
			//qDebug("%s %s %i %s",__FILE__,__FUNCTION__,__LINE__,w->className());
			if(w->inherits("KviWindow"))
			{
				//	qDebug("%s %s %i",__FILE__,__FUNCTION__,__LINE__);
				//((KviWindow *)w)->close();
				//	qDebug("%s %s %i",__FILE__,__FUNCTION__,__LINE__);
				break;
			}
			w = w->parent();
		}
		delete t;
	}

	return true;
}

static bool editor_module_can_unload(KviModule *)
{
	return g_pScriptEditorWindowList.empty();
}

KVIRC_MODULE(
    "Editor",                                                       // module name
    "4.0.0",                                                        // module version
    "Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
    "Text editor extension",
    editor_module_init,
    editor_module_can_unload,
    0,
    editor_module_cleanup,
    "editor")

// We want C linkage on this one: we want to be able to dlsym() it with a simple name
// FIXME: Is this portable enough ? Or is better to have a table entry ?

KVIMODULEEXPORTFUNC KviScriptEditor * editor_module_createScriptEditor(QWidget * par)
{
	return new ScriptEditorImplementation(par);
}

KVIMODULEEXPORTFUNC void editor_module_destroyScriptEditor(KviScriptEditor * e)
{
	delete((ScriptEditorImplementation *)e);
}
