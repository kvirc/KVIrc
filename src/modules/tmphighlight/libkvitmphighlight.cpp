//   File : libkvitmphighlight.cpp
//   Creation date :  Oct 10 01:06:09 CEST 2002 by Juanjo �varez
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Juanjo �varez (juanjux@yahoo.es)
//   Copyright (C) 2002 Szymon Stefanek (kvirc@tin.it)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//


#include "kvi_module.h"
#include "kvi_locale.h"
#include "kvi_channel.h"

//-------------------------------------------------
/*
	@doc: tmphighlight.add
	@type:
		command
	@title:
		tmphighlight.add
	@short:
		Adds a user to the channel temporary highlight list
	@syntax:
		tmphighlight.add <nick:string>
	@description:
		This command adds a user to the channel temporary highlight list, so that user messages[br]
		will be highlighted until you close the channel. This is useful when you are in a very crowded [br]
		channel with lots of conversations running in parallel and you want to follow one of them.[br]
	@seealso:
		[fnc]$tmphighlight.remove[/fnc]
		[fnc]$tmphighlight.ishighlighted[/fnc]
*/
//-------------------------------------------
// tmphighlight.add
//-------------------------------------------
static bool tmphighlight_kvs_cmd_add(KviKvsModuleCommandCall * c)
{
	
	QString szNick;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("nick",KVS_PT_NONEMPTYSTRING,0,szNick)
	KVSM_PARAMETERS_END(c)
	if( ( !c->window()->console()) || c->window()->console()->isNotConnected() )return c->context()->errorNoIrcContext();
	if(!c->window()->type() == KVI_WINDOW_TYPE_CHANNEL)
	{
		c->warning(__tr2qs("Current window is not a channel"));
		return false;
	}
	
	((KviChannel *)c->window())->addHighlightedUser(szNick);

	return true;
}
//-------------------------------------------------
/*
	@doc: tmphighlight.remove
	@type:
		command
	@title:
		tmphighlight.remove
	@short:
		Remove a user from the channel temporary highlight list
	@syntax:
		tmphighlight.remove <nick:string>
	@description:
		This command remove a user from the channel temporary highlight list, so that user messages[br]
		stop being highlighted.
	@seealso:
		[fnc]$tmphighlight.add[/fnc]
		[fnc]$tmphighlight.ishighlighted[/fnc]
*/
//-------------------------------------------
// tmphighlight.remove
//-------------------------------------------

static bool tmphighlight_kvs_cmd_remove(KviKvsModuleCommandCall * c)
{
	QString szNick;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("nick",KVS_PT_NONEMPTYSTRING,0,szNick)
	KVSM_PARAMETERS_END(c)
	if( ( !c->window()->console()) || c->window()->console()->isNotConnected() )return c->context()->errorNoIrcContext();
		if(!c->window()->type() == KVI_WINDOW_TYPE_CHANNEL)
	{
		c->warning(__tr2qs("Current window is not a channel"));
		return false;
	}

	((KviChannel *)c->window())->removeHighlightedUser(szNick);
	return true;
}
//-------------------------------------------------
/*
	@doc: tmphighlight.ishighlighted
	@type:
		function
	@title:
		$tmphighlight.ishighlighted
	@short:
		Returns 1 if the user is highlighted on this channel, 0 otherwise
	@syntax:
		<boolean> $tmphighlight.ishighlighted <nick:string>
	@description:
		This command returns 1 if the user is highlighted on this channel and on this session of 0 otherwise.
	@seealso:
		[fnc]$tmphighlight.add[/fnc]
		[fnc]$tmphighlight.remove[/fnc]

*/
//-------------------------------------------
// tmphighlight.ishighlighted
//-------------------------------------------

static bool tmphighlight_kvs_fnc_ishighlighted(KviKvsModuleFunctionCall * c)
{
	QString szNick;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("nick",KVS_PT_NONEMPTYSTRING,0,szNick)
	KVSM_PARAMETERS_END(c)
	if( ( !c->window()->console()) || c->window()->console()->isNotConnected() )return c->context()->errorNoIrcContext();
		if(!c->window()->type() == KVI_WINDOW_TYPE_CHANNEL)
	{
		c->warning(__tr2qs("Current window is not a channel"));
		return false;
	}
	c->returnValue()->setBoolean(((KviChannel *)c->window())->isHighlightedUser(szNick));
	return true;
}

//-------------------------------------------------    
static bool tmphighlight_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m,"add",tmphighlight_kvs_cmd_add);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"remove",tmphighlight_kvs_cmd_remove);
	KVSM_REGISTER_FUNCTION(m,"isHighVisible",tmphighlight_kvs_fnc_ishighlighted);
	KVSM_REGISTER_FUNCTION(m,"isHighLighted",tmphighlight_kvs_fnc_ishighlighted);
	return true;
}
//-------------------------------------------------
static bool tmphighlight_module_cleanup(KviModule *m)
{
	return true;
}
//-------------------------------------------------
static bool tmphighlight_module_can_unload(KviModule *m)
{
	return true;
}
//-------------------------------------------------
KVIRC_MODULE(
	"TmpHighlight",                                                 // module name
	"4.0.0",                                                // module version
	"          (C) 2002 Juanjo Alvarez (juanjux@yahoo.es)", // author & (C)
	"Temporal Highlightining of channel users",
	tmphighlight_module_init,
	tmphighlight_module_can_unload,
	0,
	tmphighlight_module_cleanup
)
