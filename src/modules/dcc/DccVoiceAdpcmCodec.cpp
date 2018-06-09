//=============================================================================
//
//   File : DccVoiceAdpcmCodec.cpp
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
// Copyright 1992 by Stichting Mathematisch Centrum, Amsterdam, The Netherlands.
//                           All Rights Reserved
//
// Permission to use, copy, modify, and distribute this software and its
// documentation for any purpose and without fee is hereby granted,
// provided that the above copyright notice appear in all copies and that
// both that copyright notice and this permission notice appear in
// supporting documentation, and that the names of Stichting Mathematisch
// Centrum or CWI not be used in advertising or publicity pertaining to
// distribution of the software without specific, written prior permission.
//
// STICHTING MATHEMATISCH CENTRUM DISCLAIMS ALL WARRANTIES WITH REGARD TO
// THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
// FITNESS, IN NO EVENT SHALL STICHTING MATHEMATISCH CENTRUM BE LIABLE
// FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
// OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
//
// Intel/DVI ADPCM coder/decoder.
//
// The algorithm for this coder was taken from the IMA Compatibility Project
// proceedings, Vol 2, Number 2; May 1992.
//
// Version 1.2, 18-Dec-92.
//

#define _ADPCMCODEC_CPP_
#include "DccVoiceAdpcmCodec.h"

#include <cstdio> /*DBG*/

#ifndef __STDC__
#define signed
#endif

#define ADPCM_PACKED_FRAME_SIZE_IN_BYTES 512
#define ADPCM_UNPACKED_FRAME_SIZE_IN_BYTES 2048
#define ADPCM_UNPACKED_FRAME_SIZE_IN_SHORTS 1024

// Intel ADPCM step variation table */
static int indexTable[16] = {
	-1, -1, -1, -1, 2, 4, 6, 8,
	-1, -1, -1, -1, 2, 4, 6, 8,
};

static int stepsizeTable[89] = {
	7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
	19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
	50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
	130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
	337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
	876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
	2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
	5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
	15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};

void ADPCM_compress(short indata[], char outdata[], int len, ADPCM_state * state)
{
	short * lpIn;         /* Input buffer pointer */
	signed char * lpOut;  /* output buffer pointer */
	int val;              /* Current input sample value */
	int sign;             /* Current adpcm sign bit */
	int delta;            /* Current adpcm output value */
	int diff;             /* Difference between val and valprev */
	int step;             /* Stepsize */
	int valpred;          /* Predicted output value */
	int vpdiff;           /* Current change to valpred */
	int index;            /* Current step change index */
	int outputbuffer = 0; /* place to keep previous 4-bit value */
	int bufferstep;       /* toggle between outputbuffer/output */

	lpOut = (signed char *)outdata;
	lpIn = indata;

	valpred = state->valprev;
	index = state->index;
	step = stepsizeTable[index];

	bufferstep = 1;

	for(; len > 0; len--)
	{
		val = *lpIn++;
		// Step 1 - compute difference with previous value
		diff = val - valpred;
		sign = (diff < 0) ? 8 : 0;
		if(sign)
			diff = (-diff);
		// Step 2 - Divide and clamp
		// Note:
		// This code *approximately* computes:
		//    delta = diff*4/step;
		//    vpdiff = (delta+0.5)*step/4;
		// but in shift step bits are dropped. The net result of this is
		// that even if you have fast mul/div hardware you cannot put it to
		// good use since the fixup would be too expensive.
		//
		delta = 0;
		vpdiff = (step >> 3);
		if(diff >= step)
		{
			delta = 4;
			diff -= step;
			vpdiff += step;
		}
		step >>= 1;
		if(diff >= step)
		{
			delta |= 2;
			diff -= step;
			vpdiff += step;
		}
		step >>= 1;
		if(diff >= step)
		{
			delta |= 1;
			vpdiff += step;
		}
		// Step 3 - Update previous value
		if(sign)
			valpred -= vpdiff;
		else
			valpred += vpdiff;
		// Step 4 - Clamp previous value to 16 bits
		if(valpred > 32767)
			valpred = 32767;
		else if(valpred < -32768)
			valpred = -32768;
		// Step 5 - Assemble value, update index and step values
		delta |= sign;
		index += indexTable[delta];
		if(index < 0)
			index = 0;
		if(index > 88)
			index = 88;
		step = stepsizeTable[index];
		// Step 6 - Output value
		if(bufferstep)
			outputbuffer = (delta << 4) & 0xf0;
		else
			*lpOut++ = (delta & 0x0f) | outputbuffer;
		bufferstep = !bufferstep;
	}
	// Output last step, if needed
	if(!bufferstep)
		*lpOut++ = outputbuffer;
	state->valprev = valpred;
	state->index = index;
}

