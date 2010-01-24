//=============================================================================
//
//   File : libkvieditor.cpp
//   Creation date : Mon Sep 11 2000 12:19:00 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "scripteditor.h"

#include "kvi_module.h"
#include "kvi_window.h"


KviModule * g_pEditorModulePointer = 0;

KviPointerList<KviScriptEditorImplementation> * g_pScriptEditorWindowList = 0;

static bool editor_module_init(KviModule * m)
{
	g_pScriptEditorWindowList = new KviPointerList<KviScriptEditorImplementation>;
	g_pScriptEditorWindowList->setAutoDelete(false);

	g_pEditorModulePointer = m;

	return true;
}

static bool editor_module_cleanup(KviModule *)
{
	/*
	 * This causes 2 crashes: one in KviApp destructor (closing windows needs
	 * g_pFrame, that is deleted before this unloading routine) and the second in
	 * the codetester window (it deletes us in its denstructor, and we tries to back-delete it)
	 * So it's commented out by now..
	 */

	while(g_pScriptEditorWindowList->first())
	{
		QObject * w = g_pScriptEditorWindowList->first()->parent();;
		while(w)
		{
			//debug("%s %s %i %s",__FILE__,__FUNCTION__,__LINE__,w->className());
			if(w->inherits("KviWindow"))
			{
			//	debug("%s %s %i",__FILE__,__FUNCTION__,__LINE__);
				//((KviWindow *)w)->close();
			//	debug("%s %s %i",__FILE__,__FUNCTION__,__LINE__);
				break;
			}
		w = w->parent();
		}
		delete g_pScriptEditorWindowList->first();
	}

	delete g_pScriptEditorWindowList;
	g_pScriptEditorWindowList = 0;

	return true;
}

static bool editor_module_can_unload(KviModule *)
{
	return ((g_pScriptEditorWindowList == 0) || (g_pScriptEditorWindowList->count() == 0));
}

KVIRC_MODULE(
	"Editor",                                               // module name
	"4.0.0",                                                // module version
	"Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
	"Text editor extension",
	editor_module_init,
	editor_module_can_unload,
	0,
	editor_module_cleanup,
	"editor"
)

// We want C linkage on this one: we want to be able to dlsym() it with a simple name
// FIXME: Is this portable enough ? Or is better to have a table entry ?

KVIMODULEEXPORTFUNC KviScriptEditor * editor_module_createScriptEditor(QWidget * par)
{
	return new KviScriptEditorImplementation(par);
}

KVIMODULEEXPORTFUNC void editor_module_destroyScriptEditor(KviScriptEditor * e)
{
	delete ((KviScriptEditorImplementation *)e);
}
