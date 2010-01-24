//=============================================================================
//
//   File : libkvilogview.cpp
//   Creation date : Sun Feb 10 2000 23:25:10 CEST by Juanjo �varez
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

#include "logviewwidget.h"
#include "logviewmdiwindow.h"

#include "kvi_config.h"
#include "kvi_module.h"
#include "kvi_frame.h"
#include "kvi_iconmanager.h"
#include "kvi_locale.h"
#include "kvi_app.h"

static QRect                 g_rectLogViewGeometry;
KviLogViewMDIWindow        * g_pLogViewWindow = 0;

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

// ============================================
// Module stuff
// ============================================

static bool logview_kvs_cmd_open(KviKvsModuleCommandCall * c)
{
	KviModuleExtensionDescriptor * d = c->module()->findExtensionDescriptor("tool",LOGVIEW_MODULE_EXTENSION_NAME);
	if(d)
	{
		KviPointerHashTable<QString,QVariant> dict(17,true);
		dict.setAutoDelete(true);
		QString dummy;
		dict.replace("bCreateMinimized",new QVariant(c->hasSwitch('m',dummy)));
		dict.replace("bNoRaise",new QVariant(c->hasSwitch('n',dummy)));

		d->allocate(c->window(),&dict,0);
	} else {
		c->warning("Ops.. internal error");
	}
	return true;
}

static KviModuleExtension * logview_extension_alloc(KviModuleExtensionAllocStruct * s)
{
	bool bCreateMinimized = false;
	bool bNoRaise = false;

	if(!g_pLogViewWindow)
	{
		if(s->pParams)
		{
			if(QVariant * v = s->pParams->find("bCreateMinimized"))
			{
				if(v->isValid())
				{
					if(v->type() == QVariant::Bool)
					{
						bCreateMinimized = v->toBool();
					}
				}
			}
		}

		g_pLogViewWindow = new KviLogViewMDIWindow(s->pDescriptor,g_pFrame);
		g_pFrame->addWindow(g_pLogViewWindow,!bCreateMinimized);
		if(bCreateMinimized)g_pLogViewWindow->minimize();
		return g_pLogViewWindow;
	}

	if(s->pParams)
	{
		if(QVariant * v = s->pParams->find("bNoRaise"))
		{
			if(v)
			{
				if(v->isValid() && v->type() == QVariant::Bool)
					bNoRaise = v->toBool();
			}
		}
	}

	if(!bNoRaise)g_pLogViewWindow->delayedAutoRaise();
	return g_pLogViewWindow;
}

static bool logview_module_init(KviModule * m)
{
	g_pLogViewWindow = 0;

	KVSM_REGISTER_SIMPLE_COMMAND(m,"open",logview_kvs_cmd_open);



	KviModuleExtensionDescriptor * d = m->registerExtension("tool",
							LOGVIEW_MODULE_EXTENSION_NAME,
							__tr2qs_ctx("Browse &Log Files","logview"),
							logview_extension_alloc);

	if(d)d->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_LOG)));

	return true;
}

static bool logview_module_cleanup(KviModule *)
{
	if(g_pLogViewWindow && g_pFrame)
		g_pFrame->closeWindow(g_pLogViewWindow);
	g_pLogViewWindow = 0;
	return true;
}

static bool logview_module_can_unload(KviModule *)
{
	return (!g_pLogViewWindow);
}


// ============================================
// module definition structure
// ============================================

KVIRC_MODULE(
	"KVIrc Log Viewer Widget",
	"4.0.0",
	"Juanjo Alvarez <juanjux@yahoo.es>",
	"A structured log file viewer",
	logview_module_init,
	logview_module_can_unload,
	0,
	logview_module_cleanup,
	"logview"
)
