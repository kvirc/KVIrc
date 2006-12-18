#ifndef _OPTW_IRC_H_
#define _OPTW_IRC_H_
//=============================================================================
//
//   File : optw_irc.h
//   Creation date : Sun Dec  2 18:59:49 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2006 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "kvi_optionswidget.h"

class QComboBox;

#define KVI_OPTIONS_WIDGET_ICON_KviIrcOptionsWidget KVI_SMALLICON_IRC
#define KVI_OPTIONS_WIDGET_NAME_KviIrcOptionsWidget __tr2qs_no_lookup("IRC")
#define KVI_OPTIONS_WIDGET_PRIORITY_KviIrcOptionsWidget 80000
#define KVI_OPTIONS_WIDGET_CONTAINER_KviIrcOptionsWidget true

class KviIrcOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviIrcOptionsWidget(QWidget * parent);
	~KviIrcOptionsWidget();
};

#define KVI_OPTIONS_WIDGET_ICON_KviIrcGeneralOptionsWidget KVI_SMALLICON_IRC
#define KVI_OPTIONS_WIDGET_NAME_KviIrcGeneralOptionsWidget __tr2qs_no_lookup("General")
#define KVI_OPTIONS_WIDGET_PARENT_KviIrcGeneralOptionsWidget KviIrcOptionsWidget
#define KVI_OPTIONS_WIDGET_PRIORITY_KviIrcGeneralOptionsWidget 80000

class KviIrcGeneralOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviIrcGeneralOptionsWidget(QWidget * parent);
	~KviIrcGeneralOptionsWidget();
};


#define KVI_OPTIONS_WIDGET_ICON_KviIrcAdvancedOptionsWidget KVI_SMALLICON_IRC
#define KVI_OPTIONS_WIDGET_NAME_KviIrcAdvancedOptionsWidget __tr2qs_no_lookup("Advanced")
#define KVI_OPTIONS_WIDGET_PARENT_KviIrcAdvancedOptionsWidget KviIrcOptionsWidget
#define KVI_OPTIONS_WIDGET_PRIORITY_KviIrcAdvancedOptionsWidget 50000

class KviIrcAdvancedOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviIrcAdvancedOptionsWidget(QWidget * parent);
	~KviIrcAdvancedOptionsWidget();
};


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

#endif //_OPTW_IRC_H_
