//=============================================================================
//
//   File : mp_mprisinterface.cpp
//   Created on Thu 06 Dec 2007 14:20:02 by Tomasz Moń
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2007-2008 Tomasz Moń <desowin@gmail.com>
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

#include "mp_mprisinterface.h"
#if (!defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MAC) && !defined(COMPILE_ON_MINGW))
#include "kvi_locale.h"

KviMPRISInterface::KviMPRISInterface()
: KviMediaPlayerInterface()
{
}

KviMPRISInterface::~KviMPRISInterface()
{
}

int KviMPRISInterface::detect(bool bStart)
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

#define MPRIS_GET_METADATA_FIELD(__field) \
	if (this->status() != KviMediaPlayerInterface::Playing) \
		return ""; \
	MPRIS_CALL_METHOD("GetMetadata", "") \
	foreach (QVariant v, reply.arguments()) { \
		QDBusArgument arg = qvariant_cast<QDBusArgument>(v); \
		QVariant v = qdbus_cast<QVariantMap>(arg); \
		if (v.userType() == QVariant::Map) { \
        		const QVariantMap map = v.toMap(); \
        		QVariantMap::ConstIterator it = map.find(__field); \
                        if (it != map.end() && it.key() == __field) { \
                                return it.value().toString(); \
                        } \
		} \
	} \
	return "";

#define MPRIS_CALL_METHOD_WITH_ARG(__method, __arg, __return_if_fail) \
        QDBusInterface dbus_iface(m_szServiceName, "/Player", \
                                "org.freedesktop.MediaPlayer", QDBusConnection::sessionBus()); \
        QDBusMessage reply = dbus_iface.call(QDBus::Block, __method, __arg); \
        if (reply.type() == QDBusMessage::ErrorMessage) { \
                QDBusError err = reply; \
                debug("Error: %s\n%s\n", qPrintable(err.name()), qPrintable(err.message())); \
                return __return_if_fail; \
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
	MPRIS_GET_METADATA_FIELD("title")
}

QString KviMPRISInterface::artist()
{
	MPRIS_GET_METADATA_FIELD("artist")
}

QString KviMPRISInterface::genre()
{
	MPRIS_GET_METADATA_FIELD("genre")
}

QString KviMPRISInterface::comment()
{
	MPRIS_GET_METADATA_FIELD("comment")
}

QString KviMPRISInterface::album()
{
	MPRIS_GET_METADATA_FIELD("album")
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
				if (it.key() == "time")
					return it.value().toInt();
			}
		}
	}
	return -1;
}

bool KviMPRISInterface::jumpTo(int &iPos)
{
	MPRIS_CALL_METHOD_WITH_ARG("PositionSet", QVariant(iPos), false)
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
		"Download it from http://bmpx.backtrace.info\n"
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

#endif //!COMPILE_ON_WINDOWS
