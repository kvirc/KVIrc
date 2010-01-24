//=============================================================================
//
//   File : libkvitorrent.cpp
//   Creation date : Fri Jan 1 15:42:25 2007 GMT by Alexander Stillich
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Alexander Stillich (torque at pltn dot org)
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================


#include "tc_interface.h"
#include "tc_ktorrentdbusinterface.h"
#include "tc_statusbarapplet.h"

#include "kvi_module.h"
#include "kvi_options.h"
#include "kvi_frame.h"
#include "kvi_locale.h"
#include "kvi_out.h"

static KviPointerList<KviTorrentInterfaceDescriptor> * g_pDescriptorList = 0;

static KviTorrentInterface *auto_detect_torrent_client(KviWindow * pOut = 0)
{
	int iBest = 0;
	KviTorrentInterface * pBest = 0;
	KviTorrentInterfaceDescriptor * d;
	KviTorrentInterfaceDescriptor * pDBest = 0;

	for (d=g_pDescriptorList->first(); d!=0; d=g_pDescriptorList->next())
	{
		// instance gets deleted by descriptor later
		KviTorrentInterface *i = d->instance();
		if (i)
		{
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
			pOut->outputNoFmt(KVI_OUT_MULTIMEDIA,__tr2qs_ctx("Not sure about the results, trying a second, more agressive detection pass","torrent"));
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
					KviQString::sprintf(szOut,__tr2qs_ctx("Trying media player interface \"%Q\": score %d","torrent"),&(szNam),iScore);
					pOut->output(KVI_OUT_MULTIMEDIA,szOut);
				}
			}
		}
	}
*/
	if(pDBest)
	{
		KVI_OPTION_STRING(KviOption_stringPreferredTorrentClient) = pDBest->name();
		if(pOut)
			pOut->output(KVI_OUT_TORRENT,
					__tr2qs_ctx("Choosing torrent client interface \"%Q\"", "torrent"),
					&pDBest->name());

	} else
	{
		if(pOut)
			pOut->outputNoFmt(KVI_OUT_TORRENT,
					__tr2qs_ctx("Seems that there is no usable torrent client on this machine", "torrent"));
	}

	return pBest;
}

#define TC_KVS_FAIL_ON_NO_INTERFACE \
	if (!KviTorrentInterface::selected()) \
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
				tmp += KviTorrentInterface::selected()->lastError(); \
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
		if (!KviTorrentInterface::selected()->__ifacecommand()) \
		{ \
			TC_KVS_COMMAND_ERROR \
		} \
		return true; \
	}

#define TC_KVS_INT_COMMAND(__name, __ifacecommand, __argname) \
	TC_KVS_COMMAND(__name) \
	{ \
		kvs_int_t arg; \
		KVSM_PARAMETERS_BEGIN(c) \
			KVSM_PARAMETER(__argname, KVS_PT_INT, 0, arg) \
		KVSM_PARAMETERS_END(c) \
		\
		TC_KVS_FAIL_ON_NO_INTERFACE \
		\
		if (!KviTorrentInterface::selected()->__ifacecommand(arg)) \
		{ \
			TC_KVS_COMMAND_ERROR \
		} \
		return true; \
	}

#define TC_KVS_INT_INT_STRING_COMMAND(__name, __ifacecommand, __argname1, __argname2, __argname3) \
	TC_KVS_COMMAND(__name) \
	{ \
		kvs_int_t arg1; \
		kvs_int_t arg2; \
		QString arg3; \
		KVSM_PARAMETERS_BEGIN(c) \
			KVSM_PARAMETER(__argname1, KVS_PT_INT, 0, arg1) \
			KVSM_PARAMETER(__argname2, KVS_PT_INT, 0, arg2) \
			KVSM_PARAMETER(__argname3, KVS_PT_STRING, 0, arg3) \
		KVSM_PARAMETERS_END(c) \
		\
		TC_KVS_FAIL_ON_NO_INTERFACE \
		\
		if (!KviTorrentInterface::selected()->__ifacecommand(arg1, arg2, arg3)) \
		{ \
			TC_KVS_COMMAND_ERROR \
		} \
		return true; \
	}

