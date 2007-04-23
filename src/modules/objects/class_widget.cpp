//=============================================================================
//
//   File : class_widget.cpp
//   Creation date : Mon Sep 11 16:35:32 CET 2000 by Krzysztof Godlewski
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
//=============================================================================

#include "kvi_malloc.h"
#include "kvi_debug.h"

#include "kvi_error.h"
#include "kvi_locale.h"
#include "kvi_iconmanager.h"
#include "kvi_window.h"
#include "kvi_out.h"
#include "kvi_mirccntrl.h"
#include "kvi_app.h"
#include "kvi_string.h"
#include <qcursor.h>
#include <qiconset.h>
#include <qcolor.h>
#include <qlayout.h>
#include <qbitmap.h>
#include <qmetaobject.h>
#include "class_widget.h"
#include "class_pixmap.h"

#ifdef COMPILE_USE_QT4
	#include <QKeyEvent>
	#include <QDesktopWidget>
#else
	#include <qwidgetlist.h>
#endif

#include <qwidget.h>
#include <qtooltip.h>
#include <qfont.h>
#include <qvariant.h>

// FIX ME: WFLAGS
const char * const widgettypes_tbl[] = {
			"TopLevel",
			"Dialog",
			"Popup",
			"Desktop",
			"Customize",
			"Title",
			"StaysOnTop",
			"SysMenu",
			"Minimize",
			"Maximize",
			"NoAutoErase"
			   };
#ifdef COMPILE_USE_QT4
const Qt::WidgetAttribute widgetattributes_cod[]= {
	Qt::WA_OpaquePaintEvent,
	Qt::WA_NoSystemBackground,
	Qt::WA_PaintOnScreen,
	Qt::WA_NoMousePropagation,
};

const char * const widgetattributes_tbl[] = {
	"opaquePaintEvent",
	"noSystemBackground",
	"paintOnScreen",
	"noMousePropagation",
};
#define widgetattributes_num	(sizeof(widgetattributes_tbl) / sizeof(widgetattributes_tbl[0]))


