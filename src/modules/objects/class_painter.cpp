//mdm:
//   Painter : class_painter.cpp
//   Creation date : Fri Mar 18 14:30:48 CEST 2005 
//	 by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//   Lucia Papini (^ashura^)  English Translation.
//   This file is part of the KVirc irc client distribution
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

#include "class_painter.h"
#include "class_pixmap.h"
#include "class_widget.h"
#include "kvi_debug.h"

#include "kvi_locale.h"
#include "kvi_error.h"
#include "kvi_iconmanager.h"
#include "kvi_malloc.h"


/*
	@doc:	painter
	@keyterms:
	    painter object class, line editor, input
	@title:
		painter class
	@type:
		class
	@short:
		This class provide a painter to paint line and shapes.
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		With this class you can draw many graphics objects from simple lines to complex shapes like pies and chords.[br]
		It can also draw aligned text and pixmaps. Normally, it draws in a "natural" coordinate system, but it can also do view and world transformation.[br]
		The class need to be implemented into a [classfnc]$paintEvent[/classfnc]();
		@functions:
		!fn: $drawLine(<x1:integer>,<y1_integer>,<x2:integer>,<y2:integer>)
		Draws a line from (x1, y1) to (x2, y2) and sets the current pen position to (x2, y2).
		!fn: $begin(<paint_device:object>)
		Begins painting the paint device <paint_device>: the parameter MUST be a widget or a pixmap.[br]
		Warning: A paint device can only be painted by one painter at a time.
		!fn: $end()
		Ends painting. Any resources used while painting are released.
		!fn: $setPen(<rgb or hsv array value or [<red>,<green>,<blue>][<hue>,<saturation>,<value>],[system color:RGB or HSV)
		The pen defines the lines or text, color. You can set it with Red,Green,Blue, or H,S,V value[br]
		All parameters are in integer form.
		The HSV system, like RGB, has three components:[br]
		* H, for hue, is either 0-359 if the color is chromatic (not gray), or meaningless if it is gray.[br]
		  It represents degrees on the color wheel familiar to most people. Red is 0 (degrees), green is 120 and blue is 240.[br]
		* S, for saturation, is 0-255, and the bigger it is, the stronger the color is. Grayish colors have saturation near 0; very strong colors have saturation near 255.[br]
		* V, for value, is 0-255 and represents lightness or brightness of the color. 0 is black; 255 is as far from black as possible.[br]
		Examples: RED is H=0, S=255, V=255.[br]
		Light RED could have H about 0, S about 50-100, and S=255.
		ES: $setPen(00,00,00) for black;[br]
		Default color mode is RGB;
		!fn: $setBrush(<rgb or hsv array value or [<red>,<green>,<blue>][<hue>,<saturation>,<value>],[system color:RGB or HSV)
		Sets the painter's brush to have the specified color.[br]
		Example:[br]
		class (wdg,widget)[br]
		{[br]
			paintevent()[br]
			{[br]
				%b=$new(painter)[br]
				%b->$setBackgroundMode(Opaque)[br]
				%b->$setBrush(0,250,250)[br]
				%b->$begin($$)[br]
				%b->$drawEllipse(50,50,100,50)[br]
			}[br]
		}[br]
		%aa=$new(wdg)[br]
		%aa->$show()[br]
		!fn: $drawRect(<x:integer>,<y:integer>,<w:unsigned integer>,<h:unsigned integer>)
		Draws a rectangle with upper left corner at (x, y) and with width w and height h.
		!fn: $drawWinFocusRect(<x:integer>,<y:integer>,<w:unsigned integer>,<h:unsigned integer>)
		Draws a Windows focus rectangle with upper left corner at (x, y) and with width w and height h.[br]
		This function draws nothing if the coordinate system has been rotated or sheared.
		!fn: $drawRoundRect(<x:integer>,<y:integer>,<w:unsigned integer>,<h:unsigned integer>,<xCor:integer>,<yCor:integer>)
		Draws a rectangle with rounded corners at (x, y), with width w and height h.[rb]
		The xCor and yCor arguments specify how rounded the corners should be (range is 0->99).
		!fn: $drawPie(<x:integer>,<y:integer>,<w:unsigned integer>,<h:unsigned integer>,<angle:integer>,<alen:integer>)
		Draws a pie defined by the rectangle (x, y, w, h), the start angle a and the arc length alen.[br]
		The angles <angle> and <alen> are 1/16th of a degree, i.e. a full circle equals 5760 (16*360).
		!fn: $drawArc(<x:integer>,<y:integer>,<w:unsigned integer>,<h:unsigned integer>,<angle:integer>,<alen:integer>)
		Draws an arc defined by the rectangle (x, y, w, h), the start angle a and the arc length alen.[br]
		The angles <angle> and <alen> are 1/16th of a degree, i.e. a full circle equals 5760 (16*360).
		!fn: $drawPoint(<x:integer>,<y:integer>)
		Draws a point at x and y coordinates.
		!fn: $drawEllipse(<x:integer>,<y:integer>,<sizew:unsigned integer>,<sizeh:unsigned integer>)
		Draws an ellipse with center at (x + w/2, y + h/2) and size (w, h).
		!fn: $drawChord(<x:integer>,<y:integer>,<w:unsigned integer>,<h:unsigned integer>,<angle:integer>,<alen:integer>)
		Draws a chord defined by the rectangle (x, y, w, h), the start angle a and the arc length alen.[br]
		The angles <angle> and <alen> are 1/16th of a degree, i.e. a full circle equals 5760 (16*360).
		!fn: $drawText(<x:integer>,<y:integer>,<text:string>,<nr chars:integer>,<dir:enum>)
		Draws the given <text> at position <x>, <y>.[br]
		If <len> is -1 (the default) all the text is drawn, otherwise the first <len> characters are drawn.
		The text's direction is given by <dir>, valid flag are:[br]
		[pre]
		Auto [br]
		RTL (right to left) [br]
		LTR (left to right) [br]
		[/pre]
		!fn: $drawPixmap(<x:integer>,<y:integer>,<pixmap:hobject>,<sx:integer>,<sy:integer>,<ex:integer>,<ey:integer>)
		Draws a pixmap at x,y coordinates[br]
		!fn: $setFont(<size:unsigned integer>,<family:string>,<style:enum>)[br]
		Set the font's size, family and stile, valid flag for style are:[br]
		[pre]
		italic     [br]
		bold     [br]
		underline      [br]
		overline    [br]
		strikeout  [br]
		fixedpitch  [br]
		[/pre]
		!fn: $setFontSize(<size:unsigned integer>)[br]
		Set the current painter font's size.[br]
		!fn: <integer>$fontAscent()
		Return the distance from the baseline to the highest position characters extend to.
		!fn: <integer>$fontDescent()
		Return the distance from the baseline to the lowest point characters extend to.
		!fn: <integer>$fontMetricsWidth(<text:string>)
		Returns the font metrics width for the painter's current font.
		!fn: <integer>$fontMetricsHeight()
		Returns the font metrics height for the painter's current font.
		!fn: $rotate(<angle:real>)
		Rotates the coordinate system a degrees counterclockwise.
		!fn: $translate(<dx:real>,<dy:real>)
		Translates the coordinate system by <dx>, <dy>.
		!fn: $shear(<dh:real>,<dv:real>)
		Shears the coordinate system by <dh>, <dv>.
		!fn: $scale(<dh:real>,<dw:real>)
		Scales the coordinate system by <dh>, <dv>.
		!fn: $setBackgroundMode(<bgMode:enum>)
		Sets the background mode of the painter to <bgMode>: valid values are:[br]
		- Transparent	(that is the default value);[br]
		- Opaque.[br]
		!fn: $setOpacity(<opacity_factor:real>) [QT4 ONLY] 
		Sets the painter opacity that affects all painter operations (drawpixmap, drawtext...). Valid values range are from 0 (total transparency) to 1 (total opacity)[br]
		You must invoke the [classfnc]$begin[/classfnc] before using it.
		!fn: $setTextAntialiasing(<boolean>) [QT4 ONLY] 
		Enable/disable antialias in text if possible.
		You must call the [classfnc]$begin[/classfnc] before using it.
		!fn: $setAntialiasing(<boolean>) [QT4 ONLY] 
		Enable/disable antialias in edges of primitives if possible.
		You must call the [classfnc]$begin[/classfnc] before using it.
		!fn: $setSmoothPixmapTransform(<boolean>) [QT4 ONLY] 
		Enable/disable smooth bilinear pixmap transformation algorithm (such as bilinear).
		You must call the [classfnc]$begin[/classfnc] before using it.
		Example:[br]
		[br]
		class (hello,widget)[br] 
		{[br] 
			constructor()[br]
			{[br] 
				$$->%sintbl[]= $array( 0, 38, 71, 92, 100, 92, 71, 38,0, -38, -71, -92, -100, -92, -71, -38);[br] 
				$$->%index=0[br] 
				$$->$starttimer( 30 );[br] 
				$$->$resize(800,600 );[br] 
				$$->%string=$0[br] 
				$$->%waitfor=1;[br] 
				$$->%nextanim=0[br] 
				[br]
				#zoom and rotation anim[br] 
				$$->%Zoomindex=11[br] 
				$$->%degree=0[br] 
				$$->%Noanim=0[br] 
				$$->%scrollright=-450[br] 
				[br]
				#anim effect init[br] 
				$$->%xoffset=4[br] 
				$$->%yoffset=3[br] 
				$$->%xstart=270[br] 
				$$->%ystart=200[br] 
				$$->%b=0[br] 
				$$->%yoffs=400[br] 
				[br]
				#parallax parameter[br] 
				$$->%Off=400[br] 
				$$->%roll=1[br] 
			}[br]
			timerevent()[br] 
			{[br]
				$$->%b = $(($$->%b + 1) & 15);[br] 
				if ($$->%nextanim == 1)  $$->$repaint(1);[br] 
				$$->$repaint(0);[br] 
			}[br] 
			drawAnim()[br] 
			{[br] 
				%P->$setFont(32,"times",bold);[br] 
				%w=$(%P->$fontMetricsWidth($$->%string[$$->%index]) + 20);[br] 
				%h=$(%P->$fontMetricsHeight * 2);[br] 
				%pmx = $(($$->$width/2) -%w/2);[br] 
				%pmy = $(($$->$height()/2) - %h/2);[br] 
				%x = 10;[br] 
				%y= $((%h/2) + $$->$fontDescent());[br] 
				%i=0[br] 
				while ( $str.mid("Grifisx/Noldor",%i,1) != "") [br] 
				{[br] 
					%i16 = $(($$->%b+%i) & 15);[br] 
					%char=$str.mid("Grifisx/Noldor",%i,1)[br]
					%P->$setPen($((15-%i16)*16),$((15-%i16)*16),$((15-%i16)*16) );[br]
					%P->$drawText( $(%x+$$->%xstart),$($$->%ystart+%y-$$->%sintbl[%i16]*%h/800),%char,1,Auto);[br]
					%x += %P->$fontMetricsWidth(%char);[br] 
					%i++;[br] 
				}[br] 
			}[br] 
			matrixeffect()[br] 
			{[br] 
				if (($$->%Zoomindex == 99) && ($$->%degree==360)) return %P->$drawPixmap($(400-32),$(300-32),"kvirc.png",0,0,-1,-1)[br] 
				%P->$scale(0.$$->%Zoomindex,0.$$->%Zoomindex)[br] 
				if ($$->%Zoomindex != 99) $$->%Zoomindex++;[br] 
				%P->$rotate($$->%degree)[br] 
				%P->$translate(400,300)[br] 
				%P->$drawPixmap(-32,-32,"kvirc.png",0,0,-1,-1)[br] 
				%P->$setFont(28,"times",bold);[br] 
				%P->$reset()[br] 
				if ($$->%scrollright >= 550) return[br] 
				%P->$scale(0.$$->%Zoomindex,0.$$->%Zoomindex)[br] 
				%P->$translate(400,350)[br] 
				%P->$drawText($$->%scrollright,10,"Another cool class brought to you by...",-1,Auto)        [br] 
				$$->%scrollright += 3;[br] 
				%P->$reset()[br] 
			}[br] 
			nextanim()[br] 
			{[br] 
				%p=$new(painter)[br] 
				%p->$setBackgroundMode(Opaque)[br] 
				%p->$setBrush($rand(255),$rand(255),$rand(255))[br] 
				%p->$begin($$)[br] 
				%rand=$rand(5)[br] 
				%p->$drawrect($rand(800),$rand(400),120,200)[br] 
				%p->$drawArc($rand(800),$rand(400),120,200,20,$(16*20))[br] 
				%p->$drawPie($rand(800),$rand(400),120,200,20,$(16*20))[br] 
				%p->$drawChord($rand(800),$rand(400),120,200,20,$(16*20))[br] 
				%p->$drawEllipse($rand(800),$rand(400),100,30)[br] 
				%p->$end()[br] 
				delete %p[br] 
			}[br] 
			paintEvent()[br] 
			{[br] 
				if ($$->%nextanim ==1) return $$->$nextanim()[br] 
				# pixmap creation: every effect will be painted on it then copied on widget[br] 
				%pixmap=$new(pixmap)[br] 
				%pixmap->$resize($$->$width(),$$->$height())[br]
				[br]
				# painter creation [br] 
				%P=$new(painter);[br] 
				%P->$begin(%pixmap);[br] 
				$$->$drawanim[br] 
				$$->$matrixeffect[br] 
				%i=0[br] 
				while (%i != 100)[br] 
				{[br] 
					%i16 = $(($$->%b+%i) & 15);[br] 
					%P->$setPen($((15-%i16)*16),$((15-%i16)*16),$((15-%i16)*16) );[br] 
					%P->$drawpoint($rand(800),$rand(600))[br] 
					%i++[br] 
				}[br]
				[br]
				# sets the animations order to manage the parallax effect[br] 
				%P->$end[br] 
				objects.bitBlt $$ 0 0 %pixmap[br] 
				delete %pixmap[br] 
				delete %P[br]
				if (%Pauseflag == 1) return[br]
				[br]
				# manage the animations parameters[br] 
				if (($$->%Off<=60) && ($$->%roll<182)) $$->%roll += 2;[br] 
				if ($$->%roll>182) $$->%waitfor=0[br] 
				if ($$->%Noanim != 1) $$->%degree += 16;[br] 
				if ($$->%degree >= 360)[br] 
				{[br] 
					$$->%degree=0;[br] 
					if ($$->%Zoomindex == 99) $$->%Noanim=1[br] 
				} [br] 
				if ($$->%Noanim != 1) return[br]
				[br]
				#sinusoid animation[br] 
				if (($$->%xstart <1) && ($$->%xoffset == -4)) $$->%xoffset=4;[br] 
				if (($$->%xstart >$($$->$width()-%P->$fontMetricsWidth("Grifisx/Noldor"))) && ($$->%xoffset == 4)) $$->%xoffset=-4;[br] 
				if (($$->%ystart <1) && ($$->%yoffset == -3)) $$->%yoffset=3;[br] 
				if (($$->%ystart >$($$->$height()-60)) && ($$->%yoffset == 3))   $$->%yoffset=-3;[br] 
				$$->%xstart += $$->%xoffset;[br] 
				$$->%ystart += $$->%yoffset;[br] 
			}[br] 
		}[br] 
		[br]
		%Hello=$new(hello)[br] 
		%Hello->$setcaption("Painter effects" );[br] 
		%Hello->$setFont(28,"times",bold);[br] 
		%Btn=$new(button,%Hello)[br] 
		%Btn->$setmaximumwidth(80)[br] 
		%Btn->$setmaximumheight(30)[br] 
		%Btn->$setFont(8,"times",bold);[br] 
		%Btn->$settext(Next)[br] 
		[br]
		privateimpl(%Btn,mousepressevent)[br] 
		{[br] 
			if ($$->$parent->%nextanim==0) [br] 
			{[br] 
				$$->$parent->%nextanim=1[br] 
				$$->$settext(Prev)[br] 
			} [br] 
			else[br] 
			{[br] 
				$$->$parent->%nextanim=0[br] 
				$$->$settext(Next)[br] 
			}[br]
		}[br] 
		[br]
		%lay=$new(layout,%Hello)[br] 
		%lay->$addwidget(%Btn,4,0)[br] 
		%Hello->$setBackgroundColor("000000");[br] 
		%Hello->$setmaximumwidth(800)[br] 
		%Hello->$setminimumwidth(780)[br] 
		%Hello->$setmaximumheight(600)[br] 
		%Hello->$setminimumheight(600)[br] 
		%Hello->$move(10,10)[br] 
		%Hello->$show();[br] [br] 
	
*/

