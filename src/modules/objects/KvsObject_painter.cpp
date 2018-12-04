//=============================================================================
//
//   File : KvsObject_painter.cpp
//   Creation date : Fri Mar 18 14:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//   Lucia Papini (^ashura^)  English Translation.
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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

#define PAINTER_ROTATE(__angle, __axis) \
	QTransform transform;               \
	transform.rotate(__angle, __axis);  \
	m_pPainter->setTransform(transform, true);

#define PAINTER_TRANSLATE(__x, __y) \
	QTransform transform;           \
	transform.translate(__x, __y);  \
	m_pPainter->setTransform(transform, true);

#define PAINTER_SHEAR(__sH, __sV) \
	QTransform transform;         \
	transform.shear(__sH, __sV);  \
	m_pPainter->setTransform(transform, true);

#define PAINTER_SCALE(__sX, __sY) \
	QTransform transform;         \
	transform.scale(__sX, __sY);  \
	m_pPainter->setTransform(transform, true);

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
const int align_cod[] = {
	Qt::AlignLeft,
	Qt::AlignRight,
	Qt::AlignHCenter,
	Qt::AlignTop,
	Qt::AlignBottom,
	Qt::AlignVCenter,
	Qt::AlignCenter,
	Qt::TextSingleLine,
	Qt::TextExpandTabs,
	Qt::TextShowMnemonic,
	Qt::TextWordWrap,
	Qt::TextIncludeTrailingSpaces
};
const char * const align_tbl[] = {
	"Left",
	"Right",
	"HCenter",
	"Top",
	"Bottom",
	"VCenter",
	"Center",
	"TextSingleLine",
	"TextExpandTabs",
	"TextShowMnemonic",
	"TextWordWrap",
	"TextIncludeTrailingSpaces"
};
#define align_num (sizeof(align_tbl) / sizeof(align_tbl[0]))

const QPainter::CompositionMode composition_cod[] = {
	QPainter::CompositionMode_SourceOver,
	QPainter::CompositionMode_DestinationOver,
	QPainter::CompositionMode_Clear,
	QPainter::CompositionMode_Source,
	QPainter::CompositionMode_Destination,
	QPainter::CompositionMode_SourceIn,
	QPainter::CompositionMode_DestinationIn,
	QPainter::CompositionMode_SourceOut,
	QPainter::CompositionMode_DestinationOut,
	QPainter::CompositionMode_SourceAtop,
	QPainter::CompositionMode_DestinationAtop,
	QPainter::CompositionMode_Xor,
	QPainter::CompositionMode_Plus,
	QPainter::CompositionMode_Multiply,
	QPainter::CompositionMode_Screen,
	QPainter::CompositionMode_Overlay,
	QPainter::CompositionMode_Darken,
	QPainter::CompositionMode_Lighten,
	QPainter::CompositionMode_ColorDodge,
	QPainter::CompositionMode_ColorBurn,
	QPainter::CompositionMode_HardLight,
	QPainter::CompositionMode_SoftLight,
	QPainter::CompositionMode_Difference,
	QPainter::CompositionMode_Exclusion,
};
const char * const composition_tbl[] = {
	"SourceOver",
	"DestinationOver",
	"Clear",
	"Source",
	"Destination",
	"SourceIn",
	"DestinationIn",
	"SourceOut",
	"DestinationOut",
	"SourceAtop",
	"DestinationAtop",
	"Xor",
	"Plus",
	"Multiply",
	"Screen",
	"Overlay",
	"Darken",
	"Lighten",
	"ColorDodge",
	"ColorBurn",
	"HardLight",
	"SoftLight",
	"Difference",
	"Exclusion"
};

#define penstyles_num (sizeof(penstyles_tbl) / sizeof(penstyles_tbl[0]))

#define composition_num (sizeof(composition_tbl) / sizeof(composition_tbl[0]))

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

#define brushstyles_num (sizeof(brushstyles_tbl) / sizeof(brushstyles_tbl[0]))

