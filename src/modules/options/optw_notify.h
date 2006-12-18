#ifndef _OPTW_NOTIFY_H_
#define _OPTW_NOTIFY_H_
//
//   File : optw_notify.h
//   Creation date : Sun Oct 28 15:11:30 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_optionswidget.h"

#define KVI_OPTIONS_WIDGET_ICON_KviNotifyOptionsWidget KVI_SMALLICON_REGUSERS
#define KVI_OPTIONS_WIDGET_NAME_KviNotifyOptionsWidget __tr2qs_no_lookup("Contacts")
#define KVI_OPTIONS_WIDGET_PARENT_KviNotifyOptionsWidget KviToolsOptionsWidget
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviNotifyOptionsWidget __tr2qs_no_lookup("notify,buddy,buddies,friends")
#define KVI_OPTIONS_WIDGET_MINIMAL_USER_EXPERIENCE_LEVEL_KviNotifyOptionsWidget 2

class KviNotifyOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviNotifyOptionsWidget(QWidget * parent);
	~KviNotifyOptionsWidget();
};

#define KVI_OPTIONS_WIDGET_ICON_KviNotifierOptionsWidget KVI_SMALLICON_ALERT
#define KVI_OPTIONS_WIDGET_NAME_KviNotifierOptionsWidget __tr2qs_no_lookup("Notifier")
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviNotifierOptionsWidget __tr2qs_no_lookup("popup")
#define KVI_OPTIONS_WIDGET_PARENT_KviNotifierOptionsWidget KviInterfaceFeaturesOptionsWidget

class KviNotifierOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviNotifierOptionsWidget(QWidget * parent);
	~KviNotifierOptionsWidget();
};

#endif //_OPTW_NOTIFY_H_
