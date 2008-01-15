//=============================================================================
//
//   File : kvi_channel.cpp
//   Creation date : Tue Aug 1 2000 02:20:22 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2004 Szymon Stefanek (pragma at kvirc dot net)
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

//
// Channel widget : abstraction of an IRC channel
//

#define __KVIRC__

#include "kvi_toolwindows_container.h"
#include "kvi_styled_controls.h"
#include "kvi_channel.h"
#include "kvi_console.h"
#include "kvi_iconmanager.h"
#include "kvi_ircview.h"
#include "kvi_input.h"
#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_topicw.h"
#include "kvi_ircsocket.h"
#include "kvi_out.h"
#include "kvi_malloc.h"
#include "kvi_taskbar.h"
#include "kvi_frame.h"
#include "kvi_config.h"
#include "kvi_themedlabel.h"
#include "kvi_maskeditor.h"
#include "kvi_mirccntrl.h"
#include "kvi_settings.h"
#include "kvi_parameterlist.h"
#include "kvi_modeeditor.h"
#include "kvi_app.h"
#include "kvi_useraction.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnectionserverinfo.h"
#include "kvi_defaults.h"
#include "kvi_sparser.h"
#include "kvi_modew.h"
#include "kvi_mirccntrl.h"

#ifdef COMPILE_CRYPT_SUPPORT
	#include "kvi_crypt.h"
	#include "kvi_cryptcontroller.h"
#endif

#include "kvi_kvs_script.h"
#include "kvi_kvs_eventtriggers.h"

#include <time.h>

#include <qsplitter.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <qevent.h>

#include <qpalette.h>
#include "kvi_tal_popupmenu.h"
#include "kvi_pointerhashtable.h"
#include <qmessagebox.h>
#include "kvi_tal_widgetstack.h" 

#ifndef AVERAGE_CHANNEL_USERS
	#define AVERAGE_CHANNEL_USERS 101
#endif



// FIXME: #warning "+a Anonymous channel mode!"
// FIXME: #warning "+r channel mode (reop)"
// FIXME: #warning "OnChannelFlood event...."


KviChannel::KviChannel(KviFrame * lpFrm,KviConsole * lpConsole,const char * name)
: KviWindow(KVI_WINDOW_TYPE_CHANNEL,lpFrm,name,lpConsole)
{
	// Init some member variables
	m_pInput               = 0;
	m_iStateFlags          = 0;
	m_pBanList             = new KviPointerList<KviMaskEntry>;
	m_pBanList->setAutoDelete(true);
	m_pBanExceptionList    = new KviPointerList<KviMaskEntry>;
	m_pBanExceptionList->setAutoDelete(true);
	m_pInviteList = new KviPointerList<KviMaskEntry>;
	m_pInviteList->setAutoDelete(true);
	m_pActionHistory = new KviPointerList<KviChannelAction>;
	m_pActionHistory->setAutoDelete(true);
	m_uActionHistoryHotActionCount = 0;

	m_pTmpHighLighted      = new KviPointerHashTable<const char *,QString>();
	m_pTmpHighLighted->setAutoDelete(true);

	// Register ourselves
	connection()->registerChannel(this);
	// And create the widgets layout
	// Button box
	m_pButtonBox = new KviTalHBox(this);
	
	m_pTopSplitter = new QSplitter(Qt::Horizontal,m_pButtonBox);

	m_pButtonBox->setStretchFactor(m_pTopSplitter,1);
	
	m_pButtonContainer = new KviTalHBox(m_pButtonBox);
	
	// Topic widget on the left
	m_pTopicWidget = new KviTopicWidget(m_pTopSplitter,"topic_widget");

	connect(m_pTopicWidget,SIGNAL(topicSelected(const QString &)),
		this,SLOT(topicSelected(const QString &)));
	// mode label follows the topic widget
	m_pModeWidget = new KviModeWidget(m_pTopSplitter,this,"mode_");
	KviTalToolTip::add(m_pModeWidget,__tr2qs("Channel mode"));

	createTextEncodingButton(m_pButtonContainer);

	// Central splitter
	m_pSplitter = new QSplitter(Qt::Horizontal,this);
	#ifdef COMPILE_USE_QT4
		m_pSplitter->setObjectName(name);
	#else
		m_pSplitter->setName(name);
	#endif
	m_pSplitter->setOpaqueResize(false);
	// Spitted vertially on the left
	m_pVertSplitter = new QSplitter(Qt::Vertical,m_pSplitter);
	m_pVertSplitter->setOpaqueResize(false);
	// With the IRC view over
	m_pIrcView = new KviIrcView(m_pVertSplitter,lpFrm,this);
	#ifdef COMPILE_USE_QT4
		m_pIrcView->setObjectName(name);
	#else
		m_pIrcView->setName(name);
	#endif
	connect(m_pIrcView,SIGNAL(rightClicked()),this,SLOT(textViewRightClicked()));
	// And the double view (that may be unused)
	m_pMessageView = 0;
	// The userlist on the right
	//m_pEditorsContainer= new KviToolWindowsContainer(m_pSplitter);
	
	
	// and the related buttons
	m_pDoubleViewButton = createToolButton(m_pButtonContainer,"double_view_button",KVI_SMALLICON_HIDEDOUBLEVIEW,KVI_SMALLICON_SHOWDOUBLEVIEW,__tr2qs("Split View"),false);
	connect(m_pDoubleViewButton,SIGNAL(clicked()),this,SLOT(toggleDoubleView()));
	
	m_pListViewButton = new KviWindowToolPageButton(KVI_SMALLICON_HIDELISTVIEW,KVI_SMALLICON_SHOWLISTVIEW,__tr2qs("User List"),buttonContainer(),true,"list_view_button");
	connect(m_pListViewButton,SIGNAL(clicked()),this,SLOT(toggleListView()));
	m_pBanEditorButton = new KviWindowToolPageButton(KVI_SMALLICON_UNBAN,KVI_SMALLICON_BAN,__tr2qs("Ban Editor"),buttonContainer(),false,"ban_editor_button");
	connect(m_pBanEditorButton,SIGNAL(clicked()),this,SLOT(toggleBanEditor()));
	
	if(m_pConsole->connection()->serverInfo()->supportedListModes().contains('e'))
	{
		m_pBanExceptionEditorButton =new KviWindowToolPageButton(KVI_SMALLICON_BANUNEXCEPT,KVI_SMALLICON_BANEXCEPT,__tr2qs("Ban Exception Editor"),buttonContainer(),false,"ban_exception_editor_button");
		connect(m_pBanExceptionEditorButton,SIGNAL(clicked()),this,SLOT(toggleBanExceptionEditor()));
	} else {
		m_pBanExceptionEditorButton=0;
	}
	if(m_pConsole->connection()->serverInfo()->supportedListModes().contains('I'))
	{
		m_pInviteEditorButton =new KviWindowToolPageButton(KVI_SMALLICON_INVITEUNEXCEPT,KVI_SMALLICON_INVITEEXCEPT,__tr2qs("Invite Exception Editor"),buttonContainer(),false,"invite_exception_editor_button");
		connect(m_pInviteEditorButton,SIGNAL(clicked()),this,SLOT(toggleInviteEditor()));
	} else {
		m_pInviteEditorButton = 0;
	}
	m_pModeEditorButton = new KviWindowToolPageButton(KVI_SMALLICON_CHANMODEHIDE,KVI_SMALLICON_CHANMODE,__tr2qs("Mode Editor"),buttonContainer(),false,"mode_editor_button");
	connect(m_pModeEditorButton,SIGNAL(clicked()),this,SLOT(toggleModeEditor()));
	m_pModeEditor = 0;
	
#ifdef COMPILE_CRYPT_SUPPORT
	createCryptControllerButton(m_pButtonContainer);
#endif

	m_pHideToolsButton = new KviStyledToolButton(m_pButtonBox,"hide_container_button");

#ifndef  COMPILE_USE_QT4
	m_pHideToolsButton->setUsesBigPixmap(false);
#else
	m_pHideToolsButton->setAutoRaise(true);
#endif
	m_pHideToolsButton->setFixedWidth(10);

	if(g_pIconManager->getBigIcon("kvi_horizontal_left.png"))
		m_pHideToolsButton->setPixmap(*(g_pIconManager->getBigIcon("kvi_horizontal_left.png")));
	
	connect(m_pHideToolsButton,SIGNAL(clicked()),this,SLOT(toggleToolButtons()));
	
	m_pUserListView = new KviUserListView(m_pSplitter,m_pListViewButton,connection()->userDataBase(),this,
								AVERAGE_CHANNEL_USERS,__tr2qs("User List"),"user_list_view");
//	m_pEditorsContainer->addWidget(m_pUserListView);
//	m_pEditorsContainer->raiseWidget(m_pUserListView);
	// And finally the input line on the bottom
	m_pInput   = new KviInput(this,m_pUserListView);
	// no mask editors yet
	m_pBanEditor = 0;
	m_pBanExceptionEditor = 0;
	m_pInviteEditor = 0;
	// Ensure proper focusing
	//setFocusHandler(m_pInput,this);
	// And turn on the secondary IRC view if needed

	if(KVI_OPTION_BOOL(KviOption_boolAutoLogChannels))m_pIrcView->startLogging();

	applyOptions();
	m_joinTime = QDateTime::currentDateTime();
	m_tLastReceivedWhoReply = (kvi_time_t)m_joinTime.toTime_t();
	

}

