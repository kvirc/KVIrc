#ifndef _KVI_IMAGELIB_H_
#define _KVI_IMAGELIB_H_

//
//   File : kvi_imagelib.h
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

//
// A class to manage multiple image libraries.
// The library is in fact a big image that contains
// several rows of other smaller images of the same size.
// The images inside the 'library' are indexed in the following way:
//
// -------------
// |...|...|...|
// |.0.|.1.|.2.|
// -------------
// |...|...|...|
// |.3.|.4.|.5.|
// -------------
// |...|...|...|
//
// The 'library' image should have the size that is a multiple
// of the single image size. If not , the remaining part is ignored.
// The first image starts always in the left upper corner.
//

// WARNING: This class will be removed in the near future. Don't use it.

#include "kvi_settings.h"

#include <qimage.h>

class KVILIB_API KviImageLibrary
{
public:		// Consruction & Destruction
//	KviImageLibrary(const QPixmap &pixmap,int imageWidth,int imageHeight);
	KviImageLibrary(const QString &path,int imageWidth,int imageHeight);
	KviImageLibrary(const QString &path1,const QString &path2,int imageWidth,int imageHeight);
	~KviImageLibrary();
public:	// Fields
	QImage  *m_pLibrary;
	int      m_iWidth;
	int      m_iHeight;
public:	// Methods
	bool loadLibrary(const QString &path);
	void unloadLibrary();
	int imageCount();
	bool libraryLoaded(){ return (m_pLibrary != 0); };
	QImage *getLibrary(){ return m_pLibrary; };
	QPixmap getImage(int zeroBasedIndex);
	bool setImageSize(int imageWidth,int imageHeight);
};

#endif //_KVI_IMAGELIB_H_INCLUDED_
