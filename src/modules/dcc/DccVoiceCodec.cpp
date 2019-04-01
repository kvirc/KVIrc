//=============================================================================
//
//   File : DccVoiceCodec.cpp
//   Creation date : Sun Aug 26 04:19:36 2001 GMT by Szymon Stefanek
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

#include "DccVoiceCodec.h"

#include <QImage>
#include <QByteArray>
#include <QBuffer>

DccVoiceCodec::DccVoiceCodec()
    = default;

DccVoiceCodec::~DccVoiceCodec()
    = default;

void DccVoiceCodec::encode(KviDataBuffer *, KviDataBuffer *)
{
}

void DccVoiceCodec::decode(KviDataBuffer *, KviDataBuffer *)
{
}

int DccVoiceCodec::encodedFrameSize()
{
	return 0;
}

int DccVoiceCodec::decodedFrameSize()
{
	return 0;
}

const char * DccVoiceCodec::name()
{
	return m_szName.ptr();
}

DccVoiceNullCodec::DccVoiceNullCodec()
    : DccVoiceCodec()
{
	m_szName = "null (no compression)";
}

DccVoiceNullCodec::~DccVoiceNullCodec()
    = default;

void DccVoiceNullCodec::encode(KviDataBuffer * signal, KviDataBuffer * stream)
{
	if(signal->size() < 1)
		return;
	stream->append(signal->data(), signal->size());
	signal->resize(0);
}

void DccVoiceNullCodec::decode(KviDataBuffer * stream, KviDataBuffer * signal)
{
	if(stream->size() < 1)
		return;
	signal->append(stream->data(), stream->size());

	stream->resize(0);
}

int DccVoiceNullCodec::encodedFrameSize()
{
	return 1024;
}

int DccVoiceNullCodec::decodedFrameSize()
{
	return 1024;
}

DccVideoCodec::DccVideoCodec()
    = default;

DccVideoCodec::~DccVideoCodec()
    = default;

void DccVideoCodec::encodeVideo(KviDataBuffer *, KviDataBuffer *)
{
}

void DccVideoCodec::encodeText(KviDataBuffer *, KviDataBuffer *)
{
}

void DccVideoCodec::decode(KviDataBuffer *, KviDataBuffer *, KviDataBuffer *)
{
}

int DccVideoCodec::encodedFrameSize()
{
	return 0;
}

int DccVideoCodec::decodedFrameSize()
{
	return 0;
}

const char * DccVideoCodec::name()
{
	return m_szName.ptr();
}

DccVideoSJpegCodec::DccVideoSJpegCodec()
    : DccVideoCodec()
{
	m_szName = "sjpeg";
}

DccVideoSJpegCodec::~DccVideoSJpegCodec()
    = default;

void DccVideoSJpegCodec::encodeVideo(KviDataBuffer * videoSignal, KviDataBuffer * stream)
{

	if(videoSignal->size() < 1)
		return;

	QImage img(videoSignal->data(), 320, 240, 1280, QImage::Format_ARGB32);
	QByteArray ba;
	QBuffer buffer(&ba);
	buffer.open(QIODevice::WriteOnly);
	//0 to obtain small compressed files,
	//100 for large uncompressed files,
	//and -1 (the default) to use the default settings.
	img.save(&buffer, "JPEG", 20);
	if(ba.size() > 0)
	{
		stream->append((const unsigned char *)ba.data(), ba.size());
	}
	videoSignal->clear();
}

void DccVideoSJpegCodec::encodeText(KviDataBuffer * textSignal, KviDataBuffer * stream)
{
	static unsigned const char * irct_magic_init = (unsigned const char *)"\0<text>\0";
	static unsigned const char * irct_magic_end = (unsigned const char *)"\0</text>\0";

	qDebug("encodeText %s %d", textSignal->data(), textSignal->size());
	if(textSignal->size() < 1)
		return;
	stream->append(irct_magic_init, 8);
	stream->append((const unsigned char *)textSignal->data(), textSignal->size());
	stream->append(irct_magic_end, 9);
	textSignal->clear();
	return;
}