KviChannel::~KviChannel()
{
	// Unregister ourself
	if(type() == KVI_WINDOW_TYPE_DEADCHANNEL)context()->unregisterDeadChannel(this);
	else connection()->unregisterChannel(this);
	// Then remove all the users and free mem
	m_pUserListView->enableUpdates(false);
	m_pUserListView->partAll();
	delete m_pActionHistory;
	delete m_pBanList;
	delete m_pBanExceptionList;
	delete m_pInviteList;
	delete m_pTmpHighLighted;
}

void KviChannel::toggleToolButtons()
{
	if(!buttonContainer()) return;
	toggleButtonContainer();
	QPixmap* pix= buttonContainer()->isVisible() ? 
		g_pIconManager->getBigIcon("kvi_horizontal_left.png") : 
		g_pIconManager->getBigIcon("kvi_horizontal_right.png");
	if(pix)
		m_pHideToolsButton->setPixmap(*pix);
}

void KviChannel::triggerCreationEvents()
{
	KVS_TRIGGER_EVENT_0(KviEvent_OnChannelWindowCreated,this);
}

void KviChannel::textViewRightClicked()
{
	KVS_TRIGGER_EVENT_0(KviEvent_OnChannelPopupRequest,this);
}

void KviChannel::getBaseLogFileName(QString &buffer)
{
	QString szChan(windowName());
	szChan.replace(".","%2e");
	if (console()->connection())
	{
		buffer=szChan;
		buffer.append(".");
		buffer.append(console()->currentNetworkName());
	} else {
		buffer=szChan;
		buffer.append(".");
		buffer.append(console()->ircContextId());
	}
}

void KviChannel::applyOptions()
{
	m_pUserListView->applyOptions();
	m_pTopicWidget->applyOptions();

	if(m_pMessageView)m_pMessageView->applyOptions();

	m_pModeWidget->applyOptions();

	// this applies options for IrcView and Input and forces the window to relayout
	KviWindow::applyOptions();
}

void KviChannel::getConfigGroupName(QString &buf)
{
	buf = windowName();
}

void KviChannel::saveProperties(KviConfig *cfg)
{
	KviWindow::saveProperties(cfg);
	cfg->writeEntry("TopSplitter",m_pTopSplitter->sizes());
	cfg->writeEntry("Splitter",m_pSplitter->sizes());
#ifdef COMPILE_USE_QT4
	QList<int> tmp = m_pVertSplitter->sizes();
	KviValueList<int> tmp2;
	for(QList<int>::Iterator it = tmp.begin();it != tmp.end();++it)
		tmp2.append(*it);
	cfg->writeEntry("VertSplitter",m_pMessageView ? tmp2 : m_VertSplitterSizesList);
#else
	cfg->writeEntry("VertSplitter",m_pMessageView ? m_pVertSplitter->sizes() : m_VertSplitterSizesList);
#endif
	cfg->writeEntry("PrivateBackground",m_privateBackground);
	cfg->writeEntry("DoubleView",m_pMessageView ? true : false);
	if(m_pUserListView)
		cfg->writeEntry("UserListHidden",m_pUserListView->isHidden());
	cfg->writeEntry("ToolButtonsHidden",buttonContainer()->isHidden());
}

void KviChannel::loadProperties(KviConfig *cfg)
{
	int w = width();
	KviValueList<int> def;
	def.append((w * 75) / 100);
	def.append((w * 15) / 100);
	def.append((w * 10) / 100);
	m_pTopSplitter->setSizes(cfg->readIntListEntry("TopSplitter",def));
	def.clear();
	def.append((w * 82) / 100);
	def.append((w * 18) / 100);
	m_pSplitter->setSizes(cfg->readIntListEntry("Splitter",def));
	//debug("SETTING DEFAULT SIZES");
	def.clear();
	
	def.append((w * 60) / 100);
	def.append((w * 40) / 100);
	m_VertSplitterSizesList=cfg->readIntListEntry("VertSplitter",def);
	showDoubleView(cfg->readBoolEntry("DoubleView",false));
	//def.append((w * 50) / 100);
	//def.append((w * 50) / 100);

	m_privateBackground = cfg->readPixmapEntry("PrivateBackground",KviPixmap());
	if(m_privateBackground.pixmap())
	{
		m_pIrcView->setPrivateBackgroundPixmap(*(m_privateBackground.pixmap()));
		if(m_pMessageView)m_pMessageView->setPrivateBackgroundPixmap(*(m_privateBackground.pixmap()));
	}

	KviWindow::loadProperties(cfg);
	if(m_pUserListView)
	{
		bool bHidden=cfg->readBoolEntry("UserListHidden",0);
		m_pUserListView->setHidden(bHidden);
		resizeEvent(0);
	}
	if(cfg->readBoolEntry("ToolButtonsHidden",KVI_OPTION_BOOL(KviOption_boolHideWindowToolButtons))!=buttonContainer()->isHidden())
		toggleToolButtons();
}


void KviChannel::showDoubleView(bool bShow)
{
	if(m_pMessageView)
	{
		if(bShow)return;
		m_pIrcView->joinMessagesFrom(m_pMessageView);
		m_VertSplitterSizesList=m_pVertSplitter->sizes();
		delete m_pMessageView;
		m_pMessageView = 0;
		if(m_pDoubleViewButton->isOn())m_pDoubleViewButton->setOn(false);
	} else {
		if(!bShow)return;
		m_pMessageView = new KviIrcView(m_pVertSplitter,m_pFrm,this);
		m_pVertSplitter->setSizes(m_VertSplitterSizesList);
		//setFocusHandler(m_pInput,m_pMessageView); //socket it!
		if(!(m_pDoubleViewButton->isOn()))m_pDoubleViewButton->setOn(true);
		if(m_privateBackground.pixmap())
		{
			m_pMessageView->setPrivateBackgroundPixmap(*(m_privateBackground.pixmap()));
		}
		connect(m_pMessageView,SIGNAL(rightClicked()),this,SLOT(textViewRightClicked()));
		m_pMessageView->setMasterView(m_pIrcView);
		m_pIrcView->splitMessagesTo(m_pMessageView);
		m_pMessageView->show();
	}	
}

