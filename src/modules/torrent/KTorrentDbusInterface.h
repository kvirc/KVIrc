#ifndef _TC_KTORRENTDCOPINTERFACE_H_
#define _TC_KTORRENTDCOPINTERFACE_H_
//=============================================================================
//
//   DBus interface for KTorrent client.
//
//   File : KTorrentDbusInterface.h
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

#include "TorrentInterface.h"

#ifdef COMPILE_KDE_SUPPORT

#include <QtDBus/QtDBus>

class KTorrentDbusInterface : public TorrentInterface
{
	Q_OBJECT

public:
	KTorrentDbusInterface();
	~KTorrentDbusInterface();

	int detect() override;

	bool startAll() override;
	bool stopAll() override;

	int count() override;
	bool start(int i) override;
	bool stop(int i) override;
	bool announce(int i) override;
	QString state(int i) override;
	QString name(int i) override;
	int fileCount(int i) override;
	QString fileName(int i, int file) override;
	QString filePriority(int i, int file) override;
	bool setFilePriority(int i, int file, const QString & prio) override;

	int maxUploadSpeed() override;
	int maxDownloadSpeed() override;

	bool setMaxUploadSpeed(int kbytes_per_sec) override;
	bool setMaxDownloadSpeed(int kbytes_per_sec) override;

	float speedUp() override;
	float speedDown() override;

	float trafficUp() override;
	float trafficDown() override;
	/*
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
*/
private:
	//		bool makeTorrentInfo(TorrentInfo &ti, const KviQCStringList &ret);
	bool findRunningApp();
};

TORR_DECLARE_DESCRIPTOR(KTorrentDbusInterface)

#endif // COMPILE_KDE_SUPPORT

#endif // _TC_KTORRENTDCOPINTERFACE_H_
