#ifndef _VIDEO_H_
#define _VIDEO_H_
//=============================================================================
//
//   File : DccVideoWindow.h
//   Creation date : Tue Nov 10 18:08:09 2009 GMT by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2009 Szymon Stefanek (pragma at kvirc dot net)
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

#include "DccVoiceCodec.h"
#include "DccDescriptor.h"
#include "DccThread.h"
#include "DccWindow.h"

#include "KviWindow.h"
#include "KviDataBuffer.h"
#include "kvi_sockettype.h"
#include "KviTalHBox.h"
#include "KviThemedLabel.h"
#include "KviError.h"

#include <QLabel>
#include <QToolButton>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <QComboBox>
#include <QGridLayout>

#include <QCamera>
#include <QCameraViewfinder>

#ifdef COMPILE_CRYPT_SUPPORT
class KviCryptSessionInfo;
#endif

class QSlider;
class DccMarshal;

#ifndef _DCC_VIDEO_CPP_
extern bool kvi_dcc_video_is_valid_codec(const char * codecName);
#endif

#define KVI_DCC_VIDEO_THREAD_ACTION_START_RECORDING 0
#define KVI_DCC_VIDEO_THREAD_ACTION_STOP_RECORDING 1
#define KVI_DCC_VIDEO_THREAD_ACTION_START_PLAYING 2
#define KVI_DCC_VIDEO_THREAD_ACTION_STOP_PLAYING 3
#define KVI_DCC_VIDEO_THREAD_ACTION_GRAB_FRAME 4

struct KviDccVideoThreadOptions
{
	QString szVideoDevice;
	DccVideoCodec * pCodec;
};

class DccVideoThread : public DccThread
{
	friend class DccVideoWindow;

public:
	DccVideoThread(KviWindow * wnd, kvi_socket_t fd, KviDccVideoThreadOptions * opt);
	~DccVideoThread();

protected:
	KviDccVideoThreadOptions * m_pOpt;
	KviDataBuffer m_outFrameBuffer;
	KviDataBuffer m_inFrameBuffer;
	KviDataBuffer m_videoInSignalBuffer;
	KviDataBuffer m_textInSignalBuffer;
	KviDataBuffer m_videoOutSignalBuffer;
	KviDataBuffer m_textOutSignalBuffer;
	bool m_bPlaying;
	bool m_bRecording;

protected:
	QImage m_inImage;
	QImage m_outImage;

protected:
	bool readWriteStep();
	bool videoStep();
	bool textStep();
	void startRecording();
	void restartRecording(int iDevice, int iInput, int iStandard);
	void stopRecording();
	void startPlaying();
	void stopPlaying();
	bool isPlaying() const { return m_bPlaying; }
	void run() override;
	bool handleIncomingData(KviDccThreadIncomingData * data, bool bCritical);
};

class DccVideoWindow : public DccWindow
{
	Q_OBJECT
	friend class DccVideoThread;

public:
	DccVideoWindow(DccDescriptor * dcc, const char * name);
	~DccVideoWindow();

protected:
	KviThemedLabel * m_pLabel;
	QWidget * m_pContainerWidget;
	QImage * m_pCameraImage;
	QCameraViewfinder * m_pCameraView;
	QCamera * m_pCamera;
	QLabel * m_pInVideoLabel;
	QComboBox * m_pCDevices;
	QComboBox * m_pCInputs;
	QComboBox * m_pCStandards;
	QGridLayout * m_pLayout;
	QTimer m_Timer;
	QLabel * m_pVideoLabel[3];
	QString * m_pszTarget;
	DccVideoThread * m_pSlaveThread;
	QByteArray m_tmpTextDataOut;
	QString m_szLocalNick;

protected:
	void triggerCreationEvents() override;
	void triggerDestructionEvents() override;
	const QString & target() override;
	void fillCaptionBuffers() override;
	QPixmap * myIconPtr() override;
	bool event(QEvent * e) override;
	void getBaseLogFileName(QString & buffer) override;
	void startTalking();
	void stopTalking();
	void startConnection();
	const QString & localNick() override;
	void ownMessage(const QString & text, bool bUserFeedback = true) override;
	void ownAction(const QString & text) override;
	void resizeEvent(QResizeEvent *) override;
	QSize sizeHint() const override;
protected slots:
	void handleMarshalError(KviError::Code eError);
	void connected();
	void startOrStopTalking(bool bStart);
	void connectionInProgress();
	void slotUpdateImage();
	void textViewRightClicked();
	void videoInputChanged(int);
};

#endif //_VIDEO_H_
