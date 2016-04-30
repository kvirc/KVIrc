//=============================================================================
//
//   StatuBar for Torrent client.
//
//   File : StatusBarApplet.cpp
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

#include "TorrentInterface.h"
#include "StatusBarApplet.h"

#include "KviLocale.h"
#include "KviIconManager.h"

#include "QTimer"

StatusBarApplet::StatusBarApplet(KviStatusBar * parent, KviStatusBarAppletDescriptor * desc)
    : KviStatusBarApplet(parent, desc)
{
	QTimer * timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->setInterval(250);
	timer->setSingleShot(false);
	timer->start(250);

	setText(__tr2qs_ctx("Torrent Client", "torrent"));
}

StatusBarApplet::~StatusBarApplet()
    = default;

static KviStatusBarApplet * CreateTorrentClientApplet(KviStatusBar * bar, KviStatusBarAppletDescriptor * desc)
{
	KviStatusBarApplet * pApplet = new StatusBarApplet(bar, desc);
	pApplet->setIndex(bar->insertPermanentWidgetAtTheEnd(pApplet));
	return pApplet;
}

void StatusBarApplet::selfRegister(KviStatusBar * bar)
{
	KviStatusBarAppletDescriptor * d = new KviStatusBarAppletDescriptor(
	    __tr2qs_ctx("Torrent Client", "torrent"),
	    "torrentapplet",
	    CreateTorrentClientApplet,
	    "torrent", *(g_pIconManager->getSmallIcon(KviIconManager::Away)));

	bar->registerAppletDescriptor(d);
}

QString formatSize(float sz)
{
	if(sz >= 1024.0f * 1024.0f * 1024.0f)
		return QString("%1 GiB").arg(sz / (1024.0f * 1024.0f * 1024.0f), 2, 'f', 2);
	if(sz >= 1024.0f * 1024.0f)
		return QString("%1 MiB").arg(sz / (1024.0f * 1024.0f), 2, 'f', 2);
	if(sz >= 1024.0f)
		return QString("%1 KiB").arg(sz / 1024.0f, 2, 'f', 2);
	return QString("%1 B").arg(sz, 2, 'f', 2);
}

void StatusBarApplet::update()
{
	if(TorrentInterface::selected())
	{
		QString msg = QString("up: %1 K/s (%2), dn: %3 K/s (%4)")
		                  .arg(TorrentInterface::selected()->speedUp(), 2)
		                  .arg(formatSize(TorrentInterface::selected()->trafficUp()))
		                  .arg(TorrentInterface::selected()->speedDown(), 2)
		                  .arg(formatSize(TorrentInterface::selected()->trafficDown()));

		setText(msg);
	}
	else
	{
		setText(__tr2qs_ctx("No client selected!", "torrent"));
	}
}
