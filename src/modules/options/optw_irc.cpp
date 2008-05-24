//
//   File : optw_irc.cpp
//   Creation date : Sun Dec  2 18:59:48 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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


#include "optw_irc.h"

#include "kvi_settings.h"
#include "kvi_locale.h"
#include "kvi_options.h"
#include "kvi_ircmask.h"

#include <QComboBox>


KviIrcOptionsWidget::KviIrcOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent,"irc_options_widget")
{
}


KviIrcOptionsWidget::~KviIrcOptionsWidget()
{
}


KviIrcGeneralOptionsWidget::KviIrcGeneralOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent,"irc_options_widget")
{
	createLayout();
	KviBoolSelector *b = addBoolSelector(0,0,0,0,__tr2qs_ctx("Minimize console after successful login","options"),KviOption_boolMinimizeConsoleAfterConnect);

	mergeTip(b,__tr2qs_ctx("<center>This option will cause KVIrc to minimize the console window after successfully logging into a server.</center>","options"));

	b = addBoolSelector(0,1,0,1,__tr2qs_ctx("Show network name in console taskbar entry","options"),KviOption_boolShowNetworkNameForConsoleTaskBarEntry);
	mergeTip(b,__tr2qs_ctx("<center>This option will cause KVIrc to show the network name as the console taskbar entry instead of the server name. This is nice to keep on unless your servers are not organized in networks or you often connect to two servers of the same network.</center>","options"));

	addRowSpacer(0,2,0,2);
}


KviIrcGeneralOptionsWidget::~KviIrcGeneralOptionsWidget()
{
}


KviIrcAdvancedOptionsWidget::KviIrcAdvancedOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent,"irc_advanced_options_widget")
{
	createLayout();
	KviBoolSelector *b = addBoolSelector(0,0,0,0,__tr2qs_ctx("Force immediate quit","options"),KviOption_boolForceBrutalQuit);    
	mergeTip(b,
		__tr2qs_ctx("<center>This option causes KVIrc to close " \
		"the connection immediately after sending the QUIT message.<br>" \
		"When this option is disabled, KVIrc will wait for the server " \
		"to close the connection.<br>" \
		"Note that if you use this, your QUIT message may be not displayed.</center>","options"));
	addBoolSelector(0,1,0,1,__tr2qs_ctx("Prepend gender info to realname","options"),KviOption_boolPrependGenderInfoToRealname);
	addRowSpacer(0,2,0,2);
}

KviIrcAdvancedOptionsWidget::~KviIrcAdvancedOptionsWidget()
{
}


KviChannelOptionsWidget::KviChannelOptionsWidget(QWidget * pParent)
: KviOptionsWidget(pParent)
{
}

KviChannelOptionsWidget::~KviChannelOptionsWidget()
{
}


KviChannelGeneralOptionsWidget::KviChannelGeneralOptionsWidget(QWidget * pParent)
: KviOptionsWidget(pParent)
{
	createLayout();

	KviTalGroupBox * g = addGroupBox(0,0,0,0,Qt::Horizontal,__tr2qs_ctx("On Channel Kick","options"));

 	KviBoolSelector *b = addBoolSelector(g,__tr2qs_ctx("Rejoin channel","options"),KviOption_boolRejoinChannelOnKick);
	mergeTip(b,
		__tr2qs_ctx("<center>This option will cause KVIrc to attempt " \
		"to rejoin a channel after being kicked.</center>","options"));
	b = addBoolSelector(g,__tr2qs_ctx("Keep channel open","options"),KviOption_boolKeepChannelOpenOnKick);
	mergeTip(b,
		__tr2qs_ctx("<center>This option will cause KVIrc to keep " \
			"the channel window open after being kicked.<br>" \
			"It might be a good idea to also enable the " \
			"\"Rejoin channel\" option.</center>","options"));

	g = addGroupBox(0,1,0,1,Qt::Horizontal,__tr2qs_ctx("On Channel Part","options"));

	b = addBoolSelector(g,__tr2qs_ctx("Keep channel open","options"),KviOption_boolKeepChannelOpenOnPart);
	mergeTip(b,
		__tr2qs_ctx("<center>This option will cause KVIrc to keep " \
			"the channel window open after leaving it.</center>","options"));

	b = addBoolSelector(0,2,0,2,__tr2qs_ctx("Automatically join channel on invite","options"),KviOption_boolAutoJoinOnInvite);
	mergeTip(b,
		__tr2qs_ctx("<center>This option will cause KVIrc to automatically " \
			"join a channel when an INVITE message for that channel is received.<br>" \
			"<b>Warning:</b> This may help spammers harass you. :)</center>","options"));

	b = addBoolSelector(0,3,0,3, __tr2qs_ctx("Always open channels as minimized","options"),KviOption_boolCreateMinimizedChannels);
	mergeTip(b,
		__tr2qs_ctx("<center>This option causes newly created " \
			"channel windows to be immediately minimized.<br>" \
			"Enable this if you don't like channels " \
			"popping up while you're typing something in a channel. :D</center>","options"));

	addRowSpacer(0,4,0,4);
}

