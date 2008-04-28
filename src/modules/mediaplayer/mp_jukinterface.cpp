//=============================================================================
//
//   File : mp_jukinterface.cpp
//   Created on Tue 29 Mar 2005 01:38:34 by Szymon Stfeanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005 Szymon Stfeanek <pragma at kvirc dot net>
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

#include "mp_jukinterface.h"

#ifdef COMPILE_KDE3_SUPPORT

#include "kvi_app.h"
#include "kvi_locale.h"

#include "dcopclient.h"
#include "kurl.h"

#include <QDataStream>
#include <QLinkedList>

// the dcop interface of this player is incomplete :(

MP_IMPLEMENT_DESCRIPTOR(
	KviJukInterface,
	"juk",
	__tr2qs_ctx(
		"An interface to the Juk KDE media player."
		,
		"mediaplayer"
	)
)


KviJukInterface::KviJukInterface()
	: KviDCOPHelper(true, "amarok")
{
}

KviJukInterface::~KviJukInterface()
{
}

int KviJukInterface::detect(bool bStart){ return detectApp("juk",bStart,90,96); }

bool KviJukInterface::prev(){ return voidRetVoidDCOPCall("Player","forward()"); }
bool KviJukInterface::next(){ return voidRetVoidDCOPCall("Player","back()"); }
bool KviJukInterface::play(){ return voidRetVoidDCOPCall("Player","play()"); }
bool KviJukInterface::stop(){ return voidRetVoidDCOPCall("Player","stop()"); }
bool KviJukInterface::pause(){ return voidRetVoidDCOPCall("Player","pause()"); }
bool KviJukInterface::quit(){ return voidRetVoidDCOPCall("MainApplication-Interface","quit()"); }

bool KviJukInterface::setVol(kvs_int_t &iVol)
{
	return voidRetFloatDCOPCall("player","setVolume(float)",(float)(iVol)/255.);
}

bool KviJukInterface::jumpTo(kvs_int_t &iPos)
{
	return voidRetIntDCOPCall("player","seek(int)",iPos/1000);
}

bool KviJukInterface::getShuffle()
{
	QString szMode;
	if(!stringRetVoidDCOPCall("player","randomPlayMode()",szMode))return false;
	return (szMode != "NoRandom");
}

bool KviJukInterface::setShuffle(bool &bVal)
{
	QString szMode;
	if(bVal)
	{
		szMode = "Random";
	} else {
		szMode = "NoRandom";
	}
	if(!voidRetStringDCOPCall("player","setRandomPlayMode(QString)",szMode))return false;
	return true;
}


#define MP_DCOP_STRING_CALL(_fncname,_iface,_fnc) \
QString KviJukInterface::_fncname() \
{ \
	QString ret; \
	if(!stringRetVoidDCOPCall(_iface,_fnc,ret))return false; \
	return ret; \
}

MP_DCOP_STRING_CALL(nowPlaying,"Player","playingString()")

/*
	FIXME: there is trackProperty(propertyName) for these...
MP_DCOP_STRING_CALL(artist,"player","artist()")
MP_DCOP_STRING_CALL(title,"player","title()")
MP_DCOP_STRING_CALL(genre,"player","genre()")
MP_DCOP_STRING_CALL(year,"player","year()")
MP_DCOP_STRING_CALL(comment,"player","comment()")
MP_DCOP_STRING_CALL(album,"player","album()")


int KviJukInterface::sampleRate()
{
	int ret;
	if(!intRetVoidDCOPCall("player","sampleRate()",ret))return false;
	return ret;
}
*/

int KviJukInterface::length()
{
	int ret;
	if(!intRetVoidDCOPCall("player","totalTime()",ret))return false;
	return ret * 1000;
}

int KviJukInterface::position()
{
	int ret;
	if(!intRetVoidDCOPCall("player","currentTime()",ret))return false;
	return ret * 1000;
}

KviMediaPlayerInterface::PlayerStatus KviJukInterface::status()
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

QString KviJukInterface::mrl()
{
	// ugh :/
	QString ret;
	return ret;
}

bool KviJukInterface::playMrl(const QString &mrl)
{
	QString title;
	QByteArray data, replyData;
	KviQCString replyType;
	QDataStream arg(data,IO_WriteOnly);
	arg << mrl;
	if(!g_pApp->dcopClient()->call(m_szAppId,"playlist","play(QString)",data,replyType,replyData))
		return false;
	return true;
}


#endif //COMPILE_KDE_SUPPORT
