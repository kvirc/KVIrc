//=============================================================================
//
//   File : libkvisharedfileswindow.cpp
//   Creation date : Mon Apr 21 2003 23:14:12 CEST by Szymon Stefanek
//
//   Copyright (C) 2003-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "sharedfileswindow.h"

#include "kvi_module.h"
#include "kvi_app.h"
#include "kvi_frame.h"
#include "kvi_window.h"
#include "kvi_locale.h"
#include "kvi_iconmanager.h"

#include <QSplitter>

#define KVI_SHARED_FILES_WINDOW_EXTENSION_NAME "Shared files window extension"

KviSharedFilesWindow * g_pSharedFilesWindow = 0;

static KviModuleExtension * sharedfileswindow_extension_alloc(KviModuleExtensionAllocStruct * s)
{
	bool bCreateMinimized = false;
	bool bNoRaise = false;

	if(!g_pSharedFilesWindow)
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

		g_pSharedFilesWindow = new KviSharedFilesWindow(s->pDescriptor,g_pFrame);
		g_pFrame->addWindow(g_pSharedFilesWindow,!bCreateMinimized);
		if(bCreateMinimized)g_pSharedFilesWindow->minimize();
		return g_pSharedFilesWindow;
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

	if(!bNoRaise)g_pSharedFilesWindow->delayedAutoRaise();
	return g_pSharedFilesWindow;
}


/*
	@doc: sharedfileswindow.open
	@type:
		command
	@title:
		sharedfileswindow.open
	@short:
		Opens the shared files window
	@syntax:
		sharedfileswindow.open [-m] [-n]
	@switches:
		!sw: -m
		Causes the window to be created as minimized
		!sw: -n
		Causes the window to be not raised if already open
	@description:
		Opens the shared files window
*/

static bool sharedfileswindow_kvs_cmd_open(KviKvsModuleCommandCall * c)
{
	KviModuleExtensionDescriptor * d = c->module()->findExtensionDescriptor("tool",KVI_SHARED_FILES_WINDOW_EXTENSION_NAME);
	if(d)
	{
		KviPointerHashTable<QString,QVariant> dict(17,true);
		dict.setAutoDelete(true);
		dict.replace("bCreateMinimized",new QVariant(c->hasSwitch('m',"minimized")));
		dict.replace("bNoRaise",new QVariant(c->hasSwitch('n',"noraise")));

		d->allocate(c->window(),&dict,0);
	} else {
		c->warning("Ops.. internal error");
	}
	return true;
}

static bool sharedfileswindow_module_init(KviModule * m)
{
	KviModuleExtensionDescriptor * d = m->registerExtension("tool",
		KVI_SHARED_FILES_WINDOW_EXTENSION_NAME,
		__tr2qs_ctx("Manage S&hared Files","sharedfileswindow"),
		sharedfileswindow_extension_alloc);

	if(d)d->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SHAREDFILES)));

	KVSM_REGISTER_SIMPLE_COMMAND(m,"open",sharedfileswindow_kvs_cmd_open);
	return true;
}

static bool sharedfileswindow_module_cleanup(KviModule *)
{
	if(g_pSharedFilesWindow && g_pFrame)
		g_pFrame->closeWindow(g_pSharedFilesWindow);
	g_pSharedFilesWindow = 0;
	return true;
}

static bool sharedfileswindow_module_can_unload(KviModule *)
{
	return (!g_pSharedFilesWindow);
}

KVIRC_MODULE(
	"SharedFilesWindow",
	"4.0.0",
	"Copyright (C) 2003 Szymon Stefanek (pragma at kvirc dot net)",
	"Shared files window extension",
	sharedfileswindow_module_init,
	sharedfileswindow_module_can_unload,
	0,
	sharedfileswindow_module_cleanup,
	"sharedfileswindow"
)
