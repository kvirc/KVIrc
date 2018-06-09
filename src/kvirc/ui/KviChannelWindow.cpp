//=============================================================================
//
//   File : KviChannelWindow.cpp
//   Creation date : Tue Aug  1 2000 02:20:22 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_defaults.h"
#include "kvi_out.h"
#include "KviChannelWindow.h"
#include "KviConsoleWindow.h"
#include "KviIrcNetwork.h"
#include "KviIconManager.h"
#include "KviIrcView.h"
#include "KviInput.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "KviTopicWidget.h"
#include "KviIrcSocket.h"
#include "KviMemory.h"
#include "KviWindowListBase.h"
#include "KviMainWindow.h"
#include "KviConfigurationFile.h"
#include "KviMaskEditor.h"
#include "KviControlCodes.h"
#include "KviModeEditor.h"
#include "KviApplication.h"
#include "KviUserAction.h"
#include "KviWindowToolWidget.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviIrcConnectionServerInfo.h"
#include "KviIrcConnectionRequestQueue.h"
#include "KviIrcServerParser.h"
#include "KviModeWidget.h"
#include "KviKvsScript.h"
#include "KviKvsEventTriggers.h"

#ifdef COMPILE_CRYPT_SUPPORT
#include "KviCryptEngine.h"
#include "KviCryptController.h"
#endif //COMPILE_CRYPT_SUPPORT

#include <set>
#include <ctime>

#include <QDate>
#include <QByteArray>
#include <QLabel>
#include <QEvent>
#include <QPalette>
#include <QMessageBox>
#include <QCloseEvent>
#include <QMenu>

// FIXME: #warning "+a Anonymous channel mode!"
// FIXME: #warning "OnChannelFlood event...."

KviChannelWindow::KviChannelWindow(KviConsoleWindow * lpConsole, const QString & szName)
    : KviWindow(KviWindow::Channel, szName, lpConsole)
{
	// Init some member variables
	m_pInput = nullptr;
	m_iStateFlags = 0;
	m_uActionHistoryHotActionCount = 0;

	m_pTmpHighLighted = new QStringList();

	// Register ourselves
	connection()->registerChannel(this);
	// And create the widgets layout
	// Button box
	m_pButtonBox = new KviTalHBox(this);
	m_pButtonBox->setSpacing(0);
	m_pButtonBox->setMargin(0);

	m_pTopSplitter = new KviTalSplitter(Qt::Horizontal, m_pButtonBox);
	m_pTopSplitter->setChildrenCollapsible(false);

	m_pButtonContainer = new KviTalHBox(m_pButtonBox);
	m_pButtonContainer->setSpacing(0);
	m_pButtonContainer->setMargin(0);
	// Topic widget on the left
	m_pTopicWidget = new KviTopicWidget(m_pTopSplitter, this, "topic_widget");

	connect(m_pTopicWidget, SIGNAL(topicSelected(const QString &)),
	    this, SLOT(topicSelected(const QString &)));
	// mode label follows the topic widget
	m_pModeWidget = new KviModeWidget(m_pTopSplitter, *this, "mode_");
	KviTalToolTip::add(m_pModeWidget, __tr2qs("Channel modes"));
	connect(m_pModeWidget, SIGNAL(setMode(const QString &)), this, SLOT(setMode(const QString &)));

	createTextEncodingButton(m_pButtonContainer);

	// Central splitter
	m_pSplitter = new KviTalSplitter(Qt::Horizontal, this);
	m_pSplitter->setObjectName(szName);
	m_pSplitter->setChildrenCollapsible(false);

	// Spitted vertially on the left
	m_pVertSplitter = new KviTalSplitter(Qt::Vertical, m_pSplitter);
	m_pVertSplitter->setChildrenCollapsible(false);

	QSizePolicy oPolicy = m_pVertSplitter->sizePolicy();
	oPolicy.setHorizontalPolicy(QSizePolicy::Expanding);
	m_pVertSplitter->setSizePolicy(oPolicy);

	// With the IRC view over
	m_pIrcView = new KviIrcView(m_pVertSplitter, this);
	m_pIrcView->setObjectName(szName);
	connect(m_pIrcView, SIGNAL(rightClicked()), this, SLOT(textViewRightClicked()));
	// And the double view (that may be unused)
	m_pMessageView = nullptr;
	// The userlist on the right
	//m_pEditorsContainer= new KviToolWindowsContainer(m_pSplitter);

	// and the related buttons
	m_pDoubleViewButton = new KviWindowToolPageButton(KviIconManager::HideDoubleView, KviIconManager::ShowDoubleView, __tr2qs("Split view"), buttonContainer(), false);
	m_pDoubleViewButton->setObjectName("double_view_button");
	connect(m_pDoubleViewButton, SIGNAL(clicked()), this, SLOT(toggleDoubleView()));

	m_pListViewButton = new KviWindowToolPageButton(KviIconManager::HideListView, KviIconManager::ShowListView, __tr2qs("User list"), buttonContainer(), true);
	m_pListViewButton->setObjectName("list_view_button");
	connect(m_pListViewButton, SIGNAL(clicked()), this, SLOT(toggleListView()));

	//list modes (bans, bans exceptions, etc)
	KviWindowToolPageButton * pButton = nullptr;
	char cMode = 0;
	QString szDescription = "";
	KviIrcConnectionServerInfo * pServerInfo = serverInfo();
	// bans are hardcoded
	cMode = 'b';

	if(pServerInfo)
		szDescription = pServerInfo->getChannelModeDescription(cMode);
	if(szDescription.isEmpty())
		szDescription = __tr2qs("Mode \"%1\" masks").arg(cMode);

	pButton = new KviWindowToolPageButton(KviIconManager::UnBan, KviIconManager::Ban, szDescription, buttonContainer(), false);
	pButton->setObjectName("ban_editor_button");
	connect(pButton, SIGNAL(clicked()), this, SLOT(toggleListModeEditor()));
	m_ListEditorButtons.emplace(cMode, pButton);

	//other list modes (dynamic)
	QString szListModes = "";
	if(pServerInfo)
	{
		szListModes = pServerInfo->supportedListModes();
		szListModes.remove('b');

		for(auto szListMode : szListModes)
		{
			char cMode = szListMode.unicode();
			QString szDescription = pServerInfo->getChannelModeDescription(cMode);
			if(szDescription.isEmpty())
				szDescription = __tr2qs("Mode \"%1\" masks").arg(cMode);
			KviIconManager::SmallIcon eIconOn, eIconOff;
			switch(cMode)
			{
				// must fix on/off icons here eventually once Ive patience to figure it out.
				// to Un<somethings> is to undo/turn off :p blimey O'reilly.
				case 'e':
					eIconOn = KviIconManager::BanUnExcept;
					eIconOff = KviIconManager::BanExcept;
					break;
				case 'I':
					eIconOn = KviIconManager::InviteUnExcept;
					eIconOff = KviIconManager::InviteExcept;
					break;
				case 'a':
					eIconOn = KviIconManager::ChanUnAdmin;
					eIconOff = KviIconManager::ChanAdmin;
					break;
				case 'q':
					// this could also be quiet bans..
					// on/off are inverted here on purpose because its broken somewhere, must fix eventually.
					eIconOn = KviIconManager::KickOff;
					eIconOff = KviIconManager::Kick;
					break;
				default:
					eIconOn = KviIconManager::UnBan;
					eIconOff = KviIconManager::Ban;
					break;
			}

			pButton = new KviWindowToolPageButton(eIconOn, eIconOff, szDescription, buttonContainer(), false);
			pButton->setObjectName("list_mode_editor_button");
			connect(pButton, SIGNAL(clicked()), this, SLOT(toggleListModeEditor()));
			m_ListEditorButtons.emplace(cMode, pButton);
		}
	}

	m_pModeEditorButton = new KviWindowToolPageButton(KviIconManager::ChanModeHide, KviIconManager::ChanMode, __tr2qs("Mode editor"), buttonContainer(), false);
	m_pModeEditorButton->setObjectName("mode_editor_button");
	connect(m_pModeEditorButton, SIGNAL(clicked()), this, SLOT(toggleModeEditor()));
	m_pModeEditor = nullptr;

#ifdef COMPILE_CRYPT_SUPPORT
	createCryptControllerButton(m_pButtonContainer);
#endif

	m_pHideToolsButton = new QToolButton(m_pButtonBox);
	m_pHideToolsButton->setObjectName("hide_container_button");
	m_pHideToolsButton->setAutoRaise(true);
	m_pHideToolsButton->setIconSize(QSize(22, 22));
	m_pHideToolsButton->setFixedWidth(16);

	if(g_pIconManager->getBigIcon("kvi_horizontal_left.png"))
		m_pHideToolsButton->setIcon(QIcon(*(g_pIconManager->getBigIcon("kvi_horizontal_left.png"))));

	connect(m_pHideToolsButton, SIGNAL(clicked()), this, SLOT(toggleToolButtons()));

	m_pUserListView = new KviUserListView(m_pSplitter, m_pListViewButton, connection()->userDataBase(), this, KVI_CHANNEL_AVERAGE_USERS, __tr2qs("User list"), "user_list_view");
	//m_pEditorsContainer->addWidget(m_pUserListView);
	//m_pEditorsContainer->raiseWidget(m_pUserListView);
	// And finally the input line on the bottom
	m_pInput = new KviInput(this, m_pUserListView);

	// Ensure proper focusing
	//setFocusHandler(m_pInput,this);
	// And turn on the secondary IRC view if needed

	if(KVI_OPTION_BOOL(KviOption_boolPasteLastLogOnChannelJoin))
		pasteLastLog();

	if(KVI_OPTION_BOOL(KviOption_boolAutoLogChannels))
		m_pIrcView->startLogging();

	applyOptions();
	m_joinTime = QDateTime::currentDateTime();
	m_tLastReceivedWhoReply = (kvi_time_t)m_joinTime.toTime_t();
}

