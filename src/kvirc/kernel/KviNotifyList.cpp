//=============================================================================
//
//   File : KviNotifyList.cpp
//   Creation date : Fri Oct 27 2000 23:41:01 CEST by Szymon Stefanek
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

#include "kvi_debug.h"
#include "KviNotifyList.h"
#include "KviConsoleWindow.h"
#include "KviIrcSocket.h"
#include "KviRegisteredUserDataBase.h"
#include "KviUserListView.h"
#include "KviChannelWindow.h"
#include "KviOptions.h"
#include "KviWindow.h"
#include "KviLocale.h"
#include "kvi_out.h"
#include "KviIrcServerParser.h"
#include "KviIrcMask.h"
#include "KviIrcNumericCodes.h"
#include "KviIrcConnection.h"
#include "KviApplication.h"
#include "KviQString.h"
#include "KviLagMeter.h"
#include "KviKvsEventTriggers.h"
#include "KviIrcMessage.h"

#include <QByteArray>
#include <QStringList>

#include <algorithm>
#include <memory>
#include <set>
#include <vector>

// FIXME: #warning "Finish this doc!"

/*
	@doc: notify_list
	@title:
		Notify lists
	@short:
		Tracking users on IRC
	@keyterms:
		notify property, watch property, notify lists
	@body:
		The notify list is a means of keeping track of users on IRC.[br]
		Once connected to an IRC server, you can tell KVIrc to check
		periodically if your friends are online.[br]
		This is basically achieved by setting a property in the [doc:registered_users]registered users database[/doc]
		entry.[br]
		The property is called [i]notify[/i], and you have to set it to the nickname
		that you want to look for.[br]
		So for example, assume to register a friend of yours like Szymon:
		[example]
			[cmd:reguser.add]reguser.add[/cmd] Szymon
			[cmd:reguser.addmask]reguser.addmask[/cmd] Szymon Pragma!*@*.it
		[/example]
		And then want it in the notify list; nothing easier, just set
		hist [i]notify[/i] property to the nickname that you want him to be [i]looked for[/i]:
		[example]
			[cmd:reguser.setproperty]reguser.setproperty[/cmd] Szymon notify Pragma
		[/example]
		In this way, once in a while, KVIrc will send to the server an ISON message
		with the nickname Pragma. If Szymon is online, you will be notified with a message:[br]
		[i]Pragma [someuser@somehost.it] is on IRC[/i].[br]
		If Szymon uses often [i][Pragma][/i] as his secondary nickname, you can do the following:
		[example]
			[cmd:reguser.addmask]reguser.addmask[/cmd] Szymon [Pragma]*@*.it
			[cmd:reguser.setproperty]reguser.setproperty[/cmd] Szymon notify [i]Pragma [Pragma][/i]
		[/example]
		KVIrc will then look for both nicknames getting online.[br]
		KVIrc supports three notify lists management methods:[br]
		The [i]stupid ISON method[/i], the [i]intelligent ISON method[/i] and the [i]WATCH method[/i].[br]
		The [i]stupid ISON method[/i] will assume that Szymon is online if any user with nickname
		Pragma (or [Pragma] in the second example) gets online; this means that also Pragma!someuser@somehost.com will be
		assumed to be [i]Szymon[/i] and will be shown in the notify list.[br]
		This might be a false assumption (since somehost.com does not even match *.it),
		but it is the best result that the [i]stupid ISON method[/i] can achieve.[br]
		The [i]intelligent ISON method[/i] will also check the Pragma's username and hostname
		and match it in the registered masks; so in the example above, you will be notified if
		any user that matches Pragma!*@*.it gets online; (but you will [b]not[/b] be notified if
		(for example) Pragma!someuser@somehost.com gets online).[br]
		So what's the point in including a stupid method? :) Well... the intelligent
		method [i]eats[/i] some of your IRC bandwidth; it has to send USERHOST messages
		for every group of 5 users in the notify list. If you have a lot of users
		in the notify list, it might become slow and eventually cause a
		client to server flood.[br]
		So finally, the intelligent method is the default. If you have [i]flood[/i] problems,
		or if you think that the notify list is quite slow, try the [i]stupid[/i] method:
		it is not that bad after all.[br]
		The third notify list management method is the [i]WATCH method[/i].[br]
		It uses a totally different (and better) approach to the notify lists management,
		and can be used only on the networks that support the WATCH notify method (DALnet, WebNet, etc.).[br]
		KVIrc will attempt to guess if the server you're currently using supports the WATCH command
		and eventually use this last method.[br]
		The WATCH method uses the [i]notify[/i] property to get the nicknames that have to be
		sent to the server in the /WATCH commands.
*/

// Basic NotifyListManager: this does completely nothing

KviNotifyListManager::KviNotifyListManager(KviIrcConnection * pConnection)
    : QObject()
{
	setObjectName("notify_list_manager");
	m_pConnection = pConnection;
	m_pConsole = pConnection->console();
}

KviNotifyListManager::~KviNotifyListManager()
    = default;

void KviNotifyListManager::start()
{
}

void KviNotifyListManager::stop()
{
}

bool KviNotifyListManager::handleUserhost(KviIrcMessage *)
{
	return false;
}

bool KviNotifyListManager::handleIsOn(KviIrcMessage *)
{
	return false;
}

bool KviNotifyListManager::handleWatchReply(KviIrcMessage *)
{
	return false;
}

