//=============================================================================
//
//   File : optw_interfacefeatures.cpp
//   Creation date : Sat Jun 2 2001 14:52:53 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2005 Szymon Stefanek (pragma at kvirc dot net)
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
//=============================================================================

#include "optw_interfacefeatures.h"

#include <qlayout.h>
#include <qlabel.h>
#include <string.h>

#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_app.h"
#include "kvi_fileutils.h"

KviInterfaceFeaturesOptionsWidget::KviInterfaceFeaturesOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent,"interfacefeatures_options_widget")
{
	createLayout(8,1);



	addBoolSelector(0,0,0,0,__tr2qs_ctx("Minimize on startup","options"),KviOption_boolStartupMinimized);
	addBoolSelector(0,1,0,1,__tr2qs_ctx("Confirm quit with active connections","options"),KviOption_boolConfirmCloseWhenThereAreConnections);
	addBoolSelector(0,2,0,2,__tr2qs_ctx("Remember window properties","options"),KviOption_boolWindowsRememberProperties);

	QString szSplashDisableFile;
	g_pApp->getLocalKvircDirectory(szSplashDisableFile,KviApp::Pics,"disable-splash." KVI_VERSION);
	bool bDisableSplash = KviFileUtils::fileExists(szSplashDisableFile);
	m_pDisableSplash = new KviStyledCheckBox(__tr2qs_ctx("Disable splash screen","options"),this);
	addWidgetToLayout(m_pDisableSplash,0,3,0,3);
	m_pDisableSplash->setChecked(bDisableSplash);
	
	addBoolSelector(0,4,0,4,__tr2qs_ctx("Enable visual effects","options"),KviOption_boolEnableVisualEffects);
	addBoolSelector(0,5,0,5,__tr2qs_ctx("Hide Channel window tool buttons by default","options"),KviOption_boolHideWindowToolButtons);
	
	KviTalGroupBox * g = addGroupBox(0,6,0,6,1,KviTalGroupBox::Horizontal,__tr2qs_ctx("Open Dialog Window For","options"));
	addBoolSelector(g,__tr2qs_ctx("Preferences","options"),KviOption_boolShowGeneralOptionsDialogAsToplevel);
	addBoolSelector(g,__tr2qs_ctx("Registered Users","options"),KviOption_boolShowRegisteredUsersDialogAsToplevel);
	addBoolSelector(g,__tr2qs_ctx("Identity","options"),KviOption_boolShowIdentityDialogAsToplevel);
	addBoolSelector(g,__tr2qs_ctx("Servers","options"),KviOption_boolShowServersConnectDialogAsToplevel);
	addBoolSelector(g,__tr2qs_ctx("Join Channels","options"),KviOption_boolShowChannelsJoinDialogAsToplevel);
	addRowSpacer(0,7,0,7);
}

KviInterfaceFeaturesOptionsWidget::~KviInterfaceFeaturesOptionsWidget()
{
}

void KviInterfaceFeaturesOptionsWidget::commit()
{
	KviOptionsWidget::commit();

	QString szSplashDisableFile;
	g_pApp->getLocalKvircDirectory(szSplashDisableFile,KviApp::Pics,"disable-splash." KVI_VERSION);

	if(m_pDisableSplash->isChecked())
	{
		if(!KviFileUtils::fileExists(szSplashDisableFile))
			KviFileUtils::writeFile(szSplashDisableFile,"");
	} else {
		if(KviFileUtils::fileExists(szSplashDisableFile))
			KviFileUtils::removeFile(szSplashDisableFile);
	}
	
}

#include "m_optw_interfacefeatures.moc"