KviChannelWindow::~KviChannelWindow()
{
	// Unregister ourself
	if(type() == KviWindow::DeadChannel && context())
		context()->unregisterDeadChannel(this);
	else if(connection())
		connection()->unregisterChannel(this);

	// Then remove all the users and free mem
	m_pUserListView->enableUpdates(false);
	m_pUserListView->partAll();

	delete m_pTmpHighLighted;

	for(auto i : m_ListEditors)
		delete i.second;

	for(auto i : m_ListEditorButtons)
		delete i.second;

	for(auto i : m_ModeLists)
		for(auto ii : i.second)
			delete ii;

	qDeleteAll(m_lActionHistory);
}

void KviChannelWindow::toggleToolButtons()
{
	if(!buttonContainer())
		return;

	toggleButtonContainer();
	QPixmap * pPix = buttonContainer()->isVisible() ? g_pIconManager->getBigIcon("kvi_horizontal_left.png") : g_pIconManager->getBigIcon("kvi_horizontal_right.png");
	if(pPix)
		m_pHideToolsButton->setIcon(QIcon(*pPix));
}

void KviChannelWindow::triggerCreationEvents()
{
	KVS_TRIGGER_EVENT_0(KviEvent_OnChannelWindowCreated, this);
}

void KviChannelWindow::textViewRightClicked()
{
	KVS_TRIGGER_EVENT_0(KviEvent_OnChannelPopupRequest, this);
}

void KviChannelWindow::getBaseLogFileName(QString & szBuffer)
{
	QString szChan(windowName());
	szChan.replace(".", "%2e");
	if(connection())
	{
		szBuffer = szChan;
		szBuffer.append(".");
		szBuffer.append(connection()->currentNetworkName());
	}
	else
	{
		szBuffer = szChan;
		szBuffer.append(".");
		if(context())
			szBuffer.append(context()->id());
		else
			szBuffer.append("0");
	}
}

void KviChannelWindow::applyOptions()
{
	m_pUserListView->applyOptions();
	m_pTopicWidget->applyOptions();

	if(m_pMessageView)
		m_pMessageView->applyOptions();

	m_pModeWidget->applyOptions();

	// this applies options for IrcView and Input and forces the window to relayout
	KviWindow::applyOptions();
}

void KviChannelWindow::getConfigGroupName(QString & szBuffer)
{
	szBuffer = windowName();

	// save per-network channel settings, so that the settings of two channels
	// with the same name but of different networks gets different config entries.
	if(connection() && connection()->target() && connection()->target() && connection()->target()->network())
	{
		szBuffer.append("@");
		// don't use the actual network name, because it could change during the connection
		szBuffer.append(connection()->target()->network()->name());
	}
}

void KviChannelWindow::saveProperties(KviConfigurationFile * pCfg)
{
	KviWindow::saveProperties(pCfg);
	pCfg->writeEntry("TopSplitter", m_pTopSplitter->sizes());
	pCfg->writeEntry("Splitter", m_pUserListView->isHidden() ? m_SplitterSizesList : m_pSplitter->sizes());
	//	int iTimeStamp= pCfg->readIntEntry("EntryTimestamp", 0);
	// 	qDebug("window %s, group %s, view %d==%d, ulist %d==%d timestamp %d",
	// 		m_szName.toUtf8().data(), pCfg->group().toUtf8().data(),
	// 		m_pIrcView->width(), sizes.at(0), m_pUserListView->width(), sizes.at(1), iTimeStamp);
	pCfg->writeEntry("VertSplitter", m_pMessageView ? m_pVertSplitter->sizes() : m_VertSplitterSizesList);
	pCfg->writeEntry("PrivateBackground", m_privateBackground);
	pCfg->writeEntry("DoubleView", m_pMessageView ? true : false);

	pCfg->writeEntry("UserListHidden", m_pUserListView->isHidden());
	pCfg->writeEntry("ToolButtonsHidden", buttonContainer()->isHidden());
}

void KviChannelWindow::loadProperties(KviConfigurationFile * pCfg)
{
	int iWidth = width();
	QList<int> def;
	def.append((iWidth * 82) / 100);
	def.append((iWidth * 18) / 100);
	m_pTopSplitter->setSizes(pCfg->readIntListEntry("TopSplitter", def));

	//this is an hack to simulate qt3's ResizeMode = Stretch
	for(int iWidget = 0; iWidget < m_pTopSplitter->count(); iWidget++)
		m_pTopSplitter->setStretchFactor(iWidget, 1);

	def.clear();
	def.append((iWidth * 75) / 100);
	def.append((iWidth * 25) / 100);
	m_SplitterSizesList = pCfg->readIntListEntry("Splitter", def);
	m_pSplitter->setStretchFactor(0, 1);

	def.clear();

	def.append((iWidth * 60) / 100);
	def.append((iWidth * 40) / 100);
	m_VertSplitterSizesList = pCfg->readIntListEntry("VertSplitter", def);
	showDoubleView(pCfg->readBoolEntry("DoubleView", false));

	m_privateBackground = pCfg->readPixmapEntry("PrivateBackground", KviPixmap());
	if(m_privateBackground.pixmap())
	{
		m_pIrcView->setPrivateBackgroundPixmap(*(m_privateBackground.pixmap()));
		if(m_pMessageView)
			m_pMessageView->setPrivateBackgroundPixmap(*(m_privateBackground.pixmap()));
	}

	KviWindow::loadProperties(pCfg);
	if(m_pUserListView)
	{
		bool bHidden = pCfg->readBoolEntry("UserListHidden", false);
		m_pUserListView->setHidden(bHidden);
		m_pListViewButton->setChecked(!bHidden);
		if(!bHidden)
			m_pSplitter->setSizes(m_SplitterSizesList);
		resizeEvent(nullptr);
	}

	if(pCfg->readBoolEntry("ToolButtonsHidden", KVI_OPTION_BOOL(KviOption_boolHideWindowToolButtons)) != buttonContainer()->isHidden())
		toggleToolButtons();
}

void KviChannelWindow::showDoubleView(bool bShow)
{
	if(m_pMessageView)
	{
		if(bShow)
			return;

		m_pIrcView->joinMessagesFrom(m_pMessageView);
		m_VertSplitterSizesList = m_pVertSplitter->sizes();

		delete m_pMessageView;
		m_pMessageView = nullptr;

		if(m_pDoubleViewButton->isChecked())
			m_pDoubleViewButton->setChecked(false);
	}
	else
	{
		if(!bShow)
			return;

		m_pMessageView = new KviIrcView(m_pVertSplitter, this);
		m_pVertSplitter->setSizes(m_VertSplitterSizesList);

		//this is an hack to simulate qt3's ResizeMode = Stretch
		for(int iWidget = 0; iWidget < m_pVertSplitter->count(); iWidget++)
			m_pVertSplitter->setStretchFactor(iWidget, 1);

		//setFocusHandler(m_pInput,m_pMessageView); //socket it!
		if(!(m_pDoubleViewButton->isChecked()))
			m_pDoubleViewButton->setChecked(true);

		if(m_pIrcView->hasPrivateBackgroundPixmap())
			m_pMessageView->setPrivateBackgroundPixmap(*(m_pIrcView->getPrivateBackgroundPixmap()));

		connect(m_pMessageView, SIGNAL(rightClicked()), this, SLOT(textViewRightClicked()));
		m_pMessageView->setMasterView(m_pIrcView);
		m_pIrcView->splitMessagesTo(m_pMessageView);
		m_pMessageView->show();
	}
}

