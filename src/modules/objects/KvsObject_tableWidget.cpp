//=============================================================================
//
//   File : KvsObject_tableWidget.cpp
//   Creation date : Wed 04 Feb 2009 09:30:05 CEST by Carbone Alessandro
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KvsObject_tableWidget.h"
#include "KvsObject_painter.h"
#include "KvsObject_pixmap.h"

#include "kvi_debug.h"
#include "KviApplication.h"
#include "KviLocale.h"
#include "KviError.h"
#include "KviIconManager.h"
#include "KviFile.h"
#include "KviKvsArrayCast.h"

#include <QTableWidget>
#include <QHeaderView>
#include <QPainter>
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

#define itemflags_num (sizeof(itemflags_tbl) / sizeof(itemflags_tbl[0]))

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
		Sets the number of rows in the table.
		!fn: <integer> $rowCount()
		Returns the number of rows in the table.
		!fn: <integer> $currentRow()
		Returns the current row.
		Useful in the [classfnc]customContextMenuRequestedEvent[/classfnc].
		!fn: $insertRow(<row:uinteger>)
		Insert an empty row at <row>.
		!fn: $removeRow(<row:uinteger>)
		Remove the row at <row> and all its items from the table.
		!fn: $insertColumn(<column:uinteger>)
		Insert an empty column at <column>.
		!fn: $removeColumn(<column:uinteger>)
		Remove the column at <column> and all its items from the table.
		!fn: $setColumnCount(<uinteger>)
		Sets the number of columns in the table.
		!fn: <integer> $columnCount()
		Returns the number of columns in the table.
		!fn: <integer> $currentRow()
		Returns the current column.
		Useful in the [classfnc]customContextMenuRequestedEvent[/classfnc]
		See also [classfnc]$foreground[/classfnc].
		!fn: $setForegroundColor(<row:uinteger>,<col:uinteger>,<rgb(hex string)_array(red:integer,green:integer,blue_integer)_or_red_or_colorname>,[geen:integer],[blue:integer])
		Sets the foreground of the cell at <row,<col> according to <colorname> <rgb_value>:valid values are:
		- hex string: must be a string with 6 hexadecimal digits (like the ones used to
		specify colors in HTML pages). The first two digits specify
		the RED component, the third and fourth digit specify the GREEN component
		and the last two specify the BLUE component.
		For example "FFFF00" means full red, full green and no blue that gives
		!fn: $setItemFlags(<row:uint>,<col:uint>,<flag1:string>, <flag2:string>, ...)
		Sets the flags for the cell pointed by row and col to the given flags.
		These determine whether the cell can be selected or modified.
		Supported flags are:
		[example]
			- noitemflag : no flag sets;[br]
			- selectable : cell is selectable;[br]
			- editable : cell is editable;[br]
			- dragEnabled : cell can dragged;[br]
			- dropEnabled : cell can used as drop target;[br]
			- userCheckable : cell is checkable;[br]
			- enabled :cell is enabled;[br]
			- tristate : cell is checkable with three separate states.
		[/example]
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
		!fn: $paintCellEvent(<painter:hobject>,<row,uint>,<col:uint>,<cell_width:uint>,<cell_height:uint>)
		This event handler can be reimplemented to repaint cells.
		The framework will pass the row/col coordinates, cell's width and height as parameter, a [class]painter[/class]object.
		You don't need to begin/end/delete the painter.
		!fn: $sizeHintCellRequestEvent()
		This event handler can be reimplemented to pass a custom size hint for this cell to the table layout
		!fn; $resizeRowsToContents()
		This function will try to redistribute the available space according to the space requirement of each row
		!fn; $resizeColumnsToContents()
		This function will try to redistribute the available space according to the space requirement of each column
	@signals:
		!sg: $clicked()
		This signal is emitted by the default implementation of [classfnc]$clickEvent[/classfnc]().
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_tableWidget, "tablewidget", "widget")

// Horizontal Header
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, setHorizontalHeaderLabels)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, hideHorizontalHeader)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, hideVerticalHeader)

// Vertical Header
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, setVerticalHeaderLabels)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, showHorizontalHeader)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, showVerticalHeader)

