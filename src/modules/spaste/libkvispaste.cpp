//=============================================================================
//
//   File : libkvispaste.cpp
//   Creation date : Thu Dec 27 2002 17:13:12 GMT by Juanjo �lvarez
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002 Juanjo �lvarez (juanjux@yahoo.es)
//   Copyright (C) 2002-2010 Szymon Stefanek (kvirc@tin.it)
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

#include "libkvispaste.h"
#include "SlowPasteController.h"

#include "KviModule.h"
#include "KviFileUtils.h"
#include "KviQString.h"
#include "KviApplication.h"
#include "KviLocale.h"
#include "KviConsoleWindow.h"
#include "KviOptions.h"
#include "kvi_out.h"

#include <QFile>
#include <QClipboard>

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
#include <unistd.h>
#endif

KviPointerList<SlowPasteController> * g_pControllerList = nullptr;
int ctrlId = 0;

static SlowPasteController * spaste_find_controller(KviWindow * w)
{
	for(SlowPasteController * spc = g_pControllerList->first(); spc; spc = g_pControllerList->next())
	{
		if(spc->window() == w)
			return spc;
	}
	return nullptr;
}

static KviWindow * spaste_kvs_find_window(QString & win, KviKvsModuleCommandCall * c)
{
	KviWindow * w;
	if(!win.isEmpty())
		w = g_pApp->findWindow(win);
	else
		w = c->window();
	if(!w)
	{
		c->warning(__tr("Window with ID '%Q' not found"), &win);
		return nullptr;
	}
	if((w->type() == KviWindow::Channel) || (w->type() == KviWindow::Query) || (w->type() == KviWindow::DccChat))
		return w;
	c->warning(__tr2qs("The specified window (%Q) is not a channel/query/DCC chat"), &win);
	return nullptr;
}

/*
	@doc: spaste.file
	@type:
		command
	@title:
		spaste.file
	@short:
		Sends the contents of a file to a window, with a delay between each line
	@syntax:
		spaste.file <filename:string> [window:string]
	@description:
		Sends the contents of a file to a conversation window doing a pause between each line. [br]
		the optional window parameter must be a channel, query or DCC chat window, if [br]
		no window is specified the text will be send to the current window.
	@seealso:
		[cmd]spaste.clipboard[/cmd],
		[cmd]spaste.stop[/cmd],
		[cmd]spaste.list[/cmd],
		[cmd]spaste.setdelay[/cmd]
*/

static bool spaste_kvs_cmd_file(KviKvsModuleCommandCall * c)
{
	QString szFile, szWindow;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("file name", KVS_PT_STRING, 0, szFile)
	KVSM_PARAMETER("window", KVS_PT_STRING, KVS_PF_OPTIONAL, szWindow)
	KVSM_PARAMETERS_END(c)

	KviWindow * window = spaste_kvs_find_window(szWindow, c);
	if(!window)
		return false;

	// FIXME
	if(szFile.isEmpty() || (!KviFileUtils::fileExists(szFile)))
	{
		c->warning(__tr2qs("File not found or empty"));
		return false;
	}

	QFile tmp(szFile);
	if(!tmp.open(QIODevice::ReadOnly))
	{
		c->warning(__tr2qs("I can't open that file"));
		return false;
	}
	tmp.close();

	SlowPasteController * controller = spaste_find_controller(window);
	if(!controller)
		controller = new SlowPasteController(window, ++ctrlId);
	if(!controller->pasteFileInit(szFile))
	{
		c->warning(__tr2qs("Could not paste file"));
		return false;
	}
	return true;
}

/*
	@doc: spaste.clipboard
	@type:
		command
	@title:
		spaste.clipboard
	@short:
		Sends the contents of the clipboard to a window, pausing between each line
	@syntax:
		spaste.clipboard [window:string]
	@description:
		Sends the contents of the clipboard to a conversation window, with a delay between each line. [br]
		the optional window parameter must be a channel, query or DCC chat window. [br]
		If no window is specified, the text will be sent to the current window.
	@seealso:
		[cmd]spaste.file[/cmd],
		[cmd]spaste.stop[/cmd],
		[cmd]spaste.list[/cmd],
		[cmd]spaste.setdelay[/cmd]
*/

///////////////////////////////////////////////////////////////////////////////
// spaste.clipboard
///////////////////////////////////////////////////////////////////////////////

static bool spaste_kvs_cmd_clipboard(KviKvsModuleCommandCall * c)
{
	QString szWindow;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window", KVS_PT_STRING, KVS_PF_OPTIONAL, szWindow)
	KVSM_PARAMETERS_END(c)
	KviWindow * window = spaste_kvs_find_window(szWindow, c);
	if(!window)
		return false;

	SlowPasteController * controller = spaste_find_controller(window);
	if(!controller)
		controller = new SlowPasteController(window, ++ctrlId);
	controller->pasteClipboardInit();
	return true;
}

