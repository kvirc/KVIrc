//=============================================================================
//
//   File : OptionsWidget_general.cpp
//   Creation date : Wed Nov 19 09:25:16 2005 GMT by Grifisx & Noldor
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

#include "OptionsWidget_general.h"

#include "kvi_settings.h"
#include "KviLocale.h"
#include "KviOptions.h"

OptionsWidget_general::OptionsWidget_general(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("generalopt_options_widget");
	createLayout();

	addLabel(0, 0, 4, 0, __tr2qs_ctx("This section contains the general client options "
	                                 "like <b>sound, mediafiles, URL handlers</b> etc... "
	                                 "",
	                         "options"));

	addRowSpacer(0, 1, 0, 1);

	//addAdvancedButton(4,1,4,1);
}

OptionsWidget_general::~OptionsWidget_general()
    = default;
