//=============================================================================
//
//   File : libkvichan.cpp
//   Creation date : Sun Feb 02 2002 05:27:11 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma@kvirc.net)
//   Copyright (C) 2002 Juanjo Alvarez  (juanjux@yahoo.es)
//   Copyright (C) 2004-2010 Szymon Stefanek (pragma@kvirc.net)
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
#include "KviApplication.h"
#include "KviChannelWindow.h"
#include "KviLocale.h"
#include "KviTopicWidget.h"
#include "KviIrcMask.h"
#include "KviMaskEditor.h"
#include "KviIrcUrl.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionTarget.h"

#include <QString>
#include <vector>

static KviChannelWindow * chan_kvs_find_channel(KviKvsModuleFunctionCall * c, QString & szChan, bool bNoWarnings = false)
{
	if(szChan.isEmpty())
	{
		if(c->window()->type() == KviWindow::Channel)
			return (KviChannelWindow *)(c->window());
		if(c->window()->type() == KviWindow::DeadChannel)
			return (KviChannelWindow *)(c->window());
		if(!bNoWarnings)
			c->warning(__tr2qs("The current window is not a channel"));
	}
	else
	{
		KviWindow * w = g_pApp->findWindow(szChan);
		if(!w)
		{
			if(!bNoWarnings)
				c->warning(__tr2qs("Can't find the window with ID '%Q'"), &szChan);
			return nullptr;
		}
		if(w->type() == KviWindow::Channel)
			return (KviChannelWindow *)w;
		if(w->type() == KviWindow::DeadChannel)
			return (KviChannelWindow *)w;
		if(!bNoWarnings)
			c->warning(__tr2qs("The specified window (%Q) is not a channel"), &szChan);
	}
	return nullptr;
}

/*
	@doc: chan.name
	@type:
		function
	@title:
		$chan.name
	@short:
		Returns the name of a channel
	@syntax:
		<string> $chan.name
		<string> $chan.name(<window_id:string>)
	@description:
		The first form returns the name of the current channel (assuming that the current window
		is a channel at all). If the current window is not a channel, a warning is printed
		and an empty string is returned.[br]
		The second form returns the name of the channel specified by <window_id>.[br]
		This function works also on dead channels.[br]
*/

static bool chan_kvs_fnc_name(KviKvsModuleFunctionCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szId);
	if(ch)
	{
		c->returnValue()->setString(ch->windowName());
	}
	return true;
}

/*
	@doc: chan.getUrl
	@type:
		function
	@title:
		$chan.getUrl
	@short:
		Returns the URL of a channel
	@syntax:
		<string> $chan.getUrl
		<string> $chan.getUrl(<window_id:string>)
		<string> $chan.getUrl(<channel_name:string>)
	@description:
		Returns the IRC URL for this channel.
*/

static bool chan_kvs_fnc_getUrl(KviKvsModuleFunctionCall * c)
{
	QString szId, url;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("channel id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szId, true);
	if(ch)
	{
		KviIrcUrl::join(url, ch->connection()->target()->server());
		url.append(ch->windowName());
		if(ch->hasChannelMode('k'))
		{
			url.append("?");
			url.append(ch->channelModeParam('k'));
		}
	}
	else
	{
		if(c->window()->connection())
		{
			KviIrcUrl::join(url, c->window()->connection()->target()->server());
			url.append(szId);
		}
		else
		{
			c->error("There is no active IRC connection for current context");
		}
	}
	c->returnValue()->setString(url);
	return true;
}

/*
	@doc: chan.isDead
	@type:
		function
	@title:
		$chan.isDead
	@short:
		Checks if a channel is dead
	@syntax:
		<boolean> $chan.isDead
		<boolean> $chan.isDead(<window_id:string>)
	@description:
		Returns [b]1[/b] if the channel specified by <window_id> is a dead channel and [b]0[/b] otherwise.[br]
		The form without parameters works on the current window.[br]
*/

static bool chan_kvs_fnc_isdead(KviKvsModuleFunctionCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szId);
	if(ch)
		c->returnValue()->setBoolean((ch->type() == KviWindow::DeadChannel));
	return true;
}

/*
	@doc: chan.topic
	@type:
		function
	@title:
		$chan.topic
	@short:
		Returns the topic of a channel
	@syntax:
		<string> $chan.topic
		<string> $chan.topic(<window_id:string>)
	@description:
		The first form returns the topic of the current channel (assuming that the current window
		is a channel at all). If the current window is not a channel, a warning is printed
		and an empty string is returned.[br]
		The second form returns the topic of the channel specified by <window_id>.[br]
		The topic is returned as it is known form at the call time: this means that
		if the channel is not synchronized with the server (as just after the join, for example)
		you might get an empty string (the topic is not yet known).[br]
		This function works also on dead channels although the topic returned is the last
		topic seen while the channel wasn't dead.[br]
*/

static bool chan_kvs_fnc_topic(KviKvsModuleFunctionCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szId);
	if(ch)
		c->returnValue()->setString(ch->topicWidget()->topic());
	return true;
}

/*
	@doc: chan.topicsetby
	@type:
		function
	@title:
		$chan.topicsetby
	@short:
		Returns the author of the topic of a channel
	@syntax:
		<string> $chan.topicsetby
		<string> $chan.topicsetby(<window_id:string>)
	@description:
		The first form returns the author of the topic of the current channel (assuming that the current window
		is a channel at all). If the current window is not a channel, a warning is printed
		and an empty string is returned.[br]
		The second form returns the author of the topic of the channel specified by <window_id>.[br]
		The topic author nickname is returned if it is known form at the call time: this means that
		if the channel is not synchronized with the server (as just after the join, for example)
		you might get an empty string (the topic is not yet known).[br]
		This function works also on dead channels although the information returned is the last
		information seen while the channel wasn't dead.[br]
*/

static bool chan_kvs_fnc_topicsetby(KviKvsModuleFunctionCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szId);
	if(ch)
		c->returnValue()->setString(ch->topicWidget()->topicSetBy());
	return true;
}

/*
	@doc: chan.topicsetat
	@type:
		function
	@title:
		$chan.topicsetat
	@short:
		Returns the set time of the topic of a channel
	@syntax:
		<string> $chan.topicsetat
		<string> $chan.topicsetat(<window_id:string>)
	@description:
		The first form returns the set time of the topic of the current channel (assuming that the current window
		is a channel at all). If the current window is not a channel, a warning is printed
		and an empty string is returned.[br]
		The second form returns the set time of the topic of the channel specified by <window_id>.[br]
		The topic set time is returned if it is known form at the call time: this means that
		if the channel is not synchronized with the server (as just after the join, for example)
		you might get an empty string (the topic is not yet known).[br]
		This function works also on dead channels although the information returned is the last
		information seen while the channel wasn't dead.[br]
*/

static bool chan_kvs_fnc_topicsetat(KviKvsModuleFunctionCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szId);
	if(ch)
		c->returnValue()->setString(ch->topicWidget()->topicSetAt());
	return true;
}

/*
	@doc: chan.usercount
	@type:
		function
	@title:
		$chan.usercount
	@short:
		Returns the number of users on a channel
	@syntax:
		<integer> $chan.usercount
		<integer> $chan.usercount(<window_id:string>)
	@description:
		The first form returns the number of users on the current channel (assuming that the current window
		is a channel at all). If the current window is not a channel, a warning is printed
		and an empty string is returned.[br]
		The second form returns the number of users on the channel specified by <window_id>.[br]
		The number of users returned depends on the state of KVIrc's data on the channel, which will be
		out-of-synch with the server just after the channel is joined. If the function is used prior to
		the channel's OnChannelSync event, a smaller user count than there really is will be returned.
*/

static bool chan_kvs_fnc_usercount(KviKvsModuleFunctionCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szId);
	if(ch)
		c->returnValue()->setInteger(ch->count());
	return true;
}

