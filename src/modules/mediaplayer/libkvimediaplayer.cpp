//=============================================================================
//
//   File : libkvimediaplayer.cpp
//   Creation date : Sat Nov  3 19:28:25 2001 GMT by Szymon Stefanek
//   Renamed to file : libkvimediaplayer.cpp on Fri Mar 25 2005
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2008 Tomasz Moń (desowin at gmail dot com)
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

#include "MpInterface.h"
#include "MpAmipInterface.h"
#include "MpMprisInterface.h"
#include "MpWinampInterface.h"
#include "MpXmmsInterface.h"

#include "KviModule.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "kvi_out.h"
#include <QUrl>

static KviPointerList<MpInterfaceDescriptor> * g_pDescriptorList = nullptr;

static MpInterface * g_pMPInterface = nullptr;

static MpInterface * auto_detect_player(KviWindow * pOut = nullptr)
{
	int iBest = 0;
	MpInterface * pBest = nullptr;
	MpInterfaceDescriptor * d;
	MpInterfaceDescriptor * pDBest = nullptr;
	for(d = g_pDescriptorList->first(); d; d = g_pDescriptorList->next())
	{
		MpInterface * i = d->instance();
		if(i)
		{
			int iScore = i->detect(false);
			if(iScore > iBest)
			{
				iBest = iScore;
				pBest = i;
				pDBest = d;
			}
			if(pOut)
			{
				QString szOut;
				QString szNam = d->name();
				szOut = QString(__tr2qs_ctx("Trying media player interface \"%1\": score %2", "mediaplayer")).arg(szNam).arg(iScore);
				pOut->output(KVI_OUT_MULTIMEDIA, szOut);
			}
		}
	}
	if(iBest < 90)
	{
		if(pOut)
			pOut->outputNoFmt(KVI_OUT_MULTIMEDIA, __tr2qs_ctx("Not sure about the results, trying a second, more aggressive detection pass", "mediaplayer"));
		// no sure player found... try again with a destructive test
		for(d = g_pDescriptorList->first(); d; d = g_pDescriptorList->next())
		{
			MpInterface * i = d->instance();
			if(i)
			{
				int iScore = i->detect(true);
				if(iScore > iBest)
				{
					iBest = iScore;
					pBest = i;
					pDBest = d;
				}
				if(pOut)
				{
					QString szOut;
					QString szNam = d->name();
					szOut = QString(__tr2qs_ctx("Trying media player interface \"%1\": score %2", "mediaplayer")).arg(szNam).arg(iScore);
					pOut->output(KVI_OUT_MULTIMEDIA, szOut);
				}
			}
		}
	}
	if(pDBest)
	{
		KVI_OPTION_STRING(KviOption_stringPreferredMediaPlayer) = pDBest->name();
		if(pOut)
			pOut->output(KVI_OUT_MULTIMEDIA, __tr2qs_ctx("Choosing media player interface \"%Q\"", "mediaplayer"), &(KVI_OPTION_STRING(KviOption_stringPreferredMediaPlayer)));
	}
	else
	{
		if(pOut)
			pOut->outputNoFmt(KVI_OUT_MULTIMEDIA, __tr2qs_ctx("Seems that there is no usable media player on this machine", "mediaplayer"));
	}
	return pBest;
}

#define MP_KVS_FAIL_ON_NO_INTERFACE                                                                           \
	if(!g_pMPInterface)                                                                                       \
	{                                                                                                         \
		c->warning(__tr2qs_ctx("No mediaplayer interface selected. Try /mediaplayer.detect", "mediaplayer")); \
		return true;                                                                                          \
	}

#define MP_KVS_COMMAND(__name) static bool mediaplayer_kvs_cmd_##__name(KviKvsModuleCommandCall * c)
#define MP_KVS_FUNCTION(__name) static bool mediaplayer_kvs_fnc_##__name(KviKvsModuleFunctionCall * c)

#define MP_KVS_SIMPLE_COMMAND(__name, __ifacecommand)                                                                                   \
	MP_KVS_COMMAND(__name)                                                                                                              \
	{                                                                                                                                   \
		KVSM_PARAMETERS_BEGIN(c)                                                                                                        \
		KVSM_PARAMETERS_END(c)                                                                                                          \
                                                                                                                                        \
		MP_KVS_FAIL_ON_NO_INTERFACE                                                                                                     \
                                                                                                                                        \
		if(!g_pMPInterface->__ifacecommand())                                                                                           \
		{                                                                                                                               \
			if(!c->hasSwitch('q', "quiet"))                                                                                             \
			{                                                                                                                           \
				c->warning(__tr2qs_ctx("The selected media player interface failed to execute the requested function", "mediaplayer")); \
				QString tmp = __tr2qs_ctx("Last interface error: ", "mediaplayer");                                                     \
				tmp += g_pMPInterface->lastError();                                                                                     \
				c->warning(tmp);                                                                                                        \
			}                                                                                                                           \
		}                                                                                                                               \
		return true;                                                                                                                    \
	}

#define MP_KVS_SIMPLE_STRING_FUNCTION(__name, __ifacecommand) \
	MP_KVS_FUNCTION(__name)                                   \
	{                                                         \
		MP_KVS_FAIL_ON_NO_INTERFACE                           \
		QString szRet = g_pMPInterface->__ifacecommand();     \
		c->returnValue()->setString(szRet);                   \
		return true;                                          \
	}

#define MP_KVS_SIMPLE_INT_FUNCTION(__name, __ifacecommand) \
	MP_KVS_FUNCTION(__name)                                \
	{                                                      \
		MP_KVS_FAIL_ON_NO_INTERFACE                        \
		int iRet = g_pMPInterface->__ifacecommand();       \
		c->returnValue()->setInteger(iRet);                \
		return true;                                       \
	}
// FINDME!
#define MP_KVS_SIMPLE_BOOL_FUNCTION(__name, __ifacecommand) \
	MP_KVS_FUNCTION(__name)                                 \
	{                                                       \
		MP_KVS_FAIL_ON_NO_INTERFACE                         \
		bool bRet = g_pMPInterface->__ifacecommand();       \
		c->returnValue()->setBoolean(bRet);                 \
		return true;                                        \
	}

/*
	@doc: mediaplayer.play
	@type:
		command
	@title:
		mediaplayer.play
	@short:
		Starts media playback in the selected media player
	@syntax:
		mediaplayer.play [-q]
	@description:
		Starts playback in the remote instance of the currently selected media player.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[cmd]mediaplayer.stop[/cmd],
		[cmd]mediaplayer.pause[/cmd],
		[cmd]mediaplayer.prev[/cmd],
		[cmd]mediaplayer.next[/cmd],
		[fnc]$mediaplayer.nowPlaying[/fnc],
		[fnc]$mediaplayer.status[/fnc]
*/

MP_KVS_SIMPLE_COMMAND(play, play)

/*
	@doc: mediaplayer.hide
	@type:
		command
	@title:
		mediaplayer.hide
	@short:
		Hide the mediaplayer [-q]
	@syntax:
		mediaplayer.hide
	@description:
		Hide the player interface.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[cmd]mediaplayer.quit[/cmd],
		[cmd]mediaplayer.show[/cmd],
		[cmd]mediaplayer.minimize[/cmd]
*/

