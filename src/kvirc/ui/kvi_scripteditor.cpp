//
//   File : kvi_scripteditor.cpp
//   Creation date : Sun Mar 28 1999 16:12:41 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2001 Szymon Stefanek (pragma at kvirc dot net)
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

#define __KVIRC__
#define _KVI_SCRIPTEDITOR_CPP_
#define _KVI_DEBUG_CHECK_RANGE_

#include "kvi_debug.h"
#include "kvi_scripteditor.h"
#include "kvi_modulemanager.h"


KviScriptEditor::KviScriptEditor(QWidget * par)
: QWidget(par)
{
}

KviScriptEditor::~KviScriptEditor()
{
}

void KviScriptEditor::setText(const char * txt)
{
	setText(KviQCString(txt));
}

void KviScriptEditor::setText(const KviQCString &txt)
{
}

void KviScriptEditor::setText(const QString &txt)
{
	setText(KviQCString(txt.toUtf8()));
}

void KviScriptEditor::setFindText(const QString &text)
{
}
void KviScriptEditor::setInfoText(const QString &text)
{
}

void KviScriptEditor::setFindLineeditReadOnly(bool b)
{
}

void KviScriptEditor::getText(KviQCString &txt)
{
}

void KviScriptEditor::setCursorPosition(int)
{
}

bool KviScriptEditor::isModified()
{
	return false;
}

int KviScriptEditor::getCursor()
{
	return 0;
}
void KviScriptEditor::getText(QString &txt)
{
	KviQCString tmp;
	getText(tmp);
	txt = QString::fromUtf8(tmp.data());
}

KviScriptEditor * KviScriptEditor::getDummyEditor(QWidget * par)
{
	return new KviScriptEditor(par);
}


static KviScriptEditor * (*editorModuleCreateScriptEditor)(QWidget *);
static void (*editorModuleDestroyScriptEditor)(KviScriptEditor *);

KviScriptEditor * KviScriptEditor::createInstance(QWidget * par)
{
	KviModule * m = g_pModuleManager->getModule("editor");
	// If the module can't be loaded...return a dummy widget
	// FIXME: #warning "Maybe provide some sort of basic default editable widget ?"
	if(!m)return KviScriptEditor::getDummyEditor(par); // dummy implementation


	editorModuleCreateScriptEditor = (KviScriptEditor * (*)(QWidget *)) m->getSymbol("editor_module_createScriptEditor");

	if(!editorModuleCreateScriptEditor)return KviScriptEditor::getDummyEditor(par);

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

	editorModuleDestroyScriptEditor = (void (*)(KviScriptEditor *)) m->getSymbol("editor_module_destroyScriptEditor");

	if(!editorModuleDestroyScriptEditor)
	{
		delete e;
		return;
	}

	editorModuleDestroyScriptEditor(e);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_scripteditor.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
