//=============================================================================
//
//   File : libkvitmphighlight.cpp
//   Creation date :  Oct 10 01:06:09 CEST 2002 by Juanjo Alvarez
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002 Juanjo Alvarez (juanjux@yahoo.es)
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

#include "KviModule.h"
#include "KviLocale.h"
#include "KviChannelWindow.h"

/*
	@doc: tmphighlight.addNick
	@type:
		command
	@title:
		tmphighlight.addNick
	@short:
		Adds a user to the channel temporary highlight list
	@syntax:
		tmphighlight.addNick <nick:string>
	@description:
		This command adds a user to the channel temporary highlight list, so that user messages[br]
		will be highlighted until you close the channel. This is useful when you are in a very crowded [br]
		channel with lots of conversations running in parallel and you want to follow one of them.[br]
	@seealso:
		[fnc]$tmphighlight.removeNick[/fnc]
		[fnc]$tmphighlight.isNickHighlighted[/fnc]
*/

static bool tmphighlight_kvs_cmd_addnick(KviKvsModuleCommandCall * c)
{

	QString szNick;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("nick", KVS_PT_NONEMPTYSTRING, 0, szNick)
	KVSM_PARAMETERS_END(c)
	if(!c->window()->console())
		return c->context()->errorNoIrcContext();
	if(c->window()->console()->isNotConnected())
		return c->context()->errorNoIrcContext();

	if(c->window()->type() != KviWindow::Channel)
	{
		c->warning(__tr2qs("Current window is not a channel"));
		return false;
	}

	((KviChannelWindow *)c->window())->addHighlightedUser(szNick);

	return true;
}

/*
	@doc: tmphighlight.removeNick
	@type:
		command
	@title:
		tmphighlight.removeNick
	@short:
		Remove a user from the channel temporary highlight list
	@syntax:
		tmphighlight.removeNick <nick:string>
	@description:
		This command remove a user from the channel temporary highlight list, so that user messages[br]
		stop being highlighted.
	@seealso:
		[fnc]$tmphighlight.addNick[/fnc]
		[fnc]$tmphighlight.isNickHighlighted[/fnc]
*/

static bool tmphighlight_kvs_cmd_removenick(KviKvsModuleCommandCall * c)
{
	QString szNick;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("nick", KVS_PT_NONEMPTYSTRING, 0, szNick)
	KVSM_PARAMETERS_END(c)
	if(!c->window()->console())
		return c->context()->errorNoIrcContext();
	if(c->window()->console()->isNotConnected())
		return c->context()->errorNoIrcContext();

	if(c->window()->type() != KviWindow::Channel)
	{
		c->warning(__tr2qs("Current window is not a channel"));
		return false;
	}

	((KviChannelWindow *)c->window())->removeHighlightedUser(szNick);
	return true;
}

/*
	@doc: tmphighlight.addChannel
	@type:
		command
	@title:
		tmphighlight.addChannel
	@short:
		Adds a channel to the IRC context temporary highlight list
	@syntax:
		tmphighlight.addChannel [window_id:integer]
	@description:
		This command adds a channel to the IRC context temporary highlight list, so that every user message
		from that channel will be highlighted until you close that IRC context. This is useful when you want
		to follow every message in a low traffic channel. If you don't specify a channel, the current one
		is used.
	@seealso:
		[fnc]$tmphighlight.removeChannel[/fnc]
		[fnc]$tmphighlight.isChannelHighlighted[/fnc]
		[fnc]$channel[/fnc]
*/

static bool tmphighlight_kvs_cmd_addchannel(KviKvsModuleCommandCall * c)
{
	QString szWnd;
	KviWindow * pWnd;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window_id", KVS_PT_STRING, KVS_PF_OPTIONAL, szWnd)
	KVSM_PARAMETERS_END(c)
	if(c->parameterList()->count() == 0)
	{
		pWnd = c->window();
	}
	else
	{
		pWnd = g_pApp->findWindow(szWnd.toUtf8().data());
		if(!pWnd)
		{
			c->warning(__tr2qs("Unable to find a window with the specified window ID"));
			return false;
		}
	}

	if(!pWnd->console())
		return c->context()->errorNoIrcContext();
	if(pWnd->console()->isNotConnected())
		return c->context()->errorNoIrcContext();

	if(pWnd->type() != KviWindow::Channel)
	{
		c->warning(__tr2qs("Current window is not a channel"));
		return false;
	}

	((KviConsoleWindow *)pWnd->console())->addHighlightedChannel(pWnd->windowName());

	return true;
}

/*
	@doc: tmphighlight.removeChannel
	@type:
		command
	@title:
		tmphighlight.removeChannel
	@short:
		Remove a channel from the IRC context temporary highlight list
	@syntax:
		tmphighlight.removeChannel [window_id:integer]
	@description:
		This command remove a channel from the IRC context temporary highlight list.
		If you don't specify a channel, the current one is used.
	@seealso:
		[fnc]$tmphighlight.addChannel[/fnc]
		[fnc]$tmphighlight.isChannelHighlighted[/fnc]
		[fnc]$channel[/fnc]
*/

