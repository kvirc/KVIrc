//=============================================================================
//
//   File : video.cpp
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

#include "video.h"
#include "marshal.h"
#include "broker.h"

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

#include <QToolTip>
#include <QByteArray>
#include <QBuffer>

#include <sys/ioctl.h>

extern KviDccBroker * g_pDccBroker;

#ifndef COMPILE_DISABLE_DCC_VIDEO
	Kopete::AV::VideoDevicePool *g_pVideoDevicePool=0;
	int g_iVideoDevicePoolInstances=0;
#endif

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

bool kvi_dcc_video_is_valid_codec(const char * codecName)
{
	if(kvi_strEqualCI("sjpeg",codecName))return true;
#ifndef COMPILE_DISABLE_OGG_THEORA
	if(kvi_strEqualCI("theora",codecName))return true;
#endif
	return false;
}

static KviDccVideoCodec * kvi_dcc_video_get_codec(const char *codecName)
{
#ifndef COMPILE_DISABLE_OGG_THEORA
	if(kvi_strEqualCI("theora",codecName))return new KviDccVideoTheoraCodec();
#endif
	return new KviDccVideoSJpegCodec();
}


KviDccVideoThread::KviDccVideoThread(KviWindow * wnd,kvi_socket_t fd,KviDccVideoThreadOptions * opt)
: KviDccThread(wnd,fd)
{
#ifndef COMPILE_DISABLE_DCC_VIDEO
	m_pOpt = opt;
	m_bPlaying = false;
	m_bRecording = false;
	m_pInfoMutex = new KviMutex();
	m_bRecordingRequestPending = false;

#ifndef COMPILE_DISABLE_DCC_VOICE
	m_bSoundcardChecked = false;
	m_soundFd = -1;
	m_soundFdMode = 0;
#endif
	//local video input
	if(!g_pVideoDevicePool)
	{
		g_pVideoDevicePool = Kopete::AV::VideoDevicePool::self();
		g_pVideoDevicePool->open();
		g_pVideoDevicePool->setSize(320, 240);
	}
	g_iVideoDevicePoolInstances++;
	//ensure capturing
	g_pVideoDevicePool->startCapturing();
#endif
	startRecording();
	startPlaying();

}

KviDccVideoThread::~KviDccVideoThread()
{
	stopRecording();
#ifndef COMPILE_DISABLE_DCC_VIDEO
	g_iVideoDevicePoolInstances--;
	if(g_iVideoDevicePoolInstances==0)
		g_pVideoDevicePool->close();

	delete m_pOpt->pCodec;
	delete m_pOpt;
	delete m_pInfoMutex;
#endif
}


bool KviDccVideoThread::checkSoundcard()
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


bool KviDccVideoThread::openSoundcard(int mode)
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

bool KviDccVideoThread::openSoundcardForWriting()
{
#ifndef COMPILE_DISABLE_DCC_VOICE
	return openSoundcardWithDuplexOption(O_WRONLY,O_RDONLY);
#else
	return false;
#endif
}

bool KviDccVideoThread::openSoundcardForReading()
{
#ifndef COMPILE_DISABLE_DCC_VOICE
	return openSoundcardWithDuplexOption(O_RDONLY,O_WRONLY);
#else
	return false;
#endif
}

bool KviDccVideoThread::openSoundcardWithDuplexOption(int openMode,int failMode)
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

void KviDccVideoThread::closeSoundcard()
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


