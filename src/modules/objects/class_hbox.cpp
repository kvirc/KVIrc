//=============================================================================
//
//   File : class_hbox.cpp
//   Creation date : Wed Mar 01 2005 23:00:01 by Alessandro Carbone & Tonino Imbesi
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

#define _KVI_DEBUG_CHECK_RANGE_
#include "kvi_debug.h"
#include "kvi_locale.h"
#include "class_hbox.h"
#include "kvi_tal_hbox.h"

const char * const align_tbl[] = {
			"Left", 
			"Right",
			"HCenter",
			"VCenter",
			"Center",
			"Top",
			"Bottom",
			"WordBreak"
			   };



const int align_cod[] = {
		Qt::AlignLeft,
		Qt::AlignRight,
	    Qt::AlignHCenter,
	    Qt::AlignVCenter,
	    Qt::AlignCenter,
	 	Qt::AlignTop,
	    Qt::AlignBottom,
	    Qt::AlignJustify,
	};

#define align_num	(sizeof(align_tbl) / sizeof(align_tbl[0]))

/*
	@doc: hbox
	@keyterms:
		hbox object class, child widgets
	@title:
		hbox class
	@type:
		class
	@short:
		Manages child widget horizontal geometry
	@inherits:
		[class]object[/class]
	@description:
		The hbox class widget provides horizontal geometry management for its child widgets.
	@functions:
		!fn: $setSpacing(<spacing:uint>)
		Sets the default spacing of the widgets in pixels
		!fn: $setMargin(<margin:uint>)
		Sets the dimension of the layout margin : the distance from the border to the outermost child widget edges.
		!fn: $setStretchFactor(<widget:hobject>,<stretch:uint>)
		Sets the stretch factor of widget to stretch.
		!fn: $addStretch(<stretch:integer>)
		Adds a stretchable space with zero minimum size and stretch factor stretch to the end of this box layout.
		!fn: $setAlignment(<flag1:string>, <flag2:string>, ...)
		Sets the alignment for widget w to  flags, given as parameters. 
		Valid flags are:Right,Left,Top,Bottom,HCenter,VCenter,Center  
*/


KVSO_BEGIN_REGISTERCLASS(KviKvsObject_hbox,"hbox","widget")
	KVSO_REGISTER_HANDLER(KviKvsObject_hbox,"setMargin", functionsetMargin)
	KVSO_REGISTER_HANDLER(KviKvsObject_hbox,"setSpacing", functionsetSpacing)
	KVSO_REGISTER_HANDLER(KviKvsObject_hbox,"setStretchFactor", functionsetStretchFactor )
	KVSO_REGISTER_HANDLER(KviKvsObject_hbox,"addStretch", functionaddStretch )
	KVSO_REGISTER_HANDLER(KviKvsObject_hbox,"setAlignment", functionsetAlignment )
KVSO_END_REGISTERCLASS(KviKvsObject_hbox)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_hbox,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_hbox)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_hbox)

KVSO_END_CONSTRUCTOR(KviKvsObject_hbox)

bool KviKvsObject_hbox::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{
	
	setObject(new KviTalHBox(parentScriptWidget()), true);
	return true;
}

bool KviKvsObject_hbox::functionsetMargin(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uMargin;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("margin",KVS_PT_UNSIGNEDINTEGER,0,uMargin)
	KVSO_PARAMETERS_END(c)
	if (widget()) ((KviTalHBox *)widget())->setMargin(uMargin);
	return true;
}

bool KviKvsObject_hbox::functionsetSpacing(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uSpacing;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("spacing",KVS_PT_UNSIGNEDINTEGER,0,uSpacing)
	KVSO_PARAMETERS_END(c)
	if (widget()) ((KviTalHBox *)widget())->setSpacing(uSpacing);
	return true;
}

bool KviKvsObject_hbox::functionsetStretchFactor(KviKvsObjectFunctionCall *c)
{
	KviKvsObject * pObject;
	kvs_hobject_t hObject;
	kvs_uint_t uStretch;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("stretch",KVS_PT_UNSIGNEDINTEGER,0,uStretch)
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
	if(((KviKvsObject_widget *)pObject)->widget()->parentWidget() != widget())
	{
		c->warning(__tr2qs("The widget must be a child of this hbox"));
		return true;
	}
	((KviTalHBox *)widget())->setStretchFactor(((QWidget *)(pObject->object())),uStretch);
	return true;
}
bool KviKvsObject_hbox::functionaddStretch(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iStretch;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("stretch",KVS_PT_INT,0,iStretch)
	KVSO_PARAMETERS_END(c)
	if (widget()) ((KviTalHBox *)widget())->addStretch(iStretch);
	return true;
}
bool KviKvsObject_hbox::functionsetAlignment(KviKvsObjectFunctionCall *c)
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
	if(((KviKvsObject_widget *)pObject)->widget()->parentWidget() != widget())
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
	if (widget()) ((KviTalHBox *)widget())->setAlignment(((QWidget *)(pObject->object())),(Qt::Alignment)sum);
	return true;
}

