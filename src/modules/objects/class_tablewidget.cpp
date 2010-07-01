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

// FIXME: Indentation in this file sux :D

#include "class_tablewidget.h"
#include "class_painter.h"
#include "class_pixmap.h"

#include "kvi_debug.h"
#include "kvi_app.h"
#include "kvi_locale.h"
#include "kvi_error.h"
#include "kvi_iconmanager.h"
#include "kvi_file.h"
#include "kvi_kvs_arraycast.h"

#include <QTableWidget>
#include <QHeaderView>
#include <QPainter>
#include <QPrinter>
#include <QApplication>
#include <QPoint>
#include <QPaintEvent>
#include <QTableWidgetItem>
#include <QRect>

const int item_flags[] = {
	Qt::NoItemFlags,
	Qt::ItemIsSelectable,
	Qt::ItemIsEditable,
	Qt::ItemIsDragEnabled,
	Qt::ItemIsDropEnabled,
	Qt::ItemIsUserCheckable,
	Qt::ItemIsEnabled,
	Qt::ItemIsTristate
};

const char * const itemflags_tbl[] = {
	"noitemflag",
	"selectable",
	"editable",
	"dragEnabled",
	"dropEnabled",
	"userCheckable",
	"enabled",
	"tristate"
};

#define itemflags_num	(sizeof(itemflags_tbl) / sizeof(itemflags_tbl[0]))

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
		!fn: $setText(<row:uint>,<col:uint>,[<text:string>])
		Sets the contents of the cell pointeid by row and col to text.
		!fn: $setTooltip(<row:uint>,<col:uint>,[<text:string>])
		Sets the tooltip of the cell pointeid by row and col to text.
		!fn: $setNumber(<row:uint>,<col:uint>,[<number:integer>]).
		Sets the contents of the cell pointeid by row and col to number.
                !fn: <string> $text(<row:integer>,<col:integer>)
		Returns  the text of the cell pointed by row and col.
                !fn: $setHorizontalHeaderLabels(<string array>)
		Sets the horizontal header labels using labels.
                !fn: $showHorizontalHeader()
		Shows the horizontal header.
                !fn: $hideHorizontalHeader()
		Hides the horizontal header.
                !fn: $setVerticalHeaderLabels(<string array>)
		Sets the vertical header labels using labels.
                !fn: $showVerticalHeader()
		Shows the vertical header.
		!fn: $hideVerticalHeader()
		Hides the vertical header.
		!fn: $setRowCount(<integer>)
		Sets  the number of rows in the table.
		!fn: <integer> $rowCount()
		Returns the number of rows in the table.
		!fn: <integer> $currentRow()
		Returns the current row.
		Useful in the [classfnc]customContextMenuRequestedEvent[/classfnc].
		!fn: $setColumnCount(<integer>)
		Sets  the number of columns in the table.
		!fn: <integer> $columnCount()
		Returns  the number of columns in the table.
		!fn: <integer> $currentRow()
		Returns the current column.
		Useful in the [classfnc]customContextMenuRequestedEvent[/classfnc]
		!fn: $setFlag(<row:uint>,<col:uint>,<flag1:string>, <flag2:string>, ...)
		Sets the flags for the cell pointed by row and col to the given flags.
		These determine whether the cell can be selected or modified.
		Supported flags are:
		- noitemflag : no flag sets;
		- selectable : cell is selecatble;
		- editable : cell is editable;
		- dragEnabled : cell can dragged;
		- dropEnabled : cell can used as drop target;
		- userCheckable : cell is checkable;
		- enabled :cell is enabled;
		- tristate : cell is checkable with three separate states.
		!fn: $setCellWidget(<row:uint>,<column:uint>,<widget:hobject>)
		Sets the given widget to be displayed in the cell in the given row and column, passing the ownership of the widget to the table.
		!fn: $setIcon(<row:uint>,<column:uint>,<icon:imageid or hobject>)
		Sets the cell icon pointed by <row>,<column>.
		Icon parameter can be a standard KVIrc  [doc:image_id]image identifier[/doc] icon or a KVS pixmap's object.
		!fn: $hideRow(<row:uint>)
		Hides the row <row>.
		!fn: $showRow(<row:uint>)
		Shows the row <row>.
		!fn: $hideColumn(<col:uint>)
		Hides the column <col>.
		!fn: $showColumn(<col:uint>)
		Shows the column <col>.
		!fn: $clear()
		Clears the table.
		!fn: <row,col> $itemEnteredEvent()
		This event is triggered when the mouse cursor enters an cell.
		!fn: <row,col> $cellActivatedEvent()
		This event is triggered when the cell specified by row and column has been activated
		!fn: <row,col>  $cellDoubleClickedEvent()
		This event is triggered whenever a cell in the table is double clicked.
		!fn: $paintCellEvent(<painter:hobject>,<row,uint>,<col:uint>)
		This event handler can be reimplemented to repaint cells.
		The framework will pass the row/col coordinates and, as parameter, a [class]painter[/class]object.
		You don't need to begin/end/delete the painter.
		!fn: $sizeHintCellRequestEvent()
		!fn; $resizeRowsToContents()
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

	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,resizeRowsToContents)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,resizeColumnsToContents)

	// Item (text. icon, widget)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,setText)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,setNumber)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,setToolTip)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,text)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,setCellWidget)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,setIcon)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,clear)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tablewidget,setItemFlags);

	KVSO_REGISTER_HANDLER(KviKvsObject_tablewidget,"itemEnteredEvent",itemEnteredEvent);
	KVSO_REGISTER_HANDLER(KviKvsObject_tablewidget,"cellActivatedEvent",cellActivatedEvent);
	KVSO_REGISTER_HANDLER(KviKvsObject_tablewidget,"cellDoubleClickedEvent",cellDoubleClickedEvent);
	KVSO_REGISTER_STANDARD_TRUERETURN_HANDLER(KviKvsObject_tablewidget,"paintCellEvent")
	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_tablewidget,"sizeHintCellRequestEvent")



