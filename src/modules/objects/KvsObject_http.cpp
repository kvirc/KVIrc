//=============================================================================
//
//   File : KvsObject_http.cpp
//   Creation date : Fry Sep 5 18:13:45 2008 GMT by Carbone Alesssandro
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

#include "KvsObject_http.h"

#include "kvi_debug.h"
#include "KviError.h"
#include "KviLocale.h"

// Qt >= 5.0.0 has no QHttp.
// We use the external source code module provided by digia (in qthttp)
#include <qhttp.h>

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
		!fn: <id:integer>$get(<remote_path:string>,[<local_file: string>])
		Sends a get request for path to the server set by [classfnc]$setHost[/classfnc].
		If local_file is not specified [b]readyRead()[/b] signal is emitted every time new content data is available to read.
		!fn: <id:integer>$post(<remote_path:string>,<post_data:string>,<local_file: string>
		Sends a post request for path to the server set by [classfnc]$setHost[/classfnc].
		If local_file is not specified [b]readyRead()[/b] signal is emitted every time new content data is available to read.
		!fn: $abort()
		Aborts the current request and deletes all scheduled requests.
		!fn: <id:integer>$setHost(<host:string>,<remote_port:unsigned_integer>)
		Sets the HTTP server that is used for requests to hostName on port port.
		The function does not block: the request is scheduled, and its execution is performed asynchronously.
		The function returns a unique identifier which is passed by [b]requestStartedEvent()[/b] and [b]requestFinishedEvent()[/b].
		When the request is started the requestStarted() signal is emitted. When it is finished the requestFinished() signal is emitted.
		!fn: <id:integer>$currentId()
		Returns the identifier of the HTTP request being executed or [b]0[/b] if there is no request being executed.
		!fn: <id:integer> $setProxy(<host:string>,<port:integer>,[<user:string>,<password:string>])
		Enables HTTP proxy support, using the proxy server host on port port.
		Optionals parameters user and password can be provided if the proxy server requires authentication.
		!fn: <id:integer> $setUser(<user:string>,<password:string>)
		This function sets the user name userName and password password for web pages that require authentication.
		!fn: <string> $readAll()
		Reads all the bytes from the response content and returns them as string.
		!fn: <error:string> $errorString()
		Returns a human-readable description of the last error that occurred.
		!fn: $ignoreSslErrors()
		!fn: $requestFinishedEvent(<id:integer>,<error:boolean>)
		This event is triggered by KVIrc when the request identified by <id> has finished.
		!fn: $requestStartedEvent(<id:integer>,<error:boolean>)
		This event is triggered by KVIrc when the request identified by <id> has started.
		!fn: $responseHeaderReceivedEvent(<response:string>)
		This event is triggered by KVIrc when he HTTP header of a server response is available.
		The response is passed ad string.
		!fn: $doneEvent(<error:boolean>)
		This event is triggered when the last pending request has finished; (it is emitted after the last requests requestFinished() signal).
		!fn: $dataReadProgressEvent(<done:integer>,<total:integer>)
		This event is triggered when reads data from a HTTP server.
		<done> is the amount of data has already arrived and <total> is the total amount of data.
		!fn: $dataSendProgressEvent(<done:integer>,<total:integer>)
		This event is triggered when sends data to a HTTP server.
		<done> is the amount of data has already arrived and <total> is the total amount of data.
		!fn: $stateChangedEvent(<connection_state:string>)
		This event is triggered by the framework when the state of the connection changes.
		!fn: $readyReadEvent(<response:string>)
		This event is triggered by the framework when there is new data read and there is not file specified in [b]get[/b] or [b]post[/b].
		You can call [b]readAll[/b] to reads all the bytes from the response data.
		!fn: $sslErrorsEvent(<sslerrors:string>)
		Returns the list of errors that occurred during the SSL handshake.
		Unless you call ignoreSslErrors() from within a slot connected to the signal emitted by this event,
		the framework will tear down the connection immediately after emitting the signal.
	@signals:
		!sg: $requestStarted()
		This signal is emitted by the default implementation of [classfnc]$requestStartedEvent[/classfnc].
		!sg: $requestFinished()
		This signal is emitted by the default implementation of [classfnc]$requestFinishedEvent[/classfnc].
		!sg: $dataSendProgress()
		This signal is emitted by the default implementation of [classfnc]$dataSendProgressEvent[/classfnc].
		!sg: $dataReadProgress()
		This signal is emitted by the default implementation of [classfnc]$dataReadProgressEvent[/classfnc].
		!sg: $done()
		This signal is emitted by the default implementation of [classfnc]$doneEvent[/classfnc].
		!sg: $readyRead()
		This signal is emitted by the default implementation of [classfnc]$readyReadEvent[/classfnc].
		!sg: $sslErrors()
		This signal is emitted by the default implementation of [classfnc]$sslErrorsEvent[/classfnc].
		!sg: $stateChanged()
		This signal is emitted by the default implementation of [classfnc]$stateChangedEvent[/classfnc].
		!sg: $responseHeaderReceived()
		This signal is emitted by the default implementation of [classfnc]$responseHeaderReceivedEvent[/classfnc].
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_http, "http", "object")
KVSO_REGISTER_HANDLER(KvsObject_http, "get", functionGet)
KVSO_REGISTER_HANDLER(KvsObject_http, "post", functionPost)
KVSO_REGISTER_HANDLER(KvsObject_http, "abort", functionAbort)
KVSO_REGISTER_HANDLER(KvsObject_http, "setHost", functionSetHost)
KVSO_REGISTER_HANDLER(KvsObject_http, "setProxy", functionSetProxy)
KVSO_REGISTER_HANDLER(KvsObject_http, "currentId", functionCurrentId)
KVSO_REGISTER_HANDLER(KvsObject_http, "setUser", functionSetUser)
KVSO_REGISTER_HANDLER(KvsObject_http, "readAll", functionReadAll)
KVSO_REGISTER_HANDLER(KvsObject_http, "errorString", functionErrorString)
KVSO_REGISTER_HANDLER(KvsObject_http, "setFollowRedirect", functionFollowRedirect)

