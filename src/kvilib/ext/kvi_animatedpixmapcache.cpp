/*
 * kvi_animatedpixmapcache.cpp
 *
 *  Created on: 31.07.2008
 *      Author: Alexey
 */

#include <kvi_animatedpixmapcache.h>
#include <QImageReader>
#include <QImage>
#include "kvi_time.h"

static KviAnimatedPixmapCache* g_pAnimatedCache = 0;
static QPixmap* g_pDummyPixmap = 0;

KviAnimatedPixmapCache::KviAnimatedPixmapCache()
{
	g_pAnimatedCache = this;
	m_animationTimer.setSingleShot(true);
	connect(&m_animationTimer,SIGNAL(timeout()),this,SLOT(timerEvent()));
}

KviAnimatedPixmapCache::~KviAnimatedPixmapCache() {
	if(g_pDummyPixmap) {
		delete g_pDummyPixmap;
	}
	g_pAnimatedCache = 0;
}

KviAnimatedPixmapCache::Data* KviAnimatedPixmapCache::internalLoad(QString szFile) {
	m_cacheMutex.lock();

	Data* newData = 0;

	QMultiHash<QString, Data*>::iterator i = m_hCache.find(szFile);
	while (i != m_hCache.end() && i.key() == szFile && !newData) {
		if (!i.value()->resized) {
			newData = i.value();
		}
		++i;
	}

	if (!newData) {
		newData = new Data(szFile);
		QImageReader reader(szFile);
		newData->size = reader.size();

		QImage buffer;
		QPixmap* framePixmap;
		while(reader.canRead())
		{
			uint delay = reader.nextImageDelay();
			reader.read(&buffer);
			if(!buffer.isNull())
			{
				framePixmap = new QPixmap(QPixmap::fromImage(buffer));
				newData->append(FrameInfo(framePixmap,delay));
			}
		}
		m_hCache.insert(szFile,newData);
	}
	newData->refs++;

	m_cacheMutex.unlock();

	return newData;
}
KviAnimatedPixmapCache::Data* KviAnimatedPixmapCache::internalResize(Data* data, QSize size) {
	m_cacheMutex.lock();

	bool hasToBeResized = false;

	Data* newData = 0;
	QMultiHash<QString, Data*>::iterator i = m_hCache.find(data->file);
	while (i != m_hCache.end() && i.key() == data->file && !newData) {
		if (i.value()->size == size) {
			newData = i.value();
		}
		++i;
	}

	if(!newData)
	{
		newData = new Data(*data);
		newData->size = size;
		m_hCache.insert(newData->file,newData);
		hasToBeResized = true;
		newData->resized = true;
	}

	newData->refs++;

	m_cacheMutex.unlock();

	internalFree(data);

	if(hasToBeResized)
	{
		for(int i=0;i<newData->count();i++) {
			QPixmap* old = newData->at(i).pixmap;
			newData->operator [](i).pixmap=new QPixmap(old->scaled(size,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
			delete old;
		}
	}

	return newData;
}

void KviAnimatedPixmapCache::internalFree(Data* data) {
	m_cacheMutex.lock();
	data->refs--;
	if(data->refs==0)
	{
		m_hCache.remove(data->file,data);
		for(int i=0;i<data->count();i++)
		{
			delete data->operator [](i).pixmap;
		}
		delete data;
	}
	m_cacheMutex.unlock();
}

KviAnimatedPixmapCache::Data* KviAnimatedPixmapCache::load(QString szFileName) {
	if (!g_pAnimatedCache) {
		new KviAnimatedPixmapCache();
	}
	return g_pAnimatedCache->internalLoad(szFileName);
}


KviAnimatedPixmapCache::Data* KviAnimatedPixmapCache::resize(KviAnimatedPixmapCache::Data* data, QSize size) {
	if (!g_pAnimatedCache) {
		new KviAnimatedPixmapCache();
	}
	return g_pAnimatedCache->internalResize(data, size);
}


void KviAnimatedPixmapCache::free(KviAnimatedPixmapCache::Data* data) {
	if (!g_pAnimatedCache) {
		new KviAnimatedPixmapCache();
	}
	g_pAnimatedCache->internalFree(data);
}

void  KviAnimatedPixmapCache::internalSceduleFrameChange(uint delay,KviAnimatedPixmapInterface* receiver)
{
  debug("Adding %i - %i",(uint)KviTimeUtils::getCurrentTimeMills()+delay,receiver);
  m_timerMutex.lock();
  bool  needTimerChange = false;
  long long when = KviTimeUtils::getCurrentTimeMills()+delay;

  if(m_timerData.isEmpty())
  {

	m_timerData.insert(when,receiver);
    needTimerChange = true;

  } else {
    long long currentNext = m_timerData.begin().key();

    if(when<currentNext){
    	needTimerChange = true;
    }

    m_timerData.insert(when,receiver);
  }

  if(needTimerChange)
  {
	  m_animationTimer.start(delay);
  }

  m_timerMutex.unlock();
}

void KviAnimatedPixmapCache::timerEvent()
{
  /*
   * We are adding 15msecs to the current time. This MAY lead to the situation,
   * when the current frame will be painted a bit earlier, then i should.
   * But we are just playing animated gifs, not a HDTV video. So it should be ok.
   *
   * But it makes good speedup if there will be event, sceduled in such order:
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
	long long now = KviTimeUtils::getCurrentTimeMills() + 15;

//	m_timerMutex.lock();

	QMultiMap<long long, KviAnimatedPixmapInterface*>::iterator i =
			m_timerData.begin();

	while (i != m_timerData.end() && i.key() <= now)
	{
		i.value()->nextFrame();
		debug("Calling %i - %i",(uint)KviTimeUtils::getCurrentTimeMills(),i.value());
		i = m_timerData.erase(i);
	}

	if (i != m_timerData.end())
	{
		long long nextDelay = i.key();
		uint delay = (uint) (nextDelay - KviTimeUtils::getCurrentTimeMills());
		if (delay < 0)
		{
			delay = 0;
		}
		m_animationTimer.start(delay);
	}
//	m_timerMutex.unlock();
}

void  KviAnimatedPixmapCache::sceduleFrameChange(uint when,KviAnimatedPixmapInterface* receiver)
{
  if (g_pAnimatedCache)
  {
    g_pAnimatedCache->internalSceduleFrameChange(when,receiver);
  }
}

QPixmap* KviAnimatedPixmapCache::dummyPixmap()
{
	if(!g_pDummyPixmap)
	{
		g_pDummyPixmap = new QPixmap();
	}
	return g_pDummyPixmap;
}

void KviAnimatedPixmapCache::internalNotifyDelete(
		KviAnimatedPixmapInterface* receiver)
{
	m_timerMutex.lock();

	QMultiMap<long long, KviAnimatedPixmapInterface*>::iterator i =
			m_timerData.begin();

	while (i != m_timerData.end())
	{
		if (i.value() == receiver)
		{
			i = m_timerData.erase(i);
		} else {
			i++;
		}
	}

	m_timerMutex.unlock();
}

void KviAnimatedPixmapCache::notifyDelete(KviAnimatedPixmapInterface* receiver)
{
	if (g_pAnimatedCache)
	{
		g_pAnimatedCache->internalNotifyDelete(receiver);
	}
}
