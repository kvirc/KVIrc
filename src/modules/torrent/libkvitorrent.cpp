//=============================================================================
//
//   File : libkvitorrent.cpp
//   Creation date : Fri Jan 1 15:42:25 2007 GMT by Alexander Stillich
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2005 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2007 Alexander Stillich (torque at pltn dot org)
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
//=============================================================================

#include "kvi_module.h"
#include "kvi_options.h"

#include "tc_interface.h"
#include "tc_ktorrentdcopinterface.h"

#include "kvi_locale.h"
#include "kvi_out.h"


static KviPtrList<KviTorrentInterfaceDescriptor> * g_pDescriptorList = 0;

// selected torrent interface
static KviTorrentInterface *g_pTcInterface = 0;

//static KviMediaPlayerInterface * g_pMPInterface = 0;


static KviTorrentInterface *auto_detect_torrent_client(KviWindow * pOut = 0)
{
	int iBest = 0;
	KviTorrentInterface * pBest = 0;
	KviTorrentInterfaceDescriptor * d;
	KviTorrentInterfaceDescriptor * pDBest = 0;

	for (d=g_pDescriptorList->first(); d!=0; d=g_pDescriptorList->next())
	{
//		printf("detect %s\n%s\n", d->name().ascii(), d->description().ascii());

		// instance gets deleted by descriptor later
		KviTorrentInterface *i = d->instance();
		if(i)
		{
//			printf("detect %p\n", i);
			int iScore = i->detect();
			if(iScore > iBest)
			{
				iBest = iScore;
				pBest = i;
				pDBest = d;
			}

			if(pOut)
			{
				pOut->output(KVI_OUT_TORRENT,
						   __tr2qs_ctx("Trying torrent client interface \"%Q\": score %d", "torrent"),
						   &d->name(),
						   iScore);
			}
		}
	}

/*	if(iBest < 90)
	{
		if(pOut)
			pOut->outputNoFmt(KVI_OUT_MULTIMEDIA,__tr2qs_ctx("Not sure about the results, trying a second, more agressive detection pass","mediaplayer"));
		// no sure player found... try again with a destructive test
		for(d = g_pDescriptorList->first();d;d = g_pDescriptorList->next())
		{
			KviMediaPlayerInterface * i = d->instance();
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
					KviQString::sprintf(szOut,__tr2qs_ctx("Trying media player interface \"%Q\": score %d","mediaplayer"),&(szNam),iScore);
					pOut->output(KVI_OUT_MULTIMEDIA,szOut);
				}
			}
		}
	}
*/
	if(pDBest)
	{
		// TODO: what's this?
//		KVI_OPTION_STRING(KviOption_stringPreferredMediaPlayer) = pDBest->name();
		if(pOut)
			pOut->output(KVI_OUT_TORRENT,
					   __tr2qs_ctx("Choosing torrent client interface \"%Q\"", "torrent"),
					   &pDBest->name());

	} else {
		if(pOut)
			pOut->outputNoFmt(KVI_OUT_TORRENT,
						   __tr2qs_ctx("Seems that there is no usable torrent client on this machine", "torrent"));

	}

	return pBest;
}


static bool torrent_test(KviKvsModuleCommandCall *c)
{
/*	kvs_real_t dReal; \
	KVSM_PARAMETERS_BEGIN(c) \
		KVSM_PARAMETER("test1", KVS_PT_REAL, 0, dReal) \
	KVSM_PARAMETERS_END(c) \
	c->returnValue()->setReal(dReal); \

	printf("test called %f\n", dReal);
*/
	if (!g_pTcInterface)
		return false;

	int count = g_pTcInterface->count();
	for (int i=0; i<count; i++)
	{
		QString n = g_pTcInterface->name(i);
		debug("%d: %s [%s]", i, n.ascii(), g_pTcInterface->state(i).ascii());
	}

	return true;
}

#define TC_KVS_FAIL_ON_NO_INTERFACE \
	if (!g_pTcInterface) \
	{ \
		c->warning(__tr2qs_ctx("No torrent client interface selected. Try /torrent.detect", "torrent")); \
		return true; \
	}