static bool tmphighlight_kvs_cmd_removechannel(KviKvsModuleCommandCall * c)
{
	QString szWnd;
	KviWindow * pWnd;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window_id", KVS_PT_STRING, KVS_PF_OPTIONAL, szWnd)
	KVSM_PARAMETERS_END(c)
	if(c->parameterList()->count() == 0)
	{
		pWnd = c->window();
	}
	else
	{
		pWnd = g_pApp->findWindow(szWnd.toUtf8().data());
		if(!pWnd)
		{
			c->warning(__tr2qs("Unable to find a window with the specified window ID"));
			return false;
		}
	}

	if(!pWnd->console())
		return c->context()->errorNoIrcContext();
	if(pWnd->console()->isNotConnected())
		return c->context()->errorNoIrcContext();

	if(pWnd->type() != KviWindow::Channel)
	{
		c->warning(__tr2qs("Current window is not a channel"));
		return false;
	}

	((KviConsoleWindow *)pWnd->console())->removeHighlightedChannel(pWnd->windowName());

	return true;
}

/*
	@doc: tmphighlight.isNickHighlighted
	@type:
		function
	@title:
		$tmphighlight.isNickHighlighted
	@short:
		Returns [b]1[/b] if the user is highlighted on this channel, and [b]0[/b] otherwise
	@syntax:
		<boolean> $tmphighlight.isNickHighlighted <nick:string>
	@description:
		This command returns 1 if the user is highlighted on this channel and on this session of 0 otherwise.
	@seealso:
		[fnc]$tmphighlight.addNick[/fnc]
		[fnc]$tmphighlight.removeNick[/fnc]

*/

static bool tmphighlight_kvs_fnc_isnickhighlighted(KviKvsModuleFunctionCall * c)
{
	QString szNick;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("nick", KVS_PT_NONEMPTYSTRING, 0, szNick)
	KVSM_PARAMETERS_END(c)
	if(!c->window()->console())
		return c->context()->errorNoIrcContext();
	if(c->window()->console()->isNotConnected())
		return c->context()->errorNoIrcContext();

	if(c->window()->type() != KviWindow::Channel)
	{
		c->warning(__tr2qs("Current window is not a channel"));
		return false;
	}
	c->returnValue()->setBoolean(((KviChannelWindow *)c->window())->isHighlightedUser(szNick));
	return true;
}

/*
	@doc: tmphighlight.isChannelHighlighted
	@type:
		function
	@title:
		$tmphighlight.isChannelHighlighted
	@short:
		Returns [b]1[/b] if the channel is highlighted on this IRC network, and [b]0[/b] otherwise
	@syntax:
		<boolean> $tmphighlight.isChannelHighlighted[window_id:integer]
	@description:
		This command returns 1 if the channel is highlighted on this IRC network, and [b]0[/b] otherwise.
	@seealso:
		[fnc]$tmphighlight.addChannel[/fnc]
		[fnc]$tmphighlight.removeChannel[/fnc]
		[fnc]$channel[/fnc]

*/

static bool tmphighlight_kvs_fnc_ischannelhighlighted(KviKvsModuleFunctionCall * c)
{
	QString szWnd;
	KviWindow * pWnd;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window_id", KVS_PT_STRING, KVS_PF_OPTIONAL, szWnd)
	KVSM_PARAMETERS_END(c)
	if(c->parameterList()->count() == 0)
	{
		pWnd = c->window();
	}
	else
	{
		pWnd = g_pApp->findWindow(szWnd.toUtf8().data());
		if(!pWnd)
		{
			c->warning(__tr2qs("Unable to find a window with the specified window ID"));
			return false;
		}
	}

	if(!pWnd->console())
		return c->context()->errorNoIrcContext();
	if(pWnd->console()->isNotConnected())
		return c->context()->errorNoIrcContext();

	if(pWnd->type() != KviWindow::Channel)
	{
		c->warning(__tr2qs("Current window is not a channel"));
		return false;
	}

	c->returnValue()->setBoolean(((KviConsoleWindow *)pWnd->console())->isHighlightedChannel(pWnd->windowName()));
	return true;
}

static bool tmphighlight_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m, "addNick", tmphighlight_kvs_cmd_addnick);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "removeNick", tmphighlight_kvs_cmd_removenick);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "addChannel", tmphighlight_kvs_cmd_addchannel);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "removeChannel", tmphighlight_kvs_cmd_removechannel);
	KVSM_REGISTER_FUNCTION(m, "isNickHighLighted", tmphighlight_kvs_fnc_isnickhighlighted);
	KVSM_REGISTER_FUNCTION(m, "isChannelHighLighted", tmphighlight_kvs_fnc_ischannelhighlighted);
	return true;
}

static bool tmphighlight_module_cleanup(KviModule *)
{
	return true;
}

static bool tmphighlight_module_can_unload(KviModule *)
{
	return true;
}

KVIRC_MODULE(
    "TmpHighlight", // module name
    "4.0.0",        // module version
    "(C) 2002 Juanjo Alvarez (juanjux@yahoo.es)"
    "(C) 2009 Fabio Bas (ctrlaltca at libero dot it)", // author & (C)
    "Temporary highlighting of channels and nicknames",
    tmphighlight_module_init,
    tmphighlight_module_can_unload,
    0,
    tmphighlight_module_cleanup,
    0)
