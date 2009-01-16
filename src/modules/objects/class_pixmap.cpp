//=============================================================================
//
//   File : class_pixmap.cpp
//   Creation date : Fri Mar 18 14:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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
//=============================================================================

#include "class_pixmap.h"
#include "class_memorybuffer.h"

#include "kvi_debug.h"
#include "kvi_locale.h"

#include <QFile>

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
		!fn: $scale(<width:integer>,<height:integer>,[<aspect_ratio:string>,<transformation_type:string>])
		Scales the pixmap by sx horizontally and sy vertically.
		Aspect_ratio values:
		- IgnoreAspectRatio:the pixmap is scaled ignoring his aspect ratio.
		- KeepAspectRatio: pixmap is scaled to a rectangle as large as possible inside size, preserving the aspect ratio.
		- KeepAspectRatioByExpanding, the pixmap is scaled to a rectangle as small as possible outside size, preserving the aspect ratio.
		Transformation type values:
		- Fast: fast transformation but less quality.
		- Smooth: more quality using bilinear filtering.
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

	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_pixmap,fill)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_pixmap,resize)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_pixmap,load)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_pixmap,loadFromMemoryBuffer)

	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_pixmap,height)

	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_pixmap,width)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_pixmap,setOpacity)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_pixmap,scale)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_pixmap,rotate)

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


KVSO_CLASS_FUNCTION(pixmap,fill)
{
	CHECK_INTERNAL_POINTER(m_pPixmap)
	KviKvsObject *pObject;
	kvs_int_t iXoffset,iYoffset;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("x_offset",KVS_PT_INT,0,iXoffset)
		KVSO_PARAMETER("y_offset",KVS_PT_INT,0,iYoffset)
	KVSO_PARAMETERS_END(c)

	pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	bPixmapModified=true;
	m_pPixmap->fill(((QWidget *)(pObject->object())),iXoffset,iYoffset);
	return true;
}

KVSO_CLASS_FUNCTION(pixmap,scale)
{
	CHECK_INTERNAL_POINTER(m_pPixmap)
	kvs_int_t iScaleW,iScaleH;
	QString szAspectRatio, szTransformation;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("new_width",KVS_PT_INTEGER,0,iScaleW)
		KVSO_PARAMETER("new_height",KVS_PT_INTEGER,0,iScaleH)
		KVSO_PARAMETER("aspect_ratio",KVS_PT_STRING,KVS_PF_OPTIONAL,szAspectRatio)
		KVSO_PARAMETER("tranformation_type",KVS_PT_STRING,KVS_PF_OPTIONAL,szTransformation)
	KVSO_PARAMETERS_END(c)
	Qt::AspectRatioMode ratio=Qt::IgnoreAspectRatio;
	if (KviQString::equalCI(szAspectRatio,"IgnoreAspectRatio")) ratio=Qt::IgnoreAspectRatio;
	else if (KviQString::equalCI(szAspectRatio,"KeepAspectRatio")) ratio=Qt::KeepAspectRatio;
	else if (KviQString::equalCI(szAspectRatio,"KeepAspectRatioByExpanding")) ratio=Qt::KeepAspectRatioByExpanding;
	else if (!szAspectRatio.isEmpty())c->warning(__tr2qs_ctx("Unknown aspect ratio: using default value IgnoreAspectRatio","objects"));
	Qt::TransformationMode tmode=Qt::FastTransformation;
	if (KviQString::equalCI(szTransformation,"Fast")) tmode=Qt::FastTransformation;
	else if (KviQString::equalCI(szTransformation,"Smooth")) tmode=Qt::SmoothTransformation;
	else if (!szAspectRatio.isEmpty())c->warning(__tr2qs_ctx("Unknown tranformation mode: using default value Fast","objects"));

	*m_pPixmap = m_pPixmap->scaled(iScaleW, iScaleH,ratio,tmode);
	bPixmapModified=true;
	return true;
}

