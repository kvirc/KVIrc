#ifndef _VIDEO_H_
#define _VIDEO_H_
//=============================================================================
//
//   File : video.h
//   Creation date : Tue Nov 10 18:08:09 2009 GMT by Fabio Bas
//
//   This file is part of the KVirc irc client distribution
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

#include "codec.h"
#include "descriptor.h"
#include "thread.h"
#include "window.h"

#include "kvi_window.h"
#include "kvi_databuffer.h"
#include "kvi_sockettype.h"
#include "kvi_tal_hbox.h"

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

class QSlider;
class KviDccMarshal;

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
	KviDccVideoCodec * pCodec;
	bool               bForceHalfDuplex;
	int                iPreBufferSize;
	int                iSampleRate;
	KviStr             szSoundDevice;
} KviDccVideoThreadOptions;

class KviDccVideoThread : public KviDccThread
{
	friend class KviDccVideo;
public:
	KviDccVideoThread(KviWindow * wnd,kvi_socket_t fd,KviDccVideoThreadOptions * opt);
	~KviDccVideoThread();
protected:
	KviDccVideoThreadOptions * m_pOpt;
	KviDataBuffer              m_outFrameBuffer;
	KviDataBuffer              m_inFrameBuffer;
	KviDataBuffer              m_inSignalBuffer;
	KviDataBuffer              m_videoOutSignalBuffer;
	KviDataBuffer              m_audioOutSignalBuffer;
	bool                       m_bPlaying;
	bool                       m_bRecording;
	bool                       m_bRecordingRequestPending;
	unsigned  int              m_uSleepTime;
	KviMutex                 * m_pInfoMutex;
	// stuff protected by the mutex:
	int                        m_iInputBufferSize;
	int                        m_iOutputBufferSize;
	//audio
	int                        m_soundFd;
	int                        m_soundFdMode;
	bool                       m_bSoundcardChecked;
protected:
	QImage                     m_inImage;
	QImage                     m_outImage;
protected:
	bool readWriteStep();
	bool videoStep();
	bool audioStep();
	void startRecording();
	void stopRecording();
	void startPlaying();
	void stopPlaying();
	inline bool isPlaying() { return m_bPlaying; };
	virtual void run();
	//audio
	bool checkSoundcard();
	bool openSoundcardWithDuplexOption(int openMode,int failMode);
	bool openSoundcard(int mode);
	bool openSoundcardForWriting();
	bool openSoundcardForReading();
	void closeSoundcard();
};


class KviDccVideo : public KviDccWindow
{
	Q_OBJECT
	friend class KviDccVideoThread;
public:
	KviDccVideo(KviFrame *pFrm,KviDccDescriptor * dcc,const char * name);
	~KviDccVideo();
protected:
	QLabel *m_pInVideoLabel;
	QLabel *m_pOutVideoLabel;
	QComboBox *m_pCDevices;
	QComboBox *m_pCInputs;
	QComboBox *m_pCStandards;
	QGridLayout *m_pLayout;
	QTimer m_Timer;
	QLabel * m_pLabel[2];
	QString                * m_pszTarget;
	KviDccVideoThread      * m_pSlaveThread;
protected:
//	virtual void focusInEvent(QFocusEvent *);
	virtual const QString & target();
	virtual void fillCaptionBuffers();
	virtual QPixmap * myIconPtr();
/*
	virtual void resizeEvent(QResizeEvent *e);
	virtual QSize sizeHint() const;
*/
	virtual bool event(QEvent *e);
	virtual void getBaseLogFileName(QString &buffer);
	void startTalking();
	void stopTalking();
	void startConnection();
//	int getMixerVolume(void) const;
protected slots:
	void handleMarshalError(int err);
	void connected();
	void startOrStopTalking(bool bStart);
/*
	void setMixerVolume(int);
*/
	void connectionInProgress();
	void slotUpdateImage();
	void deviceRegistered(const QString &);
	void deviceUnregistered(const QString &);
};


#endif //_VOICE_H_
