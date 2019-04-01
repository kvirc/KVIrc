//=============================================================================
//
//   File : KviConsoleWindow.cpp
//   Creation date : Sun Jun 25 2000 15:01:34 by Szymon Stefanek
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
#include "KviIrcUrl.h"
#include "KviApplication.h"
#include "KviConsoleWindow.h"
#include "KviMainWindow.h"
#include "KviIconManager.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "KviIrcView.h"
#include "KviControlCodes.h"
#include "KviInput.h"
#include "KviError.h"
#include "KviProxyDataBase.h"
#include "KviNetUtils.h"
#include "KviIrcNetwork.h"
#include "KviIrcServer.h"
#include "KviIrcServerDataBase.h"
#include "KviDnsResolver.h"
#include "KviIrcUserDataBase.h"
#include "KviChannelWindow.h"
#include "KviQueryWindow.h"
#include "KviRegisteredUserDataBase.h"
#include "KviUserListView.h"
#include "KviConfigurationFile.h"
#include "KviIrcToolBar.h"
#include "KviInternalCommand.h"
#include "KviIrcServerParser.h"
#include "KviModuleManager.h"
#include "KviFileUtils.h"
#include "KviTimeUtils.h"
#include "KviMexLinkFilter.h"
#include "KviAvatarCache.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviIrcConnectionServerInfo.h"
#include "KviIrcConnectionStateData.h"
#include "KviIrcConnectionTarget.h"
#include "KviIrcConnectionStatistics.h"
#include "KviAsynchronousConnectionData.h"
#include "KviIrcDataStreamMonitor.h"
#include "KviWindowToolWidget.h"
#include "KviMessageBox.h"
#include "KviKvsScript.h"
#include "KviKvsEventTriggers.h"
#include "KviTalHBox.h"
#include "KviNickColors.h"

#ifdef COMPILE_SSL_SUPPORT
#include "KviSSLMaster.h"
#endif

#include <QToolBar>
#include <QTimer>
#include <QMessageBox>
#include <QStringList>
#include <QCloseEvent>
#include <QRegExp>
#include <QMenu>

#include "kvi_debug.h"

extern KVIRC_API KviIrcServerDataBase * g_pServerDataBase;
extern KVIRC_API KviProxyDataBase * g_pProxyDataBase;

KviConsoleWindow::KviConsoleWindow(int iFlags) : KviWindow(KviWindow::Console, __tr2qs("CONSOLE"), this)
{
	m_pContext = new KviIrcContext(this);

	m_iFlags = iFlags;
	if(m_pContext->id() == 1)
	{
		m_iFlags |= KVI_CONSOLE_FLAG_FIRSTINAPP;
	}

	m_pButtonBox = new KviTalHBox(this);
	m_pButtonBox->setSpacing(0);
	m_pButtonBox->setMargin(0);
	new QLabel(__tr2qs("Address:"), m_pButtonBox);
	m_pAddressEdit = new KviThemedComboBox(m_pButtonBox, this, "url_editor");
	m_pAddressEdit->setAutoCompletion(true);
	m_pAddressEdit->setDuplicatesEnabled(false);
	m_pAddressEdit->setEditable(true);
	m_pAddressEdit->addItem(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Url))), "");
	recentUrlsChanged();
	m_pAddressEdit->setInsertPolicy(QComboBox::NoInsert);
	m_pAddressEdit->setMinimumHeight(24); //icon is 16px, + margins
	m_pButtonBox->setStretchFactor(m_pAddressEdit, 1);
	m_pButtonBox->setObjectName(QLatin1String("kvi_window_button_box"));

	KviTalToolTip::add(m_pAddressEdit, __tr2qs("Current IRC URI"));
	connect(m_pAddressEdit, SIGNAL(activated(const QString &)), this, SLOT(ircUriChanged(const QString &)));
	connect(m_pAddressEdit, SIGNAL(returnPressed(const QString &)), this, SLOT(ircUriChanged(const QString &)));
	connect(g_pApp, SIGNAL(recentUrlsChanged()), this, SLOT(recentUrlsChanged()));

	m_pSplitter = new KviTalSplitter(Qt::Horizontal, this);
	m_pSplitter->setObjectName("console_splitter");
	m_pSplitter->setChildrenCollapsible(false);

	m_pIrcView = new KviIrcView(m_pSplitter, this);
	connect(m_pIrcView, SIGNAL(rightClicked()), this, SLOT(textViewRightClicked()));

	// FIXME: #warning "If notify list is disabled avoid to show this"
	// FIXME: #warning "Button to show/hide the notifyListView (NOT DELETE RE_CREATE!)"
	// The userlist on the right
	//m_pEditorsContainer= new KviToolWindowsContainer(m_pSplitter);
	m_pNotifyViewButton = new KviWindowToolPageButton(KviIconManager::HideListView, KviIconManager::ShowListView, __tr2qs("Notify list"), buttonContainer(), true);
	connect(m_pNotifyViewButton, SIGNAL(clicked()), this, SLOT(toggleNotifyView()));

	m_pNotifyListView = new KviUserListView(m_pSplitter, m_pNotifyViewButton, nullptr, this, 19, __tr2qs("Notify list"), "notify_list_view");

	m_pInput = new KviInput(this, m_pNotifyListView);

	m_pTmpHighLightedChannels = new QStringList;

	applyOptions();
}

void KviConsoleWindow::recentUrlsChanged()
{
	QString cur = m_pAddressEdit->currentText();
	m_pAddressEdit->clear();
	for(auto & it : KVI_OPTION_STRINGLIST(KviOption_stringlistRecentIrcUrls))
	{
		m_pAddressEdit->addItem(*(g_pIconManager->getSmallIcon(KviIconManager::Url)), it);
	}

	int i = m_pAddressEdit->findText(cur);
	if(i != -1)
	{
		m_pAddressEdit->setCurrentIndex(i);
	}
	else
	{
		if(m_pAddressEdit->isEditable())
			m_pAddressEdit->setEditText(cur);
		else
			m_pAddressEdit->setItemText(m_pAddressEdit->currentIndex(), cur);
	}
}

