//=============================================================================
//
//   File : libkviwindow.cpp
//   Creation date : Sat Sep 01 2001 17:13:12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "UserWindow.h"

#include "KviModule.h"
#include "KviConsoleWindow.h"
#include "KviOptions.h"
#include "KviIrcSocket.h"
#include "KviMainWindow.h"
#include "KviLocale.h"
#include "KviApplication.h"
#include "KviError.h"
#include "KviIrcView.h"
#include "KviInput.h"
#include "KviIconManager.h"
#include "KviModuleManager.h"
#include "KviMemory.h"
#include "KviChannelWindow.h"

#include <QTimer>
#include <map>
#include <vector>

#ifdef COMPILE_CRYPT_SUPPORT
#include "KviCryptEngine.h"
#include "KviCryptController.h"
// KviApplication.cpp
extern KVIRC_API KviCryptEngineManager * g_pCryptEngineManager;
#endif

// KviApplication.cpp
extern KVIRC_API std::map<QString, KviWindow *> g_pGlobalWindowDict;
std::vector<UserWindow *> g_pUserWindowList;

// $window.caption $window.x $window.y $window.width $window.height $window.isActive $window.type
// $window.input.text $window.input.cursorpos $window.input.textlen

#define GET_KVS_WINDOW_ID                                                                            \
	QString szWnd;                                                                                   \
	KviWindow * pWnd;                                                                                \
	KVSM_PARAMETERS_BEGIN(c)                                                                         \
	KVSM_PARAMETER("window_id", KVS_PT_STRING, KVS_PF_OPTIONAL, szWnd)                               \
	KVSM_PARAMETERS_END(c)                                                                           \
	if(c->parameterList()->count() == 0)                                                             \
	{                                                                                                \
		pWnd = c->window();                                                                          \
	}                                                                                                \
	else                                                                                             \
	{                                                                                                \
		pWnd = g_pApp->findWindow(szWnd.toUtf8().data());                                            \
		if(!pWnd)                                                                                    \
		{                                                                                            \
			if(!c->hasSwitch('q', "quiet"))                                                          \
				c->warning(__tr2qs("The window with ID '%s' doesn't exist"), szWnd.toUtf8().data()); \
			return true;                                                                             \
		}                                                                                            \
	}

#define GET_KVS_FNC_WINDOW_ID                                          \
	QString szWnd;                                                     \
	KviWindow * pWnd;                                                  \
	KVSM_PARAMETERS_BEGIN(c)                                           \
	KVSM_PARAMETER("window_id", KVS_PT_STRING, KVS_PF_OPTIONAL, szWnd) \
	KVSM_PARAMETERS_END(c)                                             \
	if(c->parameterList()->count() == 0)                               \
	{                                                                  \
		pWnd = c->window();                                            \
	}                                                                  \
	else                                                               \
	{                                                                  \
		pWnd = g_pApp->findWindow(szWnd.toUtf8().data());              \
		if(!pWnd)                                                      \
			return true;                                               \
	}

/*
	@doc: window.clearOutput
	@type:
		command
	@title:
		window.clearOutput
	@short:
		Clears the output a window
	@syntax:
		window.clearOutput [-q] [window_id]
	@switches:
		!sw: -q | --quiet
		Don't warn if the specified window doesn't exist. Just continue silently.
	@description:
		Clears the text output of the window specified by window_id. If window_id is missing then
		the current window is used. If the window has no text output then no operation is performed.
		If the specified window does not exist a warning is printed (unless the -q switch is used)
		but the execution continues normally.
	@seealso:
		[fnc]$window.hasOutput[/fnc]
*/

static bool window_kvs_cmd_clearOutput(KviKvsModuleCommandCall * c)
{
	GET_KVS_WINDOW_ID
	if(pWnd)
	{
		if(pWnd->view())
			pWnd->view()->clearBuffer();
		if(pWnd->type() == KviWindow::Channel)
		{
			KviChannelWindow * chan = (KviChannelWindow *)pWnd;
			if(chan->messageView())
				chan->messageView()->clearBuffer();
		}
	}
	return true;
}

/*
	@doc: window.close
	@type:
		command
	@title:
		window.close
	@short:
		Closes a window
	@syntax:
		window.close [-q] [window_id]
	@description:
		Closes the window specified by window_id. If window_id is missing then
		the current window is closed. The close operation is asynchronous: it is
		performed immediately after the script has terminated the execution and
		the control is returned to the main KVIrc core. If the specified window
		does not exist a warning is printed unless the -q switch is used.
*/

static bool window_kvs_cmd_close(KviKvsModuleCommandCall * c)
{
	GET_KVS_WINDOW_ID
	if(pWnd)
	{
		pWnd->delayedClose();
	}
	return true;
}

/*
	@doc: window.dock
	@type:
		command
	@title:
		window.dock
	@short:
		Docks a window
	@syntax:
		window.dock [-q] [window_id]
	@description:
		Docks the window specified by window_id. If window_id is missing then
		the current window is docked. If the specified window was already docked then
		no operation is performed. If the specified window
		does not exist a warning is printed unless the -q switch is used.
	@seealso:
		[cmd]window.undock[/cmd], [fnc]$window.isDocked[/fnc]
*/

static bool window_kvs_cmd_dock(KviKvsModuleCommandCall * c)
{
	GET_KVS_WINDOW_ID
	if(pWnd)
	{
		pWnd->dock();
	}
	return true;
}

/*
	@doc: window.undock
	@type:
		command
	@title:
		window.undock
	@short:
		Undocks a window
	@syntax:
		window.undock [-q] [window_id]
	@description:
		Undocks the window specified by window_id. If window_id is missing then
		the current window is undocked. If the specified window was already undocked then
		no operation is performed. If the specified window
		does not exist a warning is printed unless the -q switch is used.
	@seealso:
		[cmd]window.dock[/cmd], [fnc]$window.isDocked[/fnc]
*/

static bool window_kvs_cmd_undock(KviKvsModuleCommandCall * c)
{
	GET_KVS_WINDOW_ID
	if(pWnd)
	{
		pWnd->undock();
	}
	return true;
}

/*
	@doc: window.splitView
	@type:
		command
	@title:
		window.splitView
	@short:
		Splits the view of a channel window
	@syntax:
		window.splitView [-q] [window_id]
	@description:
		Splits the view of a channel specified by window_id. If window_id is missing then
		the current window is split. If the specified window was already split then no
		operation is performed. If the specified window does not exist or is not a channel
		window then a warning is printed unless the -q switch is used.
	@seealso:
		[cmd]window.unsplitView[/cmd], [fnc]$window.isSplitView[/fnc]
*/

