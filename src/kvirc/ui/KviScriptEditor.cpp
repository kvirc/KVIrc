//=============================================================================
//
//   File : KviScriptEditor.cpp
//   Creation date : Sun Mar 28 1999 16:12:41 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#define _KVI_SCRIPTEDITOR_CPP_
#define _KVI_DEBUG_CHECK_RANGE_

#include "kvi_debug.h"
#include "KviScriptEditor.h"
#include "KviModuleManager.h"

#include <QLineEdit>

void KviScriptEditor::setText(const QString & txt)
{
	setText(QByteArray(txt.toUtf8()));
}

void KviScriptEditor::getText(QString & txt)
{
	QByteArray tmp;
	getText(tmp);
	txt = QString::fromUtf8(tmp.data());
}

static KviScriptEditor * (*editorModuleCreateScriptEditor)(QWidget *);
static void (*editorModuleDestroyScriptEditor)(KviScriptEditor *);

KviScriptEditor * KviScriptEditor::createInstance(QWidget * par)
{
	KviModule * m = g_pModuleManager->getModule("editor");
	// If the module can't be loaded...return a dummy widget
	// FIXME: #warning "Maybe provide some sort of basic default editable widget ?"
	if(!m)
		return KviScriptEditor::getDummyEditor(par); // dummy implementation

	editorModuleCreateScriptEditor = (KviScriptEditor * (*)(QWidget *))m->getSymbol("editor_module_createScriptEditor");

	if(!editorModuleCreateScriptEditor)
		return KviScriptEditor::getDummyEditor(par);

	return editorModuleCreateScriptEditor(par);
}

void KviScriptEditor::destroyInstance(KviScriptEditor * e)
{
	KviModule * m = g_pModuleManager->getModule("editor");
	if(!m)
	{
		delete e;
		return;
	}

	editorModuleDestroyScriptEditor = (void (*)(KviScriptEditor *))m->getSymbol("editor_module_destroyScriptEditor");

	if(!editorModuleDestroyScriptEditor)
	{
		delete e;
		return;
	}

	editorModuleDestroyScriptEditor(e);
}