bool KviConsoleWindow::connectionInProgress()
{
	if(context()->asynchronousConnectionData() != nullptr)
		return true;
	if(context()->state() != KviIrcContext::Idle)
		return true;
	return false;
}

KviConsoleWindow::~KviConsoleWindow()
{
	// FIXME: #warning "WARNING : THIS SHOULD BECOME A COMMAND /QUIT $option() so the idents are parsed!"

	// Force connection close: it will just return if no connection is present
	context()->terminateConnectionRequest(true);

	KVS_TRIGGER_EVENT_0(KviEvent_OnIrcContextDestroyed, this);

	//if(m_pLastIrcServer)delete m_pLastIrcServer;

	delete m_pContext;
	m_pContext = nullptr;

	delete m_pTmpHighLightedChannels;
}

void KviConsoleWindow::triggerCreationEvents()
{
	if(m_iFlags & KVI_CONSOLE_FLAG_FIRSTINAPP) // this is the first context in the application
	{
		KVS_TRIGGER_EVENT_0(KviEvent_OnKVIrcStartup, this);

		if(KVI_OPTION_BOOL(KviOption_boolShowTipAtStartup))
			g_pMainWindow->executeInternalCommand(KVI_INTERNALCOMMAND_TIP_OPEN);
	}

	if(m_iFlags & KVI_CONSOLE_FLAG_FIRSTINFRAME)
	{
		KVS_TRIGGER_EVENT_0(KviEvent_OnFrameWindowCreated, this);
	}

	KVS_TRIGGER_EVENT_0(KviEvent_OnIrcContextCreated, this);
}

void KviConsoleWindow::completeChannel(const QString & word, std::vector<QString> & matches)
{
	// FIXME: first look in our context ?
	/*
	if(!connection())return;
	for(auto & c : connection()->channelList())
	{
		if(kvi_strEqualCIN(c.windowName(),word.ptr(),word.len()))matches->append(new KviCString((*it)
	}
	*/
	QStringList * pList = g_pApp->recentChannelsForNetwork(currentNetworkName());
	if(pList)
	{
		for(auto & it : *pList)
		{
			if(KviQString::equalCIN(it, word, word.length()))
				matches.push_back(it);
		}
	}
}

void KviConsoleWindow::completeServer(const QString & word, std::vector<QString> & matches)
{
	for(auto srv : KVI_OPTION_STRINGLIST(KviOption_stringlistRecentServers))
	{
		KviQString::cutToFirst(srv, '/');
		while(srv.startsWith("/"))
			srv.remove(0, 1);
		KviQString::cutFromLast(srv, ':');
		//We should have a full server name here, without the irc:// and without the port
		if(KviQString::equalCIN(srv, word, word.length()))
			matches.push_back(srv);
	}
}

void KviConsoleWindow::getUserTipText(const QString & nick, KviIrcUserEntry * e, QString & buffer)
{
	static QString br("<br>");
	static QString bb("<b>");
	static QString be("</b>");
	static QString cln(":");
	static QString space(" ");
	static QString tds = "<tr><td style=\"background-color: rgb(48,48,48); white-space: pre; font-weight: bold; color: rgb(255,255,255); padding-left: 5px; padding-right: 5px;\">";
	static QString nrs = "<tr><td>";
	static QString enr = "</td></tr>";

	KviRegisteredUserMask * u = g_pRegisteredUserDataBase->findMatchingMask(nick, e->user(), e->host());

	buffer =  "<table>";
	buffer += tds;

	buffer += KviQString::toHtmlEscaped(nick);
	buffer += "!";
	buffer += KviQString::toHtmlEscaped(e->user().isEmpty() ? QString("*") : e->user());
	buffer += "@";
	buffer += KviQString::toHtmlEscaped(e->host().isEmpty() ? QString("*") : e->host());
	buffer += "</font>" + enr;

	if(u)
	{
		QString szComment = u->user()->getProperty("comment");
		if(!szComment.isEmpty())
		{
			buffer += "<tr bgcolor=\"#E0E0E0\"><td>";
			buffer += __tr2qs("Comment");
			buffer += cln + space + "(" + bb;
			buffer += KviQString::toHtmlEscaped(szComment);
			buffer += be + ")" + enr;
		}
	}

	if(e->avatar())
	{
		buffer += QString(nrs + R"(<center><img src="%1" width="%2"></center>)" + enr).arg(e->avatar()->localPath()).arg(e->avatar()->size().width());
	}

	if(e->hasRealName())
	{
		buffer += "<tr><td style=\"white-space: pre\">";
		buffer += __tr2qs("Real name");
		buffer += cln + space + bb;
		buffer += KviQString::toHtmlEscaped(KviControlCodes::stripControlBytes(e->realName()));
		buffer += be + enr;
	}

	if(e->gender() != KviIrcUserEntry::Unknown)
	{
		buffer += nrs;
		buffer += __tr2qs("Gender");
		buffer += cln + space + bb;
		buffer += (e->gender() == KviIrcUserEntry::Male) ? __tr2qs("Male") : __tr2qs("Female");
		buffer += be + enr;
	}

	if(u)
	{
		QString mask;
		u->mask()->mask(mask);
		buffer += nrs;
		buffer += __tr2qs("Registered as");
		buffer += cln + space + bb;
		buffer += KviQString::toHtmlEscaped(u->user()->name());
		buffer += be + br;
		buffer += "Group";
		buffer += cln + space + bb;
		buffer += KviQString::toHtmlEscaped(u->user()->group());
		buffer += be + enr + nrs;
		buffer += __tr2qs("Matched by");
		buffer += cln + space + bb;
		buffer += KviQString::toHtmlEscaped(mask);
		buffer += be + enr;
	}

	if(connection())
	{
		QString chans;
		if(connection()->getCommonChannels(nick, chans, false))
		{
			buffer += nrs;
			buffer += __tr2qs("On");
			buffer += cln + space + bb;
			buffer += KviQString::toHtmlEscaped(chans);
			buffer += be + enr;
		}
	}

	if(e->hasServer())
	{
		buffer += "<tr><td style=\"white-space: pre\">";
		buffer += __tr2qs("Using server: <b>%1</b>").arg(KviQString::toHtmlEscaped(e->server()));

		if(e->hasHops())
		{
			buffer += R"(<tr><td bgcolor="#E0E0E0"><font color="#000000">)";
			buffer += __tr2qs("Hops: <b>%1</b>").arg(e->hops());
			buffer += "</font>" + enr;
		}
		else
		{
			buffer += enr;
			buffer += "</table>";
		}
	}

	if(e->hasAccountName())
	{
		buffer += R"(<tr><td bgcolor="#E0E0E0"><font color="#000000">)";
		buffer += __tr2qs("Identified to account: <b>%1</b>").arg(e->accountName());
		buffer += "</font>" + enr;
	}

	if(e->isAway())
	{
		buffer += R"(<tr><td width="100%" bgcolor="#E0E0E0"><font color="#000000">)";
		buffer += __tr2qs("Probably away");
		buffer += "</font>" + enr;
	}
}

