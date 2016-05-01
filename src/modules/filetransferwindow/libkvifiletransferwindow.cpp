//=============================================================================
//
//   File : libkvifiletransferwindow.cpp
//   Creation date : Mon Apr 21 2003 23:14:12 CEST by Szymon Stefanek
//
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "FileTransferWindow.h"

#include "KviModule.h"
#include "KviApplication.h"
#include "KviMainWindow.h"
#include "KviWindow.h"
#include "KviLocale.h"
#include "KviIconManager.h"

#include <QSplitter>

FileTransferWindow * g_pFileTransferWindow = nullptr;

static FileTransferWindow * filetransferwindow_alloc(bool bCreateMinimized, bool bNoRaise)
{
	if(!g_pFileTransferWindow)
	{
		g_pFileTransferWindow = new FileTransferWindow();
		g_pMainWindow->addWindow(g_pFileTransferWindow, !bCreateMinimized);
		return g_pFileTransferWindow;
	}

	if(!bNoRaise)
		g_pFileTransferWindow->delayedAutoRaise();
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
		!sw: -m | --minimized
		Causes the window to be created as minimized
		!sw: -n | --noraise
		Causes the window to be not raised if already open
	@description:
		Opens the file transfer window
*/

static bool filetransferwindow_kvs_cmd_open(KviKvsModuleCommandCall * c)
{
	filetransferwindow_alloc(c->hasSwitch('m', QString::fromLatin1("minimized")), c->hasSwitch('n', QString::fromLatin1("noraise")));

	return true;
}

/*
	@doc: filetransferwindow.clear
	@type:
		command
	@title:
		filetransferwindow.clear
	@short:
		Clears the list of transfers
	@syntax:
		filetransferwindow.clear [-a]
	@switches:
		!sw: -a | --all
		Clears all the transfers, including the running ones!
	@description:
		Clears the transfers. Without the -a switch only the terminated
		transfers are cleared. With -a all the transfers are terminated (including the running ones).
*/

static bool filetransferwindow_kvs_cmd_clear(KviKvsModuleCommandCall * c)
{

	if(c->hasSwitch('a', QString::fromLatin1("all")))
		KviFileTransferManager::instance()->killAllTransfers();
	else
		KviFileTransferManager::instance()->killTerminatedTransfers();

	return true;
}

static bool filetransferwindow_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m, "open", filetransferwindow_kvs_cmd_open);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "clear", filetransferwindow_kvs_cmd_clear);

	return true;
}

static bool filetransferwindow_module_cleanup(KviModule *)
{
	if(g_pFileTransferWindow && g_pMainWindow)
		g_pMainWindow->closeWindow(g_pFileTransferWindow);
	g_pFileTransferWindow = nullptr;
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
    "File Transfer Window Extension",
    filetransferwindow_module_init,
    filetransferwindow_module_can_unload,
    0,
    filetransferwindow_module_cleanup,
    "filetransferwindow")
