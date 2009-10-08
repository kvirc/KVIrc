#ifndef _OPTW_WINDOWLIST_H_
#define _OPTW_WINDOWLIST_H_

//===============================================================================
//
//   File : optw_windowlist.h
//   Creation date : Wed Jan 16 04:30:41 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
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
//===============================================================================


#include "kvi_optionswidget.h"

#include <QComboBox>

#define KVI_OPTIONS_WIDGET_ICON_KviWindowListOptionsWidget KVI_SMALLICON_CLASSICWINDOWLIST
#define KVI_OPTIONS_WIDGET_NAME_KviWindowListOptionsWidget __tr2qs_no_lookup("Window List")
#define KVI_OPTIONS_WIDGET_MINIMAL_USER_EXPERIENCE_LEVEL_KviWindowListOptionsWidget 2
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviWindowListOptionsWidget __tr2qs_no_lookup("theme")
#define KVI_OPTIONS_WIDGET_GROUP_KviWindowListOptionsWidget "theme"

class KviWindowListOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviWindowListOptionsWidget(QWidget * parent);
	~KviWindowListOptionsWidget();
};

#define KVI_OPTIONS_WIDGET_ICON_KviTreeWindowListOptionsWidget KVI_SMALLICON_TREEWINDOWLIST
#define KVI_OPTIONS_WIDGET_NAME_KviTreeWindowListOptionsWidget __tr2qs_no_lookup("Tree Window List")
#define KVI_OPTIONS_WIDGET_PARENT_KviTreeWindowListOptionsWidget KviWindowListOptionsWidget
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviTreeWindowListOptionsWidget __tr2qs_no_lookup("theme")
#define KVI_OPTIONS_WIDGET_GROUP_KviTreeWindowListOptionsWidget "theme"
#define KVI_OPTIONS_WIDGET_CONTAINER_KviTreeWindowListOptionsWidget true

class KviTreeWindowListOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviTreeWindowListOptionsWidget(QWidget * pParent);
	~KviTreeWindowListOptionsWidget();
};


#define KVI_OPTIONS_WIDGET_ICON_KviTreeWindowListForegroundOptionsWidget KVI_SMALLICON_TREEWINDOWLIST
#define KVI_OPTIONS_WIDGET_NAME_KviTreeWindowListForegroundOptionsWidget __tr2qs_no_lookup("Foreground")
#define KVI_OPTIONS_WIDGET_PARENT_KviTreeWindowListForegroundOptionsWidget KviTreeWindowListOptionsWidget
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviTreeTaskForegroundBarOptionsWidget __tr2qs_no_lookup("theme")
#define KVI_OPTIONS_WIDGET_GROUP_KviTreeWindowListForegroundOptionsWidget "theme"

class KviTreeWindowListForegroundOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviTreeWindowListForegroundOptionsWidget(QWidget * parent);
	~KviTreeWindowListForegroundOptionsWidget();
};


#define KVI_OPTIONS_WIDGET_ICON_KviTreeWindowListBackgroundOptionsWidget KVI_SMALLICON_TREEWINDOWLIST
#define KVI_OPTIONS_WIDGET_NAME_KviTreeWindowListBackgroundOptionsWidget __tr2qs_no_lookup("Background")
#define KVI_OPTIONS_WIDGET_PARENT_KviTreeWindowListBackgroundOptionsWidget KviTreeWindowListOptionsWidget
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviTreeTaskBackgroundBarOptionsWidget __tr2qs_no_lookup("theme")
#define KVI_OPTIONS_WIDGET_GROUP_KviTreeWindowListBackgroundOptionsWidget "theme"

class KviTreeWindowListBackgroundOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
private:
	QComboBox* m_pHorizontalAlign;
	QComboBox* m_pVerticalAlign;
public:
	KviTreeWindowListBackgroundOptionsWidget(QWidget * parent);
	~KviTreeWindowListBackgroundOptionsWidget();

	virtual void commit();
};

#define KVI_OPTIONS_WIDGET_ICON_KviTreeWindowListFeaturesOptionsWidget KVI_SMALLICON_TREEWINDOWLIST
#define KVI_OPTIONS_WIDGET_NAME_KviTreeWindowListFeaturesOptionsWidget __tr2qs_no_lookup("Features")
#define KVI_OPTIONS_WIDGET_PARENT_KviTreeWindowListFeaturesOptionsWidget KviTreeWindowListOptionsWidget
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviTreeTaskFeaturesBarOptionsWidget __tr2qs_no_lookup("theme")
#define KVI_OPTIONS_WIDGET_GROUP_KviTreeWindowListFeaturesOptionsWidget "theme"

class KviTreeWindowListFeaturesOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviTreeWindowListFeaturesOptionsWidget(QWidget * parent);
	~KviTreeWindowListFeaturesOptionsWidget();
};

#define KVI_OPTIONS_WIDGET_ICON_KviClassicWindowListOptionsWidget KVI_SMALLICON_CLASSICWINDOWLIST
#define KVI_OPTIONS_WIDGET_NAME_KviClassicWindowListOptionsWidget __tr2qs_no_lookup("Classic Window List")
#define KVI_OPTIONS_WIDGET_PARENT_KviClassicWindowListOptionsWidget KviWindowListOptionsWidget
#define KVI_OPTIONS_WIDGET_MINIMAL_USER_EXPERIENCE_LEVEL_KviClassicWindowListOptionsWidget 2
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviClassicWindowListOptionsWidget __tr2qs_no_lookup("theme")
#define KVI_OPTIONS_WIDGET_GROUP_KviClassicWindowListOptionsWidget "theme"

class KviClassicWindowListOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviClassicWindowListOptionsWidget(QWidget * parent);
	~KviClassicWindowListOptionsWidget();
};

#endif //_OPTW_WINDOWLIST_H_
