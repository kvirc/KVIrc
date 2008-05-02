//=============================================================================
//
//   File : mp_audaciousinterface.cpp
//   Created on Thu 06 Dec 2007 14:20:02 by Tomasz Moń
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Elvio Basello <hellvis69 at netsons dot org>
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

#include "mp_xmms2interface.h"
#if (!defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MAC))
#include "kvi_locale.h"

MP_IMPLEMENT_DESCRIPTOR(
	KviXmms2Interface,
	"audacious",
	__tr2qs_ctx(
		"An interface to the popular xmms2 media player.\n" \
		"Download it from http://wiki.xmms2.xmms.se/index.php/Main_Page\n",
		"mediaplayer"
	)
)

KviXmms2Interface::KviXmms2Interface()
: KviMediaPlayerInterface()
{
}

KviXmms2Interface::~KviXmms2Interface()
{
}

int KviXmms2Interface::detect(bool bStart)
{
	QDBusReply<QStringList> reply = QDBusConnection::sessionBus().interface()->registeredServiceNames();
	if (!reply.isValid())		/* something fishy with dbus, it won't work */
		return 0;

	foreach (QString name, reply.value())
		if (name == "org.mpris.xmms2")	/* audacious is running */
			return 100;

	return 1;	/* dbus works, audacious may be closed */
}

#define XMMS2_SIMPLE_CALL(__action) \
	QDBusInterface dbus_iface("org.mpris.xmms2", "/Player", \
				"org.freedesktop.MediaPlayer", QDBusConnection::sessionBus()); \
	QDBusMessage reply = dbus_iface.call(QDBus::Block, __action); \
	if (reply.type() == QDBusMessage::ErrorMessage) { \
		QDBusError err = reply; \
		debug("Error: %s\n%s\n", qPrintable(err.name()), qPrintable(err.message())); \
		return FALSE; \
	} \
	return TRUE;

bool KviXmms2Interface::prev()
{
	XMMS2_SIMPLE_CALL("Prev")
}

bool KviXmms2Interface::next()
{
	XMMS2_SIMPLE_CALL("Next")
}

bool KviXmms2Interface::play()
{
	XMMS2_SIMPLE_CALL("Play")
}

bool KviXmms2Interface::stop()
{
	XMMS2_SIMPLE_CALL("Stop")
}

bool KviXmms2Interface::pause()
{
	XMMS2_SIMPLE_CALL("Pause")
}

bool KviXmms2Interface::quit()
{
	XMMS2_SIMPLE_CALL("Quit")
}

#define XMMS2_CALL_METHOD(__method, __return_if_fail) \
	QDBusInterface dbus_iface("org.mpris.xmms2", "/Player", \
				"org.freedesktop.MediaPlayer", QDBusConnection::sessionBus()); \
	QDBusMessage reply = dbus_iface.call(QDBus::Block, __method); \
	if (reply.type() == QDBusMessage::ErrorMessage) { \
		QDBusError err = reply; \
		debug("Error: %s\n%s\n", qPrintable(err.name()), qPrintable(err.message())); \
		return __return_if_fail; \
	}

#define XMMS2_CALL_METHOD_WITH_ARG(__method, __arg, __return_if_fail) \
        QDBusInterface dbus_iface("org.mpris.xmms2", "/Player", \
                                "org.freedesktop.MediaPlayer", QDBusConnection::sessionBus()); \
        QDBusMessage reply = dbus_iface.call(QDBus::Block, __method, __arg); \
        if (reply.type() == QDBusMessage::ErrorMessage) { \
                QDBusError err = reply; \
                debug("Error: %s\n%s\n", qPrintable(err.name()), qPrintable(err.message())); \
                return __return_if_fail; \
        }

QString KviXmms2Interface::nowPlaying()
{
	if (this->status() != KviMediaPlayerInterface::Playing)
		return "";

	XMMS2_CALL_METHOD("GetMetadata", "")

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

QString KviXmms2Interface::mrl()
{
	XMMS2_CALL_METHOD("GetMetadata", "")

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

bool KviXmms2Interface::setVol(kvs_int_t &iVol)
{
	XMMS2_CALL_METHOD_WITH_ARG("VolumeSet", QVariant((int)(100*iVol/255)), false);
	return true;
}

int KviXmms2Interface::getVol()
{
	XMMS2_CALL_METHOD("VolumeGet", -1)

	int iVol = reply.arguments().first().toInt();
	return iVol * 255 /100;
}

KviMediaPlayerInterface::PlayerStatus KviXmms2Interface::status()
{
	QDBusInterface dbus_iface("org.mpris.xmms2", "/Player",
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

int KviXmms2Interface::position()
{
	XMMS2_CALL_METHOD("PositionGet", -1)
	return reply.arguments().first().toInt();
}

int KviXmms2Interface::length()
{
	XMMS2_CALL_METHOD("GetMetadata", -1)

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

bool KviXmms2Interface::jumpTo(int &iPos)
{
	XMMS2_CALL_METHOD_WITH_ARG("PositionSet", QVariant(iPos), false)
	return true;
}

#define XMMS2_GET_METADATA_FIELD(__field) \
	if (this->status() != KviMediaPlayerInterface::Playing) \
		return ""; \
	QDBusInterface dbus_iface("org.mpris.xmms2", "/Player", \
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

QString KviXmms2Interface::title()
{
	XMMS2_GET_METADATA_FIELD("title")
}

QString KviXmms2Interface::artist()
{
	XMMS2_GET_METADATA_FIELD("artist")
}

QString KviXmms2Interface::genre()
{
	XMMS2_GET_METADATA_FIELD("genre")
}

QString KviXmms2Interface::comment()
{
	XMMS2_GET_METADATA_FIELD("comment")
}

QString KviXmms2Interface::album()
{
	XMMS2_GET_METADATA_FIELD("album")
}

/* audacious specific interface */
int KviXmms2Interface::getPlayListPos()
{
	QDBusInterface dbus_iface("org.mpris.xmms2", "/org/atheme/audacious",
				"org.atheme.audacious", QDBusConnection::sessionBus());
	QDBusReply<uint> pos = dbus_iface.call(QDBus::Block, "Position");
	return pos;
}

#define XMMS2_GET_TUPLE_FIELD(__field) \
	if (this->status() != KviMediaPlayerInterface::Playing) \
		return ""; \
	QDBusInterface dbus_iface("org.mpris.xmms2", "/org/atheme/audacious", \
				"org.atheme.audacious", QDBusConnection::sessionBus()); \
	QList<QVariant> args; \
	args << (uint)this->getPlayListPos() << QString(__field); \
	QDBusReply<QDBusVariant> reply = dbus_iface.callWithArgumentList(QDBus::Block, "SongTuple", args); \
	return reply.value().variant().toString(); \

QString KviXmms2Interface::year()
{
	XMMS2_GET_TUPLE_FIELD("year")
}

QString KviXmms2Interface::mediaType()
{
	XMMS2_GET_TUPLE_FIELD("codec")
}

#endif //!COMPILE_ON_WINDOWS