void KviChannelWindow::toggleDoubleView()
{
	if(m_pMessageView)
	{
		showDoubleView(false);
		if(m_pDoubleViewButton->isChecked())
			m_pDoubleViewButton->setChecked(false);
	}
	else
	{
		showDoubleView(true);
		if(!(m_pDoubleViewButton->isChecked()))
			m_pDoubleViewButton->setChecked(true);
	}
}

void KviChannelWindow::toggleListView()
{
	if(m_pUserListView->isVisible())
	{
		m_SplitterSizesList = m_pSplitter->sizes();

		m_pUserListView->hide();
		if(m_pListViewButton->isChecked())
			m_pListViewButton->setChecked(false);
	}
	else
	{
		m_pUserListView->show();
		if(!(m_pListViewButton->isChecked()))
			m_pListViewButton->setChecked(true);

		m_pSplitter->setSizes(m_SplitterSizesList);
	}
}

void KviChannelWindow::toggleModeEditor()
{
	if(m_pModeEditor)
	{
		delete m_pModeEditor;
		m_pModeEditor = nullptr;

		m_pSplitter->setMinimumHeight(20);
		if(m_pModeEditorButton->isChecked())
			m_pModeEditorButton->setChecked(false);
		resizeEvent(nullptr);
	}
	else
	{
		m_pModeEditor = new KviModeEditor(m_pSplitter, m_pModeEditorButton, "mode_editor", this);
		connect(m_pModeEditor, SIGNAL(setMode(const QString &)), this, SLOT(setMode(const QString &)));
		connect(m_pModeEditor, SIGNAL(done()), this, SLOT(modeSelectorDone()));
		m_pModeEditor->show();
		//setFocusHandlerNoClass(m_pInput,m_pModeEditor,"QLineEdit");
		if(!m_pModeEditorButton->isChecked())
			m_pModeEditorButton->setChecked(true);
	}
}

void KviChannelWindow::modeSelectorDone()
{
	if(m_pModeEditor)
		toggleModeEditor();
}

void KviChannelWindow::setMode(const QString & szMode)
{
	if(!connection())
		return;

	QByteArray channelName = connection()->encodeText(m_szName);
	QByteArray modes = connection()->encodeText(szMode);

	connection()->sendFmtData("MODE %s %s", channelName.data(), modes.data());
}

void KviChannelWindow::toggleListModeEditor()
{
	KviWindowToolPageButton * pButton = (KviWindowToolPageButton *)sender();
	if(!pButton)
		return; //wtf?

	char cMode = 0;

	for(auto iter : m_ListEditorButtons)
	{
		if(iter.second == pButton)
		{
			cMode = iter.first;
			break;
		}
	}

	if(!cMode)
		return; // wtf?

	const auto iEditor = m_ListEditors.find(cMode);
	if(iEditor != m_ListEditors.end())
	{
		KviMaskEditor * pEditor = iEditor->second;
		m_ListEditors.erase(iEditor);
		pEditor->deleteLater();

		pButton->setChecked(false);
	}
	else
	{
		if(!m_ModeLists.count(cMode))
		{
			m_ModeLists[cMode];

			m_szSentModeRequests.append(cMode);

			if(connection())
			{
				QByteArray szName = connection()->encodeText(m_szName);
				connection()->sendFmtData("MODE %s %c", szName.data(), cMode);
			}
		}

		std::vector<KviMaskEntry *> pMaskList = m_ModeLists[cMode];
		KviMaskEditor * pEditor = new KviMaskEditor(m_pSplitter, this, pButton, pMaskList, cMode, "list_mode_editor");
		connect(pEditor, SIGNAL(removeMasks(KviMaskEditor *, std::vector<KviMaskEntry *>)), this, SLOT(removeMasks(KviMaskEditor *, std::vector<KviMaskEntry *>)));
		m_ListEditors.emplace(cMode, pEditor);
		pEditor->show();
		pButton->setChecked(true);
	}
}

void KviChannelWindow::removeMasks(KviMaskEditor * pEditor, const std::vector<KviMaskEntry *> & pList)
{
	if(!connection())
		return;

	QString szMasks;
	QByteArray szFlags;
	QByteArray szTarget = connection()->encodeText(m_szName);

	int iMaxModeChangesPerLine = 3; // a good default
	KviIrcConnectionServerInfo * pServerInfo = serverInfo();
	if(pServerInfo)
	{
		iMaxModeChangesPerLine = pServerInfo->maxModeChanges();
		if(iMaxModeChangesPerLine < 1)
			iMaxModeChangesPerLine = 1;
	}

	int iCurModeChangesPerLine = iMaxModeChangesPerLine;

	// Calculate the max number of available characters in a MODE command
	// 512 (max) - 2 (cr, lf) - 4 (MODE) - 3 (spaces) - 1 (plus/minus)
	// since we already know the channel name, take it in account, too

	const int iMaxCharsPerLine = 502 - szTarget.size();
	int iCurCharsPerLine = iMaxCharsPerLine;

	for(auto i = pList.begin(); i != pList.end();)
	{
		KviMaskEntry * pEntry = *i;
		szFlags += pEditor->flag();
		iCurCharsPerLine--;

		if(!szMasks.isEmpty())
		{
			szMasks += " ";
			iCurCharsPerLine--;
		}

		szMasks += pEntry->szMask;
		iCurCharsPerLine -= pEntry->szMask.size();

		iCurModeChangesPerLine--;
		if(iCurModeChangesPerLine < 0 || iCurCharsPerLine < 0)
		{
			connection()->sendFmtData("MODE %s -%s %s", szTarget.data(), szFlags.data(), connection()->encodeText(szMasks).data());

			// move back the iterator by one position
			// WARNING: this could lead to an infinite loop if a single specified mode
			// change is longer than iMaxCharsPerLine
			pEntry = *--i;

			// reset cycle variables
			iCurCharsPerLine = iMaxCharsPerLine;
			iCurModeChangesPerLine = iMaxModeChangesPerLine;
			szFlags = "";
			szMasks = "";
		}
		else
		{
			++i;
		}
	}

	if(iCurModeChangesPerLine != iMaxModeChangesPerLine)
		connection()->sendFmtData("MODE %s -%s %s", szTarget.data(), szFlags.data(), connection()->encodeText(szMasks).data());
}

QPixmap * KviChannelWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon((m_iStateFlags & DeadChan) ? KviIconManager::DeadChannel : KviIconManager::Channel);
}

void KviChannelWindow::resizeEvent(QResizeEvent *)
{
	int iHeight = m_pInput->heightHint();
	int iHeight2 = m_pTopicWidget->sizeHint().height();
	m_pButtonBox->setGeometry(0, 0, width(), iHeight2);
	m_pSplitter->setGeometry(0, iHeight2, width(), height() - (iHeight + iHeight2));
	m_pInput->setGeometry(0, height() - iHeight, width(), iHeight);
}

QSize KviChannelWindow::sizeHint() const
{
	QSize ret(m_pSplitter->sizeHint().width(),
	    m_pIrcView->sizeHint().height() + m_pInput->heightHint() + m_pButtonBox->sizeHint().height());
	return ret;
}

void KviChannelWindow::setChannelMode(char cMode, bool bAdd)
{
	if(bAdd)
	{
		if(!(m_szChannelMode.contains(cMode)))
			m_szChannelMode.append(cMode);
	}
	else
	{
		if(m_szChannelMode.contains(cMode))
			m_szChannelMode.replace(cMode, "");
	}
	updateModeLabel();
	updateCaption();
}

void KviChannelWindow::setChannelModeWithParam(char cMode, QString & szParam)
{
	if(szParam.isEmpty())
		m_szChannelParameterModes.erase(cMode);
	else
		m_szChannelParameterModes[cMode] = szParam;
	updateModeLabel();
	updateCaption();
}

void KviChannelWindow::addHighlightedUser(const QString & szNick)
{
	if(!m_pUserListView->findEntry(szNick) || m_pTmpHighLighted->contains(szNick, Qt::CaseInsensitive))
		return;

	m_pTmpHighLighted->append(szNick);
}

void KviChannelWindow::removeHighlightedUser(const QString & szNick)
{
	m_pTmpHighLighted->removeOne(szNick);
}