void KviNotifyListManager::notifyOnLine(const QString & szNick, const QString & szUser, const QString & szHost, const QString & szReason, bool bJoin)
{
	if(bJoin)
		m_pConsole->notifyListView()->join(szNick, szUser, szHost);

	KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolNotifyListChangesToActiveWindow) ? m_pConsole->activeWindow() : m_pConsole;
	if(KVS_TRIGGER_EVENT_1_HALTED(KviEvent_OnNotifyOnLine, pOut, szNick))
		return;

	QString szWho;
	QString szMsg;

	if(!(szUser.isEmpty() || szHost.isEmpty()))
		szWho = QString("\r!n\r%1\r [%2@\r!h\r%3\r]").arg(szNick, szUser, szHost);
	else
		szWho = QString("\r!n\r%1\r").arg(szNick);

	KviPointerHashTable<QString, KviRegisteredUser> * d = g_pRegisteredUserDataBase->userDict();
	KviPointerHashTableIterator<QString, KviRegisteredUser> it(*d);

	while(KviRegisteredUser * pUser = it.current())
	{
		QString szProp = pUser->getProperty("notify");
		if(!szProp.isEmpty() && szProp.split(',', QString::SkipEmptyParts).contains(szNick))
		{
			QString szComment = pUser->getProperty("comment");
			if(!szComment.isEmpty())
				szMsg = QString("%1 (%2), Group \"%3\" is on IRC as (%4)").arg(pUser->name(), szComment, pUser->group(), szWho);
			else
				szMsg = QString("%1, Group \"%2\" is on IRC as (%3)").arg(pUser->name(), pUser->group(), szWho);
			break;
		}
		++it;
	}

	QString szFmt = __tr2qs("%1 is on IRC");

	if(szMsg.isEmpty())
		szMsg = QString(szFmt).arg(szWho);

	if((!szReason.isEmpty()) && (_OUTPUT_VERBOSE))
	{
		szMsg += '(';
		szMsg += szReason;
		szMsg += ')';
	}

	pOut->outputNoFmt(KVI_OUT_NOTIFYONLINE, szMsg);

	if(!(pOut->hasAttention(KviWindow::MainWindowIsVisible)))
	{
		if(KVI_OPTION_BOOL(KviOption_boolFlashWindowOnNotifyOnLine))
			pOut->demandAttention();
		if(KVI_OPTION_BOOL(KviOption_boolPopupNotifierOnNotifyOnLine))
		{
			szWho = "<b>";
			szWho += szNick;
			szWho += "</b>";
			szMsg = QString(szFmt).arg(szWho);
			g_pApp->notifierMessage(pOut, KVI_OPTION_MSGTYPE(KVI_OUT_NOTIFYONLINE).pixId(), szMsg, KVI_OPTION_UINT(KviOption_uintNotifierAutoHideTime));
		}
	}
}

void KviNotifyListManager::notifyOffLine(const QString & szNick, const QString & szUser, const QString & szHost, const QString & szReason)
{
	KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolNotifyListChangesToActiveWindow) ? m_pConsole->activeWindow() : m_pConsole;
	if(!KVS_TRIGGER_EVENT_1_HALTED(KviEvent_OnNotifyOffLine, pOut, szNick))
	{
		QString szWho;

		if(!(szUser.isEmpty() || szHost.isEmpty()))
			szWho = QString("\r!n\r%1\r [%2@\r!h\r%3\r]").arg(szNick, szUser, szHost);
		else
			szWho = QString("\r!n\r%1\r").arg(szNick);

		QString szMsg;

		KviPointerHashTable<QString, KviRegisteredUser> * d = g_pRegisteredUserDataBase->userDict();
		KviPointerHashTableIterator<QString, KviRegisteredUser> it(*d);

		while(KviRegisteredUser * pUser = it.current())
		{
			QString szProp = pUser->getProperty("notify");
			if(!szProp.isEmpty() && szProp.split(',', QString::SkipEmptyParts).contains(szNick))
			{
				QString szComment = pUser->getProperty("comment");
				if(!szComment.isEmpty())
					szMsg = QString("%1 (%2), Group \"%3\" has left IRC as (%4)").arg(pUser->name(), szComment, pUser->group(), szWho);
				else
					szMsg = QString("%1, Group \"%2\" has left IRC as (%3)").arg(pUser->name(), pUser->group(), szWho);
				break;
			}
			++it;
		}

		if(szMsg.isEmpty())
			szMsg = QString(__tr2qs("%1 has left IRC")).arg(szWho);

		if((!szReason.isEmpty()) && (_OUTPUT_VERBOSE))
		{
			szMsg += '(';
			szMsg += szReason;
			szMsg += ')';
		}

		pOut->outputNoFmt(KVI_OUT_NOTIFYOFFLINE, szMsg);
	}

	m_pConsole->notifyListView()->part(szNick);
}

