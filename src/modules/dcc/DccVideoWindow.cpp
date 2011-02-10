//=============================================================================
//
//   File : DccVideoWindow.cpp
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

#include "kvi_settings.h"

#include "DccVideoWindow.h"
#include "DccMarshal.h"
#include "DccBroker.h"

#include "KviIconManager.h"
#include "KviIrcView.h"
#include "KviKvsEventTriggers.h"
#include "KviLocale.h"
#include "kvi_out.h"
#include "KviNetUtils.h"
#include "KviOptions.h"
#include "KviConsoleWindow.h"
#include "KviMemory.h"
#include "kvi_socket.h"
#include "KviIrcConnection.h"
#include "KviTalVBox.h"
#include "KviControlCodes.h"
#include "KviMainWindow.h"

#include <QToolTip>
#include <QByteArray>
#include <QBuffer>
#include <QTextDocument> //for Qt::escape

#ifdef COMPILE_CRYPT_SUPPORT
	#include "KviCryptController.h"
#endif

#ifdef COMPILE_SSL_SUPPORT
	#include "KviSSLMaster.h"
#endif

#include <sys/ioctl.h>

#define FRAME_DURATION 200 // 1 / 5fps = 200msec

extern DccBroker * g_pDccBroker;

#ifndef COMPILE_DISABLE_DCC_VIDEO
	Kopete::AV::VideoDevicePool *g_pVideoDevicePool=0;
	int g_iVideoDevicePoolInstances=0;
#endif

bool kvi_dcc_video_is_valid_codec(const char * codecName)
{
	if(kvi_strEqualCI("sjpeg",codecName))return true;
#ifndef COMPILE_DISABLE_OGG_THEORA
	if(kvi_strEqualCI("theora",codecName))return true;
#endif
	return false;
}

static DccVideoCodec * kvi_dcc_video_get_codec(const char *codecName)
{
#ifndef COMPILE_DISABLE_OGG_THEORA
	if(kvi_strEqualCI("theora",codecName))return new DccVideoTheoraCodec();
#endif
	return new DccVideoSJpegCodec();
}


