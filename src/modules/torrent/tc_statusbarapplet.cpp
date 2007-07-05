#include "qtimer.h"
#include "kvi_locale.h"
#include "kvi_iconmanager.h"
#include "tc_interface.h"
#include "tc_statusbarapplet.h"

KviTorrentStatusBarApplet::KviTorrentStatusBarApplet(KviStatusBar *parent, KviStatusBarAppletDescriptor *desc)
	: KviStatusBarApplet(parent, desc)
{
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(250, FALSE);
//	updateDisplay();
//
	setText("torrent client");
}

KviTorrentStatusBarApplet::~KviTorrentStatusBarApplet()
{
}

static KviStatusBarApplet *CreateTorrentClientApplet(KviStatusBar *bar, KviStatusBarAppletDescriptor *desc)
{
	debug("CreateTorrentClientApplet");
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

	} else
	{
		setText(__tr2qs_ctx("No client selected!", "torrent"));
	}
}

