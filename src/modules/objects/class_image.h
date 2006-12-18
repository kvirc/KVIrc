#ifndef	_CLASS_IMAGE_H_
#define	_CLASS_IMAGE_H_
//vim: ts=8
//   Image : class_image.h
//   by Alessandro Carbone (Noldor) and Tonino Imbesi(Grifisx)
//   Creation date : Fri Jan 13 18:43:01 CEST 2006  
//
//   This pixmap is part of the KVirc irc client distribution
//   Copyright (C) 1999-2006 Szymon Stefanek (pragma at kvirc dot net)
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



#include <qimage.h>


#include "object_macros.h"


class KviKvsObject_image : public KviKvsObject
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_image);
public:
	QImage getImage() const;
	QImage * image(){ return m_pImage; }
protected:
	QImage * m_pImage;

protected:
	bool functionload(KviKvsObjectFunctionCall *c);
	bool functionheight(KviKvsObjectFunctionCall *c);
	bool functionwidth(KviKvsObjectFunctionCall *c);
	bool functionsetOpacity(KviKvsObjectFunctionCall *c);
	

  };

#endif	// !_CLASS_IMAGE_H_
