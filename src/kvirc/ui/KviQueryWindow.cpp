//=============================================================================
//
//   File : KviQueryWindow.cpp
//   Creation date : Tue Aug 7 2000 14:23:22 by Szymon Stefanek
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

#include "kvi_settings.h"
#include "kvi_out.h"
#include "KviQueryWindow.h"
#include "KviConsoleWindow.h"
#include "KviIconManager.h"
#include "KviIrcView.h"
#include "KviInput.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "KviIrcSocket.h"
#include "KviConfigurationFile.h"
#include "KviControlCodes.h"
#include "KviThemedLabel.h"
#include "KviUserAction.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviIrcServerParser.h"
#include "KviIrcUserDataBase.h"
#include "KviWindowToolWidget.h"
#include "KviKvsEventTriggers.h"
#include "KviTalHBox.h"

#ifdef COMPILE_CRYPT_SUPPORT
#include "KviCryptEngine.h"
#include "KviCryptController.h"
#endif //COMPILE_CRYPT_SUPPORT

#include <QByteArray>
#include <QDate>
#include <QDir>
#include <QFileInfo>
#include <QList>
#include <QPixmap>
#include <QString>

KviQueryWindow::KviQueryWindow(KviConsoleWindow * lpConsole, const QString & szNick)
    : KviWindow(KviWindow::Query, szNick, lpConsole)
{
	m_iFlags = 0;
	connection()->registerQuery(this);

	//m_pTopSplitter = new KviTalSplitter(QSplitter::Horizontal,this,"top_splitter");
	m_pButtonBox = new KviTalHBox(this);
	m_pButtonBox->setSpacing(0);
	m_pButtonBox->setMargin(0);
	m_pLabel = new KviThemedLabel(m_pButtonBox, this, "query_label");
	updateLabelText();
	m_pButtonBox->setStretchFactor(m_pLabel, 1);

	// The button box on the right
	//KviTalHBox * box = new KviTalHBox(m_pTopSplitter,"button_box");
	m_pButtonGrid = (QFrame *)new KviTalHBox(m_pButtonBox);

	createTextEncodingButton(m_pButtonGrid);

	m_pSplitter = new KviTalSplitter(Qt::Horizontal, this);
	m_pSplitter->setObjectName("query_splitter");
	m_pSplitter->setChildrenCollapsible(false);

	m_pIrcView = new KviIrcView(m_pSplitter, this);
	connect(m_pIrcView, SIGNAL(rightClicked()), this, SLOT(textViewRightClicked()));
	//m_pEditorsContainer= new KviToolWindowsContainer(m_pSplitter);

	m_pListViewButton = new KviWindowToolPageButton(KviIconManager::HideListView, KviIconManager::ShowListView, __tr2qs("Show user list"), buttonContainer(), true);
	m_pListViewButton->setObjectName("list_view_button");
	connect(m_pListViewButton, SIGNAL(clicked()), this, SLOT(toggleListView()));

#ifdef COMPILE_CRYPT_SUPPORT
	createCryptControllerButton(m_pButtonGrid);
#endif

	m_pUserListView = new KviUserListView(m_pSplitter, m_pListViewButton, connection()->userDataBase(), this, 7, __tr2qs("Query targets"), "user_list_view");

	m_pInput = new KviInput(this, m_pUserListView);

	if(KVI_OPTION_BOOL(KviOption_boolPasteLastLogOnQueryJoin))
		pasteLastLog();

	if(KVI_OPTION_BOOL(KviOption_boolAutoLogQueries))
		m_pIrcView->startLogging();
	// FIXME: #warning "Maybe tell the user all that we know about the remote szEnd(s)....channels..."

	m_pIrcView->enableDnd(true);
	connect(m_pIrcView, SIGNAL(fileDropped(const QString &)), this, SLOT(slotDndEvents(const QString &)));

	updateCaption();
}

KviQueryWindow::~KviQueryWindow()
{
	m_pUserListView->partAll();
	if(type() == KviWindow::DeadQuery)
	{
		if(context())
			context()->unregisterDeadQuery(this);
	}
	else
	{
		if(connection())
			connection()->unregisterQuery(this);
	}
}

void KviQueryWindow::updateLabelText()
{
	QString szText = getInfoLabelText();
	if(szText != m_pLabel->text())
	{
		m_pLabel->setText(szText);
		QString szBuffer;
		getWindowListTipText(szBuffer);
		KviTalToolTip::add(m_pLabel, szBuffer);
	}
}

