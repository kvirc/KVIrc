//=============================================================================
//
//   File : KvsObject_widget.cpp
//   Creation date : Mon Sep 11 16:35:32 CET 2000 by Krzysztof Godlewski
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000 Krzysztof Godlewski
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KvsObject_widget.h"
#include "KvsObject_pixmap.h"

#include "KviMemory.h"
#include "kvi_debug.h"
#include "KviError.h"
#include "KviLocale.h"
#include "KviIconManager.h"
#include "KviWindow.h"
#include "KviApplication.h"
#include "KviMainWindow.h"
#include "KviStatusBar.h"
#include "KvsObject_painter.h"

#include <QKeyEvent>
#include <QDesktopWidget>
#include <QWidget>
#include <QToolTip>
#include <QFont>
#include <QVariant>
#include <QCursor>
#include <QColor>
#include <QLayout>
#include <QBitmap>
#include <QMetaObject>
#include <QIcon>
#include <QStatusBar>
#include <QUrl>
#include <QPainter>
#include <QApplication>
#include <QPoint>
#include <QContextMenuEvent>

#ifdef COMPILE_KDE4_SUPPORT
#include <KStatusBar>
#endif

KviKvsWidget::KviKvsWidget(KvsObject_widget * object, QWidget * par)
    : QWidget(par), m_pObject(object)
{
}

KviKvsWidget::~KviKvsWidget()
    = default;

QSize KviKvsWidget::sizeHint() const
{
	QSize size = QWidget::sizeHint();
	KviKvsVariant oReturnBuffer;
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)size.width()), new KviKvsVariant((kvs_int_t)size.height()));
	m_pObject->callFunction(m_pObject, "sizeHintRequestEvent", &oReturnBuffer, &params);
	if(oReturnBuffer.isArray())
	{
		if(oReturnBuffer.array()->size() == 2)
		{
			kvs_int_t w, h;
			if(oReturnBuffer.array()->at(0)->asInteger(w) && oReturnBuffer.array()->at(1)->asInteger(h))
				return QSize(w, h);
		}
	}
	return QWidget::sizeHint();
}

const Qt::WidgetAttribute widgetattributes_cod[] = {
	Qt::WA_OpaquePaintEvent,
	Qt::WA_NoSystemBackground,
	Qt::WA_PaintOnScreen,
	Qt::WA_NoMousePropagation
};

const char * const widgetattributes_tbl[] = {
	"opaquePaintEvent",
	"noSystemBackground",
	"paintOnScreen",
	"noMousePropagation"
};

#define widgetattributes_num (sizeof(widgetattributes_tbl) / sizeof(widgetattributes_tbl[0]))

const QPalette::ColorRole colorrole_cod[] = {
	QPalette::Window,
	QPalette::Background,
	QPalette::WindowText,
	QPalette::Foreground,
	QPalette::Base,
	QPalette::AlternateBase,
	QPalette::Text,
	QPalette::Button,
	QPalette::ButtonText,
	QPalette::BrightText,
	QPalette::Highlight,
	QPalette::HighlightedText
};

const char * const colorrole_tbl[] = {
	"Window",
	"Background",
	"WindowText",
	"Foreground",
	"Base",
	"AlternateBase",
	"Text",
	"Button",
	"ButtonText",
	"BrightText",
	"Highlight",
	"HighlightedText"
};

#define colorrole_num (sizeof(colorrole_tbl) / sizeof(colorrole_tbl[0]))

const Qt::WindowType widgettypes_cod[] = {
	Qt::Window,
	Qt::Dialog,
	Qt::Popup,
	Qt::Desktop,
	Qt::WindowTitleHint,
	Qt::WindowStaysOnTopHint,
	Qt::WindowSystemMenuHint,
	Qt::WindowMinimizeButtonHint,
	Qt::WindowMaximizeButtonHint,
	Qt::SubWindow,
	Qt::FramelessWindowHint
};

const char * const widgettypes_tbl[] = {
	"Window",
	"Dialog",
	"Popup",
	"Desktop",
	"Title",
	"StaysOnTop",
	"SysMenu",
	"Minimize",
	"Maximize",
	"Subwindow",
	"FramelessWindow"
};

#define widgettypes_num (sizeof(widgettypes_tbl) / sizeof(widgettypes_tbl[0]))

