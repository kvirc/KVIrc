//=============================================================================
//
//   File : libkvisnd.cpp
//   Creation date : Thu Dec 27 2002 17:13:12 GMT by Juanjo Alvarez
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002 Juanjo Alvarez (juanjux at yahoo dot es)
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "libkvisnd.h"

#include "KviModule.h"
#include "kvi_debug.h"
#include "KviFileUtils.h"
#include "KviMemory.h"
#include "KviWindow.h"
#include "kvi_out.h"
#include "KviLocale.h"
#include "KviQString.h"

#include <QSound>

#ifdef COMPILE_PHONON_SUPPORT
#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>
#include <phonon/path.h>
#include <QUrl>
#endif //!COMPILE_PHONON_SUPPORT

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#include <mmsystem.h>
#else //!COMPILE_ON_WINDOWS

#include <QFile>
#include <unistd.h>
#include <cerrno>

#ifdef COMPILE_ESD_SUPPORT
#include <esd.h>
#endif //COMPILE_ESD_SUPPORT

#ifdef COMPILE_OSS_SUPPORT
#include <fcntl.h>
#include <sys/ioctl.h>
#ifdef HAVE_LINUX_SOUNDCARD_H
#include <linux/soundcard.h>
#else
// Hint by Andy Fawcett: Thnx :)
#ifdef HAVE_SYS_SOUNDCARD_H
#include <sys/soundcard.h>
#else
#ifdef HAVE_MACHINE_SOUNDCARD_H
#include <machine/soundcard.h>
#else
#warning "Ops.. have no soundcard.h ? ... we're going to fail here :/"
#endif
#endif
#endif
#ifdef COMPILE_AUDIOFILE_SUPPORT
#include <audiofile.h>
#endif //COMPILE_AUDIOFILE_SUPPORT
#endif //COMPILE_OSS_SUPPORT
#endif

static KviSoundPlayer * g_pSoundPlayer = nullptr;

KviSoundPlayer::KviSoundPlayer()
    : QObject()
{
	m_pThreadList = new KviPointerList<KviSoundThread>;
	m_pThreadList->setAutoDelete(true);

#ifdef COMPILE_PHONON_SUPPORT
	m_pPhononPlayer = nullptr;
#endif //!COMPILE_PHONON_SUPPORT

	m_pLastUsedSoundPlayerEntry = nullptr;

	m_pSoundSystemDict = new KviPointerHashTable<QString, KviSoundPlayerEntry>(17, false);
	m_pSoundSystemDict->setAutoDelete(true);

#ifdef COMPILE_PHONON_SUPPORT
	m_pSoundSystemDict->insert("phonon", new KviSoundPlayerEntry(KVI_PTR2MEMBER(KviSoundPlayer::playPhonon), KVI_PTR2MEMBER(KviSoundPlayer::cleanupPhonon)));
#endif //!COMPILE_PHONON_SUPPORT

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	m_pSoundSystemDict->insert("winmm", new KviSoundPlayerEntry(KVI_PTR2MEMBER(KviSoundPlayer::playWinmm), KVI_PTR2MEMBER(KviSoundPlayer::cleanupWinmm)));
#else //!COMPILE_ON_WINDOWS

#ifdef COMPILE_OSS_SUPPORT
#ifdef COMPILE_AUDIOFILE_SUPPORT
	m_pSoundSystemDict->insert("oss+audiofile", new KviSoundPlayerEntry(KVI_PTR2MEMBER(KviSoundPlayer::playOssAudiofile), KVI_PTR2MEMBER(KviSoundPlayer::cleanupOssAudiofile)));
#endif //COMPILE_AUDIOFILE_SUPPORT
	m_pSoundSystemDict->insert("oss", new KviSoundPlayerEntry(KVI_PTR2MEMBER(KviSoundPlayer::playOss), KVI_PTR2MEMBER(KviSoundPlayer::cleanupOss)));
#endif //COMPILE_OSS_SUPPORT

#ifdef COMPILE_ESD_SUPPORT
	m_pSoundSystemDict->insert("esd", new KviSoundPlayerEntry(KVI_PTR2MEMBER(KviSoundPlayer::playEsd), KVI_PTR2MEMBER(KviSoundPlayer::cleanupEsd)));
#endif //COMPILE_ESD_SUPPORT

#endif //!COMPILE_ON_WINDOWS

	m_pSoundSystemDict->insert("qt", new KviSoundPlayerEntry(KVI_PTR2MEMBER(KviSoundPlayer::playQt), KVI_PTR2MEMBER(KviSoundPlayer::cleanupQt)));
	m_pSoundSystemDict->insert("null", new KviSoundPlayerEntry(KVI_PTR2MEMBER(KviSoundPlayer::playNull), KVI_PTR2MEMBER(KviSoundPlayer::cleanupNull)));
}

