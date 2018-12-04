//=============================================================================
//
//   File : KviHttpRequest.cpp
//   Creation date : Sat Aug 17 13:43:32 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviHttpRequest.h"
#include "KviCString.h"
#include "KviDataBuffer.h"
#include "KviDnsResolver.h"
#include "KviError.h"
#include "KviFile.h"
#include "KviLocale.h"
#include "KviNetUtils.h"
#include "KviTimeUtils.h"
#include "kvi_debug.h"

#include <QTcpSocket>
#ifdef COMPILE_SSL_SUPPORT
#include <QSslSocket>
#endif
#include <QDir>
#include <QHostAddress>
#include <QString>
#include <QStringList>
#include <QTimer>

//#include <zlib.h>

class KviHttpRequestPrivate
{
public:
	QTcpSocket * pSocket;
	bool bIsSSL;
	KviDataBuffer * pBuffer;
	QTimer * pConnectTimeoutTimer;
	KviFile * pFile;
	unsigned short uPort;
};

KviHttpRequest::KviHttpRequest()
    : QObject()
{
	m_p = new KviHttpRequestPrivate();
	m_p->pSocket = nullptr;
	m_p->bIsSSL = false;
	m_p->pConnectTimeoutTimer = nullptr;
	m_p->pBuffer = new KviDataBuffer();
	m_p->pFile = nullptr;

	m_pPrivateData = nullptr;
	m_bHeaderProcessed = false;
	m_uConnectionTimeout = 60;
	m_bFollowRedirects = true;
	m_uRedirectCount = 0;
	m_uMaximumRedirectCount = 2;

	resetStatus();
	resetData();
}

KviHttpRequest::~KviHttpRequest()
{
	resetInternalStatus();

	KVI_ASSERT(!(m_p->pSocket));
	KVI_ASSERT(!(m_p->pFile));

	if(m_p->pBuffer)
		delete m_p->pBuffer;

	delete m_p;
}

void KviHttpRequest::abort()
{
	resetInternalStatus();
	m_szLastError = __tr2qs("Aborted");
	emit terminated(false);
}

void KviHttpRequest::resetInternalStatus()
{
	if(m_p->pConnectTimeoutTimer)
	{
		delete m_p->pConnectTimeoutTimer;
		m_p->pConnectTimeoutTimer = nullptr;
	}

	if(m_p->pSocket)
		closeSocket();

	if(m_p->pFile)
	{
		m_p->pFile->close();
		delete m_p->pFile;
		m_p->pFile = nullptr;
	}

	m_p->pBuffer->clear();
	m_bHeaderProcessed = false;

	//KviThreadManager::killPendingEvents(this);
	//QApplication::removePostedEvents(this);
}

void KviHttpRequest::resetStatus()
{
	m_szLastError = __tr2qs("No request");
	m_uTotalSize = 0;
	m_uReceivedSize = 0;
}

void KviHttpRequest::resetData()
{
	m_szFileName = "";
	m_eProcessingType = WholeFile;
	m_eExistingFileAction = RenameIncoming;
	m_url = QString("");
	m_connectionUrl = QString("");
	m_uMaxContentLength = 0;
	m_uContentOffset = 0;
	m_bChunkedTransferEncoding = false;
	m_bGzip = false;
	m_bIgnoreRemainingData = false;
	m_uRemainingChunkSize = 0;
	m_uRedirectCount = 0;
}

void KviHttpRequest::reset()
{
	resetStatus();
	resetData();
	resetInternalStatus();
}

bool KviHttpRequest::get(const KviUrl & u, ProcessingType p, const QString & szFileName)
{
	reset();
	setUrl(u);
	setProcessingType(p);
	setFileName(szFileName);
	return start();
}

bool KviHttpRequest::start()
{
	// ensure that the file is closed
	resetInternalStatus();
	resetStatus();

	if(m_eProcessingType == StoreToFile)
	{
		if(m_szFileName.isEmpty())
		{
			m_szLastError = __tr2qs("No filename specified for the \"StoreToFile\" processing type");
			return false;
		}

		if((m_eExistingFileAction == Resume) && (m_uContentOffset == 0))
		{
			// determine the content offset automatically
			if(KviFile::exists(m_szFileName))
			{
				// we check it
				QFileInfo fi(m_szFileName);
				m_uContentOffset = fi.size();
			}
		}
	}

	if(m_connectionUrl.host().isEmpty())
	{
		resetInternalStatus();
		m_szLastError = __tr2qs("Invalid URL: missing hostname");
		return false;
	}

	m_p->bIsSSL = KviQString::equalCI(m_connectionUrl.protocol(), "https");

	if(!KviQString::equalCI(m_connectionUrl.protocol(), "http") && !m_p->bIsSSL)
	{
		resetInternalStatus();
		m_szLastError = __tr2qs("Unsupported protocol %1").arg(m_connectionUrl.protocol());
		return false;
	}

	return doConnect();
}

