#ifndef _MP_AMAROKINTERFACE_H_
#define _MP_AMAROKINTERFACE_H_
//=============================================================================
//
//   File : mp_amarokinterface.h
//   Created on Sun 27 Mar 2005 16:55:09 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005 Szymon Stefanek <pragma at kvirc dot net>
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

#ifdef COMPILE_KDE_SUPPORT

	#include "mp_dcopinterface.h"
	
	class KviAmarokInterface : public KviMediaPlayerDCOPInterface
	{
	public:
		KviAmarokInterface();
		~KviAmarokInterface();
	public:
		virtual int detect(bool bStart);
		virtual bool prev();
		virtual bool next();
		virtual bool play();
		virtual bool stop();
		virtual bool pause();
		virtual bool jumpTo(int &iPos);
		virtual bool setVol(int &iVol);
		virtual bool mute();
		virtual int getVol();
		virtual bool quit();
		virtual QString nowPlaying();
		virtual QString mrl();
		virtual QString title();
		virtual QString artist();
		virtual QString genre();
		virtual QString comment();
		virtual QString year();
		virtual QString album();
		virtual int position();
		virtual int length();
		//virtual int bitRate(); unsupported
		virtual int sampleRate();
		virtual bool playMrl(const QString &mrl);
		virtual KviMediaPlayerInterface::PlayerStatus status();
		virtual bool getRepeat();
		virtual bool getShuffle();
		virtual bool setRepeat(bool &bVal);
		virtual bool setShuffle(bool &bVal);
	};
	
	MP_DECLARE_DESCRIPTOR(KviAmarokInterface)

#endif //COMPILE_KDE_SUPPORT

#endif //!_MP_AMAROKINTERFACE_H_