void KviConsoleWindow::toggleNotifyView()
{
	showNotifyList(!m_pNotifyListView->isVisible());
}

void KviConsoleWindow::executeInternalCommand(int index)
{
	KviKvsScript::run(kvi_getInternalCommandBuffer(index), this);
}

void KviConsoleWindow::saveProperties(KviConfigurationFile * cfg)
{
	KviWindow::saveProperties(cfg);
	cfg->writeEntry("Splitter", m_pNotifyViewButton->isChecked() ? m_pSplitter->sizes() : m_SplitterSizesList);
	cfg->writeEntry("NotifyListViewVisible", m_pNotifyViewButton->isChecked());
}

void KviConsoleWindow::getBaseLogFileName(QString & buffer)
{
	if(context()->connection())
		buffer = context()->connection()->target()->network()->name();
	else
		buffer = context()->id();
}

void KviConsoleWindow::showNotifyList(bool bShow, bool bIgnoreSizeChange)
{
	if(bShow)
	{
		m_pNotifyListView->show();
		if(!(m_pNotifyViewButton->isChecked()))
			m_pNotifyViewButton->setChecked(true);

		m_pSplitter->setSizes(m_SplitterSizesList);
	}
	else
	{
		if(!bIgnoreSizeChange)
			m_SplitterSizesList = m_pSplitter->sizes();

		m_pNotifyListView->hide();
		if(m_pNotifyViewButton->isChecked())
			m_pNotifyViewButton->setChecked(false);
	}
}

void KviConsoleWindow::loadProperties(KviConfigurationFile * cfg)
{
	int iWidth = width();
	QList<int> def;
	def.append((iWidth * 75) / 100);
	def.append((iWidth * 25) / 100);
	m_SplitterSizesList = cfg->readIntListEntry("Splitter", def);
	m_pSplitter->setStretchFactor(0, 1);

	KviWindow::loadProperties(cfg);
	showNotifyList(cfg->readBoolEntry("NotifyListViewVisible", false), true);
}

void KviConsoleWindow::textViewRightClicked()
{
	KVS_TRIGGER_EVENT_0(KviEvent_OnConsolePopupRequest, this);
}

KviWindow * KviConsoleWindow::activeWindow()
{
	if(!g_pActiveWindow)
		return this;
	if(g_pActiveWindow->console() == this)
		return g_pActiveWindow;
	return this;
}

void KviConsoleWindow::ircUriChanged(const QString & text)
{
	int iStatus = KviIrcUrl::run(text, KviIrcUrl::CurrentContext, this);
	if(iStatus & KviIrcUrl::InvalidProtocol || iStatus & KviIrcUrl::InvalidUrl)
	{
		KviMessageBox::warning(__tr2qs("KVIrc can only recognize irc://, irc6://, ircs:// or ircs6:// URL's\n"
		                               "The URL you provided is invalid. Check spelling and try again"));
	}
	m_pInput->setFocus();
}

void KviConsoleWindow::updateUri()
{
	QString uri;
	if(connection())
	{
		KviIrcServer * server = connection()->target()->server();
		if(server)
		{
			KviIrcUrl::join(uri, server);
			if(connection()->channelList().size())
			{
				KviChannelWindow * last = connection()->channelList().back();
				for(auto & c : connection()->channelList())
				{
					uri.append(c->target());
					if(c->hasChannelMode('k'))
					{
						uri.append("?");
						uri.append(c->channelModeParam('k'));
					}
					if(c != last)
						uri.append(",");
				}
			}
		}
	}

	int i = m_pAddressEdit->findText(uri);
	if(i != -1)
	{
		m_pAddressEdit->setCurrentIndex(i);
	}
	else
	{
		if(m_pAddressEdit->isEditable())
			m_pAddressEdit->setEditText(uri);
		else
			m_pAddressEdit->setItemText(m_pAddressEdit->currentIndex(), uri);
	}

	m_pAddressEdit->lineEdit()->setCursorPosition(0);
}

void KviConsoleWindow::connectionAttached()
{
	//need to update URI
	connect(m_pContext->connection(), SIGNAL(chanListChanged()), this, SLOT(updateUri()));
	updateUri();
	m_pNotifyListView->setUserDataBase(connection()->userDataBase());

	// Update log file name
	if(KVI_OPTION_BOOL(KviOption_boolAutoLogConsole))
	{
		if (m_pIrcView->isLogging())
			m_pIrcView->stopLogging();
		m_pIrcView->startLogging();
	}
}

