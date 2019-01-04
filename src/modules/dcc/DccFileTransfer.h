#ifndef _SEND_H_
#define _SEND_H_
//=============================================================================
//
//   File : DccFileTransfer.h
//   Creation date : Tue Sep 24 09 2000 15:06:12 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "DccDescriptor.h"
#include "DccWindow.h"
#include "DccThread.h"

#include "KviWindow.h"
#include "KviCString.h"
#include "kvi_sockettype.h"
#include "KviPointerList.h"
#include "KviFileTransfer.h"
#include "KviTimeUtils.h"
#include "KviTalHBox.h"
#include "KviTalVBox.h"

#include <QLabel>
#include <QProgressBar>
#include <QFile>
#include <QDialog>
#include <QCheckBox>
#include <QMenu>

class QSpinBox;
class QTimer;
class QPainter;
class DccFileTransfer;
class DccMarshal;
class QMenu;

struct KviDccSendThreadOptions
{
	KviCString szFileName;
	quint64 uStartPosition;
	int iPacketSize;
	int iIdleStepLengthInMSec;
	bool bFastSend;
	bool bNoAcks;
	bool bIsTdcc;
	unsigned int uMaxBandwidth;
};

class DccSendThread : public DccThread
{
public:
	DccSendThread(QObject * par, kvi_socket_t fd, KviDccSendThreadOptions * opt);
	~DccSendThread();

private:
	// stats: SHARED!!!
	uint m_uAverageSpeed;
	uint m_uInstantSpeed;
	quint64 m_uFilePosition;
	quint64 m_uAckedBytes = 0;
	quint64 m_uTotalSentBytes = 0;
	// internal
	unsigned long m_uStartTime;
	unsigned long m_uInstantSpeedInterval;
	quint64 m_uInstantSentBytes;
	KviDccSendThreadOptions * m_pOpt;
	KviMSecTimeInterval * m_pTimeInterval; // used for computing the instant bandwidth but not only
public:
	void initGetInfo();
	uint averageSpeed() { return m_uAverageSpeed; };
	uint instantSpeed() { return m_uInstantSpeed; };
	quint64 filePosition() { return m_uFilePosition; };
	// sent ONLY in this session
	quint64 sentBytes() { return m_uTotalSentBytes; };
	quint64 ackedBytes() { return m_uAckedBytes; };
	unsigned int bandwidthLimit() { return m_pOpt->uMaxBandwidth; };
	void setBandwidthLimit(unsigned int uMaxBandwidth) { m_pOpt->uMaxBandwidth = uMaxBandwidth; };
	void doneGetInfo();

protected:
	void updateStats();
	virtual void run();
};

struct KviDccRecvThreadOptions
{
	bool bResume;
	KviCString szFileName;
	quint64 uTotalFileSize;
	int iIdleStepLengthInMSec;
	bool bSendZeroAck;
	bool bSend64BitAck;
	bool bNoAcks;
	bool bIsTdcc;
	unsigned int uMaxBandwidth;
};

class DccRecvThread : public DccThread
{
public:
	DccRecvThread(QObject * par, kvi_socket_t fd, KviDccRecvThreadOptions * opt);
	~DccRecvThread();

protected:
	KviDccRecvThreadOptions * m_pOpt;

	// stats: SHARED!
	uint m_uAverageSpeed;
	uint m_uInstantSpeed;
	quint64 m_uFilePosition;
	quint64 m_uTotalReceivedBytes;

