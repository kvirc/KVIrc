//
//   File : optw_tray.cpp
//   Creation date : Mon 14 Aug 2006 by Alexey Uzhva
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


#include "optw_tray.h"

#include <qlayout.h>
#include <qtooltip.h>

#include "kvi_options.h"
#include "kvi_locale.h"

//#warning "Info tips"


KviTrayOptionsWidget::KviTrayOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent,"tray_options_widget")
{
	createLayout(7,1);
	
	m_pEnable = addBoolSelector(0,0,0,0, __tr2qs_ctx("Enable tray icon","options"), KviOption_boolShowDockExtension);
	m_pCloseInTray = addBoolSelector(0,1,0,1,__tr2qs_ctx("Close in tray","options"),KviOption_boolCloseInTray);
	m_pMinimizeInTray =  addBoolSelector(0,2,0,2,__tr2qs_ctx("Minimize in tray","options"),KviOption_boolMinimizeInTray);

	m_pEnableFlashing = addBoolSelector(0,3,0,3,__tr2qs_ctx("Enable tray icon flashing","options"),KviOption_boolEnableTrayIconFlashing);
	m_pLevelBasedNotify = addBoolSelector(0,4,0,4,__tr2qs_ctx("Enable level-based message notification (not a message-type based)","options"),KviOption_boolUseLevelBasedTrayNotification);
	
	m_pLevelsGroup = addGroupBox(0,5,0,5,1,Qt::Horizontal,__tr2qs_ctx("Select message levels to notify","options"));
	
	addUIntSelector(m_pLevelsGroup,__tr2qs_ctx("Minimum level for low-priority messages","options"),
				KviOption_uintMinTrayLowLevelMessage,
				1,5,1,true);
		
	addUIntSelector(m_pLevelsGroup,__tr2qs_ctx("Minimum level for high-priority message","options"),
				KviOption_uintMinTrayHighLevelMessage,
				1,5,4,true);
	
	addRowSpacer(0,6,0,6);
	
	connect(m_pEnable,SIGNAL(toggled(bool)),this,SLOT(setEnabled(bool)));
	connect(m_pLevelBasedNotify,SIGNAL(toggled(bool)),this,SLOT(setEnabled(bool)));
	setEnabled(true);
}

KviTrayOptionsWidget::~KviTrayOptionsWidget()
{
}

void KviTrayOptionsWidget::setEnabled(bool bEnabled)
{
	if(m_pEnable->isChecked())
	{
		m_pEnableFlashing->setEnabled(true);
		m_pLevelBasedNotify->setEnabled(true);
		m_pCloseInTray->setEnabled(true);
		m_pMinimizeInTray->setEnabled(true);
		if(m_pLevelBasedNotify->isChecked())
			m_pLevelsGroup->setEnabled(true);
		else
			m_pLevelsGroup->setEnabled(false);
		
	} else {
		m_pCloseInTray->setEnabled(false);
		m_pMinimizeInTray->setEnabled(false);
		m_pEnableFlashing->setEnabled(false);
		m_pLevelBasedNotify->setEnabled(false);
		m_pLevelsGroup->setEnabled(false);
	}
}


#include "m_optw_tray.moc"
