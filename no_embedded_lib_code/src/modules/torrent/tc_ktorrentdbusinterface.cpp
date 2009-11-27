//=============================================================================
//
//   DBus interface for KTorrent client.
//
//   File : tc_ktorrentdbusinterface.h
//   Creation date : Mod Nov 17 13:46:00 2008 GMT by Fabio Bas
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2008 Fabio Bas (ctrlaltca at gmail dot com)
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

#include "tc_ktorrentdbusinterface.h"

#ifdef COMPILE_KDE_SUPPORT

	#include "kvi_app.h"
	#include "kvi_locale.h"

	#include <QTimer>

	TORR_IMPLEMENT_DESCRIPTOR(
		KviKTorrentDBusInterface,
		"ktorrentdbus",
		__tr2qs_ctx(
			"An interface to KDE's excellent KTorrent client.\n" \
			"Download it from http://www.ktorrent.org\n",
			"torrent"
		)
	)

	KviKTorrentDBusInterface::KviKTorrentDBusInterface()
	{
// 		printf("KviKTorrentDBusInterface\n");

// 		QTimer *timer = new QTimer(this);
// 		connect(timer, SIGNAL(timeout()), this, SLOT(slotTimer()));
// 		timer->start(250, FALSE);

		// make sure we have a list of files,
		// otherwise functions dealing would
		// fail if called the first time
// 		slotTimer();

	}

	KviKTorrentDBusInterface::~KviKTorrentDBusInterface()
	{
// 		printf("~KviKTorrentDBusInterface\n");
	}

	bool KviKTorrentDBusInterface::findRunningApp()
	{
		QDBusReply<QStringList> reply = QDBusConnection::sessionBus().interface()->registeredServiceNames();
		if (!reply.isValid())		/* something fishy with dbus, it won't work */
			return false;

		foreach (QString name, reply.value())
			if (name == "org.ktorrent.ktorrent")	/* ktorrent is running */
				return true;

		return false;	/* dbus works, ktorrent may be closed */
	}

	int KviKTorrentDBusInterface::detect()
	{
		QDBusReply<QStringList> reply = QDBusConnection::sessionBus().interface()->registeredServiceNames();
		if (!reply.isValid())		/* something fishy with dbus, it won't work */
			return 0;

		foreach (QString name, reply.value())
			if (name == "org.ktorrent.ktorrent")	/* ktorrent is running */
				return 100;

		return 1;	/* dbus works, ktorrent may be closed */
	}

#define KTDBUS_SIMPLE_CALL(__action) \
	QDBusInterface dbus_iface("org.ktorrent.ktorrent", "/core", \
				"org.ktorrent.KTorrent", QDBusConnection::sessionBus()); \
	QDBusMessage reply = dbus_iface.call(QDBus::Block, __action); \
	if (reply.type() == QDBusMessage::ErrorMessage) { \
		QDBusError err = reply; \
		debug("Error: %s\n%s\n", qPrintable(err.name()), qPrintable(err.message())); \
		return FALSE; \
	} \
	return TRUE;

	bool KviKTorrentDBusInterface::startAll()
	{
		KTDBUS_SIMPLE_CALL("startAll");
		return true;
	}

	bool KviKTorrentDBusInterface::stopAll()
	{
		KTDBUS_SIMPLE_CALL("stopAll");
		return true;
	}