bool KviDccVideoThread::readWriteStep()
{
#ifndef COMPILE_DISABLE_DCC_VIDEO
	// Socket management
	bool bCanRead;
	bool bCanWrite;

	if(kvi_select(m_fd,&bCanRead,&bCanWrite))
	{
		while(bCanRead)
		{
			unsigned int actualSize = m_inFrameBuffer.size();
			m_inFrameBuffer.resize(actualSize + 4096);
			int readLen = kvi_socket_recv(m_fd,(void *)(m_inFrameBuffer.data() + actualSize),4096);
			if(readLen > 0)
			{
				if(readLen < 4096)m_inFrameBuffer.resize(actualSize + readLen);
				m_pOpt->pCodec->decode(&m_inFrameBuffer,&m_inSignalBuffer);
//#warning "A maximum length for the signal buffer is actually needed!!!"
			} else {
				bCanRead=false;
// 				if(!handleInvalidSocketRead(readLen))return false;
				m_inFrameBuffer.resize(actualSize);
			}
		}

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
			}
		}
//#warning "Usleep here ?"
	}
#endif // !COMPILE_DISABLE_DCC_VOICE
	return true;
}

bool KviDccVideoThread::videoStep()
{
#ifndef COMPILE_DISABLE_DCC_VIDEO
	// Are we playing ?
	if(m_bPlaying)
	{
		if(m_inSignalBuffer.size() > 0)
		{
			QImage img(m_inSignalBuffer.data(), 320, 240, 1280, QImage::Format_ARGB32);
// 			qDebug("IMG data %p size %d", m_inSignalBuffer.data(), m_inSignalBuffer.size());
			if(!img.isNull())
				m_inImage = img;
		}
	}

	// Are we recording ?
	if(m_bRecording)
	{
		//grab the image, compress using the codec
		g_pVideoDevicePool->getFrame();
		g_pVideoDevicePool->getImage(&m_outImage);
		if(m_outImage.numBytes() > 0)
		{
			m_videoOutSignalBuffer.append((const unsigned char*) m_outImage.bits(), m_outImage.numBytes());
			m_pOpt->pCodec->encodeVideo(&m_videoOutSignalBuffer,&m_outFrameBuffer);
		}
	}

#endif // !COMPILE_DISABLE_DCC_VIDEO
	return true;
}

bool KviDccVideoThread::audioStep()
{
#ifndef COMPILE_DISABLE_DCC_VIDEO
#ifndef COMPILE_DISABLE_DCC_VOICE
	// Are we playing ?
/*
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
*/
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
				int oldSize = m_audioOutSignalBuffer.size();
				int available = info.fragments * info.fragsize;
				m_audioOutSignalBuffer.addSize(available);
				int readed = read(m_soundFd,m_audioOutSignalBuffer.data() + oldSize,available);

				if(readed < available)
				{
					// huh ? ...error ?
					if(readed >= 0)m_audioOutSignalBuffer.resize(oldSize + readed);
					else {
						if((errno == EINTR) || (errno == EAGAIN))
						{
							m_audioOutSignalBuffer.resize(oldSize);
						} else {
//#warning "Critical error...do something reasonable!"
							m_audioOutSignalBuffer.resize(oldSize);
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
				m_pOpt->pCodec->encodeAudio(&m_audioOutSignalBuffer,&m_outFrameBuffer);
			}
		}// else {
			// Nothing to read
	//		m_uSleepTime += 100;
	//	}
	}

#endif // !COMPILE_DISABLE_DCC_VOICE
#endif // !COMPILE_DISABLE_DCC_VIDEO
	return true;
}

void KviDccVideoThread::startRecording()
{
#ifndef COMPILE_DISABLE_DCC_VIDEO
	//debug("Start recording");
	if(m_bRecording)return; // already started

	//FIXME this is mandatory only for audio!
	openSoundcardForReading();

	
	{
//		debug("Posting event");
		KviThreadDataEvent<int> * e = new KviThreadDataEvent<int>(KVI_DCC_THREAD_EVENT_ACTION);
		e->setData(new int(KVI_DCC_VIDEO_THREAD_ACTION_START_RECORDING));
		postEvent(KviDccThread::parent(),e);

		m_bRecording = true;
		m_bRecordingRequestPending = false;
// 	} else {
// 		m_bRecordingRequestPending = true;
	}
#endif
}