KviSoundPlayer::~KviSoundPlayer()
{
	if(m_pLastUsedSoundPlayerEntry)
		cleanupAfterLastPlayerEntry();

	stopAllSoundThreads();
	delete m_pThreadList;

	KviThreadManager::killPendingEvents(this);

	delete m_pSoundSystemDict;

#ifdef COMPILE_PHONON_SUPPORT
	if(m_pPhononPlayer)
		delete m_pPhononPlayer;
#endif //COMPILE_PHONON_SUPPORT

	g_pSoundPlayer = nullptr;
}

void KviSoundPlayer::stopAllSoundThreads()
{
	// kill any running sound thread
	m_pThreadList->setAutoDelete(false);

	while(KviSoundThread * t = m_pThreadList->first())
		delete t;

	m_pThreadList->setAutoDelete(true);
}

void KviSoundPlayer::cleanupAfterLastPlayerEntry()
{
	stopAllSoundThreads();
	if(!m_pLastUsedSoundPlayerEntry)
		return;
	SoundSystemCleanupRoutine r = m_pLastUsedSoundPlayerEntry->cleanupRoutine();
	if(r)
		(this->*r)();
	m_pLastUsedSoundPlayerEntry = nullptr;
}

void KviSoundPlayer::getAvailableSoundSystems(QStringList * l)
{
	KviPointerHashTableIterator<QString, KviSoundPlayerEntry> it(*m_pSoundSystemDict);
	while(it.current())
	{
		l->append(it.currentKey());
		++it;
	}
}

bool KviSoundPlayer::havePlayingSounds()
{
	if(m_pThreadList)
		if(m_pThreadList->count() > 0)
			return true;
#ifdef COMPILE_PHONON_SUPPORT
	if(m_pPhononPlayer)
		if(m_pPhononPlayer->state() == Phonon::PlayingState)
			return true;
#endif //COMPILE_PHONON_SUPPORT
	return false;
}

void KviSoundPlayer::registerSoundThread(KviSoundThread * t)
{
	m_pThreadList->append(t);
}

void KviSoundPlayer::unregisterSoundThread(KviSoundThread * t)
{
	m_pThreadList->removeRef(t);
}

bool KviSoundPlayer::event(QEvent * e)
{
	if(e->type() == KVI_THREAD_EVENT)
	{
		KviThread * t = ((KviThreadEvent *)e)->sender();
		if(!t)
			return true; // huh ?
		delete(KviSoundThread *)t;
		return true;
	}
	return QObject::event(e);
}

void KviSoundPlayer::detectSoundSystem()
{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	KVI_OPTION_STRING(KviOption_stringSoundSystem) = "winmm";
#else
#ifdef COMPILE_ESD_SUPPORT
	esd_format_t format = ESD_BITS16 | ESD_STREAM | ESD_PLAY | ESD_MONO;
	int esd_fd = esd_play_stream(format, 8012, nullptr, "kvirc");
	if(esd_fd >= 0)
	{
		KVI_OPTION_STRING(KviOption_stringSoundSystem) = "esd";
		return;
	}
#endif
#ifdef COMPILE_OSS_SUPPORT
#ifdef COMPILE_AUDIOFILE_SUPPORT
	KVI_OPTION_STRING(KviOption_stringSoundSystem) = "oss+audiofile";
	return;
#endif
	KVI_OPTION_STRING(KviOption_stringSoundSystem) = "oss";
#endif

	KVI_OPTION_STRING(KviOption_stringSoundSystem) = "qt";
	return;
#endif
}

