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

KVSO_END_REGISTERCLASS(KviKvsObject_pixmap)


KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_pixmap,KviKvsObject)

	m_pPixmap = new QPixmap();

KVSO_END_CONSTRUCTOR(KviKvsObject_pixmap)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_pixmap)
	
	emit aboutToDie();
	delete m_pPixmap;

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
	if(!m_pPixmap)return true;
	if (!ob)
	{
		c->warning(__tr2qs("Widget parameter is not an object"));
		return true;
	}
	if(!ob->object()->isWidgetType())
	{
		c->warning(__tr2qs("Can't fill non-widget object"));
		return true;
	}
	m_pPixmap->fill(((QWidget *)(ob->object())),iXoffset,iYoffset);
	return true;
}

bool KviKvsObject_pixmap::functionresize(KviKvsObjectFunctionCall *c)
{
	kvs_int_t uWidth,uHeight;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("width",KVS_PT_UNSIGNEDINTEGER,0,uWidth)
		KVSO_PARAMETER("height",KVS_PT_UNSIGNEDINTEGER,0,uHeight)
	KVSO_PARAMETERS_END(c)
	if(!m_pPixmap)return true;
	m_pPixmap->resize(uWidth,uHeight);
	return true;
}


bool KviKvsObject_pixmap::functionload(KviKvsObjectFunctionCall *c)
{
	QString szFile;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("file",KVS_PT_STRING,0,szFile)
	KVSO_PARAMETERS_END(c)
	
	if(!m_pPixmap)return true; 
	if(!QFile::exists(szFile))
	{
		c->warning(__tr2qs("I can't find the specified file %Q."),&szFile);
        return true;
	}

	m_pPixmap->load( szFile );
	
	return true;
}
bool KviKvsObject_pixmap::functionheight(KviKvsObjectFunctionCall *c)
{
	if(!m_pPixmap)return true; 
	c->returnValue()->setInteger(m_pPixmap->height());	
	return true;
}
bool KviKvsObject_pixmap::functionwidth(KviKvsObjectFunctionCall *c)
{
	if(!m_pPixmap)return true; 
	c->returnValue()->setInteger(m_pPixmap->width());	
	return true;
}

QPixmap KviKvsObject_pixmap::getPixmap() const
{
	QPixmap ret(*m_pPixmap);
	return ret;
}

#include "m_class_pixmap.moc"
