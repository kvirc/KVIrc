//=============================================================================
//
//   File : class_http.cpp
//   Creation date : Fry Sep 5 18:13:45 2008 GMT by Carbone Alesssandro
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "kvi_debug.h"
#include "kvi_error.h"
#include "kvi_locale.h"
#include "class_http.h"
#include <QHttp>
#include <QUrl>
#ifndef QT_NO_OPENSSL

#include <QSslError>

const char * const ssl_errors_tbl[] = {
		"NoError",	
		"UnableToGetIssuerCertificate",
		"UnableToDecryptCertificateSignature",
		"UnableToDecodeIssuerPublicKey",
		"CertificateSignatureFailed",
		"CertificateNotYetValid",
		"CertificateExpired",
		"InvalidNotBeforeField",
		"InvalidNotAfterField",
		"SelfSignedCertificate",
		"SelfSignedCertificateInChain",	
		"UnableToGetLocalIssuerCertificate",	
		"UnableToVerifyFirstCertificate",
		"CertificateRevoked",
		"InvalidCaCertificate",	
		"PathLengthExceeded",
		"InvalidPurpose",	
		"CertificateUntrusted",	
		"CertificateRejected",	
		"SubjectIssuerMismatch",	
		"AuthorityIssuerSerialNumberMismatch",
		"NoPeerCertificate",	
		"HostNameMismatch",	
		"UnspecifiedError",
		"NoSslSupport"
	};

#endif
/*
	@doc: http
	@keyterms:
		http object class
	@title:
		http class
	@type:
		class
	@short:
		An implementation of the client side of HTTP protocol.
	@inherits:
		[class]object[/class]
	@description:
		This class provides a standard HTTP functionality.[br]
	@functions:
		!fn: <id:integer>$get(<remote_path:string>,<local_file: string>)
		!fn: <id:integer>$post(<remote_path:string>,<post_data:string>,<local_file: string>)
		!fn: $abort()
		!fn: <id:integer>$setHost(<host:string>,<remote_port:unsigned_integer>)
		!fn: $setProxy(<host:string>,[<user:string>,<password:string>])
		!fn: $setUser(<user:string>,<password:string>)
		!fn: $readAll()
		!fn: <error:string> $errorString()
		!fn: $ignoreSslErrors()
		!fn: $requestFinishedEvent(<id:integer>,<error:boolean>)
		!fn: $requestStartedEvent(<id:integer>,<error:boolean>)
		!fn: $responseHeaderReceivedEvent(
		!fn: $doneEvent(<id:integer>,<error:boolean>)
		!fn: $dataReadProgressEvent(<done:integer>,<total:integer>)
		!fn: $dataSendProgressEvent(<done:integer>,<total:integer>)
		!fn: $stateChangedEvent(<connection_state:string>)
		!fn: $responseHeaderReceived(
		!fn: $readyReadEvent()
		!fn: $sslErrorsEvent(
	@signals:
		!sg: $requestStarted(
		!sg: $requestFinished(
		!sg: $dataSendProgress(
		!sg: $dataReadProgress(
		!sg: $done(
		!sg: $readyRead(
		!sg: $sslErrors(
		!sg: $stateChanged(
		!sg: $responseHeaderReceived(
		

		*/

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_http,"http","object")
	KVSO_REGISTER_HANDLER(KviKvsObject_http,"get",functionGet)
	KVSO_REGISTER_HANDLER(KviKvsObject_http,"post",functionPost)
	KVSO_REGISTER_HANDLER(KviKvsObject_http,"abort",functionAbort)
	KVSO_REGISTER_HANDLER(KviKvsObject_http,"setHost",functionSetHost)
	KVSO_REGISTER_HANDLER(KviKvsObject_http,"setProxy",functionSetProxy)
	KVSO_REGISTER_HANDLER(KviKvsObject_http,"setUser",functionSetUser)
	KVSO_REGISTER_HANDLER(KviKvsObject_http,"readAll",functionReadAll)
	KVSO_REGISTER_HANDLER(KviKvsObject_http,"errorString",functionErrorString)
	KVSO_REGISTER_HANDLER(KviKvsObject_http,"setFollowRedirect",functionFollowRedirect)
	
	#ifndef QT_NO_OPENSSL
	KVSO_REGISTER_HANDLER(KviKvsObject_http,"ignoreSSlErrors",functionIgnoreSslErrors)
	#endif
	// events
	KVSO_REGISTER_HANDLER(KviKvsObject_http,"doneEvent",functionDoneEvent)
	KVSO_REGISTER_HANDLER(KviKvsObject_http,"requestFinishedEvent",functionRequestFinishedEvent)
	KVSO_REGISTER_HANDLER(KviKvsObject_http,"requestStartedEvent",functionRequestStartedEvent)
	KVSO_REGISTER_HANDLER(KviKvsObject_http,"responseHeaderReceivedEvent",functionResponseHeaderReceivedEvent)
	KVSO_REGISTER_HANDLER(KviKvsObject_http,"dataReadProgressEvent",functionDataReadProgressEvent)
	KVSO_REGISTER_HANDLER(KviKvsObject_http,"dataSendProgressEvent",functionDataSendProgressEvent)
	KVSO_REGISTER_HANDLER(KviKvsObject_http,"stateChangedEvent",functionStateChangedEvent)
	KVSO_REGISTER_HANDLER(KviKvsObject_http,"readyReadEvent",functionReadyReadEvent)
	 #ifndef QT_NO_OPENSSL

	KVSO_REGISTER_HANDLER(KviKvsObject_http,"sslErrorsEvent",functionSslErrorsEvent)
