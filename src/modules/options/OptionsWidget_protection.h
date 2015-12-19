#ifndef _OPTW_PROTECTION_H_
#define _OPTW_PROTECTION_H_
//=============================================================================
//
//   File : OptionsWidget_protection.h
//   Creation date : Wed Oct 19 16:15:16 2005 GMT by Grifisx & Noldor
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include "kvi_settings.h"
#include "KviOptionsWidget.h"
#include "KviSelectors.h"

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_protection KviIconManager::Shield
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_protection __tr2qs_no_lookup("Protection")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_protection OptionsWidget_irc
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_protection 60000
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_protection __tr2qs_no_lookup("ctcp,flood")
#define KVI_OPTIONS_WIDGET_CONTAINER_OptionsWidget_protection true
#define KVI_OPTIONS_WIDGET_NOTCONTAINED_OptionsWidget_protection true

class OptionsWidget_protection : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_protection(QWidget * parent);
	~OptionsWidget_protection();
};

#endif //!_OPTW_PROTECTION_H_
