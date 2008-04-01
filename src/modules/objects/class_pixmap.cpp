//mdm:
//   Painter : class_pixmap.h
//   Creation date : Fri Mar 18 14:30:48 CEST 2005 
//	 by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//   Lucia Papini (^ashura^)  English Translation.
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

#include "class_pixmap.h"
#include "kvi_debug.h"

#include "kvi_locale.h"
#include <qfile.h>

/*
	@doc:	pixmap
	@keyterms:
	    pixmap object class
	@title:
		pixmap class
	@type:
		class
	@short:
		This class provide a pixmap.
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		The pixmap class is an off-screen, pixel-based paint device.		
	@functions:
		!fn: $fill(<widget:object>,<x_offset:integer>,<y:offset>)
		Fills the pixmap with the widget's background color or pixmap.[br]
		Note that x_offset, y_offest are offsets in the widget.
		!fn: $resize(<width:integer>,<height:integer>)
		Resizes the pixmap to w width and h height. Set wh or hg to 0, to have a null pixmap.
		!fn: $scale(<sx:real>,<sy:real>)
		Scales the pixmap by sx horizontally and sy vertically.
		!fn: $rotate(<a:real>)
		Rotates the pixmap by a degrees.
		!fn: $load(<file_name:string>)
		Load a pixmap from the <file>.
		!fn: <integer> $height()
		Return the height of the pixmap.
		!fn: <integer> $width()
		Return the width of the pixmap.

*/


KVSO_BEGIN_REGISTERCLASS(KviKvsObject_pixmap,"pixmap","object")

	KVSO_REGISTER_HANDLER(KviKvsObject_pixmap,"fill",functionfill)
	KVSO_REGISTER_HANDLER(KviKvsObject_pixmap,"resize",functionresize)
	KVSO_REGISTER_HANDLER(KviKvsObject_pixmap,"load",functionload)
	KVSO_REGISTER_HANDLER(KviKvsObject_pixmap,"height",functionheight)
	KVSO_REGISTER_HANDLER(KviKvsObject_pixmap,"width",functionwidth)
	KVSO_REGISTER_HANDLER(KviKvsObject_pixmap,"setOpacity",functionsetOpacity)
	KVSO_REGISTER_HANDLER(KviKvsObject_pixmap,"scale",functionscale)
	KVSO_REGISTER_HANDLER(KviKvsObject_pixmap,"rotate",functionrotate)

KVSO_END_REGISTERCLASS(KviKvsObject_pixmap)


KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_pixmap,KviKvsObject)	
	m_pPixmap=new QPixmap();
	m_pImage=new QImage();
	bPixmapModified=false;
	bImageModified=false;
KVSO_END_CONSTRUCTOR(KviKvsObject_pixmap)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_pixmap)
	
	emit aboutToDie();
	delete m_pPixmap;
	if (m_pImage) delete m_pImage;
	
KVSO_END_CONSTRUCTOR(KviKvsObject_pixmap)


bool KviKvsObject_pixmap::functionfill(KviKvsObjectFunctionCall *c)
{
	KviKvsObject *ob;
	kvs_int_t iXoffset,iYoffset;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("x_offset",KVS_PT_INT,0,iXoffset)
		KVSO_PARAMETER("y_offset",KVS_PT_INT,0,iYoffset)
	KVSO_PARAMETERS_END(c)
	
	ob=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if (!ob)
	{
		c->warning(__tr2qs("Widget parameter is not an object"));
		return true;
	}
	if(!ob->object()->isWidgetType())
	{
		c->warning(__tr2qs("Widget object required"));
		return true;
	}
	bPixmapModified=true;
	m_pPixmap->fill(((QWidget *)(ob->object())),iXoffset,iYoffset);
	return true;
}

bool KviKvsObject_pixmap::functionscale(KviKvsObjectFunctionCall *c)
{
	kvs_real_t uScaleX,uScaleY;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("sx",KVS_PT_REAL,0,uScaleX)
		KVSO_PARAMETER("sy",KVS_PT_REAL,0,uScaleY)
	KVSO_PARAMETERS_END(c)
	*m_pPixmap = m_pPixmap->scaled((m_pPixmap->width() * uScaleX), (m_pPixmap->height() * uScaleX));
	bPixmapModified=true;
	return true;
}
  
bool KviKvsObject_pixmap::functionrotate(KviKvsObjectFunctionCall *c)
{
	kvs_real_t uDegrees;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("a",KVS_PT_REAL,0,uDegrees)
	KVSO_PARAMETERS_END(c)
	QMatrix m;
	m.rotate(uDegrees);
	*m_pPixmap = m_pPixmap->transformed(m);
	bPixmapModified=true;
	return true;
}

bool KviKvsObject_pixmap::functionresize(KviKvsObjectFunctionCall *c)
{
	kvs_int_t uWidth,uHeight;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("width",KVS_PT_UNSIGNEDINTEGER,0,uWidth)
		KVSO_PARAMETER("height",KVS_PT_UNSIGNEDINTEGER,0,uHeight)
	KVSO_PARAMETERS_END(c)
	m_pPixmap->resize(uWidth,uHeight);
	bPixmapModified=true;
	return true;
}