#endif
KVSO_END_REGISTERCLASS(KviKvsObject_http)


KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_http,KviKvsObject)
	m_pHttp = new QHttp();
	m_bAbort=false;
	m_bEnableForceRedirect=false;
	connect(m_pHttp,SIGNAL(requestFinished(int,bool)),this,SLOT(slotRequestFinished(int,bool)));
	connect(m_pHttp,SIGNAL(done(bool)),this,SLOT(slotDone(bool)));
	
	connect(m_pHttp,SIGNAL(requestStarted(int)),this,SLOT(slotRequestStarted(int)));
	connect(m_pHttp,SIGNAL(dataSendProgress(int,int)),this,SLOT(slotDataSendProgress(int,int)));
	connect(m_pHttp,SIGNAL(dataReadProgress(int,int)),this,SLOT(slotDataReadProgress(int,int)));
	connect(m_pHttp,SIGNAL(responseHeaderReceived ( const QHttpResponseHeader & )),this,SLOT(slotResponseHeaderReceived(const QHttpResponseHeader &)));
	connect(m_pHttp,SIGNAL(readyRead ( const QHttpResponseHeader & )),this,SLOT(slotReadyRead(const QHttpResponseHeader &)));
	#ifndef QT_NO_OPENSSL
	connect(m_pHttp,SIGNAL(sslErrors ( const QList<QSslError> & )),this,SLOT(slotSslErrors(const QList<QSslError> & )));
	#endif
	connect(m_pHttp,SIGNAL(stateChanged(int)),this,SLOT(slotStateChanged(int)));

KVSO_END_CONSTRUCTOR(KviKvsObject_http)

KVSO_BEGIN_DESTRUCTOR(KviKvsObject_http)
	QHashIterator<int,QFile *> t(getDict);
/*

	while (t.hasNext()) 
	{
		t.next();
		int key=t.key();
		QFile *pFile=getDict.value(key);
		pFile->close();
		delete pFile;
	}
*/
	delete m_pHttp;
	getDict.clear();

KVSO_END_DESTRUCTOR(KviKvsObject_http)
//----------------------