static bool window_kvs_cmd_splitView(KviKvsModuleCommandCall * c)
{
	GET_KVS_WINDOW_ID
	if(pWnd && pWnd->type() == KviWindow::Channel)
	{
		KviChannelWindow * chan = (KviChannelWindow *)pWnd;
		if(!chan->messageView())
			chan->toggleDoubleView();
	}
	else
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs("The window with ID '%s' isn't a channel window."), szWnd.toUtf8().data());
	}
	return true;
}

/*
	@doc: window.unsplitView
	@type:
		command
	@title:
		window.unsplitView
	@short:
		Unsplits the view of a channel window
	@syntax:
		window.splitView [-q] [window_id]
	@description:
		Unsplits the view of a channel specified by window_id. If window_id is missing then
		the current window is unsplit. If the specified window wasn't already split then no
		operation is performed. If the specified window does not exist or is not a channel
		window then a warning is printed unless the -q switch is used.
	@seealso:
		[cmd]window.unsplitView[/cmd], [fnc]$window.isSplitView[/fnc]
*/

static bool window_kvs_cmd_unsplitView(KviKvsModuleCommandCall * c)
{
	GET_KVS_WINDOW_ID
	if(pWnd && pWnd->type() == KviWindow::Channel)
	{
		KviChannelWindow * chan = (KviChannelWindow *)pWnd;
		if(chan->messageView())
			chan->toggleDoubleView();
	}
	else
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs("The window with ID '%s' isn't a channel window."), szWnd.toUtf8().data());
	}
	return true;
}

/*
	@doc: window.activate
	@type:
		command
	@title:
		window.activate
	@short:
		Activates a window
	@syntax:
		window.activate [-q] [window_id]
	@description:
		Activates the window specified by window_id. If window_id is missing then
		the current window is activated. If the specified window
		does not exist a warning is printed unless the -q switch is used.
		Please note that if the window is currently docked to a frame then this
		command will [b]not[/b] raise the frame window. If you're interested in
		the user's attention then you might be interested in [cmd]window.demandAttention[/cmd].
	@seealso:
		[cmd]window.demandAttention[/cmd]
*/

static bool window_kvs_cmd_activate(KviKvsModuleCommandCall * c)
{
	GET_KVS_WINDOW_ID
	if(pWnd)
	{
		pWnd->autoRaise();
	}
	return true;
}

/*
	@doc: window.demandAttention
	@type:
		command
	@title:
		window.demandAttention
	@short:
		Flashes a window's system taskbar entry
	@syntax:
		window.demandAttention [-q] [window_id]
	@description:
		Flashes the system taskbar entry of the window
		specified by the window_id. If window_id is missing then
		the current window's system taskbar entry is flashed. If the specified window
		does not exist a warning is printed unless the -q switch is used.
		If the window is currently docked in a frame then the frame's
		system taskbar entry will be flashed.
		Please note that this command is highly system dependent:
		on systems that do not have a system taskbar or there
		is no way to flash an entry this command will do nothing.
		At the time of writing this command works flawlessly on
		Windows and in KDE compilations.
	@seealso:
		[cmd]window.demandAttention[/cmd]
*/

static bool window_kvs_cmd_demandAttention(KviKvsModuleCommandCall * c)
{
	GET_KVS_WINDOW_ID
	if(pWnd)
	{
		pWnd->demandAttention();
	}
	return true;
}

/*
	@doc: window.activityLevel
	@type:
		function
	@title:
		$window.activityLevel
	@short:
		Returns the current activity level of a window
	@syntax:
		$window.activityLevel
		$window.activityLevel(<window_id>)
	@description:
		Returns the current activity level of the window specified by <window_id>.
		The form without parameters works on the current window.[br]
		The activity level is a number describing the level of traffic in the window
		and depends on the window type. On channels and queries it is dependent on the number
		and frequency of actions performed by the users.
	@seealso:
		[fnc]$window.activityTemperature[/fnc]
*/

static bool window_kvs_fnc_activityLevel(KviKvsModuleFunctionCall * c)
{
	GET_KVS_FNC_WINDOW_ID
	if(pWnd)
	{
		unsigned int v, t;
		pWnd->activityMeter(&v, &t);
		c->returnValue()->setInteger(v);
	}
	else
	{
		c->returnValue()->setInteger(0);
	}
	return true;
}

/*
	@doc: window.activityTemperature
	@type:
		function
	@title:
		$window.activityTemperature
	@short:
		Returns the current activity temperature of a window
	@syntax:
		$window.activityTemperature
		$window.activityTemperature(<window_id>)
	@description:
		Returns the current activity temperature of the window specified by <window_id>.
		The form without parameters works on the current window.[br]
		The activity temperature describes the type of traffic in the window and is
		strictly related to the [fnc]$window.activityLevel[/fnc].[br]
		On channels and queries the temperature describes the type of the actions
		performed by the users. High temperatures denote more "human" behaviour (like
		speaking to the channel, changing the topic etc...),
		low temperatures denote automatic behaviours (like changing the channel limit
		or mode: actions often performed by bots).[br]
	@seealso:
		[fnc]$window.activityLevel[/fnc]
*/

static bool window_kvs_fnc_activityTemperature(KviKvsModuleFunctionCall * c)
{
	GET_KVS_FNC_WINDOW_ID
	if(pWnd)
	{
		unsigned int v, t;
		pWnd->activityMeter(&v, &t);
		c->returnValue()->setInteger(t);
	}
	else
	{
		c->returnValue()->setInteger(0);
	}
	return true;
}

/*
	@doc: window.isDocked
	@type:
		function
	@title:
		$window.isDocked
	@short:
		Checks if a window is currently docked
	@syntax:
		$window.isDocked
		$window.isDocked(<window_id>)
	@description:
		Returns [b]1[/b] if the window specified by <window_id> is currently docked and [b]0[/b] otherwise.
		The form with no parameters works on the current window. If the specified window
		doesn't exist then 0 is returned.
	@seealso:
		[cmd]window.dock[/cmd], [cmd]window.undock[/cmd]
*/

static bool window_kvs_fnc_isDocked(KviKvsModuleFunctionCall * c)
{
	GET_KVS_FNC_WINDOW_ID
	if(pWnd)
	{
		c->returnValue()->setBoolean(pWnd->parentWidget() ? true : false);
	}
	return true;
}

