//=============================================================================
//
//   File : mp_amarokinterface.cpp
//   Created on Sun 27 Mar 2005 16:55:09 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005 Szymon Stefanek <pragma at kvirc dot net>
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "mp_amarokinterface.h"

#ifdef COMPILE_KDE3_SUPPORT

#include "kvi_app.h"
#include "kvi_locale.h"

#include "dcopclient.h"
#include "kurl.h"

#include <QDataStream>
#include <QLinkedList>

MP_IMPLEMENT_DESCRIPTOR(
	KviAmarokInterface,
	"amarok",
	__tr2qs_ctx(
		"An interface to the AmaroK KDE media player."
		,
		"mediaplayer"
	)
)


KviAmarokInterface::KviAmarokInterface()
	: KviDCOPHelper(true, "amarok")
{
}

KviAmarokInterface::~KviAmarokInterface()
{
}

int KviAmarokInterface::detect(bool bStart){ return detectApp("amarok",bStart,95,99); }

bool KviAmarokInterface::prev(){ return voidRetVoidDCOPCall("player","prev()"); }
bool KviAmarokInterface::next(){ return voidRetVoidDCOPCall("player","next()"); }
bool KviAmarokInterface::play(){ return voidRetVoidDCOPCall("player","play()"); }
bool KviAmarokInterface::stop(){ return voidRetVoidDCOPCall("player","stop()"); }
bool KviAmarokInterface::pause(){ return voidRetVoidDCOPCall("player","pause()"); }
bool KviAmarokInterface::mute(){ return voidRetVoidDCOPCall("player","mute()"); }
bool KviAmarokInterface::quit(){ return voidRetVoidDCOPCall("MainApplication-Interface","quit()"); }

bool KviAmarokInterface::jumpTo(kvs_int_t &iPos)
{
	return voidRetIntDCOPCall("player","seek(int)",iPos/1000);
}
bool KviAmarokInterface::setVol(kvs_int_t &iVol)
{
	return voidRetIntDCOPCall("player","setVolume(int)",100*iVol/255);
}

#define MP_DCOP_STRING_CALL(_fncname,_iface,_fnc) \
QString KviAmarokInterface::_fncname() \
{ \
	QString ret; \
	if(!stringRetVoidDCOPCall(_iface,_fnc,ret))return false; \
	return ret; \
}

MP_DCOP_STRING_CALL(nowPlaying,"player","nowPlaying()")
MP_DCOP_STRING_CALL(artist,"player","artist()")
MP_DCOP_STRING_CALL(title,"player","title()")
MP_DCOP_STRING_CALL(genre,"player","genre()")
MP_DCOP_STRING_CALL(year,"player","year()")
MP_DCOP_STRING_CALL(comment,"player","comment()")
MP_DCOP_STRING_CALL(album,"player","album()")

int KviAmarokInterface::getVol()
{
	int ret;
	if(!intRetVoidDCOPCall("player","getVolume()",ret))return false;
	return ret * 255 / 100;
}

int KviAmarokInterface::sampleRate()
{
	int ret;
	if(!intRetVoidDCOPCall("player","sampleRate()",ret))return false;
	return ret;
}

int KviAmarokInterface::length()
{
	int ret;
	if(!intRetVoidDCOPCall("player","trackTotalTime()",ret))return false;
	return ret * 1000;
}

int KviAmarokInterface::position()
{
	int ret;
	if(!intRetVoidDCOPCall("player","trackCurrentTime()",ret))return false;
	return ret * 1000;
}

bool KviAmarokInterface::getRepeat()
{
	bool ret;
	if(!boolRetVoidDCOPCall("player","repeatTrackStatus()",ret))return false;
	return ret;
}

bool KviAmarokInterface::getShuffle()
{
	bool ret;
	if(!boolRetVoidDCOPCall("player","randomModeStatus()",ret))return false;
	return ret;
}

bool KviAmarokInterface::setRepeat(bool &bVal)
{
	return voidRetBoolDCOPCall("player","enableRepeatTrack(bool)",bVal);
}

bool KviAmarokInterface::setShuffle(bool &bVal)
{
	return voidRetBoolDCOPCall("player","enableRandomMode(bool)",bVal);
}

KviMediaPlayerInterface::PlayerStatus KviAmarokInterface::status()
{
	int ret;
	if(!intRetVoidDCOPCall("player","status()",ret))return KviMediaPlayerInterface::Unknown;
	switch(ret)
	{
		case 0:
			return KviMediaPlayerInterface::Stopped;
		break;
		case 1:
			return KviMediaPlayerInterface::Paused;
		break;
		case 2:
			return KviMediaPlayerInterface::Playing;
		break;
		default:
			return KviMediaPlayerInterface::Unknown;
		break;
	}
	return KviMediaPlayerInterface::Unknown;
}

QString KviAmarokInterface::mrl()
{
	QString ret;
	if(!stringRetVoidDCOPCall("player","encodedURL()",ret))return false;
	KURL url(ret);
	return url.prettyURL();
}

bool KviAmarokInterface::playMrl(const QString &mrl)
{
	QString title;
	QByteArray data, replyData;
	KviQCString replyType;
	QDataStream arg(data,IO_WriteOnly);
	arg << KURL(mrl);
	if(!g_pApp->dcopClient()->call(m_szAppId,"playlist","playMedia(KURL)",data,replyType,replyData))
		return false;
	return true;
}


#endif //COMPILE_KDE_SUPPORT