bool  KviKvsObject_http::functionSetHost(KviKvsObjectFunctionCall *c)
{
	QString szHost;
	QString szConnectionType;
	kvs_uint_t uRemotePort;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("host",KVS_PT_STRING,0,szHost)
		KVSO_PARAMETER("remote_port",KVS_PT_UNSIGNEDINTEGER,KVS_PF_OPTIONAL,uRemotePort)
	KVSO_PARAMETERS_END(c)
	QUrl url(szHost);

	if (!url.isValid())
	{
		c->warning(__tr2qs("Host '%Q' is not a valid url"),&szHost);
		return true;
	}
	if (!szHost.isEmpty() && url.host().isEmpty()) url.setHost(szHost);
	if (!uRemotePort) uRemotePort=80;
	kvs_uint_t id=0;
	QHttp::ConnectionMode mode;
	if(url.scheme().toLower()=="https")  mode=QHttp::ConnectionModeHttps;
	else {mode=QHttp::ConnectionModeHttp;url.setScheme("http");}
	if (mode==QHttp::ConnectionModeHttps) uRemotePort=443;
	if (m_pHttp) id=m_pHttp->setHost(url.host(), mode, uRemotePort);
	c->returnValue()->setInteger(id);
	return true;
}
bool  KviKvsObject_http::functionSetUser(KviKvsObjectFunctionCall *c)
{
	QString szUser;
	QString szPass;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("user",KVS_PT_STRING,0,szUser)
		KVSO_PARAMETER("password",KVS_PT_STRING,0,szPass)
	KVSO_PARAMETERS_END(c)
	kvs_int_t id;
	if (m_pHttp) id=m_pHttp->setUser(szUser,szPass);
	return true;
}
bool  KviKvsObject_http::functionSetProxy(KviKvsObjectFunctionCall *c)
{
	QString szHost;
	QString szUser,szPass;
	kvs_uint_t uRemotePort;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("host",KVS_PT_STRING,0,szHost)
		KVSO_PARAMETER("remote_port",KVS_PT_UNSIGNEDINTEGER,KVS_PF_OPTIONAL,uRemotePort)
		KVSO_PARAMETER("user",KVS_PT_STRING,KVS_PF_OPTIONAL,szUser)
		KVSO_PARAMETER("pass",KVS_PT_STRING,KVS_PF_OPTIONAL,szPass)
	KVSO_PARAMETERS_END(c)
	if (m_pHttp) m_pHttp->setProxy(szHost,uRemotePort,szUser,szPass);
	return true;
}
bool  KviKvsObject_http::functionGet(KviKvsObjectFunctionCall *c)
{
	QString szPath,szDest;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("remote_path",KVS_PT_STRING,0,szPath)
		KVSO_PARAMETER("local_filename",KVS_PT_STRING,0,szDest)
	KVSO_PARAMETERS_END(c)
	QFile *pFile=0;
	if (!szDest.isEmpty()){
		pFile=new QFile(szDest);
		if (pFile){
				pFile->open(QIODevice::WriteOnly);
		}
	}
	int id=0;
	if (szPath.isEmpty()) szPath="/";
	if (m_pHttp)
	{
		id=m_pHttp->get(szPath,pFile);
		if (pFile) getDict[id]=pFile;
		c->returnValue()->setInteger(id);
	}
	return true;
}
bool  KviKvsObject_http::functionPost(KviKvsObjectFunctionCall *c)
{
	QString szPath,szDest,szData;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("remote_path",KVS_PT_STRING,0,szPath)
		KVSO_PARAMETER("post_data",KVS_PT_STRING,0,szData)
		KVSO_PARAMETER("local_filename",KVS_PT_STRING,0,szDest)
	KVSO_PARAMETERS_END(c)
	QFile *pFile=0;
	if (!szDest.isEmpty()){
		pFile=new QFile(szDest);
		if (pFile){
				pFile->open(QIODevice::WriteOnly);
		}
	}
	int id=0;
	if (szPath.isEmpty()) szPath="/";
	if (m_pHttp)
	{
		id=m_pHttp->post(szPath,szDest.toAscii(),pFile);
		if (pFile) getDict[id]=pFile;
		c->returnValue()->setInteger(id);
	}
	return true;
}
bool  KviKvsObject_http::functionAbort(KviKvsObjectFunctionCall *c)
{
	m_bAbort=true;
	if (m_pHttp) m_pHttp->abort();
	return true;
}
bool  KviKvsObject_http::functionReadAll(KviKvsObjectFunctionCall *c)
{
	if (m_pHttp) c->returnValue()->setString(m_pHttp->readAll());
	return true;
}
bool  KviKvsObject_http::functionErrorString(KviKvsObjectFunctionCall *c)
{
	if (m_pHttp) c->returnValue()->setString(m_pHttp->errorString());
	return true;
}
bool KviKvsObject_http::functionFollowRedirect(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("enable",KVS_PT_BOOLEAN,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	m_bEnableForceRedirect=bEnabled;
	return true;
}
//signals & slots 

bool KviKvsObject_http::functionRequestFinishedEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("requestFinished",c,c->params());
	return true;
}

void KviKvsObject_http::slotRequestFinished ( int id, bool error )
{
	
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
	QFile *pFile=getDict.value(id);
	if (pFile)
	{
		QString name=pFile->fileName();
		pFile->close();
		getDict.remove(id);
		int res=m_pHttp->lastResponse().statusCode();
		if ((m_pHttp->lastResponse().statusCode()==301 || m_pHttp->lastResponse().statusCode()==302 || m_pHttp->lastResponse().statusCode()==307) && m_bEnableForceRedirect)
			redirect(name,m_pHttp->lastResponse());
		delete pFile;
	}
	callFunction(this,"requestFinishedEvent",0,new KviKvsVariantList(new KviKvsVariant((kvs_int_t) id),new KviKvsVariant(error)));

}

bool KviKvsObject_http::functionRequestStartedEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("requestStarted",c,c->params());
	return true;
}
void KviKvsObject_http::slotRequestStarted ( int id )
{
	callFunction(this,"requestStartedEvent",0,new KviKvsVariantList(new KviKvsVariant((kvs_int_t) id)));
}

