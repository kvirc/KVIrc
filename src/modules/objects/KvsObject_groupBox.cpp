//=============================================================================
//
//   File : KvsObject_groupBox.cpp
//   Creation date : Fri Jan 28 14:21:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2009 Alessandro Carbone (elfonol at gmail dot com)
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

#include <KviTalGroupBox.h>
#include "KvsObject_groupBox.h"
#include "KviError.h"
#include "kvi_debug.h"

#include "KviLocale.h"
#include "KviIconManager.h"

// Tables used in $setAlignment, $alignment and in $setOrientation & $orientation

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

#define align_num (sizeof(align_tbl) / sizeof(align_tbl[0]))

/*
	@doc:	groupbox
	@keyterms:
		groupbox, group
	@title:
		groupbox object class
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
		You can either use a child layout to manage the child geometries
		or use $setColumnLayout to manage the layout automatically.
	@functions:
		!fn: $setTitle(<text:String>)
		Sets the group box title to <text>.
		!fn: <string> $title()
		Returns the groupbox title text.
		!fn: $setFlat(<bflag:boolean>)
		Sets whether the group box is painted flat. Valid Values are [b]1[/b] or [b]0[/b].
		!fn: <boolean> $isFlat()
		Returns [b]1[/b] (true) if the groupbox is painted flat; otherwise returns [b]0[/b] (false).
		!fn: <boolean> $isCheckable()
		Returns [b]1[/b] (true) if the group box has a checkbox in its title; otherwise returns [b]0[/b] (false).
		!fn: $setCheckable(<bflag:boolean>)
		Sets whether the groupbox has a checkbox in its title: Valid values are [b]1[/b] or [b]0[/b].
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
		Returns the alignment of the groupbox title.
		!fn: $setAlignment(<alignment:string>)
		Set the alignment of the groupbox;  Valid values are: Left, Right, HCenter.
		!fn: $setOrientation<orientation:string>
		Sets the groupbox's orientation. Valid values are: Horizontal, Vertical.
	@examples:
		[example]
			[comment]# First we'll create the main widget. as a dialog[/comment]
			%widget=$new(dialog)
			%layout=$new(layout,%widget)
			[comment]# Then the groupbox[/comment]
			%gb=$new(groupbox,%widget)
			%gb->$setTitle(Login)
			%gb->$setAlignment("Left")
			[comment]# Add the groupbox to the main layout[/comment]
			%layout->$addWidget(%gb,0,0)
			[comment]
			# Now we create the user field
			# (labels + lineedit) in a horizontal box
			[/comment]
			%hbox=$new(hbox,%gb)
			%labeluser=$new(label,%hbox)
			%labeluser->$settext(User: )
			%inputuser=$new(lineedit,%hbox)
			[comment]
			# Now we create the password field
			# (labels + lineedit) in a horizontal box
			[/comment]
			%hbox=$new(hbox,%gb)
			%labelpass=$new(label,%hbox)
			%labelpass->$settext(Pass: )
			%inputpass=$new(lineedit,%hbox)
			%inputpass->$setechomode("password")
			[comment]# Now we create the OK/Cancel box buttons[/comment]
			%hbox=$new(hbox,%gb)
			%btnok=$new(button,%hbox)
			%btnok->$settext("OK")
			%btncancel=$new(button,%hbox)
			%btncancel->$settext("Cancel")
			[comment]# Let's show our nice form[/comment]
			%widget->$show()
		[/example]
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_groupBox, "groupbox", "widget")
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_groupBox, setTitle)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_groupBox, title)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_groupBox, setFlat)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_groupBox, isFlat)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_groupBox, setCheckable)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_groupBox, isCheckable)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_groupBox, setInsideMargin)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_groupBox, insideMargin)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_groupBox, setInsideSpacing)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_groupBox, insideSpacing)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_groupBox, addSpace)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_groupBox, alignment)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_groupBox, setAlignment)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_groupBox, setOrientation)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_groupBox, isChecked)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_groupBox, setChecked)

KVSO_END_REGISTERCLASS(KvsObject_groupBox)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_groupBox, KvsObject_widget)

KVSO_END_CONSTRUCTOR(KvsObject_groupBox)

KVSO_BEGIN_DESTRUCTOR(KvsObject_groupBox)

KVSO_END_CONSTRUCTOR(KvsObject_groupBox)

bool KvsObject_groupBox::init(KviKvsRunTimeContext *, KviKvsVariantList *)
{
	KviTalGroupBox * groupbox = new KviTalGroupBox(getName(), parentScriptWidget());

	groupbox->setOrientation(Qt::Horizontal);
	groupbox->setObjectName(getName());
	setObject(groupbox, true);
	return true;
}

KVSO_CLASS_FUNCTION(groupBox, setTitle)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szTitle;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("title", KVS_PT_STRING, 0, szTitle)
	KVSO_PARAMETERS_END(c)
	((KviTalGroupBox *)widget())->setTitle(szTitle);
	return true;
}
KVSO_CLASS_FUNCTION(groupBox, title)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setString(((KviTalGroupBox *)widget())->title());
	return true;
}
KVSO_CLASS_FUNCTION(groupBox, setFlat)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bFlag", KVS_PT_BOOL, 0, bEnabled)
	KVSO_PARAMETERS_END(c)
	((KviTalGroupBox *)widget())->setFlat(bEnabled);
	return true;
}
KVSO_CLASS_FUNCTION(groupBox, isFlat)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setBoolean(((KviTalGroupBox *)widget())->isFlat());
	return true;
}
KVSO_CLASS_FUNCTION(groupBox, setCheckable)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bFlag", KVS_PT_BOOL, 0, bEnabled)
	KVSO_PARAMETERS_END(c)
	((KviTalGroupBox *)widget())->setCheckable(bEnabled);
	return true;
}
KVSO_CLASS_FUNCTION(groupBox, isCheckable)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setBoolean(((KviTalGroupBox *)widget())->isCheckable());
	return true;
}
KVSO_CLASS_FUNCTION(groupBox, setChecked)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bFlag", KVS_PT_BOOL, 0, bEnabled)
	KVSO_PARAMETERS_END(c)
	((KviTalGroupBox *)widget())->setChecked(bEnabled);
	return true;
}
KVSO_CLASS_FUNCTION(groupBox, isChecked)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setBoolean(((KviTalGroupBox *)widget())->isChecked());
	return true;
}
KVSO_CLASS_FUNCTION(groupBox, setInsideMargin)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uMargin;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("margin", KVS_PT_UNSIGNEDINTEGER, 0, uMargin)
	KVSO_PARAMETERS_END(c)
	((KviTalGroupBox *)widget())->setInsideMargin(uMargin);
	return true;
}
KVSO_CLASS_FUNCTION(groupBox, insideMargin)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((KviTalGroupBox *)widget())->insideMargin());
	return true;
}
KVSO_CLASS_FUNCTION(groupBox, setInsideSpacing)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uSpacing;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("spacing", KVS_PT_UNSIGNEDINTEGER, 0, uSpacing)
	KVSO_PARAMETERS_END(c)
	((KviTalGroupBox *)widget())->setInsideSpacing(uSpacing);
	return true;
}
KVSO_CLASS_FUNCTION(groupBox, insideSpacing)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((KviTalGroupBox *)widget())->insideSpacing());
	return true;
}

KVSO_CLASS_FUNCTION(groupBox, addSpace)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t iSpace;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("columns", KVS_PT_UNSIGNEDINTEGER, 0, iSpace)
	KVSO_PARAMETERS_END(c)
	(((KviTalGroupBox *)widget())->addSpace(iSpace));
	return true;
}

KVSO_CLASS_FUNCTION(groupBox, setAlignment)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szAlign;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("alignment", KVS_PT_STRING, 0, szAlign)
	KVSO_PARAMETERS_END(c)
	for(unsigned int i = 0; i < align_num; i++)
	{
		if(KviQString::equalCI(szAlign, align_tbl[i]))
		{
			((KviTalGroupBox *)widget())->setAlignment(align_cod[i]);
			return true;
		}
	}
	c->warning(__tr2qs_ctx("Unknown alignment '%Q'", "objets"), &szAlign);
	return true;
}

KVSO_CLASS_FUNCTION(groupBox, alignment)
{
	CHECK_INTERNAL_POINTER(widget())
	int mode = ((KviTalGroupBox *)widget())->alignment();
	QString szAlignment = "";
	for(unsigned int i = 0; i < align_num; i++)
	{
		if(mode == align_cod[i])
		{
			szAlignment = align_tbl[i];
			break;
		}
	}
	c->returnValue()->setString(szAlignment);
	return true;
}

KVSO_CLASS_FUNCTION(groupBox, setOrientation)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szMode;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("orientation", KVS_PT_STRING, 0, szMode)
	KVSO_PARAMETERS_END(c)
	if(KviQString::equalCI(szMode, "Horizontal"))
		((KviTalGroupBox *)widget())->setOrientation(Qt::Vertical);
	else if(KviQString::equalCI(szMode, "Vertical"))
		((KviTalGroupBox *)widget())->setOrientation(Qt::Horizontal);
	else
		c->warning(__tr2qs_ctx("Unknown orientation '%Q'", "objects"), &szMode);

	return true;
}