/*
	@doc: chan.ownercount
	@type:
		function
	@title:
		$chan.ownercount
	@short:
		Returns the number of channel owner users on a channel
	@syntax:
		<integer> $chan.ownercount
		<integer> $chan.ownercount(<window_id:string>)
	@description:
		The first form returns the number of owners users on the current channel (assuming that the current window
		is a channel at all). If the current window is not a channel, a warning is printed
		and an empty string is returned.[br]
		The second form returns the number of owners users on the channel specified by <window_id>.[br]
		The number of owners is returned if it is known form at the call time: this means that
		if the channel is not synchronized with the server (as just after the join, for example)
		you might get a number that is actually smaller.[br]
*/

static bool chan_kvs_fnc_ownercount(KviKvsModuleFunctionCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szId);
	if(ch)
		c->returnValue()->setInteger(ch->chanOwnerCount());
	return true;
}

/*
	@doc: chan.admincount
	@type:
		function
	@title:
		$chan.admincount
	@short:
		Returns the number of channel admin users on a channel
	@syntax:
		<integer> $chan.admincount
		<integer> $chan.admincount(<window_id:string>)
	@description:
		The first form returns the number of administrator users on the current channel (assuming that the current window
		is a channel at all). If the current window is not a channel, a warning is printed
		and an empty string is returned.[br]
		The second form returns the number of administrator users on the channel specified by <window_id>.[br]
		The number of administrators is returned if it is known form at the call time: this means that
		if the channel is not synchronized with the server (as just after the join, for example)
		you might get a number that is actually smaller.[br]
*/

static bool chan_kvs_fnc_admincount(KviKvsModuleFunctionCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szId);
	if(ch)
		c->returnValue()->setInteger(ch->chanAdminCount());
	return true;
}

/*
	@doc: chan.opcount
	@type:
		function
	@title:
		$chan.opcount
	@short:
		Returns the number of op users on a channel
	@syntax:
		<integer> $chan.opcount
		<integer> $chan.opcount(<window_id:string>)
	@description:
		The first form returns the number of op users on the current channel (assuming that the current window
		is a channel at all). If the current window is not a channel, a warning is printed
		and an empty string is returned.[br]
		The second form returns the number of op users on the channel specified by <window_id>.[br]
		The number of ops is returned if it is known form at the call time: this means that
		if the channel is not synchronized with the server (as just after the join, for example)
		you might get a number that is actually smaller.[br]
*/

static bool chan_kvs_fnc_opcount(KviKvsModuleFunctionCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szId);
	if(ch)
		c->returnValue()->setInteger(ch->opCount());
	return true;
}

/*
	@doc: chan.voicecount
	@type:
		function
	@title:
		$chan.voicecount
	@short:
		Returns the number of voiced users on a channel
	@syntax:
		<integer> $chan.voicecount
		<integer> $chan.voicecount(<window_id:string>)
	@description:
		The first form returns the number of voiced users on the current channel (assuming that the current window
		is a channel at all). If the current window is not a channel, a warning is printed
		and an empty string is returned.[br]
		The second form returns the number of voiced users on the channel specified by <window_id>.[br]
		The number of voiced users is returned if it is known form at the call time: this means that
		if the channel is not synchronized with the server (as just after the join, for example)
		you might get a number that is actually smaller.[br]
*/

static bool chan_kvs_fnc_voicecount(KviKvsModuleFunctionCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szId);
	if(ch)
		c->returnValue()->setInteger(ch->voiceCount());
	return true;
}

/*
	@doc: chan.halfopcount
	@type:
		function
	@title:
		$chan.halfopcount
	@short:
		Returns the number of halfop users on a channel
	@syntax:
		<integer> $chan.halfOpCount
		<integer> $chan.halfOpCount(<window_id:string>)
	@description:
		The first form returns the number of half-operator users on the current channel (assuming that the current window
		is a channel at all). If the current window is not a channel, a warning is printed
		and an empty string is returned.[br]
		The second form returns the number of half-operator users on the channel specified by <window_id>.[br]
		The number of half-operator users is returned if it is known form at the call time: this means that
		if the channel is not synchronized with the server (as just after the join, for example)
		you might get a number that is actually smaller.[br]
*/

static bool chan_kvs_fnc_halfopcount(KviKvsModuleFunctionCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szId);
	if(ch)
		c->returnValue()->setInteger(ch->halfOpCount());
	return true;
}

/*
	@doc: chan.useropcount
	@type:
		function
	@title:
		$chan.useropcount
	@short:
		Returns the number of userop users on a channel
	@syntax:
		<integer> $chan.userOpCount
		<integer> $chan.userOpCount(<window_id:string>)
	@description:
		The first form returns the number of user-operator users on the current channel (assuming that the current window
		is a channel at all). If the current window is not a channel, a warning is printed
		and an empty string is returned.[br]
		The second form returns the number of user-operator users on the channel specified by <window_id>.[br]
		The number of user-operator users is returned if it is known form at the call time: this means that
		if the channel is not synchronized with the server (as just after the join, for example)
		you might get a number that is actually smaller.[br]
*/

static bool chan_kvs_fnc_useropcount(KviKvsModuleFunctionCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szId);
	if(ch)
		c->returnValue()->setInteger(ch->userOpCount());
	return true;
}

/*
	@doc: chan.bancount
	@type:
		function
	@title:
		$chan.bancount
	@short:
		Returns the number of entries in the channel ban list
	@syntax:
		<integer> $chan.bancount
		<integer> $chan.bancount(<window_id:string>)
	@description:
		The first form returns the number of entries in the ban list of the current channel (assuming that the current window
		is a channel at all). If the current window is not a channel, a warning is printed
		and [b]0[/b] is returned.[br]
		The second form returns the number entries in the ban list of the channel specified by <window_id>.[br]
		The number of ban list entries is returned if it is known form at the call time: this means that
		if the channel is not synchronized with the server (as just after the join, for example)
		you might get a number that is actually smaller.[br]
*/

static bool chan_kvs_fnc_bancount(KviKvsModuleFunctionCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szId);
	if(ch)
		c->returnValue()->setInteger(ch->maskCount('b'));
	return true;
}

/*
	@doc: chan.banexceptioncount
	@type:
		function
	@title:
		$chan.banexceptioncount
	@short:
		Returns the number of entries in the channel ban exception list
	@syntax:
		<integer> $chan.banexceptioncount
		<integer> $chan.banexceptioncount(<window_id:string>)
	@description:
		The first form returns the number of entries in the ban exception list of the current channel (assuming that the current window
		is a channel at all). If the current window is not a channel, a warning is printed
		and [b]0[/b] is returned.[br]
		The second form returns the number entries in the ban exception list of the channel specified by <window_id>.[br]
		The number of ban exception list entries is returned if it is known form at the call time: this means that
		if the channel is not synchronized with the server (as just after the join, for example)
		you might get a number that is actually smaller.[br]
*/

static bool chan_kvs_fnc_banexceptioncount(KviKvsModuleFunctionCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szId);
	if(ch)
		c->returnValue()->setInteger(ch->maskCount('e'));
	return true;
}

/*
	@doc: chan.invitecount
	@type:
		function
	@title:
		$chan.invitecount
	@short:
		Returns the number of entries in the channel invite list
	@syntax:
		<integer> $chan.invitecount
		<integer> $chan.invitecount(<window_id:string>)
	@description:
		The first form returns the number of entries in the invite list of the current channel (assuming that the current window
		is a channel at all). If the current window is not a channel, a warning is printed
		and [b]0[/b] is returned.[br]
		The second form returns the number entries in the invite list of the channel specified by <window_id>.[br]
		The number of invite list entries is returned if it is known form at the call time: this means that
		if the channel is not synchronized with the server (as just after the join, for example)
		you might get a number that is actually smaller.[br]
*/
static bool chan_kvs_fnc_invitecount(KviKvsModuleFunctionCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szId);
	if(ch)
		c->returnValue()->setInteger(ch->maskCount('I'));
	return true;
}

