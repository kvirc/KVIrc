#ifndef _OPTW_CTCPFLOODPROTECTION_H_
#define _OPTW_CTCPFLOODPROTECTION_H_
//=============================================================================
//
//   File : OptionsWidget_ctcpFloodProtection.h
//   Creation date : Sun Jan 21 2000 14:34:12 CEST by Szymon Stefanek
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

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_ctcpFloodProtection KviIconManager::CtcpRequestFlood
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_ctcpFloodProtection __tr2qs_no_lookup("Flood Protection")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_ctcpFloodProtection OptionsWidget_protection

class OptionsWidget_ctcpFloodProtection : public KviOptionsWidget
{
	Q_OBJECT
	//	Q_CLASSINFO("help","ctcpfloodprotectionoptions")
public:
	OptionsWidget_ctcpFloodProtection(QWidget * parent);
	~OptionsWidget_ctcpFloodProtection();
};

#endif //!_OPTW_CTCPFLOODPROTECTION_H_
