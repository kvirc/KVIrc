//mdm:
//   Image : class_image.h
//   Creation date : Fri Jan 13 18:30:48 CEST 2005 
//	 by Alessandro Carbone(Noldor) and Tonino Imbesi(Grifisx)
//   This file is part of the KVirc irc client distribution
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

#include "class_image.h"
#include "kvi_debug.h"

#include "kvi_locale.h"
#include <qfile.h>

/*
	@doc:	image
	@keyterms:
	    image object class
	@title:
		image class
	@type:
		class
	@short:
	@inherits:
		[class]object[/class]
	@description:
		Image class provides functions that can modify the image itself.
	@functions:
	!fn: $load(<file_name:string>)
	Load a image from the <file>.
	!fn: <integer> $height()
	Return the height of the image.
	!fn: <integer> $width()
	Return the width of the image.
	!fn: $setOpacity(opacity_value:integer)
	Set the image opacity; valid value are:from 0 (full transparency) to 255 (full opacity).
	*/


//===========================================================================================

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_image,"image","object")

	KVSO_REGISTER_HANDLER(KviKvsObject_image,"load",functionload)
	KVSO_REGISTER_HANDLER(KviKvsObject_image,"height",functionheight)
	KVSO_REGISTER_HANDLER(KviKvsObject_image,"width",functionwidth)
	KVSO_REGISTER_HANDLER(KviKvsObject_image,"setOpacity",functionsetOpacity)

KVSO_END_REGISTERCLASS(KviKvsObject_image)


KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_image,KviKvsObject)

	m_pImage = new QImage(341,341,32);

KVSO_END_CONSTRUCTOR(KviKvsObject_image)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_image)
	
	if (m_pImage) delete m_pImage;
KVSO_END_CONSTRUCTOR(KviKvsObject_image)


bool KviKvsObject_image::functionload(KviKvsObjectFunctionCall *c)
{
	QString szFile;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("file",KVS_PT_STRING,0,szFile)
	KVSO_PARAMETERS_END(c)
	
	if(!m_pImage)return true; 
	if(!QFile::exists(szFile))
	{
		c->warning(__tr2qs("I can't find the specified file %Q."),&szFile);
        return true;
	}

	m_pImage->load( szFile );
	return true;
}

bool KviKvsObject_image::functionsetOpacity(KviKvsObjectFunctionCall *c)
{
	if(!m_pImage)return true; 
	kvs_uint_t iOpacity;

	KVSO_PARAMETERS_BEGIN(c)
			KVSO_PARAMETER("opacity_value",KVS_PT_INT,0,iOpacity)	
	KVSO_PARAMETERS_END(c)
	if (!m_pImage->hasAlphaBuffer()) m_pImage->setAlphaBuffer(true);
	for(int y = 0;y < m_pImage->height();y++)
	{
		QRgb * dst = (QRgb *)m_pImage->scanLine(y);
		QRgb * end = dst + m_pImage->width();		
		while(dst < end)
		{
			if (qRgba((int)(qRed(*dst)),(int)(qGreen(*dst)),(int)(qBlue(*dst)),(int)(qAlpha(*dst))))
			{
				*dst = qRgba((int)(qRed(*dst)),
							(int)(qGreen(*dst)),
							(int)(qBlue(*dst)),
							(int)(iOpacity));
			}
			dst++;
		}

	}
		return true;
	}

bool KviKvsObject_image::functionheight(KviKvsObjectFunctionCall *c)
{
	if(!m_pImage)return true; 
	c->returnValue()->setInteger(m_pImage->height());	
	return true;
}
bool KviKvsObject_image::functionwidth(KviKvsObjectFunctionCall *c)
{
	if(!m_pImage)return true; 
	c->returnValue()->setInteger(m_pImage->width());	
	return true;
}
QImage KviKvsObject_image::getImage() const
{
	QImage ret(*m_pImage);
	return ret;
}
#include "m_class_image.moc"