void KviConsoleWindow::connectionDetached()
{
	//need to update URI?
	m_pNotifyListView->partAll();
	m_pNotifyListView->setUserDataBase(nullptr); // this is rather for crash tests
}

void KviConsoleWindow::closeEvent(QCloseEvent * e)
{
	if(g_pMainWindow->consoleCount() > 1)
	{
		// there are other consoles beside this one
		if(context()->state() == KviIrcContext::Connected)
		{
			if(!KVI_OPTION_BOOL(KviOption_boolAlwaysDisconnectClosingConnectedConsole))
			{
				switch(QMessageBox::warning(this,
				    __tr2qs("Confirm Close - KVIrc"),
				    __tr2qs("You have just attempted to close a console window with an active connection inside.\n"
				            "Are you sure you wish to terminate the connection?"),
				    __tr2qs("&Yes"),
				    __tr2qs("&Always"),
				    __tr2qs("&No"),
				    2, 2))
				{
					case 0:
						// nothing here
						break;
					case 1:
						KVI_OPTION_BOOL(KviOption_boolAlwaysDisconnectClosingConnectedConsole) = true;
						break;
					default: // 2 = no
						e->ignore();
						return;
						break;
				}
			}
			// ask the context to terminate the connection gracefully
			context()->terminateConnectionRequest(false);
			// the close event will recall terminateConnectionRequest()
			// to brutally interrupt it in a while
		}

		// just close
		KviWindow::closeEvent(e);
		return;
	}

	// this is the only console... ask if the user really wants to quit KVirc
	if(!KVI_OPTION_BOOL(KviOption_boolAlwaysQuitKVIrcClosingLastConsole))
	{
		switch(QMessageBox::warning(this,
		    __tr2qs("Confirm Close - KVIrc"),
		    __tr2qs("You have just attempted to close the last console window.\nAre you sure you wish to quit KVIrc?"),
		    __tr2qs("&Always"),
		    __tr2qs("&Yes"),
		    __tr2qs("&No"),
		    2, 2))
		{
			case 0:
				KVI_OPTION_BOOL(KviOption_boolAlwaysQuitKVIrcClosingLastConsole) = true;
				break;
			case 1:
				// nothing here
				break;
			default: // 2 = no
				e->ignore();
				return;
				break;
		}
	}

	g_pApp->quit();
}

// internal helper for applyHighlighting
int KviConsoleWindow::triggerOnHighlight(KviWindow * pWnd, int iType, const QString & szNick, const QString & szUser, const QString & szHost, const QString & szMsg, const QString & szTrigger)
{
	KviRegisteredUser * u = connection()->userDataBase()->registeredUser(szNick, szUser, szHost);
	if(u)
	{
		if(u->isIgnoreEnabledFor(KviRegisteredUser::Highlight))
			return iType;
	}
	if(!KVI_OPTION_STRING(KviOption_stringOnHighlightedMessageSound).isEmpty() && pWnd && !pWnd->hasAttention())
	{
		KviKvsVariantList soundParams{new KviKvsVariant{KVI_OPTION_STRING(KviOption_stringOnHighlightedMessageSound)}};
		KviKvsScript::run("snd.play $0", nullptr, &soundParams);
	}

	QString szMessageType = QString("%1").arg(iType);

	if(KVS_TRIGGER_EVENT_7_HALTED(KviEvent_OnHighlight,
	    pWnd, szNick, szUser, szHost,
	    szMsg, szTrigger,
	    szMessageType, (iType == KVI_OUT_ACTION || iType == KVI_OUT_ACTIONCRYPTED)))
		return -1;
	return KVI_OUT_HIGHLIGHT;
}

void KviConsoleWindow::addHighlightedChannel(const QString & szChan)
{
	if(m_pTmpHighLightedChannels->contains(szChan, Qt::CaseInsensitive))
		return;
	else
		m_pTmpHighLightedChannels->append(szChan);
}

void KviConsoleWindow::removeHighlightedChannel(const QString & szChan)
{
	m_pTmpHighLightedChannels->removeOne(szChan);
}