/*
	void KviKTorrentDBusInterface::slotTimer()
	{
		if (!findRunningApp())
			return;

		m_ti.clear();

		QList<int> ret;
		if (!qvalueListIntRetIntDCOPCall("KTorrent", "getTorrentNumbers(int)", ret, 0))
			ERROR_RET_VOID

		for (int i=0; i<ret.size(); i++)
		{
			KviQCStringList info;
			if (!qcstringListRetIntDCOPCall("KTorrent", "getTorrentInfo(int)", info, ret[i]))
				ERROR_RET_VOID

			if (info.size() == 0)
				continue;

			TorrentInfo item;
			if (!makeTorrentInfo(item, info))
				return;

			item.num = ret[i];
			m_ti.append(item);
		}

		qHeapSort(m_ti);
	}

	bool makeSize(float &sz, const QString &s, const QString &u)
	{
		bool ok;
		sz = s.toFloat(&ok);
		if (!ok)
			return false;

		if (u == "B")
			;
		else
		if (u == "KiB")
			sz = sz * 1024.0;
		else
		if (u == "MiB")
			sz = sz * 1024.0 * 1024.0;
		else
		if (u == "GiB")
			sz = sz * 1024.0 * 1024.0 * 1024.0;
		else
			return false;

		return true;
	}
*/
	bool KviKTorrentDBusInterface::start(int )
	{
/*
		CHECK_RANGE_BOOL(i, m_ti.size())

		debug("starting %s [%d]", (const char*)m_ti[i].name, m_ti[i].num);
		if (!voidRetIntDCOPCall("KTorrent", "start(int)", m_ti[i].num))
			ERROR_RET_BOOL
*/
		return true;
	}

	bool KviKTorrentDBusInterface::stop(int )
	{
/*
		CHECK_RANGE_BOOL(i, m_ti.size())

		debug("stopping %s [%d]", (const char*)m_ti[i].name, m_ti[i].num);
		if (!voidRetIntBoolDCOPCall("KTorrent", "stop(int, bool)", m_ti[i].num, true))
			ERROR_RET_BOOL
*/
		return true;
	}

	bool KviKTorrentDBusInterface::announce(int )
	{
/*
		CHECK_RANGE_BOOL(i, m_ti.size())

		debug("announcing %s [%d]", (const char*)m_ti[i].name, m_ti[i].num);
		if (!voidRetIntDCOPCall("KTorrent", "announce(int)", m_ti[i].num))
			ERROR_RET_BOOL
*/
		return true;
	}

	QString KviKTorrentDBusInterface::state(int )
	{
/*
		CHECK_RANGE_STRING(i, m_ti.size())

		return m_ti[i].state;
*/
	return QString();
	}

	QString KviKTorrentDBusInterface::name(int )
	{
	/*
		CHECK_RANGE_STRING(i, m_ti.size())

		return m_ti[i].name;
	*/
	return QString();
	}

	int KviKTorrentDBusInterface::fileCount(int )
	{
	/*
		CHECK_RANGE_INT(i, m_ti.size())

		int ret;
		if (!intRetIntDCOPCall("KTorrent", "getFileCount(int)", ret, m_ti[i].num))
			ERROR_RET_NUM

		return ret;
	*/
	return 0;
	}

	QString KviKTorrentDBusInterface::fileName(int , int )
	{
	/*
		CHECK_RANGE_STRING(i, m_ti.size())

		QCStringList ret;
		if (!qcstringListRetIntDCOPCall("KTorrent", "getFileNames(int)", ret, m_ti[i].num))
			ERROR_RET_STRING

		CHECK_RANGE_STRING(file, ret.size())

		return ret[file];
	*/
	return QString();
	}

	QString KviKTorrentDBusInterface::filePriority(int , int )
	{
	/*
		CHECK_RANGE_STRING(i, m_ti.size())

		QValueList<int> ret;
		if (!qvalueListIntRetIntDCOPCall("KTorrent", "getFilePriorities(int)",ret, m_ti[i].num))
			ERROR_RET_STRING

		CHECK_RANGE_STRING(file, ret.size())

		debug("prio: %d", ret[file]);
		switch (ret[file])
		{
			case 1: return "low";
			case 2: return "normal";
			case 3: return "high";
		}

		ERROR_RET_STRING
	*/
	return QString();
	}

	bool KviKTorrentDBusInterface::setFilePriority(int , int , const QString &)
	{
	/*
		CHECK_RANGE_BOOL(i, m_ti.size())

		int prion;
		if (prio == "low")
			prion = 1;
		else
		if (prio == "normal")
			prion = 2;
		else
		if (prio == "high")
			prion = 1;
		else
			ERROR_RET_BOOL

		if (!voidRetIntIntIntDCOPCall("KTorrent", "setFilePriority(int,int,int)", m_ti[i].num, file, prion))
			ERROR_RET_BOOL
*/
		return true;
	}

	int KviKTorrentDBusInterface::count()
	{
		return 0;
// 		return m_ti.size();
	}

	float KviKTorrentDBusInterface::speedUp()
	{
	/*
		KviQCStringList ret;
		if (!qcstringListRetVoidDCOPCall("KTorrent", "getInfo()", ret))
			ERROR_RET_NUM

		QStringList tmp = QStringList::split(" ", ret[2]);
		if (tmp.size() != 8)
			ERROR_RET_NUM

		bool ok;
		float f = tmp[6].toFloat(&ok);
		if (!ok)
			ERROR_RET_NUM

		return f;
	*/
	return 0.0;
	}

	float KviKTorrentDBusInterface::speedDown()
	{
	/*
		KviQCStringList ret;
		if (!qcstringListRetVoidDCOPCall("KTorrent", "getInfo()", ret))
			ERROR_RET_NUM

		QStringList tmp = QStringList::split(" ", ret[2]);
		if (tmp.size() != 8)
			ERROR_RET_NUM

		bool ok;
		float f = tmp[2].toFloat(&ok);
		if (!ok)
			ERROR_RET_NUM

		return f;
	*/
	return 0.0;
	}

	float KviKTorrentDBusInterface::trafficUp()
	{
	/*
		KviQCStringList ret;
		if (!qcstringListRetVoidDCOPCall("KTorrent", "getInfo()", ret))
			ERROR_RET_NUM

		QStringList tmp = QStringList::split(" ", ret[1]);
		if (tmp.size() != 8)
			ERROR_RET_NUM

		float f;
		if (!makeSize(f, tmp[6], tmp[7]))
			ERROR_RET_NUM

		return f;
	*/
	return 0.0;
	}

	float KviKTorrentDBusInterface::trafficDown()
	{
	/*
		KviQCStringList ret;
		if (!qcstringListRetVoidDCOPCall("KTorrent", "getInfo()", ret))
			ERROR_RET_NUM

		QStringList tmp = QStringList::split(" ", ret[1]);
		if (tmp.size() != 8)
			ERROR_RET_NUM

		float f;
		if (!makeSize(f, tmp[2], tmp[3]))
			ERROR_RET_NUM

		return f;
	*/
	return 0.0;
	}