/*
	@doc: window.isSplitView
	@type:
		function
	@title:
		$window.isSplitView
	@short:
		Checks if a window is currently in split view mode
	@syntax:
		$window.isSplitView
		$window.isSplitView(<window_id>)
	@description:
		Returns [b]1[/b] if the window specified by <window_id> is in split view mode and [b]0[/b] otherwise.
		This is only intended to apply to channel windows. If the specified window doesn't exist
		or is not a channel window then 0 is returned.
	@seealso:
		[cmd]window.splitView[/cmd], [cmd]window.unsplitView[/cmd]
*/
static bool window_kvs_fnc_isSplitView(KviKvsModuleFunctionCall * c)
{
	c->returnValue()->setBoolean(false);
	GET_KVS_FNC_WINDOW_ID
	if(pWnd && pWnd->type() == KviWindow::Channel)
		c->returnValue()->setBoolean(((KviChannelWindow *)pWnd)->messageView() ? true : false);
	return true;
}

/*
	@doc: window.hasInput
	@type:
		function
	@title:
		$window.hasInput
	@short:
		Checks if a window has an input field
	@syntax:
		$window.hasInput
		$window.hasInput(<window_id>)
	@description:
		Returns [b]1[/b] if the window specified by <window_id> has an input field and [b]0[/b] otherwise.
		The form with no parameters works on the current window. If the specified window
		doesn't exist then 0 is returned.
	@seealso:
		[fnc]$window.hasOutput[/fnc]
*/

static bool window_kvs_fnc_hasInput(KviKvsModuleFunctionCall * c)
{
	GET_KVS_FNC_WINDOW_ID
	if(pWnd)
	{
		c->returnValue()->setBoolean(pWnd->input() ? true : false);
	}
	else
		c->returnValue()->setBoolean(false);
	return true;
}

/*
	@doc: window.hasUserFocus
	@type:
		function
	@title:
		$window.hasUserFocus
	@short:
		Checks if a window has the user focus
	@syntax:
		$window.hasUserFocus
		$window.hasUserFocus(<window_id>)
	@description:
		Returns [b]1[/b] if the window specified by <window_id> has
		currently the user focus and [b]0[/b] otherwise.
		The form with no parameters works on the current window.
		If the specified window doesn't exist then 0 is returned.
		A window has the user focus if it is the KVIrc's active
		window and has the user's input focus (i.e. typing
		on the keyboard will write in this window).
	@seealso:
*/

static bool window_kvs_fnc_hasUserFocus(KviKvsModuleFunctionCall * c)
{
	GET_KVS_FNC_WINDOW_ID
	if(pWnd)
	{
		bool b = (pWnd == g_pActiveWindow) && pWnd->isActiveWindow();
		c->returnValue()->setBoolean(b ? true : false);
	}
	else
		c->returnValue()->setBoolean(false);
	return true;
}

/*
	@doc: window.console
	@type:
		function
	@title:
		$window.console
	@short:
		Returns the console that a window is attached to
	@syntax:
		$window.console
		$window.console(<window_id>)
	@description:
		Returns the ID of the console window that the window specified by window_id is attached to.
		The console is the main (and only) console of the IRC context. If window_id is missing then
		the current window is used. If this window does not belong to an IRC context (and thus has
		no attached console) then 0 is returned.
	@seealso:
*/

static bool window_kvs_fnc_console(KviKvsModuleFunctionCall * c)
{
	GET_KVS_FNC_WINDOW_ID
	if(pWnd)
	{
		c->returnValue()->setInteger(pWnd->console() ? QString(pWnd->console()->id()).toInt() : 0);
	}
	return true;
}

/*
	@doc: window.hasOutput
	@type:
		function
	@title:
		$window.hasOutput
	@short:
		Checks if a window has a text output widget
	@syntax:
		$window.hasOutput
		$window.hasOutput(<window_id>)
	@description:
		Returns [b]1[/b] if the window specified by <window_id> has a text output widget and [b]0[/b] otherwise.
		The form with no parameters works on the current window. If the specified window
		doesn't exist then 0 is returned.
	@seealso:
		[fnc]$window.hasInput[/fnc]
*/

static bool window_kvs_fnc_hasOutput(KviKvsModuleFunctionCall * c)
{
	GET_KVS_FNC_WINDOW_ID
	if(pWnd)
	{
		c->returnValue()->setBoolean(pWnd->view() ? true : false);
	}
	else
		c->returnValue()->setBoolean(false);
	return true;
}

/*
	@doc: window.exists
	@type:
		function
	@title:
		$window.exists
	@short:
		Checks for the existence of a window
	@syntax:
		$window.exists(<window_id>)
	@description:
		Returns 1 if a specified window exists
	@seealso:
*/

static bool window_kvs_fnc_exists(KviKvsModuleFunctionCall * c)
{
	GET_KVS_FNC_WINDOW_ID
	if(pWnd)
	{
		c->returnValue()->setBoolean(true);
	}
	else
		c->returnValue()->setBoolean(false);
	return true;
}

/*
	@doc: window.highlight
	@type:
		command
	@title:
		window.highlight
	@short:
		Sets the highlight (alert) level of a window
	@syntax:
		window.highlight [-q] <level> [window_id]
	@switches:
		!sw: -q | --quiet
		Be quiet
	@description:
		Sets the highlight the user window specified by [window_id] to <level>.[br]
		If <window_id> is an empty string then the current window is assumed.[br]
		If the specified window does not exist a warning is printed unless the -q switch is used.[br]
		For more infos on this feature read the documentation about [fnc]$window.highlightLevel[/fnc].
	@seealso:
		[fnc]$window.highlightLevel[/fnc]
*/

static bool window_kvs_cmd_highlight(KviKvsModuleCommandCall * c)
{
	QString szWnd;
	KviWindow * pWnd;
	kvs_uint_t level;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("level", KVS_PT_UINT, 0, level)
	KVSM_PARAMETER("window_id", KVS_PT_STRING, KVS_PF_OPTIONAL, szWnd)
	KVSM_PARAMETERS_END(c)
	if(c->parameterList()->count() == 1)
	{
		pWnd = c->window();
	}
	else
	{
		pWnd = g_pApp->findWindow(szWnd.toUtf8().data());
		if(!pWnd)
		{
			if(!c->hasSwitch('q', "quiet"))
				c->warning(__tr2qs("The window with ID '%s' doesn't exist"), szWnd.toUtf8().data());
			return true;
		}
	}

	//force the previous level to be lower
	pWnd->unhighlight();
	//level boundaries checking is done by the upstream function
	pWnd->highlightMe(level);
	return true;
}

