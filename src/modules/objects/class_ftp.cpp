//=============================================================================
//
//   File : class_ftp.cpp
//   Creation date : Mon Sep 1 08:13:45 2008 GMT by Carbone Alesssandro
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2009 Alessandro Carbone (elfonol at gmail dot com)
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

#include "kvi_debug.h"
#include "kvi_error.h"
#include "class_ftp.h"
#include "kvi_locale.h"

#include <QFtp>
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
		This function is called when some data is available to be read: the <data_length> parameter specifies
		the length of the available data in bytes.[br]
		You can use one of the $read* functions to obtain the data.
                !fn: <id:integer> $connect(<host:string>,<remote_port:integer>)
		Connects to the FTP server host using port port.The command is scheduled, and its execution is performed asynchronously.
		The function returns a unique identifier which is passed by commandStarted() and commandFinished().[br]
		!fn:$abort()
		Aborts the current command and deletes all scheduled commands.
		!fn:<id:integer> $login(<user:string>,<pass:string>)
		Logs in to the FTP server with the username user and the password <pass>.
		The function returns a unique identifier which is passed by commandStarted() and commandFinished().[br]
		!fn:<id:integer> $get(<remotefile:string>,<localfile:string>)
		Downloads the <remotefile> file from the server.
		!fn:<id:integer> $cd(<remotedir:string>)
		Changes the working directory of the server to <remotedir>.
		!fn:<id:integer> $list()
		Lists the contents of directory dir on the FTP server.
		!fn: $commandFinishedEvent(<id:integer>,<error:boolean>)
		Called by KVIrc when the ftp command identified by id has finished.
		error is true if an error occurred during the processing; otherwise error is false.
		The default implementation emits the [classfnc]$commandFinished[/classfnc]() signal.
		!fn: $listInfoEvent(<dir_entry_name:string>)
		This event is triggered for each directory entry found by [classfnc]$list[/classfnc]() command.
		The default implementation emits the [classfnc]$listInfo[/classfnc]() signal.
		!fn: $dataTransferProgressEvent(<done:integer>,<total:integer>)
		This event is triggered in response to a  [classfnc]get[/classfnc]() or  [classfnc]put[/classfnc]() request to indicate the current progress of the download or upload.
		The default implementation emits the [classfnc]$dataTransferProgress[/classfnc]() signal.
	@signals:
		!sg: $commandFinished(<id:integer>,<status:string>,<error:boolean>)
		This signal is emitted by the default implementation of [classfnc]$commandFinishedEvent[/classfnc]()
		!sg: $listInfo(<dir_entry_name:string>)
		This signal is emitted by the default implementation of [classfnc]$listInfoEvent[/classfnc]()
		!sg: $dataTransferProgress(<done:integer>,<total:integer>)
		This signal is emitted by the default implementation of [classfnc]$dataTransferProgressEvent[/classfnc]()
*/

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_ftp,"ftp","object")
	KVSO_REGISTER_HANDLER(KviKvsObject_ftp,"connect",functionConnect)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_ftp,abort)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_ftp,close)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_ftp,close)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_ftp,login)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_ftp,get)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_ftp,cd)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_ftp,list)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_ftp,commandFinishedEvent)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_ftp,listInfoEvent)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_ftp,stateChangedEvent)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_ftp,dataTransferProgressEvent)
KVSO_END_REGISTERCLASS(KviKvsObject_ftp)


KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_ftp,KviKvsObject)
	m_pFtp = new QFtp();
        m_bAbort=false;
	connect(m_pFtp,SIGNAL(commandFinished(int,bool)),this,SLOT(slotCommandFinished(int,bool)));
	connect(m_pFtp,SIGNAL(commandStarted(int)),this,SLOT(slotCommandStarted(int)));
	connect(m_pFtp,SIGNAL(dataTransferProgress(qint64,qint64)),this,SLOT(slotDataTransferProgress(qint64,qint64)));
	connect(m_pFtp,SIGNAL(done(bool)),this,SLOT(slotDone(bool)));
	connect(m_pFtp,SIGNAL(listInfo(const QUrlInfo)),this,SLOT(slotListInfo(const QUrlInfo)));
	connect(m_pFtp,SIGNAL(readyRead()),this,SLOT(slotReadyRead()));
	connect(m_pFtp,SIGNAL(stateChanged(int)),this,SLOT(slotStateChanged(int)));
KVSO_END_CONSTRUCTOR(KviKvsObject_ftp)

KVSO_BEGIN_DESTRUCTOR(KviKvsObject_ftp)
	QHashIterator<int,QFile *> t(getDict);
	while (t.hasNext())
	{
		t.next();
		int key=t.key();
		QFile *pFile=getDict.value(key);
		pFile->close();
		delete pFile;
	}
	getDict.clear();
	delete m_pFtp;
KVSO_END_DESTRUCTOR(KviKvsObject_ftp)
//----------------------



KVSO_CLASS_FUNCTION(ftp,functionConnect)
{
	CHECK_INTERNAL_POINTER(m_pFtp)
	QString szHost;
	kvs_uint_t uRemotePort;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("host",KVS_PT_STRING,0,szHost)
	KVSO_PARAMETER("remote_port",KVS_PT_UNSIGNEDINTEGER,KVS_PF_OPTIONAL,uRemotePort)
	KVSO_PARAMETERS_END(c)
	if (!uRemotePort) uRemotePort=21;
	kvs_uint_t id=0;
	id=m_pFtp->connectToHost(szHost, uRemotePort);
	c->returnValue()->setInteger(id);
	return true;
}

