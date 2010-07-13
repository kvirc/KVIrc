//=============================================================================
//
//   File : mp_mprisinterface.cpp
//   Creation date : Thu 06 Dec 2007 14:20:02 by Tomasz Moń
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2007-2009 Tomasz Moń <desowin@gmail.com>
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

#include "mp_mprisinterface.h"
#if (!defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MAC) && !defined(COMPILE_ON_MINGW))
#include "kvi_locale.h"

/*
	according to MPRIS 1.0, GetStatus returns struct of 4 integers.

	First integer: 0 = Playing, 1 = Paused, 2 = Stopped.
	Second interger: 0 = Playing linearly , 1 = Playing randomly.
	Third integer: 0 = Go to the next element once the current has finished playing , 1 = Repeat the current element
	Fourth integer: 0 = Stop playing once the last element has been played, 1 = Never give up playing
*/

struct MPRISPlayerStatus
{
	int Play;
	int Random;
	int RepeatCurrent;
	int RepeatPlaylist;
};
Q_DECLARE_METATYPE( MPRISPlayerStatus )

QDBusArgument &operator<<(QDBusArgument &argument, const MPRISPlayerStatus &status)
{
	argument.beginStructure();
	argument << status.Play << status.Random << status.RepeatCurrent << status.RepeatPlaylist;
	argument.endStructure();
	return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, MPRISPlayerStatus &status)
{
	argument.beginStructure();
	argument >> status.Play >> status.Random >> status.RepeatCurrent >> status.RepeatPlaylist;
	argument.endStructure();
	return argument;
}

KviMPRISInterface::KviMPRISInterface()
: KviMediaPlayerInterface()
{
	qDBusRegisterMetaType<MPRISPlayerStatus>();
}

KviMPRISInterface::~KviMPRISInterface()
{
}

int KviMPRISInterface::detect(bool)
{
	QDBusReply<QStringList> reply = QDBusConnection::sessionBus().interface()->registeredServiceNames();
	if (!reply.isValid())		/* something fishy with dbus, it won't work */
		return 0;

	foreach (QString name, reply.value())
		if (name == m_szServiceName)	/* player is running */
			return 100;

	return 1;	/* dbus works, player may be closed */
}

#define MPRIS_SIMPLE_CALL(__path, __action) \
	QDBusInterface dbus_iface(m_szServiceName, __path, \
				"org.freedesktop.MediaPlayer", QDBusConnection::sessionBus()); \
	QDBusMessage reply = dbus_iface.call(QDBus::Block, __action); \
	if (reply.type() == QDBusMessage::ErrorMessage) { \
		QDBusError err = reply; \
		debug("Error: %s\n%s\n", qPrintable(err.name()), qPrintable(err.message())); \
		return FALSE; \
	} \
	return TRUE;

bool KviMPRISInterface::prev()
{
	MPRIS_SIMPLE_CALL("/Player", "Prev")
}

bool KviMPRISInterface::next()
{
	MPRIS_SIMPLE_CALL("/Player", "Next")
}

bool KviMPRISInterface::play()
{
	MPRIS_SIMPLE_CALL("/Player", "Play")
}

bool KviMPRISInterface::stop()
{
	MPRIS_SIMPLE_CALL("/Player", "Stop")
}

bool KviMPRISInterface::pause()
{
	MPRIS_SIMPLE_CALL("/Player", "Pause")
}

bool KviMPRISInterface::quit()
{
	MPRIS_SIMPLE_CALL("/", "Quit")
}

#define MPRIS_CALL_METHOD(__method, __return_if_fail) \
	QDBusInterface dbus_iface(m_szServiceName, "/Player", \
				"org.freedesktop.MediaPlayer", QDBusConnection::sessionBus()); \
	QDBusMessage reply = dbus_iface.call(QDBus::Block, __method); \
	if (reply.type() == QDBusMessage::ErrorMessage) { \
		QDBusError err = reply; \
		debug("Error: %s\n%s\n", qPrintable(err.name()), qPrintable(err.message())); \
		return __return_if_fail; \
	}

