#ifndef _OPTW_HIGHLIGHTING_H_
#define _OPTW_HIGHLIGHTING_H_
//=============================================================================
//
//   File : optw_highlighting.h
//   Creation date : Sat Nov  3 18:09:37 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_optionswidget.h"

#define KVI_OPTIONS_WIDGET_ICON_KviAlertHighlightingOptionsWidget KVI_SMALLICON_RAW
#define KVI_OPTIONS_WIDGET_NAME_KviAlertHighlightingOptionsWidget __tr2qs_no_lookup("Alert/Highlight")
#define KVI_OPTIONS_WIDGET_PARENT_KviAlertHighlightingOptionsWidget KviToolsOptionsWidget
#define KVI_OPTIONS_WIDGET_CONTAINER_KviAlertHighlightingOptionsWidget true

class KviAlertHighlightingOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviAlertHighlightingOptionsWidget(QWidget * parent);
	~KviAlertHighlightingOptionsWidget();
};


#define KVI_OPTIONS_WIDGET_ICON_KviHighlightingOptionsWidget KVI_SMALLICON_RAW
#define KVI_OPTIONS_WIDGET_NAME_KviHighlightingOptionsWidget __tr2qs_no_lookup("Highlight")
#define KVI_OPTIONS_WIDGET_PARENT_KviHighlightingOptionsWidget KviAlertHighlightingOptionsWidget

class KviHighlightingOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviHighlightingOptionsWidget(QWidget * parent);
	~KviHighlightingOptionsWidget();
};


#define KVI_OPTIONS_WIDGET_ICON_KviAlertOptionsWidget KVI_SMALLICON_RAW
#define KVI_OPTIONS_WIDGET_NAME_KviAlertOptionsWidget __tr2qs_no_lookup("Window List Alert")
#define KVI_OPTIONS_WIDGET_PARENT_KviAlertOptionsWidget KviAlertHighlightingOptionsWidget

class KviAlertOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviAlertOptionsWidget(QWidget * parent);
	~KviAlertOptionsWidget();
};


#endif //_OPTW_HIGHLIGHTING_H_
