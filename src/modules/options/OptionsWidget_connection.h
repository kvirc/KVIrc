#ifndef _OPTW_CONNECTION_H_
#define _OPTW_CONNECTION_H_
//=============================================================================
//
//   File : OptionsWidget_connection.h
//   Creation date : Sat Nov 24 04:25:12 2001 GMT by Szymon Stefanek
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

class QRadioButton;

#define KVI_OPTIONS_WIDGET_ICON_KviConnectionOptionsWidget KVI_SMALLICON_WORLD
#define KVI_OPTIONS_WIDGET_NAME_KviConnectionOptionsWidget __tr2qs_no_lookup("Connection")
#define KVI_OPTIONS_WIDGET_PRIORITY_KviConnectionOptionsWidget 98000

class OptionsWidget_connection : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_connection(QWidget * parent);
	~OptionsWidget_connection();
};


#define KVI_OPTIONS_WIDGET_ICON_KviConnectionAdvancedOptionsWidget KVI_SMALLICON_WORLD
#define KVI_OPTIONS_WIDGET_NAME_KviConnectionAdvancedOptionsWidget __tr2qs_no_lookup("Advanced")
#define KVI_OPTIONS_WIDGET_PARENT_KviConnectionAdvancedOptionsWidget OptionsWidget_connection
#define KVI_OPTIONS_WIDGET_CONTAINER_KviConnectionAdvancedOptionsWidget true

class OptionsWidget_connectionAdvanced : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_connectionAdvanced(QWidget * parent);
	~OptionsWidget_connectionAdvanced();
};


#define KVI_OPTIONS_WIDGET_ICON_KviSSLOptionsWidget KVI_SMALLICON_SSL
#define KVI_OPTIONS_WIDGET_NAME_KviSSLOptionsWidget __tr2qs_no_lookup("SSL")
#define KVI_OPTIONS_WIDGET_PARENT_KviSSLOptionsWidget OptionsWidget_connectionAdvanced
#define KVI_OPTIONS_WIDGET_PRIORITY_KviSSLOptionsWidget 30



class OptionsWidget_connectionSsl : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_connectionSsl(QWidget * parent);
	~OptionsWidget_connectionSsl();
};



#define KVI_OPTIONS_WIDGET_ICON_KviTransportOptionsWidget KVI_SMALLICON_SOCKETERROR
#define KVI_OPTIONS_WIDGET_NAME_KviTransportOptionsWidget __tr2qs_no_lookup("Socket")
#define KVI_OPTIONS_WIDGET_PARENT_KviTransportOptionsWidget OptionsWidget_connectionAdvanced
#define KVI_OPTIONS_WIDGET_PRIORITY_KviTransportOptionsWidget 20


class OptionsWidget_connectionSocket : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_connectionSocket(QWidget * parent);
	~OptionsWidget_connectionSocket();
};


#define KVI_OPTIONS_WIDGET_ICON_KviIdentOptionsWidget KVI_SMALLICON_IDENT
#define KVI_OPTIONS_WIDGET_NAME_KviIdentOptionsWidget __tr2qs_no_lookup("Ident Service")
#define KVI_OPTIONS_WIDGET_PARENT_KviIdentOptionsWidget OptionsWidget_connectionAdvanced
#define KVI_OPTIONS_WIDGET_PRIORITY_KviIdentOptionsWidget 10


class OptionsWidget_identService : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_identService(QWidget * parent);
	~OptionsWidget_identService();

	virtual void commit();
protected:
	KviBoolSelector * m_pEnableIdent;
	KviBoolSelector * m_pEnableIpv6;
	KviBoolSelector * m_pIpv4InIpv6;

	QRadioButton * m_pConsoleRadio;
	QRadioButton * m_pActiveRadio;
	QRadioButton * m_pQuietRadio;
public slots:
	void enableIpv4InIpv6(bool);
};


#endif //_OPTW_CONNECTION_H_
