#ifndef _MP_AUDACIOUSINTERFACE_H_
#define _MP_AUDACIOUSINTERFACE_H_
//=============================================================================
//
//   File : mp_audaciousinterface.h
//   Created on Thu 06 Dec 2007 14:20:02 by Tomasz Moń
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2007 Tomasz Moń <desowin@gmail.com>
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
//=============================================================================

#include "kvi_settings.h"
#include "mp_interface.h"

#if (!defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MAC))
#include <QtDBus/QtDBus>

	class KviAudaciousInterface : public KviMediaPlayerInterface
	{
	public:
		KviAudaciousInterface();
		virtual ~KviAudaciousInterface();
	public:
		virtual int detect(bool bStart);
		virtual bool prev();
		virtual bool next();
		virtual bool play();
		virtual bool stop();
		virtual bool pause();
		virtual bool quit();
		virtual QString nowPlaying();
		virtual QString mrl();
		virtual bool setVol(kvs_int_t &iVol);
		virtual int getVol();
		virtual KviMediaPlayerInterface::PlayerStatus status();
		virtual int position();
		virtual int length();
		virtual bool jumpTo(int &iPos);
		virtual QString title();
		virtual QString artist();
		virtual QString genre();
		virtual QString comment();
		virtual QString album();

		virtual int getPlayListPos();
		virtual QString year();
		virtual QString mediaType();
	};
	
	MP_DECLARE_DESCRIPTOR(KviAudaciousInterface)
	
#endif //!COMPILE_ON_WINDOWS

#endif //!_MP_AUDACIOUSINTERFACE_H_