void KviChannel::toggleDoubleView()
{
	showDoubleView(!m_pMessageView);
}

void KviChannel::toggleListView()
{
	if(m_pUserListView->isVisible())
	{
		m_pUserListView->hide();
		if(m_pListViewButton->isOn())m_pListViewButton->setOn(false);
	} else {
		m_pUserListView->show();
		if(!(m_pListViewButton->isOn()))m_pListViewButton->setOn(true);
	}
}


void KviChannel::toggleModeEditor()
{
	if(m_pModeEditor)
	{
		delete m_pModeEditor;
		m_pModeEditor = 0;
		m_pSplitter->setMinimumHeight(20); //gfgf
		if(m_pModeEditorButton->isOn()) m_pModeEditorButton->setOn(false);
		resizeEvent(0);
	} else {
		m_pModeEditor = new KviModeEditor(m_pSplitter,m_pModeEditorButton,"mode_editor",console(),m_szChannelMode,m_szChannelKey,m_szChannelLimit.ptr());
		connect(m_pModeEditor,SIGNAL(setMode(const char *)),this,SLOT(setMode(const char *)));
		connect(m_pModeEditor,SIGNAL(done()),this,SLOT(modeSelectorDone()));
		m_pModeEditor->show();
		//setFocusHandlerNoClass(m_pInput,m_pModeEditor,"QLineEdit");
		if(!m_pModeEditorButton->isOn())m_pModeEditorButton->setOn(true);
	}
}

void KviChannel::modeSelectorDone()
{
	if(m_pModeEditor)toggleModeEditor();
}

void KviChannel::setMode(const char * mode)
{
	if(!connection())return;
	KviQCString tmp = connection()->encodeText(m_szName);
	connection()->sendFmtData("MODE %s %s",tmp.data(),mode);
}

void KviChannel::toggleBanEditor()
{
	toggleEditor(&m_pBanEditor,&m_pBanEditorButton,
			m_pBanList,'b',"ban_editor");
}

void KviChannel::toggleBanExceptionEditor()
{
	toggleEditor(&m_pBanExceptionEditor,&m_pBanExceptionEditorButton,
			m_pBanExceptionList,'e',"ban_exception_editor");
}

void KviChannel::toggleInviteEditor()
{
	toggleEditor(&m_pInviteEditor,&m_pInviteEditorButton,
			m_pInviteList,'I',"invite_exception_editor");
}

void KviChannel::toggleEditor(KviMaskEditor ** ppEd,KviWindowToolPageButton ** ppBtn,KviPointerList<KviMaskEntry> *l,char flag,const char *edName)
{
	if(*ppEd)
	{
		delete *ppEd;
		*ppEd = 0;
		if(!(*ppBtn))return;
		if((*ppBtn)->isOn()) (*ppBtn)->setOn(false);
	} else {
		bool bHasList = true;
		switch(flag)
		{
			case 'b':
				if(!(bHasList = hasBanList()))
				{
					m_pBanList->clear();
					setSentBanListRequest();
				}
			break;
			case 'e':
				if(!(bHasList = hasBanExceptionList()))
				{
					m_pBanExceptionList->clear();
					setSentBanExceptionListRequest();
				}
			break;
			case 'I':
				if(!(bHasList = hasInviteList()))
				{
					m_pInviteList->clear();
					setSentInviteListRequest();
				}
			break;
		}
		if(!bHasList)
		{
			if(connection())
			{
				KviQCString szName = connection()->encodeText(m_szName);
				connection()->sendFmtData("MODE %s %c",szName.data(),flag);
			}
		}

		*ppEd = new KviMaskEditor(m_pSplitter,*ppBtn,l,flag,edName);
		connect(*ppEd,SIGNAL(removeMasks(KviMaskEditor *,KviPointerList<KviMaskEntry> *)),
			this,SLOT(removeMasks(KviMaskEditor *,KviPointerList<KviMaskEntry> *)));
		//setFocusHandler(m_pInput,*ppEd); //socket it!
		(*ppEd)->show();
		if(!(*ppBtn))return;
		if(!((*ppBtn)->isOn()))(*ppBtn)->setOn(true);
	}
}

void KviChannel::removeMasks(KviMaskEditor *ed,KviPointerList<KviMaskEntry> *l)
{
	KviStr masks;
	KviStr flags;
	unsigned int count = 0;
	for(KviMaskEntry * e = l->first();e;e = l->next())
	{
		if(masks.hasData())masks.append(' ');
		masks.append(e->szMask);
		flags.append(ed->flag());
		count++;
		if(count == connection()->serverInfo()->maxModeChanges())
		{
			if(connection())
			{
				KviQCString szName = connection()->encodeText(m_szName);
				connection()->sendFmtData("MODE %s -%s %s",szName.data(),flags.ptr(),connection()->encodeText(QString(masks)).data());
			}
			flags = "";
			masks = "";
			count = 0;
		}
	}
	if(masks.hasData())
	{
		if(connection())
		{
			KviQCString szName = connection()->encodeText(m_szName);
			connection()->sendFmtData("MODE %s -%s %s",szName.data(),flags.ptr(),connection()->encodeText(QString(masks)).data());
		}
	}
}

QPixmap * KviChannel::myIconPtr()
{
	return g_pIconManager->getSmallIcon((m_iStateFlags & KVI_CHANNEL_STATE_DEADCHAN) ? KVI_SMALLICON_DEADCHANNEL : KVI_SMALLICON_CHANNEL);
}

void KviChannel::resizeEvent(QResizeEvent *e)
{
#ifdef COMPILE_USE_QT4
	int hght = m_pInput->heightHint();
	int hght2 = m_pTopicWidget->sizeHint().height();
	m_pButtonBox->setGeometry(0,0,width(),hght2);
	m_pSplitter->setGeometry(0,hght2,width(),height() - (hght + hght2));
	m_pInput->setGeometry(0,height() - hght,width(),hght);
#else
	int hght = m_pInput->heightHint();
	int hght2 = m_pButtonBox->sizeHint().height();
	m_pButtonBox->setGeometry(0,0,width(),hght2);
	m_pSplitter->setGeometry(0,hght2,width(),height() - (hght + hght2));
	m_pInput->setGeometry(0,height() - hght,width(),hght);
#endif
}

QSize KviChannel::sizeHint() const
{
	QSize ret(m_pSplitter->sizeHint().width(),
		m_pIrcView->sizeHint().height() + m_pInput->heightHint() + m_pButtonBox->sizeHint().height());
	return ret;
}

void KviChannel::setChannelMode(char  mode,bool bAdd)
{
	if(!m_pConsole->connection()->serverInfo()->supportedListModes().contains(mode)){
		if(bAdd)
		{
			if(!(m_szChannelMode.contains(mode)))m_szChannelMode.append(mode);
		} else {
			if(m_szChannelMode.contains(mode))
			{
				m_szChannelMode.replace(mode,"");
			}
		}
		updateModeLabel();
		updateCaption();
	}
}

void KviChannel::setChannelKey(const char * key)
{
	m_szChannelKey = key;
	updateModeLabel();
	updateCaption();
}

void KviChannel::setChannelLimit(const char * limit)
{
	m_szChannelLimit = limit;
	updateModeLabel();
	updateCaption();
}

void KviChannel::addHighlightedUser(const char * nick)
{
	if(!m_pUserListView->findEntry(nick))return;
	else
   		m_pTmpHighLighted->replace(nick,new QString());
}

