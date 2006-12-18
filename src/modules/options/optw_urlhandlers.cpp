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
	createLayout(7,1);

	addStringSelector(0,0,0,0,__tr2qs_ctx("<b>http://</b> handler command:","options"),KviOption_stringUrlHttpCommand);
	addStringSelector(0,1,0,1,__tr2qs_ctx("<b>https://</b> handler command:","options"),KviOption_stringUrlHttpsCommand);
	addStringSelector(0,2,0,2,__tr2qs_ctx("<b>ftp://</b> handler command:","options"),KviOption_stringUrlFtpCommand);
	addStringSelector(0,3,0,3,__tr2qs_ctx("<b>mailto:</b> handler command:","options"),KviOption_stringUrlMailtoCommand);
	addStringSelector(0,4,0,4,__tr2qs_ctx("<b>file://</b> handler command:","options"),KviOption_stringUrlFileCommand);
	addStringSelector(0,5,0,5,__tr2qs_ctx("Unknown protocol handler command:","options"),KviOption_stringUrlUnknownCommand);

	addRowSpacer(0,6,0,6);
}

KviUrlHandlersOptionsWidget::~KviUrlHandlersOptionsWidget()
{
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

#include "m_optw_urlhandlers.moc"

#endif //_OPTW_URLHANDLERS_CPP_