DccVideoThread::DccVideoThread(KviWindow * wnd,kvi_socket_t fd,KviDccVideoThreadOptions * opt)
: DccThread(wnd,fd)
{
#ifndef COMPILE_DISABLE_DCC_VIDEO
	m_pOpt = opt;
	m_bPlaying = false;
	m_bRecording = false;

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

DccVideoThread::~DccVideoThread()
{
	stopRecording();
#ifndef COMPILE_DISABLE_DCC_VIDEO
	g_iVideoDevicePoolInstances--;
	if(g_iVideoDevicePoolInstances==0)
		g_pVideoDevicePool->close();

	delete m_pOpt->pCodec;
	delete m_pOpt;
#endif
}

bool DccVideoThread::readWriteStep()
{
//	qDebug("DccVideoThread::readWriteStep()");
#ifndef COMPILE_DISABLE_DCC_VIDEO
	// Socket management
	bool bCanRead;
	bool bCanWrite;

	if(kvi_select(m_fd,&bCanRead,&bCanWrite))
	{
		while(bCanRead)
		{
			unsigned int actualSize = m_inFrameBuffer.size();
			m_inFrameBuffer.resize(actualSize + 16384);
			int readLen = kvi_socket_recv(m_fd,(void *)(m_inFrameBuffer.data() + actualSize),16384);
			if(readLen > 0)
			{
				if(readLen < 16384)m_inFrameBuffer.resize(actualSize + readLen);
				m_pOpt->pCodec->decode(&m_inFrameBuffer,&m_videoInSignalBuffer,&m_textInSignalBuffer);
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
	}
#endif // !COMPILE_DISABLE_DCC_VOICE
	return true;
}

bool DccVideoThread::videoStep()
{
//	qDebug("DccVideoThread::videoStep()");
#ifndef COMPILE_DISABLE_DCC_VIDEO
	// Are we playing ?
	if(m_bPlaying)
	{
		if(m_videoInSignalBuffer.size() > 0)
		{
			QImage img(m_videoInSignalBuffer.data(), 320, 240, 1280, QImage::Format_ARGB32);
// 			qDebug("IMG data %p size %d", m_videoInSignalBuffer.data(), m_videoInSignalBuffer.size());
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

bool DccVideoThread::textStep()
{
//	qDebug("DccVideoThread::textStep()");
#ifndef COMPILE_DISABLE_DCC_VIDEO
	// Are we playing ?
	if(m_bPlaying)
	{
		if(m_textInSignalBuffer.size() > 0)
		{
			KviDccThreadIncomingData data;
			data.iLen = m_textInSignalBuffer.size();
			data.buffer = (char*) KviMemory::allocate(data.iLen);
			memcpy(data.buffer, m_textInSignalBuffer.data(), data.iLen);
			handleIncomingData(&data,false);

			m_textInSignalBuffer.clear();
		}
	}

	// Are we recording ?
	if(m_bRecording)
	{
		if(((DccVideoWindow*)parent())->m_tmpTextDataOut.size())
		{
			m_textOutSignalBuffer.append((const unsigned char*) ((DccVideoWindow*)parent())->m_tmpTextDataOut.constData(), ((DccVideoWindow*)parent())->m_tmpTextDataOut.size());
			((DccVideoWindow*)parent())->m_tmpTextDataOut.clear();
		}

		if(m_textOutSignalBuffer.size())
			m_pOpt->pCodec->encodeText(&m_textOutSignalBuffer,&m_outFrameBuffer);
	}

#endif // !COMPILE_DISABLE_DCC_VIDEO
	return true;
}

bool DccVideoThread::handleIncomingData(KviDccThreadIncomingData * data,bool bCritical)
{
//	qDebug("DccVideoThread::handleIncomingData");
	KVI_ASSERT(data->iLen);
	KVI_ASSERT(data->buffer);
	char * aux = data->buffer;
	char * end = data->buffer + data->iLen;
	while(aux != end)
	{
		if((*aux == '\n') || (*aux == '\0'))
		{
			KviThreadDataEvent<KviCString> * e = new KviThreadDataEvent<KviCString>(KVI_DCC_THREAD_EVENT_DATA);
			// The left part is len chars long
			int len = aux - data->buffer;
//			qDebug("LEN = %d, iLen = %d",len,data->iLen);
//#warning "DO IT BETTER (the \r cutting)"
			KviCString * s = new KviCString(data->buffer,len);
			if(s->lastCharIs('\r'))s->cutRight(1);
			e->setData(s);
			// but we cut also \n (or \0)
			++aux;
			// so len += 1; --> new data->iLen -= len;
			data->iLen -= (len + 1);
//			qDebug("iLen now = %d",data->iLen);
			KVI_ASSERT(data->iLen >= 0);
			if(data->iLen > 0)
			{
				// memmove the remaining part to the beginning
				// aux points after \n or \0
				KviMemory::move(data->buffer,aux,data->iLen);
				data->buffer = (char *)KviMemory::reallocate(data->buffer,data->iLen);
				end = data->buffer + data->iLen;
				aux = data->buffer;
			} else {
				// no more data in the buffer
				KVI_ASSERT(data->iLen == 0);
				KviMemory::free(data->buffer);
				data->buffer = end = aux = 0;
			}
			postEvent(parent(),e);
		} else aux++;
//		qDebug("PASSING CHAR %c",*aux);
	}
	// now aux == end
	if(bCritical)
	{
		// need to flush everything...
		if(data->iLen > 0)
		{
			// in the last part there are no NULL and \n chars
			KviThreadDataEvent<KviCString> * e = new KviThreadDataEvent<KviCString>(KVI_DCC_THREAD_EVENT_DATA);
			KviCString * s = new KviCString(data->buffer,data->iLen);
			if(s->lastCharIs('\r'))s->cutRight(1);
			e->setData(s);
			data->iLen = 0;
			KviMemory::free(data->buffer);
			data->buffer = 0;
			postEvent(parent(),e);
		}
	}
	return true;
}

void DccVideoThread::restartRecording(int iDevice, int iInput, int)
{
#ifndef COMPILE_DISABLE_DCC_VIDEO
	m_bRecording=false;
	if(!g_pVideoDevicePool)
		g_pVideoDevicePool = Kopete::AV::VideoDevicePool::self();

	g_pVideoDevicePool->stopCapturing();
	g_pVideoDevicePool->open(iDevice);
	g_pVideoDevicePool->setSize(320, 240);
	if(iInput >= 0)
		g_pVideoDevicePool->selectInput(iInput);
	g_pVideoDevicePool->startCapturing();

	m_bRecording=true;
#endif
}

void DccVideoThread::startRecording()
{
#ifndef COMPILE_DISABLE_DCC_VIDEO
	//qDebug("Start recording");
	if(m_bRecording)return; // already started

//	qDebug("Posting event");
	KviThreadDataEvent<int> * e = new KviThreadDataEvent<int>(KVI_DCC_THREAD_EVENT_ACTION);
	e->setData(new int(KVI_DCC_VIDEO_THREAD_ACTION_START_RECORDING));
	postEvent(DccThread::parent(),e);

	m_bRecording = true;
#endif
}

void DccVideoThread::stopRecording()
{
#ifndef COMPILE_DISABLE_DCC_VIDEO
	//qDebug("Stop recording");
	if(!m_bRecording)return; // already stopped

	KviThreadDataEvent<int> * e = new KviThreadDataEvent<int>(KVI_DCC_THREAD_EVENT_ACTION);
	e->setData(new int(KVI_DCC_VIDEO_THREAD_ACTION_STOP_RECORDING));
	postEvent(DccThread::parent(),e);

	m_bRecording = false;
#endif
}

void DccVideoThread::startPlaying()
{
#ifndef COMPILE_DISABLE_DCC_VIDEO
	//qDebug("Start playing");
	if(m_bPlaying)return;

	KviThreadDataEvent<int> * e = new KviThreadDataEvent<int>(KVI_DCC_THREAD_EVENT_ACTION);
	e->setData(new int(KVI_DCC_VIDEO_THREAD_ACTION_START_PLAYING));
	postEvent(DccThread::parent(),e);
	m_bPlaying = true;

#endif
}

void DccVideoThread::stopPlaying()
{
#ifndef COMPILE_DISABLE_DCC_VIDEO
	//qDebug("Stop playing");
	if(!m_bPlaying)return;

	KviThreadDataEvent<int> * e = new KviThreadDataEvent<int>(KVI_DCC_THREAD_EVENT_ACTION);
	e->setData(new int(KVI_DCC_VIDEO_THREAD_ACTION_STOP_PLAYING));
	postEvent(DccThread::parent(),e);
	m_bPlaying = false;
#endif
}

void DccVideoThread::run()
{
// qDebug("DccVideoThread::run()");
#ifndef COMPILE_DISABLE_DCC_VIDEO
	for(;;)
	{
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
		if(!videoStep())goto exit_dcc;
		if(!textStep())goto exit_dcc;

		usleep(FRAME_DURATION*1000);
// 		qDebug("in %d out %d in_sig %d out_sig %d", m_inFrameBuffer.size(), m_outFrameBuffer.size(), m_videoInSignalBuffer.size(), m_videoOutSignalBuffer.size());
	}


exit_dcc:

#endif //! COMPILE_DISABLE_DCC_VIDEO
	kvi_socket_close(m_fd);
	m_fd = KVI_INVALID_SOCKET;
}

DccVideoWindow::DccVideoWindow(KviMainWindow *pFrm,DccDescriptor * dcc,const char * name)
: DccWindow(KviWindow::DccVideo,pFrm,name,dcc)
{
	m_pDescriptor = dcc;
	m_pSlaveThread = 0;
	m_pszTarget = 0;

	m_pLayout = new QGridLayout(this);

	m_pButtonBox = new KviTalHBox(this);

	m_pLabel = new KviThemedLabel(m_pButtonBox, this, "dcc_chat_label");
	m_pLabel->setText(name);
	m_pButtonBox->setStretchFactor(m_pLabel,1);

	m_pButtonContainer= new KviTalHBox(m_pButtonBox);
	createTextEncodingButton(m_pButtonContainer);

#ifdef COMPILE_CRYPT_SUPPORT
	createCryptControllerButton(m_pButtonContainer);
#endif
	m_pLayout->addWidget(m_pButtonBox, 0, 0, 1, 3);

	m_pIrcView  = new KviIrcView(this,pFrm,this);
	connect(m_pIrcView,SIGNAL(rightClicked()),this,SLOT(textViewRightClicked()));
	m_pInput    = new KviInput(this);

	//remote video
	m_pInVideoLabel = new QLabel();
	m_pInVideoLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_pInVideoLabel->setMinimumSize(320, 240);
	m_pInVideoLabel->setFrameShape(QFrame::Box);
	m_pInVideoLabel->setScaledContents(true);
	m_pInVideoLabel->setAlignment(Qt::AlignCenter);
	m_pLayout->addWidget(m_pInVideoLabel, 1, 0, 6, 1);

	//local video
	m_pOutVideoLabel = new QLabel();
	m_pOutVideoLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_pOutVideoLabel->setMinimumSize(320, 240);
	m_pOutVideoLabel->setFrameShape(QFrame::Box);
	m_pOutVideoLabel->setScaledContents(true);
	m_pOutVideoLabel->setAlignment(Qt::AlignCenter);
	m_pLayout->addWidget(m_pOutVideoLabel, 1, 1, 6, 1);

	//local video input: config
	m_pVideoLabel[0] = new QLabel();
	m_pVideoLabel[0]->setText(__tr2qs_ctx("Video device:","dcc"));
	m_pLayout->addWidget(m_pVideoLabel[0], 1, 2, 1, 1);

	m_pCDevices = new QComboBox();
	m_pLayout->addWidget(m_pCDevices, 2, 2, 1, 1);

	m_pVideoLabel[1] = new QLabel();
	m_pVideoLabel[1]->setText(__tr2qs_ctx("Input:","dcc"));
	m_pLayout->addWidget(m_pVideoLabel[1], 3, 2, 1, 1);

	m_pCInputs = new QComboBox();
	m_pLayout->addWidget(m_pCInputs, 4, 2, 1, 1);

	m_pVideoLabel[2] = new QLabel();
	m_pVideoLabel[2]->setText(__tr2qs_ctx("Standard:","dcc"));
	m_pLayout->addWidget(m_pVideoLabel[2], 5, 2, 1, 1);

	m_pCStandards = new QComboBox();
	m_pLayout->addWidget(m_pCStandards, 6, 2, 1, 1);

	m_pLayout->addWidget(m_pIrcView, 7, 0, 1, 3);
	m_pLayout->addWidget(m_pInput, 8, 0, 1, 3);
	m_pLayout->setRowStretch(7,1);

	setLayout(m_pLayout);

	if(KVI_OPTION_BOOL(KviOption_boolAutoLogDccChat))m_pIrcView->startLogging();

	connect(&m_Timer, SIGNAL(timeout()), this, SLOT(slotUpdateImage()) );

	m_Timer.start(FRAME_DURATION);

	m_pMarshal = new DccMarshal(this);
	connect(m_pMarshal,SIGNAL(error(KviError::Code)),this,SLOT(handleMarshalError(KviError::Code)));
	connect(m_pMarshal,SIGNAL(connected()),this,SLOT(connected()));
	connect(m_pMarshal,SIGNAL(inProgress()),this,SLOT(connectionInProgress()));

	connect(m_pCDevices, SIGNAL(currentIndexChanged(int)), this, SLOT(videoInputChanged(int)));
	connect(m_pCInputs, SIGNAL(currentIndexChanged(int)), this, SLOT(videoInputChanged(int)));
	connect(m_pCStandards, SIGNAL(currentIndexChanged(int)), this, SLOT(videoInputChanged(int)));

	startConnection();
}

DccVideoWindow::~DccVideoWindow()
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
	if(m_pVideoLabel[0])
	{
		delete m_pVideoLabel[2];
		delete m_pVideoLabel[1];
		delete m_pVideoLabel[0];
		m_pVideoLabel[2] = 0;
		m_pVideoLabel[1] = 0;
		m_pVideoLabel[0] = 0;
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

void DccVideoWindow::textViewRightClicked()
{
	KVS_TRIGGER_EVENT_1(KviEvent_OnDCCChatPopupRequest,this,m_pDescriptor->idString());
}

void DccVideoWindow::triggerCreationEvents()
{
	KVS_TRIGGER_EVENT_1(KviEvent_OnDCCChatWindowCreated,this,m_pDescriptor->idString());
}

void DccVideoWindow::triggerDestructionEvents()
{
	KVS_TRIGGER_EVENT_1(KviEvent_OnDCCChatWindowClosing,this,m_pDescriptor->idString());
}

void DccVideoWindow::startConnection()
{
	if(!(m_pDescriptor->bActive))
	{
		// PASSIVE CONNECTION
		output(KVI_OUT_DCCMSG,__tr2qs_ctx("Attempting a passive DCC VIDEO connection","dcc"));
		KviError::Code eError = m_pMarshal->dccListen(m_pDescriptor->szListenIp,m_pDescriptor->szListenPort,m_pDescriptor->bDoTimeout);
		if(eError != KviError::Success)
			handleMarshalError(eError);
	} else {
		// ACTIVE CONNECTION
		output(KVI_OUT_DCCMSG,__tr2qs_ctx("Attempting an active DCC VIDEO connection","dcc"));
		KviError::Code eError = m_pMarshal->dccConnect(m_pDescriptor->szIp.toUtf8().data(),m_pDescriptor->szPort.toUtf8().data(),m_pDescriptor->bDoTimeout);
		if(eError != KviError::Success)
			handleMarshalError(eError);
	}
}

void DccVideoWindow::connectionInProgress()
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
			KviCString port   = !m_pDescriptor->szFakePort.isEmpty() ? m_pDescriptor->szFakePort : m_pMarshal->localPort();
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
		} else output(KVI_OUT_DCCMSG,__tr2qs_ctx("DCC VIDEO request not sent: awaiting manual connection","dcc"));
	}
}

const QString & DccVideoWindow::target()
{
	// This may change on the fly...
	if(!m_pszTarget)
		m_pszTarget = new QString();

	m_pszTarget->sprintf("%s@%s:%s",m_pDescriptor->szNick.toUtf8().data(),m_pDescriptor->szIp.toUtf8().data(),m_pDescriptor->szPort.toUtf8().data());
	return *m_pszTarget;
}

void DccVideoWindow::getBaseLogFileName(QString &buffer)
{
	buffer.sprintf("dccvideo_%s_%s_%s",m_pDescriptor->szNick.toUtf8().data(),m_pDescriptor->szLocalFileName.toUtf8().data(),m_pDescriptor->szPort.toUtf8().data());
}

void DccVideoWindow::fillCaptionBuffers()
{
	KviCString tmp(KviCString::Format,"DCC Video %s@%s:%s %s",
		m_pDescriptor->szNick.toUtf8().data(),m_pDescriptor->szIp.toUtf8().data(),m_pDescriptor->szPort.toUtf8().data(),
		m_pDescriptor->szLocalFileName.toUtf8().data());

	m_szPlainTextCaption = tmp;
}

QPixmap * DccVideoWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KviIconManager::DccVoice);
}

void DccVideoWindow::ownMessage(const QString &text, bool bUserFeedback)
{
	if(!m_pSlaveThread)
	{
		output(KVI_OUT_SYSTEMWARNING,__tr2qs_ctx("Cannot send data: No active connection","dcc"));
		return;
	}

	QByteArray szData = encodeText(text);
	const char * d = szData.data();
	if(!d)return;

#ifdef COMPILE_CRYPT_SUPPORT
	if(cryptSessionInfo())
	{
		if(cryptSessionInfo()->m_bDoEncrypt)
		{
			if(*d != KviControlCodes::CryptEscape)
			{
				KviCString encrypted;
				cryptSessionInfo()->m_pEngine->setMaxEncryptLen(-1);
				switch(cryptSessionInfo()->m_pEngine->encrypt(d,encrypted))
				{
					case KviCryptEngine::Encrypted:
					{
						KviCString buf(KviCString::Format,"%s\r\n",encrypted.ptr());
						m_tmpTextDataOut.append(buf.ptr(), buf.len());
						if(bUserFeedback)
							m_pFrm->firstConsole()->outputPrivmsg(this,KVI_OUT_OWNPRIVMSGCRYPTED,
								m_pDescriptor->szLocalNick.toUtf8().data(),m_pDescriptor->szLocalUser.toUtf8().data(),
								m_pDescriptor->szLocalHost.toUtf8().data(),text,KviConsoleWindow::NoNotifications);
					}
					break;
					case KviCryptEngine::Encoded:
					{
						KviCString buf(KviCString::Format,"%s\r\n",encrypted.ptr());
						m_tmpTextDataOut.append(buf.ptr(), buf.len());
						if(bUserFeedback)
						{
							QString encr = decodeText(encrypted.ptr());
							m_pFrm->firstConsole()->outputPrivmsg(this,KVI_OUT_OWNPRIVMSG,
								m_pDescriptor->szLocalNick.toUtf8().data(),m_pDescriptor->szLocalUser.toUtf8().data(),
								m_pDescriptor->szLocalHost.toUtf8().data(),encr,KviConsoleWindow::NoNotifications);
						}
					}
					break;
					default: // also case KviCryptEngine::EncryptError
					{
						QString szErr = cryptSessionInfo()->m_pEngine->lastError();
						output(KVI_OUT_SYSTEMERROR,
							__tr2qs_ctx("The crypto engine was not able to encrypt the current message (%Q): %Q, no data was sent to the remote end","dcc"),
							&text,&szErr);
					}
					break;
				}
				return;
			} else {
				d++; //eat the escape code
				KviCString buf(KviCString::Format,"%s\r\n",d);
				QString tmp = text.right(text.length() - 1);
				m_tmpTextDataOut.append(buf.ptr(), buf.len());

				if(bUserFeedback)
					m_pFrm->firstConsole()->outputPrivmsg(this,KVI_OUT_OWNPRIVMSG,
						m_pDescriptor->szLocalNick.toUtf8().data(),m_pDescriptor->szLocalUser.toUtf8().data(),
						m_pDescriptor->szLocalHost.toUtf8().data(),tmp,KviConsoleWindow::NoNotifications);
				return;
			}
		}
	}
#endif
	KviCString buf(KviCString::Format,"%s\r\n",d);
	m_tmpTextDataOut.append(buf.ptr(), buf.len());

	if(bUserFeedback)
		m_pFrm->firstConsole()->outputPrivmsg(this,KVI_OUT_OWNPRIVMSG,
			m_pDescriptor->szLocalNick.toUtf8().data(),m_pDescriptor->szLocalUser.toUtf8().data(),
			m_pDescriptor->szLocalHost.toUtf8().data(),text,KviConsoleWindow::NoNotifications);
}

const QString & DccVideoWindow::localNick()
{
	// FIXME: This is just a complete HACK
	m_szLocalNick = m_pDescriptor->szLocalNick;
	return m_szLocalNick;
}

void DccVideoWindow::ownAction(const QString &text)
{
	if(m_pSlaveThread)
	{
		QString szTmpBuffer;
		//see bug ticket #220
		if(KVI_OPTION_BOOL(KviOption_boolStripMircColorsInUserMessages))
		{
			szTmpBuffer = KviControlCodes::stripControlBytes(text);
		} else {
			szTmpBuffer = text;
		}

		QByteArray szData = encodeText(szTmpBuffer);
		const char * d = szData.data();
		if(!d)return;
		KviCString buf(KviCString::Format,"%cACTION %s%c\r\n",0x01,d,0x01);
		m_tmpTextDataOut.append(buf.ptr(), buf.len());
		output(KVI_OUT_ACTION,"%Q %Q",&(m_pDescriptor->szLocalNick),&szTmpBuffer);
	} else {
		output(KVI_OUT_SYSTEMWARNING,__tr2qs_ctx("Cannot send data: No active connection","dcc"));
	}
}

bool DccVideoWindow::event(QEvent *e)
{
	if(e->type() == KVI_THREAD_EVENT)
	{
		switch(((KviThreadEvent *)e)->id())
		{
			case KVI_DCC_THREAD_EVENT_ERROR:
			{
				KviError::Code * pError = ((KviThreadDataEvent<KviError::Code> *)e)->getData();
				QString ssss = KviError::getDescription(*pError);
				output(KVI_OUT_DCCERROR,__tr2qs_ctx("ERROR: %Q","dcc"),&(ssss));
				delete pError;
				return true;
			}
			break;
			case KVI_DCC_THREAD_EVENT_DATA:
			{
				KviCString * encoded = ((KviThreadDataEvent<KviCString> *)e)->getData();
				KviCString d=KviCString(decodeText(encoded->ptr()));
				if(d.firstCharIs(0x01))
				{
					d.cutLeft(1);
					if(d.lastCharIs(0x01))
						d.cutRight(1);
					if(kvi_strEqualCIN("ACTION",d.ptr(),6))
						d.cutLeft(6);
					d.stripLeftWhiteSpace();
					output(KVI_OUT_ACTION,"%Q %s",&(m_pDescriptor->szNick),d.ptr());
					if(!hasAttention())
					{
						if(KVI_OPTION_BOOL(KviOption_boolFlashDccChatWindowOnNewMessages))
						{
							demandAttention();
						}
						if(KVI_OPTION_BOOL(KviOption_boolPopupNotifierOnNewDccChatMessages))
						{
							QString szMsg = "<b>";
							szMsg += m_pDescriptor->szNick;
							szMsg += "</b> ";
							szMsg += Qt::escape(QString(d.ptr()));
							//qDebug("KviIrcServerParser_ctcp.cpp:975 debug: %s",szMsg.data());
							g_pApp->notifierMessage(this,KVI_OPTION_MSGTYPE(KVI_OUT_ACTION).pixId(),szMsg,KVI_OPTION_UINT(KviOption_uintNotifierAutoHideTime));
						}
					}
				} else {

#ifdef COMPILE_CRYPT_SUPPORT
					if(KviCryptSessionInfo * cinf = cryptSessionInfo())
					{
						if(cinf->m_bDoDecrypt)
						{
							KviCString decryptedStuff;
							switch(cinf->m_pEngine->decrypt(d.ptr(),decryptedStuff))
							{
								case KviCryptEngine::DecryptOkWasEncrypted:
								case KviCryptEngine::DecryptOkWasEncoded:
								case KviCryptEngine::DecryptOkWasPlainText:
									if(!KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnDCCChatMessage,this,QString(decryptedStuff.ptr()),m_pDescriptor->idString()))
									{
										m_pFrm->firstConsole()->outputPrivmsg(this,KVI_OUT_DCCCHATMSG,
											m_pDescriptor->szNick.toUtf8().data(),m_pDescriptor->szUser.toUtf8().data(),
											m_pDescriptor->szHost.toUtf8().data(),decryptedStuff.ptr());
									}
									delete encoded;
									return true;
								break;

								default: // also case KviCryptEngine::DecryptError
								{
									QString szErr = cinf->m_pEngine->lastError();
									output(KVI_OUT_SYSTEMERROR,
										__tr2qs_ctx("The following message appears to be encrypted, but the crypto engine failed to decode it: %Q","dcc"),
										&szErr);
								}
								break;
							}
						}
					} else {
#endif
						// FIXME!
						if(!KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnDCCChatMessage,this,QString(d.ptr()),m_pDescriptor->idString()))
						{
							m_pFrm->firstConsole()->outputPrivmsg(this,KVI_OUT_DCCCHATMSG,
								m_pDescriptor->szNick.toUtf8().data(),m_pDescriptor->szUser.toUtf8().data(),
								m_pDescriptor->szHost.toUtf8().data(),d.ptr());
							if(!hasAttention())
							{
								if(KVI_OPTION_BOOL(KviOption_boolFlashDccChatWindowOnNewMessages))
								{
									demandAttention();
								}
								if(KVI_OPTION_BOOL(KviOption_boolPopupNotifierOnNewDccChatMessages))
								{
									QString szMsg = Qt::escape(QString(d.ptr()));
									g_pApp->notifierMessage(this,KviIconManager::DccChatMsg,szMsg,KVI_OPTION_UINT(KviOption_uintNotifierAutoHideTime));
								}
							}
						}
#ifdef COMPILE_CRYPT_SUPPORT
					}
#endif
				}
				delete encoded;
				return true;
			}

			case KVI_DCC_THREAD_EVENT_MESSAGE:
			{
				KviCString * str = ((KviThreadDataEvent<KviCString> *)e)->getData();
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
				qDebug("Invalid event type %d received",((KviThreadEvent *)e)->id());
			break;
		}

	}

	return KviWindow::event(e);
}

void DccVideoWindow::handleMarshalError(KviError::Code eError)
{
	QString ssss = KviError::getDescription(eError);
	output(KVI_OUT_DCCERROR,__tr2qs_ctx("DCC Failed: %Q","dcc"),&ssss);
}

void DccVideoWindow::connected()
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

	output(KVI_OUT_DCCMSG,__tr2qs_ctx("Actual codec used is '%s'","dcc"),opt->pCodec->name());

	m_pSlaveThread = new DccVideoThread(this,m_pMarshal->releaseSocket(),opt);

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
}