MP_KVS_SIMPLE_COMMAND(hide, hide)

/*
	@doc: mediaplayer.show
	@type:
		command
	@title:
		mediaplayer.show
	@short:
		Show the mediaplayer.
	@syntax:
		mediaplayer.show [-q]
	@description:
		Show the player interface.[br]
		When you use this function in your script, remember to reshow mediaplayer's window [br]
		or quit the mediaplayer window to the exit.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[cmd]mediaplayer.quit[/cmd],
		[cmd]mediaplayer.hide[/cmd],
		[cmd]mediaplayer.minimize[/cmd]
*/

MP_KVS_SIMPLE_COMMAND(show, show)

/*
	@doc: mediaplayer.minimize
	@type:
		command
	@title:
		mediaplayer.minimize
	@short:
		Minimize the mediaplayer.
	@syntax:
		mediaplayer.minimize [-q]
	@description:
		Minimize the player interface.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[cmd]mediaplayer.quit[/cmd],
		[cmd]mediaplayer.hide[/cmd],
		[cmd]mediaplayer.show[/cmd]
*/

MP_KVS_SIMPLE_COMMAND(minimize, minimize)

/*
	@doc: mediaplayer.stop
	@type:
		command
	@title:
		mediaplayer.stop
	@short:
		Stops media playback in the selected media player
	@syntax:
		mediaplayer.stop [-q]
	@description:
		Stops playback in the remote instance of the currently selected media player.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[cmd]mediaplayer.play[/cmd],
		[cmd]mediaplayer.pause[/cmd],
		[cmd]mediaplayer.prev[/cmd],
		[cmd]mediaplayer.next[/cmd],
		[fnc]$mediaplayer.nowPlaying[/fnc],
		[fnc]$mediaplayer.status[/fnc]
*/

MP_KVS_SIMPLE_COMMAND(stop, stop)

/*
	@doc: mediaplayer.next
	@type:
		command
	@title:
		mediaplayer.next
	@short:
		Starts playback of the next song (mediaplayer interface module)
	@syntax:
		mediaplayer.next [-q]
	@description:
		Starts playback of the next song in the remote instance of the slave media player.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module]
		[cmd]mediaplayer.stop[/cmd],
		[cmd]mediaplayer.pause[/cmd],
		[cmd]mediaplayer.prev[/cmd],
		[cmd]mediaplayer.play[/cmd],
		[fnc]$mediaplayer.nowPlaying[/fnc],
		[fnc]$mediaplayer.status[/fnc]
*/

MP_KVS_SIMPLE_COMMAND(next, next)

/*
	@doc: mediaplayer.prev
	@type:
		command
	@title:
		mediaplayer.prev
	@short:
		Starts playback of the previous song (mediaplayer interface module)
	@syntax:
		mediaplayer.prev [-q]
	@description:
		Starts playback of the previous song in the remote instance of the slave media player.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module]
		[cmd]mediaplayer.stop[/cmd],
		[cmd]mediaplayer.pause[/cmd],
		[cmd]mediaplayer.play[/cmd],
		[cmd]mediaplayer.next[/cmd],
		[fnc]$mediaplayer.nowPlaying[/fnc],
		[fnc]$mediaplayer.status[/fnc]
*/

MP_KVS_SIMPLE_COMMAND(prev, prev)

/*
	@doc: mediaplayer.quit
	@type:
		command
	@title:
		mediaplayer.quit
	@short:
		Quits the remote media player (mediaplayer interface module)
	@syntax:
		mediaplayer.quit [-q]
	@description:
		Quits the remote instance of the slave media player.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module]
		[cmd]mediaplayer.stop[/cmd],
		[cmd]mediaplayer.pause[/cmd],
		[cmd]mediaplayer.prev[/cmd],
		[cmd]mediaplayer.next[/cmd],
		[fnc]$mediaplayer.nowPlaying[/fnc],
		[fnc]$mediaplayer.status[/fnc]
*/

MP_KVS_SIMPLE_COMMAND(quit, quit)

/*
	@doc: mediaplayer.pause
	@type:
		command
	@title:
		mediaplayer.pause
	@short:
		Pauses playback (mediaplayer interface module)
	@syntax:
		mediaplayer.pause [-q]
	@description:
		Pauses playback in the remote instance of the slave media player.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module]
		[cmd]mediaplayer.stop[/cmd],
		[cmd]mediaplayer.pause[/cmd],
		[cmd]mediaplayer.prev[/cmd],
		[cmd]mediaplayer.next[/cmd],
		[fnc]$mediaplayer.nowPlaying[/fnc],
		[fnc]$mediaplayer.status[/fnc]
*/

MP_KVS_SIMPLE_COMMAND(pause, pause)

/*
	@doc: mediaplayer.detect
	@type:
		command
	@title:
		mediaplayer.detect
	@short:
		Detects the media player interface to use
	@syntax:
		mediaplayer.detect [-q]
	@description:
		Attempts to detect the best media player interface
		for the current system. The -q switch causes the detection
		algorithm to run quietly. This function may attempt to start
		the media players in order to verify their presence on the system.
		You can guide the function by starting the media player you
		prefer before running it: if a running media player is found, it takes
		precedence over the others.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module]
		[cmd]mediaplayer.stop[/cmd], [cmd]mediaplayer.pause[/cmd],
		[cmd]mediaplayer.prev[/cmd],
		[cmd]mediaplayer.next[/cmd],
		[fnc]$mediaplayer.nowPlaying[/fnc],
		[fnc]$mediaplayer.status[/fnc],
		[cmd]mediaplayer.setPlayer[/cmd],
		[fnc]$mediaplayer.player[/fnc]
*/

MP_KVS_COMMAND(detect)
{
	g_pMPInterface = auto_detect_player(c->hasSwitch('q', "quiet") ? nullptr : c->window());
	return true;
}

/*
	@doc: mediaplayer.setPlayer
	@type:
		command
	@title:
		mediaplayer.setPlayer
	@short:
		Sets the media player interface
	@syntax:
		mediaplayer.setPlayer <player_name>
	@description:
		Sets the media player interface to be used by the
		mediaplayer module. <interface_name> must be one
		of the player names returned by [fnc]$mediaplayer.playerList()[/fnc]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[cmd]mediaplayer.detect[/cmd], [fnc]$mediaplayer.player[/fnc]
*/

MP_KVS_COMMAND(setPlayer)
{
	QString szPlayer;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("player", KVS_PT_STRING, 0, szPlayer)
	KVSM_PARAMETERS_END(c)

	for(MpInterfaceDescriptor * d = g_pDescriptorList->first(); d; d = g_pDescriptorList->next())
	{
		if(d->name() == szPlayer)
		{
			g_pMPInterface = d->instance();
			KVI_OPTION_STRING(KviOption_stringPreferredMediaPlayer) = szPlayer;
			return true;
		}
	}

	return true;
}