KVSO_END_REGISTERCLASS(KviKvsObject_tablewidget)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_tablewidget,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_tablewidget)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_tablewidget)
/*if (m_pCellItemDelegate)
{
	delete m_pCellItemDelegate;
	m_pCellItemDelegate=0;
}*/
KVSO_END_DESTRUCTOR(KviKvsObject_tablewidget)

bool KviKvsObject_tablewidget::init(KviKvsRunTimeContext *c,KviKvsVariantList *)
{

	SET_OBJECT(QTableWidget)
	m_pCellItemDelegate=new KviCellItemDelegate(((QTableWidget *)widget()),this);

	m_pContext=c;
	((QTableWidget *)widget())->setItemDelegate(m_pCellItemDelegate);
	((QTableWidget *)widget())->verticalHeader()->resizeSections(QHeaderView::Custom );
	((QTableWidget *)widget())->horizontalHeader()->resizeSections(QHeaderView::Custom );

	connect(widget(),SIGNAL(itemEntered(QTableWidgetItem *)),this,SLOT(slotItemEntered(QTableWidgetItem *)));
	connect(widget(),SIGNAL(cellActivated(int,int)),this,SLOT(cellActivated(int,int)));
	connect(widget(),SIGNAL(cellDoubleClicked(int,int)),this,SLOT(cellDoubleClicked(int,int)));
        return true;
}

KVSO_CLASS_FUNCTION(tablewidget,clear)
{
	CHECK_INTERNAL_POINTER(widget())
	((QTableWidget *)widget())->clear();
	return true;
}

KVSO_CLASS_FUNCTION(tablewidget,resizeRowsToContents)
{
	CHECK_INTERNAL_POINTER(widget())
	//debug("resize row to contents");

	((QTableWidget *)widget())->resizeRowsToContents();
	return true;
}