	// internal
	unsigned long m_uStartTime;
	KviMSecTimeInterval * m_pTimeInterval; // used for computing the instant bandwidth
	quint64 m_uInstantReceivedBytes;
	quint64 m_uInstantSpeedInterval;
	QFile * m_pFile;

public:
	void initGetInfo();
	uint averageSpeed() { return m_uAverageSpeed; };
	uint instantSpeed() { return m_uInstantSpeed; };
	quint64 filePosition() { return m_uFilePosition; };
	// received ONLY in this session
	quint64 receivedBytes() { return m_uTotalReceivedBytes; };
	unsigned int bandwidthLimit() { return m_pOpt->uMaxBandwidth; };
	void setBandwidthLimit(unsigned int uMaxBandwidth) { m_pOpt->uMaxBandwidth = uMaxBandwidth; };
	void doneGetInfo();

protected:
	void postMessageEvent(const char * msg);
	void updateStats();
	bool sendAck(qint64 filePos, bool bUse64BitAck = false);
	virtual void run();
};

class DccFileTransferBandwidthDialog : public QDialog
{
	Q_OBJECT
public:
	DccFileTransferBandwidthDialog(QWidget * pParent, DccFileTransfer * t);
	~DccFileTransferBandwidthDialog();

protected:
	DccFileTransfer * m_pTransfer;
	QCheckBox * m_pEnableLimitCheck;
	QSpinBox * m_pLimitBox;

protected:
	void closeEvent(QCloseEvent * e) override;
protected slots:
	void okClicked();
	void cancelClicked();
};

class DccFileTransfer : public KviFileTransfer, public DccMarshalOutputContext
{
	enum GeneralStatus
	{
		Connecting,
		Transferring,
		Success,
		Failure
	};
	Q_OBJECT
public:
	DccFileTransfer(DccDescriptor * dcc);
	~DccFileTransfer();

private:
	DccSendThread * m_pSlaveSendThread;
	DccRecvThread * m_pSlaveRecvThread;
	DccDescriptor * m_pDescriptor;
	DccMarshal * m_pMarshal;

	KviCString m_szTarget;
	KviCString m_szDccType;
	QString m_szTransferIdString;

	QString m_szStatusString;
	GeneralStatus m_eGeneralStatus;

	QString m_szTransferLog; // html

	kvi_time_t m_tTransferStartTime;
	kvi_time_t m_tTransferEndTime;
	// cached stats
	quint64 m_uTotalFileSize; // total file size to transfer

	unsigned int m_uMaxBandwidth;
	DccFileTransferBandwidthDialog * m_pBandwidthDialog;

	QTimer * m_pResumeTimer; // used to signal resume timeout
public:
	bool resumeAccepted(const char * filename, const char * port, const char * szZeroPortTag);
	bool doResume(const char * filename, const char * port, quint64 filePos);

	static void init();
	static void done();
	static unsigned int runningTransfersCount();
	static DccFileTransfer * nonFailedTransferWithLocalFileName(const QString & szLocalFileName);
	static unsigned int transferCount();
	static bool handleResumeAccepted(const char * filename, const char * port, const char * szZeroPortTag);
	static bool handleResumeRequest(const char * filename, const char * port, quint64 filePos);

	bool event(QEvent * e) override;

	KviWindow * dccMarshalOutputWindow() override;
	const char * dccMarshalOutputContextString() override;

	void displayPaint(QPainter * p, int column, QRect rect) override;
	int displayHeight(int iLineSpacing) override;
	void fillContextPopup(QMenu * m) override;
	virtual void fillStatusString(QString & szBuffer);
	bool active() override;
	QString tipText() override;
	QString localFileName() override;

	bool isFileUpload() { return m_pDescriptor->isFileUpload(); };

	unsigned int averageSpeed();
	unsigned int instantSpeed();
	unsigned int transferredBytes();

	int bandwidthLimit();
	void setBandwidthLimit(int iVal);
	virtual DccThread * getSlaveThread();

protected:
	void startConnection();
	void listenOrConnect();
	void addToTransferLog(const QString & s);
	void outputAndLog(const QString & s);
	void outputAndLog(int msgtype, const QString & s);
	KviWindow * eventWindow();
protected slots:
	void connectionInProgress();
	void sslError(const char * msg);
	void startingSSLHandshake();
	void handleMarshalError(KviError::Code);
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