/*
	@doc: chan.maskcount
	@type:
		function
	@title:
		$chan.maskcount
	@short:
		Returns the number of entries in the mask list for a channel mode
	@syntax:
		<integer> $chan.maskcount(<mode:char>)
		<integer> $chan.maskcount(<mode:char>,[window_id:string])
	@description:
		The first form returns the number of entries in the ban list of the current channel (assuming that the current window
		is a channel at all). If the current window is not a channel, a warning is printed
		and [b]0[/b] is returned.[br]
		The second form returns the number entries in the ban list of the channel specified by <window_id>.[br]
		The number of list entries is returned if it is known form at the call time: this means that
		if the channel is not synchronized with the server (as just after the join, for example)
		you might get a number that is actually smaller.[br]
*/

static bool chan_kvs_fnc_maskcount(KviKvsModuleFunctionCall * c)
{
	QString szId, szMode;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("mode", KVS_PT_NONEMPTYSTRING, 0, szMode)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId)
	KVSM_PARAMETERS_END(c)

	char cMode = szMode.at(0).unicode();
	KviChannelWindow * ch = chan_kvs_find_channel(c, szId);
	if(ch)
		c->returnValue()->setInteger(ch->maskCount(cMode));
	return true;
}

/*
	@doc: chan.ison
	@type:
		function
	@title:
		$chan.ison
	@short:
		Checks if a user is on a channel
	@syntax:
		<boolean> $chan.ison(<nickname:string>[,<window_id:string>])
	@description:
		Returns [b]1[/b] if <nickname> is on the channel identified by <window_id>, and [b]0[/b] otherwise.[br]
		If <window_id> is not specified the current window is used (assuming that it is a channel at all).[br]
		If the window is not a channel, a warning is printed and an empty string is returned.[br]
*/

static bool chan_kvs_fnc_ison(KviKvsModuleFunctionCall * c)
{
	QString szId, szNick;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("nickname", KVS_PT_NONEMPTYSTRING, 0, szNick)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szId);
	if(ch)
		c->returnValue()->setBoolean(ch->isOn(szNick));
	return true;
}

/*
	@doc: chan.getflag
	@type:
		function
	@title:
		$chan.getflag
	@short:
		Returns the channel-user mode flag of a user
	@syntax:
		<char> $chan.getflag(<nickname:string>[,<window_id:string>])
	@description:
		Returns the channel user mode flag of a user on the channel specified by <window_id>.[br]
		If <window_id> is not passed, the current window is used.[br]
		If the specified window is not a channel, a warning is printed and an empty string is returned.[br]
		If the specified user is not on the channel identified by <window_id>, an empty string is returned.[br]
		The possible user flags are:[br]
			[b]![/b] for channel administrators[br]
			[b]@[/b] for ops[br]
			[b]%[/b] for halfops[br]
			[b]+[/b] for voiced users[br]
			[b]-[/b] for userops[br]
		If the user has more than one flag then the highest one is returned.[br]
		If the user has no flag at all then an empty string is returned.
*/

static bool chan_kvs_fnc_getflag(KviKvsModuleFunctionCall * c)
{
	QString szId, szNick;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("nickname", KVS_PT_NONEMPTYSTRING, 0, szNick)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szId);
	if(ch)
	{
		QChar cFlag = ch->userListView()->getUserFlag(szNick);
		if(cFlag != QChar(0))
		{
			QString szFlag = cFlag;
			c->returnValue()->setString(szFlag);
		}
		else
		{
			c->returnValue()->setNothing();
		}
	}
	return true;
}

#define IS_KVS_FUNC(__clbkname, __chanfunc)                               \
	static bool __clbkname(KviKvsModuleFunctionCall * c)                  \
	{                                                                     \
		QString szId, szNick;                                             \
		KVSM_PARAMETERS_BEGIN(c)                                          \
		KVSM_PARAMETER("nickname", KVS_PT_NONEMPTYSTRING, 0, szNick)      \
		KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId) \
		KVSM_PARAMETERS_END(c)                                            \
		KviChannelWindow * ch = chan_kvs_find_channel(c, szId);           \
		if(ch)                                                            \
			c->returnValue()->setBoolean(ch->__chanfunc(szNick, true));   \
		return true;                                                      \
	}

/*
	@doc: chan.isowner
	@type:
		function
	@title:
		$chan.isowner
	@short:
		Checks if a user is at least a channel owner
	@syntax:
		<boolean> $chan.isowner(<nickname:string>[,<window_id:string>])
	@description:
		Returns [b]1[/b] if <nickname> is at least an owner on the channel identified by <window_id>, and [b]0[/b] otherwise.[br]
		If <window_id> is not specified the current window is used (assuming that it is a channel at all).[br]
		If the window is not a channel, a warning is printed and an empty string is returned.[br]
		Note that if the user is not on the channel at all, you will get 0 as return value.[br]
*/

IS_KVS_FUNC(chan_kvs_fnc_isowner, isChanOwner)

/*
	@doc: chan.isadmin
	@type:
		function
	@title:
		$chan.isadmin
	@short:
		Checks if a user is at least channel administrator
	@syntax:
		<boolean> $chan.isadmin(<nickname:string>[,<window_id:string>])
	@description:
		Returns [b]1[/b] if <nickname> is at least an administrator on the channel identified by <window_id>, and [b]0[/b] otherwise.[br]
		If <window_id> is not specified the current window is used (assuming that it is a channel at all).[br]
		If the window is not a channel, a warning is printed and an empty string is returned.[br]
		Note that if the user is not on the channel at all, you will get 0 as return value.[br]
*/

IS_KVS_FUNC(chan_kvs_fnc_isadmin, isChanAdmin)

/*
	@doc: chan.isop
	@type:
		function
	@title:
		$chan.isop
	@short:
		Checks if a user is at least an op on a channel
	@syntax:
		<boolean> $chan.isop(<nickname:string>[,<window_id:string>])
	@description:
		Returns [b]1[/b] if <nickname> is at least an operator on the channel identified by <window_id>, and [b]0[/b] otherwise.[br]
		If <window_id> is not specified the current window is used (assuming that it is a channel at all).[br]
		If the window is not a channel, a warning is printed and an empty string is returned.[br]
		Note that if the user is not on the channel at all, you will get 0 as return value.[br]
*/

IS_KVS_FUNC(chan_kvs_fnc_isop, isOp)

/*
	@doc: chan.isvoice
	@type:
		function
	@title:
		$chan.isvoice
	@short:
		Checks if a user is at least voiced on a channel
	@syntax:
		<boolean> $chan.isvoice(<nickname:string>[,<window_id:string>])
	@description:
		Returns [b]1[/b] if <nickname> is at least voiced on the channel identified by <window_id>, and [b]0[/b] otherwise.[br]
		If <window_id> is not specified the current window is used (assuming that it is a channel at all).[br]
		If the window is not a channel, a warning is printed and an empty string is returned.[br]
		Note that if the user is not on the channel at all, you will get 0 as return value.[br]
*/

IS_KVS_FUNC(chan_kvs_fnc_isvoice, isVoice)

/*
	@doc: chan.ishalfop
	@type:
		function
	@title:
		$chan.ishalfop
	@short:
		Checks if a user is at least halfop on a channel
	@syntax:
		<boolean> $chan.ishalfop(<nickname:string>[,<window_id:string>])
	@description:
		Returns [b]1[/b] if <nickname> is at least a half-operator on the channel identified by <window_id>, and [b]0[/b] otherwise.[br]
		If <window_id> is not specified the current window is used (assuming that it is a channel at all).[br]
		If the window is not a channel, a warning is printed and an empty string is returned.[br]
		Note that if the user is not on the channel at all, you will get 0 as return value.[br]
*/

IS_KVS_FUNC(chan_kvs_fnc_ishalfop, isHalfOp)

/*
	@doc: chan.isuserop
	@type:
		function
	@title:
		$chan.isuserop
	@short:
		Checks if a user is at least a userop on a channel
	@syntax:
		<boolean> $chan.isuserop(<nickname:string>[,<window_id:string>])
	@description:
		Returns [b]1[/b] if <nickname> is at least a user-operator on the channel identified by <window_id>, and [b]0[/b] otherwise.[br]
		If <window_id> is not specified the current window is used (assuming that it is a channel at all).[br]
		If the window is not a channel, a warning is printed and an empty string is returned.[br]
		Note that if the user is not on the channel at all, you will get 0 as return value.[br]
*/

