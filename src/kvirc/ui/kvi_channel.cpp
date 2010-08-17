//=============================================================================
//
//   File : kvi_channel.cpp
//   Creation date : Tue Aug  1 2000 02:20:22 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_windowlist.h"
#include "kvi_frame.h"
#include "kvi_config.h"
#include "kvi_maskeditor.h"
#include "kvi_mirccntrl.h"
#include "kvi_settings.h"
#include "kvi_parameterlist.h"
#include "kvi_modeeditor.h"
#include "kvi_app.h"
#include "kvi_useraction.h"
#include "kvi_toolwindows_container.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnectionuserinfo.h"
#include "kvi_ircconnectionserverinfo.h"
#include "kvi_defaults.h"
#include "kvi_sparser.h"
#include "kvi_modew.h"
#include "kvi_mirccntrl.h"
#include "kvi_pointerhashtable.h"
#include "kvi_kvs_script.h"
#include "kvi_kvs_eventtriggers.h"
#include "kvi_tal_popupmenu.h"

#ifdef COMPILE_CRYPT_SUPPORT
	#include "kvi_crypt.h"
	#include "kvi_cryptcontroller.h"
#endif

#ifdef COMPILE_ZLIB_SUPPORT
	#include <zlib.h>
#endif

#include <time.h>

#include <QDir>
#include <QFileInfo>
#include <QDate>
#include <QByteArray>
#include <QLabel>
#include <QEvent>
#include <QPalette>
#include <QMessageBox>
#include <QCloseEvent>


// FIXME: #warning "+a Anonymous channel mode!"
// FIXME: #warning "+R channel mode (reop)"
// FIXME: #warning "OnChannelFlood event...."


KviChannel::KviChannel(KviFrame * lpFrm, KviConsole * lpConsole, const QString & szName)
: KviWindow(KVI_WINDOW_TYPE_CHANNEL,lpFrm,szName,lpConsole)
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
	m_pQuietBanList        = new KviPointerList<KviMaskEntry>;
	m_pQuietBanList->setAutoDelete(true);
	m_pActionHistory = new KviPointerList<KviChannelAction>;
	m_pActionHistory->setAutoDelete(true);
	m_uActionHistoryHotActionCount = 0;

	m_pTmpHighLighted      = new QStringList();

	// Register ourselves
	connection()->registerChannel(this);
	// And create the widgets layout
	// Button box
	m_pButtonBox = new KviTalHBox(this);
	m_pButtonBox->setSpacing(0);
	m_pButtonBox->setMargin(0);

	m_pTopSplitter = new KviTalSplitter(Qt::Horizontal,m_pButtonBox);
	m_pTopSplitter->setChildrenCollapsible(false);

	m_pButtonBox->setStretchFactor(m_pTopSplitter,1);

	m_pButtonContainer = new KviTalHBox(m_pButtonBox);
	m_pButtonContainer->setSpacing(0);
	m_pButtonContainer->setMargin(0);
	// Topic widget on the left
	m_pTopicWidget = new KviTopicWidget(m_pTopSplitter, this, "topic_widget");

	connect(m_pTopicWidget,SIGNAL(topicSelected(const QString &)),
		this,SLOT(topicSelected(const QString &)));
	// mode label follows the topic widget
	m_pModeWidget = new KviModeWidget(m_pTopSplitter,this,"mode_");
	KviTalToolTip::add(m_pModeWidget,__tr2qs("Channel mode"));

	createTextEncodingButton(m_pButtonContainer);

	// Central splitter
	m_pSplitter = new KviTalSplitter(Qt::Horizontal,this);
	m_pSplitter->setObjectName(szName);
	m_pSplitter->setChildrenCollapsible(false);
	
	// Spitted vertially on the left
	m_pVertSplitter = new KviTalSplitter(Qt::Vertical,m_pSplitter);
	m_pVertSplitter->setChildrenCollapsible(false);

	// With the IRC view over
	m_pIrcView = new KviIrcView(m_pVertSplitter,lpFrm,this);
	m_pIrcView->setObjectName(szName);
	connect(m_pIrcView,SIGNAL(rightClicked()),this,SLOT(textViewRightClicked()));
	// And the double view (that may be unused)
	m_pMessageView = 0;
	// The userlist on the right
	//m_pEditorsContainer= new KviToolWindowsContainer(m_pSplitter);


	// and the related buttons
	m_pDoubleViewButton = new KviWindowToolPageButton(KVI_SMALLICON_HIDEDOUBLEVIEW,KVI_SMALLICON_SHOWDOUBLEVIEW,__tr2qs("Split View"),buttonContainer(),false,"double_view_button");
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

	if(m_pConsole->connection()->serverInfo()->supportedListModes().contains('q'))
	{
		m_pQuietBanEditorButton =new KviWindowToolPageButton(KVI_SMALLICON_UNBAN,KVI_SMALLICON_BAN,__tr2qs("Quiet Ban Editor"),buttonContainer(),false,"quiet_ban_editor_button");
		connect(m_pQuietBanEditorButton,SIGNAL(clicked()),this,SLOT(toggleQuietBanEditor()));
	} else {
		m_pQuietBanEditorButton = 0;
	}

	m_pModeEditorButton = new KviWindowToolPageButton(KVI_SMALLICON_CHANMODEHIDE,KVI_SMALLICON_CHANMODE,__tr2qs("Mode Editor"),buttonContainer(),false,"mode_editor_button");
	connect(m_pModeEditorButton,SIGNAL(clicked()),this,SLOT(toggleModeEditor()));
	m_pModeEditor = 0;

#ifdef COMPILE_CRYPT_SUPPORT
	createCryptControllerButton(m_pButtonContainer);
#endif

	m_pHideToolsButton = new QToolButton(m_pButtonBox);
	m_pHideToolsButton->setObjectName("hide_container_button");
	m_pHideToolsButton->setAutoRaise(true);
	m_pHideToolsButton->setIconSize(QSize(22,22));
	m_pHideToolsButton->setFixedWidth(16);

	if(g_pIconManager->getBigIcon("kvi_horizontal_left.png"))
		m_pHideToolsButton->setIcon(QIcon(*(g_pIconManager->getBigIcon("kvi_horizontal_left.png"))));

	connect(m_pHideToolsButton,SIGNAL(clicked()),this,SLOT(toggleToolButtons()));

	m_pUserListView = new KviUserListView(m_pSplitter,m_pListViewButton,connection()->userDataBase(),this,AVERAGE_CHANNEL_USERS,__tr2qs("User List"),"user_list_view");
	//m_pEditorsContainer->addWidget(m_pUserListView);
	//m_pEditorsContainer->raiseWidget(m_pUserListView);
	// And finally the input line on the bottom
	m_pInput   = new KviInput(this,m_pUserListView);
	// no mask editors yet
	m_pBanEditor = 0;
	m_pBanExceptionEditor = 0;
	m_pInviteEditor = 0;
	m_pQuietBanEditor = 0;
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
	if(type() == KVI_WINDOW_TYPE_DEADCHANNEL)
	{
		if(context())
			context()->unregisterDeadChannel(this);
	} else {
		if(connection())
			connection()->unregisterChannel(this);
	}
	
	// Then remove all the users and free mem
	m_pUserListView->enableUpdates(false);
	m_pUserListView->partAll();

	delete m_pActionHistory;
	delete m_pBanList;
	delete m_pBanExceptionList;
	delete m_pInviteList;
	delete m_pQuietBanList;
	delete m_pTmpHighLighted;
}