void KviQueryWindow::getWindowListTipText(QString & szBuffer)
{
	QString szText;
	if(!connection())
	{
		szBuffer = "<html><body><table style=\"white-space: pre\">" START_TABLE_BOLD_ROW;
		szBuffer += __tr2qs("Dead query");
		szBuffer += END_TABLE_BOLD_ROW "</table></body></html>";
		return;
	}

	KviIrcUserEntry * pEntry = connection()->userDataBase()->find(m_szName);
	if(pEntry)
	{
		QString buffer;
		console()->getUserTipText(m_szName, pEntry, szBuffer);
	}
	else
		szBuffer = QString(__tr2qs("Nothing is known about %1")).arg(m_szName);
}

QString KviQueryWindow::getInfoLabelText()
{
	QString szTmp;
	if(KVI_OPTION_BOOL(KviOption_boolShowExtendedInfoInQueryLabel) && connection())
	{
		KviIrcUserEntry * pEntry = connection()->userDataBase()->find(m_szName);
		if(pEntry)
		{
			QString szMask;
			QString szChans;
			if(connection())
			{
				if(pEntry->hasUser())
					szMask += pEntry->user();
				else
					szMask += "*";

				szMask += "@";
				if(pEntry->hasHost())
					szMask += pEntry->host();
				else
					szMask += "*";

				if(pEntry->hasRealName())
					szTmp = __tr2qs("Query with: %1!%2 (%3)").arg(m_szName, szMask, KviControlCodes::stripControlBytes(pEntry->realName()));
				else
					szTmp = __tr2qs("Query with: %1!%2").arg(m_szName, szMask);

				if(pEntry->hasServer())
					szTmp += __tr2qs(", using server: %1").arg(pEntry->server());

				if(pEntry->hasHops())
					szTmp += __tr2qs(", hops: %1").arg(pEntry->hops());

				if(pEntry->hasAccountName())
					szTmp += __tr2qs(", account: %1").arg(pEntry->accountName());

				if(pEntry->isAway())
					szTmp += __tr2qs(", probably away");

				szTmp += "\n";

				if(connection()->getCommonChannels(m_szName, szChans, false))
					szTmp += __tr2qs("Common channels: %2").arg(szChans);
				else
					szTmp += __tr2qs("No common channels");
			}
			else
			{
				szTmp = __tr2qs("[Dead query]");
			}
		}
		else
			szTmp += "\n";
	}
	return szTmp;
}

void KviQueryWindow::slotDndEvents(const QString & szFile)
{
	KVS_TRIGGER_EVENT_1(KviEvent_OnQueryFileDropped, this, szFile);
}

void KviQueryWindow::triggerCreationEvents()
{
	KVS_TRIGGER_EVENT_0(KviEvent_OnQueryWindowCreated, this);
}

void KviQueryWindow::getBaseLogFileName(QString & szBuffer)
{
	if(connection())
	{
		szBuffer = windowName();
		szBuffer += ".";
		szBuffer += console()->currentNetworkName();
	}
	else
	{
		szBuffer = windowName();
		szBuffer += ".";
		if(context())
			szBuffer += console()->context()->id();
		else
			szBuffer += "0";
	}
}

void KviQueryWindow::mergeQuery(KviQueryWindow * pQuery)
{
	m_pIrcView->appendMessagesFrom(pQuery->m_pIrcView);
	updateLabelText();
}

void KviQueryWindow::textViewRightClicked()
{
	KVS_TRIGGER_EVENT_0(KviEvent_OnQueryPopupRequest, this);
}

void KviQueryWindow::saveProperties(KviConfigurationFile * pCfg)
{
	KviWindow::saveProperties(pCfg);
	pCfg->writeEntry("Splitter", m_pUserListView->isHidden() ? m_SplitterSizesList : m_pSplitter->sizes());
	pCfg->writeEntry("UserListViewVisible", m_pUserListView->isVisible());
}

void KviQueryWindow::loadProperties(KviConfigurationFile * pCfg)
{
	int iWidth = width();
	KviWindow::loadProperties(pCfg);
	QList<int> def;
	def.append((iWidth * 75) / 100);
	def.append((iWidth * 25) / 100);
	m_SplitterSizesList = pCfg->readIntListEntry("Splitter", def);
	m_pSplitter->setStretchFactor(0, 1);

	showListView(pCfg->readBoolEntry("UserListViewVisible", false), true);
}

