//=============================================================================
//
//   File : kvi_http.cpp
//   Creation date : Sat Aug 17 13:43:32 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
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



#include <QDir>
#include <QTimer>
//#include <zlib.h>

#include "kvi_http.h"
#include "kvi_locale.h"
#include "kvi_netutils.h"
#include "kvi_dns.h"
#include "kvi_error.h"
#include "kvi_debug.h"
#include "kvi_socket.h"
#include "kvi_time.h"

#ifdef COMPILE_SSL_SUPPORT
	#include "kvi_ssl.h"
#endif


#define KVI_HTTP_REQUEST_THREAD_EVENT_CONNECTED (KVI_THREAD_USER_EVENT_BASE + 0xCAFE)
#define KVI_HTTP_REQUEST_THREAD_EVENT_REQUESTSENT (KVI_THREAD_USER_EVENT_BASE + 0xCAFF)

KviHttpRequest::KviHttpRequest()
: QObject()
{
	m_pDns = 0;
	m_pThread = 0;
	m_pFile = 0;
	m_pPrivateData = 0;
	m_bHeaderProcessed = false;
	m_pBuffer = new KviDataBuffer();
	m_uConnectionTimeout = 60;

	resetStatus();
	resetData();
}

KviHttpRequest::~KviHttpRequest()
{
	resetInternalStatus();
	delete m_pBuffer;
}

void KviHttpRequest::abort()
{
	resetInternalStatus();
	m_szLastError = __tr2qs("Aborted");
	emit terminated(false);
}

void KviHttpRequest::resetInternalStatus()
{
	if(m_pThread)delete m_pThread;
	if(m_pDns)delete m_pDns;

	m_pDns = 0;
	m_pThread = 0;

	if(!m_pFile)return;
	m_pFile->close();
	delete m_pFile;
	m_pFile = 0;

	m_pBuffer->clear();
	m_bHeaderProcessed = false;

	KviThreadManager::killPendingEvents(this);
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
	m_uMaxContentLength = 0;
	m_uContentOffset = 0;
	m_bChunkedTransferEncoding = false;
	m_bGzip = false;
	m_bIgnoreRemainingData = false;
	m_uRemainingChunkSize = 0;
}

void KviHttpRequest::reset()
{
	resetStatus();
	resetData();
	resetInternalStatus();
}

bool KviHttpRequest::get(const KviUrl &u,ProcessingType p,const QString &szFileName)
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

	if(m_url.host().isEmpty())
	{
		resetInternalStatus();
		m_szLastError = __tr2qs("Invalid URL: Missing hostname");
		return false;
	}

        if(!KviQString::equalCI(m_url.protocol(),"http") && !KviQString::equalCI(m_url.protocol(),"https"))
	{
		resetInternalStatus();
		m_szLastError=__tr2qs("Unsupported protocol %1").arg(m_url.protocol());
		return false;
	}

	if(KviNetUtils::isValidStringIp(m_url.host()))
	{
		m_szIp = m_url.host();
		QTimer::singleShot(10,this,SLOT(haveServerIp()));
		return true;
	}

	return startDnsLookup();
}

bool KviHttpRequest::startDnsLookup()
{
	m_pDns = new KviDns();
	connect(m_pDns,SIGNAL(lookupDone(KviDns *)),this,SLOT(dnsLookupDone(KviDns *)));

	if(!m_pDns->lookup(m_url.host(),KviDns::IPv4))
	{
		resetInternalStatus();
		m_szLastError = __tr2qs("Unable to start the DNS lookup");
		return false;
	}

	QString tmp;
	KviQString::sprintf(tmp,__tr2qs("Looking up host %Q"),&(m_url.host()));
	emit status(tmp); // FIXME

	emit resolvingHost(m_url.host());

	return true;
}

void KviHttpRequest::dnsLookupDone(KviDns *d)
{
	if(d->state() == KviDns::Success)
	{
		m_szIp = d->firstIpAddress();
		delete m_pDns;
		m_pDns = 0;
		QString tmp;
		KviQString::sprintf(tmp,__tr2qs("Host %Q resolved to %Q"),&(m_url.host()),&m_szIp);
		emit status(tmp);
		haveServerIp();
	} else {
		int iErr = d->error();
		resetInternalStatus();
		m_szLastError = KviError::getDescription(iErr);
		emit terminated(false);
	}
}