// Rows-Columns
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, setRowCount)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, insertRow)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, insertColumn)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, removeRow)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, removeColumn)

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, rowCount)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, currentRow)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, setColumnCount)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, columnCount)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, currentColumn)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, itemRowColAt)

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, hideRow)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, showRow)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, hideColumn)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, showColumn)

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, resizeRowsToContents)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, resizeColumnsToContents)

// Item (text. icon, widget)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, setText)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, setNumber)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, setToolTip)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, setForeground)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, text)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, setCellWidget)

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, setIcon)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, clear)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tableWidget, setItemFlags);

KVSO_REGISTER_HANDLER(KvsObject_tableWidget, "itemEnteredEvent", itemEnteredEvent);
KVSO_REGISTER_HANDLER(KvsObject_tableWidget, "cellActivatedEvent", cellActivatedEvent);
KVSO_REGISTER_HANDLER(KvsObject_tableWidget, "cellDoubleClickedEvent", cellDoubleClickedEvent);
KVSO_REGISTER_STANDARD_TRUERETURN_HANDLER(KvsObject_tableWidget, "paintCellEvent")
KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KvsObject_tableWidget, "sizeHintCellRequestEvent")

KVSO_END_REGISTERCLASS(KvsObject_tableWidget)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_tableWidget, KvsObject_widget)

KVSO_END_CONSTRUCTOR(KvsObject_tableWidget)

KVSO_BEGIN_DESTRUCTOR(KvsObject_tableWidget)

KVSO_END_DESTRUCTOR(KvsObject_tableWidget)