#define QT_WIDGET_TABFOCUS Qt::TabFocus
#define QT_WIDGET_CLICKFOCUS Qt::ClickFocus
#define QT_WIDGET_STRONGFOCUS Qt::StrongFocus
#define QT_WIDGET_NOFOCUS Qt::NoFocus

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
		This object class is the representation of a widget.
		All the other widget-type classes inherit from this one.
	@functions:
		!fn: $show()
		Shows this widget and the child items.
		See also [classfnc]$hide[/classfnc]() and [classfnc]$isVisible[/classfnc].
		!fn: $hide()
		Hides this widget (and conseguently all the child items).
		See also [classfnc]$show[/classfnc]() and [classfnc]$isVisible[/classfnc].
		!fn: $repaint()
		Repaints the widget directly by calling [classfnc]$paintEvent[/classfnc]() immediately.
		!fn: $update([<x:ingeter>,<y:integer>,<width:integer>,<height:integer>])
		Updates entirely the widget or a rectangle.
		This function does not cause an immediate [classfnc]$paintEvent[/classfnc]();
		instead it schedules a paint event for processing when KVIrc returns to the main event loop.
		!fn: $x()
		Returns the x coordinate of the upper-left corner
		of this widget relative to the parent widget,
		or to the desktop if this widget is a toplevel one.
		!fn: $y()
		Returns the y coordinate of the upper-left corner
		of this widget relative to the parent widget,
		or to the desktop if this widget is a toplevel one.
		!fn: $width()
		Returns the width of this widget in pixels.
		!fn: $height()
		Returns the height of this widget in pixels.
		!fn: $geometry()
		Returns the widget geometry in this form:[br]
		x, y, width, height.
		!fn: $setGeometry(<x_or_array>,[<y>,<width>,<height>])
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
		!fn: $setFixedWidth(<value>)
		Sets the fixed width of this widget to <value>.
		The user will not be able to resize the widget to a smaller or a bigger
		value.
		!fn: $setFixedHeight(<value>)
		Sets the fixed height of this widget to <value>.
		The user will not be able to resize the widget to a smaller or a bigger
		value.
		!fn: $setFixedSize(<width>,<height>)
		Sets the fixed width and height of this widget to <width> and <height>.
		The user will not be able to resize the widget to a smaller or a bigger
		value.
		!fn: $setSizePolicy(<width_policy>,<height_policy>)
		Sets the size policy of this widget.
		!fn: $move(<x_or_array>[,<y>])
		Moves this widget to the coordinate <x> and <y> relative to its
		parent widget (or the desktop if this widget is a toplevel one).
		This is equivalent to [classfnc]$setGeometry[/classfnc](<x>,<y>,
		[classfnc]$width[/classfnc](),[classfnc]$height[/classfnc]()).
		!fn: $resize(<w_or_array>,[height])
		Changes the widget's width to <width> and height to <height>.
		See also [classfnc]$setGeometry[/classfnc]().
		!fn: $isEnabled()
		Returns [b]1[/b] if the widget is enabled, and [b]0[/b] otherwise.
		See also [classfnc:widget]$setEnabled[/classfnc]().
		!fn: $setEnabled(<bool>)
		Sets the widget state to enabled or disabled if <bool> is 1 or 0 respectively.
		A disabled widget does not receive keyboard nor mouse input.
		!fn: $setWindowTitle(<text>)
		Sets the title of this widget to <text>.
		This is meaningful for toplevel widgets only.
		!fn: $setToolTip(<tooltip_text>)
		Set the tooltip of this widget; the text can contain HTML formatting.
		!fn: $windowTitle()
		Returns the title text of this widget.
		!fn: $isTopLevel()
		Returns [b]1[/b] if this widget is a toplevel (parentless) one, and [b]0[/b] otherwise.
		!fn: $isVisible()
		Returns [b]1[/b] if this widget is currently visible (read: is managed
		by the window manager and displayed by the X server; the widget
		may be hidden behind other widgets). If the widget is not visible
		this function returns [b]0[/b].
		See also [classfnc]$show[/classfnc]() and [classfnc]$hide[/classfnc]().
		!fn: $raise()
		Moves this widget to the top of the stack of the widgets relative
		to its parent. See also [classfnc]$lower[/classfnc].
		!fn: $lower()
		Moves this widget to the bottom of the stack of the widgets relative
		to its parent. See also [classfnc]$raise[/classfnc]
		!fn: $hasFocus()
		Returns [b]1[/b] if this widget has the keyboard focus.
		See also [classfnc]$setFocus[/classfnc].
		!fn: $setFocus()
		Sets this widget to be the one that receives keyboard events.
		See also [classfnc]$hasFocus[/classfnc]
		!fn: $parentWidget()
		Returns the object ID of the parent widget, or [b]0[/b] if this
		widget is a toplevel one.
		!fn: $backgroundColor()
		Returns the background color of this widget in hexadecimal
		HTML-like format. For example, for a black background you will get
		the string "000000", for a red one, "FF0000", for a white one
		"FFFFFF". See also [classfnc]$setBackgroundColor[/classfnc]()
		!fn: $setBackgroundColor(<rgb(hex string)_array(red:integer,green:integer,blue_integer)_or_red>,[green:integer],[blue:integer])
		Sets the background color of this widget to <rgb_value>:valid values are:
		- hex string: must be a string with 6 hexadecimal digits (like the ones used to
		specify colors in HTML pages). The first two digits specify
		the RED component, the third and fourth digit specify the GREEN component
		and the last two specify the BLUE component.
		For example "FFFF00" means full red, full green and no blue that gives
		a yellow color, "808000" designates a brown color (dark yellow),
		"A000A0" is a kind of violet.
		- array(red:integer,green:integer,blue:integer)
		- red:integer,green:integer,blue:integer.
		See also [classfnc]$foregroundColor[/classfnc].
		!fn: $setForegroundColor(<rgb(hex string)_array(red:integer,green:integer,blue_integer)_or_red>,[green:integer],[blue:integer])
		Sets the foreground color of this widget to <rgb_value>:valid values are:
		- hex string: must be a string with 6 hexadecimal digits (like the ones used to
		specify colors in HTML pages). The first two digits specify
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
		HTML-like format.
		See also [classfnc]$setForegroundColor[/classfnc].
		!fn: $setMouseTracking(<bool>)
		Enables or disables the mouse tracking if <bool> is [b]1[/b] or [b]0[/b] respectively.
		When mouse tracking is enabled you will receive mouse move events
		even if no button is pressed, otherwise you will receive it only
		when a mouse button is being pressed (so after a mousePressEvent).
		!fn: $mousePressEvent(<button>,<x>,<y>)
		This function is called when a mouse button is pressed while
		the cursor is in this widget. <button> is [b]0[/b] if the pressed button
		is the left one, [b]1[/b] if the button is the right one and [b]2[/b] if it is the middle one.
		The <x> and <y> coordinates are relative to this widget upper-left corner
		and are expressed in pixels.
		If you call i][cmd]setreturn[/cmd] [b]1[/b][/i] you will stop the internal processing
		of this event. The default implementation does nothing.
		!fn: $mouseReleaseEvent(<button>,<x>,<y>)
		This function is called when a mouse button is released while
		the cursor is in this widget. <button> is [b]0[/b]if the released button
		is the left one, [b]1[/b] if the button is the right one and [b]2[/b] if it is the middle one.
		The <x> and <y> coordinates are relative to this widget upper-left corner
		and are expressed in pixels.
		If you call [i][cmd]setreturn[/cmd] [b]1[/b][/i] you will stop the internal processing
		of this event. The default implementation does nothing.
		!fn: $mouseDoubleClickEvent(<button>,<x>,<y>)
		This function is called when a mouse button is double clicked while
		the cursor is in this widget. <button> is [b]0[/b]if the double clicked button
		is the left one, [b]1[/b] if the button is the right one and [b]2[/b] if it is the middle one.
		The <x> and <y> coordinates are relative to this widget upper-left corner
		and are expressed in pixels.
		If you call [i][cmd]setreturn[/cmd] [b]1[/b][/i] you will stop the internal processing
		of this event. The default implementation does nothing.
		!fn: $mouseMoveEvent(<x>,<y>)
		This function is called when the mouse cursor moves inside this widget.
		The <x> and <y> coordinates are relative to this widget upper-left corner
		and are expressed in pixels.
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
		!fn: $paintEvent(<painter:h_object>,<rect_x:integer>,<rect_y:integer>,<width:integer>,<height:integer>)
		This event handler can be reimplemented to repaint all or part of the widget.
		It's needed by the Painter class.
		It's very useful for drawing flicker free animations or low level special graphic effects.
		The framework will pass as parameter a [class]painter[/class]object. You don't need to begin/end/delete the painter.
		If you call "[cmd]return[/cmd] $true" you will stop the internal processing
		of this event.
		The default implementation does nothing.
		!fn: $setParent(<widget_object:parent>)
		Sets the parent of the widget to parent
		!fn: $setWindowIcon(<image_id>)
		Sets the icon for this widget. This is meaningful only for toplevel widgets.
		See the [doc:image_id]image identifier[/doc] documentation for the explanation
		of the <image_id> parameter.
		!fn: $setBackgroundImage(<image_id>)
		Sets the background image for this widget.
		See the [doc:image_id]image identifier[/doc] documentation for the explanation
		of the <image_id> parameter.
		For some kind of widgets, setting a background pixmap may have no effect or
		have strange results. Experiment with it.
		To unset the background image call [classfnc]$setBackgroundColor[/classfnc]
		!fn: $setFont(<family:string>,<size:integer>[,<style1:string>, <style2:string>, ...])
		Set the font's family,size and style, valid flag for style are:[br]
		- italic, bold, underline, overline, strikeout, fixedpitch  [br]
		!fn: $setWFlags(<flag1>, <flag2>, ...)
		This function sets widget flags, given as parameters.
		Valid flags are:
		[pre]
			TopLevel        - indicates that this widget is a top-level widget
			Dialog          - indicates that this widget is a top-level window that should be decorated as a dialog
			Desktop         - indicates that this widget is the desktop
			Popup           - indicates that this widget is a popup top-level window
			Title           - gives the window a title bar
			StaysOnTop      - window stays on top
			SysMenu         - add a windows system menu
			Minimize        - add a minimize button for the sysmenu style
			Maximize        - add a maximize button for the sysmenu style
		[/pre]
		!fn: $centerToScreen()
		Centers the window on the screen (useful only for toplevel widgets).[br]
		The best place to call this function is in $showEvent()
		!fn: $setFocusPolicy(<key focus>)
		Sets the way the widget accepts keyboard focus.[br]
		Valid parameters are:
		[pre]
			- TabFocus;	(widget accepts keyboard focus by tabbing)
			- ClickFocus; (widget accepts keyboard focus by clicking)
			- StrongFocus; (widget accepts both tabbing/clicking)
			- NoFocus;  (widget does not accept focus at all; this is the default value)
		[/pre]
		!fn: $keyPressEvent(<key>)
		If widget accepts keyboard focus (see [classfnc]$setFocusPolicy[/classfnc] ) this function handles for keys;
		In its argument the key pressed.[br]
		Special keys are:
		[pre]
			- Return
			- Enter
			- Down (cursor arrow down)
			- Up   (cursor arrow up)
			- Left (cursor arrow left)
			- Right (cursor arrow right)
			- Shift
			- Ctrl
			- Alt
			- CapsLock
			- Backspace
			- Del
			- Esc
			- 0
			- 1
			- 2
			- 3
			- 4
			- 5
			- 6
			- 7
			- 8
			- 9
			- +
			- -
			- *
			- /
			- (
			- )
			- =
			- .
			- ^
		[/pre]
		!fn: $mapFromGlobal(<x>,<y>)
		Translates the global screen coordinate pos to widget coordinates.
		!fn: $mapToGlobal(<x>,<y>)
		Translates widget coordinates into the global screen coordinate pos.
		!fn: integer $globalCursorX()
		Return the x coordinate of mouse pointer global position.
		!fn: integer $globalCursorY()
		Return the y coordinate of the mo>use pointer global position.
		!fn: <tip:string> $maybeTipEvent(<x_tip_pos:integer>,<y_tip_pos:integer>)
		This event handler is called when an eventual tip is going to be show.
		You can be reimplement this event and set a dynamic tool tip by using [i][cmd]return[/cmd] <tooltip_string>[/i].
		If a tooltip was set with [classfnc]$setTooltip[/classfnc] the dynamic tooltip will be ignored.
		The default implementation does nothing.
		!fn: integer $setAttribute(<string>,<bool_flag>)
		Sets the attribute attribute on this widget if on is true; otherwise clears the attribute.[br]
		Valid attributes are:[br]
		[pre]
			OpaquePaintEvent - Indicates that the widget paints all its pixels when it receives a paint event.
			NoSystemBackground - Indicates that the widget has no background, i.e. when the widget receives paint events, the background is not automatically repainted.
			PaintOnScreen - Indicates that the widget wants to draw directly onto the screen.
			NoMousePropagation - Prohibits mouse events from being propagated to the widget's parent.
		[/pre]
		!fn: $setStyleSheet(<string>)
		Set a style sheet for this widget.
		!fn: $customContextMenuRequestedEvent(<x_mouse_pos:integer>,<y_mouse_pos:integer>)
		This event is triggered when the user has requested a context menu on the widget (i.e. right clicking on the widget).
		The x,y coordinates are widget relative.
		!fn: $array(<red:integer,green:integer,blue:integer) $colorPalette(<color_role:string><color_group:string>)
		Returns the color in color_group(disabled, active or inactive), used for color_role.[br]
		Valid color role are:
		[pre]
			Window - A general background color.
			WindowText - A general foreground color.
			Base - Used as the background color for text.
			Text - Used as foreground color for the text.
			Button - The general button background color.
			ButtonText - A foreground color used with the Button color.
			Highlight - A color to indicate a selected item or the current item.
			HighlightedText - A text color that contrasts with Highlight.
		[/pre]
		!fn: <short_cut_id:integer> $setKeyShortCut(<key:char>)
		Adds a shortcut with key. A [classfnc]$shortCutEvent[/classfnc] will be triggered when the user will press alt+key.
		!fn: $shortCutEvent(<shortcut_id:integer>)
		This function will be called when a shortcut key has been triggered.
		You must reimplement this event to manage a shortcut system in a custom widget.
		The default implementation does nothing.
		!fn: $insertIntoStatusBar(<index:unsigned integer>)
		Insert the widget into the statusbar at the given index.
		If index is out of range, the widget is appended.
		See also [classfnc]$removeFromStatusBar[/classfnc]().
		!fn: $removeFromStatusBar()
		Remove the widget from statusbar.
	@examples:
		[example]
			%widget = $new(widget)
			[comment]# This is the main container for other elements.[/comment]

			%widget->$setWindowTitle("This is the widget title")

			%box=$new(vbox,%widget)
			%layout=$new(layout,%widget)
			%layout->$addWidget(%box,0,0)

			%label = $new(label,%box)
			%label->$setText("This is a text label")

			%lineedit = $new(lineedit,%box)
			%lineedit->$setText("This is a lineedit")

			%multilineedit = $new(multilineedit,%box)
			%multilineedit->$setText("This is a multilineedit")

			%checkbox = $new(checkbox,%box)
			%checkbox->$setText("This is a checkbox")

			%combobox = $new(combobox,%box)
						%combobox->$insertItem("This is a combobox")
						%combobox->$insertItem("This is a combobox's item")

			%listbox = $new(listbox,%box)
						%listbox->$insertItem("This is a listbox item")
						%listbox->$insertItem("This is another item")

			%radiobutton = $new(radiobutton,%box)
			%radiobutton->$setText("This is a radiobutton")

			%button = $new(button, %box)
			%button->$setText("Close me")

			privateimpl(%widget,closeMe)
			{
				delete $$
			}
			objects.connect  %button clicked %widget closeMe

			%widget->$show()
		[/example]
