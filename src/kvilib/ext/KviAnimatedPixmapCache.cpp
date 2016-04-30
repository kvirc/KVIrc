//=============================================================================
//
//   File : KviAnimatedPixmapCache.cpp
//   Creation date : Thu Jul 31 2008 01:45:21 CEST by Alexey Uzhva
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

#include "KviAnimatedPixmapCache.h"
#include "KviTimeUtils.h"

#include <QImageReader>
#include <QImage>

#define FRAME_DELAY 100

KviAnimatedPixmapCache * KviAnimatedPixmapCache::m_pInstance = nullptr;
static QPixmap * g_pDummyPixmap = nullptr;

KviAnimatedPixmapCache::KviAnimatedPixmapCache()
{
	m_pInstance = this;
	m_animationTimer.setInterval(FRAME_DELAY);
	connect(&m_animationTimer, SIGNAL(timeout()), this, SLOT(timeoutEvent()));
}

KviAnimatedPixmapCache::~KviAnimatedPixmapCache()
{
	if(g_pDummyPixmap)
	{
		delete g_pDummyPixmap;
		g_pDummyPixmap = nullptr;
	}
	m_pInstance = nullptr;
}

void KviAnimatedPixmapCache::init()
{
	if(m_pInstance)
		return;
	m_pInstance = new KviAnimatedPixmapCache();
}

void KviAnimatedPixmapCache::done()
{
	if(!m_pInstance)
		return;
	delete m_pInstance;
	m_pInstance = nullptr;
}

KviAnimatedPixmapCache::Data * KviAnimatedPixmapCache::internalLoad(const QString & szFile, int iWidth, int iHeight)
{
	m_cacheMutex.lock();
	Data * newData = nullptr;

	QMultiHash<QString, Data *>::iterator i = m_hCache.find(szFile);
	while(i != m_hCache.end() && i.key() == szFile && !newData)
	{
		if(!i.value()->resized)
			newData = i.value();
		++i;
	}

	if(!newData)
	{
		newData = new Data(szFile);
		QImageReader reader(szFile);
		newData->size = reader.size();

		QImage buffer;
		QPixmap * framePixmap;
		while(reader.canRead())
		{
			uint delay = reader.nextImageDelay();
			reader.read(&buffer);
			if(!buffer.isNull())
			{
				if(iHeight && iWidth)
					framePixmap = new QPixmap(QPixmap::fromImage(buffer).scaled(iWidth, iHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation));
				else
					framePixmap = new QPixmap(QPixmap::fromImage(buffer));
				newData->append(FrameInfo(framePixmap, delay));
			}
		}
		m_hCache.insert(szFile, newData);
	}
	newData->refs++;

	m_cacheMutex.unlock();

	return newData;
}

KviAnimatedPixmapCache::Data * KviAnimatedPixmapCache::internalResize(Data * data, const QSize & size)
{
	m_cacheMutex.lock();

	bool hasToBeResized = false;

	Data * newData = nullptr;
	QMultiHash<QString, Data *>::iterator i = m_hCache.find(data->file);
	while(i != m_hCache.end() && i.key() == data->file && !newData)
	{
		if(i.value()->size == size)
		{
			newData = i.value();
		}
		++i;
	}

	if(!newData)
	{
		newData = new Data(*data);
		newData->size = size;
		m_hCache.insert(newData->file, newData);
		hasToBeResized = true;
		newData->resized = true;
	}

	newData->refs++;

	m_cacheMutex.unlock();

	internalFree(data);

	if(hasToBeResized)
	{
		for(int i = 0; i < newData->count(); i++)
		{
			QPixmap * old = newData->at(i).pixmap;
			newData->operator[](i).pixmap = new QPixmap(old->scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
			delete old;
		}
	}

	return newData;
}

void KviAnimatedPixmapCache::internalFree(Data * data)
{
	if(!data)
		return;
	m_cacheMutex.lock();
	data->refs--;
	if(data->refs == 0)
	{
		m_hCache.remove(data->file, data);
		for(int i = 0; i < data->count(); i++)
		{
			delete data->operator[](i).pixmap;
		}
		delete data;
	}
	m_cacheMutex.unlock();
}

void KviAnimatedPixmapCache::internalScheduleFrameChange(uint delay, KviAnimatedPixmapInterface * receiver)
{
	//qDebug("Adding %i - %i",(uint)KviTimeUtils::getCurrentTimeMills()+delay,receiver);
	m_timerMutex.lock();
	long long when = KviTimeUtils::getCurrentTimeMills() + delay;

	m_timerData.insert(when, receiver);

	if(!m_animationTimer.isActive())
		m_animationTimer.start();

	m_timerMutex.unlock();
}

void KviAnimatedPixmapCache::timeoutEvent()
{
	/*
	* We are adding 15msecs to the current time. This MAY lead to the situation,
	* when the current frame will be painted a bit earlier, then I should.
	* But we are just playing animated gifs, not a HDTV video. So it should be OK.
	*
	* But it makes good speedup if there will be event, scheduled in such order:
	* 1 event  at time X
	* 3 events at time X+2msec
	* 2 events at time X+6msec
	* 1 event  at time X+8msec
	* 1 event  at time X+9msec
	* 1 event  at time X+12msec
	* ...
	*
	* Such situation is possible if we will have lots of smiles with almost the same
	* frame delays.
	*
	* So it will not emit additional 5 events.
	*/
	long long now = KviTimeUtils::getCurrentTimeMills() + FRAME_DELAY;

	//m_timerMutex.lock();

	QMultiMap<long long, KviAnimatedPixmapInterface *>::iterator i = m_timerData.begin();

	QList<KviAnimatedPixmapInterface *> processed;
	while(i != m_timerData.end() && i.key() <= now)
	{
		if(processed.contains(i.value()))
		{
			// increase the frame index but do not emit the signals as we'll be emitting it later
			i.value()->nextFrame(false);
		}
		else
		{
			// we'll be emitting the signal later
			processed.append(i.value());
		}

		i = m_timerData.erase(i);
	}

	for(auto i : processed)
	{
		// increase the frame index and emit the signal
		i->nextFrame(true);
	}

	// m_timerMutex.unlock();

	if(m_timerData.empty())
		m_animationTimer.stop();
}

QPixmap * KviAnimatedPixmapCache::dummyPixmap()
{
	if(!g_pDummyPixmap)
		g_pDummyPixmap = new QPixmap();
	return g_pDummyPixmap;
}

void KviAnimatedPixmapCache::internalNotifyDelete(
    KviAnimatedPixmapInterface * receiver)
{
	m_timerMutex.lock();

	QMultiMap<long long, KviAnimatedPixmapInterface *>::iterator i = m_timerData.begin();

	while(i != m_timerData.end())
	{
		if(i.value() == receiver)
		{
			i = m_timerData.erase(i);
		}
		else
		{
			++i;
		}
	}

	m_timerMutex.unlock();
}
