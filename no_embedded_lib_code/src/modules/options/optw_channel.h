#ifndef _OPTW_CHANNEL_H_
#define _OPTW_CHANNEL_H_
//=============================================================================
//
//   File : optw_channel.h
//   Creation date : Sat Sep 27 19:30:02 2008 GMT by Elvio Basello
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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
//
// This file was originally part of optw_irc.h
//
//=============================================================================

#include "kvi_optionswidget.h"

class QComboBox;

#define KVI_OPTIONS_WIDGET_ICON_KviChannelOptionsWidget KVI_SMALLICON_CHANNEL
#define KVI_OPTIONS_WIDGET_NAME_KviChannelOptionsWidget __tr2qs_no_lookup("Channel")
#define KVI_OPTIONS_WIDGET_PARENT_KviChannelOptionsWidget KviIrcOptionsWidget
#define KVI_OPTIONS_WIDGET_CONTAINER_KviChannelOptionsWidget true
#define KVI_OPTIONS_WIDGET_NOTCONTAINED_KviChannelOptionsWidget true
#define KVI_OPTIONS_WIDGET_PRIORITY_KviChannelOptionsWidget 90000

class KviChannelOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviChannelOptionsWidget(QWidget * parent);
	~KviChannelOptionsWidget();
};


#define KVI_OPTIONS_WIDGET_ICON_KviChannelGeneralOptionsWidget KVI_SMALLICON_CHANNEL
#define KVI_OPTIONS_WIDGET_NAME_KviChannelGeneralOptionsWidget __tr2qs_no_lookup("General")
#define KVI_OPTIONS_WIDGET_PARENT_KviChannelGeneralOptionsWidget KviChannelOptionsWidget

class KviChannelGeneralOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviChannelGeneralOptionsWidget(QWidget * parent);
	~KviChannelGeneralOptionsWidget();
};

#define KVI_OPTIONS_WIDGET_ICON_KviChannelAdvancedOptionsWidget KVI_SMALLICON_CHANNEL
#define KVI_OPTIONS_WIDGET_NAME_KviChannelAdvancedOptionsWidget __tr2qs_no_lookup("Advanced")
#define KVI_OPTIONS_WIDGET_PARENT_KviChannelAdvancedOptionsWidget KviChannelOptionsWidget

class KviChannelAdvancedOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviChannelAdvancedOptionsWidget(QWidget * parent);
	~KviChannelAdvancedOptionsWidget();
public:
	virtual void commit();
protected:
	QComboBox * m_pBanTypeCombo;
};

#endif // _OPTW_CHANNEL_H_
