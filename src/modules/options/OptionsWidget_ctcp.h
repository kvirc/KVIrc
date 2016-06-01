#ifndef _OPTW_CTCP_H_
#define _OPTW_CTCP_H_
//=============================================================================
//
//   File : OptionsWidget_ctcp.h
//   Creation date : Sun Dec  2 20:40:43 2001 GMT by Szymon Stefanek
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

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_ctcp KviIconManager::CtcpReply
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_ctcp __tr2qs_no_lookup("CTCP")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_ctcp OptionsWidget_tools
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_ctcp 70000

class OptionsWidget_ctcp : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_ctcp(QWidget * parent);
	~OptionsWidget_ctcp();
};

#endif //_OPTW_CTCP_H_
