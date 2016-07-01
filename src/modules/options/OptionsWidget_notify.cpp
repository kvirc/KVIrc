//=============================================================================
//
//   File : OptionsWidget_notify.cpp
//   Creation date : Sun Oct 28 15:11:29 2001 GMT by Szymon Stefanek
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

#include "OptionsWidget_notify.h"

#include "kvi_settings.h"
#include "KviOptions.h"
#include "KviLocale.h"

#include <QLayout>

OptionsWidget_notify::OptionsWidget_notify(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("notify_options_widget");
	createLayout();

	KviBoolSelector * b;
	KviTalGroupBox * g;
	KviUIntSelector * u;

	KviBoolSelector * notifyEnableBox = addBoolSelector(0, 0, 0, 0, __tr2qs_ctx("Use online notify list", "options"), KviOption_boolUseNotifyList);
	b = notifyEnableBox;

	g = addGroupBox(0, 1, 0, 1, Qt::Horizontal, __tr2qs_ctx("Configuration", "options"));
	connect(b, SIGNAL(toggled(bool)), g, SLOT(setEnabled(bool)));

	b = addBoolSelector(g, __tr2qs_ctx("Show notifications in active window", "options"), KviOption_boolNotifyListChangesToActiveWindow, KVI_OPTION_BOOL(KviOption_boolUseNotifyList));
	connect(notifyEnableBox, SIGNAL(toggled(bool)), b, SLOT(setEnabled(bool)));

	b = addBoolSelector(g, __tr2qs_ctx("Flash window when users are going online", "options"), KviOption_boolFlashWindowOnNotifyOnLine, KVI_OPTION_BOOL(KviOption_boolUseNotifyList));
	connect(notifyEnableBox, SIGNAL(toggled(bool)), b, SLOT(setEnabled(bool)));

	b = addBoolSelector(g, __tr2qs_ctx("Popup notifier when users are going online", "options"), KviOption_boolPopupNotifierOnNotifyOnLine, KVI_OPTION_BOOL(KviOption_boolUseNotifyList));
	connect(notifyEnableBox, SIGNAL(toggled(bool)), b, SLOT(setEnabled(bool)));

	g = addGroupBox(0, 2, 0, 2, Qt::Horizontal, __tr2qs_ctx("Advanced Configuration", "options"));
	connect(notifyEnableBox, SIGNAL(toggled(bool)), g, SLOT(setEnabled(bool)));

	b = addBoolSelector(g, __tr2qs_ctx("Check USERHOST for online users", "options"), KviOption_boolNotifyListSendUserhostForOnlineUsers, KVI_OPTION_BOOL(KviOption_boolUseNotifyList));
	connect(notifyEnableBox, SIGNAL(toggled(bool)), b, SLOT(setEnabled(bool)));

	b = addBoolSelector(g, __tr2qs_ctx("Use smart notify list manager", "options"), KviOption_boolUseIntelligentNotifyListManager, KVI_OPTION_BOOL(KviOption_boolUseNotifyList));
	connect(notifyEnableBox, SIGNAL(toggled(bool)), b, SLOT(setEnabled(bool)));

	b = addBoolSelector(g, __tr2qs_ctx("Use the WATCH method if available", "options"), KviOption_boolUseWatchListIfAvailable, KVI_OPTION_BOOL(KviOption_boolUseNotifyList));
	connect(notifyEnableBox, SIGNAL(toggled(bool)), b, SLOT(setEnabled(bool)));

	u = addUIntSelector(g, __tr2qs_ctx("Check interval:", "options"), KviOption_uintNotifyListCheckTimeInSecs, 5, 3600, 180, KVI_OPTION_BOOL(KviOption_boolUseNotifyList));
	u->setSuffix(__tr2qs_ctx(" sec", "options"));
	connect(notifyEnableBox, SIGNAL(toggled(bool)), u, SLOT(setEnabled(bool)));

	u = addUIntSelector(g, __tr2qs_ctx("ISON delay:", "options"), KviOption_uintNotifyListIsOnDelayTimeInSecs, 5, 180, 6, KVI_OPTION_BOOL(KviOption_boolUseNotifyList));
	u->setSuffix(__tr2qs_ctx(" sec", "options"));
	connect(notifyEnableBox, SIGNAL(toggled(bool)), u, SLOT(setEnabled(bool)));

	u = addUIntSelector(g, __tr2qs_ctx("USERHOST delay:", "options"), KviOption_uintNotifyListUserhostDelayTimeInSecs, 5, 180, 6, KVI_OPTION_BOOL(KviOption_boolUseNotifyList));
	u->setSuffix(__tr2qs_ctx(" sec", "options"));
	connect(notifyEnableBox, SIGNAL(toggled(bool)), u, SLOT(setEnabled(bool)));

	addLabel(0, 3, 0, 3, __tr2qs_ctx("<p><b>Note:</b><br>The notify list is managed using the \"Registered Users\" settings.</p>", "options"));

	addRowSpacer(0, 4, 0, 4);
}

OptionsWidget_notify::~OptionsWidget_notify()
    = default;