IS_KVS_FUNC(chan_kvs_fnc_isuserop, isUserOp)

#define IS_ME_KVS_FUNC(__clbkname, __chanfunc)                            \
	static bool __clbkname(KviKvsModuleFunctionCall * c)                  \
	{                                                                     \
		QString szId;                                                     \
		KVSM_PARAMETERS_BEGIN(c)                                          \
		KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId) \
		KVSM_PARAMETERS_END(c)                                            \
		KviChannelWindow * ch = chan_kvs_find_channel(c, szId);           \
		if(ch)                                                            \
			c->returnValue()->setBoolean(ch->__chanfunc(true));           \
		return true;                                                      \
	}

/*
	@doc: chan.ismeowner
	@type:
		function
	@title:
		$chan.isMeOwner
	@short:
		Checks if the current user is at least an owner on a channel
	@syntax:
		<boolean> $chan.isMeOwner
		<boolean> $chan.isMeOwner(<window_id:string>)
	@description:
		Returns [b]1[/b] if the current user is at least an owner on the channel specified by <window_id>, and [b]0[/b] otherwise.[br]
		If <window_id> is not passed, the current window is used (assuming it is a channel at all).[br]
		If the window is not a channel, a warning is printed and an empty string is returned.[br]
		This function is a [i]shortcut[/i] for [fnc]$chan.isowner[/fnc]([fnc]$me[/fnc]).[br]
*/

IS_ME_KVS_FUNC(chan_kvs_fnc_ismeowner, isMeChanOwner)

/*
	@doc: chan.ismeadmin
	@type:
		function
	@title:
		$chan.isMeAdmin
	@short:
		Checks if the current user is at least an administrator on a channel
	@syntax:
		<boolean> $chan.isMeAdmin
		<boolean> $chan.isMeAdmin(<window_id:string>)
	@description:
		Returns [b]1[/b] if the current user is at least an administrator on the channel specified by <window_id>, and [b]0[/b] otherwise.[br]
		If <window_id> is not passed, the current window is used (assuming it is a channel at all).[br]
		If the window is not a channel, a warning is printed and an empty string is returned.[br]
		This function is a [i]shortcut[/i] for [fnc]$chan.isadmin[/fnc]([fnc]$me[/fnc]).[br]
*/

IS_ME_KVS_FUNC(chan_kvs_fnc_ismeadmin, isMeChanAdmin)

/*
	@doc: chan.ismeop
	@type:
		function
	@title:
		$chan.isMeOp
	@short:
		Checks if the current user is at least op on a channel
	@syntax:
		<boolean> $chan.isMeOp
		<boolean> $chan.isMeOp(<window_id:string>)
	@description:
		Returns [b]1[/b] if the current user is at least op on the channel specified by <window_id>, and [b]0[/b] otherwise.[br]
		If <window_id> is not passed, the current window is used (assuming it is a channel at all).[br]
		If the window is not a channel, a warning is printed and an empty string is returned.[br]
		This function is a [i]shortcut[/i] for [fnc]$chan.isop[/fnc]([fnc]$me[/fnc]).[br]
*/

IS_ME_KVS_FUNC(chan_kvs_fnc_ismeop, isMeOp)

/*
	@doc: chan.ismehalfop
	@type:
		function
	@title:
		$chan.isMeHalfOp
	@short:
		Checks if the current user is at least an half operator on a channel
	@syntax:
		<boolean> $chan.isMeHalfOp
		<boolean> $chan.isMeHalfOp(<window_id:string>)
	@description:
		Returns [b]1[/b] if the current user is at least an half operator on the channel specified by <window_id>, and [b]0[/b] otherwise.[br]
		If <window_id> is not passed, the current window is used (assuming it is a channel at all).[br]
		If the window is not a channel, a warning is printed and an empty string is returned.[br]
		This function is a [i]shortcut[/i] for [fnc]$chan.ishalfop[/fnc]([fnc]$me[/fnc]).[br]
*/

IS_ME_KVS_FUNC(chan_kvs_fnc_ismehalfop, isMeHalfOp)

/*
	@doc: chan.ismevoice
	@type:
		function
	@title:
		$chan.isMeVoice
	@short:
		Checks if the current user is at least voice on a channel
	@syntax:
		<boolean> $chan.isMeVoice
		<boolean> $chan.isMeVoice(<window_id:string>)
	@description:
		Returns [b]1[/b] if the current user is at least voice on the channel specified by <window_id>, and [b]0[/b] otherwise.[br]
		If <window_id> is not passed, the current window is used (assuming it is a channel at all).[br]
		If the window is not a channel, a warning is printed and an empty string is returned.[br]
		This function is a [i]shortcut[/i] for [fnc]$chan.isvoice[/fnc]([fnc]$me[/fnc]).[br]
*/

IS_ME_KVS_FUNC(chan_kvs_fnc_ismevoice, isMeVoice)

/*
	@doc: chan.ismeuserop
	@type:
		function
	@title:
		$chan.isMeUserOp
	@short:
		Checks if the current user is at least a user operator on a channel
	@syntax:
		<boolean> $chan.isMeUserOp
		<boolean> $chan.isMeUserOp(<window_id:String>)
	@description:
		Returns [b]1[/b] if the current user is at least a user operator on the channel specified by <window_id>, and [b]0[/b] otherwise.[br]
		If <window_id> is not passed, the current window is used (assuming it is a channel at all).[br]
		If the window is not a channel, a warning is printed and an empty string is returned.[br]
		This function is a [i]shortcut[/i] for [fnc]$chan.isuserop[/fnc]([fnc]$me[/fnc]).[br]
*/

IS_ME_KVS_FUNC(chan_kvs_fnc_ismeuserop, isMeUserOp)

/*
	@doc: chan.mode
	@type:
		function
	@title:
		$chan.mode
	@short:
		Returns the mode string of a channel
	@syntax:
		<string> $chan.mode
		<string> $chan.mode(<window_id:string>)
	@description:
		Returns the mode string of the channel identified by <window_id>.[br]
		If no <window_id> is passed, the current channel mode string is returned (assuming that
		the current window is a channel at all).[br]
		If the window is not a channel, a warning is printed and an empty string is returned.[br]
*/

static bool chan_kvs_fnc_mode(KviKvsModuleFunctionCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szId);
	if(ch)
	{
		QString chMode;
		ch->getChannelModeString(chMode);
		c->returnValue()->setString(chMode);
	}
	return true;
}

/*
	@doc: chan.key
	@type:
		function
	@title:
		$chan.key
	@short:
		Returns the key of a channel
	@syntax:
		<string> $chan.key
		<string> $chan.key(<window_id:string>)
	@description:
		Returns the key of the channel identified by <window_id>.[br]
		If no <window_id> is passed, the current channel key is returned (assuming that
		the current window is a channel at all).[br]
		If the window is not a channel, a warning is printed and an empty string is returned.[br]
		If the channel has no key set, an empty string is returned.[br]
		Alternatively, you could use $chan.modeParam(k) to get the current key.
	@seealso:
		[fnc]$chan.mode[/fnc]
		[fnc]$chan.modeParam[/fnc]
*/

static bool chan_kvs_fnc_key(KviKvsModuleFunctionCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szId);
	if(ch)
		c->returnValue()->setString(ch->hasChannelMode('k') ? ch->channelModeParam('k') : "");
	return true;
}

/*
	@doc: chan.limit
	@type:
		function
	@title:
		$chan.limit
	@short:
		Returns the limit of a channel
	@syntax:
		<integer> $chan.limit
		<integer> $chan.limit(<window_id:string>)
	@description:
		Returns the limit of the channel identified by <window_id>.[br]
		If no <window_id> is passed, the current channel limit is returned (assuming that
		the current window is a channel at all).[br]
		If the window is not a channel, a warning is printed and an empty string is returned.[br]
		If the channel has no limit set, [b]0[/b] is returned.[br]
		Alternatively, you could use $chan.modeParam(l) to get the current limit.
	@seealso:
		[fnc]$chan.mode[/fnc]
		[fnc]$chan.modeParam[/fnc]
*/

