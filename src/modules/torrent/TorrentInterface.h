#ifndef _TC_INTERFACE_H_
#define _TC_INTERFACE_H_
//=============================================================================
//
//   Common interface for BitTorrent clients.
//
//   File : TorrentInterface.h
//   Creation date : Fri Jan 1 15:42:25 2007 GMT by Alexander Stillich
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2007-2008 Alexander Stillich (torque at pltn dot org)
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

#include "kvi_settings.h"
#include "KviQString.h"

#include <QObject>

class TorrentInterface : public QObject
{
public:
	TorrentInterface() {}
	~TorrentInterface() {}

	virtual int detect() = 0;

	// returns number of torrents in client
	virtual int count() = 0;

	/*
	// path of torrent file
	virtual QCString getTorrentFile(int i)=0;

	// directory where data is downloaded to
	virtual QCString getTorrentDataDir(int i)=0;
*/
	// number of files in torrent
	virtual int fileCount(int i) = 0;
	// name of file in torrent
	virtual QString fileName(int i, int file) = 0;
	// returns file priority (low, normal, high)
	virtual QString filePriority(int i, int file) = 0;
	// sets file priority
	virtual bool setFilePriority(int i, int file, const QString & prio) = 0;

	virtual bool start(int i) = 0;
	virtual bool stop(int i) = 0;

	virtual bool announce(int i) = 0;

	virtual bool startAll() = 0;
	virtual bool stopAll() = 0;
	/*
	// remove torrent from client
	virtual bool removeTorrent(int i)=0;

	virtual bool addTorrent(const QCString &mrl);
*/
	// returns state of torrent number i (Stopped, Stalled, Seeding, Downloading)
	// this uses getTorrentInfo() to obtain the state and then
	// returns it as string
	virtual QString state(int i) = 0;

	// name of torrent as displayed in client
	// uses getTorrentInfo()
	virtual QString name(int i) = 0;

	virtual float speedUp() = 0;
	virtual float speedDown() = 0;

	virtual float trafficUp() = 0;
	virtual float trafficDown() = 0;

	virtual int maxUploadSpeed() = 0;
	virtual int maxDownloadSpeed() = 0;

	virtual bool setMaxUploadSpeed(int kbytes_per_sec) = 0;
	virtual bool setMaxDownloadSpeed(int kbytes_per_sec) = 0;

	QString lastError() { return m_lastError; }

	static void select(TorrentInterface * i) { m_selected = i; }
	static TorrentInterface * selected() { return m_selected; }

protected:
	QString m_lastError;
	static TorrentInterface * m_selected;
};

class TorrentInterfaceDescriptor
{
public:
	TorrentInterfaceDescriptor(){};
	virtual ~TorrentInterfaceDescriptor(){};

public:
	virtual const QString & name() = 0;
	virtual const QString & description() = 0;
	virtual TorrentInterface * instance() = 0;
};

#define TORR_DECLARE_DESCRIPTOR(_interfaceclass)                          \
	class _interfaceclass##Descriptor : public TorrentInterfaceDescriptor \
	{                                                                     \
	public:                                                               \
		_interfaceclass##Descriptor();                                    \
		~_interfaceclass##Descriptor();                                   \
                                                                          \
	protected:                                                            \
		_interfaceclass * m_pInstance;                                    \
		QString m_szName;                                                 \
		QString m_szDescription;                                          \
                                                                          \
	public:                                                               \
		const QString & name() override;                                    \
		const QString & description() override;                             \
		TorrentInterface * instance() override;                             \
	};

#define TORR_IMPLEMENT_DESCRIPTOR(_interfaceclass, _name, _description) \
	_interfaceclass##Descriptor::_interfaceclass##Descriptor()          \
	    : TorrentInterfaceDescriptor()                                  \
	{                                                                   \
		m_pInstance = nullptr;                                          \
		m_szName = _name;                                               \
		m_szDescription = _description;                                 \
	}                                                                   \
	_interfaceclass##Descriptor::~_interfaceclass##Descriptor()         \
	{                                                                   \
		delete m_pInstance;                                             \
	}                                                                   \
	const QString & _interfaceclass##Descriptor::name()                 \
	{                                                                   \
		return m_szName;                                                \
	}                                                                   \
	const QString & _interfaceclass##Descriptor::description()          \
	{                                                                   \
		return m_szDescription;                                         \
	}                                                                   \
	TorrentInterface * _interfaceclass##Descriptor::instance()          \
	{                                                                   \
		if(!m_pInstance)                                                \
			m_pInstance = new _interfaceclass();                        \
		return m_pInstance;                                             \
	}

#endif // _TC_INTERFACE_H_