//
//  INTELLIGENT NOTIFY LIST MANAGER: NOTIFY PROCESS:
//
//            start()                              stop()
//               |                                   ^
//         buildRegUserDict()                        |
//               |                                   |
//     m_pRegUserDict.empty() ? -- YES ------------->+
//                       |                           |
//                      NO                           |
//                       |                           |
//         newNotifySession()<------- TIMER ---------------- delayedNotifySession() --------------------------------+
//                       |    (can be stopped here)  |              ^                                               |
//                       |                           |              ^                                               |
//                  buildNotifyList()                |              |                                              YES
//                       |                           |              |                                               |
//                    m_NotifyList.empty() ? - YES ->+              |                                               |
//                       |                                          |                                               |
//                      NO                                          |                                               |
//                       |                                          |                                               |
//                      newIsOnSession()<------------- TIMER -------------------- delayedIsOnSession() -- NO - m_NotifyList.empty() ?
//                               |           (can be stopped here)  |                                               |
//                               |                                  |                                               |
//                            buildIsOnList()                       |                                               |
//                               |                                  |                                               |
//                           m_IsOnList.empty() ? -- YES ---------->+                                               |
//                               |                                                                                  |
//                              NO                                                                                  |
//                               |                                                                                  |
//                            sendIsOn() - - - - - - - - - - - -> handleIsOn()                                      |
//                                                                      |                                           |
//                                                             (build m_OnlineList)                                 |
//                                                                      |                                           |
//                                                             m_OnlineList.empty() ? - YES ----------------------->+
//                                                                      |                                           |
//                                                                     NO                                          YES
//                                                                      |                                           |
//                                                             delayedUserhostSession()<--------------- NO - m_OnlineList.empty() ?
//                                                                               |                                  ^
//                                                                             TIMER (can be stopped here)          |
//                                                                               |                                  |
//                                                                           newUserhostSession()                   |
//                                                                               |                                  |
//                                                                           buildUserhostList()                    |
//                                                                               |                                  |
//                                                                         m_UserhostList.empty()    ?    - YES --->+
//                                                                               |                          ^^^     |
//                                                                               |             (unexpected!)|||     |
//                                                                               NO                                 |
//                                                                               |                                  |
//                                                                           sendUserhost() - - - - - - - - > handleUserhost()
//

KviIsOnNotifyListManager::KviIsOnNotifyListManager(KviIrcConnection * pConnection)
    : KviNotifyListManager(pConnection)
{
	connect(&m_pDelayedNotifyTimer, SIGNAL(timeout()), this, SLOT(newNotifySession()));
	connect(&m_pDelayedIsOnTimer, SIGNAL(timeout()), this, SLOT(newIsOnSession()));
	connect(&m_pDelayedUserhostTimer, SIGNAL(timeout()), this, SLOT(newUserhostSession()));
}

KviIsOnNotifyListManager::~KviIsOnNotifyListManager()
{
	if(m_bRunning)
		stop();
}

void KviIsOnNotifyListManager::start()
{
	if(m_bRunning)
		stop();
	m_bRunning = true;
	m_pConsole->notifyListView()->partAllButOne(m_pConnection->currentNickName());

	m_bExpectingIsOn = false;
	m_bExpectingUserhost = false;

	buildRegUserDict();
	if(m_pRegUserDict.empty())
	{
		if(_OUTPUT_VERBOSE)
			m_pConsole->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Notify list: No users to check for, quitting"));
		stop();
		return;
	}
	newNotifySession();
}

void KviIsOnNotifyListManager::buildRegUserDict()
{
	m_pRegUserDict.clear();

	const KviPointerHashTable<QString, KviRegisteredUser> * d = g_pRegisteredUserDataBase->userDict();
	KviPointerHashTableIterator<QString, KviRegisteredUser> it(*d);
	while(KviRegisteredUser * u = it.current())
	{
		QString notify;
		if(u->getProperty("notify", notify))
		{
			for(const auto & single : notify.trimmed().split(' ', QString::SkipEmptyParts))
				m_pRegUserDict.emplace(single, u->name());
		}
		++it;
	}
}

void KviIsOnNotifyListManager::delayedNotifySession()
{
	unsigned int iTimeout = KVI_OPTION_UINT(KviOption_uintNotifyListCheckTimeInSecs);
	if(iTimeout < 15)
	{
		// life first of all.
		// don't allow the user to suicide
		if(_OUTPUT_VERBOSE)
			m_pConsole->output(KVI_OUT_SYSTEMWARNING,
			    __tr2qs("Notify list: Timeout (%d sec) is too short, resetting to something more reasonable (15 sec)"),
			    iTimeout);
		iTimeout = 15;
		KVI_OPTION_UINT(KviOption_uintNotifyListCheckTimeInSecs) = 15;
	}
	m_pDelayedNotifyTimer.setInterval(iTimeout * 1000);
	m_pDelayedNotifyTimer.setSingleShot(true);
	m_pDelayedNotifyTimer.start();
}

void KviIsOnNotifyListManager::newNotifySession()
{
	buildNotifyList();
	if(m_NotifyList.empty())
	{
		if(_OUTPUT_VERBOSE)
			m_pConsole->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Notify list: Notify list empty, quitting"));
		stop();
		return;
	}
	newIsOnSession();
}

void KviIsOnNotifyListManager::buildNotifyList()
{
	m_NotifyList.clear();
	for(auto & it : m_pRegUserDict)
	{
		m_NotifyList.push_back(it.first);
	}
}

void KviIsOnNotifyListManager::delayedIsOnSession()
{
	unsigned int iTimeout = KVI_OPTION_UINT(KviOption_uintNotifyListIsOnDelayTimeInSecs);
	if(iTimeout < 5)
	{
		// life first of all.
		// don't allow the user to suicide
		if(_OUTPUT_VERBOSE)
			m_pConsole->output(KVI_OUT_SYSTEMWARNING,
			    __tr2qs("Notify list: ISON delay (%d sec) is too short, resetting to something more reasonable (5 sec)"),
			    iTimeout);
		iTimeout = 5;
		KVI_OPTION_UINT(KviOption_uintNotifyListIsOnDelayTimeInSecs) = 5;
	}
	m_pDelayedIsOnTimer.setInterval(iTimeout * 1000);
	m_pDelayedIsOnTimer.setSingleShot(true);
	m_pDelayedIsOnTimer.start();
}

void KviIsOnNotifyListManager::newIsOnSession()
{
	buildIsOnList();
	if(m_IsOnList.empty())
		delayedNotifySession();
	else
		sendIsOn();
}

