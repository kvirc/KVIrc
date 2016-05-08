//=============================================================================
//
//   File : libkvihttp.cpp
//   Creation date : Tue Apr 22 2003 02:00:12 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "HttpFileTransfer.h"

#include "KviModule.h"
#include "KviCString.h"
#include "KviApplication.h"
#include "KviLocale.h"
#include "KviFileDialog.h"
#include "KviWindow.h"
#include "KviError.h"
#include "KviCommandFormatter.h"
#include "KviMainWindow.h"

static bool http_kvs_complete_get(KviKvsModuleCommandCall * c, QString & szUrl, QString & szFileName, const QString & szCallback)
{
	if(szUrl.isEmpty())
	{
		c->warning(__tr2qs_ctx("No URL specified", "http"));
		return true;
	}

	KviUrl url(szUrl);

	QString tmp;

	if(szFileName.isEmpty())
	{
		if(c->switches()->find('a', "auto-file-name"))
		{
			tmp = szUrl;
			tmp.replace('/', "_");
			tmp.replace(':', "_");
			tmp.replace('@', "_");
			tmp.replace('?', "_");
			// http____path_path2_path3_filename.ext
			g_pApp->getLocalKvircDirectory(szFileName, KviApplication::Incoming, tmp);
		}
		else
		{
			if(!KviFileDialog::askForSaveFileName(
			       szFileName,
			       __tr2qs_ctx("Choose a filename to save", "http"),
			       QString(),
			       QString(),
			       false,
			       false,
			       true,
			       g_pMainWindow))
				return true;
			if(szFileName.isEmpty())
				return true;
		}
	}

	HttpFileTransfer * hft = new HttpFileTransfer();

	bool bHead = c->switches()->find('h', "head");

	if(c->switches()->getAsStringIfExisting('p', "post-data", tmp))
	{
		if(bHead)
		{
			c->warning(__tr2qs_ctx("The switch -p is incompatible with -h: -p takes precedence", "http"));
			bHead = false;
		}
		hft->request()->setPostData(tmp);
	}

	hft->request()->setUrl(url);
	hft->request()->setProcessingType(bHead ? KviHttpRequest::HeadersOnly : KviHttpRequest::StoreToFile);
	hft->request()->setFileName(szFileName);

	if(c->switches()->getAsStringIfExisting('e', "existing-file-action", tmp))
	{
		if(KviQString::equalCI(tmp, "e"))
			hft->request()->setExistingFileAction(KviHttpRequest::RenameExisting);
		else if(KviQString::equalCI(tmp, "i"))
			hft->request()->setExistingFileAction(KviHttpRequest::RenameIncoming);
		else if(KviQString::equalCI(tmp, "o"))
			hft->request()->setExistingFileAction(KviHttpRequest::Overwrite);
		else if(KviQString::equalCI(tmp, "r"))
			hft->request()->setExistingFileAction(KviHttpRequest::Resume);
	}

	// FIXME: this should be numeric
	if(c->switches()->getAsStringIfExisting('m', "max-len", tmp))
	{
		bool bOk;
		unsigned int uContentLength = tmp.toUInt(&bOk);
		if(bOk)
			hft->request()->setMaxContentLength(uContentLength);
	}

	// FIXME: this should be numeric
	if(c->switches()->getAsStringIfExisting('o', "offset", tmp))
	{
		bool bOk;
		unsigned int uContentOffset = tmp.toUInt(&bOk);
		if(bOk)
			hft->request()->setContentOffset(uContentOffset);
	}

	// FIXME: this should be numeric
	if(c->switches()->getAsStringIfExisting('t', "timeout", tmp))
	{
		bool bOk;
		unsigned int uConnectionTimeout = tmp.toUInt(&bOk);
		if(bOk)
			hft->request()->setConnectionTimeout(uConnectionTimeout);
	}

	if(c->switches()->getAsStringIfExisting('w', "winctrl", tmp))
	{
		if(!tmp.contains('h'))
			hft->invokeTransferWindow(tmp.contains('m'), tmp.contains('n'));
	}
	else
	{
		hft->invokeTransferWindow(false, false);
	}

	KviKvsVariant * v = c->switches()->find('i', "identifier");
	if(v)
		hft->setMagicIdentifier(*v);

	if(c->switches()->find('q', "quiet"))
		hft->setNotifyCompletion(false);

	if(c->switches()->find('y', "no-output"))
		hft->setNoOutput(true);

	if(!szCallback.isEmpty())
		hft->setCompletionCallback(szCallback);

	if(c->switches()->find('c', "clear"))
		hft->setAutoClean(true);

	if(!hft->startDownload())
	{
		tmp = hft->request()->lastError();
		c->warning(__tr2qs_ctx("Failed to start the get request: %Q", "http"), &tmp);
		delete hft;
	}

	return true;
}

