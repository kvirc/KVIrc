#ifndef KVI_ANIMATEDPIXMAPCACHE_H_
#define KVI_ANIMATEDPIXMAPCACHE_H_
//=============================================================================
//
//   File : kvi_animatedpixmapcache.h
//   Creation date : Thu Jul 31 2008 01:45:21 CEST by Alexey Uzhva
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2008 Alexey Uzhva (wizard at opendoor dot ru)
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

#include <QMutex>
#include <QPixmap>
#include <QMultiHash>
#include <QMultiMap>
#include <QTimer>
#include "kvi_settings.h"

class KviAnimatedPixmapInterface {
public:
	 virtual void nextFrame() = 0;
	 virtual ~KviAnimatedPixmapInterface() {};
};

class KVILIB_API KviAnimatedPixmapCache : public QObject {
        Q_OBJECT
public:
	/*
	 * This subclass represents simple structure
	 * to store animated frame data: cached pixmap, and next frame delay.
	 *
	 * It provides copyconstructor, wich makes possible to simple
	 * assign two containers with this classes.
	 *
	 * All data will be dublicated in such case.
	 */
	class FrameInfo {
	public:
		QPixmap *pixmap; //frame pixmap
		uint delay; //next frame delay

		FrameInfo(QPixmap *_pixmap, uint _delay) {
			pixmap = _pixmap;
			delay = _delay;
		}

		FrameInfo(const FrameInfo& source) {
			pixmap = source.pixmap;
			delay = source.delay;
		}

		void detach() {
			pixmap = new QPixmap(*pixmap);
		}
	};

	/*
	 * This helper class represents data structure
	 * for storing frames.
	 *
	 * It adds references counter and
	 * mutex, to provide thread-safety.
	 */
	class Data: public QList<FrameInfo> {
	public:
		uint     refs;    //references count
		QSize    size;    //size of the pixmaps
		QString  file;    //just to speedup the cache
		bool     resized;

		Data(QString szFile) :
			QList<FrameInfo> (), refs(0), file(szFile), resized(false) {
		}

		Data(Data& other) :
			QList<FrameInfo> (other), refs(0), file(other.file), resized(false) {
				for(int i=0;i<count();i++)
				{
					this->operator [](i).detach();
				}
		}
	};
protected:

	mutable QMutex                               m_cacheMutex;
	mutable QMutex                               m_timerMutex;

	QMultiHash<QString,Data*>                    m_hCache;
	QMultiMap<long long,KviAnimatedPixmapInterface*> m_timerData;
	QTimer                                       m_animationTimer;

	/*
	 * This class is a singletone.
	 * It doesn't allow to be created directly;
	 */
	KviAnimatedPixmapCache();

	Data* internalLoad  (QString szFile);
	Data* internalResize(Data* data,QSize size);

	void  internalFree(Data* data);
	void  internalSceduleFrameChange(uint delay,KviAnimatedPixmapInterface* receiver);
	void  internalNotifyDelete(KviAnimatedPixmapInterface* receiver);
protected slots:
        virtual void timeoutEvent();
public:
	virtual ~KviAnimatedPixmapCache();

	static void  sceduleFrameChange(uint delay,KviAnimatedPixmapInterface* receiver);
	static Data* load(QString szFileName);
	static Data* resize(Data* data,QSize size);
	static void  free(Data* data);
	static QPixmap* dummyPixmap();
	static void notifyDelete(KviAnimatedPixmapInterface* receiver);
};

#endif /* KVI_ANIMATEDPIXMAPCACHE_H_ */