void KviChannelWindow::getChannelModeString(QString & szBuffer)
{
	szBuffer = m_szChannelMode;

	//add modes that use a parameter
	for(auto iter : m_szChannelParameterModes)
		szBuffer.append(QChar(iter.first));
}

QString KviChannelWindow::getChannelModeStringWithEmbeddedParams()
{
	QString szBuffer = m_szChannelMode;

	//add modes that use a parameter
	for(auto iter : m_szChannelParameterModes)
		szBuffer.append(QString(" %1:%2").arg(QChar(iter.first)).arg(iter.second));

	return szBuffer;
}

bool KviChannelWindow::setOp(const QString & szNick, bool bOp, bool bIsMe)
{
	bool bRet = m_pUserListView->setOp(szNick, bOp);
	if(bIsMe)
		emit opStatusChanged();
	return bRet;
}

void KviChannelWindow::setDeadChan()
{
	m_iStateFlags |= DeadChan;
	m_iStateFlags &= ~(NoCloseOnPart | SentSyncWhoRequest);

	m_pUserListView->enableUpdates(false);
	m_pUserListView->partAll();
	m_pUserListView->enableUpdates(true);
	m_pUserListView->setUserDataBase(nullptr);

	//clear all mask editors
	for(auto & iter : m_ListEditors)
		iter.second->clear();

	//clear all mask lists (eg bans)
	for(auto & iter : m_ModeLists)
		iter.second.clear();

	m_ModeLists.clear();
	m_szSentModeRequests.clear();

	m_pTopicWidget->reset();

	qDeleteAll(m_lActionHistory);
	m_lActionHistory.clear();
	m_uActionHistoryHotActionCount = 0;

	m_szChannelMode = "";
	m_szChannelParameterModes.clear();

	// this should be moved to irc context!
	if(connection())
		connection()->unregisterChannel(this);
	if(context())
		context()->registerDeadChannel(this);

	setType(KviWindow::DeadChannel);

	updateIcon();
	updateModeLabel();
	updateCaption();
}

void KviChannelWindow::setAliveChan()
{
	// Rise like phoenix!
	m_iStateFlags = 0;
	setType(KviWindow::Channel);
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

	// refresh all open mask editors
	for(auto & iter : m_ListEditors)
	{
		char cMode = iter.second->flag();
		m_szSentModeRequests.append(cMode);

		if(connection())
		{
			QByteArray szName = connection()->encodeText(m_szName);
			connection()->sendFmtData("MODE %s %c", szName.data(), cMode);
		}
	}
}

void KviChannelWindow::getTalkingUsersStats(QString & szBuffer, QStringList & list, bool bPast)
{
	if(list.count() < 1)
		return;

	if(list.count() == 1)
	{
		szBuffer += "<b>";
		szBuffer += KviQString::toHtmlEscaped(list.first());
		szBuffer += "</b>";
		szBuffer += " ";
		szBuffer += bPast ? __tr2qs("said something recently") : __tr2qs("is talking");
	}
	else if(list.count() == 2)
	{
		szBuffer += "<b>";
		szBuffer += KviQString::toHtmlEscaped(list.first());
		szBuffer += "</b> ";
		szBuffer += __tr2qs("and");
		szBuffer += " <b>";
		list.erase(list.begin());
		szBuffer += KviQString::toHtmlEscaped(list.first());
		szBuffer += "</b> ";
		szBuffer += bPast ? __tr2qs("were talking recently") : __tr2qs("are talking");
	}
	else
	{
		szBuffer += "<b>";
		szBuffer += KviQString::toHtmlEscaped(list.first());
		szBuffer += "</b>, <b>";
		list.erase(list.begin());
		szBuffer += KviQString::toHtmlEscaped(list.first());
		if(list.count() == 2)
		{
			szBuffer += "</b> ";
			szBuffer += __tr2qs("and");
			szBuffer += " <b>";
			list.erase(list.begin());
			szBuffer += KviQString::toHtmlEscaped(list.first());
			szBuffer += "</b>";
		}
		else
		{
			// (list.count() - 1) is > 1
			szBuffer += "</b> ";
			szBuffer += __tr2qs("and another %1 users").arg(list.count() - 1);
		}
		szBuffer += " ";
		szBuffer += bPast ? __tr2qs("were talking recently") : __tr2qs("are talking");
	}
}

void KviChannelWindow::getWindowListTipText(QString & szBuffer)
{
	static QString szHtmlBold("<b>");
	static QString szHtmlTab("&nbsp;");
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

	szBuffer = "<html><body><table style=\"white-space: pre\">" START_TABLE_BOLD_ROW;

	if(m_iStateFlags & DeadChan)
	{
		szBuffer += __tr2qs("Dead channel");
		szBuffer += szEndOfFontBoldRow;
		szBuffer += szEndOfDoc;
		return;
	}

	KviUserListViewUserStats s;
	m_pUserListView->userStats(&s);

	szBuffer += KviQString::toHtmlEscaped(m_szPlainTextCaption);
	szBuffer += szEndOfFontBoldRow;

	szBuffer += szRowStart;

	QString szOp = __tr2qs("operator");
	QString szOps = __tr2qs("operators");


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
	szBuffer += (s.uActiveOp == 1 ? szOp : szOps);

	szBuffer += p9;

	szBuffer += p10;

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
	szBuffer += (s.uHotOp == 1 ? szOp : szOps);

	szBuffer += p9;


	szBuffer += szRowEnd;
	szBuffer += szRowStart;


	if(s.uIrcOp > 0)
	{
		szBuffer += szHtmlTab;
		szBuffer += szHtmlBold;
		szNum.setNum(s.uIrcOp);
		szBuffer += szNum;
		szBuffer += szHtmlBoldEnd;
		szBuffer += (s.uIrcOp == 1 ? __tr2qs("IRC operator") : __tr2qs("IRC operators"));
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
		szBuffer += (s.uOp == 1 ? szOp : szOps);
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
			szBuffer += R"(<tr><td bgcolor="#E0E0E0"><font color="#000000">)";
			getTalkingUsersStats(szBuffer, cas.lWereTalkingUsers, true);
			szBuffer += "</font>";
			szBuffer += szRowEnd;
		}
		szBuffer += R"(<tr><td bgcolor="#E0E0E0"><font color="#000000">)";
		getTalkingUsersStats(szBuffer, cas.lTalkingUsers, false);
		szBuffer += "</font>";
		szBuffer += szRowEnd;
	}
	else
	{
		if(cas.lWereTalkingUsers.count() > 0)
		{
			szBuffer += R"(<tr><td bgcolor="#E0E0E0"><font color="#000000">)";
			getTalkingUsersStats(szBuffer, cas.lWereTalkingUsers, true);
			szBuffer += "</font>";
			szBuffer += szRowEnd;
		}
	}

	szBuffer += R"(<tr><td bgcolor="#E0E0E0"><b><font color="#000000">)";

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
		szNum.sprintf(" [%u%% ", cas.uHotActionPercent);
		szBuffer += szNum;
		szBuffer += __tr2qs("human");
		szBuffer += "]";
	}

	szBuffer += "</font></b></td></tr>";

	szBuffer += szEndOfDoc;
}