void KviIsOnNotifyListManager::buildIsOnList()
{
	m_IsOnList.clear();
	m_szIsOnString = "";
	while(!m_NotifyList.empty())
	{
		const auto sIter = m_NotifyList.begin();
		const auto & s = *sIter;

		if(((m_szIsOnString.length() + s.length()) + 1) >= 504)
			break;

		if(!m_szIsOnString.isEmpty())
			m_szIsOnString.append(' ');
		m_szIsOnString.append(s);
		m_IsOnList.push_back(std::move(s));
		m_NotifyList.erase(sIter);
	}
}

void KviIsOnNotifyListManager::sendIsOn()
{
	if(_OUTPUT_PARANOIC)
		m_pConsole->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Notify list: Checking for: %Q"), &m_szIsOnString);
	QByteArray szDec = m_pConnection->encodeText(m_szIsOnString);
	m_pConnection->sendFmtData("ISON %s", szDec.data());
	if(m_pConnection->lagMeter())
		m_pConnection->lagMeter()->lagCheckRegister("@notify_ison", 40); // not that reliable
	m_szIsOnString = "";
	m_bExpectingIsOn = true;
	// FIXME: #warning "And if can't send ?"
}

bool KviIsOnNotifyListManager::handleIsOn(KviIrcMessage * msg)
{
	if(!m_bExpectingIsOn)
		return false;

	// Check if it is our ISON
	// all the nicks must be on the IsOnList

	std::set<std::size_t> tmplist;

	KviCString nk;
	const char * aux = msg->trailing();

	while(*aux)
	{
		nk = "";
		aux = kvi_extractToken(nk, aux, ' ');
		if(nk.hasData())
		{
			bool bGotIt = false;
			QString dnk = m_pConnection->decodeText(nk.ptr());

			std::size_t i = 0;
			for(const auto & s : m_IsOnList)
			{
				if(KviQString::equalCI(s, dnk))
				{
					tmplist.insert(i);
					bGotIt = true;
					break;
				}
				i++;
			}
			if(!bGotIt)
			{
				// oops... not my userhost!
				if(_OUTPUT_VERBOSE)
					m_pConsole->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Notify list: Hey! You've used ISON behind my back? (I might be confused now...)"));
				return false;
			}
		}
	}

	// Ok... looks to be my ison (still not sure at 100%, but can't do better)
	if(m_pConnection->lagMeter())
		m_pConnection->lagMeter()->lagCheckComplete("@notify_ison");

	m_bExpectingIsOn = false;

	m_OnlineList.clear();

	// Ok... we have an IsOn reply here
	// The nicks in the IsOnList that are also in the reply are online, and go to the OnlineList
	// the remaining in the IsOnList are offline
	for(auto i = tmplist.rbegin(); i != tmplist.rend(); ++i)
	{
		m_OnlineList.push_back(std::move(m_IsOnList[*i]));
		m_IsOnList.erase(m_IsOnList.begin() + *i);
	}

	// Ok... all the users that are online, are on the OnlineList
	// the remaining users are in the m_IsOnList, and are no longer online

	// first the easy step: remove the users that have just left irc or have never been online
	// we're clearling the m_IsOnList
	for(const auto & s : m_IsOnList)
	{
		// has just left IRC... make him part
		if(m_pConsole->notifyListView()->findEntry(s))
			notifyOffLine(s);
	}

	m_IsOnList.clear();

	// ok... complex step now: the remaining users in the userhost list are online
	// if they have been online before, just remove them from the list
	// otherwise they must be matched for masks
	// and eventually inserted in the notify view later

	KviIrcUserDataBase * db = console()->connection()->userDataBase();

	std::set<std::size_t> l;
	std::size_t i = 0;
	for(const auto & ss : m_OnlineList)
	{
		if(KviUserListEntry * ent = m_pConsole->notifyListView()->findEntry(ss))
		{
			// the user was online from a previous notify session
			// might the mask have been changed ? (heh... this is tricky, maybe too much even)
			if(KVI_OPTION_BOOL(KviOption_boolNotifyListSendUserhostForOnlineUsers))
			{
				// user wants to be sure about online users....
				// check if he is on some channels
				if(ent->globalData()->nRefs() > 1)
				{
					// mmmh... we have more than one ref, so the user is at least in one query or channel
					// look him up on channels, if we find his entry, we can be sure that he is
					// still the right user
					std::vector<KviChannelWindow *> chlist = m_pConsole->connection()->channelList();
					for(auto ch : chlist)
					{
						if(KviUserListEntry * le = ch->findEntry(ss))
						{
							l.insert(i); // ok... found on a channel... we don't need a userhost to match him
							KviIrcMask mk(ss, le->globalData()->user(), le->globalData()->host());
							if(!doMatchUser(ss, mk))
								return true; // critical problems = have to restart!!!
							break;
						}
					}
				} // else Only one ref... we need a userhost to be sure (don't remove from the list)
			}
			else
			{
				// user wants no userhost for online users... we "hope" that everything will go ok.
				l.insert(i);
			}
			//l.insert(i); // we will remove him from the list
		}
		else
		{
			// the user was not online!
			// check if we have a cached mask
			if(db)
			{
				if(KviIrcUserEntry * ue = db->find(ss))
				{
					// already in the db... do we have a mask ?
					if(ue->hasUser() && ue->hasHost())
					{
						// yup! we have a complete mask to match on
						KviIrcMask mk(ss, ue->user(), ue->host());
						// lookup the user's name in the m_pRegUserDict
						if(!doMatchUser(ss, mk))
							return true; // critical problems = have to restart!!!
						l.insert(i);     // remove anyway
					}
				}
			}
		}

		i++;
	}

	for(auto i = l.rbegin(); i != l.rend(); ++i)
	{
		m_OnlineList.erase(m_OnlineList.begin() + *i);
	}

	if(m_OnlineList.empty())
	{
		if(m_NotifyList.empty())
			delayedNotifySession();
		else
			delayedIsOnSession();
	}
	else
		delayedUserhostSession();

	return true;
}

