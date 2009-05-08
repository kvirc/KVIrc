#ifndef _TC_KTORRENTDCOPINTERFACE_H_
#define _TC_KTORRENTDCOPINTERFACE_H_

//=============================================================================
//
//	DCOP interface for KTorrent client.
//
//   File : torr_ktorrentdcopinterface.h
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================


#include "tc_interface.h"

#ifdef COMPILE_KDE_SUPPORT

#include <kvi_dcophelper.h>

// DCOP interface to KTorrent. this has 'DCOP' in its name
// because in kde4 there will be a D-BUS interface.
class KviKTorrentDCOPInterface : public KviTorrentInterface,
                                 private KviDCOPHelper
{
	Q_OBJECT

public:

	KviKTorrentDCOPInterface();
	virtual ~KviKTorrentDCOPInterface();

	virtual int detect();

	virtual int count();

	virtual bool start(int i);
	virtual bool stop(int i);
	virtual bool announce(int i);
	virtual QString state(int i);
	virtual QString name(int i);

	virtual int fileCount(int i);
	virtual QString fileName(int i, int file);
	virtual QString filePriority(int i, int file);
	virtual bool setFilePriority(int i, int file, const QString &prio);

	virtual bool startAll();
	virtual bool stopAll();

	virtual int maxUploadSpeed();
	virtual int maxDownloadSpeed();

	virtual bool setMaxUploadSpeed(int kbytes_per_sec);
	virtual bool setMaxDownloadSpeed(int kbytes_per_sec);

	virtual float speedUp();
	virtual float speedDown();

	virtual float trafficUp();
	virtual float trafficDown();

private slots:

	// polls client and extracts information.
	// this is done because the order of torrents returned changes
	// each time a torrent's state changes.
	// we want to present a consistent list (in terms of indices) to
	// the user, so we extract the info and sort it by name.
	// otherwise the user would have a hard time figuring out what's
	// going on. we could sort each time a function working with
	// torrents is called, but this would be horribly slow ...
	void	slotTimer();

private:

	struct TorrentInfo
	{
		// internal number
		int num;
		// name displayed in ktorrent
		QString name;
		// state of torrent
		QString state;
		// bytes
		float size;
		// bytes
		float trafficUp;
		// bytes
		float trafficDown;
		// KB/s
		float speedUp;
		// KB/s
		float speedDown;
		// percent complete
		float percent;
		// number of peers
		int peers;

		bool operator<(const TorrentInfo &ti) { return name < ti.name; }
	};

	QValueList<TorrentInfo>	m_ti;

private:

	bool makeTorrentInfo(TorrentInfo &ti, const KviQCStringList &ret);
};

TORR_DECLARE_DESCRIPTOR(KviKTorrentDCOPInterface)

#endif // COMPILE_KDE_SUPPORT

#endif // _TC_KTORRENTDCOPINTERFACE_H_
