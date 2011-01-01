#ifndef _OPTW_DCC_H_
#define _OPTW_DCC_H_
//=============================================================================
//
//   File : OptionsWidget_dcc.h
//   Creation date : Fri Aug 17 22:08:08 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviOptionsWidget.h"

#define KVI_OPTIONS_WIDGET_ICON_KviDccOptionsWidget KVI_SMALLICON_DCCERROR
#define KVI_OPTIONS_WIDGET_NAME_KviDccOptionsWidget __tr2qs_no_lookup("DCC")
#define KVI_OPTIONS_WIDGET_PRIORITY_KviDccOptionsWidget 70000
#define KVI_OPTIONS_WIDGET_CONTAINER_KviDccOptionsWidget true

class OptionsWidget_dcc : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_dcc(QWidget * parent);
	~OptionsWidget_dcc();
};



#define KVI_OPTIONS_WIDGET_ICON_KviDccGeneralOptionsWidget KVI_SMALLICON_DCCERROR
#define KVI_OPTIONS_WIDGET_NAME_KviDccGeneralOptionsWidget __tr2qs_no_lookup("General")
#define KVI_OPTIONS_WIDGET_PRIORITY_KviDccGeneralOptionsWidget 70000
#define KVI_OPTIONS_WIDGET_PARENT_KviDccGeneralOptionsWidget OptionsWidget_dcc

class OptionsWidget_dccGeneral : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_dccGeneral(QWidget * parent);
	~OptionsWidget_dccGeneral();
};

#define KVI_OPTIONS_WIDGET_ICON_KviDccAdvancedOptionsWidget KVI_SMALLICON_DCCERROR
#define KVI_OPTIONS_WIDGET_NAME_KviDccAdvancedOptionsWidget __tr2qs_no_lookup("Advanced")
#define KVI_OPTIONS_WIDGET_PRIORITY_KviDccAdvancedOptionsWidget 70000
#define KVI_OPTIONS_WIDGET_PARENT_KviDccAdvancedOptionsWidget OptionsWidget_dcc

class OptionsWidget_dccAdvanced : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_dccAdvanced(QWidget * parent);
	~OptionsWidget_dccAdvanced();
};




#define KVI_OPTIONS_WIDGET_ICON_KviDccSendOptionsWidget KVI_SMALLICON_DCCMSG
#define KVI_OPTIONS_WIDGET_NAME_KviDccSendOptionsWidget __tr2qs_no_lookup("File Transfer")
#define KVI_OPTIONS_WIDGET_PARENT_KviDccSendOptionsWidget OptionsWidget_dcc
#define KVI_OPTIONS_WIDGET_CONTAINER_KviDccSendOptionsWidget true


class OptionsWidget_dccSend : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_dccSend(QWidget * parent);
	~OptionsWidget_dccSend();
};

#define KVI_OPTIONS_WIDGET_ICON_KviDccSendGeneralOptionsWidget KVI_SMALLICON_DCCMSG
#define KVI_OPTIONS_WIDGET_NAME_KviDccSendGeneralOptionsWidget __tr2qs_no_lookup("General")
#define KVI_OPTIONS_WIDGET_PARENT_KviDccSendGeneralOptionsWidget OptionsWidget_dccSend


class OptionsWidget_dccSendGeneral : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_dccSendGeneral(QWidget * parent);
	~OptionsWidget_dccSendGeneral();
};


#define KVI_OPTIONS_WIDGET_ICON_KviDccSendAdvancedOptionsWidget KVI_SMALLICON_DCCMSG
#define KVI_OPTIONS_WIDGET_NAME_KviDccSendAdvancedOptionsWidget __tr2qs_no_lookup("Advanced")
#define KVI_OPTIONS_WIDGET_PARENT_KviDccSendAdvancedOptionsWidget OptionsWidget_dccSend

class OptionsWidget_dccSendAdvanced : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_dccSendAdvanced(QWidget * parent);
	~OptionsWidget_dccSendAdvanced();
};


#define KVI_OPTIONS_WIDGET_ICON_KviDccChatOptionsWidget KVI_SMALLICON_DCCREQUEST
#define KVI_OPTIONS_WIDGET_NAME_KviDccChatOptionsWidget __tr2qs_no_lookup("Chat")
#define KVI_OPTIONS_WIDGET_PARENT_KviDccChatOptionsWidget OptionsWidget_dcc
#define KVI_OPTIONS_WIDGET_NOTCONTAINED_KviDccChatOptionsWidget true

class OptionsWidget_dccChat : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_dccChat(QWidget * parent);
	~OptionsWidget_dccChat();
};

#define KVI_OPTIONS_WIDGET_ICON_KviDccVoiceOptionsWidget KVI_SMALLICON_DCCVOICE
#define KVI_OPTIONS_WIDGET_NAME_KviDccVoiceOptionsWidget __tr2qs_no_lookup("Voice")
#define KVI_OPTIONS_WIDGET_PARENT_KviDccVoiceOptionsWidget OptionsWidget_dcc
#define KVI_OPTIONS_WIDGET_NOTCONTAINED_KviDccVoiceOptionsWidget true

class OptionsWidget_dccVoice : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_dccVoice(QWidget *);
	~OptionsWidget_dccVoice();
};


#endif //_OPTW_DCC_H_
