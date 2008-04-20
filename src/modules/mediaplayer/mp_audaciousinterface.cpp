//=============================================================================
//
//   File : mp_audaciousinterface.cpp
//   Created on Thu 06 Dec 2007 14:20:02 by Tomasz Moń
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2007 Tomasz Moń <desowin@gmail.com>
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

#include "mp_audaciousinterface.h"
#if (!defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MAC))
#include "kvi_locale.h"

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
: KviMediaPlayerInterface()
{
}

KviAudaciousInterface::~KviAudaciousInterface()
{
}

int KviAudaciousInterface::detect(bool bStart)
{
	QDBusReply<QStringList> reply = QDBusConnection::sessionBus().interface()->registeredServiceNames();
	if (!reply.isValid())		/* something fishy with dbus, it won't work */
		return 0;

	foreach (QString name, reply.value())
		if (name == "org.mpris.audacious")	/* audacious is running */
			return 100;

	return 1;	/* dbus works, audacious may be closed */
}

#define AUDACIOUS_SIMPLE_CALL(__action) \
	QDBusInterface dbus_iface("org.mpris.audacious", "/Player", \
				"org.freedesktop.MediaPlayer", QDBusConnection::sessionBus()); \
	QDBusMessage reply = dbus_iface.call(QDBus::Block, __action); \
	if (reply.type() == QDBusMessage::ErrorMessage) { \
		QDBusError err = reply; \
		debug("Error: %s\n%s\n", qPrintable(err.name()), qPrintable(err.message())); \
		return FALSE; \
	} \
	return TRUE;

bool KviAudaciousInterface::prev()
{
	AUDACIOUS_SIMPLE_CALL("Prev")
}

bool KviAudaciousInterface::next()
{
	AUDACIOUS_SIMPLE_CALL("Next")
}

bool KviAudaciousInterface::play()
{
	AUDACIOUS_SIMPLE_CALL("Play")
}

bool KviAudaciousInterface::stop()
{
	AUDACIOUS_SIMPLE_CALL("Stop")
}

bool KviAudaciousInterface::pause()
{
	AUDACIOUS_SIMPLE_CALL("Pause")
}

bool KviAudaciousInterface::quit()
{
	AUDACIOUS_SIMPLE_CALL("Quit")
}

QString KviAudaciousInterface::nowPlaying()
{
	if (this->status() != KviMediaPlayerInterface::Playing)
		return "";

	QDBusInterface dbus_iface("org.mpris.audacious", "/Player",
				"org.freedesktop.MediaPlayer", QDBusConnection::sessionBus());
	QDBusMessage reply = dbus_iface.call(QDBus::Block, "GetMetadata");
	if (reply.type() == QDBusMessage::ErrorMessage) {
		QDBusError err = reply;
		debug("Error: %s\n%s\n", qPrintable(err.name()), qPrintable(err.message()));
		return "";
	}

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

QString KviAudaciousInterface::mrl()
{
	QDBusInterface dbus_iface("org.mpris.audacious", "/Player",
				"org.freedesktop.MediaPlayer", QDBusConnection::sessionBus());
	QDBusMessage reply = dbus_iface.call(QDBus::Block, "GetMetadata");
	if (reply.type() == QDBusMessage::ErrorMessage) {
		QDBusError err = reply;
		debug("Error: %s\n%s\n", qPrintable(err.name()), qPrintable(err.message()));
		return "";
	}

	foreach (QVariant v, reply.arguments()) {
		QDBusArgument arg = qvariant_cast<QDBusArgument>(v);
		QVariant v = qdbus_cast<QVariantMap>(arg);
		if (v.userType() == QVariant::Map) {
        		const QVariantMap map = v.toMap();
        		QVariantMap::ConstIterator it = map.constBegin();
        		for ( ; it != map.constEnd(); ++it) {
				if (it.key() == "URI")
					return it.value().toString();
			}
		}
	}
	return "";
}

KviMediaPlayerInterface::PlayerStatus KviAudaciousInterface::status()
{
	QDBusInterface dbus_iface("org.mpris.audacious", "/Player",
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
	QDBusInterface dbus_iface("org.mpris.audacious", "/Player",
				"org.freedesktop.MediaPlayer", QDBusConnection::sessionBus());
	QDBusMessage reply = dbus_iface.call(QDBus::Block, "GetMetadata");
	if (reply.type() == QDBusMessage::ErrorMessage) {
		QDBusError err = reply;
		debug("Error: %s\n%s\n", qPrintable(err.name()), qPrintable(err.message()));
		return -1;
	}

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

#define AUDACIOUS_GET_METADATA_FIELD(__field) \
	if (this->status() != KviMediaPlayerInterface::Playing) \
		return ""; \
	QDBusInterface dbus_iface("org.mpris.audacious", "/Player", \
				"org.freedesktop.MediaPlayer", QDBusConnection::sessionBus()); \
	QDBusMessage reply = dbus_iface.call(QDBus::Block, "GetMetadata"); \
	if (reply.type() == QDBusMessage::ErrorMessage) { \
		QDBusError err = reply; \
		debug("Error: %s\n%s\n", qPrintable(err.name()), qPrintable(err.message())); \
		return ""; \
	} \
	foreach (QVariant v, reply.arguments()) { \
		QDBusArgument arg = qvariant_cast<QDBusArgument>(v); \
		QVariant v = qdbus_cast<QVariantMap>(arg); \
		if (v.userType() == QVariant::Map) { \
        		const QVariantMap map = v.toMap(); \
        		QVariantMap::ConstIterator it = map.constBegin(); \
        		for ( ; it != map.constEnd(); ++it) { \
				if (it.key() == __field) \
					return it.value().toString(); \
			} \
		} \
	} \
	return "";

QString KviAudaciousInterface::title()
{
	AUDACIOUS_GET_METADATA_FIELD("title")
}

QString KviAudaciousInterface::artist()
{
	AUDACIOUS_GET_METADATA_FIELD("artist")
}

QString KviAudaciousInterface::genre()
{
	AUDACIOUS_GET_METADATA_FIELD("genre")
}

QString KviAudaciousInterface::comment()
{
	AUDACIOUS_GET_METADATA_FIELD("comment")
}

QString KviAudaciousInterface::album()
{
	AUDACIOUS_GET_METADATA_FIELD("album")
}

/* audacious specific interface */
int KviAudaciousInterface::getPlayListPos()
{
	QDBusInterface dbus_iface("org.mpris.audacious", "/org/atheme/audacious",
				"org.atheme.audacious", QDBusConnection::sessionBus());
	QDBusReply<uint> pos = dbus_iface.call(QDBus::Block, "Position");
	return pos;
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

#endif //!COMPILE_ON_WINDOWS