// ========================================================================
KVSO_BEGIN_REGISTERCLASS(KviKvsObject_painter,"painter","object")

	// Fonts
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"setFont",functionSetFont)
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"setFontSize",functionSetFontSize)

	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"setBrush",functionsetBrush)
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"setPen",functionsetPen)
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"fontAscent",functionfontAscent)
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"fontDescent",functionfontDescent)
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"fontMetricsHeight",functionfontMetricsHeight)
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"fontMetricsWidth",functionfontMetricsWidth)
	// Draws
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"setBackGroundMode",functionsetBackGroundMode)
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"drawRect",functiondrawRect)
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"drawLine",functiondrawLine)
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"drawRoundRect",functiondrawRoundRect)
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"drawWinFocusRect",functiondrawWinFocusRect)
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"drawPoint",functiondrawPoint)
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"drawArc",functiondrawArc)
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"drawEllipse",functiondrawEllipse)
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"drawPie",functiondrawPie)
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"drawChord",functiondrawChord)
	// Text & Pixmap
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"drawText",functiondrawText)
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"drawPixmap",functiondrawPixmap)


	// MAtrix Operation
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"rotate",functionrotateMatrix)
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"shear",functionshearMatrix)
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"scale",functionscaleMatrix)
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"translate",functiontranslateMatrix)
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"reset",functionresetMatrix)