void KviQueryWindow::notifyTargetChange(const QString & szOldNick, const QString & szOldUser, const QString & szOldHost, const QString & szNick, const QString & szUser, const QString & szHost)
{
	QString szOldN = szOldNick.isEmpty() ? QString("*") : szOldNick;
	QString szOldU = szOldUser.isEmpty() ? QString("*") : szOldUser;
	QString szOldH = szOldHost.isEmpty() ? QString("*") : szOldHost;

	output(KVI_OUT_QUERYTRACE,
	    __tr2qs("The target of this query has changed from \r!n\r%Q\r [%Q@\r!h\r%Q\r] to \r!n\r%Q\r [%Q@\r!h\r%Q\r]"),
	    &szOldN, &szOldU, &szOldH, &szNick, &szUser, &szHost);

	updateLabelText();
}

void KviQueryWindow::userAction(const QString & szNick, const QString & szUser, const QString & szHost, unsigned int uActionType)
{
	int iTemperature = kvi_getUserActionTemperature(uActionType);
	if(KVI_OPTION_BOOL(KviOption_boolEnableQueryTracing))
	{
		QString szOldUser, szOldHost;
		if(!m_pUserListView->userActionVerifyMask(szNick, szUser, szHost, iTemperature, szOldUser, szOldHost))
			notifyTargetChange(szNick, szOldUser, szOldHost, szNick, szUser, szHost);
	}
	else
	{
		m_pUserListView->userAction(szNick, szUser, szHost, iTemperature);
	}

	updateLabelText();
}

void KviQueryWindow::userAction(const QString & szNick, unsigned int uActionType)
{
	int iTemperature = kvi_getUserActionTemperature(uActionType);
	m_pUserListView->userAction(szNick, iTemperature);
	updateLabelText();
}

void KviQueryWindow::userAction(KviIrcMask * pUser, unsigned int uActionType)
{
	int iTemperature = kvi_getUserActionTemperature(uActionType);
	if(KVI_OPTION_BOOL(KviOption_boolEnableQueryTracing))
	{
		QString szOldUser, szOldHost;
		if(!m_pUserListView->userActionVerifyMask(pUser->nick(), pUser->user(), pUser->host(), iTemperature, szOldUser, szOldHost))
			notifyTargetChange(pUser->nick(), szOldUser, szOldHost, pUser->nick(), pUser->user(), pUser->host());
	}
	else
	{
		m_pUserListView->userAction(pUser, iTemperature);
	}
	updateLabelText();
}

KviUserListEntry * KviQueryWindow::setTarget(const QString & szNick, const QString & szUser, const QString & szHost)
{
	KviUserListEntry * pEntry = m_pUserListView->join(szNick, szUser, szHost);

	if((!pEntry->globalData()->avatar()) && (!szUser.isEmpty()) && (szUser != "*"))
		m_pConsole->checkDefaultAvatar(pEntry->globalData(), szNick, szUser, szHost);

	setWindowName(szNick);
	updateCaption();

	if(KVI_OPTION_BOOL(KviOption_boolEnableQueryTracing))
	{
		QString szChans;
		int iChans = connection()->getCommonChannels(szNick, szChans);
		notifyCommonChannels(szNick, szUser, szHost, iChans, szChans);
	}

	KVS_TRIGGER_EVENT_3(KviEvent_OnQueryTargetAdded, this, szNick, szUser, szHost);
	updateLabelText();

	return pEntry;
}

void KviQueryWindow::notifyCommonChannels(const QString & szNick, const QString & szUser, const QString & szHost, int iChans, const QString & szChans)
{
	static QString szStar("*");
	if(iChans > 0)
	{
		output(KVI_OUT_QUERYTRACE, __tr2qs("Common channels for \r!n\r%Q\r [%Q@\r!h\r%Q\r]: %Q"),
		    &szNick, szUser.isEmpty() ? &szStar : &szUser, szHost.isEmpty() ? &szStar : &szHost, &szChans);
	}
	else
	{
		output(KVI_OUT_QUERYTRACE, __tr2qs("No common channels for \r!n\r%Q\r [%Q@\r!h\r%Q\r]"),
		    &szNick, szUser.isEmpty() ? &szStar : &szUser, szHost.isEmpty() ? &szStar : &szHost);
	}

	updateLabelText();
}