// events
KVSO_REGISTER_HANDLER(KvsObject_http, "doneEvent", functionDoneEvent)
KVSO_REGISTER_HANDLER(KvsObject_http, "requestFinishedEvent", functionRequestFinishedEvent)
KVSO_REGISTER_HANDLER(KvsObject_http, "requestStartedEvent", functionRequestStartedEvent)
KVSO_REGISTER_HANDLER(KvsObject_http, "responseHeaderReceivedEvent", functionResponseHeaderReceivedEvent)
KVSO_REGISTER_HANDLER(KvsObject_http, "dataReadProgressEvent", functionDataReadProgressEvent)
KVSO_REGISTER_HANDLER(KvsObject_http, "dataSendProgressEvent", functionDataSendProgressEvent)
KVSO_REGISTER_HANDLER(KvsObject_http, "stateChangedEvent", functionStateChangedEvent)
KVSO_REGISTER_HANDLER(KvsObject_http, "readyReadEvent", functionReadyReadEvent)
#ifndef QT_NO_OPENSSL
KVSO_REGISTER_HANDLER(KvsObject_http, "sslErrorsEvent", functionSslErrorsEvent)
KVSO_REGISTER_HANDLER(KvsObject_http, "ignoreSSlErrors", functionIgnoreSslErrors)
#endif
KVSO_END_REGISTERCLASS(KvsObject_http)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_http, KviKvsObject)
m_pHttp = new QHttp();
m_bAbort = false;
m_bEnableForceRedirect = false;
connect(m_pHttp, SIGNAL(requestFinished(int, bool)), this, SLOT(slotRequestFinished(int, bool)));
connect(m_pHttp, SIGNAL(done(bool)), this, SLOT(slotDone(bool)));

connect(m_pHttp, SIGNAL(requestStarted(int)), this, SLOT(slotRequestStarted(int)));
connect(m_pHttp, SIGNAL(dataSendProgress(int, int)), this, SLOT(slotDataSendProgress(int, int)));
connect(m_pHttp, SIGNAL(dataReadProgress(int, int)), this, SLOT(slotDataReadProgress(int, int)));
connect(m_pHttp, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)), this, SLOT(slotResponseHeaderReceived(const QHttpResponseHeader &)));
connect(m_pHttp, SIGNAL(readyRead(const QHttpResponseHeader &)), this, SLOT(slotReadyRead(const QHttpResponseHeader &)));
#ifndef QT_NO_OPENSSL
connect(m_pHttp, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(slotSslErrors(const QList<QSslError> &)));
#endif
connect(m_pHttp, SIGNAL(stateChanged(int)), this, SLOT(slotStateChanged(int)));

KVSO_END_CONSTRUCTOR(KvsObject_http)

