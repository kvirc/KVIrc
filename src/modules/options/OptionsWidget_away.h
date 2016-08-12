#ifndef _OPTW_AWAY_H_
#define _OPTW_AWAY_H_
//=============================================================================
//
//   File : OptionsWidget_away.h
//   Creation date : Mon Oct 29 15:37:18 2001 GMT by Szymon Stefanek
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

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_away KviIconManager::Away
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_away __tr2qs_no_lookup("Away")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_away OptionsWidget_tools

class OptionsWidget_away final : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_away(QWidget * parent);

protected:
	KviBoolSelector * m_pEnableAwayMessage;
	KviStringSelector * m_pAwayMessage;
	KviBoolSelector * m_pEnableAwayNick;
	KviStringSelector * m_pCustomAwayNick;
	KviBoolSelector * m_pEnableCustomAwayNick;
public slots:
	void enableCustomAwayText(bool);
	void enableAwayMessage(bool);
};

#endif //_OPTW_AWAY_H_
