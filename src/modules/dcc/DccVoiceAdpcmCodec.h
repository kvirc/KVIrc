#ifndef _ADPCMCODEC_H_
#define _ADPCMCODEC_H_
//=============================================================================
//
//   File : DccVoiceAdpcmCodec.h
//   Creation date : Wed Aug 22 19:12:46 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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
//
// adpcm.h - include file for adpcm coder.
//
// Version 1.0, 7-Jul-92.
//
//
// Average compression speed experiment: P166MMX 32MEG
//
// INPUT         | OUTPUT    | COMPRESSION TIME
// ------------------------------------------------------------------------
// shorts bytes  | bytes     | min msec.   avrg.    high peak (1 time)
// ------------------------------------------------------------------------
// 512    1024   | 256       | ~4          4          34
// 1024   2048   | 512       | ~7          8          75
// 2048   4096   | 1024      | ~13         15         175 (!!!)
// 4096   8192   | 2048      | ~26         33         91
// 8192   16384  | 4096      | ~57         80         220 (~1 sec of audio)
// 16384  32768  | 8192      | ~110        250        290
//

#include <QString> // fix for qtextstream.h

#include "DccVoiceCodec.h"

struct ADPCM_state
{
	short valprev; /* Previous output value */
	char index;    /* HelpIndex into stepsize table */
};

class DccVoiceAdpcmCodec : public DccVoiceCodec
{
public:
	DccVoiceAdpcmCodec();
	virtual ~DccVoiceAdpcmCodec();

private:
	ADPCM_state * m_pEncodeState;
	ADPCM_state * m_pDecodeState;

public:
	virtual void encode(KviDataBuffer * signal, KviDataBuffer * stream);
	virtual void decode(KviDataBuffer * stream, KviDataBuffer * signal);
	virtual int encodedFrameSize();
	virtual int decodedFrameSize();
};

#endif //_ADPCMCODEC_H_