static bool chan_kvs_fnc_limit(KviKvsModuleFunctionCall * c)
{
	QString szId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szId);
	if(ch)
	{
		kvs_int_t limit = 0;
		if(ch->hasChannelMode('l'))
			limit = ch->channelModeParam('l').toInt();
		c->returnValue()->setInteger(limit);
	}
	return true;
}

/*
	@doc: chan.modeParam
	@type:
		function
	@title:
		$chan.modeParam
	@short:
		Returns the parameter for a set mode of a channel
	@syntax:
		<string> $chan.modeParam(<mode:string>[,<window_id:string>])
	@description:
		Returns the parameter for a set mode of a channel identified by <window_id>.[br]
		If no <window_id> is passed, the current channel key is returned (assuming that
		the current window is a channel at all).[br]
		If the window is not a channel, a warning is printed and an empty string is returned.[br]
		If the channel has no such a mode set, an empty string is returned.[br]
		If you need to get the current channel key or user limit, you can use one of the specialized functions.[br]
	@seealso:
		[fnc]$chan.mode[/fnc]
		[fnc]$chan.key[/fnc]
		[fnc]$chan.limit[/fnc]
*/

static bool chan_kvs_fnc_modeParam(KviKvsModuleFunctionCall * c)
{
	QString szId;
	QString szMode;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("mode", KVS_PT_NONEMPTYSTRING, 0, szMode)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szId);
	char cMode = szMode.at(0).unicode();
	if(ch)
		c->returnValue()->setString(ch->hasChannelMode(cMode) ? ch->channelModeParam(cMode) : "");
	return true;
}

/*
	@doc: chan.users
	@type:
		function
	@title:
		$chan.users
	@short:
		Returns an array of channel user nicknames
	@syntax:
		<array> $chan.users([window_id:string],[mask:string],[flags:string])
	@description:
		Returns an array of nicknames on the channel specified by [window_id].[br]
		If [window_id] is empty, the current window is used.[br]
		If the window designated by [window_id] is not a channel a warning is printed and an empty array is returned.[br]
		If [mask] is given, each user is added to the array only
		if it matches the [mask].[br]
		[flags] may contain a subset of the letters [b]aovhnmi[/b]:[br]
		[b]ovhn[/b] are mode flags: the users are added to the array only if they are channel administrators [b]a[/b], operators [b]o[/b],
		voiced users [b]v[/b], half-operators [b]h[/b], user-operators [b]u[/b] or unflagged [b]n[/b] users. (Unflagged means not operators, not
		voiced and not half-operators). If none of the [b]ovhun[/b] flags are used, KVIrc behaves like all five were passed.[br]
		The flag [b]m[/b] causes the entire user masks to be added to the
		array entries, as known by KVIrc at the moment of this function call.[br]
		The flag [b]i[/b] causes KVIrc to invert the match and add only the users that do [b]not[/b] match [mask].[br]
		Please note that on really large channels this function may be time consuming (especially if [mask] is used):
		use with care.[br]
	@example:
		[example]
			[comment]# Get the nickname list[/comment]
			%test[] = $chan.users
			[comment]# And loop through the items[/comment]
			%i = 0
			[comment]# %test[]# returns the number of elements in the array[/comment]
			%count = %test[]#
			while(%i < %count)
			{
				echo "User %i : %test[%i]"
				%i++
			}
			[comment]# Another way of looping[/comment]
			foreach(%tmp,%test[])echo "User %tmp"
			[comment]# Find all the users that come from the *.org domain[/comment]
			%test[]=$chan.users(,*!*@*.org)
			echo %test[]
			[comment]# This will work too![/comment]
			echo $chan.users(,*!*@*.org)
			[comment]# Find all the channel operators[/comment]
			%test[] = $chan.users(,,o)
			echo %test[]
			[comment]# Find all the voiced users that do not come from *.edu[/comment]
			[comment]# See also their whole mask[/comment]
			%test[] = $chan.users(,*!*@*.edu,vim)
			echo %test[]
		[/example]

*/

static bool chan_kvs_fnc_users(KviKvsModuleFunctionCall * c)
{
	QString szWinId, szMask, szFlags;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szWinId)
	KVSM_PARAMETER("mask", KVS_PT_STRING, KVS_PF_OPTIONAL, szMask)
	KVSM_PARAMETER("flags", KVS_PT_STRING, KVS_PF_OPTIONAL, szFlags)
	KVSM_PARAMETERS_END(c)

	KviKvsArray * pArray = new KviKvsArray();
	c->returnValue()->setArray(pArray);

	KviChannelWindow * ch = chan_kvs_find_channel(c, szWinId);
	if(!ch)
		return true;

	KviUserListEntry * e = ch->userListView()->firstItem(); // Thnx Julien :)

	bool bCheckMask = !szMask.isEmpty();
	bool bOp = szFlags.contains(QChar('o'), Qt::CaseSensitive);
	bool bVoice = szFlags.contains(QChar('v'), Qt::CaseSensitive);
	bool bHalfOp = szFlags.contains(QChar('h'), Qt::CaseSensitive);
	bool bChanAdmins = szFlags.contains(QChar('a'), Qt::CaseSensitive);
	bool bUserOp = szFlags.contains(QChar('u'), Qt::CaseSensitive);
	bool bNone = szFlags.contains(QChar('n'), Qt::CaseSensitive);
	bool bCheckFlags = bOp || bVoice || bHalfOp || bNone || bChanAdmins || bUserOp;
	bool bAddMask = szFlags.contains(QChar('m'), Qt::CaseSensitive);

	int idx = 0;

	if(bAddMask || bCheckFlags || bCheckMask)
	{
		bool bMaskMustMatch = !szFlags.contains(QChar('i'), Qt::CaseSensitive);
		KviIrcMask mask(szMask);

		while(e)
		{
			if(bCheckFlags)
			{
				if(bChanAdmins)
				{
					if(e->flags() & KviIrcUserEntry::ChanAdmin)
						goto check_mask;
				}
				if(bOp)
				{
					if(e->flags() & KviIrcUserEntry::Op)
						goto check_mask;
				}
				if(bVoice)
				{
					if(e->flags() & KviIrcUserEntry::Voice)
						goto check_mask;
				}
				if(bHalfOp)
				{
					if(e->flags() & KviIrcUserEntry::HalfOp)
						goto check_mask;
				}
				if(bUserOp)
				{
					if(e->flags() & KviIrcUserEntry::UserOp)
						goto check_mask;
				}
				if(bNone)
				{
					if(!(e->flags() & KviIrcUserEntry::ModeMask))
						goto check_mask;
				}
				goto next_item;
			}
		check_mask:
			if(bCheckMask)
			{
				if(mask.matchesFixed(e->nick(), e->globalData()->user(), e->globalData()->host()) == bMaskMustMatch)
					goto add_item;
				goto next_item;
			}
		add_item:
			if(bAddMask)
			{
				QString x(e->nick());
				x.append('!');
				x.append(e->globalData()->user());
				x.append('@');
				x.append(e->globalData()->host());
				pArray->set(idx, new KviKvsVariant(x));
			}
			else
			{
				pArray->set(idx, new KviKvsVariant(e->nick()));
			}
			idx++;
		next_item:
			e = e->next();
		}
	}
	else
	{
		while(e)
		{
			pArray->set(idx, new KviKvsVariant(e->nick()));
			idx++;
			e = e->next();
		}
	}

	return true;
}

/*
	@doc: chan.banlist
	@type:
		function
	@title:
		$chan.banlist
	@short:
		Returns an array of channel ban masks
	@syntax:
		$chan.banlist([window_id])
	@description:
		Returns an array of ban masks set on the channel identified by [window_id].[br]
		If [window_id] is empty, the current window is used.[br]
		If the window designated by [window_id] is not a channel a warning is printed and an empty array is returned.[br]
	@seealso:
		[fnc]$chan.masklist[/fnc]
		[fnc]$chan.invitelist[/fnc]
		[fnc]$chan.banexceptionlist[/fnc]
*/