void KviChannel::toggleToolButtons()
{
	if(!buttonContainer())
		return;

	toggleButtonContainer();
	QPixmap * pix= buttonContainer()->isVisible() ?
		g_pIconManager->getBigIcon("kvi_horizontal_left.png") :
		g_pIconManager->getBigIcon("kvi_horizontal_right.png");
	if(pix)
		m_pHideToolsButton->setIcon(QIcon(*pix));
}

void KviChannel::triggerCreationEvents()
{
	KVS_TRIGGER_EVENT_0(KviEvent_OnChannelWindowCreated,this);
}

void KviChannel::textViewRightClicked()
{
	KVS_TRIGGER_EVENT_0(KviEvent_OnChannelPopupRequest,this);
}

void KviChannel::getBaseLogFileName(QString & szBuffer)
{
	QString szChan(windowName());
	szChan.replace(".","%2e");
	if(connection())
	{
		szBuffer = szChan;
		szBuffer.append(".");
		szBuffer.append(connection()->networkName());
	} else {
		szBuffer = szChan;
		szBuffer.append(".");
		if(context())
			szBuffer.append(context()->id());
		else szBuffer.append("0");
	}
}

void KviChannel::applyOptions()
{
	m_pUserListView->applyOptions();
	m_pTopicWidget->applyOptions();

	if(m_pMessageView)
		m_pMessageView->applyOptions();

	m_pModeWidget->applyOptions();

	// this applies options for IrcView and Input and forces the window to relayout
	KviWindow::applyOptions();
}

void KviChannel::getConfigGroupName(QString & szBuffer)
{
	szBuffer = windowName();

//TODO it would be nice to save per-network channel settings, so that the settings of two channels
//with the same name but of different networks gets different config entries.
// 	if(connection())
// 	{
// 		szBuffer.append("@");
// 		szBuffer.append(connection()->networkName());
// 	}
}

void KviChannel::saveProperties(KviConfig * cfg)
{
	KviWindow::saveProperties(cfg);
	cfg->writeEntry("TopSplitter",m_pTopSplitter->sizes());
	QList<int> sizes;
	sizes << m_pIrcView->width() << m_pUserListView->width();
	cfg->writeEntry("Splitter",sizes);
	cfg->writeEntry("VertSplitter",m_pMessageView ? m_pVertSplitter->sizes() : m_VertSplitterSizesList);
	cfg->writeEntry("PrivateBackground",m_privateBackground);
	cfg->writeEntry("DoubleView",m_pMessageView ? true : false);

	if(m_pUserListView)
		cfg->writeEntry("UserListHidden",m_pUserListView->isHidden());
	cfg->writeEntry("ToolButtonsHidden",buttonContainer()->isHidden());
}

void KviChannel::loadProperties(KviConfig * cfg)
{
	int iWidth = width();
	QList<int> def;
	def.append((iWidth * 75) / 100);
	def.append((iWidth * 15) / 100);
	def.append((iWidth * 10) / 100);
	m_pTopSplitter->setSizes(cfg->readIntListEntry("TopSplitter",def));
	def.clear();
	def.append((iWidth * 75) / 100);
	def.append((iWidth * 25) / 100);
	QList<int> sizes=cfg->readIntListEntry("Splitter",def);
	m_pSplitter->setSizes(sizes);
	m_pIrcView->resize(sizes.at(0), m_pIrcView->height());
	m_pUserListView->resize(sizes.at(1), m_pUserListView->height());
	m_pSplitter->setStretchFactor(0,0);
	m_pSplitter->setStretchFactor(0,1);

	def.clear();

	def.append((iWidth * 60) / 100);
	def.append((iWidth * 40) / 100);
	m_VertSplitterSizesList=cfg->readIntListEntry("VertSplitter",def);
	showDoubleView(cfg->readBoolEntry("DoubleView",false));

	m_privateBackground = cfg->readPixmapEntry("PrivateBackground",KviPixmap());
	if(m_privateBackground.pixmap())
	{
		m_pIrcView->setPrivateBackgroundPixmap(*(m_privateBackground.pixmap()));
		if(m_pMessageView)
			m_pMessageView->setPrivateBackgroundPixmap(*(m_privateBackground.pixmap()));
	}

	KviWindow::loadProperties(cfg);
	if(m_pUserListView)
	{
		bool bHidden = cfg->readBoolEntry("UserListHidden",0);
		m_pUserListView->setHidden(bHidden);
		resizeEvent(0);
	}

	if(cfg->readBoolEntry("ToolButtonsHidden",KVI_OPTION_BOOL(KviOption_boolHideWindowToolButtons)) != buttonContainer()->isHidden())
		toggleToolButtons();
}

