//vim: ts=8
//   File : class_label.cpp
//   Creation date : Mon Sep 18 14:21:48 CEST 2000 by Krzysztof Godlewski
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Krzysztof Godlewski
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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

#include <qfont.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include "class_label.h"
//#include "kvi_fileutils.h"
#include "kvi_error.h"
#include "kvi_debug.h"

#include "kvi_locale.h"
#include "kvi_iconmanager.h"



// Tables used in $setAlignment & $alignment
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
		QLabel::AlignLeft,
		QLabel::AlignRight,
		QLabel::AlignHCenter,
		QLabel::AlignVCenter,
		QLabel::AlignCenter,
		QLabel::AlignTop,
		QLabel::AlignBottom,
		QLabel::WordBreak
			};

#define align_num	(sizeof(align_tbl) / sizeof(align_tbl[0]))
		  
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

#define frame_num	(sizeof(frame_tbl) / sizeof(frame_tbl[0]))


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
		This widget can be used to display a text or an image. It can
		have different frame styles and text/image alignment.
	@functions:
		!fn: $setText(<text:string>)
		Sets the text to be displayed by the label.
		The text can contain limited html tags.
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
		!fn: <boolean> $autoResize()
		Returns 1, if auto-resize ability of the label is enabled. 
		See also [classfnc]$setAutoResize[/classfnc]().
		!fn: $setAutoResize(<benabled:boolean>)
		Sets auto-resize ability to enabled (if <benabled> is 1) or disabled
		 (if <benabled> is 0). If auto-resize is enabled, the label will 
		automagically resize itself accordingly to the 'size' of the
		text it contains. 
		See also [classfnc]$autoResize[/classfnc]().
		!fn: <string> $alignment()
		Returns a string containing alignment flags that are set for 
		this label. The flags are separated by commas. An example output
		could look like this:[br]
		[pre]Bottom, Right[/pre][br]
		See [classfnc]$setAlignment[/classfnc]() for explanation of all
		alignment flags.
		!fn: $setAlignment(<flag1:string>, <flag2:string>, ...)
		This function sets alignment flags, given as parameters, for 
		this label. Valid flags are:
		[pre]
		Right     - Text is aligned to right border[br]
		Left      - Text is aligned to left border[br]
		Top       - Text is aligned to the top border[br]
		Bottom    - Text is aligned to the bottom border[br]
		HCenter   - Text is horizontally centered[br]
		VCenter   - Text is vertically centered[br]
		Center    - Equals HCenter + VCenter[br]
		WordBreak - Enables automatic word breaking[br]
		[/pre]
		It is obvious that you can not set for example [i]Right[/i] 
		and [i]Left[/i] simoultaneously - this will [b]NOT[/b] 
		result in an error message - this will simply not work :)
		!fn: $clear()
		Clears the label. Equal to calling 
		[classfnc]$setText[/classfnc]("")
		!fn: $frameStyle()
		Returns a string containing this label's frame-style flags,
		separated with commas. Output from this function could look like
		this:
		[pre]Panel, Raised[/pre]
		See [classfnc]$setFrameStyle[/classfnc]() for a list of all
		frame-style flags and their explenation.
		!fn: $setFrameStyle(<flag1>, <flag2>, ...)
		Sets the frame-style flags to the ones passed as arguments.
		The flags can either decide of the shape or shadow of the
		label's frame. Valid shape flags are:[br]
		[pre]
		NoFrame     - draw no frame. You shouldn't specify a shadow when
		using this.[br]
		Box         - draws a rectangular box. Its borders can be 
		[i]Raised[/i] or [i]Sunken[/i][br]
		Panel       - draws a rectangular panel which can be 
		[i]Raised[/i] or [i]Sunken[/i][br]
		WinPanel    - similar to [i]Panel[/i], but is more in Win95
		style[br]
		Hline        - draws a horizontal line that frames nothing (useful as separator) 
		[/pre]
		Valid shadow flags are:[br]
		[pre]
		Plain       - no 3D effect (draws using foreground color)[br]
		Raised      - makes the label look like it was raised above the
		parent widget[br]
		Sunken      - makes the label look like it was "pushed" inside
		the parent widget[br]
		[/pre]
		!fn: $setImage(<image_id>)
		Sets the image to be displayed on this label. 
		Giving empty argument clears the pixmap[br]
		See the [doc:image_id]image identifier[/doc] documentation for
		the explaination of the <image_id> parameter.

*/