KVSO_BEGIN_DESTRUCTOR(KvsObject_http)
QHashIterator<int, QFile *> t(getDict);
while(t.hasNext())
{
	t.next();
	int key = t.key();
	QFile * pFile = getDict.value(key);
	pFile->close();
	delete pFile;
}
getDict.clear();
delete m_pHttp;

KVSO_END_DESTRUCTOR(KvsObject_http)

bool KvsObject_http::functionSetHost(KviKvsObjectFunctionCall * c)
{
	CHECK_INTERNAL_POINTER(m_pHttp)
	QString szHost;
	QString szConnectionType;
	kvs_uint_t uRemotePort;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("host", KVS_PT_STRING, 0, szHost)
	KVSO_PARAMETER("remote_port", KVS_PT_UNSIGNEDINTEGER, KVS_PF_OPTIONAL, uRemotePort)
	KVSO_PARAMETERS_END(c)
	QUrl url(szHost);
	if(!url.isValid())
	{
		c->warning(__tr2qs_ctx("Host '%Q' is not a valid URL", "objects"), &szHost);
		return true;
	}
	if(!szHost.isEmpty() && url.host().isEmpty())
		url.setHost(szHost);
	if(!uRemotePort)
		uRemotePort = 80;
	kvs_uint_t id = 0;
	QHttp::ConnectionMode mode;
	if(url.scheme().toLower() == "https")
		mode = QHttp::ConnectionModeHttps;
	else
	{
		mode = QHttp::ConnectionModeHttp;
		url.setScheme("http");
	}
	if(mode == QHttp::ConnectionModeHttps)
		uRemotePort = 443;
	id = m_pHttp->setHost(url.host(), mode, uRemotePort);
	c->returnValue()->setInteger(id);
	return true;
}
bool KvsObject_http::functionCurrentId(KviKvsObjectFunctionCall * c)
{
	CHECK_INTERNAL_POINTER(m_pHttp)
	c->returnValue()->setInteger(m_pHttp->currentId());
	return true;
}
bool KvsObject_http::functionSetUser(KviKvsObjectFunctionCall * c)
{
	CHECK_INTERNAL_POINTER(m_pHttp)
	QString szUser;
	QString szPass;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("user", KVS_PT_STRING, 0, szUser)
	KVSO_PARAMETER("password", KVS_PT_STRING, 0, szPass)
	KVSO_PARAMETERS_END(c)
	m_pHttp->setUser(szUser, szPass);
	return true;
}
bool KvsObject_http::functionSetProxy(KviKvsObjectFunctionCall * c)
{
	CHECK_INTERNAL_POINTER(m_pHttp)
	QString szHost;
	QString szUser, szPass;
	kvs_uint_t uRemotePort;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("host", KVS_PT_STRING, 0, szHost)
	KVSO_PARAMETER("remote_port", KVS_PT_UNSIGNEDINTEGER, KVS_PF_OPTIONAL, uRemotePort)
	KVSO_PARAMETER("user", KVS_PT_STRING, KVS_PF_OPTIONAL, szUser)
	KVSO_PARAMETER("pass", KVS_PT_STRING, KVS_PF_OPTIONAL, szPass)
	KVSO_PARAMETERS_END(c)
	m_pHttp->setProxy(szHost, uRemotePort, szUser, szPass);
	return true;
}