void KviChannel::showDoubleView(bool bShow)
{
	if(m_pMessageView)
	{
		if(bShow)
			return;

		m_pIrcView->joinMessagesFrom(m_pMessageView);
		m_VertSplitterSizesList=m_pVertSplitter->sizes();

		delete m_pMessageView;
		m_pMessageView = 0;

		if(m_pDoubleViewButton->isChecked())
			m_pDoubleViewButton->setChecked(false);
	} else {
		if(!bShow)
			return;

		m_pMessageView = new KviIrcView(m_pVertSplitter,m_pFrm,this);
		m_pVertSplitter->setSizes(m_VertSplitterSizesList);
		//setFocusHandler(m_pInput,m_pMessageView); //socket it!
		if(!(m_pDoubleViewButton->isChecked()))
			m_pDoubleViewButton->setChecked(true);

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
	if(m_pMessageView)
	{
		showDoubleView(false);
		if(m_pDoubleViewButton->isChecked())m_pDoubleViewButton->setChecked(false);
	} else {
		showDoubleView(true);
		if(!(m_pDoubleViewButton->isChecked()))m_pDoubleViewButton->setChecked(true);
	}
}

void KviChannel::toggleListView()
{
	if(m_pUserListView->isVisible())
	{
		m_pUserListView->hide();
		if(m_pListViewButton->isChecked())m_pListViewButton->setChecked(false);
	} else {
		m_pUserListView->show();
		if(!(m_pListViewButton->isChecked()))m_pListViewButton->setChecked(true);
	}
}

void KviChannel::toggleModeEditor()
{
	if(m_pModeEditor)
	{
		delete m_pModeEditor;
		m_pModeEditor = 0;

		m_pSplitter->setMinimumHeight(20);
		if(m_pModeEditorButton->isChecked())
			m_pModeEditorButton->setChecked(false);
		resizeEvent(0);
	} else {
		m_pModeEditor = new KviModeEditor(m_pSplitter,m_pModeEditorButton,"mode_editor",console(),m_szChannelMode,m_szChannelKey,m_szChannelLimit);
		connect(m_pModeEditor,SIGNAL(setMode(const char *)),this,SLOT(setMode(const char *)));
		connect(m_pModeEditor,SIGNAL(done()),this,SLOT(modeSelectorDone()));
		m_pModeEditor->show();
		//setFocusHandlerNoClass(m_pInput,m_pModeEditor,"QLineEdit");
		if(!m_pModeEditorButton->isChecked())
			m_pModeEditorButton->setChecked(true);
	}
}

void KviChannel::modeSelectorDone()
{
	if(m_pModeEditor)
		toggleModeEditor();
}

void KviChannel::setMode(const char * mode)
{
	if(!connection())
		return;

	QByteArray tmp = connection()->encodeText(m_szName);
	connection()->sendFmtData("MODE %s %s",tmp.data(),mode);
}

void KviChannel::toggleBanEditor()
{
	toggleEditor(&m_pBanEditor,&m_pBanEditorButton,m_pBanList,'b',"ban_editor");
}

void KviChannel::toggleBanExceptionEditor()
{
	toggleEditor(&m_pBanExceptionEditor,&m_pBanExceptionEditorButton,m_pBanExceptionList,'e',"ban_exception_editor");
}

void KviChannel::toggleInviteEditor()
{
	toggleEditor(&m_pInviteEditor,&m_pInviteEditorButton,m_pInviteList,'I',"invite_exception_editor");
}

void KviChannel::toggleQuietBanEditor()
{
	toggleEditor(&m_pQuietBanEditor,&m_pQuietBanEditorButton,m_pQuietBanList,'q',"quiet_ban_editor");
}

void KviChannel::toggleEditor(KviMaskEditor ** ppEd, KviWindowToolPageButton ** ppBtn, KviPointerList<KviMaskEntry> * l, char cFlag, const char * pcEdName)
{
	if(*ppEd)
	{
		delete *ppEd;
		*ppEd = 0;

		if(!(*ppBtn))
			return;
		if((*ppBtn)->isChecked())
			(*ppBtn)->setChecked(false);
	} else {
		bool bHasList = true;
		switch(cFlag)
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
			case 'q':
				if(!(bHasList = hasQuietBanList()))
				{
					m_pQuietBanList->clear();
					setSentQuietBanListRequest();
				}
			break;
		}

		if(!bHasList)
		{
			if(connection())
			{
				QByteArray szName = connection()->encodeText(m_szName);
				connection()->sendFmtData("MODE %s %c",szName.data(),cFlag);
			}
		}

		*ppEd = new KviMaskEditor(m_pSplitter,*ppBtn,l,cFlag,pcEdName);
		connect(*ppEd,SIGNAL(removeMasks(KviMaskEditor *,KviPointerList<KviMaskEntry> *)),this,SLOT(removeMasks(KviMaskEditor *,KviPointerList<KviMaskEntry> *)));
		//setFocusHandler(m_pInput,*ppEd); //socket it!
		(*ppEd)->show();
		if(!(*ppBtn))
			return;
		if(!((*ppBtn)->isChecked()))
			(*ppBtn)->setChecked(true);
	}
}

void KviChannel::removeMasks(KviMaskEditor * ed, KviPointerList<KviMaskEntry> * l)
{
	QString szMasks;
	QString szFlags;
	unsigned int uCount = 0;

	for(KviMaskEntry * e = l->first(); e; e = l->next())
	{
		if(!szMasks.isEmpty())
			szMasks.append(' ');

		szMasks.append(e->szMask);
		szFlags.append(ed->flag());
		uCount++;

		if(uCount == (unsigned int) connection()->serverInfo()->maxModeChanges())
		{
			if(connection())
			{
				QByteArray szName = connection()->encodeText(m_szName);
				connection()->sendFmtData("MODE %s -%s %s",szName.data(),szFlags.toUtf8().data(),connection()->encodeText(szMasks).data());
			}

			szFlags = "";
			szMasks = "";
			uCount = 0;
		}
	}

	if(!szMasks.isEmpty())
	{
		if(connection())
		{
			QByteArray szName = connection()->encodeText(m_szName);
			connection()->sendFmtData("MODE %s -%s %s",szName.data(),szFlags.toUtf8().data(),connection()->encodeText(szMasks).data());
		}
	}
}

QPixmap * KviChannel::myIconPtr()
{
	return g_pIconManager->getSmallIcon((m_iStateFlags & KVI_CHANNEL_STATE_DEADCHAN) ? KVI_SMALLICON_DEADCHANNEL : KVI_SMALLICON_CHANNEL);
}

void KviChannel::resizeEvent(QResizeEvent *)
{
	int iHeight = m_pInput->heightHint();
	int iHeight2 = m_pTopicWidget->sizeHint().height();
	m_pButtonBox->setGeometry(0,0,width(),iHeight2);
	m_pSplitter->setGeometry(0,iHeight2,width(),height() - (iHeight + iHeight2));
	m_pInput->setGeometry(0,height() - iHeight, width(),iHeight);
}

QSize KviChannel::sizeHint() const
{
	QSize ret(m_pSplitter->sizeHint().width(),
		m_pIrcView->sizeHint().height() + m_pInput->heightHint() + m_pButtonBox->sizeHint().height());
	return ret;
}

void KviChannel::setChannelMode(char mode, bool bAdd)
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

void KviChannel::addHighlightedUser(const QString & szNick)
{
	if(!m_pUserListView->findEntry(szNick) || m_pTmpHighLighted->contains(szNick,Qt::CaseInsensitive))
		return;

	m_pTmpHighLighted->append(szNick);
}

void KviChannel::removeHighlightedUser(const QString & szNick)
{
	m_pTmpHighLighted->removeOne(szNick);
}

void KviChannel::getChannelModeString(QString & szBuffer)
{
	szBuffer = m_szChannelMode;
	if(!m_szChannelKey.isEmpty())
		szBuffer.append('k');
	if(!m_szChannelLimit.isEmpty())
		szBuffer.append('l');
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
	m_pQuietBanList->clear();

	if(m_pBanEditor) m_pBanEditor->clear();
	if(m_pBanExceptionEditor) m_pBanExceptionEditor->clear();
	if(m_pInviteEditor) m_pInviteEditor->clear();
	if(m_pQuietBanEditor) m_pQuietBanEditor->clear();

	m_pTopicWidget->reset();


	m_pActionHistory->clear();
	m_uActionHistoryHotActionCount = 0;

	m_szChannelMode = "";
	m_szChannelKey = "";
	m_szChannelLimit = "";

	// this should be moved to irc context!
	if(connection())
		connection()->unregisterChannel(this);
	if(context())
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
	if(context())
		context()->unregisterDeadChannel(this);
	if(connection())
		connection()->registerChannel(this);
	// Update log file name
	if(m_pIrcView->isLogging())
		m_pIrcView->startLogging();

	updateIcon();
	updateCaption();
	m_pTopicWidget->reset(); // reset the topic (fixes bug #20 signaled by Klaus Weidenbach)
}