/*
	@doc: window.highlightLevel
	@type:
		function
	@title:
		$window.highlightLevel
	@short:
		Returns the current highlight (alert) level of a window
	@syntax:
		$window.highlightLevel
		$window.highlightLevel(<window_id>)
	@description:
		Every window has a current alert level; it corresponds to an highlight color of that window in the window list.
		There are 6 defined levels, they start from 0 (normal) to 5 (max alarm level). The classic window list and the tree window list use different colors to represent these levels. Classic task bar uses options:
		[ul][li]colorWindowListNormalText : normal state[/li][li]colorWindowListHighlight1Text : highlight state 1[/li][li]...[/li][li]colorWindowListHighlight5Text : highlight state 5[/li][/ul]
		While the tree window list uses options:
		[ul][li]colorTreeWindowListForeground : normal state[/li][li]colorTreeWindowListHighlight1Foreground : highlight state 1[/li][li]...[/li][li]colorTreeWindowListHighlight5Foreground : highlight state 5[/li][/ul]
		You can use [fnc]$option[/fnc] to read these options and the [cmd]option[/cmd] command to set them.
	@seealso:
		[fnc]$window.activityTemperature[/fnc]
		[fnc]$window.activityLevel[/fnc]
		[fnc]$option[/fnc]
		[cmd]option[/cmd]
*/

static bool window_kvs_fnc_highlightLevel(KviKvsModuleFunctionCall * c)
{
	GET_KVS_FNC_WINDOW_ID
	if(pWnd)
	{
		unsigned int v;
		pWnd->highlightMeter(&v);
		c->returnValue()->setInteger(v);
	}
	else
	{
		c->returnValue()->setInteger(0);
	}
	return true;
}

/*
	@doc: window.type
	@type:
		function
	@title:
		$window.type
	@short:
		Returns the type of a window
	@syntax:
		$window.type
		$window.type(<window_id>)
	@description:
		Returns the type of the window with <window_id>.[br]
		The form with no parameters returns the type of the current window.[br]
		If the window with the specified ID does not exist, an empty string is returned.[br]
	@seealso:
*/

static bool window_kvs_fnc_type(KviKvsModuleFunctionCall * c)
{
	GET_KVS_FNC_WINDOW_ID
	if(pWnd)
	{
		c->returnValue()->setString(pWnd->typeString());
	}
	return true;
}

/*
	@doc: window.context
	@type:
		function
	@title:
		$window.context
	@short:
		Returns the IRC context of a window
	@syntax:
		$window.context
		$window.context(<window_id>)
	@description:
		Returns the IRC context of the window with the specified <window_id>.[br]
		The form with no parameters returns the IRC context of the current window.[br]
		If the window with the specified ID does not exist, an empty string is returned.[br]
	@seealso:
*/

static bool window_kvs_fnc_context(KviKvsModuleFunctionCall * c)
{
	GET_KVS_FNC_WINDOW_ID
	if(pWnd)
	{
		c->returnValue()->setInteger(pWnd->context() ? pWnd->context()->id() : 0);
	}
	return true;
}

/*
	@doc: window.caption
	@type:
		function
	@title:
		$window.caption
	@short:
		Returns the caption of a window
	@syntax:
		$window.caption
		$window.caption(<window_id>)
	@description:
		Returns the caption of the window with <window_id>.[br]
		The form with no parameters returns the caption of the current window.[br]
		If the window with the specified ID does not exist, an empty string is returned.[br]
	@seealso:
*/

static bool window_kvs_fnc_caption(KviKvsModuleFunctionCall * c)
{
	GET_KVS_FNC_WINDOW_ID
	if(pWnd)
	{
		c->returnValue()->setString(pWnd->plainTextCaption());
	}
	return true;
}

/*
	@doc: window.listtypes
	@type:
		command
	@title:
		window.listtypes
	@short:
		Lists available types of windows
	@syntax:
		window.listtypes
	@description:
		Lists the types of windows that are built in the current release of KVIrc.[br]
		This is actually a command and not a static list just because new window
		types may be added in subsequent releases.[br]
	@seealso:
		[cmd]window.listtypes[/cmd]
*/

static bool window_kvs_cmd_listtypes(KviKvsModuleCommandCall * c)
{
	c->window()->listWindowTypes();
	return true;
}

/*
	@doc: window.list
	@type:
		function
	@title:
		$window.list
	@short:
		Generates lists of windows
	@syntax:
		$window.list(<type>[,<irc_context_id>])
	@description:
		Returns an array of window identifiers with a specified type and eventually belonging to a specified
		irc context.[br]
		<type> is a window type such as 'query' or 'channel'.[br]
		See [cmd]window.listtypes[/cmd] for a list of available window types in this KVIrc release.[br]
		If <type> is the special word 'all', all the window types are listed.[br]
		<irc_context_id> specifies the IRC context in which the windows are searched.[br]
		If no <irc_context_id> is specified, the current one is used.[br]
		If <irc_context_id> is the special word 'all', all the IRC context are searched.[br]
		If <irc_context_id> is the special word 'none' then only windows not belonging to any
		irc context are listed.[br]
		The special word 'any' used as <irc_context_id> merges the effects of 'all' and 'none'
		by searching all the IRC contexts AND the windows not belonging to any IRC context.[br]
		The windows that do not belong to any IRC context (such as DCC windows), must be searched
		by using 'none' or 'any' as <irc_context_id>.
	@examples:
		[example]
			[comment]# List all the queries of the current IRC context[/comment]
			[cmd]echo[/cmd] $window.list(query)
			[comment]# Equivalent to the above[/comment]
			[cmd]echo[/cmd] $window.list(query,[fnc]$ic[/fnc])
			[comment]# List all the channels in all the IRC contexts[/comment]
			[cmd]echo[/cmd] $window.list(channel,all)
			[comment]# List all the windows in the current IRC context[/comment]
			[cmd]echo[/cmd] $window.list(all)
			[comment]# List all the windows in all IRC contexts[/comment]
			[cmd]echo[/cmd] $window.list(all,all)
			[comment]# List all the DCC Send windows: They don't belong to any IRC context[/comment]
			[cmd]echo[/cmd] $window.list(dcctransfer,none)
			[comment]# List all the user windows created with $window.open[/comment]
			[comment]# They may either belong to an IRC context or not[/comment]
			[cmd]echo[/cmd] $window.list(userwnd,any)
			[comment]# Ok, let's use it[/comment]
			[comment]# A nice alias that allows iterating commands through all the consoles[/comment]
			[comment]# Note the array returned by $window.list[/comment]
			[comment]# This is by LatinSuD :)[/comment]
			[cmd]alias[/cmd](iterate)
			{
				%ctxt[]=[fnc]$window.list[/fnc](console,all)
				[cmd]for[/cmd](%i=0;%i<%ctxt[]#;%i++)
				{
					[cmd]eval[/cmd] -r=%ctxt[%i] $0-
				}
			}
			iterate [cmd]echo[/cmd] Hi ppl! :)
			[comment]# The returned array works nicely also in [cmd]foreach[/cmd][/comment]
			[comment]# Say hi to all the channels :)[/comment]
			[cmd]alias[/cmd](sayallchans)
			{
				[cmd]foreach[/cmd](%x,[fnc]$window.list[/fnc](channel,all))
						[cmd]say[/cmd] -r=%x $0-;
			}
			sayallchans Hi ppl :)
		[/example]
	@seealso:
		[cmd]window.listtypes[/cmd]
*/