void KviDccVideoThread::stopRecording()
{
#ifndef COMPILE_DISABLE_DCC_VIDEO
	//debug("Stop recording");
	m_bRecordingRequestPending = false;
	if(!m_bRecording)return; // already stopped

	KviThreadDataEvent<int> * e = new KviThreadDataEvent<int>(KVI_DCC_THREAD_EVENT_ACTION);
	e->setData(new int(KVI_DCC_VIDEO_THREAD_ACTION_STOP_RECORDING));
	postEvent(KviDccThread::parent(),e);

	m_bRecording = false;
	//FIXME this is mandatory only if we are using audio
	if(!m_bPlaying)closeSoundcard();
#endif
}

void KviDccVideoThread::startPlaying()
{
#ifndef COMPILE_DISABLE_DCC_VIDEO
	//debug("Start playing");
	if(m_bPlaying)return;
	//FIXME this is mandatory only if we are using audio
	openSoundcardForWriting();
	//if()
	{
		KviThreadDataEvent<int> * e = new KviThreadDataEvent<int>(KVI_DCC_THREAD_EVENT_ACTION);
		e->setData(new int(KVI_DCC_VIDEO_THREAD_ACTION_START_PLAYING));
		postEvent(KviDccThread::parent(),e);
		m_bPlaying = true;
	}
#endif
}

void KviDccVideoThread::stopPlaying()
{
#ifndef COMPILE_DISABLE_DCC_VIDEO
	//debug("Stop playing");
	if(!m_bPlaying)return;

	KviThreadDataEvent<int> * e = new KviThreadDataEvent<int>(KVI_DCC_THREAD_EVENT_ACTION);
	e->setData(new int(KVI_DCC_VIDEO_THREAD_ACTION_STOP_PLAYING));
	postEvent(KviDccThread::parent(),e);

	m_bPlaying = false;
	//FIXME this is mandatory only if we are using audio
	if(!m_bRecording)closeSoundcard();
#endif
}

void KviDccVideoThread::run()
{
#ifndef COMPILE_DISABLE_DCC_VIDEO
	for(;;)
	{
		m_uSleepTime = 0;

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
		//if(!videoStep() || !audioStep())goto exit_dcc;
		if(!videoStep())goto exit_dcc;

		m_pInfoMutex->lock();
		m_pInfoMutex->unlock();

		//if(m_uSleepTime)usleep(m_uSleepTime);
// 		qDebug("in %d out %d in_sig %d out_sig %d", m_inFrameBuffer.size(), m_outFrameBuffer.size(), m_inSignalBuffer.size(), m_videoOutSignalBuffer.size());
		usleep(200);

		// Start recording if the request was not fulfilled yet
		if(m_bRecordingRequestPending)startRecording();
	}


exit_dcc:

#endif //! COMPILE_DISABLE_DCC_VIDEO
	kvi_socket_close(m_fd);
	m_fd = KVI_INVALID_SOCKET;
}

