#ifndef _LIBKVISND_H_
#define _LIBKVISND_H_

//
//   File : libkvisnd.h
//   Creation date : Apr 21 2002 12:30:25 CEST by Juan Jos��varez
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

#include "kvi_settings.h"
#include "kvi_pointerlist.h"
#include "kvi_pointerhashtable.h"
#include "kvi_options.h"
#include "kvi_thread.h"
#include "kvi_string.h"

#include <QObject>

class KviSoundPlayer;

//class KviWavSoundFileReader
//{
//public:
//	KviWavSoundFileReader(QFile * f);
//	~KviWavSoundFileReader();
//protected:
//	QFile * m_pFile;
//public:
//	static bool recognize(KviFile * f);
//	bool readHeader();
//};


//bool KviWavSoundFileReader::recognize(KviFile * f)
//{
//	kvi_u32_t tag;
//	if(!f->load(tag))return false;
//
//	f->at(0);
//}

//bool KviWavSoundFileReader::readHeader()
//{
//
//
//    tag = get_le32(pb);
//
//    if (tag != MKTAG('R', 'I', 'F', 'F'))
//        return -1;
//    get_le32(pb); /* file size */
//    tag = get_le32(pb);
//    if (tag != MKTAG('W', 'A', 'V', 'E'))
//        return -1;
//
//    size = find_tag(pb, MKTAG('f', 'm', 't', ' '));
//    if (size < 0)
//        return -1;
//    id = get_le16(pb);
//    channels = get_le16(pb);
//    rate = get_le32(pb);
//    bit_rate = get_le32(pb) * 8;
//    get_le16(pb); /* block align */
//    get_le16(pb); /* bits per sample */
//    if (size >= 18) {
//        /* wav_extra_size */
//        extra_size = get_le16(pb);
//        /* skip unused data */
//        url_fseek(pb, size - 18, SEEK_CUR);
//    }
//
//    size = find_tag(pb, MKTAG('d', 'a', 't', 'a'));
//    if (size < 0)
//        return -1;
//
//    /* now we are ready: build format streams */
//    st = malloc(sizeof(AVStream));
//    if (!st)
//        return -1;
//    s->nb_streams = 1;
//    s->streams[0] = st;
//
//    st->id = 0;
//
//    st->codec.codec_type = CODEC_TYPE_AUDIO;
//    st->codec.codec_tag = id;
//    st->codec.codec_id = codec_get_id(codec_wav_tags, id);
//    st->codec.channels = channels;
//    st->codec.sample_rate = rate;
//}

class KviSoundThread : public KviThread
{
public:
	KviSoundThread(const QString &szFileName);
	virtual ~KviSoundThread();
protected:
	QString m_szFileName;
protected:
	virtual void play();
	virtual void run();
};

#ifdef COMPILE_PHONON_SUPPORT
class KviPhononSoundThread : public KviSoundThread
{
	public:
		KviPhononSoundThread(const QString &szFileName);
		virtual ~KviPhononSoundThread();
	protected:
		virtual void play();
};
#endif

#ifndef COMPILE_ON_WINDOWS
	#ifdef COMPILE_OSS_SUPPORT
		class KviOssSoundThread : public KviSoundThread
		{
		public:
			KviOssSoundThread(const QString &szFileName);
			virtual ~KviOssSoundThread();
		protected:
			virtual void play();
		};

		#ifdef COMPILE_AUDIOFILE_SUPPORT
			class KviOssAudiofileSoundThread : public KviSoundThread
			{
			public:
				KviOssAudiofileSoundThread(const QString &szFileName);
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
			KviEsdSoundThread(const QString &szFileName);
			virtual ~KviEsdSoundThread();
		protected:
			virtual void play();
		};
	#endif //COMPILE_ESD_SUPPORT

	#ifdef COMPILE_ARTS_SUPPORT
		class KviArtsSoundThread : public KviSoundThread
		{
		public:
			KviArtsSoundThread(const QString &szFileName);
			virtual ~KviArtsSoundThread();
		protected:
			virtual void play();
		};
	#endif //COMPILE_ARTS_SUPPORT
#endif //!COMPILE_ON_WINDOWS


typedef bool (KviSoundPlayer::*SoundSystemRoutine)(const QString &szFileName);

class KviSoundPlayer : public QObject
{
	friend class KviSoundThread;
	Q_OBJECT
public:
	KviSoundPlayer();
	virtual ~KviSoundPlayer();
public:
	bool play(const QString &szFileName);
	void detectSoundSystem();
	bool havePlayingSounds();
	//void getAvailableSoundSystems(KviPointerList<QString> * l);
	void getAvailableSoundSystems(QStringList * l);
	bool isMuted() {return KVI_OPTION_BOOL(KviOption_boolMuteAllSounds); };
	void setMuted(bool muted) {KVI_OPTION_BOOL(KviOption_boolMuteAllSounds)=muted;};
protected:
	KviPointerList<KviSoundThread> * m_pThreadList;
	KviPointerHashTable<QString,SoundSystemRoutine> * m_pSoundSystemDict;
protected:
	void registerSoundThread(KviSoundThread * t);
	void unregisterSoundThread(KviSoundThread * t);
	virtual bool event(QEvent *e);
protected:
#ifdef COMPILE_PHONON_SUPPORT
	bool playPhonon(const QString &szFileName);
#endif //!COMPILE_PHONON_SUPPORT
#ifdef COMPILE_ON_WINDOWS
	bool playWinmm(const QString &szFileName);
#else //!COMPILE_ON_WINDOWS
	#ifdef COMPILE_OSS_SUPPORT
		bool playOss(const QString &szFileName);
		#ifdef COMPILE_AUDIOFILE_SUPPORT
			bool playOssAudiofile(const QString &szFileName);
		#endif //COMPILE_AUDIOFILE_SUPPORT
	#endif //COMPILE_OSS_SUPPORT
	#ifdef COMPILE_ARTS_SUPPORT
		bool playArts(const QString &szFileName);
	#endif //COMPILE_ARTS_SUPPORT
	#ifdef COMPILE_ESD_SUPPORT
		bool playEsd(const QString &szFileName);
	#endif //COMPILE_ESD_SUPPORT
#endif //!COMPILE_ON_WINDOWS
	bool playQt(const QString &szFileName);
	bool playNull(const QString &szFileName);
};


#endif // _KVISND_H_