// if it returns -1 you should just return and not display the message
int KviConsoleWindow::applyHighlighting(KviWindow * wnd, int type, const QString & nick, const QString & user, const QString & host, const QString & szMsg)
{
	QString szPattern = KVI_OPTION_STRING(KviOption_stringWordSplitters);
	QString szSource;
	QString szStripMsg = KviControlCodes::stripControlBytes(szMsg);
	QRegExp rgxHlite;
	Qt::CaseSensitivity cs = KVI_OPTION_BOOL(KviOption_boolCaseSensitiveHighlighting) ? Qt::CaseSensitive : Qt::CaseInsensitive;

	if(KVI_OPTION_BOOL(KviOption_boolAlwaysHighlightNick) && connection())
	{
		if(KVI_OPTION_BOOL(KviOption_boolUseFullWordHighlighting))
		{
			if(szStripMsg.contains(connection()->userInfo()->nickName(), cs))
				return triggerOnHighlight(wnd, type, nick, user, host, szMsg, connection()->userInfo()->nickName());
		}
		else
		{
			if(!szPattern.isEmpty())
				rgxHlite.setPattern(
				    QString("(?:[%1]|\\s|^)%2(?:[%1]|\\s|$)").arg(QRegExp::escape(szPattern), QRegExp::escape(connection()->userInfo()->nickName())));
			else
				rgxHlite.setPattern(
				    QString("(?:\\s|^)%1(?:\\s|$)").arg(QRegExp::escape(connection()->userInfo()->nickName())));
			rgxHlite.setCaseSensitivity(cs);
			if(szStripMsg.contains(rgxHlite))
				return triggerOnHighlight(wnd, type, nick, user, host, szMsg, connection()->userInfo()->nickName());
		}
	}

	if(KVI_OPTION_BOOL(KviOption_boolUseWordHighlighting))
	{
		for(auto & it : KVI_OPTION_STRINGLIST(KviOption_stringlistHighlightWords))
		{
			if(it.isEmpty())
				continue;

			if(KVI_OPTION_BOOL(KviOption_boolUseFullWordHighlighting))
			{
				if(szStripMsg.contains(it, cs))
					return triggerOnHighlight(wnd, type, nick, user, host, szMsg, it);
			}
			else
			{
				if(!szPattern.isEmpty())
					rgxHlite.setPattern(
					    QString("(?:[%1]|\\s|^)%2(?:[%1]|\\s|$)").arg(QRegExp::escape(szPattern), QRegExp::escape(it)));
				else
					rgxHlite.setPattern(
					    QString("(?:\\s|^)%1(?:\\s|$)").arg(QRegExp::escape(it)));
				rgxHlite.setCaseSensitivity(cs);
				if(szStripMsg.contains(rgxHlite))
					return triggerOnHighlight(wnd, type, nick, user, host, szMsg, it);
			}
		}
	}

	if(wnd->type() == KviWindow::Channel)
	{
		if(((KviChannelWindow *)wnd)->isHighlightedUser(nick) || isHighlightedChannel(wnd->windowName()))
			return triggerOnHighlight(wnd, type, nick, user, host, szMsg, nick);

		// FIXME: this is for userhighlighing
		//        maybe mark the users as highlighted in the console user database
		//        and then lookup them there ? this would be potentially a lot faster
		KviRegisteredUser * u = connection()->userDataBase()->registeredUser(nick, user, host);

		// note that we're highlighting users only in channels since
		// in a query (or DCC) highlighting the remote end is senseless.
		if(u)
		{
			if(u->getBoolProperty("highlight"))
				return triggerOnHighlight(wnd, type, nick, user, host, szMsg, nick);
		}
	}

	return type;
}

void KviConsoleWindow::outputPrivmsg(KviWindow * wnd,
    int type,
    const QString & daNick,
    const QString & daUser,
    const QString & daHost,
    const QString & msg,
    int iFlags,
    const QString & prefix,
    const QString & suffix,
    const QDateTime & datetime)
{
	// FIXME: #warning "THIS IS USED BY WINDOWS THAT ARE NOT BOUND TO THIS IRC CONTEXT"
	// FIXME: #warning "REMEMBER IT IN ESCAPE COMMANDS"
	// KVI_ASSERT(wnd);

	bool bIsChan = (wnd->type() == KviWindow::Channel);
	bool bIsNotice = ((type == KVI_OUT_CHANNELNOTICE) || (type == KVI_OUT_CHANNELNOTICECRYPTED)
	    || (type == KVI_OUT_QUERYNOTICE) || (type == KVI_OUT_QUERYNOTICECRYPTED));

	QString nick = daNick; // not that beautiful.. :/
	QString user = daUser;
	QString host = daHost;

	if(connection())
	{
		if(nick.isEmpty())
			nick = connection()->userInfo()->nickName();
		if(user.isEmpty())
			user = connection()->userInfo()->userName();
		if(host.isEmpty())
			host = connection()->userInfo()->hostName();
	}

	QString szDecodedMessage = msg; // shallow copy

	if(KVI_OPTION_BOOL(KviOption_boolStripMircColorsInUserMessages))
		szDecodedMessage = KviControlCodes::stripControlBytes(szDecodedMessage);

	if(!(iFlags & NoHighlighting))
	{
		// HIGHLIGHTING BLOCK
		int iSaveType = type;
		type = applyHighlighting(wnd, type, nick, user, host, szDecodedMessage);
		if(type < 0)
			return; // event stopped the message!

		if(type == KVI_OUT_HIGHLIGHT)
		{
			if(!wnd->hasAttention(KviWindow::MainWindowIsVisible))
			{
				if(KVI_OPTION_BOOL(KviOption_boolFlashWindowOnHighlightedMessages) && (!(iFlags & NoWindowFlashing)))
				{
					wnd->demandAttention();
				}
				if(KVI_OPTION_BOOL(KviOption_boolPopupNotifierOnHighlightedMessages) && (!(iFlags & NoNotifier)))
				{
					QString szMsg = "<b>&lt;";
					szMsg += nick;
					szMsg += "&gt;</b> ";
					szMsg += KviQString::toHtmlEscaped(szDecodedMessage);
					//qDebug("KviConsoleWindow.cpp:629 debug: %s",szMsg.data());
					g_pApp->notifierMessage(wnd, KVI_OPTION_MSGTYPE(iSaveType).pixId(), szMsg, KVI_OPTION_UINT(KviOption_uintNotifierAutoHideTime));
				}
			}
		}
	}

	// <PREFIX>nick[!user@host]<POSTFIX>This is a test message

	QString szNick = QString("\r!nc\r%1\r").arg(nick);

	if(KVI_OPTION_BOOL(KviOption_boolShowUserAndHostInPrivmsgView))
		KviQString::appendFormatted(szNick, "!%Q@\r!h\r%Q\r", &user, &host);

	if(bIsChan && KVI_OPTION_BOOL(KviOption_boolShowChannelUserFlagInPrivmsgView))
		((KviChannelWindow *)wnd)->prependUserFlag(nick, szNick);

	if(KVI_OPTION_BOOL(KviOption_boolColorNicks) && connection())
	{
		if(KVI_OPTION_BOOL(KviOption_boolUseSpecifiedSmartColorForOwnNick) && QString::compare(nick, connection()->userInfo()->nickName(), Qt::CaseSensitive) == 0)
		{
			// it's me
			szNick.prepend(m_szOwnSmartColor);
		}
		else
		{
			//search for a cached entry
			KviIrcUserEntry * pUserEntry = connection()->userDataBase()->find(nick);
			if(pUserEntry)
			{
				int sum = pUserEntry->smartNickColor();
				if(sum < 0)
				{
					// cache miss, create entry
					sum = KviNickColors::getSmartColorForNick(&nick);
					if(KVI_OPTION_BOOL(KviOption_boolUseSpecifiedSmartColorForOwnNick))
					{
						//avoid the use of the color specifier for own nickname
						if(m_szOwnSmartColor == KviNickColors::getSmartColor(sum, KVI_OPTION_BOOL(KviOption_boolColorNicksWithBackground)))
							sum++;
					}
					pUserEntry->setSmartNickColor(sum);
				}

				szNick.prepend(KviNickColors::getSmartColor(sum, KVI_OPTION_BOOL(KviOption_boolColorNicksWithBackground)));
			}
			else
			{
				/*
				 * Received a message from a user not in userDataBase: how can this happen?
				 * - services replaying some log
				 * - user non joined msging a channel -n
				 * anyway, better fallback than wrong
				 */
				int sum = KviNickColors::getSmartColorForNick(&nick);
				if(KVI_OPTION_BOOL(KviOption_boolUseSpecifiedSmartColorForOwnNick))
				{
					//avoid the use of the color specifier for own nickname
					if(m_szOwnSmartColor == KviNickColors::getSmartColor(sum, KVI_OPTION_BOOL(KviOption_boolColorNicksWithBackground)))
						sum++;
				}
				szNick.prepend(KviNickColors::getSmartColor(sum, KVI_OPTION_BOOL(KviOption_boolColorNicksWithBackground)));
			}
		}
		szNick.prepend(KviControlCodes::Color);
		szNick.append(KviControlCodes::Color);
	}

	if(KVI_OPTION_BOOL(KviOption_boolBoldedNicks))
	{
		szNick.prepend(KviControlCodes::Bold);
		szNick.append(KviControlCodes::Bold);
	}

	QString szMessage;

	if(KVI_OPTION_BOOL(KviOption_boolUseExtendedPrivmsgView))
	{
		szMessage = prefix.isEmpty() ? KVI_OPTION_STRING(KviOption_stringExtendedPrivmsgPrefix) : prefix;
		szMessage += szNick;
		szMessage += suffix.isEmpty() ? KVI_OPTION_STRING(KviOption_stringExtendedPrivmsgPostfix) : suffix;
	}
	else
	{
		if(bIsNotice)
		{
			static QString pre1("*");
			static QString suf1("* ");
			szMessage =  prefix.isEmpty() ? pre1 : prefix;
			szMessage += szNick;
			szMessage += suffix.isEmpty() ? suf1 : suffix;
		}
		else
		{
			static QString pre2("<");
			static QString suf2("> ");
			szMessage =  prefix.isEmpty() ? pre2 : prefix;
			szMessage += szNick;
			szMessage += suffix.isEmpty() ? suf2 : suffix;
		}
	}

	szMessage += szDecodedMessage;

	if(bIsChan)
		((KviChannelWindow *)wnd)->outputMessage(type, szMessage, datetime);
	else
		wnd->outputNoFmt(type, szMessage, 0, datetime);
}

