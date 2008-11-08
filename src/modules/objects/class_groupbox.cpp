//=============================================================================
//
//   File : class_groupbox.cpp
//   Creation date : Fri Jan 28 14:21:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
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

#include <kvi_tal_groupbox.h>
#include "class_groupbox.h"
#include "kvi_error.h"
#include "kvi_debug.h"

#include "kvi_locale.h"
#include "kvi_iconmanager.h"


// Tables used in $setAlignment , $alignment and in $setOrientation & $orientation

const char * const align_tbl[] = {
	"Left",
	"Right",
	"HCenter"
};

const int align_cod[] = {
	Qt::AlignLeft,
	Qt::AlignRight,
	Qt::AlignHCenter
};

#define align_num	(sizeof(align_tbl) / sizeof(align_tbl[0]))


/*
	@doc:	groupbox
	@keyterms:
		groupbox object class,
	@title:
		groupbox class
	@type:
		class
	@short:
		Provides a groupbox bar.
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		This widget can be used to display a groupbox.
		It will be usually a parent for other child controls.
		You can either use a child layout to manage the children geometries
		or use $setColumnLayout to manage the layout automatically.
	@functions:
		!fn: $setTitle(<text:String>)
		Sets the group box title to <text>.
		!fn: <string> $title()
		Returns the group box title text.
		!fn: $setFlat(<bflag:boolean>)
		Sets whether the group box is painted flat. Valid Values are 1 or 0.
		!fn: <boolean> $isFlat()
		Returns 1 (TRUE) if the group box is painted flat; otherwise returns 0 (FALSE).
		!fn: <boolean> $isCheckable()
		Returns 1 (TRUE) if the group box has a checkbox in its title; otherwise returns 0 (FALSE).
		!fn: $setCheckable(<bflag:boolean>)
		Sets whether the group box has a checkbox in its title: Valid values are 1 or 0.
		!fn: $setInsideMargin(<margin:uint>)
		Sets the the width of the inside margin to m pixels.
		!fn: <integer> $insideMargin()
		Returns the width of the empty space between the items in the group and margin of groupbox.
		!fn: $setInsideSpacing(<spacing:uint>)
		Sets the width of the empty space between each of the items in the group to m pixels.
		!fn: <integer> $insideSpacing()
		Returns the width of the empty space between each of the items in the group.
		!fn: $addSpace()
		Adds an empty cell at the next free position.
		!fn: <string> $alignment()
		Returns the alignment of the group box title.
		!fn: $setAlignment(<alignment:string>)
		Set the alignment of the groupbox;  Valid values are Left,Right,HCenter.
		!fn: $setOrientation<orientation:string>
		Sets the group box's orientation. Valid values are: Horizontal, Vertical.
	@examples:
		[example]
		|-Start:[br]
		#Let's start.[br]
		#first we'll create the main widget.[br]
		%widget=$new(widget)[br]
		[br]
		#then the groupbox [br]
		%gb=$new(groupbox,%widget)[br]
		%gb->$setTitle(Login)[br]
		%gb->$setAlignment("Left")[br]
		[br]
		#now we create the labels and lineedits.[br]
		%labeluser=$new(label,%gb)[br]
		%labeluser->$settext(User: )[br]
		%labelpass=$new(label,%gb)[br]
		%labelpass->$settext(Pass: )[br]
		%inputuser=$new(lineedit,%gb)[br]
		%inputpass=$new(lineedit,%gb)[br]
		%inputpass->$setechomode("password")[br]
		[br]
		#now lets' layouting the groupbox's element's.[br]
		%layoutgb=$new(layout,%gb)[br]
		%layoutgb->$setmargin(20)[br]
		%layoutgb->$addwidget(%labeluser,0,0)[br]
		%layoutgb->$addwidget(%labelpass,1,0)[br]
		%layoutgb->$addwidget(%inputuser,0,1)[br]
		%layoutgb->$addwidget(%inputpass,1,1)[br]
		[br]
		# now we create a fake widget and managing the two buttons layout.[br]
		%fakewidget=$new(widget,%widget)[br]
		%layoutbtn=$new(layout,%fakewidget)[br]
		%btnok=$new(button,%fakewidget)[br]
		%btnok->$settext("OK")[br]
		%btncancel=$new(button,%fakewidget)[br]
		%btncancel->$settext("Cancel")[br]
		%layoutbtn->$addwidget(%btnok,0,0)[br]
		%layoutbtn->$addwidget(%btncancel,0,1)[br]
		[br]
		#And finally we create a main layout with the groupbox (and its "children")[br]
		#and fakewiget (with its buttons children).
		%mainlayout=$new(layout,%widget)[br]
		%mainlayout->$setspacing(10)[br]
		%mainlayout->$setmargin(10)[br]
		%mainlayout->$addwidget(%gb,0,0)[br]
		%mainlayout->$addwidget(%fakewidget,1,0)[br]
		[br]
		#Let's show our nice login request =D ! [br]
		%widget->$show()[br]
	[/example]

*/

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_groupbox,"groupbox","widget")
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_groupbox,setTitle)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_groupbox,title)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_groupbox,setFlat)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_groupbox,isFlat)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_groupbox,setCheckable)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_groupbox,isCheckable)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_groupbox,setInsideMargin)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_groupbox,insideMargin)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_groupbox,setInsideSpacing)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_groupbox,insideSpacing)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_groupbox,addSpace)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_groupbox,alignment)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_groupbox,setAlignment)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_groupbox,setOrientation)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_groupbox,setOrientation)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_groupbox,isChecked)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_groupbox,setChecked)