static bool window_kvs_fnc_list(KviKvsModuleFunctionCall * c)
{
	QString szType;
	QString szContext;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("type", KVS_PT_STRING, 0, szType)
	KVSM_PARAMETER("irc_context_id", KVS_PT_STRING, KVS_PF_OPTIONAL, szContext)
	KVSM_PARAMETERS_END(c)
	KviKvsArray * pArray = new KviKvsArray();
	c->returnValue()->setArray(pArray);

	if(szType.isEmpty())
	{
		c->warning(__tr2qs("Window type or 'all' expected as first parameter"));
		return true;
	}

	int id = 0;

	if(KviQString::equalCI(szContext, "all"))
	{
		// all contexts but no "no_context" windows
		bool bAllWindows = KviQString::equalCI(szType, "all");

		for(auto & wnd : g_pGlobalWindowDict)
		{
			if(wnd.second->context())
			{
				if(bAllWindows)
				{
					pArray->set(id, new KviKvsVariant(QString(wnd.second->id())));
					id++;
				}
				else
				{
					if(szType.toLower() == wnd.second->typeString())
					{
						pArray->set(id, new KviKvsVariant(QString(wnd.second->id())));
						id++;
					}
				}
			}
		}
	}
	else if(KviQString::equalCI(szContext, "any"))
	{
		// all contexts and also "no_context" windows
		bool bAllWindows = KviQString::equalCI(szType.toLower(), "all");

		for(auto & wnd : g_pGlobalWindowDict)
		{
			if(bAllWindows)
			{
				pArray->set(id, new KviKvsVariant(QString(wnd.second->id())));
				id++;
			}
			else
			{
				if(szType.toLower() == wnd.second->typeString())
				{
					pArray->set(id, new KviKvsVariant(QString(wnd.second->id())));
					id++;
				}
			}
		}
	}
	else if(KviQString::equalCI(szContext, "none"))
	{
		// only "no_context" windows
		bool bAllWindows = KviQString::equalCI(szType.toLower(), "all");

		for(auto & wnd : g_pGlobalWindowDict)
		{
			if(!wnd.second->context())
			{
				if(bAllWindows)
				{
					pArray->set(id, new KviKvsVariant(QString(wnd.second->id())));
					id++;
				}
				else
				{
					if(szType.toLower() == wnd.second->typeString())
					{
						pArray->set(id, new KviKvsVariant(QString(wnd.second->id())));
						id++;
					}
				}
			}
		}
	}
	else
	{
		// some specified context
		unsigned int uId = 0;

		if(!szContext.isEmpty())
		{
			// specific context
			bool bOk;
			uId = szContext.toUInt(&bOk);
			if(!bOk)
			{
				c->warning(__tr2qs("Invalid IRC context ID '%Q'"), &szContext);
				return true;
			}
		}
		else
		{
			// current irc context
			if(!c->window()->console())
			{
				return true;
			}
			uId = c->window()->context()->id();
		}

		bool bAllWindows = KviQString::equalCI(szType.toLower(), "all");

		for(auto & wnd : g_pGlobalWindowDict)
		{
			if(wnd.second->context())
			{
				if(wnd.second->context()->id() == uId)
				{
					if(bAllWindows)
					{
						pArray->set(id, new KviKvsVariant(QString(wnd.second->id())));
						id++;
					}
					else
					{
						if(szType.toLower() == wnd.second->typeString())
						{
							pArray->set(id, new KviKvsVariant(QString(wnd.second->id())));
							id++;
						}
					}
				}
			}
		}
	}
	return true;
}

/*
	@doc: window.open
	@type:
		function
	@title:
		$window.open
	@short:
		Creates and opens a new window
	@syntax:
		$window.open([<flags:string>[,<caption:string>[,<irc_context:integer>[,<icon:integer>]]]])
	@description:
		Creates a new window, opens it and returns its window identifier.
		<flags> may be any combination of the following flag characters:[br]
		[b]i[/b]: Causes the window to have an input field in that
		the user can type commands or text. The text typed is reported
		by the [event:ontextinput]OnTextInput[/event] event.[br]
		[b]m[/b]: Creates a window that is initially minimized.[br]
		[b]q[/b]: Don't print warning messages during the creation.[br]
		If <caption> is given then the new window will have it as the initial plain text <caption>.
		You can change the caption later by calling [cmd]window.setWindowTitle[/cmd].[br]
		If <irc_context> is given then the new window is bound to the specified IRC context
		and will be destroyed when the attached console closes.
		If <irc_context> is omitted or is 0 then the window will be context free (not bound
		to any context) and will exist until it is closed by the GUI, by a [cmd]window.close[/cmd]
		call or until KVIrc terminates. When <irc_context> is given but is not valid
		then a warning is printed (unless the q flag is used) and the created window is context free.[br]
		You will generally use the [fnc]$context[/fnc] function to retrieve the current IRC context ID.[br]
		[br]
		<icon> is interpreted as the index of the internal icon to be used
		for the window. If <icon> is omitted then a default icon is used.[br]
	@examples:
		[example]
			%w = $window.open()
			[cmd]window.close[/cmd] %w
			%w = $window.open("m","My funky window")
			[cmd]window.close[/cmd] %w
			%w = $window.open("im","My funky window 2",$context,10)
		[/example]
	@seealso:
		[cmd]window.close[/cmd]
*/