void KviConsoleWindow::avatarChangedUpdateWindows(const QString & nick, const QString & textLine)
{
	if(!connection())
		return; //ops...

	// in quiet mode avoid bugging the user about avatar changes
	bool bOut = ((!textLine.isEmpty()) && (!(_OUTPUT_QUIET)));

	for(auto & c : connection()->channelList())
	{
		if(c->avatarChanged(nick))
		{
			if(bOut)
				c->outputNoFmt(KVI_OUT_AVATAR, textLine);
		}
	}
	for(auto & q : connection()->queryList())
	{
		if(q->avatarChanged(nick))
		{
			if(bOut)
				q->outputNoFmt(KVI_OUT_AVATAR, textLine);
		}
	}
	m_pNotifyListView->avatarChanged(nick); // recalc the item height here too!
}

void KviConsoleWindow::avatarChanged(KviAvatar * avatar, const QString & nick, const QString & user, const QString & host, const QString & textLine)
{
	if(!connection())
		return; //ops...

	bool bRegisteredStuff = false;

	if(KVI_OPTION_BOOL(KviOption_boolSetLastAvatarAsDefaultForRegisteredUsers))
	{
		// Don't even try to do it for myself
		if(!KviQString::equalCI(nick, connection()->userInfo()->nickName()))
		{
			KviRegisteredUser * u = connection()->userDataBase()->registeredUser(nick, user, host);
			if(u)
			{
				if(avatar)
					u->setProperty("avatar", avatar->identificationString());
				else
					u->setProperty("avatar", QString());
				bRegisteredStuff = true;
			}
		}
	}

	if(!bRegisteredStuff)
	{
		// cache it
		if(avatar)
			KviAvatarCache::instance()->replace(avatar->identificationString(), KviIrcMask(nick, user, host), currentNetworkName().toUtf8().data());
		else
			KviAvatarCache::instance()->remove(KviIrcMask(nick, user, host), currentNetworkName().toUtf8().data());
	}

	avatarChangedUpdateWindows(nick, textLine);
}