void KviQueryWindow::fillCaptionBuffers()
{
	if(!connection())
	{
		QString szDead = __tr2qs("[Dead query]");

		m_szPlainTextCaption = windowName();
		m_szPlainTextCaption += " : ";
		m_szPlainTextCaption += szDead;

		return;
	}

	QString szNickOnServer = QChar('[');
	szNickOnServer += connection()->currentNickName();
	szNickOnServer += __tr2qs(" on ");
	szNickOnServer += connection()->currentServerName();
	szNickOnServer += QChar(']');

	m_szPlainTextCaption = windowName();
	m_szPlainTextCaption += QChar(' ');
	m_szPlainTextCaption += szNickOnServer;
}

bool KviQueryWindow::nickChange(const QString & szOldNick, const QString & szNewNick)
{
	bool bRet = m_pUserListView->nickChange(szOldNick, szNewNick);
	if(!bRet)
		return false; // ugh!! ?

	setWindowName(szNewNick);
	updateCaption();
	updateLabelText();
	return true;
}

void KviQueryWindow::showListView(bool bShow, bool bIgnoreSizeChange)
{
	if(bShow)
	{
		m_pUserListView->show();
		if(!(m_pListViewButton->isChecked()))
			m_pListViewButton->setChecked(true);

		m_pSplitter->setSizes(m_SplitterSizesList);
	}
	else
	{
		if(!bIgnoreSizeChange)
			m_SplitterSizesList = m_pSplitter->sizes();

		m_pUserListView->hide();
		if(m_pListViewButton->isChecked())
			m_pListViewButton->setChecked(false);
	}
}

void KviQueryWindow::toggleListView()
{
	showListView(!m_pUserListView->isVisible());
}

void KviQueryWindow::setDeadQuery()
{
	m_iFlags |= Dead;

	m_pUserListView->enableUpdates(false);
	m_pUserListView->partAll();
	m_pUserListView->enableUpdates(true);
	m_pUserListView->setUserDataBase(nullptr);
	if(connection())
		connection()->unregisterQuery(this);
	if(context())
		context()->registerDeadQuery(this);
	setType(KviWindow::DeadQuery);

	updateIcon();
	updateCaption();
	updateLabelText();
};

void KviQueryWindow::setAliveQuery()
{
	m_iFlags &= ~Dead;

	KviIrcUserDataBase * pUserDataBase = connection()->userDataBase();

	m_pUserListView->setUserDataBase(pUserDataBase);

	setType(KviWindow::Query);

	if(context())
		context()->unregisterDeadQuery(this);
	if(connection())
		connection()->registerQuery(this);

	QString szNick = target();

	if(!szNick.isEmpty())
	{
		QString szUser;
		QString szHost;

		if(pUserDataBase)
		{
			KviIrcUserEntry * pEntry = pUserDataBase->find(szNick);
			if(pEntry)
			{
				szUser = pEntry->user();
				szHost = pEntry->host();
			}
		}

		setTarget(szNick, szUser, szHost);
	}

	// Update log file name
	if(m_pIrcView->isLogging())
		m_pIrcView->startLogging();

	updateIcon();
}

void KviQueryWindow::applyOptions()
{
	m_pUserListView->applyOptions();
	m_pLabel->applyOptions();
	updateLabelText();
	// this applies options for IrcView and Input and forces the window to relayout
	KviWindow::applyOptions();
}

QPixmap * KviQueryWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(isDeadQuery() ? KviIconManager::DeadQuery : KviIconManager::Query);
}

void KviQueryWindow::resizeEvent(QResizeEvent *)
{
	int iHeight = m_pInput->heightHint();
	int iHeight2 = m_pButtonBox->sizeHint().height();
	m_pButtonBox->setGeometry(0, 0, width(), iHeight2);
	m_pSplitter->setGeometry(0, iHeight2, width(), height() - (iHeight + iHeight2));
	m_pInput->setGeometry(0, height() - iHeight, width(), iHeight);
}

QSize KviQueryWindow::sizeHint() const
{
	QSize ret(m_pSplitter->sizeHint().width(), m_pIrcView->sizeHint().height() + m_pInput->heightHint());
	return ret;
}

