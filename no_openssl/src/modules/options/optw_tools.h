#ifndef _OPTW_TOOLS_H_
#define _OPTW_TOOLS_H_

//=============================================================================
//
//   File : optw_tools.h
//   Creation date : Wed Ott 19 16:15:16 2005 GMT by Grifisx & Noldor
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "kvi_settings.h"
#include "kvi_optionswidget.h"
#include "kvi_selectors.h"

#define KVI_OPTIONS_WIDGET_ICON_KviToolsOptionsWidget KVI_SMALLICON_ADDONS
#define KVI_OPTIONS_WIDGET_NAME_KviToolsOptionsWidget __tr2qs_no_lookup("Tools")
#define KVI_OPTIONS_WIDGET_PARENT_KviToolsOptionsWidget KviIrcOptionsWidget
#define KVI_OPTIONS_WIDGET_PRIORITY_KviToolsOptionsWidget 50000
#define KVI_OPTIONS_WIDGET_NOTCONTAINED_KviToolsOptionsWidget true

class KviToolsOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviToolsOptionsWidget(QWidget * parent);
	~KviToolsOptionsWidget();
};

#endif //!_OPTW_FOLDERS_H_