bool KviKvsObject_pixmap::functionload(KviKvsObjectFunctionCall *c)
{
	QString szFile;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("file",KVS_PT_STRING,0,szFile)
	KVSO_PARAMETERS_END(c)
	
	if(!QFile::exists(szFile))
	{
		c->warning(__tr2qs("I can't find the specified file %Q."),&szFile);
        return true;
	}
	m_pPixmap->load(szFile);
	bPixmapModified=true;
	return true;
}
bool KviKvsObject_pixmap::functionheight(KviKvsObjectFunctionCall *c)
{
	c->returnValue()->setInteger(m_pPixmap->height());	
	return true;
}
bool KviKvsObject_pixmap::functionwidth(KviKvsObjectFunctionCall *c)
{
	c->returnValue()->setInteger(m_pPixmap->width());	
	return true;
}
bool KviKvsObject_pixmap::functionsetOpacity(KviKvsObjectFunctionCall *c)
{
	if(!m_pPixmap)return true; 
	kvs_real_t dOpacity;
	kvs_uint_t uXoffset,uYoffset;
	kvs_uint_t uWidth,uHeight;

	KviKvsObject * pObDest;
	kvs_hobject_t hObject;
		
	KVSO_PARAMETERS_BEGIN(c)
			KVSO_PARAMETER("opacity_factor",KVS_PT_DOUBLE,0,dOpacity)	
			KVSO_PARAMETER("destination",KVS_PT_HOBJECT,0,hObject)

			KVSO_PARAMETER("x_offset",KVS_PT_UNSIGNEDINTEGER,KVS_PF_OPTIONAL,uXoffset)
			KVSO_PARAMETER("y_offset",KVS_PT_UNSIGNEDINTEGER,KVS_PF_OPTIONAL,uYoffset)
			KVSO_PARAMETER("width",KVS_PT_UNSIGNEDINTEGER,KVS_PF_OPTIONAL,uWidth)
			KVSO_PARAMETER("height",KVS_PT_UNSIGNEDINTEGER,KVS_PF_OPTIONAL,uHeight)
	KVSO_PARAMETERS_END(c)
	pObDest=KviKvsKernel::instance()->objectController()->lookupObject(hObject);

	if (!pObDest)
	{
		c->warning(__tr2qs("Destination  parameter is not an object"));
		return true;
	}

	if(!pObDest->inherits("KviKvsObject_pixmap"))
	{
		c->warning(__tr2qs("Destination must be a pixmap object"));
		return true;
	}
	QImage *buffer=((KviKvsObject_pixmap *)pObDest)->getImage();
	((KviKvsObject_pixmap *)pObDest)->imageChanged();
	if (bPixmapModified) {
		*m_pImage=m_pPixmap->convertToImage();
	}
	if (uWidth>buffer->width() || uHeight>buffer->height())
	{
			c->warning(__tr2qs("Area dimensions are out of destination size "));
			return true;
	}
	if (!uWidth){
		if(m_pImage->width()>buffer->width())
		{
			c->warning(__tr2qs("Pixmap dimensions are out of destination size "));
			return true;
		}
	}

	if (!uHeight)
	{
		if(m_pImage->height()>buffer->height())
		{
			c->warning(__tr2qs("Pixmap dimensions are out of destination size "));
			return true;
		}
	}


if(uXoffset+uWidth>m_pImage->width())
	{
		c->warning(__tr2qs("Offset width area is out of pixmap size "));
		return true;
	}
	if( uYoffset+uHeight>m_pImage->height())
	{
		c->warning(__tr2qs("Offset height area is out of pixmap size "));
		return true;
	}

	buffer->setAlphaBuffer(true);
	int iHedge=uHeight?uHeight:m_pImage->height();
	int iWedge=uWidth?uWidth:m_pImage->width();
		
	for(int y = uYoffset;y<iHedge;y++)
	{
		QRgb * dst = (QRgb *)buffer->scanLine(y);
		QRgb * src = (QRgb *)m_pImage->scanLine(y);
		src += uXoffset;
		QRgb * end;
		end  = src + iWedge;
		while(src < end)
		{
			*dst = qRgba(
						(int)(qRed(*src)),
						(int)(qGreen(*src)),
						(int)(qBlue(*src)),
						(int)(qAlpha(*src)*dOpacity)
						);
			src++;
			dst++;
		}
	}
	return true;
}


QPixmap * KviKvsObject_pixmap::getPixmap() 
{
	if (bImageModified) {
		m_pPixmap->convertFromImage(*m_pImage);
		bImageModified=false;
	}
	return m_pPixmap;
}
QImage * KviKvsObject_pixmap::getImage()
{
	if (bPixmapModified) {
		*m_pImage=m_pPixmap->convertToImage();
		//debug ("image info2  %d and %d",test.width(),test.height());
		
		bPixmapModified=false;
	}
	
	return m_pImage;
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_pixmap.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