// FIXME: #warning "Nickname escapes (links) in the notifylist messages!"

bool KviIsOnNotifyListManager::doMatchUser(const QString & notifyString, const KviIrcMask & mask)
{
	const auto i = m_pRegUserDict.find(notifyString);
	if(i != m_pRegUserDict.end())
	{
		const QString & nam = i->second;
		// ok... find the user
		if(KviRegisteredUser * u = g_pRegisteredUserDataBase->findUserByName(nam))
		{
			// ok... match the user
			if(u->matchesFixed(mask))
			{
				// new user online
				if(!(m_pConsole->notifyListView()->findEntry(mask.nick())))
				{
					notifyOnLine(mask.nick(), mask.user(), mask.host());
				} // else already online, and matching... all ok
			}
			else
			{
				// not matched.... has he been online before ?
				if(m_pConsole->notifyListView()->findEntry(mask.nick()))
				{
					// has been online just a sec ago, but now the mask does not match
					// either reguserdb has changed, or the user went offline and another one got his nick
					// in the meantime... (ugly situation anyway)
					notifyOffLine(mask.nick(), mask.user(), mask.host(), __tr2qs("registration mask changed, or nickname is being used by someone else"));
				}
				else
				{
					// has never been online
					if(_OUTPUT_VERBOSE)
						m_pConsole->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Notify list: \r!n\r%Q\r appears to be online, but the mask [%Q@\r!h\r%Q\r] does not match (registration mask does not match, or nickname is being used by someone else)"), &(mask.nick()), &(mask.user()), &(mask.host()));
				}
			}
		}
		else
		{
			// oops... unexpected inconsistency.... reguser db modified ?
			m_pConsole->output(KVI_OUT_SYSTEMWARNING, __tr2qs("Notify list: Unexpected inconsistency, registered user DB modified? (restarting)"));
			stop();
			start();
			return false; // critical... exit from the call stack
		}
	}
	else
	{
		// oops... unexpected inconsistency
		m_pConsole->output(KVI_OUT_SYSTEMWARNING, __tr2qs("Notify list: Unexpected inconsistency, expected \r!n\r%Q\r in the registered user DB"), &notifyString);
	}
	return true;
}

void KviIsOnNotifyListManager::delayedUserhostSession()
{
	unsigned int iTimeout = KVI_OPTION_UINT(KviOption_uintNotifyListUserhostDelayTimeInSecs);
	if(iTimeout < 5)
	{
		// life first of all.
		// don't allow the user to suicide
		if(_OUTPUT_VERBOSE)
			m_pConsole->output(KVI_OUT_SYSTEMWARNING,
			    __tr2qs("Notify list: USERHOST delay (%d sec) is too short, resetting to something more reasonable (5 sec)"),
			    iTimeout);
		iTimeout = 5;
		KVI_OPTION_UINT(KviOption_uintNotifyListUserhostDelayTimeInSecs) = 5;
	}
	m_pDelayedUserhostTimer.setInterval(iTimeout * 1000);
	m_pDelayedUserhostTimer.setSingleShot(true);
	m_pDelayedUserhostTimer.start();
}

void KviIsOnNotifyListManager::newUserhostSession()
{
	buildUserhostList();
	if(m_UserhostList.empty())
	{
		// this is unexpected!
		m_pConsole->output(KVI_OUT_SYSTEMWARNING, __tr2qs("Notify list: Unexpected inconsistency, userhost list is empty!"));
		if(m_OnlineList.empty())
		{
			if(m_NotifyList.empty())
				delayedNotifySession();
			else
				delayedIsOnSession();
		}
		else
			delayedUserhostSession();
		return;
	}
	sendUserhost();
}

#define MAX_USERHOST_ENTRIES 5

void KviIsOnNotifyListManager::buildUserhostList()
{
	m_szUserhostString = "";
	m_UserhostList.clear();
	for(std::size_t i = 0; i < MAX_USERHOST_ENTRIES && !m_OnlineList.empty(); ++i)
	{
		const auto sIter = m_OnlineList.begin();
		const QString & s = *sIter;

		if(!m_szUserhostString.isEmpty())
			m_szUserhostString.append(' ');
		m_szUserhostString.append(s);
		m_UserhostList.push_back(std::move(s));
		m_OnlineList.erase(sIter);
	}
}

void KviIsOnNotifyListManager::sendUserhost()
{
	if(_OUTPUT_PARANOIC)
		m_pConsole->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Notify list: Checking userhost for: %Q"), &m_szUserhostString);
	QByteArray ccc = m_pConnection->encodeText(m_szUserhostString);
	m_pConnection->sendFmtData("USERHOST %s", ccc.data());
	if(m_pConnection->lagMeter())
		m_pConnection->lagMeter()->lagCheckRegister("@notify_userhost", 50);
	m_szUserhostString = "";
	m_bExpectingUserhost = true;
	// FIXME: #warning "And if can't send ?"
}

