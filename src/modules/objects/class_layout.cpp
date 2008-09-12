//
//   File : class_layout.cpp
//   Creation date : Fri Now 22 2002 00:50:01 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
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
#define _KVI_DEBUG_CHECK_RANGE_
#include "kvi_debug.h"
#include "kvi_locale.h"
#include "class_layout.h"

#include <QGridLayout>
#define QLAYOUT_AUTO_CONSTRAINT QLayout::SetDefaultConstraint
#define QLAYOUT_FIXED QLayout::SetFixedSize
#define QLAYOUT_FREE_RESIZE QLayout::SetNoConstraint
#define QLAYOUT_MINIMUM QLayout::SetMinimumSize


// Tables used in $setAlignment & $alignment
const char * const align_tbl[] = {
			"Left", 
			"Right",
			"HCenter",
			"VCenter",
			"Center",
			"Top",
			"Bottom",
			   };



const int align_cod[] = {
		Qt::AlignLeft,
		Qt::AlignRight,
	    Qt::AlignHCenter,
	    Qt::AlignVCenter,
	    Qt::AlignCenter,
	 	Qt::AlignTop,
	    Qt::AlignBottom,
	};
#define align_num	(sizeof(align_tbl) / sizeof(align_tbl[0]))
/*
	@doc: layout
	@keyterms:
		layout object class, child widgets
	@title:
		layout class
	@type:
		class
	@short:
		Manages child widget geometry
	@inherits:
		[class]object[/class]
	@description:
		The layout is a geometry management tool for child widgets.
		You create a layout , give it some widgets to manage and it will layout them
		automatically.[br]
		The parent of the layout must be the widget for which child widget geometries have to be managed.
		A layout is a grid of NxM cells in which you insert child widgets with [classfnc:layout]$addWidget[/classfnc]().[br]
		Widgets that must span multiple cells can be added to the layout with [classfnc:layout]$addMultiCellWidget[/classfnc]().[br]
	@functions:
		!fn: $addWidget(<widget:object widget>,<row:uint>,<column:uint>)
		Adds a widget to this layout placing it at position <row>,<column> in the grid
		!fn: $addMultiCellWidget(<widget:object widget>,<start_row:uint>,<end_row:uint>,<start_col:uint>,<end_col:uint>)
		Adds a widget to this layout spanning multiple grid cells
		!fn: $setRowStretch(<row:uint>,<stretch:uint>)
		Sets the stretch value for a particular row of this layout. The <stretch_value>
		must be a positive integer. The rows with bigger stretch values will take more space
		in the layout.
		!fn: $setColStretch(<column:uint>,<stretch:uint>)
		Sets the stretch value for a particular column in this layout. The <stretch_value>
		must be a positive integer. The rows with bigger stretch values will take more space
		in the layout.
		!fn: $addRowSpacing(<row:uint>,<spacing:uint>)
		Sets the minimum height of the specified <row> to <spacing> which must be a positive integer
		!fn: $addColSpacing(<column:uint>,<spacing:uint>)
		Sets the minimum width of the specigfied <column> to <spacing> which must be a positive integer
		!fn: $setSpacing(<spacing:uint>)
		Sets the default spacing of the widgets in pixels
		!fn: $setMargin(<margin:uint>)
		Sets the dimension of the layout margin : the distance from the border to the outermost child widget edges.
		!fn: $setAlignment(<flag1:string>, <flag2:string>, ...)
		Sets the alignment for widget w to  flags, given as parameters. 
		Valid flags are:Right,Left,Top,Bottom,HCenter,VCenter,Center  
		!fn: $setResizeMode(<resize_mode:string>)
		Sets the resize mode of the parent widget in relation to this layout.
		<mode> can be one of:[br]
		-Auto: this is the default[br]
		-Fixed: the parent widget of this layout is resized to the "sizeHint" value and it cannot be resized by the user.[br]
		-Minimum: the minimum size of the parent widget of this layout is set to minimumSize() and it cannot be smaller[br]
		-FreeResize: the parent widget of this layout is not constrained at all[br]
*/


