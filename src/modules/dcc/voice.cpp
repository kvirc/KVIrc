//=============================================================================
//
//   File : voice.cpp
//   Creation date : Thu Aug 23 04:08:09 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "voice.h"
#include "marshal.h"
#include "broker.h"
#include "adpcmcodec.h"
#include "gsmcodec.h"

#include "kvi_settings.h"
#include "kvi_iconmanager.h"
#include "kvi_ircview.h"
#include "kvi_locale.h"
#include "kvi_out.h"
#include "kvi_error.h"
#include "kvi_netutils.h"
#include "kvi_options.h"
#include "kvi_console.h"
#include "kvi_malloc.h"
#include "kvi_socket.h"
#include "kvi_ircconnection.h"
#include "kvi_tal_vbox.h"

#include <QFrame>
#include <QSlider>
#include <QToolTip>

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
	#include <sys/time.h>
	#include <sys/types.h>
	#include <unistd.h>
	#include <errno.h>
	#include <fcntl.h>
	//#include "kvi_error.h"
	#include <sys/stat.h>   // for open()
	#include <sys/ioctl.h>  // for ioctl()
#endif

extern KviDccBroker * g_pDccBroker;

//Check for the *SS Api....we don't want to fail here...

#ifndef COMPILE_DISABLE_DCC_VOICE
	#ifdef HAVE_LINUX_SOUNDCARD_H
		#include <linux/soundcard.h>
	#else
		#ifdef HAVE_SYS_SOUNDCARD_H
			#include <sys/soundcard.h>
		#else
			#ifdef HAVE_SOUNDCARD_H
				#include <soundcard.h>
			#else
				//CAN NOT COMPILE :(
				#define COMPILE_DISABLE_DCC_VOICE
				#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
					#warning "Cannot find the soundcard.h header; you will NOT be able to use DCC Voice"
				#endif
			#endif
		#endif
	#endif
#endif


//#define KVI_AUDIO_DEVICE "/dev/dsp"
// 32 fragments , 512 bytes
#define KVI_SNDCTL_FRAG_SIZE 0x00B00009
#define KVI_FRAGMENT_SIZE_IN_BYTES 512
#define KVI_FORMAT AFMT_S16_LE
#define KVI_NUM_CHANNELS 1


bool kvi_dcc_voice_is_valid_codec(const char * codecName)
{
#ifdef COMPILE_USE_GSM
	if(kvi_strEqualCI("gsm",codecName))
	{
		return kvi_gsm_codec_init();
	}
#endif
	if(kvi_strEqualCI("adpcm",codecName))return true;
	if(kvi_strEqualCI("null",codecName))return true;
	return false;
}

static KviDccVoiceCodec * kvi_dcc_voice_get_codec(const char * codecName)
{
#ifdef COMPILE_USE_GSM
	if(kvi_strEqualCI("gsm",codecName))
	{
		if(kvi_gsm_codec_init())return new KviDccVoiceGsmCodec();
	}
#endif
	if(kvi_strEqualCI("adpcm",codecName))return new KviDccVoiceAdpcmCodec();
	if(kvi_strEqualCI("null",codecName))return new KviDccVoiceNullCodec();
	return new KviDccVoiceAdpcmCodec();
}


KviDccVoiceThread::KviDccVoiceThread(KviWindow * wnd,kvi_socket_t fd,KviDccVoiceThreadOptions * opt)
: KviDccThread(wnd,fd)
{
#ifndef COMPILE_DISABLE_DCC_VOICE
	m_pOpt = opt;
	m_bPlaying = false;
	m_bRecording = false;
	m_bSoundcardChecked = false;
	m_soundFd = -1;
	m_soundFdMode = 0;
	m_pInfoMutex = new KviMutex();
	m_bRecordingRequestPending = false;
#endif
}

KviDccVoiceThread::~KviDccVoiceThread()
{
#ifndef COMPILE_DISABLE_DCC_VOICE
	delete m_pOpt->pCodec;
	delete m_pOpt;
	delete m_pInfoMutex;
#endif
}

bool KviDccVoiceThread::checkSoundcard()
{
#ifdef COMPILE_DISABLE_DCC_VOICE
	return false;
#else
	bool bOpened = false;
	if(m_soundFd == -1)
	{
		if(!openSoundcard(O_RDONLY))return false;
		bOpened = true;
	}

	int caps;

	m_bSoundcardChecked = true;

	if(ioctl(m_soundFd,SNDCTL_DSP_GETCAPS,&caps) < 0)
	{
		postMessageEvent(__tr2qs_ctx("WARNING: failed to check the soundcard duplex capabilities: if this is a half-duplex soundcard, use the DCC VOICE option to force half-duplex algorithm","dcc").toUtf8().data());
		if(bOpened)closeSoundcard();
		return false;
	}

	if(!(caps & DSP_CAP_DUPLEX))
	{
		m_pOpt->bForceHalfDuplex = true; // the device is half duplex...use it in that way
		postMessageEvent(__tr2qs_ctx("Half duplex soundcard detected, you will not be able to talk and listen at the same time","dcc").toUtf8().data());
	}

	if(bOpened)closeSoundcard();

	return true;
#endif
}