#define TC_KVS_COMMAND(__name) static bool torrent_kvs_cmd_ ## __name (KviKvsModuleCommandCall * c)
#define TC_KVS_FUNCTION(__name) static bool torrent_kvs_fnc_ ## __name (KviKvsModuleFunctionCall * c)

#define TC_KVS_COMMAND_ERROR	\
			if (!c->hasSwitch('q',"quiet")) \
			{ \
				c->warning(__tr2qs_ctx("The selected torrent client interface failed to execute the requested function", "torrent")); \
				QString tmp = __tr2qs_ctx("Last interface error: ", "torrent"); \
				tmp += g_pTcInterface->lastError(); \
				c->warning(tmp); \
			} \

#define TC_KVS_SIMPLE_COMMAND(__name, __ifacecommand) \
	TC_KVS_COMMAND(__name) \
	{ \
		KVSM_PARAMETERS_BEGIN(c) \
		KVSM_PARAMETERS_END(c) \
		\
		TC_KVS_FAIL_ON_NO_INTERFACE \
		\
		if (!g_pTcInterface->__ifacecommand()) \
		{ \
			TC_KVS_COMMAND_ERROR \
		} \
		return true; \
	}

#define TC_KVS_INT_COMMAND(__name, __ifacecommand, __argname) \
	TC_KVS_COMMAND(__name) \
	{ \
		debug("TC_KVS_INT_COMMAND"); \
		kvs_int_t arg; \
		KVSM_PARAMETERS_BEGIN(c) \
			KVSM_PARAMETER(__argname, KVS_PT_INT, 0, arg) \
		KVSM_PARAMETERS_END(c) \
		debug("TC_KVS_INT_COMMAND 2"); \
		TC_KVS_FAIL_ON_NO_INTERFACE \
		\
		if (!g_pTcInterface->__ifacecommand(arg)) \
		{ \
			debug("TC_KVS_INT_COMMAND 3"); \
			TC_KVS_COMMAND_ERROR \
		} \
		debug("TC_KVS_INT_COMMAND 4"); \
		return true; \
	}

#define TC_KVS_STRINGRET_FUNCTION(__name, __ifacecommand) \
	TC_KVS_FUNCTION(__name) \
	{ \
		TC_KVS_FAIL_ON_NO_INTERFACE \
		QString szRet = g_pTcInterface->__ifacecommand(); \
		c->returnValue()->setString(szRet); \
		return true; \
	}

#define TC_KVS_INTRET_FUNCTION(__name, __ifacecommand) \
	TC_KVS_FUNCTION(__name) \
	{ \
		TC_KVS_FAIL_ON_NO_INTERFACE \
		kvs_int_t ret = g_pTcInterface->__ifacecommand(); \
		c->returnValue()->setInteger(ret); \
		return true; \
	}

#define TC_KVS_FLOATRET_FUNCTION(__name, __ifacecommand) \
	TC_KVS_FUNCTION(__name) \
	{ \
		TC_KVS_FAIL_ON_NO_INTERFACE \
		kvs_real_t ret = g_pTcInterface->__ifacecommand(); \
		c->returnValue()->setReal(ret); \
		return true; \
	}

#define TC_KVS_STRINGRET_INT_FUNCTION(__name, __ifacecommand, __argname) \
	TC_KVS_FUNCTION(__name) \
	{ \
		kvs_int_t arg; \
		KVSM_PARAMETERS_BEGIN(c) \
			KVSM_PARAMETER(__argname, KVS_PT_INT, 0, arg) \
		KVSM_PARAMETERS_END(c) \
		TC_KVS_FAIL_ON_NO_INTERFACE \
		QString szRet = g_pTcInterface->__ifacecommand(arg); \
		c->returnValue()->setString(szRet); \
		return true; \
	}

/*
#define MP_KVS_SIMPLE_INT_FUNCTION(__name,__ifacecommand) \
MP_KVS_FUNCTION(__name) \
{ \
	MP_KVS_FAIL_ON_NO_INTERFACE \
	int iRet = g_pMPInterface->__ifacecommand(); \
	c->returnValue()->setInteger(iRet); \
	return true; \
}
// FINDME!
#define MP_KVS_SIMPLE_BOOL_FUNCTION(__name,__ifacecommand) \
MP_KVS_FUNCTION(__name) \
{ \
	MP_KVS_FAIL_ON_NO_INTERFACE \
	bool bRet = g_pMPInterface->__ifacecommand(); \
	c->returnValue()->setBoolean(bRet); \
	return true; \
}
*/