static bool window_kvs_fnc_open(KviKvsModuleFunctionCall * c)
{
	QString szFlags;
	QString szCaption;
	kvs_uint_t uCtx;
	QString szIcon;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("flags", KVS_PT_STRING, KVS_PF_OPTIONAL, szFlags)
	KVSM_PARAMETER("caption", KVS_PT_STRING, KVS_PF_OPTIONAL, szCaption)
	KVSM_PARAMETER("irc_context", KVS_PT_UINT, KVS_PF_OPTIONAL, uCtx)
	KVSM_PARAMETER("icon", KVS_PT_STRING, KVS_PF_OPTIONAL, szIcon)
	KVSM_PARAMETERS_END(c)
	QPixmap * pPix = g_pIconManager->getImage(szIcon);
	if(!pPix)
	{

		c->warning(__tr2qs("The specified icon doesn't exist: switching to 'none'"));
		szIcon.prepend("$icon(");
		szIcon.append(")");
	}
	int iFlags = 0;
	if(szFlags.contains('i'))
		iFlags |= UserWindow::HasInput;

	KviConsoleWindow * pConsole = nullptr;
	if(c->parameterList()->count() >= 3)
	{
		pConsole = g_pApp->findConsole(uCtx);
		if(!pConsole && !szFlags.contains('q'))
		{
			c->warning(__tr2qs("The specified IRC context is not valid: creating a context free window"));
		}
	}

	UserWindow * pWnd = new UserWindow(
	    szCaption.toUtf8().data(),
	    szIcon,
	    pConsole,
	    iFlags);

	g_pMainWindow->addWindow(pWnd, !szFlags.contains('m'));

	c->returnValue()->setInteger(QString(pWnd->id()).toUInt());
	return true;
}

/*
	@doc: window.setWindowTitle
	@type:
		command
	@title:
		window.setWindowTitle
	@short:
		Sets the caption of a user window
	@syntax:
		window.setWindowTitle [-q] <window_id> <plain_text_caption>
	@switches:
		!sw: -q | --quiet
		Be quiet
	@description:
		Sets the caption of the user window specified by <window_id> to <plain_text_caption>.[br]
		If <window_id> is an empty string then the current window is assumed.[br]
		If the window does not exist then a warning is printed unless the -q switch is used.[br]
	@seealso:
*/

static bool window_kvs_cmd_setWindowTitle(KviKvsModuleCommandCall * c)
{
	QString szWnd;
	QString szPlain;
	KviWindow * pWnd;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window_id", KVS_PT_STRING, 0, szWnd)
	KVSM_PARAMETER("plain_text_caption", KVS_PT_STRING, 0, szPlain)
	KVSM_PARAMETERS_END(c)

	pWnd = g_pApp->findWindow(szWnd.toUtf8().data());
	if(!pWnd)
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs("The window with ID '%s' doesn't exist"), szWnd.toUtf8().data());
		return true;
	}

	if(pWnd->type() == KviWindow::UserWindow)
	{
		((UserWindow *)pWnd)->setWindowTitleStrings(szPlain);
	}
	else
	{
		//store the window title (needed for functions that search windows by their captions)
		((KviWindow *)pWnd)->setFixedCaption(szPlain);
		((KviWindow *)pWnd)->setWindowTitle(szPlain);
	}
	return true;
}

/*
	@doc: window.setInputText
	@type:
		command
	@title:
		window.setInputText
	@short:
		Sets the window input text to <text>
	@syntax:
		window.setInputText [-q] <window_id:integer> <text:string>
	@switches:
		!sw: -q | --quiet
		Be quiet
	@description:
		Sets the window input text to <text>
	@seealso:
	[cmd]window.setInputText[/cmd] [cmd]window.insertInInputText[/cmd] [fnc]$window.inputText[/fnc]
*/

static bool window_kvs_cmd_setInputText(KviKvsModuleCommandCall * c)
{
	QString szWnd;
	QString szText;
	KviWindow * pWnd;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window_id", KVS_PT_STRING, 0, szWnd)
	KVSM_PARAMETER("text", KVS_PT_STRING, 0, szText)
	KVSM_PARAMETERS_END(c)

	pWnd = g_pApp->findWindow(szWnd.toUtf8().data());
	if(!pWnd)
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs("The window with ID '%s' doesn't exist"), szWnd.toUtf8().data());
		return true;
	}
	if(pWnd->input())
		pWnd->input()->setText(szText);
	else if(!c->hasSwitch('q', "quiet"))
		c->warning(__tr2qs("Window doesn't have input widget"));

	return true;
}

/*
	@doc: window.insertInInputText
	@type:
		command
	@title:
		window.insertInInputText
	@short:
		Sets the window input text to <text>
	@syntax:
		window.insertInInputText [-q] <window_id:integer> <text:string>
	@switches:
		!sw: -q | --quiet
		Be quiet
	@description:
		Sets the window input text to <text>
	@seealso:
		[cmd]window.setInputText[/cmd], [fnc]$window.inputText[/fnc]
*/

static bool window_kvs_cmd_insertInInputText(KviKvsModuleCommandCall * c)
{
	QString szWnd;
	QString szText;
	KviWindow * pWnd;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window_id", KVS_PT_STRING, 0, szWnd)
	KVSM_PARAMETER("text", KVS_PT_STRING, 0, szText)
	KVSM_PARAMETERS_END(c)

	pWnd = g_pApp->findWindow(szWnd.toUtf8().data());
	if(!pWnd)
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs("The window with ID '%s' doesn't exist"), szWnd.toUtf8().data());
		return true;
	}
	if(pWnd->input())
		pWnd->input()->insertText(szText);
	else if(!c->hasSwitch('q', "quiet"))
		c->warning(__tr2qs("Window doesn't have input widget"));

	return true;
}

/*
	@doc: $window.inputText
	@type:
		function
	@title:
		$window.inputText
	@short:
		Returns the window input line text
	@syntax:
		$window.inputText(<window_id:integer>)
	@description:
		Returns the window input line text
	@seealso:
		[cmd]window.setInputText[/cmd],
		[cmd]window.insertInInputText[/cmd]
*/

static bool window_kvs_fnc_inputText(KviKvsModuleFunctionCall * c)
{
	GET_KVS_FNC_WINDOW_ID
	if(pWnd)
	{
		if(pWnd->input())
			c->returnValue()->setString(pWnd->input()->text());
	}
	return true;
}

/*
	@doc: window.setBackground
	@type:
		command
	@title:
		window.setBackground
	@short:
		Sets the background image of a window
	@syntax:
		window.setBackground [-q] <window_id:integer> [image_id:string]
	@switches:
		!sw: -q | --quiet
		Be quiet
	@description:
		Sets the background image of the window specified by <window_id> to <image_id>.[br]
		If <image_id> is not provided, then the background is cleared and reset to global setting.[br]
		If the specified window or the background image does not exist a warning is printed unless the -q switch is used.
	@seealso:
*/

