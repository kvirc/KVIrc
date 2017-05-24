//=============================================================================
//
//   File : libkvihelp.cpp
//   Creation date : Sun Aug 13 2000 03:00:00 by Szymon Stefanek
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

#include "HelpWidget.h"
#include "HelpWindow.h"
#include "HelpIndex.h"
#include "KviLocale.h"
#include "KviModule.h"
#include "KviFileUtils.h"
#include "kvi_sourcesdate.h"
#include "KviApplication.h"
#include "KviMainWindow.h"

#include <QDir>
#include <QFileInfo>
#include <QSplitter>

HelpIndex * g_pDocIndex = nullptr;
KviPointerList<HelpWidget> * g_pHelpWidgetList = nullptr;
KviPointerList<HelpWindow> * g_pHelpWindowList = nullptr;

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
		If no help browser is available, a new one is created.
		[document] can be an absolute path or a relative one: in this case
		the document is searched in the KVIrc documentation directory.[br]
		If no document has been found using absolute and relative paths,
		the first document matching [document] in the help search database
		is shown. Otherwise, an error page is displayed.
		The help browser has limited HTML browsing capabilities: you can
		use it to view simple HTML files on your filesystem.[br]
		This command is exported by the "help" module.
	@switches:
		!sw: -m | --mdi
		The created browser is a MDI window,
		otherwise it is a static window.
		!sw: -n | --new
		The window creation is forced even
		if there are other help browsers already open.[br]
	@examples:
		[example]
			help.open /home/pragma/myfile.html	//absolute path
			help.open cmd_snd.play.html		//relative path
			help.open "Binding operator"		//keyword search, remember quoting
			help.open OnNickServAuth		//keyword search for an event
			help.open \$my.user			//keyword search, $ needs to be escaped
		[/example]
*/

static bool help_kvs_cmd_open(KviKvsModuleCommandCall * c)
{
	QString szDoc, szHelpDir, szParam;
	QDir dirHelp;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("document", KVS_PT_STRING, KVS_PF_OPTIONAL | KVS_PF_APPENDREMAINING, szParam)
	KVSM_PARAMETERS_END(c)

	// no document => index
	if(szParam.isEmpty())
	{
		szParam = QString("index.html");
		qDebug("No file, use default at path %s", szDoc.toUtf8().data());
	}

	/*
	 * Path checking order:
	 * 1) absolute path
	 * 2) local help (in user directory)
	 * 3) global help (in kvirc directory)
	 */

	// try absolute path
	QFileInfo f(szParam);
	if(!f.exists() || !f.isAbsolute())
	{
		// try relative path (to local help)
		g_pApp->getLocalKvircDirectory(szHelpDir, KviApplication::Help);
		dirHelp = QDir(szHelpDir);
		szDoc = dirHelp.absoluteFilePath(szParam);
		qDebug("No abs path, trying local relative path: %s", szDoc.toUtf8().data());
		f.setFile(szDoc);

		if(!f.exists())
		{
			//try relative path (to global help)
			g_pApp->getGlobalKvircDirectory(szHelpDir, KviApplication::Help);
			dirHelp = QDir(szHelpDir);

			szDoc = dirHelp.absoluteFilePath(szParam);
			qDebug("No local relative, trying global relative path: %s", szDoc.toUtf8().data());
			f.setFile(szDoc);
		}
	}

	// Search in help
	if(!f.exists())
	{
		qDebug("No path, trying search..");
		if(g_pDocIndex)
		{
			if(!g_pDocIndex->documentList().count())
			{
				QString szDoclist, szDict;
				g_pApp->getLocalKvircDirectory(szDoclist, KviApplication::Help, "help.doclist." KVI_SOURCES_DATE);
				g_pApp->getLocalKvircDirectory(szDict, KviApplication::Help, "help.dict." KVI_SOURCES_DATE);
				if(KviFileUtils::fileExists(szDoclist) && KviFileUtils::fileExists(szDict))
				{
					g_pDocIndex->readDict();
				}
				else
				{
					g_pDocIndex->makeIndex();
					g_pDocIndex->writeDict();
				}
			}

			int i = g_pDocIndex->titlesList().indexOf(QRegExp(QRegExp::escape(szParam), Qt::CaseInsensitive));
			if(i != -1)
			{
				szDoc = QUrl(g_pDocIndex->documentList()[i]).toLocalFile();
				f.setFile(szDoc);
			}
			else
			{
				QString szTmpDocName(".*/doc_");
				szTmpDocName.append(QRegExp::escape(szParam));
				szTmpDocName.append("\\.html");
				i = g_pDocIndex->documentList().indexOf(QRegExp(szTmpDocName, Qt::CaseInsensitive));
				if(i != -1)
				{
					szDoc = QUrl(g_pDocIndex->documentList()[i]).toLocalFile();
					f.setFile(szDoc);
				}
			}
		}
	}

	// Everything failed => error
	if(!f.exists())
	{
		szDoc = dirHelp.absoluteFilePath("nohelpavailable.html");

		qDebug("Document not found, defaulting to error page: %s", szDoc.toUtf8().data());
		f.setFile(szDoc);
	}

	if(!c->switches()->find('n', "new"))
	{
		HelpWidget * w = (HelpWidget *)g_pMainWindow->findChild<HelpWidget *>("help_widget");

		if(w)
		{
#ifdef COMPILE_WEBKIT_SUPPORT
			w->textBrowser()->load(QUrl::fromLocalFile(f.absoluteFilePath()));
#else
			w->textBrowser()->setSource(QUrl::fromLocalFile(f.absoluteFilePath()));
#endif
			HelpWindow * pHelpWindow = g_pHelpWindowList->first();
			if (pHelpWindow)
				pHelpWindow->delayedAutoRaise();
			return true;
		}
	}
	if(c->switches()->find('m', "mdi"))
	{
		HelpWindow * w = new HelpWindow("Help browser");
#ifdef COMPILE_WEBKIT_SUPPORT
		w->textBrowser()->load(QUrl::fromLocalFile(f.absoluteFilePath()));
#else
		w->textBrowser()->setSource(QUrl::fromLocalFile(f.absoluteFilePath()));
#endif
		g_pMainWindow->addWindow(w);
	}
	else
	{
		HelpWidget * w = new HelpWidget(g_pMainWindow->splitter(), true);
#ifdef COMPILE_WEBKIT_SUPPORT
		w->textBrowser()->load(QUrl::fromLocalFile(f.absoluteFilePath()));
#else
		w->textBrowser()->setSource(QUrl::fromLocalFile(f.absoluteFilePath()));
#endif
		w->show();
	}

	return true;
}

