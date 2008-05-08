#ifndef KVI_DBUSADAPTOR_H
#define KVI_DBUSADAPTOR_H
#include <QDBusAbstractAdaptor>
#include <QDBusInterface>
#include <QObject>
#include "kvi_settings.h"

class KviDbusAdaptor: public QDBusAbstractAdaptor
{
	Q_OBJECT
	Q_CLASSINFO("Kvirc D-Bus Interface", "org.kvirc.KVIrc")

public:
	KviDbusAdaptor(QObject * obj);
};

#endif