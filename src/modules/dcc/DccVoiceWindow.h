#ifndef _VOICE_H_
#define _VOICE_H_
//=============================================================================
//
//   File : DccVoiceWindow.h
//   Creation date : Thu Aug 23 04:08:10 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "KviError.h"

#include <QLabel>
#include <QToolButton>
#include <QTimer>

class QSlider;
class DccMarshal;

#ifndef _DCC_VOICE_CPP_
extern bool kvi_dcc_voice_is_valid_codec(const char * codecName);
#endif

#define KVI_DCC_VOICE_THREAD_ACTION_START_RECORDING 0
#define KVI_DCC_VOICE_THREAD_ACTION_STOP_RECORDING 1
#define KVI_DCC_VOICE_THREAD_ACTION_START_PLAYING 2
#define KVI_DCC_VOICE_THREAD_ACTION_STOP_PLAYING 3

struct KviDccVoiceThreadOptions
{
	bool bForceHalfDuplex;
	int iPreBufferSize;
	int iSampleRate;
	KviCString szSoundDevice;
	DccVoiceCodec * pCodec;
};

class DccVoiceThread : public DccThread
{
	friend class DccVoiceWindow;

public:
	DccVoiceThread(KviWindow * wnd, kvi_socket_t fd, KviDccVoiceThreadOptions * opt);
	~DccVoiceThread();

protected:
	//	bool                       m_bUseGsm;
	KviDccVoiceThreadOptions * m_pOpt;
	int m_soundFd;
	int m_soundFdMode;
	KviDataBuffer m_outFrameBuffer;
	KviDataBuffer m_inFrameBuffer;
	KviDataBuffer m_inSignalBuffer;
	KviDataBuffer m_outSignalBuffer;
	bool m_bPlaying;
	bool m_bRecording;
	bool m_bRecordingRequestPending;
	bool m_bSoundcardChecked;
	int m_iLastSignalBufferSize;
	long m_iLastSignalBufferTime;
	//	unsigned  int              m_uSleepTime;
	KviMutex * m_pInfoMutex;
	// stuff protected by the mutex:
	int m_iInputBufferSize;
	int m_iOutputBufferSize;

protected:
	bool checkSoundcard();
	bool openSoundcardWithDuplexOption(int openMode, int failMode);
	bool openSoundcard(int mode);
	bool openSoundcardForWriting();
	bool openSoundcardForReading();
	void closeSoundcard();
	bool readWriteStep();
	bool soundStep();
	void startRecording();
	void stopRecording();
	void startPlaying();
	void stopPlaying();
	virtual void run();
};

class DccVoiceWindow : public DccWindow
{
	Q_OBJECT
public:
	DccVoiceWindow(DccDescriptor * dcc, const char * name);
	~DccVoiceWindow();

protected:
	KviTalHBox * m_pHBox;
	QSlider * m_pVolumeSlider;
	QLabel * m_pInputLabel;
	QLabel * m_pOutputLabel;
	QLabel * m_pRecordingLabel;
	QLabel * m_pPlayingLabel;
	QToolButton * m_pTalkButton;
	QTimer * m_pUpdateTimer;
	QString m_szTarget;
	DccVoiceThread * m_pSlaveThread;

protected:
	void focusInEvent(QFocusEvent *) override;
	const QString & target() override;
	void fillCaptionBuffers() override;
	QPixmap * myIconPtr() override;
	void resizeEvent(QResizeEvent * e) override;
	QSize sizeHint() const override;
	bool event(QEvent * e) override;
	void getBaseLogFileName(QString & buffer) override;
	void startTalking();
	void stopTalking();
	void startConnection();
	int getMixerVolume(void) const;
protected slots:
	void handleMarshalError(KviError::Code eError);
	void connected();
	void updateInfo();
	void startOrStopTalking(bool bStart);
	void setMixerVolume(int);
	void connectionInProgress();
	//	void stopTalking();
};

#endif //_VOICE_H_
