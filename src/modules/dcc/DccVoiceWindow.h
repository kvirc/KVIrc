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

#if 0

/*

//
// CODEC DEFINITION
//

	Sample rate = samples/sec (ex. 8000)
	Sample size = bits (ex. 16 bits)
	Sample endianness = le/be

	Sample compressor = name

	<rate>:<bits>:<endianness>:<compressor>

	8000:16:le:null
	8000:16:le:gsm
	8000:16:le:adpcm

class KviVoiceParty
{
public:
	KviVoiceParty(const QString &szNick,const QString &szIp,unsigned short uPort);
	~KviVoiceParty();
protected:
	QString m_szIp;
	unsigned short m_uPort;
	QString m_szNick;
	KviPointerList<KviVoiceParty> * m_pChildrenTree;
public:
	const QString & ip(){ return m_szIp; };
	unsigned short port(){ return m_uPort; };
	const QString & nick(){ return m_szNick; };
	void addChild(KviVoiceParty * pChild);
};


KviVoiceParty::KviVoiceParty(const QString &szNick,const QString &szIp,unsigned short uPort)
: m_szIp(szIp), m_uPort(uPort), m_szNick(szNick)
{
	m_pChildrenTree = nullptr;
}

KviVoiceParty::~KviVoiceParty()
{
	if(m_pChildrenTree)delete m_pChildrenTree;
}

void KviVoiceParty::addChild(KviVoiceParty * pChild)
{
	if(!m_pChildrenTree)
	{
		m_pChildrenTree = new KviPointerList<KviVoiceParty>;
		m_pChildrenTree->setAutoDelete(true);
	}
	m_pChildrenTree->append(pChild);
}

class KviVoiceAudioEncoder
{
public:
	KviVoiceAudioEncoder();
	~KviVoiceAudioEncoder();
public:

};

class KviVoiceAudioDecoder
{
public:
	KviVoiceAudioDecoder();
	~KviVoiceAudioDecoder();
};

class KviVoiceLink
{
public:
	KviVoiceLink(KviVoiceParty * pRemoteParty);
	~KviVoiceLink();
protected:
	QString m_szId;
	KviVoiceParty        * m_pRemoteParty;
	KviVoiceAudioEncoder * m_pAudioEncoder;
	KviVoiceAudioDecoder * m_pAudioDecoder;
public:
	const QStirng & id(){ return m_szId; };
	KviVoiceParty * remoteParty(){ return m_pRemoteParty; };
};

KviVoiceLink::KviVoiceLink(KviVoiceParty * pRemoteParty)
{
	KviQString::sprintf("%Q:%u",&(pRemoteParty->nick()),pRemoteParty->port());
	m_pRemoteParty = pRemoteParty;
	m_pAudioEncoder = nullptr;
	m_pAudioDecoder = nullptr;
}

KviVoiceLink::~KviVoiceLink()
{
	delete m_pRemoteParty;
	if(m_pAudioEncoder)delete m_pAudioEncoder;
	if(m_pAudioDecoder)delete m_pAudioDecoder;
}

class KviVoice_r8000s16eL_to_r8000s16eB_Transformer
{

}

class KviVoice_r8000s16eB_to_r8000s16eL_Transformer
{

}

class KviVoice_r11025s16eL_to_r11025s16eB_Transformer
{

}

class KviVoice_r11025s16eB_to_r11025s16eL_Transformer
{

}

class KviVoice_r11025s16eL_to_r8000s16eL_Transformer
{

}

class KviVoice_r11025s16eL_to_r8000s16eB_Transformer
{

}

class KviVoice_r11025s16eB_to_r8000s16eL_Transformer
{

}

class KviVoice_r8000s16eL_to_r11025s16eL_Transformer
{

}

class KviVoice_r8000s16eL_to_r11025s16eB_Transformer
{

}

class KviVoice_r8000s16eB_to_r11025s16eL_Transformer
{

}



class KviVoiceConference
{
public:
	KviVoiceConference();
	~KviVoiceConference();
public:
	KviPointerList<KviVoiceLink> * m_pLinks;
	KviPointerHashTable<QString,KviVoiceLink> *
	SOCKET m_hUdpSocket;
	QString m_szLastError;

	unsigned int m_uLocalAudioSampleRate; // samples/sec
	unsigned int m_uLocalAudioSampleSize; // bits
	unsigned int m_uLocalAudioEndianness; // 0 = le, 1 = be

public:
	void conferenceThread();
protected:
	void conferenceThreadMain();
	bool setupUdpSocket();
};

KviVoiceConference::KviVoiceConference()
{
	m_pLinks = new KviPointerList<KviVoiceLink>;
	m_pLinks->setAutoDelete(true);
}

KviVoiceConference::~KviVoiceConference()
{
	delete m_pLinks;
}

bool KviVoiceConference::setupUdpSocket()
{
	return true;
}

void KviVoiceConference::shutdownUdpSocket()
{
}

void KviVoiceConference::conferenceThreadMain()
{
	for(;;)
	{
		readAndDecompressIncomingDataForEveryLink();

		mixIncomingDataToASingleStream();
		playIncomingDataSingleStream();

		readLocalAudioStream();

		foreach(link)
		{
			mixLocalAndOtherIncomingDataStreams()
			compressAndSendOtherIncomingDataStreams()
		}

	}
}

void KviVoiceConference::conferenceThread()
{
	if(!setupUdpSocket())return;

	conferenceThreadMain();

	shutdownUdpSocket();
}


*/

//
// DCC VOICE NG proto
//

//
// TCP control connection
//     --> HELLO: DccVoice protocol header
//     <-- HELLO: DccVoice protocol header
//     --> IACCEPT: Codec-description|CodecId,Codec-description|CodecID,Codec... (in order of preference)
//     <-- IACCEPT: Codec-description|CodecID,Codec-description|CodecId,Codec... (in order of preference)
//     --> MYADDRESS
//     <-- MYADDRESS
//     --> YOURIDIS: <local id for the remote end> (CID)
//     <-- YOURIDIS: <local id for the remote end> (CID)

// Audio is sent in blocks broken in chunks broken in udp packets
// Each block is a set of consecutive audio chunks that theoretically
// should be played consecutively.
// Each packet in a chunk has an ordinal
// Chunks must be relatively short in order
//   to allow a remote end that looses a packet to
//   synchronize after a short period of time
//   The maximum number of packets in a chunk is 65535 (but a chunk should be no more than 24-32 KBytes in size
//   and in general they should be as small as possible, even one packet per chunk, if the codec allows it)
//   Each chunk should be encoded independently of the others
//   We can switch codec at each chunk (but not at each packet)
// When some packets are lost we loose the entire chunk
// A block is completely synchronized in time (unless we loose some chunks: in that case
// we may decide to synchronize with silence or insert a glitch...)
// Decoding never depends on the future

// Each packet should be decompressable (eventually dependently on the previous in the chunk)
// but playable independently

// start UDP stream

// UDP Packet format:

// <magic byte>: byte
// <magic byte>: byte
// <local id>: word
// <payload len>: word
// <payload>


// Payload format:

// <codec id>: word
// <ordinal in a chunk>: word (0 = beginning of a chunk)

// read raw audio data at sample rate X, sample size Y
// multiplex data always at this sample rate and sample size

#endif

#endif //_VOICE_H_