// TODO: error handling for functions

#define TC_KVS_STRINGRET_FUNCTION(__name, __ifacecommand) \
	TC_KVS_FUNCTION(__name) \
	{ \
		TC_KVS_FAIL_ON_NO_INTERFACE \
		QString szRet = KviTorrentInterface::selected()->__ifacecommand(); \
		c->returnValue()->setString(szRet); \
		return true; \
	}

#define TC_KVS_INTRET_FUNCTION(__name, __ifacecommand) \
	TC_KVS_FUNCTION(__name) \
	{ \
		TC_KVS_FAIL_ON_NO_INTERFACE \
		kvs_int_t ret = KviTorrentInterface::selected()->__ifacecommand(); \
		c->returnValue()->setInteger(ret); \
		return true; \
	}

#define TC_KVS_FLOATRET_FUNCTION(__name, __ifacecommand) \
	TC_KVS_FUNCTION(__name) \
	{ \
		TC_KVS_FAIL_ON_NO_INTERFACE \
		kvs_real_t ret = KviTorrentInterface::selected()->__ifacecommand(); \
		c->returnValue()->setReal(ret); \
		return true; \
	}

#define TC_KVS_INTRET_INT_FUNCTION(__name, __ifacecommand, __argname) \
	TC_KVS_FUNCTION(__name) \
	{ \
		kvs_int_t arg; \
		KVSM_PARAMETERS_BEGIN(c) \
			KVSM_PARAMETER(__argname, KVS_PT_INT, 0, arg) \
		KVSM_PARAMETERS_END(c) \
		TC_KVS_FAIL_ON_NO_INTERFACE \
		kvs_int_t ret = KviTorrentInterface::selected()->__ifacecommand(arg); \
		c->returnValue()->setInteger(ret); \
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
		QString szRet = KviTorrentInterface::selected()->__ifacecommand(arg); \
		c->returnValue()->setString(szRet); \
		return true; \
	}

#define TC_KVS_STRINGRET_INT_INT_FUNCTION(__name, __ifacecommand, __argname1, __argname2) \
	TC_KVS_FUNCTION(__name) \
	{ \
		kvs_int_t arg1; \
		kvs_int_t arg2; \
		KVSM_PARAMETERS_BEGIN(c) \
			KVSM_PARAMETER(__argname1, KVS_PT_INT, 0, arg1) \
			KVSM_PARAMETER(__argname2, KVS_PT_INT, 0, arg2) \
		KVSM_PARAMETERS_END(c) \
		TC_KVS_FAIL_ON_NO_INTERFACE \
		QString szRet = KviTorrentInterface::selected()->__ifacecommand(arg1, arg2); \
		c->returnValue()->setString(szRet); \
		return true; \
	}

#define TC_KVS_INTRET_INT_INT_FUNCTION(__name, __ifacecommand, __argname1, __argname2) \
	TC_KVS_FUNCTION(__name) \
	{ \
		kvs_int_t arg1; \
		kvs_int_t arg2; \
		KVSM_PARAMETERS_BEGIN(c) \
			KVSM_PARAMETER(__argname1, KVS_PT_INT, 0, arg1) \
			KVSM_PARAMETER(__argname2, KVS_PT_INT, 0, arg2) \
		KVSM_PARAMETERS_END(c) \
		TC_KVS_FAIL_ON_NO_INTERFACE \
		kvs_int_t ret = KviTorrentInterface::selected()->__ifacecommand(arg1, arg2); \
		c->returnValue()->setInteger(ret); \
		return true; \
	}