void KviHttpRequest::closeSocket()
{
	if(!m_p->pSocket)
		return;

	QObject::disconnect(m_p->pSocket, SIGNAL(connected()), this, SLOT(slotSocketConnected()));
	QObject::disconnect(m_p->pSocket, SIGNAL(disconnected()), this, SLOT(slotSocketDisconnected()));
	QObject::disconnect(m_p->pSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotSocketError(QAbstractSocket::SocketError)));

	m_p->pSocket->abort();
	m_p->pSocket->close();

	// This can be called from a socket handler slot
	m_p->pSocket->deleteLater();

	m_p->pSocket = nullptr;
}

void KviHttpRequest::slotSocketDisconnected()
{
	switch(m_eProcessingType)
	{
		case WholeFile:
			// happens always
			emit binaryData(*m_p->pBuffer);
			break;
		case Blocks:
			// an unprocessed block ?.. should never happened.. but well :D
			if(m_p->pBuffer->size() > 0)
				emit binaryData(*m_p->pBuffer);
			break;
		case Lines:
			if(m_p->pBuffer->size() > 0)
			{
				// something left in the buffer and has no trailing LF
				KviCString tmp((const char *)(m_p->pBuffer->data()), m_p->pBuffer->size());
				emit data(tmp);
			}
			break;
		case StoreToFile:
			// same as above... should never happen.. but well :D
			if(m_p->pFile && m_p->pBuffer->size() > 0)
				m_p->pFile->write((const char *)(m_p->pBuffer->data()), m_p->pBuffer->size());
			break;
		default:
			// nothing... just make gcc happy
			break;
	}
	resetInternalStatus();
	m_szLastError = __tr2qs("Success");
	emit terminated(true);
}

void KviHttpRequest::slotSocketConnected()
{
	if(m_p->pConnectTimeoutTimer)
	{
		delete m_p->pConnectTimeoutTimer;
		m_p->pConnectTimeoutTimer = nullptr;
	}

	emit connectionEstablished();
	emit status(
	    __tr2qs("Connected to %1:%2: sending request")
	        .arg(m_p->pSocket->peerAddress().toString())
	        .arg(m_p->pSocket->peerPort()));

	KviCString szMethod;

	bool bIsPost = false;

	if(m_eProcessingType == HeadersOnly)
		szMethod = "HEAD";
	else if(m_szPostData.isEmpty())
		szMethod = "GET";
	else
	{
		szMethod = "POST";
		bIsPost = true;
	}

	KviCString szRequest(
	    KviCString::Format,
	    "%s %s HTTP/1.1\r\n"
	    "Host: %s\r\n"
	    "Connection: Close\r\n"
	    "User-Agent: KVIrc-http-slave/1.0.0\r\n"
	    "Accept: */*\r\n",
	    szMethod.ptr(),
	    m_connectionUrl.path().toUtf8().data(),
	    m_connectionUrl.host().toUtf8().data());

	if(m_uContentOffset > 0)
		szRequest.append(KviCString::Format, "Range: bytes=%u-\r\n", m_uContentOffset);

	if(bIsPost)
	{
		szRequest.append(KviCString::Format, "Content-Type: application/x-www-form-urlencoded\r\n"
		                                     "Content-Length: %u\r\n"
		                                     "Cache-control: no-cache\r\n"
		                                     "Pragma: no-cache\r\n",
		    m_szPostData.length());
	}

	szRequest += "\r\n";

	if(bIsPost)
	{
		if(!m_szPostData.isEmpty())
			szRequest.append(m_szPostData);
		szRequest += "\r\n";
	}

	// FIXME: Handle this better!
	int written = m_p->pSocket->write(szRequest.ptr(), szRequest.len());
	if(written < szRequest.len())
	{
		m_szLastError = __tr2qs("Socket write error");
		resetInternalStatus();
		emit terminated(false);
	}

	// FIXME: Handle this better
	QString req = QString::fromLatin1(szRequest.ptr());
	QStringList sl = req.split("\r\n");
	emit requestSent(sl);

	// now wait for the response
	// FIXME: Handle read timeouts!
}

