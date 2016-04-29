#ifndef _CODEC_H_
#define _CODEC_H_
//=============================================================================
//
//   File : DccVoiceCodec.h
//   Creation date : Sun Aug 26 04:19:34 2001 GMT by Szymon Stefanek
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

#include "KviCString.h"
#include "KviDataBuffer.h"

#ifndef COMPILE_DISABLE_OGG_THEORA
#include "KviOggTheoraDecoder.h"
#include "KviOggTheoraEncoder.h"
#endif

class DccVoiceCodec
{
public:
	DccVoiceCodec();
	virtual ~DccVoiceCodec();

protected:
	KviCString m_szName;

public:
	const char * name();
	virtual void encode(KviDataBuffer * signal, KviDataBuffer * stream);
	virtual void decode(KviDataBuffer * stream, KviDataBuffer * signal);
	virtual int encodedFrameSize();
	virtual int decodedFrameSize();
};

class DccVoiceNullCodec : public DccVoiceCodec
{
public:
	DccVoiceNullCodec();
	virtual ~DccVoiceNullCodec();

public:
	virtual void encode(KviDataBuffer * signal, KviDataBuffer * stream);
	virtual void decode(KviDataBuffer * stream, KviDataBuffer * signal);
	virtual int encodedFrameSize();
	virtual int decodedFrameSize();
};

class DccVideoCodec
{
public:
	DccVideoCodec();
	virtual ~DccVideoCodec();

protected:
	KviCString m_szName;

public:
	const char * name();
	virtual void encodeVideo(KviDataBuffer * videoSignal, KviDataBuffer * stream);
	virtual void encodeText(KviDataBuffer * textSignal, KviDataBuffer * stream);
	virtual void decode(KviDataBuffer * stream, KviDataBuffer * videoSignal, KviDataBuffer * textSignal);
	virtual int encodedFrameSize();
	virtual int decodedFrameSize();
};

class DccVideoSJpegCodec : public DccVideoCodec
{
public:
	DccVideoSJpegCodec();
	virtual ~DccVideoSJpegCodec();

public:
	virtual void encodeVideo(KviDataBuffer * videoSignal, KviDataBuffer * stream);
	virtual void encodeText(KviDataBuffer * textSignal, KviDataBuffer * stream);
	virtual void decode(KviDataBuffer * stream, KviDataBuffer * videoSignal, KviDataBuffer * textSignal);
	virtual int encodedFrameSize();
	virtual int decodedFrameSize();
};

#ifndef COMPILE_DISABLE_OGG_THEORA
class DccVideoTheoraCodec : public DccVideoCodec
{
public:
	DccVideoTheoraCodec();
	virtual ~DccVideoTheoraCodec();

public:
	virtual void encodeVideo(KviDataBuffer * videoSignal, KviDataBuffer * stream);
	virtual void encodeText(KviDataBuffer * textSignal, KviDataBuffer * stream);
	virtual void decode(KviDataBuffer * stream, KviDataBuffer * videoSignal, KviDataBuffer * textSignal);
	virtual int encodedFrameSize();
	virtual int decodedFrameSize();

private:
	KviOggTheoraEncoder * m_pEncoder;
	KviOggTheoraDecoder * m_pDecoder;
};
#endif // COMPILE_DISABLE_OGG_THEORA
#endif //_CODEC_H_
