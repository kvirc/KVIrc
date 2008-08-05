//
//   File : optw_notify.cpp
//   Creation date : Sun Oct 28 15:11:29 2001 GMT by Szymon Stefanek
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

#include "optw_notify.h"

#include "kvi_settings.h"
#include "kvi_options.h"
#include "kvi_locale.h"

#include <QLayout>

KviNotifyOptionsWidget::KviNotifyOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("notify_options_widget");

	createLayout();

	KviBoolSelector * b = addBoolSelector(0,0,0,0,__tr2qs_ctx("Use online notify list","options"),KviOption_boolUseNotifyList);
	KviTalGroupBox *g = addGroupBox(0,1,0,1,Qt::Horizontal,__tr2qs_ctx("Configuration","options"));
	connect(b,SIGNAL(toggled(bool)),g,SLOT(setEnabled(bool)));

	connect(b,
		SIGNAL(toggled(bool)),
		addBoolSelector(g,__tr2qs_ctx("Show notifications in active window","options"),
				KviOption_boolNotifyListChangesToActiveWindow,KVI_OPTION_BOOL(KviOption_boolUseNotifyList)),
		SLOT(setEnabled(bool)));
	connect(b,
		SIGNAL(toggled(bool)),
		addBoolSelector(g,__tr2qs_ctx("Flash window when users are going online","options"),
				KviOption_boolFlashWindowOnNotifyOnLine,KVI_OPTION_BOOL(KviOption_boolUseNotifyList)),
		SLOT(setEnabled(bool)));
	connect(b,
		SIGNAL(toggled(bool)),
		addBoolSelector(g,__tr2qs_ctx("Popup notifier when users are going online","options"),
				KviOption_boolPopupNotifierOnNotifyOnLine,KVI_OPTION_BOOL(KviOption_boolUseNotifyList)),
		SLOT(setEnabled(bool)));

	g = addGroupBox(0,2,0,2,Qt::Horizontal,__tr2qs_ctx("Advanced configuration","options"));
	connect(b,SIGNAL(toggled(bool)),g,SLOT(setEnabled(bool)));

	connect(b,
		SIGNAL(toggled(bool)),
		addBoolSelector(g,__tr2qs_ctx("Check USERHOST for online users","options"),
				KviOption_boolNotifyListSendUserhostForOnlineUsers,KVI_OPTION_BOOL(KviOption_boolUseNotifyList)),
		SLOT(setEnabled(bool)));
	connect(b,
		SIGNAL(toggled(bool)),
		addBoolSelector(g,__tr2qs_ctx("Use \"smart\" notify list manager","options"),
				KviOption_boolUseIntelligentNotifyListManager,KVI_OPTION_BOOL(KviOption_boolUseNotifyList)),
		SLOT(setEnabled(bool)));
	connect(b,
		SIGNAL(toggled(bool)),
		addBoolSelector(g,__tr2qs_ctx("Use the WATCH method if available","options"),
				KviOption_boolUseWatchListIfAvailable,KVI_OPTION_BOOL(KviOption_boolUseNotifyList)),
		SLOT(setEnabled(bool)));
	connect(b,
		SIGNAL(toggled(bool)),
		addUIntSelector(g,__tr2qs_ctx("Check interval (in seconds)","options"),
				KviOption_uintNotifyListCheckTimeInSecs,
				5,3600,180,KVI_OPTION_BOOL(KviOption_boolUseNotifyList)),
		SLOT(setEnabled(bool)));
	connect(b,
		SIGNAL(toggled(bool)),
		addUIntSelector(g,__tr2qs_ctx("ISON delay (in seconds)","options"),
				KviOption_uintNotifyListIsOnDelayTimeInSecs,
				5,180,6,KVI_OPTION_BOOL(KviOption_boolUseNotifyList)),
		SLOT(setEnabled(bool)));
	connect(b,
		SIGNAL(toggled(bool)),
		addUIntSelector(g,__tr2qs_ctx("USERHOST delay (in seconds)","options"),
				KviOption_uintNotifyListUserhostDelayTimeInSecs,
				5,180,6,KVI_OPTION_BOOL(KviOption_boolUseNotifyList)),
		SLOT(setEnabled(bool)));

	addLabel(0,3,0,3,__tr2qs_ctx("<p><b>Note:</b><br>The notify list is managed using the \"Registered Users\" settings.</p>","options"));
	addRowSpacer(0,4,0,4);
}

KviNotifyOptionsWidget::~KviNotifyOptionsWidget()
{
}


KviNotifierOptionsWidget::KviNotifierOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("notifier_options_widget");

	createLayout();

	KviBoolSelector * b = addBoolSelector(0,0,0,0,__tr2qs_ctx("Enable the notifier","options"),KviOption_boolEnableNotifier);
	QString tip = "<center>";
	tip += __tr2qs_ctx("This is an option for the impatient: it allows to forcibly and permanently disable " \
					"the notifier window. Please note that if this option is activated then " \
					"the notifier will NOT popup even if all the other options around specify " \
					"to use it in response to particular events. Also note that this option " \
					"will make all the /notifier.* commands fail silently.","options");
	tip += "</center>";
	mergeTip(b,tip);
	addBoolSelector(0,1,0,1,__tr2qs_ctx("Enable notifier window flashing","options"),KviOption_boolNotifierFlashing);
	addBoolSelector(0,2,0,2,__tr2qs_ctx("Enable notifier window fade effect","options"),KviOption_boolNotifierFading);

	addRowSpacer(0,3,0,3);
}

KviNotifierOptionsWidget::~KviNotifierOptionsWidget()
{
}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_optw_notify.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