void KviHttpRequest::slotSocketReadDataReady()
{
	KVI_ASSERT(m_p->pSocket);

	int iBytes = m_p->pSocket->bytesAvailable();

	if(iBytes <= 0)
	{
		// assume connection closed ?
		slotSocketDisconnected();
		return;
	}

	// FIXME: Avoid double-buffering here!
	KviDataBuffer oBuffer(iBytes);

	int iRead = m_p->pSocket->read((char *)(oBuffer.data()), iBytes);
	if(iRead < iBytes)
	{
		// hum.... what here ?
		if(iRead < 1)
		{
			slotSocketDisconnected();
			return;
		}

		// FIXME
		oBuffer.resize(iRead);
	}

	processData(&oBuffer);
}

void KviHttpRequest::slotSocketError(QAbstractSocket::SocketError eError)
{
	KVI_ASSERT(m_p->pSocket);

	if(eError == QAbstractSocket::RemoteHostClosedError)
	{
		slotSocketDisconnected();
		return;
	}

	m_szLastError = m_p->pSocket->errorString();
	resetInternalStatus();
	emit terminated(false);
}

void KviHttpRequest::slotConnectionTimedOut()
{
	resetInternalStatus();
	m_szLastError = __tr2qs("Connection timed out");
	emit terminated(false);
}

void KviHttpRequest::slotSocketHostResolved()
{
	emit contactingHost(QString::fromLatin1("%1:%2").arg(m_connectionUrl.host()).arg(m_p->uPort));
	emit status(__tr2qs("Contacting host %1 on port %2").arg(m_connectionUrl.host()).arg(m_p->uPort));
}

bool KviHttpRequest::doConnect()
{
	m_p->uPort = m_connectionUrl.port();
	if(m_p->uPort == 0)
		m_p->uPort = m_p->bIsSSL ? 443 : 80;

	if(m_p->pSocket)
		closeSocket();
#ifdef COMPILE_SSL_SUPPORT
	m_p->pSocket = m_p->bIsSSL ? new QSslSocket() : new QTcpSocket();
#else
	m_p->pSocket = new QTcpSocket();
#endif
	QObject::connect(m_p->pSocket, SIGNAL(connected()), this, SLOT(slotSocketConnected()));
	QObject::connect(m_p->pSocket, SIGNAL(disconnected()), this, SLOT(slotSocketDisconnected()));
	QObject::connect(m_p->pSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotSocketError(QAbstractSocket::SocketError)));
	QObject::connect(m_p->pSocket, SIGNAL(readyRead()), this, SLOT(slotSocketReadDataReady()));
	QObject::connect(m_p->pSocket, SIGNAL(hostFound()), this, SLOT(slotSocketHostResolved()));

	emit resolvingHost(m_connectionUrl.host());

#ifdef COMPILE_SSL_SUPPORT
	if(m_p->bIsSSL)
	{
		static_cast<QSslSocket *>(m_p->pSocket)->setProtocol(QSsl::AnyProtocol);
		static_cast<QSslSocket *>(m_p->pSocket)->connectToHostEncrypted(m_connectionUrl.host(), m_p->uPort);
	}
	else
	{
		m_p->pSocket->connectToHost(m_connectionUrl.host(), m_p->uPort);
	}
#else
	m_p->pSocket->connectToHost(m_connectionUrl.host(), m_p->uPort);
#endif

	if(m_p->pConnectTimeoutTimer)
	{
		delete m_p->pConnectTimeoutTimer;
		m_p->pConnectTimeoutTimer = nullptr;
	}

	m_p->pConnectTimeoutTimer = new QTimer();
	m_p->pConnectTimeoutTimer->setSingleShot(true);
	QObject::connect(m_p->pConnectTimeoutTimer, SIGNAL(timeout()), this, SLOT(slotConnectionTimedOut()));

	m_p->pConnectTimeoutTimer->start(m_uConnectionTimeout * 1000);

	return true;
}

void KviHttpRequest::emitLines(KviDataBuffer * pDataBuffer)
{
	int idx = pDataBuffer->find((const unsigned char *)"\n", 1);
	while(idx != -1)
	{
		KviCString tmp((const char *)(m_p->pBuffer->data()), idx);
		tmp.stripRight('\r');
		pDataBuffer->remove(idx + 1);
		idx = pDataBuffer->find((const unsigned char *)"\n", 1);
		emit data(tmp);
	}
}