void KviChannel::getTalkingUsersStats(QString & szBuffer, QStringList & list, bool bPast)
{
	if(list.count() < 1)
		return;

	if(list.count() == 1)
	{
		szBuffer += "<b>";
		szBuffer += list.first();
		szBuffer += "</b>";
		szBuffer += " ";
		szBuffer += bPast ? __tr2qs("said something recently") : __tr2qs("is talking");
	} else if(list.count() == 2)
	{
		szBuffer += "<b>";
		szBuffer += list.first();
		szBuffer += "</b> ";
		szBuffer += __tr2qs("and");
		szBuffer += " <b>";
		list.erase(list.begin());
		szBuffer += list.first();
		szBuffer += "</b> ";
		szBuffer += bPast ? __tr2qs("were talking recently") : __tr2qs("are talking");
	} else {
		szBuffer += "<b>";
		szBuffer += list.first();
		szBuffer += "</b>, <b>";
		list.erase(list.begin());
		szBuffer += list.first();
		if(list.count() == 2)
		{
			szBuffer += "</b> ";
			szBuffer += __tr2qs("and");
			szBuffer += " <b>";
			list.erase(list.begin());
			szBuffer += list.first();
			szBuffer += "</b>";
		} else {
			// (list.count() - 1) is > 1
			szBuffer += "</b> ";
			szBuffer += __tr2qs("and other %1 users").arg(list.count() - 1);
		}
		szBuffer += " ";
		szBuffer += bPast ? __tr2qs("were talking recently") : __tr2qs("are talking");
	}
}

void KviChannel::getWindowListTipText(QString & szBuffer)
{
	static QString szHtmlBold("<b>");
	static QString szHtmlTab("&nbsp;&nbsp;");
	static QString szHtmlBoldEnd("</b> ");
	static QString p5(" (");
	// p6 == p4
	static QString p7(" (");
	static QString p8(": ");
	static QString p9(")");
	static QString p10("<br>");

	static QString szEndOfDoc = "</table></body></html>";
	static QString szEndOfFontBoldRow = END_TABLE_BOLD_ROW;
	static QString szRowStart = "<tr><td>";
	static QString szRowEnd = "</td></tr>";

	szBuffer = "<html>" \
		"<body>" \
		"<table width=\"100%\">"\
		START_TABLE_BOLD_ROW;

	if(m_iStateFlags & KVI_CHANNEL_STATE_DEADCHAN)
	{
		szBuffer += __tr2qs("Dead channel");
		szBuffer += szEndOfFontBoldRow;
		szBuffer += szEndOfDoc;
		return;
	}

	KviUserListViewUserStats s;
	m_pUserListView->userStats(&s);


	szBuffer += m_szPlainTextCaption;
	szBuffer += szEndOfFontBoldRow;

	szBuffer += szRowStart;

	QString op = __tr2qs("operator");
	QString ops = __tr2qs("operators");

	//////////////////////

	szBuffer += szHtmlTab;
	szBuffer += szHtmlBold;

	QString szNum;

	szNum.setNum(s.uActive);
	szBuffer += szNum;

	szBuffer += szHtmlBoldEnd;
	szBuffer += (s.uActive == 1 ? __tr2qs("active user") : __tr2qs("active users"));

	szBuffer += p5;
	szBuffer += szHtmlBold;

	szNum.setNum(s.uActiveOp);

	szBuffer += szNum;
	szBuffer += szHtmlBoldEnd;
	szBuffer += (s.uActiveOp == 1 ? op : ops);

	szBuffer += p9;
/*
	FIXME: What is this supposed to mean?
	szBuffer += "<font size=\"-1\">";
	szBuffer += p7;

	szBuffer += __tr2qs("humanity");

	szBuffer += p8;
	szBuffer += szHtmlBold;

	szNum.setNum(s.iAvgTemperature);

	szBuffer += szNum;
	szBuffer += "</bold>";

	szBuffer += p9;
*/
	szBuffer += p10;
	szBuffer += "</font>";



	//////////////////////

	szBuffer += szHtmlTab;
	szBuffer += szHtmlBold;

	szNum.setNum(s.uHot);
	szBuffer += szNum;

	szBuffer += szHtmlBoldEnd;
	szBuffer += (s.uHot == 1 ? __tr2qs("hot user") : __tr2qs("hot users"));

	szBuffer += p5;
	szBuffer += szHtmlBold;

	szNum.setNum(s.uHotOp);

	szBuffer += szNum;
	szBuffer += szHtmlBoldEnd;
	szBuffer += (s.uHotOp == 1 ? op : ops);

	szBuffer += p9;

	/////////////

	szBuffer += szRowEnd;
	szBuffer += szRowStart;

	///////////////////

	if(s.uIrcOp > 0)
	{
		szBuffer += szHtmlTab;
		szBuffer += szHtmlBold;
		szNum.setNum(s.uIrcOp);
		szBuffer += szNum;
		szBuffer += szHtmlBoldEnd;
		szBuffer += (s.uIrcOp == 1 ? __tr2qs("irc operator") : __tr2qs("irc operators"));
		szBuffer += p10;
	}

	if(s.uChanOwner > 0)
	{
		szBuffer += szHtmlTab;
		szBuffer += szHtmlBold;
		szNum.setNum(s.uChanOwner);
		szBuffer += szNum;
		szBuffer += szHtmlBoldEnd;
		szBuffer += (s.uChanOwner == 1 ? __tr2qs("channel owner") : __tr2qs("channel owners"));
		szBuffer += p10;
	}

	if(s.uChanAdmin > 0)
	{
		szBuffer += szHtmlTab;
		szBuffer += szHtmlBold;
		szNum.setNum(s.uChanAdmin);
		szBuffer += szNum;
		szBuffer += szHtmlBoldEnd;
		szBuffer += (s.uChanAdmin == 1 ? __tr2qs("channel administrator") : __tr2qs("channel administrators"));
		szBuffer += p10;
	}

	if(s.uOp > 0)
	{
		szBuffer += szHtmlTab;
		szBuffer += szHtmlBold;
		szNum.setNum(s.uOp);
		szBuffer += szNum;
		szBuffer += szHtmlBoldEnd;
		szBuffer += (s.uOp == 1 ? op : ops);
		szBuffer += p10;
	}

	if(s.uHalfOp > 0)
	{
		szBuffer += szHtmlTab;
		szBuffer += szHtmlBold;
		szNum.setNum(s.uHalfOp);
		szBuffer += szNum;
		szBuffer += szHtmlBoldEnd;
		szBuffer += (s.uHalfOp == 1 ? __tr2qs("half-operator") : __tr2qs("half-operators"));
		szBuffer += p10;
	}

	if(s.uVoiced > 0)
	{
		szBuffer += szHtmlTab;
		szBuffer += szHtmlBold;
		szNum.setNum(s.uVoiced);
		szBuffer += szNum;
		szBuffer += szHtmlBoldEnd;
		szBuffer += (s.uVoiced == 1 ? __tr2qs("voiced user") : __tr2qs("voiced users"));
		szBuffer += p10;
	}

	if(s.uUserOp > 0)
	{
		szBuffer += szHtmlTab;
		szBuffer += szHtmlBold;
		szNum.setNum(s.uUserOp);
		szBuffer += szNum;
		szBuffer += szHtmlBoldEnd;
		szBuffer += (s.uUserOp == 1 ? __tr2qs("user-operator") : __tr2qs("user-operators"));
		szBuffer += p10;
	}

	szBuffer += szHtmlTab;
	szBuffer += szHtmlBold;
	szNum.setNum(s.uTotal);
	szBuffer += szNum;
	szBuffer += szHtmlBoldEnd;
	szBuffer += (s.uTotal == 1 ? __tr2qs("user total") : __tr2qs("users total"));

	szBuffer += szRowEnd;

	KviChannelActivityStats cas;
	getChannelActivityStats(&cas);

	//FIXME hardcoding styles sucks
	if(cas.lTalkingUsers.count() > 0)
	{
		if((cas.lTalkingUsers.count() < 3) && (cas.lWereTalkingUsers.count() > 0))
		{
			szBuffer += "<tr><td bgcolor=\"#E0E0E0\"><font color=\"#000000\">";
			getTalkingUsersStats(szBuffer,cas.lWereTalkingUsers,true);
			szBuffer += "</font>";
			szBuffer += szRowEnd;
		}
		szBuffer += "<tr><td bgcolor=\"#E0E0E0\"><font color=\"#000000\">";
		getTalkingUsersStats(szBuffer,cas.lTalkingUsers,false);
		szBuffer += "</font>";
		szBuffer += szRowEnd;
	} else {
		if(cas.lWereTalkingUsers.count() > 0)
		{
			szBuffer += "<tr><td bgcolor=\"#E0E0E0\"><font color=\"#000000\">";
			getTalkingUsersStats(szBuffer,cas.lWereTalkingUsers,true);
			szBuffer += "</font>";
			szBuffer += szRowEnd;
		}
	}

	szBuffer += "<tr><td bgcolor=\"#A0A0A0\"><b><font color=\"#000000\">";

	if(cas.dActionsPerMinute < 0.1)
		szBuffer += __tr2qs("No activity");
	else if(cas.dActionsPerMinute < 0.3)
		szBuffer += __tr2qs("Minimal activity");
	else if(cas.dActionsPerMinute < 1.0)
		szBuffer += __tr2qs("Very low activity");
	else if(cas.dActionsPerMinute < 3.0)
		szBuffer += cas.bStatsInaccurate ? __tr2qs("Might be low activity") : __tr2qs("Low activity");
	else if(cas.dActionsPerMinute < 10.0)
		szBuffer += cas.bStatsInaccurate ? __tr2qs("Might be medium activity") : __tr2qs("Medium activity");
	else if(cas.dActionsPerMinute < 30.0)
		szBuffer += cas.bStatsInaccurate ? __tr2qs("Might be high activity") : __tr2qs("High activity");
	else if(cas.dActionsPerMinute < 60.0)
		szBuffer += cas.bStatsInaccurate ? __tr2qs("Might be very high activity") : __tr2qs("Very high activity");
	else
		szBuffer += cas.bStatsInaccurate ? __tr2qs("Might be flooded with messages") : __tr2qs("Flooded with messages");

	if(cas.dActionsPerMinute >= 0.1)
	{
		QString szNum;
		szNum.sprintf(" [%u%% ",cas.uHotActionPercent);
		szBuffer += szNum;
		szBuffer += __tr2qs("human");
		szBuffer += "]";
	}

	szBuffer += "</font></b></td></tr>";

	szBuffer += szEndOfDoc;
}

