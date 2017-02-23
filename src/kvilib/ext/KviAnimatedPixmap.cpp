//=============================================================================
//
//   File : KviAnimatedPixmap.cpp
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

#include "KviAnimatedPixmap.h"

#include <QImageReader>

KviAnimatedPixmap::KviAnimatedPixmap(QString fileName, int iWidth, int iHeight)
    : QObject(),
      m_szFileName(fileName),
      m_uCurrentFrameNumber(0),
      m_iStarted(0)
{
	m_pFrameData = KviAnimatedPixmapCache::load(fileName, iWidth, iHeight);
}

KviAnimatedPixmap::KviAnimatedPixmap(const KviAnimatedPixmap & source)
    : QObject(),
      m_szFileName(source.m_szFileName),
      m_pFrameData(source.m_pFrameData),
      m_uCurrentFrameNumber(source.m_uCurrentFrameNumber),
      m_iStarted(0)
{
	m_pFrameData->refs++;
}

KviAnimatedPixmap::~KviAnimatedPixmap()
{
	KviAnimatedPixmapCache::notifyDelete(this);
	KviAnimatedPixmapCache::free(m_pFrameData);
}

void KviAnimatedPixmap::start()
{
	m_iStarted++;

	if(m_iStarted > 1)
		return; // was already started

	if(m_pFrameData->count() < 2)
		return;

	m_uCurrentFrameNumber = 0;

	KviAnimatedPixmapCache::scheduleFrameChange(m_pFrameData->at(m_uCurrentFrameNumber).delay, this);
}

void KviAnimatedPixmap::stop()
{
	if(!m_iStarted)
		return;

	m_iStarted--;
	if(m_iStarted < 0)
		m_iStarted = 0;
}

void KviAnimatedPixmap::nextFrame(bool bEmitSignalAndScheduleNext)
{
	if(m_iStarted < 1)
		return;

	m_uCurrentFrameNumber++;
	//Ensure, that we are not out of bounds
	m_uCurrentFrameNumber %= m_pFrameData->count();

	if(!bEmitSignalAndScheduleNext)
		return;

	if(m_iStarted)
		emit frameChanged();

	KviAnimatedPixmapCache::scheduleFrameChange(m_pFrameData->at(m_uCurrentFrameNumber).delay, this);
}

void KviAnimatedPixmap::resize(QSize newSize, Qt::AspectRatioMode ratioMode)
{
	QSize curSize(size());
	curSize.scale(newSize, ratioMode);

	m_pFrameData = KviAnimatedPixmapCache::resize(m_pFrameData, curSize);
}
