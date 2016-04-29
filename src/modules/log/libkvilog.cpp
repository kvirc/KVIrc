//==============================================================================
//
//   File : libkvilog.cpp
//   Creation date : Sat Jan 06 2001 13:28:02 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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
//==============================================================================

#include "../logview/LogFile.h"

#include "KviWindow.h"
#include "KviLocale.h"
#include "KviIrcView.h"
#include "KviModule.h"
#include "KviModuleManager.h"

//#warning "log.stats"
//#warning "log.compress (gzip -r the log directory)"

/*
	@doc: log
	@title:
		The log interface module
	@short:
		Interface to the KVirc log subsystem
	@type:
		module
	@keyterms:
		logging
	@body:
		The log module is an interface to the KVIrc logging subsystem.[br]
		It provides commands and functions that allow the management of
		logging in the KVIrc windows:[br]
		[cmd]log.start[/cmd], [cmd]log.stop[/cmd], [cmd]log.flush[/cmd],
		[fnc]$log.file[/fnc]().[br]
		NOTE:[br]
		If you want to [b]auto enable[/b] logging in certain types of windows
		you have to use the [cmd]option[/cmd] command.[br]
		For example, [b]option boolAutoLogQueries 1[/b] enables logging in all the newly created
		query windows.
*/

/*
	@doc: log.start
	@type:
		command
	@title:
		log.start
	@keyterms:
		logging
	@short:
		Starts logging in a specified window
	@syntax:
		log.start [-w = <window_id>] [-p] [filename]
	@description:
		Starts logging in the current window or in the window specified by the -w switch.[br]
		If a logging session is already running in the specified window, it is stopped first.[br]
		If [filename] is specified, this filename is used as log file, otherwise a default
		filename is used.[br]
		If the -p switch is used, all the already existing contents of the window are
		stored to the log file before starting the log action.[br]
		Please note that some windows do not have logging capabilities...[br]
		This command will print a warning in that case.[br]
	@seealso:
		[fnc]$window[/fnc],
		[cmd]log.stop[/cmd],
		[fnc]$log.file[/fnc],
		[doc:window_naming_conventions]window naming conventions documentation[/doc]
*/
static bool log_kvs_cmd_start(KviKvsModuleCommandCall * c)
{
	QString szFile;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("filename", KVS_PT_STRING, KVS_PF_OPTIONAL, szFile)
	KVSM_PARAMETERS_END(c)

	KviWindow * pWnd = c->window();
	if(c->hasSwitch('w', "window"))
	{
		QString szWindow;
		if(c->switches()->getAsStringIfExisting('w', "window", szWindow))
		{
			pWnd = g_pApp->findWindow(szWindow);
			if(!pWnd)
			{
				c->warning(__tr2qs_ctx("Window '%1' not found", "log").arg(szWindow));
				return true;
			}
		}
		else
		{
			c->warning(__tr2qs_ctx("Missing window ID after the 'w' switch", "log"));
			return true;
		}
	}

	if(pWnd->view())
	{
		if(szFile.isEmpty())
			pWnd->getDefaultLogFileName(szFile);

		if(!pWnd->view()->startLogging(szFile, c->hasSwitch('p', "log-buffer")))
			c->warning(__tr2qs_ctx("Can't log to file '%1'", "log").arg(szFile));
	}
	else
	{
		c->warning(__tr2qs_ctx("This window has no logging capabilities", "log"));
		return true;
	}
	return true;
}

/*
	@doc: log.stop
	@type:
		command
	@title:
		log.stop
	@keyterms:
		logging
	@short:
		Stops logging in a specified window
	@syntax:
		log.stop [-w = <window_id>]
	@description:
		Stops logging in the current window or in the window specified by the -w switch.[br]
		If logging is not enabled in the specified window, this command does nothing.[br]
	@seealso:
		[fnc]$window[/fnc],
		[cmd]log.start[/cmd],
		[doc:window_naming_conventions]window naming conventions documentation[/doc]
*/
static bool log_kvs_cmd_stop(KviKvsModuleCommandCall * c)
{
	KviWindow * pWnd = c->window();
	if(c->hasSwitch('w', "window"))
	{
		QString szWindow;
		if(c->switches()->getAsStringIfExisting('w', "window", szWindow))
		{
			pWnd = g_pApp->findWindow(szWindow);
			if(!pWnd)
			{
				c->warning(__tr2qs_ctx("Window '%1' not found", "log").arg(szWindow));
				return true;
			}
		}
		else
		{
			c->warning(__tr2qs_ctx("Missing window ID after the 'w' switch", "log"));
			return true;
		}
	}

	if(pWnd->view())
		pWnd->view()->stopLogging();
	return true;
}