#ifdef COMPILE_PHONON_SUPPORT
bool KviSoundPlayer::playPhonon(const QString & szFileName)
{
	if(isMuted())
		return true;

	Phonon::MediaSource media(QUrl::fromLocalFile(szFileName));

	if(!m_pPhononPlayer)
		m_pPhononPlayer = Phonon::createPlayer(Phonon::MusicCategory, media);
	else
		m_pPhononPlayer->setCurrentSource(media);

	m_pPhononPlayer->setTransitionTime(2000); // this fixes playback with the (very buggy) xine backend

	if(m_pPhononPlayer->state() == Phonon::ErrorState)
	{
		QString szError = m_pPhononPlayer->errorString();
		if(szError.isEmpty())
			qDebug("Phonon player in error state: can't play media '%s'", szFileName.isEmpty() ? "" : szFileName.toUtf8().data());
		else
			qDebug("Phonon player in error state: %s (can't play media '%s')", szError.toUtf8().data(), szFileName.isEmpty() ? "" : szFileName.toUtf8().data());

		return false;
	}

	m_pPhononPlayer->play();

	return true;
}

void KviSoundPlayer::cleanupPhonon()
{
	if(!m_pPhononPlayer)
		return;
	delete m_pPhononPlayer; // must be stopped
	m_pPhononPlayer = nullptr;
}
#endif //!COMPILE_PHONON_SUPPORT

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
bool KviSoundPlayer::playWinmm(const QString & szFileName)
{
	sndPlaySound(szFileName.toStdWString().c_str(), SND_ASYNC | SND_NODEFAULT);
	return true;
}

void KviSoundPlayer::cleanupWinmm()
{
	// FIXME: how to stop playing on windows ?
}
#else //!COMPILE_ON_WINDOWS
#ifdef COMPILE_OSS_SUPPORT
#ifdef COMPILE_AUDIOFILE_SUPPORT
bool KviSoundPlayer::playOssAudiofile(const QString & szFileName)
{
	if(isMuted())
		return true;
	KviOssAudiofileSoundThread * t = new KviOssAudiofileSoundThread(szFileName);
	if(!t->start())
	{
		delete t;
		return false;
	}
	return true;
}

void KviSoundPlayer::cleanupOssAudiofile()
{
}
#endif //COMPILE_AUDIOFILE_SUPPORT
bool KviSoundPlayer::playOss(const QString & szFileName)
{
	if(isMuted())
		return true;
	KviOssSoundThread * t = new KviOssSoundThread(szFileName);
	if(!t->start())
	{
		delete t;
		return false;
	}
	return true;
}

void KviSoundPlayer::cleanupOss()
{
}
#endif //COMPILE_OSS_SUPPORT
#ifdef COMPILE_ESD_SUPPORT
bool KviSoundPlayer::playEsd(const QString & szFileName)
{
	if(isMuted())
		return true;
	KviEsdSoundThread * t = new KviEsdSoundThread(szFileName);
	if(!t->start())
	{
		delete t;
		return false;
	}
	return true;
}

void KviSoundPlayer::cleanupEsd()
{
}
#endif //COMPILE_ESD_SUPPORT
#endif //!COMPILE_ON_WINDOWS

bool KviSoundPlayer::playQt(const QString & szFileName)
{
	if(isMuted())
		return true;
	QSound::play(szFileName);
	return true;
}

void KviSoundPlayer::cleanupQt()
{
	// how to stop Qt sounds ?
	// using the play/stop slots instead of the static ::play (TODO)
}

bool KviSoundPlayer::playNull(const QString &)
{
	// null sound system
	return true;
}

void KviSoundPlayer::cleanupNull()
{
}

bool KviSoundPlayer::play(const QString & szFileName)
{
	if(isMuted())
		return true;

	KviSoundPlayerEntry * e = m_pSoundSystemDict->find(KVI_OPTION_STRING(KviOption_stringSoundSystem));

	if(!e)
	{
		if(
		    (!KVI_OPTION_STRING(KviOption_stringSoundSystem).isEmpty()) && (!KviQString::equalCI(KVI_OPTION_STRING(KviOption_stringSoundSystem), "unknown")))
		{
			qDebug(
			    "Sound system '%s' is not valid, you may want to re-configure it in the options dialog...",
			    KVI_OPTION_STRING(KviOption_stringSoundSystem).toUtf8().data());
			return false; // detection already attempted (and failed?)
		}

		detectSoundSystem();
		e = m_pSoundSystemDict->find(KVI_OPTION_STRING(KviOption_stringSoundSystem));
		if(!e)
			return false;
	}

	if(e != m_pLastUsedSoundPlayerEntry)
	{
		// if the sound player changed, cleanup after the previous one
		if(m_pLastUsedSoundPlayerEntry)
			cleanupAfterLastPlayerEntry();
		m_pLastUsedSoundPlayerEntry = e;
	}

	SoundSystemPlayRoutine r = e->playRoutine();
	Q_ASSERT(r); // MUST BE THERE

	return (this->*r)(szFileName);
}