const Qt::WindowType widgettypes_cod[] = {
#else 
const int widgettypes_cod[] = {
#endif
		Qt::WType_TopLevel,
		Qt::WType_Dialog,
		Qt::WType_Popup,
		Qt::WType_Desktop,
		Qt::WStyle_Customize,
		Qt::WStyle_Title,
		Qt::WStyle_StaysOnTop,
		Qt::WStyle_SysMenu,
		Qt::WStyle_Minimize,
		Qt::WStyle_Maximize,
		Qt::WNoAutoErase
};


#ifdef COMPILE_USE_QT4

	#define QT_WIDGET_TABFOCUS Qt::TabFocus
	#define	QT_WIDGET_CLICKFOCUS Qt::ClickFocus
	#define QT_WIDGET_STRONGFOCUS Qt::StrongFocus
	#define QT_WIDGET_NOFOCUS Qt::NoFocus

#else
	#define QT_WIDGET_TABFOCUS QWidget::TabFocus
	#define	QT_WIDGET_CLICKFOCUS QWidget::ClickFocus
	#define QT_WIDGET_STRONGFOCUS QWidget::StrongFocus
	#define QT_WIDGET_NOFOCUS QWidget::NoFocus
#endif


#define widgettypes_num	(sizeof(widgettypes_tbl) / sizeof(widgettypes_tbl[0]))

/*
	@doc: widget
	@keyterms:
		widget object class
	@title:
		widget class
	@type:
		class
	@short:
		Base class for all widgets
	@inherits:
		[class]object[/class]
	@description:
		This object class is the rappresentation of a widget.
		All the other widget-type classes inherit from this one.
	@functions:
		!fn: $show()
		Shows this widget and the children.
		See also [classfnc]$hide[/classfnc]() and [classfnc]$isVisible[/classfnc].
		!fn: $hide()
		Hides this widget (and conseguently all the children).
		See also [classfnc]$show[/classfnc]() and [classfnc]$isVisible[/classfnc].
		!fn: $repaint(<bool erase>)
		Repaints the widget directly by calling [classfnc]$paintEvent[/classfnc]() immediately.[br]
		If erase is TRUE, erases the widget before the $paintEvent() call.
		!fn: $x()
		Returns the x coordinate of the upper-left corner
		of this widget relative to the parent widget,
		or to the desktop if this widget is a toplevel one.
		!fn: $y()
		Returns the y coordinate of the uspper-left corner
		of this widget relative to the parent widget,
		or to the desktop if this widget is a toplevel one.
		!fn: $width()
		Returns the width of this widget in pixels.
		!fn: $height()
		Returns the height of this widget in pixels.
		!fn: $geometry()
		Returns the widget geometry in this form:[br]
		x, y, width, height.
		!fn: $setGeometry(<x_or_array>,[<y>,<width>,<heigth>])
		Sets the geometry of this widget. <x> and <y> are relative
		to the parent widget or to the desktop (if this widget is
		a toplevel one). All the parameters are in pixels.
		!fn: $setMinimumWidth(<value>)
		Sets the minimum width of this widget to <value>.
		The user will not be able to resize the widget to a smaller
		value. This value is also used by the [class:layout]layout class[/class].
		!fn: $setMinimumHeight(<value>)
		Sets the minimum height of this widget to <value>.
		The user will not be able to resize the widget to a smaller
		value. This value is also used by the [class:layout]layout class[/class].
		!fn: $setMaximumWidth(<value>)
		Sets the maximum width of this widget to <value>.
		The user will not be able to resize the widget to a bigger
		value. This value is also used by the [class:layout]layout class[/class].
		!fn: $setMaximumHeight(<value>)
		Sets the maximum height of this widget to <value>.
		The user will not be able to resize the widget to a bigger
		value. This value is also used by the [class:layout]layout class[/class].
		!fn: $move(<x_or_array>[,<y>])
		Moves this widget to the coordinate <x> and <y> relative to its
		parent widget (or the desktop if this widget is a toplevel one).
		This is equivalent to [classfnc]$setGeometry[/classfnc](<x>,<y>,
		[classfnc]$width[/classfnc](),[classfnc]$height[/classfnc]()).
		!fn: $resize(<w_or_array>,[height])
		Changes the widget's width to <width> and height to <height>.
		See also [classfnc]$setGeometry[/classfnc]().
		!fn: $isEnabled()
		Returns '1' if the widget is enabled , '0' otherwise.
		See also [classfnc:widget]$setEnabled[/classfnc]().
		!fn: $setEnabled(<bool>)
		Sets the widget state to enabled or disabled if <bool> is 1 or 0 respectively.
		A disabled widget does not receive keyboard nor mouse input.
		!fn: $setCaption(<text>)
		Sets the caption of this widget to <text>.
		This is meaningful for toplevel widgets only.
		!fn: $setToolTip(<tooltip_text>)
		Set the tooltip of this widget; the text can contain HTML formatting.
		!fn: $window.caption()
		Returns the caption text of this widget.
		!fn: $isTopLevel()
		Returns '1' if this widget is a toplevel (parentless) one,
		'0' otherwise.
		!fn: $isVisible()
		Returns '1' if this widget is currently visible (read: is managed
		by the window manager and displayed by the X server; the widget
		may be hidden behind other widgets). If the widget is not visible
		this function returns '0'.
		See also [classfnc]$show[/classfnc]() and [classfnc]$hide[/classfnc]().
		!fn: $raise()
		Moves this widget to the top of the stack of the widgets relative
		to its parent. See also [classfnc]$lower[/classfnc].
		!fn: $lower()
		Moves this widget to the bottom of the stack of the widgets relative
		to its parent. See also [classfnc]$raise[/classfnc]
		!fn: $hasFocus()
		Returns '1' if this widget has the keyboard focus.
		See also [classfnc]$setFocus[/classfnc].
		!fn: $setFocus()
		Sets this widget to be the one that receives keyboard events.
		See also [classfnc]$hasFocus[/classfnc]
		!fn: $parentWidget()
		Returns the object id of the parent widget, or '0' if this
		widget is a toplevel one.
		!fn: $backgroundColor()
		Returns the background color of this widget in hexadecimal
		html-like format. For example , for a black bacground you will get
		the string "000000" , for a red one , "FF0000", for a white one
		"FFFFFF". See also [classfnc]$setBackgroundColor[/classfnc]()
		!fn: $setBackgroundColor(<rgb(hex string)_array(red:integer,green:integer,blue_integer)_or_red>,[geen:integer],[blue:integer])
		Sets the background color of this widget to <rgb_value>:valid values are:
		- hex string: must be a string with 6 hexadecimal digits (like the ones used to
		specify colors in html pages). The first two digits specify
		the RED component, the third and fourth digit specify the GREEN component
		and the last two specify the BLUE component.
		For example "FFFF00" means full red, full green and no blue that gives
		a yellow color, "808000" designates a brown color (dark yellow),
		"A000A0" is a kind of violet.
		- array(red:integer,green:integer,blue:integer)
		- red:integer,green:integer,blue:integer.
		See also [classfnc]$foregroundColor[/classfnc].
		!fn: $setForegroundColor(<rgb(hex string)_array(red:integer,green:integer,blue_integer)_or_red>,[geen:integer],[blue:integer])
		Sets the foreground color of this widget to <rgb_value>:valid values are:
		- hex string: must be a string with 6 hexadecimal digits (like the ones used to
		specify colors in html pages). The first two digits specify
		the RED component, the third and fourth digit specify the GREEN component
		and the last two specify the BLUE component.
		For example "FFFF00" means full red, full green and no blue that gives
		a yellow color, "808000" designates a brown color (dark yellow),
		"A000A0" is a kind of violet.
		- array(red:integer,green:integer,blue:integer)
		- red:integer,green:integer,blue:integer.
		See also [classfnc]$foregroundColor[/classfnc].
		!fn: $foregroundColor()
		Returns the foreground color of this widget in hexadecimal
		html-like format.
		See also [classfnc]$setForegroundColor[/classfnc].
		!fn: $setMouseTracking(<bool>)
		Enables or disables the mouse tracking if <bool> is '1' or '0' respectively.
		When mouse tracking is enabled you will receive mouse move events
		even if no button is pressed, otherwise you will receive it only
		when a mouse button is being pressed (so after a mousePressEvent).
		!fn: $mousePressEvent(<button>,<x>,<y>)
		This function is called when a mouse button is pressed while
		the cursor is in this widget. <button> is 0 if the pressed button
		is the left one, 1 if the button is the right one and 2 if it is the middle one.
		The <x> and <y> coordinates are relative to this widget upper-left corner
		and are expressed in pixels.
		If you call "[cmd]setreturn[/cmd] 1" you will stop the internal processing
		of this event. The default implementation does nothing.
		!fn: $mouseReleaseEvent(<button>,<x>,<y>)
		This function is called when a mouse button is released while
		the cursor is in this widget. <button> is 0 if the released button
		is the left one, 1 if the button is the right one and 2 if it is the middle one.
		The <x> and <y> coordinates are relative to this widget upper-left corner
		and are expressed in pixels.
		If you call "[cmd]setreturn[/cmd] 1" you will stop the internal processing
		of this event. The default implementation does nothing.
		!fn: $mouseDoubleClickEvent(<button>,<x>,<y>)
		This function is called when a mouse button is double clicked while
		the cursor is in this widget. <button> is 0 if the double clicked button
		is the left one, 1 if the button is the right one and 2 if it is the middle one.
		The <x> and <y> coordinates are relative to this widget upper-left corner
		and are expressed in pixels.
		If you call "[cmd]setreturn[/cmd] 1" you will stop the internal processing
		of this event. The default implementation does nothing.
		!fn: $mouseMoveEvent(<button>,<x>,<y>)
		This function is called when the mouse cursor moves inside this widget.
		<button> is 0 if the pressed button
		is the left one, 1 if the button is the right one and 2 if it is the middle one.
		The special value of -1 indicates that no button is being pressed.
		The <x> and <y> coordinates are relative to this widget upper-left corner
		and are expressed in pixels.
		Normally you will receive this event only if a mouse button is being pressed
		while moving. If you want to receive it also when the mouse buttons are not
		pressed, call [classfnc]$setMouseTracking[/classfnc]().
		If you call "[cmd]return[/cmd]$true" you will stop the internal processing
		of this event. The default implementation does nothing.
		!fn: $focusInEvent()
		This function is called when this widget gains keyboard focus.
		If you call "[cmd]return[/cmd] $true" you will stop the internal processing
		of this event. The default implementation does nothing.
		!fn: $focusOutEvent()
		This function is called when this widget looses keyboard focus.
		If you call "[cmd]return[/cmd] $true" you will stop the internal processing
		of this event. The default implementation does nothing.
		!fn: $mouseLeaveEvent()
		This function is called when the mouse leaves this widget.
		If you call "[cmd]return[/cmd] $true" you will stop the internal processing
		of this event. The default implementation does nothing.
		!fn: $mouseEnterEvent()
		This function is called when the mouse enters this widget.
		If you call "[cmd]return[/cmd] $true" you will stop the internal processing
		of this event. The default implementation does nothing.
		!fn: $showEvent()
		This function is called when this widget is being shown.
		If you call "[cmd]return[/cmd] $true" you will stop the internal processing
		of this event. The default implementation does nothing.
		!fn: $hideEvent()
		This function is called when this widget is being hidden.
		If you call "[cmd]return[/cmd] $true" you will stop the internal processing
		of this event. The default implementation does nothing.
		!fn: $closeEvent()
		This function is called when this widget is going to be closed.
		If you call "[cmd]return[/cmd] $true" you will ignore the close event.
		The default implementation does nothing.
		!fn: $resizeEvent()
		This function is called immediately after this widget has been resized.
		If you call "[cmd]return[/cmd] $true" you will stop the internal processing
		of this event. The default implementation does nothing.
		!fn: $moveEvent()
		This function is called immediately after this widget has been moved.
		If you call "[cmd]return[/cmd] $true" you will stop the internal processing
		of this event. The default implementation does nothing.
		!fn: <tip:string> $maybeTipEvent(<x_tip_pos:integer>,<y_tip_pos:integer>)[QT4 only]
		This event handler is called when a eventualy tip is going to be show.
		You can be reimplement this event and set a dynamic tool tip by using "[cmd]return[/cmd] <tooltip_string>".
		If a tooltip has setted with [classfnc]$setTooltip[/classfnc] the dynamic tooltip will be ignored.
		The default implementation does nothing.		
		!fn: $paintEvent()
		This event handler can be reimplemented to repaint all or part of the widget.
		It's needed by the Painter class.
		It's very useful for drawing flicker free animations or low level special graphic effects.
		If you call "[cmd]return[/cmd] $true" you will stop the internal processing
		of this event.
		The default implementation does nothing.
		!fn: $setIcon(<image_id>)
		Sets the icon for this widget. This is meaningful only for toplevel widgets.
		See the [doc:image_id]image identifier[/doc] documentation for the explaination
		of the <image_id> parameter.
		!fn: $setBackgroundImage(<image_id>)
		Sets the background image for this widget.
		See the [doc:image_id]image identifier[/doc] documentation for the explaination
		of the <image_id> parameter.
		For some kind of widgets, setting a background pixmap may have no effect or
		have strange results. Experiment with it.
		To unset the background image call [classfnc]$setBackgroundColor[/classfnc]
		!fn: $setFont(<size>,<family>,<style>)[br]
		Set the font's size, family and stile, valid flag for style are:[br]
		[pre]
		italic     [br]
		bold     [br]
		underline      [br]
		overline    [br]
		strikeout  [br]
		fixedpitch  [br]
		[/pre]
		!fn: $setWFlags(<flag1>, <flag2>, ...)
		This function sets widget flags, given as parameters.
		Valid flags are:
		[pre]
		TopLevel		- indicates that this widget is a top-level widget[br]
		Dialog		- indicates that this widget is a top-level window that should be decorated as a dialog[br]
		Desktop		- indicates that this widget is the desktop[br]
		Popup		- indicates that this widget is a popup top-level window[br]
		NoAutoErase - indicates that this widget paints all its pixels. Updating, resizing, scrolling and focus changes should therefore not erase the widget.
		Customize	- let's the user to customize the style of the widget.[br]
		[br]
		Valid parameter for a customized widget are:[br]
		Title			- gives the window a title bar[br]
		StaysOnTop	- window stays on top [br]
		SysMenu		- add a windows system menu[br]
		Minimize		- add a minimize button for the sysmenu style[br]
		Maximize		- add a maximixe button for the sysmenu style[br]
		[/pre]
		!fn: $centerToScreen()
		Centers the window on the screen (useful only for toplevel widgets).[br]
		The best place to call this function is in $showEvent()
		!fn: $setFocusPolicy(<key focus>)
		Sets the way the widget accepts keyboard focus.[br]
		Valid parameters are:
		[pre]
		- TabFocus;	(widget accepts keyboard focus by tabbing)[br]
		- ClickFocus; (widget accepts keyboard focus by clicking)[br]
		- StrongFocus; (widget accepts both tabbing/clicking)[br]
		- No Focus;  (widget does not accept focus at all; this is the default value)[br]
		[/pre]
		!fn: $keyPressEvent(<key>)
		If widget accepts keyboard focus (see [classfnc]$setFocusPolicy[/classfnc] ) this function handles for keys;
		In its argument the key pressed.[br]
		Special keys are:
		[pre]
		- Return [br]
		- Enter	[br]
		- Down (cursor arrow down) [br]
		- Up   (cursor arrow up) [br]
		- Left (cursor arrow left) [br]
		- Right (cursor arrow right) [br]
		- Shift [br]
		- Ctrl [br]
		- Alt [br]
		- CapsLock [br]
		- Backspace [br]
		- Del [br]
		- Esc [br]
		- 0 [br]
		- 1 [br]
		- 2 [br]
		- 3 [br]
		- 4 [br]
		- 5 [br]
		- 6 [br]
		- 7 [br]
		- 8 [br]
		- 9 [br]
		- + [br]
		- - [br]
		- * [br]
		- / [br]
		- ( [br]
		- ) [br]
		- = [br]
		- . [br]
		- ^ [br]

		[/pre]
		!fn: $mapFromGlobal(<x>,<y>)
		Translates the global screen coordinate pos to widget coordinates.
		!fn: $mapToGlobal(<x>,<y>)
		Translates widget coordinates into the global screen coordinate pos.
		!fn: integer $globalCursorX()
		Return the x coordinate of mouse pointer global position.
		!fn: integer $globalCursorY()
		Return the y coordinate of the mo>use pointer global position.
		@examples:
		[example]
			%Widget = $new(widget)
			# This is the main container for other elements.

			%Widget->$setCaption("This is the widget title")

			%Widget->$setGeometry(100,200,170,290)
			# 100 and 200 are distance (pixel) from the left and the top of father widget (in this case the KVIrc window)
			# 300 and 400 are the weight and the height of the widget

			%Widget->%label = $new(label,%Widget)
			%Widget->%label->$setText("This is a text label")
			%Widget->%label->$setGeometry(10,10,150,20)

			%Widget->%lineedit = $new(lineedit,%Widget)
			%Widget->%lineedit->$setText("This is a lineedit")
			%Widget->%lineedit->$setGeometry(10,40,150,20)

			%Widget->%multilineedit = $new(multilineedit,%Widget)
			%Widget->%multilineedit->$setText("This is a multilineedit")
			%Widget->%multilineedit->$setGeometry(10,70,150,40)

			%Widget->%checkbox = $new(checkbox,%Widget)
			%Widget->%checkbox->$setText("This is a checkbox")
			%Widget->%checkbox->$setGeometry(10,120,150,20)

			%Widget->%combobox = $new(combobox,%Widget)
			%Widget->%combobox->$setGeometry(10,150,150,20)
			%Widget->%combobox->$insertItem("This is a combobox")
			%Widget->%combobox->$insertItem("This is a combobox's item")

			%Widget->%listbox = $new(listbox,%Widget)
			%Widget->%listbox->$setGeometry(10,180,150,40)
			%Widget->%listbox->$insertItem("This is another item")
			%Widget->%listbox->$insertItem("This is a listbox's item")
			%Widget->%listbox->$insertItem("This is a listbox")

			%Widget->%radiobutton = $new(radiobutton,%Widget)
			%Widget->%radiobutton->$setGeometry(10,230,150,20)
			%Widget->%radiobutton->$setText("This is a radiobutton")

			%Widget->%button = $new(button, %Widget)
			%Widget->%button->$setText("Close me")
			%Widget->%button->$setGeometry(10,260,150,20)

			%Widget->$show()
			# This shows the widget

			privateimpl(%Widget->%button, mousePressEvent) {
				delete %Widget
				# This closes the widget automatically
			}
			# privateimpl is triggered when user press the button.
			# To do that, widget must be a global variable.
		[/example]

*/




//
// If you never call c->error() , c->warning() (your function never fails)
// and never call a function that can call c->error() or c->warning()
// you can avoid ENTER_STACK_FRAME and c->leaveStackFrame()
// Just return true.
// c->error() should be called only in really critical situations
// (we have to define better "critical situation")
// if possible , leave the things go after just a c->warning()
//

	// widget() will be never 0 in THIS class
	// but in derived classes WILL be
	// ... now that I think about it , it
	// may happen that widget() will be zero here too:
	// If the Qt "physical" widget gets destroyed
	// by some external factor (for example when children
	// of a wrapper widget destroyed by KVIrc).
	//
	// as a convention:
	//   if widget() can be 0 in a class
	//   the user must have a function to check it
	//   (sth as object::$hasObject() could do the trick)
	//   obviously this will happen only in well defined cases
	//   as in a qtwrapper not yet wrapped or failed to wrap (so
	//   qtwrapper::$wrap() returned '0' for example)
	//   or after the widget has been destroyed...
	//   if widget() is 0 , the functions perform no actions
	//   return no errors and results that have to be assumed
	//   as invalid (this allows the minimum overhead: if widget()

//=============================================================================================================



KVSO_BEGIN_REGISTERCLASS(KviKvsObject_widget,"widget","object")
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"reparent",function_reparent)
	// apparence
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"show",function_show)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"hide",function_hide)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"repaint",function_repaint)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"isTopLevel",function_isTopLevel)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"isVisible",function_isVisible)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"raise",function_raise)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"lower",function_lower)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"setEnabled",function_setEnabled)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"isEnabled",function_isEnabled)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"setMouseTracking",function_setMouseTracking)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"setCaption",function_setCaption)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"caption",function_caption)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"hasFocus",function_hasFocus)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"setFocus",function_setFocus)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"setToolTip",function_setToolTip)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"setWFlags",function_setWFlags)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"setIcon",function_setIcon)
	// fonts
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"fontDescent",function_fontDescent)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"fontAscent",function_fontAscent)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"fontMetricsWidth",function_fontMetricsWidth)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"fontMetricsHeight",function_fontMetricsHeight)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"setFont",function_setFont)
	// geometry
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"screenResolution",function_screenResolution)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"centerToScreen",function_centerToScreen)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"geometry",function_geometry)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"setGeometry",function_setGeometry)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"mapToGlobal",function_mapToGlobal)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"mapFromGlobal",function_mapFromGlobal)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"y",function_y)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"x",function_x)

	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"globalCursorX",function_globalCursorX)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"globalCursorY",function_globalCursorY)

	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"width",function_width)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"height",function_height)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"setMinimumWidth",function_setMinimumWidth)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"setMinimumHeight",function_setMinimumHeight)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"setMaximumWidth",function_setMaximumWidth)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"setMaximumHeight",function_setMaximumHeight)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"move",function_move)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"sizeHint",function_sizeHint)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"resize",function_resize)

	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"setFocusPolicy",function_setFocusPolicy)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"addWidgetToWrappedLayout",function_addWidgetToWrappedLayout)

	// colors and image
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"setForegroundColor",function_setPaletteForeground)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"setBackgroundColor",function_setBackgroundColor)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"setBackgroundImage",function_setBackgroundImage)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"backgroundColor",function_backgroundColor)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"foregroundColor",function_foregroundColor)
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"setMask",function_setMask)

	// QT4 only
