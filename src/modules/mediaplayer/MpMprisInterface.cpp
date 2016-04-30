//=============================================================================
//
//   File : MpMprisInterface.cpp
//   Creation date : Thu 06 Dec 2007 14:20:02 by Tomasz Moń
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2007-2009 Tomasz Moń <desowin@gmail.com>
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

#include "MpMprisInterface.h"
#if(defined(COMPILE_DBUS_SUPPORT) && !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MAC) && !defined(COMPILE_ON_MINGW))
#include "KviLocale.h"

/*
	according to MPRIS 1.0, GetStatus returns struct of 4 integers.

	First integer: 0 = Playing, 1 = Paused, 2 = Stopped.
	Second integer: 0 = Playing linearly, 1 = Playing randomly.
	Third integer: 0 = Go to the next element once the current has finished playing, 1 = Repeat the current element
	Fourth integer: 0 = Stop playing once the last element has been played, 1 = Never give up playing
*/

struct MPRISPlayerStatus
{
	int Play;
	int Random;
	int RepeatCurrent;
	int RepeatPlaylist;
};
Q_DECLARE_METATYPE(MPRISPlayerStatus)

QDBusArgument & operator<<(QDBusArgument & argument, const MPRISPlayerStatus & status)
{
	argument.beginStructure();
	argument << status.Play << status.Random << status.RepeatCurrent << status.RepeatPlaylist;
	argument.endStructure();
	return argument;
}

const QDBusArgument & operator>>(const QDBusArgument & argument, MPRISPlayerStatus & status)
{
	argument.beginStructure();
	argument >> status.Play >> status.Random >> status.RepeatCurrent >> status.RepeatPlaylist;
	argument.endStructure();
	return argument;
}

MpMprisInterface::MpMprisInterface()
    : MpInterface()
{
	qDBusRegisterMetaType<MPRISPlayerStatus>();
}

MpMprisInterface::~MpMprisInterface()
    = default;

int MpMprisInterface::detect(bool)
{
	QDBusReply<QStringList> reply = QDBusConnection::sessionBus().interface()->registeredServiceNames();
	if(!reply.isValid()) /* something fishy with dbus, it won't work */
		return 0;

	foreach(QString name, reply.value())
		if(name == m_szServiceName) /* player is running */
			return 100;

	return 1; /* dbus works, player may be closed */
}

#define MPRIS_SIMPLE_CALL(__path, __action)                                           \
	QDBusInterface dbus_iface(m_szServiceName, __path,                                \
	    "org.freedesktop.MediaPlayer", QDBusConnection::sessionBus());                \
	QDBusMessage reply = dbus_iface.call(QDBus::Block, __action);                     \
	if(reply.type() == QDBusMessage::ErrorMessage)                                    \
	{                                                                                 \
		QDBusError err = reply;                                                       \
		qDebug("Error: %s\n%s\n", qPrintable(err.name()), qPrintable(err.message())); \
		return false;                                                                 \
	}                                                                                 \
	return true;

bool MpMprisInterface::prev()
{
	MPRIS_SIMPLE_CALL("/Player", "Prev")
}

bool MpMprisInterface::next()
{
	MPRIS_SIMPLE_CALL("/Player", "Next")
}

bool MpMprisInterface::play()
{
	MPRIS_SIMPLE_CALL("/Player", "Play")
}

bool MpMprisInterface::stop()
{
	MPRIS_SIMPLE_CALL("/Player", "Stop")
}

bool MpMprisInterface::pause()
{
	MPRIS_SIMPLE_CALL("/Player", "Pause")
}

bool MpMprisInterface::quit()
{
	MPRIS_SIMPLE_CALL("/", "Quit")
}

#define MPRIS_CALL_METHOD(__method, __return_if_fail)                                 \
	QDBusInterface dbus_iface(m_szServiceName, "/Player",                             \
	    "org.freedesktop.MediaPlayer", QDBusConnection::sessionBus());                \
	QDBusMessage reply = dbus_iface.call(QDBus::Block, __method);                     \
	if(reply.type() == QDBusMessage::ErrorMessage)                                    \
	{                                                                                 \
		QDBusError err = reply;                                                       \
		qDebug("Error: %s\n%s\n", qPrintable(err.name()), qPrintable(err.message())); \
		return __return_if_fail;                                                      \
	}

