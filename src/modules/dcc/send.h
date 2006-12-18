#ifndef _SEND_H_
#define _SEND_H_
//=============================================================================
//
//   File : send.h
//   Creation date : Tue Sep 24 09 2000 15:06:12 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2005 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_window.h"
#include "kvi_string.h"

#include "descriptor.h"
#include "window.h"
#include "thread.h"

#include "kvi_sockettype.h"

#include "kvi_list.h"
#include <qlabel.h>
#include <qprogressbar.h>
#include <qpopupmenu.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qfile.h>
#include <qdialog.h>

#include "kvi_filetransfer.h"
#include "kvi_time.h"


typedef struct _KviDccSendThreadOptions
{
	KviStr       szFileName;
	int          iStartPosition;
	int          iPacketSize;
	int          iIdleStepLengthInMSec;
	bool         bFastSend;
	bool         bNoAcks;
	bool         bIsTdcc;
	unsigned int uMaxBandwidth;
} KviDccSendThreadOptions;


class KviDccSendThread : public KviDccThread
{
public:
	KviDccSendThread(QObject * par,kvi_socket_t fd,KviDccSendThreadOptions * opt);
	~KviDccSendThread();
private:
	// stats: SHARED!!!
	int            m_iAverageSpeed;
	int            m_iInstantSpeed;
	int            m_iFilePosition;
	int            m_iAckedBytes;
	int            m_iTotalSentBytes;
	// internal
	unsigned long  m_uStartTime;
	unsigned long  m_uInstantSpeedInterval;
	int            m_iInstantSentBytes;
	KviDccSendThreadOptions * m_pOpt;
	KviMSecTimeInterval * m_pTimeInterval;             // used for computing the instant bandwidth but not only
public:
	void initGetInfo();
	int averageSpeed(){ return m_iAverageSpeed; };
	int instantSpeed(){ return m_iInstantSpeed; };
	int filePosition(){ return m_iFilePosition; };
	// sent ONLY in this session
	int sentBytes(){ return m_iTotalSentBytes; };
	int ackedBytes(){ return m_iAckedBytes; };
	unsigned int bandwidthLimit(){ return m_pOpt->uMaxBandwidth; };
	void setBandwidthLimit(unsigned int uMaxBandwidth){ m_pOpt->uMaxBandwidth = uMaxBandwidth; };
	void doneGetInfo();
protected:
	void updateStats();
	virtual void run();
};

typedef struct _KviDccRecvThreadOptions
{
	bool         bResume;
	KviStr       szFileName;
	int          iTotalFileSize;
	int          iIdleStepLengthInMSec;
	bool         bSendZeroAck;
	bool         bNoAcks;
	bool         bIsTdcc;
	unsigned int uMaxBandwidth;
} KviDccRecvThreadOptions;

class KviDccRecvThread : public KviDccThread
{
public:
	KviDccRecvThread(QObject * par,kvi_socket_t fd,KviDccRecvThreadOptions * opt);
	~KviDccRecvThread();
protected:
	KviDccRecvThreadOptions * m_pOpt;

	// stats: SHARED!
	int                   m_iAverageSpeed;
	int                   m_iInstantSpeed;
	int                   m_iFilePosition;
	int                   m_iTotalReceivedBytes;

	// internal
	unsigned long         m_uStartTime;
	KviMSecTimeInterval * m_pTimeInterval;             // used for computing the instant bandwidth
	int                   m_iInstantReceivedBytes;
	unsigned long         m_uInstantSpeedInterval;
	QFile               * m_pFile;
public:
	void initGetInfo();
	int averageSpeed(){ return m_iAverageSpeed; };
	int instantSpeed(){ return m_iInstantSpeed; };
	int filePosition(){ return m_iFilePosition; };
	// received ONLY in this session
	int receivedBytes(){ return m_iTotalReceivedBytes; };
	unsigned int bandwidthLimit(){ return m_pOpt->uMaxBandwidth; };
	void setBandwidthLimit(unsigned int uMaxBandwidth){ m_pOpt->uMaxBandwidth = uMaxBandwidth; };
	void doneGetInfo();
protected:
	void postMessageEvent(const char * msg);
	void updateStats();
	bool sendAck(int filePos);
	virtual void run();
};