bool KviDccVoiceThread::openSoundcard(int mode)
{
#ifdef COMPILE_DISABLE_DCC_VOICE
	return false;
#else
	int speed = m_pOpt->iSampleRate;
	static int chans=KVI_NUM_CHANNELS;
	static int fmt=KVI_FORMAT;
	static int frag = KVI_SNDCTL_FRAG_SIZE;


	if(m_soundFd != -1)
	{
		if(m_soundFdMode == mode)return true; // already open
		closeSoundcard();
	}

	m_soundFd = ::open(m_pOpt->szSoundDevice.ptr(),mode | O_NONBLOCK);
	if(m_soundFd < 0)return false;

	if(!m_pOpt->bForceHalfDuplex)
	{
		if(ioctl(m_soundFd,SNDCTL_DSP_SETDUPLEX,0) < 0)goto exit_false;
	}

	if(ioctl(m_soundFd,SNDCTL_DSP_SETFRAGMENT,&frag)<0)goto exit_false;
	if(ioctl(m_soundFd,SNDCTL_DSP_SETFMT,&fmt)<0)goto exit_false;
	if(ioctl(m_soundFd,SNDCTL_DSP_CHANNELS,&chans)<0)goto exit_false;
	if(ioctl(m_soundFd,SNDCTL_DSP_SPEED,&speed)<0)goto exit_false;
	if(speed != m_pOpt->iSampleRate)
	{
		KviStr tmp(KviStr::Format,__tr2qs_ctx("WARNING: failed to set the requested sample rate (%d): the device used closest match (%d)","dcc").toUtf8().data(),
						m_pOpt->iSampleRate,speed);
		postMessageEvent(tmp.ptr());
	}

	//  TODO: #warning "We could also support blocking operations mode"

	m_soundFdMode = mode;


	return true;

exit_false:
	closeSoundcard();
	return false;
#endif
}

bool KviDccVoiceThread::openSoundcardForWriting()
{
#ifndef COMPILE_DISABLE_DCC_VOICE
	return openSoundcardWithDuplexOption(O_WRONLY,O_RDONLY);
#else
	return false;
#endif
}

bool KviDccVoiceThread::openSoundcardForReading()
{
#ifndef COMPILE_DISABLE_DCC_VOICE
	return openSoundcardWithDuplexOption(O_RDONLY,O_WRONLY);
#else
	return false;
#endif
}

bool KviDccVoiceThread::openSoundcardWithDuplexOption(int openMode,int failMode)
{
#ifndef COMPILE_DISABLE_DCC_VOICE
	if(m_soundFd == -1)
	{
		// soundcard not open yet...open for write (at least)
		if(m_pOpt->bForceHalfDuplex)
		{
			// Forcing half duplex... (user option or the card does not support full duplex mode)
			if(!openSoundcard(openMode))return false;
		} else {
			// Try read/write open
			if(!openSoundcard(O_RDWR))
			{
				// half-duplex sound card ?
				if(!m_bSoundcardChecked)
				{
					// We haven't checked the full-duplex support yet...
					// Try to open in RDONLY o WRONLY mode
					if(!openSoundcard(openMode))return false;
					if(!checkSoundcard())
					{
						postMessageEvent(__tr2qs_ctx("Ops...failed to test the soundcard capabilities...expect problems...","dcc").toUtf8().data());
					}
				} // else the test has been done and it is a full duplex card that is just busy
			}
		}
	} else {
		// Hmmm...already open
		// If it is open in O_RDWR or O_WRONLY mode...it is ok for us
		// but if it is open in O_RDONLY mode...we can do nothing...just wait
		return (m_soundFdMode != failMode);
	}

	return true;
#else
	return false;
#endif
}

void KviDccVoiceThread::closeSoundcard()
{
#ifndef COMPILE_DISABLE_DCC_VOICE
	if(m_soundFd != -1)
	{
		::close(m_soundFd);
		m_soundFd = -1;
		m_soundFdMode = 0;
	}
#endif
}

