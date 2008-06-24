//
//   File : libkvihelp.cpp
//   Creation date : Sun Aug 13 2000 03:00:00 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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

#include "helpwidget.h"
#include "helpwindow.h"
#include "index.h"

#include "kvi_module.h"
#include "kvi_sourcesdate.h"
#include "kvi_app.h"
#include "kvi_frame.h"

#include <QDir>
#include <QFileInfo>
#include <QSplitter>

Index        * g_pDocIndex = 0;
KviPointerList<KviHelpWidget> * g_pHelpWidgetList = 0;
KviPointerList<KviHelpWindow> * g_pHelpWindowList = 0;

/*
	@doc: help.search
	@type:
		command
	@title:
		help.search
	@short:
		Searches the documentation
	@syntax:
		help.search [-n] [-m] <key terms>
	@description:
		Finds the first available help browser in the current frame
		then searches the documentation for the specified <key terms>.
		If no help browser is available it creates one first:
		if the -m switch is present, the created browser is a MDI window,
		otherwise it is a static window.
		If the -n switch is present, the window creation is forced even
		if there are other help browsers already open.[br]
		The <key terms> are [b]space separated words[/b]
		to be matched inside the document body (logical AND).[br]
		This command is exported by the "help" module.
*/

/*tatic bool help_module_cmd_search(KviModule *m,KviCommand *c)
{
	ENTER_STACK_FRAME(c,"help_module_cmd_search");

	KviStr keys;
	if(!g_pZZZZZZUserParser->parseCmdFinalPart(c,keys))return false;

	if(keys.isEmpty())keys = "kvirc";

	if(!c->hasSwitch('n'))
	{
		// look for an already open help widget in this frame
		KviHelpWidget * w = (KviHelpWidget *)c->window()->frame()->child(
			"help_widget","KviHelpWidget");

		if(w)
		{
			w->doExactSearchFor(keys.ptr());
			return c->leaveStackFrame();
		}
	}

	if(c->hasSwitch('m'))
	{
		KviHelpWindow *w = new KviHelpWindow(c->window()->frame(),"Help browser");
		w->helpWidget()->doExactSearchFor(keys.ptr());
		c->window()->frame()->addWindow(w);
	} else {
		KviHelpWidget *w = new KviHelpWidget(c->window()->frame()->splitter(),
			c->window()->frame(),true);
		w->doExactSearchFor(keys.ptr());
		w->show();
	}

	return c->leaveStackFrame();
}*/
/*
#ifdef COMPILE_NEW_KVS
static bool help_kvs_cmd_search(KviKvsModuleCommandCall * c)
{
}
#endif
*/
/*
	@doc: help.open
	@type:
		command
	@title:
		help.open
	@short:
		Shows a help document
	@syntax:
		help.open [-n] [-m] [document: string]
	@description:
		Finds the first available help browser in the current frame
		then opens the specified [document].
		If no [document] is specified it the documentation index is shown.
		If no help browser is available , a new one is created.
		[document] can be an absolute path or a relative one: in this case
		the document is searched in the KVIrc documentation directory.[br]
		The help browser has limited html browsing capabilities: you can
		use it to view simple html files on your filesystem.[br]
		This command is exported by the "help" module.
	@switches:
		!sw: -m | --mdi
		The created browser is a MDI window,
		otherwise it is a static window.
		!sw: -n | --new
		The window creation is forced even
		if there are other help browsers already open.[br]
*/


static bool help_kvs_cmd_open(KviKvsModuleCommandCall * c)
{
	QString doc, szHelpDir;
        QDir dirHelp;

	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("document",KVS_PT_STRING,KVS_PF_OPTIONAL,doc)
	KVSM_PARAMETERS_END(c)

	g_pApp->getGlobalKvircDirectory(szHelpDir,KviApp::Help);
        dirHelp = QDir(szHelpDir);

	if(doc.isEmpty()){
		doc = dirHelp.absoluteFilePath("index.html");
		qDebug ("No file, use default at path %s",doc.toUtf8().data());
	} else qDebug("Doc set from user to %s",doc.toUtf8().data());

	QFileInfo * f= new QFileInfo(doc);
	qDebug("Path %s",doc.toUtf8().data());
	if(f)
	{
		if(!f->exists())
		{
			doc = dirHelp.absoluteFilePath(doc);
			qDebug("No abs path, trying relative path: %s",doc.toUtf8().data());
			f->setFile(doc);
		}
		if(!f->exists())
		{
			doc = dirHelp.absoluteFilePath("nohelpavailable.html");
			qDebug("No rel path, defaulting to error page: %s",doc.toUtf8().data());
			f->setFile(doc);
		}
	}

	if(!c->switches()->find('n',"new"))
	{
		KviHelpWidget * w = (KviHelpWidget *)c->window()->frame()->child("help_widget","KviHelpWidget");
		if(w)
		{
			w->textBrowser()->setSource(QUrl::fromLocalFile(f->absoluteFilePath()));
			return true;
		}
	}
	if(c->switches()->find('m',"mdi"))
	{
		KviHelpWindow *w = new KviHelpWindow(c->window()->frame(),"Help browser");
		w->textBrowser()->setSource(QUrl::fromLocalFile(f->absoluteFilePath()));
		c->window()->frame()->addWindow(w);
	} else {
		KviHelpWidget *w = new KviHelpWidget(c->window()->frame()->splitter(),
			c->window()->frame(),true);
		w->textBrowser()->setSource(QUrl::fromLocalFile(f->absoluteFilePath()));
		w->show();
	}

	return true;
}

static bool help_module_init(KviModule * m)
{
	QString szHelpDir,szDocList;

	g_pApp->getLocalKvircDirectory(szDocList,KviApp::Help,"help.doclist." KVI_SOURCES_DATE);
	g_pApp->getGlobalKvircDirectory(szHelpDir,KviApp::Help);

	g_pDocIndex = new Index(szHelpDir,szDocList);
	g_pDocIndex->setDocListFile(szDocList);

	g_pApp->getLocalKvircDirectory(szHelpDir,KviApp::Help,"help.dict." KVI_SOURCES_DATE);
	g_pDocIndex->setDictionaryFile(szHelpDir);

	g_pHelpWidgetList = new KviPointerList<KviHelpWidget>;
	g_pHelpWidgetList->setAutoDelete(false);
	g_pHelpWindowList = new KviPointerList<KviHelpWindow>;
	g_pHelpWindowList->setAutoDelete(false);

	KVSM_REGISTER_SIMPLE_COMMAND(m,"open",help_kvs_cmd_open);


	return true;
}

static bool help_module_cleanup(KviModule *m)
{
	if(g_pDocIndex) delete g_pDocIndex;
	while(g_pHelpWidgetList->first())delete g_pHelpWidgetList->first();
	delete g_pHelpWidgetList;
	g_pHelpWidgetList = 0;
	while(g_pHelpWindowList->first())g_pHelpWindowList->first()->close();
	delete g_pHelpWindowList;
	g_pHelpWindowList = 0;
	return true;
}

static bool help_module_can_unload(KviModule *m)
{
	return (g_pHelpWidgetList->isEmpty() && g_pHelpWindowList->isEmpty());
}

KVIRC_MODULE(
	"Help",                                                 // module name
	"4.0.0",                                                // module version
	"Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
	"Help browser extension",
	help_module_init,
	help_module_can_unload,
	0,
	help_module_cleanup
)
