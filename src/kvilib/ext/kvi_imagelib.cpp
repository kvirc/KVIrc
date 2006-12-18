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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
#define __KVILIB__



//#define _KVI_DEBUG_CLASS_NAME_ "KviImageLibrary"
#define _KVI_DEBUG_CHECK_RANGE_
#include "kvi_debug.h"

#include <qnamespace.h>

#include "kvi_imagelib.h"
#include "kvi_fileutils.h"
#include "kvi_locale.h"
#include "kvi_memmove.h"


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
	__range_valid(imageWidth>0);
	__range_valid(imageHeight>0);
	m_iWidth=((imageWidth>0) ? imageWidth : 16);
	m_iHeight=((imageHeight>0) ? imageHeight : 16);
	return true;
}

bool KviImageLibrary::loadLibrary(const QString &path)
{
	__range_valid(path);
	if(m_pLibrary)delete m_pLibrary;
	m_pLibrary=new QImage(path);
	if(m_pLibrary->isNull())
	{
		delete m_pLibrary;
		m_pLibrary=0;
		debug(__tr("WARNING : Can not load image library %s"),path.latin1());
	}
// else {
//		if(!m_pLibrary->mask())m_pLibrary->setMask(m_pLibrary->createHeuristicMask());
//	}
	return (m_pLibrary != 0);
}

//void KviImageLibrary::forceMask()
//{
//	if(m_pLibrary){
//		if(!m_pLibrary->mask())m_pLibrary->setMask(m_pLibrary->createHeuristicMask());
//	}
//}

int KviImageLibrary::imageCount()
{
	if(!m_pLibrary)return 0;
	if((m_iWidth<1)||(m_iHeight<1))return 0;
	int nRows=m_pLibrary->width()/m_iWidth;
	return ( nRows * (m_pLibrary->height()/m_iHeight));
}

QPixmap KviImageLibrary::getImage(int zeroBasedIndex)
{
	__range_valid(zeroBasedIndex >=0);
	__range_valid(zeroBasedIndex < imageCount());

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

	QImage image(m_iWidth,m_iHeight,m_pLibrary->depth());
	//Copy the image data
	//bitBlt(&image,0,0,m_pLibrary,xOffset,yOffset,m_iWidth,m_iHeight,Qt::CopyROP,false);
	int d = image.depth() / 8;
	if(d == 4)image.setAlphaBuffer(true);

//	debug("IMAGE DEPTH IS %d",d);
	for(int i=0;i<m_iHeight;i++)
		kvi_memmove(image.scanLine(i),m_pLibrary->scanLine(i + yOffset) + (xOffset * d),m_iWidth * d);
/*
	if(m_pLibrary->mask())
	{
		//copy the mask too
		QBitmap bmp(m_iWidth,m_iHeight);
		bitBlt(&bmp,0,0,m_pLibrary->mask(),xOffset,yOffset,m_iWidth,m_iHeight,Qt::CopyROP,false);
		image.setMask(bmp);
	}
*/

	QPixmap p(image);

//	debug("PIXMAP DEPTH IS %d",p.depth());

	return p;
}
