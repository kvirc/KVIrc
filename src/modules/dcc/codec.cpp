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


#include <QImage>
#include <QByteArray>
#include <QBuffer>

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

void KviDccVideoCodec::encodeVideo(KviDataBuffer *,KviDataBuffer *)
{
}

void KviDccVideoCodec::encodeText(KviDataBuffer *,KviDataBuffer *)
{
}

void KviDccVideoCodec::decode(KviDataBuffer *,KviDataBuffer *,KviDataBuffer *)
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

KviDccVideoSJpegCodec::KviDccVideoSJpegCodec()
: KviDccVideoCodec()
{
	m_szName = "sjpeg";
}

KviDccVideoSJpegCodec::~KviDccVideoSJpegCodec()
{
}

void KviDccVideoSJpegCodec::encodeVideo(KviDataBuffer * videoSignal,KviDataBuffer * stream)
{
	
	if(videoSignal->size() < 1) return;

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
		stream->append((const unsigned char*)ba.data(),ba.size());
        }
	videoSignal->clear();
}

void KviDccVideoSJpegCodec::encodeText(KviDataBuffer * textSignal, KviDataBuffer * stream)
{
	static unsigned const char *irct_magic_init = (unsigned const char*) "\0<text>\0";
	static unsigned const char *irct_magic_end = (unsigned const char*) "\0</text>\0";

	qDebug("encodeText %s %d",textSignal->data(), textSignal->size());
	if(textSignal->size() < 1) return;
	stream->append(irct_magic_init,8);
	stream->append((const unsigned char*)textSignal->data(),textSignal->size());
	stream->append(irct_magic_end,9);
	textSignal->clear();
	return;
}

void KviDccVideoSJpegCodec::decode(KviDataBuffer * stream,KviDataBuffer * videoSignal,KviDataBuffer * textSignal)
{
	static unsigned const char jpg_magic_init[4] = { 0xFF, 0xD8, 0xFF, 0xE0}; //SOI + APP0
	static unsigned const char jpg_magic_end[2] = { 0xFF, 0xD9}; //EOI

	static unsigned const char *irct_magic_init = (unsigned const char*) "\0<text>\0";
	static unsigned const char *irct_magic_end = (unsigned const char*) "\0</text>\0";

	if(stream->size() < 1)return;


	int txtStart = stream->find(irct_magic_init, 8);
	int txtEnd = stream->find(irct_magic_end, 9);

	int jpgStart = stream->find(jpg_magic_init, 4);
		
	if(txtStart!=-1 && txtEnd !=-1 && txtStart<jpgStart)
	{
		qDebug("a txtStart %d txtEnd %d",txtStart,txtEnd);
		stream->remove(txtStart+8);
		int len = txtEnd - txtStart -8; //-8 irct_magic_init
		if(len>0)
		{
			textSignal->append(stream->data(), len);
			char*txt=(char*)malloc(len+1);
			memcpy(txt,stream->data(), len);
			txt[len]='\0';
			qDebug("a recv |%s| %d",txt,len);
		}
		stream->remove(len+9);
	}

	jpgStart = stream->find(jpg_magic_init, 4);
	int jpgEnd = stream->find(jpg_magic_end, 2);

	if(jpgStart != -1 && jpgEnd != -1)
	{
// 		qDebug("jpgStart %d jpgEnd %d",jpgStart,jpgEnd);
		QImage img;
		//remove junk before jpeg start
		stream->remove(jpgStart);
		int len = jpgEnd - jpgStart + 1;
		
		img.loadFromData(stream->data(), stream->size());
		if(!img.isNull())
		{
			videoSignal->clear();
			videoSignal->append(img.bits(),img.numBytes());
		}
		stream->remove(len);
	}

	txtStart = stream->find(irct_magic_init, 8);
	txtEnd = stream->find(irct_magic_end, 9);

	if(txtStart!=-1 && txtEnd !=-1)
	{
		qDebug("b txtStart %d txtEnd %d",txtStart,txtEnd);
		stream->remove(txtStart+8);
		int len = txtEnd - txtStart -8; //-8 irct_magic_init
		if(len>0)
		{
			textSignal->append(stream->data(), len);
			char*txt=(char*)malloc(len+1);
			memcpy(txt,stream->data(), len);
			txt[len]='\0';
			qDebug("b recv |%s| %d",txt,len);
		}
		stream->remove(len+9);
	}
}

int KviDccVideoSJpegCodec::encodedFrameSize()
{
	return 0;
}

int KviDccVideoSJpegCodec::decodedFrameSize()
{
	return 0;
}


#ifndef COMPILE_DISABLE_OGG_THEORA
KviDccVideoTheoraCodec::KviDccVideoTheoraCodec()
: KviDccVideoCodec()
{
	m_szName = "theora";
	m_pEncoder = 0;
	m_pDecoder = 0;
}

KviDccVideoTheoraCodec::~KviDccVideoTheoraCodec()
{
	if(m_pEncoder)
		delete m_pEncoder;
	m_pEncoder=0;

	if(m_pDecoder)
		delete m_pDecoder;
	m_pDecoder=0;
}

void KviDccVideoTheoraCodec::encodeVideo(KviDataBuffer * videoSignal,KviDataBuffer * stream)
{
	if(videoSignal->size() < 1) return;

	if(!m_pEncoder)
		m_pEncoder = new KviTheoraEncoder(stream);

	m_pEncoder->addVideoFrame((QRgb *) videoSignal->data(),videoSignal->size());
	videoSignal->clear();
}

void KviDccVideoTheoraCodec::encodeText(KviDataBuffer * textSignal, KviDataBuffer * stream)
{
	if(textSignal->size() < 1) return;

	if(!m_pEncoder)
		m_pEncoder = new KviTheoraEncoder(stream);

	m_pEncoder->addTextFrame(textSignal->data(),textSignal->size());
	textSignal->clear();
}

void KviDccVideoTheoraCodec::decode(KviDataBuffer * stream,KviDataBuffer * videoSignal,KviDataBuffer * textSignal)
{
	if(stream->size() < 1)return;

	if(!m_pDecoder)
		m_pDecoder = new KviTheoraDecoder(videoSignal, textSignal);

	m_pDecoder->addData(stream);
}

int KviDccVideoTheoraCodec::encodedFrameSize()
{
	return 0;
}

int KviDccVideoTheoraCodec::decodedFrameSize()
{
	return 0;
}
#endif // COMPILE_DISABLE_OGG_THEORA