/*
	@doc: mediaplayer.player
	@type:
		function
	@title:
		$mediaplayer.player
	@short:
		Returns the currently set media player interface
	@syntax:
		$mediaplayer.player()
	@description:
		Returns the currently set media player interface.
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[cmd]mediaplayer.detect[/cmd], [cmd]mediaplayer.setPlayer[/cmd]
*/

MP_KVS_FUNCTION(player)
{
	c->returnValue()->setString(KVI_OPTION_STRING(KviOption_stringPreferredMediaPlayer));
	return true;
}

/*
	@doc: mediaplayer.playerList
	@type:
		function
	@title:
		$mediaplayer.playerList
	@short:
		Returns a the list of the supported media player interfaces
	@syntax:
		$mediaplayer.playerList()
	@description:
		Returns an array containing the supported media player interfaces.
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module]
		[cmd]mediaplayer.detect[/cmd], [cmd]mediaplayer.setPlayer[/cmd],
		[fnc]$mediaplayer.player[/fnc]
*/

MP_KVS_FUNCTION(playerList)
{
	KviKvsArray * pArray = new KviKvsArray();
	int id = 0;

	for(MpInterfaceDescriptor * d = g_pDescriptorList->first(); d; d = g_pDescriptorList->next())
	{
		pArray->set(id++, new KviKvsVariant(d->name()));
	}
	c->returnValue()->setArray(pArray);
	return true;
}
/*
	@doc: mediaplayer.playmrl
	@type:
		command
	@title:
		mediaplayer.playMrl
	@short:
		Plays the specified media on the selected media player
	@syntax:
		mediaplayer.playMrl <mrl>
	@description:
		Plays the media specified by the <mrl> on the currently
		selected media player interface. MRL is a standard Media Resource Locator.
		Examples of such locators are:
		[example]
			file:///home/myfile.mp3
			http://streaming.server.top:123
			dvd:///dev/dvd
			dvb://channelname
		[/example]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[fnc]$mediaplayer.mrl[/fnc],
		[fnc]$mediaplayer.localFile[/fnc],
		[cmd]mediaplayer.play[/cmd]
*/

MP_KVS_COMMAND(playMrl)
{
	QString szMrl;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("player", KVS_PT_STRING, 0, szMrl)
	KVSM_PARAMETERS_END(c)

	MP_KVS_FAIL_ON_NO_INTERFACE
	if(!g_pMPInterface->playMrl(szMrl))
	{
		if(!c->hasSwitch('q', "quiet"))
		{
			c->warning(__tr2qs_ctx("The selected media player interface failed to execute the requested function", "mediaplayer"));
			QString tmp = __tr2qs_ctx("Last interface error: ", "mediaplayer");
			tmp += g_pMPInterface->lastError();
			c->warning(tmp);
		}
	}

	return true;
}

MP_KVS_COMMAND(amipExec)
{
	QString szMrl;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("player", KVS_PT_STRING, 0, szMrl)
	KVSM_PARAMETERS_END(c)

	MP_KVS_FAIL_ON_NO_INTERFACE
	if(!g_pMPInterface->amipExec(szMrl))
	{
		if(!c->hasSwitch('q', "quiet"))
		{
			c->warning(__tr2qs_ctx("The selected media player interface failed to execute the requested function", "mediaplayer"));
			QString tmp = __tr2qs_ctx("Last interface error: ", "mediaplayer");
			tmp += g_pMPInterface->lastError();
			c->warning(tmp);
		}
	}

	return true;
}

/*
	@doc: mediaplayer.jumpTo
	@type:
		command
	@title:
		mediaplayer.jumpTo
	@short:
		Sets the position in seconds of the current song.
	@syntax:
		mediaplayer.jumpTo [-q] <pos>
	@description:
		Sets the position in seconds of the current song .
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[fnc]$mediaplayer.position[/fnc],
		[fnc]$mediaplayer.length[/fnc],
*/

MP_KVS_COMMAND(jumpTo)
{
	kvs_int_t iPos;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("position", KVS_PT_INT, 0, iPos)
	KVSM_PARAMETERS_END(c)

	MP_KVS_FAIL_ON_NO_INTERFACE
	if(!g_pMPInterface->jumpTo(iPos))
	{
		if(!c->hasSwitch('q', "quiet"))
		{
			c->warning(__tr2qs_ctx("The selected media player interface failed to execute the requested function", "mediaplayer"));
			QString tmp = __tr2qs_ctx("Last interface error: ", "mediaplayer");
			tmp += g_pMPInterface->lastError();
			c->warning(tmp);
		}
	}
	return true;
}

/*
	@doc: mediaplayer.setVol
	@type:
		command
	@title:
		mediaplayer.setVol
	@short:
		Set the media player volume to <vol> (from 0-255)
	@syntax:
		mediaplayer.setVol [-q] <vol>
	@description:
		Set the media player volume to <vol> (the valid range is from 0 to ....)
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[fnc]$mediaplayer.getVol[/fnc]
*/

MP_KVS_COMMAND(setVol)
{
	kvs_int_t iVol;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("volume", KVS_PT_INT, 0, iVol)
	KVSM_PARAMETERS_END(c)

	MP_KVS_FAIL_ON_NO_INTERFACE
	if(!g_pMPInterface->setVol(iVol))
	{
		if(!c->hasSwitch('q', "quiet"))
		{
			c->warning(__tr2qs_ctx("The selected media player interface failed to execute the requested function", "mediaplayer"));
			QString tmp = __tr2qs_ctx("Last interface error: ", "mediaplayer");
			tmp += g_pMPInterface->lastError();
			c->warning(tmp);
		}
	}
	return true;
}

/*
	@doc: mediaplayer.getVol
	@type:
		function
	@title:
		$mediaplayer.getVol
	@short:
		Returns the current volume of the media player
	@syntax:
		$mediaplayer.getVol()
	@description:
		Returns the current volume of the media player within 0 to 255.
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[cmd]mediaplayer.setVol[/cmd]
*/

MP_KVS_SIMPLE_INT_FUNCTION(getVol, getVol)

/*
	@doc: mediaplayer.mute
	@type:
		command
	@title:
		mediaplayer.mute
	@short:
		Mute the volume selected media player
	@syntax:
		mediaplayer.mute
	@description:
		Mutes the volume selected media player.[br]
		If already muted, it restores the volume.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[cmd]mediaplayer.play[/cmd], [cmd]mediaplayer.pause[/cmd],
		[cmd]mediaplayer.prev[/cmd],
		[cmd]mediaplayer.next[/cmd],
		[fnc]$mediaplayer.nowPlaying[/fnc],
		[fnc]$mediaplayer.status[/fnc]
*/

MP_KVS_SIMPLE_COMMAND(mute, mute)

