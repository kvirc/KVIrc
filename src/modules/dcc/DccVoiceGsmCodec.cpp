//=============================================================================
//
//   File : DccVoiceGsmCodec.cpp
//   Creation date : Wed Aug 22 19:12:55 2001 GMT by Szymon Stefanek
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

#define _GSMCODEC_CPP_

#include "DccVoiceGsmCodec.h"

#ifdef COMPILE_USE_GSM
#include <dlfcn.h>

#define GSM_PACKED_FRAME_SIZE_IN_BYTES 33
#define GSM_UNPACKED_FRAME_SIZE_IN_BYTES 320
#define GSM_UNPACKED_FRAME_SIZE_IN_SHORTS 160

void * (*gsm_session_create)() = nullptr;
void (*gsm_session_destroy)(void *) = nullptr;
void (*gsm_session_encode)(void *, short *, unsigned char *) = nullptr;
int (*gsm_session_decode)(void *, unsigned char *, short *) = nullptr;

void * g_pGSMCodecLibraryHandle = nullptr;

bool kvi_gsm_codec_init()
{
	if(g_pGSMCodecLibraryHandle)
		return true; // Already initialized

	g_pGSMCodecLibraryHandle = dlopen("libgsm.so", RTLD_NOW | RTLD_GLOBAL);
	if(!g_pGSMCodecLibraryHandle)
		return false; // no way to open it

	gsm_session_create = (void * (*)())dlsym(g_pGSMCodecLibraryHandle, "gsm_create");
	gsm_session_destroy = (void (*)(void *))dlsym(g_pGSMCodecLibraryHandle, "gsm_destroy");
	gsm_session_encode = (void (*)(void *, short *, unsigned char *))dlsym(g_pGSMCodecLibraryHandle, "gsm_encode");
	gsm_session_decode = (int (*)(void *, unsigned char *, short *))dlsym(g_pGSMCodecLibraryHandle, "gsm_decode");

	if(!(gsm_session_create && gsm_session_destroy && gsm_session_encode && gsm_session_decode))
	{
		dlclose(g_pGSMCodecLibraryHandle);
		g_pGSMCodecLibraryHandle = nullptr;
		return false;
	}
	return true;
}

void kvi_gsm_codec_done()
{
	if(g_pGSMCodecLibraryHandle)
	{
		dlclose(g_pGSMCodecLibraryHandle);
		g_pGSMCodecLibraryHandle = nullptr;
	}
}

DccVoiceGsmCodec::DccVoiceGsmCodec()
    : DccVoiceCodec()
{
	m_pEncodeState = gsm_session_create();
	m_pDecodeState = gsm_session_create();
	m_szName = "gsm (compression 33:320)";
}

DccVoiceGsmCodec::~DccVoiceGsmCodec()
{
	gsm_session_destroy(m_pEncodeState);
	gsm_session_destroy(m_pDecodeState);
}

void DccVoiceGsmCodec::encode(KviDataBuffer * signal, KviDataBuffer * stream)
{
	if(signal->size() < GSM_UNPACKED_FRAME_SIZE_IN_BYTES)
		return; // nothing to encode

	unsigned char * ptr = signal->data();

	int uFrames = signal->size() / GSM_UNPACKED_FRAME_SIZE_IN_BYTES;
	int uTotalDataCompressed = uFrames * GSM_UNPACKED_FRAME_SIZE_IN_BYTES;
	int uFrameOffset = stream->size();
	unsigned char * endPtr = ptr + uTotalDataCompressed;

	stream->addSize(GSM_PACKED_FRAME_SIZE_IN_BYTES * uFrames);

	while(ptr != endPtr)
	{
		gsm_session_encode(m_pEncodeState, (short *)ptr, stream->data() + uFrameOffset);
		ptr += GSM_UNPACKED_FRAME_SIZE_IN_BYTES;
		uFrameOffset += GSM_PACKED_FRAME_SIZE_IN_BYTES;
	}
	signal->remove(uTotalDataCompressed);
}

void DccVoiceGsmCodec::decode(KviDataBuffer * stream, KviDataBuffer * signal)
{
	if(stream->size() < GSM_PACKED_FRAME_SIZE_IN_BYTES)
		return; // nothing to decode

	unsigned char * ptr = stream->data();

	// Gsm codec
	int uFrames = stream->size() / GSM_PACKED_FRAME_SIZE_IN_BYTES;
	int uTotalDataDecompressed = uFrames * GSM_PACKED_FRAME_SIZE_IN_BYTES;
	int uSignalOffset = signal->size();
	unsigned char * endPtr = ptr + (uTotalDataDecompressed);

	signal->addSize(GSM_UNPACKED_FRAME_SIZE_IN_BYTES * uFrames);

	while(ptr != endPtr)
	{
		// We don't check the return value here
		// Well..it is either an unrecoverable internal error
		// or a broken frame...
		// but if we receive broken frames over DCC...well....better
		// check the hardware...or the remote codec as well...
		gsm_session_decode(m_pDecodeState, ptr, (short *)(signal->data() + uSignalOffset));
		ptr += GSM_PACKED_FRAME_SIZE_IN_BYTES;
		uSignalOffset += GSM_UNPACKED_FRAME_SIZE_IN_BYTES;
	}
	stream->remove(uTotalDataDecompressed);
}

int DccVoiceGsmCodec::encodedFrameSize()
{
	return GSM_PACKED_FRAME_SIZE_IN_BYTES;
}

int DccVoiceGsmCodec::decodedFrameSize()
{
	return GSM_UNPACKED_FRAME_SIZE_IN_BYTES;
}

#endif // COMPILE_USE_GSM
