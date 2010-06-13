#ifndef _MP_AUDACIOUSINTERFACE_H_
#define _MP_AUDACIOUSINTERFACE_H_
//=============================================================================
//
//   File : mp_mprisinterface.h
//   Creation date : Thu 06 Dec 2007 14:20:02 by Tomasz Moń
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2007-2009 Tomasz Moń <desowin@gmail.com>
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

#include "mp_interface.h"

#include "kvi_settings.h"

#if (!defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MAC) && !defined(COMPILE_ON_MINGW))
	#include <QtDBus/QtDBus>

	class KviMPRISInterface : public KviMediaPlayerInterface
	{
	public:
		KviMPRISInterface();
		virtual ~KviMPRISInterface();
	public:
		QString m_szServiceName;
		virtual int detect(bool bStart);
		virtual bool prev();
		virtual bool next();
		virtual bool play();
		virtual bool stop();
		virtual bool pause();
		virtual bool quit();
		virtual KviMediaPlayerInterface::PlayerStatus status();
		virtual QString nowPlaying();
		virtual QString mrl();
		virtual QString title();
		virtual QString artist();
		virtual QString genre();
		virtual QString comment();
		virtual QString year();
		virtual QString album();
		virtual int bitRate();
		virtual int sampleRate();
		virtual bool setVol(kvs_int_t &iVol);
		virtual int getVol();
		virtual int position();
		virtual int length();
		virtual bool jumpTo(kvs_int_t &iPos);
	};

	class KviAudaciousInterface : public KviMPRISInterface
	{
	public:
		KviAudaciousInterface();
	public:
		virtual bool quit();
		virtual KviMediaPlayerInterface::PlayerStatus status();
		virtual QString mrl();
		virtual int length();

		virtual int getPlayListPos();
		virtual QString year();
		virtual QString mediaType();
	};

	class KviBmpxInterface : public KviMPRISInterface
	{
	public:
		KviBmpxInterface();
		virtual KviMediaPlayerInterface::PlayerStatus status();
	};

	class KviAmarok2Interface : public KviMPRISInterface
	{
	public:
		KviAmarok2Interface();
	};

	class KviQmmpInterface : public KviMPRISInterface
	{
	public:
		KviQmmpInterface();
	};

	class KviXmms2Interface : public KviMPRISInterface
	{
	public:
		KviXmms2Interface();
	};
	
	class KviSongbirdInterface : public KviMPRISInterface
	{
	public:
		KviSongbirdInterface();
		virtual KviMediaPlayerInterface::PlayerStatus status();
	};

	MP_DECLARE_DESCRIPTOR(KviAudaciousInterface)
	MP_DECLARE_DESCRIPTOR(KviBmpxInterface)
	MP_DECLARE_DESCRIPTOR(KviAmarok2Interface)
	MP_DECLARE_DESCRIPTOR(KviQmmpInterface)
	MP_DECLARE_DESCRIPTOR(KviXmms2Interface)
	MP_DECLARE_DESCRIPTOR(KviSongbirdInterface)

#endif //COMPILE_ON_WINDOWS

#endif //_MP_AUDACIOUSINTERFACE_H_