void KviChannel::removeHighlightedUser(const char * nick)
{
	m_pTmpHighLighted->remove(nick);
}

void KviChannel::getChannelModeString(QString &buffer)
{
	buffer = m_szChannelMode;
	if(!m_szChannelKey.isEmpty())buffer.append('k');
	if(m_szChannelLimit.hasData())buffer.append('l');
}

void KviChannel::setDeadChan()
{
	m_iStateFlags |= KVI_CHANNEL_STATE_DEADCHAN;
	m_iStateFlags &= ~(KVI_CHANNEL_STATE_NOCLOSEONPART | KVI_CHANNEL_STATE_SENTSYNCWHOREQUEST);

	m_pUserListView->enableUpdates(false);
	m_pUserListView->partAll();
	m_pUserListView->enableUpdates(true);
	m_pUserListView->setUserDataBase(0);

	m_pBanList->clear();
	m_pBanExceptionList->clear();
	m_pInviteList->clear();

	m_pActionHistory->clear();
	m_uActionHistoryHotActionCount = 0;

	m_szChannelMode = "";
	m_szChannelKey = "";
	m_szChannelLimit = "";

	// this should be moved to irc context!
	connection()->unregisterChannel(this);
	context()->registerDeadChannel(this);

	setType(KVI_WINDOW_TYPE_DEADCHANNEL);

	updateIcon();
	updateModeLabel();
	updateCaption();
}

void KviChannel::setAliveChan()
{
	// Rise like phoenix!
	m_iStateFlags = 0;
	setType(KVI_WINDOW_TYPE_CHANNEL);
	m_pUserListView->setUserDataBase(connection()->userDataBase());
	m_joinTime = QDateTime::currentDateTime();
	context()->unregisterDeadChannel(this);
	connection()->registerChannel(this);
	// Update log file name
	if(m_pIrcView->isLogging())m_pIrcView->startLogging();
	updateIcon();
	updateCaption();
	m_pTopicWidget->reset(); // reset the topic (fixes bug #20 signaled by Klaus Weidenbach)
}

void KviChannel::getTalkingUsersStats(QString &buffer,QStringList &l,bool bPast)
{
	if(l.count() < 1)return;

	if(l.count() == 1)
	{
		buffer += "<b>";
		buffer += l.first();
		buffer += "</b>";
		buffer += " ";
		buffer += bPast ? __tr2qs("said something recently") : __tr2qs("is talking");
	} else if(l.count() == 2)
	{
		buffer += "<b>";
		buffer += l.first();
		buffer += "</b> ";
		buffer += __tr2qs("and");
		buffer += " <b>";
		l.remove(l.begin());
		buffer += l.first();
		buffer += "</b> ";
		buffer += bPast ? __tr2qs("were talking recently") : __tr2qs("are talking");
	} else {
		buffer += "<b>";
		buffer += l.first();
		buffer += "</b>, <b>";
		l.remove(l.begin());
		buffer += l.first();
		if(l.count() == 2)
		{
			buffer += "</b> ";
			buffer += __tr2qs("and");
			buffer += " <b>";
			l.remove(l.begin());
			buffer += l.first();
			buffer += "</b>";
		} else {
			// (l.count() - 1) is > 1
			buffer += "</b> ";
			buffer += __tr2qs("and other %1 users").arg(l.count() - 1);
		}
		buffer += " ";
		buffer += bPast ? __tr2qs("were talking recently") : __tr2qs("are talking");
	}
}

void KviChannel::getTaskBarTipText(QString &buffer)
{
	static QString html_bold("<b>");
	static QString html_tab("&nbsp;&nbsp;");
	static QString html_eofbold("</b> ");
	static QString p5(" (");
	// p6 == p4
	static QString p7(" (");
	static QString p8(": ");
	static QString p9(")");
	static QString p10("<br>");

	static QString end_of_doc = "</table></body></html>";
	static QString end_of_fontboldrow = END_TABLE_BOLD_ROW;
	static QString start_of_row = "<tr><td>";
	static QString end_of_row = "</td></tr>";

	buffer = "<html>" \
				"<body>" \
					"<table width=\"100%\">"\
						START_TABLE_BOLD_ROW;

	if(m_iStateFlags & KVI_CHANNEL_STATE_DEADCHAN)
	{
		buffer += __tr2qs("Dead channel");
		buffer += end_of_fontboldrow;
		buffer += end_of_doc;
		return;
	}

	KviUserListViewUserStats s;
	m_pUserListView->userStats(&s);


	buffer += m_szPlainTextCaption;
	buffer += end_of_fontboldrow;

	buffer += start_of_row;
	
	QString op = __tr2qs("operator");
	QString ops = __tr2qs("operators");

	//////////////////////

	buffer += html_tab;
	buffer += html_bold;

	QString num;

	num.setNum(s.uActive);
	buffer += num;

	buffer += html_eofbold;
	buffer += (s.uActive == 1 ? __tr2qs("active user") : __tr2qs("active users"));

	buffer += p5;
	buffer += html_bold;

	num.setNum(s.uActiveOp);

	buffer += num;
	buffer += html_eofbold;
	buffer += (s.uActiveOp == 1 ? op : ops);

	buffer += p9;
/*
 * #warning FIXME: What is this supposed to mean?
	buffer += "<font size=\"-1\">";
	buffer += p7;

	buffer += __tr2qs("humanity");

	buffer += p8;
	buffer += html_bold;

	num.setNum(s.iAvgTemperature);

	buffer += num;
	buffer += "</bold>";

	buffer += p9;
*/
	buffer += p10;
	buffer += "</font>";



	//////////////////////

	buffer += html_tab;
	buffer += html_bold;

	num.setNum(s.uHot);
	buffer += num;

	buffer += html_eofbold;
	buffer += (s.uHot == 1 ? __tr2qs("hot user") : __tr2qs("hot users"));

	buffer += p5;
	buffer += html_bold;

	num.setNum(s.uHotOp);

	buffer += num;
	buffer += html_eofbold;
	buffer += (s.uHotOp == 1 ? op : ops);

	buffer += p9;

	/////////////

	buffer += end_of_row;
	buffer += start_of_row;

	///////////////////

	if(s.uChanOwner > 0)
	{
		buffer += html_tab;
		buffer += html_bold;
		num.setNum(s.uChanOwner);
		buffer += num;
		buffer += html_eofbold;
		buffer += (s.uChanOwner == 1 ? __tr2qs("channel owner") : __tr2qs("channel owners"));
		buffer += p10;
	}

	if(s.uChanAdmin > 0)
	{
		buffer += html_tab;
		buffer += html_bold;
		num.setNum(s.uChanAdmin);
		buffer += num;
		buffer += html_eofbold;
		buffer += (s.uChanAdmin == 1 ? __tr2qs("channel administrator") : __tr2qs("channel administrators"));
		buffer += p10;
	}

	if(s.uOp > 0)
	{
		buffer += html_tab;
		buffer += html_bold;
		num.setNum(s.uOp);
		buffer += num;
		buffer += html_eofbold;
		buffer += (s.uOp == 1 ? op : ops);
		buffer += p10;
	}

	if(s.uHalfOp > 0)
	{
		buffer += html_tab;
		buffer += html_bold;
		num.setNum(s.uHalfOp);
		buffer += num;
		buffer += html_eofbold;
		buffer += (s.uHalfOp == 1 ? __tr2qs("half-operator") : __tr2qs("half-operators"));
		buffer += p10;
	}

	if(s.uVoiced > 0)
	{
		buffer += html_tab;
		buffer += html_bold;
		num.setNum(s.uVoiced);
		buffer += num;
		buffer += html_eofbold;
		buffer += (s.uVoiced == 1 ? __tr2qs("voiced user") : __tr2qs("voiced users"));
		buffer += p10;
	}

	if(s.uUserOp > 0)
	{
		buffer += html_tab;
		buffer += html_bold;
		num.setNum(s.uUserOp);
		buffer += num;
		buffer += html_eofbold;
		buffer += (s.uUserOp == 1 ? __tr2qs("user-operator") : __tr2qs("user-operators"));
		buffer += p10;
	}

	buffer += html_tab;
	buffer += html_bold;
	num.setNum(s.uTotal);
	buffer += num;
	buffer += html_eofbold;
	buffer += (s.uTotal == 1 ? __tr2qs("user total") : __tr2qs("users total"));

	buffer += end_of_row;

	KviChannelActivityStats cas;
	getChannelActivityStats(&cas);


	if(cas.lTalkingUsers.count() > 0)
	{
		if((cas.lTalkingUsers.count() < 3) && (cas.lWereTalkingUsers.count() > 0))
		{
			buffer += "<tr><td bgcolor=\"#E0E0E0\">";
			getTalkingUsersStats(buffer,cas.lWereTalkingUsers,true);
			buffer += end_of_row;
		}
		buffer += "<tr><td bgcolor=\"#E0E0E0\">";
		getTalkingUsersStats(buffer,cas.lTalkingUsers,false);
		buffer += end_of_row;
	} else {
		if(cas.lWereTalkingUsers.count() > 0)
		{
			buffer += "<tr><td bgcolor=\"#E0E0E0\">";
			getTalkingUsersStats(buffer,cas.lWereTalkingUsers,true);
			buffer += end_of_row;
		}
	}

	buffer += "<tr><td bgcolor=\"#A0A0A0\"><b>";

	if(cas.dActionsPerMinute < 0.1)buffer += __tr2qs("No activity");
	else if(cas.dActionsPerMinute < 0.3)buffer += __tr2qs("Minimal activity");
	else if(cas.dActionsPerMinute < 1.0)buffer += __tr2qs("Very low activity");
	else if(cas.dActionsPerMinute < 3.0)buffer += cas.bStatsInaccurate ? __tr2qs("Might be low activity") : __tr2qs("Low activity");
	else if(cas.dActionsPerMinute < 10.0)buffer += cas.bStatsInaccurate ? __tr2qs("Might be medium activity") : __tr2qs("Medium activity");
	else if(cas.dActionsPerMinute < 30.0)buffer += cas.bStatsInaccurate ? __tr2qs("Might be high activity") : __tr2qs("High activity");
	else if(cas.dActionsPerMinute < 60.0)buffer += cas.bStatsInaccurate ? __tr2qs("Might be very high activity") : __tr2qs("Very high activity");
	else buffer += cas.bStatsInaccurate ? __tr2qs("Might be flooded with messages") : __tr2qs("Flooded with messages");


	if(cas.dActionsPerMinute >= 0.1)
	{
		QString num;
		num.sprintf(" [%u%% ",cas.uHotActionPercent);
		buffer += num;
		buffer += __tr2qs("human");
		buffer += "]";
	}


	buffer += "</b></td></tr>";

	buffer += end_of_doc;
}