KVSO_CLASS_FUNCTION(tablewidget,resizeColumnsToContents)
{
	CHECK_INTERNAL_POINTER(widget())
	//debug("resize coluns to contents");
	((QTableWidget *)widget())->resizeColumnsToContents();
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
		KVSO_PARAMETER("text",KVS_PT_NONEMPTYSTRING,0,szText)
	KVSO_PARAMETERS_END(c)
	if(uRow >= (uint)((QTableWidget *)widget())->rowCount() || uRow >= (uint)((QTableWidget *)widget())->rowCount())
	c->warning(__tr2qs_ctx("Item out of the tablewidget size","objects"));
	QTableWidgetItem * pItem = ((QTableWidget *)widget())->item(uRow,uCol);
	if(!pItem)
	{
		pItem = new QTableWidgetItem();
		((QTableWidget *)widget())->setItem(uRow,uCol,pItem);
	}
	pItem->setText(szText);
	return true;
}

KVSO_CLASS_FUNCTION(tablewidget,setNumber)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uCol,uRow;
	kvs_int_t iNumber;
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("row",KVS_PT_UNSIGNEDINTEGER,0,uRow)
		KVSO_PARAMETER("col",KVS_PT_UNSIGNEDINTEGER,0,uCol)
		KVSO_PARAMETER("number",KVS_PT_INT,0,iNumber)
	KVSO_PARAMETERS_END(c)
	if(uRow >= (uint)((QTableWidget *)widget())->rowCount() || uRow >= (uint)((QTableWidget *)widget())->rowCount())
	c->warning(__tr2qs_ctx("Item out of the tablewidget size","objects"));
	QTableWidgetItem * pItem = ((QTableWidget *)widget())->item(uRow,uCol);
	if(!pItem)
	{
		pItem = new QTableWidgetItem();
		((QTableWidget *)widget())->setItem(uRow,uCol,pItem);
	}
	pItem->setData(0,(int)iNumber);
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
		KVSO_PARAMETER("icon",KVS_PT_VARIANT,0,vPixmap)
	KVSO_PARAMETERS_END(c)
	QPixmap * pix = 0;
	if(vPixmap->isHObject())
	{
		vPixmap->asHObject(obHpixmap);
		obPixmap=KviKvsKernel::instance()->objectController()->lookupObject(obHpixmap);
		if(!obPixmap->inheritsClass("pixmap"))
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
	QTableWidgetItem * pItem = ((QTableWidget *)widget())->item(uRow,uCol);
	if(!pItem)
	{
		pItem = new QTableWidgetItem();
                ((QTableWidget *)widget())->setItem(uRow,uCol,pItem);
	}
	pItem->setIcon(QIcon(*pix));
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
	QTableWidgetItem * pItem = ((QTableWidget *)widget())->item(uRow,uCol);
	if(!pItem) c->returnValue()->setNothing();
	else c->returnValue()->setString(((QTableWidget *)widget())->item(uRow,uCol)->text());
	return true;
}

KVSO_CLASS_FUNCTION(tablewidget,setRowCount)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uRow;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("row",KVS_PT_UNSIGNEDINTEGER,0,uRow)
	KVSO_PARAMETERS_END(c)
	((QTableWidget *)widget())->setRowCount(uRow);
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
	kvs_int_t iXpos,iYpos;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x_pos",KVS_PT_INT,0,iXpos)
		KVSO_PARAMETER("y_pos",KVS_PT_INT,0,iYpos)
	KVSO_PARAMETERS_END(c)
	KviKvsArray *pArray;
	QPoint pPoint=((QTableWidget *)widget())->viewport()->mapFromGlobal(QPoint(iXpos,iYpos));
	QTableWidgetItem *pItem=((QTableWidget *)widget())->itemAt(pPoint.x(),pPoint.y());
	pArray=new KviKvsArray;
	if (!pItem) {
		pArray->set(0,new KviKvsVariant((kvs_int_t)-1));
		pArray->set(1,new KviKvsVariant((kvs_int_t)-1));
	}
	else {
		pArray->set(0,new KviKvsVariant((kvs_int_t)pItem->row()));
		pArray->set(1,new KviKvsVariant((kvs_int_t)pItem->column()));
	}
	c->returnValue()->setArray(pArray);
        return true;
}

