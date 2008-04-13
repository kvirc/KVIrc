//
//   File : class_groupbox.cpp
//   Creation date : Fri Jan 28 14:21:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2005 Szymon Stefanek (pragma at kvirc dot net)
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
		!fn: $setColumns(<columns:uint>)
		Sets the number of columns or rows (depending of the orientation) in the group box.
		!fn: <integer> $columns()
		Returns the number of columns or rows in the groupbox.
		!fn: $addSpace()
		Adds an empty cell at the next free position.
		!fn: <string> $alignment()
		Returns the alignment of the group box title.
		!fn: $setAlignment(<alignment:string>)
		Set the alignment of the groupbox;  Valid values are Left,Right,HCenter.
		!fn: $setOrientation<orientation:string>
		Sets the group box's orientation. Valid values are: Horizontal, Vertical.
		!fn: $setColumnLayout(<columns:integer>,<orientation:string>)
		Enables the automatic layout management. The children are arranged in n columns with the specified orientation.[br]
		Valid values for <orientation> are: Horizontal, Vertical.
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
	KVSO_REGISTER_HANDLER(KviKvsObject_groupbox,"setTitle", functionSetTitle)
	KVSO_REGISTER_HANDLER(KviKvsObject_groupbox,"title", functionTitle)
	KVSO_REGISTER_HANDLER(KviKvsObject_groupbox,"setFlat", functionSetFlat)
	KVSO_REGISTER_HANDLER(KviKvsObject_groupbox,"isFlat", functionIsFlat)
	KVSO_REGISTER_HANDLER(KviKvsObject_groupbox,"setCheckable", functionSetCheckable)
	KVSO_REGISTER_HANDLER(KviKvsObject_groupbox,"isCheckable", functionIsCheckable)
	KVSO_REGISTER_HANDLER(KviKvsObject_groupbox,"setInsideMargin", functionSetInsideMargin)
	KVSO_REGISTER_HANDLER(KviKvsObject_groupbox,"insideMargin", functionInsideMargin)
	KVSO_REGISTER_HANDLER(KviKvsObject_groupbox,"setInsideSpacing", functionSetInsideSpacing)
	KVSO_REGISTER_HANDLER(KviKvsObject_groupbox,"insideSpacing", functionInsideSpacing)
	KVSO_REGISTER_HANDLER(KviKvsObject_groupbox,"setColumns", functionSetColumns)
	KVSO_REGISTER_HANDLER(KviKvsObject_groupbox,"columns", functionColumns)
	KVSO_REGISTER_HANDLER(KviKvsObject_groupbox,"addSpace", functionAddSpace)
	KVSO_REGISTER_HANDLER(KviKvsObject_groupbox,"alignment", functionAlignment)
	KVSO_REGISTER_HANDLER(KviKvsObject_groupbox,"setAlignment", functionSetAlignment)
	KVSO_REGISTER_HANDLER(KviKvsObject_groupbox,"setOrientation", functionSetOrientation)
	KVSO_REGISTER_HANDLER(KviKvsObject_groupbox,"isChecked", functionIsChecked)
	KVSO_REGISTER_HANDLER(KviKvsObject_groupbox,"setChecked", functionSetChecked)
	KVSO_REGISTER_HANDLER(KviKvsObject_groupbox,"setColumnLayout",functionSetColumnLayout)
KVSO_END_REGISTERCLASS(KviKvsObject_groupbox)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_groupbox,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_groupbox)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_groupbox)

KVSO_END_CONSTRUCTOR(KviKvsObject_groupbox)

bool KviKvsObject_groupbox::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{
	KviTalGroupBox *groupbox=new KviTalGroupBox(name(),parentScriptWidget());
	groupbox->setObjectName(name());
	setObject(groupbox,true);
	return true;
}