void KviChannel::fillCaptionBuffers()
{
	static QString begin("<nobr><font color=\"");
	static QString boldbegin("\"><b>");
	static QString endofbold("</b></font> <font color=\"");
	static QString endoffont("\">");
	static QString end("</font></nobr>");

	if(!connection())
	{
		QString dead = __tr2qs("[Dead channel]");

		m_szNameWithUserFlag = m_szName;

		m_szPlainTextCaption = m_szName;
		m_szPlainTextCaption += " : ";
		m_szPlainTextCaption += dead;

		m_szHtmlActiveCaption = begin;
		m_szHtmlActiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextActive).name();
		m_szHtmlActiveCaption += boldbegin;
		m_szHtmlActiveCaption += m_szName;
		m_szHtmlActiveCaption += endofbold;
		m_szHtmlActiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextActive2).name();
		m_szHtmlActiveCaption += endoffont;
		m_szHtmlActiveCaption += dead;
		m_szHtmlActiveCaption += end;

		m_szHtmlInactiveCaption = begin;
		m_szHtmlInactiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextInactive).name();
		m_szHtmlInactiveCaption += boldbegin;
		m_szHtmlInactiveCaption += m_szName;
		m_szHtmlInactiveCaption += endofbold;
		m_szHtmlInactiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextInactive2).name();
		m_szHtmlInactiveCaption += endoffont;
		m_szHtmlInactiveCaption += dead;
		m_szHtmlInactiveCaption += end;
		return;
	}

	char uFlag = getUserFlag(connection()->currentNickName());


	if(uFlag)
	{
		m_szNameWithUserFlag = QChar(uFlag);
		m_szNameWithUserFlag += m_szName;
	} else {
		m_szNameWithUserFlag = m_szName;
	}

	QString szChanMode;
	getChannelModeString(szChanMode);

	m_szPlainTextCaption = m_szNameWithUserFlag;
	if(!szChanMode.isEmpty())
	{
		m_szPlainTextCaption += " (+";
		m_szPlainTextCaption += szChanMode;
		m_szPlainTextCaption += QChar(')');
	}

	QString szNickOnServer = QChar('[');
	szNickOnServer += connection()->currentNickName();
	szNickOnServer += __tr2qs(" on ");
	szNickOnServer += connection()->currentServerName();
	szNickOnServer += QChar(']');

	m_szPlainTextCaption += QChar(' ');
	m_szPlainTextCaption += szNickOnServer;

	m_szHtmlActiveCaption = begin;
	m_szHtmlActiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextActive).name();
	m_szHtmlActiveCaption += boldbegin;
	m_szHtmlActiveCaption += m_szNameWithUserFlag;
	m_szHtmlActiveCaption += endofbold;
	m_szHtmlActiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextActive2).name();
	m_szHtmlActiveCaption += endoffont;
	m_szHtmlActiveCaption += szNickOnServer;
	m_szHtmlActiveCaption += end;

	m_szHtmlInactiveCaption = begin;
	m_szHtmlInactiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextInactive).name();
	m_szHtmlInactiveCaption += boldbegin;
	m_szHtmlInactiveCaption += m_szNameWithUserFlag;
	m_szHtmlInactiveCaption += endofbold;
	m_szHtmlInactiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextInactive2).name();
	m_szHtmlInactiveCaption += endoffont;
	m_szHtmlInactiveCaption += szNickOnServer;
	m_szHtmlInactiveCaption += end;
}