#ifdef COMPILE_USE_QT4
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"setOpacity",functionsetOpacity)
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"setTextAntialiasing",functionsetTextAntialiasing)
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"setAntialiasing",functionsetAntialiasing)
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"setSmoothPixmapTransform",functionsetSmoothPixmapTransform)
#endif

	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"begin",functionbegin)
	KVSO_REGISTER_HANDLER(KviKvsObject_painter,"end",functionend)

	
KVSO_END_REGISTERCLASS(KviKvsObject_painter)


KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_painter,KviKvsObject)

	m_pPainter = new QPainter();

	m_pDeviceObject=0;
	
KVSO_END_CONSTRUCTOR(KviKvsObject_painter)

KVSO_BEGIN_DESTRUCTOR(KviKvsObject_painter)

	if (m_pPainter) delete m_pPainter;
	m_pPainter = 0;

KVSO_END_CONSTRUCTOR(KviKvsObject_painter)

#define KVSO_PARAMETERS_PAINTER(__pXOrArray,__iY,__iW,__iH)\
if(__pXOrArray->isArray())\
	{\
		if(__pXOrArray->array()->size() < 4)\
		{\
			c->error(__tr2qs("The array passed as parameter must contain at least 4 elements"));\
			return false;\
		}\
		KviKvsVariant * pX = __pXOrArray->array()->at(0);\
		KviKvsVariant * pY = __pXOrArray->array()->at(1);\
		KviKvsVariant * pW = __pXOrArray->array()->at(2);\
		KviKvsVariant * pH = __pXOrArray->array()->at(3);\
		if(!(pX && pY && pW && pH))\
		{\
			c->error(__tr2qs("One of the geometry array parameters is empty"));\
			return false;\
		}\
		if(!(pX->asInteger(iX) && pY->asInteger(__iY) && pW->asInteger(__iW) && pH->asInteger(__iH)))\
		{\
			c->error(__tr2qs("One of the geometry array parameters didn't evaluate to an integer"));\
			return false;\
		}\
	} else {\
		if(c->params()->count() < 4)\
		{\
			QString error=function+" requires either an array as first parameter or four integers";\
			c->error(__tr2qs(error));\
			return false;\
		}\
		if(!__pXOrArray->asInteger(iX))\
		{\
			c->error(__tr2qs("The first parameter didn't evaluate to an array nor an integer"));\
			return false;\
		}\
	}\