TC_KVS_COMMAND(detect)
{
	g_pTcInterface = auto_detect_torrent_client(c->hasSwitch('q',"quiet") ? 0 : c->window());
	return true;
}

/*
	@doc: torrent.maxUploadSpeed
	@type:
		function
	@title:
		$torrent.maxUploadSpeed
	@short:
		Returns maximum upload speed set in client.
	@syntax:
		$torrent.maxUploadSpeed()
	@description:
		Returns maximum upload speed set in client.
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
*/
TC_KVS_INTRET_FUNCTION(maxUploadSpeed, maxUploadSpeed)


/*
	@doc: torrent.maxDownloadSpeed
	@type:
		function
	@title:
		$torrent.maxDownloadSpeed
	@short:
		Returns maximum download speed set in client.
	@syntax:
		$torrent.maxDownloadSpeed()
	@description:
		Returns maximum download speed set in client.
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
*/
TC_KVS_INTRET_FUNCTION(maxDownloadSpeed, maxDownloadSpeed)

/*
	@doc: torrent.setMaxUploadSpeed
	@type:
		command
	@title:
		torrent.setMaxUploadSpeed
	@short:
		Sets maximum upload speed
	@syntax:
		torrent.setMaxUploadSpeed <kbytes_per_sec>
	@description:
		Sets maximum upload speed
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
*/
TC_KVS_INT_COMMAND(setMaxUploadSpeed, setMaxUploadSpeed, "kbytes_per_sec")

/*
	@doc: torrent.setMaxDownloadSpeed
	@type:
		command
	@title:
		torrent.setMaxDownloadSpeed
	@short:
		Sets maximum download speed
	@syntax:
		torrent.setMaxDownloadSpeed <kbytes_per_sec>
	@description:
		Sets maximum download speed
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
*/
TC_KVS_INT_COMMAND(setMaxDownloadSpeed, setMaxDownloadSpeed, "kbytes_per_sec")

/*
	@doc: torrent.speedUp
	@type:
		function
	@title:
		$torrent.speedUp
	@short:
		Returns current total upload speed
	@syntax:
		$torrent.speedUp()
	@description:
		Returns current total upload speed
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
*/
TC_KVS_FLOATRET_FUNCTION(speedUp, speedUp)

/*
	@doc: torrent.speedDown
	@type:
		function
	@title:
		$torrent.speedDown
	@short:
		Returns current total download speed
	@syntax:
		$torrent.speedDown()
	@description:
		Returns current total download speed
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
*/
TC_KVS_FLOATRET_FUNCTION(speedDown, speedDown)

/*
	@doc: torrent.count
	@type:
		function
	@title:
		$torrent.count
	@short:
		Returns number of torrents in client
	@syntax:
		$torrent.name()
	@description:
		Returns number of torrents in client
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
*/
TC_KVS_INTRET_FUNCTION(count, count)

/*
	@doc: torrent.name
	@type:
		function
	@title:
		$torrent.name
	@short:
		Returns torrent name as displayed in client
	@syntax:
		$torrent.name(torrent_number)
	@description:
		Returns torrent name as displayed in client
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
*/
TC_KVS_STRINGRET_INT_FUNCTION(name, name, "torrent_number")

/*
	@doc: torrent.start
	@type:
		command
	@title:
		torrent.start
	@short:
		Starts downloading of torrent <torrent_number>
	@syntax:
		torrent.start <torrent_number>
	@description:
		Starts downloading of torrent <torrent_number>
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
		[cmd]torrent.stop[/cmd], [cmd]torrent.stopAll[/cmd], [cmd]torrent.startAll[/cmd]
*/
TC_KVS_INT_COMMAND(start, start, "torrent_number")

