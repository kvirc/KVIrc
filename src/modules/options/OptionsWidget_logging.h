#ifndef _OPTW_LOGGING_H_
#define _OPTW_LOGGING_H_
//=============================================================================
//
//   File : OptionsWidget_logging.h
//   Creation date : Mon Oct 29 15:37:18 2001 GMT by Szymon Stefanek
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

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_logging KviIconManager::Log
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_logging __tr2qs_no_lookup("Logging")
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_logging __tr2qs_no_lookup("save,output")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_logging OptionsWidget_tools

//noway #define KVI_OPTIONS_WIDGET_NO_PARENT_OptionsWidget_logging OptionsWidget_irc

class OptionsWidget_logging : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_logging(QWidget * parent);
	~OptionsWidget_logging();
};

#endif //_OPTW_LOGGING_H_