KVSO_BEGIN_REGISTERCLASS(KviKvsObject_layout,"layout","object")
	KVSO_REGISTER_HANDLER(KviKvsObject_layout,"addWidget", functionAddWidget)
	KVSO_REGISTER_HANDLER(KviKvsObject_layout,"addMultiCellWidget", functionAddMultiCellWidget)
	KVSO_REGISTER_HANDLER(KviKvsObject_layout,"setRowStretch", functionSetRowStretch)
	KVSO_REGISTER_HANDLER(KviKvsObject_layout,"setColumnStretch", functionSetColumnStretch)
	KVSO_REGISTER_HANDLER(KviKvsObject_layout,"addRowSpacing", functionAddRowSpacing)
	KVSO_REGISTER_HANDLER(KviKvsObject_layout,"addColSpacing", functionAddColSpacing)
	KVSO_REGISTER_HANDLER(KviKvsObject_layout,"setMargin", functionSetMargin)
	KVSO_REGISTER_HANDLER(KviKvsObject_layout,"setSpacing", functionSetSpacing)
	KVSO_REGISTER_HANDLER(KviKvsObject_layout,"setResizeMode", functionSetResizeMode)
	KVSO_REGISTER_HANDLER(KviKvsObject_layout,"setAlignment", functionsetAlignment )
KVSO_END_REGISTERCLASS(KviKvsObject_layout)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_layout,KviKvsObject)

KVSO_END_CONSTRUCTOR(KviKvsObject_layout)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_layout)

KVSO_END_CONSTRUCTOR(KviKvsObject_layout)

bool KviKvsObject_layout::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{
	QWidget * w = parentScriptWidget();

	if(!w)
	{
		pContext->warning(__tr2qs("The parent of a layout must be a widget!"));
		return false;
	}
	setObject(new QGridLayout(w));
	setObjectName(getName());
	return true;
}

bool KviKvsObject_layout::functionAddWidget(KviKvsObjectFunctionCall *c)
{
	KviKvsObject * pObject;
	kvs_hobject_t hObject;
	kvs_uint_t uCol,uRow;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("row",KVS_PT_UNSIGNEDINTEGER,0,uRow)
		KVSO_PARAMETER("col",KVS_PT_UNSIGNEDINTEGER,0,uCol)
	KVSO_PARAMETERS_END(c)
	pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!widget())return true;
	if (!pObject)
	{
		c->warning(__tr2qs("Widget parameter is not an object"));
		return true;
	}
	if (!pObject->object())
	{
		c->warning(__tr2qs("Widget parameter is not a valid object"));
		return true;
	}
	if(!pObject->object()->isWidgetType())
	{
		c->warning(__tr2qs("Can't add a non-widget object"));
		return true;
	}
	((QGridLayout *)object())->addWidget(((QWidget *)(pObject->object())),uRow,uCol);
	return true;
}

bool KviKvsObject_layout::functionAddMultiCellWidget(KviKvsObjectFunctionCall *c)
{
	KviKvsObject * pObject;
	kvs_hobject_t hObject;
	kvs_uint_t uStartCol,uStartRow,uEndCol,uEndRow;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("start_row",KVS_PT_UNSIGNEDINTEGER,0,uStartRow)
		KVSO_PARAMETER("end_row",KVS_PT_UNSIGNEDINTEGER,0,uEndRow)
		KVSO_PARAMETER("start_column",KVS_PT_UNSIGNEDINTEGER,0,uStartCol)
		KVSO_PARAMETER("end_column",KVS_PT_UNSIGNEDINTEGER,0,uEndCol)
	KVSO_PARAMETERS_END(c)
	pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!widget())return true;
	if (!pObject)
	{
		c->warning(__tr2qs("Widget parameter is not an object"));
		return true;
	}
	if (!pObject->object())
	{
		c->warning(__tr2qs("Widget parameter is not a valid object"));
		return true;
	}
	if(!pObject->object()->isWidgetType())
	{
		c->warning(__tr2qs("Can't add a non-widget object"));
		return true;
	}
	// { addWidget(w, fromRow, fromCol, (toRow < 0) ? -1 : toRow - fromRow + 1, (toCol < 0) ? -1 : toCol - fromCol + 1, _align); }
	((QGridLayout *)object())->addWidget(((QWidget *)(pObject->object())),uStartRow, uStartCol,(uEndRow < 0) ? -1 : uEndRow - uStartRow + 1, (uEndCol < 0) ? -1 : uEndCol - uStartCol + 1);
	return true;
}

bool KviKvsObject_layout::functionSetRowStretch(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uRow,uStretch;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("row",KVS_PT_UNSIGNEDINTEGER,0,uRow)
		KVSO_PARAMETER("stretch",KVS_PT_UNSIGNEDINTEGER,0,uStretch)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	((QGridLayout *)object())->setRowStretch(uRow,uStretch);
	return true;
}