void KviHttpRequest::haveServerIp()
{
	unsigned short uPort = m_url.port();
	if(uPort == 0)uPort = 80;

	QString tmp;
	KviQString::sprintf(tmp,"%Q:%u",&m_szIp,uPort);
	emit contactingHost(tmp);

	if(m_pThread)delete m_pThread;

	m_pThread = new KviHttpRequestThread(
			this,
			m_url.host(),
			m_szIp,
			uPort,
			m_url.path(),
			m_uContentOffset,
			(m_eProcessingType == HeadersOnly) ? KviHttpRequestThread::Head : (m_szPostData.isEmpty() ? KviHttpRequestThread::Get : KviHttpRequestThread::Post),
			m_szPostData,
			m_url.protocol()=="https"
		);

	m_pThread->setConnectionTimeout(m_uConnectionTimeout);

	if(!m_pThread->start())
	{
		resetInternalStatus();
		m_szLastError = __tr2qs("Unable to start the request slave thread");
		emit terminated(false);
		return;
	}

	KviQString::sprintf(tmp,__tr2qs("Contacting host %Q on port %u"),&m_szIp,uPort);
	emit status(tmp);
}

bool KviHttpRequest::event(QEvent *e)
{
	if(e->type() == KVI_THREAD_EVENT)
	{
		switch(((KviThreadEvent *)e)->id())
		{
			case KVI_THREAD_EVENT_BINARYDATA:
			{
				KviDataBuffer * b = ((KviThreadDataEvent<KviDataBuffer> *)e)->getData();
				processData(b);
				delete b;
				return true;
			}
			break;
			case KVI_HTTP_REQUEST_THREAD_EVENT_CONNECTED:
				emit connectionEstabilished();
				emit status(__tr2qs("Connection established, sending request"));
				return true;
			break;
			case KVI_HTTP_REQUEST_THREAD_EVENT_REQUESTSENT:
			{
				QString * req = ((KviThreadDataEvent<QString> *)e)->getData();
				QStringList sl = req->split("\r\n");
				emit requestSent(sl);
				delete req;
				return true;
			}
			break;
			case KVI_THREAD_EVENT_SUCCESS:
				if(!m_pThread && !m_bHeaderProcessed)
				{
					// the thread has already been deleted
					// probably because the response was something like a 404
					// just ignore the event
					return true;
				}
				switch(m_eProcessingType)
				{
					case WholeFile:
						// happens always
						emit binaryData(*m_pBuffer);
					break;
					case Blocks:
						// an unprocessed block ?.. should never happend.. but well :D
						if(m_pBuffer->size() > 0)emit binaryData(*m_pBuffer);
					break;
					case Lines:
						if(m_pBuffer->size() > 0)
						{
							// something left in the buffer and has no trailing LF
							KviStr tmp((const char *)(m_pBuffer->data()),m_pBuffer->size());
							emit data(tmp);
						}
					break;
					case StoreToFile:
						// same as above... should never happen.. but well :D
						if(m_pFile && m_pBuffer->size() > 0)m_pFile->write((const char *)(m_pBuffer->data()),m_pBuffer->size());
					break;
					default:
						// nothing... just make gcc happy
					break;
				}
				resetInternalStatus();
				m_szLastError = __tr2qs("Success");
				emit terminated(true);
				return true;
			break;
			case KVI_THREAD_EVENT_ERROR:
			{
				KviStr * err = ((KviThreadDataEvent<KviStr> *)e)->getData();
				m_szLastError = __tr2qs_no_xgettext(err->ptr());
				delete err;
				resetInternalStatus();
				emit terminated(false);
				return true;
			}
			break;
			case KVI_THREAD_EVENT_MESSAGE:
			{
				KviStr * msg = ((KviThreadDataEvent<KviStr> *)e)->getData();
				emit status(__tr2qs_no_xgettext(msg->ptr()));
				delete msg;
				return true;
			}
			break;
		}
	}
	return QObject::event(e);
}

void KviHttpRequest::emitLines(KviDataBuffer * pDataBuffer)
{
	int idx = pDataBuffer->find((const unsigned char *)"\n",1);
	while(idx != -1)
	{
		KviStr tmp((const char *)(m_pBuffer->data()),idx);
		tmp.stripRight('\r');
		pDataBuffer->remove(idx + 1);
		idx = pDataBuffer->find((const unsigned char *)"\n",1);
		emit data(tmp);
	}
}

//    header += "Accept: ";
//    QString acceptHeader = metaData("accept");
//    if (!acceptHeader.isEmpty())
//      header += acceptHeader;
//    else
//      header += DEFAULT_ACCEPT_HEADER;
//    header += "\r\n";
//
//#ifdef DO_GZIP
//    if (m_request.allowCompressedPage)
//      header += "Accept-Encoding: x-gzip, x-deflate, gzip, deflate, identity\r\n";
//#endif
//
//    if (!m_request.charsets.isEmpty())
//      header += "Accept-Charset: " + m_request.charsets + "\r\n";
//
//    if (!m_request.languages.isEmpty())
//      header += "Accept-Language: " + m_request.languages + "\r\n";
//
//
//    /* support for virtual hosts and required by HTTP 1.1 */
//    header += "Host: ";
//      header += "Pragma: no-cache\r\n"; /* for HTTP/1.0 caches */
//      header += "Cache-control: no-cache\r\n"; /* for HTTP >=1.1 caches */