*/

//
// If you never call c->error(), c->warning() (your function never fails)
// and never call a function that can call c->error() or c->warning()
// you can avoid ENTER_STACK_FRAME and c->leaveStackFrame()
// Just return true.
// c->error() should be called only in really critical situations
// (we have to define better "critical situation")
// if possible, leave the things go after just a c->warning()
//

// widget() will be never 0 in THIS class
// but in derived classes WILL be
// ... now that I think about it, it
// may happen that widget() will be zero here too:
// If the Qt "physical" widget gets destroyed
// by some external factor (for example when child
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
//   if widget() is 0, the functions perform no actions
//   return no errors and results that have to be assumed
//   as invalid (this allows the minimum overhead: if widget()

KVSO_BEGIN_REGISTERCLASS(KvsObject_widget, "widget", "object")
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setParent)
// apparence
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, show)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, hide)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, update)

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, repaint)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, isTopLevel)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, isVisible)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, raise)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, lower)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setEnabled)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, isEnabled)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setMouseTracking)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setWindowTitle)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setWindowTitle)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, windowTitle)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, windowTitle)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, hasFocus)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setFocus)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setToolTip)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setWFlags)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setWindowIcon)
// fonts
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, fontDescent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, fontAscent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, fontMetricsLineSpacing)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, fontMetricsWidth)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, fontMetricsHeight)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setFont)
// geometry
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, screenResolution)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, centerToScreen)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, geometry)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setGeometry)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, mapToGlobal)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, mapFromGlobal)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, y)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, x)

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, globalCursorX)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, globalCursorY)

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, width)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, height)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setMinimumWidth)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setMinimumHeight)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setMaximumWidth)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setMaximumHeight)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setFixedWidth)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setFixedHeight)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setFixedSize)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setSizePolicy)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, move)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, sizeHint)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, resize)

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setFocusPolicy)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, addWidgetToWrappedLayout)

// colors and image
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setForegroundColor)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setBackgroundColor)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setBackgroundImage)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, backgroundColor)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, foregroundColor)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setMask)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setAttribute)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, colorPalette)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setStyleSheet)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, setKeyShortcut)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, grab)
// statusbar
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, insertIntoStatusBar)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_widget, removeFromStatusBar)
// events
KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KvsObject_widget, "mousePressEvent")
KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KvsObject_widget, "mouseReleaseEvent")
KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KvsObject_widget, "mouseDoubleClickEvent")
KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KvsObject_widget, "mouseMoveEvent")
KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KvsObject_widget, "focusInEvent")
KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KvsObject_widget, "focusOutEvent")
KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KvsObject_widget, "mouseLeaveEvent")
KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KvsObject_widget, "mouseEnterEvent")
KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KvsObject_widget, "showEvent")
KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KvsObject_widget, "hideEvent")
KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KvsObject_widget, "closeEvent")
KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KvsObject_widget, "resizeEvent")
KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KvsObject_widget, "moveEvent")
KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KvsObject_widget, "paintEvent")
KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KvsObject_widget, "keyPressEvent")
KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KvsObject_widget, "sizeHintRequestEvent")
KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KvsObject_widget, "maybeTipEvent")
KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KvsObject_widget, "shortCutEvent")
KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KvsObject_widget, "customContextMenuRequestedEvent")

KVSO_END_REGISTERCLASS(KvsObject_widget)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_widget, KviKvsObject)
KVSO_END_CONSTRUCTOR(KvsObject_widget)

KVSO_BEGIN_DESTRUCTOR(KvsObject_widget)
emit aboutToDie();
KVSO_END_CONSTRUCTOR(KvsObject_widget)

bool KvsObject_widget::init(KviKvsRunTimeContext * c, KviKvsVariantList *)
{
	setObject(new KviKvsWidget(this, parentScriptWidget()));
	m_pContext = c;
	widget()->setObjectName(getName());
	return true;
}

