//=============================================================================
//
//   File : mp_interface.cpp
//   Creation date : Fri Mar 25 20:01:25 2005 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2005-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "mp_interface.h"
#include "mp_mp3.h"

#include "kvi_options.h"
#include "kvi_locale.h"

#include <QTextCodec>

static QTextCodec * mediaplayer_get_codec()
{
	QTextCodec * c= QTextCodec::codecForName(KVI_OPTION_STRING(KviOption_stringWinampTextEncoding).toUtf8().data());
	if(!c)c = QTextCodec::codecForLocale();
	return c;

}

void KviMediaPlayerInterface::notImplemented()
{
	setLastError(__tr2qs_ctx("Function not implemented","mediaplayer"));
}

int KviMediaPlayerInterface::position()
{
	notImplemented();
	return -1;
}

int KviMediaPlayerInterface::length()
{
	notImplemented();
	return -1;
}

bool KviMediaPlayerInterface::quit()
{
	notImplemented();
	return false;
}

bool KviMediaPlayerInterface::hide()
{
	notImplemented();
	return false;
}

bool KviMediaPlayerInterface::show()
{
	notImplemented();
	return false;
}

bool KviMediaPlayerInterface::minimize()
{
	notImplemented();
	return false;
}

QString KviMediaPlayerInterface::getLocalFile()
{
	QString ret = mrl();
	if(ret.isEmpty())return ret;
	if(ret.startsWith("file://",Qt::CaseInsensitive))
	{
		ret.remove(0,7);
		return ret;
	}
	return QString();
}

QString KviMediaPlayerInterface::amipEval(const QString &)
{
	return QString();
}

#define SCAN_MP3_FILE \
	QString f = getLocalFile(); \
	if(f.isEmpty())return QString(); \
	mp3info mp3; \
	if(!scan_mp3_file(f,&mp3))return QString(); \
	QTextCodec *pCodec; \
	pCodec=mediaplayer_get_codec();

#define SCAN_MP3_FILE_RET_INT \
	QString f = getLocalFile(); \
	if(f.isEmpty())return -1; \
	mp3info mp3; \
	if(!scan_mp3_file(f,&mp3))return -1;


QString KviMediaPlayerInterface::artist()
{
	SCAN_MP3_FILE
	return pCodec->toUnicode(QByteArray(mp3.id3.artist));
}

QString KviMediaPlayerInterface::title()
{
	SCAN_MP3_FILE
	return pCodec->toUnicode(QByteArray(mp3.id3.title));
}

QString KviMediaPlayerInterface::genre()
{
	SCAN_MP3_FILE
	return pCodec->toUnicode(QByteArray(get_typegenre(mp3.id3.genre[0])));
}

QString KviMediaPlayerInterface::comment()
{
	SCAN_MP3_FILE
	return pCodec->toUnicode(QByteArray(mp3.id3.comment));
}

QString KviMediaPlayerInterface::year()
{
	SCAN_MP3_FILE
	return QString(mp3.id3.year);
}

QString KviMediaPlayerInterface::album()
{
	SCAN_MP3_FILE
	return pCodec->toUnicode(QByteArray(mp3.id3.album));
}

bool KviMediaPlayerInterface::playMrl(const QString &)
{
	notImplemented();
	return false;
}

bool KviMediaPlayerInterface::amipExec(const QString &)
{
	notImplemented();
	return false;
}

bool KviMediaPlayerInterface::setVol(kvs_int_t &)
{
	notImplemented();
	return false;
}

int KviMediaPlayerInterface::getVol()
{
	notImplemented();
	return -1;
}

bool KviMediaPlayerInterface::mute()
{
	notImplemented();
	return false;
}

bool KviMediaPlayerInterface::jumpTo(kvs_int_t &)
{
	notImplemented();
	return false;
}

KviMediaPlayerInterface::PlayerStatus KviMediaPlayerInterface::status()
{
	notImplemented();
	return KviMediaPlayerInterface::Unknown;
}

int KviMediaPlayerInterface::bitRate()
{
	SCAN_MP3_FILE_RET_INT
	return header_bitrate(&(mp3.header));
}

int KviMediaPlayerInterface::sampleRate()
{
	SCAN_MP3_FILE_RET_INT
	return header_frequency(&(mp3.header));
}

int KviMediaPlayerInterface::channels()
{
	SCAN_MP3_FILE_RET_INT
	return header_channels(&(mp3.header));
}

int KviMediaPlayerInterface::getPlayListPos()
{
	notImplemented();
	return -1;
}

bool KviMediaPlayerInterface::setPlayListPos(kvs_int_t &)
{
	notImplemented();
	return false;
}

int KviMediaPlayerInterface::getListLength()
{
	notImplemented();
	return -1;
}

int KviMediaPlayerInterface::getEqData(kvs_int_t &)
{
	notImplemented();
	return -1;
}

bool KviMediaPlayerInterface::setEqData(kvs_int_t &, kvs_int_t &)
{
	notImplemented();
	return false;
}

bool KviMediaPlayerInterface::getRepeat()
{
	notImplemented();
	return false;
}

bool KviMediaPlayerInterface::getShuffle()
{
	notImplemented();
	return false;
}

bool KviMediaPlayerInterface::setRepeat(bool &)
{
	notImplemented();
	return false;
}

bool KviMediaPlayerInterface::setShuffle(bool &)
{
	notImplemented();
	return false;
}

QString KviMediaPlayerInterface::mediaType()
{
	QString ret = mrl();
	if(ret.endsWith(".mp3",Qt::CaseInsensitive))ret = "MPEG Layer 3";
	else if(ret.endsWith(".ogg",Qt::CaseInsensitive))ret = "OGG Vorbis";
	else if(ret.endsWith(".avi",Qt::CaseInsensitive))ret = "Audio Video Interleave";
	else if(ret.endsWith(".mpeg",Qt::CaseInsensitive))ret = "MPEG Video";
	else if(ret.endsWith(".mpg",Qt::CaseInsensitive))ret = "MPEG Video";
	else if(ret.startsWith("http://",Qt::CaseInsensitive))ret = "HTTP Audio Stream";
	else ret = QString();
	return ret;
}