void KviConsoleWindow::checkDefaultAvatar(KviIrcUserEntry * e, const QString & nick, const QString & user, const QString & host)
{
	// look it up in the cache
	QString szAvatar = KviAvatarCache::instance()->lookup(KviIrcMask(nick, user, host), currentNetworkName());
	if(!szAvatar.isEmpty())
	{
		// got a cache hit... is it on disk ?
		KviAvatar * avatar = g_pIconManager->getAvatar(QString(), szAvatar);
		if(avatar)
		{
			// cached image on disk
			e->setAvatar(avatar);
			avatarChangedUpdateWindows(nick, QString());
			return;
		}
		// no cached image on disk.. will need to requery it anyway
		// remove from cache
		KviAvatarCache::instance()->remove(KviIrcMask(nick, user, host), currentNetworkName());
	}

	// registered ?
	KviRegisteredUser * u = connection()->userDataBase()->registeredUser(nick, user, host);
	if(u)
	{
		// the user is registered...
		QString szAvatar;
		if(u->getProperty("avatar", szAvatar))
		{
			// the user has a default avatar...
			KviAvatar * avatar = g_pIconManager->getAvatar(szAvatar, KviFileUtils::extractFileName(szAvatar));
			if(avatar)
			{
				e->setAvatar(avatar);
				avatarChangedUpdateWindows(nick, QString());
				return;
			}
		}
	}
}

void KviConsoleWindow::resetAvatarForMatchingUsers(KviRegisteredUser * u)
{
	if(!connection())
		return;

	QString szAvatar;
	if(!u->getProperty("avatar", szAvatar))
		return;

	KviPointerHashTableIterator<QString, KviIrcUserEntry> it(*(connection()->userDataBase()->dict()));
	while(KviIrcUserEntry * e = it.current())
	{
		if(e->hasHost())
		{
			if(u->matchesFixed(it.currentKey(), e->user(), e->host()))
			{
				KviAvatar * a = g_pIconManager->getAvatar(QString(), szAvatar);
				e->setAvatar(a);
				avatarChangedUpdateWindows(it.currentKey(), QString());
			}
		}
		++it;
	}
}

KviAvatar * KviConsoleWindow::setAvatar(const QString & nick, const QString & user, const QString & host, const QString & szLocalPath, const QString & szName)
{
	if(!connection())
		return nullptr;
	KviIrcUserEntry * e = connection()->userDataBase()->find(nick);
	if(e)
	{
		// User and host must match
		if((!user.isEmpty()) && e->hasUser())
		{
			if(!KviQString::equalCI(user, e->user()))
				return nullptr;
		}

		if((!host.isEmpty()) && e->hasHost())
		{
			if(!KviQString::equalCI(host, e->host()))
				return nullptr;
		}

		// Ok...got it
		KviAvatar * avatar = g_pIconManager->getAvatar(szLocalPath, szName);
		if(avatar)
		{
			e->setAvatar(avatar);
			avatarChanged(avatar, nick, user, host, QString());
			return avatar;
		}
		else
		{
			if(_OUTPUT_PARANOIC)
				output(KVI_OUT_VERBOSE, __tr2qs("Failed to load avatar with name \"%Q\" and local path \"%Q\""), &szName, &szLocalPath);
		}
	}
	return nullptr;
}

KviAvatar * KviConsoleWindow::defaultAvatarFromOptions()
{
	QPixmap * avatar = KVI_OPTION_PIXMAP(KviOption_pixmapMyAvatar).pixmap();

	if(!avatar)
		return nullptr;

	if(avatar->isNull())
		return nullptr;

	if(KVI_OPTION_STRING(KviOption_stringMyAvatar).isEmpty())
		return nullptr;

	KviAvatar * loadedAvatar = new KviAvatar(KVI_OPTION_PIXMAP(KviOption_pixmapMyAvatar).path(), KVI_OPTION_STRING(KviOption_stringMyAvatar));

	if(loadedAvatar->isValid())
		return loadedAvatar;

	delete loadedAvatar;
	return nullptr;
}

KviAvatar * KviConsoleWindow::currentAvatar()
{
	if(!connection())
		return nullptr;

	KviIrcUserEntry * e = connection()->userDataBase()->find(connection()->userInfo()->nickName());

	if(!e)
		return nullptr;

	KviAvatar * a = e->avatar();

	if(!a)
	{
		a = defaultAvatarFromOptions();
		if(a)
		{
			e->setAvatar(a);
			avatarChanged(a, connection()->userInfo()->nickName(), QString(), QString(), QString());
		}
	}

	return a;
}

void KviConsoleWindow::setAvatarFromOptions()
{
	if(!connection())
		return;
	KviIrcUserEntry * e = connection()->userDataBase()->find(connection()->userInfo()->nickName());

	if(!e)
		return;

	// a=0 => avatar unset
	KviAvatar * a = defaultAvatarFromOptions();
	e->setAvatar(a);
	avatarChanged(a, connection()->userInfo()->nickName(), QString(), QString(), QString());
}

void KviConsoleWindow::applyOptions()
{
	m_pAddressEdit->applyOptions();
	m_pNotifyListView->applyOptions();
	m_pInput->applyOptions();
	m_pIrcView->applyOptions();

	KviWindow::applyOptions();

	// trick
	resize(width() - 1, height() - 1);
	resize(width() + 1, height() + 1);

	if(KVI_OPTION_BOOL(KviOption_boolUseSpecifiedSmartColorForOwnNick))
	{
		int iBack = KVI_OPTION_UINT(KviOption_uintUserIrcViewOwnBackground);
		if(iBack == KviControlCodes::Transparent)
		{
			m_szOwnSmartColor = QString("%1").arg(KVI_OPTION_UINT(KviOption_uintUserIrcViewOwnForeground));
		}
		else
		{
			m_szOwnSmartColor = QString("%1,%2").arg(KVI_OPTION_UINT(KviOption_uintUserIrcViewOwnForeground)).arg(iBack);
		}
	}
}

void KviConsoleWindow::resizeEvent(QResizeEvent *)
{
	//qDebug("Console window resize event %d,%d",width(),height());
	int hght = m_pInput->heightHint();
	int hght2 = m_pButtonBox->sizeHint().height();
	m_pButtonBox->setGeometry(0, 0, width(), hght2);
	m_pSplitter->setGeometry(0, hght2, width(), height() - (hght + hght2));
	m_pInput->setGeometry(0, height() - hght, width(), hght);
}