bool KviDccVoiceThread::readWriteStep()
{
#ifndef COMPILE_DISABLE_DCC_VOICE
	// Socket management
	bool bCanRead;
	bool bCanWrite;

	if(kvi_select(m_fd,&bCanRead,&bCanWrite))
	{
		if(bCanRead)
		{
			unsigned int actualSize = m_inFrameBuffer.size();
			m_inFrameBuffer.resize(actualSize + 1024);
			int readLen = kvi_socket_recv(m_fd,(void *)(m_inFrameBuffer.data() + actualSize),1024);
			if(readLen > 0)
			{
				if(readLen < 1024)m_inFrameBuffer.resize(actualSize + readLen);
				m_pOpt->pCodec->decode(&m_inFrameBuffer,&m_inSignalBuffer);
//#warning "A maximum length for the signal buffer is actually needed!!!"
			} else {
				if(!handleInvalidSocketRead(readLen))return false;
				m_inFrameBuffer.resize(actualSize);
			}
		}// else {
		//	m_uSleepTime += 100;
		//}

		if(bCanWrite)
		{
			// Have somethihg to write ?
			if(m_outFrameBuffer.size() > 0)
			{
				int written = kvi_socket_send(m_fd,m_outFrameBuffer.data(),m_outFrameBuffer.size());
				if(written > 0)
				{
					m_outFrameBuffer.remove(written);
				} else {
					if(!handleInvalidSocketRead(written))return false;
				}
			}// else {
	//			m_uSleepTime += 100;
	//		}
		}// else {
	//		m_uSleepTime += 100;
//		}
//#warning "Usleep here ?"
	}// else {
//		if(!(m_bPlaying || m_bRecording))
//		{
//			// Really NOTHING is happening...sleep a bit more
//			m_uSleepTime += 800;
//		} else {
//			m_uSleepTime += 100;
//		}
//	}

#endif // !COMPILE_DISABLE_DCC_VOICE
	return true;
}

bool KviDccVoiceThread::soundStep()
{
#ifndef COMPILE_DISABLE_DCC_VOICE
	// Are we playing ?
	if(m_bPlaying)
	{
		// Do we have something to write ?
		audio_buf_info info;
		if(m_inSignalBuffer.size() > 0)
		{
			// Get the number of fragments that can be written to the soundcard without blocking

			if(ioctl(m_soundFd,SNDCTL_DSP_GETOSPACE,&info) < 0)
			{
				debug("get o space failed");
				info.bytes = KVI_FRAGMENT_SIZE_IN_BYTES; // dummy... if this is not correct...well...we will block for 1024/16000 of a sec
				info.fragments = 1;
				info.fragsize = KVI_FRAGMENT_SIZE_IN_BYTES;
			}
			if(info.fragments > 0)
			{
				int toWrite = info.fragments * info.fragsize;
				//debug("Can write %d bytes",toWrite);
				if(m_inSignalBuffer.size() < toWrite)toWrite = m_inSignalBuffer.size();
				int written = write(m_soundFd,m_inSignalBuffer.data(),toWrite);
				if(written > 0)m_inSignalBuffer.remove(written);
				else {
//#warning "Do something for -1 here ?"
//#warning "Usleep ?"
				}
			} //else {
				// No stuff can be written...we are running too fast ?
			//	m_uSleepTime += 100; // sleep for a while
			//}
		} else {
			// hmmmm....playing , but nothing to write , possible underrun or EOF
			// a nice idea would be to use SNDCTL_DSP_GETODELAY here...
			// but it appears to be broken on some audio devices
			if(ioctl(m_soundFd,SNDCTL_DSP_GETOSPACE,&info) < 0)info.fragstotal = info.fragments; // dummy...but what should we do ?
			if(info.fragstotal == info.fragments)
			{
				// underrun or EOF: close the device
				stopPlaying();
			}
		}
	} else {
		// do we have anything to play ?
		if(m_inSignalBuffer.size() > 0)
		{
			if(m_inSignalBuffer.size() >= m_pOpt->iPreBufferSize)
			{
				// yep...stuff to play... open the soundcard , if possible
				startPlaying();

				m_iLastSignalBufferSize = m_inSignalBuffer.size();
			} else {
				// have stuff to play , but it's not enough to fill the pre-buffer
				//
				struct timeval tv;
				gettimeofday(&tv,0);

				long int sigBufferTime = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);

				if(m_inSignalBuffer.size() == m_iLastSignalBufferSize)
				{
					// the same signal buffer size... check the time
					// m_pOpt->iPreBufferSize / 16 gives us the preBufferTime in msecs
					// we calc the remaining preBufferTime by subtracting the
					// size of buffer already filled and we also add 50 milliseconds... smart heuristic
					int preBufferTime = ((m_pOpt->iPreBufferSize - m_iLastSignalBufferSize) / 16) + 50;
					// if the buffer size hasn't changed since preBufferTime
					// it's time to start playing anyway, since there is
					// either a network stall or it was just a really short data stream
					if((sigBufferTime - m_iLastSignalBufferTime) > preBufferTime)
					{
						startPlaying();
						if(m_bPlaying)m_iLastSignalBufferSize = 0;
					}
				} else {
					// signal buffer size differs...we have received new packets
					// and still pre-buffering
					m_iLastSignalBufferSize = m_inSignalBuffer.size();
					m_iLastSignalBufferTime = sigBufferTime;
				}
			}

		}
	}

	// Are we recording ?
	if(m_bRecording)
	{
		fd_set rs;
		FD_ZERO(&rs);
		FD_SET(m_soundFd,&rs);
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 10;
		int ret = select(m_soundFd + 1,&rs,0,0,&tv);
		if(ret > 0)
		{
			// This is rather easy...
			audio_buf_info info;
			if(ioctl(m_soundFd,SNDCTL_DSP_GETISPACE,&info) < 0)
			{
				debug("Ispace failed");
				info.fragments = 0; // dummy...
				info.bytes = 0;
			}

			//debug("INFO: fragments: %d, fragstotal: %d, fragsize: %d, bytes: %d",info.fragments,info.fragstotal,info.fragsize,info.bytes);

			if(info.fragments == 0 && info.bytes == 0)
			{
				// force a dummy read: the device needs to be triggered
				info.fragments = 1;
			}

			if(info.fragments > 0)
			{
				int oldSize = m_outSignalBuffer.size();
				int available = info.fragments * info.fragsize;
				m_outSignalBuffer.addSize(available);
				int readed = read(m_soundFd,m_outSignalBuffer.data() + oldSize,available);

				if(readed < available)
				{
					// huh ? ...error ?
					if(readed >= 0)m_outSignalBuffer.resize(oldSize + readed);
					else {
						if((errno == EINTR) || (errno == EAGAIN))
						{
							m_outSignalBuffer.resize(oldSize);
						} else {
//#warning "Critical error...do something reasonable!"
							m_outSignalBuffer.resize(oldSize);
						}
					}
				}
/*
				debug("Signal buffer:");
				for(int i=0;i<200;i+=2)
				{
					if(i >= m_outSignalBuffer.size())break;
					printf("%04x ",*(((unsigned short *)(m_outSignalBuffer.data() + i))));
					if((i % 6) == 0)printf("\n");
				}
				debug("END\n");
*/
				m_pOpt->pCodec->encode(&m_outSignalBuffer,&m_outFrameBuffer);
			}
		}// else {
			// Nothing to read
	//		m_uSleepTime += 100;
	//	}
	}