KVSO_CLASS_FUNCTION(pixmap,rotate)
{
	CHECK_INTERNAL_POINTER(m_pPixmap)
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

KVSO_CLASS_FUNCTION(pixmap,resize)
{
	CHECK_INTERNAL_POINTER(m_pPixmap)
	kvs_int_t uWidth,uHeight;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("width",KVS_PT_UNSIGNEDINTEGER,0,uWidth)
		KVSO_PARAMETER("height",KVS_PT_UNSIGNEDINTEGER,0,uHeight)
	KVSO_PARAMETERS_END(c)
	delete m_pPixmap;
	m_pPixmap=new QPixmap(uWidth,uHeight);
	bPixmapModified=true;
	return true;
}


KVSO_CLASS_FUNCTION(pixmap,load)
{
	CHECK_INTERNAL_POINTER(m_pPixmap)
	QString szFile;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("file",KVS_PT_STRING,0,szFile)
	KVSO_PARAMETERS_END(c)
	if(!QFile::exists(szFile))
	{
		c->warning(__tr2qs_ctx("I can't find the specified file '%Q'.","objects"),&szFile);
        return true;
	}
	m_pPixmap->load(szFile);
	bPixmapModified=true;
	return true;
}
KVSO_CLASS_FUNCTION(pixmap,loadFromMemoryBuffer)
{
	CHECK_INTERNAL_POINTER(m_pPixmap)
	KviKvsObject * pObject;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("hobject",KVS_PT_HOBJECT,0,hObject)
	KVSO_PARAMETERS_END(c)
	pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if (!pObject)
	{
		c->warning(__tr2qs_ctx("Buffer parameter is not an object","objects"));
		return true;
	}
	if (!pObject->inheritsClass("memorybuffer"))
	{
		c->warning(__tr2qs_ctx("Buffer parameter is not a memorybuffer object","objects"));
		return true;
	}

	m_pPixmap->loadFromData(((KviKvsObject_memorybuffer *)pObject)->dataBuffer());

	bPixmapModified=true;

	return true;
}

KVSO_CLASS_FUNCTION(pixmap,height)
{
	CHECK_INTERNAL_POINTER(m_pPixmap)
	c->returnValue()->setInteger(m_pPixmap->height());
	return true;
}
KVSO_CLASS_FUNCTION(pixmap,width)
{
	CHECK_INTERNAL_POINTER(m_pPixmap)
	c->returnValue()->setInteger(m_pPixmap->width());
	return true;
}
// maybe DEPRECATED?
KVSO_CLASS_FUNCTION(pixmap,setOpacity)
{
	CHECK_INTERNAL_POINTER(m_pPixmap)
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
		c->warning(__tr2qs_ctx("Destination parameter is not an object","objects"));
		return true;
	}

	if(!pObDest->inheritsClass("pixmap"))
	{
		c->warning(__tr2qs_ctx("Destination must be a pixmap object","objects"));
		return true;
	}
	QImage *buffer=((KviKvsObject_pixmap *)pObDest)->getImage();
	((KviKvsObject_pixmap *)pObDest)->imageChanged();
	if (bPixmapModified) {
		*m_pImage=m_pPixmap->toImage();
	}
	if (uWidth > (uint) buffer->width() || uHeight > (uint) buffer->height())
	{
			c->warning(__tr2qs_ctx("Area dimensions are out of destination size ","objects"));
			return true;
	}
	if (!uWidth)
	{
		if(m_pImage->width()>buffer->width())
		{
			c->warning(__tr2qs_ctx("Pixmap dimensions are out of destination size ","objects"));
			return true;
		}
	}

	if (!uHeight)
	{
		if(m_pImage->height()>buffer->height())
		{
			c->warning(__tr2qs_ctx("Pixmap dimensions are out of destination size ","objects"));
			return true;
		}
	}

	if(uXoffset + uWidth > (uint) m_pImage->width())
	{
		c->warning(__tr2qs_ctx("Offset width area is out of pixmap size ","objects"));
		return true;
	}
	if(uYoffset + uHeight > (uint) m_pImage->height())
	{
		c->warning(__tr2qs_ctx("Offset height area is out of pixmap size ","objects"));
		return true;
	}

//	buffer->setAlphaBuffer(true);
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
	if (bImageModified)
	{
		m_pPixmap->fromImage(*m_pImage);
		bImageModified=false;
	}
	return m_pPixmap;
}
QImage * KviKvsObject_pixmap::getImage()
{
	if (bPixmapModified)
	{
		*m_pImage=m_pPixmap->toImage();
		bPixmapModified=false;
	}

	return m_pImage;
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_pixmap.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