/*
	@doc: mediaplayer.mrl
	@type:
		function
	@title:
		$mediaplayer.mrl
	@short:
		Returns the MRL of the currently playing media
	@syntax:
		$mediaplayer.mrl()
	@description:
		Returns the MRL of the media currently played by the selected media player interface.
		The MRL is a standard Media Resource Locator.
		Examples of such locators are:
		[example]
			file:///home/myfile.mp3
			http://streaming.server.top:123
			dvd:///dev/dvd
			dvb://channelname
		[/example]
		This means that the returned string may [b]not[/b] refer to a local file:
		it refers to the local file only if the [b]file://[/b] prefix is found ([fnc]$mediaplayer.localFile()[/fnc]
		checks that for you).
		The returned string is empty if the player isn't currently playing.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[cmd]mediaplayer.playMrl[/cmd],
		[fnc]$mediaplayer.localFile[/fnc]
*/

MP_KVS_SIMPLE_STRING_FUNCTION(mrl, mrl)

/*
	@doc: mediaplayer.nowPlaying
	@type:
		function
	@title:
		$mediaplayer.nowPlaying
	@short:
		Returns a description of the currently played media
	@syntax:
		$mediaplayer.nowPlaying()
	@description:
		Returns a description for the media currently played
		by the selected media player interface. The description
		will usually contain the title of the media and eventually
		some additional information like the artist name or album.
		This function is granted to be implemented by all the media
		player interfaces and return an empty string only
		if the player is not currently playing or there is a communication
		error.
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[fnc]$mediaplayer.mrl[/fnc], [cmd]mediaplayer.playMrl[/cmd],
		[cmd]mediaplayer.play[/cmd], [fnc]$mediaplayer.title[/fnc],
		[fnc]$mediaplayer.artist[/fnc], [fnc]$mediaplayer.genre[/fnc],
		[fnc]$mediaplayer.year[/fnc], [fnc]$mediaplayer.comment[/fnc],
		[fnc]$mediaplayer.mediaType[/fnc],
		[fnc]$mediaplayer.bitRate[/fnc],
		[fnc]$mediaplayer.sampleRate[/fnc],
		[fnc]$mediaplayer.length[/fnc],
		[fnc]$mediaplayer.position[/fnc]
*/

MP_KVS_SIMPLE_STRING_FUNCTION(nowPlaying, nowPlaying)

/*
	@doc: mediaplayer.title
	@type:
		function
	@title:
		$mediaplayer.title
	@short:
		Returns the title of the current media
	@syntax:
		$mediaplayer.title()
	@description:
		Returns the title of the multimedia file being played by the
		remote instance of the media player. This function is not granted
		to work with all the player interfaces and all the media types:
		if you want to write portable code you should use [fnc]$mediaplayer.nowPlaying[/fnc]
		instead.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[fnc]$mediaplayer.mrl[/fnc], [cmd]mediaplayer.playMrl[/cmd],
		[cmd]mediaplayer.play[/cmd], [fnc]$mediaplayer.nowPlaying[/fnc],
		[fnc]$mediaplayer.artist[/fnc], [fnc]$mediaplayer.genre[/fnc],
		[fnc]$mediaplayer.year[/fnc], [fnc]$mediaplayer.comment[/fnc],
		[fnc]$mediaplayer.mediaType[/fnc]
*/

MP_KVS_SIMPLE_STRING_FUNCTION(title, title)

/*
	@doc: mediaplayer.artist
	@type:
		function
	@title:
		$mediaplayer.artist
	@short:
		Returns the artist of the media player
	@syntax:
		$mediaplayer.artist()
	@description:
		Returns the artist property of the media currently played by
		the selected media player interface. This function is not granted
		to work with all the player interfaces and all the media types:
		if you want to write portable code you should use [fnc]$mediaplayer.nowPlaying[/fnc]
		instead.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[fnc]$mediaplayer.mrl[/fnc], [cmd]mediaplayer.playMrl[/cmd],
		[cmd]mediaplayer.play[/cmd], [fnc]$mediaplayer.nowPlaying[/fnc],
		[fnc]$mediaplayer.title[/fnc], [fnc]$mediaplayer.genre[/fnc],
		[fnc]$mediaplayer.year[/fnc], [fnc]$mediaplayer.comment[/fnc],
		[fnc]$mediaplayer.mediaType[/fnc],
		[fnc]$mediaplayer.album[/fnc],
		[fnc]$mediaplayer.length[/fnc],
		[fnc]$mediaplayer.position[/fnc]
*/

MP_KVS_SIMPLE_STRING_FUNCTION(artist, artist)

/*
	@doc: mediaplayer.genre
	@type:
		function
	@title:
		$mediaplayer.genre
	@short:
		Returns the genre of the media player
	@syntax:
		$mediaplayer.genre()
	@description:
		Returns the genre property of the media currently played by
		the selected media player interface. This function is not granted
		to work with all the player interfaces and all the media types:
		if you want to write portable code you should use [fnc]$mediaplayer.nowPlaying[/fnc]
		instead.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[fnc]$mediaplayer.mrl[/fnc], [cmd]mediaplayer.playMrl[/cmd],
		[cmd]mediaplayer.play[/cmd], [fnc]$mediaplayer.nowPlaying[/fnc],
		[fnc]$mediaplayer.artist[/fnc], [fnc]$mediaplayer.artist[/fnc],
		[fnc]$mediaplayer.year[/fnc], [fnc]$mediaplayer.comment[/fnc],
		[fnc]$mediaplayer.mediaType[/fnc], [fnc]$mediaplayer.album[/fnc],
		[fnc]$mediaplayer.length[/fnc], [fnc]$mediaplayer.position[/fnc]
*/

MP_KVS_SIMPLE_STRING_FUNCTION(genre, genre)

/*
	@doc: mediaplayer.year
	@type:
		function
	@title:
		$mediaplayer.year
	@short:
		Returns the year of the media player
	@syntax:
		$mediaplayer.year()
	@description:
		Returns the year property of the media currently played by
		the selected media player interface. This function is not granted
		to work with all the player interfaces and all the media types:
		if you want to write portable code you should use [fnc]$mediaplayer.nowPlaying[/fnc]
		instead.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[fnc]$mediaplayer.mrl[/fnc], [cmd]mediaplayer.playMrl[/cmd],
		[cmd]mediaplayer.play[/cmd], [fnc]$mediaplayer.nowPlaying[/fnc],
		[fnc]$mediaplayer.artist[/fnc], [fnc]$mediaplayer.artist[/fnc],
		[fnc]$mediaplayer.genre[/fnc], [fnc]$mediaplayer.comment[/fnc],
		[fnc]$mediaplayer.mediaType[/fnc], [fnc]$mediaplayer.album[/fnc],
		[fnc]$mediaplayer.length[/fnc], [fnc]$mediaplayer.position[/fnc]
*/

MP_KVS_SIMPLE_STRING_FUNCTION(year, year)