static bool help_module_init(KviModule * m)
{
	QString szHelpDir, szDocList;

	g_pApp->getLocalKvircDirectory(szDocList, KviApplication::Help, "help.doclist." KVI_SOURCES_DATE);
	g_pApp->getGlobalKvircDirectory(szHelpDir, KviApplication::Help);

	g_pDocIndex = new HelpIndex(szHelpDir, szDocList);
	g_pDocIndex->setDocListFile(szDocList);

	g_pApp->getLocalKvircDirectory(szHelpDir, KviApplication::Help, "help.dict." KVI_SOURCES_DATE);
	g_pDocIndex->setDictionaryFile(szHelpDir);

	g_pHelpWidgetList = new KviPointerList<HelpWidget>;
	g_pHelpWidgetList->setAutoDelete(false);
	g_pHelpWindowList = new KviPointerList<HelpWindow>;
	g_pHelpWindowList->setAutoDelete(false);

	KVSM_REGISTER_SIMPLE_COMMAND(m, "open", help_kvs_cmd_open);

	return true;
}

static bool help_module_cleanup(KviModule *)
{
	if(g_pDocIndex)
		delete g_pDocIndex;
	while(g_pHelpWidgetList->first())
		delete g_pHelpWidgetList->first();
	delete g_pHelpWidgetList;
	g_pHelpWidgetList = nullptr;
	while(g_pHelpWindowList->first())
		g_pHelpWindowList->first()->close();
	delete g_pHelpWindowList;
	g_pHelpWindowList = nullptr;
	return true;
}

static bool help_module_can_unload(KviModule *)
{
	return (g_pHelpWidgetList->isEmpty() && g_pHelpWindowList->isEmpty());
}

KVIRC_MODULE(
    "Help",                                                         // module name
    "4.0.0",                                                        // module version
    "Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
    "Help browser extension",
    help_module_init,
    help_module_can_unload,
    0,
    help_module_cleanup,
    0)