#endif // !COMPILE_DISABLE_DCC_VOICE
	return true;
}

void KviDccVoiceThread::startRecording()
{
#ifndef COMPILE_DISABLE_DCC_VOICE
	//debug("Start recording");
	if(m_bRecording)return; // already started
	if(openSoundcardForReading())
	{
//		debug("Posting event");
		KviThreadDataEvent<int> * e = new KviThreadDataEvent<int>(KVI_DCC_THREAD_EVENT_ACTION);
		e->setData(new int(KVI_DCC_VOICE_THREAD_ACTION_START_RECORDING));
		postEvent(parent(),e);

		m_bRecording = true;
		m_bRecordingRequestPending = false;
	} else {
		m_bRecordingRequestPending = true;
	}
#endif
}

void KviDccVoiceThread::stopRecording()
{
#ifndef COMPILE_DISABLE_DCC_VOICE
	//debug("Stop recording");
	m_bRecordingRequestPending = false;
	if(!m_bRecording)return; // already stopped

	KviThreadDataEvent<int> * e = new KviThreadDataEvent<int>(KVI_DCC_THREAD_EVENT_ACTION);
	e->setData(new int(KVI_DCC_VOICE_THREAD_ACTION_STOP_RECORDING));
	postEvent(parent(),e);

	m_bRecording = false;
	if(!m_bPlaying)closeSoundcard();
#endif
}

void KviDccVoiceThread::startPlaying()
{
#ifndef COMPILE_DISABLE_DCC_VOICE
	//debug("Start playing");
	if(m_bPlaying)return;

	if(openSoundcardForWriting())
	{
		KviThreadDataEvent<int> * e = new KviThreadDataEvent<int>(KVI_DCC_THREAD_EVENT_ACTION);
		e->setData(new int(KVI_DCC_VOICE_THREAD_ACTION_START_PLAYING));
		postEvent(parent(),e);
		m_bPlaying = true;
	}
#endif
}

void KviDccVoiceThread::stopPlaying()
{
#ifndef COMPILE_DISABLE_DCC_VOICE
	//debug("Stop playing");
	if(!m_bPlaying)return;

	KviThreadDataEvent<int> * e = new KviThreadDataEvent<int>(KVI_DCC_THREAD_EVENT_ACTION);
	e->setData(new int(KVI_DCC_VOICE_THREAD_ACTION_STOP_PLAYING));
	postEvent(parent(),e);

	m_bPlaying = false;
	if(!m_bRecording)closeSoundcard();
#endif
}