/*
	@doc: painter
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
		It can also draw aligned text and pixmaps. Normally, it draws in a [i]natural[/i] coordinate system, but it can also do view and world transformation.[br]
		The class need to be implemented into a [classfnc]$paintEvent[/classfnc]();
	@functions:
		!fn: $drawLine(<x1:integer>,<y1_integer>,<x2:integer>,<y2:integer>)
		Draws a line from (x1, y1) to (x2, y2) and sets the current pen position to (x2, y2).
		!fn: $begin(<paint_device:object>)
		Begins painting the paint device <paint_device>: the parameter [b]must[/b] be a widget or a pixmap.[br]
		Warning: A paint device can only be painted by one painter at a time.
		!fn: $beginPdf(<file_name:string>)
		Begins painting on a file using the PDF format.
		At present the PDF support is in experimental state.
		!fn: $end()
		Ends painting. Any resources used while painting are released.
		!fn: $setPen(<rgb or hsv array value or [<red>,<green>,<blue>][<hue>,<saturation>,<value>],[system color:RGB or HSV)
		The pen defines the lines or text, color. You can set it with Red, Green, Blue, or H, S, V value[br]
		All parameters are in integer form.
		!fn: $setPen(<rgb or hsv array value or [<red>,<green>,<blue>][<hue>,<saturation>,<value>],[opacity],[system color:RGB or HSV])
		The pen defines the lines or text, color. You can set it with Red, Green, Blue, or H, S, V value[br]
		All parameters are in integer form.
		The HSV system, like RGB, has three components:[br]
		[pre]
			* H, for hue, is either 0-359 if the color is chromatic (not gray), or meaningless if it is gray.
			It represents degrees on the color wheel familiar to most people. Red is 0 (degrees), green is 120 and blue is 240.
			* S, for saturation, is 0-255, and the bigger it is, the stronger the color is. Grayish colors have saturation near 0; very strong colors have saturation near 255.
			* V, for value, is 0-255 and represents lightness or brightness of the color. 0 is black; 255 is as far from black as possible.
		[/pre]
		Examples: [b]Red[/b] is H=0, S=255, V=255.[br]
		Light red could have H about 0, S about 50-100, and S=255.
		ES: $setPen(00,00,00) for black;[br]
		Default color mode is RGB;
		!fn: $setBrush(<rgb or hsv array value or [<red>,<green>,<blue>][<hue>,<saturation>,<value>],[system color:RGB or HSV)
		Sets the painter's brush to have the specified color.[br]
		Example:
		[example]
			class (wdg,widget)
			{
				paintevent()
				{
					%b=$new(painter)
					%b->$setBackgroundMode(Opaque)
					%b->$setBrush(0,250,250)
					%b->$begin($$)
					%b->$drawEllipse(50,50,100,50)
				}
			}
			%aa=$new(wdg)[br]
			%aa->$show()
		[/example]
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
		!fn: $drawText(<x:integer>,<y:integer>,<width:unsigned integer>,<height:unsigned integer>,<text:string>[,<flag:string>[,<flag:string>[,...]]])
		Draws the given <text> within the rectangle specified by <x>,<y> <width> and <height>.[br]
		The <flag> parameters may be:[br]
		[pre]
			Left
			Top
			Right
			Bottom
			HCenter
			VCenter
			Center
			TextSingleLine
			TextExpandTabs
			TextShowMnemonic
			TextWordWrap
			TextIncludeTrailingSpaces
		[/pre]
		!fn: $drawPixmap(<x:integer>,<y:integer>,<pixmap:hobject>,<sx:integer>,<sy:integer>,<ex:integer>,<ey:integer>)
		Draws a pixmap at x,y coordinates[br]
		!fn: $setFont(<family:string>,<size:integer>[,<style:enum>,<style:enum>,..])[br]
		Set the font's family, size and style, valid flag for style are:[br]
		[pre]
			italic
			bold
			underline
			overline
			strikeout
			fixedpitch
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
		Sets the background mode of the painter to <bgMode>:
		Valid values are:[br]
		[pre]
			- Transparent (that is the default value);
			- Opaque.
		[/pre]
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
		You must call the [classfnc]$begin[/classfnc] before using it.[br]
		Example:
		[example]
			class (hello,widget)
			{
				constructor()
				{
					$$->%sintbl[]= $array( 0, 38, 71, 92, 100, 92, 71, 38,0, -38, -71, -92, -100, -92, -71, -38);
					$$->%index=0
					$$->$starttimer( 30 );
					$$->$resize(800,600 );
					$$->%string=$0
					$$->%waitfor=1;
					$$->%nextanim=0
				[comment]#zoom and rotation anim[/comment]
				$$->%Zoomindex=11
				$$->%degree=0
				$$->%Noanim=0
				$$->%scrollright=-450

				[comment]#anim effect init[/comment]
				$$->%xoffset=4
				$$->%yoffset=3
				$$->%xstart=270
				$$->%ystart=200
				$$->%b=0
				$$->%yoffs=400

				[comment]#parallax parameter[/comment]
				$$->%Off=400
				$$->%roll=1
			}
			timerevent()
			{
				$$->%b = $(($$->%b + 1) & 15);
				if ($$->%nextanim == 1)  $$->$repaint(1);
				$$->$repaint(0);
			}
			drawAnim()
			{
				%P->$setFont(32,"times",bold);
				%w=$(%P->$fontMetricsWidth($$->%string[$$->%index]) + 20);
				%h=$(%P->$fontMetricsHeight * 2);
				%pmx = $(($$->$width/2) -%w/2);
				%pmy = $(($$->$height()/2) - %h/2);
				%x = 10;
				%y= $((%h/2) + $$->$fontDescent());
				%i=0
				while ( $str.mid("Grifisx/Noldor",%i,1) != "")
				{
					%i16 = $(($$->%b+%i) & 15);
					%char=$str.mid("Grifisx/Noldor",%i,1)
					%P->$setPen($((15-%i16)*16),$((15-%i16)*16),$((15-%i16)*16) );
					%P->$drawText( $(%x+$$->%xstart),$($$->%ystart+%y-$$->%sintbl[%i16]*%h/800),%char,1,Auto);
					%x += %P->$fontMetricsWidth(%char);
					%i++;
				}
			}
			matrixeffect()
			{
				if (($$->%Zoomindex == 99) && ($$->%degree==360)) return %P->$drawPixmap($(400-32),$(300-32),"kvirc.png",0,0,-1,-1)
				%P->$scale(0.$$->%Zoomindex,0.$$->%Zoomindex)
				if ($$->%Zoomindex != 99) $$->%Zoomindex++;
				%P->$rotate($$->%degree)
				%P->$translate(400,300)
				%P->$drawPixmap(-32,-32,"kvirc.png",0,0,-1,-1)
				%P->$setFont(28,"times",bold);
				%P->$reset()
				if ($$->%scrollright >= 550) return
				%P->$scale(0.$$->%Zoomindex,0.$$->%Zoomindex)
				%P->$translate(400,350)
				%P->$drawText($$->%scrollright,10,"Another cool class brought to you by...",-1,Auto)
				$$->%scrollright += 3;
				%P->$reset()
			}
			nextanim()
			{
				%p=$new(painter)
				%p->$setBackgroundMode(Opaque)
				%p->$setBrush($rand(255),$rand(255),$rand(255))
				%p->$begin($$)
				%rand=$rand(5)
				%p->$drawrect($rand(800),$rand(400),120,200)
				%p->$drawArc($rand(800),$rand(400),120,200,20,$(16*20))
				%p->$drawPie($rand(800),$rand(400),120,200,20,$(16*20))
				%p->$drawChord($rand(800),$rand(400),120,200,20,$(16*20))
				%p->$drawEllipse($rand(800),$rand(400),100,30)
				%p->$end()
				delete %p
			}
			paintEvent()
			{
				if ($$->%nextanim ==1) return $$->$nextanim()
				[comment]# pixmap creation: every effect will be painted on it then copied on widget[/comment]
				%pixmap=$new(pixmap)
				%pixmap->$resize($$->$width(),$$->$height())

				[comment]# painter creation[/comment]
				%P=$new(painter);
				%P->$begin(%pixmap);
				$$->$drawanim
				$$->$matrixeffect
				%i=0
				while (%i != 100)
				{
					%i16 = $(($$->%b+%i) & 15);
					%P->$setPen($((15-%i16)*16),$((15-%i16)*16),$((15-%i16)*16) );
					%P->$drawpoint($rand(800),$rand(600))
					%i++
				}

				[comment]# sets the animations order to manage the parallax effect[/comment]
				%P->$end
				objects.bitBlt $$ 0 0 %pixmap
				delete %pixmap
				delete %P
				if (%Pauseflag == 1) return
				[comment]# manage the animations parameters[/comment]
				if (($$->%Off<=60) && ($$->%roll<182)) $$->%roll += 2;
				if ($$->%roll>182) $$->%waitfor=0
				if ($$->%Noanim != 1) $$->%degree += 16;
				if ($$->%degree >= 360)
				{
					$$->%degree=0;
					if ($$->%Zoomindex == 99) $$->%Noanim=1
				}
				if ($$->%Noanim != 1) return
				[comment]#sinusoid animation[/comment]
				if (($$->%xstart <1) && ($$->%xoffset == -4)) $$->%xoffset=4;
				if (($$->%xstart >$($$->$width()-%P->$fontMetricsWidth("Grifisx/Noldor"))) && ($$->%xoffset == 4)) $$->%xoffset=-4;
				if (($$->%ystart <1) && ($$->%yoffset == -3)) $$->%yoffset=3;
				if (($$->%ystart >$($$->$height()-60)) && ($$->%yoffset == 3))   $$->%yoffset=-3;
				$$->%xstart += $$->%xoffset;
				$$->%ystart += $$->%yoffset;
			}
		}

			%Hello=$new(hello)
			%Hello->$setWindowTitle("Painter effects" );
			%Hello->$setFont(28,"times",bold);
			%Btn=$new(button,%Hello)
			%Btn->$setmaximumwidth(80)
			%Btn->$setmaximumheight(30)
			%Btn->$setFont(8,"times",bold);
			%Btn->$settext(Next)

			privateimpl(%Btn,mousepressevent)
			{
				if ($$->$parent->%nextanim==0)
				{
					$$->$parent->%nextanim=1
					$$->$settext(Prev)
				}
				else
				{
					$$->$parent->%nextanim=0
					$$->$settext(Next)
				}
			}

			%lay=$new(layout,%Hello)
			%lay->$addwidget(%Btn,4,0)
			%Hello->$setBackgroundColor("000000");
			%Hello->$setmaximumwidth(800)
			%Hello->$setminimumwidth(780)
			%Hello->$setmaximumheight(600)
			%Hello->$setminimumheight(600)
			%Hello->$move(10,10)
			%Hello->$show();
		[/example]
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_painter, "painter", "object")

// Fonts
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, setFont)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, setFontSize)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, fontAscent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, fontDescent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, fontMetricsHeight)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, fontMetricsWidth)

// Draws
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, drawRect)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, drawLine)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, drawRoundRect)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, drawPoint)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, drawArc)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, drawEllipse)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, drawPie)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, drawChord)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, fillRect)

// Text & Pixmap & Icons
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, drawText)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, drawPixmap)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, drawPixmapMirrored)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, drawPixmapWithColorEffect)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, drawIcon)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, drawHtmlText)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, htmlTextSize)

// Transform Operation
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, rotate)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, shear)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, scale)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, translate)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, reset)

// ??
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, setOpacity)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, setTextAntialiasing)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, setAntialiasing)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, setSmoothPixmapTransform)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, setBackGroundMode)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, setCompositionMode)

// QPainterPath stuff
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, drawPath)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, resetPath)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, pathAddText)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, pathMoveTo)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, pathLineTo)

// Pen
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, setPenJoinStyle)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, setPenCapStyle)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, setPenWidth)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, setPenStyle)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, setPen)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, pen)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, colorNames)

// Brush
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, setBrush)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, setBrushStyle)

// Gradients
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, setGradientStart)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, setGradientStop)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, setGradientColor)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, setGradientAsBrush)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, clearGradient)

// painter state
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, save)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, restore)

// painter device
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, painterDeviceWidth)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, painterDeviceHeight)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, begin)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, end)

// extra
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_painter, beginPdf)

KVSO_END_REGISTERCLASS(KvsObject_painter)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_painter, KviKvsObject)

m_pPainter = new QPainter();
bDonotdeleteinternalqpainter = false;
m_pPrinter = nullptr;
m_pDeviceObject = nullptr;
m_pPainterPath = nullptr;
m_pGradient = nullptr;

KVSO_END_CONSTRUCTOR(KvsObject_painter)

KVSO_BEGIN_DESTRUCTOR(KvsObject_painter)

if(m_pPainterPath)
	delete m_pPainterPath;
m_pPainterPath = nullptr;
if(m_pGradient)
	delete m_pGradient;
m_pGradient = nullptr;
if(m_pPainter && !bDonotdeleteinternalqpainter)
	delete m_pPainter;
m_pPainter = nullptr;
if(m_pPrinter)
	delete m_pPrinter;
m_pPrinter = nullptr;

KVSO_END_CONSTRUCTOR(KvsObject_painter)

#define KVSO_PARAMETERS_PAINTER(__pXOrArray, __iY, __iW, __iH)                                                                  \
	if(__pXOrArray->isArray())                                                                                                  \
	{                                                                                                                           \
		if(__pXOrArray->array()->size() < 4)                                                                                    \
		{                                                                                                                       \
			c->error(__tr2qs_ctx("The array passed as parameter must contain at least 4 elements", "objects"));                 \
			return false;                                                                                                       \
		}                                                                                                                       \
		KviKvsVariant * pX = __pXOrArray->array()->at(0);                                                                       \
		KviKvsVariant * pY = __pXOrArray->array()->at(1);                                                                       \
		KviKvsVariant * pW = __pXOrArray->array()->at(2);                                                                       \
		KviKvsVariant * pH = __pXOrArray->array()->at(3);                                                                       \
		if(!(pX && pY && pW && pH))                                                                                             \
		{                                                                                                                       \
			c->error(__tr2qs_ctx("One of the geometry array parameters is empty", "objects"));                                  \
			return false;                                                                                                       \
		}                                                                                                                       \
		if(!(pX->asInteger(iX) && pY->asInteger(__iY) && pW->asInteger(__iW) && pH->asInteger(__iH)))                           \
		{                                                                                                                       \
			c->error(__tr2qs_ctx("One of the geometry array parameters didn't evaluate to an integer", "objects"));             \
			return false;                                                                                                       \
		}                                                                                                                       \
	}                                                                                                                           \
	else                                                                                                                        \
	{                                                                                                                           \
		if(c->params()->count() < 4)                                                                                            \
		{                                                                                                                       \
			QString error = function + __tr2qs_ctx(" requires either an array as first parameter or four integers", "objects"); \
			c->error(error);                                                                                                    \
			return false;                                                                                                       \
		}                                                                                                                       \
		if(!__pXOrArray->asInteger(iX))                                                                                         \
		{                                                                                                                       \
			c->error(__tr2qs_ctx("The first parameter didn't evaluate to an array nor an integer", "objects"));                 \
			return false;                                                                                                       \
		}                                                                                                                       \
	}

#define KVSO_2ARRAYPARAMETERS(__pXOrArray, __iY)                                                                               \
	if(__pXOrArray->isArray())                                                                                                 \
	{                                                                                                                          \
		if(__pXOrArray->array()->size() < 2)                                                                                   \
		{                                                                                                                      \
			c->error(__tr2qs_ctx("The array passed as parameter must contain at least 2 elements", "objects"));                \
			return false;                                                                                                      \
		}                                                                                                                      \
		KviKvsVariant * pX = __pXOrArray->array()->at(0);                                                                      \
		KviKvsVariant * pY = __pXOrArray->array()->at(1);                                                                      \
		if(!(pX && pY))                                                                                                        \
		{                                                                                                                      \
			c->error(__tr2qs_ctx("One of the array parameters is empty", "objects"));                                          \
			return false;                                                                                                      \
		}                                                                                                                      \
		if(!(pX->asInteger(iX) && pY->asInteger(__iY)))                                                                        \
		{                                                                                                                      \
			c->error(__tr2qs_ctx("One of the array parameters didn't evaluate to an integer", "objects"));                     \
			return false;                                                                                                      \
		}                                                                                                                      \
	}                                                                                                                          \
	else                                                                                                                       \
	{                                                                                                                          \
		if(c->params()->count() < 2)                                                                                           \
		{                                                                                                                      \
			QString error = function + __tr2qs_ctx(" requires either an array as first parameter or two integers", "objects"); \
			c->error(error);                                                                                                   \
			return false;                                                                                                      \
		}                                                                                                                      \
		if(!__pXOrArray->asInteger(iX))                                                                                        \
		{                                                                                                                      \
			c->error(__tr2qs_ctx("The first parameter didn't evaluate to an array nor an integer", "objects"));                \
			return false;                                                                                                      \
		}                                                                                                                      \
	}

#define KVSO_3OR4ARRAYPARAMETERS(__pCol1OrArray, __iCol2, __iCol3, __iOpacity)                                               \
	if(__pCol1OrArray->isArray())                                                                                            \
	{                                                                                                                        \
		if(__pCol1OrArray->array()->size() < 3)                                                                              \
		{                                                                                                                    \
			c->error(__tr2qs_ctx("The array passed as parameter must contain at least 3 elements", "objects"));              \
			return false;                                                                                                    \
		}                                                                                                                    \
		KviKvsVariant * c1 = __pCol1OrArray->array()->at(0);                                                                 \
		KviKvsVariant * c2 = __pCol1OrArray->array()->at(1);                                                                 \
		KviKvsVariant * c3 = __pCol1OrArray->array()->at(2);                                                                 \
		if(!(c1 && c2 && c3))                                                                                                \
		{                                                                                                                    \
			c->error(__tr2qs_ctx("One of the array parameters is empty", "objects"));                                        \
			return false;                                                                                                    \
		}                                                                                                                    \
		if(!(c1->asInteger(iCol1) && c2->asInteger(__iCol2) && c3->asInteger(__iCol3)))                                      \
		{                                                                                                                    \
			c->error(__tr2qs_ctx("One of the array parameters didn't evaluate to an integer", "objects"));                   \
			return false;                                                                                                    \
		}                                                                                                                    \
		if(__pCol1OrArray->array()->size() > 3)                                                                              \
		{                                                                                                                    \
			KviKvsVariant * c4 = __pCol1OrArray->array()->at(3);                                                             \
			if(!(c4->asInteger(__iOpacity)))                                                                                 \
			{                                                                                                                \
				c->error(__tr2qs_ctx("The opacity value didn't evaluate to an integer", "objects"));                         \
				return false;                                                                                                \
			}                                                                                                                \
		}                                                                                                                    \
	}                                                                                                                        \
	else                                                                                                                     \
	{                                                                                                                        \
		if(c->params()->count() < 3)                                                                                         \
		{                                                                                                                    \
			QString error = function + __tr2qs_ctx(" requires either an array as first parameter or 3 integers", "objects"); \
			c->error(error);                                                                                                 \
			return false;                                                                                                    \
		}                                                                                                                    \
		if(!__pCol1OrArray->asInteger(iCol1))                                                                                \
		{                                                                                                                    \
			c->error(__tr2qs_ctx("The first parameter didn't evaluate to an array nor an integer", "objects"));              \
			return false;                                                                                                    \
		}                                                                                                                    \
	}

KVSO_CLASS_FUNCTION(painter, setFont)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szFamily;
	QStringList szListStyle;
	kvs_int_t iSize;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("size", KVS_PT_INTEGER, 0, iSize)
	KVSO_PARAMETER("family", KVS_PT_STRING, 0, szFamily)
	KVSO_PARAMETER("style", KVS_PT_STRINGLIST, KVS_PF_OPTIONAL, szListStyle)
	KVSO_PARAMETERS_END(c)
	QFont font = m_pPainter->font();
	font.setFamily(szFamily);
	font.setPointSize(iSize);
	QString szStyle;
	for(int i = 0; i < szListStyle.length(); i++)
	{
		szStyle = szListStyle.at(i);
		if(KviQString::equalCI(szStyle, "italic"))
			font.setItalic(true);
		else if(KviQString::equalCI(szStyle, "bold"))
			font.setBold(true);
		else if(KviQString::equalCI(szStyle, "underline"))
			font.setUnderline(true);
		else if(KviQString::equalCI(szStyle, "overline"))
			font.setOverline(true);
		else if(KviQString::equalCI(szStyle, "strikeout"))
			font.setStrikeOut(true);
		else if(KviQString::equalCI(szStyle, "fixedpitch"))
			font.setFixedPitch(true);
		else
			c->warning(__tr2qs_ctx("Unknown style '%Q'", "objects"), &szStyle);
	}
	m_pPainter->setFont(font);
	return true;
}

KVSO_CLASS_FUNCTION(painter, setFontSize)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_uint_t uSize;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("size", KVS_PT_UNSIGNEDINTEGER, 0, uSize)
	KVSO_PARAMETERS_END(c)
	QFont font = m_pPainter->font();
	font.setPointSize(uSize);
	m_pPainter->setFont(font);
	return true;
}

KVSO_CLASS_FUNCTION(painter, setBackGroundMode)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szMode;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("background mode", KVS_PT_STRING, 0, szMode)
	KVSO_PARAMETERS_END(c)
	if(KviQString::equalCI(szMode, "Transparent"))
		m_pPainter->setBackgroundMode(Qt::TransparentMode);
	else if(KviQString::equalCI(szMode, "Opaque"))
		m_pPainter->setBackgroundMode(Qt::OpaqueMode);
	else
		c->warning(__tr2qs_ctx("Unknown '%Q' background mode", "objects"), &szMode);
	return true;
}

KVSO_CLASS_FUNCTION(painter, drawLine)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	KviKvsVariant * pXOrArray;
	kvs_int_t iX, iY, iX1, iY1;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x_or_array", KVS_PT_VARIANT, 0, pXOrArray)
	KVSO_PARAMETER("y", KVS_PT_INT, KVS_PF_OPTIONAL, iY)
	KVSO_PARAMETER("x1", KVS_PT_INT, KVS_PF_OPTIONAL, iX1)
	KVSO_PARAMETER("y1", KVS_PT_INT, KVS_PF_OPTIONAL, iY1)
	KVSO_PARAMETERS_END(c)
	QString function = "$drawLine";
	KVSO_PARAMETERS_PAINTER(pXOrArray, iY, iX1, iY1)
	if(m_pPainter)
		m_pPainter->drawLine(iX, iY, iX1, iY1);
	return true;
}

KVSO_CLASS_FUNCTION(painter, drawRect)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	KviKvsVariant * pXOrArray;
	kvs_int_t iX, iY, iW, iH;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x_or_array", KVS_PT_VARIANT, 0, pXOrArray)
	KVSO_PARAMETER("y", KVS_PT_INT, KVS_PF_OPTIONAL, iY)
	KVSO_PARAMETER("w", KVS_PT_INT, KVS_PF_OPTIONAL, iW)
	KVSO_PARAMETER("h", KVS_PT_INT, KVS_PF_OPTIONAL, iH)
	KVSO_PARAMETERS_END(c)
	QString function = "$drawRect";
	KVSO_PARAMETERS_PAINTER(pXOrArray, iY, iW, iH)
	m_pPainter->drawRect(iX, iY, iW, iH);
	return true;
}

KVSO_CLASS_FUNCTION(painter, drawEllipse)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	KviKvsVariant * pXOrArray;
	kvs_int_t iX, iY, iW, iH;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x_or_array", KVS_PT_VARIANT, 0, pXOrArray)
	KVSO_PARAMETER("y", KVS_PT_INT, KVS_PF_OPTIONAL, iY)
	KVSO_PARAMETER("w", KVS_PT_INT, KVS_PF_OPTIONAL, iW)
	KVSO_PARAMETER("h", KVS_PT_INT, KVS_PF_OPTIONAL, iH)
	KVSO_PARAMETERS_END(c)
	QString function = "$drawEllipse";
	KVSO_PARAMETERS_PAINTER(pXOrArray, iY, iW, iH)
	m_pPainter->drawEllipse(iX, iY, iW, iH);
	return true;
}

KVSO_CLASS_FUNCTION(painter, drawRoundRect)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	KviKvsVariant * pXOrArray;
	kvs_int_t iXrnd, iYrnd, iX, iY, iW, iH;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x_round", KVS_PT_INT, 0, iXrnd)
	KVSO_PARAMETER("y_round", KVS_PT_INT, 0, iYrnd)
	KVSO_PARAMETER("x_or_array", KVS_PT_VARIANT, 0, pXOrArray)
	KVSO_PARAMETER("y", KVS_PT_INT, KVS_PF_OPTIONAL, iY)
	KVSO_PARAMETER("w", KVS_PT_INT, KVS_PF_OPTIONAL, iW)
	KVSO_PARAMETER("h", KVS_PT_INT, KVS_PF_OPTIONAL, iH)
	KVSO_PARAMETERS_END(c)
	QString function = "$drawRoundRect";
	KVSO_PARAMETERS_PAINTER(pXOrArray, iY, iW, iH)
	m_pPainter->drawRoundRect(iX, iY, iW, iH, iXrnd, iYrnd);
	return true;
}

KVSO_CLASS_FUNCTION(painter, drawArc)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	KviKvsVariant * pXOrArray;
	kvs_int_t iSangle, iLena, iX, iY, iW, iH;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("start_angle", KVS_PT_INT, 0, iSangle)
	KVSO_PARAMETER("a_lenght", KVS_PT_INT, 0, iLena)
	KVSO_PARAMETER("x_or_array", KVS_PT_VARIANT, 0, pXOrArray)
	KVSO_PARAMETER("y", KVS_PT_INT, KVS_PF_OPTIONAL, iY)
	KVSO_PARAMETER("w", KVS_PT_INT, KVS_PF_OPTIONAL, iW)
	KVSO_PARAMETER("h", KVS_PT_INT, KVS_PF_OPTIONAL, iH)
	KVSO_PARAMETERS_END(c)
	QString function = "$drawArc";
	KVSO_PARAMETERS_PAINTER(pXOrArray, iY, iW, iH)
	m_pPainter->drawArc(iX, iY, iW, iH, iSangle, iLena);
	return true;
}

KVSO_CLASS_FUNCTION(painter, drawChord)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	KviKvsVariant * pXOrArray;
	kvs_int_t iSangle, iLena, iX, iY, iW, iH;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("start_angle", KVS_PT_INT, 0, iSangle)
	KVSO_PARAMETER("a_lenght", KVS_PT_INT, 0, iLena)
	KVSO_PARAMETER("x_or_array", KVS_PT_VARIANT, 0, pXOrArray)
	KVSO_PARAMETER("y", KVS_PT_INT, KVS_PF_OPTIONAL, iY)
	KVSO_PARAMETER("w", KVS_PT_INT, KVS_PF_OPTIONAL, iW)
	KVSO_PARAMETER("h", KVS_PT_INT, KVS_PF_OPTIONAL, iH)
	KVSO_PARAMETERS_END(c)
	QString function = "$drawChord";
	KVSO_PARAMETERS_PAINTER(pXOrArray, iY, iW, iH)
	m_pPainter->drawChord(iX, iY, iW, iH, iSangle, iLena);
	return true;
}

KVSO_CLASS_FUNCTION(painter, setBrush)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_int_t iCol1, iCol2, iCol3, iOpacity;
	KviKvsVariant *var1, *var2, *var3;
	QString szColorMode, szColor;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("Color_1_Or_Colorname_Or_Pixmap", KVS_PT_VARIANT, 0, var1)
	KVSO_PARAMETER("Color_2", KVS_PT_VARIANT, KVS_PF_OPTIONAL, var2)
	KVSO_PARAMETER("Colo3_3", KVS_PT_VARIANT, KVS_PF_OPTIONAL, var3)
	KVSO_PARAMETER("color_mode", KVS_PT_STRING, KVS_PF_OPTIONAL, szColorMode)
	KVSO_PARAMETER("opacity", KVS_PT_INT, KVS_PF_OPTIONAL, iOpacity)
	KVSO_PARAMETERS_END(c)
	if(var1->isHObject())
	{
		kvs_hobject_t hObj;
		var1->asHObject(hObj);
		KviKvsObject * pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObj);
		if(!pObject)
		{
			c->warning(__tr2qs_ctx("Pixmap parameter is not an object!", "objects"));
			return true;
		}
		if(pObject->inheritsClass("pixmap"))
			m_pPainter->setBrush(*((KvsObject_pixmap *)pObject)->getImage());
		else
			c->warning(__tr2qs_ctx("Object Pixmap required!", "objects"));
		return true;
	}
	if(!var1->asInteger(iCol1))
	{
		var1->asString(szColor);
		if(c->paramCount() < 2)
			iOpacity = 255;
		else
		{
			if(!var2->asInteger(iOpacity))
			{
				c->warning(__tr2qs_ctx("The opacity parameter didn't evaluate to integer", "objects"));
				return true;
			}
		}
		QColor col(szColor);
		col.setAlpha(iOpacity);
		m_pPainter->setBrush(col);
	}
	else
	{
		if(c->paramCount() < 3)
		{
			c->error(__tr2qs_ctx("Color name or triplet RGB/HSV value required", "objects"));
			return true;
		}
		if(!var2->asInteger(iCol2) || !var3->asInteger(iCol3))
		{
			c->error(__tr2qs_ctx("One of the triplet parameters didn't evaluate to an integer", "objects"));
			return true;
		}
		if(c->paramCount() < 5)
			iOpacity = 255;
		QColor col;
		if(KviQString::equalCI(szColorMode, "HSV"))
			col.setHsv(iCol1, iCol2, iCol3, iOpacity);
		else
			col.setRgb(iCol1, iCol2, iCol3, iOpacity);
		m_pPainter->setBrush(col);
	}
	return true;
}
KVSO_CLASS_FUNCTION(painter, pen)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szFlags;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("flags", KVS_PT_STRING, KVS_PF_OPTIONAL, szFlags)
	KVSO_PARAMETERS_END(c)
	if(szFlags.isEmpty())
	{
		c->returnValue()->setString(m_pPainter->pen().color().name());
		return true;
	}
	QColor col = m_pPainter->pen().color();
	if(szFlags.indexOf('a', 0, Qt::CaseInsensitive) != -1)
	{
		KviKvsArray * pArray = new KviKvsArray();
		pArray->set(0, new KviKvsVariant((kvs_int_t)col.red()));
		pArray->set(1, new KviKvsVariant((kvs_int_t)col.green()));
		pArray->set(2, new KviKvsVariant((kvs_int_t)col.blue()));
		c->returnValue()->setArray(pArray);
	}
	else if(szFlags.indexOf('h', 0, Qt::CaseInsensitive) != -1)
	{
		KviKvsHash * pHash = new KviKvsHash();
		pHash->set("red", new KviKvsVariant((kvs_int_t)col.red()));
		pHash->set("green", new KviKvsVariant((kvs_int_t)col.green()));
		pHash->set("blue", new KviKvsVariant((kvs_int_t)col.blue()));
		c->returnValue()->setHash(pHash);
	}
	return true;
}
KVSO_CLASS_FUNCTION(painter, setPen)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_int_t iCol1, iCol2, iCol3, iOpacity;
	QString szColorMode, szColor;
	KviKvsVariant *var1, *var2, *var3;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("Color_1_Or_Colorname", KVS_PT_VARIANT, 0, var1)
	KVSO_PARAMETER("Color_2", KVS_PT_VARIANT, KVS_PF_OPTIONAL, var2)
	KVSO_PARAMETER("Colo3_3", KVS_PT_VARIANT, KVS_PF_OPTIONAL, var3)
	KVSO_PARAMETER("color_mode", KVS_PT_STRING, KVS_PF_OPTIONAL, szColorMode)
	KVSO_PARAMETER("opacity", KVS_PT_INT, KVS_PF_OPTIONAL, iOpacity)
	KVSO_PARAMETERS_END(c)
	if(!var1->asInteger(iCol1))
	{
		var1->asString(szColor);
		if(c->paramCount() < 2)
			iOpacity = 255;
		else
		{
			if(!var2->asInteger(iOpacity))
			{
				c->warning(__tr2qs_ctx("The opacity parameter didn't evaluate to integer", "objects"));
				return true;
			}
		}
		QColor col(szColor);
		col.setAlpha(iOpacity);
		m_pPainter->setPen(col);
	}
	else
	{
		if(c->paramCount() < 3)
		{
			c->error(__tr2qs_ctx("Color name or triplet RGB/HSV value required", "objects"));
			return true;
		}
		if(!var2->asInteger(iCol2) || !var3->asInteger(iCol3))
		{
			c->error(__tr2qs_ctx("One of the triplet parameters didn't evaluate to an integer", "objects"));
			return true;
		}
		if(c->paramCount() < 5)
			iOpacity = 255;
		QColor col;
		if(KviQString::equalCI(szColorMode, "HSV"))
			col.setHsv(iCol1, iCol2, iCol3, iOpacity);
		else
			col.setRgb(iCol1, iCol2, iCol3, iOpacity);
		m_pPainter->setPen(col);
	}

	return true;
}

KVSO_CLASS_FUNCTION(painter, drawPie)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	KviKvsVariant * pXOrArray;
	kvs_int_t iSangle, iLena, iX, iY, iW, iH;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("start_angle", KVS_PT_INT, 0, iSangle)
	KVSO_PARAMETER("a_lenght", KVS_PT_INT, 0, iLena)
	KVSO_PARAMETER("x_or_array", KVS_PT_VARIANT, 0, pXOrArray)
	KVSO_PARAMETER("y", KVS_PT_INT, KVS_PF_OPTIONAL, iY)
	KVSO_PARAMETER("w", KVS_PT_INT, KVS_PF_OPTIONAL, iW)
	KVSO_PARAMETER("h", KVS_PT_INT, KVS_PF_OPTIONAL, iH)
	KVSO_PARAMETERS_END(c)
	QString function = "$drawPie";
	KVSO_PARAMETERS_PAINTER(pXOrArray, iY, iW, iH)
	m_pPainter->drawPie(iX, iY, iW, iH, iSangle, iLena);
	return true;
}

KVSO_CLASS_FUNCTION(painter, drawPoint)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	KviKvsVariant * pXOrArray;
	kvs_int_t iX, iY;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x_or_array", KVS_PT_VARIANT, 0, pXOrArray)
	KVSO_PARAMETER("y", KVS_PT_INT, KVS_PF_OPTIONAL, iY)
	KVSO_PARAMETERS_END(c)
	QString function = "$drawPoint";
	KVSO_2ARRAYPARAMETERS(pXOrArray, iY)
	m_pPainter->drawPoint(iX, iY);
	return true;
}

KVSO_CLASS_FUNCTION(painter, fontDescent)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	c->returnValue()->setInteger(m_pPainter->fontMetrics().descent());
	return true;
}

KVSO_CLASS_FUNCTION(painter, fontAscent)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	c->returnValue()->setInteger(m_pPainter->fontMetrics().ascent());
	return true;
}

KVSO_CLASS_FUNCTION(painter, fontMetricsWidth)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("text", KVS_PT_STRING, 0, szText)
	KVSO_PARAMETERS_END(c)
	if(m_pPainter->isActive())
		c->returnValue()->setInteger(m_pPainter->fontMetrics().width(szText));
	else
		c->warning(__tr2qs_ctx("$fontMetricsWidth: the painter is not active!", "objects"));
	return true;
}

KVSO_CLASS_FUNCTION(painter, fontMetricsHeight)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	if(m_pPainter->isActive())
		c->returnValue()->setInteger(m_pPainter->fontMetrics().height());
	else
		c->warning(__tr2qs_ctx("$fontMetricsHeight: the painter is not active!", "objects"));
	return true;
}

KVSO_CLASS_FUNCTION(painter, begin)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	KviKvsObject * pObject;
	kvs_hobject_t hObject;
	KviKvsVariant * v;
	QString szFileName;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("paint_device", KVS_PT_VARIANT, 0, v)
	KVSO_PARAMETER("file_name", KVS_PT_STRING, KVS_PF_OPTIONAL, szFileName)
	KVSO_PARAMETERS_END(c)
	QPaintDevice * pd;
	if(v->isString())
	{
		QString szDev;
		v->asString(szDev);
		if(KviQString::equalCI(szDev, "printer") || KviQString::equalCI(szDev, "pdf"))
		{
			if(m_pPrinter)
				delete m_pPrinter;
			m_pPrinter = new QPrinter();
			if(KviQString::equalCI(szDev, "pdf"))
			{
				//m_pPrinter=new QPrinter();
				if(szFileName.isEmpty())
				{
					c->warning(__tr2qs_ctx("Missing filename", "objects"));
					return true;
				}
				m_pPrinter->setOutputFormat(QPrinter::PdfFormat);
				m_pPrinter->setOutputFileName(szFileName);
			}
			else
			{
				QPrintDialog printDialog(m_pPrinter, nullptr);
				if(printDialog.exec() == QDialog::Accepted)
				{
					qDebug("papersize %d", m_pPrinter->paperSize());
					m_pPainter->begin(m_pPrinter);
					return true;
				}
				else
				{
					m_pPrinter = nullptr;
					return true;
				}
			}
			//if (i==3) m_pPrinter->setPaperSize(QPrinter::A3);
			//if (i==4) m_pPrinter->setPaperSize(QPrinter::A4);
		}
		else
		{
			c->warning(__tr2qs_ctx("No valid paint device", "objects"));
			return true;
		}
		m_pPainter->begin(m_pPrinter);
		return true;
	}
	else if(v->isHObject())
	{
		v->asHObject(hObject);
		pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
		if(!pObject)
		{
			c->warning(__tr2qs_ctx("Pixmap or Widget parameter is not an object", "objects"));
			return true;
		}
		if(pObject->inheritsClass("pixmap"))
			pd = ((KvsObject_pixmap *)pObject)->getImage();
		else if(pObject->inheritsClass("widget"))
			pd = ((KvsObject_widget *)pObject)->widget();
		else
		{
			c->warning(__tr2qs_ctx("Widget or pixmap required ", "objects"));
			return true;
		}
	}
	else
	{
		c->warning(__tr2qs_ctx("No valid paint device", "objects"));
		return true;
	}
	attachDevice(pObject, pd);
	//if (pObject->inheritsClass("pixmap")) ((KvsObject_pixmap *)pObject)->pixmapChanged();
	return true;
}

void KvsObject_painter::attachDevice(KviKvsObject * o, QPaintDevice * p)
{
	if(m_pDeviceObject)
		detachDevice();
	m_pDeviceObject = o;
	// KvsObject_pixmap and KvsObject_widget object have this signal
	// it is emitted BEFORE the real QPaintDevice is deleted, so we can eventually
	// call m_pPainter->end() in time
	QObject::connect(m_pDeviceObject, SIGNAL(aboutToDie()), this, SLOT(detachDevice()));
	m_pPainter->begin(p);
}

void KvsObject_painter::detachDevice()
{
	if(!m_pDeviceObject)
		return;
	disconnect(m_pDeviceObject, SIGNAL(aboutToDie()), this, SLOT(detachDevice()));
	m_pPainter->end();
	if(m_pPrinter)
	{
		delete m_pPrinter;
		m_pPrinter = nullptr;
	}
	m_pDeviceObject = nullptr;
}

KVSO_CLASS_FUNCTION(painter, end)
{
	if(!m_pDeviceObject)
	{
		m_pPainter->end();
		if(m_pPrinter)
		{
			delete m_pPrinter;
			m_pPrinter = nullptr;
		}
	}
	else
		detachDevice();
	return true;
}

KVSO_CLASS_FUNCTION(painter, beginPdf)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szFileName;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("file_name", KVS_PT_STRING, 0, szFileName)
	KVSO_PARAMETERS_END(c)
	if(m_pPrinter)
		delete m_pPrinter;
	m_pPrinter = new QPrinter(); //QPrinter::HighResolution);
	m_pPrinter->setOutputFormat(QPrinter::PdfFormat);
	m_pPrinter->setOutputFileName(szFileName);
	m_pPainter->begin(m_pPrinter);
	return true;
}

KVSO_CLASS_FUNCTION(painter, drawText)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szText;
	QStringList szAlignList;
	kvs_int_t iX, iY, iW, iH; //,iN=-1;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x", KVS_PT_INT, 0, iX)
	KVSO_PARAMETER("y", KVS_PT_INT, 0, iY)
	KVSO_PARAMETER("w", KVS_PT_INT, 0, iW)
	KVSO_PARAMETER("h", KVS_PT_INT, 0, iH)
	KVSO_PARAMETER("text", KVS_PT_STRING, 0, szText)
	KVSO_PARAMETER("align", KVS_PT_STRINGLIST, KVS_PF_OPTIONAL, szAlignList)
	KVSO_PARAMETERS_END(c)
	int align, sum = 0;
	if(szAlignList.count())
	{
		for(auto & it : szAlignList)
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
	}
	QRect rect;
	rect.setX(iX);
	rect.setY(iY);
	rect.setWidth(iW);
	rect.setHeight(iH);
	if(szAlignList.count())
	{
		m_pPainter->drawText(rect, sum, szText);
	}
	else
		m_pPainter->drawText(rect, szText);
	return true;
}

KVSO_CLASS_FUNCTION(painter, drawPixmap)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t iX, iY, iWidth, iHeight, iStartx, iStarty;
	bool bTiled;
	KviKvsObject * obj;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x", KVS_PT_REAL, 0, iX)
	KVSO_PARAMETER("y", KVS_PT_REAL, 0, iY)
	KVSO_PARAMETER("pixmap", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETER("start_x", KVS_PT_REAL, 0, iStartx)
	KVSO_PARAMETER("start_y", KVS_PT_REAL, 0, iStarty)
	KVSO_PARAMETER("end_x", KVS_PT_REAL, 0, iWidth)
	KVSO_PARAMETER("end_y", KVS_PT_REAL, 0, iHeight)
	KVSO_PARAMETER("b_Tiled", KVS_PT_BOOLEAN, KVS_PF_OPTIONAL, bTiled)
	KVSO_PARAMETERS_END(c)
	obj = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!obj)
	{
		c->warning(__tr2qs_ctx("Pixmap parameter is not an object", "objects"));
		return true;
	}
	if(!obj->inheritsClass("pixmap"))
	{
		c->warning(__tr2qs_ctx("Pixmap object required", "objects"));
		return true;
	}
	if(bTiled)
	{
		QPixmap * pPixmap = ((KvsObject_pixmap *)obj)->getPixmap();
		//QPoint point(iX,iY);

		//QRectF rectf(QPointF(iStartx,iStarty),QSizeF(iWidth,iHeight));
		//m_pPainter->drawTiledPixmap(rectf,*pPixmap,pointf);
		//m_pPainter->drawTiledPixmap((int)iX,(int)iY,(int)iWidth,(int)iHeight,*pPixmap,(int)iStartx,(int)iStarty);
		qDebug("drawing at x %i -  y %i - w %i - h %i - startpx %i - startpy %i casted", (int)iX, (int)iY, (int)iWidth, (int)iHeight, (int)iStartx, (int)iStarty);
		m_pPainter->drawTiledPixmap(iX, iY, iWidth, iHeight, *pPixmap, iStartx, iStarty);
	}
	else
	{
		QImage * pImage = ((KvsObject_pixmap *)obj)->getImage();
		QPoint pointf(iX, iY);
		QRectF rectf(QPointF(iStartx, iStarty), QSizeF(iWidth, iHeight));
		m_pPainter->drawImage(pointf, *pImage, rectf);
	}
	return true;
}

KVSO_CLASS_FUNCTION(painter, drawPixmapWithColorEffect)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szEffect;
	kvs_real_t rX, rY;
	kvs_int_t r, g, b, iX, iY, iW, iH;
	KviKvsObject * obj;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("string_effect", KVS_PT_STRING, 0, szEffect)
	KVSO_PARAMETER("x", KVS_PT_REAL, 0, rX)
	KVSO_PARAMETER("y", KVS_PT_REAL, 0, rY)
	KVSO_PARAMETER("pixmap", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETER("red", KVS_PT_INT, KVS_PF_OPTIONAL, r)
	KVSO_PARAMETER("green", KVS_PT_INT, KVS_PF_OPTIONAL, g)
	KVSO_PARAMETER("blue", KVS_PT_INT, KVS_PF_OPTIONAL, b)
	KVSO_PARAMETER("start_x", KVS_PT_INT, KVS_PF_OPTIONAL, iX)
	KVSO_PARAMETER("start_y", KVS_PT_INT, KVS_PF_OPTIONAL, iY)
	KVSO_PARAMETER("width", KVS_PT_INT, KVS_PF_OPTIONAL, iW)
	KVSO_PARAMETER("height", KVS_PT_INT, KVS_PF_OPTIONAL, iH)
	KVSO_PARAMETERS_END(c)
	obj = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!obj)
	{
		c->warning(__tr2qs_ctx("Pixmap parameter is not an object", "objects"));
		return true;
	}
	if(!obj->inheritsClass("pixmap"))
	{
		c->warning(__tr2qs_ctx("Pixmap object required", "objects"));
		return true;
	}
	QImage * pImage = ((KvsObject_pixmap *)obj)->getImage();
	//QImage pDest(pImage->width(),pImage->height(),QImage::Format_ARGB32);
	QRgb col;
	QRgb * dataSource;
	QRgb * dataDest;
	int w = pImage->width();
	int h = pImage->height();
	if(!iW)
		iW = w;
	if(!iH)
		iH = h;
	QImage pDest(iW, iH, QImage::Format_ARGB32);

	int iXend = iX + iW;
	int iYend = iY + iH;

	if(KviQString::equalCI(szEffect, "addrgb"))
	{
		int sumR, sumG, sumB, idx = 0;
		for(int y = iY; y < iYend; y++)
		{
			dataSource = (QRgb *)pImage->scanLine(y) + iX;
			dataDest = (QRgb *)pDest.scanLine(idx);
			idx++;
			for(int x = iX; x < iXend; x++)
			{
				col = *dataSource;
				sumR = qRed(col) + r;
				sumG = qGreen(col) + g;
				sumB = qBlue(col) + b;
				*dataDest++ = qRgba(sumR < 255 ? sumR : 255,
				    sumG < 255 ? sumG : 255,
				    sumB < 255 ? sumB : 255,
				    qAlpha(col));
				dataSource++;
			}
		}
	}
	else if(KviQString::equalCI(szEffect, "subrgb"))
	{
		int subR, subG, subB, idx = 0;
		for(int y = iY; y < iYend; y++)
		{
			dataSource = (QRgb *)pImage->scanLine(y) + iX;
			dataDest = (QRgb *)pDest.scanLine(idx);
			idx++;
			for(int x = iX; x < iXend; x++)
			{
				col = *dataSource;
				subR = qRed(col) - r;
				subG = qGreen(col) - g;
				subB = qBlue(col) - b;
				*dataDest++ = qRgba(subR > 0 ? subR : 0,
				    subG > 0 ? subG : 0,
				    subB > 0 ? subB : 0,
				    qAlpha(col));
				dataSource++;
			}
		}
	}
	else if(KviQString::equalCI(szEffect, "grayscale"))
	{
		QRgb col;
		int res, idx = 0;
		for(int y = iY; y < iYend; y++)
		{
			dataSource = (QRgb *)pImage->scanLine(y) + iX;
			dataDest = (QRgb *)pDest.scanLine(idx);
			for(int x = iX; x < iXend; x++)
			{
				col = *dataSource;
				res = (qRed(col) + qGreen(col) + qBlue(col)) / 3;
				*dataDest++ = qRgba(res, res, res, qAlpha(col));
				dataSource++;
			}
			idx++;
		}
	}
	m_pPainter->drawImage(QPointF(rX, rY), pDest);
	return true;
}
KVSO_CLASS_FUNCTION(painter, drawPixmapMirrored)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	bool bHorizontal, bVertical;
	kvs_int_t iX, iY, iW, iH;
	KviKvsObject * obj;
	kvs_hobject_t hObject;
	kvs_real_t rX, rY;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x", KVS_PT_REAL, 0, rX)
	KVSO_PARAMETER("y", KVS_PT_REAL, 0, rY)
	KVSO_PARAMETER("pixmap", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETER("bHorizontal", KVS_PT_BOOLEAN, 0, bHorizontal)
	KVSO_PARAMETER("bVertical", KVS_PT_BOOLEAN, 0, bVertical)
	KVSO_PARAMETER("start_x", KVS_PT_INT, KVS_PF_OPTIONAL, iX)
	KVSO_PARAMETER("start_y", KVS_PT_INT, KVS_PF_OPTIONAL, iY)
	KVSO_PARAMETER("width", KVS_PT_INT, KVS_PF_OPTIONAL, iW)
	KVSO_PARAMETER("height", KVS_PT_INT, KVS_PF_OPTIONAL, iH)
	KVSO_PARAMETERS_END(c)
	obj = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!obj)
	{
		c->warning(__tr2qs_ctx("Pixmap parameter is not an object", "objects"));
		return true;
	}
	if(!obj->inheritsClass("pixmap"))
	{
		c->warning(__tr2qs_ctx("Pixmap object required", "objects"));
		return true;
	}
	QImage * pImage = ((KvsObject_pixmap *)obj)->getImage();
	QPoint sourceXY(0, 0);
	if(c->parameterCount() >= 5)
	{
		sourceXY.setX(iX);
		sourceXY.setY(iY);
	}
	QSize sourceWH(pImage->width(), pImage->height());
	if(c->parameterCount() >= 5)
	{
		sourceWH.setWidth(iW);
		sourceWH.setHeight(iH);
	}
	QImage mirrored = pImage->mirrored(bHorizontal, bVertical);
	m_pPainter->drawImage(rX, rY, mirrored, sourceXY.x(), sourceXY.y(), sourceWH.width(), sourceWH.height());
	return true;
}
KVSO_CLASS_FUNCTION(painter, rotate)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t dAngle;
	QString szAxis;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("angle", KVS_PT_DOUBLE, 0, dAngle)
	KVSO_PARAMETER("axis", KVS_PT_STRING, KVS_PF_OPTIONAL, szAxis)
	KVSO_PARAMETERS_END(c)
	Qt::Axis axis = Qt::ZAxis;
	if(!szAxis.isEmpty())
	{
		if(KviQString::equalCI(szAxis, "XAxis"))
			axis = Qt::XAxis;
		else if(KviQString::equalCI(szAxis, "YAxis"))
			axis = Qt::YAxis;
		else if(KviQString::equalCI(szAxis, "ZAxis"))
			axis = Qt::ZAxis;
		else
			c->warning(__tr2qs_ctx("Unknown axis '%Q' switching to default ZAxis", "objects"), &szAxis);
	}
	PAINTER_ROTATE(dAngle, axis)
	return true;
}

KVSO_CLASS_FUNCTION(painter, translate)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t dXtrasl, dYtrasl;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("trasl_x", KVS_PT_DOUBLE, 0, dXtrasl)
	KVSO_PARAMETER("trasl_y", KVS_PT_DOUBLE, 0, dYtrasl)
	KVSO_PARAMETERS_END(c)
	PAINTER_TRANSLATE(dXtrasl, dYtrasl)
	return true;
}

KVSO_CLASS_FUNCTION(painter, shear)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t dShearh, dShearv;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("shear_h", KVS_PT_DOUBLE, 0, dShearh)
	KVSO_PARAMETER("shear_v", KVS_PT_DOUBLE, 0, dShearv)
	KVSO_PARAMETERS_END(c)
	PAINTER_SHEAR(dShearh, dShearv);
	return true;
}

KVSO_CLASS_FUNCTION(painter, scale)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t dScalex, dScaley;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("scale_x", KVS_PT_DOUBLE, 0, dScalex)
	KVSO_PARAMETER("scale_y", KVS_PT_DOUBLE, 0, dScaley)
	KVSO_PARAMETERS_END(c)
	PAINTER_SCALE(dScalex, dScaley);
	return true;
}

KVSO_CLASS_FUNCTION(painter, reset)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	m_pPainter->resetTransform();
	return true;
}

KVSO_CLASS_FUNCTION(painter, drawHtmlText)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szText;
	kvs_int_t iX, iY, iW, iH;

	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x", KVS_PT_INT, 0, iX)
	KVSO_PARAMETER("y", KVS_PT_INT, 0, iY)
	KVSO_PARAMETER("text", KVS_PT_STRING, 0, szText)
	KVSO_PARAMETER("w", KVS_PT_INT, 0, iW)
	KVSO_PARAMETER("h", KVS_PT_INT, 0, iH)

	KVSO_PARAMETERS_END(c)
	QTextDocument doc;
	doc.setHtml(szText);
	doc.setDefaultFont(m_pPainter->font());
	m_pPainter->save();
	m_pPainter->translate(iX, iY);
	doc.setPageSize(QSize(iW, iH));
	doc.drawContents(m_pPainter);
	m_pPainter->restore();
	return true;
}

KVSO_CLASS_FUNCTION(painter, htmlTextSize)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("text", KVS_PT_STRING, 0, szText)
	KVSO_PARAMETERS_END(c)
	QTextDocument doc;
	doc.setHtml(szText);
	doc.setDefaultFont(m_pPainter->font());
	QSizeF size = doc.size();
	KviKvsArray * a = new KviKvsArray();
	a->set(0, new KviKvsVariant((kvs_real_t)size.width()));
	a->set(1, new KviKvsVariant((kvs_real_t)size.height()));
	c->returnValue()->setArray(a);
	return true;
}

KVSO_CLASS_FUNCTION(painter, drawIcon)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szIcon, szState;
	kvs_int_t iX, iY, iW, iH;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x", KVS_PT_INT, 0, iX)
	KVSO_PARAMETER("y", KVS_PT_INT, 0, iY)
	KVSO_PARAMETER("icon", KVS_PT_STRING, 0, szIcon)
	KVSO_PARAMETER("state", KVS_PT_STRING, KVS_PF_OPTIONAL, szState)
	KVSO_PARAMETER("w", KVS_PT_INT, KVS_PF_OPTIONAL, iW)
	KVSO_PARAMETER("h", KVS_PT_INT, KVS_PF_OPTIONAL, iH)
	KVSO_PARAMETERS_END(c)
	QPixmap * pix = g_pIconManager->getImage(szIcon);
	if(!pix)
	{
		c->warning("The icon '%Q' does not exist", &szIcon);
		return true;
	}
	QSize pixsize(pix->width(), pix->height());
	QIcon ico(*pix);
	QIcon::Mode mode = QIcon::Normal;
	if(!ico.isNull())
	{
		if(KviQString::equalCI(szState, "Disabled"))
			mode = QIcon::Disabled;
		else if(KviQString::equalCI(szState, "Active"))
			mode = QIcon::Active;
		else if(KviQString::equalCI(szState, "Selected"))
			mode = QIcon::Selected;
		if(c->parameterCount() < 5)
			m_pPainter->drawPixmap(iX, iY, ico.pixmap(pixsize, mode));
		else
			m_pPainter->drawPixmap(iX, iY, ico.pixmap(QSize(iW, iH), mode));
	}
	return true;
}

KVSO_CLASS_FUNCTION(painter, setOpacity)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t dOpacity;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("opacity_factor", KVS_PT_DOUBLE, 0, dOpacity)
	KVSO_PARAMETERS_END(c)
	m_pPainter->setOpacity(dOpacity);
	return true;
}

KVSO_CLASS_FUNCTION(painter, setTextAntialiasing)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bEnabled", KVS_PT_BOOL, 0, bEnabled)
	KVSO_PARAMETERS_END(c)
	m_pPainter->setRenderHint(QPainter::TextAntialiasing, bEnabled);
	return true;
}

KVSO_CLASS_FUNCTION(painter, setAntialiasing)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bEnabled", KVS_PT_BOOL, 0, bEnabled)
	KVSO_PARAMETERS_END(c)
	m_pPainter->setRenderHint(QPainter::Antialiasing, bEnabled);
	return true;
}

KVSO_CLASS_FUNCTION(painter, setSmoothPixmapTransform)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bEnabled", KVS_PT_BOOL, 0, bEnabled)
	KVSO_PARAMETERS_END(c)
	m_pPainter->setRenderHint(QPainter::SmoothPixmapTransform, bEnabled);
	return true;
}

KVSO_CLASS_FUNCTION(painter, setPenJoinStyle)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szStyle;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("style", KVS_PT_STRING, 0, szStyle)
	KVSO_PARAMETERS_END(c)
	Qt::PenJoinStyle style = Qt::BevelJoin;
	if(KviQString::equalCI(szStyle, "Milter"))
		style = Qt::MiterJoin;
	else if(KviQString::equalCI(szStyle, "Round"))
		style = Qt::RoundJoin;
	QPen pen = m_pPainter->pen();
	pen.setJoinStyle(style);
	m_pPainter->setPen(pen);
	return true;
}

KVSO_CLASS_FUNCTION(painter, setPenCapStyle)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szStyle;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("style", KVS_PT_STRING, 0, szStyle)
	KVSO_PARAMETERS_END(c)
	Qt::PenCapStyle style = Qt::SquareCap;
	if(KviQString::equalCI(szStyle, "Flat"))
		style = Qt::FlatCap;
	else if(KviQString::equalCI(szStyle, "Round"))
		style = Qt::RoundCap;
	QPen pen = m_pPainter->pen();
	pen.setCapStyle(style);
	m_pPainter->setPen(pen);
	return true;
}

KVSO_CLASS_FUNCTION(painter, save)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	m_pPainter->save();
	return true;
}

KVSO_CLASS_FUNCTION(painter, restore)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	m_pPainter->restore();
	return true;
}

KVSO_CLASS_FUNCTION(painter, setPenStyle)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szStyle;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("style", KVS_PT_STRING, 0, szStyle)
	KVSO_PARAMETERS_END(c)
	bool found = false;
	unsigned int j = 0;
	for(; j < penstyles_num; j++)
	{
		if(KviQString::equalCI(szStyle, penstyles_tbl[j]))
		{
			found = true;
			break;
		}
	}
	QPen pen = m_pPainter->pen();
	if(found)
	{
		pen.setStyle(penstyles_cod[j]);
		m_pPainter->setPen(pen);
	}
	else
		c->warning(__tr2qs_ctx("Unknown pen style '%Q'", "objects"), &szStyle);
	return true;
}

KVSO_CLASS_FUNCTION(painter, setBrushStyle)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szStyle;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("style", KVS_PT_STRING, 0, szStyle)
	KVSO_PARAMETERS_END(c)
	bool found = false;
	unsigned int j = 0;
	for(; j < brushstyles_num; j++)
	{
		if(KviQString::equalCI(szStyle, brushstyles_tbl[j]))
		{
			found = true;
			break;
		}
	}
	QBrush brush = m_pPainter->brush();
	if(found)
	{
		brush.setStyle(brushstyles_cod[j]);
		m_pPainter->setBrush(brush);
	}
	else
		c->warning(__tr2qs_ctx("Unknown brush style '%Q'", "objects"), &szStyle);
	return true;
}

KVSO_CLASS_FUNCTION(painter, setGradientStart)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t dX, dY;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x_Point", KVS_PT_DOUBLE, 0, dX)
	KVSO_PARAMETER("y_Point", KVS_PT_DOUBLE, 0, dY)
	KVSO_PARAMETERS_END(c)
	if(!m_pGradient)
		m_pGradient = new(QLinearGradient);
	m_pGradient->setStart(dX, dY);
	return true;
}

KVSO_CLASS_FUNCTION(painter, setGradientStop)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t dX, dY;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x_Point", KVS_PT_DOUBLE, 0, dX)
	KVSO_PARAMETER("y_Point", KVS_PT_DOUBLE, 0, dY)
	KVSO_PARAMETERS_END(c)
	if(!m_pGradient)
		m_pGradient = new(QLinearGradient);
	m_pGradient->setFinalStop(dX, dY);
	return true;
}

KVSO_CLASS_FUNCTION(painter, setGradientColor)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t dAt;
	kvs_int_t iCol1, iCol2, iCol3, iOpacity;
	KviKvsVariant *var1, *var2, *var3;
	QString szColorMode, szColor;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("at", KVS_PT_DOUBLE, 0, dAt)
	KVSO_PARAMETER("Color_1_Or_Colorname", KVS_PT_VARIANT, 0, var1)
	KVSO_PARAMETER("Color_2", KVS_PT_VARIANT, KVS_PF_OPTIONAL, var2)
	KVSO_PARAMETER("Colo3_3", KVS_PT_VARIANT, KVS_PF_OPTIONAL, var3)
	KVSO_PARAMETER("color_mode", KVS_PT_STRING, KVS_PF_OPTIONAL, szColorMode)
	KVSO_PARAMETER("opacity", KVS_PT_INT, KVS_PF_OPTIONAL, iOpacity)
	KVSO_PARAMETERS_END(c)
	if(!var1->asInteger(iCol1))
	{
		var1->asString(szColor);
		if(c->paramCount() < 3)
			iOpacity = 255;
		else
		{
			if(!var2->asInteger(iOpacity))
			{
				c->warning(__tr2qs_ctx("The opacity parameter didn't evaluate to integer", "objects"));
				return true;
			}
		}
		if(!m_pGradient)
			m_pGradient = new(QLinearGradient);
		QColor col(szColor);
		col.setAlpha(iOpacity);
		m_pGradient->setColorAt(dAt, col);
	}
	else
	{
		if(c->paramCount() < 4)
		{
			c->error(__tr2qs_ctx("Color name or triplet RGB/HSV values required", "objects"));
			return true;
		}
		if(!var2->asInteger(iCol2) || !var3->asInteger(iCol3))
		{
			c->error(__tr2qs_ctx("One of the triplet parameters didn't evaluate to an integer", "objects"));
			return true;
		}
		if(c->paramCount() < 5)
			iOpacity = 255;
		QColor col;
		if(KviQString::equalCI(szColorMode, "HSV"))
			col.setHsv(iCol1, iCol2, iCol3, iOpacity);
		else
			col.setRgb(iCol1, iCol2, iCol3, iOpacity);
		if(!m_pGradient)
			m_pGradient = new(QLinearGradient);
		m_pGradient->setColorAt(dAt, col);
	}
	return true;
}

KVSO_CLASS_FUNCTION(painter, setGradientAsBrush)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	if(!m_pGradient)
		m_pGradient = new(QLinearGradient);
	m_pPainter->setBrush(*m_pGradient);
	return true;
}

KVSO_CLASS_FUNCTION(painter, clearGradient)
{
	if(!m_pGradient)
		delete m_pGradient;
	m_pGradient = nullptr;
	return true;
}

KVSO_CLASS_FUNCTION(painter, setPenWidth)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t iWidth;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("width", KVS_PT_REAL, 0, iWidth)
	KVSO_PARAMETERS_END(c)
	QPen pen = m_pPainter->pen();
	pen.setWidthF(iWidth);
	m_pPainter->setPen(pen);
	return true;
}

KVSO_CLASS_FUNCTION(painter, fillRect)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_int_t iX, iY, iW, iH;
	kvs_int_t iCol1, iCol2, iCol3, iOpacity;
	QString szColorMode, szColor;
	KviKvsVariant *var1, *var2, *var3;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x", KVS_PT_INT, 0, iX)
	KVSO_PARAMETER("y", KVS_PT_INT, 0, iY)
	KVSO_PARAMETER("w", KVS_PT_INT, 0, iW)
	KVSO_PARAMETER("h", KVS_PT_INT, 0, iH)
	KVSO_PARAMETER("Color_1_Or_Colorname", KVS_PT_VARIANT, 0, var1)
	KVSO_PARAMETER("Color_2", KVS_PT_VARIANT, KVS_PF_OPTIONAL, var2)
	KVSO_PARAMETER("Colo3_3", KVS_PT_VARIANT, KVS_PF_OPTIONAL, var3)
	KVSO_PARAMETER("opacity", KVS_PT_INT, KVS_PF_OPTIONAL, iOpacity)
	KVSO_PARAMETER("color_mode", KVS_PT_STRING, KVS_PF_OPTIONAL, szColorMode)
	KVSO_PARAMETERS_END(c)
	QColor col;
	if(!var1->asInteger(iCol1))
	{
		var1->asString(szColor);
		if(c->paramCount() < 6)
			iOpacity = 255;
		else
		{
			if(!var2->asInteger(iOpacity))
			{
				c->warning(__tr2qs_ctx("The opacity parameter didn't evaluate to integer", "objects"));
				return true;
			}
		}
		col.setNamedColor(szColor);
		col.setAlpha(iOpacity);
	}
	else
	{
		if(c->paramCount() < 7)
		{
			c->error(__tr2qs_ctx("Color name or triplet RGB/HSV value required", "objects"));
			return true;
		}
		if(!var2->asInteger(iCol2) || !var3->asInteger(iCol3))
		{
			c->error(__tr2qs_ctx("One of the triplet parameters didn't evaluate to an integer", "objects"));
			return true;
		}
		if(c->paramCount() < 5)
			iOpacity = 255;
		QColor col;
		if(KviQString::equalCI(szColorMode, "HSV"))
			col.setHsv(iCol1, iCol2, iCol3, iOpacity);
		else
			col.setRgb(iCol1, iCol2, iCol3, iOpacity);
	}
	m_pPainter->fillRect(iX, iY, iW, iH, QBrush(col));
	return true;
}

KVSO_CLASS_FUNCTION(painter, colorNames)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QStringList color = QColor::colorNames();
	KviKvsArray * a = new KviKvsArray();
	//a->set(0,new KviKvsVariant((kvs_int_t)col.red()));
	for(int i = 0; i < color.count(); i++)
	{
		a->set(i, new KviKvsVariant(color.at(i)));
	}
	c->returnValue()->setArray(a);
	return true;
}
KVSO_CLASS_FUNCTION(painter, painterDeviceWidth)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_int_t width = m_pPainter->device()->width();
	c->returnValue()->setInteger(width);
	return true;
}
KVSO_CLASS_FUNCTION(painter, painterDeviceHeight)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_int_t height = m_pPainter->device()->height();
	c->returnValue()->setInteger(height);
	return true;
}
KVSO_CLASS_FUNCTION(painter, pathMoveTo)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t dX, dY;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x", KVS_PT_REAL, 0, dX)
	KVSO_PARAMETER("y", KVS_PT_REAL, 0, dY)
	KVSO_PARAMETERS_END(c)
	if(!m_pPainterPath)
		m_pPainterPath = new QPainterPath(QPointF(0, 0));
	m_pPainterPath->moveTo(dX, dY);
	return true;
}

KVSO_CLASS_FUNCTION(painter, pathLineTo)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t dX, dY;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x", KVS_PT_REAL, 0, dX)
	KVSO_PARAMETER("y", KVS_PT_REAL, 0, dY)
	KVSO_PARAMETERS_END(c)
	if(!m_pPainterPath)
		m_pPainterPath = new QPainterPath(QPointF(0, 0));
	m_pPainterPath->lineTo(dX, dY);
	return true;
}

KVSO_CLASS_FUNCTION(painter, pathAddText)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	kvs_real_t dX, dY;
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x", KVS_PT_REAL, 0, dX)
	KVSO_PARAMETER("y", KVS_PT_REAL, 0, dY)
	KVSO_PARAMETER("text", KVS_PT_STRING, 0, szText)
	KVSO_PARAMETERS_END(c)
	if(!m_pPainterPath)
		m_pPainterPath = new QPainterPath(QPointF(0, 0));
	m_pPainterPath->addText(dX, dY, m_pPainter->font(), szText);
	return true;
}

KVSO_CLASS_FUNCTION(painter, drawPath)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	m_pPainter->drawPath(*m_pPainterPath);
	//delete m_pPainterPath;
	//m_pPainterPath=nullptr;
	return true;
}

KVSO_CLASS_FUNCTION(painter, setCompositionMode)
{
	CHECK_INTERNAL_POINTER(m_pPainter)
	QString szComposition;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("style", KVS_PT_STRING, 0, szComposition)
	KVSO_PARAMETERS_END(c)
	bool found = false;
	unsigned int j = 0;
	for(; j < composition_num; j++)
	{
		if(KviQString::equalCI(szComposition, composition_tbl[j]))
		{
			found = true;
			break;
		}
	}
	if(found)
	{
		m_pPainter->setCompositionMode(composition_cod[j]);
	}
	else
		c->warning(__tr2qs_ctx("Unknown composition mode '%Q'", "objects"), &szComposition);
	return true;
}
KVSO_CLASS_FUNCTION(painter, resetPath)
{
	if(m_pPainterPath)
	{
		delete m_pPainterPath;
		m_pPainterPath = nullptr;
	}
	return true;
}
void KvsObject_painter::setInternalPainter(QPainter * p)
{
	delete m_pPainter;
	m_pPainter = p;
	bDonotdeleteinternalqpainter = true;
}