bool KviIsOnNotifyListManager::handleUserhost(KviIrcMessage * msg)
{
	if(!m_bExpectingUserhost)
		return false;
	// first check for consistency: all the replies must be on the USERHOST list

	std::map<std::size_t, std::unique_ptr<KviIrcMask>> tmplist;

	KviCString nk;
	const char * aux = msg->trailing();

	while(*aux)
	{
		nk = "";
		aux = kvi_extractToken(nk, aux, ' ');
		if(nk.hasData())
		{
			// split it in a mask
			KviCString nick;
			KviCString user;
			KviCString host;

			int idx = nk.findFirstIdx('=');
			if(idx != -1)
			{
				nick = nk.left(idx);
				if(nick.lastCharIs('*'))
					nick.cutRight(1);
				nk.cutLeft(idx + 1);
				if(nk.firstCharIs('+') || nk.firstCharIs('-'))
					nk.cutLeft(1);

				idx = nk.findFirstIdx('@');
				if(idx != -1)
				{
					user = nk.left(idx);
					nk.cutLeft(idx + 1);
					host = nk;
				}
				else
				{
					user = "*";
					host = nk;
				}

				bool bGotIt = false;
				QString szNick = m_pConnection->decodeText(nick.ptr());
				QString szUser = m_pConnection->decodeText(user.ptr());
				QString szHost = m_pConnection->decodeText(host.ptr());

				std::size_t i = 0;
				for(const auto & s : m_UserhostList)
				{
					if(KviQString::equalCI(s, szNick))
					{
						tmplist.emplace(i, std::make_unique<KviIrcMask>(szNick, szUser, szHost));
						bGotIt = true;
						break;
					}
				}

				if(!bGotIt)
				{
					// oops... not my userhost!
					if(_OUTPUT_VERBOSE)
						m_pConsole->output(KVI_OUT_SYSTEMWARNING, __tr2qs("Notify list: Hey! You've used USERHOST behind my back? (I might be confused now...)"));
					return false;
				}
			}
			else
			{
				if(_OUTPUT_VERBOSE)
					m_pConsole->output(KVI_OUT_SYSTEMWARNING, __tr2qs("Notify list: Broken USERHOST reply from the server? (%s)"), nk.ptr());
			}
		}
	}

	// Ok... looks to be my usershot (still not sure at 100%, but can't do better)

	if(m_pConnection->lagMeter())
		m_pConnection->lagMeter()->lagCheckComplete("@notify_userhost");

	m_bExpectingUserhost = false;

	for(auto & pair : tmplist)
	{
		KviIrcMask * mk = pair.second.get();

		if(!doMatchUser(mk->nick(), *mk))
			return true; // have to restart!!!
	}

	for(auto i = tmplist.rbegin(); i != tmplist.rend(); ++i)
		m_UserhostList.erase(m_UserhostList.begin() + i->first);

	for(const auto & s : m_UserhostList)
	{
		// oops... someone is no longer online ?
		if(_OUTPUT_VERBOSE)
			m_pConsole->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Notify list: \r!n\r%Q\r appears to have gone offline before USERHOST reply was received, will recheck in the next loop"), &s);
	}

	m_UserhostList.clear();

	if(m_OnlineList.empty())
	{
		if(m_NotifyList.empty())
			delayedNotifySession();
		else
			delayedIsOnSession();
	}
	else
	{
		delayedUserhostSession();
	}

	return true;
}

void KviIsOnNotifyListManager::stop()
{
	if(!m_bRunning)
		return;

	if(m_pConnection->lagMeter())
		m_pConnection->lagMeter()->lagCheckAbort("@notify_userhost");
	if(m_pConnection->lagMeter())
		m_pConnection->lagMeter()->lagCheckAbort("@notify_ison");

	m_pDelayedNotifyTimer.stop();
	m_pDelayedIsOnTimer.stop();
	m_pDelayedUserhostTimer.stop();
	m_pConsole->notifyListView()->partAllButOne(m_pConnection->currentNickName());
	m_pRegUserDict.clear();
	m_NotifyList.clear();
	m_IsOnList.clear();
	m_OnlineList.clear();
	m_UserhostList.clear();
	m_szIsOnString = "";
	m_szUserhostString = "";
	m_bRunning = false;
}

//
// Stupid notify list manager
//

KviStupidNotifyListManager::KviStupidNotifyListManager(KviIrcConnection * pConnection)
    : KviNotifyListManager(pConnection)
{
	m_iRestartTimer = 0;
}

KviStupidNotifyListManager::~KviStupidNotifyListManager()
{
	if(m_iRestartTimer)
	{
		killTimer(m_iRestartTimer);
		m_iRestartTimer = 0;
	}
}