void ADPCM_uncompress(char indata[], short outdata[], int len, ADPCM_state * state)
{
	signed char * inp;   /* Input buffer pointer */
	short * outp;        /* output buffer pointer */
	int sign;            /* Current adpcm sign bit */
	int delta;           /* Current adpcm output value */
	int step;            /* Stepsize */
	int valpred;         /* Predicted value */
	int vpdiff;          /* Current change to valpred */
	int index;           /* Current step change index */
	int inputbuffer = 0; /* place to keep next 4-bit value */
	int bufferstep;      /* toggle between inputbuffer/input */

	outp = outdata;
	inp = (signed char *)indata;

	valpred = state->valprev;
	index = state->index;
	step = stepsizeTable[index];

	bufferstep = 0;

	for(; len > 0; len--)
	{
		/* Step 1 - get the delta value */
		if(bufferstep)
			delta = inputbuffer & 0xf;
		else
		{
			inputbuffer = *inp++;
			delta = (inputbuffer >> 4) & 0xf;
		}
		bufferstep = !bufferstep;

		/* Step 2 - Find new index value (for later) */
		index += indexTable[delta];
		if(index < 0)
			index = 0;
		if(index > 88)
			index = 88;

		/* Step 3 - Separate sign and magnitude */
		sign = delta & 8;
		delta = delta & 7;

		/* Step 4 - Compute difference and new predicted value */
		/*
		** Computes 'vpdiff = (delta+0.5)*step/4', but see comment
		** in adpcm_coder.
		*/
		vpdiff = step >> 3;
		if(delta & 4)
			vpdiff += step;
		if(delta & 2)
			vpdiff += step >> 1;
		if(delta & 1)
			vpdiff += step >> 2;

		if(sign)
			valpred -= vpdiff;
		else
			valpred += vpdiff;

		/* Step 5 - clamp output value */
		if(valpred > 32767)
			valpred = 32767;
		else if(valpred < -32768)
			valpred = -32768;

		/* Step 6 - Update step value */
		step = stepsizeTable[index];

		/* Step 7 - Output value */
		*outp++ = valpred;
	}

	state->valprev = valpred;
	state->index = index;
}

DccVoiceAdpcmCodec::DccVoiceAdpcmCodec()
    : DccVoiceCodec()
{
	m_pEncodeState = new ADPCM_state;
	m_pEncodeState->index = 0;
	m_pEncodeState->valprev = 0;
	m_pDecodeState = new ADPCM_state;
	m_pDecodeState->index = 0;
	m_pDecodeState->valprev = 0;
	m_szName = "adpcm (compression 1:4)";
}

DccVoiceAdpcmCodec::~DccVoiceAdpcmCodec()
{
	delete m_pEncodeState;
	delete m_pDecodeState;
}

void DccVoiceAdpcmCodec::encode(KviDataBuffer * signal, KviDataBuffer * stream)
{
	if(signal->size() < ADPCM_UNPACKED_FRAME_SIZE_IN_BYTES)
		return; // nothing to encode

	char * ptr = (char *)signal->data();

	int uFrames = signal->size() / ADPCM_UNPACKED_FRAME_SIZE_IN_BYTES;
	int uTotalDataCompressed = uFrames * ADPCM_UNPACKED_FRAME_SIZE_IN_BYTES;
	int uFrameOffset = stream->size();
	char * endPtr = ptr + uTotalDataCompressed;

	stream->addSize(ADPCM_PACKED_FRAME_SIZE_IN_BYTES * uFrames);

	while(ptr != endPtr)
	{
		ADPCM_compress((short *)ptr, (char *)(stream->data() + uFrameOffset), ADPCM_UNPACKED_FRAME_SIZE_IN_SHORTS, m_pEncodeState);
		ptr += ADPCM_UNPACKED_FRAME_SIZE_IN_BYTES;
		uFrameOffset += ADPCM_PACKED_FRAME_SIZE_IN_BYTES;
	}
	signal->remove(uTotalDataCompressed);
}

void DccVoiceAdpcmCodec::decode(KviDataBuffer * stream, KviDataBuffer * signal)
{
	if(stream->size() < ADPCM_PACKED_FRAME_SIZE_IN_BYTES)
		return; // nothing to decode

	char * ptr = (char *)stream->data();

	// Adpcm codec
	int uFrames = stream->size() / ADPCM_PACKED_FRAME_SIZE_IN_BYTES;
	int uTotalDataDecompressed = uFrames * ADPCM_PACKED_FRAME_SIZE_IN_BYTES;
	int uSignalOffset = signal->size();
	char * endPtr = ptr + (uTotalDataDecompressed);

	signal->addSize(ADPCM_UNPACKED_FRAME_SIZE_IN_BYTES * uFrames);

	while(ptr != endPtr)
	{
		ADPCM_uncompress((char *)ptr, (short *)(signal->data() + uSignalOffset), ADPCM_UNPACKED_FRAME_SIZE_IN_SHORTS, m_pDecodeState);
		ptr += ADPCM_PACKED_FRAME_SIZE_IN_BYTES;
		uSignalOffset += ADPCM_UNPACKED_FRAME_SIZE_IN_BYTES;
	}
	stream->remove(uTotalDataDecompressed);
}

int DccVoiceAdpcmCodec::encodedFrameSize()
{
	return ADPCM_PACKED_FRAME_SIZE_IN_BYTES;
}

int DccVoiceAdpcmCodec::decodedFrameSize()
{
	return ADPCM_UNPACKED_FRAME_SIZE_IN_BYTES;
}