void KviDccVoiceThread::run()
{
#ifndef COMPILE_DISABLE_DCC_VOICE
	for(;;)
	{
//		m_uSleepTime = 0;

		// Dequeue events
		while(KviThreadEvent * e = dequeueEvent())
		{
			if(e->id() == KVI_THREAD_EVENT_TERMINATE)
			{
				delete e;
				goto exit_dcc;
			} else if(e->id() == KVI_DCC_THREAD_EVENT_ACTION)
			{
				int * act = ((KviThreadDataEvent<int> *)e)->getData();
				if(*act)startRecording();
				else stopRecording();
				delete act;
				delete e;
			} else {
				// Other events are senseless to us
				delete e;
			}
		}

		if(!readWriteStep())goto exit_dcc;
		if(!soundStep())goto exit_dcc;

		m_pInfoMutex->lock();
		m_iInputBufferSize = m_inSignalBuffer.size();
		m_iOutputBufferSize = (m_outFrameBuffer.size() / m_pOpt->pCodec->encodedFrameSize()) * m_pOpt->pCodec->decodedFrameSize();
		m_pInfoMutex->unlock();

		// Actually the maximum that we can sleep here is
		// around 500 usecs... = 0.0005 sec -> 8 bytes at 8 KHz

	//	if(m_uSleepTime)usleep(m_uSleepTime);

		// Start recording if the request was not fulfilled yet
		if(m_bRecordingRequestPending)startRecording();
	}


exit_dcc:

#endif //! COMPILE_DISABLE_DCC_VOICE
	closeSoundcard();
	kvi_socket_close(m_fd);
	m_fd = KVI_INVALID_SOCKET;
}


KviDccVoice::KviDccVoice(KviFrame *pFrm,KviDccDescriptor * dcc,const char * name)
: KviDccWindow(KVI_WINDOW_TYPE_DCCVOICE,pFrm,name,dcc)
{
	m_pDescriptor = dcc;
	m_pSlaveThread = 0;

	m_pSplitter = new KviTalSplitter(Qt::Horizontal,this);
	m_pSplitter->setObjectName("dcc_window_splitter");
	m_pIrcView = new KviIrcView(m_pSplitter,pFrm,this);

	m_pHBox = new KviTalHBox(this);

	KviTalVBox * vbox = new KviTalVBox(m_pHBox);

	m_pInputLabel = new QLabel(__tr2qs_ctx("Input buffer","dcc"),vbox);
	m_pInputLabel->setFrameStyle(QFrame::Sunken | QFrame::Panel);
	m_pOutputLabel = new QLabel(__tr2qs_ctx("Output buffer","dcc"),vbox);
	m_pOutputLabel->setFrameStyle(QFrame::Sunken | QFrame::Panel);
	vbox->setSpacing(1);

	KviTalVBox * vbox2 = new KviTalVBox(m_pHBox);

	m_pRecordingLabel = new QLabel(vbox2);
	m_pRecordingLabel->setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_RECORD)));
	m_pRecordingLabel->setEnabled(false);
	m_pRecordingLabel->setFrameStyle(QFrame::Raised | QFrame::Panel);

	m_pPlayingLabel = new QLabel(vbox2);
	m_pPlayingLabel->setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_PLAY)));
	m_pPlayingLabel->setEnabled(false);
	m_pPlayingLabel->setFrameStyle(QFrame::Raised | QFrame::Panel);

	vbox2->setSpacing(1);

	//#warning "The volume slider should be enabled only when receiving data"
	//m_pVolumeSlider = new QSlider(-100, 0, 10, 0, Qt::Vertical, m_pHBox, "dcc_voice_volume_slider");
	m_pVolumeSlider = new QSlider(Qt::Vertical,m_pHBox);
	m_pVolumeSlider->setObjectName("dcc_voice_volume_slider");
	m_pVolumeSlider->setMinimum(-100);
	m_pVolumeSlider->setMaximum(0);
	m_pVolumeSlider->setPageStep(10);
	m_pVolumeSlider->setValue(0);

	m_pVolumeSlider->setValue(getMixerVolume());
	/* Update the tooltip */
	setMixerVolume(m_pVolumeSlider->value());
	m_pVolumeSlider->setMaximumWidth(16);
	m_pVolumeSlider->setMaximumHeight(2*m_pPlayingLabel->height());
	connect(m_pVolumeSlider, SIGNAL(valueChanged(int)), this, SLOT(setMixerVolume(int)));

	m_pTalkButton = new QToolButton(m_pHBox);
	m_pTalkButton->setEnabled(false);
	m_pTalkButton->setCheckable(true);
	QIcon iset;
	iset.addPixmap(*(g_pIconManager->getBigIcon(KVI_BIGICON_DISCONNECTED)),QIcon::Normal,QIcon::Off);
	iset.addPixmap(*(g_pIconManager->getBigIcon(KVI_BIGICON_CONNECTED)),QIcon::Normal,QIcon::On);
	m_pTalkButton->setIcon(iset);
	m_pTalkButton->setIconSize(QSize(32,32));
	connect(m_pTalkButton,SIGNAL(toggled(bool)),this,SLOT(startOrStopTalking(bool)));

	m_pHBox->setStretchFactor(vbox,1);
	m_pHBox->setMargin(2);
	m_pHBox->setSpacing(1);

	//setFocusHandler(m_pIrcView,this);

	m_pMarshal = new KviDccMarshal(this);
	connect(m_pMarshal,SIGNAL(error(int)),this,SLOT(handleMarshalError(int)));
	connect(m_pMarshal,SIGNAL(connected()),this,SLOT(connected()));
	connect(m_pMarshal,SIGNAL(inProgress()),this,SLOT(connectionInProgress()));

	m_pUpdateTimer = new QTimer();

	startConnection();
}