static bool chan_kvs_fnc_banlist(KviKvsModuleFunctionCall * c)
{
	QString szWinId, szMask, szFlags;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szWinId)
	KVSM_PARAMETERS_END(c)

	KviKvsArray * pArray = new KviKvsArray();
	c->returnValue()->setArray(pArray);

	KviChannelWindow * ch = chan_kvs_find_channel(c, szWinId);

	if(!ch)
		return true;

	const std::vector<KviMaskEntry *> & l = ch->modeMasks('b');
	if(l.empty())
		return true;

	int idx = 0;
	for(auto e : l)
	{
		pArray->set(idx, new KviKvsVariant(e->szMask));
		idx++;
	}

	return true;
}

/*
	@doc: chan.banexceptionlist
	@type:
		function
	@title:
		$chan.banexceptionlist
	@short:
		Returns an array of channel ban exception masks
	@syntax:
		<array> $chan.banexceptionlist([window_id])
	@description:
		Returns an array of ban exception masks set on the channel identified by [window_id].[br]
		If [window_id] is empty, the current window is used.[br]
		If the window designated by [window_id] is not a channel a warning is printed and an empty array is returned.[br]
	@seealso:
		[fnc]$chan.banlist[/fnc]
		[fnc]$chan.invitelist[/fnc]
		[fnc]$chan.masklist[/fnc]
*/

static bool chan_kvs_fnc_banexceptionlist(KviKvsModuleFunctionCall * c)
{
	QString szWinId, szMask, szFlags;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szWinId)
	KVSM_PARAMETERS_END(c)

	KviKvsArray * pArray = new KviKvsArray();
	c->returnValue()->setArray(pArray);

	KviChannelWindow * ch = chan_kvs_find_channel(c, szWinId);

	if(!ch)
		return true;

	const std::vector<KviMaskEntry *> & l = ch->modeMasks('e');
	if(l.empty())
		return true;

	int idx = 0;
	for(auto e : l)
	{
		pArray->set(idx, new KviKvsVariant(e->szMask));
		idx++;
	}

	return true;
}

/*
	@doc: chan.invitelist
	@type:
		function
	@title:
		$chan.invitelist
	@short:
		Returns an array of channel invite masks
	@syntax:
		<array> $chan.invitelist([window_id])
	@description:
		Returns an array of invite masks set on the channel identified by [window_id].[br]
		If [window_id] is empty, the current window is used.[br]
		If the window designated by [window_id] is not a channel a warning is printed and an empty array is returned.[br]
	@seealso:
		[fnc]$chan.banlist[/fnc]
		[fnc]$chan.masklist[/fnc]
		[fnc]$chan.banexceptionlist[/fnc]
*/

static bool chan_kvs_fnc_invitelist(KviKvsModuleFunctionCall * c)
{
	QString szWinId, szMask, szFlags;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szWinId)
	KVSM_PARAMETERS_END(c)

	KviKvsArray * pArray = new KviKvsArray();
	c->returnValue()->setArray(pArray);

	KviChannelWindow * ch = chan_kvs_find_channel(c, szWinId);

	if(!ch)
		return true;

	const std::vector<KviMaskEntry *> & l = ch->modeMasks('I');
	if(l.empty())
		return true;

	int idx = 0;
	for(auto e : l)
	{
		pArray->set(idx, new KviKvsVariant(e->szMask));
		idx++;
	}

	return true;
}

/*
	@doc: chan.masklist
	@type:
		function
	@title:
		$chan.masklist
	@short:
		Returns an array of masks for a channel mode
	@syntax:
		$chan.masklist(<mode:char>[,window_id])
	@description:
		Returns an array of masks set for the channel mode <mode> on the channel identified by [window_id].[br]
		If [window_id] is empty, the current window is used.[br]
		If the window designated by [window_id] is not a channel a warning is printed and an empty array is returned.[br]
	@seealso:
		[fnc]$chan.banlist[/fnc]
		[fnc]$chan.invitelist[/fnc]
		[fnc]$chan.banexceptionlist[/fnc]
*/

static bool chan_kvs_fnc_masklist(KviKvsModuleFunctionCall * c)
{
	QString szWinId, szMask, szMode;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("mode", KVS_PT_NONEMPTYSTRING, 0, szMode)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szWinId)
	KVSM_PARAMETERS_END(c)

	char cMode = szMode.at(0).unicode();

	KviKvsArray * pArray = new KviKvsArray();
	c->returnValue()->setArray(pArray);

	KviChannelWindow * ch = chan_kvs_find_channel(c, szWinId);

	if(!ch)
		return true;

	const std::vector<KviMaskEntry *> & l = ch->modeMasks(cMode);
	if(l.empty())
		return true;

	int idx = 0;
	for(auto e : l)
	{
		pArray->set(idx, new KviKvsVariant(e->szMask));
		idx++;
	}

	return true;
}

/*
	@doc: chan.matchban
	@type:
		function
	@title:
		$chan.matchban
	@short:
		Matches a mask against the channel ban list
	@syntax:
		<string> $chan.matchban([window_id],<complete_mask>)
	@description:
		Returns the ban mask that matches <complete_mask> on channel identified by [window_id].[br]
		If no ban mask matches <complete_mask> an empty string is returned.[br]
		If [window_id] is empty, the current window is used.[br]
		If the window designated by [window_id] is not a channel a warning is printed and an empty string is returned.[br]
		This function is useful to determine if a ban set on the channel matches a user.[br]
*/

static bool chan_kvs_fnc_matchban(KviKvsModuleFunctionCall * c)
{
	QString szWinId, szMask;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, 0, szWinId)
	KVSM_PARAMETER("mask", KVS_PT_STRING, 0, szMask)
	KVSM_PARAMETERS_END(c)

	KviChannelWindow * ch = chan_kvs_find_channel(c, szWinId);

	if(!ch)
	{
		c->returnValue()->setNothing();
		return true;
	}

	const std::vector<KviMaskEntry *> & l = ch->modeMasks('b');
	if(l.empty())
	{
		c->returnValue()->setNothing();
		return true;
	}

	for(auto e : l)
	{
		if(KviQString::matchString(e->szMask, szMask))
		{
			c->returnValue()->setString(e->szMask);
			return true;
		}
	}

	c->returnValue()->setNothing();
	return true;
}

/*
	@doc: chan.matchbanexception
	@type:
		function
	@title:
		$chan.matchbanexception
	@short:
		Matches a mask against the channel ban exception list
	@syntax:
		<string> $chan.matchbanexception([window_id],<complete_mask>)
	@description:
		Returns the ban exception mask that matches <complete_mask> on channel identified by [window_id].[br]
		If no ban exception mask matches <complete_mask> an empty string is returned.[br]
		If [window_id] is empty, the current window is used.[br]
		If the window designated by [window_id] is not a channel a warning is printed and an empty string is returned.[br]
		This function is useful to determine if a ban exception set on the channel matches a user.[br]
*/

static bool chan_kvs_fnc_matchbanexception(KviKvsModuleFunctionCall * c)
{
	QString szWinId, szMask;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, 0, szWinId)
	KVSM_PARAMETER("mask", KVS_PT_STRING, 0, szMask)
	KVSM_PARAMETERS_END(c)

	KviChannelWindow * ch = chan_kvs_find_channel(c, szWinId);

	if(!ch)
	{
		c->returnValue()->setNothing();
		return true;
	}

	const std::vector<KviMaskEntry *> & l = ch->modeMasks('e');
	if(l.empty())
	{
		c->returnValue()->setNothing();
		return true;
	}

	for(auto e : l)
	{
		if(KviQString::matchString(e->szMask, szMask))
		{
			c->returnValue()->setString(e->szMask);
			return true;
		}
	}

	c->returnValue()->setNothing();
	return true;
}