TC_KVS_COMMAND(detect)
{
	KviTorrentInterface::select(auto_detect_torrent_client(c->hasSwitch('q',"quiet") ? 0 : c->window()));
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
	@doc: torrent.trafficUp
	@type:
		function
	@title:
		$torrent.trafficUp
	@short:
		Returns the total number of bytes uploaded
	@syntax:
		$torrent.trafficUp()
	@description:
		Returns the total number of bytes uploaded
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
*/
TC_KVS_FLOATRET_FUNCTION(trafficUp, trafficUp)

/*
	@doc: torrent.trafficDown
	@type:
		function
	@title:
		$torrent.trafficDown
	@short:
		Returns the total number of bytes download
	@syntax:
		$torrent.trafficDown()
	@description:
		Returns the total number of bytes download
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
*/
TC_KVS_FLOATRET_FUNCTION(trafficDown, trafficDown)

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
		[cmd]torrent.stop[/cmd], [cmd]torrent.stopAll[/cmd],
		[cmd]torrent.startAll[/cmd]
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
		[cmd]torrent.stopAll[/cmd], [cmd]torrent.startAll[/cmd],
		[cmd]torrent.start[/cmd]
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
	@doc: torrent.fileCount
	@type:
		function
	@title:
		$torrent.fileCount
	@short:
		Returns the number of files in a torrent.
	@syntax:
		$torrent.fileCount <torrent_number>
	@description:
		Returns the number of files in a torrent.
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
*/
TC_KVS_INTRET_INT_FUNCTION(fileCount, fileCount, "torrent_number")

/*
	@doc: torrent.fileName
	@type:
		function
	@title:
		$torrent.fileName
	@short:
		Returns the name of a file in a torrent.
	@syntax:
		$torrent.fileName <torrent_number> <file_number>
	@description:
		Returns the name of a file in a torrent.
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
*/
TC_KVS_STRINGRET_INT_INT_FUNCTION(fileName, fileName, "torrent_number", "file_number")

/*
	@doc: torrent.filePriority
	@type:
		function
	@title:
		$torrent.filePriority
	@short:
		Returns the priority of a file in a torrent.
	@syntax:
		$torrent.filePriority <torrent_number> <file_number>
	@description:
		Returns the priority of a file in a torrent.
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
*/
TC_KVS_STRINGRET_INT_INT_FUNCTION(filePriority, filePriority, "torrent_number", "file_number")

/*
	@doc: torrent.setFilePriority
	@type:
		command
	@title:
		torrent.setFilePriority
	@short:
		Sets the priority of a file in a torrent.
	@syntax:
		torrent.setFilePriority <torrent_number> <file_number>
	@description:
		Sets the priority of a file in a torrent.
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
*/
TC_KVS_INT_INT_STRING_COMMAND(setFilePriority, setFilePriority, "torrent_number", "file_number", "priority")

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
		[cmd]torrent.stopAll[/cmd], [cmd]torrent.start[/cmd],
		[cmd]torrent.stop[/cmd]
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
		[cmd]torrent.startAll[/cmd], [cmd]torrent.start[/cmd],
		[cmd]torrent.stop[/cmd]
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
		[cmd]torrent.startAll[/cmd], [cmd]torrent.start[/cmd],
		[cmd]torrent.stop[/cmd]
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
		[cmd]torrent.detect[/cmd], [fnc]$torrent.client[/fnc]
*/

TC_KVS_COMMAND(setClient)
{
	QString client;

	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("client", KVS_PT_STRING, 0, client)
	KVSM_PARAMETERS_END(c)

	for (KviTorrentInterfaceDescriptor *d = g_pDescriptorList->first(); d; d=g_pDescriptorList->next())
	{
		if (d->name() == client)
		{
			KviTorrentInterface::select(d->instance());
			KVI_OPTION_STRING(KviOption_stringPreferredTorrentClient) = client;

			if (!c->hasSwitch('q',"quiet"))
				c->window()->output(KVI_OUT_TORRENT,
							__tr2qs_ctx("Using client interface \"%Q\".", "torrent"),
							&client);
			return true;
		}
	}

	if (!c->hasSwitch('q',"quiet"))
		c->window()->output(KVI_OUT_TORRENT,
					__tr2qs_ctx("Invalid client interface \"%Q\"!", "torrent"),
					&client);

	return false;
}


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
TC_KVS_FUNCTION(client)
{
	c->returnValue()->setString(KVI_OPTION_STRING(KviOption_stringPreferredTorrentClient));
	return true;
}

/*
	@doc: torrent.clientList
	@type:
		function
	@title:
		$torrent.clientList
	@short:
		Returns a list of all supported clients.
	@syntax:
		$torrent.clientList()
	@description:
		Returns a list of all supported clients.
		Take a look at the [module:torrent]torrent client documentation[/module]
		for more details about how it works.[br]
	@seealso:
		[module:torrent]torrent client documentation[/module],
		[cmd]torrent.detect[/cmd], [cmd]torrent.setClient[/cmd],
		[cmd]torrent.client[/cmd]
*/
TC_KVS_FUNCTION(clientList)
{
	KviKvsArray *pArray = new KviKvsArray();
	int id=0;

	for (KviTorrentInterfaceDescriptor *d=g_pDescriptorList->first(); d; d=g_pDescriptorList->next())
		pArray->set(id++, new KviKvsVariant(d->name()));

	c->returnValue()->setArray(pArray);
	return true;
}

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

	TC_KVS_REGCMD(detect, "detect");
	TC_KVS_REGCMD(setClient, "setClient");
	TC_KVS_REGCMD(start, "start")
	TC_KVS_REGCMD(stop, "stop")
	TC_KVS_REGCMD(announce, "announce")
	TC_KVS_REGCMD(startAll, "startAll")
	TC_KVS_REGCMD(stopAll, "stopAll")
	TC_KVS_REGCMD(setMaxUploadSpeed, "setMaxUploadSpeed")
	TC_KVS_REGCMD(setMaxDownloadSpeed, "setMaxDownloadSpeed")
	TC_KVS_REGCMD(setFilePriority, "setFilePriority")
	TC_KVS_REGFNC(client, "client")
	TC_KVS_REGFNC(clientList, "clientList")
	TC_KVS_REGFNC(maxUploadSpeed, "maxUploadSpeed")
	TC_KVS_REGFNC(maxDownloadSpeed, "maxDownloadSpeed")
	TC_KVS_REGFNC(speedUp, "speedUp")
	TC_KVS_REGFNC(speedDown, "speedDown")
	TC_KVS_REGFNC(trafficUp, "trafficUp")
	TC_KVS_REGFNC(trafficDown, "trafficDown")
	TC_KVS_REGFNC(count, "count")
	TC_KVS_REGFNC(name, "name")
	TC_KVS_REGFNC(state, "state")
	TC_KVS_REGFNC(fileCount, "fileCount")
	TC_KVS_REGFNC(fileName, "fileName")
	TC_KVS_REGFNC(filePriority, "filePriority")

	g_pDescriptorList = new KviPointerList<KviTorrentInterfaceDescriptor>;
	g_pDescriptorList->setAutoDelete(true);

#ifdef COMPILE_KDE_SUPPORT
	g_pDescriptorList->append(new KviKTorrentDBusInterfaceDescriptor);
#endif // COMPILE_KDE_SUPPORT

	KviTorrentInterface::select(0);

	if (g_pFrame->mainStatusBar())
		KviTorrentStatusBarApplet::selfRegister(g_pFrame->mainStatusBar());


	if(KVI_OPTION_STRING(KviOption_stringPreferredMediaPlayer) == "auto")
	{
		KviTorrentInterface::select(auto_detect_torrent_client());

	} else
	{
		for (KviTorrentInterfaceDescriptor *d=g_pDescriptorList->first(); d; d=g_pDescriptorList->next())
		{
			if (d->name() == KVI_OPTION_STRING(KviOption_stringPreferredTorrentClient))
				KviTorrentInterface::select(d->instance());
		}
	}

	return true;
}

static bool torrent_module_cleanup(KviModule * )
{
	delete g_pDescriptorList;
	return true;
}

static bool torrent_module_can_unload(KviModule * )
{
	return true;
}

static bool torrent_module_ctrl(KviModule *,const char *,void *) // KviModule * m,const char * operation,void * param)
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
	"Torrent",
	"4.0.0",
	"Copyright (C) 2007 Alexander Stillich (torque at pltn dot org)" \
	"              2008 Fabio Bas (ctrlaltca at gmail dot com)",
	"Interface to various torrent clients",
	torrent_module_init,
	torrent_module_can_unload,
	torrent_module_ctrl,
	torrent_module_cleanup,
	"torrent"
)