/*
	@doc: mediaplayer.comment
	@type:
		function
	@title:
		$mediaplayer.comment
	@short:
		Returns the comment of the media player
	@syntax:
		$mediaplayer.comment()
	@description:
		Returns the comment property of the media currently played by
		the selected media player interface. This function is not granted
		to work with all the player interfaces and all the media types:
		if you want to write portable code you should use [fnc]$mediaplayer.nowPlaying[/fnc]
		instead.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[fnc]$mediaplayer.mrl[/fnc], [cmd]mediaplayer.playMrl[/cmd],
		[cmd]mediaplayer.play[/cmd], [fnc]$mediaplayer.nowPlaying[/fnc],
		[fnc]$mediaplayer.artist[/fnc], [fnc]$mediaplayer.artist[/fnc],
		[fnc]$mediaplayer.year[/fnc], [fnc]$mediaplayer.year[/fnc],
		[fnc]$mediaplayer.mediaType[/fnc], [fnc]$mediaplayer.album[/fnc],
		[fnc]$mediaplayer.length[/fnc], [fnc]$mediaplayer.position[/fnc]
*/

MP_KVS_SIMPLE_STRING_FUNCTION(comment, comment)

/*
	@doc: mediaplayer.album
	@type:
		function
	@title:
		$mediaplayer.album
	@short:
		Returns the album of the media player
	@syntax:
		$mediaplayer.album()
	@description:
		Returns the album property of the media currently played by
		the selected media player interface. This function is not granted
		to work with all the player interfaces and all the media types:
		if you want to write portable code you should use [fnc]$mediaplayer.nowPlaying[/fnc]
		instead.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[fnc]$mediaplayer.mrl[/fnc], [cmd]mediaplayer.playMrl[/cmd],
		[cmd]mediaplayer.play[/cmd], [fnc]$mediaplayer.nowPlaying[/fnc],
		[fnc]$mediaplayer.artist[/fnc], [fnc]$mediaplayer.artist[/fnc],
		[fnc]$mediaplayer.year[/fnc], [fnc]$mediaplayer.comment[/fnc],
		[fnc]$mediaplayer.mediaType[/fnc],
		[fnc]$mediaplayer.bitRate[/fnc],
		[fnc]$mediaplayer.sampleRate[/fnc],
		[fnc]$mediaplayer.length[/fnc],
		[fnc]$mediaplayer.position[/fnc]
*/

MP_KVS_SIMPLE_STRING_FUNCTION(album, album)

/*
	@doc: mediaplayer.mediaType
	@type:
		function
	@title:
		$mediaplayer.mediaType
	@short:
		Returns the currently played media type
	@syntax:
		$mediaplayer.mediaType()
	@description:
		Returns a string describing the media type currently played
		by the selected media player interface. This function is not granted
		to work with all the player interfaces and all the media types.
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module]
		[fnc]$mediaplayer.mrl[/fnc], [cmd]mediaplayer.playMrl[/cmd],
		[cmd]mediaplayer.play[/cmd], [fnc]$mediaplayer.nowPlaying[/fnc],
		[fnc]$mediaplayer.artist[/fnc],
		[fnc]$mediaplayer.artist[/fnc],
		[fnc]$mediaplayer.year[/fnc], [fnc]$mediaplayer.comment[/fnc]
*/

MP_KVS_SIMPLE_STRING_FUNCTION(mediaType, mediaType)

/*
	@doc: mediaplayer.bitRate
	@type:
		function
	@title:
		$mediaplayer.bitRate
	@short:
		Returns the bit rate of the current song
	@syntax:
		$mediaplayer.bitRate
	@description:
		Returns the bit rate (in bits/sec) of the multimedia file being played by the
		remote instance of the media player.This function is not granted
		to work with all the player interfaces and all the media types so
		you may get -1 or 0 in return sometimes.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module]
		[fnc]$mediaplayer.mrl[/fnc], [cmd]mediaplayer.playMrl[/cmd],
		[cmd]mediaplayer.play[/cmd], [fnc]$mediaplayer.nowPlaying[/fnc],
		[fnc]$mediaplayer.artist[/fnc], [fnc]$mediaplayer.artist[/fnc],
		[fnc]$mediaplayer.year[/fnc], [fnc]$mediaplayer.comment[/fnc],
		[fnc]$mediaplayer.sampleRate[/fnc],
		[fnc]$mediaplayer.length[/fnc],
		[fnc]$mediaplayer.position[/fnc]
*/

MP_KVS_SIMPLE_INT_FUNCTION(bitRate, bitRate)

/*
	@doc: mediaplayer.sampleRate
	@type:
		function
	@title:
		$mediaplayer.sampleRate
	@short:
		Returns the sample rate of the current song
	@syntax:
		$mediaplayer.sampleRate
	@description:
		Returns the sample rate (in Hz) of the multimedia file being played by the
		remote instance of the media player.This function is not granted
		to work with all the player interfaces and all the media types so
		you may get [b]-1[/b] or [b]0[/b] in return sometimes.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module]
		[fnc]$mediaplayer.mrl[/fnc], [cmd]mediaplayer.playMrl[/cmd],
		[cmd]mediaplayer.play[/cmd], [fnc]$mediaplayer.nowPlaying[/fnc],
		[fnc]$mediaplayer.artist[/fnc], [fnc]$mediaplayer.artist[/fnc],
		[fnc]$mediaplayer.year[/fnc], [fnc]$mediaplayer.comment[/fnc],
		[fnc]$mediaplayer.bitRate[/fnc], [fnc]$mediaplayer.length[/fnc],
		[fnc]$mediaplayer.position[/fnc], [fnc]$mediaplayer.channels[/fnc]
*/

MP_KVS_SIMPLE_INT_FUNCTION(sampleRate, sampleRate)

/*
	@doc: mediaplayer.length
	@type:
		function
	@title:
		$mediaplayer.length
	@short:
		Returns the length of the current media
	@syntax:
		$mediaplayer.length()
	@description:
		Returns the length in milliseconds of the multimedia file being played by the
		remote instance of the media player.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[fnc]$mediaplayer.mrl[/fnc], [cmd]mediaplayer.playMrl[/cmd],
		[cmd]mediaplayer.play[/cmd], [fnc]$mediaplayer.nowPlaying[/fnc],
		[fnc]$mediaplayer.artist[/fnc],
		[fnc]$mediaplayer.artist[/fnc],
		[fnc]$mediaplayer.year[/fnc], [fnc]$mediaplayer.comment[/fnc],
		[fnc]$mediaplayer.sampleRate[/fnc],
		[fnc]$mediaplayer.bitRate[/fnc],
		[fnc]$mediaplayer.position[/fnc]
*/

MP_KVS_SIMPLE_INT_FUNCTION(length, length)

/*
	@doc: mediaplayer.position
	@type:
		function
	@title:
		$mediaplayer.position
	@short:
		Returns the position of the current media
	@syntax:
		$mediaplayer.position()
	@description:
		Returns the currently elapsed time of the multimedia file being played by the
		remote instance of the media player.
		The time is expressed in milliseconds.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module]
		[fnc]$mediaplayer.mrl[/fnc], [cmd]mediaplayer.playMrl[/cmd],
		[cmd]mediaplayer.play[/cmd], [fnc]$mediaplayer.nowPlaying[/fnc],
		[fnc]$mediaplayer.artist[/fnc], [fnc]$mediaplayer.artist[/fnc],
		[fnc]$mediaplayer.year[/fnc], [fnc]$mediaplayer.comment[/fnc],
		[fnc]$mediaplayer.sampleRate[/fnc],
		[fnc]$mediaplayer.length[/fnc],
		[fnc]$mediaplayer.bitRate[/fnc], [fnc]$mediaplayer.channels[/fnc]
*/

