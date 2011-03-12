//=============================================================================
//
//   File : KvsObject_pixmap.cpp
//   Creation date : Fri Mar 18 14:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVIrc irc client distribution
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
                        !fn: $scale(<width:integer>,<height:integer>,[<aspect_ratio:string>])
                        Scales the pixmap or the animation by sx horizontally and sy vertically.
		Aspect_ratio values:
		- IgnoreAspectRatio:the pixmap is scaled ignoring his aspect ratio.
		- KeepAspectRatio: pixmap is scaled to a rectangle as large as possible inside size, preserving the aspect ratio.
		- KeepAspectRatioByExpanding, the pixmap is scaled to a rectangle as small as possible outside size, preserving the aspect ratio.
                        Default value is KeepAspectRatio.
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


KVSO_BEGIN_REGISTERCLASS(KvsObject_pixmap,"pixmap","object")

        KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap,fill)
        KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap,resize)
        KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap,scale)
        KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap,load)
        KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap,loadAnimation)
        KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap,save)
        KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap,startAnimation)
        KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap,stopAnimation)
        KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap,loadFromMemoryBuffer)
        KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap,height)
        KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap,width)
        KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap,grabWidget)
        KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_pixmap,frameChangedEvent)

KVSO_END_REGISTERCLASS(KvsObject_pixmap)


KVSO_BEGIN_CONSTRUCTOR(KvsObject_pixmap,KviKvsObject)
        m_pAnimatedPixmap = 0;
        m_pPixmap = 0;
KVSO_END_CONSTRUCTOR(KvsObject_pixmap)


KVSO_BEGIN_DESTRUCTOR(KvsObject_pixmap)
        emit aboutToDie();
        if (m_pPixmap)delete m_pPixmap;
        if (m_pAnimatedPixmap) delete m_pAnimatedPixmap;
KVSO_END_CONSTRUCTOR(KvsObject_pixmap)


KVSO_CLASS_FUNCTION(pixmap,fill)
{
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
        if (m_pAnimatedPixmap)
        {
              delete m_pAnimatedPixmap;
              m_pAnimatedPixmap=0;
        }
        if(!m_pPixmap) m_pPixmap=new QPixmap();
        m_pPixmap->fill(((QWidget *)(pObject->object())),iXoffset,iYoffset);
        return true;
}
KVSO_CLASS_FUNCTION(pixmap,resize)
{
        kvs_int_t iWidth,iHeight;
        KVSO_PARAMETERS_BEGIN(c)
            KVSO_PARAMETER("width",KVS_PT_INTEGER,0,iWidth)
            KVSO_PARAMETER("height",KVS_PT_INTEGER,0,iHeight)
        KVSO_PARAMETERS_END(c)
        if(m_pPixmap) delete m_pPixmap;
        if (m_pAnimatedPixmap){
           delete m_pAnimatedPixmap;
           m_pAnimatedPixmap=0;
        }
        m_pPixmap=new QPixmap(iWidth,iHeight);
        return true;
}
KVSO_CLASS_FUNCTION(pixmap,scale)
{
        kvs_int_t iWidth,iHeight;
        QString szAspectRatio;
        KVSO_PARAMETERS_BEGIN(c)
            KVSO_PARAMETER("width",KVS_PT_INTEGER,0,iWidth)
            KVSO_PARAMETER("height",KVS_PT_INTEGER,0,iHeight)
            KVSO_PARAMETER("aspect_ratio",KVS_PT_STRING,KVS_PF_OPTIONAL,szAspectRatio)
        KVSO_PARAMETERS_END(c)
        Qt::AspectRatioMode ratio=Qt::KeepAspectRatio;
        if(!m_pPixmap || m_pAnimatedPixmap) return true;
        if(!szAspectRatio.isEmpty())
        {
            if(KviQString::equalCI(szAspectRatio,"IgnoreAspectRatio")) ratio=Qt::IgnoreAspectRatio;
            else if (KviQString::equalCI(szAspectRatio,"KeepAspectRatio")) ratio=Qt::KeepAspectRatio;
            else if (KviQString::equalCI(szAspectRatio,"KeepAspectRatioByExpanding")) ratio=Qt::KeepAspectRatioByExpanding;
            else  c->warning(__tr2qs_ctx("Unknown aspect ratio %Q - Switching to KeepAspectRatio ratio","objects"),&szAspectRatio);
        }
        if (m_pAnimatedPixmap) m_pAnimatedPixmap->resize(QSize(iWidth,iHeight),ratio);
        else *m_pPixmap=m_pPixmap->scaled(iWidth,iHeight,ratio,Qt::SmoothTransformation);
        return true;
}
KVSO_CLASS_FUNCTION(pixmap,loadAnimation)

{
        QString szFile;
        KVSO_PARAMETERS_BEGIN(c)
            KVSO_PARAMETER("file",KVS_PT_STRING,0,szFile)
        KVSO_PARAMETERS_END(c)
        if(!QFile::exists(szFile))
        {
            c->warning(__tr2qs_ctx("I can't find the specified file '%Q'.","objects"),&szFile);
            return true;
        }
        if (m_pAnimatedPixmap) delete m_pAnimatedPixmap;
        if (m_pPixmap)
        {
            delete m_pPixmap;
            m_pPixmap=0;
        }
        m_pAnimatedPixmap= new KviAnimatedPixmap(szFile);
        connect(m_pAnimatedPixmap,SIGNAL(frameChanged()),this,SLOT(frameChanged()));
        return true;
}

