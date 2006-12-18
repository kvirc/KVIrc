#ifndef _MP_JUKINTERFACE_H_
#define _MP_JUKINTERFACE_H_
//=============================================================================
//
//   File : mp_jukinterface.h
//   Created on Tue 29 Mar 2005 01:38:34 by Szymon Stfeanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005 Szymon Stfeanek <pragma at kvirc dot net>
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
	
	class KviJukInterface : public KviMediaPlayerDCOPInterface
	{
	public:
		KviJukInterface();
		~KviJukInterface();
	public:
		virtual int detect(bool bStart);
		virtual bool prev();
		virtual bool next();
		virtual bool play();
		virtual bool stop();
		virtual bool pause();
		virtual bool quit();
		virtual bool jumpTo(int &iPos);
		virtual bool setVol(int &iVol);
		virtual QString nowPlaying();
		virtual QString mrl();
		/*
		virtual QString title();
		virtual QString artist();
		virtual QString genre();
		virtual QString comment();
		virtual QString year();
		virtual QString album();
		*/
		virtual int position();
		virtual int length();
		/*
		virtual int bitRate();
		virtual int sampleRate();
		*/
		virtual bool getShuffle();
		virtual bool setShuffle(bool &bVal);

		virtual bool playMrl(const QString &mrl);
		virtual KviMediaPlayerInterface::PlayerStatus status();
	};
	
	MP_DECLARE_DESCRIPTOR(KviJukInterface)

#endif //COMPILE_KDE_SUPPORT

#endif //!_MP_JUKINTERFACE_H_
