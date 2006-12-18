#ifndef _ADPCMCODEC_H_
#define _ADPCMCODEC_H_
//
//   File : adpcmcodec.h
//   Creation date : Wed Aug 22 19:12:46 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Code derived from adpcm.c : Intel ADPCM coder/decoder
//   Adapted for the KVirc distribution by Szymon Stefanek (pragma at kvirc dot net)
//   Last revision : 22 Aug 2001
//   See kvi_adpcm.cpp for the complete copyright notice.
//
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

#include <qstring.h> // fix for qtextstream.h
#include "codec.h"

typedef struct adpcm_state
{
    short valprev; /* Previous output value */
    char  index;   /* Index into stepsize table */
} ADPCM_state;


class KviDccVoiceAdpcmCodec : public KviDccVoiceCodec
{
public:
	KviDccVoiceAdpcmCodec();
	virtual ~KviDccVoiceAdpcmCodec();
private:
	ADPCM_state * m_pEncodeState;
	ADPCM_state * m_pDecodeState;
public:
	virtual void encode(KviDataBuffer * signal,KviDataBuffer * stream);
	virtual void decode(KviDataBuffer * stream,KviDataBuffer * signal);
	virtual int encodedFrameSize();
	virtual int decodedFrameSize();
};


#endif //_ADPCMCODEC_H_
