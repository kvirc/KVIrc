//=============================================================================
//
//   File : codec.cpp
//   Creation date : Sun Aug 26 04:19:36 2001 GMT by Szymon Stefanek
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

#include "codec.h"

KviDccVoiceCodec::KviDccVoiceCodec()
{
}

KviDccVoiceCodec::~KviDccVoiceCodec()
{
}

void KviDccVoiceCodec::encode(KviDataBuffer *,KviDataBuffer *)
{
}

void KviDccVoiceCodec::decode(KviDataBuffer *,KviDataBuffer *)
{
}

int KviDccVoiceCodec::encodedFrameSize()
{
	return 0;
}

int KviDccVoiceCodec::decodedFrameSize()
{
	return 0;
}

const char * KviDccVoiceCodec::name()
{
	return m_szName.ptr();
}

KviDccVoiceNullCodec::KviDccVoiceNullCodec()
: KviDccVoiceCodec()
{
	m_szName = "null (no compression)";
}

KviDccVoiceNullCodec::~KviDccVoiceNullCodec()
{
}

void KviDccVoiceNullCodec::encode(KviDataBuffer * signal,KviDataBuffer * stream)
{
	if(signal->size() < 1)return;
	stream->append(signal->data(),signal->size());
	signal->resize(0);
}

void KviDccVoiceNullCodec::decode(KviDataBuffer * stream,KviDataBuffer * signal)
{
	if(stream->size() < 1)return;
	signal->append(stream->data(),stream->size());
	stream->resize(0);
}

int KviDccVoiceNullCodec::encodedFrameSize()
{
	return 1024;
}

int KviDccVoiceNullCodec::decodedFrameSize()
{
	return 1024;
}

//--------------------------

KviDccVideoCodec::KviDccVideoCodec()
{
}

KviDccVideoCodec::~KviDccVideoCodec()
{
}

void KviDccVideoCodec::encode(KviDataBuffer *,KviDataBuffer *)
{
}

void KviDccVideoCodec::decode(KviDataBuffer *,KviDataBuffer *)
{
}

int KviDccVideoCodec::encodedFrameSize()
{
	return 0;
}

int KviDccVideoCodec::decodedFrameSize()
{
	return 0;
}

const char * KviDccVideoCodec::name()
{
	return m_szName.ptr();
}

KviDccVideoJpegCodec::KviDccVideoJpegCodec()
: KviDccVideoCodec()
{
	m_szName = "jpeg";
}

KviDccVideoJpegCodec::~KviDccVideoJpegCodec()
{
}

void KviDccVideoJpegCodec::encode(KviDataBuffer * signal,KviDataBuffer * stream)
{
	if(signal->size() < 1)return;
	stream->append(signal->data(),signal->size());
	signal->resize(0);
}

void KviDccVideoJpegCodec::decode(KviDataBuffer * stream,KviDataBuffer * signal)
{
	static unsigned const char jpg_magic_init[4] = { 0xFF, 0xD8, 0xFF, 0xE0}; //SOI + APP0
	static unsigned const char jpg_magic_end[2] = { 0xFF, 0xD9}; //EOI

	if(stream->size() < 1)return;
	
	int start = stream->find(jpg_magic_init, 4);
	int end = stream->find(jpg_magic_end, 2);
	if(start != -1 && end != -1)
	{
		//remove junk before jpeg start
		stream->remove(start);
		int len = end - start + 1;
		signal->clear();
		signal->append(stream->data(),stream->size());
		stream->remove(len);
	}
}

int KviDccVideoJpegCodec::encodedFrameSize()
{
	return 0;
}

int KviDccVideoJpegCodec::decodedFrameSize()
{
	return 0;
}