/*
	@doc: http.get
	@type:
		command
	@title:
		http.get
	@keyterms:
		HTTP extension
	@short:
		Retrieves a file via HTTP GET
	@syntax:
		http.get [switches] <http_url> [save_file_name]
	@description:
		Attempts to download the file at <http_url> by using the HTTP GET or POST protocol.[br]
		If [save_file_name] is specified, then is is used as save file name, otherwise
		a save file dialog is displayed (unless -a is used).[br]
		The event OnHTTPGetTerminated is triggered upon the download completion (both
		in case of success or failure). If you want a callback command to be triggered
		instead please use [cmd]http.asyncGet[/cmd].
		If the URL contains a https:// prefix then a SSL connection will be used.
	@switches:
		!sw: -a=<auto_file_name> | --auto-file-name=<auto_file_name>
		Don't show the save file dialog but determine automatically a file name.
		The file is put in the KVIrc incoming directory and the file name
		is the processed URL.[br]
		[br]
		!sw: -e=<existing_file_action> | --existing-file-action=<existing_file_action>
		Specifies the action to be taken when the local file already exists.[br]
		The action can be one of "i","e","o" or "r".[br]
		"i" causes the incoming file to be automatically renamed by appending a
		non colliding suffix. "e" causes the existing file to be renamed
		by appending a non colliding suffix (the incoming file will then have its name preserved).
		"o" causes the existing file to be overwritten and "r" will attempt to resume
		a interrupted transfer.[br]
		The default is to rename the incoming file.[br]
		[br]
		!sw: -m=<max_content_length> | --max-len=<max_content_length>
		Causes content longer than <max_content_length> to be discarded.[br]
		This is mainly to prevent you from automatically downloading 300 MiB files
		and to prevent DOS attacks from malicious servers that do not report the Content-length header.[br]
		If the Content-length header is reported by the server then the transfer is aborted
		if the length exceeds <max_content_length>.[br]
		If the Content-length header is missing then the transfer is interrupted when
		the received data length exceeds <max_content_length>.[br]
		-m=0 means "accept any content length" (which is the default).[br]
		[br]
		!sw: -o=<content_offset> | --offset=<content_offset>
		Causes the download to start from position <content offset>.[br]
		This can be used to download only a part of the file starting at byte <content_offset>.[br]
		<content_offset> is used regardless if the file is resumed or not.
		Please note that you don't need to specify the content offset when using
		-e=r : the offset is automatically calculated. If you specify both -o=<content_offset>
		and -e=r then the file will be resumed, the transfer will start at the specified offset
		and the received stream will be appended to the existing file.(avoid it unless you know what you're doing:
		it's easy to download broken files).[br]
		[br]
		!sw: -t=<timeout_in_seconds> | --timeout=<timeout_in_seconds>
		Changes the default connection timeout to the <timeout_in_seconds>.
		A connection stuck for more than <timeout_in_seconds> seconds will be simply aborted.
		The default timeout is 60 seconds and is appropriate for most operations. Use with care.[br]
		[br]
		!sw: -h | --head
		Causes the connection to use the HTTP HEAD method that effectively
		does not transfer real data. The server sends only the response headers.
		This might be used in conjunction with the -v option to print the headers to the
		active window.[br]
		[br]
		!sw: -w=<flags> | --winctrl
		This switch controls the creation and visualization of the transfer window.
		<flags> can be any combination of 'm','n' and 'h'.
		The 'h' flag causes the window to not be created. The transfer will simply run in background.
		Note that with 'h' the user has no possibility to interact with the transfer.
		The 'm' flag causes the transfer window to be created as "minimized". 'm' does nothing
		if the window already exists. The 'n' flag causes the window to be [b]not[/b] activated (brought to top).
		[br]
		!sw: -i=<magic identifier> | --identifier=<magic identifier>
		This identifier is passed as $3 parameter to the [event:OnHTTPGetTerminated]OnHTTPGetTerminated[/event]
		when this transfer terminates. If this switch is not present then an empty string is used.
		With [cmd]http.asyncGet[/cmd] this parameter is passed to the callback command instead.
		[br]
		!sw: -p=<post data> | --post-data=<post data>
		The request is sent in form of a POST request. <post data> is the urlencoded payload of
		the request. -p is incompatible with -h.
		[br]
		!sw: -q | --quiet
		Do not notify download completion in the notifier window nor in the console.
		[br]
		!sw: -y | --no-output
		Suppress any output in the file transfer window. This will effectively disable
		the file transfer window highlighting (so the user will not be alerted by a failed
		download unless he's really watching the window). This is useful when you're notifying
		failures in some other way...
		[br]
		!sw: -c | --clear
		Automatically remove the transfer from the transfer list when terminated
	@seealso:
		[cmd]http.asyncGet[/cmd]
*/

