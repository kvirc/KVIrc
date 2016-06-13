#ifndef _OPTW_CHANNEL_H_
#define _OPTW_CHANNEL_H_
//=============================================================================
//
//   File : OptionsWidget_channel.h
//   Creation date : Sat Sep 27 19:30:02 2008 GMT by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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

//
// This file was originally part of OptionsWidget_irc.h
//

#include "KviOptionsWidget.h"

class QComboBox;

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_channel KviIconManager::Channel
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_channel __tr2qs_no_lookup("Channel")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_channel OptionsWidget_irc
#define KVI_OPTIONS_WIDGET_CONTAINER_OptionsWidget_channel true
#define KVI_OPTIONS_WIDGET_NOTCONTAINED_OptionsWidget_channel true
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_channel 90000

class OptionsWidget_channel : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_channel(QWidget * parent);
	~OptionsWidget_channel();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_channelGeneral KviIconManager::Channel
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_channelGeneral __tr2qs_no_lookup("General")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_channelGeneral OptionsWidget_channel

class OptionsWidget_channelGeneral : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_channelGeneral(QWidget * parent);
	~OptionsWidget_channelGeneral();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_channelAdvanced KviIconManager::Gui
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_channelAdvanced __tr2qs_no_lookup("Advanced")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_channelAdvanced OptionsWidget_channel

class OptionsWidget_channelAdvanced : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_channelAdvanced(QWidget * parent);
	~OptionsWidget_channelAdvanced();

public:
	virtual void commit();

protected:
	QComboBox * m_pBanTypeCombo;
};

#endif // _OPTW_CHANNEL_H_
