//=============================================================================
//
//   File : KvsObject_label.cpp
//   Creation date : Mon Sep 18 14:21:48 CEST 2000 by Krzysztof Godlewski
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000 Krzysztof Godlewski
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2014 OmegaPhil (OmegaPhil at startmail dot com)
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

#include "KviError.h"
#include "kvi_debug.h"
#include "KvsObject_label.h"
#include "KviLocale.h"
#include "KviIconManager.h"

#include <QFont>
#include <QPixmap>
#include <QMessageBox>
#include <QLabel>

// Tables used in $setAlignment & $alignment
const char * const align_tbl[] = {
	"Left",
	"Right",
	"HCenter",
	"VCenter",
	"Center",
	"Top",
	"Bottom",
	"Justify"
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

#define align_num (sizeof(align_tbl) / sizeof(align_tbl[0]))

// used in $frameStyle & $setFrameStyle
const char * const frame_tbl[] = {
	"NoFrame",
	"Box",
	"Panel",
	"WinPanel",
	"Hline",
	// shadow styles
	"Plain",
	"Raised",
	"Sunken"
};

const int frame_cod[] = {
	QFrame::NoFrame,
	QFrame::Box,
	QFrame::Panel,
	QFrame::WinPanel,
	QFrame::HLine,
	QFrame::Plain,
	QFrame::Raised,
	QFrame::Sunken
};

#define frame_num (sizeof(frame_tbl) / sizeof(frame_tbl[0]))

/*
	@doc:	label
	@keyterms:
		label object class, show image
	@title:
		label class
	@type:
		class
	@short:
		Displays text or an image
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		This widget can be used to display a text or an image.[br]
		It can have different frame styles and text/image alignment.
	@functions:
		!fn: $setText(<text:string>)
		Sets the text to be displayed by the label.
		The text can contain limited HTML tags.
		See also [classfnc]$text[/classfnc]().
		!fn: <string> $text()
		Returns the text currently displayed by the label.
		See also [classfnc]$setText[/classfnc]().
		!fn: <integer> $margin()
		Returns current value of margin for this label (default is 0).
		See also [classfnc]$setMargin[/classfnc]().
		!fn: $setMargin(<margin:uint>)
		Sets margin width / height to <margin>.
		See also [classfnc]$margin[/classfnc]().
		!fn: <string> $alignment()
		Returns a string containing alignment flags that are set for
		this label. The flags are separated by commas.[br]
		An example output could look like this:[br]
		[pre]Bottom, Right[/pre]
		See [classfnc]$setAlignment[/classfnc]() for explanation of all
		alignment flags.
		!fn: $setAlignment(<flag1:string>, <flag2:string>, ...)
		This function sets alignment flags, given as parameters, for
		this label. Valid flags are:
		[pre]
			Right     - Text is aligned to right border
			Left      - Text is aligned to left border
			Top       - Text is aligned to the top border
			Bottom    - Text is aligned to the bottom border
			HCenter   - Text is horizontally centered
			VCenter   - Text is vertically centered
			Center    - Equals HCenter + VCenter
			Justify   - Text is spaced apart to cover available room
		[/pre]
		It is obvious that you can not set [i]Right[/i]
		and [i]Left[/i] simultaneously - this will [b]not[/b]
		result in an error message - it will simply not work :)
		!fn: $clear()
		Clears the label. Equal to calling [classfnc]$setText[/classfnc]("")
		!fn: <string> $frameStyle()
		Returns a string containing this label's frame-style flags,
		separated with commas. Output from this function could look like
		this:
		[pre]Panel, Raised[/pre]
		See [classfnc]$setFrameStyle[/classfnc]() for a list of all
		frame-style flags and their explanation.
		!fn: $setFrameStyle(<flag1>, <flag2>, ...)
		Sets the frame-style flags to the ones passed as arguments.
		The flags determine the shape or shadow of the
		label's frame. Valid shape flags are:[br]
		[pre]
			NoFrame   - Draw no frame. You shouldn't specify a shadow when using this.
			Box       - Draws a rectangular box. Its borders can be [i]Raised[/i] or [i]Sunken[/i]
			Panel     - Draws a rectangular panel which can be [i]Raised[/i] or [i]Sunken[/i]
			WinPanel  - Similar to [i]Panel[/i], but is more in Win95 style
			Hline     - Draws a horizontal line that frames nothing (useful as separator)
		[/pre]
		Valid shadow flags are:[br]
		[pre]
			Plain     - No 3D effect (draws using foreground color)
			Raised    - Makes the label look like it was raised above the parent widget
			Sunken    - Makes the label look like it was [i]pushed[/i] inside the parent widget
		[/pre]
		!fn: $setImage(<image_id>)
		Sets the image to be displayed on this label.
		Giving empty argument clears the pixmap.[br]
		See the [doc:image_id]image identifier[/doc] documentation for
		the explanation of the <image_id> parameter.

*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_label, "label", "widget")
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_label, setText)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_label, text)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_label, margin)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_label, setMargin)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_label, alignment)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_label, setAlignment)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_label, clear)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_label, frameStyle)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_label, setFrameStyle)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_label, setImage)
KVSO_END_REGISTERCLASS(KvsObject_label)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_label, KvsObject_widget)

KVSO_END_CONSTRUCTOR(KvsObject_label)

KVSO_BEGIN_DESTRUCTOR(KvsObject_label)

KVSO_END_CONSTRUCTOR(KvsObject_label)

bool KvsObject_label::init(KviKvsRunTimeContext *, KviKvsVariantList *)
{
	SET_OBJECT(QLabel)
	((QLabel *)widget())->setWordWrap(true);
	((QLabel *)widget())->setTextInteractionFlags(Qt::TextSelectableByMouse);
	return true;
}

KVSO_CLASS_FUNCTION(label, setText)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("text", KVS_PT_STRING, 0, szText)
	KVSO_PARAMETERS_END(c)
	((QLabel *)widget())->setText(szText);
	return true;
}

KVSO_CLASS_FUNCTION(label, text)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setString(((QLabel *)widget())->text());
	return true;
}

KVSO_CLASS_FUNCTION(label, setMargin)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t iMargin;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("margin", KVS_PT_UNSIGNEDINTEGER, 0, iMargin)
	KVSO_PARAMETERS_END(c)
	((QLabel *)widget())->setMargin(iMargin);
	return true;
}
KVSO_CLASS_FUNCTION(label, margin)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(((QLabel *)widget())->margin());
	return true;
}

KVSO_CLASS_FUNCTION(label, setAlignment)
{
	CHECK_INTERNAL_POINTER(widget())
	QStringList alignment;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("alignment", KVS_PT_STRINGLIST, KVS_PF_OPTIONAL, alignment)
	KVSO_PARAMETERS_END(c)
	int align, sum = 0;
	for(auto & it : alignment)
	{

		align = 0;
		for(unsigned int j = 0; j < align_num; j++)
		{
			if(KviQString::equalCI(it, align_tbl[j]))
			{
				align = align_cod[j];
				break;
			}
		}
		if(align)
			sum = sum | align;
		else
			c->warning(__tr2qs_ctx("Unknown alignment '%Q'", "objects"), &it);
	}
	((QLabel *)widget())->setAlignment((Qt::Alignment)sum);
	return true;
}

KVSO_CLASS_FUNCTION(label, alignment)
{
	CHECK_INTERNAL_POINTER(widget())
	int mode = ((QLabel *)widget())->alignment();
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

KVSO_CLASS_FUNCTION(label, clear)
{
	CHECK_INTERNAL_POINTER(widget())
	((QLabel *)widget())->clear();
	return true;
}

KVSO_CLASS_FUNCTION(label, setFrameStyle)
{
	CHECK_INTERNAL_POINTER(widget())
	QStringList style;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("style", KVS_PT_STRINGLIST, KVS_PF_OPTIONAL, style)
	KVSO_PARAMETERS_END(c)
	if(!widget())
		return true;
	int framestyle, sum = 0;
	for(auto & it : style)
	{
		framestyle = 0;
		for(unsigned int j = 0; j < align_num; j++)
		{
			if(KviQString::equalCI(it, frame_tbl[j]))
			{
				framestyle = frame_cod[j];
				break;
			}
		}
		if(framestyle)
			sum = sum | framestyle;
		else
			c->warning(__tr2qs_ctx("Unknown style '%Q'", "objects"), &it);
	}
	((QLabel *)widget())->setFrameStyle(sum);
	return true;
}

KVSO_CLASS_FUNCTION(label, frameStyle)
{
	CHECK_INTERNAL_POINTER(widget())
	int mode = ((QLabel *)widget())->frameStyle();
	QString szStyle = "";
	for(unsigned int i = 0; i < frame_num; i++)
	{
		if(mode == frame_cod[i])
		{
			szStyle = frame_tbl[i];
			break;
		}
	}
	c->returnValue()->setString(szStyle);
	return true;
}

KVSO_CLASS_FUNCTION(label, setImage)
{
	CHECK_INTERNAL_POINTER(widget())
	QString icon;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("icon", KVS_PT_STRING, 0, icon)
	KVSO_PARAMETERS_END(c)
	QPixmap * pix = g_pIconManager->getImage(icon);
	if(pix)
		((QLabel *)widget())->setPixmap(*pix);
	return true;
}
