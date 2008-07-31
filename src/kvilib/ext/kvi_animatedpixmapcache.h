/*
 * kvi_animatedpixmapcache.h
 *
 *  Created on: 31.07.2008
 *      Author: Alexey
 */

#ifndef KVI_ANIMATEDPIXMAPCACHE_H_
#define KVI_ANIMATEDPIXMAPCACHE_H_

#include <QMutex>
#include <QPixmap>
#include <QMultiHash>

class KviAnimatedPixmapCache {

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
			QList<FrameInfo> (), refs(0), resized(false), file(szFile) {
		}

		Data(Data& other) :
			QList<FrameInfo> (other), refs(0), resized(false), file(other.file) {
				for(int i=0;i<count();i++)
				{
					this->operator [](i).detach();
				}
		}
	};
protected:

	mutable QMutex                 m_cacheMutex;
	QMultiHash<QString,Data*>      m_hCache;

	/*
	 * This class is a singletone.
	 * It doesn't allow to be created directly;
	 */
	KviAnimatedPixmapCache();

	Data* internalLoad  (QString szFile);
	Data* internalResize(Data* data,QSize size);

	void  internalFree(Data* data);
public:
	virtual ~KviAnimatedPixmapCache();

	static Data* load(QString szFileName);
	static Data* resize(Data* data,QSize size);
	static void  free(Data* data);
};

#endif /* KVI_ANIMATEDPIXMAPCACHE_H_ */