KVSO_END_REGISTERCLASS(KviKvsObject_groupbox)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_groupbox,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_groupbox)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_groupbox)

KVSO_END_CONSTRUCTOR(KviKvsObject_groupbox)

bool KviKvsObject_groupbox::init(KviKvsRunTimeContext *,KviKvsVariantList *)
{
	KviTalGroupBox *groupbox=new KviTalGroupBox(getName(),parentScriptWidget());

	groupbox->setOrientation(Qt::Horizontal);
	groupbox->setObjectName(getName());
	setObject(groupbox,true);
	return true;
}

KVSO_CLASS_FUNCTION(groupbox,setTitle)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szTitle;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("title",KVS_PT_STRING,0,szTitle)
	KVSO_PARAMETERS_END(c)
	((KviTalGroupBox *)widget())->setTitle(szTitle);
	return true;
}
KVSO_CLASS_FUNCTION(groupbox,title)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setString(((KviTalGroupBox *)widget())->title());
	return true;
}
KVSO_CLASS_FUNCTION(groupbox,setFlat)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bFlag",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	((KviTalGroupBox *)widget())->setFlat(bEnabled);
	return true;
}
KVSO_CLASS_FUNCTION(groupbox,isFlat)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setBoolean(((KviTalGroupBox *)widget())->isFlat());
	return true;
}
KVSO_CLASS_FUNCTION(groupbox,setCheckable)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bFlag",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	((KviTalGroupBox *)widget())->setCheckable(bEnabled);
	return true;
}
KVSO_CLASS_FUNCTION(groupbox,isCheckable)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setBoolean(((KviTalGroupBox *)widget())->isCheckable());
	return true;
}
KVSO_CLASS_FUNCTION(groupbox,setChecked)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bFlag",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	((KviTalGroupBox *)widget())->setChecked(bEnabled);
	return true;
}
KVSO_CLASS_FUNCTION(groupbox,isChecked)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setBoolean(((KviTalGroupBox *)widget())->isChecked());
	return true;
}
KVSO_CLASS_FUNCTION(groupbox,setInsideMargin)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uMargin;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("margin",KVS_PT_UNSIGNEDINTEGER,0,uMargin)
	KVSO_PARAMETERS_END(c)
	((KviTalGroupBox *)widget())->setInsideMargin(uMargin);
	return true;
}
KVSO_CLASS_FUNCTION(groupbox,insideMargin)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((KviTalGroupBox *)widget())->insideMargin());
	return true;
}
KVSO_CLASS_FUNCTION(groupbox,setInsideSpacing)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uSpacing;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("spacing",KVS_PT_UNSIGNEDINTEGER,0,uSpacing)
	KVSO_PARAMETERS_END(c)
    ((KviTalGroupBox *)widget())->setInsideSpacing(uSpacing);
	return true;
}
KVSO_CLASS_FUNCTION(groupbox,insideSpacing)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((KviTalGroupBox *)widget())->insideSpacing());
	return true;
}

KVSO_CLASS_FUNCTION(groupbox,addSpace)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t iSpace;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("colums",KVS_PT_UNSIGNEDINTEGER,0,iSpace)
	KVSO_PARAMETERS_END(c)
	(((KviTalGroupBox *)widget())->addSpace(iSpace));
	return true;
}

KVSO_CLASS_FUNCTION(groupbox,setAlignment)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szAlign;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("alignment",KVS_PT_STRING,0,szAlign)
	KVSO_PARAMETERS_END(c)
	for(unsigned int i = 0; i < align_num; i++)
	{
		if(KviQString::equalCI(szAlign, align_tbl[i]))
		{
			((KviTalGroupBox *)widget())->setAlignment(align_cod[i]);
			return true;
		}
	}
	c->warning(__tr2qs_ctx("Unknown alignment '%Q'","objets"),&szAlign);
	return true;
}

KVSO_CLASS_FUNCTION(groupbox,alignment)
{
	CHECK_INTERNAL_POINTER(widget())
	int mode = ((KviTalGroupBox *)widget())->alignment();
	QString szAlignment="";
	for(unsigned int i = 0; i < align_num; i++)
	{
		if(mode == align_cod[i])
		{
			szAlignment=align_tbl[i];
			break;
		}
	}
	c->returnValue()->setString(szAlignment);
	return true;
}

KVSO_CLASS_FUNCTION(groupbox,setOrientation)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szMode;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("orientation",KVS_PT_STRING,0,szMode)
	KVSO_PARAMETERS_END(c)
	if(KviQString::equalCI(szMode, "Horizontal"))
		((KviTalGroupBox *)widget())->setOrientation(Qt::Vertical);
	else
	if(KviQString::equalCI(szMode, "Vertical"))
		((KviTalGroupBox *)widget())->setOrientation(Qt::Horizontal);
	else c->warning( __tr2qs_ctx("Unknown orientation '%Q'","objects"),&szMode);

	return true;
}