/*
	@doc: spaste.stop
	@type:
		command
	@title:
		spaste.stop
	@short:
		Stops one or more slow-paste process.
	@syntax:
		spaste.stop [-a] [id:integer]
	@description:
		This commands stop one or more slow-paste processes. It can operate in three ways. The first, [br]
		without any parameter or switch, stops all slow-paste processes running in the same window [br]
		as the command. The second, using the -a switch, stops all slow paste processes running [br]
		on all windows. The third form, without the switch and specifying a numerical slow paste process ID [br]
		(which you can obtain with the [cmd]spaste.list[/cmd] command), stops only that process ID.
		@switches:
			!sw: -a | --all
			Stops all slow paste processes running
			@seealso:
		[cmd]spaste.clipboard[/cmd],
		[cmd]spaste.file[/cmd],
		[cmd]spaste.list[/cmd],
		[cmd]spaste.setdelay[/cmd]

*/

///////////////////////////////////////////////////////////////////////////////
// spaste.stop
///////////////////////////////////////////////////////////////////////////////

static bool spaste_kvs_cmd_stop(KviKvsModuleCommandCall * c)
{
	kvs_uint_t iId = 0;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("delay", KVS_PT_UNSIGNEDINTEGER, KVS_PF_OPTIONAL, iId)
	KVSM_PARAMETERS_END(c)

	if(c->hasSwitch('a', "all")) //Delete all spaste's
	{
		while(g_pControllerList->first())
			delete g_pControllerList->first();
		return true;
	}

	KviPointerListIterator<SlowPasteController> it(*g_pControllerList);
	SlowPasteController * item;

	if(!iId) //Delete all spaste's from the current window
	{
		if((c->window()->type() != KviWindow::Channel) && (c->window()->type() != KviWindow::Query) && (c->window()->type() != KviWindow::DccChat) && (c->window()->type() != KviWindow::DeadChannel) && (c->window()->type() != KviWindow::DeadQuery))
		{
			QString szWinId = c->window()->id();
			c->warning(__tr2qs("The specified window (%Q) is not a Channel/Query/DCC"), &szWinId);
			return false;
		}
		else
		{
			while((item = it.current()) != nullptr)
			{
				++it;
				if(KviQString::equalCS(item->window()->id(), c->window()->id()))
					delete item;
			}
		}
	}
	else
	{
		//Delete the spaste with the given id
		while((item = it.current()) != nullptr)
		{
			++it;
			if(item->getId() == (kvs_int_t)iId)
				delete item;
		}
	}
	return true;
}

/*
	@doc: spaste.list
	@type:
		command
	@title:
		spaste.list
	@short:
		Lists all the running spaste processes.
	@syntax:
		spaste.list
	@description:
		This command will list in the window where it is executed all the current slow-paste [br]
		processes, their identification numbers, and the windows where they are running. [br]
	@seealso:
		[cmd]spaste.clipboard[/cmd],
		[cmd]spaste.file[/cmd],
		[cmd]spaste.stop[/cmd],
		[cmd]spaste.setdelay[/cmd]
*/

static bool spaste_kvs_cmd_list(KviKvsModuleCommandCall * c)
{
	KviPointerListIterator<SlowPasteController> it(*g_pControllerList);
	SlowPasteController * item;

	while((item = it.current()) != nullptr)
	{
		++it;
		QString szWinId = item->window()->id();
		c->window()->output(KVI_OUT_NONE, __tr2qs("Slow-paste ID:%d Window:%Q"), item->getId(), &szWinId);
	}
	return true;
}

/*
	@doc: spaste.setdelay
	@type:
		command
	@title:
		spaste.setdelay
	@short:
		Sets the delay time in milliseconds for the spaste module command delay
	@syntax:
		spaste.setdelay <time_in_msecs:integer>
	@description:
		Sets the delay time in milliseconds for the spaste module command delay.
	@seealso:
		[cmd]spaste.clipboard[/cmd],
		[cmd]spaste.file[/cmd],
		[cmd]spaste.stop[/cmd],
		[cmd]spaste.list[/cmd]
*/

static bool spaste_kvs_cmd_setdelay(KviKvsModuleCommandCall * c)
{
	kvs_int_t delay;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("delay", KVS_PT_INTEGER, 0, delay)
	KVSM_PARAMETERS_END(c)
	KVI_OPTION_UINT(KviOption_uintPasteDelay) = delay;
	return true;
}

static bool spaste_module_init(KviModule * m)
{
	g_pControllerList = new KviPointerList<SlowPasteController>;
	g_pControllerList->setAutoDelete(false);

	KVSM_REGISTER_SIMPLE_COMMAND(m, "file", spaste_kvs_cmd_file);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "clipboard", spaste_kvs_cmd_clipboard);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setdelay", spaste_kvs_cmd_setdelay);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "stop", spaste_kvs_cmd_stop);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "list", spaste_kvs_cmd_list);
	return true;
}

static bool spaste_module_cleanup(KviModule *)
{
	while(g_pControllerList->first())
		delete g_pControllerList->first();
	delete g_pControllerList;
	g_pControllerList = nullptr;

	return true;
}

static bool spaste_module_can_unload(KviModule *)
{
	return (g_pControllerList->isEmpty());
}

KVIRC_MODULE(
    "SPaste",                                     // module name
    "4.0.0",                                      // module version
    "(C) 2002 Juanjo Alvarez (juanjux@yahoo.es)", // author & (C)
    "Delayed paste commands",
    spaste_module_init,
    spaste_module_can_unload,
    0,
    spaste_module_cleanup,
    0)