KviSoundThread::KviSoundThread(const QString & szFileName)
    : KviThread()
{
	g_pSoundPlayer->registerSoundThread(this);
	m_szFileName = szFileName;
	m_bTerminate = false;
}

KviSoundThread::~KviSoundThread()
{
	m_bTerminate = true; // force a termination request (since the base class will wait())
	g_pSoundPlayer->unregisterSoundThread(this);
}

void KviSoundThread::terminate()
{
	m_bTerminate = true;
}

void KviSoundThread::play()
{
}

void KviSoundThread::run()
{
	play();
#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
	postEvent(g_pSoundPlayer, new KviThreadEvent(KVI_THREAD_EVENT_SUCCESS));
#endif
}

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
#ifdef COMPILE_OSS_SUPPORT
#ifdef COMPILE_AUDIOFILE_SUPPORT
KviOssAudiofileSoundThread::KviOssAudiofileSoundThread(const QString & szFileName)
    : KviSoundThread(szFileName)
{
}

KviOssAudiofileSoundThread::~KviOssAudiofileSoundThread()
    = default;

void KviOssAudiofileSoundThread::play()
{
#define BUFFER_FRAMES 4096

	AFfilehandle file;
	AFframecount framesRead;
	int sampleFormat, sampleWidth, channelCount, format, freq;
	float frameSize;
	void * buffer;

	file = afOpenFile(m_szFileName.toUtf8().data(), "r", nullptr);
	if(!file)
	{
		qDebug("libaudiofile could not open the file %s", m_szFileName.toUtf8().data());
		qDebug("giving up playing sound...");
		return; // screwed up
	}

	sampleFormat = -1;

	afGetVirtualSampleFormat(file, AF_DEFAULT_TRACK, &sampleFormat, &sampleWidth);

	if(sampleFormat == -1)
	{
		qDebug("libaudiofile couldn't find the sample format for file %s", m_szFileName.toUtf8().data());
		qDebug("giving up playing sound...");
		afCloseFile(file);
		return; // screwed up
	}

	frameSize = afGetVirtualFrameSize(file, AF_DEFAULT_TRACK, 1);
	channelCount = afGetVirtualChannels(file, AF_DEFAULT_TRACK);
	buffer = KviMemory::allocate(int(BUFFER_FRAMES * frameSize));

	int audiofd_c = open("/dev/dsp", O_WRONLY /*| O_EXCL | O_NDELAY*/);

	QFile audiofd;

	if(audiofd_c < 0)
	{
		qDebug("Could not open audio device /dev/dsp! [OSS+AUDIOFILE]");
		qDebug("(the device is probably busy, errno = %d)", errno);
		goto exit_thread;
	}

	audiofd.open(audiofd_c, QIODevice::WriteOnly); // ???? what the heck is this for ?

	if(sampleWidth == 8)
		format = AFMT_U8;
	else if(sampleWidth == 16)
		format = AFMT_S16_NE;

	if(ioctl(audiofd.handle(), SNDCTL_DSP_SETFMT, &format) == -1)
	{
		qDebug("Could not set format width to DSP! [OSS]");
		goto exit_thread;
	}

	if(ioctl(audiofd.handle(), SNDCTL_DSP_CHANNELS, &channelCount) == -1)
	{
		qDebug("Could not set DSP channels! [OSS]");
		goto exit_thread;
	}

	freq = (int)afGetRate(file, AF_DEFAULT_TRACK);
	if(ioctl(audiofd.handle(), SNDCTL_DSP_SPEED, &freq) == -1)
	{
		qDebug("Could not set DSP speed %d! [OSS]", freq);
		goto exit_thread;
	}

	framesRead = afReadFrames(file, AF_DEFAULT_TRACK, buffer, BUFFER_FRAMES);

	while(!m_bTerminate && (framesRead > 0))
	{
		audiofd.write((char *)buffer, (int)(framesRead * frameSize));
		framesRead = afReadFrames(file, AF_DEFAULT_TRACK, buffer, BUFFER_FRAMES);
	}

exit_thread:
	audiofd.close();
	if(audiofd_c >= 0)
		close(audiofd_c);
	afCloseFile(file);
	KviMemory::free(buffer);
}
#endif //COMPILE_AUDIOFILE_SUPPORT

