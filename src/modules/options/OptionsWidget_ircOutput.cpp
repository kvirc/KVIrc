//=============================================================================
//
//   File : OptionsWidget_ircOutput.cpp
//   Creation date : Wed Nov 15 2000 11:45:06 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "OptionsWidget_ircOutput.h"

#include "kvi_settings.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "KviSelectors.h"
#include "KviTalToolTip.h"

#include <QLayout>
#include <QComboBox>
#include <QLabel>

OptionsWidget_ircOutput::OptionsWidget_ircOutput(QWidget * pParent)
    : KviOptionsWidget(pParent)
{
	setObjectName("ircoutput_options_widget");
	createLayout();

	QLabel * pLabel = new QLabel(__tr2qs_ctx("Output verbosity:", "options"), this);
	addWidgetToLayout(pLabel, 0, 0, 0, 0);

	m_pVerbosityCombo = new QComboBox(this);
	m_pVerbosityCombo->addItem(__tr2qs_ctx("Mute", "options"));
	m_pVerbosityCombo->addItem(__tr2qs_ctx("Quiet", "options"));
	m_pVerbosityCombo->addItem(__tr2qs_ctx("Normal", "options"));
	m_pVerbosityCombo->addItem(__tr2qs_ctx("Verbose", "options"));
	m_pVerbosityCombo->addItem(__tr2qs_ctx("Paranoic", "options"));
	addWidgetToLayout(m_pVerbosityCombo, 1, 0, 1, 0);

	if(KVI_OPTION_UINT(KviOption_uintOutputVerbosityLevel) > KVI_VERBOSITY_LEVEL_PARANOIC)
		KVI_OPTION_UINT(KviOption_uintOutputVerbosityLevel) = KVI_VERBOSITY_LEVEL_NORMAL;
	m_pVerbosityCombo->setCurrentIndex(KVI_OPTION_UINT(KviOption_uintOutputVerbosityLevel));

	pLabel = new QLabel(__tr2qs_ctx("DateTime format:", "options"), this);
	addWidgetToLayout(pLabel, 0, 1, 0, 1);

	m_pDatetimeCombo = new QComboBox(this);
	m_pDatetimeCombo->addItem(__tr2qs_ctx("Classic format", "options"));
	m_pDatetimeCombo->addItem(__tr2qs_ctx("ISO 8601 format", "options"));
	m_pDatetimeCombo->addItem(__tr2qs_ctx("System locale format", "options"));
	addWidgetToLayout(m_pDatetimeCombo, 1, 1, 1, 1);

	m_pDatetimeCombo->setCurrentIndex(KVI_OPTION_UINT(KviOption_uintOutputDatetimeFormat));

	KviTalGroupBox * pGroup = addGroupBox(0, 2, 1, 2, Qt::Horizontal, __tr2qs_ctx("Show in Active Window", "options"));
	addBoolSelector(pGroup, __tr2qs_ctx("External messages", "options"), KviOption_boolExternalMessagesToActiveWindow);
	addBoolSelector(pGroup, __tr2qs_ctx("External CTCP replies", "options"), KviOption_boolCtcpRepliesToActiveWindow);
	addBoolSelector(pGroup, __tr2qs_ctx("WHOIS replies", "options"), KviOption_boolWhoisRepliesToActiveWindow);
	addBoolSelector(pGroup, __tr2qs_ctx("WHO replies", "options"), KviOption_boolWhoRepliesToActiveWindow);
	addBoolSelector(pGroup, __tr2qs_ctx("ChanServ and NickServ notices", "options"), KviOption_boolServicesNoticesToActiveWindow);
	addBoolSelector(pGroup, __tr2qs_ctx("Invite messages", "options"), KviOption_boolInvitesToActiveWindow);
	addBoolSelector(pGroup, __tr2qs_ctx("Server replies", "options"), KviOption_boolServerRepliesToActiveWindow);
	addBoolSelector(pGroup, __tr2qs_ctx("Server notices", "options"), KviOption_boolServerNoticesToActiveWindow);
	addBoolSelector(pGroup, __tr2qs_ctx("Broadcast and WALLOPS messages", "options"), KviOption_boolOperatorMessagesToActiveWindow);

	KviBoolSelector * b;

	addBoolSelector(0, 3, 1, 3, __tr2qs_ctx("Show extended server information", "options"), KviOption_boolShowExtendedServerInfo);
	addBoolSelector(0, 4, 1, 4, __tr2qs_ctx("Show server pings", "options"), KviOption_boolShowPingPong);
	
	b = addBoolSelector(0, 5, 1, 5, __tr2qs_ctx("Show own part messages", "options"), KviOption_boolShowOwnParts);
	mergeTip(b, __tr2qs_ctx("When enabled, the current user's part messages will be shown in the console.<br>"
	                        "When <b>Keep Channel Open</b> is enabled on part, this message will be shown in the dead channel instead.", "options"));

	addBoolSelector(0, 6, 1, 6, __tr2qs_ctx("Show compact mode changes", "options"), KviOption_boolShowCompactModeChanges);

	addRowSpacer(0, 7, 1, 7);
}

OptionsWidget_ircOutput::~OptionsWidget_ircOutput()
    = default;

void OptionsWidget_ircOutput::commit()
{
	KviOptionsWidget::commit();

	int iIdx = m_pVerbosityCombo->currentIndex();
	if((iIdx < 0) || (iIdx > KVI_VERBOSITY_LEVEL_PARANOIC))
	{
		iIdx = KVI_VERBOSITY_LEVEL_NORMAL;
	}
	KVI_OPTION_UINT(KviOption_uintOutputVerbosityLevel) = iIdx;

	KVI_OPTION_UINT(KviOption_uintOutputDatetimeFormat) = m_pDatetimeCombo->currentIndex();
}
