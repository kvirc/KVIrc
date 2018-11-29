#ifndef _LIBKVISND_H_
#define _LIBKVISND_H_
//=============================================================================
//
//   File : libkvisnd.h
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

#include "kvi_settings.h"
#include "KviPointerList.h"
#include "KviPointerHashTable.h"
#include "KviOptions.h"
#include "KviThread.h"
#include "KviCString.h"

#include <QObject>

class KviSoundPlayer;

class KviSoundThread : public KviThread
{
public:
	KviSoundThread(const QString & szFileName);
	virtual ~KviSoundThread();

protected:
	bool m_bTerminate;
	QString m_szFileName;

public:
	void terminate();

protected:
	virtual void play();
	virtual void run();
};

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
#ifdef COMPILE_OSS_SUPPORT
class KviOssSoundThread : public KviSoundThread
{
public:
	KviOssSoundThread(const QString & szFileName);
	virtual ~KviOssSoundThread();

protected:
	virtual void play();
};

#ifdef COMPILE_AUDIOFILE_SUPPORT
class KviOssAudiofileSoundThread : public KviSoundThread
{
public:
	KviOssAudiofileSoundThread(const QString & szFileName);
	virtual ~KviOssAudiofileSoundThread();

protected:
	virtual void play();
};
#endif //COMPILE_AUDIOFILE_SUPPORT
#endif //COMPILE_OSS_SUPPORT

#ifdef COMPILE_ESD_SUPPORT
class KviEsdSoundThread : public KviSoundThread
{
public:
	KviEsdSoundThread(const QString & szFileName);
	virtual ~KviEsdSoundThread();

protected:
	virtual void play();
};
#endif //COMPILE_ESD_SUPPORT

#ifdef COMPILE_ARTS_SUPPORT
class KviArtsSoundThread : public KviSoundThread
{
public:
	KviArtsSoundThread(const QString & szFileName);
	virtual ~KviArtsSoundThread();

protected:
	virtual void play();
};
#endif //COMPILE_ARTS_SUPPORT
#endif //!COMPILE_ON_WINDOWS

#ifdef COMPILE_PHONON_SUPPORT
namespace Phonon
{
	class MediaObject;
}
#endif //!COMPILE_PHONON_SUPPORT

typedef bool (KviSoundPlayer::*SoundSystemPlayRoutine)(const QString & szFileName);
typedef void (KviSoundPlayer::*SoundSystemCleanupRoutine)();

class KviSoundPlayerEntry
{
private:
	SoundSystemPlayRoutine m_pPlayRoutine;
	SoundSystemCleanupRoutine m_pCleanupRoutine;

public:
	KviSoundPlayerEntry(SoundSystemPlayRoutine pPlayRoutine, SoundSystemCleanupRoutine pCleanupRoutine)
	    : m_pPlayRoutine(pPlayRoutine), m_pCleanupRoutine(pCleanupRoutine)
	{
	}

	SoundSystemPlayRoutine playRoutine() const
	{
		return m_pPlayRoutine;
	}

	SoundSystemCleanupRoutine cleanupRoutine() const
	{
		return m_pCleanupRoutine;
	}
};

class KviSoundPlayer : public QObject
{
	friend class KviSoundThread;
	Q_OBJECT
public:
	KviSoundPlayer();
	virtual ~KviSoundPlayer();

public:
	bool play(const QString & szFileName);
	void detectSoundSystem();
	bool havePlayingSounds();
	void getAvailableSoundSystems(QStringList * l);
	bool isMuted()
	{
		return KVI_OPTION_BOOL(KviOption_boolMuteAllSounds);
	}
	void setMuted(bool muted)
	{
		KVI_OPTION_BOOL(KviOption_boolMuteAllSounds) = muted;
	}

protected:
	KviPointerList<KviSoundThread> * m_pThreadList;
	KviPointerHashTable<QString, KviSoundPlayerEntry> * m_pSoundSystemDict;
#ifdef COMPILE_PHONON_SUPPORT
	Phonon::MediaObject * m_pPhononPlayer;
#endif //!COMPILE_PHONON_SUPPORT
	KviSoundPlayerEntry * m_pLastUsedSoundPlayerEntry;

protected:
	void registerSoundThread(KviSoundThread * t);
	void unregisterSoundThread(KviSoundThread * t);
	bool event(QEvent * e) override;

protected:
	void stopAllSoundThreads();
	void cleanupAfterLastPlayerEntry();
#ifdef COMPILE_PHONON_SUPPORT
	bool playPhonon(const QString & szFileName);
	void cleanupPhonon();
#endif //!COMPILE_PHONON_SUPPORT
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	bool playWinmm(const QString & szFileName);
	void cleanupWinmm();
#else //!COMPILE_ON_WINDOWS
#ifdef COMPILE_OSS_SUPPORT
	bool playOss(const QString & szFileName);
	void cleanupOss();
#ifdef COMPILE_AUDIOFILE_SUPPORT
	bool playOssAudiofile(const QString & szFileName);
	void cleanupOssAudiofile();
#endif //COMPILE_AUDIOFILE_SUPPORT
#endif //COMPILE_OSS_SUPPORT
#ifdef COMPILE_ARTS_SUPPORT
	bool playArts(const QString & szFileName);
	void cleanupArts();
#endif //COMPILE_ARTS_SUPPORT
#ifdef COMPILE_ESD_SUPPORT
	bool playEsd(const QString & szFileName);
	void cleanupEsd();
#endif //COMPILE_ESD_SUPPORT
#endif //!COMPILE_ON_WINDOWS
	bool playQt(const QString & szFileName);
	void cleanupQt();
	bool playNull(const QString & szFileName);
	void cleanupNull();
};

#endif // _KVISND_H_