void KviChannel::fillCaptionBuffers()
{
	if(!connection())
	{
		QString szDead = __tr2qs("[Dead channel]");

		m_szNameWithUserFlag = m_szName;

		m_szPlainTextCaption = m_szName;
		m_szPlainTextCaption += " : ";
		m_szPlainTextCaption += szDead;

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
}

void KviChannel::ownMessage(const QString & szBuffer)
{
	if(!connection())
		return;

	//my full mask as seen by other users
	QString MyFullMask = connection()->userInfo()->nickName() + "!" + connection()->userInfo()->userName() + "@" + connection()->userInfo()->hostName();
	QByteArray szMyFullMask = connection()->encodeText(MyFullMask);
	//target name
	QByteArray szName = connection()->encodeText(windowName());
	//message
	QByteArray szData = encodeText(szBuffer);
	const char * d = szData.data();
	/* max length of a PRIVMSG text. Max buffer length for our send is 512 byte, but we have to
	* remember that the server will prepend to the message our full mask and truncate the resulting
	* at 512 bytes again..
	* So we have:
	* :NickName!~ident@hostname.tld PRIVMSG #channel :text of message(CrLf)
	* NickName!~ident@hostname.tld#channeltext of message
	* 512(rfc) -2(CrLf) -2(:) -3(spaces) -7(PRIVMSG) = 498
	* usable bytes, excluding our full mask and the target name.
	*/
	int maxMsgLen = 498 - szName.length() - szMyFullMask.length();

	// our copy of the message
	QString szTmpBuffer(szBuffer);

	if(!d)
		return;

#ifdef COMPILE_CRYPT_SUPPORT
	if(cryptSessionInfo())
	{
		if(cryptSessionInfo()->m_bDoEncrypt)
		{
			if(*d != KVI_TEXT_CRYPTESCAPE)
			{
				KviStr encrypted;
				cryptSessionInfo()->m_pEngine->setMaxEncryptLen(maxMsgLen);
				switch(cryptSessionInfo()->m_pEngine->encrypt(d,encrypted))
				{
					case KviCryptEngine::Encrypted:
						if(!connection()->sendFmtData("PRIVMSG %s :%s",szName.data(),encrypted.ptr()))
							return;
						m_pConsole->outputPrivmsg(this,KVI_OUT_OWNPRIVMSGCRYPTED,
								QString(),QString(),QString(),szBuffer,KviConsole::NoNotifications);
					break;
					case KviCryptEngine::Encoded:
					{
						if(!connection()->sendFmtData("PRIVMSG %s :%s",szName.data(),encrypted.ptr()))
							return;
						// ugly ,but we must redecode here
						QString szRedecoded = decodeText(encrypted.ptr());
						m_pConsole->outputPrivmsg(this,KVI_OUT_OWNPRIVMSG,
							QString(),QString(),QString(),szRedecoded,KviConsole::NoNotifications);
					}
					break;
					default: // also case KviCryptEngine::EncryptError
					{
						QString szEngineError = cryptSessionInfo()->m_pEngine->lastError();
						output(KVI_OUT_SYSTEMERROR,
							__tr2qs("The crypto engine was unable to encrypt the current message (%Q): %Q, no data sent to the server"),
							&szBuffer,&szEngineError);
					}
					break;
				}
				userAction(connection()->currentNickName(),KVI_USERACTION_PRIVMSG);
				return;
			} else {
				//eat the escape code
				d++;
				szTmpBuffer.remove(0,1);
				//let the normal function do it
			}
		}
	}
#endif

	if(szData.length() > maxMsgLen)
	{
		/* Multi message; we want to split the message, preferably on a word boundary,
		 * and send each message part as a different PRIVMSG
		 * Due to encoding stuff, this is frikin'time eater
		 */
		QTextEncoder * p_Encoder = makeEncoder(); // our temp encoder
		QByteArray szTmp;		// used to calculate the length of an encoded message
		int iPos;			// contains the index where to truncate szTmpBuffer
		int iC;				// cycles counter (debugging/profiling purpose)
		float fPosDiff;			// optimization internal; aggressivity factor
		QString szCurSubString;		// truncated parts as reported to the user

		// run until we've something remaining in the message
		while(szTmpBuffer.length())
		{
			// init counters
			iPos = szTmpBuffer.length();
			iC = 0;

			// first part (optimization): quickly find an high index that is _surely_lesser_
			// than the correct one
			while(1)
			{
				iC++;
				szTmp = p_Encoder->fromUnicode(szTmpBuffer.left(iPos));
				if(szTmp.length() <= maxMsgLen)
					break;
				//if szTmp.length() == 0 we already have break'ed out from here,
				// so we can safely use it as a divisor
				fPosDiff = (float)maxMsgLen / szTmp.length();
				iPos = (int)(iPos*fPosDiff); // cut the string at each cycle
				//printf("OPTIMIZATION: fPosDiff %f, iPos %d\n", fPosDiff, iPos);
			}
			//printf("Multi message: %d optimization cyles", iC);

			// now, do it the simple way: increment our index until we perfectly fit into the
			// available space
			while(1)
			{
				iC++;

				szTmp = p_Encoder->fromUnicode(szTmpBuffer.left(iPos));

				// perfect match
				if(iPos == szTmpBuffer.length()) break;

				if(szTmp.length() > maxMsgLen)
				{
					// overflowed.. last one was the good one
					iPos--;
					szTmp = p_Encoder->fromUnicode(szTmpBuffer.left(iPos));
					break;
				} else {
					//there's still free space.. add another char
					iPos++;
				}

			}
			//printf(", finished at %d cycles, truncated at pos %d\n", iC, iPos);

			//prepare the feedback string for the user
			szCurSubString=szTmpBuffer.left(iPos);

			//send the string to the server
			if(connection()->sendFmtData("PRIVMSG %s :%s",szName.data(),szTmp.data()))
			{
				//feeedback the user
				m_pConsole->outputPrivmsg(this,KVI_OUT_OWNPRIVMSG,QString(),QString(),QString(),szCurSubString,KviConsole::NoNotifications);
				userAction(connection()->currentNickName(),KVI_USERACTION_PRIVMSG);
			} else {
				// skipped a part in this multi message.. we don't want to continue
				return;
			}

			// remove the sent part of the string
			szTmpBuffer.remove(0, iPos);
			//printf("Sent %d chars, %d remaining in the Qstring\n",iPos, szTmpBuffer.length());
		}

	} else {
		if(connection()->sendFmtData("PRIVMSG %s :%s",szName.data(),d))
		{
			m_pConsole->outputPrivmsg(this,KVI_OUT_OWNPRIVMSG,QString(),QString(),QString(),szTmpBuffer,KviConsole::NoNotifications);
			userAction(connection()->currentNickName(),KVI_USERACTION_PRIVMSG);
		}
	}
}

void KviChannel::ownAction(const QString & szBuffer)
{
	if(!connection())
		return;
        QString szTmpBuffer;

	//see bug ticket #220
	if(KVI_OPTION_BOOL(KviOption_boolStripMircColorsInUserMessages))
	{
		szTmpBuffer = KviMircCntrl::stripControlBytes(szBuffer);
	} else {
		szTmpBuffer = szBuffer;
	}

	QString szName = m_szName;
	QByteArray szData = encodeText(szTmpBuffer);
	const char * d = szData.data();

	if(!d)
		return;
	if(KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnMeAction,this,szTmpBuffer,szName))
		return;
	if(!connection()->sendFmtData("PRIVMSG %s :%cACTION %s%c",connection()->encodeText(szName).data(),0x01,d,0x01))
		return;

	QString szBuf = "\r!nc\r";
	szBuf += connection()->currentNickName();
	szBuf += "\r ";
	szBuf += szTmpBuffer;
	outputMessage(KVI_OUT_ACTION,szBuf);
	userAction(connection()->currentNickName(),KVI_USERACTION_ACTION);
}

