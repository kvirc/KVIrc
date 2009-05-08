#ifndef _OPTW_DCC_H_
#define _OPTW_DCC_H_
//=============================================================================
//
//   File : optw_dcc.h
//   Creation date : Fri Aug 17 22:08:08 2001 GMT by Szymon Stefanek
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

#define KVI_OPTIONS_WIDGET_ICON_KviDccOptionsWidget KVI_SMALLICON_DCCERROR
#define KVI_OPTIONS_WIDGET_NAME_KviDccOptionsWidget __tr2qs_no_lookup("DCC")
#define KVI_OPTIONS_WIDGET_PRIORITY_KviDccOptionsWidget 70000
#define KVI_OPTIONS_WIDGET_CONTAINER_KviDccOptionsWidget true

class KviDccOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviDccOptionsWidget(QWidget * parent);
	~KviDccOptionsWidget();
};



#define KVI_OPTIONS_WIDGET_ICON_KviDccGeneralOptionsWidget KVI_SMALLICON_DCCERROR
#define KVI_OPTIONS_WIDGET_NAME_KviDccGeneralOptionsWidget __tr2qs_no_lookup("General")
#define KVI_OPTIONS_WIDGET_PRIORITY_KviDccGeneralOptionsWidget 70000
#define KVI_OPTIONS_WIDGET_PARENT_KviDccGeneralOptionsWidget KviDccOptionsWidget

class KviDccGeneralOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviDccGeneralOptionsWidget(QWidget * parent);
	~KviDccGeneralOptionsWidget();
};

#define KVI_OPTIONS_WIDGET_ICON_KviDccAdvancedOptionsWidget KVI_SMALLICON_DCCERROR
#define KVI_OPTIONS_WIDGET_NAME_KviDccAdvancedOptionsWidget __tr2qs_no_lookup("Advanced")
#define KVI_OPTIONS_WIDGET_PRIORITY_KviDccAdvancedOptionsWidget 70000
#define KVI_OPTIONS_WIDGET_PARENT_KviDccAdvancedOptionsWidget KviDccOptionsWidget

class KviDccAdvancedOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviDccAdvancedOptionsWidget(QWidget * parent);
	~KviDccAdvancedOptionsWidget();
};




#define KVI_OPTIONS_WIDGET_ICON_KviDccSendOptionsWidget KVI_SMALLICON_DCCMSG
#define KVI_OPTIONS_WIDGET_NAME_KviDccSendOptionsWidget __tr2qs_no_lookup("File Transfer")
#define KVI_OPTIONS_WIDGET_PARENT_KviDccSendOptionsWidget KviDccOptionsWidget
#define KVI_OPTIONS_WIDGET_CONTAINER_KviDccSendOptionsWidget true


class KviDccSendOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviDccSendOptionsWidget(QWidget * parent);
	~KviDccSendOptionsWidget();
};

#define KVI_OPTIONS_WIDGET_ICON_KviDccSendGeneralOptionsWidget KVI_SMALLICON_DCCMSG
#define KVI_OPTIONS_WIDGET_NAME_KviDccSendGeneralOptionsWidget __tr2qs_no_lookup("General")
#define KVI_OPTIONS_WIDGET_PARENT_KviDccSendGeneralOptionsWidget KviDccSendOptionsWidget


class KviDccSendGeneralOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviDccSendGeneralOptionsWidget(QWidget * parent);
	~KviDccSendGeneralOptionsWidget();
};


#define KVI_OPTIONS_WIDGET_ICON_KviDccSendAdvancedOptionsWidget KVI_SMALLICON_DCCMSG
#define KVI_OPTIONS_WIDGET_NAME_KviDccSendAdvancedOptionsWidget __tr2qs_no_lookup("Advanced")
#define KVI_OPTIONS_WIDGET_PARENT_KviDccSendAdvancedOptionsWidget KviDccSendOptionsWidget

class KviDccSendAdvancedOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviDccSendAdvancedOptionsWidget(QWidget * parent);
	~KviDccSendAdvancedOptionsWidget();
};


#define KVI_OPTIONS_WIDGET_ICON_KviDccChatOptionsWidget KVI_SMALLICON_DCCREQUEST
#define KVI_OPTIONS_WIDGET_NAME_KviDccChatOptionsWidget __tr2qs_no_lookup("Chat")
#define KVI_OPTIONS_WIDGET_PARENT_KviDccChatOptionsWidget KviDccOptionsWidget
#define KVI_OPTIONS_WIDGET_NOTCONTAINED_KviDccChatOptionsWidget true

class KviDccChatOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviDccChatOptionsWidget(QWidget * parent);
	~KviDccChatOptionsWidget();
};

#define KVI_OPTIONS_WIDGET_ICON_KviDccVoiceOptionsWidget KVI_SMALLICON_DCCVOICE
#define KVI_OPTIONS_WIDGET_NAME_KviDccVoiceOptionsWidget __tr2qs_no_lookup("Voice")
#define KVI_OPTIONS_WIDGET_PARENT_KviDccVoiceOptionsWidget KviDccOptionsWidget
#define KVI_OPTIONS_WIDGET_NOTCONTAINED_KviDccVoiceOptionsWidget true

class KviDccVoiceOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviDccVoiceOptionsWidget(QWidget *);
	~KviDccVoiceOptionsWidget();
};


#endif //_OPTW_DCC_H_
