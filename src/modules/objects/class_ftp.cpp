//=============================================================================
//
//   File : class_ftp.cpp
//   Creation date : Mon Sep 1 08:13:45 2008 GMT by Carbone Alesssandro
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2008 Alessandro Carbone (elfonol at gmail dot com)
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
		!fn: $dataAvailableEvent(<data_length>)
		This function is called when some data is available to be read: the <data_length> parameter specifies
		the length of the available data in bytes.[br]
		You can use one of the $read* functions to obtain the data.

		!fn: $connect(<host:string>,<remote_port:integer>)
		Connects to the FTP server host using port port.The command is scheduled, and its execution is performed asynchronously.
		The function returns a unique identifier which is passed by commandStarted() and commandFinished().[br]
		!fn: $abort()
		!fn: $login
		!fn: $get
		!fn: $cd
		!fn: $list
		!fn:
		*/

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_ftp,"ftp","object")
	KVSO_REGISTER_HANDLER(KviKvsObject_ftp,"connect",functionConnect)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_ftp,abort)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_ftp,login)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_ftp,get)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_ftp,cd)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_ftp,list)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_ftp,commandFinishedEvent)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_ftp,listInfoEvent)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_ftp,dataTransferProgressEvent)
KVSO_END_REGISTERCLASS(KviKvsObject_ftp)


KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_ftp,KviKvsObject)
	m_pFtp = new QFtp();
	m_pFile=0;
	connect(m_pFtp,SIGNAL(commandFinished(int,bool)),this,SLOT(slotCommandFinished(int,bool)));
	connect(m_pFtp,SIGNAL(commandStarted(int)),this,SLOT(slotCommandStarted(int)));
	connect(m_pFtp,SIGNAL(dataTransferProgress(qint64,qint64)),this,SLOT(slotDataTransferProgress(qint64,qint64)));
	connect(m_pFtp,SIGNAL(done(bool)),this,SLOT(slotDone(bool)));
	connect(m_pFtp,SIGNAL(listInfo(const QUrlInfo)),this,SLOT(slotListInfo(const QUrlInfo)));
	connect(m_pFtp,SIGNAL(readyRead()),this,SLOT(slotReadyRead()));
	connect(m_pFtp,SIGNAL(stateChanged(int)),this,SLOT(slotStateChanged(int)));
KVSO_END_CONSTRUCTOR(KviKvsObject_ftp)

KVSO_BEGIN_DESTRUCTOR(KviKvsObject_ftp)
	if (m_pFile) delete m_pFile;
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
	if (m_pFile) delete m_pFile;
	m_pFile=new QFile(szDest);
	m_pFile->open(QIODevice::WriteOnly);
	int id=0;
	id=m_pFtp->get(szFile,m_pFile);
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
	m_pFtp->abort();
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
	if (m_pFtp->currentCommand()==QFtp::Get){
		status="Finished to load";
		m_pFile->close();
		delete m_pFile;
	}
	else if (m_pFtp->currentCommand()==QFtp:: ConnectToHost) status="connected";
	else if (m_pFtp->currentCommand()==QFtp:: Login) status="logged";
	else if (m_pFtp->currentCommand()==QFtp:: Cd) status="entered";
	else if (m_pFtp->currentCommand()==QFtp:: List) status="listCompleted";
	callFunction(this,"commandFinishedEvent",0,new KviKvsVariantList(new KviKvsVariant((kvs_int_t) id),
		new KviKvsVariant(status),new KviKvsVariant(error)));
}

void KviKvsObject_ftp::slotCommandStarted ( int )
{
}

void KviKvsObject_ftp::slotDataTransferProgress ( qint64 done, qint64 total )
{
	//FIXME qint64 to kvs_int_t cast
	callFunction(this,"dataTransferProgressEvent",0,new KviKvsVariantList(
		new KviKvsVariant((kvs_int_t) done),new KviKvsVariant((kvs_int_t) total)));
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
	callFunction(this,"listInfoEvent",0,new KviKvsVariantList(new KviKvsVariant(i.name())));

}
KVSO_CLASS_FUNCTION(ftp,listInfoEvent)
{
	emitSignal("listInfo",c,c->params());
	return true;
}

void KviKvsObject_ftp::slotReadyRead ()
{
}

void KviKvsObject_ftp::slotRawCommandReply (int replyCode, const QString & detail)
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
	callFunction(this,"stateChangedEvent",0,new KviKvsVariantList(
		new KviKvsVariant(szState)));
}
KVSO_CLASS_FUNCTION(ftp,stateChangedEvent)
{
	emitSignal("stateChanged",c,c->params());
	return true;
}
#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_ftp.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