void KviStupidNotifyListManager::start()
{
	if(m_iRestartTimer)
	{
		killTimer(m_iRestartTimer);
		m_iRestartTimer = 0;
	}
	if(_OUTPUT_VERBOSE)
		m_pConsole->outputNoFmt(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Starting notify list"));
	buildNickList();
	if(m_pNickList.empty())
	{
		if(_OUTPUT_VERBOSE)
			m_pConsole->outputNoFmt(KVI_OUT_SYSTEMMESSAGE, __tr2qs("No users in the notify list"));
		return; // Ok... no nicknames in the list
	}
	m_iNextNickToCheck = 0;
	m_pConsole->notifyListView()->partAllButOne(m_pConnection->currentNickName());
	sendIsOn();
}

void KviStupidNotifyListManager::sendIsOn()
{
	m_szLastIsOnMsg = "";
	KVI_ASSERT(m_iNextNickToCheck < m_pNickList.size());

	std::size_t i = m_iNextNickToCheck;
	for(; i < m_pNickList.size(); ++i)
	{
		const auto nick = m_pNickList[i];
		if((nick.length() + 5 + m_szLastIsOnMsg.length()) >= 510)
			break;
		KviQString::appendFormatted(m_szLastIsOnMsg, " %Q", &nick);
	}
	if(_OUTPUT_PARANOIC)
		m_pConsole->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Notify list: Checking for: %Q"), &m_szLastIsOnMsg);
	QByteArray dat = m_pConnection->encodeText(m_szLastIsOnMsg);
	m_pConnection->sendFmtData("ISON%s", dat.data());

	if(m_pConnection->lagMeter())
		m_pConnection->lagMeter()->lagCheckRegister("@notify_naive", 20);

	m_iNextNickToCheck += i;
}

bool KviStupidNotifyListManager::handleIsOn(KviIrcMessage * msg)
{
	if(m_pConnection->lagMeter())
		m_pConnection->lagMeter()->lagCheckComplete("@notify_naive");

	KviCString nk;
	const char * aux = msg->trailing();
	while(*aux)
	{
		nk = "";
		aux = kvi_extractToken(nk, aux, ' ');
		if(nk.hasData())
		{
			QString nkd = m_pConnection->decodeText(nk.ptr());
			QString nksp = " " + nkd;
			m_szLastIsOnMsg.replace(nksp, "", Qt::CaseInsensitive);
			if(!(m_pConsole->notifyListView()->findEntry(nkd)))
			{
				// not yet notified
				notifyOnLine(nkd);
			}
		}
	}
	// ok... check the users that have left irc now...
	QStringList sl = m_szLastIsOnMsg.isEmpty() ? QStringList() : m_szLastIsOnMsg.split(' ', QString::SkipEmptyParts);

	for(auto & it : sl)
	{
		if(m_pConsole->notifyListView()->findEntry(it))
		{
			// has just left irc
			notifyOffLine(it);
		} // else has never been here...
	}

	if(((unsigned int)m_iNextNickToCheck) >= m_pNickList.size())
	{
		// have to restart
		unsigned int iTimeout = KVI_OPTION_UINT(KviOption_uintNotifyListCheckTimeInSecs);
		if(iTimeout < 5)
		{
			// life first of all.
			// don't allow the user to suicide
			if(_OUTPUT_VERBOSE)
				m_pConsole->output(KVI_OUT_SYSTEMWARNING,
				    __tr2qs("Notify list: Timeout (%d sec) is too short, resetting to something more reasonable (5 sec)"),
				    iTimeout);
			iTimeout = 5;
			KVI_OPTION_UINT(KviOption_uintNotifyListCheckTimeInSecs) = 5;
		}
		m_iRestartTimer = startTimer(iTimeout * 1000);
	}
	else
		sendIsOn();
	return true;
}

void KviStupidNotifyListManager::timerEvent(QTimerEvent * e)
{
	if(e->timerId() == m_iRestartTimer)
	{
		killTimer(m_iRestartTimer);
		m_iRestartTimer = 0;
		m_iNextNickToCheck = 0;
		sendIsOn();
		return;
	}
	QObject::timerEvent(e);
}

void KviStupidNotifyListManager::stop()
{
	if(m_pConnection->lagMeter())
		m_pConnection->lagMeter()->lagCheckAbort("@notify_naive");

	if(m_iRestartTimer)
	{
		killTimer(m_iRestartTimer);
		m_iRestartTimer = 0;
	}
	m_pConsole->notifyListView()->partAllButOne(m_pConnection->currentNickName());

	// The ISON Method needs no stopping
}

void KviStupidNotifyListManager::buildNickList()
{
	const KviPointerHashTable<QString, KviRegisteredUser> * d = g_pRegisteredUserDataBase->userDict();
	KviPointerHashTableIterator<QString, KviRegisteredUser> it(*d);
	m_pNickList.clear();
	while(it.current())
	{
		QString notify;
		if(it.current()->getProperty("notify", notify))
			m_pNickList.push_back(notify);
		++it;
	}
}

//
// Watch notify list manager
//

KviWatchNotifyListManager::KviWatchNotifyListManager(KviIrcConnection * pConnection)
    : KviNotifyListManager(pConnection)
{
}

void KviWatchNotifyListManager::buildRegUserDict()
{
	m_pRegUserDict.clear();

	const KviPointerHashTable<QString, KviRegisteredUser> * d = g_pRegisteredUserDataBase->userDict();
	KviPointerHashTableIterator<QString, KviRegisteredUser> it(*d);
	while(KviRegisteredUser * u = it.current())
	{
		QString notify;
		if(u->getProperty("notify", notify))
		{
			notify = notify.trimmed();
			QStringList sl = notify.split(' ', QString::SkipEmptyParts);
			for(auto & slit : sl)
				m_pRegUserDict.emplace(slit, u->name());
		}
		++it;
	}
}

void KviWatchNotifyListManager::start()
{
	m_pConsole->notifyListView()->partAllButOne(m_pConnection->currentNickName());

	buildRegUserDict();

	QString watchStr;

	for(auto & it : m_pRegUserDict)
	{
		const QString & nk = it.first;
		if(!nk.contains('*'))
		{
			if((watchStr.length() + nk.length() + 2) > 501)
			{
				QByteArray dat = m_pConnection->encodeText(watchStr);
				m_pConnection->sendFmtData("WATCH%s", dat.data());
				if(_OUTPUT_VERBOSE)
					m_pConsole->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Notify list: Adding watch entries for %Q"), &watchStr);
				watchStr = "";
			}
			KviQString::appendFormatted(watchStr, " +%Q", &nk);
		}
	}

	if(!watchStr.isEmpty())
	{
		QByteArray dat = m_pConnection->encodeText(watchStr);
		m_pConnection->sendFmtData("WATCH%s", dat.data());
		if(_OUTPUT_VERBOSE)
			m_pConsole->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Notify list: Adding watch entries for %Q"), &watchStr);
	}
}
void KviWatchNotifyListManager::stop()
{
	m_pConsole->notifyListView()->partAllButOne(m_pConnection->currentNickName());
	m_pConnection->sendFmtData("WATCH c");
	m_pRegUserDict.clear();
}

bool KviWatchNotifyListManager::doMatchUser(KviIrcMessage * msg, const QString & notifyString, const KviIrcMask & mask)
{
	const auto m = m_pRegUserDict.find(notifyString);
	if(m != m_pRegUserDict.end())
	{
		const QString & nam = m->second;
		// ok... find the user
		if(KviRegisteredUser * u = g_pRegisteredUserDataBase->findUserByName(nam))
		{
			// ok... match the user
			if(u->matchesFixed(mask))
			{
				// new user online
				if(!(m_pConsole->notifyListView()->findEntry(mask.nick())))
				{
					notifyOnLine(mask.nick(), mask.user(), mask.host(), "watch");
				}
				else
				{
					// else already online, and matching... all ok
					if(msg->numeric() == RPL_NOWON)
					{
						// This is a reply to a /watch +something (should not happen, unless the user is messing) or to /watch l (user requested)
						notifyOnLine(mask.nick(), mask.user(), mask.host(),
						    __tr2qs("watch entry listing requested by user"), false);
					}
					else
					{
						// This is a RPL_LOGON.... we're desynched ?
						notifyOnLine(mask.nick(), mask.user(), mask.host(),
						    __tr2qs("possible watch list desync"), false);
					}
				}
			}
			else
			{
				// not matched.... has he been online before ?
				if(m_pConsole->notifyListView()->findEntry(mask.nick()))
				{
					// has been online just a sec ago, but now the mask does not match
					// prolly the reguserdb has been changed
					notifyOffLine(mask.nick(), mask.user(), mask.host(),
					    __tr2qs("registration mask changed or desync with the watch service"));
				}
				else
				{
					// has never been online
					if(_OUTPUT_VERBOSE)
						m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,
						    __tr("Notify list: \r!n\r%Q\r appears to be online, but the mask [%Q@\r!h\r%Q\r] does not match (watch: registration mask does not match, or nickname is being used by someone else)"),
						    &(mask.nick()), &(mask.user()), &(mask.host()));
				}
			}
		}
		else
		{
			// oops... unexpected inconsistency.... reguser db modified ?
			m_pConsole->output(KVI_OUT_SYSTEMWARNING,
			    __tr2qs("Notify list: Unexpected inconsistency, registered user DB modified? (watch: restarting)"));
			stop();
			start();
			return false; // critical... exit from the call stack
		}
	}
	else
	{
		// not in our dictionary
		// prolly someone used /WATCH behind our back... bad boy!
		if(!(m_pConsole->notifyListView()->findEntry(mask.nick())))
		{
			notifyOnLine(mask.nick(), mask.user(), mask.host(), __tr2qs("watch entry added by user"));
		}
	}
	return true;
}