bool KvsObject_tableWidget::init(KviKvsRunTimeContext * c, KviKvsVariantList *)
{

	SET_OBJECT(QTableWidget)
	m_pCellItemDelegate = new KviCellItemDelegate(((QTableWidget *)widget()), this);

	m_pContext = c;
	((QTableWidget *)widget())->setItemDelegate(m_pCellItemDelegate);
	((QTableWidget *)widget())->verticalHeader()->resizeSections(QHeaderView::Custom);
	((QTableWidget *)widget())->horizontalHeader()->resizeSections(QHeaderView::Custom);

	connect(widget(), SIGNAL(itemEntered(QTableWidgetItem *)), this, SLOT(slotItemEntered(QTableWidgetItem *)));
	connect(widget(), SIGNAL(cellActivated(int, int)), this, SLOT(cellActivated(int, int)));
	connect(widget(), SIGNAL(cellDoubleClicked(int, int)), this, SLOT(cellDoubleClicked(int, int)));
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, clear)
{
	CHECK_INTERNAL_POINTER(widget())
	((QTableWidget *)widget())->clear();
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, resizeRowsToContents)
{
	CHECK_INTERNAL_POINTER(widget())
	//qDebug("resize row to contents");

	((QTableWidget *)widget())->resizeRowsToContents();
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, resizeColumnsToContents)
{
	CHECK_INTERNAL_POINTER(widget())
	//qDebug("resize coluns to contents");
	((QTableWidget *)widget())->resizeColumnsToContents();
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, setForeground)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsVariant * pColArray;
	kvs_uint_t uCol, uRow;
	kvs_int_t iColR, iColG, iColB;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("row", KVS_PT_UNSIGNEDINTEGER, 0, uRow)
	KVSO_PARAMETER("col", KVS_PT_UNSIGNEDINTEGER, 0, uCol)
	KVSO_PARAMETER("hex_rgb_array_or_red_colorname", KVS_PT_VARIANT, 0, pColArray)
	KVSO_PARAMETER("green", KVS_PT_INT, KVS_PF_OPTIONAL, iColG)
	KVSO_PARAMETER("blue", KVS_PT_INT, KVS_PF_OPTIONAL, iColB)
	KVSO_PARAMETERS_END(c)
	QTableWidgetItem * pItem = ((QTableWidget *)widget())->item(uRow, uCol);
	if(!pItem)
	{
		pItem = new QTableWidgetItem();
		((QTableWidget *)widget())->setItem(uRow, uCol, pItem);
	}
	if(pColArray->isArray())
	{
		if(pColArray->array()->size() < 3)
		{
			c->error(__tr2qs_ctx("The array passed as parameter must contain at least 3 elements", "objects"));
			return false;
		}
		KviKvsVariant * pColR = pColArray->array()->at(0);
		KviKvsVariant * pColG = pColArray->array()->at(1);
		KviKvsVariant * pColB = pColArray->array()->at(2);

		if(!(pColR && pColG && pColB))
		{
			c->error(__tr2qs_ctx("One of the colors array parameters is empty", "objects"));
			return false;
		}

		if(!(pColR->asInteger(iColR) && pColG->asInteger(iColG) && pColB->asInteger(iColB)))
		{
			c->error(__tr2qs_ctx("One of the colors array parameters didn't evaluate to an integer", "objects"));
			return false;
		}
	}
	else
	{
		QColor color;
		if(c->params()->count() == 3)
		{
			if(pColArray->isString())
			{
				QString szColor;
				pColArray->asString(szColor);
				// maybe a color name?
				color.setNamedColor(szColor);
				if(!color.isValid())
				{
					// isn't a color name: lets try with an hex triplet
					color.setNamedColor("#" + szColor);
					if(!color.isValid())
					{
						c->warning(__tr2qs_ctx("Not a valid color!", "objects"));
						return true;
					}
				}
			}
			else
			{
				c->warning(__tr2qs_ctx("Not a valid color!", "objects"));
				return true;
			}
			QBrush brush(color);
			pItem->setForeground(brush);
			return true;
		}
		if(c->params()->count() < 5)
		{
			c->error(__tr2qs_ctx("$setForegroundColor requires either an array as first parameter, one hex string or color name, or three integers", "objects"));
			return false;
		}
		if(!pColArray->asInteger(iColR))
		{
			c->error(__tr2qs_ctx("The first parameter didn't evaluate to an array nor an integer", "objects"));
			return false;
		}
	}

	QBrush brush(QColor(iColR, iColG, iColB));
	pItem->setForeground(brush);
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, setText)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uCol, uRow;
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("row", KVS_PT_UNSIGNEDINTEGER, 0, uRow)
	KVSO_PARAMETER("col", KVS_PT_UNSIGNEDINTEGER, 0, uCol)
	KVSO_PARAMETER("text", KVS_PT_STRING, 0, szText)
	KVSO_PARAMETERS_END(c)
	if(uRow >= (uint)((QTableWidget *)widget())->rowCount() || uRow >= (uint)((QTableWidget *)widget())->rowCount())
		c->warning(__tr2qs_ctx("Item out of the tablewidget size", "objects"));
	QTableWidgetItem * pItem = ((QTableWidget *)widget())->item(uRow, uCol);
	if(!pItem)
	{
		pItem = new QTableWidgetItem();
		((QTableWidget *)widget())->setItem(uRow, uCol, pItem);
	}
	pItem->setText(szText);
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, setNumber)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uCol, uRow;
	kvs_int_t iNumber;
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("row", KVS_PT_UNSIGNEDINTEGER, 0, uRow)
	KVSO_PARAMETER("col", KVS_PT_UNSIGNEDINTEGER, 0, uCol)
	KVSO_PARAMETER("number", KVS_PT_INT, 0, iNumber)
	KVSO_PARAMETERS_END(c)
	if(uRow >= (uint)((QTableWidget *)widget())->rowCount() || uRow >= (uint)((QTableWidget *)widget())->rowCount())
		c->warning(__tr2qs_ctx("Item out of the tablewidget size", "objects"));
	QTableWidgetItem * pItem = ((QTableWidget *)widget())->item(uRow, uCol);
	if(!pItem)
	{
		pItem = new QTableWidgetItem();
		((QTableWidget *)widget())->setItem(uRow, uCol, pItem);
	}
	pItem->setData(0, (int)iNumber);
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, insertRow)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uRow;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("row", KVS_PT_UNSIGNEDINTEGER, 0, uRow)
	KVSO_PARAMETERS_END(c)
	((QTableWidget *)widget())->insertRow(uRow);
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, insertColumn)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uCol;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("row", KVS_PT_UNSIGNEDINTEGER, 0, uCol)
	KVSO_PARAMETERS_END(c)
	((QTableWidget *)widget())->insertColumn(uCol);
	return true;
}
KVSO_CLASS_FUNCTION(tableWidget, removeRow)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uRow;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("row", KVS_PT_UNSIGNEDINTEGER, 0, uRow)
	KVSO_PARAMETERS_END(c)
	((QTableWidget *)widget())->removeRow(uRow);
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, removeColumn)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uCol;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("row", KVS_PT_UNSIGNEDINTEGER, 0, uCol)
	KVSO_PARAMETERS_END(c)
	((QTableWidget *)widget())->removeColumn(uCol);
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, setIcon)
{
	kvs_uint_t uCol, uRow;
	KviKvsObject * obPixmap;
	kvs_hobject_t obHpixmap;
	KviKvsVariant * vPixmap;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("row", KVS_PT_UNSIGNEDINTEGER, 0, uRow)
	KVSO_PARAMETER("col", KVS_PT_UNSIGNEDINTEGER, 0, uCol)
	KVSO_PARAMETER("icon", KVS_PT_VARIANT, 0, vPixmap)
	KVSO_PARAMETERS_END(c)
	QPixmap * pix = nullptr;
	if(vPixmap->isHObject())
	{
		vPixmap->asHObject(obHpixmap);
		obPixmap = KviKvsKernel::instance()->objectController()->lookupObject(obHpixmap);
		if(!obPixmap->inheritsClass("pixmap"))
		{
			c->warning(__tr2qs_ctx("Pixmap object or image Id required", "objects"));
			return true;
		}
		pix = ((KvsObject_pixmap *)obPixmap)->getPixmap();
	}
	else
	{
		QString szPix;
		vPixmap->asString(szPix);
		pix = g_pIconManager->getImage(szPix);
		if(!pix)
		{
			c->warning(__tr2qs_ctx("Error occurred: the suitable file '%Q' is not of the correct format or it is not a valid icon number.", "objects"), &szPix);
			return true;
		}
	}
	QTableWidgetItem * pItem = ((QTableWidget *)widget())->item(uRow, uCol);
	if(!pItem)
	{
		pItem = new QTableWidgetItem();
		((QTableWidget *)widget())->setItem(uRow, uCol, pItem);
	}
	pItem->setIcon(QIcon(*pix));
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, text)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uCol, uRow;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("row", KVS_PT_UNSIGNEDINTEGER, 0, uRow)
	KVSO_PARAMETER("col", KVS_PT_UNSIGNEDINTEGER, 0, uCol)
	KVSO_PARAMETERS_END(c)
	QTableWidgetItem * pItem = ((QTableWidget *)widget())->item(uRow, uCol);
	if(!pItem)
		c->returnValue()->setNothing();
	else
		c->returnValue()->setString(((QTableWidget *)widget())->item(uRow, uCol)->text());
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, setRowCount)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uRow;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("row", KVS_PT_UNSIGNEDINTEGER, 0, uRow)
	KVSO_PARAMETERS_END(c)
	((QTableWidget *)widget())->setRowCount(uRow);
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, rowCount)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((QTableWidget *)widget())->rowCount());
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, currentRow)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((QTableWidget *)widget())->currentRow());
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, currentColumn)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((QTableWidget *)widget())->currentColumn());
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, columnCount)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((QTableWidget *)widget())->columnCount());
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, itemRowColAt)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iXpos, iYpos;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x_pos", KVS_PT_INT, 0, iXpos)
	KVSO_PARAMETER("y_pos", KVS_PT_INT, 0, iYpos)
	KVSO_PARAMETERS_END(c)
	KviKvsArray * pArray;
	QPoint pPoint = ((QTableWidget *)widget())->viewport()->mapFromGlobal(QPoint(iXpos, iYpos));
	QTableWidgetItem * pItem = ((QTableWidget *)widget())->itemAt(pPoint.x(), pPoint.y());
	pArray = new KviKvsArray();
	if(!pItem)
	{
		pArray->set(0, new KviKvsVariant((kvs_int_t)-1));
		pArray->set(1, new KviKvsVariant((kvs_int_t)-1));
	}
	else
	{
		pArray->set(0, new KviKvsVariant((kvs_int_t)pItem->row()));
		pArray->set(1, new KviKvsVariant((kvs_int_t)pItem->column()));
	}
	c->returnValue()->setArray(pArray);
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, setToolTip)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szTooltip;
	kvs_uint_t uRow, uCol;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("row", KVS_PT_UNSIGNEDINTEGER, 0, uRow)
	KVSO_PARAMETER("col", KVS_PT_UNSIGNEDINTEGER, 0, uCol)
	KVSO_PARAMETER("tooltip", KVS_PT_STRING, 0, szTooltip)
	KVSO_PARAMETERS_END(c)
	QTableWidgetItem * pItem = ((QTableWidget *)widget())->item(uRow, uCol);
	if(pItem)
		pItem->setToolTip(szTooltip);
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, setColumnCount)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uCol;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("col", KVS_PT_UNSIGNEDINTEGER, 0, uCol)
	KVSO_PARAMETERS_END(c)
	((QTableWidget *)widget())->setColumnCount(uCol);
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, hideColumn)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uCol;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("col", KVS_PT_UNSIGNEDINTEGER, 0, uCol)
	KVSO_PARAMETERS_END(c)
	((QTableWidget *)widget())->hideColumn(uCol);
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, showColumn)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uCol;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("col", KVS_PT_UNSIGNEDINTEGER, 0, uCol)
	KVSO_PARAMETERS_END(c)
	((QTableWidget *)widget())->showColumn(uCol);
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, hideRow)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uRow;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("col", KVS_PT_UNSIGNEDINTEGER, 0, uRow)
	KVSO_PARAMETERS_END(c)
	((QTableWidget *)widget())->hideRow(uRow);
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, showRow)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uRow;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("col", KVS_PT_UNSIGNEDINTEGER, 0, uRow)
	KVSO_PARAMETERS_END(c)
	((QTableWidget *)widget())->showRow(uRow);
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, setHorizontalHeaderLabels)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsArrayCast ac;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("labels", KVS_PT_ARRAYCAST, 0, ac)
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
			else
				szColumns.append("");
			uIdx++;
		}
	}
	((QTableWidget *)object())->setHorizontalHeaderLabels(szColumns);
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, setVerticalHeaderLabels)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsArrayCast ac;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("labels", KVS_PT_ARRAYCAST, 0, ac)
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
			else
				szColumns.append("");
			uIdx++;
		}
	}
	((QTableWidget *)object())->setVerticalHeaderLabels(szColumns);
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, setCellWidget)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject * pObject;
	kvs_hobject_t hObject;
	kvs_uint_t uRow, uCol;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("row", KVS_PT_UNSIGNEDINTEGER, 0, uRow)
	KVSO_PARAMETER("column", KVS_PT_UNSIGNEDINTEGER, 0, uCol)
	KVSO_PARAMETER("widget", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETERS_END(c)
	pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	((QTableWidget *)object())->setCellWidget(uRow, uCol, ((QWidget *)(pObject->object())));
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, hideHorizontalHeader)
{
	((QTableWidget *)widget())->horizontalHeader()->hide();
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, hideVerticalHeader)
{
	((QTableWidget *)widget())->verticalHeader()->hide();
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, showHorizontalHeader)
{
	((QTableWidget *)widget())->horizontalHeader()->show();
	return true;
}