#ifdef COMPILE_USE_QT4
	KVSO_REGISTER_HANDLER(KviKvsObject_widget,"setAttribute",function_setAttribute)
#endif

	// events
	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_widget,"mousePressEvent")
	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_widget,"mouseReleaseEvent")
	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_widget,"mouseDoubleClickEvent")
	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_widget,"mouseMoveEvent")
	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_widget,"focusInEvent")
	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_widget,"focusOutEvent")
	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_widget,"mouseLeaveEvent")
	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_widget,"mouseEnterEvent")
	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_widget,"showEvent")
	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_widget,"hideEvent")
	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_widget,"closeEvent")
	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_widget,"resizeEvent")
	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_widget,"moveEvent")
	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_widget,"paintEvent")
	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_widget,"keyPressEvent")
#ifdef COMPILE_USE_QT4
	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_widget,"maybeTipEvent")
#endif


KVSO_END_REGISTERCLASS(KviKvsObject_widget)


KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_widget,KviKvsObject)
KVSO_END_CONSTRUCTOR(KviKvsObject_widget)

KVSO_BEGIN_DESTRUCTOR(KviKvsObject_widget)
	emit aboutToDie();
KVSO_END_CONSTRUCTOR(KviKvsObject_widget)

bool KviKvsObject_widget::init(KviKvsRunTimeContext * pContext,KviKvsVariantList * pParams)
{
	setObject(new QWidget(parentScriptWidget(),getName()),true);
	return true;
}