bool KviKvsObject_layout::functionSetColumnStretch(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uCol,uStretch;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("column",KVS_PT_UNSIGNEDINTEGER,0,uCol)
		KVSO_PARAMETER("stretch",KVS_PT_UNSIGNEDINTEGER,0,uStretch)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	((QGridLayout *)object())->setColumnStretch(uCol,uStretch);
	return true;
}

bool KviKvsObject_layout::functionSetMargin(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uMargin;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("margin",KVS_PT_UNSIGNEDINTEGER,0,uMargin)
	KVSO_PARAMETERS_END(c)
    if (widget()) ((QGridLayout *)object())->setMargin(uMargin);
	return true;
}

bool KviKvsObject_layout::functionSetSpacing(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uSpacing;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("spacing",KVS_PT_UNSIGNEDINTEGER,0,uSpacing)
	KVSO_PARAMETERS_END(c)
    if (widget()) ((QGridLayout *)object())->setSpacing(uSpacing);
	return true;
}

bool KviKvsObject_layout::functionAddRowSpacing(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uSpacing,uRow;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("row",KVS_PT_UNSIGNEDINTEGER,0,uRow)
		KVSO_PARAMETER("spacing",KVS_PT_UNSIGNEDINTEGER,0,uSpacing)
	KVSO_PARAMETERS_END(c)
    if (widget()) //((QGridLayout *)object())->addRowSpacing(uRow,uSpacing);
	     ((QGridLayout *)object())->addItem(new QSpacerItem(0, uSpacing), uRow, 0);
	return true;
}

bool KviKvsObject_layout::functionAddColSpacing(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uSpacing,uCol;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("column",KVS_PT_UNSIGNEDINTEGER,0,uCol)
		KVSO_PARAMETER("spacing",KVS_PT_UNSIGNEDINTEGER,0,uSpacing)
	KVSO_PARAMETERS_END(c)
    if (widget()) ((QGridLayout *)object())->addItem(new QSpacerItem(uSpacing,0), 0, uCol);
	return true;
}

bool KviKvsObject_layout::functionSetResizeMode(KviKvsObjectFunctionCall *c)
{
	QString szMode;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("resize_mode",KVS_PT_STRING,0,szMode)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	QLayout::SizeConstraint r = QLAYOUT_AUTO_CONSTRAINT;
	if(KviQString::equalCI(szMode,"FreeResize")) r = QLAYOUT_FREE_RESIZE;
	else if(KviQString::equalCI(szMode,"Minimum")) r = QLAYOUT_MINIMUM;
	else if(KviQString::equalCI(szMode,"Fixed"))r = QLAYOUT_FIXED;
	else c->warning(__tr2qs("Invalid resize mode defaulting to Auto"));
	((QGridLayout *)object())->setSizeConstraint(r);
	return true;

}

bool KviKvsObject_layout::functionsetAlignment(KviKvsObjectFunctionCall *c)
{
	QStringList alignment;
	KviKvsObject * pObject;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("alignment",KVS_PT_STRINGLIST,KVS_PF_OPTIONAL,alignment)
	KVSO_PARAMETERS_END(c)
	pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!widget())return true;
	if (!pObject)
	{
		c->warning(__tr2qs("Widget parameter is not an object"));
		return true;
	}
	if (!pObject->object())
	{
		c->warning(__tr2qs("Widget parameter is not a valid object"));
		return true;
	}
	if(!pObject->object()->isWidgetType())
	{
		c->warning(__tr2qs("Can't add a non-widget object"));
		return true;
	}
	int index=((QGridLayout *)widget())->indexOf(((QWidget *)(pObject->object())));
	if(index ==-1)
	{
		c->warning(__tr2qs("The widget must be a child of this hbox"));
		return true;
	}

	int align,sum=0;
	for ( QStringList::Iterator it = alignment.begin(); it != alignment.end(); ++it )
		{
		
			align = 0;
			for(unsigned int j = 0; j < align_num; j++)
			{
				if(KviQString::equalCI((*it), align_tbl[j]))
				{
					align=align_cod[j];
					break;
				}
			}
			if(align)
				sum = sum | align;
			else
				c->warning(__tr2qs("Unknown alignment: '%Q'"),&(*it));
			
		}
	if (widget()) ((QGridLayout *)widget())->setAlignment(((QWidget *)(pObject->object())),(Qt::Alignment)sum);
	return true;
}