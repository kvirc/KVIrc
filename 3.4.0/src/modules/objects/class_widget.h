#ifndef _CLASS_WIDGET_H_
#define _CLASS_WIDGET_H_
//vim: ts=8
//   File : class_widget.h
//   Creation date : Mon 11 Sep 16:34:36 CET 2000 by Krzysztof Godlewski
//
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
#include <kvi_tal_tooltip.h>
#include "kvi_string.h"
#include "qtooltip.h"
#include "object_macros.h"


class KviKvsObject_widget : public KviKvsObject
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_widget)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);

	virtual bool eventFilter(QObject *o,QEvent *e);

	// ok, it is clear that we're messing with the naming conventions for the
	// object classes :D
	// let's try to use this one:
	// function_isEnabled
	// function_setEnabled..
	// ... etc
	// so the name of the C++ function matches exactly the name of the KVS function at this point
	
	// let's also try to keep alphabetic order for the functions here.. so one can find them quickly

	bool function_addWidgetToWrappedLayout(KviKvsObjectFunctionCall *c);
	bool function_backgroundColor(KviKvsObjectFunctionCall *c);
	bool function_caption(KviKvsObjectFunctionCall *c);
	bool function_centerToScreen(KviKvsObjectFunctionCall *c);
	bool function_fontAscent(KviKvsObjectFunctionCall * c);
	bool function_fontDescent(KviKvsObjectFunctionCall * c);
	bool function_fontMetricsHeight(KviKvsObjectFunctionCall * c);
	bool function_fontMetricsWidth(KviKvsObjectFunctionCall * c);
	bool function_foregroundColor(KviKvsObjectFunctionCall *c);
	bool function_geometry(KviKvsObjectFunctionCall *c);
	bool function_globalCursorX(KviKvsObjectFunctionCall *c);
	bool function_globalCursorY(KviKvsObjectFunctionCall *c);
	bool function_hasFocus(KviKvsObjectFunctionCall *c);
	bool function_height(KviKvsObjectFunctionCall *);
	bool function_hide(KviKvsObjectFunctionCall *);
	bool function_isEnabled(KviKvsObjectFunctionCall *c);
	bool function_isTopLevel(KviKvsObjectFunctionCall *c);
	bool function_isVisible(KviKvsObjectFunctionCall *c);
	bool function_lower(KviKvsObjectFunctionCall *);
	bool function_mapFromGlobal(KviKvsObjectFunctionCall *c);
	bool function_mapToGlobal(KviKvsObjectFunctionCall *c);
	bool function_move(KviKvsObjectFunctionCall *c);
	bool function_parentWidget(KviKvsObjectFunctionCall *c);
	bool function_raise(KviKvsObjectFunctionCall *);
	bool function_repaint(KviKvsObjectFunctionCall *c);
	bool function_reparent(KviKvsObjectFunctionCall *c);
	bool function_resize(KviKvsObjectFunctionCall *c);
	bool function_screenResolution(KviKvsObjectFunctionCall * c);
	bool function_setBackgroundColor(KviKvsObjectFunctionCall *c);
	bool function_setBackgroundImage(KviKvsObjectFunctionCall *c);
	bool function_setCaption(KviKvsObjectFunctionCall *c);
	bool function_setEnabled(KviKvsObjectFunctionCall *c);
	bool function_setFocus(KviKvsObjectFunctionCall *c);
	bool function_setFocusPolicy(KviKvsObjectFunctionCall *c);
	bool function_setFont(KviKvsObjectFunctionCall *c);
	bool function_setIcon(KviKvsObjectFunctionCall *c);
	bool function_setMaximumWidth(KviKvsObjectFunctionCall *c);
	bool function_setMaximumHeight(KviKvsObjectFunctionCall *c);
	bool function_setMinimumWidth(KviKvsObjectFunctionCall *c);
	bool function_setMinimumHeight(KviKvsObjectFunctionCall *c);
	bool function_setMouseTracking(KviKvsObjectFunctionCall *c);
	bool function_setPaletteForeground(KviKvsObjectFunctionCall *c);
	bool function_setToolTip(KviKvsObjectFunctionCall *);
	bool function_setGeometry(KviKvsObjectFunctionCall *c);
	bool function_setWFlags(KviKvsObjectFunctionCall *c);
	bool function_show(KviKvsObjectFunctionCall *c);
	bool function_sizeHint(KviKvsObjectFunctionCall *c);
	bool function_width(KviKvsObjectFunctionCall *);
	bool function_x(KviKvsObjectFunctionCall *);
	bool function_y(KviKvsObjectFunctionCall *);
	bool function_setMask(KviKvsObjectFunctionCall *c);
	

#ifdef COMPILE_USE_QT4
bool function_setAttribute(KviKvsObjectFunctionCall *c);
#endif

signals:
	void aboutToDie();
};

#endif	// !_CLASS_WIDGET_H_
