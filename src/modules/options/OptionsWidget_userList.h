#ifndef _OPTW_USERLIST_H_
#define _OPTW_USERLIST_H_
//=============================================================================
//
//   File : OptionsWidget_userList.h
//   Creation date : Sat Mar 02 2002 12:09:19 CEST by Szymon Stefanek
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

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_userList KviIconManager::UserList
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_userList __tr2qs_no_lookup("Userlist")
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_userList __tr2qs_no_lookup("theme,userlist,channel")
#define KVI_OPTIONS_WIDGET_GROUP_OptionsWidget_userList "theme"
#define KVI_OPTIONS_WIDGET_CONTAINER_OptionsWidget_userList true

class OptionsWidget_userList : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_userList(QWidget * parent);
	~OptionsWidget_userList();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_userListBackground KviIconManager::Canvas
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_userListBackground __tr2qs_no_lookup("Background")
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_userListBackground __tr2qs_no_lookup("theme,userlist,channel,background")
#define KVI_OPTIONS_WIDGET_GROUP_OptionsWidget_userListBackground "theme"
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_userListBackground OptionsWidget_userList
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_userListBackground 30

class OptionsWidget_userListBackground : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_userListBackground(QWidget * parent);
	~OptionsWidget_userListBackground();

protected:
	QComboBox * m_pHorizontalAlign;
	QComboBox * m_pVerticalAlign;

public:
	virtual void commit();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_userListForeground KviIconManager::Canvas
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_userListForeground __tr2qs_no_lookup("Foreground")
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_userListForeground __tr2qs_no_lookup("theme,userlist,channel,colors")
#define KVI_OPTIONS_WIDGET_GROUP_OptionsWidget_userListForeground "theme"
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_userListForeground OptionsWidget_userList
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_userListForeground 20

class OptionsWidget_userListForeground : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_userListForeground(QWidget * parent);
	~OptionsWidget_userListForeground();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_userListGrid KviIconManager::UserList
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_userListGrid __tr2qs_no_lookup("Userlist Grid")
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_userListGrid __tr2qs_no_lookup("theme,userlist,channel,grid")
#define KVI_OPTIONS_WIDGET_GROUP_OptionsWidget_userListGrid "theme"
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_userListGrid OptionsWidget_userList
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_userListGrid 30

class OptionsWidget_userListGrid : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_userListGrid(QWidget * parent);
	~OptionsWidget_userListGrid();

protected:
	QComboBox * m_pGridTypeCombo;

public:
	virtual void commit();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_userListFeatures KviIconManager::UserList
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_userListFeatures __tr2qs_no_lookup("Features")
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_userListFeatures __tr2qs_no_lookup("userlist,channel,width")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_userListFeatures OptionsWidget_userList
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_userListFeatures 10

class OptionsWidget_userListFeatures : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_userListFeatures(QWidget * parent);
	~OptionsWidget_userListFeatures();
};

#endif //!_OPTW_USERLIST_H_
