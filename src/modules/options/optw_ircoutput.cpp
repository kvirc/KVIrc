//
//   File : optw_ircoutput.cpp
//   Creation date : Wed Nov 15 2000 11:45:06 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)
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

#include "optw_ircoutput.h"

#include <qlayout.h>

#include "kvi_settings.h"
#include "kvi_options.h"
#include "kvi_locale.h"

#include <qcombobox.h>
#include <qlabel.h>

#ifdef COMPILE_INFO_TIPS
	#include "kvi_tal_tooltip.h"
#endif

KviIrcOutputOptionsWidget::KviIrcOutputOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent,"ircoutput_options_widget")
{
	createLayout(7,2);

	QLabel * l =new QLabel(__tr2qs_ctx("Output verbosity","options"),this);
	addWidgetToLayout(l,0,0,0,0);

	m_pVerbosityCombo = new QComboBox(false,this);
	addWidgetToLayout(m_pVerbosityCombo,1,0,1,0);
	
	m_pVerbosityCombo->insertItem(__tr2qs_ctx("Mute","options"));
	m_pVerbosityCombo->insertItem(__tr2qs_ctx("Quiet","options"));
	m_pVerbosityCombo->insertItem(__tr2qs_ctx("Normal","options"));
	m_pVerbosityCombo->insertItem(__tr2qs_ctx("Verbose","options"));
	m_pVerbosityCombo->insertItem(__tr2qs_ctx("Paranoic","options"));

	if(KVI_OPTION_UINT(KviOption_uintOutputVerbosityLevel) > KVI_VERBOSITY_LEVEL_PARANOIC)
		KVI_OPTION_UINT(KviOption_uintOutputVerbosityLevel) = KVI_VERBOSITY_LEVEL_NORMAL;
	m_pVerbosityCombo->setCurrentItem(KVI_OPTION_UINT(KviOption_uintOutputVerbosityLevel));

	KviTalGroupBox * g = addGroupBox(0,1,1,1,1,Qt::Horizontal,__tr2qs_ctx("Show in active window","options"));
	addBoolSelector(g,__tr2qs_ctx("External messages","options"),KviOption_boolExternalMessagesToActiveWindow);
	addBoolSelector(g,__tr2qs_ctx("External CTCP replies","options"),KviOption_boolCtcpRepliesToActiveWindow);
	addBoolSelector(g,__tr2qs_ctx("Whois replies","options"),KviOption_boolWhoisRepliesToActiveWindow);
	addBoolSelector(g,__tr2qs_ctx("ChanServ and NickServ notices","options"),KviOption_boolServicesNoticesToActiveWindow);
	addBoolSelector(g,__tr2qs_ctx("Invite messages","options"),KviOption_boolInvitesToActiveWindow);
	addBoolSelector(g,__tr2qs_ctx("Server replies","options"),KviOption_boolServerRepliesToActiveWindow);
	addBoolSelector(g,__tr2qs_ctx("Server notices","options"),KviOption_boolServerNoticesToActiveWindow);
	addBoolSelector(g,__tr2qs_ctx("Broadcast and WALLOPS messages","options"),KviOption_boolOperatorMessagesToActiveWindow);
//	if(KVI_OPTION_UINT(KviOption_uintUserExperienceLevel)>=2)
//	{
		addBoolSelector(0,2,1,2,__tr2qs_ctx("Show extended server information","options"),KviOption_boolShowExtendedServerInfo);
		addBoolSelector(0,3,1,3,__tr2qs_ctx("Show server pings","options"),KviOption_boolShowPingPong);
		addBoolSelector(0,4,1,4,__tr2qs_ctx("Show own parts in the console","options"),KviOption_boolShowOwnParts);
		addBoolSelector(0,5,1,5,__tr2qs_ctx("Show compact mode changes","options"),KviOption_boolShowCompactModeChanges);

		addRowSpacer(0,6,1,6);
//	} else {
//		addRowSpacer(0,2,1,2);
//	}
}

KviIrcOutputOptionsWidget::~KviIrcOutputOptionsWidget()
{
}

void KviIrcOutputOptionsWidget::commit()
{
	KviOptionsWidget::commit();
	
	int i = m_pVerbosityCombo->currentItem();
	if((i < 0) || (i > KVI_VERBOSITY_LEVEL_PARANOIC))i = KVI_VERBOSITY_LEVEL_NORMAL;
	KVI_OPTION_UINT(KviOption_uintOutputVerbosityLevel) = i;
}

#include "m_optw_ircoutput.moc"
