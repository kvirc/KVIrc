/*
//
//   File : libkvimp3player.cpp
//   Creation date : Sat Nov  3 19:28:25 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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
*/

#include "kvi_module.h"
#include "kvi_library.h"
#include "kvi_locale.h"


#include "kvi_settings.h"


#include <qstring.h>

#if 0
/*
	@doc: mp3player
	@type:
		module
	@short:
		Interface to winamp/xmms
	@title:
		Interface to winamp/xmms (DEPRECATED)
	@body:
		This module has been replaced by the [module:mediaplayer]mediaplayer[/module] module
		and exists only for compatibility reasons. Don't use it: it may disappear
		in one of the following versions.
*/


/*
	@doc: mp3player.play
	@type:
		command
	@title:
		mp3player.play
	@short:
		Deprecated: present for compatibility only
	@syntax:
		mp3player.play
	@description:
		Starts playback in the remote instance of the slave mp3 player.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/


/*static bool mp3player_cmd_play( KviModule *m, KviCommand *c )
{
	ENTER_STACK_FRAME(c,"mp3player_cmd_play");
	KviStr dummy;
	if(!g_pUserParser->parseCmdFinalPart(c,dummy))
		return false;
	g_pUserParser->parseCommandBuffer("mediaplayer.play",c->window());
	return c->leaveStackFrame();
}*/

/*
	@doc: mp3player.stop
	@type:
		command
	@title:
		mp3player.stop
	@short:
		Deprecated: present for compatibility only
	@syntax:
		mp3player.stop
	@description:
		Stops playback in the remote instance of the slave mp3 player.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

/*static bool mp3player_cmd_stop( KviModule *m, KviCommand *c )
{
	ENTER_STACK_FRAME( c, "mp3player_cmd_stop" );
	KviStr dummy;
	if ( !g_pUserParser->parseCmdFinalPart( c, dummy ) )
		return false;
	g_pUserParser->parseCommandBuffer("mediaplayer.stop",c->window());
	return c->leaveStackFrame();
}*/

/*
	@doc: mp3player.next
	@type:
		command
	@title:
		mp3player.next
	@short:
		Deprecated: present for compatibility only
	@syntax:
		mp3player.next
	@description:
		Starts playback of the next song in the remote instance of the slave mp3 player.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

/*static bool mp3player_cmd_next( KviModule *m, KviCommand *c )
{
	ENTER_STACK_FRAME( c, "mp3player_cmd_next" );
	KviStr dummy;
	if ( !g_pUserParser->parseCmdFinalPart( c, dummy ) )
		return false;
	g_pUserParser->parseCommandBuffer("mediaplayer.next",c->window());
	return c->leaveStackFrame();*/
}

/*
	@doc: mp3player.prev
	@type:
		command
	@title:
		mp3player.prev
	@short:
		Deprecated: present for compatibility only
	@syntax:
		mp3player.prev
	@description:
		Starts playback of the previous song in the remote instance of the slave mp3 player.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

/*static bool mp3player_cmd_prev( KviModule * m, KviCommand * c )
{
	ENTER_STACK_FRAME( c, "mp3player_cmd_prev" );
	KviStr dummy;
	if ( !g_pUserParser->parseCmdFinalPart( c, dummy ) )
		return false;
	g_pUserParser->parseCommandBuffer("mediaplayer.prev",c->window());
	return c->leaveStackFrame();*/
}