/*
	@doc: chan.matchinvite
	@type:
		function
	@title:
		$chan.matchinvite
	@short:
		Matches a mask against the channel invite list
	@syntax:
		<string> $chan.matchinvite([window_id:string],<complete_mask>)
	@description:
		Returns the invite mask that matches <complete_mask> on channel identified by [window_id].[br]
		If no invite mask matches <complete_mask> an empty string is returned.[br]
		If [window_id] is empty, the current window is used.[br]
		If the window designated by [window_id] is not a channel a warning is printed and an empty string is returned.[br]
		This function is useful to determine if a invite set on the channel matches a user.[br]
*/

static bool chan_kvs_fnc_matchinvite(KviKvsModuleFunctionCall * c)
{
	QString szWinId, szMask;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, 0, szWinId)
	KVSM_PARAMETER("mask", KVS_PT_STRING, 0, szMask)
	KVSM_PARAMETERS_END(c)

	KviChannelWindow * ch = chan_kvs_find_channel(c, szWinId);

	if(!ch)
	{
		c->returnValue()->setNothing();
		return true;
	}

	const std::vector<KviMaskEntry *> & l = ch->modeMasks('I');
	if(l.empty())
	{
		c->returnValue()->setNothing();
		return true;
	}

	for(auto e : l)
	{
		if(KviQString::matchString(e->szMask, szMask))
		{
			c->returnValue()->setString(e->szMask);
			return true;
		}
	}

	c->returnValue()->setNothing();
	return true;
}

/*
	@doc: chan.matchqban
	@type:
		function
	@title:
		$chan.matchqban
	@short:
		Matches a mask against the channel ban list searching for +q bans (aka mute bans)
	@syntax:
		<string> $chan.matchqban([window_id],<complete_mask>)
	@description:
		Warning: this function is network-specific, makes some (bad) assumptions about a non-standard channel mode and
		will probably be dropped in a future version.[br]
		Use [fnc]$chan.matchmask[/fnc] instead.[br]
		Some networks use +q channel mode to set [i]mute bans[/i].[br]
		When a user mask matches such a ban, he won't be able to send messages to the channel.[br]
		The [i]mute bans[/i] masks will be inserted in the normal channel bans list, with a percent sign % prepended.[br]
		This function returns the [i]mute ban[/i] mask that matches <complete_mask> on channel identified by [window_id].[br]
		If no ban mask matches <complete_mask> an empty string is returned.[br]
		If [window_id] is empty, the current window is used.[br]
		If the window designated by [window_id] is not a channel a warning is printed and an empty string is returned.[br]
		This function is useful to determine if a [i]mute ban[/i] set on the channel matches a user.[br]
*/

static bool chan_kvs_fnc_matchqban(KviKvsModuleFunctionCall * c)
{
	QString szWinId, szMask;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("window id", KVS_PT_STRING, 0, szWinId)
	KVSM_PARAMETER("mask", KVS_PT_STRING, 0, szMask)
	KVSM_PARAMETERS_END(c)

	KviChannelWindow * ch = chan_kvs_find_channel(c, szWinId);
	szMask.prepend('%');

	if(!ch)
	{
		c->returnValue()->setNothing();
		return true;
	}

	const std::vector<KviMaskEntry *> & l = ch->modeMasks('b');
	if(l.empty())
	{
		c->returnValue()->setNothing();
		return true;
	}

	for(auto e : l)
	{
		if(KviQString::matchString(e->szMask, szMask))
		{
			c->returnValue()->setString(e->szMask);
			return true;
		}
	}

	c->returnValue()->setNothing();
	return true;
}

/*
	@doc: chan.matchmask
	@type:
		function
	@title:
		$chan.matchmask
	@short:
		Matches a mask against the channel list for a specific mode
	@syntax:
		<string> $chan.matchmask(<mode:char>,<complete_mask:string>[,window_id])
	@description:
		Matches the provided <complete_mask> against the mask list for channel mode <mode> on the channel identified by [window_id].[br]
		If no mask matches <complete_mask> an empty string is returned.[br]
		If [window_id] is empty, the current window is used.[br]
		If the window designated by [window_id] is not a channel a warning is printed and an empty string is returned.[br]
		Please note that some IRC servers use channel list modes not only to store masks, but also for other data such
		as nicknames. This function should be able to match them as long as they are strings.[br]
*/

static bool chan_kvs_fnc_matchmask(KviKvsModuleFunctionCall * c)
{
	QString szWinId, szMask, szMode;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("mode", KVS_PT_NONEMPTYSTRING, 0, szMode)
	KVSM_PARAMETER("mask", KVS_PT_STRING, 0, szMask)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szWinId)
	KVSM_PARAMETERS_END(c)

	char cMode = szMode.at(0).unicode();
	KviChannelWindow * ch = chan_kvs_find_channel(c, szWinId);

	if(!ch)
	{
		c->returnValue()->setNothing();
		return true;
	}

	const std::vector<KviMaskEntry *> & l = ch->modeMasks(cMode);
	if(l.empty())
	{
		c->returnValue()->setNothing();
		return true;
	}

	for(auto e : l)
	{
		if(KviQString::matchString(e->szMask, szMask))
		{
			c->returnValue()->setString(e->szMask);
			return true;
		}
	}

	c->returnValue()->setNothing();
	return true;
}

/*
	@doc: chan.usermodelevel
	@type:
		function
	@title:
		$chan.usermodelevel
	@short:
		Returns the channel user-mode level
	@syntax:
		<integer> $chan.userModeLevel(<nickname:string>[,<window_id:string>])
	@description:
		Returns an integer identifying the specified user's channel mode on the channel specified by <window_id>.[br]
		If <window_id> is not passed, the current window is used.[br]
		If the specified window is not a channel, a warning is printed and [b]0[/b] is returned.[br]
		This number can be useful to implement comparison functions between
		users in order to determine the actions they can issue between each other.[br]
		For example it is granted that an op will have userModeLevel greater than
		a voiced user or that a simple [i]modeless[/i] user will have
		a userModeLevel lower than a halfop.[br]
		IRC allows multiple modes to be applied to a single user on a channel,
		in that case this function will return the level of the highest mode
		applied to the user.[br]
		Note: Don't rely on this number being any particular exact value except
		for the completely modeless users (in which case this function will return always [b]0[/b]).
*/

static bool chan_kvs_fnc_usermodelevel(KviKvsModuleFunctionCall * c)
{
	QString szNick, szWinId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("nickname", KVS_PT_STRING, 0, szNick)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szWinId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szWinId);

	kvs_int_t mode = 0;
	if(ch)
		mode = ch->userListView()->getUserModeLevel(szNick);
	c->returnValue()->setInteger(mode);
	return true;
}

/*
	@doc: chan.userjointime
	@type:
		function
	@title:
		$chan.userJoinTime
	@short:
		Returns the time that a user has joined the channel
	@syntax:
		<integer> $chan.userJoinTime(<nickname:string>[,<window_id:string>])
	@description:
		Returns the UNIX time at which the user specified by <nickname> has joined the channel specified by <window_id>.[br]
		If <window_id> is not passed, the current window is used.[br]
		If the specified window is not a channel, a warning is printed and [b]0[/b] is returned.[br]
		[b]0[/b] is also returned when the user's join time is unknown: this is true
		for all the users that were on the channel before the local user has joined it (e.g.
		you know the join time only for users that YOU see joining).[br]
	@seealso:
		[fnc]$unixTime[/fnc], [fnc]$chan.userLastActionTime[/fnc]
*/

static bool chan_kvs_fnc_userjointime(KviKvsModuleFunctionCall * c)
{
	QString szNick, szWinId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("nickname", KVS_PT_STRING, 0, szNick)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szWinId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szWinId);

	kvs_int_t time = 0;
	if(ch)
		time = ch->userListView()->getUserJoinTime(szNick);
	c->returnValue()->setInteger(time);
	return true;
}

