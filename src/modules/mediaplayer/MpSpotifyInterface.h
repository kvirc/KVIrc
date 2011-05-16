#ifndef _MP_SPOTIFYINTERFACE_H_
#define _MP_SPOTIFYINTERFACE_H_
//=============================================================================
//
//   File : MpSpotifyInterface.h
//   Creation date : Sat 09 Apr 2011 10:38:12 by David López
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2011 David López <grannost at gmail dot com>
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

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)

	#include "MpInterface.h"

	class KviSpotifyInterface : public MpInterface
	{
	public:
		KviSpotifyInterface();
		~KviSpotifyInterface();
	public:
		virtual int detect(bool bStart);
		virtual QString nowPlaying();
		virtual QString artist();
		virtual QString title();
		virtual bool show();
		virtual bool minimize();
		virtual bool hide();
		virtual bool prev() { notImplemented(); return false; };
		virtual bool next() { notImplemented(); return false; };
		virtual bool play() { notImplemented(); return false; };
		virtual bool stop() { notImplemented(); return false; };
		virtual bool pause() { notImplemented(); return false; };
		virtual QString mrl() { notImplemented(); return QString(); };
		virtual MpInterface::PlayerStatus status();
	protected:
		QString get_window_title();
	};

	MP_DECLARE_DESCRIPTOR(KviSpotifyInterface)

#endif //COMPILE_ON_WINDOWS

#endif //!_MP_SPOTIFYINTERFACE_H_
