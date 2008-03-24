#ifndef _TC_STATUSBARAPPLET_H_
#define _TC_STATUSBARAPPLET_H_

#include "kvi_statusbar.h"
#include "kvi_statusbarapplet.h"

// TODO: check if it gets destroyed
// TODO: proper icon!
class KviTorrentStatusBarApplet : public KviStatusBarApplet
{
	Q_OBJECT

public:

	KviTorrentStatusBarApplet(KviStatusBar *parent, KviStatusBarAppletDescriptor *desc);
	virtual ~KviTorrentStatusBarApplet();

	static void selfRegister(KviStatusBar *bar);

private slots:

	void update();

};

#endif // _TC_STATUSBARAPPLET_H_
