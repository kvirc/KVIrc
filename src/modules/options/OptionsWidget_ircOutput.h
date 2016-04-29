#ifndef _OPTW_IRCOUTPUT_H_
#define _OPTW_IRCOUTPUT_H_
//=============================================================================
//
//   File : OptionsWidget_ircOutput.h
//   Creation date : Wed Nov 15 2000 11:47:51 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

class QComboBox;

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_ircOutput KviIconManager::Raw
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_ircOutput __tr2qs_no_lookup("Verbosity and Output")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_ircOutput OptionsWidget_irc
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_ircOutput __tr2qs_no_lookup("debug")
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_ircOutput 75000

class OptionsWidget_ircOutput : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_ircOutput(QWidget * parent);
	~OptionsWidget_ircOutput();

protected:
	QComboBox * m_pVerbosityCombo;
	QComboBox * m_pDatetimeCombo;

protected:
	virtual void commit();
};

#endif //!_OPTW_IRCOUTPUT_H_