bool KviChannel::nickChange(const QString & szOldNick, const QString & szNewNick)
{
	bool bWasHere = m_pUserListView->nickChange(szOldNick,szNewNick);
	if(bWasHere)
		channelAction(szNewNick,KVI_USERACTION_NICK,kvi_getUserActionTemperature(KVI_USERACTION_NICK));
	return bWasHere;
}

bool KviChannel::part(const QString & szNick)
{
	bool bWasHere = m_pUserListView->part(szNick);
	if(bWasHere)
		channelAction(szNick,KVI_USERACTION_PART,kvi_getUserActionTemperature(KVI_USERACTION_PART));
	return bWasHere;
}

bool KviChannel::activityMeter(unsigned int * puActivityValue, unsigned int * puActivityTemperature)
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


	if(dActionsPerMinute < 0.3)
		*puActivityValue = KVI_ACTIVITY_NONE;
	else if(dActionsPerMinute < 1.0)
		*puActivityValue = KVI_ACTIVITY_VERYLOW;
	else if(dActionsPerMinute < 4.0)
		*puActivityValue = KVI_ACTIVITY_LOW;
	else if(dActionsPerMinute < 10.0)
		*puActivityValue = KVI_ACTIVITY_MEDIUM;
	else if(dActionsPerMinute < 30.0)
		*puActivityValue = KVI_ACTIVITY_HIGH;
	else
		*puActivityValue = KVI_ACTIVITY_VERYHIGH;

	if(uHotActionPercent < KVI_CHANACTIVITY_LIMIT_ICE)
		*puActivityTemperature = KVI_ACTIVITY_ICE;
	else if(uHotActionPercent < KVI_CHANACTIVITY_LIMIT_VERYCOLD)
		*puActivityTemperature = KVI_ACTIVITY_VERYCOLD;
	else if(uHotActionPercent < KVI_CHANACTIVITY_LIMIT_COLD)
		*puActivityTemperature = KVI_ACTIVITY_COLD;
	else if(uHotActionPercent < KVI_CHANACTIVITY_LIMIT_UNDEFINED)
		*puActivityTemperature = KVI_ACTIVITY_UNDEFINED;
	else if(uHotActionPercent < KVI_CHANACTIVITY_LIMIT_HOT)
		*puActivityTemperature = KVI_ACTIVITY_HOT;
	else if(uHotActionPercent < KVI_CHANACTIVITY_LIMIT_VERYHOT)
		*puActivityTemperature = KVI_ACTIVITY_VERYHOT;
	else
		*puActivityTemperature = KVI_ACTIVITY_FIRE;

	return true;
}

void KviChannel::channelAction(const QString & szNick, unsigned int uActionType, int iTemperature)
{
	KviChannelAction * a = new KviChannelAction;
	a->tTime = kvi_unixTime();
	a->uActionType = uActionType;
	a->iTemperature = iTemperature;
	a->szNick = szNick;

	if(iTemperature > 0)
		m_uActionHistoryHotActionCount++;

	m_pActionHistory->append(a);
	fixActionHistory();
}

void KviChannel::fixActionHistory()
{
	while(m_pActionHistory->count() > KVI_CHANNEL_ACTION_HISTORY_MAX_COUNT)
		m_pActionHistory->removeFirst();

	KviChannelAction * a = m_pActionHistory->last();
	if(!a)
		return;

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
	if(!m_pMessageView)
		return;
	if(m_pMessageView->hasLineMark())
		m_pMessageView->clearLineMark(true);
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

	int iFake;
	s->lTalkingUsers.clear();
	s->lWereTalkingUsers.clear();

	for(a = m_pActionHistory->last(); a; a = m_pActionHistory->prev())
	{
		if(a->tTime >= tNow)
			s->uActionsInTheLastMinute++;

		if(a->iTemperature > 0)
			s->uHotActionCount++;

		s->iAverageActionTemperature += a->iTemperature;

		if((a->uActionType == KVI_USERACTION_PRIVMSG) ||
			(a->uActionType == KVI_USERACTION_NOTICE) ||
			(a->uActionType == KVI_USERACTION_ACTION))
		{
			if(!userDict.find(a->szNick))
			{
				if(isOn(a->szNick.toAscii()))
				{
					if(a->tTime >= tTwoMinsAgo)
						s->lTalkingUsers.append(a->szNick);
					else
						s->lWereTalkingUsers.append(a->szNick);

					userDict.insert(a->szNick,&iFake);
				}
			}
		}
	}

	s->iAverageActionTemperature = s->iAverageActionTemperature / (int)s->uActionCount;

	s->uHotActionPercent = (s->uHotActionCount * 100) / s->uActionCount;
}