MP_KVS_SIMPLE_INT_FUNCTION(position, position)

/*
	@doc: mediaplayer.channels
	@type:
		function
	@title:
		$mediaplayer.channels
	@short:
		Returns the number of channels of the current song
	@syntax:
		$mediaplayer.channels()
	@description:
		Returns the number of channels of the multimedia file being played by the
		remote instance of the media player.This function is not granted
		to work with all the player interfaces and all the media types so
		you may get -1 or 0 in return sometimes.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module]
*/

MP_KVS_SIMPLE_INT_FUNCTION(channels, channels)

/*
	@doc: mediaplayer.setPlayListPos
	@type:
		command
	@title:
		mediaplayer.setPlayListPos
	@short:
		Set the position for the current song.
	@syntax:
		mediaplayer.setPlayListPos [-q] <pos:integer>
	@description:
		Set the position in the player list for the current song to <pos>.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[fnc]$mediaplayer.getPlayListPos[/fnc]
*/

MP_KVS_COMMAND(setPlayListPos)
{
	kvs_int_t iPos;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("position", KVS_PT_INT, 0, iPos)
	KVSM_PARAMETERS_END(c)

	MP_KVS_FAIL_ON_NO_INTERFACE
	if(!g_pMPInterface->setPlayListPos(iPos))
	{
		if(!c->hasSwitch('q', "quiet"))
		{
			c->warning(__tr2qs_ctx("The selected media player interface failed to execute the requested function", "mediaplayer"));
			QString tmp = __tr2qs_ctx("Last interface error: ", "mediaplayer");
			tmp += g_pMPInterface->lastError();
			c->warning(tmp);
		}
	}
	return true;
}

/*
	@doc: mediaplayer.getPlayListPos
	@type:
		function
	@title:
		$mediaplayer.getPlayListPos
	@short:
		Get the position for the current song.
	@syntax:
		$mediaplayer.getPlayListPos()
	@description:
		Get the song's position in the player list.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[cmd]mediaplayer.getPlayListPos[/cmd]
*/

MP_KVS_SIMPLE_INT_FUNCTION(getPlayListPos, getPlayListPos)

/*
	@doc: mediaplayer.getListLength
	@type:
		function
	@title:
		$mediaplayer.getListLength
	@short:
		Return the play list length.
	@syntax:
		$mediaplayer.getListLength()
	@description:
		Return the player list length.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
*/

MP_KVS_SIMPLE_INT_FUNCTION(getListLength, getListLength)

/*
	@doc: mediaplayer.getRepeat
	@type:
		function
	@title:
		$mediaplayer.getRepeat
	@short:
		Return if [i]Repeat[/i] is on.
	@syntax:
		$mediaplayer.getRepeat()
	@description:
		Return the value of the Repeat flag for the current track ([b]1[/b] for [b]ON[/b], [b]0[/b] for [b]OFF[/b]).[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[cmd]mediaplayer.setRepeat[/cmd], [cmd]mediaplayer.setShuffle[/cmd],
		[fnc]$mediaplayer.getShuffle[/fnc]
*/

MP_KVS_SIMPLE_INT_FUNCTION(getRepeat, getRepeat)

/*
	@doc: mediaplayer.getShuffle
	@type:
		function
	@title:
		$mediaplayer.getShuffle
	@short:
		Return if [i]shuffle[/i] is on.
	@syntax:
		$mediaplayer.getShuffle()
	@description:
		Return the value of the Shuffle flag ([b]1[/b] for [b]ON[/b], [b]0[/b] for [b]OFF[/b]).[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[cmd]mediaplayer.setShuffle[/cmd], [cmd]mediaplayer.setRepeat[/cmd],
		[fnc]$mediaplayer.getRepeat[/fnc]
*/

MP_KVS_SIMPLE_BOOL_FUNCTION(getShuffle, getShuffle)

/*
	@doc: mediaplayer.getEqData
	@type:
		function
	@title:
		$mediaplayer.getEqData
	@short:
		Return the equalizer value.
	@syntax:
		$mediaplayer.getEqData(<item:integer>,<options:string>)
	@description:
		Return the value of the EQ <item>.[br]
		if [b]q[/b] is given as an option, it runs in quiet mode.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[cmd]mediaplayer.setShuffle[/cmd], [cmd]mediaplayer.setRepeat[/cmd],
		[fnc]$mediaplayer.getRepeat[/fnc]

*/

MP_KVS_FUNCTION(getEqData)
{
	kvs_int_t iValue;
	QString szOptions;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("item", KVS_PT_INT, 0, iValue)
	KVSM_PARAMETER("options", KVS_PT_STRING, KVS_PF_OPTIONAL, szOptions)
	KVSM_PARAMETERS_END(c)

	MP_KVS_FAIL_ON_NO_INTERFACE

	bool bQuiet = szOptions.indexOf('q', Qt::CaseInsensitive) != -1;

	int ret = g_pMPInterface->getEqData(iValue);

	if(!ret && !bQuiet)
	{
		c->warning(__tr2qs_ctx("The selected media player interface failed to execute the requested function", "mediaplayer"));
		QString tmp = __tr2qs_ctx("Last interface error: ", "mediaplayer");
		tmp += g_pMPInterface->lastError();
		c->warning(tmp);
	}

	c->returnValue()->setInteger(ret);
	return true;
}

/*
	@doc: mediaplayer.setEqData
	@type:
		command
	@title:
		mediaplayer.setEqData
	@short:
		Return the equalizer value.
	@syntax:
		mediaplayer.setEqData [-q] <item:integer> <value:integer>
	@description:
		Set the value of the EQ <item> to <value>.[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[fnc]$mediaplayer.getEqData[/fnc]
*/

MP_KVS_COMMAND(setEqData)
{
	kvs_int_t iPos;
	kvs_int_t iValue;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("item", KVS_PT_INT, 0, iPos)
	KVSM_PARAMETER("value", KVS_PT_INT, 0, iValue)
	KVSM_PARAMETERS_END(c)

	MP_KVS_FAIL_ON_NO_INTERFACE
	if(!g_pMPInterface->setEqData(iPos, iValue))
	{
		if(!c->hasSwitch('q', "quiet"))
		{
			c->warning(__tr2qs_ctx("The selected media player interface failed to execute the requested function", "mediaplayer"));
			QString tmp = __tr2qs_ctx("Last interface error: ", "mediaplayer");
			tmp += g_pMPInterface->lastError();
			c->warning(tmp);
		}
	}
	return true;
}

/*
	@doc: mediaplayer.localFile
	@type:
		function
	@title:
		$mediaplayer.localFile
	@short:
		Returns the name of the local file played by the media player
	@syntax:
		$mediaplayer.localFile()
	@description:
		Returns the name of the local file played by the currently
		selected media player interface. This function returns
		an empty string if the player is not playing, there is a communication
		error or the media played is not a local file (e.g. a stream or another
		kind of media).[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[fnc]$mediaplayer.mrl[/fnc], [cmd]mediaplayer.play[/cmd]
*/

