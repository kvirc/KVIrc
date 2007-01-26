//=============================================================================
//
//	Common interface for BitTorrent clients.
//
//   File : torr_ktorrentdcopinterface.cpp
//   Creation date : Fri Jan 1 15:42:25 2007 GMT by Alexander Stillich
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2005 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2007 Alexander Stillich (torque at pltn dot org)
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

#include "kvi_app.h"
#include "dcopclient.h"

#include "tc_ktorrentdcopinterface.h"

#ifdef COMPILE_KDE_SUPPORT

#include "kvi_locale.h"
#include <qtimer.h>

TORR_IMPLEMENT_DESCRIPTOR(
	KviKTorrentDCOPInterface,
	"ktorrentdcop",
	__tr2qs_ctx(
		"An interface to KDE's excellent KTorrent client.\n" \
		"Download it from http://www.ktorrent.org\n",
		"torrent"
	)
)

#define ERROR_MSG \
	QCString msg; \
	if (!findRunningApp(m_szAppId)) \
		msg = "KTorrent's isn't running!"; \
	else \
		msg = "Something's wrong here! KTorrent's DCOP interface has probably changed."; \
	m_lastError = __tr2qs_ctx(QString(msg), "torrent"); \
	debug("%s (%s:%d): %s", __PRETTY_FUNCTION__, __FILE__, __LINE__, (const char*)msg); \

#define ERROR_MSG_RANGE \
	KviQString::sprintf(m_lastError, __tr2qs_ctx("Index out of range: %d [0-%d]!", "torrent"), i, (m_ti.size()>0)?(m_ti.size()-1):0); \
	debug("%s (%s:%d): Index out of range: %d [0-%d]!", __PRETTY_FUNCTION__ , __FILE__, __LINE__, i, (m_ti.size()>0)?(m_ti.size()-1):0);

#define ERROR_RET_BOOL \
	{ \
		ERROR_MSG \
		return false; \
	}

#define ERROR_RET_NUM \
	{ \
		ERROR_MSG \
		return -1; \
	}

// TODO: unused?
#define ERROR_RET_STRING \
	{ \
		ERROR_MSG \
		return ""; \
	}

#define ERROR_RET_VOID \
	{ \
		ERROR_MSG \
		return; \
	}

#define CHECK_RANGE_BOOL \
	if (i<0 || i>=m_ti.size()) \
	{ \
		ERROR_MSG_RANGE \
		return false; \
	}

#define CHECK_RANGE_STRING \
	if (i<0 || i>=m_ti.size()) \
	{ \
		ERROR_MSG_RANGE \
		return ""; \
	}


KviKTorrentDCOPInterface::KviKTorrentDCOPInterface()
	: KviDCOPHelper(false, "ktorrent")
{
	printf("KviKTorrentDCOPInterface\n");

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(slotTimer()));
	timer->start(250, FALSE);
}

KviKTorrentDCOPInterface::~KviKTorrentDCOPInterface() 
{
	printf("~KviKTorrentDCOPInterface\n");
}

void KviKTorrentDCOPInterface::slotTimer()
{
	if (!findRunningApp(m_szAppId))
		return;
//	debug("timer");

	m_ti.clear();

	KviValueList<int> ret;
	if (!qvalueListIntRetIntDCOPCall("KTorrent", "getTorrentNumbers(int)", ret, 0))
		ERROR_RET_VOID

	for (int i=0; i<ret.size(); i++)
	{
		KviQCStringList info;
		if (!qcstringListRetIntDCOPCall("KTorrent", "getTorrentInfo(int)", info, ret[i]))
			ERROR_RET_VOID

		if (info.size() == 0)
			continue;

//		debug("i: %d, %d, %s %s", i, info.size(), (const char *)info[0], (const char*)info[1]);

		TorrentInfo item;
		if (!makeTorrentInfo(item, info))
			return;
//		item.ti.name.sprintf("%s [%d]", (const char*)info[0], ret[i]);
		item.num = ret[i];
		m_ti.append(item);
	}

	qHeapSort(m_ti);


/*	for (int i=0; i<m_ti.size(); i++)
	{
		debug("sorted: %s", (const char *)m_ti[i].name);
	}
*/
}

int KviKTorrentDCOPInterface::detect()
{
	if (!findRunningApp(m_szAppId))
		return 0;
/*
	ClientInfo ci;
	if (getClientInfo(ci))
	{
		debug("tup: %f, tdown: %f\n"
			 "sup: %f, sdown: %f\n",
			 ci.transferredUp, ci.transferredDown,
			 ci.speedUp, ci.speedDown);
	} else
	{
		debug("error\n");
	}

	TorrentInfo ti;
	if (getTorrentInfo(0, ti))
	{
		debug("name: %s\n"
                "state: %d\n"
                "size: %f\n"
			 "tup: %f\n"
			 "tdown: %f\n"
			 "sup: %f\n"
			 "sdown: %f\n"
			 "perc: %f\n"
			 "peers: %d\n",
			 (const char *)ti.name,
			 ti.state,
			 ti.size,
			 ti.transferredUp,
			 ti.transferredDown,
			 ti.speedUp,
			 ti.speedDown,
			 ti.percent,
			 ti.peers);
	} else
	{
		debug("error\n");
	}
*/

	return 100;
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
	if (u == "KB")
		sz = sz * 1024.0;
	else
	if (u == "MB")
		sz = sz * 1024.0 * 1024.0;
	else
	if (u == "GB")
		sz = sz * 1024.0 * 1024.0 * 1024.0;
	else
		return false;

	return true;
}