#define KVSO_2ARRAYPARAMETERS(__pXOrArray,__iY)\
if(__pXOrArray->isArray())\
	{\
		if(__pXOrArray->array()->size() < 2)\
		{\
			c->error(__tr2qs("The array passed as parameter must contain at least 2 elements"));\
			return false;\
		}\
		KviKvsVariant * pX = __pXOrArray->array()->at(0);\
		KviKvsVariant * pY = __pXOrArray->array()->at(1);\
		if(!(pX && pY ))\
		{\
			c->error(__tr2qs("One of the array parameters is empty"));\
			return false;\
		}\
		if(!(pX->asInteger(iX) && pY->asInteger(__iY)))\
		{\
			c->error(__tr2qs("One of the array parameters didn't evaluate to an integer"));\
			return false;\
		}\
	} else {\
		if(c->params()->count() < 2)\
		{\
			QString error=function+" requires either an array as first parameter or two integers";\
			c->error(__tr2qs(error));\
			return false;\
		}\
		if(!__pXOrArray->asInteger(iX))\
		{\
			c->error(__tr2qs("The first parameter didn't evaluate to an array nor an integer"));\
			return false;\
		}\
	}\

#define KVSO_3ARRAYPARAMETERS(__pCol1OrArray,__iCol2,__iCol3)\
if(__pCol1OrArray->isArray())\
	{\
		if(__pCol1OrArray->array()->size() < 3)\
		{\
			c->error(__tr2qs("The array passed as parameter must contain at least 3 elements"));\
			return false;\
		}\
		KviKvsVariant * c1 = __pCol1OrArray->array()->at(0);\
		KviKvsVariant * c2 = __pCol1OrArray->array()->at(1);\
		KviKvsVariant * c3 = __pCol1OrArray->array()->at(2);\
		if(!(c1 && c2 && c3))\
		{\
			c->error(__tr2qs("One of the array parameters is empty"));\
			return false;\
		}\
		if(!(c1->asInteger(iCol1) && c2->asInteger(__iCol2) && c3->asInteger(__iCol3)))\
		{\
			c->error(__tr2qs("One of the array parameters didn't evaluate to an integer"));\
			return false;\
		}\
	} else {\
		if(c->params()->count() < 3)\
		{\
			QString error=function+" requires either an array as first parameter or 3 integers";\
			c->error(__tr2qs(error));\
			return false;\
		}\
		if(!__pCol1OrArray->asInteger(iCol1))\
		{\
			c->error(__tr2qs("The first parameter didn't evaluate to an array nor an integer"));\
			return false;\
		}\
	}\