bool KvsObject_widget::eventFilter(QObject * o, QEvent * e)
{
	if(o == object())
	{
		bool ret = false;
		int aparam;
		bool brokenhandler = false;
		KviKvsVariant oReturnBuffer(ret);

		switch(e->type())
		{
			case QEvent::ContextMenu:
			{
				QPoint iPoint = widget()->mapFromGlobal(((QContextMenuEvent *)e)->globalPos());
				KviKvsVariantList params(new KviKvsVariant((kvs_int_t)iPoint.x()), new KviKvsVariant((kvs_int_t)iPoint.y()));
				callFunction(this, "customContextMenuRequestedEvent", &oReturnBuffer, &params);
				break;
			}
			case QEvent::Shortcut:
			{
				KviKvsVariantList params(new KviKvsVariant((kvs_int_t)((QShortcutEvent *)e)->shortcutId()));
				callFunction(this, "shortCutEvent", &oReturnBuffer, &params);
				break;
			}
			case QEvent::ToolTip:
			{
				QHelpEvent * helpEvent = static_cast<QHelpEvent *>(e);
				QPoint point = helpEvent->pos();
				QString szTooltip = "";
				KviKvsVariant * tipret = new KviKvsVariant(szTooltip);
				KviKvsVariantList params(new KviKvsVariant((kvs_int_t)point.x()), new KviKvsVariant((kvs_int_t)point.y()));
				callFunction(this, "maybeTipEvent", tipret, &params);
				tipret->asString(szTooltip);
				if(!szTooltip.isEmpty())
					QToolTip::showText(helpEvent->globalPos(), szTooltip);
				break;
			}
			case QEvent::Paint:
			{
				QRect rect = ((QPaintEvent *)e)->rect();
				KviKvsObjectClass * pClass = KviKvsKernel::instance()->objectController()->lookupClass("painter");
				KviKvsVariantList params;
				KviKvsObject * pObject = pClass->allocateInstance(nullptr, "internalpainter", m_pContext, &params);
				QPainter p(widget());
				((KvsObject_painter *)pObject)->setInternalPainter(&p);
				p.setClipRect(rect);
				kvs_hobject_t hobj = pObject->handle();
				KviKvsVariantList params2(new KviKvsVariant(hobj), new KviKvsVariant((kvs_int_t)rect.x()), new KviKvsVariant((kvs_int_t)rect.y()), new KviKvsVariant((kvs_int_t)rect.width()), new KviKvsVariant((kvs_int_t)rect.height()));
				callFunction(this, "paintEvent", &oReturnBuffer, &params2);
				pObject = KviKvsKernel::instance()->objectController()->lookupObject(hobj);
				if(pObject)
					pObject->dieNow();
				break;
			}

			case QEvent::KeyPress:
			{
				QString tmp = "";
				switch(((QKeyEvent *)e)->key())
				{
					case Qt::Key_Return:
						tmp = "Return";
						break;
					case Qt::Key_Down:
						tmp = "Down";
						break;
					case Qt::Key_Up:
						tmp = "Up";
						break;
					case Qt::Key_Left:
						tmp = "Left";
						break;
					case Qt::Key_Right:
						tmp = "Right";
						break;
					case Qt::Key_Shift:
						tmp = "Shift";
						break;
					case Qt::Key_Control:
						tmp = "Ctrl";
						break;
					case Qt::Key_Alt:
						tmp = "Alt";
						break;
					case Qt::Key_CapsLock:
						tmp = "CapsLock";
						break;
					case Qt::Key_Backspace:
						tmp = "Backspace";
						break;
					case Qt::Key_Delete:
						tmp = "Del";
						break;
					case Qt::Key_Enter:
						tmp = "Enter";
						break;
					case Qt::Key_Escape:
						tmp = "Esc";
						break;
					case Qt::Key_0:
						tmp = "0";
						break;
					case Qt::Key_1:
						tmp = "1";
						break;
					case Qt::Key_2:
						tmp = "2";
						break;
					case Qt::Key_3:
						tmp = "3";
						break;
					case Qt::Key_4:
						tmp = "4";
						break;
					case Qt::Key_5:
						tmp = "5";
						break;
					case Qt::Key_6:
						tmp = "6";
						break;
					case Qt::Key_7:
						tmp = "7";
						break;
					case Qt::Key_8:
						tmp = "8";
						break;
					case Qt::Key_9:
						tmp = "9";
						break;
					case Qt::Key_Plus:
						tmp = "+";
						break;
					case Qt::Key_Minus:
						tmp = "-";
						break;
					case Qt::Key_Slash:
						tmp = "/";
						break;
					case Qt::Key_Asterisk:
						tmp = "*";
						break;
					case Qt::Key_Period:
						tmp = ".";
						break;
					case Qt::Key_ParenLeft:
						tmp = "(";
						break;
					case Qt::Key_ParenRight:
						tmp = ")";
						break;
					case Qt::Key_Equal:
						tmp = "=";
						break;
					case Qt::Key_AsciiCircum:
						tmp = "^";
						break;
					default:
						if(!((QKeyEvent *)e)->text().isEmpty())
							tmp = ((QKeyEvent *)e)->text();
				}

				KviKvsVariantList params(new KviKvsVariant(tmp));
				callFunction(this, "keyPressEvent", nullptr, &params);
			}
			break;
			case QEvent::MouseButtonPress:
			{
				if(((QMouseEvent *)e)->button() & Qt::LeftButton)
					aparam = 0;
				else
				{
					if(((QMouseEvent *)e)->button() & Qt::RightButton)
						aparam = 1;
					else
						aparam = 2;
				}
				KviKvsVariantList lParams;
				lParams.append(new KviKvsVariant((kvs_int_t)aparam));
				lParams.append(new KviKvsVariant((kvs_int_t)((QMouseEvent *)e)->pos().x()));
				lParams.append(new KviKvsVariant((kvs_int_t)((QMouseEvent *)e)->pos().y()));
				if(!callFunction(this, "mousePressEvent", &oReturnBuffer, &lParams))
					brokenhandler = true; // ignore results of a broken event handler
			}
			break;
			case QEvent::MouseButtonRelease:
			{
				if(((QMouseEvent *)e)->button() & Qt::LeftButton)
					aparam = 0;
				else
				{
					if(((QMouseEvent *)e)->button() & Qt::RightButton)
						aparam = 1;
					else
						aparam = 2;
				}
				KviKvsVariantList lParams;
				lParams.append(new KviKvsVariant((kvs_int_t)aparam));
				lParams.append(new KviKvsVariant((kvs_int_t)((QMouseEvent *)e)->pos().x()));
				lParams.append(new KviKvsVariant((kvs_int_t)((QMouseEvent *)e)->pos().y()));
				if(!callFunction(this, "mouseReleaseEvent", &oReturnBuffer, &lParams))
					brokenhandler = true; // ignore results of a broken event handler
			}
			break;
			case QEvent::MouseButtonDblClick:
			{
				if(((QMouseEvent *)e)->button() & Qt::LeftButton)
					aparam = 0;
				else
				{
					if(((QMouseEvent *)e)->button() & Qt::RightButton)
						aparam = 1;
					else
						aparam = 2;
				}
				KviKvsVariantList lParams;
				lParams.append(new KviKvsVariant((kvs_int_t)aparam));
				lParams.append(new KviKvsVariant((kvs_int_t)((QMouseEvent *)e)->pos().x()));
				lParams.append(new KviKvsVariant((kvs_int_t)((QMouseEvent *)e)->pos().y()));
				if(!callFunction(this, "mouseDoubleClickEvent", nullptr, &lParams))
					brokenhandler = true; // ignore results of a broken event handler
			}

			break;
			case QEvent::MouseMove:
			{
				KviKvsVariantList lParams;
				lParams.append(new KviKvsVariant((kvs_int_t)((QMouseEvent *)e)->pos().x()));
				lParams.append(new KviKvsVariant((kvs_int_t)((QMouseEvent *)e)->pos().y()));
				if(!callFunction(this, "mouseMoveEvent", nullptr, &lParams))
					brokenhandler = true; // ignore results of a broken event handler
			}
			break;
			case QEvent::FocusIn:
				if(!callFunction(this, "focusInEvent", &oReturnBuffer, nullptr))
					brokenhandler = true;
				break;
			case QEvent::FocusOut:
				if(!callFunction(this, "focusOutEvent", &oReturnBuffer, nullptr))
					brokenhandler = true;
				break;
			case QEvent::Resize:
			{
				KviKvsVariantList lParams;
				lParams.append(new KviKvsVariant((kvs_int_t)((QResizeEvent *)e)->size().width()));
				lParams.append(new KviKvsVariant((kvs_int_t)((QResizeEvent *)e)->size().height()));
				if(!callFunction(this, "resizeEvent", &oReturnBuffer, &lParams))
					brokenhandler = true;
				break;
			}
			case QEvent::Move:
				if(!callFunction(this, "moveEvent", &oReturnBuffer, nullptr))
					brokenhandler = true;
				break;
			case QEvent::Close:
				if(!callFunction(this, "closeEvent", &oReturnBuffer, nullptr))
					brokenhandler = true;
				break;
			case QEvent::Enter:
				if(!callFunction(this, "mouseEnterEvent", &oReturnBuffer, nullptr))
					brokenhandler = true;
				break;
			case QEvent::Leave:
				if(!callFunction(this, "mouseLeaveEvent", &oReturnBuffer, nullptr))
					brokenhandler = true;
				break;
			case QEvent::Show:
				if(!callFunction(this, "showEvent", &oReturnBuffer, nullptr))
					brokenhandler = true;
				break;
			case QEvent::Hide:
				if(!callFunction(this, "hideEvent", &oReturnBuffer, nullptr))
					ret = false;
				break;
			default:
				return KviKvsObject::eventFilter(o, e);
				break;
		}
		if(!brokenhandler)
		{
			ret = oReturnBuffer.asBoolean();
			//qDebug ("Propagation %i",ret);

			return ret;
		}
	}
	return KviKvsObject::eventFilter(o, e);
}

KVSO_CLASS_FUNCTION(widget, show)
{
	CHECK_INTERNAL_POINTER(widget())
	QWidget * wid = widget();
	wid->show();
	return true;
}