static bool http_kvs_cmd_get(KviKvsModuleCommandCall * c)
{
	QString szUrl, szFileName;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("url", KVS_PT_NONEMPTYSTRING, 0, szUrl)
	KVSM_PARAMETER("filename", KVS_PT_STRING, KVS_PF_OPTIONAL, szFileName)
	KVSM_PARAMETERS_END(c)

	return http_kvs_complete_get(c, szUrl, szFileName, QString());
}
/*
	@doc: http.asyncGet
	@type:
		command
	@title:
		http.asyncGet
	@keyterms:
		HTTP extension
	@short:
		Retrieves a file via HTTP GET and triggers a callback
	@syntax:
		http.asyncGet [switches] (<http_url> [,save_file_name])
		{
			<callback command>
		}
	@description:
		Attempts to download the file at <http_url> by using the HTTP GET or POST protocol.[br]
		If [save_file_name] is specified, then is is used as save file name, otherwise
		a save file dialog is displayed (unless -a is used).[br]
		This command is really similar to [cmd]http.get[/cmd]: it has exactly the same
		parameters and switches (so also refer to its documentation).
		The only difference is that asyncGet triggers the <callback command> upon completion
		instead of the global OnHTTPGetTerminated event.
		The parameters passed to the callback are exactly the same.
		If the URL contains a https:// prefix then a SSL connection will be used.
	@seealso:
		[cmd]http.get[/cmd]
*/

static bool http_kvs_cmd_asyncGet(KviKvsModuleCallbackCommandCall * c)
{
	QString szUrl, szFileName;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("url", KVS_PT_NONEMPTYSTRING, 0, szUrl)
	KVSM_PARAMETER("filename", KVS_PT_STRING, KVS_PF_OPTIONAL, szFileName)
	KVSM_PARAMETERS_END(c)

	return http_kvs_complete_get(c, szUrl, szFileName, c->callback()->code());
}

static bool http_module_init(KviModule * m)
{
	HttpFileTransfer::init();

	KVSM_REGISTER_SIMPLE_COMMAND(m, "get", http_kvs_cmd_get);
	KVSM_REGISTER_CALLBACK_COMMAND(m, "asyncGet", http_kvs_cmd_asyncGet);

	return true;
}

static bool http_module_cleanup(KviModule *)
{
	HttpFileTransfer::done();
	return true;
}

static bool http_module_can_unload(KviModule *)
{
	return (HttpFileTransfer::runningTransfers() == 0);
}

KVIRC_MODULE(
    "Http",                                                         // module name
    "4.0.0",                                                        // module version
    "Copyright (C) 2003 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
    "HTTP interface for KVIrc",
    http_module_init,
    http_module_can_unload,
    0,
    http_module_cleanup,
    "http")