KviDccVoice::~KviDccVoice()
{
	g_pDccBroker->unregisterDccWindow(this);
	if(m_pSlaveThread)
	{
		m_pSlaveThread->terminate();
		delete m_pSlaveThread;
		m_pSlaveThread = 0;
	}

	KviThreadManager::killPendingEvents(this);

	delete m_pUpdateTimer;
//	delete m_pDescriptor;
//	delete m_pMarshal;
}


void KviDccVoice::startConnection()
{
	if(!(m_pDescriptor->bActive))
	{
		// PASSIVE CONNECTION
		output(KVI_OUT_DCCMSG,__tr2qs_ctx("Attempting a passive DCC VOICE connection","dcc"));
		int ret = m_pMarshal->dccListen(m_pDescriptor->szListenIp,m_pDescriptor->szListenPort,m_pDescriptor->bDoTimeout);
		if(ret != KviError_success)handleMarshalError(ret);
	} else {
		// ACTIVE CONNECTION
		output(KVI_OUT_DCCMSG,__tr2qs_ctx("Attempting an active DCC VOICE connection","dcc"));
		int ret = m_pMarshal->dccConnect(m_pDescriptor->szIp.toUtf8().data(),m_pDescriptor->szPort.toUtf8().data(),m_pDescriptor->bDoTimeout);
		if(ret != KviError_success)handleMarshalError(ret);
	}
}

void KviDccVoice::connectionInProgress()
{
	if(m_pDescriptor->bActive)
	{
		output(KVI_OUT_DCCMSG,__tr2qs_ctx("Contacting host %Q on port %Q","dcc"),&(m_pDescriptor->szIp),&(m_pDescriptor->szPort));
	} else {

		output(KVI_OUT_DCCMSG,__tr2qs_ctx("Listening on interface %Q port %Q","dcc"),
			&(m_pMarshal->localIp()),&(m_pMarshal->localPort()));

		if(m_pDescriptor->bSendRequest)
		{
			QString ip     = !m_pDescriptor->szFakeIp.isEmpty() ? m_pDescriptor->szFakeIp : m_pDescriptor->szListenIp;
			KviStr port   = !m_pDescriptor->szFakePort.isEmpty() ? m_pDescriptor->szFakePort : m_pMarshal->localPort();
//#warning "OPTION FOR SENDING 127.0.0.1 and so on (not an unsigned nuumber)"
			struct in_addr a;
			if(KviNetUtils::stringIpToBinaryIp(ip,&a)) {
				ip.setNum(htonl(a.s_addr));
			}

			m_pDescriptor->console()->connection()->sendFmtData("PRIVMSG %s :%cDCC VOICE %s %Q %s %d%c",
					m_pDescriptor->console()->connection()->encodeText(m_pDescriptor->szNick).data(),
					0x01,m_pDescriptor->szCodec.ptr(),
					&ip,port.ptr(),m_pDescriptor->iSampleRate,0x01);
			output(KVI_OUT_DCCMSG,__tr2qs_ctx("Sent DCC VOICE (%s) request to %Q, waiting for the remote client to connect...","dcc"),
					m_pDescriptor->szCodec.ptr(),&(m_pDescriptor->szNick));
		} else output(KVI_OUT_DCCMSG,__tr2qs_ctx("DCC VOICE request not sent: awaiting manual connections","dcc"));
	}
}

const QString & KviDccVoice::target()
{
	// This may change on the fly...
	m_szTarget.sprintf("%s@%s:%s",
		m_pDescriptor->szNick.toUtf8().data(),m_pDescriptor->szIp.toUtf8().data(),m_pDescriptor->szPort.toUtf8().data());
	return m_szTarget;
}

