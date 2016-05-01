//=============================================================================
//
//   File : OptionsWidget_protection.cpp
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

#include "OptionsWidget_protection.h"

#include "kvi_settings.h"
#include "KviOptions.h"
#include "KviLocale.h"

#include <QLayout>

OptionsWidget_protection::OptionsWidget_protection(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("protection_options_widget");

	createLayout();
}

OptionsWidget_protection::~OptionsWidget_protection()
    = default;