#define MPRIS_GET_METADATA_FIELD(__field, __return_type, __return_if_fail) \
	if(this->status() != MpInterface::Playing)                             \
		return __return_if_fail;                                           \
	MPRIS_CALL_METHOD("GetMetadata", __return_if_fail)                     \
	foreach(QVariant w, reply.arguments())                                 \
	{                                                                      \
		QDBusArgument arg = qvariant_cast<QDBusArgument>(w);               \
		QVariant v = qdbus_cast<QVariantMap>(arg);                         \
		if(v.userType() == QVariant::Map)                                  \
		{                                                                  \
			const QVariantMap map = v.toMap();                             \
			QVariantMap::ConstIterator it = map.find(__field);             \
			if(it != map.end() && it.key() == __field)                     \
			{                                                              \
				return it.value().value<__return_type>();                  \
			}                                                              \
		}                                                                  \
	}                                                                      \
	return __return_if_fail;

#define MPRIS_CALL_METHOD_WITH_ARG(__method, __arg, __return_if_fail)                 \
	QDBusInterface dbus_iface(m_szServiceName, "/Player",                             \
	    "org.freedesktop.MediaPlayer", QDBusConnection::sessionBus());                \
	QDBusMessage reply = dbus_iface.call(QDBus::Block, __method, __arg);              \
	if(reply.type() == QDBusMessage::ErrorMessage)                                    \
	{                                                                                 \
		QDBusError err = reply;                                                       \
		qDebug("Error: %s\n%s\n", qPrintable(err.name()), qPrintable(err.message())); \
		return __return_if_fail;                                                      \
	}

MpInterface::PlayerStatus MpMprisInterface::status()
{
	MPRIS_CALL_METHOD("GetStatus", MpInterface::Unknown)

	MPRISPlayerStatus status;

	if(reply.arguments().isEmpty())
		return MpInterface::Unknown;

	status = qdbus_cast<MPRISPlayerStatus>(reply.arguments().first());

	switch(status.Play)
	{
		case 0:
			return MpInterface::Playing;
		case 1:
			return MpInterface::Paused;
		case 2:
			return MpInterface::Stopped;
		default:
			return MpInterface::Unknown;
	}
}

QString MpMprisInterface::nowPlaying()
{
	if(this->status() != MpInterface::Playing)
		return "";

	MPRIS_CALL_METHOD("GetMetadata", "")

	QString artist;
	QString title;
	foreach(QVariant w, reply.arguments())
	{
		QDBusArgument arg = qvariant_cast<QDBusArgument>(w);
		QVariant v = qdbus_cast<QVariantMap>(arg);
		if(v.userType() == QVariant::Map)
		{
			const QVariantMap map = v.toMap();
			QVariantMap::ConstIterator it = map.constBegin();
			for(; it != map.constEnd(); ++it)
			{ /* maybe do some configurable formatting */
				if(it.key() == "artist")
					artist = it.value().toString();
				else if(it.key() == "title")
					title = it.value().toString();
			}
		}
	}
	if(artist.length() && title.length())
		return artist + " - " + title;
	else
		return "";
}

QString MpMprisInterface::mrl()
{
	MPRIS_CALL_METHOD("GetMetadata", "")

	foreach(QVariant w, reply.arguments())
	{
		QDBusArgument arg = qvariant_cast<QDBusArgument>(w);
		QVariant v = qdbus_cast<QVariantMap>(arg);
		if(v.userType() == QVariant::Map)
		{
			const QVariantMap map = v.toMap();
			QVariantMap::ConstIterator it = map.find("location");
			if(it != map.end() && it.key() == "location")
			{
				return it.value().toString();
			}
		}
	}
	return "";
}

QString MpMprisInterface::title()
{
	MPRIS_GET_METADATA_FIELD("title", QString, "")
}

QString MpMprisInterface::artist()
{
	MPRIS_GET_METADATA_FIELD("artist", QString, "")
}

QString MpMprisInterface::genre()
{
	MPRIS_GET_METADATA_FIELD("genre", QString, "")
}

QString MpMprisInterface::comment()
{
	MPRIS_GET_METADATA_FIELD("comment", QString, "")
}

QString MpMprisInterface::year()
{
	MPRIS_GET_METADATA_FIELD("year", QString, "")
}

QString MpMprisInterface::album()
{
	MPRIS_GET_METADATA_FIELD("album", QString, "")
}

int MpMprisInterface::bitRate()
{
	MPRIS_GET_METADATA_FIELD("audio-bitrate", int, -1)
}

int MpMprisInterface::sampleRate()
{
	MPRIS_GET_METADATA_FIELD("audio-samplerate", int, -1)
}

bool MpMprisInterface::setVol(kvs_int_t & iVol)
{
	MPRIS_CALL_METHOD_WITH_ARG("VolumeSet", QVariant((int)(100 * iVol / 255)), false);
	return true;
}

int MpMprisInterface::getVol()
{
	MPRIS_CALL_METHOD("VolumeGet", -1)

	int iVol = reply.arguments().first().toInt();
	return iVol * 255 / 100;
}