KVSO_CLASS_FUNCTION(widget, update)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iX, iY, iW, iH;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x", KVS_PT_INT, KVS_PF_OPTIONAL, iX)
	KVSO_PARAMETER("y", KVS_PT_INT, KVS_PF_OPTIONAL, iY)
	KVSO_PARAMETER("w", KVS_PT_INT, KVS_PF_OPTIONAL, iW)
	KVSO_PARAMETER("h", KVS_PT_INT, KVS_PF_OPTIONAL, iH)
	KVSO_PARAMETERS_END(c)
	if(iW || iH)
		widget()->update(QRect(iX, iY, iW, iH));
	else
		widget()->update();
	return true;
}

KVSO_CLASS_FUNCTION(widget, setEnabled)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bEnabled", KVS_PT_BOOL, 0, bEnabled)
	KVSO_PARAMETERS_END(c)
	widget()->setEnabled(bEnabled);
	return true;
}

KVSO_CLASS_FUNCTION(widget, isEnabled)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setBoolean(widget()->isEnabled());
	return true;
}

KVSO_CLASS_FUNCTION(widget, fontDescent)
{
	CHECK_INTERNAL_POINTER(widget())
	QFontMetrics fm = widget()->fontMetrics();
	int d = fm.descent();
	c->returnValue()->setInteger(d);
	return true;
}

KVSO_CLASS_FUNCTION(widget, fontAscent)
{
	CHECK_INTERNAL_POINTER(widget())
	QFontMetrics fm = widget()->fontMetrics();
	int d = fm.ascent();
	c->returnValue()->setInteger(d);
	return true;
}

KVSO_CLASS_FUNCTION(widget, repaint)
{
	CHECK_INTERNAL_POINTER(widget())
	widget()->repaint();
	return true;
}

KVSO_CLASS_FUNCTION(widget, fontMetricsWidth)
{
	CHECK_INTERNAL_POINTER(widget())
	QString m_szStr;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("string", KVS_PT_STRING, 0, m_szStr)
	KVSO_PARAMETERS_END(c)
	c->returnValue()->setInteger(widget()->fontMetrics().width(m_szStr));
	return true;
}

KVSO_CLASS_FUNCTION(widget, fontMetricsHeight)
{
	CHECK_INTERNAL_POINTER(widget())
	int fm = widget()->fontMetrics().height();
	c->returnValue()->setInteger(fm);
	return true;
}
KVSO_CLASS_FUNCTION(widget, fontMetricsLineSpacing)
{
	CHECK_INTERNAL_POINTER(widget())
	int fm = widget()->fontMetrics().lineSpacing();
	c->returnValue()->setInteger(fm);
	return true;
}
KVSO_CLASS_FUNCTION(widget, screenResolution)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsArray * a = new KviKvsArray();
	QRect rect = g_pApp->desktop()->screenGeometry(g_pApp->desktop()->primaryScreen());
	a->set(0, new KviKvsVariant((kvs_int_t)rect.width()));
	a->set(1, new KviKvsVariant((kvs_int_t)rect.height()));
	c->returnValue()->setArray(a);
	return true;
}

KVSO_CLASS_FUNCTION(widget, geometry)
{
	CHECK_INTERNAL_POINTER(widget())
	QRect r = widget() ? widget()->geometry() : QRect(0, 0, 0, 0);
	KviKvsArray * a = new KviKvsArray();
	a->set(0, new KviKvsVariant((kvs_int_t)r.x()));
	a->set(1, new KviKvsVariant((kvs_int_t)r.y()));
	a->set(2, new KviKvsVariant((kvs_int_t)r.width()));
	a->set(3, new KviKvsVariant((kvs_int_t)r.height()));
	c->returnValue()->setArray(a);
	return true;
}

KVSO_CLASS_FUNCTION(widget, setGeometry)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsVariant * pXOrArray;
	kvs_int_t iX, iY, iW, iH;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x_or_array", KVS_PT_VARIANT, 0, pXOrArray)
	KVSO_PARAMETER("y", KVS_PT_INT, KVS_PF_OPTIONAL, iY)
	KVSO_PARAMETER("w", KVS_PT_INT, KVS_PF_OPTIONAL, iW)
	KVSO_PARAMETER("h", KVS_PT_INT, KVS_PF_OPTIONAL, iH)
	KVSO_PARAMETERS_END(c)

	// this is a little bit tricky: we accept two syntax versions
	// (this is something like a C++ overload)
	// $setGeometry(x,y,w,h) OR $setGeometry($array(x,y,w,h))

	// For this purpose we ask the parameter processing engine
	// to return the first parameter as variant and we manually check its data type
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
			c->error(__tr2qs_ctx("The array passed as parameter must contain at least 4 elements", "objects"));
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
			c->error(__tr2qs_ctx("One of the geometry array parameters is empty", "objects"));
			return false;
		}
		if(!(pX->asInteger(iX) && pY->asInteger(iY) && pW->asInteger(iW) && pH->asInteger(iH)))
		{
			c->error(__tr2qs_ctx("One of the geometry array parameters didn't evaluate to an integer", "objects"));
			return false;
		}
		// ok: the params are correct
	}
	else
	{
		// The user passed something else as first parameter
		// make sure that it is an integer.
		// Also make sure that we really have 4 parameters
		if(c->params()->count() < 4)
		{
			c->error(__tr2qs_ctx("$setGeometry() requires either an array as first parameter or four integers", "objects"));
			return false;
		}
		if(!pXOrArray->asInteger(iX))
		{
			c->error(__tr2qs_ctx("The first parameter didn't evaluate to an array nor an integer", "objects"));
			return false;
		}
		// ok: the params are correct
	}

	widget()->setGeometry(iX, iY, iW, iH);

	return true;
}

KVSO_CLASS_FUNCTION(widget, mapToGlobal)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iX, iY;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x", KVS_PT_INT, 0, iX)
	KVSO_PARAMETER("y", KVS_PT_INT, 0, iY)
	KVSO_PARAMETERS_END(c)
	QPoint point = widget()->mapToGlobal(QPoint(iX, iY));
	KviKvsArray * a = new KviKvsArray();
	a->set(0, new KviKvsVariant((kvs_int_t)point.x()));
	a->set(1, new KviKvsVariant((kvs_int_t)point.y()));
	c->returnValue()->setArray(a);
	return true;
}

KVSO_CLASS_FUNCTION(widget, mapFromGlobal)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iX, iY;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x", KVS_PT_INT, 0, iX)
	KVSO_PARAMETER("y", KVS_PT_INT, 0, iY)
	KVSO_PARAMETERS_END(c)
	QPoint point = widget()->mapFromGlobal(QPoint(iX, iY));
	KviKvsArray * a = new KviKvsArray();
	a->set(0, new KviKvsVariant((kvs_int_t)point.x()));
	a->set(1, new KviKvsVariant((kvs_int_t)point.y()));
	c->returnValue()->setArray(a);
	return true;
}

KVSO_CLASS_FUNCTION(widget, centerToScreen)
{
	CHECK_INTERNAL_POINTER(widget())
	QRect rect = g_pApp->desktop()->screenGeometry(g_pApp->desktop()->primaryScreen());
	widget()->move((rect.width() - widget()->width()) / 2, (rect.height() - widget()->height()) / 2);
	return true;
}

KVSO_CLASS_FUNCTION(widget, setForegroundColor)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsVariant * pColArray;
	kvs_int_t iColR, iColG, iColB;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("hex_rgb_array_or_red", KVS_PT_VARIANT, 0, pColArray)
	KVSO_PARAMETER("green", KVS_PT_INT, KVS_PF_OPTIONAL, iColG)
	KVSO_PARAMETER("blue", KVS_PT_INT, KVS_PF_OPTIONAL, iColB)
	KVSO_PARAMETERS_END(c)
	if(pColArray->isArray())
	{
		if(pColArray->array()->size() < 3)
		{
			c->error(__tr2qs_ctx("The array passed as parameter must contain at least 3 elements", "objects"));
			return false;
		}
		KviKvsVariant * pColR = pColArray->array()->at(0);
		KviKvsVariant * pColG = pColArray->array()->at(1);
		KviKvsVariant * pColB = pColArray->array()->at(2);

		if(!(pColR && pColG && pColB))
		{
			c->error(__tr2qs_ctx("One of the colors array parameters is empty", "objects"));
			return false;
		}

		if(!(pColR->asInteger(iColR) && pColG->asInteger(iColG) && pColB->asInteger(iColB)))
		{
			c->error(__tr2qs_ctx("One of the colors array parameters didn't evaluate to an integer", "objects"));
			return false;
		}
	}
	else
	{
		QColor color;
		if(c->params()->count() == 1)
		{
			if(pColArray->isString())
			{
				QString szColor;
				pColArray->asString(szColor);
				// maybe a color name?
				color.setNamedColor(szColor);
				if(!color.isValid())
				{
					// itsn't a color name: let try with an hex triplette
					color.setNamedColor("#" + szColor);
					if(!color.isValid())
					{
						c->warning(__tr2qs_ctx("Not a valid color!", "objects"));
						return true;
					}
				}
			}
			else
			{
				c->warning(__tr2qs_ctx("Not a valid color!", "objects"));
				return true;
			}
			QPalette p = widget()->palette();
			p.setColor(widget()->foregroundRole(), color);
			widget()->setPalette(p);
			return true;
		}
		if(c->params()->count() < 3)
		{
			c->error(__tr2qs_ctx("$setForegroundColor requires either an array as first parameter, one hex string or color name, or three integers", "objects"));
			return false;
		}
		if(!pColArray->asInteger(iColR))
		{
			c->error(__tr2qs_ctx("The first parameter didn't evaluate to an array nor an integer", "objects"));
			return false;
		}
	}

	QPalette p = widget()->palette();
	p.setColor(widget()->foregroundRole(), QColor(iColR, iColG, iColB));
	widget()->setPalette(p);
	return true;
}