KviChannelGeneralOptionsWidget::~KviChannelGeneralOptionsWidget()
{
}


KviChannelAdvancedOptionsWidget::KviChannelAdvancedOptionsWidget(QWidget * pParent)
: KviOptionsWidget(pParent)
{
	createLayout();

	KviBoolSelector * b = addBoolSelector(0,0,4,0,__tr2qs_ctx("Log joined channels history","options"),KviOption_boolLogChannelHistory);
	
	addLabel(0,1,0,1,__tr2qs_ctx("Default ban mask:","options"));
	
	m_pBanTypeCombo=new QComboBox(this);
	addWidgetToLayout(m_pBanTypeCombo,1,1,4,1);
	
	KviIrcMask hostmask("nick!user@machine.host.top");
	KviIrcMask ipmask("nick!user@192.168.1.2");
	QString tmp1;
	QString tmp2;
	for(int i=0;i<=26;i++)
	{
		
		hostmask.mask(tmp1,(KviIrcMask::MaskType)i);
		ipmask.mask(tmp2,(KviIrcMask::MaskType)i);
		m_pBanTypeCombo->insertItem(m_pBanTypeCombo->count(),QString("%1 (%2)").arg(tmp1).arg(tmp2));
	}
	m_pBanTypeCombo->setCurrentIndex(KVI_OPTION_UINT(KviOption_uintDefaultBanType));

	KviTalGroupBox * g = addGroupBox(0,2,4,2,Qt::Horizontal,__tr2qs_ctx("On Channel Join","options"));
	addBoolSelector(g,__tr2qs_ctx("Do not send /WHO request","options"),KviOption_boolDisableWhoRequestOnJoin);
  	addBoolSelector(g,__tr2qs_ctx("Do not request ban list","options"),KviOption_boolDisableBanListRequestOnJoin);
  	addBoolSelector(g,__tr2qs_ctx("Do not request ban exception list","options"),KviOption_boolDisableBanExceptionListRequestOnJoin);
 	addBoolSelector(g,__tr2qs_ctx("Do not request invite list","options"),KviOption_boolDisableInviteListRequestOnJoin);

 	b = addBoolSelector(0,3,4,3,__tr2qs_ctx("Do not update the away list","options"),KviOption_boolDisableAwayListUpdates);
	mergeTip(b,
		__tr2qs_ctx("<center>KVIrc sends out a channel /WHO message every now and then to keep " \
			"the channel away list in sync. Use this option to disable this feature (and to save " \
			"your IRC bandwidth.</center>","options"));

	g = addGroupBox(0,4,4,4,Qt::Horizontal,__tr2qs_ctx("On Channel Join","options"));
	addBoolSelector(g,__tr2qs_ctx("Echo channel topic","options"),KviOption_boolEchoNumericTopic);
	addBoolSelector(g,__tr2qs_ctx("Show channel sync time","options"),KviOption_boolShowChannelSyncTime);

	addRowSpacer(0,5,4,5);
}

KviChannelAdvancedOptionsWidget::~KviChannelAdvancedOptionsWidget()
{
}

void KviChannelAdvancedOptionsWidget::commit()
{
	KVI_OPTION_UINT(KviOption_uintDefaultBanType)=m_pBanTypeCombo->currentIndex();
	if((KVI_OPTION_UINT(KviOption_uintDefaultBanType)<0 )|| (KVI_OPTION_UINT(KviOption_uintDefaultBanType)>26)) KVI_OPTION_UINT(KviOption_uintDefaultBanType)=7;
	KviOptionsWidget::commit();
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_optw_irc.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
