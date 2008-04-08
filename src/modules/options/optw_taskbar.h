#ifndef _OPTW_TASKBAR_H_
#define _OPTW_TASKBAR_H_

//===============================================================================
//
//   File : optw_taskbar.h
//   Creation date : Wed Jan 16 04:30:41 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2006 Szymon Stefanek (pragma at kvirc dot net)
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
//===============================================================================


#include "kvi_optionswidget.h"

#include <QComboBox>

#define KVI_OPTIONS_WIDGET_ICON_KviTaskBarOptionsWidget KVI_SMALLICON_CLASSICTASKBAR
#define KVI_OPTIONS_WIDGET_NAME_KviTaskBarOptionsWidget __tr2qs_no_lookup("Taskbar")
#define KVI_OPTIONS_WIDGET_MINIMAL_USER_EXPERIENCE_LEVEL_KviTaskBarOptionsWidget 2
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviTaskBarOptionsWidget __tr2qs_no_lookup("theme")
#define KVI_OPTIONS_WIDGET_GROUP_KviTaskBarOptionsWidget "theme"

class KviTaskBarOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviTaskBarOptionsWidget(QWidget * parent);
	~KviTaskBarOptionsWidget();
};

#define KVI_OPTIONS_WIDGET_ICON_KviTreeTaskBarOptionsWidget KVI_SMALLICON_TREETASKBAR
#define KVI_OPTIONS_WIDGET_NAME_KviTreeTaskBarOptionsWidget __tr2qs_no_lookup("Tree Taskbar")
#define KVI_OPTIONS_WIDGET_PARENT_KviTreeTaskBarOptionsWidget KviTaskBarOptionsWidget
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviTreeTaskBarOptionsWidget __tr2qs_no_lookup("theme")
#define KVI_OPTIONS_WIDGET_GROUP_KviTreeTaskBarOptionsWidget "theme"
#define KVI_OPTIONS_WIDGET_CONTAINER_KviTreeTaskBarOptionsWidget true

class KviTreeTaskBarOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviTreeTaskBarOptionsWidget(QWidget * pParent);
	~KviTreeTaskBarOptionsWidget();
};


#define KVI_OPTIONS_WIDGET_ICON_KviTreeTaskBarForegroundOptionsWidget KVI_SMALLICON_TREETASKBAR
#define KVI_OPTIONS_WIDGET_NAME_KviTreeTaskBarForegroundOptionsWidget __tr2qs_no_lookup("Foreground")
#define KVI_OPTIONS_WIDGET_PARENT_KviTreeTaskBarForegroundOptionsWidget KviTreeTaskBarOptionsWidget
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviTreeTaskForegroundBarOptionsWidget __tr2qs_no_lookup("theme")
#define KVI_OPTIONS_WIDGET_GROUP_KviTreeTaskBarForegroundOptionsWidget "theme"

class KviTreeTaskBarForegroundOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviTreeTaskBarForegroundOptionsWidget(QWidget * parent);
	~KviTreeTaskBarForegroundOptionsWidget();
};


#define KVI_OPTIONS_WIDGET_ICON_KviTreeTaskBarBackgroundOptionsWidget KVI_SMALLICON_TREETASKBAR
#define KVI_OPTIONS_WIDGET_NAME_KviTreeTaskBarBackgroundOptionsWidget __tr2qs_no_lookup("Background")
#define KVI_OPTIONS_WIDGET_PARENT_KviTreeTaskBarBackgroundOptionsWidget KviTreeTaskBarOptionsWidget
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviTreeTaskBackgroundBarOptionsWidget __tr2qs_no_lookup("theme")
#define KVI_OPTIONS_WIDGET_GROUP_KviTreeTaskBarBackgroundOptionsWidget "theme"

class KviTreeTaskBarBackgroundOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
private:
	QComboBox* m_pHorizontalAlign;
	QComboBox* m_pVerticalAlign;
public:
	KviTreeTaskBarBackgroundOptionsWidget(QWidget * parent);
	~KviTreeTaskBarBackgroundOptionsWidget();
	
	virtual void commit();
};

#define KVI_OPTIONS_WIDGET_ICON_KviTreeTaskBarFeaturesOptionsWidget KVI_SMALLICON_TREETASKBAR
#define KVI_OPTIONS_WIDGET_NAME_KviTreeTaskBarFeaturesOptionsWidget __tr2qs_no_lookup("Features")
#define KVI_OPTIONS_WIDGET_PARENT_KviTreeTaskBarFeaturesOptionsWidget KviTreeTaskBarOptionsWidget
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviTreeTaskFeaturesBarOptionsWidget __tr2qs_no_lookup("theme")
#define KVI_OPTIONS_WIDGET_GROUP_KviTreeTaskBarFeaturesOptionsWidget "theme"

class KviTreeTaskBarFeaturesOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviTreeTaskBarFeaturesOptionsWidget(QWidget * parent);
	~KviTreeTaskBarFeaturesOptionsWidget();
};

#define KVI_OPTIONS_WIDGET_ICON_KviClassicTaskBarOptionsWidget KVI_SMALLICON_CLASSICTASKBAR
#define KVI_OPTIONS_WIDGET_NAME_KviClassicTaskBarOptionsWidget __tr2qs_no_lookup("Classic Taskbar")
#define KVI_OPTIONS_WIDGET_PARENT_KviClassicTaskBarOptionsWidget KviTaskBarOptionsWidget
#define KVI_OPTIONS_WIDGET_MINIMAL_USER_EXPERIENCE_LEVEL_KviClassicTaskBarOptionsWidget 2
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviClassicTaskBarOptionsWidget __tr2qs_no_lookup("theme")
#define KVI_OPTIONS_WIDGET_GROUP_KviClassicTaskBarOptionsWidget "theme"

class KviClassicTaskBarOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviClassicTaskBarOptionsWidget(QWidget * parent);
	~KviClassicTaskBarOptionsWidget();
};

#endif //_OPTW_TASKBAR_H_