//        header += "Referer: "; //Don't try to correct spelling!
//        header += m_request.referrer;
//        header += "\r\n";
bool KviHttpRequest::openFile()
{
	if(m_eProcessingType != StoreToFile)return true;

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
				int i=0;
				QString tmp = m_szFileName;
				do {
					i++;
					m_szFileName = tmp + QString(".kvirnm-%1").arg(i);
				} while(KviFile::exists(m_szFileName));
			}
			break;
			case RenameExisting:
			{
				int i=0;
				QString tmp;
				do {
					i++;
					tmp = m_szFileName + QString(".kvirnm-%1").arg(i);
				} while(KviFile::exists(tmp));
				QDir d;
				if(!d.rename(m_szFileName,tmp))
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

	m_pFile = new KviFile(m_szFileName);

	if(!m_pFile->open(QFile::WriteOnly | (bAppend ? QFile::Append : QFile::Truncate)))
	{
		resetInternalStatus();
		KviQString::sprintf(m_szLastError,__tr2qs("Can't open file \"%Q\" for writing"),&m_szFileName);
		emit terminated(false);
		return false;
	}

	return true;
}


bool KviHttpRequest::processHeader(KviStr &szHeader)
{
	int idx = szHeader.findFirstIdx("\r\n");
	KviStr szResponse;
	if(idx != -1)
	{
		szResponse = szHeader.left(idx);
		szHeader.cutLeft(idx + 2);
	} else {
		szResponse = szHeader;
		szHeader = "";
	}

	szResponse.trimmed();

	bool bValid = false;

	unsigned int uStatus = 0;

	// check the response value
	if(kvi_strEqualCSN(szResponse.ptr(),"HTTP",4))
	{
		KviStr szR = szResponse;
		szR.cutToFirst(' ');
		szR.trimmed();
		int idx = szR.findFirstIdx(' ');
		KviStr szNumber;
		if(idx != -1)szNumber = szR.left(idx);
		else szNumber = szR;
		bool bOk;
		uStatus = szNumber.toUInt(&bOk);
		if(bOk)bValid = true;
	}

	if(!bValid)
	{
		// the response is invalid ?
		resetInternalStatus();
		m_szLastError=__tr2qs("Invalid HTTP response: %s").arg(szResponse.ptr());
		emit terminated(false);
		return false;
	}

	QString tmp;
	KviQString::sprintf(tmp,__tr2qs("Received HTTP response: %s"),szResponse.ptr());

	emit status(tmp);
	emit receivedResponse(QString(szResponse.ptr()));

	KviPointerList<KviStr> hlist;
	hlist.setAutoDelete(true);

	idx = szHeader.findFirstIdx("\r\n");
	while(idx != -1)
	{
		if(idx > 0)
		{
			hlist.append(new KviStr(szHeader.ptr(),idx));
			szHeader.cutLeft(idx + 2);
		}
		idx = szHeader.findFirstIdx("\r\n");
	}
	if(szHeader.hasData())hlist.append(new KviStr(szHeader));

	KviPointerHashTable<const char *,KviStr> hdr(11,false,true);
	hdr.setAutoDelete(true);

	for(KviStr * s = hlist.first();s;s = hlist.next())
	{
		idx = s->findFirstIdx(":");
		if(idx != -1)
		{
			KviStr szName = s->left(idx);
			s->cutLeft(idx + 1);
			s->trimmed();
			hdr.replace(szName.ptr(),new KviStr(*s));
			//debug("FOUND HEADER (%s)=(%s)",szName.ptr(),s->ptr());
		}
	}

	KviStr * size = hdr.find("Content-length");
	if(size)
	{
		bool bOk;
		m_uTotalSize = size->toUInt(&bOk);
		if(!bOk)m_uTotalSize = 0;
	}

	KviStr * contentEncoding = hdr.find("Content-encoding");
	if(contentEncoding)
	{
		m_bGzip = contentEncoding->equalsCI("gzip");
	}

	KviStr * transferEncoding = hdr.find("Transfer-Encoding");
	if(transferEncoding)
	{
		if(kvi_strEqualCI(transferEncoding->ptr(),"chunked"))
		{
			// be prepared to handle the chunked transfer encoding as required by HTTP/1.1
			m_bChunkedTransferEncoding = true;
			m_uRemainingChunkSize = 0;
		}
	}

	emit header(&hdr);

	// check the status

	//				case 200: // OK
	//				case 206: // Partial content

	//				case 100: // Continue ??
	//				case 101: // Switching protocols ???
	//				case 201: // Created
	//				case 202: // Accepted
	//				case 203: // Non-Authoritative Information
	//				case 204: // No content
	//				case 205: // Reset content
	//				case 300: // Multiple choices
	//				case 301: // Moved permanently
	//				case 302: // Found
	//				case 303: // See Other
	//				case 304: // Not modified
	//				case 305: // Use Proxy
	//				case 306: // ???
	//				case 307: // Temporary Redirect
	//				case 400: // Bad request
	//				case 401: // Unauthorized
	//				case 402: // Payment Required
	//				case 403: // Forbidden
	//				case 404: // Not found
	//				case 405: // Method not allowed
	//				case 406: // Not acceptable
	//				case 407: // Proxy authentication required
	//				case 408: // Request timeout
	//				case 409: // Conflict
	//				case 410: // Gone
	//				case 411: // Length required
	//				case 412: // Precondition failed
	//				case 413: // Request entity too large
	//				case 414: // Request-URI Too Long
	//				case 415: // Unsupported media type
	//				case 416: // Requested range not satisfiable
	//				case 417: // Expectation Failed
	//				case 500: // Internal server error
	//				case 501: // Not implemented
	//				case 502: // Bad gateway
	//				case 503: // Service unavailable
	//				case 504: // Gateway timeout
	//				case 505: // HTTP Version not supported

	if((uStatus != 200) && (uStatus != 206))
	{
		// this is not "OK" and not "Partial content"
		// Error , redirect or something confusing
		if(m_eProcessingType != HeadersOnly)
		{
			// this is an error then
			resetInternalStatus();
			m_szLastError = szResponse.ptr();
			emit terminated(false);
			return false;
		} // else the server will terminate (it was a HEAD request)
	}

	if((m_uMaxContentLength > 0) && (m_uTotalSize > ((unsigned int)m_uMaxContentLength)))
	{
		resetInternalStatus();
		m_szLastError=__tr2qs("Stream exceeding maximum length");
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
		m_pBuffer->append(*data);

		int idx = m_pBuffer->find((const unsigned char *)"\r\n\r\n",4);
		if(idx == -1)
		{
			// header not complete
			if(m_pBuffer->size() > 4096)
			{
				resetInternalStatus();
				m_szLastError = __tr2qs("Header too long: exceeded 4096 bytes");
				emit terminated(false);
			}
			return;
		}
		KviStr szHeader((const char *)(m_pBuffer->data()),idx);
		m_pBuffer->remove(idx + 4);

		if(!processHeader(szHeader))return;
		m_bHeaderProcessed = true;

		if(m_eProcessingType == StoreToFile)
		{
			if(!openFile())return;
		}

		m_uReceivedSize = m_pBuffer->size();


		// here the header is complete and the eventual remaining data is in m_pBuffer. data has been already used.

	} else {
		// header already processed
		m_uReceivedSize += data->size();

		// here the header is complete and some data *might* be already in m_pBuffer. data is unused yet.

		// Optimisation: If the transfer is NOT chunked (so we don't have to parse it)
		// and the requested processing type is either Blocks or StoreToFile
		// then we just can avoid to copy the data to m_pBuffer.
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
					m_pFile->write((const char *)(data->data()),data->size());
				break;
				default:
				break;
			}

			if(((m_uTotalSize > 0) && (m_uReceivedSize > m_uTotalSize)) || ((m_uMaxContentLength > 0) && (m_uReceivedSize > m_uMaxContentLength)))
			{
				resetInternalStatus();
				m_szLastError=__tr2qs("Stream exceeded expected length");
				emit terminated(false);
			}

			return;
		}

		// need to append to m_pBuffer and process it
		m_pBuffer->append(*data);
	}

	// we're processing data in m_pBuffer here
	if(m_bChunkedTransferEncoding)
	{
		// The transfer encoding is chunked: the buffer contains
		// chunks of data with an initial header composed
		// of a hexadecimal length, an optional bullshit and a single CRLF
		// The transfer terminates when we read a last chunk of size 0
		// that may be followed by optional headers...
		// This sux :)
		while(m_pBuffer->size() > 0) // <-- note that we may exit from this loop also for other conditions (there is a goto below)
		{
			// we process chunks of parts of chunks at a time.
			if(m_uRemainingChunkSize > 0)
			{
				// process the current chunk data
				unsigned int uProcessSize = m_uRemainingChunkSize;
				if(uProcessSize > (unsigned int)m_pBuffer->size())uProcessSize = m_pBuffer->size();
				m_uRemainingChunkSize -= uProcessSize;

				switch(m_eProcessingType)
				{
					case Blocks:
						if((unsigned int)m_pBuffer->size() == uProcessSize)
						{
							// avoid copying to a new buffer
							emit binaryData(*m_pBuffer);
						} else {
							// must copy
							KviDataBuffer tmp(uProcessSize,m_pBuffer->data());
							emit binaryData(tmp);
							m_pBuffer->remove(uProcessSize);
						}
					break;
					case Lines:
						if((unsigned int)m_pBuffer->size() == uProcessSize)
						{
							// avoid copying to a new buffer
							emitLines(m_pBuffer);
						} else {
							// must copy
							KviDataBuffer tmp(uProcessSize,m_pBuffer->data());
							emitLines(&tmp);
							m_pBuffer->remove(uProcessSize);
						}
					break;
					case StoreToFile:
						m_pFile->write((const char *)(m_pBuffer->data()),uProcessSize);
						m_pBuffer->remove(uProcessSize);
					break;
					default:
						// nothing.. just make gcc happy
					break;
				}
				// now either the buffer is empty or there is another chunk header: continue looping
			} else {
				// We're looking for the beginning of a chunk now.
				// Note that we might be at the end of a previous chunk that has a CRLF terminator
				// we need to skip it.
				int crlf = m_pBuffer->find((const unsigned char *)"\r\n",2);
				if(crlf != -1)
				{
					if(crlf == 0)
					{
						// This is a plain CRLF at the beginning of the buffer BEFORE a chunk header.
						// It comes from the previous chunk terminator. Skip it.
						m_pBuffer->remove(2);
					} else {
						// got a chunk header
						KviStr szHeader((const char *)(m_pBuffer->data()),crlf);
						szHeader.cutFromFirst(' ');
						// now szHeader should contain a hexadecimal chunk length... (why the hell it is hex and not decimal ????)
						QString szHexHeader = szHeader.ptr();
						bool bOk;
						m_uRemainingChunkSize = szHexHeader.toLong(&bOk,16);
						if(!bOk)
						{
							resetInternalStatus();
							m_szLastError = __tr2qs("Protocol error: invalid chunk size");
							emit terminated(false);
							return;
						}
						m_pBuffer->remove(crlf+2);
						if(m_uRemainingChunkSize == 0)
						{
							// this is the last chunk of data. It may be followed by optional headers
							// but we actually don't need them (since we're surely not in HEAD mode)
							m_bIgnoreRemainingData = true;
							m_pBuffer->clear();
							goto check_stream_length;
						}
					}
					// the rest is valid data of a non-zero chunk: continue looping
				} else {
					// chunk header not complete
					if(m_pBuffer->size() > 4096)
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
	} else {
		// the transfer encoding is not chunked: m_pBuffer contains only valid data
		switch(m_eProcessingType)
		{
			case Blocks:
				if(m_pBuffer->size() > 0)emit binaryData(*m_pBuffer);
				m_pBuffer->clear();
			break;
			case Lines:
				if(m_pBuffer->size() > 0)emitLines(m_pBuffer);
			break;
			case StoreToFile:
				m_pFile->write((const char *)(m_pBuffer->data()),m_pBuffer->size());
				m_pBuffer->clear();
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
		m_szLastError=__tr2qs("Stream exceeded expected length");
		emit terminated(false);
	}
	return;
}

KviHttpRequestThread::KviHttpRequestThread(
		KviHttpRequest * r,
		const QString &szHost,
		const QString &szIp,
		unsigned short uPort,
		const QString & szPath,
		unsigned int uContentOffset,
		RequestMethod m,
		const QString &szPostData,
		bool bUseSSL
	) : KviSensitiveThread()
{
	m_pRequest = r;
	m_szHost = szHost;
	m_szIp = szIp;
	m_szPath = szPath;
	m_uPort = uPort > 0 ? uPort : 80;
	m_uContentOffset = uContentOffset;
	m_eRequestMethod = m;
	m_szPostData = szPostData;
	m_sock = KVI_INVALID_SOCKET;
	m_bUseSSL = bUseSSL;
	m_uConnectionTimeout = 60;
#ifdef COMPILE_SSL_SUPPORT
	m_pSSL = 0;
#endif
}

KviHttpRequestThread::~KviHttpRequestThread()
{
}

bool KviHttpRequestThread::processInternalEvents()
{
	while(KviThreadEvent *e = dequeueEvent())
	{
		switch(e->id())
		{
			case KVI_THREAD_EVENT_TERMINATE:
			{
				delete e;
				return false;
			}
			break;
			default:
				debug("Unrecognized event in http thread");
				delete e;
				return false;
			break;
		}
	}

	return true;
}

bool KviHttpRequestThread::failure(const char *error)
{
	if(error)
	{
		postEvent(m_pRequest,new KviThreadDataEvent<KviStr>(KVI_THREAD_EVENT_ERROR,new KviStr(error)));
	} /*else {
		postEvent(m_pRequest,new KviThreadDataEvent<KviStr>(KVI_THREAD_EVENT_ERROR,new KviStr(__tr2qs("Aborted"))));
	}*/
	return false;
}

bool KviHttpRequestThread::selectForWrite(int iTimeoutInSecs)
{

	kvi_time_t startTime = kvi_unixTime();

	for(;;)
	{
		if(!processInternalEvents())
		{
			return failure(0);
		}

		fd_set writeSet;

		FD_ZERO(&writeSet);

		FD_SET(m_sock,&writeSet);

		struct timeval tmv;
		tmv.tv_sec  = 0;
		tmv.tv_usec = 1000; // we wait 1000 usecs for an event


		int nRet = kvi_socket_select(m_sock + 1,0,&writeSet,0,&tmv);

		if(nRet > 0)
		{
			if(FD_ISSET(m_sock,&writeSet))
			{
				// connected!
				return true;
			}
		} else {
			if(nRet < 0)
			{
				int err = kvi_socket_error();
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
				if((err != EAGAIN) && (err != EINTR) && (err != WSAEWOULDBLOCK))
#else
				if((err != EAGAIN) && (err != EINTR))
#endif
				{
					return failure(KviError::getUntranslatedDescription(KviError::translateSystemError(err)));
				}
			}
		}


		if((time(0) - startTime) > iTimeoutInSecs)
			return failure(__tr_no_lookup("Operation timed out"));

		usleep(100000); // 1/10 sec
	}

	return false;
}

bool KviHttpRequestThread::sslFailure()
{
#ifdef COMPILE_SSL_SUPPORT
	KviStr buffer;
	if(m_pSSL->getLastErrorString(buffer))
	{
		failure(buffer.ptr());
	} else {
		failure(__tr_no_lookup("Unexpected SSL error"));
	}
#endif
	return false;
}

bool KviHttpRequestThread::connectToRemoteHost()
{
	m_sock = kvi_socket_create(KVI_SOCKET_PF_INET,KVI_SOCKET_TYPE_STREAM,0); //tcp
	if(m_sock == KVI_INVALID_SOCKET)
		return failure(__tr_no_lookup("Failed to create the socket"));

	if(!kvi_socket_setNonBlocking(m_sock))
		return failure(__tr_no_lookup("Failed to enter non blocking mode"));

	sockaddr_in saddr;

	if(!KviNetUtils::stringIpToBinaryIp(m_szIp,&(saddr.sin_addr)))
		return failure(__tr_no_lookup("Invalid target address"));

	saddr.sin_port = htons(m_uPort);
	saddr.sin_family = AF_INET;

	if(!kvi_socket_connect(m_sock,(struct sockaddr *)&saddr,sizeof(saddr)))
	{
		int err = kvi_socket_error();
		if(!kvi_socket_recoverableConnectError(err))
		{
			return failure(KviError::getUntranslatedDescription(KviError::translateSystemError(err)));
		}
	}

	// now loop selecting for write

	if(!selectForWrite(m_uConnectionTimeout))
		return false;

	int sockError;
	int iSize=sizeof(sockError);
	if(!kvi_socket_getsockopt(m_sock,SOL_SOCKET,SO_ERROR,(void *)&sockError,&iSize))sockError = -1;
	if(sockError != 0)
	{
		//failed
		if(sockError > 0)sockError = KviError::translateSystemError(sockError);
		else sockError = KviError_unknownError;
		return failure(KviError::getUntranslatedDescription(sockError));
	}

#ifdef COMPILE_SSL_SUPPORT
	if(m_bUseSSL)
	{
		m_pSSL = new KviSSL();
		if(!m_pSSL->initContext(KviSSL::Client))
			return failure(__tr_no_lookup("Failed to initialize the SSL context"));
		if(!m_pSSL->initSocket(m_sock))
			return failure(__tr_no_lookup("Failed to initialize the SSL connection"));

		for(;;)
		{
			switch(m_pSSL->connect())
			{
				case KviSSL::Success:
					// done: connected.
					return true;
				break;
				case KviSSL::WantRead:
					if(!selectForRead(m_uConnectionTimeout))
						return false;
				break;
				case KviSSL::WantWrite:
					if(!selectForWrite(m_uConnectionTimeout))
						return false;
				break;
				case KviSSL::RemoteEndClosedConnection:
					return failure(__tr_no_lookup("Remote end has closed the connection"));
				break;
				case KviSSL::SSLError:
					return sslFailure();
				break;
				case KviSSL::SyscallError:
				{
					// syscall problem
					int err = kvi_socket_error();
					if(!kvi_socket_recoverableError(err))
					{
						// Declare problems :)
						return failure(__tr_no_lookup("Unrecoverable SSL error during handshake"));
					} // else can recover ? (EAGAIN , EINTR ?) ... should select for read or for write
				}
				break;
				default:
					return sslFailure();
				break;
			}
		}

		// never here
		return true;
	}
#endif

	return true;
}


bool KviHttpRequestThread::sendBuffer(const char * buffer,int bufLen,int iTimeoutInSecs)
{
	const char * ptr = buffer;
	int curLen       = bufLen;

	time_t startTime = time(0);

	for(;;)
	{
		if(!processInternalEvents())
			return failure();

		int wrtn;
#ifdef COMPILE_SSL_SUPPORT
		if(m_pSSL)
		{
			wrtn = m_pSSL->write((char *)ptr,curLen);
		} else {
#endif
			wrtn = kvi_socket_send(m_sock,ptr,curLen);
#ifdef COMPILE_SSL_SUPPORT
		}
#endif

		if(wrtn > 0)
		{
			curLen -= wrtn;

			if(curLen <= 0)
				break;

			ptr += wrtn;
		} else {
			if(wrtn < 0)
			{
#ifdef COMPILE_SSL_SUPPORT
				if(m_pSSL)
				{
					// ops...might be an SSL error
					switch(m_pSSL->getProtocolError(wrtn))
					{
						case KviSSL::WantWrite:
							if(!selectForWrite(m_uConnectionTimeout))
								return false;
							break;
						case KviSSL::WantRead:
							if(!selectForRead(m_uConnectionTimeout))
								return false;
							break;
						case KviSSL::SyscallError:
							if(wrtn == 0)
							{
								return failure(__tr_no_lookup("Remote end has closed the connection"));
							} else {
								int iSSLErr = m_pSSL->getLastError(true);
								if(iSSLErr != 0)
								{
									return sslFailure();
								} else {
									goto handle_system_error;
								}
							}
						break;
						case KviSSL::SSLError:
							return sslFailure();
						break;
						default:
							return sslFailure();
						break;
					}
				} else {
#endif //COMPILE_SSL_SUPPORT

handle_system_error:
					int err = kvi_socket_error();
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
					if((err != EAGAIN) && (err != EINTR) && (err != WSAEWOULDBLOCK))
#else
					if((err != EAGAIN) && (err != EINTR))
#endif
					{
						return failure(KviError::getUntranslatedDescription(KviError::translateSystemError(err)));
					}
#ifdef COMPILE_SSL_SUPPORT
				}
#endif
			}
		}

		int diff = time(0) - startTime;
		if(diff > iTimeoutInSecs)
			return failure(__tr_no_lookup("Operation timed out"));

		usleep(10000);
	}

	return true;
}

int KviHttpRequestThread::selectForReadStep()
{
	// calls select on the main socket
	// returns 1 if there is data available for reading
	// returns 0 if there is no data available but there was no error
	// returns -1 if there was a critical error (socket closed)
	fd_set readSet;

	FD_ZERO(&readSet);

	FD_SET(m_sock,&readSet);

	struct timeval tmv;
	tmv.tv_sec  = 0;
	tmv.tv_usec = 1000; // we wait 1000 usecs for an event


	int nRet = kvi_socket_select(m_sock + 1,&readSet,0,0,&tmv);

	if(nRet > 0)
	{
		if(FD_ISSET(m_sock,&readSet))
		{
			// ok
			return 1;
		}
	} else {
		if(nRet < 0)
		{
			int err = kvi_socket_error();
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
			if((err != EAGAIN) && (err != EINTR) && (err != WSAEWOULDBLOCK))
#else
			if((err != EAGAIN) && (err != EINTR))
#endif
			{
				failure(KviError::getUntranslatedDescription(KviError::translateSystemError(err)));
				return -1;
			}
		}
	}

	return 0;
}

bool KviHttpRequestThread::selectForRead(int iTimeoutInSecs)
{
	// waits for some data to arrive on the socket
	// up to iTimeoutInSecs seconds
	// returns true if data is available on the socket
	// or false if there was a select() error or no data
	// was available in the specified amount of time

	time_t startTime = time(0);

	for(;;)
	{
		if(!processInternalEvents())
		{
			return failure(); // ensure that the socket is closed
		}

		int nRet = selectForReadStep();

		if(nRet < 0)return false;
		if(nRet > 0)return true;

		int diff = time(0) - startTime;
		if(diff > iTimeoutInSecs)
			return failure(__tr_no_lookup("Operation timed out (while selecting for read)"));

		usleep(100000); // 1/10 sec
	}

	return false;
}

bool KviHttpRequestThread::readDataStep()
{
	unsigned char buffer[2048];
	int readed;


#ifdef COMPILE_SSL_SUPPORT
	if(m_pSSL)
	{
		readed = m_pSSL->read((char *)buffer,2048);
		if(readed <= 0)
		{
			// ssl error....?
			switch(m_pSSL->getProtocolError(readed))
			{
				case KviSSL::ZeroReturn:
					readed = 0;
				break;
				case KviSSL::WantRead:
					return selectForRead(m_uConnectionTimeout);
				break;
				case KviSSL::WantWrite:
					return selectForWrite(m_uConnectionTimeout);
				break;
				case KviSSL::SyscallError:
				{
					int iE = m_pSSL->getLastError(true);
					if(iE != 0)return sslFailure();
				}
				break;
				case KviSSL::SSLError:
					return sslFailure();
				break;
				default:
					return sslFailure();
				break;
			}
		}
	} else {
#endif
		readed = kvi_socket_read(m_sock,buffer,2048);
#ifdef COMPILE_SSL_SUPPORT
	}
#endif

	if(readed > 0)
	{
		postEvent(m_pRequest,new KviThreadDataEvent<KviDataBuffer>(KVI_THREAD_EVENT_BINARYDATA,new KviDataBuffer(readed,buffer)));
	} else {
		if(readed < 0)
		{
			// Read error ?
			int err = kvi_socket_error();
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
			if((err != EAGAIN) && (err != EINTR) && (err != WSAEWOULDBLOCK))
#else
			if((err != EAGAIN) && (err != EINTR))
#endif
			{
				// yes...read error
				return failure(KviError::getUntranslatedDescription(KviError::translateSystemError(err)));
			}
			return selectForRead(m_uConnectionTimeout); // EINTR or EAGAIN...transient problem
		} else {
			// readed == 0
			// Connection closed by remote host
			postEvent(m_pRequest,new KviThreadEvent(KVI_THREAD_EVENT_SUCCESS));
			return false;
		}
	}
	return selectForRead(m_uConnectionTimeout);
}

void KviHttpRequestThread::run()
{
	// setup:
	//    nothing needed

	// run:
	runInternal();

	// cleanup:
#ifdef COMPILE_SSL_SUPPORT
	if(m_pSSL)
	{
		delete m_pSSL;
		m_pSSL = 0;
	}
#endif

	if(kvi_socket_isValid(m_sock))
	{
		kvi_socket_close(m_sock);
		m_sock = KVI_INVALID_SOCKET;
	}
}

void KviHttpRequestThread::runInternal()
{
#ifndef COMPILE_SSL_SUPPORT
	if(m_bUseSSL)
	{
		failure(__tr_no_lookup("This KVIrc executable has no SSL support"));
		return;
	}
#endif

	if(!connectToRemoteHost())return;

	postEvent(m_pRequest,new KviThreadEvent(KVI_HTTP_REQUEST_THREAD_EVENT_CONNECTED));

	// FIXME: Other headers ?

	KviStr szMethod;
	switch(m_eRequestMethod)
	{
		case Head: szMethod = "HEAD"; break;
		case Post: szMethod = "POST"; break;
		case Get: szMethod = "GET"; break;
	}

	KviStr szRequest(KviStr::Format,"%s %s HTTP/1.1\r\n" \
				"Host: %s\r\n" \
				"Connection: Close\r\n" \
				"User-Agent: KVIrc-http-slave/1.0.0\r\n" \
				"Accept: */*\r\n",
				szMethod.ptr(),KviQString::toUtf8(m_szPath).data(),KviQString::toUtf8(m_szHost).data());

	if(m_uContentOffset > 0)
		szRequest.append(KviStr::Format,"Range: bytes=%u-\r\n",m_uContentOffset);

	if(m_eRequestMethod == Post)
	{
		szRequest.append(KviStr::Format,"Content-Type: application/x-www-form-urlencoded\r\n" \
				"Content-Length: %u\r\n" \
				"Cache-control: no-cache\r\n" \
				"Pragma: no-cache\r\n",m_szPostData.length());
	}

	szRequest += "\r\n";

	if(m_eRequestMethod == Post)
	{
		if(!m_szPostData.isEmpty())
			szRequest.append(m_szPostData);
		szRequest += "\r\n";
	}

	//debug("SENDING REQUEST:\n%s",szRequest.ptr());

	if(!sendBuffer(szRequest.ptr(),szRequest.len(),60))return;

	// now loop reading data
	postEvent(m_pRequest,new KviThreadDataEvent<QString>(KVI_HTTP_REQUEST_THREAD_EVENT_REQUESTSENT,new QString(szRequest)));

	for(;;)
	{
		if(!readDataStep())return;
	}
}
