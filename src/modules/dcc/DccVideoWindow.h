#ifndef _VIDEO_H_
#define _VIDEO_H_
//=============================================================================
//
//   File : DccVideoWindow.h
//   Creation date : Tue Nov 10 18:08:09 2009 GMT by Fabio Bas
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2009 Szymon Stefanek (pragma at kvirc dot net)
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

#ifndef COMPILE_DISABLE_DCC_VIDEO
	#include "avdevice/videodevicepool.h"
#endif

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

typedef struct _KviDccVideoThreadOptions
{
	QString            szVideoDevice;
	DccVideoCodec * pCodec;
} KviDccVideoThreadOptions;

class DccVideoThread : public DccThread
{
	friend class DccVideoWindow;
public:
	DccVideoThread(KviWindow * wnd,kvi_socket_t fd,KviDccVideoThreadOptions * opt);
	~DccVideoThread();
protected:
	KviDccVideoThreadOptions * m_pOpt;
	KviDataBuffer              m_outFrameBuffer;
	KviDataBuffer              m_inFrameBuffer;
	KviDataBuffer              m_videoInSignalBuffer;
	KviDataBuffer              m_textInSignalBuffer;
	KviDataBuffer              m_videoOutSignalBuffer;
	KviDataBuffer              m_textOutSignalBuffer;
	bool                       m_bPlaying;
	bool                       m_bRecording;
protected:
	QImage                     m_inImage;
	QImage                     m_outImage;
protected:
	bool readWriteStep();
	bool videoStep();
	bool textStep();
	void startRecording();
	void restartRecording(int iDevice, int iInput, int iStandard);
	void stopRecording();
	void startPlaying();
	void stopPlaying();
	inline bool isPlaying() { return m_bPlaying; };
	virtual void run();
	virtual bool handleIncomingData(KviDccThreadIncomingData *data,bool bCritical);
};


class DccVideoWindow : public DccWindow
{
	Q_OBJECT
	friend class DccVideoThread;
public:
	DccVideoWindow(KviMainWindow *pFrm,DccDescriptor * dcc,const char * name);
	~DccVideoWindow();
protected:
	KviThemedLabel         * m_pLabel;
	QWidget                * m_pContainerWidget;
	QLabel                 * m_pInVideoLabel;
	QLabel                 * m_pOutVideoLabel;
	QComboBox              * m_pCDevices;
	QComboBox              * m_pCInputs;
	QComboBox              * m_pCStandards;
	QGridLayout            * m_pLayout;
	QTimer                   m_Timer;
	QLabel                 * m_pVideoLabel[2];
	QString                * m_pszTarget;
	DccVideoThread         * m_pSlaveThread;
	QByteArray               m_tmpTextDataOut;
	QString                  m_szLocalNick;
protected:
	virtual void triggerCreationEvents();
	virtual void triggerDestructionEvents();
	virtual const QString & target();
	virtual void fillCaptionBuffers();
	virtual QPixmap * myIconPtr();
	virtual bool event(QEvent *e);
	virtual void getBaseLogFileName(QString &buffer);
	void startTalking();
	void stopTalking();
	void startConnection();
	virtual const QString & localNick();
	virtual void ownMessage(const QString &text, bool bUserFeedback = true);
	virtual void ownAction(const QString &text);
	virtual void resizeEvent(QResizeEvent *);
	virtual QSize sizeHint() const;
protected slots:
	void handleMarshalError(KviError::Code eError);
	void connected();
	void startOrStopTalking(bool bStart);
	void connectionInProgress();
	void slotUpdateImage();
	void deviceRegistered(const QString &);
	void deviceUnregistered(const QString &);
	void textViewRightClicked();
	void videoInputChanged(int );
};


#endif //_VOICE_H_