QSize KviConsoleWindow::sizeHint() const
{
	QSize ret(m_pIrcView->sizeHint().height(), m_pIrcView->sizeHint().height() + m_pInput->heightHint());
	return ret;
}

void KviConsoleWindow::fillStatusString()
{
	switch(context()->state())
	{
		case KviIrcContext::Idle:
			m_szStatusString = __tr2qs("No connection");
			break;
		case KviIrcContext::PendingReconnection:
			m_szStatusString = __tr2qs("Waiting to reconnect...");
			break;
		case KviIrcContext::Connecting:
			m_szStatusString = __tr2qs("Connection in progress...");
			break;
		case KviIrcContext::LoggingIn:
			m_szStatusString = __tr2qs("Login in progress...");
			break;
		case KviIrcContext::Connected:
			m_szStatusString = connection()->userInfo()->nickName();
			if(!connection()->userInfo()->userMode().isEmpty())
			{
				m_szStatusString += " (+";
				m_szStatusString += connection()->userInfo()->userMode();

				if(connection()->userInfo()->isAway())
				{
					m_szStatusString += QChar(' ');
					m_szStatusString += __tr2qs("away");
				}
				m_szStatusString += QChar(')');
			}
			else
			{
				if(connection()->userInfo()->isAway())
				{
					m_szStatusString += " (";
					m_szStatusString += __tr2qs("away");
					m_szStatusString += QChar(')');
				}
			}

			m_szStatusString += __tr2qs(" on ");
			m_szStatusString += connection()->serverInfo()->name();
			break;
	}
}

void KviConsoleWindow::fillCaptionBuffers()
{
	fillStatusString();

	m_szPlainTextCaption = windowName();
	m_szPlainTextCaption += " [";
	m_szPlainTextCaption += m_szStatusString;
	m_szPlainTextCaption += QChar(']');
}

QPixmap * KviConsoleWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(isConnected() ? KviIconManager::Links : KviIconManager::Console);
}

void KviConsoleWindow::getWindowListTipText(QString & buffer)
{
	fillStatusString();

	static QString html_channel(__tr2qs("channel"));
	static QString html_channels(__tr2qs("channels"));
	static QString html_query(__tr2qs("query"));
	static QString html_queries(__tr2qs("queries"));
	static QString html_bold("<b>");
	static QString html_tab("&nbsp;");
	static QString html_eofbold("</b>");
	static QString html_hrbr("<br><hr>");
	static QString html_cln(":");
	static QString html_space(" ");
	static QString html_commaspace(", ");
	static QString html_br("<br>");
	static QString html_spaceparopen(" (");
	static QString html_spaceparclosed(")");
	static QString nrs = "<tr><td>";
	static QString enr = "</td></tr>";

	// no wrapping contents
	buffer = "<table style=\"white-space: pre\">" START_TABLE_BOLD_ROW;
	buffer += m_szStatusString;
	buffer += END_TABLE_BOLD_ROW;
	if((context()->state() == KviIrcContext::Connected) && connection())
	{
		QString num;
		unsigned int uD, uH;

		uD = connection()->channelList().size();
		uH = connection()->queryList().size();

		if(uD || uH > 0)
		{
			buffer += nrs;
			buffer += html_tab;

			if(uD > 0)
			{
				num.setNum(uD);

				buffer += html_bold;
				buffer += num;
				buffer += html_eofbold;
				buffer += html_space;
				buffer += uD > 1 ? html_channels : html_channel;
				if(uH > 0)
					buffer += html_br;
			}

			if(uH > 0)
			{
				num.setNum(uH);

				buffer += html_tab;
				buffer += html_bold;
				buffer += num;
				buffer += html_eofbold;
				buffer += html_space;
				buffer += uH > 1 ? html_queries : html_query;
			}

			buffer += enr;
		}

		QString szTmp;
		QDateTime date;
		date.setTime_t(connection()->statistics()->connectionStartTime());
		switch(KVI_OPTION_UINT(KviOption_uintOutputDatetimeFormat))
		{
			case 0:
				// this is the equivalent to an empty date.toString() call, but it's needed
				// to ensure qt4 will use the default() locale and not the system() one
				szTmp = QLocale().toString(date, "ddd MMM d hh:mm:ss yyyy");
				break;
			case 1:
				szTmp = date.toString(Qt::ISODate);
				break;
			case 2:
				szTmp = date.toString(Qt::SystemLocaleShortDate);
				break;
		}

		buffer += nrs;

		buffer += __tr2qs("Connected since");
		buffer += html_cln;
		buffer += html_space;
		buffer += html_bold;
		buffer += szTmp;
		buffer += html_eofbold;
		buffer += html_br;

		QString tspan = KviTimeUtils::formatTimeInterval((unsigned int)(kvi_secondsSince(connection()->statistics()->connectionStartTime())),
		    KviTimeUtils::NoLeadingEmptyIntervals | KviTimeUtils::NoLeadingZeroes);

		buffer += __tr2qs("Online for");
		buffer += html_cln;
		buffer += html_space;
		buffer += html_bold;
		buffer += tspan;
		buffer += html_eofbold;

		buffer += enr + R"(<tr><td bgcolor="#E0E0E0"><font color="#000000">)";

		tspan = KviTimeUtils::formatTimeInterval((unsigned int)(kvi_secondsSince(connection()->statistics()->lastMessageTime())),
		    KviTimeUtils::NoLeadingEmptyIntervals | KviTimeUtils::NoLeadingZeroes);

		buffer += __tr2qs("Server idle for");
		buffer += html_cln;
		buffer += html_space;
		buffer += html_bold;
		buffer += tspan;
		buffer += html_eofbold;
		buffer += "</font>" + enr;
	}

	buffer += "</table>";
}
