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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "kvi_debug.h"
#include "kvi_locale.h"
#include "kvi_error.h"
#include "kvi_iconmanager.h"
#include "kvi_file.h"

#include "kvi_kvs_arraycast.h"
#include "class_tablewidget.h"
#include "class_pixmap.h"
#include <QTableWidget>
#include <QHeaderView>


#include <QPainter>
#include <QPrinter>
#include <QApplication>
#include <QPoint>
#include <QPaintEvent>
#include <QTableWidgetItem>
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
                This widget provides a table.
	@functions:
                !fn: $setText(<row:integer>,<col:integer>,[<text:string>])
                !fn: <string> $text(<row:integer>,<col:integer>)
                !fn: $setHorizontalHeaderLabels(<string array>)
                !fn: $showHorizontalHeader()
                !fn: $hideHorizontalHeader()
                !fn: $setVerticalHeaderLabels(<string array>)
                !fn: $showVerticalHeader()
                !fn: $hideVerticalHeader()
                !fn: $setRowCount(<integer>)
                !fn: <integer> $rowCount()
                !fn: $setColumnCount(<integer>)
                !fn: <integer> $columnCount()
                !fn: $setCellWidget(<hobject>)
                !fn: $setIcon()
                !fn: $clear()
            @signals:
		!sg: $clicked()
		This signal is emitted by the default implementation of [classfnc]$clickEvent[/classfnc]().

*/


KVSO_BEGIN_REGISTERCLASS(KviKvsObject_tablewidget,"tablewidget","widget")

        // Horizontal Header
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,setHorizontalHeaderLabels)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,hideHorizontalHeader)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,hideVerticalHeader)

        // Vertical Header
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,setVerticalHeaderLabels)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,showHorizontalHeader)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,showVerticalHeader)

        // Rows-Columns
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,setRowCount)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,rowCount)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,currentRow)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,setColumnCount)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,columnCount)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,currentColumn)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,itemRowColAt)

        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,hideRow)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,showRow)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,hideColumn)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,showColumn)

        // Item (text. icon, widget)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,setText)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,setToolTip)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,text)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,setCellWidget)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,setIcon)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,clear)

        KVSO_REGISTER_HANDLER(KviKvsObject_tablewidget,"itemEnteredEvent",itemEnteredEvent);


KVSO_END_REGISTERCLASS(KviKvsObject_tablewidget)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_tablewidget,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_tablewidget)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_tablewidget)

KVSO_END_DESTRUCTOR(KviKvsObject_tablewidget)