static bool window_kvs_cmd_setBackground(KviKvsModuleCommandCall * c)
{
	QString szWnd;
	QString szBackground;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window_id", KVS_PT_STRING, 0, szWnd)
	KVSM_PARAMETER("background_path", KVS_PT_STRING, KVS_PF_OPTIONAL, szBackground)
	KVSM_PARAMETERS_END(c)

	KviWindow * pWnd = g_pApp->findWindow(szWnd.toUtf8().data());
	if(!pWnd)
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs("The window with ID '%s' doesn't exist"), szWnd.toUtf8().data());
		return true;
	}

	if(!pWnd->view())
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs("The window with ID '%s' does not support background images!"));
		return true;
	}

	QPixmap p;
	if(!szBackground.isEmpty())
	{
		p = QPixmap(szBackground);
		if(p.isNull())
		{
			if(!c->hasSwitch('q', "quiet"))
				c->warning(__tr2qs("Failed to load the selected image!"));
			return true;
		}
	}

	pWnd->view()->setPrivateBackgroundPixmap(p);
	if(pWnd->isChannel())
	{
		KviChannelWindow * pChanWin = (KviChannelWindow *)pWnd;
		if(pChanWin->messageView())
			pChanWin->messageView()->setPrivateBackgroundPixmap(p);
	}

	return true;
}

/*
	@doc: window.savePropertiesAsDefault
	@type:
		command
	@title:
		window.savePropertiesAsDefault
	@short:
		Saves the window properties as default
	@syntax:
		window.savePropertiesAsDefault [window_id]
	@description:
		Saves the window properties of the specified window as default for every window
		of the same type (e.g. all queries, all channels, ..).
		If window_id is missing then the current window properties are used.
*/

static bool window_kvs_cmd_savePropertiesAsDefault(KviKvsModuleCommandCall * c)
{
	GET_KVS_WINDOW_ID
	if(pWnd)
	{
		pWnd->savePropertiesAsDefault();
	}
	return true;
}

#ifdef COMPILE_CRYPT_SUPPORT
static bool initializeCryptEngine(KviCryptEngine * eng, KviCString & szEncryptKey, KviCString & szDecryptKey, QString & szError)
{
	char * encKey = nullptr;

	char * tmpKey;
	int encKeyLen = szEncryptKey.hexToBuffer(&tmpKey, false);
	if(encKeyLen > 0)
	{
		encKey = (char *)KviMemory::allocate(encKeyLen);
		KviMemory::move(encKey, tmpKey, encKeyLen);
		KviCString::freeBuffer(tmpKey);
	}
	else
	{
		szError = __tr2qs("The encryption key wasn't a valid hexadecimal string");
		return false;
	}

	char * decKey = nullptr;

	int decKeyLen = szDecryptKey.hexToBuffer(&tmpKey, false);
	if(decKeyLen > 0)
	{
		decKey = (char *)KviMemory::allocate(decKeyLen);
		KviMemory::move(decKey, tmpKey, decKeyLen);
		KviCString::freeBuffer(tmpKey);
	}
	else
	{
		szError = __tr2qs("The decryption key wasn't a valid hexadecimal string");
		if(encKey)
			KviMemory::free(encKey);
		return false;
	}
	bool bRet = eng->init(encKey, encKeyLen, decKey, decKeyLen);
	if(!bRet)
		szError = eng->lastError();
	if(encKey)
		KviMemory::free(encKey);
	if(decKey)
		KviMemory::free(decKey);
	return bRet;
}
#endif

/*
	@doc: window.setCryptEngine
	@type:
		command
	@title:
		window.setCryptEngine
	@short:
		Sets the crypt engine for a window that supports it
	@syntax:
		window.setCryptEngine [-q] [-n] [-m] <window_id:integer> <enginename:string> <hex_encrypt_key:string> [hex_decrypt_key:string]
	@switches:
		!sw: -q | --quiet
		Be quiet: do echo the raw data.
		!sw: -n | --onlydecrypt
		Disables encryption
		!sw: -m | --onlyencrypt
		Disables decryption
	@description:
		Sets the specified [doc:crypt_engines]cryptographic engine[/doc] for the window. If <enginename> is empty
		then any current encryption engine is removed (i.e. encrypting is disabled).
		The must be both expressed in hexadecimal notation and are internally transformed in bytes.
		If only the encrypt key is specified then it will be used for both encrypting and
		decrypting. This command works only if cryptography support is compiled in.
	@examples:
		[example]
		[comment]# This is a really lame example :D[/comment]
		alias(saylame)
		{
			window.setCryptEngine $window Lamerizer
			say $0-
			window.setCryptEngine $window
		}
		saylame Hello eleet!
		[/example]

	@seealso:
		[fnc]$asciiToHex[/fnc], [fnc]$features[/fnc]
*/

static bool window_kvs_cmd_setCryptEngine(KviKvsModuleCommandCall * c)
{
	QString szWnd;
	QString szEngine;
	QString szEncryptKey;
	QString szDecryptKey;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window_id", KVS_PT_STRING, 0, szWnd)
	KVSM_PARAMETER("enginename", KVS_PT_STRING, KVS_PF_OPTIONAL, szEngine)
	KVSM_PARAMETER("hex_encrypt_key", KVS_PT_STRING, KVS_PF_OPTIONAL, szEncryptKey)
	KVSM_PARAMETER("hex_decrypt_key", KVS_PT_STRING, KVS_PF_OPTIONAL, szDecryptKey)
	KVSM_PARAMETERS_END(c)
	if(szDecryptKey.isEmpty())
		szDecryptKey = szEncryptKey;
#ifdef COMPILE_CRYPT_SUPPORT
	KviWindow * pWnd = g_pApp->findWindow(szWnd.toUtf8().data());
	if(!pWnd)
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs("The window with ID '%s' doesn't exist"), szWnd.toUtf8().data());
		return true;
	}
	if(c->hasSwitch('n', "onlydecrypt") && c->hasSwitch('m', "onlyencrypt"))
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs("Both -n and -m switches specified, -n takes precedence"));
	}

	if(szEngine.isEmpty())
	{
		pWnd->setCryptSessionInfo(nullptr);
	}
	else
	{
		if(szEncryptKey.isEmpty() || szDecryptKey.isEmpty())
		{
			if(!c->hasSwitch('q', "quiet"))
				c->warning(__tr2qs("No encryption key specified: can't allocate engine"));
			return true;
		}

		(void)g_pModuleManager->loadModulesByCaps("crypt");

		KviCryptEngine * e = g_pCryptEngineManager->allocateEngine(szEngine.toUtf8().data());
		if(e)
		{
			KviCString enc = KviCString(szEncryptKey.toUtf8().data());
			KviCString dec = KviCString(szDecryptKey.toUtf8().data());
			QString szError;
			if(initializeCryptEngine(e, enc, dec, szError))
			{
				KviCryptSessionInfo * inf = KviCryptController::allocateCryptSessionInfo();
				inf->m_pEngine = e;
				inf->m_szEngineName = szEngine;

				inf->m_bDoEncrypt = (!c->hasSwitch('n', "onlydecrypt"));
				inf->m_bDoDecrypt = (!c->hasSwitch('m', "onlyencrypt")) || c->hasSwitch('n', "onlydecrypt");
				pWnd->setCryptSessionInfo(inf);
			}
			else
			{
				if(szError.isEmpty())
					szError = __tr2qs("Unknown engine error");
				g_pCryptEngineManager->deallocateEngine(e);
				if(!c->hasSwitch('q', "quiet"))
					c->warning(__tr2qs("Failed to initialize the specified encryption engine: %Q"), &szError);
			}
		}
		else
		{
			if(!c->hasSwitch('q', "quiet"))
				c->warning(__tr2qs("The encryption engine \"%Q\" doesn't exist"), &szEngine);
		}
	}