void KviChannelWindow::fillCaptionBuffers()
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
	}
	else
	{
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

void KviChannelWindow::ownMessage(const QString & szBuffer, bool bUserFeedback)
{
	if(!connection())
		return;

	//my full mask as seen by other users
	QString szMyFullMask = connection()->userInfo()->nickName() + "!" + connection()->userInfo()->userName() + "@" + connection()->userInfo()->hostName();
	QByteArray myFullMask = connection()->encodeText(szMyFullMask); //target name
	QByteArray name = connection()->encodeText(windowName());       //message
	QByteArray data = encodeText(szBuffer);
	const char * pData = data.data();
	/* max length of a PRIVMSG text. Max buffer length for our send is 512 byte, but we have to
	* remember that the server will prepend to the message our full mask and truncate the resulting
	* at 512 bytes again..
	* So we have:
	* :NickName!~ident@hostname.tld PRIVMSG #channel :text of message(CrLf)
	* NickName!~ident@hostname.tld#channeltext of message
	* 512(rfc) -2(CrLf) -2(:) -3(spaces) -7(PRIVMSG) = 498
	* usable bytes, excluding our full mask and the target name.
	*/
	int iMaxMsgLen = 498 - name.length() - myFullMask.length();

	// our copy of the message
	QString szTmpBuffer(szBuffer);

	if(!pData)
		return;

#ifdef COMPILE_CRYPT_SUPPORT
	if(cryptSessionInfo())
	{
		if(cryptSessionInfo()->m_bDoEncrypt)
		{
			if(*pData != KviControlCodes::CryptEscape)
			{
				KviCString szEncrypted;
				cryptSessionInfo()->m_pEngine->setMaxEncryptLen(iMaxMsgLen);
				switch(cryptSessionInfo()->m_pEngine->encrypt(pData, szEncrypted))
				{
					case KviCryptEngine::Encrypted:
						if(!connection()->sendFmtData("PRIVMSG %s :%s", name.data(), szEncrypted.ptr()))
							return;
						if(bUserFeedback)
							m_pConsole->outputPrivmsg(this, KVI_OUT_OWNPRIVMSGCRYPTED,
							    QString(), QString(), QString(), szBuffer, KviConsoleWindow::NoNotifications);
						break;
					case KviCryptEngine::Encoded:
					{
						if(!connection()->sendFmtData("PRIVMSG %s :%s", name.data(), szEncrypted.ptr()))
							return;
						if(bUserFeedback)
						{
							// ugly, but we must redecode here
							QString szRedecoded = decodeText(szEncrypted.ptr());
							m_pConsole->outputPrivmsg(this, KVI_OUT_OWNPRIVMSG,
							    QString(), QString(), QString(), szRedecoded, KviConsoleWindow::NoNotifications);
						}
					}
					break;
					default: // also case KviCryptEngine::EncryptError
					{
						QString szEngineError = cryptSessionInfo()->m_pEngine->lastError();
						output(KVI_OUT_SYSTEMERROR,
						    __tr2qs("The encryption engine was unable to encrypt the current message (%Q): %Q, no data sent to the server"),
						    &szBuffer, &szEngineError);
					}
					break;
				}
				userAction(connection()->currentNickName(), KVI_USERACTION_PRIVMSG);
				return;
			}
			else
			{
				//eat the escape code
				pData++;
				szTmpBuffer.remove(0, 1);
				//let the normal function do it
			}
		}
	}
#endif

	if(data.length() > iMaxMsgLen)
	{
		/* Multi message; we want to split the message, preferably on a word boundary,
		 * and send each message part as a different PRIVMSG
		 * Due to encoding stuff, this is frikin'time eater
		 */
		QTextEncoder * pEncoder = makeEncoder(); // our temp encoder
		QByteArray szTmp;                        // used to calculate the length of an encoded message
		int iPos;                                // contains the index where to truncate szTmpBuffer
		int iC;                                  // cycles counter (debugging/profiling purpose)
		float fPosDiff;                          // optimization internal; aggressivity factor
		QString szCurSubString;                  // truncated parts as reported to the user

		// run until we've something remaining in the message
		while(szTmpBuffer.length())
		{
			// init counters
			iPos = szTmpBuffer.length();
			iC = 0;

			// first part (optimization): quickly find an high index that is _surely_lesser_
			// than the correct one
			while(true)
			{
				iC++;
				szTmp = pEncoder->fromUnicode(szTmpBuffer.left(iPos));
				if(szTmp.length() <= iMaxMsgLen)
					break;
				//if szTmp.length() == 0 we already have break'ed out from here,
				// so we can safely use it as a divisor
				fPosDiff = (float)iMaxMsgLen / szTmp.length();
				iPos = (int)(iPos * fPosDiff); // cut the string at each cycle
				                               //printf("OPTIMIZATION: fPosDiff %f, iPos %d\n", fPosDiff, iPos);
			}
			//printf("Multi message: %d optimization cyles", iC);

			// now, do it the simple way: increment our index until we perfectly fit into the
			// available space
			while(true)
			{
				iC++;

				szTmp = pEncoder->fromUnicode(szTmpBuffer.left(iPos));

				// perfect match
				if(iPos == szTmpBuffer.length())
					break;

				if(szTmp.length() > iMaxMsgLen)
				{
					// overflowed.. last one was the good one
					iPos--;
					szTmp = pEncoder->fromUnicode(szTmpBuffer.left(iPos));
					break;
				}
				else
				{
					//there's still free space.. add another char
					iPos++;
				}
			}
			//printf(", finished at %d cycles, truncated at pos %d\n", iC, iPos);
			//prepare the feedback string for the user
			szCurSubString = szTmpBuffer.left(iPos);

			//send the string to the server
			if(connection()->sendFmtData("PRIVMSG %s :%s", name.data(), szTmp.data()))
			{
				//feeedback the user
				if(bUserFeedback)
					m_pConsole->outputPrivmsg(this, KVI_OUT_OWNPRIVMSG, QString(), QString(), QString(), szCurSubString, KviConsoleWindow::NoNotifications);
				userAction(connection()->currentNickName(), KVI_USERACTION_PRIVMSG);
			}
			else
			{
				// skipped a part in this multi message.. we don't want to continue
				return;
			}

			// remove the sent part of the string
			szTmpBuffer.remove(0, iPos);
			//printf("Sent %d chars, %d remaining in the Qstring\n",iPos, szTmpBuffer.length());
		}
	}
	else
	{
		if(connection()->sendFmtData("PRIVMSG %s :%s", name.data(), pData))
		{
			if(bUserFeedback)
				m_pConsole->outputPrivmsg(this, KVI_OUT_OWNPRIVMSG, QString(), QString(), QString(), szTmpBuffer, KviConsoleWindow::NoNotifications);
			userAction(connection()->currentNickName(), KVI_USERACTION_PRIVMSG);
		}
	}
}

void KviChannelWindow::ownAction(const QString & szBuffer)
{
	if(!connection())
		return;

	QString szTmpBuffer;

	//see bug ticket #220
	if(KVI_OPTION_BOOL(KviOption_boolStripMircColorsInUserMessages))
		szTmpBuffer = KviControlCodes::stripControlBytes(szBuffer);
	else
		szTmpBuffer = szBuffer;

	//my full mask as seen by other users
	QString szMyName = connection()->userInfo()->nickName();
	QString szMyFullMask = szMyName + "!" + connection()->userInfo()->userName() + "@" + connection()->userInfo()->hostName();
	QByteArray myFullMask = connection()->encodeText(szMyFullMask); //target name
	QByteArray name = connection()->encodeText(windowName());       //message
	QByteArray data = encodeText(szBuffer);
	/* max length of a PRIVMSG text. Max buffer length for our send is 512 byte, but we have to
	* remember that the server will prepend to the message our full mask and truncate the resulting
	* at 512 bytes again..
	* So we have:
	* :NickName!~ident@hostname.tld PRIVMSG #channel :text of message(CrLf)
	* NickName!~ident@hostname.tld#channeltext of message
	* 512(rfc) -2(CrLf) -2(:) -3(spaces) -7(PRIVMSG) -8(\x01ACTION\x01) = 490
	* usable bytes, excluding our full mask and the target name.
	*/
	int iMaxMsgLen = 490 - name.length() - myFullMask.length();

	if(KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnMeAction, this, szTmpBuffer, windowName()))
		return;

#ifdef COMPILE_CRYPT_SUPPORT
	if(cryptSessionInfo() && cryptSessionInfo()->m_bDoEncrypt)
	{
		if(szTmpBuffer[0] != KviControlCodes::CryptEscape)
		{
			KviCString szEncrypted;
			cryptSessionInfo()->m_pEngine->setMaxEncryptLen(iMaxMsgLen);
			switch(cryptSessionInfo()->m_pEngine->encrypt(data.data(), szEncrypted))
			{
				case KviCryptEngine::Encrypted:
				{
					if(!connection()->sendFmtData("PRIVMSG %s :%cACTION %s%c", name.data(), 0x01, szEncrypted.ptr(), 0x01))
						return;

					QString szBuf = "\r!nc\r";
					szBuf += szMyName;
					szBuf += "\r ";
					szBuf += szTmpBuffer;
					outputMessage(KVI_OUT_OWNACTIONCRYPTED, szBuf);
				}
				break;
				case KviCryptEngine::Encoded:
				{
					if(!connection()->sendFmtData("PRIVMSG %s :%cACTION %s%c", name.data(), 0x01, szEncrypted.ptr(), 0x01))
						return;

					// ugly, but we must redecode here
					QString szRedecoded = decodeText(szEncrypted.ptr());

					QString szBuf = "\r!nc\r";
					szBuf += szMyName;
					szBuf += "\r ";
					szBuf += szRedecoded;
					outputMessage(KVI_OUT_OWNACTIONCRYPTED, szBuf);
				}
				break;
				default:
				{
					QString szEngineError = cryptSessionInfo()->m_pEngine->lastError();
					output(KVI_OUT_SYSTEMERROR,
					    __tr2qs("The encryption engine was unable to encrypt the current message (%Q): %Q, no data sent to the server"),
					    &szBuffer, &szEngineError);
				}
			}
			userAction(szMyName, KVI_USERACTION_ACTION);
			return;
		}
		else
		{
			//eat the escape code
			szTmpBuffer.remove(0, 1);
			data = encodeText(szTmpBuffer);
		}
	}
#endif

	if(data.length() > iMaxMsgLen)
	{
		/* Multi message; we want to split the message, preferably on a word boundary,
		 * and send each message part as a different PRIVMSG
		 * Due to encoding stuff, this is frikin'time eater
		 */
		QTextEncoder * pEncoder = makeEncoder(); // our temp encoder
		QByteArray szTmp;                        // used to calculate the length of an encoded message
		int iPos;                                // contains the index where to truncate szTmpBuffer
		int iC;                                  // cycles counter (debugging/profiling purpose)
		float fPosDiff;                          // optimization internal; aggressivity factor
		QString szCurSubString;                  // truncated parts as reported to the user

		// run until we've something remaining in the message
		while(szTmpBuffer.length())
		{
			// init counters
			iPos = szTmpBuffer.length();
			iC = 0;

			// first part (optimization): quickly find an high index that is _surely_lesser_
			// than the correct one
			while(true)
			{
				iC++;
				szTmp = pEncoder->fromUnicode(szTmpBuffer.left(iPos));
				if(szTmp.length() <= iMaxMsgLen)
					break;
				//if szTmp.length() == 0 we already have break'ed out from here,
				// so we can safely use it as a divisor
				fPosDiff = (float)iMaxMsgLen / szTmp.length();
				iPos = (int)(iPos * fPosDiff); // cut the string at each cycle
				                               //printf("OPTIMIZATION: fPosDiff %f, iPos %d\n", fPosDiff, iPos);
			}
			//printf("Multi message: %d optimization cyles", iC);
			// now, do it the simple way: increment our index until we perfectly fit into the
			// available space
			while(true)
			{
				iC++;

				szTmp = pEncoder->fromUnicode(szTmpBuffer.left(iPos));

				// perfect match
				if(iPos == szTmpBuffer.length())
					break;

				if(szTmp.length() > iMaxMsgLen)
				{
					// overflowed.. last one was the good one
					iPos--;
					szTmp = pEncoder->fromUnicode(szTmpBuffer.left(iPos));
					break;
				}
				else
				{
					//there's still free space.. add another char
					iPos++;
				}
			}
			//printf(", finished at %d cycles, truncated at pos %d\n", iC, iPos);
			//prepare the feedback string for the user
			szCurSubString = szTmpBuffer.left(iPos);
			//send the string to the server
			if(connection()->sendFmtData("PRIVMSG %s :%cACTION %s%c", name.data(), 0x01, szTmp.data(), 0x01))
			{
				//feeedback the user
				QString szBuf = "\r!nc\r";
				szBuf += connection()->currentNickName();
				szBuf += "\r ";
				szBuf += szTmp.data();
				outputMessage(KVI_OUT_OWNACTION, szBuf);
				userAction(connection()->currentNickName(), KVI_USERACTION_ACTION);
			}
			else
			{
				// skipped a part in this multi message.. we don't want to continue
				return;
			}

			// remove the sent part of the string
			szTmpBuffer.remove(0, iPos);
			//printf("Sent %d chars, %d remaining in the Qstring\n",iPos, szTmpBuffer.length());
		}
	}
	else
	{
		if(!connection()->sendFmtData("PRIVMSG %s :%cACTION %s%c", name.data(), 0x01, data.data(), 0x01))
			return;

		QString szBuf = "\r!nc\r";
		szBuf += connection()->currentNickName();
		szBuf += "\r ";
		szBuf += szTmpBuffer;
		outputMessage(KVI_OUT_OWNACTION, szBuf);
		userAction(connection()->currentNickName(), KVI_USERACTION_ACTION);
	}
}

