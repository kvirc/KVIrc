#ifndef _OPTW_URLHANDLERS_H_
#define _OPTW_URLHANDLERS_H_
//=============================================================================
//
//   File : OptionsWidget_urlHandlers.h
//   Creation date : Fri Aug 23 02:57:41 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

class QRadioButton;

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_urlHandlers KviIconManager::Url
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_urlHandlers __tr2qs_no_lookup("URL Handlers")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_urlHandlers OptionsWidget_general
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_urlHandlers 70000
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_urlHandlers __tr2qs_no_lookup("URL,programs")

class OptionsWidget_urlHandlers : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_urlHandlers(QWidget * parent);
	~OptionsWidget_urlHandlers();

	virtual void commit();

protected:
	QRadioButton * m_pClickRadio;
	QRadioButton * m_pDoubleClickRadio;

	KviStringSelector * m_pHttpHandler;
	KviStringSelector * m_pHttpsHandler;
	KviStringSelector * m_pFtpHandler;
	KviStringSelector * m_pMailtoHandler;
	KviStringSelector * m_pFileHandler;
	KviStringSelector * m_pOtherHandler;
public slots:
	void toggleEditors(bool);
};

#endif //_OPTW_URLHANDLERS_H_