#define MPRIS_GET_METADATA_FIELD(__field, __return_type, __return_if_fail) \
	if (this->status() != KviMediaPlayerInterface::Playing) \
		return __return_if_fail; \
	MPRIS_CALL_METHOD("GetMetadata", __return_if_fail) \
	foreach (QVariant v, reply.arguments()) { \
		QDBusArgument arg = qvariant_cast<QDBusArgument>(v); \
		QVariant v = qdbus_cast<QVariantMap>(arg); \
		if (v.userType() == QVariant::Map) { \
        		const QVariantMap map = v.toMap(); \
        		QVariantMap::ConstIterator it = map.find(__field); \
                        if (it != map.end() && it.key() == __field) { \
                                return it.value().value< __return_type >(); \
                        } \
		} \
	} \
	return __return_if_fail;

#define MPRIS_CALL_METHOD_WITH_ARG(__method, __arg, __return_if_fail) \
        QDBusInterface dbus_iface(m_szServiceName, "/Player", \
                                "org.freedesktop.MediaPlayer", QDBusConnection::sessionBus()); \
        QDBusMessage reply = dbus_iface.call(QDBus::Block, __method, __arg); \
        if (reply.type() == QDBusMessage::ErrorMessage) { \
                QDBusError err = reply; \
                debug("Error: %s\n%s\n", qPrintable(err.name()), qPrintable(err.message())); \
                return __return_if_fail; \
        }

KviMediaPlayerInterface::PlayerStatus KviMPRISInterface::status()
{
	MPRIS_CALL_METHOD("GetStatus", KviMediaPlayerInterface::Unknown)

	MPRISPlayerStatus status;

	if(reply.arguments().isEmpty())
		return KviMediaPlayerInterface::Unknown;

	status = qdbus_cast<MPRISPlayerStatus>(reply.arguments().first());

	switch (status.Play) {
		case 0: return KviMediaPlayerInterface::Playing;
		case 1: return KviMediaPlayerInterface::Paused;
		case 2: return KviMediaPlayerInterface::Stopped;
		default: return KviMediaPlayerInterface::Unknown;
	}
}

QString KviMPRISInterface::nowPlaying()
{
	if (this->status() != KviMediaPlayerInterface::Playing)
		return "";

	MPRIS_CALL_METHOD("GetMetadata", "")

	QString artist;
	QString title;
	foreach (QVariant v, reply.arguments()) {
		QDBusArgument arg = qvariant_cast<QDBusArgument>(v);
		QVariant v = qdbus_cast<QVariantMap>(arg);
		if (v.userType() == QVariant::Map) {
        		const QVariantMap map = v.toMap();
        		QVariantMap::ConstIterator it = map.constBegin();
        		for ( ; it != map.constEnd(); ++it) { /* maybe do some configureable formatting */
				if (it.key() == "artist")
					artist = it.value().toString();
				else if (it.key() == "title")
					title = it.value().toString();
			}
		}
	}
	if (artist.length() && title.length())
		return artist + " - " + title;
	else
		return "";
}

QString KviMPRISInterface::mrl()
{
	MPRIS_CALL_METHOD("GetMetadata", "")

	foreach (QVariant v, reply.arguments()) {
		QDBusArgument arg = qvariant_cast<QDBusArgument>(v);
		QVariant v = qdbus_cast<QVariantMap>(arg);
		if (v.userType() == QVariant::Map) {
        		const QVariantMap map = v.toMap();
        		QVariantMap::ConstIterator it = map.find("location");
        		if (it != map.end() && it.key() == "location") {
				return it.value().toString();
			}
		}
	}
	return "";
}

QString KviMPRISInterface::title()
{
	MPRIS_GET_METADATA_FIELD("title", QString, "")
}

QString KviMPRISInterface::artist()
{
	MPRIS_GET_METADATA_FIELD("artist", QString, "")
}

QString KviMPRISInterface::genre()
{
	MPRIS_GET_METADATA_FIELD("genre", QString, "")
}

QString KviMPRISInterface::comment()
{
	MPRIS_GET_METADATA_FIELD("comment", QString, "")
}

QString KviMPRISInterface::year()
{
	MPRIS_GET_METADATA_FIELD("year", QString, "")
}

QString KviMPRISInterface::album()
{
	MPRIS_GET_METADATA_FIELD("album", QString, "")
}

int KviMPRISInterface::bitRate()
{
	MPRIS_GET_METADATA_FIELD("audio-bitrate", int, -1)
}

int KviMPRISInterface::sampleRate()
{
	MPRIS_GET_METADATA_FIELD("audio-samplerate", int, -1)
}

