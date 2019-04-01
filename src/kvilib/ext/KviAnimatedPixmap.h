#ifndef KVI_ANIMATEDPIXMAP_H_
#define KVI_ANIMATEDPIXMAP_H_
//=============================================================================
//
//   File : KviAnimatedPixmap.h
//   Creation date : Wed Jul 30 2008 01:45:21 CEST by Alexey Uzhva
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Alexey Uzhva (wizard at opendoor dot ru)
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
#include "KviAnimatedPixmapCache.h"

#include <QObject>
#include <QString>
#include <QTimer>

class QPixmap;

/**
 * This class should not be here, because we already have QMovie
 * with the same functionality.
 *
 * But QMovie is really stupid, bacause:
 * 	1) It does not support infinitive animation.
 *     It relies on image metadata to determine loop count. So most of pics will
 *     be played only once.
 *
 *  2) It duplicates QPixmap while we calls currentFrame(), that's a very
 *     big overload for us.
 *
 *  3) If we try to scale QMovie by using built-in scale mechanism, it will
 *     manually scale each frame each time, we will call currentFrame().
 *     So if we plan to play movie 10,100,1000 times, it will scale image
 *     10,100,1000 times too:(
 *
 *  4) KviAnimatedPixmap uses internal cache mechanism with KviAnimatedPixmapCache class.
 *     It's managed inside KviAnimatedPixmap and KviAnimatedPixmapCache. No other actions
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

class KVILIB_API KviAnimatedPixmap : public QObject, public KviAnimatedPixmapInterface
{
	Q_OBJECT
protected:
	QString m_szFileName;
	KviAnimatedPixmapCache::Data * m_pFrameData;

	uint m_uCurrentFrameNumber;
	int m_iStarted;

public:
	/*
	* Creates KviAnimatedPixmap, and loads data from "fileName".
	*/
	KviAnimatedPixmap(QString fileName, int iWidth = 0, int iHeight = 0);

	virtual ~KviAnimatedPixmap();

	/*
	 * Creates new pixmap using "source" as source of images.
	 */
	KviAnimatedPixmap(const KviAnimatedPixmap & source);

	/*
	 * Returns true if animation is started.
	 * Returns false otherways.
	 */
	bool isStarted() const
	{
		return m_iStarted > 0;
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
	bool isValid() const
	{
		return (m_pFrameData->count() > 0);
	}

	/*
	 * Returns active frame's pixmap.
	 * Never fails.
	 */

	QPixmap * pixmap()
	{
		if(m_pFrameData->count() > 0)
			return m_pFrameData->at(m_uCurrentFrameNumber).pixmap;
		else
			return KviAnimatedPixmapCache::dummyPixmap();
	}

	/*
	 * Returns active frame number
	 */
	uint activeFrameNumber() const
	{
		return m_uCurrentFrameNumber;
	}

	/*
	 * Returns animation frame count
	 */
	uint framesCount() const
	{
		return m_pFrameData->count();
	}

	/*
	 * Returns current image size
	 */
	const QSize & size() const
	{
		return m_pFrameData->size;
	}

	/*
	 * Resizes all frames to the newSize size, using "ratioMode"
	 * ratio.
	 */
	void resize(QSize newSize, Qt::AspectRatioMode ratioMode);

	/*
	 * Called when the frame changes
	 */
	void nextFrame(bool bEmitSignalAndScheduleNext);

signals:

	/*
	 * Slot, to be connected to m_animationTimer, to receive animation
	 * frame changes.
	 */
	void frameChanged();
};

#endif /* KVI_ANIMATEDPIXMAP_H_ */