/*
	@doc: mp3player.pause
	@type:
		command
	@title:
		mp3player.pause
	@short:
		Deprecated: present for compatibility only
	@syntax:
		mp3player.pause
	@description:
		Pauses playback in the remote instance of the slave mp3 player.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

static bool mp3player_cmd_pause( KviModule * m, KviCommand * c )
{
	ENTER_STACK_FRAME( c, "mp3player_cmd_pause" );
	KviStr dummy;
	if ( !g_pUserParser->parseCmdFinalPart( c, dummy ) )
		return false;
	g_pUserParser->parseCommandBuffer("mediaplayer.pause",c->window());
	return c->leaveStackFrame();
}

/*
	@doc: mp3player.quit
	@type:
		command
	@title:
		mp3player.quit
	@short:
		Deprecated: present for compatibility only
	@syntax:
		mp3player.quit
	@description:
		Quits the remote instance of the slave mp3 player.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

static bool mp3player_cmd_quit( KviModule * m, KviCommand * c )
{
	ENTER_STACK_FRAME( c, "mp3player_cmd_quit" );
	KviStr dummy;
	if ( !g_pUserParser->parseCmdFinalPart( c, dummy ) )
		return false;
	g_pUserParser->parseCommandBuffer("mediaplayer.quit",c->window());
	return c->leaveStackFrame();
}

/*
	@doc: mp3player.add
	@type:
		command
	@title:
		mp3player.add
	@short:
		Deprecated: present for compatibility only
	@syntax:
		mp3player.add(<filename>)
	@description:
		Adds the <filename> to the playlist of the remote instance of the slave mp3 player.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

static bool mp3player_cmd_add( KviModule * m, KviCommand * c )
{
	ENTER_STACK_FRAME( c, "mp3player_cmd_add" );
	KviStr fName;
	if ( !g_pUserParser->parseCmdFinalPart( c, fName ) )
		return false;
	return c->leaveStackFrame();
}

/*
	@doc: mp3player.addurl
	@type:
		command
	@title:
		mp3player.addurl
	@short:
		Deprecated: present for compatibility only
	@syntax:
		mp3player.addurl(<url>)
	@description:
		Adds the <url> to the playlist of the remote instance of the slave mp3 player.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

static bool mp3player_cmd_addurl( KviModule * m, KviCommand * c )
{
	ENTER_STACK_FRAME( c, "mp3player_cmd_addurl" );
	KviStr fName;
	if ( !g_pUserParser->parseCmdFinalPart( c, fName ) )
		return false;
	return c->leaveStackFrame();
}

/*
	@doc: mp3player.gettitle
	@type:
		function
	@title:
		$mp3player.gettitle
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.gettitle
	@description:
		Returns the title of the multimedia file being played by the
		remote instance of the mp3 player.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/
static bool mp3player_fnc_gettitle( KviModule * m, KviCommand * c, KviParameterList * params, KviStr & buffer )
{
	ENTER_STACK_FRAME( c, "mp3player_fnc_gettitle" );
	KviStr szRetBuffer;
	g_pUserParser->substituteIdentifiers("$mediaplayer.nowPlaying()",c->window(),szRetBuffer);
	buffer.append(szRetBuffer);
	return c->leaveStackFrame();
}

/*
	@doc: mp3player.getfilename
	@type:
		function
	@title:
		$mp3player.getfilename
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.getfilename
	@description:
		Returns the file name (with path) of the multimedia file being played by the
		remote instance of the mp3 player.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

static bool mp3player_fnc_getfilename( KviModule * m, KviCommand * c, KviParameterList * params, KviStr & buffer )
{
	ENTER_STACK_FRAME( c, "mp3player_fnc_getfilename" );
	KviStr szRetBuffer;
	g_pUserParser->substituteIdentifiers("$mediaplayer.localFile()",c->window(),szRetBuffer);
	buffer.append(szRetBuffer);
	return c->leaveStackFrame();
}

/*
	@doc: mp3player.getposition
	@type:
		function
	@title:
		$mp3player.getposition
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.getposition
	@description:
		Returns the position in the playlist of the multimedia file being played by the
		remote instance of the mp3 player.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

static bool mp3player_fnc_getposition( KviModule * m, KviCommand * c, KviParameterList * params, KviStr & buffer )
{
	ENTER_STACK_FRAME( c, "mp3player_fnc_getposition" );
	KviStr szRetBuffer;
	g_pUserParser->substituteIdentifiers("$mediaplayer.position()",c->window(),szRetBuffer);
	buffer.append(szRetBuffer);
	return c->leaveStackFrame();
}

/*
	@doc: mp3player.getlength
	@type:
		function
	@title:
		$mp3player.getlength
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.getlength
	@description:
		Returns the length in milliseconds of the multimedia file being played by the
		remote instance of the mp3 player.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

static bool mp3player_fnc_getlength( KviModule * m, KviCommand * c, KviParameterList * params, KviStr & buffer )
{
	ENTER_STACK_FRAME( c, "mp3player_fnc_getlength" );
	KviStr szRetBuffer;
	g_pUserParser->substituteIdentifiers("$mediaplayer.length()",c->window(),szRetBuffer);
	buffer.append(szRetBuffer);
	return c->leaveStackFrame();
}

/*
	@doc: mp3player.gettime
	@type:
		function
	@title:
		$mp3player.gettime
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.gettime
	@description:
		Returns the currently elapsed time of the multimedia file being played by the
		remote instance of the mp3 player.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

static bool mp3player_fnc_gettime( KviModule * m, KviCommand * c, KviParameterList * params, KviStr & buffer )
{
	ENTER_STACK_FRAME( c, "mp3player_fnc_gettime" );
	KviStr szRetBuffer;
	g_pUserParser->substituteIdentifiers("$mediaplayer.position()",c->window(),szRetBuffer);
	buffer.append(szRetBuffer);
	return c->leaveStackFrame();
}

/*
	@doc: mp3player.getrate
	@type:
		function
	@title:
		$mp3player.getrate
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.getrate
	@description:
		Returns the rate of the multimedia file being played by the
		remote instance of the mp3 player.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/


static bool mp3player_fnc_getrate( KviModule * m, KviCommand * c, KviParameterList * params, KviStr & buffer )
{
	ENTER_STACK_FRAME( c, "mp3player_fnc_getrate" );
	KviStr szRetBuffer;
	g_pUserParser->substituteIdentifiers("$mediaplayer.bitRate()",c->window(),szRetBuffer);
	buffer.append(szRetBuffer);
	return c->leaveStackFrame();
}

/*
	@doc: mp3player.getchannels
	@type:
		function
	@title:
		$mp3player.getchannels
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.getchannels
	@description:
		Returns the number of channels of the multimedia file being played by the
		remote instance of the mp3 player.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

static bool mp3player_fnc_getchannels( KviModule * m, KviCommand * c, KviParameterList * params, KviStr & buffer )
{
	ENTER_STACK_FRAME( c, "mp3player_fnc_getchannels" );
	KviStr szRetBuffer;
	g_pUserParser->substituteIdentifiers("$mediaplayer.channels()",c->window(),szRetBuffer);
	buffer.append(szRetBuffer);
	return c->leaveStackFrame();
}

/*
	@doc: mp3player.getfreq
	@type:
		function
	@title:
		$mp3player.getfreq
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.getfreq
	@description:
		Returns the frequency of the multimedia file being played by the
		remote instance of the mp3 player.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

static bool mp3player_fnc_getfreq( KviModule * m, KviCommand * c, KviParameterList * params, KviStr & buffer )
{
	ENTER_STACK_FRAME( c, "mp3player_fnc_getfreq" );
	KviStr szRetBuffer;
	g_pUserParser->substituteIdentifiers("$mediaplayer.sampleRate()",c->window(),szRetBuffer);
	buffer.append(szRetBuffer);
	return c->leaveStackFrame();
}

/*
	@doc: mp3player.isplaying
	@type:
		function
	@title:
		$mp3player.isplaying
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.isplaying
	@description:
		Returns 1 if the remote instance of the mp3 player is in playback mode , 0 otherwise.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

static bool mp3player_fnc_isplaying( KviModule * m, KviCommand * c, KviParameterList * params, KviStr & buffer )
{
	ENTER_STACK_FRAME( c, "mp3player_fnc_isplaying" );
	KviStr szRetBuffer;
	g_pUserParser->substituteIdentifiers("$($mediaplayer.status() == \"playing\")",c->window(),szRetBuffer);
	buffer.append(szRetBuffer);
	return c->leaveStackFrame();
}

/*
	@doc: mp3player.ispaused
	@type:
		function
	@title:
		$mp3player.ispaused
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.ispaused
	@description:
		Returns 1 if the remote instance of the mp3 player is in pause mode , 0 otherwise.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

static bool mp3player_fnc_ispaused( KviModule * m, KviCommand * c, KviParameterList * params, KviStr & buffer )
{
	ENTER_STACK_FRAME( c, "mp3player_fnc_ispaused" );
	KviStr szRetBuffer;
	g_pUserParser->substituteIdentifiers("$($mediaplayer.status() == \"paused\")",c->window(),szRetBuffer);
	buffer.append(szRetBuffer);
	return c->leaveStackFrame();
}

/*
	@doc: mp3player.getmp3tag_title
	@type:
		function
	@title:
		$mp3player.getmp3tag_title
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.getmp3tag_title
	@description:
		Returns 1 if successful , 0 otherwise.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

static bool mp3player_fnc_getmp3tag_title( KviModule * m, KviCommand * c, KviParameterList * params, KviStr & buffer )
{
	ENTER_STACK_FRAME( c, "mp3player_fnc_getmp3tag_title" );
	KviStr szRetBuffer;
	g_pUserParser->substituteIdentifiers("$mediaplayer.title()",c->window(),szRetBuffer);
	buffer.append(szRetBuffer);
	return c->leaveStackFrame();
}


/*
	@doc: mp3player.getmp3tag_album
	@type:
		function
	@title:
		$mp3player.getmp3tag_album
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.getmp3tag_album
	@description:
		Returns 1 if successful , 0 otherwise.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

static bool mp3player_fnc_getmp3tag_album( KviModule * m, KviCommand * c, KviParameterList * params, KviStr & buffer )
{
	ENTER_STACK_FRAME( c, "mp3player_fnc_getmp3tag_album" );
	KviStr szRetBuffer;
	g_pUserParser->substituteIdentifiers("$mediaplayer.album()",c->window(),szRetBuffer);
	buffer.append(szRetBuffer);
	return c->leaveStackFrame();
}


/*
	@doc: mp3player.getmp3tag_date
	@type:
		function
	@title:
		$mp3player.getmp3tag_date
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.getmp3tag_date
	@description:
		Returns 1 if successful , 0 otherwise.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

static bool mp3player_fnc_getmp3tag_date( KviModule * m, KviCommand * c, KviParameterList * params, KviStr & buffer )
{
	ENTER_STACK_FRAME( c, "mp3player_fnc_getmp3tag_date" );
	return c->leaveStackFrame();
}

/*
	@doc: mp3player.getmp3tag_comment
	@type:
		function
	@title:
		$mp3player.getmp3tag_comment
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.getmp3tag_comment
	@description:
		Returns 1 if successful , 0 otherwise.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

static bool mp3player_fnc_getmp3tag_comment( KviModule * m, KviCommand * c, KviParameterList * params, KviStr & buffer )
{
	ENTER_STACK_FRAME( c, "mp3player_fnc_getmp3tag_comment" );
	KviStr szRetBuffer;
	g_pUserParser->substituteIdentifiers("$mediaplayer.comment()",c->window(),szRetBuffer);
	buffer.append(szRetBuffer);
	return c->leaveStackFrame();
}


/*
	@doc: mp3player.getmp3tag_genre
	@type:
		function
	@title:
		$mp3player.getmp3tag_genre
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.getmp3tag_genre
	@description:
		Returns 1 if successful , 0 otherwise.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

static bool mp3player_fnc_getmp3tag_genre( KviModule * m, KviCommand * c, KviParameterList * params, KviStr & buffer )
{
	ENTER_STACK_FRAME( c, "mp3player_fnc_getmp3tag_genre" );
	KviStr szRetBuffer;
	g_pUserParser->substituteIdentifiers("$mediaplayer.genre()",c->window(),szRetBuffer);
	buffer.append(szRetBuffer);
	return c->leaveStackFrame();
}

/*
	@doc: mp3player.getmp3tag_artist
	@type:
		function
	@title:
		$mp3player.getmp3tag_artist
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.getmp3tag_artist
	@description:
		Returns 1 if successful , 0 otherwise.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

static bool mp3player_fnc_getmp3tag_artist( KviModule * m, KviCommand * c, KviParameterList * params, KviStr & buffer )
{
	ENTER_STACK_FRAME( c, "mp3player_fnc_getmp3tag_artist" );
	KviStr szRetBuffer;
	g_pUserParser->substituteIdentifiers("$mediaplayer.artist()",c->window(),szRetBuffer);
	buffer.append(szRetBuffer);
	return c->leaveStackFrame();
}

/*
	@doc: mp3player.getmp3tag_version
	@type:
		function
	@title:
		$mp3player.getmp3tag_version
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.getmp3tag_version
	@description:
		Returns 1 if successful , 0 otherwise.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

static bool mp3player_fnc_getmp3tag_version( KviModule * m, KviCommand * c, KviParameterList * params, KviStr & buffer )
{
	ENTER_STACK_FRAME( c, "mp3player_fnc_getmp3tag_version" );
	return c->leaveStackFrame();
}

/*
	@doc: mp3player.getmp3tag_layer
	@type:
		function
	@title:
		$mp3player.getmp3tag_layer
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.getmp3tag_layer
	@description:
		Returns 1 if successful , 0 otherwise.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

static bool mp3player_fnc_getmp3tag_layer( KviModule * m, KviCommand * c, KviParameterList * params, KviStr & buffer )
{
	ENTER_STACK_FRAME( c, "mp3player_fnc_getmp3tag_layer" );
	return c->leaveStackFrame();
}

/*
	@doc: mp3player.getmp3tag_crc
	@type:
		function
	@title:
		$mp3player.getmp3tag_crc
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.getmp3tag_crc
	@description:
		Returns 1 if successful , 0 otherwise.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

static bool mp3player_fnc_getmp3tag_crc( KviModule * m, KviCommand * c, KviParameterList * params, KviStr & buffer )
{
	ENTER_STACK_FRAME( c, "mp3player_fnc_getmp3tag_crc" );
	return c->leaveStackFrame();
}

/*
	@doc: mp3player.getmp3tag_copright
	@type:
		function
	@title:
		$mp3player.getmp3tag_copright
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.getmp3tag_copright
	@description:
		Returns 1 if successful , 0 otherwise.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

static bool mp3player_fnc_getmp3tag_copyright( KviModule * m, KviCommand * c, KviParameterList * params, KviStr & buffer )
{
	ENTER_STACK_FRAME( c, "mp3player_fnc_getmp3tag_copyright" );
	return c->leaveStackFrame();
}

/*
	@doc: mp3player.getmp3tag_original
	@type:
		function
	@title:
		$mp3player.getmp3tag_original
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.getmp3tag_original
	@description:
		Returns 1 if successful , 0 otherwise.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

static bool mp3player_fnc_getmp3tag_original( KviModule * m, KviCommand * c, KviParameterList * params, KviStr & buffer )
{
	ENTER_STACK_FRAME( c, "mp3player_fnc_getmp3tag_original" );
	return c->leaveStackFrame();
}

/*
	@doc: mp3player.getmp3tag_emphasis
	@type:
		function
	@title:
		$mp3player.getmp3tag_emphasis
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.getmp3tag_emphasis
	@description:
		Returns 1 if successful , 0 otherwise.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/

static bool mp3player_fnc_getmp3tag_emphasis( KviModule * m, KviCommand * c, KviParameterList * params, KviStr & buffer )
{
	ENTER_STACK_FRAME( c, "mp3player_fnc_getmp3tag_emphasis" );
	return c->leaveStackFrame();
}

/*
	@doc: mp3player.getmp3tag_tracknumber
	@type:
		function
	@title:
		$mp3player.getmp3tag_tracknumber
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.getmp3tag_tracknumber
	@description:
		Returns 1 if successful , 0 otherwise.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/
static bool mp3player_fnc_getmp3tag_tracknumber( KviModule * m, KviCommand * c, KviParameterList * params, KviStr & buffer )
{
	ENTER_STACK_FRAME( c, "mp3player_fnc_getmp3tag_tracknumber" );
	return c->leaveStackFrame();
}


/*
	@doc: mp3player.mp3player_cmd_setdcop_player_type
	@type:
		function
	@title:
		$mp3player.mp3player_cmd_set_dcop_player_type
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.mp3player_cmd_set_dcop_player_type
	@description:
		Returns 1 if successful , 0 otherwise.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/
static bool mp3player_cmd_set_dcop_player_type( KviModule * m, KviCommand * c )
{
	ENTER_STACK_FRAME( c, "mp3player_cmd_set_dcop_player_type" );
	KviStr playerType;
	if ( !g_pUserParser->parseCmdFinalPart( c, playerType ) )
		return false;
	KviStr tmp = "mediaplayer.setplayer ";
	tmp += playerType;
	g_pUserParser->parseCommandBuffer(tmp.ptr(),c->window());
	return c->leaveStackFrame();
}


/*
	@doc: mp3player.mp3player_fnc_get_dcop_player_type
	@type:
		function
	@title:
		$mp3player.mp3player_fnc_get_dcop_player_type
	@short:
		Deprecated: present for compatibility only
	@syntax:
		$mp3player.mp3player_fnc_get_dcop_player_type
	@description:
		Returns 1 if successful , 0 otherwise.[br]
		Take a look at the [module:mp3player]mp3player module documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:mp3player]mp3 player module documentation[/module]
*/
static bool mp3player_fnc_get_dcop_player_type( KviModule * m, KviCommand * c, KviParameterList * params, KviStr & buffer )
{
	ENTER_STACK_FRAME( c, "mp3player_cmd_get_dcop_player_type" );
	KviStr szRetBuffer;
	g_pUserParser->substituteIdentifiers("$mediaplayer.player()",c->window(),szRetBuffer);
	buffer.append(szRetBuffer);
	return c->leaveStackFrame();
}


