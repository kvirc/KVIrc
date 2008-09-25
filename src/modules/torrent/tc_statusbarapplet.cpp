//=============================================================================
//
//   DCOP interface for KTorrent client.
//
//   File : tc_statusbarapplet.cpp
//   Creation date : Fri Jan 1 15:42:25 2007 GMT by Alexander Stillich
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007-2008 Alexander Stillich (torque at pltn dot org)
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

#include "tc_interface.h"
#include "tc_statusbarapplet.h"

#include "kvi_locale.h"
#include "kvi_iconmanager.h"

#include "QTimer"

KviTorrentStatusBarApplet::KviTorrentStatusBarApplet(KviStatusBar *parent, KviStatusBarAppletDescriptor *desc)
	: KviStatusBarApplet(parent, desc)
{
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->setInterval(250);
	timer->setSingleShot(false);
	timer->start(250);
//	updateDisplay();
//
	setText("torrent client");
}

KviTorrentStatusBarApplet::~KviTorrentStatusBarApplet()
{
}

static KviStatusBarApplet *CreateTorrentClientApplet(KviStatusBar *bar, KviStatusBarAppletDescriptor *desc)
{
	qDebug("CreateTorrentClientApplet");
	return new KviTorrentStatusBarApplet(bar, desc);
}

void KviTorrentStatusBarApplet::selfRegister(KviStatusBar *bar)
{
	KviStatusBarAppletDescriptor *d = new KviStatusBarAppletDescriptor(
		__tr2qs("Torrent Client"),
			"torrentapplet",
			CreateTorrentClientApplet,
			"torrent", *(g_pIconManager->getSmallIcon(KVI_SMALLICON_AWAY)));

	bar->registerAppletDescriptor(d);
}

QString formatSize(float sz)
{
	if (sz >= 1024.0f*1024.0f*1024.0f)
		return QString("%1 GB").arg(sz / (1024.0f*1024.0f*1024.0f), 2, 'f', 2);
	if (sz >= 1024.0f*1024.0f)
		return QString("%1 MB").arg(sz / (1024.0f*1024.0f), 2, 'f', 2);
	if (sz >= 1024.0f)
		return QString("%1 KB").arg(sz / 1024.0f, 2, 'f', 2);
	return QString("%1 B").arg(sz, 2, 'f', 2);
}

void KviTorrentStatusBarApplet::update()
{
	if (KviTorrentInterface::selected())
	{
		QString msg = QString("up: %1 K/s (%2), dn: %3 K/s (%4)")
					.arg(KviTorrentInterface::selected()->speedUp(), 2)
					.arg(formatSize(KviTorrentInterface::selected()->trafficUp()))
					.arg(KviTorrentInterface::selected()->speedDown(), 2)
					.arg(formatSize(KviTorrentInterface::selected()->trafficDown()));

		setText(msg);
	} else {
		setText(__tr2qs_ctx("No client selected!", "torrent"));
	}
}

#include "tc_statusbarapplet.moc"