bool KviKvsObject_widget::eventFilter(QObject *o,QEvent *e)
{
	if(o == object())
	{
		bool ret=false;
		int aparam;
		bool brokenhandler=false;
		KviKvsVariant *retv=new KviKvsVariant(ret);
			
		switch(e->type())
		{
			#ifdef COMPILE_USE_QT4
			case QEvent::ToolTip:
			{
				QHelpEvent *helpEvent = static_cast<QHelpEvent *>(e);
				QPoint point=helpEvent->pos();
				QString szTooltip;
				KviKvsVariant *tipret=new KviKvsVariant(szTooltip);
				KviKvsVariantList params(new KviKvsVariant((kvs_int_t)point.x()),new KviKvsVariant((kvs_int_t)point.y()));
				callFunction(this,"maybeTipEvent",tipret,&params);
				tipret->asString(szTooltip);
				QToolTip::showText(helpEvent->globalPos(),szTooltip);
				break;
			}
			#endif
			case QEvent::Paint:
			{
				QRect rect=((QPaintEvent *)e)->rect();
				KviKvsVariantList params(new KviKvsVariant((kvs_int_t)rect.x()),new KviKvsVariant((kvs_int_t)rect.y()),new KviKvsVariant((kvs_int_t)rect.width()),new KviKvsVariant((kvs_int_t)rect.height()));
				callFunction(this,"paintEvent",retv,&params);
				break;
			}

		case QEvent::KeyPress:
			{
				QString tmp="";
					switch(((QKeyEvent *)e)->key())
					{
					case Qt::Key_Return:
						tmp="Return";
						break;
					case Qt::Key_Down:
						tmp="Down";
						break;
					case Qt::Key_Up:
						tmp="Up";
						break;
					case Qt::Key_Left:
						tmp="Left";
						break;
					case Qt::Key_Right:
						tmp="Right";
						break;
					case Qt::Key_Shift:
						tmp="Shift";
						break;
					case Qt::Key_Control:
						tmp="Ctrl";
						break;
					case Qt::Key_Alt:
						tmp="Alt";
						break;
					case Qt::Key_CapsLock:
						tmp="CapsLock";
						break;
					case Qt::Key_Backspace:
						tmp="Backspace";
						break;
					case Qt::Key_Delete:
						tmp="Del";
						break;

					case Qt::Key_Enter:
						tmp="Enter";
						break;
					case Qt::Key_Escape:
						tmp="Esc";
						break;
					case Qt::Key_0:
						tmp="0";
						case Qt::Key_1:
						tmp="1";
						break;
					case Qt::Key_2:
						tmp="2";
						break;
					case Qt::Key_3:
						tmp="3";
						break;
					case Qt::Key_4:
						tmp="4";
						break;
					case Qt::Key_5:
						tmp="5";
						break;
					case Qt::Key_6:
						tmp="6";
						break;
					case Qt::Key_7:
						tmp="7";
						break;
					case Qt::Key_8:
						tmp="8";
						break;
					case Qt::Key_9:
						tmp="9";
						break;
					case Qt::Key_Plus:
						tmp="+";
						break;
					case Qt::Key_Minus:
						tmp="-";
						break;
					case Qt::Key_Slash:
						tmp="/";
						break;
					case Qt::Key_Asterisk:
						tmp="*";
						break;
					case Qt::Key_Period:
						tmp=".";
						break;
					case Qt::Key_ParenLeft:
						tmp="(";
						break;
					case Qt::Key_ParenRight:
						tmp=")";
						break;
					case Qt::Key_Equal:
						tmp="=";
						break;
					case Qt::Key_AsciiCircum:
						tmp="^";
						break;
					default:
						if (!((QKeyEvent *)e)->text().isEmpty()) tmp = ((QKeyEvent *)e)->text();
					}

				KviKvsVariantList params(new KviKvsVariant(tmp));
				callFunction(this,"keyPressEvent",0,&params);
			}
			break;
			case QEvent::MouseButtonPress:
					if(((QMouseEvent *)e)->button() & Qt::LeftButton)aparam = 0;
				else {
					if(((QMouseEvent *)e)->button() & Qt::RightButton)aparam = 1;
					else aparam = 2;
				}
				if(!callFunction(this,"mousePressEvent",retv,new KviKvsVariantList(new KviKvsVariant((kvs_int_t)aparam),new KviKvsVariant((kvs_int_t)((QMouseEvent *)e)->pos().x()),new KviKvsVariant((kvs_int_t)((QMouseEvent *)e)->pos().y())
				))) brokenhandler = true; // ignore results of a broken event handler
		break;
			case QEvent::MouseButtonRelease:
				if(((QMouseEvent *)e)->button() & Qt::LeftButton)aparam = 0;
				else {
					if(((QMouseEvent *)e)->button() & Qt::RightButton)aparam = 1;
					else aparam = 2;
				}
				if(!callFunction(this,"mouseReleaseEvent",retv,new KviKvsVariantList(new KviKvsVariant((kvs_int_t)aparam),new KviKvsVariant((kvs_int_t)((QMouseEvent *)e)->pos().x()),new KviKvsVariant((kvs_int_t)((QMouseEvent *)e)->pos().y())
				))) brokenhandler = true; // ignore results of a broken event handler
			break;
			case QEvent::MouseButtonDblClick:
				if(( (QMouseEvent *)e)->button() & Qt::LeftButton)aparam = 0;
				else {
					if(((QMouseEvent *)e)->button() & Qt::RightButton)aparam = 1;
					else aparam = 2;
				}
					if(!callFunction(this,"mouseDoubleClickEvent",retv,new KviKvsVariantList(new KviKvsVariant((kvs_int_t)aparam),new KviKvsVariant((kvs_int_t)((QMouseEvent *)e)->pos().x()),new KviKvsVariant((kvs_int_t)((QMouseEvent *)e)->pos().y())
				))) brokenhandler = true; // ignore results of a broken event handler

			break;
			case QEvent::MouseMove:
				if( (((QMouseEvent *)e)->state()) & Qt::LeftButton) aparam = 0;
				else
				{
					if(((QMouseEvent *)e)->state() & Qt::RightButton)aparam = 1;
					else
						{
							if(((QMouseEvent *)e)->state() & Qt::MidButton)aparam = 2;
							else aparam = -1;
						}
				}
				if(!callFunction(this,"mouseMoveEvent",retv,new KviKvsVariantList(new KviKvsVariant((kvs_int_t)aparam),new KviKvsVariant((kvs_int_t)((QMouseEvent *)e)->pos().x()),new KviKvsVariant((kvs_int_t)((QMouseEvent *)e)->pos().y())
				))) brokenhandler = true; // ignore results of a broken event handler
			break;
			case QEvent::FocusIn:
				if(!callFunction(this,"focusInEvent",retv,0))brokenhandler = true;
			break;
			case QEvent::FocusOut:
				if(!callFunction(this,"focusOutEvent",retv,0))brokenhandler = true;
			break;
			case QEvent::Resize:
				if(!callFunction(this,"resizeEvent",retv,0))brokenhandler = true;
			break;
			case QEvent::Move:
				if(!callFunction(this,"moveEvent",retv,0))brokenhandler = true;
			break;
			case QEvent::Close:
				if(!callFunction(this,"closeEvent",retv,0))brokenhandler = true;
			break;
			case QEvent::Enter:
				if(!callFunction(this,"mouseEnterEvent",retv,0))brokenhandler = true;
			break;
			case QEvent::Leave:
				if(!callFunction(this,"mouseLeaveEvent",retv,0))brokenhandler = true;
			break;
			case QEvent::Show:
				if(!callFunction(this,"showEvent",retv,0))brokenhandler = true;
			break;
			case QEvent::Hide:
				if(!callFunction(this,"hideEvent",retv,0))ret =false;
			break;
			default:
				return KviKvsObject::eventFilter(o,e);
			break;

		}
		if (!brokenhandler)	ret=retv->asBoolean();
		delete retv;
		return ret;
		/*
		if(ret.length() == 1)
		{
			if(KviQString::equalCI("1",ret))return true;
		}
		*/
	}

	return KviKvsObject::eventFilter(o,e);
}