KVSO_CLASS_FUNCTION(tablewidget,setToolTip)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szTooltip;
	kvs_uint_t uRow,uCol;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("row",KVS_PT_UNSIGNEDINTEGER,0,uRow)
		KVSO_PARAMETER("col",KVS_PT_UNSIGNEDINTEGER,0,uCol)
		KVSO_PARAMETER("tooltip",KVS_PT_STRING,0,szTooltip)
	KVSO_PARAMETERS_END(c)
	QTableWidgetItem *pItem=((QTableWidget *)widget())->item(uRow,uCol);
	if (pItem) pItem->setToolTip(szTooltip);
	return true;
}

KVSO_CLASS_FUNCTION(tablewidget,setColumnCount)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uCol;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("col",KVS_PT_UNSIGNEDINTEGER,0,uCol)
	KVSO_PARAMETERS_END(c)
	((QTableWidget *)widget())->setColumnCount(uCol);
	return true;
}

KVSO_CLASS_FUNCTION(tablewidget,hideColumn)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uCol;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("col",KVS_PT_UNSIGNEDINTEGER,0,uCol)
	KVSO_PARAMETERS_END(c)
	((QTableWidget *)widget())->hideColumn(uCol);
	return true;
}

KVSO_CLASS_FUNCTION(tablewidget,showColumn)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uCol;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("col",KVS_PT_UNSIGNEDINTEGER,0,uCol)
	KVSO_PARAMETERS_END(c)
	((QTableWidget *)widget())->showColumn(uCol);
	return true;
}

KVSO_CLASS_FUNCTION(tablewidget,hideRow)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uRow;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("col",KVS_PT_UNSIGNEDINTEGER,0,uRow)
	KVSO_PARAMETERS_END(c)
	((QTableWidget *)widget())->hideRow(uRow);
	return true;
}

KVSO_CLASS_FUNCTION(tablewidget,showRow)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uRow;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("col",KVS_PT_UNSIGNEDINTEGER,0,uRow)
	KVSO_PARAMETERS_END(c)
	((QTableWidget *)widget())->showRow(uRow);
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
	kvs_uint_t uRow,uCol;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("row",KVS_PT_UNSIGNEDINTEGER,0,uRow)
		KVSO_PARAMETER("column",KVS_PT_UNSIGNEDINTEGER,0,uCol)
		KVSO_PARAMETER("widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETERS_END(c)
		pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	((QTableWidget *)object())->setCellWidget(uRow,uCol,((QWidget *)(pObject->object())));
	return true;
}

KVSO_CLASS_FUNCTION(tablewidget,hideHorizontalHeader)
{
	Q_UNUSED(c);
	((QTableWidget *)widget())->horizontalHeader()->hide();
	return true;
}

KVSO_CLASS_FUNCTION(tablewidget,hideVerticalHeader)
{
	Q_UNUSED(c);
	((QTableWidget *)widget())->verticalHeader()->hide();
	return true;
}

KVSO_CLASS_FUNCTION(tablewidget,showHorizontalHeader)
{
	Q_UNUSED(c);
	((QTableWidget *)widget())->horizontalHeader()->show();
	return true;
}

KVSO_CLASS_FUNCTION(tablewidget,showVerticalHeader)
{
	Q_UNUSED(c);
	((QTableWidget *)widget())->verticalHeader()->show();
	return true;
}
KVSO_CLASS_FUNCTION(tablewidget,setItemFlags)
{
	CHECK_INTERNAL_POINTER(widget())

	QStringList itemflags;
	kvs_uint_t iRow,iCol;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("row",KVS_PT_UNSIGNEDINTEGER,0,iRow)
		KVSO_PARAMETER("column",KVS_PT_UNSIGNEDINTEGER,0,iCol)
		KVSO_PARAMETER("flags",KVS_PT_STRINGLIST,KVS_PF_OPTIONAL,itemflags)
		KVSO_PARAMETERS_END(c)
	int flag,sum=0;
	for ( int i=0;i<itemflags.count();i++)
	{
		flag = 0;
		for(unsigned int j = 0; j < itemflags_num; j++)
		{
			if(KviQString::equalCI(itemflags.at(i), itemflags_tbl[j]))
			{
				flag=item_flags[j];
				break;
			}
		}
		if(flag)
		{
			//      if (flag==Qt::ItemIsUserCheckable)
			//                    pItem->setCheckState(0,Qt::Unchecked);
			sum = sum | flag;
		} else {
			c->warning(__tr2qs_ctx("Unknown item flag '%Q'","objects"),&itemflags.at(i));
		}
	}
	QTableWidgetItem *pItem=((QTableWidget *)widget())->item(iRow,iCol);
	if(pItem) pItem->setFlags((Qt::ItemFlags)sum);
	return true;
}

