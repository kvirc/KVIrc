//=============================================================================
//
//   File : OptionsWidget_tray.cpp
//   Creation date : Mon 14 Aug 2006 by Alexey Uzhva
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2006-2008 Alexey Uzhva (wizard at opendoor dot ru)
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

#include "OptionsWidget_tray.h"

#include "KviMainWindow.h"
#include "KviInternalCommand.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "KviTalToolTip.h"

#include <QLayout>

OptionsWidget_tray::OptionsWidget_tray(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("tray_options_widget");
	createLayout();

	m_pEnable = addBoolSelector(0, 0, 0, 0, __tr2qs_ctx("Enable tray icon", "options"), KviOption_boolShowDockExtension);
#ifndef COMPILE_ON_MAC
	m_pMinimizeInTray = addBoolSelector(0, 1, 0, 1, __tr2qs_ctx("Minimize in tray", "options"), KviOption_boolMinimizeInTray);
#ifdef COMPILE_KDE_SUPPORT
	QString szTip = __tr2qs_ctx("Take into account that if you're using KDE, this won't work if "
	                            "you've set \"Keep window thumbnails\" as \"Always\" at \"Desktop Effects\" "
	                            "advanced settings.", "options");

	mergeTip(m_pMinimizeInTray, szTip);
#endif
#endif
	m_pCloseInTray = addBoolSelector(0, 2, 0, 2, __tr2qs_ctx("Close in tray", "options"), KviOption_boolCloseInTray);

	m_pEnableFlashing = addBoolSelector(0, 3, 0, 3, __tr2qs_ctx("Enable tray icon flashing", "options"), KviOption_boolEnableTrayIconFlashing);
	m_pLevelBasedNotify = addBoolSelector(0, 4, 0, 4, __tr2qs_ctx("Enable level-based message notification (not a message-type based)", "options"), KviOption_boolUseLevelBasedTrayNotification);

	m_pLevelsGroup = addGroupBox(0, 5, 0, 5, Qt::Horizontal, __tr2qs_ctx("Select Message Levels to Notify", "options"));

	addUIntSelector(m_pLevelsGroup, __tr2qs_ctx("Minimum level for low-priority messages:", "options"), KviOption_uintMinTrayLowLevelMessage, 1, 5, 1, true);

	addUIntSelector(m_pLevelsGroup, __tr2qs_ctx("Minimum level for high-priority message:", "options"), KviOption_uintMinTrayHighLevelMessage, 1, 5, 4, true);

	addRowSpacer(0, 6, 0, 6);

	connect(m_pEnable, SIGNAL(toggled(bool)), this, SLOT(setEnabled(bool)));
	connect(m_pLevelBasedNotify, SIGNAL(toggled(bool)), this, SLOT(setEnabled(bool)));
	setEnabled(true);
}

OptionsWidget_tray::~OptionsWidget_tray()
    = default;

void OptionsWidget_tray::setEnabled(bool)
{
	if(m_pEnable->isChecked())
	{
		m_pEnableFlashing->setEnabled(true);
		m_pLevelBasedNotify->setEnabled(true);
		m_pCloseInTray->setEnabled(true);
		if(m_pLevelBasedNotify->isChecked())
			m_pLevelsGroup->setEnabled(true);
		else
			m_pLevelsGroup->setEnabled(false);

		g_pMainWindow->executeInternalCommand(KVI_INTERNALCOMMAND_TRAYICON_SHOW);
	}
	else
	{
		m_pCloseInTray->setEnabled(false);
		m_pEnableFlashing->setEnabled(false);
		m_pLevelBasedNotify->setEnabled(false);
		m_pLevelsGroup->setEnabled(false);

		m_pCloseInTray->setChecked(false);
		m_pEnableFlashing->setChecked(false);
		m_pLevelBasedNotify->setChecked(false);
		m_pLevelsGroup->setChecked(false);

		g_pMainWindow->executeInternalCommand(KVI_INTERNALCOMMAND_TRAYICON_HIDE);
	}
}
