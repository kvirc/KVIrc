#ifndef _CODEC_H_
#define _CODEC_H_
//=============================================================================
//
//   File : codec.h
//   Creation date : Sun Aug 26 04:19:34 2001 GMT by Szymon Stefanek
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

#include "kvi_string.h"
#include "kvi_oggtheora.h"
#include "kvi_databuffer.h"

class KviDccVoiceCodec
{
public:
	KviDccVoiceCodec();
	virtual ~KviDccVoiceCodec();
protected:
	KviStr m_szName;
public:
	const char * name();
	virtual void encode(KviDataBuffer * signal,KviDataBuffer * stream);
	virtual void decode(KviDataBuffer * stream,KviDataBuffer * signal);
	virtual int encodedFrameSize();
	virtual int decodedFrameSize();
};

class KviDccVoiceNullCodec : public KviDccVoiceCodec
{
public:
	KviDccVoiceNullCodec();
	virtual ~KviDccVoiceNullCodec();
public:
	virtual void encode(KviDataBuffer * signal,KviDataBuffer * stream);
	virtual void decode(KviDataBuffer * stream,KviDataBuffer * signal);
	virtual int encodedFrameSize();
	virtual int decodedFrameSize();
};

class KviDccVideoCodec
{
public:
	KviDccVideoCodec();
	virtual ~KviDccVideoCodec();
protected:
	KviStr m_szName;
public:
	const char * name();
	virtual void encodeVideo(KviDataBuffer * videoSignal, KviDataBuffer * stream);
	virtual void encodeText(KviDataBuffer * textSignal, KviDataBuffer * stream);
	virtual void decode(KviDataBuffer * stream,KviDataBuffer * videoSignal,KviDataBuffer * textSignal);
	virtual int encodedFrameSize();
	virtual int decodedFrameSize();
};

class KviDccVideoSJpegCodec : public KviDccVideoCodec
{
public:
	KviDccVideoSJpegCodec();
	virtual ~KviDccVideoSJpegCodec();
public:
	virtual void encodeVideo(KviDataBuffer * videoSignal, KviDataBuffer * stream);
	virtual void encodeText(KviDataBuffer * textSignal, KviDataBuffer * stream);
	virtual void decode(KviDataBuffer * stream,KviDataBuffer * videoSignal,KviDataBuffer * textSignal);
	virtual int encodedFrameSize();
	virtual int decodedFrameSize();
};

#ifndef COMPILE_DISABLE_OGG_THEORA
class KviDccVideoTheoraCodec : public KviDccVideoCodec
{
public:
	KviDccVideoTheoraCodec();
	virtual ~KviDccVideoTheoraCodec();
public:
	virtual void encodeVideo(KviDataBuffer * videoSignal, KviDataBuffer * stream);
	virtual void encodeText(KviDataBuffer * textSignal, KviDataBuffer * stream);
	virtual void decode(KviDataBuffer * stream,KviDataBuffer * videoSignal,KviDataBuffer * textSignal);
	virtual int encodedFrameSize();
	virtual int decodedFrameSize();
private:
	KviTheoraEncoder *m_pEncoder;
	KviTheoraDecoder *m_pDecoder;
};
#endif // COMPILE_DISABLE_OGG_THEORA
#endif //_CODEC_H_