void KviKvsObject_tablewidget::slotItemEntered(QTableWidgetItem * i)
{
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)i->row()),new KviKvsVariant((kvs_int_t)i->column()));
	callFunction(this,"itemEnteredEvent",0,&params);
}

KVSO_CLASS_FUNCTION(tablewidget,itemEnteredEvent)
{
	emitSignal("itemEntered",c,c->params());
	return true;
}

void KviKvsObject_tablewidget::cellActivated(int iRow,int iCol)
{
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)iRow),new KviKvsVariant((kvs_int_t)iCol));
	callFunction(this,"cellActivatedEvent",0,&params);
}

KVSO_CLASS_FUNCTION(tablewidget,cellActivatedEvent)
{
	emitSignal("cellActivated",c,c->params());
	return true;
}

void KviKvsObject_tablewidget::cellDoubleClicked(int iRow,int iCol)
{
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)iRow),new KviKvsVariant((kvs_int_t)iCol));
	callFunction(this,"cellDoubleClickedEvent",0,&params);
}

KVSO_CLASS_FUNCTION(tablewidget,cellDoubleClickedEvent)
{
	emitSignal("cellDoubleClicked",c,c->params());
	return true;
}
bool KviKvsObject_tablewidget::paint(QPainter * p, const QStyleOptionViewItem & option, const QModelIndex & index)
{
	p->save();
	KviKvsObjectClass * pClass = KviKvsKernel::instance()->objectController()->lookupClass("painter");
	KviKvsVariantList params;
	KviKvsObject * pObject = pClass->allocateInstance(0,"internalpainter",m_pContext,&params);
	((KviKvsObject_painter *)pObject)->setInternalPainter(p);
	p->setClipRect(option.rect);
	p->translate(option.rect.x(),option.rect.y());
	int col=index.column();
	int row=index.row();
	kvs_hobject_t handle=pObject->handle();
	KviKvsVariantList parameters(new KviKvsVariant(handle),new KviKvsVariant((kvs_int_t) row),new KviKvsVariant((kvs_int_t) col));
	bool ret=false;
	KviKvsVariant *retv=new KviKvsVariant(ret);
	callFunction(this,"paintCellEvent",retv,&parameters);
	pObject=KviKvsKernel::instance()->objectController()->lookupObject(handle);
	if (pObject) pObject->dieNow();
	p->restore();
	return retv->asBoolean();
}

KviCellItemDelegate::KviCellItemDelegate(QAbstractItemView * pWidget,KviKvsObject_tablewidget * parent)
: QItemDelegate(pWidget), m_pParentScript(parent)
{
}

KviCellItemDelegate::~KviCellItemDelegate()
{
}

void KviCellItemDelegate::paint(QPainter * pPainter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	if (m_pParentScript->paint(pPainter,option,index))
		QItemDelegate::paint(pPainter,option,index);
}

QSize KviCellItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex & index) const
{
	KviKvsVariant *sizeret=new KviKvsVariant();
	//debug("sizehint cell");
	KviKvsVariantList parameters(new KviKvsVariant((kvs_int_t) index.row()),new KviKvsVariant((kvs_int_t) index.column()));
	m_pParentScript->callFunction(m_pParentScript,"sizeHintCellRequestEvent",sizeret,&parameters);
	if (sizeret->isArray())
	{
		if (sizeret->array()->size()==2)
		{
			kvs_int_t w,h;
			if (sizeret->array()->at(0)->asInteger(w) && sizeret->array()->at(1)->asInteger(h))return QSize(w,h);
		}
	}
	return QItemDelegate::sizeHint(option,index);
}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_tablewidget.moc"
#endif //COMPILE_USE_STANDALONE_MOC_SOURCES
