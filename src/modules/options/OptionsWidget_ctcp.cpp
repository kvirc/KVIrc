//=============================================================================
//
//   File : OptionsWidget_ctcp.cpp
//   Creation date : Sun Dec  2 20:40:43 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "OptionsWidget_ctcp.h"

#include "kvi_settings.h"
#include "KviLocale.h"
#include "KviOptions.h"


OptionsWidget_ctcp::OptionsWidget_ctcp(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("ctcp_options_widget");

	createLayout();

	addLabel(0,0,0,0,__tr2qs_ctx("<center><b>Settings for CTCP.</b></center>"\
		"<p>The <b>Client-To-Client Protocol</b> (CTCP) " \
		"is used to transmit special control messages over an IRC connection. " \
		"These messages can request information from clients or negotiate file transfers." \
		"<br><br></p>","options"));


	KviTalGroupBox *gs = addGroupBox(0,1,0,1,Qt::Horizontal,__tr2qs_ctx("CTCP Replies","options"));
	KviStringSelector * s = addStringSelector(gs, __tr2qs_ctx("Append to VERSION reply:","options"),KviOption_stringCtcpVersionPostfix);
	s->setMinimumLabelWidth(200);

	mergeTip(s,
		__tr2qs_ctx("<center>This text will be appended to the CTCP VERSION reply.<br>For example, you can place a script name here.</center>","options"));

	s = addStringSelector(gs, __tr2qs_ctx("Append to SOURCE reply:","options"),KviOption_stringCtcpSourcePostfix);
	s->setMinimumLabelWidth(200);

	mergeTip(s,
		__tr2qs_ctx("<center>This text will be appended to the CTCP SOURCE reply.<br>For example, you can place the source URL for a script here.</center>","options"));

//	s = addStringSelector(gs, __tr2qs_ctx("USERINFO reply:","options"),KviOption_stringCtcpUserinfoReply);

//	mergeTip(s,
//		__tr2qs_ctx("<center>This is the CTCP USERINFO reply.<br>It can contain some information about yourself.</center>","options"));

	s = addStringSelector(gs, __tr2qs_ctx("PAGE reply:","options"),KviOption_stringCtcpPageReply);
	s->setMinimumLabelWidth(200);

	mergeTip(s,
		__tr2qs_ctx("<center>This is the CTCP PAGE reply.<br>It should contain some sort of acknowledgement for CTCP PAGE messages.</center>","options"));

	addBoolSelector(gs,__tr2qs_ctx("Show the CTCP replies in the active window","options"),KviOption_boolCtcpRepliesToActiveWindow);

	addBoolSelector(gs,__tr2qs_ctx("Show dialog for CTCP page requests","options"),KviOption_boolShowDialogOnCtcpPage);
	addBoolSelector(gs,__tr2qs_ctx("Show dialog for Channel CTCP page requests","options"),KviOption_boolShowDialogOnChannelCtcpPage);

	addRowSpacer(0,3,0,3);

}


OptionsWidget_ctcp::~OptionsWidget_ctcp()
{
}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_OptionsWidget_ctcp.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
