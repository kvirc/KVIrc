//=============================================================================
//
//   File : libkviuserlist.cpp
//   Creation date : Fri Sep 03 2010 15:23:00 CEST by Fabio Bas
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

#include "KviModule.h"
#include "KviLocale.h"
#include "KviChannelWindow.h"
#include "KviUserListView.h"

#define GET_KVS_WINDOW_ID_AND_NICK                                                                   \
	QString szWnd, szNick;                                                                           \
	KviWindow * pWnd;                                                                                \
	KVSM_PARAMETERS_BEGIN(c)                                                                         \
	KVSM_PARAMETER("nick", KVS_PT_NONEMPTYSTRING, 0, szNick)                                         \
	KVSM_PARAMETER("window_id", KVS_PT_STRING, KVS_PF_OPTIONAL, szWnd)                               \
	KVSM_PARAMETERS_END(c)                                                                           \
	if(c->parameterList()->count() == 1)                                                             \
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
	}                                                                                                \
	if(!pWnd)                                                                                        \
		return true;

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
	}                                                                  \
	if(!pWnd)                                                          \
	{                                                                  \
		return true;                                                   \
	}

// FIXME: #warning "THIS HAS TO WORK FOR QUERIES TOO!"
/*
	@doc: userlist.selected
	@type:
		function
	@title:
		$userlist.selected
	@short:
		Returns the list of selected nicknames in the channel's userlist
	@syntax:
		<array> $userlist.selected
		<array> $userlist.selected(<window_id:string>)
	@description:
		The form with the <window id> parameter returns an array of the selected
		nicknames in the channel designated by <window id>.[br]
		The form without parameters returns an array of the selected nicknames
		in the current window (assuming that it is a channel),
		thus it is equivalent to calling $userlist.selected([fnc]$window[/fnc]).[br]
		The returned value may be assigned to a dictionary too: it will be used to simulate an array.[br]
		In a non-array/dictionary context it returns the selected nicknames as a comma separated list.
	@examples:
		[example]
			[cmd]echo[/cmd] $userlist.selected
			[cmd]foreach[/cmd](%i,$userlist.selected)[cmd]echo[/cmd] %i
		[/example]
	@seealso:
		[fnc]$window[/fnc],
		[fnc]$channel[/fnc],
		[doc:window_naming_conventions]Window naming conventions[/doc]
*/

static bool userlist_kvs_fnc_selected(KviKvsModuleFunctionCall * c)
{
	GET_KVS_FNC_WINDOW_ID

	if(pWnd->type() != KviWindow::Channel)
	{
		c->warning(__tr2qs_ctx("The specified window is not a channel", "kvs"));
		c->returnValue()->setNothing();
		return true;
	}

	KviKvsArray * a = new KviKvsArray();

	kvs_int_t i = 0;
	for(QString * s = ((KviChannelWindow *)pWnd)->firstSelectedNickname(); s; s = ((KviChannelWindow *)pWnd)->nextSelectedNickname())
	{
		a->set(i, new KviKvsVariant(*s));
		i++;
	}

	c->returnValue()->setArray(a);
	return true;
}

static bool userlist_kvs_cmd_select(KviKvsModuleCommandCall * c)
{
	GET_KVS_WINDOW_ID_AND_NICK

	if(pWnd->type() != KviWindow::Channel)
	{
		c->warning(__tr2qs_ctx("The specified window is not a channel", "kvs"));
		return true;
	}

	((KviUserListView *)((KviChannelWindow *)pWnd)->userListView())->select(szNick);
	return true;
}

static bool userlist_kvs_cmd_ensureVisible(KviKvsModuleCommandCall * c)
{
	GET_KVS_WINDOW_ID_AND_NICK

	if(pWnd->type() != KviWindow::Channel)
	{
		c->warning(__tr2qs_ctx("The specified window is not a channel", "kvs"));
		return true;
	}

	((KviUserListView *)((KviChannelWindow *)pWnd)->userListView())->ensureVisible(szNick);
	return true;
}

static bool userlist_module_init(KviModule * m)
{
	KVSM_REGISTER_FUNCTION(m, "selected", userlist_kvs_fnc_selected);

	KVSM_REGISTER_SIMPLE_COMMAND(m, "select", userlist_kvs_cmd_select);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "ensureVisible", userlist_kvs_cmd_ensureVisible);
	return true;
}

static bool userlist_module_cleanup(KviModule *)
{
	return true;
}

static bool userlist_module_can_unload(KviModule *)
{
	return true;
}

KVIRC_MODULE(
    "Userlist",                                                  // module name
    "4.0.0",                                                     // module version
    "Copyright (C) 2010 Fabio Bas (ctrlaltca at gmail dot com)", // author & (C)
    "KVIrc userlist management functions",
    userlist_module_init,
    userlist_module_can_unload,
    0,
    userlist_module_cleanup,
    0)