KviOssSoundThread::KviOssSoundThread(const QString & szFileName)
    : KviSoundThread(szFileName)
{
}

KviOssSoundThread::~KviOssSoundThread()
    = default;

void KviOssSoundThread::play()
{
#define OSS_BUFFER_SIZE 16384

	if(!m_szFileName.endsWith(QString(".au")))
	{
		qDebug("Oss only player supports only *.au files");
		return;
	}

	QFile f(m_szFileName);
	int fd = -1;
	char buf[OSS_BUFFER_SIZE];
	int iDataLen = 0;

	if(!f.open(QIODevice::ReadOnly))
	{
		qDebug("Could not open sound file %s! [OSS]", m_szFileName.toUtf8().data());
		return;
	}

	int iSize = f.size();

	if(iSize < 24)
	{
		qDebug("Could not play sound, file %s too small! [OSS]", m_szFileName.toUtf8().data());
		goto exit_thread;
	}

	if(f.read(buf, 24) < 24)
	{
		qDebug("Error while reading the sound file header (%s)! [OSS]", m_szFileName.toUtf8().data());
		goto exit_thread;
	}

	iSize -= 24;

	fd = open("/dev/audio", O_WRONLY /* | O_EXCL | O_NDELAY*/);
	if(fd < 0)
	{
		qDebug("Could not open device file /dev/audio!");
		qDebug("Maybe other program is using the device? Hint: fuser -uv /dev/audio");
		goto exit_thread;
	}

	while(!m_bTerminate && (iSize > 0))
	{
		int iCanRead = OSS_BUFFER_SIZE - iDataLen;
		if(iCanRead > 0)
		{
			int iToRead = iSize > iCanRead ? iCanRead : iSize;
			int iReaded = f.read(buf + iDataLen, iToRead);
			if(iReaded < 1)
			{
				qDebug("Error while reading the file data (%s)! [OSS]", m_szFileName.toUtf8().data());
				goto exit_thread;
			}
			iSize -= iReaded;
			iDataLen += iReaded;
		}
		if(iDataLen > 0)
		{
			int iWritten = write(fd, buf, iDataLen);
			if(iWritten < 0)
			{
				if((errno != EINTR) && (errno != EAGAIN))
				{
					qDebug("Error while writing the audio data (%s)! [OSS]", m_szFileName.toUtf8().data());
					goto exit_thread;
				}
			}
			iDataLen -= iWritten;
		}
		else
		{
			// nothing to write ????
			goto exit_thread;
		}
	}

exit_thread:
	f.close();
	if(fd >= 0)
		close(fd);
}

#endif //COMPILE_OSS_SUPPORT

#ifdef COMPILE_ESD_SUPPORT

KviEsdSoundThread::KviEsdSoundThread(const QString & szFileName)
    : KviSoundThread(szFileName)
{
}

KviEsdSoundThread::~KviEsdSoundThread()
{
}

void KviEsdSoundThread::play()
{
	// ESD has a really nice API
	if(!esd_play_file(nullptr, m_szFileName.toUtf8().data(), 1)) // this is sync.. FIXME: it can't be stopped!
		qDebug("Could not play sound %s! [ESD]", m_szFileName.toUtf8().data());
}

#endif //COMPILE_ESD_SUPPORT
#endif //!COMPILE_ON_WINDOWS

/*
	@doc: snd.play
	@type:
		command
	@title:
		snd.play
	@short:
		Play a sound file from the disk
	@syntax:
		snd.play [-q|quiet] <filename:string>
	@description:
		Play a file, using the sound system specified by the user in the options.[br]
		The supported file formats vary from one sound system to another, but the best
		bet could be Au Law (.au) files. Phonon, EsounD and Linux/OSS with audiofile support also
		support other formats like .wav files but in OSS without audiofile only .au files are
		supported.
		On windows the supported file formats are determined by the drivers installed.
		You should be able to play at least *.wav files.[br]
		(This is a task where the Windows interface is really well done, I must say that :)
	@switches:
		!sw: -q | --quiet
		Causes the command to run quietly (no complains about missing files, invalid formats...
*/