KviDccVideo::KviDccVideo(KviFrame *pFrm,KviDccDescriptor * dcc,const char * name)
: KviDccWindow(KVI_WINDOW_TYPE_DCCVOICE,pFrm,name,dcc)
{
	m_pDescriptor = dcc;
	m_pSlaveThread = 0;
	m_pszTarget = 0;

	m_pLayout = new QGridLayout();

	//remote video
	m_pInVideoLabel = new QLabel();
	m_pInVideoLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_pInVideoLabel->setMinimumSize(320, 240);
	m_pInVideoLabel->setFrameShape(QFrame::Box);
	m_pInVideoLabel->setScaledContents(true);
	m_pInVideoLabel->setAlignment(Qt::AlignCenter);
	m_pLayout->addWidget(m_pInVideoLabel, 0, 0, 10, 1);
	
	//local video
	m_pOutVideoLabel = new QLabel();
	m_pOutVideoLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_pOutVideoLabel->setMinimumSize(320, 240);
	m_pOutVideoLabel->setFrameShape(QFrame::Box);
	m_pOutVideoLabel->setScaledContents(true);
	m_pOutVideoLabel->setAlignment(Qt::AlignCenter);
	m_pLayout->addWidget(m_pOutVideoLabel, 11, 0, 10, 1);

	//local video input: config
	m_pLabel[0] = new QLabel();
	m_pLabel[0]->setText(__tr2qs_ctx("Video device:","dcc"));
	m_pLayout->addWidget(m_pLabel[0], 11, 1, 1, 1);
	
	m_pCDevices = new QComboBox();
	m_pLayout->addWidget(m_pCDevices, 12, 1, 1, 1);

	m_pLabel[1] = new QLabel();
	m_pLabel[1]->setText(__tr2qs_ctx("Input:","dcc"));
	m_pLayout->addWidget(m_pLabel[1], 13, 1, 1, 1);

	m_pCInputs = new QComboBox();
	m_pLayout->addWidget(m_pCInputs, 14, 1, 1, 1);

	m_pLabel[2] = new QLabel();
	m_pLabel[2]->setText(__tr2qs_ctx("Standard:","dcc"));
	m_pLayout->addWidget(m_pLabel[2], 15, 1, 1, 1);

	m_pCStandards = new QComboBox();
	m_pLayout->addWidget(m_pCStandards, 16, 1, 1, 1);

	setLayout(m_pLayout);

	connect(&m_Timer, SIGNAL(timeout()), this, SLOT(slotUpdateImage()) );

	m_Timer.start(200); //5fps

	m_pMarshal = new KviDccMarshal(this);
	connect(m_pMarshal,SIGNAL(error(int)),this,SLOT(handleMarshalError(int)));
	connect(m_pMarshal,SIGNAL(connected()),this,SLOT(connected()));
	connect(m_pMarshal,SIGNAL(inProgress()),this,SLOT(connectionInProgress()));

	startConnection();
}

KviDccVideo::~KviDccVideo()
{
	if(m_pInVideoLabel)
	{
		delete m_pInVideoLabel;
		m_pInVideoLabel=0;
	}
	if(m_pOutVideoLabel)
	{
		delete m_pOutVideoLabel;
		m_pOutVideoLabel=0;
	}
	if(m_pCDevices)
	{
		delete m_pCDevices;
		m_pCDevices=0;
	}
	if(m_pCInputs)
	{
		delete m_pCInputs;
		m_pCInputs=0;
	}
	if(m_pCStandards)
	{
		delete m_pCStandards;
		m_pCStandards=0;
	}
	if(m_pLabel[0])
	{
		delete m_pLabel[2];
		delete m_pLabel[1];
		delete m_pLabel[0];
		m_pLabel[2] = 0;
		m_pLabel[1] = 0;
		m_pLabel[0] = 0;
	}
	if(m_pLayout)
	{
		delete m_pLayout;
		m_pLayout=0;
	}

	g_pDccBroker->unregisterDccWindow(this);

	if(m_pSlaveThread)
	{
		m_pSlaveThread->terminate();
		delete m_pSlaveThread;
		m_pSlaveThread = 0;
	}

	KviThreadManager::killPendingEvents(this);

	if(m_pszTarget)
	{
		delete m_pszTarget;
		m_pszTarget = 0;
	}
}

void KviDccVideo::startConnection()
{
	if(!(m_pDescriptor->bActive))
	{
		// PASSIVE CONNECTION
		output(KVI_OUT_DCCMSG,__tr2qs_ctx("Attempting a passive DCC VIDEO connection","dcc"));
		int ret = m_pMarshal->dccListen(m_pDescriptor->szListenIp,m_pDescriptor->szListenPort,m_pDescriptor->bDoTimeout);
		if(ret != KviError_success)handleMarshalError(ret);
	} else {
		// ACTIVE CONNECTION
		output(KVI_OUT_DCCMSG,__tr2qs_ctx("Attempting an active DCC VIDEO connection","dcc"));
		int ret = m_pMarshal->dccConnect(m_pDescriptor->szIp.toUtf8().data(),m_pDescriptor->szPort.toUtf8().data(),m_pDescriptor->bDoTimeout);
		if(ret != KviError_success)handleMarshalError(ret);
	}
}