void KviChannel::userAction(const QString & szNick, const QString & szUser, const QString & szHost, unsigned int uActionType)
{
	int iTemperature = kvi_getUserActionTemperature(uActionType);
	channelAction(szNick,uActionType,iTemperature);
	m_pUserListView->userAction(szNick,szUser,szHost,iTemperature);
}

void KviChannel::userAction(const QString & szNick, unsigned int uActionType)
{
	int iTemperature = kvi_getUserActionTemperature(uActionType);
	channelAction(szNick,uActionType,iTemperature);
	m_pUserListView->userAction(szNick,iTemperature);
}

void KviChannel::userAction(KviIrcMask * user, unsigned int uActionType)
{
	int iTemperature = kvi_getUserActionTemperature(uActionType);
	channelAction(user->nick(),uActionType,iTemperature);
	m_pUserListView->userAction(user,iTemperature);
}

void KviChannel::topicSelected(const QString & szTopic)
{
	if(!connection())
		return;

	QByteArray szEncoded = encodeText(szTopic);
	QByteArray szName = connection()->encodeText(m_szName);
	connection()->sendFmtData("TOPIC %s :%s",szName.data(),szEncoded.length() ? szEncoded.data() : "");
}

void KviChannel::closeEvent(QCloseEvent * e)
{
	if((m_iStateFlags & KVI_CHANNEL_STATE_SENTPART) || (m_iStateFlags & KVI_CHANNEL_STATE_DEADCHAN) || !(m_pConsole->isConnected()))
	{
		if(context()) context()->unregisterDeadChannel(this);
		KviWindow::closeEvent(e);
	} else {
		e->ignore();
		// FIXME: #warning "THIS PART SHOULD BECOME A COMMAND /PART $option()..so the identifiers are parsed"
		if(connection())
		{
			QString szTmp = KVI_OPTION_STRING(KviOption_stringPartMessage);
			szTmp.replace(";","\\;");
			szTmp.replace("\n"," ");
			KviKvsVariant vRet;

			if(KviKvsScript::evaluate(szTmp,this,0,&vRet))
				vRet.asString(szTmp);

			QByteArray dat = encodeText(szTmp);
			partMessageSent();
			QByteArray szName = connection()->encodeText(m_szName);
			connection()->sendFmtData("PART %s :%s",szName.data(),dat.data() ? dat.data() : "");
			// be sure to not reference ourselves here.. we could be disconnected!
		} else {
			partMessageSent(); // huh ?
		}
	}
}

void KviChannel::partMessageSent(bool bCloseOnPart, bool bShowMessage)
{
	m_iStateFlags |= KVI_CHANNEL_STATE_SENTPART;
	if(!bCloseOnPart)
		m_iStateFlags |= KVI_CHANNEL_STATE_NOCLOSEONPART;
	if(bShowMessage)
		outputNoFmt(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Sent part request, waiting for reply..."));
}

#define IS_FNC(__name,__ulvname) \
bool KviChannel::__name(bool bAtLeast) \
{ \
	if(!connection()) \
		return false; \
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
	if(!connection())
		return 0;

	return m_pUserListView->flags(connection()->currentNickName());
}

void KviChannel::setMask(char cFlag, const QString & szMask, bool bAdd, const QString & szSetBy, unsigned int uSetAt)
{
	if(!connection())
		return;

	KviPointerList<KviMaskEntry> * list = m_pBanList;
	KviMaskEditor * editor = m_pBanEditor;
	switch(cFlag)
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
		case 'q':
			m_iStateFlags ^= KVI_CHANNEL_STATE_HAVEQUIETBANLIST;
			list = m_pQuietBanList;
			editor = m_pQuietBanEditor;
			break;
	}

	internalMask(szMask,bAdd,szSetBy,uSetAt,list,&editor);
	m_pUserListView->setMaskEntries(cFlag,(int)list->count());
}

void KviChannel::internalMask(const QString & szMask, bool bAdd, const QString & szSetBy, unsigned int uSetAt, KviPointerList<KviMaskEntry> * l, KviMaskEditor ** ppEd)
{
	KviMaskEntry * e = 0;
	if(bAdd)
	{
		for(e = l->first(); e; e = l->next())
		{
			if(KviQString::equalCI(e->szMask,szMask))
				return; //already there
		}
		e = new KviMaskEntry;
		e->szMask = szMask;
		e->szSetBy = (!szSetBy.isEmpty()) ? szSetBy : __tr2qs("(Unknown)");
		e->uSetAt = uSetAt;
		l->append(e);
		if(*ppEd)
			(*ppEd)->addMask(e);
	} else {
		for(e = l->first(); e; e = l->next())
		{
			if(KviQString::equalCI(e->szMask,szMask))
				break;
		}

		if(e)
		{
			if(*ppEd)
				(*ppEd)->removeMask(e);
			l->removeRef(e);
		}
	}
}

void KviChannel::updateModeLabel()
{
	QString szTmp = m_szChannelMode;
	QString szTip = __tr2qs("<b>Channel mode:</b>");
	//const char * aux = m_szChannelMode.toUtf8().data(); leaks memory and will not work with getChannelModeDescription() (can channel modes be multibyte ?)
	KviStr szMod = m_szChannelMode;
	const char * pcAux = szMod.ptr();
	while(*pcAux)
	{
		KviQString::appendFormatted(szTip,"<br>%c: %Q",*pcAux,&(m_pConsole->connection()->serverInfo()->getChannelModeDescription(*pcAux)));
		++pcAux;
	}

	if(!m_szChannelKey.isEmpty())
	{
		if(!szTmp.isEmpty())
			szTmp.append(' ');

		KviQString::appendFormatted(szTmp,"k:%s",m_szChannelKey.toUtf8().data());
		KviQString::appendFormatted(szTip,__tr2qs("<br><b>Key:</b> %s"),m_szChannelKey.toUtf8().data());
	}

	if(!m_szChannelLimit.isEmpty())
	{
		if(!szTmp.isEmpty())
			szTmp.append(' ');

		KviQString::appendFormatted(szTmp,"l:%s",m_szChannelLimit.toUtf8().data());
		KviQString::appendFormatted(szTip,__tr2qs("<br><b>Limit:</b> %s"),m_szChannelLimit.toUtf8().data());
	}

	m_pModeWidget->refreshModes();
	KviTalToolTip::remove(m_pModeWidget);
	KviTalToolTip::add(m_pModeWidget,szTip);
}

void KviChannel::outputMessage(int iMsgType, const QString & szMsg)
{
	QString szBuf(szMsg);
	preprocessMessage(szBuf);

	const QChar * pC = KviQString::nullTerminatedArray(szBuf);
	if(!pC)
		return;

	internalOutput(m_pMessageView ? m_pMessageView : m_pIrcView,iMsgType,(const kvi_wchar_t *)pC);
}

