#ifndef _OPTW_INTERFACEFEATURES_H_
#define _OPTW_INTERFACEFEATURES_H_
//=============================================================================
//
//   File : OptionsWidget_interfaceFeatures.h
//   Creation date : Sat Jun 2 2001 14:50:01 CEST by Szymon Stefanek
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

#include <QCheckBox>

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_interfaceFeatures KviIconManager::Theme
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_interfaceFeatures __tr2qs_no_lookup("Interface")
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_interfaceFeatures 50000

class OptionsWidget_interfaceFeatures : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_interfaceFeatures(QWidget * parent);
	~OptionsWidget_interfaceFeatures();
};

#endif //!_OPTW_INTERFACEFEATURES_H_