bool KviKvsObject_widget::function_show(KviKvsObjectFunctionCall *c)
{
	if(!widget())return true; // should we warn here ?
	widget()->show();
	return true;
}

bool KviKvsObject_widget::function_setEnabled(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	widget()->setEnabled(bEnabled);
	return true;
}
bool KviKvsObject_widget::function_isEnabled(KviKvsObjectFunctionCall *c)
{
	if(!widget()) return true;
	c->returnValue()->setBoolean(widget()->isEnabled());
	return true;
}
bool KviKvsObject_widget::function_fontDescent(KviKvsObjectFunctionCall * c)
{
	if(!widget()) return true;
	QFontMetrics fm = widget()->fontMetrics();
    int d = fm.descent();
	c->returnValue()->setInteger(d);
    return true;
}

bool KviKvsObject_widget::function_fontAscent(KviKvsObjectFunctionCall * c)
{
	if(!widget()) return true;
    QFontMetrics fm = widget()->fontMetrics();
    int d = fm.ascent();
	c->returnValue()->setInteger(d);
    return true;

}

bool KviKvsObject_widget::function_repaint(KviKvsObjectFunctionCall * c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(!widget()) return true;
    widget()->repaint(bEnabled);
	return true;
}

bool KviKvsObject_widget::function_fontMetricsWidth(KviKvsObjectFunctionCall * c)
{
	QString m_szStr;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("string",KVS_PT_STRING,0,m_szStr)
	KVSO_PARAMETERS_END(c)
	if(!widget()) return true;
//	int fm = widget()->fontMetrics().width(m_szStr);
	c->returnValue()->setInteger(widget()->fontMetrics().width(m_szStr));
	return true;
}

bool KviKvsObject_widget::function_fontMetricsHeight(KviKvsObjectFunctionCall * c)
{
	if(!widget())return true;
    int fm = widget()->fontMetrics().height();
	c->returnValue()->setInteger(fm);
    return true;
}
bool KviKvsObject_widget::function_screenResolution(KviKvsObjectFunctionCall * c)
{
	KviKvsArray * a = new KviKvsArray();
	a->set(0,new KviKvsVariant((kvs_int_t)g_pApp->desktop()->width()));
	a->set(1,new KviKvsVariant((kvs_int_t)g_pApp->desktop()->height()));
	c->returnValue()->setArray(a);
	return true;
}

bool KviKvsObject_widget::function_geometry(KviKvsObjectFunctionCall *c)
{
	QRect r = widget() ? widget()->geometry() : QRect(0,0,0,0);
	KviKvsArray * a = new KviKvsArray();
	a->set(0,new KviKvsVariant((kvs_int_t)r.x()));
	a->set(1,new KviKvsVariant((kvs_int_t)r.y()));
	a->set(2,new KviKvsVariant((kvs_int_t)r.width()));
	a->set(3,new KviKvsVariant((kvs_int_t)r.height()));
	c->returnValue()->setArray(a);
	return true;
}

bool KviKvsObject_widget::function_setGeometry(KviKvsObjectFunctionCall *c)
{
	KviKvsVariant * pXOrArray;
	kvs_int_t iX,iY,iW,iH;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x_or_array",KVS_PT_VARIANT,0,pXOrArray)
		KVSO_PARAMETER("y",KVS_PT_INT,KVS_PF_OPTIONAL,iY)
		KVSO_PARAMETER("w",KVS_PT_INT,KVS_PF_OPTIONAL,iW)
		KVSO_PARAMETER("h",KVS_PT_INT,KVS_PF_OPTIONAL,iH)
	KVSO_PARAMETERS_END(c)

	// this is a little bit tricky: we accept two syntax versions
	// (this is something like a C++ overload)
	// $setGeometry(x,y,w,h) OR $setGeometry($array(x,y,w,h))

	// For this purpose we ask the parameter processing engine
	// to return the first parameter as variant and we manually check its data typoe
	// We also set the following three parameters as optional
	// so the user can either pass one or four parameters.

	// Because of this trick, we must check the parameter consistency
	// manually.

	if(pXOrArray->isArray())
	{
		// the user has passed a complete geometry array as first parameter
		// make sure it has at least 4 elements
		if(pXOrArray->array()->size() < 4)
		{
			c->error(__tr2qs("The array passed as parameter must contain at least 4 elements"));
			return false;
		}
		// we must also manually extract the data from the array
		KviKvsVariant * pX = pXOrArray->array()->at(0);
		KviKvsVariant * pY = pXOrArray->array()->at(1);
		KviKvsVariant * pW = pXOrArray->array()->at(2);
		KviKvsVariant * pH = pXOrArray->array()->at(3);
		// note that some of the array elements may be empty in fact
		// a null value returned by KviKvsArray::at() means that
		// no parameter was set in that place
		if(!(pX && pY && pW && pH))
		{
			c->error(__tr2qs("One of the geometry array parameters is empty"));
			return false;
		}
		if(!(pX->asInteger(iX) && pY->asInteger(iY) && pW->asInteger(iW) && pH->asInteger(iH)))
		{
			c->error(__tr2qs("One of the geometry array parameters didn't evaluate to an integer"));
			return false;
		}
		// ok: the params are correct
	} else {
		// The user passed something else as first parameter
		// make sure that it is an integer.
		// Also make sure that we really have 4 parameters
		if(c->params()->count() < 4)
		{
			c->error(__tr2qs("$setGeometry() requires either an array as first parameter or four integers"));
			return false;
		}
		if(!pXOrArray->asInteger(iX))
		{
			c->error(__tr2qs("The first parameter didn't evaluate to an array nor an integer"));
			return false;
		}
		// ok: the params are correct
	}

	if(widget())
		widget()->setGeometry(iX,iY,iW,iH);

	return true;
}
bool KviKvsObject_widget::function_mapToGlobal(KviKvsObjectFunctionCall *c)

