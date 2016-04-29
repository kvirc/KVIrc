#ifndef _OPTW_HIGHLIGHTING_H_
#define _OPTW_HIGHLIGHTING_H_
//=============================================================================
//
//   File : OptionsWidget_alertsAndHighlighting.h
//   Creation date : Sat Nov  3 18:09:37 2001 GMT by Szymon Stefanek
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

class QRadioButton;
class QLineEdit;

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_alertsAndHighlighting KviIconManager::Warning
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_alertsAndHighlighting __tr2qs_no_lookup("Alert/Highlight")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_alertsAndHighlighting OptionsWidget_tools
#define KVI_OPTIONS_WIDGET_CONTAINER_OptionsWidget_alertsAndHighlighting true

class OptionsWidget_alertsAndHighlighting : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_alertsAndHighlighting(QWidget * parent);
	~OptionsWidget_alertsAndHighlighting();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_highlighting KviIconManager::QueryTrace
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_highlighting __tr2qs_no_lookup("Highlight")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_highlighting OptionsWidget_alertsAndHighlighting

class OptionsWidget_highlighting : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_highlighting(QWidget * parent);
	~OptionsWidget_highlighting();

private:
	QRadioButton * m_pHighlightAllOccurencesRadioButton;
	QRadioButton * m_pHighlightWholeWordsOnlyRadioButton;
	QLineEdit * m_pWordSplitterCharactersEdit;

protected:
	virtual void commit();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_alerts KviIconManager::Warning
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_alerts __tr2qs_no_lookup("Window List Alert")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_alerts OptionsWidget_alertsAndHighlighting

class OptionsWidget_alerts : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_alerts(QWidget * parent);
	~OptionsWidget_alerts();
};

#endif //_OPTW_HIGHLIGHTING_H_