#else
	if(!c->hasSwitch('q', "quiet"))
		c->warning(__tr2qs("This executable has been compiled without crypt support"));
#endif
	return true;
}

/*
	@doc: $window.cryptEngine
	@type:
		function
	@title:
		$window.cryptEngine
	@short:
		Returns the name of the encryption engine currently set in a window
	@syntax:
		$window.cryptEngine(<window_id:integer>)
	@description:
		Returns the name of the encryption engine set in the current window.
		If no current engine is set then empty string is returned.
	@seealso:
		[cmd]window.setCryptEngine[/cmd]
*/

static bool window_kvs_fnc_cryptEngine(KviKvsModuleFunctionCall * c)
{
	GET_KVS_FNC_WINDOW_ID
	if(pWnd)
	{
#ifdef COMPILE_CRYPT_SUPPORT
		if(KviCryptSessionInfo * pCryptSessionInfo = pWnd->cryptSessionInfo())
			c->returnValue()->setString(pCryptSessionInfo->m_szEngineName);
#else  //!COMPILE_CRYPT_SUPPORT
// do nothing
#endif //!COMPILE_CRYPT_SUPPORT
	}
	return true;
}

static bool window_kvs_fnc_fake(KviKvsModuleFunctionCall * c)
{
	return true;
}

static bool window_kvs_cmd_fake(KviKvsModuleCommandCall * c)
{
	qDebug("This command was removed due to structural interface changes");
	return true;
}

static bool window_module_init(KviModule * m)
{
	KVSM_REGISTER_FUNCTION(m, "activityTemperature", window_kvs_fnc_activityTemperature);
	KVSM_REGISTER_FUNCTION(m, "activityLevel", window_kvs_fnc_activityLevel);
	KVSM_REGISTER_FUNCTION(m, "highlightLevel", window_kvs_fnc_highlightLevel);
	KVSM_REGISTER_FUNCTION(m, "console", window_kvs_fnc_console);
	KVSM_REGISTER_FUNCTION(m, "hasUserFocus", window_kvs_fnc_hasUserFocus);
	KVSM_REGISTER_FUNCTION(m, "hasOutput", window_kvs_fnc_hasOutput);
	KVSM_REGISTER_FUNCTION(m, "isDocked", window_kvs_fnc_isDocked);
	KVSM_REGISTER_FUNCTION(m, "isSplitView", window_kvs_fnc_isSplitView);
	KVSM_REGISTER_FUNCTION(m, "isMinimized", window_kvs_fnc_fake); // compat only
	KVSM_REGISTER_FUNCTION(m, "isMaximized", window_kvs_fnc_fake); // compat only
	KVSM_REGISTER_FUNCTION(m, "caption", window_kvs_fnc_caption);
	KVSM_REGISTER_FUNCTION(m, "type", window_kvs_fnc_type);
	KVSM_REGISTER_FUNCTION(m, "exists", window_kvs_fnc_exists);
	KVSM_REGISTER_FUNCTION(m, "hasInput", window_kvs_fnc_hasInput);
	KVSM_REGISTER_FUNCTION(m, "list", window_kvs_fnc_list);
	KVSM_REGISTER_FUNCTION(m, "open", window_kvs_fnc_open);
	KVSM_REGISTER_FUNCTION(m, "inputText", window_kvs_fnc_inputText);
	KVSM_REGISTER_FUNCTION(m, "context", window_kvs_fnc_context);
	KVSM_REGISTER_FUNCTION(m, "cryptEngine", window_kvs_fnc_cryptEngine);

	KVSM_REGISTER_SIMPLE_COMMAND(m, "highlight", window_kvs_cmd_highlight);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "close", window_kvs_cmd_close);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "clearOutput", window_kvs_cmd_clearOutput);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "dock", window_kvs_cmd_dock);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "undock", window_kvs_cmd_undock);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "splitView", window_kvs_cmd_splitView);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "unsplitView", window_kvs_cmd_unsplitView);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "minimize", window_kvs_cmd_fake);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "maximize", window_kvs_cmd_fake);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "restore", window_kvs_cmd_fake);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "activate", window_kvs_cmd_activate);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "demandAttention", window_kvs_cmd_demandAttention);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "listtypes", window_kvs_cmd_listtypes);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setBackground", window_kvs_cmd_setBackground);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setWindowTitle", window_kvs_cmd_setWindowTitle);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setCryptEngine", window_kvs_cmd_setCryptEngine);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setInputText", window_kvs_cmd_setInputText);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "insertInInputText", window_kvs_cmd_insertInInputText);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "savePropertiesAsDefault", window_kvs_cmd_savePropertiesAsDefault);

	return true;
}

static bool window_module_cleanup(KviModule *)
{
	while(!g_pUserWindowList.empty())
	{
		auto & w = g_pUserWindowList.front();
		w->close();
	}
	return true;
}

static bool window_module_can_unload(KviModule *)
{
	return g_pUserWindowList.empty();
}

KVIRC_MODULE(
    "Window",                                                            // module name
    "4.0.0",                                                             // module version
    "Copyright (C) 2001-2004 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
    "KVIrc window management functions",
    window_module_init,
    window_module_can_unload,
    0,
    window_module_cleanup,
    0)
