/*
 * kvi_animatedpixmap.h
 *
 *  Created on: 30.07.2008
 *      Author: Alexey
 */

#ifndef KVI_ANIMATEDPIXMAP_H_
#define KVI_ANIMATEDPIXMAP_H_

#include <QObject>
#include <QPixmap>
#include <QList>
#include <QTimer>
#include "kvi_settings.h"
#include "kvi_animatedpixmapcache.h"

/**
 * This class should not be here, because we already have QMovie
 * with the same functionality.
 *
 * But QMovie is really stupid, bacause:
 * 	1) It does not support infinitive animation.
 *     It relies on image metadata to determine loop count. So most of pics will
 *     be played only once.
 *
 *  2) It dublicates QPixmap while we calls currentFrame(), that's a very
 *     big overload for us.
 *
 *  3) If we try to scale QMovie by using built-in scale mechanism, it will
 *     manually scale each frame each time, we will call currentFrame().
 *     So if we plan to play movie 10,100,1000 times, it will scale image
 *     10,100,1000 times too:(
 *
 *  4) KviAnimatedPixmap uses internal cache mechanism with KviAnimatedPixmapCache class.
 *     It's managed inside KviAnimatedPixmap and KviAnimatedPixmapCache. No othet actions
 *     needed to perform caching.
 *
 * So... Here is my workaround. It's working in this way:
 * You should create new pixmap with constructor KviAnimatedPixmap(QString fileName).
 * It will automagically look at cache and use it, if possible.
 *
 * KviAnimatedPixmap stores animation as a set of QPixmap's, and delays.
 * when started (see start()) it emits frameChanged() events at the frame change.
 *
 * You should use pixmap() methd to access the current frame.
 *
 * This class owns all pixmaps. Do not store links to them.
 */

class KVILIB_API KviAnimatedPixmap : public QObject , public KviAnimatedPixmapInterface{
	Q_OBJECT

protected:
	QString                        m_szFileName;
	KviAnimatedPixmapCache::Data*  m_lFrames;

	uint                           m_uCurrentFrameNumber;
	bool                           m_bStarted;
public:
	/*
	* Creates KviAnimatedPixmap, and loads data from "fileName".
	*/
	KviAnimatedPixmap(QString fileName);

	virtual ~KviAnimatedPixmap();

	/*
	 * Creates new pixmap using "source" as source of images.
	 */
	KviAnimatedPixmap(KviAnimatedPixmap* source);

	/*
	 * Returns true if animation is started.
	 * Returns false otherways.
	 */
	inline bool isStarted()
	{
		return m_bStarted;
	}

	/*
	 * Starts the animation
	 */
	void start();

	/*
	 * Stops the animation
	 */
	void stop();

	/*
	 * Returns true if animation has at least one loaded frame.
	 * Returns false otherways.
	 */
	inline bool isValid()
	{
		return (m_lFrames->count()>0);
	}

	/*
	 * Returns active frame's pixmap.
	 * Never fails.
	 */
//#ifdef COMPILE_ON_MINGW
      	/*
	 * Mingw/gcc3.4.2 have some problems linking kvirc because of this:
	 * It doesn't like the way we import inline static members from a singletone class, and spit out this error on linking:
         *
         * CMakeFiles\kvirc4.dir\ui\kvi_userlistview.obj(.text$_ZN17KviAnimatedPixmap6pixmapEv[KviAnimatedPixmap::pixmap()]+0x38):
         * In function `ZN24KviPointerHashTableEntryI7QString16KviUserListEntryE3keyEv':
         * C:/kvirctrunk/src/kvirc/../kvilib/core/kvi_pointerlist.h: undefined reference to `KviAnimatedPixmapCache::dummyPixmap()'
	 */

/*	inline QPixmap* pixmap()
	{
		if(m_lFrames->count()>0)
			return m_lFrames->at(m_uCurrentFrameNumber).pixmap;
		else
			return new QPixmap(); // FIXME, memory leak
	}

#else*/
	inline QPixmap* pixmap()
	{
		if(m_lFrames->count()>0)
			return m_lFrames->at(m_uCurrentFrameNumber).pixmap;
		else
			return KviAnimatedPixmapCache::dummyPixmap();
	}
//#endif
	/*
	 * Returns active frame number
	 */
	inline uint activeFrameNumber()
	{
		return m_uCurrentFrameNumber;
	}

	/*
	 * Returns animation frame count
	 */
	inline uint framesCount()
	{
		return m_lFrames->count();
	}

	/*
	 * Returns current image size
	 */
	inline const QSize& size()
	{
		return m_lFrames->size;
	}

	/*
	 * Resizes all frames to the newSize size, using "ratioMode"
	 * ratio.
	 */
	void resize(QSize newSize,Qt::AspectRatioMode ratioMode);

	/*
	 * Called when the frame changes
	 */
	void nextFrame();

signals:

	/*
	 * Slot, to be connected to m_animationTimer, to receive animation
	 * frame changes.
	 */
	void frameChanged();
};

#endif /* KVI_ANIMATEDPIXMAP_H_ */
