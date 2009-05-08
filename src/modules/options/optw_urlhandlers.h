#ifndef _OPTW_URLHANDLERS_H_
#define _OPTW_URLHANDLERS_H_
//=============================================================================
//
//   File : optw_urlhandlers.h
//   Creation date : Fri Aug 23 02:57:41 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
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

class QRadioButton;

#define KVI_OPTIONS_WIDGET_ICON_KviUrlHandlersOptionsWidget KVI_SMALLICON_WORLD
#define KVI_OPTIONS_WIDGET_NAME_KviUrlHandlersOptionsWidget __tr2qs_no_lookup("URL handlers")
#define KVI_OPTIONS_WIDGET_PARENT_KviUrlHandlersOptionsWidget KviGeneralOptOptionsWidget
#define KVI_OPTIONS_WIDGET_PRIORITY_KviUrlHandlersOptionsWidget 70000
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviUrlHandlersOptionsWidget __tr2qs_no_lookup("url,programs")

class KviUrlHandlersOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviUrlHandlersOptionsWidget(QWidget * parent);
	~KviUrlHandlersOptionsWidget();

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