KVSO_CLASS_FUNCTION(widget, setBackgroundColor)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsVariant * pColArray;
	kvs_int_t iColR, iColG, iColB;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("name_hex_rgb_array_or_red", KVS_PT_VARIANT, 0, pColArray)
	KVSO_PARAMETER("green", KVS_PT_INT, KVS_PF_OPTIONAL, iColG)
	KVSO_PARAMETER("blue", KVS_PT_INT, KVS_PF_OPTIONAL, iColB)
	KVSO_PARAMETERS_END(c)
	if(pColArray->isArray())
	{
		if(pColArray->array()->size() < 3)
		{
			c->error(__tr2qs_ctx("The array passed as parameter must contain at least 3 elements", "objects"));
			return false;
		}
		KviKvsVariant * pColR = pColArray->array()->at(0);
		KviKvsVariant * pColG = pColArray->array()->at(1);
		KviKvsVariant * pColB = pColArray->array()->at(2);

		if(!(pColR && pColG && pColB))
		{
			c->error(__tr2qs_ctx("One of the colors array parameters is empty", "objects"));
			return false;
		}

		if(!(pColR->asInteger(iColR) && pColG->asInteger(iColG) && pColB->asInteger(iColB)))
		{
			c->error(__tr2qs_ctx("One of the colors array parameters didn't evaluate to an integer", "objects"));
			return false;
		}
	}
	else
	{
		QColor color;
		if(c->params()->count() == 1)
		{
			if(pColArray->isString())
			{
				QString szColor;
				pColArray->asString(szColor);
				// maybe a color name?
				color.setNamedColor(szColor);
				if(!color.isValid())
				{
					// itsn't a color name: let try with an hex triplette
					color.setNamedColor("#" + szColor);
					if(!color.isValid())
					{
						c->warning(__tr2qs_ctx("Not a valid color!", "objects"));
						return true;
					}
				}
			}
			else
			{
				c->warning(__tr2qs_ctx("Not a valid color!", "objects"));
				return true;
			}
			QPalette p = widget()->palette();
			p.setColor(widget()->backgroundRole(), color);
			widget()->setPalette(p);
			return true;
		}
		if(c->params()->count() < 3)
		{
			c->error(__tr2qs_ctx("$setBackgroundColor requires either an array as first parameter, one hex string or color name, or three integers", "objects"));
			return false;
		}
		if(!pColArray->asInteger(iColR))
		{
			c->error(__tr2qs_ctx("The first parameter didn't evaluate to an array nor an integer", "objects"));
			return false;
		}
	}

	QPalette p = widget()->palette();
	p.setColor(widget()->backgroundRole(), QColor(iColR, iColG, iColB));
	widget()->setPalette(p);
	return true;
}

KVSO_CLASS_FUNCTION(widget, backgroundColor)
{
	CHECK_INTERNAL_POINTER(widget())
	QColor col = widget()->palette().color(widget()->backgroundRole());
	KviKvsArray * a = new KviKvsArray();
	a->set(0, new KviKvsVariant((kvs_int_t)col.red()));
	a->set(1, new KviKvsVariant((kvs_int_t)col.green()));
	a->set(2, new KviKvsVariant((kvs_int_t)col.blue()));
	c->returnValue()->setArray(a);
	return true;
}

KVSO_CLASS_FUNCTION(widget, foregroundColor)
{
	CHECK_INTERNAL_POINTER(widget())
	QColor col = widget()->palette().color(widget()->foregroundRole());
	KviKvsArray * a = new KviKvsArray();
	a->set(0, new KviKvsVariant((kvs_int_t)col.red()));
	a->set(1, new KviKvsVariant((kvs_int_t)col.green()));
	a->set(2, new KviKvsVariant((kvs_int_t)col.blue()));
	c->returnValue()->setArray(a);
	return true;
}

KVSO_CLASS_FUNCTION(widget, parentWidget)
{
	CHECK_INTERNAL_POINTER(widget())
	if(parentScriptWidget())
		c->returnValue()->setHObject(parentObject()->handle());
	else
		c->returnValue()->setHObject((kvs_hobject_t) nullptr);
	return true;
}

KVSO_CLASS_FUNCTION(widget, setMouseTracking)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bEnabled", KVS_PT_BOOL, 0, bEnabled)
	KVSO_PARAMETERS_END(c)
	widget()->setMouseTracking(bEnabled);
	return true;
}

KVSO_CLASS_FUNCTION(widget, setWindowTitle)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szWindowTitle;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("title", KVS_PT_STRING, 0, szWindowTitle)
	KVSO_PARAMETERS_END(c)
	widget()->setWindowTitle(szWindowTitle);
	return true;
}

KVSO_CLASS_FUNCTION(widget, windowTitle)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setString(widget()->windowTitle().toUtf8().data());
	return true;
}

KVSO_CLASS_FUNCTION(widget, isTopLevel)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setBoolean(widget()->isTopLevel());
	return true;
}

KVSO_CLASS_FUNCTION(widget, isVisible)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setBoolean(widget()->isVisible());
	return true;
}

KVSO_CLASS_FUNCTION(widget, raise)
{
	CHECK_INTERNAL_POINTER(widget())
	widget()->raise();
	return true;
}

KVSO_CLASS_FUNCTION(widget, lower)
{
	CHECK_INTERNAL_POINTER(widget())
	widget()->lower();
	return true;
}

KVSO_CLASS_FUNCTION(widget, hasFocus)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setBoolean(widget()->hasFocus());
	return true;
}

KVSO_CLASS_FUNCTION(widget, setFocus)
{
	CHECK_INTERNAL_POINTER(widget())
	widget()->setFocus();
	return true;
}

KVSO_CLASS_FUNCTION(widget, hide)
{
	CHECK_INTERNAL_POINTER(widget())
	widget()->hide();
	return true;
}

KVSO_CLASS_FUNCTION(widget, setToolTip)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szTooltip;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("tooltip", KVS_PT_STRING, 0, szTooltip)
	KVSO_PARAMETERS_END(c)
	widget()->setToolTip(szTooltip);
	return true;
}

KVSO_CLASS_FUNCTION(widget, x)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(widget()->x());
	return true;
}

KVSO_CLASS_FUNCTION(widget, y)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(widget()->y());
	return true;
}

KVSO_CLASS_FUNCTION(widget, width)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(widget()->width());
	return true;
}

KVSO_CLASS_FUNCTION(widget, height)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(widget()->height());
	return true;
}

KVSO_CLASS_FUNCTION(widget, setMinimumWidth)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iW;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("w", KVS_PT_INT, 0, iW)
	KVSO_PARAMETERS_END(c)
	widget()->setMinimumWidth(iW);
	return true;
}

KVSO_CLASS_FUNCTION(widget, setMinimumHeight)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iH;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("h", KVS_PT_INT, 0, iH)
	KVSO_PARAMETERS_END(c)
	widget()->setMinimumHeight(iH);
	return true;
}

KVSO_CLASS_FUNCTION(widget, setMaximumWidth)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iW;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("w", KVS_PT_INT, 0, iW)
	KVSO_PARAMETERS_END(c)
	widget()->setMaximumWidth(iW);
	return true;
}

KVSO_CLASS_FUNCTION(widget, setMaximumHeight)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iH;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("h", KVS_PT_INT, 0, iH)
	KVSO_PARAMETERS_END(c)
	widget()->setMaximumHeight(iH);
	return true;
}

KVSO_CLASS_FUNCTION(widget, setFixedWidth)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iW;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("w", KVS_PT_INT, 0, iW)
	KVSO_PARAMETERS_END(c)
	widget()->setFixedWidth(iW);
	return true;
}

