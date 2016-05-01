#ifndef _OPTW_URLHANDLERS_CPP_
#define _OPTW_URLHANDLERS_CPP_
//=============================================================================
//
//   File : OptionsWidget_urlHandlers.cpp
//   Creation date : Fri Aug 23 02:57:40 2002 GMT by Szymon Stefanek
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

#include "OptionsWidget_urlHandlers.h"

#include "KviOptions.h"
#include "KviLocale.h"

#include <QLayout>
#include <QLabel>
#include <QRadioButton>

OptionsWidget_urlHandlers::OptionsWidget_urlHandlers(QWidget * parent)
    : KviOptionsWidget(parent)
{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#define START_ROW 2
#else
#define START_ROW 1
#endif

	setObjectName("urlhandlers_options_widget");
	createLayout();

	KviTalGroupBox * gbox = addGroupBox(0, 0, 0, 0, Qt::Horizontal, __tr2qs_ctx("Mouse Handler", "options"));

	addLabel(gbox, __tr2qs_ctx("How many clicks to open links?", "options"));

	m_pClickRadio = new QRadioButton(__tr2qs_ctx("Single-click", "options"), gbox);
	m_pDoubleClickRadio = new QRadioButton(__tr2qs_ctx("Double-click", "options"), gbox);

	switch(KVI_OPTION_UINT(KviOption_uintUrlMouseClickNum))
	{
		case 1:
			m_pClickRadio->setChecked(true);
			break;
		case 2:
			m_pDoubleClickRadio->setChecked(true);
			break;
	}

	gbox = addGroupBox(0, 1, 0, 1, Qt::Horizontal, __tr2qs_ctx("Protocol Handler", "options"));

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	KviBoolSelector * b = addBoolSelector(gbox, __tr2qs_ctx("Use system URL handlers", "options"), KviOption_boolUseSystemUrlHandlers);
#endif

	m_pHttpHandler = addStringSelector(gbox, __tr2qs_ctx("<b>http://</b> handler command:", "options"), KviOption_stringUrlHttpCommand);
	m_pHttpHandler->setMinimumLabelWidth(225);
	m_pHttpsHandler = addStringSelector(gbox, __tr2qs_ctx("<b>https://</b> handler command:", "options"), KviOption_stringUrlHttpsCommand);
	m_pHttpsHandler->setMinimumLabelWidth(225);
	m_pFtpHandler = addStringSelector(gbox, __tr2qs_ctx("<b>ftp://</b> handler command:", "options"), KviOption_stringUrlFtpCommand);
	m_pFtpHandler->setMinimumLabelWidth(225);
	m_pMailtoHandler = addStringSelector(gbox, __tr2qs_ctx("<b>mailto:</b> handler command:", "options"), KviOption_stringUrlMailtoCommand);
	m_pMailtoHandler->setMinimumLabelWidth(225);
	m_pFileHandler = addStringSelector(gbox, __tr2qs_ctx("<b>file://</b> handler command:", "options"), KviOption_stringUrlFileCommand);
	m_pFileHandler->setMinimumLabelWidth(225);
	m_pOtherHandler = addStringSelector(gbox, __tr2qs_ctx("Unknown protocol handler command:", "options"), KviOption_stringUrlUnknownCommand);
	m_pOtherHandler->setMinimumLabelWidth(225);

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	m_pHttpHandler->setEnabled(!KVI_OPTION_BOOL(KviOption_boolUseSystemUrlHandlers));
	m_pHttpsHandler->setEnabled(!KVI_OPTION_BOOL(KviOption_boolUseSystemUrlHandlers));
	m_pFtpHandler->setEnabled(!KVI_OPTION_BOOL(KviOption_boolUseSystemUrlHandlers));
	m_pFileHandler->setEnabled(!KVI_OPTION_BOOL(KviOption_boolUseSystemUrlHandlers));
	m_pMailtoHandler->setEnabled(!KVI_OPTION_BOOL(KviOption_boolUseSystemUrlHandlers));
	m_pOtherHandler->setEnabled(!KVI_OPTION_BOOL(KviOption_boolUseSystemUrlHandlers));
	connect(b, SIGNAL(toggled(bool)), this, SLOT(toggleEditors(bool)));
#endif

	addRowSpacer(0, START_ROW + 6, 0, START_ROW + 6);
}