bool KvsObject_http::functionGet(KviKvsObjectFunctionCall * c)
{
	CHECK_INTERNAL_POINTER(m_pHttp)
	QString szPath, szDest;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("remote_path", KVS_PT_STRING, 0, szPath)
	KVSO_PARAMETER("local_filename", KVS_PT_STRING, KVS_PF_OPTIONAL, szDest)
	KVSO_PARAMETERS_END(c)

	QFile * pFile = nullptr;
	if(!szDest.isEmpty())
	{
		pFile = new QFile(szDest);
		if(pFile)
		{
			pFile->open(QIODevice::WriteOnly);
		}
		else
		{
			c->warning(__tr2qs_ctx("'%Q' is not a valid file path", "objects"), &szDest);
			c->returnValue()->setInteger(-1);
			return true;
		}
	}

	if(szPath.isEmpty())
		szPath = "/";
	int id = m_pHttp->get(szPath, pFile);
	if(pFile)
		getDict[id] = pFile;
	c->returnValue()->setInteger(id);
	return true;
}
bool KvsObject_http::functionPost(KviKvsObjectFunctionCall * c)
{
	CHECK_INTERNAL_POINTER(m_pHttp)
	QString szPath, szDest, szData;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("remote_path", KVS_PT_STRING, 0, szPath)
	KVSO_PARAMETER("post_data", KVS_PT_STRING, 0, szData)
	KVSO_PARAMETER("local_filename", KVS_PT_STRING, 0, szDest)
	KVSO_PARAMETERS_END(c)
	QFile * pFile = nullptr;
	if(!szDest.isEmpty())
	{
		pFile = new QFile(szDest);
		if(pFile)
		{
			pFile->open(QIODevice::WriteOnly);
		}
	}
	if(szPath.isEmpty())
		szPath = "/";
	int id = m_pHttp->post(szPath, szDest.toUtf8(), pFile);
	if(pFile)
		getDict[id] = pFile;
	c->returnValue()->setInteger(id);
	return true;
}
bool KvsObject_http::functionAbort(KviKvsObjectFunctionCall * c)
{
	CHECK_INTERNAL_POINTER(m_pHttp)
	m_bAbort = true;
	m_pHttp->abort();
	return true;
}
bool KvsObject_http::functionReadAll(KviKvsObjectFunctionCall * c)
{
	CHECK_INTERNAL_POINTER(m_pHttp)
	c->returnValue()->setString(m_pHttp->readAll());
	return true;
}
bool KvsObject_http::functionErrorString(KviKvsObjectFunctionCall * c)
{
	CHECK_INTERNAL_POINTER(m_pHttp)
	c->returnValue()->setString(m_pHttp->errorString());
	return true;
}
bool KvsObject_http::functionFollowRedirect(KviKvsObjectFunctionCall * c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("enable", KVS_PT_BOOLEAN, 0, bEnabled)
	KVSO_PARAMETERS_END(c)
	m_bEnableForceRedirect = bEnabled;
	return true;
}
//signals & slots

bool KvsObject_http::functionRequestFinishedEvent(KviKvsObjectFunctionCall * c)
{
	emitSignal("requestFinished", c, c->params());
	return true;
}

void KvsObject_http::slotRequestFinished(int id, bool error)
{

	if(m_bAbort)
	{
		m_bAbort = false;
		QHashIterator<int, QFile *> t(getDict);
		while(t.hasNext())
		{
			t.next();
			int key = t.key();
			QFile * pFile = getDict.value(key);
			pFile->close();
			delete pFile;
		}
		getDict.clear();
		return;
	}
	QFile * pFile = getDict.value(id);
	if(pFile)
	{
		QString name = pFile->fileName();
		pFile->close();
		getDict.remove(id);
		int res = m_pHttp->lastResponse().statusCode();
		if((res == 301 || res == 302 || res == 307) && m_bEnableForceRedirect)
			redirect(name, m_pHttp->lastResponse());
		delete pFile;
	}
	KviKvsVariantList lParams;
	lParams.append(new KviKvsVariant((kvs_int_t)id));
	lParams.append(new KviKvsVariant(error));
	callFunction(this, "requestFinishedEvent", nullptr, &lParams);
}

bool KvsObject_http::functionRequestStartedEvent(KviKvsObjectFunctionCall * c)
{
	emitSignal("requestStarted", c, c->params());
	return true;
}
void KvsObject_http::slotRequestStarted(int id)
{
	KviKvsVariantList lParams;
	lParams.append(new KviKvsVariant((kvs_int_t)id));
	callFunction(this, "requestStartedEvent", nullptr, &lParams);
}

void KvsObject_http::slotDataReadProgress(int done, int total)
{
	KviKvsVariantList lParams;
	lParams.append(new KviKvsVariant((kvs_int_t)done));
	lParams.append(new KviKvsVariant((kvs_int_t)total));
	callFunction(this, "dataReadProgressEvent", nullptr, &lParams);
}
bool KvsObject_http::functionDataReadProgressEvent(KviKvsObjectFunctionCall * c)
{
	emitSignal("dataReadProgress", c, c->params());
	return true;
}
void KvsObject_http::slotDataSendProgress(int done, int total)
{
	KviKvsVariantList lParams;
	lParams.append(new KviKvsVariant((kvs_int_t)done));
	lParams.append(new KviKvsVariant((kvs_int_t)total));
	callFunction(this, "dataSendProgressEvent", nullptr, &lParams);
}
bool KvsObject_http::functionDataSendProgressEvent(KviKvsObjectFunctionCall * c)
{
	emitSignal("dataSendProgress", c, c->params());
	return true;
}
bool KvsObject_http::functionDoneEvent(KviKvsObjectFunctionCall * c)
{
	emitSignal("done", c, c->params());
	return true;
}
void KvsObject_http::slotDone(bool error)
{
	KviKvsVariantList lParams;
	lParams.append(new KviKvsVariant(error));
	callFunction(this, "doneEvent", nullptr, &lParams);
}