bool KviKvsObject_painter::functionSetFont(KviKvsObjectFunctionCall *c)
{
	QString szFamily,szStyle;
	kvs_int_t uSize;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("size",KVS_PT_UNSIGNEDINTEGER,0,uSize)
		KVSO_PARAMETER("family",KVS_PT_STRING,0,szFamily)
		KVSO_PARAMETER("style",KVS_PT_STRING,0,szStyle)
	KVSO_PARAMETERS_END(c)
	if(!m_pPainter)return true;
	QFont font=m_pPainter->font();
	font.setFamily(szFamily);
	font.setPointSize(uSize);
	if(KviQString::equalCI(szStyle,"italic")) font.setItalic(TRUE);
	if(KviQString::equalCI(szStyle,"bold")) font.setBold(TRUE);
	if(KviQString::equalCI(szStyle,"underline"))font.setUnderline(TRUE);
	if(KviQString::equalCI(szStyle,"overline")) font.setOverline(TRUE);
	if(KviQString::equalCI(szStyle,"strikeout"))font.setStrikeOut(TRUE);
	if(KviQString::equalCI(szStyle,"fixedpitch")) font.setFixedPitch(TRUE);
	m_pPainter->setFont(font);
	return true;
}
bool KviKvsObject_painter::functionSetFontSize(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uSize;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("size",KVS_PT_UNSIGNEDINTEGER,0,uSize)
	KVSO_PARAMETERS_END(c)
	if(!m_pPainter)return true;
	QFont font=m_pPainter->font();
	font.setPointSize(uSize);
	m_pPainter->setFont(font);
	return true;
}
bool KviKvsObject_painter::functionsetBackGroundMode(KviKvsObjectFunctionCall *c)
{
	QString szMode;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("background mode",KVS_PT_STRING,0,szMode)
	KVSO_PARAMETERS_END(c)
	if(!m_pPainter)return true;
	if(KviQString::equalCI(szMode,"Transparent")) m_pPainter->setBackgroundMode(Qt::TransparentMode);
	else if(KviQString::equalCI(szMode,"Opaque")) m_pPainter->setBackgroundMode(Qt::OpaqueMode);
	else c->warning( __tr2qs("Unknown orientation"));
	return true;
	
}

