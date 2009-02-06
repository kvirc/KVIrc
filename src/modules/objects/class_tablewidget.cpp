//=============================================================================
//
//   File : class_tablewidget.cpp
//   Creation date : Wed 04 Feb 2009 09:30:05 CEST by Carbone Alessandro
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_debug.h"
#include "kvi_locale.h"
#include "kvi_error.h"
#include "kvi_iconmanager.h"
#include "kvi_file.h"

#include "class_tablewidget.h"
#include "class_pixmap.h"
#include "class_sql.h"
 #include <QTableWidget>

/*
            @doc:tablewidget
	@title:
                        tablewidget class
	@type:
		class
	@short:
                        Tablewidget widget.
	@inherits:
		[class]object[/class]
                        [class]widget[/class]
	@description:
                        This widget provides a table widget.
	@functions:
		!fn: $setText([<text:string>])
                @signals:
		!sg: $clicked()
		This signal is emitted by the default implementation of [classfnc]$clickEvent[/classfnc]().

*/


KVSO_BEGIN_REGISTERCLASS(KviKvsObject_tablewidget,"tablewidget","widget")

        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,setText)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,text)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,setIcon)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,setRowCount)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,rowCount)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,setColumnCount)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,columnCount)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,setHorizontalHeaderLabels)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,setVerticalHeaderLabels)

KVSO_END_REGISTERCLASS(KviKvsObject_tablewidget)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_tablewidget,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_tablewidget)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_tablewidget)

KVSO_END_DESTRUCTOR(KviKvsObject_tablewidget)

bool KviKvsObject_tablewidget::init(KviKvsRunTimeContext *,KviKvsVariantList *)
{
        SET_OBJECT(QTableWidget)
        return true;
}
KVSO_CLASS_FUNCTION(tablewidget,setText)
{
            CHECK_INTERNAL_POINTER(widget())
            kvs_uint_t uCol,uRow;
            QString szText;
            KVSO_PARAMETERS_BEGIN(c)
                        KVSO_PARAMETER("row",KVS_PT_UNSIGNEDINTEGER,0,uRow)
                        KVSO_PARAMETER("col",KVS_PT_UNSIGNEDINTEGER,0,uCol)
                        KVSO_PARAMETER("col",KVS_PT_STRING,0,szText)
            KVSO_PARAMETERS_END(c)
            if(uRow>=((QTableWidget *)widget())->rowCount() || uRow>=((QTableWidget *)widget())->rowCount())
                c->warning(__tr2qs_ctx("Item out of the tablewidget size","objects"));
            QTableWidgetItem *item=((QTableWidget *)widget())->item(uRow,uCol);
            if(!item)
            {
                item=new QTableWidgetItem();
                ((QTableWidget *)widget())->setItem(uRow,uCol,item);
            }
            item->setText(szText);
            return true;
}
KVSO_CLASS_FUNCTION(tablewidget,setIcon)
{
            kvs_uint_t uCol,uRow;
            KviKvsObject *obPixmap;
            kvs_hobject_t obHpixmap;
            KviKvsVariant * vPixmap;
            KVSO_PARAMETERS_BEGIN(c)
                        KVSO_PARAMETER("row",KVS_PT_UNSIGNEDINTEGER,0,uRow)
                        KVSO_PARAMETER("col",KVS_PT_UNSIGNEDINTEGER,0,uCol)
                        KVSO_PARAMETER("pixmap",KVS_PT_VARIANT,0,vPixmap)
            KVSO_PARAMETERS_END(c)
            QPixmap *pix = 0;
            if(vPixmap->isHObject())
            {
                        vPixmap->asHObject(obHpixmap);
                        obPixmap=KviKvsKernel::instance()->objectController()->lookupObject(obHpixmap);
                        if (!obPixmap->inheritsClass("pixmap"))
                        {
                           c->warning(__tr2qs_ctx("Pixmap object or image Id required","objects"));
                           return true;
                        }
                        pix=((KviKvsObject_pixmap *)obPixmap)->getPixmap();
            } else {
                        QString szPix;
                        vPixmap->asString(szPix);
                        pix=g_pIconManager->getImage(szPix);
                        if(!pix)
                        {
                        c->warning(__tr2qs_ctx("Error occured: the suitable file '%Q' is not of the correct format or it is not a valid icon number.","objects"),&szPix);
                                    return true;
                        }
            }
            QTableWidgetItem *item=((QTableWidget *)widget())->item(uRow,uCol);
            if(!item)
            {
                item=new QTableWidgetItem();
                ((QTableWidget *)widget())->setItem(uRow,uCol,item);
            }
            item->setIcon(QIcon(*pix));
            return true;
}
KVSO_CLASS_FUNCTION(tablewidget,text)
{
	CHECK_INTERNAL_POINTER(widget())
            kvs_uint_t uCol,uRow;
            KVSO_PARAMETERS_BEGIN(c)
                        KVSO_PARAMETER("row",KVS_PT_UNSIGNEDINTEGER,0,uRow)
                        KVSO_PARAMETER("col",KVS_PT_UNSIGNEDINTEGER,0,uCol)
            KVSO_PARAMETERS_END(c)
            QTableWidgetItem *item=((QTableWidget *)widget())->item(uRow,uCol);
            if(!item) c->returnValue()->setNothing();
            else c->returnValue()->setString(((QTableWidget *)widget())->item(uRow,uCol)->text());
	return true;
}
KVSO_CLASS_FUNCTION(tablewidget,setRowCount)
{
            CHECK_INTERNAL_POINTER(widget())
            kvs_int_t iRow;
            KVSO_PARAMETERS_BEGIN(c)
                        KVSO_PARAMETER("row",KVS_PT_UNSIGNEDINTEGER,0,iRow)
            KVSO_PARAMETERS_END(c)
            ((QTableWidget *)widget())->setRowCount(iRow);
            return true;
}
KVSO_CLASS_FUNCTION(tablewidget,rowCount)
{
            CHECK_INTERNAL_POINTER(widget())
            c->returnValue()->setInteger(((QTableWidget *)widget())->rowCount());
            return true;
}
KVSO_CLASS_FUNCTION(tablewidget,columnCount)
{
            CHECK_INTERNAL_POINTER(widget())
            c->returnValue()->setInteger(((QTableWidget *)widget())->columnCount());
            return true;
}

