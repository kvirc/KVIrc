#ifndef _OPTW_DCC_H_
#define _OPTW_DCC_H_
//=============================================================================
//
//   File : OptionsWidget_dcc.h
//   Creation date : Fri Aug 17 22:08:08 2001 GMT by Szymon Stefanek
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
#include "KviIconManager.h"

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_dcc KviIconManager::Dcc
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_dcc __tr2qs_no_lookup("DCC")
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_dcc 70000
#define KVI_OPTIONS_WIDGET_CONTAINER_OptionsWidget_dcc true

class OptionsWidget_dcc : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_dcc(QWidget * parent);
	~OptionsWidget_dcc();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_dccGeneral KviIconManager::Dcc
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_dccGeneral __tr2qs_no_lookup("General")
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_dccGeneral 70000
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_dccGeneral OptionsWidget_dcc

class OptionsWidget_dccGeneral : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_dccGeneral(QWidget * parent);
	~OptionsWidget_dccGeneral();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_dccAdvanced KviIconManager::Gui
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_dccAdvanced __tr2qs_no_lookup("Advanced")
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_dccAdvanced 70000
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_dccAdvanced OptionsWidget_dcc

class OptionsWidget_dccAdvanced : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_dccAdvanced(QWidget * parent);
	~OptionsWidget_dccAdvanced();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_dccSend KviIconManager::FileTransfer
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_dccSend __tr2qs_no_lookup("File Transfer")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_dccSend OptionsWidget_dcc
#define KVI_OPTIONS_WIDGET_CONTAINER_OptionsWidget_dccSend true

class OptionsWidget_dccSend : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_dccSend(QWidget * parent);
	~OptionsWidget_dccSend();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_dccSendGeneral KviIconManager::FileTransfer
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_dccSendGeneral __tr2qs_no_lookup("General")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_dccSendGeneral OptionsWidget_dccSend

class OptionsWidget_dccSendGeneral : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_dccSendGeneral(QWidget * parent);
	~OptionsWidget_dccSendGeneral();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_dccSendAdvanced KviIconManager::Gui
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_dccSendAdvanced __tr2qs_no_lookup("Advanced")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_dccSendAdvanced OptionsWidget_dccSend

class OptionsWidget_dccSendAdvanced : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_dccSendAdvanced(QWidget * parent);
	~OptionsWidget_dccSendAdvanced();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_dccChat KviIconManager::Dcc
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_dccChat __tr2qs_no_lookup("Chat")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_dccChat OptionsWidget_dcc
#define KVI_OPTIONS_WIDGET_NOTCONTAINED_OptionsWidget_dccChat true

class OptionsWidget_dccChat : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_dccChat(QWidget * parent);
	~OptionsWidget_dccChat();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_dccVoice KviIconManager::DccVoice
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_dccVoice __tr2qs_no_lookup("Voice")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_dccVoice OptionsWidget_dcc
#define KVI_OPTIONS_WIDGET_NOTCONTAINED_OptionsWidget_dccVoice true

class OptionsWidget_dccVoice : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_dccVoice(QWidget *);
	~OptionsWidget_dccVoice();
};

#endif //_OPTW_DCC_H_
