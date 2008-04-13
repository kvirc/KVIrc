//
//   File : class_vbox.cpp
//   Creation date : Wed Mar 01 2005 23:00:01 by Alessandro Carbone & Tonino Imbesi
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2006 Szymon Stefanek (pragma at kvirc dot net)
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

#include "class_vbox.h"

#include "kvi_tal_vbox.h"
/*
	@doc: vbox
	@keyterms:
		vbox object class, child widgets
	@title:
		vbox class
	@type:
		class
	@short:
		Manages child widget vertical geometry
	@inherits:
		[class]object[/class]
	@description:
		The vbox class widget provides vertical geometry management for its child widgets.
	@functions:
		!fn: $setSpacing(<spacing:int>)
		Sets the default spacing of the widgets in pixels
		!fn: $setMargin(<margin:int>)
		Sets the dimension of the layout margin : the distance from the border to the outermost child widget edges.
		!fn: $setStretchFactor(<widget:hobject>,<stretch:uint>)
		Sets the stretch factor of widget to stretch.
*/


KVSO_BEGIN_REGISTERCLASS(KviKvsObject_vbox,"vbox","widget")
	KVSO_REGISTER_HANDLER(KviKvsObject_vbox,"setMargin", functionsetMargin)
	KVSO_REGISTER_HANDLER(KviKvsObject_vbox,"setSpacing", functionsetSpacing)
	KVSO_REGISTER_HANDLER(KviKvsObject_vbox,"setStretchFactor", functionsetStretchFactor )
KVSO_END_REGISTERCLASS(KviKvsObject_vbox)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_vbox,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_vbox)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_vbox)

KVSO_END_CONSTRUCTOR(KviKvsObject_vbox)

bool KviKvsObject_vbox::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{
	SET_OBJECT(KviTalVBox);
	return true;
}

bool KviKvsObject_vbox::functionsetMargin(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iMargin;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("margin",KVS_PT_INT,0,iMargin)
	KVSO_PARAMETERS_END(c)
	if (widget()) ((KviTalVBox *)widget())->setMargin(iMargin);
	return true;
}

bool KviKvsObject_vbox::functionsetSpacing(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iSpacing;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("spacing",KVS_PT_INT,0,iSpacing)
	KVSO_PARAMETERS_END(c)
	if (widget()) ((KviTalVBox *)widget())->setSpacing(iSpacing);
	return true;
}

bool KviKvsObject_vbox::functionsetStretchFactor(KviKvsObjectFunctionCall *c)
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
		c->warning(__tr2qs("The widget must be a child of this vbox"));
		return true;
	}
	((KviTalVBox *)widget())->setStretchFactor(((QWidget *)(pObject->object())),uStretch);
	return true;
}