/*
	@doc: chan.userlastactiontime
	@type:
		function
	@title:
		$chan.userLastActionTime
	@short:
		Returns the time that a user has last performed some kind of action on the channel
	@syntax:
		<integer> $chan.userLastActionTime(<nickname:string>[,<window_id:string>])
	@description:
		Returns the UNIX time at which the user specified by <nickname> has performed
		some kind of action on the channel specified by <window_id>.[br]
		If <window_id> is not passed, the current window is used.[br]
		If the specified window is not a channel, a warning is printed and [b]0[/b] is returned.[br]
		[b]0[/b] is also returned when the user's last action time is unknown: this is true
		for all the users that were on the channel before the local user has joined it
		and have performed no actions since that moment.
	@seealso:
		[fnc]$unixTime[/fnc], [fnc]$chan.userJoinTime[/fnc]
*/

static bool chan_kvs_fnc_userlastactiontime(KviKvsModuleFunctionCall * c)
{
	QString szNick, szWinId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("nickname", KVS_PT_STRING, 0, szNick)
	KVSM_PARAMETER("window id", KVS_PT_STRING, KVS_PF_OPTIONAL, szWinId)
	KVSM_PARAMETERS_END(c)
	KviChannelWindow * ch = chan_kvs_find_channel(c, szWinId);

	kvs_int_t time = 0;
	if(ch)
		time = ch->userListView()->getUserLastActionTime(szNick);
	c->returnValue()->setInteger(time);
	return true;
}

/*
	@doc: chan.common
	@type:
		function
	@title:
		$chan.common
	@short:
		Returns the common channel with a user
	@syntax:
		<string> $chan.common(<nickname:string>[,<context_id:string>])
	@description:
		Returns the common channel with the user specified by <nickname>.[br]
		If <context_id> is not passed, the current context is used.[br]
		If the specified window is not a channel, a warning is printed and [b]0[/b] is returned.
*/

static bool chan_kvs_fnc_common(KviKvsModuleFunctionCall * c)
{
	QString szNick, szChans;
	kvs_uint_t iContextId;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("nickname", KVS_PT_STRING, 0, szNick)
	KVSM_PARAMETER("context_id", KVS_PT_UINT, KVS_PF_OPTIONAL, iContextId)
	KVSM_PARAMETERS_END(c)

	KviConsoleWindow * pConsole = nullptr;
	if(c->parameterCount() > 1)
		pConsole = g_pApp->findConsole(iContextId);
	else
		pConsole = c->window()->console();

	if(pConsole && pConsole->connection())
	{
		pConsole->connection()->getCommonChannels(szNick, szChans, false);
		c->returnValue()->setString(szChans);
	}

	return true;
}

static bool chan_module_init(KviModule * m)
{
	KVSM_REGISTER_FUNCTION(m, "admincount", chan_kvs_fnc_admincount);
	KVSM_REGISTER_FUNCTION(m, "bancount", chan_kvs_fnc_bancount);
	KVSM_REGISTER_FUNCTION(m, "banexceptioncount", chan_kvs_fnc_banexceptioncount);
	KVSM_REGISTER_FUNCTION(m, "banexceptionlist", chan_kvs_fnc_banexceptionlist);
	KVSM_REGISTER_FUNCTION(m, "banlist", chan_kvs_fnc_banlist);
	KVSM_REGISTER_FUNCTION(m, "common", chan_kvs_fnc_common);
	KVSM_REGISTER_FUNCTION(m, "getflag", chan_kvs_fnc_getflag);
	KVSM_REGISTER_FUNCTION(m, "geturl", chan_kvs_fnc_getUrl);
	KVSM_REGISTER_FUNCTION(m, "halfopcount", chan_kvs_fnc_halfopcount);
	KVSM_REGISTER_FUNCTION(m, "invitecount", chan_kvs_fnc_invitecount);
	KVSM_REGISTER_FUNCTION(m, "invitelist", chan_kvs_fnc_invitelist);
	KVSM_REGISTER_FUNCTION(m, "isadmin", chan_kvs_fnc_isadmin);
	KVSM_REGISTER_FUNCTION(m, "isdead", chan_kvs_fnc_isdead);
	KVSM_REGISTER_FUNCTION(m, "ishalfop", chan_kvs_fnc_ishalfop);
	KVSM_REGISTER_FUNCTION(m, "ismeadmin", chan_kvs_fnc_ismeadmin);
	KVSM_REGISTER_FUNCTION(m, "ismehalfop", chan_kvs_fnc_ismehalfop);
	KVSM_REGISTER_FUNCTION(m, "ismeop", chan_kvs_fnc_ismeop);
	KVSM_REGISTER_FUNCTION(m, "ismeowner", chan_kvs_fnc_ismeowner);
	KVSM_REGISTER_FUNCTION(m, "ismeuserop", chan_kvs_fnc_ismeuserop);
	KVSM_REGISTER_FUNCTION(m, "ismevoice", chan_kvs_fnc_ismevoice);
	KVSM_REGISTER_FUNCTION(m, "ison", chan_kvs_fnc_ison);
	KVSM_REGISTER_FUNCTION(m, "isop", chan_kvs_fnc_isop);
	KVSM_REGISTER_FUNCTION(m, "isowner", chan_kvs_fnc_isowner);
	KVSM_REGISTER_FUNCTION(m, "isuserop", chan_kvs_fnc_isuserop);
	KVSM_REGISTER_FUNCTION(m, "isvoice", chan_kvs_fnc_isvoice);
	KVSM_REGISTER_FUNCTION(m, "key", chan_kvs_fnc_key);
	KVSM_REGISTER_FUNCTION(m, "limit", chan_kvs_fnc_limit);
	KVSM_REGISTER_FUNCTION(m, "masklist", chan_kvs_fnc_masklist);
	KVSM_REGISTER_FUNCTION(m, "maskcount", chan_kvs_fnc_maskcount);
	KVSM_REGISTER_FUNCTION(m, "matchban", chan_kvs_fnc_matchban);
	KVSM_REGISTER_FUNCTION(m, "matchqban", chan_kvs_fnc_matchqban);
	KVSM_REGISTER_FUNCTION(m, "matchbanexception", chan_kvs_fnc_matchbanexception);
	KVSM_REGISTER_FUNCTION(m, "matchinvite", chan_kvs_fnc_matchinvite);
	KVSM_REGISTER_FUNCTION(m, "matchmask", chan_kvs_fnc_matchmask);
	KVSM_REGISTER_FUNCTION(m, "mode", chan_kvs_fnc_mode);
	KVSM_REGISTER_FUNCTION(m, "modeParam", chan_kvs_fnc_modeParam);
	KVSM_REGISTER_FUNCTION(m, "name", chan_kvs_fnc_name);
	KVSM_REGISTER_FUNCTION(m, "opcount", chan_kvs_fnc_opcount);
	KVSM_REGISTER_FUNCTION(m, "ownercount", chan_kvs_fnc_ownercount);
	KVSM_REGISTER_FUNCTION(m, "topic", chan_kvs_fnc_topic);
	KVSM_REGISTER_FUNCTION(m, "topicsetat", chan_kvs_fnc_topicsetat);
	KVSM_REGISTER_FUNCTION(m, "topicsetby", chan_kvs_fnc_topicsetby);
	KVSM_REGISTER_FUNCTION(m, "usercount", chan_kvs_fnc_usercount);
	KVSM_REGISTER_FUNCTION(m, "userjointime", chan_kvs_fnc_userjointime);
	KVSM_REGISTER_FUNCTION(m, "userlastactiontime", chan_kvs_fnc_userlastactiontime);
	KVSM_REGISTER_FUNCTION(m, "usermodelevel", chan_kvs_fnc_usermodelevel);
	KVSM_REGISTER_FUNCTION(m, "useropcount", chan_kvs_fnc_useropcount);
	KVSM_REGISTER_FUNCTION(m, "users", chan_kvs_fnc_users);
	KVSM_REGISTER_FUNCTION(m, "voicecount", chan_kvs_fnc_voicecount);

	return true;
}

static bool chan_module_cleanup(KviModule *)
{
	return true;
}

KVIRC_MODULE(
    "Chan",  // module name
    "4.0.0", // module version
    "Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)"
    "          (C) 2002 Juanjo Alvarez (juanjux at yahoo dot es)",
    "Scripting interface for the channel management",
    chan_module_init,
    0,
    0,
    chan_module_cleanup,
    0)
