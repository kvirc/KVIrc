#ifndef _OPTW_LAG_H_
#define _OPTW_LAG_H_
//=============================================================================
//
//   File : OptionsWidget_lag.h
//   Creation date : Wed Oct 16 10:45:54 CEST 2002 by Juanjo Álvarez (juanjux@yahoo.es)
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002 Juanjo Álvarez
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

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_lag KviIconManager::ServerPing
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_lag __tr2qs_no_lookup("Lag")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_lag OptionsWidget_tools
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_lag __tr2qs_no_lookup("time,ping")

class OptionsWidget_lag : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_lag(QWidget * parent);
	~OptionsWidget_lag();
};

#endif //_OPTW_LAG_H_
