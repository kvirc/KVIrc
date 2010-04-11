//=============================================================================
//
//   File : optw_irc.cpp
//   Creation date : Sun Dec  2 18:59:48 2001 GMT by Szymon Stefanek
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

#include "optw_irc.h"

#include "kvi_settings.h"
#include "kvi_locale.h"
#include "kvi_options.h"
#include "kvi_ircmask.h"


KviIrcOptionsWidget::KviIrcOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("irc_options_widget");
}

KviIrcOptionsWidget::~KviIrcOptionsWidget()
{
}


KviIrcGeneralOptionsWidget::KviIrcGeneralOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("irc_options_widget");
	createLayout();
	KviBoolSelector *b = addBoolSelector(0,0,0,0,__tr2qs_ctx("Minimize console after successful login","options"),KviOption_boolMinimizeConsoleAfterConnect);

	mergeTip(b,__tr2qs_ctx("<center>This option will cause KVIrc to minimize the console window after successfully logging into a server.</center>","options"));

	b = addBoolSelector(0,1,0,1,__tr2qs_ctx("Show network name in console window list entry","options"),KviOption_boolShowNetworkNameForConsoleWindowListEntry);
	mergeTip(b,__tr2qs_ctx("<center>This option will cause KVIrc to show the network name as the console window list entry instead of the server name. This is nice to keep on unless your servers are not organized in networks or you often connect to two servers of the same network.</center>","options"));

	addRowSpacer(0,2,0,2);
}

KviIrcGeneralOptionsWidget::~KviIrcGeneralOptionsWidget()
{
}


KviIrcAdvancedOptionsWidget::KviIrcAdvancedOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("irc_advanced_options_widget");
	createLayout();
	KviBoolSelector *b = addBoolSelector(0,0,0,0,__tr2qs_ctx("Force immediate quit","options"),KviOption_boolForceBrutalQuit);
	mergeTip(b,
		__tr2qs_ctx("<center>This option causes KVIrc to close " \
		"the connection immediately after sending the QUIT message.<br>" \
		"When this option is disabled, KVIrc will wait for the server " \
		"to close the connection.<br>" \
		"Note that if you use this, your QUIT message may be not displayed.</center>","options"));
	addBoolSelector(0,1,0,1,__tr2qs_ctx("Prepend gender info to realname","options"),KviOption_boolPrependGenderInfoToRealname);
	addBoolSelector(0,2,0,2,__tr2qs_ctx("Prepend avatar info to realname","options"),KviOption_boolPrependAvatarInfoToRealname);
	addBoolSelector(0,3,0,3,__tr2qs_ctx("Prepend smart nick color info to realname","options"),KviOption_boolPrependNickColorInfoToRealname);
	addRowSpacer(0,4,0,4);
}

KviIrcAdvancedOptionsWidget::~KviIrcAdvancedOptionsWidget()
{
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_optw_irc.moc"
#endif //COMPILE_USE_STANDALONE_MOC_SOURCES
