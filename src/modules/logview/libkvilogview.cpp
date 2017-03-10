//=============================================================================
//
//   File : libkvilogview.cpp
//   Creation date : Sun Feb 10 2000 23:25:10 CEST by Juanjo Alvarez
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2011 Elvio Basello (hellvis69 at gmail dot com)
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

#include "LogViewWidget.h"
#include "LogViewWindow.h"

#include "KviConfigurationFile.h"
#include "KviModule.h"
#include "KviMainWindow.h"
#include "KviIconManager.h"
#include "KviLocale.h"
#include "KviApplication.h"

static QRect g_rectLogViewGeometry;
LogViewWindow * g_pLogViewWindow = nullptr;

#define LOGVIEW_MODULE_EXTENSION_NAME "Log viewer extension"

/*
	@doc: logview.open
	@type:
		command
	@title:
		logview.open
	@short:
		Opens the log viewer window
	@syntax:
		logview.open [-m] [-n]
	@switches:
		!sw: -m
		Causes the window to be created as minimized
		!sw: -n
		Causes the window to be not raised if already open
	@description:
		Opens a window that allows visual browsing of the logs
		stored on disk.
*/

static bool logview_kvs_cmd_open(KviKvsModuleCommandCall * c)
{
	QString dummy;
	bool bCreateMinimized = c->hasSwitch('m', dummy);
	bool bNoRaise = c->hasSwitch('n', dummy);

	if(!g_pLogViewWindow)
	{
		g_pLogViewWindow = new LogViewWindow();
		g_pMainWindow->addWindow(g_pLogViewWindow, !bCreateMinimized);
		return true;
	}

	if(!bNoRaise)
		g_pLogViewWindow->delayedAutoRaise();
	return true;
}

static bool logview_module_init(KviModule * m)
{
	g_pLogViewWindow = nullptr;

	KVSM_REGISTER_SIMPLE_COMMAND(m, "open", logview_kvs_cmd_open);

	return true;
}

static bool logview_module_cleanup(KviModule *)
{
	if(g_pLogViewWindow && g_pMainWindow)
		g_pMainWindow->closeWindow(g_pLogViewWindow);
	g_pLogViewWindow = nullptr;
	return true;
}

static bool logview_module_can_unload(KviModule *)
{
	return (!g_pLogViewWindow);
}

static bool logview_module_ctrl(KviModule *, const char * pcOperation, void * pParam)
{
	if(!kvi_strEqualCI("logview::export", pcOperation))
		return false;

	LogFileData * pData = (LogFileData *)pParam;
	if(!pData)
		return false;

	LogFile log{pData->szName};
	int iId = LogFile::PlainText;
	if(pData->szType == QLatin1String("html"))
		iId = LogFile::HTML;

	g_pLogViewWindow->createLog(&log, iId, &(pData->szFile));

	return true;
}

KVIRC_MODULE(
    "KVIrc Log Viewer Widget",
    "4.0.0",
    "Copyright (C) 2000 Juanjo Alvarez <juanjux@yahoo.es>\n"
    "	2011 Elvio Basello (hellvis69 at gmail dot com)",
    "A structured log file viewer",
    logview_module_init,
    logview_module_can_unload,
    logview_module_ctrl,
    logview_module_cleanup,
    "logview")