void KviDccVoice::getBaseLogFileName(QString &buffer)
{
	buffer.sprintf("dccvoice_%s_%s_%s",m_pDescriptor->szNick.toUtf8().data(),m_pDescriptor->szLocalFileName.toUtf8().data(),m_pDescriptor->szPort.toUtf8().data());
}

void KviDccVoice::fillCaptionBuffers()
{
	KviStr tmp(KviStr::Format,"DCC Voice %s@%s:%s %s",
		m_pDescriptor->szNick.toUtf8().data(),m_pDescriptor->szIp.toUtf8().data(),m_pDescriptor->szPort.toUtf8().data(),
		m_pDescriptor->szLocalFileName.toUtf8().data());

	m_szPlainTextCaption = tmp;
}

QPixmap * KviDccVoice::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KVI_SMALLICON_DCCVOICE);
}

bool KviDccVoice::event(QEvent *e)
{
	if(e->type() == KVI_THREAD_EVENT)
	{
		switch(((KviThreadEvent *)e)->id())
		{
			case KVI_DCC_THREAD_EVENT_ERROR:
			{
				int * err = ((KviThreadDataEvent<int> *)e)->getData();
				QString ssss = KviError::getDescription(*err);
				output(KVI_OUT_DCCERROR,__tr2qs_ctx("ERROR: %Q","dcc"),&(ssss));
				delete err;
				m_pUpdateTimer->stop();
				updateInfo();
				m_pTalkButton->setEnabled(false);
				m_pRecordingLabel->setEnabled(false);
				m_pPlayingLabel->setEnabled(false);
				return true;
			}
			break;
			case KVI_DCC_THREAD_EVENT_MESSAGE:
			{
				KviStr * str = ((KviThreadDataEvent<KviStr> *)e)->getData();
				outputNoFmt(KVI_OUT_DCCMSG,__tr_no_xgettext_ctx(str->ptr(),"dcc"));
				delete str;
				return true;
			}
			break;
			case KVI_DCC_THREAD_EVENT_ACTION:
			{
				int * act = ((KviThreadDataEvent<int> *)e)->getData();
				switch(*act)
				{
					case KVI_DCC_VOICE_THREAD_ACTION_START_RECORDING:
						m_pRecordingLabel->setEnabled(true);
					break;
					case KVI_DCC_VOICE_THREAD_ACTION_STOP_RECORDING:
						m_pRecordingLabel->setEnabled(false);
					break;
					case KVI_DCC_VOICE_THREAD_ACTION_START_PLAYING:
						m_pPlayingLabel->setEnabled(true);
					break;
					case KVI_DCC_VOICE_THREAD_ACTION_STOP_PLAYING:
						m_pPlayingLabel->setEnabled(false);
					break;
				}
				delete act;
				return true;
			}
			break;
			default:
				debug("Invalid event type %d received",((KviThreadEvent *)e)->id());
			break;
		}

	}

	return KviWindow::event(e);
}

void KviDccVoice::updateInfo()
{
	if(m_pSlaveThread)
	{
		m_pSlaveThread->m_pInfoMutex->lock();
		int iOSize = m_pSlaveThread->m_iOutputBufferSize;
		int iISize = m_pSlaveThread->m_iInputBufferSize;
		m_pSlaveThread->m_pInfoMutex->unlock();
		KviStr tmp(KviStr::Format,__tr_ctx("Input buffer: %d bytes","dcc"),iISize);
		m_pInputLabel->setText(tmp.ptr());
		tmp.sprintf(__tr_ctx("Output buffer: %d bytes","dcc"),iOSize);
		m_pOutputLabel->setText(tmp.ptr());
	}
}

void KviDccVoice::resizeEvent(QResizeEvent *)
{
	int hght2 = m_pHBox->sizeHint().height();
	m_pHBox->setGeometry(0,0,width(),hght2);
	m_pSplitter->setGeometry(0,hght2,width(),height() - hght2);
}

QSize KviDccVoice::sizeHint() const
{
	int w = m_pIrcView->sizeHint().width();
	int w2 = m_pHBox->sizeHint().width();
	QSize ret(w > w2 ? w : w2, m_pIrcView->sizeHint().height() + m_pHBox->sizeHint().height());
	return ret;
}

void KviDccVoice::handleMarshalError(int err)
{
	QString ssss = KviError::getDescription(err);
	output(KVI_OUT_DCCERROR,__tr2qs_ctx("DCC Failed: %Q","dcc"),&ssss);
	m_pTalkButton->setEnabled(false);
	m_pTalkButton->setChecked(false);
	m_pRecordingLabel->setEnabled(false);
	m_pPlayingLabel->setEnabled(false);
}