void KviQueryWindow::ownMessage(const QString & szBuffer, bool bUserFeedback)
{
	if(!connection())
	{
		outputNoFmt(KVI_OUT_SYSTEMWARNING, __tr2qs("This query has no active targets, no message sent"));
		return;
	}

	//my full mask as seen by other users
	QString szTmpMask = connection()->userInfo()->nickName() + "!" + connection()->userInfo()->userName() + "@" + connection()->userInfo()->hostName();
	QByteArray szMyFullMask = connection()->encodeText(szTmpMask); //target name
	QByteArray szName = connection()->encodeText(windowName());    //message
	QByteArray szData = encodeText(szBuffer);
	const char * pcData = szData.data();

	/* max length of a PRIVMSG text. Max buffer length for our sszEnd is 512 byte, but we have to
	* remember that the server will prepszEnd to the message our full mask and truncate the resulting
	* at 512 bytes again..
	* So we have:
	* :NickName!~ident@hostname.tld PRIVMSG #channel :text of message(CrLf)
	* NickName!~ident@hostname.tld#channeltext of message
	* 512(rfc) -2(CrLf) -2(:) -3(spaces) -7(PRIVMSG) = 498
	* usable bytes, excluding our full mask and the target name.
	*/
	int iMaxMsgLen = 498 - szName.length() - szMyFullMask.length();

	// our copy of the message
	QString szTmpBuffer(szBuffer);

	if(!pcData)
		return;

#ifdef COMPILE_CRYPT_SUPPORT
	if(cryptSessionInfo())
	{
		if(cryptSessionInfo()->m_bDoEncrypt)
		{
			if(*pcData != KviControlCodes::CryptEscape)
			{
				KviCString szEncrypted;
				cryptSessionInfo()->m_pEngine->setMaxEncryptLen(500 - szName.length());
				switch(cryptSessionInfo()->m_pEngine->encrypt(pcData, szEncrypted))
				{
					case KviCryptEngine::Encrypted:
						if(!connection()->sendFmtData("PRIVMSG %s :%s", szName.data(), szEncrypted.ptr()))
							return;
						if(bUserFeedback)
							m_pConsole->outputPrivmsg(this, KVI_OUT_OWNPRIVMSGCRYPTED,
							    QString(), QString(), QString(), szBuffer, KviConsoleWindow::NoNotifications);
						break;
					case KviCryptEngine::Encoded:
					{
						if(!connection()->sendFmtData("PRIVMSG %s :%s", szName.data(), szEncrypted.ptr()))
							return;
						if(bUserFeedback)
						{
							// ugly,but we must redecode here
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
						    __tr2qs("The encryption engine was unable to encrypt the current message (%Q): %s, no data sent to the server"),
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
				pcData++;
				szTmpBuffer.remove(0, 1);
				//let the normal function do it
			}
		}
	}
#endif

	if(szData.length() > iMaxMsgLen)
	{
		/* Multi message; we want to split the message, preferably on a word boundary,
		 * and sszEnd each message part as a different PRIVMSG
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
				fPosDiff = (float)iMaxMsgLen / (float)szTmp.length();
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

			//sszEnd the string to the server
			if(connection()->sendFmtData("PRIVMSG %s :%s", szName.data(), szTmp.data()))
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
		if(connection()->sendFmtData("PRIVMSG %s :%s", szName.data(), pcData))
		{
			if(bUserFeedback)
				m_pConsole->outputPrivmsg(this, KVI_OUT_OWNPRIVMSG, QString(), QString(), QString(), szTmpBuffer, KviConsoleWindow::NoNotifications);
			userAction(connection()->currentNickName(), KVI_USERACTION_PRIVMSG);
		}
	}
}

void KviQueryWindow::ownAction(const QString & szBuffer)
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
	QByteArray myFullMask = connection()->encodeText(szMyFullMask);
	QByteArray name = connection()->encodeText(windowName());
	QByteArray data = encodeText(szTmpBuffer);
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

					output(KVI_OUT_OWNACTIONCRYPTED, "\r!nc\r%Q\r %Q", &szMyName, &szTmpBuffer);
				}
				break;
				case KviCryptEngine::Encoded:
				{
					if(!connection()->sendFmtData("PRIVMSG %s :%cACTION %s%c", name.data(), 0x01, szEncrypted.ptr(), 0x01))
						return;

					// ugly, but we must redecode here
					QString szRedecoded = decodeText(szEncrypted.ptr());

					output(KVI_OUT_OWNACTIONCRYPTED, "\r!nc\r%Q\r %Q", &szMyName, &szRedecoded);
				}
				break;
				default: // also case KviCryptEngine::EncryptError
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
#endif //COMPILE_CRYPT_SUPPORT

	if(!connection()->sendFmtData("PRIVMSG %s :%cACTION %s%c", name.data(), 0x01, data.data(), 0x01))
		return;

	output(KVI_OUT_OWNACTION, "\r!nc\r%Q\r %Q", &szMyName, &szTmpBuffer);
	m_pUserListView->userAction(szMyName, KVI_USERACTION_ACTION);
}
