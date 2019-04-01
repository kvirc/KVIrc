#ifndef _OPTW_WINDOWLIST_H_
#define _OPTW_WINDOWLIST_H_
//=============================================================================
//
//   File : OptionsWidget_windowList.h
//   Creation date : Wed Jan 16 04:30:41 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviOptionsWidget.h"

#include <QComboBox>

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_windowList KviIconManager::ClassicWindowList
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_windowList __tr2qs_no_lookup("Window List")
#define KVI_OPTIONS_WIDGET_MINIMAL_USER_EXPERIENCE_LEVEL_OptionsWidget_windowList 2
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_windowList __tr2qs_no_lookup("theme")
#define KVI_OPTIONS_WIDGET_GROUP_OptionsWidget_windowList "theme"

class OptionsWidget_windowList : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_windowList(QWidget * parent);
	~OptionsWidget_windowList();

protected:
	void commit() override;

private:
	QComboBox * m_pWindowListType;
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_windowListTree KviIconManager::TreeWindowList
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_windowListTree __tr2qs_no_lookup("Tree Window List")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_windowListTree OptionsWidget_windowList
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_windowListTree __tr2qs_no_lookup("theme")
#define KVI_OPTIONS_WIDGET_GROUP_OptionsWidget_windowListTree "theme"
#define KVI_OPTIONS_WIDGET_CONTAINER_OptionsWidget_windowListTree true

class OptionsWidget_windowListTree : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_windowListTree(QWidget * pParent);
	~OptionsWidget_windowListTree();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_windowListTreeForeground KviIconManager::Canvas
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_windowListTreeForeground __tr2qs_no_lookup("Foreground")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_windowListTreeForeground OptionsWidget_windowListTree
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviTreeTaskForegroundBarOptionsWidget __tr2qs_no_lookup("theme")
#define KVI_OPTIONS_WIDGET_GROUP_OptionsWidget_windowListTreeForeground "theme"

class OptionsWidget_windowListTreeForeground : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_windowListTreeForeground(QWidget * parent);
	~OptionsWidget_windowListTreeForeground();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_windowListTreeBackground KviIconManager::Canvas
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_windowListTreeBackground __tr2qs_no_lookup("Background")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_windowListTreeBackground OptionsWidget_windowListTree
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviTreeTaskBackgroundBarOptionsWidget __tr2qs_no_lookup("theme")
#define KVI_OPTIONS_WIDGET_GROUP_OptionsWidget_windowListTreeBackground "theme"

class OptionsWidget_windowListTreeBackground : public KviOptionsWidget
{
	Q_OBJECT
private:
	QComboBox * m_pHorizontalAlign;
	QComboBox * m_pVerticalAlign;

public:
	OptionsWidget_windowListTreeBackground(QWidget * parent);
	~OptionsWidget_windowListTreeBackground();

	virtual void commit();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_windowListTreeFeatures KviIconManager::TreeWindowList
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_windowListTreeFeatures __tr2qs_no_lookup("Features")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_windowListTreeFeatures OptionsWidget_windowListTree
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviTreeTaskFeaturesBarOptionsWidget __tr2qs_no_lookup("theme")
#define KVI_OPTIONS_WIDGET_GROUP_OptionsWidget_windowListTreeFeatures "theme"

class OptionsWidget_windowListTreeFeatures : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_windowListTreeFeatures(QWidget * parent);
	~OptionsWidget_windowListTreeFeatures();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_windowListClassic KviIconManager::ClassicWindowList
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_windowListClassic __tr2qs_no_lookup("Classic Window List")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_windowListClassic OptionsWidget_windowList
#define KVI_OPTIONS_WIDGET_MINIMAL_USER_EXPERIENCE_LEVEL_OptionsWidget_windowListClassic 2
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_windowListClassic __tr2qs_no_lookup("theme")
#define KVI_OPTIONS_WIDGET_GROUP_OptionsWidget_windowListClassic "theme"

class OptionsWidget_windowListClassic : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_windowListClassic(QWidget * parent);
	~OptionsWidget_windowListClassic();
};

#endif //_OPTW_WINDOWLIST_H_
