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

MpMprisInterface::MpMprisInterface()
    : MpInterface()
{
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

#define MPRIS_SIMPLE_CALL(_path, _action)                                           \
	QDBusInterface dbus_iface(m_szServiceName, "/org/mpris/MediaPlayer2",             \
	    _path, QDBusConnection::sessionBus());                                       \
	QDBusMessage reply = dbus_iface.call(QDBus::Block, _action);                     \
	if(reply.type() == QDBusMessage::ErrorMessage)                                    \
	{                                                                                 \
		QDBusError err = reply;                                                       \
		qDebug("Error: %s\n%s\n", qPrintable(err.name()), qPrintable(err.message())); \
		return false;                                                                 \
	}                                                                                 \
	return true;

bool MpMprisInterface::prev()
{
	MPRIS_SIMPLE_CALL("org.mpris.MediaPlayer2.Player", "Previous")
}

bool MpMprisInterface::next()
{
	MPRIS_SIMPLE_CALL("org.mpris.MediaPlayer2.Player", "Next")
}

bool MpMprisInterface::play()
{
	MPRIS_SIMPLE_CALL("org.mpris.MediaPlayer2.Player", "Play")
}

bool MpMprisInterface::stop()
{
	MPRIS_SIMPLE_CALL("org.mpris.MediaPlayer2.Player", "Stop")
}

bool MpMprisInterface::pause()
{
	MPRIS_SIMPLE_CALL("org.mpris.MediaPlayer2.Player", "Pause")
}

bool MpMprisInterface::quit()
{
	MPRIS_SIMPLE_CALL("org.mpris.MediaPlayer2", "Quit")
}

#define MPRIS_GET_PROPERTY(_property, _return_if_fail)                  \
	QDBusInterface dbus_iface(m_szServiceName, "/org/mpris/MediaPlayer2", \
	    "org.mpris.MediaPlayer2.Player", QDBusConnection::sessionBus());  \
	QVariant reply = dbus_iface.property(_property);                     \
	if(!reply.isValid())                                                  \
		return _return_if_fail;

#define MPRIS_GET_METADATA_FIELD(_field, _return_type, _return_if_fail) \
	if(this->status() != MpInterface::Playing)                             \
		return _return_if_fail;                                           \
	MPRIS_GET_PROPERTY("Metadata", _return_if_fail)                       \
	QVariantMap map = reply.toMap();                                       \
	foreach (const QString &key, map.keys())                               \
	{                                                                      \
		if(key == _field)                                                 \
			return map.value(key).value<_return_type>();                  \
	}                                                                      \
	return _return_if_fail;

#define MPRIS_SET_PROPERTY(_property, _arg, _return_if_fail)           \
	QDBusInterface dbus_iface(m_szServiceName, "/org/mpris/MediaPlayer2", \
	    "org.mpris.MediaPlayer2.Player", QDBusConnection::sessionBus());  \
	if(false == dbus_iface.setProperty(_property, _arg))                \
		return _return_if_fail;

#define MPRIS_CALL_METHOD_WITH_TWO_ARG(_method, _arg1, _arg2, _return_if_fail)    \
	QDBusInterface dbus_iface(m_szServiceName, "/org/mpris/MediaPlayer2",             \
	    "org.mpris.MediaPlayer2.Player", QDBusConnection::sessionBus());              \
	QDBusMessage reply = dbus_iface.call(QDBus::Block, _method, _arg1, _arg2);     \
	if(reply.type() == QDBusMessage::ErrorMessage)                                    \
	{                                                                                 \
		QDBusError err = reply;                                                       \
		qDebug("Error: %s\n%s\n", qPrintable(err.name()), qPrintable(err.message())); \
		return _return_if_fail;                                                      \
	}

MpInterface::PlayerStatus MpMprisInterface::status()
{
	MPRIS_GET_PROPERTY("PlaybackStatus", MpInterface::Unknown)

	QString szStatus = reply.toString();
	if(szStatus == "Playing")
		return MpInterface::Playing;
	if(szStatus == "Paused")
		return MpInterface::Paused;
	if(szStatus == "Stopped")
		return MpInterface::Stopped;
	return MpInterface::Unknown;
}

QString MpMprisInterface::nowPlaying()
{
	if(this->status() != MpInterface::Playing)
		return "";

	MPRIS_GET_PROPERTY("Metadata", "")
	QVariantMap map = reply.toMap();

	QString artist;
	QString title;
	foreach (const QString &key, map.keys())
	{
		/* maybe do some configurable formatting */
		if(key == "xesam:artist")
			artist = map.value(key).toString();
		else if(key == "xesam:title")
			title = map.value(key).toString();
	}

	if(artist.length() && title.length())
		return artist + " - " + title;
	else
		return "";
}

QString MpMprisInterface::mrl()
{
	MPRIS_GET_METADATA_FIELD("xesam:url", QString, "")
}

QString MpMprisInterface::title()
{
	MPRIS_GET_METADATA_FIELD("xesam:title", QString, "")
}

QString MpMprisInterface::artist()
{
	MPRIS_GET_METADATA_FIELD("xesam:artist", QString, "")
}

QString MpMprisInterface::genre()
{
	MPRIS_GET_METADATA_FIELD("xesam:genre", QString, "")
}

QString MpMprisInterface::comment()
{
	MPRIS_GET_METADATA_FIELD("xesam:comment", QString, "")
}

QString MpMprisInterface::year()
{
	MPRIS_GET_METADATA_FIELD("xesam:contentCreated", QString, "")
}

QString MpMprisInterface::album()
{
	MPRIS_GET_METADATA_FIELD("xesam:album", QString, "")
}

int MpMprisInterface::bitRate()
{
	// not-standard
	MPRIS_GET_METADATA_FIELD("audio-bitrate", int, -1)
}

int MpMprisInterface::sampleRate()
{
	// not-standard
	MPRIS_GET_METADATA_FIELD("audio-samplerate", int, -1)
}

bool MpMprisInterface::setVol(kvs_int_t & iVol)
{
	double dVol = iVol;
	MPRIS_SET_PROPERTY("Volume", QVariant(dVol / 255), false);
	return true;
}

int MpMprisInterface::getVol()
{
	MPRIS_GET_PROPERTY("Volume", -1)
	return reply.toDouble() * 255;
}

int MpMprisInterface::position()
{
	MPRIS_GET_PROPERTY("Position", -1)
	// from usecs to msecs
	return (int) reply.toLongLong() / 1000;
}

int MpMprisInterface::length()
{
	if(this->status() != MpInterface::Playing)
		return -1;

	MPRIS_GET_PROPERTY("Metadata", -1)
	QVariantMap map = reply.toMap();
	foreach (const QString &key, map.keys())
	{
		// from usecs to msecs
		if(key == "mpris:length")
			return (int) map.value(key).toLongLong() / 1000;
	}

	return -1;
}

QVariant MpMprisInterface::getTrackId()
{
	if(this->status() != MpInterface::Playing)
		return QVariant();

	MPRIS_GET_PROPERTY("Metadata", QVariant())
	QVariantMap map = reply.toMap();

	return map.value("mpris:trackid");
}

bool MpMprisInterface::jumpTo(kvs_int_t & iPos)
{
	QVariant oTrackId = getTrackId();
	if(!oTrackId.isValid())
		return false;

	qlonglong llPos = iPos;
	// from usecs to msecs
	MPRIS_CALL_METHOD_WITH_TWO_ARG("SetPosition", oTrackId, QVariant(llPos * 1000), false)
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
	m_szServiceName = "org.mpris.MediaPlayer2.audacious";
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
	m_szServiceName = "org.mpris.MediaPlayer2.amarok";
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
	m_szServiceName = "org.mpris.MediaPlayer2.qmmp";
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
	m_szServiceName = "org.mpris.MediaPlayer2.xmms2";
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
	m_szServiceName = "org.mpris.MediaPlayer2.Totem";
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
	m_szServiceName = "org.mpris.MediaPlayer2.vlc";
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
	m_szServiceName = "org.mpris.MediaPlayer2.clementine";
}

/* Strawberry interface */
MP_IMPLEMENT_DESCRIPTOR(
    MpStrawberryInterface,
    "strawberry",
    __tr2qs_ctx(
        "An interface for Strawberry.\n"
        "Download it from https://www.strawberrymusicplayer.org/\n",
        "mediaplayer"))

MpStrawberryInterface::MpStrawberryInterface()
    : MpMprisInterface()
{
	m_szServiceName = "org.mpris.MediaPlayer2.strawberry";
}

/* Generic Mpris interface */
MP_IMPLEMENT_DESCRIPTOR(
    MpMprisGenericInterface,
    "generic mpris 2",
    __tr2qs_ctx(
        "An generic interface for MPRIS 2 media players.",
        "mediaplayer"))

MpMprisGenericInterface::MpMprisGenericInterface()
    : MpMprisInterface()
{
}

int MpMprisGenericInterface::detect(bool)
{
	QDBusReply<QStringList> reply = QDBusConnection::sessionBus().interface()->registeredServiceNames();
	if(!reply.isValid()) /* something fishy with dbus, it won't work */
		return 0;

	foreach(QString name, reply.value())
		if(name.startsWith("org.mpris.MediaPlayer2.")) {
			m_szServiceName = name;
			// prefer the player-specific interfaces
			return 99;
		}

	return 1; /* dbus works, player may be closed */
}
#endif //COMPILE_ON_WINDOWS