void KviDccVideo::connectionInProgress()
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

			m_pDescriptor->console()->connection()->sendFmtData("PRIVMSG %s :%cDCC VIDEO %s %Q %s %d%c",
					m_pDescriptor->console()->connection()->encodeText(m_pDescriptor->szNick).data(),
					0x01,m_pDescriptor->szCodec.ptr(),
					&ip,port.ptr(),m_pDescriptor->iSampleRate,0x01);
			output(KVI_OUT_DCCMSG,__tr2qs_ctx("Sent DCC VIDEO (%s) request to %Q, waiting for the remote client to connect...","dcc"),
					m_pDescriptor->szCodec.ptr(),&(m_pDescriptor->szNick));
		} else output(KVI_OUT_DCCMSG,__tr2qs_ctx("DCC VIDEO request not sent: awaiting manual connections","dcc"));
	}
}

const QString & KviDccVideo::target()
{
	// This may change on the fly...
	if(!m_pszTarget)
		m_pszTarget = new QString();
		
	m_pszTarget->sprintf("%s@%s:%s",m_pDescriptor->szNick.toUtf8().data(),m_pDescriptor->szIp.toUtf8().data(),m_pDescriptor->szPort.toUtf8().data());
	return *m_pszTarget;
}

void KviDccVideo::getBaseLogFileName(QString &buffer)
{
	buffer.sprintf("dccvideo_%s_%s_%s",m_pDescriptor->szNick.toUtf8().data(),m_pDescriptor->szLocalFileName.toUtf8().data(),m_pDescriptor->szPort.toUtf8().data());
}

void KviDccVideo::fillCaptionBuffers()
{
	KviStr tmp(KviStr::Format,"DCC Video %s@%s:%s %s",
		m_pDescriptor->szNick.toUtf8().data(),m_pDescriptor->szIp.toUtf8().data(),m_pDescriptor->szPort.toUtf8().data(),
		m_pDescriptor->szLocalFileName.toUtf8().data());

	m_szPlainTextCaption = tmp;
}

QPixmap * KviDccVideo::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KVI_SMALLICON_DCCVOICE);
}

bool KviDccVideo::event(QEvent *e)
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
/*				m_pTalkButton->setEnabled(false);
				m_pRecordingLabel->setEnabled(false);
				m_pPlayingLabel->setEnabled(false);*/
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
					case KVI_DCC_VIDEO_THREAD_ACTION_START_RECORDING:
// 						m_pRecordingLabel->setEnabled(true);
					break;
					case KVI_DCC_VIDEO_THREAD_ACTION_STOP_RECORDING:
// 						m_pRecordingLabel->setEnabled(false);
					break;
					case KVI_DCC_VIDEO_THREAD_ACTION_START_PLAYING:
// 						m_pPlayingLabel->setEnabled(true);
					break;
					case KVI_DCC_VIDEO_THREAD_ACTION_STOP_PLAYING:
// 						m_pPlayingLabel->setEnabled(false);
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

void KviDccVideo::handleMarshalError(int err)
{
	QString ssss = KviError::getDescription(err);
	output(KVI_OUT_DCCERROR,__tr2qs_ctx("DCC Failed: %Q","dcc"),&ssss);
/*	m_pTalkButton->setEnabled(false);
	m_pTalkButton->setChecked(false);
	m_pRecordingLabel->setEnabled(false);
	m_pPlayingLabel->setEnabled(false);*/
}