KVSO_CLASS_FUNCTION(ftp,login)
{
	CHECK_INTERNAL_POINTER(m_pFtp)
	QString szUser,szPass;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("user",KVS_PT_STRING,0,szUser)
	KVSO_PARAMETER("password",KVS_PT_STRING,0,szPass)
	KVSO_PARAMETERS_END(c)
	int id=m_pFtp->login(szUser, szPass);
	c->returnValue()->setInteger(id);
	return true;
}

KVSO_CLASS_FUNCTION(ftp,get)
{
	CHECK_INTERNAL_POINTER(m_pFtp)
	QString szFile,szDest;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("remote_filename",KVS_PT_STRING,0,szFile)
	KVSO_PARAMETER("local_filename",KVS_PT_STRING,0,szDest)
	KVSO_PARAMETERS_END(c)
	QFile *pFile;
	pFile=new QFile(szDest);
	pFile->open(QIODevice::WriteOnly);
	int id=0;
	id=m_pFtp->get(szFile,pFile);
	getDict[id]=pFile;
	c->returnValue()->setInteger(id);
	return true;
}

KVSO_CLASS_FUNCTION(ftp,cd)
{
	CHECK_INTERNAL_POINTER(m_pFtp)
	QString szPath;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("remote_filename",KVS_PT_STRING,0,szPath)
	KVSO_PARAMETERS_END(c)
	int id=m_pFtp->cd(szPath);
	c->returnValue()->setInteger(id);
	return true;
}

KVSO_CLASS_FUNCTION(ftp,list)
{
	CHECK_INTERNAL_POINTER(m_pFtp)
	QString szPath;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("remote_dir",KVS_PT_STRING,0,szPath)
	KVSO_PARAMETERS_END(c)
	int id=m_pFtp->list(szPath);
	c->returnValue()->setInteger(id);
	return true;
}

KVSO_CLASS_FUNCTION(ftp,abort)
{
	CHECK_INTERNAL_POINTER(m_pFtp)
	m_bAbort=true;
	m_pFtp->abort();
	return true;
}

KVSO_CLASS_FUNCTION(ftp,close)
{
	CHECK_INTERNAL_POINTER(m_pFtp)
	m_bAbort=true;
	m_pFtp->close();
	return true;
}

//signals & slots
KVSO_CLASS_FUNCTION(ftp,commandFinishedEvent)
{
	emitSignal("commandFinished",c,c->params());
	return true;
}

void KviKvsObject_ftp::slotCommandFinished ( int id, bool error )
{
	QString status="";
	if (m_bAbort)
	{
		m_bAbort=false;
		QHashIterator<int,QFile *> t(getDict);
		while (t.hasNext())
		{
			t.next();
			int key=t.key();
			QFile *pFile=getDict.value(key);
			pFile->close();
			delete pFile;
		}
		getDict.clear();
		return;
	}
	if (m_pFtp->currentCommand()==QFtp::Get)
	{
		status="Downloaded";
		QFile *pFile=getDict.value(id);
		pFile->close();
		delete pFile;
		getDict.remove(id);
	}
	else if (m_pFtp->currentCommand()==QFtp:: ConnectToHost) status="connected";
	else if (m_pFtp->currentCommand()==QFtp:: Login) status="logged";
	else if (m_pFtp->currentCommand()==QFtp:: Cd) status="entered";
	else if (m_pFtp->currentCommand()==QFtp:: List) status="listCompleted";
	KviKvsVariantList lParams;
	lParams.append(new KviKvsVariant((kvs_int_t) id));
	lParams.append(new KviKvsVariant(status));
	lParams.append(new KviKvsVariant(error));
	callFunction(this,"commandFinishedEvent",0,&lParams);
}

void KviKvsObject_ftp::slotCommandStarted ( int )
{
}

void KviKvsObject_ftp::slotDataTransferProgress ( qint64 done, qint64 total )
{
	KviKvsVariantList lParams;
	lParams.append(new KviKvsVariant((kvs_int_t) done));
	lParams.append(new KviKvsVariant((kvs_int_t) total));
        callFunction(this,"dataTransferProgressEvent",0,&lParams);
}

KVSO_CLASS_FUNCTION(ftp,dataTransferProgressEvent)
{
	emitSignal("dataTransferProgress",c,c->params());
	return true;
}

void KviKvsObject_ftp::slotDone ( bool )
{
}

void KviKvsObject_ftp::slotListInfo ( const QUrlInfo & i )
{
	KviKvsVariantList lParams;
	lParams.append(new KviKvsVariant(i.name()));
	callFunction(this,"listInfoEvent",0,&lParams);
}

KVSO_CLASS_FUNCTION(ftp,listInfoEvent)
{
	emitSignal("listInfo",c,c->params());
	return true;
}

void KviKvsObject_ftp::slotReadyRead ()
{
}

void KviKvsObject_ftp::slotRawCommandReply (int , const QString & )
{
}

void KviKvsObject_ftp::slotStateChanged (int state)
{
	QString szState="";
	if (state==QFtp::Unconnected) szState="Unconnected";
	else if (state==QFtp::HostLookup) szState="HostLookup";
	else if (state==QFtp::Connecting) szState="Connecting";
	else if (state==QFtp::Connected) szState="Connected";
	else if (state==QFtp::LoggedIn) szState="LoggedIn";
	else if (state==QFtp::Closing) szState="Closing";
	KviKvsVariantList lParams;
	lParams.append(new KviKvsVariant(szState));
	callFunction(this,"stateChangedEvent",0,&lParams);
}

KVSO_CLASS_FUNCTION(ftp,stateChangedEvent)
{
	emitSignal("stateChanged",c,c->params());
	return true;
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_ftp.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
