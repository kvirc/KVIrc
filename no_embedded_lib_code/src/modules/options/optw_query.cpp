//=============================================================================
//
//   File : optw_query.cpp
//   Creation date : Mon Oct 29 15:37:16 2001 GMT by Szymon Stefanek
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


#include "optw_query.h"

#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_settings.h"

#include <QLayout>


KviQueryOptionsWidget::KviQueryOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("query_options_widget");

	createLayout();

	KviTalGroupBox * g = addGroupBox(0,0,1,0,Qt::Horizontal,__tr2qs_ctx("Open Query For","options"));
	KviBoolSelector * b  = addBoolSelector(g, __tr2qs_ctx("Private messages","options"),KviOption_boolCreateQueryOnPrivmsg);
	mergeTip(b,
		__tr2qs_ctx("<center>This option enables query window creation " \
			"when a private message (PRIVMSG) is received.<br>" \
			"If you disable this, private messages will " \
			"be shown in the active window or a common channel.</center>","options"));

	b = addBoolSelector(g, __tr2qs_ctx("Private notices","options"),KviOption_boolCreateQueryOnNotice);
	mergeTip(b,
		__tr2qs_ctx("<center>This option enables query window creation " \
			"when a private notice (NOTICE) is received.<br>" \
			"If you disable this, private notices will " \
			"be shown in the active window or a common channel.</center>","options"));


	b = addBoolSelector(0,1,1,1,__tr2qs_ctx("Always open queries as minimized","options"),KviOption_boolCreateMinimizedQuery);

	mergeTip(b,
		__tr2qs_ctx("<center>This option causes newly created " \
			"query windows to be immediately minimized.<br>" \
			"Enable this if you don't like queries " \
			"popping up while you're typing something in a channel. :D</center>","options"));


	b = addBoolSelector(0,2,1,2, __tr2qs_ctx("Enable target user tracking","options"),KviOption_boolEnableQueryTracing);
	mergeTip(b,
		__tr2qs_ctx("<center>This option will enable target user tracking.<br>" \
			"Some actions of the target user (e.g. joins and parts) " \
			"will be displayed in the window.<br></center>","options"));

#if (defined(COMPILE_ON_WINDOWS) || defined(COMPILE_KDE_SUPPORT) || defined(COMPILE_ON_MINGW))
	b = addBoolSelector(0,3,1,3,__tr2qs_ctx("Flash system taskbar on new query message","options"),KviOption_boolFlashQueryWindowOnNewMessages);
		mergeTip(b,
			__tr2qs_ctx("<center>This option causes the system taskbar entry for KVIrc to flash when a new query message " \
				"is received and the KVIrc window is not the active.</center>","options"));
#endif

	b = addBoolSelector(0,4,1,4, __tr2qs_ctx("Popup notifier on new query message","options"),KviOption_boolPopupNotifierOnNewQueryMessages);
	mergeTip(b,
		__tr2qs_ctx("<center>This option causes a small notifier window to pop up " \
			"in the low right corner of the screen when a new message is received " \
			"and the KVIrc window is not active.</center>","options"));
	b = addBoolSelector(0,5,1,5, __tr2qs_ctx("Show information about query target at the top of the query","options"),KviOption_boolShowExtendedInfoInQueryLabel);
	mergeTip(b,
		__tr2qs_ctx("<center>This option enables query window information " \
			"label. It can show you known information about query target at the top of the window.<br>" \
			"Uncheck if you think,that it wastes your query space/" \
			"</center>","options"));

	b = addBoolSelector(0,6,1,6,__tr2qs_ctx("Paste last query log","options"),KviOption_boolPasteLastLogOnQueryJoin);

	KviTalHBox * box = new KviTalHBox(this);
	addWidgetToLayout(box,0,7,1,7);

	KviUIntSelector * u = addUIntSelector(box,__tr2qs_ctx("Paste up to:","options"),KviOption_uintLinesToPasteOnQueryJoin,0,50,10,KVI_OPTION_BOOL(KviOption_boolPasteLastLogOnQueryJoin));
	u->setSuffix(__tr2qs_ctx(" lines","options"));
	mergeTip(u,__tr2qs_ctx("<center>Minimum value: <b>0 lines</b><br>Maximum value: <b>50 lines</b></center>","options"));
	connect(b,SIGNAL(toggled(bool)),u,SLOT(setEnabled(bool)));
	u = addUIntSelector(box,__tr2qs_ctx("Interval:","options"),KviOption_uintDaysIntervalToPasteOnQueryJoin,1,10,10,KVI_OPTION_BOOL(KviOption_boolPasteLastLogOnQueryJoin));
	u->setSuffix(__tr2qs_ctx(" days","options"));
	mergeTip(u,__tr2qs_ctx("<center>Minimum value: <b>0 days</b><br>Maximum value: <b>10 days</b></center>","options"));
	connect(b,SIGNAL(toggled(bool)),u,SLOT(setEnabled(bool)));

	addRowSpacer(0,8,1,8);
}

KviQueryOptionsWidget::~KviQueryOptionsWidget()
{
}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_optw_query.moc"
#endif //COMPILE_USE_STANDALONE_MOC_SOURCES
