//=============================================================================
//
//   File : KvsObject_painter.cpp
//   Creation date : Fri Mar 18 14:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//   Lucia Papini (^ashura^)  English Translation.
//
//   This file is part of the KVIrc irc client distribution
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "KvsObject_painter.h"
#include "KvsObject_pixmap.h"
#include "KvsObject_widget.h"

#include "kvi_debug.h"
#include "KviLocale.h"
#include "KviError.h"
#include "KviIconManager.h"
#include "KviMemory.h"
 #include <QPrintDialog>
#include <QTextDocument>
#include <QIcon>

#define PAINTER_ROTATE(__angle,__axis)\
         QTransform transform;\
     transform.rotate(__angle,__axis);\
     m_pPainter->setTransform(transform,true);

#define PAINTER_TRANSLATE(__x,__y)\
         QTransform transform;\
     transform.translate(__x,__y);\
      m_pPainter->setTransform(transform,true);

#define PAINTER_SHEAR(__sH,__sV)\
         QTransform transform;\
    transform.shear(__sH,__sV);\
     m_pPainter->setTransform(transform,true);

#define PAINTER_SCALE(__sX,__sY)\
         QTransform transform;\
     transform.scale(__sX,__sY);\
     m_pPainter->setTransform(transform,true);


const Qt::PenStyle penstyles_cod[] = {
	Qt::NoPen,
	Qt::SolidLine,
	Qt::DashLine,
	Qt::DotLine,
	Qt::DashDotLine,
	Qt::DashDotDotLine
};

const char * const penstyles_tbl[] = {
	"noPen",
	"solidLine",
	"dashLine",
	"dotLine",
	"dashDotLine",
	"dashDotDotLine"
};

#define penstyles_num	(sizeof(penstyles_tbl) / sizeof(penstyles_tbl[0]))

const Qt::BrushStyle brushstyles_cod[] = {
	Qt::NoBrush,
	Qt::SolidPattern,
	Qt::Dense1Pattern,
	Qt::Dense2Pattern,
	Qt::Dense3Pattern,
	Qt::Dense4Pattern,
	Qt::Dense5Pattern,
	Qt::Dense6Pattern,
	Qt::Dense7Pattern,
	Qt::HorPattern,
	Qt::VerPattern,
	Qt::CrossPattern,
	Qt::BDiagPattern,
	Qt::FDiagPattern,
	Qt::DiagCrossPattern
};

const char * const brushstyles_tbl[] = {
	"NoBrush",
	"SolidPattern",
	"Dense1Pattern",
	"Dense2Pattern",
	"Dense3Pattern",
	"Dense4Pattern",
	"Dense5Pattern",
	"Dense6Pattern",
	"Dense7Pattern",
	"HorPattern",
	"VerPattern",
	"CrossPattern",
	"BDiagPattern",
	"FDiagPattern",
	"DiagCrossPattern"
};

#define brushstyles_num	(sizeof(brushstyles_tbl) / sizeof(brushstyles_tbl[0]))


