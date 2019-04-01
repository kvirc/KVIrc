//=============================================================================
//
//   File : MpInterface.cpp
//   Creation date : Fri Mar 25 20:01:25 2005 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "MpInterface.h"
#include "MpMp3.h"

#include "KviOptions.h"
#include "KviLocale.h"

#include <QTextCodec>

static QTextCodec * mediaplayer_get_codec()
{
	QTextCodec * c = QTextCodec::codecForName(KVI_OPTION_STRING(KviOption_stringWinampTextEncoding).toUtf8().data());
	if(!c)
		c = QTextCodec::codecForLocale();
	return c;
}

void MpInterface::notImplemented()
{
	setLastError(__tr2qs_ctx("Function not implemented", "mediaplayer"));
}

int MpInterface::position()
{
	notImplemented();
	return -1;
}

int MpInterface::length()
{
	notImplemented();
	return -1;
}

bool MpInterface::quit()
{
	notImplemented();
	return false;
}

bool MpInterface::hide()
{
	notImplemented();
	return false;
}

bool MpInterface::show()
{
	notImplemented();
	return false;
}

bool MpInterface::minimize()
{
	notImplemented();
	return false;
}

QString MpInterface::getLocalFile()
{
	QString ret = mrl();
	if(ret.isEmpty())
		return ret;
	if(ret.startsWith("file://", Qt::CaseInsensitive))
	{
		ret.remove(0, 7);
		return ret;
	}
	return QString();
}

QString MpInterface::amipEval(const QString &)
{
	return QString();
}

#define SCAN_MP3_FILE           \
	QString f = getLocalFile(); \
	if(f.isEmpty())             \
		return QString();       \
	mp3info mp3;                \
	if(!scan_mp3_file(f, &mp3)) \
		return QString();       \
	[[maybe_unused]] QTextCodec * pCodec = mediaplayer_get_codec();

#define SCAN_MP3_FILE_RET_INT   \
	QString f = getLocalFile(); \
	if(f.isEmpty())             \
		return -1;              \
	mp3info mp3;                \
	if(!scan_mp3_file(f, &mp3)) \
		return -1;

QString MpInterface::artist()
{
	SCAN_MP3_FILE
	return pCodec->toUnicode(QByteArray(mp3.id3.artist));
}

QString MpInterface::title()
{
	SCAN_MP3_FILE
	return pCodec->toUnicode(QByteArray(mp3.id3.title));
}

QString MpInterface::genre()
{
	SCAN_MP3_FILE
	return pCodec->toUnicode(QByteArray(get_typegenre(mp3.id3.genre[0])));
}

QString MpInterface::comment()
{
	SCAN_MP3_FILE
	return pCodec->toUnicode(QByteArray(mp3.id3.comment));
}

QString MpInterface::year()
{
	SCAN_MP3_FILE
	return QString(mp3.id3.year);
}

QString MpInterface::album()
{
	SCAN_MP3_FILE
	return pCodec->toUnicode(QByteArray(mp3.id3.album));
}

bool MpInterface::playMrl(const QString &)
{
	notImplemented();
	return false;
}

bool MpInterface::amipExec(const QString &)
{
	notImplemented();
	return false;
}

bool MpInterface::setVol(kvs_int_t &)
{
	notImplemented();
	return false;
}

int MpInterface::getVol()
{
	notImplemented();
	return -1;
}

bool MpInterface::mute()
{
	notImplemented();
	return false;
}

bool MpInterface::jumpTo(kvs_int_t &)
{
	notImplemented();
	return false;
}

MpInterface::PlayerStatus MpInterface::status()
{
	notImplemented();
	return MpInterface::Unknown;
}

int MpInterface::bitRate()
{
	SCAN_MP3_FILE_RET_INT
	return header_bitrate(&(mp3.header));
}

int MpInterface::sampleRate()
{
	SCAN_MP3_FILE_RET_INT
	return header_frequency(&(mp3.header));
}

int MpInterface::channels()
{
	SCAN_MP3_FILE_RET_INT
	return header_channels(&(mp3.header));
}

int MpInterface::getPlayListPos()
{
	notImplemented();
	return -1;
}

bool MpInterface::setPlayListPos(kvs_int_t &)
{
	notImplemented();
	return false;
}

int MpInterface::getListLength()
{
	notImplemented();
	return -1;
}

int MpInterface::getEqData(kvs_int_t &)
{
	notImplemented();
	return -1;
}

bool MpInterface::setEqData(kvs_int_t &, kvs_int_t &)
{
	notImplemented();
	return false;
}

bool MpInterface::getRepeat()
{
	notImplemented();
	return false;
}

bool MpInterface::getShuffle()
{
	notImplemented();
	return false;
}

bool MpInterface::setRepeat(bool &)
{
	notImplemented();
	return false;
}

bool MpInterface::setShuffle(bool &)
{
	notImplemented();
	return false;
}

QString MpInterface::mediaType()
{
	QString ret = mrl();
	if(ret.endsWith(".mp3", Qt::CaseInsensitive))
		ret = "MPEG Layer 3";
	else if(ret.endsWith(".ogg", Qt::CaseInsensitive))
		ret = "OGG Vorbis";
	else if(ret.endsWith(".avi", Qt::CaseInsensitive))
		ret = "Audio Video Interleave";
	else if(ret.endsWith(".mpeg", Qt::CaseInsensitive))
		ret = "MPEG Video";
	else if(ret.endsWith(".mpg", Qt::CaseInsensitive))
		ret = "MPEG Video";
	else if(ret.startsWith("http://", Qt::CaseInsensitive))
		ret = "HTTP Audio Stream";
	else
		ret = QString();
	return ret;
}