KVSO_BEGIN_REGISTERCLASS(KviKvsObject_label,"label","widget")
	KVSO_REGISTER_HANDLER(KviKvsObject_label,"setText",functionSetText)
	KVSO_REGISTER_HANDLER(KviKvsObject_label,"text", functionText)
	KVSO_REGISTER_HANDLER(KviKvsObject_label,"margin", functionMargin)
	KVSO_REGISTER_HANDLER(KviKvsObject_label,"setMargin", functionSetMargin)
	KVSO_REGISTER_HANDLER(KviKvsObject_label,"autoResize", functionAutoResize)
	KVSO_REGISTER_HANDLER(KviKvsObject_label,"setAutoResize", functionSetAutoResize)
	KVSO_REGISTER_HANDLER(KviKvsObject_label,"alignment", functionAlignment)
	KVSO_REGISTER_HANDLER(KviKvsObject_label,"setAlignment", functionSetAlignment)
	KVSO_REGISTER_HANDLER(KviKvsObject_label,"clear", functionClear)
	KVSO_REGISTER_HANDLER(KviKvsObject_label,"frameStyle", functionFrameStyle)
	KVSO_REGISTER_HANDLER(KviKvsObject_label,"setFrameStyle", functionSetFrameStyle)
	KVSO_REGISTER_HANDLER(KviKvsObject_label,"setImage", functionSetImage)
KVSO_END_REGISTERCLASS(KviKvsObject_label)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_label,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_label)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_label)

KVSO_END_CONSTRUCTOR(KviKvsObject_label)

bool KviKvsObject_label::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{
	setObject(new QLabel(parentScriptWidget(), name()), true);
	return true;
}

bool KviKvsObject_label::functionSetText(KviKvsObjectFunctionCall *c)
{
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QLabel *)widget())->setText(szText);
	return true;
}
bool KviKvsObject_label::functionText(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setString(((QLabel *)widget())->text());
	return true;
}

bool KviKvsObject_label::functionSetMargin(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t iMargin;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("margin",KVS_PT_UNSIGNEDINTEGER,0,iMargin)
	KVSO_PARAMETERS_END(c)
    if (widget()) ((QLabel *)widget())->setMargin(iMargin);
	return true;
}
bool KviKvsObject_label::functionMargin(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setInteger(((QLabel *)widget())->margin());
	return true;
}

bool KviKvsObject_label::functionSetAutoResize(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(widget())
		((QLabel *)widget())->setAutoResize(bEnabled);
	return true;
}
bool KviKvsObject_label::functionAutoResize(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setBoolean(((QLabel *)widget())->autoResize());
	return true;
}

bool KviKvsObject_label::functionSetAlignment(KviKvsObjectFunctionCall *c)
{
	QStringList alignment;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("alignment",KVS_PT_STRINGLIST,KVS_PF_OPTIONAL,alignment)
	KVSO_PARAMETERS_END(c)
	if (!widget()) return true;
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
	((QLabel *)widget())->setAlignment(sum);
	return true;
}
bool KviKvsObject_label::functionAlignment(KviKvsObjectFunctionCall *c)
{
	int mode = ((QLabel *)widget())->alignment();
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
bool KviKvsObject_label::functionClear(KviKvsObjectFunctionCall *c)
{
	if(widget())
		((QLabel *)widget())->clear();
	return true;
}


bool KviKvsObject_label::functionSetFrameStyle(KviKvsObjectFunctionCall *c)
{

	QStringList style;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("style",KVS_PT_STRINGLIST,KVS_PF_OPTIONAL,style)
	KVSO_PARAMETERS_END(c)
	if (!widget()) return true;
	int framestyle,sum=0;
	for ( QStringList::Iterator it = style.begin(); it != style.end(); ++it )
		{
			framestyle = 0;
			for(unsigned int j = 0; j < align_num; j++)
			{
				if(KviQString::equalCI((*it), frame_tbl[j]))
				{
					framestyle=frame_cod[j];
					break;
				}
			}
			if(framestyle)
				sum = sum | framestyle;
			else
				c->warning(__tr2qs("Unknown style: '%Q'"),&(*it));
			
		}
	((QLabel *)widget())->setFrameStyle(sum);
	return true;




}
bool KviKvsObject_label::functionFrameStyle(KviKvsObjectFunctionCall *c)
{
	int mode = ((QLabel *)widget())->frameStyle();
	QString szStyle="";
	for(unsigned int i = 0; i < frame_num; i++)
	{
		if(mode == frame_cod[i])
		{
			szStyle=frame_tbl[i];
			break;
		}
	}
	c->returnValue()->setString(szStyle);
	return true;
}
bool KviKvsObject_label::functionSetImage(KviKvsObjectFunctionCall *c)
{
	
	QString icon;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("icon",KVS_PT_STRING,0,icon)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	QPixmap * pix = g_pIconManager->getImage(icon);
	if(pix) ((QLabel *)widget())->setPixmap(*pix);
	return true;
}