/*
	@doc:   painter
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
		!fn: $beginPdf(<file_name:string>)
		Begins painting on a file using the pdf format.
		At present the pdf support is in experimental state.
		!fn: $end()
		Ends painting. Any resources used while painting are released.
		!fn: $setPen(<rgb or hsv array value or [<red>,<green>,<blue>][<hue>,<saturation>,<value>],[system color:RGB or HSV)
		The pen defines the lines or text, color. You can set it with Red,Green,Blue, or H,S,V value[br]
		All parameters are in integer form.
		!fn: $setPen(<rgb or hsv array value or [<red>,<green>,<blue>][<hue>,<saturation>,<value>],[opacity],[system color:RGB or HSV])
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
                !fn: $drawRoundRect(<start_angle:integer>,<angle_length:integer>,<width:unsigned integer>,<height:unsigned integer>,<xr:integer>,<y:integer>)
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
		!fn: $drawText(<x:integer>,<y:integer>,<text:string>,[<nr chars:integer>,<dir:enum>])
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
		!fn: $setFont(<family:string>,<size:integer>[,<style:enum>,<style:enum>,..])[br]
		Set the font's family, size and style, valid flag for style are:[br]
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
                !fn: $rotate(<angle:real>,[<axis:string>])
                Rotates the coordinate system a degrees counterclockwise by the given angle about the optional parameter <axis>.
                Valid values for axis are: ZAxis, XAxis, YAxis.
                Optional parameter <axis>
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
                !fn: $setOpacity(<opacity_factor:real>)
		Sets the painter opacity that affects all painter operations (drawpixmap, drawtext...). Valid values range are from 0 (total transparency) to 1 (total opacity)[br]
		You must invoke the [classfnc]$begin[/classfnc] before using it.
                !fn: $setTextAntialiasing(<boolean>)
		Enable/disable antialias in text if possible.
		You must call the [classfnc]$begin[/classfnc] before using it.
                !fn: $setAntialiasing(<boolean>)
		Enable/disable antialias in edges of primitives if possible.
		You must call the [classfnc]$begin[/classfnc] before using it.
                !fn: $setSmoothPixmapTransform(<boolean>)
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
		%Hello->$setWindowTitle("Painter effects" );[br]
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
KVSO_BEGIN_REGISTERCLASS(KvsObject_painter,"painter","object")

	// Fonts
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,setFont)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,setFontSize)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,fontAscent)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,fontDescent)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,fontMetricsHeight)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,fontMetricsWidth)

	// Draws
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,drawRect)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,drawLine)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,drawRoundRect)
        KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,drawPoint)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,drawArc)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,drawEllipse)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,drawPie)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,drawChord)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,fillRect)

	// Text & Pixmap & Icons
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,drawText)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,drawPixmap)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,drawIcon)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,drawHtmlText)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,htmlTextSize)


        // Transform Operation
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,rotate)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,shear)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,scale)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,translate)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,reset)



	// ??
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,setOpacity)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,setTextAntialiasing)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,setAntialiasing)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,setSmoothPixmapTransform)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,setBackGroundMode)



	// QPainterPath stuff
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,drawPath)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,resetPath)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,pathAddText)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,pathMoveTo)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,pathLineTo)

	// Pen
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,setPenJoinStyle)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,setPenCapStyle)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,setPenWidth)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,setPenStyle)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,setPen)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,colorNames)


	// Brush
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,setBrush)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,setBrushStyle)


	// Gradients
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,setGradientStart)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,setGradientStop)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,setGradientColor)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,setGradientAsBrush)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,clearGradient)

	// painter state
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,save)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,restore)

	// painter device
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,painterDeviceWidth)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,painterDeviceHeight)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,begin)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,end)

	// extra
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter,beginPdf)


KVSO_END_REGISTERCLASS(KvsObject_painter)


KVSO_BEGIN_CONSTRUCTOR(KvsObject_painter,KviKvsObject)

	m_pPainter = new QPainter();
        bDonotdeleteinternalqpainter=false;
	m_pPrinter = 0 ;
	m_pDeviceObject=0;
	m_pPainterPath=0;
	m_pGradient=0;

KVSO_END_CONSTRUCTOR(KvsObject_painter)

KVSO_BEGIN_DESTRUCTOR(KvsObject_painter)

if (m_pPainterPath) delete m_pPainterPath;
	m_pPainterPath=0;
if (m_pGradient) delete m_pGradient;
	m_pGradient=0;
if (m_pPainter && !bDonotdeleteinternalqpainter) delete m_pPainter;
	m_pPainter = 0;
if (m_pPrinter) delete m_pPrinter;
	m_pPrinter = 0;

KVSO_END_CONSTRUCTOR(KvsObject_painter)

#define KVSO_PARAMETERS_PAINTER(__pXOrArray,__iY,__iW,__iH)\
if(__pXOrArray->isArray())\
	{\
		if(__pXOrArray->array()->size() < 4)\
		{\
			c->error(__tr2qs_ctx("The array passed as parameter must contain at least 4 elements","objects"));\
			return false;\
		}\
		KviKvsVariant * pX = __pXOrArray->array()->at(0);\
		KviKvsVariant * pY = __pXOrArray->array()->at(1);\
		KviKvsVariant * pW = __pXOrArray->array()->at(2);\
		KviKvsVariant * pH = __pXOrArray->array()->at(3);\
		if(!(pX && pY && pW && pH))\
		{\
			c->error(__tr2qs_ctx("One of the geometry array parameters is empty","objects"));\
			return false;\
		}\
		if(!(pX->asInteger(iX) && pY->asInteger(__iY) && pW->asInteger(__iW) && pH->asInteger(__iH)))\
		{\
			c->error(__tr2qs_ctx("One of the geometry array parameters didn't evaluate to an integer","objects"));\
			return false;\
		}\
	} else {\
		if(c->params()->count() < 4)\
		{\
			QString error=function+__tr2qs_ctx(" requires either an array as first parameter or four integers","objects");\
			c->error(error);\
			return false;\
		}\
		if(!__pXOrArray->asInteger(iX))\
		{\
			c->error(__tr2qs_ctx("The first parameter didn't evaluate to an array nor an integer","objects"));\
			return false;\
		}\
	}\

#define KVSO_2ARRAYPARAMETERS(__pXOrArray,__iY)\
if(__pXOrArray->isArray())\
	{\
		if(__pXOrArray->array()->size() < 2)\
		{\
			c->error(__tr2qs_ctx("The array passed as parameter must contain at least 2 elements","objects"));\
			return false;\
		}\
		KviKvsVariant * pX = __pXOrArray->array()->at(0);\
		KviKvsVariant * pY = __pXOrArray->array()->at(1);\
		if(!(pX && pY ))\
		{\
			c->error(__tr2qs_ctx("One of the array parameters is empty","objects"));\
			return false;\
		}\
		if(!(pX->asInteger(iX) && pY->asInteger(__iY)))\
		{\
			c->error(__tr2qs_ctx("One of the array parameters didn't evaluate to an integer","objects"));\
			return false;\
		}\
	} else {\
		if(c->params()->count() < 2)\
		{\
			QString error=function+__tr2qs_ctx(" requires either an array as first parameter or two integers","objects");\
			c->error(error);\
			return false;\
		}\
		if(!__pXOrArray->asInteger(iX))\
		{\
			c->error(__tr2qs_ctx("The first parameter didn't evaluate to an array nor an integer","objects"));\
			return false;\
		}\
	}\

#define KVSO_3OR4ARRAYPARAMETERS(__pCol1OrArray,__iCol2,__iCol3,__iOpacity)\
if(__pCol1OrArray->isArray())\
	{\
		if(__pCol1OrArray->array()->size() < 3)\
		{\
			c->error(__tr2qs_ctx("The array passed as parameter must contain at least 3 elements","objects"));\
			return false;\
		}\
		KviKvsVariant * c1 = __pCol1OrArray->array()->at(0);\
		KviKvsVariant * c2 = __pCol1OrArray->array()->at(1);\
		KviKvsVariant * c3 = __pCol1OrArray->array()->at(2);\
		if(!(c1 && c2 && c3))\
		{\
			c->error(__tr2qs_ctx("One of the array parameters is empty","objects"));\
			return false;\
		}\
		if(!(c1->asInteger(iCol1) && c2->asInteger(__iCol2) && c3->asInteger(__iCol3)))\
		{\
			c->error(__tr2qs_ctx("One of the array parameters didn't evaluate to an integer","objects"));\
			return false;\
		}\
		if(__pCol1OrArray->array()->size() > 3)\
		{\
			KviKvsVariant * c4 = __pCol1OrArray->array()->at(3);\
			if(!(c4->asInteger(__iOpacity)))\
			{\
				c->error(__tr2qs_ctx("The opacity value didn't evaluate to an integer","objects"));\
				return false;\
			}\
		}\
	} else {\
		if(c->params()->count() < 3)\
		{\
			QString error=function+__tr2qs_ctx(" requires either an array as first parameter or 3 integers","objects");\
			c->error(error);\
			return false;\
		}\
		if(!__pCol1OrArray->asInteger(iCol1))\
		{\
			c->error(__tr2qs_ctx("The first parameter didn't evaluate to an array nor an integer","objects"));\
			return false;\
		}\
	}\

KVSO_CLASS_FUNCTION(painter,setFont)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szFamily;
	QStringList szListStyle;
	kvs_int_t iSize;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("size",KVS_PT_INTEGER,0,iSize)
		KVSO_PARAMETER("family",KVS_PT_STRING,0,szFamily)
		KVSO_PARAMETER("style",KVS_PT_STRINGLIST,KVS_PF_OPTIONAL,szListStyle)
	KVSO_PARAMETERS_END(c)
	QFont font=m_pPainter->font();
	font.setFamily(szFamily);
	font.setPointSize(iSize);
	QString szStyle;
	for(int i=0;i<szListStyle.length();i++)
	{
		szStyle=szListStyle.at(i);
		if(KviQString::equalCI(szStyle,"italic")) font.setItalic(TRUE);
		else if(KviQString::equalCI(szStyle,"bold")) font.setBold(TRUE);
		else if(KviQString::equalCI(szStyle,"underline"))font.setUnderline(TRUE);
		else if(KviQString::equalCI(szStyle,"overline")) font.setOverline(TRUE);
		else if(KviQString::equalCI(szStyle,"strikeout"))font.setStrikeOut(TRUE);
		else if(KviQString::equalCI(szStyle,"fixedpitch")) font.setFixedPitch(TRUE);
		else c->warning(__tr2qs_ctx("Unknown style '%Q'","objects"),&szStyle);
	}
	m_pPainter->setFont(font);
	return true;
}

KVSO_CLASS_FUNCTION(painter,setFontSize)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_uint_t uSize;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("size",KVS_PT_UNSIGNEDINTEGER,0,uSize)
	KVSO_PARAMETERS_END(c)
	QFont font=m_pPainter->font();
	font.setPointSize(uSize);
	m_pPainter->setFont(font);
	return true;
}

KVSO_CLASS_FUNCTION(painter,setBackGroundMode)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szMode;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("background mode",KVS_PT_STRING,0,szMode)
	KVSO_PARAMETERS_END(c)
	if(KviQString::equalCI(szMode,"Transparent")) m_pPainter->setBackgroundMode(Qt::TransparentMode);
	else if(KviQString::equalCI(szMode,"Opaque")) m_pPainter->setBackgroundMode(Qt::OpaqueMode);
	else c->warning( __tr2qs_ctx("Unknown '%Q' background mode","objects"),&szMode);
	return true;
}

KVSO_CLASS_FUNCTION(painter,drawLine)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	KviKvsVariant * pXOrArray;
	kvs_int_t iX,iY,iX1,iY1;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x_or_array",KVS_PT_VARIANT,0,pXOrArray)
		KVSO_PARAMETER("y",KVS_PT_INT,KVS_PF_OPTIONAL,iY)
		KVSO_PARAMETER("x1",KVS_PT_INT,KVS_PF_OPTIONAL,iX1)
		KVSO_PARAMETER("y1",KVS_PT_INT,KVS_PF_OPTIONAL,iY1)
	KVSO_PARAMETERS_END(c)
	QString function="$drawLine";
	KVSO_PARAMETERS_PAINTER(pXOrArray,iY,iX1,iY1)
	if(m_pPainter)	m_pPainter->drawLine(iX,iY,iX1,iY1);
	return true;
}

KVSO_CLASS_FUNCTION(painter,drawRect)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
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
	m_pPainter->drawRect(iX,iY,iW,iH);
	return true;
}


KVSO_CLASS_FUNCTION(painter,drawEllipse)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
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
	m_pPainter->drawEllipse(iX,iY,iW,iH);
	return true;
}

KVSO_CLASS_FUNCTION(painter,drawRoundRect)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
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
	m_pPainter->drawRoundRect(iX,iY,iW,iH,iXrnd,iYrnd);
	return true;
}

KVSO_CLASS_FUNCTION(painter,drawArc)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
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
	m_pPainter->drawArc(iX,iY,iW,iH,iSangle,iLena);
	return true;
}

KVSO_CLASS_FUNCTION(painter,drawChord)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
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
	m_pPainter->drawChord(iX,iY,iW,iH,iSangle,iLena);
	return true;
}

KVSO_CLASS_FUNCTION(painter,setBrush)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_int_t iCol1,iCol2,iCol3,iOpacity;
	KviKvsVariant *var1,*var2,*var3;
	QString szColorMode,szColor;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("Color_1_Or_Colorname_Or_Pixmap",KVS_PT_VARIANT,0,var1)
		KVSO_PARAMETER("Color_2",KVS_PT_VARIANT,KVS_PF_OPTIONAL,var2)
		KVSO_PARAMETER("Colo3_3",KVS_PT_VARIANT,KVS_PF_OPTIONAL,var3)
		KVSO_PARAMETER("color_mode",KVS_PT_STRING,KVS_PF_OPTIONAL,szColorMode)
		KVSO_PARAMETER("opacity",KVS_PT_INT,KVS_PF_OPTIONAL,iOpacity)
	KVSO_PARAMETERS_END(c)
	if(var1->isHObject())
	{
		kvs_hobject_t hObj;
		var1->asHObject(hObj);
		KviKvsObject *pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObj);
		if (!pObject)
		{
			c->warning(__tr2qs_ctx("Pixmap parameter is not an object!","objects"));
			return true;
		}
		if(pObject->inheritsClass("pixmap"))
			m_pPainter->setBrush(*((KvsObject_pixmap *)pObject)->getPixmap());
		else c->warning(__tr2qs_ctx("Object Pixmap required!","objects"));
		return true;
	}
	if (!var1->asInteger(iCol1))
	{
		var1->asString(szColor);
		if (c->paramCount()<2) iOpacity=255;
		else
		{
			if (!var2->asInteger(iOpacity))
			{
				c->warning(__tr2qs_ctx("The opacity parameter didn't evaluate to integer","objects"));
				return true;
			}
		}
		QColor col(szColor);
		col.setAlpha(iOpacity);
		m_pPainter->setBrush(col);
	}
	else
	{
		if(c->paramCount()<3)
		{
			c->error(__tr2qs_ctx("Color name or triplette rgb/hsv value required","objects"));
			return true;
		}
		if (!var2->asInteger(iCol2)||!var3->asInteger(iCol3))
		{
			c->error(__tr2qs_ctx("One of the triplette parameters didn't evaluate to an integer","objects"));\
				return true;
		}
		if (c->paramCount()<5) iOpacity=255;
		QColor col;
		if(KviQString::equalCI(szColorMode, "HSV"))
		col.setHsv(iCol1,iCol2,iCol3,iOpacity);
		else
		col.setRgb(iCol1,iCol2,iCol3,iOpacity);
		m_pPainter->setBrush(col);

	}
	return true;
}

KVSO_CLASS_FUNCTION(painter,setPen)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_int_t iCol1,iCol2,iCol3,iOpacity;
	QString szColorMode,szColor;
	KviKvsVariant *var1,*var2,*var3;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("Color_1_Or_Colorname",KVS_PT_VARIANT,0,var1)
		KVSO_PARAMETER("Color_2",KVS_PT_VARIANT,KVS_PF_OPTIONAL,var2)
		KVSO_PARAMETER("Colo3_3",KVS_PT_VARIANT,KVS_PF_OPTIONAL,var3)
		KVSO_PARAMETER("color_mode",KVS_PT_STRING,KVS_PF_OPTIONAL,szColorMode)
		KVSO_PARAMETER("opacity",KVS_PT_INT,KVS_PF_OPTIONAL,iOpacity)
	KVSO_PARAMETERS_END(c)
	if (!var1->asInteger(iCol1))
	{
		var1->asString(szColor);
		if (c->paramCount()<2) iOpacity=255;
		else
		{
			if (!var2->asInteger(iOpacity))
			{
				c->warning(__tr2qs_ctx("The opacity parameter didn't evaluate to integer","objects"));
				return true;
			}
		}
		QColor col(szColor);
		col.setAlpha(iOpacity);
		m_pPainter->setPen(col);
	}
	else
	{
		if(c->paramCount()<3)
		{
			c->error(__tr2qs_ctx("Color name or triplette rgb/hsv value required","objects"));
			return true;
		}
		if (!var2->asInteger(iCol2)||!var3->asInteger(iCol3))
		{
			c->error(__tr2qs_ctx("One of the triplette parameters didn't evaluate to an integer","objects"));\
				return true;
		}
		if (c->paramCount()<5) iOpacity=255;
		QColor col;
		if(KviQString::equalCI(szColorMode, "HSV"))
		col.setHsv(iCol1,iCol2,iCol3,iOpacity);
		else
		col.setRgb(iCol1,iCol2,iCol3,iOpacity);
		m_pPainter->setPen(col);

	}

	return true;
}

KVSO_CLASS_FUNCTION(painter,drawPie)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
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
	m_pPainter->drawPie(iX,iY,iW,iH,iSangle,iLena);
	return true;
}

KVSO_CLASS_FUNCTION(painter,drawPoint)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	KviKvsVariant * pXOrArray;
	kvs_int_t iX,iY;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x_or_array",KVS_PT_VARIANT,0,pXOrArray)
		KVSO_PARAMETER("y",KVS_PT_INT,KVS_PF_OPTIONAL,iY)
	KVSO_PARAMETERS_END(c)
	QString function="$drawPoint";
	KVSO_2ARRAYPARAMETERS(pXOrArray,iY)
	m_pPainter->drawPoint(iX,iY);
	return true;
}

KVSO_CLASS_FUNCTION(painter,fontDescent)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	c->returnValue()->setInteger(m_pPainter->fontMetrics().descent());
	return true;
}

KVSO_CLASS_FUNCTION(painter,fontAscent)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	c->returnValue()->setInteger(m_pPainter->fontMetrics().ascent());
	return true;

}

KVSO_CLASS_FUNCTION(painter,fontMetricsWidth)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	if (m_pPainter->isActive())
		c->returnValue()->setInteger(m_pPainter->fontMetrics().width(szText));
	else
		c->warning(__tr2qs_ctx("$fontMetricsWidth: the painter is not active!","objects"));
	return true;
}

KVSO_CLASS_FUNCTION(painter,fontMetricsHeight)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	if (m_pPainter->isActive())
		c->returnValue()->setInteger(m_pPainter->fontMetrics().height());
	else
		c->warning(__tr2qs_ctx("$fontMetricsHeight: the painter is not active!","objects"));
	return true;
}

KVSO_CLASS_FUNCTION(painter,begin)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	KviKvsObject * pObject;
	kvs_hobject_t hObject;
        KviKvsVariant *v;
        QString szFileName;
        KVSO_PARAMETERS_BEGIN(c)
                        KVSO_PARAMETER("paint_device",KVS_PT_VARIANT,0,v)
                        KVSO_PARAMETER("file_name",KVS_PT_STRING,KVS_PF_OPTIONAL,szFileName)
	KVSO_PARAMETERS_END(c)
        QPaintDevice * pd;
        if(v->isString())
        {
             QString szDev;
             v->asString(szDev);
             if(KviQString::equalCI(szDev,"printer") || KviQString::equalCI(szDev,"pdf"))
             {
                 if (m_pPrinter) delete m_pPrinter;
                 m_pPrinter=new QPrinter();
                 if ( KviQString::equalCI(szDev,"pdf"))
                 {
                 //        m_pPrinter=new QPrinter();
                     if(szFileName.isEmpty())
                     {
                         c->warning(__tr2qs_ctx("Missing filename","objects"));
                         return true;
                     }
                     m_pPrinter->setOutputFormat(QPrinter::PdfFormat);
                     m_pPrinter->setOutputFileName(szFileName);
                 }
                 else
                 {
                     QPrintDialog printDialog(m_pPrinter, 0);
                     if (printDialog.exec() == QDialog::Accepted)
                     {
                       qDebug("papersize %d",m_pPrinter->paperSize());
                       m_pPainter->begin(m_pPrinter);
                       return true;
                     }
                     else
                     {
                       m_pPrinter=0;
                       return true;
                     }
                   }
                   //if (i==3) m_pPrinter->setPaperSize(QPrinter::A3);
                   //if (i==4) m_pPrinter->setPaperSize(QPrinter::A4);
                }
                else
                {
                    c->warning(__tr2qs_ctx("No valid paint device","objects"));
                    return true;
                }
                m_pPainter->begin(m_pPrinter);
                return true;
        }
        else if (v->isHObject())
        {
                v->asHObject(hObject);
                pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
                if (!pObject)
                {
                    c->warning(__tr2qs_ctx("Pixmap or Widget parameter is not an object","objects"));
                    return true;
                }
                if(pObject->inheritsClass("pixmap"))pd=((KvsObject_pixmap *)pObject)->getPixmap();
                else if (pObject->inheritsClass("widget")) pd=((KvsObject_widget *)pObject)->widget();
                else
                {
                    c->warning(__tr2qs_ctx("Widget or Pixmap required ","objects"));
                    return true;
                }
       }
       else
       {
                c->warning(__tr2qs_ctx("No valid paint device","objects"));
                return true;
       }
       attachDevice(pObject,pd);
            //if (pObject->inheritsClass("pixmap")) ((KvsObject_pixmap *)pObject)->pixmapChanged();
       return true;
}

void KvsObject_painter::attachDevice(KviKvsObject * o,QPaintDevice * p)
{
	if(m_pDeviceObject)detachDevice();
	m_pDeviceObject = o;
	// KvsObject_pixmap and KvsObject_widget object have this signal
	// it is emitted BEFORE the real QPaintDevice is deleted, so we can eventually
	// call m_pPainter->end() in time

	QObject::connect(m_pDeviceObject,SIGNAL(aboutToDie()),this,SLOT(detachDevice()));
	m_pPainter->begin(p);
}

void KvsObject_painter::detachDevice()
{
	if(!m_pDeviceObject)return;
	disconnect(m_pDeviceObject,SIGNAL(aboutToDie()),this,SLOT(detachDevice()));
	m_pPainter->end();
	if (m_pPrinter)
	{
		delete m_pPrinter;
		m_pPrinter=0;
	}
	m_pDeviceObject = 0;
}

KVSO_CLASS_FUNCTION(painter,end)
{
	Q_UNUSED(c);

	if(!m_pDeviceObject)
	{
		m_pPainter->end();
		if (m_pPrinter)
		{
			delete m_pPrinter;
			m_pPrinter=0;
		}
	}
	else detachDevice();
	return true;
}

KVSO_CLASS_FUNCTION(painter,beginPdf)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szFileName;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("file_name",KVS_PT_STRING,0,szFileName)
	KVSO_PARAMETERS_END(c)
	if (m_pPrinter) delete m_pPrinter;
	m_pPrinter=new QPrinter();//QPrinter::HighResolution);
	m_pPrinter->setOutputFormat(QPrinter::PdfFormat);
	m_pPrinter->setOutputFileName(szFileName);
	m_pPainter->begin(m_pPrinter);
	return true;
}
KVSO_CLASS_FUNCTION(painter,drawText)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szText,szMode;
	kvs_int_t iX,iY,iN=-1;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x",KVS_PT_INT,0,iX)
		KVSO_PARAMETER("y",KVS_PT_INT,0,iY)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
		KVSO_PARAMETER("num_chars",KVS_PT_INT,KVS_PF_OPTIONAL,iN)
		KVSO_PARAMETER("mode",KVS_PT_STRING,KVS_PF_OPTIONAL,szMode)
	KVSO_PARAMETERS_END(c)
	if (!szMode.isEmpty() && !KviQString::equalCI(szMode,"Auto"))
	{
		if(KviQString::equalCI(szMode,"RTL"))m_pPainter->setLayoutDirection(Qt::RightToLeft);
		else if(KviQString::equalCI(szMode,"LTR"))m_pPainter->setLayoutDirection(Qt::LeftToRight);
		else
		{
				c->warning(__tr2qs_ctx("Invalid mode '%Q'","objects"),&szMode);
				return true;
		}
	}
	m_pPainter->drawText(iX,iY,szText.left(iN));
	return true;
}

KVSO_CLASS_FUNCTION(painter,drawPixmap)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t iX,iY;
	kvs_int_t iStartx,iStarty,iEndx,iEndy;
	bool bTiled;
	KviKvsObject *obj;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x",KVS_PT_REAL,0,iX)
		KVSO_PARAMETER("y",KVS_PT_REAL,0,iY)
		KVSO_PARAMETER("pixmap",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("start_x",KVS_PT_INT,0,iStartx)
		KVSO_PARAMETER("start_y",KVS_PT_INT,0,iStarty)
		KVSO_PARAMETER("end_x",KVS_PT_INT,0,iEndx)
		KVSO_PARAMETER("end_y",KVS_PT_INT,0,iEndy)
		KVSO_PARAMETER("b_Tiled",KVS_PT_BOOLEAN,KVS_PF_OPTIONAL,bTiled)
	KVSO_PARAMETERS_END(c)
	obj=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if (!obj)
	{
		c->warning(__tr2qs_ctx("Pixmap parameter is not an object","objects"));
		return true;
	}
	if (!obj->inheritsClass("pixmap"))
	{
		c->warning(__tr2qs_ctx("Pixmap object required","objects"));
		return true;
	}

	m_pPainter->drawPixmap(QPointF(iX,iY),*((KvsObject_pixmap *)obj)->getPixmap(),QRect(iStartx,iStarty,iEndx,iEndy));
	return true;
}

KVSO_CLASS_FUNCTION(painter,rotate)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
        kvs_real_t dAngle;
        QString szAxis;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("angle",KVS_PT_DOUBLE,0,dAngle)
                KVSO_PARAMETER("axis",KVS_PT_STRING,KVS_PF_OPTIONAL,szAxis)
	KVSO_PARAMETERS_END(c)
        Qt::Axis axis = Qt::ZAxis;
        if(!szAxis.isEmpty())
        {
            if (KviQString::equalCI(szAxis,"XAxis"))axis=Qt::XAxis;
            else if (KviQString::equalCI(szAxis,"YAxis")) axis=Qt::YAxis;
            else if (KviQString::equalCI(szAxis,"ZAxis")) axis=Qt::ZAxis;
            else c->warning(__tr2qs_ctx("Unknown axis '%Q' switching to default ZAxis","objects"),&szAxis);
        }
        PAINTER_ROTATE(dAngle,axis)
	return true;
}

KVSO_CLASS_FUNCTION(painter,translate)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t dXtrasl,dYtrasl;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("trasl_x",KVS_PT_DOUBLE,0,dXtrasl)
		KVSO_PARAMETER("trasl_y",KVS_PT_DOUBLE,0,dYtrasl)
	KVSO_PARAMETERS_END(c)
	PAINTER_TRANSLATE(dXtrasl,dYtrasl)
        return true;
}

KVSO_CLASS_FUNCTION(painter,shear)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t dShearh,dShearv;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("shear_h",KVS_PT_DOUBLE,0,dShearh)
		KVSO_PARAMETER("shear_v",KVS_PT_DOUBLE,0,dShearv)
	KVSO_PARAMETERS_END(c)
	PAINTER_SHEAR(dShearh,dShearv);
        return true;
}

KVSO_CLASS_FUNCTION(painter,scale)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t dScalex,dScaley;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("scale_x",KVS_PT_DOUBLE,0,dScalex)
		KVSO_PARAMETER("scale_y",KVS_PT_DOUBLE,0,dScaley)
	KVSO_PARAMETERS_END(c)
	PAINTER_SCALE(dScalex,dScaley);
        return true;
}

KVSO_CLASS_FUNCTION(painter,reset)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
        m_pPainter->resetTransform();
	return true;
}

KVSO_CLASS_FUNCTION(painter,drawHtmlText)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szText;
	kvs_int_t iX,iY,iW,iH;

	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x",KVS_PT_INT,0,iX)
		KVSO_PARAMETER("y",KVS_PT_INT,0,iY)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
                KVSO_PARAMETER("w",KVS_PT_INT,0,iW)
                KVSO_PARAMETER("h",KVS_PT_INT,0,iH)

	KVSO_PARAMETERS_END(c)
        QTextDocument doc;
	doc.setHtml(szText);
	doc.setDefaultFont(m_pPainter->font());
	m_pPainter->save();
	m_pPainter->translate(iX,iY);
        doc.setPageSize(QSize(iW,iH));
	doc.drawContents(m_pPainter);
	m_pPainter->restore();
	return true;
}

KVSO_CLASS_FUNCTION(painter,htmlTextSize)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	QTextDocument doc;
	doc.setHtml(szText);
	doc.setDefaultFont(m_pPainter->font());
	QSizeF size=doc.size();
	KviKvsArray * a = new KviKvsArray();
	a->set(0,new KviKvsVariant((kvs_real_t)size.width()));
	a->set(1,new KviKvsVariant((kvs_real_t)size.height()));
	c->returnValue()->setArray(a);
	return true;
}

KVSO_CLASS_FUNCTION(painter,drawIcon)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szIcon,szState;
	kvs_int_t iX,iY,iW,iH;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x",KVS_PT_INT,0,iX)
		KVSO_PARAMETER("y",KVS_PT_INT,0,iY)
		KVSO_PARAMETER("icon",KVS_PT_STRING,0,szIcon)
		KVSO_PARAMETER("state",KVS_PT_STRING,KVS_PF_OPTIONAL,szState)
		KVSO_PARAMETER("w",KVS_PT_INT,KVS_PF_OPTIONAL,iW)
		KVSO_PARAMETER("h",KVS_PT_INT,KVS_PF_OPTIONAL,iH)
	KVSO_PARAMETERS_END(c)
	QPixmap *pix=g_pIconManager->getImage(szIcon);
	if (!pix)
	{
		c->warning("The icon '%Q' does not exists",&szIcon);
		return true;
	}
	QSize pixsize(pix->width(),pix->height());
	QIcon ico(*pix);
	QIcon::Mode mode=QIcon::Normal;
	if(!ico.isNull())
	{
		if (KviQString::equalCI(szState,"Disabled"))
			mode=QIcon::Disabled;
		else if (KviQString::equalCI(szState,"Active"))mode=QIcon::Active;
		else if (KviQString::equalCI(szState,"Selected"))mode=QIcon::Selected;
		if (c->parameterCount()<5)
			m_pPainter->drawPixmap(iX,iY,ico.pixmap(pixsize,mode));
		else
			m_pPainter->drawPixmap(iX,iY,ico.pixmap(QSize(iW,iH),mode));
	}
	return true;
}

KVSO_CLASS_FUNCTION(painter,setOpacity)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t dOpacity;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("opacity_factor",KVS_PT_DOUBLE,0,dOpacity)
	KVSO_PARAMETERS_END(c)
	m_pPainter->setOpacity(dOpacity);
	return true;
}

KVSO_CLASS_FUNCTION(painter,setTextAntialiasing)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	m_pPainter->setRenderHint(QPainter::TextAntialiasing,bEnabled);
	return true;
}

KVSO_CLASS_FUNCTION(painter,setAntialiasing)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	m_pPainter->setRenderHint(QPainter::Antialiasing,bEnabled);
	return true;
}

KVSO_CLASS_FUNCTION(painter,setSmoothPixmapTransform)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	m_pPainter->setRenderHint(QPainter::SmoothPixmapTransform,bEnabled);
	return true;
}

KVSO_CLASS_FUNCTION(painter,setPenJoinStyle)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szStyle;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("style",KVS_PT_STRING,0,szStyle)
	KVSO_PARAMETERS_END(c)
	Qt::PenJoinStyle style=Qt::BevelJoin;
	if(KviQString::equalCI(szStyle,"Milter"))style=Qt::MiterJoin;
	else if (KviQString::equalCI(szStyle,"Round"))style=Qt::RoundJoin;
	QPen pen=m_pPainter->pen();
	pen.setJoinStyle(style);
	m_pPainter->setPen(pen);
	return true;
}

KVSO_CLASS_FUNCTION(painter,setPenCapStyle)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szStyle;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("style",KVS_PT_STRING,0,szStyle)
	KVSO_PARAMETERS_END(c)
	Qt::PenCapStyle style=Qt::SquareCap;
	if(KviQString::equalCI(szStyle,"Flat"))	style=Qt::FlatCap;
	else if (KviQString::equalCI(szStyle,"Round"))style=Qt::RoundCap;
	QPen pen=m_pPainter->pen();
	pen.setCapStyle(style);
	m_pPainter->setPen(pen);
	return true;
}

KVSO_CLASS_FUNCTION(painter,save)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	m_pPainter->save();
    return true;
}

KVSO_CLASS_FUNCTION(painter,restore)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	m_pPainter->restore();
    return true;
}

KVSO_CLASS_FUNCTION(painter,setPenStyle)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szStyle;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("style",KVS_PT_STRING,0,szStyle)
	KVSO_PARAMETERS_END(c)
	bool found=false;
	unsigned int j = 0;
	for(; j < penstyles_num; j++)
	{
		if(KviQString::equalCI(szStyle,penstyles_tbl[j]))
		{
			found=true;
			break;
		}
	}
	QPen pen=m_pPainter->pen();
	if (found){
		pen.setStyle(penstyles_cod[j]);
		m_pPainter->setPen(pen);
	}
	else c->warning(__tr2qs_ctx("Unknown pen style '%Q'","objects"),&szStyle);
	return true;
}

KVSO_CLASS_FUNCTION(painter,setBrushStyle)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szStyle;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("style",KVS_PT_STRING,0,szStyle)
	KVSO_PARAMETERS_END(c)
	bool found=false;
	unsigned int j = 0;
	for(; j < brushstyles_num; j++)
	{
		if(KviQString::equalCI(szStyle,brushstyles_tbl[j]))
		{
			found=true;
			break;
		}
	}
	QBrush brush=m_pPainter->brush();
	if (found){
		brush.setStyle(brushstyles_cod[j]);
		m_pPainter->setBrush(brush);
	}
	else c->warning(__tr2qs_ctx("Unknown brush style '%Q'","objects"),&szStyle);
	return true;
}

KVSO_CLASS_FUNCTION(painter,setGradientStart)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t dX,dY;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x_Point",KVS_PT_DOUBLE,0,dX)
		KVSO_PARAMETER("y_Point",KVS_PT_DOUBLE,0,dY)
	KVSO_PARAMETERS_END(c)
	if (!m_pGradient) m_pGradient=new(QLinearGradient);
	m_pGradient->setStart(dX,dY);
	return true;
}

KVSO_CLASS_FUNCTION(painter,setGradientStop)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t dX,dY;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x_Point",KVS_PT_DOUBLE,0,dX)
		KVSO_PARAMETER("y_Point",KVS_PT_DOUBLE,0,dY)
	KVSO_PARAMETERS_END(c)
	if (!m_pGradient) m_pGradient=new(QLinearGradient);
	m_pGradient->setFinalStop(dX,dY);
	return true;
}

KVSO_CLASS_FUNCTION(painter,setGradientColor)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t dAt;
	kvs_int_t iCol1,iCol2,iCol3,iOpacity;
	KviKvsVariant *var1,*var2,*var3;
	QString szColorMode,szColor;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("at",KVS_PT_DOUBLE,0,dAt)
		KVSO_PARAMETER("Color_1_Or_Colorname",KVS_PT_VARIANT,0,var1)
		KVSO_PARAMETER("Color_2",KVS_PT_VARIANT,KVS_PF_OPTIONAL,var2)
		KVSO_PARAMETER("Colo3_3",KVS_PT_VARIANT,KVS_PF_OPTIONAL,var3)
		KVSO_PARAMETER("color_mode",KVS_PT_STRING,KVS_PF_OPTIONAL,szColorMode)
		KVSO_PARAMETER("opacity",KVS_PT_INT,KVS_PF_OPTIONAL,iOpacity)
	KVSO_PARAMETERS_END(c)
	if (!var1->asInteger(iCol1))
	{
			var1->asString(szColor);
		if (c->paramCount()<3) iOpacity=255;
		else
		{
			if (!var2->asInteger(iOpacity))
			{
				c->warning(__tr2qs_ctx("The opacity parameter didn't evaluate to integer","objects"));
				return true;
			}
		}
		if (!m_pGradient) m_pGradient=new(QLinearGradient);
		QColor col(szColor);
		col.setAlpha(iOpacity);
		m_pGradient->setColorAt(dAt,col);
	}
	else
	{
		if(c->paramCount()<4)
		{
			c->error(__tr2qs_ctx("Color name or triplette RGB/HSV values required","objects"));
			return true;
		}
		if ( !var2->asInteger(iCol2)||!var3->asInteger(iCol3))
		{
			c->error(__tr2qs_ctx("One of the triplette parameters didn't evaluate to an integer","objects"));\
				return true;
		}
		if (c->paramCount()<5) iOpacity=255;
		QColor col;
		if(KviQString::equalCI(szColorMode, "HSV"))
		col.setHsv(iCol1,iCol2,iCol3,iOpacity);
		else
		col.setRgb(iCol1,iCol2,iCol3,iOpacity);
		if (!m_pGradient) m_pGradient=new(QLinearGradient);
		m_pGradient->setColorAt(dAt,col);

	}
	return true;
}

KVSO_CLASS_FUNCTION(painter,setGradientAsBrush)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	if (!m_pGradient) m_pGradient=new(QLinearGradient);
	m_pPainter->setBrush(*m_pGradient);
	return true;
}

KVSO_CLASS_FUNCTION(painter,clearGradient)
{
	Q_UNUSED(c);
	if (!m_pGradient) delete m_pGradient;
	m_pGradient=0;
	return true;
}

KVSO_CLASS_FUNCTION(painter,setPenWidth)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t iWidth;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("width",KVS_PT_REAL,0,iWidth)
	KVSO_PARAMETERS_END(c)
	QPen pen=m_pPainter->pen();
	pen.setWidthF(iWidth);
	m_pPainter->setPen(pen);
	return true;
}

KVSO_CLASS_FUNCTION(painter,fillRect)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_int_t iX,iY,iW,iH;
	kvs_int_t iCol1,iCol2,iCol3,iOpacity;
	QString szColorMode;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x",KVS_PT_INT,0,iX)
		KVSO_PARAMETER("y",KVS_PT_INT,0,iY)
		KVSO_PARAMETER("w",KVS_PT_INT,0,iW)
		KVSO_PARAMETER("h",KVS_PT_INT,0,iH)
		KVSO_PARAMETER("Color_1",KVS_PT_INT,0,iCol1)
		KVSO_PARAMETER("Color_2",KVS_PT_INT,0,iCol2)
		KVSO_PARAMETER("Colo3_3",KVS_PT_INT,0,iCol3)
		KVSO_PARAMETER("opacity",KVS_PT_INT,KVS_PF_OPTIONAL,iOpacity)
		KVSO_PARAMETER("color_mode",KVS_PT_STRING,KVS_PF_OPTIONAL,szColorMode)
	KVSO_PARAMETERS_END(c)
	if (c->paramCount()<8) iOpacity=255;
	QColor col;
	if(KviQString::equalCI(szColorMode, "HSV"))
		col.setHsv(iCol1,iCol2,iCol3,iOpacity);
	else
		col.setRgb(iCol1,iCol2,iCol3,iOpacity);
	m_pPainter->fillRect(iX,iY,iW,iH,col);
	return true;
}

KVSO_CLASS_FUNCTION(painter,colorNames)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QStringList color=QColor::colorNames();
	KviKvsArray * a = new KviKvsArray();
	//a->set(0,new KviKvsVariant((kvs_int_t)col.red()));
	for(int i=0;i<color.count();i++)
	{
		a->set(i,new KviKvsVariant(color.at(i)));
	}
	c->returnValue()->setArray(a);
	return true;
}
KVSO_CLASS_FUNCTION(painter,painterDeviceWidth)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_int_t width=m_pPainter->device()->width();
	c->returnValue()->setInteger(width);
	return true;
}
KVSO_CLASS_FUNCTION(painter,painterDeviceHeight)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_int_t height=m_pPainter->device()->height();
	c->returnValue()->setInteger(height);
	return true;
}
KVSO_CLASS_FUNCTION(painter,pathMoveTo)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t dX,dY;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x",KVS_PT_REAL,0,dX)
		KVSO_PARAMETER("y",KVS_PT_REAL,0,dY)
	KVSO_PARAMETERS_END(c)
	if (!m_pPainterPath) m_pPainterPath=new QPainterPath(QPointF(0,0));
	m_pPainterPath->moveTo(dX,dY);
	return true;
}

KVSO_CLASS_FUNCTION(painter,pathLineTo)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t dX,dY;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x",KVS_PT_REAL,0,dX)
		KVSO_PARAMETER("y",KVS_PT_REAL,0,dY)
	KVSO_PARAMETERS_END(c)
	if (!m_pPainterPath) m_pPainterPath=new QPainterPath(QPointF(0,0));
	m_pPainterPath->lineTo(dX,dY);
	return true;
}

KVSO_CLASS_FUNCTION(painter,pathAddText)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t dX,dY;
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x",KVS_PT_REAL,0,dX)
		KVSO_PARAMETER("y",KVS_PT_REAL,0,dY)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	if (!m_pPainterPath) m_pPainterPath=new QPainterPath(QPointF(0,0));
	m_pPainterPath->addText(dX,dY,m_pPainter->font(),szText);
	return true;
}

KVSO_CLASS_FUNCTION(painter,drawPath)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	m_pPainter->drawPath(*m_pPainterPath);
	//delete m_pPainterPath;
	//m_pPainterPath=0;
	return true;
}

KVSO_CLASS_FUNCTION(painter,resetPath)
{
	Q_UNUSED(c);
	if (m_pPainterPath){
		delete m_pPainterPath;
		m_pPainterPath=0;
	}
	return true;
}
void KvsObject_painter::setInternalPainter(QPainter *p)
{
    delete m_pPainter;
    m_pPainter=p;
    bDonotdeleteinternalqpainter=true;
}
#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_KvsObject_painter.moc"
#endif //COMPILE_USE_STANDALONE_MOC_SOURCES
