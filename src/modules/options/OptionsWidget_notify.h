#ifndef _OPTW_NOTIFY_H_
#define _OPTW_NOTIFY_H_
//=============================================================================
//
//   File : OptionsWidget_notify.h
//   Creation date : Sun Oct 28 15:11:30 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_notify KviIconManager::UserWindow
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_notify __tr2qs_no_lookup("Notifications")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_notify OptionsWidget_tools
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_notify __tr2qs_no_lookup("notify,notifications,buddy,buddies,friend,friends")
#define KVI_OPTIONS_WIDGET_MINIMAL_USER_EXPERIENCE_LEVEL_OptionsWidget_notify 2

class OptionsWidget_notify : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_notify(QWidget * parent);
	~OptionsWidget_notify();
};

#endif //_OPTW_NOTIFY_H_
