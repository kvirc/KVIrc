//=============================================================================
//
//   File : kvi_imagelib.cpp
//   Creation date : Wed Jul 21 1999 16:41:26 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#define __KVILIB__

#include <qnamespace.h>

#include "kvi_imagelib.h"
#include "kvi_fileutils.h"
#include "kvi_locale.h"
#include "kvi_memmove.h"

#include <qpixmap.h>


//KviImageLibrary::KviImageLibrary(const QPixmap &pixmap,int imageWidth,int imageHeight)
//{
//	if(pixmap.isNull())m_pLibrary=0;
//	else m_pLibrary=new QPixmap(pixmap);
//	setImageSize(imageWidth,imageHeight);
//}

KviImageLibrary::KviImageLibrary(const QString &path,int imageWidth,int imageHeight)
{
	m_pLibrary=0;
	loadLibrary(path);
	setImageSize(imageWidth,imageHeight);
}

KviImageLibrary::KviImageLibrary(const QString &path1,const QString &path2,int imageWidth,int imageHeight)
{
	m_pLibrary=0;
	if(KviFileUtils::fileExists(path1))
	{
		loadLibrary(path1);
		if(m_pLibrary==0)loadLibrary(path2);
	} else loadLibrary(path2);
	setImageSize(imageWidth,imageHeight);
}


KviImageLibrary::~KviImageLibrary()
{
	unloadLibrary();
}

void KviImageLibrary::unloadLibrary()
{
	if(m_pLibrary)delete m_pLibrary;
	m_pLibrary=0;
}

bool KviImageLibrary::setImageSize(int imageWidth,int imageHeight)
{
	m_iWidth=((imageWidth>0) ? imageWidth : 16);
	m_iHeight=((imageHeight>0) ? imageHeight : 16);
	return true;
}

bool KviImageLibrary::loadLibrary(const QString &path)
{
	if(m_pLibrary)delete m_pLibrary;
	m_pLibrary=new QImage(path);
	if(m_pLibrary->isNull())
	{
		delete m_pLibrary;
		m_pLibrary=0;
		debug("WARNING : Can not load image library %s",KviQString::toUtf8(path).data());
	}
	return (m_pLibrary != 0);
}

int KviImageLibrary::imageCount()
{
	if(!m_pLibrary)return 0;
	if((m_iWidth<1)||(m_iHeight<1))return 0;
	int nRows=m_pLibrary->width()/m_iWidth;
	return ( nRows * (m_pLibrary->height()/m_iHeight));
}

QPixmap KviImageLibrary::getImage(int zeroBasedIndex)
{
	if((zeroBasedIndex >= imageCount())||(zeroBasedIndex < 0)||(m_pLibrary->depth() < 8))
	{
		QPixmap image(32,32);
		image.fill(); //White fill
		return image;
	}

	// Im per row is not zero...because imageCount returned non zero.
	int imPerRow=(m_pLibrary->width() / m_iWidth);
	int xOffset=(zeroBasedIndex % imPerRow) * m_iWidth;
	int yOffset=(zeroBasedIndex / imPerRow) * m_iHeight;

#ifdef COMPILE_USE_QT4
	QImage image(m_iWidth,m_iHeight,m_pLibrary->format());
#else
	QImage image(m_iWidth,m_iHeight,m_pLibrary->depth());
#endif

	int d = image.depth() / 8;
#ifndef COMPILE_USE_QT4
	if(d == 4)image.setAlphaBuffer(true); // Qt 4.x should manage it automagically
#endif
	//Copy the image data
	//bitBlt(&image,0,0,m_pLibrary,xOffset,yOffset,m_iWidth,m_iHeight,Qt::CopyROP,false);

	for(int i=0;i<m_iHeight;i++)
		kvi_memmove(image.scanLine(i),m_pLibrary->scanLine(i + yOffset) + (xOffset * d),m_iWidth * d);

#ifdef COMPILE_USE_QT4
	QPixmap p = QPixmap::fromImage(image);
#else
	QPixmap p(image);
#endif
	return p;
}