static bool snd_kvs_cmd_play(KviKvsModuleCommandCall * c)
{
	QString szFile;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("file name", KVS_PT_STRING, 0, szFile)
	KVSM_PARAMETERS_END(c)
	if(szFile.isEmpty() || (!KviFileUtils::fileExists(szFile)))
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs("Sound file '%Q' not found"), &szFile);
		return true;
	}

	if(!g_pSoundPlayer->play(szFile))
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs("Unable to play sound '%Q'"), &szFile);
	}

	return true;
}

static bool snd_kvs_cmd_autodetect(KviKvsModuleCommandCall * c)
{
	g_pSoundPlayer->detectSoundSystem();
	if(KviQString::equalCI(KVI_OPTION_STRING(KviOption_stringSoundSystem), "null"))
	{
		c->window()->outputNoFmt(KVI_OUT_SYSTEMERROR, __tr2qs("Sorry, I can't find a sound system to use on this machine"));
	}
	else
	{
		c->window()->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Sound system detected to: %s"), KVI_OPTION_STRING(KviOption_stringSoundSystem).toUtf8().data());
	}
	return true;
}

/*
	@doc: snd.mute
	@type:
		command
	@title:
		snd.mute
	@short:
		Mute all sounds
	@syntax:
		snd.mute
	@description:
		Mute all sounds
*/

static bool snd_kvs_cmd_mute(KviKvsModuleCommandCall * c)
{
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETERS_END(c)
	g_pSoundPlayer->setMuted(true);
	return true;
}

/*
	@doc: snd.unmute
	@type:
		command
	@title:
		snd.unmute
	@short:
		UnMute all sounds
	@syntax:
		snd.mute
	@description:
		UnMute all sounds
*/

static bool snd_kvs_cmd_unmute(KviKvsModuleCommandCall * c)
{
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETERS_END(c)
	g_pSoundPlayer->setMuted(false);
	return true;
}

/*
	@doc: snd.isMuted
	@type:
		function
	@title:
		$snd.isMuted
	@short:
		Returns if the sounds muted
	@syntax:
		<bool> $snd.isMuted()
	@description:
		Returns if the sounds muted
*/

static bool snd_kvs_fnc_ismuted(KviKvsModuleFunctionCall * c)
{
	c->returnValue()->setBoolean(g_pSoundPlayer->isMuted());
	return true;
}

static bool snd_module_init(KviModule * m)
{
	g_pSoundPlayer = new KviSoundPlayer();

	KVSM_REGISTER_SIMPLE_COMMAND(m, "autodetect", snd_kvs_cmd_autodetect);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "play", snd_kvs_cmd_play);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "mute", snd_kvs_cmd_mute);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "unmute", snd_kvs_cmd_unmute);

	KVSM_REGISTER_FUNCTION(m, "isMuted", snd_kvs_fnc_ismuted);

	return true;
}

static bool snd_module_cleanup(KviModule *)
{
	delete g_pSoundPlayer;
	g_pSoundPlayer = nullptr;
	return true;
}

static bool snd_module_can_unload(KviModule *)
{
	return !(g_pSoundPlayer->havePlayingSounds());
}

static bool snd_module_ctrl(KviModule *, const char * operation, void * param)
{
	if(kvi_strEqualCI(operation, "getAvailableSoundSystems"))
	{
		// we expect param to be a pointer to QStringList
		QStringList * l = (QStringList *)param;
		g_pSoundPlayer->getAvailableSoundSystems(l);
		return true;
	}
	if(kvi_strEqualCI(operation, "detectSoundSystem"))
	{
		g_pSoundPlayer->detectSoundSystem();
		return true;
	}
	if(kvi_strEqualCI(operation, "play"))
	{
		QString * pszFileName = (QString *)param;
		if(pszFileName)
			return g_pSoundPlayer->play(*pszFileName);
		return false;
	}
	return false;
}

KVIRC_MODULE(
    "Sound", // module name
    "4.0.0", // module version
    "(C) 2002 Szymon Stefanek (pragma at kvirc dot net),"
    "Juanjo Alvarez (juanjux at yahoo dot es)", // author & (C)
    "Sound playing commands",
    snd_module_init,
    snd_module_can_unload,
    snd_module_ctrl,
    snd_module_cleanup,
    0)