bool KvsObject_http::functionResponseHeaderReceivedEvent(KviKvsObjectFunctionCall * c)
{
	emitSignal("responseHeaderReceived", c, c->params());
	return true;
}
bool KvsObject_http::functionReadyReadEvent(KviKvsObjectFunctionCall * c)
{
	emitSignal("readyRead", c, c->params());
	return true;
}
void KvsObject_http::redirect(QString & file, const QHttpResponseHeader & r)
{
	QUrl url(r.value("location"));
	m_pHttp->setHost(url.host());
	QFile * pFile = nullptr;
	pFile = new QFile(file);
	pFile->open(QIODevice::WriteOnly);
	int id = m_pHttp->get(url.path(), pFile);
	getDict[id] = pFile;
}

void KvsObject_http::slotResponseHeaderReceived(const QHttpResponseHeader & r)
{
	QString szResponse;
	switch(r.statusCode())
	{
		case 200:
			szResponse = "OK";
			break;
		case 301:
			szResponse = "Moved Permanently";
			break;
		case 302:
			szResponse = "Found";
			break;
		case 303:
			szResponse = "See Other";
			break;
		case 307:
			szResponse = "Temporary Redirect";
			break;
		default:
			szResponse = r.reasonPhrase();
			m_bAbort = true;
	}
	if(r.statusCode() == 400)
	{
		m_bAbort = true;
		m_pHttp->abort();
	}
	KviKvsVariantList lParams;
	lParams.append(new KviKvsVariant(szResponse));
	callFunction(this, "responseHeaderReceivedEvent", nullptr, &lParams);
}
void KvsObject_http::slotReadyRead(const QHttpResponseHeader & r)
{
	QString szResponse;
	switch(r.statusCode())
	{
		case 200:
			szResponse = "OK";
			break;
		case 301:
			szResponse = "Moved Permanently";
			break;
		case 302:
			szResponse = "Found";
			break;
		case 303:
			szResponse = "See Other";
			break;
		case 307:
			szResponse = "Temporary Redirect";
			break;
		default:
			szResponse = r.reasonPhrase();
			m_bAbort = true;
	}
	KviKvsVariantList lParams;
	lParams.append(new KviKvsVariant(szResponse));
	callFunction(this, "readyreadEvent", nullptr, &lParams);
}

void KvsObject_http::slotStateChanged(int state)
{
	QString szState = "";
	if(state == QHttp::Unconnected)
		szState = "Unconnected";
	else if(state == QHttp::HostLookup)
		szState = "HostLookup";
	else if(state == QHttp::Connecting)
		szState = "Connecting";
	else if(state == QHttp::Connected)
		szState = "Connected";
	else if(state == QHttp::Sending)
		szState = "Sending";
	else if(state == QHttp::Reading)
		szState = "Reading";

	else if(state == QHttp::Closing)
		szState = "Closing";
	KviKvsVariantList lParams;
	lParams.append(new KviKvsVariant(szState));
	callFunction(this, "stateChangedEvent", nullptr, &lParams);
}
bool KvsObject_http::functionStateChangedEvent(KviKvsObjectFunctionCall * c)
{
	emitSignal("stateChanged", c, c->params());
	return true;
}

#ifndef QT_NO_OPENSSL

bool KvsObject_http::functionIgnoreSslErrors(KviKvsObjectFunctionCall *)
{
	if(m_pHttp)
		m_pHttp->ignoreSslErrors();
	return true;
}
void KvsObject_http::slotSslErrors(QList<QSslError> sslerrors)
{
	KviKvsArray * pArray = nullptr;
	pArray = new KviKvsArray();
	for(int i = 0; i < sslerrors.count(); i++)
	{
		pArray->set(i, new KviKvsVariant(ssl_errors_tbl[sslerrors.at(i).error()]));
	}
	KviKvsVariantList lParams;
	lParams.append(new KviKvsVariant(pArray));
	callFunction(this, "sslErrorEvent", nullptr, &lParams);
}

bool KvsObject_http::functionSslErrorsEvent(KviKvsObjectFunctionCall * c)
{

	emitSignal("sslErrors", c, c->params());
	return true;
}
#endif