MP_KVS_FUNCTION(localFile)
{

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETERS_END(c)

	MP_KVS_FAIL_ON_NO_INTERFACE
	QString szRet = g_pMPInterface->mrl();
	if(szRet.isEmpty())
		return true;
	if(szRet.startsWith("file://", Qt::CaseInsensitive))
	{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		c->returnValue()->setString(szRet.mid(7));
#else
		QUrl url(szRet);
		c->returnValue()->setString(url.toLocalFile());
#endif
	}
	return true;
}

MP_KVS_FUNCTION(amipEval)
{
	QString szMrl;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("player", KVS_PT_STRING, 0, szMrl)
	KVSM_PARAMETERS_END(c)

	MP_KVS_FAIL_ON_NO_INTERFACE
	QString szRet = g_pMPInterface->amipEval(szMrl);
	if(szRet.isEmpty())
		return true;

	c->returnValue()->setString(szRet);

	return true;
}

/*
	@doc: mediaplayer.status
	@type:
		function
	@title:
		$mediaplayer.status
	@short:
		Returns the status of the media player
	@syntax:
		$mediaplayer.status()
	@description:
		Returns the status if the currently selected media player: [b][i]playing[/b][/i],
		[b][i]stopped[/b][/i], [b][i]paused[/b][/i] or [b][i]unknown[/b][/i].
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[cmd]mediaplayer.play[/cmd]
*/

MP_KVS_FUNCTION(status)
{

	// 	KVSM_PARAMETERS_BEGIN(c)
	// 	KVSM_PARAMETERS_END(c)

	MP_KVS_FAIL_ON_NO_INTERFACE
	MpInterface::PlayerStatus eStat = g_pMPInterface->status();
	switch(eStat)
	{
		case MpInterface::Stopped:
			c->returnValue()->setString("stopped");
			break;
		case MpInterface::Playing:
			c->returnValue()->setString("playing");
			break;
		case MpInterface::Paused:
			c->returnValue()->setString("paused");
			break;
		default:
			c->returnValue()->setString("unknown");
			break;
	}
	return true;
}

/*
	@doc: mediaplayer
	@type:
		module
	@short:
		Interface to various media players
	@title:
		The mediaplayer interface module
	@body:
		The mediaplayer module is an interface to various popular media players.
		It allows controlling them remotely to a certain degree and retrieving
		the information about the currently played media (in order to implement the nice /playing
		scripts). The module has replaced the old mediaplayer module which after
		years of development had developed some design flaws and needed a complete rewrite.
		At the moment of writing the supported players are: XMMS (UNIX), Audacious (UNIX), Winamp (Windows),
		every player supported by the AMIP project (Windows), every player using the MPRIS D-Bus interface
		(Amarok2, JuK under KDE). You choose the player interface by either setting
		the option in the settings dialog, by manually setting [cmd]option[/cmd] stringPreferredMediaPlayer
		or by running [cmd]mediaplayer.detect[/cmd] that will guess the media player interface for you.[br]
		If you're going to use the Winamp media player then you must install the [i]gen_kvirc.dll[/i] plugin
		(included in the KVIrc distribution) as a [i]general[/i] Winamp plugin.
		This is achieved by simply copying the file gen_kvirc.dll
		to the Winamp plugins directory and restarting Winamp.
		[br][br]
		The commands and functions exported by this module serve mainly for two
		purposes: control the playback and get information about the currently played media.
		Playback control is achieved by the means of [cmd]mediaplayer.play[/cmd],
		[cmd]mediaplayer.stop[/cmd], [cmd]mediaplayer.pause[/cmd], [cmd]mediaplayer.next[/cmd],
		[cmd]mediaplayer.prev[/cmd] and [fnc]$mediaplayer.status()[/fnc]. The
		information about the currently played media can be retrieved with
		[fnc]$mediaplayer.nowplaying()[/fnc] and [fnc]$mediaplayer.mrl()[/fnc].[br][br]
		Several other informative function are available as well but they are not
		granted to work with all the supported media players or media types (e.g.
		with Winamp you can play video files that probably have no album, genre
		or sampleRate information).[br][br]
		If you intend to write portable code, you should check the return value
		of the functions like [fnc]$mediaplayer.artist()[/fnc], [fnc]$mediaplayer.title()[/fnc]
		or [fnc]$mediaplayer.channels()[/fnc] and be ready to discard it when it
		does not appear to be meaningful. The functions that are granted
		to work are [fnc]$mediaplayer.nowplaying()[/fnc], [fnc]$mediaplayer.mrl()[/fnc]
		and [fnc]$mediaplayer.localFile()[/fnc].
*/

/*
	@doc: mediaplayer.setRepeat
	@type:
		command
	@title:
		mediaplayer.setRepeat
	@short:
		Set the repeat flag.
	@syntax:
		mediaplayer.setRepeat [-q] <repeat:bool>
	@description:
		Set the repeat flag to [i]repeat[/i] ([b]1[/b] for [b]ON[/b], [b]0[/b] for [b]OFF[/b]).[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[fnc]$mediaplayer.getRepeat[/fnc], [cmd]mediaplayer.setShuffle[/cmd],
		[fnc]$mediaplayer.getShuffle[/fnc]
*/

MP_KVS_COMMAND(setRepeat)
{
	bool bVal;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("repeat", KVS_PT_BOOL, 0, bVal)
	KVSM_PARAMETERS_END(c)

	MP_KVS_FAIL_ON_NO_INTERFACE
	if(!g_pMPInterface->setRepeat(bVal))
	{
		if(!c->hasSwitch('q', "quiet"))
		{
			c->warning(__tr2qs_ctx("The selected media player interface failed to execute the requested function", "mediaplayer"));
			QString tmp = __tr2qs_ctx("Last interface error: ", "mediaplayer");
			tmp += g_pMPInterface->lastError();
			c->warning(tmp);
		}
	}
	return true;
}

/*
	@doc: mediaplayer.setShuffle
	@type:
		command
	@title:
		mediaplayer.setShuffle
	@short:
		Set the repeat flag.
	@syntax:
		mediaplayer.setShuffle [-q] <shuffle:bool>
	@description:
		Set the Shuffle flag to [i]shuffle[/i] ([b]1[/b] for [b]ON[/b], [b]0[/b] for [b]OFF[/b]).[br]
		Take a look at the [module:mediaplayer]mediaplayer module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mediaplayer]media player module documentation[/module],
		[fnc]$mediaplayer.getShuffle[/fnc], [cmd]mediaplayer.setRepeat[/cmd],
		[fnc]$mediaplayer.getRepeat[/fnc]
*/

MP_KVS_COMMAND(setShuffle)
{
	bool bVal;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("shuffle", KVS_PT_BOOL, 0, bVal)
	KVSM_PARAMETERS_END(c)

	MP_KVS_FAIL_ON_NO_INTERFACE
	if(!g_pMPInterface->setShuffle(bVal))
	{
		if(!c->hasSwitch('q', "quiet"))
		{
			c->warning(__tr2qs_ctx("The selected media player interface failed to execute the requested function", "mediaplayer"));
			QString tmp = __tr2qs_ctx("Last interface error: ", "mediaplayer");
			tmp += g_pMPInterface->lastError();
			c->warning(tmp);
		}
	}
	return true;
}