bool KviMPRISInterface::setVol(kvs_int_t &iVol)
{
	MPRIS_CALL_METHOD_WITH_ARG("VolumeSet", QVariant((int)(100*iVol/255)), false);
	return true;
}

int KviMPRISInterface::getVol()
{
	MPRIS_CALL_METHOD("VolumeGet", -1)

	int iVol = reply.arguments().first().toInt();
	return iVol * 255 /100;
}

int KviMPRISInterface::position()
{
	MPRIS_CALL_METHOD("PositionGet", -1)
	return reply.arguments().first().toInt();
}

int KviMPRISInterface::length()
{
	MPRIS_CALL_METHOD("GetMetadata", -1)

	foreach (QVariant v, reply.arguments()) {
		QDBusArgument arg = qvariant_cast<QDBusArgument>(v);
		QVariant v = qdbus_cast<QVariantMap>(arg);
		if (v.userType() == QVariant::Map) {
        		const QVariantMap map = v.toMap();
        		QVariantMap::ConstIterator it = map.constBegin();
        		for ( ; it != map.constEnd(); ++it) {
				if (it.key() == "mtime")
					return it.value().toInt();
			}
		}
	}
	return -1;
}

bool KviMPRISInterface::jumpTo(kvs_int_t &iPos)
{
	MPRIS_CALL_METHOD_WITH_ARG("PositionSet", QVariant((int)iPos), false)
	return true;
}

/* audacious interface */
MP_IMPLEMENT_DESCRIPTOR(
	KviAudaciousInterface,
	"audacious",
	__tr2qs_ctx(
		"An interface to the popular audacious media player.\n" \
		"Download it from http://audacious-media-player.org\n"
		,
		"mediaplayer"
	)
)

KviAudaciousInterface::KviAudaciousInterface()
: KviMPRISInterface()
{
	m_szServiceName = "org.mpris.audacious";
}

int KviAudaciousInterface::getPlayListPos()
{
	QDBusInterface dbus_iface("org.mpris.audacious", "/org/atheme/audacious",
				"org.atheme.audacious", QDBusConnection::sessionBus());
	QDBusReply<uint> pos = dbus_iface.call(QDBus::Block, "Position");
	return pos;
}


bool KviAudaciousInterface::quit()
{
	if (KviMPRISInterface::quit())
		return TRUE;

	/* compability with older versions */
	MPRIS_SIMPLE_CALL("/Player", "Quit")
}

QString KviAudaciousInterface::mrl()
{
	MPRIS_CALL_METHOD("GetMetadata", "")

	foreach (QVariant v, reply.arguments()) {
		QDBusArgument arg = qvariant_cast<QDBusArgument>(v);
		QVariant v = qdbus_cast<QVariantMap>(arg);
		if (v.userType() == QVariant::Map) {
        		const QVariantMap map = v.toMap();
        		QVariantMap::ConstIterator it = map.find("location");
        		if (it != map.end() && it.key() == "location") {
				return it.value().toString();
			}
			/* Some audacious versions send URI instead of location */
			it = map.find("URI");
                        if (it != map.end() && it.key() == "URI") {
                                return it.value().toString();
                        }
		}
	}
	return "";
}

KviMediaPlayerInterface::PlayerStatus KviAudaciousInterface::status()
{
	KviMediaPlayerInterface::PlayerStatus status;
	status = KviMPRISInterface::status();
	if (status != KviMediaPlayerInterface::Unknown)
		return status;

	/* compability with older versions */
	QDBusInterface dbus_iface(m_szServiceName, "/Player",
				"org.freedesktop.MediaPlayer", QDBusConnection::sessionBus());
	if (!dbus_iface.isValid())
		return KviMediaPlayerInterface::Unknown;

	QDBusMessage reply = dbus_iface.call(QDBus::Block, "GetStatus");

	switch (reply.arguments().first().toInt()) {
		case 0: return KviMediaPlayerInterface::Playing;
		case 1: return KviMediaPlayerInterface::Paused;
		case 2: return KviMediaPlayerInterface::Stopped;
		default: return KviMediaPlayerInterface::Unknown;
	}
}