bool KviHttpRequest::openFile()
{
	if(m_eProcessingType != StoreToFile)
		return true;

	bool bAppend = false;

	// take action when the file is existing
	if(KviFile::exists(m_szFileName))
	{
		switch(m_eExistingFileAction)
		{
			case Resume:
			{
				bAppend = true;
			}
			break;
			case RenameIncoming:
			{
				int i = 0;
				QString tmp = m_szFileName;
				do
				{
					i++;
					m_szFileName = tmp + QString(".kvirnm-%1").arg(i);
				} while(KviFile::exists(m_szFileName));
			}
			break;
			case RenameExisting:
			{
				int i = 0;
				QString tmp;
				do
				{
					i++;
					tmp = m_szFileName + QString(".kvirnm-%1").arg(i);
				} while(KviFile::exists(tmp));
				QDir d;
				if(!d.rename(m_szFileName, tmp))
				{
					// fail :(
					resetInternalStatus();
					m_szLastError = __tr2qs("Failed to rename the existing file, please rename manually and retry");
					emit terminated(false);
					return false;
				}
			}
			break;
			case Overwrite:
			default:
				// nothing
				break;
		}
	}

	m_p->pFile = new KviFile(m_szFileName);

	if(!m_p->pFile->open(QFile::WriteOnly | (bAppend ? QFile::Append : QFile::Truncate)))
	{
		resetInternalStatus();
		m_szLastError = QString(__tr2qs("Can't open file \"%1\" for writing")).arg(m_szFileName);
		emit terminated(false);
		return false;
	}

	return true;
}

