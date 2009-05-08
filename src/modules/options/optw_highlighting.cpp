#ifndef _OPTW_HIGHLIGHTING_CPP_
#define _OPTW_HIGHLIGHTING_CPP_
//
//   File : optw_highlighting.cpp
//   Creation date : Sat Nov  3 18:09:35 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//

#include "optw_highlighting.h"

#include <qlayout.h>

#include "kvi_options.h"
#include "kvi_locale.h"

#include <qlabel.h>

KviAlertHighlightingOptionsWidget::KviAlertHighlightingOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent,"highlighting_options_widget")
{
}

KviAlertHighlightingOptionsWidget::~KviAlertHighlightingOptionsWidget()
{
}



KviHighlightingOptionsWidget::KviHighlightingOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent,"highlighting_options_widget")
{
	createLayout(7,1);

	KviBoolSelector * b;

	connect(
		addBoolSelector(0,0,0,0,__tr2qs_ctx("Enable word highlighting","options"),KviOption_boolUseWordHighlighting),
		SIGNAL(toggled(bool)),
		addStringListSelector(0,1,0,1,__tr2qs_ctx("Words to highlight:","options"),KviOption_stringlistHighlightWords,KVI_OPTION_BOOL(KviOption_boolUseWordHighlighting)),
		SLOT(setEnabled(bool)));
	addStringSelector(0,2,0,2,__tr2qs_ctx("Word splitters:","options"),KviOption_stringWordSplitters);
	// This can be used even without Word highlighting
	b = addBoolSelector(0,3,0,3,__tr2qs_ctx("Highlight messages containing my nickname","options"),KviOption_boolAlwaysHighlightNick);
#ifdef COMPILE_INFO_TIPS
	mergeTip(b,__tr2qs_ctx("<center>If this option is enabled, KVIrc will highlight any user message containing your current nickname</center>","options"));
#endif
	b = addBoolSelector(0,4,0,4,__tr2qs_ctx("Flash the system taskbar entry on highlighted messages","options"),KviOption_boolFlashWindowOnHighlightedMessages);
#ifdef COMPILE_INFO_TIPS
	mergeTip(b,__tr2qs_ctx("<center>If this option is enabled, KVIrc will (attempt to) flash the system taskbar entry when a highlighted message is printed and KVIrc is not the active window</center>","options"));
#endif
	b = addBoolSelector(0,5,0,5,__tr2qs_ctx("Popup the notifier window on highlighted messages","options"),KviOption_boolPopupNotifierOnHighlightedMessages);
#ifdef COMPILE_INFO_TIPS
	mergeTip(b,__tr2qs_ctx("<center>If this option is enabled, KVIrc will popup a little notifier window in the low right corner of your desktop when a highlighted message is printed and KVIrc is not the active window</center>","options"));
#endif

	addRowSpacer(0,6,0,6);
}

KviHighlightingOptionsWidget::~KviHighlightingOptionsWidget()
{
}






KviAlertOptionsWidget::KviAlertOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent,"highlighting_options_widget")
{
	createLayout(2,1);

	KviBoolSelector * b;

	KviTalGroupBox * g = addGroupBox(0,0,0,0,1,Qt::Horizontal,__tr2qs_ctx("Alert Restrictions","options"));
	KviBoolSelector * b3 = addBoolSelector(g,__tr2qs_ctx("Restrict alert","options"),KviOption_boolHighlightOnlyNormalMsg);
#ifdef COMPILE_INFO_TIPS
	mergeTip(b3, __tr2qs_ctx("<center>If this option is enabled, KVIrc will alert in the taskbar only if a normal message is received in a channel.<br>Actions like joins, parts and mode changes will be ignored.<br> This is useful if you are in channels with a high rate of traffic and only want to be alerted for messages that are interesting to you.</center>","options"));
#endif
	KviBoolSelector * b4 = addBoolSelector(g,__tr2qs_ctx("Alert for highlighted words","options"),KviOption_boolHighlightOnlyNormalMsgHighlightInChanToo,KVI_OPTION_BOOL(KviOption_boolHighlightOnlyNormalMsg));
#ifdef COMPILE_INFO_TIPS
	mergeTip(b4, __tr2qs_ctx("<center>If this option is enabled, the taskbar will also alert for messages which contain a word from the highlighted words list above.</center>","options"));
#endif
	KviBoolSelector * b5 = addBoolSelector(g,__tr2qs_ctx("Alert for query messages","options"),KviOption_boolHighlightOnlyNormalMsgQueryToo,KVI_OPTION_BOOL(KviOption_boolHighlightOnlyNormalMsg));
#ifdef COMPILE_INFO_TIPS
	mergeTip(b5, __tr2qs_ctx("<center>If this option is enabled, the taskbar will also alert for messages which are shown in queries.</center>","options"));
#endif
	connect(b3,SIGNAL(toggled(bool)),b4,SLOT(setEnabled(bool)));
	connect(b3,SIGNAL(toggled(bool)),b5,SLOT(setEnabled(bool)));

	KviBoolSelector * b6 = addBoolSelector(g,__tr2qs_ctx("Use custom alert level","options"),KviOption_boolHighlightOnlyAtCostumHighlightLevel);
#ifdef COMPILE_INFO_TIPS
	mergeTip(b6, __tr2qs_ctx("<center>If this option is enabled, KVIrc will alert in the taskbar only if the specified alert level is reached.</center>","options"));
#endif
	KviUIntSelector * b7 = addUIntSelector(g,__tr2qs_ctx("Minimum alert level:","options"),KviOption_uintMinHighlightLevel,1,KVI_MSGTYPE_MAXLEVEL,KVI_MSGTYPE_MAXLEVEL,KVI_OPTION_UINT(KviOption_boolHighlightOnlyAtCostumHighlightLevel));
	b7->setEnabled(KVI_OPTION_BOOL(KviOption_boolHighlightOnlyAtCostumHighlightLevel));
#ifdef COMPILE_INFO_TIPS
	mergeTip(b6, __tr2qs_ctx("<center>This option sets the minimum alert level for the taskbar.</center>","options"));
#endif
	connect(b6,SIGNAL(toggled(bool)),b7,SLOT(setEnabled(bool)));

	addRowSpacer(0,1,0,1);
}

KviAlertOptionsWidget::~KviAlertOptionsWidget()
{
}


#include "m_optw_highlighting.moc"

#endif //_OPTW_HIGHLIGHTING_CPP_