void KviChannel::checkChannelSync()
{
	if(m_iStateFlags & KVI_CHANNEL_STATE_SYNCHRONIZED)
		return;

	if(m_iStateFlags & KVI_CHANNEL_STATE_SENTWHOREQUEST)
	{
		if(!(m_iStateFlags & KVI_CHANNEL_STATE_HAVEWHOLIST))
			return;
	}

	if(m_iStateFlags & KVI_CHANNEL_STATE_SENTBANLISTREQUEST)
	{
		if(!(m_iStateFlags & KVI_CHANNEL_STATE_HAVEBANLIST))
			return;
	}

	if(m_iStateFlags & KVI_CHANNEL_STATE_SENTBANEXCEPTIONLISTREQUEST)
	{
		if(!(m_iStateFlags & KVI_CHANNEL_STATE_HAVEBANEXCEPTIONLIST))
			return;
	}

	if(m_iStateFlags & KVI_CHANNEL_STATE_SENTINVITELISTREQUEST)
	{
		if(!(m_iStateFlags & KVI_CHANNEL_STATE_HAVEINVITELIST))
			return;
	}

	if(m_iStateFlags & KVI_CHANNEL_STATE_SENTQUIETBANLISTREQUEST)
	{
		if(!(m_iStateFlags & KVI_CHANNEL_STATE_HAVEQUIETBANLIST))
			return;
	}

	m_iStateFlags |= KVI_CHANNEL_STATE_SYNCHRONIZED;
	// we already have all the spontaneous server replies
	// (so probably mode, topic (or no topic is set),names)
	// we have already received the I and e lists (if requested)
	kvs_int_t iSyncTime = m_joinTime.time().msecsTo(QTime::currentTime());
	if(iSyncTime < 0)
		iSyncTime += 86400000;

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
	QStringList strings = szMessage.split(" ",QString::KeepEmptyParts);
	for(QStringList::Iterator it = strings.begin(); it != strings.end(); ++it)
	{
		if((*it).contains('\r'))
			continue;

		QString szTmp = KviMircCntrl::stripControlBytes(*it);
		if(findEntry(*it))
			*it = QString("\r!n\r%1\r").arg(*it);
		if(m_pConsole)
		{
			if(m_pConsole->connection())
			{
				if(m_pConsole->connection()->serverInfo()->supportedChannelTypes().contains(szTmp[0]))
				{
					if((*it) == szTmp)
						*it = QString("\r!c\r%1\r").arg(*it);
					else
						*it = QString("\r!c%1\r%2\r").arg(szTmp, *it);
				}
			}
		}
	}
	szMessage = strings.join(" ");
}

void KviChannel::unhighlight()
{
	if(!m_pWindowListItem)
		return;
	m_pWindowListItem->unhighlight();
}

void KviChannel::pasteLastLog()
{
	QString szChannel = target().toLower();
	QString szNetwork = console()->currentNetworkName().toLower();
	QDate date = QDate::currentDate();

	// Create the filter for the dir
	// Format: channel_<channel>.<network>_*.*.*.log*
	QString szLogFilter = "channel_";
	szLogFilter += szChannel;
	szLogFilter += ".";
	szLogFilter += szNetwork;
	szLogFilter += "_*.*.*.log*";

	// Get the logs
	QString szLogPath;
	g_pApp->getLocalKvircDirectory(szLogPath,KviApp::Log);
	QDir logDir(szLogPath);
	QStringList filter = QStringList(szLogFilter);
	QStringList logList = logDir.entryList(filter,QDir::Files,QDir::Name | QDir::Reversed);

	// Scan log files
	// Format: channel_#channelName.networkName_year.month.day.log
	// Format: channel_#channelName.networkName_year.month.day.log.gz
	bool bGzip;
	QString szFileName;

	for(QStringList::Iterator it = logList.begin(); it != logList.end(); ++it)
	{
		int iLogYear, iLogMonth, iLogDay;

		szFileName = (*it);
		QString szTmpName = (*it);
		QFileInfo fi(szTmpName);
		bGzip = false;

		// Skip the log just created on join
		if(fi.suffix() == "tmp")
			continue;

		// Remove trailing dot and extension .gz
		if(fi.suffix() == "gz")
		{
			bGzip = true;
			szTmpName.chop(3);
		}

		// Ok, we have the right channel/network log. Get date
		QString szLogDate = szTmpName.section('.',-4,-1).section('_',1,1);
		iLogYear = szLogDate.section('.',0,0).toInt();
		iLogMonth = szLogDate.section('.',1,1).toInt();
		iLogDay = szLogDate.section('.',2,2).toInt();

		// Check log validity
		int iInterval = -(int)KVI_OPTION_UINT(KviOption_uintDaysIntervalToPasteOnChannelJoin);
		QDate logDate(iLogYear,iLogMonth,iLogDay);
		QDate checkDate = date.addDays(iInterval);

		if(logDate < checkDate)
			return;
		else
			break;
	}

	// Get the right log name
	szFileName.prepend("/");
	szFileName.prepend(szLogPath);

	// Load the log
	QByteArray log = loadLogFile(szFileName,bGzip);
	if(log.size() > 0)
	{
		QList<QByteArray> list = log.split('\n');
		unsigned int uCount = list.size();
		unsigned int uLines = KVI_OPTION_UINT(KviOption_uintLinesToPasteOnChannelJoin);
		unsigned int uStart = uCount - uLines - 1;

/*
		// Check if the log is smaller than the lines to print
		if(uStart < 0)
			uStart = 0;
*/
		QString szDummy = __tr2qs("Starting last log");
		outputMessage(KVI_OUT_CHANPRIVMSG,szDummy);

		// Scan the log file
		for(unsigned int i = uStart; i < uCount; i++)
		{
			QString szLine = QString(list.at(i));
			szLine = szLine.section(' ',1);
#ifdef COMPILE_ON_WINDOWS
			// Remove the \r char at the szEnd of line
			szLine.chop(1);
#endif
			// Print the line in the channel buffer
			outputMessage(KVI_OUT_CHANPRIVMSG,szLine);
		}

		szDummy = __tr2qs("End of log");
		outputMessage(KVI_OUT_CHANPRIVMSG,szDummy);
	}
}

QByteArray KviChannel::loadLogFile(const QString & szFileName, bool bGzip)
{
	QByteArray data;

#ifdef COMPILE_ZLIB_SUPPORT
	if(bGzip)
	{
		gzFile logFile = gzopen(szFileName.toLocal8Bit().data(),"rb");
		if(logFile)
		{
			char cBuff[1025];
			int iLen;

			iLen = gzread(logFile,cBuff,1024);
			while(iLen > 0)
			{
				cBuff[iLen] = 0;
				data.append(cBuff);
				iLen = gzread(logFile,cBuff,1024);
			}

			gzclose(logFile);
		} else {
			debug("Cannot open compressed file %s",szFileName.toUtf8().data());
		}

	} else {
#endif
		QFile logFile(szFileName);
		if(!logFile.open(QIODevice::ReadOnly))
			return QByteArray();

		data = logFile.readAll();
		logFile.close();
#ifdef COMPILE_ZLIB_SUPPORT
	}
#endif

	return data;
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "kvi_channel.moc"
#endif
