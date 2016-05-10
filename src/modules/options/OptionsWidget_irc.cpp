//=============================================================================
//
//   File : OptionsWidget_irc.cpp
//   Creation date : Sun Dec  2 18:59:48 2001 GMT by Szymon Stefanek
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

#include "OptionsWidget_irc.h"

#include "kvi_settings.h"
#include "KviLocale.h"
#include "KviOptions.h"
#include "KviIrcMask.h"

OptionsWidget_irc::OptionsWidget_irc(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("irc_options_widget");
}

OptionsWidget_irc::~OptionsWidget_irc()
    = default;

OptionsWidget_ircGeneral::OptionsWidget_ircGeneral(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("irc_options_widget");
	createLayout();

	KviBoolSelector * b = addBoolSelector(0, 1, 0, 1, __tr2qs_ctx("Show network name in console window list entry", "options"), KviOption_boolShowNetworkNameForConsoleWindowListEntry);
	mergeTip(b, __tr2qs_ctx("This option will cause KVIrc to show the network name as the "
	                        "console window list entry instead of the server name. "
	                        "This is nice to keep on unless your servers are not organized "
	                        "in networks or you often connect to two servers of the same network.", "options"));

	addRowSpacer(0, 2, 0, 2);
}

OptionsWidget_ircGeneral::~OptionsWidget_ircGeneral()
    = default;

OptionsWidget_ircAdvanced::OptionsWidget_ircAdvanced(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("irc_advanced_options_widget");
	createLayout();

	KviBoolSelector * b = addBoolSelector(0, 0, 0, 0, __tr2qs_ctx("Force immediate quit", "options"), KviOption_boolForceBrutalQuit);
	mergeTip(b, __tr2qs_ctx("This option causes KVIrc to close "
	                        "the connection immediately after sending the QUIT message. "
	                        "When this option is disabled, KVIrc will wait for the server "
	                        "to close the connection.<br>"
	                        "Note that if you use this, your QUIT message may be not displayed.","options"));

	addBoolSelector(0, 1, 0, 1, __tr2qs_ctx("Prepend gender info to real name", "options"), KviOption_boolPrependGenderInfoToRealname);
	addBoolSelector(0, 2, 0, 2, __tr2qs_ctx("Prepend avatar info to real name", "options"), KviOption_boolPrependAvatarInfoToRealname);
	b = addBoolSelector(0, 3, 0, 3, __tr2qs_ctx("Prepend smart nick color info to real name", "options"), KviOption_boolPrependNickColorInfoToRealname);
	mergeTip(b, __tr2qs_ctx("This feature will not work if you have chosen your "
	                        "nick background color to be transparent.", "options"));

	addRowSpacer(0, 4, 0, 4);
}

OptionsWidget_ircAdvanced::~OptionsWidget_ircAdvanced()
    = default;