int MpMprisInterface::position()
{
	MPRIS_CALL_METHOD("PositionGet", -1)
	return reply.arguments().first().toInt();
}

int MpMprisInterface::length()
{
	MPRIS_CALL_METHOD("GetMetadata", -1)

	foreach(QVariant w, reply.arguments())
	{
		QDBusArgument arg = qvariant_cast<QDBusArgument>(w);
		QVariant v = qdbus_cast<QVariantMap>(arg);
		if(v.userType() == QVariant::Map)
		{
			const QVariantMap map = v.toMap();
			QVariantMap::ConstIterator it = map.constBegin();
			for(; it != map.constEnd(); ++it)
			{
				if(it.key() == "mtime")
					return it.value().toInt();
			}
		}
	}
	return -1;
}

bool MpMprisInterface::jumpTo(kvs_int_t & iPos)
{
	MPRIS_CALL_METHOD_WITH_ARG("PositionSet", QVariant((int)iPos), false)
	return true;
}

/* audacious interface */
MP_IMPLEMENT_DESCRIPTOR(
    MpAudaciousInterface,
    "audacious",
    __tr2qs_ctx(
        "An interface for the Audacious media player.\n"
        "Download it from http://audacious-media-player.org\n",
        "mediaplayer"))

MpAudaciousInterface::MpAudaciousInterface()
    : MpMprisInterface()
{
	m_szServiceName = "org.mpris.audacious";
}

int MpAudaciousInterface::getPlayListPos()
{
	QDBusInterface dbus_iface("org.mpris.audacious", "/org/atheme/audacious",
	    "org.atheme.audacious", QDBusConnection::sessionBus());
	QDBusReply<uint> pos = dbus_iface.call(QDBus::Block, "Position");
	return pos;
}

bool MpAudaciousInterface::quit()
{
	if(MpMprisInterface::quit())
		return true;

	/* compatibility with older versions */
	MPRIS_SIMPLE_CALL("/Player", "Quit")
}

QString MpAudaciousInterface::mrl()
{
	MPRIS_CALL_METHOD("GetMetadata", "")

	foreach(QVariant w, reply.arguments())
	{
		QDBusArgument arg = qvariant_cast<QDBusArgument>(w);
		QVariant v = qdbus_cast<QVariantMap>(arg);
		if(v.userType() == QVariant::Map)
		{
			const QVariantMap map = v.toMap();
			QVariantMap::ConstIterator it = map.find("location");
			if(it != map.end() && it.key() == "location")
			{
				return it.value().toString();
			}
			/* Some audacious versions send URI instead of location */
			it = map.find("URI");
			if(it != map.end() && it.key() == "URI")
			{
				return it.value().toString();
			}
		}
	}
	return "";
}

MpInterface::PlayerStatus MpAudaciousInterface::status()
{
	MpInterface::PlayerStatus status;
	status = MpMprisInterface::status();
	if(status != MpInterface::Unknown)
		return status;

	/* compatibility with older versions */
	QDBusInterface dbus_iface(m_szServiceName, "/Player",
	    "org.freedesktop.MediaPlayer", QDBusConnection::sessionBus());
	if(!dbus_iface.isValid())
		return MpInterface::Unknown;

	QDBusMessage reply = dbus_iface.call(QDBus::Block, "GetStatus");

	switch(reply.arguments().first().toInt())
	{
		case 0:
			return MpInterface::Playing;
		case 1:
			return MpInterface::Paused;
		case 2:
			return MpInterface::Stopped;
		default:
			return MpInterface::Unknown;
	}
}

int MpAudaciousInterface::length()
{
	int length = MpMprisInterface::length();
	if(length != -1)
		return length;

	/* compatibility with older versions */
	MPRIS_CALL_METHOD("GetMetadata", -1)

	foreach(QVariant w, reply.arguments())
	{
		QDBusArgument arg = qvariant_cast<QDBusArgument>(w);
		QVariant v = qdbus_cast<QVariantMap>(arg);
		if(v.userType() == QVariant::Map)
		{
			const QVariantMap map = v.toMap();
			QVariantMap::ConstIterator it = map.constBegin();
			for(; it != map.constEnd(); ++it)
			{
				if(it.key() == "length")
					return it.value().toInt();
			}
		}
	}
	return -1;
}