KVSO_CLASS_FUNCTION(widget, setFixedHeight)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iH;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("h", KVS_PT_INT, 0, iH)
	KVSO_PARAMETERS_END(c)
	widget()->setFixedHeight(iH);
	return true;
}

KVSO_CLASS_FUNCTION(widget, setFixedSize)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iW, iH;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("w", KVS_PT_INT, 0, iW)
	KVSO_PARAMETER("h", KVS_PT_INT, 0, iH)
	KVSO_PARAMETERS_END(c)
	widget()->setFixedSize(iW, iH);
	return true;
}

KVSO_CLASS_FUNCTION(widget, setSizePolicy)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szHorizontal, szVertical;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("h", KVS_PT_STRING, 0, szHorizontal)
	KVSO_PARAMETER("v", KVS_PT_STRING, 0, szVertical)
	KVSO_PARAMETERS_END(c)

	QSizePolicy::Policy hPolicy = QSizePolicy::Preferred;
	if(KviQString::equalCI(szHorizontal, "Fixed"))
		hPolicy = QSizePolicy::Fixed;
	else if(KviQString::equalCI(szHorizontal, "Minimum"))
		hPolicy = QSizePolicy::Minimum;
	else if(KviQString::equalCI(szHorizontal, "Maximum"))
		hPolicy = QSizePolicy::Maximum;
	else if(KviQString::equalCI(szHorizontal, "Preferred"))
		hPolicy = QSizePolicy::Preferred;
	else if(KviQString::equalCI(szHorizontal, "Expanding"))
		hPolicy = QSizePolicy::Expanding;
	else if(KviQString::equalCI(szHorizontal, "MinimumExpanding"))
		hPolicy = QSizePolicy::MinimumExpanding;
	else if(KviQString::equalCI(szHorizontal, "Ignored"))
		hPolicy = QSizePolicy::Ignored;
	else
		c->warning(__tr2qs_ctx("Unknown policy '%Q'", "objects"), &szHorizontal);

	QSizePolicy::Policy vPolicy = QSizePolicy::Preferred;
	if(KviQString::equalCI(szVertical, "Fixed"))
		vPolicy = QSizePolicy::Fixed;
	else if(KviQString::equalCI(szVertical, "Minimum"))
		vPolicy = QSizePolicy::Minimum;
	else if(KviQString::equalCI(szVertical, "Maximum"))
		vPolicy = QSizePolicy::Maximum;
	else if(KviQString::equalCI(szVertical, "Preferred"))
		vPolicy = QSizePolicy::Preferred;
	else if(KviQString::equalCI(szVertical, "Expanding"))
		vPolicy = QSizePolicy::Expanding;
	else if(KviQString::equalCI(szVertical, "MinimumExpanding"))
		vPolicy = QSizePolicy::MinimumExpanding;
	else if(KviQString::equalCI(szVertical, "Ignored"))
		vPolicy = QSizePolicy::Ignored;
	else
		c->warning(__tr2qs_ctx("Unknown policy '%Q'", "objects"), &szVertical);

	widget()->setSizePolicy(hPolicy, vPolicy);

	return true;
}

KVSO_CLASS_FUNCTION(widget, move)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iX, iY;
	KviKvsVariant * pXOrArray;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("x_or_array", KVS_PT_VARIANT, 0, pXOrArray)
	KVSO_PARAMETER("y", KVS_PT_INT, KVS_PF_OPTIONAL, iY)
	KVSO_PARAMETERS_END(c)
	if(pXOrArray->isArray())
	{
		if(pXOrArray->array()->size() < 2)
		{
			c->error(__tr2qs_ctx("The array passed as parameter must contain at least 2 elements", "objects"));
			return false;
		}
		KviKvsVariant * pX = pXOrArray->array()->at(0);
		KviKvsVariant * pY = pXOrArray->array()->at(1);
		if(!(pX && pY))
		{
			c->error(__tr2qs_ctx("One of the move array parameters is empty", "objects"));
			return false;
		}
		if(!(pX->asInteger(iX) && pY->asInteger(iY)))
		{
			c->error(__tr2qs_ctx("One of the move array parameters didn't evaluate to an integer", "objects"));
			return false;
		}
		// ok: the params are correct
	}
	else
	{
		if(c->params()->count() < 2)
		{
			c->error(__tr2qs_ctx("$move() requires either an array as first parameter or two integers", "objects"));
			return false;
		}
		if(!pXOrArray->asInteger(iX))
		{
			c->error(__tr2qs_ctx("The first parameter didn't evaluate to an array nor an integer", "objects"));
			return false;
		}
		// ok: the params are correct
	}
	widget()->move(QPoint(iX, iY));
	return true;
}

KVSO_CLASS_FUNCTION(widget, sizeHint)
{
	CHECK_INTERNAL_POINTER(widget())
	QSize sizehint = widget()->sizeHint();
	KviKvsArray * a = new KviKvsArray();
	a->set(0, new KviKvsVariant((kvs_int_t)sizehint.width()));
	a->set(1, new KviKvsVariant((kvs_int_t)sizehint.height()));
	c->returnValue()->setArray(a);
	return true;
}

KVSO_CLASS_FUNCTION(widget, resize)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsVariant * pWOrArray;
	kvs_int_t iW, iH;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("width_or_array", KVS_PT_VARIANT, 0, pWOrArray)
	KVSO_PARAMETER("height", KVS_PT_INT, KVS_PF_OPTIONAL, iH)
	KVSO_PARAMETERS_END(c)
	if(pWOrArray->isArray())
	{
		if(pWOrArray->array()->size() < 2)
		{
			c->error(__tr2qs_ctx("The array passed as parameter must contain at least 2 elements", "objects"));
			return false;
		}
		KviKvsVariant * pW = pWOrArray->array()->at(0);
		KviKvsVariant * pH = pWOrArray->array()->at(1);
		if(!(pW && pH))
		{
			c->error(__tr2qs_ctx("One of the resize array parameters is empty", "objects"));
			return false;
		}
		if(!(pW->asInteger(iW) && pH->asInteger(iH)))
		{
			c->error(__tr2qs_ctx("One of the resize array parameters didn't evaluate to an integer", "objects"));
			return false;
		}
		// ok: the params are correct
	}
	else
	{
		if(c->params()->count() < 2)
		{
			c->error(__tr2qs_ctx("$resize() requires either an array as first parameter or two integers", "objects"));
			return false;
		}
		if(!pWOrArray->asInteger(iW))
		{
			c->error(__tr2qs_ctx("The first parameter didn't evaluate to an array nor an integer", "objects"));
			return false;
		}
		// ok: the params are correct
	}
	widget()->resize(QSize(iW, iH));
	return true;
}

KVSO_CLASS_FUNCTION(widget, setFocusPolicy)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szMode;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("focus", KVS_PT_STRING, 0, szMode)
	KVSO_PARAMETERS_END(c)
	if(KviQString::equalCI(szMode, "TabFocus"))
		widget()->setFocusPolicy(QT_WIDGET_TABFOCUS);
	else if(KviQString::equalCI(szMode, "ClickFocus"))
		widget()->setFocusPolicy(QT_WIDGET_CLICKFOCUS);
	else if(KviQString::equalCI(szMode, "StrongFocus"))
		widget()->setFocusPolicy(QT_WIDGET_STRONGFOCUS);
	else if(KviQString::equalCI(szMode, "NoFocus"))
		widget()->setFocusPolicy(QT_WIDGET_NOFOCUS);
	else
		c->warning(__tr2qs_ctx("Invalid parameters", "objects"));
	return true;
}

KVSO_CLASS_FUNCTION(widget, setWFlags)
{
	CHECK_INTERNAL_POINTER(widget())
	QStringList wflags;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("widget_flags", KVS_PT_STRINGLIST, KVS_PF_OPTIONAL, wflags)
	KVSO_PARAMETERS_END(c)
	Qt::WindowFlags flag, sum = nullptr;
	for(auto & it : wflags)
	{
		flag = nullptr;
		for(size_t j{}; j < widgettypes_num; j++)
		{
			if(KviQString::equalCI(it, widgettypes_tbl[j]))
			{
				flag = widgettypes_cod[j];
				break;
			}
		}
		if(flag)
			sum = sum | flag;
		else
			c->warning(__tr2qs_ctx("Unknown widget flag '%Q'", "objects"), &it);
	}
	widget()->setWindowFlags(sum);
	return true;
}

