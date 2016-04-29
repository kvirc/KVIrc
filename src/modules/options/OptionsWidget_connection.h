#ifndef _OPTW_CONNECTION_H_
#define _OPTW_CONNECTION_H_
//=============================================================================
//
//   File : OptionsWidget_connection.h
//   Creation date : Sat Nov 24 04:25:12 2001 GMT by Szymon Stefanek
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

class QRadioButton;

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_connection KviIconManager::World
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_connection __tr2qs_no_lookup("Connection")
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_connection 98000

class OptionsWidget_connection : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_connection(QWidget * parent);
	~OptionsWidget_connection();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_connectionAdvanced KviIconManager::Gui
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_connectionAdvanced __tr2qs_no_lookup("Advanced")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_connectionAdvanced OptionsWidget_connection
#define KVI_OPTIONS_WIDGET_CONTAINER_OptionsWidget_connectionAdvanced true

class OptionsWidget_connectionAdvanced : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_connectionAdvanced(QWidget * parent);
	~OptionsWidget_connectionAdvanced();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_connectionSsl KviIconManager::Ssl
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_connectionSsl __tr2qs_no_lookup("SSL")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_connectionSsl OptionsWidget_connectionAdvanced
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_connectionSsl 30

class OptionsWidget_connectionSsl : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_connectionSsl(QWidget * parent);
	~OptionsWidget_connectionSsl();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_connectionSocket KviIconManager::Socket
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_connectionSocket __tr2qs_no_lookup("Socket")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_connectionSocket OptionsWidget_connectionAdvanced
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_connectionSocket 20

class OptionsWidget_connectionSocket : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_connectionSocket(QWidget * parent);
	~OptionsWidget_connectionSocket();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_identService KviIconManager::Ident
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_identService __tr2qs_no_lookup("Ident Service")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_identService OptionsWidget_connectionAdvanced
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_identService 10

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