// FIXME: #warning "DEDICATED WATCH LIST VERBOSITY FLAG ? (To allow the user to use /WATCH l and manual /WATCH)"

bool KviWatchNotifyListManager::handleWatchReply(KviIrcMessage * msg)
{
	// 600: RPL_LOGON
	// :prefix 600 <target> <nick> <user> <host> <logintime> :logged online
	// 601: RPL_LOGOFF
	// :prefix 601 <target> <nick> <user> <host> <logintime> :logged offline
	// 604: RPL_NOWON
	// :prefix 604 <target> <nick> <user> <host> <logintime> :is online
	// 605: RPL_NOWOFF
	// :prefix 605 <target> <nick> <user> <host> 0 :is offline

	// FIXME: #warning "Use the logintime in some way ?"

	const char * nk = msg->safeParam(1);
	const char * us = msg->safeParam(2);
	const char * ho = msg->safeParam(3);
	QString dnk = m_pConnection->decodeText(nk);
	QString dus = m_pConnection->decodeText(us);
	QString dho = m_pConnection->decodeText(ho);

	if((msg->numeric() == RPL_LOGON) || (msg->numeric() == RPL_NOWON))
	{
		KviIrcMask m(dnk, dus, dho);
		doMatchUser(msg, dnk, m);
		return true;
	}
	else if(msg->numeric() == RPL_WATCHOFF)
	{
		if(m_pConsole->notifyListView()->findEntry(dnk))
		{
			notifyOffLine(dnk, dus, dho, __tr2qs("removed from watch list"));
		}
		else
		{
			if(_OUTPUT_VERBOSE)
				m_pConsole->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Notify list: Stopped watching for \r!n\r%Q\r"), &dnk);
		}
		if(m_pRegUserDict.count(dnk))
			m_pRegUserDict.erase(dnk); // kill that

		return true;
	}
	else if((msg->numeric() == RPL_LOGOFF) || (msg->numeric() == RPL_NOWOFF))
	{
		if(m_pConsole->notifyListView()->findEntry(dnk))
		{
			notifyOffLine(dnk, dus, dho, __tr2qs("watch"));
		}
		else
		{
			if(msg->numeric() == RPL_NOWOFF)
			{
				// This is a reply to a /watch +something
				if(_OUTPUT_VERBOSE)
					m_pConsole->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Notify list: \r!n\r%Q\r is offline (watch)"), &dnk);
			}
			else
			{
				// This is a RPL_LOGOFF for a user that has not matched the reg-mask
				notifyOffLine(dnk, dus, dho, __tr2qs("unmatched watch list entry"));
			}
		}
		return true;
	}

	return false;
}
