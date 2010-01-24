//=============================================================================
//
//   File : libkvifiletransferwindow.cpp
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

#include "filetransferwindow.h"

#include "kvi_module.h"
#include "kvi_app.h"
#include "kvi_frame.h"
#include "kvi_window.h"
#include "kvi_locale.h"
#include "kvi_iconmanager.h"

#include <QSplitter>

KviFileTransferWindow * g_pFileTransferWindow = 0;

static KviModuleExtension * filetransferwindow_extension_alloc(KviModuleExtensionAllocStruct * s)
{
	bool bCreateMinimized = false;
	bool bNoRaise = false;

	if(!g_pFileTransferWindow)
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

		g_pFileTransferWindow = new KviFileTransferWindow(s->pDescriptor,g_pFrame);
		g_pFrame->addWindow(g_pFileTransferWindow,!bCreateMinimized);
		if(bCreateMinimized)g_pFileTransferWindow->minimize();
		return g_pFileTransferWindow;
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

	if(!bNoRaise)g_pFileTransferWindow->delayedAutoRaise();
	return g_pFileTransferWindow;
}


/*
	@doc: filetransferwindow.open
	@type:
		command
	@title:
		filetransferwindow.open
	@short:
		Opens the file transfer window
	@syntax:
		filetransferwindow.open [-m] [-n]
	@switches:
		!sw: -m
		Causes the window to be created as minimized
		!sw: -n
		Causes the window to be not raised if already open
	@description:
		Opens the file transfer window
*/

static bool filetransferwindow_kvs_cmd_open(KviKvsModuleCommandCall * c)
{
	KviModuleExtensionDescriptor * d = c->module()->findExtensionDescriptor("tool",KVI_FILE_TRANSFER_WINDOW_EXTENSION_NAME);

	if(d)
	{
		KviPointerHashTable<QString,QVariant> dict(17,true);
		dict.setAutoDelete(true);
		QString dummy;
		dict.replace("bCreateMinimized",new QVariant(c->hasSwitch('m',dummy)));
		dict.replace("bNoRaise",new QVariant(c->hasSwitch('n',dummy)));

		d->allocate(c->window(),&dict,0);
	} else {
		c->warning(__tr("Ops.. internal error"));
	}
	return true;
}

static bool filetransferwindow_module_init(KviModule * m)
{
	KviModuleExtensionDescriptor * d = m->registerExtension("tool",
		KVI_FILE_TRANSFER_WINDOW_EXTENSION_NAME,
		__tr2qs("Manage File &Transfers"),
		filetransferwindow_extension_alloc);

	if(d)d->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_FILETRANSFER)));


	KVSM_REGISTER_SIMPLE_COMMAND(m,"open",filetransferwindow_kvs_cmd_open);

	return true;
}

static bool filetransferwindow_module_cleanup(KviModule *)
{
	if(g_pFileTransferWindow && g_pFrame)
		g_pFrame->closeWindow(g_pFileTransferWindow);
	g_pFileTransferWindow = 0;
	return true;
}

static bool filetransferwindow_module_can_unload(KviModule *)
{
	return (!g_pFileTransferWindow);
}

KVIRC_MODULE(
	"FileTransferWindow",
	"4.0.0",
	"Copyright (C) 2008 Szymon Stefanek (pragma at kvirc dot net)",
	"Links window extension",
	filetransferwindow_module_init,
	filetransferwindow_module_can_unload,
	0,
	filetransferwindow_module_cleanup,
	"filetransferwindow"
)