/*
	@doc: log.flush
	@type:
		command
	@title:
		log.flush
	@keyterms:
		logging
	@short:
		Flushes the log file for a specified window
	@syntax:
		log.flush [-w = <window_id>]
	@description:
		Flushes the log file the current window or in the window specified by the -w switch.[br]
		If logging is not enabled in the specified window, this command does nothing.[br]
		If this command is never used, the log file is flushed at regular file-size intervals,
		depending on the underlying I/O subsystem. This is usually 4KiB.[br]
		The [i]block flushing[/i] is used to achieve a fast I/O on files, but may cause
		the last data block to be lost in case of a program crash.[br]
	@seealso:
		[fnc]$window[/fnc],
		[cmd]log.start[/cmd],
		[cmd]log.stop[/cmd],
		[doc:window_naming_conventions]window naming conventions documentation[/doc]
*/
static bool log_kvs_cmd_flush(KviKvsModuleCommandCall * c)
{
	KviWindow * pWnd = c->window();
	if(c->hasSwitch('w', "window"))
	{
		QString szWindow;
		if(c->switches()->getAsStringIfExisting('w', "window", szWindow))
		{
			pWnd = g_pApp->findWindow(szWindow);
			if(!pWnd)
			{
				c->warning(__tr2qs_ctx("Window '%1' not found", "log").arg(szWindow));
				return true;
			}
		}
		else
		{
			c->warning(__tr2qs_ctx("Missing window ID after the 'w' switch", "log"));
			return true;
		}
	}

	if(pWnd->view())
		pWnd->view()->flushLog();
	return true;
}

/*
	@doc: log.file
	@type:
		function
	@title:
		$log.file
	@short:
		Returns the current log filename
	@syntax:
		$log.file
		$log.file(<window id>)
	@description:
		The form with the <window id> parameter returns the log file name
		of the window that has the specified ID. If logging is not enabled
		in the specified window, this function returns an empty string.[br]
		The form without parameters returns the log file name of the current window,
		thus it is equivalent to calling $log.file([fnc]$window[/fnc])
	@examples:
		[example]
		[cmd]log.start[/cmd] mylogfilename.log
		[cmd]echo[/cmd] $log.file
		[cmd]echo[/cmd] $log.file([fnc]$channel[/fnc](#kvirc))
		[/example]
	@seealso:
		[fnc]$window[/fnc],
		[cmd]log.start[/cmd],
		[cmd]log.stop[/cmd],
		[doc:window_naming_conventions]window naming conventions documentation[/doc]
*/
static bool log_kvs_fnc_file(KviKvsModuleFunctionCall * c)
{
	QString szWindow, szBuffer;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szWindow)
	KVSM_PARAMETERS_END(c)

	KviWindow * pWnd = c->window();

	if(!szWindow.isEmpty())
	{
		pWnd = g_pApp->findWindow(szWindow);
		if(!pWnd)
		{
			c->warning(__tr2qs_ctx("Window with ID '%1' not found, returning empty string", "log").arg(szWindow));
			return true;
		}
	}

	if(pWnd->view())
		pWnd->view()->getLogFileName(szBuffer);
	c->returnValue()->setString(szBuffer);
	return true;
}

/*
	@doc: log.export
	@type:
		function
	@title:
		$log.export
	@short:
		Exports the specified log to the given format and returns the filename
	@syntax:
		<string> $log.export(<filename:string>[,<type:string>])
	@description:
		Exports the log file named <filename> to the format passed as <type>.[br]
		<filename> has to be a valid path, or given using [fnc]$log.file[/fnc] function.[br]
		If no <type> is passed, plain text will be used.
	@example:
		[example]
		[cmd]echo[/cmd] $log.export([fnc]$log.file[/fnc],"html")
		[/example]
	@seealso:
		[cmd]log.start[/cmd],
		[cmd]log.stop[/cmd]
*/
static bool log_kvs_fnc_export(KviKvsModuleFunctionCall * c)
{
	QString szFile, szType;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("filename", KVS_PT_NONEMPTYSTRING, 0, szFile)
	KVSM_PARAMETER("type", KVS_PT_STRING, KVS_PF_OPTIONAL, szType)
	KVSM_PARAMETERS_END(c)

	if(szType.isEmpty())
		szType = "txt";

	KviModule * m = g_pModuleManager->getModule("logview");
	if(!m)
	{
		c->error(__tr2qs_ctx("Failed to load logview module, aborting", "log"));
		return false;
	}

	LogFileData log;
	log.szName = szFile;
	log.szType = szType;

	if(!m->ctrl("logview::export", (void *)&log))
	{
		c->error(__tr2qs_ctx("Failed to export the log '%1'", "log").arg(szFile));
		return false;
	}

	c->returnValue()->setString(log.szFile);

	return true;
}

static bool log_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m, "start", log_kvs_cmd_start);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "stop", log_kvs_cmd_stop);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "flush", log_kvs_cmd_flush);

	KVSM_REGISTER_FUNCTION(m, "file", log_kvs_fnc_file);
	KVSM_REGISTER_FUNCTION(m, "export", log_kvs_fnc_export);
	return true;
}

static bool log_module_cleanup(KviModule *)
{
	return true;
}

KVIRC_MODULE(
    "Log",   // module name
    "4.0.0", // module version
    "Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)\n"
    "	2011 Elvio Basello (hellvis69 at gmail dot com)", // author & (C)
    "User interface to the logging system for KVIrc",
    log_module_init,
    0,
    0,
    log_module_cleanup,
    0)
