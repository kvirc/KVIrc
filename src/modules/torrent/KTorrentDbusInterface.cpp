//=============================================================================
//
//   DBus interface for KTorrent client.
//
//   File : KTorrentDbusInterface.cpp
//   Creation date : Mon Nov 17 13:46:00 2008 GMT by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Fabio Bas (ctrlaltca at gmail dot com)
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

#include "KTorrentDbusInterface.h"

#ifdef COMPILE_KDE_SUPPORT

#include "KviApplication.h"
#include "KviLocale.h"

#include <QTimer>

TORR_IMPLEMENT_DESCRIPTOR(
    KTorrentDbusInterface,
    "ktorrentdbus",
    __tr2qs_ctx(
        "An interface for KDE's KTorrent client.\n"
        "Download it from http://www.ktorrent.org\n",
        "torrent"))

KTorrentDbusInterface::KTorrentDbusInterface()
{
	// make sure we have a list of files,
	// otherwise functions dealing would
	// fail if called the first time
	// 		slotTimer();
}

KTorrentDbusInterface::~KTorrentDbusInterface()
    = default;

bool KTorrentDbusInterface::findRunningApp()
{
	QDBusReply<QStringList> reply = QDBusConnection::sessionBus().interface()->registeredServiceNames();
	if(!reply.isValid()) /* something fishy with dbus, it won't work */
		return false;

	foreach(QString name, reply.value())
		if(name == "org.ktorrent.ktorrent") /* ktorrent is running */
			return true;

	return false; /* dbus works, ktorrent may be closed */
}

int KTorrentDbusInterface::detect()
{
	QDBusReply<QStringList> reply = QDBusConnection::sessionBus().interface()->registeredServiceNames();
	if(!reply.isValid()) /* something fishy with dbus, it won't work */
		return 0;

	foreach(QString name, reply.value())
		if(name == "org.ktorrent.ktorrent") /* ktorrent is running */
			return 100;

	return 1; /* dbus works, ktorrent may be closed */
}

#define KTDBUS_SIMPLE_CALL(__action)                                                  \
	QDBusInterface dbus_iface("org.ktorrent.ktorrent", "/core",                       \
	    "org.ktorrent.KTorrent", QDBusConnection::sessionBus());                      \
	QDBusMessage reply = dbus_iface.call(QDBus::Block, __action);                     \
	if(reply.type() == QDBusMessage::ErrorMessage)                                    \
	{                                                                                 \
		QDBusError err = reply;                                                       \
		qDebug("Error: %s\n%s\n", qPrintable(err.name()), qPrintable(err.message())); \
		return false;                                                                 \
	}                                                                                 \
	return true;

bool KTorrentDbusInterface::startAll()
{
	KTDBUS_SIMPLE_CALL("startAll");
	return true;
}

bool KTorrentDbusInterface::stopAll()
{
	KTDBUS_SIMPLE_CALL("stopAll");
	return true;
}

bool KTorrentDbusInterface::start(int)
{
	return true;
}

bool KTorrentDbusInterface::stop(int)
{
	return true;
}

bool KTorrentDbusInterface::announce(int)
{
	return true;
}

QString KTorrentDbusInterface::state(int)
{
	return QString();
}

QString KTorrentDbusInterface::name(int)
{
	return QString();
}

int KTorrentDbusInterface::fileCount(int)
{
	return 0;
}

QString KTorrentDbusInterface::fileName(int, int)
{
	return QString();
}

QString KTorrentDbusInterface::filePriority(int, int)
{
	return QString();
}

bool KTorrentDbusInterface::setFilePriority(int, int, const QString &)
{
	return true;
}

int KTorrentDbusInterface::count()
{
	return 0;
	// 		return m_ti.size();
}

float KTorrentDbusInterface::speedUp()
{
	return 0.0;
}

float KTorrentDbusInterface::speedDown()
{
	return 0.0;
}

float KTorrentDbusInterface::trafficUp()
{
	return 0.0;
}

float KTorrentDbusInterface::trafficDown()
{
	return 0.0;
}

int KTorrentDbusInterface::maxUploadSpeed()
{
	return -1;
}

int KTorrentDbusInterface::maxDownloadSpeed()
{
	return -1;
}

bool KTorrentDbusInterface::setMaxUploadSpeed(int)
{
	return true;
}

bool KTorrentDbusInterface::setMaxDownloadSpeed(int)
{
	return true;
}

#endif //	COMPILE_KDE_SUPPORT
