//=============================================================================
//
//   File : OptionsWidget_lag.cpp
//   Creation date : Wed Oct 16 10:45:54 CEST 2002 by Juanjo Álvarez (juanjux@yahoo.es)
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002 Juanjo Álvarez
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

#include "OptionsWidget_lag.h"

#include "KviOptions.h"
#include "KviLocale.h"
#include "KviTalToolTip.h"

#include <QLayout>

OptionsWidget_lag::OptionsWidget_lag(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("lag_options_widget");
	createLayout();

	KviBoolSelector * pUse = addBoolSelector(0, 0, 0, 0, __tr2qs_ctx("Enable lag meter", "options"), KviOption_boolUseLagMeterEngine);
	mergeTip(pUse, __tr2qs_ctx("This enables the lag meter engine, which checks at regular intervals how much latency the server has.", "options"));
	KviTalGroupBox * g = addGroupBox(0, 1, 0, 1, Qt::Horizontal, __tr2qs_ctx("Configuration", "options"), KVI_OPTION_BOOL(KviOption_boolUseLagMeterEngine));
	KviUIntSelector * pInterval = addUIntSelector(g, __tr2qs_ctx("Lag meter heartbeat:", "options"), KviOption_uintLagMeterHeartbeat, 2000, 10000, 5000,
		KVI_OPTION_BOOL(KviOption_boolUseLagMeterEngine));
	pInterval->setSuffix(__tr2qs_ctx(" msec", "options"));
	mergeTip(pInterval, __tr2qs_ctx("This option allows you to set the lag meter heartbeat interval.<br>"
                                        "The lower the heartbeat interval the higher will be the accuracy of the lag check "
                                        "but also higher CPU usage and data traffic to the server.<br>"
                                        "Please note that this is NOT the interval between pings sent to the server: "
                                        "the pings (if any) will be sent really less often. "
                                        "5000 is a reasonable value.", "options"));

	connect(pUse, SIGNAL(toggled(bool)), pInterval, SLOT(setEnabled(bool)));

	KviUIntSelector * pAlarm = addUIntSelector(g, __tr2qs_ctx("Trigger event if lag exceeds:", "options"), KviOption_uintLagAlarmTime, 5000, 1000000, 30000,
		KVI_OPTION_BOOL(KviOption_boolUseLagMeterEngine));
	pAlarm->setSuffix(__tr2qs_ctx(" msec", "options"));
	mergeTip(pAlarm, __tr2qs_ctx("This option controls the threshold for the "
                                     "OnLagAlarmTimeUp and OnLagAlarmTimeDown events. When the lag goes above "
	                             "the threshold OnLagAlarmTimeUp will be triggered and when the lag falls "
	                             "back below the threshold then OnLagAlarmTimeDown will be triggered", "options"));

	connect(pUse, SIGNAL(toggled(bool)), pAlarm, SLOT(setEnabled(bool)));

	KviBoolSelector * pShow = addBoolSelector(g, __tr2qs_ctx("Show lag in IRC context display", "options"), KviOption_boolShowLagOnContextDisplay,
		KVI_OPTION_BOOL(KviOption_boolUseLagMeterEngine));
	mergeTip(pShow, __tr2qs_ctx("This makes the IRC context display applet show the current lag after the user's nickname (in seconds)", "options"));

	connect(pUse, SIGNAL(toggled(bool)), pShow, SLOT(setEnabled(bool)));
	connect(pUse, SIGNAL(toggled(bool)), g, SLOT(setEnabled(bool)));

	addRowSpacer(0, 2, 0, 2);
}

OptionsWidget_lag::~OptionsWidget_lag()
    = default;
