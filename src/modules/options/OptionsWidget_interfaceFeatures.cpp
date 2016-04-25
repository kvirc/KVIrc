//=============================================================================
//
//   File : OptionsWidget_interfaceFeatures.cpp
//   Creation date : Sat Jun 2 2001 14:52:53 CEST by Szymon Stefanek
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

#include "OptionsWidget_interfaceFeatures.h"

#include "KviOptions.h"
#include "KviLocale.h"
#include "KviApplication.h"
#include "KviFileUtils.h"

#include <QLayout>
#include <QLabel>

#include <string.h>


OptionsWidget_interfaceFeatures::OptionsWidget_interfaceFeatures(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("interfacefeatures_options_widget");
	createLayout();

	KviTalGroupBox * g;

	addBoolSelector(0,0,0,0,__tr2qs_ctx("Minimize application on startup","options"),KviOption_boolStartupMinimized);
	addBoolSelector(0,1,0,1,__tr2qs_ctx("Confirm quit with active connections","options"),KviOption_boolConfirmCloseWhenThereAreConnections);
	addBoolSelector(0,2,0,2,__tr2qs_ctx("Remember window properties","options"),KviOption_boolWindowsRememberProperties);
	addBoolSelector(0,3,0,3,__tr2qs_ctx("Enable visual effects","options"),KviOption_boolEnableVisualEffects);
	addBoolSelector(0,4,0,4,__tr2qs_ctx("Hide channel window tool buttons by default","options"),KviOption_boolHideWindowToolButtons);
#ifdef COMPILE_ON_MAC
	addBoolSelector(0,5,0,5,__tr2qs_ctx("Require ⌘ to be held down to copy text","options"),KviOption_boolRequireControlToCopy);
#else
	addBoolSelector(0,5,0,5,__tr2qs_ctx("Require Ctrl to be held down to copy text","options"),KviOption_boolRequireControlToCopy);
#endif
	g = addGroupBox(0,6,0,6,Qt::Horizontal,__tr2qs_ctx("Open Dialog Window for","options"));
	addBoolSelector(g,__tr2qs_ctx("Preferences","options"),KviOption_boolShowGeneralOptionsDialogAsToplevel);
	addBoolSelector(g,__tr2qs_ctx("Registered users","options"),KviOption_boolShowRegisteredUsersDialogAsToplevel);
	addBoolSelector(g,__tr2qs_ctx("Identity","options"),KviOption_boolShowIdentityDialogAsToplevel);
	addBoolSelector(g,__tr2qs_ctx("Servers","options"),KviOption_boolShowServersConnectDialogAsToplevel);
	addBoolSelector(g,__tr2qs_ctx("Join channels","options"),KviOption_boolShowChannelsJoinDialogAsToplevel);

	addRowSpacer(0,7,0,7);
}

OptionsWidget_interfaceFeatures::~OptionsWidget_interfaceFeatures()
{
}