#define AUDACIOUS_GET_TUPLE_FIELD(__field)                                                             \
	if(this->status() != MpInterface::Playing)                                                         \
		return "";                                                                                     \
	QDBusInterface dbus_iface("org.mpris.audacious", "/org/atheme/audacious",                          \
	    "org.atheme.audacious", QDBusConnection::sessionBus());                                        \
	QList<QVariant> args;                                                                              \
	args << (uint) this->getPlayListPos() << QString(__field);                                         \
	QDBusReply<QDBusVariant> reply = dbus_iface.callWithArgumentList(QDBus::Block, "SongTuple", args); \
	return reply.value().variant().toString();

QString MpAudaciousInterface::year()
{
	AUDACIOUS_GET_TUPLE_FIELD("year")
}

QString MpAudaciousInterface::mediaType()
{
	AUDACIOUS_GET_TUPLE_FIELD("codec")
}

/* BMPx interface */
MP_IMPLEMENT_DESCRIPTOR(
    MpBmpxInterface,
    "bmpx",
    __tr2qs_ctx(
        "An interface for BMPx.\n"
        "Download it from http://sourceforge.net/projects/beepmp\n",
        "mediaplayer"))

MpBmpxInterface::MpBmpxInterface()
    : MpMprisInterface()
{
	m_szServiceName = "org.mpris.bmp";
}

/* BMPx doesn't provide GetStatus method, always assume Playing status */
MpInterface::PlayerStatus MpBmpxInterface::status()
{
	return MpInterface::Playing;
}

/* Amarok2 interface */
MP_IMPLEMENT_DESCRIPTOR(
    MpAmarok2Interface,
    "amarok2",
    __tr2qs_ctx(
        "An interface for Amarok2.\n"
        "Download it from http://amarok.kde.org\n",
        "mediaplayer"))

MpAmarok2Interface::MpAmarok2Interface()
    : MpMprisInterface()
{
	m_szServiceName = "org.mpris.amarok";
}

/* Qmmp interface */
MP_IMPLEMENT_DESCRIPTOR(
    MpQmmpInterface,
    "Qmmp",
    __tr2qs_ctx(
        "An interface for Qmmp.\n"
        "Download it from http://qmmp.ylsoftware.com\n",
        "mediaplayer"))

MpQmmpInterface::MpQmmpInterface()
    : MpMprisInterface()
{
	m_szServiceName = "org.mpris.qmmp";
}

/* xmms2 interface */
MP_IMPLEMENT_DESCRIPTOR(
    MpXmms2Interface,
    "xmms2",
    __tr2qs_ctx(
        "An interface for the XMMS2 media player.\n"
        "Download it from http://xmms2.org\n",
        "mediaplayer"))

MpXmms2Interface::MpXmms2Interface()
    : MpMprisInterface()
{
	m_szServiceName = "org.mpris.xmms2";
}

/* mozilla songbird interface */
MP_IMPLEMENT_DESCRIPTOR(
    MpSongbirdInterface,
    "songbird",
    __tr2qs_ctx(
        "An interface for the Mozilla Songbird media player.\n"
        "Download it from http://www.getsongbird.com.\n"
        "To use it you have to install also the MPRIS addon "
        "available at http://addons.songbirdnest.com/addon/1626.\n",
        "mediaplayer"))

MpSongbirdInterface::MpSongbirdInterface()
    : MpMprisInterface()
{
	m_szServiceName = "org.mpris.songbird";
}

MpInterface::PlayerStatus MpSongbirdInterface::status()
{
	return MpInterface::Playing;
}

/* Totem interface */
MP_IMPLEMENT_DESCRIPTOR(
    MpTotemInterface,
    "totem",
    __tr2qs_ctx(
        "An interface for Totem.\n"
        "Download it from http://projects.gnome.org/totem/\n",
        "mediaplayer"))

MpTotemInterface::MpTotemInterface()
    : MpMprisInterface()
{
	m_szServiceName = "org.mpris.Totem";
}

/* Vlc interface */
MP_IMPLEMENT_DESCRIPTOR(
    MpVlcInterface,
    "vlc",
    __tr2qs_ctx(
        "An interface for VLC.\n"
        "Download it from http://www.videolan.org/\n"
        "You need to manually enable the D-Bus control\n"
        "interface in the VLC preferences\n",
        "mediaplayer"))

MpVlcInterface::MpVlcInterface()
    : MpMprisInterface()
{
	m_szServiceName = "org.mpris.vlc";
}

/* Clementine interface */
MP_IMPLEMENT_DESCRIPTOR(
    MpClementineInterface,
    "clementine",
    __tr2qs_ctx(
        "An interface for Clementine.\n"
        "Download it from http://www.clementine-player.org/\n",
        "mediaplayer"))

MpClementineInterface::MpClementineInterface()
    : MpMprisInterface()
{
	m_szServiceName = "org.mpris.clementine";
}
#endif //COMPILE_ON_WINDOWS