/*
	bool KviKTorrentDBusInterface::makeTorrentInfo(TorrentInfo &ti, const KviQCStringList &ret)
	{
		if (ret.size() != 10)
			ERROR_RET_BOOL

		ti.name = ret[0];
		if (ret[1] == "Seeding")
			ti.state = "Seeding";
		else
		if (ret[1] == "Stalled")
			ti.state = "Stalled";
		else
		if (ret[1] == "Downloading")
			ti.state = "Downloading";
		else
			ti.state = "Stopped";


		QStringList tmp;
		tmp = QStringList::split(" ", ret[2]);
		if (tmp.size()!=2 || !makeSize(ti.trafficDown, tmp[0], tmp[1]))
			ERROR_RET_BOOL

		tmp = QStringList::split(" ", ret[3]);
		if (tmp.size()!=2 || !makeSize(ti.size, tmp[0], tmp[1]))
			ERROR_RET_BOOL

		tmp = QStringList::split(" ", ret[4]);
		if (tmp.size()!=2 || !makeSize(ti.trafficUp, tmp[0], tmp[1]))
			ERROR_RET_BOOL


		bool ok;
		tmp = QStringList::split(" ", ret[5]);
		if (tmp.size()!=2)
			ERROR_RET_BOOL
		ti.speedDown = tmp[0].toFloat(&ok);
		if (!ok)
			ERROR_RET_BOOL

		tmp = QStringList::split(" ", ret[6]);
		if (tmp.size()!=2)
			ERROR_RET_BOOL
		ti.speedUp = tmp[0].toFloat(&ok);
		if (!ok)
			ERROR_RET_BOOL


		// torrent name, status, downloaded, size, uploaded, down spd, up spd, none, peers, % complete
		ti.peers = ret[8].toInt(&ok);
		if (!ok)
			ERROR_RET_BOOL

		tmp = QStringList::split(" ", ret[9]);
		if (tmp.size()!=2)
			ERROR_RET_BOOL
		ti.percent = tmp[0].toFloat(&ok);
		if (!ok)
			ERROR_RET_BOOL

		return true;
	}
*/
	int KviKTorrentDBusInterface::maxUploadSpeed()
	{
	/*
		int ret;
		if (!intRetVoidDCOPCall("KTorrent", "maxUploadRate()", ret))
		{
			ERROR_MSG
			return -1;
		}
		return ret;
	*/
	return -1;
	}

	int KviKTorrentDBusInterface::maxDownloadSpeed()
	{
	/*
		int ret;
		if (!intRetVoidDCOPCall("KTorrent", "maxDownloadRate()", ret))
		{
			ERROR_MSG
			return -1;
		}
		return ret;
	*/
	return -1;
	}

	bool KviKTorrentDBusInterface::setMaxUploadSpeed(int )
	{
	/*
		if (!voidRetIntDCOPCall("KTorrent", "setMaxUploadSpeed(int)", kbytes_per_sec))
			ERROR_RET_BOOL
	*/
		return true;
	}

	bool KviKTorrentDBusInterface::setMaxDownloadSpeed(int )
	{
	/*
		if (!voidRetIntDCOPCall("KTorrent", "setMaxDownloadSpeed(int)", kbytes_per_sec))
			ERROR_RET_BOOL
	*/
		return true;
	}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "tc_ktorrentdbusinterface.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

#endif //	COMPILE_KDE_SUPPORT