void KviDccVoice::connected()
{
	output(KVI_OUT_DCCMSG,__tr2qs_ctx("Connected to %Q:%Q","dcc"),
		&(m_pMarshal->remoteIp()),&(m_pMarshal->remotePort()));
	output(KVI_OUT_DCCMSG,__tr2qs_ctx("Local end is %Q:%Q","dcc"),
		&(m_pMarshal->localIp()),&(m_pMarshal->localPort()));
	if(!(m_pDescriptor->bActive))
	{
		m_pDescriptor->szIp   = m_pMarshal->remoteIp();
		m_pDescriptor->szPort = m_pMarshal->remotePort();
		m_pDescriptor->szHost = m_pMarshal->remoteIp();
	}
	updateCaption();

	connect(m_pUpdateTimer,SIGNAL(timeout()),this,SLOT(updateInfo()));
	m_pUpdateTimer->start(1000);

	KviDccVoiceThreadOptions * opt = new KviDccVoiceThreadOptions;


	opt->pCodec = kvi_dcc_voice_get_codec(m_pDescriptor->szCodec.ptr());

	output(KVI_OUT_DCCMSG,__tr2qs_ctx("Actual codec used is '%s'","dcc"),opt->pCodec->name());

	opt->bForceHalfDuplex = KVI_OPTION_BOOL(KviOption_boolDccVoiceForceHalfDuplex);
//	opt->bForceDummyReadTrigger = false;
	opt->iPreBufferSize = KVI_OPTION_UINT(KviOption_uintDccVoicePreBufferSize);
	opt->szSoundDevice = KVI_OPTION_STRING(KviOption_stringDccVoiceSoundDevice).toUtf8().data();
	opt->iSampleRate = m_pDescriptor->iSampleRate;

	m_pSlaveThread = new KviDccVoiceThread(this,m_pMarshal->releaseSocket(),opt);
	connect(m_pUpdateTimer,SIGNAL(timeout()),this,SLOT(updateInfo()));
	m_pSlaveThread->start();

	m_pTalkButton->setEnabled(true);
}

void KviDccVoice::stopTalking()
{
	KviThreadDataEvent<int> * e = new KviThreadDataEvent<int>(KVI_DCC_THREAD_EVENT_ACTION);
	e->setData(new int(0));
	m_pSlaveThread->enqueueEvent(e);
}

void KviDccVoice::startTalking()
{
	KviThreadDataEvent<int> * e = new KviThreadDataEvent<int>(KVI_DCC_THREAD_EVENT_ACTION);
	e->setData(new int(1));
	m_pSlaveThread->enqueueEvent(e);
}

void KviDccVoice::startOrStopTalking(bool bStart)
{
	if(bStart)startTalking();
	else stopTalking();
}

int KviDccVoice::getMixerVolume(void) const
{
#ifndef COMPILE_DISABLE_DCC_VOICE
	int fd;
	int ret;
	int left; //, right;
	int req;

	if((fd = ::open(KVI_OPTION_STRING(KviOption_stringDccVoiceMixerDevice).toUtf8().data(), O_RDONLY)) == -1)
	{
		return 0;
	}

	req = KVI_OPTION_BOOL(KviOption_boolDccVoiceVolumeSliderControlsPCM) ? SOUND_MIXER_READ_PCM : SOUND_MIXER_READ_VOLUME;

	if(::ioctl(fd,req,&ret))
	{
		::close(fd);
		return 0;
	}

	left = (ret & 0x00ff);
//	right = (ret & 0xff00) >> 8;

	::close(fd);

	return -left;
#else
	return 0;
#endif
}

void KviDccVoice::setMixerVolume(int vol)
{
#ifndef COMPILE_DISABLE_DCC_VOICE
	int fd;
	int val;
	int req;

	if((fd = ::open(KVI_OPTION_STRING(KviOption_stringDccVoiceMixerDevice).toUtf8().data(), O_WRONLY)) == -1)
		return;

	req = KVI_OPTION_BOOL(KviOption_boolDccVoiceVolumeSliderControlsPCM) ? SOUND_MIXER_WRITE_PCM : SOUND_MIXER_WRITE_VOLUME;

	val = (-vol << 8) | -vol;
	::ioctl(fd, req, &val);
	::close(fd);

	m_pVolumeSlider->setToolTip(__tr2qs_ctx("Volume: %1","dcc").arg(-vol));
#endif
}


/* The code below doesn't work. Guess I have to catch some other widget's focusInEvent. Which one ? */
/* The point is to move the volume slider to correct position if for example user switched to
 * another KVirc window, fired up xmms, changed the volume, and returned to our dcc voice window */
void KviDccVoice::focusInEvent(QFocusEvent *e)
{
//	debug("focusInEvent()");
	m_pVolumeSlider->setValue(getMixerVolume());
	setMixerVolume(m_pVolumeSlider->value());

	KviWindow::focusInEvent(e);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_voice.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