bool KviKvsObject_painter::functiondrawLine(KviKvsObjectFunctionCall *c)
{
	KviKvsVariant * pXOrArray;
	kvs_int_t iX,iY,iW,iH;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x_or_array",KVS_PT_VARIANT,0,pXOrArray)
		KVSO_PARAMETER("y",KVS_PT_INT,KVS_PF_OPTIONAL,iY)
		KVSO_PARAMETER("w",KVS_PT_INT,KVS_PF_OPTIONAL,iW)
		KVSO_PARAMETER("h",KVS_PT_INT,KVS_PF_OPTIONAL,iH)
	KVSO_PARAMETERS_END(c)
	QString function="$drawLine";
	KVSO_PARAMETERS_PAINTER(pXOrArray,iY,iW,iH)
	if(m_pPainter)	m_pPainter->drawLine(iX,iY,iW,iH);
	return true;
}
bool KviKvsObject_painter::functiondrawRect(KviKvsObjectFunctionCall *c)
{
	KviKvsVariant * pXOrArray;
	kvs_int_t iX,iY,iW,iH;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x_or_array",KVS_PT_VARIANT,0,pXOrArray)
		KVSO_PARAMETER("y",KVS_PT_INT,KVS_PF_OPTIONAL,iY)
		KVSO_PARAMETER("w",KVS_PT_INT,KVS_PF_OPTIONAL,iW)
		KVSO_PARAMETER("h",KVS_PT_INT,KVS_PF_OPTIONAL,iH)
	KVSO_PARAMETERS_END(c)
	QString function="$drawRect";
	KVSO_PARAMETERS_PAINTER(pXOrArray,iY,iW,iH)
	if(m_pPainter)	m_pPainter->drawRect(iX,iY,iW,iH);
	return true;
}
// FIX ME: REMOVE functiondrawWinFocusRect
bool KviKvsObject_painter::functiondrawWinFocusRect(KviKvsObjectFunctionCall *c)
{
	KviKvsVariant * pXOrArray;
	kvs_int_t iX,iY,iW,iH;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x_or_array",KVS_PT_VARIANT,0,pXOrArray)
		KVSO_PARAMETER("y",KVS_PT_INT,KVS_PF_OPTIONAL,iY)
		KVSO_PARAMETER("w",KVS_PT_INT,KVS_PF_OPTIONAL,iW)
		KVSO_PARAMETER("h",KVS_PT_INT,KVS_PF_OPTIONAL,iH)
	KVSO_PARAMETERS_END(c)
	QString function="$drawWinFocusRect";
	KVSO_PARAMETERS_PAINTER(pXOrArray,iY,iW,iH)
#ifndef COMPILE_USE_QT4
		if(m_pPainter)	m_pPainter->drawWinFocusRect(iX,iY,iW,iH);
#endif
	return true;
}
bool KviKvsObject_painter::functiondrawEllipse(KviKvsObjectFunctionCall *c)
{
	KviKvsVariant * pXOrArray;
	kvs_int_t iX,iY,iW,iH;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x_or_array",KVS_PT_VARIANT,0,pXOrArray)
		KVSO_PARAMETER("y",KVS_PT_INT,KVS_PF_OPTIONAL,iY)
		KVSO_PARAMETER("w",KVS_PT_INT,KVS_PF_OPTIONAL,iW)
		KVSO_PARAMETER("h",KVS_PT_INT,KVS_PF_OPTIONAL,iH)
	KVSO_PARAMETERS_END(c)
	QString function="$drawEllipse";
	KVSO_PARAMETERS_PAINTER(pXOrArray,iY,iW,iH)
	if(m_pPainter)	m_pPainter->drawEllipse(iX,iY,iW,iH);
	return true;
}
bool KviKvsObject_painter::functiondrawRoundRect(KviKvsObjectFunctionCall *c)
{
	KviKvsVariant * pXOrArray;
	kvs_int_t iXrnd,iYrnd,iX,iY,iW,iH;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x_round",KVS_PT_INT,0,iXrnd)
		KVSO_PARAMETER("y_round",KVS_PT_INT,0,iYrnd)
		KVSO_PARAMETER("x_or_array",KVS_PT_VARIANT,0,pXOrArray)
		KVSO_PARAMETER("y",KVS_PT_INT,KVS_PF_OPTIONAL,iY)
		KVSO_PARAMETER("w",KVS_PT_INT,KVS_PF_OPTIONAL,iW)
		KVSO_PARAMETER("h",KVS_PT_INT,KVS_PF_OPTIONAL,iH)
	KVSO_PARAMETERS_END(c)
	QString function="$drawRoundRect";
	KVSO_PARAMETERS_PAINTER(pXOrArray,iY,iW,iH)
	if(m_pPainter)
		m_pPainter->drawRoundRect(iX,iY,iW,iH,iXrnd,iYrnd);
	return true;
}
bool KviKvsObject_painter::functiondrawArc(KviKvsObjectFunctionCall *c)
{
	KviKvsVariant * pXOrArray;
	kvs_int_t iSangle,iLena,iX,iY,iW,iH;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("start_angle",KVS_PT_INT,0,iSangle)
		KVSO_PARAMETER("a_lenght",KVS_PT_INT,0,iLena)
		KVSO_PARAMETER("x_or_array",KVS_PT_VARIANT,0,pXOrArray)
		KVSO_PARAMETER("y",KVS_PT_INT,KVS_PF_OPTIONAL,iY)
		KVSO_PARAMETER("w",KVS_PT_INT,KVS_PF_OPTIONAL,iW)
		KVSO_PARAMETER("h",KVS_PT_INT,KVS_PF_OPTIONAL,iH)
	KVSO_PARAMETERS_END(c)
	QString function="$drawArc";
	KVSO_PARAMETERS_PAINTER(pXOrArray,iY,iW,iH)
	if(m_pPainter)
		m_pPainter->drawArc(iX,iY,iW,iH,iSangle,iLena);
	return true;
}
bool KviKvsObject_painter::functiondrawChord(KviKvsObjectFunctionCall *c)
{
	KviKvsVariant * pXOrArray;
	kvs_int_t iSangle,iLena,iX,iY,iW,iH;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("start_angle",KVS_PT_INT,0,iSangle)
		KVSO_PARAMETER("a_lenght",KVS_PT_INT,0,iLena)
		KVSO_PARAMETER("x_or_array",KVS_PT_VARIANT,0,pXOrArray)
		KVSO_PARAMETER("y",KVS_PT_INT,KVS_PF_OPTIONAL,iY)
		KVSO_PARAMETER("w",KVS_PT_INT,KVS_PF_OPTIONAL,iW)
		KVSO_PARAMETER("h",KVS_PT_INT,KVS_PF_OPTIONAL,iH)
	KVSO_PARAMETERS_END(c)
	QString function="$drawChord";
	KVSO_PARAMETERS_PAINTER(pXOrArray,iY,iW,iH)
	if(m_pPainter)
		m_pPainter->drawChord(iX,iY,iW,iH,iSangle,iLena);
	return true;
}
bool KviKvsObject_painter::functionsetBrush(KviKvsObjectFunctionCall *c)
{
	KviKvsVariant * pCol1OrArray;
	kvs_int_t iCol1,iCol2,iCol3;
	QString szColorMode;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("Col1_or_array",KVS_PT_VARIANT,0,pCol1OrArray)
		KVSO_PARAMETER("Color_2",KVS_PT_INT,KVS_PF_OPTIONAL,iCol2)
		KVSO_PARAMETER("Colo3_3",KVS_PT_INT,KVS_PF_OPTIONAL,iCol3)
		KVSO_PARAMETER("color_mode",KVS_PT_STRING,KVS_PF_OPTIONAL,szColorMode)
	KVSO_PARAMETERS_END(c)
	QString function="$drawChord";
	KVSO_3ARRAYPARAMETERS(pCol1OrArray,iCol2,iCol3)
	if(!m_pPainter) return true;
	if(KviQString::equalCI(szColorMode, "HSV"))
		m_pPainter->setBrush(QColor(iCol1,iCol2,iCol3,QColor::Hsv));
	else
		m_pPainter->setBrush(QColor(iCol1,iCol2,iCol3,QColor::Rgb));
	return true;
}
bool KviKvsObject_painter::functionsetPen(KviKvsObjectFunctionCall *c)
{
	KviKvsVariant * pCol1OrArray;
	kvs_int_t iCol1,iCol2,iCol3;
	QString szColorMode;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("Col1_or_array",KVS_PT_VARIANT,0,pCol1OrArray)
		KVSO_PARAMETER("Color_2",KVS_PT_INT,KVS_PF_OPTIONAL,iCol2)
		KVSO_PARAMETER("Colo3_3",KVS_PT_INT,KVS_PF_OPTIONAL,iCol3)
		KVSO_PARAMETER("color_mode",KVS_PT_STRING,KVS_PF_OPTIONAL,szColorMode)
	KVSO_PARAMETERS_END(c)
	QString function="$setPen";
	KVSO_3ARRAYPARAMETERS(pCol1OrArray,iCol2,iCol3)
	if(!m_pPainter) return true;
	if(KviQString::equalCI(szColorMode, "HSV"))
		m_pPainter->setPen(QColor(iCol1,iCol2,iCol3,QColor::Hsv));
	else
		m_pPainter->setPen(QColor(iCol1,iCol2,iCol3,QColor::Rgb));
	
	return true;
}
bool KviKvsObject_painter::functiondrawPie(KviKvsObjectFunctionCall *c)
{
	KviKvsVariant * pXOrArray;
	kvs_int_t iSangle,iLena,iX,iY,iW,iH;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("start_angle",KVS_PT_INT,0,iSangle)
		KVSO_PARAMETER("a_lenght",KVS_PT_INT,0,iLena)
		KVSO_PARAMETER("x_or_array",KVS_PT_VARIANT,0,pXOrArray)
		KVSO_PARAMETER("y",KVS_PT_INT,KVS_PF_OPTIONAL,iY)
		KVSO_PARAMETER("w",KVS_PT_INT,KVS_PF_OPTIONAL,iW)
		KVSO_PARAMETER("h",KVS_PT_INT,KVS_PF_OPTIONAL,iH)
	KVSO_PARAMETERS_END(c)
	QString function="$drawPie";
	KVSO_PARAMETERS_PAINTER(pXOrArray,iY,iW,iH)
	if(m_pPainter)
		m_pPainter->drawPie(iX,iY,iW,iH,iSangle,iLena);
	return true;
}
bool KviKvsObject_painter::functiondrawPoint(KviKvsObjectFunctionCall *c)
{
	KviKvsVariant * pXOrArray;
	kvs_int_t iX,iY;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x_or_array",KVS_PT_VARIANT,0,pXOrArray)
		KVSO_PARAMETER("y",KVS_PT_INT,KVS_PF_OPTIONAL,iY)
	KVSO_PARAMETERS_END(c)
	QString function="$drawPoint";
	KVSO_2ARRAYPARAMETERS(pXOrArray,iY)
	if(m_pPainter)
		m_pPainter->drawPoint(iX,iY);
	return true;
}
bool KviKvsObject_painter::functionfontDescent(KviKvsObjectFunctionCall * c)
{
	if(m_pPainter) 
		c->returnValue()->setInteger(m_pPainter->fontMetrics().descent());
    return true;
}

