#ifndef _KviHttpRequest_h_
#define _KviHttpRequest_h_
//=============================================================================
//
//   File : KviHttpRequest.h
//   Creation date : Sat Aug 17 13:43:31 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_inttypes.h"

#include "KviHeapObject.h"
#include "KviCString.h"
#include "KviUrl.h"
#include "KviPointerHashTable.h"

#include <QObject>
#include <QAbstractSocket>

class KviDnsResolver;
class KviDataBuffer;


//
// This class implements a HTTP protocol client.
// It's able to send GET, POST and HEAD requests,
// download stuff to a file or to a qt SLOT().
//

// FIXME: Document and hide internals.

class KviHttpRequestPrivate;

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

	virtual ~KviHttpRequest();

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
	bool                   m_bHeaderProcessed;
	bool                   m_bChunkedTransferEncoding;
	bool                   m_bGzip;
	unsigned int           m_uRemainingChunkSize;
	bool                   m_bIgnoreRemainingData; // used in chunked transfer after the last chunk has been seen
	unsigned int           m_uConnectionTimeout; // in seconds, 60 secs by default

private:

	KviHttpRequestPrivate * m_p;

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
	void data(const KviCString &data);
	void binaryData(const KviDataBuffer &data);
	void header(KviPointerHashTable<const char *,KviCString> * hdr);
	void requestSent(const QStringList &request);

private:
	void closeSocket();
	bool doConnect();

	void processData(KviDataBuffer * data);
	bool processHeader(KviCString &szHeader);
	bool openFile();
	void emitLines(KviDataBuffer * pDataBuffer);

	void resetStatus();
	void resetData();
	void resetInternalStatus();

private slots:
	void slotSocketReadDataReady();
	void slotSocketDisconnected();
	void slotSocketConnected();
	void slotSocketError(QAbstractSocket::SocketError socketError);
	void slotConnectionTimedOut();
	void slotSocketHostResolved();
};


#endif //_KviHttpRequest_h_