class KviDccFileTransfer;
class KviStyledCheckBox;
class QSpinBox;
class QTimer;

class KviDccFileTransferBandwidthDialog : public QDialog
{
	Q_OBJECT
public:
	KviDccFileTransferBandwidthDialog(QWidget * pParent,KviDccFileTransfer * t);
	~KviDccFileTransferBandwidthDialog();
protected:
	KviDccFileTransfer * m_pTransfer;
	KviStyledCheckBox * m_pEnableLimitCheck;
	QSpinBox * m_pLimitBox;
protected:
	virtual void closeEvent(QCloseEvent *e);
protected slots:
	void okClicked();
	void cancelClicked();
};


class KviDccMarshal;
class QPainter;
class QPopupMenu;

class KviDccFileTransfer : public KviFileTransfer, public KviDccMarshalOutputContext
{
	enum GeneralStatus { Connecting , Transferring , Success , Failure };
	Q_OBJECT
public:
	KviDccFileTransfer(KviDccDescriptor * dcc);
	~KviDccFileTransfer();
private:
	KviDccSendThread       * m_pSlaveSendThread;
	KviDccRecvThread       * m_pSlaveRecvThread;
	KviDccDescriptor       * m_pDescriptor;
	KviDccMarshal          * m_pMarshal;

	KviStr                   m_szTarget;
	KviStr                   m_szDccType;
	QString                  m_szTransferIdString;

	QString                  m_szStatusString;
	GeneralStatus            m_eGeneralStatus;

	QString                  m_szTransferLog; // html

	kvi_time_t               m_tTransferStartTime;
	kvi_time_t               m_tTransferEndTime;
	// cached stats
	unsigned int             m_uTotalFileSize; // total file size to transfer
	
	unsigned int             m_uMaxBandwidth;
	KviDccFileTransferBandwidthDialog * m_pBandwidthDialog;
	
	QTimer                 * m_pResumeTimer; // used to signal resume timeout
public:
	bool resumeAccepted(const char * filename,const char * port,const char *szZeroPortTag);
	bool doResume(const char * filename,const char * port,unsigned int filePos);

	static void init();
	static void done();
	static unsigned int runningTransfersCount();
	static KviDccFileTransfer * nonFailedTransferWithLocalFileName(const QString &szLocalFileName);
	static unsigned int transferCount();
	static bool handleResumeAccepted(const char * filename,const char * port,const char * szZeroPortTag);
	static bool handleResumeRequest(const char * filename,const char * port,unsigned int filePos);

	virtual bool event(QEvent *e);

	virtual KviWindow * dccMarshalOutputWindow();
	virtual const char * dccMarshalOutputContextString();

	virtual void displayPaint(QPainter * p,int column,int width,int height);
	virtual int displayHeight(int iLineSpacing);
	virtual void fillContextPopup(QPopupMenu * m,int column);
	virtual void fillStatusString(QString &szBuffer);
	virtual bool active();
	virtual void die();
	virtual QString tipText();
	virtual QString localFileName();
	
	bool isFileUpload(){ return m_pDescriptor->isFileUpload(); };

	unsigned int averageSpeed();
	unsigned int transferredBytes();
	
	int bandwidthLimit();
	void setBandwidthLimit(int iVal);
protected:
	void startConnection();
	void listenOrConnect();
	void addToTransferLog(const QString &s);
	void outputAndLog(const QString &s);
	void outputAndLog(int msgtype,const QString &s);
	KviWindow * eventWindow();
protected slots:
	void connectionInProgress();
	void sslError(const char * msg);
	void startingSSLHandshake();
	void handleMarshalError(int err);
	void connected();
	void bandwidthDialogDestroyed();
	void configureBandwidth();
	void resumeTimedOut();
public slots:
	void abort();
	void retryDCC();
	void retryTDCC();
	void retryRevDCC();
};

#endif //_SEND_H_
