#ifndef _KVI_HTTP_H_
#define _KVI_HTTP_H_
//=============================================================================
//
//   File : kvi_http.h
//   Creation date : Sat Aug 17 13:43:31 2002 GMT by Szymon Stefanek
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

#include "kvi_settings.h"
#include "kvi_heapobject.h"
#include "kvi_string.h"
#include "kvi_thread.h"
#include "kvi_sockettype.h"
#include "kvi_databuffer.h"
#include "kvi_inttypes.h"
#include "kvi_url.h"
#include "kvi_pointerhashtable.h"
#include "kvi_file.h"

#include <QObject>
#include <QStringList>

class KviDns;
class KviSSL;
class KviHttpRequestThread;

//
// This class implements a HTTP protocol client.
// It's able to send GET, POST and HEAD requests,
// download stuff to a file or to a qt SLOT().
//

class KVILIB_API KviHttpRequest : public QObject, public KviHeapObject
{
	Q_OBJECT
public:
	enum ProcessingType
	{
		HeadersOnly,    // Download headers only (HEAD request)
		WholeFile,      // Emit the data as whole file (binaryData() is emitted)
		Blocks,         // Emit the data as blocks (binaryData() is emitted)
		Lines,          // Emit the data as ASCII text lines (the client must take care of decoding the data)
		StoreToFile     // Store the data to a file
	};
	enum ExistingFileAction
	{
		Overwrite,      // Overwrite existing file
		RenameIncoming, // Automatically rename the incoming file
		RenameExisting, // Automatically rename the existing file
		Resume          // Attempt to resume the file (get partial content)
	};
public:
	KviHttpRequest();
	~KviHttpRequest();
protected:
	// data
	KviUrl                 m_url;
	QString                m_szFileName;
	ProcessingType         m_eProcessingType;
	ExistingFileAction     m_eExistingFileAction;
	void                 * m_pPrivateData;
	unsigned int           m_uMaxContentLength;
	unsigned int           m_uContentOffset;
	QString                m_szPostData;
	// status
	QString                m_szLastError;
	unsigned int           m_uTotalSize;
	unsigned int           m_uReceivedSize;
	// internal status
	QString                m_szIp;
	KviDns               * m_pDns;
	KviHttpRequestThread * m_pThread;
	KviDataBuffer        * m_pBuffer;
	bool                   m_bHeaderProcessed;
	bool                   m_bChunkedTransferEncoding;
	bool                   m_bGzip;
	unsigned int           m_uRemainingChunkSize;
	bool                   m_bIgnoreRemainingData; // used in chunked transfer after the last chunk has been seen
	KviFile              * m_pFile;
	unsigned int           m_uConnectionTimeout; // in seconds, 60 secs by default
protected:
	bool startDnsLookup();
	virtual bool event(QEvent *e);
	void processData(KviDataBuffer * data);
	bool processHeader(KviStr &szHeader);
	bool openFile();
	void emitLines(KviDataBuffer * pDataBuffer);

	void resetStatus();
	void resetData();
	void resetInternalStatus();
protected slots:
	void dnsLookupDone(KviDns *d);
	void haveServerIp();
public:
	void setConnectionTimeout(unsigned int uConnectionTimeout)
	{
		m_uConnectionTimeout = uConnectionTimeout;
		if(m_uConnectionTimeout < 5)
			m_uConnectionTimeout = 5; // keep it sane
	}

	const KviUrl & url(){ return m_url; };
	ProcessingType processingType(){ return m_eProcessingType; };
	ExistingFileAction existingFileAction(){ return m_eExistingFileAction; };
	const QString &fileName(){ return m_szFileName; };
	void * privateData(){ return m_pPrivateData; };
	unsigned int maxContentLength(){ return m_uMaxContentLength; };
	unsigned int contentOffset(){ return m_uContentOffset; };
	unsigned int totalSize(){ return m_uTotalSize; };
	unsigned int receivedSize(){ return m_uReceivedSize; };

	void reset();

	void setPostData(const QString &szPostData){ m_szPostData = szPostData; };
	void setUrl(const KviUrl &u){ m_url = u; };
	void setProcessingType(ProcessingType t){ m_eProcessingType = t; };
	void setExistingFileAction(ExistingFileAction a){ m_eExistingFileAction = a; };
	void setFileName(const QString &szFileName){ m_szFileName = szFileName; };
	void setPrivateData(void * ptr){ m_pPrivateData = ptr; };
	void setMaxContentLength(int uMaxContentLength){ m_uMaxContentLength = uMaxContentLength; }; //0 means unlimited
	// this will work regardless of ExistingFileAction : even if the file doesn't exist
	void setContentOffset(int uContentOffset){ m_uContentOffset = uContentOffset; };

	bool start();

	// this is a shortcut for reset()+setUrl()+setProcessingType()+setFileName()+start()
	bool get(const KviUrl &u,ProcessingType p = WholeFile,const QString &szFileName = QString());

	const QString & lastError(){ return m_szLastError; };

	void abort();
signals:
	void resolvingHost(const QString &hostname);
	void contactingHost(const QString &ipandport);
	void connectionEstabilished();
	void receivedResponse(const QString &response);

	void terminated(bool bSuccess);


	void status(const QString &message);
	void data(const KviStr &data);
	void binaryData(const KviDataBuffer &data);
	void header(KviPointerHashTable<const char *,KviStr> * hdr);
	void requestSent(const QStringList &request);
};


class KviHttpRequestThread : public KviSensitiveThread
{
	friend class KviHttpRequest;
public:
	enum RequestMethod { Post, Get , Head };
protected:
	KviHttpRequestThread(KviHttpRequest * r,
			const QString &szHost,
			const QString &szIp,
			unsigned short uPort,
			const QString &szPath,
			unsigned int uContentOffset,
			RequestMethod m,
			const QString &szPostData = QString(),
			bool bUseSSL = false
		);

public:
	~KviHttpRequestThread();
protected:
	KviHttpRequest * m_pRequest;

	QString           m_szHost;
	QString          m_szIp;
	QString          m_szPath;
	unsigned int     m_uContentOffset;
	RequestMethod    m_eRequestMethod;
	QString          m_szPostData;

	unsigned short   m_uPort;
	kvi_socket_t     m_sock;
	bool             m_bUseSSL;
	unsigned int     m_uConnectionTimeout;
#ifdef COMPILE_SSL_SUPPORT
	KviSSL         * m_pSSL;
#endif
protected:
	void setConnectionTimeout(unsigned int uTimeout)
	{
		m_uConnectionTimeout = uTimeout;
		if(m_uConnectionTimeout < 5)
			m_uConnectionTimeout = 5; // keep it sane
	}
	int selectForReadStep();
	bool selectForRead(int iTimeoutInSecs);
	bool readDataStep();
	bool sendBuffer(const char *buffer,int bufLen,int iTimeoutInSecs);
	bool failure(const char *error=0);
	bool sslFailure();
	bool selectForWrite(int iTimeoutInSecs);
	bool connectToRemoteHost();
	bool processInternalEvents();
	void runInternal();
	virtual void run();
};


#endif //_KVI_HTTP_H_