bool KviChannelWindow::nickChange(const QString & szOldNick, const QString & szNewNick)
{
	bool bWasHere = m_pUserListView->nickChange(szOldNick, szNewNick);
	if(bWasHere)
	{
		channelAction(szNewNick, KVI_USERACTION_NICK, kvi_getUserActionTemperature(KVI_USERACTION_NICK));
		// update any nick/mask editor; by now we limit to the q and a mode
		if(m_ModeLists.count('q'))
			setModeInList('q', szOldNick, false, QString(), 0, szNewNick);
		if(m_ModeLists.count('a'))
			setModeInList('a', szOldNick, false, QString(), 0, szNewNick);
	}
	return bWasHere;
}

bool KviChannelWindow::part(const QString & szNick)
{
	bool bWasHere = m_pUserListView->part(szNick);
	if(bWasHere)
	{
		channelAction(szNick, KVI_USERACTION_PART, kvi_getUserActionTemperature(KVI_USERACTION_PART));
		// update any nick/mask editor; by now we limit to the q and a mode
		if(m_ModeLists.count('q'))
			setModeInList('q', szNick, false, QString(), 0);
		if(m_ModeLists.count('a'))
			setModeInList('a', szNick, false, QString(), 0);
	}
	return bWasHere;
}

bool KviChannelWindow::activityMeter(unsigned int * puActivityValue, unsigned int * puActivityTemperature)
{
	fixActionHistory();

	unsigned int uHotActionPercent;
	double dActionsPerMinute;

	if(m_lActionHistory.count() < 1)
	{
		// nothing is happening
		uHotActionPercent = 0;
		dActionsPerMinute = 0;
	}
	else
	{
		kvi_time_t tNow = kvi_unixTime();

		KviChannelAction * pAction = m_lActionHistory.last();

		double dSpan = (double)(tNow - pAction->tTime);

		if(m_lActionHistory.count() < KVI_CHANNEL_ACTION_HISTORY_MAX_COUNT)
		{
			if(m_joinTime.secsTo(QDateTime::currentDateTime()) < KVI_CHANNEL_ACTION_HISTORY_MAX_TIMESPAN)
			{
				// we can't exactly estimate
				if(dSpan < 60.0)
					dSpan = 60.0;
			}
			else
			{
				// there are less actions at all or they have been pushed out because of the timespan
				dSpan = KVI_CHANNEL_ACTION_HISTORY_MAX_TIMESPAN;
			}
		} // else the actions have been pushed out of the history because they were too much

		if(dSpan > 0.0)
			dActionsPerMinute = (((double)(m_lActionHistory.count())) / (dSpan)) * 60.0;
		else
			dActionsPerMinute = (double)(m_lActionHistory.count()); // ???

		uHotActionPercent = (m_uActionHistoryHotActionCount * 100) / (m_lActionHistory.count());
	}

	if(dActionsPerMinute < 0.3)
		*puActivityValue = KviWindow::None;
	else if(dActionsPerMinute < 1.0)
		*puActivityValue = KviWindow::VeryLow;
	else if(dActionsPerMinute < 4.0)
		*puActivityValue = KviWindow::Low;
	else if(dActionsPerMinute < 10.0)
		*puActivityValue = KviWindow::Medium;
	else if(dActionsPerMinute < 30.0)
		*puActivityValue = KviWindow::High;
	else
		*puActivityValue = KviWindow::VeryHigh;

	if(uHotActionPercent < Ice)
		*puActivityTemperature = KviWindow::Ice;
	else if(uHotActionPercent < VeryCold)
		*puActivityTemperature = KviWindow::VeryCold;
	else if(uHotActionPercent < Cold)
		*puActivityTemperature = KviWindow::Cold;
	else if(uHotActionPercent < Undefined)
		*puActivityTemperature = KviWindow::Undefined;
	else if(uHotActionPercent < Hot)
		*puActivityTemperature = KviWindow::Hot;
	else if(uHotActionPercent < VeryHot)
		*puActivityTemperature = KviWindow::VeryHot;
	else
		*puActivityTemperature = KviWindow::Fire;

	return true;
}

void KviChannelWindow::channelAction(const QString & szNick, unsigned int uActionType, int iTemperature)
{
	KviChannelAction * pAction = new KviChannelAction;
	pAction->tTime = kvi_unixTime();
	pAction->uActionType = uActionType;
	pAction->iTemperature = iTemperature;
	pAction->szNick = szNick;

	if(iTemperature > 0)
		m_uActionHistoryHotActionCount++;

	m_lActionHistory.append(pAction);
	fixActionHistory();
}