/*bool KviKTorrentDCOPInterface::getClientInfo(ClientInfo &ci)
{
	KviQCStringList ret;
	if (!qcstringListRetVoidDCOPCall("KTorrent", "getInfo()", ret))
  		ERROR_RET_BOOL

	bool ok;
	QStringList tmp;

	tmp = QStringList::split(" ", ret[1]);
	if (tmp.size() != 8)
		ERROR_RET_BOOL

	if (!makeSize(ci.trafficDown, tmp[2], tmp[3]))
  		ERROR_RET_BOOL
	if (!makeSize(ci.trafficUp, tmp[6], tmp[7]))
  		ERROR_RET_BOOL

	tmp = QStringList::split(" ", ret[2]);
	if (tmp.size() != 8)
  		ERROR_RET_BOOL

	ci.speedDown = tmp[2].toFloat(&ok);
	if (!ok)
  		ERROR_RET_BOOL
	ci.speedUp = tmp[6].toFloat(&ok);
	if (!ok)
  		ERROR_RET_BOOL

	return true;
}

/*
bool KviKTorrentDCOPInterface::getTorrentInfo(int i, TorrentInfo &ti)
{
	CHECK_RANGE

	ti = m_ti[i];
	return true;
}
*/

bool KviKTorrentDCOPInterface::start(int i)
{
	CHECK_RANGE_BOOL

	debug("starting %s [%d]", (const char*)m_ti[i].name, m_ti[i].num);
	if (!voidRetIntDCOPCall("KTorrent", "start(int)", m_ti[i].num))
		ERROR_RET_BOOL

	return true;
}

bool KviKTorrentDCOPInterface::stop(int i)
{
	CHECK_RANGE_BOOL

	debug("stopping %s [%d]", (const char*)m_ti[i].name, m_ti[i].num);
	if (!voidRetIntBoolDCOPCall("KTorrent", "stop(int, bool)", m_ti[i].num, true))
		ERROR_RET_BOOL

	return true;
}

bool KviKTorrentDCOPInterface::announce(int i)
{
	CHECK_RANGE_BOOL

	debug("announcing %s [%d]", (const char*)m_ti[i].name, m_ti[i].num);
	if (!voidRetIntDCOPCall("KTorrent", "announce(int)", m_ti[i].num))
		ERROR_RET_BOOL
	return true;
}

QString KviKTorrentDCOPInterface::state(int i)
{
	CHECK_RANGE_STRING

	return m_ti[i].state;
}

QString KviKTorrentDCOPInterface::name(int i)
{
	CHECK_RANGE_STRING
	
	return m_ti[i].name;
}

bool KviKTorrentDCOPInterface::startAll()
{
	if (!voidRetIntDCOPCall("KTorrent", "startAll(int)", 3))
		ERROR_RET_BOOL

	return true;
}

bool KviKTorrentDCOPInterface::stopAll()
{
	if (!voidRetIntDCOPCall("KTorrent", "stopAll(int)", 3))
		ERROR_RET_BOOL

	return true;
}

int KviKTorrentDCOPInterface::count()
{
	return m_ti.size();
}

float KviKTorrentDCOPInterface::speedUp()
{
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
}

float KviKTorrentDCOPInterface::speedDown()
{
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
}

float KviKTorrentDCOPInterface::trafficUp()
{
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
}

float KviKTorrentDCOPInterface::trafficDown()
{
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
}

bool	KviKTorrentDCOPInterface::makeTorrentInfo(TorrentInfo &ti, const KviQCStringList &ret)
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

int KviKTorrentDCOPInterface::maxUploadSpeed()
{
	int ret;
	if (!intRetVoidDCOPCall("KTorrent", "maxUploadRate()", ret))
	{
		ERROR_MSG
		return -1;
	}
	return ret;
}

int KviKTorrentDCOPInterface::maxDownloadSpeed()
{
	int ret;
	if (!intRetVoidDCOPCall("KTorrent", "maxDownloadRate()", ret))
	{
		ERROR_MSG
		return -1;
	}
	return ret;
}

bool KviKTorrentDCOPInterface::setMaxUploadSpeed(int kbytes_per_sec)
{
	if (!voidRetIntDCOPCall("KTorrent", "setMaxUploadSpeed(int)", kbytes_per_sec))
		ERROR_RET_BOOL
	return true;
}

bool KviKTorrentDCOPInterface::setMaxDownloadSpeed(int kbytes_per_sec)
{
	if (!voidRetIntDCOPCall("KTorrent", "setMaxDownloadSpeed(int)", kbytes_per_sec))
		ERROR_RET_BOOL
	return true;
}

#endif // COMPILE_KDE_SUPPORT

#include "tc_ktorrentdcopinterface.moc"
