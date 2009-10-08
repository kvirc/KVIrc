#ifndef _OPTW_INPUT_H_
#define _OPTW_INPUT_H_

//=============================================================================
//
//   File : optw_input.h
//   Creation date : Sat Aug 11 2001 04:25:01 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_optionswidget.h"

#include <QComboBox>

#define KVI_OPTIONS_WIDGET_ICON_KviInputLookOptionsWidget KVI_SMALLICON_INPUT
#define KVI_OPTIONS_WIDGET_NAME_KviInputLookOptionsWidget __tr2qs_no_lookup("Input")
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviInputLookOptionsWidget __tr2qs_no_lookup("theme,colors,text")
#define KVI_OPTIONS_WIDGET_GROUP_KviInputLookOptionsWidget "theme"

class KviInputLookOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
private:
	QComboBox* m_pHorizontalAlign;
	QComboBox* m_pVerticalAlign;
public:
	KviInputLookOptionsWidget(QWidget * parent);
	~KviInputLookOptionsWidget();

	virtual void commit();
};

#define KVI_OPTIONS_WIDGET_ICON_KviInputFeaturesOptionsWidget KVI_SMALLICON_INPUT
#define KVI_OPTIONS_WIDGET_NAME_KviInputFeaturesOptionsWidget __tr2qs_no_lookup("Input")
#define KVI_OPTIONS_WIDGET_PARENT_KviInputFeaturesOptionsWidget KviInterfaceFeaturesOptionsWidget

class KviInputFeaturesOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviInputFeaturesOptionsWidget(QWidget * parent);
	~KviInputFeaturesOptionsWidget();
};

#endif //!_OPTW_INPUT_H_
