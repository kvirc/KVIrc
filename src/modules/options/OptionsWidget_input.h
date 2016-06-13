#ifndef _OPTW_INPUT_H_
#define _OPTW_INPUT_H_
//=============================================================================
//
//   File : OptionsWidget_input.h
//   Creation date : Sat Aug 11 2001 04:25:01 CEST by Szymon Stefanek
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

#include <QComboBox>

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_inputLook KviIconManager::Input
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_inputLook __tr2qs_no_lookup("Input")
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_inputLook __tr2qs_no_lookup("theme,colors,text")
#define KVI_OPTIONS_WIDGET_GROUP_OptionsWidget_inputLook "theme"

class OptionsWidget_inputLook : public KviOptionsWidget
{
	Q_OBJECT
private:
	QComboBox * m_pHorizontalAlign;
	QComboBox * m_pVerticalAlign;

public:
	OptionsWidget_inputLook(QWidget * parent);
	~OptionsWidget_inputLook();

	virtual void commit();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_inputFeatures KviIconManager::Input
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_inputFeatures __tr2qs_no_lookup("Input")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_inputFeatures OptionsWidget_interfaceFeatures

class OptionsWidget_inputFeatures : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_inputFeatures(QWidget * parent);
	~OptionsWidget_inputFeatures();
};

#endif //!_OPTW_INPUT_H_
