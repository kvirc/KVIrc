/*
 * kvi_animatedpixmapcache.cpp
 *
 *  Created on: 31.07.2008
 *      Author: Alexey
 */

#include <kvi_animatedpixmapcache.h>
#include <QImageReader>
#include <QImage>

static KviAnimatedPixmapCache* g_pAnimatedCache = 0;

KviAnimatedPixmapCache::KviAnimatedPixmapCache() {
	g_pAnimatedCache = this;
}

KviAnimatedPixmapCache::~KviAnimatedPixmapCache() {
	// TODO Auto-generated destructor stub
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
