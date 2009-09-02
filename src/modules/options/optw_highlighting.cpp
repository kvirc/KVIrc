#ifndef _OPTW_HIGHLIGHTING_CPP_
#define _OPTW_HIGHLIGHTING_CPP_
//=============================================================================
//
//   File : optw_highlighting.cpp
//   Creation date : Sat Nov  3 18:09:35 2001 GMT by Szymon Stefanek
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

#include "optw_highlighting.h"
#include "kvi_options.h"
#include "kvi_locale.h"

#include <QLayout>
#include <QLabel>

KviAlertHighlightingOptionsWidget::KviAlertHighlightingOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("highlighting_options_widget");
}

KviAlertHighlightingOptionsWidget::~KviAlertHighlightingOptionsWidget()
{
}


KviHighlightingOptionsWidget::KviHighlightingOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("highlighting_options_widget");

	createLayout();

	KviBoolSelector * b;

	connect(
		addBoolSelector(0,0,0,0,__tr2qs_ctx("Enable word highlighting","options"),KviOption_boolUseWordHighlighting),
		SIGNAL(toggled(bool)),
		addStringListSelector(0,1,0,1,__tr2qs_ctx("Words to highlight:","options"),KviOption_stringlistHighlightWords,KVI_OPTION_BOOL(KviOption_boolUseWordHighlighting)),
		SLOT(setEnabled(bool)));

	connect(
		addBoolSelector(0,2,0,2,__tr2qs_ctx("Highlight every occurence of a word inside text","options"),KviOption_boolUseFullWordHighlighting),
		SIGNAL(toggled(bool)),
		addStringSelector(0,3,0,3,__tr2qs_ctx("Highlight only whole words using these word splitters:","options"),KviOption_stringWordSplitters,!KVI_OPTION_BOOL(KviOption_boolUseFullWordHighlighting)),
		SLOT(setDisabled(bool)));


	// This can be used even without Word highlighting
	b = addBoolSelector(0,4,0,4,__tr2qs_ctx("Highlight messages containing my nickname","options"),KviOption_boolAlwaysHighlightNick);
	mergeTip(b,__tr2qs_ctx("<center>If this option is enabled, KVIrc will highlight any user message containing your current nickname</center>","options"));
	b = addBoolSelector(0,5,0,5,__tr2qs_ctx("Flash the system taskbar entry on highlighted messages","options"),KviOption_boolFlashWindowOnHighlightedMessages);
	mergeTip(b,__tr2qs_ctx("<center>If this option is enabled, KVIrc will (attempt to) flash the system taskbar entry when a highlighted message is printed and KVIrc is not the active window</center>","options"));
	b = addBoolSelector(0,6,0,6,__tr2qs_ctx("Popup the notifier window on highlighted messages","options"),KviOption_boolPopupNotifierOnHighlightedMessages);
	mergeTip(b,__tr2qs_ctx("<center>If this option is enabled, KVIrc will popup a little notifier window in the low right corner of your desktop when a highlighted message is printed and KVIrc is not the active window</center>","options"));

	addRowSpacer(0,7,0,7);
}

KviHighlightingOptionsWidget::~KviHighlightingOptionsWidget()
{
}


KviAlertOptionsWidget::KviAlertOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("highlighting_options_widget");

	createLayout();

	KviTalGroupBox * g = addGroupBox(0,0,0,0,Qt::Horizontal,__tr2qs_ctx("Alert Restrictions","options"));
	KviBoolSelector * b3 = addBoolSelector(g,__tr2qs_ctx("Restrict alert","options"),KviOption_boolHighlightOnlyNormalMsg);
	mergeTip(b3, __tr2qs_ctx("<center>If this option is enabled, KVIrc will alert in the window list only if a normal message is received in a channel.<br>Actions like joins, parts and mode changes will be ignored.<br> This is useful if you are in channels with a high rate of traffic and only want to be alerted for messages that are interesting to you.</center>","options"));

	KviBoolSelector * b4 = addBoolSelector(g,__tr2qs_ctx("Alert for highlighted words","options"),KviOption_boolHighlightOnlyNormalMsgHighlightInChanToo,KVI_OPTION_BOOL(KviOption_boolHighlightOnlyNormalMsg));
	mergeTip(b4, __tr2qs_ctx("<center>If this option is enabled, the window list will also alert for messages which contain a word from the highlighted words list above.</center>","options"));

	KviBoolSelector * b5 = addBoolSelector(g,__tr2qs_ctx("Alert for query messages","options"),KviOption_boolHighlightOnlyNormalMsgQueryToo,KVI_OPTION_BOOL(KviOption_boolHighlightOnlyNormalMsg));
	mergeTip(b5, __tr2qs_ctx("<center>If this option is enabled, the window list will also alert for messages which are shown in queries.</center>","options"));
	connect(b3,SIGNAL(toggled(bool)),b4,SLOT(setEnabled(bool)));
	connect(b3,SIGNAL(toggled(bool)),b5,SLOT(setEnabled(bool)));

	KviBoolSelector * b6 = addBoolSelector(g,__tr2qs_ctx("Use custom alert level","options"),KviOption_boolHighlightOnlyAtCostumHighlightLevel);
	mergeTip(b6, __tr2qs_ctx("<center>If this option is enabled, KVIrc will alert in the window list only if the specified alert level is reached.</center>","options"));

	KviUIntSelector * b7 = addUIntSelector(g,__tr2qs_ctx("Minimum alert level:","options"),KviOption_uintMinHighlightLevel,1,KVI_MSGTYPE_MAXLEVEL,KVI_MSGTYPE_MAXLEVEL,KVI_OPTION_BOOL(KviOption_boolHighlightOnlyAtCostumHighlightLevel));
	b7->setEnabled(KVI_OPTION_BOOL(KviOption_boolHighlightOnlyAtCostumHighlightLevel));
	mergeTip(b7, __tr2qs_ctx("<center>This option sets the minimum alert level for the window list.</center>","options"));
	connect(b6,SIGNAL(toggled(bool)),b7,SLOT(setEnabled(bool)));

	addRowSpacer(0,1,0,1);
}

KviAlertOptionsWidget::~KviAlertOptionsWidget()
{
}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_optw_highlighting.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

#endif //_OPTW_HIGHLIGHTING_CPP_