KVSO_CLASS_FUNCTION(tableWidget, showVerticalHeader)
{
	((QTableWidget *)widget())->verticalHeader()->show();
	return true;
}
KVSO_CLASS_FUNCTION(tableWidget, setItemFlags)
{
	CHECK_INTERNAL_POINTER(widget())

	QStringList itemflags;
	kvs_uint_t iRow, iCol;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("row", KVS_PT_UNSIGNEDINTEGER, 0, iRow)
	KVSO_PARAMETER("column", KVS_PT_UNSIGNEDINTEGER, 0, iCol)
	KVSO_PARAMETER("flags", KVS_PT_STRINGLIST, KVS_PF_OPTIONAL, itemflags)
	KVSO_PARAMETERS_END(c)
	int flag, sum = 0;
	for(int i = 0; i < itemflags.count(); i++)
	{
		flag = 0;
		for(unsigned int j = 0; j < itemflags_num; j++)
		{
			if(KviQString::equalCI(itemflags.at(i), itemflags_tbl[j]))
			{
				flag = item_flags[j];
				break;
			}
		}
		if(flag)
		{
			// if (flag==Qt::ItemIsUserCheckable)
			//	pItem->setCheckState(0,Qt::Unchecked);
			sum = sum | flag;
		}
		else
		{
			c->warning(__tr2qs_ctx("Unknown item flag '%Q'", "objects"), &itemflags.at(i));
		}
	}
	QTableWidgetItem * pItem = ((QTableWidget *)widget())->item(iRow, iCol);
	if(pItem)
		pItem->setFlags((Qt::ItemFlags)sum);
	return true;
}