static bool mediaplayer_module_init(KviModule * m)
{
	g_pDescriptorList = new KviPointerList<MpInterfaceDescriptor>;
	g_pDescriptorList->setAutoDelete(true);

#if(defined(COMPILE_DBUS_SUPPORT) && !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MAC) && !defined(COMPILE_ON_MINGW))
	g_pDescriptorList->append(MP_CREATE_DESCRIPTOR(MpAudaciousInterface));
	g_pDescriptorList->append(MP_CREATE_DESCRIPTOR(KviAudaciousClassicInterface));
	g_pDescriptorList->append(MP_CREATE_DESCRIPTOR(KviXmmsInterface));
	g_pDescriptorList->append(MP_CREATE_DESCRIPTOR(MpXmms2Interface));
	g_pDescriptorList->append(MP_CREATE_DESCRIPTOR(MpBmpxInterface));
	g_pDescriptorList->append(MP_CREATE_DESCRIPTOR(MpAmarok2Interface));
	g_pDescriptorList->append(MP_CREATE_DESCRIPTOR(MpQmmpInterface));
	g_pDescriptorList->append(MP_CREATE_DESCRIPTOR(MpSongbirdInterface));
	g_pDescriptorList->append(MP_CREATE_DESCRIPTOR(MpTotemInterface));
	g_pDescriptorList->append(MP_CREATE_DESCRIPTOR(MpVlcInterface));
	g_pDescriptorList->append(MP_CREATE_DESCRIPTOR(MpClementineInterface));
#endif

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	g_pDescriptorList->append(MP_CREATE_DESCRIPTOR(MpAmipInterface));
	g_pDescriptorList->append(MP_CREATE_DESCRIPTOR(KviWinampInterface));
#endif
	g_pMPInterface = nullptr;

	if(KVI_OPTION_STRING(KviOption_stringPreferredMediaPlayer) == "auto")
	{
		g_pMPInterface = auto_detect_player();
	}
	else
	{
		for(MpInterfaceDescriptor * d = g_pDescriptorList->first(); d; d = g_pDescriptorList->next())
		{
			if(d->name() == KVI_OPTION_STRING(KviOption_stringPreferredMediaPlayer))
			{
				g_pMPInterface = d->instance();
			}
		}
	}

// check for "auto" interface too!

#define MP_KVS_REGCMD(__name, __stringname) KVSM_REGISTER_SIMPLE_COMMAND(m, __stringname, mediaplayer_kvs_cmd_##__name)
#define MP_KVS_REGFNC(__name, __stringname) KVSM_REGISTER_FUNCTION(m, __stringname, mediaplayer_kvs_fnc_##__name)

	MP_KVS_REGCMD(play, "play");
	MP_KVS_REGCMD(stop, "stop");
	MP_KVS_REGCMD(next, "next");
	MP_KVS_REGCMD(prev, "prev");
	MP_KVS_REGCMD(quit, "quit");
	MP_KVS_REGCMD(pause, "pause");
	MP_KVS_REGCMD(detect, "detect");
	MP_KVS_REGCMD(playMrl, "playMrl");
	MP_KVS_REGCMD(amipExec, "amipExec");
	MP_KVS_REGCMD(hide, "hide");
	MP_KVS_REGCMD(show, "show");
	MP_KVS_REGCMD(minimize, "minimize");
	MP_KVS_REGCMD(jumpTo, "jumpTo");
	MP_KVS_REGCMD(setPlayer, "setPlayer");
	MP_KVS_REGCMD(setVol, "setVol");
	MP_KVS_REGCMD(mute, "mute");
	MP_KVS_REGCMD(setRepeat, "setRepeat");
	MP_KVS_REGCMD(setShuffle, "setShuffle");
	MP_KVS_REGCMD(setPlayListPos, "setPlayListPos");
	MP_KVS_REGCMD(setEqData, "setEqData");

	MP_KVS_REGFNC(nowPlaying, "nowPlaying");
	MP_KVS_REGFNC(mrl, "mrl");
	MP_KVS_REGFNC(title, "title");
	MP_KVS_REGFNC(artist, "artist");
	MP_KVS_REGFNC(genre, "genre");
	MP_KVS_REGFNC(year, "year");
	MP_KVS_REGFNC(comment, "comment");
	MP_KVS_REGFNC(album, "album");
	MP_KVS_REGFNC(mediaType, "mediaType");
	MP_KVS_REGFNC(bitRate, "bitRate");
	MP_KVS_REGFNC(sampleRate, "sampleRate");
	MP_KVS_REGFNC(length, "length");
	MP_KVS_REGFNC(position, "position");
	MP_KVS_REGFNC(status, "status");
	MP_KVS_REGFNC(player, "player");
	MP_KVS_REGFNC(playerList, "playerList");
	MP_KVS_REGFNC(localFile, "localFile");
	MP_KVS_REGFNC(amipEval, "amipEval");
	MP_KVS_REGFNC(channels, "channels");
	MP_KVS_REGFNC(getListLength, "getListLength");
	MP_KVS_REGFNC(getPlayListPos, "getPlayListPos");
	MP_KVS_REGFNC(getEqData, "getEqData");
	MP_KVS_REGFNC(getRepeat, "getRepeat");
	MP_KVS_REGFNC(getShuffle, "getShuffle");
	MP_KVS_REGFNC(getVol, "getVol");

	return true;
}

static bool mediaplayer_module_cleanup(KviModule *)
{
	delete g_pDescriptorList;
	return true;
}

static bool mediaplayer_module_can_unload(KviModule *)
{
	return true;
}

static bool mediaplayer_module_ctrl(KviModule *, const char * operation, void * param)
{
	if(kvi_strEqualCI(operation, "getAvailableMediaPlayers"))
	{
		// we expect param to be a pointer to QStringList
		QStringList * l = (QStringList *)param;
		for(MpInterfaceDescriptor * d = g_pDescriptorList->first(); d; d = g_pDescriptorList->next())
		{
			l->append(d->name());
		}
		return true;
	}
	if(kvi_strEqualCI(operation, "detectMediaPlayer"))
	{
		auto_detect_player(nullptr);
		return true;
	}
	return false;
}

KVIRC_MODULE(
    "mediaplayer",
    "4.0.0",
    "Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net),"
    "Christoph Thielecke (crissi99 at gmx dot de),"
    "Tonino Imbesi (grifisx at barmes dot org),"
    "Alessandro Carbone (elfonol at gmail dot com),"
    "Alexey Uzhva (wizard at opendoor dot ru),"
    "Serge Baranov (sbaranov at gmail dot com)",
    "Interface to various media players",
    mediaplayer_module_init,
    mediaplayer_module_can_unload,
    mediaplayer_module_ctrl,
    mediaplayer_module_cleanup,
    "mediaplayer")
