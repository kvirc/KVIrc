//=============================================================================
//
//   File : KvsObject_ftp.cpp
//   Creation date : Mon Sep 1 08:13:45 2008 GMT by Carbone Alesssandro
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2009 Alessandro Carbone (elfonol at gmail dot com)
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

#include "KvsObject_ftp.h"

#include "kvi_debug.h"
#include "KviError.h"

#include "KviLocale.h"

// Qt >= 5.0.0 has no QFtp.
// We use the external source code module provided by digia (in qtftp)
#include <qftp.h>

#include <QHashIterator>

/*
	@doc: ftp
	@keyterms:
		ftp object class
	@title:
		ftp class
	@type:
		class
	@short:
		An implementation of the client side of FTP protocol.
	@inherits:
		[class]object[/class]
	@description:
		This class provides a standard FTP functionality.[br]
	@functions:
		!fn:$dataAvailableEvent(<data_length>)
		This function is called when some data is available to be read.[br]
		The <data_length> parameter specifies the length of the available data in bytes.[br]
		You can use one of the $read* functions to obtain the data.
		!fn: <id:integer> $connect(<host:string>,<remote_port:integer>)
		Connects to the FTP server host using port port.[br]
		The command is scheduled, and its execution is performed asynchronously.[br]
		The function returns a unique identifier which is passed by [b]commandStarted()[/b] and [b]commandFinished()[/b].
		!fn:$abort()
		Aborts the current command and deletes all scheduled commands.
		!fn:<id:integer> $login(<user:string>,<pass:string>)
		Logs in to the FTP server with the username user and the password <pass>.[br]
		The function returns a unique identifier which is passed by [b]commandStarted()[/b] and [b]commandFinished()[/b].
		!fn:<id:integer> $get(<remotefile:string>,<localfile:string>)
		Downloads the <remotefile> file from the server.
		!fn:<id:integer> $put(<localfile:string>,<remotefile:string>)
		Uploads the <localfile> to the server.
		!fn:<id:integer> $cd(<remotedir:string>)
		Changes the working directory of the server to <remotedir>.
		!fn:<id:integer> $list()
		Lists the contents of the directory on the FTP server.
		!fn: $commandFinishedEvent(<id:integer>,<error:boolean>)
		Called by KVIrc when the FTP command identified by ID has finished.[br]
		Error is true if an error occurred during the processing; otherwise error is false.[br]
		The default implementation emits the [classfnc]$commandFinished[/classfnc]() signal.[br]
		!fn: $listInfoEvent(<dir_entry_name:string>)
		This event is triggered for each directory entry found by [classfnc]$list[/classfnc]() command.[br]
		The default implementation emits the [classfnc]$listInfo[/classfnc]() signal.
		!fn: $dataTransferProgressEvent(<done:integer>,<total:integer>)
		This event is triggered in response to a [b]get()[/b] or [b]put()[/b] request to indicate the current progress of the download or upload.[br]
		The default implementation emits the [classfnc]$dataTransferProgress[/classfnc]() signal.
	@signals:
		!sg: $commandFinished(<id:integer>,<szCommand:string>,<error:boolean>)
		This signal is emitted by the default implementation of [classfnc]$commandFinishedEvent[/classfnc]()
		!sg: $listInfo(<dir_entry_name:string>)
		This signal is emitted by the default implementation of [classfnc]$listInfoEvent[/classfnc]()
		!sg: $dataTransferProgress(<done:integer>,<total:integer>)
		This signal is emitted by the default implementation of [classfnc]$dataTransferProgressEvent[/classfnc]()
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_ftp, "ftp", "object")
KVSO_REGISTER_HANDLER(KvsObject_ftp, "connect", functionConnect)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_ftp, abort)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_ftp, close)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_ftp, login)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_ftp, get)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_ftp, put)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_ftp, cd)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_ftp, list)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_ftp, commandFinishedEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_ftp, listInfoEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_ftp, stateChangedEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_ftp, dataTransferProgressEvent)
KVSO_END_REGISTERCLASS(KvsObject_ftp)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_ftp, KviKvsObject)
m_pFtp = new QFtp();
connect(m_pFtp, SIGNAL(commandFinished(int, bool)), this, SLOT(slotCommandFinished(int, bool)));
connect(m_pFtp, SIGNAL(commandStarted(int)), this, SLOT(slotCommandStarted(int)));
connect(m_pFtp, SIGNAL(dataTransferProgress(qint64, qint64)), this, SLOT(slotDataTransferProgress(qint64, qint64)));
connect(m_pFtp, SIGNAL(done(bool)), this, SLOT(slotDone(bool)));
connect(m_pFtp, SIGNAL(listInfo(const QUrlInfo)), this, SLOT(slotListInfo(const QUrlInfo)));
connect(m_pFtp, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
connect(m_pFtp, SIGNAL(stateChanged(int)), this, SLOT(slotStateChanged(int)));
KVSO_END_CONSTRUCTOR(KvsObject_ftp)

KVSO_BEGIN_DESTRUCTOR(KvsObject_ftp)
delete m_pFtp;
KVSO_END_DESTRUCTOR(KvsObject_ftp)

KVSO_CLASS_FUNCTION(ftp, functionConnect)
{
	CHECK_INTERNAL_POINTER(m_pFtp)
	QString szHost;
	kvs_uint_t uRemotePort;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("host", KVS_PT_STRING, 0, szHost)
	KVSO_PARAMETER("remote_port", KVS_PT_UNSIGNEDINTEGER, KVS_PF_OPTIONAL, uRemotePort)
	KVSO_PARAMETERS_END(c)
	if(!uRemotePort)
		uRemotePort = 21;
	kvs_uint_t id = 0;
	id = m_pFtp->connectToHost(szHost, uRemotePort);
	c->returnValue()->setInteger(id);
	return true;
}

KVSO_CLASS_FUNCTION(ftp, login)
{
	CHECK_INTERNAL_POINTER(m_pFtp)
	QString szUser, szPass;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("user", KVS_PT_STRING, 0, szUser)
	KVSO_PARAMETER("password", KVS_PT_STRING, 0, szPass)
	KVSO_PARAMETERS_END(c)
	int id = m_pFtp->login(szUser, szPass);
	c->returnValue()->setInteger(id);
	return true;
}

KVSO_CLASS_FUNCTION(ftp, get)
{
	CHECK_INTERNAL_POINTER(m_pFtp)
	QString szFile, szDest;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("remote_filename", KVS_PT_STRING, 0, szFile)
	KVSO_PARAMETER("local_filename", KVS_PT_STRING, 0, szDest)
	KVSO_PARAMETERS_END(c)
	QFile * pFile;
	pFile = new QFile(szDest);
	pFile->open(QIODevice::WriteOnly);
	int id = m_pFtp->get(szFile, pFile);
	c->returnValue()->setInteger(id);
	return true;
}
KVSO_CLASS_FUNCTION(ftp, put)
{
	CHECK_INTERNAL_POINTER(m_pFtp)
	QString szRemoteFile, szLocaleFile;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("locale_filename", KVS_PT_STRING, 0, szLocaleFile)
	KVSO_PARAMETER("remote_filename", KVS_PT_STRING, 0, szRemoteFile)
	KVSO_PARAMETERS_END(c)
	QFile * pFile;
	pFile = new QFile(szLocaleFile);
	pFile->open(QIODevice::ReadOnly);
	int id = m_pFtp->put(pFile, szRemoteFile);
	c->returnValue()->setInteger(id);
	return true;
}
KVSO_CLASS_FUNCTION(ftp, cd)
{
	CHECK_INTERNAL_POINTER(m_pFtp)
	QString szPath;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("remote_filename", KVS_PT_STRING, 0, szPath)
	KVSO_PARAMETERS_END(c)
	int id = m_pFtp->cd(szPath);
	c->returnValue()->setInteger(id);
	return true;
}

KVSO_CLASS_FUNCTION(ftp, list)
{
	CHECK_INTERNAL_POINTER(m_pFtp)
	QString szPath;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("remote_dir", KVS_PT_STRING, 0, szPath)
	KVSO_PARAMETERS_END(c)
	int id = m_pFtp->list(szPath);
	c->returnValue()->setInteger(id);
	return true;
}

KVSO_CLASS_FUNCTION(ftp, abort)
{
	CHECK_INTERNAL_POINTER(m_pFtp)
	m_pFtp->abort();
	return true;
}

KVSO_CLASS_FUNCTION(ftp, close)
{
	CHECK_INTERNAL_POINTER(m_pFtp)
	m_pFtp->close();
	return true;
}

//signals & slots
KVSO_CLASS_FUNCTION(ftp, commandFinishedEvent)
{
	emitSignal("commandFinished", c, c->params());
	return true;
}

void KvsObject_ftp::slotCommandFinished(int iId, bool bError)
{
	QString szCommand = "";

	switch(m_pFtp->currentCommand())
	{
		case QFtp::None:
			szCommand = "none";
			break;
		case QFtp::SetTransferMode:
			szCommand = "setTransferMode";
			break;
		case QFtp::SetProxy:
			szCommand = "setProxy";
			break;
		case QFtp::ConnectToHost:
			szCommand = "connectToHost";
			break;
		case QFtp::Login:
			szCommand = "login";
			break;
		case QFtp::Close:
			szCommand = "close";
			break;
		case QFtp::List:
			szCommand = "list";
			break;
		case QFtp::Cd:
			szCommand = "cd";
			break;
		case QFtp::Get:
		{
			delete m_pFtp->currentDevice();
			szCommand = "get";
			break;
		}
		case QFtp::Put:
		{
			delete m_pFtp->currentDevice();
			szCommand = "put";
			break;
		}
		case QFtp::Remove:
			szCommand = "remove";
			break;
		case QFtp::Mkdir:
			szCommand = "mkdir";
			break;
		case QFtp::Rmdir:
			szCommand = "rmdir";
			break;
		case QFtp::Rename:
			szCommand = "rename";
			break;
		case QFtp::RawCommand:
			szCommand = "rawCommand";
			break;
	}

	KviKvsVariantList lParams;
	lParams.append(new KviKvsVariant((kvs_int_t)iId));
	lParams.append(new KviKvsVariant(szCommand));
	lParams.append(new KviKvsVariant(bError));
	if(bError)
		lParams.append(new KviKvsVariant(m_pFtp->errorString()));
	callFunction(this, "commandFinishedEvent", nullptr, &lParams);
}

void KvsObject_ftp::slotCommandStarted(int)
{
}

void KvsObject_ftp::slotDataTransferProgress(qint64 done, qint64 total)
{
	KviKvsVariantList lParams;
	lParams.append(new KviKvsVariant((kvs_int_t)done));
	lParams.append(new KviKvsVariant((kvs_int_t)total));
	callFunction(this, "dataTransferProgressEvent", nullptr, &lParams);
}

KVSO_CLASS_FUNCTION(ftp, dataTransferProgressEvent)
{
	emitSignal("dataTransferProgress", c, c->params());
	return true;
}

void KvsObject_ftp::slotDone(bool)
{
}

void KvsObject_ftp::slotListInfo(const QUrlInfo & i)
{
	KviKvsVariantList lParams;
	lParams.append(new KviKvsVariant(i.name()));
	callFunction(this, "listInfoEvent", nullptr, &lParams);
}

KVSO_CLASS_FUNCTION(ftp, listInfoEvent)
{
	emitSignal("listInfo", c, c->params());
	return true;
}

void KvsObject_ftp::slotReadyRead()
{
}

void KvsObject_ftp::slotRawCommandReply(int, const QString &)
{
}

void KvsObject_ftp::slotStateChanged(int state)
{
	QString szState = "";
	if(state == QFtp::Unconnected)
		szState = "Unconnected";
	else if(state == QFtp::HostLookup)
		szState = "HostLookup";
	else if(state == QFtp::Connecting)
		szState = "Connecting";
	else if(state == QFtp::Connected)
		szState = "Connected";
	else if(state == QFtp::LoggedIn)
		szState = "LoggedIn";
	else if(state == QFtp::Closing)
		szState = "Closing";
	KviKvsVariantList lParams;
	lParams.append(new KviKvsVariant(szState));
	callFunction(this, "stateChangedEvent", nullptr, &lParams);
}

KVSO_CLASS_FUNCTION(ftp, stateChangedEvent)
{
	emitSignal("stateChanged", c, c->params());
	return true;
}