void KvsObject_tableWidget::slotItemEntered(QTableWidgetItem * i)
{
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)i->row()), new KviKvsVariant((kvs_int_t)i->column()));
	callFunction(this, "itemEnteredEvent", nullptr, &params);
}

KVSO_CLASS_FUNCTION(tableWidget, itemEnteredEvent)
{
	emitSignal("itemEntered", c, c->params());
	return true;
}

void KvsObject_tableWidget::cellActivated(int iRow, int iCol)
{
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)iRow), new KviKvsVariant((kvs_int_t)iCol));
	callFunction(this, "cellActivatedEvent", nullptr, &params);
}

KVSO_CLASS_FUNCTION(tableWidget, cellActivatedEvent)
{
	emitSignal("cellActivated", c, c->params());
	return true;
}

void KvsObject_tableWidget::cellDoubleClicked(int iRow, int iCol)
{
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)iRow), new KviKvsVariant((kvs_int_t)iCol));
	callFunction(this, "cellDoubleClickedEvent", nullptr, &params);
}

KVSO_CLASS_FUNCTION(tableWidget, cellDoubleClickedEvent)
{
	emitSignal("cellDoubleClicked", c, c->params());
	return true;
}
bool KvsObject_tableWidget::paint(QPainter * p, const QStyleOptionViewItem & option, const QModelIndex & index)
{
	p->save();
	KviKvsObjectClass * pClass = KviKvsKernel::instance()->objectController()->lookupClass("painter");
	KviKvsVariantList params;
	KviKvsObject * pObject = pClass->allocateInstance(nullptr, "internalpainter", m_pContext, &params);
	((KvsObject_painter *)pObject)->setInternalPainter(p);
	p->setClipRect(option.rect);
	p->translate(option.rect.x(), option.rect.y());
	int col = index.column();
	int row = index.row();
	kvs_hobject_t handle = pObject->handle();
	kvs_int_t cell_height = (kvs_int_t)option.rect.height();
	kvs_int_t cell_width = (kvs_int_t)option.rect.width();
	KviKvsVariantList parameters(new KviKvsVariant(handle), new KviKvsVariant((kvs_int_t)row), new KviKvsVariant((kvs_int_t)col), new KviKvsVariant(cell_width), new KviKvsVariant(cell_height));
	bool ret = false;
	KviKvsVariant oReturnBuffer(ret);
	callFunction(this, "paintCellEvent", &oReturnBuffer, &parameters);
	pObject = KviKvsKernel::instance()->objectController()->lookupObject(handle);
	if(pObject)
		pObject->dieNow();
	p->restore();
	return oReturnBuffer.asBoolean();
}

KviCellItemDelegate::KviCellItemDelegate(QAbstractItemView * pWidget, KvsObject_tableWidget * parent)
    : QItemDelegate(pWidget), m_pParentScript(parent)
{
}

KviCellItemDelegate::~KviCellItemDelegate()
    = default;

void KviCellItemDelegate::paint(QPainter * pPainter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	if(m_pParentScript->paint(pPainter, option, index))
		QItemDelegate::paint(pPainter, option, index);
}

QSize KviCellItemDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	KviKvsVariant vSizeBuffer;

	KviKvsVariantList parameters(new KviKvsVariant((kvs_int_t)index.row()), new KviKvsVariant((kvs_int_t)index.column()));
	m_pParentScript->callFunction(m_pParentScript, "sizeHintCellRequestEvent", &vSizeBuffer, &parameters);
	if(vSizeBuffer.isArray())
	{
		if(vSizeBuffer.array()->size() == 2)
		{
			kvs_int_t w, h;
			if(vSizeBuffer.array()->at(0)->asInteger(w) && vSizeBuffer.array()->at(1)->asInteger(h))
				return QSize(w, h);
		}
	}
	return QItemDelegate::sizeHint(option, index);
}
