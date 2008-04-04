#ifndef _OPTW_URLHANDLERS_CPP_
#define _OPTW_URLHANDLERS_CPP_
//
//   File : optw_urlhandlers.cpp
//   Creation date : Fri Aug 23 02:57:40 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
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

#include "optw_urlhandlers.h"

#include <qlayout.h>

#include "kvi_options.h"
#include "kvi_locale.h"

#include <qlabel.h>


KviUrlHandlersOptionsWidget::KviUrlHandlersOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent,"urlhandlers_options_widget")
{
#ifdef COMPILE_ON_WINDOWS
	#define START_ROW 1
#else
	#define START_ROW 0
#endif

	createLayout(START_ROW+7,1);

#ifdef COMPILE_ON_WINDOWS
	KviBoolSelector *b = addBoolSelector(0,0,0,0,__tr2qs_ctx("Use system URL handlers","options"),KviOption_boolUseSystemUrlHandlers);
#endif

	m_pHttpHandler=addStringSelector(0,START_ROW+0,0,START_ROW+0,__tr2qs_ctx("<b>http://</b> handler command:","options"),KviOption_stringUrlHttpCommand);
	m_pHttpsHandler=addStringSelector(0,START_ROW+1,0,START_ROW+1,__tr2qs_ctx("<b>https://</b> handler command:","options"),KviOption_stringUrlHttpsCommand);
	m_pFtpHandler=addStringSelector(0,START_ROW+2,0,START_ROW+2,__tr2qs_ctx("<b>ftp://</b> handler command:","options"),KviOption_stringUrlFtpCommand);
	m_pMailtoHandler=addStringSelector(0,START_ROW+3,0,START_ROW+3,__tr2qs_ctx("<b>mailto:</b> handler command:","options"),KviOption_stringUrlMailtoCommand);
	m_pFileHandler=addStringSelector(0,START_ROW+4,0,START_ROW+4,__tr2qs_ctx("<b>file://</b> handler command:","options"),KviOption_stringUrlFileCommand);
	m_pOtherHandler=addStringSelector(0,START_ROW+5,0,START_ROW+5,__tr2qs_ctx("Unknown protocol handler command:","options"),KviOption_stringUrlUnknownCommand);

#ifdef COMPILE_ON_WINDOWS
	m_pHttpHandler->setEnabled(!KVI_OPTION_BOOL(KviOption_boolUseSystemUrlHandlers));
	m_pHttpsHandler->setEnabled(!KVI_OPTION_BOOL(KviOption_boolUseSystemUrlHandlers));
	m_pFtpHandler->setEnabled(!KVI_OPTION_BOOL(KviOption_boolUseSystemUrlHandlers));
	m_pFileHandler->setEnabled(!KVI_OPTION_BOOL(KviOption_boolUseSystemUrlHandlers));
	m_pMailtoHandler->setEnabled(!KVI_OPTION_BOOL(KviOption_boolUseSystemUrlHandlers));
	m_pOtherHandler->setEnabled(!KVI_OPTION_BOOL(KviOption_boolUseSystemUrlHandlers));

	connect(b,SIGNAL(toggled(bool)),this,SLOT(toggleEditors(bool)));

#endif

	addRowSpacer(0,START_ROW+6,0,START_ROW+6);
}

KviUrlHandlersOptionsWidget::~KviUrlHandlersOptionsWidget()
{
}

void KviUrlHandlersOptionsWidget::toggleEditors(bool bToggled)
{
	m_pHttpHandler->setEnabled(!bToggled);
	m_pHttpsHandler->setEnabled(!bToggled);
	m_pFtpHandler->setEnabled(!bToggled);
	m_pFileHandler->setEnabled(!bToggled);
	m_pMailtoHandler->setEnabled(!bToggled);
	m_pOtherHandler->setEnabled(!bToggled);
}

void KviUrlHandlersOptionsWidget::commit()
{
	KviOptionsWidget::commit();

#ifdef COMPILE_ON_WINDOWS
	KVI_OPTION_STRING(KviOption_stringUrlHttpCommand).replace("\\\\","@MAGIC@","options");
	KVI_OPTION_STRING(KviOption_stringUrlHttpCommand).replace("\\","\\\\","options");
	KVI_OPTION_STRING(KviOption_stringUrlHttpCommand).replace("@MAGIC@","\\\\","options");

	KVI_OPTION_STRING(KviOption_stringUrlHttpsCommand).replace("\\\\","@MAGIC@","options");
	KVI_OPTION_STRING(KviOption_stringUrlHttpsCommand).replace("\\","\\\\","options");
	KVI_OPTION_STRING(KviOption_stringUrlHttpsCommand).replace("@MAGIC@","\\\\","options");

	KVI_OPTION_STRING(KviOption_stringUrlFtpCommand).replace("\\\\","@MAGIC@","options");
	KVI_OPTION_STRING(KviOption_stringUrlFtpCommand).replace("\\","\\\\","options");
	KVI_OPTION_STRING(KviOption_stringUrlFtpCommand).replace("@MAGIC@","\\\\","options");

	KVI_OPTION_STRING(KviOption_stringUrlMailtoCommand).replace("\\\\","@MAGIC@","options");
	KVI_OPTION_STRING(KviOption_stringUrlMailtoCommand).replace("\\","\\\\","options");
	KVI_OPTION_STRING(KviOption_stringUrlMailtoCommand).replace("@MAGIC@","\\\\","options");

	KVI_OPTION_STRING(KviOption_stringUrlFileCommand).replace("\\\\","@MAGIC@","options");
	KVI_OPTION_STRING(KviOption_stringUrlFileCommand).replace("\\","\\\\","options");
	KVI_OPTION_STRING(KviOption_stringUrlFileCommand).replace("@MAGIC@","\\\\","options");

	KVI_OPTION_STRING(KviOption_stringUrlUnknownCommand).replace("\\\\","@MAGIC@","options");
	KVI_OPTION_STRING(KviOption_stringUrlUnknownCommand).replace("\\","\\\\","options");
	KVI_OPTION_STRING(KviOption_stringUrlUnknownCommand).replace("@MAGIC@","\\\\","options");
	
	//Check for escaped " in url handler ( \" )
        KVI_OPTION_STRING(KviOption_stringUrlHttpCommand).replace("\\\\\"","\\\"","options");
        KVI_OPTION_STRING(KviOption_stringUrlHttpsCommand).replace("\\\\\"","\\\"","options");
        KVI_OPTION_STRING(KviOption_stringUrlFtpCommand).replace("\\\\\"","\\\"","options");
        KVI_OPTION_STRING(KviOption_stringUrlMailtoCommand).replace("\\\\\"","\\\"","options");
        KVI_OPTION_STRING(KviOption_stringUrlFileCommand).replace("\\\\\"","\\\"","options");
        KVI_OPTION_STRING(KviOption_stringUrlUnknownCommand).replace("\\\\\"","\\\"","options");
#endif
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_optw_urlhandlers.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

#endif //_OPTW_URLHANDLERS_CPP_