bool KviKvsObject_painter::functionfontAscent(KviKvsObjectFunctionCall * c)
{
	if(m_pPainter) 
		c->returnValue()->setInteger(m_pPainter->fontMetrics().ascent());
    return true;
	
}

bool KviKvsObject_painter::functionfontMetricsWidth(KviKvsObjectFunctionCall * c)
{
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	if(m_pPainter)
		c->returnValue()->setInteger(m_pPainter->fontMetrics().width(szText));
	return true;
}

bool KviKvsObject_painter::functionfontMetricsHeight(KviKvsObjectFunctionCall * c)
{
	if(m_pPainter)
		c->returnValue()->setInteger(m_pPainter->fontMetrics().height());
    return true;
}
bool KviKvsObject_painter::functionbegin(KviKvsObjectFunctionCall *c)
{
	KviKvsObject * pObject;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("paint_device",KVS_PT_HOBJECT,0,hObject)
	KVSO_PARAMETERS_END(c)
	pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!m_pPainter)return true;
	if (!pObject)
	{
		c->warning(__tr2qs("Pixmap or Widget parameter is not an object"));
		return true;
	}

	QPaintDevice * pd = 0;
	if(pObject->inherits("KviKvsObject_pixmap"))pd =((KviKvsObject_pixmap *)pObject)->getPixmap();
	else if (pObject->inherits("KviKvsObject_widget")) pd=((KviKvsObject_widget *)pObject)->widget();
	
	if (!pd)
		c->warning(__tr2qs("Widget or Pixmap required "));
	else {
		attachDevice(pObject,pd);
	//	m_pPainter->setOpacity(0.4);	
		if (pObject->inherits("KviKvsObject_pixmap")) ((KviKvsObject_pixmap *)pObject)->pixmapChanged();
	}
	return true;
}
void KviKvsObject_painter::attachDevice(KviKvsObject * o,QPaintDevice * p)
{
	if(!m_pPainter)return;
	if(m_pDeviceObject)detachDevice();
	m_pDeviceObject = o;
	// KviKvsObject_pixmap and KviKvsObject_widget object have this signal
	// it is emitted BEFORE the real QPaintDevice is deleted, so we can eventually
	// call m_pPainter->end() in time

	QObject::connect(m_pDeviceObject,SIGNAL(aboutToDie()),this,SLOT(detachDevice()));
	m_pPainter->begin(p);
}

void KviKvsObject_painter::detachDevice()
{
	if(!m_pDeviceObject)return;
	if(!m_pPainter)return;
	disconnect(m_pDeviceObject,SIGNAL(aboutToDie()),this,SLOT(detachDevice()));
	m_pPainter->end();
	m_pDeviceObject = 0;
}

bool KviKvsObject_painter::functionend(KviKvsObjectFunctionCall *c)
{
	detachDevice();
	return true;
}

bool KviKvsObject_painter::functiondrawText(KviKvsObjectFunctionCall *c)
{
	QString szText,szMode;
	kvs_int_t iX,iY,iLen;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x",KVS_PT_INT,0,iX)
		KVSO_PARAMETER("y",KVS_PT_INT,0,iY)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
		KVSO_PARAMETER("length",KVS_PT_INT,0,iLen)
		KVSO_PARAMETER("mode",KVS_PT_STRING,KVS_PF_OPTIONAL,szMode)
	KVSO_PARAMETERS_END(c)
	if(!m_pPainter)return true;
#ifdef COMPILE_USE_QT4
	if (!szMode.isEmpty()){
		if(KviQString::equalCI(szMode,"RTL"))m_pPainter->setLayoutDirection(Qt::RightToLeft);
		else if(KviQString::equalCI(szMode,"LTR"))m_pPainter->setLayoutDirection(Qt::LeftToRight);
		else
		{
				c->warning(__tr2qs("Invalid mode '%Q'"),&szMode);
				return true;
		}
	}

	 m_pPainter->drawText(iX,iY,szText);
#else
	if(KviQString::equalCI(szMode,"Auto")) m_pPainter->drawText(iX,iY,szText,iLen,QPainter::Auto);
	else if(KviQString::equalCI(szMode,"RTL")) m_pPainter->drawText(iX,iY,szText,iLen,QPainter::RTL);
	else if(KviQString::equalCI(szMode,"LTR"))m_pPainter->drawText(iX,iY,szText,iLen,QPainter::LTR);
	else c->warning(__tr2qs("Invalid mode '%Q'"),&szMode);

#endif
	return true;
}