bool KviKvsObject_groupbox::functionSetTitle(KviKvsObjectFunctionCall *c)
{
	QString szTitle;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("title",KVS_PT_STRING,0,szTitle)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((KviTalGroupBox *)widget())->setTitle(szTitle);
	return true;
}
bool KviKvsObject_groupbox::functionTitle(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setString(((KviTalGroupBox *)widget())->title());
	return true;
}
bool KviKvsObject_groupbox::functionSetFlat(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bFlag",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((KviTalGroupBox *)widget())->setFlat(bEnabled);
	return true;
}
bool KviKvsObject_groupbox::functionIsFlat(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setBoolean(((KviTalGroupBox *)widget())->isFlat());
	return true;
}
bool KviKvsObject_groupbox::functionSetCheckable(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bFlag",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((KviTalGroupBox *)widget())->setCheckable(bEnabled);
	return true;
}
bool KviKvsObject_groupbox::functionIsCheckable(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setBoolean(((KviTalGroupBox *)widget())->isCheckable());
	return true;
}
bool KviKvsObject_groupbox::functionSetChecked(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bFlag",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((KviTalGroupBox *)widget())->setChecked(bEnabled);
	return true;
}
bool KviKvsObject_groupbox::functionIsChecked(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setBoolean(((KviTalGroupBox *)widget())->isChecked());
	return true;
}
bool KviKvsObject_groupbox::functionSetInsideMargin(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uMargin;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("margin",KVS_PT_UNSIGNEDINTEGER,0,uMargin)
	KVSO_PARAMETERS_END(c)
    if (widget()) ((KviTalGroupBox *)widget())->setInsideMargin(uMargin);
	return true;
}
bool KviKvsObject_groupbox::functionInsideMargin(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setInteger(((KviTalGroupBox *)widget())->insideMargin());
	return true;
}
bool KviKvsObject_groupbox::functionSetInsideSpacing(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uSpacing;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("spacing",KVS_PT_UNSIGNEDINTEGER,0,uSpacing)
	KVSO_PARAMETERS_END(c)
    if (widget()) ((KviTalGroupBox *)widget())->setInsideSpacing(uSpacing);
	return true;
}
bool KviKvsObject_groupbox::functionInsideSpacing(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setInteger(((KviTalGroupBox *)widget())->insideSpacing());
	return true;
}
bool KviKvsObject_groupbox::functionSetColumns(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uColums;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("colums",KVS_PT_UNSIGNEDINTEGER,0,uColums)
	KVSO_PARAMETERS_END(c)
    if (widget()) ((KviTalGroupBox *)widget())->setColumns(uColums);
	return true;
}
bool KviKvsObject_groupbox::functionColumns(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setInteger(((KviTalGroupBox *)widget())->columns());
	return true;
}
bool KviKvsObject_groupbox::functionAddSpace(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t iSpace;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("colums",KVS_PT_UNSIGNEDINTEGER,0,iSpace)
	KVSO_PARAMETERS_END(c)
	if (widget()) (((KviTalGroupBox *)widget())->addSpace(iSpace));
	return true;
}

bool KviKvsObject_groupbox::functionSetAlignment(KviKvsObjectFunctionCall *c)
{
	QString szAlign;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("alignment",KVS_PT_STRING,0,szAlign)
	KVSO_PARAMETERS_END(c)
	if (!widget()) return true;
	for(unsigned int i = 0; i < align_num; i++)
	{
		if(KviQString::equalCI(szAlign, align_tbl[i]))
		{
			((KviTalGroupBox *)widget())->setAlignment(align_cod[i]);
			return true;
		}
	}
	c->warning(__tr2qs("Unknown alignment"));
	return true;
}
bool KviKvsObject_groupbox::functionAlignment(KviKvsObjectFunctionCall *c)
{
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

bool KviKvsObject_groupbox::functionSetOrientation(KviKvsObjectFunctionCall *c)
{
	QString szMode;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("orientation",KVS_PT_STRING,0,szMode)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	if(KviQString::equalCI(szMode, "Horizontal"))
		((KviTalGroupBox *)widget())->setOrientation(Qt::Horizontal);
	else
	if(KviQString::equalCI(szMode, "Vertical"))
		((KviTalGroupBox *)widget())->setOrientation(Qt::Vertical);
	else c->warning( __tr2qs("Unknown orientation: "));
	return true;
}

bool KviKvsObject_groupbox::functionSetColumnLayout(KviKvsObjectFunctionCall *c)
{
	QString szMode;
	kvs_uint_t uCol;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("columns",KVS_PT_UNSIGNEDINTEGER,0,uCol)
		KVSO_PARAMETER("orientation",KVS_PT_STRING,0,szMode)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	if(szMode=="Horizontal")
		((KviTalGroupBox *)widget())->setColumnLayout(uCol,Qt::Horizontal);
	else
		if(szMode=="Vertical")
			((KviTalGroupBox *)widget())->setColumnLayout(uCol,Qt::Vertical);
		else c->warning( __tr2qs("Unknown orientation: "));
	return true;
}