void KviChannel::ownMessage(const QString &buffer)
{
	if(!connection())return;

	KviQCString szName = connection()->encodeText(windowName());
	KviQCString szData = encodeText(buffer);
	const char * d = szData.data();
	if(!d)return;
	
#ifdef COMPILE_CRYPT_SUPPORT
	if(cryptSessionInfo())
	{
		if(cryptSessionInfo()->bDoEncrypt)
		{
			if(*d != KVI_TEXT_CRYPTESCAPE)
			{
				KviStr encrypted;
				cryptSessionInfo()->pEngine->setMaxEncryptLen(500 - szName.length());
				switch(cryptSessionInfo()->pEngine->encrypt(d,encrypted))
				{
					case KviCryptEngine::Encrypted:
						if(!connection()->sendFmtData("PRIVMSG %s :%s",szName.data(),encrypted.ptr()))return;
						m_pConsole->outputPrivmsg(this,KVI_OUT_OWNPRIVMSGCRYPTED,
								QString::null,QString::null,QString::null,buffer,KviConsole::NoNotifications);
					break;
					case KviCryptEngine::Encoded:
					{
						if(!connection()->sendFmtData("PRIVMSG %s :%s",szName.data(),encrypted.ptr()))return;
						// ugly ,but we must redecode here
						QString szRedecoded = decodeText(encrypted.ptr());
						m_pConsole->outputPrivmsg(this,KVI_OUT_OWNPRIVMSG,
							QString::null,QString::null,QString::null,szRedecoded,KviConsole::NoNotifications);
					}
					break;
					default: // also case KviCryptEngine::EncryptError
					{
						QString szEngineError = cryptSessionInfo()->pEngine->lastError();
						output(KVI_OUT_SYSTEMERROR,
							__tr2qs("The crypto engine was unable to encrypt the current message (%Q): %Q, no data sent to the server"),
							&buffer,&szEngineError);
					}
					break;
				}
				userAction(connection()->currentNickName(),KVI_USERACTION_PRIVMSG);
				return;
			} else {
				d++; //eat the escape code
				QString tmp = buffer.right(buffer.length() - 1);
				if(!connection()->sendFmtData("PRIVMSG %s :%s",szName.data(),d))return;
				m_pConsole->outputPrivmsg(this,KVI_OUT_OWNPRIVMSG,QString::null,QString::null,QString::null,tmp,KviConsole::NoNotifications);
				userAction(connection()->currentNickName(),KVI_USERACTION_PRIVMSG);
				return;
			}
		}
	}
#endif

	if(connection()->sendFmtData("PRIVMSG %s :%s",szName.data(),d))
	{
		m_pConsole->outputPrivmsg(this,KVI_OUT_OWNPRIVMSG,QString::null,QString::null,QString::null,buffer,KviConsole::NoNotifications);
		userAction(connection()->currentNickName(),KVI_USERACTION_PRIVMSG);
	}
}

void KviChannel::ownAction(const QString &buffer)
{
	if(!connection())return;
	KviQCString szName = connection()->encodeText(m_szName);
	KviQCString szData = encodeText(buffer);
	const char * d = szData.data();
	if(!d)return;
	if(!connection()->sendFmtData("PRIVMSG %s :%cACTION %s%c",szName.data(),0x01,d,0x01))return;
	if(KVS_TRIGGER_EVENT_1_HALTED(KviEvent_OnMeAction,this,QString(d)))return;
	QString szBuffer = "\r!nc\r";
	szBuffer += connection()->currentNickName();
	szBuffer += "\r ";
	szBuffer += buffer;
	outputMessage(KVI_OUT_ACTION,szBuffer);
	userAction(connection()->currentNickName(),KVI_USERACTION_ACTION);
}

bool KviChannel::nickChange(const QString &oldNick,const QString &newNick)
{
	bool bWasHere = m_pUserListView->nickChange(oldNick,newNick);
	if(bWasHere)channelAction(newNick,KVI_USERACTION_NICK,kvi_getUserActionTemperature(KVI_USERACTION_NICK));
	return bWasHere;
}

bool KviChannel::part(const QString &nick)
{
	bool bWasHere = m_pUserListView->part(nick);
	if(bWasHere)channelAction(nick,KVI_USERACTION_PART,kvi_getUserActionTemperature(KVI_USERACTION_PART));
	return bWasHere;
}


#define KVI_CHANACTIVITY_LIMIT_ICE 5
#define KVI_CHANACTIVITY_LIMIT_VERYCOLD 10
#define KVI_CHANACTIVITY_LIMIT_COLD 20
#define KVI_CHANACTIVITY_LIMIT_UNDEFINED 30
#define KVI_CHANACTIVITY_LIMIT_HOT 50
#define KVI_CHANACTIVITY_LIMIT_VERYHOT 70


bool KviChannel::activityMeter(unsigned int * puActivityValue,unsigned int * puActivityTemperature)
{
	fixActionHistory();

	unsigned int uHotActionPercent;
	double dActionsPerMinute;

	if(m_pActionHistory->count() < 1)
	{
		// nothing is happening
		uHotActionPercent = 0;
		dActionsPerMinute = 0;
	} else {
		kvi_time_t tNow = kvi_unixTime();
	
		KviChannelAction * a = m_pActionHistory->last();
	
		double dSpan = (double)(tNow - a->tTime);
	
		if(m_pActionHistory->count() < KVI_CHANNEL_ACTION_HISTORY_MAX_COUNT)
		{
			if(m_joinTime.secsTo(QDateTime::currentDateTime()) < KVI_CHANNEL_ACTION_HISTORY_MAX_TIMESPAN)
			{
				// we can't exactly estimate
				if(dSpan < 60.0)dSpan = 60.0;
			} else {
				// there are less actions at all or they have been pushed out because of the timespan
				dSpan = KVI_CHANNEL_ACTION_HISTORY_MAX_TIMESPAN;
			}
		} // else the actions have been pushed out of the history because they were too much
	
		if(dSpan > 0.0)
			dActionsPerMinute = (((double)(m_pActionHistory->count())) / (dSpan)) * 60.0;
		else
			dActionsPerMinute = (double)(m_pActionHistory->count()); // ???
	
		uHotActionPercent = (m_uActionHistoryHotActionCount * 100) / (m_pActionHistory->count());
	}


	if(dActionsPerMinute < 0.3)*puActivityValue = KVI_ACTIVITY_NONE;
	else if(dActionsPerMinute < 1.0)*puActivityValue = KVI_ACTIVITY_VERYLOW;
	else if(dActionsPerMinute < 4.0)*puActivityValue = KVI_ACTIVITY_LOW;
	else if(dActionsPerMinute < 10.0)*puActivityValue = KVI_ACTIVITY_MEDIUM;
	else if(dActionsPerMinute < 30.0)*puActivityValue = KVI_ACTIVITY_HIGH;
	else *puActivityValue = KVI_ACTIVITY_VERYHIGH;

	if(uHotActionPercent < KVI_CHANACTIVITY_LIMIT_ICE)*puActivityTemperature = KVI_ACTIVITY_ICE;
	else if(uHotActionPercent < KVI_CHANACTIVITY_LIMIT_VERYCOLD)*puActivityTemperature = KVI_ACTIVITY_VERYCOLD;
	else if(uHotActionPercent < KVI_CHANACTIVITY_LIMIT_COLD)*puActivityTemperature = KVI_ACTIVITY_COLD;
	else if(uHotActionPercent < KVI_CHANACTIVITY_LIMIT_UNDEFINED)*puActivityTemperature = KVI_ACTIVITY_UNDEFINED;
	else if(uHotActionPercent < KVI_CHANACTIVITY_LIMIT_HOT)*puActivityTemperature = KVI_ACTIVITY_HOT;
	else if(uHotActionPercent < KVI_CHANACTIVITY_LIMIT_VERYHOT)*puActivityTemperature = KVI_ACTIVITY_VERYHOT;
	else *puActivityTemperature = KVI_ACTIVITY_FIRE;

	return true;
}


void KviChannel::channelAction(const QString &nick,unsigned int uActionType,int iTemperature)
{
	KviChannelAction * a = new KviChannelAction;
	a->tTime = kvi_unixTime();
	a->uActionType = uActionType;
	a->iTemperature = iTemperature;
	a->szNick = nick;

	if(iTemperature > 0)m_uActionHistoryHotActionCount++;

	m_pActionHistory->append(a);
	fixActionHistory();
}

void KviChannel::fixActionHistory()
{
	while(m_pActionHistory->count() > KVI_CHANNEL_ACTION_HISTORY_MAX_COUNT)m_pActionHistory->removeFirst();
	KviChannelAction * a = m_pActionHistory->last();
	if(!a)return;

	kvi_time_t tMinimum = a->tTime - KVI_CHANNEL_ACTION_HISTORY_MAX_TIMESPAN;

	KviChannelAction * act = m_pActionHistory->first();
	while(act && (act->tTime < tMinimum))
	{
		if(act->iTemperature > 0)m_uActionHistoryHotActionCount--;
		m_pActionHistory->removeFirst();
		act = m_pActionHistory->first();
	}
}