{
	if(!widget())return true;
	kvs_int_t iX,iY;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x",KVS_PT_INT,0,iX)
		KVSO_PARAMETER("y",KVS_PT_INT,0,iY)
	KVSO_PARAMETERS_END(c)
	QPoint point=widget()->mapToGlobal(QPoint(iX,iY));
	KviKvsArray * a = new KviKvsArray();
	a->set(0,new KviKvsVariant((kvs_int_t)point.x()));
	a->set(1,new KviKvsVariant((kvs_int_t)point.y()));
	c->returnValue()->setArray(a);
	return true;
}

bool KviKvsObject_widget::function_mapFromGlobal(KviKvsObjectFunctionCall *c)

{
	if(!widget())return true;
	kvs_int_t iX,iY;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x",KVS_PT_INT,0,iX)
		KVSO_PARAMETER("y",KVS_PT_INT,0,iY)
	KVSO_PARAMETERS_END(c)
	QPoint point=widget()->mapFromGlobal(QPoint(iX,iY));
	KviKvsArray * a = new KviKvsArray();
	a->set(0,new KviKvsVariant((kvs_int_t)point.x()));
	a->set(1,new KviKvsVariant((kvs_int_t)point.y()));
	c->returnValue()->setArray(a);
	return true;
}
bool KviKvsObject_widget::function_centerToScreen(KviKvsObjectFunctionCall *c)
{
	if(widget()) widget()->move((g_pApp->desktop()->width() - widget()->width())/2,(g_pApp->desktop()->height() - widget()->height())/2);
	return true;
}

bool KviKvsObject_widget::function_setPaletteForeground(KviKvsObjectFunctionCall *c)
{
	KviKvsVariant * pColArray;
	kvs_int_t iColR,iColG,iColB;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("hex_rgb_array_or_red",KVS_PT_VARIANT,0,pColArray)
		KVSO_PARAMETER("green",KVS_PT_INT,KVS_PF_OPTIONAL,iColG)
		KVSO_PARAMETER("blue",KVS_PT_INT,KVS_PF_OPTIONAL,iColB)
	KVSO_PARAMETERS_END(c)

	if(pColArray->isArray())
	{
		if(pColArray->array()->size() < 3)
		{
			c->error(__tr2qs("The array passed as parameter must contain at least 3 elements"));
			return false;
		}
		KviKvsVariant * pColR = pColArray->array()->at(0);
		KviKvsVariant * pColG = pColArray->array()->at(1);
		KviKvsVariant * pColB = pColArray->array()->at(2);

		if(!(pColR && pColG && pColB))
		{
			c->error(__tr2qs("One of the colors array parameters is empty"));
			return false;
		}
		if(!(pColR->asInteger(iColR) && pColG->asInteger(iColG) && pColB->asInteger(iColB)))
		{
			c->error(__tr2qs("One of the colors array parameters didn't evaluate to an integer"));
			return false;
		}

	} else {
			if (c->params()->count()==1)
			{
				bool bOk,bOk1,bOk2;
				QString value;
				pColArray->asString(value);
				int i=0;
				if (value.length()!=6)
				{
						c->warning(__tr2qs("A string of 6 digits hex is required"));
						return true;
				}
				QString buffer(value.mid(0,2));
				iColR=buffer.toInt(&bOk,16);
				buffer=value.mid(2,2);
				iColG=buffer.toInt(&bOk1,16);
				buffer=value.mid(4,2);
				iColB=buffer.toInt(&bOk2,16);
				if (!bOk || !bOk1 || !bOk2)
				{
					c->warning(__tr2qs("Not an hex digits"));
				return true;
				}
			if (widget()) widget()->setPaletteForegroundColor(QColor(iColR,iColG,iColB));
			return true;
			}

		if(c->params()->count() < 3)
		{

			c->error(__tr2qs("$setForegroundColor requires either an array as first parameter, one hex string or three integers"));
			return false;
		}
		if(!pColArray->asInteger(iColR))
		{
			c->error(__tr2qs("The first parameter didn't evaluate to an array nor an integer"));
			return false;
		}
	}
	if (widget()) widget()->setPaletteForegroundColor(QColor(iColR,iColG,iColB));
	return true;
}

bool KviKvsObject_widget::function_setBackgroundColor(KviKvsObjectFunctionCall *c)
{
	KviKvsVariant * pColArray;
	kvs_int_t iColR,iColG,iColB;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("hex_rgb_array_or_red",KVS_PT_VARIANT,0,pColArray)
		KVSO_PARAMETER("green",KVS_PT_INT,KVS_PF_OPTIONAL,iColG)
		KVSO_PARAMETER("blue",KVS_PT_INT,KVS_PF_OPTIONAL,iColB)
	KVSO_PARAMETERS_END(c)

	if(pColArray->isArray())
	{
		if(pColArray->array()->size() < 3)
		{
			c->error(__tr2qs("The array passed as parameter must contain at least 3 elements"));
			return false;
		}
		KviKvsVariant * pColR = pColArray->array()->at(0);
		KviKvsVariant * pColG = pColArray->array()->at(1);
		KviKvsVariant * pColB = pColArray->array()->at(2);

		if(!(pColR && pColG && pColB))
		{
			c->error(__tr2qs("One of the colors array parameters is empty"));
			return false;
		}
		if(!(pColR->asInteger(iColR) && pColG->asInteger(iColG) && pColB->asInteger(iColB)))
		{
			c->error(__tr2qs("One of the colors array parameters didn't evaluate to an integer"));
			return false;
		}

	} else {
			if (c->params()->count()==1)
			{
				bool bOk,bOk1,bOk2;
				QString value;
				pColArray->asString(value);
				int i=0;
				if (value.length()!=6)
				{
						c->warning(__tr2qs("A string of 6 digits hex is required"));
						return true;
				}
				QString buffer(value.mid(0,2));
				iColR=buffer.toInt(&bOk,16);
				buffer=value.mid(2,2);
				iColG=buffer.toInt(&bOk1,16);
				buffer=value.mid(4,2);
				iColB=buffer.toInt(&bOk2,16);
				if (!bOk || !bOk1 || !bOk2)
				{
					c->warning(__tr2qs("Not an hex digits"));
				return true;
				}
			if (widget()) widget()->setBackgroundColor(QColor(iColR,iColG,iColB));
			return true;
			}

		if(c->params()->count() < 3)
		{

			c->error(__tr2qs("$setBackgroundColor requires either an array as first parameter, one hex string or three integers"));
			return false;
		}
		if(!pColArray->asInteger(iColR))
		{
			c->error(__tr2qs("The first parameter didn't evaluate to an array nor an integer"));
			return false;
		}
	}
	if (widget()) widget()->setBackgroundColor(QColor(iColR,iColG,iColB));
	return true;
}

bool KviKvsObject_widget::function_backgroundColor(KviKvsObjectFunctionCall *c)
{
	if(!widget())return true;
	QColor col = widget()->backgroundColor();
	KviKvsArray * a = new KviKvsArray();
	a->set(0,new KviKvsVariant((kvs_int_t)col.red()));
	a->set(1,new KviKvsVariant((kvs_int_t)col.green()));
	a->set(2,new KviKvsVariant((kvs_int_t)col.blue()));
	c->returnValue()->setArray(a);
	return true;
}

