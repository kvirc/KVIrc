//=============================================================================
//
//   File : KvsObject_pixmap.cpp
//   Creation date : Fri Mar 18 14:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include "KvsObject_pixmap.h"
#include "KvsObject_widget.h"

#include "KvsObject_memoryBuffer.h"
#include "KviAnimatedPixmap.h"

#include "kvi_debug.h"
#include "KviLocale.h"

#include <QFile>

/*
	@doc: pixmap
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
		!fn: $fill(<colorname, RGB or HSV array value or [<red>, <green>, <blue>][<hue>, <saturation>, <value>], [opacity], [system color:RGB or HSV])
		Fills the pixmap with color and opacity.[br]
		!fn: $resize(<width:integer>,<height:integer>)
		Resizes the pixmap to w width and h height. Set [b]wh[/b] or [b]hg to [b]0[/b], to have a null pixmap.
		!fn: $scale(<width:integer>,<height:integer>,[<aspect_ratio:string>])
		Scales the pixmap or the animation by [b]sx[/b] horizontally and [b]sy[/b] vertically.[br]
		Aspect_ratio values:
		[pre]
			- IgnoreAspectRatio:the pixmap is scaled ignoring his aspect ratio.
			- KeepAspectRatio: pixmap is scaled to a rectangle as large as possible inside size, preserving the aspect ratio.
			- KeepAspectRatioByExpanding, the pixmap is scaled to a rectangle as small as possible outside size, preserving the aspect ratio.
		[/pre]
		[b]Default value is KeepAspectRatio.[/b][br][br]
		!fn: $load(<file_name:string>)
		Load a pixmap from the file <file_name>.
		!fn: $save(<file_name:string>)
		Save the pixmap or the current animation frame to the file <file_name>.
		!fn: $loadAnimation(<file_name:string>)
		Load an animation from the file <file_name> and start it.
		!fn: $startAnimation()
		Start the animation.
		!fn: $stopAnimation()
		Stop the animation.
		!fn: <integer> $height()
		Return the height of the pixmap or of the animation.
		!fn: <integer> $width()
		Return the width of the pixmap or of the animation.
		!fn: $frameChangedEvent()
		This event is triggered by KVIrc when the current animation frame changes.
	@signals:
		!sg: $frameChanged()
		This signal is emitted by the default implementation of [classfnc]$frameChangedEvent[/classfnc]().[br]
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_pixmap, "pixmap", "object")

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap, fill)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap, resize)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap, scale)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap, load)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap, loadAnimation)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap, save)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap, startAnimation)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap, stopAnimation)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap, loadFromMemoryBuffer)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap, height)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap, width)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap, rotate)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap, mirrored)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap, setPixel)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap, pixel)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap, grabWidget)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap, frameChangedEvent)

KVSO_END_REGISTERCLASS(KvsObject_pixmap)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_pixmap, KviKvsObject)
m_pAnimatedPixmap = nullptr;
m_currentType = Image;
m_pPixmap = nullptr;
m_pImage = nullptr;
KVSO_END_CONSTRUCTOR(KvsObject_pixmap)

KVSO_BEGIN_DESTRUCTOR(KvsObject_pixmap)
emit aboutToDie();
if(m_pPixmap)
	delete m_pPixmap;
if(m_pAnimatedPixmap)
	delete m_pAnimatedPixmap;
if(m_pImage)
	delete m_pImage;
KVSO_END_CONSTRUCTOR(KvsObject_pixmap)

KVSO_CLASS_FUNCTION(pixmap, fill)
{
	kvs_int_t iCol1, iCol2, iCol3, iOpacity;
	QString szColorMode, szColor;
	KviKvsVariant *var1, *var2, *var3;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("Color_1_Or_Colorname", KVS_PT_VARIANT, 0, var1)
	KVSO_PARAMETER("Color_2", KVS_PT_VARIANT, KVS_PF_OPTIONAL, var2)
	KVSO_PARAMETER("Colo3_3", KVS_PT_VARIANT, KVS_PF_OPTIONAL, var3)
	KVSO_PARAMETER("color_mode", KVS_PT_STRING, KVS_PF_OPTIONAL, szColorMode)
	KVSO_PARAMETER("opacity", KVS_PT_INT, KVS_PF_OPTIONAL, iOpacity)
	KVSO_PARAMETERS_END(c)

	QColor col;
	if(!var1->asInteger(iCol1))
	{
		var1->asString(szColor);
		if(c->paramCount() < 2)
			iOpacity = 255;
		else
		{
			if(!var2->asInteger(iOpacity))
			{
				c->warning(__tr2qs_ctx("The opacity parameter didn't evaluate to integer", "objects"));
				return true;
			}
		}
		col.setNamedColor(szColor);
		col.setAlpha(iOpacity);
	}
	else
	{
		if(c->paramCount() < 3)
		{
			c->error(__tr2qs_ctx("Color name or triplet RGB/HSV value required", "objects"));
			return true;
		}
		if(!var2->asInteger(iCol2) || !var3->asInteger(iCol3))
		{
			c->error(__tr2qs_ctx("One of the triplet parameters didn't evaluate to an integer", "objects"));
			return true;
		}
		if(c->paramCount() < 5)
			iOpacity = 255;

		if(KviQString::equalCI(szColorMode, "HSV"))
			col.setHsv(iCol1, iCol2, iCol3, iOpacity);
		else
			col.setRgb(iCol1, iCol2, iCol3, iOpacity);
	}
	if(m_currentType == Image)
	{
		if(m_pImage)
		{
			if(!m_pPixmap)
				m_pPixmap = new QPixmap();
			*m_pPixmap = QPixmap::fromImage(*m_pImage);
			delete m_pImage;
			m_pImage = nullptr;
		}
		else
		{
			m_pPixmap = nullptr;
		}
	}
	else if(m_currentType == AnimatedPixmap)
	{
		delete m_pAnimatedPixmap;
		m_pAnimatedPixmap = nullptr;
	}
	m_currentType = Pixmap;
	if(!m_pPixmap)
		m_pPixmap = new QPixmap();
	m_pPixmap->fill(col);
	return true;
}

KVSO_CLASS_FUNCTION(pixmap, resize)
{
	kvs_int_t iWidth, iHeight;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("width", KVS_PT_INTEGER, 0, iWidth)
	KVSO_PARAMETER("height", KVS_PT_INTEGER, 0, iHeight)
	KVSO_PARAMETERS_END(c)

	if(m_currentType == Image)
	{
		if(m_pImage)
		{
			delete m_pImage;
			m_pImage = nullptr;
		}
	}
	else if(m_currentType == AnimatedPixmap)
	{
		if(m_pAnimatedPixmap)
		{
			delete m_pAnimatedPixmap;
			m_pAnimatedPixmap = nullptr;
		}
	}
	else if(m_pPixmap)
	{
		delete m_pPixmap;
	}
	m_currentType = Image;
	m_pImage = new QImage(iWidth, iHeight, QImage::Format_ARGB32_Premultiplied);
	m_pImage->fill(Qt::transparent);
	return true;
}

KVSO_CLASS_FUNCTION(pixmap, setPixel)
{
	kvs_int_t iCol1, iCol2, iCol3, iOpacity, iX, iY;
	QString szColorMode, szColor;
	KviKvsVariant *var1, *var2, *var3;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x", KVS_PT_INTEGER, 0, iX)
	KVSO_PARAMETER("y", KVS_PT_INTEGER, 0, iY)
	KVSO_PARAMETER("Color_1_Or_Colorname", KVS_PT_VARIANT, 0, var1)
	KVSO_PARAMETER("Color_2", KVS_PT_VARIANT, KVS_PF_OPTIONAL, var2)
	KVSO_PARAMETER("Colo3_3", KVS_PT_VARIANT, KVS_PF_OPTIONAL, var3)
	KVSO_PARAMETER("color_mode", KVS_PT_STRING, KVS_PF_OPTIONAL, szColorMode)
	KVSO_PARAMETER("opacity", KVS_PT_INT, KVS_PF_OPTIONAL, iOpacity)
	KVSO_PARAMETERS_END(c)

	QColor col;
	if(!var1->asInteger(iCol1))
	{
		var1->asString(szColor);
		if(c->paramCount() < 4)
		{
			iOpacity = 255;
		}
		else
		{
			if(!var2->asInteger(iOpacity))
			{
				c->warning(__tr2qs_ctx("The opacity parameter didn't evaluate to integer", "objects"));
				return true;
			}
		}
		col.setNamedColor(szColor);
		col.setAlpha(iOpacity);
	}
	else
	{
		if(c->paramCount() < 5)
		{
			c->error(__tr2qs_ctx("Color name or triplet RGB/HSV value required", "objects"));
			return true;
		}
		if(!var2->asInteger(iCol2) || !var3->asInteger(iCol3))
		{
			c->error(__tr2qs_ctx("One of the triplet parameters didn't evaluate to an integer", "objects"));
			return true;
		}
		if(c->paramCount() < 7)
			iOpacity = 255;

		if(KviQString::equalCI(szColorMode, "HSV"))
			col.setHsv(iCol1, iCol2, iCol3, iOpacity);
		else
			col.setRgb(iCol1, iCol2, iCol3, iOpacity);
	}

	if(m_currentType == AnimatedPixmap)
	{
		c->warning(__tr2qs_ctx("AnimatedPixmap not supported", "objects"));
		return true;
	}
	else if((m_currentType == Image && !m_pImage) || (m_currentType == Pixmap && !m_pPixmap))
	{
		c->error(__tr2qs_ctx("The pixmap is null", "objects"));
		return false;
	}
	else if(m_currentType == Pixmap)
	{
		delete m_pImage;
		m_pImage = new QImage();
		*m_pImage = m_pPixmap->toImage();
		delete m_pPixmap;
	}
	m_currentType = Image;
	m_pImage->setPixel(iX, iY, col.rgba());
	return true;
}

KVSO_CLASS_FUNCTION(pixmap, pixel)
{
	kvs_int_t iX, iY;
	QString szFlags;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x", KVS_PT_INTEGER, 0, iX)
	KVSO_PARAMETER("y", KVS_PT_INTEGER, 0, iY)
	KVSO_PARAMETER("b", KVS_PT_STRING, KVS_PF_OPTIONAL, szFlags)
	KVSO_PARAMETERS_END(c)

	if(m_currentType == AnimatedPixmap)
	{
		c->warning(__tr2qs_ctx("AnimatedPixmap not supported", "objects"));
		return true;
	}
	else if((m_currentType == Image && !m_pImage) || (m_currentType == Pixmap && !m_pPixmap))
	{
		c->error(__tr2qs_ctx("The pixmap is null", "objects"));
		return false;
	}
	else if(m_currentType == Pixmap)
	{
		// 		qDebug("mpimage %i",m_pImage);
		// 		qDebug("pixmap %i %i",m_pPixmap->width(),m_pPixmap->height());
		delete m_pImage;
		m_pImage = new QImage();
		*m_pImage = m_pPixmap->toImage();
		delete m_pPixmap;
		m_pPixmap = nullptr;
	}
	m_currentType = Image;
	QRgb rgb = m_pImage->pixel(iX, iY);
	QColor col(rgb);
	if(szFlags.isEmpty())
	{
		c->returnValue()->setString(col.name());
		return true;
	}
	if(szFlags.indexOf('a', 0, Qt::CaseInsensitive) != -1)
	{
		KviKvsArray * pArray = new KviKvsArray();
		pArray->set(0, new KviKvsVariant((kvs_int_t)col.red()));
		pArray->set(1, new KviKvsVariant((kvs_int_t)col.green()));
		pArray->set(2, new KviKvsVariant((kvs_int_t)col.blue()));
		c->returnValue()->setArray(pArray);
	}
	else if(szFlags.indexOf('h', 0, Qt::CaseInsensitive) != -1)
	{
		KviKvsHash * pHash = new KviKvsHash();
		pHash->set("red", new KviKvsVariant((kvs_int_t)col.red()));
		pHash->set("green", new KviKvsVariant((kvs_int_t)col.green()));
		pHash->set("blue", new KviKvsVariant((kvs_int_t)col.blue()));
		c->returnValue()->setHash(pHash);
	}
	return true;
}
KVSO_CLASS_FUNCTION(pixmap, rotate)
{
	kvs_real_t dAngle;
	QString szAxis;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("angle", KVS_PT_DOUBLE, 0, dAngle)
	KVSO_PARAMETER("axis", KVS_PT_STRING, KVS_PF_OPTIONAL, szAxis)
	KVSO_PARAMETERS_END(c)

	Qt::Axis axis = Qt::ZAxis;
	if(!szAxis.isEmpty())
	{
		if(KviQString::equalCI(szAxis, "XAxis"))
			axis = Qt::XAxis;
		else if(KviQString::equalCI(szAxis, "YAxis"))
			axis = Qt::YAxis;
		else if(KviQString::equalCI(szAxis, "ZAxis"))
			axis = Qt::ZAxis;
		else
			c->warning(__tr2qs_ctx("Unknown axis '%Q' switching to default ZAxis", "objects"), &szAxis);
	}

	if(m_currentType == Pixmap)
	{
		if(m_pPixmap)
		{
			if(!m_pImage)
				m_pImage = new QImage();
			*m_pImage = m_pPixmap->toImage();
		}
		else
		{
			c->error(__tr2qs_ctx("The pixmap is null", "objects"));
			return false;
		}
	}
	else if(m_currentType == AnimatedPixmap)
	{
		c->warning(__tr2qs_ctx("AnimatedPixmap not supported", "objects"));
		return true;
	}

	if(!m_pImage)
	{
		c->error(__tr2qs_ctx("The pixmap is null", "objects"));
		return false;
	}
	m_currentType = Image;
	QTransform transform;
	transform.rotate(dAngle, axis);
	*m_pImage = m_pImage->transformed(transform);
	return true;
}

KVSO_CLASS_FUNCTION(pixmap, mirrored)
{
	bool bHorizontal, bVertical;

	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bHorizontal", KVS_PT_BOOLEAN, 0, bHorizontal)
	KVSO_PARAMETER("bVertical", KVS_PT_BOOLEAN, 0, bVertical)
	KVSO_PARAMETERS_END(c)
	if(m_currentType == Pixmap)
	{
		if(m_pPixmap)
		{
			if(!m_pImage)
				m_pImage = new QImage();
			*m_pImage = m_pPixmap->toImage();
		}
		else
		{
			c->error(__tr2qs_ctx("The pixmap is null", "objects"));
			return false;
		}
	}
	else if(m_currentType == AnimatedPixmap)
	{
		c->warning(__tr2qs_ctx("AnimatedPixmap not supported", "objects"));
		return true;
	}

	if(!m_pImage)
	{
		c->error(__tr2qs_ctx("The pixmap is null", "objects"));
		return false;
	}
	m_currentType = Image;
	*m_pImage = m_pImage->mirrored(bHorizontal, bVertical);
	return true;
}

KVSO_CLASS_FUNCTION(pixmap, scale)
{
	kvs_int_t iWidth, iHeight;
	QString szAspectRatio;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("width", KVS_PT_INTEGER, 0, iWidth)
	KVSO_PARAMETER("height", KVS_PT_INTEGER, 0, iHeight)
	KVSO_PARAMETER("aspect_ratio", KVS_PT_STRING, KVS_PF_OPTIONAL, szAspectRatio)
	KVSO_PARAMETERS_END(c)
	Qt::AspectRatioMode ratio = Qt::KeepAspectRatio;
	if(!szAspectRatio.isEmpty())
	{
		if(KviQString::equalCI(szAspectRatio, "IgnoreAspectRatio"))
			ratio = Qt::IgnoreAspectRatio;
		else if(KviQString::equalCI(szAspectRatio, "KeepAspectRatio"))
			ratio = Qt::KeepAspectRatio;
		else if(KviQString::equalCI(szAspectRatio, "KeepAspectRatioByExpanding"))
			ratio = Qt::KeepAspectRatioByExpanding;
		else
			c->warning(__tr2qs_ctx("Unknown aspect ratio %Q - Switching to KeepAspectRatio ratio", "objects"), &szAspectRatio);
	}
	if(m_currentType == Pixmap)
	{
		if(m_pPixmap)
			*m_pPixmap = m_pPixmap->scaled(iWidth, iHeight, ratio, Qt::SmoothTransformation);
		else
		{
			c->error(__tr2qs_ctx("The pixmap is null", "objects"));
			return false;
		}
	}
	else if(m_currentType == AnimatedPixmap)
		m_pAnimatedPixmap->resize(QSize(iWidth, iHeight), ratio);
	else if(m_currentType == Image)
	{
		if(!m_pImage)
		{
			c->error(__tr2qs_ctx("The pixmap is null", "objects"));
			return false;
		}
		*m_pImage = m_pImage->scaled(iWidth, iHeight, ratio, Qt::SmoothTransformation);
	}
	return true;
}

KVSO_CLASS_FUNCTION(pixmap, loadAnimation)
{
	QString szFile;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("file", KVS_PT_STRING, 0, szFile)
	KVSO_PARAMETERS_END(c)

	if(!QFile::exists(szFile))
	{
		c->warning(__tr2qs_ctx("I can't find the specified file '%Q'.", "objects"), &szFile);
		return true;
	}
	if(m_pAnimatedPixmap)
	{
		delete m_pAnimatedPixmap;
		m_pAnimatedPixmap = nullptr;
	}
	if(m_pPixmap)
	{
		delete m_pPixmap;
		m_pPixmap = nullptr;
	}
	m_pAnimatedPixmap = new KviAnimatedPixmap(szFile);
	connect(m_pAnimatedPixmap, SIGNAL(frameChanged()), this, SLOT(frameChanged()));
	return true;
}

KVSO_CLASS_FUNCTION(pixmap, startAnimation)
{
	if(m_pAnimatedPixmap)
		m_pAnimatedPixmap->start();
	return true;
}

KVSO_CLASS_FUNCTION(pixmap, stopAnimation)
{
	if(m_pAnimatedPixmap)
		m_pAnimatedPixmap->stop();
	return true;
}

KVSO_CLASS_FUNCTION(pixmap, load)
{
	QString szFile;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("file", KVS_PT_STRING, 0, szFile)
	KVSO_PARAMETERS_END(c)

	if(!QFile::exists(szFile))
	{
		c->warning(__tr2qs_ctx("I can't find the specified file '%Q'.", "objects"), &szFile);
		return true;
	}
	if(!m_pImage)
		m_pImage = new QImage();
	m_currentType = Image;
	m_pImage->load(szFile);
	return true;
}

KVSO_CLASS_FUNCTION(pixmap, save)
{
	QString szFile;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("file", KVS_PT_STRING, 0, szFile)
	KVSO_PARAMETERS_END(c)

	if(m_currentType == Pixmap)
	{
		if(m_pPixmap)
		{
			m_pPixmap->save(szFile);
		}
		else
		{
			c->error(__tr2qs_ctx("The pixmap is null", "objects"));
			return false;
		}
	}
	else if(m_currentType == Image)
	{
		if(m_pImage)
		{
			m_pImage->save(szFile);
		}
		else
		{
			c->error(__tr2qs_ctx("The pixmap is null", "objects"));
			return false;
		}
	}
	else
	{
		m_pAnimatedPixmap->pixmap()->save(szFile);
	}
	return true;
}

void KvsObject_pixmap::setInternalPixmap(QPixmap * pPixmap)
{
	if(m_currentType == Image && m_pImage)
		delete m_pImage;
	else if(m_currentType == Pixmap && m_pPixmap)
		delete m_pPixmap;
	m_currentType = Pixmap;
	m_pPixmap = pPixmap;
}
void KvsObject_pixmap::setInternalImage(QImage * pImage)
{
	if(m_currentType == Image && m_pImage)
		delete m_pImage;
	else if(m_currentType == Pixmap && m_pPixmap)
		delete m_pPixmap;
	m_currentType = Image;
	m_pImage = pImage;
}
KVSO_CLASS_FUNCTION(pixmap, loadFromMemoryBuffer)
{
	KviKvsObject * pObject;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("hobject", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETERS_END(c)

	pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!pObject)
	{
		c->warning(__tr2qs_ctx("Buffer parameter is not an object", "objects"));
		return true;
	}
	if(!pObject->inheritsClass("memorybuffer"))
	{
		c->warning(__tr2qs_ctx("Buffer parameter is not a memorybuffer object", "objects"));
		return true;
	}
	if(m_pAnimatedPixmap)
	{
		delete m_pAnimatedPixmap;
		m_pAnimatedPixmap = nullptr;
	}
	if(!m_pPixmap)
		m_pPixmap = new QPixmap();
	m_pPixmap->loadFromData(((KvsObject_memoryBuffer *)pObject)->dataBuffer());
	return true;
}

KVSO_CLASS_FUNCTION(pixmap, grabWidget)
{
	CHECK_INTERNAL_POINTER(m_pPixmap)
	KviKvsObject * pObject;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("widget", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETERS_END(c)

	pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	if(m_pAnimatedPixmap)
	{
		delete m_pAnimatedPixmap;
		m_pAnimatedPixmap = nullptr;
	}
	if(!m_pPixmap)
		m_pPixmap = new QPixmap();
	*m_pPixmap = QPixmap::grabWidget(((KvsObject_widget *)pObject)->widget());
	return true;

}

KVSO_CLASS_FUNCTION(pixmap, height)
{
	if(m_currentType == Image && m_pImage)
		c->returnValue()->setInteger(m_pImage->height());
	else if(m_currentType == Pixmap && m_pPixmap)
		c->returnValue()->setInteger(m_pPixmap->height());
	else
		c->returnValue()->setInteger(0);
	return true;
}

KVSO_CLASS_FUNCTION(pixmap, width)
{
	if(m_currentType == Image && m_pImage)
		c->returnValue()->setInteger(m_pImage->width());
	else if(m_currentType == Pixmap && m_pPixmap)
		c->returnValue()->setInteger(m_pPixmap->width());
	else
		c->returnValue()->setInteger(0);
	return true;
}

KVSO_CLASS_FUNCTION(pixmap, frameChangedEvent)
{
	emitSignal("frameChanged", c);
	return true;
}

// slots
void KvsObject_pixmap::frameChanged()
{
	KviKvsVariantList * params = nullptr;
	callFunction(this, "frameChangedEvent", params);
}