bool KviHttpRequest::processHeader(KviCString & szHeader)
{
	int idx = szHeader.findFirstIdx("\r\n");
	KviCString szResponse;
	if(idx != -1)
	{
		szResponse = szHeader.left(idx);
		szHeader.cutLeft(idx + 2);
	}
	else
	{
		szResponse = szHeader;
		szHeader = "";
	}

	szResponse.trim();

	bool bValid = false;

	unsigned int uStatus = 0;

	// check the response value
	if(kvi_strEqualCSN(szResponse.ptr(), "HTTP", 4))
	{
		KviCString szR = szResponse;
		szR.cutToFirst(' ');
		szR.trim();
		int idx = szR.findFirstIdx(' ');
		KviCString szNumber;
		if(idx != -1)
			szNumber = szR.left(idx);
		else
			szNumber = szR;
		bool bOk;
		uStatus = szNumber.toUInt(&bOk);
		if(bOk)
			bValid = true;
	}

	QString szUniResponse = QString::fromUtf8(szResponse.ptr());

	if(!bValid)
	{
		// the response is invalid ?
		resetInternalStatus();
		m_szLastError = __tr2qs("Invalid HTTP response: %1").arg(szUniResponse);
		emit terminated(false);
		return false;
	}

	emit status(__tr2qs("Received HTTP response: %1").arg(szUniResponse));

	KviPointerList<KviCString> hlist;
	hlist.setAutoDelete(true);

	idx = szHeader.findFirstIdx("\r\n");
	while(idx != -1)
	{
		if(idx > 0)
		{
			hlist.append(new KviCString(szHeader.ptr(), idx));
			szHeader.cutLeft(idx + 2);
		}
		idx = szHeader.findFirstIdx("\r\n");
	}
	if(szHeader.hasData())
		hlist.append(new KviCString(szHeader));

	KviPointerHashTable<const char *, KviCString> hdr(11, false, true);
	hdr.setAutoDelete(true);

	for(KviCString * s = hlist.first(); s; s = hlist.next())
	{
		idx = s->findFirstIdx(":");
		if(idx != -1)
		{
			KviCString szName = s->left(idx);
			s->cutLeft(idx + 1);
			s->trim();
			hdr.replace(szName.ptr(), new KviCString(*s));
			//qDebug("FOUND HEADER (%s)=(%s)",szName.ptr(),s->ptr());
		}
	}

	KviCString * size = hdr.find("Content-length");
	if(size)
	{
		bool bOk;
		m_uTotalSize = size->toUInt(&bOk);
		if(!bOk)
			m_uTotalSize = 0;
	}

	KviCString * contentEncoding = hdr.find("Content-encoding");
	if(contentEncoding)
	{
		m_bGzip = contentEncoding->equalsCI("gzip");
	}

	KviCString * transferEncoding = hdr.find("Transfer-Encoding");
	if(transferEncoding)
	{
		if(kvi_strEqualCI(transferEncoding->ptr(), "chunked"))
		{
			// be prepared to handle the chunked transfer encoding as required by HTTP/1.1
			m_bChunkedTransferEncoding = true;
			m_uRemainingChunkSize = 0;
		}
	}

	// check the status

	// case 200: // OK
	// case 206: // Partial content
	// case 100: // Continue
	// case 101: // Switching protocols
	// case 201: // Created
	// case 202: // Accepted
	// case 203: // Non-Authoritative Information
	// case 204: // No content
	// case 205: // Reset content
	// case 300: // Multiple choices
	// case 301: // Moved permanently
	// case 302: // Found
	// case 303: // See Other
	// case 304: // Not modified
	// case 305: // Use Proxy
	// case 306: // Switch Proxy
	// case 307: // Temporary Redirect
	// case 400: // Bad request
	// case 401: // Unauthorized
	// case 402: // Payment Required
	// case 403: // Forbidden
	// case 404: // Not found
	// case 405: // Method not allowed
	// case 406: // Not acceptable
	// case 407: // Proxy authentication required
	// case 408: // Request timeout
	// case 409: // Conflict
	// case 410: // Gone
	// case 411: // Length required
	// case 412: // Precondition failed
	// case 413: // Request entity too large
	// case 414: // Request-URI Too Long
	// case 415: // Unsupported media type
	// case 416: // Requested range not satisfiable
	// case 417: // Expectation Failed
	// case 500: // Internal server error
	// case 501: // Not implemented
	// case 502: // Bad gateway
	// case 503: // Service unavailable
	// case 504: // Gateway timeout
	// case 505: // HTTP Version not supported

	if(
	    (uStatus != 200) && // OK
	    (uStatus != 206)    // Partial content
	    )
	{
		// This is not "OK" and not "Partial content"
		// Error, redirect or something confusing
		if(m_eProcessingType != HeadersOnly)
		{
			switch(uStatus)
			{
				case 301: // Moved permanently
				case 302: // Found
				case 303: // See Other
				case 307: // Temporary Redirect
				{
					if(!m_bFollowRedirects)
					{
						resetInternalStatus();
						m_szLastError = szResponse.ptr();
						emit terminated(false);
						return false;
					}

					m_uRedirectCount++;

					if(m_uRedirectCount > m_uMaximumRedirectCount)
					{
						resetInternalStatus();
						m_szLastError = __tr2qs("Too many redirects");
						emit terminated(false);
						return false;
					}

					KviCString * headerLocation = hdr.find("Location");

					if(!headerLocation || headerLocation->isEmpty())
					{
						resetInternalStatus();
						m_szLastError = __tr2qs("Bad redirect");
						emit terminated(false);
						return false;
					}

					KviUrl url;
					QString location(headerLocation->ptr());

					if(location.startsWith('/'))
					{
						// relative redirect, use the old url and only update the path
						url = m_connectionUrl;
						url.setPath(location);
					} else {
						// absolute redirect
						url.setUrl(location);
					}

					if(
					    (url.url() == m_connectionUrl.url()) || (url.url() == m_url.url()))
					{
						resetInternalStatus();
						m_szLastError = __tr2qs("Redirect loop");
						emit terminated(false);
						return false;
					}

					m_connectionUrl = url;

					emit status(__tr2qs("Following Redirect to %1").arg(url.url()));

					if(!start())
						emit terminated(false);

					return false; // will exit the call stack
				}
				break;
					break;
				default:
					// assume error
					resetInternalStatus();
					m_szLastError = szResponse.ptr();
					emit terminated(false);
					return false;
					break;
			}
			// this is an error then
		} // else the server will terminate (it was a HEAD request)
	}

	emit receivedResponse(szUniResponse);

	emit header(&hdr);

	if((m_uMaxContentLength > 0) && (m_uTotalSize > ((unsigned int)m_uMaxContentLength)))
	{
		resetInternalStatus();
		m_szLastError = __tr2qs("The amount of received data exceeds the maximum length");
		emit terminated(false);
		return false;
	}

	// fixme: could check for data type etc...

	return true;
}
#define BUFFER_SIZE 32768