bool KviKvsObject_widget::function_foregroundColor(KviKvsObjectFunctionCall *c)
{
	if(!widget())return true;
	QColor col = widget()->foregroundColor();
	KviKvsArray * a = new KviKvsArray();
	a->set(0,new KviKvsVariant((kvs_int_t)col.red()));
	a->set(1,new KviKvsVariant((kvs_int_t)col.green()));
	a->set(2,new KviKvsVariant((kvs_int_t)col.blue()));
	c->returnValue()->setArray(a);
	return true;
}

bool KviKvsObject_widget::function_parentWidget(KviKvsObjectFunctionCall *c)
{
	if(!widget())return true;
	if(parentScriptWidget()) c->returnValue()->setHObject(parentObject()->handle());
	else
		c->returnValue()->setHObject((kvs_hobject_t)0);
    return true;

}
bool KviKvsObject_widget::function_setMouseTracking(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(widget()) widget()->setMouseTracking(bEnabled);
	return true;
}

bool KviKvsObject_widget::function_setCaption(KviKvsObjectFunctionCall *c)
{
	QString szCaption;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("caption",KVS_PT_STRING,0,szCaption)
	KVSO_PARAMETERS_END(c)
	if(widget()) widget()->setCaption(szCaption);
	return true;
}

bool KviKvsObject_widget::function_caption(KviKvsObjectFunctionCall *c)
{
	if(widget()) c->returnValue()->setString(widget()->caption().utf8().data());
	return true;
}

bool KviKvsObject_widget::function_isTopLevel(KviKvsObjectFunctionCall *c)
{
	if(widget()) c->returnValue()->setBoolean(widget()->isTopLevel());
	return true;
}

bool KviKvsObject_widget::function_isVisible(KviKvsObjectFunctionCall *c)
{
	if(widget()) c->returnValue()->setBoolean(widget()->isVisible());
	return true;
}

bool KviKvsObject_widget::function_raise(KviKvsObjectFunctionCall *)
{
	if(widget()) widget()->raise();
	return true;
}

bool KviKvsObject_widget::function_lower(KviKvsObjectFunctionCall *)
{
	if(widget())widget()->lower();
	return true;
}

bool KviKvsObject_widget::function_hasFocus(KviKvsObjectFunctionCall *c)
{
	if(widget())  c->returnValue()->setBoolean(widget()->hasFocus());
	return true;
}

bool KviKvsObject_widget::function_setFocus(KviKvsObjectFunctionCall *c)
{
	if (widget())widget()->setFocus();
	return true;
}
bool KviKvsObject_widget::function_hide(KviKvsObjectFunctionCall *)
{
	if(widget())widget()->hide();
	return true;
}

bool KviKvsObject_widget::function_setToolTip(KviKvsObjectFunctionCall *c)
{
	QString szTooltip;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("tooltip",KVS_PT_STRING,0,szTooltip)
	KVSO_PARAMETERS_END(c)
	if(widget())QToolTip::add( widget(), szTooltip  );
	return true;
}

bool KviKvsObject_widget::function_x(KviKvsObjectFunctionCall *c)
{
	if(widget()) c->returnValue()->setInteger(widget()->x());
	return true;
}

bool KviKvsObject_widget::function_y(KviKvsObjectFunctionCall *c)
{
	if(widget())c->returnValue()->setInteger(widget()->y());
	return true;
}

bool KviKvsObject_widget::function_width(KviKvsObjectFunctionCall *c)
{
	if(widget())c->returnValue()->setInteger(widget()->width());
	return true;
}

bool KviKvsObject_widget::function_height(KviKvsObjectFunctionCall *c)
{
	if(widget())c->returnValue()->setInteger(widget()->height());
	return true;
}

bool KviKvsObject_widget::function_setMinimumWidth(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iW;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("w",KVS_PT_INT,0,iW)
	KVSO_PARAMETERS_END(c)
	if (widget()) widget()->setMinimumWidth(iW);
	return true;
}

bool KviKvsObject_widget::function_setMinimumHeight(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iH;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("h",KVS_PT_INT,0,iH)
	KVSO_PARAMETERS_END(c)
	if (widget()) widget()->setMinimumHeight(iH);
	return true;
}

bool KviKvsObject_widget::function_setMaximumWidth(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iW;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("w",KVS_PT_INT,0,iW)
	KVSO_PARAMETERS_END(c)
    if (widget()) widget()->setMaximumWidth(iW);
	return true;
}

bool KviKvsObject_widget::function_setMaximumHeight(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iH;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("h",KVS_PT_INT,0,iH)
	KVSO_PARAMETERS_END(c)
	if(widget()) widget()->setMaximumHeight(iH);
	return true;
}

bool KviKvsObject_widget::function_move(KviKvsObjectFunctionCall *c)
{
	kvs_int_t iX,iY;

	KviKvsVariant * pXOrArray;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("x_or_array",KVS_PT_VARIANT,0,pXOrArray)
		KVSO_PARAMETER("y",KVS_PT_INT,KVS_PF_OPTIONAL,iY)
		KVSO_PARAMETERS_END(c)
		if(pXOrArray->isArray())
	{
		if(pXOrArray->array()->size() < 2)
		{
			c->error(__tr2qs("The array passed as parameter must contain at least 2 elements"));
			return false;
		}
		KviKvsVariant * pX = pXOrArray->array()->at(0);
		KviKvsVariant * pY = pXOrArray->array()->at(1);
		if(!(pX && pY))
		{
			c->error(__tr2qs("One of the move array parameters is empty"));
			return false;
		}
		if(!(pX->asInteger(iX) && pY->asInteger(iY)))
		{
			c->error(__tr2qs("One of the move array parameters didn't evaluate to an integer"));
			return false;
		}
		// ok: the params are correct
	} else {
		if(c->params()->count() < 2)
		{
			c->error(__tr2qs("$move() requires either an array as first parameter or two integers"));
			return false;
		}
		if(!pXOrArray->asInteger(iX))
		{
			c->error(__tr2qs("The first parameter didn't evaluate to an array nor an integer"));
			return false;
		}
		// ok: the params are correct
	}

	if(!widget())return true;
	widget()->move(QPoint(iX,iY));
	return true;
}
bool KviKvsObject_widget::function_sizeHint(KviKvsObjectFunctionCall *c)
{
	if(!widget())return true;
	QSize sizehint = widget()->sizeHint();
	KviKvsArray * a = new KviKvsArray();
	a->set(0,new KviKvsVariant((kvs_int_t)sizehint.width()));
	a->set(1,new KviKvsVariant((kvs_int_t)sizehint.height()));
	c->returnValue()->setArray(a);
	return true;
}

bool KviKvsObject_widget::function_resize(KviKvsObjectFunctionCall *c)
{
	KviKvsVariant * pWOrArray;
	kvs_int_t iW,iH;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("width_or_array",KVS_PT_VARIANT,0,pWOrArray)
		KVSO_PARAMETER("height",KVS_PT_INT,KVS_PF_OPTIONAL,iH)
		KVSO_PARAMETERS_END(c)
		if(pWOrArray->isArray())
	{
		if(pWOrArray->array()->size() < 2)
		{
			c->error(__tr2qs("The array passed as parameter must contain at least 2 elements"));
			return false;
		}
		KviKvsVariant * pW = pWOrArray->array()->at(0);
		KviKvsVariant * pH = pWOrArray->array()->at(1);
		if(!(pW && pH))
		{
			c->error(__tr2qs("One of the resize array parameters is empty"));
			return false;
		}
		if(!(pW->asInteger(iW) && pH->asInteger(iH)))
		{
			c->error(__tr2qs("One of the resize array parameters didn't evaluate to an integer"));
			return false;
		}
		// ok: the params are correct
	} else {
		if(c->params()->count() < 2)
		{
			c->error(__tr2qs("$resize() requires either an array as first parameter or two integers"));
			return false;
		}
		if(!pWOrArray->asInteger(iW))
		{
			c->error(__tr2qs("The first parameter didn't evaluate to an array nor an integer"));
			return false;
		}
		// ok: the params are correct
	}

	if(!widget())return true;
	widget()->resize(QSize(iW,iH));
	return true;
}
bool KviKvsObject_widget::function_setFocusPolicy(KviKvsObjectFunctionCall *c)
{
	QString szMode;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("focus",KVS_PT_STRING,0,szMode)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	if(KviQString::equalCI(szMode, "TabFocus"))
		widget()->setFocusPolicy(QT_WIDGET_TABFOCUS);
	else
	if(KviQString::equalCI(szMode, "ClickFocus"))
		widget()->setFocusPolicy(QT_WIDGET_CLICKFOCUS);
	else
	if(KviQString::equalCI(szMode, "StrongFocus"))
		widget()->setFocusPolicy(QT_WIDGET_STRONGFOCUS);
	else
	if(KviQString::equalCI(szMode, "NoFocus"))
		widget()->setFocusPolicy(QT_WIDGET_NOFOCUS);
	else c->warning(__tr2qs("Invalid parameters"));
	return true;
}