void DccVideoWindow::stopTalking()
{
	KviThreadDataEvent<int> * e = new KviThreadDataEvent<int>(KVI_DCC_THREAD_EVENT_ACTION);
	e->setData(new int(0));
	m_pSlaveThread->enqueueEvent(e);
}

void DccVideoWindow::startTalking()
{
	KviThreadDataEvent<int> * e = new KviThreadDataEvent<int>(KVI_DCC_THREAD_EVENT_ACTION);
	e->setData(new int(1));
	m_pSlaveThread->enqueueEvent(e);
}

void DccVideoWindow::startOrStopTalking(bool bStart)
{
	if(bStart)startTalking();
	else stopTalking();
}

void DccVideoWindow::slotUpdateImage()
{
#ifndef COMPILE_DISABLE_DCC_VIDEO
	if(m_pSlaveThread && isVisible())
	{
		m_pOutVideoLabel->setPixmap(QPixmap::fromImage(m_pSlaveThread->m_outImage));
		m_pInVideoLabel->setPixmap(QPixmap::fromImage(m_pSlaveThread->m_inImage));
	}
#endif
}

void DccVideoWindow::deviceRegistered(const QString &)
{

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
}


void DccVideoWindow::deviceUnregistered(const QString & )
{
#ifndef COMPILE_DISABLE_DCC_VIDEO
	g_pVideoDevicePool->fillDeviceQComboBox(m_pCDevices);
	g_pVideoDevicePool->fillInputQComboBox(m_pCInputs);
	g_pVideoDevicePool->fillStandardQComboBox(m_pCStandards);
#endif
}

void DccVideoWindow::videoInputChanged(int )
{
	if(m_pSlaveThread)
		m_pSlaveThread->restartRecording(m_pCDevices->currentIndex(), m_pCInputs->currentIndex(), m_pCStandards->currentIndex());
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_DccVideoWindow.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