void KviDccVideo::connected()
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

	KviDccVideoThreadOptions * opt = new KviDccVideoThreadOptions;

	opt->pCodec = kvi_dcc_video_get_codec(m_pDescriptor->szCodec.ptr());
	opt->bForceHalfDuplex = KVI_OPTION_BOOL(KviOption_boolDccVoiceForceHalfDuplex);
	opt->iPreBufferSize = KVI_OPTION_UINT(KviOption_uintDccVoicePreBufferSize);
	opt->szSoundDevice = KVI_OPTION_STRING(KviOption_stringDccVoiceSoundDevice).toUtf8().data();
	opt->iSampleRate = 8000;

	output(KVI_OUT_DCCMSG,__tr2qs_ctx("Actual codec used is '%s'","dcc"),opt->pCodec->name());

	m_pSlaveThread = new KviDccVideoThread(this,m_pMarshal->releaseSocket(),opt);

#ifndef COMPILE_DISABLE_DCC_VIDEO
	if(g_pVideoDevicePool)
	{
		g_pVideoDevicePool->fillDeviceQComboBox(m_pCDevices);
		g_pVideoDevicePool->fillInputQComboBox(m_pCInputs);
		g_pVideoDevicePool->fillStandardQComboBox(m_pCStandards);

		connect(g_pVideoDevicePool, SIGNAL(deviceRegistered(const QString &) ), SLOT(deviceRegistered(const QString &)) );
		connect(g_pVideoDevicePool, SIGNAL(deviceUnregistered(const QString &) ), SLOT(deviceUnregistered(const QString &)) );
	}	
#endif

	m_pSlaveThread->start();

// 	m_pTalkButton->setEnabled(true);
}

void KviDccVideo::stopTalking()
{
	KviThreadDataEvent<int> * e = new KviThreadDataEvent<int>(KVI_DCC_THREAD_EVENT_ACTION);
	e->setData(new int(0));
	m_pSlaveThread->enqueueEvent(e);
}

void KviDccVideo::startTalking()
{
	KviThreadDataEvent<int> * e = new KviThreadDataEvent<int>(KVI_DCC_THREAD_EVENT_ACTION);
	e->setData(new int(1));
	m_pSlaveThread->enqueueEvent(e);
}

void KviDccVideo::startOrStopTalking(bool bStart)
{
	if(bStart)startTalking();
	else stopTalking();
}

void KviDccVideo::slotUpdateImage()
{
#ifndef COMPILE_DISABLE_DCC_VIDEO
	if(m_pSlaveThread && isVisible())
	{
		m_pOutVideoLabel->setPixmap(QPixmap::fromImage(m_pSlaveThread->m_outImage));
		m_pInVideoLabel->setPixmap(QPixmap::fromImage(m_pSlaveThread->m_inImage));
	}
#endif
}

void KviDccVideo::deviceRegistered(const QString &)
{
	/*
#ifndef COMPILE_DISABLE_DCC_VIDEO
	g_pVideoDevicePool->fillDeviceQComboBox(m_pCDevices);
	g_pVideoDevicePool->fillInputQComboBox(m_pCInputs);
	g_pVideoDevicePool->fillStandardQComboBox(m_pCStandards);

	// update the mVideoImageLabel to show the camera frames
	g_pVideoDevicePool->open();
	g_pVideoDevicePool->setSize(320, 240);
	g_pVideoDevicePool->startCapturing();

// 	setVideoInputParameters();

// 	if ( g_pVideoDevicePool->hasDevices() )
// 	{
// 		m_Timer.start(200); //5fps
// 	}
#endif
#*/
}


void KviDccVideo::deviceUnregistered(const QString & )
{
/*
#ifndef COMPILE_DISABLE_DCC_VIDEO
	g_pVideoDevicePool->fillDeviceQComboBox(m_pCDevices);
	g_pVideoDevicePool->fillInputQComboBox(m_pCInputs);
	g_pVideoDevicePool->fillStandardQComboBox(m_pCStandards);
#endif
*/
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_video.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