void KviChannel::lostUserFocus()
{
	KviWindow::lostUserFocus();
	if(!m_pMessageView)return;
	if(m_pMessageView->hasLineMark())m_pMessageView->clearLineMark(true);
}


void KviChannel::getChannelActivityStats(KviChannelActivityStats * s)
{
	fixActionHistory();

	s->uActionCount = m_pActionHistory->count();
	s->iAverageActionTemperature = 0;
	s->uActionsInTheLastMinute = 0;
	s->uHotActionCount = 0;
	s->uHotActionPercent = 0;
	s->bStatsInaccurate = false;

	if(s->uActionCount < 1)
	{
		// nothing is happening
		s->uLastActionTimeSpan = 0;
		s->uFirstActionTimeSpan = 0;
		s->dActionsPerMinute = 0;

		return;
	}

	kvi_time_t tNow = kvi_unixTime();

	KviChannelAction * a = m_pActionHistory->last();
	s->uLastActionTimeSpan = tNow - a->tTime;

	a = m_pActionHistory->first();
	s->uFirstActionTimeSpan = tNow - a->tTime;

	double dSpan = (double)s->uFirstActionTimeSpan;

	if(s->uActionCount < KVI_CHANNEL_ACTION_HISTORY_MAX_COUNT)
	{
		if(m_joinTime.secsTo(QDateTime::currentDateTime()) < KVI_CHANNEL_ACTION_HISTORY_MAX_TIMESPAN)
		{
			// we can't exactly estimate
			s->bStatsInaccurate = true;
			if(dSpan < 60.0)dSpan = 60.0;
		} else {
			// there are less actions at all or they have been pushed out because of the timespan
			dSpan = KVI_CHANNEL_ACTION_HISTORY_MAX_TIMESPAN;
		}
	} // else the actions have been pushed out of the history because they were too much

	if(dSpan > 0.0)
		s->dActionsPerMinute = (((double)s->uActionCount) / (dSpan)) * 60.0;
	else
		s->dActionsPerMinute = (double)s->uActionCount; // ???

	kvi_time_t tTwoMinsAgo = tNow;
	tTwoMinsAgo-= 120;
	tNow -= 60;

	KviPointerHashTable<QString,int> userDict;
	userDict.setAutoDelete(false);

	int fake;
	s->lTalkingUsers.clear();
	s->lWereTalkingUsers.clear();

	for(a = m_pActionHistory->last();a;a = m_pActionHistory->prev())
	{
		if(a->tTime >= tNow)s->uActionsInTheLastMinute++;

		if(a->iTemperature > 0)s->uHotActionCount++;
		s->iAverageActionTemperature += a->iTemperature;

		if((a->uActionType == KVI_USERACTION_PRIVMSG) ||
			(a->uActionType == KVI_USERACTION_NOTICE) ||
			(a->uActionType == KVI_USERACTION_ACTION))
		{
			if(!userDict.find(a->szNick))
			{
				if(isOn(a->szNick.ascii()))
				{
					if(a->tTime >= tTwoMinsAgo)s->lTalkingUsers.append(a->szNick);
					else s->lWereTalkingUsers.append(a->szNick);
					userDict.insert(a->szNick,&fake);
				}
			}
		}
	}

	s->iAverageActionTemperature = s->iAverageActionTemperature / (int)s->uActionCount;

	s->uHotActionPercent = (s->uHotActionCount * 100) / s->uActionCount;
}



void KviChannel::userAction(const QString &nick,const QString &user,const QString &host,unsigned int uActionType)
{
	int iTemperature = kvi_getUserActionTemperature(uActionType);
	channelAction(nick,uActionType,iTemperature);
	m_pUserListView->userAction(nick,user,host,iTemperature);
}

void KviChannel::userAction(const QString &nick,unsigned int uActionType)
{
	int iTemperature = kvi_getUserActionTemperature(uActionType);
	channelAction(nick,uActionType,iTemperature);
	m_pUserListView->userAction(nick,iTemperature);
}

void KviChannel::userAction(KviIrcMask * user,unsigned int uActionType)
{
	int iTemperature = kvi_getUserActionTemperature(uActionType);
	channelAction(user->nick(),uActionType,iTemperature);
	m_pUserListView->userAction(user,iTemperature);
}

void KviChannel::topicSelected(const QString & topic)
{
	if(!connection())return;
	KviQCString szEncoded = encodeText(topic);
	KviQCString szName = connection()->encodeText(m_szName);
	connection()->sendFmtData("TOPIC %s :%s",szName.data(),szEncoded.length() ? szEncoded.data() : "");
}

void KviChannel::closeEvent(QCloseEvent *e)
{
	if((m_iStateFlags & KVI_CHANNEL_STATE_SENTPART) || (m_iStateFlags & KVI_CHANNEL_STATE_DEADCHAN) || !(m_pConsole->isConnected()))
	{
		m_pContext->unregisterDeadChannel(this);
		KviWindow::closeEvent(e);
	} else {
		e->ignore();
		// FIXME: #warning "THIS PART SHOULD BECOME A COMMAND /PART $option()..so the identifiers are parsed"
		if(connection())
		{
			QString tmp = KVI_OPTION_STRING(KviOption_stringPartMessage);
			tmp.replace(";","\\;");
			tmp.replace("\n"," ");
			KviKvsVariant vRet;
			
			if(KviKvsScript::evaluate(tmp,this,0,&vRet))vRet.asString(tmp);

			KviQCString dat = encodeText(tmp);
			partMessageSent();
			KviQCString szName = connection()->encodeText(m_szName);
			connection()->sendFmtData("PART %s :%s",szName.data(),dat.data() ? dat.data() : "");
			// be sure to not reference ourselves here.. we could be disconnected!
		} else {
			partMessageSent(); // huh ?
		}
	}
}