void KviHttpRequest::processData(KviDataBuffer * data)
{
	//	unsigned char obuffer[BUFFER_SIZE];
	if(m_bChunkedTransferEncoding && m_bIgnoreRemainingData)
	{
		// In chunked transfer encoding mode there may be additional headers
		// after the last chunk of data. We simply ignore them.
		return;
	}

	if(!m_bHeaderProcessed)
	{
		// time to process the header
		m_p->pBuffer->append(*data);

		int idx = m_p->pBuffer->find((const unsigned char *)"\r\n\r\n", 4);
		if(idx == -1)
		{
			// header not complete
			if(m_p->pBuffer->size() > 4096)
			{
				resetInternalStatus();
				m_szLastError = __tr2qs("Header too long: exceeded 4096 bytes");
				emit terminated(false);
			}
			return;
		}
		KviCString szHeader((const char *)(m_p->pBuffer->data()), idx);
		m_p->pBuffer->remove(idx + 4);

		if(!processHeader(szHeader))
			return;

		m_bHeaderProcessed = true;

		if(m_eProcessingType == StoreToFile)
		{
			if(!openFile())
				return;
		}

		m_uReceivedSize = m_p->pBuffer->size();
		// here the header is complete and the eventual remaining data is in m_p->pBuffer. data has been already used.
	}
	else
	{
		// header already processed
		m_uReceivedSize += data->size();

		// here the header is complete and some data *might* be already in m_p->pBuffer. data is unused yet.

		// Optimisation: If the transfer is NOT chunked (so we don't have to parse it)
		// and the requested processing type is either Blocks or StoreToFile
		// then we just can avoid to copy the data to m_p->pBuffer.
		// This is a good optimisation since for large files we can save allocating
		// space for and moving megabytes of data...

		if((!m_bChunkedTransferEncoding) && ((m_eProcessingType == Blocks) || (m_eProcessingType == StoreToFile)))
		{
			switch(m_eProcessingType)
			{
				case Blocks:
					emit binaryData(*data);
					break;
				case StoreToFile:
					m_p->pFile->write((const char *)(data->data()), data->size());
					break;
				default:
					break;
			}

			if(((m_uTotalSize > 0) && (m_uReceivedSize > m_uTotalSize)) || ((m_uMaxContentLength > 0) && (m_uReceivedSize > m_uMaxContentLength)))
			{
				resetInternalStatus();
				m_szLastError = __tr2qs("The amount of received data exceeds expected length");
				emit terminated(false);
			}

			return;
		}

		// need to append to m_p->pBuffer and process it
		m_p->pBuffer->append(*data);
	}

	// we're processing data in m_p->pBuffer here
	if(m_bChunkedTransferEncoding)
	{
		// The transfer encoding is chunked: the buffer contains
		// chunks of data with an initial header composed
		// of a hexadecimal length, an optional bullshit and a single CRLF
		// The transfer terminates when we read a last chunk of size 0
		// that may be followed by optional headers...
		// This sux :)
		while(m_p->pBuffer->size() > 0) // <-- note that we may exit from this loop also for other conditions (there is a goto below)
		{
			// we process chunks of parts of chunks at a time.
			if(m_uRemainingChunkSize > 0)
			{
				// process the current chunk data
				unsigned int uProcessSize = m_uRemainingChunkSize;
				if(uProcessSize > (unsigned int)m_p->pBuffer->size())
					uProcessSize = m_p->pBuffer->size();
				m_uRemainingChunkSize -= uProcessSize;

				switch(m_eProcessingType)
				{
					case Blocks:
						if((unsigned int)m_p->pBuffer->size() == uProcessSize)
						{
							// avoid copying to a new buffer
							emit binaryData(*m_p->pBuffer);
						}
						else
						{
							// must copy
							KviDataBuffer tmp(uProcessSize, m_p->pBuffer->data());
							emit binaryData(tmp);
							m_p->pBuffer->remove(uProcessSize);
						}
						break;
					case Lines:
						if((unsigned int)m_p->pBuffer->size() == uProcessSize)
						{
							// avoid copying to a new buffer
							emitLines(m_p->pBuffer);
						}
						else
						{
							// must copy
							KviDataBuffer tmp(uProcessSize, m_p->pBuffer->data());
							emitLines(&tmp);
							m_p->pBuffer->remove(uProcessSize);
						}
						break;
					case StoreToFile:
						m_p->pFile->write((const char *)(m_p->pBuffer->data()), uProcessSize);
						m_p->pBuffer->remove(uProcessSize);
						break;
					default:
						// nothing.. just make gcc happy
						break;
				}
				// now either the buffer is empty or there is another chunk header: continue looping
			}
			else
			{
				// We're looking for the beginning of a chunk now.
				// Note that we might be at the end of a previous chunk that has a CRLF terminator
				// we need to skip it.
				int crlf = m_p->pBuffer->find((const unsigned char *)"\r\n", 2);
				if(crlf != -1)
				{
					if(crlf == 0)
					{
						// This is a plain CRLF at the beginning of the buffer BEFORE a chunk header.
						// It comes from the previous chunk terminator. Skip it.
						m_p->pBuffer->remove(2);
					}
					else
					{
						// got a chunk header
						KviCString szHeader((const char *)(m_p->pBuffer->data()), crlf);
						szHeader.cutFromFirst(' ');
						// now szHeader should contain a hexadecimal chunk length... (why the hell it is hex and not decimal ????)
						QString szHexHeader = szHeader.ptr();
						bool bOk;
						m_uRemainingChunkSize = szHexHeader.toLong(&bOk, 16);
						if(!bOk)
						{
							resetInternalStatus();
							m_szLastError = __tr2qs("Protocol error: invalid chunk size");
							emit terminated(false);
							return;
						}
						m_p->pBuffer->remove(crlf + 2);
						if(m_uRemainingChunkSize == 0)
						{
							// this is the last chunk of data. It may be followed by optional headers
							// but we actually don't need them (since we're surely not in HEAD mode)
							m_bIgnoreRemainingData = true;
							m_p->pBuffer->clear();
							goto check_stream_length;
						}
					}
					// the rest is valid data of a non-zero chunk: continue looping
				}
				else
				{
					// chunk header not complete
					if(m_p->pBuffer->size() > 4096)
					{
						resetInternalStatus();
						m_szLastError = __tr2qs("Chunk header too long: exceeded 4096 bytes");
						emit terminated(false);
						return;
					}
					goto check_stream_length;
				}
			}
		}
	}
	else
	{
		// the transfer encoding is not chunked: m_p->pBuffer contains only valid data
		switch(m_eProcessingType)
		{
			case Blocks:
				if(m_p->pBuffer->size() > 0)
					emit binaryData(*m_p->pBuffer);
				m_p->pBuffer->clear();
				break;
			case Lines:
				if(m_p->pBuffer->size() > 0)
					emitLines(m_p->pBuffer);
				break;
			case StoreToFile:
				m_p->pFile->write((const char *)(m_p->pBuffer->data()), m_p->pBuffer->size());
				m_p->pBuffer->clear();
				break;
			default:
				// nothing.. just make gcc happy
				break;
		}
	}

check_stream_length:

	if(((m_uTotalSize > 0) && (m_uReceivedSize > m_uTotalSize)) || ((m_uMaxContentLength > 0) && (m_uReceivedSize > m_uMaxContentLength)))
	{
		resetInternalStatus();
		m_szLastError = __tr2qs("The amount of received data exceeds expected length");
		emit terminated(false);
	}
	return;
}

#if 0

bool KviHttpRequest::startDnsLookup()
{
	m_pDns = new KviDnsResolver();
	connect(m_pDns,SIGNAL(lookupDone(KviDnsResolver *)),this,SLOT(dnsLookupDone(KviDnsResolver *)));

	if(!m_pDns->lookup(m_connectionUrl.host(),KviDnsResolver::IPv4))
	{
		resetInternalStatus();
		m_szLastError = __tr2qs("Unable to start the DNS lookup");
		return false;
	}

	QString tmp;
	tmp = QString(__tr2qs("Looking up host %1")).arg(m_connectionUrl.host());
	emit status(tmp); // FIXME


	return true;
}

void KviHttpRequest::dnsLookupDone(KviDnsResolver *d)
{
	if(d->state() == KviDnsResolver::Success)
	{
		m_szIp = d->firstIpAddress();
		delete m_pDns;
		m_pDns = nullptr;
		QString tmp;
		tmp = QString(__tr2qs("Host %1 resolved to %2")).arg(m_connectionUrl.host(),m_szIp);
		emit status(tmp);
		haveServerIp();
	} else {
		int iErr = d->error();
		resetInternalStatus();
		m_szLastError = KviError::getDescription(iErr);
		emit terminated(false);
	}
}
#endif