void KviChannelWindow::fixActionHistory()
{
	while(m_lActionHistory.count() > KVI_CHANNEL_ACTION_HISTORY_MAX_COUNT)
		delete m_lActionHistory.takeFirst();

	if(m_lActionHistory.isEmpty())
		return;

	KviChannelAction * pAction = m_lActionHistory.last();
	kvi_time_t tMinimum = pAction->tTime - KVI_CHANNEL_ACTION_HISTORY_MAX_TIMESPAN;

	KviChannelAction * pAct = m_lActionHistory.first();
	while(pAct && (pAct->tTime < tMinimum))
	{
		if(pAct->iTemperature > 0)
			m_uActionHistoryHotActionCount--;
		delete m_lActionHistory.takeFirst();
		pAct = m_lActionHistory.first();
	}
}

void KviChannelWindow::lostUserFocus()
{
	KviWindow::lostUserFocus();
	if(!m_pMessageView)
		return;
	if(m_pMessageView->hasLineMark())
		m_pMessageView->clearLineMark(true);
}

void KviChannelWindow::getChannelActivityStats(KviChannelActivityStats * pStats)
{
	fixActionHistory();

	pStats->uActionCount = m_lActionHistory.count();
	pStats->iAverageActionTemperature = 0;
	pStats->uActionsInTheLastMinute = 0;
	pStats->uHotActionCount = 0;
	pStats->uHotActionPercent = 0;
	pStats->bStatsInaccurate = false;

	if(pStats->uActionCount < 1)
	{
		// nothing is happening
		pStats->uLastActionTimeSpan = 0;
		pStats->uFirstActionTimeSpan = 0;
		pStats->dActionsPerMinute = 0;

		return;
	}

	kvi_time_t tNow = kvi_unixTime();

	KviChannelAction * pAction = m_lActionHistory.last();
	pStats->uLastActionTimeSpan = tNow - pAction->tTime;

	pAction = m_lActionHistory.first();
	pStats->uFirstActionTimeSpan = tNow - pAction->tTime;

	double dSpan = (double)pStats->uFirstActionTimeSpan;

	if(pStats->uActionCount < KVI_CHANNEL_ACTION_HISTORY_MAX_COUNT)
	{
		if(m_joinTime.secsTo(QDateTime::currentDateTime()) < KVI_CHANNEL_ACTION_HISTORY_MAX_TIMESPAN)
		{
			// we can't exactly estimate
			pStats->bStatsInaccurate = true;
			if(dSpan < 60.0)
				dSpan = 60.0;
		}
		else
		{
			// there are less actions at all or they have been pushed out because of the timespan
			dSpan = KVI_CHANNEL_ACTION_HISTORY_MAX_TIMESPAN;
		}
	} // else the actions have been pushed out of the history because they were too much

	if(dSpan > 0.0)
		pStats->dActionsPerMinute = (((double)pStats->uActionCount) / (dSpan)) * 60.0;
	else
		pStats->dActionsPerMinute = (double)pStats->uActionCount; // ???

	kvi_time_t tTwoMinsAgo = tNow;
	tTwoMinsAgo -= 120;
	tNow -= 60;

	std::set<QString> userDict;

	pStats->lTalkingUsers.clear();
	pStats->lWereTalkingUsers.clear();

	for(unsigned i = m_lActionHistory.count(); i-- > 0; )
	{
		pAction = m_lActionHistory[i];

		if(pAction->tTime >= tNow)
			pStats->uActionsInTheLastMinute++;

		if(pAction->iTemperature > 0)
			pStats->uHotActionCount++;

		pStats->iAverageActionTemperature += pAction->iTemperature;

		if((pAction->uActionType == KVI_USERACTION_PRIVMSG) || (pAction->uActionType == KVI_USERACTION_NOTICE) || (pAction->uActionType == KVI_USERACTION_ACTION))
		{
			if(userDict.count(pAction->szNick) == 0)
			{
				if(isOn(pAction->szNick.toLatin1()))
				{
					if(pAction->tTime >= tTwoMinsAgo)
						pStats->lTalkingUsers.append(pAction->szNick);
					else
						pStats->lWereTalkingUsers.append(pAction->szNick);

					userDict.insert(pAction->szNick);
				}
			}
		}
	}

	pStats->iAverageActionTemperature = pStats->iAverageActionTemperature / (int)pStats->uActionCount;

	pStats->uHotActionPercent = (pStats->uHotActionCount * 100) / pStats->uActionCount;
}

void KviChannelWindow::userAction(const QString & szNick, const QString & szUser, const QString & szHost, unsigned int uActionType)
{
	int iTemperature = kvi_getUserActionTemperature(uActionType);
	channelAction(szNick, uActionType, iTemperature);
	m_pUserListView->userAction(szNick, szUser, szHost, iTemperature);
}

void KviChannelWindow::userAction(const QString & szNick, unsigned int uActionType)
{
	int iTemperature = kvi_getUserActionTemperature(uActionType);
	channelAction(szNick, uActionType, iTemperature);
	m_pUserListView->userAction(szNick, iTemperature);
}

void KviChannelWindow::userAction(KviIrcMask * user, unsigned int uActionType)
{
	int iTemperature = kvi_getUserActionTemperature(uActionType);
	channelAction(user->nick(), uActionType, iTemperature);
	m_pUserListView->userAction(user, iTemperature);
}

void KviChannelWindow::topicSelected(const QString & szTopic)
{
	if(!connection())
		return;

	QByteArray encoded = encodeText(szTopic);
	QByteArray name = connection()->encodeText(m_szName);

#ifdef COMPILE_CRYPT_SUPPORT
	if(encoded.length() <= 0)
	{
		connection()->sendFmtData("TOPIC %s :", name.data());
		return;
	}

	const char * pcData = encoded.data();

	QString szTmpTopic(szTopic);

	if(!pcData)
		return;

	if(cryptSessionInfo())
	{
		if(cryptSessionInfo()->m_bDoEncrypt)
		{
			if(*pcData != KviControlCodes::CryptEscape)
			{
				KviCString szEncrypted;
				switch(cryptSessionInfo()->m_pEngine->encrypt(pcData, szEncrypted))
				{
					case KviCryptEngine::Encrypted:
					case KviCryptEngine::Encoded:
						connection()->sendFmtData("TOPIC %s :%s", name.data(), szEncrypted.ptr());
						return;
						break;
					default:
						QString szEngineError = cryptSessionInfo()->m_pEngine->lastError();
						output(KVI_OUT_SYSTEMERROR,
						    __tr2qs("The encryption engine was unable to encrypt the current message (%Q): %s, no data sent to the server"),
						    &szTopic, &szEngineError);
						break;
				}
			}
			else
			{
				szTmpTopic.remove(0, 1);
				// re-encode for removed CryptEscape
				encoded = encodeText(szTmpTopic);
			}
		}
	}
#endif

	connection()->sendFmtData("TOPIC %s :%s", name.data(), encoded.length() ? encoded.data() : "");
}

void KviChannelWindow::closeEvent(QCloseEvent * pEvent)
{
	if((m_iStateFlags & SentPart) || (m_iStateFlags & DeadChan) || !(m_pConsole->isConnected()))
	{
		if(context())
			context()->unregisterDeadChannel(this);
		KviWindow::closeEvent(pEvent);
	}
	else
	{
		pEvent->ignore();
		// FIXME: #warning "THIS PART SHOULD BECOME A COMMAND /PART $option()..so the identifiers are parsed"
		if(connection())
		{
			QString szTmp = KVI_OPTION_STRING(KviOption_stringPartMessage);
			KviQString::escapeKvs(&szTmp, KviQString::PermitVariables | KviQString::PermitFunctions);
			KviKvsVariant vRet;

			if(KviKvsScript::evaluate(szTmp, this, nullptr, &vRet))
				vRet.asString(szTmp);

			QByteArray dat = encodeText(szTmp);
			partMessageSent();
			QByteArray name = connection()->encodeText(m_szName);
			connection()->sendFmtData("PART %s :%s", name.data(), dat.data() ? dat.data() : "");
			// be sure to not reference ourselves here.. we could be disconnected!
		}
		else
		{
			partMessageSent(); // huh ?
		}
	}
}