OptionsWidget_urlHandlers::~OptionsWidget_urlHandlers()
    = default;

void OptionsWidget_urlHandlers::toggleEditors(bool bToggled)
{
	m_pHttpHandler->setEnabled(!bToggled);
	m_pHttpsHandler->setEnabled(!bToggled);
	m_pFtpHandler->setEnabled(!bToggled);
	m_pFileHandler->setEnabled(!bToggled);
	m_pMailtoHandler->setEnabled(!bToggled);
	m_pOtherHandler->setEnabled(!bToggled);
}

void OptionsWidget_urlHandlers::commit()
{
	KviOptionsWidget::commit();

	if(m_pClickRadio->isChecked())
		KVI_OPTION_UINT(KviOption_uintUrlMouseClickNum) = 1;
	if(m_pDoubleClickRadio->isChecked())
		KVI_OPTION_UINT(KviOption_uintUrlMouseClickNum) = 2;

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	KVI_OPTION_STRING(KviOption_stringUrlHttpCommand)
	    .replace("\\\\", "@MAGIC@");
	KVI_OPTION_STRING(KviOption_stringUrlHttpCommand)
	    .replace("\\", "\\\\");
	KVI_OPTION_STRING(KviOption_stringUrlHttpCommand)
	    .replace("@MAGIC@", "\\\\");

	KVI_OPTION_STRING(KviOption_stringUrlHttpsCommand)
	    .replace("\\\\", "@MAGIC@");
	KVI_OPTION_STRING(KviOption_stringUrlHttpsCommand)
	    .replace("\\", "\\\\");
	KVI_OPTION_STRING(KviOption_stringUrlHttpsCommand)
	    .replace("@MAGIC@", "\\\\");

	KVI_OPTION_STRING(KviOption_stringUrlFtpCommand)
	    .replace("\\\\", "@MAGIC@");
	KVI_OPTION_STRING(KviOption_stringUrlFtpCommand)
	    .replace("\\", "\\\\");
	KVI_OPTION_STRING(KviOption_stringUrlFtpCommand)
	    .replace("@MAGIC@", "\\\\");

	KVI_OPTION_STRING(KviOption_stringUrlMailtoCommand)
	    .replace("\\\\", "@MAGIC@");
	KVI_OPTION_STRING(KviOption_stringUrlMailtoCommand)
	    .replace("\\", "\\\\");
	KVI_OPTION_STRING(KviOption_stringUrlMailtoCommand)
	    .replace("@MAGIC@", "\\\\");

	KVI_OPTION_STRING(KviOption_stringUrlFileCommand)
	    .replace("\\\\", "@MAGIC@");
	KVI_OPTION_STRING(KviOption_stringUrlFileCommand)
	    .replace("\\", "\\\\");
	KVI_OPTION_STRING(KviOption_stringUrlFileCommand)
	    .replace("@MAGIC@", "\\\\");

	KVI_OPTION_STRING(KviOption_stringUrlUnknownCommand)
	    .replace("\\\\", "@MAGIC@");
	KVI_OPTION_STRING(KviOption_stringUrlUnknownCommand)
	    .replace("\\", "\\\\");
	KVI_OPTION_STRING(KviOption_stringUrlUnknownCommand)
	    .replace("@MAGIC@", "\\\\");

	//Check for escaped " in url handler ( \" )
	KVI_OPTION_STRING(KviOption_stringUrlHttpCommand)
	    .replace("\\\\\"", "\\\"");
	KVI_OPTION_STRING(KviOption_stringUrlHttpsCommand)
	    .replace("\\\\\"", "\\\"");
	KVI_OPTION_STRING(KviOption_stringUrlFtpCommand)
	    .replace("\\\\\"", "\\\"");
	KVI_OPTION_STRING(KviOption_stringUrlMailtoCommand)
	    .replace("\\\\\"", "\\\"");
	KVI_OPTION_STRING(KviOption_stringUrlFileCommand)
	    .replace("\\\\\"", "\\\"");
	KVI_OPTION_STRING(KviOption_stringUrlUnknownCommand)
	    .replace("\\\\\"", "\\\"");
#endif
}

#endif //_OPTW_URLHANDLERS_CPP_