bool KviKvsObject_painter::functiondrawPixmap(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iX,iY,iStartx,iStarty,iEndx,iEndy;
	KviKvsObject *obj;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x",KVS_PT_INT,0,iX)
		KVSO_PARAMETER("y",KVS_PT_INT,0,iY)
		KVSO_PARAMETER("pixmap",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("start_x",KVS_PT_INT,0,iStartx)
		KVSO_PARAMETER("start_y",KVS_PT_INT,0,iStarty)
		KVSO_PARAMETER("end_x",KVS_PT_INT,0,iEndx)
		KVSO_PARAMETER("end_y",KVS_PT_INT,0,iEndy)
	KVSO_PARAMETERS_END(c)
	obj=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!m_pPainter)return true;
	if (!obj)
	{
		c->warning(__tr2qs("Pixmap parameter is not an object"));
		return true;
	}
	if (!obj->inherits("KviKvsObject_pixmap"))
	{
		c->warning(__tr2qs("Pixmap object required"));
		return true;
	}
	QPixmap * pm=((KviKvsObject_pixmap *)obj)->getPixmap();
	m_pPainter->drawPixmap(iX,iY,*((KviKvsObject_pixmap *)obj)->getPixmap(),iStartx,iStarty,iEndx,iEndy);
	return true;
}

bool KviKvsObject_painter::functionrotateMatrix(KviKvsObjectFunctionCall *c)
{
	kvs_real_t dAngle;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("angle",KVS_PT_DOUBLE,0,dAngle)
	KVSO_PARAMETERS_END(c)
	if(!m_pPainter)return true;
	m_pMatrix.rotate(dAngle);
    m_pPainter->setWorldMatrix( m_pMatrix,false );
	return true;
}
bool KviKvsObject_painter::functiontranslateMatrix(KviKvsObjectFunctionCall *c)
{
	kvs_real_t dXtrasl,dYtrasl;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("trasl_x",KVS_PT_DOUBLE,0,dXtrasl)
		KVSO_PARAMETER("trasl_y",KVS_PT_DOUBLE,0,dYtrasl)
	KVSO_PARAMETERS_END(c)
	if(!m_pPainter)return true;
	QWMatrix tmpMatrix;
    tmpMatrix.translate(dXtrasl,dYtrasl);
    tmpMatrix = m_pMatrix * tmpMatrix;
    m_pPainter->setWorldMatrix( tmpMatrix );
    m_pMatrix=tmpMatrix;
	return true;
}
bool KviKvsObject_painter::functionshearMatrix(KviKvsObjectFunctionCall *c)
{
	kvs_real_t dShearh,dShearv;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("shear_h",KVS_PT_DOUBLE,0,dShearh)
		KVSO_PARAMETER("shear_v",KVS_PT_DOUBLE,0,dShearv)
	KVSO_PARAMETERS_END(c)
	if(!m_pPainter)return true;
	m_pMatrix.shear(dShearh,dShearv);
	m_pPainter->setWorldMatrix(m_pMatrix);
	return true;
}
bool KviKvsObject_painter::functionscaleMatrix(KviKvsObjectFunctionCall *c)
{
	kvs_real_t dScalex,dScaley;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("scale_x",KVS_PT_DOUBLE,0,dScalex)
		KVSO_PARAMETER("scale_y",KVS_PT_DOUBLE,0,dScaley)
	KVSO_PARAMETERS_END(c)
	if(!m_pPainter)return true;
	m_pMatrix.scale(dScalex,dScaley);
	m_pPainter->setWorldMatrix(m_pMatrix);
	return true;
}
bool KviKvsObject_painter::functionresetMatrix(KviKvsObjectFunctionCall *c)

{
	if(!m_pPainter)return true;
	m_pMatrix.reset();
	m_pPainter->setWorldMatrix( m_pMatrix );
    return true;
}

#ifdef COMPILE_USE_QT4
bool KviKvsObject_painter::functionsetOpacity(KviKvsObjectFunctionCall *c)
{
	if(!m_pPainter)return true; 
	kvs_real_t dOpacity;
		
	KVSO_PARAMETERS_BEGIN(c)
			KVSO_PARAMETER("opacity_factor",KVS_PT_DOUBLE,0,dOpacity)	
	KVSO_PARAMETERS_END(c)
	m_pPainter->setOpacity(dOpacity);
	return true;
}
bool KviKvsObject_painter::functionsetTextAntialiasing(KviKvsObjectFunctionCall *c)
{
	if(!m_pPainter)return true; 
	
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	m_pPainter->setRenderHint(QPainter::TextAntialiasing,bEnabled);
	return true;
}
bool KviKvsObject_painter::functionsetAntialiasing(KviKvsObjectFunctionCall *c)
{
	if(!m_pPainter)return true; 
	
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	m_pPainter->setRenderHint(QPainter::Antialiasing,bEnabled);
	return true;
}
bool KviKvsObject_painter::functionsetSmoothPixmapTransform(KviKvsObjectFunctionCall *c)
{
	if(!m_pPainter)return true; 
	
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	m_pPainter->setRenderHint(QPainter::SmoothPixmapTransform,bEnabled);
	return true;
}
#endif



#include "m_class_painter.moc"