void DccVideoSJpegCodec::decode(KviDataBuffer * stream, KviDataBuffer * videoSignal, KviDataBuffer * textSignal)
{
	static unsigned const char jpg_magic_init[4] = { 0xFF, 0xD8, 0xFF, 0xE0 }; //SOI + APP0
	static unsigned const char jpg_magic_end[2] = { 0xFF, 0xD9 };              //EOI

	static unsigned const char * irct_magic_init = (unsigned const char *)"\0<text>\0";
	static unsigned const char * irct_magic_end = (unsigned const char *)"\0</text>\0";

	if(stream->size() < 1)
		return;

	int txtStart = stream->find(irct_magic_init, 8);
	int txtEnd = stream->find(irct_magic_end, 9);

	int jpgStart = stream->find(jpg_magic_init, 4);

	if(txtStart != -1 && txtEnd != -1 && txtStart < jpgStart)
	{
		qDebug("a txtStart %d txtEnd %d", txtStart, txtEnd);
		stream->remove(txtStart + 8);
		int len = txtEnd - txtStart - 8; //-8 irct_magic_init
		if(len > 0)
		{
			textSignal->append(stream->data(), len);
			char * txt = (char *)malloc(len + 1);
			memcpy(txt, stream->data(), len);
			txt[len] = '\0';
			qDebug("a recv |%s| %d", txt, len);
		}
		stream->remove(len + 9);
	}

	if(stream->size() < 1)
		return;

	jpgStart = stream->find(jpg_magic_init, 4);
	int jpgEnd = stream->find(jpg_magic_end, 2);

	if(jpgStart != -1 && jpgEnd != -1)
	{
		// 		qDebug("jpgStart %d jpgEnd %d",jpgStart,jpgEnd);
		QImage img;
		//remove junk before jpeg start
		if(jpgStart > 0)
			stream->remove(jpgStart);

		int len = jpgEnd - jpgStart + 1;

		img.loadFromData(stream->data(), stream->size());
		if(!img.isNull())
		{
			videoSignal->clear();
			videoSignal->append(img.bits(), img.byteCount());
		}
		stream->remove(len);
	}

	txtStart = stream->find(irct_magic_init, 8);
	txtEnd = stream->find(irct_magic_end, 9);

	if(txtStart != -1 && txtEnd != -1)
	{
		qDebug("b txtStart %d txtEnd %d", txtStart, txtEnd);
		stream->remove(txtStart + 8);
		int len = txtEnd - txtStart - 8; //-8 irct_magic_init
		if(len > 0)
		{
			textSignal->append(stream->data(), len);
			char * txt = (char *)malloc(len + 1);
			memcpy(txt, stream->data(), len);
			txt[len] = '\0';
			qDebug("b recv |%s| %d", txt, len);
		}
		stream->remove(len + 9);
	}
}

int DccVideoSJpegCodec::encodedFrameSize()
{
	return 0;
}

int DccVideoSJpegCodec::decodedFrameSize()
{
	return 0;
}

#ifndef COMPILE_DISABLE_OGG_THEORA
DccVideoTheoraCodec::DccVideoTheoraCodec()
    : DccVideoCodec()
{
	m_szName = "theora";
	m_pEncoder = nullptr;
	m_pDecoder = nullptr;
}

DccVideoTheoraCodec::~DccVideoTheoraCodec()
{
	if(m_pEncoder)
		delete m_pEncoder;
	m_pEncoder = nullptr;

	if(m_pDecoder)
		delete m_pDecoder;
	m_pDecoder = nullptr;
}

void DccVideoTheoraCodec::encodeVideo(KviDataBuffer * videoSignal, KviDataBuffer * stream)
{
	if(videoSignal->size() < 1)
		return;

	if(!m_pEncoder)
		m_pEncoder = new KviOggTheoraEncoder(stream);

	m_pEncoder->addVideoFrame((QRgb *)videoSignal->data(), videoSignal->size());
	videoSignal->clear();
}

void DccVideoTheoraCodec::encodeText(KviDataBuffer * textSignal, KviDataBuffer * stream)
{
	if(textSignal->size() < 1)
		return;

	if(!m_pEncoder)
		m_pEncoder = new KviOggTheoraEncoder(stream);

	m_pEncoder->addTextFrame(textSignal->data(), textSignal->size());
	textSignal->clear();
}

void DccVideoTheoraCodec::decode(KviDataBuffer * stream, KviDataBuffer * videoSignal, KviDataBuffer * textSignal)
{
	if(stream->size() < 1)
		return;

	if(!m_pDecoder)
		m_pDecoder = new KviOggTheoraDecoder(videoSignal, textSignal);

	m_pDecoder->addData(stream);
}

int DccVideoTheoraCodec::encodedFrameSize()
{
	return 0;
}

int DccVideoTheoraCodec::decodedFrameSize()
{
	return 0;
}
#endif // COMPILE_DISABLE_OGG_THEORA