void KviChannel::partMessageSent(bool bCloseOnPart,bool bShowMessage)
{
	m_iStateFlags |= KVI_CHANNEL_STATE_SENTPART;
	if(!bCloseOnPart)m_iStateFlags |= KVI_CHANNEL_STATE_NOCLOSEONPART;
	if(bShowMessage)outputNoFmt(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Sent part request, waiting for reply..."));
}

#define IS_FNC(__name,__ulvname) \
bool KviChannel::__name(bool bAtLeast) \
{ \
	if(!connection())return false; \
	return m_pUserListView->__ulvname(connection()->currentNickName(),bAtLeast); \
}

IS_FNC(isMeChanOwner,isChanOwner)
IS_FNC(isMeChanAdmin,isChanAdmin)
IS_FNC(isMeOp,isOp)
IS_FNC(isMeVoice,isVoice)
IS_FNC(isMeHalfOp,isHalfOp)
IS_FNC(isMeUserOp,isUserOp)

int KviChannel::myFlags()
{
	if(!connection())return 0;
	return m_pUserListView->flags(connection()->currentNickName());
}


void KviChannel::setMask(char flag, const QString &mask,bool bAdd,const QString &setBy,unsigned int setAt)
{
	if(!connection())return;
	KviPointerList<KviMaskEntry> * list = m_pBanList;
	KviMaskEditor * editor = m_pBanEditor;
	switch(flag)
	{
		case 'b':
			m_iStateFlags ^= KVI_CHANNEL_STATE_HAVEBANLIST;
		break;
		case 'e':
			m_iStateFlags ^= KVI_CHANNEL_STATE_HAVEBANEXCEPTIONLIST;
			list = m_pBanExceptionList;
			editor = m_pBanExceptionEditor;
		break;
		case 'I':
			m_iStateFlags ^= KVI_CHANNEL_STATE_HAVEINVITELIST;
			list = m_pInviteList;
			editor = m_pInviteEditor;
		break;
	}
			
	internalMask(mask,bAdd,setBy,setAt,list,&editor);
	m_pUserListView->setMaskEntries(flag,(int)list->count());
}

void KviChannel::internalMask(const QString &mask,bool bAdd,const QString &setBy,unsigned int setAt,KviPointerList<KviMaskEntry> *l,KviMaskEditor **ppEd)
{
	KviMaskEntry * e = 0;
	if(bAdd)
	{
		for(e = l->first();e;e = l->next())
		{
			if(KviQString::equalCI(e->szMask,mask))return; //already there
		}
		e = new KviMaskEntry;
		e->szMask = mask;
		e->szSetBy = (!setBy.isEmpty()) ? setBy : __tr2qs("(Unknown)");
		e->uSetAt = setAt;
		l->append(e);
		if(*ppEd)(*ppEd)->addMask(e);
	} else {
		for(e = l->first();e;e = l->next())
		{
			if(KviQString::equalCI(e->szMask,mask))break;
		}
		if(e)
		{
			if(*ppEd)(*ppEd)->removeMask(e);
			l->removeRef(e);
		}
	}
}

void KviChannel::updateModeLabel()
{
	QString tmp = m_szChannelMode;
	QString tip = __tr2qs("<b>Channel mode:</b>");
	//const char * aux = m_szChannelMode.utf8().data(); leaks memory and will not work with getChannelModeDescription() (can channel modes be multibyte ?)
	KviStr mod = m_szChannelMode;
	const char * aux = mod.ptr();
	while(*aux)
	{
		KviQString::appendFormatted(tip,"<br>%c: %Q",*aux,&(m_pConsole->connection()->serverInfo()->getChannelModeDescription(*aux)));
		++aux;
	}

	if(!m_szChannelKey.isEmpty())
	{
		if(!tmp.isEmpty())tmp.append(' ');
		KviQString::appendFormatted(tmp,"k:%s",m_szChannelKey.utf8().data());
		KviQString::appendFormatted(tip,__tr2qs("<br><b>Key:</b> %s"),m_szChannelKey.utf8().data());
	}

	if(m_szChannelLimit.hasData())
	{
		if(!tmp.isEmpty())tmp.append(' ');
		KviQString::appendFormatted(tmp,"l:%s",m_szChannelLimit.ptr());
		KviQString::appendFormatted(tip,__tr2qs("<br><b>Limit:</b> %s"),m_szChannelLimit.ptr());
	}

	m_pModeWidget->refreshModes();
	KviTalToolTip::remove(m_pModeWidget);
	KviTalToolTip::add(m_pModeWidget,tip);
}

/*
void KviChannel::outputMessage(int msg_type,const char *format,...)
{
	kvi_wchar_t txt_ptr[512]; //It should be enough for all outputs...
	kvi_va_list list;
	kvi_va_start(list,format);
	if(kvi_wvsnprintcf(txt_ptr,512,format,list) < 0){
		//Just in case...
		kvi_va_end(list);
		int len = 512;
		kvi_wchar_t *long_txt_ptr = 0;
		int result;
		do{
			len += 512;
			//first time long_txt_ptr == 0 so it is equivalent to malloc
			//At least the man page says that...
			long_txt_ptr = (kvi_wchar_t *)kvi_realloc((void *)long_txt_ptr,len * sizeof(kvi_wchar_t));
			kvi_va_start(list,format);
			result = kvi_wvsnprintcf(long_txt_ptr,len,format,list);
			kvi_va_end(list);
		} while(result < 0);
		internalOutput(m_pMessageView ? m_pMessageView : m_pIrcView,msg_type,long_txt_ptr);
		kvi_free((void *)long_txt_ptr);
	} else {
		//Succesful vsnprintf
		kvi_va_end(list);
		internalOutput(m_pMessageView ? m_pMessageView : m_pIrcView,msg_type,txt_ptr);
	}
}
*/

void KviChannel::outputMessage(int msg_type,const QString &msg)
{
	QString szBuf(msg);
	preprocessMessage(szBuf);
	const QChar * pC = KviQString::nullTerminatedArray(szBuf);
	if(!pC)return;
	internalOutput(m_pMessageView ? m_pMessageView : m_pIrcView,msg_type,(const kvi_wchar_t *)pC);
}



void KviChannel::checkChannelSync()
{
	if(m_iStateFlags & KVI_CHANNEL_STATE_SYNCHRONIZED)return;

	if(m_iStateFlags & KVI_CHANNEL_STATE_SENTWHOREQUEST)
	{
		if(!(m_iStateFlags & KVI_CHANNEL_STATE_HAVEWHOLIST))return;
	}

	if(m_iStateFlags & KVI_CHANNEL_STATE_SENTBANLISTREQUEST)
	{
		if(!(m_iStateFlags & KVI_CHANNEL_STATE_HAVEBANLIST))return;
	}

	if(m_iStateFlags & KVI_CHANNEL_STATE_SENTBANEXCEPTIONLISTREQUEST)
	{
		if(!(m_iStateFlags & KVI_CHANNEL_STATE_HAVEBANEXCEPTIONLIST))return;
	}

	if(m_iStateFlags & KVI_CHANNEL_STATE_SENTINVITELISTREQUEST)
	{
		if(!(m_iStateFlags & KVI_CHANNEL_STATE_HAVEINVITELIST))return;
	}

	m_iStateFlags |= KVI_CHANNEL_STATE_SYNCHRONIZED;
	// we already have all the spontaneous server replies
	// (so probably mode, topic (or no topic is set),names)
	// we have already received the I and e lists (if requested)
	kvs_int_t iSyncTime = m_joinTime.time().msecsTo(QTime::currentTime());
	if(iSyncTime < 0)iSyncTime += 86400000;

	bool bStop = KVS_TRIGGER_EVENT_1_HALTED(KviEvent_OnChannelSync,this,iSyncTime);

	if(!bStop && KVI_OPTION_BOOL(KviOption_boolShowChannelSyncTime))
	{
		output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Channel synchronized in %d.%d seconds"),iSyncTime / 1000,iSyncTime % 1000);
	}
}

bool KviChannel::eventFilter(QObject * o, QEvent * e)
{
	if(e->type() == QEvent::FocusOut && o == m_pTopicWidget && \
		m_pTopicWidget->isVisible())
		m_pTopicWidget->deactivate();
	
	return KviWindow::eventFilter(o, e);
}


void KviChannel::preprocessMessage(QString & szMessage)
{
	QStringList strings = QStringList::split(" ",szMessage, TRUE);
	for ( QStringList::Iterator it = strings.begin(); it != strings.end(); ++it ) {
		if((*it).contains('\r')) continue;
		QString tmp = KviMircCntrl::stripControlBytes(*it);
		if( findEntry(*it) ) 	*it=QString("\r!n\r%1\r").arg(*it);
		if(m_pConsole)
			if(m_pConsole->connection())
				if(m_pConsole->connection()->serverInfo()->supportedChannelTypes().contains(tmp[0]))
					if((*it)==tmp)
						*it=QString("\r!c\r%1\r").arg(*it);
					else
						*it=QString("\r!c%1\r%2\r").arg(tmp).arg(*it);
	}
	szMessage=strings.join(" ");
}

void KviChannel::unhighlight()
{
	if(!m_pTaskBarItem)return;
	m_pTaskBarItem->unhighlight();
}

#include "kvi_channel.moc"