void KviKvsObject_http::slotDataReadProgress ( int done,int total )
{
	callFunction(this,"dataReadProgressEvent",0,new KviKvsVariantList(
		new KviKvsVariant((kvs_int_t)done),new KviKvsVariant((kvs_int_t)total)));
}
bool KviKvsObject_http::functionDataReadProgressEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("dataReadProgress",c,c->params());
	return true;
}
void KviKvsObject_http::slotDataSendProgress ( int done,int total )
{
	callFunction(this,"dataSendProgressEvent",0,new KviKvsVariantList(
		new KviKvsVariant((kvs_int_t)done),new KviKvsVariant((kvs_int_t)total)));
}
bool KviKvsObject_http::functionDataSendProgressEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("dataSendProgress",c,c->params());
	return true;
}
bool KviKvsObject_http::functionDoneEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("done",c,c->params());
	return true;
}
void KviKvsObject_http::slotDone ( bool error )
{
	callFunction(this,"doneEvent",0,new KviKvsVariantList(new KviKvsVariant(error)));

}

bool KviKvsObject_http::functionResponseHeaderReceivedEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("responseHeaderReceived",c,c->params());
	return true;
}
bool KviKvsObject_http::functionReadyReadEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("readyRead",c,c->params());
	return true;
}
void KviKvsObject_http::redirect(QString &file,const QHttpResponseHeader &r)
{
	QUrl url(r.value("location"));
	m_pHttp->setHost(url.host());
	QFile *pFile=0;
	pFile=new QFile(file);
	pFile->open(QIODevice::WriteOnly);
	int id=m_pHttp->get(url.path(),pFile);
	getDict[id]=pFile;
	debug("exit redirect");
}

void KviKvsObject_http::slotResponseHeaderReceived(const QHttpResponseHeader &r)
{
	QString szResponse;
	switch (r.statusCode()) {
		case 200:	szResponse="Ok"  ;break;
		case 301:	szResponse="Moved Permanently" ;break;
		case 302:	szResponse="Found" ;break;
		case 303:	szResponse="See Other" ;break;
		case 307:	szResponse="Temporary Redirect" ;break;
		default: szResponse=r.reasonPhrase();
			m_bAbort=true;
	}
	
	callFunction(this,"responseHeaderReceivedEvent",0,new KviKvsVariantList(
		new KviKvsVariant(szResponse)));
	
}
void KviKvsObject_http::slotReadyRead(const QHttpResponseHeader &r)
{
	QString szResponse;
	switch (r.statusCode()) {
		case 200:	szResponse="Ok"  ;break;
		case 301:	szResponse="Moved Permanently" ;break;
		case 302:	szResponse="Found" ;break;
		case 303:	szResponse="See Other" ;break;
		case 307:	szResponse="Temporary Redirect" ;break;
		default: szResponse=r.reasonPhrase();
			m_bAbort=true;
	}
	callFunction(this,"readyReadEvent",0,new KviKvsVariantList(
		new KviKvsVariant(szResponse)));
}




void KviKvsObject_http::slotStateChanged ( int state)
{
	QString szState="";
	if (state==QHttp::Unconnected) szState="Unconnected";
	else if (state==QHttp::HostLookup) szState="HostLookup";
	else if (state==QHttp::Connecting) szState="Connecting";
	else if (state==QHttp::Connected) szState="Connected";
	else if (state==QHttp::Sending) szState="Sending";
	else if (state==QHttp::Reading) szState="Reading";

	else if (state==QHttp::Closing) szState="Closing";
	callFunction(this,"stateChangedEvent",0,new KviKvsVariantList(
		new KviKvsVariant(szState)));
}
bool KviKvsObject_http::functionStateChangedEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("stateChanged",c,c->params());
	return true;
}

#ifndef QT_NO_OPENSSL

bool  KviKvsObject_http::functionIgnoreSslErrors(KviKvsObjectFunctionCall *c)
{
	if (m_pHttp) m_pHttp->ignoreSslErrors();
	return true;
}
void KviKvsObject_http::slotSslErrors(QList<QSslError> sslerrors)
{
	KviKvsArray *pArray=0;
	pArray=new KviKvsArray();
	for(int i=0;i<sslerrors.count();i++)
	{
		pArray->set(i,new KviKvsVariant(ssl_errors_tbl[sslerrors.at(i).error()]));
	}
	callFunction(this,"sslErrorsEvent",0,new KviKvsVariantList(new KviKvsVariant(pArray)));
}

bool KviKvsObject_http::functionSslErrorsEvent(KviKvsObjectFunctionCall *c)
{

	emitSignal("sslErrors",c,c->params());
	return true;
}
#endif
#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_http.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