bool KviKvsObject_tablewidget::init(KviKvsRunTimeContext *,KviKvsVariantList *)
{

        SET_OBJECT(QTableWidget)
        connect(widget(),SIGNAL(itemEntered(QTableWidgetItem *)),this,SLOT(slotItemEntered(QTableWidgetItem *)));
        return true;
}
KVSO_CLASS_FUNCTION(tablewidget,clear)
{
        CHECK_INTERNAL_POINTER(widget())
        ((QTableWidget *)widget())->clear();
        return true;
}
KVSO_CLASS_FUNCTION(tablewidget,setText)
{
            CHECK_INTERNAL_POINTER(widget())
            kvs_int_t iCol,iRow;
            QString szText;
            KVSO_PARAMETERS_BEGIN(c)
                        KVSO_PARAMETER("row",KVS_PT_UNSIGNEDINTEGER,0,iRow)
                        KVSO_PARAMETER("col",KVS_PT_UNSIGNEDINTEGER,0,iCol)
                        KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
            KVSO_PARAMETERS_END(c)
            if(iRow>=((QTableWidget *)widget())->rowCount() || iRow>=((QTableWidget *)widget())->rowCount())
                c->warning(__tr2qs_ctx("Item out of the tablewidget size","objects"));
            QTableWidgetItem *item=((QTableWidget *)widget())->item(iRow,iCol);
            if(!item)
            {
                item=new QTableWidgetItem();
                ((QTableWidget *)widget())->setItem(iRow,iCol,item);
            }
            item->setText(szText);
            return true;
}
KVSO_CLASS_FUNCTION(tablewidget,setIcon)
{
            kvs_int_t iCol,iRow;
            KviKvsObject *obPixmap;
            kvs_hobject_t obHpixmap;
            KviKvsVariant * vPixmap;
            KVSO_PARAMETERS_BEGIN(c)
                        KVSO_PARAMETER("row",KVS_PT_UNSIGNEDINTEGER,0,iRow)
                        KVSO_PARAMETER("col",KVS_PT_UNSIGNEDINTEGER,0,iCol)
                        KVSO_PARAMETER("icon",KVS_PT_VARIANT,0,vPixmap)
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
            QTableWidgetItem *item=((QTableWidget *)widget())->item(iRow,iCol);
            if(!item)
            {
                item=new QTableWidgetItem();
                ((QTableWidget *)widget())->setItem(iRow,iCol,item);
            }
            item->setIcon(QIcon(*pix));
            return true;
}
KVSO_CLASS_FUNCTION(tablewidget,text)
{
            CHECK_INTERNAL_POINTER(widget())
            kvs_int_t iCol,iRow;
            KVSO_PARAMETERS_BEGIN(c)
                        KVSO_PARAMETER("row",KVS_PT_UNSIGNEDINTEGER,0,iRow)
                        KVSO_PARAMETER("col",KVS_PT_UNSIGNEDINTEGER,0,iCol)
            KVSO_PARAMETERS_END(c)
            QTableWidgetItem *item=((QTableWidget *)widget())->item(iRow,iCol);
            if(!item) c->returnValue()->setNothing();
            else c->returnValue()->setString(((QTableWidget *)widget())->item(iRow,iCol)->text());
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
KVSO_CLASS_FUNCTION(tablewidget,currentRow)
{
            CHECK_INTERNAL_POINTER(widget())
            c->returnValue()->setInteger(((QTableWidget *)widget())->currentRow());
            return true;
}
KVSO_CLASS_FUNCTION(tablewidget,currentColumn)
{
            CHECK_INTERNAL_POINTER(widget())
            c->returnValue()->setInteger(((QTableWidget *)widget())->currentColumn());
            return true;
}
KVSO_CLASS_FUNCTION(tablewidget,columnCount)
{
            CHECK_INTERNAL_POINTER(widget())
            c->returnValue()->setInteger(((QTableWidget *)widget())->columnCount());
            return true;
}
KVSO_CLASS_FUNCTION(tablewidget,itemRowColAt)
{
        CHECK_INTERNAL_POINTER(widget())
        kvs_int_t uX,uY;
        KVSO_PARAMETERS_BEGIN(c)
                KVSO_PARAMETER("uX",KVS_PT_UNSIGNEDINTEGER,0,uX)
                KVSO_PARAMETER("uY",KVS_PT_UNSIGNEDINTEGER,0,uY)
        KVSO_PARAMETERS_END(c)
        KviKvsArray *pArray;
        QTableWidgetItem *pItem=((QTableWidget *)widget())->itemAt(QPoint(uX,uY));
        pArray=new KviKvsArray;
        pArray->set(0,new KviKvsVariant((kvs_int_t)pItem->row()));
        pArray->set(1,new KviKvsVariant((kvs_int_t)pItem->column()));
        c->returnValue()->setArray(pArray);
        return true;
}
KVSO_CLASS_FUNCTION(tablewidget,setToolTip)
{
        CHECK_INTERNAL_POINTER(widget())
        QString szTooltip;
        kvs_int_t uX,uY;
        KVSO_PARAMETERS_BEGIN(c)
                KVSO_PARAMETER("uX",KVS_PT_UNSIGNEDINTEGER,0,uX)
                KVSO_PARAMETER("uY",KVS_PT_UNSIGNEDINTEGER,0,uY)
                KVSO_PARAMETER("tooltip",KVS_PT_STRING,0,szTooltip)
        KVSO_PARAMETERS_END(c)
        ((QTableWidget *)widget())->itemAt(uX,uY)->setToolTip(szTooltip);
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

KVSO_CLASS_FUNCTION(tablewidget,hideColumn)
{
            CHECK_INTERNAL_POINTER(widget())
            kvs_int_t iCol;
            KVSO_PARAMETERS_BEGIN(c)
                        KVSO_PARAMETER("col",KVS_PT_UNSIGNEDINTEGER,0,iCol)
            KVSO_PARAMETERS_END(c)
            ((QTableWidget *)widget())->hideColumn(iCol);
            return true;
}
KVSO_CLASS_FUNCTION(tablewidget,showColumn)
{
            CHECK_INTERNAL_POINTER(widget())
            kvs_int_t iCol;
            KVSO_PARAMETERS_BEGIN(c)
                        KVSO_PARAMETER("col",KVS_PT_UNSIGNEDINTEGER,0,iCol)
            KVSO_PARAMETERS_END(c)
            ((QTableWidget *)widget())->showColumn(iCol);
            return true;
}KVSO_CLASS_FUNCTION(tablewidget,hideRow)
{
            CHECK_INTERNAL_POINTER(widget())
            kvs_int_t iRow;
            KVSO_PARAMETERS_BEGIN(c)
                        KVSO_PARAMETER("col",KVS_PT_UNSIGNEDINTEGER,0,iRow)
            KVSO_PARAMETERS_END(c)
            ((QTableWidget *)widget())->hideRow(iRow);
            return true;
}KVSO_CLASS_FUNCTION(tablewidget,showRow)
{
            CHECK_INTERNAL_POINTER(widget())
            kvs_int_t iRow;
            KVSO_PARAMETERS_BEGIN(c)
                        KVSO_PARAMETER("col",KVS_PT_UNSIGNEDINTEGER,0,iRow)
            KVSO_PARAMETERS_END(c)
            ((QTableWidget *)widget())->showRow(iRow);
            return true;
}
KVSO_CLASS_FUNCTION(tablewidget,setHorizontalHeaderLabels)
{
            CHECK_INTERNAL_POINTER(widget())
            KviKvsArrayCast ac;
            KVSO_PARAMETERS_BEGIN(c)
                        KVSO_PARAMETER("labels",KVS_PT_ARRAYCAST,0,ac)
            KVSO_PARAMETERS_END(c)
            QStringList szColumns;
            if(KviKvsArray * a = ac.array())
            {
                kvs_int_t uIdx = 0;
                kvs_int_t uSize = a->size();
                while(uIdx < uSize)
                {
                        KviKvsVariant * v = a->at(uIdx);
                        if(v)
                        {
                                QString tmp;
                                v->asString(tmp);
                                szColumns.append(tmp);
                        }
                        else szColumns.append("");
                        uIdx++;
                }
            }
            ((QTableWidget *)object())->setHorizontalHeaderLabels(szColumns);
            return true;
}

KVSO_CLASS_FUNCTION(tablewidget,setVerticalHeaderLabels)
{
            CHECK_INTERNAL_POINTER(widget())
            KviKvsArrayCast ac;
            KVSO_PARAMETERS_BEGIN(c)
                        KVSO_PARAMETER("labels",KVS_PT_ARRAYCAST,0,ac)
            KVSO_PARAMETERS_END(c)
            QStringList szColumns;
            if(KviKvsArray * a = ac.array())
            {
                kvs_int_t uIdx = 0;
                kvs_int_t uSize = a->size();
                while(uIdx < uSize)
                {
                        KviKvsVariant * v = a->at(uIdx);
                        if(v)
                        {
                                QString tmp;
                                v->asString(tmp);
                                szColumns.append(tmp);
                        }
                        else szColumns.append("");
                        uIdx++;
                }
            }
            ((QTableWidget *)object())->setVerticalHeaderLabels(szColumns);
            return true;
}


KVSO_CLASS_FUNCTION(tablewidget,setCellWidget)
{
            CHECK_INTERNAL_POINTER(widget())
            KviKvsObject * pObject;
            kvs_hobject_t hObject;
            kvs_int_t iRow,iCol;
            KVSO_PARAMETERS_BEGIN(c)
                        KVSO_PARAMETER("row",KVS_PT_UNSIGNEDINTEGER,0,iRow)
                        KVSO_PARAMETER("column",KVS_PT_UNSIGNEDINTEGER,0,iCol)
                        KVSO_PARAMETER("widget",KVS_PT_HOBJECT,0,hObject)
            KVSO_PARAMETERS_END(c)
            pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
            CHECK_HOBJECT_IS_WIDGET(pObject)
            ((QTableWidget *)object())->setCellWidget(iRow,iCol,((QWidget *)(pObject->object())));
            return true;
}
KVSO_CLASS_FUNCTION(tablewidget,hideHorizontalHeader)
{
            ((QTableWidget *)widget())->horizontalHeader()->hide();
            return true;
}
KVSO_CLASS_FUNCTION(tablewidget,hideVerticalHeader)
{
            ((QTableWidget *)widget())->verticalHeader()->hide();
            return true;
}

KVSO_CLASS_FUNCTION(tablewidget,showHorizontalHeader)
{
            ((QTableWidget *)widget())->horizontalHeader()->show();
            return true;
}
KVSO_CLASS_FUNCTION(tablewidget,showVerticalHeader)
{
            ((QTableWidget *)widget())->verticalHeader()->show();
            return true;
}
void KviKvsObject_tablewidget::slotItemEntered(QTableWidgetItem * i)

{
      // if (!i) callFunction(this,"itecurrentItemChangeEvent",0,0);
        debug("entered");
        KviKvsVariantList params(new KviKvsVariant((kvs_int_t)i->row()),new KviKvsVariant((kvs_int_t)i->column()));
        callFunction(this,"itemEnteredEvent",0,&params);
}
KVSO_CLASS_FUNCTION(tablewidget,itemEnteredEvent)
{

        emitSignal("itemEntered",c,c->params());
        return true;
}
#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_tablewidget.moc"
#endif //COMPILE_USE_STANDALONE_MOC_SOURCES