KVSO_CLASS_FUNCTION(tablewidget,setColumnCount)
{
            CHECK_INTERNAL_POINTER(widget())
            kvs_int_t iCol;
            KVSO_PARAMETERS_BEGIN(c)
                        KVSO_PARAMETER("col",KVS_PT_UNSIGNEDINTEGER,0,iCol)
            KVSO_PARAMETERS_END(c)
            ((QTableWidget *)widget())->setColumnCount(iCol);
            return true;
}

KVSO_CLASS_FUNCTION(tablewidget,setHorizontalHeaderLabels)
{
            CHECK_INTERNAL_POINTER(widget())
            QStringList columns;
            KVSO_PARAMETERS_BEGIN(c)
                        KVSO_PARAMETER("labels",KVS_PT_STRINGLIST,KVS_PF_OPTIONAL,columns)
            KVSO_PARAMETERS_END(c)
            ((QTableWidget *)object())->setHorizontalHeaderLabels(columns);
            return true;
}


KVSO_CLASS_FUNCTION(tablewidget,setVerticalHeaderLabels)
{
            CHECK_INTERNAL_POINTER(widget())
            QStringList columns;
            KVSO_PARAMETERS_BEGIN(c)
                        KVSO_PARAMETER("labels",KVS_PT_STRINGLIST,KVS_PF_OPTIONAL,columns)
            KVSO_PARAMETERS_END(c)
            debug("list count %d",columns.count());
            ((QTableWidget *)object())->setVerticalHeaderLabels(columns);
            return true;
}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_tablewidget.moc"
#endif //COMPILE_USE_STANDALONE_MOC_SOURCES