bool KviKvsObject_widget::function_setWFlags(KviKvsObjectFunctionCall *c)
{
	QStringList wflags;
	//Qt::WindowType sum;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("widget_flags",KVS_PT_STRINGLIST,KVS_PF_OPTIONAL,wflags)
	KVSO_PARAMETERS_END(c)
	if (!widget()) return true;
	#ifdef COMPILE_USE_QT4
		Qt::WindowFlags flag,sum=0;
	#else
		int flag,sum=0;
	#endif
	for ( QStringList::Iterator it = wflags.begin(); it != wflags.end(); ++it )
	{
			
				flag=0;
			for(unsigned int j = 0; j < widgettypes_num; j++)
			{
				if(KviQString::equalCI((*it), widgettypes_tbl[j]))
				{
					flag=widgettypes_cod[j];
					break;
				}
			}
			if(flag)
				sum = sum | flag;
			else
				c->warning(__tr2qs("Unknown widget flag '%Q'"),&(*it));

		}
	
#ifdef COMPILE_USE_QT4
	widget()->setWindowFlags(sum);
#else
	 widget()->reparent(widget()->parentWidget(),sum,QPoint(widget()->x(),widget()->y()));
#endif
	 return true;
}

bool KviKvsObject_widget::function_setFont(KviKvsObjectFunctionCall *c)
{

	QString szFamily,szStyle;
	kvs_int_t uSize;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("size",KVS_PT_UNSIGNEDINTEGER,0,uSize)
		KVSO_PARAMETER("family",KVS_PT_STRING,0,szFamily)
		KVSO_PARAMETER("style",KVS_PT_STRING,0,szStyle)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	QFont font=widget()->font();
	font.setFamily(szFamily);
	font.setPointSize(uSize);
	if(KviQString::equalCI(szStyle,"italic")) font.setItalic(TRUE);
	else if(KviQString::equalCI(szStyle,"bold")) font.setBold(TRUE);
	else if(KviQString::equalCI(szStyle,"underline"))font.setUnderline(TRUE);
	else if(KviQString::equalCI(szStyle,"overline")) font.setOverline(TRUE);
	else if(KviQString::equalCI(szStyle,"strikeout"))font.setStrikeOut(TRUE);
	else if(KviQString::equalCI(szStyle,"fixedpitch")) font.setFixedPitch(TRUE);
	else c->warning(__tr2qs("Unknown style '%Q'"),&szStyle);
	widget()->setFont(font);
	return true;
}

bool KviKvsObject_widget::function_addWidgetToWrappedLayout(KviKvsObjectFunctionCall *c)
{
	KviKvsObject *ob;
	kvs_int_t uCol,uRow;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("col",KVS_PT_UNSIGNEDINTEGER,0,uCol)
		KVSO_PARAMETER("row",KVS_PT_UNSIGNEDINTEGER,0,uRow)
	KVSO_PARAMETERS_END(c)
	ob=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!widget())return true;
	if (!ob)
	{
		c->warning(__tr2qs("Widget parameter is not an object"));
		return true;
	}
	if (!ob->object())
	{
		c->warning(__tr2qs("Widget parameter is not a valid object"));
		return true;
	}
	QLayout *lay=widget()->layout();
	if (!lay)
	{
		c->warning(__tr2qs("No Layout associated to the widget "));
		return true;
	}
	if(!ob->object()->isWidgetType())
	{
		c->warning(__tr2qs("Can't add a non-widget object"));
		return true;
	}
	lay->add(((QWidget *)(ob->object())));
	return true;
}
bool KviKvsObject_widget::function_reparent(KviKvsObjectFunctionCall *c)
{
	KviKvsObject *ob;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("widget",KVS_PT_HOBJECT,KVS_PF_OPTIONAL,hObject)
	KVSO_PARAMETERS_END(c)
	ob=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!widget())    return true;
	if(!ob)
	{
		widget()->reparent(0,QPoint(widget()->x(),widget()->y()));
		return true;
	}
	else
	if(!ob->object()->isWidgetType())
	{
		c->warning(__tr("Parent must be a widget object"));
		return true;
	}
	widget()->reparent(((QWidget *)(ob->object())),QPoint(((QWidget *)(ob->object()))->x(),((QWidget *)(ob->object()))->y()));
	return true;
}
bool KviKvsObject_widget::function_setIcon(KviKvsObjectFunctionCall *c)
{

	QString icon;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("icon",KVS_PT_STRING,0,icon)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	QPixmap * pix = g_pIconManager->getImage(icon);
	if(pix)widget()->setIcon(*pix);
	return true;
}

bool KviKvsObject_widget::function_setBackgroundImage(KviKvsObjectFunctionCall *c)
{
	QString image;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("image",KVS_PT_STRING,0,image)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	QPixmap * pix = g_pIconManager->getImage(image);
	if(pix)widget()->setPaletteBackgroundPixmap(*pix);
	return true;
}
bool KviKvsObject_widget::function_globalCursorX(KviKvsObjectFunctionCall *c)
{
	if(widget())c->returnValue()->setInteger(QCursor::pos().x());
	return true;
}
bool KviKvsObject_widget::function_globalCursorY(KviKvsObjectFunctionCall *c)
{
	if(widget())c->returnValue()->setInteger(QCursor::pos().y());
	return true;
}
bool KviKvsObject_widget::function_setMask(KviKvsObjectFunctionCall *c)
{
	KviKvsObject *obj;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("pixmap",KVS_PT_HOBJECT,0,hObject)
	KVSO_PARAMETERS_END(c)
	obj=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!widget())return true;
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
#ifdef COMPILE_USE_QT4
	QBitmap mask(pm->mask());
#else
	QBitmap mask(*pm->mask());
#endif
	if (mask.isNull()) c->warning(__tr2qs("Null mask"));
	widget()->setMask(mask);
	return true;
}
#ifdef COMPILE_USE_QT4
bool KviKvsObject_widget::function_setAttribute(KviKvsObjectFunctionCall *c)
{
	QString attribute;
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("widget_atribute",KVS_PT_STRING,0,attribute)
		KVSO_PARAMETER("bool_flag",KVS_PT_BOOLEAN,0,bFlag)
	KVSO_PARAMETERS_END(c)
	if (!widget()) return true;
	bool found=false;
	unsigned int j = 0;
	for(; j < widgetattributes_num; j++)
	{
		if(KviQString::equalCI(attribute, widgetattributes_tbl[j]))
		{
			found=true;
			break;
		}
	}
	if(found)widget()->setAttribute(widgetattributes_cod[j],bFlag);
	else c->warning(__tr2qs("Unknown widget attribute '%Q'"),&attribute);	
	return true;
}
#endif

#include "m_class_widget.moc"