/*
	@doc: torrent.stop
	@type:
		command
	@title:
		torrent.stop
	@short:
		Stops downloading of torrent <torrent_number>
	@syntax:
		torrent.stop <torrent_number>
	@description:
		Stops downloading of torrent <torrent_number>
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
		[cmd]torrent.stopAll[/cmd], [cmd]torrent.startAll[/cmd], [cmd]torrent.start[/cmd]
*/
TC_KVS_INT_COMMAND(stop, stop, "torrent_number")

/*
	@doc: torrent.announce
	@type:
		command
	@title:
		torrent.announce
	@short:
		Manually announces torrent to tracker
	@syntax:
		torrent.announce <torrent_number>
	@description:
		Manually announces torrent to tracker
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
*/
TC_KVS_INT_COMMAND(announce, announce, "torrent_number")

/*
	@doc: torrent.startAll
	@type:
		command
	@title:
		torrent.startAll
	@short:
		Starts downloading of all torrents
	@syntax:
		torrent.startAll
	@description:
		Starts downloading of all torrents
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
		[cmd]torrent.stopAll[/cmd], [cmd]torrent.start[/cmd], [cmd]torrent.stop[/cmd]
*/

TC_KVS_SIMPLE_COMMAND(startAll, startAll)

/*
	@doc: torrent.stopAll
	@type:
		command
	@title:
		torrent.stopAll
	@short:
		Stops downloading of all torrents
	@syntax:
		torrent.stopAll
	@description:
		Stops downloading of all torrents
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
		[cmd]torrent.startAll[/cmd], [cmd]torrent.start[/cmd], [cmd]torrent.stop[/cmd]
*/

TC_KVS_SIMPLE_COMMAND(stopAll, stopAll)


/*
	@doc: torrent.list
	@type:
		function
	@title:
		torrent.list
	@short:
		Returns a list of all torrents
	@syntax:
		$torrent.list()
	@description:
		Returns a list of all torrents
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
		[cmd]torrent.startAll[/cmd], [cmd]torrent.start[/cmd], [cmd]torrent.stop[/cmd]
*/

//MP_KVS_SIMPLE_COMMAND(list,list)

/*
	@doc: torrent.status
	@type:
		function
	@title:
		torrent.status
	@short:
		Returns status of a torrent
	@syntax:
		$torrent.status(<torrent_number>)
	@description:
		Returns the status of torrent <torrent_number>
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
		[fnc]torrent.list[/fnc], [cmd]torrent.start[/cmd], [cmd]torrent.stop[/cmd], [cmd]torrent.startAll[/cmd], [cmd]torrent.stopAll[/cmd]
*/

//MP_KVS_SIMPLE_COMMAND(list,list)

/*
	@doc: torrent.setClient
	@type:
		command
	@title:
		torrent.setClient
	@short:
		Sets the torrent client interface
	@syntax:
		torrent.setClient <client_name>
	@description:
		Sets the torrent client interface to be used by the
		torrent interface module. <interface_name> must be one
		of the client names returned by [cmd]torrent.clientList[/cmd]
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
		[cmd]torrent.detect[/cmd], [fnc]$torrent.client()[/fnc]
*/
/*
MP_KVS_COMMAND(setClient)
{
	QString szPlayer;

	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("player",KVS_PT_STRING,0,szPlayer)
	KVSM_PARAMETERS_END(c)

	for(KviMediaPlayerInterfaceDescriptor * d = g_pDescriptorList->first();d;d = g_pDescriptorList->next())
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
*/
/*
	@doc: torrent.client
	@type:
		function
	@title:
		$torrent.client
	@short:
		Returns the currently set torrent client interface
	@syntax:
		$torrent.client()
	@description:
		Returns the currently set torrent client interface.
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
		[cmd]torrent.detect[/cmd], [cmd]torrent.setClient[/cmd]
*/