int KviAudaciousInterface::length()
{
	int length = KviMPRISInterface::length();
	if (length != -1)
		return length;

	/* compability with older versions */
	MPRIS_CALL_METHOD("GetMetadata", -1)

	foreach (QVariant v, reply.arguments()) {
		QDBusArgument arg = qvariant_cast<QDBusArgument>(v);
		QVariant v = qdbus_cast<QVariantMap>(arg);
		if (v.userType() == QVariant::Map) {
        		const QVariantMap map = v.toMap();
        		QVariantMap::ConstIterator it = map.constBegin();
        		for ( ; it != map.constEnd(); ++it) {
				if (it.key() == "length")
					return it.value().toInt();
			}
		}
	}
	return -1;
}

#define AUDACIOUS_GET_TUPLE_FIELD(__field) \
	if (this->status() != KviMediaPlayerInterface::Playing) \
		return ""; \
	QDBusInterface dbus_iface("org.mpris.audacious", "/org/atheme/audacious", \
				"org.atheme.audacious", QDBusConnection::sessionBus()); \
	QList<QVariant> args; \
	args << (uint)this->getPlayListPos() << QString(__field); \
	QDBusReply<QDBusVariant> reply = dbus_iface.callWithArgumentList(QDBus::Block, "SongTuple", args); \
	return reply.value().variant().toString(); \

QString KviAudaciousInterface::year()
{
	AUDACIOUS_GET_TUPLE_FIELD("year")
}

QString KviAudaciousInterface::mediaType()
{
	AUDACIOUS_GET_TUPLE_FIELD("codec")
}

/* BMPx interface */
MP_IMPLEMENT_DESCRIPTOR(
	KviBmpxInterface,
	"bmpx",
	__tr2qs_ctx(
		"An interface to BMPx.\n" \
		"Download it from http://sourceforge.net/projects/beepmp\n"
		,
		"mediaplayer"
	)
)

KviBmpxInterface::KviBmpxInterface()
: KviMPRISInterface()
{
	m_szServiceName = "org.mpris.bmp";
}

/* BMPx doesn't provide GetStatus method, always assume Playing status */
KviMediaPlayerInterface::PlayerStatus KviBmpxInterface::status()
{
	return KviMediaPlayerInterface::Playing;
}

/* Amarok2 interface */
MP_IMPLEMENT_DESCRIPTOR(
	KviAmarok2Interface,
	"amarok2",
	__tr2qs_ctx(
		"An interface to Amarok2.\n" \
		"Download it from http://amarok.kde.org\n"
		,
		"mediaplayer"
	)
)

KviAmarok2Interface::KviAmarok2Interface()
: KviMPRISInterface()
{
	m_szServiceName = "org.mpris.amarok";
}

/* Qmmp interface */
MP_IMPLEMENT_DESCRIPTOR(
        KviQmmpInterface,
        "Qmmp",
        __tr2qs_ctx(
                "An interface to Qmmp.\n" \
                "Download it from http://qmmp.ylsoftware.com\n"
                ,
                "mediaplayer"
        )
)

KviQmmpInterface::KviQmmpInterface()
: KviMPRISInterface()
{
        m_szServiceName = "org.mpris.qmmp";
}

/* xmms2 interface */
MP_IMPLEMENT_DESCRIPTOR(
	KviXmms2Interface,
	"xmms2",
	__tr2qs_ctx(
		"An interface to the Xmms2 media player.\n" \
		"Download it from http://xmms2.org\n",
		"mediaplayer"
	)
)

KviXmms2Interface::KviXmms2Interface()
: KviMPRISInterface()
{
	m_szServiceName = "org.mpris.xmms2";
}

/* mozilla songbird interface */
MP_IMPLEMENT_DESCRIPTOR(
	KviSongbirdInterface,
	"songbird",
	__tr2qs_ctx(
		"An interface to the Mozilla Songbird media player.\n" \
		"Download it from http://www.getsongbird.com.\n" \
		"To use it you have to install also the MPRIS addon " \
		"available at http://addons.songbirdnest.com/addon/1626.\n",
		"mediaplayer"
	)
)

KviSongbirdInterface::KviSongbirdInterface()
: KviMPRISInterface()
{
	m_szServiceName = "org.mpris.songbird";
}

KviMediaPlayerInterface::PlayerStatus KviSongbirdInterface::status()
{
	return KviMediaPlayerInterface::Playing;
}

#endif //COMPILE_ON_WINDOWS
