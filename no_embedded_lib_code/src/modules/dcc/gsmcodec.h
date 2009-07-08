#ifndef _GSMCODEC_H_
#define _GSMCODEC_H_
//=============================================================================
//
//   File : gsmcodec.h
//   Creation date : Wed Aug 22 19:12:54 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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


#ifdef COMPILE_USE_GSM

	#include "codec.h"

	#ifndef _GSMCODEC_CPP_
		extern bool kvi_gsm_codec_init();
		extern void kvi_gsm_codec_done();
	#endif //_GSMCODEC_CPP_

	class KviDccVoiceGsmCodec : public KviDccVoiceCodec
	{
	public:
		KviDccVoiceGsmCodec();
		virtual ~KviDccVoiceGsmCodec();
	private:
		void * m_pEncodeState;
		void * m_pDecodeState;
	public:
		virtual void encode(KviDataBuffer * signal,KviDataBuffer * stream);
		virtual void decode(KviDataBuffer * stream,KviDataBuffer * signal);
		virtual int encodedFrameSize();
		virtual int decodedFrameSize();
	};

#endif //COMPILE_USE_GSM

#endif //_GSMCODEC_H_
