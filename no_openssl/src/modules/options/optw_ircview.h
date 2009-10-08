#ifndef _OPTW_IRCVIEW_H_
#define _OPTW_IRCVIEW_H_

//=============================================================================
//
//   File : optw_ircview.h
//   Creation date : Sat Aug 11 2001 03:28:19 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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

#include <QComboBox>


#define KVI_OPTIONS_WIDGET_ICON_KviIrcViewThemeOptionsWidget KVI_SMALLICON_IRCVIEW
#define KVI_OPTIONS_WIDGET_NAME_KviIrcViewThemeOptionsWidget __tr2qs_no_lookup("Output")
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviIrcViewThemeOptionsWidget __tr2qs_no_lookup("theme,colors,output")
#define KVI_OPTIONS_WIDGET_GROUP_KviIrcViewThemeOptionsWidget "theme"
#define KVI_OPTIONS_WIDGET_CONTAINER_KviIrcViewThemeOptionsWidget true

class KviIrcViewThemeOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviIrcViewThemeOptionsWidget(QWidget * pParent);
	~KviIrcViewThemeOptionsWidget();
};

#define KVI_OPTIONS_WIDGET_ICON_KviIrcViewLookOptionsWidget KVI_SMALLICON_IRCVIEW
#define KVI_OPTIONS_WIDGET_NAME_KviIrcViewLookOptionsWidget __tr2qs_no_lookup("Look")
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviIrcViewLookOptionsWidget __tr2qs_no_lookup("theme,colors,output")
#define KVI_OPTIONS_WIDGET_GROUP_KviIrcViewLookOptionsWidget "theme"
#define KVI_OPTIONS_WIDGET_PARENT_KviIrcViewLookOptionsWidget KviIrcViewThemeOptionsWidget
#define KVI_OPTIONS_WIDGET_PRIORITY_KviIrcViewLookOptionsWidget 30

class KviIrcViewLookOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
private:
	QComboBox * m_pHorizontalAlign;
	QComboBox * m_pVerticalAlign;
public:
	KviIrcViewLookOptionsWidget(QWidget * parent);
	~KviIrcViewLookOptionsWidget();
public:
	virtual void commit();
};

#define KVI_OPTIONS_WIDGET_ICON_KviIrcViewFeaturesOptionsWidget KVI_SMALLICON_IRCVIEW
#define KVI_OPTIONS_WIDGET_NAME_KviIrcViewFeaturesOptionsWidget __tr2qs_no_lookup("Features")
#define KVI_OPTIONS_WIDGET_GROUP_KviIrcViewFeaturesOptionsWidget "theme"
#define KVI_OPTIONS_WIDGET_PARENT_KviIrcViewFeaturesOptionsWidget KviIrcViewThemeOptionsWidget
#define KVI_OPTIONS_WIDGET_PRIORITY_KviIrcViewFeaturesOptionsWidget 20

class KviIrcViewFeaturesOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviIrcViewFeaturesOptionsWidget(QWidget * parent);
	~KviIrcViewFeaturesOptionsWidget();
};

#define KVI_OPTIONS_WIDGET_ICON_KviIrcViewMarkerOptionsWidget KVI_SMALLICON_IRCVIEW
#define KVI_OPTIONS_WIDGET_NAME_KviIrcViewMarkerOptionsWidget __tr2qs_no_lookup("Marker")
#define KVI_OPTIONS_WIDGET_GROUP_KviIrcViewMarkerOptionsWidget "theme"
#define KVI_OPTIONS_WIDGET_PARENT_KviIrcViewMarkerOptionsWidget KviIrcViewThemeOptionsWidget
#define KVI_OPTIONS_WIDGET_PRIORITY_KviIrcViewMarkerOptionsWidget 10

class KviIrcViewMarkerOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviIrcViewMarkerOptionsWidget(QWidget * pParent);
	~KviIrcViewMarkerOptionsWidget();
	virtual void commit();
private:
	QComboBox * m_pMarkerStyle;
};


#endif //!_OPTW_IRCVIEW_H_