static bool mp3player_module_init( KviModule * m )
{
/*
	m->registerCommand( "play", mp3player_cmd_play );
	m->registerCommand( "stop", mp3player_cmd_stop );
	m->registerCommand( "next", mp3player_cmd_next );
	m->registerCommand( "prev", mp3player_cmd_prev );
	m->registerCommand( "pause", mp3player_cmd_pause );
	m->registerCommand( "add", mp3player_cmd_add );
	m->registerCommand( "addurl", mp3player_cmd_addurl );
	m->registerCommand( "quit", mp3player_cmd_quit );
	m->registerCommand( "setplayer", mp3player_cmd_set_dcop_player_type ); // Crissi
	m->registerFunction( "gettitle", mp3player_fnc_gettitle );
	m->registerFunction( "getposition", mp3player_fnc_getposition );
	m->registerFunction( "getfilename", mp3player_fnc_getfilename );
	m->registerFunction( "getrate", mp3player_fnc_getrate );
	m->registerFunction( "getfreq", mp3player_fnc_getfreq );
	m->registerFunction( "getchannels", mp3player_fnc_getchannels );
	m->registerFunction( "getlength", mp3player_fnc_getlength );
	m->registerFunction( "gettime", mp3player_fnc_gettime );
	m->registerFunction( "isplaying", mp3player_fnc_isplaying );
	m->registerFunction( "ispaused", mp3player_fnc_ispaused );
	m->registerFunction( "getplayer", mp3player_fnc_get_dcop_player_type ); // Crissi

	// Crissi
	m->registerFunction( "getmp3tag_title", mp3player_fnc_getmp3tag_title );
	m->registerFunction( "getmp3tag_artist", mp3player_fnc_getmp3tag_artist );
	m->registerFunction( "getmp3tag_album", mp3player_fnc_getmp3tag_album );
	m->registerFunction( "getmp3tag_date", mp3player_fnc_getmp3tag_date );
	m->registerFunction( "getmp3tag_comment", mp3player_fnc_getmp3tag_comment );
	m->registerFunction( "getmp3tag_genre", mp3player_fnc_getmp3tag_genre );
	m->registerFunction( "getmp3tag_version", mp3player_fnc_getmp3tag_version );
	m->registerFunction( "getmp3tag_layer", mp3player_fnc_getmp3tag_layer );
	m->registerFunction( "getmp3tag_crc", mp3player_fnc_getmp3tag_crc );
	m->registerFunction( "getmp3tag_copyright", mp3player_fnc_getmp3tag_copyright );
	m->registerFunction( "getmp3tag_original", mp3player_fnc_getmp3tag_original );
	m->registerFunction( "getmp3tag_emphasis", mp3player_fnc_getmp3tag_emphasis );
	m->registerFunction( "getmp3tag_tracknumber", mp3player_fnc_getmp3tag_tracknumber );
*/
	return true;
}

static bool mp3player_module_cleanup( KviModule * m )
{
	return true;
}

static bool mp3player_module_can_unload( KviModule * m )
{
	return true;
}

KVIRC_MODULE(
    "mp3player",                                              // module name
    "1.0.1",                                                // module version
    "Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)\nChristoph Thielecke (crissi99 at gmx dot de)", // author & (C)
    "Interface to mp3 players",
    mp3player_module_init,
    mp3player_module_can_unload,
    0,
    mp3player_module_cleanup
)
#endif