KVSO_CLASS_FUNCTION(widget, setFont)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szFamily;
	QStringList szListStyle;
	kvs_int_t iSize;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("family", KVS_PT_STRING, 0, szFamily)
	KVSO_PARAMETER("size", KVS_PT_INTEGER, 0, iSize)
	KVSO_PARAMETER("style", KVS_PT_STRINGLIST, KVS_PF_OPTIONAL, szListStyle)
	KVSO_PARAMETERS_END(c)
	QFont font = widget()->font();
	if(!szFamily.isEmpty())
		font.setFamily(szFamily);
	if(iSize)
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
	widget()->setFont(font);
	return true;
}
KVSO_CLASS_FUNCTION(widget, addWidgetToWrappedLayout)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject * ob;
	kvs_uint_t uCol, uRow;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("widget", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETER("col", KVS_PT_UNSIGNEDINTEGER, 0, uCol)
	KVSO_PARAMETER("row", KVS_PT_UNSIGNEDINTEGER, 0, uRow)
	KVSO_PARAMETERS_END(c)
	ob = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!ob)
	{
		c->warning(__tr2qs_ctx("Widget parameter is not an object", "objects"));
		return true;
	}
	if(!ob->object())
	{
		c->warning(__tr2qs_ctx("Widget parameter is not a valid object", "objects"));
		return true;
	}
	QLayout * lay = widget()->layout();
	if(!lay)
	{
		c->warning(__tr2qs_ctx("No Layout associated to the widget ", "objects"));
		return true;
	}
	if(!ob->object()->isWidgetType())
	{
		c->warning(__tr2qs_ctx("Can't add a non-widget object", "objects"));
		return true;
	}
	lay->addWidget(((QWidget *)(ob->object())));
	return true;
}
KVSO_CLASS_FUNCTION(widget, setParent)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject * ob;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("widget", KVS_PT_HOBJECT, KVS_PF_OPTIONAL, hObject)
	KVSO_PARAMETERS_END(c)
	ob = KviKvsKernel::instance()->objectController()->lookupObject(hObject);

	if(!widget())
		return true;

	if(!ob)
	{
		widget()->setParent(nullptr);
		return true;
	}
	else if(!ob->object()->isWidgetType())
	{
		c->warning(__tr2qs_ctx("Parent must be a widget object", "objects"));
		return true;
	}
	widget()->setParent(((QWidget *)(ob->object())));
	return true;
}

KVSO_CLASS_FUNCTION(widget, setWindowIcon)
{
	CHECK_INTERNAL_POINTER(widget())
	QString icon;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("icon", KVS_PT_STRING, 0, icon)
	KVSO_PARAMETERS_END(c)
	QPixmap * pix = g_pIconManager->getImage(icon);
	if(pix)
		widget()->setWindowIcon(QIcon(*pix));
	return true;
}

KVSO_CLASS_FUNCTION(widget, setBackgroundImage)
{
	CHECK_INTERNAL_POINTER(widget())
	QString image;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("image", KVS_PT_STRING, 0, image)
	KVSO_PARAMETERS_END(c)
	QPixmap * pix = g_pIconManager->getImage(image);
	if(pix)
	{
		QPalette palette = widget()->palette();
		palette.setBrush(widget()->backgroundRole(), QBrush(*pix));
		widget()->setPalette(palette);
	}
	else
	{
		c->warning(__tr2qs_ctx("The pixmap is not valid", "objects"));
		return true;
	}
	return true;
}

KVSO_CLASS_FUNCTION(widget, globalCursorX)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(QCursor::pos().x());
	return true;
}

KVSO_CLASS_FUNCTION(widget, globalCursorY)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger(QCursor::pos().y());
	return true;
}

KVSO_CLASS_FUNCTION(widget, setMask)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject * obj;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("pixmap", KVS_PT_HOBJECT, 0, hObject)
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
	QPixmap * pm = ((KvsObject_pixmap *)obj)->getPixmap();
	QBitmap mask(pm->mask());
	if(mask.isNull())
		c->warning(__tr2qs_ctx("Null mask", "objects"));
	widget()->setMask(mask);
	return true;
}

KVSO_CLASS_FUNCTION(widget, setAttribute)
{
	CHECK_INTERNAL_POINTER(widget())
	QString attribute;
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("widget_atribute", KVS_PT_STRING, 0, attribute)
	KVSO_PARAMETER("bool_flag", KVS_PT_BOOLEAN, 0, bFlag)
	KVSO_PARAMETERS_END(c)
	bool found = false;
	unsigned int j = 0;
	for(; j < widgetattributes_num; j++)
	{
		if(KviQString::equalCI(attribute, widgetattributes_tbl[j]))
		{
			found = true;
			break;
		}
	}
	if(found)
		widget()->setAttribute(widgetattributes_cod[j], bFlag);
	else
		c->warning(__tr2qs_ctx("Unknown widget attribute '%Q'", "objects"), &attribute);
	return true;
}

KVSO_CLASS_FUNCTION(widget, colorPalette)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szColorRole, szColorGroup;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("color_role", KVS_PT_STRING, 0, szColorRole)
	KVSO_PARAMETER("color_group", KVS_PT_STRING, 0, szColorGroup)
	KVSO_PARAMETERS_END(c)
	bool found = false;
	unsigned int j = 0;
	for(; j < colorrole_num; j++)
	{
		if(KviQString::equalCI(szColorRole, colorrole_tbl[j]))
		{
			found = true;
			break;
		}
	}
	if(!found)
	{
		c->warning(__tr2qs_ctx("Unknown Color Role '%Q'", "objects"), &szColorRole);
		return true;
	}
	QPalette::ColorGroup cg;
	if(KviQString::equalCI(szColorGroup, "Disabled"))
		cg = QPalette::Disabled;
	else if(KviQString::equalCI(szColorGroup, "Active"))
		cg = QPalette::Active;
	else if(KviQString::equalCI(szColorGroup, "Inactive"))
		cg = QPalette::Inactive;
	else
	{
		c->warning(__tr2qs_ctx("Unknown Color Group '%Q'", "objects"), &szColorGroup);
		return true;
	}
	QColor col = widget()->palette().color(cg, colorrole_cod[j]);
	KviKvsArray * a = new KviKvsArray();
	a->set(0, new KviKvsVariant((kvs_int_t)col.red()));
	a->set(1, new KviKvsVariant((kvs_int_t)col.green()));
	a->set(2, new KviKvsVariant((kvs_int_t)col.blue()));
	c->returnValue()->setArray(a);
	return true;
}

KVSO_CLASS_FUNCTION(widget, setStyleSheet)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szStyleSheet;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("style_sheet", KVS_PT_STRING, 0, szStyleSheet)
	KVSO_PARAMETERS_END(c)
	widget()->setStyleSheet(szStyleSheet);
	return true;
}

KVSO_CLASS_FUNCTION(widget, setKeyShortcut)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szKey;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("key", KVS_PT_STRING, 0, szKey)
	KVSO_PARAMETERS_END(c)
	szKey.prepend("&");
	c->returnValue()->setInteger((kvs_int_t)widget()->grabShortcut(QKeySequence::mnemonic(szKey)));
	return true;
}

KVSO_CLASS_FUNCTION(widget, insertIntoStatusBar)
{
	CHECK_INTERNAL_POINTER(widget())
	if(g_pMainWindow->mainStatusBar())
		g_pMainWindow->mainStatusBar()->insertPermanentWidgetAtTheEnd(widget(), 0);
	return true;
}

KVSO_CLASS_FUNCTION(widget, removeFromStatusBar)
{
	CHECK_INTERNAL_POINTER(widget())
	g_pMainWindow->statusBar()->removeWidget(widget());
	return true;
}
KVSO_CLASS_FUNCTION(widget, grab)
{
	qDebug("Grab");
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject * ob;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("widget", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETERS_END(c)
	qDebug("Get widget");
	ob = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!ob)
	{
		c->warning(__tr2qs_ctx("Widget parameter is not an object", "objects"));
		return true;
	}
	qDebug("check widgetype");
	if(!ob->object()->isWidgetType())
	{
		c->warning(__tr2qs_ctx("Widget object required", "objects"));
		return true;
	}

	QPixmap * pPixmap = new QPixmap();
	qDebug("grabbing");
	*pPixmap = QPixmap::grabWidget(((QWidget *)(ob->object())));
	KviKvsObjectClass * pClass = KviKvsKernel::instance()->objectController()->lookupClass("pixmap");
	KviKvsVariantList params;
	KviKvsObject * pObject = pClass->allocateInstance(nullptr, "internalpixmap", c->context(), &params);
	((KvsObject_pixmap *)pObject)->setInternalPixmap(pPixmap);
	c->returnValue()->setHObject(pObject->handle());
	return true;
}