/*MP_KVS_FUNCTION(client)
{
	c->returnValue()->setString(KVI_OPTION_STRING(KviOption_stringPreferredMediaPlayer));
	return true;
}
*/
/*
	@doc: torrent.clientList
	@type:
		command
	@title:
		torrent.clientList
	@short:
		Returns a list of all supported clients
	@syntax:
		torrent.clientList
	@description:
		Returns a list of all supported clients
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
		[cmd]torrent.detect[/cmd], [cmd]torrent.setClient[/cmd], [cmd]torrent.client[/cmd]
*/
/*MP_KVS_FUNCTION(clientList)
{
	//c->returnValue()->setString(KVI_OPTION_STRING(KviOption_stringPreferredMediaPlayer));
	return true;
}
*/

/*
	@doc: torrent.state
	@type:
		function
	@title:
		$torrent.state
	@short:
		Returns state of torrent (Stopped, Stalled, Seeding, Downloading)
	@syntax:
		$torrent.state <torrent_number>
	@description:
		Returns state of torrent (Stopped, Stalled, Seeding, Downloading)
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
*/
TC_KVS_STRINGRET_INT_FUNCTION(state, state, "torrent_number")

static bool torrent_module_init(KviModule *m)
{
	#define TC_KVS_REGCMD(__name,__stringname) KVSM_REGISTER_SIMPLE_COMMAND(m, __stringname, torrent_kvs_cmd_ ## __name)
	#define TC_KVS_REGFNC(__name,__stringname) KVSM_REGISTER_FUNCTION(m, __stringname, torrent_kvs_fnc_ ## __name)

	KVSM_REGISTER_SIMPLE_COMMAND(m, "test", torrent_test);
	TC_KVS_REGCMD(detect, "detect");
	TC_KVS_REGCMD(start, "start")
	TC_KVS_REGCMD(stop, "stop")
	TC_KVS_REGCMD(announce, "announce")
	TC_KVS_REGCMD(startAll, "startAll")
	TC_KVS_REGCMD(stopAll, "stopAll")
	TC_KVS_REGCMD(setMaxUploadSpeed, "setMaxUploadSpeed")
	TC_KVS_REGCMD(setMaxDownloadSpeed, "setMaxDownloadSpeed")
	TC_KVS_REGFNC(maxUploadSpeed, "maxUploadSpeed")
	TC_KVS_REGFNC(maxDownloadSpeed, "maxDownloadSpeed")
	TC_KVS_REGFNC(speedUp, "speedUp")
	TC_KVS_REGFNC(speedDown, "speedDown")
	TC_KVS_REGFNC(count, "count")
	TC_KVS_REGFNC(name, "name")
	TC_KVS_REGFNC(state, "state")

	g_pDescriptorList = new KviPtrList<KviTorrentInterfaceDescriptor>;
	g_pDescriptorList->setAutoDelete(true);

#ifdef COMPILE_KDE_SUPPORT
	g_pDescriptorList->append(new KviKTorrentDCOPInterfaceDescriptor);
#endif // COMPILE_KDE_SUPPORT

	g_pTcInterface = 0;

/*	if(KVI_OPTION_STRING(KviOption_stringPreferredMediaPlayer) == "auto")
	{
		g_pMPInterface = auto_detect_player();

	} else
	{
		for (KviMediaPlayerInterfaceDescriptor *d=g_pDescriptorList->first(); d; d=g_pDescriptorList->next())
		{
			if (d->name() == KVI_OPTION_STRING(KviOption_stringPreferredMediaPlayer))
			{
				g_pMPInterface = d->instance();
			}
		}
	}

	// check for "auto" interface too!


	#define MP_KVS_REGCMD(__name,__stringname) KVSM_REGISTER_SIMPLE_COMMAND(m,__stringname,mediaplayer_kvs_cmd_ ## __name)
	#define MP_KVS_REGFNC(__name,__stringname) KVSM_REGISTER_FUNCTION(m,__stringname,mediaplayer_kvs_fnc_ ## __name)

	MP_KVS_REGCMD(play,"play");
	MP_KVS_REGCMD(stop,"stop");
	MP_KVS_REGCMD(next,"next");
	MP_KVS_REGCMD(prev,"prev");
	MP_KVS_REGCMD(quit,"quit");
	MP_KVS_REGCMD(pause,"pause");
	MP_KVS_REGCMD(detect,"detect");
	MP_KVS_REGCMD(playMrl,"playMrl");
	MP_KVS_REGCMD(hide,"hide");
	MP_KVS_REGCMD(show,"show");
	MP_KVS_REGCMD(minimize,"minimize");
	MP_KVS_REGCMD(jumpTo,"jumpTo");
	MP_KVS_REGCMD(setPlayer,"setPlayer");
	MP_KVS_REGCMD(setVol,"setVol");
	MP_KVS_REGCMD(mute,"mute");
	MP_KVS_REGCMD(setRepeat,"setRepeat");
	MP_KVS_REGCMD(setShuffle,"setShuffle");
	MP_KVS_REGCMD(setPlayListPos,"setPlayListPos");
	MP_KVS_REGCMD(setEqData,"setEqData");

	MP_KVS_REGFNC(nowPlaying,"nowPlaying");
	MP_KVS_REGFNC(mrl,"mrl");
	MP_KVS_REGFNC(title,"title");
	MP_KVS_REGFNC(artist,"artist");
	MP_KVS_REGFNC(genre,"genre");
	MP_KVS_REGFNC(year,"year");
	MP_KVS_REGFNC(comment,"comment");
	MP_KVS_REGFNC(album,"album");
	MP_KVS_REGFNC(mediaType,"mediaType");
	MP_KVS_REGFNC(bitRate,"bitRate");
	MP_KVS_REGFNC(sampleRate,"sampleRate");
	MP_KVS_REGFNC(length,"length");
	MP_KVS_REGFNC(position,"position");
	MP_KVS_REGFNC(status,"status");
	MP_KVS_REGFNC(player,"player");
	MP_KVS_REGFNC(playerList,"playerList");
	MP_KVS_REGFNC(localFile,"localFile");
	MP_KVS_REGFNC(channels,"channels");
	MP_KVS_REGFNC(getListLength,"getListLength");
	MP_KVS_REGFNC(getPlayListPos,"getPlayListPos");
	MP_KVS_REGFNC(getEqData,"getEqData");
	MP_KVS_REGFNC(getRepeat,"getRepeat");
	MP_KVS_REGFNC(getShuffle,"getShuffle");
	MP_KVS_REGFNC(getVol,"getVol");

*/
/*
	// Crissi
	m->registerFunction( "getmp3tag_date", mediaplayer_fnc_getmp3tag_date );
	m->registerFunction( "getmp3tag_version", mediaplayer_fnc_getmp3tag_version );
	m->registerFunction( "getmp3tag_layer", mediaplayer_fnc_getmp3tag_layer );
	m->registerFunction( "getmp3tag_crc", mediaplayer_fnc_getmp3tag_crc );
	m->registerFunction( "getmp3tag_copyright", mediaplayer_fnc_getmp3tag_copyright );
	m->registerFunction( "getmp3tag_original", mediaplayer_fnc_getmp3tag_original );
	m->registerFunction( "getmp3tag_emphasis", mediaplayer_fnc_getmp3tag_emphasis );
	m->registerFunction( "getmp3tag_tracknumber", mediaplayer_fnc_getmp3tag_tracknumber );
*/
	return true;
}

static bool torrent_module_cleanup( KviModule * m )
{
	delete g_pDescriptorList;
	return true;
}

static bool torrent_module_can_unload( KviModule * m )
{
	return true;
}

static bool torrent_module_ctrl(KviModule * m,const char * operation,void * param)
{
/*	if(kvi_strEqualCI(operation,"getAvailableMediaPlayers"))
	{
		// we expect param to be a pointer to QStringList
		QStringList * l = (QStringList *)param;
		for(KviMediaPlayerInterfaceDescriptor * d = g_pDescriptorList->first();d;d = g_pDescriptorList->next())
		{
			l->append(d->name());
		}
		return true;
	}
	if(kvi_strEqualCI(operation,"detectMediaPlayer"))
	{
		auto_detect_player(0);
		return true;
	}
*/
	return false;
}



KVIRC_MODULE(
	"torrent",
	"1.0.0",
	"Copyright (C) 2007 Alexander Stillich (torque at pltn dot org)",
	"Interface to various torrent clients",
	torrent_module_init,
	torrent_module_can_unload,
	torrent_module_ctrl,
	torrent_module_cleanup
)
