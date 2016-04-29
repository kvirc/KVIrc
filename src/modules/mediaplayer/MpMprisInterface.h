#ifndef _MP_AUDACIOUSINTERFACE_H_
#define _MP_AUDACIOUSINTERFACE_H_
//=============================================================================
//
//   File : MpMprisInterface.h
//   Creation date : Thu 06 Dec 2007 14:20:02 by Tomasz Moń
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2007-2009 Tomasz Moń <desowin@gmail.com>
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

#include "MpInterface.h"

#include "kvi_settings.h"

#if(defined(COMPILE_DBUS_SUPPORT) && !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MAC) && !defined(COMPILE_ON_MINGW))
#include <QtDBus/QtDBus>

class MpMprisInterface : public MpInterface
{
public:
	MpMprisInterface();
	virtual ~MpMprisInterface();

public:
	QString m_szServiceName;
	virtual int detect(bool bStart);
	virtual bool prev();
	virtual bool next();
	virtual bool play();
	virtual bool stop();
	virtual bool pause();
	virtual bool quit();
	virtual MpInterface::PlayerStatus status();
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
	virtual bool setVol(kvs_int_t & iVol);
	virtual int getVol();
	virtual int position();
	virtual int length();
	virtual bool jumpTo(kvs_int_t & iPos);
};

class MpAudaciousInterface : public MpMprisInterface
{
public:
	MpAudaciousInterface();

public:
	virtual bool quit();
	virtual MpInterface::PlayerStatus status();
	virtual QString mrl();
	virtual int length();

	virtual int getPlayListPos();
	virtual QString year();
	virtual QString mediaType();
};

class MpBmpxInterface : public MpMprisInterface
{
public:
	MpBmpxInterface();
	virtual MpInterface::PlayerStatus status();
};

class MpAmarok2Interface : public MpMprisInterface
{
public:
	MpAmarok2Interface();
};

class MpQmmpInterface : public MpMprisInterface
{
public:
	MpQmmpInterface();
};

class MpXmms2Interface : public MpMprisInterface
{
public:
	MpXmms2Interface();
};

class MpSongbirdInterface : public MpMprisInterface
{
public:
	MpSongbirdInterface();
	virtual MpInterface::PlayerStatus status();
};

class MpTotemInterface : public MpMprisInterface
{
public:
	MpTotemInterface();
};

class MpVlcInterface : public MpMprisInterface
{
public:
	MpVlcInterface();
};

class MpClementineInterface : public MpMprisInterface
{
public:
	MpClementineInterface();
};

MP_DECLARE_DESCRIPTOR(MpAudaciousInterface)
MP_DECLARE_DESCRIPTOR(MpBmpxInterface)
MP_DECLARE_DESCRIPTOR(MpAmarok2Interface)
MP_DECLARE_DESCRIPTOR(MpQmmpInterface)
MP_DECLARE_DESCRIPTOR(MpXmms2Interface)
MP_DECLARE_DESCRIPTOR(MpSongbirdInterface)
MP_DECLARE_DESCRIPTOR(MpTotemInterface)
MP_DECLARE_DESCRIPTOR(MpVlcInterface)
MP_DECLARE_DESCRIPTOR(MpClementineInterface)
#endif //COMPILE_ON_WINDOWS

#endif //_MP_AUDACIOUSINTERFACE_H_