KVSO_CLASS_FUNCTION(pixmap,startAnimation)
{
	Q_UNUSED(c);
        if (m_pAnimatedPixmap)    m_pAnimatedPixmap->start();
        return true;
}

KVSO_CLASS_FUNCTION(pixmap,stopAnimation)
{
	Q_UNUSED(c);
        if (m_pAnimatedPixmap)    m_pAnimatedPixmap->stop();
        return true;
}

KVSO_CLASS_FUNCTION(pixmap,load)
{
        QString szFile;
        KVSO_PARAMETERS_BEGIN(c)
            KVSO_PARAMETER("file",KVS_PT_STRING,0,szFile)
        KVSO_PARAMETERS_END(c)
        if(!QFile::exists(szFile))
        {
            c->warning(__tr2qs_ctx("I can't find the specified file '%Q'.","objects"),&szFile);
            return true;
        }
        if (m_pAnimatedPixmap) delete m_pAnimatedPixmap;
        if(!m_pPixmap) m_pPixmap=new QPixmap();
        m_pPixmap->load(szFile);
        return true;
}
KVSO_CLASS_FUNCTION(pixmap,save)
{
        QString szFile;
        KVSO_PARAMETERS_BEGIN(c)
            KVSO_PARAMETER("file",KVS_PT_STRING,0,szFile)
        KVSO_PARAMETERS_END(c)
        if (m_pAnimatedPixmap) m_pAnimatedPixmap->pixmap()->save(szFile);
        else if(m_pPixmap) m_pPixmap->save(szFile);
        return true;
}

void KvsObject_pixmap::setInternalPixmap(QPixmap *pPixmap)
{
    delete m_pPixmap;
    m_pPixmap=pPixmap;
}
KVSO_CLASS_FUNCTION(pixmap,loadFromMemoryBuffer)
{
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
        if (m_pAnimatedPixmap){
            delete m_pAnimatedPixmap;
            m_pAnimatedPixmap=0;
        }
        if (!m_pPixmap) m_pPixmap=new QPixmap();
        m_pPixmap->loadFromData(((KvsObject_memoryBuffer *)pObject)->dataBuffer());
        return true;
}

KVSO_CLASS_FUNCTION(pixmap,grabWidget)
{
        CHECK_INTERNAL_POINTER(m_pPixmap)
        KviKvsObject * pObject;
        kvs_hobject_t hObject;
        KVSO_PARAMETERS_BEGIN(c)
               KVSO_PARAMETER("widget",KVS_PT_HOBJECT,0,hObject)
        KVSO_PARAMETERS_END(c)
        pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
        CHECK_HOBJECT_IS_WIDGET(pObject)
        if (m_pAnimatedPixmap){
            delete m_pAnimatedPixmap;
            m_pAnimatedPixmap=0;
        }
        if (!m_pPixmap) m_pPixmap=new QPixmap();
        *m_pPixmap=QPixmap::grabWidget(((KvsObject_widget *)pObject)->widget());
  /*          QString szFileName;
            KVSO_PARAMETERS_BEGIN(c)
                        KVSO_PARAMETER("file_name",KVS_PT_STRING,0,szFileName)
            KVSO_PARAMETERS_END(c)
            QPrinter printer;//(QPrinter::HighResolution);
            printer.setOutputFormat(QPrinter::PdfFormat);
            printer.setPaperSize(QPrinter::A4);
            printer.setOutputFileName(szFileName);

            QPixmap pixmap=QPixmap::grabWidget(((QTableWidget *)widget()));
            QPainter painter(&printer);
            painter.drawPixmap(QPoint(0,0), pixmap, pixmap.rect());
             */

        return true;
}
KVSO_CLASS_FUNCTION(pixmap,height)
{
        if(!m_pAnimatedPixmap && !m_pPixmap) c->returnValue()->setInteger(0);
        else if(m_pAnimatedPixmap) c->returnValue()->setInteger(m_pAnimatedPixmap->size().height());
        else c->returnValue()->setInteger(m_pPixmap->height());
        return true;
}
KVSO_CLASS_FUNCTION(pixmap,width)
{
        if(!m_pAnimatedPixmap && !m_pPixmap) c->returnValue()->setInteger(0);
        else if(m_pAnimatedPixmap) c->returnValue()->setInteger(m_pAnimatedPixmap->size().width());
        else c->returnValue()->setInteger(m_pPixmap->width());
        return true;
}

QPixmap * KvsObject_pixmap::getPixmap()
{
        if (!m_pAnimatedPixmap && !m_pPixmap) return new QPixmap();
        else if(m_pAnimatedPixmap) return m_pAnimatedPixmap->pixmap();
        else return m_pPixmap;
}
KVSO_CLASS_FUNCTION(pixmap,frameChangedEvent)
{
        emitSignal("frameChanged",c);
        return true;
}

// slots
void KvsObject_pixmap::frameChanged()
{
        KviKvsVariantList *params=0;
        callFunction(this,"frameChangedEvent",params);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_KvsObject_pixmap.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