void KviChannelWindow::partMessageSent(bool bCloseOnPart, bool bShowMessage)
{
	m_iStateFlags |= SentPart;
	if(!bCloseOnPart)
		m_iStateFlags |= NoCloseOnPart;
	if(bShowMessage)
		outputNoFmt(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Sent part request, waiting for reply..."));
}

#define IS_FNC(__name, __ulvname)                                                     \
	bool KviChannelWindow::__name(bool bAtLeast)                                      \
	{                                                                                 \
		if(!connection())                                                             \
			return false;                                                             \
		return m_pUserListView->__ulvname(connection()->currentNickName(), bAtLeast); \
	}

IS_FNC(isMeChanOwner, isChanOwner)
IS_FNC(isMeChanAdmin, isChanAdmin)
IS_FNC(isMeOp, isOp)
IS_FNC(isMeVoice, isVoice)
IS_FNC(isMeHalfOp, isHalfOp)
IS_FNC(isMeUserOp, isUserOp)

int KviChannelWindow::myFlags()
{
	if(!connection())
		return 0;

	return m_pUserListView->flags(connection()->currentNickName());
}

void KviChannelWindow::setModeInList(char cMode, const QString & szMask, bool bAdd, const QString & szSetBy, unsigned int uSetAt, QString szChangeMask)
{
	if(!connection())
		return;

	if(!m_ModeLists.count(cMode))
	{
		// we want to remove an item but we don't have any list?
		if(!bAdd)
			return;
		// lazily insert it
		m_ModeLists[cMode];
	}

	std::vector<KviMaskEntry *> & pList = m_ModeLists[cMode];
	KviMaskEditor * pEditor = nullptr;
	if(m_ListEditors.count(cMode))
		pEditor = m_ListEditors[cMode];

	internalMask(szMask, bAdd, szSetBy, uSetAt, pList, &pEditor, szChangeMask);
	m_pUserListView->setMaskEntries(cMode, pList.size());
}

void KviChannelWindow::internalMask(const QString & szMask, bool bAdd, const QString & szSetBy, unsigned int uSetAt, std::vector<KviMaskEntry *> & pList, KviMaskEditor ** ppEd, QString & szChangeMask)
{
	KviMaskEntry * pEntry = nullptr;
	if(bAdd)
	{
		for(auto & e : pList)
		{
			if(KviQString::equalCI(e->szMask, szMask))
				return; //already there
		}
		pEntry = new KviMaskEntry;
		pEntry->szMask = szMask;
		pEntry->szSetBy = (!szSetBy.isEmpty()) ? szSetBy : __tr2qs("(Unknown)");
		pEntry->uSetAt = uSetAt;
		pList.push_back(pEntry);
		if(*ppEd)
			(*ppEd)->addMask(pEntry);
	}
	else
	{
		auto iter = pList.begin();
		for(; iter != pList.end() && !KviQString::equalCI((*iter)->szMask, szMask); ++iter);

		if(iter != pList.end())
		{
			//delete mask from the editor
			if(*ppEd)
				(*ppEd)->removeMask(*iter);

			if(szChangeMask.isNull())
			{
				//delete mask
				pList.erase(iter);
			}
			else
			{
				//update mask
				(*iter)->szMask = szChangeMask;
				if(*ppEd)
					(*ppEd)->addMask(*iter);
			}
		}
	}
}

void KviChannelWindow::updateModeLabel()
{
	static QString br("<br>");
	static QString tds = "<tr><td style=\"background-color: rgb(48,48,48); white-space: nowrap; font-weight: bold; color: rgb(255,255,255); text-align:center; padding-left: 5px; padding-right: 5px;\">";
	static QString snr = "<tr><td style=\"white-space: pre; padding-left: 5px; padding-right: 5px;\">";
	static QString enr = "</td></tr>";

	QString szTip = "<html><body><table>";
	szTip += tds;
	szTip += __tr2qs("Channel Modes");
	szTip += enr;

	KviCString szMod = m_szChannelMode;
	const char * pcAux = szMod.ptr();
	KviIrcConnectionServerInfo * pServerInfo = serverInfo();

	szTip += snr;
	while(*pcAux)
	{
		if(pServerInfo)
			KviQString::appendFormatted(szTip, "<b>%c</b>: %Q" + br, *pcAux, &(m_pConsole->connection()->serverInfo()->getChannelModeDescription(*pcAux)));
		++pcAux;
	}

	for(auto & iter : m_szChannelParameterModes)
	{
		QString szDescription = KviQString::toHtmlEscaped(m_pConsole->connection()->serverInfo()->getChannelModeDescription(iter.first));
		QString szValue = KviQString::toHtmlEscaped(iter.second);
		KviQString::appendFormatted(szTip, br + "<b>%c</b>: %Q: <b>%Q</b>", iter.first, &szDescription, &szValue);
	}

	szTip += enr + snr + "<hr>";
	szTip +=  __tr2qs("Double-click to edit...");
	szTip += enr;
	szTip += "</table></body></html>";

	m_pModeWidget->refreshModes();
	KviTalToolTip::remove(m_pModeWidget);
	KviTalToolTip::add(m_pModeWidget, szTip);
}

void KviChannelWindow::outputMessage(int iMsgType, const QString & szMsg, const QDateTime & datetime)
{
	QString szBuf(szMsg);
	preprocessMessage(szBuf);

	const QChar * pC = szBuf.constData();
	if(!pC)
		return;

	if(m_pMessageView && m_pIrcView->messageShouldGoToMessageView(iMsgType))
		internalOutput(m_pMessageView, iMsgType, (const kvi_wchar_t *)pC, 0, datetime);
	else
		internalOutput(m_pIrcView, iMsgType, (const kvi_wchar_t *)pC, 0, datetime);
}

void KviChannelWindow::checkChannelSync()
{
	if(m_iStateFlags & Synchronized)
		return;

	if(m_iStateFlags & SentWhoRequest)
	{
		if(!(m_iStateFlags & HaveWhoList))
			return;
	}

	// check if we're in the on-join request queue list
	if(connection()->requestQueue()->isQueued(this))
		return;

	// check if there's any request still runinng
	if(m_szSentModeRequests.size() != 0)
		return;

	m_iStateFlags |= Synchronized;
	// we already have all the spontaneous server replies
	// (so probably mode, topic (or no topic is set),names)
	// we have already received the I and e lists (if requested)
	kvs_int_t iSyncTime = m_joinTime.time().msecsTo(QTime::currentTime());
	if(iSyncTime < 0)
		iSyncTime += 86400000;

	bool bStop = KVS_TRIGGER_EVENT_1_HALTED(KviEvent_OnChannelSync, this, iSyncTime);

	if(!bStop && KVI_OPTION_BOOL(KviOption_boolShowChannelSyncTime))
	{
		output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Channel synchronized in %d.%d seconds"), iSyncTime / 1000, iSyncTime % 1000);
	}
}

KviIrcView * KviChannelWindow::lastClickedView() const
{
	if(m_pMessageView && m_pIrcView && m_pMessageView->lastMouseClickTime() >= m_pIrcView->lastMouseClickTime())
			return m_pMessageView;

	return m_pIrcView;
}

bool KviChannelWindow::eventFilter(QObject * pObject, QEvent * pEvent)
{
	if(pEvent->type() == QEvent::FocusOut && pObject == m_pTopicWidget && m_pTopicWidget->isVisible())
		m_pTopicWidget->deactivate();

	return KviWindow::eventFilter(pObject, pEvent);
}

void KviChannelWindow::preprocessMessage(QString & szMessage)
{
	// FIXME: slow

	KviIrcConnectionServerInfo * pServerInfo = serverInfo();
	if(!pServerInfo)
		return;

	static QString szNonStandardLinkPrefix = QString::fromLatin1("\r![");

	if(szMessage.contains(szNonStandardLinkPrefix))
		return; // contains a non standard link that may contain spaces, do not break it.

	QStringList strings = szMessage.split(" ", QString::KeepEmptyParts);
	for(auto & it : strings)
	{
		if(it.contains('\r'))
			continue;

		QString szTmp = KviControlCodes::stripControlBytes(it).trimmed();
		if(szTmp.length() < 1)
			continue;

		// FIXME: Do we REALLY need this ?
		if(findEntry(it))
		{
			it = QString("\r!n\r%1\r").arg(it);
			continue;
		}

		if(pServerInfo->supportedChannelTypes().contains(szTmp[0]))
		{
			if(it == szTmp)
				it = QString("\r!c\r%1\r").arg(it);
			else
				it = QString("\r!c%1\r%2\r").arg(szTmp, it);
		}
	}
	szMessage = strings.join(" ");
}

void KviChannelWindow::unhighlight()
{
	if(!m_pWindowListItem)
		return;
	m_pWindowListItem->unhighlight();
}

KviIrcConnectionServerInfo * KviChannelWindow::serverInfo()
{
	return connection() ? connection()->serverInfo() : nullptr;
}
